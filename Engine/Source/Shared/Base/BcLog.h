/**************************************************************************
*
* File:		BcLog.h
* Author: 	Neil Richardson
* Ver/Date:	
* Description:
*		Logging system.
*		
*		
*		
* 
**************************************************************************/

#ifndef __BCLOG_H__
#define __BCLOG_H__

#include "Base/BcTypes.h"
#include "Base/BcGlobal.h"
#include "Base/BcMisc.h"
#include "Base/BcName.h"
#include <vector>

//////////////////////////////////////////////////////////////////////////
/* @class BcLog
 * @brief Abstract interface and global access for logging.
 */
class BcLog:
	public BcGlobal< BcLog >
{
public:
	BcLog(){};
	virtual ~BcLog(){};

	/**
	 * Write to log.
	 */
	virtual void write( const BcChar* pText, ... ) = 0;
	
	/**
	 * Flush log.
	 */
	virtual void flush() = 0;

	/**
	 * Set category suppression.
	 */
	virtual void setCategorySuppression( const std::string& Category, BcBool IsSuppressed ) = 0;

	/**
	 * Get category suppression.
	 */
	virtual BcBool getCategorySuppression( const std::string& Category ) const = 0;

	/*
	 * Get log data
	 */
	virtual std::vector<std::string> getLogData() = 0;

protected:
	friend class BcLogScopedCategory;
	friend class BcLogScopedIndent;
	friend class BcLogListener;

	/**
	 * Register listener.
	 */
	virtual void registerListener( class BcLogListener* Listener ) = 0;

	/**
	 * Deregister listener.
	 */
	virtual void deregisterListener( class BcLogListener* Listener ) = 0;

	/**
	 * Set category.
	 */
	virtual void setCategory( const std::string& Category ) = 0;

	/**
	 * Get category.
	 */
	virtual std::string getCategory() = 0;

	/**
	 * Increase indent.
	 */
	virtual void increaseIndent() = 0;

	/**
	 * Decreate indent.
	 */
	virtual void decreaseIndent() = 0;
};

//////////////////////////////////////////////////////////////////////////
/* @class BcLogEntry
 * @brief Log entry containing all useful, loggable information per
 *        user log entry.
 */
struct BcLogEntry
{
	BcF64 Time_;
	BcThreadId ThreadId_;
	std::string Category_;
	int Indent_;
	std::string Text_;
};

//////////////////////////////////////////////////////////////////////////
/**
 * @class BcLogListener
 * @brief Interface for receiving callbacks from the logging system.
 */
class BcLogListener
{
public:
	BcLogListener();
	BcLogListener( const BcLogListener& Other );
	virtual ~BcLogListener();

	/**
	 * Called when there has been any logging occur.
	 */
	virtual void onLog( const BcLogEntry& Entry ) = 0;
};

//////////////////////////////////////////////////////////////////////////
/* @class BcLogScopedCategory
 * @brief Scoped log Category setting to set/unset current Category.
 */
class BcLogScopedCategory
{
public:
	BcLogScopedCategory( const std::string& Category );
	~BcLogScopedCategory();

private:
	std::string OldCategory_;
};

//////////////////////////////////////////////////////////////////////////
/* @class BcLogScopedIndent
 * @brief Scoped log indent setting to indent/unindent.
 */
class BcLogScopedIndent
{
public:
	BcLogScopedIndent();
	~BcLogScopedIndent();
};

//////////////////////////////////////////////////////////////////////////
// Macros
#if !PSY_PRODUCTION
#  define PSY_LOGSCOPEDCATEGORY( _NAME ) \
	BcLogScopedCategory _LogScopedCategory_##__LINE__( std::string( _NAME ) );

#  define PSY_LOGSCOPEDINDENT \
	BcLogScopedIndent _LogScopedIndent_##__LINE__;

#  define PSY_LOG \
	if( BcLog::pImpl() ) BcLog::pImpl()->write

#else
#  define PSY_LOGSCOPEDCATEGORY( _NAME )
#  define PSY_LOGSCOPEDINDENT
#  define PSY_LOG
#endif

#endif
