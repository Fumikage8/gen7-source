// ============================================================================
/*
 * @file gflnet2_CTR_CecSystem.h
 * @brief ctr版すれ違い通信行う為のクラスです。
 * @date 2015.02.13
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECSYSTEM_H_INCLUDE )
#define GFLNET2_CTR_CECSYSTEM_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include/cec/gflnet2_ICecSystem.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_CecAllocator;
class CTR_CecController;
class CTR_CecCoreMessageBox;
class CTR_CecSendMessageBox;
class CTR_CecRecvMessageBox;


class CTR_CecSystem : public ICecSystem
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecSystem );

public:
  CTR_CecSystem();
  virtual~CTR_CecSystem();

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの初期化を行います。
   * @param  pHeap すれ違い通信ライブラリ内で使用するヒープです。
   * @param  pInitParam すれ違い通信ライブラリの初期化パラメータです。
   */
  // --------------------------------------------------------------------------
  virtual void Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam );

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの終了処理を行います。
   */
  // --------------------------------------------------------------------------
  virtual void Terminate();

  // --------------------------------------------------------------------------
  /**
   * @brief  すれ違い通信ライブラリの更新処理を行います。
   *         主に非同期で実行された処理のスレッドを監視してたりします。
   */
  // --------------------------------------------------------------------------
  virtual void Update();

  // --------------------------------------------------------------------------
  /**
   * @brief  3DS本体のメッセージボックスを作成したり削除したりするクラスを取得します。
   * @return 3DS本体のメッセージボックスを作成したり削除したりするクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecCoreMessageBox* GetCecCoreMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  送信ボックスに対して操作を行うクラスを取得します。
   * @return 送信ボックスに対して操作を行うクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecSendMessageBox* GetCecSendMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  受信ボックスに対して操作を行うクラスを取得します。
   * @return 受信ボックスに対して操作を行うクラス
   */
  // --------------------------------------------------------------------------
  virtual ICecRecvMessageBox* GetCecRecvMessageBox();

private:
  CTR_CecAllocator*         m_pCecAllocator;
  CTR_CecController*        m_pCecController;
  CTR_CecCoreMessageBox*    m_pCecCoreMessageBox;
  CTR_CecSendMessageBox*    m_pCecSendMessageBox;
  CTR_CecRecvMessageBox*    m_pCecRecvMessageBox;

};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECSYSTEM_H_INCLUDE
#endif
