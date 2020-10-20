//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUserData.cpp
 *	@brief  3D���[�U�f�[�^
 *	@author	Koji Kawada
 *	@date		2011.08.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/ut.h>

// gflib
#include <gfl_debug.h>

// gflib grp g3d
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
///	3D���[�U�f�[�^�N���X
//=====================================

//-------------------------------------
// ����
//-------------------------------------
UserData::UserData(void)
  : m_data_type(DATA_TYPE_INVALID),
    m_res_meta_data()
{
  // �l������Ă��Ȃ����̃`�F�b�N
  STATIC_ASSERT(DATA_TYPE_FLOAT   != DATA_TYPE_INVALID);
  STATIC_ASSERT(DATA_TYPE_INT     != DATA_TYPE_INVALID);
  STATIC_ASSERT(DATA_TYPE_STRING  != DATA_TYPE_INVALID);
  STATIC_ASSERT(DATA_TYPE_UNKNOWN != DATA_TYPE_INVALID);

  STATIC_ASSERT(ENCODING_ASCII   != ENCODING_UNKNOWN);
  STATIC_ASSERT(ENCODING_UTF8    != ENCODING_UNKNOWN);
  STATIC_ASSERT(ENCODING_UTF16LE != ENCODING_UNKNOWN);
  STATIC_ASSERT(ENCODING_UTF16BE != ENCODING_UNKNOWN);
}
UserData::~UserData()
{
  // �������Ȃ�
}

gfl::grp::g3d::UserData::DataType  UserData::GetDataType(void) const
{
  return m_data_type;
}

s32       UserData::GetValueCount(void) const
{
  s32 count = 0.0f;
  switch( this->GetDataType() )
  {
  case DATA_TYPE_INT:
    {
       nw::ut::ResIntArrayMetaData res = this->CastInt();
       count = res.GetValuesCount();
    }
    break;
  case DATA_TYPE_FLOAT:
    {
       nw::ut::ResFloatArrayMetaData res = this->CastFloat();
       count = res.GetValuesCount();
    }
    break;
  case DATA_TYPE_STRING:
    {
       nw::ut::ResStringArrayMetaData res = this->CastString();
       count = res.GetValuesCount();
    }
    break;
  case DATA_TYPE_UNKNOWN:
    {
      GFL_ASSERT_MSG(0, "���[�U�f�[�^�̃f�[�^�̌^��񂪕s���ł��B\n");
      count = 0;
    }
    break;
  case DATA_TYPE_INVALID:
    {
      GFL_ASSERT_MSG(0, "���[�U�f�[�^�������ł��B\n");
      count = 0;
    }
    break;
  }
  return count;
}

//-------------------------------------
// ����
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
s32  UserData::GetIntValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_INT )
  {
    return  this->CastInt().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return 0;
  }
}
// ����ɐݒ�ł����Ƃ�true��Ԃ�
b32  UserData::SetIntValue(const s32 index, const s32 value)
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_INT )
  {
    this->CastInt().SetValues(index, value);
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return false;
  }
}

//-------------------------------------
// ����
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�A�߂�l�͕s�� 
f32  UserData::GetFloatValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_FLOAT )
  {
    return  this->CastFloat().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return 0;
  }
}
// ����ɐݒ�ł����Ƃ�true��Ԃ�
b32  UserData::SetFloatValue(const s32 index, const f32 value)
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_FLOAT )
  {
    this->CastFloat().SetValues(index, value);
    return true;
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return false;
  }
}

//-------------------------------------
// ������
//-------------------------------------
// ����Ɏ擾�ł��Ȃ������ꍇ�AENCODING_UNKNOWN��Ԃ�
UserData::Encoding        UserData::GetEncoding(void) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  static_cast<Encoding>(this->CastString().GetEncoding());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return  ENCODING_UNKNOWN;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�ANULL��Ԃ�
const char*     UserData::GetStringValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  this->CastString().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return NULL;
  }
}
// ����Ɏ擾�ł��Ȃ������ꍇ�ANULL��Ԃ�
const wchar_t*  UserData::GetWStringValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  this->CastString().GetWValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���[�U�f�[�^�̃f�[�^�̌^���%d���Ⴂ�܂��B\n", type );
    return NULL;
  }
}

//-------------------------------------
// ����
//-------------------------------------
void  UserData::SetNwResMetaData(nw::ut::ResMetaData res_meta_data)
{
  m_res_meta_data = res_meta_data;
  if( m_res_meta_data.IsValid() )
  {
    m_data_type = static_cast<DataType>(m_res_meta_data.GetDataType());
  }
  else
  {
    m_data_type = DATA_TYPE_INVALID;
  }
}

//-------------------------------------
// ����
//-------------------------------------
nw::ut::ResIntArrayMetaData    UserData::CastInt(void) const
{
  return  nw::ut::ResDynamicCast<nw::ut::ResIntArrayMetaData>(m_res_meta_data);
}
nw::ut::ResFloatArrayMetaData  UserData::CastFloat(void) const
{
  return  nw::ut::ResDynamicCast<nw::ut::ResFloatArrayMetaData>(m_res_meta_data);
}
nw::ut::ResStringArrayMetaData UserData::CastString(void) const
{
  return  nw::ut::ResDynamicCast<nw::ut::ResStringArrayMetaData>(m_res_meta_data);
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

