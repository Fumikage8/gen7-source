#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_FriendLoginListener.h
 * @brief  フレンドサーバへのログイン完了通知イベントハンドラ用クラス
 * @author h.suzuki
 */
//=============================================================================

#ifndef __GFLNET_FREINDS_LOGIN_LISTENER_H__
#define __GFLNET_FREINDS_LOGIN_LISTENER_H__
#pragma once

#include <nn.h>



namespace gflnet2{
namespace friends{

//------------------------------------------------------------------
/**
 * @brief フレンドサーバへのログイン完了通知用クラス
 *
 * @note フレンドサーバへのログイン完了通知を受け取る場合は、
 *       本クラスを継承し仮想関数を実装してください。
 */
//------------------------------------------------------------------
class FriendLoginListener
{
//  GFL_FORBID_COPY_AND_ASSIGN(FriendLoginListener); //コピーコンストラクタ＋代入禁止
public:

  //------------------------------------------------------------------
  /**
   * @brief ログイン完了イベント
   *
   * @note  イベントはスレッド内からコールされるため
   * @note  リスナ関数側でスレッドアンセーフを考慮した同期対応を行うこと。
   *
   * @param[in] result 実行結果
   */
  //------------------------------------------------------------------
   virtual void OnFriendServerLoginFinish( const nn::Result &result ) = 0;
};


} // friends
} // gflnet



#endif // __GFLNET_FREINDS_LOGIN_LISTENER_H__
#endif
