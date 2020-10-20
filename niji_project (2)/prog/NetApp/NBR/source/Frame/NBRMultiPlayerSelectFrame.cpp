// ============================================================================
/*
 * @file NBRMultiPlayerSelectFrame.cpp
 * @brief 通信対戦受付アプリのマルチチーム振り分けプロセスです。
 *        フレームID:NBR_FRAME_ID_MULTI_PLAYER_SELECT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/source/Frame/NBRMultiPlayerSelectFrame.h"
#include "NetApp/NBR/source/NBRFrameResult.h"

#include "System/include/GflUse.h"

#include "NetApp/NBR/source/View/NBRParentView.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

// コンストラクタ
NBRMultiPlayerSelectFrame::NBRMultiPlayerSelectFrame(
  NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
  View::NBRParentView* pParentView,
  NBRNet* pNet ) :
  NBRFrameBase( pNBRWork, pParentView, pNet )
  ,m_pLowerView(NULL)
  ,m_seq(SEQ_GUIDE_MESSAGE_WAIT)
  ,m_targetPlace(0)
  ,m_targetNetId(0)
  ,m_randomSelectEnable(false)
{
  for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
  {
    m_decideNetId[i] = -1;
  }

  gfl2::std::MemClear( m_buttonIndex2NetIdTbl, sizeof(m_buttonIndex2NetIdTbl) );
}

// コンストラクタ
NBRMultiPlayerSelectFrame::~NBRMultiPlayerSelectFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
//--------------------------------------------------------------------------
/**
 * @brief   このフレームの起動処理を記述します。true で終了
 */
//--------------------------------------------------------------------------
bool NBRMultiPlayerSelectFrame::startup()
{
  m_pParentView->ChangeLowerSubView( View::LOWER_VIEW_MULTI_SELECT );
  m_pParentView->ChangeUpperSubView( View::UPPER_VIEW_VS );

  m_pLowerView = m_pParentView->GetLowerViewMultiSelect();

  // テキスト初期化
  m_pLowerView->InitializeText();
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::ALL_VIEW_OFF );
  m_pParentView->GetUpperViewVs()->SetAllVisible( true );

  // 上画面設定
  SetupUpperViewVs();

  // チームの　ふりわけを　きめてください
  m_pMesageMenuView->StartMessage( msg_nbr_win_16, View::MESSAGE_FINISH_INPUT );

  m_seq = SEQ_GUIDE_MESSAGE_WAIT;

  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー設定
 * @note    このフレームで実装するリスナーの登録処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::setListener()
{
  m_pNet->RegistRecvListener( this );
  m_pNet->AddRecvPacketEnableCommand( NBRNet::NET_GAME_COMMAND_END );
  m_pLowerView->SetListener( this );
  m_pMesageMenuView->SetListener( this );
}

//--------------------------------------------------------------------------
/**
 * @brief   リスナー解除
 * @note    このフレームで実装するリスナーの解除処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::removeListener()
{
  m_pNet->UnregistRecvListener();
  m_pNet->ClearRecvPacketEnableCommand();
  m_pLowerView->RemoveListener();
  m_pMesageMenuView->RemoveListener();
}

//--------------------------------------------------------------------------
/**
 * @brief   更新
 * @note    このフレームのシーケンス制御処理を記述します。
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::updateSequence()
{
  switch( m_seq ){
  case SEQ_GUIDE_MESSAGE_WAIT:     //! ガイドメッセージ終了待ち
    {
      // メッセージ終了待ち
      if( m_pMesageMenuView->IsEndMessage() )
      {
        // 基本画面に変更
        ChangeMainView();
        m_seq = SEQ_TEAM_BUTTON_WAIT;
      }
    }
    break;
  case SEQ_TEAM_BUTTON_WAIT:       //! チームボタン押下待ち
    // ボタン押下待ち
    break;
  case SEQ_MULTI_SELECT_SEND:      //! マルチ振り分け選択送信
    {
      // 立ち位置送信
      if( IsTimeOut() && m_pNet->SendMultiSelect( m_targetNetId, m_targetPlace ) )
      {
        // 立ち位置更新
        if( PlayerPosUpdate() == false )
        {
          // 基本画面に変更
          ChangeMainView();
          m_seq = SEQ_TEAM_BUTTON_WAIT;
        }
      }
    }
    break;
  case SEQ_MULTI_DECIDE_SEND:      //! マルチ振り分け決定送信
    {
      // 決定した立ち位置コピー
      u8 place[ NBRNetDefine::NET_NUM_MAX ] ={};
      for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
      {
        if( m_decideNetId[i] == -1 ) { GFL_ASSERT(0); }
        place[ m_decideNetId[i] ] = i;
      }

      m_pNBRWork->m_place[0] = static_cast<BTL_CLIENT_ID>(place[0]);
      m_pNBRWork->m_place[1] = static_cast<BTL_CLIENT_ID>(place[1]);
      m_pNBRWork->m_place[2] = static_cast<BTL_CLIENT_ID>(place[2]);
      m_pNBRWork->m_place[3] = static_cast<BTL_CLIENT_ID>(place[3]);

      // 決定送信
      if( IsTimeOut() && m_pNet->SendMultiDecide( place ) )
      {
        m_seq = SEQ_TIMING_TO_BATTLE_START;
      }
    }
    break;
  case SEQ_TIMING_TO_BATTLE_START: //! バトルへ同期
    {
      // 同期待ち
      if( m_pNet->IsTimingLoopCheck( NBRNetDefine::TIMING_COMMAND_MULTI_DECIDE ) )
      {
        exitFrame( MultiPlayerSelectResult::TO_BATTLE_START );
      }
    }
    break;
  }
}


//--------------------------------------------------------------------------
/**
 * @brief   このフレームが終了してもよいかの判定を記述します。
 * @return  trueで終了してよい
 */
//--------------------------------------------------------------------------
bool NBRMultiPlayerSelectFrame::cleanup()
{
  // メッセージの最低表示時間経過待ち
  if( IsTimeOut() == false )
  {
    return false;
  }

  // 選択肢コマンドが出ている場合は消す
  if( m_pMesageMenuView->IsShowList() == true )
  {
    m_pMesageMenuView->HideMessage();
  }

  return true;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
// NetRecvListener
//------------------------------------------------------------------
/**
 * @brief  通信終了通知
 */
//------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::OnNBRNetRecvEnd()
{
  ICHI_PRINT("OnNBRNetRecvEnd\n");
  // 下画面非表示
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::ALL_VIEW_OFF );
  // 選択肢表示中なら消す
  if( m_pMesageMenuView->IsShowList() )
  {
    m_pMesageMenuView->HideMessage();
  }
  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_OPPONENT;
  exitFrame( MultiPlayerSelectResult::APP_END );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
// NBRLowerViewMultiSelectListener
//--------------------------------------------------------------------------
/**
 * @brief     指定番目のプレイヤー選択ボタン押下通知
 * @param[in] 1～4番目
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::OnPlayerSelectButtonAction( int no )
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnPlayerSelectButtonAction no%d\n", no);
  GFL_ASSERT_MSG( m_seq==SEQ_TEAM_BUTTON_WAIT , "SEQ ERROR %d \n", m_seq );

  // ボタン番号を立ち位置のルールに合うように変換
  if( no == 1 ){ no = 2; }
  else if( no == 2 ){ no = 1; }

  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    Savedata::MyStatus* myStatus[NBRNetDefine::NET_NUM_MAX];
    for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
    {
      myStatus[netId] = m_pNBRWork->GetMyStatusFromNetId(netId);
    }

    // その場所に人が入っていたら、「おまかせ」は表示しない
    m_randomSelectEnable = ( m_decideNetId[ no ] == -1 ) ? true : false;

    // @fix GFNMcat[2128]：表示の番号のみを変更する。※noは立ち位置なので変更しない！
    int viewPos = no;
    switch( viewPos ){
    case 0: viewPos = 0; break; // BTL_CLIENT_PLAYER
    case 1: viewPos = 0; break; // BTL_CLIENT_ENEMY1
    case 2: viewPos = 1; break; // BTL_CLIENT_PARTNER
    case 3: viewPos = 1; break; // BTL_CLIENT_ENEMY2
    }

    // 選択肢表示
    m_pMesageMenuView->ShowMutliPlayerList( viewPos, myStatus, m_randomSelectEnable, m_decideNetId[ no ] ); 

    // ボタンIndexとNetIdの対応テーブル作成しておく（ボタン可変だから）
    {
      gfl2::std::MemClear( m_buttonIndex2NetIdTbl, sizeof(m_buttonIndex2NetIdTbl) );
      int index=0;

      // 「おまかせ」ボタン
      if( m_randomSelectEnable == true )
      {
        m_buttonIndex2NetIdTbl[ index++ ] = BUTTON_INDEX_RANDOM;
      }

      // プレイヤーボタン
      for( int netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
      {
        // 選択しているボタンの人と被ったら除外
        if( m_decideNetId[ no ] == netId )
        {
          continue;
        }
        m_buttonIndex2NetIdTbl[ index++ ] = netId;
      }
      // 戻るボタン
      m_buttonIndex2NetIdTbl[ index ] = BUTTON_INDEX_RETURN;
    }

    // カーソル非表示
    m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::CURSOR_OFF );

    m_targetPlace = no;
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   決定ボタン押下通知
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::OnDecideButtonAction()
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnDecideButtonAction\n");

  ChangeNetworkWaitView();
  m_seq = SEQ_MULTI_DECIDE_SEND;
}

//--------------------------------------------------------------------------
/**
 * @brief   戻るボタン通知
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::OnBackButtonAction()
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnBackButtonAction\n");

  // やめますか[はい・いいえ]の選択肢出す
  m_pMesageMenuView->ShowAppEndList();

  // ボタン出てる時はカーソル非表示
  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::CURSOR_OFF );
  }
}


//------------------------------------------------------------------
// NBRLowerViewMessageMenuListener
//------------------------------------------------------------------
/**
 * @brief   はいボタン通知
 */
void NBRMultiPlayerSelectFrame::OnYesButtonAction()
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnYesButtonAction\n");

  // 画面を終了時のメッセージにしておく
  m_pMesageMenuView->ShowAppEndMessage();
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::ALL_VIEW_OFF );

  // 終了フレームへ
  m_pNBRWork->m_appEndFrameEndType = ApplicationSystem::NBRWork::END_TYPE_ONESELF;
  exitFrame( MultiPlayerSelectResult::APP_END );
}

/**
 * @brief   いいえボタン通知
 */
void NBRMultiPlayerSelectFrame::OnNoButtonAction()
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnNoButtonAction\n");

  // ボタン出てた時はカーソル表示
  if( m_seq == SEQ_TEAM_BUTTON_WAIT )
  {
    m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::CURSOR_ON );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   マルチ用プレイヤー選択ボタン通知
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::OnMutliButtonAction( u32 buttonIndex )
{
  ICHI_PRINT(">ICHI NBRMultiPlayerSelectFrame::OnMutliButtonAction %d \n", buttonIndex);
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::CURSOR_ON );

  ICHI_PRINT(">ICHI m_buttonIndex2NetIdTbl %d \n", m_buttonIndex2NetIdTbl[ buttonIndex ]);
  switch( m_buttonIndex2NetIdTbl[ buttonIndex ] ){
  case BUTTON_INDEX_RANDOM:
    // ランダム
    m_targetNetId = GetEmptyPlaceNetIdAtRandom();
    break;
  case BUTTON_INDEX_RETURN:
    // 戻るボタン
    return;
  default:
    m_targetNetId = m_buttonIndex2NetIdTbl[ buttonIndex ];
    break;
  }

  // 通信待機中画面に変更
  ChangeNetworkWaitView();

  // 立ち位置送信シーケンスへ
  m_seq = SEQ_MULTI_SELECT_SEND;
}

//--------------------------------------------------------------------------
/**
 * @brief 基本画面にする
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::ChangeMainView()
{
  m_pMesageMenuView->HideMessage();
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::ALL_VIEW_ON );

  // 全部選択済みなら、決定ボタンアクティブ
  if( IsAllPlaceDecide() )
  {
    m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::TEAM_DECIDE_BUTTON_ACTIVE );
  }
  else
  {
    m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::TEAM_DECIDE_BUTTON_PASSIVE );
  }
}

//--------------------------------------------------------------------------
/**
 * @brief つうしん中の画面にする
 */
//--------------------------------------------------------------------------
void NBRMultiPlayerSelectFrame::ChangeNetworkWaitView()
{
  // 通信待機中
  m_pMesageMenuView->ShowMessage( msg_nbr_win_01, true );
  m_pLowerView->SetVisible( View::NBRLowerViewMultiSelect::RETURN_BUTTON_ONLY_ON );
  TimeOutStart();
}

//--------------------------------------------------------------------------
/**
 * @brief 立ち位置の更新
 * @return 3つ目決まったので4つ目も自動で決まった時にtrue
 */
//--------------------------------------------------------------------------
bool NBRMultiPlayerSelectFrame::PlayerPosUpdate()
{
  // 既にどこかの場所に入っているプレイヤー名を選んだ場合は、
  // 既に入っていた場所を空白にします。
  {
    for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
    {
      if( m_decideNetId[i] == m_targetNetId )
      {// 既に入っていた
        m_decideNetId[i] = -1; // 初期化
        // 入っていたところを空白にする
        m_pLowerView->DecidePosPlayer( i, NULL );
        // 上画面更新
        m_pParentView->GetUpperViewVs()->SetPlayerData( i, NULL );
      }
    }
  }

  // ボタンに名前反映
  m_pLowerView->DecidePosPlayer( m_targetPlace, m_pNBRWork->GetMyStatusFromNetId(m_targetNetId) );
  // 上画面更新
  m_pParentView->GetUpperViewVs()->SetPlayerData( m_targetPlace, m_pNBRWork->GetMyStatusFromNetId(m_targetNetId) );

  // 決めた事保存
  m_decideNetId[ m_targetPlace ] = m_targetNetId;
  ICHI_PRINT(">ICHI NetID=%d を 立ち位置%dに決定\n", m_targetNetId, m_targetPlace );

  // 3つ場所を入れたら4つ目も自動で入れる
  {
    int targetPlace = 0;
    u8 count=0;
    for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
    {
      if( m_decideNetId[i] == -1 )
      {
        count++;
        targetPlace = i;
      }
    }
    // 残り1つだったら自動で決める
    if( count == 1 )
    {
      m_targetPlace = targetPlace;
      m_targetNetId = GetEmptyPlaceNetIdAtRandom();
      ICHI_PRINT(">ICHI 残り1つなので自動で決める\n");

      // @fix GFNMcat[1299]【「マルチバトル」の「チーム割り振り画面」にて全チームを割り振っても「このチームで決定」アイコンが半透明のまま】対処
      m_decideNetId[ m_targetPlace ] = m_targetNetId;
      ICHI_PRINT(">ICHI NetID=%d を 立ち位置%dに決定\n", m_targetNetId, m_targetPlace );

      return true;
    }
    
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 全ての立ち位置決定したかチェック
 * @return trueで全て決定した
 */
//--------------------------------------------------------------------------
bool NBRMultiPlayerSelectFrame::IsAllPlaceDecide()
{
  for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
  {
    if( m_decideNetId[i] == -1 )
    {
      return false;
    }
  }
  return true;
}

//--------------------------------------------------------------------------
/**
 * @brief 空の立ち位置のNetIDをランダムで取得
 */
//--------------------------------------------------------------------------
int NBRMultiPlayerSelectFrame::GetEmptyPlaceNetIdAtRandom()
{
  int emptyNetId[NBRNetDefine::NET_NUM_MAX] ={};
  u32 emptyNum = 0;;

  for( u8 netId=0; netId<NBRNetDefine::NET_NUM_MAX; netId++ )
  {
    bool isFoundNetId = false;
    for( u8 i=0; i<BTL_CLIENT_NUM; i++ )
    {
      if( m_decideNetId[i] == netId )
      {
        isFoundNetId = true;
        break;
      }
    }
    // 見つからなかったら空なので格納
    if( !isFoundNetId )
    {
      emptyNetId[ emptyNum ] = netId;
      emptyNum++;
    }
  }
  if( emptyNum == 0 )
  {
    GFL_ASSERT(0);
    return 0;
  }

  return emptyNetId[ System::GflUse::GetPublicRand( emptyNum ) ];
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
