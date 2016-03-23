#ifndef __REFLECTION_FIELD_H__
#define __REFLECTION_FIELD_H__

#include "Reflection/ReAttributable.h"
#include "Reflection/ReContainerAccessor.h"
#include "Reflection/ReContainerAccessorArray.h"
#include "Reflection/ReContainerAccessorList.h"
#include "Reflection/ReContainerAccessorMap.h"
#include "Reflection/ReContainerAccessorSet.h"
#include "Reflection/ReContainerAccessorVector.h"

#include <cstddef>

//////////////////////////////////////////////////////////////////////////
// Typedef
typedef std::vector< class ReField* > ReFieldVector;

//////////////////////////////////////////////////////////////////////////
// Field
class ReField:
    public ReAttributable
{
public:
    REFLECTION_DECLARE_DERIVED( ReField, ReAttributable );

public:
    ReField();
	ReField( const ReField& ) = delete;
    virtual ~ReField()
	{
		delete ContainerAccessor_;
		ContainerAccessor_ = nullptr;
	}

	/**
	 * Initialisation.
	 */
	template< typename _Class, typename _Ty >
	ReField(
			const std::string& Name,
			_Ty _Class::*field,
			BcU32 Flags = 0 ):
		Type_( nullptr ),
		Size_( sizeof( _Ty ) ),
		Offset_( 0 ),
		FieldFlags_( 0 ),
		ContainerAccessor_( nullptr ),
		KeyType_( nullptr ),
		ValueType_( nullptr )
	{
        typedef ReTypeTraits< _Ty > LocalTypeTraits;
		setName( Name );
		setFlags( Flags | LocalTypeTraits::Flags | bcRFF_FIELD );

#if COMPILER_MSVC
		setOffset( offsetof( _Class, *field ) );
#else
		setOffset( (size_t)( &( ( (_Class*)( 0 ) )->*field ) ) );
#endif // COMPILER_MSVC
 
		ContainerAccessor_ = CreateContainerAccessor( ( ( _Class* ) 0 )->*field, KeyType_, ValueType_, KeyFlags_, ValueFlags_ );

		// If we get a container accessor, use the value type.
		if( ContainerAccessor_ == nullptr )
		{
			if( std::is_enum< _Ty >::value )
			{
				setType( ReManager::GetEnum( LocalTypeTraits::Name() ) );
			}
			else
			{
				setType( ReManager::GetClass( LocalTypeTraits::Name() ) );
			}
		}
		else
		{
			setType( ValueType_ );
		}
	}

	/**
	 * Set type.
	 */
    void							setType( const ReClass* pType );

	/**
	 * Get type.
	 */
    const ReClass*					getType() const;

	/**
	 * Set size.
	 */
    void							setSize( BcSize Size );

	/**
	 * Get size.
	 */
    BcSize							getSize() const;

	/**
	 * Set offset.
	 */
	void							setOffset( BcSize Offset );

	/**
	 * Get offset.
	 */
	BcSize							getOffset() const;

	/**
	 * Set flags.
	 */
	void							setFlags( BcU32 Flags );

	/**
	 * Get flags.
	 */
	BcU32							getFlags() const;

	/**
	 * Get data from source pointer.
	 */
	template< typename _Ty >
	_Ty*							getData( void* pObjectData ) const
	{
		// Handle if type is a simple deref pointer (* and &)
		if( ( FieldFlags_ & bcRFF_SIMPLE_DEREF ) != 0 )
		{
			return *reinterpret_cast< _Ty** >( reinterpret_cast< BcU8* >( pObjectData ) + getOffset() );
		}
		else
		{
			return reinterpret_cast< _Ty* >( reinterpret_cast< BcU8* >( pObjectData ) + getOffset() );
		}
	}

	/**
	 * @brief Is this field a container?
	 */
	bool							isContainer() const;

	/**
	 * @brief Is this field a pointer type?
	 */
	bool							isPointerType() const;

	/**
	 * Get type.
	 */
    const ReClass*						getKeyType() const;

	/**
	 * Get type.
	 */
    const ReClass*						getValueType() const;

	/**
	 * Get flags.
	 */
	BcU32								getKeyFlags() const;

	/**
	 * Get flags.
	 */
	BcU32								getValueFlags() const;

	/**
	 * @brief New write iterator. Returns nullptr if it's not a container. Owner must delete.
	 */
	ReContainerAccessor::WriteIterator* newWriteIterator( void* pContainerData ) const;

	/**
	 * @brief New read iterator. Returns nullptr if it's not a container. Owner must delete.
	 */
	ReContainerAccessor::ReadIterator* newReadIterator( void* pContainerData ) const;

protected:
	BcSize								Offset_;
    const ReClass*						Type_;
	BcSize								Size_;
	BcU32								FieldFlags_;

	ReContainerAccessor*				ContainerAccessor_;
    const ReClass*						KeyType_;
    const ReClass*						ValueType_;
	BcU32								KeyFlags_;
	BcU32								ValueFlags_;		
};

#endif
