//===================================================================
/**
 * @file    WeatherEffectRain.h
 * @brief   天候エフェクト雨
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#if !defined( WEATHER_EFFECT_RAIN_H_INCLUDED )
#define WEATHER_EFFECT_RAIN_H_INCLUDED
#pragma once

#include "Test/FieldTest/include/Weather/WeatherEffectBase.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )

class WeatherEffectRain : public WeatherEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(WeatherEffectRain); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   コンストラクタ
   * @param   pSystemHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  WeatherEffectRain( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);
  ///< デストラクタ
  virtual ~WeatherEffectRain();

  /**
   * @brief   初期化
   */
  virtual void Initialize();

  /**
   * @brief   読み込み
   * @retval  true 完了
   * @retval  false 実行中
   */
  virtual b8 Load();

  /**
   * @brief   開始演出
   * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
   * @param   changeTime 切り替え時間(秒)
   * @retval  true 完了
   * @retval  false 実行中
   */
  virtual b8 UpdateIn( f32 intervalFrameTime, f32 changeTime);

  /**
   * @brief   演出メイン処理
   */
  virtual void UpdateMain();

  /**
   * @brief   終了演出
   * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
   * @param   changeTime 切り替え時間(秒)
   * @retval  true 完了
   * @retval  false 実行中
   */
  virtual b8 UpdateOut( f32 intervalFrameTime, f32 changeTime);

  /**
   * @brief   破棄
   * @retval  true  破棄完了
   * @retval  false 破棄実行中
   */
  virtual b8 Terminate();

  /**
   * @brief   再開処理
   */
  virtual void Restart();

  /**
   * @brief   描画
   */
  void Draw();

  /**
   * @brief   アルファ値設定
   * @param   alpha アルファ値(0.0〜1.0)
   * @param   changeFrame 遷移にかけるフレーム数
   */
  virtual void SetAlpha( f32 alpha, f32 change5rame);

  /**
   * @brief   カメラ更新後に行う更新
   * @param   cameraPos カメラ位置
   */
  virtual void UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos);



private:

  void UpdateCore();
  void UpdateGroundEffect();

  void CalculateGroundEffectPosition( gfl2::math::Vector3* pOutPos, const gfl2::math::Vector3& basePos, f32 rangeX, f32 rangeZ);
};

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

#endif // WEATHER_EFFECT_RAIN_H_INCLUDED

