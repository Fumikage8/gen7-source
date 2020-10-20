//==============================================================================
/**
 @file    AppCamera.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.01.22
 @brief   カメラ
 */
//==============================================================================


// nijiのインクルード
#include <AppLib/include/Util/AppCamera.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
//
//  AppCamera
//
//==============================================================================
AppCamera::AppCamera(void)
  : m_viewMtx(gfl2::math::Matrix34::GetIdentity()),
    m_projMtx(gfl2::math::Matrix44::GetIdentity()),
    m_projType(ProjectionType::PERSPECTIVE)
{}

void AppCamera::SetupOrthoCamera(
    RenderPlaceType::Tag renderPlace,
    f32 scale, f32 front, f32 rear
)
{
  // プロジェクションマトリクス
  f32 left   = 0.0f;
  f32 right  = 0.0f;
  f32 bottom = 0.0f; 
  f32 top    = 0.0f;
  bool isPivot = false;
#if defined(GF_PLATFORM_CTR)
  isPivot = true;
#endif
  switch(renderPlace)
  {
  case RenderPlaceType::UPPER_DISPLAY:
    {
      left   = -200.0f;
      right  =  200.0f;
      bottom = -120.0f;
      top    =  120.0f;
      break;
    }
  case RenderPlaceType::LOWER_DISPLAY:
    {
      left   = -160.0f;
      right  =  160.0f;
      bottom = -120.0f;
      top    =  120.0f;
      break;
    }
  default:
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
      break;
    }
  }
  m_projMtx = gfl2::math::Matrix44::GetOrthographic(
      left/scale, right/scale, bottom/scale, top/scale,
      0.0f, front-rear,
      isPivot
  );
  m_projType = ProjectionType::ORTHO;

  // ビューマトリクス
  gfl2::math::Vector3 eyePosition( 0.0f, 0.0f, front );    // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition( 0.0f, 0.0f, rear );   // 注視点
  gfl2::math::Vector3 upDirection( 0.0f, 1.0f, 0.0f );     // カメラの上方向
  m_viewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);
}

void AppCamera::SetViewMatrix(
    const gfl2::math::Vector3&  position,
    const gfl2::math::Vector3&  targetPosition,
    f32                         twist
)
{
  nw::math::MTX34 nwViewMat;
  nw::math::VEC3  nwPosition       = gfl2::math::ConvertGfVec3ToNwVec3(position);
  nw::math::VEC3  nwTargetPosition = gfl2::math::ConvertGfVec3ToNwVec3(targetPosition);
  nw::math::MTX34LookAt(&nwViewMat, &nwPosition, twist, &nwTargetPosition);
  m_viewMtx = gfl2::math::ConvertNwMtx34ToGfMtx34(nwViewMat);
}

void AppCamera::SetOrthoProjectionMatrix(
    f32                         aspectRatio,
    f32                         height,
    f32                         near,
    f32                         far,
    bool                        isPivot
)
{
  f32 left    = -height*aspectRatio/2.0f;
  f32 right   =  height*aspectRatio/2.0f;
  f32 bottom  = -height/2.0f;
  f32 top     =  height/2.0f;

  m_projMtx = gfl2::math::Matrix44::GetOrthographic(
      left, right, bottom, top,
      near, far,
      isPivot
  );

  m_projType = ProjectionType::ORTHO;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

