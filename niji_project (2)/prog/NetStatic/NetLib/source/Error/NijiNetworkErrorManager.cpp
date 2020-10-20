//==============================================================================
/**
 * @file	NijiNetworkErrorManager.cpp
 * @brief	Niji通信エラー管理クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 16:52:02
 */
// =============================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorListener.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkForceDisconnectListener.h"
#include "NetStatic/NetLib/source/Error/NijiNetworkErrorDialog.h" // 非公開ヘッダ
#include "NetStatic/NetLib/include/Wifi/WifiConnectRunner.h"
#include "NetStatic/NetLib/include/P2P/P2pConnectionManager.h"

// heap
#include <heap/include/gfl2_Heap.h>
#include "system/include/HeapDefine.h"

// mine
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameEvent.h"

SINGLETON_INSTANCE_DEFINITION(NetLib::Error::NijiNetworkErrorManager)
template class gfl2::base::SingletonAccessor<NetLib::Error::NijiNetworkErrorManager>;


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

// デバッグ機能
#if PM_DEBUG
bool NijiNetworkErrorManager::m_debugRButtonErrorEnable = false; //! Rボタンで通信エラー発生有効フラグ
bool NijiNetworkErrorManager::m_debugLButtonErrorEnable = false; //! Rボタンで通信エラー発生有効フラグ
#endif // PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 * @param   networkErrorDialog エラーダイアログ管理クラスのポインタ
 */
//------------------------------------------------------------------
NijiNetworkErrorManager::NijiNetworkErrorManager( NijiNetworkErrorDialog* networkErrorDialog ) :
   m_state(STATE_INITIAL)
  ,m_networkErrorDialog(networkErrorDialog)
  ,m_pListenerList()
  ,m_pForceDisconnectListener(NULL)
  ,m_isP2pDisconnectSpan(false)
  ,m_isSaveCriticalSpan(false)
  ,m_isNotNotifySpan(false)
{
  // 0クリア
  gfl2::std::MemClear( &m_lastErrorInfoData, sizeof(NijiNetworkError::ERROR_INFO_DATA) );

  // エラーリスナーのリストバッファく作成
  {
    gfl2::heap::HeapBase* pSysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_NETWORK_RESIDENT );
    m_pListenerList.CreateBuffer( pSysHeap, LISTNER_MAX );
  }
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
NijiNetworkErrorManager::~NijiNetworkErrorManager()
{
  m_pListenerList.Clear();
}

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
void NijiNetworkErrorManager::SetNetworkError( NijiNetworkError& error )
{
  // セーブ中FatalError期間は、全ての通信エラーをFatalErrorにする
  if( m_isSaveCriticalSpan == true )
  {
    error.SetFatalError();
    GFL_PRINT("!!!　強制でFatalErrorに変更\n");
  }


  // P2P切断中は、P2P切断エラーが必ず来るので無視する
  if( m_isP2pDisconnectSpan == true )
  {
    if( error.GetErrorInfoData().error_level == NijiNetworkError::ERROR_LEVEL_2 )
    {
      GFL_PRINT(">NetError SetNetworkError P2PIgnore\n");
      return;
    }
  }

  // 初めてのエラーセットの場合は、条件なしで設定
  // 既にエラーがセットされている場合は、エラーレベルが高い場合のみ上書き
  if( m_state == STATE_INITIAL || error.GetErrorInfoData().error_level > m_lastErrorInfoData.error_level )
  {
    // エラーを保存
    m_lastErrorInfoData = error.GetErrorInfoData();
    // エラー設定された状態 
    m_state = STATE_ERROR_SET;

    // エラー通知（リスナーが登録されていない場合でも問題ない）
    this->NotifyListner();

    // エラーダイアログ表示リクエスト（エラーダイアログは即時表示）
    this->ErrorDialogRequest();

    GFL_PRINT(">NetError SetNetworkError Set\n");
  }
  else
  {
    GFL_PRINT(">NetError SetNetworkError Ignore\n");
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 通信エラーが発生したかどうか
 *
 * @retval true：発生した
 * @retval false：発生してない
 */
/* -------------------------------------------------------------------------*/
bool NijiNetworkErrorManager::IsError() const
{
  // エラーセットされた状態もエラー発生に変更 2016/02/01 ichiraku
  if( m_state == STATE_ERROR_SET ||
      m_state == STATE_ERROR_NOTICE )
  {
    return true;
  }
  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief セッション切断が発生したかどうか(バトルスポット専用)
 *
 * @retval true：発生した
 * @retval false：発生してない
 */
/* -------------------------------------------------------------------------*/
bool NijiNetworkErrorManager::IsSessionLeft() const
{
  // @fix GFNMCat[5723]:バトルスポットのロイヤルでステーション不一致のエラーが発生すると、アプリ終了する為切断数があがる
  // バトルスポットのNetZFatalを切断扱いとする
  if( m_lastErrorInfoData.error_level == NijiNetworkError::ERROR_LEVEL_0 &&
      m_lastErrorInfoData.error_type == NijiNetworkError::ERROR_TYPE_NETZ_FATAL )
  {
    return true;
  }

  if( m_lastErrorInfoData.error_level == NijiNetworkError::ERROR_LEVEL_0 &&
      m_lastErrorInfoData.error_type == NijiNetworkError::ERROR_TYPE_SESSION_LEFT )
  {
    return true;
  }

  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief Fatalエラーが発生したかどうか
 *
 * @retval true：発生した
 * @retval false：発生してない
 */
/* -------------------------------------------------------------------------*/
bool NijiNetworkErrorManager::IsFatalError() const
{
  if( IsError() )
  {
    if( m_lastErrorInfoData.error_level == NijiNetworkError::ERROR_LEVEL_4 )
    {
      return true;
    }
  }
  return false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 更新
 */
/* -------------------------------------------------------------------------*/
void NijiNetworkErrorManager::Update()
{
#if PM_DEBUG
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    const gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

    // P2P切断エラー
    if( m_debugLButtonErrorEnable )
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_L ) )
      {
        NijiNetworkError error;
        error.SetResult( NijiNetworkError::ERROR_TYPE_SESSION_LEFT );
        SetNetworkError( error );
      }
    }

    // WiFi切断エラー
    if( m_debugRButtonErrorEnable )
    {
      if( pButton->IsTrigger( gfl2::ui::BUTTON_R ) )
      {
        NijiNetworkError error;
        error.SetResult( NijiNetworkError::ERROR_TYPE_LOGINSERVER_DISCONNECT );
        SetNetworkError( error );
      }
    }
  }
#endif // PM_DEBUG

  // エラー通知が完了していない場合は、通知するまでチェックする
  if( m_state == STATE_ERROR_SET ) 
  {
    this->NotifyListner();
  }
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 共通の通信エラー対処
 */
/* -------------------------------------------------------------------------*/
void NijiNetworkErrorManager::CommonNetworkErrorHandling()
{
  // 通知していなくてもくる GFL_ASSERT( m_state == STATE_ERROR_NOTICE );

  GFL_PRINT(">NetError 共通通信エラー対処 \n");

  // P2Pは各自で処理しているはず
#if PM_DEBUG
  //! エラー対処正常終了チェック
  GFL_SINGLETON_INSTANCE(NetLib::P2P::P2pConnectionManager)->DEBUG_CheckErrorHandling();
#endif // PM_DEBUG

  // 念の為
  if( IsError() == true )
  {
    // WiFi切断が必要なエラーかどうか
    if( m_lastErrorInfoData.error_level >= NijiNetworkError::ERROR_LEVEL_3 )
    {

      // 強制WiFi切断
#if defined(GF_PLATFORM_CTR)
      NetLib::Wifi::ForceDisconnectWifi();
#endif
      NetLib::NijiNetworkSystem::TerminateNetwork();

      // 強制切断リスナー通知
      if( m_pForceDisconnectListener != NULL )
      {
        m_pForceDisconnectListener->OnForceDisconnect();
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   エラー情報をクリア（＝通信エラー対処が完了した）
 * @note    呼ぶ箇所・タイミングを間違えないように。
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::Clear()
{
  //  GFL_ASSERT( m_state == STATE_ERROR_NOTICE );
  // 0クリア
  gfl2::std::MemClear( &m_lastErrorInfoData, sizeof(NijiNetworkError::ERROR_INFO_DATA) );
  // 初期状態
  m_state = STATE_INITIAL;
  m_isP2pDisconnectSpan = false;
  m_isSaveCriticalSpan  = false;
  m_isNotNotifySpan = false;
  GFL_PRINT(">NetError NijiNetworkErrorManager::Clear() \n");
}

//------------------------------------------------------------------
/**
 * @brief   イベントリスナー登録
 * @param   listener   通知を受け取るNijiNetworkErrorListener
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::RegistListener( NijiNetworkErrorListener* listener )
{
  GFL_PRINT("NijiNetworkErrorManager::RegistListener\n");

  // リスナーリストが空
  if( m_pListenerList.Size() >= LISTNER_MAX )
  {
    GFL_ASSERT_MSG(0,"listener max!");
  }

  // 末尾登録
  m_pListenerList.PushBack( listener );
}

//------------------------------------------------------------------
/**
 * @brief   イベントリスナー解除
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::UnregistListener( void )
{
  GFL_PRINT("NijiNetworkErrorManager::UnregistListener\n");
  // 末尾を削除
  m_pListenerList.PopBack();
}

//------------------------------------------------------------------
/**
 * @brief   強制切断リスナー登録
 * @param   listener   通知を受け取るNijiNetworkForceDisconnectListener
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::RegistForceDisconnectListener( NijiNetworkForceDisconnectListener* listener )
{
  m_pForceDisconnectListener = listener;
}

//------------------------------------------------------------------
/**
 * @brief   強制切断リスナー解除
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::UnregistForceDisconnectListener( void )
{
  m_pForceDisconnectListener = NULL;
}

//------------------------------------------------------------------
/**
 * @brief   P2P切断期間を設定
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::P2pDisconnectSpanOn()
{
  GFL_PRINT("NijiNetworkErrorManager::P2pDisconnectSpanOn\n");
  m_isP2pDisconnectSpan = true;
}
void NijiNetworkErrorManager::P2pDisconnectSpanOff()
{
  GFL_PRINT("NijiNetworkErrorManager::P2pDisconnectSpanOff\n");
  m_isP2pDisconnectSpan = false;
}
bool NijiNetworkErrorManager::IsP2pDisconnectSpanOn()
{
  return m_isP2pDisconnectSpan;
}

//------------------------------------------------------------------
/**
 * @brief   セーブ中通信エラーをFatalErroにする期間を設定
 *          必ずSaveCriticalSpanOffで解除すること
 *
 * @attention この期間は全ての通信エラーがFATALになります。
 *            使用を間違えるとガイドライン違反になります。
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::SaveCriticalSpanOn()
{
  GFL_PRINT("NijiNetworkErrorManager::SaveCriticalSpanOn\n");
  m_isSaveCriticalSpan = true;
}
void NijiNetworkErrorManager::SaveCriticalSpanOff()
{
  GFL_PRINT("NijiNetworkErrorManager::SaveCriticalSpanOff\n");
  m_isSaveCriticalSpan = false;
}

//------------------------------------------------------------------
/**
 * @brief   通信エラーを通知しない期間を設定
 *           必ずNotNotifySpanOffで解除すること
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::NotNotifySpanOn()
{
  GFL_PRINT("NijiNetworkErrorManager::NotNotifySpanOn\n");
  m_isNotNotifySpan = true;
}
void NijiNetworkErrorManager::NotNotifySpanOff()
{
  GFL_PRINT("NijiNetworkErrorManager::NotNotifySpanOff\n");
  m_isNotNotifySpan = false;
}


//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   リスナーにエラー通知
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::NotifyListner()
{
  // 通知しない期間に設定されていた場合
  if( m_isNotNotifySpan == true )
  {
    GFL_PRINT("!!!!!!!!!!!!!!!!!\n");
    GFL_PRINT("通信エラー無視期間設定なので、無視する\n");
    GFL_PRINT("!!!!!!!!!!!!!!!!!\n");
    // エラー通知した事にして終了。
    m_state = STATE_ERROR_NOTICE;
    return;
  }


  // エラーセットされている状態の時のみ通知する
  if( m_state == STATE_ERROR_SET ) 
  {
    // リスナーにエラー通知するかチェック
    if( IsNotifyListener() == true )
    {
      // 末尾に登録された物にのみ通知する
      gfl2::util::ListIterator<NijiNetworkErrorListener*> listener = m_pListenerList.End();
      --listener;
      (*listener)->OnNijiNetworkError();

      // エラー通知完了状態
      m_state = STATE_ERROR_NOTICE;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   リスナーにエラー通知するかチェック
 * @retval  true:通知する
 * @retval  false:通知しない
 */
//------------------------------------------------------------------
bool NijiNetworkErrorManager::IsNotifyListener() const
{
    // リスナーが存在しないと通知しない
  if( m_pListenerList.Size() == 0 )
  {
    return false;
  }

  // エラーレベル0の場合はエラー通知をしない
  if( m_lastErrorInfoData.error_level == NijiNetworkError::ERROR_LEVEL_0 )
  {
    return false;
  }

  // 通信イベントでないと通知しない
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
    GameSys::GameEvent* pGameEvent = pGameEventManager->GetGameEvent();

    bool bIsNormalEvent = ( pGameEvent ) ? pGameEvent->IsNormalEvent() : true;
    if( bIsNormalEvent == true )
    {
      return false;
    }
  }

  // プロセスがないと通知しない
  //   memo:イベント処理内でアプリが存在しない時に通信エラー発生した時にどうするかという話
  //        アプリ開始前に通信エラーチェックするより、通知をプールしておいてアプリ生成された時に通知して
  //        外側から終わらせたほうが共通化出来るし、漏れがない。
  {
    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    if( pGameManager->IsProcessExists() == false )
    {
      return false;
    }
  }

     
  return true;
}

//------------------------------------------------------------------
/**
 * @brief   エラーダイアログに表示リクエスト
 */
//------------------------------------------------------------------
void NijiNetworkErrorManager::ErrorDialogRequest()
{
  // FatalErrorの場合はエラーアプレットを表示しない
  if( m_lastErrorInfoData.error_type == NijiNetworkError::ERROR_TYPE_FATAL )
  {
    return;
  }

  // エラー通知
  m_networkErrorDialog->RequestShow( m_lastErrorInfoData );
}


GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)
