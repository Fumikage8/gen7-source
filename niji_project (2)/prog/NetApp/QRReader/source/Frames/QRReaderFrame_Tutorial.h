
#if !defined(QRREADERFRAME_TUTORIAL_H_INCLUDED)
#define QRREADERFRAME_TUTORIAL_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrame_Tutorial.h
 * @date   2016/01/14 Thu. 16:54:39
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
  *  @date   2016/01/14 Thu. 16:55:41
  */
//==============================================================================
class Frame_Tutorial
  : public  NetAppLib::System::NetAppFrameBase
  , public  LowerView_Main::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Tutorial );
public:

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Tutorial(QRReaderWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Tutorial();

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
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  LowerView_Main::IEventHandler */
  virtual void  LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode);

  void  ExitFrame(const FrameResult = RESULT_Tutorial_Exit);



 /*
    members
  */
  QRReaderWork*                   m_pWork;
  QRReaderViewContainer*          m_pViewContainer;
  u32                             m_State;
  FrameResult                     m_Result;

  /*  view  */
  UpperView_Main*                 m_pUpperView;
  LowerView_Main*                 m_pLowerView;
  MessageMenu*                    m_pMessageMenu;
};  /*  class EvolutionFrame  */




} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERFRAME_TUTORIAL_H_INCLUDED)  */

