//==============================================================================
/**
 * @file	NijiNetworkErrorDialog.cpp
 * @brief	通信エラーのエラー表示（アプレット）管理クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/11 17:00:40
 */
// =============================================================================
#include <nw/snd.h>
#include <heap/include/gfl2_Heap.h>
#include <base/include/gfl2_Singleton.h>
#include <thread/include/gfl2_Thread.h>
// NexManager
#include <gflnet2/include/nex/gflnet2_NexManager.h>

#include "NetStatic/NetLib/include/Error/NijiNetworkError.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErreulaApplet.h"
#include "system/include/HeapDefine.h"
#include "system/include/PokemonLanguage.h"
#include "Print/include/PrintSystem.h"

#include "arc_index/message.gaix"

// mine
#include "NijiNetworkErrorDialog.h" // 非公開ヘッダ

#include "NijiNetworkErrorDialogStatus.h" // Dialog Status Control

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Error)

//------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------
NijiNetworkErrorDialog::NijiNetworkErrorDialog()
{
  m_msgData = NULL;
  Initialize();
}

//------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------
NijiNetworkErrorDialog::~NijiNetworkErrorDialog()
{
  Terminate();
}


//=============================================================================
/**
 *  publicメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   表示リクエスト
 * @param   エラー情報クラス
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::RequestShow( NijiNetworkError::ERROR_INFO_DATA error )
{
  if( m_requestError.Size() >= MAX_REQUEST_NUM )
  {
    // システム上あり得ないはず
    GFL_ASSERT(0);
    return;
  }

  m_requestError.PushBack( error );

  GFL_PRINT(">NetError Applet RequestShow \n");
  GFL_PRINT(">         AppletType[%d]\n", error.applet_type );
  GFL_PRINT(">         AppletArg[%d]\n", error.applet_arg );
}

//------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::Update()
{
  // エラー情報がある場合は、エラーアプレット表示
  if( m_requestError.Size() > 0 )
  {

    gfl2::util::ListIterator<NijiNetworkError::ERROR_INFO_DATA> error = m_requestError.Begin();
    DispErrorApplet( *error );
    m_requestError.PopFront();
  }
}

//=============================================================================
/**
 *  privateメソッド
 */
//=============================================================================
//------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::Initialize()
{
  // メッセージデータ読み込み
  if( m_msgData == NULL )
  {
    gfl2::heap::HeapBase* pMessageHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_MESSAGE );
    System::MSGLANGID msgid = GFL_SINGLETON_INSTANCE(System::Language)->GetMsgLangId();
    m_msgData = GFL_NEW( pMessageHeap ) gfl2::str::MsgData( print::GetMessageArcId(msgid), GARC_message_message_error_DAT, pMessageHeap, gfl2::str::MsgData::LOAD_FULL );
  }

  // リストバッファく作成
  {
    gfl2::heap::HeapBase* pSysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_SYSTEM );
    m_requestError.CreateBuffer( pSysHeap, MAX_REQUEST_NUM );
  }
}

//------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::Terminate()
{
  m_requestError.Clear();
  GFL_SAFE_DELETE( m_msgData );
}

//------------------------------------------------------------------
/**
 * @brief   エラー情報でアプレット表示
 * @param   error エラー情報クラス
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::DispErrorApplet( const NijiNetworkError::ERROR_INFO_DATA& error )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* pSysHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_SYSTEM );

  nw::snd::SoundSystem::LockSoundThread(); // GFBTS2500対策のサウンドスレッド一時停止

  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) != NULL ) 
  {
    // NexDispatch処理用スレッドを開始
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->StartDispatchAsync( pSysHeap );
  }

  GFL_PRINT("DispErrorApplet Start…\n");

  NijiNetworkErrorDialogStatus::NetworkErrorDialogTrigger();

  // エラーによってアプレット表示が違う
  if( error.applet_type == NijiNetworkError::APPLET_TYPE_MESSAGE )
  {
    DispErrorAppletMessage( error.applet_arg );
  }
  else
  {
    DispErrorAppletErrorCode( error.applet_arg );
  }

  GFL_PRINT("DispErrorApplet End\n");

  if( GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager) != NULL ) 
  {
    // NexDispatch処理用スレッドを停止
    GFL_SINGLETON_INSTANCE(gflnet2::nex::NexManager)->EndDispatchAsync();
  }

  nw::snd::SoundSystem::UnlockSoundThread();// GFBTS2500対策のサウンドスレッド一時停止解除
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
 * @brief   エラーコードを指定してアプレット表示
 * @param   errorCode エラーコード
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::DispErrorAppletMessage( const int messageId )
{
#if defined(GF_PLATFORM_CTR)
  gfl2::heap::HeapBase* pMessageHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_ERROR_APPLET_MESSAGE );

  gfl2::str::StrBuf buf( ERROR_TEXT_SIZE_MAX, pMessageHeap );
  m_msgData->GetString( messageId, buf );

  NijiNetworkErreulaApplet::ErrorDisp( &buf );
#endif // GF_PLATFORM_CTR
}

//------------------------------------------------------------------
/**
 * @brief   エラーメッセージのIDを指定してアプレット表示
 * @param   messageId メッセージID
 */
//------------------------------------------------------------------
void NijiNetworkErrorDialog::DispErrorAppletErrorCode( const u32 errorCode )
{
#if defined(GF_PLATFORM_CTR)
  NijiNetworkErreulaApplet::ErrorDisp( errorCode, nn::erreula::ERROR_TYPE_ERROR_CODE );
#endif // GF_PLATFORM_CTR
}

GFL_NAMESPACE_END(Error)
GFL_NAMESPACE_END(NetLib)
