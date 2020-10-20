#if !defined(EASYCAMERA_H_INCLUDED)
#define EASYCAMERA_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyCamera.h
 * @date   2016/09/09 Fri. 15:40:05
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
  *  @date   2016/09/09 Fri. 15:40:57
  */
//==============================================================================
class EasyCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyCamera);

public:
  EasyCamera(void);
  virtual ~EasyCamera();
  
  
  void  Create(app::util::Heap* pUtilHeap, const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
  void  Release(void);

  void  Update(void)  {UpdateCore(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager());}
  void  UpdateCore(gfl2::ui::DeviceManager* pDeviceManager = NULL);

  void  Reset(void);
  void  SetEnable(const bool bEnable) {m_bEnable = bEnable;}




  /*
    move
  */
  void  Move_AroundGazePoint(const f32 hSpeed, const f32 vSpeed);
  void  Move_EyeDirection(const f32 speed);
  void  Move_GazePoint(const f32 hSpeed, const f32 vSpeed);



  /*
  */
  poke_3d::model::BaseCamera*
        GetBaseCamera(void)         {return m_pBaseCamera;}

  app::util::AppRenderingManager::RenderPlaceType::Tag
        GetRenderPlace(void) const  {return m_RenderPlace;}


protected:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pDrawEnvNode;
  poke_3d::model::BaseCamera*                                 m_pBaseCamera;
  app::util::Heap*                                            m_pUtilHeap;

  struct  CamInfo
  {
    gfl2::math::Vector3                                       pos;
    gfl2::math::Vector3                                       at;
    gfl2::math::Vector3                                       rot;
    f32                                                       dist;
  }m_CamInfo;

  app::util::AppRenderingManager::RenderPlaceType::Tag        m_RenderPlace;

  bool                                                        m_bEnable;
};


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(EASYCAMERA_H_INCLUDED)  */
