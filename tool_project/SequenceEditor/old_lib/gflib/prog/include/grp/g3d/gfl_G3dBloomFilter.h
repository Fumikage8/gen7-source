#ifndef __GFL_G3DBLOOMFILTER_H__
#define __GFL_G3DBLOOMFILTER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dBloomFilter.h
 *	@brief  3D�u���[���t�B���^
 *	@author	Koji Kawada
 *	@date		2011.04.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dCamera.h>


// �؂�ւ�
#define GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE (1)  // ���ꂪ1�̂Ƃ��A�t���[���o�b�t�@�̏c���T�C�Y��2�ׂ̂���ɂȂ��Ă���
                                                              // �t���[���o�b�t�@��1�x�ʂ̃e�N�X�`���ɃR�s�[���邱�ƂȂ����̂܂܃e�N�X�`���Ƃ��Ďg�p�ł���B
                                                              // ���ꂪ0�̂Ƃ��A�t���[���o�b�t�@�̏c���T�C�Y��2�ׂ̂���ɂ͂Ȃ��Ă��炸
                                                              // �t���[���o�b�t�@��1�x�ʂ̃e�N�X�`���ɃR�s�[���Ȃ���΂Ȃ�Ȃ��B


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�u���[���t�B���^�N���X
//=====================================
class BloomFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomFilter);


  // ���L�N���X�����private�֐��ɂ��A�N�Z�X���Ă悢
  friend class Scene;
  friend class G3DPostEffectSystem;


  // �p��
  // plain_image           ���P�x�𒊏o����錳�摜
  // high_luminance        plain_image���獂�P�x�𒊏o����ߒ��⌋�ʂɗp����
  // high_luminance_board  high_luminance��
  // bloom_board           �uhigh_luminance_board��`�悵�Ăł������P�x�݂̂̉摜�v����������u���o����(�u���[��)�̉摜�v��\�����B�u���[�������Z�`�悷��B
  // bloom_filter          ����
  // small_image           �uhigh_luminance_board��`�悵�Ăł������P�x�݂̂̉摜�v�̏k���摜


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#if 0
  //f32�ɂ����̂Ń{�c
  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  /// bloom_board�̍L���� 
  //=====================================
  enum BloomBoardExpanse
  {
    BLOOM_BOARD_EXPANSE_ZERO,
    BLOOM_BOARD_EXPANSE_SMALL,
    BLOOM_BOARD_EXPANSE_MIDDLE,
    BLOOM_BOARD_EXPANSE_LARGE
  };
#endif

  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	bloom_board�̐ݒ���e
  //=====================================
  struct BloomBoardDescription
  {
    u32              small_image_count;   // �\��t����摜�͏k���񐔉���̂��̂�(0�̂Ƃ�m_high_luminance_board_render_target�A
                                          // n�̂Ƃ�m_small_image_array[n-1]��\��t����Bn<=m_small_image_num)
    gfl::math::VEC2  uv;                  // UV���W(-1.0f<= <=1.0f)
    f32              rate;                // ���Z����ۂ̊���(0.0f<= <=1.0f)
  };

  // BloomBoardDescription�̏������֐�
  // �f�t�H���g�l��ݒ肷��
  static void InitializeBloomBoardDescriptionForDefault(
      BloomBoardDescription*  description
  );

  // BloomBoardDescription�̏������֐�
  static void InitializeBloomBoardDescription(
      BloomBoardDescription*  description,
      const u32              small_image_count,
      const gfl::math::VEC2& uv,
      const f32              rate
  );

  //-------------------------------------
  ///	�ݒ���e
  //=====================================
  struct Description
  {
    s32                  high_luminance_board_render_target_width;   // �e�N�X�`���Ƃ��ėp���郌���_�[�^�[�Q�b�g�Ȃ̂ŁA���A�����ǂ�������s�̏����𖞂������ƁB�����`�ł��悢�B
    s32                  high_luminance_board_render_target_height;  // 2�ׂ̂���B8<= <=1024�BFORMAT_HW_ETC1�̂Ƃ���16<= <=1024�BnngxFilterBlockImage�̐���������small_image_num�ɂ����ӂ��邱�ƁB
    Texture::Format      high_luminance_board_render_target_format;  // �����_�[�^�[�Q�b�g�Ɏg����t�H�[�}�b�ggfl::grp::RenderColorFormat�ƑΉ��������̂�n�����ƁB�����small_image�̃t�H�[�}�b�g�ɂ��Ȃ�܂��B
    gfl::grp::MemoryArea high_luminance_board_render_target_color_memory_area;  // @note �A�h���X�w����K�v���H
    s32                  high_luminance_board_scene_calculate_group_index;
    s32                  high_luminance_board_scene_draw_group_index;
    gfl::grp::ColorF32   high_luminance_color_weight;                    // �P�x�v�Z�ɗp����RGB�̔䗦
    f32                  high_luminance_min_value;                       // ���P�x�Ƃ݂Ȃ��l
    u32       small_image_num;          // �k����(0�̂Ƃ�1����k�����Ȃ�)
    s32                     bloom_board_scene_calculate_group_index;
    s32                     bloom_board_scene_draw_group_index;
    u32                     bloom_board_num;
    const BloomBoardDescription*  bloom_board_description_array;  // bloom_board_num���̔z���������NULL��n���ĉ������B
        // ��
        // gfl::grp::g3d::BloomFilter::Description              bloom_filter_description;
        // gfl::grp::g3d::BloomFilter::BloomBoardDescription    bloom_board_description_array[3];
        // bloom_filter_description.bloom_board_num               = 3;
        // bloom_filter_description.bloom_board_description_array = bloom_board_description_array;
    s32       camera_scene_calculate_group_index;
    s32       camera_scene_camera_index;
    gfl::grp::MemoryArea  frame_buffer_copy_texture_memory_area;  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 �̂Ƃ��̂ݎg�p�����l�B
                                                                  // gfl::grp::MEMORY_AREA_NONE�̏ꍇ�͎g�p����Ȃ��̂ŁA
                                                                  // plain_image�̓t���[���o�b�t�@�ł͂Ȃ��I�t�X�N���[���o�b�t�@�łȂ���΂Ȃ�Ȃ��B
  };

  // Description�̏������֐�
  // �f�t�H���g�l��ݒ肷��
  static void InitializeDescriptionForDefault(
      Description*  description
  );

  // Description�̏������֐�
  static void InitializeDescription(
      Description*  description,
      const s32                  high_luminance_board_render_target_width,
      const s32                  high_luminance_board_render_target_height,
      const Texture::Format      high_luminance_board_render_target_format,
      const gfl::grp::MemoryArea high_luminance_board_render_target_color_memory_area,
      const s32                  high_luminance_board_scene_calculate_group_index,
      const s32                  high_luminance_board_scene_draw_group_index,
      const gfl::grp::ColorF32&  high_luminance_color_weight,
      const f32                  high_luminance_min_value,
      const u32       small_image_num,
      const s32                     bloom_board_scene_calculate_group_index,
      const s32                     bloom_board_scene_draw_group_index,
      const u32                     bloom_board_num,
      const BloomBoardDescription*  bloom_board_description_array,
      const s32       camera_scene_calculate_group_index,
      const s32       camera_scene_camera_index,
      const gfl::grp::MemoryArea  frame_buffer_copy_texture_memory_area  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 �̂Ƃ��̂ݎg�p�����l�B
  ); 


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // friend�N���X����̂݃A�N�Z�X���Ă��悢private�����o�֐�
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  BloomFilter(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~BloomFilter();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *  @param[in]  heap_allocator            �q�[�v�������̃A���P�[�^
   *  @param[in]  device_allocator          �f�o�C�X�������̃A���P�[�^
   *  @param[in]  graphics_system           �O���t�B�b�N�X�V�X�e��
   *  @param[in]  frame_buffer_memory_area  �I���X�N���[���̃t���[���o�b�t�@�̃������G���A
   *  @param[in]  description               �ݒ���e
   */
  //-----------------------------------------------------------------------------
  // �R�}���h�~��
  void Create(
      gfl::heap::NwAllocator*         heap_allocator,
      gfl::heap::NwAllocator*         device_allocator,
      const gfl::grp::GraphicsSystem* graphics_system,
      const gfl::grp::MemoryArea      frame_buffer_memory_area,
      const Description*              description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  Model*                   GetHighLuminanceBoardModel(void) const  {  return m_high_luminance_board_model;  }
  s32                      GetHighLuminanceBoardSceneCalculateGroupIndex(void) const  {  return m_high_luminance_board_scene_calculate_group_index;  }
  s32                      GetHighLuminanceBoardSceneDrawGroupIndex(void) const  {  return m_high_luminance_board_scene_draw_group_index;  }
  gfl::grp::RenderTarget*  GetHighLuminanceBoardRenderTarget(void) const  {  return m_high_luminance_board_render_target;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  u32  GetSmallImageNum(void) const  { return m_small_image_num; } 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  s32     GetBloomBoardSceneCalculateGroupIndex(void) const  {  return m_bloom_board_scene_calculate_group_index;  }
  s32     GetBloomBoardSceneDrawGroupIndex(void) const  {  return m_bloom_board_scene_draw_group_index;  }
  u32     GetBloomBoardNum(void) const  {  return m_bloom_board_num;  }
  Model*  GetBloomBoardModel(const u32 bloom_board_index) const  {  return m_bloom_board_array[bloom_board_index].model;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  s32      GetCameraSceneCalculateGroupIndex(void) const  {  return m_dummy_camera_scene_calculate_group_index;  }
  s32      GetCameraSceneCameraIndex(void) const  {  return m_dummy_camera_scene_camera_index;  }
  Camera*  GetCamera(void) const  {  return m_dummy_camera;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �k���R�s�[���s��
   */
  //-----------------------------------------------------------------------------
  // �R�}���h�~��(���s���֐����Ă�)
  void CopySmallerAll(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���P�x�𒊏o����錳�摜��ݒ肷��
   *                 �I���X�N���[���̃t���[���o�b�t�@�����摜�Ƃ���ꍇ
   */
  //-----------------------------------------------------------------------------
  // �R�}���h�~��
  void SetPlainImageFrameBufferToHighLuminanceBoard(
      gfl::grp::GraphicsSystem*       graphics_system,
      const gfl::grp::DisplayType     plain_image_display_type
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���P�x�𒊏o����錳�摜��ݒ肷��
   *                 �I�t�X�N���[�������摜�Ƃ���ꍇ
   */
  //-----------------------------------------------------------------------------
  // �R�}���h�~��
  void SetPlainImageOffScreenBufferToHighLuminanceBoard(const gfl::grp::RenderTarget* plain_image_off_screen_buffer);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �ݒ� / �擾
   */
  //-----------------------------------------------------------------------------
  void SetHighLuminanceColorWeight(const gfl::grp::ColorF32& c);
  void GetHighLuminanceColorWeight(gfl::grp::ColorF32* c) const;

  void SetHighLuminanceMinValue(const f32 v);
  f32  GetHighLuminanceMinValue(void) const;

  // bloom_board�̍L�����ݒ�/�擾����
  void SetBloomBoardExpanse(const f32 bloom_board_expanse);
  f32  GetBloomBoardExpanse(void) const;
  // bloom_board�̋�����ݒ�/�擾����
  void SetBloomBoardIntensity(const f32 bloom_board_intensity);
  f32  GetBloomBoardIntensity(void) const;
  // bloom_board�ɓ\��t����摜�͏k���񐔉���̂��̂�(0�̂Ƃ�m_high_luminance_board_render_target�A
  // n�̂Ƃ�m_small_image_array[n-1]��\��t����Bn<=m_small_image_num)
  void SetBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count);
  u32  GetBloomBoardSmallImageCount(const u32 bloom_board_index) const;

  // @note ���ɒ������鍀�ڂ𑝂₷�Ƃ�����A�A�A
  // 1��1��bloom_board�̔z�u�𓮂�����悤�ɂ���
  // �A�A�A���炢���Ȃ�




  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�֐�
private:
  // �k���R�s�[���s��
  // �R�}���h�~��
  void CopySmaller( const nw::gfx::res::ResTexture src_texture, nw::gfx::res::ResTexture dst_texture );


  ///////////////////////////////////////////////////////////////////////////////
  // friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�ϐ�
private:
  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	bloom_board�̏����܂Ƃ߂��\����
  //=====================================
  struct BloomBoard
  {
    Resource*        resource;
    Model*           model;               // �|���S�����f��
    u32              small_image_count;
    gfl::math::VEC2  uv;
    f32              rate;
  };


private:
#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  gfl::grp::RenderTarget*    m_frame_buffer_render_target_array[gfl::grp::DISPLAY_BOTH_MAX];  // �I���X�N���[���o�b�t�@�ł���t���[���o�b�t�@�Ɠ����J���[�o�b�t�@�����I�t�X�N���[���o�b�t�@
#else
  gfl::grp::g3d::Texture*    m_frame_buffer_copy_texture;  // �I���X�N���[���o�b�t�@�ł���t���[���o�b�t�@�̊G���R�s�[�����e�N�X�`���B
                                                           // �t���[���o�b�t�@��400*240�����A
                                                           // ���̃e�N�X�`����512*256(�e�N�X�`����2�ׂ̂���łȂ���΂Ȃ�Ȃ��̂�)�ɂ��Ă���B
#endif

  Resource*               m_high_luminance_board_resource;
  Model*                  m_high_luminance_board_model;
  s32                     m_high_luminance_board_scene_calculate_group_index;
  s32                     m_high_luminance_board_scene_draw_group_index;
  gfl::grp::RenderTarget* m_high_luminance_board_render_target;

  u32        m_small_image_num;
  Texture**  m_small_image_array;  // m_small_image_array = GFL_NEW_ARRAY() Texture*[m_small_image_num];
                                   // m_small_image_array[n-1]��n��k���摜(1<=n)
                                   // 0��k���摜�́A����1����k�����Ă��Ȃ��摜�́Am_high_luminance_board_render_target

  s32          m_bloom_board_scene_calculate_group_index;
  s32          m_bloom_board_scene_draw_group_index;
  u32          m_bloom_board_num;
  BloomBoard*  m_bloom_board_array;
  f32          m_bloom_board_expanse;
  f32          m_bloom_board_intensity;

  Camera*  m_dummy_camera;
  s32      m_dummy_camera_scene_calculate_group_index;
  s32      m_dummy_camera_scene_camera_index;




#if GFL_DEBUG
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // check_board�̓f�o�b�O�p�r

  ///////////////////////////////////////////////////////////////////////////////
  // friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�ϐ�
private:
  Resource* m_texture_check_board_resource;
  Model*    m_texture_check_board_model;
  
  ///////////////////////////////////////////////////////////////////////////////
  // friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�֐�
private:
  void CreateTextureCheckBoardModel(
      gfl::heap::NwAllocator*  heap_allocator,
      gfl::heap::NwAllocator*  device_allocator
  );
  void DestroyTextureCheckBoardModel(void);
  
  ///////////////////////////////////////////////////////////////////////////////
  // friend�N���X����̂݃A�N�Z�X���Ă��悢private�����o�֐�
private:
  Model* GetTextureCheckBoardModel(void) const  {  return m_texture_check_board_model;  }
#if 0
//���܂��\������Ȃ��̂ŁA�g�p�֎~�B
  // �R�}���h�~��
  void   SetImageFrameBufferToTextureCheckBoard(const gfl::grp::DisplayType image_display_type);
#endif
  // �R�}���h�~��
  void   SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer);
  // �R�}���h�~��
  void   SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer, const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate);

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
#endif  // #if GFL_DEBUG

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DBLOOMFILTER_H__
