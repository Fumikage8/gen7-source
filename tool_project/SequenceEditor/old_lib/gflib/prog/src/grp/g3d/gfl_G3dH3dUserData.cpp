//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dUserData.cpp
 *	@brief  H3Dユーザデータ
 *	@author	Koji Kawada
 *	@date		2012.08.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

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
#include <grp/g3d/gfl_G3dH3dUserData.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dユーザデータクラス
//=====================================

H3dUserData::H3dUserData(void)
  : m_meta_data(NULL)
{
  STATIC_ASSERT( DATA_TYPE_INT                   != DATA_TYPE_INVALID );
  STATIC_ASSERT( DATA_TYPE_FLOAT                 != DATA_TYPE_INVALID );
  STATIC_ASSERT( DATA_TYPE_STRING                != DATA_TYPE_INVALID );
  STATIC_ASSERT( DATA_TYPE_WSTRING               != DATA_TYPE_INVALID );
  STATIC_ASSERT( DATA_TYPE_ORIENTED_BOUNDING_BOX != DATA_TYPE_INVALID );
}

H3dUserData::~H3dUserData()
{}

const char*  H3dUserData::GetName(void) const
{
  if( m_meta_data )
  {
    return m_meta_data->name;
  }
  return NULL;
}
H3dUserData::DataType     H3dUserData::GetDataType(void) const
{
  if( m_meta_data )
  {
    return static_cast<DataType>(m_meta_data->dataType);
  }
  return DATA_TYPE_INVALID;
}
s32          H3dUserData::GetValueCount(void) const
{
  if( m_meta_data )
  {
    return m_meta_data->dataCount;
  }
  return 0;
}

s32  H3dUserData::GetIntValue(const s32 index) const
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_INT )
    {
      return m_meta_data->GetIntArray()[index];
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return 0;
}

//Setは禁止
//kujiraBTS6180、GFBTS2202対処のために許可してもらいました
//H3Dの設計方針に反する点
//UserDataのポインタにconstがついて、変更がそもそも不可になる可能性がある点
//上記のことから、この修正方法で未来永劫OKなわけでないことに留意してください
b32  H3dUserData::SetIntValue(const s32 index, const s32 value)
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_INT )
    {
      m_meta_data->GetIntArray()[index] = value;
      return true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return false;
}

f32  H3dUserData::GetFloatValue(const s32 index) const
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_FLOAT )
    {
      return m_meta_data->GetFloatArray()[index];
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return 0;
}

/*
Setは禁止
b32  H3dUserData::SetFloatValue(const s32 index, const f32 value)
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_FLOAT )
    {
      m_meta_data->GetFloatArray()[index] = value;
      return true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return false;
}
*/

const char*     H3dUserData::GetStringValue(const s32 index) const
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_STRING )
    {
      return m_meta_data->GetStringArray()[index];
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return NULL;
}

const wchar_t*  H3dUserData::GetWStringValue(const s32 index) const
{
  if( m_meta_data )
  {
    if( 0<=index && index<GetValueCount() && GetDataType()==DATA_TYPE_WSTRING )
    {
      return m_meta_data->GetWStringArray()[index];
    }
    else
    {
      GFL_ASSERT_MSG(0, "index=%d\n", index);
    }
  }
  return NULL;
}

/*
const H3dUserData::OrientedBoundingBox*  H3dUserData::GetOBB(void)
{
  if( m_meta_data )
  {
    if( GetDataType()==DATA_TYPE_ORIENTED_BOUNDING_BOX )
    {
      m_obb = m_meta_data->GetOBB();
      return m_obb;
    }
  }
  return NULL;
}
*/


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

