// ============================================================================
/*
 * @file P2PTradeRequestClient.h
 * @brief P2P交換アプリで使用する通信リクエストを投げるクラスです。
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADEREQUESTCLIENT_H_INCLUDE )
#define P2PTRADEREQUESTCLIENT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include "pml/include/pml_PokePara.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeDefine.h"
#include "NetStatic/NetAppLib/source/P2PTrade/P2PTradeNet.h"
#include "App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h"
#include "NetStatic/NetEvent/include/EvolutionEvent.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeTypes.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeResponseListener;
class P2PTradeSequenceBase;


class P2PTradeRequestClient : public NetAppLib::P2PTrade::P2PTradeNetListener
{

public:

  P2PTradeRequestClient( gfl2::heap::HeapBase* pHeap );
  virtual~P2PTradeRequestClient();

  //------------------------------------------------------------------
  /**
    *  @brief    初期化処理
    *  @retval   true  初期化終了
    *  @retval   false 初期化中
    */
  //------------------------------------------------------------------
  bool Initialize();

  //------------------------------------------------------------------
  /**
    *  @brief    更新処理
    *  @note     P2PTradeEvent の Update で呼び出します
    */
  //------------------------------------------------------------------
  void Update();

  //------------------------------------------------------------------
  /**
    *  @brief    破棄できるか
    *  @note     P2PTradeEvent の MainFunc の SEQUENCE_FINISH の時に呼び出します
    */
  //------------------------------------------------------------------
  bool CanDestroy();

  //------------------------------------------------------------------
  /**
    *  @brief    P2P交換の通信レスポンスリスナーの登録を行います
    *  @param    pListener P2P交換の通信レスポンスリスナー
    */
  //------------------------------------------------------------------
  void SetP2PTradeResponseListener( NetAppLib::P2PTrade::P2PTradeResponseListener* pListener );

  //------------------------------------------------------------------
  /**
    *  @brief    P2P交換の通信レスポンスリスナーの解除を行います
    */
  //------------------------------------------------------------------
  void RemoveP2PTradeResponseListener();


  //------------------------------------------------------------------
  /**
    *  @brief    メッセージIDを送信するリクエストの追加を行います
    *  @param    eMessageID 送信するメッセージID
    *  @param    monsno 欲しいポケモンの種族番号
    *  @note     自分が AddSendMessageIDRequest をコールすると
    *            相手の OnMessageIDReceived がコールされます
    */
  //------------------------------------------------------------------
  void AddSendMessageIDRequest( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, const pml::pokepara::PokemonParam& pokemonParam );

  //------------------------------------------------------------------
  /**
    *  @brief    最後に受信したメッセージIDを取得します
    *  @return   最後に受信したメッセージID
    */
  //------------------------------------------------------------------
  NetAppLib::P2PTrade::MessageID GetLastRecvMessageID();

  //------------------------------------------------------------------
  /**
    *  @brief    最後に受信した欲しいポケモンの種族番号を取得します
    *  @return   最後に受信した欲しいポケモンの種族番号
    */
  //------------------------------------------------------------------
  MonsNo GetLastRecvMonsNo();

  //------------------------------------------------------------------
  /**
    *  @brief    最後に受信した欲しいポケモンのPokemonParamのバッファを取得します
    *  @return   最後に受信した欲しいポケモンのPokemonParamのバッファ
    */
  //------------------------------------------------------------------
  void* GetLastRecvPokemonParamBuffer();



  //------------------------------------------------------------------
  /**
    *  @brief    ポケモンのパラメータを送信するリクエストの追加を行います
    *  @param    pokemonParam 送信するポケモンのパラメータ
    *  @note     自分が AddSendPokemonParamRequest をコールすると
    *            相手の OnPokemonParamReceived がコールされます
    */
  //------------------------------------------------------------------
  void AddSendPokemonParamRequest( const pml::pokepara::PokemonParam& pokemonParam );

  //------------------------------------------------------------------
  /**
    *  @brief    最後に受信したポケモンのパラメータを取得します
    *  @return   最後に受信したポケモンのパラメータ
    */
  //------------------------------------------------------------------
  const pml::pokepara::PokemonParam& GetLastRecvPokemonParam();



  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換リクエストの追加を行います
    *  @param    pokemonParam 交換するポケモンのパラメータ
    *  @param    tray         ボックスのトレイ番号
    *  @param    pos          ボックス内の位置
    *  @note     最初のコマンドメニューで交換を選んだ時に使用します
    *            受信した相手のポケモンの不正チェックも行います
    *            自分が AddFirstTradeRequest をコールすると
    *            自分の OnFirstTradeSuccess
    *                   OnFirstTradePokemonInvalid
    *                   OnFirstTradeValidationError のどれかがコールされます
    */
  //------------------------------------------------------------------
  void AddFirstTradeRequest( const pml::pokepara::PokemonParam& pokemonParam, u8 tray, u8 pos );

  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換リクエストのキャンセルを行います
    *  @note     最初の交換リクエストを投げた後の待機画面で、
    *            Bボタンを押した時に使用します
    *            自分が CancelFirstTradeRequest をコールすると
    *            自分の OnFirstTradeCancel
    *                   OnFirstTradeSuccess
    *                   OnFirstTradePokemonInvalid
    *                   OnFirstTradeValidationError のどれかがコールされます
    */
  //------------------------------------------------------------------
  void CancelFirstTradeRequest();



  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換リクエストの追加を行います
    *  @note     2回目のコマンドメニューで交換を選んだ時に使用します
    *            自分が AddFinalTradeRequest をコールすると
    *            自分の OnFinalTradeSuccess
    *                   OnFinalTradeComplete
    *                   OnTradeCancelReceived のどれかがコールされます
    */
  //------------------------------------------------------------------
  void AddFinalTradeRequest();

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換リクエストのキャンセルを行います
    *  @note     最後の交換リクエストを投げた後の待機画面で、
    *            Bボタンを押した時に使用します
    *            自分が CancelFinalTradeRequest をコールすると
    *            自分の OnFinalTradeCancel
    *                   OnFinalTradeSuccess
    *                   OnTradeCancelReceived のどれかがコールされます
    */
  //------------------------------------------------------------------
  void CancelFinalTradeRequest();



  //------------------------------------------------------------------
  /**
    *  @brief    交換キャンセルを送信するリクエストの追加を行います
    *  @note     2回目のコマンドメニューでやめるを選んだ時に使用します
    *            自分が AddSendTradeCancelRequest をコールすると
    *            相手の OnTradeCancelReceived がコールされます
    */
  //------------------------------------------------------------------
  void AddSendTradeCancelRequest();


  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が成立したか判定します
    *  @retval   true 最後の交換が成立した
    *  @retval   false 最後の交換が成立していない
    *  @note     P2PTradeResponseListener の OnFinalTradeSuccess のフラグチェック版の関数です
    */
  //------------------------------------------------------------------
  bool IsFinalTradeSuccess();

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が完了したか判定します
    *  @retval   true 最後の交換が完了した
    *  @retval   false 最後の交換が完了していない
    *  @note     P2PTradeResponseListener の OnFinalTradeComplete のフラグチェック版の関数です
    */
  //------------------------------------------------------------------
  bool IsFinalTradeComplete();



  //------------------------------------------------------------------
  /**
    *  @brief    交換終了を送信するリクエストの追加を行います
    *  @note     自分が AddTradeExitRequest をコールすると
    *            相手の OnTradeExitReceived がコールされ
    *            自分の OnTradeExitSuccess がコールされます
    */
  //------------------------------------------------------------------
  void AddTradeExitRequest();


  //------------------------------------------------------------------
  /**
    *  @brief    同期セーブを開始のリクエストを投げます
    *  @note     P2PTradeEvent で呼び出す
    */
  //------------------------------------------------------------------
  void StartSyncSaveRequest();

  void AddFinalSyncRequest();
  void AddFinalSyncCancelRequest();

  void ResetFlag();

public:

  virtual void OnRecvDataCommand( void* pRecvData, u8 commandID );

  void DebugPrint();

  bool& GetCallZukanRegister() { return m_bCallZukanRegister; }
  app::event::ZukanRegisterEventParam& GetZukanRegisterEventParam() { return m_ZukanRegisterEventParam; }
  bool& GetCallEvolution() { return m_bCallEvolution; }
  NetEvent::Evolution::EvolutionEventParam& GetEvolutionEventParam() { return m_EvolutionEventParam; }
  pml::pokepara::PokemonParam& GetRecvPokemonParam();
  u8* GetSendPokemonBuffer();

private:

  enum CommandID
  {
    // --------------------------------------------
    COMMAND_ID_MESSAGE_INFO,

    // --------------------------------------------
    COMMAND_ID_POKEMON_PARAM,

  };

  gfl2::heap::HeapBase*                             m_pHeap;
  NetAppLib::P2PTrade::P2PTradeResponseListener*    m_pP2PTradeResponseListener;
  NetAppLib::P2PTrade::P2PTradeNet                  m_MessageNet;
  NetAppLib::P2PTrade::P2PTradeNet                  m_PokemonParamNet;

  NetAppLib::P2PTrade::MessageInfo                  m_LastRecvMessageInfo;
  pml::pokepara::PokemonParam                       m_LastRecvPokemonParam;

  NetAppLib::P2PTrade::P2PTradeSequenceBase*        m_pTradeSequence;
  u32                                               m_InitializeSequence;

  bool                                              m_bCallZukanRegister;
  app::event::ZukanRegisterEventParam               m_ZukanRegisterEventParam;
  bool                                              m_bCallEvolution;
  NetEvent::Evolution::EvolutionEventParam          m_EvolutionEventParam;

};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADEREQUESTCLIENT_H_INCLUDE
