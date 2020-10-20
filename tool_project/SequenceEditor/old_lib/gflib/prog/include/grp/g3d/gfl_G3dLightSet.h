#ifndef __GFL_G3DLIGHTSET_H__
#define __GFL_G3DLIGHTSET_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLightSet.h
 *	@brief  3D���C�g�Z�b�g
 *	@author	Koji Kawada
 *	@date		2012.05.16
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dLight.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D���C�g�Z�b�g�N���X
//=====================================
class LightSet
{
  GFL_FORBID_COPY_AND_ASSIGN(LightSet);

  friend class Scene;


public:
  // �ݒ���e
  struct Description
  {
    s32  max_vertex_lights;  // ���_���C�g�̍ő吔

    Description(void)
      : max_vertex_lights(0)
    {}
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  LightSet(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~LightSet();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
   *  @param[in]     description        �ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           device_allocator,
      Description*                      description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���C�g��ǉ�����
   *
   *  @param[in]     light  �ǉ����郉�C�g
   *                        ���̃��C�g�̃|�C���^���o���Ă��������ł��B
   *
   *  @retval  �ǉ��ɐ���������true��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 AddLight(Light* light);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���C�g���폜����
   *
   *  @param[in]     light  �폜���郉�C�g
   *                        ���̃��C�g�̃|�C���^��Y��邾���ł��B
   */
  //-----------------------------------------------------------------------------
  void RemoveLight(Light* light);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���C�g��S�č폜����
   *                 �o���Ă��郉�C�g�̃|�C���^��S�ĖY��邾���ł��B
   */
  //-----------------------------------------------------------------------------
  void RemoveAllLight(void);


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         NW4C�̃��C�g�Z�b�g�𓾂�
   *                 friend�N���X���炾���A�N�Z�X�\�B
   *
   *  @retval  NW4C�̃��C�g�Z�b�g
   */
  //-----------------------------------------------------------------------------
  nw::gfx::LightSet* GetNwLightSet(void) const { return m_light_set; }


private:
  nw::gfx::LightSet* m_light_set;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DLIGHTSET_H__
