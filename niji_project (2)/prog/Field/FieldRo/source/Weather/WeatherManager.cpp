//======================================================================
/**
 * @file WeatherManager.cpp
 * @date 2015/10/20 16:32:16
 * @author saito_nozomu
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include "Field/FieldRo/include/Weather/WeatherManager.h"
#include "Field/FieldRo/include/Weather/WeatherBase.h"

#include "Field/FieldRo/include/Weather/Rain.h"
#include "Field/FieldRo/include/Weather/Snow.h"
#include "Field/FieldRo/include/Weather/SandStorm.h"
#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRain.h"

#include "Field/FieldRo/include/Collision/BaseCollisionScene.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN(weather)


void WeatherManager::Initialize(InitParam& param)
{
  m_heap = param.pHeap;
  m_pEffectManager = param.pEffectManager;
  m_pCameraManager = param.pCameraManager;
  m_pCollisionSceneContainer = param.pCollisionSceneContainer;
  m_pWeather = NULL;
  m_pFadeoutWeather = NULL;
  m_seq = SEQ_IDLE;
  m_ReserveWeatherType = WEATHER_NONE;
  {
    m_pRain = GFL_NEW( m_heap) Rain(m_heap);
    weather::Rain::InitParam rain_param;
    rain_param.pEffectManager = m_pEffectManager;
    rain_param.pCameraManager = m_pCameraManager;
    rain_param.pCollisionSceneContainer = m_pCollisionSceneContainer;
    m_pRain->Initialize(rain_param);

    m_pSnow = GFL_NEW( m_heap) Snow(m_heap);
    weather::Snow::InitParam snow_param;
    snow_param.pEffectManager = m_pEffectManager;
    snow_param.pCameraManager = m_pCameraManager;
    snow_param.pCollisionSceneContainer = m_pCollisionSceneContainer;
    m_pSnow->Initialize(snow_param);

    m_pSandStorm = GFL_NEW( m_heap) SandStorm(m_heap);
    weather::SandStorm::InitParam sand_param;
    sand_param.pEffectManager = m_pEffectManager;
    sand_param.pCameraManager = m_pCameraManager;
    m_pSandStorm->Initialize(sand_param);
  }
}

void WeatherManager::Terminate(void)
{
  Sound::StopPlayer( PLAYER_SE_WEATHER, SE_FADE_FRAME );
  if(m_pRain != NULL) GFL_SAFE_DELETE( m_pRain );
  if(m_pSnow != NULL) GFL_SAFE_DELETE( m_pSnow );
  if(m_pSandStorm != NULL) GFL_SAFE_DELETE( m_pSandStorm );
}

void WeatherManager::Start(WEATHER_TYPE type)
{
  if(m_seq == SEQ_IDLE)
  {
    if (type == WEATHER_NONE) return;
    WEATHER_CATEGORY category = GetWeatherCategory(type);
    switch(category)
    {
    case CATEGORY_RAIN:
      m_pRain->SetupRequest(type);
      m_pWeather = m_pRain;
      break;
    case CATEGORY_SNOW:
      m_pSnow->SetupRequest(type);
      m_pWeather = m_pSnow;
      break;
    case CATEGORY_SANDSTORM:
      m_pSandStorm->SetupRequest(type);
      m_pWeather = m_pSandStorm;
      break;
    default:
      GFL_ASSERT_MSG(0,"WeatherCategoryError %d",category);
      return;
    }

    m_seq = SEQ_SETUP_WAIT;
  }
}

void WeatherManager::End(void)
{
  if (m_seq == SEQ_EXECUTE)
  {
    if (m_pWeather != NULL)
    {
      m_pFadeoutWeather = m_pWeather;
      m_pWeather->StopRequest();
      m_pWeather = NULL;
      m_ReserveWeatherType = WEATHER_NONE;
    }
  }
}

/**
 * @brief アップデート処理
 * @detail 毎シンクコールする
 */
void WeatherManager::Update(void)
{

  if ( !CheckExecute() ) return;

  switch(m_seq){
  case SEQ_IDLE:
    if(m_ReserveWeatherType != WEATHER_NONE)
    {
      Start(m_ReserveWeatherType);
    }
    break;
  case SEQ_SETUP_WAIT:
    if ( m_pWeather->IsFinishedSetup() )
    {
      m_pWeather->StartRequest();
      m_seq = SEQ_EXECUTE;
    }
    break;
  case SEQ_EXECUTE:
    if (m_pFadeoutWeather != NULL)
    {
      m_pFadeoutWeather->Update();
      if (m_pFadeoutWeather->IsFinishedFadeout())
      {
        m_pFadeoutWeather = NULL;
        m_seq = SEQ_IDLE;
      }
    }
    break;
  }

  if (m_pWeather != NULL) m_pWeather->Update();

}

/**
 * @brief 稼動している天候があるかチェック
 * @return tureであるfalseでない
 */
bool WeatherManager::CheckExecute(void)
{
  if ( (m_pWeather == NULL) &&
       (m_pFadeoutWeather == NULL) &&
       (m_ReserveWeatherType == WEATHER_NONE)
       ) return false;
  return true;
}

/**
 * @brief 天候変更
 * @param type 天候種類
 */
void WeatherManager::Change(WEATHER_TYPE type)
{
  //現在の天候タイプと比較
  WEATHER_TYPE now_type = m_pWeather->GetType();
  if ( now_type != type)
  {
    WEATHER_CATEGORY now_category = GetWeatherCategory(now_type);
    WEATHER_CATEGORY category = GetWeatherCategory(type);
    //カテゴリを調べる
    if( now_category != category ) //カテゴリ違う
    {
      //一端終わらせる
      End();
      //終わらせたあと、タイプを予約
      m_ReserveWeatherType = type;
    }
    else                                      //カテゴリ同じ
    {
      //天候モジュールは変更せずに、モジュール内部で変更
      m_pWeather->ChangeWeatherRequest(type);
    }
  }

}

/**
 * @brief 現在の天候取得
 * @return WEATHER_TYPE 天候種類
 */
WEATHER_TYPE WeatherManager::GetNowWeather(void)
{
  WEATHER_TYPE now_type = WEATHER_NONE;
  if (m_pWeather != NULL) now_type = m_pWeather->GetType();
  return now_type;
}

/**
 * @brief 天候実行
 * @param type 天候種類
 */
void WeatherManager::Execute(WEATHER_TYPE type)
{
  if ( CheckExecute() )
  {
    Change(type);
  }
  else
  {
    Start(type);
  }
}

/**
 * @brief 天候が変化中か？
 * @return tureで変更中
 */
 bool WeatherManager::IsChanging(void)
 {
   if ( !CheckExecute() ) return false;
   if ( m_seq == SEQ_IDLE ) return false;
   if ( (m_seq == SEQ_EXECUTE) && (m_pFadeoutWeather==NULL) ) return false;

   return true; //変化中
 }

/**
 * @brief 天候タイプから天候カテゴリを取得
 * @param type 天候タイプ
 * @return WEATHER_CATEGORY 天候カテゴリ
 */
WEATHER_CATEGORY WeatherManager::GetWeatherCategory(WEATHER_TYPE type)
{
  const WEATHER_CATEGORY category[WEATHER_MAX] = {
      CATEGORY_NONE,
      CATEGORY_RAIN,
      CATEGORY_RAIN,
      CATEGORY_RAIN,
      CATEGORY_SNOW,
      CATEGORY_SNOW,
      CATEGORY_SNOW,
      CATEGORY_SANDSTORM,
    };

  if (type < WEATHER_MAX)
  {
    return category[type];
  }

  GFL_ASSERT_MSG(0, "WeatherTypeError %d",type);

  return CATEGORY_NONE;

}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)