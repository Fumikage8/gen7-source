#ifdef  GF_PLATFORM_CTR
//=============================================================================
/**
 * @file   gflnet_FiendManager.cpp
 * @brief  フレンド・プレゼンス管理マネージャクラス
 * @author h.suzuki
 * @date   2012.2.17
 */
//=============================================================================

#include <gflnet2/include/friends/gflnet2_FriendManager.h>
#include "gflnet2_FriendAddThread.h"
#include "gflnet2_FriendLoginThread.h"



//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(gflnet2::friends::FriendManager)
template class gfl2::base::SingletonAccessor<gflnet2::friends::FriendManager>;
#endif


namespace gflnet2{
namespace friends{


//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
FriendManager::FriendManager( void )
{
  m_isInit         = false;
  m_currentThread  = NULL;
  m_addThread      = NULL;
  m_loginThread    = NULL;
  m_storeFriendKeyNum = 0;
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
FriendManager::~FriendManager()
{
  Finalize();
}

//------------------------------------------------------------------
/**
 * @brief  FriendManagerの初期化
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::Initialize( void )
{
  nn::Result ret = nn::ResultSuccess();

  if( !m_isInit )
  {
    ret = nn::friends::CTR::Initialize();

    if( ret == nn::ResultSuccess() ){
      m_isInit = true;
    }
  }
  return ret;
}


//------------------------------------------------------------------
/**
 * @brief   更新処理
 */
//------------------------------------------------------------------
void FriendManager::Update( void )
{
}

//------------------------------------------------------------------
/**
 * @brief  FriendManagerの終了処理
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::Finalize( void )
{
  nn::Result ret = nn::ResultSuccess();

  if( m_isInit )
  {
    Logout();

    ret = nn::friends::CTR::Finalize();

    if( ret == nn::ResultSuccess() ){
      m_isInit = false;
    }
  }
  return ret;
}


//------------------------------------------------------------------
/**
 * @brief  初期化状態を調査
 * @result true： 初期済み
 *         false: 未初期化・または初期化中
 */
//------------------------------------------------------------------
bool FriendManager::IsInitialized( void )
{
  return nn::friends::CTR::IsInitialized();
}


//------------------------------------------------------------------
/**
 * @brief  フレンドサーバログイン状態を調査
 * @result true： ログイン済み
 *         false: 未ログイン・またはログイン準備中
 */
//------------------------------------------------------------------
bool FriendManager::IsLoggedIn( void )
{
  return nn::friends::CTR::HasLoggedIn();
}


//------------------------------------------------------------------
/**
 * @brief  Miiの初期設定が完了しているか判定
 *
 * @retval true： 設定済み
 * @retval false: 未設定
 */
//------------------------------------------------------------------
bool FriendManager::IsMyPreferenceValid( void )
{
  return nn::friends::CTR::IsMyPreferenceValid();
}


//------------------------------------------------------------------
/**
 * @brief  自身のMiiの名前を取得する
 *
 * @return 取得に成功したならtrueを返却
 */
//------------------------------------------------------------------
bool FriendManager::GetMyScreenName( gfl2::str::STRCODE screenName[SCREEN_NAME_SIZE] )
{
  nn::Result ret = nn::friends::CTR::GetMyScreenName( screenName );
  return ret.IsSuccess();

}

//------------------------------------------------------------------
/**
 * @brief  フレンドサーバへのログイン要求
 *
 * @note   同期にてフレンドサーバへの接続を行う。
 *
 * @param  heap            内部スレッド確保用に使用されるメモリ
 * @param  listener        ログイン完了通知用リスナ。
 *                         返り値がResultSuccess()でない場合はリスナは発行されない
 * @param  timeout         ログイン処理のタイムアウト時間
 * @param  thread_priority ログイン通知を行うスレッドのプライオリティー
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::Login( FriendLoginListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority )
{
  nn::Result ret = nn::ResultSuccess();

  WaitAsyncFinished();

  m_timeout = nn::fnd::TimeSpan::FromNanoSeconds( timeout.GetNanoSeconds() );
  m_event.Initialize( false );
  ret = nn::friends::CTR::Login( &m_event );

  if( ret == nn::ResultSuccess() )
  {
    m_event.Wait( m_timeout );

    ret = nn::friends::CTR::GetLastResponseResult();

    if( listener != NULL ){
      listener->OnFriendServerLoginFinish( ret );
    }
  }
  m_event.Finalize();

  return ret;
}


//------------------------------------------------------------------
/**
 * @brief  フレンドサーバへのログイン要求
 *
 * @attention 非同期処理終了後、必ずDeleteAsync()を実行すること
 *
 * @note   非同期にてフレンドサーバへの接続を行う。
 *
 * @param  heap            内部スレッド確保用に使用されるメモリ
 * @param  listener        ログイン完了通知用リスナ。
 *                         返り値がResultSuccess()でない場合はリスナは発行されない
 * @param  timeout         ログイン処理のタイムアウト時間
 * @param  thread_priority ログイン通知を行うスレッドのプライオリティー
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::LoginASync( gfl2::heap::HeapBase* heap, FriendLoginListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority )
{
  DeleteAsync();

  m_timeout = nn::fnd::TimeSpan::FromNanoSeconds( timeout.GetNanoSeconds() );
  m_event.Initialize( false );
  nn::Result ret = nn::friends::CTR::Login( &m_event );

  if( ret == nn::ResultSuccess() )
  {
    // スレッド開始
    m_loginThread = GFL_NEW_LOW( heap ) FriendLoginThread( heap, THREAD_STACK_SIZE, listener, &m_event, &m_timeout );
    m_loginThread->Start();
    //
    m_currentThread = m_loginThread;
  }
  else
  {
    m_event.Finalize();
  }
  return ret;
}



//------------------------------------------------------------------
/**
 * @brief  フレンドサーバへのログアウト要求
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::Logout( void )
{
  this->DeleteAsync();

  m_event.Finalize();
  return  nn::friends::CTR::Logout();
}


//------------------------------------------------------------------
/**
 * @brief  自分のプリシンバルIDを取得。
 *
 * @result プリシンバルID。フレンドサーバに一度も接続されていない場合は
 *         INVALID_PRINCIPAL_IDが返却される。
 */
//------------------------------------------------------------------
PrincipalId FriendManager::GetMyPrincipalId( void )
{
  return nn::friends::GetMyPrincipalId();
}

//------------------------------------------------------------------
/**
 * @brief  フレンドリストに登録されたフレンドキーのリストを取得。
 *         取得されるキーには、フレンド関係がまだ成立していないものも含まれる。
 *
 * @param  pFriendKeyList フレンドキー格納先バッファ
 * @param  pNum           取得できたフレンドキーの数
 * @param  offset         取得を開始するフレンドキーのインデックス[0〜(FRIEND_LIST_SIZE-1)]
 * @param  size           バッファに格納する最大取得件数
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::GetFriendKeyList( FriendKey *pFriendKeyList, size_t *pNum, size_t offset, size_t size )
{
  return nn::friends::CTR::GetFriendKeyList( pFriendKeyList, pNum, offset, size );
}

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
nn::Result FriendManager::GetFriendNameList( gfl2::str::STRCODE (*pScreenNameList)[SCREEN_NAME_SIZE], const FriendKey *pFriendKeyList,
                                             size_t size, bool replaceForeignCharacters, u8* pFontRegionList )
{
  return nn::friends::CTR::GetFriendScreenName( pScreenNameList, pFriendKeyList, size, replaceForeignCharacters, pFontRegionList );
}

//------------------------------------------------------------------
/**
 * @brief  フレンド関係状態のビット情報を取得する
 *
 * @param  pAttributeFlagsList ビット情報格納先
 * @param  pFriendKeyList      取得対象となるフレンドキーリスト
 * @param  size                バッファに格納する最大取得件数
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::GetFriendAttributeFlags( bit32* pAttributeFlagsList, const FriendKey* pFriendKeyList, size_t size )
{
  return nn::friends::CTR::GetFriendAttributeFlags( pAttributeFlagsList, pFriendKeyList, size );
}

//------------------------------------------------------------------
/**
 * @brief  暗号化ローカルフレンドコードを復号化。
 *         暗号化されたコードがフレンドのものでない場合は、「INVALID_LOCAL_FRIEND_CODE」が返却される
 *
 * @param  dst            複合化されたローカルフレンドキー格納先バッファ
 * @param  src            暗号化されたローカルフレンドキー配列
 * @param  size           暗号化されたローカルフレンドキー件数
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::UnscrambleLocalFriendCode( LocalFriendCode* dst, const ScrambledLocalFriendCode* src, size_t size )
{
  return nn::friends::CTR::UnscrambleLocalFriendCode( dst, src, size );
}

//------------------------------------------------------------------
/**
 * @brief  ほかのユーザにフレンド登録してもらうのに必要な、自分のアカウント情報を取得する。
 *         ※事前にnn::cfg::CTR::Initialize()の実行が必須！
 *
 * @param  pApproachContext アカウント情報の格納先バッファ
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::GetMyApproachContext( ApproachContext *pApproachContext )
{
  return nn::friends::CTR::GetMyApproachContext( pApproachContext );
}


//-------------------------------------------------------------------------------
/**
 * @brief 非同期通信が実行中かどうかを調べる
 * @retval true   実行中
 * @retval false  実行していない
 */
//-------------------------------------------------------------------------------
bool FriendManager::CheckAsync( void ) const
{
  if( m_addThread )
  {
    if( m_addThread->IsAlive() )
    {
      return true;
    }
  }

  if( m_loginThread )
  {
    if( m_loginThread->IsAlive() )
    {
      return true;
    }
  }

  return false;
}

//-------------------------------------------------------------------------------
/**
 * @brief 実行中の非同期通信をキャンセルする
 *
 * @note  CheckAsync()で非同期処理の終了を確認した後、EndAsync()を呼んでください。
 */
//-------------------------------------------------------------------------------
void FriendManager::CancelAsync( void )
{
  if( this->CheckAsync() ) {
    if( m_addThread )
    {
      m_addThread->Kill();
    }

    if( m_loginThread )
    {
      m_loginThread->Kill();
    }
  }
}


//-------------------------------------------------------------------------------
/**
 * @brief スレッドの終了待ちを行います。
 */
//-------------------------------------------------------------------------------
void FriendManager::WaitAsyncFinished( void )
{
  if( m_loginThread ){
    m_loginThread->Cancel();
  }
  else if( m_addThread ){
    m_addThread->Cancel();
  }
  this->CancelAsync();

  while( this->CheckAsync() )
  {
    nn::os::Thread::Sleep( nn::fnd::TimeSpan::FromMilliSeconds(1) );
  }
}


//------------------------------------------------------------------
/**
 * @brief  ほかのユーザにフレンド登録してもらうのに必要な、自分のアカウント情報を取得する。
 *
 * @attention 事前にnn::cfg::CTR::Initialize()の実行が必須！
 *
 *
 * @note   非同期にて自分のアカウント情報を取得する。
 * @note   完了通知は「gflnet2::friends::FriendAddListener::OnFriendAddFinish」で検知すること。
 * @note   非同期処理終了後、必ずDeleteAsync()を実行すること
 *
 * @param  heap            内部スレッド確保用に使用されるメモリ
 * @param  approachContext 登録対象アカウント情報
 * @param  listener        登録完了通知用リスナ
 * @param  timeout         タイムアウト時間
 * @param  thread_priority 登録完了通知を行うスレッドのプライオリティー
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::AddFriendWithApproachAsync( gfl2::heap::HeapBase* heap, const ApproachContext & approachContext, FriendAddListener* listener, const nn::fnd::TimeSpan &timeout, int thread_priority )
{
  DeleteAsync();

  m_timeout = nn::fnd::TimeSpan::FromNanoSeconds( timeout.GetNanoSeconds() );
  m_event.Initialize( false );

  nn::Result result = nn::friends::CTR::AddFriendWithApproach( &m_event, approachContext );

  if( result.IsSuccess() )
  {
    // スレッド開始
    m_addThread = GFL_NEW_LOW( heap ) gflnet2::friends::FriendAddThread( heap, THREAD_STACK_SIZE, listener, &m_event, &m_timeout );
    m_addThread->Start();
    m_currentThread = m_addThread;
  }
  else{
    m_event.Finalize();
  }

  return result;
}


//-------------------------------------------------------------------------------
/**
 * @brief 非同期処理スレッドを破棄する
 */
//-------------------------------------------------------------------------------
void FriendManager::DeleteAsync( void )
{
  this->WaitAsyncFinished();
  m_event.Finalize();

  GFL_SAFE_DELETE( m_currentThread );
  m_addThread   = NULL;
  m_loginThread = NULL;
}


//------------------------------------------------------------------
/**
 * @brief  CTRに登録されているフレンドキーリストをマネージャに記録させる
 *
 * @result 実行結果
 */
//------------------------------------------------------------------
nn::Result FriendManager::StoreFriendKeyList( void )
{
  nn::Result result;

  result = nn::friends::GetFriendKeyList( m_storeFriendKey, &m_storeFriendKeyNum );
  if( result.IsFailure() ){
    return result;
  }
  if( m_storeFriendKeyNum > 0 ){
    result = GetFriendAttributeFlags( m_storeAttribute, m_storeFriendKey, m_storeFriendKeyNum );
  }
  return result;
}


//------------------------------------------------------------------
/**
 * @brief  指定のフレンドキーがCTRに登録されているフレンドキーリストに登録されているか判定。
 *
 * @attention 事前にStoreFriendKeyList()をコールすること。
 *
 * @note   ローカルフレンドは含めるが、フレンド関係未成立のユーザーは除外する。
 *
 * @param[in]  friendKey 検査対象となるフレンドキー
 * @param[in]  isPidOnly  PIDのみで判定するならtrueを指定
 *
 * @return フレンドキーリストに登録されているならtrueを返却
 */
//------------------------------------------------------------------
bool FriendManager::IsExistFriendKey( const FriendKey& friendKey, bit32 checkBit, bool isPidOnly )
{
  if( friendKey.principalId != INVALID_PRINCIPAL_ID )
  {
    for( size_t i = 0; i < m_storeFriendKeyNum; i++ )
    {
      if( friendKey.principalId == m_storeFriendKey[i].principalId ){
        return (checkBit==ATTRIBUTE_FLAG_NONE) || ((m_storeAttribute[i]&checkBit) != 0);
      }
    }
  }

  if( !isPidOnly )
  {
    if( friendKey.localFriendCode != INVALID_LOCAL_FRIEND_CODE )
    {
      for( size_t i = 0; i < m_storeFriendKeyNum; i++ )
      {
        if( friendKey.localFriendCode == m_storeFriendKey[i].localFriendCode ){
          return (checkBit==ATTRIBUTE_FLAG_NONE) || ((m_storeAttribute[i]&checkBit) != 0);
        }
      }
    }
  }

  return false;
}


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
bool FriendManager::IsApprovedServerFriend( PrincipalId principalId )
{
  if( principalId == INVALID_PRINCIPAL_ID ){
    return false;  // 無効なIDの場合は、一致条件として処理せず
  }

  for( size_t i = 0; i < m_storeFriendKeyNum; i++ )
  {
    if( principalId == m_storeFriendKey[i].principalId ){
      return IsFriendOnFriendServer(m_storeAttribute[i]);
    }
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief  フレンドサーバが承認している友達か判定
 *
 * @param[in]  attribute アトリビュートビット
 *
 * @return フレンドサーバが承認している友達であればtrueを返却する
 */
//------------------------------------------------------------------
bool FriendManager::IsFriendOnFriendServer( bit32 attribute )
{
  if( (attribute&ATTRIBUTE_FLAG_ESTABLISHED) == 0 ){
    return false;      // 友達関係未成立
  }

  // フレンドサーバが友達として認識しているか？
  return (attribute&ATTRIBUTE_FLAG_REMOTE_ACCESSIBLE) != 0;
}


//------------------------------------------------------------------
/**
 * @brief  フレンドサーバ用エラー情報からネットワークエラーコードを取得する
 *
 * @param  エラーを管理するnn::Result情報
 *
 * @result ネットワークエラーコード
 */
//------------------------------------------------------------------
u32 FriendManager::GetNetworkErrorCode( const nn::Result& result )
{
  return nn::friends::ResultToErrorCode( result );
}


} // friends
} // gflnet
#endif
