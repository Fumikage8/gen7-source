//=================================================================================================
/**
*	@file	movemodel_rsc_manager.cpp
*	@brief
*	@author	tamada@gamefreak.co.jp
*	@date	2014.04.24
*/
//=================================================================================================
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include "System/include/GflvUse.h"
#include "../include/chara_rsc_manager.h"
#include "../include/chara_define.h"
///#include "test/FieldTest/include/Debug/FieldDebugInstanceMemoryCollection.h"
#include "test/FieldTest/include/FieldTestUtility.h"


#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryCollisionMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMcnfData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// arc
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include "arc_def_index/arc_def.h"
#include "arc_index/FieldChara.gaix"          // @todo 本当は直接ラベルを参照しないほうがよい
#include "arc_index/FieldCharaMotion.gaix"    // @todo 本当は直接ラベルを参照しないほうがよい

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
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "MoveModel Binary[%d]",  i );
    ///int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );@saito
    ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);@saito
    binaryTable[ i ] = GflHeapAllocMemoryAlign( pDeviceHeap , buffsize, 128 );
    ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);@saito

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


//-----------------------------------------------------------------------------
/**
*  @brief	初期化
*/
//-----------------------------------------------------------------------------
void RscManager::Setup( gfl2::heap::HeapBase * pDevice, gfl2::fs::AsyncFileManager* pAsyncFileManager )
{
  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    m_pBinaryDataArray[i] = NULL;
  }
  m_binaryDataIndex = 0;

  for( u32 i=0 ;i<RESOURCE_NODE_CNT ; i++ )
  {
    m_pResourceNodeArray[i] = NULL;
  }
  m_resourceNodeIndex = 0;

  m_pDevice = pDevice;
  m_pAsyncReadMan = pAsyncFileManager;

  m_ppModelBinaryTable = NULL;
  m_ppModelResourceNodeTable = NULL;

  // モデルリソースを準備
  {
    // バイナリ読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_FIELD_CHARA;
      openReq.heapForFile  = pDevice;
      openReq.heapForReq   = pDevice;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_FIELD_CHARA;
      closeReq.heapForReq  = pDevice;

      pAsyncFileManager->SyncArcFileOpen( openReq );
      m_ppModelBinaryTable = LoadBinary( pDevice, pDevice, pAsyncFileManager, ARCID_FIELD_CHARA, &m_ModelDataCount );
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // リソースを作成
    {
      m_ppModelResourceNodeTable = GFL_NEW( pDevice ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_ModelDataCount ];
      for( u32 i = 0; i < m_ModelDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack	resourceData;
        resourceData.SetModelData( static_cast<c8*>( m_ppModelBinaryTable[i] ) );

        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "MoveModel ModelResource[%d]",  i );
        ///int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );@saito
        ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);@saito
        m_ppModelResourceNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);@saito
      }
    }
  }


  // モーションリソースを準備
  {
    // バイナリ読み込み
    {
      gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
      openReq.arcId        = ARCID_FIELD_CHARA_MOTION;
      openReq.heapForFile  = pDevice;
      openReq.heapForReq   = pDevice;

      gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
      closeReq.arcId       = ARCID_FIELD_CHARA_MOTION;
      closeReq.heapForReq  = pDevice;

      pAsyncFileManager->SyncArcFileOpen( openReq );
      m_ppMotionBinaryTable = LoadBinary( pDevice, pDevice, pAsyncFileManager, ARCID_FIELD_CHARA_MOTION, &m_MotionDataCount );
      pAsyncFileManager->SyncArcFileClose( closeReq );
    }

    // リソースを作成
    {
      m_ppMotionResourceNodeTable = GFL_NEW( pDevice ) gfl2::renderingengine::scenegraph::resource::ResourceNode* [ m_MotionDataCount ];
      for( u32 i = 0; i < m_MotionDataCount; i++ )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
        resourceData.SetMotionData( static_cast<c8*>( m_ppMotionBinaryTable[i] ) );

        char strbuff[128] = {0};
        Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "MoveModel MotionResource[%d]",  i );
        ///int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );@saito
        ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);@saito
        m_ppMotionResourceNodeTable[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
        ///FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);@saito
      }
    }

  }

}
void RscManager::Terminate(void)
{
  // モーションデータ破棄
  {
    for( u32 i = 0; i < m_MotionDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppMotionResourceNodeTable[ i ] );
      GflHeapSafeFreeMemory( m_ppMotionBinaryTable[ i ] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppMotionResourceNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppMotionBinaryTable );
  }

  // モデルデータ破棄
  {
    for( u32 i = 0; i < m_ModelDataCount; i++ )
    {
      GFL_SAFE_DELETE( m_ppModelResourceNodeTable[ i ] );
      GflHeapSafeFreeMemory( m_ppModelBinaryTable[ i ] );
    }
    GFL_SAFE_DELETE_ARRAY( m_ppModelResourceNodeTable );
    GFL_SAFE_DELETE_ARRAY( m_ppModelBinaryTable );
  }

  m_pDevice = NULL;
  m_pAsyncReadMan = NULL;

  for( u32 i=0 ;i<BIN_DATA_CNT ; i++ )
  {
    if (m_pBinaryDataArray[i] !=NULL)
    {
      ///gfl2::gfx::GLMemory::Free( m_pBinaryDataArray[i] );
      m_ResourceAllocator.SystemMemoryFree(m_pBinaryDataArray[i]);
    }
  }

  for( u32 i=0 ;i<RESOURCE_NODE_CNT ; i++ )
  {
    if (m_pResourceNodeArray[i]!=NULL)
    {
      GFL_PRINT("%d\n",i);
      gfl2::gfx::GLMemory::Free( m_pResourceNodeArray[i] );
    }
  }

}


//----------------------------------------------------------------------------
/**
*	@brief  HIGHモデルの取得関数
*
*	@param	id		リソースID
*	@return モデルリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetHighModelResource(chara::RSC_ID id)
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinModelRootNode = NULL;


  //
  // ARCID_FIELD_CHARA 参照
  //

  switch( id )
  {
  case chara::RSCID_TEST_KISEKAE:   // 着せ替え検証用
    {
      ///return m_ppModelResourceNodeTable[ GARC_FieldChara_tr0001_00_GFBMDLP ];//@saito


      // パックされたデータからリソースノードを作成
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00.wingl.gfbmdlp" );
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
      break;
    }
  case chara::RSCID_TEST_KENTAUROS: // ポケライド検証用
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_pm0128_00_GFBMDLP ];
    }
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_proportion_Cf_fi01_GFBMDLP ];
    }
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_tr9999_00_GFBMDLP ];
    }

  case chara::RSCID_TEST_RIM:
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_pSphere1_GFBMDLP ];
    }
  }

  //
  // その他
  //

  switch( id )
  {
  case chara::RSCID_MAP00_00_00:
    {
      // パックされたデータからリソースノードを作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
        c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00_00_00/map00_00_00.ctr.gfbmdlp" );
#else
        c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00_00_00/map00_00_00.wingl.gfbmdlp" );
#endif
        resourceData.SetModelData( pBinData );
        pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

        m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
        m_binaryDataIndex++;        
      }

      break;
    } // chara::RSCID_MAP00_00_00

  case chara::RSCID_MAP00_00_01:
    {
      // パックされたデータからリソースノードを作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
        c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00_00_01/map00_00_01.ctr.gfbmdlp" );
#else
        c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00_00_01/map00_00_01.wingl.gfbmdlp" );
#endif
        resourceData.SetModelData( pBinData );
        pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

        m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
        m_binaryDataIndex++;
      }

      break;
    } // chara::RSCID_MAP00_00_01


  case chara::RSCID_MAP00_01_00:
    {

      // パックされたデータからリソースノードを作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
        c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00_01_00/map00_01_00.ctr.gfbmdlp" );
#else
        c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00_01_00/map00_01_00.wingl.gfbmdlp" );
#endif
        resourceData.SetModelData( pBinData );
        pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

        m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
        m_binaryDataIndex++;
      }

      break;

    } // chara::RSCID_MAP00_01_00


  case chara::RSCID_MAP00_01_01:
    {
      // パックされたデータからリソースノードを作成
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
        c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00_01_01/map00_01_01.ctr.gfbmdlp" );
#else
        c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00_01_01/map00_01_01.wingl.gfbmdlp" );
#endif
        resourceData.SetModelData( pBinData );
        pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

        m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
        m_binaryDataIndex++;
      }

      break;
    } // chara::RSCID_MAP00_01_01

    case chara::RSCID_MAP00_TEST:
      {
        // モデル
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00_test/com_block_ikuta_150227.ctr.gfbmdl" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/com_block_ikuta_150227.wingl.gfbmdl" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        // シェーダー
        {
          static const char* pShaderFileList[] =
          {
#if defined(GF_PLATFORM_CTR)
            "rom:/data/test/field_test/map00_test/Shaders/com1_rp_02_f_Manual.ctr.gfbfsh",
            "rom:/data/test/field_test/map00_test/Shaders/Default.ctr.gfbvsh",
            "rom:/data/test/field_test/map00_test/Shaders/light_Manual.ctr.gfbfsh",
#else
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Shaders/com1_rp_02_f_Manual.wingl.gfbfsh",
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Shaders/Default.wingl.gfbvsh",
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Shaders/light_Manual.wingl.gfbfsh",
#endif
          };

          u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
          for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
            c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

            resourceData.SetShaderData( pBinData );

            gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
            pBinModelRootNode->AddChild( pBinShdNode );

            m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
            m_binaryDataIndex++;
          }
        }
        // テクスチャ
        {
          static const c8* pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
            "rom:/data/test/field_test/map00_test/Textures/com1_rp_02_f.ctr.tex",
            "rom:/data/test/field_test/map00_test/Textures/com1_rp_04_f2.ctr.tex",
            "rom:/data/test/field_test/map00_test/Textures/light.ctr.tex",
            "rom:/data/test/field_test/map00_test/Textures/testout5_arp03.ctr.tex",
#else
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Textures/com1_rp_02_f.wingl.tex",
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Textures/com1_rp_04_f2.wingl.tex",
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Textures/light.wingl.tex",
            "R:/home/niji/exec/data_win32/data/test/field_test/map00_test/Textures/testout5_arp03.wingl.tex",
#endif
          };

          u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
          for( u32 texNo = 0; texNo < texNum; ++texNo )
          {
            gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
            gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
            c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

            texResourceData.SetTextureData( pTexBinData );

            pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
            pBinModelRootNode->AddChild( pBinTexRootNode );

            m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
            m_binaryDataIndex++;
          }
        }


        break;
      } // case chara::RSCID_MAP00_TEST


    case chara::RSCID_HERO:
      {
        // パックされたデータからリソースノードを作成
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/tr0001/tr0001_00.ctr.gfbmdlp" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00.wingl.gfbmdlp" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        break;
      } // chara::RSCID_HERO:

    case chara::RSCID_EDGE_TEST_00:
      {
        // パックされたデータからリソースノードを作成
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/edge_test_00/proportion_Cf_fi00.ctr.gfbmdlp" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/edge_test_00/proportion_Cf_fi00.wingl.gfbmdlp" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        break;
      } // chara::RSCID_EDGE_TEST_00

    case chara::RSCID_EDGE_TEST_01:
      {
        // パックされたデータからリソースノードを作成
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/edge_test_01/proportion_Cf_fi01.ctr.gfbmdlp" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/edge_test_01/proportion_Cf_fi01.wingl.gfbmdlp" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        break;
      } // chara::RSCID_EDGE_TEST_01


    case chara::RSCID_MAT_TEST_00:
      {
        // パックされたデータからリソースノードを作成
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/mat_test_00/proportion_Cf_fi00m.ctr.gfbmdlp" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_00/proportion_Cf_fi00m.wingl.gfbmdlp" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        break;
      } // chara::RSCID_MAT_TEST_00

    case chara::RSCID_MAT_TEST_01:
      {
        // パックされたデータからリソースノードを作成
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
#if defined(GF_PLATFORM_CTR)
          c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/mat_test_01/proportion_Cf_fi01m.ctr.gfbmdlp" );
#else
          c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_01/proportion_Cf_fi01m.wingl.gfbmdlp" );
#endif
          resourceData.SetModelData( pBinData );
          pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

          m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
          m_binaryDataIndex++;
        }

        break;
      } // chara::RSCID_MAT_TEST_01

  } // switch

  if( id == chara::RSCID_MAP_01_KABE )
  {
    // モデル	
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map01/map_kabe.ctr.gfbmdl" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map01/map_kabe.wingl.gfbmdl" );
#endif
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/map01/Shader/Comb1.ctr.gfbvsh",
      "rom:/data/test/field_test/map01/Shader/Comb1.ctr.gfbfsh",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/map01/Shader/Comb1.wingl.gfbvsh",
      "R:/home/niji/exec/data_win32/data/test/field_test/map01/Shader/Comb1.wingl.gfbfsh",
#endif
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
        "rom:/data/test/field_test/map01/Texture/tex1.ctr.tex",
#else
        "R:/home/niji/exec/data_win32/data/test/field_test/map01/Texture/tex1.wingl.tex",
#endif
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }
  }

  if( id == chara::RSCID_MAP_01_YUKA )
  {
    // モデル	
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map01/map_yuka.ctr.gfbmdl" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map01/map_yuka.wingl.gfbmdl" );
#endif
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/map01/Shader/Comb1.ctr.gfbvsh",
      "rom:/data/test/field_test/map01/Shader/Comb1.ctr.gfbfsh",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/map01/Shader/Comb1.wingl.gfbvsh",
      "R:/home/niji/exec/data_win32/data/test/field_test/map01/Shader/Comb1.wingl.gfbfsh",
#endif
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
        "rom:/data/test/field_test/map01/Texture/tex1.ctr.tex",
#else
        "R:/home/niji/exec/data_win32/data/test/field_test/map01/Texture/tex1.wingl.tex",
#endif
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }
  }
  ///@saito ビューア地形
  if( id == chara::RSCID_VIEWER_MAP )
  {
    // パックされたデータからリソースノードを作成
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.wingl.gfbmdlp" );
    resourceData.SetModelData( pBinData );
    pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

    m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
    m_binaryDataIndex++;

#if 0
    // モデル	
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.wingl.gfbmdl" );
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
      "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/Shader/Comb1.wingl.gfbvsh",
      "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/Shader/Comb1.wingl.gfbfsh",
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
        "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/Texture/texture.wingl.tex",
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }
#endif
  }

  if( id == chara::RSCID_PIKACYU )
  {
    // モデル	
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/pm0025_00/pm0025_00.ctr.gfbmdl" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00.gfbmdl.wingl" );
#endif
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/pm0025_00/Shader/PDC_Body_Shadow.ctr.gfbvsh",
      "rom:/data/test/field_test/pm0025_00/Shader/PDC_Body_Shadow.ctr.gfbfsh",
      "rom:/data/test/field_test/pm0025_00/Shader/PDC_pm_keikouo_Neon.ctr.gfbvsh",
      "rom:/data/test/field_test/pm0025_00/Shader/PDC_pm_keikouo_Neon.ctr.gfbfsh",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Shader/PDC_Body_Shadow.gfvsh.wingl",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Shader/PDC_Body_Shadow.gffsh.wingl",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Shader/PDC_pm_keikouo_Neon.gfvsh.wingl",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Shader/PDC_pm_keikouo_Neon.gffsh.wingl",
#endif
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_Eye1.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_EyeNor.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_EyeHlm.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_Mouth1.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_MouthNor.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_MouthHlm.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyALta.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyA1.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/DummyTex.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyANor.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/Default_Lta_10D.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_Hta.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/LinerTable.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyAHlm.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyB1.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBInc.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBNor.3ds.tex",
        "rom:/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBHlm.3ds.tex"
#else
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_Eye1.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_EyeNor.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_EyeHlm.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_Mouth1.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_MouthNor.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_MouthHlm.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyALta.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyA1.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/DummyTex.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyANor.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/Default_Lta_10D.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_Hta.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/LinerTable.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyAHlm.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyB1.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBInc.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBNor.gfbtex.wingl",
        "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/Texture/pm0025_00_BodyBHlm.gfbtex.wingl"
#endif
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }
  }
  if( id == chara::RSCID_ATTACH_OBJ_TSURIZAO )
  {
    // モデル
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/ob0010/ob0010_00_fi.ctr.gfbmdl" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/ob0010/ob0010_00_fi.wingl.gfbmdl" );
#endif
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/ob0010/Shaders/FieldChar.ctr.gfbvsh",
      "rom:/data/test/field_test/ob0010/Shaders/CHARA_Ftrainer01.ctr.gfbfsh",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/ob0010/Shaders/FieldChar.wingl.gfbvsh",
      "R:/home/niji/exec/data_win32/data/test/field_test/ob0010/Shaders/CHARA_Ftrainer01.wingl.gfbfsh",
#endif
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
        "rom:/data/test/field_test/ob0010/Textures/ob0010_00_fi.ctr.tex",
#else
        "R:/home/niji/exec/data_win32/data/test/field_test/ob0010/Textures/ob0010_00_fi.wingl.tex",
#endif
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }

  }

  m_pResourceNodeArray[m_resourceNodeIndex] = pBinModelRootNode;
  m_resourceNodeIndex++;
  
  return pBinModelRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  LOWモデルの取得関数
*
*	@param	id		リソースID
*	@return モデルリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetLowModelResource(chara::RSC_ID id)
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinModelRootNode = NULL;
  
  //
  // ARCID_FIELD_CHARA 参照
  //

  switch( id )
  {
#if 0
  case chara::RSCID_TEST_KISEKAE:   // 着せ替え検証用
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_tr0001_00_GFBMDLP ];
    }
#endif
  case chara::RSCID_TEST_KENTAUROS: // ポケライド検証用
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_pm0128_00_low_GFBMDLP ];
    }
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_proportion_Cf_fi01_low_GFBMDLP ];
    }
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
      return m_ppModelResourceNodeTable[ GARC_FieldChara_tr9999_00_low_GFBMDLP ];
    }
  }

  if( id == chara::RSCID_HERO )
  {
    // モデル	
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryMdlData resourceData;
#if defined(GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/tr0001/tr0001_00.ctr.gfbmdl" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00.wingl.gfbmdl" );
#endif
      resourceData.SetModelData( pBinData );
      pBinModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // シェーダ
    static const char* pShaderFileList[] = 
    {
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/tr0001/Shader/charafield01.ctr.gfbvsh",
      "rom:/data/test/field_test/tr0001/Shader/charafield01.ctr.gfbfsh",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Shader/charafield01.wingl.gfbvsh",
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Shader/charafield01.wingl.gfbfsh",
#endif
    };

    u32 shaderNum = sizeof(pShaderFileList) / sizeof(pShaderFileList[0]);
    for( u32 shaderNo = 0; shaderNo < shaderNum; ++shaderNo )
    {
      gfl2::renderingengine::scenegraph::resource::GfBinaryShdData resourceData;
      c8 *pBinData = (c8*)ReadData( pShaderFileList[shaderNo] );

      resourceData.SetShaderData( pBinData );

      gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinShdNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );
      pBinModelRootNode->AddChild( pBinShdNode );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }

    // テクスチャ
    {
      static const c8*			pTextureFileList[] = {
#if defined(GF_PLATFORM_CTR)
        "rom:/data/test/field_test/tr0001/Texture/Toontable.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_bag_ye.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_boots_re.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_btms_bl.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_face02.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_hair_br.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_hat_re.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_legs.ctr.tex",
        "rom:/data/test/field_test/tr0001/Texture/tr0001_tops_re.ctr.tex",
#else
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/Toontable.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_bag_ye.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_boots_re.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_btms_bl.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_face02.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_hair_br.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_hat_re.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_legs.wingl.tex",
        "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/Texture/tr0001_tops_re.wingl.tex",
#endif
      };

      u32 texNum = sizeof(pTextureFileList) / sizeof(pTextureFileList[0]);
      for( u32 texNo = 0; texNo < texNum; ++texNo )
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData	texResourceData;
        gfl2::renderingengine::scenegraph::resource::ResourceNode		*pBinTexRootNode;
        c8 *pTexBinData = (c8*)ReadData( pTextureFileList[texNo] );

        texResourceData.SetTextureData( pTexBinData );

        pBinTexRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &texResourceData );
        pBinModelRootNode->AddChild( pBinTexRootNode );

        m_pBinaryDataArray[m_binaryDataIndex] = pTexBinData;
        m_binaryDataIndex++;
      }
    }
  }

  m_pResourceNodeArray[m_resourceNodeIndex] = pBinModelRootNode;
  m_resourceNodeIndex++;
  
  return pBinModelRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  モーションの取得関数
*
*	@param	id		リソースID
*	@return モーションリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetMotionResource(chara::RSC_ID id,u32 motionId)
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode = NULL;

  //
  // ARCID_FIELD_CHARA 参照
  //

  switch( id )
  {
  case chara::RSCID_TEST_KISEKAE:   // 着せ替え検証用
    {
      static const c8*				s_pMotionFilePath[] = 
    {
      "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00_wait.gfbmot",
      "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00_walk.gfbmot",
      "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00_walk.gfbmot",
      "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00_walk.gfbmot",
      "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001_00_walk.gfbmot",
    };

    u32 size = sizeof( s_pMotionFilePath ) / sizeof( c8* );
    if( size < motionId )
    {
      return NULL;
    }

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
    c8* pMotData = (c8*)ReadData( s_pMotionFilePath[motionId] ); // @todo しまったこれだと開放できない
    animationResourceData.SetMotionData(pMotData);

    pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

    m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
    m_binaryDataIndex++;
/**
      static s32 datID[] = 
      {
        GARC_FieldCharaMotion_tr0001_00_wait_GFBMOT,
        GARC_FieldCharaMotion_tr0001_00_walk_GFBMOT,
        GARC_FieldCharaMotion_tr0001_00_walk_GFBMOT,
        GARC_FieldCharaMotion_tr0001_00_walk_GFBMOT
      };

      if( motionId < GFL_NELEMS( datID ) && datID[motionId] >= 0 )
      {
        return m_ppMotionResourceNodeTable[ datID[ motionId ] ];
      }
*///@saito
      break;
    }
  case chara::RSCID_TEST_KENTAUROS: // ポケライド検証用
    {
      static s32 datID[] = 
      {
        GARC_FieldCharaMotion_pm0128_00_wait_GFBMOT,
        GARC_FieldCharaMotion_pm0128_00_walk_GFBMOT,
        -1,
        -1
      };

      if( motionId < GFL_NELEMS( datID ) && datID[motionId] >= 0 )
      {
        return m_ppMotionResourceNodeTable[ datID[ motionId ] ];
      }
      break;
    }
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
    {
      static s32 datID[] = 
      {
        GARC_FieldCharaMotion_proportion_Cf_fi01_wait_GFBMOT,
        GARC_FieldCharaMotion_proportion_Cf_fi01_walk_GFBMOT,
        -1,
        -1
      };

      if( motionId < GFL_NELEMS( datID ) && datID[motionId] >= 0 )
      {
        return m_ppMotionResourceNodeTable[ datID[ motionId ] ];
      }
      break;
    }
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
      static s32 datID[] = 
      {
        GARC_FieldCharaMotion_tr9999_00_wait_GFBMOT,
        GARC_FieldCharaMotion_tr9999_00_walk_GFBMOT,
        GARC_FieldCharaMotion_pm0128_00_walk_GFBMOT
        -1,
        -1
      };

      if( motionId < GFL_NELEMS( datID ) && datID[motionId] >= 0 )
      {
        return m_ppMotionResourceNodeTable[ datID[ motionId ] ];
      }
      break;
    }
  }

  //
  // その他
  //

  if( id == chara::RSCID_PIKACYU )
  {
    static const c8*				s_pMotionFilePath[] = 
    {
      //"../data/pm0025_00/wait.gfmot",
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/pm0025_00/pm0025_00_ba10_waitA01.bgfmot",
      "rom:/data/test/field_test/pm0025_00/pm0025_00_ba51_walk01.bgfmot",
      "rom:/data/test/field_test/pm0025_00/pm0025_00_ba52_run01.bgfmot",
      "rom:/data/test/field_test/pm0025_00/pm0025_00_ba51_walk01.bgfmot",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00_ba10_waitA01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00_ba51_walk01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00_ba52_run01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00_ba51_walk01.bgfmot",
#endif
    };

    u32 size = sizeof( s_pMotionFilePath ) / sizeof( c8* );
    if( size < motionId )
    {
      return NULL;
    }

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
    c8* pMotData = (c8*)ReadData( s_pMotionFilePath[motionId] ); // @todo しまったこれだと開放できない
    animationResourceData.SetMotionData(pMotData);

    pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

    m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
    m_binaryDataIndex++;
  }

  if( id == chara::RSCID_HERO )
  {
    static const c8*				s_pMotionFilePath[] = 
    {
      //"../data/pm0025_00/wait.gfmot",
#if defined(GF_PLATFORM_CTR)
      "rom:/data/test/field_test/tr0001/tr0001_00_fi01_wait01.bgfmot",
      "rom:/data/test/field_test/tr0001/tr0001_00_fi20_walk01.bgfmot",
      "rom:/data/test/field_test/tr0001/tr0001_00_fi21_run01.bgfmot",
      "rom:/data/test/field_test/tr0001/tr0001_00_fi20_walk02.bgfmot",
      "rom:/data/test/field_test/tr0001/tr0001_00_fi00_throw01.gfbmot",
#else
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00_fi01_wait01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00_fi20_walk01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00_fi21_run01.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00_fi20_walk02.bgfmot",
      "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001_00_fi00_throw01.gfbmot",
#endif
    };

    u32 size = sizeof( s_pMotionFilePath ) / sizeof( c8* );
    if( size < motionId )
    {
      return NULL;
    }

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
    c8* pMotData = (c8*)ReadData( s_pMotionFilePath[motionId] ); // @todo しまったこれだと開放できない
    animationResourceData.SetMotionData(pMotData);

    pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );

    m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
    m_binaryDataIndex++;
  }

  if( id == chara::RSCID_ATTACH_OBJ_TSURIZAO )
  {
    static const c8*				s_pMotionFilePath[] = 
    {
#if defined(GF_PLATFORM_CTR)
      NULL,
      "rom:/data/test/field_test/ob0010/ob0010_throw01.gfbmot",
      NULL,
      NULL,
      NULL,
#else
      NULL,
      "R:/home/niji/exec/data_win32/data/test/field_test/ob0010/ob0010_throw01.gfbmot",
      NULL,
      NULL,
      NULL,
#endif
    };

    u32 size = sizeof( s_pMotionFilePath ) / sizeof( c8* );
    if( size < motionId || s_pMotionFilePath[motionId] == NULL )
    {
      return NULL;
    }

    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
    c8* pMotData = (c8*)ReadData( s_pMotionFilePath[motionId] ); // @todo しまったこれだと開放できない
    animationResourceData.SetMotionData(pMotData);

    pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );

    m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
    m_binaryDataIndex++;
  }

  m_pResourceNodeArray[m_resourceNodeIndex] = pMotionResourceRootNode;
  m_resourceNodeIndex++;
  
  return pMotionResourceRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  口制御モーションの取得関数
*
*	@param	id		リソースID
*	@return 口制御モーションリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetMouthMotionResource( chara::RSC_ID id )
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode = NULL;
  
  switch( id )
  {
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
    {
      return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_proportion_Cf_fi01_mouth01_GFBMOT ];
    }
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
      return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_tr9999_00_mouth01_GFBMOT ];
    }
  }

  switch( id )
  {
  case chara::RSCID_MAT_TEST_00:
    {
#if defined(GF_PLATFORM_CTR)
      static const c8* sc_pMotionFilePath = "rom:/data/test/field_test/mat_test_00/proportion_Cf_fi00m_mouth.gfbmot";
#else
      static const c8* sc_pMotionFilePath = "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_00/proportion_Cf_fi00m_mouth.gfbmot";
#endif

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
      c8* pMotData = (c8*)ReadData( sc_pMotionFilePath ); // @todo しまったこれだと開放できない
      animationResourceData.SetMotionData( pMotData );
      pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

      m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
      m_binaryDataIndex++;

      break;
    } // case chara::RSCID_MAT_TEST_01:

  case chara::RSCID_MAT_TEST_01:
    {
#if defined(GF_PLATFORM_CTR)
      static const c8* sc_pMotionFilePath = "rom:/data/test/field_test/mat_test_01/proportion_Cf_fi01m_mouth.gfbmot";
#else
      static const c8* sc_pMotionFilePath = "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_01/proportion_Cf_fi01m_mouth.gfbmot";
#endif

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
      c8* pMotData = (c8*)ReadData( sc_pMotionFilePath ); // @todo しまったこれだと開放できない
      animationResourceData.SetMotionData( pMotData );
      pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

      m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
      m_binaryDataIndex++;

      break;
    } // case chara::RSCID_MAT_TEST_01:
  } 

  m_pResourceNodeArray[m_resourceNodeIndex] = pMotionResourceRootNode;
  m_resourceNodeIndex++;
  
  return pMotionResourceRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  目パチ制御モーションの取得関数
*
*	@param	id		リソースID
*	@return 口制御モーションリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetEyeMotionResource( chara::RSC_ID id )
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode = NULL;

  switch( id )
  {
  case chara::RSCID_TEST_KISEKAE:   // 着せ替え検証用
    {
      ///return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_tr0001_00_eye01_GFBMOT ];@saito
    }
  case chara::RSCID_TEST_KENTAUROS: // ポケライド検証用
    {
      return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_pm0128_00_eye01_GFBMOT ];
    }
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
    {
      return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_proportion_Cf_fi01_eye01_GFBMOT ];
    }
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
      return m_ppMotionResourceNodeTable[ GARC_FieldCharaMotion_tr9999_00_eye01_GFBMOT ];
    }
  }

  switch( id )
  {
  case chara::RSCID_MAT_TEST_00:
    {
#if defined(GF_PLATFORM_CTR)
      static const c8* sc_pMotionFilePath = "rom:/data/test/field_test/mat_test_00/proportion_Cf_fi00m_eye.gfbmot";
#else
      static const c8* sc_pMotionFilePath = "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_00/proportion_Cf_fi00m_eye.gfbmot";
#endif

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
      c8* pMotData = (c8*)ReadData( sc_pMotionFilePath ); // @todo しまったこれだと開放できない
      animationResourceData.SetMotionData( pMotData );
      pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

      m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
      m_binaryDataIndex++;

      break;
    } // case chara::RSCID_MAT_TEST_01:

  case chara::RSCID_MAT_TEST_01:
    {
#if defined(GF_PLATFORM_CTR)
      static const c8* sc_pMotionFilePath = "rom:/data/test/field_test/mat_test_01/proportion_Cf_fi01m_eye.gfbmot";
#else
      static const c8* sc_pMotionFilePath = "R:/home/niji/exec/data_win32/data/test/field_test/mat_test_01/proportion_Cf_fi01m_eye.gfbmot";
#endif

      gfl2::renderingengine::scenegraph::resource::GfBinaryMotData animationResourceData;
      c8* pMotData = (c8*)ReadData( sc_pMotionFilePath ); // @todo しまったこれだと開放できない
      animationResourceData.SetMotionData( pMotData );
      pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &animationResourceData );		

      m_pBinaryDataArray[m_binaryDataIndex] = pMotData;
      m_binaryDataIndex++;

      break;
    } // case chara::RSCID_MAT_TEST_01:
  } 

  m_pResourceNodeArray[m_resourceNodeIndex] = pMotionResourceRootNode;
  m_resourceNodeIndex++;
  
  return pMotionResourceRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  モーションの取得関数
*
*	@param	id		リソースID
*	@return モーションリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetMotionConfigResource(chara::RSC_ID id,u32 motionId)
{
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResourceRootNode = NULL;

  switch( id )
  {
  case chara::RSCID_TEST_KISEKAE:   // 着せ替え検証用
  case chara::RSCID_TEST_NPC_1500:  // 1500ポリゴンNPCモデル（Llモデル）
  case chara::RSCID_TEST_NPC_2000:  // 2000ポリゴンNPCモデル（Lhモデル）
    {
#if defined( GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/tr0001/tr0001old.bmcnf" );
#else
      ///c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001old.bmcnf" );@saito
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/Player/tr0001old.bmcnf");
#endif // GF_PLATFORM_CTR

      // モーションコンフィグ設定
      gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData motionConfigResourceData;
      motionConfigResourceData.SetMcnfData( pBinData );
      pMotionConfigResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);
      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
      return pMotionConfigResourceRootNode;
    }
  }


  if( id == chara::RSCID_HERO )
  {
#if defined( GF_PLATFORM_CTR)
    c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/tr0001/tr0001old.bmcnf" );
#else
    c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/tr0001/tr0001old.bmcnf" );
#endif // GF_PLATFORM_CTR

    // モーションコンフィグ設定
    gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData motionConfigResourceData;
    motionConfigResourceData.SetMcnfData( pBinData );
    pMotionConfigResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);

    m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
    m_binaryDataIndex++;
  }

  if( id == chara::RSCID_PIKACYU )
  {
#if defined( GF_PLATFORM_CTR)
    c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/pm0025_00/pm0025_00_motion_kw.bmcnf" );
#else
    c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/pm0025_00/pm0025_00_motion_kw.bmcnf" );
#endif // GF_PLATFORM_CTR

    // モーションコンフィグ設定
    gfl2::renderingengine::scenegraph::resource::GfBinaryMcnfData motionConfigResourceData;
    motionConfigResourceData.SetMcnfData( pBinData );
    pMotionConfigResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(&motionConfigResourceData);

    m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
    m_binaryDataIndex++;
  }

  m_pResourceNodeArray[m_resourceNodeIndex] = pMotionConfigResourceRootNode;
  m_resourceNodeIndex++;
  return pMotionConfigResourceRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  衝突されるコリジョンの取得関数
*
*	@param	id		リソースID
*	@return コリジョンリソースノードのインスタンス
*/
//-----------------------------------------------------------------------------
gfl2::renderingengine::scenegraph::resource::ResourceNode* RscManager::GetStaticCollisionResource(chara::RSC_ID id)
{
  //******注意*******//
  // CreateResourceNodeに失敗する場合はキャッシュの再作成をお願いします
  //******注意*******//

  gfl2::renderingengine::scenegraph::resource::ResourceNode *pBinColModelRootNode = NULL;

  if( id == chara::RSCID_MAP00_00_01 || id == chara::RSCID_MAP00_TEST )
  {
    // 高さコリジョンメッシュデータ読み込み
    {
      s32 size=0;
#if defined( GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map00/map00_height.gfbcol" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map00/map00_height.gfbcol" );
#endif // GF_PLATFORM_CTR

      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( pBinData, size);
      pBinColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }
  }

  if( id == chara::RSCID_MAP_01_KABE )
  {
    // 高さコリジョンメッシュデータ読み込み
    {
      s32 size=0;
#if defined( GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map01/map_height_kabe.gfbcol" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map01/map_height_kabe.gfbcol" );
      ///c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.gfbcol" );
#endif // GF_PLATFORM_CTR

      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( pBinData, size);
      pBinColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }
  }
  if( id == chara::RSCID_MAP_01_YUKA )
  {
    {
      // 高さコリジョンメッシュデータ読み込み
      s32 size=0;
#if defined( GF_PLATFORM_CTR)
      c8 *pBinData = (c8*)ReadData( "rom:/data/test/field_test/map01/map_height_yuka.gfbcol" );
#else
      c8 *pBinData = (c8*)ReadData( "R:/home/niji/exec/data_win32/data/test/field_test/map01/map_height_yuka.gfbcol" );
#endif // GF_PLATFORM_CTR

      gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
      resourceData.SetModelData( pBinData, size);
      pBinColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

      m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
      m_binaryDataIndex++;
    }
  }
  if ( id == chara::RSCID_VIEWER_MAP )
  {
    // 高さコリジョンメッシュデータ読み込み
    s32 size=0;
    c8 *pBinData = (c8*)ReadData( "R:/home/niji/tool/FieldTools/MapViewer/TargetRes/map.gfbcol" );

    gfl2::renderingengine::scenegraph::resource::GfBinaryCollisionMdlData resourceData;
    resourceData.SetModelData( pBinData, size);
    pBinColModelRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( &resourceData );

    m_pBinaryDataArray[m_binaryDataIndex] = pBinData;
    m_binaryDataIndex++;
  }

  m_pResourceNodeArray[m_resourceNodeIndex] = pBinColModelRootNode;
  m_resourceNodeIndex++;
  
  return pBinColModelRootNode;
}

//----------------------------------------------------------------------------
/**
*	@brief  ファイル読み込み
*	@param  利用するアロケータ
*	@param  ファイルパス
*	@return Rawデータ
*/
//-----------------------------------------------------------------------------
void* RscManager::ReadData( const char* pFilePath )
{
#if defined(GF_PLATFORM_CTR)
  nn::fs::FileReader file( pFilePath );
  u32 fileSize = file.GetSize();
  void* pShaderBinary = m_ResourceAllocator.SystemMemoryMalloc(fileSize, 128);
  file.Read(pShaderBinary, fileSize);
  return pShaderBinary;
#else
  FILE		*hFile;
  int			file_size;
  char		*read_buff;

  fopen_s( &hFile, pFilePath, "rb" );

  if ( hFile == NULL )
  {
    assert(0);
    return NULL;
  }

  fseek( hFile, 0, SEEK_END );
  file_size = ftell( hFile );
  fseek( hFile, 0, SEEK_SET );

  u32		align = 16;
  file_size += align;//必ずNULLが入るようにしたい。
  u32 buf_size = file_size + ((-file_size) & (align-1));

  read_buff = reinterpret_cast<c8*>( m_ResourceAllocator.SystemMemoryMalloc(buf_size,128) );
  memset( read_buff, 0, buf_size );
  fread( read_buff, file_size, 1, hFile );
  fclose( hFile );

  return read_buff;
#endif
}
