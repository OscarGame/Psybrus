/**************************************************************************
*
* File:		SysSystem.cpp
* Author: 	Neil Richardson 
* Ver/Date:	
* Description:
*		System implementation.
*		
*
*
* 
**************************************************************************/

#include "System/SysSystem.h"
#include "System/SysKernel.h"

#include "Base/BcMemory.h"
#include "Base/BcString.h"
#include "Base/BcProfiler.h"

//////////////////////////////////////////////////////////////////////////
// Reflection
REFLECTION_DEFINE_BASE( SysSystem );

void SysSystem::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Name_",				&SysSystem::Name_ ),
		new ReField( "pKernel_",			&SysSystem::pKernel_ ),
		new ReField( "ProcessState_",		&SysSystem::ProcessState_ ),
		new ReField( "StopTriggered_",		&SysSystem::StopTriggered_ ),
		new ReField( "PerfTimer_",			&SysSystem::PerfTimer_ ),
		new ReField( "LastTickTime_",		&SysSystem::LastTickTime_ ),
	};
		
	ReRegisterAbstractClass< SysSystem >( Fields );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
SysSystem::SysSystem():
	pKernel_( NULL ),
	ProcessState_( STATE_OPEN ),
	StopTriggered_( BcFalse )
{
	// Setup function pointers.
	ProcessFuncs_[ STATE_OPEN ] = 		&SysSystem::processOpen;
	ProcessFuncs_[ STATE_UPDATE ] = 	&SysSystem::processUpdate;
	ProcessFuncs_[ STATE_CLOSE ] =		&SysSystem::processClose;
	ProcessFuncs_[ STATE_FINISHED ] = 	&SysSystem::processFinished;
	
	// Perf stuff.
	LastTickTime_ = 0.0f;
}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
SysSystem::~SysSystem()
{
	BcAssert( ProcessState_ == STATE_FINISHED );
}

//////////////////////////////////////////////////////////////////////////
// pKernel
void SysSystem::pKernel( SysKernel* pKernel )
{
	pKernel_ = pKernel;
}

//////////////////////////////////////////////////////////////////////////
// pKernel
SysKernel* SysSystem::pKernel()
{
	return pKernel_;
}

//////////////////////////////////////////////////////////////////////////
// stop
void SysSystem::stop()
{
	StopTriggered_ = BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// isOpened
BcBool SysSystem::isOpened() const
{
	return ProcessState_ > STATE_OPEN;
}

//////////////////////////////////////////////////////////////////////////
// isFinished
BcBool SysSystem::isFinished() const
{
	return ProcessState_ == STATE_FINISHED;
}

//////////////////////////////////////////////////////////////////////////
// lastTickTime
BcF32 SysSystem::lastTickTime() const
{
	return LastTickTime_;
}

//////////////////////////////////////////////////////////////////////////
// process
BcBool SysSystem::process()
{
	PSY_PROFILER_SECTION( "%s::SysSystem::process", (*Name_).c_str() );

	// Mark perf timer.
	PerfTimer_.mark();
	
	// Cache process func.
	ProcessFunc processFunc = ProcessFuncs_[ ProcessState_ ];
	
	// Call the correct function for processing.
	BcBool RetVal = (this->*processFunc)();
	
	// Store last tick time.
	LastTickTime_ = (BcF32)PerfTimer_.time();

	BcAssert( LastTickTime_ >= 0.0f );
	
	//PSY_LOG( "System %p time: %f ms\n", this, LastTickTime_ * 1000.0f );
	
	return RetVal;
}

//////////////////////////////////////////////////////////////////////////
// processOpen
BcBool SysSystem::processOpen()
{
	PSY_LOG( "============================================================================\n" );
	PSY_LOG( "SysSystem (%s @ 0x%p) open:\n", (*Name_).c_str(), this );

	// Pre-open event.
	EvtPublisher::publish( sysEVT_SYSTEM_PRE_OPEN, SysSystemEvent( this ) );

	// Tick open.
	open();

	// Post-open event.
	EvtPublisher::publish( sysEVT_SYSTEM_POST_OPEN, SysSystemEvent( this ) );

	// Advance to update if a stop hasn't been triggered.
	if( StopTriggered_ == BcFalse )
	{	
		ProcessState_ = STATE_UPDATE;
	}
	else
	{
		ProcessState_ = STATE_CLOSE;
	}

	return BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// processUpdate
BcBool SysSystem::processUpdate()
{
	// Pre-update event.
	EvtPublisher::publish( sysEVT_SYSTEM_PRE_UPDATE, SysSystemEvent( this ) );

	// Tick update.
	update();

	// Post-update event.
	EvtPublisher::publish( sysEVT_SYSTEM_POST_UPDATE, SysSystemEvent( this ) );
	
	// Stop if need be.
	if( StopTriggered_ == BcTrue )
	{
		ProcessState_ = STATE_CLOSE;
	}
	
	return BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// processClose
BcBool SysSystem::processClose()
{
	PSY_LOG( "============================================================================\n" );
	PSY_LOG( "SysSystem (%s @ 0x%p) close:\n", (*Name_).c_str(), this );

	// Pre-close event.
	EvtPublisher::publish( sysEVT_SYSTEM_PRE_CLOSE, SysSystemEvent( this ) );

	// Tick close.
	close();
	
	// Post-close event.
	EvtPublisher::publish( sysEVT_SYSTEM_POST_CLOSE, SysSystemEvent( this ) );

	// Advance to finished.
	ProcessState_ = STATE_FINISHED;

	//
	return BcTrue;	
}

//////////////////////////////////////////////////////////////////////////
// processFinished
BcBool SysSystem::processFinished()
{
	// Do nothing.
	return BcFalse;
}


//////////////////////////////////////////////////////////////////////////
// setName
void SysSystem::setName( const BcName& Name )
{
	Name_ = Name;
}
