#ifndef __GFL_G3DANIMATIONSLOT_H__
#define __GFL_G3DANIMATIONSLOT_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationSlot.h
 *	@brief  3D�A�j���[�V���������蓖�Ă�X���b�g
 *	@author	Koji Kawada
 *	@date		2011.10.27
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
#include <grp/g3d/gfl_G3dIAnimation.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dUnitAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {

//-------------------------------------
///	�A�j���[�V���������蓖�Ă�X���b�g
//=====================================
class AnimationSlot
{
  GFL_FORBID_COPY_AND_ASSIGN(AnimationSlot);

  friend class AnimationManager;


public:
  struct AnimationPack
  {
    IAnimation* animation;
    f32         weight;
  };

  enum Mode
  {
    MODE_NONE,
    MODE_ANIMATION,
    MODE_BLEND_INTERPOLATOR,
    MODE_BLEND_ADDER
  };

  struct Description
  {
    Mode             mode;
    IAnimation*      animation;
    ResAnim::Type    blend_type;
    s32              blend_num_max;
    s32              blend_num;
    AnimationPack*   blend_pack_array;
    b32              interpolator_ignore_no_anim_member;
    b32              interpolator_normalization_enable;

    Description(void)
      : mode(MODE_NONE),
        animation(NULL),
        blend_type(ResAnim::TYPE_NONE),
        blend_num_max(0),
        blend_num(0),
        blend_pack_array(NULL),
        interpolator_ignore_no_anim_member(false),
        interpolator_normalization_enable(true)
    {
    }
  };

  static void InitializeDescriptionForDefault(
      Description*  description
  );
  static void InitializeDescriptionForAnimation(
      Description*  description,
      IAnimation*   animation
  );
  static void InitializeDescriptionForBlendInterpolator(
      Description*     description,
      ResAnim::Type    blend_type,
      s32              blend_num_max,
      s32              blend_num,
      AnimationPack*   blend_pack_array,
      b32              interpolator_ignore_no_anim_member = false,
      b32              interpolator_normalization_enable = true
  );
  static void InitializeDescriptionForBlendAdder(
      Description*     description,
      ResAnim::Type    blend_type,
      s32              blend_num_max,
      s32              blend_num,
      AnimationPack*   blend_pack_array
  );


public:
  AnimationSlot(void);
  virtual ~AnimationSlot();

  void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Description*            description = NULL
  );
  
  void CreateAnimation(
    IAnimation*              animation
  );
  void CreateBlendInterpolator(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array,
    b32                     ignore_no_anim_member = false,
    b32                     normalization_enable  = true
  );
  void ChangeBlendInterpolator(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array,
    b32                     ignore_no_anim_member = false,
    b32                     normalization_enable  = true
  );
  void CreateBlendAdder(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array
  );
  void ChangeBlendAdder(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array
  );

  virtual void Destroy(void);

  // �v�Z����
  // �K�����t���[���Ă�ŉ������B
  virtual void Calculate(void);

  // ���݂̃A�j���[�V�����𓾂�
  // �K�����t���[��Calculate�̌�ŌĂсA�Ăяo�����Ŏg�p���Ă���A�j���[�V�����̃|�C���^��ύX���ĉ������B
  // Calculate�ł��̃A�j���[�V�����̃|�C���^��������Ă��܂���������Ȃ��̂ŁACalculate�̑O�ɂ͌Ă΂Ȃ����ƁB
  nw::gfx::AnimObject* GetNwAnimObject(void) const;

  void ChangeAnimation(
    IAnimation*              animation
  );
  void ChangeBlendInterpolator(
    s32                     num,
    AnimationPack*          pack_array,
    b32                     normalization_enable  = true
  );
  void ChangeBlendAdder(
    s32                     num,
    AnimationPack*          pack_array
  );
 
  ResAnim::Type GetType(void) const  {  return m_type;  }


  // �A�j���[�V�����̐�(�u�����h�A�j���[�V�����̂Ƃ��͂��̒��g�𐔂���)
  s32 GetAnimationNum(void) const;


  void SetFrame(f32 frame);
  f32 GetFrame(void) const;

  void SetStepFrame(f32 step_frame);
  f32 GetStepFrame(void) const;

  void SetStartFrame(f32 frame);
  f32 GetStartFrame(void) const;
  void SetEndFrame(f32 frame);
  f32 GetEndFrame(void) const;
  void SetStartAndEndFrame(f32 start_frame, f32 end_frame);
  bool IsFrameStartFrame(void) const;
  bool IsFrameEndFrame(void) const;
  bool IsFrameStepTerminalFrame(void) const;

  bool IsLoop(void) const;
  void SetLoop(bool loop);

  b32 SetBlendWeight(s32 index, f32 weight);
  f32 GetBlendWeight(s32 index) const;

  b32 SetBlendAnimation(s32 index, IAnimation* animation, f32 weight);

  void SetBlendInterpolatorNormalizationEnable(b32 enable);
  b32  IsBlendInterpolatorNormalizationEnable(void) const;


private:
  // ���
  enum State
  {
    STATE_NONE,                          // �A�j���[�V�����𐶐����Ă��Ȃ�
    STATE_ONE,                           // �P���IAnimation�ł���m_animation�ɂȂ��Ă���
    STATE_BLEND_INTERPOLATOR_SKELETAL,   // �X�P���^���A�j���[�V�����̕�ԃu�����h�N���X�𐶐����Ă���
    STATE_BLEND_INTERPOLATOR_OTHERS,     // �X�P���^���A�j���[�V�����ȊO�̕�ԃu�����h�N���X�𐶐����Ă���
    STATE_BLEND_ADDER_SKELETAL,          // �X�P���^���A�j���[�V�����̉��Z�u�����h�N���X�𐶐����Ă���
    STATE_BLEND_ADDER_OTHERS             // �X�P���^���A�j���[�V�����ȊO�̉��Z�u�����h�N���X�𐶐����Ă���
  };


private:
  nw::gfx::AnimObject* GetNwAnimObject(void);
  b32 BindBlend(nw::gfx::AnimGroup* nw_anim_group);

  void SetAnimObjectIndex(s32 anim_object_index)
  {
    m_anim_object_index = anim_object_index;
  }
  s32  GetAnimObjectIndex(void) const
  {
    return m_anim_object_index;
  }

  State GetState(void) const
  {
    return static_cast<State>(m_state);
  }

  void SetEnable(b32 enable)
  {
    if(enable) m_enable = 1;
    else       m_enable = 0;
  }
  b32 IsEnable(void)
  {
    return m_enable;
  }

private:
  // �萔
  enum
  {
    ANIM_OBJECT_INDEX_NONE     = -1
  };

private:
  // �S�ĂŎg�p���郁���o�ϐ�
  s16                              m_enable;  // �L���̂Ƃ�1�A�����̂Ƃ�0
  s16                              m_state;   // State�^�Ƃ��Ďg������
  ResAnim::Type                    m_type;

  s32                              m_anim_object_index;
      // SetSkeletalAnimObject, SetMaterialAnimObject, SetVisibilityAnimObject �ɓn��objectIndex
      // �X���b�g�̃C���f�b�N�X�Ɠ����ɂ��Ă���

  // m_state��
  // STATE_ONE
  // �̂Ƃ��g�p���郁���o�ϐ�
  IAnimation*                      m_animation;  // ���̂Ƃ���Ő��������A�j���[�V�������o���Ă��邾���̃|�C���^ 

  // m_state��
  // STATE_BLEND_INTERPOLATOR_SKELETAL
  // STATE_BLEND_INTERPOLATOR_OTHERS
  // STATE_BLEND_ADDER_SKELETAL
  // STATE_BLEND_ADDER_OTHERS
  // �̂Ƃ��g�p���郁���o�ϐ�
  s32                              m_blend_num_max;
  IAnimation**                     m_blend_animation_array;  // ���̂Ƃ���Ő��������A�j���[�V�������o���Ă��邾���̃|�C���^
                                                             // ���t���[�������ɓo�^����Ă���UnitAnimation�ɃA�N�Z�X����̂ŕK�{�A������heap_allocator���K�{
  nw::gfx::AnimBlender*     m_anim_blender;                  // �A�j���[�V�����u�����h�N���X
  nw::gfx::AnimGroup*       m_anim_blender_bind_anim_group;  // �o�C���h���Ă���A�j���[�V�����O���[�v���o���Ă����BNULL�̂Ƃ��o�C���h���Ă��Ȃ��B
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DANIMATIONSLOT_H__

