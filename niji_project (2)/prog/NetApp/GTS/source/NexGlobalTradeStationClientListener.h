//=============================================================================
/**
 * @file   NexGlobalTradeStationClientListener.h
 * @brief  NexGlobalTradeStationClient関連通知イベントハンドラ
 * @author h.suzuki
 */
//=============================================================================
#ifdef GF_PLATFORM_CTR

#if !defined( NEXGLOBALTRADESTATIONCLIENTLISTENER_H_INCLUDE )
#define NEXGLOBALTRADESTATIONCLIENTLISTENER_H_INCLUDE
#pragma once

#include <nex.h>
#include "NetApp/GTS/source/GTSTypes.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)

//------------------------------------------------------------------
/*!
 * @brief NexGlobalTradeStationClient通知イベントハンドラ
 *
 * @note ＧＴＳ通信の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexGlobalTradeStationClientListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexGlobalTradeStationClientListener); //コピーコンストラクタ＋代入禁止
public:
  //------------------------------------------------------------------
  /**
  * @brief UploadPokemonの成功通知
  */
  //------------------------------------------------------------------
  virtual void OnUploadPokemonSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief DownloadMyPokemonの成功通知
  *
  * @param[in] downloadResult ダウンロード結果
  */
  //------------------------------------------------------------------
  virtual void OnDownloadMyPokemonSuccess( const nn::nex::GlobalTradeStationDownloadMyPokemonResult& downloadResult ){};

  //------------------------------------------------------------------
  /**
  * @brief DownloadOtherPokemonの成功通知
  *
  * @param[in] downloadResult ダウンロード結果
  */
  //------------------------------------------------------------------
  virtual void OnDownloadOtherPokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& downloadResult ){};

  //------------------------------------------------------------------
  /**
  * @brief TradePokemonの成功通知
  *
  * @param[in] tradeResult 交換結果
  */
  //------------------------------------------------------------------
  virtual void OnTradePokemonSuccess( const nn::nex::GlobalTradeStationTradePokemonResult& tradeResult ){};

  //------------------------------------------------------------------
  /**
  * @brief SearchPokemonの成功通知
  *
  * @param[in] searchResult 検索結果
  */
  //------------------------------------------------------------------
  virtual void OnSearchPokemonSuccess( const nn::nex::GlobalTradeStationSearchPokemonResult& searchResult ){};

  //------------------------------------------------------------------
  /**
  * @brief DeletePokemonの成功通知
  */
  //------------------------------------------------------------------
  virtual void OnDeletePokemonSuccess( void ){};

  //------------------------------------------------------------------
  /**
  * @brief PrepareUploadPokemonの成功通知
  *
  * @param[in] prepareUploadKey 自分が預けようとするポケモンを特定するためのキー
  */
  //------------------------------------------------------------------
  virtual void OnPrepareUploadPokemonSuccess( const NetApp::GTS::GtsSerializeRecordKey& prepareUploadKey ){};

  //------------------------------------------------------------------
  /**
  * @brief PrepareTradePokemonの成功通知
  *
  * @param[in] preTradeResult  交換準備結果
  * @param[in] prepareTradeKey 自分が交換に出すポケモンを特定するキー
  */
  //------------------------------------------------------------------
  virtual void OnPrepareTradePokemonSuccess( const nn::nex::GlobalTradeStationDownloadPokemonResult& preTradeResult,
                                             const NetApp::GTS::GtsSerializeRecordKey& prepareTradeKey ){};

  //------------------------------------------------------------------
  /**
  * @brief NexGlobalTradeStationClient通信の失敗通知
  *
  * @param[in] result     実行結果
  * @param[in] errorCode  エラーアプレット用エラーコード
  */
  //------------------------------------------------------------------
  virtual void OnGlobalTradeStationClientError( const nn::nex::qResult& result, u32 errorCode ) = 0;

  //------------------------------------------------------------------
  /**
  * @brief NexGlobalTradeStationClient通信のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnGlobalTradeStationClientCanceled( void ){};
};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)



#endif // NEXGLOBALTRADESTATIONCLIENTLISTENER_H_INCLUDE
#endif // GF_PLATFORM_CTR
