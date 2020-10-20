#ifndef __GFL_G3DANIMATIONMANAGER_H__
#define __GFL_G3DANIMATIONMANAGER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationManager.h
 *	@brief  3D�A�j���[�V���������蓖�Ă�X���b�g�̊Ǘ�
 *	@author	Koji Kawada
 *	@date		2011.10.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dUnitAnimation.h>
#include <grp/g3d/gfl_G3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	�A�j���[�V���������蓖�Ă�X���b�g�̊Ǘ�
//=====================================
class AnimationManager
{
  GFL_FORBID_COPY_AND_ASSIGN(AnimationManager);

  friend class Model;
  friend class Camera;
  friend class StereoCamera;
  friend class Light;


  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  // 
  // �ǂ�����ł��A�N�Z�X�ł��郁���o
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
public:
  // �萔
  static const u32 DEFAULT_SLOT_NUM = 3;
  
  static const s32 DEFAULT_OWN_SLOT_BLEND_NUM_MAX     = 2;
  static const s32 DEFAULT_OWN_UNIT_ANIMATION_NUM_MAX = 2; 

public:
  // �ݒ���e
  struct Description
  {
    u32                         slot_num;
    AnimationSlot::Description* slot_description_array;
    b32                         use_own_animation;
    s32                         own_slot_blend_num_max;
    s32                         own_unit_animation_num_max;
    Description(void)
      : slot_num(DEFAULT_SLOT_NUM),
        slot_description_array(NULL),
        use_own_animation(true),
        own_slot_blend_num_max(DEFAULT_OWN_SLOT_BLEND_NUM_MAX),
        own_unit_animation_num_max(DEFAULT_OWN_UNIT_ANIMATION_NUM_MAX)
    {
      // �������Ȃ�
    }
  };

public:
  // object��max_members�𓾂�
  // object��SkeletalModel�̂Ƃ��̓{�[���̐��Ƃ���r���Ă���
  static s32 GetMaxMembers( Object* object, ResAnim::Type type );




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friend�N���X���炵���A�N�Z�X���Ă͂Ȃ�Ȃ������o�֐�
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  static nw::gfx::AnimGroup* GetNwAnimGroupAndMemberCount( Object* object, ResAnim::Type type, s32* member_count );

private:
  // �R���X�g���N�^
  AnimationManager(void);
  // �f�X�g���N�^
  virtual ~AnimationManager();

  // ����
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Object*                 object,
      Description*            description = NULL
  );

  // UseOwn�p
  void UseOwn_Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Object*                 object,
      u32                     slot_num,
      u32                     unit_num,
      u32                     animation_num_in_unit
  );

  // �j��
  virtual void Destroy(void);

  // �v�Z����
  // �K�����t���[���Ă�ŉ������B
  virtual void Calculate(void);

  // �X���b�g�����擾
  u32 GetSlotNum(void) const
  {
    return m_slot_num;
  }

private:
  // �X���b�g
  void CreateSlot(
    u32                         slot_index,
    gfl::heap::NwAllocator*     heap_allocator,
    gfl::heap::NwAllocator*     device_allocator,
    AnimationSlot::Description* slot_description_array = NULL
  );
  ResAnim::Type GetSlotType(
    u32                         slot_index
  ) const;

  // @note AnimationSlot::GetAnimationNum�̖߂�l���C�����悤���Ǝv���Ă���̂ŁA
  // private�����o�ŁB
  // ���̖߂�l�𒼐ڎg���̂́A���̏C�����I���܂ő҂��ĉ������B
  s32 GetSlotAnimationNum(
    u32                         slot_index
  ) const;


  // �X���b�g�̗L��/����
  b32 IsSlotEnable(u32 slot_index) const;
  void SetSlotEnable(u32 slot_index, b32 enable);


  // ResAnim::Type�̃A�j���[�V���������蓖�Ă���AnimGroup���I���W�i���l�Ƀ��Z�b�g����B
  // ResAnim::Type�̃A�j���[�V���������蓖�Ă���AnimGroup�ɃA�j���[�V���������݂���ꍇ�A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
  // ResAnim::Type�̃A�j���[�V���������蓖�Ă���AnimGroup�ɃA�j���[�V���������݂��Ȃ��ꍇ�A�S�Ẵ����o�����Z�b�g�����B
  void Reset(ResAnim::Type type);
  // �X���b�g�ɃA�j���[�V���������蓖�Ă��Ă���ꍇ�A���̃A�j���[�V������ResAnim::Type�ɑ΂���Reset���s���B
  // �X���b�g�ɃA�j���[�V���������蓖�Ă��Ă��Ȃ��ꍇ�A�������Ȃ��B
  void ResetSlot(u32 slot_index);


  // UseOwn�p
  void UseOwn_ChangeAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     slot_index,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members,
    b32                     force = false
  );
  void UseOwn_BlendAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );

  // �t���[������
  void SetFrame(u32 slot_index, f32 frame);
  f32  GetFrame(u32 slot_index) const;
  void SetStepFrame(u32 slot_index, f32 step_frame);
  f32  GetStepFrame(u32 slot_index) const;
  void SetStartFrame(u32 slot_index, f32 frame);
  f32  GetStartFrame(u32 slot_index) const;
  void SetEndFrame(u32 slot_index, f32 frame);
  f32  GetEndFrame(u32 slot_index) const;
  void SetStartAndEndFrame(u32 slot_index, f32 start_frame, f32 end_frame);
  b32  IsFrameStartFrame(u32 slot_index) const;
  b32  IsFrameEndFrame(u32 slot_index) const;
  b32  IsFrameStepTerminalFrame(u32 slot_index) const;
  b32  IsLoop(u32 slot_index) const;
  void SetLoop(u32 slot_index, b32 loop);

  // UseOwn�p
  void UseOwn_SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame);
  f32  UseOwn_GetUnitStepFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitStartFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitEndFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame);
  b32  UseOwn_IsUnitFrameStartFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitFrameStepTerminalFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitLoop(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop);

  // �u�����h����
  void SetBlendWeight(u32 slot_index, u32 unit_index, f32 weight);
  f32  GetBlendWeight(u32 slot_index, u32 unit_index);




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friend�N���X������A�N�Z�X���Ă͂Ȃ�Ȃ������o�֐�
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  void SetAnimObjectAllSlot(b32 is_not_null);  // is_not_null��false�̂Ƃ�NULL��ݒ肷��
  void SetAnimObject(u32 slot_index, b32 is_not_null);  // is_not_null��false�̂Ƃ�NULL��ݒ肷��
      // SetAnimObjectAllSlot��SetAnimObject�͎��Ă��邪�X�s�[�h�d���̂��ߗp�ӂ����B




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friend�N���X������A�N�Z�X���Ă͂Ȃ�Ȃ������o�ϐ�
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  Object*         m_object;  // ����AnimationManager�������o�Ɏ��I�u�W�F�N�g�̃|�C���^���o���Ă���


  // AnimationSlot
  u32             m_slot_num;
  AnimationSlot*  m_slot_array;


  // �}�l�[�W�����L�A�j���[�V����(�}�l�[�W���������A�j�����s��)
  b32              m_use_own_animation;
  s32              m_own_slot_blend_num_max;  // �eSlot�ɂ���UnitAnimation�̌�
  s32              m_own_unit_animation_num_max;  // �eSlot�ɂ���UnitAnimation�ɓo�^�ł���Animation�̌�
  s32              m_own_unit_animation_num_over;  // = m_own_unit_animation_num_max+1
                                                   // �eUnitAnimation�ɓo�^�ł���Animation�̌����傫���l
                                                   // UnitAnimation�ɂ�Animation�̓���ւ����ł���悤�ɁA�o�^�ς݂�m_own_unit_animation_num_max�A
                                                   // �I�[�o�[�������ꂩ��o�^���Ēǂ��o����ڂɂȂ�悤�ɂ��Ă���B
  b32*             m_own_set_slot_array;   // �X���b�g�̃u�����h�������w�肵�����Ƃ�����ꍇ��true�A�Ȃ��ꍇ��false
  Animation****    m_own_animation_array;  // �SSlot�A�SUnitAnimation�����v����Animation�𑽎����z��ŋ�؂�
                                           // �eUnitAnimation�ɓo�^�ł���Animation�̌����1������UnitAnimation�ɂē���ւ����ł���悤�ɂ��Ă���B
                                           // m_own_animation_array[m_slot_num][m_own_slot_blend_num_max][m_own_unit_animation_num_over];
  UnitAnimation*** m_own_unit_animation_array;  // �SSlot��UnitAnimation�𑽎����z��ŋ�؂�
                                                // m_own_unit_animation_array[m_slot_num][m_own_slot_blend_num_max];
                                                // m_own_unit_animation_array[i][j]�ɂ�m_own_unit_animation_num_max��Animation���o�^�ł���B
  s32**            m_own_unit_animation_next_index_array;  // �eUnitAnimation�ɓo�^����Animation�����͂ǂꂩ�������C���f�b�N�X
                                                           // m_own_unit_animation_array[m_slot_num][m_own_slot_blend_num_max];
                                                           // m_own_unit_animation_array[i][j]�ɓo�^���鎟�̃A�j���[�V������
                                                           // m_own_animation_array[i][j][ m_own_unit_animation_next_index_array[i][j] ]�ł���
                                                           // �����o�^������m_own_unit_animation_next_index_array[i][j]��
                                                           // (m_own_unit_animation_next_index_array[i][j]+1)%(m_own_unit_animation_num_over)�ɂȂ�B
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DANIMATIONMANAGER_H__

