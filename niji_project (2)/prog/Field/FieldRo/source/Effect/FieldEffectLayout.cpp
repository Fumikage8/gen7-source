//======================================================================
/**
 * @file FieldEffectLayout.cpp
 * @date 2015/07/14 16:41:32
 * @author miyachi_soichi
 * @brief フィールド用汎用UIエフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// ui
#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"

// Effect
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @brief  コンストラクタ
 *  @param  pHeap           ヒープ
 *  @param  pRenderManager  UI描画管理
 */
EffectLayout::EffectLayout( app::util::Heap *pHeap, app::util::AppRenderingManager *pRenderManager )
: app::ui::UIView( pHeap, pHeap->GetDeviceAllocator(), pHeap->GetDeviceAllocator() )
, m_pAppRenderingManager( pRenderManager )
, m_pAppHeap( pHeap )
, m_nPriority( 128 )
, m_bVisible( false )
{
}

 /**
  * @brief  デストラクタ
  */
EffectLayout::~EffectLayout( void )
{
  Finalize();
}

/**
 *  @brief  レイアウト作成
 *  @param  param   初期化用データ
 */
void EffectLayout::Initialize( const SetupParam &param, gfl2::str::MsgData *pMsgData, print::WordSet *pWordSet )
{
  // レイアウトの作成
  if( pMsgData && pWordSet )
  {
    Create2D(
      m_pAppHeap, NULL, param.layoutNum,
      param.lytResTbl, param.resTblNum,
      param.lytDataTbl, param.dataTblNum,
      pMsgData, pWordSet
      );
  }
  else
  {
    Create2D(
      m_pAppHeap, NULL, param.layoutNum,
      param.lytResTbl, param.resTblNum,
      param.lytDataTbl, param.dataTblNum,
      UNUSE_MESSAGE_DATA
      );
  }
}

/**
 *  @brief  レイアウト破棄
 */
void EffectLayout::Finalize( void )
{
  // レイアウトを破棄
  Delete2D();
  DeleteGraphicsSystem();
}

/**
 *  @brief  更新
 *  UiView側の更新処理：呼び出し元UiView::UpdateTree()
 */
void EffectLayout::Update( void )
{
  // 基底の更新
  UIView::Update();
}

/**
 *  @brief  描画
 *  UiView側の描画処理：呼び出し元UiView::DrawTree()
 */
void EffectLayout::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  // 描画対象ではない
  if( !m_bVisible ){ return; }

  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    p2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, displayNo, 0, m_nPriority );
  }
}

/**
 *  @brief  アニメーション再生
 *  @param  anm           アニメ番号
 *  @param  auto_anm      オートアニメーションするかどうか
 *  @param  auto_unbind   終了時に自動でアンバインドするか
 */
void EffectLayout::StartAnimation( u32 anm, bool auto_anm, bool auto_unbind )
{
  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    p2DUtil->StartAnime( 0, anm, auto_anm, auto_unbind );
  }
}

/**
 *  @brief  アニメーション設定
 *  @param  anm   アニメ番号
 *  @param  frame フレーム番号
 */
void EffectLayout::SetAnimationFrame( u32 anm, f32 frame )
{
  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    p2DUtil->SetAnimeFrame( 0, anm, frame );
  }
}


/**
 *  @brief  アニメーションをとめる。
 *  @param  anm           アニメ番号
 */
void EffectLayout::StopAnimation( u32 anm )
{
  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    p2DUtil->StopAnime( 0, anm );
  }
}

/**
 *  @brief  座標設定
 *  @param  idx   PaneIndex
 *  @param  pos   座標
 */
void EffectLayout::SetPosition( u32 idx, const gfl2::math::Vector3 *pos )
{
  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    gfl2::math::VEC3 nw_pos( pos->x, pos->y, pos->z );
    gfl2::lyt::LytPaneIndex pane_idx = static_cast<gfl2::lyt::LytPaneIndex>( idx );
    p2DUtil->SetPanePos( 0, pane_idx, &nw_pos );
  }
}

/**
 *  @brief  描画切り替え
 *  @param  idx   PaneIndex
 *  @param  clr   色
 */
void EffectLayout::SetColor( u32 idx, const gfl2::math::Vector4 &clr )
{
  app::util::G2DUtil *p2DUtil = UIView::GetG2DUtil();
  if( p2DUtil )
  {
    nw::ut::Color8 stn( static_cast<u8>(clr.GetX()), static_cast<u8>(clr.GetY()), static_cast<u8>(clr.GetZ()), static_cast<u8>(clr.GetW()) );
    p2DUtil->SetVtxColorElementOfPicturePane( 0, idx, stn );
    p2DUtil->SetPaneVisible( 0, idx, true );
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
