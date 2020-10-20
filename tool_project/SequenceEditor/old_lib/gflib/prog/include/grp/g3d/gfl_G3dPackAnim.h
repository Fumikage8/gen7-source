#ifndef __GFL_G3DPACKANIM_H__
#define __GFL_G3DPACKANIM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnim.h
 *	@brief  3D�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C��������
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if 0
/*[PackAnim�̃��[��]
�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C���͎��̃��[���𖞂����Ȃ���΂Ȃ�Ȃ��B
�Eg3d_omake_tool��G3DTOOLANIM_MakePackAnimFile��1�ɂ܂Ƃ߂��t�@�C�������邱�ƁB
�E����AnimationType�̃t�@�C�������ł܂Ƃ߂邱�ƁB
�E�X�̃t�@�C���̒��g�́A1�̃A�j���[�V�����f�[�^�������琬�邱�ƁB
  ����������ƁAResource����AnimationType���w�肵��0�ԂŎ擾�ł��Ȃ���΂Ȃ�Ȃ��B*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C���������N���X
//=====================================
class PackAnim
{
  GFL_FORBID_COPY_AND_ASSIGN(PackAnim);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  PackAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �f�X�g���N�^
   *
   *          Destory���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~PackAnim();

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ����
   *
   *  @param[in] heap_allocator
   *  @param[in] device_allocator
   *  @param[in] type
   *  @param[in] buffer  �A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C����ǂݍ��񂾃o�b�t�@�B
   *                     VRAM���f�o�C�X�������Ɋm�ۂ��Ă����ĉ������B
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    AnimationType           type,
    void*                   buffer
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �j��
   *
   *          ���x�Ă�ł����v�ł��B
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  �܂Ƃ߂��Ă���A�j���[�V�����t�@�C���̃����o���̍ő�l��Ԃ�
   *
   *          ����ƃ��f���̃����o�����r���đ傫���ق����̗p����Ƃ悢�B
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetMaxAnimMembers(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �܂Ƃ߂��Ă���A�j���[�V�����t�@�C������Ԃ�
   *
   *          GetResAnim��NULL��Ԃ��Ƃ��̂��̂�1�Ƃ��ăJ�E���g���Ă���B
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetAnimNum(void) const;
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �܂Ƃ߂��Ă���A�j���[�V�����t�@�C����anim_no�Ԗڂ��������ResAnim�𓾂�
   *
   *  @param[in]  anim_no  ���Ԗڂ̃A�j���[�V�����t�@�C����
   *
   *  @retval  ResAnim
   *           anim_no�Ԗڂ̃A�j���[�V�����t�@�C�����Ȃ��Ƃ���NULL��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual ResAnim* GetResAnim(s32 anim_no) const;

private:
  void*         m_buffer;
  Resource**    m_resource_array;
  ResAnim**     m_res_anim_array;
  AnimationType m_type;
  s32           m_anim_num;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DPACKANIM_H__

