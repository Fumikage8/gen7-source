#if !defined(EGGHATCHINGFRAMEFACTORY_H_INCLUDED)
#define EGGHATCHINGFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingFrameFactory.h
 * @date   2015/11/24 Tue. 12:35:42
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
namespace EggHatching {
  class EggHatchingWork;
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
  FrameFactory(EggHatchingWork* pWork);

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
  EggHatchingWork*  m_pWork;
};



} /*  namespace EggHatching */
} /*  namespace NetApp      */



#endif  /*  #if !defined(EGGHATCHINGFRAMEFACTORY_H_INCLUDED)  */
