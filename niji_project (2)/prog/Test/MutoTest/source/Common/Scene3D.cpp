#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   Scene3D.cpp
 * @date   2016/09/06 Tue. 19:33:38
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <gfx/include/gfl2_DrawUtil.h>
#include  <GameSys/include/GameManager.h>
#include  <thread/include/gfl2_ThreadStatic.h>

#include  <arc_def_index/arc_def.h>
#include  <arc_index/debug_viewer_chara_model_battle.gaix>


#include  "./Scene3D.h"


namespace
{
  static const u32  QuitCountMax  = 4;

  static const app::util::AppRenderingManager::RenderPlaceType::Tag  renderPlace  = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;

#if 0
#define __AXIS_LEN  (500.0f)
#define __AXIS_W    (  5.0f)
#define __COL_X     1.0f,    0,    0, 0.5f
#define __COL_Y        0, 1.0f,    0, 0.5f
#define __COL_Z        0,    0, 1.0f, 0.5f
#define __VERT(_x, _y, _z, _col)  gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4((_x),(_y),(_z),1.0f), gfl2::math::Vector4(), gfl2::math::Vector4(_col))
  static const gfl2::gfx::DrawUtil::VertexFormat  vertices_Grid[] = 
  {
    /*  x y z  */
    __VERT(        0,         0,  -__AXIS_W, __COL_X), __VERT(       0,         0, __AXIS_W, __COL_X), __VERT(__AXIS_LEN,           0,          0, __COL_X),
    __VERT(        0, -__AXIS_W,          0, __COL_X), __VERT(       0,  __AXIS_W,        0, __COL_X), __VERT(__AXIS_LEN,           0,          0, __COL_X),

    __VERT(        0,         0,  -__AXIS_W, __COL_Y), __VERT(       0,         0, __AXIS_W, __COL_Y), __VERT(0,           __AXIS_LEN,          0, __COL_Y),
    __VERT(-__AXIS_W,         0,          0, __COL_Y), __VERT(__AXIS_W,         0,        0, __COL_Y), __VERT(0,           __AXIS_LEN,          0, __COL_Y),

    __VERT(        0, -__AXIS_W,          0, __COL_Z), __VERT(       0,  __AXIS_W,        0, __COL_Z), __VERT(0,                    0, __AXIS_LEN, __COL_Z),
    __VERT(-__AXIS_W,         0,          0, __COL_Z), __VERT(__AXIS_W,         0,        0, __COL_Z), __VERT(0,                    0, __AXIS_LEN, __COL_Z),

    /*  grid  */
  };
#undef  __VERT
#undef  __COL_X
#undef  __COL_Y
#undef  __COL_Z
#undef  __AXIS_LEN
#undef  __AXIS_W
#endif
}



namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
Scene3D::Scene3D(void)
  : m_pHeap(NULL)
  , m_pDrawEnvNode(NULL)
//  , m_pDrawEnvNode_Light(NULL)
  , m_pRenderingManager(NULL)
  /*  camera  */
  , m_DefaultCamera()
  , m_pCamera(NULL)
  /*  light  */
  , m_pLight(NULL)
  /*  others  */
  , m_pNijiAllocator(NULL)
  , m_QuitCount(0)
  , m_bQuitRequest(false)
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
Scene3D::~Scene3D()
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
void  Scene3D::Initialize(app::util::Heap* pUtilHeap)
{
  m_pHeap = pUtilHeap;

  /*  renderingmanager  */
  {
    gfl2::heap::HeapBase*                       pHeapBase = m_pHeap->GetDeviceHeap();
    app::util::AppRenderingManager::Description desc;

    /*  desctiption  */
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder  = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder  = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;

    /*  creation  */
    m_pNijiAllocator    = GFL_NEW(pHeapBase) System::nijiAllocator(pHeapBase);
    m_pRenderingManager = GFL_NEW(pHeapBase) app::util::AppRenderingManager();
    m_pRenderingManager->SyncCreate(
        desc,                             // 呼び出し元はdescインスタンスを破棄してもいい(descに書かれているポインタが指すものは破棄しないように)
        m_pHeap,                          // 生成に使用するヒープ。m_pDevHeap, m_pDevHeapAllocator, m_pDevGLHeapAllocatorしか使っていない。
        m_pNijiAllocator,                 // VRAM限定。RenderOrderTypeにPOKEMODELを書いているときは必須、そうでないときはNULLでよい。
        GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager()                              // ファイルマネージャ
    );
  }

  /*  camera  */
  {
    m_DefaultCamera.Create(m_pHeap);
    SetCamera(m_DefaultCamera);
  }



  // 上画面用ライト
  {
//    m_pDrawEnvNode_Light = app::util::AppRenderingManager::CreateDefaultDrawEnv(m_pHeap);
//    m_pDrawEnvNode_Light->GetLightSet(2)->GetLightNode(0)->SetColor( gfl2::math::Vector3( 1.0f*0.85f, 1.0f*0.85f, 1.0f*0.85f ) );
//    m_pDrawEnvNode_Light->GetLightSet(2)->GetLightNode(1)->SetColor( gfl2::math::Vector3( 0.2f*0.85f, 0.2f*0.85f, 0.2f*0.85f ) );
//    m_pRenderingManager->AddDrawEnv(renderPlace, m_pDrawEnvNode_Light);
  }



  /*  resource  */
  {
    const u32 loaders = GFL_NELEMS(m_ResourceLoaders);
    u32       loaded  = 0;

    m_ResourceLoaders[MODEL_Grid].RequestLoad(m_pHeap, ARCID_DEBUG_VIEWER_CHARA_MODEL_BATTLE, GARC_debug_viewer_chara_model_battle_grid_ctr_GFBMDLP);

    do
    {
      loaded  = 0;
      gfl2::thread::ThreadStatic::CurrentSleep(1);
      for(u32 index = 0; index < loaders; ++index)
      {
        if(m_ResourceLoaders[index].ObserveLoad())  ++loaded;
      }
    }while(loaded < loaders);

    m_EasyModels[MODEL_Grid].Create(m_ResourceLoaders[MODEL_Grid].GetData(), m_pHeap);

    for(u32 index = 0; index < GFL_NELEMS(m_EasyModels); ++index)
    {
      AddModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_EasyModels[index].GetBaseModel(), 0, false);
    }
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
void  Scene3D::Finalize(void)
{
  /*  cam  */
  {
    UnsetCamera();
    m_DefaultCamera.Release();
  }

  /*  env  */
  {
    if(m_pRenderingManager)
    {
//      m_pRenderingManager->RemoveDrawEnv(renderPlace, m_pDrawEnvNode_Light); 
    }
//    GFL_SAFE_DELETE(m_pDrawEnvNode_Light);
  }

  /*  renderingManager  */
  {
    GFL_SAFE_DELETE(m_pRenderingManager);
    GFL_SAFE_DELETE(m_pNijiAllocator);
  }

  /*  resources  */
  {
    for(u32 index = 0; index < GFL_NELEMS(m_EasyModels); ++index)
    {
      RemoveModel_U(m_EasyModels[index]);
      m_EasyModels[index].Release();
    }
  }

  /*  resource  */
  {
    for(u32 index = 0; index < GFL_NELEMS(m_ResourceLoaders); ++index)
    {
      m_ResourceLoaders[index].ReleaseData();
    }
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
void  Scene3D::OnUpdate(void)
{
  /*  cam  */
  {
    if(m_pCamera)
    {
      m_pCamera->Update();
    }
  }


  if(m_pRenderingManager)
  {
    m_pRenderingManager->Update();
  }

  if(m_bQuitRequest)
  {
    if(m_QuitCount < QuitCountMax)
    {
      ++m_QuitCount;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Scene3D::OnPreDraw(void)
{
  if(m_bQuitRequest)  return;

  if(m_pRenderingManager)
  {
    m_pRenderingManager->PreDraw();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Scene3D::OnDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_bQuitRequest)  return;

  if(m_pRenderingManager)
  {
    m_pRenderingManager->Draw(displayNo);
  }

#if 0
  {
    gfl2::gfx::GFGL::BeginScene();
      gfl2::gfx::DrawUtil::BeginRenderExtension();

        gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);
        gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);
        gfl2::gfx::DrawUtil::SetMaterialColor(gfl2::math::Vector4(1,1,1,1));
        gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
        gfl2::gfx::DrawUtil::SetDepthTestEnable(false);

        if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
        {
          gfl2::gfx::DrawUtil::SetScreenSize( 320, 240);
          gfl2::gfx::GFGL::SetViewport( 0, 0, 320, 240);
          gfl2::gfx::GFGL::SetScissor(0, 0, 320, 240);
        }
        else
        {
          gfl2::gfx::DrawUtil::SetScreenSize( 400, 240);
          gfl2::gfx::GFGL::SetViewport( 0, 0, 400, 240);
          gfl2::gfx::GFGL::SetScissor(0, 0, 400, 240);
        }

        /*  cam  */
        {
          gfl2::gfx::DrawUtil::SetProjectionMatrix(m_pBaseCamera->GetProjectionMatrix());
          gfl2::gfx::DrawUtil::SetViewMatrix(m_pBaseCamera->GetViewMatrix());
          
//          gfl2::gfx::DrawUtil::SetModelMatrix(const math::Matrix44& mat);
        }


        /*  grid  */
        if(1)
        {
        }

        /*  axis  */
        if(1)
        {
          gfl2::gfx::DrawUtil::DrawTriangles(vertices_Grid, GFL_NELEMS(vertices_Grid), false);
        }



      gfl2::gfx::DrawUtil::EndRenderExtension();
    gfl2::gfx::GFGL::EndScene();
  }
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::AddModel_U(EasyModel& rModel)
{
  AddModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, rModel.GetBaseModel());
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Scene3D::RemoveModel_U(EasyModel& rModel)
{
  RemoveModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, rModel.GetBaseModel());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::AddModel_L(EasyModel& rModel)
{
  AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, rModel.GetBaseModel());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::RemoveModel_L(EasyModel& rModel)
{
  RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, rModel.GetBaseModel());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::AddModel(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo, bool enableEdgeRenderPath)
{
  if(!m_pRenderingManager)  return;
  
  m_pRenderingManager->AddModel(renderPlace, pModel, layerNo, enableEdgeRenderPath);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::RemoveModel(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, poke_3d::model::BaseModel* pModel, u32 layerNo)
{
  if(!m_pRenderingManager)  return;
  
  m_pRenderingManager->RemoveModel(renderPlace, pModel, layerNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  Scene3D::IsStable(void) const
{
  return  (QuitCountMax <= m_QuitCount);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::SetCamera(EasyCamera& rCamera)
{
  if(!m_pRenderingManager)  return;

  UnsetCamera();

  m_pCamera = &rCamera;
  m_pCamera->SetEnable(true);
  m_pRenderingManager->SetCamera(m_pCamera->GetRenderPlace(), m_pCamera->GetBaseCamera());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::UnsetCamera(void)
{
  if(!m_pRenderingManager)  return;
  if(!m_pCamera)            return;
  m_pCamera->SetEnable(false);
  m_pRenderingManager->UnsetCamera(m_pCamera->GetRenderPlace());
  
  m_pCamera = NULL;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  Scene3D::SetLight(EasyLight& rLight)
{
  if(!m_pRenderingManager)  return;

  m_pLight = &rLight;
  m_pRenderingManager->AddDrawEnv(m_pLight->GetRenderPlace(), m_pLight->GetDrawEnvNode()); 
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Scene3D::UnsetLight(void)
{
  if(!m_pRenderingManager)  return;
  if(!m_pLight)             return;

  m_pRenderingManager->RemoveDrawEnv(m_pLight->GetRenderPlace(), m_pLight->GetDrawEnvNode()); 

  m_pLight  = NULL;
}




} /*  namespace MutoTest */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
