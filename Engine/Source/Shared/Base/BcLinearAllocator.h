#pragma once

#include "Base/BcTypes.h"

#include <atomic>

//////////////////////////////////////////////////////////////////////////
// BcLinearAllocator
class BcLinearAllocator
{
public:
	BcLinearAllocator( size_t Size, size_t Alignment = sizeof( size_t ) );
	~BcLinearAllocator();

	BcLinearAllocator( const BcLinearAllocator& ) = delete;

	/**
	 * Reset allocator back to 0.
	 */
	void reset();

	/**
	 * Allocate.
	 * @return Valid pointer if allocation was successful, nullptr if it wasn't.
	 */
	void* allocate( size_t Bytes );

private:
	std::unique_ptr< BcU8 > Base_;
	size_t Size_;
	size_t Alignment_;
	std::atomic< size_t > Offset_;

};