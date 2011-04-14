/**************************************************************************
*
* File:		ImgImage.cpp
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*
*		
*
*
* 
**************************************************************************/

#include "ImgImage.h"

//////////////////////////////////////////////////////////////////////////
// Ctor
ImgImage::ImgImage()
{
	Width_ = 0;
	Height_ = 0;
	Format_ = imgFMT_RGBA;
	pPixelData_ = NULL;
	pPaletteEntries_ = NULL;
	NoofPaletteEntries_ = 0;
}

//////////////////////////////////////////////////////////////////////////
// Dtor
ImgImage::~ImgImage()
{
	delete [] pPixelData_;
	delete [] pPaletteEntries_;
}

//////////////////////////////////////////////////////////////////////////
// Copy Ctor
ImgImage::ImgImage( const ImgImage& Original )
{
	Width_ = 0;
	Height_ = 0;
	Format_ = imgFMT_RGB;
	pPixelData_ = NULL;
	pPaletteEntries_ = NULL;
	NoofPaletteEntries_ = 0;

	//
	BcAssertException( pPixelData_ != NULL, ImgException( "ImgImage: Pixel data is NULL." ) );
 
	create( Original.Width_, Original.Height_, Original.Format_ );
	
	if( Original.pPixelData_ != NULL )
	{
		BcMemCopy( pPixelData_, Original.pPixelData_, sizeof( ImgColour ) * Width_ * Height_ );
	}

	if( Original.pPaletteEntries_ != NULL )
	{
		BcMemCopy( pPaletteEntries_, Original.pPaletteEntries_, sizeof( ImgColour ) * NoofPaletteEntries_ );
	}
}

//////////////////////////////////////////////////////////////////////////
// create
void ImgImage::create( BcU32 Width, BcU32 Height, eImgFormat Format, const ImgColour* pFillColour )
{
	Width_ = Width;
	Height_ = Height;
	Format_ = Format;
	pPixelData_ = new ImgColour[ Width * Height ];
	
	// Palette.
	switch( Format )
	{
	case imgFMT_RGB:
	case imgFMT_RGBA:
		NoofPaletteEntries_ = 0;
		break;

	case imgFMT_INDEXED:
		NoofPaletteEntries_ = 256;
		pPaletteEntries_ = new ImgColour[ NoofPaletteEntries_ ];
		break;
	}
	
	if( pFillColour != NULL )
	{
		for( BcU32 i = 0; i < Width_; ++i )
		{
			for( BcU32 j = 0; j < Height_; ++j )
			{
				setPixel( i, j, *pFillColour );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// setPixel
void ImgImage::setPixel( BcU32 X, BcU32 Y, const ImgColour& Colour )
{
	BcAssertException( pPixelData_ != NULL, ImgException( "ImgImage: Pixel data is NULL." ) );
	BcAssertException( X < Width_, ImgException( "ImgImage: X out of bounds." ) );
	BcAssertException( Y < Height_, ImgException( "ImgImage: Y out of bounds." ) );

	BcU32 Index = X + ( Y * Width_ );
	
	pPixelData_[ Index ] = findColour( Colour );
}

//////////////////////////////////////////////////////////////////////////
// getPixel
const ImgColour& ImgImage::getPixel( BcU32 X, BcU32 Y ) const
{
	BcAssertException( pPixelData_ != NULL, ImgException( "ImgImage: Pixel data is NULL." ) );
	BcAssertException( X < Width_, ImgException( "ImgImage: X out of bounds." ) );
	BcAssertException( Y < Height_, ImgException( "ImgImage: Y out of bounds." ) );

	BcU32 Index = X + ( Y * Width_ );
	return pPixelData_[ Index ];	
}

//////////////////////////////////////////////////////////////////////////
// setPalette
void ImgImage::setPalette( BcU32 Idx, const ImgColour& Colour )
{
	BcAssertException( pPaletteEntries_ != NULL, ImgException( "ImgImage: Palette data is NULL." ) );
	BcAssertException( Idx < NoofPaletteEntries_, ImgException( "ImgImage: Idx out of bounds." ) );

	pPaletteEntries_[ Idx ] = Colour;
}

//////////////////////////////////////////////////////////////////////////
// getPalette
const ImgColour& ImgImage::getPalette( BcU32 Idx ) const
{
	BcAssertException( pPaletteEntries_ != NULL, ImgException( "ImgImage: Palette data is NULL." ) );
	BcAssertException( Idx < NoofPaletteEntries_, ImgException( "ImgImage: Idx out of bounds." ) );

	return pPaletteEntries_[ Idx ];
}

//////////////////////////////////////////////////////////////////////////
// findColour
ImgColour ImgImage::findColour( const ImgColour& Colour )
{
	// TODO: Implement.
	return Colour;
}

//////////////////////////////////////////////////////////////////////////
// resize
ImgImage* ImgImage::resize( BcU32 Width, BcU32 Height )
{
	// Create image.
	ImgImage* pImage = new ImgImage();
	pImage->create( Width, Height, Format_ );

	if( Width != ( Width_ >> 1 ) || Height != ( Height_ >> 1 ) )
	{
		BcReal SrcW = BcReal( Width_ - 1 );
		BcReal SrcH = BcReal( Height_ - 1 );

		// Bilinear filtering implementation.
		for( BcU32 iX = 0; iX < Width; ++iX )
		{
			BcReal iXF = BcReal( iX ) / BcReal( Width );
			BcReal iSrcXF = SrcW * iXF;
			BcU32 iSrcX = BcU32( iSrcXF );
			BcReal iLerpX = iSrcXF - BcReal( iSrcX );

			for( BcU32 iY = 0; iY < Height; ++iY )
			{
				BcReal iYF = BcReal( iY ) / BcReal( Height );
				BcReal iSrcYF = SrcW * iYF;
				BcU32 iSrcY = BcU32( iSrcYF );
				BcReal iLerpY = iSrcYF - BcReal( iSrcY );

				const ImgColour& PixelA = getPixel( iSrcX, iSrcY );
				const ImgColour& PixelB = getPixel( iSrcX + 1, iSrcY );
				const ImgColour& PixelC = getPixel( iSrcX, iSrcY + 1 );
				const ImgColour& PixelD = getPixel( iSrcX + 1, iSrcY + 1 );

				ImgColour DstPixelT;
				ImgColour DstPixelB;
				ImgColour DstPixel;

				DstPixelT.R_ = BcU8( ( BcReal( PixelA.R_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelB.R_ ) * iLerpX ) );
				DstPixelT.G_ = BcU8( ( BcReal( PixelA.G_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelB.G_ ) * iLerpX ) );
				DstPixelT.B_ = BcU8( ( BcReal( PixelA.B_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelB.B_ ) * iLerpX ) );
				DstPixelT.A_ = BcU8( ( BcReal( PixelA.A_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelB.A_ ) * iLerpX ) );
				DstPixelB.R_ = BcU8( ( BcReal( PixelC.R_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelD.R_ ) * iLerpX ) );
				DstPixelB.G_ = BcU8( ( BcReal( PixelC.G_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelD.G_ ) * iLerpX ) );
				DstPixelB.B_ = BcU8( ( BcReal( PixelC.B_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelD.B_ ) * iLerpX ) );
				DstPixelB.A_ = BcU8( ( BcReal( PixelC.A_ ) * ( 1.0f - iLerpX ) ) + ( BcReal( PixelD.A_ ) * iLerpX ) );

				DstPixel.R_ = BcU8( ( BcReal( DstPixelT.R_ ) * ( 1.0f - iLerpY ) ) + ( BcReal( DstPixelB.R_ ) * iLerpY ) );
				DstPixel.G_ = BcU8( ( BcReal( DstPixelT.G_ ) * ( 1.0f - iLerpY ) ) + ( BcReal( DstPixelB.G_ ) * iLerpY ) );
				DstPixel.B_ = BcU8( ( BcReal( DstPixelT.B_ ) * ( 1.0f - iLerpY ) ) + ( BcReal( DstPixelB.B_ ) * iLerpY ) );
				DstPixel.A_ = BcU8( ( BcReal( DstPixelT.A_ ) * ( 1.0f - iLerpY ) ) + ( BcReal( DstPixelB.A_ ) * iLerpY ) );		

				pImage->setPixel( iX, iY, DstPixel );
			}
		}
	}
	else
	{
		// Better implementation for downscaling.
		for( BcU32 iX = 0; iX < Width; ++iX )
		{
			BcU32 iSrcX = iX << 1;
			for( BcU32 iY = 0; iY < Height; ++iY )
			{
				BcU32 iSrcY = iY << 1;

				const ImgColour& PixelA = getPixel( iSrcX, iSrcY );
				const ImgColour& PixelB = getPixel( iSrcX + 1, iSrcY );
				const ImgColour& PixelC = getPixel( iSrcX, iSrcY + 1 );
				const ImgColour& PixelD = getPixel( iSrcX + 1, iSrcY + 1 );

				ImgColour DstPixel;

				DstPixel.R_ = BcU8( ( BcU32( PixelA.R_ ) +
				                      BcU32( PixelB.R_ ) +
				                      BcU32( PixelC.R_ ) +
				                      BcU32( PixelD.R_ ) ) >> 2 );
				DstPixel.G_ = BcU8( ( BcU32( PixelA.G_ ) +
				                      BcU32( PixelB.G_ ) +
				                      BcU32( PixelC.G_ ) +
				                      BcU32( PixelD.G_ ) ) >> 2 );
				DstPixel.B_ = BcU8( ( BcU32( PixelA.B_ ) +
				                      BcU32( PixelB.B_ ) +
				                      BcU32( PixelC.B_ ) +
				                      BcU32( PixelD.B_ ) ) >> 2 );
				DstPixel.A_ = BcU8( ( BcU32( PixelA.A_ ) +
				                      BcU32( PixelB.A_ ) +
				                      BcU32( PixelC.A_ ) +
				                      BcU32( PixelD.A_ ) ) >> 2 );

				pImage->setPixel( iX, iY, DstPixel );
			}
		}
	}

	return pImage;
}

//////////////////////////////////////////////////////////////////////////
// generateMipMaps
BcU32 ImgImage::generateMipMaps( BcU32 NoofLevels, ImgImage** ppOutImages )
{
	BcU32 LevelsCreated = 1;

	// Assign first as ourself.
	*ppOutImages = this;

	ImgImage* pPrevImage = *ppOutImages;
	++ppOutImages;

	// Generate smaller images.
	for( BcU32 i = 0; i < ( NoofLevels - 1 ); ++i )
	{
		BcU32 W = pPrevImage->width() >> 1;
		BcU32 H = pPrevImage->height() >> 1;

		// Bail if target is too small.
		if( W < 8 || H < 8 )
		{
			break;
		}

		// Perform resize.
		*ppOutImages = pPrevImage->resize( W, H );
		pPrevImage = *ppOutImages;
		++ppOutImages;
		++LevelsCreated;
	}

	return LevelsCreated;
}

//////////////////////////////////////////////////////////////////////////
// width
BcU32 ImgImage::width() const
{
	return Width_;
}

//////////////////////////////////////////////////////////////////////////
// height
BcU32 ImgImage::height() const
{
	return Height_;
}

//////////////////////////////////////////////////////////////////////////
// format
eImgFormat ImgImage::format() const
{
	return Format_;
}

//////////////////////////////////////////////////////////////////////////
// getImageData
const ImgColour* ImgImage::getImageData() const
{
	return pPixelData_;
}
