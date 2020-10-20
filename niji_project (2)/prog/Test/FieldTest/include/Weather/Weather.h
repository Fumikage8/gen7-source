//===================================================================
/**
 * @file    Weather.h
 * @brief   天候管理
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#if !defined( WEATHER_H_INCLUDED )
#define WEATHER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include "Test/FieldTest/include/Weather/WeatherDefine.h"


namespace gfl2 {
  namespace gfx {
    class IGLAllocator;
  }
}

namespace Field { namespace Effect {
class Manager;
}}


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )


class WeatherEffectManager;


class Weather
{
  GFL_FORBID_COPY_AND_ASSIGN(Weather); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   * @param   pSystemHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  Weather( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);
  ///< デストラクタ
  virtual ~Weather();

  /**
   * @brief   天候変更リクエスト
   * @param   requestType 変更したい天候
   * @param   changeTime 遷移にかける時間(秒)
   */
  void Request( Type requestType, f32 changeTime);

  /**
   * @brief   生成
   */
  void Create();

  /**
   * @brief   破棄
   * @retval  true  破棄完了
   * @retval  false 破棄実行中
   */
  b8 Destroy();

  /**
   * @brief   初期化
   */
  void Initialzie();

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   描画
   */
  void Draw();

  /**
   * @brief   天候が有効か
   * @retval  true 有効
   * @retval  false 無効
   */
  b8 IsEnable();

  /**
   * @brief   現在の天候の種類を取得
   * @return  天候の種類 WeatherDefine.h Type
   */
  Type GetNowWeatherType();

  /**
   * @brief   前の天候の種類を取得
   * @return  天候の種類 WeatherDefine.h Type
   */
  Type GetOldWeatherType();

  /**
   * @brief   天候エフェクト管理クラスの取得
   * @return  天候エフェクト管理クラス
   */
  WeatherEffectManager* GetWeatherEffectManager() { return m_pWeatherEffectManager; }

private:

  Type        m_nowWeatherType;    ///< 現在の天候の種類
  Type        m_oldWeatherType;    ///< 前の天候の種類

  gfl2::heap::HeapBase*     m_pHeap;                      ///< インスタンス確保などで使用
  gfl2::gfx::IGLAllocator*  m_pGLAllocator;               ///< リソース用アロケータ

  Field::Effect::Manager*   m_pEffectManager;             ///< フィールドエフェクト管理
  WeatherEffectManager*     m_pWeatherEffectManager;      ///< 天候エフェクト管理

};

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

#endif // WEATHER_H_INCLUDED

