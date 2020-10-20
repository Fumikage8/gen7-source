
#if defined(GF_PLATFORM_CTR)

#if !defined(QRREADERFRAME_QRSIMPLEREAD_H_INCLUDED)
#define QRREADERFRAME_QRSIMPLEREAD_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderFrame_QRSimpleRead.h
 * @date   2016/01/28 Thu. 16:38:46
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
class Frame_QRSimpleRead
  : public    NetAppLib::System::NetAppFrameBase
  , protected LowerView_Main::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_QRSimpleRead );
public:
  Frame_QRSimpleRead(QRReaderWork* pWork);
  virtual ~Frame_QRSimpleRead();

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

  /*  LowerView_Main::IEventHandler  */
  virtual void  LowerView_Main_OnEvent(const LowerView_Main::IEventHandler::EventCode eventCode);

  void  ExitFrame(const FrameResult = RESULT_QRSimpleRead_Exit);
  void  AnalyzeDecodedBinary(void);
  void  AnalyzeDecodedBinary_BattleTeam(void);    /**<  チーム選択用解析  */
  void  AnalyzeDecodedBinary_FriendMatch(void);   /**<  なかま大会用解析  */
  void  AnalyzeDecodedBinary_LiveMatch(void);     /**<  ライブ大会用解析  */

  void  QRDecodeUpdateEnable(const bool bCamera, const bool bQRDecode)
        {
          m_IsDeviceCameraUpdate  = bCamera;
          m_IsQRDecoderUpdate     = bQRDecode;
        }
  void  ShowInstruction(void);


 /*
    members
  */
  QRReaderWork*                   m_pWork;
  QRReaderViewContainer*          m_pViewContainer;
  u32                             m_State;
  FrameResult                     m_Result;
  bool                            m_IsCameraError;
  bool                            m_IsCameraErrorInfoEnd;

  /*  QR  */
  QRUtil::QRDecoder               m_QRDecoder;
  void*                           m_pQRDecodeSource;
  u32                             m_QRDecodeDirtyFrame;
  u32                             m_CameraEndFrameCount;
  bool                            m_IsDeviceCameraUpdate;
  bool                            m_IsQRDecoderUpdate;
  bool                            m_IsQRDecodeRequestEnable;
  bool                            m_IsOK;
  bool                            m_IsDataTypeMismatch;       /**<  目的のタイプではないQRを読み込んでいる  */

  /*  view  */
  UpperView_Main*                 m_pUpperView;
  UpperView_Pokemon*              m_pUpperView_Pokemon;
  LowerView_Main*                 m_pLowerView;
  MessageMenu*                    m_pMessageMenu;
};  /*  class EvolutionFrame  */




} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERFRAME_QRSIMPLEREAD_H_INCLUDED)  */


#endif  /* #if defined(GF_PLATFORM_CTR)  */
