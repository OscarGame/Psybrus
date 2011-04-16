/**************************************************************************
*
* File:		RsProgramGL.h
* Author:	Neil Richardson 
* Ver/Date:	28/02/11	
* Description:
*		
*		
*
*
* 
**************************************************************************/

#ifndef __RSPROGRAMGL_H__
#define __RSPROGRAMGL_H__

#include "RsShaderGL.h"
#include "RsTextureGL.h"
#include "RsProgram.h"

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
class RsProgramParameterGL;
class RsProgramGL;

////////////////////////////////////////////////////////////////////////////////
// RsProgramParameterGL
class RsProgramParameterGL:
	public RsProgramParameter
{
public:
	RsProgramParameterGL( const std::string& Name, RsProgramGL* pParent, GLuint Parameter );
	virtual ~RsProgramParameterGL();
	
	void								setInt( BcS32 Value );
	void								setFloat( BcReal Value );
	void								setVector( const BcVec2d& Value );
	void								setVector( const BcVec3d& Value );
	void								setVector( const BcVec4d& Value );
	void								setMatrix( const BcMat4d& Value );

private:
	RsProgramGL*						pParent_;
	GLint								Parameter_;
};

////////////////////////////////////////////////////////////////////////////////
// RsProgramGL
class RsProgramGL:
	public RsProgram
{
public:
	RsProgramGL( RsShaderGL* pVertexShader, RsShaderGL* pFragmentShader );
	virtual ~RsProgramGL();
	
	void								create();
	void								update();
	void								destroy();	
	
	virtual RsProgramParameter*			findParameter( const std::string& Name );
	virtual void						bind();

private:	
	void								bindAttribute( eRsVertexChannel Channel, const BcChar* Name );

private:
	typedef std::list< RsProgramParameterGL* >	TParameterList;
	typedef TParameterList::iterator				TParameterListIterator;
	TParameterList						ParameterList_;

	RsShaderGL*						pVertexShader_;
	RsShaderGL*						pFragmentShader_;	
	
	BcU32								TotalSampler_;
};

#endif
