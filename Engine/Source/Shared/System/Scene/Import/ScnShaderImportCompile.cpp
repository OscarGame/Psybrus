/**************************************************************************
*
* File:		ScnShaderImportCompile.cpp
* Author:	Neil Richardson 
* Ver/Date: 
* Description:
*		
*		
*
*
* 
**************************************************************************/

#include "ScnShaderImport.h"

#if PSY_IMPORT_PIPELINE

#include "Base/BcFile.h"

#if PLATFORM_WINDOWS
#include "Base/BcComRef.h"

#include <D3DCompiler.h>
#include <D3DCompiler.inl>

#pragma comment( lib, "D3DCompiler.lib" )
#pragma comment (lib, "dxguid.lib")

namespace
{
	class ScnShaderIncludeHandler : public ID3DInclude 
	{
	public:
		ScnShaderIncludeHandler( 
			class ScnShaderImport& Importer,
			const std::vector< std::string >& IncludePaths ):
			Importer_( Importer ),
			IncludePaths_( IncludePaths )
		{

		}

	private:
		ScnShaderIncludeHandler& operator = ( const ScnShaderIncludeHandler& Other )
		{
			return *this;
		}

	public:
		HRESULT __stdcall Open( D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
		{
			BcFile IncludeFile;

			for( auto& IncludePath : IncludePaths_ )
			{
				std::string IncludeFileName = IncludePath + pFileName;

				if( IncludeFile.open( IncludeFileName.c_str(), bcFM_READ ) )
				{
					Importer_.addDependency( IncludeFileName.c_str() );
					*ppData = IncludeFile.readAllBytes().release();
					*pBytes = static_cast< UINT >( IncludeFile.size() );
					return S_OK;
				}
			}

			return E_FAIL;
		}

		HRESULT __stdcall Close(LPCVOID pData)
		{
			delete [] (BcU8*)pData;
			return S_OK;
		}

	private:
		class ScnShaderImport& Importer_;
		const std::vector< std::string >& IncludePaths_;
	};
}
#endif // PLATFORM_WINDOWS

#if PLATFORM_LINUX
#include <cstdlib>
#endif // PLATFORM_LINUX

BcBool ScnShaderImport::compileShader( 
	const std::string& FileName,
	const std::string& EntryPoint,
	const std::map< std::string, std::string >& Defines, 
	const std::vector< std::string >& IncludePaths,
	const std::string& Target,
	BcBinaryData& ShaderByteCode,
	std::vector< std::string >& ErrorMessages )
{
	BcBool RetVal = BcFalse;
#if PLATFORM_WINDOWS
	CsResourceImporter::addDependency( FileName.c_str() );
	std::wstring WFileName( FileName.begin(), FileName.end() );
	// Create macros.
	std::vector< D3D_SHADER_MACRO > Macros;
	Macros.reserve( Defines.size() + 1 );
	for( auto& DefineEntry : Defines )
	{
		D3D_SHADER_MACRO Macro = { DefineEntry.first.c_str(), DefineEntry.second.c_str() };
		Macros.push_back( Macro );
	}
	D3D_SHADER_MACRO EmptyMacro = { nullptr, nullptr };
	Macros.push_back( EmptyMacro );

	ID3D10Blob* OutByteCode;
	ID3D10Blob* OutErrorMessages;
	ScnShaderIncludeHandler IncludeHandler( *this, IncludePaths );
	UINT Flags = 0;
	//Flags |= D3DCOMPILE_DEBUG;
	D3DCompileFromFile( WFileName.c_str(), &Macros[ 0 ], &IncludeHandler, EntryPoint.c_str(), Target.c_str(), Flags, 0, &OutByteCode, &OutErrorMessages );

	// Extract byte code if we have it.
	if( OutByteCode != nullptr )
	{
		ShaderByteCode = std::move( BcBinaryData( OutByteCode->GetBufferPointer(), OutByteCode->GetBufferSize(), BcTrue ) );
		OutByteCode->Release();

		RetVal = BcTrue;
	}

	// Extract error messages if we have any.
	if( OutErrorMessages != nullptr )
	{
		LPVOID BufferData = OutErrorMessages->GetBufferPointer();

		// TODO: Split up into lines.
		std::string Error = (const char*)BufferData;
		ErrorMessages.push_back( Error );
		OutErrorMessages->Release();
	}
#endif // PLATFORM_WINDOWS

#if PLATFORM_LINUX || PLATFORM_OSX
	// LINUX TODO: Use env path or config file.
	auto PsybrusSDKRoot = "../Psybrus";

	// Generate some unique ids.
	BcU32 ShaderCompileId = ++ShaderCompileId_;
	std::array< char, 64 > BytecodeFilename;
	std::array< char, 64 > LogFilename;

	BcSPrintf( BytecodeFilename.data(), BytecodeFilename.size() - 1, "%s/built_shader_%u.bytecode", IntermediatePath_.c_str(), ShaderCompileId);
	BcSPrintf( LogFilename.data(), LogFilename.size() - 1, "%s/built_shader_%u.log", IntermediatePath_ .c_str(), ShaderCompileId );

	std::string CommandLine = std::string( "wine " ) + PsybrusSDKRoot + "/Tools/ShaderCompiler/ShaderCompiler.exe";
	CommandLine += std::string( " -i" ) + FileName;
	CommandLine += std::string( " -e" ) + LogFilename.data();
	CommandLine += std::string( " -o" ) + BytecodeFilename.data();
	CommandLine += std::string( " -T" ) + Target;
	CommandLine += std::string( " -E" ) + EntryPoint;
	for( auto Define : Defines )
	{
		CommandLine += " -D" + Define.first + "=" + Define.second;
	}
	for( auto IncludePath : IncludePaths )
	{
		CommandLine += " -I" + IncludePath;
	}

	int RetCode = std::system( CommandLine.c_str() );

	// If successful, load in output file.
	if( RetCode == 0 )
	{
		BcFile ByteCodeFile;
		if( ByteCodeFile.open( BytecodeFilename.data(), bcFM_READ ) )
		{
			auto ByteCode = ByteCodeFile.readAllBytes();
			ShaderByteCode = std::move( BcBinaryData( ByteCode.get(), ByteCodeFile.size(), BcTrue ) );
			RetVal = BcTrue;
		}
	}
	else
	{
		PSY_LOG( "Error: %u", RetCode );
	}

#endif // PLATFORM_LINUX || PLATFORM_OSX

	return RetVal;
}

RsProgramVertexAttributeList ScnShaderImport::extractShaderVertexAttributes(
	BcBinaryData& ShaderByteCode )
{
	RsProgramVertexAttributeList VertexAttributeList;
#if PLATFORM_WINDOWS
	BcComRef< ID3D11ShaderReflection > ShaderReflection;
	D3DReflect( ShaderByteCode.getData< const BcU8 >( 0 ), ShaderByteCode.getDataSize(),
		IID_ID3D11ShaderReflection, (void**)&ShaderReflection );

	BcU32 ChannelIdx = 0;
	for( BcU32 Idx = 0; Idx < 16; ++Idx )
	{
		D3D11_SIGNATURE_PARAMETER_DESC Desc;
		if( SUCCEEDED( ShaderReflection->GetInputParameterDesc( Idx, &Desc ) ) )
		{
			auto VertexAttribute = semanticToVertexAttribute( ChannelIdx++, Desc.SemanticName, Desc.SemanticIndex ); 
			if( VertexAttribute.Usage_ != RsVertexUsage::INVALID )
			{
				VertexAttributeList.push_back( VertexAttribute );
			}
		}
	}
#endif // PLATFORM_WINDOWS


#if PLATFORM_LINUX || PLATFORM_OSX
	struct VertexAttribute
	{
		char SemanticName_[ 32 ];
		unsigned int SemanticIndex_;
		unsigned int ChannelIdx_;
	};

	BcU32 SizeOfVertexAttrs = *ShaderByteCode.getData< BcU32 >( ShaderByteCode.getDataSize() - sizeof( BcU32 ) );
	BcU32 NoofVertexAttrs = SizeOfVertexAttrs / sizeof( VertexAttribute );
	VertexAttribute* VertexAttrs = ShaderByteCode.getData< VertexAttribute >( ShaderByteCode.getDataSize() - ( SizeOfVertexAttrs + sizeof( BcU32 ) ) );
	for( BcU32 Idx = 0; Idx < NoofVertexAttrs; ++Idx )
	{
		auto VertexAttribute = semanticToVertexAttribute( VertexAttrs[ Idx ].ChannelIdx_, VertexAttrs[ Idx ].SemanticName_, VertexAttrs[ Idx ].SemanticIndex_ ); 
		VertexAttributeList.push_back( VertexAttribute );
	}

#endif // PLATFORM_LINUX || PLATFORM_OSX

	return VertexAttributeList;
}

#endif // PSY_IMPORT_PIPELINE
