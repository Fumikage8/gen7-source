#if !defined(NETAPPEFFECTBG_H_INCLUDED)
#define NETAPPEFFECTBG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppEffectBG.h
 * @date   2016/01/08 Fri. 19:01:10
 * @author muto_yu
 * @brief  effect(particle)によるBG表現
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <heap/include/gfl2_Heap.h>
#include  <model/include/gfl2_BaseCamera.h>

#include  "GameSys/include/GameManager.h"
#include  "AppLib/include/Util/AppRenderingManager.h"
#include  "AppLib/include/Util/EffectUtil.h"


#include  "../System/ApplicationWorkBase.h"


namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  各種リソースを渡すためのI/F
  *  @date   2016/12/28 Wed. 15:10:11
  */
//==============================================================================
class INetAppEffectBGResourceProvider
{
public:
  virtual app::util::AppRenderingManager* GetRenderingManager(void) = 0;            /**<  renderingManager  */
  virtual app::util::Heap*                GetUtilHeap(void) = 0;                    /**<  eset用HeapBase    */
  virtual void*                           GetResource(const u32 resourceIndex) = 0; /**<  esetデータ        */
  
  virtual ~INetAppEffectBGResourceProvider(){}
};

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/08 Fri. 19:02:18
  */
//==============================================================================
class NetAppEffectBG
  : protected INetAppEffectBGResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppEffectBG);
public:
  /**
    setup前にproviderを指定しておくことで、heapやrenderingmanager、esetデータをproviderから取得される
    未設定であればApplivationWorkBaseから
  */
  void  SetResourceProvider(INetAppEffectBGResourceProvider* pProvider=NULL) {m_pResourceProvider = pProvider ? pProvider : this;}


  NetAppEffectBG(void);
  virtual ~NetAppEffectBG();


  //------------------------------------------------------------------
  /**
    *  @brief     ApplicationWorkBaseを使用しないSetup
    *  @attention SetResourceProviderによるProvider指定が必須
    */
  //------------------------------------------------------------------
  void  Setup(const u32 resourceMax)  {GFL_ASSERT_STOP(m_pResourceProvider != this);  Setup(NULL, NULL, resourceMax);}

  //------------------------------------------------------------------
  /**
    *  @brief   setup
    *  @param   pWorkBase         [in]  ApplicationWorkBase、各種リソースの取得に使用。別途Providerを指定していればNULLで良い。
    *  @param   pResourceIDArray  [in]  ApplicationWorkBase内のResourceManagerからリソースデータを取得するためのResourceIDの配列。Providerを指定している場合はNULLで良い。
    */
  //------------------------------------------------------------------
  void  Setup(NetAppLib::System::ApplicationWorkBase* pWorkBase, u32* pResourceIDArray, u32 resourceMax );
  void  Setup(NetAppLib::System::ApplicationWorkBase* pWorkBase, u32* pResourceIDArray, u32 resourceMax,
    u32* pUpperEffectGroupIDArray, u32 upperMax, u32* pLowerEffectGroupIDArray, u32 lowerMax );
  void  Cleanup(void);
  void  Update(void);

  app::util::EffectUtil&        GetEffectUtil(void)   {return m_EffectUtil;}


  void  SetUpperCamera(
          const f32 posX,         const f32 posY,         const f32 posZ,
          const f32 atX,          const f32 atY,          const f32 atZ,
          const f32 fovDegree =    30.0f,
          const f32 near      =     1.0f,
          const f32 far       = 65535.0f,
          const f32 upX = 0.0f,   const f32 upY = 1.0f,   const f32 upZ = 0.0f
        );


  void  SetLowerCamera(
          const f32 posX,         const f32 posY,         const f32 posZ,
          const f32 atX,          const f32 atY,          const f32 atZ,
          const f32 fovDegree =    30.0f,
          const f32 near      =     1.0f,
          const f32 far       = 65535.0f,
          const f32 upX = 0.0f,   const f32 upY = 1.0f,   const f32 upZ = 0.0f
        );


  poke_3d::model::BaseCamera*   GetUpperCamera(void)  {return m_UpperCameraInfo.pCamera;}
  poke_3d::model::BaseCamera*   GetLowerCamera(void)  {return m_LowerCameraInfo.pCamera;}

  void  SetVisibility(const bool bVisibilityUpper, const bool bVisibilityLower);



  /*  clearcolor  */
  void  PushClearColor(const f32 r = 0.0f, const f32 g = 0.0f, const f32 b = 0.0f, const f32 a = 0.0f);
  void  PopClearColor(void);


protected:
  void  SetClearSetting(const bool isRestoreBackup);
  void  GetClearSetting(void);

  NetAppLib::System::ApplicationWorkBase*   m_pWorkBase;
  app::util::EffectUtil                     m_EffectUtil;
  u32                                       m_EffectLayerNum;
  INetAppEffectBGResourceProvider*          m_pResourceProvider;

  struct  CameraInfo
  {
    NetAppEffectBG*                                             pOwner;
    app::util::AppRenderingManager::RenderPlaceType::Tag        renderPlaceTag;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   pDrawEnvNode;
    poke_3d::model::BaseCamera*                                 pCamera;


    CameraInfo(const app::util::AppRenderingManager::RenderPlaceType::Tag _renderPlaceTag)
      : pOwner(NULL)
      , renderPlaceTag(_renderPlaceTag)
      , pDrawEnvNode(NULL)
      , pCamera(NULL)
    {
    }

    void  Setup(const bool isLower)
    {
      GFL_ASSERT(!pDrawEnvNode);
      GFL_ASSERT(!pCamera);
      
      const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlaceType = isLower ? app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY : app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;

      app::util::AppRenderingManager::CreateDefaultCamera(pOwner->m_pResourceProvider->GetUtilHeap(), &pDrawEnvNode, &pCamera, renderPlaceType);
    }

    void  Cleanup(void)
    {
      GFL_SAFE_DELETE(pDrawEnvNode);
      GFL_SAFE_DELETE(pCamera);
    }

    void  Set(void)
    {
      pOwner->m_pResourceProvider->GetRenderingManager()->SetEffectCamera(renderPlaceTag, pCamera);
    }

    void  Unset(void)
    {
      pOwner->m_pResourceProvider->GetRenderingManager()->UnsetEffectCamera(renderPlaceTag);
    }

    void  SetParam(
            const f32 posX,       const f32 posY, const f32 posZ,
            const f32 atX,        const f32 atY,  const f32 atZ,
            const f32 fovDegree,
            const f32 _near,
            const f32 _far,
            const f32 upX,        const f32 upY,  const f32 upZ
          )
          {
            GFL_ASSERT(pCamera);
            pCamera->SetupCameraLookAt(
              gfl2::math::Vector3(posX, posY, posZ),
              gfl2::math::Vector3(atX,  atY,  atZ),
              gfl2::math::Vector3(upX,  upY,  upZ)
            );
            pCamera->SetFovy(gfl2::math::ConvDegToRad(fovDegree));
            pCamera->SetNear(_near);
            pCamera->SetFar(_far);
          }

  }m_UpperCameraInfo, m_LowerCameraInfo;


  enum  {
    CLEARSETTING_BACKUP,
    CLEARSETTING_MINE,
    
    CLEARSETTING_MAX
    
  };
  GameSys::GameManager::BUFFER_CLEAR_SETTING  m_BufferClearSetting[CLEARSETTING_MAX][gfl2::gfx::CtrDisplayNo::NumberOf];

  u32                                         m_ResourceMax;
  u32*                                        m_pResourceIDArray; /**<  resourceManagerに渡すResourceIDの配列。WorkBase使用時のSetup時のみに使用。  */

  /*
    INetAppEffectBGResourceProvider I/F (default)
  */
  private:
    virtual app::util::AppRenderingManager* GetRenderingManager(void)             { GFL_ASSERT_STOP(m_pWorkBase); return m_pWorkBase->GetAppRenderingManager(); }
    virtual app::util::Heap*                GetUtilHeap(void)                     { GFL_ASSERT_STOP(m_pWorkBase); return m_pWorkBase->GetAppHeap(); }
    virtual void*                           GetResource(const u32 resourceIndex)  { GFL_ASSERT_STOP(m_pWorkBase); return m_pWorkBase->GetResourceManager()->GetResourceBuffer(m_pResourceIDArray[resourceIndex]); }
};




} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPEFFECTBG_H_INCLUDED)  */
