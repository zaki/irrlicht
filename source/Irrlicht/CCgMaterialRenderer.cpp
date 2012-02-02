// Copyright (C) 2011 Patryk Nadrowski
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_CG_

#include "CCgMaterialRenderer.h"

namespace irr
{
namespace video
{

CCgUniform::CCgUniform(const CGparameter& pParameter, bool pIsGlobal) : Parameter(pParameter), Type(CG_UNKNOWN_TYPE)
{
	Name = cgGetParameterName(Parameter);

	if(pIsGlobal)
		Space = CG_GLOBAL;
	else
		Space = CG_PROGRAM;
}

const core::stringc& CCgUniform::getName() const
{
	return Name;
}

const CGparameter& CCgUniform::getParameter() const
{
	return Parameter;
}

CGenum CCgUniform::getSpace() const
{
	return Space;
}

CGtype CCgUniform::getType() const
{
	return Type;
}

CCgUniform1f::CCgUniform1f(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_FLOAT;
}

void CCgUniform1f::update(const float* pData, const SMaterial& pMaterial) const
{
	cgSetParameter1f(Parameter, *pData);
}

CCgUniform2f::CCgUniform2f(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_FLOAT2;
}

void CCgUniform2f::update(const float* pData, const SMaterial& pMaterial) const
{
	cgSetParameter2f(Parameter, *pData, *(pData+1));
}

CCgUniform3f::CCgUniform3f(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_FLOAT3;
}

void CCgUniform3f::update(const float* pData, const SMaterial& pMaterial) const
{
	cgSetParameter3f(Parameter, *pData, *(pData+1), *(pData+2));
}

CCgUniform4f::CCgUniform4f(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_FLOAT4;
}

void CCgUniform4f::update(const float* pData, const SMaterial& pMaterial) const
{
	cgSetParameter4f(Parameter, *pData, *(pData+1), *(pData+2), *(pData+3));
}

CCgUniform4x4f::CCgUniform4x4f(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_FLOAT4x4;
}

void CCgUniform4x4f::update(const float* pData, const SMaterial& pMaterial) const
{
	cgSetMatrixParameterfr(Parameter, pData);
}

CCgUniformSampler2D::CCgUniformSampler2D(const CGparameter& pParameter, bool pIsGlobal) : CCgUniform(pParameter, pIsGlobal)
{
	Type = CG_SAMPLER2D;
}

void CCgUniformSampler2D::update(const float* pData, const SMaterial& pMaterial) const
{
}

CCgMaterialRenderer::CCgMaterialRenderer(IShaderConstantSetCallBack* pCallBack, IMaterialRenderer* pBaseMaterial, s32 pUserData) :
	CallBack(pCallBack), BaseMaterial(pBaseMaterial), UserData(pUserData),
	VertexProgram(0), FragmentProgram(0), GeometryProgram(0), VertexProfile(CG_PROFILE_UNKNOWN), FragmentProfile(CG_PROFILE_UNKNOWN), GeometryProfile(CG_PROFILE_UNKNOWN),
	Material(IdentityMaterial), Error(CG_NO_ERROR)
{
	#ifdef _DEBUG
	setDebugName("CCgMaterialRenderer");
	#endif

	if(BaseMaterial)
		BaseMaterial->grab();

	if(CallBack)
		CallBack->grab();
}

CCgMaterialRenderer::~CCgMaterialRenderer()
{
	if(CallBack)
		CallBack->drop();

	if(BaseMaterial)
		BaseMaterial->drop();

	for(unsigned int i = 0; i < UniformInfo.size(); ++i)
		delete UniformInfo[i];

	UniformInfo.clear();
}

bool CCgMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}

void CCgMaterialRenderer::setVertexShaderConstant(const f32* pData, s32 pStartRegister, s32 pConstantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.", ELL_WARNING);
}

bool CCgMaterialRenderer::setVertexShaderConstant(const c8* pName, const f32* pData, int pCount)
{
	return setPixelShaderConstant(pName, pData, pCount);
}

void CCgMaterialRenderer::setPixelShaderConstant(const f32* pData, s32 pStartRegister, s32 pConstantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.", ELL_WARNING);
}

bool CCgMaterialRenderer::setPixelShaderConstant(const c8* pName, const f32* pData, int pCount)
{
	bool Status = false;

	for(unsigned int i = 0; i < UniformInfo.size(); ++i)
	{
		if(UniformInfo[i]->getName() == pName)
		{
			UniformInfo[i]->update(pData, Material);

			Status = true;
		}
	}

	return Status;
}

void CCgMaterialRenderer::getUniformList()
{
	for(unsigned int i = 0; i < UniformInfo.size(); ++i)
		delete UniformInfo[i];

	UniformInfo.clear();

	for(unsigned int i = 0; i < 2; ++i)
	{
		CGenum Space = CG_GLOBAL;
		bool IsGlobal = 1;

		if(i == 1)
		{
			Space = CG_PROGRAM;
			IsGlobal = 0;
		}

		for(unsigned int j = 0; j < 3; ++j)
		{
			CGprogram* Program = 0;

			switch(j)
			{
			case 0:
				Program = &VertexProgram;
				break;
			case 1:
				Program = &FragmentProgram;
				break;
			case 2:
				Program = &GeometryProgram;
				break;
			}

			if(*Program)
			{
				CGparameter Parameter = cgGetFirstParameter(*Program, Space);

				while(Parameter)
				{
					if(cgGetParameterVariability(Parameter) == CG_UNIFORM && cgGetParameterDirection(Parameter) == CG_IN)
					{
						CCgUniform* Uniform = 0;

						CGtype Type = cgGetParameterType(Parameter);

						// TODO: add more uniform types

						switch(Type)
						{
						case CG_FLOAT:
						case CG_FLOAT1:
							Uniform = new CCgUniform1f(Parameter, IsGlobal);
							break;
						case CG_FLOAT2:
							Uniform = new CCgUniform2f(Parameter, IsGlobal);
							break;
						case CG_FLOAT3:
							Uniform = new CCgUniform3f(Parameter, IsGlobal);
							break;
						case CG_FLOAT4:
							Uniform = new CCgUniform4f(Parameter, IsGlobal);
							break;
						case CG_FLOAT4x4:
							Uniform = new CCgUniform4x4f(Parameter, IsGlobal);
							break;
						case CG_SAMPLER2D:
							Uniform = new CCgUniformSampler2D(Parameter, IsGlobal);
							break;
						}

						if(Uniform)
							UniformInfo.push_back(Uniform);
					}

					Parameter = cgGetNextParameter(Parameter);
				}
			}
		}
	}
}

}
}

#endif
