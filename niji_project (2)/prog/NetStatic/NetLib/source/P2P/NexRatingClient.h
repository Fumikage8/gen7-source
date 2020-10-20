//=============================================================================
/**
 * @file   NexRatingClient.h
 *
 * @brief  nn::nex::Ratingのラッパークラス。
 *
 * @author h.suzuki
 */
//=============================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef __NEX_RATING_CLIENT_H__
#define __NEX_RATING_CLIENT_H__
#pragma once

#include <nex.h>
//#include <gflnet2/include/gflnet2.h>
#include <RendezVous/Services/JugemPackages.h> // NexのRating系
#include <macro/include/gfl2_Macros.h>
#include <gflnet2/include\friends/gflnet2_FriendManager.h>
#include "NexRatingClientListener.h" // 非公開ヘッダ
#include <battle/include/battle_SetupParam_Record.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(P2P)

//------------------------------------------------------------------
/**
*  @brief レーティング管理クラス（RatingClientアクセス）用クラス。
*  @brief  gflnet2::nex::NexMatchMakingClientと組み合わせて使用する。
*
*  @par   レーティングセッション確立の手順は以下のとおり。
*           - [オーナー] NexMatchMakingClient::AutoMatchmake() → NexRatingClient::CreateRatingSession() → NexMatchMakingClient::OpenSession() → NetZセッション作成
*           - [参加者  ] NexMatchMakingClient::AutoMatchmake() → NexRatingClient::JoinRatingSession()
*
*  @attention 事前にゲームサーバへログインしておくこと
*  @attention 各非同期通信は排他制御となる。
**/
//------------------------------------------------------------------
class NexRatingClient
{
public:
    /**
    * @brief レポート情報に指定できるスロット数
    */
    typedef enum
    {
      E_SLOT1,      //!< 対戦情報格納スロット1
      E_SLOT2,      //!< 対戦情報格納スロット2
      E_SLOT_MAX    //!< 対戦情報格納スロット最大数
    }REPORT_SLOT_ID;

    /**
     * @brief 対戦結果レート
     */
    typedef enum
    {
      E_RATE_WIN,    //!< 勝利
      E_RATE_DRAW,   //!< 引き分け
      E_RATE_LOSE    //!< 敗北
    }REPORT_RATE;


    static const u32 ERROR_CODE_BAN_USER = 20102;  //!< BANされたユーザーアクセス時のネットワークエラー


public:
    NexRatingClient( void );
    virtual ~NexRatingClient();

public:
  //------------------------------------------------------------------
  /**
  * @brief  RatingClientの初期化
  *
  * @param[in]  credentials     ゲームサーバーログイン情報
  *
  * @return 初期化に成功したならtrueを返却
  */
  //------------------------------------------------------------------
  bool Initialize( nn::nex::Credentials* credentials );

  //------------------------------------------------------------------
  /**
  * @brief  RatingClientの終了
  *
  * @caution 必ず事前にIsConnecting()がfalseであることを確認してください
  */
  //------------------------------------------------------------------
  void Finalize( void );

public:
  //------------------------------------------------------------------
  /**
  * @brief レーティング用リスナの登録
  *
  * @param[in] listener 登録対象のレーティング用リスナ。
  *
  * @return 登録に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool RegistRatingClientListener( NexRatingClientListener* listener );

  //------------------------------------------------------------------
  /**
  * @brief レーティング用リスナの解除
  *
  * @param[in] listener 削除対象のレーティング用リスナ。
  */
  //------------------------------------------------------------------
  void UnregistRatingClientListener( NexRatingClientListener* listener );

public:
  //------------------------------------------------------------------
  /**
  * @brief レーティングセッションを生成する（非同期通信）
  *
  * @attention マッチメイクセッションのオーナーのみ実行可能。
  * @attention nn::nex::MatchmakeExtensionClient::OpenParticipation()コール前に実行すること。
  *
  * @note  通信終了時は「NexRatingClientListener::OnCreateRatingSessionFinish()」がコールされる。
  * @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool CreateRatingSession( void );

  //------------------------------------------------------------------
  /**
  * @brief レーティングセッションに参加する（非同期通信）
  *
  * @attention 事前にマッチメイクセッションに参加している必要がある。
  * @attention マッチメイクセッションの非オーナーのみ実行可能
  *
  * @param[in]  sessionKey 参加セッションのキー。
  *                    CreateRatingSesssion()によって発行されたセッションキーを使用する。
  *
  * @note  通信終了時は「NexRatingClientListener::OnJoinRatingSessionSuccess()」がコールされる。
  * @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool JoinRatingSession( const nn::nex::qVector<qByte>& sessionKey );

  //------------------------------------------------------------------
  /**
  * @brief 統計データをレーティングサーバに送信する（非同期通信）
  *
  * @attention 本関数を実行する前に必ずSetReportStatusInfo()にて2名分の結果情報をセットすること！
  * @attention 参加者2名が本関数にてレポートを完了させるまではセッションを終了させてはならない。
  *
  * @note  通信終了時は「NexRatingClientListener::OnRatingReportSuccess()」がコールされる。
  * @note  エラー時は「NexRatingClientListener::OnNexRatingClientError()」がコールされる。
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool ReportRatingStats( void );

  //------------------------------------------------------------------
  /**
  * @brief 対戦結果データをセットする
  *
  * @attention ReportRatingStats()を実行する前に必ず本関数にて2名分の結果情報をセットすること！
  *
  * @param[in] slot            設定するスロットID
  * @param[in] pid             プリンシバルID
  * @param[in] nexUniqueId     NEXユニークID
  * @param[in] categoryId      カテゴリId
  * @param[in] dissconnectFlag 通信切断フラグ
  * @param[in] rate            対戦結果レート
  * @param[in] battleLog       バトルログ情報
  * @param[in] battleLogSize   バトルログ情報サイズ
  *
  * @return 実行に成功したならtrueを返却する
  */
  //------------------------------------------------------------------
  bool SetReportStatusInfo( REPORT_SLOT_ID slot,
                            gflnet2::friends::PrincipalId pid,
                            u64 nexUniqueId,
                            u32 categoryId,
                            nn::nex::RatingStatsFlags::RatingStatsFlags dissconnectFlag,
                            REPORT_RATE rate,
                            const void* battleLog,
                            u32 battleLogSize, 
                            BtlRecordResult2 result2);

  //------------------------------------------------------------------
  /**
   * @brief 通信中か判定
   *
   * @return 通信中ならtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsConnecting( void ) const;

  //------------------------------------------------------------------
  /**
   * @brief 通信キャンセルリクエストを発行する。
   * @brief キャンセルが実施された場合、完了時にOnNexRatingClientCanceled()がコールされる。
   *
   * @note  通信状態にかかわらず、コールしても良い。
   *
   * @return キャンセル処理が実施されたならtrueを返却する
   */
  //------------------------------------------------------------------
  bool CancelConnecting( void );

  //------------------------------------------------------------------
  /**
   *	@brief  レーティングセッショントークンを取得
   *
   *  @param[in]  key セッショントークンの格納先
   */
  //------------------------------------------------------------------
  void GetRatingSessionToken( nn::nex::qVector<qByte>& key );

public:
  //------------------------------------------------------------------
  /*
   * @brief  完了通知検知用関数
   *
   * @caution コールバック関数にのみ開示するため、ユーザー使用禁止！
   *
   * @param[in,out]  pContext 完了通知対象となるコンテキスト
   */
  //------------------------------------------------------------------
  void OnCreateSessionCompletion(nn::nex::CallContext *pContext);
  void OnJoinSessionCompletion(nn::nex::CallContext *pContext);
  void OnReportRatingCompletion(nn::nex::CallContext *pContext);

private:
  //------------------------------------------------------------------
  /**
   * @brief 通信ロック状態化判定
   *
   * @return 通信ロック状態ならtrueを返却
   */
  //------------------------------------------------------------------
  bool IsAccessLock( void );

  //------------------------------------------------------------------
  /**
   * @brief 関数呼び出しエラー通知処理
   *
   * @param result
   */
  //------------------------------------------------------------------
  void ReportFunctionError( void );

  //------------------------------------------------------------------
  /**
   * @brief キャンセル通知処理
   */
  //------------------------------------------------------------------
  void ReporCanceled( void );

  //------------------------------------------------------------------
  /**
   * @brief エラー通知処理
   *
   * @param[in] result 実行結果
   */
  //------------------------------------------------------------------
  void ReportError( const nn::nex::qResult& result );

private:
    static const u32             LISTENER_NUM = 4;          //!< リスナ最大登録可能数
    //
    NexRatingClientListener*     m_listener[LISTENER_NUM];  //!< レーティング用リスナ
    //
    bool                         m_isInit;                  //!< 初期化フラグ
    bool                         m_isAccess;                //!< 通信フラグ
    //
    nn::nex::ProtocolCallContext* m_context;                //!< コンテキスト
    //
    nn::nex::RatingClient*       m_ratingClient;            //!< レーティングクライアント
    nn::nex::RatingSessionToken* m_ratingSessionToken;      //!< セッショントークン
    nn::nex::RatingStats*        m_reportStats[E_SLOT_MAX]; //!< レポート情報
    //
    nn::nex::qList<nn::nex::RatingStats>* m_statusList;     //!< レポート情報リスト
};


GFL_NAMESPACE_END(P2P)
GFL_NAMESPACE_END(NetLib)

#endif      // __NEX_RATING_CLIENT_H__
#endif // GF_PLATFORM_CTR
