#ifndef __GFL_G3D_EDGEFILTER_H__
#define __GFL_G3D_EDGEFILTER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3deDGEfILTER.h
 *	@brief  �G�b�W�t�B���^�[
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
#include <grp/g3d/gfl_G3dPostEffectSystem.h>

#include <base/gfl_KeyContainer.h>




namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�G�b�W�t�B���^�[�N���X
//=====================================
class G3DEdgeFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DEdgeFilter);
public:
  
  enum EdgeType
  {
    DEFAULT_EDGE = 0,   //! �f�t�H���g�G�b�W
    VCOLOR_EDGE,        //! ���_�J���[�G�b�W
    ID_EDGE,            //! �h�c�G�b�W
    NORMAL_EDGE,        //! �@���G�b�W
#if GFL_DEBUG
    DEBUG_RGB,          //! �f�o�b�O�p
    DEBUG_A,            //! �f�o�b�O�p
    DEBUG_R,            //! �f�o�b�O�p
#endif
    INSIDE_EDGE,        //! �����G�b�W
    DOUBLE_LAYERED_EDGE,//! �Q�w�G�b�W(�f�t�H���g�G�b�W+�����G�b�W)
    TYPE_MAX
  };
  
  static const u32          MAX_PATH = 3;
  
  #define   TEXTURE_WIDTH   (512.0f)
  #define   TEXTURE_HEIGHT  (256.0f)

  //�I�t�X�N���[���o�b�t�@�͏c��
  #define   OFFSCREAN_WIDTH   (400.0f)
  #define   OFFSCREAN_HEIGHT  (240.0f)
  #define   LOW_OFFSCREAN_WIDTH   (320.0f)
  #define   LOW_OFFSCREAN_HEIGHT  (240.0f)

  G3DEdgeFilter(void);

  ~G3DEdgeFilter();

  /**
   * @brief ���[�N�m��
   * @param heap_allocator      �������m�ې�
   * @param device_allocator    �������m�ې�
   */
  void CreateWork( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator );

  /**
   * @brief �G�b�W���[�N�j��
   */
  void DestroyWork(void);

  /**
   * @beirf �G�b�W�����ɗp����e�N�X�`���ݒ�
   * @param texture   �e�N�X�`��
   */
  void SetTexture( nw::gfx::res::ResTexture texture, s32 layerNo = -1 );
  void SetColorTarget( nw::gfx::res::ResTexture target );
  void CreateDepthTarget( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system );
  
  /**
   * @brief �G�b�W�̕��ݒ�
   * @param scale   �X�P�[��
   */
  void ScaleEdgeWidth( f32 scale = 1.0f, gfl::grp::DisplayType type = gfl::grp::DISPLAY_UPPER );
  void ScaleEdgeWidth( f32 screanWidth, f32 screanHeight, f32 scale = 1.0f );
  
  /**
   * @brief �f�B�X�v���C�^�C�v�̐ݒ�
   * @param type   �f�B�X�v���C�^�C�v
   */
  void SetDisplayType( gfl::grp::DisplayType type );
  
  /**
   * @brief �G�b�W�̃J���[����
   * @param r   ��
   * @param g   ��
   * @param b   ��
   * @param a   �h�c�J���[
   */
  void SetEdgeColor( f32 r, f32 g, f32 b, f32 a = 0.0f );
  
  /**
   * @brief �G�b�W�`��p���f���i�S���_�v���[���j�擾
   * @return ���f��
   */
  gfl::grp::g3d::Model* GetShaderModel(s32 type, s32 path = 0)
  {
    return m_pEdgeModel[type][path];
  }
  
  void SetEdgeType( EdgeType type );
  
  EdgeType GetEdgeType()
  {
    return m_Type;
  }

private:
  
  
  gfl::grp::g3d::Resource         m_Resource[TYPE_MAX];
  gfl::grp::g3d::Model            *m_pEdgeModel[TYPE_MAX][MAX_PATH];
  nw::gfx::res::ResTexture        m_ColorTarget;
  nw::gfx::res::ResImageTexture   m_DepthTarget;
  EdgeType                        m_Type;
  f32                             m_Scale;

};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DEDGESYSTEM_H__
