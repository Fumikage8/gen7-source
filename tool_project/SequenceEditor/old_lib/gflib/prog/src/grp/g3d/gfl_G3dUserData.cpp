//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUserData.cpp
 *	@brief  3Dユーザデータ
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
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dユーザデータクラス
//=====================================

//-------------------------------------
// 共通
//-------------------------------------
UserData::UserData(void)
  : m_data_type(DATA_TYPE_INVALID),
    m_res_meta_data()
{
  // 値が被っていないかのチェック
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
  // 何もしない
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
      GFL_ASSERT_MSG(0, "ユーザデータのデータの型情報が不明です。\n");
      count = 0;
    }
    break;
  case DATA_TYPE_INVALID:
    {
      GFL_ASSERT_MSG(0, "ユーザデータが無効です。\n");
      count = 0;
    }
    break;
  }
  return count;
}

//-------------------------------------
// 整数
//-------------------------------------
// 正常に取得できなかった場合、戻り値は不定 
s32  UserData::GetIntValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_INT )
  {
    return  this->CastInt().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return 0;
  }
}
// 正常に設定できたときtrueを返す
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
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return false;
  }
}

//-------------------------------------
// 実数
//-------------------------------------
// 正常に取得できなかった場合、戻り値は不定 
f32  UserData::GetFloatValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_FLOAT )
  {
    return  this->CastFloat().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return 0;
  }
}
// 正常に設定できたときtrueを返す
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
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return false;
  }
}

//-------------------------------------
// 文字列
//-------------------------------------
// 正常に取得できなかった場合、ENCODING_UNKNOWNを返す
UserData::Encoding        UserData::GetEncoding(void) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  static_cast<Encoding>(this->CastString().GetEncoding());
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return  ENCODING_UNKNOWN;
  }
}
// 正常に取得できなかった場合、NULLを返す
const char*     UserData::GetStringValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  this->CastString().GetValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return NULL;
  }
}
// 正常に取得できなかった場合、NULLを返す
const wchar_t*  UserData::GetWStringValue(const s32 index) const
{
  DataType type = this->GetDataType();
  if( type == DATA_TYPE_STRING )
  {
    return  this->CastString().GetWValues(index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "ユーザデータのデータの型情報%dが違います。\n", type );
    return NULL;
  }
}

//-------------------------------------
// 共通
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
// 共通
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

