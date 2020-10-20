//======================================================================
/**
 * @file FieldDebugWeather.cpp
 * @date 2015/10/26 16:49:59
 * @author saito_nozomu
 * @brief 天候デバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include "Field/FieldRo/include/Debug/FieldDebugWeather.h"

// GameSys
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Local Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
//

static void ExecuteWeather(void *userWork, Field::weather::WeatherKind kind)
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  gfl2::ui::DeviceManager* pDeviceManager = pGameManager->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

  if( !pButton->IsTrigger( gfl2::ui::BUTTON_A ) ) return;
  
  DebugWeather *pDebug = reinterpret_cast<DebugWeather*>( userWork );
  pDebug->Execute(kind);
}

static void ExecuteSunny( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::SUNNY);
}

static void ExecuteCloudiness( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::CLOUDINESS);
}

static void ExecuteDry( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::DRY);
}

static void ExecuteSunShower( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::SUNSHOWER);
}

static void ExecuteRain( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::RAIN);
}

static void ExecuteMist( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::MIST);
}

static void ExecuteThunder( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::THUNDERSTORM);
}

static void ExecuteSnow( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::SNOW);
}

static void ExecuteSnowStorm( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::SNOWSTORM);
}

static void ExecuteDiamondDust( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::DIAMONDDUST);
}

static void ExecuteSandStorm( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::SANDSTORM);
}

static void ExecuteReset( void *userWork, gfl2::debug::DebugWinItem *item )
{
  ExecuteWeather(userWork, Field::weather::FORCE_WEATHER_NONE);
}

/**
 *  @brief  Group ctor
 */
static void CreateDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  DebugWeather *pDebug = reinterpret_cast<DebugWeather*>( userWork );
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pDebug && pGroup )
  {
    gfl2::heap::HeapBase *pHeap = pDebug->GetHeap();
    gfl2::str::MsgData *pMsgData = pDebug->GetMessageData();
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_sunny,       pDebug, ExecuteSunny,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_cloudiness,  pDebug, ExecuteCloudiness,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_dry,         pDebug, ExecuteDry,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_sunshower,   pDebug, ExecuteSunShower,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_rain,        pDebug, ExecuteRain,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_mist,        pDebug, ExecuteMist,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_thunder,     pDebug, ExecuteThunder,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_snow,        pDebug, ExecuteSnow,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_snowstorm,   pDebug, ExecuteSnowStorm,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_diamonddust, pDebug, ExecuteDiamondDust,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_sandstorm,   pDebug, ExecuteSandStorm,   NULL );
    gfl2::debug::DebugWin_AddItemUpdateFunc( pGroup, pHeap, pMsgData, msg_dmenu_field_weather_reset,       pDebug, ExecuteReset,   NULL );
  }
}
/**
 *  @brief  Group dtor
 */
static void DestroyDebugGroupFunc( void *userWork, gfl2::debug::DebugWinItem *item )
{
  gfl2::debug::DebugWinGroup *pGroup = reinterpret_cast<gfl2::debug::DebugWinGroup*>( item );
  if( pGroup )
  {
    pGroup->RemoveAllItem();
  }
}

/**
 *  @brief  初期化
 */
void DebugWeather::Initialize( gfl2::heap::HeapBase *pHeap, Field::weather::WeatherControl *pWeatherControl, gfl2::str::MsgData* pMsgData )
{
  m_pHeap = pHeap;
  m_pMsgData = pMsgData;
  m_pWeatherControl = pWeatherControl;
}

/**
 *  @brief  メニューの作成
 */
void DebugWeather::CreateDebugMenu( gfl2::debug::DebugWinGroup *root )
{
  gfl2::debug::DebugWinGroup *pGroup = gfl2::debug::DebugWin_AddGroup( m_pHeap, m_pMsgData, msg_dmenu_field_weather, root );
  pGroup->SetCreateDestroyGroupFunc( this, CreateDebugGroupFunc, DestroyDebugGroupFunc );
}


/**
 *  @brief  天候の実行
 *  @param 天候の種類
 */
void DebugWeather::Execute( Field::weather::WeatherKind kind )
{
  GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
  pGameManager->GetGameData()->SetDebugWeather(kind);
  m_pWeatherControl->ChangeWeatherRequest();
}

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)
#endif //PM_DEBUG