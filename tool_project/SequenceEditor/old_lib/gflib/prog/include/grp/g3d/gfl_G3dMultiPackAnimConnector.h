#ifndef __GFL_G3DMULTIPACKANIMCONNECTOR_H__
#define __GFL_G3DMULTIPACKANIMCONNECTOR_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dMultiPackAnimConnector.h
 *	@brief  3D�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C��������
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[MultiPackAnimConnector�̃��[��]
�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C���͎��̃��[���𖞂����Ȃ���΂Ȃ�Ȃ��B
�E[PackAnimConnector�̃��[��]�𖞂����B
�E�����ԍ��̃A�j���[�V�����f�[�^�͓����ݒ�łȂ���΂Ȃ�Ȃ��B
  ��FPackAnim��aaaa, bbbb, cccc��3�̂Ƃ�
        aaaa[0] 15�t���[�� �����^�C��  aaaa[1] 30�t���[�� ���[�v  aaaa[2] 45�t���[�� �����^�C�� 
        bbbb[0] 15�t���[�� �����^�C��  bbbb[1] 30�t���[�� ���[�v  bbbb[2] 45�t���[�� �����^�C��
        cccc[0] 15�t���[�� �����^�C��  cccc[1] 30�t���[�� ���[�v  cccc[2] 45�t���[�� �����^�C��
      �Ƃ����ӂ��ɓ����ݒ�łȂ���΂Ȃ�Ȃ��B
  ����́A��ɓ����ԍ��A�����t���[�����Đ�����邱�ƂɗR������B�܂�A
        aaaa��1�Ԃ�8�t���[���ڂ��Đ�����Ă���Ƃ��́Abbbb��1�Ԃ�8�t���[���ځAcccc��1�Ԃ�8�t���[���ڂ��Đ�����Ă���
  �Ƃ������ƂɗR������B
�E�y�����I�ȍ\�z�z�A�j���[�V������؂�ւ���Ƃ��̃��[�V�����u�����h�̎d���́A
  ���ꂼ��̃X���b�g�Őݒ�ł���悤�ɂ��邩������Ȃ��B
  �e�N�X�`��UV�A�j���[�V�����̕\��A�j���ƁA�X�P���^���A�j���[�V�����̑S�g�A�j���ŁA
  ������Ԃ̎d��������킯�ɂ͂����Ȃ��̂ŁB*/
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
///	1��Model�ƕ�����PackAnim�����ѕt����N���X
//=====================================
class ModelMultiPackAnimConnector
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelMultiPackAnimConnector);

public:
  ModelMultiPackAnimConnector(void);
  virtual ~ModelMultiPackAnimConnector();

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
   *          slot_index������PackAnim��ݒ�ς݂̃X���b�g�̏ꍇ�APackAnim��ݒ肵�����ƁA����܂Őݒ肵�Ă���PackAnim�̐ݒ�͔j�����܂��B
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
   *  @brief  slot_index������PackAnim��ݒ�ς݂̃X���b�g�̏ꍇ�A�ݒ肵�Ă���PackAnim�̐ݒ��j������
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetPackAnim(s32 slot_index);
  //-----------------------------------------------------------------------------
  /**
   *  @brief  PackAnim��ݒ�ς݂̑S�X���b�g�ɑ΂��āA�ݒ肵�Ă���PackAnim�̐ݒ��j������
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetAllPackAnim(void);

  virtual void ChangeAnimNo(s32 anim_no);
  
  virtual void SetStepFrame(f32 step_frame);
  virtual void SetLoop(b32 loop);

private:
  struct PackAnimElem
  {
    ModelPackAnimConnector*  pack_anim_connector;
    PackAnimElem*            prev;  // �擪�̂Ƃ�NULL
    PackAnimElem*            next;  // �����̂Ƃ�NULL
  };

private:
  PackAnimElem* getPackAnimElem(s32 slot_index) const;

private:
  ////////////////////////////////////////////
  // slot_index����v����Ƃ��AUnsetPackAnim�Őݒ肪�j����������
  ////////////////////////////////////////////
  PackAnimElem*  m_pack_anim_list_head;  // �擪�̃|�C���^

  ////////////////////////////////////////////
  // UnsetPackAnim�Őݒ肪�j�����ꂽ���ʁAm_pack_anim_list_head��NULL�ɂȂ�����ݒ肪�j����������
  ////////////////////////////////////////////
  s32            m_anim_no;
 
  ////////////////////////////////////////////
  // UnsetPackAnim�Őݒ肪�j������Ȃ�����
  ////////////////////////////////////////////
  Model*         m_model;
  f32            m_step_frame;
  b32            m_loop;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DMULTIPACKANIMCONNECTOR_H__

