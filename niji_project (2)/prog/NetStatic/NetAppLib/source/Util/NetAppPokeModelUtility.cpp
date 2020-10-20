//==============================================================================
/**
 * @file   NetAppPokeModelUtility.cpp
 * @date   2015/08/06 Thu. 18:10:14
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
//#include <nn/util.h>

#include  "GameSys/include/GameManager.h"
#include  "PokeTool/include/PokeTool.h"
#include  "System/include/Camera/ModelViewerInFrame.h"
#include  "AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationWorkBase.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"

#if PM_DEBUG
  #include  "./NetAppPokeModelUtilityDebug/NetAppPokeModelUtilityDebug.h"
#endif

// =============================================================================
/**/
//==============================================================================
namespace
{
}


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================



//==============================================================================
/**
  *  @brief  一体分の環境
  *  @date   2015/08/06 Thu. 18:12:25
  */
//==============================================================================
class NetAppPokeModelUtility::ModelEnvironment
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelEnvironment);
public:
  ModelEnvironment(void);
  virtual ~ModelEnvironment();


  void  Initialize(NetAppPokeModelUtility* pOwner, gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderManager, PokeTool::PokeModelSystem* pPokeModelSystem, ::System::Camera::CModelViewerInFrame* pInFrameSystem, ::System::Camera::InFrameCamera* pInFrameCamera);


  void  Request(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, const PokeTool::SimpleParam& rSimpleParam, const PokeTool::MODEL_ANIMETYPE animeType, const u32 layerNo);
  void  Update(void);
  void  ReleaseRequest(void);
  bool  IsStable(void) const;
  void  SetShaderColor(const gfl2::gfx::Color& color)
        {
          if(m_pPokeModel)
          {
            if(m_State == STATE_Idle)
            {
              m_pPokeModel->SetShaderColor(color);
            }
          }
        }

  void  SetVisibility_InFrame(const bool isVisible)
        {
          if(m_pInFrameSystem)
          {
            m_pInFrameSystem->SetModelVisible(isVisible);
          }
        }

  void  SetEventHandler(NetAppPokeModelUtility::IEventHandler* pHandler)  {m_pHandler = pHandler;}


  
  app::util::AppRenderingManager::RenderPlaceType::Tag  GetRenderPlaceType(void) const  {return m_RenderPlace;}
  u32                                                   GetLayerNo(void) const          {return m_LayerNo;}

protected:
  void  Event(const NetAppPokeModelUtility::IEventHandler::EventID eventID)
        {
          if(m_pHandler)  m_pHandler->NetAppPokeModelUtility_OnEvent(m_pOwner, eventID, m_pPokeModel);
        }


  enum  State
  {
    STATE_Initial,
    STATE_Loading,
    STATE_Idle,
    STATE_Cleanup,

    STATE_
  };
  static const u32  CleanupCount  = 2;

  NetAppPokeModelUtility*                               m_pOwner;
  gfl2::heap::HeapBase*                                 m_pHeapBase;
  app::util::AppRenderingManager*                       m_pRenderManager;
  PokeTool::PokeModelSystem*                            m_pPokeModelSystem;
  PokeTool::PokeModel*                                  m_pPokeModel;
  ::System::Camera::CModelViewerInFrame*                m_pInFrameSystem;
  ::System::Camera::InFrameCamera*                      m_pInFrameCamera;
  State                                                 m_State;
  bool                                                  m_IsReleaseRequest;
  u32                                                   m_CleanupCount;
  NetAppPokeModelUtility::IEventHandler*                m_pHandler;
  /*  request  */
  PokeTool::SimpleParam                                 m_NextPokeToolSimpleParam;
  PokeTool::MODEL_ANIMETYPE                             m_NextAnimeType;
  app::util::AppRenderingManager::RenderPlaceType::Tag  m_RenderPlace;
  u32                                                   m_LayerNo;
};


/*
*/
NetAppPokeModelUtility::ModelEnvironment::ModelEnvironment(void)
  : m_pOwner(NULL)
  , m_pHeapBase(NULL)
  , m_pRenderManager(NULL)
  , m_pPokeModelSystem(NULL)
  , m_pPokeModel(NULL)
  , m_pInFrameSystem(NULL)
  , m_pInFrameCamera(NULL)
  , m_State(STATE_Initial)
  , m_IsReleaseRequest(false)
  , m_CleanupCount(CleanupCount)
  , m_pHandler(NULL)
  , m_NextPokeToolSimpleParam()
  , m_NextAnimeType()
  , m_RenderPlace(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY)
  , m_LayerNo(0)
{
}

/*
*/
NetAppPokeModelUtility::ModelEnvironment::~ModelEnvironment()
{
  GFL_ASSERT(IsStable());
}

void  NetAppPokeModelUtility::ModelEnvironment::Initialize(NetAppPokeModelUtility* pOwner, gfl2::heap::HeapBase* pHeapBase, app::util::AppRenderingManager* pRenderManager, PokeTool::PokeModelSystem* pPokeModelSystem, ::System::Camera::CModelViewerInFrame* pInFrameSystem, ::System::Camera::InFrameCamera* pInFrameCamera)
{
  m_pOwner            = pOwner;
  m_pHeapBase         = pHeapBase;
  m_pRenderManager    = pRenderManager;
  m_pPokeModelSystem  = pPokeModelSystem;
  m_pInFrameSystem    = pInFrameSystem;
  m_pInFrameCamera    = pInFrameCamera;
}


void  NetAppPokeModelUtility::ModelEnvironment::Request(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, const PokeTool::SimpleParam& rSimpleParam, const PokeTool::MODEL_ANIMETYPE animeType, const u32 layerNo)
{
  m_NextPokeToolSimpleParam = rSimpleParam;
  m_NextAnimeType           = animeType;
  m_RenderPlace             = renderPlace;
  m_LayerNo                 = layerNo;
}


void  NetAppPokeModelUtility::ModelEnvironment::Update(void)
{
  switch(m_State)
  {
    case  STATE_Initial:
      if(m_NextPokeToolSimpleParam.monsNo != MONSNO_NULL)
      {
        PokeTool::PokeModel::SetupOption  setupOption;

        setupOption.dataHeap    = m_pHeapBase;
        setupOption.workHeap    = m_pHeapBase->GetLowerHandle();
        setupOption.animeType   = m_NextAnimeType;
//        setupOption.useShadow   = false;
//        setupOption.useIdModel  = true;

        m_pPokeModel  = m_pPokeModelSystem->CreateModelSimple(&m_NextPokeToolSimpleParam, setupOption);
        m_NextPokeToolSimpleParam.monsNo  = MONSNO_NULL;
        
        m_State       = STATE_Loading;
      }
      break;

    case  STATE_Loading:
      if( m_pPokeModel->IsFinishLoad() )
      {
        m_pPokeModel->Create();
        m_pPokeModel->ChangeAnimation( PokeTool::MODEL_ANIME_KW_WAIT );
        m_pPokeModel->SetupColorShader();
        SetShaderColor(gfl2::gfx::Color(0.0f, 0.0f, 0.0f, 0.0f));
        m_pRenderManager->SetInFrameCamera( m_RenderPlace, m_pInFrameCamera, m_LayerNo );
        m_pRenderManager->AddPokeModel(m_RenderPlace, m_pPokeModel,m_LayerNo);

        if( m_pInFrameSystem )
        {
          m_pInFrameSystem->SetAndInitModel( m_pPokeModel );
          m_pInFrameSystem->Update( m_pInFrameCamera ); // 1フレーム変な位置にポケモンが出るのを防ぐために、すぐにインフレームのUpdateを呼んでおく。
        }

        m_State = STATE_Idle;
        Event(NetAppPokeModelUtility::IEventHandler::EVENT_OnModelLoded);
      }
      break;

    case  STATE_Idle:
      m_pPokeModel->SetMegaGangarEffectState(m_pPokeModel->GetPosition().y);
      if(m_IsReleaseRequest || (m_NextPokeToolSimpleParam.monsNo != MONSNO_NULL))
      {
        m_pPokeModel->CleanColorShader();
        m_pRenderManager->RemovePokeModel(m_RenderPlace, m_pPokeModel,m_LayerNo);
        m_pRenderManager->UnsetInFrameCamera( m_RenderPlace, m_LayerNo );
        if( m_pInFrameSystem )
        {
          m_pInFrameSystem->ResetModel();
        }
        m_State  = STATE_Cleanup;
      }
      break;

    case  STATE_Cleanup:
      if(m_CleanupCount)
      {
        --m_CleanupCount;
        break;
      }
      
      m_pPokeModelSystem->DeleteModel(m_pPokeModel);
      m_pPokeModel        = NULL;
      m_IsReleaseRequest  = false;
      m_CleanupCount      = CleanupCount;
      m_State             = STATE_Initial;
      break;
  }
}


void  NetAppPokeModelUtility::ModelEnvironment::ReleaseRequest(void)
{
  if((m_State == STATE_Initial) || (m_State == STATE_Cleanup))  return;

  m_IsReleaseRequest                = true;
  m_CleanupCount                    = CleanupCount;
  m_NextPokeToolSimpleParam.monsNo  = MONSNO_NULL;
}


bool  NetAppPokeModelUtility::ModelEnvironment::IsStable(void) const
{
  return (m_State == STATE_Initial);
}











//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
NetAppPokeModelUtility::NetAppPokeModelUtility(System::ApplicationWorkBase* pWorkBase)
  : m_pWorkBase(pWorkBase)
  , m_PokeModelSystem(NULL)
  , m_pEnvNode(NULL)
  , m_pModelEnvironments(NULL)
  , m_State(STATE_Initial)
  , m_Slots(0)
  , m_IsCleanupRequest(false)
  , m_PokeModelSystemHeapOption()
  , m_pGLHeapAllocator(NULL)
  , m_pInFrameSystem(NULL)
  , m_pInFrameCamera(NULL)
#if PM_DEBUG
  , m_pPokeModelUtilDebug(NULL)
  , m_pParentDebugWinGroup(NULL)
  , m_DebugOffset_OfsX(0)
  , m_DebugOffset_OfsY(0)
  , m_DebugOffset_OfsSize(0)
#endif
{
//  m_PokeModelSystemHeapOption.useIdModel      = true;
//  m_PokeModelSystemHeapOption.useShadow       = false;
  m_PokeModelSystemHeapOption.useColorShader  = true; 

  /*
    2016/05/19  Yu Muto
    GFNMcat[4396]   アプリ内で、ゼルネアス、ホワイトキュレム、ブラックキュレムのモデル表示が「かわいがり待機」になっていない場所があります
      defaultでMODEL_ANIMETYPE_BATTLEとされているものを、MODEL_ANIMETYPE_APPに変更
  */
  m_PokeModelSystemHeapOption.animeType       = PokeTool::MODEL_ANIMETYPE_APP;

#if PM_DEBUG
  m_FrameRect[0]  = m_FrameRect[1]  =  0.0f;
  m_FrameRect[2]  = m_FrameRect[3]  = 16.0f;
#endif
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppPokeModelUtility::~NetAppPokeModelUtility() 
{
  GFL_ASSERT(IsStable());
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::Setup(u32 slots, const InFrameParam* pInFrameParam, const u32 layerNo)
{
  GFL_ASSERT(m_State == STATE_Initial);

  gfl2::heap::HeapBase*   pHeapBase = m_pWorkBase->GetAppHeap()->GetDeviceHeap();

  m_Slots = slots;

  /*  GLHeapAllocator  */
  m_pGLHeapAllocator = GFL_NEW( pHeapBase ) gfl2::util::GLHeapAllocator( pHeapBase );

  /*  modelSys  */
  {
    m_PokeModelSystem = GFL_NEW(pHeapBase)  PokeTool::PokeModelSystem();
    m_PokeModelSystem->InitSystemAsync(pHeapBase, pHeapBase, GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), m_Slots);

    m_pEnvNode  = app::util::AppRenderingManager::CreateDefaultDrawEnv(m_pWorkBase->GetAppHeap());
    app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame(
      m_pEnvNode->GetLightSet( 0 )->GetLightNode( 0 ),
      ::System::Camera::CModelViewerInFrame::TURN_DIRECTION_FREE
   );
  }

  if( pInFrameParam )
  {
    /*  InFrameSystem  */
    {
      m_pInFrameSystem = GFL_NEW(pHeapBase) ::System::Camera::CModelViewerInFrame();
      m_pInFrameSystem->Init( pInFrameParam->screen, pInFrameParam->left, pInFrameParam->top, pInFrameParam->right, pInFrameParam->bottom, pInFrameParam->turnDirection );
      m_pInFrameSystem->SetDrawPosition( pInFrameParam->drawPosition );
    }

    /*  InFrameCamera  */
    {
      m_pInFrameCamera = GFL_NEW(pHeapBase) ::System::Camera::InFrameCamera();
      m_pWorkBase->GetAppRenderingManager()->SetInFrameCamera( pInFrameParam->renderPlace, m_pInFrameCamera, layerNo );
    }
  }

  /*  environments  */
  {
    m_pModelEnvironments  = GFL_NEW_ARRAY(pHeapBase)  ModelEnvironment[m_Slots];
    for(u32 index = 0; index < m_Slots; ++index)
    {
      m_pModelEnvironments[index].Initialize(this, pHeapBase, m_pWorkBase->GetAppRenderingManager(), m_PokeModelSystem, m_pInFrameSystem, m_pInFrameCamera);
    }
  }

#if PM_DEBUG
  {
    m_pPokeModelUtilDebug = GFL_NEW(pHeapBase)  NetAppPokeModelUtilityDebug(this);
  }
#endif


  m_State = STATE_Setup;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::SetFramePosition( int left, int top, int right, int bottom )
{
  if( m_pInFrameSystem )
  {
#if PM_DEBUG
    m_FrameRect[0]  = left;
    m_FrameRect[1]  = top;
    m_FrameRect[2]  = right;
    m_FrameRect[3]  = bottom;

    m_pInFrameSystem->SetFrame(
      left   + m_DebugOffset_OfsX,
      top    + m_DebugOffset_OfsY,
      right  + m_DebugOffset_OfsX + m_DebugOffset_OfsSize,
      bottom + m_DebugOffset_OfsY + m_DebugOffset_OfsSize
    );
#else
    m_pInFrameSystem->SetFrame( left, top, right, bottom );
#endif


    
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::SetShaderColor(const u32 slot, const gfl2::gfx::Color& color)
{
  GFL_ASSERT(slot < m_Slots);

  if( m_pModelEnvironments )
  {
    m_pModelEnvironments[slot].SetShaderColor(color);
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
void  NetAppPokeModelUtility::SetVisibility_InFrame(const u32 slot, const bool isVisible)
{
  GFL_ASSERT(slot < m_Slots);

  if( m_pModelEnvironments )
  {
    m_pModelEnvironments[slot].SetVisibility_InFrame(isVisible);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::SetFramePosition(const gfl2::lyt::LytPane* pPane, const bool isUpper)
{
  if(pPane)
  {
    const f32 halfScreenW = isUpper ? 200.0f : 160.0f;
    const f32 halfScreenH = 120.0f;

    const f32 layoutPosX  = pPane->GetGlobalMtx()._03;
    const f32 layoutPosY  = pPane->GetGlobalMtx()._13;
    const f32 halfSizeW   = pPane->GetSize().width / 2.0f;
    const f32 halfSizeH   = pPane->GetSize().height / 2.0f;
    
    /*  layout座標系のyは上が+  */
    SetFramePosition(
      ( layoutPosX - halfSizeW) + halfScreenW,
      (-layoutPosY - halfSizeH) + halfScreenH,
      ( layoutPosX + halfSizeW) + halfScreenW,
      (-layoutPosY + halfSizeH) + halfScreenH
    );
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::Setup(u32 slots, const PokeTool::PokeModelSystem::HeapOption& rHeapOption, const InFrameParam* pInFrameParam, const u32 layerNo)
{
  m_PokeModelSystemHeapOption = rHeapOption;

//  m_PokeModelSystemHeapOption.useIdModel      = true;
//  m_PokeModelSystemHeapOption.useShadow       = false;
  m_PokeModelSystemHeapOption.useColorShader  = true;   /**<  このフラグは強制  */
  Setup(slots, pInFrameParam, layerNo);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::Cleanup(void)
{
#if PM_DEBUG
  {
    GFL_SAFE_DELETE(m_pPokeModelUtilDebug);
  }
#endif

  if( m_pInFrameSystem )
  {
    m_pInFrameSystem->ResetModel();
  }
  GFL_SAFE_DELETE_ARRAY(m_pModelEnvironments);
  GFL_SAFE_DELETE(m_pInFrameCamera);
  GFL_SAFE_DELETE(m_pInFrameSystem);
  GFL_SAFE_DELETE(m_PokeModelSystem);
  GFL_SAFE_DELETE(m_pEnvNode);
  GFL_SAFE_DELETE(m_pGLHeapAllocator);
  m_Slots = 0;
  m_State = STATE_Initial;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::CleanupRequest(void)
{
  m_IsCleanupRequest  = true;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::RequestEx(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, const u32 slot, const PokeTool::SimpleParam& rPokeToolSimpleParam, u32 layerNo)
{
  GFL_ASSERT(slot < m_Slots);

  if( m_pModelEnvironments )
  {
    m_pModelEnvironments[slot].Request(renderPlace, rPokeToolSimpleParam, m_PokeModelSystemHeapOption.animeType, layerNo);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppPokeModelUtility::ReleaseRequest(const u32 slot)
{
  GFL_ASSERT(slot < m_Slots);

  if( m_pModelEnvironments )
  {
    m_pModelEnvironments[slot].ReleaseRequest();
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
void  NetAppPokeModelUtility::Update(void)
{
  if( m_PokeModelSystem )
  {
    m_PokeModelSystem->Update();
  }

  if( m_pInFrameSystem )
  {
    m_pInFrameSystem->Update( m_pInFrameCamera );
  }

  switch(m_State)
  {
    /*
      初期状態
    */
    case STATE_Initial:
      break;


    case STATE_Setup:
    {
      if(m_PokeModelSystem->IsFinishInitSystemAsync())
      {
        PokeTool::PokeModelSystem::HeapOption heapOption;

        /*  システム準備完了  */
        m_PokeModelSystem->CreateSystem(m_pGLHeapAllocator, true, false);
        m_PokeModelSystem->CreatePokeModelHeap(m_pWorkBase->GetAppHeap()->GetDeviceHeap(), &m_PokeModelSystemHeapOption);
        m_pWorkBase->GetAppRenderingManager()->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pEnvNode);
//        m_pWorkBase->GetAppRenderingManager()->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pEnvNode);
        m_pWorkBase->GetAppRenderingManager()->SetPokeModelSystem(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_PokeModelSystem);
//        m_pWorkBase->GetAppRenderingManager()->SetPokeModelSystem(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_PokeModelSystem);
        m_State = STATE_Idle;
      }
    }
      break;


    /*
      待機中
    */
    case STATE_Idle:
      
      if(m_IsCleanupRequest)
      {
        for(u32 index = 0; index < m_Slots; ++index)
        {
          m_pModelEnvironments[index].ReleaseRequest();
        }
        m_State = STATE_CleanupModels;
      }
      else
      {
#if PM_DEBUG
        m_pPokeModelUtilDebug->OnUpdate();
#endif
        for(u32 index = 0; index < m_Slots; ++index)
        {
          m_pModelEnvironments[index].Update();
        }
      }
      break;


    case  STATE_CleanupModels:
    {
      bool  isFinish  = true;
      
      for(u32 index = 0; index < m_Slots; ++index)
      {
        m_pModelEnvironments[index].Update();
        if(!m_pModelEnvironments[index].IsStable())
        {
          isFinish  = false;
          break;
        }
      }
      
      if(isFinish)
      {
        m_pWorkBase->GetAppRenderingManager()->UnsetPokeModelSystem(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
//        m_pWorkBase->GetAppRenderingManager()->UnsetPokeModelSystem(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
        m_pWorkBase->GetAppRenderingManager()->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pEnvNode);
//        m_pWorkBase->GetAppRenderingManager()->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pEnvNode);
        m_PokeModelSystem->DeletePokeModelHeap();
        m_PokeModelSystem->TermSystemAsync();
        m_State = STATE_Cleanup;
      }
      
    }
      break;
    

    case  STATE_Cleanup:
      if( m_PokeModelSystem->IsFinishTermSystemAsync())
      {
        Cleanup();
      }
      break;
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
void  NetAppPokeModelUtility::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  NetAppPokeModelUtility::DebugDraw(gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pPokeModelUtilDebug->OnDraw(displayNo);
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppPokeModelUtility::SetEventHandler(const u32 slot, IEventHandler* pHandler)
{
  GFL_ASSERT(slot < m_Slots);

  if( m_pModelEnvironments )
  {
    m_pModelEnvironments[slot].SetEventHandler(pHandler);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void NetAppPokeModelUtility::ClearHandlers(void)
{
  for(u32 slot = 0; slot < m_Slots; ++slot)
  {
    SetEventHandler(slot, NULL);
  }
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
app::util::AppRenderingManager::RenderPlaceType::Tag  NetAppPokeModelUtility::GetEnvRenderPlaceType(const u32 slot) const
{
  GFL_ASSERT(slot < m_Slots);
  GFL_ASSERT(m_pModelEnvironments);

  return m_pModelEnvironments[slot].GetRenderPlaceType();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  NetAppPokeModelUtility::GetEnvTargetLayer(const u32 slot) const
{
  GFL_ASSERT(slot < m_Slots);
  GFL_ASSERT(m_pModelEnvironments);

  return m_pModelEnvironments[slot].GetLayerNo();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
u32  NetAppPokeModelUtility::GetFrameRect(const u32 slot, int& rLeft, int& rTop, int& rRight, int& rBottom) const
{
  GFL_ASSERT(slot < m_Slots);
  GFL_ASSERT(m_pModelEnvironments);

  return m_pModelEnvironments[slot].GetLayerNo();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  NetAppPokeModelUtility::SetDebugOffset(const f32 ofsX, const f32 ofsY, const f32 ofsSize)
{
  m_DebugOffset_OfsX    = ofsX;
  m_DebugOffset_OfsY    = ofsY;
  m_DebugOffset_OfsSize = ofsSize;

  SetFramePosition(m_FrameRect[0], m_FrameRect[1], m_FrameRect[2], m_FrameRect[3]);
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  NetAppPokeModelUtility::AddDebugOffset(const f32 addX, const f32 addY, const f32 addSize)
{
  m_DebugOffset_OfsX    += addX;
  m_DebugOffset_OfsY    += addY;
  m_DebugOffset_OfsSize += addSize;

  SetFramePosition(m_FrameRect[0], m_FrameRect[1], m_FrameRect[2], m_FrameRect[3]);
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  NetAppPokeModelUtility::ResetDebugOffset(void)
{
  m_DebugOffset_OfsX    =
  m_DebugOffset_OfsY    =
  m_DebugOffset_OfsSize = 0.0f;

  SetFramePosition(m_FrameRect[0], m_FrameRect[1], m_FrameRect[2], m_FrameRect[3]);
}
#endif










} /*  namespace Util      */
} /*  namespace NetAppLib */
