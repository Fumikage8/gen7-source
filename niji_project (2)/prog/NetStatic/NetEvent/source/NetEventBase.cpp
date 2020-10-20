//==============================================================================
/**
 * @file	NetEventBase.cpp
 * @brief	通信Eventの基底クラス
 * @author	ichiraku_katsuhiko
 * @date	2015/05/18 17:50:30
 */
// =============================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameProcManager.h"
#include "NetStatic/NetLib/include/Error/NijiNetworkErrorManager.h"

// mine
#include "NetStatic/NetEvent/include/NetEventBase.h"

GFL_NAMESPACE_BEGIN(NetEvent)


//------------------------------------------------------------------
/**
* @brief コンストラクタ
*
* @param heap    workをとるためのメモリクラス
*/
//------------------------------------------------------------------
NetEventBase::NetEventBase( gfl2::heap::HeapBase* heap  ) :
   GameSys::GameEvent(heap)
  ,m_isErrorIgnore(false)
{
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) != NULL )
  {
    // 通信エラーリスナー登録
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->RegistListener( this );
  }
}

// デストラクタ
NetEventBase::~NetEventBase()
{
  if( GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager) != NULL )
  {
    // 通信エラーリスナー解除
    GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->UnregistListener();
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
bool NetEventBase::IsError() const
{
  return GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsError();
}

/* -------------------------------------------------------------------------*/
/**
 * @brief Fatalエラーが発生したかどうか
 *
 * @retval true：発生した
 * @retval false：発生してない
 */
/* -------------------------------------------------------------------------*/
bool NetEventBase::IsFatalError() const
{
  return GFL_SINGLETON_INSTANCE(NetLib::Error::NijiNetworkErrorManager)->IsFatalError();
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 通信エラーが発生しても無視する。（ =アプリ終了させない）
 *
 * @li    使用は許可制です。ichiraku 2015/12/28
 * 
 *        許可済みEvent 
 *        ・BoxEvent　（PCからの起動は、通信エラーで終わらない）
 */
/* -------------------------------------------------------------------------*/
void NetEventBase::SetErrorIgnore()
{
  GFL_PRINT("NetEventBase::SetErrorIgnore \n");
  m_isErrorIgnore = true;
}

//------------------------------------------------------------------
/**
* @brief 通信エラー通知
*/
//------------------------------------------------------------------
void NetEventBase::OnNijiNetworkError()
{
  // プロセス強制終了リクエストフラグを立てる
  //  →UpdateFuncが終了とみなされる。
  if( GFL_SINGLETON_INSTANCE(GameSys::GameManager) != NULL )
  {
    if( m_isErrorIgnore == true )
    {
      GFL_PRINT(">NetError ErrorIgnore!!! \n");
      return;
    }

    GFL_ASSERT_MSG( GFL_SINGLETON_INSTANCE(GameSys::GameManager)->IsProcessExists(),"プロセルないのに強制終了している");
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager()->RequestForceEnd();
    GFL_PRINT(">NetError RequestForceEnd!!! \n");
  }
  
  GFL_PRINT("NetEventBase::OnNijiNetworkError\n");
}

GFL_NAMESPACE_END(NetEvent)
