// ============================================================================
/*
 * @file gflnet2_CTR_CecMessageBoxAccessor.h
 * @brief メッセージボックスにアクセスする為のクラスです。
 *        nn::cec::MessageBoxのラッパークラス的なクラスとなっています。
 *        実際に必要な機能だけ実装しています。
 * @date 2015.02.16
 */
// ============================================================================
#ifdef  GF_PLATFORM_CTR
#if !defined( GFLNET2_CTR_CECMESSAGEBOXACCESSOR_H_INCLUDE )
#define GFLNET2_CTR_CECMESSAGEBOXACCESSOR_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <nn/cec.h>
#include <gflnet2/include/cec/gflnet2_CecMessageBoxCreateParam.h>
#include <gflnet2/include/cec/gflnet2_CecResultCode.h>
#include <gflnet2/include/cec/gflnet2_CecTypes.h>

GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)
GFL_NAMESPACE_BEGIN(ctr)


class CTR_CecAllocator;
class CTR_CecControlObject;


class CTR_CecMessageBoxAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( CTR_CecMessageBoxAccessor );

public:
  CTR_CecMessageBoxAccessor( CTR_CecAllocator* pCecAllocator );
  virtual~CTR_CecMessageBoxAccessor();

  // --------------------------------------------------------------------------
  /**
   * @brief  初期化処理を行います。
   * @param  pHeap メッセージボックスを作成する為に必要なヒープです。
   * @param  pInitParam メッセージボックスにアクセスする為に必要なパラメータです。
   */
  // --------------------------------------------------------------------------
  void Initialize( gfl2::heap::HeapBase* pHeap, const gflnet2::cec::CecSystemInitParam* pInitParam );

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスへアクセスします。
   * @param  pCecControlObject 実行オブジェクトです。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode Execute( CTR_CecControlObject* pCecControlObject );

private:

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスが作成されているか調べます。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode isCreateMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスの作成を行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode createMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージボックスの削除を行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode deleteMessageBox();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージの全削除を行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode deleteAllMessage();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージの書き込みを行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode writeMessage();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージの読み込みを行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode readMessage();

  // --------------------------------------------------------------------------
  /**
   * @brief  メッセージ数の取得を行います。
   * @return 実行結果を返します。
   */
  // --------------------------------------------------------------------------
  ResultCode getMessageCount();

  // --------------------------------------------------------------------------
  /**
   * @brief  リザルトコードの変換を行います。
   * @param  result nnのリザルトコード
   * @return ライブラリで定義してあるリザルトコードを返します。
   */
  // --------------------------------------------------------------------------
  ResultCode convertToResultCode( nn::Result result );

  // --------------------------------------------------------------------------
  /**
   * @brief  ライブラリ内で使用しているボックスタイプをnnのボックスタイプに変換します。
   * @param  eMessageBoxType ライブラリ内で使用しているボックスタイプ
   * @return nnのボックスタイプ
   */
  // --------------------------------------------------------------------------
  nn::cec::CTR::CecBoxType convertToCecBoxType( const MessageBoxType eMessageBoxType ) const;

  // --------------------------------------------------------------------------
  /**
   * @brief  送信メッセージのセットアップを行います。
   *         送信するデータ、送信するアイコン、送信する情報テキストを設定します。
   * @param  message セットアップを行うメッセージ
   * @retval true 成功
   * @retval false 失敗
   */
  // --------------------------------------------------------------------------
  bool setupSendMessage( nn::cec::Message& message ) const;

private:
  CTR_CecAllocator*           m_pCecAllocator;
  nn::cec::MessageBox*        m_pMessageBox;
  CTR_CecControlObject*       m_pCecControlObject;
  u32                         m_cecTitleID;
  u32                         m_privateID;
};


GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CTR_CECMESSAGEBOXACCESSOR_H_INCLUDE
#endif