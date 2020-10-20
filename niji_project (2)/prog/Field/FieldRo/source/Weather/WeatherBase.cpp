//======================================================================
/**
 * @file WeatherBase.cpp
 * @date 2015/10/20 16:33:38
 * @author saito_nozomu
 * @brief 天候クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include "Field/FieldRo/include/Weather/WeatherBase.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

WeatherBase::WeatherBase()
{
  m_Seq = SEQ_IDLE;
  m_SetupFinish = false;
  m_Stop = true;
  m_Fadeout = true;
  m_Type = WEATHER_NONE;
  m_StartedSE = false;
  m_FadeOutSeq = 0;
}

WeatherBase::~WeatherBase()
{
}

void WeatherBase::SetupRequest(WEATHER_TYPE type)
{
  m_FadeOutSeq = 0;
  m_Type = type;
  m_SetupFinish = false;
  m_Seq = SEQ_SETUP;
}

void WeatherBase::StartRequest(void)
{
  StartFunc();
  m_Stop = false;
  m_Fadeout = false;
  m_Seq = SEQ_MAIN;
  m_StartedSE = false;
  //フェードインさせる
  ChangeWeatherRequestCore(m_Type);
}

void WeatherBase::StopRequest(void)
{
  m_FadeOutSeq = 0;
  StopFunc();
  m_Stop = true;
  m_Seq = SEQ_FADEOUT;
  Sound::StopPlayer( PLAYER_SE_WEATHER, SE_FADE_FRAME );
  m_StartedSE = true;//一応、すでに鳴らしたことにする
}

void WeatherBase::Update(void)
{
  switch(m_Seq){
  case SEQ_IDLE:
    break;
  case SEQ_SETUP:
    if( SetupFunc() )
    {
      m_SetupFinish = true;
      m_Seq = SEQ_IDLE;
    }
    break;
  case SEQ_MAIN:
    if (!m_StartedSE)
    {
      if (Sound::GetPlayingCount( PLAYER_SE_WEATHER ) == 0)
      {
        StartSE();
        m_StartedSE = true;
      }
    }
    MainFunc();
    break;
  case SEQ_FADEOUT:
    MainFunc();
    if ( FadeOutFunc() )
    {
      m_FadeOutSeq = 0;
      m_Fadeout = true;
      m_Seq = SEQ_IDLE;
    }
    break;
  }
}

/**
 * @brief 天候の変更要請
 * @param[in] type 天候種類
 */
void WeatherBase::ChangeWeatherRequest(WEATHER_TYPE type)
{
  Sound::StopPlayer( PLAYER_SE_WEATHER, SE_FADE_FRAME );
  m_StartedSE = false;//天候変更に備え、ＳＥ再生履歴をクリア
  ChangeWeatherRequestCore(type);
}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)