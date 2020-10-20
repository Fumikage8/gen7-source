//======================================================================
/**
 * @file WazaOshieUpperDraw.cpp
 * @date 2015/09/04 21:08:55
 * @author fukushima_yuya
 * @brief 上画面の描画
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/WazaOshie/source/WazaOshieUpperDraw.h>
#include <App/WazaOshie/source/WazaOshieDrawerBase.h>

// gflib2
#include <Fade/include/gfl2_FadeManager.h>

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/app/common_status/common_status.h>
#include <niji_conv_header/app/common_status/common_status_pane.h>
#include <niji_conv_header/app/common_status/common_status_anim_list.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( WazaOshie )

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
namespace {
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
WazaOshieUpperDraw::WazaOshieUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRanderingManager )
  : app::ui::UIView( pHeap, pHeap->GetSystemAllocator(), pHeap->GetDeviceAllocator() )
  , m_pHeap( pHeap )
  , m_pAppRenderingManager( pAppRanderingManager )
  , m_pFadeManager( NULL )
  , m_Seq( SEQ_INIT )
  , m_pDrawerBase( NULL )
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   初期化処理
 * @param   pAppParam
 * @param   pAppLytBuff   レイアウトバイナリ
 * @param   fadeMan       フェードマネージャ
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::Fade::FadeManager* fadeMan )
{
  m_pFadeManager = fadeMan;

  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( m_pHeap, false );

  Initialize2D( m_pHeap, pAppLytBuff );

  m_Seq = SEQ_READY;
}

//------------------------------------------------------------------------------
/**
 * @brief   終了処理
 */
//------------------------------------------------------------------------------
bool WazaOshieUpperDraw::End( void )
{
  GFL_SAFE_DELETE( m_pDrawerBase );

  Delete2D();
  DeleteGraphicsSystem();

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::Update( void )
{
  app::ui::UIView::Update();

  switch( m_Seq )
  {
  case SEQ_READY:
    {
      // ポケモンリストを開く
      WazaOshieOpen();
    } break;

  case SEQ_IN:
    {
      if( !IsEndInOutAnim(true) ) break;

      m_Seq = SEQ_MAIN;
    } break;

  case SEQ_OUT:
    {
      if( !IsEndInOutAnim(false) ) break;

      m_Seq = SEQ_END_READY;
    } break;

  case SEQ_END_READY:
    {
      m_Seq = SEQ_END;
    } break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   描画
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::Draw( gfl2::gfx::CtrDisplayNo no )
{
  app::util::G2DUtil* pG2DUtil = m_pDrawerBase->GetG2D();
  pG2DUtil->AddDrawableLytWkForOneFrame( m_pAppRenderingManager, no, 0 );
}

//-----------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 */
//-----------------------------------------------------------------------------
bool WazaOshieUpperDraw::IsExit( void )
{
  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のオープン
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::WazaOshieOpen( void )
{
  m_Seq = SEQ_IN;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンリスト画面のクローズ
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::WazaOshieClose( void )
{
  m_Seq = SEQ_OUT;
}

//------------------------------------------------------------------------------
/**
 * @brief   イン・アウトアニメが終了したかどうか
 * @param   in    インアニメかどうか
 * @return  true = 終了、false = 継続
 */
//------------------------------------------------------------------------------
bool WazaOshieUpperDraw::IsEndInOutAnim( bool in )
{
  return m_pFadeManager->IsEnd( gfl2::Fade::DISP_UPPER );
}

//------------------------------------------------------------------------------
/**
 * @brief   2D関連作成
 * @param   pHeap         ヒープ
 * @param   pAppLytBuff   レイアウトバイナリデータ
 */
//------------------------------------------------------------------------------
void WazaOshieUpperDraw::Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  const app::util::G2DUtil::LytResData resTbl[] = {
    { pAppLytBuff, 1, static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT) }
  };

  const app::util::G2DUtil::LytwkData setupData[] = {
    { // 上
      0,
      LYTRES_COMMON_STATUS_COMMONSTATUS_BGR_UPP_000_BFLYT,
      LA_COMMON_STATUS_COMMONSTATUS_BGR_UPP_000___NUM,
      pAppLytBuff,
      true,
      app::util::G2DUtil::SETUP_UPPER,
      true
    }
  };

  // 2Dの生成
  Create2D(
    pHeap,
    NULL,
    1,
    resTbl,
    GFL_NELEMS(resTbl),
    setupData,
    GFL_NELEMS(setupData),
    this->UNUSE_MESSAGE_DATA );

  //this->SetVisible( false );
  this->SetInputEnabled( false ); //!< 入力を無効に

  gfl2::heap::HeapBase* devHeap = m_pHeap->GetDeviceHeap();
  m_pDrawerBase = GFL_NEW(devHeap) DrawerBase( this, m_pHeap, 0, NULL, NULL );

  // 背景アニメの再生
  m_pDrawerBase->StartAnim( LA_COMMON_STATUS_COMMONSTATUS_BGR_UPP_000__BG_WAVE_GREEN_UPP_00_BG_LOOP_00 );
}

GFL_NAMESPACE_END( WazaOshie )
GFL_NAMESPACE_END( App )
