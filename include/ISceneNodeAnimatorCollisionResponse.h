// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_ANIMATOR_COLLISION_RESPONSE_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_COLLISION_RESPONSE_H_INCLUDED__

#include "ISceneNode.h"

namespace irr
{
namespace scene
{

	//! Special scene node animator for doing automatic collision detection and response.
	/** This scene node animator can be attached to any scene node modifying it in that
	way, that it cannot move through walls of the world, is influenced by gravity and
	acceleration. This animator is useful for example for first person shooter 
	games. Attach it for example to a first person shooter camera, and the camera will
	behave as the player control in a first person shooter game: The camera stops and
	slides at walls, walks up stairs, falls down if there is no floor under it, and so on.
	*/
	class ISceneNodeAnimatorCollisionResponse : public ISceneNodeAnimator
	{
	public:

		//! destructor
		virtual ~ISceneNodeAnimatorCollisionResponse() {}

		//! Returns if the attached scene node is falling, which means that
		//! there is no blocking wall from the scene node in the direction of
		//! the gravity. The implementation of this method is very fast,
		//! no collision detection is done when invoking it. 
		//! \return Returns true if the scene node is falling, false if not.
		virtual bool isFalling() = 0;

		//! Sets the radius of the ellipsoid with which collision detection and
		//! response is done. If you have got a scene node, and you are unsure about
		//! how big the radius should be, you could use the following code to determine
		//! it:
		//! \code
		//! core::aabbox<f32> box = yourSceneNode->getBoundingBox();
		//! core::vector3df radius = box.MaxEdge - box.getCenter();
		//! \endcode
		//! \param radius: New radius of the ellipsoid.
		virtual void setEllipsoidRadius(const core::vector3df& radius) = 0;

		//! Returns the radius of the ellipsoid with wich the collision detection and
		//! response is done.
		//! \return Radius of the ellipsoid.
		virtual core::vector3df getEllipsoidRadius() const = 0;

		//! Sets the gravity of the environment. A good example value would be
		//! core::vector3df(0,-100.0f,0) for letting gravity affect all object to
		//! fall down. For bigger gravity, make increase the length of the vector.
		//! You can disable gravity by setting it to core::vector3df(0,0,0);
		//! \param gravity: New gravity vector.
		virtual void setGravity(const core::vector3df& gravity) = 0;

		//! Returns current vector of gravity.
		//! \return Returns the gravity vector.
		virtual core::vector3df getGravity() const = 0;

		//! By default, the ellipsoid for collision detection is created around
		//! the center of the scene node, which means that the ellipsoid surrounds
		//! it completely. If this is not what you want, you may specify a translation
		//! for the ellipsoid.
		//! \param translation: Translation of the ellipsoid relative 
		//! to the position of the scene node.
		virtual void setEllipsoidTranslation(const core::vector3df &translation) = 0;

		//! Returns the translation of the ellipsoid for collision detection. See
		//! ISceneNodeAnimatorCollisionResponse::setEllipsoidTranslation() for
		//! more details.
		//! \return Returns the tranlation of the ellipsoid relative to the position
		//! of the scene node.
		virtual core::vector3df getEllipsoidTranslation() const = 0;

		//! Sets a triangle selector holding all triangles of the world with which
		//! the scene node may collide.
		//! \param newWorld: New triangle selector containing triangles to let the
		//! scene node collide with.
		virtual void setWorld(ITriangleSelector* newWorld) = 0;

		//! Returns the current triangle selector containing all triangles for
		//! collision detection.
		virtual ITriangleSelector* getWorld() const = 0;
	};
} // end namespace scene
} // end namespace irr

#endif

