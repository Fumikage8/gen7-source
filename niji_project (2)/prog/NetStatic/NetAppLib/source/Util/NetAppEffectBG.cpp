//==============================================================================
/**
 * @file   NetAppEffectBG.cpp
 * @date   2016/01/12 Tue. 11:55:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/Util/NetAppEffectBG.h"


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppEffectBG::NetAppEffectBG(void)
  : m_pWorkBase(NULL)
  , m_EffectUtil()
  , m_EffectLayerNum(0)
  , m_pResourceProvider(this)
  , m_UpperCameraInfo(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY)
  , m_LowerCameraInfo(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY)
  , m_ResourceMax( 0 )
  , m_pResourceIDArray(NULL)
{
  m_UpperCameraInfo.pOwner = this;
  m_LowerCameraInfo.pOwner = this;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
NetAppEffectBG::~NetAppEffectBG()
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::Setup(NetAppLib::System::ApplicationWorkBase* pWorkBase, u32* pResourceIDArray, u32 resourceMax)
{
  u32                     effectGroupIDNumMaxArray[1] = { resourceMax };
  gfl2::heap::HeapBase*   pHeapBase = NULL;

  m_pWorkBase         = pWorkBase;
  m_ResourceMax       = resourceMax;
  m_pResourceIDArray  = pResourceIDArray;
  m_EffectLayerNum    = 1;

  pHeapBase = m_pResourceProvider->GetUtilHeap()->GetDeviceHeap();

  /*  effect  */
  {
    app::util::EffectUtil::Description  desc;

    desc.layerNum                 = m_EffectLayerNum;
    desc.effectResNumMax          = resourceMax;
    desc.mode                     = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
    desc.effectGroupIDNumMaxArray = effectGroupIDNumMaxArray;

    m_EffectUtil.Create(pHeapBase, desc);

    for( u32 i = 0; i < resourceMax; ++i )
    {
      void* pEffectResource = m_pResourceProvider->GetResource(i);
      m_EffectUtil.EffectSystemEntryResource(pEffectResource, i);
      m_EffectUtil.GetData(0)->effectGroupIDList->PushFront(i);
    }

    m_EffectUtil.CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();

    m_pResourceProvider->GetRenderingManager()->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_EffectUtil.GetData(0)->effectRenderPathConfig);
    m_pResourceProvider->GetRenderingManager()->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_EffectUtil.GetData(0)->effectRenderPathConfig);
  }

  /*  camera  */
  {
    m_UpperCameraInfo.Setup(false);
    m_LowerCameraInfo.Setup(true);
    m_UpperCameraInfo.Set();
    m_LowerCameraInfo.Set();
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::Setup(NetAppLib::System::ApplicationWorkBase* pWorkBase, u32* pResourceIDArray, u32 resourceMax,
                            u32* pUpperEffectGroupIDArray, u32 upperMax, u32* pLowerEffectGroupIDArray, u32 lowerMax )
{
  u32                     effectGroupIDNumMaxArray[2] = { upperMax, lowerMax };
  gfl2::heap::HeapBase*   pHeapBase = NULL;

  m_pWorkBase         = pWorkBase;
  m_ResourceMax       = resourceMax;
  m_pResourceIDArray  = pResourceIDArray;
  m_EffectLayerNum    = 2;

  pHeapBase = m_pResourceProvider->GetUtilHeap()->GetDeviceHeap();

  /*  effect  */
  {
    app::util::EffectUtil::Description  desc;

    desc.layerNum                 = m_EffectLayerNum;
    desc.effectResNumMax          = resourceMax;
    desc.mode                     = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
    desc.effectGroupIDNumMaxArray = effectGroupIDNumMaxArray;

    m_EffectUtil.Create(pHeapBase, desc);

    for( u32 i = 0; i < resourceMax; ++i )
    {
      void* pEffectResource = m_pResourceProvider->GetResource(i);
      m_EffectUtil.EffectSystemEntryResource(pEffectResource, i);
    }

    for( u32 i = 0; i < upperMax; ++i )
    {
      m_EffectUtil.GetData(0)->effectGroupIDList->PushFront( pUpperEffectGroupIDArray[i] );
    }

    for( u32 i = 0; i < lowerMax; ++i )
    {
      m_EffectUtil.GetData(1)->effectGroupIDList->PushFront( pLowerEffectGroupIDArray[i] );
    }


    m_EffectUtil.CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();

    m_pResourceProvider->GetRenderingManager()->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_EffectUtil.GetData(0)->effectRenderPathConfig);
    m_pResourceProvider->GetRenderingManager()->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_EffectUtil.GetData(1)->effectRenderPathConfig);
  }

  /*  camera  */
  {
    m_UpperCameraInfo.Setup(false);
    m_LowerCameraInfo.Setup(true);
    m_UpperCameraInfo.Set();
    m_LowerCameraInfo.Set();
  }

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::Cleanup(void)
{
  m_pResourceProvider->GetRenderingManager()->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
  m_pResourceProvider->GetRenderingManager()->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);

  /*  camera  */
  {
    m_UpperCameraInfo.Unset();
    m_LowerCameraInfo.Unset();
    m_UpperCameraInfo.Cleanup();
    m_LowerCameraInfo.Cleanup();
  }

  m_EffectUtil.KillAllEmitterSetForMode1Res1EmitterSet1Group();
  for( u32 i = 0; i < m_ResourceMax; ++i )
  {
    m_EffectUtil.EffectSystemClearResource(i);
  }
  m_EffectUtil.Destroy();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::Update(void)
{
  m_EffectUtil.UpdateForMode1Res1EmitterSet1Group();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  NetAppEffectBG::SetUpperCamera(
  const f32 posX,  const f32 posY,  const f32 posZ,
  const f32 atX,   const f32 atY,   const f32 atZ,
  const f32 fovDegree,
  const f32 _near,
  const f32 _far,
  const f32 upX,   const f32 upY,   const f32 upZ
)
{
  m_UpperCameraInfo.SetParam(
    posX, posY, posZ,
    atX,  atY,  atZ,
    fovDegree,
    _near,
    _far,
    upX,  upY,  upZ
  );
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::SetLowerCamera(
  const f32 posX,  const f32 posY,  const f32 posZ,
  const f32 atX,   const f32 atY,   const f32 atZ,
  const f32 fovDegree,
  const f32 _near     ,
  const f32 _far      ,
  const f32 upX,   const f32 upY,   const f32 upZ
)
{
  m_LowerCameraInfo.SetParam(
    posX, posY, posZ,
    atX,  atY,  atZ,
    fovDegree,
    _near,
    _far,
    upX,  upY,  upZ
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::SetVisibility(const bool bVisibilityUpper, const bool bVisibilityLower)
{
  const app::util::AppRenderingManager::RenderPlaceType::Tag  tagUpper        = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
  const app::util::AppRenderingManager::RenderPlaceType::Tag  tagLower        = app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY;
  app::util::AppRenderingManager*                             pRenderManager  = m_pResourceProvider->GetRenderingManager();
  const u32                                                   upperDataIndex  = 0;
  const u32                                                   lowerDataIndex  = (m_EffectLayerNum == 2) ? 1 : 0;

  if(bVisibilityUpper)  pRenderManager->SetEffectRenderPathConfig(tagUpper, m_EffectUtil.GetData(upperDataIndex)->effectRenderPathConfig);  else  pRenderManager->UnsetEffectRenderPathConfig(tagUpper);
  if(bVisibilityLower)  pRenderManager->SetEffectRenderPathConfig(tagLower, m_EffectUtil.GetData(lowerDataIndex)->effectRenderPathConfig);  else  pRenderManager->UnsetEffectRenderPathConfig(tagLower);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::PushClearColor(const f32 r, const f32 g, const f32 b, const f32 a)
{
  const gfl2::gfx::Color  col(r ,g, b, a);
  
  GetClearSetting();
  
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::LEFT].color  = col;
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::RIGHT].color = col;
  m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::DOWN].color  = col;

  SetClearSetting(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::PopClearColor(void)
{
  SetClearSetting(true);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::SetClearSetting(const bool isRestoreBackup)
{
  const u32 settingIndex  = isRestoreBackup ? CLEARSETTING_BACKUP : CLEARSETTING_MINE;
  
  GameSys::GameManager* pGame = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::LEFT]);
  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::RIGHT]);
  pGame->SetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  m_BufferClearSetting[settingIndex][gfl2::gfx::CtrDisplayNo::DOWN]);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  NetAppEffectBG::GetClearSetting(void)
{
  GameSys::GameManager* pGame = GFL_SINGLETON_INSTANCE(GameSys::GameManager);

  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::LEFT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::RIGHT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  &(m_BufferClearSetting[CLEARSETTING_BACKUP][gfl2::gfx::CtrDisplayNo::DOWN]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::LEFT,  &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::LEFT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::RIGHT, &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::RIGHT]));
  pGame->GetBufferClearSetting( gfl2::gfx::CtrDisplayNo::DOWN,  &(m_BufferClearSetting[CLEARSETTING_MINE][gfl2::gfx::CtrDisplayNo::DOWN]));
}





} /*  namespace Util      */
} /*  namespace NetAppLib */
