//======================================================================
/**
 * @file	Object_Light.c++
 * @brief	ライトＯＢＪ (ENV+MOT)
 * @author  Pete
 * @data  15/07/16
 */
//======================================================================

#include <math/include/gfl2_math.h>

#include <DemoLib/Object/include/Object_Light.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>


GFL_NAMESPACE_BEGIN(DemoLib)
  GFL_NAMESPACE_BEGIN(Obj)


Object_Light::Object_Light(ObjectRenderPipeLine *pipe)
: mRenderingPipeLine(pipe)
, mIsAddRenderPath(false)
, m_pDrawEnvNode(NULL)
, m_pDrawEnvResourceRootNode(NULL)
, m_AnimationController(NULL)
{
}

Object_Light::~Object_Light()
{
}

void Object_Light::UpdateAnimation(int step)
{
  UpdateAlways(step);
}

void Object_Light::UpdateAlways(int step)
{
  if(mIsAddRenderPath && m_AnimationController!=NULL && IsEnable()) 
  {
    gfl2::animation::LightAnimationSlot* pLightSlot = m_AnimationController->GetLightSlot();

    if(pLightSlot!=NULL && step!=1)    
    {
      f32 curFrame = pLightSlot->GetFrame();
      f32 frame = curFrame + (step-1);
      pLightSlot->SetFrame(frame);
      
    }

      m_AnimationController->Update();
  }
}


void Object_Light::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  SetIsEnable(true);
}

void Object_Light::CreateBuffer(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator)
{
  gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envResourceData;
  envResourceData.SetBinaryEnvData( static_cast<c8*>(buf) );

  m_pDrawEnvResourceRootNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(allocator,&envResourceData);
  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(allocator,m_pDrawEnvResourceRootNode);

  SetIsEnable(true);
}

void Object_Light::Delete(void)
{
  if( IsEnable() )
  {
    SetIsEnable(false);      
    GFL_SAFE_DELETE(m_pDrawEnvResourceRootNode);
    GFL_SAFE_DELETE(m_pDrawEnvNode);

    if(m_AnimationController!=NULL) m_AnimationController->Finalize();
    GFL_SAFE_DELETE(m_AnimationController);
  }
   
}

void Object_Light::AddRenderPath(int opt)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    GFL_PRINT("Adding Light to RenderPath...\n");
    mSceneOpt = opt;  //自動削除タスクのため覚えておく
    mRenderingPipeLine->AddObject_Light(m_pDrawEnvNode,mSceneOpt);    
    mIsAddRenderPath = true;
  }
}

void Object_Light::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
    GFL_PRINT("Removing Light from RenderPath...\n");
    mRenderingPipeLine->RemoveObject_Light(m_pDrawEnvNode,mSceneOpt);    
    mIsAddRenderPath = false;
  }
}

void Object_Light::ChangeAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,gfl2::heap::HeapBase *heap,bool loop )
{
  
  if( IsEnable() )
  {
    if(m_AnimationController==NULL)
    {    
      m_AnimationController = GFL_NEW(heap)gfl2::animation::DrawEnvAnimationController();
      m_AnimationController->Initialize( heap, mRenderingPipeLine->GetDrawEnvNode(mSceneOpt));
    }
    
    gfl2::animation::LightAnimationSlot* pLightSlot = m_AnimationController->GetLightSlot();
    if(pLightSlot!=NULL)
    {    
      pLightSlot->SetAnimationResource(anmRes);    
      pLightSlot->SetLoop(loop);
      pLightSlot->SetFrame(0.0f);
    }

  }
}



  GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
