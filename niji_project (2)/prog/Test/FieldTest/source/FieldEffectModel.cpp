//===================================================================
/**
 * @file    FieldEffectModel.cpp
 * @brief   エフェクトモデルラス
 * @author  saita_kazuki
 * @date    2015.03.17
 */
//===================================================================

#include "Test/FieldTest/include/FieldEffectModel.h"

#include "Test/FieldTest/include/FieldTestUtility.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include "test/FieldTest/include/debug/FieldDebugInstanceMemoryCollection.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )


/**
 * @brief       コンストラクタ
 */
Model::Model() :
  m_pModelRootNode( NULL),
  m_pModelInstanceNode( NULL),
  m_pMotionRootNode( NULL),
  m_pAnimationController( NULL)
{
}

/**
 * @brief       デストラクタ
 */
Model::~Model()
{
}

/**
 * @brief       初期化
 */
void Model::Initialize( gfl2::heap::HeapBase* pHeap,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelRootNode,
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionRootNode,
  gfl2::animation::AnimationController::InitDescription& desc)
{
  m_pModelRootNode = pModelRootNode;
  m_pMotionRootNode = pMotionRootNode;

  {
    static int create_indtance_count = 0; char strbuff[128] = {0};
    Test::FieldTest::Utility::Sprintf_S( strbuff, 127, "EffectModel[%d]",  create_indtance_count ++ );
    int _index = FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_AddCollectionData( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_StartCollection(_index);
    m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pModelRootNode);
    m_pModelInstanceNode->SetNodeName( strbuff );
    FIELD_TEST_DEBUG_INSTANCE_MEMORY_COLLECTION_EndCollection(_index);
  }

  m_pAnimationController = GFL_NEW( pHeap) gfl2::animation::AnimationController();
  m_pAnimationController->Initialize( m_pModelInstanceNode, desc);

  if( desc.jointSlotCount > 0)
  {
    gfl2::animation::JointAnimationSlot* pJointSlot = m_pAnimationController->GetJointSlot();
    pJointSlot->SetAnimationResource( m_pMotionRootNode);
    pJointSlot->SetLoop( true);
  }

  if( desc.materialSlotCount > 0)
  {
    gfl2::animation::MaterialAnimationSlot* pMaterialSlot = m_pAnimationController->GetMaterialSlot();
    pMaterialSlot->SetAnimationResource( m_pMotionRootNode);
    pMaterialSlot->SetLoop( true);
  }

  if( desc.visibilitySlotCount > 0)
  {
    gfl2::animation::VisibilityAnimationSlot* pVisibilitySlot = m_pAnimationController->GetVisibilitySlot();
    pVisibilitySlot->SetAnimationResource( m_pMotionRootNode);
    pVisibilitySlot->SetLoop( true);
  }
}

/**
 * @brief       破棄
 */
void Model::Terminate()
{
  GFL_SAFE_DELETE( m_pAnimationController);
  GFL_SAFE_DELETE( m_pModelInstanceNode);
}

/**
 * @brief       更新
 */
void Model::Update()
{
  m_pAnimationController->Update();
}

/**
 * @brief       表示フラグ変更
 */
void Model::SetVisible( b8 isVisible)
{
  if( this->IsVisible() != isVisible)
  {
    // フラグが切り替わったらシーンへの登録、解除を行う
    if( isVisible)
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( m_pModelInstanceNode);
    }
    else
    {
      gfl2::renderingengine::scenegraph::SceneGraphManager::RemoveChild( m_pModelInstanceNode);
    }
  }

  m_pModelInstanceNode->SetVisible( isVisible);
}

/**
 * @brief       表示フラグ取得
 */
b8 Model::IsVisible()
{
  return m_pModelInstanceNode->IsVisible();
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )
