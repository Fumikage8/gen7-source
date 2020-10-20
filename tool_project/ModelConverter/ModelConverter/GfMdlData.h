#ifndef GFMDLDATA_H_INCLUDED
#define GFMDLDATA_H_INCLUDED

//後日Windowsアプリ用拡張ライブラリを用意して、そちらで提供いたします。
#if defined(WIN32)

#include <xml/include/rapidxml.hpp> // no need to link

#include <stdio.h>
#include <string>
#include <sstream>
#include <memory.h>
#include <types/include/gfl2_Typedef.h>

#include <math/include/gfl2_MathCommon.h>
#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_IResourceData.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>
#include <math/include/gfl2_MathAABB.h>
#include <util/include/gfl2_FnvHash.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ParserUtil.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_MaterialResourceNode.h>
#include <gfx/include/gfl2_types.h>
#include <RenderingEngine/include/renderer/gfl2_MaterialTypes.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNode.h>

#include "gr_Vertex.h"

using namespace rapidxml;

namespace engine = gfl2::renderingengine;
namespace scenegraph_resource = gfl2::renderingengine::scenegraph::resource;


namespace ModelConverter {

  // バージョンID
  union VersionId
  {
    // 2014.12.08 バージョンIDは
    // X.Y のように X : メジャーバージョン, Y : マイナーバージョン(2桁) で統一
    struct Version
    {
#if defined(GFL2_LITTLE_ENDIAN)
      u16 m_Minor;
      u16 m_Major;
#else
      u16 m_Major;
      u16 m_Minor;
#endif
    };
    u32 m_Value;      // 数値そのものを比較するときに使用
    Version m_Version;

    VersionId()
    {
      m_Version.m_Major = 0;
      m_Version.m_Minor = 0;
    }

    VersionId(u16 major, u16 minor)
    {
      m_Version.m_Major = major;
      m_Version.m_Minor = minor;
    }
  };

  struct JointData//!@todo 適切な場所から参照
  {
    static const u32 JOINT_NAME_MAX = 64;//!@todo 適切な場所から参照

    JointData()
    {
      memset(&m_LocalScale,0,sizeof(m_LocalScale));
      memset(&m_LocalRotation,0,sizeof(m_LocalRotation));
      memset(&m_LocalTranslate,0,sizeof(m_LocalTranslate));

      memset(&m_Status32,0,sizeof(m_Status32));
      memset( m_Name, 0, sizeof(m_Name) );
      memset( m_ParentName, 0, sizeof(m_ParentName) );

      m_Status32 = 0; // Clear Status Completely
      m_BitStatus.m_NeededRendering = false;
      m_BitStatus.m_ScaleSegmentCompensate = false;
      m_BitStatus.m_BillboardType = 0;

      m_LocalMatrix = gfl2::math::Matrix::GetIdentity();
      m_GlobalMatrix = gfl2::math::Matrix::GetIdentity();
    }

    c8 m_Name[JOINT_NAME_MAX];
    c8 m_ParentName[JOINT_NAME_MAX];
    gfl2::math::Matrix m_LocalMatrix;
    gfl2::math::Matrix m_GlobalMatrix;

    // 4 Byte Status:
    union
    {
      struct
      {
        unsigned char m_NeededRendering : 1;
        unsigned char m_ScaleSegmentCompensate : 1;
        unsigned char m_BillboardType : 2;
        unsigned char m_padding : 4;
      } m_BitStatus;
      u8 m_Status[4];
      u32 m_Status32;
    };

    f32 m_LocalScale[3];
    f32 m_LocalRotation[3];
    f32 m_LocalTranslate[3];
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  TextureData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct TextureData
  {
    TextureData():m_No(0), m_BinarySize(0), m_pBinaryData(NULL){}
    ~TextureData(){}

    void destroy()
    {
      if ( m_pBinaryData )
        delete[] m_pBinaryData;
    }

    static const u32 PATH_MAX = 64;//!@todo 適切な場所から参照

    u32 m_No;
    std::string m_FilePath;


    u32 m_BinarySize;
    u8 *m_pBinaryData;
    gfl2::util::FnvHash m_Hash;

    void ReadTextureDataNode( const xml_node<>* top_node, std::string fileDir );
    void CreateBinary();
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  ShaderData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct ShaderData
  {
    ShaderData():m_No(0), m_BinarySize(0), m_pBinaryData(NULL){}
    ~ShaderData(){}

    void destroy()
    {
      if ( m_pBinaryData )
      {
        delete[] m_pBinaryData;
        m_pBinaryData = NULL;
      }
    }

    enum ShaderKind{//!@todo 適切な場所から参照
      Vertex,
      Fragment,
      //Geometry,
      Max
    };

    struct FileHeader//!@todo 適切な場所から参照
    {//16バイトアラインサイズ
      static const u32 SHADER_NAME_MAX = 64;
      c8 shaderName[SHADER_NAME_MAX];
      s32 shaderNameHash;
      s32 dumy[3];
    };

    struct ShaderHeader//!@todo 適切な場所から参照
    {//16バイトアラインサイズ
      static const u32 PATH_MAX = 64;

      s32 size;
      u32 shaderType;
      s32 dumy[2];
      c8 path[PATH_MAX];
    };


    u32 m_No;
    std::string m_ShaderName;
    std::string m_VsFilePath;
    std::string m_FsFilePath;
    std::string m_GsFilePath;

    std::string m_VsCode;
    std::string m_FsCode;
    std::string m_GsCode;


    u32 m_BinarySize;
    u8 *m_pBinaryData;
    gfl2::util::FnvHash m_VsHash;
    gfl2::util::FnvHash m_FsHash;

    void ReadShaderDataNode( const xml_node<>* top_node, std::string fileDir );
    void CreateBinary();
  };


  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  ModelData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct ModelData
  {
    ModelData();

    ~ModelData(){}

    void destroy()
    {
      if ( m_pBinaryData )
        delete[] m_pBinaryData;
    }

    static const u32 PATH_MAX = 64;//!@todo 適切な場所から参照

    struct TextureLinkData{
      TextureLinkData():m_No(0){}
      u32 m_No;
      std::string m_FilePath;
    };

    gfl2::math::AABB m_AABB;
    gfl2::math::Matrix m_TransMat;
    std::vector<std::string> m_ShaderList;
    std::vector<TextureLinkData> m_TextureList;
    std::vector<std::string> m_MaterialList;
    std::vector<std::string> m_MeshPathList;
    std::vector<std::string> m_MeshList;

    //ユーザーデータ
    gfl2::renderingengine::scenegraph::resource::GfMdlResourceNode::DefaultModelUserData m_ModelUserData;
    std::vector<gfl2::math::Vector4> m_KusaPositionList;

    u32 m_BinarySize;
    u8 *m_pBinaryData;

    void ReadModelDataNode( const xml_node<>* top_node );
    void CreateBinary( const u8* pJointData, u32 jointDataSize );
    bool CreateBinaryCTR( const u8* pJointData, u32 jointDataSize );
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  MaterialData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct MaterialData
  {
    static const u32 NAME_DATA_MAX = 64;//!@todo 適切な場所から参照

    MaterialData();
    ~MaterialData(){}

    inline bool  operator ==( const MaterialData& rhs ) const;

    void destroy()
    {
      if ( m_pBinaryData )
        delete[] m_pBinaryData;
    }

    struct Texture
    {
      struct Attribute
      {
        u32 m_UvSetNo;
        std::string m_MappintType;
        u32 m_MappintTypeNo;
        f32 m_ScaleU;
        f32 m_ScaleV;
        f32 m_Rotate;
        f32 m_TranslateU;
        f32 m_TranslateV;
        std::string m_RepeatU;
        u32 m_RepeatUNo;
        std::string m_RepeatV;
        u32 m_RepeatVNo;
        std::string m_MagFilter;
        u32 m_MagFilterNo;
        std::string m_MinFilter;
        u32 m_MinFilterNo;
        f32 m_MipBiasNo;
        std::string m_UvSetName;
        std::vector<std::string> m_UvSetNameList;
      };

      u32 m_SlotNo;
      u32 m_Usage;
      std::string m_UvUsage;
      u32 m_UvUsageNo;
      Attribute m_Attribute;
    };

    struct Combiner
    {
      std::string m_PresetName;
    };

    struct LookUpTableCommand
    {
      std::string m_TextureName;
      u32 m_TextureNameHash;
      u32 m_CommandBuffer[1024];
      u32 m_CommandSize;
    };

    static std::vector<LookUpTableCommand> s_LookUpTableCommandList;

    std::string m_Name;
    std::string m_Type;
    std::vector<Texture> m_TextureList;
    Combiner m_Combiner;
    scenegraph_resource::MaterialResourceNode::BlendTest m_BlendTest;
    scenegraph_resource::MaterialResourceNode::AttributeParam m_AttributeParam;
    scenegraph_resource::MaterialResourceNode::UserData m_UserData;
    bool m_fUvSetAdjust;

    u8* m_pBinaryData;
    u32 m_BinarySize;

    void ReadMaterialDataNode( const xml_node<>* top_node, const VersionId versionId );
    bool CreateBinary( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList, const bool enableLog = false );
    bool CreateBinaryCTR( std::vector<std::string> &rFilePathList, std::vector<ShaderData> &rShaderDataList, const bool enableLog = false );
    gfl2::gfx::CompareFunc GetAlphaTestTypeFromAttributeName( const char* name );
    gfl2::gfx::CompareFunc GetDepthTestTypeFromAttributeName( const char* name );
    gfl2::gfx::CompareFunc GetStencilTestTypeFromAttributeName( const char* name );
    gfl2::gfx::StencilOp GetStencilOperationFromAttributeName( const char* name );
    engine::renderer::BlendMode GetBlendModeFromAttributeName( const char* name );
    gfl2::gfx::BlendOp GetBlendFuncFromAttributeName( const char* name );
    engine::renderer::BlendElement GetBlendElementFromAttributeName( const char* name );
    engine::renderer::LogicalOperation GetLogicalOperationFromAttributeName( const char* name );
    scenegraph_resource::LutInputAngleType GetLutInputAngleTypeFromAttributeName( const char* name );
    gfl2::gfx::CullMode GetCullModeFromAttributeName( const char* name );
    scenegraph_resource::MathFormulaType GetMathFormulaTypeFromAttributeName( const char* name );
    s16 GetCombinerInputSourceFromAttributeName( const char* name );
    s16 GetCombinerOperandFromAttributeName( const char* name );
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  JointData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct JointsData
  {
    JointsData():m_pBinaryData(NULL),m_BinarySize(0){}
    ~JointsData(){}

    void destroy()
    {
      if ( m_pBinaryData )
        delete[] m_pBinaryData;
    }

    static const u32 PATH_MAX = 64;//!@todo 適切な場所から参照

    u8 *m_pBinaryData;
    u32 m_BinarySize;
    std::vector<JointData>	m_JointDataList;

    void ReadJointDataNode( const xml_node<>* jointsNode );
    void ReadSkeltonDataNode( const xml_node<>* jointsNode );
    void CreateBinary();
    void CreateBinaryCTR();

    std::vector<s32> m_JointNoReplaceList;
  };

  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  //! @brief  MeshData構造体
  //■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■
  struct MeshData
  {
    MeshData():
      m_fUvSetAdjust( false ),
      m_pBinaryData(NULL),
      m_IsBlendShape( 0 ),
      m_IsMultiUv( false )
    {

    }
    ~MeshData(){}

    void destroy()
    {
      if ( m_pBinaryData )
        delete[] m_pBinaryData;
    }

    static const u32 NAME_DATA_MAX = 64;//!@todo 適切な場所から参照
    static const u32 UV_SET_MAX = 8;
    static const u32 COLOR_SET_MAX = 8;

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
      c8* pName;
      u32 elements;
    };
    //@todo ハードによってはサイズ最適化
    static const typeInfo s_TypeInfos[ VertexDataType::NumberOf ];

    struct VertexData
    {
      std::string m_Usage;
      std::string m_UvSetName;
      std::vector<gfl2::math::Vector4> m_DataArray;
    };

    struct VertexBuffer
    {
      std::vector<gfl2::math::Vector4> m_Position;
      std::vector<gfl2::math::Vector4> m_Normal;
      std::vector<gfl2::math::Vector4> m_Tangent;
      std::vector<gfl2::math::Vector4> m_Uv[UV_SET_MAX];
      std::vector<gfl2::math::Vector4> m_Color[COLOR_SET_MAX];
      std::vector<gfl2::math::Vector4> m_JointNo;
      std::vector<gfl2::math::Vector4> m_JointWeights;
    };

    // Alignment Optimization(64)
#pragma pack(push, 64)
    struct VertexCacheData
    {
      VertexCacheData():m_HitCount(0){}

      // Order changed for Optimization:
      gfl2::math::Vector4 m_Position;
      gfl2::math::Vector4 m_Normal;
      gfl2::math::Vector4 m_JointNo;
      gfl2::math::Vector4 m_JointWeights;
      gfl2::math::Vector4 m_Tangent;
      gfl2::math::Vector4 m_Uv[MeshData::UV_SET_MAX];
      gfl2::math::Vector4 m_Color[MeshData::COLOR_SET_MAX];

      int m_HitCount;

      inline bool  operator ==( const VertexCacheData& rhs ) const
      {
        register u32* pThis = (u32*)this;
        register u32* pCheck = (u32*)&rhs;

        for( u32 i = 0; i < ( sizeof(VertexCacheData) >>2 ) - 1; ++i )
        {
          if(*pThis != *pCheck) return(false);
          ++pThis;
          ++pCheck;
        }
        return true;
      }

      inline bool  operator !=( const VertexCacheData& rhs ) const
      {
        return !(*this == rhs);
      }
    };
#pragma pack(pop) // END ALIGNMENT(64)


    struct IndexData
    {
      std::string m_Usage;
      std::vector<gfl2::math::Vector4> m_DataArray;
    };

    struct FaceData
    {
      std::string m_MeshName;
      u32         m_FaceIndex;

      std::string m_Material;
      VertexBuffer m_FaceVertexBuffer;
      std::vector<s32> m_FaceIndexBuffer;
      std::vector<u32> m_JointList;//最大DivJointSize関数指定のjointMaxになる

      //! 頂点バッファ作成
      bool CreateVertexBuffer( std::vector<IndexData> m_IndexDataArray, std::vector<VertexData*> pVertexDataList[ VertexDataType::NumberOf ], std::string meshName, const MaterialData &rMaterialData, bool fUvSetAdjust );
      b32 Merge( const FaceData &rSrc );
      void VertexSizeOptimize(const MaterialData* pMaterialData);
      void VertexMerge(const MaterialData* pMaterialData);
      void Optimize( u32 vertexCacheSize );
      u32 JointWeightMax();

      std::vector<u32> GetJointNoList();
      std::vector<FaceData> DivJointSize( u32 jointMax );
      void DataCheck( u32 jointStartRegNo, u32 jointMax );
    };

    bool m_fUvSetAdjust;
    std::string m_Path;
    std::string m_Name;
    gfl2::math::AABB m_AABB;
    b32 m_IsBlendShape;
    b32 m_IsVisibilityAnimation;
    b32 m_IsMultiUv;
    std::vector<VertexData> m_VertexDataList;
    std::vector<VertexData*> m_pVertexDataList[ VertexDataType::NumberOf ];//m_VertexDataListを並べなおした物
    std::vector<FaceData> m_FaceDataList;


    u8* m_pBinaryData;
    u32 m_BinarySize;

    bool ReadMeshDataNode( const xml_node<>* top_node, const JointsData& rJointsData, const std::vector<MaterialData> &rMaterialDataList );
    void CreateBinary( b32 fSizeOpt, const bool enableLog = false );
    nn::gr::CTR::Vertex CreateGrVertex( FaceData& rFaceData );
    bool CreateBinaryCTR( b32 fSizeOpt, const std::vector<MaterialData> &rMaterialDataList, const bool enableLog = false );
    void CalcAABB();
    void DivJointSize( u32 jointStartRegNo, u32 jointMax );
    void Optimize( const std::vector<MaterialData> &rMaterialDataList );

    void DataCheck( u32 jointStartRegNo, u32 jointMax );
    u32 GetVertexCount( void ) const;
    u32 GetPolygonCount( void ) const;
    std::string GetFaceConvertLog( const int level = 0 ) const;
  };

  //-----------------------------------------------------------------------------
  /**
   * @class GfMdlData
   * @brief GfMdlDataクラス
   */
  class GfMdlData : public scenegraph_resource::IResourceData, public rapidxml::xml_document<>
  {
  public:

    GfMdlData();
    GfMdlData( c8* pData ); //! @note 暗黙の型変換に注意！
    GfMdlData( std::string filePath ); //! @note 暗黙の型変換に注意！
    GfMdlData( std::wstring wstrFilePath ); //! @note 暗黙の型変換に注意！

    virtual ~GfMdlData();

    void LogEnable( b32 enable ){ m_LogEnable = enable; }

    //! @brief データからの読み込み
    bool ReadData( c8* pData );

    //! @brief ファイルからの読み込み（相対パス対応）
    bool ReadFile( std::string filePath, std::string textureDir, std::string shaderDir );

    //! @brief バイナリデータ作成
    bool CreateBinary( b32 fSizeOpt, const std::string outputFilePath );

    //! @brief バイナリデータ作成
    bool CreateBinaryCTR( const std::string outputFilePath );

    //! @brief ファイルの書き込み
    bool WriteFile( std::string filePath );

    //! @brief ファイルの書き込み
    bool WriteFileCTR( std::string filePath );

    //! @brief マテリアルマージ
    void MaterialMerge();

    //! @brief 頂点サイズ最適化
    void VertexSizeOptimize();

    //! @brief 頂点マージ
    void VertexMerge();

    //! @brief メッシュマージ
    void MeshMerge();

    //! @brief ジョイント分割
    void MeshDivJointSize( u32 jointStartRegNo, u32 jointMax );

    //! @brief メッシュ最適化
    void MeshOptimize( u32 vertexCacheSize = 32 );//PICA2000で３２頂点
    void MeshOptimizeCTR();

    //! @brief データチェック
    void DataCheck( u32 jointStartRegNo, u32 jointMax );

    //! @brief モデルコンバートログ取得
    std::string GetModelConvertLog( const std::string outputFilePath, const std::string platform ) const;

    //! @brief マテリアルコンバートログ取得
    std::string GetMaterialConvertLog( const int level = 0 ) const;

    //! @brief メッシュコンバートログ取得
    std::string GetMeshConvertLog( const int level = 0 ) const;

    //! @brief 全データの数の取得
    virtual s32 GetSize( void );

    //! @brief データの取得
    virtual const void* GetData( u32 index );

    //! @brief 個々のデータのサイズ取得
    virtual u32 GetDataSize( u32 index );

    //! @brief タイプデータの取得
    virtual Type GetType( u32 index );

    //! @brief ファイルタイプの取得
    virtual Type GetFileType( void )
    {
      return IResourceData::Type("gfmdl");
    }

    //! @brief データを保持しているか？
    virtual bool HasData(void)
    {
      return this->GetData(0) != NULL;
    }

    //! @brief バージョンID取得
    virtual u32 GetVersionId()
    {
      return 0;
    }

    //-----------------------------------------------------------------------------
    /// @brief UTF-16をUTF-8に変換
    //-----------------------------------------------------------------------------
    std::string UTF16toUTF8( const std::wstring& str );

  private:

    bool parseData( c8* pData, std::string textureDir = "", std::string shaderDir = "" );

    ModelData m_ModelData;
    std::vector<MaterialData> m_MaterialDataList;

    static const int MAX_MESH_DATA_LIST = 1024;
    std::vector<MeshData> m_MeshDataList;

    std::vector<TextureData> m_TextureDataList;
    std::vector<ShaderData> m_ShaderDataList;
    JointsData m_JointData;
    std::string m_FileDir;
    VersionId m_VersionId;
    bool m_fUvSetAdjust;

    std::stringstream   m_ConvertLog;
    b32                 m_LogEnable;
  };
}

#endif // #if degined(WIN32)

#endif // #ifndef GFMDLDATA_H_INCLUDED
