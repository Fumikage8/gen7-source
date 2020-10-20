#ifndef __GFL_G3DTEXTURE_H__
#define __GFL_G3DTEXTURE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dTexture.h
 *	@brief  3D�e�N�X�`��
 *	@author	Koji Kawada
 *	@date		2011.04.06
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
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dResource.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3D�e�N�X�`���N���X
//=====================================
class Texture : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Texture);


  // ���Lfriend�N���X�����private�֐��ɂ��A�N�Z�X���Ă悢
  friend class Model;
  friend class BloomFilter;
  friend class System;


public:
  //---------------------------------------------------------------------------
  /**
   *           �萔�錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�e�N�X�`���̃^�C�v
  //=====================================
  enum Type
  {
    TYPE_IMAGE_TEXTURE,  // �摜�f�[�^�ɂ�镽�ʃe�N�X�`��(���ʂ̃e�N�X�`��)
    TYPE_CUBE_TEXTURE,   // �L���[�u�e�N�X�`��
    TYPE_SHADOW_TEXTURE  // �V���h�E�e�N�X�`��
  };

  //-------------------------------------
  ///	�e�N�X�`���̃t�H�[�}�b�g
  //=====================================
  enum Format
  {
    FORMAT_RGBA8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA8   ,    //!< 1�s�N�Z����4�o�C�g�ARGBA���e8�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_RGB8        = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB8    ,    //!< 1�s�N�Z����3�o�C�g�ARGB���e8�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_RGBA5551    = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA5551,    //!< 1�s�N�Z����2�o�C�g�ARGB���e5�r�b�g�AA��1�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_RGB565      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGB565  ,    //!< 1�s�N�Z����2�o�C�g�ARGB���e5,6,5�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_RGBA4       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_RGBA4   ,    //!< 1�s�N�Z����2�o�C�g�ARGBA���e4�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_LA8         = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA8     ,    //!< 1�s�N�Z����2�o�C�g�AAlpha��Luminance���e8�r�b�g�̃t�H�[�}�b�g�ł� 
    FORMAT_HILO8       = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_HILO8   ,    //!< 1�s�N�Z����2�o�C�g�AX,Y���e8�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_L8          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L8      ,    //!< 1�s�N�Z����1�o�C�g�ALuminance��8�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_A8          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A8      ,    //!< 1�s�N�Z����1�o�C�g�AAlpha��8�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_LA4         = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_LA4     ,    //!< 1�s�N�Z����1�o�C�g�AAlpha��Luminance���e4�r�b�g�̃t�H�[�}�b�g�ł� 
    FORMAT_L4          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_L4      ,    //!< 1�s�N�Z����4�r�b�g�ALuminance��4�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_A4          = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_A4      ,    //!< 1�s�N�Z����4�r�b�g�AAlpha��4�r�b�g�̃t�H�[�}�b�g�ł��B
    FORMAT_ETC1        = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1    ,    //!< 1�s�N�Z����4�r�b�g�����̈��k�t�H�[�}�b�g�ł��B
    FORMAT_ETC1A4      = nw::gfx::res::ResPixelBasedTexture::FORMAT_HW_ETC1A4  ,    //!< 1�s�N�Z����8�r�b�g�����̈��k�t�H�[�}�b�g�ł��B                    
    FORMAT_MAX         = 14
  };

  //-------------------------------------
  ///	�L���[�u�e�N�X�`���̖�
  //=====================================
  enum CubeFace
  {
    CUBE_FACE_POSITIVE_X = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_X,   //!< X ���́{�����ł��B
    CUBE_FACE_NEGATIVE_X = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_X,   //!< X ���́|�����ł��B
    CUBE_FACE_POSITIVE_Y = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Y,   //!< Y ���́{�����ł��B
    CUBE_FACE_NEGATIVE_Y = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Y,   //!< Y ���́|�����ł��B
    CUBE_FACE_POSITIVE_Z = nw::gfx::res::ResCubeTexture::CUBE_FACE_POSITIVE_Z,   //!< Z ���́{�����ł��B
    CUBE_FACE_NEGATIVE_Z = nw::gfx::res::ResCubeTexture::CUBE_FACE_NEGATIVE_Z,   //!< Z ���́|�����ł��B
    CUBE_FACE_MAX        = nw::gfx::res::ResCubeTexture::MAX_CUBE_FACE           //!< �ʂ̐��ł��B       
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *           static�����o�֐�
   */
  //-----------------------------------------------------------------------------
  // gfl::grp::g3d::Texture::Format -> gfl::grp::RenderColorFormat
  static gfl::grp::RenderColorFormat    ConvertTextureFormatToRenderColorFormat(gfl::grp::g3d::Texture::Format texture_format);
  // gfl::grp::RenderColorFormat    -> gfl::grp::g3d::Texture::Format
  static gfl::grp::g3d::Texture::Format ConvertRenderColorFormatToTextureFormat(gfl::grp::RenderColorFormat    render_color_format);

  // format�̃e�N�X�`����1�s�N�Z���̃r�b�g�T�C�Y�𓾂�(�o�C�g�T�C�Y�ł͂���܂���)
  static u8 GetTextureBitsPerPixel(Format format);


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
    Type                         type;                   // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // �e�N�X�`���̃^�C�v

    s32                          height;                 // [TYPE_IMAGE_TEXTURE,                    TYPE_SHADOW_TEXTURE]  // ����
    s32                          width;                  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // ��

    gfl::grp::MemoryArea         memory_area;            // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // �������z�u�ꏊ(location_address��0�ȊO�̂Ƃ��͎g�p���Ȃ�)
    gfl::grp::MemoryTransfer     memory_transfer;        // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // �������]���^�C�v(location_address��0�ȊO�̂Ƃ��͎g�p���Ȃ�)
    u32                          location_address;       // [TYPE_IMAGE_TEXTURE,                    TYPE_SHADOW_TEXTURE]  // �C���[�W�W�J�ς݂̃A�h���X(VRAM ��̃A�h���X�ł��\���܂���)
    u32                          cube_face_location_address[CUBE_FACE_MAX];
                                                         // [                    TYPE_CUBE_TEXTURE                     ]  // �C���[�W�W�J�ς݂̃A�h���X(VRAM ��̃A�h���X�ł��\���܂���)
    b32                          executing_memory_fill;  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // �m�ۂ����s�N�Z���̈��0�ŃN���A����ꍇtrue
    b32                          is_dynamic_allocation;  // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE, TYPE_SHADOW_TEXTURE]  // ���I��FCRAM��ɗ̈���m�ۂ���ꍇ��true

    s32                          mipmap_size;            // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE                     ]  // �~�b�v�}�b�v�T�C�Y
                                                                                                                          // (�~�b�v�}�b�v���g�p���Ȃ��Ƃ���1���A�~�b�v�}�b�v���g�p����Ƃ��̓~�b�v�}�b�v��+1���w�肷��)
    Format                       format;                 // [TYPE_IMAGE_TEXTURE, TYPE_CUBE_TEXTURE                     ]  // �e�N�X�`���̃t�H�[�}�b�g

    b32                          is_perspective_shadow;  // [                                       TYPE_SHADOW_TEXTURE]  // ���e�V���h�E�̏ꍇ��true
    f32                          shadow_z_bias;          // [                                       TYPE_SHADOW_TEXTURE]  // �V���h�E�e�N�X�`����Z�o�C�A�X�l
    f32                          shadow_z_scale;         // [                                       TYPE_SHADOW_TEXTURE]  // �I�t�Z�b�g�l�ɏ�Z����X�P�[��
     
    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                  (TYPE_IMAGE_TEXTURE),
        height                (128),
        width                 (256),
        memory_area           (MEMORY_AREA_FCRAM),
        memory_transfer       (MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        location_address      (0),
        executing_memory_fill (false),
        is_dynamic_allocation (false),
        mipmap_size           (1),
        format                (FORMAT_RGBA4), 
        is_perspective_shadow (true),
        shadow_z_bias         (0.0f),
        shadow_z_scale        (1.0f)
    {
      for( s32 i=0; i<CUBE_FACE_MAX; ++i )  // �z��ɂȂ��Ă��郁���o�ϐ��́A�������q���X�g�ł͏������ł��Ȃ��B
        cube_face_location_address[i] = 0;
    }
  };

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  Texture(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~Texture();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *
   *	@param	heap_allocator     �q�[�v�A���P�[�^
   *	@param  device_allocator   �f�o�C�X�A���P�[�^
   *	@param  description        �ݒ���e
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *                 �����_�[�^�[�Q�b�g���e�N�X�`���Ƃ���
   *                 ����Ő�������Texture�C���X�^���X�������́A�����œn����render_target���ێ����Ă����ĉ������B
   *                 �c�����ӁI
   *
   *	@param  description        �����_�[�^�[�Q�b�g
   */
  //-----------------------------------------------------------------------------
  void Create(
      const gfl::grp::RenderTarget*  render_target
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);

private:
  nw::gfx::res::ResTexture CreateImageTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );
  nw::gfx::res::ResTexture CreateCubeTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );
  nw::gfx::res::ResTexture CreateShadowTexture(
      gfl::heap::NwAllocator*    device_allocator,
      const Description*         description
  );

  void DestroyImageTexture (nw::gfx::res::ResTexture nw_res_texture);
  void DestroyCubeTexture  (nw::gfx::res::ResTexture nw_res_texture);
  void DestroyShadowTexture(nw::gfx::res::ResTexture nw_res_texture);


public:
  nw::gfx::res::ResTexture  GetNwResTexture(void) const  {  return m_nw_res_texture;  }


private:
  nw::gfx::res::ResTexture     m_nw_res_texture;
  b32                          m_is_create;

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DTEXTURE_H__
