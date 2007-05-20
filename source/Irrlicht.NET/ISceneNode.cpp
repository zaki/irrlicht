// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ISceneNode.h"
#include "ISceneNodeAnimator.h"
#include "NativeConverter.h"
#include <vcclr.h> // for PtrToStringChars

namespace Irrlicht
{
namespace Scene
{

	ISceneNode::ISceneNode(irr::scene::ISceneNode* realSceneNode)
		: SceneNode(realSceneNode)
	{
		SceneNode->grab();
	}

	ISceneNode::~ISceneNode()
	{
		SceneNode->drop();
	}

	void ISceneNode::Render()
	{
		SceneNode->render();
	}

	System::String* ISceneNode::get_Name()
	{
		return new System::String(SceneNode->getName());
	}

	void ISceneNode::set_Name(System::String* name)
	{
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(name);

		SceneNode->setName(str2);

		Marshal::FreeHGlobal(str2);
	}

	Core::Box3D ISceneNode::get_BoundingBox()
	{
		return irr::NativeConverter::getNETBox(SceneNode->getBoundingBox());
	}

	Core::Box3D ISceneNode::get_TransformedBoundingBox()
	{
		return irr::NativeConverter::getNETBox(SceneNode->getTransformedBoundingBox());
	}

	Core::Matrix4 ISceneNode::get_AbsoluteTransformation()
	{
		return irr::NativeConverter::getNETMatrix(SceneNode->getAbsoluteTransformation());
	}

	Core::Matrix4 ISceneNode::get_RelativeTransformation()
	{
		return irr::NativeConverter::getNETMatrix(SceneNode->getRelativeTransformation());
	}

	bool ISceneNode::get_Visible()
	{
		return SceneNode->isVisible();
	}

	void ISceneNode::set_Visible(bool visible)
	{
		SceneNode->setVisible(visible);
	}

	int ISceneNode::get_ID()
	{
		return SceneNode->getID();
	}

	void ISceneNode::set_ID(int id)
	{
		SceneNode->setID(id);
	}

	void ISceneNode::AddChild(ISceneNode* child)
	{
		SceneNode->addChild(child ? child->SceneNode : 0);
	}

	bool ISceneNode::RemoveChild(ISceneNode* child)
	{
		return SceneNode->removeChild(child ? child->SceneNode : 0);
	}

	void ISceneNode::RemoveAll()
	{
		return SceneNode->removeAll();
	}

	void ISceneNode::Remove()
	{
		SceneNode->remove();
	}

	Video::Material ISceneNode::GetMaterial(int i)
	{
		return irr::NativeConverter::getNETMaterial(SceneNode->getMaterial(i));
	}

	void ISceneNode::SetMaterial(int i, Video::Material mat)
	{
		SceneNode->getMaterial(i) = irr::NativeConverter::getNativeMaterial(mat);
	}

	int ISceneNode::get_MaterialCount()
	{
		return SceneNode->getMaterialCount();
	}

	void ISceneNode::SetMaterialFlag(Video::MaterialFlag flag, bool newvalue)
	{
		SceneNode->setMaterialFlag((irr::video::E_MATERIAL_FLAG)flag, newvalue);
	}

	void ISceneNode::SetMaterialTexture(int textureLayer, Video::ITexture* texture)
	{
		SceneNode->setMaterialTexture(textureLayer, texture ? texture->get_NativeTexture() : 0);
	}

	void ISceneNode::SetMaterialType(Video::MaterialType newType)
	{
		SceneNode->setMaterialType((irr::video::E_MATERIAL_TYPE)newType);
	}
	
	Core::Vector3D ISceneNode::get_Scale()
	{
		return irr::NativeConverter::getNETVector(SceneNode->getScale());
	}

	void ISceneNode::set_Scale(Core::Vector3D scale)
	{
		SceneNode->setScale(irr::NativeConverter::getNativeVector(scale));
	}

	Core::Vector3D ISceneNode::get_Rotation()
	{
		return irr::NativeConverter::getNETVector(SceneNode->getRotation());
	}

	void ISceneNode::set_Rotation(Core::Vector3D v)
	{
		SceneNode->setRotation(irr::NativeConverter::getNativeVector(v));
	}

	Core::Vector3D ISceneNode::get_Position()
	{
		return irr::NativeConverter::getNETVector(SceneNode->getPosition());
	}

	void ISceneNode::set_Position(Core::Vector3D v)
	{
		SceneNode->setPosition(irr::NativeConverter::getNativeVector(v));
	}

	Core::Vector3D ISceneNode::get_AbsolutePosition()
	{
		return irr::NativeConverter::getNETVector(SceneNode->getAbsolutePosition());
	}

	void ISceneNode::set_AutomaticCulling(bool enabled)
	{
		// TODO: culling enum
		SceneNode->setAutomaticCulling(enabled ? irr::scene::EAC_BOX : irr::scene::EAC_OFF);
	}

	bool ISceneNode::get_AutomaticCulling()
	{
		// TODO: culling enum
		return SceneNode->getAutomaticCulling() != irr::scene::EAC_OFF;
	}

	void ISceneNode::set_DebugDataVisible(bool visible)
	{
		SceneNode->setDebugDataVisible(visible ? irr::scene::EDS_FULL : irr::scene::EDS_OFF);
	}

	bool ISceneNode::get_DebugDataVisible()
	{
		return SceneNode->isDebugDataVisible() != irr::scene::EDS_OFF;
	}

	void ISceneNode::set_IsDebugObject(bool debugObject)
	{
		SceneNode->setIsDebugObject(debugObject);
	}

	bool ISceneNode::get_IsDebugObject()
	{
		return SceneNode->isDebugObject();
	}

	void ISceneNode::SetParent(ISceneNode* newParent)
	{
		SceneNode->setParent(newParent ? newParent->SceneNode : 0);
	}

	void ISceneNode::UpdateAbsolutePosition()
	{
		SceneNode->updateAbsolutePosition();
	}

	irr::scene::ISceneNode* ISceneNode::get_NativeSceneNode()
	{
		return SceneNode;
	}

	/// <summary>
	/// Adds an animator which should animate this node.
	/// </summary>
	void ISceneNode::AddAnimator(ISceneNodeAnimator* animator)
	{
		if (!animator)
			return;

		SceneNode->addAnimator(animator->get_NativeSceneNodeAnimator());
	}

	void ISceneNode::RemoveAnimators()
	{
		SceneNode->removeAnimators();
	}

	void ISceneNode::RemoveAnimator(ISceneNodeAnimator* animator)
	{
		SceneNode->removeAnimator(animator->get_NativeSceneNodeAnimator());	
	}

	SceneNodeType ISceneNode::get_SceneNodeType()
	{
		return (Irrlicht::Scene::SceneNodeType)SceneNode->getType();
	}

	ISceneNode* ISceneNode::get_Children() []
	{
		ISceneNode* ret[] = new ISceneNode*[SceneNode->getChildren().getSize()];

		int i=0; 

		const irr::core::list<irr::scene::ISceneNode*>& children = SceneNode->getChildren();

		for ( irr::core::list<irr::scene::ISceneNode*>::Iterator it = children.begin();
			  it != children.end(); ++it, ++i )
		{
			ret[i] = new ISceneNode(*it);
		}

		return ret;
    }
}
}
