#if !defined(QRREADERFRAME_FIELDSCANRUNNING_H_INCLUDED)
#define QRREADERFRAME_FIELDSCANRUNNING_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrame_FieldScanRunning.h
 * @date   2016/02/03 Wed. 18:59:30
 * @author muto_yu
 * @brief  fieldScan起動中
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


namespace NetApp    {
namespace QRReader  {
  class QRReaderWork;
  class QRReaderViewContainer;
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/02/03 Wed. 18:59:30
  */
//==============================================================================
class Frame_FieldScanRunning
  : public    NetAppLib::System::NetAppFrameBase
  , protected UpperView_Pokemon::IRemainingTimeProvider
  , protected LowerView_Main::IEventHandler
  , protected LowerView_Main::IFieldScanPowerProvider
  , protected NetAppLib::Util::NetAppPokeModelUtility::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_FieldScanRunning );
public:

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_FieldScanRunning(QRReaderWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_FieldScanRunning();

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

  /*  UpperView_Pokemon::IRemainingTimeProvider  */
  virtual u32   IRemainingTimeProvider_OnQueryMinutes(void);
  virtual u32   IRemainingTimeProvider_OnQuerySeconds(void);

  /*  LowerView_Main::IEventHandler  */
  virtual void  LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode);
  /*  LowerView_Main::IFieldScanPowerProvider  */
  virtual u32   IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive);

  /*  NetAppLib::Util::NetAppPokeModelUtility::IEventHandler  */
  virtual void  NetAppPokeModelUtility_OnEvent(NetAppLib::Util::NetAppPokeModelUtility* pSender, const NetAppLib::Util::NetAppPokeModelUtility::IEventHandler::EventID eventID, PokeTool::PokeModel* pPokeModel);

  void  OnEntry(void);
  void  ScanRunning(void);
  void  ExitFrame(const FrameResult = RESULT_FieldScanRunning_Exit);

 /*
    members
  */
  QRReaderWork*           m_pWork;
  QRReaderViewContainer*  m_pViewContainer;
  u32                     m_State;
  FrameResult             m_Result;
  
  u32                     m_ScanFrame;
  u32                     m_ScanPower;

  /*  fieldscan  */
  u32                     m_RemainingMinutes;
  u32                     m_RemainingSeconds;

  /*  view  */
  UpperView_Main*         m_pUpperView;
  UpperView_Pokemon*      m_pUpperView_Pokemon;
  LowerView_Main*         m_pLowerView;
  MessageMenu*            m_pMessageMenu;
};  /*  class EvolutionFrame  */




} /*  namespace QRReader  */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERFRAME_FIELDSCANRUNNING_H_INCLUDED)  */

