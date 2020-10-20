
#if !defined(PHOTOVIEWFRAMEFACTORY_H_INCLUDED)
#define PHOTOVIEWFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewFrameFactory.h
 * @date   2017/02/07 Tue. 13:17:20
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

#include  <NetStatic/NetAppLib/include/System/IApplicationFrameFactory.h>


namespace NetApp  {
namespace PhotoView {
  class PhotoViewWork;
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
  FrameFactory(PhotoViewWork* pWork);

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
  PhotoViewWork*  m_pWork;
};


} /*  namespace PhotoView */
} /*  namespace NetApp      */
#endif  /*  #if !defined(PHOTOVIEWFRAMEFACTORY_H_INCLUDED)  */
