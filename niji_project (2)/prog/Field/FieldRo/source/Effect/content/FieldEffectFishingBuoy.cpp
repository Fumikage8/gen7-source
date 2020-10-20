/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectFishingBuoy.cpp
 *  @brief  フィールドエフェクト：うき
 *  @author Masanori Kanamaru
 *  @date   2015.10.28
 */

#include "Field/FieldRo/include/Effect/content/FieldEffectFishingBuoy.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <Fs/include/gfl2_BinLinkerAccessor.h>

#include "Field/FieldRo/include/FieldRootNode.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Effect )

EffectFishingBuoy::EffectFishingBuoy()
: pFieldRootNode(NULL)
, pRenderingPipeLine(NULL)
, pAllocator(NULL)
, pModelInstanceNode(NULL)
, pModelResourceRootNode(NULL)
, pMotionResourceRootNodes()
{
}

void EffectFishingBuoy::Initialize(SetupData &setupData)
{
  this->pFieldRootNode     = setupData.pRootNode;
  this->pRenderingPipeLine = setupData.pPipeLine;

  this->pAllocator = GFL_NEW(setupData.pHeap) gfl2::util::GLHeapAllocator( setupData.pHeap );
  gfl2::fs::BinLinkerAccessor binLinkerAccessor;
  binLinkerAccessor.Initialize( setupData.pResource );

  {
    // モデルリソース作成
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack resourceData;
    resourceData.SetModelData( static_cast<c8*>( binLinkerAccessor.GetData(0)));
    this->pModelResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( this->pAllocator, &resourceData );
    this->pFieldRootNode->AddResourceChild( this->pModelResourceRootNode );
    // モデルインスタンス作成
    this->pModelInstanceNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( this->pAllocator, this->pModelResourceRootNode );
  }
  this->model.Create(this->pAllocator, setupData.pHeap, this->pModelInstanceNode);

  // モーション作成
  for(u32 i = 0; i < BL_IDX_FISHING_BUOY_MAX - 1; ++i)
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData resourceData;
    resourceData.SetMotionData( static_cast<c8*>( binLinkerAccessor.GetData( i + 1) ));
    this->pMotionResourceRootNodes[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( this->pAllocator, &resourceData );
    this->pFieldRootNode->AddResourceChild(this->pMotionResourceRootNodes[i]);
  }

  // 位置を指定
  this->model.SetPosition(setupData.pos);
  this->model.SetVisible(false);

  // 登録
  this->pFieldRootNode->AddAfterTransChild( this->pModelInstanceNode );
  this->pRenderingPipeLine->AddEdgeRenderingTarget( this->pModelInstanceNode );
}

bool EffectFishingBuoy::Terminate()
{
  this->pFieldRootNode->RemoveAfterTransChild(this->pModelInstanceNode);
  this->model.Destroy();

  this->pFieldRootNode->RemoveResourceChild(this->pModelResourceRootNode );
  GFL_SAFE_DELETE( this->pModelResourceRootNode );

  for(u32 i = 0; i < BL_IDX_FISHING_BUOY_MAX - 1; ++i)
  {
    this->pFieldRootNode->RemoveResourceChild(this->pMotionResourceRootNodes[i]);
    GFL_SAFE_DELETE(this->pMotionResourceRootNodes[i]);
  }

  GFL_SAFE_DELETE(this->pAllocator);

  return true;
}

void EffectFishingBuoy::Delete()
{
  this->model.SetVisible(false);
  this->pRenderingPipeLine->RemoveEdgeRenderingTarget(this->pModelInstanceNode);
}

bool EffectFishingBuoy::IsAnimationLastFrame()
{
  return GFL_BOOL_CAST(this->model.IsAnimationLastFrame());
}

void EffectFishingBuoy::ChangeAnimation(int motion)
{
  GFL_ASSERT_MSG(motion < BL_IDX_FISHING_BUOY_MAX, "Fail Animation Change Buoy !! \n");

  this->model.SetVisible(true);
  this->model.ChangeAnimationByResourceNode(this->pMotionResourceRootNodes[motion - 1]);
}


void EffectFishingBuoy::updateNormal()
{
  this->model.UpdateAnimation();
  if (this->model.IsAnimationLastFrame() && this->model.GetAnimationResourceNode() == this->pMotionResourceRootNodes[BL_IDX_FISHING_BUOY_FISHING_UKI_ENTER_GFBMOT - 1])
  {
    this->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_ROOP_GFBMOT);
  }
}

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )