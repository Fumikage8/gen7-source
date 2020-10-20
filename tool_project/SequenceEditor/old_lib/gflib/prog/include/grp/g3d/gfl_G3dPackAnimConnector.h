#ifndef __GFL_G3DPACKANIMCONNECTOR_H__
#define __GFL_G3DPACKANIMCONNECTOR_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnimConnector.h
 *	@brief  3D�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C��������
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[PackAnimConnector�̃��[��]
�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C���͎��̃��[���𖞂����Ȃ���΂Ȃ�Ȃ��B
�E[PackAnim�̃��[��]�𖞂������ƁB
�EStartFrame, EndFrame�͎g���Ȃ��̂ŁA�������g�����ƑO��̃A�j���[�V�����f�[�^�͊܂܂Ȃ����ƁB
�ELoop�͑S���ɑ΂��ē����ݒ������̂ŁA�X�̃t�@�C���ɐݒ肵�Ă����Ă��g���Ȃ��B
  ���[�V�����u�����h���łȂ�1�̃A�j���[�V�����f�[�^�Đ����̂Ƃ��́A�X�̃t�@�C���̐ݒ���g��Ȃ����Ƃ��m�肵�Ă���B
  �y�����I�ȍ\�z�z�������A�A�j���[�V������؂�ւ���Ƃ��̃��[�V�����u�����h���́A
  �X�̃t�@�C���̐ݒ���g����������Ȃ��̂ŁA�X�̃t�@�C���ɂ�����Ɛݒ肵�Ă������Ƃ����߂�B
�EStepFrame�͑S���ɑ΂��ē����ݒ������B
�EFrame�͍��Đ����̃A�j���[�V�����f�[�^�̃t���[���ԍ��ɂȂ�B*/
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
///	1��Model��1��PackAnim�����ѕt����N���X
//=====================================
class ModelPackAnimConnector
{
public:
  ModelPackAnimConnector(void);
  virtual ~ModelPackAnimConnector();

  //-----------------------------------------------------------------------------
  /**
   *  @brief  ���f����ݒ肷��
   *
   *          ���f����ݒ肵�����ƁAPackAnim�̐ݒ���Ȃ��Ȃ�܂��B
   */
  //-----------------------------------------------------------------------------
  virtual void SetModel(Model* model);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  PackAnim��ݒ肷��
   *
   *          SetModel�����Ă���łȂ��ƁAPackAnim��ݒ�ł��܂���B
   *          PackAnim��ݒ肵�����ƁA����܂Őݒ肵�Ă���PackAnim�̐ݒ�͔j�����܂��B
   */
  //-----------------------------------------------------------------------------
  virtual void SetPackAnim(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      s32 slot_index,
      PackAnim* pack_anim
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  �ݒ肵�Ă���PackAnim�̐ݒ��j������
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetPackAnim(void);


  virtual void ChangeAnimNo(s32 anim_no);

  virtual void SetFrame(f32 frame);
  virtual f32 GetFrame(void) const;
  virtual void SetStepFrame(f32 step_frame);
  virtual f32 GetStepFrame(void) const;
  virtual void SetLoop(b32 loop);
  virtual b32 IsLoop(void) const;

  virtual s32 GetSlotIndex(void) const  { return m_slot_index; }

private:
  ////////////////////////////////////////////
  // UnsetPackAnim�Őݒ肪�j����������
  ////////////////////////////////////////////
  PackAnim*  m_pack_anim;
  s32        m_slot_index;
  s32        m_anim_no;
  b32        m_is_set;  // m_model!=NULL����m_pack_anim!=NULL����m_pack_anim->GetMaxAnimMembers()>0����
                        // m_model->ChangeAnimation��m_slot_index�ɗ̈���m�ۍς݂̂Ƃ�true�B����ȊO�̂Ƃ�false�B
  // @note ChangeAnimation�������Ǘ��s���Ή�  // ChangeAnimation��NULL��n������ɉ��L�A�j����0�t���[���Ŏ~�߂Ă������Ƃɂ���B
  ResAnim*   m_res_anim_change_animation_temp;

  ////////////////////////////////////////////
  // UnsetPackAnim�Őݒ肪�j������Ȃ�����
  ////////////////////////////////////////////
  Model*     m_model;
  f32        m_step_frame;
  b32        m_loop;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DPACKANIMCONNECTOR_H__

