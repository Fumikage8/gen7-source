#if !defined(GIFTFRAME_MAIN_H_INCLUDED)
#define GIFTFRAME_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrame_Main.h
 * @date   2015/07/31 Fri. 15:51:13
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
#include  "../GiftFrameDef.h"

#include  "../Views/UpperView_Message.h"
#include  "../Views/LowerView_Selection.h"

namespace NetApp  {
namespace Gift {
  class GiftWork;
  class GiftViewContainer;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  Frame_Main
  *  @brief  Gift大会Frame:main
  */
//==============================================================================
class Frame_Main
  : public NetAppLib::System::NetAppFrameBase
  , protected LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_Main );

public:

  //------------------------------------------------------------------
  /**
    *  @func     Frame_Main
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_Main(GiftWork* pWork);

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


  void  ExitFrame(const FrameResult = RESULT_MAIN_Exit);

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(const EventCode eventCode);
  /*  endimplement LowerView_Selection::IEventHandler  */


  /*
    members
  */
  GiftWork*             m_pWork;
  GiftViewContainer*    m_pViewContainer;
  u32                   m_State;
  FrameResult           m_Result;

  UpperView_Message*    m_pUpperView_Message;
  LowerView_Selection*  m_pLowerView_Selection;

};  /*  class GiftFrame  */





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTFRAME_MAIN_H_INCLUDED)  */
