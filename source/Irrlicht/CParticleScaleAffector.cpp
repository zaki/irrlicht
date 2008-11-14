#include "CParticleScaleAffector.h"
#include "IAttributes.h"

namespace irr
{
	namespace scene
	{
		CParticleScaleAffector::CParticleScaleAffector(const core::vector2df& scaleTo)
			: ScaleTo(scaleTo)
		{
		}


		void CParticleScaleAffector::affect (u32 now, SParticle *particlearray, u32 count)
		{
			for(u32 i=0;i<count;i++)
			{
				const u32 maxdiff = particlearray[i].endTime - particlearray[i].startTime;
				const u32 curdiff = now - particlearray[i].startTime;
				const f32 newscale = (f32)curdiff/maxdiff;
				particlearray[i].size = particlearray[i].startSize+newscale*ScaleTo;
			}
		}


		void CParticleScaleAffector::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options)
		{
			out->addFloat("ScaleToX", ScaleTo.X);
			out->addFloat("ScaleToY", ScaleTo.Y);
		}


		s32 CParticleScaleAffector::deserializeAttributes(s32 startIndex, io::IAttributes* in, io::SAttributeReadWriteOptions* options)
		{
			ScaleTo.X = in->getAttributeAsFloat("ScaleToX");
			ScaleTo.Y = in->getAttributeAsFloat("ScaleToY");
			return 0;
		}


		E_PARTICLE_AFFECTOR_TYPE CParticleScaleAffector::getType() const
		{
			return scene::EPAT_SCALE;
		}
	}
}

