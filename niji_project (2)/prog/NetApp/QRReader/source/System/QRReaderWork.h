
#if !defined(QRREADERWORK_H_INCLUDED)
#define QRREADERWORK_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderWork.h
 * @date   2016/01/14 Thu. 17:43:32
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
#include  <str/include/gfl2_Str.h>
#include  <arc_index/message.gaix>
#include  <NetStatic/NetAppLib/include/System/ApplicationWorkBase.h>
#include  <NetStatic/NetAppLib/include/Message/NetAppMessageDataUtility.h>
#include  <NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h>

#include  "AppLib/include/Tool/AppCommonGrpIconData.h"

#include  "../../include/QRReaderProcParam.h"
#include  "../Views/QRReaderViewContainer.h"


namespace NetApp    {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class QRReaderWork
  : public  NetAppLib::System::ApplicationWorkBase
{
  GFL_FORBID_COPY_AND_ASSIGN( QRReaderWork );

public:
  QRReaderWork(void);
  virtual ~QRReaderWork();


  /*
    members
  */
  APP_PARAM*                                m_pAppParam;
  QRReaderViewContainer*                    m_pViewContainer;
  NetAppLib::Util::NetAppPokeModelUtility*  m_pPokeModelUtil;
  bool                                      m_IsNeedCannotChargeConfirmIfFieldScanIsActive;
  bool                                      m_IsTransitionFromFieldScanFrame;


  app::tool::AppCommonGrpIconData*          m_pCommonIconData;

private:
};



} /*  namespace QRReader */
} /*  namespace NetApp      */
#endif  /*  #if !defined(QRREADERWORK_H_INCLUDED)  */
