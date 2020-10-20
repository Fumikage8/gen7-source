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
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainManager.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainFactory.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainLayoutData.h"
#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainBlock.h"
///#include "ViewerProc/include/FieldRo/Terrain/FieldTerrainDistantView.h"
///#include "ViewerProc/include/FieldRo/Block/FieldTerrainBlockLodChangeStaticListener.h"

#include "ViewerProc/include/FieldStatic/Collision/CollisionSceneNone.h"

// field debug
///#include "Field/FieldRo/include/Debug/FieldDebugTypes.h"


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static s32 BLOCK_INDEX_ERROR = -1000;

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainManager::TerrainManager( void ) :

  // 制御
  m_BlockControlPosition( 0.0f, 0.0f, 0.0f )
  ,m_HighBlockStartX( BLOCK_INDEX_ERROR )
  ,m_HighBlockStartZ( BLOCK_INDEX_ERROR )

  // オブジェクト
  ,m_pTerrainLayoutData( NULL )
  ,m_pTerrainFactory( NULL )
  ,m_ppTerrainBlockTable( NULL )
  ,m_pDistantViewTable( NULL )
  ,m_DistantViewNum( 0 )
  ,m_pCollsionScene( NULL )
  ,m_pEnvManager( NULL )
{
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
  m_pWallCollsionScene = rInitDesc.m_pWallCollsionScene;
  m_pEnvManager        = rInitDesc.m_pEnvManager;

	// コリジョンシーン作成
  {
    m_pCollsionScene = GFL_NEW( rInitDesc.m_pSystemHeap ) CollisionSceneNone;
    m_pCollsionScene->Setup( rInitDesc.m_pDeviceAllocator, rInitDesc.m_pDeviceHeap );
  }

  // 地形生成機能作成
  {
    TerrainFactory::InitializeDescription initDesc;
    initDesc.m_HighModleWorkNum = 4;
    initDesc.m_pAllocator = rInitDesc.m_pDeviceAllocator;
    initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
    initDesc.m_pHighModelDeviceHeap = rInitDesc.m_pDeviceHeap;
    initDesc.m_pTerrainLayoutData = rInitDesc.pTerrainLayoutData;
    initDesc.m_pAreaResourceDataAccessor = rInitDesc.pAreaResourceDataAccessor;
    ///initDesc.m_pFileManager = rInitDesc.m_pFileManager;
    initDesc.m_pEnvManager  = rInitDesc.m_pEnvManager;
  	initDesc.m_pRenderingPipeLine = rInitDesc.m_pRenderingPipeLine;
    m_pTerrainFactory = GFL_NEW( rInitDesc.m_pSystemHeap ) TerrainFactory;
    m_pTerrainFactory->Initialize( initDesc, m_pCollsionScene );
  }

#if 0
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
        if( rInitDesc.pTerrainLayoutData->IsExist( x, z ) )
        {
#if 0
          if( x != 8 || z != 4 )
          {
            m_ppTerrainBlockTable[ z ][ x ] = NULL;
          }
          else
#endif
          {
            TerrainBlock::InitializeDescription initDesc;
            initDesc.m_X = x;
            initDesc.m_Z = z;
            initDesc.m_Pos.x = blockStartPos.x + x * blockSize + blockSize * 0.5f;
            initDesc.m_Pos.z = blockStartPos.z + z * blockSize + blockSize * 0.5f;
            initDesc.m_Pos.y = blockStartPos.y;
            initDesc.m_Pos = gfl2::math::Vector3( 0.0f, 0.0f, 0.0f );
            initDesc.pTerrainFactory = m_pTerrainFactory;
            initDesc.pCollisionScene = m_pCollsionScene;
            initDesc.m_LodChangeStaticListenerMax = 10;
            initDesc.m_pSystemHeap = rInitDesc.m_pSystemHeap;
            initDesc.m_pWallCollisionScene = m_pWallCollsionScene;

            m_ppTerrainBlockTable[ z ][ x ] = GFL_NEW( rInitDesc.m_pSystemHeap ) TerrainBlock;
            m_ppTerrainBlockTable[ z ][ x ]->Initialize( initDesc );
          }
        }
        else
        {
          m_ppTerrainBlockTable[ z ][ x ] = NULL;
        }
      }
    }
  }
#endif //@saito_del
  // 遠景ブロック作成
  {
    m_DistantViewNum = 0;///m_pTerrainFactory->GetDistantViewModelNum(); // @todo 本当はレイアウトから取得したい@saito_change
    /**
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
		*///@saito_del
  }

}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainManager::Terminate( void )
{
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

  // 外部オブジェクトクリア
  m_pWallCollsionScene = NULL;
  m_pEnvManager        = NULL;
}

/**
* @brief 更新処理
*
* @return 無し
*/
void TerrainManager::Update( void )
{
  // 地形生成機能更新(highモデル動的ロード更新)
  {
    m_pTerrainFactory->Update();
  }
#if 0
  // ブロック制御更新
#if 0
  if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_ONLY ) == 0 )
#endif
  {
    // 現在のブロックを決定
    gfl2::math::Vector3 startPosition;
    m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
    f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
    f32 localX = m_BlockControlPosition.x - startPosition.x;
    f32 localZ = m_BlockControlPosition.z - startPosition.z;

    f32 workBlockX = localX / blockSize;
    f32 workBlockZ = localZ / blockSize;

    s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) );
    s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() ) );

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

    if( m_HighBlockStartX != highBlockStartX || m_HighBlockStartZ != highBlockStartZ )
    {

      s32 blockHighModelNum = 2;

      // 前回highモデルだったブロックはmiddle
      if( m_HighBlockStartX != BLOCK_INDEX_ERROR && m_HighBlockStartZ != BLOCK_INDEX_ERROR )
      {
        for( s32 z = m_HighBlockStartZ; z < m_HighBlockStartZ + blockHighModelNum; z++ )
        {
          if( z >= m_pTerrainLayoutData->GetBlockNumOfZAxis() || z < 0 )
          {
            continue;
          }

          for( s32 x = m_HighBlockStartX; x < m_HighBlockStartX + blockHighModelNum; x++ )
          {
            if( x >= m_pTerrainLayoutData->GetBlockNumOfXAxis() || x < 0 || IsBlockInstanceExist( x, z ) == false )
            {
              continue;
            }

            // @todo 今回もhighモデルの場合の対応、今はリクエストを設定しているだけなのでこれでよい
            m_ppTerrainBlockTable[z][x]->ChangeLod( Block::LodType::MIDDLE );
          }
        }
      }

      m_HighBlockStartX = highBlockStartX;
      m_HighBlockStartZ = highBlockStartZ;
      GFL_RELEASE_PRINT( "HighModel Block Update x=%d z=%d \n", m_HighBlockStartX, m_HighBlockStartZ );
      GFL_RELEASE_PRINT( "m_BlockControlPosition x=%f y=%f z=%f \n", m_BlockControlPosition.x, m_BlockControlPosition.y, m_BlockControlPosition.z );

      // highモデル化
      for( s32 z = m_HighBlockStartZ; z < m_HighBlockStartZ + blockHighModelNum; z++ )
      {
        if( z >= m_pTerrainLayoutData->GetBlockNumOfZAxis() || z < 0 )
        {
          continue;
        }

        for( s32 x = m_HighBlockStartX; x < m_HighBlockStartX + blockHighModelNum; x++ )
        {
          if( x >= m_pTerrainLayoutData->GetBlockNumOfXAxis() || x < 0 || IsBlockInstanceExist( x, z ) == false )
          {
            continue;
          }

          m_ppTerrainBlockTable[z][x]->ChangeLod( Block::LodType::HIGH );
        }
      }
    }
  }
#if 0
  else
  {

    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          (*ppBlock)->ChangeLod( Block::LodType::LOW );
        }
      }
    }

    // 
    s32 targetX = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_ONLY_HIGH_X );
    s32 targetZ = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_ONLY_HIGH_Z );
    if( IsBlockInstanceExist( targetX, targetZ ) )
    {
      m_ppTerrainBlockTable[ targetZ ][ targetX ]->ChangeLod( Block::LodType::HIGH );
    }

    targetX = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_ONLY_LOD_X );
    targetZ = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_ONLY_LOD_Z );
    if( IsBlockInstanceExist( targetX, targetZ ) )
    {
      m_ppTerrainBlockTable[ targetZ ][ targetX ]->ChangeLod( Block::LodType::MIDDLE );
    }
  }
#endif

  // ブロック更新　@todo 最適化非表示ブロックは更新自体走らなくする？
  {
    for( u32 z = 0; z < m_pTerrainLayoutData->GetBlockNumOfZAxis(); z++ )
    {
      TerrainBlock** ppBlock = m_ppTerrainBlockTable[ z ];
      for( u32 x = 0; x < m_pTerrainLayoutData->GetBlockNumOfXAxis(); x++, ppBlock++ )
      {
        if( *ppBlock )
        {
          (*ppBlock)->Update();
        }
      }
    }
  }
#endif //@saito_del
}
#if 0
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
  // 登録ブロックを取得
  gfl2::math::Vector3 listenerPos;
  if( pListener->GetControlPosition( &listenerPos ) )
  {
    s32 x;
    s32 z;

    if( this->GetBlockIndex( listenerPos, &x, &z ) && IsBlockInstanceExist( x, z ) )
    {
      return m_ppTerrainBlockTable[x][z]->RegistListener( pListener );
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
  // @todo 座標が変わっているかも対策
  // 登録ブロックを取得
  gfl2::math::Vector3 listenerPos;
  if( pListener->GetControlPosition( &listenerPos ) )
  {
    s32 x;
    s32 z;

    if( this->GetBlockIndex( listenerPos, &x, &z ) && IsBlockInstanceExist( x, z ) )
    {
      return m_ppTerrainBlockTable[x][z]->UnRegistListener( pListener );
    }
  }

  return false;
}

b32 TerrainManager::GetBlockIndex( const gfl2::math::Vector3& rPos, s32* pX, s32* pZ ) const
{
  if( m_pTerrainLayoutData == NULL || m_pTerrainLayoutData->IsValid() == false )
  {
    return false;
  }

  // @note 枠内におさめる
  gfl2::math::Vector3 startPosition;
  m_pTerrainLayoutData->GetBlockStartPosition( &startPosition );
  f32 blockSize = m_pTerrainLayoutData->GetBlockSize();
  f32 localX = m_BlockControlPosition.x - startPosition.x;
  f32 localZ = m_BlockControlPosition.z - startPosition.z;
  s32 workBlockX = localX / blockSize;
  s32 workBlockZ = localZ / blockSize;
  s32 blockX = gfl2::math::Clamp( static_cast<s32>( workBlockX ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfXAxis() ) );
  s32 blockZ = gfl2::math::Clamp( static_cast<s32>( workBlockZ ), 0, static_cast<s32>( m_pTerrainLayoutData->GetBlockNumOfZAxis() ) );

  *pX = blockX;
  *pZ = blockZ;
  return true;
}

b32 TerrainManager::IsBlockInstanceExist( s32 x, s32 z ) const
{
  if( 
    x < 0 || x >= m_pTerrainLayoutData->GetBlockNumOfXAxis() || 
    z < 0 || z >= m_pTerrainLayoutData->GetBlockNumOfZAxis()
    )
  {
    return false;
  }

  return ( m_ppTerrainBlockTable[z][x] != NULL );
}
#endif //@saito_del
GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );