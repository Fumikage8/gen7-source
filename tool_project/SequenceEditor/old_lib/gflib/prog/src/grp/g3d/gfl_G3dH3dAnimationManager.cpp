//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dAnimationManager.cpp
 *	@brief  H3Dアニメーションマネージャ
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
#include <grp/g3d/gfl_G3dH3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dH3dAnimationManager.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス実装
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dアニメーションマネージャクラス
//=====================================

H3dAnimationManager::H3dAnimationManager(void)
  : m_slot_num(0),
    m_slot_array(NULL)
{
}
H3dAnimationManager::~H3dAnimationManager()
{
  Destroy();
}

b32 H3dAnimationManager::CreateAnimationManagerAndAnimationSlot(gfl::heap::NwAllocator* heap_allocator, H3dModel* model, s32 slot_num, const H3dAnimationSlot::Description* slot_desc_array)
{
  b32 ret = true;
  ret = CreateAnimationManager(heap_allocator, slot_num);
  if( !ret ) return false;

  for(s32 i=0; i<m_slot_num; ++i)
  {
    ret = CreateAnimationSlot(heap_allocator, model, i, &slot_desc_array[i]);
    if( !ret ) return false;
  }

  return true;
}

b32 H3dAnimationManager::CreateAnimationManager(gfl::heap::NwAllocator* heap_allocator, s32 slot_num)
{
  m_slot_num = slot_num;
  if( m_slot_num > 0 )
  {
    m_slot_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) H3dAnimationSlot*[m_slot_num];
  }
  for(s32 i=0; i<m_slot_num; ++i)
  {
    m_slot_array[i] = NULL;
  }
  return true;
}

b32 H3dAnimationManager::CreateAnimationSlot(gfl::heap::NwAllocator* heap_allocator, H3dModel* model, s32 slot_index, const H3dAnimationSlot::Description* slot_desc)
{
  GFL_ASSERT(m_slot_array[slot_index] == NULL);
  m_slot_array[slot_index] = GFL_NEW(heap_allocator->GetHeapBase()) H3dAnimationSlot;
  return m_slot_array[slot_index]->Create(heap_allocator, model, slot_desc);
}

b32 H3dAnimationManager::IsAnimationSlotCreated(s32 slot_index) const
{
  if( m_slot_array && m_slot_array[slot_index] ) return true;
  return false;
}

void H3dAnimationManager::Destroy(void)
{
  if( m_slot_array )
  {
    for(s32 i=0; i<m_slot_num; ++i)
    {
      DestroyAnimationSlot(i);
    }
    GFL_DELETE_ARRAY m_slot_array;
    m_slot_array = NULL;
  }
  
  // コンストラクタと同じ初期化
  m_slot_num = 0;
  m_slot_array = NULL;
}

void H3dAnimationManager::DestroyAnimationSlot(s32 slot_index)
{
  if( m_slot_array )
  {
    if( m_slot_array[slot_index] )
    {
      GFL_DELETE m_slot_array[slot_index];
      m_slot_array[slot_index] = NULL;
    }
  }
}

void H3dAnimationManager::Calculate(void)
{
#if 0
個別Calculateをつくる前の処理

  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->Calculate();
    }
  }

#else
  CalculateLocalSkeletalAnimation();
  CalculateVisibilityAnimation();
  CalculateMaterialAnimation();
  CalculateAnimationFrame();
  CalculateWorldSkeletalAnimation();
#endif
}

void H3dAnimationManager::CalculateLocalSkeletalAnimation(void)
{
  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->CalculateLocalSkeletalAnimation();
    }
  }
}

void H3dAnimationManager::CalculateVisibilityAnimation(void)
{
  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->CalculateVisibilityAnimation();
    }
  }
}

void H3dAnimationManager::CalculateMaterialAnimation(void)
{
  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->CalculateMaterialAnimation();
    }
  }
}

void H3dAnimationManager::CalculateAnimationFrame(void)
{
  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->CalculateAnimationFrame();
    }
  }
}

void H3dAnimationManager::CalculateWorldSkeletalAnimation(void)
{
  for(s32 i=0; i<m_slot_num; ++i)
  {
    if( m_slot_array[i] )
    {
      m_slot_array[i]->CalculateWorldSkeletalAnimation();
    }
  }
}



H3dAnimationSlot::ChangeAnimationResult H3dAnimationManager::ChangeAnimationByResAnim(s32 slot_index, H3dResAnim* res_anim)
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->ChangeByResAnim(res_anim);
  return H3dAnimationSlot::CHANGE_ANIMATION_RESULT_ERROR;
}
H3dAnimationSlot::ChangeAnimationResult H3dAnimationManager::ChangeAnimationByResAnimListIndex(s32 slot_index, s32 list_index)
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->ChangeByResAnimListIndex(list_index);
  return H3dAnimationSlot::CHANGE_ANIMATION_RESULT_ERROR;
}

H3dResAnim* H3dAnimationManager::GetAnimationAsResAnim(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->GetAnimationAsResAnim();
  return NULL;
}

void H3dAnimationManager::SetAnimationFrame(s32 slot_index, f32 frame)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetFrame(frame);
}
f32 H3dAnimationManager::GetAnimationFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->GetFrame();
  return 0.0f;
}

void H3dAnimationManager::SetAnimationStepFrame(s32 slot_index, f32 step_frame)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetStepFrame(step_frame);
}
f32 H3dAnimationManager::GetAnimationStepFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->GetStepFrame();
  return 0.0f;
}

void H3dAnimationManager::SetAnimationStartFrame(s32 slot_index, f32 start_frame)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetStartFrame(start_frame);
}
f32 H3dAnimationManager::GetAnimationStartFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->GetStartFrame();
  return 0.0f;
}

void H3dAnimationManager::SetAnimationEndFrame(s32 slot_index, f32 end_frame)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetEndFrame(end_frame);
}
f32 H3dAnimationManager::GetAnimationEndFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->GetEndFrame();
  return 0.0f;
}

void H3dAnimationManager::SetAnimationStartAndEndFrame(s32 slot_index, f32 start_frame, f32 end_frame)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetStartAndEndFrame(start_frame, end_frame);
}

b32 H3dAnimationManager::IsAnimationFrameStartFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->IsFrameStartFrame();
  return false;
}
b32 H3dAnimationManager::IsAnimationFrameEndFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->IsFrameEndFrame();
  return false;
}
b32 H3dAnimationManager::IsAnimationFrameStepTerminalFrame(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->IsFrameStepTerminalFrame();
  return false;
}

b32 H3dAnimationManager::IsAnimationLoop(s32 slot_index) const
{
  if( m_slot_array[slot_index] )
    return m_slot_array[slot_index]->IsLoop();
  return false;
}
void H3dAnimationManager::SetAnimationLoop(s32 slot_index, b32 loop)
{
  if( m_slot_array[slot_index] )
    m_slot_array[slot_index]->SetLoop(loop);
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

