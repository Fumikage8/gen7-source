//===================================================================
/**
 * @file    WeatherEffectBase.h
 * @brief   天候エフェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#if !defined( WEATHER_EFFECT_BASE_H_INCLUDED )
#define WEATHER_EFFECT_BASE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

#include <Effect/include/gfl2_EffectHandle.h>

namespace gfl2
{
  namespace renderingengine { namespace scenegraph
  {
    namespace resource
    {
      class ResourceNode;
    }
    namespace instance
    {
      class ModelInstanceNode;
    }
  }}

  namespace animation {
    class AnimationController;
  }

  namespace gfx {
    class IGLAllocator;
  }
}

namespace Field { namespace Effect {
class Manager;
class Model;
}}


// @todo デバッグフラグにしたい
//#define UNDER_RESOURCE_PARTICLE


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )


class WeatherEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(WeatherEffectBase); //コピーコンストラクタ＋代入禁止

public:

  static const u32 EFFECT_UNDER_MAX = 8;

  ///< 設定情報
  struct Description
  {
    // @todo 変わる
    gfl2::heap::HeapBase*     pSystemHeap;          // インスタンス確保などに使用するヒープ
    gfl2::gfx::IGLAllocator*  pGLAllocator;         // リソース確保用アロケータ
    c16*                      model_path;           // モデルリソースパス
    c16**                     shader_path_table;    // シェーダーリソースパス
    u32                       shader_num;           // シェーダーリソース数
    c16**                     texture_path_table;   // テクスチャリソースパス
    u32                       texture_num;          // テクスチャリソース数
  };

  /**
   * @brief   コンストラクタ
   * @param   pSystemHeap インスタンス生成などに使用するヒープ
   * @param   pGLAllocator リソース確保用アロケータ
   * @param   pEffectManager フィールドエフェクト管理クラス
   */
  WeatherEffectBase( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager);
  ///< デストラクタ
  virtual ~WeatherEffectBase();

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
  virtual void SetAlpha( f32 alpha, f32 changeFrame);

  /**
   * @brief   カメラ更新後に行う更新
   * @param   cameraPos カメラ位置
   */
  virtual void UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos);


  /**
   * @brief   自機位置の更新
   * @param   playerPos
   * @todo 仮対応
   */
  void UpdatePlayerPos( gfl2::math::Vector3& playerPos) { m_playerPos = playerPos; }

  /**
   * @brief   エフェクトモデルインスタンスの取得
   * @param   idx モデル番号。現状はEFFECT_UNDER_MAX
   */
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetEffectModelInstanceNode( u32 idx);

protected:

  gfl2::heap::HeapBase*     m_pSystemHeap;
  gfl2::gfx::IGLAllocator*  m_pGLAllocator;

  Field::Effect::Manager*   m_pEffectManager;

  gfl2::Effect::Handle      m_eftHandleFall;
  gfl2::Effect::Handle      m_eftHandleUnder[ EFFECT_UNDER_MAX ];

  Field::Effect::Model*     m_pEffectModel[ EFFECT_UNDER_MAX ];

  u8                    m_seq;
  f32                   m_timeCnt;
  gfl2::math::Vector3   m_playerPos;



  void SetEmissionRatioEmitterAll( gfl2::Effect::Handle& handle, f32 value);
  void SetEmissionIntervalEmitterAll( gfl2::Effect::Handle& handle, f32 value);


  enum ResourceSwitch
  {
    RESOURCE_SWITCH_NONE,
    RESOURCE_SWITCH_PARTICLE,
    RESOURCE_SWITCH_MODEL,
  };
  s32 m_resourceSwitch;

#if PM_DEBUG
  void UpdateResourceSwitch();
  void SetStopCalcAndDraw_FallEmitterSet( bool isStopCalcAndDraw);
  void SetStopCalcAndDraw_UnderEmitterSetAll( bool isStopCalcAndDraw);
  void SetVisible_EffectModelAll( bool isVisible);
#endif // PM_DEBUG
};

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

#endif // WEATHER_EFFECT_BASE_H_INCLUDED

