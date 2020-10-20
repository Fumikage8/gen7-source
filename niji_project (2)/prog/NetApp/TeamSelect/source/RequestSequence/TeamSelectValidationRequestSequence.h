// ============================================================================
/*
 * @file TeamSelectValidationRequestSequence.h
 * @brief ポケパーティのレギュレーションチェックと不正チェックを行うリクエストです。
 * @date 2015.06.23
 */
// ============================================================================
#if !defined( CHECKREGULATIONANDVALIDATIONREQUESTSEQUENCE_H_INCLUDE )
#define CHECKREGULATIONANDVALIDATIONREQUESTSEQUENCE_H_INCLUDE
#pragma once
#include <gflnet2/include/ServerClient/gflnet2_RequestSequenceBase.h>

#include <macro/include/gfl2_macros.h>
#include <NetApp/TeamSelect/source/RequestSequence/TeamSelectResponseListener.h>
#include <NetApp/TeamSelect/source/RequestSequence/TeamSelectRequestClient.h>

#if defined(GF_PLATFORM_CTR)
#include <NetStatic/NetLib/include/Validation/PokemonValidation.h>
#endif

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(TeamSelect)

//------------------------------------------------------------------
/**
* @brief  ポケパーティのレギュレーションチェックと不正チェックを行うリクエスト
*/
//------------------------------------------------------------------
class TeamSelectValidationRequestSequence :
  public gflnet2::ServerClient::RequestSequenceBase 
#if defined(GF_PLATFORM_CTR)
  ,public NetLib::Validation::PokemonValidationListener
#endif
{
  GFL_FORBID_COPY_AND_ASSIGN( TeamSelectValidationRequestSequence );
private:

  enum
  {
    DUMMY_WAIT = 30 * 2,//ダミーウェイト2秒
  };

  enum
  {
    SEQ_INIT = 0,
    SEQ_VALIDATION_WAIT,
    SEQ_VALIDATION_VALID,
    SEQ_VALIDATION_INVALID
  };

public:
  //------------------------------------------------------------------
  /**
  * @brief  コンストラクタ/デストラクタ
  */
  //------------------------------------------------------------------
  TeamSelectValidationRequestSequence();
  virtual~TeamSelectValidationRequestSequence();

public://RequestSequenceBaseのオーバーライド

  //------------------------------------------------------------------
  /**
  * @brief  更新処理。
  *
  * @note   	メインループ毎にポーリングされる
  *			      継承して実装する通信シーケンス本体
  *           クラスに用意されているセッター/ゲッターを使用して個別の
  *           通信シーケンスを実装してください
  *           キャンセル操作が行われた後はこのアップデートは呼ばれなくなり、
  *           UpdateCancelが呼ばれるようになります。
  */
  //------------------------------------------------------------------
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE Update( void );

  //------------------------------------------------------------------
  /**
  * @brief  キャンセル処理の更新
  *
  * @note   	
  *			      リクエストをキャンセルするとUpdateではなくこちらが呼ばれるようになります。
  *           キャンセル処理が完了したらREQUEST_STATE_FINISHを返すとリクエストは終了します。
  *           キャンセルが行われなかった場合、この関数は呼ばれません。
  */
  //------------------------------------------------------------------
  virtual gflnet2::ServerClient::REQUEST_SEQUENCE_STATE UpdateCancel( void );


  //------------------------------------------------------------------
  /**
  * @brief  リスナー登録解除
  */
  //------------------------------------------------------------------
  virtual void RemoveListener( void );

  //------------------------------------------------------------------
  /**
  * @brief  リスナー設定
  */
  //------------------------------------------------------------------
  void SetListener( TeamSelectResponseListener* pListener );

  //------------------------------------------------------------------
  /**
  * @brief  リクエストパラメータ設定
  */
  //------------------------------------------------------------------
  void SetRequestParam( TeamSelectRequestClient::CHECK_REQUEST_PARAM& pParam );

public:

#if defined(GF_PLATFORM_CTR)

  //------------------------------------------------------------------
  /**
   * @brief ポケモン不正チェック処理の完了イベントを検知
   *
   * @note  本イベント終了後、[checkResult]配列、および[certData]は解放される
   *
   * @param[in]  checkResult  個々のポケモンの検査結果が配列で格納される。
   * @param[in]  resultNum    検査結果配列の数
   * @param[in]  certData     署名データ
   * @param[in]  certDataSize 署名データサイズ。この値が0の場合は、不正なポケモンデータであることを指す。
   */
  //------------------------------------------------------------------
   virtual void OnPokemonValidationSuccess( const nn::nex::qInt32* checkResult, const u32 resultNum,
                                            const u8* certData, const u32 certDataSize );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @note  HTTP通信のステータスコードが200番台外の場合のエラー通知
   *
   * @param[in] httpStatus サーバから返却されたHTTPステータスコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const s32 httpStatus );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のエラーイベントを検知
   *
   * @param[in] result    エラー内容
   * @param[in] errorCode ネットワークエラーコード
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationError( const nn::nex::qResult &result, u32 errorCode );

   //------------------------------------------------------------------
   /**
   * @brief ポケモン不正チェック処理のキャンセルイベントを検知
   */
   //------------------------------------------------------------------
   virtual void OnPokemonValidationCanceled( void );

#endif

private:

  TeamSelectResponseListener*           m_pListener;
  TeamSelectRequestClient::CHECK_REQUEST_PARAM m_requestParam;
  u32 m_dummyWaitCount;
#if defined(GF_PLATFORM_CTR)
  NetLib::Validation::PokemonValidation* m_pValidator;
#endif

};


GFL_NAMESPACE_END(TeamSelect)
GFL_NAMESPACE_END(NetApp)

#endif // CHECKREGULATIONANDVALIDATIONREQUESTSEQUENCE_H_INCLUDE
