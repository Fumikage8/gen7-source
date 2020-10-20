
#if !defined(BATTLESPOTBUILDER_H_INCLUDED)
#define BATTLESPOTBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotBuilder.h
 * @date   2015/10/02 Fri. 12:54:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"


namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================



//==============================================================================
/**
  */
//==============================================================================
class BattleSpotBuilder
  : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleSpotBuilder );

public:
  //------------------------------------------------------------------
  /**
    *  @func     BattleSpotBuilder
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  BattleSpotBuilder(
    BattleSpotWork*             pWork,
    NetAppLib::System::FrameID  startFrameID
  );

  //------------------------------------------------------------------
  /**
    *  @func     ~BattleSpotBuilder
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~BattleSpotBuilder();


  /*
    implement IApplicationSystemBuilder I/F
  */
  virtual NetAppLib::System::IApplicationFrameSwitcher*   CreateApplicationFrameSwitcher(void);
  virtual NetAppLib::System::IApplicationFrameFactory*    CreateApplicationFrameFactory(void);
  virtual NetAppLib::System::ApplicationWorkBase*         GetApplicationWork();
  virtual NetAppLib::System::FrameID                      GetApplicationStartFrameID(void);
  virtual u32                                             GetApplicationResourceCount(void);
  virtual app::util::AppRenderingManager::Description*    CreateAppRenderingManagerDescription();
  virtual bool                                            IsCreateMessageUtility();
  /*  trainerIcon  */
  virtual bool                                            IsCreateAppToolTrainerIconRendering();
  virtual u32                                             GetTrainerIconNum();
  virtual app::util::G2DUtil::SETUP_DISPLAY_TYPE          GetTrainerIconDrawDisplayType() { return app::util::G2DUtil::SETUP_LOWER; }


private:
  BattleSpotWork*               m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID    m_StartFrameID;   /**<  StartFrameID  */
};





} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTBUILDER_H_INCLUDED)  */
