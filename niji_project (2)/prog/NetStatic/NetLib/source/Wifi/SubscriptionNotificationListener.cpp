#if defined(GF_PLATFORM_CTR)
//======================================================================
/**
 * @file	SubscriptionNotificationListener.cpp
 * @author	suzuki_hisanao
 */
//======================================================================
#include <NetStatic/NetLib/include/Wifi/SubscriptionNotificationListener.h>


GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(Wifi)



//--------------------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//--------------------------------------------------------------------------------------------
SubscriptionNotificationListener::SubscriptionNotificationListener( void ) : nn::nex::NotificationEventHandler(), m_eventList()
{
}


//--------------------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------------------
SubscriptionNotificationListener::~SubscriptionNotificationListener()
{
  m_eventList.clear();
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 更新イベントで取得した情報一式を引数リストに上書きする
 *        本関数実行中は、NEXのシステムスレッドはロックされる
 *
 * @param dstList 格納先イベントリスト
 *
 * @return 通知された情報が存在するならtrue、通知情報が存在しないならfalse
 */
//--------------------------------------------------------------------------------------------
qBool SubscriptionNotificationListener::CopyNotification( nn::nex::qList<nn::nex::NotificationEvent>& dstList )
{
  nn::nex::Scheduler* scheduler = nn::nex::Scheduler::GetInstance();

  if( scheduler != NULL )
  {
    // システムロック。関数終了時のデストラクタでロックが解除される
    nn::nex::ScopedCS scope(scheduler->SystemLock());

    // ハンドラで取得したリスト情報を引き渡し、自身をクリア
    dstList.clear();
    if( !m_eventList.empty() )
    {
      dstList = m_eventList;
      m_eventList.clear();

      return true;
    }
  }

  return false;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief 更新イベントで取得した情報一式を引数リストにpush_backする
 *        本関数実行中は、NEXのシステムスレッドはロックされる
 *
 * @param eventList 格納先イベントリスト
 *
 * @return 通知された情報が存在するならtrue、通知情報が存在しないならfalse
 */
//--------------------------------------------------------------------------------------------
qBool SubscriptionNotificationListener::PushbackNotification( nn::nex::qList<nn::nex::NotificationEvent>& dstList )
{
  // システムロック。関数終了時のデストラクタでロックが解除される
  nn::nex::ScopedCS scope(nn::nex::Scheduler::GetInstance()->SystemLock());

  if( !m_eventList.empty() )
  {
    for (nn::nex::qList<nn::nex::NotificationEvent>::iterator it = m_eventList.begin(); it != m_eventList.end(); ++it)
    {
      nn::nex::NotificationEvent *event = &(*it);
      dstList.push_back( *event );
    }
    m_eventList.clear();
    return true;
  }
  return false;
}




//--------------------------------------------------------------------------------------------
/**
 * @brief PSS更新イベント通知処理
 *        イベントハンドラの為、ユーザーの使用は禁止
 *
 * @param event 通知イベント
 */
//--------------------------------------------------------------------------------------------
// @override
void SubscriptionNotificationListener::ProcessNotificationEvent( const nn::nex::NotificationEvent &event )
{
  m_eventList.push_back(event);

  KOUSAKA_PRINT( "SubscriptionNotificationListener::ProcessNotificationEvent[%u][%u]\n", event.GetSource(), event.GetType() );
}


GFL_NAMESPACE_END(Wifi)
GFL_NAMESPACE_END(NetLib)

#endif