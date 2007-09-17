// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_

#include "CSkinnedMesh.h"
#include "CBoneSceneNode.h"
#include "IAnimatedMeshSceneNode.h"
#include "os.h"


namespace irr
{
namespace scene
{


//! constructor
CSkinnedMesh::CSkinnedMesh()
: SkinningBuffers(0), HasAnimation(0), PreparedForSkinning(0),
	AnimationFrames(0.f), lastAnimatedFrame(0.f), lastSkinnedFrame(0.f),
	BoneControlUsed(false), AnimateNormals(0), InterpolationMode(EIM_LINEAR)
{
	#ifdef _DEBUG
	setDebugName("CSkinnedMesh");
	#endif

	SkinningBuffers=&LocalBuffers;
}


//! destructor
CSkinnedMesh::~CSkinnedMesh()
{
	for (u32 i=0; i<AllJoints.size(); ++i)
		delete AllJoints[i];

	for (u32 j=0; j<LocalBuffers.size(); ++j)
	{
		if (LocalBuffers[j])
			LocalBuffers[j]->drop();
	}
}


//! returns the amount of frames in milliseconds.
//! If the amount is 1, it is a static (=non animated) mesh.
u32 CSkinnedMesh::getFrameCount() const
{
	return core::floor32(AnimationFrames);
}


//! returns the animated mesh based on a detail level. 0 is the lowest, 255 the highest detail. Note, that some Meshes will ignore the detail level.
IMesh* CSkinnedMesh::getMesh(s32 frame, s32 detailLevel, s32 startFrameLoop, s32 endFrameLoop)
{
	//animate(frame,startFrameLoop, endFrameLoop);
	if (frame==-1)
		return this;

	animateMesh((f32)frame, 1.0f);
	buildAll_LocalAnimatedMatrices();
	buildAll_GlobalAnimatedMatrices();
	skinMesh();
	return this;
}


//--------------------------------------------------------------------------
//			Keyframe Animation
//--------------------------------------------------------------------------


//! Animates this mesh's joints based on frame input
//! blend: {0-old position, 1-New position}
void CSkinnedMesh::animateMesh(f32 frame, f32 blend)
{
	if ( !HasAnimation  || lastAnimatedFrame==frame)
		return;

	lastAnimatedFrame=frame;

	if (blend<=0.f)
		return; //No need to animate

	for (u32 i=0; i<AllJoints.size(); ++i)
	{
		//To Bitplane: The joints can be animated here with no input from their parents, but for setAnimationMode extra checks are needed to their parents

		SJoint *Joint = AllJoints[i];

		const core::vector3df oldPosition = Joint->Animatedposition;
		const core::vector3df oldScale = Joint->Animatedscale;
		const core::quaternion oldRotation = Joint->Animatedrotation;

		core::vector3df position = oldPosition;
		core::vector3df scale = oldScale;
		core::quaternion rotation = oldRotation;

		if (!BoneControlUsed)
		{
			Joint->positionHint=-1;
			Joint->scaleHint=-1;
			Joint->rotationHint=-1;
		}

		getFrameData(frame, Joint,
				position, Joint->positionHint,
				scale, Joint->scaleHint,
				rotation, Joint->rotationHint);

		if (blend==1.0f)
		{
			//No blending needed
			Joint->Animatedposition = position;
			Joint->Animatedscale = scale;
			Joint->Animatedrotation = rotation;
		}
		else
		{
			//Blend animation
			Joint->Animatedposition = core::lerp(oldPosition, position, blend);
			Joint->Animatedscale = core::lerp(oldScale, scale, blend);
			Joint->Animatedrotation.slerp(oldRotation, rotation, blend);
		}

		//Note:
		//_LocalAnimatedMatrix needs to be built at some point, but this function may be called lots of times for
		//one render (to play two animations at the same time) _LocalAnimatedMatrix only needs to be built once.
		//a call to buildAllLocalAnimatedMatrices is needed before skinning the mesh, and before the user gets the joints to move

		//----------------
		// Temp!
		buildAll_LocalAnimatedMatrices();
		//-----------------
	}

	BoneControlUsed=false;
}


void CSkinnedMesh::buildAll_LocalAnimatedMatrices()
{
	for (u32 i=0; i<AllJoints.size(); ++i)
	{
		SJoint *Joint = AllJoints[i];

		//Could be faster:

		if (Joint->UseAnimationFrom &&
			(Joint->UseAnimationFrom->PositionKeys.size() ||
			 Joint->UseAnimationFrom->ScaleKeys.size() ||
			 Joint->UseAnimationFrom->RotationKeys.size() ))
		{
			Joint->LocalAnimatedMatrix.makeIdentity();
			Joint->LocalAnimatedMatrix.setTranslation(Joint->Animatedposition);
			Joint->LocalAnimatedMatrix*=Joint->Animatedrotation.getMatrix();

			if (Joint->ScaleKeys.size())
			{
				//Joint->_LocalAnimatedMatrix.setScale(Joint->_Animatedscale);
				core::matrix4 scaleMatrix;
				scaleMatrix.setScale(Joint->Animatedscale);
				Joint->LocalAnimatedMatrix *= scaleMatrix;
			}
		}
		else
		{
			Joint->LocalAnimatedMatrix=Joint->LocalMatrix;
		}
	}
}


void CSkinnedMesh::buildAll_GlobalAnimatedMatrices(SJoint *Joint, SJoint *ParentJoint)
{
	if (!Joint)
	{
		for (u32 i=0; i<RootJoints.size(); ++i)
			buildAll_GlobalAnimatedMatrices(RootJoints[i], 0);
		return;
	}
	else
	{
		// Find global matrix...
		if (!ParentJoint)
			Joint->GlobalAnimatedMatrix = Joint->LocalAnimatedMatrix;
		else
			Joint->GlobalAnimatedMatrix = ParentJoint->GlobalAnimatedMatrix * Joint->LocalAnimatedMatrix;
	}

	for (u32 j=0; j<Joint->Children.size(); ++j)
		buildAll_GlobalAnimatedMatrices(Joint->Children[j], Joint);
}


void CSkinnedMesh::getFrameData(f32 frame, SJoint *Joint,
				core::vector3df &position, s32 &positionHint,
				core::vector3df &scale, s32 &scaleHint,
				core::quaternion &rotation, s32 &rotationHint)
{
	s32 foundPositionIndex = -1;
	s32 foundScaleIndex = -1;
	s32 foundRotationIndex = -1;

	if (Joint->UseAnimationFrom)
	{
		const core::array<SPositionKey> &PositionKeys=Joint->UseAnimationFrom->PositionKeys;
		const core::array<SScaleKey> &ScaleKeys=Joint->UseAnimationFrom->ScaleKeys;
		const core::array<SRotationKey> &RotationKeys=Joint->UseAnimationFrom->RotationKeys;

		if (PositionKeys.size())
		{
			foundPositionIndex = -1;

			//Test the Hints...
			if ((u32)positionHint < PositionKeys.size())
			{
				//check this hint
				if (PositionKeys[positionHint].frame>=frame && PositionKeys[positionHint-1].frame<frame )
					foundPositionIndex=positionHint;
				else if (positionHint+1 < (s32)PositionKeys.size())
				{
					//check the next index
					if ( PositionKeys[positionHint+1].frame>=frame &&
							PositionKeys[positionHint+0].frame<frame)
					{
						positionHint++;
						foundPositionIndex=positionHint;
					}
				}
			}

			//The hint test failed, do a full scan...
			if (foundPositionIndex==-1)
			{
				for (u32 i=0; i<PositionKeys.size(); ++i)
				{
					if (PositionKeys[i].frame >= frame) //Keys should to be sorted by frame
					{
						foundPositionIndex=i;
						positionHint=i;
						break;
					}
				}
			}

			//Do interpolation...
			if (foundPositionIndex!=-1)
			{
				if (InterpolationMode==EIM_CONSTANT || foundPositionIndex==0)
				{
					position = PositionKeys[foundPositionIndex].position;
				}
				else if (InterpolationMode==EIM_LINEAR)
				{
					const SPositionKey& KeyA = PositionKeys[foundPositionIndex];
					const SPositionKey& KeyB = PositionKeys[foundPositionIndex-1];

					const f32 fd1 = frame - KeyA.frame;
					const f32 fd2 = KeyB.frame - frame;
					position = ((KeyB.position-KeyA.position)/(fd1+fd2))*fd1 + KeyA.position;
				}
			}
		}

		//------------------------------------------------------------

		if (ScaleKeys.size())
		{
			foundScaleIndex = -1;

			//Test the Hints...
			if ((u32)scaleHint < ScaleKeys.size())
			{
				//check this hint
				if (ScaleKeys[scaleHint].frame>=frame && ScaleKeys[scaleHint-1].frame<frame )
					foundScaleIndex=scaleHint;
				else if (scaleHint+1 < (s32)ScaleKeys.size())
				{
					//check the next index
					if ( ScaleKeys[scaleHint+1].frame>=frame &&
							ScaleKeys[scaleHint+0].frame<frame)
					{
						scaleHint++;
						foundScaleIndex=scaleHint;
					}
				}
			}


			//The hint test failed, do a full scan...
			if (foundScaleIndex==-1)
			{
				for (u32 i=0; i<ScaleKeys.size(); ++i)
				{
					if (ScaleKeys[i].frame >= frame) //Keys should to be sorted by frame
					{
						foundScaleIndex=i;
						scaleHint=i;
						break;
					}
				}
			}

			//Do interpolation...
			if (foundScaleIndex!=-1)
			{
				if (InterpolationMode==EIM_CONSTANT || foundScaleIndex==0)
				{
					scale = ScaleKeys[foundScaleIndex].scale;
				}
				else if (InterpolationMode==EIM_LINEAR)
				{
					const SScaleKey& KeyA = ScaleKeys[foundScaleIndex];
					const SScaleKey& KeyB = ScaleKeys[foundScaleIndex-1];

					const f32 fd1 = frame - KeyA.frame;
					const f32 fd2 = KeyB.frame - frame;
					scale = ((KeyB.scale-KeyA.scale)/(fd1+fd2))*fd1 + KeyA.scale;
				}
			}
		}

		//-------------------------------------------------------------

		if (RotationKeys.size())
		{
			foundRotationIndex = -1;

			//Test the Hints...
			if ((u32)rotationHint < RotationKeys.size())
			{
				//check this hint
				if (RotationKeys[rotationHint].frame>=frame && RotationKeys[rotationHint-1].frame<frame )
					foundRotationIndex=rotationHint;
				else if (rotationHint+1 < (s32)RotationKeys.size())
				{
					//check the next index
					if ( RotationKeys[rotationHint+1].frame>=frame &&
							RotationKeys[rotationHint+0].frame<frame)
					{
						rotationHint++;
						foundRotationIndex=rotationHint;
					}
				}
			}


			//The hint test failed, do a full scan...
			if (foundRotationIndex==-1)
			{
				for (u32 i=0; i<RotationKeys.size(); ++i)
				{
					if (RotationKeys[i].frame >= frame) //Keys should be sorted by frame
					{
						foundRotationIndex=i;
						rotationHint=i;
						break;
					}
				}
			}

			//Do interpolation...
			if (foundRotationIndex!=-1)
			{
				if (InterpolationMode==EIM_CONSTANT || foundRotationIndex==0)
				{
					rotation = RotationKeys[foundRotationIndex].rotation;
				}
				else if (InterpolationMode==EIM_LINEAR)
				{
					const SRotationKey& KeyA = RotationKeys[foundRotationIndex];
					const SRotationKey& KeyB = RotationKeys[foundRotationIndex-1];

					const f32 fd1 = frame - KeyA.frame;
					const f32 fd2 = KeyB.frame - frame;
					const f32 t = fd1/(fd1+fd2);

					/*
					f32 t = 0;
					if (KeyA.frame!=KeyB.frame)
						t = (frame-KeyA.frame) / (KeyB.frame - KeyA.frame);
					*/

					rotation.slerp(KeyA.rotation, KeyB.rotation, t);
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
//				Software Skinning
//--------------------------------------------------------------------------

//! Preforms a software skin on this mesh based of joint positions
void CSkinnedMesh::skinMesh()
{

	if ( !HasAnimation)
		return;


	//----------------
	// Temp!
	buildAll_GlobalAnimatedMatrices();
	//-----------------


	//Software skin....

	u32 i;




	//rigid animation
	for (i=0; i<AllJoints.size(); ++i)
	{
		for (u32 j=0; j<AllJoints[i]->AttachedMeshes.size(); ++j)
		{
			SSkinMeshBuffer* Buffer=(*SkinningBuffers)[ AllJoints[i]->AttachedMeshes[j] ];
			Buffer->Transformation=AllJoints[i]->GlobalAnimatedMatrix;
		}
	}

	//clear skinning helper array

	for (i=0; i<Vertices_Moved.size(); ++i)
		for (u32 j=0; j<Vertices_Moved[i].size(); ++j)
			Vertices_Moved[i][j]=false;


	//skin starting with the root joints
	for (i=0; i<RootJoints.size(); ++i)
		SkinJoint(RootJoints[i], 0);
}

void CSkinnedMesh::SkinJoint(SJoint *Joint, SJoint *ParentJoint)
{
	if (Joint->Weights.size())
	{
		//Find this joints pull on vertices...
		core::matrix4 JointVertexPull(core::matrix4::EM4CONST_NOTHING);
		JointVertexPull.setbyproduct(Joint->GlobalAnimatedMatrix, Joint->GlobalInversedMatrix);

		core::vector3df ThisVertexMove, ThisNormalMove;

		core::array<scene::SSkinMeshBuffer*> &BuffersUsed=*SkinningBuffers;

		//Skin Vertices Positions and Normals...
		for (u32 i=0; i<Joint->Weights.size(); ++i)
		{
			SWeight& weight = Joint->Weights[i];

			// Pull this vertex...
			JointVertexPull.transformVect(ThisVertexMove, weight.StaticPos);

			if (AnimateNormals)
				JointVertexPull.rotateVect(ThisNormalMove, weight.StaticNormal);

			if (! (*(weight.Moved)) )
			{
				*(weight.Moved) = true;

				BuffersUsed[weight.buffer_id]->getVertex(weight.vertex_id)->Pos = ThisVertexMove * weight.strength;

				if (AnimateNormals)
					BuffersUsed[weight.buffer_id]->getVertex(weight.vertex_id)->Normal = ThisNormalMove * weight.strength;

				//*(weight._Pos) = ThisVertexMove * weight.strength;
			}
			else
			{
				BuffersUsed[weight.buffer_id]->getVertex(weight.vertex_id)->Pos += ThisVertexMove * weight.strength;

				if (AnimateNormals)
					BuffersUsed[weight.buffer_id]->getVertex(weight.vertex_id)->Normal += ThisNormalMove * weight.strength;

				//*(weight._Pos) += ThisVertexMove * weight.strength;
			}
		}
	}

	//Skin all children
	for (u32 j=0; j<Joint->Children.size(); ++j)
		SkinJoint(Joint->Children[j], Joint);
}


E_ANIMATED_MESH_TYPE CSkinnedMesh::getMeshType() const
{
	return EAMT_SKINNED;
}


//! Gets joint count.
u32 CSkinnedMesh::getJointCount() const
{
	return AllJoints.size();
}


//! Gets the name of a joint.
const c8* CSkinnedMesh::getJointName(u32 number) const
{
	if (number >= AllJoints.size())
		return 0;
	return AllJoints[number]->Name.c_str();
}


//! Gets a joint number from its name
s32 CSkinnedMesh::getJointNumber(const c8* name) const
{
	for (u32 i=0; i<AllJoints.size(); ++i)
	{
		if (AllJoints[i]->Name == name)
			return i;
	}

	return -1;
}


//! returns amount of mesh buffers.
u32 CSkinnedMesh::getMeshBufferCount() const
{
	return LocalBuffers.size();
}


//! returns pointer to a mesh buffer
IMeshBuffer* CSkinnedMesh::getMeshBuffer(u32 nr) const
{
	if (nr < LocalBuffers.size())
		return LocalBuffers[nr];
	else
		return 0;
}


//! Returns pointer to a mesh buffer which fits a material
IMeshBuffer* CSkinnedMesh::getMeshBuffer(const video::SMaterial &material) const
{
	for (u32 i=0; i<LocalBuffers.size(); ++i)
	{
		if (LocalBuffers[i]->getMaterial() == material)
			return LocalBuffers[i];
	}
	return 0;
}


//! returns an axis aligned bounding box
const core::aabbox3d<f32>& CSkinnedMesh::getBoundingBox() const
{
	return BoundingBox;
}


//! set user axis aligned bounding box
void CSkinnedMesh::setBoundingBox( const core::aabbox3df& box)
{
	BoundingBox = box;
}


//! sets a flag of all contained materials to a new value
void CSkinnedMesh::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (u32 i=0; i<LocalBuffers.size(); ++i)
		LocalBuffers[i]->Material.setFlag(flag,newvalue);
}


//! uses animation from another mesh
bool CSkinnedMesh::useAnimationFrom(const ISkinnedMesh *mesh)
{
	bool unmatched=false;

	for(u32 i=0;i<AllJoints.size();++i)
	{
		SJoint *joint=AllJoints[i];
		joint->UseAnimationFrom=0;

		if (joint->Name=="")
			unmatched=true;
		else
		{
			for(u32 j=0;j<mesh->getAllJoints().size();++j)
			{
				SJoint *otherJoint=mesh->getAllJoints()[j];
				if (joint->Name==otherJoint->Name)
				{
					joint->UseAnimationFrom=otherJoint;
				}
			}
			if (!joint->UseAnimationFrom) unmatched=true;
		}
	}

	checkForAnimation();

	return !unmatched;
}


//!Update Normals when Animating
//!False= Don't (default)
//!True= Update normals, slower
void CSkinnedMesh::updateNormalsWhenAnimating(bool on)
{
	AnimateNormals = on;
}


//!Sets Interpolation Mode
void CSkinnedMesh::setInterpolationMode(E_INTERPOLATION_MODE mode)
{
	InterpolationMode = mode;
}


core::array<scene::SSkinMeshBuffer*> &CSkinnedMesh::getMeshBuffers()
{
	return LocalBuffers;
}


core::array<CSkinnedMesh::SJoint*> &CSkinnedMesh::getAllJoints()
{
	return AllJoints;
}


const core::array<CSkinnedMesh::SJoint*> &CSkinnedMesh::getAllJoints() const
{
	return AllJoints;
}


void CSkinnedMesh::CalculateGlobalMatrixes(SJoint *Joint,SJoint *ParentJoint)
{
	if (!Joint && ParentJoint) // bit of protection from endless loops
		return;

	//Go through the root bones
	if (!Joint)
	{
		for (u32 i=0; i<RootJoints.size(); ++i)
			CalculateGlobalMatrixes(RootJoints[i],0);
		return;
	}

	if (!ParentJoint)
		Joint->GlobalMatrix = Joint->LocalMatrix;
	else
		Joint->GlobalMatrix = ParentJoint->GlobalMatrix * Joint->LocalMatrix;

	Joint->LocalAnimatedMatrix=Joint->LocalMatrix;
	Joint->GlobalAnimatedMatrix=Joint->GlobalMatrix;

	if (Joint->GlobalInversedMatrix.isIdentity())//might be pre calculated
	{
		Joint->GlobalInversedMatrix = Joint->GlobalMatrix;
		Joint->GlobalInversedMatrix.makeInverse(); // slow
	}

	for (u32 j=0; j<Joint->Children.size(); ++j)
		CalculateGlobalMatrixes(Joint->Children[j],Joint);
}


void CSkinnedMesh::checkForAnimation()
{
	u32 i,j;
	//Check for animation...
	HasAnimation = false;
	for(i=0;i<AllJoints.size();++i)
	{
		if (AllJoints[i]->UseAnimationFrom)
		{
			if (AllJoints[i]->UseAnimationFrom->PositionKeys.size() ||
				AllJoints[i]->UseAnimationFrom->ScaleKeys.size() ||
				AllJoints[i]->UseAnimationFrom->RotationKeys.size() )
			{
				HasAnimation = true;
			}
		}
	}

	//meshes with weights, are still counted as animated for ragdolls, etc
	if (!HasAnimation)
	{
		for(i=0;i<AllJoints.size();++i)
		{
			if (AllJoints[i]->Weights.size())
				HasAnimation = true;
		}
	}

	if (HasAnimation)
	{
		//--- Find the length of the animation ---
		AnimationFrames=0;
		for(i=0;i<AllJoints.size();++i)
		{
			if (AllJoints[i]->UseAnimationFrom)
			{
				if (AllJoints[i]->UseAnimationFrom->PositionKeys.size())
					if (AllJoints[i]->UseAnimationFrom->PositionKeys.getLast().frame > AnimationFrames)
						AnimationFrames=AllJoints[i]->UseAnimationFrom->PositionKeys.getLast().frame;

				if (AllJoints[i]->UseAnimationFrom->ScaleKeys.size())
					if (AllJoints[i]->UseAnimationFrom->ScaleKeys.getLast().frame > AnimationFrames)
						AnimationFrames=AllJoints[i]->UseAnimationFrom->ScaleKeys.getLast().frame;

				if (AllJoints[i]->UseAnimationFrom->RotationKeys.size())
					if (AllJoints[i]->UseAnimationFrom->RotationKeys.getLast().frame > AnimationFrames)
						AnimationFrames=AllJoints[i]->UseAnimationFrom->RotationKeys.getLast().frame;
			}
		}
	}

	if (HasAnimation && !PreparedForSkinning)
	{
		PreparedForSkinning=true;

		//check for bugs:
		for(i=0; i < AllJoints.size(); ++i)
		{
			SJoint *Joint = AllJoints[i];
			for (j=0; j<Joint->Weights.size(); ++j)
			{
				u16 buffer_id=Joint->Weights[j].buffer_id;
				u32 vertex_id=Joint->Weights[j].vertex_id;

				//check for invalid ids
				if (buffer_id>=LocalBuffers.size())
				{
					os::Printer::log("Skinned Mesh: Weight buffer id too large", ELL_WARNING);
					Joint->Weights[j].buffer_id = Joint->Weights[j].vertex_id =0;
				}
				else if (vertex_id>=LocalBuffers[buffer_id]->getVertexCount())
				{
					os::Printer::log("Skinned Mesh: Weight vertex id too large", ELL_WARNING);
					Joint->Weights[j].buffer_id = Joint->Weights[j].vertex_id =0;
				}
			}
		}

		//An array used in skinning

		for (i=0; i<Vertices_Moved.size(); ++i)
			for (j=0; j<Vertices_Moved[i].size(); ++j)
				Vertices_Moved[i][j] = false;

		// For skinning: cache weight values for speed

		for (i=0; i<AllJoints.size(); ++i)
		{
			SJoint *Joint = AllJoints[i];
			for (j=0; j<Joint->Weights.size(); ++j)
			{
				u32 vertex_id=Joint->Weights[j].vertex_id;
				u32 buffer_id=Joint->Weights[j].buffer_id;

				Joint->Weights[j].Moved = &Vertices_Moved[buffer_id] [vertex_id];
				Joint->Weights[j].StaticPos = LocalBuffers[buffer_id]->getVertex(vertex_id)->Pos;
				Joint->Weights[j].StaticNormal = LocalBuffers[buffer_id]->getVertex(vertex_id)->Normal;

				//Joint->Weights[j]._Pos=&Buffers[buffer_id]->getVertex(vertex_id)->Pos;
			}
		}

		// normalize weights
		normalizeWeights();
	}
}


//! called by loader after populating with mesh and bone data
void CSkinnedMesh::finalize()
{
	u32 i;

	lastAnimatedFrame=-1;
	lastSkinnedFrame=-1;
	AnimateNormals=false;

	//calculate bounding box

	for (i=0; i<LocalBuffers.size(); ++i)
	{
		LocalBuffers[i]->recalculateBoundingBox();
	}

	// Get BoundingBox...
	if (LocalBuffers.empty())
		BoundingBox.reset(0,0,0);
	else
	{
		BoundingBox.reset(LocalBuffers[0]->BoundingBox.MaxEdge);
		for (u32 i=0; i<LocalBuffers.size(); ++i)
		{
			BoundingBox.addInternalBox(LocalBuffers[i]->BoundingBox);
		}
	}

	//add 5% padding to bounding box
	core::vector3df Padding=BoundingBox.getExtent()*0.05f;
	BoundingBox.MinEdge-=Padding;
	BoundingBox.MaxEdge+=Padding;


	if (AllJoints.size() || RootJoints.size())
	{
		// populate AllJoints or RootJoints, depending on which is empty
		if (!RootJoints.size())
		{

			for(u32 CheckingIdx=0; CheckingIdx < AllJoints.size(); ++CheckingIdx)
			{

				bool foundParent=false;
				for(i=0; i < AllJoints.size(); ++i)
				{
					for(u32 n=0; n < AllJoints[i]->Children.size(); ++n)
					{
						if (AllJoints[i]->Children[n] == AllJoints[CheckingIdx])
							foundParent=true;
					}
				}

				if (!foundParent)
					RootJoints.push_back(AllJoints[CheckingIdx]);
			}
		}
		else
		{
			AllJoints=RootJoints;
		}
	}

	for(i=0; i < AllJoints.size(); ++i)
	{
		AllJoints[i]->UseAnimationFrom=AllJoints[i];
	}

	//Set array sizes...

	for (i=0; i<LocalBuffers.size(); ++i)
	{
		Vertices_Moved.push_back( core::array<bool>() );
		Vertices_Moved[i].set_used(LocalBuffers[i]->getVertexCount());
	}


	//Todo: optimise keys here...


	checkForAnimation();


	if (HasAnimation)
	{

		//--- optimize and check keyframes ---
		for(i=0;i<AllJoints.size();++i)
		{
			core::array<SPositionKey> &PositionKeys =AllJoints[i]->PositionKeys;
			core::array<SScaleKey> &ScaleKeys = AllJoints[i]->ScaleKeys;
			core::array<SRotationKey> &RotationKeys = AllJoints[i]->RotationKeys;

			if (PositionKeys.size()>2)
			{
				for(u32 j=0;j<PositionKeys.size()-2;++j)
				{
					if (PositionKeys[j].position == PositionKeys[j+1].position && PositionKeys[j+1].position == PositionKeys[j+2].position)
					{
						PositionKeys.erase(j+1); //the middle key is unneeded
						--j;
					}
				}
			}

			if (PositionKeys.size()>1)
			{
				for(u32 j=0;j<PositionKeys.size()-1;++j)
				{
					if (PositionKeys[j].frame >= PositionKeys[j+1].frame) //bad frame, unneed and may cause problems
					{
						PositionKeys.erase(j+1);
						--j;
					}
				}
			}

			if (ScaleKeys.size()>2)
			{
				for(u32 j=0;j<ScaleKeys.size()-2;++j)
				{
					if (ScaleKeys[j].scale == ScaleKeys[j+1].scale && ScaleKeys[j+1].scale == ScaleKeys[j+2].scale)
					{
						ScaleKeys.erase(j+1); //the middle key is unneeded
						--j;
					}
				}
			}

			if (ScaleKeys.size()>1)
			{
				for(u32 j=0;j<ScaleKeys.size()-1;++j)
				{
					if (ScaleKeys[j].frame >= ScaleKeys[j+1].frame) //bad frame, unneed and may cause problems
					{
						ScaleKeys.erase(j+1);
						--j;
					}
				}
			}

			if (RotationKeys.size()>2)
			{
				for(u32 j=0;j<RotationKeys.size()-2;++j)
				{
					if (RotationKeys[j].rotation == RotationKeys[j+1].rotation && RotationKeys[j+1].rotation == RotationKeys[j+2].rotation)
					{
						RotationKeys.erase(j+1); //the middle key is unneeded
						--j;
					}
				}
			}

			if (RotationKeys.size()>1)
			{
				for(u32 j=0;j<RotationKeys.size()-1;++j)
				{
					if (RotationKeys[j].frame >= RotationKeys[j+1].frame) //bad frame, unneed and may cause problems
					{
						RotationKeys.erase(j+1);
						--j;
					}
				}
			}


			//Fill empty keyframe areas
			if (PositionKeys.size())
			{
				SPositionKey *Key;
				Key=&PositionKeys[0];//getFirst
				if (Key->frame!=0)
				{
					PositionKeys.push_front(*Key);
					Key=&PositionKeys[0];//getFirst
					Key->frame=0;
				}

				Key=&PositionKeys.getLast();
				if (Key->frame!=AnimationFrames)
				{
					PositionKeys.push_back(*Key);
					Key=&PositionKeys.getLast();
					Key->frame=AnimationFrames;
				}
			}

			if (ScaleKeys.size())
			{
				SScaleKey *Key;
				Key=&ScaleKeys[0];//getFirst
				if (Key->frame!=0)
				{
					ScaleKeys.push_front(*Key);
					Key=&ScaleKeys[0];//getFirst
					Key->frame=0;
				}

				Key=&ScaleKeys.getLast();
				if (Key->frame!=AnimationFrames)
				{
					ScaleKeys.push_back(*Key);
					Key=&ScaleKeys.getLast();
					Key->frame=AnimationFrames;
				}
			}

			if (RotationKeys.size())
			{
				SRotationKey *Key;
				Key=&RotationKeys[0];//getFirst
				if (Key->frame!=0)
				{
					RotationKeys.push_front(*Key);
					Key=&RotationKeys[0];//getFirst
					Key->frame=0;
				}

				Key=&RotationKeys.getLast();
				if (Key->frame!=AnimationFrames)
				{
					RotationKeys.push_back(*Key);
					Key=&RotationKeys.getLast();
					Key->frame=AnimationFrames;
				}
			}
		}
	}

	//Needed for animation and skinning...

	CalculateGlobalMatrixes(0,0);

	//animateMesh(0, 1);
	//buildAll_LocalAnimatedMatrices();
	//buildAll_GlobalAnimatedMatrices();



	//rigid animation for non animated meshes
	for (i=0; i<AllJoints.size(); ++i)
	{
		for (u32 j=0; j<AllJoints[i]->AttachedMeshes.size(); ++j)
		{
			SSkinMeshBuffer* Buffer=(*SkinningBuffers)[ AllJoints[i]->AttachedMeshes[j] ];
			Buffer->Transformation=AllJoints[i]->GlobalAnimatedMatrix;
		}
	}



}


scene::SSkinMeshBuffer *CSkinnedMesh::createBuffer()
{
	scene::SSkinMeshBuffer *buffer=new scene::SSkinMeshBuffer();
	LocalBuffers.push_back(buffer);
	return buffer;
}


CSkinnedMesh::SJoint *CSkinnedMesh::createJoint(SJoint *parent)
{
	SJoint *joint=new SJoint;

	AllJoints.push_back(joint);
	if (!parent)
	{
		//Add root joints to array in finalize()
	}
	else
	{
		//Set parent (Be careful of the mesh loader also setting the parent)
		parent->Children.push_back(joint);
	}

	return joint;
}


CSkinnedMesh::SPositionKey *CSkinnedMesh::createPositionKey(SJoint *joint)
{
	if (!joint)
		return 0;
	SPositionKey *key;

	joint->PositionKeys.push_back(SPositionKey());
	key=&joint->PositionKeys.getLast();

	key->frame=0;
	return key;
}


CSkinnedMesh::SScaleKey *CSkinnedMesh::createScaleKey(SJoint *joint)
{
	if (!joint)
		return 0;
	SScaleKey *key;

	joint->ScaleKeys.push_back(SScaleKey());
	key=&joint->ScaleKeys.getLast();

	return key;
}


CSkinnedMesh::SRotationKey *CSkinnedMesh::createRotationKey(SJoint *joint)
{
	if (!joint)
		return 0;
	SRotationKey *key;

	joint->RotationKeys.push_back(SRotationKey());
	key=&joint->RotationKeys.getLast();

	return key;
}


CSkinnedMesh::SWeight *CSkinnedMesh::createWeight(SJoint *joint)
{
	if (!joint)
		return 0;

	joint->Weights.push_back(SWeight());

	SWeight *weight=&joint->Weights.getLast();

	//Could do stuff here...

	return weight;
}


void CSkinnedMesh::normalizeWeights()
{
	// note: unsure if weights ids are going to be used.

	// Normalise the weights on bones....

	u32 i,j;
	core::array< core::array<f32> > Vertices_TotalWeight;

	for (i=0; i<LocalBuffers.size(); ++i)
	{
		Vertices_TotalWeight.push_back(core::array<f32>());
		Vertices_TotalWeight[i].set_used(LocalBuffers[i]->getVertexCount());
	}


	for (i=0; i<Vertices_TotalWeight.size(); ++i)
		for (j=0; j<Vertices_TotalWeight[i].size(); ++j)
			Vertices_TotalWeight[i][j] = 0;

	for (i=0; i<AllJoints.size(); ++i)
	{
		SJoint *Joint=AllJoints[i];
		for (j=0; j<Joint->Weights.size(); ++j)
		{
			if (Joint->Weights[j].strength<=0)//Check for invalid weights
			{
				Joint->Weights.erase(j);
				j--;
			}
			else
			{
				Vertices_TotalWeight[ Joint->Weights[j].buffer_id ] [ Joint->Weights[j].vertex_id ] += Joint->Weights[j].strength;
			}
		}
	}

	for (i=0; i<AllJoints.size(); ++i)
	{
		SJoint *Joint=AllJoints[i];
		for (j=0; j< Joint->Weights.size(); ++j)
		{
			f32 total = Vertices_TotalWeight[ Joint->Weights[j].buffer_id ] [ Joint->Weights[j].vertex_id ];
			if (total != 0 && total != 1)
				Joint->Weights[j].strength /= total;
		}
	}
}


void CSkinnedMesh::recoverJointsFromMesh(core::array<IBoneSceneNode*> &JointChildSceneNodes)
{
	for (u32 i=0;i<AllJoints.size();++i)
	{
		IBoneSceneNode* node=JointChildSceneNodes[i];
		SJoint *joint=AllJoints[i];
		node->setPosition( joint->LocalAnimatedMatrix.getTranslation() );
		node->setRotation( joint->LocalAnimatedMatrix.getRotationDegrees() );

		//node->setScale( joint->LocalAnimatedMatrix.getScale() );

		node->positionHint=joint->positionHint;
		node->scaleHint=joint->scaleHint;
		node->rotationHint=joint->rotationHint;

		//node->setAbsoluteTransformation(joint->GlobalMatrix); //not going to work

		//Note: This updateAbsolutePosition will not work well if joints are not nested like b3d
		//node->updateAbsolutePosition();
	}
}

void CSkinnedMesh::transferJointsToMesh(const core::array<IBoneSceneNode*> &JointChildSceneNodes)
{
	for (u32 i=0;i<AllJoints.size();++i)
	{
		IBoneSceneNode* node=JointChildSceneNodes[i];
		SJoint *joint=AllJoints[i];
		joint->LocalAnimatedMatrix.setTranslation( node->getPosition() );
		joint->LocalAnimatedMatrix.setRotationDegrees( node->getRotation() );
		//joint->LocalAnimatedMatrix.setScale( node->getScale() );

		joint->positionHint=node->positionHint;
		joint->scaleHint=node->scaleHint;
		joint->rotationHint=node->rotationHint;
	}
	//Remove cache, temp...
	lastAnimatedFrame=-1;
	lastSkinnedFrame=-1;

	BoneControlUsed=true;
}


void CSkinnedMesh::createJoints(core::array<IBoneSceneNode*> &JointChildSceneNodes,
	IAnimatedMeshSceneNode* AnimatedMeshSceneNode, ISceneManager* SceneManager)
{
	u32 i;

	//Create new joints
	for (i=0;i<AllJoints.size();++i)
	{
		IBoneSceneNode* node = new CBoneSceneNode(0, SceneManager, 0, i, AllJoints[i]->Name.c_str());

		JointChildSceneNodes.push_back(node);
	}

	//Match up parents
	for (i=0;i<JointChildSceneNodes.size();++i)
	{
		IBoneSceneNode* node=JointChildSceneNodes[i];
		SJoint *joint=AllJoints[i]; //should be fine

		s32 parentID=-1;

		for (u32 j=0;j<AllJoints.size();++j)
		{
			if (i!=j && parentID==-1)
			{
				SJoint *parentTest=AllJoints[j];
				for (u32 n=0;n<parentTest->Children.size();++n)
				{
					if (parentTest->Children[n]==joint)
					{
						parentID=j;
						break;
					}
				}
			}
		}

		if (parentID!=-1)
			node->setParent( JointChildSceneNodes[parentID] );
		else
			node->setParent( AnimatedMeshSceneNode );

		node->drop();
	}
}


void CSkinnedMesh::convertMeshToTangents()
{
	// now calculate tangents
	for (u32 b=0; b < LocalBuffers.size(); ++b)
	{
		if (LocalBuffers[b])
		{
			LocalBuffers[b]->MoveTo_Tangents();

			s32 idxCnt = LocalBuffers[b]->getIndexCount();

			u16* idx = LocalBuffers[b]->getIndices();
			video::S3DVertexTangents* v =
				(video::S3DVertexTangents*)LocalBuffers[b]->getVertices();

			for (s32 i=0; i<idxCnt; i+=3)
			{
				calculateTangents(
					v[idx[i+0]].Normal,
					v[idx[i+0]].Tangent,
					v[idx[i+0]].Binormal,
					v[idx[i+0]].Pos,
					v[idx[i+1]].Pos,
					v[idx[i+2]].Pos,
					v[idx[i+0]].TCoords,
					v[idx[i+1]].TCoords,
					v[idx[i+2]].TCoords);

				calculateTangents(
					v[idx[i+1]].Normal,
					v[idx[i+1]].Tangent,
					v[idx[i+1]].Binormal,
					v[idx[i+1]].Pos,
					v[idx[i+2]].Pos,
					v[idx[i+0]].Pos,
					v[idx[i+1]].TCoords,
					v[idx[i+2]].TCoords,
					v[idx[i+0]].TCoords);

				calculateTangents(
					v[idx[i+2]].Normal,
					v[idx[i+2]].Tangent,
					v[idx[i+2]].Binormal,
					v[idx[i+2]].Pos,
					v[idx[i+0]].Pos,
					v[idx[i+1]].Pos,
					v[idx[i+2]].TCoords,
					v[idx[i+0]].TCoords,
					v[idx[i+1]].TCoords);
			}
		}
	}
}


void CSkinnedMesh::calculateTangents(
	core::vector3df& normal,
	core::vector3df& tangent,
	core::vector3df& binormal,
	core::vector3df& vt1, core::vector3df& vt2, core::vector3df& vt3, // vertices
	core::vector2df& tc1, core::vector2df& tc2, core::vector2df& tc3) // texture coords
{
	core::vector3df v1 = vt1 - vt2;
	core::vector3df v2 = vt3 - vt1;
	normal = v2.crossProduct(v1);
	normal.normalize();

	// binormal

	f32 deltaX1 = tc1.X - tc2.X;
	f32 deltaX2 = tc3.X - tc1.X;
	binormal = (v1 * deltaX2) - (v2 * deltaX1);
	binormal.normalize();

	// tangent

	f32 deltaY1 = tc1.Y - tc2.Y;
	f32 deltaY2 = tc3.Y - tc1.Y;
	tangent = (v1 * deltaY2) - (v2 * deltaY1);
	tangent.normalize();

	// adjust

	core::vector3df txb = tangent.crossProduct(binormal);
	if (txb.dotProduct(normal) < 0.0f)
	{
		tangent *= -1.0f;
		binormal *= -1.0f;
	}
}


} // end namespace scene
} // end namespace irr

#endif // _IRR_COMPILE_WITH_SKINNED_MESH_SUPPORT_

