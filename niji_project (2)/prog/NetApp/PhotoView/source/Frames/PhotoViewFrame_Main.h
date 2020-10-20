
#if !defined(PHOTOVIEWFRAME_MAIN_H_INCLUDED)
#define PHOTOVIEWFRAME_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewFrame_Main.h
 * @date   2017/02/07 Tue. 13:21:14
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
#include  <NetStatic/NetAppLib/include/System/NetAppFrameBase.h>
#include  "../PhotoViewFrameDef.h"

namespace NetApp  {
namespace PhotoView {
  class PhotoViewWork;
  class PhotoViewViewContainer;
  class UpperView_Photo;
  class LowerView_Info;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Main
  *  @brief  PhotoView大会Frame:main
  */
//==============================================================================
class Frame_Main
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Info::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Main );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Main
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Main(PhotoViewWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @func     ~Frame_Main
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_Main();


private:
  /*
    override  NetAppFrameBase I/F
  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  lowerview  */
  virtual void  LowerView_Info_OnEvent(LowerView_Info* pSender, const LowerView_Info::IEventHandler::EventCode eventCode);

  void  ExitFrame(const FrameResult = RESULT_MAIN_Exit);

  /*  slideshow  */
  void  SlideShow_Update(void);
  void  RequestPhoto(void);

  /*
    members
  */
  PhotoViewWork*                                    m_pWork;
  PhotoViewViewContainer*                           m_pViewContainer;
  u32                                               m_State;
  FrameResult                                       m_Result;
  u32                                               m_FrameCount;
  gfl2::util::List<APP_PARAM::PhotoInfo>::Iterator  m_JFPlayerDataListItr;
  gfl2::util::List<APP_PARAM::PhotoInfo>::Iterator  m_JFPlayerDataListItr_Begin;
  gfl2::util::List<APP_PARAM::PhotoInfo>::Iterator  m_JFPlayerDataListItr_End;

  /*  views  */
  UpperView_Photo*          m_pUpperView_Photo;
  LowerView_Info*           m_pLowerView_Info;
};  /*  class PhotoViewFrame  */





} /*  namespace PhotoView */
} /*  namespace NetApp      */
#endif  /*  #if !defined(PHOTOVIEWFRAME_MAIN_H_INCLUDED)  */
