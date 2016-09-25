/**************************************************************************
*
* File:		DsCore.h
* Author:	Neil Richardson 
* Ver/Date:	11/12/12	
* Description:
*		Debug system core.
*		
*
*
* 
**************************************************************************/

#ifndef __DSCORE_H__
#define __DSCORE_H__

#include "Base/BcGlobal.h"
#include "Base/BcHtml.h"

#include "System/SysSystem.h"
#include "System/Debug/DsImGui.h"
#include "System/Debug/DsImGuiFieldEditor.h"

#include <functional>
#include <map>
#include <mutex>
#include <regex>

//////////////////////////////////////////////////////////////////////////
// Typedefs.
typedef const std::vector< std::string >& DsParameters;


//////////////////////////////////////////////////////////////////////////
/**	\class DsCore
*	\brief Debug System Core
*
*	Debugging system core.
*/
class DsCore :
	public BcGlobal< DsCore >,
	public SysSystem
{
public:
	static BcU32 JOB_QUEUE_ID;
	static const BcU32 DsCoreSerialised = 0x00008000;
public:
	DsCore();
	virtual ~DsCore();

	virtual void open() = 0;
	virtual void update() = 0;
	virtual void close() = 0;

	/**
	 * Register for panel to be drawn.
	 * @param Category Category panel is listed under.
	 * @param Name Name of the panel.
	 * @param Shortcut Keyboard shortcut for panel.
	 */
	virtual BcU32 registerPanel( 
		const char* Category, const char* Name, const char* Shortcut,
		std::function< void( BcU32 )> Func ) = 0;

	/**
	 * Deregister panl.
	 */
	virtual void deregisterPanel( BcU32 Handle ) = 0;

	/**
	 * Add view overlay.
	 * Adds a debug overlay for a given view. Must be called each frame.
	 */
	virtual void addViewOverlay( const class MaMat4d& View, const class MaMat4d& Proj, const struct RsViewport& Viewport ) = 0;

	virtual BcU32 registerPage( 
		std::string regex, 
		std::vector< std::string > namedCaptures, 
		std::function < void( DsParameters, BcHtmlNode&, std::string )> fn, 
		std::string display ) = 0;

	virtual BcU32 registerPage( 
		std::string regex, 
		std::vector< std::string > namedCaptures, 
		std::function < void( DsParameters, BcHtmlNode&, std::string) > fn ) = 0;

	virtual BcU32 registerPageNoHtml( 
		std::string regex, 
		std::vector< std::string > namedCaptures, 
		std::function < void( DsParameters, BcHtmlNode&, std::string )> fn ) = 0;
	
	virtual void deregisterPage( BcU32 Handle ) = 0;

	virtual BcU32 registerFunction( 
		std::string Display, 
		std::function< void() > Function ) = 0;

	virtual void deregisterFunction( BcU32 Handle ) = 0;

private:
};


#endif
