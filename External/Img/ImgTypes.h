/**************************************************************************
*
* File:		ImgTypes.h
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*
*		
*
*
* 
**************************************************************************/

#ifndef __IMGTYPES_H__
#define __IMGTYPES_H__

//////////////////////////////////////////////////////////////////////////
// Includes
#include "BcTypes.h"
#include "BcDebug.h"
#include "BcMemory.h"
#include "BcString.h"

//////////////////////////////////////////////////////////////////////////
// ImgColour
class ImgColour
{
public:
	BcU8		B_;
	BcU8		G_;
	BcU8		R_;
	BcU8		A_;
};

//////////////////////////////////////////////////////////////////////////
// Format
enum eImgFormat
{
	imgFMT_RGB = 0,
	imgFMT_RGBA,
	imgFMT_INDEXED,
};

#endif
