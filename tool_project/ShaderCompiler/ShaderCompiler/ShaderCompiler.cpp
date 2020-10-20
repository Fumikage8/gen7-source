#include "ShaderCompiler.h"
#include<RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <util/include/gfl2_FnvHash.h>
#include "gr_ShaderLite.h"

#include <windows.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

using namespace gfl2;
using namespace gfl2::renderingengine;
using namespace gfl2::renderingengine::scenegraph::resource;

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderCompiler::ShaderCompiler()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
ShaderCompiler::~ShaderCompiler()
{

}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::ReadFile( Type type, std::string filePath )
{
	int			size;
	char*		pSrc = ShaderCompiler::ReadFile( filePath.c_str(), &size, 1 );
	m_FileName = filePath;
	m_ShaderName = ParserUtil::GetFileNameStr( filePath.c_str() );

	m_ShaderName = ParserUtil::SplitStr( m_ShaderName, "." )[0];

	if ( pSrc )
	{
		SetSource( type, pSrc );
		delete[] pSrc;
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::SetSource( Type type, const char* pSrc )
{
	m_Source[type] = pSrc;
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::CompileHLSL( std::string shaderTypeName, std::vector<std::string> argList )
{
	/*HRESULT					hr;
	DWORD						dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;
	ID3DBlob				*pBlob = NULL;
	ID3DBlob				*pErrorBlob = NULL;
	Type						type;
	std::string			shaderFuncName = "main";

	if ( shaderTypeName == "vs" )
	{
		type = Type::Vertex;
		shaderTypeName = "vs_4_0";
	}
	else if ( shaderTypeName == "gs" )
	{
		type = Type::Geometry;
		shaderTypeName = "gs_4_0";
	}
	else if ( shaderTypeName == "ps" )
	{
		type = Type::Pixel;
		shaderTypeName = "ps_4_0";
	}

	D3D_SHADER_MACRO		*pDefList;
	int									macroNum = 0;

	if ( argList.size() )
	{
		macroNum = (argList.size() / 2) + 1;
		pDefList = new D3D_SHADER_MACRO[ macroNum ];
		pDefList[ macroNum - 1 ].Name = NULL;
		pDefList[ macroNum - 1 ].Definition = NULL;
	}

	for( unsigned int i = 0; i < argList.size(); i += 2 )
	{
		pDefList[i/2].Name = argList[i].c_str();
		pDefList[i/2].Definition = argList[i+1].c_str();
	}

	hr = D3DCompile( m_Source[type].c_str(), m_Source[type].length(), NULL, pDefList, NULL, shaderFuncName.c_str(), shaderTypeName.c_str(), dwShaderFlags, 0, &pBlob, &pErrorBlob );

	if( FAILED(hr) )
	{

		if ( shaderTypeName == "vs_4_0" )
		{
			shaderFuncName = "VsMain";
		}
		else if ( shaderTypeName == "gs_4_0" )
		{
			shaderFuncName = "GsMain";
		}
		else if ( shaderTypeName == "ps_4_0" )
		{
			shaderFuncName = "PsMain";
		}

		hr = D3DCompile( m_Source[type].c_str(), m_Source[type].length(), NULL, pDefList, NULL, shaderFuncName.c_str(), shaderTypeName.c_str(), dwShaderFlags, 0, &pBlob, &pErrorBlob );

		if( FAILED(hr) )
		{
			if( pErrorBlob != NULL )
			{
				m_ErrorCode = (char*)pErrorBlob->GetBufferPointer();
				if ( pDefList )				delete[] pDefList;
				if ( pBlob )					pBlob->Release();
				if ( pErrorBlob )			pErrorBlob->Release();
				return;
			}
		}
	}

	{
		FileHeader				fileHeader;
		

		memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
		strcat_s( fileHeader.shaderName, m_ShaderName.c_str() );

		util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderName.c_str()) );
		fileHeader.shaderNameHash = hash.value;

		m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
	}

	{
		int					shaderNum = 1;
		m_BinaryMaker.Write( &shaderNum, sizeof(shaderNum) );
	}

	ShaderHeader		header;

	header.size = pBlob->GetBufferSize();
	header.shaderType = type;
	memset( header.path, 0, sizeof(header.path) );
	ParserUtil::GetFileName( header.path, m_FileName.c_str() );

	m_BinaryMaker.Write( &header, sizeof(header) );
	m_BinaryMaker.Write(pBlob->GetBufferPointer(), pBlob->GetBufferSize());

	if ( pDefList )			delete[] pDefList;
	if ( pBlob )				pBlob->Release();
	if ( pErrorBlob )		pErrorBlob->Release();*/
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::CompileGLSL( std::string shaderTypeName, std::vector<std::string> argList )
{
	Type						type;

	if ( shaderTypeName == "vs" )
	{
		type = Type::Vertex;
	}
	else if ( shaderTypeName == "gs" )
	{
		type = Type::Geometry;
	}
	else if ( shaderTypeName == "ps" )
	{
		type = Type::Pixel;

	}

	{
		FileHeader				fileHeader;
		

		memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
		strcat_s( fileHeader.shaderName, m_ShaderName.c_str() );

		util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderName.c_str()) );
		fileHeader.shaderNameHash = hash.value;
		fileHeader.shaderNum = 1;

		m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
	}

	{
		int					shaderNum = 1;
		m_BinaryMaker.Write( &shaderNum, sizeof(shaderNum) );
	}

	ShaderHeader		header;

	header.size = m_Source[type].length();
	header.shaderType = type;
	memset( header.path, 0, sizeof(header.path) );
	ParserUtil::GetFileName( header.path, m_FileName.c_str() );

	m_BinaryMaker.Write( &header, sizeof(header) );
	m_BinaryMaker.Write( m_Source[type].c_str(), header.size );
}

void ShaderCompiler::CompileCTR(std::string shaderTypeName, std::vector<std::string> argList )
{
	std::string					modulePath;

	{
		char Path[MAX_PATH+1]; 

		GetModuleFileName( NULL, Path, MAX_PATH );
		char drive[MAX_PATH+1]
				,dir  [MAX_PATH+1]
				,fname[MAX_PATH+1]
				,ext  [MAX_PATH+1];

		_splitpath(Path,drive,dir,fname,ext);//パス名を構成要素に分解します

		modulePath = drive;
		modulePath += dir;
		ParserUtil::ReplaceStr( modulePath, "\\", "/" );
	}

	std::string					tempFileName = modulePath + "temp.vsh";
	std::string					tempObjFileName = modulePath + "temp.vsh.o";
	std::string					tempBinFileName = modulePath + "temp.vsh.shbin";

	{//コードを抜き出しvshファイル作成
		std::vector<std::string>			codeList;
		codeList = ParserUtil::SplitStr( m_Source[Type::Vertex], "//<ctr_code_start>" );
		codeList = ParserUtil::SplitStr( codeList[1], "//<ctr_code_end>" );

		std::string										vshCode = codeList[0];

		ParserUtil::ReplaceStr( vshCode, "/<ctr_code_start>", "" );

		ParserUtil::FileCreate( tempFileName.c_str(), vshCode.c_str(), vshCode.length() );
	}

	{//コンパイル
		std::string			assemblerPath = getenv( "CTRSDK_ROOT" );//C:\home\xy_ctrlib\CTR_SDK
		std::string			linkerPath;

		ParserUtil::ReplaceStr( assemblerPath, "\\", "/" );
		linkerPath = assemblerPath;

		assemblerPath += "/tools/CommandLineTools/ctr_VertexShaderAssembler32.exe";
		linkerPath += "/tools/CommandLineTools/ctr_VertexShaderLinker32.exe";

		STARTUPINFO si;// スタートアップ情報
		PROCESS_INFORMATION pi;// プロセス情報

		{//アセンブル
			ZeroMemory(&si,sizeof(si));
			si.cb=sizeof(si);

			std::string					arg = assemblerPath;
			arg += " " + tempFileName + " -O" +tempObjFileName;

			CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

			CloseHandle(pi.hThread);

			WaitForSingleObject(pi.hProcess,INFINITE);

			CloseHandle(pi.hProcess);
		}

		{//リンク
			ZeroMemory(&si,sizeof(si));
			si.cb=sizeof(si);

			std::string					arg = linkerPath;
			arg += " " + tempObjFileName + " -O" +tempBinFileName;

			CreateProcess( NULL, (char*)arg.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi );

			CloseHandle(pi.hThread);

			WaitForSingleObject(pi.hProcess,INFINITE);

			CloseHandle(pi.hProcess);
		}
	}

	BinaryMaker		commandBinaryMaker;
	int						shaderNum = 1;

	{//コマンド作成
		nn::gr::CTR::ShaderLite				shaderLite;
		u32														command[1024*10];
		s32														size;

		char		*pShaderBin = ShaderCompiler::ReadFile( tempBinFileName.c_str(), &size, 1 );

		u32		workSize = shaderLite.GetRequiredBufferSize( pShaderBin );
		u32*	pWork = new u32[workSize];

		memset( command, 0, sizeof(command) );
		memset( pWork, 0, sizeof(u32) * workSize );

		shaderLite.SetupBinary( pShaderBin, 0, -1, pWork );
		u32			*pTail;
		u32			length;

		u32			dummyCommand[] =  { 0x00000000, 0x00000000 };
		u32			returnCommand[] = { 0x00000001, 0x000f023d };
		u32			commandHeader[4] = { 0, 0, 0, 0 };

		{
			pTail = shaderLite.MakeFullCommand( command );
			length = (u32)pTail - (u32)command;

			if ( (length % 16) == 0 )
			{
				commandHeader[0] = length + 16;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
			else
			{
				commandHeader[0] = length + 8;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
		}

		{
			pTail = shaderLite.MakeShaderCommand( command, true );
			length = (u32)pTail - (u32)command;

			if ( (length % 16) == 0 )
			{
				commandHeader[0] = length + 16;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
			else
			{
				commandHeader[0] = length + 8;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
		}

		{
			pTail = shaderLite.MakeShaderCommand( command, false );
			length = (u32)pTail - (u32)command;

			if ( (length % 16) == 0 )
			{
				commandHeader[0] = length + 16;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( dummyCommand, sizeof(dummyCommand) );
				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
			else
			{
				commandHeader[0] = length + 8;

				commandBinaryMaker.Write( commandHeader, sizeof(commandHeader) );
				commandBinaryMaker.Write( command, length );

				commandBinaryMaker.Write( returnCommand, sizeof(returnCommand) );
			}
		}

		delete[] pShaderBin;
		delete[] pWork;

		commandBinaryMaker.CreateFixedBuffer();
	}

	{//----バイナリ生成-----
		{//ヘッダー
			FileHeader				fileHeader;
		
			memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
			strcat_s( fileHeader.shaderName, m_ShaderName.c_str() );

			util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderName.c_str()) );
			fileHeader.shaderNameHash = hash.value;
			fileHeader.shaderNum = shaderNum;

			m_BinaryMaker.Write( &fileHeader, sizeof(fileHeader) );
		}

		for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
		{
			{
				ShaderHeader		header;

				header.size = commandBinaryMaker.GetBinarySize();
				header.shaderType = Type::Vertex;
				memset( header.path, 0, sizeof(header.path) );
				ParserUtil::GetFileName( header.path, m_FileName.c_str() );

				m_BinaryMaker.Write( &header, sizeof(header) );
			}

			m_BinaryMaker.Write(commandBinaryMaker.CreateFixedBuffer(), commandBinaryMaker.GetBinarySize());
		}
	}
}

//-----------------------------------------------------------------------------
/// @brief	
//-----------------------------------------------------------------------------
void ShaderCompiler::WriteFile( std::string filePath )
{
	u8*			pData = m_BinaryMaker.CreateFixedBuffer();
	u32			binarySize = m_BinaryMaker.GetBinarySize();

	static const u32	sc_Version = 0x14110400;
	u32								dataCount = 1;

	{
		BinaryMaker		bm;

		// バージョン
		bm.Write(&sc_Version, sizeof(sc_Version));

		// データ数
		bm.Write(&dataCount, sizeof(dataCount));

		//16バイトアライン
		{
			u32				padding[2] = {0, 0};
			bm.Write( &padding, sizeof(padding) );
		}

		//
		GfBinaryMdlData::ChunkHeader		modelHeader(IResourceData::Type("shader").m_ID, binarySize);
		bm.Write(&modelHeader, sizeof(modelHeader));

		bm.Write(pData, binarySize);

		u8*			pOutData = bm.CreateFixedBuffer();
		u32			outSize = bm.GetBinarySize();

		ShaderCompiler::WriteFile( filePath.c_str(), pOutData, outSize );

		delete [] pOutData;
	}

	delete [] pData;
}

//-----------------------------------------------------------------------------
/// @brief	リードファイル関数
//-----------------------------------------------------------------------------
char* ShaderCompiler::ReadFile( const char* filename, int* p_size, int align )
{
	FILE		*hFile;
	int			file_size;
	char		*read_buff;

	fopen_s( &hFile, filename, "rb" );

	if ( hFile == NULL ){
		return NULL;
	}

	fseek( hFile, 0, SEEK_END );
	file_size = ftell( hFile );
	*p_size = file_size;
	fseek( hFile, 0, SEEK_SET );

	file_size += align;//必ずNULLが入るようにしたい。
	int buf_size = file_size + ((-file_size) & (align-1));

	read_buff = new char[buf_size];
	memset( read_buff, 0, buf_size );
	fread( read_buff, file_size, 1, hFile );
	fclose( hFile );
	
	return( read_buff );
}

//-----------------------------------------------------------------------------
/// @brief	ファイル生成関数
//-----------------------------------------------------------------------------
void ShaderCompiler::WriteFile( const char* filename, const void* p_data, int size )
{
	FILE		*hFile;

	char					filePath[128];

	ShaderCompiler::GetFilePath( filePath, filename );

	if ( PathIsDirectory(filePath) == false )
	{
		CreateDirectory( filePath, NULL );
	}

	fopen_s( &hFile, filename, "wb" );
	fwrite( p_data, 1, size, hFile );
	fclose( hFile );
}

//-----------------------------------------------------------------------------
/// @brief	ファイルパスの取得
//-----------------------------------------------------------------------------
void ShaderCompiler::GetFilePath( char* p_out, const char* filename )
{
	int			size;

	size = (int)strlen( filename );

	while( size ){
		if ( filename[size] == '/' )		break;
		if ( filename[size] == '\\' )		break;
		--size;
	}

	if ( size )
	{
		memcpy( p_out, filename, size + 1 );
		p_out[ size + 1 ] = 0;//term code
	}
}