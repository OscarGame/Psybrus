/**************************************************************************
*
* File:		MainShared.cpp
* Author:	Neil Richardson 
* Ver/Date:	25/09/11
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "MainShared.h"

BcU32 GResolutionWidth = 1280;
BcU32 GResolutionHeight = 720;

#include "Events/EvtBinding.h"
#include "Events/EvtEvent.h"
#include "Events/EvtPublisher.h"
#include "Events/EvtProxyBuffered.h"

#include "Math/MaMat3d.h"
#include "Math/MaMat4d.h"

#include "System/SysKernel.h"

#include "System/Content/CsCore.h"
#include "System/Debug/DsCore.h"
#include "System/File/FsCore.h"
#include "System/Os/OsCore.h"
#include "System/Renderer/RsCore.h"
#include "System/Sound/SsCore.h"
#include "System/Scene/ScnCore.h"
#include "System/SysKernel.h"

#include "Import/Img/Img.h"
#include "Import/Img/gif.h"

#include "Base/BcBuildInfo.h"

#define SEARCH_FOR_CORRECT_PATH ( PLATFORM_WINDOWS | PLATFORM_LINUX | PLATFORM_OSX )

#if SEARCH_FOR_CORRECT_PATH
#include <boost/filesystem.hpp>
#endif

#include <chrono>

//////////////////////////////////////////////////////////////////////////
// Screenshot utility.
namespace ScreenshotUtil
{
	size_t ScreenshotJobQueue( 0 );
	BcBool ScreenCapturing( BcFalse );
	GifWriter Writer;
	size_t FrameCounter( 0 );
	std::atomic< size_t > TotalFramesRemaining( 0 );

	void Init()
	{
		// Create job queue for screenshot encoding/saving.
		ScreenshotJobQueue = SysKernel::pImpl()->createJobQueue( 1, 1 );
	}

	void TakeScreenshot()
	{
		if( ScreenCapturing )
		{
			return;
		}

		RsCore::pImpl()->getContext( nullptr )->takeScreenshot(
			[]( RsScreenshot Screenshot )->BcBool
			{
				// Convert to image.
				const BcU32 W = Screenshot.Width_;
				const BcU32 H = Screenshot.Height_;
				const BcU32* ImageData = reinterpret_cast< const BcU32* >( Screenshot.Data_ );
				ImgImage* Image( new ImgImage() );
				Image->create( W, H, nullptr );
				Image->setPixels( reinterpret_cast< const ImgColour* >( ImageData ) );

				SysKernel::pImpl()->pushFunctionJob( ScreenshotJobQueue, 
					[ Image ]()->void
					{
						PSY_LOG( "Processing screenshot..." );

						// Solid alpha.
						for( BcU32 Y = 0; Y < Image->height(); ++Y )
						{
							for( BcU32 X = 0; X < Image->width(); ++X )
							{
								ImgColour Pixel = Image->getPixel( X, Y );
								Pixel.A_ = 0xff;
								Image->setPixel( X, Y, Pixel );
							}
						}

						// Save out image.
						auto Time = std::time( nullptr );
						auto LocalTime = *std::localtime( &Time );
						BcChar FileName[ 1024 ] = { 0 };
#if PLATFORM_ANDROID
						strftime( FileName, sizeof( FileName ) - 1, "/sdcard/Pictures/screenshot_%Y-%m-%d-%H-%M-%S.png", &LocalTime );
#else
						strftime( FileName, sizeof( FileName ) - 1, "screenshot_%Y-%m-%d-%H-%M-%S.png", &LocalTime );
#endif
						Img::save( FileName, Image );
						PSY_LOG( "Saved screenshot to %s", FileName );
						delete Image;
					} );

				return BcFalse;
			} );
	}

	void BeginCapture()
	{
		// Early out.
		if( ScreenshotUtil::ScreenCapturing == BcTrue || ScreenshotUtil::TotalFramesRemaining > 0 )
		{
			return;
		}

		// Begin the gif capture.
		auto Time = std::time( nullptr );
		auto LocalTime = *std::localtime( &Time );
		BcChar FileName[ 1024 ] = { 0 };
#if PLATFORM_ANDROID
		strftime( FileName, sizeof( FileName ) - 1, "/sdcard/Pictures/screencapture_%Y-%m-%d-%H-%M-%S.gif", &LocalTime );
#else
		strftime( FileName, sizeof( FileName ) - 1, "screencapture_%Y-%m-%d-%H-%M-%S.gif", &LocalTime );
#endif
		const BcU32 W = OsCore::pImpl()->getClient( 0 )->getWidth();
		const BcU32 H = OsCore::pImpl()->getClient( 0 )->getHeight();
		GifBegin( &Writer, FileName, W / 2, H / 2, 1, 8, false );

		// Mark capturing.
		ScreenCapturing = BcTrue;
		FrameCounter = 0;

		// 
		RsCore::pImpl()->getContext( nullptr )->takeScreenshot( 
			[]( RsScreenshot Screenshot )->BcBool
			{
				const BcBool IsScreenCapturing = ScreenCapturing;
				if( ( FrameCounter % 6 ) == 0 || IsScreenCapturing == BcFalse )
				{
					const BcU32 W = Screenshot.Width_;
					const BcU32 H = Screenshot.Height_;
					ImgImage* Image = new ImgImage();
					Image->create( W, H, nullptr );
					Image->setPixels( reinterpret_cast< ImgColour* >( Screenshot.Data_ ) );
					++TotalFramesRemaining;
					SysKernel::pImpl()->pushFunctionJob( ScreenshotJobQueue, 
						[ W, H, IsScreenCapturing, Image ]()->void
						{
							auto ImageDiv2 = Image->resize( W / 2, H / 2 );
							GifWriteFrame(
								&Writer,
								reinterpret_cast< const uint8_t* >( ImageDiv2->getImageData() ), 
								W / 2, H / 2, 0, 8, false );

							if( IsScreenCapturing == BcFalse )
							{
								GifEnd( &Writer );
							}

							delete Image;
							TotalFramesRemaining--;
						} );
				}
				++FrameCounter;
				return IsScreenCapturing;
			} );
	}

	void EndCapture()
	{
		ScreenCapturing = BcFalse;
	}
}

//////////////////////////////////////////////////////////////////////////
// MainUnitTests
void MainUnitTests()
{
	PSY_LOG( "============================================================================\n" );
	PSY_LOG( "MainUnitTests:\n" );

	BcAssertScopedHandler AssertHandler(
		[]( const BcChar* Message, const BcChar* File, int Line )
		{
			BcPrintf( "Caught assertion: \"%s\" in %s on line %u.\n", Message, File, Line );
			return BcFalse;
		} );

	// Types unit test.
	extern void BcTypes_UnitTest();
	BcTypes_UnitTest();
	
#if !PLATFORM_ANDROID
	// Fixed unit test.
	extern void BcFixed_UnitTest();
	BcFixed_UnitTest();

	// Relative ptr unit test.
	extern void BcRelativePtr_UnitTest();
	BcRelativePtr_UnitTest();
#endif

	// SysKernel unit test.
	extern void SysKernel_UnitTest();
	SysKernel_UnitTest();
}

//////////////////////////////////////////////////////////////////////////
// onCsCoreOpened
#if !PLATFORM_ANDROID && !PLATFORM_HTML5
eEvtReturn onCsCoreOpened( EvtID ID, const EvtBaseEvent& Event )
{
	// Register reflection.
	extern void AutoGenRegisterReflection();
	AutoGenRegisterReflection();

	return evtRET_REMOVE;
}
#endif

//////////////////////////////////////////////////////////////////////////
// onDsCoreOpened
eEvtReturn onDsCoreOpened( EvtID ID, const EvtBaseEvent& Event )
{
	DsCore::pImpl()->registerPanel( 
		"Engine", []( BcU32 )->void
		{
			static BcF32 GameTimeTotal = 0.0f;
			static BcF32 RenderTimeTotal = 0.0f;
			static BcF32 FrameTimeTotal = 0.0f;
			static BcF32 GameTimeAccum = 0.0f;
			static BcF32 RenderTimeAccum = 0.0f;
			static BcF32 FrameTimeAccum = 0.0f;
			static int CaptureAmount = 60;
			static int CaptureAccum = 0;

			GameTimeAccum += SysKernel::pImpl()->getGameThreadTime();
			RenderTimeAccum += RsCore::pImpl()->getFrameTime();
			FrameTimeAccum += SysKernel::pImpl()->getFrameTime();
			++CaptureAccum;
			if( CaptureAccum >= CaptureAmount )
			{
				GameTimeTotal = GameTimeAccum / BcF32( CaptureAccum );
				RenderTimeTotal = RenderTimeAccum / BcF32( CaptureAccum );
				FrameTimeTotal = FrameTimeAccum / BcF32( CaptureAccum );
				GameTimeAccum = 0.0f;
				RenderTimeAccum = 0.0f;
				FrameTimeAccum = 0.0f;
				CaptureAccum = 0;
			}

			OsClient* Client = OsCore::pImpl()->getClient( 0 );
			MaVec2d WindowPos = MaVec2d( Client->getWidth() - 300.0f, 10.0f );
			static bool ShowOpened = true;
			ImGui::SetNextWindowPos( WindowPos );
			if ( ImGui::Begin( "Engine", &ShowOpened, ImVec2( 0.0f, 0.0f ), 0.3f, 
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize ) )
			{
				ImGui::Text( "Build: %s-%s-%s", 
					BUILD_ACTION,
					BUILD_TOOLCHAIN,
					BUILD_CONFIG );
				ImGui::Text( "Date/Time: %s %s", 
					BUILD_DATE,
					BUILD_TIME );
				ImGui::Text( "Worker count: %u", 
					SysKernel::pImpl()->workerCount() );
				ImGui::Text( "Game time: %.2fms (%.2fms avg.)", 
					SysKernel::pImpl()->getGameThreadTime() * 1000.0f, GameTimeTotal * 1000.0f );
				ImGui::Text( "Render time: %.2fms (%.2fms avg.)", 
					RsCore::pImpl()->getFrameTime() * 1000.0f, RenderTimeTotal * 1000.0f );
				ImGui::Text( "Frame time: %.2fms (%.2fms avg.)", 
					SysKernel::pImpl()->getFrameTime() * 1000.0f, FrameTimeTotal * 1000.0f );

				if( ScreenshotUtil::ScreenCapturing == BcFalse && ScreenshotUtil::TotalFramesRemaining == 0 )
				{
					if( ImGui::Button( "Begin Capture (F1)" ) )
					{
						ScreenshotUtil::BeginCapture();
					}
				}
				else
				{
					if( ImGui::Button( "End Capture (F1)" ) )
					{
						ScreenshotUtil::EndCapture();
					}
				}

				if( !ScreenshotUtil::ScreenCapturing )
				{
					ImGui::SameLine();
					if( ImGui::Button( "Screenshot (F2)" ) )
					{
						ScreenshotUtil::TakeScreenshot();
					}
				}

				ImGui::Text( "Capture frames processing: %u", ScreenshotUtil::TotalFramesRemaining.load() );
			}
			ImGui::End();
		} );

	return evtRET_REMOVE;
}

//////////////////////////////////////////////////////////////////////////
// OnQuit
eEvtReturn onQuit( EvtID ID, const EvtBaseEvent& Event )
{
	SysKernel::pImpl()->stop();

	return evtRET_REMOVE;
}


//////////////////////////////////////////////////////////////////////////
// onScreenshot
eEvtReturn onScreenshot( EvtID ID, const EvtBaseEvent& Event )
{
	const auto& KeyEvent = Event.get< OsEventInputKeyboard >();
	if( KeyEvent.KeyCode_ == OsEventInputKeyboard::KEYCODE_F1 )
	{
		if( ScreenshotUtil::ScreenCapturing == BcFalse )
		{
			ScreenshotUtil::BeginCapture();
		}
		else
		{
			ScreenshotUtil::EndCapture();
		}
	}
	else if( KeyEvent.KeyCode_ == OsEventInputKeyboard::KEYCODE_F2 )
	{
		ScreenshotUtil::TakeScreenshot();
	}
	return evtRET_PASS;
}

//////////////////////////////////////////////////////////////////////////
// MainShared
void MainShared()
{
#if SEARCH_FOR_CORRECT_PATH
	// Search each level up from the executable until we hit either a "PackedContent" folder,
	// or we hit a "Dist" folder in non-production builds. This will allow executables to be
	// subfoldered by platform or similar.
	using namespace boost::filesystem;
	path SearchPath( canonical( SysExePath_ ).parent_path() );
	path CurrentPath( canonical( current_path() ) );

	// Search up the path to find where we should be.
	bool FoundRoot = false;
	while( FoundRoot == false && SearchPath.has_leaf() )
	{
		auto It = directory_iterator( SearchPath );
		while( It != directory_iterator() )
		{
			directory_entry Entry = *It;
			if( Entry.path().filename() == "PackedContent" )
			{
				CurrentPath = SearchPath;
				FoundRoot = true;
				break;
			}
#if !PSY_PRODUCTION
			else if( Entry.path().filename() == "Dist" )
			{
				CurrentPath = Entry;
				FoundRoot = true;
				break;
			}
#endif
			++It;
		}
		SearchPath = SearchPath.remove_leaf();
	}

	// Change to found path.
	if( FoundRoot )
	{
		current_path( CurrentPath );
	}
#endif


	// Setup default system job queues.
	SysKernel::DEFAULT_JOB_QUEUE_ID = SysKernel::pImpl()->createJobQueue( std::thread::hardware_concurrency(), 0 );

	/*
	// Disable render thread for debugging.
	if( SysArgs_.find( "-norenderthread " ) != std::string::npos )
	{
		RsCore::JOB_QUEUE_ID = -1;
	}
	
	// Disable sound thread for debugging.
	if( SysArgs_.find( "-nosoundthread ") != std::string::npos )
	{
		SsCore::JOB_QUEUE_ID = -1;
	}
	
	// Disable file thread for debugging.
	if( SysArgs_.find( "-nofilethread " ) != std::string::npos )
	{
		FsCore::JOB_QUEUE_ID = -1;
	}
	*/

	// Parse command line params for disabling systems.
	if( SysArgs_.find( "-noremote" ) != std::string::npos )
	{
		GPsySetupParams.Flags_ &= ~psySF_REMOTE;
	}


	if( SysArgs_.find( "-norender" ) != std::string::npos )
	{
		GPsySetupParams.Flags_ &= ~psySF_RENDER;
	}

	if( SysArgs_.find( "-nosound" ) != std::string::npos )
	{
		GPsySetupParams.Flags_ &= ~psySF_SOUND;
	}
	
	// HACK: If we are importing packages, disable renderer and sound systems.
	if( SysArgs_.find( "ImportPackages" ) != std::string::npos )
	{
		GPsySetupParams.Flags_ &= ~( psySF_RENDER | psySF_SOUND );
	}


	// Start file system.
	SysKernel::pImpl()->startSystem( "FsCore" );

	// Start OS system.
	SysKernel::pImpl()->startSystem( "OsCore" );

	// Start render system.
	if( GPsySetupParams.Flags_ & psySF_RENDER )
	{
		SysKernel::pImpl()->startSystem( "RsCore" );
	}

	// Start sound system.
	if( GPsySetupParams.Flags_ & psySF_SOUND )
	{
		SysKernel::pImpl()->startSystem( "SsCore" );
	}

	// Start content system, depending on startup flags.
	SysKernel::pImpl()->startSystem( "CsCore" );

	// Start debug system if not a production build.
#if !defined( PSY_PRODUCTION )
	SysKernel::pImpl()->startSystem( "DsCoreLogging" );
#endif

//#if !defined( PSY_PRODUCTION )
	SysKernel::pImpl()->startSystem( "DsCore" );
//#endif

	// Init screenshot.
	ScreenshotUtil::Init();

	// Start scene system.
	SysKernel::pImpl()->startSystem( "ScnCore" );

	// Log kernel information.
	PSY_LOG( "============================================================================\n" );
	PSY_LOG( "MainShared:\n" );
	PSY_LOG( " - Command line: %s\n", SysArgs_.c_str() );
	PSY_LOG( " - Setup Flags: 0x%x\n", GPsySetupParams.Flags_ );
	PSY_LOG( " - Name: %s\n", GPsySetupParams.Name_.c_str() );
	PSY_LOG( " - Tick Rate: 1.0/%.1f\n", 1.0f / GPsySetupParams.TickRate_ );
	PSY_LOG( " - SysKernel::DEFAULT_JOB_QUEUE_ID: 0x%x\n", SysKernel::DEFAULT_JOB_QUEUE_ID );
	PSY_LOG( " - FsCore::JOB_QUEUE_ID: 0x%x\n", FsCore::JOB_QUEUE_ID );
	PSY_LOG( " - RsCore::JOB_QUEUE_ID: 0x%x\n", RsCore::JOB_QUEUE_ID );
	PSY_LOG( " - SsCore::JOB_QUEUE_ID: 0x%x\n", SsCore::JOB_QUEUE_ID );

#if !PLATFORM_ANDROID && !PLATFORM_HTML5
	// Setup callback for post CsCore open for resource registration.
	CsCore::pImpl()->subscribe( sysEVT_SYSTEM_POST_OPEN, onCsCoreOpened );
#endif

	// Setup callback for post DsCore open.
	if( DsCore::pImpl() )
	{
		DsCore::pImpl()->subscribe( sysEVT_SYSTEM_POST_OPEN, onDsCoreOpened );
	}
	
	// Subscribe to quit.
	OsCore::pImpl()->subscribe( osEVT_CORE_QUIT, onQuit );

	// Subscribe to F1 & F2 for screenshot
	OsCore::pImpl()->subscribe( osEVT_INPUT_KEYDOWN, onScreenshot );
}
