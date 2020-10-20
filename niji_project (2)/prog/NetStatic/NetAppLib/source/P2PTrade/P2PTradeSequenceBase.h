// ============================================================================
/*
 * @file P2PTradeSequenceBase.h
 * @brief P2P交換アプリで使用する通信モジュールです。
 * @date 2015.09.28
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADESEQUENCEBASE_H_INCLUDE )
#define P2PTRADESEQUENCEBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeNet.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeResponseListener.h"
#include "NetStatic/NetLib/include/P2P/P2pSyncSave.h"
#include "pml/include/pml_PokePara.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"
#include "NetStatic/NetAppLib/include/Util/NetAppPokemonValidationUtility.h"
#include "NetStatic/NetAppLib/include/Util/NetAppCommonSaveUtility.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeSequenceBase : public NetAppLib::P2PTrade::P2PTradeNetListener
#if defined( GF_PLATFORM_CTR )
  , public NetAppLib::Util::ServerErrorListener
#endif // defined( GF_PLATFORM_CTR )
{
public:

  P2PTradeSequenceBase(
    gfl2::heap::HeapBase*                         pHeap,
    bool&                                         bOutCallZukanRegister,
    app::event::ZukanRegisterEventParam&          outZukanRegisterEventParam,
    bool&                                         bOutCallEvolution,
    NetEvent::Evolution::EvolutionEventParam&     outEvolutionEventParam );
  virtual~P2PTradeSequenceBase();

  bool Initialize();
  void Finalize();

  bool CanDestroy();

  virtual void Update( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener ) = 0;
  virtual void OnRecvDataCommand( void* pRecvData, u8 commandID ) = 0;
  virtual void AddFirstTradeRequest( const pml::pokepara::PokemonParam& pokemonParam, u8 tray, u8 pos ) = 0;
  virtual void CancelFirstTradeRequest() = 0;
  virtual void AddFinalTradeRequest() = 0;
  virtual void CancelFinalTradeRequest() = 0;
  virtual void AddSendTradeCancelRequest() = 0;
  virtual void AddTradeExitRequest() = 0;
  virtual void AddFinalSyncRequest() = 0;
  virtual void AddFinalSyncCancelRequest() = 0;

  // @note : P2PTradeEvent 専用関数( BoxからP2PTradeProcへ切り替える時に同期セーブを走らせる関数 )
  virtual void StartSyncSaveRequest();
  // @note : P2PTradeEvent 専用関数( Box起動時に全フラグを初期化する関数 )
  virtual void ResetFlag();

  u8* GetSendPokemonBuffer(){ return m_SendPokemonBuffer; }
  pml::pokepara::PokemonParam& GetRecvPokemonParam(){ return m_RecvPokemonParam; }

  bool IsFinalTradeSuccess(){ return m_bIsFinalTradeSuccess; }
  bool IsFinalTradeComplete(){ return m_bIsFinalTradeComplete; }

protected:

  enum State
  {
    STATE_IDLE,
    STATE_SEND_POKEMON,                          // 親子
    STATE_RECV_POKEMON,                          // 親子
    STATE_SEND_VERIFY_START,                     // 親
    STATE_VERIFY,                                // 親子
    STATE_SEND_VERIFY_RESULT,                    // 子
    STATE_RECV_VERIFY_RESULT,                    // 親
    STATE_SEND_FIRST_TRADE_RESULT,               // 親
    STATE_RECV_FIRST_TRADE_RESULT,               // 子
    STATE_NOTIFICATION_FIRST_TRADE_RESULT,       // 親子
    STATE_WAIT_FINAL_TRADE_REQUEST,              // 親子
    STATE_SEND_FINAL_TRADE,                      // 子
    STATE_RECV_FINAL_TRADE,                      // 親
    STATE_SEND_FINAL_TRADE_RESULT,               // 親
    STATE_RECV_FINAL_TRADE_RESULT,               // 子
    STATE_NOTIFICATION_FINAL_TRADE_SUCCESS,      // 親子
    STATE_SAVE_PREPARE,                          // 親子
    STATE_SAVE_START,                            // 親子
    STATE_SAVE_WAIT,                             // 親子
    STATE_SYNC_SAVE_PREPARE_SYNC_START,          // 親子
    STATE_SYNC_SAVE_PREPARE_SYNC_WAIT,           // 親子
    STATE_SYNC_SAVE_START,                       // 親子
    STATE_SYNC_SAVE_WAIT,                        // 親子
    STATE_NOTIFICATION_FINAL_TRADE_COMPLETE,     // 親子
    STATE_SEND_FORCE_CANCEL,                     // 親子
    STATE_FINAL_SYNC                             // 親子
  };

  enum CommandID
  {
    COMMAND_ID_POKEMON_PARAM,
    COMMAND_ID_VERIFY_START,
    COMMAND_ID_VERIFY_RESULT,
    COMMAND_ID_FIRST_TRADE_RESULT,
    COMMAND_ID_CANCEL_FIRST_TRADE,
    COMMAND_ID_FINAL_TRADE,
    COMMAND_ID_FINAL_TRADE_RESULT,
    COMMAND_ID_CANCEL_FINAL_TRADE,
    COMMAND_ID_FORCE_CANCEL,
    COMMAND_ID_EXIT,
    COMMAND_ID_EXIT_RESPONSE,
    COMMAND_ID_FINAL_SYNC,
    COMMAND_ID_FINAL_SYNC_COMPLETE,
    COMMAND_ID_CANCEL_FINAL_SYNC,
    COMMAND_ID_CANCEL_FINAL_SYNC_COMPLETE
  };

  enum VerifyResult
  {
    VERIFY_RESULT_ERROR,
    VERIFY_RESULT_SUCCESS,
    VERIFY_RESULT_POKEMON_INVALID
  };

  enum SyncNo
  {
    SYNC_NO_1 = 87,
    SYNC_NO_2 = 88,
    SYNC_NO_3 = 89,
    SYNC_NO_4 = 90
  };

  typedef struct
  {
    u8      tradeSuccess;
    u8      verifyResultHost;
    u8      verifyResultGuest;
    u8      reserve;
  } FirstTradeResult;

  State getState();
  void setState( State eState );

  virtual void resetFlag();

  virtual void tradeCancel( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener ) = 0;
  virtual void tradeExit( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener ) = 0;
  virtual void finalSync( NetAppLib::P2PTrade::P2PTradeResponseListener* pP2PTradeResponseListener ) = 0;

  void startSyncSave();
  bool updateSyncSave();

#if defined( GF_PLATFORM_CTR )
  //サーバエラーが発生した瞬間に通知する為だけのイベント
  virtual void OnServerError( void );
#endif // defined( GF_PLATFORM_CTR )

public:

  virtual void DebugPrint();

protected:

  gfl2::heap::HeapBase*                             m_pHeap;
#if defined( GF_PLATFORM_CTR )
  NetAppLib::Util::NetAppPokemonValidationUtility   m_NetAppPokemonValidationUtility;
#endif // defined( GF_PLATFORM_CTR )
  NetAppLib::Util::NetAppCommonSaveUtility          m_NetAppCommonSaveUtility;
  bool&                                             m_bCallZukanRegister;
  app::event::ZukanRegisterEventParam&              m_ZukanRegisterEventParam;
  bool&                                             m_bCallEvolution;
  NetEvent::Evolution::EvolutionEventParam&         m_EvolutionEventParam;
  bool                                              m_bInBox;
  u32                                               m_Tray;
  u32                                               m_Pos;

  NetAppLib::P2PTrade::P2PTradeNet                  m_P2PTradeNet;
  u8                                                m_SendPokemonBuffer[ pml::pokepara::PokemonParam::DATASIZE ];
  pml::pokepara::PokemonParam                       m_RecvPokemonParam;
  bool                                              m_RecvPokemonFlag;

  VerifyResult                                      m_eVerifyResult;
  FirstTradeResult                                  m_FirstTradeResult;
  bool                                              m_bCancelFirstTradeFlag;
  bool                                              m_bRecvCancelFirstTradeFlag;
  bool                                              m_bCancelFinalTradeFlag;
  bool                                              m_bRecvCancelFinalTradeFlag;
  bool                                              m_bRecvForceCancelFlag;
  bool                                              m_bExitTradeFlag;
  bool                                              m_bRecvExitTradeFlag;
  bool                                              m_bRecvExitResponseFlag;

  bool                                              m_bIsFinalTradeSuccess;
  bool                                              m_bIsFinalTradeComplete;

  u16                                               m_sendMonsNo;
  u8                                                m_sendFormNo;

  bool                                              m_bIsStartSyncSaveRequestFlag;

  bool                                              m_bCallExitReceivedHandler;

private:

  NetLib::P2P::P2pSyncSave*                         m_pP2pSyncSave;
  State                                             m_eState;

};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADESEQUENCEBASE_H_INCLUDE
