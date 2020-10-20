/**
* @brief  フィールドの遠景1つ分クラス
* @file   TerrainDistantView.h
* @author ikuta_junya
* @data   2015.04.19
*/

// field
#include "Field/FieldRo/include/Terrain/FieldTerrainDistantView.h"
#include "Field/FieldRo/include/Terrain/FieldTerrainFactory.h"
#include "Field/FieldRo/include/Terrain/Block/FieldTerrainBlockTypes.h"

// field debug
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Terrain );

const static u32 INVISIBLE_CULLING = 0x01;
const static u32 INVISIBLE_DEBUG_DRAW_SWITCH = 0x01 << 1;

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TerrainDistantView::TerrainDistantView( void ) :
  // 制御
  m_DistantViewNo( 0 )
  ,m_InvisibleBit( 0 )
  // オブジェクト
  ,m_pModel( NULL )
  // 外部オブジェクト
  ,m_pTerrainFactory( NULL )
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TerrainDistantView::~TerrainDistantView( void )
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
void TerrainDistantView::Initialize( const InitializeDescription& rInitDesc )
{
  m_pTerrainFactory = rInitDesc.m_pTerrainFactory;

  m_DistantViewNo = rInitDesc.m_DistantViewNo;
  m_pModel = m_pTerrainFactory->CreateDistantViewModel( m_DistantViewNo );
}

/**
* @brief 終了処理
*
* @return 無し
*/
void TerrainDistantView::Terminate( void )
{
  if( m_pTerrainFactory && m_pModel )
  {
    m_pTerrainFactory->DeleteModel( &m_pModel );
  }

  m_DistantViewNo = 0;
  m_pTerrainFactory = NULL;
}

/**
* @brief 更新処理
*
* @param localAnimationFrame;      Localアニメの現在フレーム
* @param oneDayAnimationFrame;     1日アニメの現在フレーム
*
* @return 無し
*/
void TerrainDistantView::Update( f32 localAnimationFrame, f32 oneDayAnimationFrame, const gfl2::math::Matrix44* pCullingMatrixTable )
{
  // Visible制御
  {
#if PM_DEBUG
    // デバッグスイッチによる非表示
    b32 debugVisible = false;
    if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN ) )
    {
      debugVisible = FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::TERRAIN_DISTANT_VIEW );
    }

    if( debugVisible )
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_DEBUG_DRAW_SWITCH) );
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_DEBUG_DRAW_SWITCH );
    }
#endif

    // 視錐台カリングによる非表示
    b32 cullingEnable = true;
    if( pCullingMatrixTable == NULL || m_pModel == NULL )
    {
      cullingEnable = false;
    }

#if PM_DEBUG
    if( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_TERRAIN_CULLING ) == 0 )
    {
      cullingEnable = false;
    }
#endif

    if( cullingEnable )
    {
      gfl2::math::AABB aabb = m_pModel->GetModelInstanceNode()->GetWorldAABB();

      // ＡＡＢＢをカリング判定する。
      if ( gfl2::math::AABB::Util::IsIntersectFrustum( pCullingMatrixTable, aabb ) )
      {
        // 内側
        m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_CULLING) );
      }
      else
      {
        // 外側
        m_InvisibleBit = ( m_InvisibleBit | INVISIBLE_CULLING );
      }
    }
    else
    {
      m_InvisibleBit = ( m_InvisibleBit & (~INVISIBLE_CULLING) );
    }

    UpdateVisibleImpl( !m_InvisibleBit );
  }

  // 各種更新
  if( m_InvisibleBit == 0 && m_pModel != NULL ) // @fix cov_ctr[10130]NullPtrCheck
  {
    m_pModel->SetAnimationFrame( localAnimationFrame, Block::AnimationSlotNo::LOCAL );
    m_pModel->SetAnimationFrame( oneDayAnimationFrame, Block::AnimationSlotNo::ONE_DAY );
    m_pModel->UpdateAnimation();
  }
}

/**
*  @brief 現在モデルが表示されているか取得する
*/
b32 TerrainDistantView::IsVisible( void ) const
{
  if( m_pModel == NULL )
  {
    return false;
  }

  return m_pModel->IsVisible();
}

/**
*  @brief 現在モデルのDrawTag数を取得
*/
s32 TerrainDistantView::GetDrawTagNum( void ) const
{
  if( m_pModel == NULL )
  {
    return 0;
  }

  return m_pModel->GetModelInstanceNode()->GetDrawTagNum();
}

//
// private
//

/**
* @brief モデルの表示・非表示切り替え
*
* @param visible  true 表示 false 非表示
*
* @return 無し
*/
void TerrainDistantView::UpdateVisibleImpl( b32 visible )
{
  if( m_pModel == NULL )
  {
    return;
  }

  if( m_pModel->IsVisible() == visible )
  {
    return;
  }

  m_pModel->SetVisible( visible );

  // @note コンテナやシーン自体から外す場合はここにコードを追加する
}


GFL_NAMESPACE_END( Terrain );
GFL_NAMESPACE_END( Field );