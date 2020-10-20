#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_FriendManager.h
 * @brief  フレンド・プレゼンス管理マネージャクラス
 * @author h.suzuki
 */
//=============================================================================
#ifndef __GFLNET_FRIEND_MANAGER_H__
#define __GFLNET_FRIEND_MANAGER_H__
#pragma once

#include <nn/init.h>
#include <nn.h>
#include <nn/ac.h>
#include <nn/ac/ac_Result.h>
#include <nn/friends.h>


#include <gflnet2/include/gflnet2_NetworkDefine.h>
//#include <ro/gfl_RoManager.h>

#include <gflnet2/include/friends/gflnet2_FriendLoginListener.h>
#include <gflnet2/include/friends/gflnet2_FriendAddListener.h>
#include <str/include/gfl2_StrSys.h>

#include <base/include/gfl2_Singleton.h>

// thread
#include <thread/include/gfl2_Thread.h>
#include <thread/include/gfl2_Threadstatic.h>

namespace gflnet2{
/**
* @namespace gflnet2::friends
* @brief     フレンドプレゼンス関連
*/
namespace friends{

//--------------------------------------------------------------------------
class FriendLoginThread;
class FriendAddThread;

//--------------------------------------------------------------------------
typedef nn::friends::PrincipalId               PrincipalId;
typedef nn::friends::FriendKey                 FriendKey  ;
typedef nn::friends::LocalFriendCode           LocalFriendCode;
typedef nn::friends::PrincipalId               PrincipalId;
typedef nn::friends::ApproachContext           ApproachContext;
typedef nn::uds::CTR::ScrambledLocalFriendCode ScrambledLocalFriendCode;

/**
 * @brief  フレンドプレゼンス管理マネージャクラス
 */
class FriendManager : public gfl2::base::Singleton<FriendManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(FriendManager); //コピーコンストラクタ＋代入禁止

public:
  static const int             FRIEND_LIST_SIZE            = nn::friends::CTR::FRIEND_LIST_SIZE;          //!< フレンドリスト最大数
  static const PrincipalId     INVALID_PRINCIPAL_ID        = nn::friends::CTR::INVALID_PRINCIPAL_ID;      //!< プリンシバルID
  static const LocalFriendCode INVALID_LOCAL_FRIEND_CODE   = nn::friends::CTR::INVALID_LOCAL_FRIEND_CODE; //!< フレンドコード
  static const u32             SCREEN_NAME_SIZE            = nn::friends::CTR::SCREEN_NAME_SIZE;          //!< スクリーンネームサイズ

public:
  static const bit32           ATTRIBUTE_FLAG_NONE              = 0;                                                   //!< フラグなし
  static const bit32           ATTRIBUTE_FLAG_ESTABLISHED       = nn::friends::CTR::ATTRIBUTE_FLAG_ESTABLISHED;        //!< フレンド関係が成立したことがある
  static const bit32           ATTRIBUTE_FLAG_REMOTE_ACCESSIBLE = nn::friends::CTR::ATTRIBUTE_FLAG_REMOTE_ACCESSIBLE ; //!< 相手からフレンド関係を解消されていない

private:
  bool                 m_isInit;            //!< 初期済みフラグ
  nn::os::Event        m_event;             //!< 非同期処理イベント
  nn::fnd::TimeSpan    m_timeout;           //!< 非同期処理タイムアウト
  //
  gfl2::thread::ThreadInterface*   m_currentThread;     //!< 非同期処理通知用スレッド
  FriendAddThread*     m_addThread;         //!< 友達登録スレッド
  FriendLoginThread*   m_loginThread;       //!< ログインスレッド
  //
  size_t               m_storeFriendKeyNum;                //!< フレンドキー情報数
  FriendKey            m_storeFriendKey[FRIEND_LIST_SIZE]; //!< フレンドキーリスト
  bit32                m_storeAttribute[FRIEND_LIST_SIZE]; //!< フレンド属性リスト

private:
  static const int     THREAD_STACK_SIZE = 8192; //!< スレッド用スタックサイズ

private:
  bool CheckAsync( void ) const;
  void CancelAsync( void );
  void WaitAsyncFinished( void );
  bool IsFriendOnFriendServer( bit32 attribute );
public:
  void DeleteAsync( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FriendManager( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FriendManager();

  //------------------------------------------------------------------
  /**
   * @brief  FriendManagerの初期化
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Initialize( void );

  //------------------------------------------------------------------
  /**
   * @brief  FriendManagerの終了処理
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Finalize( void );

  //------------------------------------------------------------------
  /**
   * @brief  更新処理。
   *
   * @note   メインループ毎にポーリングすること
   */
  //------------------------------------------------------------------
  void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief  初期化状態を調査
   *
   * @retval true  初期済み
   * @retval false 未初期化・または初期化中
   */
  //------------------------------------------------------------------
  bool IsInitialized( void );

  //------------------------------------------------------------------
  /**
   * @brief  自身のMiiの初期設定が完了しているか判定
   *
   * @retval true  設定済み
   * @retval false 未設定
   */
  //------------------------------------------------------------------
  bool IsMyPreferenceValid( void );

  //------------------------------------------------------------------
  /**
   * @brief  自身のMiiの名前を取得する
   *
   * @return 取得に成功したならtrueを返却
   */
  //------------------------------------------------------------------
  bool GetMyScreenName( gfl2::str::STRCODE screenName[SCREEN_NAME_SIZE] );

  //------------------------------------------------------------------
  /**
   * @brief  自分のプリシンバルIDを取得。
   *
   * @return プリシンバルID。
   * @return フレンドサーバに一度も接続されていない場合はINVALID_PRINCIPAL_IDが返却される。
   */
  //------------------------------------------------------------------
  static PrincipalId GetMyPrincipalId( void );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドリストに登録されたフレンドキーのリストを取得。
   *
   * @note   取得されるキーには、フレンド関係がまだ成立していないものも含まれる。
   *
   * @param[in,out] pFriendKeyList フレンドキー格納先バッファ
   * @param[out]    pNum           取得できたフレンドキーの数
   * @param[in]     offset         取得を開始するフレンドキーのインデックス[0〜(FRIEND_LIST_SIZE-1)]
   * @param[in]     size           バッファに格納する最大取得件数
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result GetFriendKeyList( FriendKey *pFriendKeyList, size_t *pNum, size_t offset = 0, size_t size = FRIEND_LIST_SIZE );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドリストに登録されたフレンドの表示名リストを取得。
   *
   * @note   取得できる表示名の詳細は「nn::friends::CTR::GetFriendScreenName()」のAPIリファレンスを確認すること。
   *
   * @param[out] pScreenNameList          取得した表示名を格納するバッファ
   * @param[in]  pFriendKeyList           取得対象となるフレンドキーのリスト
   * @param[in]  size                     フレンドキーリストの件数
   * @param[in]  replaceForeignCharacters フレンドの表示名のフォントリージョンが自分と異なる場合にASCII以外の文字を?で置換するか
   * @param[out] pFontRegionList          フレンドの表示名のフォントリージョンを格納するバッファへのポインタ(NULL指定可能）
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result GetFriendNameList( gfl2::str::STRCODE (*pScreenNameList)[SCREEN_NAME_SIZE], const FriendKey *pFriendKeyList,
                                size_t size=1, bool replaceForeignCharacters=true, u8* pFontRegionList=NULL );

  //------------------------------------------------------------------
  /**
   * @brief  フレンド関係状態のビット情報を取得する
   *
   * @param[out]  pAttributeFlagsList ビット情報格納先
   * @param[in]   pFriendKeyList      取得対象となるフレンドキーリスト
   * @param[in]   size                バッファに格納する最大取得件数
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result GetFriendAttributeFlags( bit32* pAttributeFlagsList, const FriendKey* pFriendKeyList, size_t size =FRIEND_LIST_SIZE );

  //------------------------------------------------------------------
  /**
   * @brief  暗号化ローカルフレンドコードを復号化。
   *
   * @note   暗号化されたコードがフレンドのものでない場合は、「INVALID_LOCAL_FRIEND_CODE」が返却される
   *
   * @param[out]  dst            複合化されたローカルフレンドキー格納先バッファ
   * @param[in]   src            暗号化されたローカルフレンドキー配列
   * @param[in]   size           暗号化されたローカルフレンドキー件数
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result UnscrambleLocalFriendCode( LocalFriendCode* dst, const ScrambledLocalFriendCode* src, size_t size );

  //------------------------------------------------------------------
  /**
   * @brief  ほかのユーザにフレンド登録してもらうのに必要な、自分のアカウント情報を取得する。
   *
   * @attention 事前にnn::cfg::CTR::Initialize()の実行が必須！
   *
   * @param[out]  pApproachContext アカウント情報の格納先バッファ
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result GetMyApproachContext( ApproachContext *pApproachContext );

  //------------------------------------------------------------------
  /**
   * @brief  ほかのユーザにフレンド登録してもらうのに必要な自分のアカウント情報を取得する。
   *
   * @note   非同期にて自分のアカウント情報を取得する。
   * @note   完了通知は「gflnet2::friends::FriendAddListener::OnFriendAddFinish」で検知すること。
   * @note   非同期処理終了後、必ずDeleteAsync()を実行すること
   *
   * @attention 事前にnn::cfg::CTR::Initialize()の実行が必須！
   *
   * @param[in,out] heap            内部スレッド確保用に使用されるメモリ
   * @param[in]     approachContext 登録対象アカウント情報
   * @param[in]     listener        登録完了通知用リスナ
   * @param[in]     timeout         タイムアウト時間
   * @param[in]     thread_priority 登録完了通知を行うスレッドのプライオリティー
   *
   * @result 実行結果
   */
  //------------------------------------------------------------------
  nn::Result AddFriendWithApproachAsync( gfl2::heap::HeapBase* heap, const ApproachContext & approachContext, FriendAddListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバログイン状態を調査
   *
   * @retval true  ログイン済み
   * @retval false 未ログイン・またはログイン準備中
   */
  //------------------------------------------------------------------
  bool IsLoggedIn( void );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバへのログイン要求
   *
   * @note   同期にてフレンドサーバへの接続を行う。
   * @note   返り値がResultSuccess()でない場合はリスナは発行されない
   *
   * @param[in]  listener        ログイン完了通知用リスナ。
   * @param[in]  timeout         ログイン処理のタイムアウト時間
   * @param[in]  thread_priority ログイン通知を行うスレッドのプライオリティー
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Login( FriendLoginListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバへのログイン要求
   *
   * @note   非同期にてフレンドサーバへの接続を行う。
   * @note   返り値がResultSuccess()でない場合はリスナは発行されない
   *
   * @param[in,out] heap            内部スレッド確保用に使用されるメモリ
   * @param[in]     listener        ログイン完了通知用リスナ。
   * @param[in]     timeout         ログイン処理のタイムアウト時間
   * @param[in]     thread_priority ログイン通知を行うスレッドのプライオリティー
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result LoginASync( gfl2::heap::HeapBase* heap, FriendLoginListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバへのログアウト要求
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result Logout( void );

  //------------------------------------------------------------------
  /**
   * @brief  CTRに登録されているフレンドキーリストをマネージャに記録させる
   *
   * @return 実行結果
   */
  //------------------------------------------------------------------
  nn::Result StoreFriendKeyList( void );

  //------------------------------------------------------------------
  /**
   * @brief  指定のフレンドキーがCTRに登録されているフレンドキーリストに登録されているか判定。
   *
   * @attention 事前にStoreFriendKeyList()をコールすること。
   *
   * @note   ローカルフレンドは含めるが、フレンド関係未成立のユーザーは除外する。
   *
   * @param[in]  friendKey  検査対象となるフレンドキー
   * @param[in]  isPidOnly  PIDのみで判定するならtrueを指定
   *
   * @return フレンドキーリストに登録されているならtrueを返却
   */
  //------------------------------------------------------------------
  bool IsExistFriendKey( const FriendKey& friendKey, bit32 checkBit=ATTRIBUTE_FLAG_ESTABLISHED, bool isPidOnly=false );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバが承認している友達か判定
   *
   * @attention 事前にStoreFriendKeyList()をコールすること。
   *
   * @param[in]  principalId プリンシパルID
   *
   * @return フレンドサーバが承認している友達であればtrueを返却する
   */
  //------------------------------------------------------------------
  bool IsApprovedServerFriend( PrincipalId principalId );

  //------------------------------------------------------------------
  /**
   * @brief  フレンドサーバ用エラー情報からネットワークエラーコードを取得する
   *
   * @param[in]  エラーを管理するnn::Result情報
   *
   * @return ネットワークエラーコード
   */
  //------------------------------------------------------------------
  static u32 GetNetworkErrorCode( const nn::Result& result );
};



} // friends
} // gflnet

#endif // __GFLNET_FRIEND_MANAGER_H__

#endif
