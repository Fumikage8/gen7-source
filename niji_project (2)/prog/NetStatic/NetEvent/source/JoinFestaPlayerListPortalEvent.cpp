// ============================================================================
/*
 * @file JoinFestaPlayerListPortalEvent.cpp
 * @brief JoinFestaPlayerListSelect系のポータルイベント
 * @date 2015.12.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetStatic/NetEvent/include/JoinFestaPlayerListPortalEvent.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

//プレイヤーリスト系イベント
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaPlayerListSelectMultiEvent.h"
#include "NetStatic/NetEvent/include/JoinFestaBeaconEvent.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"

GFL_NAMESPACE_BEGIN(NetEvent)


// コンストラクタ
JoinFestaPlayerListPortalEvent::JoinFestaPlayerListPortalEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap )
,m_requestId( Field::SubScreen::OUT_REQ_ID_NONE )
,m_originalRequestId( Field::SubScreen::OUT_REQ_ID_NONE )
,m_listSelectAppResult()
,m_listSelectMultiAppResult()
,m_beaconAppParam()
{
  ::std::memset( &m_listSelectAppResult , 0 , sizeof(m_listSelectAppResult) );
  ::std::memset( &m_listSelectMultiAppResult , 0 , sizeof(m_listSelectMultiAppResult) );
  ::std::memset( &m_beaconAppParam , 0 , sizeof(m_beaconAppParam) );
  GFL_PRINT("JoinFestaPlayerListPortalEvent 開始\n");
}

// デストラクタ
JoinFestaPlayerListPortalEvent::~JoinFestaPlayerListPortalEvent()
{
  GFL_PRINT("JoinFestaPlayerListPortalEvent 終了\n");
}


//--------------------------------------------------------------
// イベントを動かしてよいかどうかの判定関数 純粋仮想関数
//--------------------------------------------------------------
bool JoinFestaPlayerListPortalEvent::BootChk( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );
  return true;
}

//--------------------------------------------------------------
// bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
//--------------------------------------------------------------
void JoinFestaPlayerListPortalEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  GFL_UNUSED( pGameManager );
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
void JoinFestaPlayerListPortalEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


//--------------------------------------------------------------
// 呼び出される関数 純粋仮想関数
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT JoinFestaPlayerListPortalEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  enum {
    SEQ_CALL_REQUEST_EVENT,   //! リクエストされたイベント呼び出し
    SEQ_RETURN_EVENT,         //! イベント戻り
    SEQ_END,                  //! 終了
  };

  switch( GetSeqNo() ){
  case SEQ_CALL_REQUEST_EVENT:   //! リクエストされたイベント呼び出し
    {
      CallAppEvent( pGameManager );
      SetSeqNo( SEQ_RETURN_EVENT ); 
    }
    break;
  case SEQ_RETURN_EVENT:         //! イベント戻り
    {
      // 通信エラー発生で即終了
      if( IsError() )
      {
        return GameSys::GMEVENT_RES_FINISH;
      }

      bool isEnd = ReturnAppEvent();
      if( isEnd == true )
      {
        SetSeqNo( SEQ_END ); 
      }
      else
      {
        SetSeqNo( SEQ_CALL_REQUEST_EVENT ); 
      }
    }
    break;
  case SEQ_END:                  //! 終了
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

/* -------------------------------------------------------------------------*/
/**
 *  @brief  起動するイベントを設定する
 *  @param  アプリ起動番号
 */
/* -------------------------------------------------------------------------*/
void JoinFestaPlayerListPortalEvent::SetRequestID( Field::SubScreen::FieldMenuOutRequestID requestId )
{
  GFL_PRINT("JoinFestaPlayerListPortalEvent::SetRequestID %d \n", requestId );
  m_requestId = requestId;
  m_originalRequestId = requestId; 
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリのイベント呼び出し
 */
/* -------------------------------------------------------------------------*/
void JoinFestaPlayerListPortalEvent::CallAppEvent( GameSys::GameManager* pGameManager )
{
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S:       //!< プレイヤーリスト（シングル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D:       //!< プレイヤーリスト（ダブル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE:          //!< プレイヤーリスト（交換選択モード１）
    {
      NetApp::JoinFestaPlayerListSelect::APP_PARAM appParam;//ダミー
      ::std::memset( &m_listSelectAppResult , 0 , sizeof(m_listSelectAppResult) );
      m_selectedPersonalDataArray[0].Reset();//先頭のだけ使う
      m_listSelectAppResult.pSelectJoinFestaPersonalData = &m_selectedPersonalDataArray[0];

      NetEvent::JoinFestaPlayerListSelect::JoinFestaPlayerListSelectEvent::StartEvent( pGameManager, appParam , &m_listSelectAppResult );
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M:       //!< プレイヤーリスト（マルチ対戦選択モード２）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R:       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
    {
      NetApp::JoinFestaPlayerListSelectMulti::APP_PARAM appParam;//ダミー  
      ::std::memset( &m_listSelectMultiAppResult , 0 , sizeof(m_listSelectMultiAppResult) );
      for( u32 index = 0 ; index < NetApp::JoinFestaBeacon::MULTI_CONNECT_NUM ; ++index )
      {
        m_selectedPersonalDataArray[index].Reset();
      }
      m_listSelectMultiAppResult.pSelectJoinFestaPersonalData1 = &m_selectedPersonalDataArray[0];
      m_listSelectMultiAppResult.pSelectJoinFestaPersonalData2 = &m_selectedPersonalDataArray[1];
      m_listSelectMultiAppResult.pSelectJoinFestaPersonalData3 = &m_selectedPersonalDataArray[2];

      NetEvent::JoinFestaPlayerListSelectMulti::JoinFestaPlayerListSelectMultiEvent::StartEvent( pGameManager, appParam , &m_listSelectMultiAppResult );
    }
    break;
  case  Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING:
    {
      //ビーコンマッチングアプリイベント呼び出し
      ::std::memset( &m_beaconAppParam , 0 , sizeof(m_beaconAppParam) );
      for( u32 index = 0 ; index < NetApp::JoinFestaBeacon::MULTI_CONNECT_NUM ; ++index )
      {
        m_beaconAppParam.m_pSelectedPersonalDataArray[index] = &m_selectedPersonalDataArray[index];
        if( m_selectedPersonalDataArray[index].IsEnable() )
        {
          m_beaconAppParam.connectNum++;
        }
      }

      //タイプリスト
      const u32 TYPE_INDEX_MAX = 5;
      const s32 TYPE_LIST[TYPE_INDEX_MAX][2] =
      {
        { Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE ,    NetApp::JoinFestaBeacon::TYPE_TRADE },
        { Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S , NetApp::JoinFestaBeacon::TYPE_BATTLE_S },
        { Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D , NetApp::JoinFestaBeacon::TYPE_BATTLE_D },
        { Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M , NetApp::JoinFestaBeacon::TYPE_BATTLE_M },
        { Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R , NetApp::JoinFestaBeacon::TYPE_BATTLE_R }
      };

      //タイプ設定
      for( u32 index = 0 ; index < TYPE_INDEX_MAX ; ++index )
      {
        if( m_originalRequestId == TYPE_LIST[index][0] )
        {
          m_beaconAppParam.type = TYPE_LIST[index][1];
          break;
        }
      }

      m_beaconAppParam.inviteType = NetApp::JoinFestaBeacon::TYPE_INVITE;

      //解除はJoinFestaEventLinkEventで行われる
      System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );

      NetEvent::JoinFestaBeacon::JoinFestaBeaconEvent::StartEvent( pGameManager, &m_beaconAppParam );
    }
    break;
  default:
    GFL_ASSERT_MSG(0,"%d",m_requestId);
    break;
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief アプリの結果確認
 * @return イベント終了の場合はtrue
 */
/* -------------------------------------------------------------------------*/
bool JoinFestaPlayerListPortalEvent::ReturnAppEvent()
{
  bool isSelected = false;

  //アプリの結果で分岐
  switch( m_requestId ){
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_S:       //!< プレイヤーリスト（シングル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_D:       //!< プレイヤーリスト（ダブル対戦選択モード１）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_TRADE:          //!< プレイヤーリスト（交換選択モード１）
    {
      if( m_listSelectAppResult.bIsSelectedJoinFestaPersonalData )
      {//選択した
        isSelected = true;
      }
    }
    break;
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_M:       //!< プレイヤーリスト（マルチ対戦選択モード２）
  case Field::SubScreen::OUT_REQ_ID_PLAYER_LIST_SELECT_BATTLE_R:       //!< プレイヤーリスト（ロイヤル対戦選択モード２）
    {
      if( m_listSelectMultiAppResult.bIsSelectedJoinFestaPersonalData )
      {//選択した
        isSelected = true;
      }
    }
    break;
  case  Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING:
    {
      if( m_beaconAppParam.isCancel )
      {
        // 元のプレイヤー選択へ戻る
        m_requestId = m_originalRequestId;
        return false;
      }
    }
    break;
  default:
    GFL_ASSERT_MSG(0,"%d",m_requestId);
    break;
  }


  // プレイヤー選択していた場合は、ビーコンマッチングでマッチング待ち
  if( isSelected )
  {
    m_requestId = Field::SubScreen::OUT_REQ_ID_BEACON_MATCHING; 
    return false;
  }

  return true;
}




GFL_NAMESPACE_END(NetEvent)
