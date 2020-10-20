
#if defined(GF_PLATFORM_CTR)

#if !defined(QRREADERFRAME_QRREAD_H_INCLUDED)
#define QRREADERFRAME_QRREAD_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file      QRReaderFrame_QRRead.h
 * @date      2016/01/18 Mon. 15:26:53
 * @author    muto_yu
 * @brief  
 * @attention CTR専用
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
#include  "../Common/QR/util_QREncoder.h"
#include  "../Common/QR/util_QRDecoder.h"

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
class Frame_QRRead
  : public    NetAppLib::System::NetAppFrameBase
  , protected UpperView_Main::IEventHandler
  , protected UpperView_Main::IQRScanPowerProvider
  , protected LowerView_Main::IEventHandler
  , protected LowerView_Main::IFieldScanPowerProvider
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_QRRead );
public:

  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  Frame_QRRead(QRReaderWork* pWork);

  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~Frame_QRRead();

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

  /*  UpperView_Main::IEventHandler  */
  virtual void  UpperView_Main_OnEvent(const UpperView_Main::IEventHandler::EventCode eventCode);

  /*  UpperView_Main::IQRScanPowerProvider  */
  virtual u8    QRScanPowerProvider_OnQueryPower(void);

  /*  LowerView_Main::IEventHandler  */
  virtual void  LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode);

  /*  LowerView_Main::IFieldScanPowerProvider  */
  virtual u32   IFieldScanPowerProvider_OnQueryPower(bool& rIsFull, bool& rIsActive);



  void  ExitFrame(const FrameResult = RESULT_QRRead_Exit);
  void  AnalyzeDecodedBinary(void);

  void  QRDecodeUpdateEnable(const bool bCamera, const bool bQRDecode)
        {
          m_IsDeviceCameraUpdate  = bCamera;
          m_IsQRDecoderUpdate     = bQRDecode;
        }
  void  ShowDataTypeMessage(void);

 /*
    members
  */
  QRReaderWork*                   m_pWork;
  QRReaderViewContainer*          m_pViewContainer;
  u32                             m_State;
  FrameResult                     m_Result;
  bool                            m_IsCameraError;
  bool                            m_IsCameraErrorInfoEnd;

  u32                             m_ScanPower;                /**<  fieldScanPower            */
  u32                             m_ScanPowerGain;            /**<  fieldScanPower増加量残り  */
  u32                             m_ScanPowerChargeWaitFrame;

  /*  QR  */
  gfl2::heap::HeapBase*           m_pEncoderHeap;
  QRUtil::QREncoder               m_QREncoder;                            /**<  エンコーダ（演出で使用する）  */
  QRUtil::QRDecoder               m_QRDecoder;
  void*                           m_pQRDecodeSource;
  u32                             m_QRDecodeDirtyFrame;
  u32                             m_CameraEndFrameCount;
  bool                            m_IsDeviceCameraUpdate;
  bool                            m_IsQRDecoderUpdate;
  bool                            m_IsQRDecodeRequestEnable;
  bool                            m_bFieldScanPowerFullChargedTrigger;    /**<  FieldScanPowerが満タンになった  */
//  bool                            m_bScanPowerChargeEOM;
  bool                            m_IsPromotionalData;                    /**<  連動QRだったか  */

  /*  view  */
  UpperView_Main*                 m_pUpperView;
  UpperView_Pokemon*              m_pUpperView_Pokemon;
  LowerView_Main*                 m_pLowerView;
  MessageMenu*                    m_pMessageMenu;
  MessageMenu::Mode               m_ScanResultMode;
};  /*  class EvolutionFrame  */




} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERFRAME_QRREAD_H_INCLUDED)  */


#endif  /* #if defined(GF_PLATFORM_CTR)  */
