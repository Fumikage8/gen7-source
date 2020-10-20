
#if !defined(GIFTBUILDER_H_INCLUDED)
#define GIFTBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftBuilder.h
 * @date   2015/07/31 Fri. 15:11:19
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


namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  GiftBuilder
  *  @brief  builder
             - FrameSwitcher
             - FrameFactory
             を生成
  *  @date   2015.05.13 Wed.
  */
//==============================================================================
class GiftBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( GiftBuilder );

public:
  //------------------------------------------------------------------
  /**
    *  @func     GiftBuilder
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  GiftBuilder(
    GiftWork* pWork,
    NetAppLib::System::FrameID        startFrameID
  );

  //------------------------------------------------------------------
  /**
    *  @func     ~GiftBuilder
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~GiftBuilder();


  /*
    implement IApplicationSystemBuilder I/F
  */
  //------------------------------------------------------------------
  /**
    *  @func     CreateApplicationFrameSwitcher
    *  @brief    FrameSwitcherの生成
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::IApplicationFrameSwitcher*   CreateApplicationFrameSwitcher(void);

  //------------------------------------------------------------------
  /**
    *  @func     CreateApplicationFrameFactory
    *  @brief    FrameFactoryの生成
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::IApplicationFrameFactory*    CreateApplicationFrameFactory(void);

  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::ApplicationWorkBase*           GetApplicationWork();

  //------------------------------------------------------------------
  /**
    *  @func     GetApplicationStartFrameID
    *  @brief    StartFrameIDを返す
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::FrameID                      GetApplicationStartFrameID(void);

  //------------------------------------------------------------------
  /**
    *  @func     GetApplicationResourceCount
    *  @brief    リソース数を返す
    */
  //------------------------------------------------------------------
  virtual u32   GetApplicationResourceCount(void);


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescription();


  //------------------------------------------------------------------
  /**
    *  @brief    UIViewマネージャのボタン監視モード
    */
  //------------------------------------------------------------------
  virtual bool IsEnableInputExclusiveKeyMode(){ return true; }


private:
  GiftWork*   m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID          m_StartFrameID;   /**<  StartFrameID  */

};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTBUILDER_H_INCLUDED)  */








