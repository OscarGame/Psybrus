/**************************************************************************
*
* File:		RsViewport.h
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __RSVIEWPORT_H__
#define __RSVIEWPORT_H__

#include "Math/MaMat4d.h"
#include "Math/MaAABB.h"
#include "Math/MaPlane.h"

//////////////////////////////////////////////////////////////////////////
// RsViewport
struct RsViewport
{
public:
	RsViewport();
	RsViewport( BcU32 X, BcU32 Y, BcU32 Width, BcU32 Height, BcF32 ZNear = 1.0f, BcF32 ZFar = 1024.0f );

	//
	void viewport( BcU32 X, BcU32 Y, BcU32 Width, BcU32 Height, BcF32 ZNear = 1.0f, BcF32 ZFar = 1024.0f );

	bool operator == ( const RsViewport& B ) const;
	bool operator != ( const RsViewport& B ) const;

public:
	BcU32 x() const;
	BcU32 y() const;

	BcU32 width() const;
	BcU32 height() const;

	BcF32 zFar() const;
	BcF32 zNear() const;


private:
	// Viewport
	BcU32 Top_;
	BcU32 Bottom_;
	BcU32 Left_;
	BcU32 Right_;
	
	//
	BcF32 ZFar_;
	BcF32 ZNear_;

};

//////////////////////////////////////////////////////////////////////////
// Inlines
inline RsViewport::RsViewport( BcU32 X, BcU32 Y, BcU32 Width, BcU32 Height, BcF32 ZNear, BcF32 ZFar )
{
	viewport( X, Y, Width, Height, ZNear, ZFar );
}

inline void RsViewport::viewport( BcU32 X, BcU32 Y, BcU32 Width, BcU32 Height, BcF32 ZNear, BcF32 ZFar )
{
	Top_ = Y;
	Bottom_ = ( Y + Height ) - 1;
	Left_ = X;
	Right_ = ( X + Width ) - 1;
	ZNear_ = ZNear;
	ZFar_ = ZFar;
}

inline bool RsViewport::operator == ( const RsViewport& B ) const
{
	return  Top_ == B.Top_ &&
		Bottom_ == B.Bottom_ &&
		Left_ == B.Left_ &&
		Right_ == B.Right_ &&
		ZNear_ == B.ZNear_ &&
		ZFar_ == B.ZFar_;
}

inline bool RsViewport::operator != ( const RsViewport& B ) const
{
	return  Top_ != B.Top_ ||
		Bottom_ != B.Bottom_ ||
		Left_ != B.Left_ ||
		Right_ != B.Right_ ||
		ZNear_ != B.ZNear_ ||
		ZFar_ != B.ZFar_;
}

inline BcU32 RsViewport::x() const
{
	return Left_;
}

inline BcU32 RsViewport::y() const
{
	return Top_;
}

inline BcU32 RsViewport::width() const
{
	return ( Right_ - Left_ ) + 1;
}

inline BcU32 RsViewport::height() const
{
	return ( Bottom_ - Top_ ) + 1;
}

inline BcF32 RsViewport::zFar() const
{
	return ZFar_;
}

inline BcF32 RsViewport::zNear() const
{
	return ZNear_;
}

#endif
