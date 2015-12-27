#ifndef __REFLECTION_CONTAINERACCESSORARRAY_H__
#define __REFLECTION_CONTAINERACCESSORARRAY_H__

#include "Reflection/ReContainerAccessor.h"

//////////////////////////////////////////////////////////////////////////
// ArrayContainerAccessor
template< typename _Ty, size_t _Size >
class ReArrayContainerAccessor:
	public ReContainerAccessor
{
public:
    typedef ReTypeTraits< _Ty > ValueTraits;

public:
	class ArrayWriteIterator:
		public WriteIterator
	{
	public:
		ArrayWriteIterator( _Ty* pArrayData ):
			pArrayData_( pArrayData ),
			Index_( 0 )
		{

		}

		virtual ~ArrayWriteIterator()
		{

		}
		
		ArrayWriteIterator& operator = ( ArrayWriteIterator& Other )
		{
			pArrayData_ = Other.pArrayData_;
			Index_ = Other.Index_;
			return *this;
		}
		
		virtual void clear()
		{

		}

		template< typename _Ty >
		typename std::enable_if< std::is_copy_constructible< _Ty >::value >::type
		internalAdd( void* pValue )
		{
			pArrayData_[ Index_++ ] = *reinterpret_cast< _Ty* >( pValue );
		}

		template< typename _Ty >
		typename std::enable_if< !std::is_copy_constructible< _Ty >::value >::type
		internalAdd( void* pValue )
		{
		}

		virtual void add( void* pValue )
		{
			BcAssertMsg( std::is_copy_constructible< _Ty >::value, "_Ty is not trivially copyable." );
			internalAdd< _Ty >( pValue );
		}

		virtual void addMove( void* pValue )
		{
			BcAssertMsg( std::is_move_constructible< _Ty >::value, "_Ty is not move constructible." );
			pArrayData_[ Index_++ ] = std::move( *reinterpret_cast< _Ty* >( pValue ) );
		}

		virtual void add( void* pKey, void* pValue )
		{
			BcAssertMsg( false, "ArrayContainerAccessor does not expose key." );
		}

		virtual void addMove( void* pKey, void* pValue )
		{
			BcAssertMsg( false, "ArrayContainerAccessor does not expose key." );
		}

		virtual bool isValid() const
		{
			return Index_ < _Size;
		}

	private:
		_Ty* pArrayData_;
		size_t Index_;
	};

	class ArrayReadIterator:
		public ReadIterator
	{
	public:
		ArrayReadIterator( _Ty* pArrayData ):
			pArrayData_( pArrayData ),
			Index_( 0 )
		{

		}

		virtual ~ArrayReadIterator()
		{

		}

		ArrayReadIterator& operator = ( ArrayReadIterator& Other )
		{
			pArrayData_ = Other.pArrayData_;
			Index_ = Other.Index_;
			return *this;
		}

		virtual void* getKey() const
		{
			return nullptr;
		}

		virtual void* getValue() const
		{
			return &pArrayData_[ Index_ ];
		}

		virtual void next()
		{
			++Index_;
		}

		virtual bool isValid() const
		{
			return Index_ < _Size;
		}

	private:
		_Ty* pArrayData_;
		size_t Index_;
	};

public:
    ReArrayContainerAccessor()
	{
		pKeyType_ = nullptr;
		pValueType_ = ReManager::GetClass< typename ValueTraits::Type >();
		KeyFlags_ = 0;
		ValueFlags_ = ValueTraits::Flags;
	}

    virtual ~ReArrayContainerAccessor()
	{

	}

	virtual WriteIterator* newWriteIterator( void* pContainerData ) const
	{
		return new ArrayWriteIterator( reinterpret_cast< _Ty* >( pContainerData ) );
	}

	virtual ReadIterator* newReadIterator( void* pContainerData ) const
	{
		return new ArrayReadIterator( reinterpret_cast< _Ty* >( pContainerData ) );
	}
};

template < typename _Ty, size_t _Size >
ReContainerAccessor* CreateContainerAccessor( _Ty ( & )[ _Size ], const ReClass*& pKeyType, const ReClass*& pValueType, BcU32& KeyFlags, BcU32& ValueFlags )
{
	pKeyType = nullptr;
	pValueType = ReManager::GetClass<
		typename ReTypeTraits< _Ty >::Type >();
	KeyFlags = 0;
	ValueFlags = ReTypeTraits< _Ty >::Flags;
    return new ReArrayContainerAccessor< _Ty, _Size >();
}
		
template < typename _Ty, size_t _Size >
ReContainerAccessor* CreateContainerAccessor( std::array< _Ty, _Size >&, const ReClass*& pKeyType, const ReClass*& pValueType, BcU32& KeyFlags, BcU32& ValueFlags )
{
	pKeyType = nullptr;
	pValueType = ReManager::GetClass< 
		typename ReTypeTraits< _Ty >::Type >();
	KeyFlags = 0;
	ValueFlags = ReTypeTraits< _Ty >::Flags;
    return new ReArrayContainerAccessor< _Ty, _Size >();
}

#endif
