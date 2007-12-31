// Copyright (C) 2002-2005 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// 07.10.2005 - Multicolor-Listbox addet by A. Buschhueter (Acki)
//                                          A_Buschhueter@gmx.de

#ifndef __C_GUI_TABLE_BAR_H_INCLUDED__
#define __C_GUI_TABLE_BAR_H_INCLUDED__

#include "IGUITable.h"
#include "irrArray.h"

namespace irr
{
namespace gui
{

	class IGUIFont;
	class IGUIScrollBar;

	class CGUITable : public IGUITable
	{
	public:
		//! constructor
		CGUITable(IGUIEnvironment* environment, IGUIElement* parent,
			s32 id, core::rect<s32> rectangle, bool clip=true,
			bool drawBack=false, bool moveOverSelect=true);

		//! destructor
		~CGUITable();

		//! Adds a column
		virtual void addColumn(const wchar_t* caption, s32 id=-1);

		//! Returns the number of columns in the table control
		virtual s32 getColumnCount() const;

		//! Makes a column active. This will trigger an ordering process.
		/** \param idx: The id of the column to make active.
			\return Returns true if successful. */
		virtual bool setActiveColumn(s32 idx);

		//! Returns which header is currently active
		virtual s32 getActiveColumn() const;

		//! Returns the ordering used by the currently active column
		virtual EGUI_ORDERING_MODE getActiveColumnOrdering() const;

		//! set a column width
		virtual void setColumnWidth(u32 columnIndex, u32 width);

		//! This tells the table control whether is should send an
		//! EGET_TABLE_HEADER_CHANGED message or not when a column
		//! header is clicked. If set to false, the table control will
		//! use a default alphabetical ordering scheme.
		/** \param columnIndex: The index of the column header.
		\param state: If true, an EGET_TABLE_HEADER_CHANGED message will be sent.*/
		virtual void setColumnCustomOrdering(u32 columnIndex, bool state);

		//! Returns which row is currently selected
		virtual s32 getSelected() const;

		//! Returns amount of rows in the tabcontrol
		virtual s32 getRowCount() const;

		//! adds a row to the table
		/** \param rowIndex: zero based index of rows. The row will be
			inserted at this position. If a row already exists
			there, it will be placed after it. If the row is larger
			than the actual number of rows by more than one, it
			won't be created. Note that if you create a row that is
			not at the end, there might be performance issues*/
		virtual void addRow(u32 rowIndex);

		//! Remove a row from the table
		virtual void removeRow(u32 rowIndex);

		//! clear the table rows, but keep the columns intact
		virtual void clearRows();

		//! Swap two row positions. This is useful for a custom ordering algo.
		virtual void swapRows(u32 rowIndexA, u32 rowIndexB);

		//! This tells the table to start ordering all the rows. You
		//! need to explicitly tell the table to reorder the rows when
		//! a new row is added or the cells data is changed. This makes
		//! the system more flexible and doesn't make you pay the cost
		//! of ordering when adding a lot of rows.
		virtual void orderRows();

		//! Set the text of a cell
		virtual void setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text);

		//! Set the text of a cell, and set a color of this cell.
		virtual void setCellText(u32 rowIndex, u32 columnIndex, const wchar_t* text, video::SColor color);

		//! Set the data of a cell
		virtual void setCellData(u32 rowIndex, u32 columnIndex, void *data);

		//! Set the color of a cell text
		virtual void setCellColor(u32 rowIndex, u32 columnIndex, video::SColor color);

		//! Get the text of a cell
		virtual const wchar_t* getCellText(u32 rowIndex, u32 columnIndex ) const;

		//! Get the data of a cell
		virtual void* getCellData(u32 rowIndex, u32 columnIndex ) const;

		//! clears the table, deletes all items in the table
		virtual void clear();

		//! called if an event happened.
		virtual bool OnEvent(const SEvent& event);

		//! draws the element and its children
		virtual void draw();

		//! Writes attributes of the object.
		//! Implement this to expose the attributes of your scene node animator for
		//! scripting languages, editors, debuggers or xml serialization purposes.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const;

		//! Reads attributes of the object.
		//! Implement this to set the attributes of your scene node animator for
		//! scripting languages, editors, debuggers or xml deserialization purposes.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0);

	private:

		struct Cell
		{
			core::stringw text;
			core::stringw BrokenText;
			video::SColor color;
			void *data;
		};

		struct Row
		{
			core::array<Cell> Items;
			u32 height;
		};

		struct Column
		{
			core::stringw name;
			video::SColor TextColor;
			u32 width;
			bool useCustomOrdering;
		};

		void breakText(core::stringw &text, u32 cellWidth);
		void selectNew(s32 ypos, bool onlyHover=false);
		bool selectColumnHeader(s32 xpos, s32 ypos);
		void recalculate();

		core::array< Column > Columns;
		core::array< Row > Rows;
		gui::IGUIFont* Font;
		gui::IGUIScrollBar* ScrollBar;
		bool Clip;
		bool DrawBack;
		bool MoveOverSelect;
		bool Selecting;

		s32 ItemHeight;
		s32 TotalItemHeight;
		s32 Selected;
		s32 CellHeightPadding;
		s32 CellWidthPadding;
		s32 ActiveTab;
		EGUI_ORDERING_MODE CurrentOrdering;
	};

} // end namespace gui
} // end namespace irr

#endif

