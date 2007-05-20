// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "ISceneCollisionManager.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	ISceneCollisionManager::ISceneCollisionManager(irr::scene::ISceneCollisionManager* scm)
		: SCM(scm)
	{
	}

	bool ISceneCollisionManager::GetCollisionPoint(Core::Line3D ray,
		Scene::ITriangleSelector* selector, 
		[PARAMFLAG::Out] Core::Vector3D& outCollisionPoint,
		[PARAMFLAG::Out] Core::Triangle3D& outTriangle)
	{
		irr::core::vector3df outCP;
		irr::core::triangle3df outTr;

		bool ret = SCM->getCollisionPoint(
			irr::NativeConverter::getNativeLine(ray),
			selector ? selector->get_NativeTriangleSelector() : 0,
			outCP,
			outTr);

		outCollisionPoint = irr::NativeConverter::getNETVector(outCP);
		outTriangle = irr::NativeConverter::getNETTriangle(outTr);

		return ret;
	}

	Core::Vector3D ISceneCollisionManager::GetCollisionResultPosition(
		Scene::ITriangleSelector* selector,
		Core::Vector3D ellipsoidPosition,
		Core::Vector3D ellipsoidRadius, 
		Core::Vector3D ellipsoidDirectionAndSpeed,
		[PARAMFLAG::Out] Core::Triangle3D& triout,
		[PARAMFLAG::Out] bool& outFalling,
		float slidingSpeed,
		Core::Vector3D gravityDirectionAndSpeed)
	{
		bool outF;
		irr::core::triangle3df outTr;

		irr::core::vector3df ret = SCM->getCollisionResultPosition(
			selector ? selector->get_NativeTriangleSelector() : 0,
			irr::NativeConverter::getNativeVector(ellipsoidPosition),
			irr::NativeConverter::getNativeVector(ellipsoidRadius),
			irr::NativeConverter::getNativeVector(ellipsoidDirectionAndSpeed),
			outTr,
			outF,
			slidingSpeed,
			irr::NativeConverter::getNativeVector(gravityDirectionAndSpeed));

		triout = irr::NativeConverter::getNETTriangle(outTr);
		outFalling = outF;
		return irr::NativeConverter::getNETVector(ret);
	}

	Core::Line3D ISceneCollisionManager::GetRayFromScreenCoordinates(
		Core::Position2D pos, Scene::ICameraSceneNode* camera)
	{
		return irr::NativeConverter::getNETLine(
			SCM->getRayFromScreenCoordinates(
			irr::NativeConverter::getNativePos(pos), 
			camera ? camera->get_NativeCameraSceneNode(): 0));
	}

	Core::Position2D ISceneCollisionManager::GetScreenCoordinatesFrom3DPosition(
		Core::Vector3D pos, Scene::ICameraSceneNode* camera)
	{
		irr::core::position2d<irr::s32> p = SCM->getScreenCoordinatesFrom3DPosition(
			irr::NativeConverter::getNativeVector(pos),
			camera ? camera->get_NativeCameraSceneNode(): 0);
		return Core::Position2D(p.X, p.Y);
	}

	Scene::ISceneNode* ISceneCollisionManager::GetSceneNodeFromScreenCoordinatesBB(Core::Position2D pos,
		int idBitMask, bool noDebugObjects)
	{
		irr::scene::ISceneNode* node = SCM->getSceneNodeFromScreenCoordinatesBB(
			irr::NativeConverter::getNativePos(pos), idBitMask, noDebugObjects);

		if (!node)
			return 0;

		return new ISceneNode(node);
	}

	Scene::ISceneNode* ISceneCollisionManager::GetSceneNodeFromRayBB(Core::Line3D ray, 
		int idBitMask, bool noDebugObjects)
	{
		irr::scene::ISceneNode* node = SCM->getSceneNodeFromRayBB(
			irr::NativeConverter::getNativeLine(ray), idBitMask, noDebugObjects);

		if (!node)
			return 0;

		return new ISceneNode(node);
	}

	Scene::ISceneNode* ISceneCollisionManager::GetSceneNodeFromCameraBB(
		Scene::ICameraSceneNode* camera, int idBitMask, bool noDebugObjects)
	{
		irr::scene::ISceneNode* node = SCM->getSceneNodeFromCameraBB(
			camera ? camera->get_NativeCameraSceneNode(): 0, idBitMask, noDebugObjects);

		if (!node)
			return 0;

		return new ISceneNode(node);
	}

}
}