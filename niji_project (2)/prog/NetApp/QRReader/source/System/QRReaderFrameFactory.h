

#if !defined(QRREADERFRAMEFACTORY_H_INCLUDED)
#define QRREADERFRAMEFACTORY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrameFactory.h
 * @date   2016/01/14 Thu. 17:15:36
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
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================
class FrameFactory
  : public NetAppLib::System::IApplicationFrameFactory
{
  GFL_FORBID_COPY_AND_ASSIGN( FrameFactory );

public:
  FrameFactory(QRReaderWork* pWork);
  virtual ~FrameFactory();

  /*
    implement IApplciationFrameFactory
  */
  virtual NetAppLib::System::NetAppFrameBase*   CreateNetAppFrame(NetAppLib::System::FrameID  frameID);

private:
  QRReaderWork*  m_pWork;
};



} /*  namespace QRReader */
} /*  namespace NetApp   */



#endif  /*  #if !defined(QRREADERFRAMEFACTORY_H_INCLUDED)  */
