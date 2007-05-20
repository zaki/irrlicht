// Copyright (C) 2002-2006 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#using <mscorlib.dll>
using namespace System;

#pragma unmanaged
#include "..\\..\\include\\irrlicht.h"
#pragma managed

#include "Color.h"
#include "Vector3D.h"
#include "Matrix4.h"
#include "ITexture.h"
#include "Material.h"
#include "Position2D.h"
#include "Rect.h"
#include "Vertex3D.h"
#include "Triangle3D.h"
#include "Box3D.h"
#include "Light.h"
#include "IGPUProgrammingServices.h"

namespace Irrlicht
{
public __gc class IrrlichtDevice;

namespace Video
{
	public __gc class IGPUProgrammingServices;

	/// <summary> 
	/// Enumeration for querying features of the video driver. 
	/// </summary>
	__value public enum VideoDriverFeature
	{
		/// Is driver able to render to a surface?
		RENDER_TO_TARGET = 0,	

		/// Is driver able to render with a bilinear filter applied?
		BILINEAR_FILER,			

		/// Is hardeware transform and lighting supported?
		HARDWARE_TL,			

		/// Can the driver handle mip maps?
		MIP_MAP,		

		/// Are stencilbuffers switched on and does the device support stencil buffers?
		STENCIL_BUFFER,

		/// Is Vertex Shader 1.1 supported?
		VERTEX_SHADER_1_1,

		/// Is Vertex Shader 2.0 supported?
		VERTEX_SHADER_2_0,

		/// Is Vertex Shader 3.0 supported?
		VERTEX_SHADER_3_0,

		/// Is Pixel Shader 1.1 supported?
		PIXEL_SHADER_1_1,

		/// Is Pixel Shader 1.2 supported?
		PIXEL_SHADER_1_2,

		/// Is Pixel Shader 1.3 supported?
		PIXEL_SHADER_1_3,

		/// Is Pixel Shader 1.4 supported?
		PIXEL_SHADER_1_4,

		/// Is Pixel Shader 2.0 supported?
		PIXEL_SHADER_2_0,

		/// Is Pixel Shader 3.0 supported?
		PIXEL_SHADER_3_0,

		/// Are ARB vertex programs v1.0 supported?
		ARB_VERTEX_PROGRAM_1,

		/// Are ARB fragment programs v1.0 supported?
		ARB_FRAGMENT_PROGRAM_1,

		/// Is GLSL supported?
		ARB_GLSL,

		/// Is HLSL supported?
		HLSL
	};

	/// <summary> 
	/// Enumeration for geometry transformation states. 
	/// </summary>
	__value public enum TransformationState
	{
		/// View transformation
		VIEW = 0, 

		/// World transformation
		WORLD, 

		/// Projection transformation
		PROJECTION, 

		/// Not used
		COUNT 
	};

	/// <summary>
	/// Interface to driver which is able to perform 2d and 3d gfx functions.
	/// The IVideoDriver interface is one of the most important interfaces of
	/// the Irrlicht Engine: All rendering and texture manipulating is done with
	/// this interface. You are able to use the Irrlicht Engine by only invoking methods
	/// of this interface if you would like to, although the Irrlicht::Scene::ISceneManager interface
	/// provides a lot of powerful classes and methods to make the programmers life
	/// easier.
	///
	/// This class has been ported directly from the native C++ Irrlicht Engine, so it may not 
	/// be 100% complete yet and the design may not be 100% .NET like.
	/// </summary>
	public __gc class IVideoDriver
	{
	public:

		/// <summary>
		/// You should access the IVideoDriver 
		/// through the Irrlicht::IrrlichtDevice.VideoDriver property. Simply don't use
		/// this constructor.
		///</summary>
		///<param name="driver">The real, unmanaged C++ video driver</param>
		IVideoDriver(irr::video::IVideoDriver* driver);

		/// <summary>
		/// Returns the type of the driver.
		/// </summary>
		__property DriverType get_DriverType();

		/// <summary>
		/// Applications must call this method before performing any rendering. 
		/// </summary>
		/// <param name="backBuffer"> Specifies if the back buffer should be cleared, which
		/// means that the screen is filled with a color specified with the parameter color.
		/// If this parameter is false, the back buffer will not be cleared and the color
		/// parameter is ignored. </param>
		/// <param name="zBuffer"> Speciefies if the depth or z buffer should be cleared. It is
		/// not nesesarry to do so, if only 2d drawing is used. </param>
		/// <returns> Returns false if failed. Begin Scene can clear the back- and 
		/// the z-buffer. </returns>
		bool BeginScene(bool backBuffer, bool zBuffer, Color color);

		/// <summary>
		/// Presents the rendered image on the screen.
		/// Applications must call this method after performing any rendering. 
		/// </summary>
		/// <returns> Returns false if failed and true if succeeded. </returns>
		bool EndScene();

		/// <summary>
		/// Presents the rendered image on the screen.
		/// Applications must call this method after performing any rendering. 
		/// </summary>
		/// <returns> Returns false if failed and true if succeeded. </returns>
		bool EndScene( System::IntPtr windowHandleWhereToPresent );

		/// <summary>
		/// Presents the rendered image on the screen.
		/// Applications must call this method after performing any rendering. 
		/// </summary>
		/// <returns> Returns false if failed and true if succeeded. </returns>
		bool EndScene( System::IntPtr windowHandleWhereToPresent, Core::Rect sourceRect );

		/// <summary>
		/// Queries the features of the driver, returns true if a feature is available
		/// </summary>
		/// <param name="feature"> A feature to query. </param>
		/// <returns> Returns true if the feature is available, false if not. </returns>
		bool QueryFeature(VideoDriverFeature feature);

		/// <summary>
		/// Sets the view, world or projection transformation. 
		/// </summary>
		/// <param name="state"> Transformation type to be set. Can be view, world or projection. </param>
		/// <param name="mat"> Matrix describing the transformation. </param>
		void SetTransform(TransformationState state, Core::Matrix4 mat);

		/// <summary>
		/// Returns the current transformation state which has been set by SetTransform
		/// </summary>
		Core::Matrix4 IVideoDriver::GetTransform( TransformationState state );

		/// <summary> Sets a material. All 3d drawing functions draw geometry now
		/// using this material. </summary>
		/// <param name="material"> Material to be used from now on. </param>
		virtual void SetMaterial(Material material);

		/// <summary>
		/// Returns a pointer to a texture. Loads the texture if it is not
		/// already loaded, and generates mipmap levels if wished.
		/// The texture can be in BMP, JPG, TGA, PCX and PSD format.
		/// For loading BMP, TGA, PCX and PSD files, the engine uses its own methods.
		/// PCX loading is based on some code by Dean P. Macri, who sent
		/// it in for free use by the engine. It currently not supports
		/// all .pcx formats. Also, compressed BMP's and TGAs don't work correctly.
		/// For loading JPG-Files the JPEG LIB 6b, written by 
		/// The Independent JPEG Group is used by the engine. Thanx for such a great
		/// library!
		/// </summary>
		/// <param name="filename"/> Filename of the texture to be loaded.</param>
		/// \return Returns a pointer to the texture and NULL if the texture
		/// could not be loaded. 
		/// This pointer should not be dropped. See IUnknown::drop() for more information.
		ITexture* GetTexture(System::String* filename);

		/// <summary>
		/// Returns a pointer to a texture. Loads the texture if it is not
		/// already loaded, and generates mipmap levels if wished.
		/// The texture can be in BMP, JPG, TGA, PCX and PSD format.
		/// For loading BMP, TGA, PCX and PSD files, the engine uses its own methods.
		/// PCX loading is based on some code by Dean P. Macri, who sent
		/// it in for free use by the engine. It currently not supports
		/// all .pcx formats. Also, compressed BMP's and TGAs don't work correctly.
		/// For loading JPG-Files the JPEG LIB 6b, written by 
		/// The Independent JPEG Group is used by the engine. Thanx for such a great
		/// library!
		/// <summary/>
		/// <param name="file"> Pointer to an already opened file. </param>
		/// <returns> Returns a pointer to the texture and NULL if the texture
		/// could not be loaded. </returns>
		// ITexture* getTexture(io::IReadFile* file) = 0;

		/// <summary>
		/// Creates an empty Texture of specified size. 
		/// </summary>
		/// <param name="size"> Size of the texture.</param>
		/// <param name="name"> A name for the texture. Later calls of getTexture() with this name
		/// will return this texture </param>
		/// <returns> Returns a pointer to the new created Texture. 
		/// The format of the new texture will be chosen by the driver, and will in most 
		/// cases have the ECF_A1R5G5B5 or ECF_A8R8G8B8 format.</returns>
		ITexture* AddTexture(Core::Dimension2D size,
			System::String* name);

		/// <summary>
		/// Creates an empty Texture of specified size.
		/// </summary>
		/// <param name="size"> Size of the texture.</param>
		/// <param name="name"> A name for the texture. Later calls of getTexture() with this name
		/// will return this texture </param>
		/// <param name="format"> Desired color format of the texture. Please note that
		/// the driver may choose to create the texture in another color format.
		/// (Default: A1R5G5B5)</param>
		/// <returns> Returns a pointer to the new created Texture. 
		/// The format of the new texture will be chosen by the driver, and will in most 
		/// cases have the ECF_A1R5G5B5 or ECF_A8R8G8B8 format.</returns>
		ITexture* AddTexture(Core::Dimension2D size,
			System::String* name, ColorFormat format);

		/// <summary>
		/// Creates a texture from a loaded IImage.
		/// </summary>
		/// <param name="name">A name for the texture. Later calls of getTexture() with this name
		/// will return this texture</param>
		/// <param name="image"> Image from which the texture is created from.</param>
		/// <returns>Returns a pointer to the new created Texture. 
		/// The format of the new texture will be chosen by the driver, and will in most 
		/// cases have the ECF_A1R5G5B5 or ECF_A8R8G8B8 format.</returns>
		//virtual ITexture* addTexture(const c8* name, IImage* image) = 0;

		/// <summary>
		/// Removes a texture from the texture cache and deletes it, freeing lot of
		/// memory.
		/// </summary>
		/// <param name="texture"> Texture to delete from the engines cache.</param>
		void RemoveTexture(ITexture* texture);

		/// <summary>
		/// Removes all texture from the texture cache and deletes them, freeing lot of
		/// memory.
		/// </summary>
		void RemoveAllTextures();

		/// <summary>
		/// Creates an 1bit alpha channel of the texture based of an color key.
		/// This makes the texture transparent at the regions where this color
		/// key can be found when using for example draw2DImage with useAlphachannel
		/// = true.
		/// </summary>
		/// <param name="texture"> Texture of which its alpha channel is modified.</param>
		/// <param name="color"> Color key color. Every pixel with this color will get transparent 
		/// like described above. Please note that the colors of a texture may get
		/// converted when loading it, so the color values may not be exactly the same
		/// in the engine and for example in picture edit programs. To avoid this 
		/// problem, you could use the makeColorKeyTexture method, which takes the position
		/// of a pixel instead a color value.</param>
		void MakeColorKeyTexture(ITexture* texture, Color color);

		/// <summary>
		/// Creates an 1bit alpha channel of the texture based of an color key position.
		/// This makes the texture transparent at the regions where this color
		/// key can be found when using for example draw2DImage with useAlphachannel
		/// = true.
		/// </summary>
		/// <param name="texture"> Texture of which its alpha channel is modified.</param>
		/// <param name="colorKeyPixelPos"> Position of a pixel with the color key color.
		/// Every pixel with this color will get transparent 
		/// like described above. </param>
		void MakeColorKeyTexture(ITexture* texture,	Core::Position2D colorKeyPixelPos);

		/// <summary>
		/// Sets a new render target. This will only work, if the driver
		/// supports the RENDER_TO_TARGET feature, which can be 
		/// queried with queryFeature().
		/// </summary>
		/// <param name="texture"> New render target. </param>
		void SetRenderTarget(ITexture* texture);

		/// <summary>
		/// Sets a new viewport or retrieves the old one. Every rendering operation is done into this
		/// new area.
		/// </summary>
		__property void set_ViewPort(Core::Rect area);

		/// <summary>
		/// Sets a new viewport or retrieves the old one. Every rendering operation is done into this
		/// new area.
		/// </summary>
		__property Core::Rect get_ViewPort();

		/// <summary>
		/// Draws an indexed triangle list.
		/// Note that there may be at maximum 65536 vertices, because the
		/// index list is a array of 16 bit values each with a maximum value 
		/// of 65536. If there are more than 65536 vertices in the list, 
		/// results of this operation are not defined.
		/// </summary>
		/// <param name="vertices"> Pointer to array of vertices.</param>
		/// <param name="vertexCount"> Amount of vertices in the array.</param>
		/// <param name="indexList"> Pointer to array of indizes.</param>
		/// <param name="triangleCount"> amount of Triangles. Usually amount of indizes / 3.</param>
		void DrawIndexedTriangleList(Vertex3D vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount);

		/// <summary>
		/// Draws an indexed triangle list.
		/// Note that there may be at maximum 65536 vertices, because the
		/// index list is a array of 16 bit values each with a maximum value 
		/// of 65536. If there are more than 65536 vertices in the list, 
		/// results of this operation are not defined.
		/// </summary>
		/// <param name="vertices"> Pointer to array of vertices.</param>
		/// <param name="vertexCount"> Amount of vertices in the array.</param>
		/// <param name="indexList"> Pointer to array of indizes.</param>
		/// <param name="triangleCount"> amount of Triangles. Usually amount of indizes / 3.</param>
		virtual void DrawIndexedTriangleList(Vertex3D2Tex vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount);

		/// <summary>
		/// Draws an indexed triangle fan.
		/// Note that there may be at maximum 65536 vertices, because the
		/// index list is a array of 16 bit values each with a maximum value 
		/// of 65536. If there are more than 65536 vertices in the list, 
		/// results of this operation are not defined.
		/// Please note that some of the implementation code for this method is based on 
		/// free code sent in by Mario Gruber, lots of thanks go to him!
		/// </summary>
		/// <param name="vertices"> Pointer to array of vertices.</param>
		/// <param name="vertexCount"> Amount of vertices in the array.</param>
		/// <param name="indexList"> Pointer to array of indizes.</param>
		/// <param name="triangleCount"> amount of Triangles. Usually amount of indizes / 3.</param>
		virtual void DrawIndexedTriangleFan(Vertex3D vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount);

		/// <summary>
		/// Draws an indexed triangle fan.
		/// Note that there may be at maximum 65536 vertices, because the
		/// index list is a array of 16 bit values each with a maximum value 
		/// of 65536. If there are more than 65536 vertices in the list, 
		/// results of this operation are not defined.
		/// Please note that some of the implementation code for this method is based on 
		/// free code sent in by Mario Gruber, lots of thanks go to him!
		/// </summary>
		/// <param name="vertices"> Pointer to array of vertices.</param>
		/// <param name="vertexCount"> Amount of vertices in the array.</param>
		/// <param name="indexList"> Pointer to array of indizes.</param>
		/// <param name="triangleCount"> amount of Triangles. Usually amount of indizes / 3.</param>
		virtual void DrawIndexedTriangleFan(Vertex3D2Tex vertices[],
			int vertexCount, unsigned short indexList __gc[], int triangleCount);

		/// <summary>
		/// Draws a 3d line.
		/// This method usually simply calls drawIndexedTriangles with some 
		/// triangles. Note that the line is drawed using the current transformation
		/// matrix and material.
		/// This method was created for making culling debugging easier. It works with
		/// all drivers because it does simply a call to drawIndexedTriangleList and
		/// hence is not very fast but it might be useful for further development. 
		/// </summary>
		/// <param name="start"> Start of the 3d line.</param>
		/// <param name="end"> End of the 3d line.</param>
		/// <param name="color"> Color of the line. </param>
		void Draw3DLine(Core::Vector3D start, Core::Vector3D end,
			Color color);

		/// <summary>
		/// Draws a 3d triangle.
		/// This method usually simply calls drawIndexedTriangles with some 
		/// triangles. Note that the line is drawed using the current transformation
		/// matrix and material.
		/// This method was created for making collision debugging easier. It works with
		/// all drivers because it does simply a call to drawIndexedTriangleList and
		/// hence is not very fast but it might be useful for further development. 
		/// </summary>
		/// <param name="triangle"> The triangle to draw.</name>
		/// <param name="color"> Color of the line. </param>
		void Draw3DTriangle(Core::Triangle3D triangle, Color color);

		/// <summary>
		/// Draws a 3d axis aligned box.
		/// This method usually simply calls drawIndexedTriangles with some 
		/// triangles. Note that the line is drawed using the current transformation
		/// matrix and material. 
		/// This method was created for making culling debugging easier. It works with
		/// all drivers because it does simply a call to drawIndexedTriangleList and
		/// hence is not very fast but it might be useful for further development. 
		/// </summary>
		/// <param name="box"> The axis aligned box to draw</param>
		/// <param name="color"> Color to use while drawing the box.</param>
		void Draw3DBox(Core::Box3D box, Color color);

		/// <summary>
		/// Simply draws an 2d image, without any special effects
		/// </summary>
		/// <param name="texture"> Pointer to texture to use.</param>
		/// <param name="destPos"> upper left 2d destination position where the image will be drawn.</param>
		void Draw2DImage(ITexture* texture, Core::Position2D pos);

		/// <summary>
		/// Draws an 2d image, using a color (if color is other than 
		/// Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		/// </summary>
		/// <param name="texture"> Texture to be drawn.</param>
		/// <param name="destPos"> Upper left 2d destination position where the image will be drawn.</param>
		/// <param name="sourceRect"> Source rectangle in the image.</param>
		/// <param name="clipRect"> Rectangle on the screen where the image is clipped to.</param>
		/// <param name="color"> Color with wich the image is colored. If the color equals 
		/// Color(255,255,255,255), the color is ignored. Note that the alpha component
		/// is used: If alpha is other than 255, the image will be transparent.</param>
		/// <param name="useAlphaChannelOfTexture"> If true, the alpha channel of the texture is 
		/// used to draw the image.</param>
		void Draw2DImage(ITexture* texture, Core::Position2D destPos,
			Core::Rect sourceRect, Core::Rect clipRect,
			Color color, bool useAlphaChannelOfTexture);

		/// <summary>
		/// Draws an 2d image, using a color (if color is other than 
		/// Color(255,255,255,255)) and the alpha channel of the texture if wanted.
		/// </summary>
		/// <param name="texture"> Texture to be drawn.</param>
		/// <param name="destPos"> Upper left 2d destination position where the image will be drawn.</param>
		/// <param name="sourceRect"> Source rectangle in the image.</param>
		/// <param name="color"> Color with wich the image is colored. If the color equals 
		/// Color(255,255,255,255), the color is ignored. Note that the alpha component
		/// is used: If alpha is other than 255, the image will be transparent.</param>
		/// <param name="useAlphaChannelOfTexture"> If true, the alpha channel of the texture is 
		/// used to draw the image.</param>
		void Draw2DImage(ITexture* texture, Core::Position2D destPos,
			Core::Rect sourceRect,
			Color color, bool useAlphaChannelOfTexture);

		/// <summary>
		/// Draws an 2d rectangle.
		/// </summary>
		/// <param name="color"> Color of the rectangle to draw. The alpha component will not
		/// be ignored and specifies how transparent the rectangle will be.</param>
		/// <param name="pos"> Position of the rectangle.</param>
		/// <param name="clip">Rectangle against which the rectangle will be clipped.</param>
		void Draw2DRectangle(Color color, Core::Rect pos,	Core::Rect clip);

		/// <summary>
		/// Draws an 2d rectangle.
		/// </summary>
		/// <param name="color"> Color of the rectangle to draw. The alpha component will not
		/// be ignored and specifies how transparent the rectangle will be.</param>
		/// <param name="pos"> Position of the rectangle.</param>
		void Draw2DRectangle(Color color, Core::Rect pos);

		/// <summary>
		/// Draws a 2d line. 
		/// </summary>
		/// <param name="start"> Screen coordinates of the start of the line in pixels.</param>
		/// <param name="end"> Screen coordinates of the start of the line in pixels.</param>
		/// <param name="color"> Color of the line to draw.</param>
		void Draw2DLine(Core::Position2D start, Core::Position2D end, Color color);

		/// <summary>
		/// Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
		/// this: Frist, draw all geometry. Then use this method, to draw the shadow
		/// volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
		/// Please note that the code for the opengl version of the method is based on 
		/// free code sent in by Philipp Dortmann, lots of thanks go to him!
		/// </summary>
		/// <param name="triangles"> Array of 3d vectors, specifing the shadow volume.</param>
		/// <param name="count"> Amount of triangles in the array.</param>
		/// <param name="zfail"> If set to true, zfail method is used, otherwise zpass.</param>
		void DrawStencilShadowVolume(Core::Vector3D triangles[], int count, bool zfail);

		/// <summary>
		/// Fills the stencil shadow with color. After the shadow volume has been drawn
		/// into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
		/// to draw the color of the shadow. 
		/// Please note that the code for the opengl version of the method is based on 
		/// free code sent in by Philipp Dortmann, lots of thanks go to him!
		/// </summary>
		/// <param name="clearStencilBuffer"> Set this to false, if you want to draw every shadow 
		/// with the same color, and only want to call drawStencilShadow() once after all
		/// shadow volumes have been drawn. Set this to true, if you want to paint every
		/// shadow with its own color.</param>
		/// <param name="leftUpEdge> Color of the shadow in the upper left corner of screen.</param>
		/// <param name="rightUpEdge> Color of the shadow in the upper right corner of screen.</param>
		/// <param name="leftDownEdge> Color of the shadow in the lower left corner of screen.</param>
		/// <param name="rightDownEdge> Color of the shadow in the lower right corner of screen.</param>
		void DrawStencilShadow(bool clearStencilBuffer, 
			Color leftUpEdge, Color rightUpEdge, Color leftDownEdge, Color rightDownEdge);

		/// Draws a mesh buffer
		/// <param name="mb"/> Buffer to draw;</param>
		//virtual void DrawMeshBuffer(scene::IMeshBuffer* mb) = 0;

		/// <summary>
		/// Sets the fog mode. These are global values attached to each 3d object
		/// rendered, which has the fog flag enabled in its material.
		/// </summary>
		/// <param color: Color of the fog</param>
		/// <param linearFog: Set this to true for linear fog, otherwise exponential fog is applied.</param>
		/// <param start: Only used in linear fog mode (linearFog=true). Specifies where fog starts.</param>
		/// <param end: Only used in linear fog mode (linearFog=true). Specifies where fog ends.</param>
		/// <param density: Only used in expotential fog mode (linearFog=false). Must be a value between 0 and 1.</param>
		/// <param pixelFog: Set this to false for vertex fog, and true if you want pixel fog.</param>
		/// <param rangeFog: Set this to true to enable range-based vertex fog. The distance
		/// from the viewer is used to compute the fog, not the z-coordinate. This is
		/// better, but slower. This is only available with D3D and vertex fog.</param>
		void SetFog(Color color, bool linearFog, float start, float end, 
			float density, bool pixelFog, bool rangeFog);

		/// <summary>
		/// Returns the size of the screen or render window.
		/// </summary>
		/// <returns> Size of screen or render window.</returns>
		__property Core::Dimension2D get_ScreenSize();

		/// <summary>
		/// Returns current frames per second value.
		/// </summary>
		/// <returns> Returns amount of frames per second drawn.</returns>
		__property int get_FPS();

		/// <summary>
		/// Returns amount of primitives (mostly triangles) which were drawn in the last frame.
		/// Together with getFPS() very useful method for statistics.
		/// </summary>
		/// <returns> Amount of primitives drawn in the last frame.</returns>
		__property int get_PrimitiveCountDrawn();

		/// <summary>
		/// Deletes all dynamic lights which were previously added with addDynamicLight().
		/// </summary>
		void DeleteAllDynamicLights();

		/// Adds a dynamic light.
		/// <param name="light"> Data specifing the dynamic light.</param>
		void AddDynamicLight(Light light);

		/// <summary>
		/// Sets the dynamic ambient light color. The default color is
		/// (0,0,0,0) which means it is dark.
		/// </summary>
		/// <param name="color"> New color of the ambient light.</param>
		void SetAmbientLight(Colorf color);

		/// <summary>
		/// Returns the maximal amount of dynamic lights the device can handle
		/// </summary>
		/// <returns> Maximal amount of dynamic lights.</returns>
		__property int get_MaximalDynamicLightAmount();

		/// <summary>
		/// Returns current amount of dynamic lights set
		/// </summary>
		/// <returns> Current amount of dynamic lights set</returns>
		__property int get_DynamicLightCount();

		/// <summary>
		/// Returns light data which was previously set with IVideDriver::addDynamicLight().
		/// </summary>
		/// <param name="idx"> Zero based index of the light. Must be greater than 0 and smaller
		/// than IVideoDriver()::getDynamicLightCount.</name>
		/// <returns> Light data.</returns>
		Light GetDynamicLight(int idx);
        
		/// <summary>
		/// Returns the name of the video driver. Example: In case of the DirectX8
		/// driver, it would return "Direct3D8.1".
		/// </summary>
		__property System::String* get_Name();

		/// Adds an external image loader to the engine. This is useful if 
		/// the Irrlicht Engine should be able to load textures of currently 
		/// unsupported file formats (e.g .gif). The IImageLoader only needs
		/// to be implemented for loading this file format. A pointer to
		/// the implementation can be passed to the engine using this method.
		/// <param name="loader"/> Pointer to the external loader created.</param>
		//virtual void AddExternalImageLoader(IImageLoader* loader) = 0;

		/// <summary>
		/// Returns the maximum amount of primitives (mostly vertices) which
		/// the device is able to render with one drawIndexedTriangleList
		/// call.
		/// </summary>
		__property int get_MaximalPrimitiveCount();

		/// <summary>
		/// Enables or disables a texture creation flag. This flag defines how
		/// textures should be created. By changing this value, you can influence
		/// the speed of rendering a lot. But please note that the video drivers
		/// take this value only as recommendation. It could happen that you
		/// enable the ETCM_ALWAYS_16_BIT mode, but the driver creates 32 bit
		/// textures.
		/// </summary>
		/// <param name="flag"> Texture creation flag.</param>
		/// <param name="enbabled"> Specifies if the given flag should be enabled or disabled.</param>
		void SetTextureCreationFlag(TextureCreationFlag flag, bool enabled);

		/// <summary>
		/// Returns if a texture creation flag is enabled or disabled.
		/// You can change this value using setTextureCreationMode().
		/// </summary>
		/// <param name="flag"> Texture creation flag.</name>
		/// <returns> Returns the current texture creation mode.</returns>
		bool GetTextureCreationFlag(TextureCreationFlag flag);

		/// <summary>
		/// Creates a software image from a file. No hardware texture will
		/// be created for this image. This method is useful for example if
		/// you want to read a heightmap for a terrain renderer.
		/// </summary>
		/// <param name="filename"> Name of the file from which the image is created.</name>
		/// <returns> Returns the created image.</returns>
		//IImage* CreateImageFromFile(const char* filename) = 0;

		/// Creates a software image from a file. No hardware texture will
		/// be created for this image. This method is useful for example if
		/// you want to read a heightmap for a terrain renderer.
		/// <param name="file"/> File from which the image is created.</param>
		/// \return Returns the created image.
		/// If you no longer need the image, you should call IImage::drop().
		/// See IUnknown::drop() for more information.
		//virtual IImage* CreateImageFromFile(io::IReadFile* file) = 0;

		/// <summary>
		/// Returns pointer to the IGPUProgrammingServices interface. 
		/// </summary>
		/// <returns> Returns 0 if the videodriver does not support this.</returns>
		__property IGPUProgrammingServices* get_GPUProgrammingServices();

	private:

		irr::video::IVideoDriver* Driver;
		IGPUProgrammingServices* Services;
	};


}
}