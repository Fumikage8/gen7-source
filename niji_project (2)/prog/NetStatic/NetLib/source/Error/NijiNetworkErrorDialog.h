//==============================================================================
/**
 * @file	NijiNetworkErrorDialog.h
 * @brief	通信エラーのエラー表示（アプレット）管理クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:57:24
 */
// =============================================================================
#ifndef __NIJI_NETWORK_ERROR_DIALOG_H__
#define __NIJI_NETWORK_ERROR_DIALOG_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_Str.h>                 // MsgData  

#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief	通信エラーのエラー表示（アプレット）管理クラス
 */
//------------------------------------------------------------------
class NijiNetworkErrorDialog
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkErrorDialog);

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
private:
  static const int MAX_REQUEST_NUM = 3;   ///< リクエスト登録数最大3件まで（エラーレベルより）
  static const int ERROR_TEXT_SIZE_MAX = 512; ///< エラーメッセージ最大サイズ


  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  NijiNetworkErrorDialog();
  // デストラクタ
  virtual ~NijiNetworkErrorDialog();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   表示リクエスト
   * @param   エラー情報データ
   */
  //------------------------------------------------------------------
  void RequestShow( NijiNetworkError::ERROR_INFO_DATA error );

  /// 更新
  void Update();

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:
  /// 初期化
  void Initialize();
  /// 破棄
  void Terminate();
  // エラーアプレット表示
  void DispErrorApplet( const NijiNetworkError::ERROR_INFO_DATA& error );
  void DispErrorAppletMessage( const int messageId );
  void DispErrorAppletErrorCode( const u32 errorCode );

private:
  gfl2::util::List<NijiNetworkError::ERROR_INFO_DATA> m_requestError; ///< リクエストキュー
  gfl2::str::MsgData* m_msgData;  ///< エラーメッセージ用MsgData
};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_DIALOG_H__
