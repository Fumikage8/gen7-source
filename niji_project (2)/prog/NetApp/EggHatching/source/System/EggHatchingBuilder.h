
#if !defined(EGGHATCHINGBUILDER_H_INCLUDED)
#define EGGHATCHINGBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingBuilder.h
 * @date   2015/11/24 Tue. 12:33:33
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
namespace EggHatching {
  class EggHatchingWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class EggHatchingBuilder : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( EggHatchingBuilder );

public:
  EggHatchingBuilder(
    EggHatchingWork*            pWork,
    NetAppLib::System::FrameID  startFrameID
  );
  virtual ~EggHatchingBuilder();


  /*
    implement IApplicationSystemBuilder I/F
  */
  virtual NetAppLib::System::IApplicationFrameSwitcher*   CreateApplicationFrameSwitcher(void);
  virtual NetAppLib::System::IApplicationFrameFactory*    CreateApplicationFrameFactory(void);
  virtual NetAppLib::System::ApplicationWorkBase*         GetApplicationWork();
  virtual NetAppLib::System::FrameID                      GetApplicationStartFrameID(void);
  virtual u32                                             GetApplicationResourceCount(void);


private:
  EggHatchingWork*              m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID    m_StartFrameID;   /**<  StartFrameID  */

};


} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGBUILDER_H_INCLUDED)  */
