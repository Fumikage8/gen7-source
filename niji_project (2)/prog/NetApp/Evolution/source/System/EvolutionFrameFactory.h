
#if !defined(EVOLUTIONFRAMEFACTORY_H_INCLUDED)
#define EVOLUTIONFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrameFactory.h
 * @date   2015/08/28 Fri. 17:56:49
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

#include  "NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h"


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================
class FrameFactory
  : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameFactory );

public:

  //------------------------------------------------------------------
  /**
    *  @func     FrameFactory
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  FrameFactory(EvolutionWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~FrameFactory
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~FrameFactory();

  /*
    implement IApplciationFrameFactory
  */
  //------------------------------------------------------------------
  /**
    *  @func     CreateFrame
    *  @brief    Frameの生成
    */
  //------------------------------------------------------------------
  virtual NetAppLib::System::NetAppFrameBase*   CreateNetAppFrame(NetAppLib::System::FrameID  frameID);

private:
  EvolutionWork*  m_pWork;
};



} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAMEFACTORY_H_INCLUDED)  */
