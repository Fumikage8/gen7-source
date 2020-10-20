// ============================================================================
/*
 * @file NBRLowerViewMessageMenu.cpp
 * @brief 通信対戦受付アプリ　下画面ビューメッセージメニュ
 * @date 2015.07.28
 * @author ichiraku_katsuhiko
 */
// ============================================================================
// mine
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMessageMenu.h"
#include "NetApp/NBR/source/NBRResourceID.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(View)

// コンストラクタ
NBRLowerViewMessageMenu::NBRLowerViewMessageMenu( NetAppLib::System::ApplicationWorkBase* pWorkBase,
                       app::util::G2DUtil* pG2dUtil,
                       app::util::G2DUtil::LAYOUT_WORK* pLayoutWork ) :
  NBRLowerViewBase( pWorkBase )
  ,m_NetAppMessageMenu( pWorkBase, this )
  ,m_bEndMsgStream(false)
  ,m_pListener(NULL)
  ,m_bShowList(false)
{
  m_strbuf = GFL_NEW( pWorkBase->GetAppHeap()->GetDeviceHeap() ) gfl2::str::StrBuf( 512, pWorkBase->GetAppHeap()->GetDeviceHeap() ); 

  m_NetAppMessageMenu.Setup( NBR_RESOURCE_ID_COMMON_WINDOW );
  AddSubView( &m_NetAppMessageMenu );
}

// デストラクタ
NBRLowerViewMessageMenu::~NBRLowerViewMessageMenu()
{
  GFL_DELETE m_strbuf;
  m_NetAppMessageMenu.RemoveFromSuperView();
}

//--------------------------------------------------------------------------
/**
 * @breif  NetAppMessageMenuのイベントリスナー
 * @param  eEventCode   発生したイベントの定義
 *
 *        EVENT_OnEOM          メッセージ表示が終端に到達
 *        EVENT_OnFinish       EOM後、ユーザの入力まちなどまでが完了した                  
 *        EVENT_OnSelectedItem リスト選択でItemが選択された: GetSelectedItemIndex()で取得  
 *
 *        EVENT_OnSelectedYes  Yes/NoモードでYesが選択された
 *        EVENT_OnSelectedNo   Yes/NoモードでNoが選択された
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::MessageMenuView_OnEvent( const NetAppLib::UI::NetAppMessageMenu::IEventHandler::EventCode eEventCode )
{
  switch( eEventCode ){
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnEOM:            /**<  メッセージ表示が終端に到達                                  */
  //case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnFinish:         /**<  EOM後、ユーザの入力まちなどまでが完了した                   */
    m_bEndMsgStream = true;
    break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedItem:   /**<  リスト選択でItemが選択された: GetSelectedItemIndex()で取得  */
    if( m_pListener != NULL )
    {
      m_pListener->OnMutliButtonAction( m_NetAppMessageMenu.GetSelectedItemIndex() );
      m_bShowList = false;
      // Modal解除
      SetModal( false );
    }
    break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedYes:    /**<  Yes/NoモードでYesが選択された                               */
    if( m_pListener != NULL )
    {
      m_pListener->OnYesButtonAction();
      m_bShowList = false;
      // Modal解除
      SetModal( false );
    }
    break;
  case NetAppLib::UI::NetAppMessageMenu::IEventHandler::EVENT_OnSelectedNo:     /**<  Yes/NoモードでNoが選択された                                */
    if( m_pListener != NULL )
    {
      m_pListener->OnNoButtonAction();
      m_bShowList = false;
      // Modal解除
      SetModal( false );
    }
    break;
  }
}


//--------------------------------------------------------------------------
/**
 * @brief 準備完了しているか
 * @return trueで完了
 */
//--------------------------------------------------------------------------
bool NBRLowerViewMessageMenu::IsInitialized()
{
  return m_NetAppMessageMenu.App::Tool::MessageMenuView::IsSetup();
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージ表示（一括）
 * @param[in]  msgId            メッセージID
 * @param[in]  isShowTimeIcon   タイムアイコンを表示するか
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowMessage( const u32 msgId, const bool isShowTimeIcon )
{
  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();

  gfl2::str::StrBuf* buf = g2dUtil->GetTempStrBuf(1);
  g2dUtil->GetString( buf, msgId );

  ShowMessageStrBuf( buf, isShowTimeIcon );
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージ表示（一括）
 * @param[in]  buf              文字バッファ
 * @param[in]  isShowTimeIcon   タイムアイコンを表示するか
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowMessageStrBuf( gfl2::str::StrBuf* buf, const bool isShowTimeIcon )
{
  // 選択肢コマンドが出ていたら消す
  if( IsShowList() )
  {
    HideMessage();
  }

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( buf, true, isShowTimeIcon, false );
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージ開始（ストリーム）
 * @param[in] msgId      メッセージID
 * @param[in] finishType メッセージ終了タイプ
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::StartMessage( const u32 msgId, const MessageFinishType finishType )
{
  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();

  gfl2::str::StrBuf* buf = g2dUtil->GetTempStrBuf(1);
  g2dUtil->GetString( buf, msgId );

  // ストリームメッセージ
  StartMessageStrBuf( buf, finishType );
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージ開始（ストリーム）
 * @param[in] buf      文字バッファ
 * @param[in] finishType メッセージ終了タイプ
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::StartMessageStrBuf( gfl2::str::StrBuf* buf, const MessageFinishType finishType )
{
  // 選択肢コマンドが出ていたら消す
  if( IsShowList() )
  {
    HideMessage();
  }

  // 終了動作
  if( finishType == MESSAGE_FINISH_INPUT )
  {
    // 将来、▼を出さないで入力まちしたいパターン用に残しておく
    m_NetAppMessageMenu.SetFinishMode_None();
    //m_NetAppMessageMenu.SetFinishMode_Input();
  }
  else
  {
    m_NetAppMessageMenu.SetFinishMode_None();
    //m_NetAppMessageMenu.SetFinishMode_Timer(0);
  }

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( buf, false, false, false );
  m_bEndMsgStream = false;
}

//--------------------------------------------------------------------------
/**
 * @brief メッセージ終了判定
 * @param trueで終了
 */
//--------------------------------------------------------------------------
bool NBRLowerViewMessageMenu::IsEndMessage( void ) const
{
  return m_bEndMsgStream;
}

//--------------------------------------------------------------------------
/**
 * @brief アプリ終了時の待機メッセージ
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowAppEndMessage()
{
  // つうしんを　しゅうりょうしています
  this->ShowMessage( msg_nbr_win_31, true );
}

//--------------------------------------------------------------------------
/**
* @breif  ルール選択者決定でのチュートリアルメッセージ
* @param  connectNum  接続人数
* @param  isMutli     マルチ
*/
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::StartMessageRuleSelectorTutorial( int connectNum, bool isMulti )
{
  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();
  // メッセージID
  u32 msgId = (isMulti==true) ? msg_nbr_win_04 : msg_nbr_win_03;

  // 数値を埋め込む
  gfl2::str::StrBuf* srcBuf = g2dUtil->GetTempStrBuf(0);
  gfl2::str::StrBuf* dstBuf = g2dUtil->GetTempStrBuf(1);
  print::WordSet*    wordset= g2dUtil->GetWordSet();
  g2dUtil->GetString( srcBuf, msgId );
  wordset->RegisterNumber( 0, connectNum, 1, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
  wordset->Expand( dstBuf, srcBuf );

  // ストリームメッセージ
  StartMessageStrBuf( dstBuf, MESSAGE_FINISH_INPUT );
}

//--------------------------------------------------------------------------
/**
 * @brief 名前埋め込みでメッセージウィンドウ（一括）
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowMessageMyStatus( const u32 msgId, Savedata::MyStatus* pMyStatus, bool isShowTimeIcon )
{
  this->ShowMessageMyStatusCore( msgId, MESSAGE_FINISH_NONE, pMyStatus, true, isShowTimeIcon );
}

//--------------------------------------------------------------------------
/**
 * @brief 名前埋め込みでメッセージウィンドウ（ストリーム）
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::StartMessageMyStatus( const u32 msgId, const MessageFinishType finishType, Savedata::MyStatus* pMyStatus )
{
  this->ShowMessageMyStatusCore( msgId, finishType, pMyStatus, false, false );
}

//--------------------------------------------------------------------------
/**
 * @brief 対戦をやめますか選択肢
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowAppEndList()
{
  // たいせんを　やめますか？
  ShowList( msg_nbr_win_30 );
}

//--------------------------------------------------------------------------
/**
 * @brief 選択肢が表示されているか
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowRuleOtherList()
{
  // ルールをきめるのを　ほかのひとに　おまかせしますか
  ShowList( msg_nbr_win_35 );
}

//--------------------------------------------------------------------------
/**
 * @brief 別の対戦ルールにするかの選択肢
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowCannotJoinList()
{
  // えらぶのをやめて　べつのたいせんルールに　しますか？
  ShowList( msg_nbr_win_27 );
}

//--------------------------------------------------------------------------
/**
 * @brief マルチで立ち位置にプレイヤーを選ぶ選択肢
 * @param[in] pos         何番目か(0～3)
 * @param[in] pMyStatus   ４人全員のマイステータス
 * @param[in] isRandomEnable  おまかせが有りかどうか
 * @param[in] disableNo   非表示にする番号がある場合その番号
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowMutliPlayerList( const u8 pos, Savedata::MyStatus* pMyStatus[4], bool isRandomEnable, int disableNo )
{
  gfl2::heap::HeapBase* pHeap = m_pWork->GetDeviceHeap(); 

  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();

  // 選択肢用文字バッファ
  gfl2::str::StrBuf player1Buf( ::System::STRLEN_PLAYER_NAME+::System::EOM_LEN, pHeap ); 
  gfl2::str::StrBuf player2Buf( ::System::STRLEN_PLAYER_NAME+::System::EOM_LEN, pHeap ); 
  gfl2::str::StrBuf player3Buf( ::System::STRLEN_PLAYER_NAME+::System::EOM_LEN, pHeap ); 
  gfl2::str::StrBuf player4Buf( ::System::STRLEN_PLAYER_NAME+::System::EOM_LEN, pHeap ); 
  gfl2::str::StrBuf randBuf( 60, pHeap ); 
  gfl2::str::StrBuf returnBuf( 60, pHeap ); 
  gfl2::str::StrBuf* messageBuf = m_strbuf;

  {
    // 名前を埋め込む
    gfl2::str::StrBuf* srcBuf = g2dUtil->GetTempStrBuf(0);
    print::WordSet*    wordset= g2dUtil->GetWordSet();
    g2dUtil->GetString( srcBuf, msg_nbr_sel_22 );

    // プレイヤー1
    wordset->RegisterPlayerName( 0, pMyStatus[0] );
    wordset->Expand( &player1Buf, srcBuf );
    // プレイヤー2
    wordset->RegisterPlayerName( 0, pMyStatus[1] );
    wordset->Expand( &player2Buf, srcBuf );
    // プレイヤー3
    wordset->RegisterPlayerName( 0, pMyStatus[2] );
    wordset->Expand( &player3Buf, srcBuf );
    // プレイヤー4
    wordset->RegisterPlayerName( 0, pMyStatus[3] );
    wordset->Expand( &player4Buf, srcBuf );
  }


  g2dUtil->GetString( &randBuf, msg_nbr_sel_23 ); // おまかせにする
  g2dUtil->GetString( &returnBuf, msg_nbr_sel_24 ); // もどる
  g2dUtil->GetString( messageBuf, msg_nbr_win_18 ); // N番目は誰にしますか？

  // N番目はだれにしますか？
  {
    gfl2::str::StrBuf* srcBuf = g2dUtil->GetTempStrBuf(0);
    print::WordSet*    wordset= g2dUtil->GetWordSet();
    g2dUtil->GetString( srcBuf, msg_nbr_win_18 );
    wordset->RegisterNumber( 0, pos+1, 1, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU );
    wordset->Expand( messageBuf, srcBuf );
  }


  // 選択肢作成
  m_NetAppMessageMenu.App::Tool::MessageMenuView::SetYNListMode( false );

  if( isRandomEnable == true )
  {
    m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( randBuf ); 
  }
  if( disableNo != 0 ){
    m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( player1Buf ); 
  }
  if( disableNo != 1 ){
    m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( player2Buf ); 
  }
  if( disableNo != 2 ){
    m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( player3Buf ); 
  }
  if( disableNo != 3 ){
    m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( player4Buf ); 
  }
  m_NetAppMessageMenu.App::Tool::MessageMenuView::AddItem( returnBuf, true ); 
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessageWithListMenu( messageBuf, true, NULL, NULL );

  m_bShowList = true;

  // Modalにする
  SetModal( true );

}

//--------------------------------------------------------------------------
/**
 * @brief 共通：非表示にする
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::HideMessage()
{
  m_NetAppMessageMenu.App::Tool::MessageMenuView::HideMessage();
  m_bShowList = false;
  // Modal解除
  SetModal( false );
}

//--------------------------------------------------------------------------
/**
 * @brief 名前埋め込みでメッセージ
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowMessageMyStatusCore( const u32 msgId, const MessageFinishType finishType, Savedata::MyStatus* pMyStatus, const bool isImmediateMessage, const bool isShowTimerIcon )
{
  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();

  // 自分の名前を埋め込む
  gfl2::str::StrBuf* srcBuf = g2dUtil->GetTempStrBuf(0);
  gfl2::str::StrBuf* dstBuf = g2dUtil->GetTempStrBuf(1);
  print::WordSet*    wordset= g2dUtil->GetWordSet();
  g2dUtil->GetString( srcBuf, msgId );
  wordset->RegisterPlayerName( 0, pMyStatus );
  wordset->Expand( dstBuf, srcBuf );

  // ストリームの時は挙動設定する
  if( isImmediateMessage == false )
  {
    // 終了動作
    if( finishType == MESSAGE_FINISH_INPUT )
    {
      // 将来、▼を出さないで入力まちしたいパターン用に残しておく
      m_NetAppMessageMenu.SetFinishMode_None();
      //m_NetAppMessageMenu.SetFinishMode_Input();
    }
    else
    {
      m_NetAppMessageMenu.SetFinishMode_None();
      //m_NetAppMessageMenu.SetFinishMode_Timer(0);
    }
    m_bEndMsgStream = false;
  }

  // 選択肢コマンドが出ていたら消す
  if( IsShowList() )
  {
    HideMessage();
  }

  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessage( dstBuf, isImmediateMessage, isShowTimerIcon, false );
}

//--------------------------------------------------------------------------
/**
 * @brief 選択肢表示
 */
//--------------------------------------------------------------------------
void NBRLowerViewMessageMenu::ShowList( u32 msgId )
{
  //GFL_ASSERT( m_pListener );

  // 親のもの
  app::util::G2DUtil* g2dUtil = GetSuperView()->GetG2DUtil();

  gfl2::str::StrBuf* yesBuf = g2dUtil->GetTempStrBuf(0);
  gfl2::str::StrBuf* noBuf  = g2dUtil->GetTempStrBuf(1);
  gfl2::str::StrBuf* messageBuf = m_strbuf;

  g2dUtil->GetString( yesBuf, msg_nbr_sel_20 ); // はい
  g2dUtil->GetString( noBuf, msg_nbr_sel_21 ); // いいえ
  g2dUtil->GetString( messageBuf, msgId );

  // 選択肢作成
  m_NetAppMessageMenu.App::Tool::MessageMenuView::SetYNListMode( true );
  m_NetAppMessageMenu.App::Tool::MessageMenuView::ShowMessageWithListMenu( messageBuf, true, yesBuf, noBuf );

  m_bShowList = true;

  // Modalにする
  SetModal( true );
}

GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
