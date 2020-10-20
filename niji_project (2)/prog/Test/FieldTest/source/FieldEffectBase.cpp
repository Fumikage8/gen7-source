//===================================================================
/**
 * @file    FieldEffectBase.cpp
 * @brief   フィールドエフェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#include "Test/FieldTest/include/FieldEffectBase.h"
#include "Test/FieldTest/include/FieldEffectManager.h"
#include "Test/FieldTest/include/FieldEffectModel.h"

#include "Test/FieldTest/include/movemodel.h"

// FieldTest ( debug )
#include "test/FieldTest/include/debug/FieldDebugMenu.h"
#include "test/FieldTest/include/debug/FieldDebugTypes.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

/**
 * @brief   コンストラクタ
 * @param   pHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pEffectManager フィールドエフェクト管理クラス
 */
FieldEffectBase::FieldEffectBase( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  m_pHeap( pHeap),
  m_pGLAllocator( pGLAllocator),
  m_pEffectManager( pEffectManager),
  m_eftHandle(),
  m_pEffectModel( NULL),
  m_seq( 0),
  m_isEnable( false),
  m_pMoveModel( NULL)
{
}

/**
 * @brief   デストラクタ
 */
FieldEffectBase::~FieldEffectBase()
{
}

/**
 * @brief   初期化
 */
void FieldEffectBase::Initialize( fld::mmodel::MoveModel* pMoveModel)
{
}

/**
 * @brief   再初期化
 */
void FieldEffectBase::Reinitialize( fld::mmodel::MoveModel* pMoveModel)
{
}

/**
 * @brief   破棄
 */
b8 FieldEffectBase::Terminate()
{
  return true;
}

/**
 * @brief   更新
 */
void FieldEffectBase::Update( fld::mmodel::MoveModel* pMoveModel)
{
}

/**
 * @brief   終了したか
 */
b8 FieldEffectBase::IsFinish()
{
  return true;
}

/**
 * @brief   エフェクトモデルインスタンスの取得
 */
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* FieldEffectBase::GetEffectModelInstanceNode()
{
  if( m_pEffectModel)
  {
    return m_pEffectModel->GetModelInstanceNode();
  }
  return NULL;
}

#if PM_DEBUG
void FieldEffectBase::UpdateResourceSwitch()
{
  s32 oldResourceSwitch = m_resourceSwitch;
  m_resourceSwitch = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_FOOT_SPRAY ];
  if( oldResourceSwitch != m_resourceSwitch)
  {
    if( m_resourceSwitch == RESOURCE_SWITCH_NONE)
    {
      this->SetStopCalcAndDraw_EmitterSet( true);
      this->SetVisible_EffectModel( false);
    }
    else if( m_resourceSwitch == RESOURCE_SWITCH_PARTICLE)
    {
      this->SetStopCalcAndDraw_EmitterSet( false);
      this->SetVisible_EffectModel( false);
    }
    else if( m_resourceSwitch == RESOURCE_SWITCH_MODEL)
    {
      this->SetStopCalcAndDraw_EmitterSet( false);
      this->SetVisible_EffectModel( true);
    }
  }
}

void FieldEffectBase::SetStopCalcAndDraw_EmitterSet( bool isStopCalcAndDraw)
{
  if( m_eftHandle.IsValid())
  {
    m_eftHandle.GetEmitterSet()->SetStopCalcAndDraw( isStopCalcAndDraw);
  }
}

void FieldEffectBase::SetVisible_EffectModel( bool isVisible)
{
  m_pEffectModel->SetVisible( isVisible);
}
#endif // PM_DEBUG

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
