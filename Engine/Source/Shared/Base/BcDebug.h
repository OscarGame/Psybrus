/**************************************************************************
*
* File:		BcDebug.h
* Author: 	Neil Richardson
* Ver/Date:
* Description:
*
*
*
*
*
**************************************************************************/

#ifndef __BCDEBUG_H__
#define __BCDEBUG_H__

#include "Base/BcTypes.h"
#include "Base/BcLog.h"

#include <string>

//////////////////////////////////////////////////////////////////////////
// BcPrintf
#if 1
void BcPrintf( const BcChar* Text, ... );
#else
#  define BcPrintf( ... )
#endif

//////////////////////////////////////////////////////////////////////////
// BcMessageBox
enum BcMessageBoxType
{
	bcMBT_OK = 0,
	bcMBT_OKCANCEL,
	bcMBT_YESNO,
	bcMBT_YESNOCANCEL
};

enum BcMessageBoxIcon
{
	bcMBI_WARNING = 0,
	bcMBI_ERROR,
	bcMBI_QUESTION
};

enum BcMessageBoxReturn
{
	bcMBR_OK = 0,
	bcMBR_YES = 0,
	bcMBR_NO = 1,
	bcMBR_CANCEL = 2,
};

extern BcMessageBoxReturn BcMessageBox( const BcChar* pTitle, const BcChar* pMessage, BcMessageBoxType Type = bcMBT_OK, BcMessageBoxIcon Icon = bcMBI_WARNING );

//////////////////////////////////////////////////////////////////////////
// BcBacktrace
struct BcBacktraceEntry
{
	void* Address_;
	std::string Symbol_;
};

struct BcBacktraceResult
{
	std::vector< BcBacktraceEntry > Backtrace_;
};

extern void BcPrintBacktrace( const BcBacktraceResult& Result );
extern BcBacktraceResult BcBacktrace();

//////////////////////////////////////////////////////////////////////////
// BcAssert
extern BcBool BcAssertInternal( const BcChar* pMessage, const BcChar* pFile, int Line, ... );

/**
 * Signature for assert handler.
 * @param Message.
 * @param File.
 * @param Line.
 * @return True if wanting to debug break.
 */
using BcAssertFunc = std::function< BcBool( const BcChar*, const BcChar*, int ) >;

/**
 * Set assert handler. Overwrites old.
 * @param Func Handler to use.
 */
extern void BcAssertSetHandler( BcAssertFunc Func );

/**
 * Get assert handler.
 * @return Current assert handler.
 */
extern BcAssertFunc BcAssertGetHandler();

/**
 * Utility class to set/restore an assert handler.
 * Should only be used from one thread.
 */
class BcAssertScopedHandler
{
public:
	BcAssertScopedHandler( BcAssertFunc Func ):
		OldFunc_( BcAssertGetHandler() )
	{
		BcAssertSetHandler( Func );
	}

	~BcAssertScopedHandler()
	{
		BcAssertSetHandler( OldFunc_ );
	}

private:
	BcAssertFunc OldFunc_;
};



#if ( defined( PSY_DEBUG ) || defined( PSY_RELEASE ) ) && 1
#  define BcAssertMsg( Condition, Message, ... )	\
	if( !( Condition ) ) \
	{ \
		if( BcAssertInternal( Message, __FILE__, __LINE__, ##__VA_ARGS__ ) ) \
			BcBreakpoint; \
	}
#  define BcAssert( Condition )	\
	if( !( Condition ) ) \
	{ \
		if( BcAssertInternal( #Condition, __FILE__, __LINE__ ) ) \
			BcBreakpoint; \
	}

#  define BcPreCondition( Condition )	BcAssert( Condition )
#  define BcPostCondition( Condition )	BcAssert( Condition )
#  define BcAssertException( Condition, Exception )	\
	if( !( Condition ) ) \
	{ \
		throw Exception; \
	}

#else
#  define BcAssertMsg( Condition, Message, ... )
#  define BcAssert( Condition )
#  define BcPreCondition( Condition )
#  define BcPostCondition( Condition )
#  define BcAssertException( Condition, Exception )	\
	if( !( Condition ) ) \
	{ \
		throw Exception; \
	}
#endif

//////////////////////////////////////////////////////////////////////////
// BcVerify
extern BcBool BcVerifyInternal( const BcChar* pMessage, const BcChar* pFile, int Line, ... );

#if ( defined( PSY_DEBUG ) || defined( PSY_RELEASE ) ) && 1
#  define BcVerifyMsg( Condition, Message, ... )	\
	{ \
		static BcBool ShouldNotify = BcTrue; \
		if( ShouldNotify && !( Condition ) ) \
		{ \
			if( BcVerifyInternal( Message, __FILE__, __LINE__, ##__VA_ARGS__ ) ) \
				ShouldNotify = BcFalse; \
		} \
	}
#  define BcVerify( Condition )			\
	{ \
		static BcBool ShouldNotify = BcTrue; \
		if( ShouldNotify && !( Condition ) ) \
		{ \
			if( BcVerifyInternal( #Condition, __FILE__, __LINE__ ) ) \
				ShouldNotify = BcFalse; \
		} \
	}
#else
#  define BcVerifyMsg( Condition, Message, ... )
#  define BcVerify( Condition )
#endif

#endif

