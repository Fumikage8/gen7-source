//======================================================================
/**
 * @file FieldEffectNewTrial5ForModel.cpp
 * @date 2016/01/12 12:00:59
 * @author kawazoe_shinichi
 * @brief 電気試練用エフェクト for モデルタイプ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// gfl2
#include <util/include/gfl2_GLHeapAllocator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>
// field
#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"
// self
#include "Field/FieldRo/include/Effect/content/FieldEffectBaseModel.h"
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectNewTrial5ForModel.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
// ref
#include "niji_reference_files/script/FieldPawnTypes.h"

// conv_header
#include "niji_conv_header/field/effect/demo_new_trial5_02.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @brief  コンストラクタ
*/
EffectNewTrial5ForModel::EffectNewTrial5ForModel(void)
: IEffectBase()
, m_pModel                  (NULL)
, m_pHeap                   (NULL)
, m_pAllocator              (NULL)
, m_pModelResourceRootNode  (NULL)
, m_pMotionResourceRootNode (NULL)
, m_pModelInstanceNode      (NULL)
, m_pFieldRootNode          (NULL)
, m_pRenderingPipeLine      (NULL)
, m_pResource               (NULL)
, m_vPosition               (0.0f,0.0f,0.0f)
, m_vScale                  (1.0f,1.0f,1.0f)
{
}

/**
*  @brief  デストラクタ
*/
EffectNewTrial5ForModel::~EffectNewTrial5ForModel(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  初期化
*/
void EffectNewTrial5ForModel::Initialize(SetupData &setupData)
{
  m_pHeap               = setupData.pHeap;
  m_pResource           = setupData.pResource;
  m_pFieldRootNode      = setupData.pRootNode;
  m_pRenderingPipeLine  = setupData.pPipeLine;
  m_vPosition           = setupData.position;

  // アロケータ作成
  m_pAllocator = GFL_NEW(m_pHeap) gfl2::util::GLHeapAllocator(m_pHeap);

}

/**
*  @brief  破棄
*/
bool EffectNewTrial5ForModel::Terminate(void)
{
  if (m_pModelInstanceNode)
  {
    m_pFieldRootNode->RemoveAfterTransChild(m_pModelInstanceNode);

    if (m_pModel)
    {
      m_pModel->Destroy();
    }

    m_pFieldRootNode->RemoveResourceChild(m_pModelResourceRootNode);
    GFL_SAFE_DELETE(m_pModelResourceRootNode);
    m_pFieldRootNode->RemoveResourceChild(m_pMotionResourceRootNode);
    GFL_SAFE_DELETE(m_pMotionResourceRootNode);

    GFL_SAFE_DELETE(m_pModel);
  }

  GFL_SAFE_DELETE(m_pAllocator);

  return true;
}

/**
*  @brief  各エフェクトで必要な破棄作業
*/
void EffectNewTrial5ForModel::Delete(void)
{
  if (m_pModel)
  {
    m_pModel->SetVisible(false);
  }
  if (m_pModelInstanceNode)
  {
    m_pRenderingPipeLine->RemoveEdgeRenderingTarget(m_pModelInstanceNode);
  }
}



/**
*  @brief  エフェクトを再生する
*/
void EffectNewTrial5ForModel::CreateEffect(s32 idx)
{
  // 既に再生中の場合破棄して新しく再生しなおすことはできない（参照カウント）
  GFL_ASSERT(m_pModelInstanceNode == NULL);

  s32 modelID = -1;
  s32 motionID = -1;

  switch (idx)
  {
  case Field::FieldScript::EFF_S5_MDL_KUWA_BEAM1:
  case Field::FieldScript::EFF_S5_MDL_KUWA_BEAM2:
  case Field::FieldScript::EFF_S5_MDL_KUWA_BEAM3:
    modelID   = BL_IDX_DEMO_NEW_TRIAL5_02_EF_B_DEMO_NEW_TRIAL5_KUWA_BEAM_CTR_GFBMDLP;
    motionID  = BL_IDX_DEMO_NEW_TRIAL5_02_EF_B_DEMO_NEW_TRIAL5_KUWA_BEAM_GFBMOT;
    break;
  case Field::FieldScript::EFF_S5_MDL_Z:
    modelID   = BL_IDX_DEMO_NEW_TRIAL5_02_EF_B_DEMO_NEW_TRIAL5_Z_CTR_GFBMDLP;
    motionID  = BL_IDX_DEMO_NEW_TRIAL5_02_EF_B_DEMO_NEW_TRIAL5_Z_GFBMOT;
    break;
 
  }

  if (modelID != -1)
  {
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;
    binLinkerAccessor.Initialize(m_pResource);
    // モデル生成
    m_pModel = GFL_NEW(m_pHeap) poke_3d::model::BaseModel();
    void *pModelData = binLinkerAccessor.GetData(modelID);
    createModel(pModelData);
    m_pModel->Create(m_pAllocator, m_pHeap, m_pModelInstanceNode);
    // モーション生成
    void *pMotionData = binLinkerAccessor.GetData(motionID);
    createMotion(pMotionData);
    m_pModel->ChangeAnimationByResourceNode(m_pMotionResourceRootNode);
    m_pModel->SetAnimationLoop(true);

    // 通常シーンへの描画
    m_pModel->SetPosition(m_vPosition);
    m_pModel->SetScale(m_vScale);
    m_pFieldRootNode->AddAfterTransChild(m_pModelInstanceNode);
    m_pRenderingPipeLine->AddEdgeRenderingTarget(m_pModelInstanceNode);
  }
  
}

/**
*  @brief  再生確認
*/
bool EffectNewTrial5ForModel::IsAlive(void)
{
  if (!m_pModel){ return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  更新処理
*/
void EffectNewTrial5ForModel::updateNormal(void)
{
  if (m_pModel)
  {
    m_pModel->UpdateAnimation();
  }
}


/**
*  @brief  モデルの作成
*/
void EffectNewTrial5ForModel::createModel(void *pModelBinary)
{
  // モデルリソース作成
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
  resourceData.SetModelData(static_cast<c8*>(pModelBinary));
  m_pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData);
  m_pFieldRootNode->AddResourceChild(m_pModelResourceRootNode);

  // モデルインスタンス作成
  m_pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode(m_pAllocator, m_pModelResourceRootNode);
}

/**
*  @brief  モーションの作成
*/
void EffectNewTrial5ForModel::createMotion(void *pMotionBinary)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
  resourceData.SetMotionData(static_cast<c8*>(pMotionBinary));
  m_pMotionResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pAllocator, &resourceData);
  m_pFieldRootNode->AddResourceChild(m_pMotionResourceRootNode);
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)