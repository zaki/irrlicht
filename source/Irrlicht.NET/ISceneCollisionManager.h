// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;
using namespace System::Runtime::InteropServices;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Line3D.h"
#include "Triangle3D.h"
#include "ITriangleSelector.h"
#include "Position2D.h"
#include "ICameraSceneNode.h"

namespace Irrlicht
{
namespace Scene
{

/// <summary>
/// The Scene Collision Manager provides methods for performing collision tests and 
///	picking on scene nodes.
/// </summary>
public __gc class ISceneCollisionManager
{
public:

	/// <summary>
	/// Creates a scm from a native C++ scene node. Don't use this, its better to 
	/// get access to the ISceneCollisionManager via ISceneManager.SceneCollisionManager.
	/// </summary>
	ISceneCollisionManager(irr::scene::ISceneCollisionManager* realSCM);

	/// <summary>
	/// Finds the collision point of a line and lots of triangles, if there is one.
	/// </summary>
	/// <param name="ray"> Line with witch collisions are tested.</param>
	/// <param name="selector"> TriangleSelector containing the triangles. It can
	/// be created for example using ISceneManager::createTriangleSelector() or 
	/// ISceneManager::createTriangleOctTreeSelector().</param>
	/// <param name="outCollisionPoint"> If a collision is detected, this will contain the
	/// position of the nearest collision.</param>
	/// <param name="outTriangle"> If a collision is detected, this will contain the triangle
	/// with which the ray collided.</param>
	/// <returns> Returns true if a collision was detected and false if not.</returns>
	bool GetCollisionPoint(Core::Line3D ray,
		Scene::ITriangleSelector* selector, 
		[PARAMFLAG::Out] Core::Vector3D& outCollisionPoint,
		[PARAMFLAG::Out] Core::Triangle3D& outTriangle);

	/// <summary>
	/// Collides a moving ellipsoid with a 3d world with gravity and returns
	/// the resulting new position of the ellipsoid. This can be used for moving
	/// a character in a 3d world: The character will slide at walls and is able
	/// to walk up stairs. The method used how to calculate the collision result
	/// position is based on the paper "Improved Collision detection and Response"
	/// by Kasper Fauerby.
	/// </summary>
	/// <param name="selector"> TriangleSelector containing the triangles of the world.
	/// It can be created for example using ISceneManager::createTriangleSelector() or 
	/// ISceneManager::createTriangleOctTreeSelector().</param>
	/// <param name="ellipsoidPosition"> Position of the ellipsoid.</param>
	/// <param name="ellipsoidRadius"> Radius of the ellipsoid.</param>
	/// <param name="ellipsoidDirectionAndSpeed"> Direction and speed of 
	/// the movement of the ellipsoid.</param>
	/// <param name="gravityDirectionAndSpeed"> Direction and force of gravity.</param>
	/// <param name="triout"> Optional parameter where the last triangle causing a 
	/// collision is stored, if there is a collision.</param>
	/// <param name="outFalling"> Is set to true if the ellipsoid is falling down, caused
	/// by gravity.</param>
	/// <param name="slidingSpeed">Set this to a small value lile 0.0005</param>
	/// <returns> Returns the new position of the ellipsoid.</returns>
	Core::Vector3D GetCollisionResultPosition(
		Scene::ITriangleSelector* selector,
		Core::Vector3D ellipsoidPosition,
		Core::Vector3D ellipsoidRadius, 
		Core::Vector3D ellipsoidDirectionAndSpeed,
		[PARAMFLAG::Out] Core::Triangle3D& triout,
		[PARAMFLAG::Out] bool& outFalling,
		float slidingSpeed,
		Core::Vector3D gravityDirectionAndSpeed);

	/// <summary>
	/// Returns a 3d ray which would go through the 2d screen coodinates.
	/// </summary>
	/// <param name="pos"> Screen coordinates in pixels.</param>
	/// <param name="camera"> Camera from which the ray starts. If null, the
	/// active camera is used.</param> 
	/// <returns> Returns a ray starting from the position of the camera
	/// and ending at a lenght of the far value of the camera at a position
	/// which would be behind the 2d screen coodinates.</returns>
	Core::Line3D GetRayFromScreenCoordinates(
		Core::Position2D pos, Scene::ICameraSceneNode* camera);

	/// <summary>
	/// Calculates 2d screen position from a 3d position.
	/// </summary>
	/// <param name="pos"> 3D position in world space to be transformed into
	/// 2d.</param>
	/// <param name="camera"> Camera to be used. If null, the currently active
	/// camera is used.</param>
	/// <returns> Returns the 2d screen coordinates which a object in the 
	/// 3d world would have if it would be rendered to the screen. If the 
	/// 3d position is behind the camera, it is set to (-10000,-10000). In
	/// most cases you can ignore this fact, because if you use this method
	/// for drawing a decorator over a 3d object, it will be clipped by the
	/// screen borders.</returns>
	Core::Position2D GetScreenCoordinatesFrom3DPosition(
		Core::Vector3D pos, Scene::ICameraSceneNode* camera);

	/// <summary>
	/// Returns the scene node, which is currently visible under the overgiven 
	/// screencoordinates, viewed from the currently active camera. The collision
	/// tests are done using a bounding box for each scene node.
	/// </summary>
	/// <param name="pos"> Position in pixel screen coordinates, under which the returned
	/// scene node will be.</param>
	/// <param name="idBitMask"> Only scene nodes with an id with bits set like in this mask
	/// will be tested. If the BitMask is 0, this feature is disabled.</param>
	/// <returns> Returns the visible scene node under screen coordinates with matching
	/// bits in its id. If there is no scene node under this position, 0 is returned.</returns>
	Scene::ISceneNode* GetSceneNodeFromScreenCoordinatesBB(Core::Position2D pos,
		int idBitMask, bool noDebugObjects);

	/// <summary>
	/// Returns the nearest scene node which collides with a 3d ray and 
	/// which id matches a bitmask. The collision tests are done using a bounding
	/// box for each scene node.
	/// </summary>
	/// <param name="ray"> Line with witch collisions are tested.</param>
	/// <param name="idBitMask"> Only scene nodes with an id with bits set like in this mask
	/// will be tested. If the BitMask is 0, this feature is disabled.</param>
	/// <returns> Returns the scene node nearest to ray.start, which collides with the
	/// ray and matches the idBitMask, if the mask is not null. If no scene
	/// node is found, 0 is returned.</returns>
	Scene::ISceneNode* GetSceneNodeFromRayBB(Core::Line3D ray, 
		int idBitMask, bool noDebugObjects);

	/// <summary>
	/// Returns the scene node, at which the overgiven camera is looking at and
	/// which id matches the bitmask. A ray is simply casted from the position
	/// of the camera to the view target position, and all scene nodes are tested
	/// against this ray. The collision tests are done using a bounding
	/// box for each scene node.
	/// </summary>	
	/// <param name="camera"> Camera from which the ray is casted.</param>
	/// <param name="idBitMaks"> Only scene nodes with an id with bits set like in this mask
	/// will be tested. If the BitMask is 0, this feature is disabled.</param>
	/// <returns> Returns the scene node nearest to the camera, which collides with the
	/// ray and matches the idBitMask, if the mask is not null. If no scene
	/// node is found, 0 is returned.</returns>
	Scene::ISceneNode* GetSceneNodeFromCameraBB(Scene::ICameraSceneNode* camera,
		int idBitMask, bool noDebugObjects);

protected:

	irr::scene::ISceneCollisionManager* SCM;
};


}
}