// Copyright (C) 2003-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_GUI_TABLE_H_INCLUDED__
#define __I_GUI_TABLE_H_INCLUDED__

#include "IGUIElement.h"
#include "irrTypes.h"
#include "SColor.h"
#include "IGUISkin.h"

namespace irr
{
namespace gui
{

    enum EGUI_ORDERING_MODE
	{
		//! Order the elements from the smallest to the largest.
		EGOM_ASCENDING,

		//! Order the elements from the largest to the smallest.
		EGOM_DESCENDING,

		//! this value is not used, it only specifies the amount of default ordering types
		//! available.
		EGOM_COUNT
	};

	class IGUIFont;

	//! Default list box GUI element.
	class IGUITable : public IGUIElement
	{
	public:
		//! constructor
		IGUITable(IGUIEnvironment* environment, IGUIElement* parent, s32 id, core::rect<s32> rectangle)
			: IGUIElement(EGUIET_TABLE, environment, parent, id, rectangle) {}

		//! Adds a column
		virtual void addColumn(const wchar_t* caption, s32 id=-1) = 0;

		//! Returns the number of columns in the table control
		virtual s32 getColumncount() const = 0;

		//! Makes a column active. This will trigger an ordering process.
		/** \param idx: The id of the column to make active.
		 \return Returns true if successful. */
		virtual bool setActiveColumn(s32 idx) = 0;

		//! Returns which header is currently active
		virtual s32 getActiveColumn() const = 0;

		//! Returns the ordering used by the currently active column
		virtual EGUI_ORDERING_MODE getActiveColumnOrdering() const = 0;

		//! Set the width of a column
		virtual void setColumnWidth(u32 columnIndex, u32 width) = 0;

		//! This tells the table control whether is should send a EGET_TABLE_HEADER_CHANGED message or not when
		//! a column header is clicked. If set to false, the table control will use a default alphabetical ordering scheme.
		/** \param columnIndex: The index of the column header.
		\param state: If true, a EGET_TABLE_HEADER_CHANGED message will be sent and you can order the table data as you whish.*/
		virtual void setColumnCustomOrdering(u32 columnIndex, bool state) = 0;

		//! Returns which row is currently selected
		virtual s32 getSelected() const = 0;

		//! Returns amount of rows in the tabcontrol
		virtual s32 getRowcount() const = 0;

		//! adds a row to the table
		/** \param rowIndex: zero based index of rows. The row will be inserted at this
		     position, if a row already exist there, it will be placed after it. If the row
				 is larger than the actual number of row by more than one, it won't be created.
		     Note that if you create a row that's not at the end, there might be performance issues*/
		virtual void addRow(u32 rowIndex) = 0;

		//! Remove a row from the table
		virtual void removeRow(u32 rowIndex) = 0;

		//! clears the table rows, but keeps the columns intact
		virtual void clearRows() = 0;

		//! Swap two row positions. This is useful for a custom ordering algo.
		virtual void swapRows(u32 rowIndexA, u32 rowIndexB) = 0;

		//! This tells the table to start ordering all the rows. You need to explicitly
		//! tell the table to re order the rows when a new row is added or the cells data is
		//! changed. This makes the system more flexible and doesn't make you pay the cost of
		//! ordering when adding a lot of rows.
		virtual void orderRows() = 0;

		//! Set the text of a cell
		virtual void setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text) = 0;

		//! Set the text of a cell, and set a color of this cell.
		virtual void setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text, video::SColor color) = 0;

		//! Set the data of a cell
		virtual void setCellData(u32 rowIndex, u32 columnIndex, void *data) = 0;

		//! Set the color of a cell text
		virtual void setCellColor(u32 rowIndex, u32 columnIndex, video::SColor color) = 0;

		//! Get the text of a cell
		virtual const wchar_t* getCellText(u32 rowIndex, u32 columnIndex ) const = 0;

		//! Get the data of a cell
		virtual void* getCellData(u32 rowIndex, u32 columnIndex ) const = 0;

		//! clears the table, deletes all items in the table
		virtual void clear() = 0;
	};


} // end namespace gui
} // end namespace irr

#endif

