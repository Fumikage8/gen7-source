#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNodeFactory.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

#ifdef PERFORMANCE_COUNTER_ENABLE
	#include <system/include/Timer/PerformanceCounter.h>
using namespace gfl2::system;
#endif

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

#include <d3d11.h>
#include <d3dcompiler.h>

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

class StringUtil{
public:
	static std::wstring ShitJIStoUTF16( const std::string& str )
	{
		int len = ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, NULL, 0 );

		if ( len > 0 ) { 
			std::vector<wchar_t> utf16( len );

			if ( ::MultiByteToWideChar( CP_ACP, 0, str.c_str(), -1, &utf16[0], len ) ){
				return std::wstring( &utf16[0] );
			}
		}

		return NULL;
	}

	static std::string GetRelativePath( std::string filePath, std::string fileDir )
	{
		std::string										fileName = ParserUtil::GetFileNameStr( filePath.c_str() );
		ParserUtil::ReplaceStr( filePath, "\\", "/" );
		std::vector<std::string>			dirNames = ParserUtil::SplitStr( filePath, "/" );
		std::string										textureDirName = "";

		if ( dirNames.size() >= 2 )
		{
			textureDirName = dirNames[ dirNames.size() - 2 ];
			textureDirName += "/";
		}

		filePath = fileDir + textureDirName + fileName;

		return filePath;
	}
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  TextureData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct TextureData
{
	TextureData():m_pBinaryData(NULL){}
	~TextureData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	u32															m_No;
	std::string				m_FilePath;


	u32															m_BinarySize;
	u8															*m_pBinaryData;
  util::FnvHash                   m_Hash;

	void ReadTextureDataNode( const xml_node<>* top_node, std::string fileDir );
	void CreateBinary();
};

void TextureData::ReadTextureDataNode( const xml_node<>* top_node, std::string fileDir )
{
	m_No = atoi( top_node->first_attribute("No")->value() );

	const xml_node<>* path_node = top_node->first_node("FilePath");

	m_FilePath = path_node->value();

	if ( fileDir.length())
		m_FilePath = StringUtil::GetRelativePath( m_FilePath, fileDir );
}

void TextureData::CreateBinary()
{
	BinaryMaker			binary;

	struct ImageHeader
	{//16バイトアラインサイズ
		s32						size;
		s32						dumy[3];
		c8						path[Constant::RESOURCE_FILE_NAME_MAX];
	};

	ImageHeader				header;

	memset( header.path, 0, sizeof(header.path) );
	ParserUtil::GetFileName( header.path, m_FilePath.c_str() );

	c8*			pImageData = ParserUtil::ReadFile( m_FilePath.c_str(), &header.size );

	if ( pImageData == NULL )
	{
		::OutputDebugStringA( "====================== renderingengine 警告： " );
		::OutputDebugStringA( m_FilePath.c_str() );
		::OutputDebugStringA( " が見つかりませんでした。ダミーテクスチャを割り当てます。======================\n" );

		GF_ALIGNED(0x80) static unsigned char  s_Dummy_tga[] = {
			#include "../../../Resource/Dummy.tga.txt"
			0x00
		};

		pImageData = reinterpret_cast<c8*>(s_Dummy_tga);
		header.size = sizeof(s_Dummy_tga);
		binary.Write( &header, sizeof(header) );
		binary.Write( pImageData, header.size );

		m_pBinaryData = binary.CreateFixedBuffer();
		m_BinarySize = binary.GetBinarySize();
		m_Hash = util::FnvHash(reinterpret_cast<const u8*>(pImageData));
	}
	else
	{
		binary.Write( &header, sizeof(header) );
		binary.Write( pImageData, header.size );

		m_pBinaryData = binary.CreateFixedBuffer();
		m_BinarySize = binary.GetBinarySize();
		m_Hash = util::FnvHash(reinterpret_cast<const u8*>(pImageData));

		delete[] pImageData;
		pImageData = NULL;
	}

}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  ShaderData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct ShaderData
{
	ShaderData():m_pBinaryData(NULL){}
	~ShaderData(){}

	void destroy()
	{
		if ( m_pBinaryData )
		{
			delete[] m_pBinaryData;
			m_pBinaryData = NULL;
		}
	}

	enum ShaderKind{
		Vertex,
		Fragment,
		Geometry,
		Max
	};
	
	struct FileHeader
	{//16バイトアラインサイズ
    c8								shaderName[Constant::RESOURCE_FILE_NAME_MAX];
		s32								shaderNameHash;
		s32								dumy[3];
	};

	struct ShaderHeader
	{//16バイトアラインサイズ
		s32						size;
		u32						shaderType;
		s32						dumy[2];
		c8						path[Constant::RESOURCE_FILE_NAME_MAX];
	};


	u32													m_No;
	std::string									m_ShaderName;
	std::string									m_VsFilePath;
	std::string									m_FsFilePath;
	std::string									m_GsFilePath;


	u32															m_BinarySize;
	u8															*m_pBinaryData;
  util::FnvHash                   m_VsHash;
  util::FnvHash                   m_FsHash;
	util::FnvHash                   m_GsHash;

	void ReadShaderDataNode( const xml_node<>* top_node, std::string fileDir );
	void CreateBinary();
};


void ShaderData::ReadShaderDataNode( const xml_node<>* top_node, std::string fileDir )
{
	m_No = atoi( top_node->first_attribute("No")->value() );
	{
		m_ShaderName = top_node->first_attribute("Name")->value();
		std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_ShaderName );
		m_ShaderName = ParserUtil::UTF16toShitJIS( utf16 );
	}

	{
		const xml_node<>* path_node = top_node->first_node("VshFilePath");
		m_VsFilePath = path_node->value();

		std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_VsFilePath );
		m_VsFilePath = ParserUtil::UTF16toShitJIS( utf16 );
		if ( fileDir.length())
			m_VsFilePath = StringUtil::GetRelativePath( m_VsFilePath, fileDir );
	}

	{
		const xml_node<>* path_node = top_node->first_node("GshFilePath");
		if ( path_node )
		{
			m_GsFilePath = path_node->value();

			std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_GsFilePath );
			m_GsFilePath = ParserUtil::UTF16toShitJIS( utf16 );
			if ( fileDir.length())
				m_GsFilePath = StringUtil::GetRelativePath( m_GsFilePath, fileDir );
		}
	}

	{
		const xml_node<>* path_node = top_node->first_node("FshFilePath");
		m_FsFilePath = path_node->value();

		std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_FsFilePath );
		m_FsFilePath = ParserUtil::UTF16toShitJIS( utf16 );
		if ( fileDir.length())
			m_FsFilePath = StringUtil::GetRelativePath( m_FsFilePath, fileDir );
	}
}

void ShaderData::CreateBinary()
{
	BinaryMaker			binary;

	{
		FileHeader				fileHeader;
		

		memset( fileHeader.shaderName, 0, sizeof(fileHeader.shaderName) );
		strcat_s( fileHeader.shaderName, m_ShaderName.c_str() );

		util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderName.c_str()) );
		fileHeader.shaderNameHash = hash.value;

		binary.Write( &fileHeader, sizeof(fileHeader) );
	}

	std::string						files[ShaderKind::Max] = {
		m_VsFilePath,
		m_FsFilePath,
		m_GsFilePath
	};

	{
		int					shaderNum = 0;

		for( int type = 0; type < ShaderKind::Max; ++type )
		{
			if ( files[type].length() == 0 )
				continue;
			++shaderNum;
		}
		
		binary.Write( &shaderNum, sizeof(shaderNum) );
	}

	static const char*		option[ShaderKind::Max] = {
		"vs_4_0",
		"ps_4_0",
		"gs_4_0"
	};

	static const char*		funcName[ShaderKind::Max] = {
		"VsMain",
		"PsMain",
		"GsMain",
	};
	
	for( int type = 0; type < ShaderKind::Max; ++type )
	{
		if ( files[type].length() == 0 )
			continue;

		s32							size;
		ShaderHeader		header;

		c8*				pShaderBin = NULL;
    c8* pAllocatedMemory = NULL;

		{
			std::string					shaderFilePath = files[type];
			c8*									pImageData = ParserUtil::ReadFile( shaderFilePath.c_str(), &size );
      pAllocatedMemory = pImageData;

			pShaderBin = pImageData;

#if defined(_WIN64)

			if ( pImageData == NULL )
			{ 
				::OutputDebugStringA( "====================== renderingengine 警告： " );
				::OutputDebugStringA( shaderFilePath.c_str() );
				::OutputDebugStringA( " が見つかりませんでした。ダミーシェーダーを割り当てます。======================\n" );

				GF_ALIGNED(0x80) static unsigned char  s_dummy_gfvsh[] = {
					#include "../../../Resource/dummy.gfvsh.txt"
					0x00
				};

				GF_ALIGNED(0x80) static unsigned char  s_dummy_gffsh[] = {
					#include "../../../Resource/dummy.gffsh.txt"
					0x00
				};

				switch( type ){
				case ShaderKind::Vertex:
					pImageData = reinterpret_cast<c8*>(s_dummy_gfvsh);
					break;
				case ShaderKind::Fragment:
					pImageData = reinterpret_cast<c8*>(s_dummy_gffsh);
					break;
				}

				ID3DBlob		*pBlob = NULL;
				ID3DBlob		*pErrorBlob = NULL;

			UINT				flagCompile = D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_SKIP_VALIDATION;
			#if defined (DEBUG_DEVICE)
				flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
			#endif

        D3D_SHADER_MACRO  deflist[]= {
			    {  "HLSL",  "1", },
			    {  NULL, NULL },
		    };
				HRESULT		hr = D3DCompile( pImageData, strlen(pImageData), NULL, deflist, NULL, funcName[type], option[type], flagCompile, 0, &pBlob, &pErrorBlob );

				if (FAILED(hr))
				{
					char*			err = (char*)pErrorBlob->GetBufferPointer();
					GFL_ASSERT(0);
				}

				size = static_cast<s32>( pBlob->GetBufferSize() );
				pShaderBin = new char[size];
				memcpy( pShaderBin, pBlob->GetBufferPointer(), size );
				pBlob->Release();
				if ( pErrorBlob )
					pErrorBlob->Release();
			}
			else
			{
				util::FnvHash				hash( reinterpret_cast<u8*>(pImageData) );
				char								cacheFilePath[GFL_MAX_FILEPATH];	
				std::string					cacheFileDir;

				switch( type ){
				case ShaderKind::Vertex:
          m_VsHash = hash;
					break;
				case ShaderKind::Fragment:
          m_FsHash = hash;
					break;
				}

				delete[] pImageData;

				cacheFileDir = ParserUtil::GetFilePathStr( files[type].c_str() );

				memset(cacheFilePath, 0, sizeof(cacheFilePath));
			#if defined (DEBUG_DEVICE)
				sprintf_s( cacheFilePath, "%s/ShaderCache/%d.cache", cacheFileDir.c_str(), hash.value );	
			#else
				sprintf_s( cacheFilePath, "%s/ShaderCache/%d.cache.opt", cacheFileDir.c_str(), hash.value );
			#endif

				//if ( m_FsFilePath.find( "Bchara_hlight07バトルキャラシェーダー.gffsh" ) == std::string::npos )
				s32							alineSize;
				pShaderBin = ParserUtil::ReadFile( cacheFilePath, &size, &alineSize);

				if ( pShaderBin == NULL )
				{
					UINT				flagCompile = D3D10_SHADER_OPTIMIZATION_LEVEL3 | D3D10_SHADER_SKIP_VALIDATION;
				#if defined (DEBUG_DEVICE)
					flagCompile = D3D10_SHADER_DEBUG | D3D10_SHADER_SKIP_OPTIMIZATION | D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_OPTIMIZATION_LEVEL0;
				#endif
					ID3DBlob		*pBlob = NULL;
					ID3DBlob		*pErrorBlob = NULL;

					D3D_SHADER_MACRO  deflist[]= {
						{  "HLSL",  "1", },
						{  NULL, NULL },
					};

					std::wstring	pathWStr = StringUtil::ShitJIStoUTF16( shaderFilePath );
					HRESULT		hr = D3DCompileFromFile( pathWStr.c_str(), deflist, NULL,  funcName[type], option[type], flagCompile, 0, &pBlob, &pErrorBlob );

					if (FAILED(hr))
					{
						char*			err = (char*)pErrorBlob->GetBufferPointer();
						GFL_ASSERT(0);
					}

					size = static_cast<s32>( pBlob->GetBufferSize() );
					pShaderBin = new char[size];
					memcpy( pShaderBin, pBlob->GetBufferPointer(), size );
					pBlob->Release();
					if ( pErrorBlob )
						pErrorBlob->Release();

					ParserUtil::FileCreate( cacheFilePath, reinterpret_cast<char*>(pShaderBin), size );
				}
			}
#else
      if ( pImageData == NULL )
			{ 
				::OutputDebugStringA( "====================== renderingengine 警告： " );
				::OutputDebugStringA( shaderFilePath.c_str() );
				::OutputDebugStringA( " が見つかりませんでした。ダミーシェーダーを割り当てます。======================\n" );

				GF_ALIGNED(0x80) static unsigned char  s_dummy_gfvsh[] = {
					#include "../../../Resource/dummy.gfvsh.txt"
					0x00
				};

				GF_ALIGNED(0x80) static unsigned char  s_dummy_gffsh[] = {
					#include "../../../Resource/dummy.gffsh.txt"
					0x00
				};

				switch( type ){
				case ShaderKind::Vertex:
          size = sizeof(s_dummy_gfvsh);
					pImageData = reinterpret_cast<c8*>(s_dummy_gfvsh);
					break;
				case ShaderKind::Fragment:
          size = sizeof(s_dummy_gffsh);
					pImageData = reinterpret_cast<c8*>(s_dummy_gffsh);
					break;
				case ShaderKind::Geometry:
					size = 0;
					//変形せずにそのまま出力
					break;
				}
        
        pShaderBin = pImageData;
      }
#endif

		}

		if ( size )
		{
			header.size = size;
			header.shaderType = type;
			memset( header.path, 0, sizeof(header.path) );
			ParserUtil::GetFileName( header.path, files[type].c_str() );

			binary.Write( &header, sizeof(header) );
			binary.Write( pShaderBin, header.size );

	#if defined(_WIN32)
      if (pAllocatedMemory != NULL)
      {
        delete[] pAllocatedMemory;
      }
	#endif
		}
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}



//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  ModelData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct ModelData
{
	ModelData():m_pBinaryData(NULL){}
	~ModelData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	struct TextureLinkData{
		u32							m_No;
		std::string			m_FilePath;
	};

	math::AABB											m_AABB;
	std::vector<std::string>				m_ShaderList;
	std::vector<TextureLinkData>		m_TextureList;
	std::vector<std::string>				m_MaterialList;
	std::vector<std::string>				m_MeshPathList;
	std::vector<std::string>				m_MeshList;
	u32															m_BinarySize;
	u8															*m_pBinaryData;

	void ReadModelDataNode( const xml_node<>* top_node );
	void CreateBinary( const u8* pJointData, u32 jointDataSize );
};

void ModelData::ReadModelDataNode( const xml_node<>* top_node )
{
	for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
	{
		std::string				nodeName = node->name();

		if ( nodeName == "ModelAABB" )
		{
			std::vector<math::Vector4>		aabb;
			ParserUtil::ParseFloatArray( &aabb, node, 3, 0.0f );
			m_AABB.SetMin( aabb[0] );
			m_AABB.SetMax( aabb[1] );
		}
		else if ( nodeName == "TextureList" )
		{
			for( const xml_node<>* texture_node = node->first_node(); texture_node; texture_node = texture_node->next_sibling( ) )
			{
				TextureLinkData		linkData;
				std::string				textureNodeName = texture_node->name();

				linkData.m_No = std::atoi( texture_node->first_attribute( "No" )->value() );
				linkData.m_FilePath = texture_node->first_node("FilePath")->value();

				m_TextureList.push_back( linkData );
			}
		}
		if ( nodeName == "ShaderList" )
		{
			for( const xml_node<>* shader_node = node->first_node("Shader"); shader_node; shader_node = shader_node->next_sibling("Shader") )
			{
				std::string			name = shader_node->first_attribute("Name")->value();
				std::wstring			utf16 = ParserUtil::UTF8toUTF16( name );
				name = ParserUtil::UTF16toShitJIS( utf16 );
				m_ShaderList.push_back( name );
			}
		}
		else if ( nodeName == "Materials" )
		{
			for( const xml_node<>* material_node = node->first_node(); material_node; material_node = material_node->next_sibling( ) )
			{
				m_MaterialList.push_back( material_node->first_attribute( "Name" )->value() );
			}
		}
		else if ( nodeName == "Meshes" )
		{
			for( const xml_node<>* mesh_node = node->first_node(); mesh_node; mesh_node = mesh_node->next_sibling( ) )
			{
				std::string					fullPath = mesh_node->first_attribute( "Name" )->value();
				u32									size = static_cast<u32>( fullPath.length() + 1 );
				c8									*pathBuffer = new c8[ size ];

				memset( pathBuffer, 0, size );
				ParserUtil::GetNodeName( pathBuffer, size, fullPath.c_str() );
				GFL_ASSERT( strlen(pathBuffer) < Constant::RESOURCE_FILE_NAME_MAX );

				m_MeshPathList.push_back( fullPath );
				m_MeshList.push_back( pathBuffer );

				delete[] pathBuffer;
			}
		}
	}
}

void ModelData::CreateBinary( const u8* pJointData, u32 jointDataSize )
{
	BinaryMaker			binary;

	{//Shaders情報
		u32					ShaderNum = static_cast<u32>( m_ShaderList.size() );
		binary.Write( &ShaderNum, sizeof( ShaderNum ) );

		for( u32 i = 0; i < m_ShaderList.size(); ++i )
		{
			util::FnvHash				hash( reinterpret_cast<const u8*>(m_ShaderList[i].c_str()) );
			c8									*pathBuffer = new c8[ m_ShaderList[i].length() + 1];
			c8									nameBuffer[Constant::RESOURCE_FILE_NAME_MAX];

			memset( nameBuffer, 0, sizeof(nameBuffer) );

			ParserUtil::GetFileName( pathBuffer, m_ShaderList[i].c_str() );
			GFL_ASSERT( strlen(pathBuffer) < Constant::RESOURCE_FILE_NAME_MAX );
			strcat_s( nameBuffer, pathBuffer );

			binary.Write( &hash.value, sizeof( hash.value ) );
			binary.Write( nameBuffer, Constant::RESOURCE_FILE_NAME_MAX );

			delete[] pathBuffer;
		}
	}

	{//TextureList情報
		u32					textureNum = static_cast<u32>( m_TextureList.size() );
		binary.Write( &textureNum, sizeof( textureNum ) );

		for( u32 i = 0; i < m_TextureList.size(); ++i )
		{
			util::FnvHash				hash( reinterpret_cast<const u8*>(m_TextureList[i].m_FilePath.c_str()) );
			c8									*pathBuffer = new c8[ m_TextureList[i].m_FilePath.length() + 1];
			c8									nameBuffer[Constant::RESOURCE_FILE_NAME_MAX];

			memset( nameBuffer, 0, sizeof(nameBuffer) );

			ParserUtil::GetFileName( pathBuffer, m_TextureList[i].m_FilePath.c_str() );
			GFL_ASSERT( strlen(pathBuffer) < Constant::RESOURCE_FILE_NAME_MAX );
			strcat_s( nameBuffer, pathBuffer );

			//binary.Write( &m_TextureList[i].m_No, sizeof( m_TextureList[i].m_No ) );
			binary.Write( &hash.value, sizeof( hash.value ) );
			binary.Write( nameBuffer, Constant::RESOURCE_FILE_NAME_MAX );

			delete[] pathBuffer;
		}
	}

	{//Materials情報
		u32					materialNum = static_cast<u32>( m_MaterialList.size() );
		binary.Write( &materialNum, sizeof( materialNum ) );

		for( u32 i = 0; i < m_MaterialList.size(); ++i )
		{
			util::FnvHash				hash( reinterpret_cast<const u8*>(m_MaterialList[i].c_str()) );
			c8									*pathBuffer = new c8[ m_MaterialList[i].length() + 1];
			c8									nameBuffer[Constant::RESOURCE_FILE_NAME_MAX];

			memset( nameBuffer, 0, sizeof(nameBuffer) );

			ParserUtil::GetFileName( pathBuffer, m_MaterialList[i].c_str() );
			GFL_ASSERT( strlen(pathBuffer) < Constant::RESOURCE_FILE_NAME_MAX );
			strcat_s( nameBuffer, pathBuffer );

			binary.Write( &hash.value, sizeof( hash.value ) );
			binary.Write( nameBuffer, Constant::RESOURCE_FILE_NAME_MAX );

			delete[] pathBuffer;
		}
	}

	{//Meshes情報
		u32					meshNum = static_cast<u32>( m_MeshList.size() );
		binary.Write( &meshNum, sizeof( meshNum ) );

		for( u32 i = 0; i < m_MeshList.size(); ++i )
		{
			util::FnvHash				hash( reinterpret_cast<const u8*>(m_MeshList[i].c_str()) );
			c8									nameBuffer[Constant::RESOURCE_FILE_NAME_MAX];

			memset( nameBuffer, 0, sizeof(nameBuffer) );
			strcat_s( nameBuffer, m_MeshList[i].c_str() );

			binary.Write( &hash.value, sizeof( hash.value ) );
			binary.Write( nameBuffer, Constant::RESOURCE_FILE_NAME_MAX );
		}
	}

	{//Model情報
		binary.Write( &m_AABB, sizeof( math::AABB ) );
	}

	if ( pJointData )
	{//Joint情報
		binary.Write( pJointData, jointDataSize );
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  MaterialData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct MaterialData
{
	MaterialData():m_pBinaryData(NULL) {}
	~MaterialData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	struct Texture
	{
		struct Attribute
		{
			u32						m_UvSetNo;
			std::string		m_MappintType;
			f32						m_ScaleU;
			f32						m_ScaleV;
			f32						m_Rotate;
			f32						m_TranslateU;
			f32						m_TranslateV;
			std::string		m_RepeatU;
			std::string		m_RepeatV;
			std::string		m_MagFilter;
			std::string		m_MinFilter;
		};

		u32					m_SlotNo;
		u32					m_Usage;
		std::string	m_UvUsage;
		Attribute		m_Attribute;
	};

	struct Combiner
	{
		std::string					m_PresetName;
	};

	std::string														m_Name;
	std::string														m_Type;
	std::vector<Texture>									m_TextureList;
	Combiner															m_Combiner;
	MaterialResourceNode::BlendTest				m_BlendTest;
  MaterialResourceNodeFactory::BinaryFileAccessor10::AttributeParam	m_AttributeParam;
	MaterialResourceNode::UserData				m_UserData;

	u8*															m_pBinaryData;
	u32															m_BinarySize;

	void ReadMaterialDataNode( const xml_node<>* top_node );
	void CreateBinary( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList );
	gfx::CompareFunc GetAlphaTestTypeFromAttributeName( const char* name );
	gfx::CompareFunc GetDepthTestTypeFromAttributeName( const char* name );
	gfx::CompareFunc GetStencilTestTypeFromAttributeName( const char* name );
	gfx::StencilOp GetStencilOperationFromAttributeName( const char* name );
	renderer::BlendMode GetBlendModeFromAttributeName( const char* name );
	gfx::BlendOp GetBlendFuncFromAttributeName( const char* name );
	renderer::BlendElement GetBlendElementFromAttributeName( const char* name );
	renderer::LogicalOperation GetLogicalOperationFromAttributeName( const char* name );
	LutInputAngleType GetLutInputAngleTypeFromAttributeName( const char* name );
	gfl2::gfx::CullMode GetCullModeFromAttributeName( const char* name );
	MathFormulaType GetMathFormulaTypeFromAttributeName( const char* name );
	s16 GetCombinerInputSourceFromAttributeName( const char* name );
	s16 GetCombinerOperandFromAttributeName( const char* name );
};

gfx::CompareFunc MaterialData::GetAlphaTestTypeFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"LESS",
		"LEQUAL",
		"EQUAL",
		"GEQUAL",
		"GREATER",
		"NOTEQUAL",
		"ALWAYS",
		"NEVER",
	};
	for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<gfx::CompareFunc>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return gfx::CompareFunc::NumberOf;
}

gfx::CompareFunc MaterialData::GetDepthTestTypeFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"LESS",
		"LEQUAL",
		"EQUAL",
		"GEQUAL",
		"GREATER",
		"NOTEQUAL",
		"ALWAYS",
		"NEVER",
	};
	for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<gfx::CompareFunc>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return gfx::CompareFunc::NumberOf;
}

gfx::CompareFunc MaterialData::GetStencilTestTypeFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"LESS",
		"LEQUAL",
		"EQUAL",
		"GEQUAL",
		"GREATER",
		"NOTEQUAL",
		"ALWAYS",
		"NEVER",
	};
	for(u32 i = 0; i < gfx::CompareFunc::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<gfx::CompareFunc>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return gfx::CompareFunc::NumberOf;
}

gfx::StencilOp MaterialData::GetStencilOperationFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"KEEP",
		"ZERO",
		"REPLACE",
		"INCR",
		"INCR_WRAP",
		"DECR",
		"DECR_WRAP",
		"INVERT",
	};
	for(u32 i = 0; i < gfx::StencilOp::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<gfx::StencilOp>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return gfx::StencilOp::NumberOf;
}

renderer::BlendMode MaterialData::GetBlendModeFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"Blend",
		"Separate",
		"LogicOp",
		"None",
	};
	for(u32 i = 0; i < renderer::BlendMode::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<renderer::BlendMode>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return renderer::BlendMode::NumberOf;
}

 gfx::BlendOp MaterialData::GetBlendFuncFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"ADD",
		"SUBTRACT",
		"REVERSE_SUBTRACT",
		"MIN",
		"MAX",
	};
	for(u32 i = 0; i <  gfx::BlendOp::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast< gfx::BlendOp>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return  gfx::BlendOp::NumberOf;
}

renderer::BlendElement MaterialData::GetBlendElementFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"Zero",
		"One",
		"SourceColor",
		"OneMinusSourceColor",
		"DestinationColor",
		"OneMinusDestinationColor",
		"SourceAlpha",
		"OneMinusSourceAlpha",
		"DestinationAlpha",
		"OneMinusDestinationAlpha",
		"ConstantColor",
		"OneMinusConstantColor",
		"ConstantAlpha",
		"OneMinusConstantAlpha",
		"SourceAlphaSaturate",
	};
	for(u32 i = 0; i < renderer::BlendElement::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<renderer::BlendElement>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return renderer::BlendElement::NumberOf;
}

renderer::LogicalOperation MaterialData::GetLogicalOperationFromAttributeName(const char* name)
{
	const char* attributeNames[] = 
	{
		"CLEAR",
		"SET",
		"COPY",
		"NOOP",
		"COPY_INVERTED",
		"INVERT",
		"AND",
		"OR",
		"XOR",
		"AND_REVERSE",
		"OR_REVERSE",
		"AND_INVERTED",
		"OR_INVERTED",
		"NAND",
		"NOR",
		"EQUIV",
	};
	for(u32 i = 0; i < renderer::LogicalOperation::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<renderer::LogicalOperation>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return renderer::LogicalOperation::NumberOf;
}

LutInputAngleType MaterialData::GetLutInputAngleTypeFromAttributeName(const char* name)
{
	const char* attributeNames[] =
	{
		"NH",
		"VH",
		"NV",
		"LN",
		"SP",
		"CP"
	};
	for(u32 i = 0; i < renderer::LogicalOperation::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<LutInputAngleType>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return LutInputAngleType::NumberOf;
}

gfl2::gfx::CullMode MaterialData::GetCullModeFromAttributeName( const char* name )
{
	const char* attributeNames[] =
	{
		"Always",
		"Back",
		"Front"
	};
	for(u32 i = 0; i < gfl2::gfx::CullMode::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<gfl2::gfx::CullMode>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return gfl2::gfx::CullMode::NumberOf;
}

MathFormulaType MaterialData::GetMathFormulaTypeFromAttributeName( const char* name )
{
	const char* attributeNames[] =
	{
		"Replace",
		"Modulate",
		"Add",
		"AddSigned",
		"Interpolate",
		"Subtract",
		"DotProduct3Rgb",
		"DotProduct3Rgba",
		"AddMult",
		"MultAdd",
		"NumberOf"
	};
	for(u32 i = 0; i < MathFormulaType::NumberOf; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<MathFormulaType>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return MathFormulaType::NumberOf;
}

s16 MaterialData::GetCombinerInputSourceFromAttributeName( const char* name )
{
	const char* attributeNames[] = {
		"Texture0",
		"Texture1",
		"Texture2",
		"Texture3",		// !< 未使用(プロシージャル用)
		"Constant",
		"PrimaryColor",
		"FragmentPrimaryColor",
		"FragmentSecondaryColor",
		"Previous",
		"PreviousBuffer",
	};

	s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
	for(s16 i = 0; i < length; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return i;
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return length;
}

s16 MaterialData::GetCombinerOperandFromAttributeName( const char* name )
{
	const char* attributeNames[] = {
		"Color",
		"OneMinusColor",
		"Alpha",
		"OneMinusAlpha",
		"Red",
		"OneMinusRed",
		"Green",
		"OneMinusGreen",
		"Blue",
		"OneMinusBlue",
	};

	s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
	for(s16 i = 0; i < length; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return i;
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return length;
}

PokeNormalType GetPokeNormalTypeFromAttributeName( const char* name )
{
	const char* attributeNames[] = {
		"None",
		"Dummy",
		"ObjectSpace",
	};

	s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
	for(s16 i = 0; i < length; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<PokeNormalType>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return static_cast<PokeNormalType>(length);
}

BlendType GetBlendTypeFromAttributeName( const char* name )
{
	const char* attributeNames[] = {
		"None",
		"Nuki",			//! 抜き設定
		"Liner",		//! 線形補完
		"Add",			//! 加算
		"Sub",			//! 減算
		"Mul",			//! 乗算
		"Inverse",	//! ネガポジ反転
	};

	s16 length = static_cast<s16>(sizeof(attributeNames) / sizeof(attributeNames[0]));
	for(s16 i = 0; i < length; ++i)
	{
		if(strcmp(attributeNames[i], name) == 0)
		{
			return static_cast<BlendType>(i);
		}
	}
	GFL_ASSERT(0 && "アトリビュート名が不適切です");
	return static_cast<BlendType>(length);
}

void MaterialData::ReadMaterialDataNode( const xml_node<>* top_node )
{
	{
		m_Name = top_node->first_attribute( "Name" )->value();
		std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_Name );
		m_Name = ParserUtil::UTF16toShitJIS( utf16 );
	}

	m_Type = top_node->first_attribute( "Type" )->value();
  m_AttributeParam.m_Priority = 0;
  xml_attribute<>* priority_attribute = top_node->first_attribute("Priority");
  if (priority_attribute)
  {
    m_AttributeParam.m_Priority = atoi(priority_attribute->value());
  }
  xml_attribute<>* materialhash_attribute = top_node->first_attribute("MaterialHash");
  m_AttributeParam.m_MaterialHash = 0;
  if (materialhash_attribute)
  {
    m_AttributeParam.m_MaterialHash = static_cast<u32>(atoi(materialhash_attribute->value()));
  }


	{// <PsLightingEnable>などの先頭ノードの取得
		xml_node<>*				pslightingenable_node = top_node->first_node( "PsLightingEnable" );
		m_AttributeParam.m_PsLightingEnable = (pslightingenable_node != NULL);
		if (m_AttributeParam.m_PsLightingEnable)
		{
			m_AttributeParam.m_PsLightingEnable = ( atoi( pslightingenable_node->first_attribute( "PsLightingEnable" )->value() ) != 0 );
		}
		xml_node<>*				vslightingenable_node = top_node->first_node( "VsLightingEnable" );
		m_AttributeParam.m_VsLightingEnable = (vslightingenable_node != NULL);
		if (m_AttributeParam.m_VsLightingEnable)
		{
			m_AttributeParam.m_VsLightingEnable = ( atoi( vslightingenable_node->first_attribute( "VsLightingEnable" )->value() ) != 0 );
		}
		xml_node<>*				lightsetno_node = top_node->first_node( "LightSetNo" );
		m_AttributeParam.m_LightSetNo = (lightsetno_node != NULL);
		if (m_AttributeParam.m_LightSetNo)
		{
			m_AttributeParam.m_LightSetNo = (s16)atoi( lightsetno_node->first_attribute( "LightSetNo" )->value() );
		}
		xml_node<>*				fogenable_node = top_node->first_node( "FogEnable" );
		m_AttributeParam.m_FogEnable = (fogenable_node != NULL);
		if (m_AttributeParam.m_FogEnable)
		{
			m_AttributeParam.m_FogEnable = ( atoi( fogenable_node->first_attribute( "FogEnable" )->value() ) != 0 );
		}
		xml_node<>*				fogno_node = top_node->first_node( "FogNo" );
		m_AttributeParam.m_FogNo = 0;
		if (fogno_node)
		{
			m_AttributeParam.m_FogNo = atoi( fogno_node->first_attribute( "FogNo" )->value() );
		}
		xml_node<>*				cullmode_node = top_node->first_node( "CullMode" );
		m_AttributeParam.m_CullMode = gfl2::gfx::CullMode::CW;  // 標準はCW(裏面カリング)
		if (cullmode_node)
		{
			m_AttributeParam.m_CullMode = MaterialData::GetCullModeFromAttributeName( cullmode_node->first_attribute( "CullMode" )->value() );
		}
	}

	{// カラーの取得
		xml_node<>*				colors_node = top_node->first_node( "Colors" );
		if (colors_node)
		{
			xml_node<>*		materialcolor_node = colors_node->first_node( "MaterialColor" );
			if (materialcolor_node)
			{
				xml_node<>*		emission_node = materialcolor_node->first_node( "Emission" );
        gfx::Color    f32Color;
				f32Color.x = static_cast<float>( atof( emission_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( emission_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( emission_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( emission_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_MaterialColor.m_Emission = f32Color;

				xml_node<>*		ambient_node = materialcolor_node->first_node( "Ambient" );
				f32Color.x = static_cast<float>( atof( ambient_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( ambient_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( ambient_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( ambient_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_MaterialColor.m_Ambient = f32Color;

				xml_node<>*		diffuse_node = materialcolor_node->first_node( "Diffuse" );
				f32Color.x = static_cast<float>( atof( diffuse_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( diffuse_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( diffuse_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( diffuse_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_MaterialColor.m_Diffuse = f32Color;

				xml_node<>*		specular0_node = materialcolor_node->first_node( "Specular0" );
				f32Color.x = static_cast<float>( atof( specular0_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( specular0_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( specular0_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( specular0_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_MaterialColor.m_Specular0 = f32Color;

				xml_node<>*		specular1_node = materialcolor_node->first_node( "Specular1" );
				f32Color.x = static_cast<float>( atof( specular1_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( specular1_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( specular1_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( specular1_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_MaterialColor.m_Specular1 = f32Color;
			}
			xml_node<>*		constantcolor_node = colors_node->first_node( "ConstantColor" );
			if (constantcolor_node)
			{
				for(s32 i = 0; i < CombinerAttribute::CombinerMax; ++i)
				{
					const char* attr_name = "ConstantColor";
					char attr_buff[64];
					sprintf_s( attr_buff, "%s%d", attr_name, i );
					xml_node<>*		emission_node = constantcolor_node->first_node( attr_buff );
          gfx::Color    f32Color;
					f32Color.x = static_cast<float>( atof( emission_node->first_attribute( "R" )->value() ) );
					f32Color.y = static_cast<float>( atof( emission_node->first_attribute( "G" )->value() ) );
					f32Color.z = static_cast<float>( atof( emission_node->first_attribute( "B" )->value() ) );
					f32Color.w = static_cast<float>( atof( emission_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_ConstantColor[i] = f32Color;
				}
			}
			xml_node<>*		etccolor_node = colors_node->first_node( "EtcColor" );
			if (etccolor_node)
			{
				xml_node<>*		blendcolor_node = etccolor_node->first_node( "BlendColor" );
        gfx::Color    f32Color;
				f32Color.x = static_cast<float>( atof( blendcolor_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( blendcolor_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( blendcolor_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( blendcolor_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_EtcColor.m_BlendColor = f32Color;

				xml_node<>*		buffercolor_node = etccolor_node->first_node( "BufferColor" );
				f32Color.x = static_cast<float>( atof( buffercolor_node->first_attribute( "R" )->value() ) );
				f32Color.y = static_cast<float>( atof( buffercolor_node->first_attribute( "G" )->value() ) );
				f32Color.z = static_cast<float>( atof( buffercolor_node->first_attribute( "B" )->value() ) );
				f32Color.w = static_cast<float>( atof( buffercolor_node->first_attribute( "A" )->value() ) );
        m_AttributeParam.m_EtcColor.m_BufferColor = f32Color;

				for( s32 i = 0; i < CombinerAttribute::TextureMax; ++i )
				{
					char node_name[64] = {0};
					sprintf_s( node_name, "TextureBorderColor%d", i );
					xml_node<>*		texturebordercolor_node = etccolor_node->first_node( node_name );
					f32Color.x = static_cast<float>( atof( texturebordercolor_node->first_attribute( "R" )->value() ) );
					f32Color.y = static_cast<float>( atof( texturebordercolor_node->first_attribute( "G" )->value() ) );
					f32Color.z = static_cast<float>( atof( texturebordercolor_node->first_attribute( "B" )->value() ) );
					f32Color.w = static_cast<float>( atof( texturebordercolor_node->first_attribute( "A" )->value() ) );
          m_AttributeParam.m_EtcColor.m_TextureBorderColor[i] = f32Color;
				}
			}
		}
	}

	{// バンプ関連の情報
		xml_node<>*				bumpmapno_node = top_node->first_node( "BumpMapNo" );
		m_AttributeParam.m_BumpMapNo = 0;
		if (bumpmapno_node)
		{
			m_AttributeParam.m_BumpMapNo = static_cast<s16>( atoi( bumpmapno_node->first_attribute( "BumpMapNo" )->value() ) );
		}
		xml_node<>*				bumpmaptype_node = top_node->first_node( "BumpMapType" );
		m_AttributeParam.m_BumpMapType = 0;
		if (bumpmaptype_node)
		{
			m_AttributeParam.m_BumpMapType = static_cast<s16>( atoi( bumpmaptype_node->first_attribute( "BumpMapType" )->value() ) );
		}
		xml_node<>*				zrecastenable_node = top_node->first_node( "ZRecastEnable" );
		m_AttributeParam.m_ZRecastEnable = false;
		if (zrecastenable_node)
		{
			m_AttributeParam.m_ZRecastEnable = static_cast<b32>( atoi( zrecastenable_node->first_attribute( "ZRecastEnable" )->value() ) != 0 );
		}
	}

	{// テクスチャの取得
		xml_node<>*				textures_node = top_node->first_node( "Textures" );

		int cntTexture = 0;
		for( const xml_node<>* texture_node = textures_node->first_node("Texture");texture_node; texture_node = texture_node->next_sibling("Texture") )
		{
			Texture						texture;
			xml_attribute<>		*textureNoAttr = texture_node->first_attribute("Usage");

			if ( textureNoAttr == NULL )
			{
				textureNoAttr = texture_node->first_attribute("TextureNo");
			}
			
			texture.m_SlotNo				= atoi( texture_node->first_attribute( "SlotNo" )->value() );
			texture.m_Usage					= atoi( textureNoAttr->value() );
			texture.m_UvUsage				= texture_node->first_attribute( "UvUsage" )->value();

			for( const xml_node<>* attribute_node = texture_node->first_node("Attribute"); attribute_node; attribute_node = attribute_node->next_sibling("Attribute") )
			{
				texture.m_Attribute.m_UvSetNo			= atoi( attribute_node->first_attribute( "UvSetNo" )->value() );
				texture.m_Attribute.m_MappintType	= attribute_node->first_attribute( "MappintType" )->value();
				texture.m_Attribute.m_ScaleU			= static_cast<f32>( atof( attribute_node->first_attribute( "ScaleU" )->value() ) );
				texture.m_Attribute.m_ScaleV			= static_cast<f32>( atof( attribute_node->first_attribute( "ScaleV" )->value() ) );
				texture.m_Attribute.m_Rotate			= static_cast<f32>( atof( attribute_node->first_attribute( "Rotate" )->value() ) );
				texture.m_Attribute.m_TranslateU	= static_cast<f32>( atof( attribute_node->first_attribute( "TranslateU" )->value() ) );
				texture.m_Attribute.m_TranslateV	= static_cast<f32>( atof( attribute_node->first_attribute( "TranslateV" )->value() ) );
				texture.m_Attribute.m_RepeatU			= attribute_node->first_attribute( "RepeatU" )->value();
				texture.m_Attribute.m_RepeatV			= attribute_node->first_attribute( "RepeatV" )->value();
				texture.m_Attribute.m_MagFilter		= attribute_node->first_attribute( "MagFilter" )->value();
				texture.m_Attribute.m_MinFilter		= attribute_node->first_attribute( "MinFilter" )->value();
			}

			m_TextureList.push_back( texture );
			++cntTexture;
		}
	}

	{// フラグメント情報の取得
		xml_node<>*		fragments_node = top_node->first_node( "Fragments" );
		if( fragments_node )
		{
			// フラグメントライティングの項目の取得
			const xml_node<>*		fragmentlighting_node = fragments_node->first_node( "FragmentLighting" );

      {
        m_AttributeParam.m_PrimaryFresnelEnable = ( atoi( fragmentlighting_node->first_attribute( "PrimaryFresnelEnable" )->value() ) != 0 );
			  m_AttributeParam.m_SecondaryFresnelEnable = ( atoi( fragmentlighting_node->first_attribute( "SecondaryFresnelEnable" )->value() ) != 0 );
			  m_AttributeParam.m_SpecularColorClamp = ( atoi( fragmentlighting_node->first_attribute( "SpecularColorClamp" )->value() ) != 0 );
			  m_AttributeParam.m_CookTorrance0Enable = ( atoi( fragmentlighting_node->first_attribute( "CookTorrance0Enable" )->value() ) != 0 );
			  m_AttributeParam.m_CookTorrance1Enable = ( atoi( fragmentlighting_node->first_attribute( "CookTorrance1Enable" )->value() ) != 0 );
      }

			{
				const xml_node<>*	distribution0sampler_node = fragmentlighting_node->first_node( "Distribution0Sampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution0] = ( distribution0sampler_node != NULL);
				if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution0] )
				{
					m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(distribution0sampler_node->first_attribute( "LutInputAngleD0")->value() );
					m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution0].m_LutScale = static_cast<float>( atof(distribution0sampler_node->first_attribute( "LutScaleD0" )->value() ) );
				
				}
				
				const xml_node<>*	distribution1sampler_node = fragmentlighting_node->first_node( "Distribution1Sampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution1] = ( distribution1sampler_node != NULL);
				if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Distribution1] )
				{
					m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(distribution1sampler_node->first_attribute( "LutInputAngleD1")->value() );
					m_AttributeParam.m_LookUpTable[LookUpTableType::Distribution1].m_LutScale = static_cast<float>( atof(distribution1sampler_node->first_attribute( "LutScaleD1" )->value() ) );
					
				}
				
				const xml_node<>*	reflectancersampler_node = fragmentlighting_node->first_node( "ReflectanceRSampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionR] = ( reflectancersampler_node != NULL);
				if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionR] )
				{
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancersampler_node->first_attribute( "LutInputAngleRR")->value() );
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionR].m_LutScale = static_cast<float>( atof(reflectancersampler_node->first_attribute( "LutScaleRR" )->value() ) );
					
				}

				const xml_node<>*	reflectancegsampler_node = fragmentlighting_node->first_node( "ReflectanceGSampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionG] = ( reflectancegsampler_node != NULL);
				if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionG] )
				{
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancegsampler_node->first_attribute( "LutInputAngleRG")->value() );
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionG].m_LutScale = static_cast<float>( atof(reflectancegsampler_node->first_attribute( "LutScaleRG" )->value() ) );
					
				}

				const xml_node<>*	reflectancebsampler_node = fragmentlighting_node->first_node( "ReflectanceBSampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionB] = ( reflectancebsampler_node != NULL);
				if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::ReflectionB] )
				{
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(reflectancebsampler_node->first_attribute( "LutInputAngleRB")->value() );
					m_AttributeParam.m_LookUpTable[LookUpTableType::ReflectionB].m_LutScale = static_cast<float>( atof( reflectancebsampler_node->first_attribute( "LutScaleRB" )->value() ) );
					
				}

				const xml_node<>*	fresnelsampler_node = fragmentlighting_node->first_node( "FresnelSampler" );
				m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Fresnel] = (fresnelsampler_node != NULL);
        if ( m_AttributeParam.m_LookUpTableEnable[LookUpTableType::Fresnel] )
				{
          {
					  m_AttributeParam.m_LookUpTable[LookUpTableType::Fresnel].m_LutInputAngle = MaterialData::GetLutInputAngleTypeFromAttributeName(fresnelsampler_node->first_attribute( "LutInputAngleFR")->value() );
					  m_AttributeParam.m_LookUpTable[LookUpTableType::Fresnel].m_LutScale = static_cast<float>( atof( fresnelsampler_node->first_attribute( "LutScaleFR" )->value() ) );
					  
          }
				}
			}

			// テクスチャコンバイナの項目の取得
			const xml_node<>*		texturecombiners_node = fragments_node->first_node( "TextureCombiners" );
			if (texturecombiners_node)
			{
				for ( s32 i = 0; i < CombinerAttribute::CombinerMax; ++i)
				{
					char node_name[64] = {0};
					sprintf_s( node_name, "TextureCombiner%d", i );
					const xml_node<>*		texturecombiner_node = texturecombiners_node->first_node( node_name );
					m_AttributeParam.m_TextureCombinerConstant[i] = static_cast<s16>( atoi( texturecombiner_node->first_attribute( "ConstantColorNo" )->value() ) );
					const char*	child_node_name[CombinerColorType::NumberOf] = { "Color", "Alpha" };
					for ( s32 j = 0; j < CombinerColorType::NumberOf; ++j )
					{
						const xml_node<>*		combiner_node = texturecombiner_node->first_node( child_node_name[j] );
						m_AttributeParam.m_TextureCombinerAttribute[j][i].m_MathFormula = GetMathFormulaTypeFromAttributeName(combiner_node->first_attribute( "MathFormula" )->value() );
						m_AttributeParam.m_TextureCombinerAttribute[j][i].m_Scale = static_cast<float>( atof( combiner_node->first_attribute( "Scale" )->value() ) );
						for ( s32 type = 0; type < InputSourceType::NumberOf; ++type )
						{
							char source_attribute[64] = {0};
							char operand_attribute[64] = {0};
							sprintf_s(source_attribute, "Source%c", 'A' + type);
							sprintf_s(operand_attribute, "Operand%c", 'A' + type);
							m_AttributeParam.m_TextureCombinerAttribute[j][i].m_InputSource[type] = GetCombinerInputSourceFromAttributeName( combiner_node->first_attribute( source_attribute )->value() );
							m_AttributeParam.m_TextureCombinerAttribute[j][i].m_Operand[type] = GetCombinerOperandFromAttributeName( combiner_node->first_attribute( operand_attribute )->value() );
						}
						m_AttributeParam.m_TextureCombinerAttribute[j][i].m_OutPutBuffer = ( atoi( combiner_node->first_attribute( "OutputBuffer" )->value() ) != 0 );
					}
				}
			}
		}
	}

	{// ブレンド情報の取得
		xml_node<>*		blendtest_node = top_node->first_node( "BlendTest" );
		if( blendtest_node )
		{
      m_BlendTest.m_DrawLayer = static_cast<scenegraph::resource::DrawLayer>(atoi( blendtest_node->first_attribute( "DrawLayer")->value() ));
			
			const xml_node<>* alphatest_node = blendtest_node->first_node( "AlphaTest" );
			m_BlendTest.m_AlphaTest.m_AlphaTestType = GetAlphaTestTypeFromAttributeName( alphatest_node->first_attribute( "AlphaTestType" )->value() );
			m_BlendTest.m_AlphaTest.m_AlphaTestRefValue = static_cast<float>( atof( alphatest_node->first_attribute( "AlphaTestRefValue" )->value() ) );

			const xml_node<>* depthtest_node = blendtest_node->first_node( "DepthTest" );
			m_BlendTest.m_DepthTest.m_DepthTestType = GetDepthTestTypeFromAttributeName( depthtest_node->first_attribute( "DepthTestType" )->value() );
			m_BlendTest.m_DepthTest.m_UpDateDepth = atoi( depthtest_node->first_attribute( "UpDateDepth" )->value() );
			m_BlendTest.m_DepthTest.m_PolygonOffset = atoi( depthtest_node->first_attribute( "PolygonOffSet" )->value() );

			const xml_node<>* stenciltest_node = blendtest_node->first_node( "StencilTest" );
			m_BlendTest.m_StencilTest.m_StencilTestEnable = static_cast<b32>( atoi( stenciltest_node->first_attribute( "StencilTestRefEnable" )->value() ) );
			m_BlendTest.m_StencilTest.m_StencilTestType = GetStencilTestTypeFromAttributeName( stenciltest_node->first_attribute( "StencilTestType" )->value() );
			m_BlendTest.m_StencilTest.m_StencilValue = static_cast<u8>( atoi( stenciltest_node->first_attribute( "StencilValue" )->value() ) );
			m_BlendTest.m_StencilTest.m_StencilMask = static_cast<u8>( atoi( stenciltest_node->first_attribute( "StencilMask" )->value() ) );
			m_BlendTest.m_StencilTest.m_StencilFailOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilFailOp" )->value() );
			m_BlendTest.m_StencilTest.m_StencilZFailOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilZFailOp" )->value() );
			m_BlendTest.m_StencilTest.m_StencilZpassOp = GetStencilOperationFromAttributeName( stenciltest_node->first_attribute( "StencilZpassOp" )->value() );

			const xml_node<>* blender_node = blendtest_node->first_node( "Blender" );
			m_BlendTest.m_Blender.m_BlendMode = GetBlendModeFromAttributeName( blender_node->first_attribute( "BlendMode" )->value() );
			m_BlendTest.m_Blender.m_LogicalOperation = GetLogicalOperationFromAttributeName( blender_node->first_attribute( "LogicalOperation" )->value() );
			{
				const xml_node<>* color_node = blender_node->first_node( "Color" );
				m_BlendTest.m_Blender.m_Color.m_BlendFunc = GetBlendFuncFromAttributeName( color_node->first_attribute( "BlendFunc" )->value() );
				m_BlendTest.m_Blender.m_Color.m_SrcElement = GetBlendElementFromAttributeName( color_node->first_attribute( "SrcElement" )->value() );
				m_BlendTest.m_Blender.m_Color.m_DstElement = GetBlendElementFromAttributeName( color_node->first_attribute( "DstElement" )->value() );

				const xml_node<>* alpha_node = blender_node->first_node( "Alpha" );
				m_BlendTest.m_Blender.m_Alpha.m_BlendFunc = GetBlendFuncFromAttributeName( alpha_node->first_attribute( "BlendFunc" )->value() );
				m_BlendTest.m_Blender.m_Alpha.m_SrcElement = GetBlendElementFromAttributeName( alpha_node->first_attribute( "SrcElement" )->value() );
				m_BlendTest.m_Blender.m_Alpha.m_DstElement = GetBlendElementFromAttributeName( alpha_node->first_attribute( "DstElement" )->value() );
			}
		}
		else
		{// バージョン0.5以前のモデルデータの対応
      m_BlendTest.m_DrawLayer = scenegraph::resource::DrawLayer::Layer0;

			m_BlendTest.m_AlphaTest.m_AlphaTestType = gfx::CompareFunc::Always;
			m_BlendTest.m_AlphaTest.m_AlphaTestRefValue = 0.0f;

			m_BlendTest.m_DepthTest.m_DepthTestType = gfx::CompareFunc::LessEqual;
			m_BlendTest.m_DepthTest.m_UpDateDepth = true;
			m_BlendTest.m_DepthTest.m_PolygonOffset = 0;

			m_BlendTest.m_StencilTest.m_StencilTestEnable = false;
			m_BlendTest.m_StencilTest.m_StencilTestType = gfx::CompareFunc::Less;
			m_BlendTest.m_StencilTest.m_StencilValue = 0;
			m_BlendTest.m_StencilTest.m_StencilMask = 0;
			m_BlendTest.m_StencilTest.m_StencilFailOp = gfx::StencilOp::Keep;
			m_BlendTest.m_StencilTest.m_StencilZFailOp = gfx::StencilOp::Keep;
			m_BlendTest.m_StencilTest.m_StencilZpassOp = gfx::StencilOp::Keep;

			m_BlendTest.m_Blender.m_BlendMode = renderer::BlendMode::None;
			m_BlendTest.m_Blender.m_LogicalOperation = renderer::LogicalOperation::CLEAR;
			{
				m_BlendTest.m_Blender.m_Color.m_BlendFunc = gfx::BlendOp::Add;
				m_BlendTest.m_Blender.m_Color.m_SrcElement = renderer::BlendElement::One;
				m_BlendTest.m_Blender.m_Color.m_DstElement = renderer::BlendElement::Zero;

				m_BlendTest.m_Blender.m_Alpha.m_BlendFunc = gfx::BlendOp::Add;
				m_BlendTest.m_Blender.m_Alpha.m_SrcElement = renderer::BlendElement::One;
				m_BlendTest.m_Blender.m_Alpha.m_DstElement = renderer::BlendElement::Zero;
			}
		}
	}

	// [UserData]の取得
	{
		xml_node<>*				userdata_node = top_node->first_node( "UserData" );
		if (userdata_node)
		{
			m_UserData.m_EdgeType = static_cast<EdgeType>( atoi( userdata_node->first_attribute( "EdgeType" )->value()) );
			m_UserData.m_IDEdgeEnable = ( atoi( userdata_node->first_attribute( "IDEdgeEnable" )->value()) != 0);
			m_UserData.m_EdgeID = static_cast<u32>( atoi( userdata_node->first_attribute( "EdgeID" )->value()) );
			m_UserData.m_ProjectionType = static_cast<ProjectionType>( atoi( userdata_node->first_attribute( "ProjectionType" )->value()) );
			m_UserData.m_RimPow = static_cast<f32>( atof( userdata_node->first_attribute( "RimPow" )->value()) );
			m_UserData.m_RimScale = static_cast<f32>( atof( userdata_node->first_attribute( "RimScale" )->value()) );
			m_UserData.m_PhongPow = static_cast<f32>( atof( userdata_node->first_attribute( "PhongPow" )->value()) );
			m_UserData.m_PhongScale = static_cast<f32>( atof( userdata_node->first_attribute( "PhongScale" )->value()) );
			m_UserData.m_IDEdgeOffsetEnable = ( atoi( userdata_node->first_attribute( "IDEdgeOffsetEnable" )->value()) != 0);
			// note: テクスチャの名前ではなく数値になっているのでプラグイン側の修正が必要(-1とかしたくない)
			m_UserData.m_EdgeMapAlphaMask = static_cast<CombinerAttribute::UvTextures>( atoi( userdata_node->first_attribute( "EdgeMapAlphaMask" )->value()) - 1 );
			for ( u32 tex = CombinerAttribute::UvTextures::Texture0; tex < CombinerAttribute::UvTextures::NumberOf; ++tex )
			{
				c8 attr_name[64] = {0};
				sprintf_s(attr_name, "BakeTexture%d", tex);
				m_UserData.m_BakeTextures[tex] = static_cast<TextureBakeType>( atoi( userdata_node->first_attribute( attr_name )->value()) );
			}
			for ( u32 tex = LookUpTableType::Distribution0; tex < LookUpTableType::NumberOf; ++tex )
			{
				c8 attr_name[64] = {0};
				sprintf_s(attr_name, "BakeConstant%d", tex);
				m_UserData.m_BakeConstants[tex] = static_cast<TextureBakeType>( atoi( userdata_node->first_attribute( attr_name )->value()) );
			}
			xml_attribute<>* vertexshadertype_attribute = userdata_node->first_attribute( "VertexShaderType" );
			s32 vertexShaderType = static_cast<u32>(atoi( vertexshadertype_attribute->value() ) );
			m_UserData.m_VertexShaderType = static_cast<VertexShaderType::Enum>( vertexShaderType );
			m_UserData.m_ShaderParam0 = static_cast<f32>( atoi( userdata_node->first_attribute( "ShaderParam0" )->value()) );
			m_UserData.m_ShaderParam1 = static_cast<f32>( atoi( userdata_node->first_attribute( "ShaderParam1" )->value()) );
			m_UserData.m_ShaderParam2 = static_cast<f32>( atoi( userdata_node->first_attribute( "ShaderParam2" )->value()) );
			m_UserData.m_ShaderParam3 = static_cast<f32>( atoi( userdata_node->first_attribute( "ShaderParam3" )->value()) );

		}
	}

	// [特殊設定]の取得
	{
		xml_node<>*				specialsetting_node = top_node->first_node( "SpecialSetting" );
		if ( specialsetting_node )
		{
			m_AttributeParam.m_IsPokemonShader = ( atoi( specialsetting_node->first_attribute( "IsPokemonShader" )->value()) != 0 );
			if ( m_UserData.m_VertexShaderType == VertexShaderType::PokeFire )
				m_AttributeParam.m_IsPokemonShader = false;

			m_AttributeParam.m_UseShadowTexture = ( atoi( specialsetting_node->first_attribute( "UseShadowTexture" )->value()) != 0 );
			m_AttributeParam.m_UseObjectSpaceNormalMap = ( atoi( specialsetting_node->first_attribute( "UseObjectSpaceNormalMap" )->value()) != 0 );
			// ハイライトマップ, アウトラインカラーマップ, ブラースケールマップに関してはノードの有無で判断
			xml_attribute<>*				highlighttextureenable_attribute = specialsetting_node->first_attribute( "HighLightTextureEnable" );
			m_AttributeParam.m_HighLightTextureEnable = (highlighttextureenable_attribute != NULL);

/*		if ( 0 )
			{
				xml_attribute<>*				outlinecolortextureenable_attribute = specialsetting_node->first_attribute( "OutLineTextureEnable" );
				m_AttributeParam.m_OutLineColorTextureEnable = (outlinecolortextureenable_attribute != NULL);
				xml_attribute<>*				blurscaletextureenable_attribute = specialsetting_node->first_attribute( "BlurScaleTextureEnable" );
				m_AttributeParam.m_HighLightTextureEnable = (blurscaletextureenable_attribute != NULL);
			} */
			m_AttributeParam.m_PokeNormalType = GetPokeNormalTypeFromAttributeName( specialsetting_node->first_attribute( "PokeNormalType" )->value() );
			m_AttributeParam.m_BlendType =GetBlendTypeFromAttributeName( specialsetting_node->first_attribute( "BlendType" )->value() );
			m_AttributeParam.m_UpdateShaderParam = ( atoi( specialsetting_node->first_attribute( "UpdateShaderParam" )->value()) != 0 );
		} 

		
	}

	{
		xml_node<>*				combiner_node = top_node->first_node( "Combiner" );
		if (combiner_node)
		{
			m_Combiner.m_PresetName = combiner_node->first_attribute( "PresetName" )->value();
			std::wstring			utf16 = ParserUtil::UTF8toUTF16( m_Combiner.m_PresetName );
			m_Combiner.m_PresetName = ParserUtil::UTF16toShitJIS( utf16 );
		}
	}
}

void MaterialData::CreateBinary( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList )
{
	BinaryMaker			binary;

  char						nameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];

	{//マテリアル名出力
		util::FnvHash				hash( reinterpret_cast<const u8*>(m_Name.c_str()) );
		
		GFL_ASSERT( m_Name.length() < Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
		memset( nameBuffer, 0, sizeof(nameBuffer) );		
		memcpy(nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

		binary.Write( &hash.value, sizeof( hash.value ) );
		binary.Write( nameBuffer, sizeof( nameBuffer ) );
	}

	{//シェーダータイプ名出力
		util::FnvHash				hash( reinterpret_cast<const u8*>(m_Type.c_str()) );
		
		GFL_ASSERT( m_Type.length() < Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
		memset( nameBuffer, 0, sizeof(nameBuffer) );		
		memcpy(nameBuffer,m_Type.c_str(),strlen(m_Type.c_str())); // Garbage Data Fix

		binary.Write( &hash.value, sizeof( hash.value ) );
		binary.Write( nameBuffer, sizeof( nameBuffer ) );
	}

	for( u32 i = 0; i < 2; ++i )
	{//シェーダー名出力
		std::string					shaderName = m_Combiner.m_PresetName;
		util::FnvHash				hash( reinterpret_cast<const u8*>(shaderName.c_str()) );
		
		GFL_ASSERT( shaderName.length() < Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
		memset( nameBuffer, 0, sizeof(nameBuffer) );		
		memcpy(nameBuffer,shaderName.c_str(),strlen(shaderName.c_str())); // Garbage Data Fix

		binary.Write( &hash.value, sizeof( hash.value ) );
		binary.Write( nameBuffer, sizeof( nameBuffer ) );
	}

	static const c8* s_pUvUsageName[] = {
		"ColorLayer0",
		"ColorLayer1",
		"ColorLayer2",
		"Normal"
	};

	static const c8*			s_pMappingTypeNames[] = {
			"UvMap",
			"CameraCubeEnvMap",
			"CameraSphereEnvMap",
			"ProjectionMap"
	};

	// Maya上では, WRAP, MIRROR, CLAMP, BORDERの順だが,
	// Runtime上では, WRAP, MIRROR, BORDER, CLAMPなのでそれに合わせる
	static const c8			*pRepeatTypeName[] = {
		"WRAP", "MIRROR", "BORDER", "CLAMP"
	};

	static const c8			*pFilterName[] = {
		"POINT", "LINEAR", "POINT_LINEAR", "LINEAR_LINEAR"
	};

	u32					textureNum = static_cast<u32>( m_TextureList.size() );
	binary.Write( &textureNum, sizeof( textureNum ) );
	
	for( u32 i = 0; i < textureNum; ++i )
	{
		const Texture				&rTexture = m_TextureList[i];

		u32			uvUsageNameNo = 0;
		for( u32 no = 0; no < ARRAY_SIZE(s_pUvUsageName); ++no )
		{
			if ( rTexture.m_UvUsage == s_pUvUsageName[no] )
			{
				uvUsageNameNo = no;
				break;
			}
		}

		u32			mappingTypeNo = 0;
		for( u32 no = 0; no < ARRAY_SIZE(s_pMappingTypeNames); ++no )
		{
			if ( rTexture.m_Attribute.m_MappintType == s_pMappingTypeNames[no] )
			{
				mappingTypeNo = no;
				break;
			}
		}

		u32			repeatUTypeNo = 0;
		u32			repeatVTypeNo = 0;
		for( u32 no = 0; no < ARRAY_SIZE(pRepeatTypeName); ++no )
		{
			if ( rTexture.m_Attribute.m_RepeatU == pRepeatTypeName[no] )
			{
				repeatUTypeNo = no;
			}
			if ( rTexture.m_Attribute.m_RepeatV == pRepeatTypeName[no] )
			{
				repeatVTypeNo = no;
			}
		}

		u32			magFilterTypeNo = 0;
		u32			minFilterTypeNo = 0;
		for( u32 no = 0; no < ARRAY_SIZE(pFilterName); ++no )
		{
			if ( rTexture.m_Attribute.m_MagFilter == pFilterName[no] )	magFilterTypeNo = no;
			if ( rTexture.m_Attribute.m_MinFilter == pFilterName[no] )	minFilterTypeNo = no;
		}

		{
			//POINT_LINEAR -> POINTにLINEAR_LINEAR -> LINEARに変換
			if ( magFilterTypeNo >= 2 )		magFilterTypeNo -= 2;
			if ( minFilterTypeNo >= 2 )		minFilterTypeNo -= 2;
		}

		{
			c8									*pathBuffer = new c8[ rFilePathList[rTexture.m_Usage].length() + 1 ];
			c8									nameBuffer[Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX];

			memset( nameBuffer, 0, sizeof(nameBuffer) );

			ParserUtil::GetFileName( pathBuffer, rFilePathList[rTexture.m_Usage].c_str() );
			GFL_ASSERT( strlen(pathBuffer) < Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
			strcat_s( nameBuffer, pathBuffer );

			util::FnvHash									nameHash( reinterpret_cast<u8*>(nameBuffer) );
			u32														hash = nameHash.value;

			binary.Write( &hash, sizeof( u32 ) );
			binary.Write( nameBuffer, Constant::MATERIAL_ATTRIBUTE_NAME_DATA_MAX );
			
			delete[] pathBuffer;
		}

		binary.Write( &rTexture.m_SlotNo, sizeof( rTexture.m_SlotNo ) );
		binary.Write( &rTexture.m_Usage, sizeof( rTexture.m_Usage ) );
		binary.Write( &uvUsageNameNo, sizeof( uvUsageNameNo ) );

		binary.Write( &rTexture.m_Attribute.m_UvSetNo, sizeof( rTexture.m_Attribute.m_UvSetNo ) );
		binary.Write( &mappingTypeNo, sizeof( mappingTypeNo ) );
		binary.Write( &rTexture.m_Attribute.m_ScaleU, sizeof( rTexture.m_Attribute.m_ScaleU ) );
		binary.Write( &rTexture.m_Attribute.m_ScaleV, sizeof( rTexture.m_Attribute.m_ScaleV ) );
		binary.Write( &rTexture.m_Attribute.m_Rotate, sizeof( rTexture.m_Attribute.m_Rotate ) );
		binary.Write( &rTexture.m_Attribute.m_TranslateU, sizeof( rTexture.m_Attribute.m_TranslateU ) );
		binary.Write( &rTexture.m_Attribute.m_TranslateV, sizeof( rTexture.m_Attribute.m_TranslateV ) );
		binary.Write( &repeatUTypeNo, sizeof( repeatUTypeNo ) );
		binary.Write( &repeatVTypeNo, sizeof( repeatVTypeNo ) );
		binary.Write( &magFilterTypeNo, sizeof( magFilterTypeNo ) );
		binary.Write( &minFilterTypeNo, sizeof( minFilterTypeNo ) );
	}

	{// マテリアル情報を書き込み
		binary.Write( &m_AttributeParam, sizeof( m_AttributeParam ) );
	}

	{// ブレンド情報を書き込み
		binary.Write( &m_BlendTest, sizeof( m_BlendTest ) );
	}

	{// UserData情報を書き込み
		binary.Write( &m_UserData, sizeof( m_UserData ) );
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  JointData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct JointsData
{
	JointsData():m_pBinaryData(NULL){}
	~JointsData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	u8																*m_pBinaryData;
	u32																m_BinarySize;
	std::vector<GfMdlData::JointData>	m_JointDataList;

	void ReadJointDataNode( const xml_node<>* jointsNode );
	void ReadSkeltonDataNode( const xml_node<>* jointsNode );
	void CreateBinary();
};

void JointsData::ReadJointDataNode( const xml_node<>* jointsNode )
{
	if ( jointsNode == NULL )
		return;

	for( const xml_node<>* jointNode = jointsNode->first_node("Joint"); jointNode; jointNode = jointNode->next_sibling("Joint") )
	{
		GfMdlData::JointData			data;
		std::string								str;
		std::wstring							utf16;

		{
			str = jointNode->first_attribute( "Name" )->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			strcpy_s( data.m_Name, str.c_str() );
		}

		xml_attribute<>		*parentNameAttr  = jointNode->first_attribute( "ParentName" );
		if ( parentNameAttr )
		{
			str = parentNameAttr->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			strcpy_s( data.m_ParentName, str.c_str() ); 
		}

		{
			str = jointNode->first_attribute( "NeededRendering" )->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			data.m_NeededRendering = ( str == "TRUE" ) ? true : false;
		}

		const xml_node<>	*localMatrixNode = jointNode->first_node( "LocalMatrix");
		if ( localMatrixNode )
		{
			std::vector<math::Vector4>			row;
			math::Matrix44									matrix;
			ParserUtil::ParseFloatArray( &row, localMatrixNode, 4, 0.0f );
			for( u32 i = 0; i < 4; ++i )
			{
				matrix[i] = row[i];
			}
			data.m_LocalMatrix = matrix;
		}

		const xml_node<>	*globalMatrixNode = jointNode->first_node( "GlobalMatrix");
		if ( globalMatrixNode )
		{
			std::vector<math::Vector4>			row;
			math::Matrix44									matrix;
			ParserUtil::ParseFloatArray( &row, globalMatrixNode, 4, 0.0f );
			for( u32 i = 0; i < 4; ++i )
			{
				matrix[i] = row[i];
			}
			data.m_GlobalMatrix = matrix;
		}

		const xml_node<>	*localScale = jointNode->first_node( "LocalScale");
		if ( localScale )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localScale, 1, 0.0f);

			data.m_LocalScale[0] = row[0].x;
			data.m_LocalScale[1] = row[1].x;
			data.m_LocalScale[2] = row[2].x;
		}

		const xml_node<>	*localRotation = jointNode->first_node( "LocalRotation");
		if ( localRotation )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localRotation, 1, 0.0f);

			data.m_LocalRotation[0] = row[0].x;
			data.m_LocalRotation[1] = row[1].x;
			data.m_LocalRotation[2] = row[2].x;
		}

		const xml_node<>	*localTranslate = jointNode->first_node( "LocalTransform");
		if ( localTranslate )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localTranslate, 1, 0.0f);

			data.m_LocalTranslate[0] = row[0].x;
			data.m_LocalTranslate[1] = row[1].x;
			data.m_LocalTranslate[2] = row[2].x;
		}

		m_JointDataList.push_back( data );
	}
}

void JointsData::ReadSkeltonDataNode( const xml_node<>* skeltonNode )
{
	if ( skeltonNode == NULL )
		return;

	for( const xml_node<>* node = skeltonNode->first_node("Node"); node; node = node->next_sibling("Node") )
	{
		GfMdlData::JointData			data;
		std::string								str;
		std::wstring							utf16;

		{
			str = node->first_attribute( "Name" )->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			strcpy_s( data.m_Name, str.c_str() );
		}

		xml_attribute<>		*parentNameAttr  = node->first_attribute( "ParentName" );
		if ( parentNameAttr )
		{
			str = parentNameAttr->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			strcpy_s( data.m_ParentName, str.c_str() ); 
		}

		{
			str = node->first_attribute( "NeededRendering" )->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			str = ParserUtil::UTF16toShitJIS( utf16 );
			data.m_NeededRendering = ( str == "TRUE" ) ? true : false;
		}

		const xml_node<>	*localMatrixNode = node->first_node( "LocalMatrix");
		if ( localMatrixNode )
		{
			std::vector<math::Vector4>			row;
			math::Matrix44									matrix;
			ParserUtil::ParseFloatArray( &row, localMatrixNode, 4, 0.0f );
			for( u32 i = 0; i < 4; ++i )
			{
				matrix[i] = row[i];
			}
			data.m_LocalMatrix = matrix;
		}

		const xml_node<>	*globalMatrixNode = node->first_node( "GlobalMatrix");
		if ( globalMatrixNode )
		{
			std::vector<math::Vector4>			row;
			math::Matrix44									matrix;
			ParserUtil::ParseFloatArray( &row, globalMatrixNode, 4, 0.0f );
			for( u32 i = 0; i < 4; ++i )
			{
				matrix[i] = row[i];
			}
			data.m_GlobalMatrix = matrix;
		}

		const xml_node<>	*localScale = node->first_node( "LocalScale");
		if ( localScale )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localScale, 1, 0.0f);

			data.m_LocalScale[0] = row[0].x;
			data.m_LocalScale[1] = row[1].x;
			data.m_LocalScale[2] = row[2].x;
		}

		const xml_node<>	*localRotation = node->first_node( "LocalRotation");
		if ( localRotation )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localRotation, 1, 0.0f);

			data.m_LocalRotation[0] = row[0].x;
			data.m_LocalRotation[1] = row[1].x;
			data.m_LocalRotation[2] = row[2].x;
		}

		const xml_node<>	*localTranslate = node->first_node( "LocalTransform");
		if ( localTranslate )
		{
			std::vector<math::Vector4>			row;
      ParserUtil::ParseFloatArray(&row, localTranslate, 1, 0.0f);

			data.m_LocalTranslate[0] = row[0].x;
			data.m_LocalTranslate[1] = row[1].x;
			data.m_LocalTranslate[2] = row[2].x;
		}

		m_JointDataList.push_back( data );
	}
}

void JointsData::CreateBinary()
{
	BinaryMaker			binary;

	struct JointsDataHeader
	{//16バイトアラインサイズ
		s32						size;
		s32						dumy[3];
	};

	JointsDataHeader				header;

	header.size = static_cast<u32>(m_JointDataList.size());
	binary.Write( &header, sizeof(header) );

	for( u32 i = 0; i < m_JointDataList.size(); ++i )
	{
		binary.Write( &m_JointDataList[i], sizeof(GfMdlData::JointData) );
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}


//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  MeshData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct MeshData
{
	MeshData():m_pBinaryData(NULL){}
	~MeshData(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

	enum VertexDataType{
		Position = 0,
		Normal,
		BiNormal,
		Tangent,
		Uv,
		Color,
		JointNo,
		JointWeights,
		NumberOf
	};

	struct typeInfo{
		c8*						pName;
		u32						elements;
	};
	
	static const typeInfo			s_TypeInfos[ VertexDataType::NumberOf ];

	struct VertexData
	{
		std::string											m_Usage;
		std::vector<math::Vector4>				m_DataArray;
	};

	struct FaceData
	{
		struct IndexData
		{
			std::string											m_Usage;
			std::vector<math::Vector4>				m_DataArray;
		};

		std::string												m_Material;
		std::vector<IndexData>						m_IndexDataArray;
	};

	struct VertexBuffer
	{
		std::vector<math::Vector4>				m_Position;
		std::vector<math::Vector4>				m_Normal;
		std::vector<math::Vector4>				m_BiNormal;
		std::vector<math::Vector4>				m_Tangent;
		std::vector<math::Vector4>				m_Uv[8];
		std::vector<math::Vector4>				m_Color[8];
		std::vector<math::Vector4>				m_JointNo;
		std::vector<math::Vector4>				m_JointWeights;

		//! 頂点バッファ作成
		static VertexBuffer CreateVertexBuffer( FaceData &rFaceData, std::vector<VertexData*> pVertexDataList[ VertexDataType::NumberOf ] );
	};

	std::string											m_Path;
	std::string											m_Name;
	math::AABB											m_AABB;
	b32															m_IsBlendShape;
	std::vector<VertexData>					m_VertexDataList;
	std::vector<VertexData*>				m_pVertexDataList[ VertexDataType::NumberOf ];//m_VertexDataListを並べなおした物
	std::vector<FaceData>						m_FaceDataList;
	std::vector<VertexBuffer>				m_FaceVertexBuffer;

	u8*													m_pBinaryData;
	u32													m_BinarySize;

	void ReadMeshDataNode( const xml_node<>* top_node );
	void CreateBinary( const JointsData *pJointsData );
};

const MeshData::typeInfo			MeshData::s_TypeInfos[ MeshData::VertexDataType::NumberOf ] = {
	{ "Position", 4 },
	{ "Normal", 4 },
	{ "BiNormal", 4 },
	{ "Tangent", 4 },
	{ "Uv", 4 },
	{ "Color", 4 },
	{ "JointNo", 4 },
	{ "JointWeight", 4 },
};

void MeshData::ReadMeshDataNode( const xml_node<>* top_node )
{
	m_Path = top_node->first_attribute( "Name" )->value();

	{
		u32									size = static_cast<u32>( m_Path.length() ) + 1;
		c8									*pathBuffer = new c8[ size ];

		memset( pathBuffer, 0, size );
		ParserUtil::GetNodeName( pathBuffer, size, m_Path.c_str() );
    GFL_ASSERT( strlen(pathBuffer) < Constant::MESH_NAME_MAX );

		m_Name = pathBuffer;

		delete[] pathBuffer;
	}

	m_IsBlendShape = false;

	xml_attribute<>		*isBlendShapeAttr  = top_node->first_attribute( "IsBlendShape" );
	if ( isBlendShapeAttr )
	{
		if ( strcmp( isBlendShapeAttr->value(), "TRUE" ) == 0 )
		{
			m_IsBlendShape = true;
		}
	}

	{
		const xml_node<>* aabb_node = top_node->first_node("AABB");
		if ( aabb_node )
		{
			std::vector<math::Vector4>		aabb;
			ParserUtil::ParseFloatArray( &aabb, aabb_node, 3, 0.0f );
			m_AABB.SetMin( aabb[0] );
			m_AABB.SetMax( aabb[1] );
		}
	}

	{
		const xml_node<>* vertexdata_node = top_node->first_node("VertexData");

		for( const xml_node<>* vector_node = vertexdata_node->first_node();vector_node; vector_node = vector_node->next_sibling() )
		{
			VertexData					vertexData;

			std::string					vectorType = vector_node->name();

			vertexData.m_Usage = vector_node->first_attribute( "Usage" )->value();

			if ( vertexData.m_Usage == "BiNormal" )
				continue;//BiNormalはシェーダー内で生成

			if ( vectorType == "Vector2" )
			{
				ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 2, 0.0f );
			}
			else if ( vectorType == "Vector3" )
			{
				ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 3, 1.0f );
			}
			else if ( vectorType == "Vector4" )
			{
				ParserUtil::ParseFloatArray( &vertexData.m_DataArray, vector_node, 4, 0.0f );
			}

			m_VertexDataList.push_back( vertexData );
		}
	}

	{//m_VertexDataListを並べなおす
		u32					vertexDataNum = static_cast<u32>( m_VertexDataList.size() );

		for( u32 i = 0; i < vertexDataNum; ++i )
		{
			const VertexData			&rVertexData = m_VertexDataList[i];

			u32			usageNo = 0;
			for( u32 no = 0; no < ARRAY_SIZE(s_TypeInfos); ++no )
			{
				if ( rVertexData.m_Usage == s_TypeInfos[no].pName )
				{
					usageNo = no;
					break;
				}
			}

			
			

			m_pVertexDataList[ usageNo ].push_back( &m_VertexDataList[i] );
		}
	}

	{
		const xml_node<>* faces_node = top_node->first_node("Faces");

		for( const xml_node<>* face_node = faces_node->first_node("Face");face_node; face_node = face_node->next_sibling("Face") )
		{
			FaceData				faceData;

			faceData.m_Material = face_node->first_attribute( "Material" )->value();

			for( const xml_node<>* index_node = face_node->first_node("int");index_node; index_node = index_node->next_sibling("int") )
			{
				FaceData::IndexData			indexData;
				indexData.m_Usage = index_node->first_attribute( "Usage" )->value();

				ParserUtil::ParseFloatArray( &indexData.m_DataArray, index_node, 3, 0.0f );
				faceData.m_IndexDataArray.push_back( indexData );
			}

			m_FaceDataList.push_back( faceData );
		}
	}

	{
		for( u32 faceNo = 0; faceNo < static_cast<u32>( m_FaceDataList.size() ); ++faceNo )
		{
			FaceData				&rFaceData = m_FaceDataList[faceNo];
			VertexBuffer		vertexBuffer;

			vertexBuffer = VertexBuffer::CreateVertexBuffer( rFaceData, m_pVertexDataList );
			m_FaceVertexBuffer.push_back( vertexBuffer );
		}
	}
}

void MeshData::CreateBinary( const JointsData *pJointsData )
{
	BinaryMaker			binary;

	c8						nameBuffer[Constant::MESH_NAME_MAX];

	{//メッシュ情報出力
		util::FnvHash				hash( reinterpret_cast<const u8*>(m_Name.c_str()) );
		
		GFL_ASSERT( m_Name.length() < Constant::MESH_NAME_MAX );
		memset( nameBuffer, 0, sizeof(nameBuffer) );		
		memcpy(nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

		binary.Write( &hash.value, sizeof( hash.value ) );
		binary.Write( nameBuffer, sizeof( nameBuffer ) );
		binary.Write( &m_IsBlendShape, sizeof( m_IsBlendShape ) );
		binary.Write( &m_AABB, sizeof( m_AABB ) );
	}
	
	{//頂点宣言情報出力
		if ( this->m_IsBlendShape == false )
		{
			static const u32			vertexDataNum = 9;
			binary.Write( &vertexDataNum, sizeof( vertexDataNum ) );

			gfx::StreamUsage			streamUsages[ vertexDataNum ] = 
			{
				gfx::StreamUsage::Position,
				gfx::StreamUsage::Normal,
				gfx::StreamUsage::Tangent,
				gfx::StreamUsage::TexCoord,
				gfx::StreamUsage::TexCoord,
				gfx::StreamUsage::TexCoord,
				gfx::StreamUsage::Color,
				gfx::StreamUsage::BlendIndices,
				gfx::StreamUsage::BlendWeight,
			};

			for ( u32 i = 0; i < vertexDataNum; ++i )
			{
				u32			usageNo = streamUsages[i];
				binary.Write( &usageNo, sizeof( usageNo ) );
			}
		}
		else
		{
			static const u32			vertexDataNum = 1;
			binary.Write( &vertexDataNum, sizeof( vertexDataNum ) );

			u32			usageNo = gfx::StreamUsage::Position;
			binary.Write( &usageNo, sizeof( usageNo ) );
		}
	}

	{//メッシュ出力
		u32					faceDataNum = static_cast<u32>( m_FaceDataList.size() );
		binary.Write( &faceDataNum, sizeof( faceDataNum ) );

		for( u32 faceNo = 0; faceNo < faceDataNum; ++faceNo )
		{
			FaceData				&rFaceData = m_FaceDataList[faceNo];

			{
				util::FnvHash				hash( reinterpret_cast<const u8*>(rFaceData.m_Material.c_str()) );
		
				GFL_ASSERT( rFaceData.m_Material.length() < Constant::MESH_NAME_MAX );
				memset( nameBuffer, 0, sizeof(nameBuffer) );				
				memcpy(nameBuffer,rFaceData.m_Material.c_str() ,strlen(rFaceData.m_Material.c_str() )); // Garbage Data Fix

				binary.Write( &hash.value, sizeof( hash.value ) );
				binary.Write( nameBuffer, sizeof( nameBuffer ) );
			}

			{//ジョイントの並び出力
				u8				jointNum = 0xFF;

				binary.Write( &jointNum, sizeof( jointNum ) );

				for( u32 jointCnt = 0; jointCnt < jointNum; ++jointCnt )
				{
					u8		jointNo = jointCnt;
					binary.Write( &jointNo, sizeof( jointNo ) );
				}
			}

			VertexBuffer			&vertexBuffer = m_FaceVertexBuffer[faceNo];

			if ( this->m_IsBlendShape == false )
			{//現在のシェーダーのフォーマットに合わせつつ、インターリーブ配列化して出力
				u32			vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );

				binary.Write( &vertexSize, sizeof( vertexSize ) );

				for( u32 vno = 0; vno < vertexSize; ++vno )
				{
					math::Vector4				dummy;

					binary.Write( &vertexBuffer.m_Position[vno], sizeof( math::Vector4 ) );

					if ( vertexBuffer.m_Normal.size() )
						binary.Write( &vertexBuffer.m_Normal[vno], sizeof( math::Vector4 ) );
					else
						binary.Write( &dummy, sizeof( math::Vector4 ) );

					if ( vertexBuffer.m_Tangent.size() )
						binary.Write( &vertexBuffer.m_Tangent[vno], sizeof( math::Vector4 ) );
					else
						binary.Write( &dummy, sizeof( math::Vector4 ) );

					for( u32 uvNo = 0; uvNo < 3; ++uvNo )
					{
						if ( vertexBuffer.m_Uv[uvNo].size() )
						{
							math::Vector4			uv = vertexBuffer.m_Uv[uvNo][vno];
							binary.Write( &uv, sizeof( math::Vector4 ) );
						}
						else
							binary.Write( &dummy, sizeof( math::Vector4 ) );
					}

					for( u32 colorNo = 0; colorNo < 1; ++colorNo )
					{
						if ( vertexBuffer.m_Color[colorNo].size() )
							binary.Write( &vertexBuffer.m_Color[colorNo][vno], sizeof( math::Vector4 ) );
						else
							binary.Write( &dummy, sizeof( math::Vector4 ) );
					}

					if ( vertexBuffer.m_JointNo.size() )
						binary.Write( &vertexBuffer.m_JointNo[vno], sizeof( math::Vector4 ) );
					else
						binary.Write( &dummy, sizeof( math::Vector4 ) );

					if ( vertexBuffer.m_JointWeights.size() )
						binary.Write( &vertexBuffer.m_JointWeights[vno], sizeof( math::Vector4 ) );
					else
						binary.Write( &dummy, sizeof( math::Vector4 ) );
				}
			}
			else
			{
				u32			vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );

				binary.Write( &vertexSize, sizeof( vertexSize ) );

				for( u32 vno = 0; vno < vertexSize; ++vno )
				{
					binary.Write( &vertexBuffer.m_Position[vno], sizeof( math::Vector4 ) );
				}
			}

			{
				u32			vertexSize = static_cast<u32>( vertexBuffer.m_Position.size() );

				binary.Write( &vertexSize, sizeof( vertexSize ) );

				for( u32 vno = 0; vno < vertexSize; ++vno )
				{
					binary.Write( &vno, sizeof( u32 ) );//32bitインデックス
				}
			}
		}
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//! 頂点バッファ作成
MeshData::VertexBuffer MeshData::VertexBuffer::CreateVertexBuffer( FaceData &rFaceData, std::vector<VertexData*> pVertexDataList[ MeshData::VertexDataType::NumberOf ] )
{
	static const c8* s_pIndexUsageNames[] = {
		"PositionJointWeightIndex",
		"NormalIndex",
		"TangentBiNormalIndex",
		"UvIndex",
		"ColorIndex"
	};

	VertexBuffer		vertexBuffer;
	u32							indexDataNum = static_cast<u32>( rFaceData.m_IndexDataArray.size() );
	u32							indexLength = static_cast<u32>( rFaceData.m_IndexDataArray[0].m_DataArray.size() );//長さはみな同じ
	u32							uvCount = 0;
	u32							colorCount = 0;

	for( u32 idx = 0; idx < indexDataNum; ++idx )
	{
		FaceData::IndexData			&rIndexData = rFaceData.m_IndexDataArray[idx];

		u32			usageTypeNo = 0;
		for( u32 no = 0; no < ARRAY_SIZE(s_pIndexUsageNames); ++no )
		{
			if ( rIndexData.m_Usage == s_pIndexUsageNames[no] )
			{
				usageTypeNo = no;
				break;
			}
		}

		switch( usageTypeNo ){
		case 0://"PositionJointWeightIndex",
			{
				if ( pVertexDataList[VertexDataType::Position].size() )
				{
					VertexData			*pPosition = pVertexDataList[VertexDataType::Position][0];
					for( u32 no = 0; no < indexLength; ++no )
					{
						for( u32 vpos = 0; vpos < 3; ++vpos )
						{//３頂点ごと
							u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
							vertexBuffer.m_Position.push_back( pPosition->m_DataArray[index] );
						}
					}
				}

				if ( pVertexDataList[VertexDataType::JointNo].size() )
				{//二つはセット
					VertexData			*pJointNo = pVertexDataList[VertexDataType::JointNo][0];
					VertexData			*pJointWeight = pVertexDataList[VertexDataType::JointWeights][0];

					for( u32 no = 0; no < indexLength; ++no )
					{
						for( u32 vpos = 0; vpos < 3; ++vpos )
						{//３頂点ごと
							u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
							vertexBuffer.m_JointNo.push_back(pJointNo->m_DataArray[index]);
							vertexBuffer.m_JointWeights.push_back(pJointWeight->m_DataArray[index]);
						}
					}
				}
			}
			break;
		case 1://"NormalIndex",
			if ( pVertexDataList[VertexDataType::Normal].size() )
			{
				VertexData			*pNormal = pVertexDataList[VertexDataType::Normal][0];

				for( u32 no = 0; no < indexLength; ++no )
				{
					for( u32 vpos = 0; vpos < 3; ++vpos )
					{//３頂点ごと
						u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
						vertexBuffer.m_Normal.push_back(pNormal->m_DataArray[index]);
					}
				}
			}
			break;
		case 2://"TangentBiNormalIndex",
			{
				if ( pVertexDataList[VertexDataType::Tangent].size() )
				{
					VertexData			*pTangent = pVertexDataList[VertexDataType::Tangent][0];
							
					for( u32 no = 0; no < indexLength; ++no )
					{
						for( u32 vpos = 0; vpos < 3; ++vpos )
						{//３頂点ごと
							u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
							vertexBuffer.m_Tangent.push_back(pTangent->m_DataArray[index]);
						}
					}
				}

				if ( pVertexDataList[VertexDataType::BiNormal].size() )
				{
					VertexData			*pBiNormal = pVertexDataList[VertexDataType::BiNormal][0];

					for( u32 no = 0; no < indexLength; ++no )
					{
						for( u32 vpos = 0; vpos < 3; ++vpos )
						{//３頂点ごと
							u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
							vertexBuffer.m_BiNormal.push_back(pBiNormal->m_DataArray[index]);
						}
					}
				}

			}
			break;
		case 3://"UvIndex",
			{
				VertexData			*pUv = pVertexDataList[VertexDataType::Uv][uvCount];

				for( u32 no = 0; no < indexLength; ++no )
				{
					for( u32 vpos = 0; vpos < 3; ++vpos )
					{//３頂点ごと
						u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
						vertexBuffer.m_Uv[uvCount].push_back(pUv->m_DataArray[index]);
					}
				}
				++uvCount;
			}
			break;
		case 4://"ColorIndex"
			{
				VertexData			*pColor = pVertexDataList[VertexDataType::Color][colorCount];

				for( u32 no = 0; no < indexLength; ++no )
				{
					for( u32 vpos = 0; vpos < 3; ++vpos )
					{//３頂点ごと
						u32							index = static_cast<u32>( rIndexData.m_DataArray[no][vpos] );
						vertexBuffer.m_Color[colorCount].push_back(pColor->m_DataArray[index]);
					}
				}
				++colorCount;
			}
			break;
		}
	}

	return vertexBuffer;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfMdlData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfMdlData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:
	Impl() : gfx::GLMemory(), m_VersionId(0) {}
	Impl( c8* pData );
	Impl( c8* pData, std::string fileDir );

	virtual ~Impl();

	bool parseData( c8* pData, std::string fileDir = "" );

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );
  u32 GetVersionId( void );

	ModelData											m_ModelData;
	std::vector<MaterialData>			m_MaterialDataList;
	std::vector<MeshData>					m_MeshDataList;
	std::vector<TextureData>			m_TextureDataList;
	std::vector<ShaderData>				m_ShaderDataList;
	JointsData										m_JointData;
	std::string										m_FileDir;
  u32                           m_VersionId;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfMdlData::Impl::Impl( c8* pData ):rapidxml::xml_document<>()
{
	parseData( pData );
}

GfMdlData::Impl::Impl( c8* pData, std::string fileDir ):rapidxml::xml_document<>()
{
	m_FileDir = fileDir;
	parseData( pData, m_FileDir);
}

bool GfMdlData::Impl::parseData( c8 *pData, std::string fileDir )
{
  util::FnvHash				hash( reinterpret_cast<u8*>(pData) );
  char								cacheFilePath[ParserUtil::GFL_MAX_FILEPATH];	
  memset(cacheFilePath, 0, sizeof(cacheFilePath));
#if defined (DEBUG_DEVICE)
  sprintf_s( cacheFilePath, "%s/ModelCache/%d.cache", fileDir.c_str(), hash.value );	
#else
  sprintf_s( cacheFilePath, "%s/ModelCache/%d.cache.opt", fileDir.c_str(), hash.value );
#endif

  s32 size = 0;
  c8* pCache = ParserUtil::ReadFile( cacheFilePath, &size );
	//c8* pCache = NULL;
  c8* pCacheOriginal = pCache;

  static const u32 sc_cacheVersion14100700 = 0x14100700;      // モデルのバージョンが0.91以前(2014.10.07)
  static const u32 sc_cacheVersion14120500 = 0x14120500;      // モデルのバージョンが0.92(2014.12.05)
  static const u32 sc_cacheVersion15010700 = 0x15010700;      // モデルのバージョンが0.93以降(2015.01.07)
	static const u32 sc_cacheVersion15011500 = 0x15011500;      // モデルのバージョンが0.951以降(2015.01.15)
	static const u32 sc_cacheVersion15020200 = 0x15020200;      // モデルのバージョンが0.96以降(2015.02.02)
	static const u32 sc_cacheVersion15020300 = 0x15020300;      // メッシュのジョイント数分割対応(2015.02.02)
	static const u32 sc_cacheVersion15022500 = 0x15022500;      // メッシュのジョイント数分割対応(2015.02.25)
	static const u32 sc_cacheVersion15030200 = 0x15030200;      // ＶＳ，　ＦＳ分割対応(2015.03.02)

	m_VersionId = GfModelVersion::OptimizeAndAddUserData;

  if (pCache != NULL)
  {
    // キャッシュバージョン
    u32 cacheVersion = 0;
    memcpy(&cacheVersion, pCache, sizeof(cacheVersion));
    pCache += sizeof(cacheVersion);

    // キャッシュバージョンが現行バージョンと合わなければキャッシュ削除して読み込み直し
    if (cacheVersion != sc_cacheVersion15030200)
    {
      delete[] pCacheOriginal;
      DeleteFile(cacheFilePath);
      return this->parseData(pData, fileDir);
    }

    // モデル
    memcpy(&m_ModelData.m_BinarySize, pCache, sizeof(m_ModelData.m_BinarySize));
    pCache += sizeof(m_ModelData.m_BinarySize);

    m_ModelData.m_pBinaryData = new u8[m_ModelData.m_BinarySize];
    memcpy(m_ModelData.m_pBinaryData, pCache, m_ModelData.m_BinarySize);
    pCache += m_ModelData.m_BinarySize;

    // テクスチャ
    {
      u32 texCount = 0;
      memcpy(&texCount, pCache, sizeof(texCount));
      pCache += sizeof(texCount);

      m_TextureDataList.resize(texCount);
      for (u32 i = 0; i < texCount; ++i)
      {
        TextureData& texData = m_TextureDataList[i];
        // ハッシュチェック
        {
          u32 filePathSize = 0;
          memcpy(&filePathSize, pCache, sizeof(filePathSize));
          pCache += sizeof(filePathSize);

          texData.m_FilePath.resize(filePathSize);
          memcpy(&texData.m_FilePath[0], pCache, filePathSize);
          pCache += filePathSize;

          memcpy(&texData.m_Hash, pCache, sizeof(texData.m_Hash));
          pCache += sizeof(texData.m_Hash);


          u8* dat = reinterpret_cast<u8*>(ParserUtil::ReadFile( texData.m_FilePath.c_str(), &size ));
          if (dat == NULL)
          {
            m_TextureDataList.clear();
            m_ModelData.destroy();
            delete[] pCacheOriginal;
            DeleteFile(cacheFilePath);
            return this->parseData(pData, fileDir);
          }
          util::FnvHash hash(dat);
          delete[] dat;

          // キャッシュ削除して読み込み直し
          if (texData.m_Hash.value != hash.value)
          {
            m_TextureDataList.clear();
            m_ModelData.destroy();
            delete[] pCacheOriginal;
            DeleteFile(cacheFilePath);
            return this->parseData(pData, fileDir);
          }
        }
        memcpy(&texData.m_BinarySize, pCache, sizeof(texData.m_BinarySize));
        pCache += sizeof(texData.m_BinarySize);

        texData.m_pBinaryData = new u8[texData.m_BinarySize];
        memcpy(texData.m_pBinaryData, pCache, texData.m_BinarySize);
        pCache += texData.m_BinarySize;
      }
    }

    // シェーダー
    {
      u32 shaderCount = 0;
      memcpy(&shaderCount, pCache, sizeof(shaderCount));
      pCache += sizeof(shaderCount);

      m_ShaderDataList.resize(shaderCount);
      for (u32 i = 0; i < shaderCount; ++i)
      {
        ShaderData& shaderData = m_ShaderDataList[i];
        // ハッシュチェック
        {
          u32 filePathSize = 0;
          memcpy(&filePathSize, pCache, sizeof(filePathSize));
          pCache += sizeof(filePathSize);

          shaderData.m_VsFilePath.resize(filePathSize);
          memcpy(&shaderData.m_VsFilePath[0], pCache, filePathSize);
          pCache += filePathSize;

          memcpy(&shaderData.m_VsHash, pCache, sizeof(shaderData.m_VsHash));
          pCache += sizeof(shaderData.m_VsHash);

          if (shaderData.m_VsHash.value != 0) 
          {
            u8* dat = reinterpret_cast<u8*>(ParserUtil::ReadFile( shaderData.m_VsFilePath.c_str(), &size ));
            if (dat == NULL)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
            util::FnvHash hash(dat);
            delete[] dat;

            // キャッシュ削除して読み込み直し
            if (shaderData.m_VsHash.value != hash.value)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
          }
        }
        // ハッシュチェック
        {
          u32 filePathSize = 0;
          memcpy(&filePathSize, pCache, sizeof(filePathSize));
          pCache += sizeof(filePathSize);

          shaderData.m_FsFilePath.resize(filePathSize);
          memcpy(&shaderData.m_FsFilePath[0], pCache, filePathSize);
          pCache += filePathSize;

          memcpy(&shaderData.m_FsHash, pCache, sizeof(shaderData.m_FsHash));
          pCache += sizeof(shaderData.m_FsHash);

          if (shaderData.m_FsHash.value != 0)
          {
            u8* dat = reinterpret_cast<u8*>(ParserUtil::ReadFile( shaderData.m_FsFilePath.c_str(), &size ));
            if (dat == NULL)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
            util::FnvHash hash(dat);
            delete[] dat;

            // キャッシュ削除して読み込み直し
            if (shaderData.m_FsHash.value != hash.value)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
          }
        }

				// ハッシュチェック
        {
          u32 filePathSize = 0;
          memcpy(&filePathSize, pCache, sizeof(filePathSize));
          pCache += sizeof(filePathSize);

					if ( filePathSize )
					{
						shaderData.m_GsFilePath.resize(filePathSize);
						memcpy(&shaderData.m_GsFilePath[0], pCache, filePathSize);
						pCache += filePathSize;
					}

          memcpy(&shaderData.m_GsHash, pCache, sizeof(shaderData.m_GsHash));
          pCache += sizeof(shaderData.m_GsHash);

          if (shaderData.m_GsHash.value != 0)
          {
            u8* dat = reinterpret_cast<u8*>(ParserUtil::ReadFile( shaderData.m_GsFilePath.c_str(), &size ));
            if (dat == NULL)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
            util::FnvHash hash(dat);
            delete[] dat;

            // キャッシュ削除して読み込み直し
            if (shaderData.m_GsHash.value != hash.value)
            {
              m_TextureDataList.clear();
              m_ShaderDataList.clear();
              m_ModelData.destroy();
              delete[] pCacheOriginal;
              DeleteFile(cacheFilePath);
              return this->parseData(pData, fileDir);
            }
          }
        }
        memcpy(&shaderData.m_BinarySize, pCache, sizeof(shaderData.m_BinarySize));
        pCache += sizeof(shaderData.m_BinarySize);

        shaderData.m_pBinaryData = new u8[shaderData.m_BinarySize];
        memcpy(shaderData.m_pBinaryData, pCache, shaderData.m_BinarySize);
        pCache += shaderData.m_BinarySize;
      }
    }

    // マテリアル
    {
      u32 materialCount = 0;
      memcpy(&materialCount, pCache, sizeof(materialCount));
      pCache += sizeof(materialCount);

      m_MaterialDataList.resize(materialCount);
      for (u32 i = 0; i < materialCount; ++i)
      {
        MaterialData& materialData = m_MaterialDataList[i];
        memcpy(&materialData.m_BinarySize, pCache, sizeof(materialData.m_BinarySize));
        pCache += sizeof(materialData.m_BinarySize);

        materialData.m_pBinaryData = new u8[materialData.m_BinarySize];
        memcpy(materialData.m_pBinaryData, pCache, materialData.m_BinarySize);
        pCache += materialData.m_BinarySize;
      }
    }

    // メッシュ
    {
      u32 meshCount = 0;
      memcpy(&meshCount, pCache, sizeof(meshCount));
      pCache += sizeof(meshCount);

      m_MeshDataList.resize(meshCount);
      for (u32 i = 0; i < meshCount; ++i)
      {
        MeshData& meshData = m_MeshDataList[i];
        memcpy(&meshData.m_BinarySize, pCache, sizeof(meshData.m_BinarySize));
        pCache += sizeof(meshData.m_BinarySize);

        meshData.m_pBinaryData = new u8[meshData.m_BinarySize];
        memcpy(meshData.m_pBinaryData, pCache, meshData.m_BinarySize);
        pCache += meshData.m_BinarySize;
      }
    }

    delete[] pCacheOriginal;
    pCacheOriginal = NULL;
  }
  else
  {
    // キャッシュが無いのでパースする
    this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );

    const xml_node<>* top_node = first_node("GfModel");
    std::string			versionName = top_node->first_attribute("version")->value();

    if (top_node == NULL)
    {
      return false;
    }

  #ifdef PERFORMANCE_COUNTER_ENABLE
    u64				parseStart = Timer::PerformanceCounter::GetMicroSecond();
  #endif
    {
      const xml_node<>	*jointsNode = top_node->first_node( "Joints");//旧フォーマット対応
      const xml_node<>	*skeltonNode = top_node->first_node( "Skelton");

      if ( jointsNode )
      {//旧フォーマット対応
        m_JointData.ReadJointDataNode( jointsNode );		
      }
      else if ( skeltonNode )
      {
        m_JointData.ReadSkeltonDataNode( skeltonNode );		
      }

      m_ModelData.ReadModelDataNode( top_node );
    }

    for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
    {
      std::string				nodeName = node->name();
      if ( nodeName == "TextureList" )
      {
        for( const xml_node<>* texture_node = node->first_node("Texture"); texture_node; texture_node = texture_node->next_sibling("Texture") )
        {
          TextureData				textureData;
          textureData.ReadTextureDataNode( texture_node, fileDir );
          this->m_TextureDataList.push_back( textureData );
        }
      }
    }

    for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
    {
      std::string				nodeName = node->name();
      if ( nodeName == "ShaderList" )
      {
        for( const xml_node<>* shader_node = node->first_node("Shader"); shader_node; shader_node = shader_node->next_sibling("Shader") )
        {
          ShaderData				shaderData;
          shaderData.ReadShaderDataNode( shader_node, fileDir );
          this->m_ShaderDataList.push_back( shaderData );
        }
      }
    }

    {
      for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
      {
        std::string				nodeName = node->name();
        if ( nodeName == "Materials" )
        {
          for( const xml_node<>* material_node = node->first_node("Material"); material_node; material_node = material_node->next_sibling("Material") )
          {
            MaterialData				materialData;
            materialData.ReadMaterialDataNode( material_node );
            this->m_MaterialDataList.push_back( materialData );
          }
        }
      }
    }

    for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
    {
      std::string				nodeName = node->name();
      if ( nodeName == "Meshes" )
      {
        for( const xml_node<>* mesh_node = node->first_node("Mesh"); mesh_node; mesh_node = mesh_node->next_sibling("Mesh") )
        {
          MeshData				meshData;
          meshData.ReadMeshDataNode( mesh_node );
          this->m_MeshDataList.push_back( meshData );
        }
      }
    }

    // キャッシュ作成
    {
      m_JointData.CreateBinary();
      m_ModelData.CreateBinary( m_JointData.m_pBinaryData, m_JointData.m_BinarySize );

      for( u32 i = 0; i < m_TextureDataList.size(); ++i )
        m_TextureDataList[i].CreateBinary();

      for( u32 i = 0; i < m_ShaderDataList.size(); ++i )
        m_ShaderDataList[i].CreateBinary();

      {
        std::vector<std::string>				filePathList;

        for( u32 i = 0; i < m_TextureDataList.size(); ++i )
        {
          filePathList.push_back( m_TextureDataList[i].m_FilePath );
        }

        for( u32 i = 0; i < m_MaterialDataList.size(); ++i )
          m_MaterialDataList[i].CreateBinary( filePathList, m_ShaderDataList );
      }

      for( u32 i = 0; i < m_MeshDataList.size(); ++i )
        m_MeshDataList[i].CreateBinary(&m_JointData);


      BinaryMaker bm;

      // キャッシュバージョン
      u32 cacheVersion = 0;
      cacheVersion = sc_cacheVersion15030200;
      GFL_ASSERT(cacheVersion != 0);
      bm.Write(&cacheVersion, sizeof(cacheVersion));

      // モデル
      bm.Write(&m_ModelData.m_BinarySize, sizeof(m_ModelData.m_BinarySize));
      bm.Write(m_ModelData.m_pBinaryData, m_ModelData.m_BinarySize);

      // テクスチャ
      u32 texCount = static_cast<u32>(m_TextureDataList.size());
      bm.Write(&texCount, sizeof(texCount));
      for (u32 i = 0; i < texCount; ++i)
      {
        TextureData& texData = m_TextureDataList[i];

        {
          // テクスチャパス
          u32 filePathSize = static_cast<u32>(texData.m_FilePath.size());
          bm.Write(&filePathSize, sizeof(filePathSize));
          bm.Write(texData.m_FilePath.c_str(), filePathSize);
          // ここにハッシュ埋め込んでおく
          bm.Write(&texData.m_Hash, sizeof(texData.m_Hash));
        }
        bm.Write(&texData.m_BinarySize, sizeof(texData.m_BinarySize));
        bm.Write(texData.m_pBinaryData, texData.m_BinarySize);
      }

      // シェーダー
      u32 shaderCount = static_cast<u32>(m_ShaderDataList.size());
      bm.Write(&shaderCount, sizeof(shaderCount));
      for (u32 i = 0; i < shaderCount; ++i)
      {
        ShaderData& shaderData = m_ShaderDataList[i];

        {
          // シェーダーパス
          u32 filePathSize = static_cast<u32>(shaderData.m_VsFilePath.size());
          bm.Write(&filePathSize, sizeof(filePathSize));
          bm.Write(shaderData.m_VsFilePath.c_str(), filePathSize);
          // ここにハッシュ埋め込んでおく
          bm.Write(&shaderData.m_VsHash, sizeof(shaderData.m_VsHash));
        }
        {
          // シェーダーパス
          u32 filePathSize = static_cast<u32>(shaderData.m_FsFilePath.size());
          bm.Write(&filePathSize, sizeof(filePathSize));
          bm.Write(shaderData.m_FsFilePath.c_str(), filePathSize);
          // ここにハッシュ埋め込んでおく
          bm.Write(&shaderData.m_FsHash, sizeof(shaderData.m_FsHash));
        }

				{
          // シェーダーパス
          u32 filePathSize = static_cast<u32>(shaderData.m_GsFilePath.size());
          bm.Write(&filePathSize, sizeof(filePathSize));
					if ( filePathSize )
						bm.Write(shaderData.m_GsFilePath.c_str(), filePathSize);
          // ここにハッシュ埋め込んでおく
          bm.Write(&shaderData.m_GsHash, sizeof(shaderData.m_GsHash));
        }

        bm.Write(&shaderData.m_BinarySize, sizeof(shaderData.m_BinarySize));
        bm.Write(shaderData.m_pBinaryData, shaderData.m_BinarySize);
      }

      // マテリアル
      u32 materialCount = static_cast<u32>(m_MaterialDataList.size());
      bm.Write(&materialCount, sizeof(materialCount));
      for (u32 i = 0; i < materialCount; ++i)
      {
        MaterialData& materialData = m_MaterialDataList[i];
        bm.Write(&materialData.m_BinarySize, sizeof(materialData.m_BinarySize));
        bm.Write(materialData.m_pBinaryData, materialData.m_BinarySize);
      }

      // メッシュ
      u32 meshCount = static_cast<u32>(m_MeshDataList.size());
      bm.Write(&meshCount, sizeof(meshCount));
      for (u32 i = 0; i < meshCount; ++i)
      {
        MeshData& meshData = m_MeshDataList[i];
        bm.Write(&meshData.m_BinarySize, sizeof(meshData.m_BinarySize));
        bm.Write(meshData.m_pBinaryData, meshData.m_BinarySize);
      }

      // キャッシュ作成
      u8* buffer = bm.CreateFixedBuffer();
      u32 bufferSize = bm.GetBinarySize();
      ParserUtil::FileCreate( cacheFilePath, reinterpret_cast<char*>(buffer), bufferSize );
      delete[] buffer;
    }
  }

#ifdef PERFORMANCE_COUNTER_ENABLE
	u64				parseEnd = Timer::PerformanceCounter::GetMicroSecond();
#endif


#ifdef PERFORMANCE_COUNTER_ENABLE
	u64				makeEnd = Timer::PerformanceCounter::GetMicroSecond();

	{
		char		buff[32];
		memset(buff, 0, sizeof(buff));
		itoa( static_cast<s32>(parseEnd - parseStart), buff, 10 );
		::OutputDebugStringA( "解析時間　＝　" );
		::OutputDebugStringA( buff );
		::OutputDebugStringA( "\n" );
	}

	{
		char		buff[32];
		memset(buff, 0, sizeof(buff));
		itoa( static_cast<s32>(makeEnd - parseEnd), buff, 10 );
		::OutputDebugStringA( "バイナリ生成時間　＝　" );
		::OutputDebugStringA( buff );
		::OutputDebugStringA( "\n" );
	}
#endif

  return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfMdlData::Impl::~Impl()
{
	for ( u32 i = 0; i < m_TextureDataList.size(); ++i )
	{
		m_TextureDataList[i].destroy();
	}
	
	m_ModelData.destroy();

	for ( u32 i = 0; i < m_MaterialDataList.size(); ++i )
	{
		m_MaterialDataList[i].destroy();
	}

	for ( u32 i = 0; i < m_MeshDataList.size(); ++i )
	{
		m_MeshDataList[i].destroy();
	}

	for ( u32 i = 0; i < m_ShaderDataList.size(); ++i )
	{
		m_ShaderDataList[i].destroy();
	}

	m_JointData.destroy();
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfMdlData::Impl::GetSize( void )
{
	s32					size = 0;
	size += 1;//m_ModelData
	size += static_cast<u32>( m_MaterialDataList.size() );
	size += static_cast<u32>( m_MeshDataList.size() );
	size += static_cast<u32>( m_TextureDataList.size() );
	size += static_cast<u32>( m_ShaderDataList.size() );
	return size;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfMdlData::Impl::GetData( u32 index )
{
	if ( index == 0 )
		return m_ModelData.m_pBinaryData;
	--index;

	if ( index < m_MaterialDataList.size() )
	{
		return m_MaterialDataList[index].m_pBinaryData;
	}
	index -= static_cast<u32>(m_MaterialDataList.size());

	if ( index < m_MeshDataList.size() )
	{
		return m_MeshDataList[index].m_pBinaryData;
	}
	index -= static_cast<u32>(m_MeshDataList.size());

	if ( index < m_TextureDataList.size() )
	{
		return m_TextureDataList[index].m_pBinaryData;
	}
	index -= static_cast<u32>(m_TextureDataList.size());

	if ( index < m_ShaderDataList.size() )
	{
		return m_ShaderDataList[index].m_pBinaryData;
	}
	//index -= static_cast<u32>(m_ShaderDataList.size());

	return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfMdlData::Impl::GetDataSize( u32 index )
{
	if ( index == 0 )
		return m_ModelData.m_BinarySize;
	--index;

	if ( index < m_MaterialDataList.size() )
	{
		return m_MaterialDataList[index].m_BinarySize;
	}
	index -= static_cast<u32>( m_MaterialDataList.size() );

	if ( index < m_MeshDataList.size() )
	{
		return m_MeshDataList[index].m_BinarySize;
	}
	index -= static_cast<u32>( m_MeshDataList.size() );

	if ( index < m_TextureDataList.size() )
	{
		return m_TextureDataList[index].m_BinarySize;
	}
	index -= static_cast<u32>( m_TextureDataList.size() );

	if ( index < m_ShaderDataList.size() )
	{
		return m_ShaderDataList[index].m_BinarySize;
	}
	//index -= static_cast<u32>( m_ShaderDataList.size() );

	return 0;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfMdlData::Impl::GetType( u32 index )
{
	if ( index == 0 )
		return IResourceData::Type("gfmodel");
	--index;

	if ( index < m_MaterialDataList.size() )
	{
		return IResourceData::Type("material");
	}
	index -= static_cast<u32>( m_MaterialDataList.size() );

	if ( index < m_MeshDataList.size() )
	{
		return IResourceData::Type("mesh");
	}
	index -= static_cast<u32>( m_MeshDataList.size() );

	if ( index < m_TextureDataList.size() )
	{
		return IResourceData::Type("texture");
	}
	index -= static_cast<u32>( m_TextureDataList.size() );

	if ( index < m_ShaderDataList.size() )
	{
		return IResourceData::Type("shader");
	}
	index -= static_cast<u32>( m_ShaderDataList.size() );

	return IResourceData::Type("none");
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfMdlData::Impl::GetVersionId( void )
{
  return m_VersionId;
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfMdlData::GfMdlData() : IResourceData()
{
	m_pImpl = new Impl();
}

GfMdlData::GfMdlData( c8* pData ) : IResourceData()
{
	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData );
}

GfMdlData::GfMdlData( std::string filePath )
{
	s32						size;
	c8*						pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData, fileDir );

	delete[] pData;
}

GfMdlData::GfMdlData( std::wstring wstrFilePath )
{
	std::string filePath = UTF16toUTF8(wstrFilePath);
	s32						size;
	c8*						pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData, fileDir );
	delete[] pData;
}

bool GfMdlData::ReadData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }
	return m_pImpl->parseData( pData );
}

bool GfMdlData::ReadFile( std::string filePath )
{
	s32						size;
	c8*						pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

  if (pData == NULL)
  {
    return false;
  }
	bool result = m_pImpl->parseData( pData, fileDir );

	delete[] pData;

  return result;
}

GfMdlData::~GfMdlData()
{
	delete m_pImpl;
}

//! @brief 全データの数の取得
s32 GfMdlData::GetSize( void )
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* GfMdlData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfMdlData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfMdlData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}

//! @brief バージョンIDの取得
u32 GfMdlData::GetVersionId( void )
{
  return m_pImpl->GetVersionId();
}

std::string GfMdlData::UTF16toUTF8( const std::wstring& str )
{
	int len = ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL );

	if ( len > 0 ) { 
		std::vector<char> utf8( len );

		if ( ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL ) ){
			return std::string( &utf8[0] );
		}
	}

	return NULL;
}

}}}}


#endif