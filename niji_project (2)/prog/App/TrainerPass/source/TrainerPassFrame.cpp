//=============================================================================
/**
 * @file    TrainerPassFrame.cpp
 * @brief   トレーナーパス画面フレーム処理
 * @author  Hiroyuki Nakamura
 * @date    2015.11.06
 */
//=============================================================================

// module
#include "App/AppEvent/include/TrainerPass/TrainerPassGameData.h"
#include "TrainerPassFrame.h"
#include "TrainerPassDebugWin.h"
// gflib2
#include <Fade/include/gfl2_FadeManager.h>
// niji
#include "GameSys/include/GameData.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/app_util_Heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/TrainerPassGraphic.gaix"
#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TrainerPass)

//! メインシーケンス
enum MainSeq
{
  SEQ_START = 0,      //!< 開始
  SEQ_MAIN,           //!< メイン
  SEQ_PAGE_CHANGE_L,  //!< ページ切り替え(-)
  SEQ_PAGE_CHANGE_R,  //!< ページ切り替え(+)

  SEQ_STAMP_INIT,     //!< スタンプ用初期処理
  SEQ_STAMP_CHANGE_L, //!< スタンプ用ページ切り替え(-)
  SEQ_STAMP_CHANGE_R, //!< スタンプ用ページ切り替え(+)
  SEQ_STAMP_PUT,      //!< スタンプ用押す処理

  SEQ_END             //!< 終了
};


//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
TrainerPassFrame::TrainerPassFrame(void)
  : m_appParam( NULL )
  , m_heap( NULL )
  , m_renderingManager( NULL )
  , m_playData( NULL )
  , m_pLytResBuff( NULL )
  , m_pMsgBuff( NULL )
  , m_pDraw( NULL )
  , m_stampPage( 0 )
  , m_stampWait( 0 )
  , m_mainSeq( 0 )
  , m_subSeq( 0 )
#if PM_DEBUG
  , m_pDebugWin( NULL )
  , m_pDebugWinHeap( NULL )
#endif  // PM_DEBUG
{
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
TrainerPassFrame::~TrainerPassFrame()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   GameData関連取得
 * @date    2015.11.10
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::GetGameData(void)
{
  m_playData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetTrainerPassPlayData();
}

//-----------------------------------------------------------------------------
/**
 * @func    Setup
 * @brief   パラメータセット
 * @date    2015.11.06
 *
 * @param   param       外部設定パラメータ
 * @parma   heap        ヒープ
 * @param   render_man  描画マネージャー
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Setup( APP_PARAM * param, app::util::Heap * heap, app::util::AppRenderingManager * render_man )
{
  m_appParam = param;
  m_heap = heap;
  m_renderingManager = render_man;

  if( m_appParam->call_mode == CALL_MODE_MENU )
  {
    m_stampPage = m_playData->GetStampPage();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   フレーム初期化
 * @date    2015.11.06
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TrainerPassFrame::InitFunc(void)
{
  switch( GetSubSeq() )
  {
  case 0:
#if PM_DEBUG
    CreateDebugWin();
#endif
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
    if( CreateDraw() == false )
    {
      break;
    }
    return applib::frame::RES_FINISH;
  }

  return applib::frame::RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   フレーム終了
 * @date    2015.11.06
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TrainerPassFrame::EndFunc(void)
{
  if( DeleteDraw() == false )
  {
    return applib::frame::RES_CONTINUE;
  }
  UnloadMessage();
  UnloadLayoutResource();
#if PM_DEBUG
  DeleteDebugWin();
#endif
  return applib::frame::RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateFunc
 * @brief   フレーム更新
 * @date    2015.11.06
 *
 * @return  applib::frame::Result
 */
//-----------------------------------------------------------------------------
applib::frame::Result TrainerPassFrame::UpdateFunc(void)
{
  m_pDraw->UpdateTree();

  switch( GetSubSeq() )
  {
  case SEQ_START:
    Update_SeqStart();
    break;

  case SEQ_MAIN:
    break;

  case SEQ_PAGE_CHANGE_L:   // ページ切り替え(-)
    Update_SeqPageChangeL();
    break;

  case SEQ_PAGE_CHANGE_R:   // ページ切り替え(+)
    Update_SeqPageChangeR();
    break;

  case SEQ_STAMP_INIT:      // スタンプ用初期処理
    Update_SeqStampInit();
    break;
  case SEQ_STAMP_CHANGE_L:  // スタンプ用ページ切り替え(-)
    Update_SeqStampChangeL();
    break;
  case SEQ_STAMP_CHANGE_R:  // スタンプ用ページ切り替え(+)
    Update_SeqStampChangeR();
    break;
  case SEQ_STAMP_PUT:       // スタンプ用押す処理
    Update_SeqStampPut();
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
 * @date    2015.11.06
 *
 * @param   no  描画先のディスプレイ番号
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  m_pDraw->DrawTree( no );
}


//-----------------------------------------------------------------------------
/**
 * @func    CreateDraw
 * @brief   描画クラス生成
 * @date    2015.11.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::CreateDraw(void)
{
  switch( m_subSeq )
  {
  case 0:
    if( m_appParam->call_mode == CALL_MODE_MENU )
    {
      m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) TrainerPassDraw( m_heap, m_renderingManager, false );
    }
    else
    {
      m_pDraw = GFL_NEW(m_heap->GetSystemHeap()) TrainerPassDraw( m_heap, m_renderingManager, true );
    }
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pDraw->Init( m_pLytResBuff, m_pMsgBuff ) == false )
    {
      break;
    }
    m_pDraw->SetDrawListener( this );
    m_subSeq = 0;
    return true;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteDraw
 * @brief   描画クラス削除
 * @date    2015.11.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::DeleteDraw(void)
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
 * @date    2015.11.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::LoadLayoutResource(void)
{
  switch( m_subSeq )
  {
  case 0:   // ファイルオープン
    app::util::FileAccessor::FileOpen( ARCID_TRAINER_PASS_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:   // ファイルロード
    if( app::util::FileAccessor::IsFileOpen( ARCID_TRAINER_PASS_GRA ) == false )
    {
      break;
    }
    app::util::FileAccessor::FileLoad(
      ARCID_TRAINER_PASS_GRA,
      GARC_TrainerPassGraphic_TrainerPass_applyt_COMP,
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
    app::util::FileAccessor::FileClose( ARCID_TRAINER_PASS_GRA, m_heap->GetSystemHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 3:   // ファイル操作終了
    if( app::util::FileAccessor::IsFileClose( ARCID_TRAINER_PASS_GRA ) == false )
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
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::UnloadLayoutResource(void)
{
  GflHeapFreeMemory( m_pLytResBuff );
}

//-----------------------------------------------------------------------------
/**
 * @func    LoadMessage
 * @brief   メッセージ読み込み
 * @date    2015.11.06
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::LoadMessage(void)
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
      GARC_message_trainer_pass_DAT,
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
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::UnloadMessage(void)
{
  GflHeapFreeMemory( m_pMsgBuff );
}

#if 0
//-----------------------------------------------------------------------------
/**
 * @func    CreateWordSetLoader
 * @brief   WordSetLoader生成
 * @date    2015.10.29
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::CreateWordSetLoader(void)
{
  switch( m_subSeq )
  {
  case 0:
    m_pWordSetLoader = GFL_NEW(m_heap->GetSystemHeap()) WordSetLoader( m_heap->GetDeviceHeap(), PRELOAD_PLACENAME );
    m_pWordSetLoader->StartLoad( m_heap->GetDeviceHeap() );
    m_subSeq++;
    /* FALL THROUGH */
  case 1:
    if( m_pWordSetLoader->WaitLoad() == false )
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
 * @func    DeleteWordSetLoader
 * @brief   WordSetLoader削除
 * @date    2015.10.29
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::DeleteWordSetLoader(void)
{
  GFL_SAFE_DELETE( m_pWordSetLoader );
}
#endif

//-----------------------------------------------------------------------------
/**
 * @func    InitFade
 * @brief   初期フェード設定
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::InitFade(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeIn
 * @brief   フェードイン開始
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::SetFadeIn(void)
{
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFadeOut
 * @brief   フェードアウト開始
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::SetFadeOut(void)
{
  static const gfl2::math::Vector4 color( 0.0f, 0.0f, 0.0f, 255.0f );
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut( gfl2::Fade::DISP_DOUBLE, &color );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFadeEnd
 * @brief   フェード終了チェック
 * @date    2015.11.06
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::IsFadeEnd(void)
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
 * @date    2015.11.06
 *
 * @param   seq   シーケンス
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::ChangeSequence( u32 seq )
{
  SetSubSeq( seq );
  m_mainSeq = 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEnd
 * @brief   終了設定
 * @date    2015.11.10
 *
 * @param   mode  終了モード
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::SetEnd( EndMode mode )
{
  m_appParam->end_mode = mode;
  m_playData->SetStampPage( m_stampPage );
  ChangeSequence( SEQ_END );
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStart
 * @brief   アップデート：開始処理
 * @date    2015.11.06
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqStart(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->Start( m_stampPage );
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
    if( m_appParam->call_mode == CALL_MODE_MENU )
    {
      m_pDraw->SetInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    else
    {
      ChangeSequence( SEQ_STAMP_INIT );
    }
/*
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
*/
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqPageChangeL
 * @brief   アップデート：ページ切り替え(-)
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqPageChangeL(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartPageChangeAnime( -1 );
    m_pDraw->PutPage( TrainerPassDraw::PUT_COPY, m_stampPage );
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_COPY, m_stampPage );
    m_mainSeq++;
    break;

  case 1:
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_DEFAULT, m_stampPage-1 );
    m_pDraw->PutPage( TrainerPassDraw::PUT_DEFAULT, m_stampPage-1 );
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pDraw->IsEndPageChangeAnime() != false )
    {
      m_stampPage--;
      m_pDraw->SetPage( m_stampPage );
      m_pDraw->SetInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqPageChangeR
 * @brief   アップデート：ページ切り替え(+)
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqPageChangeR(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartPageChangeAnime( 1 );
    m_pDraw->PutPage( TrainerPassDraw::PUT_COPY, m_stampPage+1 );
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_COPY, m_stampPage+1 );
    m_mainSeq++;
    break;

  case 1:
    if( m_pDraw->IsEndPageChangeAnime() != false )
    {
      m_stampPage++;
      m_pDraw->PutPage( TrainerPassDraw::PUT_DEFAULT, m_stampPage );
      m_pDraw->ChangeStamp( TrainerPassDraw::PUT_DEFAULT, m_stampPage );
      m_pDraw->SetPage( m_stampPage );
      m_pDraw->SetInputEnabled( true );
      ChangeSequence( SEQ_MAIN );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStampInit
 * @brief   アップデート：スタンプ用初期処理
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqStampInit(void)
{
  if( m_stampWait >= STAMP_INIT_WAIT )
  {
    if( m_appParam->stamp_no == m_stampPage )
    {
      ChangeSequence( SEQ_STAMP_PUT );
    }
    else if( m_appParam->stamp_no < m_stampPage )
    {
      ChangeSequence( SEQ_STAMP_CHANGE_L );
    }
    else
    {
      ChangeSequence( SEQ_STAMP_CHANGE_R );
    }
    m_stampWait = 0;
  }
  else
  {
    m_stampWait++;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStampChangeL
 * @brief   アップデート：スタンプ用ページ切り替え(-)
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqStampChangeL(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartPageChangeAnime( -1 );
    m_pDraw->PutPage( TrainerPassDraw::PUT_COPY, m_stampPage, m_appParam->stamp_no );
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_COPY, m_stampPage, m_appParam->stamp_no );
    Sound::PlaySE( SEQ_SE_TPASS_PAGETURN );
    m_mainSeq++;
    break;

  case 1:
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_DEFAULT, m_stampPage-1, m_appParam->stamp_no );
    m_pDraw->PutPage( TrainerPassDraw::PUT_DEFAULT, m_stampPage-1, m_appParam->stamp_no );
    m_mainSeq++;
    /* FALL THROUGH */
  case 2:
    if( m_pDraw->IsEndPageChangeAnime() != false )
    {
      m_stampPage--;
      m_pDraw->SetPage( m_stampPage );
      if( m_stampPage == m_appParam->stamp_no )
      {
        ChangeSequence( SEQ_STAMP_PUT );
      }
      else
      {
        m_mainSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStampChangeR
 * @brief   アップデート：スタンプ用ページ切り替え(+)
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqStampChangeR(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartPageChangeAnime( 1 );
    m_pDraw->PutPage( TrainerPassDraw::PUT_COPY, m_stampPage+1, m_appParam->stamp_no );
    m_pDraw->ChangeStamp( TrainerPassDraw::PUT_COPY, m_stampPage+1, m_appParam->stamp_no );
    Sound::PlaySE( SEQ_SE_TPASS_PAGETURN );
    m_mainSeq++;
    break;

  case 1:
    if( m_pDraw->IsEndPageChangeAnime() != false )
    {
      m_stampPage++;
      m_pDraw->PutPage( TrainerPassDraw::PUT_DEFAULT, m_stampPage, m_appParam->stamp_no );
      m_pDraw->ChangeStamp( TrainerPassDraw::PUT_DEFAULT, m_stampPage, m_appParam->stamp_no );
      m_pDraw->SetPage( m_stampPage );
      if( m_stampPage == m_appParam->stamp_no )
      {
        ChangeSequence( SEQ_STAMP_PUT );
      }
      else
      {
        m_mainSeq = 0;
      }
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqStampPut
 * @brief   アップデート：スタンプ用押す処理
 * @date    2015.11.09
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::Update_SeqStampPut(void)
{
  switch( m_mainSeq )
  {
  case 0:
    m_pDraw->StartStampAnime( true );
    m_mainSeq++;
    /* FALL THROUGH */
  case 1:
    m_pDraw->PlaySE_StampPut();   // フレームをみてSEを再生する
    if( m_pDraw->IsEndStampAnime() != false )
    {
      m_pDraw->PutPage( TrainerPassDraw::PUT_DEFAULT, m_stampPage );
      m_pDraw->ChangeStamp( TrainerPassDraw::PUT_DEFAULT, m_stampPage );
      m_stampWait = 0;
      m_mainSeq++;
    }
    break;

  case 2:
    if( m_stampWait != STAMP_PUT_WAIT )
    {
      m_stampWait++;
      break;
    }
    m_pDraw->StartStampAnime( false );
    m_mainSeq++;
    /* FALL THROUGH */
  case 3:
    m_pDraw->PlaySE_StampRelease();   // フレームをみてSEを再生する
    if( m_pDraw->IsEndStampAnime() != false )
    {
      m_stampWait = 0;
      m_mainSeq++;
    }
    break;

  case 4:
    if( m_stampWait != STAMP_END_WAIT )
    {
      m_stampWait++;
      break;
    }
    SetEnd( END_MODE_NORMAL );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Update_SeqEnd
 * @brief   アップデート：終了処理
 * @date    2015.11.06
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool TrainerPassFrame::Update_SeqEnd(void)
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
 * @date    2015.11.06
 *
 * @param   id    動作ID
 *
 * @note  override TrainerPassDraw::DrawListener
 */
//-----------------------------------------------------------------------------
void TrainerPassFrame::OnAction( TrainerPassDraw::DrawListener::ActionID id )
{
  switch( id )
  {
  case TrainerPassDraw::DrawListener::ACTION_ID_B_BUTTON:
    SetEnd( END_MODE_NORMAL );
    break;

  case TrainerPassDraw::DrawListener::ACTION_ID_A_BUTTON:
    SetEnd( END_MODE_PHOTO );
    break;

  case TrainerPassDraw::DrawListener::ACTION_ID_STAMP_L:
    m_pDraw->SetInputEnabled( false );
    ChangeSequence( SEQ_PAGE_CHANGE_L );
    break;

  case TrainerPassDraw::DrawListener::ACTION_ID_STAMP_R:
    m_pDraw->SetInputEnabled( false );
    ChangeSequence( SEQ_PAGE_CHANGE_R );
    break;
  }
}



#if PM_DEBUG
void TrainerPassFrame::CreateDebugWin(void)
{
  static const u32 DEBUG_WIN_HEAP_SIZE = 0x4000;
  m_pDebugWinHeap = GFL_CREATE_LOCAL_HEAP( m_heap->GetDeviceHeap(), DEBUG_WIN_HEAP_SIZE, gfl2::heap::HEAP_TYPE_EXP, false );
  m_pDebugWin = GFL_NEW(m_pDebugWinHeap) TrainerPassDebugWin( m_pDebugWinHeap, NULL );
}
void TrainerPassFrame::DeleteDebugWin(void)
{
  GFL_SAFE_DELETE( m_pDebugWin );
  GFL_DELETE_HEAP( m_pDebugWinHeap );
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(TrainerPass)
GFL_NAMESPACE_END(App)
