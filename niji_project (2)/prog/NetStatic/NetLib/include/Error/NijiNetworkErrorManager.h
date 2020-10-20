//==============================================================================
/**
 * @file	NijiNetworkErrorManager.h
 * @brief	Niji通信エラー管理クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:44:47
 */
// =============================================================================

#ifndef __NIJI_NETWORK_ERROR_MANAGER_H__
#define __NIJI_NETWORK_ERROR_MANAGER_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <base/include/gfl2_Singleton.h>
#include <util/include/gfl2_List.h>

#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"

// 前方宣言
namespace NetLib{
  namespace Error{
    class NijiNetworkErrorListener;
    class NijiNetworkErrorDialog;
    class NijiNetworkForceDisconnectListener;
  }
}


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   Niji通信エラー管理クラス
 */
//------------------------------------------------------------------
class NijiNetworkErrorManager : public gfl2::base::Singleton<NijiNetworkErrorManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(NijiNetworkErrorManager);
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
public:
  // コンストラクタ
  NijiNetworkErrorManager( NijiNetworkErrorDialog* networkErrorDialog );
  // デストラクタ
  virtual ~NijiNetworkErrorManager();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   エラーセット
   * @param   エラー情報クラス
   */
  //------------------------------------------------------------------
  void SetNetworkError( NijiNetworkError& error );

  // 
  // 以下、自分でエラーハンドリングする場合のみ使用する。
  // 通常は通信Eventで呼ばれているので使う必要はない。
  //
  /* -------------------------------------------------------------------------*/
  /**
   * @brief 通信エラーが発生したかどうか
   *
   * @retval true：発生した
   * @retval false：発生してない
   */
  /* -------------------------------------------------------------------------*/
  bool IsError() const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief セッション切断が発生したかどうか(バトルスポット専用)
   *
   * @retval true：発生した
   * @retval false：発生してない
   */
  /* -------------------------------------------------------------------------*/
  bool IsSessionLeft() const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief Fatalエラーが発生したかどうか
   *
   * @retval true：発生した
   * @retval false：発生してない
   */
  /* -------------------------------------------------------------------------*/
  bool IsFatalError() const;

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 更新
   */
  /* -------------------------------------------------------------------------*/
  void Update();

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 共通の通信エラー対処
   */
  /* -------------------------------------------------------------------------*/
  void CommonNetworkErrorHandling();

  //------------------------------------------------------------------
  /**
   * @brief   エラー情報をクリア（＝通信エラー対処が完了した）
   * @note    呼ぶ箇所・タイミングを間違えないように。
   */
  //------------------------------------------------------------------
  void Clear();

  //------------------------------------------------------------------
  /**
   * @brief   イベントリスナー登録
   * @param   listener   通知を受け取るNijiNetworkErrorListener
   *
   * @note    通信エラーがプールされていると、登録した関数内で
   *          OnNijiNetworkErrorが呼び出される事があります。
   */
  //------------------------------------------------------------------
  void RegistListener( NijiNetworkErrorListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   イベントリスナー解除
   */
  //------------------------------------------------------------------
  void UnregistListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   強制切断リスナー登録
   * @param   listener   通知を受け取るNijiNetworkForceDisconnectListener
   */
  //------------------------------------------------------------------
  void RegistForceDisconnectListener( NijiNetworkForceDisconnectListener* listener );

  //------------------------------------------------------------------
  /**
   * @brief   強制切断リスナー解除
   */
  //------------------------------------------------------------------
  void UnregistForceDisconnectListener( void );

  //------------------------------------------------------------------
  /**
   * @brief   P2P切断期間を設定
   *           必ずP2pDisconnectSpanOffで解除すること
   */
  //------------------------------------------------------------------
  void P2pDisconnectSpanOn();
  void P2pDisconnectSpanOff();
  bool IsP2pDisconnectSpanOn();

  //------------------------------------------------------------------
  /**
   * @brief   セーブ中通信エラーをFatalErroにする期間を設定
   *          必ずSaveCriticalSpanOffで解除すること
   *
   * @caution この期間は全ての通信エラーがFATALになります。
   *            使用を間違えるとガイドライン違反になります。
   */
  //------------------------------------------------------------------
  void SaveCriticalSpanOn();
  void SaveCriticalSpanOff();

  //------------------------------------------------------------------
  /**
   * @brief   通信エラーを通知しない期間を設定
   *           必ずNotNotifySpanOffで解除すること
   *
   * @caution  期間中に通信エラーが発生していた場合を考慮して、
   *             IsErrorでエラー発生チェックをして下さい。
   */
  //------------------------------------------------------------------
  void NotNotifySpanOn();
  void NotNotifySpanOff();

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  //------------------------------------------------------------------
  /**
   * @brief   リスナーにエラー通知
   */
  //------------------------------------------------------------------
  void NotifyListner();

  //------------------------------------------------------------------
  /**
   * @brief   リスナーにエラー通知するかチェック
   * @retval  true:通知する
   * @retval  false:通知しない
   */
  //------------------------------------------------------------------
  bool IsNotifyListener() const;

  //------------------------------------------------------------------
  /**
   * @brief   エラーダイアログに表示リクエスト
   */
  //------------------------------------------------------------------
  void ErrorDialogRequest();

private:
  //-----------------------------------------------------------------------------
  //-------------------------------------
  /// 状態
  //=====================================
  typedef enum{
    STATE_INITIAL,      //! 初期状態 
    STATE_ERROR_SET,    //! エラーセットされた状態
    STATE_ERROR_NOTICE, //! エラー通知した状態
  } ENUM_STATE;
  //-----------------------------------------------------------------------------
  //! リスナー登録出来る数
  static const u32 LISTNER_MAX = 5;
  //-----------------------------------------------------------------------------
  ENUM_STATE                            m_state;                //! 状態
  NijiNetworkError::ERROR_INFO_DATA     m_lastErrorInfoData;    //! 最後に処理したエラー情報データ
  NijiNetworkErrorDialog*               m_networkErrorDialog;   //! エラーダイアログ管理クラス
  gfl2::util::List<NijiNetworkErrorListener*>     m_pListenerList;   //! モデル表示しているポイントのポインタリスト
  NijiNetworkForceDisconnectListener*   m_pForceDisconnectListener;  //! モデル表示しているポイントのポインタリスト
  bool                                  m_isP2pDisconnectSpan;  //! P2P切断期間フラグ
  bool                                  m_isSaveCriticalSpan;   //! セーブ中FatalError期間フラグ
  bool                                  m_isNotNotifySpan;      //! 通信エラー通知しない期間フラグ


  // デバッグ機能
#if PM_DEBUG
public:
  static bool m_debugRButtonErrorEnable; //! Rボタンで通信エラー発生有効フラグ
  static bool m_debugLButtonErrorEnable; //! Rボタンで通信エラー発生有効フラグ
#endif // PM_DEBUG

};

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)

#endif // __NIJI_NETWORK_ERROR_MANAGER_H__
