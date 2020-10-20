
#if !defined(EVOLUTIONBUILDER_H_INCLUDED)
#define EVOLUTIONBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionBuilder.h
 * @date   2015/08/28 Fri. 17:55:05
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


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================



//==============================================================================
/**
  *  @class  EvolutionBuilder
  *  @brief  Evolution大会builder
             - FrameSwitcher
             - FrameFactory
             を生成
  *  @date   2015.05.13 Wed.
  */
//==============================================================================
class EvolutionBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( EvolutionBuilder );

public:
  //------------------------------------------------------------------
  /**
    *  @func     EvolutionBuilder
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  EvolutionBuilder(
    EvolutionWork*              pWork,
    NetAppLib::System::FrameID  startFrameID
  );

  //------------------------------------------------------------------
  /**
    *  @func     ~EvolutionBuilder
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~EvolutionBuilder();


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
  virtual app::util::AppRenderingManager::Description*    CreateAppRenderingManagerDescription();


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::ApplicationWorkBase*         GetApplicationWork();

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


private:
  EvolutionWork*                m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID    m_StartFrameID;   /**<  StartFrameID  */

};





} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONBUILDER_H_INCLUDED)  */
