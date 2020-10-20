
#if !defined(PHOTOVIEWBUILDER_H_INCLUDED)
#define PHOTOVIEWBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewBuilder.h
 * @date   2017/02/07 Tue. 13:16:04
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
#include  <NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h>


/*  NetApp::PhotoView  */
namespace NetApp  {
namespace PhotoView {
  class PhotoViewWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @brief  
  *  @date   2017/02/07 Tue. 13:16:31
  */
//==============================================================================
class PhotoViewBuilder
  : public  NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoViewBuilder );

public:
  //------------------------------------------------------------------
  /**
    *  @func     PhotoViewBuilder
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  PhotoViewBuilder(
    PhotoViewWork* pWork,
    NetAppLib::System::FrameID        startFrameID
  );

  //------------------------------------------------------------------
  /**
    *  @func     ~PhotoViewBuilder
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~PhotoViewBuilder();


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
//  virtual bool                                            IsCreateAppToolTrainerIconRendering();
//  virtual bool                                            IsCreateAppToolTrainerIconRenderingByNPCOnly() const;
//  virtual u32                                             GetTrainerIconNum();
//  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE          GetTrainerIconDrawDisplayType();

  /*  regionName  */
//  virtual bool                                            IsCreateAppCountryRegionNameUtility();

  //------------------------------------------------------------------
  /**
    *  @brief    UIViewマネージャのボタン監視モード
    */
  //------------------------------------------------------------------
  virtual bool                                            IsEnableInputExclusiveKeyMode() { return true; }

private:
  PhotoViewWork*              m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID  m_StartFrameID;   /**<  StartFrameID  */

};


} /*  namespace PhotoView    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWBUILDER_H_INCLUDED)  */

