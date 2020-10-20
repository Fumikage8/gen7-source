//===================================================================
/**
 * @file    FieldEffectFootWaterSpray.cpp
 * @brief   フィールドエフェクト足元水しぶきクラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#include "Test/FieldTest/include/FieldEffectFootWaterSpray.h"
#include "Test/FieldTest/include/FieldEffectManager.h"
#include "Test/FieldTest/include/FieldEffectModel.h"
#include "Test/FieldTest/include/movemodel.h"
#include "Test/FieldTest/include/field_transform.h"

#include <niji_conv_header/field/effect/effect_foot_spray.h>

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
FieldEffectFootWaterSpray::FieldEffectFootWaterSpray( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  FieldEffectBase( pHeap, pGLAllocator, pEffectManager),
  m_isFinish( false)
{
}

/**
 * @brief   デストラクタ
 */
FieldEffectFootWaterSpray::~FieldEffectFootWaterSpray()
{
}

/**
 * @brief   初期化
 */
void FieldEffectFootWaterSpray::Initialize( fld::mmodel::MoveModel* pMoveModel)
{
  gfl2::math::Vector _pos = pMoveModel->GetTransform()->GetTranslation(); // @todo いずれVector3に置き換え
  gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceRootNode;
  pModelResourceRootNode = m_pEffectManager->GetModelResourceRootNode( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode;
  pMotionResourceRootNode = m_pEffectManager->GetMotionResourceRootNode( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);

  gfl2::animation::AnimationController::InitDescription desc;
  desc.materialSlotCount   = 1;
  desc.visibilitySlotCount = 1;

  m_pEffectModel = GFL_NEW( m_pHeap) Field::Effect::Model(); 
  m_pEffectModel->Initialize( m_pHeap, pModelResourceRootNode, pMotionResourceRootNode, desc);

  m_pEffectModel->GetModelInstanceNode()->SetLocalTranslate( pos);
  m_pEffectModel->GetModelInstanceNode()->SetVisible( false);

  m_isEnable = true;

  m_resourceSwitch = RESOURCE_SWITCH_PARTICLE;
#if PM_DEBUG
  m_resourceSwitch = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_FOOT_SPRAY ];
#endif
}

/**
 * @brief   再初期化
 */
void FieldEffectFootWaterSpray::Reinitialize( fld::mmodel::MoveModel* pMoveModel)
{
  m_isFinish = false;

  gfl2::math::Vector _pos = pMoveModel->GetTransform()->GetTranslation(); // @todo いずれVector3に置き換え
  gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());

  m_resourceSwitch = RESOURCE_SWITCH_PARTICLE;
#if PM_DEBUG
  this->UpdateResourceSwitch();
#endif // PM_DEBUG

  if( m_resourceSwitch == RESOURCE_SWITCH_PARTICLE)
  {
    if( m_eftHandle.IsValid())
    {
      m_eftHandle.GetEmitterSet()->Kill();
    }

    m_pEffectManager->GetSystem()->CreateEmitterSetID(
      &m_eftHandle, pos,
      EmitterSetFieldFootSpray::foot_spray, Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY, Field::Effect::Manager::GROUP_ID_0
    );
  }
  else if( m_resourceSwitch == RESOURCE_SWITCH_MODEL)
  {
    m_pEffectModel->GetModelInstanceNode()->SetLocalTranslate( pos);
    m_pEffectModel->GetModelInstanceNode()->SetVisible( true);
  }
}

/**
 * @brief   破棄
 */
b8 FieldEffectFootWaterSpray::Terminate()
{
  enum
  {
    TERMINATE_EFFECT,

    FINISH,
  };

  switch( m_seq)
  {
  case TERMINATE_EFFECT:
    if( m_eftHandle.IsValid())
    {
      m_eftHandle.GetEmitterSet()->Kill();
    }

    m_pEffectModel->Terminate();
    GFL_SAFE_DELETE( m_pEffectModel);

    m_seq++;
    //break;

  case FINISH:
    return true;
    break;
  }
  return false;
}

/**
 * @brief   更新
 */
void FieldEffectFootWaterSpray::Update( fld::mmodel::MoveModel* pMoveModel)
{
  if( m_isEnable == false)
  {
    return;
  }

#if PM_DEBUG
  this->UpdateResourceSwitch();
#endif // PM_DEBUG

  gfl2::math::Vector _pos = pMoveModel->GetTransform()->GetTranslation(); // @todo いずれVector3に置き換え
  gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());

  if( m_resourceSwitch == RESOURCE_SWITCH_PARTICLE)
  {
    if( m_eftHandle.IsValid())
    {
      if( m_eftHandle.GetEmitterSet()->GetNumEmitter() == 0)
      {
        m_isFinish = true;
        m_eftHandle.GetEmitterSet()->SetStopCalcAndDraw( true);
      }
    }
    else
    {
      m_isFinish = true;
    }
  }
  else if( m_resourceSwitch == RESOURCE_SWITCH_MODEL)
  {
    m_pEffectModel->GetAnimationController()->Update();
    if( m_pEffectModel->GetAnimationController()->GetMaterialSlot()->IsLastFrame())
    {
      m_isFinish = true;
      m_pEffectModel->GetModelInstanceNode()->SetVisible( false);
    }
  }
}

/**
 * @brief   終了したか
 */
b8 FieldEffectFootWaterSpray::IsFinish()
{
  return m_isFinish;
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
