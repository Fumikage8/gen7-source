#ifndef __GFL_G3DSCENEDRAWGROUP_H__
#define __GFL_G3DSCENEDRAWGROUP_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneDrawGroup.h
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


namespace gfl {
namespace grp {
namespace g3d {


//-----------------------------------------------------------------------------
/**
 *					�^�錾
 */
//-----------------------------------------------------------------------------
typedef void (*SceneDrawGroupCommandInvokeCallback)(nw::gfx::RenderContext* render_context, void* work);


//-----------------------------------------------------------------------------
/**
 *					�O���֐��錾
 */
//-----------------------------------------------------------------------------
extern void SceneDrawGroupCommandInvokeCallbackDefault(nw::gfx::RenderContext* render_context, void* work);


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�V�[���`��O���[�v�N���X
//=====================================
class SceneDrawGroup
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneDrawGroup);


  // ���L�N���X���炵���A�N�Z�X���Ȃ�
  friend class Scene;
  friend class RenderSystem;
  friend class ShadowSystem;
  friend struct RenderSceneInternalFunctor;


public:
  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�ݒ���e
  //=====================================
  struct Description
  {
    s32  max_render_elements;     // �L���[�ɐςޕ`��v�f�̏����
    bool is_fixed_size_memory;    // �������ȊO�ɂ����������m�ۂ��邩�ǂ����̃t���O  //@���g�p�폜�\��
    
    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : max_render_elements   (256*6),
        is_fixed_size_memory  (true)
    {}
  };


private:
  SceneDrawGroup(gfl::heap::NwAllocator* allocator, s32 render_queue_index, const Description* description = NULL);
  ~SceneDrawGroup();

private:
  enum State
  {
    STATE_NONE,                 // �O���[�v��������
    STATE_AFTER_ADD_OR_REMOVE,  // �V�[���m�[�h��ǉ��������͍폜��������
    STATE_AFTER_SUBMIT_VIEW,    // SubmitView����
    STATE_AFTER_DRAW            // Draw����
  };

private:
  void SetState(const State state) { m_state = state; }
  State GetState(void) { return m_state; }

  void SetBeginCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL)
  {
    if( callback )
    {
      m_begin_command_invoke_callback = callback;
    }
    else
    {
      m_begin_command_invoke_callback = SceneDrawGroupCommandInvokeCallbackDefault;
    }
    m_begin_command_invoke_work = work;
  }
  void SetEndCommandInvokeCallback(SceneDrawGroupCommandInvokeCallback callback = NULL, void* work = NULL)
  {
    if( callback )
    {
      m_end_command_invoke_callback = callback;
    }
    else
    {
      m_end_command_invoke_callback = SceneDrawGroupCommandInvokeCallbackDefault;
    }
    m_end_command_invoke_work = work;
  }

  void BeginCommandInvokeCallback(nw::gfx::RenderContext* render_context)
  {
    m_begin_command_invoke_callback(render_context, m_begin_command_invoke_work);
  }
  void EndCommandInvokeCallback(nw::gfx::RenderContext* render_context)
  {
    m_end_command_invoke_callback(render_context, m_end_command_invoke_work);
  }

  nw::gfx::RenderQueue*  GetRenderQueue(void)              const  { return m_render_queue; }
  s32                    GetRenderQueueIndex(void)         const  { return m_render_queue_index; }
  void                   SetActiveCamera(s32 camera_index)        { m_active_camera_index = camera_index; }
  s32                    GetActiveCameraIndex(void)        const  { return m_active_camera_index; }

private:
  State                                m_state;

  nw::gfx::RenderQueue*                m_render_queue;
  s32                                  m_render_queue_index;
  s32                                  m_active_camera_index;

  SceneDrawGroupCommandInvokeCallback  m_begin_command_invoke_callback;
  void*                                m_begin_command_invoke_work; 
  SceneDrawGroupCommandInvokeCallback  m_end_command_invoke_callback;
  void*                                m_end_command_invoke_work; 
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENEDRAWGROUP_H__
