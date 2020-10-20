//=========================================================================
/**
 * @file  SystemEventManager.cpp
 * @brief 本体システムイベント通知リスナを管理するマネージャ
 */
//=========================================================================

#include <System/include/SystemEventManager.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <types/include/gfl2_Typedef.h>

//-----------------------------------------------------------------------------
/**
*    Singletonインスタンスの実体を定義
*/
//-----------------------------------------------------------------------------
SINGLETON_INSTANCE_DEFINITION(System::SystemEventManager);
template class gfl2::base::SingletonAccessor<System::SystemEventManager>;

namespace System{

//----------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 */
//-----------------------------------------------------------------------------
SystemEventManager::SystemEventManager( void )
{
  ::std::memset( m_event, 0, sizeof(m_event) );
  ::std::memset( m_reservedApplet, 0, sizeof(m_reservedApplet) );
  m_criticalSection.Initialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//-----------------------------------------------------------------------------
SystemEventManager::~SystemEventManager( void )
{
  m_criticalSection.Finalize();
}


//-----------------------------------------------------------------------------
/**
 * @brief アプレット起動を予約する
 *
 * @note 実行可能なタイミングでSystemEventListener::OnCallApplet()がコールされる
 *
 * @param applet 予約したいアプレット
 */
//-----------------------------------------------------------------------------
void SystemEventManager::ReserveAppletCalling( RESERVE_APPLET applet )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  if( applet != E_RESERVE_APPLET_MAX ){
    m_reservedApplet[applet] = true;
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief システムイベントリスナを登録
 *
 * @param event 登録するイベントリスナ
 */
//-----------------------------------------------------------------------------
void SystemEventManager::RegistSystemEventListener( SystemEventListener* event )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] == event ){
      return; // 登録済み
    }
  }
  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] == NULL )
    {
      m_event[i] = event;
      return;
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * @brief システムイベントリスナを解除
 *
 * @param event 解除するイベントリスナ
 */
//-----------------------------------------------------------------------------
void SystemEventManager::RemoveSystemEventListener( SystemEventListener* event )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] == event )
    {
      m_event[i] = NULL;
      return;
    }
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  スリープ遷移時イベントをコール
 *
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
 */
//-----------------------------------------------------------------------------
void SystemEventManager::CallSuspendWithSleep( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      m_event[i]->OnSuspendWithSleep();
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スリープからの復帰時イベントをコール
 *
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
 */
//-----------------------------------------------------------------------------
void SystemEventManager::CallResumeWithSleep( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      m_event[i]->OnResumeWithSleep();
    }
  }
}


//----------------------------------------------------------------------------
/**
 *  @brief  ホームメニュー遷移時イベントをコール
 *
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
 */
//-----------------------------------------------------------------------------
void SystemEventManager::CallSuspendWithHomeMenu( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      m_event[i]->OnSuspendWithHomeMenu();
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ホームメニューからの復帰時イベントをコール
 *
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
 */
//-----------------------------------------------------------------------------
void SystemEventManager::CallResumeWithHomeMenu( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      m_event[i]->OnResumeWithHomeMenu();
    }
  }
}



//----------------------------------------------------------------------------
/**
 *  @brief  システムアプレット起動許可通知イベントをコール
 *
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
 */
//-----------------------------------------------------------------------------
void SystemEventManager::CallSystemApplet( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 applet = 0; applet < (u32)(E_RESERVE_APPLET_MAX); applet++ )
  {
    if( m_reservedApplet[applet] )
    {
      for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
      {
        if( m_event[i] != NULL ){
          m_event[i]->OnCallApplet( static_cast<RESERVE_APPLET>(applet) );
        }
      }
      m_reservedApplet[applet] = false;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  システムアプレット予約があるかどうか
 *  @return ある場合TRUE
 */
//-----------------------------------------------------------------------------
bool SystemEventManager::IsReservedApplet(void)
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御
  
  for( u32 applet = 0; applet < (u32)(E_RESERVE_APPLET_MAX); applet++ ){
    if( m_reservedApplet[applet] ){
      return true;
    }
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    終了発生時のイベントをコール
 *  @retval   true  全処理終了
 *  @retval   false 終了処理継続中
 */
//-----------------------------------------------------------------------------
bool SystemEventManager::CallCloseApplication( void )
{
  bool result = true;

  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      if( m_event[i]->OnCloseApplication() == false ){
        result = false;
      }
    }
  }
  return result;
}

//-----------------------------------------------------------------------------
/**
 *  @brief    ホーム、スリープ、電源OFFのイベントをコール
 *  @retval   true  全処理終了
 *  @retval   false 終了処理継続中
 */
//-----------------------------------------------------------------------------
bool SystemEventManager::CallSuspendWithHomeSleepAndPower( void )
{
  bool result = true;

  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      if( m_event[i]->OnSuspendWithHomeSleepAndPower() == false ){
        result = false;
      }
    }
  }
  return result;
}

//-----------------------------------------------------------------------------
/**
*  @brief    蓋とじイベントをコール（スリープ許可禁止にかかわらず呼ばれる）
 *  @note   本関数はシステム用関数であり、ユーザーコール禁止！
*/
//-----------------------------------------------------------------------------
void SystemEventManager::CallCloseCover( void )
{
  gfl2::thread::ScopedLock lock(m_criticalSection);    // 排他制御

  for( u32 i = 0; i < EVENT_COUNT_MAX; i++ )
  {
    if( m_event[i] != NULL ){
      m_event[i]->CallCloseCover();
    }
  }
}

}

