
//==============================================================================
/**
 * @file   QRReaderWork.cpp
 * @date   2016/01/14 Thu. 17:44:21
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./QRReaderWork.h"


// =============================================================================
/**/
//==============================================================================
namespace {
  static const u32  APPHEAP_SIZE  = 0x00080000;
  static const u32  DEVHEAP_SIZE  = 0x01800000;   /*  app 20MB + QREnc 4MB  */
} /*  namespace  */

/*
*/
namespace NetApp   {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @func     QRReader
  *  @brief    ctor
  */
//------------------------------------------------------------------
QRReaderWork::QRReaderWork(void)
  : NetAppLib::System::ApplicationWorkBase(APPHEAP_SIZE, DEVHEAP_SIZE)
  , m_pAppParam(NULL)
  , m_pViewContainer(NULL)
  , m_pPokeModelUtil(NULL)
  , m_IsNeedCannotChargeConfirmIfFieldScanIsActive(true)
  , m_IsTransitionFromFieldScanFrame(false)
  , m_pCommonIconData(NULL)
{
}


//------------------------------------------------------------------
/**
  *  @func     QRReader
  *  @brief    ctor
  */
//------------------------------------------------------------------
QRReaderWork::~QRReaderWork()
{
}


} /*  namespace QRReader */
} /*  namespace NetApp   */
