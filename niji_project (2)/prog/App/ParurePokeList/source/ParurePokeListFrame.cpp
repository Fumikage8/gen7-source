//=============================================================================
/**
 * @file    ParurePokeListFrame.cpp
 * @brief   ポケパルレポケモン選択画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.12
 */
//=============================================================================

// module
#include "App/ParurePokeList/include/ParurePokeListAppParam.h"
#include "ParurePokeListFrame.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/KawaigariMenuGraphic.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(PokeParure)

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
ParurePokeListFrame::ParurePokeListFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pParticleResBuff( NULL )
  , m_pDraw( NULL )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
{
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
ParurePokeListFrame::~ParurePokeListFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
/*
void ParurePokeListFrame::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
  m_saveMisc = gd->GetMisc();
  m_myItem   = gd->GetMyItem();
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.11.12
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.11.12
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ParurePokeListFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
    InitFade();
    AddSubSeq();
    /* FALL THROUGH */
  case 1:
    if( LoadLayoutResource() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 2:
    if( LoadMessage() == false )
    {
      break;
    }
    AddSubSeq();
    /* FALL THROUGH */
  case 3:
    CreateDraw();
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.11.12
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ParurePokeListFrame::EndFunc(void)
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
 * @date    2015.11.12
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result ParurePokeListFrame::UpdateFunc(void)
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
 * @date    2015.11.12
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}



//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::CreateDraw(void)
{
  m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) ParurePokeListDraw( m_appParam, m_heap, m_renderingManager );
  m_pDraw->Init( m_pLytResBuff, m_pMsgBuff, m_pParticleResBuff );
  m_pDraw->SetListener( this );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::DeleteDraw(void)
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
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_KAWAIGARI_MENU_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_KAWAIGARI_MENU_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_KAWAIGARI_MENU_GRA,
      GARC_KawaigariMenuGraphic_KawaigariMenu_applyt_COMP,
      &m_pLytResBuff,
      m_heap->GetDeviceHeap(),
      true,
      nw::lyt::ARCHIVE_RESOURCE_ALIGNMENT );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pLytResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_KAWAIGARI_MENU_GRA,
      GARC_KawaigariMenuGraphic_Kawaigari_Particle_Bubble_bptcl_COMP,
      &m_pParticleResBuff,
      m_heap->GetDeviceHeap(),
      true,
      128 );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイルクローズ
    if( app::util::FileAccessor::IsFileLoad( &m_pParticleResBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( ARCID_KAWAIGARI_MENU_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 4:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_KAWAIGARI_MENU_GRA ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;

  default:
    GFL_ASSERT( 0 );
    break;
  };

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadLayourResource
 * @brief   レイアウトリソース削除
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pParticleResBuff );
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.11.12
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::LoadMessage(void)
{
  gfl2::fs::ArcFile::ARCID arc_id = print::GetMessageArcId();

  switch( m_subSeq )
  {
  case 0:
    app::util::FileAccessor::FileOpen( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( app::util::FileAccessor::IsFileOpen( arc_id ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      arc_id,
      GARC_message_pokeparure_DAT,
      &m_pMsgBuff,
      m_heap->GetDeviceHeap(),
      false,
      4 );
    m_subSeq++;
    /* FALL THROUGH */
  case 2:
    if( app::util::FileAccessor::IsFileLoad( &m_pMsgBuff ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileClose( arc_id, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:
    if( app::util::FileAccessor::IsFileClose( arc_id ) == false )
    {
      break;
    }
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    UnloadMessage
 * @brief   メッセージ削除
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.11.12
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::IsFadeEnd(void)
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
 * @date    2015.11.12
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.11.12
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::Update_SeqStart(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->Start();
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->IsStart() == false )
    {
      break;
    }
    SetFadeIn();
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( IsFadeEnd() == false )
    {
      break;
    }
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
 * @date    2015.11.12
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool ParurePokeListFrame::Update_SeqEnd(void)
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
 * @date    2015.11.12
 *
 * @param   pos   選択位置
 *
 * @note  override ParurePokeListDraw::Listener
 */
//-----------------------------------------------------------------------------
void ParurePokeListFrame::OnAction( u32 pos )
{
  m_appParam->select_pos = pos;
  ChangeSequence( SEQ_END );
}


GFL_NAMESPACE_END(PokeParure)
GFL_NAMESPACE_END(App)
