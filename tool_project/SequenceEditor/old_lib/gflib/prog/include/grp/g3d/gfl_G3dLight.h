#ifndef __GFL_G3DLIGHT_H__
#define __GFL_G3DLIGHT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLight.h
 *	@brief  3D���C�g
 *	@author	Koji Kawada
 *	@date		2010.10.12
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

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D���C�g�N���X
//=====================================
class Light : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Light);

  friend class AnimationManager;
  friend class RenderSystem;
  friend class LightSet;

public:
  enum Type
  {
    TYPE_NONE,
    TYPE_AMBIENT,
    TYPE_FRAGMENT,
    TYPE_HEMI_SPHERE,
    TYPE_VERTEX
  };
  enum Kind
  {
    KIND_NONE,
    KIND_DIRECTIONAL,
    KIND_POINT,
    KIND_SPOT
  };

  // ���\�[�X���烉�C�g�𐶐�����Ƃ��̐ݒ���e
  struct ResourceDescription
  {
    b32  animation_enable;          // �A�j���[�V�������s���ꍇ��true(�A�j���[�V���������蓖�Ă�X���b�g��1����܂�)
    u32  animation_blend_unit_num;  // �X���b�g���ŃA�j���[�V�����̃u�����h���s���ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł��X���b�g����1���j�b�g������܂�)
    u32  animation_num_in_unit;     // ���j�b�g���ŕ�ԃu�����h���Ȃ���A�j���[�V������ύX����ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł����j�b�g����1����܂�)
    ResourceDescription(void)
      : animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  static void InitializeResourceDescriptionForDefault(
      ResourceDescription*  description
  );

  // �ݒ���e
  struct Description
  {
    Type  type;
    Kind  kind;
    b32 animation_enable;          // �A�j���[�V�������s���ꍇ��true(�A�j���[�V���������蓖�Ă�X���b�g��1����܂�)
    u32 animation_blend_unit_num;  // �X���b�g���ŃA�j���[�V�����̃u�����h���s���ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł��X���b�g����1���j�b�g������܂�)
    u32 animation_num_in_unit;     // ���j�b�g���ŕ�ԃu�����h���Ȃ���A�j���[�V������ύX����ꍇ2�ȏ�A�s��Ȃ��ꍇ0��1(0�ł�1�ł����j�b�g����1����܂�)
    Description(void)
      : type(TYPE_NONE),
        kind(KIND_NONE),
        animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );
  static void InitializeDescriptionForAmbientLight(
      Description*  description
  );
  static void InitializeDescriptionForFragmentLight(
      Description*  description,
      const Kind    kind
  );
  static void InitializeDescriptionForHemiSphereLight(
      Description*  description
  );
  static void InitializeDescriptionForVertexLight(
      Description*  description,
      const Kind    kind
  );


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  Light(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Light();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          �A���P�[�^
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     description        ���\�[�X���烉�C�g�𐶐�����Ƃ��̐ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      const ResourceDescription*        description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     description        �ݒ���e
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           device_allocator,
      const Description*                description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �v�Z����
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void);


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V������ύX����
   *                 ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
   *
   *  @param[in]     allocator               �A���P�[�^
   *  @param[in]     res_anim                �A�j���[�V�������\�[�X
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B
   *                                         ���߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
   *                                         Resource::GetLightMemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );

  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index��unit_index�Ɋ��蓖�Ă�A�j���[�V������ύX����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
   *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
   *  @param[in]     unit_index              �X���b�g���ɂ��������j�b�g�̃C���f�b�N�X
   *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
   *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
   *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
   *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
   *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
   *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̃t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
   *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
   *
   *  @param[in]     step_frame      �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
   *
   *  @retval        �X�V�t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
   *
   *  @param[in]     frame           �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̏I���t���[�����擾����
   *
   *  @retval        �t���[��
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
   *
   *  @param[in]     start_frame     �J�n�t���[��
   *  @param[in]     end_frame       �I���t���[��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
   *
   *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *
   *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *
   *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(bool loop);



  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[������
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 unit_index) const;
  void SetUnitStepFrame(u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 unit_index) const;
  void SetUnitStartFrame(u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 unit_index) const;
  void SetUnitEndFrame(u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 unit_index) const;
  b32  IsUnitLoop(u32 unit_index) const;
  void SetUnitLoop(u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index���ɂ��������j�b�g�̃A�j���[�V�������u�����h����
   *
   *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
   *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
   *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
   *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_index�̃A�j���[�V�������Ȃ��ɂ���
   *                                         AnimationSlot::MODE_ANIMATION           �u�����h����߂�animation_unit_index�Ŏw�肵���A�j���[�V���������蓖�Ă�
   *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  ��ԃu�����h
   *                                         AnimationSlot::MODE_BLEND_ADDER         ���Z�u�����h
   *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
   *                                         ���j�b�g0�̃u�����h�d��
   *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
   *                                         ���j�b�g1�̃u�����h�d��
   *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
   *                                                      false�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�̓o�C���h���̒l���u�����h����܂��B
   *                                                      true�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�͏d�� 0 �Ƃ��ău�����h����܂��B
   *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
   *                                                      true�ɂ���ƁA�u�����h�d�݂����v�� 1 �ɂȂ�悤���K�����Ă���u�����h���s���܂��B
   *                                                      false�ɂ���ƁA�ݒ肵���d�݂����̂܂܃u�����h�Ɏg�p�����B
   *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATION�̂Ƃ��̂ݎg�p�B
   *                                                      �X���b�g���̂ǂ̃��j�b�g�ɂ���A�j���[�V���������蓖�Ă邩���w�肷��B
   */
  //-----------------------------------------------------------------------------
  void BlendUnitAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      f32                        weight0,
      f32                        weight1,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �u�����h�d��
   */
  //-----------------------------------------------------------------------------
  void SetUnitBlendWeight(u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
   *
   *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
   *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(void);




public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���s�ړ�
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  void SetDirection(f32 x, f32 y, f32 z);
  void SetDirection(const gfl::math::VEC3& d);
  void GetDirection(gfl::math::VEC3* d) const;
  gfl::math::VEC3 GetDirection(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X���擾����
   *
   *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
   *
   *  @retval        bool            ����Ɏ擾�ł�����true
   */
  //-----------------------------------------------------------------------------
  bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[���h�}�g���N�X��ݒ肷��
   *                 Scene��Calculate�̌�łȂ��Ɣ��f����܂���B
   *
   *  @param[in]     matrix          �ݒ肷��}�g���N�X
   *
   *  @retval        bool            ����ɐݒ�ł�����true
   */
  //-----------------------------------------------------------------------------
  bool SetWorldMatrix(const gfl::math::MTX34& matrix);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J���[
   */
  //-----------------------------------------------------------------------------
  void SetAmbient(f32 r, f32 g, f32 b);
  void SetAmbient(f32 r, f32 g, f32 b, f32 a);
  void SetAmbient(gfl::grp::ColorF32& c);
  void SetAmbient(u8 r, u8 g, u8 b);
  void SetAmbient(u8 r, u8 g, u8 b, u8 a);
  void SetAmbient(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetAmbient(void) const;
  void GetAmbient(gfl::grp::ColorF32* c) const;
  void GetAmbient(gfl::grp::ColorU8*  c) const;

  void SetDiffuse(f32 r, f32 g, f32 b);
  void SetDiffuse(f32 r, f32 g, f32 b, f32 a);
  void SetDiffuse(gfl::grp::ColorF32& c);
  void SetDiffuse(u8 r, u8 g, u8 b);
  void SetDiffuse(u8 r, u8 g, u8 b, u8 a);
  void SetDiffuse(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetDiffuse(void) const;
  void GetDiffuse(gfl::grp::ColorF32* c) const;
  void GetDiffuse(gfl::grp::ColorU8*  c) const;

  void SetSpecular0(f32 r, f32 g, f32 b);
  void SetSpecular0(f32 r, f32 g, f32 b, f32 a);
  void SetSpecular0(gfl::grp::ColorF32& c);
  void SetSpecular0(u8 r, u8 g, u8 b);
  void SetSpecular0(u8 r, u8 g, u8 b, u8 a);
  void SetSpecular0(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSpecular0(void) const;
  void GetSpecular0(gfl::grp::ColorF32* c) const;
  void GetSpecular0(gfl::grp::ColorU8*  c) const;

  void SetSpecular1(f32 r, f32 g, f32 b);
  void SetSpecular1(f32 r, f32 g, f32 b, f32 a);
  void SetSpecular1(gfl::grp::ColorF32& c);
  void SetSpecular1(u8 r, u8 g, u8 b);
  void SetSpecular1(u8 r, u8 g, u8 b, u8 a);
  void SetSpecular1(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSpecular1(void) const;
  void GetSpecular1(gfl::grp::ColorF32* c) const;
  void GetSpecular1(gfl::grp::ColorU8*  c) const;

  void SetGroundColor(f32 r, f32 g, f32 b);
  void SetGroundColor(f32 r, f32 g, f32 b, f32 a);
  void SetGroundColor(gfl::grp::ColorF32& c);
  void SetGroundColor(u8 r, u8 g, u8 b);
  void SetGroundColor(u8 r, u8 g, u8 b, u8 a);
  void SetGroundColor(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetGroundColor(void) const;
  void GetGroundColor(gfl::grp::ColorF32* c) const;
  void GetGroundColor(gfl::grp::ColorU8*  c) const;

  void SetSkyColor(f32 r, f32 g, f32 b);
  void SetSkyColor(f32 r, f32 g, f32 b, f32 a);
  void SetSkyColor(gfl::grp::ColorF32& c);
  void SetSkyColor(u8 r, u8 g, u8 b);
  void SetSkyColor(u8 r, u8 g, u8 b, u8 a);
  void SetSkyColor(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSkyColor(void) const;
  void GetSkyColor(gfl::grp::ColorF32* c) const;
  void GetSkyColor(gfl::grp::ColorU8*  c) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ��������
   */
  //-----------------------------------------------------------------------------
  void SetDistanceLookUpTable(
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �p�x����
   */
  //-----------------------------------------------------------------------------
#if 0
  //�p�x�����p�̎Q�ƃe�[�u����Get�͂ł��Ă�Set�͂ł��Ȃ��B
  void SetAngleLookUpTable(
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );
#endif


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �L���t���O
   */
  //-----------------------------------------------------------------------------
  void SetEnable(bool enable);
  bool IsEnable(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  virtual Type GetType(void) const  {  return m_type;  }
  virtual Kind GetKind(void) const  {  return m_kind;  }

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::Light*            GetNwLight(void) const            {  return nw::ut::DynamicCast<nw::gfx::Light*>( GetNwSceneNode() );  }
  virtual nw::gfx::AmbientLight*     GetNwAmbientLight(void) const     {  return nw::ut::DynamicCast<nw::gfx::AmbientLight*>( GetNwSceneNode() );  }
  virtual nw::gfx::FragmentLight*    GetNwFragmentLight(void) const    {  return nw::ut::DynamicCast<nw::gfx::FragmentLight*>( GetNwSceneNode() );  }
  virtual nw::gfx::HemiSphereLight*  GetNwHemiSphereLight(void) const  {  return nw::ut::DynamicCast<nw::gfx::HemiSphereLight*>( GetNwSceneNode() );  }
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
  virtual nw::gfx::VertexLight*      GetNwVertexLight(void) const      {  return nw::ut::DynamicCast<nw::gfx::VertexLight*>( GetNwSceneNode() );  }
#endif 

  virtual nw::gfx::res::ResAmbientLight     GetNwResAmbientLight(void) const;
  virtual nw::gfx::res::ResFragmentLight    GetNwResFragmentLight(void) const;
  virtual nw::gfx::res::ResHemiSphereLight  GetNwResHemiSphereLight(void) const;
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
  virtual nw::gfx::res::ResVertexLight      GetNwResVertexLight(void) const;
#endif 

  virtual void CheckTypeAndKind(void);


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
   *  @param[in]     description        �ݒ���e
   */
  //-----------------------------------------------------------------------------
  void CreateAmbientLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateFragmentLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateHemiSphereLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateVertexLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );


private:
  Type   m_type;
  Kind   m_kind;
  //Animation*  m_animation;
  u32                m_animation_slot_num;  // 0��1  // 0�̂Ƃ��A�j���[�V������t�����Ȃ��A1�̂Ƃ��A�j���[�V������t������(���ۂɕt���Ă��邩�ǂ�����m_animation_manager�Ŋm�F���ĉ�����)�B
  u32                m_animation_blend_unit_num;
  u32                m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DLIGHT_H__
