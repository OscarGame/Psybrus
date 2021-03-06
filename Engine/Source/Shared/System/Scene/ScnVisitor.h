/**************************************************************************
*
* File:		SgVisitor.h
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		Scenegraph Visitor
*			
*
*
* 
**************************************************************************/

#ifndef __ScnVisitor_H__
#define __ScnVisitor_H__

#include "System/Scene/ScnTypes.h"

//////////////////////////////////////////////////////////////////////////
// Macro
#define DECLARE_VISITABLE_BASE( _ClassName ) \
	virtual void visit_accept( ScnVisitor* pVisitor ) \
	{ \
		pVisitor->visit( (_ClassName*)this ); \
	}

#define DECLARE_VISITABLE( _ClassName ) \
	void visit_accept( ScnVisitor* pVisitor ) override \
	{ \
		pVisitor->visit( (_ClassName*)this ); \
	}

//////////////////////////////////////////////////////////////////////////
// ScnVisitor
class ScnVisitor
{
public:
	ScnVisitor();
	virtual ~ScnVisitor();

	/**
	 * Called before recursing into the scene.
	 */
	virtual void preScene();

	/**
	 * Called after recursing into the scene.
	 */
	virtual void postScene();

	/**
	 * Called when a component is hit.
	 */
	virtual void visit( class ScnSpatialComponent* pComponent );

	/**
	 * Called when a component is hit.
	 */
	virtual void visit( class ScnRenderableComponent* pComponent );

	/**
	 * Called when a component is hit.
	 */
	virtual void visit( class ScnLightComponent* pComponent );

private:

};


#endif
