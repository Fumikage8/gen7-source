/**
* @brief  フィールドの地形管理を担うクラス
* @file   FieldTerrain.cpp
* @author ikuta_junya
* @data   2015.02.07
*/


// niji
#include "arc_def_index/arc_def.h"
#include "arc_index/TerrainModel.gaix"           // @todo 消す
#include "arc_index/TerrainMiddleModel.gaix"      // @todo 消す
#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h" // @todo field用に置き換える 150215 ikuta_junya

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>



// FieldTest
#include "test/FieldTest/include/Terrain/FieldTerrain.h"
#include "test/FieldTest/include/Terrain/FieldTerrainBlock.h"
#include "test/FieldTest/include/Terrain/FieldTerrainDistantView.h"
#include "test/FieldTest/include/FieldTestUtility.h"
#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );


s32 highModelDatIdTable[ 6 ][ 6 ] = 
{
  -1, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, -1, 
  -1, -1, GARC_TerrainModel_map00_11_08_GFBMDL, GARC_TerrainModel_map00_12_08_GFBMDL, -1, -1, 
  -1, -1, GARC_TerrainModel_map00_11_09_GFBMDL, GARC_TerrainModel_map00_12_09_GFBMDL, -1, -1, 
  -1, -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, -1, 
};


s32 middleModelDatIdTable[ 6 ][ 6 ] = 
{
  GARC_TerrainMiddleModel_map00_09_06_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_06_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_06_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_06_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_06_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_06_middle_GFBMDL, 
  GARC_TerrainMiddleModel_map00_09_07_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_07_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_07_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_07_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_07_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_07_middle_GFBMDL, 
  GARC_TerrainMiddleModel_map00_09_08_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_08_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_08_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_08_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_08_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_08_middle_GFBMDL, 
  GARC_TerrainMiddleModel_map00_09_09_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_09_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_09_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_09_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_09_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_09_middle_GFBMDL, 
  GARC_TerrainMiddleModel_map00_09_10_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_10_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_10_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_10_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_10_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_10_middle_GFBMDL, 
  GARC_TerrainMiddleModel_map00_09_11_middle_GFBMDL, GARC_TerrainMiddleModel_map00_10_11_middle_GFBMDL, GARC_TerrainMiddleModel_map00_11_11_middle_GFBMDL, GARC_TerrainMiddleModel_map00_12_11_middle_GFBMDL, GARC_TerrainMiddleModel_map00_13_11_middle_GFBMDL, GARC_TerrainMiddleModel_map00_14_11_middle_GFBMDL, 
};

s32 blockWidthCount = 6;
s32 blockHightCount = 6;
f32 blockWidth = 3200.0f;
f32 blockHight = 3200.0f;
gfl2::math::Vector2 blockStartPosition( blockWidth * -3.0f, blockHight * -3.0f );

s32 distantViewBlockAreaTable[][4] =
{
  // xmin xmax zmin zmax
  {-11,-1,-5,-1},    // low_a
  {-9,-4,-5,-1},    // low_b
  {-3,-1,-6,-5},      // low_c
  {0,4,-6,-2},      // low_d
  {5,6,-6,-2},      // low_e
  {-11,-10,0,5},      // low_f
  {-9,-4,0,5},      // low_g
  {-3,-1,2,5},      // low_h
  {0,4,2,5},      // low_i
  {5,6,2,5},      // low_j
  {3,6,-1,1},      // low_k
};


/**
* @brief アーカイブファイルの中身全てを読み込む
*
* @param arcId  アーカイブID
*
* @return バイナリ生成結果
*/
void** LoadBinary( gfl2::heap::HeapBase* pSystemHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, u32 arcId, u32* pSize )
{
  // test
  //void** binaryTable( NULL ); // WIN32
  void** binaryTable = NULL;
  u32 size( 0 );

  gfl2::fs::Result fsResult;
  fsResult.Clear();

  const gfl2::fs::ArcFile* pArcFile = pAsyncFileManager->GetArcFile( arcId );
  pArcFile->GetDataCount( &size );

  binaryTable = GFL_NEW( pSystemHeap ) void* [ size ];

  for( u32 i = 0; i < size; i++ )
  {
    std::size_t buffsize;
    std::size_t realSize;
    pArcFile->GetDataSize( &buffsize, i, NULL );

    char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Arc[%d] Binary[%d]",  arcId, i );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    binaryTable[ i ] = GflHeapAllocMemoryAlign( pDeviceHeap , buffsize, 128 );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

    gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
    req.arcId         = arcId;
    req.datId         = i; 
    req.pBuf          = binaryTable[ i ];
    req.bufSize       = buffsize;
    req.pRealReadSize = &realSize;
    req.result        = &fsResult;

    pAsyncFileManager->SyncArcFileLoadDataBuf( req );

    if( fsResult.IsSuccess() == false)
    {
      fsResult.Print();
      GFL_ASSERT_STOP_MSG( 0, "!! ERROR !! arcId=%d datId=%d \n", req.arcId, req.datId );
    }
  }

  *pSize = size;
  return binaryTable;
}


struct MATRIX_DATA
{
  u32   blockWidthCount;
  u32   blockHeightCount;
  s32*  pHighModelDatTable;
  s32*  pMiddleModelDatTable;
};



/**
* @brief コンストラクタ
*
* @return 無し
*/ 
FieldTerrain::FieldTerrain( void ) :

  // インスタンス
  m_pModelInstanceNode( NULL ),

  // 地形ブロックの実体
  m_pFieldTerrainBlockTable( NULL ),
  m_BlockCountWidth( 0 ),
  m_BlockCountHeight( 0 ),
  m_HighBlockStartX( -1 ),
  m_HighBlockStartZ( -1 ),

  // 遠景
  m_pDistantViewTable( NULL ),

  // バイナリ
  m_ppTextureBinaryTable( NULL ),
  m_ppShaderBinaryTable( NULL ),
  m_ppModelBinaryTable( NULL ),
  m_TextureDataCount( 0 ),
  m_ShaderDataCount( 0 ),
  m_ModelDataCount( 0 ),

  // リソース
  m_ppTextureResourceNodeTable( NULL ),
  m_ppShaderResourceNodeTable( NULL ),
  m_pModelResourceRootNode( NULL ),
  m_ppModelResourceRootNodeTable( NULL ),

  m_pTextureResourceRootNode( NULL ),
  m_pShaderResourceRootNode( NULL )

{

}

/**
* @brief デストラクタ
*
* @return 無し
*/
FieldTerrain::~FieldTerrain( void )
{
  Finalize();
}

/**
* @brief 初期化処理
*
* @param  pSystemHeap システムヒープ
* @param  pDeviceHeap システムヒープ
* @param  pAsyncFileManager garc読み込みクラス
*
* @return 無し
*/
void FieldTerrain::Initialize( gfl2::heap::HeapBase* pSystemHeap, gfl2::heap::HeapBase* pDeviceHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager, Test::FieldTest::MyRenderingPipeLine* pRenderingPipeLine )
{
#if 0
  MATRIX_DATA* pMatrixData = GFL_NEW( pSystemHeap ) MATRIX_DATA;

  pMatrixData->blockHeightCount = 5;
  pMatrixData->blockWidthCount = 5;

  pMatrixData->pHighModelDatTable = GFL_NEW( pSystemHeap ) s32 [ pMatrixData->blockHeightCount * pMatrixData->blockWidthCount ];
  pMatrixData->pMiddleModelDatTable = GFL_NEW( pSystemHeap ) s32 [ pMatrixData->blockHeightCount * pMatrixData->blockWidthCount ];
#endif

  // @todo とりあえず同期読み込み

  // テクスチャ
  {
    // テクスチャバイナリ読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_TERRAIN_TEXTURE;
      openReq.heapForFile  = pSystemHeap;
      openReq.heapForReq   = pSystemHeap;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_TERRAIN_TEXTURE;
      closeReq.heapForReq  = pSystemHeap;

      pAsyncFileManager->SyncArcFileOpen( openReq );
      m_ppTextureBinaryTable = LoadBinary( pSystemHeap, pDeviceHeap, pAsyncFileManager, ARCID_TERRAIN_TEXTURE, &m_TextureDataCount );
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // テクスチャリソースを作成
    {
      m_pTextureResourceRootNode = FieldResourceNode::CreateNode();
      gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pTextureResourceRootNode );

      m_ppTextureResourceNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_TextureDataCount ];
      for( u32 i = 0; i < m_TextureDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;

        texResourceData.SetTextureData( static_cast<c8*>( m_ppTextureBinaryTable[i] ) );
        // m_ppTextureResourceNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain Texture[%d]", i  );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pTextureResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData ) );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

  }

  // シェーダー
  {
    // シェーダーバイナリ読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_TERRAIN_SHADER;
      openReq.heapForFile  = pSystemHeap;
      openReq.heapForReq   = pSystemHeap;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_TERRAIN_SHADER;
      closeReq.heapForReq  = pSystemHeap;

      pAsyncFileManager->SyncArcFileOpen( openReq );
      m_ppShaderBinaryTable = LoadBinary( pSystemHeap, pDeviceHeap, pAsyncFileManager, ARCID_TERRAIN_SHADER, &m_ShaderDataCount );
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // シェーダーリソースを作成
    {
      m_pShaderResourceRootNode = FieldResourceNode::CreateNode();
      m_ppShaderResourceNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_ShaderDataCount ];
      for( u32 i = 0; i < m_ShaderDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
        resourceData.SetShaderData( static_cast<c8*>( m_ppShaderBinaryTable[i] ) );
        // gfl2::renderingengine::scenegraph::resource::ResourceNode* pWork = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        // m_ppShaderResourceNodeTable[i] = pWork;

        // m_pShaderResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData ) );
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain Shader [%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_pTextureResourceRootNode->AddChild( gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData ) );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

  }

  // モデル
  {
    // モデルバイナリを読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_TERRAIN_MODEL;
      openReq.heapForFile  = pSystemHeap;
      openReq.heapForReq   = pSystemHeap;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_TERRAIN_MODEL;
      closeReq.heapForReq  = pSystemHeap;

      pAsyncFileManager->SyncArcFileOpen( openReq );

      m_ppModelBinaryTable  = LoadBinary( pSystemHeap, pDeviceHeap, pAsyncFileManager, ARCID_TERRAIN_MODEL, &m_ModelDataCount ); 

      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // モデルリソースを作成
#if 0
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
      resourceData.SetModelData( static_cast<c8*>( m_ppModelBinaryTable[0] ) );
      m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
    }
#endif

    {
      m_ppModelResourceRootNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_ModelDataCount ];
      for( u32 i = 0; i < m_ModelDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
        resourceData.SetModelData( static_cast<c8*>( m_ppModelBinaryTable[i] ) );
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain ModelResource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_ppModelResourceRootNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

  }

  //
  // 遠景モデル
  //
  {
    // モデルバイナリを読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_TERRAIN_DISTANT_VIEW_MODEL;
      openReq.heapForFile  = pSystemHeap;
      openReq.heapForReq   = pSystemHeap;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_TERRAIN_DISTANT_VIEW_MODEL;
      closeReq.heapForReq  = pSystemHeap;

      pAsyncFileManager->SyncArcFileOpen( openReq );

      m_ppDistantViewModelBinaryTable = LoadBinary( pSystemHeap, pDeviceHeap, pAsyncFileManager, ARCID_TERRAIN_DISTANT_VIEW_MODEL, &m_DistantViewModelDataCount ); 

      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // モデルリソースを作成
    {
      m_ppDistantViewModelResourceRootNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_DistantViewModelDataCount ];
      for( u32 i = 0; i < m_DistantViewModelDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
        resourceData.SetModelData( static_cast<c8*>( m_ppDistantViewModelBinaryTable[i] ) );

        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain DistantViewModelResource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_ppDistantViewModelResourceRootNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

    // インスタンスを生成
    {
      m_ppDistantViewModelInstanceNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* [ m_DistantViewModelDataCount ];
      for( u32 i = 0; i < m_DistantViewModelDataCount; i++ )
      {
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain DistantViewModel[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_ppDistantViewModelInstanceNodeTable[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
          m_ppDistantViewModelResourceRootNodeTable[i],
          m_pTextureResourceRootNode
          );
        m_ppDistantViewModelInstanceNodeTable[i]->SetNodeName( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

        pRenderingPipeLine->AddEdgeRenderingTarget( m_ppDistantViewModelInstanceNodeTable[i] );
      }
    }
  }


  //
  // ミドルモデル
  //
  {
    // モデルバイナリを読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_TERRAIN_MIDDLE_MODEL;
      openReq.heapForFile  = pSystemHeap;
      openReq.heapForReq   = pSystemHeap;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_TERRAIN_MIDDLE_MODEL;
      closeReq.heapForReq  = pSystemHeap;

      pAsyncFileManager->SyncArcFileOpen( openReq );

      m_ppMiddleModelBinaryTable = LoadBinary( pSystemHeap, pDeviceHeap, pAsyncFileManager, ARCID_TERRAIN_MIDDLE_MODEL, &m_MiddleModelDataCount ); 

      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // モデルリソースを作成
    {
      m_ppMiddleModelResourceRootNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_MiddleModelDataCount ];
      for( u32 i = 0; i < m_MiddleModelDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
        resourceData.SetModelData( static_cast<c8*>( m_ppMiddleModelBinaryTable[i] ) );
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain MiddleModelResource[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_ppMiddleModelResourceRootNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
      }
    }

    // インスタンスを生成
    {
      m_ppMiddleModelInstanceNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* [ m_MiddleModelDataCount ];
      for( u32 i = 0; i < m_MiddleModelDataCount; i++ )
      {
        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain MiddleModel[%d]",  i );
        int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
        m_ppMiddleModelInstanceNodeTable[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
          m_ppMiddleModelResourceRootNodeTable[i],
          m_pTextureResourceRootNode
          );
        m_ppMiddleModelInstanceNodeTable[i]->SetNodeName( strbuff );
        FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

        pRenderingPipeLine->AddEdgeRenderingTarget( m_ppMiddleModelInstanceNodeTable[i] );
        m_ppMiddleModelInstanceNodeTable[i]->SetVisible( false );
      }
    }
  }

  // インスタンス生成
#if 0
  {
    m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
      m_pModelResourceRootNode,
      m_pTextureResourceRootNode
      );

    pRenderingPipeLine->AddEdgeRenderingTarget( m_pModelInstanceNode );
  }
#endif
  {
    m_ppModelInstanceNodeTable = GFL_NEW( pSystemHeap ) gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* [ m_ModelDataCount ];
    for( u32 i = 0; i < m_ModelDataCount; i++ )
    {
      char strbuff[128] = {0};
      Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain Model[%d]",  i );
      int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
      m_ppModelInstanceNodeTable[i] = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(
        m_ppModelResourceRootNodeTable[i],
        m_pTextureResourceRootNode
        );
      m_ppModelInstanceNodeTable[i]->SetNodeName(strbuff);
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);

      pRenderingPipeLine->AddEdgeRenderingTarget( m_ppModelInstanceNodeTable[i] );
      m_ppModelInstanceNodeTable[i]->SetVisible( false );
    }
  }

  // 配置
  m_BlockCountHeight = blockHightCount;
  m_BlockCountWidth = blockWidthCount;
  m_pFieldTerrainBlockTable = GFL_NEW( pSystemHeap ) FieldTerrainBlock [ blockWidthCount * blockHightCount ];
  FieldTerrainBlock* pWork = m_pFieldTerrainBlockTable;
  for( u32 h = 0; h < blockHightCount; h++ )
  {
    for( u32 w = 0; w < blockWidthCount; w++ )
    {
      // middle
      {
        s32 datId = middleModelDatIdTable[h][w];

        if( datId != -1 )
        {
          pWork->SetModel( FieldTerrainBlock::LOD_MIDDLE, m_ppMiddleModelInstanceNodeTable[ datId ] );
          
          gfl2::math::AABB aabb = m_ppMiddleModelInstanceNodeTable[ datId ]->GetAABB();
          gfl2::math::Vector min = aabb.GetMin();
          gfl2::math::Vector max = aabb.GetMax();
          GFL_RELEASE_PRINT( "Trrain Model Info [%d][%d]\n", h, w );
          GFL_RELEASE_PRINT( "AABB min x=%f y=%f z=%f\n", min.x, min.y, min.z );
          GFL_RELEASE_PRINT( "AABB max x=%f y=%f z=%f\n", max.x, max.y, max.z );
        }
      }
      
      // highモデル登録
      {
        s32 datId = highModelDatIdTable[h][w];

        if( datId != -1 )
        {
          pWork->SetModel( FieldTerrainBlock::LOD_HIGHT, m_ppModelInstanceNodeTable[ datId ] );
        }
      }


      pWork->ChangeLod( FieldTerrainBlock::LOD_MIDDLE );
      pWork++;
    }
  }

  // 遠景
  {
    m_pDistantViewTable = GFL_NEW( pSystemHeap ) FieldTerrainDistantView[ m_DistantViewModelDataCount ];
    for( u32 i = 0; i < m_DistantViewModelDataCount; i++ )
    {
      m_pDistantViewTable[i].SetModel( m_ppDistantViewModelInstanceNodeTable[i] );
    }
  }

  // コリジョンモデル
  {
    // バイナリを読み込み
    {

#if defined( GF_PLATFORM_CTR)
      m_pCollisionModelBinary = (c8*)Test::FieldTest::Utility::ReadData( L"rom:/data/test/field_test/map00/map00_height.gfbcol", 128 );
#else
      m_pCollisionModelBinary = (c8*)Test::FieldTest::Utility::ReadData( L"R:/home/niji/exec/data_win32/data/test/field_test/map00/map00_height.gfbcol", 128 );
#endif // GF_PLATFORM_CTR

    }

    // リソース生成
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( static_cast< c8* >( m_pCollisionModelBinary ), 0 );

      char strbuff[128] = {0};
      Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "Terrain CollisionResource[%d]",  0 );
      int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
      m_pCollisionModeResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
    }

    // インスタンス生成
    {
      m_pCollisionModel = GFL_NEW( pSystemHeap ) gfl2::collision::CollisionModel();
      m_pCollisionModel->Initialize( m_pCollisionModeResourceRootNode );
    }

  }

  //  m_pFieldTerrainBlockTable( NULL ),
  // m_BlockCountWidth( 0 ),
  // m_BlockCountHeight( 0 )

}

/**
* @brief 終了処理
*
* @return 無し
*/
void FieldTerrain::Finalize( void )
{
  // テクスチャデータ破棄
  {
    for( u32 i = 0; i < m_TextureDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppTextureResourceNodeTable[i] );
      GflHeapSafeFreeMemory( m_ppTextureBinaryTable[i] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppTextureBinaryTable );
    GFL_SAFE_DELETE_ARRAY( m_ppTextureResourceNodeTable );
    GFL_SAFE_DELETE( m_pTextureResourceRootNode );
  }
  // シェーダデータ破棄
  {
    for( u32 i = 0; i < m_ShaderDataCount; i++ )
    {
      GflHeapSafeFreeMemory( m_ppShaderBinaryTable[i] );
      GFL_SAFE_DELETE( m_ppShaderResourceNodeTable[i] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppShaderBinaryTable );
    GFL_SAFE_DELETE_ARRAY( m_ppShaderResourceNodeTable );
    GFL_SAFE_DELETE( m_pShaderResourceRootNode );
  }
  // モデルデータ破棄
  {
    for( u32 i = 0; i < m_ModelDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppModelInstanceNodeTable[i] );
      GFL_SAFE_DELETE( m_ppModelResourceRootNodeTable[i] );
      GflHeapSafeFreeMemory( m_ppModelBinaryTable[i] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppModelInstanceNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppModelResourceRootNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppModelBinaryTable );
  }
  // ミドルモデル
  {
    for( u32 i = 0; i < m_MiddleModelDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppMiddleModelInstanceNodeTable[i] );
      GFL_SAFE_DELETE( m_ppMiddleModelResourceRootNodeTable[i] );
      GflHeapSafeFreeMemory( m_ppMiddleModelBinaryTable[i] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppMiddleModelInstanceNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppMiddleModelResourceRootNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppMiddleModelBinaryTable );
  }
  // 遠景モデル
  {
    for( u32 i = 0; i < m_DistantViewModelDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppDistantViewModelInstanceNodeTable[i] );
      GFL_SAFE_DELETE( m_ppDistantViewModelResourceRootNodeTable[i] );
      GflHeapSafeFreeMemory( m_ppDistantViewModelBinaryTable[i] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppDistantViewModelInstanceNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppDistantViewModelResourceRootNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppDistantViewModelBinaryTable );
    GFL_SAFE_DELETE_ARRAY( m_pDistantViewTable );
  }
  // コリジョンモデル
  {
    GFL_SAFE_DELETE( m_pCollisionModel );
    GFL_SAFE_DELETE( m_pCollisionModeResourceRootNode );
    gfl2::gfx::GLMemory::Free( m_pCollisionModelBinary );
  }
  // 配置
  GFL_SAFE_DELETE_ARRAY( m_pFieldTerrainBlockTable );
}


void FieldTerrain::Update( const gfl2::math::Vector3& rPos, const gfl2::math::Matrix44& rViewProjMatrix )
{
  // カリング
  {
    // カリングに使用するマトリックスを生成
    gfl2::math::Matrix44 cullingMatrix[4];
    gfl2::math::AABB::Util::CalcFrustumPlanes( cullingMatrix, rViewProjMatrix );

    // とりあえず全部更新
    FieldTerrainBlock* pWork = m_pFieldTerrainBlockTable;
    for( u32 h = 0; h < blockHightCount; h++ )
    {
      for( u32 w = 0; w < blockWidthCount; w++ )
      {
        gfl2::math::AABB      aabb;
#if 0
        gfl2::math::Vector min;
        gfl2::math::Vector max;

        min.z = blockStartPosition.y + blockWidth * h;
        max.z = blockStartPosition.y + blockWidth * (h + 1);
        min.x = blockStartPosition.x + blockWidth * w;
        max.x = blockStartPosition.x + blockWidth * (w + 1);
        min.y = -5000.0f;
        max.y = 5000.0f;

        aabb.SetMinMax( min, max );
#endif
        pWork->UpdateCulling( &cullingMatrix[0], aabb );
        pWork++;
      }
    }

    // 遠景
    for( u32 i = 0; i < m_DistantViewModelDataCount; i++ )
    {
      gfl2::math::AABB   aabb;
#if 0
      if( i >= GFL_NELEMS( distantViewBlockAreaTable ) )
      {
        aabb.SetZero();
      }
      else
      {
        gfl2::math::Vector min;
        gfl2::math::Vector max;
        min.x = distantViewBlockAreaTable[i][0] * blockWidth;
        max.x = ( distantViewBlockAreaTable[i][1] + 1 )* blockWidth;
        min.z = distantViewBlockAreaTable[i][2] * blockWidth;
        max.z = ( distantViewBlockAreaTable[i][3] + 1 )* blockWidth;
        min.y = -5000.0f;
        max.y = 5000.0f;
        aabb.SetMinMax( min, max );
      }
#endif
      m_pDistantViewTable[i].UpdateCulling( &cullingMatrix[0], aabb );
    }

  }


  // 現在のブロックを決定
  f32 localX = rPos.x - blockStartPosition.x;
  f32 localZ = rPos.z - blockStartPosition.y;
  
  f32 workBlockX = localX / blockWidth;
  f32 workBlockZ = localZ / blockHight;

  s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, blockWidthCount );
  s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, blockHightCount );


  s32 highBlockStartX = blockX;
  s32 highBlockStartZ = blockZ;

  if( workBlockX - blockX < 0.5f )
  {
    highBlockStartX -= 1;
  }

  if( workBlockZ - blockZ < 0.5f )
  {
    highBlockStartZ -= 1;
  }

  if( m_HighBlockStartX == highBlockStartX && m_HighBlockStartZ == highBlockStartZ )
  {
    return;
  }

  m_HighBlockStartX = highBlockStartX;
  m_HighBlockStartZ = highBlockStartZ;

  // とりあえず全部Middle
  for( u32 h = 0; h < blockHightCount; h++ )
  {
    for( u32 w = 0; w < blockWidthCount; w++ )
    {
      GetBlock( w, h )->ChangeLod( FieldTerrainBlock::LOD_MIDDLE );
    }
  }

  // highモデル化
  for( u32 h = 0; h < 2; h++ )
  {
    s32 targetH = highBlockStartZ + h;
    if( targetH < 0 || targetH >= blockHightCount )
    {
      continue;
    }

    for( u32 w = 0; w < 2; w++ )
    {
      s32 targetW = highBlockStartX + w;
      if( targetW < 0 || targetW >= blockWidthCount )
      {
        continue;
      }

      GetBlock( targetW, targetH )->ChangeLod( FieldTerrainBlock::LOD_HIGHT );
    }
  }
}

FieldTerrainBlock* FieldTerrain::GetBlock( u32 width, u32 height )
{
  if( width >= m_BlockCountWidth || height >= m_BlockCountHeight )
  {
    return NULL;
  }

  u32 index = m_BlockCountWidth * height + width;
  return &m_pFieldTerrainBlockTable[ index ];
}



GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );
