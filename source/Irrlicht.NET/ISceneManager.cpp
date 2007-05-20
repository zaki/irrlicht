// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ISceneManager.h"
#include "NativeConverter.h"
#include "ITriangleSelector.h"
#include "ISceneNodeAnimator.h"
#include "IMetaTriangleSelector.h"
#include "ISceneCollisionManager.h"
#include "ITerrainSceneNode.h"
#include "IMeshManipulator.h"

namespace Irrlicht
{
namespace Scene
{

	ISceneManager::ISceneManager(irr::scene::ISceneManager* manager)
		: Manager(manager)
	{
		SCM = new ISceneCollisionManager(Manager->getSceneCollisionManager());
		Manipulator = new IMeshManipulator(Manager->getMeshManipulator());
		Root = new ISceneNode(Manager->getRootSceneNode());
	}

	IAnimatedMesh* ISceneManager::GetMesh(System::String* filename)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);

		irr::scene::IAnimatedMesh* am = Manager->getMesh(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);

		if (!am)
			return 0;

		return new IAnimatedMesh(am);
	}

	ISceneNode* ISceneManager::AddCubeSceneNode(float size, ISceneNode* parent, int id,
			 Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale)
	{
		irr::scene::ISceneNode* node = 
			Manager->addCubeSceneNode(size, parent ? parent->get_NativeSceneNode() : 0, id, 
			irr::NativeConverter::getNativeVector(position),
			irr::NativeConverter::getNativeVector(rotation),
			irr::NativeConverter::getNativeVector(scale));

		if (!node)
			return 0;

		return new ISceneNode(node);
	}

	ISceneNode* ISceneManager::AddCubeSceneNode(float size, ISceneNode* parent, int id, Core::Vector3D position)
	{
		return AddCubeSceneNode(size, parent, id, position,
			Irrlicht::Core::Vector3D(0,0,0), Irrlicht::Core::Vector3D(1,1,1));
	}

	ICameraSceneNode* ISceneManager::AddCameraSceneNodeFPS()
	{
		return AddCameraSceneNodeFPS(0, 100, 500, -1);
	}

	ICameraSceneNode* ISceneManager::AddCameraSceneNodeFPS(ISceneNode* parent,
		float rotateSpeed, float moveSpeed, int id)
	{
		irr::scene::ICameraSceneNode* node = 
			Manager->addCameraSceneNodeFPS(parent ? parent->get_NativeSceneNode() : 0,
			rotateSpeed, moveSpeed, id);
		if (!node)
			return 0;
		return new ICameraSceneNode(node);
	}

	void ISceneManager::DrawAll()
	{
		Manager->drawAll();
	}

	ISceneNode* ISceneManager::AddOctTreeSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, int id)
	{
		return AddOctTreeSceneNode(mesh, parent, id, 128);
	}

	ISceneNode* ISceneManager::AddOctTreeSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, 
			int id, int minimalPolysPerNode)
	{
		irr::scene::ISceneNode* node = Manager->addOctTreeSceneNode(
			mesh ? mesh->get_NativeAnimatedMesh() : 0, 
			parent ? parent->get_NativeSceneNode() : 0,
			id, minimalPolysPerNode);
		if (!node)
			return 0;
		return new ISceneNode(node);
	}

	ISceneNode* ISceneManager::AddOctTreeSceneNode(IMesh* mesh, ISceneNode* parent, int id)
	{
		return AddOctTreeSceneNode(mesh, parent, id, 128);
	}

	ISceneNode* ISceneManager::AddOctTreeSceneNode(IMesh* mesh, ISceneNode* parent, int id, int minimalPolysPerNode)
	{
		irr::scene::ISceneNode* node = Manager->addOctTreeSceneNode(
			mesh ? mesh->get_NativeMesh() : 0, 
			parent ? parent->get_NativeSceneNode() : 0,
			id, minimalPolysPerNode);
		if (!node)
			return 0;
		return new ISceneNode(node);
	}

	IAnimatedMeshSceneNode* ISceneManager::AddAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, int id,
		Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale)
	{
		irr::scene::IAnimatedMeshSceneNode* am = 
			Manager->addAnimatedMeshSceneNode(
				mesh ? mesh->get_NativeAnimatedMesh() : 0,
				parent ? parent->get_NativeSceneNode() : 0,
				id, 
				irr::NativeConverter::getNativeVector(position),
				irr::NativeConverter::getNativeVector(rotation),
				irr::NativeConverter::getNativeVector(scale));
		if (!am)
			return 0;

		return new IAnimatedMeshSceneNode(am);
	}

	IAnimatedMeshSceneNode* ISceneManager::AddAnimatedMeshSceneNode(IAnimatedMesh* mesh, ISceneNode* parent, int id)
	{
		return AddAnimatedMeshSceneNode(mesh, parent, id,
			Core::Vector3D(0,0,0), Core::Vector3D(0,0,0), Core::Vector3D(1,1,1));
	}

	ISceneNode* ISceneManager::AddMeshSceneNode(IMesh* mesh, ISceneNode* parent, int id)
	{
		return AddMeshSceneNode(mesh, parent, id, 
			Core::Vector3D(0,0,0), Core::Vector3D(0,0,0),
			Core::Vector3D(1,1,1));
	}

	ISceneNode* ISceneManager::AddMeshSceneNode(IMesh* mesh, ISceneNode* parent, int id,
			 Core::Vector3D position, Core::Vector3D rotation, Core::Vector3D scale)
	{
		irr::scene::ISceneNode* node =
			Manager->addMeshSceneNode(mesh ? mesh->get_NativeMesh() : 0,
			parent ? parent->get_NativeSceneNode() : 0,
			id, 
			irr::NativeConverter::getNativeVector(position),
			irr::NativeConverter::getNativeVector(rotation),
			irr::NativeConverter::getNativeVector(scale));
		if (!node)
			return 0;
		return new ISceneNode(node);
	}

	ISceneNode* ISceneManager::AddWaterSurfaceSceneNode(IMesh* mesh,
		float waveHeight, float waveSpeed, float waveLenght, 
		ISceneNode* parent, int id)
	{
		irr::scene::ISceneNode* node = 
			Manager->addWaterSurfaceSceneNode(
			mesh? mesh->get_NativeMesh() : 0,
			waveHeight, waveSpeed, waveLenght, 
			parent ? parent->get_NativeSceneNode() : 0,
			id);
		if (!node)
			return 0;
		return new ISceneNode(node);
	}

	ICameraSceneNode* ISceneManager::AddCameraSceneNode(ISceneNode* parent,
			 Core::Vector3D position, Core::Vector3D lookat, int id)
	{
		irr::scene::ICameraSceneNode* cam =
			Manager->addCameraSceneNode(
			parent ? parent->get_NativeSceneNode() : 0,
			irr::NativeConverter::getNativeVector(position),
			irr::NativeConverter::getNativeVector(lookat), id);
		if (!cam)
			return 0;
		return new ICameraSceneNode(cam);
	}

	ICameraSceneNode* ISceneManager::AddCameraSceneNodeMaya(ISceneNode* parent,
			float rotateSpeed, float zoomSpeed, float translationSpeed, int id)
	{
		irr::scene::ICameraSceneNode* cam =
			Manager->addCameraSceneNodeMaya(
			parent ? parent->get_NativeSceneNode() : 0,
			rotateSpeed, zoomSpeed, translationSpeed, id);
		if (!cam)
			return 0;
		return new ICameraSceneNode(cam);
	}

	ILightSceneNode* ISceneManager::AddLightSceneNode(ISceneNode* parent,
			Core::Vector3D position, Video::Colorf color, float radius, int id)
	{
		irr::scene::ILightSceneNode* l =
			Manager->addLightSceneNode(
				parent ? parent->get_NativeSceneNode() : 0,
				irr::NativeConverter::getNativeVector(position),
				irr::NativeConverter::getNativeColorf(color), radius, id);
		
		if (!l)
			return 0;
		return new ILightSceneNode(l);
	}


	ITerrainSceneNode* ISceneManager::AddTerrainSceneNode(
			System::String* heightMapFileName, 
			ISceneNode* parent, int id, 
			Core::Vector3D position,
			Core::Vector3D scale,
			Video::Color color	)
	{
		char* strName = 
			(char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(heightMapFileName); 

		irr::scene::ITerrainSceneNode* t = 
			Manager->addTerrainSceneNode( strName,
				parent ? parent->get_NativeSceneNode() : 0,
				id, 
				irr::NativeConverter::getNativeVector(position),
				irr::core::vector3df(0,0,0),
				irr::NativeConverter::getNativeVector(scale),
				color.color );

		System::Runtime::InteropServices::Marshal::FreeHGlobal(strName);
		
		return t ? new ITerrainSceneNode(t) : 0;
	}


	IBillboardSceneNode* ISceneManager::AddBillboardSceneNode(ISceneNode* parent,
		Core::Dimension2Df size, Core::Vector3D position, int id)
	{
		irr::scene::IBillboardSceneNode* b =
			Manager->addBillboardSceneNode(
			parent ? parent->get_NativeSceneNode() : 0,
			irr::NativeConverter::getNativeDim(size),
			irr::NativeConverter::getNativeVector(position),
			id);
		if (!b)
			return 0;
		return new IBillboardSceneNode(b);
	}

	ISceneNode* ISceneManager::AddSkyBoxSceneNode(Video::ITexture* top, Video::ITexture* bottom,
			Video::ITexture* left, Video::ITexture* right, Video::ITexture* front,
			Video::ITexture* back, ISceneNode* parent, int id)
	{
		irr::scene::ISceneNode* n =
			Manager->addSkyBoxSceneNode(
			top ? top->get_NativeTexture() : 0,
			bottom ? bottom->get_NativeTexture() : 0,
			left ? left->get_NativeTexture() : 0,
			right ? right->get_NativeTexture() : 0,
			front ? front->get_NativeTexture() : 0,
			back ? back->get_NativeTexture() : 0,
			parent ? parent->get_NativeSceneNode() : 0,
			id);
		if (!n)
			return 0;
		return new ISceneNode(n);
	}

	IParticleSystemSceneNode* ISceneManager::AddParticleSystemSceneNode(
			 bool withDefaultEmitter, ISceneNode* parent, int id,
			 Core::Vector3D position,
			 Core::Vector3D rotation,
			 Core::Vector3D scale )
	{
		irr::scene::IParticleSystemSceneNode* node = Manager->addParticleSystemSceneNode(
			withDefaultEmitter, parent ? parent->get_NativeSceneNode() : 0,
			id, irr::NativeConverter::getNativeVector( position ),
			irr::NativeConverter::getNativeVector( rotation ),
			irr::NativeConverter::getNativeVector( scale ) );

		if (!node)
			return 0;

		return new IParticleSystemSceneNode( node );
	}


	ISceneNode* ISceneManager::AddEmptySceneNode(ISceneNode* parent, int id)
	{
		irr::scene::ISceneNode* n = 
			Manager->addEmptySceneNode(
				parent ? parent->get_NativeSceneNode() : 0,
				id);
		if (!n)
			return 0;
		return new ISceneNode(n);
	}

	
	ISceneNode* ISceneManager::AddTextSceneNode(GUI::IGUIFont* font, System::String* text,
			Video::Color color, ISceneNode* parent,	const Core::Vector3D position, int id)
	{
		char* strName = 
			(char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(text); 

		irr::scene::ISceneNode* n = 
			Manager->addTextSceneNode(
				font ? font->get_NativeFont() : 0, irr::core::stringw(strName).c_str(), 
				irr::NativeConverter::getNativeColor(color), 
				parent ? parent->get_NativeSceneNode() : 0,
				irr::NativeConverter::getNativeVector(position), id);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(strName);

		if (!n)
			return 0;

		return new ISceneNode(n);
	}

	ISceneNodeAnimator* ISceneManager::CreateRotationAnimator(Core::Vector3D rotationPerSecond)
	{
		irr::scene::ISceneNodeAnimator* anim =
			Manager->createRotationAnimator(irr::NativeConverter::getNativeVector(rotationPerSecond));

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}

	ISceneNodeAnimator* ISceneManager::CreateFlyCircleAnimator( Core::Vector3D center, 
		float radius, float speed)
	{
		irr::scene::ISceneNodeAnimator* anim =
			Manager->createFlyCircleAnimator(
				irr::NativeConverter::getNativeVector(center), radius, speed);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}

	ISceneNodeAnimator* ISceneManager::CreateFlyStraightAnimator( Core::Vector3D startPoint,
		Core::Vector3D endPoint, int timeForWay, bool loop)
	{
		irr::scene::ISceneNodeAnimator* anim =
			Manager->createFlyStraightAnimator(
				irr::NativeConverter::getNativeVector(startPoint),
				irr::NativeConverter::getNativeVector(endPoint),
				timeForWay, loop);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}

	ISceneNodeAnimator* ISceneManager::CreateTextureAnimator( Video::ITexture* textures[],
			int timePerFrame, bool loop)
	{
		irr::core::array<irr::video::ITexture*> tex;
		for (int i=0; i<textures->Count; ++i)
			tex.push_back(textures[i] == 0 ? 0 : textures[i]->get_NativeTexture());

		irr::scene::ISceneNodeAnimator* anim =
			Manager->createTextureAnimator(tex, timePerFrame, loop);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}

	ISceneNodeAnimator* ISceneManager::CreateDeleteAnimator(int timeMs)
	{
		irr::scene::ISceneNodeAnimator* anim =
			Manager->createDeleteAnimator(timeMs);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}


	ISceneNodeAnimator* ISceneManager::CreateCollisionResponseAnimator(
			ITriangleSelector* world, ISceneNode* sceneNode, 
			Core::Vector3D ellipsoidRadius,
			Core::Vector3D gravityPerSecond,
			Core::Vector3D ellipsoidTranslation,
			float slidingValue)
	{
		if (!world || !sceneNode)
			return 0;

		irr::scene::ISceneNodeAnimator* anim =
			Manager->createCollisionResponseAnimator(world->get_NativeTriangleSelector(),
				sceneNode->get_NativeSceneNode(), 
				irr::NativeConverter::getNativeVector(ellipsoidRadius),
				irr::NativeConverter::getNativeVector(gravityPerSecond),
				irr::NativeConverter::getNativeVector(ellipsoidTranslation),
				slidingValue);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}


	ISceneNodeAnimator* ISceneManager::CreateFollowSplineAnimator(int startTime,
		Core::Vector3D points[], float speed, float tightness)
	{
		irr::core::array<irr::core::vector3df> pos;
		for (int i=0; i<points->Count; ++i)
			pos.push_back(irr::NativeConverter::getNativeVector(points[i]));

		irr::scene::ISceneNodeAnimator* anim =
			Manager->createFollowSplineAnimator(startTime, pos, speed, tightness);

		if (!anim)
			return 0;

		return new ISceneNodeAnimator(anim);
	}


	ITriangleSelector* ISceneManager::CreateTriangleSelector(IMesh* mesh, ISceneNode* node)
	{
		if (!mesh || !node)
			return 0;

		irr::scene::ITriangleSelector* s = Manager->createTriangleSelector(mesh->get_NativeMesh(),
			node->get_NativeSceneNode());
		if (!s)
			return 0;

		return new ITriangleSelector(s);
	}

	ITriangleSelector* ISceneManager::CreateTriangleSelectorFromBoundingBox(ISceneNode* node)
	{
		if (!node)
			return 0;

		irr::scene::ITriangleSelector* s = Manager->createTriangleSelectorFromBoundingBox(
			node->get_NativeSceneNode());
		if (!s)
			return 0;

		return new ITriangleSelector(s);
	}

	ITriangleSelector* ISceneManager::CreateOctTreeTriangleSelector(IMesh* mesh,
			ISceneNode* node, int minimalPolysPerNode)
	{
		if (!mesh || !node)
			return 0;

		irr::scene::ITriangleSelector* s = Manager->createOctTreeTriangleSelector(mesh->get_NativeMesh(),
			node->get_NativeSceneNode(), minimalPolysPerNode);
		if (!s)
			return 0;

		return new ITriangleSelector(s);
	}

	IMetaTriangleSelector* ISceneManager::CreateMetaTriangleSelector()
	{
		irr::scene::IMetaTriangleSelector* ms = Manager->createMetaTriangleSelector();
		if (!ms)
			return 0;

		return new IMetaTriangleSelector(ms);
	}

	ITriangleSelector* ISceneManager::CreateTerrainTriangleSelector(
			ITerrainSceneNode* node, int LOD)
	{
		irr::scene::ITriangleSelector* ms = Manager->createTerrainTriangleSelector(
			node ? (irr::scene::ITerrainSceneNode*)node->get_NativeSceneNode() : 0, LOD);

		if (!ms)
			return 0;

		return new ITriangleSelector(ms);
	}

	ISceneCollisionManager* ISceneManager::get_SceneCollisionManager()
	{
		return SCM;
	}

	ICameraSceneNode* ISceneManager::get_ActiveCamera()
	{
		irr::scene::ICameraSceneNode* active = Manager->getActiveCamera();
		return active ? new ICameraSceneNode(active) : 0;
	}

	void ISceneManager::set_ActiveCamera(ICameraSceneNode* camera)
	{
		irr::scene::ICameraSceneNode* node = 0;
		if (camera)
			node = camera->get_NativeCameraSceneNode();

		Manager->setActiveCamera(node);
	}

	IAnimatedMesh* ISceneManager::AddHillPlaneMesh(System::String* name, 
		Core::Dimension2Df tileSize, Core::Dimension2D tileCount, 
		Video::Material material, float hillHeight, Core::Dimension2Df countHills, 
		Core::Dimension2Df textureRepeatCount ) 
	{ 
		char* strName = 
			(char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(name); 

		irr::scene::IAnimatedMesh* a = 
			Manager->addHillPlaneMesh(strName, 
					irr::NativeConverter::getNativeDim(tileSize), 
					irr::NativeConverter::getNativeDim(tileCount),
					&irr::NativeConverter::getNativeMaterial(material), 
					hillHeight,
					irr::NativeConverter::getNativeDim(countHills), 
					irr::NativeConverter::getNativeDim(textureRepeatCount)); 

		System::Runtime::InteropServices::Marshal::FreeHGlobal(strName);

		return a ? new IAnimatedMesh(a) : 0;  
	}

	
	void ISceneManager::set_ShadowColor(Video::Color color)
	{
		Manager->setShadowColor(color.color);
	}

	Video::Color ISceneManager::get_ShadowColor()
	{
		return Video::Color(Manager->getShadowColor().color);
	}

	Irrlicht::Scene::IMeshManipulator* ISceneManager::get_MeshManipulator()
	{
		return Manipulator;
	}

	void ISceneManager::Clear()
	{
		Manager->clear();
	}

	ISceneNode* ISceneManager::get_RootSceneNode()
	{
		return Root;
	}


	void ISceneManager::LoadScene(System::String* filename)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);

		Manager->loadScene(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}


	void ISceneManager::SaveScene(System::String* filename)
	{
		char* str = (char*)(void*)
			System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(filename);

		Manager->saveScene(str);

		System::Runtime::InteropServices::Marshal::FreeHGlobal(str);
	}


}
}
