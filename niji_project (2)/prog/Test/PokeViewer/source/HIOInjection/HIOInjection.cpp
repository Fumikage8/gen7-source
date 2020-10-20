#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOInjection.cpp
 * @date   2016/10/04 Tue. 16:49:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <gfx/include/gfl2_PCDebugUtil.h>
#include  <System/include/Screenshot.h>
#include  "./HIOInjection.h"



namespace
{
  /**  HIO通信チャンネル：Host側と合わせる  */
  enum  HIOCh
  {
    HIOCH_Capture,
    HIOCH_Environment,
    HIOCH_Pokemon,
    HIOCH_Others,
    
    HIOCH_MAX
  };

}


namespace test  {
namespace pokeviewer  {
// =============================================================================
/**/
//==============================================================================
class HIOInjection_ImplBase
{
public:
  HIOInjection_ImplBase(HIOInjection* pOwner)
    : m_pOwner(pOwner)
  {}
  virtual ~HIOInjection_ImplBase(){}

  virtual void  Initialize(gfl2::heap::HeapBase* pHeapBase){}
  virtual void  Finalize(void){}
  virtual void  Update(void){}

protected:
  HIOInjection* m_pOwner;
};


#if NETAPP_HIOUTIL_AVAILABLE
  /*
    hio有効時実装
  */
  class HIOInjection::HIOInjection_Impl
    : public    HIOInjection_ImplBase
    , protected NetAppLib::Debug::HIOUtil::IEventHandler
  {
  public:
    HIOInjection_Impl(HIOInjection* pOwner)
      : HIOInjection_ImplBase(pOwner)
      , m_IsCapture(false)
      , m_CaptureState(0)
      , m_ClearColor()
      , m_ShaderColor()
      , m_IsShaderColorEnable(false)
      , m_IsLoad(false)
    {
      m_CaptureImageOutPath[0] =
      m_AlphaImageOutPath[0]   = '\0';
    }

    virtual ~HIOInjection_Impl()
    {
    }

    virtual void  Initialize(gfl2::heap::HeapBase* pHeapBase)
    {
      for(s32 ch = 0; ch < HIOCH_MAX; ++ch)
      {
        m_HIOUtil[ch].Initialize(pHeapBase, ch);
        m_HIOUtil[ch].SetHandler(this);
      }
    }

    virtual void  Finalize(void)
    {
      for(s32 ch = 0; ch < HIOCH_MAX; ++ch)
      {
        m_HIOUtil[ch].Finalize();
      }
    }


    virtual void   Update(void)
    {
      if(m_IsCapture)
      {
        UpdateCapture();
      }
      else if(m_IsLoad)
      {
        UpdateLoad();
      }
      else
      {
        for(s32 ch = 0; ch < HIOCH_MAX; ++ch)
        {
          m_HIOUtil[ch].Update();
        }
      }
    }

    /*  NetAppLib::Debug::HIOUtil::IEventHandler  */
    virtual void  OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID);

    void  ChEventHandling(const s32 ch, const NetAppLib::Debug::HIOUtil::DataInfo& rInfo, void* pData, const u32 dataBytes);

  protected:
    static const u32  OUTPATH_MAX = 1024;

    NetAppLib::Debug::HIOUtil   m_HIOUtil[HIOCH_MAX];
    /*  capture  */
    bool                        m_IsCapture;
    u32                         m_CaptureState;
    char                        m_CaptureImageOutPath[OUTPATH_MAX];
    char                        m_AlphaImageOutPath[OUTPATH_MAX];
    /*  backup  */
    gfl2::gfx::Color            m_ClearColor;
    gfl2::gfx::Color            m_ShaderColor;
    bool                        m_IsShaderColorEnable;
    u32                         m_GuideRectSize;
    
    /*  load  */
    bool                        m_IsLoad;
    u32                         m_LoadState;
    
    void  StartCapture(void)
    {
      m_IsCapture     = true;
      m_CaptureState  = 0;
    }

    void  UpdateCapture(void)
    {
      switch(m_CaptureState)
      {
        case  0:
        {
          m_ClearColor          = m_pOwner->m_pModelManager->GetClearColor();
          m_ShaderColor         = m_pOwner->m_pModelManager->GetPokeShaderColor();
          m_IsShaderColorEnable = m_pOwner->m_pModelManager->IsPokeShaderEnable();
          m_GuideRectSize       = m_pOwner->m_pModelManager->GetGuideRectSize();
          m_pOwner->m_pModelManager->SetGuideRectSize(0);
        }
          break;
          
        case  3:
          ScreenShot(m_CaptureImageOutPath);  /*  color  */
          break;


        case  4:
          m_pOwner->m_pModelManager->SetClearColor(gfl2::gfx::Color(1,1,1));
          m_pOwner->m_pModelManager->SetPokeShaderColor(gfl2::gfx::Color(0,0,0));
          m_pOwner->m_pModelManager->SetPokeShaderEnable(true);
          break;

        case  7:
          ScreenShot(m_AlphaImageOutPath);  /*  alpha  */
          /*  restore  */
          m_pOwner->m_pModelManager->SetClearColor(m_ClearColor);
          m_pOwner->m_pModelManager->SetPokeShaderColor(m_ShaderColor);
          m_pOwner->m_pModelManager->SetPokeShaderEnable(m_IsShaderColorEnable);
          m_pOwner->m_pModelManager->SetGuideRectSize(m_GuideRectSize);
          
          m_IsCapture = false;
          break;

        default:
          break;
      }

      ++m_CaptureState;
    }
    
    void ScreenShot(const char* pOutPath)
    {
      const gfl2::gfx::DisplayMode  displayMode = gfl2::gfx::DisplayMode::CTR_LEFT;
      const u32                     displayNo   = displayMode - gfl2::gfx::DisplayMode::CTR_LEFT;
      gfl2::heap::HeapBase*         pHeapBase   = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);

      gfl2::gfx::PCDebugUtil::PrintScreen(pOutPath, System::GflUse::m_HandleList[displayNo], pHeapBase);
    }
    
    
    
    void  StartLoad(void)
    {
      m_IsLoad    = true;
    }
    
    void  UpdateLoad(void)
    {
      if(m_pOwner->m_pModelManager->IsStable()) m_IsLoad = false;
    }
  };


#else
  /*
    hio無効時実装
  */
  class HIOInjection::HIOInjection_Impl
    : public  HIOInjection_ImplBase
  {
  public:
    HIOInjection_Impl(HIOInjection* pOwner)
      : HIOInjection_ImplBase(pOwner)
    {
    }
  };

#endif



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
HIOInjection::HIOInjection(void)
  : m_pModelManager(NULL)
  , m_pImpl(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
HIOInjection::~HIOInjection()
{
  Finalize();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  HIOInjection::Initialize(gfl2::heap::HeapBase* pHeapBase, PokeViewerPokeModelManager* pModelManager)
{
  GFL_ASSERT(!m_pImpl);

  SetModelManager(pModelManager);
  m_pImpl = GFL_NEW(pHeapBase)  HIOInjection_Impl(this);
  m_pImpl->Initialize(pHeapBase);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  HIOInjection::Finalize(void)
{
  m_pImpl->Finalize();
  GFL_SAFE_DELETE(m_pImpl);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void   HIOInjection::Update(void)
{
  if(m_pImpl) m_pImpl->Update();
}



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
#if NETAPP_HIOUTIL_AVAILABLE
void  HIOInjection::HIOInjection_Impl::OnEvent(NetAppLib::Debug::HIOUtil* pSender, const NetAppLib::Debug::HIOUtil::IEventHandler::EventID eventID)
{
  switch(eventID)
  {
    case  NetAppLib::Debug::HIOUtil::IEventHandler::EVENT_ReadEnd:
    {
      const NetAppLib::Debug::HIOUtil::DataInfo&  rInfo = pSender->GetDataInfo();
      u32   dataBytes;
      void* pData = pSender->GetData(&dataBytes);
      
      ChEventHandling(pSender->GetCh(), rInfo, pData, dataBytes);
    }
      break;
      
    default:
      break;
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
#if NETAPP_HIOUTIL_AVAILABLE
void  HIOInjection::HIOInjection_Impl::ChEventHandling(const s32 ch, const NetAppLib::Debug::HIOUtil::DataInfo& rInfo, void* pData, const u32 dataBytes)
{
  GFL_PRINT("ChEventHandling: ch(%d), %u[B]\n", ch, dataBytes);

  switch(ch)
  {
    case  HIOCH_Environment:
      if(m_pOwner->m_pModelManager)
      {
        /*
            userData[index++] = (ClearColor.B << 16) | (ClearColor.G << 8) | (ClearColor.R << 0);


            userData[index++] = CameraMode;

            userData[index++] = FX20_12.ToFx20_12(CameraX).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraY).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraZ).Row;

            userData[index++] = FX20_12.ToFx20_12(CameraLookAtX).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLookAtY).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLookAtZ).Row;

            userData[index++] = FX20_12.ToFx20_12(CameraLongitude).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraLatitude).Row;
            userData[index++] = FX20_12.ToFx20_12(CameraPolarCoordRadius).Row;

            userData[index++] = CameraFOV;

            userData[index++] = FX20_12.ToFx20_12(LightLongitude).Row;
            userData[index++] = FX20_12.ToFx20_12(LightLatitude).Row;

            userData[index++] = EnableShadow ? 1 : 0;

            userData[index++] = MotionIndex;
            userData[index++] = MotionFrame;
        */
        
        
        /*  clearColor  */
        {
          u8 r  = ((rInfo.userdata[0] & 0x000000ff) >>  0);
          u8 g  = ((rInfo.userdata[0] & 0x0000ff00) >>  8);
          u8 b  = ((rInfo.userdata[0] & 0x00ff0000) >> 16);

          gfl2::gfx::Color  clearColor;

          clearColor.SetR((f32)r / 255.0f);
          clearColor.SetG((f32)g / 255.0f);
          clearColor.SetB((f32)b / 255.0f);

          
          m_pOwner->m_pModelManager->SetClearColor(clearColor);
        }
        

        {
          gfl2::math::Vector3 vec;
          u32                 index;

          index = 2;

          {
            /*  camera  */
            vec.x = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            vec.y = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            vec.z = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            //          m_pOwner->m_pModelManager->SetCameraPosition(vec);

            vec.x = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            vec.y = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            vec.z = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            m_pOwner->m_pModelManager->SetCameraTarget(vec);


            vec.y = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]); /*  longitude  */
            vec.x = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]); /*  latitude   */
            vec.z = 0.0f;
            m_pOwner->m_pModelManager->SetCameraRotation(vec);

            m_pOwner->m_pModelManager->SetCameraPolarCoodRadius(NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]));

            {
              s32 fov = rInfo.userdata[index++];
              m_pOwner->m_pModelManager->SetCameraFovy(fov);
            }
          }
          {
            /*  light  */
            const f32   longitude = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            const f32   latitude  = NetAppLib::Debug::HIOUtil::Fx20_12ToF32(rInfo.userdata[index++]);
            const bool  shadow    = (rInfo.userdata[index++] != 0);

            m_pOwner->m_pModelManager->SetLightRotation(latitude, longitude);
            /*  shadow  */
            m_pOwner->m_pModelManager->SetShadowVisibility(shadow);
          }
          {
            /*  mot  */
            const s32   motionIndex         = rInfo.userdata[index++];
            const s32   motionFrame         = rInfo.userdata[index++];

            m_pOwner->m_pModelManager->SetMotionIndex(motionIndex);
            m_pOwner->m_pModelManager->PlayMotion(false);
            m_pOwner->m_pModelManager->SetMotionFrame(motionFrame);
          }
        }
      }
      break;


    case  HIOCH_Pokemon:
    {
      struct 
      {
        s32 monsNo;
        s32 formNo;
        s32 gender;
        s32 isRare;
      }cmdPoke;

      if(m_pOwner->m_pModelManager)
      {
        cmdPoke.monsNo  = rInfo.userdata[0];
        cmdPoke.formNo  = rInfo.userdata[1];
        cmdPoke.gender  = rInfo.userdata[2];
        cmdPoke.isRare  = rInfo.userdata[3];
        
        m_pOwner->m_pModelManager->SetMonsNo(cmdPoke.monsNo);
        m_pOwner->m_pModelManager->ChangeFormNo(cmdPoke.formNo - m_pOwner->m_pModelManager->GetFormNo());    /*  change = add  */

        if(cmdPoke.gender != m_pOwner->m_pModelManager->GetSex())               m_pOwner->m_pModelManager->ToggleSex();
        if((cmdPoke.isRare != 0) != (m_pOwner->m_pModelManager->IsRare() != 0)) m_pOwner->m_pModelManager->ToggleRare();
        
        m_pOwner->m_pModelManager->LoadPokeModel();
        StartLoad();
      }

    }
      break;


    case  HIOCH_Capture:
    {
      /*  "<CaptureImageOutPath>,<m_AlphaImageOutPath>"  */
      const char  delim         = ',';
      const char* pAlphaOutPath = NULL;
      char*       pCurrent      = static_cast<char*>(pData);

      for(u32 index = 0; index < (dataBytes-1); ++index)
      {
        if(*pCurrent == delim)
        {
          *pCurrent = '\0';
          pAlphaOutPath = pCurrent+1;
          break;
        }
        
        ++pCurrent;
      }

      if(pAlphaOutPath)
      {
        nw::ut::strcpy(m_CaptureImageOutPath, GFL_NELEMS(m_CaptureImageOutPath),  (const char*)pData);
        nw::ut::strcpy(m_AlphaImageOutPath,   GFL_NELEMS(m_AlphaImageOutPath),    pAlphaOutPath);
        
        GFL_PRINT("%s\n", m_CaptureImageOutPath);
        GFL_PRINT("%s\n", m_AlphaImageOutPath);
        StartCapture();
      }
      else
      {
        GFL_PRINT("HIOCH_Capture: invalid data\n");
        GFL_PRINT("=>%s\n", m_CaptureImageOutPath);
        GFL_PRINT("=>%s\n", m_AlphaImageOutPath);
      }
      
      
      GflHeapSafeFreeMemory(pData);
    }
      break;

    /*    */
    case  HIOCH_Others:
    {
      /*
            userData[index++] = GuideGridSize;
      */
      const u32 guideGridSize = rInfo.userdata[0];
      
      
      m_pOwner->m_pModelManager->SetGuideRectSize(guideGridSize);
    }
      break;


    default:
      break;
  }
}
#endif



} /*  namespace pokevierwer  */
} /*  namespace test  */
#endif // PM_DEBUG
