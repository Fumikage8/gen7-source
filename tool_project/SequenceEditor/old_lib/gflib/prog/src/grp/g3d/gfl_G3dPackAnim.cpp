//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnim.cpp
 *	@brief  3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱う
 *	@author	Koji Kawada
 *	@date		2012.08.15
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
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

#include <grp/g3d/gfl_G3dPackAnim.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱うクラス
//=====================================

PackAnim::PackAnim(void)
  : m_buffer(NULL),
    m_resource_array(NULL),
    m_res_anim_array(NULL),
    m_type(ANIMATION_TYPE_NONE),
    m_anim_num(0)
{
  // 何もしない
}

PackAnim::~PackAnim()
{
  Destroy();
}

void PackAnim::Create(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  AnimationType           type,
  void*                   buffer
)
{
  m_buffer = buffer;
  m_type = type;

  gfl::base::BinLinkerAccessor bin_linker_accessor(m_buffer);
  m_anim_num = bin_linker_accessor.GetDataMax();

  if( m_anim_num > 0 )
  {
    m_resource_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Resource*[m_anim_num];
    m_res_anim_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ResAnim*[m_anim_num];
    for(s32 i=0; i<m_anim_num; ++i)
    {
      m_resource_array[i] = NULL;
      m_res_anim_array[i] = NULL;
      if( bin_linker_accessor.GetDataSize(i) > 0 )
      {
        m_resource_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) Resource;
        m_resource_array[i]->AttachBuffer(device_allocator, bin_linker_accessor.GetData(i));
        m_resource_array[i]->Setup(device_allocator);
        m_res_anim_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) ResAnim;
        m_resource_array[i]->GetResAnim(static_cast<gfl::grp::g3d::Resource::AnimationResourceType>(m_type), 0, m_res_anim_array[i]);
      }
    }
  }
}

void PackAnim::Destroy(void)
{
  if( m_res_anim_array )
  {
    for(s32 i=0; i<m_anim_num; ++i)
    {
      if( m_res_anim_array[i] )
      {
        GFL_DELETE m_res_anim_array[i];
        m_res_anim_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_res_anim_array;
    m_res_anim_array = NULL;
  }

  if( m_resource_array )
  {
    for(s32 i=0; i<m_anim_num; ++i)
    {
      if( m_resource_array[i] )
      {
        GFL_DELETE m_resource_array[i];
        m_resource_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_resource_array;
    m_resource_array = NULL;
  }

  m_anim_num = 0;
  m_type = ANIMATION_TYPE_NONE;
  m_buffer = NULL;
}

s32 PackAnim::GetMaxAnimMembers(void) const
{
  s32 max = 0;
  for(s32 i=0; i<m_anim_num; ++i)
  {
    if( m_res_anim_array[i] )
    {
      s32 curr = m_res_anim_array[i]->GetMemberAnimSetCount();
      if( max < curr )
      {
        max = curr;
      }
    }
  }
  return max;
}

s32 PackAnim::GetAnimNum(void) const
{
  return m_anim_num;
}

ResAnim* PackAnim::GetResAnim(s32 anim_no) const
{
  if( m_res_anim_array )
  {
    if( anim_no != ANIM_NO_NONE )
    {
      return m_res_anim_array[anim_no];
    }
  }
  return NULL;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

