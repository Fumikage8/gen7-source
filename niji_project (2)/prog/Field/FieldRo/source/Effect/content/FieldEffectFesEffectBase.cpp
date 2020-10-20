//==============================================================================
/**
 * @file   FieldEffectFesEffectBase.cpp
 * @date   2016/02/09 Tue. 19:47:39
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h> // リソース生成のため
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include  <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include  <Fs/include/gfl2_BinLinkerAccessor.h>


/*  fieldmap  */
#include  "Field/FieldRo/include/FieldRootNode.h"
#include  "Field/FieldRo/include/MyRenderingPipeLine.h"
#include  "Field/FieldRo/include/Fieldmap.h"
#include  "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include  "Field/FieldRo/include/Camera/FieldCameraUnit.h"


#include  "Field/FieldRo/include/Effect/FieldEffectResource_define.h"
#include  "../../../include/Effect/content/FieldEffectFesEffectBase.h"


namespace Field   {
namespace Effect  {
// =============================================================================
/**/
//==============================================================================
const gfl2::math::Vector3 EffectFesEffectBase::POSITION_ZERO(0.0f, 0.0f, 0.0f);


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
EffectFesEffectBase::EffectFesEffectBase(void)
  : IEffectBase()
  , m_SetupData()
  , m_pGLHeapAllocator(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EffectFesEffectBase::~EffectFesEffectBase()
{
  GFL_SAFE_DELETE(m_pGLHeapAllocator);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::Setup(const SetupData& rSetupData)
{
  GFL_ASSERT(!m_pGLHeapAllocator);

  m_SetupData         = rSetupData;
  m_pGLHeapAllocator  = GFL_NEW( m_SetupData.pHeapBase ) gfl2::util::GLHeapAllocator( m_SetupData.pHeapBase );
  
  OnSetup();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  EffectFesEffectBase::Terminate( void )
{
  return OnTerminate();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::Update( void )
{
  OnUpdate();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::UpdateToCamCoord(gfl2::Effect::Handle* pHandle, const gfl2::math::Vector3& rOffset)
{
  if(pHandle->IsValid())
  {
    if(pHandle->GetEmitterSet()->IsAlive())
    {
      gfl2::math::Matrix34  mtx;

      GetCameraCoordMatrix(mtx, rOffset);
      gfl2::Effect::EmitterSet::SetGFMtxFast(pHandle->GetEmitterSet(), mtx);
    }
  }
}




//------------------------------------------------------------------
/**
  *  @brief   
  *  @note    esetのリソースはmanager側でeffectsystemに登録されている
  */
//------------------------------------------------------------------
void EffectFesEffectBase::StartCore(gfl2::Effect::Handle* pHandle, const Field::Effect::EffectResourceID resourceID, const gfl2::math::Matrix34* pWorldMatrix, const gfl2::math::Vector3* pOffset)
{
  if(!pOffset)  pOffset = &POSITION_ZERO;

  const gfl2::math::Matrix34  unitMtx(gfl2::math::Matrix34::GetIdentity());
  gfl2::math::Matrix34        transMtx(gfl2::math::Matrix34::GetTranslation(*pOffset));
  gfl2::math::Matrix34        mtx;


  StopCore(pHandle);

  if(!pWorldMatrix)
  {
    pWorldMatrix  = &unitMtx;
  }
  mtx = *pWorldMatrix * transMtx;

  m_SetupData.pEffectSystem->CreateEmitterSetID(pHandle, mtx, 0, resourceID);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void EffectFesEffectBase::StopCore(gfl2::Effect::Handle* pHandle)
{
  if(
    pHandle
    && pHandle->IsValid()
    && pHandle->GetEmitterSet()
//    && pHandle->GetEmitterSet()->IsAlive()
  )
  {
    pHandle->GetEmitterSet()->Kill();
    pHandle->Invalidate();
  }
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::GetViewMatrix(gfl2::math::Matrix34& rOutMatrix)
{
  Field::Fieldmap*                  pFieldMap   = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap();
  Field::Camera::CameraManager*     pCamManager = pFieldMap->GetCameraManager();
  Field::Camera::CameraUnit*        pCamUnit    = pCamManager->GetMainGamePlayCamera();
  const poke_3d::model::BaseCamera* pBaseCamera = pCamUnit->GetBaseCamera();

  rOutMatrix  = pBaseCamera->GetViewMatrix();
}


//------------------------------------------------------------------
/**
  *  @brief    カメラ座標系に貼り付けるためのマトリクスを取得
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::GetCameraCoordMatrix(gfl2::math::Matrix34& rOutMatrix, const gfl2::math::Vector3& rOffset)
{
  gfl2::math::Matrix34  view;
  gfl2::math::Matrix34  invView;
  gfl2::math::Matrix34  transMtx(gfl2::math::Matrix34::GetTranslation(rOffset));


  GetViewMatrix(view);
  invView = view.Inverse();

  rOutMatrix  = (invView * transMtx);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelMotionResource::CreateModel(EffectFesEffectBase* pCaller, const u32 binLinkerDataID)
{
  if(m_pResourceNode) return;
  
  m_pCaller = pCaller;

  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack  gfBinData;
  c8*                                                               pBinData;

  {
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;

    binLinkerAccessor.Initialize( m_pCaller->m_SetupData.pResource );
    pBinData  = static_cast<c8*>(binLinkerAccessor.GetData(binLinkerDataID));
  }

  gfBinData.SetModelData(pBinData);
  m_pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pCaller->m_pGLHeapAllocator, &gfBinData );

  m_pCaller->m_SetupData.pFieldRootNode->AddResourceChild( m_pResourceNode );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelMotionResource::CreateMotion(EffectFesEffectBase* pCaller, const u32 binLinkerDataID)
{
  if(m_pResourceNode) return;
  
  m_pCaller = pCaller;

  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData  gfBinData;
  c8*                                                           pBinData;

  {
    gfl2::fs::BinLinkerAccessor binLinkerAccessor;

    binLinkerAccessor.Initialize( m_pCaller->m_SetupData.pResource );
    pBinData  = static_cast<c8*>(binLinkerAccessor.GetData(binLinkerDataID));
  }

  gfBinData.SetMotionData(pBinData);
  m_pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode( m_pCaller->m_pGLHeapAllocator, &gfBinData );

  m_pCaller->m_SetupData.pFieldRootNode->AddResourceChild( m_pResourceNode );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelMotionResource::Destroy(void)
{
  if(m_pResourceNode)
  {
    m_pCaller->m_SetupData.pFieldRootNode->RemoveResourceChild(m_pResourceNode);
    GFL_SAFE_DELETE(m_pResourceNode);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelInstance::Create(EffectFesEffectBase* pCaller, ModelMotionResource& rModelResource)
{
  if(m_pModelInstanceNode)  return;
  
  m_pCaller             = pCaller;
  m_pModelInstanceNode  = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateModelInstanceNode( m_pCaller->m_pGLHeapAllocator, rModelResource.GetResourceNode());
  m_Model.Create(m_pCaller->m_pGLHeapAllocator, m_pCaller->m_SetupData.pHeapBase, m_pModelInstanceNode);

  m_pCaller->m_SetupData.pFieldRootNode->AddAfterTransChild(m_pModelInstanceNode);
  m_pCaller->m_SetupData.pFieldPipeLine->AddEdgeRenderingTarget(m_pModelInstanceNode);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelInstance::Update(void)
{
  if(!m_pModelInstanceNode) return;
  
  m_Model.UpdateAnimation();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelInstance::Destroy(void)
{
  if(m_pModelInstanceNode)
  {
    m_pCaller->m_SetupData.pFieldPipeLine->RemoveEdgeRenderingTarget(m_pModelInstanceNode);
    m_Model.Destroy();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EffectFesEffectBase::ModelInstance::SetMotion(ModelMotionResource* pMotionResource)
{
  if(!m_pModelInstanceNode) return;

  m_Model.ChangeAnimationByResourceNode(pMotionResource ? pMotionResource->GetResourceNode() : NULL);
}





} /*  namespace Effect  */
} /*  namespace Field   */
