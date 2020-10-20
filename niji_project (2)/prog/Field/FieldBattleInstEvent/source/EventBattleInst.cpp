//======================================================================
/**
* @file EventBattleInst.cpp
* @date 2016/02/06 18:45:46
* @author onoue_masayuki
* @brief バトル施設関連イベント
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include "Field/FieldBattleInstEvent/include/EventBattleInst.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "Savedata/include/MyStatus.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "FieldScript/include/ScriptWork.h"

#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN(BattleInst)

EventBattleInstCommBase::EventBattleInstCommBase( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap ),
  mpP2pPacketTransfer(NULL),
  isFinishTransfer(false),
  mpInst(NULL),
  mpHeap(NULL)
{
}

//デストラクタ
EventBattleInstCommBase::~EventBattleInstCommBase()
{
  GFL_SAFE_DELETE(mpP2pPacketTransfer);
}

// 起動チェック
bool EventBattleInstCommBase::BootChk( GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstCommBase::InitFunc( GameSys::GameManager* gmgr)
{
  mpHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  mpP2pPacketTransfer = GFL_NEW( mpHeap ) NetLib::P2P::P2pPacketTransfer( mpHeap, this );
  mpInst = gmgr->GetBattleInstManager()->GetBattleInst();
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstCommBase::EndFunc( GameSys::GameManager* gmgr)
{
  GFL_SAFE_DELETE( mpP2pPacketTransfer );
}

void EventBattleInstCommBase::SendDataCommand( void* pSendData, u32 sendSize, CommCommandID commandID )
{
  isFinishTransfer = false;
  currentCommand = commandID;
  mpP2pPacketTransfer->SendDataCommand( pSendData, sendSize, currentCommand );
  ONOUE_PRINT("BINSTCOMM Send %d\n",currentCommand);
}

//--------------------------------------------------------------
/**
* @brief   パケットマネージャーの更新
* @return  通信が確立しているときはtrue
*/
//--------------------------------------------------------------
bool EventBattleInstCommBase::UpdateP2pPacketTransfer()
{
  bool isError = GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError();
  bool isSafe = (!isError && !mpInst->GetIsCommExit() && mpP2pPacketTransfer);
  if( isSafe )
  {
    mpP2pPacketTransfer->Update();
  }

  return isSafe;
}

EventBattleInstConnect::EventBattleInstConnect( gfl2::heap::HeapBase* pHeap )
: EventBattleInstCommBase( pHeap ),
  seq(SEND_COMM_DATA)
{
}

void EventBattleInstConnect::StartEvent( GameSys::GameManager* gmgr )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleInstConnect>( pGameEventManager );
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstConnect::MainFunc( GameSys::GameManager* gmgr)
{
  if( !UpdateP2pPacketTransfer() )
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventBattleInstDisconnect>( pGameEventManager );
    return GameSys::GMEVENT_RES_FINISH;
  }

  switch (seq)
  {
  case SEND_COMM_DATA:
    {
      mpInst->SetupSendData();

      mpInst->GetSendCommData( &mSendData );
      Savedata::MyStatus* pMyStatus = gmgr->GetGameData()->GetPlayerStatus();
      ONOUE_PRINT("BINSTCOMM Sed TransferdID = %d\n",pMyStatus->GetTransferdID());
      SendDataCommand( &mSendData, sizeof(mSendData), COMMAND_ID_SEND_COMMDATA );
      seq++;
    }
    break;
  case LAST_RECIVE_WAIT:
    if( isFinishTransfer )
    {
      mpInst->SetCommData( &mReciveData );
      Savedata::MyStatus* pMyStatus = gmgr->GetGameData()->GetPlayerStatus();
      BattleInst::CommRecieveData* pCommRecieveData = mpInst->GetCommPartnerData();
      mMyCheckResult = CheckResult();
      
      // GFNMCat[3097] バトルツリーで違うルールを選んでマッチングすると通信切断が行なわれなくなる
      mpInst->SetIsComm( true );//マルチプレイ設定

      // NMCat[1056] バトルツリーで違うルールを選んでマッチングするととまる
      if( mMyCheckResult == Field::BATTLE_INST_CHECK_RESULT_OK )
      {
        // TransferdIDの大きいほうが親
        bool isParent = pMyStatus->GetTransferdID() > pCommRecieveData->MyStatus->GetTransferdID();
        mpInst->SetIsParent( isParent );//親かどうかを保存

        int myWinNum = mSendData.WinNum;
        int otherWinNum = mReciveData.WinNum;
        int commWinCount = myWinNum > otherWinNum ? myWinNum : otherWinNum;//大きい方に合わせる
        mpInst->SetCommWinCount( commWinCount );//マルチプレイ時の共通連勝数


        ONOUE_PRINT("BINSTCOMM Recive TransferdID = %d\n",pCommRecieveData->MyStatus->GetTransferdID());
        ONOUE_PRINT("BINSTCOMM isParent = %d\n",isParent);

        ONOUE_PRINT("BINSTCOMM mMyCheckResult = %d\n",mMyCheckResult);
      }

      SendDataCommand( &mMyCheckResult, sizeof(Field::BattleInstCommCheckResult), COMMAND_ID_SEND_CHECK_RESULT );
      seq++;
    }
    break;
  case RECIVE_CHECK_RESULTWAIT:
    if(isFinishTransfer)
    {
      // エラー結果のマージ
      if( mMyCheckResult == mReciveCheckResult )
      {
        // 自分のエラーコードを採用（未知のエラーはなし）
        mpInst->SetCommCheckResult(mMyCheckResult);
      }
      else if( mMyCheckResult != Field::BATTLE_INST_CHECK_RESULT_OK )
      {
        // 自分のエラーコードを採用（未知のエラーはなし）
        mpInst->SetCommCheckResult(mMyCheckResult);
      }
      else if(mReciveCheckResult != Field::BATTLE_INST_CHECK_RESULT_OK )
      {
        // 受け取った側のエラーコードを採用する場合は念のため未知のエラーかチェックする
        if( Field::BATTLE_INST_CHECK_RESULT_NIJI_MAX <= mReciveCheckResult  )
        {
          // 不明なエラーコード
          mpInst->SetCommCheckResult(Field::BATTLE_INST_CHECK_RESULT_NG_ETC);
        }else{
          // 知っているエラーコード
          mpInst->SetCommCheckResult(mReciveCheckResult);
        }
      }
      ONOUE_PRINT("BINSTCOMM CheckResult = %d\n",mpInst->GetCommCheckResult());
      seq++;
    }
    break;

  case END:
    // [fix]GFCHECK[8230] 到達しないコード部分を削除
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   通信結果をチェック
* @return  BattleTreeCommCheckResult  チェック結果
*/
//--------------------------------------------------------------
Field::BattleInstCommCheckResult EventBattleInstConnect::CheckResult()
{
#if PM_DEBUG
  if( Field::Debug::DebugTypes::s_battleInstDebugBtlInstForceCommError != -1 )
  {
    return (Field::BattleInstCommCheckResult)Field::Debug::DebugTypes::s_battleInstDebugBtlInstForceCommError;
  }
#endif

  BattleInst::CommRecieveData* pCommRecieveData = mpInst->GetCommPartnerData();
  if( mpInst->GetRank() !=  pCommRecieveData->Data.SuperRank )
  {
    return Field::BATTLE_INST_CHECK_RESULT_NG_RANK;
  }

  // momiji追加: niji互換切る対応
  // 相手がnijiだったらエラーにする
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  if( pP2pConnectionManager->IsNijiConnect() )
  {
    GFL_PRINT("相手がnijiなのでエラーにして切断する\n");
    return Field::BATTLE_INST_CHECK_RESULT_NG_ETC;
  }

  return Field::BATTLE_INST_CHECK_RESULT_OK;
}

//--------------------------------------------------------------
/**
* @brief   パケット受信コールバック
*/
//--------------------------------------------------------------
void EventBattleInstConnect::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();
  int netID = 0;

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    // [fix]GFCHECK[8230] ログ表示用の変数を削除
    switch (currentCommand)
    {
    case COMMAND_ID_SEND_COMMDATA:
      {
        pP2pPacketTransfer->GetReceivedData( i, currentCommand, netID, &mReciveData, sizeof(mReciveData) );
      }
      break;
    case COMMAND_ID_SEND_CHECK_RESULT:
      pP2pPacketTransfer->GetReceivedData( i, currentCommand, netID, &mReciveCheckResult, sizeof(mReciveCheckResult) );
      break;
    }
  }

  isFinishTransfer = true;
}


EventBattleInstTradeTrainerData::EventBattleInstTradeTrainerData( gfl2::heap::HeapBase* pHeap )
: EventBattleInstCommBase( pHeap ),
  seq(CREATE_TRAINER_DATA),
  dummyCode(-1)
{
}

void EventBattleInstTradeTrainerData::StartEvent( GameSys::GameManager* gmgr )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleInstTradeTrainerData>( pGameEventManager );
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstTradeTrainerData::MainFunc( GameSys::GameManager* gmgr)
{
  if( !UpdateP2pPacketTransfer() )
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventBattleInstDisconnect>( pGameEventManager );
    return GameSys::GMEVENT_RES_FINISH;
  }

  switch (seq)
  {
  case CREATE_TRAINER_DATA:
    {
      if ( mpInst->GetIsParent() )
      {
        int data_idx1 = mpInst->SelectTrainer(-1);
        int data_idx2 = mpInst->SelectTrainer(data_idx1);
        mpInst->SetCommVsTrainerNo( data_idx1, data_idx2 );

        mpInst->SetNoEntryTrainer( 0, data_idx1 );//次の抽選から外す
        mpInst->SetNoEntryTrainer( 1, data_idx2 );//次の抽選から外す
        
        // 親はこの段階でトレーナーデータを作成
        MakeTrainerData();

        //通信のために対戦ポケモンを設定
        mpInst->SelectVsPokemonForComm();
      }
      seq = SEND_TRAINER_DATA;
    }
    break;
  case SEND_TRAINER_DATA:
    {
      mpInst->GetCommVsTrainer(&mPartySendTmp);
      ONOUE_PRINT("CommVsTrainer size:%d\n",sizeof(mPartySendTmp));
      SendDataCommand( &mPartySendTmp, sizeof(mPartySendTmp), COMMAND_ID_SEND_TRAINER_DATA );
    }
    seq = RECIVE_TRAINER_DATA;
    break;
  case RECIVE_TRAINER_DATA:
    if( isFinishTransfer )
    {
      if ( !mpInst->GetIsParent() )
      {
        // 子はこの段階でトレーナーデータを作成
        mpInst->SetCommVsTrainer(&mPartyReciveTmp);
        MakeTrainerData();
      }
      seq++;
    }
    break;
  case END:
    // [fix]GFCHECK[8230] 到達しないコード部分を削除
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   パケット受信コールバック
*/
//--------------------------------------------------------------
void EventBattleInstTradeTrainerData::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();
  int netID = 0;

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    // [fix]GFCHECK[8230] ログ表示用の変数を削除
    switch (currentCommand)
    {
    case COMMAND_ID_SEND_TRAINER_DATA:
      {
        // ダミーデータを送った側(親)はトレーナーデータを受け取る
        pP2pPacketTransfer->GetReceivedData( i, COMMAND_ID_SEND_TRAINER_DATA, netID, &mPartyReciveTmp, sizeof(mPartyReciveTmp) );
      }
    }
  }

  isFinishTransfer = true;
}


void EventBattleInstTradeTrainerData::MakeTrainerData()
{
  int data_idx1;
  int data_idx2;

  mpInst->GetCommVsTrainerNo( &data_idx1, &data_idx2 );
  mpInst->MakeTrainerDataWithBoss( 0, data_idx1 );
  mpInst->MakeTrainerDataWithBoss( 1, data_idx2 );
  ONOUE_PRINT("BINSTCOMM 通信用対戦相手取得終了 %d_%d isParent = %d\n",data_idx1, data_idx2,mpInst->GetIsParent() );
}


EventBattleInstCommYesNo::EventBattleInstCommYesNo( gfl2::heap::HeapBase* pHeap )
: EventBattleInstCommBase( pHeap ),
  seq(SEND_YES_NO)
{
}

void EventBattleInstCommYesNo::StartEvent( GameSys::GameManager* gmgr, Field::FieldScript::Work* pWork, u16 retWk,int answer )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  EventBattleInstCommYesNo* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleInstCommYesNo>( pGameEventManager );

  pEvent->mRetWk = retWk;
  pEvent->mAnswer = answer;
  pEvent->mpWork = pWork;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstCommYesNo::MainFunc( GameSys::GameManager* gmgr)
{
  if( !UpdateP2pPacketTransfer() )
  {
    GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
    GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, EventBattleInstDisconnect>( pGameEventManager );
    return GameSys::GMEVENT_RES_FINISH;
  }

  switch (seq)
  {
  case SEND_YES_NO:
    {
      Savedata::MyStatus* pMyStatus = gmgr->GetGameData()->GetPlayerStatus();
      ONOUE_PRINT("BINST:mAnswer:%d\n",mAnswer);
      SendDataCommand( &mAnswer, sizeof(int), COMMAND_ID_YES_NO );
      seq++;
    }
    break;
  case LAST_RECIVE_WAIT:
    if( isFinishTransfer )
    {
      // スクリプトに反映
      ONOUE_PRINT("BINST:mCommAnswer:%d\n",mCommAnswer);
      mpWork->SetTempWorkValue(mRetWk,mCommAnswer);
      seq++;
    }
    break;
  case END:
    // [fix]GFCHECK[8230] 到達しないコード部分を削除
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   パケット受信コールバック
*/
//--------------------------------------------------------------
void EventBattleInstCommYesNo::OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer )
{
  u32 receivedDataCount = pP2pPacketTransfer->GetReceivedDataCount();
  int netID = 0;

  for( u32 i = 0; i < receivedDataCount; ++i )
  {
    // [fix]GFCHECK[8230] ログ表示用の変数を削除
    switch (currentCommand)
    {
    case COMMAND_ID_YES_NO:
      {
        pP2pPacketTransfer->GetReceivedData( i, currentCommand, netID, &mCommAnswer, sizeof(int) );
      }
      break;
    }
  }

  isFinishTransfer = true;
}



void EventBattleInstDisconnect::StartEvent( GameSys::GameManager* gmgr )
{
  GameSys::GameEventManager* pGameEventManager = gmgr->GetGameEventManager();
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleInstDisconnect>( pGameEventManager );
}

EventBattleInstDisconnect::EventBattleInstDisconnect( gfl2::heap::HeapBase* pHeap )
: GameSys::GameEvent( pHeap ),
  mpInst(NULL),
  seq(START_DISCONNECT)
{
}

//デストラクタ
EventBattleInstDisconnect::~EventBattleInstDisconnect()
{
}

// 起動チェック
bool EventBattleInstDisconnect::BootChk( GameSys::GameManager* gmgr)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstDisconnect::InitFunc( GameSys::GameManager* gmgr)
{
  mpInst = gmgr->GetBattleInstManager()->GetBattleInst();
}
//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstDisconnect::MainFunc( GameSys::GameManager* gmgr)
{
  NetLib::P2P::P2pConnectionManager* pP2pConnectionManager = GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager);
  ONOUE_PRINT("BINSTCOMM exit seq  = %d\n",seq);

  switch (seq)
  {
  case START_DISCONNECT:
    if( !mpInst->GetIsCommExit() )
    {
      pP2pConnectionManager->DisconnectStart();
      seq = WAIT_DISCONNECT;
    }else{
      seq = END;
    }
    break;

  case WAIT_DISCONNECT:
    if( pP2pConnectionManager->IsDisconnectSuccess() == false )
    {
      break;
    }
    pP2pConnectionManager->Terminate();
    NetLib::NijiNetworkSystem::TerminateNetwork();
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->Clear();
    mpInst->SetIsCommExit();
    seq = END;
    break;
  case END:
    return GameSys::GMEVENT_RES_FINISH;
  default:
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   gmgr       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleInstDisconnect::EndFunc( GameSys::GameManager* gmgr)
{
}
GFL_NAMESPACE_END(BattleInst) 