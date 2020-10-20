// ============================================================================
/*
 * @file P2PTradeTestProc.h
 * @brief P2PTradeRequestClientクラスのテストです。
 * @date 2015.09.25
 * @author Akira endo
 */
// ============================================================================
#if PM_DEBUG

#if !defined( P2PTRADETESTPROC_H_INCLUDED )
#define P2PTRADETESTPROC_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <ro/include/gfl2_RoManager.h>
#include <GameSys/include/GameProc.h>
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeRequestClient.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeResponseListener.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(P2PTradeTest)


class P2PTradeTestProc :
  public GameSys::GameProc,
  public NetAppLib::P2PTrade::P2PTradeResponseListener
{
	GFL_FORBID_COPY_AND_ASSIGN( P2PTradeTestProc );

public:
  P2PTradeTestProc();
  virtual~P2PTradeTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
    *  @brief    メッセージIDを受信した時に呼ばれるハンドラです
    *  @param    eMessageID 受信したメッセージID
    *  @param    monsno 欲しいポケモンの種族番号
    *  @note     関連関数 AddSendMessageIDRequest
    */
  //------------------------------------------------------------------
  virtual void OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer );

  //------------------------------------------------------------------
  /**
    *  @brief    ポケモンのパラメータを受信した時に呼ばれるハンドラです
    *  @param    pokemonParam 受信したポケモンのパラメータ
    *  @note     関連関数 AddSendPokemonParamRequest
    */
  //------------------------------------------------------------------
  virtual void OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam );



  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換が成立した時に呼ばれるハンドラです
    *  @param    pokemonParam 交換相手のポケモンのパラメータ
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam );

  //------------------------------------------------------------------
  /**
    *  @brief    交換相手のポケモンが不正だった時に呼ばれるハンドラです
    *  @param    bMyPokemonInvalid [ true : 自分のポケモンが不正 / false : 相手のポケモンが不正 ]
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradePokemonInvalid( bool bMyPokemonInvalid );

  //------------------------------------------------------------------
  /**
    *  @brief    不正チェックサーバーへ接続出来なかった時などに呼ばれるハンドラです
    *  @param    bMyValidationError [ true : 自分の通信エラー / false : 相手の通信エラー ]
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeValidationError( bool bMyValidationError );

  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換リクエストがキャンセルされた時に呼ばれるハンドラです
    *  @note     関連関数 CancelFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeCancel();



  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が成立した時に呼ばれるハンドラです
    *  @note     関連関数 AddFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeSuccess();

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が完了した時に呼ばれるハンドラです
    *  @note     関連関数 AddFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeComplete();

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換リクエストがキャンセルされた時に呼ばれるハンドラです
    *  @note     関連関数 CancelFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeCancel();



  //------------------------------------------------------------------
  /**
    *  @brief    交換キャンセルを受信した時に呼ばれるハンドラです
    *  @note     関連関数 AddSendTradeCancelRequest
    */
  //------------------------------------------------------------------
  virtual void OnTradeCancelReceived();

  virtual void OnTradeExitReceived();
  virtual void OnTradeExitSuccess();

private:

  enum State
  {
    STATE_SELECT_POKEMON,
    STATE_WAIT_FIRST_TRADE_RESULT,
    STATE_TRADE_CONFIRM,
    STATE_WAIT_FINAL_TRADE_RESULT,
    STATE_TRADE_COMPLETE

  };

  gfl2::ro::Module*                             m_pRoModule;
  NetAppLib::P2PTrade::P2PTradeRequestClient*      m_pP2PTradeRequestClient;
  State                                         m_eState;
  pml::pokepara::PokemonParam*                  m_pSendPokemon;
  pml::pokepara::PokemonParam*                  m_pRecvPokemon;


};


GFL_NAMESPACE_END(P2PTradeTest)
GFL_NAMESPACE_END(Test)

#endif // P2PTRADETESTPROC_H_INCLUDED

#endif // PM_DEBUG
