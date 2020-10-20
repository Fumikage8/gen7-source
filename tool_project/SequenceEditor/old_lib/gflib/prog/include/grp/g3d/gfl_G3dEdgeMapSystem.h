#ifndef __GFL_G3DEDGEMAPSYSTEM_H__
#define __GFL_G3DEDGEMAPSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dEdgeMapSystem.h
 *	@brief  3D�e�V�X�e��
 *	@author	Masateru Ishiguro
 *	@date		2011.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/gfx/gfx_Model.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>
#include <grp/g3d/gfl_G3dH3dModel.h>

#include <base/gfl_KeyContainer.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )	\
	class CLASSNAME {											\
	public:																\
		enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )		\
		};                                  \
	};

namespace gfl {
namespace grp {
namespace g3d {
  
//-------------------------------------------------------------------------------------------------------------------------------------------------
//!  ���[�U�[�A�g���r���[�gEdgeMapAlphaMask
DECLARE_ENUM_BEGIN( EdgeMapAlphaMask )
  None = 0,
  Texture0,
  Texture1,
  Texture2,
  NumberOf
DECLARE_ENUM_END( EdgeMapAlphaMask )  

  
enum{
  DEFAULT_EDGE_MODEL_VCOL_A = 0,
  ALPHA_EDGE_MODEL_VCOL_A,
  DISABLE_NORMAL_EDGE_MODEL_VCOL_A,
  ALPHA_DISABLE_NORMAL_EDGE_MODEL_VCOL_A,
  DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A,
  ALPHA_DISABLE_NORMAL_EDGE_DISABLE_MODEL_VCOL_A,
  EDGE_MODEL_MAX
};

DECLARE_ENUM_BEGIN( CommandType )
  Upper = 0,                      // ���ʁ@or �����
  Lower,                          // �����
  UpperRight,                     // �E���
  None,                           // �Ή������ʂȂ��B 
  NumberOf
DECLARE_ENUM_END( CommandType )  
  
DECLARE_ENUM_BEGIN( EdgeType )
  DefaultEdgeTypeID = 0,
  VColorEdgeTypeID,
  None,                       // ���߁i�`�悵�Ȃ��j
  Erace,                      // ����
  OwnColor,                   // ���O�œh��i�̂Ń}�e���A���؂�ւ����s��Ȃ��j
  NumberOf
DECLARE_ENUM_END( CommandType )    

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
  
//-------------------------------------
///	�v�������_�[�p�X���b�g
//=====================================  
class EdgeMapPreRenderCallbackSlot : public nw::gfx::Model::RenderSlot, public gfl::grp::g3d::H3dMeshListener
{
public:
    
    EdgeMapPreRenderCallbackSlot() : nw::gfx::Model::RenderSlot(NULL),
      m_MorphingEnable( false )
    {

    }
    virtual ~EdgeMapPreRenderCallbackSlot(){}
    
    //----------------------------------------------------------------------
    /**
     * @brief nw::gfx::Model�p�R�[���o�b�N�֐�
     * @param model           �V�F�[�_�[�K���Ώ�
     * @param mesh            �V�F�[�_�[�K���Ώ�
     * @param renderContext   ���݂�RenderContext
     */
    //----------------------------------------------------------------------
    virtual void Invoke(nw::gfx::Model* model, nw::gfx::ResMesh mesh, nw::gfx::RenderContext* renderContext);
    
    //----------------------------------------------------------------------
    /**
     * @brief gfl::grp::g3d::H3dModel�p�R�[���o�b�N�֐�
     * @param model             �V�F�[�_�[�K���Ώ�
     * @param material_index    ���݂̃}�e���A���C���f�b�N�X
     * @param command           �R�}���h�̐ςݐ�
     * @param env               ���݂�H3dEnvironment
     */
    //----------------------------------------------------------------------
    virtual void Invoke(gfl::grp::g3d::H3dModel* model, s32 material_index, gfl::grp::g3d::H3dCommand* command, gfl::grp::g3d::H3dEnvironment* env);
    
    //----------------------------------------------------------------------
    /**
     * @brief �G�b�W�}�b�v���f���ݒ�֐��i���������ɐݒ肳��܂��B�j
     * @param id            ���f���C���f�b�N�X
     * @param p_model       ���f���f�[�^�|�C���^
     */
    //----------------------------------------------------------------------
    void SetEdgeModel( s32 id, gfl::grp::g3d::Model* p_model ){ m_pGfxEdgeModel[id] = p_model; }
    
    //----------------------------------------------------------------------
    /**
     * @brief �G�b�W�}�b�vH3d���f���ݒ�֐��i���������ɐݒ肳��܂��B�j
     * @param id            ���f���C���f�b�N�X
     * @param p_model       ���f���f�[�^�|�C���^
     */
    //----------------------------------------------------------------------
    void SetEdgeModel( s32 id, gfl::grp::g3d::H3dModel* p_model ){ m_pH3dEdgeModel[id] = p_model; }
    
    //----------------------------------------------------------------------
    /**
     * @brief �R�[���o�b�N�L�������ݒ�֐�
     * 
     * �@�@�@�@�G�b�W�}�b�v�ɓo�^���ꂽ���f���͂��̃��X�i�[���Ԃ牺���邪�A
     * �@�@�@�@���ۂɃG�b�W�}�b�v�ɕ`�����łȂ��ꍇ�́Afalse���n�����B
     * 
     * @param f     �L�������t���O
     */
    //----------------------------------------------------------------------
    void RenderCallbackEnable( b32 f ){ m_fEnable = f; }
    
    //----------------------------------------------------------------------
    /**
     * @brief ���[�t�B���O�Ή��V�F�[�_�[���g����
     * 
     * �@�@�@�@�@kujira�ł́A���ǃ��[�t�B���O�V�F�[�_�[�͎�������܂���ł����B
     * �@�@�@�@�@�̂ł��̊֐��͎g�p����Ă��܂���B
     * 
     * @param f   �L�������t���O
     */
    //----------------------------------------------------------------------
    void SetMorphingEnable( b32 f ){ m_MorphingEnable = f; }
    
private:    
    gfl::grp::g3d::Model*       m_pGfxEdgeModel[EDGE_MODEL_MAX];    //!< �}�e���A�������ւ��悤Gfx���f��
    gfl::grp::g3d::H3dModel*    m_pH3dEdgeModel[EDGE_MODEL_MAX];    //!< �}�e���A�������ւ��悤H3d���f��
    b32                         m_fEnable;                          //!< Invoke�L���t���O
    b32                         m_MorphingEnable;                   //!< ���[�t�B���O�L���t���O
};

//-------------------------------------
///	�|�X�g�����_�[�p�X���b�g
//=====================================
class EdgeMapPostRenderCallbackSlot : public nw::gfx::Model::RenderSlot, public gfl::grp::g3d::H3dMeshListener
{
public:
    EdgeMapPostRenderCallbackSlot() : nw::gfx::Model::RenderSlot(NULL){}
    
    virtual ~EdgeMapPostRenderCallbackSlot(){}
    
    //----------------------------------------------------------------------
    /**
     * @brief nw::gfx::Model�p�R�[���o�b�N�֐�
     * @param model           �V�F�[�_�[�K���Ώ�
     * @param mesh            �V�F�[�_�[�K���Ώ�
     * @param renderContext   ���݂�RenderContext
     */
    //----------------------------------------------------------------------
    virtual void Invoke(nw::gfx::Model* model, nw::gfx::ResMesh mesh, nw::gfx::RenderContext* renderContext);
    
    //----------------------------------------------------------------------
    /**
     * @brief gfl::grp::g3d::H3dModel�p�R�[���o�b�N�֐�
     * @param model             �V�F�[�_�[�K���Ώ�
     * @param material_index    ���݂̃}�e���A���C���f�b�N�X
     * @param command           �R�}���h�̐ςݐ�
     * @param env               ���݂�H3dEnvironment
     */
    //----------------------------------------------------------------------
    virtual void Invoke(gfl::grp::g3d::H3dModel* model, s32 material_index, gfl::grp::g3d::H3dCommand* command, gfl::grp::g3d::H3dEnvironment* env);
    
    //----------------------------------------------------------------------
    /**
     * @brief �R�[���o�b�N�L�������ݒ�֐�
     * 
     * �@�@�@�@�G�b�W�}�b�v�ɓo�^���ꂽ���f���͂��̃��X�i�[���Ԃ牺���邪�A
     * �@�@�@�@���ۂɃG�b�W�}�b�v�ɕ`�����łȂ��ꍇ�́Afalse���n�����B
     * 
     * @param f     �L�������t���O
     */
    //----------------------------------------------------------------------
    void RenderCallbackEnable( b32 f ){ m_fEnable = f; }
    
    b32                         m_fEnable;      //!< Invoke�L���t���O   @note privete�ɂ����ق����ǂ��B
};

//-------------------------------------
///	�G�b�W�V�X�e���N���X
//=====================================
class G3DEdgeMapSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(G3DEdgeMapSystem);
  friend class Scene;
  
public:
  /**
   * @brief �h�c�G�b�W���L�����̃��[�U�[�^�O���擾
   * @return �^�O��
   */
  static const char* GetIDEdgeEnableTagName();
  
  /**
   * @brief �G�b�W�h�c�l�̃��[�U�[�^�O���擾
   * @return �^�O��
   */
  static const char* GetEdgeIDTagName();
  
  /**
   * @brief ���[�t�B���O�L�������ݒ�֐�
   * @param fEnable     �L�������t���O
   */
  void MorphingEnable( f32 fEnable );
  
  /**
   * @brief �R�[���o�b�N�L�������֐�
   * 
   * �@�@�@�@�G�b�W�}�b�v�ɓo�^���ꂽ���f���͂��̃��X�i�[���Ԃ牺���邪�A
   * �@�@�@�@���ۂɃG�b�W�}�b�v�ɕ`�����łȂ��ꍇ�́Afalse���n�����B
   * 
   * @param f   �L�������t���O
   */
  void RenderCallbackEnable( b32 f );
  
public:
  
  static const s32          EDGE_MAP_WIDTH = 512;     //!< �G�b�W�}�b�v�`��惌���_�[�^�[�Q�b�g�̕�
  static const s32          EDGE_MAP_HEIGHT = 256;    //!< �G�b�W�}�b�v�`��惌���_�[�^�[�Q�b�g�̍���
  
  G3DEdgeMapSystem();
  ~G3DEdgeMapSystem();
  
  /**
   * @brief �G�b�W�V�X�e�����[�N�m��
   * @param heap_allocator      �������m�ې�
   * @param device_allocator    �������m�ې�
   * @param p_desc              �����_�[�^�[�Q�b�g�ݒ���e
   */
  void CreateWork(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::RenderTarget::Description* p_desc, gfl::grp::RenderTarget::Description* p_low_desc, b32 f_delete_memory_control );
  
  /**
   * @brief �}�e���A���؂�ւ��p���f���쐬�֐�
   * @param heap_allocator            �������m�ې�
   * @param device_allocator          �������m�ې�
   */
  void createGfxModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  
  /**
   * @brief �}�e���A���؂�ւ��p���f���쐬�֐�
   * @param heap_allocator            �������m�ې�
   * @param device_allocator          �������m�ې�
   * @param f_delete_memory_control   ������R���g���[��
   */
  void createH3dModel(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, b32 f_delete_memory_control);

  /**
   * @brief �G�b�W���[�N�j��
   */
  void DestroyWork(void);

  /**
   * @brief �`��O���[�v�ݒ�
   * @param scene_draw_group_index    �`��O���[�v
   */
  void SetGroupIndex( s32 scene_draw_group_index ){ m_GroupIndex = scene_draw_group_index; }

  /**
   * @brief   �`��O���[�v�C���f�b�N�X�擾
   * @return  �`��O���[�v�C���f�b�N�X
   */
  s32 GetGroupIndex(){ return m_GroupIndex; }

  /**
   * @brief   �����_�[�^�[�Q�b�g�擾
   * @return  �����_�[�^�[�Q�b�g
   */
  gfl::grp::RenderTarget* GetEdgeRenderTarget( s32 type ) const
  {
    switch( type ){
    case DISPLAY_UPPER:                          // ���ʁ@or �����
      return m_render_target[CommandType::Upper];
    case DISPLAY_LOWER:                          // �����
      return m_render_target[CommandType::Lower];
    case DISPLAY_UPPER_RIGHT:                    // �E���
      return m_render_target[CommandType::UpperRight];
    }

    return m_render_target[CommandType::None];// �Ή������ʂȂ��B 
  }
  
  /**
   * @brief �}�e���A�������ւ����X�i�[�擾
   * @return ���X�i�[�C���X�^���X
   */
  EdgeMapPreRenderCallbackSlot* GetPreRenderCallbackSlot(){ return &m_PreRenderCallbackSlot; }
  
  /**
   * @brief �}�e���A�������ւ����X�i�[�擾
   * @return ���X�i�[�C���X�^���X
   */
  EdgeMapPostRenderCallbackSlot* GetPostRenderCallbackSlot(){ return &m_PostRenderCallbackSlot; }
  
  DECLARE_ENUM_BEGIN( CommonResourceType )
    NormalTexture = 0,
    NormalToUvShader,
    NumberOf
  DECLARE_ENUM_END( CommonResourceType )  

  s32                       m_GroupIndex;                                         //!< �`��t���[�v
  gfl::grp::g3d::Resource   m_GfxCommonResource[CommonResourceType::NumberOf];    //!< �}�e���A�������ւ��悤���f�����\�[�X
  gfl::grp::g3d::Resource   m_GfxResource[EDGE_MODEL_MAX];                        //!< �}�e���A�������ւ��悤���f�����\�[�X
  gfl::grp::g3d::Model      m_GfxEdgeModel[EDGE_MODEL_MAX];                       //!< �}�e���A�������ւ��悤���f��
  
  gfl::grp::g3d::H3dResource   m_H3dCommonResource[CommonResourceType::NumberOf];             //!< �}�e���A�������ւ��悤���f�����\�[�X
  b8                           m_H3dCommonResourceReleaseFlag[CommonResourceType::NumberOf];  //!< �}�e���A�������ւ��悤���f�����\�[�X����t���O
  gfl::grp::g3d::H3dResource   m_H3dResource[EDGE_MODEL_MAX];                                 //!< �}�e���A�������ւ��悤���f�����\�[�X
  b8                           m_H3dResourceReleaseFlag[EDGE_MODEL_MAX];                      //!< �}�e���A�������ւ��悤���f�����\�[�X����t���O
  gfl::grp::g3d::H3dModel      m_H3dEdgeModel[EDGE_MODEL_MAX];                                //!< �}�e���A�������ւ��悤���f��
  void                         *m_pH3dCommonData[ CommonResourceType::NumberOf ];             //!< �}�e���A�������ւ��悤���f�����\�[�X
  void                         *m_pH3dResourceData[ EDGE_MODEL_MAX ];                         //!< �}�e���A�������ւ��悤���f�����\�[�X
  
  gfl::grp::RenderTarget*         m_render_target[CommandType::NumberOf];                     //!< �G�b�W�}�b�v�`���
  EdgeMapPreRenderCallbackSlot    m_PreRenderCallbackSlot;                                    //!< �`��O�R�[���o�b�N���X�i�[
  EdgeMapPostRenderCallbackSlot   m_PostRenderCallbackSlot;                                   //!< �`���R�[���o�b�N���X�i�[
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#undef		DECLARE_ENUM_BEGIN
#undef		DECLARE_ENUM_END

#endif // __GFL_G3DEDGEMAPSYSTEM_H__
