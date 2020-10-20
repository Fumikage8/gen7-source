//==============================================================================
/**
 * @file	NetEventBase.h
 * @brief	通信Eventの基底クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/18 17:23:34
 */
// =============================================================================
#ifndef __NET_EVNET_BASE_H__
#define __NET_EVNET_BASE_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "GameSys/include/GameEvent.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorListener.h"

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

GFL_NAMESPACE_BEGIN(NetEvent)

/**
 * @brief 通信イベント
 *
 * ◆エラーハンドリングについて
 *
 *   Procが終了した時にNijiNetworkErrorManagerのIsError関数で通信エラーが発生して終了したのかチェックする
 *   あとは、通信エラーの対処をするEventなのかどうかで処理が変わります。
 *
 *   ●通信エラーを処理しないEventの場合（=通信エラーが発生したらアプリ終了するだけのEvent）
 *  
 *     呼び出し元で通信エラー対処をするので、何もしなくてよい
 *
 *   ●通信エラーを処理するEventの場合
 *
 *     IsErrorで通信エラー発生していた場合は、速やかにEventを終了させる
 *
 */
class NetEventBase : public GameSys::GameEvent, public NetLib::Error::NijiNetworkErrorListener
{
  GFL_FORBID_COPY_AND_ASSIGN(NetEventBase); //コピーコンストラクタ＋代入禁止
public:
  /* -------------------------------------------------------------------------*/
  /**
  * @brief コンストラクタ
  *
  * @param heap    workをとるためのメモリクラス
  */
  /* -------------------------------------------------------------------------*/
  NetEventBase( gfl2::heap::HeapBase* heap );
  // デストラクタ
  virtual ~NetEventBase();

public:
  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
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
   * @brief Fatalエラーが発生したかどうか
   *
   * @retval true：発生した
   * @retval false：発生してない
   */
  /* -------------------------------------------------------------------------*/
  bool IsFatalError() const;


  /* -------------------------------------------------------------------------*/
  /**
   * @brief 通信エラーが発生しても無視する。（ =アプリ終了させない）
   *
   * @li    使用は許可制です。ichiraku 2015/12/28
   * 
   *        許可済みEvent 
   *        ・BoxEvent　（PCからの起動は、通信エラーで終わらない）
   */
  /* -------------------------------------------------------------------------*/
  void SetErrorIgnore();

public:
  // NijiNetworkErrorListener
  /* -------------------------------------------------------------------------*/
  /**
  * @brief 通信エラー通知
  *
  * @param error_level    エラーレベル
  * @param error_type     エラー種類
  */
  /* -------------------------------------------------------------------------*/
  virtual void OnNijiNetworkError();

  //--------------------------------------------------------------
  /**
    * @brief   通常のGameEventか判定
    * @retval  true 通常のGameEvent
    * @retval  false NetEvent
    */
  //--------------------------------------------------------------
  virtual bool IsNormalEvent(void){ return false; }

private:

  bool  m_isErrorIgnore;      //! 通信エラー無視フラグ

};

GFL_NAMESPACE_END(NetEvent)

#endif // __NET_EVNET_BASE_H__
