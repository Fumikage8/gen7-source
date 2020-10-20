#if !defined(NETAPPPOKEMODELUTILITY_H_INCLUDED)
#define NETAPPPOKEMODELUTILITY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppPokeModelUtility.h
 * @date   2015/08/06 Thu. 14:51:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <macro/include/gfl2_macros.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <Layout/include/gfl2_Layout.h>

#if PM_DEBUG
  #include <Debug/DebugWin/include/DebugWinSystem.h>
  #include <Debug/include/gfl2_DebugWinSystem.h>
  #include <Debug/include/gfl2_DebugWinItemTypes.h>
#endif


#include "PokeTool/include/PokeTool.h"
#include "PokeTool/include/PokeModelSystem.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include "System/include/Camera/ModelViewerInFrame.h"
#include "System/include/Camera/InFrameCamera.h"



namespace NetAppLib {

  namespace System  {
    class ApplicationWorkBase;
  }


namespace Util      {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/08/06 Thu. 15:00:31
  */
//==============================================================================
class NetAppPokeModelUtility
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokeModelUtility);

public:

  enum  State
  {
    STATE_Initial,            /**<  初期状態        */
    STATE_Setup,              /**<  準備中          */
    STATE_Idle,               /**<  動作中          */
    STATE_CleanupModels,      /**<  モデル解体中    */
    STATE_Cleanup,            /**<  システム解体中  */

    STATE_
  };

  typedef struct
  {
    ::System::Camera::CModelViewerInFrame::EScreen           screen;
    int                                                      left;
    int                                                      top;
    int                                                      right;
    int                                                      bottom;
    ::System::Camera::CModelViewerInFrame::ETurnDirection    turnDirection;
    ::System::Camera::CModelViewerInFrame::EDrawPosition     drawPosition;
    app::util::AppRenderingManager::RenderPlaceType::Tag     renderPlace;
  } InFrameParam;


  NetAppPokeModelUtility(System::ApplicationWorkBase* pWorkBase);
  virtual ~NetAppPokeModelUtility();


#if PM_DEBUG
  /**
    debugWinGroupの登録先を指定
    debugWinGroupの生存期間はSetup～Cleanup
  */
  void  SetParentDebugWinGroup(gfl2::debug::DebugWinGroup* pParentGroup = NULL)  {m_pParentDebugWinGroup = pParentGroup;}
#endif



  void  Setup(u32 slots, const InFrameParam* pInFrameParam = NULL, const u32 layerNo = 0);
  void  Setup(u32 slots, const PokeTool::PokeModelSystem::HeapOption& rHeapOption, const InFrameParam* pInFrameParam = NULL, const u32 layerNo = 0);

  void  CleanupRequest(void);


  /*
  void  Request(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, const u32 slot, const MonsNo monsNo, const u32 layerNo = 0)
        {
          PokeTool::SimpleParam param;
          
          param.monsNo  = monsNo;
          RequestEx(renderPlace, slot, param, layerNo);
        }
        */
  void  RequestEx(const app::util::AppRenderingManager::RenderPlaceType::Tag renderPlace, const u32 slot, const PokeTool::SimpleParam& rPokeToolSimpleParam, u32 layerNo = 0);
  void  ReleaseRequest(const u32 slot);


  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);

#if PM_DEBUG
  virtual void  DebugDraw(gfl2::gfx::CtrDisplayNo displayNo);
  void          SetDebugOffset(const f32 ofsX, const f32 ofsY, const f32 ofsSize);
  void          AddDebugOffset(const f32 addX, const f32 addY, const f32 addSize);
  void          ResetDebugOffset(void);
  void          GetDebugOffset(f32& rOfsX, f32& rOfsY, f32& rOfsSize) const {rOfsX  = m_DebugOffset_OfsX; rOfsY = m_DebugOffset_OfsY; rOfsSize  = m_DebugOffset_OfsSize;}
#endif

  void  SetFramePosition( int left, int top, int right, int bottom );
  void  SetFramePosition(const gfl2::lyt::LytPane* pPane, const bool isUpper = true);


  bool  IsStable(void) const  {return (m_State == STATE_Initial);}

  void  SetShaderColor(const u32 slot, const gfl2::gfx::Color& color);


  //------------------------------------------------------------------
  /**
    *  @brief    CModelViewerInFrameに対するVisibility設定
    */
  //------------------------------------------------------------------
  void  SetVisibility_InFrame(const u32 slot, const bool isVisible);

  /*
    event
  */
  //==============================================================================
  /**
    *  @brief  
    *  @date   2016/02/19 Fri. 11:27:09
    */
  //==============================================================================
  class IEventHandler
  {
  public:
    enum  EventID
    {
      EVENT_OnModelLoded,
   
      EVENT_
    };

    virtual void  NetAppPokeModelUtility_OnEvent(NetAppPokeModelUtility* pSender, const EventID eventID, PokeTool::PokeModel* pPokeModel) = 0;
    virtual ~IEventHandler()  {}
  };
  void SetEventHandler(const u32 slot, IEventHandler* pHandler);
  void ClearHandlers(void);




protected:
  void  Cleanup(void);
  void  CreateModel();
  void  ReleaseModel();

  app::util::AppRenderingManager::RenderPlaceType::Tag  GetEnvRenderPlaceType(const u32 slot) const;
  u32  GetEnvTargetLayer(const u32 slot) const;
  u32  GetFrameRect(const u32 slot, int& rLeft, int& rTop, int& rRight, int& rBottom) const;



  class ModelEnvironment;

  System::ApplicationWorkBase*            m_pWorkBase;
  PokeTool::PokeModelSystem*              m_PokeModelSystem;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pEnvNode;

  ModelEnvironment*                       m_pModelEnvironments;
  State                                   m_State;
  u32                                     m_Slots;
  bool                                    m_IsCleanupRequest;

  PokeTool::PokeModelSystem::HeapOption   m_PokeModelSystemHeapOption;
  gfl2::util::GLHeapAllocator*            m_pGLHeapAllocator;

  ::System::Camera::CModelViewerInFrame*  m_pInFrameSystem;
  ::System::Camera::InFrameCamera*        m_pInFrameCamera;

#if PM_DEBUG
  class NetAppPokeModelUtilityDebug;
  NetAppPokeModelUtilityDebug*            m_pPokeModelUtilDebug;
  gfl2::debug::DebugWinGroup*             m_pParentDebugWinGroup;
  f32                                     m_FrameRect[4];
  f32                                     m_DebugOffset_OfsX;
  f32                                     m_DebugOffset_OfsY;
  f32                                     m_DebugOffset_OfsSize;
#endif
};



} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPPOKEMODELUTILITY_H_INCLUDED)  */
