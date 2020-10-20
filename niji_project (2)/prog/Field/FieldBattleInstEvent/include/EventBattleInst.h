//======================================================================
/**
 * @file EventBattleInst.h
 * @date 2016/02/06 18:46:02
 * @author onoue_masayuki
 * @brief バトル施設
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __EVENTBATTLEINST_H_INCLUDED__
#define __EVENTBATTLEINST_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameEvent.h"
#include "System/include/HeapDefine.h"
#include "NetStatic/NetLib/include/P2P/P2pPacketTransfer.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstDefine.h"
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"

namespace GameSys
{
  class GameManager;
}

namespace Field { namespace FieldScript {
  class Work;
}}

GFL_NAMESPACE_BEGIN(BattleInst)

class BattleInst;

/**
* @class EventBattleInstCommBase
* @brief バトル施設データ送受信イベント
*/
class EventBattleInstCommBase : 
  public GameSys::GameEvent,
  public NetLib::P2P::P2pPacketTransferListener
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstCommBase);

public:
  // コンストラクタ
  EventBattleInstCommBase( gfl2::heap::HeapBase* pHeap );
  //デストラクタ
  virtual ~EventBattleInstCommBase();
  virtual bool BootChk( GameSys::GameManager* gmgr);
  virtual void InitFunc( GameSys::GameManager* gmgr);
  virtual void EndFunc( GameSys::GameManager* gmgr);

protected:
  void SendDataCommand( void* pSendData, u32 sendSize, CommCommandID commandID );
  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer ) = 0;
  bool UpdateP2pPacketTransfer();

private:
  NetLib::P2P::P2pPacketTransfer*               mpP2pPacketTransfer;

protected:
  bool isFinishTransfer;
  CommCommandID currentCommand;
  BattleInst* mpInst;
  gfl2::heap::HeapBase* mpHeap;
};

/**
* @class EventBattleInstConnect
* @brief バトル施設データ送受信イベント
*/
class EventBattleInstConnect : 
  public EventBattleInstCommBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstConnect);

public:

  // コンストラクタ
  EventBattleInstConnect( gfl2::heap::HeapBase* pHeap );

  //--------------------------------------------------------------
  /**
  * @brief   イベントを起動
  * @param   gmgr       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  * @return  GMEVENT_RESULT  イベント制御関数の戻り値
  */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr);

  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

private:

  //--------------------------------------------------------------
  /**
  * @brief   通信結果をチェック
  * @return  BattleTreeCommCheckResult  チェック結果
  */
  //--------------------------------------------------------------
  Field::BattleInstCommCheckResult CheckResult();

  enum Sequence
  {
    SEND_COMM_DATA,
    LAST_RECIVE_WAIT,
    RECIVE_CHECK_RESULTWAIT,
    END,
  };
  int seq;
  BattleInst::CommSendData mSendData;
  BattleInst::CommSendData mReciveData;
  Field::BattleInstCommCheckResult mMyCheckResult;
  Field::BattleInstCommCheckResult mReciveCheckResult;
};


/**
* @class EventBattleInstTradeTrainerData
* @brief バトル施設データ送受信イベント
*/
class EventBattleInstTradeTrainerData : 
  public EventBattleInstCommBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstTradeTrainerData);

public:

  // コンストラクタ
  EventBattleInstTradeTrainerData( gfl2::heap::HeapBase* pHeap );


  //--------------------------------------------------------------
  /**
  * @brief   イベントを起動
  * @param   gmgr       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  * @return  GMEVENT_RESULT  イベント制御関数の戻り値
  */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr);

  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );
  
  void MakeTrainerData();

private:

  enum Sequence
  {
    CREATE_TRAINER_DATA,
    SEND_TRAINER_DATA,
    RECIVE_TRAINER_DATA,
    END,
  };
  int seq;
  int dummyCode;

  BattleInst::CommSendVsTrainer mPartySendTmp;
  BattleInst::CommSendVsTrainer mPartyReciveTmp;
};


/**
* @class EventBattleInstCommYesNo
* @brief バトル施設データ送受信イベント
*/
class EventBattleInstCommYesNo : 
  public EventBattleInstCommBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstCommYesNo);

public:

  // コンストラクタ
  EventBattleInstCommYesNo( gfl2::heap::HeapBase* pHeap );

  //--------------------------------------------------------------
  /**
  * @brief   イベントを起動
  * @param   gmgr       ゲームマネージャークラス
  */
  //--------------------------------------------------------------
  static void StartEvent( GameSys::GameManager* gmgr, Field::FieldScript::Work* pWork, u16 retWk,int answer );

  //--------------------------------------------------------------
  /**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   gmgr       ゲームマネージャークラス
  * @return  GMEVENT_RESULT  イベント制御関数の戻り値
  */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr);

  virtual void OnPacketTransferSuccess( NetLib::P2P::P2pPacketTransfer* pP2pPacketTransfer );

private:

  enum Sequence
  {
    SEND_YES_NO,
    LAST_RECIVE_WAIT,
    END,
  };
  int seq;

  u16 mRetWk;
  int mAnswer;
  int mCommAnswer;
  Field::FieldScript::Work* mpWork;

};


/**
* @class EventBattleInstDisconnect
* @brief 通信切断イベント
*/
class EventBattleInstDisconnect : 
  public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(EventBattleInstDisconnect);

public:
  static void StartEvent( GameSys::GameManager* gmgr );
  // コンストラクタ
  EventBattleInstDisconnect( gfl2::heap::HeapBase* pHeap );
  //デストラクタ
  virtual ~EventBattleInstDisconnect();

  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr);
  virtual bool BootChk( GameSys::GameManager* gmgr);
  virtual void InitFunc( GameSys::GameManager* gmgr);
  virtual void EndFunc( GameSys::GameManager* gmgr);
private:
  enum Sequence
  {
    START_DISCONNECT,
    WAIT_DISCONNECT,
    END,
  };
protected:
  BattleInst* mpInst;
  Sequence seq;
};

GFL_NAMESPACE_END(BattleInst)

#endif // __EVENTBATTLEINST_H_INCLUDED__