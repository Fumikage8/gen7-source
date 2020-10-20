#ifndef __GFL_G3DH3DENVIRONMENT_H__
#define __GFL_G3DH3DENVIRONMENT_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEnvironment.h
 *	@brief  H3D��
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*@note ���ӎ���

�J������p�ӂ����Ƃ��́A���̂��Ƃɒ��ӂ���ƌ������悭�Ȃ肻���B

      nn::math::MTX44 projection = nn::math::MTX44::Identity();
      nn::math::MTX34 view = nn::math::MTX34::Identity();

      // ���ׂẴV�F�[�_�̃v���W�F�N�V�����ƃr���[�� Uniform �𓯂��ԍ��̃��W�X�^�ɂ��邱�ƂŁA
      // ���ʂ� BindSymbol �Œl��ݒ肵�܂��B
      nn::gr::BindSymbolVSFloat projMtxBind;
      nn::gr::BindSymbolVSFloat viewMtxBind;
      projMtxBind.start = 86;
      projMtxBind.end = 89;
      viewMtxBind.start = 90;
      viewMtxBind.end = 92;

      bit32* command;
      nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));
      bit32* beginCommand = command;
      command = projMtxBind.MakeUniformCommand(command, projection);
      command = viewMtxBind.MakeUniformCommand(command, view);
      size_t commandSize = (command - beginCommand) * sizeof(bit32);
      nngxMoveCommandbufferPointer(commandSize);



����

����H3dEnvironment�ł�
nw::h3d::SceneState
�������܂��B
*/

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



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D���N���X
//=====================================
class H3dEnvironment
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dEnvironment);


  friend class H3dModel;
  friend class RenderSystem;


public:
  
  //---------------------------------------------------------------------------
  //! @brief �����K�p�ł��钸�_���C�g�̃Z�b�g�ł��B�@@fix ���삳��v�]�F�o�g���w�i�̃��C�e�B���O�Ή�
  //---------------------------------------------------------------------------
  struct VertexLightSet
  {
      static const int MAX_VERTEX_LIGHT_QUANTITY = 8;

      nw::h3d::res::VertexLightContent vertexLights[MAX_VERTEX_LIGHT_QUANTITY];
      nw::h3d::res::HemiSphereLightContent hemisphereLight;
      u8 vertexLightSize;
      NN_PADDING3;

      VertexLightSet() : vertexLightSize(0) {}

      nn::math::VEC4 m_position;
      
      void MakeCommand(
          nn::gr::CTR::CommandBufferJumpHelper& dcs,
          const nn::gr::CTR::BindSymbolVSInteger& vertexLightCnt,
          const nn::gr::CTR::BindSymbolVSFloat& univMtxBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslGColBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslSColBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslSDirBind) const;
  };
  typedef nw::h3d::fnd::DataArray<VertexLightSet> VertexLightSetArray;


  // �����_�[���[�h
  enum
  {
    RENDER_MODE_BIT_FLAG_NONE             = 0,        // �Ȃ�
    RENDER_MODE_BIT_FLAG_IGNORE_MATERIAL  = 1 <<  0,  // �}�e���A���̐ݒ�𖳎�����
    //RENDER_MODE_BIT_FLAG_IGNORE_SHADER    = 1 <<  1,  // �V�F�[�_�̐ݒ�𖳎�����

    RENDER_MODE_BIT_FLAG_DEFAULT  = RENDER_MODE_BIT_FLAG_NONE    // �f�t�H���g
  };

private:
  enum
  {
    RENDER_MODE_BIT_FLAG_IGNORE_SHADER    = 1 <<  1,  // �V�F�[�_�̐ݒ�𖳎�����  // �}�e���A���̐ݒ�𖳎�������V�F�[�_�̐ݒ����������悤��
                                                                                   // ���Ă���̂ŁA����͕s�v�ȋC������̂ŁA���̓A�N�Z�X�s�B
  };

public:
  // �ݒ���e
  struct Description
  {
    s32 texture_projector_num_max;
    s32 light_set_num_max;
    s32 fog_num_max;

    Description()
      : texture_projector_num_max(0),
        light_set_num_max(0),
        fog_num_max(0)
    {}
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �R���X�g���N�^ / �f�X�g���N�^
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   */
  //-----------------------------------------------------------------------------
  H3dEnvironment(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dEnvironment();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���� / �j��
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  void Create(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description& a_desc);
  void CreateFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene);
  //�g�p�֎~void CreateAndSetFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �ݒ�
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Scene����ݒ肷��
   *  @attention     ���̊֐��͖��t���[���Ă΂Ȃ���GPU�n���O�A�b�v���邩������܂���B
   */
  //-----------------------------------------------------------------------------
  void SetFromScene(const Scene* scene, const nw::gfx::Camera& camera);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �����_�[���[�h
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  �����_�[���[�h��ݒ肷��
  //         �r�b�g��ǉ��ŗ��Ă�̂ł͂Ȃ��A�����o�ϐ��ɂ��̂܂ܑ�����܂��B
  void SetRenderModeBitFlag(bit32 bit_flag);
  // @brief  �����_�[���[�h���擾����
  bit32 GetRenderModeBitFlag(void) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  //�@���C�g�J���[�擾�@@fix �Ƃ��₳��A���ߎq����v�]�F�t�B�[���h�G���J�E���g���\���œK���̂��߂̑Ή�
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  const nn::gr::FragmentLight& GetFragmentLight( s32 lightSetNo ) const;
  const nw::h3d::fnd::ColorSet& GetLightColor( s32 lightSetNo, int lightIdx ) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ͌��J���Ă������J�����o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  nw::h3d::SceneState* GetNwSceneState(void) { return (&m_scene_state); }
      // �uGetNwSceneState(void) const�v���Ɩ߂�l�̌^���uconst nw::h3d::SceneState*�v�ɂ��Ȃ��Ƃ����Ȃ�
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   */
  //-----------------------------------------------------------------------------
  void CreateFromSceneEnvironment(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const nw::gfx::SceneEnvironment& env, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         nw::gfx::SceneEnvironment����ݒ肷��
   *  @attention     ���̊֐��͖��t���[���Ă΂Ȃ���GPU�n���O�A�b�v���邩������܂���B
   */
  //-----------------------------------------------------------------------------
  void SetFromSceneEnvironment(const nw::gfx::SceneEnvironment& env, const nw::gfx::Camera& view_camera, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t�H�O��ݒ肷��R�}���h�𐶐�����
   */
  //-----------------------------------------------------------------------------
  bit32* MakeFogAllCommand(s32 index, bit32* command) const;
  bit32* MakeFogConfigCommand(s32 index, bit32* command) const;
  bit32* MakeFogTableCommand(s32 index, bit32* command) const;
  bit32* MakeFogDisableCommand(s32 index, bit32* command) const;


  VertexLightSet* GetVertexLightSet( s32 index ) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friend�ɂ����J���Ă��Ȃ�����J�����o
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  static s32 getLutDataEnableIndex(s32 light_set_index, s32 light_index);
  b32 setLutData(nw::gfx::res::ResLookupTable lut, s32 light_set_index, s32 light_index);  // �Z�b�g�ł�����true��Ԃ��A�Z�b�g�ł��Ȃ�������false��Ԃ�

private:
  //
  void*  m_buff;
  nw::h3d::SceneState  m_scene_state;
  VertexLightSetArray  m_VertexLightSets;
  VertexLightSet       *m_pVertexLightSetBuffer;

  // ���C�g
  u32                     m_max_light_sets;   // ���C�g�Z�b�g��
  gfl::base::Bit*         m_lut_data_enable;  // m_max_light_sets*nw::h3d::Constants::LIGHT_QUANTITY�������������m�ۂ���
                                              // ���Ԃ̓��C�g�Z�b�g0��01234567,���C�g�Z�b�g1��01234567,���C�g�Z�b�g2��01234567,...
  nw::h3d::res::LutData** m_lut_data_array;   // m_lut_data_array[m_max_light_sets][nw::h3d::Constants::LIGHT_QUANTITY]�ƂȂ�悤�Ƀ��������m�ۂ���




  //aaaaaaaaaaaaaaaaa
  static const int LUTDATA_BUFFER_SIZE = 1032;
  static const int SUBROUTINE_JUMP_COMMAND_SIZE = 8;
  struct LutDataCache
  {
      u8 lutDataBuffer[LUTDATA_BUFFER_SIZE + SUBROUTINE_JUMP_COMMAND_SIZE];
      nw::h3d::res::LutData lutData;
  };
  LutDataCache*** m_lut_data_cache_array;




  // �t�H�O
  const u32** m_fog_command_cache_array;

  //
  bit32  m_render_mode_bit_flag;  // @note �����̃n�[�h�ŕ��񏈗��ŕ`�悷��悤�ɂȂ�����A�V�[����1�����Ȃ����Ƀ����_�[���[�h���������Ă�����
                                  //       ���b�V���`�掞�ɂ����ύX�ł��Ă��܂��̂͊댯�B�����_�[���[�h�����f������(���b�V�����ƁH)�Ɏ������ق������������B
};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DENVIRONMENT_H__

