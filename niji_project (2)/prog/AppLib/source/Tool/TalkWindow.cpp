//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		app_tool_TalkWindow.cpp
 *	@brief  汎用会話用ウィンドウ
 *	@author	Toru=Nagihashi  =>k.ohno
 *	@date		2012.06.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <AppLib/include/Tool/TalkWindow.h>

#include <thread/include/gfl2_ThreadStatic.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/fieldwindow.gaix>

#include "arc_index/message.gaix"
#include <arc_index/script_message.gaix>

#include <Layout/include/gfl2_GraphicsType.h>
#include <Sound/include/Sound.h>

#include "AppLib/include/Tool/app_tool_TimeIcon.h"

//素材
#include "niji_conv_header/app/window/res2d/fieldwindow.h"
#include "niji_conv_header/app/window/res2d/fieldwindow_anim_list.h"
#include "niji_conv_header/app/window/res2d/fieldwindow_pane.h"

// メッセージウィンドウ用ペイン
static const gfl2::lyt::LytPaneIndex messageWindowPane[] =
{
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_00,
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_01,
};

static const gfl2::lyt::LytPaneIndex goldWindowPane[] =
{
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,
  PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,
};

static const gfl2::lyt::LytPaneIndex telopWindowPane[] =
{
  PANENAME_TRP_MIN_UPP_000_PANE_TEXT_00,
  PANENAME_TRP_MIN_UPP_000_PANE_TEXT_01,
};

static const gfl2::lyt::LytPaneIndex telopWhiteWindowPane[] =
{
  PANENAME_TRP_MIN_UPP_001_PANE_TEXT_00,
  PANENAME_TRP_MIN_UPP_001_PANE_TEXT_01,
};

static const gfl2::lyt::LytPaneIndex creditWindowPane[] =
{
  PANENAME_WINDOW_EVENT_UP_000_PANE_TEXTMSG_00,
  PANENAME_WINDOW_EVENT_UP_000_PANE_TEXTMSG_01,
};



static const gfl2::lyt::LytPaneIndex nikkiWindowPane[] =
{
  PANENAME_WINDOW_BOK_UPP_000_PANE_TEXTBOX_00,
  PANENAME_WINDOW_BOK_UPP_000_PANE_TEXTBOX_01,
  PANENAME_WINDOW_BOK_UPP_000_PANE_TEXTBOX_02,
  PANENAME_WINDOW_BOK_UPP_000_PANE_TEXTBOX_03,
  PANENAME_WINDOW_BOK_UPP_000_PANE_TEXTBOX_04,
};

static const gfl2::lyt::LytPaneIndex telopClearWindowPane[] =
{
  PANENAME_TRP_MIN_UPP_002_PANE_TEXT_00,
  PANENAME_TRP_MIN_UPP_002_PANE_TEXT_01,
};




GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Tool)

//-----------------------------------------------------------------------
//  生成・破棄
//-----------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
TalkWindow::TalkWindow(app::util::Heap *pheap, app::util::AppRenderingManager * rendering_manager)
   : app::ui::UIView( pheap, pheap->GetDeviceAllocator(), pheap->GetDeviceAllocator() ),
     m_renderingManager( rendering_manager ),
     m_pAppHeap(pheap),
     m_state( WIN_STATUS_SETUP_WAIT ),
     m_res_id_create(false),
     m_strwin_result(gfl2::str::StrWin::RES_FINISH ),
     m_timer( CLOSE_TIMER_NONE ),
     m_auto_close( false ),
     m_bDraw( false ),
     m_endResult( gfl2::str::StrWin::RES_DONE )
{
  mpScriptWordSet=NULL;
  m_bMsgData = false;
  m_bTrainerMsgData = false;
	m_WindowType=App::Tool::WIN_TYPE_SYS;

}

/**
 * @brief デストラクタ
 */
TalkWindow::~TalkWindow()
{
}

/**
 * @brief 初期化
 *
 * @param heap  ヒープ
 * @param device_allocator  デバイスアロケータ
 * @param graphics_system グラフィックスシステム
 * @param layout_system レイアウトシステム
 * @param device_manager  デバイスマネージャ
 */
//void TalkWindow::Initialize(  const gfl::ui::DeviceManager* device_manager )
//{
//m_device_manager  = device_manager;
//}

//-----------------------------------------------------------------------
//  セットアップ・解放
//-----------------------------------------------------------------------
/**
 * @brief レイアウトワークを渡す
 * @param buflen  メッセージバッファ長（省略可：バッファ長指定の注意点はヘッダ定義を参照のこと）
 */
void TalkWindow::Setup( void* pAppLytBuff, u32 buflen /*=TEMP_STRBUF_LENGTH*/ )
{
  m_pArcReadBuff = pAppLytBuff;

  m_res_id_create = false;

  Initialize2D();
  initMsg( buflen );
  SetInputListener( this );
  _setWindowType();

}


//------------------------------------------------------------------
/**
 * @brief	  ファイル読み込みようテーブル初期化
 */
//------------------------------------------------------------------

void TalkWindow::fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr)
{
  pfrs->openr.arcId       = arcid;

  // ファイル情報のメモリ確保に使う。クローズするまでメモリを取られ続ける。
  // 今回は読み込みが完了したらすぐクローズするので[一時的]であるため後方確保。
  // （最後までクローズしない場合は最後までメモリを使い続けるので前方確保がいいです。）
  pfrs->openr.heapForFile = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  // オープンが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->openr.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();

  pfrs->loadr.arcId             = arcid;
  pfrs->loadr.datId             = dataid;
  pfrs->loadr.ppBuf             = pReadBuffAddr;
  pfrs->loadr.heapForBuf        = pAppHeap->GetDeviceHeap();                    // 読み込んだもののメモリ確保に使う。自分で解放するまでメモリを取られ続ける。
  pfrs->loadr.heapForReq        = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 読み込みが完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.heapForCompressed = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // 解凍が完了するまで[一時的]に使うメモリなので後方確保。
  pfrs->loadr.align             = 128;

  pfrs->closer.arcId       = arcid;
  pfrs->closer.heapForReq  = pAppHeap->GetFileReadHeap()->GetLowerHandle();  // クローズが完了するまで[一時的]に使うメモリなので後方確保。

}

void TalkWindow::fileReadBlock( app::util::FileAccessor::RequestData* pfrs, gfl2::fs::AsyncFileManager* pAsyncFileMan )
{
  pAsyncFileMan->AddArcFileOpenReq(pfrs->openr);
  for ever
  {
    if(  pAsyncFileMan->IsArcFileOpenFinished( pfrs->openr.arcId ))
    {
      pAsyncFileMan->AddArcFileLoadDataReq(pfrs->loadr);
      break;
    }
    gfl2::thread::ThreadStatic::CurrentSleepMicro( 1 );
  }
  for ever
  {
    if(pAsyncFileMan->IsArcFileLoadDataFinished( pfrs->loadr.ppBuf ) )
    {
      pAsyncFileMan->AddArcFileCloseReq(pfrs->closer);
      break;
    }
    gfl2::thread::ThreadStatic::CurrentSleepMicro( 1 );
  }
  for ever
  {
    if(pAsyncFileMan->IsArcFileCloseFinished( pfrs->openr.arcId ) )
    {
      break;
    }
    gfl2::thread::ThreadStatic::CurrentSleepMicro( 1 );
  }
}


/**
 * @brief レイアウトワーク作成 固定読み込みnonwait版
 *
 * @param res_id  リソースID
 * @param buflen  メッセージバッファ長（省略可：バッファ長指定の注意点はヘッダ定義を参照のこと）
 */
void TalkWindow::Setup( gfl2::fs::AsyncFileManager* pAsyncFileMan, u32 buflen /*=TEMP_STRBUF_LENGTH*/ )
{
  app::util::FileAccessor::RequestData frs;
  fileReadStructSetup(&frs, ARCID_FIELDWINDOW, GARC_fieldwindow_fieldwindow_applyt_COMP, m_pAppHeap, &m_pArcReadBuff);
  fileReadBlock(&frs, pAsyncFileMan);

  m_res_id_create = true;

  Initialize2D();
  initMsg( buflen );
  SetInputListener( this );
  _setWindowType();
}


//----------------------------------------------------------------------
/**
 *	@brief  メッセージファイル読み込み
 */
//----------------------------------------------------------------------
void TalkWindow::InitMsgData( u32 arcid, u32 gaixno )
{
  GFL_ASSERT(mpMsgData==NULL);

  mpMsgData = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::str::MsgData( arcid,
                                                                       gaixno,
                                                                       m_pAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );
  m_bMsgData = true;  //内部生成の場合true
}

void TalkWindow::InitMsgData( gfl2::str::MsgData* pMsgData )
{
  GFL_ASSERT(mpMsgData==NULL);

  mpMsgData = pMsgData;
  m_bMsgData = false;
}


void TalkWindow::InitTrainerMsgData( gfl2::str::MsgData* pTrNameMsgData )
{
  GFL_ASSERT(mpTrNameMsgData==NULL);

  mpTrNameMsgData = pTrNameMsgData;
  m_bTrainerMsgData = false;
}

void TalkWindow::InitTrainerMsgData( void )
{
  GFL_ASSERT(mpTrNameMsgData==NULL);

  mpTrNameMsgData = GFL_NEW(m_pAppHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(),
                                                                             GARC_message_namelist_DAT, m_pAppHeap->GetDeviceHeap(),
                                                                             gfl2::str::MsgData::LOAD_FULL );
  m_bTrainerMsgData = true;
}



void TalkWindow::initMsg( u32 buflen )
{
  mpTempStrBuf = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( buflen, m_pAppHeap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( buflen, m_pAppHeap->GetDeviceHeap() );
  mpTrainerStrBuf = GFL_NEW( m_pAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, m_pAppHeap->GetDeviceHeap() );


  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
//  pG2DUtil->CreateMessageWindow(  app::util::G2DUtil::SETUP_UPPER, messageWindowPane,true, m_pAppHeap  );
  if(m_WindowType == App::Tool::WIN_TYPE_DIARY)
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, 5, nikkiWindowPane, m_pAppHeap,false );
    pG2DUtil->GetMsgWin()->ChangeLineBreakSE(SEQ_SE_FLD_BOOKPAGE);//■ページをめくる
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP)
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, telopWindowPane, m_pAppHeap );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP_WHITE)
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, telopWhiteWindowPane, m_pAppHeap );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_CREDIT)
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, creditWindowPane, m_pAppHeap );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_MONEY)
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, goldWindowPane, m_pAppHeap );
  }
  else if (m_WindowType == App::Tool::WIN_TYPE_TELOP_CLEAR)
  {
    pG2DUtil->CreateMessageWindow(m_uiDeviceManager, 0, telopClearWindowPane, m_pAppHeap);
  }
  else
  {
    pG2DUtil->CreateMessageWindow( m_uiDeviceManager, 0, messageWindowPane, m_pAppHeap );
  }
  pG2DUtil->SetMessageCallback(this);
}


void TalkWindow::endMsg(void)
{
  if(m_bTrainerMsgData){
    GFL_SAFE_DELETE( mpTrNameMsgData );
  }
  if(m_bMsgData){
    GFL_SAFE_DELETE( mpMsgData );
  }
  GFL_SAFE_DELETE( mpTrainerStrBuf );
  GFL_SAFE_DELETE( mpTempStrBuf2 );
  GFL_SAFE_DELETE( mpTempStrBuf );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMsgCallback
 * @brief   メッセージ表示コールバック関数
 * @date    2015.12.26
 *
 * @param   arg   コールバックID
 *
 * @retval  true  = 継続
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool TalkWindow::UpdateMsgCallback( u16 arg )
{
  switch( arg )
  {

  case 0:   // SEウェイト
    if( Sound::IsSEFinished( m_msgCallBackWork ) != 0 )
    {
      return false;
    }
    return true;

  case 1:   // MEウェイト
    if( Sound::IsMEFinished( m_msgCallBackWork ) != 0 )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;

  case 3:   // 技マシン忘れ（ポカン）
    setMsgCallBackSE( SEQ_SE_WAZAWASURE );
    return false;

  case 4:   // 技を覚えた
    setMsgCallBackME( STRM_ME_LVUP );
    return false;

  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    setMsgCallBackSE
 * @brief   メッセージ表示コールバック中のSE再生
 * @date    2015.12.26
 *
 * @param   se  SE番号
 */
//-----------------------------------------------------------------------------
void TalkWindow::setMsgCallBackSE( u32 se )
{
  m_msgCallBackWork= se;
  Sound::PlaySE( se );
}

//-----------------------------------------------------------------------------
/**
 * @func    setMsgCallBackME
 * @brief   メッセージ表示コールバック中のME再生
 * @date    2015.12.26
 *
 * @param   me  ME番号
 */
//-----------------------------------------------------------------------------
void TalkWindow::setMsgCallBackME( u32 me )
{
  m_msgCallBackWork = me;
  Sound::StartME( me );
}





/**
 * @brief  メッセージのセット
 * @param  pBuff  メッセージバッファ
 */
void TalkWindow::SetMessage( const gfl2::str::StrBuf* pBuff )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  mpTempStrBuf->SetStr(*pBuff);

  if(m_WindowType != App::Tool::WIN_TYPE_MONEY){
  
    if(mpScriptWordSet){
      mpScriptWordSet->Expand( mpTempStrBuf2 , mpTempStrBuf);
      pG2DUtil->SetMessage( mpTempStrBuf2 );
    }
    else{
      pG2DUtil->SetMessage( mpTempStrBuf );
    }
  }
  else{

    if(mpScriptWordSet){
      mpScriptWordSet->Expand( mpTempStrBuf2 , mpTempStrBuf);
      pG2DUtil->SetTextBoxPaneString(0, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,mpTempStrBuf2);
    }
    else{
      pG2DUtil->SetTextBoxPaneString(0, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,mpTempStrBuf);
    }
  }
  dispFieldName();
}





/**
 * @brief メッセージのセット
 * @param  msgid  メッセージのヘッダーのラベル
 */
void TalkWindow::SetMessage(  u32 msgid, gfl2::str::StrBuf* pName )
{
  GFL_ASSERT(mpMsgData); // 開発中になくせる不具合
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  mpMsgData->GetString(msgid, *mpTempStrBuf);

  if(m_WindowType != App::Tool::WIN_TYPE_MONEY){
    if(mpScriptWordSet){
      mpScriptWordSet->Expand( mpTempStrBuf2 , mpTempStrBuf);
      pG2DUtil->SetMessage( mpTempStrBuf2 );
    }
    else{
      pG2DUtil->SetMessage( mpTempStrBuf );
    }
  }
  else{
    print::MessageWindow* pWin = pG2DUtil->GetMsgWin();

    if(mpScriptWordSet){
      mpScriptWordSet->Expand( mpTempStrBuf2 , mpTempStrBuf);
      pG2DUtil->SetTextBoxPaneString(0, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,mpTempStrBuf2);
    }
    else{
      pG2DUtil->SetTextBoxPaneString(0, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXTMSG_02,mpTempStrBuf);
    }
  }

  if( pName )
  {
    dispFieldName( pName );
  }
  else
  {
    dispFieldName();
  }
}





/**
 * @brief レイアウトワーク破棄
 *
 * @note  LytResIDを内部で読み込んでいた場合それの破棄も行います。（外部の場合は破棄しません）
 */
void TalkWindow::End( void )
{
  //  m_win_gra.DeleteLayoutWork();

  if( m_res_id_create )
  {
    GflHeapFreeMemory( m_pArcReadBuff );
    //    m_layout_system->ReleaseLytRes( m_res_id );
    m_res_id_create = false;
  }

  endMsg();

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->DeleteMessageWindow();

  Delete2D();
  DeleteGraphicsSystem();


}


//-----------------------------------------------------------------------
//  システム
//-----------------------------------------------------------------------
/**
 * @brief 更新
 */
void TalkWindow::Update( void )
{

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( (m_WindowType == App::Tool::WIN_TYPE_DIARY) ||
      (m_WindowType == App::Tool::WIN_TYPE_TELOP) || 
      (m_WindowType == App::Tool::WIN_TYPE_TELOP_WHITE) || 
      (m_WindowType == App::Tool::WIN_TYPE_SYS_AT_ONCE) ||
      (m_WindowType == App::Tool::WIN_TYPE_TELOP_CLEAR)){
    pG2DUtil->GetMsgWin()->SetMessageSpeedTemporary( 0, 100 );

    while(1){
      m_strwin_result = pG2DUtil->PrintMessage();
      if(gfl2::str::StrWin::RES_CONTINUE != m_strwin_result){
        break;
      }
    }

  }
  else{
    m_strwin_result = pG2DUtil->PrintMessage();
  }
  app::ui::UIView::Update();

  this->UpdateState();

  //  return this->GetStatus();
}

/**
 * @brief メッセージウィンドウ呼び出し
 */
void TalkWindow::CallMsgWin( void )
{
  // GFL_ASSERT_STOP( m_msg_win ); //@check

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if(pG2DUtil->GetMsgWin()){
    //    pG2DUtil->GetMsgWin()->Clear();
  }
  m_bDraw = true;
  m_state  = WIN_STATUS_WIN_FRAME_IN;
}
/**
 * @brief メッセージウィンドウを閉じる
 */
void TalkWindow::CloseMsgWin( void )
{

  bool is_frame_out = false;
  //m_win_gra.SetFrameInOutAnm( false );

  if(m_WindowType == App::Tool::WIN_TYPE_DIARY){
    Sound::PlaySE( SEQ_SE_FLD_BOOKCLOSE );
  }
  {
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->GetMsgWin()->ChangeLineBreakSE(0);
  }
  if( is_frame_out == true )
  {
    // アニメーションを行う
    m_state  = WIN_STATUS_WIN_FRAME_OUT_WAIT;
  }
  else
  {
    // すぐに終了する
    this->CloseMsgWinReadily();
  }
}
/**
 * @brief メッセージウィンドウを強制閉じる
 */
void TalkWindow::CloseMsgWinReadily( void )
{
  m_bDraw = false;
}

/**
 * @brief 状態を取得
 *
 * @return 状態
 */
TalkWindow::WIN_STATUS TalkWindow::GetStatus( void ) const
{
  return m_state;
}

/**
 * @brief StrWinの更新状態を取得
 *
 * @return 更新状態
 */
gfl2::str::StrWin::Result TalkWindow::GetStrWinResult( void ) const
{
  return m_strwin_result;
}

/**
 * @brief ポーズ解除して次の行を表示する
 */
void TalkWindow::Restart(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->Restart();
  }
}

/**
 * @brief ウィンドウが自動で閉じるタイマーの設定
 *
 * @param time  フレーム  CLOSE_TIMER_NONEだと無効
 */
void TalkWindow::SetTimer( s32 time )
{
  m_timer = time;
}
/**
 * @brief ウィンドウが自動で閉じる設定になっているか
 *
 * @return  trueなっている  falseなっていない
 */
bool TalkWindow::IsSetTimer(void ) const
{
  return m_timer != CLOSE_TIMER_NONE;
}

/**
 * @brief ウィンドウタイプを設定
 *
 * @param window_type ウィンドウタイプ
 */
void TalkWindow::_setWindowType( void )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil(); 

  int tbl[WIN_TYPE_MAX]={
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_00,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_02,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_00,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_02,
    LA_FIELDWINDOW_WINDOW_BOK_UPP_000__PARTS_00_IN,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_04,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_05,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_06,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_07,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01,
    0,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_PATERN_01,
    0,
  };



  int tbl2[WIN_TYPE_MAX]={
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_DOWN,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_DOWN,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_DOWN,
    0,
    0,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_UP,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_DOWN,
    0,
    0,
    LA_FIELDWINDOW_WINDOW_MSG_UPP_000_POS_DOWN,
    0,
  };


  if(tbl2[m_WindowType]!=0){
    pG2DUtil->StartAnime(0, tbl2[m_WindowType]);
  }

  if(m_WindowType == App::Tool::WIN_TYPE_DIARY){
    pG2DUtil->CreateMsgCursor(m_pAppHeap,  0, PANENAME_WINDOW_BOK_UPP_000_PANE_PARTS_00);
    pG2DUtil->StartAnime(0, LA_FIELDWINDOW_WINDOW_BOK_UPP_000__PARTS_00_KEEP);
    pG2DUtil->SetMsgCursorUserVisible(false, false);
    pG2DUtil->StartAnime(0, tbl[m_WindowType]);
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP){
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP_WHITE){
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_CREDIT){
    pG2DUtil->CreateMsgCursor( m_pAppHeap, 0, PANENAME_WINDOW_EVENT_UP_000_PANE_CUR_00 );
    pG2DUtil->StartAnime( 0, LA_FIELDWINDOW_WINDOW_EVENT_UP_000__CUR_00_KEEP );
    pG2DUtil->SetMsgCursorUserVisible( true, false );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_MONEY){
    pG2DUtil->StartAnime(0, tbl[m_WindowType]);
    pG2DUtil->SetPaneVisible(0,  PANENAME_WINDOW_MSG_UPP_000_PANE_CUR_00,false);
  }
  else if (m_WindowType == App::Tool::WIN_TYPE_TELOP_CLEAR){
  }
  else{
    pG2DUtil->StartAnime(0, tbl[m_WindowType],false);
    pG2DUtil->CreateMsgCursor(m_pAppHeap,  0, PANENAME_WINDOW_MSG_UPP_000_PANE_CUR_00);
    pG2DUtil->StartAnime(0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__CUR_00_KEEP);
    pG2DUtil->SetMsgCursorUserVisible(false, false);
  }


}


void TalkWindow::SetWindowType( TalkWindowType window_type )
{
  m_WindowType = window_type;

}




void TalkWindow::SetMsgCursorUserVisible(bool bUser, bool flg)
{
  GetG2DUtil()->SetMsgCursorUserVisible(bUser, flg);
}




/**
 * @brief ウィンドウカラーを設定
 *
 * @param color ウィンドウカラー
 */
void TalkWindow::SetWindowColor( WIN_COLOR_TYPE color )
{
  //カラー変更仕様、画像が無い
}

/**
 * @brief ウィンドウ位置を設定
 *
 * @param vec 位置
 */
void TalkWindow::SetWindowPos( const gfl2::math::Vector3 *vec )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil(); 

  gfl2::math::VEC3 vec3(vec->x,vec->y,vec->z);

  pG2DUtil->SetPanePos( 0,PANENAME_WINDOW_MSG_UPP_000_PANE_POS, &vec3 );
}

/**
 * @brief ウィンドウサイズを変更する（可変ウィンドウのみに対応）
 *
 * @param size  ウィンドウサイズ
 */
//void TalkWindow::SetWindowSize( const gfl2::math::Vector2 *size )
//{
//  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

//	nw::lyt::Size sizex(size->x,size->y);
 // pG2DUtil->SetSize( 0,PANENAME_WINDOW_MSG_UPP_000_PANE_BG_00, sizex );
//}



/**
 * @brief オート描画フラグをセット
 *
 * @param is_auto true オート描画ON（デフォルト）  false OFF
 */
void TalkWindow::SetAutoDrawFlag( bool is_auto )
{
  //  m_win_gra.SetAutoDrawFlag( is_auto );
}

/**
 * @brief 描画
 *
 * @param graphics  グラフィック
 * @param drawtype  描画先
 * @note SetAutoDrawFlagにfalseを設定した場合呼んでください
 */
void TalkWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_bDraw){
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->AddDrawableLytWkForOneFrame(m_renderingManager,displayNo,0);
  }
}
void TalkWindow::Draw( gfl2::gfx::CtrDisplayNo displayNo, u8 order, u32 layerNo )
{
  if(m_bDraw){
    app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
    pG2DUtil->AddDrawableLytWkForOneFrame(m_renderingManager,displayNo,0,order,layerNo);
  }
}


/**
 * @brief 状態を更新
 */
void TalkWindow::UpdateState( void )
{
  // シーケンス制御
  switch( m_state )
  {
  case WIN_STATUS_WIN_FRAME_IN:
    // through break
  case WIN_STATUS_WIN_FRAME_IN_WAIT:
    m_state  = WIN_STATUS_WIN_UPDATE;
    break;
  case WIN_STATUS_WIN_UPDATE:
  case WIN_STATUS_SUSPEND:           // 待機中
    switch(m_strwin_result){
    case gfl2::str::StrWin::RES_DONE:       ///< 表示処理が最後まで終了した
      //GFL_PRINT("表示処理が最後まで終了した\n");
      if( m_endResult == m_strwin_result )
      {
        m_state = WIN_STATUS_WIN_EXIT;
      }
      break;
    case gfl2::str::StrWin::RES_CONTINUE:   ///< 表示処理中
      m_state = WIN_STATUS_WIN_UPDATE;
     // GFL_PRINT("表示処理中\n");
      break;
    case gfl2::str::StrWin::RES_PAUSE:      ///< ポーズ後、指示待ち
      //GFL_PRINT("ポーズ後、指示待ち\n");
      m_state = WIN_STATUS_SUSPEND;
      break;
    case gfl2::str::StrWin::RES_LINE_FEED:  ///< ポーズ解除の処理中
     // GFL_PRINT("ポーズ解除の処理中\n");
      break;
    case gfl2::str::StrWin::RES_FINISH:     ///< 表示処理の終了後、設定されたフィニッシュ待ちまで終了
      //GFL_PRINT("表示処理の終了後、設定されたフィニッシュ待ちまで終了\n");
      if( m_endResult == m_strwin_result )
      {
        m_state = WIN_STATUS_WIN_EXIT;
      }
      break;
    }
    break;

  case WIN_STATUS_WIN_FRAME_OUT_WAIT:
    break;

  case WIN_STATUS_WIN_EXIT:
    break;

  case WIN_STATUS_SETUP_WAIT:
    break;

  default:
    break;

  }
}
/**
 * @brief リソースの更新
 */
void TalkWindow::UpdateResource( void )
{
  //  m_win_gra.Update();
}

/**
 * @brief レイアウトワークを作成
 * @param framerate フレームレート
 */
void TalkWindow::Initialize2D( void )
{


  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  m_pArcReadBuff, 2,  app::util::G2DUtil::ATTACH_ROOT, },
  };

  if(m_WindowType == App::Tool::WIN_TYPE_DIARY){
    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_FIELDWINDOW_WINDOW_BOK_UPP_000_BFLYT,
        LA_FIELDWINDOW_WINDOW_MSG_UPP_000___NUM,
        m_pArcReadBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
        },
    };

    Create2D( m_pAppHeap, NULL, 1,
              resTbl, GFL_NELEMS(resTbl),
              setupData, GFL_NELEMS(setupData),
              UNUSE_MESSAGE_DATA );

  }
  else if(m_WindowType == App::Tool::WIN_TYPE_CREDIT){
    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_FIELDWINDOW_WINDOW_EVENT_UP_000_BFLYT,
        LA_FIELDWINDOW_WINDOW_EVENT_UP_000___NUM,
        m_pArcReadBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
        },
    };

    Create2D( m_pAppHeap, NULL, 1,
              resTbl, GFL_NELEMS(resTbl),
              setupData, GFL_NELEMS(setupData),
              UNUSE_MESSAGE_DATA );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP){
    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_FIELDWINDOW_TRP_MIN_UPP_000_BFLYT,
        LA_FIELDWINDOW_TRP_MIN_UPP_000___NUM,
        m_pArcReadBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
        },
    };

    Create2D( m_pAppHeap, NULL, 1,
      resTbl, GFL_NELEMS(resTbl),
      setupData, GFL_NELEMS(setupData),
      UNUSE_MESSAGE_DATA );
  }
  else if(m_WindowType == App::Tool::WIN_TYPE_TELOP_WHITE){
    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
          LYTRES_FIELDWINDOW_TRP_MIN_UPP_001_BFLYT,
          LA_FIELDWINDOW_TRP_MIN_UPP_001___NUM,
          m_pArcReadBuff,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
      },
    };

    Create2D( m_pAppHeap, NULL, 1,
              resTbl, GFL_NELEMS(resTbl),
              setupData, GFL_NELEMS(setupData),
              UNUSE_MESSAGE_DATA );
  }
  else if (m_WindowType == App::Tool::WIN_TYPE_TELOP_CLEAR){
    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_FIELDWINDOW_TRP_MIN_UPP_002_BFLYT,
        LA_FIELDWINDOW_TRP_MIN_UPP_002___NUM,
        m_pArcReadBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    Create2D(m_pAppHeap, NULL, 1,
      resTbl, GFL_NELEMS(resTbl),
      setupData, GFL_NELEMS(setupData),
      UNUSE_MESSAGE_DATA);
  }
  else{

    app::util::G2DUtil::LytwkData setupData[] = {
      {//上
        0,
        LYTRES_FIELDWINDOW_WINDOW_MSG_UPP_000_BFLYT,
        LA_FIELDWINDOW_WINDOW_MSG_UPP_000___NUM,
        m_pArcReadBuff,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
        },
    };

    Create2D( m_pAppHeap, NULL, 1,
              resTbl, GFL_NELEMS(resTbl),
              setupData, GFL_NELEMS(setupData),
              UNUSE_MESSAGE_DATA );



  }

  m_bDraw = false;
}


/**
 * @brief   タイマーの更新
 *
 * @return  trueタイマー終了  falseカウント中or無効中
 */
bool TalkWindow::UpdateTimer(void)
{
  // タイマーが設定されているかチェック
  if( IsSetTimer() == false )
  {
    // タイマー無効
    return false;
  }

  // タイマーを進める
  m_timer--;

  // クローズチェック
  if( m_timer <= 0 )
  {
    return true;
  }

  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ウィンドウの左ラインまで移動させるためのベクトルを取得
 *
 * @param center_pos  ウィンドウの中心位置
 * @param head_pos_sc 頭のスクリーン座標
 * @param win_w       ウィンドウ幅
 * @param win_h       ウィンドウ高さ
 *
 * @return  ウィンドウの左ラインと交差する位置に配置するための移動ベクトル
 */
/* -------------------------------------------------------------------------*/
gfl2::math::Vector3 TalkWindow::CreateHeadDirVecL(
  gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h )
{
  f32 x;
  f32 ratio;
  gfl2::math::Vector3 trans_vec;
  gfl2::math::Vector3 ret_vec;

  // Normalize時0ベクトルになるのを回避するため
  if( head_pos_sc.x == center_pos.x )
  {
    head_pos_sc.x += 0.1f;
  }

  // 中心から頭の位置への方向ベクトル
  trans_vec = head_pos_sc - center_pos;
  trans_vec.Normalize();

  // ベクトルの倍率を計算
  x = -(win_w/2);
  ratio = x/trans_vec.x;

  // 移動ベクトルを取得
  ret_vec = (trans_vec * ratio);
  return ret_vec;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ウィンドウの下ラインまで移動させるためのベクトルを取得
 *
 * @param center_pos  ウィンドウの中心位置
 * @param head_pos_sc 頭のスクリーン座標
 * @param win_w       ウィンドウ幅
 * @param win_h       ウィンドウ高さ
 *
 * @return  ウィンドウの下ラインと交差する位置に配置するための移動ベクトル
 */
/* -------------------------------------------------------------------------*/
gfl2::math::Vector3 TalkWindow::CreateHeadDirVecD(
  gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h )
{
  f32 y;
  f32 ratio;
  gfl2::math::Vector3 trans_vec;
  gfl2::math::Vector3 ret_vec;

  // Normalize時0ベクトルになるのを回避するため
  if( head_pos_sc.y == center_pos.y )
  {
    head_pos_sc.y += 0.1f;
  }

  // 中心から頭の位置への方向ベクトル
  trans_vec = head_pos_sc - center_pos;
  trans_vec.Normalize();

  // ベクトルの倍率を計算
  y = -win_h/2;
  ratio = y/trans_vec.y;

  // 移動ベクトルを取得
  ret_vec = (trans_vec * ratio);
  return ret_vec;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ウィンドウの上ラインまで移動させるためのベクトルを取得
 *
 * @param center_pos  ウィンドウの中心位置
 * @param head_pos_sc 頭のスクリーン座標
 * @param win_w       ウィンドウ幅
 * @param win_h       ウィンドウ高さ
 *
 * @return  ウィンドウの上ラインと交差する位置に配置するための移動ベクトル
 */
/* -------------------------------------------------------------------------*/
gfl2::math::Vector3 TalkWindow::CreateHeadDirVecU(
  gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w,f32 win_h )
{
  f32 y;
  f32 ratio;
  gfl2::math::Vector3 trans_vec;
  gfl2::math::Vector3 ret_vec;

  // Normalize時0ベクトルになるのを回避するため
  if( head_pos_sc.y == center_pos.y )
  {
    head_pos_sc.y += 0.1f;
  }

  // 中心から頭の位置への方向ベクトル
  trans_vec = head_pos_sc - center_pos;
  trans_vec.Normalize();

  // ベクトルの倍率を計算
  y = win_h/2;
  ratio = y/trans_vec.y;

  // 移動ベクトルを取得
  ret_vec = (trans_vec * ratio);
  return ret_vec;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief ウィンドウの右ラインまで移動させるためのベクトルを取得
 *
 * @param center_pos  ウィンドウの中心位置
 * @param head_pos_sc 頭のスクリーン座標
 * @param win_w       ウィンドウ幅
 * @param win_h       ウィンドウ高さ
 *
 * @return  ウィンドウの右ラインと交差する位置に配置するための移動ベクトル
 */
/* -------------------------------------------------------------------------*/
gfl2::math::Vector3 TalkWindow::CreateHeadDirVecR(
  gfl2::math::Vector3 center_pos , gfl2::math::Vector3 head_pos_sc,f32 win_w ,f32 win_h )
{
  f32 x;
  f32 ratio;
  gfl2::math::Vector3 trans_vec;
  gfl2::math::Vector3 ret_vec;

  // Normalize時0ベクトルになるのを回避するため
  if( head_pos_sc.x == center_pos.x )
  {
    head_pos_sc.x += 0.1f;
  }

  // 中心から頭の位置への方向ベクトル
  trans_vec = head_pos_sc - center_pos;
  trans_vec.Normalize();

  // ベクトルの倍率を計算
  x = win_w/2;
  ratio = x/trans_vec.x;

  // 移動ベクトルを取得
  ret_vec = (trans_vec * ratio);
  return ret_vec;
}

void TalkWindow::SetSuspendKey( bool flag )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->SetSuspendKey(flag);
  }
}
void TalkWindow::SetEnableCtrl( bool enableKey , bool enableTouch )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->SetEnableCtrl(enableKey, enableTouch);
  }
}
void TalkWindow::SetEnableMessageBoost( bool isEnable )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->SetEnableMessageBoost(isEnable);
  }
}

void TalkWindow::SetMessageSpeed( Savedata::ConfigSave::MSG_SPEED spd )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->GetMsgWin()->SetMessageSpeed(spd);
  }
}


void TalkWindow::SetMessageFinishMode( gfl2::str::StrWin::FinishType type, u8 frame )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->GetMsgWin() )
  {
    pG2DUtil->SetMessageFinishMode( type, frame );
    if( type == gfl2::str::StrWin::FINISH_USER )
    {
      m_endResult = gfl2::str::StrWin::RES_FINISH;
    }
    else
    {
      m_endResult = gfl2::str::StrWin::RES_DONE;
    }
  }
}




app::ui::UIInputListener::ListenerResult TalkWindow::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) )
  {
    //    m_state  = WIN_STATUS_WIN_EXIT;
    return app::ui::UIInputListener::DISABLE_ACCESS;
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    //    m_state  = WIN_STATUS_WIN_EXIT;
    return app::ui::UIInputListener::DISABLE_ACCESS;
  }
  return app::ui::UIInputListener::ENABLE_ACCESS;
}




//----------------------------------------------------------------------------------
/**
 *  文字列中のタブIDから名前欄のトレーナーmsg番号を取り出す
 *  @attention  解析ルーチンは統一化したい
 *
 * @param   buf    文字列
 * @param   args   取り出せたらトレーナーの名前msgのインデックス順番
 * @return  取り出せたらtrue なければfalse
 */
//----------------------------------------------------------------------------------
bool TalkWindow::GetTrainerNo( const gfl2::str::StrBuf* buf, int* args )
{
  const gfl2::str::STRCODE* sp;
  u8  argIdxDec[print::WordSet::DEFAULT_WORD_MAX];
  const gfl2::str::STRCODE eomCode = gfl2::str::EOM_CODE;
  const gfl2::str::STRCODE tagCode = gfl2::str::TAG_START_CODE;

  // タグを走査して、bufIdxから減算すべき値argIdxDecのテーブルを作成。
  // タグが bufIdx の順に出現するとは限らないため（特に、言語によってはトレーナータイプ名がトレーナー名よりも後に来る点が挙げられる）。
  for ( int i = 0; i < GFL_NELEMS(argIdxDec); i ++ )
  {
    argIdxDec[i] = 0;
  }
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsWordSetTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u8  tagGrp = print::GetTagGroup( sp );

        if( tagGrp == print::TAGGROUP_WORD )
        {
          u8 tagIdx = print::GetTagIndex( sp );
          switch( tagIdx ){
          case print::TAGIDX_WORD_TRAINER_TYPE:
          case print::TAGIDX_WORD_TRAINER_TYPE_AND_NAME:
            // [Name:TrainerType ] の bufIdx より大きい bufIdx を持つWORDは、取得するargIdxを1減らす
            for ( int i = bufIdx+1; i < GFL_NELEMS(argIdxDec); i ++ )
            {
              argIdxDec[i] ++;
            }
            break;
          default:
            break;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else{
      ++sp;
    }
  }

  // 文字列中に出現するタグに対応するWORDをWordSetに設定
  sp = buf->GetPtr();
  while( *sp != eomCode )
  {
    if( *sp == tagCode )
    {
      if( print::IsNameAreaTagGroup(sp) )
      {
        u32 bufIdx = print::GetTagParam( sp, 0 );
        u32 argIdx = bufIdx;
        u8  tagGrp = print::GetTagGroup( sp );
        if( argIdx >= argIdxDec[bufIdx] ){
          argIdx -= argIdxDec[bufIdx];
        }
        //   BTL_N_Printf( DBGSTR_PRINT_BufIdx, bufIdx);

        if( tagGrp == print::TAGGROUP_WORD )
        {
          u8 tagIdx = print::GetTagIndex( sp );

          if( tagIdx == print::TAGIDX_WORD_TRAINER_NAME_FIELD)
          {
            u16 bufIdx2 = print::GetTagParam( sp, 0 );
            OHNO_PRINT("ここにきたらOK %d\n",bufIdx2);   //トレーナIDを取得
            args[0] = bufIdx2;
            return true;
          }
        }
      }
      sp = gfl2::str::SkipTagCode( sp );
    }
    else{
      ++sp;
    }
  }
  return false;
}



void TalkWindow::dispFieldName(void )
{
  int trno = 0;

  if((m_WindowType == App::Tool::WIN_TYPE_TALK)||(m_WindowType == App::Tool::WIN_TYPE_GIZA)
     ||(m_WindowType == App::Tool::WIN_TYPE_TALK_DOWN)||(m_WindowType == App::Tool::WIN_TYPE_GIZA_DOWN))
  {
    if(GetTrainerNo(mpTempStrBuf,&trno) && mpTrNameMsgData )
    {  //バッファにトレーナー名表示TAGがある
      OHNO_PRINT("FieldMessageSampleDraw  %d \n",trno);
      app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
      mpTrNameMsgData->GetString( trno, *mpTrainerStrBuf );  //トレーナー名を取得
      pG2DUtil->SetTextBoxPaneString(gfl2::lyt::DISPLAY_UPPER, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXT_NAME, mpTrainerStrBuf);

      pG2DUtil->SetPaneVisible(gfl2::lyt::DISPLAY_UPPER,  PANENAME_WINDOW_MSG_UPP_000_PANE_NULL_NAME,true);

    }
  }
}

void TalkWindow::dispFieldName( gfl2::str::StrBuf* pName )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  pG2DUtil->SetTextBoxPaneString(gfl2::lyt::DISPLAY_UPPER, PANENAME_WINDOW_MSG_UPP_000_PANE_TEXT_NAME, pName);

  pG2DUtil->SetPaneVisible(gfl2::lyt::DISPLAY_UPPER,  PANENAME_WINDOW_MSG_UPP_000_PANE_NULL_NAME,true);
}


//----------------------------------------------------------------------------------
/**
 * @brief タイマーアイコン表示切り替え
 *
 * @param flg   true = 表示, false = 非表示
 */
//----------------------------------------------------------------------------------
void TalkWindow::SetVisibleTimerIcon( bool flg )
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( flg != false )
  {
    pG2DUtil->StopAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_OUT );
    pG2DUtil->StartAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_IN );
    App::Tool::TimeIcon::StartAnime( pG2DUtil->GetLayoutWork(0), LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_KEEP );
  }
  else
  {
    pG2DUtil->StopAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_IN );
    pG2DUtil->StartAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_OUT );
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief タイマーアイコンの非表示アニメが終了したか
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//----------------------------------------------------------------------------------
bool TalkWindow::IsTimerIconOutAnimeEnd(void)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();

  if( pG2DUtil->IsAnimeEnd(0,LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_OUT) != false )
  {
    pG2DUtil->StopAnime( 0, LA_FIELDWINDOW_WINDOW_MSG_UPP_000__TIMERICON_KEEP );
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(Tool)
GFL_NAMESPACE_END(App)
