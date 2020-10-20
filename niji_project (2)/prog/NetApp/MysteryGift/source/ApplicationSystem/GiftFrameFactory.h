
#if !defined(GIFTFRAMEFACTORY_H_INCLUDED)
#define GIFTFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrameFactory.h
 * @date   2015/07/31 Fri. 15:12:37
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



namespace NetApp  {
namespace Gift {
  class GiftWork;

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
  FrameFactory(GiftWork* pWork);

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
  GiftWork*  m_pWork;
};


} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAMEFACTORY_H_INCLUDED)  */

