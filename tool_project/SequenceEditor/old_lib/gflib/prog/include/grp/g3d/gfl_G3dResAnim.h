#ifndef __GFL_G3DRESANIM_H__
#define __GFL_G3DRESANIM_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResAnim.h
 *	@brief  3D�A�j���[�V�������\�[�X
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/anim.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dUserData.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�A�j���[�V�������\�[�X�N���X
//=====================================
class ResAnim : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(ResAnim);

  // �t�����h�N���X
  friend class Animation;
  friend class Resource;


public:
  // �A�j���[�V�������\�[�X�̃^�C�v
  enum Type  // @note gfl::grp::g3d::AnimationType�����ɂ������̂ŁA����������B
  {
    TYPE_NONE        = ANIMATION_TYPE_NONE,
    TYPE_SKELETAL    = ANIMATION_TYPE_SKELETAL,
    TYPE_MATERIAL    = ANIMATION_TYPE_MATERIAL,
    TYPE_VISIBILITY  = ANIMATION_TYPE_VISIBILITY,
    TYPE_CAMERA      = ANIMATION_TYPE_CAMERA,
    TYPE_LIGHT       = ANIMATION_TYPE_LIGHT
  };

  // �����o�[�A�j���[�V�����̌^
  enum MemberAnimType
  {
    MEMBER_ANIM_TYPE_FLOAT             = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT,
    MEMBER_ANIM_TYPE_INT               = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT,
    MEMBER_ANIM_TYPE_BOOL              = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL, 
    MEMBER_ANIM_TYPE_VECTOR2           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2,  
    MEMBER_ANIM_TYPE_VECTOR3           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3,   
    MEMBER_ANIM_TYPE_TRANSFORM         = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM,  
    MEMBER_ANIM_TYPE_RGBA_COLOR        = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR,  
    MEMBER_ANIM_TYPE_TEXTURE           = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE,  
    MEMBER_ANIM_TYPE_BAKED_TRANSFORM   = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM,
    MEMBER_ANIM_TYPE_FULL_BAKED        = nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED,     
    MEMBER_ANIM_TYPE_INVALID           = -2
  };

  // ��p��ResCameraAnim�N���X�����邩�ǂ�����������
  // ResCameraAnim�̃J�����s��̃A�b�v�f�[�g�^�C�v
  enum ViewUpdaterKind
  {
    VIEW_UPDATER_KIND_NONE     = -1,
    VIEW_UPDATER_KIND_AIM      = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_AIM,
    VIEW_UPDATER_KIND_LOOKAT   = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_LOOKAT,
    VIEW_UPDATER_KIND_ROTATE   = nw::anim::res::ResCameraAnimData::VIEW_UPDATER_ROTATE
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]     allocator          NULL  ��allocator�͎g��Ȃ��Ȃ�܂����B
   *  @param[in]     resource           ���\�[�X
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[in]     type               �A�j���[�V�������\�[�X�̃^�C�v
   */
  //-----------------------------------------------------------------------------
  // void Resource::GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const; ���g�p���ĉ������B
  GFL_GRP_G3D_DEPRECATED_FUNCTION( virtual void Create(
      gfl::heap::NwAllocator*           allocator, 
      Resource*                         resource,
      s32                               index_in_resource,
      Type                              type               = TYPE_SKELETAL
  ) );
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�������\�[�X�̃^�C�v�𓾂�
   *
   *  @retval        Type    �A�j���[�V�������\�[�X�̃^�C�v
   */
  //-----------------------------------------------------------------------------
  Type GetType(void) const  {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  ResAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~ResAnim();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim�C���X�^���X�����S�ɃR�s�[���܂��B
   *
   *                 �R�s�[���͔j�����Ă����v�ł����A�R�s�[�����w���Ă���Resource�͔j�����Ȃ��ŉ������B
   *                 ResAnim���p�������N���X�̂��Ƃ͍l�����Ă��܂���̂ŁA
   *                 �p�������N���X���������ꍇ�͂���ɍ��킹��Copy�֐��������ĉ������B
   *                 �I�u�W�F�N�gID�̓R�s�[���ƈ�������̂ɂȂ�܂��B
   *
   *  @param[in]  src  �R�s�[��
   */
  //-----------------------------------------------------------------------------
  void Copy(const ResAnim& src);


public:
  //-----------------------------------------------------------------------------
  /*
   *    �����o�[�A�j���[�V�����̏��
   */
  //-----------------------------------------------------------------------------
  //-------------------------------------
  // ����
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim�̖��O���擾����
   *
   *  @retval        const char*    ���O
   */
  //-----------------------------------------------------------------------------
  const char* GetName(void) const; 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[�������擾����
   *
   *  @retval        f32   �t���[����
   */
  //-----------------------------------------------------------------------------
  f32 GetFrameSize(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�[�A�j���[�V�����̗v�f�����擾����
   *
   *  @retval        s32    �����o�[�A�j���[�V�����̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimSetCount(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ����疼�O���擾����
   *
   *  @param[in]     member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *
   *  @retval        const char*    �����o�[�A�j���[�V�����̗v�f�̖��O
   */
  //-----------------------------------------------------------------------------
  const char* GetMemberAnimSetPath(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���O���烁���o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ����擾����
   *
   *  @param[in]     key    �����o�[�A�j���[�V�����̗v�f�̖��O
   *
   *  @retval        s32    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimSetIndex(const char* key) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�[�A�j���[�V�����̌^���擾����
   *
   *  @param[in]     member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *
   *  @retval        MemberAnimTypes    �����o�[�A�j���[�V�����̌^
   */
  //-----------------------------------------------------------------------------
  MemberAnimType GetMemberAnimType(const s32 member_anim_set_index) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_FLOAT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetFloatValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  f32 GetFloatValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_INT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  s32 GetIntValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  s32 GetIntValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_BOOL
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  b32 GetBoolValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  b32 GetBoolValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_VECTOR2
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��x�̔���A1��y�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   *             true���Ԃ��Ă����Ƃ��ł��x�N�g���̊e�v�f�ɐ���Ȓl������Ƃ͌���܂���̂ŁA�e�v�f�̔����value_valid�ōs���ĉ������B
   */
  //-----------------------------------------------------------------------------
  b32 GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL) const;
  b32 GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�x�̒l���擾����A1�̂Ƃ�y�̒l���擾����
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetVector2Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetVector2Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_VECTOR3
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��x�̔���A1��y�̔���A2��z�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   *             true���Ԃ��Ă����Ƃ��ł��x�N�g���̊e�v�f�ɐ���Ȓl������Ƃ͌���܂���̂ŁA�e�v�f�̔����value_valid�ōs���ĉ������B
   */
  //-----------------------------------------------------------------------------
  b32 GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�x�̒l���擾����A1�̂Ƃ�y�̒l���擾����A2�̂Ƃ�z�̒l���擾����
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetVector3Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetVector3Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �X�P�[���̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��x�̔���A1��y�̔���A2��z�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   *             true���Ԃ��Ă����Ƃ��ł��x�N�g���̊e�v�f�ɐ���Ȓl������Ƃ͌���܂���̂ŁA�e�v�f�̔����value_valid�ōs���ĉ������B
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �X�P�[���̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�x�̒l���擾����A1�̂Ƃ�y�̒l���擾����A2�̂Ƃ�z�̒l���擾����
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���s�ړ��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��x�̔���A1��y�̔���A2��z�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   *             true���Ԃ��Ă����Ƃ��ł��x�N�g���̊e�v�f�ɐ���Ȓl������Ƃ͌���܂���̂ŁA�e�v�f�̔����value_valid�ōs���ĉ������B
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  b32 GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���s�ړ��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�x�̒l���擾����A1�̂Ƃ�y�̒l���擾����A2�̂Ƃ�z�̒l���擾����
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���W�ϊ���\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��rotate��x�̔���A1��rotate��y�̔���A2��rotate��z�̔���A
   *                                                            3��translate��x�̔���A4��translate��y�̔���A5��translate��z�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   *             false���Ԃ��Ă����Ƃ��A�}�g���N�X�̂ǂ̗v�f�Ɍ��������邩��value_valid�ŕ�����܂��B
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  b32 GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���W�ϊ���\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element_row              row�s column��
   *  @param[in]    element_column           f._12�Ȃ�row=1, column=2
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��rotate��x�̔���A1��rotate��y�̔���A2��rotate��z�̔���A
   *                                                            3��translate��x�̔���A4��translate��y�̔���A5��translate��z�̔���)
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s��
   *             value_valid�̂ǂꂩ��false�̂Ƃ��A����Ɏ擾�ł��Ă��܂���B
   */
  //-----------------------------------------------------------------------------
  f32 GetTransformMatrixValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;
  f32 GetTransformMatrixValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column, \
      b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL, b32* value4_valid=NULL, b32* value5_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_RGBA_COLOR
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true(0��r�̔���A1��g�̔���A2��b�̔���A3��a�̔���)
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL) const;
  b32 GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid=NULL, b32* value1_valid=NULL, b32* value2_valid=NULL, b32* value3_valid=NULL) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�r�̒l���擾����A1�̂Ƃ�g�̒l���擾����A2�̂Ƃ�b�̒l���擾����A3�̂Ƃ�a�̒l���擾����
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true 
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetRgbaColorValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid=NULL) const;
  f32 GetRgbaColorValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_TEXTURE
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[out]   value_valid              ����Ȓl�̂Ƃ�true
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid=NULL) const;
  s32 GetTextureValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid=NULL) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �X�P�[���̃A�j���[�V���������݂��邩�ǂ��� 
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *
   *  @retval    ���݂���Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformScale(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ��]�̃A�j���[�V���������݂��邩�ǂ��� 
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *
   *  @retval    ���݂���Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformRotate(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���s�ړ��̃A�j���[�V���������݂��邩�ǂ��� 
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *
   *  @retval    ���݂���Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 DoesExistBakedTransformTranslate(const s32 member_anim_set_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �X�P�[���̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �X�P�[���̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element                  0�Ƃ�x�̒l���擾����A1�̂Ƃ�y�̒l���擾����A2�̂Ƃ�z�̒l���擾����
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element) const;
  f32 GetBakedTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ��]��\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ��]��\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element_row              row�s column��
   *  @param[in]    element_column           f._12�Ȃ�row=1, column=2
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformRotateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetBakedTransformRotateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���s�ړ���\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     ���s�ړ���\���s��̃A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element_row              row�s column��
   *  @param[in]    element_column           f._12�Ȃ�row=1, column=2
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetBakedTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetBakedTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;

  //-------------------------------------
  // MEMBER_ANIM_TYPE_FULL_BAKED
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[out]   value                    frame�ɂ�����A�j���[�V�������ʂ̒l��Ԃ�
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *
   *  @retval    ����Ɏ擾�ł����Ƃ�true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const;
  b32 GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �A�j���[�V�������ʂ̒l�𓾂�
   *
   *  @param[in]    member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
   *  @param[in]    frame                    �t���[��
   *  @param[in]    element_row              row�s column��
   *  @param[in]    element_column           f._12�Ȃ�row=1, column=2
   *
   *  @retval    frame�ɂ�����A�j���[�V�������ʂ̒l
   *             ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
   */
  //-----------------------------------------------------------------------------
  f32 GetFullBakedValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const;
  f32 GetFullBakedValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�U�f�[�^
   */
  //-----------------------------------------------------------------------------
  s32 GetUserDataCount() const;
  s32 GetUserDataIndex(const char* key) const;
  void GetUserData(const s32 user_data_index, UserData* user_data) const;


  // ��p��ResCameraAnim�N���X�����邩�ǂ�����������
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResCameraAnim��ViewUpdaterKind���擾����
   *
   *  @retval        ViewUpdaterKind
   *                 ResAnim��ResCameraAnim�łȂ��ꍇ�́AVIEW_UPDATER_KIND_NONE��Ԃ�
   */
  //-----------------------------------------------------------------------------
  ViewUpdaterKind GetResCameraAnimViewUpdaterKind(void) const;
  
  // �t�����h�N���X����̂݃A�N�Z�X����private�����o�֐�
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X���ێ����Ă���nw::anim::ResAnim���擾
   *
   *  @retval        nw::anim::ResAnim    ���̃N���X���ێ����Ă���nw::anim::ResAnim
   */
  //-----------------------------------------------------------------------------
  nw::anim::ResAnim GetNwResAnim(void) const {  return m_nw_res_anim;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃N���X�ŕێ�����悤nw::anim::ResAnim��ݒ�
   *
   *  @param[in]     type           �A�j���[�V�������\�[�X�̃^�C�v
   *  @param[in]     nw_res_anim    �A�j���[�V�������\�[�X
   */
  //-----------------------------------------------------------------------------
  void SetNwResAnim(Resource::AnimationResourceType type, nw::anim::ResAnim nw_res_anim);

  
  // �t�����h�N���X����A�N�Z�X���Ă͂Ȃ�Ȃ�private�����o
private:
  enum
  {
    // ���̕��я����g�p���Ă���ӏ�������̂ŁA���я��͕ς��Ȃ��悤�ɂ��ĉ������B
    VALUE_VALID_SCALE_X,
    VALUE_VALID_SCALE_Y,
    VALUE_VALID_SCALE_Z,
    VALUE_VALID_ROTATE_X,
    VALUE_VALID_ROTATE_Y,
    VALUE_VALID_ROTATE_Z,
    VALUE_VALID_TRANSLATE_X,
    VALUE_VALID_TRANSLATE_Y,
    VALUE_VALID_TRANSLATE_Z,
    VALUE_VALID_MAX
  };

  // �t�����h�N���X����A�N�Z�X���Ă͂Ȃ�Ȃ�private�����o�֐�
private:
  //-----------------------------------------------------------------------------
  /*
   *    �����o�[�A�j���[�V�����̏��
   */
  //-----------------------------------------------------------------------------
  s32 GetMemberAnimTypeSize(const s32 member_anim_set_index) const;
  b32 GetTransformValue(nw::gfx::CalculatedTransform* value, const s32 member_anim_set_index, const f32 frame, b32 value_valid_array[VALUE_VALID_MAX]) const;
  void DoesExistBakedTransform(b32* scale_exist, b32* rotate_exist, b32* translate_exist, const s32 member_anim_set_index) const;


private:
  Type               m_type;
  nw::anim::ResAnim  m_nw_res_anim;  // ���̂Ƃ���ɂ��郊�\�[�X���o���Ă��������ŁA������j���͂��Ȃ��B
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRESANIM_H__
