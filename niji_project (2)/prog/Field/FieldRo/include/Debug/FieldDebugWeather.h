//======================================================================
/**
 * @file FieldDebugWeather.h
 * @date 2015/10/26 16:45:33
 * @author saito_nozomu
 * @brief 天候デバッグ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DEBUG_WEATHER_H_INCLUDED__
#define __FIELD_DEBUG_WEATHER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"

#if PM_DEBUG

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinGroup;
GFL_NAMESPACE_END(debug)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)
class WeatherControl;
GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_BEGIN(Debug)



class DebugWeather
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugWeather);
public:
  /**
   *  @brief  コンストラクタ
   */
  DebugWeather( void ){}

  /**
   *  @brief  デストラクタ
   */
  ~DebugWeather( void ){}

  /**
   *  @brief  初期化
   */
  void Initialize( gfl2::heap::HeapBase *pHeap, Field::weather::WeatherControl *pWeatherControl, gfl2::str::MsgData* pMsgData );

  /**
   *  @brief  メニューの作成
   */
  void CreateDebugMenu( gfl2::debug::DebugWinGroup *root );

  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }
  inline  gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  void Execute(Field::weather::WeatherKind kind);

private:
  gfl2::heap::HeapBase          *m_pHeap;
  gfl2::str::MsgData*           m_pMsgData;
  Field::weather::WeatherControl *m_pWeatherControl;
};


GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif //PM_DEBUG

#endif // __FIELD_DEBUG_WEATHER_H_INCLUDED__