//===================================================================
/**
 * @file    WeatherEffectManager.h
 * @brief   天候エフェクト管理
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#if !defined( WEATHER_EFFECT_MANAGER_H_INCLUDED )
#define WEATHER_EFFECT_MANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include "Test/FieldTest/include/Weather/WeatherDefine.h"


namespace gfl2 { namespace gfx {
class IGLAllocator;
}}

namespace Field { namespace Effect {
class Manager;
}}


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( FieldTest )
class MyRenderingPipeLine;
GFL_NAMESPACE_END( FieldTest )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )


class WeatherEffectBase;


class WeatherEffectManager
{
  GFL_FORBID_COPY_AND_ASSIGN(WeatherEffectManager); //コピーコンストラクタ＋代入禁止

public:

  ///< 設定情報
  struct Description
  {
    // @todo 変わる
    f32       fps;
  };

  /**
   * @brief   コンストラクタ
   * @param   pSystemHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  WeatherEffectManager( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);
  ///< デストラクタ
  virtual ~WeatherEffectManager();

  /**
   * @brief   エフェクト変更リクエスト
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
   * @param   desc 設定情報
   */
  void Initialzie( const Description& desc);

  /**
   * @brief   更新
   */
  void Update();

  /**
   * @brief   描画
   */
  void Draw();

  /**
   * @brief   カメラ更新後に行う更新
   * @param   cameraPos カメラ位置
   */
  void UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos);

  /**
   * @brief   自機位置の更新
   * @param   playerPos
   * @todo 仮対応
   */
  void UpdatePlayerPos( gfl2::math::Vector3& playerPos);


  void SetRenderingPipeLine( Test::FieldTest::MyRenderingPipeLine* pRenderingPipeLine) { m_pRenderingPipeLine = pRenderingPipeLine; }

private:

  ///< エフェクトの状態
  enum EffectState
  {
    STATE_NONE,               // なし
    STATE_INITIALIZE,         // 初期
    STATE_LOAD,               // 読み込み
    STATE_UPDATE_IN,          // 開始演出
    STATE_UPDATE_MAIN,        // 演出メイン
    STATE_UPDATE_OUT,         // 終了演出
    STATE_TERMINATE,          // 破棄
    STATE_RESTART,            // 再開

    STATE_MAX,
  };

  void UpdateRequest();
  void UpdateEffect();

  // 更新処理テーブル
  typedef EffectState (WeatherEffectManager::*UpdateFunc)( Type type);
  static const UpdateFunc m_UpdateFunc[ STATE_MAX ];

  // 更新処理
  EffectState UpdateFunc_None       ( Type type);   // なし
  EffectState UpdateFunc_Initialize ( Type type);   // 初期化
  EffectState UpdateFunc_Load       ( Type type);   // 読み込み
  EffectState UpdateFunc_UpdateIn   ( Type type);   // 開始演出
  EffectState UpdateFunc_UpdateMain ( Type type);   // 演出メイン
  EffectState UpdateFunc_UpdateOut  ( Type type);   // 終了演出
  EffectState UpdateFunc_Terminate  ( Type type);   // 破棄
  EffectState UpdateFunc_Restart    ( Type type);   // 再開

  void SwitchEffectTypeAndState();
  b8 IsSwitchEffect();

  b8 IsEffectFinishInitialize( Type type, EffectState state);

  void SetNowWeatherType( Type type);
  void SetNextWeatherType( Type type);
  void SetNowEffectState( EffectState state);
  void SetNextEffectState( EffectState state);

  f32 CalculateIntervalTimeFrameTime();

private:

  u8                    m_seq;

  f32                   m_fps;

  Type                  m_nowWeatherType;
  Type                  m_nextWeatherType;

  EffectState           m_nowEffectState;
  EffectState           m_nextEffectState;

  Type                  m_requestType;
  f32                   m_changeTime;

  WeatherEffectBase*    m_pEffect[ TYPE_MAX ];

  gfl2::heap::HeapBase*     m_pHeap;                      ///< インスタンス確保などで使用
  gfl2::gfx::IGLAllocator*  m_pGLAllocator;               ///< リソース用アロケータ

  Field::Effect::Manager*   m_pEffectManager;             ///< フィールドエフェクト管理

  Test::FieldTest::MyRenderingPipeLine*      m_pRenderingPipeLine;
};

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

#endif // WEATHER_EFFECT_MANAGER_H_INCLUDED

