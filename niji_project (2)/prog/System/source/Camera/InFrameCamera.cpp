//==============================================================================
/**
 @file    InFrameCamera.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.26
 @brief   インフレームカメラ
 */
//==============================================================================


// nijiのインクルード
#include <System/include/Camera/ModelViewerInFrame.h>
#include <System/include/Camera/InFrameCamera.h>


GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)


//==============================================================================
//
//  InFrameCamera
//
//==============================================================================
InFrameCamera::InFrameCamera(void)
  : m_projMtx(gfl2::math::Matrix44::GetIdentity()),
    m_viewMtx(gfl2::math::Matrix34::GetIdentity())
{}


GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)

