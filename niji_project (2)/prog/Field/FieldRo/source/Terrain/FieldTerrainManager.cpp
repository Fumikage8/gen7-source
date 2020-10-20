/**
* @brief  フィールドの地形管理を担うクラス
* @file   FieldTerrainManager.cpp
* @author ikuta_junya
* @data   2015.04.19
*/


// gfl2
#include <fs/include/gfl2_BinLinkerAccessor.h>
#include <debug/include/gfl2_DebugPrint.h>

// RenderingEngine
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainManager.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainLayoutData.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBlock.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainDistantView.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainBgPartsManager.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockLodChangeStaticListener.h"

#include "Field/FieldRo/include/Collision/CollisionSceneNone.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

#if PM_DEBUG
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#endif

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static s32 BLOCK_INDEX_ERROR = -1000;
const static s32 HIGH_MODEL_LINE_MAX( 2 );             // 各軸毎のHighモデル読み込み数
const static s32 HIGH_MODEL_LINE_MAX_PLAN( 1 );        // 各軸毎のHighモデル読み込み数(PLANモード)
const static s32 MIDDLE_MODEL_DRAW_LINE_LIMITLESS(-1); // ミドルモデル表示制限なし

// 固定サイズのメモリ確保を行うローカルヒープを保持するクラス
// @note テンプレートクラスにしたくなかったので、実際の確保機能までは備えていない
class LocalHeapUtilForFixedSize
{
public:
  LocalHeapUtilForFixedSize( gfl2::heap::HeapBase* pParent, size_t size, u32 num ) :
     m_pHeap( NULL ),
     m_Count( 0 ),
     m_Max( 0 )
  {
    if( num <= 0 )
    {
      GFL_ASSERT( 0 );
      return;
    }

    m_Max = num;
    size_t memSizeOnce = size + pParent->CalcAllocateAdjustSize( pParent->GetDefaultAlign() );
    size_t heapSize = memSizeOnce * m_Max * 1.5f; // 1.5倍は念のため
    m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME( pParent, heapSize, gfl2::heap::HEAP_TYPE_EXP, false, "LodListener" );
  }

  ~LocalHeapUtilForFixedSize()
  {
    if( m_pHeap )
    {
      GFL_DELETE_HEAP( m_pHeap );
      m_pHeap = NULL;
    }
  }

  //
  // 操作系関数
  //

  // 使用にあわせて必ず呼ぶ
  void IncCount( void )
  {
    GFL_ASSERT_STOP( m_Count < m_Max );
    m_Count++;
  }
  void DecCount( void )
  {
    GFL_ASSERT_STOP( m_Count > 0 );
    m_Count--;
  }
  gfl2::heap::HeapBase* GetHeap( void )
  {
    return m_pHeap;
  }

  //
  // 情報取得系関数
  //
  u32 GetMax( void ) const
  {
    return m_Max;
  }

  u32 GetCount( void ) const
  {
    return m_Count;
  }

  b32 IsFull( void ) const
  {
    return ( m_Count >= m_Max );
  }

private:
  gfl2::heap::HeapBase* m_pHeap;  // ローカルヒープ実体
  u32 m_Count;                    // 使用数
  u32 m_Max;                      // 最大数
};


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainManager::TerrainManager( void ) :

  // 制御
  m_IsControlStart( false )
  ,m_BlockControlMode( BlockControlMode::NORMAL )
  ,m_BlockControlModeDefault( BlockControlMode::NORMAL )
  ,m_BlockControlPosition( 0.0f, 0.0f, 0.0f )
  ,m_HighBlockStartX( BLOCK_INDEX_ERROR )
  ,m_HighBlockStartZ( BLOCK_INDEX_ERROR )
  ,m_LocalAnimationFrame( 0.0f )
  ,m_OneDayAnimationFrame( 0.0f )
  ,m_LocalAnimationFrameSize( 0.0f )
  ,m_OneDayAnimationFrameSize( 0.0f )
  ,m_MiddleModelDrawLine( MIDDLE_MODEL_DRAW_LINE_LIMITLESS )
  ,m_IsHideMeshSystemOff( false )
#if PM_DEBUG
  ,m_DebugLocalAnimationStop( false )
#endif


  // オブジェクト
  ,m_pTerrainLayoutData( NULL )
  ,m_pTerrainFactory( NULL )
  ,m_ppTerrainBlockTable( NULL )
  ,m_pDistantViewTable( NULL )
  ,m_DistantViewNum( 0 )
  ,m_pBgPartsManager( NULL )
  ,m_pCollsionScene( NULL )
  ,m_pLocalHeapUtilForStaticLodListener( NULL )
  ,m_pEnvManager( NULL )
{
  for( u32 i = 0; i < WallIndex::MAX; i++ )
  {
    m_pWallCollsionSceneTable[ i ] = NULL;
  }
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainManager::~TerrainManager( void )
{
  Terminate();
}

/**
* @brief 初期化処理
*
* @param  rInitDesc   初期化設定
*
* @return 無し
*/
void TerrainManager::Initialize( const InitializeDescription& rInitDesc )
{
  m_pTerrainLayoutData = rInitDesc.pTerrainLayoutData;
  m_pEnvManager        = rInitDesc.m_pEnvManager;

  // 転写
  for( u32 i = 0; i < WallIndex::MAX; i++ )
  {
    m_pWallCollsionSceneTable[ i ] = rInitDesc.m_pWallCollsionSceneTable[ i ];
  }

  // リスナー関連
  {
    // @note 各リスナーのリストは各ブロックが保持していますが、各ブロック毎にワークを保持するとメモリにかなりの無駄が発生するため、リスナーの総数及びリスト用のヒープの管理はマネージャーが行うようにしています

    // Lodのリスナー用のヒープを作成
    if( rInitDesc.m_StaticLodListenerMax > 0 )
    {
      m_pLocalHeapUtilForStaticLodListener = GFL_NEW( rInitDesc.m_pSystemHeap ) LocalHeapUtilForFixedSize(
        rInitDesc.m_pSystemHeap,
        sizeof(  ListNode< Block::LodChangeStaticListener* > ),
        rInitDesc.m_StaticLodListenerMax
        );
    }

  }


  // 地形生成機能作成
  {
    TerrainFactory::InitializeDescription initDesc;

    // Highモデル同時生成数設定
    if( m_pTerrainLayoutData->IsPlanMode() )
    {
      initDesc.m_HighModleWorkNum = HIGH_MODEL_LINE_MAX_PLAN * HIGH_MODEL_LINE_MAX_PLAN;
    }
    else
    {
      // @note 地形ブロックを圧縮形式に移行した際にメモリが不足してしまった際の対処。同時にロードされるブロックが2個の場合、不要な2個のメモリを使う2個に足しこむ
      // x、zのサイズが1の場合、2ブロックしか使わない
      if( rInitDesc.pTerrainLayoutData->GetBlockNumOfXAxis() == 1 || rInitDesc.pTerrainLayoutData->GetBlockNumOfZAxis() == 1 )
      {
        initDesc.m_HighModleWorkNum = HIGH_MODEL_LINE_MAX;
      }
      else
      {
        initDesc.m_HighModleWorkNum = HIGH_MODEL_LINE_MAX * HIGH_MODEL_LINE_MAX;
      }
    }

    // 1ブロックあたりのメモリ設定を取得
    initDesc.m_MemoryMode = GetMemoryMode( rInitDesc.m_MemoryMode, initDesc.m_HighModleWorkNum );
    initDesc.m_pAllocator = rInitDesc.m_pDeviceAllocator;
    initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
    initDesc.m_pHighModelDeviceHeap = rInitDesc.m_pDeviceHeap;
    initDesc.m_pDeviceHeap = rInitDesc.m_pDeviceHeap;
    initDesc.m_pTerrainLayoutData = rInitDesc.pTerrainLayoutData;
    initDesc.m_pAreaResourceDataAccessor = rInitDesc.pAreaResourceDataAccessor;
    initDesc.m_pFileManager = rInitDesc.m_pFileManager;
    initDesc.m_pEnvManager  = rInitDesc.m_pEnvManager;
    initDesc.m_pRenderingPipeLine = rInitDesc.m_pRenderingPipeLine;
    m_pTerrainFactory = GFL_NEW( rInitDesc.m_pSystemHeap ) TerrainFactory;
    m_pTerrainFactory->Initialize( initDesc );


    // アニメーションの長さを記憶
    m_LocalAnimationFrameSize = m_pTerrainFactory->GetLocalAnimationFrameSize();
    m_OneDayAnimationFrameSize = m_pTerrainFactory->GetOneDayAnimationFrameSize();
  }

  // コリジョンシーン作成
  {
    m_pCollsionScene = GFL_NEW( rInitDesc.m_pSystemHeap ) CollisionSceneNone;
    m_pCollsionScene->Setup( rInitDesc.m_pDeviceAllocator,rInitDesc.m_pDeviceHeap );
  }

  // 地形ブロック作成
  {
    gfl2::math::Vector3 blockStartPos;
    f32 blockSize = rInitDesc.pTerrainLayoutData->GetBlockSize();
    rInitDesc.pTerrainLayoutData->GetBlockStartPosition( &blockStartPos );

    m_ppTerrainBlockTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) TerrainBlock**[ rInitDesc.pTerrainLayoutData->GetBlockNumOfZAxis() ];
    for( u32 z = 0; z < rInitDesc.pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      m_ppTerrainBlockTable[ z ] = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) TerrainBlock*[ rInitDesc.pTerrainLayoutData->GetBlockNumOfXAxis() ];
      for( u32 x = 0; x < rInitDesc.pTerrainLayoutData->GetBlockNumOfXAxis(); x++ )
      {
        // モデルが登録されていない場合は、インスタンス自体を生成しません
        if( rInitDesc.pTerrainLayoutData->IsExist( x, z ) )
        {
          TerrainBlock::InitializeDescription initDesc;
          initDesc.m_X = x;
          initDesc.m_Z = z;
          initDesc.m_ReplaceNo = rInitDesc.pTerrainLayoutData->GetReplaceNo( x, z );    // @note workを参照した結果が返る
          initDesc.m_Pos.x = blockStartPos.x + x * blockSize + blockSize * 0.5f;
          initDesc.m_Pos.z = blockStartPos.z + z * blockSize + blockSize * 0.5f;
          initDesc.m_Pos.y = blockStartPos.y;
          initDesc.m_Size = rInitDesc.pTerrainLayoutData->GetBlockSize();
          initDesc.m_Pos = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
          initDesc.pTerrainFactory = m_pTerrainFactory;
          initDesc.pCollisionScene = m_pCollsionScene;
          initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
          initDesc.m_ppWallCollisionSceneTable = m_pWallCollsionSceneTable;
          initDesc.m_pCameraEyeCollsionScene = rInitDesc.m_pCameraEyeCollsionScene;

          m_ppTerrainBlockTable[ z ][ x ] = GFL_NEW( rInitDesc.m_pSystemHeap ) TerrainBlock;
          m_ppTerrainBlockTable[ z ][ x ]->Initialize( initDesc );
        }
        else
        {
          m_ppTerrainBlockTable[ z ][ x ] = NULL;
        }
      } // for x
    } // for z
  } // 地形ブロック作成

  // 遠景ブロック作成
  {
    // パックされている数だけ生成。アッパーではレイアウトデータに含めた方がよいしているが、レイアウトデータに設定が含まれる可能性がある
    m_DistantViewNum = m_pTerrainFactory->GetDistantViewModelNum(); 
    
    // 遠景は無いこともある
    if( m_DistantViewNum > 0 )
    {
      m_pDistantViewTable = GFL_NEW_ARRAY( rInitDesc.m_pSystemHeap ) TerrainDistantView [ m_DistantViewNum ];
      TerrainDistantView* pTarget = m_pDistantViewTable;
      for( u32 i = 0; i < m_DistantViewNum; i++, pTarget++ )
      {
        TerrainDistantView::InitializeDescription initDesc;
        initDesc.m_DistantViewNo = i;
        initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
        initDesc.m_pTerrainFactory = m_pTerrainFactory;

        pTarget->Initialize( initDesc );
      }
    }
  } // 遠景ブロック作成

  // 地形ブロック管理設定
  m_BlockControlMode = rInitDesc.m_BlockControlMode;
  if( m_pTerrainLayoutData->IsPlanMode() )
  {
    m_BlockControlMode = BlockControlMode::PLAN;
  }
  m_BlockControlModeDefault = m_BlockControlMode;


  // 背景パーツ管理を生成
  {
    TerrainBgPartsManager::InitializeDescription initDesc;
    initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
    initDesc.m_pTerrainFactory = m_pTerrainFactory;
    m_pBgPartsManager = GFL_NEW( rInitDesc.m_pSystemHeap ) TerrainBgPartsManager();
    m_pBgPartsManager->Initialize( initDesc );
    m_pBgPartsManager->SetCreateEnable( true );
  }


  GFL_RELEASE_PRINT( "BlockControlMode=\"%d\"", m_BlockControlMode );

}

/**
* @brief 初期化待ち処理
*
* @return true 完了 false 未完了
*/
b32 TerrainManager::InitializeWait( void )
{
  // arcファイルOPEN待ち
  return m_pTerrainFactory->InitializeWait();
}

/**
* @brief 地形ブロックのコントロール開始
* @note  フィールド初期化の流れで実行される
*
* @return 無し
*/
void TerrainManager::ControlStart( void )
{
  m_IsControlStart = true;

  // 地形ブロックに通知
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          (*ppBlock)->ControlStart();
        }

      } // for x
    } // for z
  }
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainManager::Terminate( void )
{
  // 背景パーツ管理を破棄
  {
    if( m_pBgPartsManager )
    {
      m_pBgPartsManager->Terminate();
      GFL_SAFE_DELETE( m_pBgPartsManager );
    }
  }

  // 遠景破棄
  {
    GFL_SAFE_DELETE_ARRAY( m_pDistantViewTable );
    m_DistantViewNum = 0;
  }

  // 地形ブロック破棄
  {
    if( m_pTerrainLayoutData )
    {
      for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
      {
        for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++ )
        {
          GFL_SAFE_DELETE( m_ppTerrainBlockTable[ z ][ x ] );
        }
        GFL_SAFE_DELETE_ARRAY( m_ppTerrainBlockTable[ z ] );
      }

      GFL_SAFE_DELETE_ARRAY( m_ppTerrainBlockTable );
    }
  }

  // コリジョンシーン破棄
  {
    GFL_SAFE_DELETE( m_pCollsionScene );
  }

  // 地形生成機能破棄
  {
    GFL_SAFE_DELETE( m_pTerrainFactory );
  }

  // 地形配置情報破棄
  {
    GFL_SAFE_DELETE( m_pTerrainLayoutData );
  }

  // リスナー関連
  {
    GFL_SAFE_DELETE( m_pLocalHeapUtilForStaticLodListener );
  }


  // 外部オブジェクトクリア
  for( u32 i = 0; i < WallIndex::MAX; i++ )
  {
    m_pWallCollsionSceneTable[ i ] = NULL;
  }

  m_pEnvManager        = NULL;
}

/**
* @brief 更新処理
*
* @param hour 時間
* @param minute 分
* @param second 秒
*
* @return 無し
*/
void TerrainManager::Update( s32 hour, s32 minute, s32 second, const gfl2::math::Matrix44* pViewProjMatrix )
{
  // 地形生成機能更新(highモデル動的ロード更新)
  {
    m_pTerrainFactory->Update();
  }

  // アニメーションフレーム更新
  {
    UpdateAnimationFrame( hour, minute, second );
  }

  // LOD変更制御
  if( m_IsControlStart )
  {
    UpdateLodControl();
  }

  // カリング用行列の作成
  gfl2::math::Matrix44 cullingMatrixTable[4];
  {
    gfl2::math::AABB::Util::CalcFrustumPlanes( cullingMatrixTable, *pViewProjMatrix );
  }

  // 地形ブロック更新
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          // 距離によるMiddleモデルの表示制限
          if( m_MiddleModelDrawLine != MIDDLE_MODEL_DRAW_LINE_LIMITLESS && 
            (
            static_cast<s32>(x) < m_HighBlockStartX - m_MiddleModelDrawLine
            || static_cast<s32>(z) < m_HighBlockStartZ - m_MiddleModelDrawLine
            || static_cast<s32>(x) > m_HighBlockStartX + ( HIGH_MODEL_LINE_MAX - 1 ) + m_MiddleModelDrawLine
            || static_cast<s32>(z) > m_HighBlockStartZ + ( HIGH_MODEL_LINE_MAX - 1 ) + m_MiddleModelDrawLine
            )
            )
          {
            // Middleモデル非表示
            (*ppBlock)->SetMiddleModelInvisibleForDistance( true );
          }
          else
          {
            // Middleモデル表示
            (*ppBlock)->SetMiddleModelInvisibleForDistance( false );
          }

          (*ppBlock)->Update( m_LocalAnimationFrame, m_OneDayAnimationFrame, &cullingMatrixTable[0] );
        }

      } // for x
    } // for z
  } // ブロック更新


  // 遠景更新
  {
    TerrainDistantView* ptr = m_pDistantViewTable;
    for( u32 i = 0; i < m_DistantViewNum; i++, ptr++ )
    {
      ptr->Update( m_LocalAnimationFrame, m_OneDayAnimationFrame, &cullingMatrixTable[0] );
    }
  }

  // 背景パーツ更新
  {
    m_pBgPartsManager->Update( m_LocalAnimationFrame, m_OneDayAnimationFrame, &cullingMatrixTable[0] );
  }
}

/**
* @brief カメラコリジョン前更新処理
*/
void TerrainManager::UpdateCameraCollisonPrev( void )
{
  for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
  {
    TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
    for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
    {
      if( *ppBlock )
      {
        (*ppBlock)->UpdateCameraCollisonPrev();
      }
    } // for x
  } // for z
}

/**
* @brief カメラコリジョン後更新処理
*/
void TerrainManager::UpdateCameraCollisonAfter( void )
{
  for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
  {
    TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
    for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
    {
      if( *ppBlock )
      {
        (*ppBlock)->UpdateCameraCollisonAfter();
      }
    } // for x
  } // for z
}

/**
* @brief フィールド開始用更新処理
* @note  事前にSetBlockControlPositionで自機座標を設定してください
*
* @return true Setup継続 false 完了
*/
b32 TerrainManager::UpdateStartSetup( void )
{
  // @note この関数は暗転中に実行される更新です、Highモデルの読み込みに関連する更新のみを行います


  // 地形生成機能更新(highモデル動的ロード更新)
  {
    m_pTerrainFactory->Update();
  }

  // LOD変更制御
  if( m_IsControlStart )
  {
    UpdateLodControl();
  }

  b32 setupContinue( false );

  // 地形ブロック更新
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          if( (*ppBlock)->UpdateStartSetup() )
          {
            setupContinue = true;
          }
        }

      } // for x
    } // for z
  } // ブロック更新
  
  // 背景パーツ更新
  {
    if( m_pBgPartsManager->UpdateStartSetup() )
    {
      setupContinue = true;
    }
  }

  // true Setup継続 false 完了
  return setupContinue;
}

/**
* @brief 終了開始リクエスト処理
* @note  終了処理の一番最初に実行される、UpdateWaitTerminate()がfalseを返した後にTerminate()が実行される
*
* @return 無し
*/
void TerrainManager::TerminateReq( void )
{
  m_pBgPartsManager->SetCreateEnable( false );
  m_pBgPartsManager->UnLoadBgPartsAll();
}

/**
* @brief 終了時可能まで待つ処理
* @note  Highモデルのロードが終わるまで待つ
*
* @return true Wait継続 false 完了
*/
b32 TerrainManager::UpdateWaitTerminate( void )
{
  // @note この関数は暗転中に実行される更新です、Highモデルの読み込みに関連する更新のみを行います
  // @note 今は開始更新と中身一緒、変更が必要になった際に整備する
  b32 setupContinue( false );

  // 地形生成機能更新(highモデル動的ロード更新)
  {
    m_pTerrainFactory->Update();
  }

  // LOD更新はしない

  // 地形ブロック更新
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          if( (*ppBlock)->UpdateEndSetup() )
          {
            setupContinue = true;
          }
        }
      } // for x
    } // for z
  } // ブロック更新

  // 背景パーツ更新
  {
    if( m_pBgPartsManager->UpdateEndSetup() )
    {
      setupContinue = true;
    }
  }

  // TerminateReqで破棄命令を出している
  if( m_pBgPartsManager->CountBgParts() > 0 )
  {
    setupContinue = true;
  }

  return setupContinue;
}

/**
* @brief 地形ブロックのLODが変更中か取得
*
* @return true 変更中 false 変更中ではない
*/
b32 TerrainManager::IsBlockLodChanging( void ) const
{
  // 地形ブロック全チェック
  for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
  {
    TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
    for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
    {
      if( *ppBlock )
      {
        if( (*ppBlock)->IsLodChanging() )
        {
          return true;
        }
      }
    } // for x
  } // for z

  return false;
}

/**
* @brief 地形ブロックのLOD変更通知を受け取るリスナーを登録
* @note  リスナーの寿命は登録側で管理してください
* @note  今は1ブロックあたり10個しかリスナーをもてないが、今後固定長ではなく動的的確保にする可能性がある
*
* @param  pListner  登録するリスナー
*
* @return 成否
*/
b32 TerrainManager::RegistBlockListener( Block::LodChangeStaticListener* pListener )
{
  if( m_pLocalHeapUtilForStaticLodListener == NULL || m_pLocalHeapUtilForStaticLodListener->IsFull() || pListener->isRegist() )
  {
    GFL_ASSERT( 0 );  // 設定よりも多くのリスナーがAddされた、レジスト済みのリスナーが渡された、開発中に気づくためのアサート
    return false;
  }

  // 登録ブロックを取得
  gfl2::math::Vector3 listenerPos;
  if( pListener->GetControlPosition( &listenerPos ) )
  {
    s32 x;
    s32 z;

    if( this->GetBlockIndex( listenerPos, &x, &z ) && IsBlockInstanceExist( x, z ) )
    {
      if( m_ppTerrainBlockTable[z][x]->RegistListener( pListener, m_pLocalHeapUtilForStaticLodListener->GetHeap() ) )
      {
        m_pLocalHeapUtilForStaticLodListener->IncCount();
        return true;
      }
    }
  }

  return false;
}

/**
* @brief 地形ブロックのLOD変更通知を受け取るリスナーを登録削除
* @note  リスナーの寿命は登録側で管理してください
*
* @param  pListner  登録削除するリスナー
*
* @return 成否
*/
b32 TerrainManager::UnRegistBlockListener( Block::LodChangeStaticListener* pListener )
{
  if( m_pLocalHeapUtilForStaticLodListener == NULL || m_pLocalHeapUtilForStaticLodListener->GetCount() == 0 || !pListener->isRegist() )
  {
    GFL_ASSERT( 0 );  // 開発中に気づくためのアサート
    return false;
  }


  // 登録ブロックを取得
  u32 x;
  u32 z;
  pListener->getRegistBlock( &x, &z );

  // 登録削除
  if( IsBlockInstanceExist( x, z ) )
  {
    if( m_ppTerrainBlockTable[z][x]->UnRegistListener( pListener ) )
    {
      m_pLocalHeapUtilForStaticLodListener->DecCount();
      return true;
    }
  }

  return false;
}

/**
* @brief 指定座標の地形ブロック番号を取得する(座標が枠外の場合は枠内に含める版)
* @note  地形ブロック番号が枠内であっても、インスタンスが存在しない場合がある(モデルが一切登録されていない場合)
*
* @param  rPos 座標
* @param  pX   X軸番号格納先
* @param  pZ   Z軸番号格納先
*
* @return true 取得成功 false 失敗(未セットアップ状態で実行した)
*/
b32 TerrainManager::GetBlockIndexClamp( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const
{
  if( m_pTerrainLayoutData == NULL || m_pTerrainLayoutData->IsValid() == false )
  {
    return false;
  }

  gfl2::math::Vector3 startPosition;
  m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
  f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
  f32 localX = rPos.x - startPosition.x;
  f32 localZ = rPos.z - startPosition.z;
  s32 workBlockX = localX / blockSize;
  s32 workBlockZ = localZ / blockSize;
  s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() - 1 ) );
  s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() - 1 ) );

  *pX = blockX;
  *pZ = blockZ;
  return true;
}

/**
* @brief 指定座標の地形ブロック番号を取得する
* @note  地形ブロック番号が枠内であっても、インスタンスが存在しない場合がある(モデルが一切登録されていない場合)
*
* @param  rPos 座標
* @param  pX   X軸番号格納先
* @param  pZ   Z軸番号格納先
*
* @return true 取得成功 false 失敗(未セットアップ状態　or 枠外)
*/
b32 TerrainManager::GetBlockIndex( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const
{
  if( m_pTerrainLayoutData == NULL || m_pTerrainLayoutData->IsValid() == false )
  {
    return false;
  }

  gfl2::math::Vector3 startPosition;
  m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
  f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
  f32 localX = rPos.x - startPosition.x;
  f32 localZ = rPos.z - startPosition.z;
  s32 workBlockX = localX / blockSize;
  s32 workBlockZ = localZ / blockSize;

  // 枠外
  if( workBlockX < 0 || workBlockZ < 0 || 
    workBlockX >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || 
    workBlockZ >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() )
    )
  {
    return false;
  }

  *pX = workBlockX;
  *pZ = workBlockZ;
  return true;
}


/**
* @brief 指定番号の地形ブロックの実体が存在するか取得する
*
* @param  x X軸番号
* @param  z Z軸番号
*
* @return true 存在する false 存在しない
*/
b32 TerrainManager::IsBlockInstanceExist( s32 x, s32 z ) const
{
  if( m_pTerrainLayoutData == NULL || m_ppTerrainBlockTable == NULL )
  {
    return false;
  }

  if( 
    x < 0 || x >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || 
    z < 0 || z >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() )
    )
  {
    return false;
  }

  return ( m_ppTerrainBlockTable[z][x] != NULL );
}

/** 
* @brief 指定番号の地形ブロックの実体を取得
*
* @param  x X軸番号
* @param  z Z軸番号
*
* @return 地形ブロックの実体
* @retval NULL 存在しない
*/
const TerrainBlock* TerrainManager::GetBlockInstance( s32 x, s32 z ) const
{
  if( m_pTerrainLayoutData == NULL || m_ppTerrainBlockTable == NULL )
  {
    return NULL;
  }

  if( 
    x < 0 || x >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || 
    z < 0 || z >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() )
    )
  {
    return NULL;
  }

  return ( m_ppTerrainBlockTable[z][x] );
}

/**
 *  @brief  草モデルの取得
 *  @param  x           ブロックX座標
 *  @param  z           ブロックZ座標
 *  @param  floorIndex  フロアインデックス
 */
KusaModel::FieldKusaModel* TerrainManager::GetKusaModel( s32 x, s32 z, s32 floorIndex )
{
  if( this->IsBlockInstanceExist( x, z ) )
  {
    return m_ppTerrainBlockTable[z][x]->GetKusaModel( floorIndex );
  }
  return NULL;
}

/**
 *  @brief  草モデルのフロア数を取得
 */
u32 TerrainManager::GetKusaModelFloorNum(void) const
{
  return m_pTerrainFactory->GetEncountGrassModelTableNum();
}

/**
* @brief 地形ブロックの制御点を座標で指定する
* @note  BlockControlMode::Fixになる
* @note  変更リクエストの発行まで行う
* 
* @param  rPos  座標
*
* @return 無し
*/
void TerrainManager::SetExternalBlockControlPointByPosition( const gfl2::math::Vector3& rPos )
{
  // 制御モード変更
  m_BlockControlMode = BlockControlMode::FIX;

  // 地形ブロック制御点の計算
  s32 highBlockStartX = 0;
  s32 highBlockStartZ = 0;
  CalcBlockControlPointForNormal( rPos, &highBlockStartX, &highBlockStartZ );

  // 地形ブロック変更リクエスト発行
  ChangeBlockControl( highBlockStartX, highBlockStartZ );
}

/**
* @brief 地形ブロックの制御点をブロック番号で指定する
* @note  BlockControlMode::Fixになる
* @note  変更リクエストの発行まで行う
* 
* @param  x           ブロックX座標
* @param  z           ブロックZ座標
*
* @return 無し
*/
void TerrainManager::SetExternalBlockControlPointByBlockNo( u32 x, u32 z )
{
  // 制御モード変更
  m_BlockControlMode = BlockControlMode::FIX;

  // 地形ブロック変更リクエスト発行
  ChangeBlockControl( x, z );
}

/**
* @brief 地形ブロックの制御点の座標指定を解除する
* @note  BlockControlModeが初期値になる
* @note  変更リクエストの発行まで行う
* 
* @return 無し
*/
void TerrainManager::ClearExternalBlockControlPoint( void )
{
  // 制御モードを戻す
  m_BlockControlMode = m_BlockControlModeDefault;

  // 地形ブロック変更リクエスト発行
  UpdateLodControl();
}

/**
* @brief 指定座標の地形ブロックがHighモデルか取得する
* @note  見た目、コリジョンは地形ブロック境界線からはみでる事を許容しています
* 
* @return true Highモデル false Highモデルでは無い
*/
b32 TerrainManager::IsHighBlock( const gfl2::math::Vector3& rPos ) const
{
  // 座標が所属する地形ブロックを計算
  s32 x, z;
  if( !GetBlockIndex( rPos, &x, &z ) )
  {
    // 枠外
    return false;
  }

  // 地形ブロックの状態を取得
  const TerrainBlock* pTerrainBlock = GetBlockInstance( x, z );
  if( pTerrainBlock && pTerrainBlock->GetLod() == Block::LodType::HIGH )
  {
    return true;
  }

  return false;
}

/**
* @brief 指定座標の地形ブロックがHighモデルか取得する(Trg)
* @note  見た目、コリジョンは地形ブロック境界線からはみでる事を許容しています
* 
* @return true Highモデル false Highモデルでは無い
*/
b32 TerrainManager::IsHighBlockTrg( const gfl2::math::Vector3& rPos ) const
{
  // 座標が所属する地形ブロックを計算
  s32 x, z;
  if( !GetBlockIndex( rPos, &x, &z ) )
  {
    // 枠外
    return false;
  }

  // 地形ブロックの状態を取得
  const TerrainBlock* pTerrainBlock = GetBlockInstance( x, z );
  if( pTerrainBlock && pTerrainBlock->GetLod() == Block::LodType::HIGH && pTerrainBlock->GetLodPrev() != Block::LodType::HIGH )
  {
    return true;
  }

  return false;
}

/**
* @brief 指定の地形ブロックのリプレイスNoを取得する
* 
*/
u32 TerrainManager::GetReplaceNo( s32 x, s32 z ) const
{
  // 地形ブロックの状態を取得
  const TerrainBlock* pTerrainBlock = GetBlockInstance( x, z );
  if( pTerrainBlock )
  {
    return pTerrainBlock->GetReplaceNo();
  }

  return 0;
}

/**
* @brief Middleモデルの表示ライン制限を設定
*
* @param  line  表示ライン数
*/
void TerrainManager::SetMiddleModelDrawLineLimit( s32 line )
{
  m_MiddleModelDrawLine = line;
}

/**
* @brief Middleモデルの表示ライン制限をクリア(=全表示)
*/
void TerrainManager::ClearMiddleModelDrawLineLimit( void )
{
  m_MiddleModelDrawLine = MIDDLE_MODEL_DRAW_LINE_LIMITLESS;
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を有効化する
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void TerrainManager::HideMeshSystemOn( b32 oneTime )
{
  m_IsHideMeshSystemOff = false;

  for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
  {
    TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
    for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
    {
      if( *ppBlock )
      {
        (*ppBlock)->HideMeshSystemOn( oneTime );
      }
    } // for x
  } // for z
}

/**
* @brief カメラ接触でメッシュの非表示にする機能を無効化する
*
* @param oneTime true:透過遷移を挟さまない false:透過遷移を挟む
*/
void TerrainManager::HideMeshSystemOff( b32 oneTime )
{
  m_IsHideMeshSystemOff = true;
  
  for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
  {
    TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
    for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
    {
      if( *ppBlock )
      {
        (*ppBlock)->HideMeshSystemOff( oneTime );
      }

    } // for x
  } // for z
}

#if PM_DEBUG
void TerrainManager::SetBlockControlSkip( b32 flag  )
{
  if( flag )
  {
    // 制御モード変更
    m_BlockControlMode = BlockControlMode::FIX;
  }
  else
  {
    // 制御モードを戻す
    m_BlockControlMode = m_BlockControlModeDefault;
  }
}

TerrainBlock* TerrainManager::GetBlock( s32 x, s32 z )
{
  if( m_pTerrainLayoutData == NULL || m_ppTerrainBlockTable == NULL )
  {
    return NULL;
  }

  if( 
    x < 0 || x >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || 
    z < 0 || z >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() )
    )
  {
    return NULL;
  }

  return m_ppTerrainBlockTable[z][x];
}

void TerrainManager::UpdateDrawTagInfo( Debug::FieldDebugMenu* pDebugMenu )
{
  s32 drawTagCountForHighModel = 0;
  s32 drawTagCountForMiddleModel = 0;
  s32 drawTagCountForDistantViewModel = 0;
  s32 highModelIndex = 0;

  for( u32 i = 0; i < Debug::FieldDebugMenu::DRAW_TAG_TERRAIN_HIGH_MODEL_SIZE; i++ )
  {
     pDebugMenu->SetDrawTagNumForTerrainHighModelIndex( i, 0, 0, 0);
  }


  // 地形ブロック更新
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          if( (*ppBlock)->GetLod() == Block::LodType::HIGH )
          {
            if( (*ppBlock)->IsVisible() == false )
            {
              continue;
            }
            pDebugMenu->SetDrawTagNumForTerrainHighModelIndex( highModelIndex, x, z, (*ppBlock)->GetDrawTagNum() );
            highModelIndex++;

            drawTagCountForHighModel += (*ppBlock)->GetDrawTagNum();
          }
          else
          {
            if( (*ppBlock)->IsVisible() )
            {
              drawTagCountForMiddleModel += (*ppBlock)->GetDrawTagNum();
            }
          }
        }

      } // for x
    } // for z
  } // ブロック更新

  // 遠景更新
  {
    TerrainDistantView* ptr = m_pDistantViewTable;
    for( u32 i = 0; i < m_DistantViewNum; i++, ptr++ )
    {
      if( ptr->IsVisible() == false )
      {
        continue;
      }

      drawTagCountForDistantViewModel += ptr->GetDrawTagNum();
    }
  }

  pDebugMenu->SetDrawTagNumForTerrainHighModel( drawTagCountForHighModel );
  pDebugMenu->SetDrawTagNumForTerrainMiddleModel( drawTagCountForMiddleModel );
  pDebugMenu->SetDrawTagNumForTerrainDistantViewModel( drawTagCountForDistantViewModel );

}

//
// @fix CONNMCat[582]:攻略本用マップ撮影機能、ローカルアニメを特定のフレームで停止する機能
//

/**
* @brief ローカルアニメを指定フレームで一時停止させる
*
* @param frame 指定フレーム
*
* @return 無し
*/
void TerrainManager::DebugSuspendLocalAnimation( f32 frame )
{
  frame = gfl2::math::Clamp( frame, 0.0f, m_LocalAnimationFrameSize );
  m_LocalAnimationFrame = frame;
  m_DebugLocalAnimationStop = true;
}

/**
* @brief ローカルアニメの一時停止を解除
*
* @return 無し
*/
void TerrainManager::DebugResumeLocalAnimation( void )
{
  m_DebugLocalAnimationStop = false;
}

#endif // PM_DEBUG

//
// private
//

/**
*  @brief  アニメーションのフレーム値の更新
*
*  @param hour 時間
*  @param minute 分
*  @param second 秒
*
*  @return 無し
*/
void TerrainManager::UpdateAnimationFrame( s32 hour, s32 minute, s32 second )
{
  // ローカルアニメ
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::UPDATE_TERRAIN_ANIM_LOCAL ) && !m_DebugLocalAnimationStop )
#endif
  {
    f32 frameSize = m_LocalAnimationFrameSize;

    if( frameSize > 0 )
    {
      m_LocalAnimationFrame += 1.0f;

      if( m_LocalAnimationFrame > frameSize )
      {
        m_LocalAnimationFrame -= frameSize;
      }
    }
  }

  // 1日アニメ
#if PM_DEBUG
  if( FIELD_DEBUG_SWITCH( Field::Debug::DebugTypes::Switch::UPDATE_TERRAIN_ANIM_ONE_DAY ) )
#endif
  {
    f32 frameSize = m_OneDayAnimationFrameSize;

    if( frameSize > 0 )
    {
      f32 totalSecond = ( 24 * 60 * 60 );
      f32 nowSecond = ( hour * 60 * 60 ) + ( minute * 60 ) + second;

      f32 t = nowSecond / totalSecond;
      t = gfl2::math::Clamp( t, 0.0f, 1.0f );

      m_OneDayAnimationFrame = frameSize * t;
    }
  }
}

/**
*  @brief  LOD制御更新
*
*  @return 無し
*/
void TerrainManager::UpdateLodControl( void )
{
  s32 highBlockStartX = m_HighBlockStartX;
  s32 highBlockStartZ = m_HighBlockStartZ;


  // 基準点更新(一番左上にあるブロック)
  // highBlockStartX、highBlockStartZを計算
  switch( m_BlockControlMode )
  {
  case BlockControlMode::NORMAL:
    {
      // ブロックの半分の地点で制御点を切り替える
      CalcBlockControlPointForNormal( m_BlockControlPosition, &highBlockStartX, &highBlockStartZ );
      break;
    }

  case BlockControlMode::SP_ROOM:
    {
      // 2x2ブロック単位のブロック境界線で制御点を切り替える
      
      // 現在いるブロックを算出
      gfl2::math::Vector3 startPosition;
      m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
      f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
      f32 localX = m_BlockControlPosition.x - startPosition.x;
      f32 localZ = m_BlockControlPosition.z - startPosition.z;

      f32 workBlockX = localX / blockSize;
      f32 workBlockZ = localZ / blockSize;
      
      s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() - 1 ) );
      s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() - 1 ) );

      // 2の倍数化
      highBlockStartX = ( blockX / 2 ) * 2;
      highBlockStartZ = ( blockZ / 2 ) * 2;

      break;
    }

  case BlockControlMode::PLAN:
    {
      highBlockStartX = 0;
      highBlockStartZ = 0;
      break;
    }

  case BlockControlMode::FIX:
    {
      break;
    }
  }

  // ブロック制御更新
  ChangeBlockControl( highBlockStartX, highBlockStartZ );
}

/**
*  @brief  地形ブロック制御点を変更
*
*  @param  x           ブロックX座標
*  @param  z           ブロックZ座標
*
*  @return true 実際に変更された  false 現在と同じ設定なので何もしなかった
*/
b32 TerrainManager::ChangeBlockControl( s32 x, s32 z )
{
  // 現在の制御点から変更無し
  if( m_HighBlockStartX == x && m_HighBlockStartZ == z )
  {
    return false;
  }

  s32 highBlockStartX = x;
  s32 highBlockStartZ = z;
  s32 blockHighModelNum = 0;

  // 各軸毎のHighモデルロード数
  if( m_pTerrainLayoutData->IsPlanMode() )
  {
    blockHighModelNum = HIGH_MODEL_LINE_MAX_PLAN;
  }
  else
  {
    blockHighModelNum = HIGH_MODEL_LINE_MAX;
  }

  // 前回highモデルだったブロックに対し、middle化するリクエストを発行
  if( m_HighBlockStartX != BLOCK_INDEX_ERROR && m_HighBlockStartZ != BLOCK_INDEX_ERROR ) // 初回チェック
  {
    for( s32 z = m_HighBlockStartZ; z < m_HighBlockStartZ + blockHighModelNum; z++ )
    {
      if( z >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() ) || z < 0 )
      {
        continue;
      }

      for( s32 x = m_HighBlockStartX; x < m_HighBlockStartX + blockHighModelNum; x++ )
      {
        if( x >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || x < 0 || IsBlockInstanceExist( x, z ) == false )
        {
          continue;
        }

        m_ppTerrainBlockTable[z][x]->ChangeLod( Block::LodType::MIDDLE );
      }
    }
  }

  m_HighBlockStartX = highBlockStartX;
  m_HighBlockStartZ = highBlockStartZ;
  GFL_RELEASE_PRINT( "HighModel Block Update x=%d z=%d \n", m_HighBlockStartX, m_HighBlockStartZ );
  GFL_RELEASE_PRINT( "m_BlockControlPosition x=%f y=%f z=%f \n", m_BlockControlPosition.x, m_BlockControlPosition.y, m_BlockControlPosition.z );

  //　highモデル化するリクエストを発行
  for( s32 z = m_HighBlockStartZ; z < m_HighBlockStartZ + blockHighModelNum; z++ )
  {
    if( z >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() ) || z < 0 )
    {
      continue;
    }

    for( s32 x = m_HighBlockStartX; x < m_HighBlockStartX + blockHighModelNum; x++ )
    {
      if( x >= static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) || x < 0 || IsBlockInstanceExist( x, z ) == false )
      {
        continue;
      }

      m_ppTerrainBlockTable[z][x]->ChangeLod( Block::LodType::HIGH );
    } // for x
  } // for z

  // @fix GFCHECK[8230]:ワーニング対処。適切にreturnするように修正、対処時点で参照箇所は無し。
  return true;
}

/**
*  @brief  地形ブロック制御点を計算する(通常モード)
*
*  @param  rPos     指定座標
*  @param  pX       格納先(ブロックX座標)
*  @param  pZ       格納先(ブロックZ座標)
*
*  @return 無し
*/

void TerrainManager::CalcBlockControlPointForNormal( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const
{ 
  // ブロックの半分の地点で制御点を切り替える

  // 現在いるブロックを算出
  gfl2::math::Vector3 startPosition;
  m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
  f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
  f32 localX = rPos.x - startPosition.x;
  f32 localZ = rPos.z - startPosition.z;

  f32 workBlockX = localX / blockSize;
  f32 workBlockZ = localZ / blockSize;

  s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() - 1 ) );
  s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() - 1 ) );

  // 現在いるブロックの半分未満なら、手間のブロックが制御点
  s32 highBlockStartX = blockX;
  s32 highBlockStartZ = blockZ;

  if( highBlockStartX > 0 && workBlockX - blockX < 0.5f )
  {
    highBlockStartX -= 1;
  }

  if( highBlockStartZ > 0 && workBlockZ - blockZ < 0.5f )
  {
    highBlockStartZ -= 1;
  }

  *pX = highBlockStartX;
  *pZ = highBlockStartZ;
}

/**
*  @brief  地形ブロック用メモリのモードを取得
*
*  @param  memMode            エリアで指定しているメモリモード
*  @param  highModleWorkNum   Highモデル同時存在数
*
*  @return メモリモード
*/
MemoryMode::Value TerrainManager::GetMemoryMode( MemoryMode::Value memMode, u32 highModleWorkNum ) const
{
  // 2ブロック(2倍)モード
  if( highModleWorkNum == 2 )
  {
    switch( memMode )
    {
    case MemoryMode::NORMAL:
      {
        return MemoryMode::NORMAL_2X;
      }
    }
  }

  // 1ブロック(4倍)モード
  if( highModleWorkNum == 1 )
  {
    switch( memMode )
    {
    case MemoryMode::NORMAL:
      {
        return MemoryMode::NORMAL_4X;
      }
    }
  }

  return memMode;
}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );