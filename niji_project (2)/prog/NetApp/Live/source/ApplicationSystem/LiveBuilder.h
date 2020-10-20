#if !defined(LIVEBUILDER_H_INCLUDED)
#define LIVEBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveBuilder.h
 * @date   2015/07/09 Thu. 15:05:34
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/*    includes  */
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"


/*  NetApp::Live  */
namespace NetApp  {
namespace Live {
  class LiveWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  LiveBuilder
  *  @brief  Live大会builder
             - FrameSwitcher
             - FrameFactory
             を生成
  *  @date   2015.05.13 Wed.
  */
//==============================================================================
class LiveBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( LiveBuilder );

public:
  //------------------------------------------------------------------
  /**
    *  @func     LiveBuilder
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LiveBuilder(
    LiveWork* pWork,
    NetAppLib::System::FrameID        startFrameID
  );

  //------------------------------------------------------------------
  /**
    *  @func     ~LiveBuilder
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~LiveBuilder();


  /*
    implement IApplicationSystemBuilder I/F
  */
  virtual NetAppLib::System::IApplicationFrameSwitcher*   CreateApplicationFrameSwitcher(void);
  virtual NetAppLib::System::IApplicationFrameFactory*    CreateApplicationFrameFactory(void);
  
  /*  renderingmanager  */
//  virtual u32                                             CreateAppRenderingManagerCount();
  virtual app::util::AppRenderingManager::Description*    CreateAppRenderingManagerDescription();
//  virtual app::util::AppRenderingManager::Description*    CreateAppRenderingManagerDescriptions();

  /*  work/resource  */
  virtual NetAppLib::System::ApplicationWorkBase*         GetApplicationWork();
  virtual NetAppLib::System::FrameID                      GetApplicationStartFrameID(void);
  virtual u32                                             GetApplicationResourceCount(void);

  /*  messageUtil  */
//  virtual bool                                            IsCreateMessageUtility();
//  virtual void                                            GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen );
//  virtual bool                                            IsCreateNetAppMessageMenu();

  /*  trainerIcon  */
  virtual bool                                            IsCreateAppToolTrainerIconRendering();
//  virtual bool                                            IsCreateAppToolTrainerIconRenderingByNPCOnly() const;
  virtual u32                                             GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE          GetTrainerIconDrawDisplayType();

  /*  regionName  */
//  virtual bool                                            IsCreateAppCountryRegionNameUtility();

  //------------------------------------------------------------------
  /**
    *  @brief    UIViewマネージャのボタン監視モード
    */
  //------------------------------------------------------------------
  virtual bool                                            IsEnableInputExclusiveKeyMode() { return true; }

private:
  LiveWork*   m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID          m_StartFrameID;   /**<  StartFrameID  */

};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEBUILDER_H_INCLUDED)  */

