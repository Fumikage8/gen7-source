#ifndef __GFL_GRAPHICSTYPE_H__
#define __GFL_GRAPHICSTYPE_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsType.h
 *	@brief  �O���t�B�b�N�X���@���ʌ^�A�萔��`
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <nw/ut.h>

#include <gfl_Macros.h>

namespace gfl {
namespace grp {

//-----------------------------------------------------------------------------
/**
 *					�f�o�b�O�p
*/
//-----------------------------------------------------------------------------
// OPENGL�n�R�}���h�̃G���[�`�F�b�N
#if GFL_DEBUG
  
#define GFL_GRPGL_ERROR_CHECK() {\
  GLenum error = glGetError();\
  GFL_ASSERT_MSG( error == GL_NO_ERROR, "GLError code = 0x%x", error );\
}
  
#else
  
#define GFL_GRPGL_ERROR_CHECK() ((void)0)
  
#endif
  

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
// �������^�C�v
enum MemoryArea
{
  MEMORY_AREA_NONE,
  MEMORY_AREA_FCRAM = NN_GX_MEM_FCRAM,                      //!< FCRAM�̗̈��\���܂��B
  MEMORY_AREA_VRAMA = NN_GX_MEM_VRAMA,                      //!< VRAM��A�`�����l���̗̈��\���܂��B
  MEMORY_AREA_VRAMB = NN_GX_MEM_VRAMB,                      //!< VRAM��B�`�����l���̗̈��\���܂��B
  ENUM_FORCE_DWORD(MemoryArea)
} ;
 
// �������]���^�C�v
enum MemoryTransfer
{
  MEMORY_TRANSFER_COPY_FCRAM_DMP      = GL_COPY_FCRAM_DMP,    //< �f�[�^�̃R�s�[�����C���������ɐ����B
  MEMORY_TRANSFER_NO_COPY_FCRAM_DMP   = GL_NO_COPY_FCRAM_DMP, //< �f�[�^�̃R�s�[�����C���������ɐ������Ȃ��B
  ENUM_FORCE_DWORD(MemoryTransfer)
};

// �������g�p�p�r
typedef enum
{
  MEM_SYSTEM        = NN_GX_MEM_SYSTEM,               // �V�X�e���Ɏg�p�B
  MEM_TEXTURE       = NN_GX_MEM_TEXTURE,              // �e�N�X�`���Ɏg�p�B
  MEM_VERTEXBUFFER  = NN_GX_MEM_VERTEXBUFFER,         // ���_�o�b�t�@�Ɏg�p�B
  MEM_DISPLAYBUFFER = NN_GX_MEM_DISPLAYBUFFER,        // �f�B�X�v���C�o�b�t�@�Ɏg�p�B
  MEM_COMMANDBUFFER = NN_GX_MEM_COMMANDBUFFER,        // �R�}���h�o�b�t�@�Ɏg�p�B
  MEM_RENDERBUFFER  = NN_GX_MEM_RENDERBUFFER,         // �����_�[�o�b�t�@�Ɏg�p�B


  ENUM_FORCE_DWORD(MemoryUseType)
} MemoryUseType;

// �\���ʒ�`
typedef enum
{
  DISPLAY_UPPER = 0,                      // ���ʁ@or �����
  DISPLAY_LOWER,                          // �����
  DISPLAY_UPPER_RIGHT,                    // �E���

  DISPLAY_BOTH_MAX,                       // �S��ʂ̐�
  DISPLAY_NONE,                           // �Ή������ʂȂ��B 

  DISPLAY_NORMAL_MAX = DISPLAY_UPPER_RIGHT, // ���̎��Ȃ��̉�ʐ��B


  ENUM_FORCE_DWORD(DisplayType)
} DisplayType;
  

//-------------------------------------
///	�f�B�X�v���C�@���
//=====================================
typedef enum{
  DISPLAY_MSK_UPPER       = 1<<DISPLAY_UPPER,       // ����/���ʍ�
  DISPLAY_MSK_LOWER       = 1<<DISPLAY_LOWER,       // �����
  DISPLAY_MSK_UPPER_RIGHT = 1<<DISPLAY_UPPER_RIGHT, // �E���

  DISPLAY_MSK_NORMAL  = DISPLAY_MSK_UPPER | DISPLAY_MSK_LOWER,                      // �ʏ펞
  DISPLAY_MSK_BOTH    = DISPLAY_MSK_UPPER | DISPLAY_MSK_UPPER_RIGHT | DISPLAY_MSK_LOWER, // ���̎���

  DISPLAY_MSK_STEREO  = DISPLAY_MSK_UPPER | DISPLAY_MSK_UPPER_RIGHT, // ���̎�����

  ENUM_FORCE_DWORD(DisplayMsk)
} DisplayMsk;


//-------------------------------------
///	�����_���[�J���[�t�H�[�}�b�g
//=====================================
enum RenderColorFormat
{
  RENDER_COLOR_FORMAT_NONE = 0,
  RENDER_COLOR_FORMAT_RGBA8 = GL_RGBA8_OES,
  RENDER_COLOR_FORMAT_RGB8 = GL_RGB8_OES,
  RENDER_COLOR_FORMAT_RGBA4 = GL_RGBA4,
  RENDER_COLOR_FORMAT_RGB5_A1 = GL_RGB5_A1,
  RENDER_COLOR_FORMAT_RGB565 = GL_RGB565,

  RENDER_COLOR_FORMAT_COUNT = 6,

  ENUM_FORCE_DWORD(RenderColorFormat)
};

//-------------------------------------
///	�����_���[ �[�x�o�b�t�@�t�H�[�}�b�g
//=====================================
enum RenderDepthFormat
{
  RENDER_DEPTH_FORMAT_NONE = 0,
  RENDER_DEPTH_FORMAT_16 = GL_DEPTH_COMPONENT16,
  RENDER_DEPTH_FORMAT_24 = GL_DEPTH_COMPONENT24_OES,
  RENDER_DEPTH_FORMAT_24_STENCIL8 = GL_DEPTH24_STENCIL8_EXT,

  RENDER_DEPTH_FORMAT_COUNT = 4,

  ENUM_FORCE_DWORD(RenderDepthFormat)
};

// �V���h�E�̎�ʂł��B
enum ShadowType
{
  SHADOW_KIND_NONE = 0,
  SHADOW_KIND_TEXTURE,
  SHADOW_KIND_CUBE,

  ENUM_FORCE_DWORD(ShadowType)
};

//-----------------------------------------------------------------------------
/**
 *        �u���b�N�C���[�W�T�C�Y
*/
//-----------------------------------------------------------------------------
static const s32 DEFAULT_BLOCK_IMAGE_SIZE = (8);

//-----------------------------------------------------------------------------
/**
 *        ��ʃT�C�Y��`
*/
//-----------------------------------------------------------------------------
#define   DISPLAY_UPPER_WIDTH   (400)
#define   DISPLAY_UPPER_HEIGHT  (240)
#define   DISPLAY_LOWER_WIDTH   (320)
#define   DISPLAY_LOWER_HEIGHT  (240)


//-----------------------------------------------------------------------------
/**
 *					�^�錾
*/
//-----------------------------------------------------------------------------
typedef nw::ut::Color8      ColorU8;   // u8�̂Ƃ���0����&�����A255����&�s����
typedef nw::ut::FloatColor  ColorF32;  // f32�̂Ƃ���0.0f����&�����A1.0f����&�s����

extern ColorF32 ConvertColorU8ToColorF32(const ColorU8& c);
extern ColorU8  ConvertColorF32ToColorU8(const ColorF32& c);

// ���`��Ԃ��v�Z����
extern ColorF32* ColorF32Lerp(    // pOut��Ԃ�
    ColorF32*       pOut,         // �v�Z���ʂ��󂯎��Bp1, p2�Ɠ����ł��\��Ȃ��B
    const ColorF32* p1,
    const ColorF32* p2,
    f32             t             // 0.0f�ł����p1���A1.0f�ł����p2�����ʂƂȂ�B
);

// �X�J���[�{���v�Z����
extern ColorF32* ColorF32Scale(    // pOut��Ԃ�
    ColorF32*       pOut,          // �v�Z���ʂ��󂯎��Bp�Ɠ����ł��\��Ȃ��B
    const ColorF32* p,
    f32             scale          // �|���鐔
);

} /* end namespace grp */
} /* end namespace gfl */
#endif // __GFL_GRAPHICSTYPE_H__
