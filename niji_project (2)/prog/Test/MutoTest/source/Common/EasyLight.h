
#if !defined(EASYLIGHT_H_INCLUDED)
#define EASYLIGHT_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyLight.h
 * @date   2016/09/14 Wed. 11:18:40
 * @author muto_yu
 * @brief  
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
#include  <ui/include/gfl2_UI_DeviceManager.h>

#include  <GameSys/include/GameManager.h>
#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/include/Util/AppRenderingManager.h>


namespace Test  {
namespace MutoTest {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/14 Wed. 11:16:51
  */
//==============================================================================
class EasyLight
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyLight);

public:
  EasyLight(void);
  virtual ~EasyLight();
  



  enum  LightSetID
  {
    LIGHTSET_0,
    LIGHTSET_1,
    LIGHTSET_2,
    
    LIGHTSET_MAX
  };


  enum  LightID
  {
    LIGHT_Ambient,
    LIGHT_Directional_0,
//    LIGHT_Directional_1,

    LIGHT_NUM
  };


  void  Create(app::util::Heap* pUtilHeap, const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
  void  Release(void);
  void  Reset(void);

  /*
  */

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode(void)        {return m_pDrawEnvNode;}
  app::util::AppRenderingManager::RenderPlaceType::Tag      GetRenderPlace(void) const  {return m_RenderPlace;}


protected:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::LightNode*     m_pLights[LIGHTSET_MAX][LIGHT_NUM];

  app::util::Heap*                                            m_pUtilHeap;

  app::util::AppRenderingManager::RenderPlaceType::Tag        m_RenderPlace;
};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(EASYLIGHT_H_INCLUDED)  */
