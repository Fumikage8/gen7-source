//===================================================================
/**
 * @file    WeatherEffectShine.cpp
 * @brief   天候エフェクト晴れ
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#include "Test/FieldTest/include/Weather/WeatherEffectShine.h"
#include "Test/FieldTest/include/FieldEffectManager.h"

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )

/**
 * @brief   コンストラクタ
 * @param   pSystemHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pEffectManager フィールドエフェクト管理クラス
 */
WeatherEffectShine::WeatherEffectShine( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  WeatherEffectBase( pSystemHeap, pGLAllocator, pEffectManager)
{
}

/**
 * @brief   デストラクタ
 */
WeatherEffectShine::~WeatherEffectShine()
{
}

/**
 * @brief   初期化
 */
void WeatherEffectShine::Initialize()
{
}

/**
 * @brief   読み込み
 * @retval  true 完了
 * @retval  false 実行中
 */
b8 WeatherEffectShine::Load()
{
  return true;
}

/**
 * @brief   開始演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectShine::UpdateIn( f32 intervalFrameTime, f32 changeTime)
{
  return true;
}

/**
 * @brief   演出メイン処理
 */
void WeatherEffectShine::UpdateMain()
{
}

/**
 * @brief   終了演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectShine::UpdateOut( f32 intervalFrameTime, f32 changeTime)
{
  return true;
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 WeatherEffectShine::Terminate()
{
  return true;
}

/**
 * @brief   再開処理
 */
void WeatherEffectShine::Restart()
{
}

/**
 * @brief   描画
 */
void WeatherEffectShine::Draw()
{
}

/**
 * @brief   アルファ値設定
 * @param   alpha アルファ値(0.0〜1.0)
 * @param   change_frame 遷移にかけるフレーム数
 */
void WeatherEffectShine::SetAlpha( f32 alpha, f32 changeFrame)
{
}

/**
 * @brief   カメラ更新後に行う更新
 * @param   cameraPos カメラ位置
 */
void WeatherEffectShine::UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos)
{
}

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

