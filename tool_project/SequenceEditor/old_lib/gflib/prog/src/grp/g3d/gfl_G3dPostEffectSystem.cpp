//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPostEffectSystem.cpp
 *	@brief  ポスト処理システム
 *	@author	Masateru Ishiguro
 *	@date		2011.04.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dPostEffectSystem.h>


namespace gfl {
namespace grp {
namespace g3d {
  
//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DPostEffectSystem::Description::Description() :
  edge_draw_group_id(0),
  edge_enable(false),
  edge_type( G3DEdgeFilter::DEFAULT_EDGE ),
  bloom_filter_enable(false),
  bloom_filter_description(NULL)
{ 

}


//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DPostEffectSystem::G3DPostEffectSystem(void) :
  m_pG3DEdgeFilter(NULL),
  m_EdgeEnable(false),
  m_bloom_filter(NULL),
  m_BloomEnable(false)
{

}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
G3DPostEffectSystem::~G3DPostEffectSystem()
{
  DestroyWork();
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DPostEffectSystem::CreateWork(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, G3DPostEffectSystem::Description* p_desc,
                                     const gfl::grp::GraphicsSystem* graphics_system,
                                     const gfl::grp::MemoryArea      frame_buffer_memory_area)
{
  m_EdgeEnable = p_desc->edge_enable;
  
  if( m_EdgeEnable )
  {
    m_pG3DEdgeFilter = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::G3DEdgeFilter();
    m_pG3DEdgeFilter->CreateWork( heap_allocator, device_allocator );
    this->m_EdgeDrawGroupID = p_desc->edge_draw_group_id;
  }

  // ブルームフィルタ
  m_BloomEnable = p_desc->bloom_filter_enable;
  if( m_BloomEnable )
  {
    m_bloom_filter = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::BloomFilter;
    m_bloom_filter->Create(
        heap_allocator,
        device_allocator,
        graphics_system,
        frame_buffer_memory_area,
        p_desc->bloom_filter_description
    );
  }
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void G3DPostEffectSystem::DestroyWork(void)
{
  GFL_SAFE_DELETE( m_pG3DEdgeFilter );
  
  // ブルームフィルタ
  if( m_bloom_filter )
  {
    GFL_DELETE m_bloom_filter;
  }
  m_bloom_filter = NULL;
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

