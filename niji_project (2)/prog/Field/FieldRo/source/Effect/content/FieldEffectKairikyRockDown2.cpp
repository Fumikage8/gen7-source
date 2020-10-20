//======================================================================
/**
 * @file FieldEffectKairikyRockDown2.cpp
 * @date 2017/01/19 15:19:39
 * @author kawazoe_shinichi
 * @brief カイリキ岩落下エフェクト（フェローチェマップの大きい岩専用）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown2.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectKairikyRockDown2
 *  @brief  カイリキ岩落下エフェクト（フェローチェマップの大きい岩専用）
 */
/**
 *  @brief  コンストラクタ
 */
 EffectKairikyRockDown2::EffectKairikyRockDown2(void)
: IEffectBase()
, m_pEffectHeap   ( NULL )
, m_pEffectSystem ( NULL )
, m_pEffectHandle ( NULL )
, m_vPosition     (0.0f,0.0f,0.0f)
{
}

/**
 *  @brief  デストラクタ
 */
EffectKairikyRockDown2::~EffectKairikyRockDown2(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectKairikyRockDown2::Initialize(SetupData &setupData)
{
  m_vPosition = setupData.position;
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;
}

/**
 *  @brief  破棄
 */
bool EffectKairikyRockDown2::Terminate(void)
{
  End();
  GFL_SAFE_DELETE( m_pEffectHandle );
  if( m_pEffectSystem && m_pEffectHeap )
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
 *  @brief  再生
 */
void EffectKairikyRockDown2::Start(void)
{
  if( !IsAlive() )
  {
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN_L );
  }
}

/**
 *  @brief  停止
 */
void EffectKairikyRockDown2::End(void)
{
  if( IsAlive() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
 *  @brief  再生確認
 */
bool EffectKairikyRockDown2::IsAlive(void)
{
  if( !m_pEffectHandle->GetEmitterSet() )             { return false; }
  if( !m_pEffectHandle->GetEmitterSet()->IsAlive() )  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  更新処理
 */
void EffectKairikyRockDown2::updateNormal(void)
{
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
