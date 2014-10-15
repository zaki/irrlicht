// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h
// Written by Michael Zeilfelder

#ifndef C_GUI_PROFILER_H_INCLUDED__
#define C_GUI_PROFILER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_GUI_

#include "IGUIProfiler.h"

namespace irr
{

class IProfiler;
struct SProfileData;

namespace gui
{
	class IGUITable;

	//! Element to display profiler information
	class CGUIProfiler : public IGUIProfiler
	{
	public:
		//! constructor
		CGUIProfiler(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle, IProfiler* profiler = NULL);

		//! Show first page of profile data
		virtual void firstPage(bool includeOverview) _IRR_OVERRIDE_;

		//! Show next page of profile data
		virtual void nextPage(bool includeOverview) _IRR_OVERRIDE_;

		//! Show previous page of profile data
		virtual void previousPage(bool includeOverview) _IRR_OVERRIDE_;

		//! Don't display stats for data which never got called
		/** Default is false */
		virtual void setIgnoreUncalled(bool ignore) _IRR_OVERRIDE_;

		//! Check if we display stats for data which never got called
		virtual bool getIgnoreUncalled() const _IRR_OVERRIDE_;

		//! Sets another skin independent font.
		virtual void setOverrideFont(IGUIFont* font=0) _IRR_OVERRIDE_;

		//! Gets the override font (if any)
		virtual IGUIFont* getOverrideFont() const _IRR_OVERRIDE_;

		//! Get the font which is used right now for drawing
		virtual IGUIFont* getActiveFont() const _IRR_OVERRIDE_;

		//! Sets whether to draw the background. By default disabled,
		virtual void setDrawBackground(bool draw) _IRR_OVERRIDE_;

		//! Checks if background drawing is enabled
		/** \return true if background drawing is enabled, false otherwise */
		virtual bool isDrawBackgroundEnabled() const _IRR_OVERRIDE_;

		//! Allows to freeze updates which makes it easier to read the numbers
		virtual void setFrozen(bool freeze) _IRR_OVERRIDE_;

		//! Are updates currently frozen
		virtual bool getFrozen() const _IRR_OVERRIDE_;


		virtual IGUIElement* getElementFromPoint(const core::position2d<s32>& point) _IRR_OVERRIDE_
		{
			// This element should never get focus from mouse-clicks
			return 0;
		}

		virtual void draw() _IRR_OVERRIDE_;

	protected:

		void updateDisplay();
		void fillRow(u32 rowIndex, const SProfileData& data, bool overviewTitle, bool groupTitle);
		void rebuildColumns();

		IProfiler * Profiler;
		irr::gui::IGUITable* DisplayTable;
		irr::u32 CurrentGroupIdx;
		irr::s32 CurrentGroupPage;
		irr::s32 NumGroupPages;
		bool IgnoreUncalled;
		bool DrawBackground;
		bool Frozen;
		bool UnfreezeOnce;
	};

} // end namespace gui
} // end namespace irr

#endif // _IRR_COMPILE_WITH_GUI_

#endif // __C_GUI_IMAGE_H_INCLUDED__
