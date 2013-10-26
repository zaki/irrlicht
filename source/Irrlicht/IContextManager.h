// Copyright (C) 2013 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __IRR_I_CONTEXT_MANAGER_H_INCLUDED__
#define __IRR_I_CONTEXT_MANAGER_H_INCLUDED__

namespace irr
{
namespace video
{
	class IContextManager : public virtual IReferenceCounted
	{
	public:
        // Initialize manager
        virtual bool initialize() =0;

        // Terminate manager
        virtual void terminate() =0;

        // Create surface.
        virtual bool createSurface() =0;

        // Destroy surface.
        virtual void destroySurface() =0;

        // Create context.
        virtual bool createContext() =0;

        // Destroy context.
        virtual void destroyContext() =0;

        // Swap buffers.
        virtual void swapBuffers() =0;
	};

} // end namespace video
} // end namespace irr


#endif
