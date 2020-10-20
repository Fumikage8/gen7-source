#if !defined(SYSTEM_CAMERA_INFRAMECAMERA_H_INCLUDED)
#define SYSTEM_CAMERA_INFRAMECAMERA_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    InFrameCamera.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.26
 @brief   インフレームカメラ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_math.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)
  class CModelViewerInFrame;
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)




GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)


//==============================================================================
/**
 @class     InFrameCamera
 @brief     インフレームカメラ
 */
//==============================================================================
class InFrameCamera
{
  GFL_FORBID_COPY_AND_ASSIGN(InFrameCamera);

  friend class CModelViewerInFrame;

public:
  InFrameCamera(void);
  virtual ~InFrameCamera() {}

  const gfl2::math::Matrix44& GetProjectionMatrix(void) const { return m_projMtx; }
  const gfl2::math::Matrix34& GetViewMatrix(void) const { return m_viewMtx; }

  //! @name  friendクラスModelViewerInFrameからアクセスするメンバ
  //@{
private:
  void SetProjectionMatrix(const gfl2::math::Matrix44& projMtx) { m_projMtx = projMtx; }
  void SetViewMatrix(const gfl2::math::Matrix34& viewMtx) { m_viewMtx = viewMtx; }
  //@}

private:
  gfl2::math::Matrix44  m_projMtx;
  gfl2::math::Matrix34  m_viewMtx;
};


GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)


#endif  // SYSTEM_CAMERA_INFRAMECAMERA_H_INCLUDED

