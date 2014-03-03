#include "CMeshTextureLoader.h"
#include "IFileSystem.h"
#include "IVideoDriver.h"

namespace irr
{
namespace scene
{

CMeshTextureLoader::CMeshTextureLoader(irr::io::IFileSystem* fs, irr::video::IVideoDriver* driver)
: FileSystem(fs)
, VideoDriver(driver)
, MeshFile(0)
, MaterialFile(0)
, WasRecentTextureCached(false)
{
}

//! Set a custom texture path.
void CMeshTextureLoader::setTexturePath(const irr::io::path& path)
{
	TexturePath = path;
	preparePath(TexturePath);
}

//! Get the current custom texture path.
const irr::io::path& CMeshTextureLoader::getTexturePath() const
{
	return TexturePath;
}

bool CMeshTextureLoader::checkTextureName( const irr::io::path& filename, bool checkCache)
{
	if (FileSystem->existFile(filename))
	{
		TextureName = filename;
		WasRecentTextureCached = checkCache && VideoDriver->findTexture(TextureName) != 0;
		return true;
	}

	return false;
}

//! Get the texture by searching for it in all paths that makes sense for the given textureName.
irr::video::ITexture* CMeshTextureLoader::getTexture(const irr::io::path& textureName)
{
	if ( textureName.empty() || !FileSystem || !VideoDriver)
		return NULL;

	// Pre-process texture filename.
	irr::io::path simplifiedTexName(textureName);
	simplifiedTexName.replace(_IRR_TEXT('\\'),_IRR_TEXT('/'));

	bool checkCache = getCheckForCachedTextures();

	// user defined texture path
	if ( !TexturePath.empty() )
	{
		if ( checkTextureName(TexturePath + simplifiedTexName, checkCache) )
			return VideoDriver->getTexture(TextureName);

		if ( checkTextureName(TexturePath + FileSystem->getFileBasename(simplifiedTexName), checkCache) )
			return VideoDriver->getTexture(TextureName);
	}

	// just the name itself
	if ( checkTextureName(simplifiedTexName, checkCache) )
		return VideoDriver->getTexture(TextureName);

	// look in files relative to the folder of the meshfile
	if ( MeshFile )
	{
		if ( MeshPath.empty() )
		{
			MeshPath = FileSystem->getFileDir(MeshFile->getFileName());
			preparePath(MeshPath);
		}
		if ( !MeshPath.empty() )
		{
			if ( checkTextureName(MeshPath + simplifiedTexName, checkCache) )
				return VideoDriver->getTexture(TextureName);

			if ( checkTextureName(MeshPath + FileSystem->getFileBasename(simplifiedTexName), checkCache) )
				return VideoDriver->getTexture(TextureName);
		}
	}

	// look in files relative to the folder of the materialfile
	if ( MaterialFile )
	{
		if ( MaterialPath.empty() )
		{
			MaterialPath = FileSystem->getFileDir(MaterialFile->getFileName());
			preparePath(MaterialPath);
		}
		if ( !MaterialPath.empty() )
		{
			if ( checkTextureName(MaterialPath + simplifiedTexName, checkCache) )
				return VideoDriver->getTexture(TextureName);

			if ( checkTextureName(MaterialPath + FileSystem->getFileBasename(simplifiedTexName), checkCache) )
				return VideoDriver->getTexture(TextureName);
		}
	}

	// check current working directory
	if ( checkTextureName(FileSystem->getFileBasename(simplifiedTexName), checkCache) )
		return VideoDriver->getTexture(TextureName);

	TextureName = _IRR_TEXT("");
	return NULL;
}

//! Check if the last call to getTexture found a texture which was already cached.
bool CMeshTextureLoader::wasRecentTextureInCache() const
{
	return WasRecentTextureCached;
}

//! Meshloaders will search paths relative to the meshFile.
void CMeshTextureLoader::setMeshFile(const irr::io::IReadFile* meshFile)
{
	// no grab (would need a weak_ptr)
	MeshFile = meshFile;
	MeshPath = _IRR_TEXT("");	// do a lazy evaluation later
}

//! Meshloaders will try to look relative to the path of the materialFile
void CMeshTextureLoader::setMaterialFile(const irr::io::IReadFile* materialFile)
{
	// no grab (would need a weak_ptr)
	MaterialFile = materialFile;
	MaterialPath = _IRR_TEXT("");	// do a lazy evaluation later
}

} // end namespace scnene
} // end namespace irr
