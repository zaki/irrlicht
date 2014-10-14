// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#ifndef I_GUI_PROFILER_H_INCLUDED__
#define I_GUI_PROFILER_H_INCLUDED__

#include "IGUIElement.h"

namespace irr
{
namespace gui
{
	class IGUIFont;

	//! Element to display profiler information
	class IGUIProfiler : public IGUIElement
	{
	public:
		//! constructor
		IGUIProfiler(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_PROFILER, environment, parent, id, rectangle)
		{}

		//! Show first page of profile data
		/** \param includeOverview When true show the group-overview page, when false show the profile data of the first group */
		virtual void firstPage(bool includeOverview=true) = 0;

		//! Show next page of profile data
		/** \param includeOverview Include the group-overview page  */
		virtual void nextPage(bool includeOverview=true) = 0;

		//! Show previous page of profile data
		/** \param includeOverview Include the group-overview page  */
		virtual void previousPage(bool includeOverview=true) = 0;

		//! Don't display stats for data which never got called
		virtual void setIgnoreUncalled(bool ignore) = 0;

		//! Check if we display stats for data which never got called
		virtual bool getIgnoreUncalled() const = 0;

		//! Sets another skin independent font.
		/** If this is set to zero, the button uses the font of the skin.
		\param font: New font to set. */
		virtual void setOverrideFont(IGUIFont* font=0) = 0;

		//! Gets the override font (if any)
		/** \return The override font (may be 0) */
		virtual IGUIFont* getOverrideFont(void) const = 0;

		//! Get the font which is used right now for drawing
		/** Currently this is the override font when one is set and the
		font of the active skin otherwise */
		virtual IGUIFont* getActiveFont() const = 0;

		//! Sets whether to draw the background. By default disabled,
		virtual void setDrawBackground(bool draw) = 0;

		//! Checks if background drawing is enabled
		/** \return true if background drawing is enabled, false otherwise */
		virtual bool isDrawBackgroundEnabled() const = 0;
	};

} // end namespace gui
} // end namespace irr

#endif
