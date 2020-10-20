#ifndef __GFL_G3DH3DRESANIM_H__
#define __GFL_G3DH3DRESANIM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResAnim.h
 *	@brief  H3D���\�[�X�A�j���[�V����
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*����
����H3dResAnim�ł�
nw::h3d::res::SkeletalAnimContent
nw::h3d::res::MaterialAnimContent
nw::h3d::res::VisibilityAnimContent
nw::h3d::res::LightAnimContent������͂܂������Ă��܂���
nw::h3d::res::CameraAnimContent������͂܂������Ă��܂���
nw::h3d::res::FogAnimContent������͂܂������Ă��܂���
�������܂��B
H3dResource���g���A�j���[�V�����ł��B*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dContent.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

// �O���錾
class H3dUserData;



//-------------------------------------
///	H3D�A�j���[�V�����R���e���g�N���X
//=====================================
class H3dResAnim : public H3dContent
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dResAnim);

  friend class H3dAnimationSlot;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ��`
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // @brief  �A�j���[�V�����v�f�̃o�C���h��̌^
  enum AnimPrimitiveType
  {
    // NintendoWare/CTR/include/nw/h3d/h3d_Const.h
    // struct AnimPrimitiveType
    //     enum enum_t

    ANIM_PRIMITIVE_TYPE_FLOAT                 = nw::h3d::AnimPrimitiveType::FLOAT                ,
    ANIM_PRIMITIVE_TYPE_INT                   = nw::h3d::AnimPrimitiveType::INT                  ,
    ANIM_PRIMITIVE_TYPE_VECTOR2               = nw::h3d::AnimPrimitiveType::VECTOR2              ,
    ANIM_PRIMITIVE_TYPE_VECTOR3               = nw::h3d::AnimPrimitiveType::VECTOR3              ,
    ANIM_PRIMITIVE_TYPE_TRANSFORM             = nw::h3d::AnimPrimitiveType::TRANSFORM            ,
    ANIM_PRIMITIVE_TYPE_RGBA_COLOR            = nw::h3d::AnimPrimitiveType::RGBA_COLOR           ,
    ANIM_PRIMITIVE_TYPE_TEXTURE               = nw::h3d::AnimPrimitiveType::TEXTURE              ,
    ANIM_PRIMITIVE_TYPE_BAKED_TRANSFORM       = nw::h3d::AnimPrimitiveType::BAKED_TRANSFORM      ,
    ANIM_PRIMITIVE_TYPE_BAKED_BOOL            = nw::h3d::AnimPrimitiveType::BAKED_BOOL           ,
    ANIM_PRIMITIVE_TYPE_FULL_BAKED_TRANSFORM  = nw::h3d::AnimPrimitiveType::FULL_BAKED_TRANSFORM ,
    //ANIM_PRIMITIVE_TYPE_BOOL                // ��Ή�
    ANIM_PRIMITIVE_TYPE_INVALID               = -2
  };

  // @brief  �萔
  enum
  {
    INDEX_NONE  = -1  // �Ȃ��Ƃ��̃C���f�b�N�X
  };


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �\����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  1�r�b�g�x�N�g�� 
  struct BITVEC4
  {
    u32 x: 1;
    u32 y: 1;
    u32 z: 1;
    u32 w: 1;
  };
  typedef BITVEC4 BITVEC3;
  typedef BITVEC4 BITVEC2;

  // @brief  u32�x�N�g�� 
  struct U32VEC3
  {
    u32 x;
    u32 y;
    u32 z;
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R���X�g���N�^ / �f�X�g���N�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  H3dResAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dResAnim();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���� / �j��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // ���N���X�̂܂�



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���[�U�f�[�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  s32 GetUserDataCount() const;
  // �y���Ӂz���[�U�[�f�[�^�͐擪�� $ (�_���[) ��t���������O�̃��^�f�[�^�Ƃ��ĕێ�����܂��B
  //         $�����Y��Ȃ��悤�ɁB
  s32 GetUserDataIndex(const char* name) const;
  void GetUserData(const s32 user_data_index, H3dUserData* user_data) const;

private:
  nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* getMetaDataMap(void) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �ݒ� / �擾
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����^�C�v�𓾂�
   */
  //-----------------------------------------------------------------------------
  AnimationType GetAnimationType(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����v�f�̐��𓾂�
   */
  //-----------------------------------------------------------------------------
  u32 GetAnimElementNum(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[�����𓾂�
   */
  //-----------------------------------------------------------------------------
  f32 GetFrameSize(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[�����𐳋K������
   */
  //-----------------------------------------------------------------------------
  f32 NormalizeFrame(f32 frame) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
   *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 IsLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
   *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
   */
  //-----------------------------------------------------------------------------
  void SetLoop(b32 loop);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // AnimElementData
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-------------------------------------
  // ����
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����v�f�̃C���f�b�N�X����o�C���h��̖��O���擾����
   *  @param[in]     anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @retval        const char*         �o�C���h��̖��O
   *                                    �C���f�b�N�X���s���̂Ƃ�NULL��Ԃ�
   */
  //-----------------------------------------------------------------------------
  const char* GetTargetName(const s32 anim_element_index) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����v�f�̃o�C���h��̖��O����C���f�b�N�X���擾����
   *  @param[in]     name  �o�C���h��̖��O
   *  @retval        s32   �C���f�b�N�X
   *                       �Ȃ��Ƃ�INDEX_NONE��Ԃ�
   */
  //-----------------------------------------------------------------------------
  s32 GetAnimElementIndex(const char* target_name) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�����v�f�̃o�C���h��̌^���擾����
   *  @param[in]     anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @retval        AnimPrimitiveType   �o�C���h��̌^
   *                                     �C���f�b�N�X���s���̂Ƃ�ANIM_PRIMITIVE_TYPE_INVALID��Ԃ�
   */
  //-----------------------------------------------------------------------------
  AnimPrimitiveType GetAnimPrimitiveType(const s32  anim_element_index) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_FLOAT
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l
   *
   *  @retval    �L���Ȓl�̏ꍇ��true��Ԃ��B�L���Ȓl�łȂ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetFloatValue(const s32 anim_element_index, const f32 frame, f32* value) const;
  b32 GetFloatValue(const s32 anim_element_index, const s32 frame, f32* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_INT
  //-------------------------------------
  // �Ȃ�

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_VECTOR2
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�B
   *                                    valid.x��1�̂Ƃ�value.x�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.y��1�̂Ƃ�value.y�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *  @param[out]   valid               �l���L���̂Ƃ�1�A�����̂Ƃ�0�B
   *
   *  @retval    1�ł��L���Ȓl������ꍇ��true��Ԃ��B1���L���Ȓl���Ȃ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetVector2Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC2* value, BITVEC2* valid) const;
  b32 GetVector2Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC2* value, BITVEC2* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_VECTOR3
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�B
   *                                    valid.x��1�̂Ƃ�value.x�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.y��1�̂Ƃ�value.y�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.z��1�̂Ƃ�value.z�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *  @param[out]   valid               �l���L���̂Ƃ�1�A�����̂Ƃ�0�B
   *
   *  @retval    1�ł��L���Ȓl������ꍇ��true��Ԃ��B1���L���Ȓl���Ȃ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetVector3Value(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetVector3Value(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�B
   *                                    valid.x��1�̂Ƃ�value.x�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.y��1�̂Ƃ�value.y�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.z��1�̂Ƃ�value.z�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *  @param[out]   valid               �l���L���̂Ƃ�1�A�����̂Ƃ�0�B
   *
   *  @retval    1�ł��L���Ȓl������ꍇ��true��Ԃ��B1���L���Ȓl���Ȃ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  b32 GetTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  b32 GetTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;
  b32 GetTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value, BITVEC3* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_RGBA_COLOR
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�B
   *                                    valid.x��1�̂Ƃ�value.x�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.y��1�̂Ƃ�value.y�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.z��1�̂Ƃ�value.z�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.w��1�̂Ƃ�value.w�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *  @param[out]   valid               �l���L���̂Ƃ�1�A�����̂Ƃ�0�B
   *
   *  @retval    1�ł��L���Ȓl������ꍇ��true��Ԃ��B1���L���Ȓl���Ȃ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetRgbaColorValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC4* value, BITVEC4* valid) const;
  b32 GetRgbaColorValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC4* value, BITVEC4* valid) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_TEXTURE
  //-------------------------------------
  // �Ȃ�

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l
   *
   *  @retval    �L���Ȓl�̏ꍇ��true��Ԃ��B�L���Ȓl�łȂ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedTransformScaleValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const;
  b32 GetBakedTransformScaleValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const;

  b32 GetBakedTransformRotateValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const;
  b32 GetBakedTransformRotateValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const;

  b32 GetBakedTransformTranslateValue(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value) const;
  b32 GetBakedTransformTranslateValue(const s32 anim_element_index, const s32 frame, gfl::math::VEC3* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_BAKED_BOOL
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l
   *
   *  @retval    �L���Ȓl�̏ꍇ��true��Ԃ��B�L���Ȓl�łȂ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetBakedBoolValue(const s32 anim_element_index, const f32 frame, b32* value) const;
  b32 GetBakedBoolValue(const s32 anim_element_index, const s32 frame, b32* value) const;

  //-------------------------------------
  // ANIM_PRIMITIVE_TYPE_FULL_BAKED_TRANSFORM
  //-------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l
   *
   *  @retval    �L���Ȓl�̏ꍇ��true��Ԃ��B�L���Ȓl�łȂ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  b32 GetFullBakedTransformValue(const s32 anim_element_index, const f32 frame, gfl::math::MTX34* value) const;
  b32 GetFullBakedTransformValue(const s32 anim_element_index, const s32 frame, gfl::math::MTX34* value) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ͌��J���Ă������J�����o
  // �h���N���X���A�N�Z�X���郁���o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
protected:
  const nw::h3d::res::AnimContent* GetNwAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::AnimContent*>(m_cont));
  }
  const nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::SkeletalAnimContent*>(m_cont));
  }
  const nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::MaterialAnimContent*>(m_cont));
  }
  const nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::VisibilityAnimContent*>(m_cont));
  }
  const nw::h3d::res::LightAnimContent* GetNwLightAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::LightAnimContent*>(m_cont));
  }
  const nw::h3d::res::CameraAnimContent* GetNwCameraAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::CameraAnimContent*>(m_cont));
  }
  const nw::h3d::res::FogAnimContent* GetNwFogAnimContent(void) const
  {
    return (reinterpret_cast<const nw::h3d::res::FogAnimContent*>(m_cont));
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �h���N���X���A�N�Z�X���郁���o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
protected:
  nw::h3d::res::AnimContent*  m_cont;  // m_res���̃R���e���g



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �����Ŏg�������o
  // friend�͎g�p�֎~
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  DIM                FLOAT      = 1
   *                     VECTOR2    = 2
   *                     VECTOR3    = 3
   *                     RGBA_COLOR = 4
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�Bf32 value[DIM]��n�����ƁB
   *                                    valid[i]��true�̂Ƃ�value[i]�ɂ͂�����Ƃ����l�������Ă���B
   *                                    valid[i]��false�̂Ƃ�value[i]�͕s��B
   *  @param[out]   valid               �L���Ȓl�̂Ƃ�true�A�����Ȓl�̂Ƃ�false�Bb32 valid[DIM]��n�����ƁB
   */
  //-----------------------------------------------------------------------------
  template<s32 DIM>
  void getFloatVectorValue(const s32 anim_element_index, const f32 frame, f32* value, b32* valid) const
  {
    // f32 value[DIM];
    // b32 valid[DIM];


    // nw::h3d::res::AnimFloatVectorElementData
    // 
    // NintendoWare/CTR/include/nw/h3d/res/h3d_AnimFloatVectorElementData.h
    //
    // typedef AnimFloatVectorElementData<1> AnimFloatElementData;
    // typedef AnimFloatVectorElementData<2> AnimVector2ElementData;
    // typedef AnimFloatVectorElementData<3> AnimVector3ElementData;
    // typedef AnimFloatVectorElementData<4> AnimColorElementData;
    //
    // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
    //
    // template<int Dim>
    // template<class Context>
    // bit32 AnimFloatVectorElementData<Dim>::CalcAndApply(f32* values, Context context) const
 
    if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() )
    {
      nw::h3d::res::AnimFloatVectorElementData<DIM>* element = static_cast<nw::h3d::res::AnimFloatVectorElementData<DIM>*>(m_cont->animElements[anim_element_index]);
    
      for(s32 i=0; i<DIM; ++i)
      {
        if( !(element->flags & (nw::h3d::AnimFloatVectorElementFlag::NOT_EXIST_0 << i)) )
        {
          value[i] = (element->flags & (nw::h3d::AnimFloatVectorElementFlag::CONSTANT_0 << i)) ?
                  element->component[i].constant : element->component[i].curve->Calc(frame);
          valid[i] = true;
        }
        else
        {
          valid[i] = false;
        }
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index);
      for(s32 i=0; i<DIM; ++i)
      {
        valid[i] = false;
      }
    }
  }


  //-----------------------------------------------------------------------------
  /**
   *  @brief     �w�肵���t���[���ɂ�����l�𓾂�
   *
   *  name               scale
   *                     rotate
   *                     translate
   *
   *  @param[in]    anim_element_index  �C���f�b�N�X(0<= <GetAnimElementNum())
   *  @param[in]    frame               �t���[��
   *  @param[out]   value               �l�B
   *                                    valid.x��1�̂Ƃ�value.x�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.y��1�̂Ƃ�value.y�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *                                    valid.z��1�̂Ƃ�value.z�ɂ͂�����Ƃ����l�������Ă���B0�̂Ƃ��s��B
   *  @param[out]   valid               �l���L���̂Ƃ�1�A�����̂Ƃ�0�B
   *
   *  @param[in]    flag_not_exist      nw::h3d::AnimTransformElementFlag::NOT_EXIST_SCALE_X�Ȃ�
   *  @param[in]    flag_constant       nw::h3d::AnimTransformElementFlag::CONSTANT_SCALE_X�Ȃ�
   *
   *  @retval    1�ł��L���Ȓl������ꍇ��true��Ԃ��B1���L���Ȓl���Ȃ��ꍇ��false��Ԃ��B
   */
  //-----------------------------------------------------------------------------
  
  // NintendoWare/CTR/sources/libraries/h3d/h3d_AnimResourceImpl.h
  //
  // template <class Context>
  // void AnimTransformElementData::CalcAndApply(math::Transform3* transform3, Context context) const

#define GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(name) \
  b32 getTransformValue_##name(const s32 anim_element_index, const f32 frame, gfl::math::VEC3* value, BITVEC3* valid, U32VEC3* flag_not_exist, U32VEC3* flag_constant) const \
  { \
    if( 0<=anim_element_index && anim_element_index<GetAnimElementNum() ) \
    { \
      nw::h3d::res::AnimTransformElementData* element = static_cast<nw::h3d::res::AnimTransformElementData*>(m_cont->animElements[anim_element_index]); \
      if( !(element->flags & flag_not_exist->x) ) \
      { \
        value->x = (element->flags & flag_constant->x) ? \
                element->name##X.constant : element->name##X.curve->Calc(frame); \
        valid->x = 1; \
      } \
      else \
      { \
        valid->x = 0; \
      } \
      if( !(element->flags & flag_not_exist->y) ) \
      { \
        value->y = (element->flags & flag_constant->y) ? \
                element->name##Y.constant : element->name##Y.curve->Calc(frame); \
        valid->y = 1; \
      } \
      else \
      { \
        valid->y = 0; \
      } \
      if( !(element->flags & flag_not_exist->z) ) \
      { \
        value->z = (element->flags & flag_constant->z) ? \
                element->name##Z.constant : element->name##Z.curve->Calc(frame); \
        valid->z = 1; \
      } \
      else \
      { \
        valid->z = 0; \
      } \
    } \
    else \
    { \
      GFL_ASSERT_MSG(0, "anim_element_index = %d\n", anim_element_index); \
      valid->x = 0; \
      valid->y = 0; \
      valid->z = 0; \
    } \
    return (valid->x || valid->y || valid->z)?true:false; \
  }

  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(scale)
  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(rotate)
  GFL_GRP_G3D_H3D_RES_ANIM_GET_TRANSFORM_VALUE(translate)


};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DRESANIM_H__

