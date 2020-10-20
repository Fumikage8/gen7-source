//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnimConnector.cpp
 *	@brief  3D�A�j���[�V�����t�@�C����1�ɂ܂Ƃ߂�PackAnim�t�@�C��������
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

#include <grp/g3d/gfl_G3dPackAnimConnector.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	1��Model��1��PackAnim�����ѕt����N���X
//=====================================

ModelPackAnimConnector::ModelPackAnimConnector(void)
  : m_pack_anim(NULL),
    m_slot_index(SLOT_INDEX_NONE),
    m_anim_no(ANIM_NO_NONE),
    m_is_set(false),
    m_res_anim_change_animation_temp(NULL),
    
    m_model(NULL),
    m_step_frame(1.0f),
    m_loop(false)
{
  // �������Ȃ�
}

ModelPackAnimConnector::~ModelPackAnimConnector()
{
  UnsetPackAnim();

  // �R���X�g���N�^�Ɠ���������
  {
    m_pack_anim = NULL;
    m_slot_index = SLOT_INDEX_NONE;
    m_anim_no = ANIM_NO_NONE;
    m_is_set = false;
    m_res_anim_change_animation_temp = NULL;
    
    m_model = NULL;
    m_step_frame = 1.0f;
    m_loop = false;
  }
}

void ModelPackAnimConnector::SetModel(Model* model)
{
  UnsetPackAnim();

  m_model = model;
}

void ModelPackAnimConnector::SetPackAnim(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    s32 slot_index,
    PackAnim* pack_anim
)
{
  UnsetPackAnim();

  if( m_model )
  {
    m_pack_anim = pack_anim;
    m_slot_index = slot_index;

    s32 num = m_pack_anim->GetAnimNum();
    ResAnim* res_anim = NULL;
    for(s32 i=0; i<num; ++i)
    {
      res_anim = m_pack_anim->GetResAnim(i);
      if( res_anim ) break;
    }

    if( res_anim )
    {
      // m_model->ChangeAnimation��m_slot_index�ɗ̈���m�ۂ���
      m_model->ChangeAnimation(
        device_allocator,
        res_anim,
        0,
        m_slot_index,
        m_pack_anim->GetMaxAnimMembers()
      );
      m_is_set = true;

      // �̈�͊m�ۂ����܂܂ŃA�j���[�V�������͂����Ă���
#if 0
      m_model->ChangeAnimation(
        NULL,
        NULL,
        0,
        m_slot_index
      );
#endif
      // @note ChangeAnimation�������Ǘ��s���Ή�  // ChangeAnimation��NULL��n������ɉ��L�A�j����0�t���[���Ŏ~�߂Ă������Ƃɂ���B
      m_res_anim_change_animation_temp = res_anim;
      m_model->ChangeAnimation(
        NULL,
        m_res_anim_change_animation_temp,
        0,
        m_slot_index
      );
      m_model->SetAnimationFrame(0.0f, m_slot_index);
      m_model->SetAnimationStepFrame(0.0f, m_slot_index);
    }
  }
  else
  {
    GFL_ASSERT(m_model);  //@check
  }
}

void ModelPackAnimConnector::ChangeAnimNo(s32 anim_no)
{
  m_anim_no = anim_no;

  if( m_is_set )
  {
    ResAnim* res_anim = m_pack_anim->GetResAnim(m_anim_no);
    if( res_anim )
    {
      m_model->ChangeAnimation(
        NULL,
        res_anim,
        0,
        m_slot_index
      );
      m_model->SetAnimationFrame(0.0f, m_slot_index);
      m_model->SetAnimationStepFrame(m_step_frame, m_slot_index);
      m_model->SetAnimationLoop(m_loop, m_slot_index);
    }
    else
    {
#if 0
      m_model->ChangeAnimation(
        NULL,
        NULL,
        0,
        m_slot_index
      );
#endif
      // @note ChangeAnimation�������Ǘ��s���Ή�  // ChangeAnimation��NULL��n������ɉ��L�A�j����0�t���[���Ŏ~�߂Ă������Ƃɂ���B
      m_model->ChangeAnimation(
        NULL,
        m_res_anim_change_animation_temp,
        0,
        m_slot_index
      );
      m_model->SetAnimationFrame(0.0f, m_slot_index);
      m_model->SetAnimationStepFrame(0.0f, m_slot_index);
    }
  }
}

void ModelPackAnimConnector::SetFrame(f32 frame)
{
  if( m_is_set )
  {
    ResAnim* res_anim = m_pack_anim->GetResAnim(m_anim_no);
    if( res_anim )
    {
      m_model->SetAnimationFrame(frame, m_slot_index);
    }
  }
}

f32 ModelPackAnimConnector::GetFrame(void) const
{
  if( m_is_set )
  {
    ResAnim* res_anim = m_pack_anim->GetResAnim(m_anim_no);
    if( res_anim )
    {
      return m_model->GetAnimationFrame(m_slot_index);
    }
  }
  return 0.0f;
}

void ModelPackAnimConnector::SetStepFrame(f32 step_frame)
{
  m_step_frame = step_frame;
  
  if( m_is_set )
  {
    ResAnim* res_anim = m_pack_anim->GetResAnim(m_anim_no);
    if( res_anim )
    {
      // ���A�j���[�V������ݒ蒆�̂Ƃ��́A�ύX���Ă����B
      m_model->SetAnimationStepFrame(m_step_frame, m_slot_index);
    }
  }
}

f32 ModelPackAnimConnector::GetStepFrame(void) const
{
  return m_step_frame;
}

void ModelPackAnimConnector::SetLoop(b32 loop)
{
  m_loop = loop;

  if( m_is_set )
  {
    ResAnim* res_anim = m_pack_anim->GetResAnim(m_anim_no);
    if( res_anim )
    {
      // ���A�j���[�V������ݒ蒆�̂Ƃ��́A�ύX���Ă����B
      m_model->SetAnimationLoop(m_loop, m_slot_index);
    }
  }
}

b32 ModelPackAnimConnector::IsLoop(void) const
{
  return m_loop;
}

void ModelPackAnimConnector::UnsetPackAnim(void)
{
  if( m_model )
  {
    if( m_is_set )
    {
      // @note ���f������A�j���[�V���������S�ɂ͂����̂͂��̕��@��OK���H
      m_model->ChangeAnimation(
          NULL,
          NULL,
          0,
          m_slot_index
      );
    }

    m_pack_anim = NULL;
    m_slot_index = SLOT_INDEX_NONE;
    m_anim_no = ANIM_NO_NONE;
    m_is_set = false;
    m_res_anim_change_animation_temp = NULL;
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

