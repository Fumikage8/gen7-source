//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dMultiPackAnimConnector.cpp
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
#include <grp/g3d/gfl_G3dMultiPackAnimConnector.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	1��Model�ƕ�����PackAnim�����ѕt����N���X
//=====================================
ModelMultiPackAnimConnector::ModelMultiPackAnimConnector(void)
  : m_pack_anim_list_head(NULL),

    m_anim_no(ANIM_NO_NONE),

    m_model(NULL),
    m_step_frame(0.0f),
    m_loop(false)
{
  // �������Ȃ�
}

ModelMultiPackAnimConnector::~ModelMultiPackAnimConnector()
{
  UnsetAllPackAnim();

  // �R���X�g���N�^�Ɠ���������
  {
    m_pack_anim_list_head = NULL;

    m_anim_no = ANIM_NO_NONE;

    m_model = NULL;
    m_step_frame = 0.0f;
    m_loop = false;
  }
}

void ModelMultiPackAnimConnector::SetModel(Model* model)
{
  UnsetAllPackAnim();

  m_model = model;
}

void ModelMultiPackAnimConnector::SetPackAnim(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    s32 slot_index,
    PackAnim* pack_anim
)
{
  UnsetPackAnim(slot_index);

  if( m_model )
  {
    PackAnimElem* curr_elem = NULL;
    
    //////////////////////////////////////////
    // PackAnimConnector���쐬����
    //////////////////////////////////////////
    {
      curr_elem = GFL_NEW(heap_allocator->GetHeapBase()) PackAnimElem;
      curr_elem->pack_anim_connector = NULL;
      curr_elem->prev = NULL;
      curr_elem->next = NULL;

      ModelPackAnimConnector* pac = GFL_NEW(heap_allocator->GetHeapBase()) ModelPackAnimConnector;
      { 
        pac->SetModel(m_model);
        pac->SetPackAnim(
            heap_allocator,
            device_allocator,
            slot_index,
            pack_anim
        );
        pac->ChangeAnimNo(m_anim_no);
        pac->SetStepFrame(m_step_frame);
        pac->SetLoop(m_loop);
      }

      curr_elem->pack_anim_connector = pac;
    }
 
    //////////////////////////////////////////
    // ���ɑ��̃X���b�g��PackAnim�����݂���ꍇ�A�t���[���ԍ��������ō쐬����PackAnimConnector�ɂ��K�p����
    //////////////////////////////////////////
    if( m_pack_anim_list_head )
    {
      f32 max_frame = 0.0f;  // �A�j���[�V�������Ȃ����̂�0.0f��Ԃ��Ă���̂ŁA�S�X���b�g���`�F�b�N���čő�̃t���[�����̗p���邱�Ƃɂ���B
      PackAnimElem* elem = m_pack_anim_list_head;
      while(elem)
      {
        f32 frame = elem->pack_anim_connector->GetFrame();
        if( max_frame < frame ) max_frame = frame; 
        elem = elem->next;
      }
      curr_elem->pack_anim_connector->SetFrame(max_frame);
    }
 
    //////////////////////////////////////////
    // ���X�g�ɒǉ�����
    //////////////////////////////////////////
    {
      // �����𓾂�
      PackAnimElem* tail = NULL;
      PackAnimElem* elem = m_pack_anim_list_head;
      while(elem)
      {
        tail = elem;
        elem = elem->next;
      }
 
      if( tail )
      {
        // �����ɒǉ�
        tail->next = curr_elem;
        curr_elem->prev = tail;
      }
      else
      {
        // ���X�g�ɏ��ǉ�
        m_pack_anim_list_head = curr_elem;
      }
    }
  }
  else
  {
    GFL_ASSERT(m_model);  //@check
  }
}

void ModelMultiPackAnimConnector::UnsetPackAnim(s32 slot_index)
{
  PackAnimElem* elem = getPackAnimElem(slot_index);
  if( elem )
  {
    GFL_DELETE elem->pack_anim_connector;

    if( elem->prev ) elem->prev->next = elem->next;
    else m_pack_anim_list_head = elem->next;  // �擪�������̂ŁA�擪�ύX

    if( elem->next ) elem->next->prev = elem->prev;

    GFL_DELETE elem;
  }

  // �S�Ă�PackAnim�̐ݒ��j�������Ƃ�
  if( !m_pack_anim_list_head ) m_anim_no = ANIM_NO_NONE;
}

void ModelMultiPackAnimConnector::UnsetAllPackAnim(void)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    s32 curr_slot_index = elem->pack_anim_connector->GetSlotIndex();
    elem = elem->next;  // �j���O�Ɏ���PackAnimElem�̃|�C���^�𓾂Ă���
    UnsetPackAnim(curr_slot_index);
  }
}

void ModelMultiPackAnimConnector::ChangeAnimNo(s32 anim_no)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->ChangeAnimNo(anim_no);
    elem = elem->next;
  }
}

void ModelMultiPackAnimConnector::SetStepFrame(f32 step_frame)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->SetStepFrame(step_frame);
    elem = elem->next;
  }
}

void ModelMultiPackAnimConnector::SetLoop(b32 loop)
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    elem->pack_anim_connector->SetLoop(loop);
    elem = elem->next;
  }
}

ModelMultiPackAnimConnector::PackAnimElem* ModelMultiPackAnimConnector::getPackAnimElem(s32 slot_index) const
{
  PackAnimElem* elem = m_pack_anim_list_head;
  while(elem)
  {
    s32 curr_slot_index = elem->pack_anim_connector->GetSlotIndex();
    if( slot_index == curr_slot_index ) return elem;
    elem = elem->next;
  }
  return elem;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

