/**************************************************************************
*
* File:		cTypes.h
* Author: 	Neil Richardson
* Ver/Date:
* Description:
*
*
*
*
*
**************************************************************************/

#ifndef __BCTYPES_H__
#define __BCTYPES_H__

#include "Base/BcPortability.h"

//////////////////////////////////////////////////////////////////////////
// Windows defines
#if PLATFORM_WINDOWS || PLATFORM_WINPHONE

#include <new>
#include <cstdint>

#pragma warning ( disable : 4311 )
#pragma warning ( disable : 4312 )
#pragma warning ( disable : 4996 )
#pragma warning ( disable : 4127 )
#pragma warning ( disable : 4100 )
#pragma warning ( disable : 4201 )
#pragma warning ( disable : 4706 ) // Assignment within conditional expression.

typedef std::uint64_t				BcU64;
typedef std::uint32_t				BcU32;
typedef	std::uint16_t				BcU16;
typedef std::uint8_t				BcU8;

typedef std::int64_t				BcS64;
typedef std::int32_t				BcS32;
typedef	std::int16_t				BcS16;
typedef std::int8_t					BcS8;

typedef	float						BcF32;
typedef	double						BcF64;
typedef char						BcChar;
typedef BcU32						BcBool;
typedef void*						BcHandle;
typedef size_t						BcSize;

#define BcTrue						BcBool( 1 )
#define BcFalse						BcBool( 0 )

#if COMPILER_MSVC
#  define BcBreakpoint				__debugbreak()
#else
#  define BcBreakpoint				asm( "int $3" )
#endif

#define BcErrorCode					0xffffffff
#define BcLogWrite( t )
#define BcUnusedVar( t )			(void)t

#define BcInline					inline

#if COMPILER_MSVC
#define BcForceInline				__forceinline
#else
#define BcForceInline				inline
#endif

#define BcAlign( decl, v )			decl // TODO: Get rid of vectors in stl containers: __declspec( align( v ) ) decl
#define BcOffsetOf( s, m ) 			(size_t)&(((s *)0)->m)

#define BcPrefetch( a )				_mm_prefetch( reinterpret_cast<char*>a, _MM_HINT_NTA )

#define BcArraySize( a )			( sizeof( a ) / sizeof( a[0] ) )

#ifndef NULL
#define NULL						( 0 )
#endif

// Redefine min and max.
#undef min
#define min min

#undef max
#define max max

// array hack.
#include <array>

namespace std
{
	using std::tr1::array;
};

// cmath will define this. We say no.
#undef DOMAIN

#endif

//////////////////////////////////////////////////////////////////////////
// Linux defines
#if PLATFORM_LINUX

#include <new>
#include <cstdint>
#include <unistd.h>
#include <signal.h>

typedef std::uint64_t				BcU64;
typedef std::uint32_t				BcU32;
typedef	std::uint16_t				BcU16;
typedef std::uint8_t				BcU8;

typedef std::int64_t				BcS64;
typedef std::int32_t				BcS32;
typedef	std::int16_t				BcS16;
typedef std::int8_t					BcS8;

typedef	float						BcF32;
typedef	double						BcF64;
typedef char						BcChar;
typedef BcU32						BcBool;
typedef void*						BcHandle;
typedef std::size_t					BcSize;

#define BcTrue						BcBool( 1 )
#define BcFalse						BcBool( 0 )

#define BcBreakpoint				raise( SIGTRAP )

#define BcErrorCode					0xffffffff
#define BcLogWrite( t )
#define BcUnusedVar( t )			(void)t

#define BcInline					inline
#define BcForceInline				inline

#define BcAlign( decl, v )			decl // TODO: Get rid of vectors in stl containers: __declspec( align( v ) ) decl
#define BcOffsetOf( s, m ) 			(size_t)&(((s *)0)->m)

#define BcPrefetch( a )				

#define BcArraySize( a )			( sizeof( a ) / sizeof( a[0] ) )

#ifndef NULL
#define NULL						( 0 )
#endif

// array hack.
#include <array>

// cmath will define this. We say no.
#undef DOMAIN

#endif

//////////////////////////////////////////////////////////////////////////
// OSX defines
#if PLATFORM_OSX

#include <new>
#include <cstdint>
#include <unistd.h>
#include <signal.h>

typedef std::uint64_t				BcU64;
typedef std::uint32_t				BcU32;
typedef	std::uint16_t				BcU16;
typedef std::uint8_t				BcU8;

typedef std::int64_t				BcS64;
typedef std::int32_t				BcS32;
typedef	std::int16_t				BcS16;
typedef std::int8_t					BcS8;

typedef	float						BcF32;
typedef	double						BcF64;
typedef char						BcChar;
typedef BcU32						BcBool;
typedef void*						BcHandle;
typedef std::size_t					BcSize;

#define BcTrue						BcBool( 1 )
#define BcFalse						BcBool( 0 )

#define BcBreakpoint				raise( SIGTRAP )

#define BcErrorCode					0xffffffff
#define BcLogWrite( t )
#define BcUnusedVar( t )			(void)t

#define BcInline					inline
#define BcForceInline				inline

#define BcAlign( decl, v )			decl // TODO: Get rid of vectors in stl containers: __declspec( align( v ) ) decl
#define BcOffsetOf( s, m ) 			(size_t)&(((s *)0)->m)

#define BcPrefetch( a )				

#define BcArraySize( a )			( sizeof( a ) / sizeof( a[0] ) )

#ifndef NULL
#define NULL						( 0 )
#endif

// array hack.
#include <array>

// cmath will define this. We say no.
#undef DOMAIN

#endif


//////////////////////////////////////////////////////////////////////////
// HTML5 (emscripten) defines
#if PLATFORM_HTML5

#include <emscripten.h>
#include <new>
#include <cstdint>
#include <cstdlib>
#include <cassert>

typedef std::uint64_t				BcU64;
typedef std::uint32_t				BcU32;
typedef	std::uint16_t				BcU16;
typedef std::uint8_t				BcU8;

typedef std::int64_t				BcS64;
typedef std::int32_t				BcS32;
typedef	std::int16_t				BcS16;
typedef std::int8_t					BcS8;

typedef	float						BcF32;
typedef	double						BcF64;
typedef char						BcChar;
typedef BcU32						BcBool;
typedef void*						BcHandle;
typedef std::size_t					BcSize;

#define BcTrue						BcBool( 1 )
#define BcFalse						BcBool( 0 )

#if !PSY_PRODUCTION
#define BcBreakpoint				emscripten_debugger()
#else
#define BcBreakpoint				
#endif

#define BcErrorCode					0xffffffff
#define BcLogWrite( t )
#define BcUnusedVar( t )			(void)t

#define BcInline					inline
#define BcForceInline				inline

#define BcAlign( decl, v )			decl // TODO: Get rid of vectors in stl containers: __declspec( align( v ) ) decl
#define BcOffsetOf( s, m ) 			(size_t)&(((s *)0)->m)

#define BcPrefetch( a )				

#define BcArraySize( a )			( sizeof( a ) / sizeof( a[0] ) )

#ifndef NULL
#define NULL						( 0 )
#endif

// array hack.
#include <array>

// cmath will define this. We say no.
#undef DOMAIN

#endif


//////////////////////////////////////////////////////////////////////////
// Android defines
#if PLATFORM_ANDROID

#include <new>
#include <cstdint>
#include <unistd.h>
#include <signal.h>
#include <cassert>

typedef std::uint64_t				BcU64;
typedef std::uint32_t				BcU32;
typedef	std::uint16_t				BcU16;
typedef std::uint8_t				BcU8;

typedef std::int64_t				BcS64;
typedef std::int32_t				BcS32;
typedef	std::int16_t				BcS16;
typedef std::int8_t					BcS8;

typedef	float						BcF32;
typedef	double						BcF64;
typedef char						BcChar;
typedef BcU32						BcBool;
typedef void*						BcHandle;
typedef std::size_t					BcSize;

#define BcTrue						BcBool( 1 )
#define BcFalse						BcBool( 0 )

#define BcBreakpoint				raise( SIGTRAP )

#define BcErrorCode					0xffffffff
#define BcLogWrite( t )
#define BcUnusedVar( t )			(void)t

#define BcInline					inline
#define BcForceInline				inline

#define BcAlign( decl, v )			decl // TODO: Get rid of vectors in stl containers: __declspec( align( v ) ) decl
#define BcOffsetOf( s, m ) 			(size_t)&(((s *)0)->m)

#define BcPrefetch( a )				

#define BcArraySize( a )			( sizeof( a ) / sizeof( a[0] ) )

#ifndef NULL
#define NULL						( 0 )
#endif

// array hack.
#include <array>

// cmath will define this. We say no.
#undef DOMAIN

#endif

//////////////////////////////////////////////////////////////////////////
// Enum class flag operators.
#define DEFINE_ENUM_CLASS_FLAG_OPERATOR( _Type, _Operator ) \
	inline _Type operator _Operator##= ( _Type& A, _Type B ) \
	{ \
		A = (_Type)( (int)A _Operator (int)B ); \
		return A; \
	} \
	inline _Type operator _Operator ( _Type A, _Type B ) \
	{ \
		return (_Type)( (int)A _Operator (int)B ); \
	} 

#define DEFINE_ENUM_CLASS_UNARY_FLAG_OPERATOR( _Type, _Operator ) \
	inline _Type operator _Operator ( _Type A ) \
	{ \
		return (_Type)( _Operator (int)A ); \
	} 

//////////////////////////////////////////////////////////////////////////
// Enum class flag utilities.
template< typename _Enum >
inline bool BcContainsAllFlags( _Enum Value, _Enum Flags )
{
	static_assert( sizeof( _Enum ) <= sizeof( int ), "Enum size too large." );
	return ( (int)Value & (int)Flags ) == (int)Flags;
}

template< typename _Enum >
inline bool BcContainsAnyFlags( _Enum Value, _Enum Flags )
{
	static_assert( sizeof( _Enum ) <= sizeof( int ), "Enum size too large." );
	return ( (int)Value & (int)Flags ) != 0;
}

//////////////////////////////////////////////////////////////////////////
// TODO: Move this to a better place...
// Mega super awesome hack.
// Temporary until better demangling is setup.
#include <cstdlib>
#include <memory>
#include <string>

namespace CompilerUtility
{
	std::string Demangle( const char* Name );
}

//////////////////////////////////////////////////////////////////////////
// Symbol exporting.
#if PLATFORM_WINDOWS
#define PSY_EXPORT __declspec(dllexport)
#else
#define PSY_EXPORT
#endif


#endif // include_guard
