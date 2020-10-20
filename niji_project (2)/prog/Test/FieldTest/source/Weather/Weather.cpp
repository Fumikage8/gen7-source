//===================================================================
/**
 * @file    Weather.cpp
 * @brief   天候管理
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#include "Test/FieldTest/include/Weather/Weather.h"
#include "Test/FieldTest/include/Weather/WeatherEffectManager.h"
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
Weather::Weather( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  m_nowWeatherType( TYPE_NONE),
  m_oldWeatherType( TYPE_NONE),
  m_pHeap( pSystemHeap),
  m_pGLAllocator( pGLAllocator),
  m_pEffectManager( pEffectManager),
  m_pWeatherEffectManager( NULL)
{
}

/**
 * @brief   デストラクタ
 */
Weather::~Weather()
{
}

/**
 * @brief   天候変更リクエスト
 * @param   requestType 変更したい天候
 * @param   changeTime 遷移にかける時間(秒)
 */
void Weather::Request( Type requestType, f32 changeTime)
{
  m_pWeatherEffectManager->Request( requestType, changeTime);
  m_oldWeatherType = m_nowWeatherType;
  m_nowWeatherType = requestType;
}

/**
 * @brief   生成
 */
void Weather::Create()
{
  m_pWeatherEffectManager = GFL_NEW( m_pHeap) WeatherEffectManager( m_pHeap, m_pGLAllocator, m_pEffectManager);
  m_pWeatherEffectManager->Create();
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 Weather::Destroy()
{
  m_pWeatherEffectManager->Destroy();

  GFL_SAFE_DELETE( m_pWeatherEffectManager);
  return true;
}

/**
 * @brief   初期化
 */
void Weather::Initialzie()
{
}

/**
 * @brief   更新
 */
void Weather::Update()
{
  // @todo 天球は生成されているか？
  // @todo 天球は有効か？




  m_pWeatherEffectManager->Update();
}

/**
 * @brief   描画
 */
void Weather::Draw()
{
}

/**
 * @brief   天候が有効か
 * @retval  true 有効
 * @retval  false 無効
 */
b8 Weather::IsEnable()
{
  return ( m_nowWeatherType != TYPE_NONE );
}

/**
 * @brief   現在の天候の種類を取得
 */
Type Weather::GetNowWeatherType()
{
  return m_nowWeatherType;
}

/**
 * @brief   前の天候の種類を取得
 */
Type Weather::GetOldWeatherType()
{
  return m_oldWeatherType;
}

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

