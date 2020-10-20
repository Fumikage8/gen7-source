// ============================================================================
/*
 * @file P2PTradeResponseListener.h
 * @brief P2P交換の通信レスポンスリスナーです。
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADERESPONSELISTENER_H_INCLUDE )
#define P2PTRADERESPONSELISTENER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "pml/include/pml_PokePara.h"
#include "NetStatic/NetAppLib/include/P2PTrade/P2PTradeDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


class P2PTradeResponseListener
{

public:

  P2PTradeResponseListener(){}
  virtual~P2PTradeResponseListener(){}

  //------------------------------------------------------------------
  /**
    *  @brief    メッセージIDを受信した時に呼ばれるハンドラです
    *  @param    eMessageID 受信したメッセージID
    *  @param    monsno 欲しいポケモンの種族番号
    *  @note     関連関数 AddSendMessageIDRequest
    */
  //------------------------------------------------------------------
  virtual void OnMessageIDReceived( NetAppLib::P2PTrade::MessageID eMessageID, MonsNo monsno, void* pPokemonParamBuffer ) = 0;
  
  //------------------------------------------------------------------
  /**
    *  @brief    ポケモンのパラメータを受信した時に呼ばれるハンドラです
    *  @param    pokemonParam 受信したポケモンのパラメータ
    *  @note     関連関数 AddSendPokemonParamRequest
    */
  //------------------------------------------------------------------
  virtual void OnPokemonParamReceived( const pml::pokepara::PokemonParam& pokemonParam ) = 0;



  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換が成立した時に呼ばれるハンドラです
    *  @param    pokemonParam 交換相手のポケモンのパラメータ
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeSuccess( const pml::pokepara::PokemonParam& pokemonParam ) = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    交換相手のポケモンが不正だった時に呼ばれるハンドラです
    *  @param    bMyPokemonInvalid [ true : 自分のポケモンが不正 / false : 相手のポケモンが不正 ]
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradePokemonInvalid( bool bMyPokemonInvalid ) = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    不正チェックサーバーへ接続出来なかった時などに呼ばれるハンドラです
    *  @param    bMyValidationError [ true : 自分の通信エラー / false : 相手の通信エラー ]
    *  @note     関連関数 AddFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeValidationError( bool bMyValidationError ) = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    最初の交換リクエストがキャンセルされた時に呼ばれるハンドラです
    *  @note     関連関数 CancelFirstTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFirstTradeCancel() = 0;



  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が成立した時に呼ばれるハンドラです
    *  @note     関連関数 AddFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeSuccess() = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換が完了した時に呼ばれるハンドラです
    *  @note     関連関数 AddFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeComplete() = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    最後の交換リクエストがキャンセルされた時に呼ばれるハンドラです
    *  @note     関連関数 CancelFinalTradeRequest
    */
  //------------------------------------------------------------------
  virtual void OnFinalTradeCancel() = 0;



  //------------------------------------------------------------------
  /**
    *  @brief    交換キャンセルを受信した時に呼ばれるハンドラです
    *  @note     関連関数 AddSendTradeCancelRequest
    */
  //------------------------------------------------------------------
  virtual void OnTradeCancelReceived() = 0;



  //------------------------------------------------------------------
  /**
    *  @brief    交換終了を受信した時に呼ばれるハンドラです
    *  @note     関連関数 AddTradeExitRequest
    */
  //------------------------------------------------------------------
  virtual void OnTradeExitReceived() = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    交換終了が成立した時に呼ばれるハンドラです
    *  @note     関連関数 AddTradeExitRequest
    */
  //------------------------------------------------------------------
  virtual void OnTradeExitSuccess() = 0;


  virtual void OnFinalSyncComplete(){}
  virtual void OnFinalSyncCancelComplete(){}


};



GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADERESPONSELISTENER_H_INCLUDE
