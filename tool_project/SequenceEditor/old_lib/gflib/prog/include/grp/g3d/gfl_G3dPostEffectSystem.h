#ifndef __GFL_G3D_POSTEFFECTSYSTEM_H__
#define __GFL_G3D_POSTEFFECTSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPostEffectSystem.h
 *	@brief  �|�X�g�����V�X�e��
 *	@author	Masateru Ishiguro
 *	@date		2011.04.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dEdgeFilter.h>
#include <grp/g3d/gfl_G3dBloomFilter.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�|�X�g�����N���X
//=====================================
class G3DPostEffectSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DPostEffectSystem);
  friend class Scene;
  
public:  
  struct Description{
    Description();
    
    s32                                       edge_draw_group_id;
    b32                                       edge_enable;
    G3DEdgeFilter::EdgeType                   edge_type;
   
    // �u���[���t�B���^�̐ݒ���e
    b32                                       bloom_filter_enable;
    gfl::grp::g3d::BloomFilter::Description*  bloom_filter_description;
  };
  
  inline void SetEdgeEnable(b32 f);
  inline b32 IsEdgeEnable();

  inline void SetBloomEnable(b32 f);
  inline b32 IsBloomEnable();

private:

  G3DPostEffectSystem(void);

  ~G3DPostEffectSystem();

  /**
   * @brief ���[�N�m��
   * @param heap_allocator      �������m�ې�
   * @param device_allocator    �������m�ې�
   * @param p_desc              �ݒ���e
   * @param graphics_system           �O���t�B�b�N�X�V�X�e��(Description��bloom_filter_enable��true�̂Ƃ��̂ݎg�p)
   * @param frame_buffer_memory_area  �I���X�N���[���̃t���[���o�b�t�@�̃������G���A(Description��bloom_filter_enable��true�̂Ƃ��̂ݎg�p)
   */
  void CreateWork(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, Description* p_desc,
                  const gfl::grp::GraphicsSystem* graphics_system          = NULL,
                  const gfl::grp::MemoryArea      frame_buffer_memory_area = gfl::grp::MEMORY_AREA_NONE);

  /**
   * @brief ���[�N�j��
   */
  virtual void DestroyWork(void);
  
  s32 GetEdgeGroupIndex(){ return m_EdgeDrawGroupID; }

  // �u���[���t�B���^���擾����
  gfl::grp::g3d::BloomFilter*  GetBloomFilter(void) const  { return m_bloom_filter; }


  G3DEdgeFilter                *m_pG3DEdgeFilter;
  s32                          m_EdgeDrawGroupID;
  b32                          m_EdgeEnable;

  // �u���[���t�B���^�̃C���X�^���X
  gfl::grp::g3d::BloomFilter*  m_bloom_filter;
  b32                          m_BloomEnable;
};

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W�L�������ݒ�
 */
//-----------------------------------------------------------------------------
inline void G3DPostEffectSystem::SetEdgeEnable( b32 f )
{
  m_EdgeEnable = f;
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W���L�����H
 * @return �L�������t���O
 */
//-----------------------------------------------------------------------------
inline b32 G3DPostEffectSystem::IsEdgeEnable()
{
  return m_EdgeEnable;
}

//-----------------------------------------------------------------------------
/**
 * @brief �u���[���L�������ݒ�
 */
//-----------------------------------------------------------------------------
inline void G3DPostEffectSystem::SetBloomEnable(b32 f)
{
  m_BloomEnable = f;
}

//-----------------------------------------------------------------------------
/**
 * @brief �u���[�����L�����H
 * @return �L�������t���O
 */
//-----------------------------------------------------------------------------
inline b32 G3DPostEffectSystem::IsBloomEnable()
{
  return m_BloomEnable;
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DEDGESYSTEM_H__
