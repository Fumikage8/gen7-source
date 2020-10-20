
//=============================================================================
/**
 * @file   gfl_G3dCameraViewClip.h
 * @brief  カメラビュークリップ制御クラスヘッダ
 * @author 池田 哲平
 * @date   2012.08.25
 */
//=============================================================================

#ifndef __GFL_G3D_CAMERA_VIEW_CLIP_H__
#define __GFL_G3D_CAMERA_VIEW_CLIP_H__
#pragma once

// #define IS_DEBUG_CAMERA_VIEW_CLIP

namespace gfl { namespace grp { namespace g3d
{
  //-----------------------------------------------------------------------------
  /**
   * @brief カメラのビュークリップ制御を行うクラス
   */
  //-----------------------------------------------------------------------------
  class CameraViewClip
  {
  private:
    enum { PLANE_NUM = 6 }; ///< 視錐台を形成するために必要な平面の数
    gfl::math::Plane m_ainPlane[PLANE_NUM]; ///< 視錐台を形成する平面達

#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
    gfl::math::VEC3 m_ainDebugPosi[PLANE_NUM]; ///< デバッグで平面を描画するときに使用する位置
#endif

  public:
    CameraViewClip(); ///< コンストラクタ
    void Update(const gfl::grp::g3d::Camera& inCamera); ///< フレーム処理

    bool IsClip(const gfl::math::AABB_VEC3& inAABB) const ; ///< AABBのクリップ判定を行う

#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
    int  GetPlaneNumberForDebug() const { return PLANE_NUM; } ///< 平面の数を取得する
    void GetPlaneForDebug(gfl::math::VEC3* pNormDir, gfl::math::VEC3* pPosi, const int nIndex) const ///< 平面を取得する
    { m_ainPlane[nIndex].GetNormalVector(pNormDir); (*pPosi) = m_ainDebugPosi[nIndex]; }
#endif
  };

} } } // namespace gfl::grp::g3d

#endif // __GFL_G3D_CAMERA_VIEW_CLIP_H__

/*  EOF  */





