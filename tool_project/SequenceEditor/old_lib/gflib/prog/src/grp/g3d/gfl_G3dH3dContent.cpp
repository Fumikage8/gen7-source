//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dContent.cpp
 *	@brief  H3Dコンテントの基底クラス
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
#include <grp/g3d/gfl_G3dH3dContent.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dコンテントクラス
//=====================================
H3dContent::H3dContent(void)
  : m_res(NULL),
    m_cont_index(H3dResource::INDEX_NONE)
{}

H3dContent::~H3dContent()
{
  Destroy();
}

b32 H3dContent::Create(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  {
    // resourceを覚えておく
    m_res = resource;
  }

  // 派生クラス
  return createInternal(heap_allocator, device_allocator, resource, name);
}
b32 H3dContent::Create(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, s32 index_in_resource)
{
  {
    // resourceを覚えておく
    m_res = resource;
  }

  // 派生クラス
  return createInternal(heap_allocator, device_allocator, resource, index_in_resource);
}

void H3dContent::Destroy(void)
{
  // 派生クラス
  destroyInternal();
  
  {
    // コンストラクタと同じ初期化
    m_res = NULL;
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

