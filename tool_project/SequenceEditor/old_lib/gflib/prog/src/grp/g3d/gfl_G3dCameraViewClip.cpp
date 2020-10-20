
//=============================================================================
/**
 * @file   gfl_G3dCameraViewClip.cpp
 * @brief  カメラビュークリップ制御クラス
 * @author 池田 哲平
 * @date   2012.08.25
 */
//=============================================================================

#include "grp/g3d/gfl_G3dCameraViewClip.h"

namespace gfl { namespace grp { namespace g3d
{
  //-----------------------------------------------------------------------------
  // コンストラクタ
  //-----------------------------------------------------------------------------
  CameraViewClip::CameraViewClip()
  {
    for( int i = 0; i < PLANE_NUM; i++ )
    {
      m_ainPlane[i].Set(gfl::math::VEC4_UNIT_Y);
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      m_ainDebugPosi[i] = gfl::math::VEC3_ZERO;
#endif
    }
  }

#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
  static void GetDebugPosition(gfl::math::VEC3* pPosi, const gfl::math::VEC3& inDir, const float fFarClip, const float fCos, const gfl::math::MTX33& inCameraRotMat, const gfl::math::VEC3& inCameraPosi)
  {
    gfl::math::MTX34 inCameraMat; gfl::math::MTX34SetMTX33VEC3(&inCameraMat, inCameraRotMat, inCameraPosi);
    (*pPosi) = inDir*((fFarClip/fCos)*0.5f);
    gfl::math::VEC3Transform(pPosi, &inCameraMat, pPosi);
  }
#endif

  //-----------------------------------------------------------------------------
  // フレーム処理
  //-----------------------------------------------------------------------------
  void CameraViewClip::Update(const gfl::grp::g3d::Camera& inCamera)
  {
    gfl::math::VEC3  inCameraPosi, inCameraDir;
    gfl::math::MTX33 inCameraRotMat;
    {
      gfl::math::VEC3 inLookAtPosi, inUpVec;
      const_cast<gfl::grp::g3d::Camera* >(&inCamera)->GetCameraAndTargetPosition(inCameraPosi, inLookAtPosi);
      gfl::math::VEC3Sub(&inCameraDir, &inLookAtPosi, &inCameraPosi);
      gfl::math::VEC3Normalize(&inCameraDir, &inCameraDir);
      inCamera.GetUpVector(&inUpVec);
      gfl::math::VEC3 inAxisX; gfl::math::VEC3Cross(&inAxisX, &inUpVec, &inCameraDir); gfl::math::VEC3Normalize(&inAxisX, &inAxisX);
      gfl::math::VEC3 inAxisY; gfl::math::VEC3Cross(&inAxisY, &inCameraDir, &inAxisX);
      gfl::math::MTX33SetAxis(&inCameraRotMat, inAxisX, inAxisY, inCameraDir);
    }

    const float fFarClip = inCamera.GetFarClip();
    gfl::math::VEC3 ainNormDir[PLANE_NUM], ainPosi[PLANE_NUM], inAdd;
    int nIndex; // はじく可能性が高い順にインデックスを付けていく

    // 前面
    {
      nIndex = 0;
      ainNormDir[nIndex] = gfl::math::VEC3_UNIT_Z;

      gfl::math::VEC3Scale(&inAdd, &inCameraDir, inCamera.GetNearClip());
      gfl::math::VEC3Add(&ainPosi[nIndex], &inCameraPosi, &inAdd);
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      m_ainDebugPosi[nIndex] = ainPosi[nIndex];
#endif
    }

    // 後面
    {
      nIndex = 5;
      ainNormDir[nIndex] = -gfl::math::VEC3_UNIT_Z;

      gfl::math::VEC3Scale(&inAdd, &inCameraDir, fFarClip);
      gfl::math::VEC3Add(&ainPosi[nIndex], &inCameraPosi, &inAdd);
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      m_ainDebugPosi[nIndex] = ainPosi[nIndex];
#endif
    }

    const float fViewAngleHalf = inCamera.GetFovy()*0.5f;
    float fSin, fCos;

    // gfl::math::SinCosRad(&fSin, &fCos, fViewAngleHalf*inCamera.GetAspect()); // この方法は間違い
    const float fDepth = 1.0f/gfl::math::TanRad(fViewAngleHalf);
    gfl::math::SinCosRad(&fSin, &fCos, gfl::math::Atan2Rad(inCamera.GetAspect(), fDepth));

    // 左面
    {
      nIndex = 1;
      const gfl::math::VEC3 inDir(fSin, 0.0f, fCos);
      gfl::math::VEC3Cross(&ainNormDir[nIndex], &inDir, &gfl::math::VEC3_UNIT_Y);
      ainPosi[nIndex] = inCameraPosi;
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      GetDebugPosition(&m_ainDebugPosi[nIndex], inDir, fFarClip, fCos, inCameraRotMat, inCameraPosi);
#endif
    }

    // 右面
    {
      nIndex = 2;
      const gfl::math::VEC3 inDir(-fSin, 0.0f, fCos);
      gfl::math::VEC3Cross(&ainNormDir[nIndex], &gfl::math::VEC3_UNIT_Y, &inDir);
      ainPosi[nIndex] = inCameraPosi;
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      GetDebugPosition(&m_ainDebugPosi[nIndex], inDir, fFarClip, fCos, inCameraRotMat, inCameraPosi);
#endif
    }

    gfl::math::SinCosRad(&fSin, &fCos, fViewAngleHalf);

    // 上面
    {
      nIndex = 3;
      const gfl::math::VEC3 inDir(0.0f, fSin, fCos);
      gfl::math::VEC3Cross(&ainNormDir[nIndex], &gfl::math::VEC3_UNIT_X, &inDir);
      ainPosi[nIndex] = inCameraPosi;
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      GetDebugPosition(&m_ainDebugPosi[nIndex], inDir, fFarClip, fCos, inCameraRotMat, inCameraPosi);
#endif
    }

    // 下面
    {
      nIndex = 4;
      const gfl::math::VEC3 inDir(0.0f, -fSin, fCos);
      gfl::math::VEC3Cross(&ainNormDir[nIndex], &inDir, &gfl::math::VEC3_UNIT_X);
      ainPosi[nIndex] = inCameraPosi;
#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
      GetDebugPosition(&m_ainDebugPosi[nIndex], inDir, fFarClip, fCos, inCameraRotMat, inCameraPosi);
#endif
    }

    for( int i = 0; i < PLANE_NUM; i++ )
    {
      gfl::math::VEC3Transform(&ainNormDir[i], &inCameraRotMat, &ainNormDir[i]);
      m_ainPlane[i].Set(ainNormDir[i], ainPosi[i]);
    }
  }

  //-----------------------------------------------------------------------------
  // AABBのクリップ判定を行う
  //-----------------------------------------------------------------------------
  bool CameraViewClip::IsClip(const gfl::math::AABB_VEC3& inAABB) const
  {
    gfl::math::VEC3 inPosi, inLenHalf; inAABB.GetCenterAndLengthHalf(&inPosi, &inLenHalf);
    for( int i = 0; i < PLANE_NUM; i++ )
    {
      const gfl::math::VEC4& inPlaneVec = m_ainPlane[i].GetPlaneVector();
      const float fRadius = gfl::math::FAbs(inLenHalf.x*inPlaneVec.x) + gfl::math::FAbs(inLenHalf.y*inPlaneVec.y) + gfl::math::FAbs(inLenHalf.z*inPlaneVec.z); // 実効半径取得
      if( m_ainPlane[i].GetDistance(inPosi) < -fRadius ) return true;
    }

    return false;
  }

} } } // namespace gfl::grp::g3d

/*  EOF  */





