//===================================================================
/**
 * @file    FieldEffectFootManager.cpp
 * @brief   フィールドエフェクト足元エフェクト管理クラス
 * @author  saita_kazuki
 * @date    2015.03.18
 */
//===================================================================

#include "Test/FieldTest/include/FieldEffectFootManager.h"
#include "Test/FieldTest/include/FieldEffectManager.h"
#include "Test/FieldTest/include/FieldEffectModel.h"
#include "Test/FieldTest/include/FieldEffectFootWaterSpray.h"
#include "Test/FieldTest/include/movemodel.h"
#include "Test/FieldTest/include/field_transform.h"

#include "Test/FieldTest/include/FieldTestMyRenderingPipeLine.h" // @todo field用に置き換える 150215 ikuta_junya

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


/**
 * @brief   コンストラクタ
 * @param   pHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pEffectManager フィールドエフェクト管理クラス
 */
FootManager::FootManager( gfl2::heap::HeapBase* pHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager, Test::FieldTest::MyRenderingPipeLine* pRenderingPipeLine) :
  m_pHeap( pHeap),
  m_pGLAllocator( pGLAllocator),
  m_pEffectManager( pEffectManager),
  m_pRenderingPipeLine( pRenderingPipeLine),
  m_pMoveModel( NULL),
  m_pEffect()
{
}

/**
 * @brief   デストラクタ
 */
 FootManager::~FootManager()
 {
 }

/**
 * @brief   初期化
 */
void FootManager::Initialize()
{
  m_pEffectManager->CreateResource( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);
  m_pEffectManager->CreateModelResource( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);

  for( u32 i = 0; i < EFFECT_BUFFER_MAX; ++i)
  {
    m_pEffect[i] = GFL_NEW( m_pHeap) Field::Effect::FieldEffectFootWaterSpray( m_pHeap, m_pGLAllocator, m_pEffectManager);
    m_pEffect[i]->Initialize( m_pMoveModel);

    gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pEffect[i]->GetEffectModelInstanceNode();
    if( pModelInstanceNode)
    {
      m_pRenderingPipeLine->AddEdgeRenderingTarget( pModelInstanceNode);
    }
  }
}

/**
 * @brief   破棄
 */
b8 FootManager::Terminate()
{
  b8 isFinish = true;
  for( u32 i = 0; i < EFFECT_BUFFER_MAX; ++i)
  {
    if( m_pEffect[i])
    {
      isFinish = false;
      if( m_pEffect[i]->Terminate())
      {
        gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode = m_pEffect[i]->GetEffectModelInstanceNode();
        if( pModelInstanceNode)
        {
          m_pRenderingPipeLine->RemoveEdgeRenderingTarget( pModelInstanceNode);
        }
        GFL_SAFE_DELETE( m_pEffect[i]);
      }
    }
  }

  m_pEffectManager->DestroyResource( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);
  m_pEffectManager->DestroyModelResource( Field::Effect::Manager::RESOURCE_ID_FOOT_SPRAY);
  return isFinish;
}

/**
 * @brief   更新
 */
void FootManager::Update()
{
  this->UpdateEffect();
  this->UpdateCreateRequest();
}

/**
 * @brief   足元エフェクトを出す動作モデルを登録する
 */
b8 FootManager::RegisterMoveModel( fld::mmodel::MoveModel* pMoveModel)
{
  if( m_pMoveModel == NULL)
  {
    m_pMoveModel = pMoveModel;

    gfl2::math::Vector _pos = m_pMoveModel->GetTransform()->GetTranslation();
    gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());
    m_oldPos = pos;

    return true;
  }
  return false;
}

/**
 * @brief   エフェクト更新
 */
void FootManager::UpdateEffect()
{
  for( u32 i = 0; i < EFFECT_BUFFER_MAX; ++i)
  {
    if( m_pEffect[i])
    {
      if( !m_pEffect[i]->IsFinish())
      {
        m_pEffect[i]->Update( m_pMoveModel);
      }
    }
  }
}

/**
 * @brief   エフェクト生成リクエストの更新
 */
void FootManager::UpdateCreateRequest()
{
  gfl2::math::Vector _pos = m_pMoveModel->GetTransform()->GetTranslation();
  gfl2::math::Vector3 pos( _pos.GetX(), _pos.GetY(), _pos.GetZ());
  gfl2::math::Vector3 vec = m_oldPos - pos;

  const f32 MOVE_LENGTH = 50.0f;
  if( MOVE_LENGTH < vec.Length())
  {
    m_oldPos = pos;
    for( u32 i = 0; i < EFFECT_BUFFER_MAX; ++i)
    {
      if( m_pEffect[i]->IsFinish())
      {
        m_pEffect[i]->Reinitialize( m_pMoveModel);
        break;
      }
    }
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
