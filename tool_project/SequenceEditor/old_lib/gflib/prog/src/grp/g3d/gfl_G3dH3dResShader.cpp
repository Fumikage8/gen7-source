//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResShader.cpp
 *	@brief  H3Dシェーダコンテント
 *	@author	Koji Kawada
 *	@date		2012.09.11
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
#include <grp/g3d/gfl_G3dH3dResShader.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dシェーダコンテントクラス
//=====================================
H3dResShader::H3dResShader(void)
  : H3dContent(),
    m_cont(NULL)
{}

H3dResShader::~H3dResShader()
{
  Destroy();
}

b32 H3dResShader::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  s32 index = resource->GetShaderContentIndex(name);
  return createInternal(heap_allocator, device_allocator, resource, index);
}

b32 H3dResShader::createInternal(gfl::heap::NwAllocator* /*heap_allocator*/, gfl::heap::NwAllocator* /*device_allocator*/, const H3dResource* resource, s32 index_in_resource)
{
  // resourceを覚えておく
  m_cont_index = index_in_resource;
  m_cont = resource->GetNwShaderContent(index_in_resource);
  return true;
}

void H3dResShader::destroyInternal(void)
{
  // コンストラクタと同じ初期化
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

