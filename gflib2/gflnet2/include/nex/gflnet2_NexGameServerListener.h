#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_NexGameServerListener.h
 *
 * @brief  ゲームサーバ「ログイン・ログアウト」イベント通知用ハンドラ
 *
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_NEX_GAME_SERVER_LISTENER_H__
#define __GFLNET_NEX_GAME_SERVER_LISTENER_H__
#pragma once

#include <nn.h>
#include <nex.h>



namespace gflnet2{
namespace nex{

//------------------------------------------------------------------
/*!
 * @brief  ゲームサーバ「ログイン・ログアウト」イベント通知用ハンドラ
 *
 * @note ゲームサーバ関連の通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class NexGameServerListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(NexGameServerListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのログイン成功通知
  *
  * @param[in,out] credential ログイン時に発行された認証情報
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLoginSuccess( nn::nex::Credentials* credential ){}

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのログイン失敗通知
  *
  * @param[in] loginFuncResult    NexManager::Login()の実行結果
  * @param[in] lastLoginErrorCode nn::nex::NgsFacade::GetLastLoginErrorCode()の値
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLoginFailed( const nn::nex::qResult& loginFuncResult, qInt32 lastLoginErrorCode ){}

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのログインキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLoginCanceled( void ){}

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバからのログアウト成功通知
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLogoutSuccess( void ){}

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバのログアウト失敗通知
  *
  * @param[in] logoutFuncResult nn::nex::NgsFacade::Logout()の実行結果
  * @param[in] errorCode        logoutFuncResultからエラーコードに変換した値
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLogoutFailed( const nn::nex::qResult& logoutFuncResult, qInt32 errorCode ){}

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバからのログアウトキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnGameServerLogoutCanceled( void ){}

  //------------------------------------------------------------------
  /**
  * @brief NEXユニークID発行の成功通知
  *
  * @param[in] uniqueId 発行されたNEXユニークID
  */
  //------------------------------------------------------------------
  virtual void OnAcquireNexUniqueIdSuccess( nn::nex::UniqueId uniqueId ){}

  //------------------------------------------------------------------
  /**
  * @brief NEXユニークID発行の失敗通知
  *
  * @param[in] result     NEXユニークID発行処理の実行結果
  * @param[in] errorCode  resultからエラーコードに変換した値
  */
  //------------------------------------------------------------------
  virtual void OnAcquireNexUniqueIdFailed( const nn::nex::qResult& result, qInt32 errorCode ){}

  //------------------------------------------------------------------
  /**
  * @brief NEXユニークID発行のキャンセル通知
  */
  //------------------------------------------------------------------
  virtual void OnAcquireNexUniqueIdCanceled( void ){}






  //======================================================================================
  // FOR UPPER FIX
  //======================================================================================

  //------------------------------------------------------------------
  /**
  * @brief ゲームサーバの終了処理成功通知
  */
  //------------------------------------------------------------------
  virtual void OnGameServerTerminated( void ){}

};


} // nex
} // gflnet



#endif // __GFLNET_NEX_GAME_SERVER_LISTENER_H__
#endif
