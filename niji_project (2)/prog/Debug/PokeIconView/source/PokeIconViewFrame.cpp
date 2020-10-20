#if PM_DEBUG
//=============================================================================
/**
 * @file    PokeIconViewFrame.cpp
 * @brief   ポケモンアイコン確認画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2016.04.15
 */
//=============================================================================

// module
#include "PokeIconViewFrame.h"
#include "PokeIconViewDraw.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/DebugPokeIconView.gaix"
#include "arc_index/debug_message.gaix"


GFL_NAMESPACE_BEGIN(Debug)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,      //!< 開始
  SEQ_MAIN,           //!< メイン
  SEQ_END             //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
PokeIconViewFrame::PokeIconViewFrame(void)
  : m_heap( NULL )
  , m_renderingManager( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
PokeIconViewFrame::~PokeIconViewFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2016.04.15
 *
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::Setup( app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2016.04.15
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result PokeIconViewFrame::InitFunc(void)
{
  InitFade();
  LoadLayoutResource();
  LoadMessage();
  CreateDraw();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2016.04.15
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result PokeIconViewFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  UnloadMessage();
  UnloadLayoutResource();
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2016.04.15
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result PokeIconViewFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    break;

  case SEQ_MAIN:
    break;

  case SEQ_END:
    if( Update_SeqEnd() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawFunc
 * @brief   フレーム描画
 * @date    2016.04.15
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) PokeIconViewDraw( m_heap, m_renderingManager );
  m_pDraw->Init( m_pLytResBuff, m_pMsgBuff );
  m_pDraw->SetListener( this );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2016.04.15
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIconViewFrame::DeleteDraw(void)
{
  if( m_pDraw != NULL )
  {
    if( m_pDraw->End() == false )
    {
      return false;
    }
    GFL_SAFE_DELETE( m_pDraw );
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadLayoutResource
 * @brief   レイアウトリソース読み込み
 * @date    2015.10.13
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::LoadLayoutResource(void)
{
  app::util::FileAccessor::FileOpenSync( ARCID_DEBUG_POKEICON_VIEW, m_heap->GetSystemHeap() );

  app::util::FileAccessor::FileLoadSync(
    ARCID_DEBUG_POKEICON_VIEW,
    GARC_DebugPokeIconView_DebugPokeIconView_applyt_COMP,
    &m_pLytResBuff,
    m_heap->GetDeviceHeap(),
    true,
    nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );

  app::util::FileAccessor::FileCloseSync( ARCID_DEBUG_POKEICON_VIEW );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId_Debug();

  app::util::FileAccessor::FileOpenSync( arc_id, m_heap->GetSystemHeap() );

  app::util::FileAccessor::FileLoadSync(
    arc_id,
    GARC_debug_message_d_nakahiro_DAT,
    &m_pMsgBuff,
    m_heap->GetDeviceHeap(),
    false,
    4 );

  app::util::FileAccessor::FileCloseSync( arc_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}


//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2016.04.15
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIconViewFrame::IsFadeEnd(void)
{
  gfl2::Fade::FadeManager * man = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  if( man->IsEnd(gfl2::Fade::DISP_UPPER) == false || man->IsEnd(gfl2::Fade::DISP_LOWER) == false )
  {
    return false;
  }
  return true;
}


//-----------------------------------------------------------------------------
/**
 * @func    ChangeSequence
 * @brief   シーケンス切り替え
 * @date    2016.04.15
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2016.04.15
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::Update_SeqStart(void)
{
  switch( m_mainSeq )
  {
  case 0:
    SetFadeIn();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsFadeEnd() == false )
    {
      break;
    }
    m_pDraw->Start();
    ChangeSequence( SEQ_MAIN );
    {
      GFL_PRINT(
        "■Sys: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetSystemHeap()->GetTotalFreeSize(),
        m_heap->GetSystemHeap()->GetTotalAllocatableSize() );
      GFL_PRINT(
        "■Dev: Free = 0x%08x, Alloc = 0x%08x\n",
        m_heap->GetDeviceHeap()->GetTotalFreeSize(),
        m_heap->GetDeviceHeap()->GetTotalAllocatableSize() );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2016.04.15
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIconViewFrame::Update_SeqEnd(void)
{
  switch( m_mainSeq )
  {
  case 0:
    SetFadeOut();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( IsFadeEnd() == false )
    {
      break;
    }
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
/**
 * @func    OnAction
 * @brief   入力時のリスナー
 * @date    2016.04.15
 *
 * @param   id    動作ID
 *
 * @note  override PokeIconViewDraw::Listener
 */
//-----------------------------------------------------------------------------
void PokeIconViewFrame::OnAction( PokeIconViewDraw::Listener::ActionID id )
{
  switch( id )
  {
  case PokeIconViewDraw::Listener::ACTION_EXIT:               // 終了
    m_pDraw->SetInputEnabled( false );
    ChangeSequence( SEQ_END );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_POKE_PLUS:   // ポケモン切り替え+
    m_pDraw->ChangePage( 1 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_POKE_MINUS:  // ポケモン切り替え-
    m_pDraw->ChangePage( -1 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_PAGE_PLUS:   // ページ切り替え+
    m_pDraw->ChangePage( 10 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_PAGE_MINUS:  // ページ切り替え-
    m_pDraw->ChangePage( -10 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_SEX:         // 性別切り替え
    m_pDraw->ChangeSex();
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_FORM_PLUS:   // フォルム切り替え+
    m_pDraw->ChangeForm( 1 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_FORM_MINUS:  // フォルム切り替え-
    m_pDraw->ChangeForm( -1 );
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_FLIP:        // 反転切り替え
    m_pDraw->ChangeFlip();
    break;
  case PokeIconViewDraw::Listener::ACTION_CHANGE_BG_COLOR:    // 背景色切り替え
    m_pDraw->ChangeBgColor();
    break;
  }
}


GFL_NAMESPACE_END(Debug)

#endif  // PM_DEBUG
