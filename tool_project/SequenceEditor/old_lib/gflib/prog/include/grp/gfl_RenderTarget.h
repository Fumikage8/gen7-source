#ifndef __GFL_RENDERTARGET_H__
#define __GFL_RENDERTARGET_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_RenderTarget.h
 *	@brief  �����_�[�^�[�Q�b�g
 *	@author	Koji Kawada
 *	@date		2011.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_fs.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_Viewport.h>



namespace gfl {
namespace grp {


namespace g3d {
  class Scene;
  class RenderSystem;
}  // namespace g3d


//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	�����_�[�^�[�Q�b�g�N���X
//=====================================
class RenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderTarget);


  // ���L�N���X�ɂ���private�����o�֐��ւ̃A�N�Z�X������
  friend class gfl::grp::g3d::Scene;
  friend class gfl::grp::g3d::RenderSystem;


public:
  //---------------------------------------------------------------------------
  /**
   *           �萔
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	�`��Ώۂ̃^�C�v
  //=====================================
  enum Type
  {
    TYPE_NONE,              // �f�[�^�Ȃ�
    TYPE_ON_SCREEN_BUFFER,  // ��ʂɕ\�������邽�߂̕`��Ώ�
    TYPE_OFF_SCREEN_BUFFER  // ��ʂɕ\�����Ȃ��`��Ώ�
  };

  //-------------------------------------
  ///	�o�b�t�@�^�C�v
  //=====================================
  enum RenderBufferType
  {
    RENDERBUFFER_TYPE_COLOR, // �J���[�o�b�t�@
    RENDERBUFFER_TYPE_DEPTH  // �f�v�X�o�b�t�@
  };
  
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
    Type                         type;           // �`��Ώۂ̃^�C�v
    s32                          width;          // ��
    s32                          height;         // ����
    gfl::grp::RenderColorFormat  color_format;   // �F�̌`��
    gfl::grp::RenderDepthFormat  depth_format;   // �[�x�̌`��
    gfl::grp::MemoryArea         color_area;     // �J���[�o�b�t�@�̃������z�u�ꏊ(color_address��0�ȊO�̂Ƃ��͎g�p���Ȃ�)
    gfl::grp::MemoryArea         depth_area;     // �f�v�X�o�b�t�@�̃������z�u�ꏊ(depth_address��0�ȊO�̂Ƃ��͎g�p���Ȃ�)
    u32                          color_address;  // �J���[�o�b�t�@�̃A�h���X���ڎw��(0�̂Ƃ���color_area���g�p����)
    u32                          depth_address;  // �f�v�X�o�b�t�@�̃A�h���X���ڎw��(0�̂Ƃ���depth_area���g�p����)
    gfl::grp::ShadowType         shadow_kind;    // �V���h�E�̎��

    // (type==TYPE_OFF_SCREEN_BUFFER) �̂Ƃ��g�p����ݒ�
    s32                          mipmap_size;            // �~�b�v�}�b�v�T�C�Y
                                                         // (�~�b�v�}�b�v���g�p���Ȃ��Ƃ���1���A�~�b�v�}�b�v���g�p����Ƃ��̓~�b�v�}�b�v��+1���w�肷��)
    gfl::grp::MemoryTransfer     color_transfer;         // �e�N�X�`���̃������]���^�C�v
    b32                          executing_memory_fill;  // �m�ۂ����s�N�Z���̈��0�ŃN���A����ꍇtrue
    b32                          is_dynamic_allocation;  // ���I��FCRAM��ɗ̈���m�ۂ���ꍇ��true
    // �����_�[�^�[�Q�b�g�ɂ���ꍇ�A�e�N�X�`���̃t�H�[�}�b�g��
    // RGBA8,
    // RGB8,
    // RGB5_A1,
    // RGB565,
    // RGBA4
    // �����g���Ȃ��̂ŁAgfl::grp::RenderColorFormat�Ɉ�v����B

    // (type==TYPE_OFF_SCREEN_BUFFER && shadow_kind==SHADOW_KIND_TEXTURE) �̂Ƃ��g�p����ݒ�
    b32                          is_perspective_shadow;  // ���e�V���h�E�̏ꍇ��true
    f32                          shadow_z_bias;          // �V���h�E�e�N�X�`����Z�o�C�A�X�l
    f32                          shadow_z_scale;         // �I�t�Z�b�g�l�ɏ�Z����X�P�[��
    gfl::grp::DisplayType        display_type;           // �`��ʏ��
    GLuint                       fob_id;                 // �m�ۍς�GL�p�t���[���o�b�t�@�I�u�W�F�N�g�i�Ȃ��ꍇ�O�j
     
    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                  (TYPE_OFF_SCREEN_BUFFER),
        width                 (512),
        height                (256),
        color_format          (gfl::grp::RENDER_COLOR_FORMAT_RGBA8),
        depth_format          (gfl::grp::RENDER_DEPTH_FORMAT_NONE),
        color_area            (gfl::grp::MEMORY_AREA_VRAMA),
        depth_area            (gfl::grp::MEMORY_AREA_NONE),
        color_address         (0),
        depth_address         (0),
        shadow_kind           (gfl::grp::SHADOW_KIND_NONE),
        mipmap_size           (1),
        color_transfer        (gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        executing_memory_fill (false),
        is_dynamic_allocation (false),
        is_perspective_shadow (true),
        shadow_z_bias         (0.0f),
        shadow_z_scale        (1.0f),
        display_type          (gfl::grp::DISPLAY_NONE),
        fob_id                (0)
    {}

  };


  //----------------------------------------------------------------------------
  /**
   *	@brief  OnScreenRenderTarget�pDescription�̏�����
   *
   *	�ʏ�̃����_�[�^�[�Q�b�g�ł��B
   *
   *	�J���[�o�b�t�@����B
   *	�f�v�X�o�b�t�@����B
   *
   *  @param  p_desc                �f�[�^�ݒ��
   *	@param	width                 ��
   *	@param	height                ����
   *	@param	color_format          �J���[�t�H�[�}�b�g
   *	@param	depth_format          �f�v�X�t�H�[�}�b�g
   *	@param	color_area            �J���[�o�b�t�@�̃������z�u�ꏊ
   *	@param	depth_area            �f�v�X�o�b�t�@�̃������z�u�ꏊ
   *	@param	display_type          �f�B�X�v���C�^�C�v
   */
  //-----------------------------------------------------------------------------
  static void InitializeOnScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type );



  //----------------------------------------------------------------------------
  /**
   *	@brief  OffScreenRenderTarget�p��Description��������
   *
   * �J���[�o�b�t�@���e�N�X�`���Ƃ��Ďg�p���邱�Ƃ��ł���^�C�v�̃����_�[�^�[�Q�b�g�ł��B
   * �f�v�X�o�b�t�@�͂���܂���B
   *
   *	�J���[�o�b�t�@��ʏ�e�N�X�`���Ƃ��ėp��
   *	�f�v�X�o�b�t�@�Ȃ��B
   *
   *  @param  p_desc                �f�[�^�ݒ��
   *	@param	width                 ��
   *	@param	height                ����
   *	@param	color_format          �J���[�t�H�[�}�b�g
   *	@param	color_area            �J���[�o�b�t�@�������G���A
   *	@param	display_type          �f�B�X�v���C�^�C�v
   *  @param  mipmap_size           �~�b�v�}�b�v�T�C�Y
   *	@param	color_transfer        �e�N�X�`���̃������]���^�C�v
   *	@param  executing_memory_fill �m�ۂ����s�N�Z����0�ŃN���A���邩�H ����ꍇtrue
   *	@param  is_dynamic_allocation ���I��FCRAM��ɗ̈���m�ۂ��邩�H ����ꍇtrue
   */
  //-----------------------------------------------------------------------------
  static void InitializeOffScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::MemoryArea color_area, gfl::grp::DisplayType display_type, s32 mipmap_size = 1, gfl::grp::MemoryTransfer color_transfer = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP, b32 executing_memory_fill = false, b32 is_dynamic_allocation = false );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  OffScreenDepthRenderTarget�p��Description��������
   *
   * �J���[�o�b�t�@���e�N�X�`���Ƃ��Ďg�p���邱�Ƃ��ł���^�C�v�̃����_�[�^�[�Q�b�g�ł��B
   * �f�v�X�o�b�t�@���p�ӂ��܂��B
   *
   *	�J���[�o�b�t�@��ʏ�e�N�X�`���Ƃ��ėp��
   *	�f�v�X�o�b�t�@����B
   *
   *  @param  p_desc                �f�[�^�ݒ��
   *	@param	width                 ��
   *	@param	height                ����
   *	@param	color_format          �J���[�t�H�[�}�b�g
   *	@param	depth_format          �f�v�X�t�H�[�}�b�g
   *	@param	color_area            �J���[�o�b�t�@�������G���A
   *	@param	depth_area            �f�v�X�o�b�t�@�������G���A
   *	@param	display_type          �f�B�X�v���C�^�C�v
   *  @param  mipmap_size           �~�b�v�}�b�v�T�C�Y
   *	@param	color_transfer        �e�N�X�`���̃������]���^�C�v
   *	@param  executing_memory_fill �m�ۂ����s�N�Z����0�ŃN���A���邩�H ����ꍇtrue
   *	@param  is_dynamic_allocation ���I��FCRAM��ɗ̈���m�ۂ��邩�H ����ꍇtrue
   */
  //-----------------------------------------------------------------------------
  static void InitializeOffScreenDepthDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type, s32 mipmap_size = 1, gfl::grp::MemoryTransfer color_transfer = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP, b32 executing_memory_fill = false, b32 is_dynamic_allocation = false );

  //----------------------------------------------------------------------------
  /**
   *	@brief  DirectAddressRenderTarget�p��Description��������
   *
   * �J���[�o�b�t�@�ƃf�v�X�o�b�t�@�̃A�h���X���w�肵�āA����������^�C�v�̃����_�[�^�[�Q�b�g�ł��B
   *
   *	�J���[�o�b�t�@����B
   *	�f�v�X�o�b�t�@����B
   *
   *  @param  p_desc                �f�[�^�ݒ��
   *	@param	width                 ��
   *	@param	height                ����
   *	@param	color_format          �J���[�t�H�[�}�b�g
   *	@param	depth_format          �f�v�X�t�H�[�}�b�g
   *	@param	color_address         �J���[�o�b�t�@�A�h���X
   *	@param	depth_address         �f�v�X�o�b�t�@�A�h���X
   *	@param	display_type          �f�B�X�v���C�^�C�v
   *	@param	fob_id                �m�ۍς�GL�p�t���[���o�b�t�@�I�u�W�F�N�g�i�Ȃ��ꍇ�ɂ�0������Ă��������j
   */
  //-----------------------------------------------------------------------------
  static void InitializeDirectAddressDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, gfl::grp::DisplayType display_type, GLuint fob_id );


  //----------------------------------------------------------------------------
  /**
   *	@brief  Description�ɃV���h�E�e�N�X�`���Ɋւ��镔���̐ݒ���s���B
   *	        ���K���AInitializeOffScreen�`Description�֐���ɂ��ł��������B
   *
   *	@param	p_desc                �f�[�^�ݒ��
   *	@param  shadow_kind           �V���h�E�̎��
   *	@param  is_perspective_shadow ���e�V���h�E�̏ꍇ��true
   *	@param  shadow_z_bias         �V���h�E�e�N�X�`����Z�o�C�A�X�l
   *	@param  shadow_z_scale        �I�t�Z�b�g�l�ɏ�Z����X�P�[��
   */
  //-----------------------------------------------------------------------------
  static void SetShadowTextureDescription( Description* p_desc, gfl::grp::ShadowType shadow_kind, b32 is_perspective_shadow, f32 shadow_z_bias, f32 shadow_z_scale );



public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create�͌Ă΂�܂���B
   */
  //-----------------------------------------------------------------------------
  RenderTarget(void);
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 Create���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );
  RenderTarget( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL);
  // @note gfl::grp::g3d::Texture��n���֐�����������p�~�\��
  RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description = NULL );
  // @note allocator, depth_buffer, description �������Ɏ��֐��쐬�\��
  // @note allocator, nw_res_texture, depth_buffer, description �������Ɏ��֐��쐬�\��

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   *                 Destroy���Ăт܂��B
   */
  //-----------------------------------------------------------------------------
  virtual ~RenderTarget();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����
   *  OffScreenBuffer�́A�����Ńe�N�X�`���̈���m�ۂ���B
   *  OnScreenBuffer�́ACreateOnScreenBuffer�Ɠ����̌��ʂɂȂ�B
   */
  //-----------------------------------------------------------------------------
  void Create( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I���X�N���[���o�b�t�@�̐���
   */
  //-----------------------------------------------------------------------------
  void CreateOnScreenBuffer( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �I�t�X�N���[���o�b�t�@�̐���
   */
  //-----------------------------------------------------------------------------
  // @note gfl::grp::g3d::Texture��n���֐�����������p�~�\��
  void CreateOffScreenBuffer( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description = NULL );
  // @note allocator, depth_buffer, description �������Ɏ��֐��쐬�\��
  // @note allocator, nw_res_texture, depth_buffer, description �������Ɏ��֐��쐬�\��

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �j��
   *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����ς݂��ۂ�
   *
   *  @retval        b32  �����ς݂̂Ƃ�true�A��������Ă��Ȃ��Ƃ�false
   */
  //-----------------------------------------------------------------------------
  b32 IsCreated(void) const  {  return (m_buffer_type != TYPE_NONE);  } 


public:
  // @note gfl::grp::g3d::Texture��Ԃ��֐�����������p�~�\��
  nw::gfx::res::ResTexture  GetNwResTexture(void) const;  // �o�b�t�@�����̃N���X�ɂ��点���ꍇ�͏c��������ւ�����G�������܂��B

public:
  // nw::gfx::FrameBufferObject�����̋@�\
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[���o�b�t�@�̐ݒ�p�R�}���h��ݒ肷��
   *
   *  @param  graphics      �O���t�B�b�N�X
   */
  //-----------------------------------------------------------------------------
  void ActivateBuffer( void ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t���[���o�b�t�@�̃N���A�����s����
   *                 �N���A�R�}���h��ς݂����R�}���h���X�g���o�C���h���Ă���g�p���ĉ������B
   *                 GraphicsSystem�ɂ���N���A�֐����g�p���邱�Ƃ𐄏����܂��B
   */
  //-----------------------------------------------------------------------------
  void ClearBuffer(
      b32 b_color,
      const gfl::grp::ColorF32& clear_color,
      b32 b_depth,
      f32 clear_depth,
      b32 b_stencil = false,
      u8 clear_stencil = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̏����������ɏo��
   *
   *  @param  graphics  �O���t�B�b�N�X
   *	@param	type      �o�b�t�@�^�C�v
   *	@param  memory    �o�͐�A�h���X
   *
   *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
   *  �o�b�t�@�����̃N���X�ɂ��点���ꍇ�͏c��������ւ�����G�������܂��B
   */
  //-----------------------------------------------------------------------------
  void OutputImage( RenderBufferType type, void* memory );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̏����������Ƀ��j�A�o��
   *
   *	@param	type      �o�b�t�@�^�C�v
   *	@param  memory    �o�͐�A�h���X
   */
  //-----------------------------------------------------------------------------
  void OutputB2LImage( RenderBufferType type, void* memory );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �J���[�o�b�t�@�ɏ����R�s�[�i�u���b�N���[�h�j
   *
   *	@param  memory  ���̓A�h���X
   */
  //-----------------------------------------------------------------------------
  void InputImage( const void* memory );

#if GFL_HIO_ENABLE
	//----------------------------------------------------------------------------
  /**
   *	@brief  �t���[���o�b�t�@�̏����t�@�C���ɏo��
   *
   *  @param  graphics  �O���t�B�b�N�X
	 *	@param	file_path    �o�̓p�X
   *	@param	type		�o�b�t�@�^�C�v
   *	@param  allocator  ���[�N�̊m�ې�
   *
   *	*�����ŃJ�����g�o�b�t�@��ύX���܂��B
   *  �o�b�t�@�����̃N���X�ɂ��点���ꍇ�͏c��������ւ�����G�������܂��B
   */
  //-----------------------------------------------------------------------------
  void OutputImageFile( const char* file_path, RenderBufferType type, gfl::heap::NwAllocator* allocator );
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         gl�̃t���[���o�b�t�@�I�u�W�F�N�g��ID���擾����
   */
  //-----------------------------------------------------------------------------
  GLuint GetFboID(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief        �J���[�o�b�t�@�̃A�h���X���擾
   *	@return       �J���[�o�b�t�@�̃A�h���X
   */
  //-----------------------------------------------------------------------------
  u32 GetColorAddress( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        �f�v�X�o�b�t�@�̃A�h���X���擾
   *	@return       �f�v�X�o�b�t�@�̃A�h���X
   */
  //-----------------------------------------------------------------------------
  u32 GetDepthAddress( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        ���̎擾
   *	@return       ��
   */
  //-----------------------------------------------------------------------------
  s32 GetWidth( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        �����̎擾
   *	@return       ����
   */
  //-----------------------------------------------------------------------------
  s32 GetHeight( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief      �f�B�X�v���C�^�C�v�̎擾
   *
   *	@return     �f�B�X�v���C�^�C�v
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::DisplayType GetDisplayType(void) const { return m_display_type; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      �f�B�X�v���C�^�C�v�̎擾
   *
   *	@param   display_type  �f�B�X�v���C�^�C�v
   */
  //-----------------------------------------------------------------------------
  inline void SetDisplayType(const gfl::grp::DisplayType display_type) { m_display_type = display_type; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      �f�v�X�o�b�t�@�t�H�[�}�b�g�̎擾
   *
   *	@return     �f�v�X�o�b�t�@�t�H�[�}�b�g
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::RenderDepthFormat GetRenderDepthFormat(void) const { return m_depth_format; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      �J���[�o�b�t�@�t�H�[�}�b�g�̎擾
   *
   *	@return     �J���[�o�b�t�@�t�H�[�}�b�g
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::RenderColorFormat GetRenderColorFormat(void) const { return m_color_format; }


  //----------------------------------------------------------------------------
  /**
   *	@brief      �r���[�|�[�g���̎擾
   *
   *  @param[out]  left      �X�N���[���@�����W
   *  @param[out]  top       �X�N���[���@����W
   *  @param[out]  right     �X�N���[���@�E���W
   *  @param[out]  bottom    �X�N���[���@�����W
   */
  //-----------------------------------------------------------------------------
  void GetViewport( f32* left, f32* top, f32* right, f32* bottom ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief      �r���[�|�[�g���̐ݒ�
   *
   *  @param  left      �X�N���[���@�����W
   *  @param  top       �X�N���[���@����W
   *  @param  right     �X�N���[���@�E���W
   *  @param  bottom    �X�N���[���@�����W
   */
  //-----------------------------------------------------------------------------
  void SetViewport( f32 left, f32 top, f32 right, f32 bottom );
  
  //-----------------------------------------------------------------------------
  /**
   * �f�v�X�o�b�t�@�̃������z�u�ꏊ�擾
   * @return �f�v�X�o�b�t�@�̃������z�u�ꏊ���e
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::MemoryArea GetDepthMemoryArea(){ return m_depth_area; }
  
  //-----------------------------------------------------------------------------
  /**
   * �s�N�Z���t�H�[�}�b�g�ϊ��֐�
   * @param p_data      �ϊ��������f�[�^
   * @param format      ���̓t�H�[�}�b�g
   * @param w           �� 
   * @param h           ����
   */
  //-----------------------------------------------------------------------------
  static void ConvertPixelFormat( char* p_data, GLenum format, int w, int h );


private:
  // friend�N���X�ɂ����A�N�Z�X�����������o�֐�
  nw::gfx::IRenderTarget*  GetNwRenderTarget(void) const  { return m_nw_render_target; }
  const nw::gfx::Viewport& GetNwViewport(void) const  { return m_view_port.GetNwViewport(); };  // �o�b�t�@�����̃N���X�ɂ��点���ꍇ�͏c��������ւ�����G�������܂��B
  
private:
  // ���̃N���X���炵���A�N�Z�X���Ȃ������o�֐�
	
  void SetDescriptionViewport( const Description* description, Viewport* viewport );

private:
  // @note depth_buffer_type ���K�v���H

  
  enum TextureOwnerType
  {
    TEXTURE_OWNER_NONE = 0,     // �e�N�X�`�����Ȃ�
    TEXTURE_OWNER_THIS,         // �N���X���Ő��������e�N�X�`�����g�p
    TEXTURE_OWNER_USER,         // ���[�U�[�����������e�N�X�`�����g�p
  } ;

private:

  Type                          m_buffer_type;        // �o�b�t�@�^�C�v
  DisplayType                   m_display_type;       // �`��ʃ^�C�v
  TextureOwnerType              m_texture_owner;      // �e�N�X�`��Owner�^�C�v
  grp::RenderDepthFormat        m_depth_format;       // �f�v�X�t�H�[�}�b�g
  grp::RenderColorFormat        m_color_format;       // �J���[�t�H�[�}�b�g
  gfl::grp::MemoryArea          m_depth_area;         // �f�v�X�o�b�t�@�̃������z�u�ꏊ

  nw::gfx::IRenderTarget*   m_nw_render_target;  // NULL�̂Ƃ�Create����Ă��Ȃ�

  // m_buffer_type��BUFFER_TYPE_OFF_SCREEN_BUFFRE_OWN_TEXTURE or BUFFER_TYPE_OFF_SCREEN_BUFFRE_USER_TEXTURE �̂Ƃ��L��
  nw::gfx::res::ResTexture* m_nw_res_texture;


  Viewport  m_view_port;  // �r���[�|�[�g��� 

public:
  // @note �ڍs���ɂ����g�p��������֐�(�����ɔp�~�\��)
  void SetNwRenderTarget(nw::gfx::IRenderTarget* nw_render_target)  { m_nw_render_target = nw_render_target; }
};

}  // namespace grp
}  // namespace gfl

#endif // __GFL_RENDERTARGET_H__
