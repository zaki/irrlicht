// Copyright (C) 2002-2007 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_MATERIAL_LAYER_H_INCLUDED__
#define __S_MATERIAL_LAYER_H_INCLUDED__

#include "matrix4.h"

namespace irr
{
namespace video
{
	class ITexture;

	//! Texture coord clamp mode outside [0.0, 1.0]
	enum E_TEXTURE_CLAMP
	{
		//! Texture repeats
		ETC_REPEAT = 0,
		//! Texture is clamped to the last pixel
		ETC_CLAMP,
		//! Texture is clamped to the edge pixel
		ETC_CLAMP_TO_EDGE,
		//! Texture is clamped to the border pixel (if exists)
		ETC_CLAMP_TO_BORDER,
		//! Texture is alternatingly mirrored (0..1..0..1..0..)
		ETC_MIRROR
	};
	static const char* const aTextureClampNames[] = {
			"texture_clamp_repeat",
			"texture_clamp_clamp",
			"texture_clamp_clamp_to_edge",
			"texture_clamp_clamp_to_border",
			"texture_clamp_mirror", 0};

	//! struct for holding material parameters which exist per texture layer
	class SMaterialLayer
	{
	public:
		//! default constructor
		SMaterialLayer()
			: Texture(0), TextureMatrix(0),
				TextureWrap(ETC_REPEAT),
				BilinearFilter(true),
				TrilinearFilter(false),
				AnisotropicFilter(false)
			{}

		//! copy constructor
		SMaterialLayer(const SMaterialLayer& other)
		{
			// This pointer is checked during assignment
			TextureMatrix = 0;
			*this = other;
		}

		//! destructor
		~SMaterialLayer()
		{
			delete TextureMatrix;
		}

		//! Assignment operator
		SMaterialLayer& operator=(const SMaterialLayer& other)
		{
			Texture = other.Texture;
			if (TextureMatrix)
			{
				if (other.TextureMatrix)
					*TextureMatrix = *other.TextureMatrix;
				else
				{
					delete TextureMatrix;
					TextureMatrix = 0;
				}
			}
			else
			{
				if (other.TextureMatrix)
					TextureMatrix = new core::matrix4(*other.TextureMatrix);
				else
					TextureMatrix = 0;
			}
			TextureWrap = other.TextureWrap;
			BilinearFilter = other.BilinearFilter;
			TrilinearFilter = other.TrilinearFilter;
			AnisotropicFilter = other.AnisotropicFilter;

			return *this;
		}

		//! Texture
		ITexture* Texture;

		//! Texture Matrix
		//! Do not acces this element directly as the internal
		//! ressource management has to cope with Null pointers etc.
		core::matrix4* TextureMatrix;

		//! Texture Clamp Mode
		E_TEXTURE_CLAMP TextureWrap;

		//! Is bilinear filtering enabled? Default: true
		bool BilinearFilter;

		//! Is trilinear filtering enabled? Default: false
		/** If the trilinear filter flag is enabled,
		the bilinear filtering flag is ignored. */
		bool TrilinearFilter;

		//! Is anisotropic filtering enabled? Default: false
		/** In Irrlicht you can use anisotropic texture filtering
		    in conjunction with bilinear or trilinear texture
		    filtering to improve rendering results. Primitives
		    will look less blurry with this flag switched on. */
		bool AnisotropicFilter;

		//! Gets the texture transformation matrix
		core::matrix4& getTextureMatrix()
		{
			if (!TextureMatrix)
				TextureMatrix = new core::matrix4(core::matrix4::EM4CONST_IDENTITY);
			return *TextureMatrix;
		}

		//! Gets the immutable texture transformation matrix
		const core::matrix4& getTextureMatrix() const
		{
			if (TextureMatrix)
				return *TextureMatrix;
			else
				return core::IdentityMatrix;
		}

		//! Sets the texture transformation matrix to mat
		void setTextureMatrix(const core::matrix4& mat)
		{
			if (!TextureMatrix)
				TextureMatrix = new core::matrix4(mat);
			else
				*TextureMatrix = mat;
		}

		//! Inequality operator
		inline bool operator!=(const SMaterialLayer& b) const
		{
			bool different = 
				Texture != b.Texture ||
				TextureWrap != b.TextureWrap ||
				BilinearFilter != b.BilinearFilter ||
				TrilinearFilter != b.TrilinearFilter ||
				AnisotropicFilter != b.AnisotropicFilter;
			if (different)
				return true;
			else
				different |= (TextureMatrix != b.TextureMatrix);
			return different;
		}

		//! Equality operator
		inline bool operator==(const SMaterialLayer& b) const
		{ return !(b!=*this); }
	};

} // end namespace video
} // end namespace irr

#endif // __S_MATERIAL_LAYER_H_INCLUDED__

