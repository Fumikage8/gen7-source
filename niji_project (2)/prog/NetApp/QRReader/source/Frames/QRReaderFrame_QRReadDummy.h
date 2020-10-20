#if !defined(QRREADERFRAME_QRREADDUMMY_H_INCLUDED)
#define QRREADERFRAME_QRREADDUMMY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrame_QRReadDummy.h
 * @date   2016/01/19 Tue. 17:43:23
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

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"

#include  "../QRReaderFrameDef.h"


namespace NetApp   {
namespace QRReader {
  class QRReaderWork;
  class QRReaderViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/19 Tue. 17:44:15
  */
//==============================================================================
class Frame_QRReadDummy
  : public  NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_QRReadDummy );
public:

  Frame_QRReadDummy(QRReaderWork* pWork)
    : NetAppLib::System::NetAppFrameBase(pWork)
    , m_pWork(pWork)
  {}
  virtual ~Frame_QRReadDummy()
  {}

  /*
      implement FrameCellSuper I/F
  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
//  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*
      end implement FrameCellSuper I/F
  */


private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup()         {m_pWork->SetFrameResult(RESULT_QRRead_Exit); return true;}
  virtual void  setListener()     {}
  virtual void  removeListener()  {}
  virtual void  updateSequence()  {}
  virtual bool  cleanup()         {return true;}


 /*
    members
  */
  QRReaderWork*                   m_pWork;
};  /*  class EvolutionFrame  */




} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERFRAME_QRREADDUMMY_H_INCLUDED)  */
