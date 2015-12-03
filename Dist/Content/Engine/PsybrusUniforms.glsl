#pragma once

////////////////////////////////////////////////////////////////////////
// cbuffer macros

#if PSY_INPUT_BACKEND_TYPE == PSY_BACKEND_TYPE_GLSL_ES
#undef PSY_USE_CBUFFER
#define PSY_USE_CBUFFER 0
#else
#undef PSY_USE_CBUFFER
#define PSY_USE_CBUFFER 1
#endif

#if PSY_USE_CBUFFER
#	define BEGIN_CBUFFER( _n ) layout(std140) uniform _n {
#	define ENTRY( _p, _t, _n ) _t _n;
#	define END_CBUFFER };
#else
#	define BEGIN_CBUFFER( _n )
#	define ENTRY( _p, _t, _n ) uniform _t _p##VS_X##_n;
#	define END_CBUFFER
#endif

////////////////////////////////////////////////////////////////////////
// ScnShaderViewUniformBlockData
BEGIN_CBUFFER( ScnShaderViewUniformBlockData )
	ENTRY( ScnShaderViewUniformBlockData, float4x4, InverseProjectionTransform_ )
	ENTRY( ScnShaderViewUniformBlockData, float4x4, ProjectionTransform_ )
	ENTRY( ScnShaderViewUniformBlockData, float4x4, InverseViewTransform_ )
	ENTRY( ScnShaderViewUniformBlockData, float4x4, ViewTransform_ )
	ENTRY( ScnShaderViewUniformBlockData, float4x4, ClipTransform_ )
	ENTRY( ScnShaderViewUniformBlockData, float4, ViewTime_ )
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define InverseProjectionTransform_ ScnShaderViewUniformBlockDataVS_XInverseProjectionTransform_
#  define ProjectionTransform_ ScnShaderViewUniformBlockDataVS_XProjectionTransform_
#  define InverseViewTransform_ ScnShaderViewUniformBlockDataVS_XInverseViewTransform_
#  define ViewTransform_ ScnShaderViewUniformBlockDataVS_XViewTransform_
#  define ClipTransform_ ScnShaderViewUniformBlockDataVS_XClipTransform_
#  define ViewTime_ ScnShaderViewUniformBlockDataVS_XViewTime_

#endif


////////////////////////////////////////////////////////////////////////
// ScnShaderLightUniformBlockData
BEGIN_CBUFFER( ScnShaderLightUniformBlockData )
	ENTRY( ScnShaderLightUniformBlockData, float4, LightPosition_[4] )
	ENTRY( ScnShaderLightUniformBlockData, float4, LightDirection_[4] )
	ENTRY( ScnShaderLightUniformBlockData, float4, LightAmbientColour_[4] )
	ENTRY( ScnShaderLightUniformBlockData, float4, LightDiffuseColour_[4] )
	ENTRY( ScnShaderLightUniformBlockData, float4, LightAttn_[4] )
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define LightPosition_ ScnShaderLightUniformBlockDataVS_XLightPosition_
#  define LightDirection_ ScnShaderLightUniformBlockDataVS_XLightDirection_
#  define LightAmbientColour_ ScnShaderLightUniformBlockDataVS_XLightAmbientColour_
#  define LightDiffuseColour_ ScnShaderLightUniformBlockDataVS_XLightDiffuseColour_
#  define LightAttn_ ScnShaderLightUniformBlockDataVS_XLightAttn_

#endif


////////////////////////////////////////////////////////////////////////
// ScnShaderObjectUniformBlockData
BEGIN_CBUFFER( ScnShaderObjectUniformBlockData )
	ENTRY( ScnShaderObjectUniformBlockData, float4x4, WorldTransform_ )
	ENTRY( ScnShaderObjectUniformBlockData, float4x4, NormalTransform_ )
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define WorldTransform_ ScnShaderObjectUniformBlockDataVS_XWorldTransform_
#  define NormalTransform_ ScnShaderObjectUniformBlockDataVS_XNormalTransform_

#endif


////////////////////////////////////////////////////////////////////////
// ScnShaderBoneUniformBlockData
BEGIN_CBUFFER( ScnShaderBoneUniformBlockData )
	ENTRY( ScnShaderBoneUniformBlockData, float4x4, BoneTransform_[24] )
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define BoneTransform_ ScnShaderBoneUniformBlockDataVS_XBoneTransform_

#endif


////////////////////////////////////////////////////////////////////////
// ScnShaderAlphaTestUniformBlockData
BEGIN_CBUFFER( ScnShaderAlphaTestUniformBlockData )
	/// x = smoothstep min, y = smoothstep max, z = ref (<)
	ENTRY( ScnShaderAlphaTestUniformBlockData, float4, AlphaTestParams_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define AlphaTestParams_ ScnShaderAlphaTestUniformBlockDataVS_XAlphaTestParams_

#endif

////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessConfigData
BEGIN_CBUFFER( ScnShaderPostProcessConfigData )
	ENTRY( ScnShaderPostProcessConfigData, float4, InputDimensions_[16] ) 
	ENTRY( ScnShaderPostProcessConfigData, float4, OutputDimensions_[4] ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define InputDimensions_ ScnShaderPostProcessConfigDataVS_XInputDimensions_
#  define OutputDimensions_ ScnShaderPostProcessConfigDataVS_XOutputDimensions_

#endif

////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessCopyBlockData
BEGIN_CBUFFER( ScnShaderPostProcessCopyBlockData )
	/// Colour transform to copy using.
	ENTRY( ScnShaderPostProcessCopyBlockData, float4x4, ColourTransform_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define ColourTransform_ ScnShaderPostProcessCopyVS_XColourTransform_

#endif

////////////////////////////////////////////////////////////////////////
// ScnShaderPostProcessBlurBlockData
BEGIN_CBUFFER( ScnShaderPostProcessBlurBlockData )
	ENTRY( ScnShaderPostProcessBlurBlockData, float2, TextureDimensions_ ) 
	ENTRY( ScnShaderPostProcessBlurBlockData, float, Radius_ ) 
	ENTRY( ScnShaderPostProcessBlurBlockData, float, Unused_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define TextureDimensions_ ScnShaderPostProcessBlurBlockDataVS_XTextureDimensions_
#  define Radius_ ScnShaderPostProcessBlurBlockDataVS_XRadius_

#endif

////////////////////////////////////////////////////////////////////////
// ScnFontUniformBlockData
BEGIN_CBUFFER( ScnFontUniformBlockData )
	/// x = smoothstep min, y = smoothstep max, z = ref (<)
	ENTRY( ScnFontUniformBlockData, float4, TextSettings_ ) 
	ENTRY( ScnFontUniformBlockData, float4, BorderSettings_ ) 
	ENTRY( ScnFontUniformBlockData, float4, ShadowSettings_ ) 
	ENTRY( ScnFontUniformBlockData, float4, TextColour_ ) 
	ENTRY( ScnFontUniformBlockData, float4, BorderColour_ ) 
	ENTRY( ScnFontUniformBlockData, float4, ShadowColour_ ) 
END_CBUFFER

#if !PSY_USE_CBUFFER

#  define TextSettings_ ScnFontUniformBlockDataVS_XTextSettings_
#  define BorderSettings_ ScnFontUniformBlockDataVS_XBorderSettings_
#  define ShadowSettings_ ScnFontUniformBlockDataVS_XShadowSettings_
#  define TextColour_ ScnFontUniformBlockDataVS_XTextColour_
#  define BorderColour_ ScnFontUniformBlockDataVS_XBorderColour_
#  define ShadowColour_ ScnFontUniformBlockDataVS_XShadowColour_

#endif

