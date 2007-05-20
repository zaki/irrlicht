#include "IParticleAffector.h"
#include "NativeConverter.h"

namespace Irrlicht
{
namespace Scene
{

	IParticleAffector::IParticleAffector( irr::scene::IParticleAffector* realAffector )
		: particleAffector( realAffector )
	{
		particleAffector->grab();
	}

	IParticleAffector::~IParticleAffector()
	{
		particleAffector->drop();
	}
}
}