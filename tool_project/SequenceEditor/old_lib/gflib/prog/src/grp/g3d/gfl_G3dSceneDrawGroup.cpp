//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneDrawGroup.cpp
 *	@brief  3D�V�[���`��O���[�v
 *	@author	Koji Kawada
 *	@date		2011.02.14
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSceneDrawGroup.h>


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�O���֐���`
 */
//-----------------------------------------------------------------------------
void SceneDrawGroupCommandInvokeCallbackDefault(nw::gfx::RenderContext* render_context, void* work)
{
  GFL_UNUSED(render_context);
  GFL_UNUSED(work);
}


//-----------------------------------------------------------------------------
/**
 *					�N���X��`
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�V�[���`��O���[�v�N���X
//=====================================
SceneDrawGroup::SceneDrawGroup(gfl::heap::NwAllocator* allocator, s32 render_queue_index, const Description* description)
  : m_state(STATE_NONE),
    m_render_queue(NULL),
    m_render_queue_index(render_queue_index),
    m_active_camera_index(CAMERA_INDEX_NONE),
    m_begin_command_invoke_callback(SceneDrawGroupCommandInvokeCallbackDefault),
    m_begin_command_invoke_work(NULL),
    m_end_command_invoke_callback(SceneDrawGroupCommandInvokeCallbackDefault),
    m_end_command_invoke_work(NULL)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_render_queue = nw::gfx::RenderQueue::Builder()            // ����RenderQueue�ɓƎ��̃����o�ϐ��������������Ȃ�A
      .MaxRenderElements(l_description.max_render_elements)   // typedef BasicRenderQueue<> RenderQueue���p������RenderQueue��
      .IsFixedSizeMemory(l_description.is_fixed_size_memory)  // ����΂������ȁB
      .Create(allocator);
}

SceneDrawGroup::~SceneDrawGroup()
{
  nw::gfx::SafeDestroy(m_render_queue);
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl
