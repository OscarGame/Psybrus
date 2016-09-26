#include "Serialisation/SeSerialisation.h"

//////////////////////////////////////////////////////////////////////////
// Unit tests.
#if !PSY_PRODUCTION
#include <catch.hpp>

#include "Reflection/ReReflection.h"

#include "Serialisation/SeSerialisation.h"
#include "Serialisation/SeJsonReader.h"
#include "Serialisation/SeJsonWriter.h"

#include "Base/BcString.h"

extern int SeUnitTest_init = 0;

namespace TestData
{
	class ObjectCodecBasic:
		public SeISerialiserObjectCodec
	{
	public:
		ObjectCodecBasic()
		{
		}

		BcBool shouldSerialiseContents( void* InData, const ReClass* InType ) override
		{
			return BcTrue;
		}

		std::string serialiseAsStringRef( void* InData, const ReClass* InType )
		{
			std::array< char, 128 > Buffer;
			BcSPrintf( Buffer.data(), Buffer.size(), "%llu", (unsigned long long)InData );
			return Buffer.data();
		}
	
		BcBool isMatchingField( const class ReField* Field, const std::string& Name )
		{
			std::string FieldName = *Field->getName();
			if( *Field->getName() == Name )
			{
				return BcTrue;
			}
			return BcFalse;
		}

		BcBool shouldSerialiseField( void* InData, BcU32 ParentFlags, const class ReField* Field ) override
		{
			return BcTrue;
		}

		BcBool findObject( void*& OutObject, const ReClass* Type, BcU32 Key ) override
		{
			OutObject = nullptr;
			return BcFalse;
		}

		BcBool findObject( void*& OutObject, const ReClass* Type, const std::string& Key ) override
		{
			OutObject = nullptr;
			return BcFalse;
		}
	};


	class TestBase
	{
	public:
		REFLECTION_DECLARE_BASE_NOAUTOREG( TestBase );

		TestBase()
		{
		}

		TestBase( bool Alternative )
		{
			if( Alternative )
			{
				A_ = 3;
				B_ = 2;
				C_ = 1;
				D_ = 0;
				E_ = 2.0f; 
				F_ = 0.0f; 
				G_ = "AltTestG"; 
				H_ = "AltTestH"; 
			}
		}

		bool operator == ( const TestBase& Other ) const
		{
			return A_ == Other.A_ && B_ == Other.B_ &&
				C_ == Other.C_ && D_ == Other.D_ &&
				E_ == Other.E_ && F_ == Other.F_ &&
				G_ == Other.G_ && H_ == Other.H_;
		}

		int A_ = 0;
		int B_ = 1;
		unsigned int C_ = 2;
		unsigned int D_ = 3;
		float E_ = 0.0f; 
		float F_ = 2.0f; 
		std::string G_ = "TestG"; 
		std::string H_ = "TestH"; 
	};

	REFLECTION_DEFINE_BASE( TestBase );

	void TestBase::StaticRegisterClass()
	{
		static bool IsRegistered = false;
		if( IsRegistered == false )
		{
			ReField* Fields[] = 
			{
				new ReField( "A_", &TestBase::A_, bcRFF_IMPORTER ),
				new ReField( "B_", &TestBase::B_, bcRFF_IMPORTER ),
				new ReField( "C_", &TestBase::C_, bcRFF_IMPORTER ),
				new ReField( "D_", &TestBase::D_, bcRFF_IMPORTER ),
				new ReField( "E_", &TestBase::E_, bcRFF_IMPORTER ),
				new ReField( "F_", &TestBase::F_, bcRFF_IMPORTER ),
				new ReField( "G_", &TestBase::G_, bcRFF_IMPORTER ),
				new ReField( "H_", &TestBase::H_, bcRFF_IMPORTER ),
			};
	
			ReRegisterClass< TestBase >( Fields );

			IsRegistered = true;
		}
	}

	class TestBasePointers
	{
	public:
		REFLECTION_DECLARE_BASE_NOAUTOREG( TestBasePointers );

		TestBasePointers()
		{
		}

		~TestBasePointers()
		{
			delete A_;
			delete B_;
		}

		bool operator == ( const TestBasePointers& Other ) const
		{
			bool RetVal = true;
			if( A_ && Other.A_ )
			{
				RetVal &= *A_ == *Other.A_;
			}
			else
			{
				RetVal &= A_ == Other.A_;
			}
			if( B_ && Other.B_ )
			{
				RetVal &= *B_ == *Other.B_;
			}
			else
			{
				RetVal &= B_ == Other.B_;
			}
			return RetVal;
		}

		TestBase* A_ = nullptr;
		TestBase* B_ = nullptr;
	};

	REFLECTION_DEFINE_BASE( TestBasePointers );

	void TestBasePointers::StaticRegisterClass()
	{
		static bool IsRegistered = false;
		if( IsRegistered == false )
		{
			ReField* Fields[] = 
			{
				new ReField( "A_", &TestBasePointers::A_, bcRFF_IMPORTER | bcRFF_OWNER ),
				new ReField( "B_", &TestBasePointers::B_, bcRFF_IMPORTER | bcRFF_OWNER ),
			};
	
			ReRegisterClass< TestBasePointers >( Fields );

			IsRegistered = true;
		}
	}
}


TEST_CASE( "Serialisation-JsonWriter-Base" )
{
	using namespace TestData;

	// Register.
	TestBase::StaticRegisterClass();

	// Create object to save.
	TestBase JsonWriterTestBase_;
	
	// Write Json out.
	ObjectCodecBasic ObjectCodec;
	SeJsonWriter Writer( &ObjectCodec );
	Writer.serialise( &JsonWriterTestBase_, JsonWriterTestBase_.getClass() );

	INFO( Writer.getOutput() );

	const auto& Value = Writer.getValue();

	// Basic checks.
	REQUIRE( Value.type() == Json::objectValue );
	REQUIRE( Value[ SeISerialiser::SerialiserVersionString ].type() == Json::intValue );
	REQUIRE( Value[ SeISerialiser::SerialiserVersionString ].asUInt() == SERIALISER_VERSION );
	REQUIRE( Value[ SeISerialiser::ObjectsString ].type() == Json::arrayValue );
	REQUIRE( Value[ SeISerialiser::ObjectsString ].size() == 1 );
	REQUIRE( Value[ SeISerialiser::RootIDString ].type() == Json::stringValue );

	// Object check.
	const auto& Object = Value[ SeISerialiser::ObjectsString ][ 0 ];
	REQUIRE( Object.type() == Json::objectValue );

	// Check members.
	REQUIRE( (int)BcStrAtoi( Object[ "A_" ].asCString() ) == JsonWriterTestBase_.A_ );
	REQUIRE( (int)BcStrAtoi( Object[ "B_" ].asCString() ) == JsonWriterTestBase_.B_ );
	REQUIRE( (unsigned int)BcStrAtoi( Object[ "C_" ].asCString() ) == JsonWriterTestBase_.C_ );
	REQUIRE( (unsigned int)BcStrAtoi( Object[ "D_" ].asCString() ) == JsonWriterTestBase_.D_ );
	REQUIRE( BcStrAtof( Object[ "E_" ].asCString() ) == JsonWriterTestBase_.E_ );
	REQUIRE( BcStrAtof( Object[ "F_" ].asCString() ) == JsonWriterTestBase_.F_ );
	REQUIRE( JsonWriterTestBase_.G_ == Object[ "G_" ].asCString() );
	REQUIRE( JsonWriterTestBase_.H_ == Object[ "H_" ].asCString() );
}


TEST_CASE( "Serialisation-JsonWriter-Base-Pointers" )
{
	using namespace TestData;

	// Register.
	TestBase::StaticRegisterClass();
	TestBasePointers::StaticRegisterClass();

	// Create object to save.
	TestBasePointers JsonWriterTestBasePointers_;
	JsonWriterTestBasePointers_.A_ = new TestBase( false );
	JsonWriterTestBasePointers_.B_ = new TestBase( true );
	
	// Write Json out.
	ObjectCodecBasic ObjectCodec;
	SeJsonWriter Writer( &ObjectCodec );
	Writer.serialise( &JsonWriterTestBasePointers_, JsonWriterTestBasePointers_.getClass() );

	INFO( Writer.getOutput() );

	const auto& Value = Writer.getValue();

	// Basic checks.
	REQUIRE( Value.type() == Json::objectValue );
	REQUIRE( Value[ SeISerialiser::SerialiserVersionString ].type() == Json::intValue );
	REQUIRE( Value[ SeISerialiser::SerialiserVersionString ].asUInt() == SERIALISER_VERSION );
	REQUIRE( Value[ SeISerialiser::ObjectsString ].type() == Json::arrayValue );
	REQUIRE( Value[ SeISerialiser::ObjectsString ].size() == 3 );
	REQUIRE( Value[ SeISerialiser::RootIDString ].type() == Json::stringValue );

	// Object check.
	const auto& Object = Value[ SeISerialiser::ObjectsString ][ 0 ];
	REQUIRE( Object.type() == Json::objectValue );

	// Check of the contained objects.
	{
		const auto& Object = Value[ SeISerialiser::ObjectsString ][ 1 ];
		REQUIRE( Object.type() == Json::objectValue );

		TestBase TestBaseObject( false );
		REQUIRE( (int)BcStrAtoi( Object[ "A_" ].asCString() ) == TestBaseObject.A_ );
		REQUIRE( (int)BcStrAtoi( Object[ "B_" ].asCString() ) == TestBaseObject.B_ );
		REQUIRE( (unsigned int)BcStrAtoi( Object[ "C_" ].asCString() ) == TestBaseObject.C_ );
		REQUIRE( (unsigned int)BcStrAtoi( Object[ "D_" ].asCString() ) == TestBaseObject.D_ );
		REQUIRE( BcStrAtof( Object[ "E_" ].asCString() ) == TestBaseObject.E_ );
		REQUIRE( BcStrAtof( Object[ "F_" ].asCString() ) == TestBaseObject.F_ );
		REQUIRE( TestBaseObject.G_ == Object[ "G_" ].asCString() );
		REQUIRE( TestBaseObject.H_ == Object[ "H_" ].asCString() );
	}

	{
		const auto& Object = Value[ SeISerialiser::ObjectsString ][ 2 ];
		REQUIRE( Object.type() == Json::objectValue );

		TestBase TestBaseObject( true );
		REQUIRE( (int)BcStrAtoi( Object[ "A_" ].asCString() ) == TestBaseObject.A_ );
		REQUIRE( (int)BcStrAtoi( Object[ "B_" ].asCString() ) == TestBaseObject.B_ );
		REQUIRE( (unsigned int)BcStrAtoi( Object[ "C_" ].asCString() ) == TestBaseObject.C_ );
		REQUIRE( (unsigned int)BcStrAtoi( Object[ "D_" ].asCString() ) == TestBaseObject.D_ );
		REQUIRE( BcStrAtof( Object[ "E_" ].asCString() ) == TestBaseObject.E_ );
		REQUIRE( BcStrAtof( Object[ "F_" ].asCString() ) == TestBaseObject.F_ );
		REQUIRE( TestBaseObject.G_ == Object[ "G_" ].asCString() );
		REQUIRE( TestBaseObject.H_ == Object[ "H_" ].asCString() );
	}
}

TEST_CASE( "Serialisation-JsonReader-Base" )
{
	using namespace TestData;

	// Register.
	TestBase::StaticRegisterClass();

	// Create object to save.
	TestBase JsonWriterTestBase_;
	
	// Write Json out.
	ObjectCodecBasic ObjectCodec;
	SeJsonWriter Writer( &ObjectCodec );
	Writer.serialise( &JsonWriterTestBase_, JsonWriterTestBase_.getClass() );

	INFO( Writer.getOutput() );

	// Read Json in.
	SeJsonReader Reader( &ObjectCodec );
	TestBase Object( true );
	Reader.setRootValue( Writer.getValue() );
	Reader.serialise( &Object, Object.getClass() );

	REQUIRE( Object == JsonWriterTestBase_ );

}

TEST_CASE( "Serialisation-JsonReader-Base-Pointers" )
{
	using namespace TestData;

	// Register.
	TestBase::StaticRegisterClass();
	TestBasePointers::StaticRegisterClass();

	// Create object to save.
	TestBasePointers JsonWriterTestBasePointers_;
	JsonWriterTestBasePointers_.A_ = new TestBase( false );
	JsonWriterTestBasePointers_.B_ = new TestBase( true );
	
	// Write Json out.
	ObjectCodecBasic ObjectCodec;
	SeJsonWriter Writer( &ObjectCodec );
	Writer.serialise( &JsonWriterTestBasePointers_, JsonWriterTestBasePointers_.getClass() );

	INFO( Writer.getOutput() );

	// Read Json in.
	SeJsonReader Reader( &ObjectCodec );
	TestBasePointers Object;
	Reader.setRootValue( Writer.getValue() );
	Reader.serialise( &Object, Object.getClass() );

	REQUIRE( Object == JsonWriterTestBasePointers_ );

}

#endif // !PSY_PRODUCTION

//////////////////////////////////////////////////////////////////////////
// Statics
const char* SeISerialiser::SerialiserVersionString = "$SerialiserVersion";
const char* SeISerialiser::RootIDString = "$RootID";
const char* SeISerialiser::ObjectsString = "$Objects";
const char* SeISerialiser::ClassString = "$Class";
const char* SeISerialiser::IDString = "$ID";
const char* SeISerialiser::FieldString = "$Field";
const char* SeISerialiser::ValueString = "$Value";
