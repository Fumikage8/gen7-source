//======================================================================
/**
 * @file FieldEffectBallDemo.cpp
 * @date 2015/11/10 14:34:28
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectBallDemo.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectBallDemo
 *  @brief  エフェクト：モンスターボール演出
 */
/**
 *  @brief  コンストラクタ
 */
EffectBallDemo::EffectBallDemo( void )
: IEffectBase()
, m_pEffectHeap( NULL )
, m_pEffectSystem( NULL )
, m_pEffectHandle( NULL )
{
}

/**
 *  @brief  デストラクタ
 */
EffectBallDemo::~EffectBallDemo( void )
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
 *  @brief  初期化
 */
void EffectBallDemo::Initialize( SetupData &setupData )
{
  m_vPosition = setupData.position;
  m_pEffectHeap = setupData.pEffectHeap;
  m_pEffectSystem = setupData.pEffectSystem;
  m_pEffectHandle = GFL_NEW( m_pEffectHeap->GetGFHeapBase() ) gfl2::Effect::Handle;

  m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_BALL_DEMO );
}

/**
 *  @brief  破棄
 */
bool EffectBallDemo::Terminate( void )
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
void EffectBallDemo::Start( void )
{
  if( !IsAlive() )
  {
    m_pEffectSystem->CreateEmitterSetID( m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_BALL_DEMO );
  }
}

/**
 *  @brief  停止
 */
void EffectBallDemo::End( void )
{
  if( m_pEffectHandle->GetEmitterSet() )
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
 *  @brief  再生確認
 */
bool EffectBallDemo::IsAlive( void )
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
void EffectBallDemo::updateNormal( void )
{
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)
