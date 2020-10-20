// ============================================================================
/*
 * @file QRCodeTestProc.h
 * @brief QRCodeのテストです。
 * @date 2015.03.04
 */
// ============================================================================
#if PM_DEBUG

#if !defined( QRCODETESTPROC_H_INCLUDED )
#define QRCODETESTPROC_H_INCLUDED
#pragma once


#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <GameSys/include/GameProc.h>
#if GF_PLATFORM_CTR
#include <Device/include/Camera/gfl2_DeviceCamera_System.h>
#endif // GF_PLATFORM_CTR

#include <qr/include/gfl2_IQREncoder.h>
#include <qr/include/gfl2_IQRDecoder.h>

GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(QRCodeTest)

class QRTestEventListener;

class QRCodeTestProc : public GameSys::GameProc
{
	GFL_FORBID_COPY_AND_ASSIGN( QRCodeTestProc );

public:
  QRCodeTestProc();
  virtual~QRCodeTestProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

private:
  void DisplayPrint();

private:

#if GF_PLATFORM_CTR
  gfl2::device::camera::System*                     m_pCameraSystem;
  u32                                               m_DeviceCameraState;
#endif // GF_PLATFORM_CTR

  gfl2::heap::NwAllocator*		                      m_pMemAllocator;
	gfl2::heap::NwAllocator*		                      m_pDevAllocator;
  System::nijiAllocator*                            m_pNijiAllocator;
	gfl2::util::UtilTextRenderingPipeLine*		        m_pRenderingPipeLine;
  gfl2::str::ResFont*				                        m_pResFont;
	gfl2::util::DrawUtilText* mpDrawUtilText;

  gfl2::qr::IQREncoder*                             m_pQREncoder;
  gfl2::qr::IQRDecoder*                             m_pQRDecoder;
  QRTestEventListener*                              m_pQREventListener;
  gfl2::qr::QRYUVImageInfo*                         m_pQRYUVImageInfo;
  gfl2::qr::QREncodeData*                           m_pQREncodeData;
  u32                                               m_YuvImageSize;

};


GFL_NAMESPACE_END(QRCodeTest)
GFL_NAMESPACE_END(Test)

#endif // QRCODETESTPROC_H_INCLUDED

#endif // PM_DEBUG
