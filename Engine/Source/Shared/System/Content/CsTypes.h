/**************************************************************************
*
* File:		CsTypes.h
* Author:	Neil Richardson 
* Ver/Date:	7/03/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __CSTYPES_H__
#define __CSTYPES_H__

#include "Base/BcTypes.h"
#include "Base/BcDebug.h"
#include "Base/BcName.h"
#include "Base/BcPath.h"

#include "System/File/FsFile.h"
#include "Base/BcEndian.h"
#include "Base/BcHash.h"

#include <atomic>
#include <mutex>
#include <exception>

#undef ERROR

//////////////////////////////////////////////////////////////////////////
// Paths
class CsPaths
{
public:
	static const BcPath PACKED_CONTENT;
	static const BcPath INTERMEDIATE;
	static const BcPath CONTENT;
	static const BcPath PSYBRUS_CONTENT;

	/**
	 * Resolve content path. Will search CONTENT, followed by PSYBRUS_CONTENT,
	 * and finally just the path itself. (for generated/external content).
	 */
	static const BcPath resolveContent( const char* FileName );
};

//////////////////////////////////////////////////////////////////////////
// CsCrossRefId
typedef BcU32 CsCrossRefId;
static const CsCrossRefId CSCROSSREFID_INVALID = static_cast< CsCrossRefId >( -1 );

//////////////////////////////////////////////////////////////////////////
// Callbacks
typedef std::function< void( class CsPackage*, BcU32 ) > CsPackageReadyCallback;

//////////////////////////////////////////////////////////////////////////
// CsDependencyList
typedef std::list< class CsDependency > CsDependencyList;
typedef CsDependencyList::iterator CsDependencyListIterator;

//////////////////////////////////////////////////////////////////////////
// CsMessageCategory
enum class CsMessageCategory
{
	INFO = 0,	///!< Information. No action required.
	WARNING,	///!< Warning. Could continue, but something is still wrong.
	ERROR,		///!< Error. Can't import resource, may continue trying to accumulate errors & warnings.
	CRITICAL,	///!< Critical. Can't import resource, unable to continue on.

	MAX
};

//////////////////////////////////////////////////////////////////////////
// CsDependency
class CsDependency
{
public:
	REFLECTION_DECLARE_BASIC( CsDependency );

	CsDependency();
	CsDependency( const std::string& FileName );
	CsDependency( const std::string& FileName, const FsStats& Stats );
	CsDependency( const CsDependency& Other );
	~CsDependency();

	/**
	 * Get file name.
	 */
	const std::string& getFileName() const;

	/**
	 * Get stats.
	 */
	const FsStats& getStats() const;

	/**
	 * Had dependancy changed?
	 */
	BcBool hasChanged() const;

	/**
	 * Update stats.
	 */
	void updateStats();

	/**
	 * Comparison.
	 */
	bool operator < ( const CsDependency& Dep ) const;

private:
	std::string FileName_;
	FsStats Stats_;
};

//////////////////////////////////////////////////////////////////////////
// CsFileHash
struct CsFileHash
{
	std::string getName() const;

	BcU32 Hash_[ 5 ];
};

//////////////////////////////////////////////////////////////////////////
// CsImportException
class CsImportException:
	public std::exception
{
public:
	CsImportException( 
		const char* File,
		const char* Error,
		... ) NOEXCEPT;
	const char* what() const NOEXCEPT;
	const char* file() const NOEXCEPT;
	const char* error() const NOEXCEPT;

private:
	BcChar File_[ 1024 ];
	BcChar Error_[ 4096 ];
};

#endif
