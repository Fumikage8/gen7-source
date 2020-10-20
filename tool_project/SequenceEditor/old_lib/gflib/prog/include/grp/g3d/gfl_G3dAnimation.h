#ifndef __GFL_G3DANIMATION_H__
#define __GFL_G3DANIMATION_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimation.h
 *	@brief  3D�A�j���[�V����
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dIAnimation.h>
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
///	3D�A�j���[�V�����N���X
//=====================================
class Animation : public IAnimation
{
  GFL_FORBID_COPY_AND_ASSIGN(Animation);

  friend class UnitAnimation;
  friend class AnimationSlot;
  friend class AnimationManager;

public:
  struct AnimationPack
  {
    Animation* animation;
    f32        weight;
  };

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  Animation(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~Animation();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����𐶐�����
   *
   *  @param[in]     device_allocator         �f�o�C�X�A���P�[�^
   *  @param[in]     res_anim                 �A�j���[�V�������\�[�X
   *  @param[in]     resource                 ���\�[�X
   *  @param[in]     animation_resource_type  �A�j���[�V�������\�[�X�̃^�C�v
   *  @param[in]     index_in_resource        ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     max_members              �A�j���[�V�����Ώۃ����o�̍ő吔
   *                                          TransformAnimEvaluator::Bind �ɓn�� AnimGroup �� AnimGroup::GetMemberCount �̒l��ݒ肵�Ă��������B
   *                                          ������ AnimGroup �� Bind ����ꍇ�́A�ő�l��ݒ肵�Ă��������B
   *  @param[in]     max_anim_members         ���ۂɃA�j���[�V�������郁���o�̍ő吔��ݒ肵�܂��B
   *                                          AnimData() �ɓn�� anim::res::ResAnim �� anim::res::ResAnim::GetMemberAnimSetCount �̒l��ݒ肵�Ă��������B
   *                                          AnimEvaluator::ChangeAnim �ŕ����� ResAnim ��؂�ւ���ꍇ�́A�ő�l��ݒ肵�Ă��������B
   *                                          gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                          �����œn����res_anim(resource)��GetMemberAnimSetCount���g�p���܂��B
   *                                          gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                          �����œn����max_members�ɂ��܂��B
   * 
   *  �����ɂ���Change�́A���̂ǂꂩ�̏������s���܂��B
   *  �E���ɐ�������Ă���A�j���[�V�����Ɠ���^�C�v�ł���Ȃ�A�V���ɐ������邱�ƂȂ��A�j���[�V������ύX����(��������ɂ���Change�Ɠ����̏���)�B
   *  �E���ɐ�������Ă���A�j���[�V�����ƈقȂ�^�C�v�ł���Ȃ�ADestroy�������Create����B
   *  �E�A�j���[�V��������������Ă��Ȃ��Ȃ�ACreate����B
   */
  //-----------------------------------------------------------------------------
  void CreateAnimation(
    gfl::heap::NwAllocator*  device_allocator,
    ResAnim*                 res_anim,
    s32                      max_members,
    s32                      max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator*  device_allocator,
    ResAnim*                 res_anim,
    s32                      max_members,
    s32                      max_anim_members
  );
  void CreateAnimation(
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    s32                              max_members,
    s32                              max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    s32                              max_members,
    s32                              max_anim_members
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ԃu�����h�A�j���[�V�����𐶐�����
   *  @brief         ���Z�u�����h�A�j���[�V�����𐶐�����
   *
   *  @param[in]     heap_allocator           �f�o�C�X�A���P�[�^
   *  @param[in]     device_allocator         �f�o�C�X�A���P�[�^
   *  @param[in]     type                     �A�j���[�V�������\�[�X�̃^�C�v
   *                                          pack_array�œn���A�j���[�V�����̃^�C�v�͑S�Ă��̃^�C�v�ɂȂ��Ă��Ȃ���΂Ȃ�Ȃ�
   *  @param[in]     num_max                  �u�����h����A�j���[�V�����̍ő吔
   *                                          1�̂Ƃ������u�����h����Ȃ��̂Ńu�����h�p��Animation�𐶐����Ȃ��ق����悢
   *  @param[in]     num                      pack_array�̗v�f��
   *                                          num<=num_max
   *                                          0��n���Ă��悢
   *  @param[in]     pack_array               num�̗v�f���������z��
   *                                          ���̔z��̏��ԂɃC���f�b�N�X�����蓖�Ă���(pack_array[n].animation�Ȃ�n��)
   *                                          num��0�̂Ƃ���NULL��n���Ă��悢
   *                                          pack_array�ɗ񋓂���animation�͒u�������Ȃ����肱�̃N���X�Ń|�C���^��ێ����Ă���̂ŁA
   *                                          �Ăяo�����Ŕj�����Ȃ��ŉ������Banimation�����c���Ă����pack_array�z��͔j�����Ă��\���܂���B
   *  @param[in]     ignore_no_anim_member    �A�j���[�V�����̑��݂��Ȃ������o�𖳎�����ꍇ��true
   *  @param[in]     normalization_enable     �A�j���[�V�����̃u�����h�d�݂𐳋K������ꍇ��true
   *
   *  �����ɂ���Change�́A���̂ǂꂩ�̏������s���܂��B
   *  �E���ɐ�������Ă���A�j���[�V�����Ɠ���^�C�v�ł���Ȃ�A�V���ɐ������邱�ƂȂ��A�j���[�V������ύX����(��������ɂ���Change�Ɠ����̏���)�B
   *  �E���ɐ�������Ă���A�j���[�V�����ƈقȂ�^�C�v�ł���Ȃ�ADestroy�������Create����B
   *  �E�A�j���[�V��������������Ă��Ȃ��Ȃ�ACreate����B
   */
  //-----------------------------------------------------------------------------
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

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������j������:
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *
   *  @param[in]     res_anim                 �A�j���[�V�������\�[�X
   *  @param[in]     resource                 ���\�[�X
   *  @param[in]     animation_resource_type  �A�j���[�V�������\�[�X�̃^�C�v
   *  @param[in]     index_in_resource        ���\�[�X���ɂ�����C���f�b�N�X
   *
   *  �����ɂ���Change�́A���ɐ�������Ă���A�j���[�V�����Ɠ���^�C�v�łȂ���Ύg�p�ł��܂���B
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    ResAnim*                 res_anim
  );
  void ChangeAnimation(
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ԃu�����h�A�j���[�V������ύX����
   *  @brief         ���Z�u�����h�A�j���[�V������ύX����
   *
   *  @param[in]     num                      pack_array�̗v�f��
   *                                          num<=m_blend_num_max
   *                                          0��n���Ă��悢
   *  @param[in]     pack_array               num�̗v�f���������z��
   *                                          ���̔z��̏��ԂɃC���f�b�N�X�����蓖�Ă���(pack_array[n].animation�Ȃ�n��)
   *                                          num��0�̂Ƃ���NULL��n���Ă��悢
   *                                          pack_array�ɗ񋓂���animation�͒u�������Ȃ����肱�̃N���X�Ń|�C���^��ێ����Ă���̂ŁA
   *                                          �Ăяo�����Ŕj�����Ȃ��ŉ������Banimation�����c���Ă����pack_array�z��͔j�����Ă��\���܂���B
   *  @param[in]     normalization_enable     �A�j���[�V�����̃u�����h�d�݂𐳋K������ꍇ��true
   * 
   *  �����ɂ���Change�́A���ɐ�������Ă���A�j���[�V�����Ɠ���^�C�v�łȂ���Ύg�p�ł��܂���B
   */
  //-----------------------------------------------------------------------------
  void ChangeBlendInterpolator(
    s32                     num,
    AnimationPack*          pack_array,
    b32                     normalization_enable  = true
  );
  void ChangeBlendAdder(
    s32                     num,
    AnimationPack*          pack_array
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃^�C�v�𓾂�
   *
   *  @retval        Type     �A�j���[�V�����̃^�C�v
   */
  //-----------------------------------------------------------------------------
  virtual ResAnim::Type GetType(void) const  {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
   *
   *  @param[in]     frame      �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
   *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
   *
   *  @param[in]     step_frame      �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
   *
   *  @retval        �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
   *
   *  @param[in]     frame      �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
   *
   *  @param[in]     frame      �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
   *
   *  @param[in]     start_frame    �J�n�t���[��
   *  @param[in]     end_frame      �I���t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����J�n�t���[�����ۂ����肷��
   *
   *  @retval        bool    ���݂̃t���[�����J�n�t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
   *
   *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[����step_frame���猩�Ē[�t���[�����ۂ����肷��
   *
   *  @retval        bool    ���݂̃t���[����step_frame���猩�Ē[�t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameStepTerminalFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *
   *  @retval        b32       ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *
   *  @param[in]     loop    ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  virtual void SetLoop(b32 loop);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃u�����h�d�݂�ݒ肷��
   *
   *  @param[in]     index   �u�����h���Ă���A�j���[�V�����̃C���f�b�N�X
   *  @param[in]     weight  �E�F�C�g
   *
   *  @retval        true�̂Ƃ��ݒ�ł����Afalse�̂Ƃ��ݒ�ł��Ă��Ȃ�(���R��index�ɃA�j���[�V�������Ȃ��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  b32 SetBlendWeight(s32 index, f32 weight);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃u�����h�d�݂��擾����
   *
   *  @param[in]     index   �u�����h���Ă���A�j���[�V�����̃C���f�b�N�X
   * 
   *  @retval        �E�F�C�g
   */
  //-----------------------------------------------------------------------------
  f32 GetBlendWeight(s32 index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �u�����h����A�j���[�V������ǉ�����
   *
   *  @param[in]     index       animation�����蓖�Ă�C���f�b�N�X
   *  @param[in]     animation   �ǉ�����A�j���[�V����
   *                             animation�͒u�������Ȃ����肱�̃N���X�Ń|�C���^��ێ����Ă���̂ŁA
   *                             �Ăяo�����Ŕj�����Ȃ��ŉ������B
   *                             NULL�̂Ƃ��Aindex�Ɋ��蓖�Ă��Ă���A�j���[�V�������Ȃ��ɂ���
   *  @param[in]     weight      �E�F�C�g
   *                             animation��NULL�̂Ƃ���weight�͎g�p���Ȃ��̂łǂ�Ȓl�ł��\��Ȃ�
   * 
   *  @retval        �ǉ��ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 SetBlendAnimation(s32 index, Animation* animation, f32 weight);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��ԃu�����A�j���[�V�����̃u�����h�d�݂̐��K���̐ݒ�/�擾
   *
   *  @param[in]     enable      true�̂Ƃ����K������
   * 
   *  @retval        true�̂Ƃ����K������
   */
  //-----------------------------------------------------------------------------
  void SetBlendInterpolatorNormalizationEnable(b32 enable);
  b32  IsBlendInterpolatorNormalizationEnable(void) const;

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::gfx::AnimObject*���擾
   *
   *  @retval        nw::gfx::AnimObject*    ���̃N���X���ێ����Ă���nw::gfx::AnimObject
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::AnimObject* GetNwAnimObject(void) const  {  return m_anim_object;  }

  // �A�j���[�V�����̐�(�u�����h�A�j���[�V�����̂Ƃ��͂��̒��g����ł�1��Ԃ�)
  virtual s32 GetAnimationNum(void) const; 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�������֘A�t����
   *
   *  @param[in]     nw_anim_group      �A�j���[�V�����O���[�v
   *  @retval        b32           �o�C���h�ɐ���������true��Ԃ�
   *
   *  m_nw_anim_object��nw_anim_group���o���Ă����悤�ɂȂ�B
   */
  //-----------------------------------------------------------------------------
  b32 Bind(nw::gfx::AnimGroup* nw_anim_group);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         max_anim_members�����߂�
   *
   *  @param[in]     res_anim
   *  @param[in]     max_members
   *  @param[in]     max_anim_members  ���Ƃ��Ƃ̒l
   *
   *  @retval        max_anim_members  ���߂Ȃ������l
   */
  //-----------------------------------------------------------------------------
  static s32 GetMaxAnimMembers(ResAnim* res_anim, const s32 max_members, const s32 max_anim_members);

private:
  // ���
  enum State
  {
    STATE_NONE,                          // �A�j���[�V�����𐶐����Ă��Ȃ�
    STATE_EVALUATOR_SKELETAL,            // �X�P���^���A�j���[�V�����̕]���N���X�𐶐����Ă���
    STATE_EVALUATOR_OTHERS,              // �X�P���^���A�j���[�V�����ȊO�̕]���N���X�𐶐����Ă���
    STATE_BLEND_INTERPOLATOR_SKELETAL,   // �X�P���^���A�j���[�V�����̕�ԃu�����h�N���X�𐶐����Ă���
    STATE_BLEND_INTERPOLATOR_OTHERS,     // �X�P���^���A�j���[�V�����ȊO�̕�ԃu�����h�N���X�𐶐����Ă���
    STATE_BLEND_ADDER_SKELETAL,          // �X�P���^���A�j���[�V�����̉��Z�u�����h�N���X�𐶐����Ă���
    STATE_BLEND_ADDER_OTHERS             // �X�P���^���A�j���[�V�����ȊO�̉��Z�u�����h�N���X�𐶐����Ă���
  };

private:
  // �S�ĂŎg�p���郁���o�ϐ�
  State                            m_state;
  ResAnim::Type                    m_type;

  // m_state��
  // STATE_EVALUATOR_SKELETAL
  // STATE_EVALUATOR_OTHERS
  // �̂Ƃ��g�p���郁���o�ϐ�
  s32                              m_max_members;
  s32                              m_max_anim_members;

  // m_state��
  // STATE_BLEND_INTERPOLATOR_SKELETAL
  // STATE_BLEND_INTERPOLATOR_OTHERS
  // STATE_BLEND_ADDER_SKELETAL
  // STATE_BLEND_ADDER_OTHERS
  // �̂Ƃ��g�p���郁���o�ϐ�
  s32                              m_blend_num_max;
  Animation**                      m_blend_animation_array;  // ���̂Ƃ���Ő��������A�j���[�V�������o���Ă��邾���̃|�C���^
                                                             // �o���Ă����Ă��g��Ȃ��̂łȂ��Ă��悳�����A�Ȃ����Ȃ�heap_allocator��n���Ȃ��čς�
  
  // �S�ĂŎg�p���郁���o�ϐ�
  nw::gfx::AnimObject*             m_anim_object;         // �A�j���[�V�����]���N���X or �A�j���[�V�����u�����h�N���X
  nw::gfx::AnimGroup*              m_bind_anim_group;     // �o�C���h���Ă���A�j���[�V�����O���[�v���o���Ă����BNULL�̂Ƃ��o�C���h���Ă��Ȃ��B
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DANIMATION_H__
