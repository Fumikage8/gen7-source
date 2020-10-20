//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResVisibilityAnim.cpp
 *	@brief  H3D�A�j���[�V�����R���e���g
 *	@author	Koji Kawada
 *	@date		2012.04.09
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
#include <grp/g3d/gfl_G3dH3dResVisibilityAnim.h>



namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D�r�W�r���e�B�A�j���[�V�����R���e���g�N���X
//=====================================

H3dResVisibilityAnim::H3dResVisibilityAnim(void)
  : H3dResAnim()
{
  Destroy();
}

H3dResVisibilityAnim::~H3dResVisibilityAnim()
{}

b32 H3dResVisibilityAnim::createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name)
{
  s32 index = resource->GetVisibilityAnimContentIndex(name);
  return createInternal(heap_allocator, device_allocator, resource, index);
}

b32 H3dResVisibilityAnim::createInternal(gfl::heap::NwAllocator* /*heap_allocator*/, gfl::heap::NwAllocator* /*device_allocator*/, const H3dResource* resource, s32 index_in_resource)
{
  // resource���o���Ă���
  m_cont_index = index_in_resource;
  m_cont = resource->GetNwVisibilityAnimContent(index_in_resource);
  return true;
}

void H3dResVisibilityAnim::destroyInternal(void)
{
  // �R���X�g���N�^�Ɠ���������
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

