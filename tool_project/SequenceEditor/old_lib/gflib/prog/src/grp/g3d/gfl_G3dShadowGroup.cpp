//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowGroup.cpp
 *	@brief  3D影グループ
 *	@author	Koji Kawada
 *	@date		2011.03.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
 */
//-----------------------------------------------------------------------------
ShadowGroup::ShadowGroup(void)
  : m_type(TYPE_NONE),
    m_scene_draw_group_index(SCENE_DRAW_GROUP_INDEX_NONE),
    m_camera_index(CAMERA_INDEX_NONE),
    m_camera(NULL),
    m_render_target(NULL)
{
  // 何もしない
}

ShadowGroup::ShadowGroup(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const Description*      description)
  : m_type(TYPE_NONE),
    m_scene_draw_group_index(SCENE_DRAW_GROUP_INDEX_NONE),
    m_camera_index(CAMERA_INDEX_NONE),
    m_camera(NULL),
    m_render_target(NULL)
{
  Create(
      heap_allocator,
      device_allocator,
      description);
}

ShadowGroup::~ShadowGroup()
{
  Destroy();
}

void ShadowGroup::Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const Description*      description)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  switch( l_description.type )
  {
  case TYPE_NONE:
    {
      m_type                   = l_description.type;
      // 何もしない
    }
    break;
  case TYPE_PROJECTION_SHADOW:
    {
      m_type                   = l_description.type;
      m_camera_index           = l_description.camera_index;
      m_scene_draw_group_index = l_description.scene_draw_group_index;
      m_camera = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Camera();
      m_camera->Create(device_allocator, l_description.camera_description);
      m_render_target = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
      m_render_target->Create(heap_allocator, device_allocator, l_description.render_target_description);
    }
    break;
  case TYPE_SHADOW_MAP:
    {
      m_type                   = l_description.type;
      m_camera_index           = l_description.camera_index;
      m_scene_draw_group_index = l_description.scene_draw_group_index;
      m_camera = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Camera();
      m_camera->Create(device_allocator, l_description.camera_description);
      m_render_target = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::RenderTarget();
      m_render_target->Create(heap_allocator, device_allocator, l_description.render_target_description);
    }
    break;
  }
}

void ShadowGroup::Destroy(void)
{
  if( m_render_target )
  {
    GFL_DELETE m_render_target;
    m_render_target = NULL;
  }
  if( m_camera )
  {
    GFL_DELETE m_camera; 
    m_camera = NULL;
  }
  m_camera_index = CAMERA_INDEX_NONE;
  m_scene_draw_group_index = SCENE_DRAW_GROUP_INDEX_NONE;
  m_type = TYPE_NONE;
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

