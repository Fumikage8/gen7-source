
//=============================================================================
/**
 * @file   gfl_G3dCameraViewClip.h
 * @brief  �J�����r���[�N���b�v����N���X�w�b�_
 * @author �r�c �N��
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
   * @brief �J�����̃r���[�N���b�v������s���N���X
   */
  //-----------------------------------------------------------------------------
  class CameraViewClip
  {
  private:
    enum { PLANE_NUM = 6 }; ///< ��������`�����邽�߂ɕK�v�ȕ��ʂ̐�
    gfl::math::Plane m_ainPlane[PLANE_NUM]; ///< ��������`�����镽�ʒB

#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
    gfl::math::VEC3 m_ainDebugPosi[PLANE_NUM]; ///< �f�o�b�O�ŕ��ʂ�`�悷��Ƃ��Ɏg�p����ʒu
#endif

  public:
    CameraViewClip(); ///< �R���X�g���N�^
    void Update(const gfl::grp::g3d::Camera& inCamera); ///< �t���[������

    bool IsClip(const gfl::math::AABB_VEC3& inAABB) const ; ///< AABB�̃N���b�v������s��

#ifdef IS_DEBUG_CAMERA_VIEW_CLIP
    int  GetPlaneNumberForDebug() const { return PLANE_NUM; } ///< ���ʂ̐����擾����
    void GetPlaneForDebug(gfl::math::VEC3* pNormDir, gfl::math::VEC3* pPosi, const int nIndex) const ///< ���ʂ��擾����
    { m_ainPlane[nIndex].GetNormalVector(pNormDir); (*pPosi) = m_ainDebugPosi[nIndex]; }
#endif
  };

} } } // namespace gfl::grp::g3d

#endif // __GFL_G3D_CAMERA_VIEW_CLIP_H__

/*  EOF  */





