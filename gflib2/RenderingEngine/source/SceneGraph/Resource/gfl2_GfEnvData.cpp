#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_DrawEnvResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_LightNode.h>
#include <debug/include/gfl2_Assert.h>
#include <RenderingEngine/include/gfl2_RenderingEngineEnums.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")  

#include <d3d11.h>
#include <d3dcompiler.h>

using namespace rapidxml;

#define					ARRAY_SIZE(array)		(sizeof(array) / sizeof(array[0]))

namespace gfl2 { namespace renderingengine { namespace scenegraph { namespace resource {

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  DataHeader構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct DataHeader
{
	DataHeader()
	{
		nameHash = 0;
		for(int i = 0; i < 3; ++i )		dummy[i] = 0;
		memset( nameBuffer, 0, Constant::FLAT_RESOURCE_NAME_MAX);
	}

	u32						nameHash;
	s32						dummy[3];
	c8						nameBuffer[Constant::FLAT_RESOURCE_NAME_MAX];
};

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  EnvTextureData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct EnvTextureData
{
	EnvTextureData():m_pBinaryData(NULL){}
	~EnvTextureData(){ this->destroy(); }

	void destroy()
	{
		if ( m_pBinaryData )
    {
			delete[] m_pBinaryData;
      m_pBinaryData = NULL;
    }
	}

	u32															m_No;
	::std::string				m_FilePath;


	u32															m_BinarySize;
	u8															*m_pBinaryData;
  util::FnvHash                   m_Hash;

	void ReadTextureDataNode( const xml_node<>* top_node, ::std::string fileDir );
	void CreateBinary();
};

void EnvTextureData::ReadTextureDataNode( const xml_node<>* pEnvNode, ::std::string fileDir )
{
	class LocalFunc{
	public:
		static ::std::string GetRelativePath( ::std::string filePath, ::std::string fileDir )
		{
			::std::string										fileName = ParserUtil::GetFileNameStr( filePath.c_str() );
			ParserUtil::ReplaceStr( filePath, "\\", "/" );
			::std::vector< ::std::string>		dirNames = ParserUtil::SplitStr( filePath, "/" );
			::std::string										textureDirName = "";

			if ( dirNames.size() >= 2 )
			{
				textureDirName = dirNames[ dirNames.size() - 2 ];
				textureDirName += "/";
			}

			filePath = fileDir + textureDirName + fileName;
			return filePath;
		}
	};

	const xml_node<>* pEnvMapTextureNode = pEnvNode->first_node("EnvMapTexture");

	if ( pEnvMapTextureNode )
	{
		::std::string					str;
		::std::wstring				utf16;

		str = pEnvMapTextureNode->first_attribute( "FilePath" )->value();
		utf16 = ParserUtil::UTF8toUTF16( str );
		m_FilePath = ParserUtil::UTF16toShitJIS( utf16 );
	}

	if ( fileDir.length() )
		m_FilePath = LocalFunc::GetRelativePath( m_FilePath, fileDir );
}

void EnvTextureData::CreateBinary()
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
  c8* pImageDataOriginal = pImageData;

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
	}

	binary.Write( &header, sizeof(header) );
	binary.Write( pImageData, header.size );

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
  m_Hash = util::FnvHash(reinterpret_cast<const u8*>(pImageData));

  if (pImageDataOriginal != NULL)
  {
    delete[] pImageDataOriginal;
  }
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  DrawEnv構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct DrawEnv
{
	DrawEnv():m_pBinaryData(NULL){}
	~DrawEnv(){this->destroy();}

	void destroy()
	{
		if ( m_pBinaryData )
    {
			delete[] m_pBinaryData;
      m_pBinaryData = NULL;
    }
	}

	void ReadDrawEnvNode( const xml_node<>* pEnvNode, const ::std::string& version );
	void CreateBinary();

  ::std::string   m_Name;


  // ↓gfl2_DrawEnvResourceNode.hのDrawEnvResourceNode::EnvDataと同じ内容
	::std::string                     m_EnvTextureName;

  math::Vector4                      m_WorldPos;
  math::Vector4                      m_FrontDOF;
  math::Vector4                      m_BackDOF;
	math::AABB						            m_AABB;

  //kawa20150513
  DrawEnvResourceNode::Fog          m_Fog[DrawEnvResourceNode::FOG_NUM];
  DrawEnvResourceNode::GlareFilter  m_GlareFilter;

  int                               m_Priority;
  u32                               m_IsLinkEnv;
  // ↑


	u8						*m_pBinaryData;
	u32						m_BinarySize;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void DrawEnv::ReadDrawEnvNode( const xml_node<>* pEnvNode, const ::std::string& version )
{
  static const ::std::string  VERSION_0_8 = "0.8";

	{
		::std::string					str;
		::std::wstring				utf16;

    if(pEnvNode->first_attribute( "Name" ))
		{
      str = pEnvNode->first_attribute( "Name" )->value();
    }
    else
    {
      str = "DRAWENV1";
    }
		utf16 = ParserUtil::UTF8toUTF16( str );
		m_Name = ParserUtil::UTF16toShitJIS( utf16 );
	}

  if(pEnvNode->first_attribute( "Priority" ))
  {
	  m_Priority = atoi( pEnvNode->first_attribute( "Priority" )->value() );
  }
  else m_Priority = 0;

  const xml_node<>* pEnvMapTextureNode = pEnvNode->first_node("EnvMapTexture");
  
	if ( pEnvMapTextureNode &&  pEnvMapTextureNode->first_attribute( "FilePath" ))
	{
		::std::string					str;
		::std::wstring				utf16;

		str = pEnvMapTextureNode->first_attribute( "FilePath" )->value();
		utf16 = ParserUtil::UTF8toUTF16( str );
		m_EnvTextureName = ParserUtil::UTF16toShitJIS( utf16 );
		m_EnvTextureName = ParserUtil::GetFileNameStr( m_EnvTextureName.c_str() );
	}
  
  if(pEnvNode->first_attribute( "IsLinkEnv" ))
	{
		::std::string					str;
		::std::wstring				utf16;

		str = pEnvNode->first_attribute( "IsLinkEnv" )->value();
		utf16 = ParserUtil::UTF8toUTF16( str );
		str = ParserUtil::UTF16toShitJIS( utf16 );

		m_IsLinkEnv = ( str == "true" ) ? 1 : 0;
	}

  if(pEnvNode->first_node("WorldPos"))
	{
		const xml_node<>* pWorldPosNode = pEnvNode->first_node("WorldPos");
		m_WorldPos.x = static_cast<f32>( atof( pWorldPosNode->first_attribute( "X" )->value() ) );
		m_WorldPos.y = static_cast<f32>( atof( pWorldPosNode->first_attribute( "Y" )->value() ) );
		m_WorldPos.z = static_cast<f32>( atof( pWorldPosNode->first_attribute( "Z" )->value() ) );
	}

  //kawa20150513
  {
    // 初期化
    for(u32 fogNo=0; fogNo<DrawEnvResourceNode::FOG_NUM; ++fogNo)
    {
      m_Fog[fogNo].enable = 0;
    }

    // xmlを解析し、値を取得する
    if( version == VERSION_0_8 )
    {
      if(pEnvNode->first_node("FogLength"))
      {
        const xml_node<>* pFogLengthNode = pEnvNode->first_node("FogLength");
        m_Fog[0].nearLength = static_cast<f32>( atof( pFogLengthNode->first_attribute( "Near" )->value() ) );
        m_Fog[0].farLength = static_cast<f32>( atof( pFogLengthNode->first_attribute( "Far" )->value() ) );
      }
      if(pEnvNode->first_node("FogColor"))
      {
        const xml_node<>* pFogColorNode = pEnvNode->first_node("FogColor");
        m_Fog[0].color.x = static_cast<f32>( atof( pFogColorNode->first_attribute( "R" )->value() ) );
        m_Fog[0].color.y = static_cast<f32>( atof( pFogColorNode->first_attribute( "G" )->value() ) );
        m_Fog[0].color.z = static_cast<f32>( atof( pFogColorNode->first_attribute( "B" )->value() ) );
      }
      {
        m_Fog[0].enable = 1;
        m_Fog[0].strength = 1.0f;
      }
    }
    else
    {
      for( const xml_node<>* pFogNode = pEnvNode->first_node("Fog"); pFogNode; pFogNode = pFogNode->next_sibling("Fog") )
      {
        int fogNo = atoi( pFogNode->first_attribute("No")->value() );
        if( 0<=fogNo && fogNo<DrawEnvResourceNode::FOG_NUM )
        {
          ::std::string  str   = pFogNode->first_attribute( "Enable" )->value();
          ::std::wstring utf16 = ParserUtil::UTF8toUTF16( str );
          str = ParserUtil::UTF16toShitJIS( utf16 );
          m_Fog[fogNo].enable = ( str == "true" ) ? 1 : 0;

          const xml_node<>* pFogLengthNode = pFogNode->first_node("FogLength");
          m_Fog[fogNo].nearLength = static_cast<f32>( atof( pFogLengthNode->first_attribute( "Near" )->value() ) );
          m_Fog[fogNo].farLength  = static_cast<f32>( atof( pFogLengthNode->first_attribute( "Far" )->value() ) );

          const xml_node<>* pFogStrengthNode = pFogNode->first_node("FogStrength");
          m_Fog[fogNo].strength = static_cast<f32>( atof( pFogStrengthNode->first_attribute( "Value" )->value() ) );

          const xml_node<>* pFogColorNode = pFogNode->first_node("FogColor");
          m_Fog[fogNo].color.x = static_cast<f32>( atof( pFogColorNode->first_attribute( "R" )->value() ) );
          m_Fog[fogNo].color.y = static_cast<f32>( atof( pFogColorNode->first_attribute( "G" )->value() ) );
          m_Fog[fogNo].color.z = static_cast<f32>( atof( pFogColorNode->first_attribute( "B" )->value() ) );
        }
      }
    }
  }

  if(pEnvNode->first_node("DepthOfField"))
	{
		const xml_node<>* pDepthOfFieldNode = pEnvNode->first_node("DepthOfField");

		const xml_node<>* pFrontNode = pDepthOfFieldNode->first_node("Front");
		const xml_node<>* pBackNode = pDepthOfFieldNode->first_node("Back");

		m_FrontDOF.x = static_cast<f32>( atof( pFrontNode->first_attribute( "Near" )->value() ) );
		m_FrontDOF.y = static_cast<f32>( atof( pFrontNode->first_attribute( "Far" )->value() ) );
		m_FrontDOF.z = static_cast<f32>( atof( pFrontNode->first_attribute( "Scale" )->value() ) );

		m_BackDOF.x = static_cast<f32>( atof( pBackNode->first_attribute( "Near" )->value() ) );
		m_BackDOF.y = static_cast<f32>( atof( pBackNode->first_attribute( "Far" )->value() ) );
		m_BackDOF.z = static_cast<f32>( atof( pBackNode->first_attribute( "Scale" )->value() ) );
	}

  //kawa20150513
  {
    // 初期化
    m_GlareFilter.enable = 0;

    // xmlを解析し、値を取得する
    if( version == VERSION_0_8 )
    {
      if(pEnvNode->first_node("GlareFilter"))
      {
        const xml_node<>* pGlareFilterNode = pEnvNode->first_node("GlareFilter");

        if(pGlareFilterNode->first_attribute( "Range" ))
          m_GlareFilter.range = static_cast<f32>( atof( pGlareFilterNode->first_attribute( "Range" )->value() ) );

        if(pGlareFilterNode->first_attribute( "Intensity" ))
          m_GlareFilter.intensity = static_cast<f32>( atof( pGlareFilterNode->first_attribute( "Intensity" )->value() ) );
      }      
      {
        m_GlareFilter.enable = 1;
        m_GlareFilter.reduceNum = 2;
        m_GlareFilter.strength = 1.0f;
        m_GlareFilter.weight.x = m_GlareFilter.weight.y = m_GlareFilter.weight.z = 1.0f/3.0f;
      }
    }
    else if(pEnvNode->first_node("GlareFilter"))
    {
      const xml_node<>* pGlareFilterNode = pEnvNode->first_node("GlareFilter");

      if(pGlareFilterNode->first_attribute( "Enable" ))
      {      
        ::std::string  str   = pGlareFilterNode->first_attribute( "Enable" )->value();
        ::std::wstring utf16 = ParserUtil::UTF8toUTF16( str );
        str = ParserUtil::UTF16toShitJIS( utf16 );
        m_GlareFilter.enable = ( str == "true" ) ? 1 : 0;
      }

      if(pGlareFilterNode->first_attribute( "ReduceNum" ))
        m_GlareFilter.reduceNum = static_cast<u16>( atoi( pGlareFilterNode->first_attribute( "ReduceNum" )->value() ) );

      if(pGlareFilterNode->first_attribute( "Strength" ))
        m_GlareFilter.strength  = static_cast<f32>( atof( pGlareFilterNode->first_attribute( "Strength" )->value() ) );

      if(pGlareFilterNode->first_attribute( "Range" ))
        m_GlareFilter.range     = static_cast<f32>( atof( pGlareFilterNode->first_attribute( "Range" )->value() ) );

      if(pGlareFilterNode->first_attribute( "Intensity" ))
        m_GlareFilter.intensity = static_cast<f32>( atof( pGlareFilterNode->first_attribute( "Intensity" )->value() ) );

      if(pGlareFilterNode->first_node("Weight"))
      {      
        const xml_node<>* pGlareFilterWeightNode = pGlareFilterNode->first_node("Weight");
        m_GlareFilter.weight.x = static_cast<f32>( atof( pGlareFilterWeightNode->first_attribute( "R" )->value() ) );
        m_GlareFilter.weight.y = static_cast<f32>( atof( pGlareFilterWeightNode->first_attribute( "G" )->value() ) );
        m_GlareFilter.weight.z = static_cast<f32>( atof( pGlareFilterWeightNode->first_attribute( "B" )->value() ) );
      }
    }

  }

	{
		const xml_node<>* aabb_node = pEnvNode->first_node("AABB");
		if ( aabb_node )
		{
			::std::vector<math::Vector4>		aabb;
			ParserUtil::ParseFloatArray( &aabb, aabb_node, 3, 0.0f );
			m_AABB.SetMin( aabb[0] );
			m_AABB.SetMax( aabb[1] );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void DrawEnv::CreateBinary()
{
	BinaryMaker		binary;
  c8						nameBuffer[Constant::FLAT_RESOURCE_NAME_MAX];

	{
		DataHeader					header;
		util::FnvHash				hash( reinterpret_cast<const u8*>(m_Name.c_str()) );

		header.nameHash = hash.value;
		memcpy(header.nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

		binary.Write( &header, sizeof( header ) );
	}

	memset( nameBuffer, 0, sizeof(nameBuffer) );
	memcpy(nameBuffer,m_EnvTextureName.c_str(),strlen(m_EnvTextureName.c_str())); // Garbage Data Fix

	binary.Write( nameBuffer, sizeof( nameBuffer ) );

	binary.Write( &m_WorldPos, sizeof( m_WorldPos ) );
  binary.Write( &m_FrontDOF, sizeof( m_FrontDOF ) );
  binary.Write( &m_BackDOF, sizeof( m_BackDOF ) );
	binary.Write( &m_AABB, sizeof( m_AABB ) );

  //kawa20150513
  binary.Write( m_Fog, sizeof( m_Fog ) );
  binary.Write( &m_GlareFilter, sizeof( m_GlareFilter ) );

  binary.Write( &m_Priority, sizeof( m_Priority ) );
  binary.Write( &m_IsLinkEnv, sizeof( m_IsLinkEnv ) );

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  LightSet構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct LightSet
{
	struct LightData
	{
		::std::string											m_Name;
		math::Vector4											m_Position;
		math::Vector4											m_Color;
		math::Vector4											m_Direction;
		float															m_ConeAngle;
		LightType		                      m_Type;
		float															m_Intensity;
	};

	LightSet():m_pBinaryData(NULL){}
	~LightSet(){this->destroy();}

	void destroy()
	{
		if ( m_pBinaryData )
    {
			delete[] m_pBinaryData;
      m_pBinaryData = NULL;
    }
	}

	void ReadLightSetNode( const xml_node<>* pLightSetNode );
	void CreateBinary();

	u32												m_SetNo;
	::std::vector<LightData>	m_LightDatas;

	u8						*m_pBinaryData;
	u32						m_BinarySize;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void LightSet::ReadLightSetNode( const xml_node<>* pLightSetNode )
{
	m_SetNo = atoi( pLightSetNode->first_attribute( "No" )->value() );

	const xml_node<>* pLightsNode = pLightSetNode->first_node("Lights");

	for( const xml_node<>* pLightNode = pLightsNode->first_node("Light"); pLightNode; pLightNode = pLightNode->next_sibling("Light") )
	{
		LightData					data;

		{
			::std::string					str;
			::std::wstring				utf16;

			str = pLightNode->first_attribute( "Name" )->value();
			utf16 = ParserUtil::UTF8toUTF16( str );
			data.m_Name = ParserUtil::UTF16toShitJIS( utf16 );
		}

		{
			::std::string					str;
			str = pLightNode->first_attribute( "Type" )->value();

			if ( str == "DirectionalLight" )
				data.m_Type = LightType::Directional;
			else if ( str == "AmbientLight" )
				data.m_Type = LightType::Ambient;
			else if ( str == "PointLight" )
				data.m_Type = LightType::Point;
			else if ( str == "SpotLight" )
				data.m_Type = LightType::Spot;
		}

		{
			const xml_node<>* pLocalPosNode = pLightNode->first_node("LocalPos");
			data.m_Position.x = static_cast<f32>( atof( pLocalPosNode->first_attribute( "X" )->value() ) );
			data.m_Position.y = static_cast<f32>( atof( pLocalPosNode->first_attribute( "Y" )->value() ) );
			data.m_Position.z = static_cast<f32>( atof( pLocalPosNode->first_attribute( "Z" )->value() ) );
		}

		{
			const xml_node<>* pLocalDirectionNode = pLightNode->first_node("LocalDirection");
			data.m_Direction.x = static_cast<f32>( atof( pLocalDirectionNode->first_attribute( "X" )->value() ) );
			data.m_Direction.y = static_cast<f32>( atof( pLocalDirectionNode->first_attribute( "Y" )->value() ) );
			data.m_Direction.z = static_cast<f32>( atof( pLocalDirectionNode->first_attribute( "Z" )->value() ) );
		}

		{
			const xml_node<>* pColorNode = pLightNode->first_node("Color");
			data.m_Color.x = static_cast<f32>( atof( pColorNode->first_attribute( "R" )->value() ) );
			data.m_Color.y = static_cast<f32>( atof( pColorNode->first_attribute( "G" )->value() ) );
			data.m_Color.z = static_cast<f32>( atof( pColorNode->first_attribute( "B" )->value() ) );
		}

		{
			const xml_node<>* pConeAngleNode = pLightNode->first_node("ConeAngle");
			data.m_ConeAngle = static_cast<f32>( atof( pConeAngleNode->first_attribute( "Value" )->value() ) );
		}

		{
			const xml_node<>* pNode = pLightNode->first_node("Intensity");
			data.m_Intensity = static_cast<f32>( atof( pNode->first_attribute( "Value" )->value() ) );
		}

		m_LightDatas.push_back( data );
	}
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void LightSet::CreateBinary()
{
	BinaryMaker					binary;

	binary.Write( &m_SetNo, sizeof( m_SetNo ) );

	{
		u32			lightNum = static_cast<u32>( m_LightDatas.size() );
		binary.Write( &lightNum, sizeof( lightNum ) );
	}

	for( u32 i = 0; i < m_LightDatas.size(); ++i )
	{
		const LightData			&rLightData = m_LightDatas[i];

		{
			c8								nameBuffer[Constant::FLAT_RESOURCE_NAME_MAX];
			util::FnvHash			hash( reinterpret_cast<const u8*>(rLightData.m_Name.c_str()) );
		
			GFL_ASSERT( rLightData.m_Name.length() < Constant::FLAT_RESOURCE_NAME_MAX );
			memset( nameBuffer, 0, sizeof(nameBuffer) );
			memcpy(nameBuffer,rLightData.m_Name.c_str(),strlen(rLightData.m_Name.c_str())); // Garbage Data Fix

			binary.Write( &hash.value, sizeof( hash.value ) );
			binary.Write( nameBuffer, sizeof( nameBuffer ) );
		}

		binary.Write( &rLightData.m_Position, sizeof( rLightData.m_Position ) );
		binary.Write( &rLightData.m_Color, sizeof( rLightData.m_Color ) );
		binary.Write( &rLightData.m_Direction, sizeof( rLightData.m_Direction ) );
		binary.Write( &rLightData.m_ConeAngle, sizeof( rLightData.m_ConeAngle ) );
		binary.Write( &rLightData.m_Type, sizeof( rLightData.m_Type ) );
		binary.Write( &rLightData.m_Intensity, sizeof( rLightData.m_Intensity ) );
	}

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  Camera構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct Camera
{
	Camera():m_pBinaryData(NULL){}
	~Camera(){this->destroy();}

	void destroy()
	{
		if ( m_pBinaryData )
    {
			delete[] m_pBinaryData;
      m_pBinaryData = NULL;
    }
	}

	void ReadCameraNode( const xml_node<>* pCameraNode );
	void CreateBinary();

	::std::string           m_Name;
	math::Matrix44						m_LocalMatrix;
	math::Vector4						m_LocalScale;
	math::Vector4						m_LocalTransform;
	math::Vector4						m_LocalRotation;

  float                   m_Near;
  float                   m_Far;
  float                   m_Fovy;
  float                   m_AspectRatio;

	u8						*m_pBinaryData;
	u32						m_BinarySize;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void Camera::ReadCameraNode( const xml_node<>* pCameraNode )
{
	{
		::std::string					str;
		::std::wstring				utf16;

		str = pCameraNode->first_attribute( "Name" )->value();
		utf16 = ParserUtil::UTF8toUTF16( str );
		m_Name = ParserUtil::UTF16toShitJIS( utf16 );
	}

	{
		const xml_node<>* pNode = pCameraNode->first_node("Param");
		m_Near = static_cast<f32>( atof( pNode->first_attribute( "Near" )->value() ) );
		m_Far = static_cast<f32>( atof( pNode->first_attribute( "Far" )->value() ) );
		m_Fovy = static_cast<f32>( atof( pNode->first_attribute( "Fovy" )->value() ) );
		m_AspectRatio = static_cast<f32>( atof( pNode->first_attribute( "AspectRatio" )->value() ) );
	}

	{
		const xml_node<>* pNode = pCameraNode->first_node("LocalMatrix");

		::std::vector<math::Vector4>		column;
		ParserUtil::ParseFloatArray( &column, pNode, 4, 0.0f );
		m_LocalMatrix[0] = column[0];
		m_LocalMatrix[1] = column[1];
		m_LocalMatrix[2] = column[2];
		m_LocalMatrix[3] = column[3];
	}

	{
		const xml_node<>* pNode = pCameraNode->first_node("LocalScale");

		::std::vector<math::Vector4>		localScale;
		ParserUtil::ParseFloatArray( &localScale, pNode, 3, 0.0f );
		m_LocalScale = localScale[0];
	}

	{
		const xml_node<>* pNode = pCameraNode->first_node("LocalRotation");

		::std::vector<math::Vector4>		localRotation;
		ParserUtil::ParseFloatArray( &localRotation, pNode, 3, 0.0f );
		m_LocalRotation = localRotation[0];
	}

	{
		const xml_node<>* pNode = pCameraNode->first_node("LocalTransform");

		::std::vector<math::Vector4>		localTransform;
		ParserUtil::ParseFloatArray( &localTransform, pNode, 3, 0.0f );
		m_LocalTransform = localTransform[0];
	}
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
void Camera::CreateBinary()
{
	BinaryMaker					binary;

	{
		DataHeader					header;
		util::FnvHash				hash( reinterpret_cast<const u8*>(m_Name.c_str()) );

		header.nameHash = hash.value;
		
		memset(header.nameBuffer,0,sizeof(header.nameBuffer));
		memcpy(header.nameBuffer,m_Name.c_str(),strlen(m_Name.c_str())); // Garbage Data Fix

		binary.Write( &header, sizeof( header ) );
	}

	binary.Write( &m_LocalMatrix, sizeof( m_LocalMatrix ) );
	binary.Write( &m_LocalScale, sizeof( m_LocalScale ) );
	binary.Write( &m_LocalTransform, sizeof( m_LocalTransform ) );
	binary.Write( &m_LocalRotation, sizeof( m_LocalRotation ) );

  binary.Write( &m_Near, sizeof( m_Near ) );
  binary.Write( &m_Far, sizeof( m_Far ) );
  binary.Write( &m_Fovy, sizeof( m_Fovy ) );
  binary.Write( &m_AspectRatio, sizeof( m_AspectRatio ) );

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfEnvData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfEnvData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:
	Impl():gfx::GLMemory(),rapidxml::xml_document<>(),m_EnvTextureDataNum(0){}
  Impl( c8* pData );
	Impl( c8* pData, ::std::string fileDir );

	virtual ~Impl();

	bool parseData( c8* pData, ::std::string fileDir = "" );

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );

	DrawEnv										m_DrawEnv;
  u32                       m_EnvTextureDataNum;  // EnvTextureDataの数(0か1)  //kawa20150430
	EnvTextureData						m_EnvTextureData;
	::std::vector<LightSet>		m_LightSetList;
	::std::vector<Camera>			m_CameraList;
	::std::string							m_FileDir;
};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfEnvData::Impl::Impl( c8* pData ):rapidxml::xml_document<>(),m_EnvTextureDataNum(0)
{
	parseData( pData );
}

GfEnvData::Impl::Impl( c8* pData, ::std::string fileDir ):rapidxml::xml_document<>(),m_EnvTextureDataNum(0)
{
	m_FileDir = fileDir;
	parseData( pData, m_FileDir);
}

bool GfEnvData::Impl::parseData( c8* pData, ::std::string fileDir )
{
	this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );

	const xml_node<>* top_node = first_node("GfEnv");
  if (top_node == NULL)
  {
    return false;
  }
  ::std::string version = top_node->first_attribute( "version" )->value();  //kawa20150513
	const xml_node<>* drawEnvNode = top_node->first_node("DrawEnv");

	{
		m_DrawEnv.ReadDrawEnvNode( drawEnvNode, version );
	}

	{
    const xml_node<>* pEnvMapTextureNode = drawEnvNode->first_node("EnvMapTexture");  //kawa20150430
    if ( pEnvMapTextureNode )
    {
		  m_EnvTextureData.ReadTextureDataNode( drawEnvNode, fileDir );
      m_EnvTextureDataNum = 1;
    }
    else
    {
      m_EnvTextureDataNum = 0;
    }
	}

	{
		const xml_node<>* lightSetsNode = drawEnvNode->first_node("LightSets");
		if ( lightSetsNode )
		{
			for( const xml_node<>* lightSetNode = lightSetsNode->first_node("LightSet"); lightSetNode; lightSetNode = lightSetNode->next_sibling("LightSet") )
			{
				LightSet			set;
				set.ReadLightSetNode( lightSetNode );
				m_LightSetList.push_back(set);
			}
		}
	}

	{
		const xml_node<>* camerasNode = drawEnvNode->first_node("Cameras");
		if ( camerasNode )
		{
			for( const xml_node<>* cameraNode = camerasNode->first_node("Camera"); cameraNode; cameraNode = cameraNode->next_sibling("Camera") )
			{
				Camera			data;
				data.ReadCameraNode( cameraNode );
				m_CameraList.push_back( data );
			}
		}
	}

	m_DrawEnv.CreateBinary();

  if( m_EnvTextureDataNum > 0 )  //kawa20150430
  {
	  m_EnvTextureData.CreateBinary();
  }

	for( u32 i = 0; i < m_LightSetList.size(); ++i )
	{
		m_LightSetList[i].CreateBinary();
	}

	for( u32 i = 0; i < m_CameraList.size(); ++i )
	{
		m_CameraList[i].CreateBinary();
	}

  return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfEnvData::Impl::~Impl()
{
	m_DrawEnv.destroy();

  if( m_EnvTextureDataNum > 0 )  //kawa20150430
  {
	  m_EnvTextureData.destroy();
  }

	for ( u32 i = 0; i < m_LightSetList.size(); ++i )
	{
		m_LightSetList[i].destroy();
	}

	for ( u32 i = 0; i < m_CameraList.size(); ++i )
	{
		m_CameraList[i].destroy();
	}

}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfEnvData::Impl::GetSize( void )
{
	s32					size = 0;
	size += 1 + m_EnvTextureDataNum;  // m_DrawEnv + m_EnvTextureData  //kawa20150430
	size += static_cast<u32>( m_LightSetList.size() );
	size += static_cast<u32>( m_CameraList.size() );
	return size;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfEnvData::Impl::GetData( u32 index )
{
	if ( index == 0 )
		return m_DrawEnv.m_pBinaryData;
	--index;

	if ( index < m_EnvTextureDataNum )  //kawa20150430
  {
		return m_EnvTextureData.m_pBinaryData;
  }
	index -= m_EnvTextureDataNum;

	if ( index < m_LightSetList.size() )
	{
		return m_LightSetList[index].m_pBinaryData;
	}
	index -= static_cast<u32>(m_LightSetList.size());

	if ( index < m_CameraList.size() )
	{
		return m_CameraList[index].m_pBinaryData;
	}
	index -= static_cast<u32>(m_CameraList.size());

	return NULL;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfEnvData::Impl::GetDataSize( u32 index )
{
	if ( index == 0 )
		return m_DrawEnv.m_BinarySize;
	--index;

	if ( index < m_EnvTextureDataNum )  //kawa20150430
  {
		return m_EnvTextureData.m_BinarySize;
  }
	index -= m_EnvTextureDataNum;

	if ( index < m_LightSetList.size() )
	{
		return m_LightSetList[index].m_BinarySize;
	}
	index -= static_cast<u32>( m_LightSetList.size() );

	if ( index < m_CameraList.size() )
	{
		return m_CameraList[index].m_BinarySize;
	}
	index -= static_cast<u32>( m_CameraList.size() );

	return 0;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfEnvData::Impl::GetType( u32 index )
{
	if ( index == 0 )
		return IResourceData::Type("gfenv");
	--index;

	if ( index < m_EnvTextureDataNum )  //kawa20150430
  {
		return IResourceData::Type("texture");
  }
	index -= m_EnvTextureDataNum;

	if ( index < m_LightSetList.size() )
	{
		return IResourceData::Type("lightset");
	}
	index -= static_cast<u32>( m_LightSetList.size() );

	if ( index < m_CameraList.size() )
	{
		return IResourceData::Type("camera");
	}
	index -= static_cast<u32>( m_CameraList.size() );

	return IResourceData::Type("none");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfEnvDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfEnvData::GfEnvData() : IResourceData()
{
	m_pImpl = new Impl();
}

GfEnvData::GfEnvData( c8* pData ) : IResourceData()
{
	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData );
}

GfEnvData::GfEnvData( ::std::string filePath )
{
	s32						  size;
	c8*						  pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	::std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData, fileDir );

	delete[] pData;
}

GfEnvData::GfEnvData( ::std::wstring wstrFilePath )
{
	::std::string   filePath = UTF16toUTF8(wstrFilePath);
	s32						  size;
	c8*						  pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	::std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

	GFL_ASSERT(pData);
	m_pImpl = new Impl( pData, fileDir );
	delete[] pData;
}

GfEnvData::~GfEnvData()
{
	delete m_pImpl;
	m_pImpl = NULL;
}

//----------------------------------------------------------------------------
/**
//! @brief データからの読み込み
*/
//-----------------------------------------------------------------------------
bool GfEnvData::ReadData( c8* pData )
{
  if (pData == NULL)
  {
    return false;
  }

	return m_pImpl->parseData( pData, "" );
}

//----------------------------------------------------------------------------
/**
//! @brief ファイルからの読み込み（相対パス対応）
*/
//-----------------------------------------------------------------------------
bool GfEnvData::ReadFile( ::std::string filePath )
{
	s32						  size;
	c8*						  pData = ParserUtil::ReadFile( filePath.c_str(), &size );
	::std::string		fileDir = ParserUtil::GetFilePathStr( filePath.c_str() );

  if (pData  == NULL)
  {
    return false;
  }
	bool result = m_pImpl->parseData( pData, fileDir );

	delete[] pData;

  return result;
}

//! @brief 全データの数の取得
s32 GfEnvData::GetSize( void )
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* GfEnvData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfEnvData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfEnvData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}

::std::string GfEnvData::UTF16toUTF8( const ::std::wstring& str )
{
	int len = ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL );

	if ( len > 0 ) { 
		::std::vector<char> utf8( len );

		if ( ::WideCharToMultiByte( CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL ) ){
			return ::std::string( &utf8[0] );
		}
	}

	return NULL;
}

//! @brief EnvTextureDataの数(0か1)の取得  //kawa20150430
u32 GfEnvData::GetEnvTextureDataNum(void) const
{
  return m_pImpl->m_EnvTextureDataNum;
}
//! @brief LightSetの数の取得
u32 GfEnvData::GetLightSetNum(void) const
{
  return m_pImpl->m_LightSetList.size();
}
//! @brief Cameraの数の取得
u32 GfEnvData::GetCameraNum(void) const
{
  return m_pImpl->m_CameraList.size();
}

}}}}


#endif