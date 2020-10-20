//==============================================================================
/**
 * @file    DeviceTurnWatcher.cpp
 * @brief   3DSが逆さまになっているかどうかの判定
 * @author  tamada
 * @date    2012.05.26
 */
//==============================================================================

// module
#include "System/include/DeviceTurnWatcher.h"


GFL_NAMESPACE_BEGIN(System)

//======================================================================
//======================================================================

  ///さかさ判定値
  static const f32 DEVICE_TURN_Z_VALUE = 0.5f;

  static const u32 DEVICE_TURN_COUNT_NORMAL = 180;  ///<計測モード：通常の判定時間
  static const u32 DEVICE_TURN_COUNT_SHORT  = 60;   ///<計測モード：短縮の判定時間

#if PM_DEBUG
//------------------------------------------------------------------------
//------------------------------------------------------------------------
  static void putLog( const wchar_t * str )
  {
/*
    xy_debug::LogWindow_AddMsg(xy_debug::LOGWIN_ID_SAKASA_SHINKA, str );
    char buf[ 128 ];
    ::std::wcstombs( buf, str, sizeof(buf) );
    GFL_PRINT( buf );
*/
  }
#endif

//------------------------------------------------------------------------
//------------------------------------------------------------------------
DeviceTurnWatcher::DeviceTurnWatcher( gfl2::heap::HeapBase * heap, gfl2::ui::DeviceManager * device_manager )
  : m_device_manager(device_manager)
  , m_is_turned( false )
  , m_count ( 0 )
  , m_rate( COUNT_RATE_30F )
{
#if defined(GF_PLATFORM_WIN32)
  // win版はなにもしない
#elif defined(GF_PLATFORM_CTR)
	m_device_manager->BootAccelerometer( heap, gfl2::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );
#if PM_DEBUG
/*
  //xy_debug::LogWindow_SetEnable( true );
  xy_debug::LogWindow_SetDispFlg( true, xy_debug::LOGWIN_ID_SAKASA_SHINKA );
  putLog(L"DeviceManager Create\n");
*/
#endif  // PM_DEBUG
#endif  // GF_PLATFORM_CTR
}

//------------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------------
DeviceTurnWatcher::~DeviceTurnWatcher()
{
#if defined(GF_PLATFORM_WIN32)
  // win版はなにもしない
#elif defined(GF_PLATFORM_CTR)
	m_device_manager->StopAccelerometer( gfl2::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );
#if PM_DEBUG
/*
  putLog(L"DeviceManager Delete\n");
  xy_debug::LogWindow_SetDispFlg( false, xy_debug::LOGWIN_ID_SAKASA_SHINKA );
*/
#endif  // PM_DEBUG
#endif  // GF_PLATFORM_CTR
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void DeviceTurnWatcher::Initialize( DeviceTurnWatcher::COUNT_RATE rate, DeviceTurnWatcher::COUNT_MODE mode )
{
#if defined(GF_PLATFORM_WIN32)
  // win版はなにもしない
#elif defined(GF_PLATFORM_CTR)
  m_is_turned = false;
  m_count = 0;
  m_rate = rate;
  m_mode = mode;
  if ( mode == COUNT_MODE_NORMAL )
  {
    m_count_limit = DEVICE_TURN_COUNT_NORMAL;
  } else {
    m_count_limit = DEVICE_TURN_COUNT_SHORT;
  }
#if PM_DEBUG
  putLog(L"Turn Count Initialized\n");
#endif  // PM_DEBUG
#endif  // GF_PLATFORM_CTR
}

//------------------------------------------------------------------------
/**
 * @brief 計測処理　更新
 *
 * @note  重力なみの加速度が3秒遺贈
 */
//------------------------------------------------------------------------
void DeviceTurnWatcher::Update()
{
#if defined(GF_PLATFORM_WIN32)
  // win版はなにもしない
#elif defined(GF_PLATFORM_CTR)
  gfl2::ui::Accelerometer * pAccelerometer;
  pAccelerometer = m_device_manager->GetAccelerometer( gfl2::ui::CTR_DeviceManager::ACCELEROMETER_STANDARD );

  //有効データが取得できない間は何もしない
  if (pAccelerometer->IsValid() == false ) return;

  gfl2::math::VEC3 acc;
  pAccelerometer->GetAcceleration(&acc);
  if (acc.z < DEVICE_TURN_Z_VALUE )
  {
#if PM_DEBUG
      if ( m_count != 0 ) putLog(L"Turn Count Reset\n");
#endif
    m_count = 0;
  }
  else
  {
    m_count += m_rate;
    if ( m_count > m_count_limit && m_is_turned == false )
    {
      m_is_turned = true;
#if PM_DEBUG
      putLog(L"Turn Count Reached\n");
#endif
    }
  }
#endif  // GF_PLATFORM_CTR
}

GFL_NAMESPACE_END(System)
