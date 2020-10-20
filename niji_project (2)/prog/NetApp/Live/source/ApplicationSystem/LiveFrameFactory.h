#if !defined(LIVEFRAMEFACTORY_H_INCLUDED)
#define LIVEFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveFrameFactory.h
 * @date   2015/07/09 Thu. 15:08:18
 * @author muto_yu
 * @brief  Live大会FrameFactory
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


namespace NetApp  {
namespace Live {
  class LiveWork;

// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  */
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
  FrameFactory(LiveWork* pWork);

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
  LiveWork*  m_pWork;
};









} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LIVEFRAMEFACTORY_H_INCLUDED)  */
