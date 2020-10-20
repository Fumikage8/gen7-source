// ============================================================================
/*
 * @file gflnet2_CTR_CecCoreMessageBox.h
 * @brief すれ違い通信ボックスに関する操作を行うクラスです。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECCOREMESSAGEBOX_H_INCLUDE )
#define GFLNET2_CTR_CECCOREMESSAGEBOX_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecCoreMessageBox.h>
#include <gflnet2/source/cec/ctr/gflnet2_CTR_CecControlObject.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_ICecControlObjectRegister;


class CTR_CecCoreMessageBox : public ICecCoreMessageBox, public CTR_CecControlObject
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecCoreMessageBox );

public:
  CTR_CecCoreMessageBox( CTR_ICecControlObjectRegister* pCecControlObjectRegister );
  virtual~CTR_CecCoreMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されているか調べます。
   * @retval true 作成されている
   * @retval false 作成されていない
   */
  // --------------------------------------------------------------------------
  virtual bool IsCreateMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されているか調べます。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void IsCreateMessageBoxAsync( ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されている時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnIsCreateMessageBoxSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスが作成されていない時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnIsCreateMessageBoxFailure();

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
  virtual bool CreateMessageBox( const CecMessageBoxCreateCoreParam* pCreateCoreParam );

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
  virtual void CreateMessageBoxAsync( const CecMessageBoxCreateCoreParam* pCreateCoreParam, ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの作成に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateMessageBoxSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの作成に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnCreateMessageBoxFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にあるメッセージボックスを削除します。
   *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
   * @retval true 削除に成功
   * @retval false 削除に失敗
   */
  // --------------------------------------------------------------------------
  virtual bool DeleteMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にあるメッセージボックスを削除します。
   *         メッセージボックスを削除するとすれ違い通信は行われなくなります。
   *         非同期で行われるので結果はイベントリスナーに通知されます。
   * @param  pEventListener 結果が通知されるイベントリスナーを指定します
   */
  // --------------------------------------------------------------------------
  virtual void DeleteMessageBoxAsync( ICecMessageBoxEventListener* pEventListener );

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの削除に成功した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteMessageBoxSuccess();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体にメッセージボックスの削除に失敗した時にコールされるイベントハンドラです。
   */
  // --------------------------------------------------------------------------
  virtual void OnDeleteMessageBoxFailure();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスを作成する為のパラメータを取得します。
   * @return メッセージボックスを作成する為のパラメータ
   */
  // --------------------------------------------------------------------------
  virtual const CecMessageBoxCreateCoreParam* GetCecMessageBoxCreateCoreParam() const;

  // --------------------------------------------------------------------------
  /**
   * @brief  非同期で実行されたコマンドが終了した時にコールされるイベントハンドラです。
   * @param  eResultCode 実行結果です
   *         この実行結果を参照してアプリ側へ成功、失敗の通知を行います。
   */
  // --------------------------------------------------------------------------
  virtual void OnCommandResult( ResultCode eResultCode );

private:
  CTR_ICecControlObjectRegister*        m_pCecControlObjectRegister;
  ICecMessageBoxEventListener*          m_pCecMessageBoxEventListener;
  CecMessageBoxCreateCoreParam          m_CecMessageBoxCreateCoreParam;
  bool                                  m_bIsCreateMessageBox;
  bool                                  m_bCreateMessageBoxResult;
  bool                                  m_bDeleteMessageBoxResult;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECCOREMESSAGEBOX_H_INCLUDE
#endif
