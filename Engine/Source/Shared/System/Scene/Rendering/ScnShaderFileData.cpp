/**************************************************************************
*
* File:		ScnShaderFileData.cpp
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "System/Scene/Rendering/ScnShaderFileData.h"

//////////////////////////////////////////////////////////////////////////
// ScnShaderViewUniformBlockData
REFLECTION_DEFINE_BASIC( ScnShaderViewUniformBlockData );

void ScnShaderViewUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "InverseProjectionTransform_", &ScnShaderViewUniformBlockData::InverseProjectionTransform_ ),
		new ReField( "ProjectionTransform_", &ScnShaderViewUniformBlockData::ProjectionTransform_ ),
		new ReField( "InverseViewTransform_", &ScnShaderViewUniformBlockData::InverseViewTransform_ ),
		new ReField( "ViewTransform_", &ScnShaderViewUniformBlockData::ViewTransform_ ),
		new ReField( "ClipTransform_", &ScnShaderViewUniformBlockData::ClipTransform_ )
	};
		
	ReRegisterClass< ScnShaderViewUniformBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderLightUniformBlockData
REFLECTION_DEFINE_BASIC( ScnShaderLightUniformBlockData );

void ScnShaderLightUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "LightPosition_", &ScnShaderLightUniformBlockData::LightPosition_ ),
		new ReField( "LightDirection_", &ScnShaderLightUniformBlockData::LightDirection_ ),
		new ReField( "LightAmbientColour_", &ScnShaderLightUniformBlockData::LightAmbientColour_ ),
		new ReField( "LightDiffuseColour_", &ScnShaderLightUniformBlockData::LightDiffuseColour_ ),
		new ReField( "LightAttn_", &ScnShaderLightUniformBlockData::LightAttn_ )
	};
		
	ReRegisterClass< ScnShaderLightUniformBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderObjectUniformBlockData
REFLECTION_DEFINE_BASIC( ScnShaderObjectUniformBlockData );

void ScnShaderObjectUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "WorldTransform_", &ScnShaderObjectUniformBlockData::WorldTransform_ ),
		new ReField( "NormalTransform_", &ScnShaderObjectUniformBlockData::NormalTransform_ ),
	};
		
	ReRegisterClass< ScnShaderObjectUniformBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderBoneUniformBlockData
REFLECTION_DEFINE_BASIC( ScnShaderBoneUniformBlockData );

void ScnShaderBoneUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "BoneTransform_", &ScnShaderBoneUniformBlockData::BoneTransform_ ),
	};
		
	ReRegisterClass< ScnShaderBoneUniformBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderAlphaTestUniformBlockData
REFLECTION_DEFINE_BASIC( ScnShaderAlphaTestUniformBlockData );

void ScnShaderAlphaTestUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "AlphaTestParams_", &ScnShaderAlphaTestUniformBlockData::AlphaTestParams_ ),
	};
		
	ReRegisterClass< ScnShaderAlphaTestUniformBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessConfigData
REFLECTION_DEFINE_BASIC( ScnShaderPostProcessConfigData );

void ScnShaderPostProcessConfigData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "InputDimensions_", &ScnShaderPostProcessConfigData::InputDimensions_ ),
		new ReField( "OutputDimensions_", &ScnShaderPostProcessConfigData::OutputDimensions_ ),
	};
		
	ReRegisterClass< ScnShaderPostProcessConfigData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessCopyBlockData
REFLECTION_DEFINE_BASIC( ScnShaderPostProcessCopyBlockData );

void ScnShaderPostProcessCopyBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "ColourTransform_", &ScnShaderPostProcessCopyBlockData::ColourTransform_ ),
	};
		
	ReRegisterClass< ScnShaderPostProcessCopyBlockData >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessBlurBlockData
REFLECTION_DEFINE_BASIC( ScnShaderPostProcessBlurBlockData );

void ScnShaderPostProcessBlurBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "TextureDimensions_", &ScnShaderPostProcessBlurBlockData::TextureDimensions_ ),
		new ReField( "Radius_", &ScnShaderPostProcessBlurBlockData::Radius_ ),
	};
		
	ReRegisterClass< ScnShaderPostProcessBlurBlockData >( Fields );
}
