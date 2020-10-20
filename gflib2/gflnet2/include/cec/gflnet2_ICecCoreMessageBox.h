// ============================================================================
/*
 * @file gflnet2_ICecCoreMessageBox.h
 * @brief 3DS本体のすれ違い通信ボックスに関する操作を行うクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_ICECCOREMESSAGEBOX_H_INCLUDE )
#define GFLNET2_ICECCOREMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_CecMessageBoxCreateParam.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)

class ICecMessageBoxEventListener;

class ICecCoreMessageBox
{
public:

  virtual~ICecCoreMessageBox(){}

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されているか調べます。
   * @retval true 作成されている
   * @retval false 作成されていない
   */
  // --------------------------------------------------------------------------
  virtual bool IsCreateMessageBox() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されているか調べます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void IsCreateMessageBoxAsync( ICecMessageBoxEventListener* pEventListener ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスを作成します。
   *         メッセージボックスを作成し、送信ボックスにデータを書き込む事によって、
   *         自動的にすれ違い通信が行われます。
   * @param  pCreateCoreParam メッセージボックスを作成する時のパラメータです
   * @retval true 作成に成功
   * @retval false 作成に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool CreateMessageBox( const CecMessageBoxCreateCoreParam* pCreateCoreParam ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスを作成します。
   *         メッセージボックスを作成し、送信ボックスにデータを書き込む事によって、
   *         自動的にすれ違い通信が行われます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pCreateCoreParam メッセージボックスを作成する時のパラメータです
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void CreateMessageBoxAsync( const CecMessageBoxCreateCoreParam* pCreateCoreParam, ICecMessageBoxEventListener* pEventListener ) = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にあるメッセージボックスを削除します。
   *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteMessageBox() = 0;

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にあるメッセージボックスを削除します。
   *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteMessageBoxAsync( ICecMessageBoxEventListener* pEventListener ) = 0;
};


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_ICECCOREMESSAGEBOX_H_INCLUDE
