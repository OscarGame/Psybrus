/**************************************************************************
*
* File:		ScnSoundImport.h
* Author:	Neil Richardson 
* Ver/Date:	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __ScnSoundImport_H__
#define __ScnSoundImport_H__

#include "System/Content/CsCore.h"
#include "System/Content/CsResourceImporter.h"
#include "System/Scene/Sound/ScnSoundFileData.h"
#include "System/Sound/SsSource.h"

//////////////////////////////////////////////////////////////////////////
// ScnSoundImport
class ScnSoundImport:
	public CsResourceImporter
{
public:
	REFLECTION_DECLARE_DERIVED_MANUAL_NOINIT( ScnSoundImport, CsResourceImporter );

public:
	ScnSoundImport();
	ScnSoundImport( ReNoInit );
	virtual ~ScnSoundImport();

	/**
	 * Import.
	 */
	BcBool import() override;

private:
	std::string Source_;
	BcBool IsStream_;
	BcBool IsLoop_;
	SsSourceFileData FileData_;
};

#endif
