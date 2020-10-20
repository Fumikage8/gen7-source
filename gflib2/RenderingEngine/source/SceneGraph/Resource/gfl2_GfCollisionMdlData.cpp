#if defined(GF_PLATFORM_WIN32)//Windowsでしか提供するつもりなし

//なのでSTL使っちゃう！わーい。けど、このcpp内にとどめる。
#include <vector>
#include <string>
#include <xml/include/rapidxml.hpp> // no need to link

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_CollisionMeshResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>

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

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  CollisionMeshData構造体
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
struct CollisionMeshData
{
  CollisionMeshData(){}
	~CollisionMeshData(){}

	enum VertexDataType
  {
		Position = 0,

		NumberOf
	};

	struct typeInfo
  {
		c8*						pName;
		u32						elements;
	};
	//@note ハードによってはサイズ最適化
	static const typeInfo			s_TypeInfos[ VertexDataType::NumberOf ];

  struct Triangle
  {
    math::Vector4 m_Positions[3];
    math::Vector4 m_Normal;
    s32          m_GroundAttribute;
#if TEST_NIJI
    u8           m_VertexColorG[3];
#endif
  };

  std::vector<Triangle>       m_TriangleList;

	void ReadCollisionMeshDataNode( const xml_node<>* top_node, const char *xml_filename = "", b32 isSkelton = false );
};

const CollisionMeshData::typeInfo			CollisionMeshData::s_TypeInfos[ CollisionMeshData::VertexDataType::NumberOf ] = {
	{ "Position", 4 },
};

void CollisionMeshData::ReadCollisionMeshDataNode( const xml_node<>* top_node, const char *xml_filename,  b32 isSkelton )
{
  std::vector<math::Vector4> positionList;

#if TEST_NIJI
  std::vector<math::Vector4> colorList;
#endif

  // 頂点データ取得
  {
    const xml_node<>* vertexdata_node = top_node->first_node("VertexData");

    for( const xml_node<>* vector_node = vertexdata_node->first_node();vector_node; vector_node = vector_node->next_sibling() )
    {
      std::string usage = vector_node->first_attribute("Usage")->value();

      if (usage == "Position")
      {
        ParserUtil::ParseFloatArray( &positionList, vector_node, 3, 1.0f );
      }

#if TEST_NIJI
      if (usage == "Color")
      {
        ParserUtil::ParseFloatArray( &colorList, vector_node, 4, 1.0f );
      }
#endif

    }
  }

  // インデックスデータ取得
  {
    const xml_node<>* faces_node = top_node->first_node("Faces");

    for( const xml_node<>* face_node = faces_node->first_node("Face");face_node; face_node = face_node->next_sibling("Face") )
    {
      // face単位でインデックスリストを保持する
      std::vector<math::Vector4> positionIndexList;

#if TEST_NIJI
      std::vector<math::Vector4> colorIndexList;
#endif

      // マテリアル名から地面属性を設定
      s32 groundAttribute = -1;

#if TEST_NIJI

      // スケルトンの仕様にあわせた例外コード、メッシュ名とデータを紐付けする @attention スケルトン廃止時にコード削除する 
      if( isSkelton )
      {
        // Mesh名から取得
        c8* pTargetMeshName = top_node->first_attribute("Name")->value();
        
          std::string meshName( pTargetMeshName );

          // gfmdlのメッシュ名の変更により修正
          // gfmdl version 2.1以前 <Mesh Name="|camera|camera_01|camera_01Shape" ～
          // ↓
          // gfmdl version 2.1以降 <Mesh Name="|camera|camera_01" ～
          // gfmdl version 2.1以前のモデル用の処理
          std::string::size_type t = meshName.rfind( "Shape" );
          if( t != std::string::npos)
          {
            t = meshName.find_last_of( "|" );
            if( t != std::string::npos )
            {
              meshName.erase( t );
            }
          }

          t = meshName.find_last_of( "|" );
          if( t != std::string::npos )
          {
            meshName.erase( 0, t + 1 );
          }

        if( pTargetMeshName == NULL )
        {
          // メッシュ名が設定されていない
          groundAttribute = -1;
        }
        else if( xml_filename == NULL || std::strcmp( xml_filename, "" ) == 0 )
        {
          // 参照テーブルがないのでアトリビュートを設定しない
          groundAttribute = -1;
        }
        else
        {
          s32 size;
          c8 *pData = ParserUtil::ReadFile( xml_filename, &size );
          if( pData != NULL )
          {
            s32 meshNameLen = strlen( pTargetMeshName );
            rapidxml::xml_document<> attr_xml;
            attr_xml.parse< parse_no_utf8 >( pData );

            const xml_node<> *top_node = attr_xml.first_node( "ROOT" );
            if( top_node )
            {
              s32 count = 0;
              for( xml_node<> *attr_node = top_node->first_node(); attr_node; attr_node = attr_node->next_sibling() )
              {
                // 末尾が一致すればOK
                if( std::strcmp( attr_node->name(), meshName.c_str() ) == 0 )
                {
                  groundAttribute = count;
                }

                ++ count;
              }
            }

            delete[] pData;
          }
        }
      }
      else
      {
        c8* pTargetMaterialName = face_node->first_attribute("Material")->value();
        if( pTargetMaterialName == NULL )
        {
          // マテリアル名が設定されていない
          groundAttribute = -1;
        }
        else if( xml_filename == NULL || std::strcmp( xml_filename, "" ) == 0 )
        {
          // 参照テーブルがないのでアトリビュートを設定しない
          groundAttribute = -1;
        }
        else
        {
          s32 size;
          c8 *pData = ParserUtil::ReadFile( xml_filename, &size );
          if( pData != NULL )
          {
            rapidxml::xml_document<> attr_xml;
            attr_xml.parse< parse_no_utf8 >( pData );

            const xml_node<> *top_node = attr_xml.first_node( "ROOT" );
            if( top_node )
            {
              s32 count = 0;
              for( xml_node<> *attr_node = top_node->first_node(); attr_node; attr_node = attr_node->next_sibling() )
              {
                if( std::strcmp( attr_node->name(), pTargetMaterialName ) == 0 )
                {
                  groundAttribute = count;
                }

                ++ count;
              }
            }

            delete[] pData;
          }
        }
      }
#endif

      for( const xml_node<>* index_node = face_node->first_node("int");index_node; index_node = index_node->next_sibling("int") )
      {
        std::string usage = index_node->first_attribute("Usage")->value();

        if (usage == "PositionJointWeightIndex")
        {
          ParserUtil::ParseFloatArray( &positionIndexList, index_node, 3, 0.0f );
        }
#if TEST_NIJI
        if (usage == "ColorIndex")
        {
          ParserUtil::ParseFloatArray( &colorIndexList, index_node, 3, 0.0f );
        }
#endif
      }

      // ポリゴンを展開
      for (u32 i = 0; i < positionIndexList.size(); ++i)
      {
        Triangle triangle;

#if TEST_NIJI
        if( colorIndexList.size() > 0 )
        {
          math::Vector4 colorIndexes = colorIndexList[i];
          u32 colorIndex0 = static_cast<u32>(colorIndexes.x);
          u32 colorIndex1 = static_cast<u32>(colorIndexes.y);
          u32 colorIndex2 = static_cast<u32>(colorIndexes.z);

          // 頂点カラーG( HDRからLDRへ変換 )
          triangle.m_VertexColorG[0]  = (u32)( colorList[colorIndex0].y * 255.0f );
          triangle.m_VertexColorG[1]  = (u32)( colorList[colorIndex1].y * 255.0f );
          triangle.m_VertexColorG[2]  = (u32)( colorList[colorIndex2].y * 255.0f );
        }
        else
        {
          // 頂点カラーG( dammy )
          triangle.m_VertexColorG[0]  = 0;
          triangle.m_VertexColorG[1]  = 0;
          triangle.m_VertexColorG[2]  = 0;
        }
#endif

        math::Vector4 positionIndexes = positionIndexList[i];
        u32 positionIndex0 = static_cast<u32>(positionIndexes.x);
        u32 positionIndex1 = static_cast<u32>(positionIndexes.y);
        u32 positionIndex2 = static_cast<u32>(positionIndexes.z);

        // 頂点
        triangle.m_Positions[0]     = positionList[positionIndex0];
        triangle.m_Positions[1]     = positionList[positionIndex1];
        triangle.m_Positions[2]     = positionList[positionIndex2];

        // 地面属性
        triangle.m_GroundAttribute  = groundAttribute;

        // 法線
        math::Vector4 v0 = triangle.m_Positions[1] - triangle.m_Positions[0];
        math::Vector4 v1 = triangle.m_Positions[2] - triangle.m_Positions[0];
        triangle.m_Normal = v0.Cross(v1).Normalize3();

        // ポリゴンの状況(頂点が真横に並んでいる等)によっては非数がくるケースがある
        if( _finite( triangle.m_Normal.x ) == 0 
         || _finite( triangle.m_Normal.y ) == 0 
         || _finite( triangle.m_Normal.z ) == 0 )
        {
          continue;
        }
        m_TriangleList.push_back(triangle);
      }
		}
	}
}

struct CollisionMeshDataMerged
{
	CollisionMeshDataMerged():m_pBinaryData(NULL){}
	~CollisionMeshDataMerged(){}

	void destroy()
	{
		if ( m_pBinaryData )
			delete[] m_pBinaryData;
	}

  std::vector<CollisionMeshData::Triangle>       m_TriangleList;

  void Merge(std::vector<CollisionMeshData>& meshDataList);
  void CreateBinary();

  u8* m_pBinaryData;
  u32 m_BinarySize;
};

void CollisionMeshDataMerged::Merge(std::vector<CollisionMeshData>& meshDataList)
{
  // マージする
  for (u32 i = 0; i < meshDataList.size(); ++i)
  {
    CollisionMeshData& meshData = meshDataList[i];
    m_TriangleList.insert(m_TriangleList.end(), meshData.m_TriangleList.begin(), meshData.m_TriangleList.end());
  }
}

void CollisionMeshDataMerged::CreateBinary()
{
	BinaryMaker			binary;

  // ヘッダー
  u32 triangleCount = static_cast<u32>(m_TriangleList.size());
  binary.Write(&triangleCount, sizeof(triangleCount));

  for (u32 i = 0; i < m_TriangleList.size(); ++i)
  {
    CollisionMeshData::Triangle& triangle = m_TriangleList[i];
    binary.Write(&triangle, sizeof(triangle));
  }

	m_pBinaryData = binary.CreateFixedBuffer();
	m_BinarySize = binary.GetBinarySize();
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfCollisionMdlData::Implクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
class GfCollisionMdlData::Impl : public gfx::GLMemory, public rapidxml::xml_document<>
{
public:
	Impl(){}
	Impl( c8* pData, ::std::string outputFilePath , ::std::string attributeFilePath );

	virtual ~Impl();

	bool parseData( c8* pData, ::std::string outputFilePath , ::std::string attributeFilePath, b32 isSkelton = false );

	s32 GetSize( void );
	const void* GetData( u32 index );
	u32 GetDataSize( u32 index );
	IResourceData::Type GetType( u32 index );

	CollisionMeshDataMerged m_CollisionMeshData;

};

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfCollisionMdlData::Impl::Impl( c8* pData, ::std::string outputFilePath, ::std::string attributeFilePath ):gfx::GLMemory(),rapidxml::xml_document<>()
{
  parseData( pData, outputFilePath , attributeFilePath );
}

bool GfCollisionMdlData::Impl::parseData( c8 *pData, ::std::string outputFilePath, ::std::string attributeFilePath, b32 isSkelton )
{
  
  static const u32 sc_cacheVersion = 0x14120500;
  
  // @note コリジョンコンバーターが実行された際は必ず生成を行いたいので、キャッシュを無効化した 150605 ikuta_junya
#if 0
  s32 size = 0;    
  c8* pCache = ParserUtil::ReadFile( outputFilePath.c_str(), &size );
  c8* pCacheOriginal = pCache;

  if (pCache != NULL)
  {
    // キャッシュがあるのでそちらを使う

    // キャッシュバージョン
    u32 cacheVersion = 0;
    memcpy(&cacheVersion, pCache, sizeof(cacheVersion));
    pCache += sizeof(cacheVersion);
    size -= sizeof(cacheVersion);

    // キャッシュバージョンが合わなければキャッシュ削除して読み込み直し
    if (cacheVersion != sc_cacheVersion)
    {
      delete[] pCacheOriginal;
      DeleteFile(outputFilePath.c_str());
      return this->parseData(pData,outputFilePath,attributeFilePath);
    }

    // メッシュ
    m_CollisionMeshData.m_pBinaryData = new u8[size];
    memcpy(m_CollisionMeshData.m_pBinaryData, pCache, size);
    m_CollisionMeshData.m_BinarySize = size;
    delete[] pCacheOriginal;
  }
  else
#endif
  {
    // キャッシュが無いのでパースする
    this->parse< parse_no_utf8 | parse_no_entity_translation >( pData );

    const xml_node<>* top_node = first_node("GfModel");
    if (top_node == NULL)
    {
      return false;
    }

    std::vector<CollisionMeshData> meshDataList;
    for( const xml_node<>* node = top_node->first_node(); node; node = node->next_sibling( ) )
    {
      std::string				nodeName = node->name();
      if ( nodeName == "Meshes" )
      {
        for( const xml_node<>* mesh_node = node->first_node("Mesh"); mesh_node; mesh_node = mesh_node->next_sibling("Mesh") )
        {
          CollisionMeshData				CollisionMeshData;
          CollisionMeshData.ReadCollisionMeshDataNode( mesh_node , attributeFilePath.data(), isSkelton );

          meshDataList.push_back(CollisionMeshData);
        }
      }
    }
    m_CollisionMeshData.Merge(meshDataList);

    // キャッシュ作成
    {
      m_CollisionMeshData.CreateBinary();


      BinaryMaker bm;

      // キャッシュバージョン
      bm.Write(&sc_cacheVersion, sizeof(sc_cacheVersion));

      // メッシュ
      bm.Write(m_CollisionMeshData.m_pBinaryData, m_CollisionMeshData.m_BinarySize);

      // キャッシュ作成
      u8* buffer = bm.CreateFixedBuffer();
      u32 bufferSize = bm.GetBinarySize();
      ParserUtil::FileCreate( outputFilePath.c_str(), reinterpret_cast<char*>(buffer), bufferSize );

      delete[] buffer;
    }
  }

  return true;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
GfCollisionMdlData::Impl::~Impl()
{
  m_CollisionMeshData.destroy();
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
s32 GfCollisionMdlData::Impl::GetSize( void )
{
	return 1;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
const void* GfCollisionMdlData::Impl::GetData( u32 index )
{
	return m_CollisionMeshData.m_pBinaryData;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
u32 GfCollisionMdlData::Impl::GetDataSize( u32 index )
{
	return m_CollisionMeshData.m_BinarySize;
}

//----------------------------------------------------------------------------
/**
 *   @brief 
*/
//-----------------------------------------------------------------------------
IResourceData::Type GfCollisionMdlData::Impl::GetType( u32 index )
{
  return IResourceData::Type("collmesh");
}

//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
//! @brief  GfCollisionMdlDataクラス
//■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
GfCollisionMdlData::GfCollisionMdlData() : IResourceData()
{
	m_pImpl = new Impl();
}

bool GfCollisionMdlData::ReadData( c8* pData , ::std::string outputFilePath, ::std::string attributeFilePath )
{
  if (pData == NULL)
  {
    return false;
  }

	return m_pImpl->parseData( pData , outputFilePath , attributeFilePath );
}

bool GfCollisionMdlData::ReadFileForSkelton( ::std::string inputFilePath , ::std::string outputFilePath, ::std::string attributeFilePath )
{
  s32						size;
  c8*						pData   = ParserUtil::ReadFile( inputFilePath.c_str(), &size );
  if (pData == NULL)
  {
    return false;
  }

  bool result = m_pImpl->parseData( pData,outputFilePath,attributeFilePath, true );
  delete[] pData;

  return result;
}

bool GfCollisionMdlData::ReadFile( ::std::string inputFilePath, ::std::string outputFilePath, ::std::string attributeFilePath )
{
	s32						size;
	c8*						pData   = ParserUtil::ReadFile( inputFilePath.c_str(), &size );
  if (pData == NULL)
  {
    return false;
  }

	bool result = m_pImpl->parseData( pData,outputFilePath,attributeFilePath );
	delete[] pData;

  return result;
}

GfCollisionMdlData::~GfCollisionMdlData()
{
	delete m_pImpl;
}

//! @brief 全データの数の取得
s32 GfCollisionMdlData::GetSize( void )
{
	return m_pImpl->GetSize();
}

//! @brief データの取得
const void* GfCollisionMdlData::GetData( u32 index )
{
	return m_pImpl->GetData(index);
}

//! @brief 個々のデータのサイズ取得
u32 GfCollisionMdlData::GetDataSize( u32 index )
{
	return m_pImpl->GetDataSize(index);
}

//! @brief タイプデータの取得
IResourceData::Type GfCollisionMdlData::GetType( u32 index )
{
	return m_pImpl->GetType(index);
}

}}}}


#endif