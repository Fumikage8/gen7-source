//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResAnim.cpp
 *	@brief  3D�A�j���[�V�������\�[�X
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>
#include <nw/anim.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Base.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dResource.h>

#include <grp/g3d/gfl_G3dResAnim.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�A�j���[�V�������\�[�X�N���X
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     allocator          NULL  ��allocator�͎g��Ȃ��Ȃ�܂����B
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     type               �A�j���[�V�������\�[�X�̃^�C�v
 */
//-----------------------------------------------------------------------------
void ResAnim::Create(
    gfl::heap::NwAllocator*           /*allocator*/, 
    Resource*                         resource,
    s32                               index_in_resource,
    Type                              type
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�̃o�C�i�����\�[�X���擾�ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::anim::ResAnim res;
  
  switch( type )
  {
  case TYPE_SKELETAL:
    {
      res = nw_res_graphics_file->GetSkeletalAnims(index_in_resource);
    }
    break;
  case TYPE_MATERIAL:
    {
      res = nw_res_graphics_file->GetMaterialAnims(index_in_resource);
    }
    break;
  case TYPE_VISIBILITY:
    {
      res = nw_res_graphics_file->GetVisibilityAnims(index_in_resource);
    }
    break;
  case TYPE_CAMERA:
    {
      res = nw_res_graphics_file->GetCameraAnims(index_in_resource);
    }
    break;
  case TYPE_LIGHT:
    {
      res = nw_res_graphics_file->GetLightAnims(index_in_resource);
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V�������\�[�X�̃^�C�v���s���ł��B\n" );
      return;
    }
    //break;
  }

  m_type = type;
  m_nw_res_anim = res;  // ���\�[�X������A�P������ł����B

  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void ResAnim::Destroy(void)
{
  m_nw_res_anim = nw::anim::ResAnim();  // ���\�[�X������A�f�X�g���N�^�͌Ă΂Ȃ��Ă����B
  m_type = TYPE_NONE;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
ResAnim::ResAnim(void)
  : m_type(TYPE_NONE),
    m_nw_res_anim()
{
  // �l������Ă��Ȃ����̃`�F�b�N
  STATIC_ASSERT(MEMBER_ANIM_TYPE_FLOAT           != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_INT             != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_BOOL            != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_VECTOR2         != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_VECTOR3         != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_TRANSFORM       != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_RGBA_COLOR      != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_TEXTURE         != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_BAKED_TRANSFORM != MEMBER_ANIM_TYPE_INVALID);
  STATIC_ASSERT(MEMBER_ANIM_TYPE_FULL_BAKED      != MEMBER_ANIM_TYPE_INVALID);

  GFL_SINGLETON_INSTANCE(System)->GetResAnimManager()->AddObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
ResAnim::~ResAnim()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetResAnimManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
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
void ResAnim::Copy(const ResAnim& src)
{
  m_type = src.m_type;
  m_nw_res_anim = src.m_nw_res_anim;
}


//-----------------------------------------------------------------------------
/*
 *    �����o�[�A�j���[�V�����̏��
 */
//-----------------------------------------------------------------------------
//-------------------------------------
// ����
//-------------------------------------

//-----------------------------------------------------------------------------
/*
 *  @brief         ResAnim�̖��O���擾����
 *
 *  @retval        const char*    ���O
 */
//-----------------------------------------------------------------------------
const char* ResAnim::GetName(void) const
{
  return m_nw_res_anim.GetName();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �t���[�������擾����
 *
 *  @retval        f32   �t���[����
 */
//-----------------------------------------------------------------------------
f32 ResAnim::GetFrameSize(void) const
{
  return m_nw_res_anim.GetFrameSize();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����o�[�A�j���[�V�����̗v�f�����擾����
 *
 *  @retval        s32    �����o�[�A�j���[�V�����̗v�f��
 */
//-----------------------------------------------------------------------------
s32 ResAnim::GetMemberAnimSetCount(void) const
{
  return m_nw_res_anim.GetMemberAnimSetCount();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ����疼�O���擾����
 *
 *  @param[in]     member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
 *
 *  @retval        const char*    �����o�[�A�j���[�V�����̗v�f�̖��O
 */
//-----------------------------------------------------------------------------
const char* ResAnim::GetMemberAnimSetPath(const s32 member_anim_set_index) const
{
  nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
  return res_member_anim.GetPath();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���O���烁���o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ����擾����
 *
 *  @param[in]     key    �����o�[�A�j���[�V�����̗v�f�̖��O
 *
 *  @retval        s32    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
 */
//-----------------------------------------------------------------------------
s32 ResAnim::GetMemberAnimSetIndex(const char* key) const
{
  return m_nw_res_anim.GetMemberAnimSetIndex(key);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����o�[�A�j���[�V�����̌^���擾����
 *
 *  @param[in]     member_anim_set_index    �����o�[�A�j���[�V�����̗v�f�̃C���f�b�N�X�ԍ�
 *
 *  @retval        MemberAnimTypes    �����o�[�A�j���[�V�����̌^
 */
//-----------------------------------------------------------------------------
ResAnim::MemberAnimType ResAnim::GetMemberAnimType(const s32 member_anim_set_index) const
{
  MemberAnimType member_anim_type = MEMBER_ANIM_TYPE_INVALID;
  
  nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
  u32 primitive_type = res_member_anim.GetPrimitiveType();

  switch(primitive_type)
  {
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT:
    member_anim_type = MEMBER_ANIM_TYPE_FLOAT;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT:
    member_anim_type = MEMBER_ANIM_TYPE_INT;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL:
    member_anim_type = MEMBER_ANIM_TYPE_BOOL;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2:
    member_anim_type = MEMBER_ANIM_TYPE_VECTOR2;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3:
    member_anim_type = MEMBER_ANIM_TYPE_VECTOR3;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
    member_anim_type = MEMBER_ANIM_TYPE_TRANSFORM;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
    member_anim_type = MEMBER_ANIM_TYPE_RGBA_COLOR;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE:
    member_anim_type = MEMBER_ANIM_TYPE_TEXTURE;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM:
    member_anim_type = MEMBER_ANIM_TYPE_BAKED_TRANSFORM;
    break;
  case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED:
    member_anim_type = MEMBER_ANIM_TYPE_FULL_BAKED;
    break;
  }

  return member_anim_type;
}

//-------------------------------------
// MEMBER_ANIM_TYPE_FLOAT
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetFloatValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_FLOAT )
  {
    // NintendoWare/CTR/include/nw/anim/res/anim_ResAnim.h
    // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
    // ���Q�l�ɂ����B
    
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(float), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    float dstBufFloat;
    s32   originalValueNone = 0;
 
    void* dstBuf        = &dstBufFloat;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value_valid ) *value_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    return dstBufFloat;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value_valid ) *value_valid = false;
    return 0.0f;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetFloatValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid) const
{
  return GetFloatValue(member_anim_set_index, static_cast<f32>(frame), value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_INT
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
s32 ResAnim::GetIntValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_INT )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(s32), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    s32   dstBufInt;
    s32   originalValueNone = 0;
 
    void* dstBuf        = &dstBufInt;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value_valid ) *value_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    return dstBufInt;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value_valid ) *value_valid = false;
    return 0;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
s32 ResAnim::GetIntValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid) const
{
  return GetIntValue(member_anim_set_index, static_cast<f32>(frame), value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_BOOL
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
b32 ResAnim::GetBoolValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_BOOL )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(bool), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    bool  dstBufBool;
    s32   originalValueNone = 0;
 
    void* dstBuf        = &dstBufBool;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value_valid ) *value_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    return dstBufBool;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value_valid ) *value_valid = false;
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
b32 ResAnim::GetBoolValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid) const
{
  return GetBoolValue(member_anim_set_index, static_cast<f32>(frame), value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_VECTOR2
//-------------------------------------
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid, b32* value1_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_VECTOR2 )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(nw::math::VEC2), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    nw::math::VEC2  dstBufVector2;
    s32             originalValueNone = 0;
 
    void* dstBuf        = &dstBufVector2;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value0_valid ) *value0_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    if( value1_valid ) *value1_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1;
    *value = dstBufVector2;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value0_valid ) *value0_valid = false;
    if( value1_valid ) *value1_valid = false;
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetVector2Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid) const
{
  gfl::math::VEC2 value;
  b32             value_valid_array[2];
  if( GetVector2Value(&value, member_anim_set_index, frame, &value_valid_array[0], &value_valid_array[1]) )
  {
    if( value_valid ) 
    {
      *value_valid = value_valid_array[element];
    }
    return (element==0)?(value.x):(value.y);
  }
  else
  {
    if( value_valid ) *value_valid = false;
    return 0.0f;
  } 
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetVector2Value(gfl::math::VEC2* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid, b32* value1_valid) const
{
  return GetVector2Value(value, member_anim_set_index, static_cast<f32>(frame), value0_valid, value1_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetVector2Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid) const
{
  return GetVector2Value(member_anim_set_index, static_cast<f32>(frame), element, value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_VECTOR3
//-------------------------------------
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_VECTOR3 )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(nw::math::VEC3), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    nw::math::VEC3  dstBufVector3;
    s32             originalValueNone = 0;
 
    void* dstBuf        = &dstBufVector3;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value0_valid ) *value0_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    if( value1_valid ) *value1_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1;
    if( value2_valid ) *value2_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2;
    *value = dstBufVector3;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value0_valid ) *value0_valid = false;
    if( value1_valid ) *value1_valid = false;
    if( value2_valid ) *value2_valid = false;
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetVector3Value(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid) const
{
  gfl::math::VEC3 value;
  b32             value_valid_array[3];
  if( GetVector3Value(&value, member_anim_set_index, frame, &value_valid_array[0], &value_valid_array[1], &value_valid_array[2]) )
  {
    if(value_valid)
    {
      *value_valid = value_valid_array[element];
    }
    return (element==0)?(value.x):((element==1)?(value.y):(value.z));
  }
  else
  {
    if( value_valid ) *value_valid = false;
    return 0.0f;
  } 
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetVector3Value(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  return GetVector3Value(value, member_anim_set_index, static_cast<f32>(frame), value0_valid, value1_valid, value2_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetVector3Value(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid) const
{
  return GetVector3Value(member_anim_set_index, static_cast<f32>(frame), element, value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_TRANSFORM
//-------------------------------------
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  nw::gfx::CalculatedTransform dstBuf;
  b32 value_valid_array[VALUE_VALID_MAX];
  b32 ret = GetTransformValue(&dstBuf, member_anim_set_index, frame, value_valid_array);
  if(ret)
  {
    const gfl::math::VEC3& scale = dstBuf.Scale();
    *value = scale;
  }
  if( value0_valid ) *value0_valid = value_valid_array[VALUE_VALID_SCALE_X];
  if( value1_valid ) *value1_valid = value_valid_array[VALUE_VALID_SCALE_Y];
  if( value2_valid ) *value2_valid = value_valid_array[VALUE_VALID_SCALE_Z];
  return ret;
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid) const
{
  f32 ret = 0.0f;
  gfl::math::VEC3 value;
  b32 value_valid_array[VALUE_VALID_MAX];
  if( GetTransformScaleValue(&value, member_anim_set_index, frame, value_valid_array) )
  {
    ret = (element==0)?(value.x):((element==1)?(value.y):(value.z));
  }
  if( value_valid ) *value_valid = value_valid_array[(element==0)?(VALUE_VALID_SCALE_X):((element==1)?(VALUE_VALID_SCALE_Y):(VALUE_VALID_SCALE_Z))];
  return ret;
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  nw::gfx::CalculatedTransform dstBuf;
  b32 value_valid_array[VALUE_VALID_MAX];
  b32 ret = GetTransformValue(&dstBuf, member_anim_set_index, frame, value_valid_array);
  if(ret)
  {
    gfl::math::VEC3 translate;
    dstBuf.GetTranslate(&translate);
    *value = translate;
  }
  if( value0_valid ) *value0_valid = value_valid_array[VALUE_VALID_TRANSLATE_X];
  if( value1_valid ) *value1_valid = value_valid_array[VALUE_VALID_TRANSLATE_Y];
  if( value2_valid ) *value2_valid = value_valid_array[VALUE_VALID_TRANSLATE_Z];
  return ret;
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid) const
{
  f32 ret = 0.0f;
  gfl::math::VEC3 value;
  b32 value_valid_array[VALUE_VALID_MAX];
  if( GetTransformTranslateValue(&value, member_anim_set_index, frame, value_valid_array) )
  {
    ret = (element==0)?(value.x):((element==1)?(value.y):(value.z));
  }
  if( value_valid ) *value_valid = value_valid_array[(element==0)?(VALUE_VALID_TRANSLATE_X):((element==1)?(VALUE_VALID_TRANSLATE_Y):(VALUE_VALID_TRANSLATE_Z))];
  return ret;
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame, \
    b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid, b32* value4_valid, b32* value5_valid) const
{
  nw::gfx::CalculatedTransform dstBuf;
  b32 value_valid_array[VALUE_VALID_MAX];
  b32 ret = GetTransformValue(&dstBuf, member_anim_set_index, frame, value_valid_array);
  if(ret)
  {
    const gfl::math::MTX34& transform_matrix = dstBuf.TransformMatrix();
    *value = transform_matrix;
  }
  if( value0_valid ) *value0_valid = value_valid_array[VALUE_VALID_ROTATE_X];
  if( value1_valid ) *value1_valid = value_valid_array[VALUE_VALID_ROTATE_Y];
  if( value2_valid ) *value2_valid = value_valid_array[VALUE_VALID_ROTATE_Z];
  if( value3_valid ) *value3_valid = value_valid_array[VALUE_VALID_TRANSLATE_X];
  if( value4_valid ) *value4_valid = value_valid_array[VALUE_VALID_TRANSLATE_Y];
  if( value5_valid ) *value5_valid = value_valid_array[VALUE_VALID_TRANSLATE_Z];
  for(s32 i=VALUE_VALID_ROTATE_X; i<=VALUE_VALID_TRANSLATE_Z; ++i)
  {
    if( !(value_valid_array[i]) )
    {
      ret = false;
      break;
    }
  }
  return ret;
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s��  // row�s column�� f._12�Ȃ�row=1, column=2
f32 ResAnim::GetTransformMatrixValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column, \
    b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid, b32* value4_valid, b32* value5_valid) const
{
  f32 ret = 0.0f;
  gfl::math::MTX34 value;
  b32 value_valid_array[VALUE_VALID_MAX];
  if( GetTransformMatrixValue(&value, member_anim_set_index, frame, value_valid_array) )
  {
    gfl::math::VEC3 column_value = value.GetColumn(element_column);
    ret = (element_row==0)?(column_value.x):((element_row==1)?(column_value.y):(column_value.z));
  }
  if( value0_valid ) *value0_valid = value_valid_array[VALUE_VALID_ROTATE_X];
  if( value1_valid ) *value1_valid = value_valid_array[VALUE_VALID_ROTATE_Y];
  if( value2_valid ) *value2_valid = value_valid_array[VALUE_VALID_ROTATE_Z];
  if( value3_valid ) *value3_valid = value_valid_array[VALUE_VALID_TRANSLATE_X];
  if( value4_valid ) *value4_valid = value_valid_array[VALUE_VALID_TRANSLATE_Y];
  if( value5_valid ) *value5_valid = value_valid_array[VALUE_VALID_TRANSLATE_Z];
  return ret;
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  return GetTransformScaleValue(value, member_anim_set_index, static_cast<f32>(frame), value0_valid, value1_valid, value2_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid) const
{
  return GetTransformScaleValue(member_anim_set_index, static_cast<f32>(frame), element, value_valid);
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformTranslateValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid) const
{
  return GetTransformTranslateValue(value, member_anim_set_index, static_cast<f32>(frame), value0_valid, value1_valid, value2_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid) const
{
  return GetTransformTranslateValue(member_anim_set_index, static_cast<f32>(frame), element, value_valid);
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetTransformMatrixValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame,
    b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid, b32* value4_valid, b32* value5_valid) const
{
  return GetTransformMatrixValue(value, member_anim_set_index, static_cast<f32>(frame), \
         value0_valid, value1_valid, value2_valid, value3_valid, value4_valid, value5_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s��  // row�s column�� f._12�Ȃ�row=1, column=2
f32 ResAnim::GetTransformMatrixValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column, \
    b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid, b32* value4_valid, b32* value5_valid) const
{
  return GetTransformMatrixValue(member_anim_set_index, static_cast<f32>(frame), element_row, element_column, \
         value0_valid, value1_valid, value2_valid, value3_valid, value4_valid, value5_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_RGBA_COLOR
//-------------------------------------
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const f32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_RGBA_COLOR )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(nw::ut::ResFloatColor), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    // NintendoWare/CTR/include/nw/ut/ut_Color.h
    nw::ut::ResFloatColor  dstBufRgbaColor;
    s32                    originalValueNone = 0;
 
    void* dstBuf        = &dstBufRgbaColor;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value0_valid ) *value0_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    if( value1_valid ) *value1_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1;
    if( value2_valid ) *value2_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2;
    if( value3_valid ) *value3_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT3;
    *value = dstBufRgbaColor;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value0_valid ) *value0_valid = false;
    if( value1_valid ) *value1_valid = false;
    if( value2_valid ) *value2_valid = false;
    if( value3_valid ) *value3_valid = false;
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetRgbaColorValue(const s32 member_anim_set_index, const f32 frame, const u32 element, b32* value_valid) const
{
  gfl::grp::ColorF32 value;
  b32                value_valid_array[4];
  if( GetRgbaColorValue(&value, member_anim_set_index, frame, &value_valid_array[0], &value_valid_array[1], &value_valid_array[2], &value_valid_array[3]) )
  {
    if( value_valid )
    {
      *value_valid = value_valid_array[element];
    }
    return (element==0)?(value.r):(  (element==1)?(value.g):( (element==2)?(value.b):(value.a) )  );
  }
  else
  {
    if( value_valid ) *value_valid = false;
    return 0.0f;
  } 
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetRgbaColorValue(gfl::grp::ColorF32* value, const s32 member_anim_set_index, const s32 frame, b32* value0_valid, b32* value1_valid, b32* value2_valid, b32* value3_valid) const
{
  return GetRgbaColorValue(value, member_anim_set_index, static_cast<f32>(frame), value0_valid, value1_valid, value2_valid, value3_valid);
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetRgbaColorValue(const s32 member_anim_set_index, const s32 frame, const u32 element, b32* value_valid) const
{
  return GetRgbaColorValue(member_anim_set_index, static_cast<f32>(frame), element, value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_TEXTURE
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
s32 ResAnim::GetTextureValue(const s32 member_anim_set_index, const f32 frame, b32* value_valid) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_TEXTURE )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(nw::ut::Offset), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    // NintendoWare/CTR/include/nw/ut/ut_ResType.h
    nw::ut::Offset   dstBufTexture;
    s32              originalValueNone = 0;
 
    void* dstBuf        = &dstBufTexture;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueNone;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

    if( value_valid ) *value_valid = dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0;
    s32 value = s32(dstBufTexture); 
    return value;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    if( value_valid ) *value_valid = false;
    return 0;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
s32 ResAnim::GetTextureValue(const s32 member_anim_set_index, const s32 frame, b32* value_valid) const
{
  return GetTextureValue(member_anim_set_index, static_cast<f32>(frame), value_valid);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_BAKED_TRANSFORM
//-------------------------------------
// ���݂���Ƃ�true��Ԃ�
b32 ResAnim::DoesExistBakedTransformScale(const s32 member_anim_set_index) const
{
  b32 scale_exist;
  b32 rotate_exist;
  b32 translate_exist;
  DoesExistBakedTransform(&scale_exist, &rotate_exist, &translate_exist, member_anim_set_index);
  return scale_exist;
}
// ���݂���Ƃ�true��Ԃ� 
b32 ResAnim::DoesExistBakedTransformRotate(const s32 member_anim_set_index) const
{
  b32 scale_exist;
  b32 rotate_exist;
  b32 translate_exist;
  DoesExistBakedTransform(&scale_exist, &rotate_exist, &translate_exist, member_anim_set_index);
  return rotate_exist;
}
// ���݂���Ƃ�true��Ԃ� 
b32 ResAnim::DoesExistBakedTransformTranslate(const s32 member_anim_set_index) const
{
  b32 scale_exist;
  b32 rotate_exist;
  b32 translate_exist;
  DoesExistBakedTransform(&scale_exist, &rotate_exist, &translate_exist, member_anim_set_index);
  return translate_exist;
}

// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const f32 frame) const
{
  if( DoesExistBakedTransformScale(member_anim_set_index) )
  {
    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  

    nw::math::VEC3  scale;
    bit32           scale_flags = 0;
    res_baked_transform_anim.EvaluateScale(&scale, &scale_flags, frame);

    *value = scale;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�����݂��܂���B\n" );
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformScaleValue(const s32 member_anim_set_index, const f32 frame, const u32 element) const
{
  gfl::math::VEC3 value;
  if( GetBakedTransformScaleValue(&value, member_anim_set_index, frame) )
  {
    return (element==0)?(value.x):(  (element==1)?(value.y):(value.z)  );
  }
  else
  {
    return 0.0f;
  }
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const
{
  if( DoesExistBakedTransformRotate(member_anim_set_index) )
  {
    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  

    nw::math::MTX34  rotate;
    bit32            rotate_flags = 0;
    res_baked_transform_anim.EvaluateRotate(&rotate, &rotate_flags, frame);

    *value = rotate;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�����݂��܂���B\n" );
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformRotateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const
{
  gfl::math::MTX34 value;
  if( GetBakedTransformRotateValue(&value, member_anim_set_index, frame) )
  {
    gfl::math::VEC3 column_value = value.GetColumn(element_column);
    return (element_row==0)?(column_value.x):((element_row==1)?(column_value.y):(column_value.z));
  }
  else
  {
    return 0.0f;
  }
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const
{
  if( DoesExistBakedTransformTranslate(member_anim_set_index) )
  {
    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  

    nw::math::MTX34  translate;
    bit32            translate_flags = 0;
    res_baked_transform_anim.EvaluateTranslate(&translate, &translate_flags, frame);

    *value = translate;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�����݂��܂���B\n" );
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformTranslateValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const
{
  gfl::math::MTX34 value;
  if( GetBakedTransformTranslateValue(&value, member_anim_set_index, frame) )
  {
    gfl::math::VEC3 column_value = value.GetColumn(element_column);
    return (element_row==0)?(column_value.x):((element_row==1)?(column_value.y):(column_value.z));
  }
  else
  {
    return 0.0f;
  }
}

// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformScaleValue(gfl::math::VEC3* value, const s32 member_anim_set_index, const s32 frame) const
{
  return GetBakedTransformScaleValue(value, member_anim_set_index, static_cast<f32>(frame));
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformScaleValue(const s32 member_anim_set_index, const s32 frame, const u32 element) const
{
  return GetBakedTransformScaleValue(member_anim_set_index, static_cast<f32>(frame), element);
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformRotateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const
{
  return GetBakedTransformRotateValue(value, member_anim_set_index, static_cast<f32>(frame));
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformRotateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const
{
  return GetBakedTransformRotateValue(member_anim_set_index, static_cast<f32>(frame), element_row, element_column);
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetBakedTransformTranslateValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const
{
  return GetBakedTransformTranslateValue(value, member_anim_set_index, static_cast<f32>(frame));
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetBakedTransformTranslateValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const
{
  return GetBakedTransformTranslateValue(member_anim_set_index, static_cast<f32>(frame), element_row, element_column);
}

//-------------------------------------
// MEMBER_ANIM_TYPE_FULL_BAKED
//-------------------------------------
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const f32 frame) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_FULL_BAKED )
  {
    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
   
    // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
    // nw::ut::ResDynamicCast
    // ���Q�l�ɂ����B
    nw::anim::res::ResFullBakedAnim res_full_baked_anim(res_member_anim.ptr());

    nw::math::MTX34 transform;
    res_full_baked_anim.EvaluateTransform(&transform, frame);

    *value = transform;
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    return false;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetFullBakedValue(const s32 member_anim_set_index, const f32 frame, const u32 element_row, const u32 element_column) const
{
  gfl::math::MTX34 value;
  if( GetFullBakedValue(&value, member_anim_set_index, frame) )
  {
    gfl::math::VEC3 column_value = value.GetColumn(element_column);
    return (element_row==0)?(column_value.x):((element_row==1)?(column_value.y):(column_value.z));
  }
  else
  {
    return 0.0f;
  }
}
// ����Ɏ擾�ł����Ƃ�true��Ԃ�
b32 ResAnim::GetFullBakedValue(gfl::math::MTX34* value, const s32 member_anim_set_index, const s32 frame) const
{
  return GetFullBakedValue(value, member_anim_set_index, static_cast<f32>(frame));
}
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32 ResAnim::GetFullBakedValue(const s32 member_anim_set_index, const s32 frame, const u32 element_row, const u32 element_column) const
{
  return GetFullBakedValue(member_anim_set_index, static_cast<f32>(frame), element_row, element_column);
}




/*
 * const nw::ut::ResMetaData     data = m_pResAnim->GetNwResAnim().GetUserData( "FormatType" );
    nw::ut::ResIntArrayMetaData   intData = nw::ut::ResDynamicCast<nw::ut::ResIntArrayMetaData>( data );
    s32                           formatType = intData.GetValues( 0 );
 */

s32 ResAnim::GetUserDataCount() const
{
  return m_nw_res_anim.GetUserDataCount();
}

s32 ResAnim::GetUserDataIndex(const char* key) const
{
  // ���[�U�f�[�^�p�̒萔 @note ������ׂ��ꏊ�ړ�����Ɨǂ�
  enum
  {
    USER_DATA_INDEX_NOT_FOUND    = -1  // �w�肵�����O�������[�U�f�[�^�����݂��Ȃ��ꍇ�̃��[�U�f�[�^�̃C���f�b�N�X
  };
  
  s32 index = m_nw_res_anim.GetUserDataIndex(key);
  if( index >= 0 ) return index;                      // ���t����
  else             return USER_DATA_INDEX_NOT_FOUND;  // ���t����Ȃ�����
}

void ResAnim::GetUserData(const s32 user_data_index, UserData* user_data) const
{
  nw::ut::ResMetaData res_meta_data = m_nw_res_anim.GetUserData(user_data_index);
  user_data->SetNwResMetaData(res_meta_data);
}
  
//-----------------------------------------------------------------------------
/*
 *  @brief         ResCameraAnim��ViewUpdaterKind���擾����
 *
 *  @retval        ViewUpdaterKind
 *                 ResAnim��ResCameraAnim�łȂ��ꍇ�́AVIEW_UPDATER_KIND_NONE��Ԃ�
 */
//-----------------------------------------------------------------------------
ResAnim::ViewUpdaterKind ResAnim::GetResCameraAnimViewUpdaterKind(void) const
{
  ViewUpdaterKind kind = VIEW_UPDATER_KIND_NONE;
  if(m_type==TYPE_CAMERA)
  {
    //nw::anim::res::ResCameraAnim nw_res_camera_anim = nw::ut::ResDynamicCast<nw::anim::res::ResCameraAnim>(m_nw_res_anim);  nw::anim::res��nw��dynamic_cast���g���Ȃ��B
    
    nw::anim::res::ResCameraAnim nw_res_camera_anim(NULL);
    if( m_nw_res_anim.IsValid() )
    {
      nw_res_camera_anim = nw::anim::res::ResCameraAnim(m_nw_res_anim.ptr());
      s32 nw_kind = nw_res_camera_anim.GetViewUpdaterKind();
      kind = static_cast<ViewUpdaterKind>(nw_kind);
    }
  }
  return kind;
}

//-----------------------------------------------------------------------------
/*
 *    �����o�[�A�j���[�V�����̏��
 */
//-----------------------------------------------------------------------------
s32 ResAnim::GetMemberAnimTypeSize(const s32 member_anim_set_index) const
{
  // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
  // ResMemberAnim::GetPrimitiveSize
  // ResMemberAnim::EvaluateResultForType
  // ��
  // nw::anim::res::ResMemberAnim::PrimitiveType
  // �ɂ���Ă�NW_ASSERT�ɂЂ��������Ă��܂��̂�
  // �Ђ�������PrimitiveType�ł͎g��Ȃ��悤�ɂ��Ă����B

  nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
  s32 primitive_size = res_member_anim.GetPrimitiveSize();
  return primitive_size;
}

b32 ResAnim::GetTransformValue(nw::gfx::CalculatedTransform* value, const s32 member_anim_set_index, const f32 frame, b32 value_valid_array[VALUE_VALID_MAX]) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_TRANSFORM )
  {
    GFL_ASSERT_MSG(GetMemberAnimTypeSize(member_anim_set_index)==sizeof(nw::gfx::CalculatedTransform), "NW4C���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �o�b�t�@�����邩������Ȃ������s�ł���B

    nw::gfx::CalculatedTransform* dstBufTransform = value;
    // ���̌^�̂�OriginalValue���K�v
    nw::math::Transform3          originalValueTransform(gfl::math::VEC3(1.0f,1.0f,1.0f), gfl::math::VEC3(0.0f,0.0f,0.0f), gfl::math::VEC3(0.0f,0.0f,0.0f));
 
    void* dstBuf        = dstBufTransform;
    bit32 dstFlags      = 0;
    void* originalValue = &originalValueTransform;

    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    {
      for(s32 i=0; i<VALUE_VALID_MAX; ++i)
      {
        value_valid_array[i] = true;
      }
      const u32 flags = res_member_anim.GetFlags();
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_SCALE_X_NOT_EXIST )     value_valid_array[VALUE_VALID_SCALE_X] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_SCALE_Y_NOT_EXIST )     value_valid_array[VALUE_VALID_SCALE_Y] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_SCALE_Z_NOT_EXIST )     value_valid_array[VALUE_VALID_SCALE_Z] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_ROTATE_X_NOT_EXIST )    value_valid_array[VALUE_VALID_ROTATE_X] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_ROTATE_Y_NOT_EXIST )    value_valid_array[VALUE_VALID_ROTATE_Y] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_ROTATE_Z_NOT_EXIST )    value_valid_array[VALUE_VALID_ROTATE_Z] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_TRANSLATE_X_NOT_EXIST ) value_valid_array[VALUE_VALID_TRANSLATE_X] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_TRANSLATE_Y_NOT_EXIST ) value_valid_array[VALUE_VALID_TRANSLATE_Y] = false;
      if( flags & nw::anim::res::ResTransformAnimData::FLAG_TRANSLATE_Z_NOT_EXIST ) value_valid_array[VALUE_VALID_TRANSLATE_Z] = false;
    }

    dstFlags = res_member_anim.EvaluateResultForType(
        dstBuf,
        dstFlags,
        frame,
        originalValue
    );

#if 0
    gfl::math::VEC3          translate;               dstBufTransform->GetTranslate(&translate);
    const gfl::math::VEC3&   scale                    = dstBufTransform->Scale();
    //nw::math::VEC3&          direct_scale             = dstBufTransform->DirectScale();  // gfl::math::VEC3&����
    //    // a reference of type "gfl::math::VEC3 &" (not const-qualified) cannot be initialized with a value of type "nn::math::VEC3"
    //    // �Ƃ����R���p�C���G���[�ɂȂ��Ă��܂��B
    const gfl::math::MTX34&  transform_matrix         = dstBufTransform->TransformMatrix();
    //gfl::math::MTX34&        direct_transform_matrix  = dstBufTransform->DirectTransformMatrix();
#endif

    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    for(s32 i=0; i<VALUE_VALID_MAX; ++i)
    {
      value_valid_array[i] = false;
    }
    return false;
  }
}

void ResAnim::DoesExistBakedTransform(b32* scale_exist, b32* rotate_exist, b32* translate_exist, const s32 member_anim_set_index) const
{
  MemberAnimType member_anim_type = this->GetMemberAnimType(member_anim_set_index);
  if( member_anim_type == MEMBER_ANIM_TYPE_BAKED_TRANSFORM )
  {
    nw::anim::res::ResMemberAnim res_member_anim = m_nw_res_anim.GetMemberAnimSet(member_anim_set_index);
    
    // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
    // nw::ut::ResDynamicCast
    // ���Q�l�ɂ����B
    nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  

    if( res_baked_transform_anim.GetScale() )  *scale_exist = true;
    else                                       *scale_exist = false;
    
    if( res_baked_transform_anim.GetRotate() )  *rotate_exist = true;
    else                                        *rotate_exist = false;
    
    if( res_baked_transform_anim.GetTranslate() )  *translate_exist = true;
    else                                           *translate_exist = false;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "�f�[�^�̌^���%d���Ԉ���Ă��܂��B\n", member_anim_type );
    *scale_exist = false;
    *rotate_exist = false;
    *translate_exist = false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���̃N���X�ŕێ�����悤nw::anim::ResAnim��ݒ�
 *
 *  @param[in]     type           �A�j���[�V�������\�[�X�̃^�C�v
 *  @param[in]     nw_res_anim    �A�j���[�V�������\�[�X
 */
//-----------------------------------------------------------------------------
void ResAnim::SetNwResAnim(Resource::AnimationResourceType type, nw::anim::ResAnim nw_res_anim)
{
  switch(type)
  {
  case Resource::ANIMATION_RESOURCE_TYPE_NONE:
    m_type = TYPE_NONE;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_SKELETAL:
    m_type = TYPE_SKELETAL;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_MATERIAL:
    m_type = TYPE_MATERIAL;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_VISIBILITY:
    m_type = TYPE_VISIBILITY;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_CAMERA:
    m_type = TYPE_CAMERA;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_LIGHT:
    m_type = TYPE_LIGHT;
    break;
  case Resource::ANIMATION_RESOURCE_TYPE_ALL:
  default:
    GFL_ASSERT_MSG(0, "�A�j���[�V�������\�[�X�̃^�C�v%d���s���ł��B\n", type);
    m_type = TYPE_NONE;
    break;
  }
  m_nw_res_anim = nw_res_anim;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

