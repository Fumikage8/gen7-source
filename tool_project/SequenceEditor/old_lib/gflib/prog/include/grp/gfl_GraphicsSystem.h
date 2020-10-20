//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_GraphicsSystem.h
 *	@brief  �O���t�B�b�N�X�V�X�e��    �`����\�z�E�Ǘ�
 *	@author	tomoya takahashi
 *	@date		2010.10.22
 *	@date		2011.03.23    VRAM�̈�ߖ�̂��߁A�t���[���o�b�t�@�P�Ŋe��ʗp�̕`���
 *	�@�@�@�@�@�@�@�@�@�@�@�s����V�X�e���ɕύX�J�n
 *
 *
 *	VRAM�̈�ߖ�̂��߂̍\��
 *	- �`���t���[���o�b�t�@���Ƃ̃R�}���h��ʁX�̃R�}���h�o�b�t�@�Ɋi�[����B
 *	  -#���ډE�ڂ̕`�揈����1�̃t���[���o�b�t�@�ōs���ꍇ�A�J���[��[�x����
 *	  �@�������݂����d�ɍs���Ȃ��悤�A���ڗp�̂RD�@�QD���ׂĂ̕`���Ɂ@
 *	  �@�E�ڗp�RD�@�QD���ׂĂ̕`����s���悤�ɏ����ǂ��R�}���h���o�b�t�@�Ɋi�[���Ă����K�v������܂��B
 *
 *	�@-#�������A�RD�`��A�QD�`��ȂǁA�ʁX�̕`��V�X�e�����g�p���A�����_�����O���Ă����ۂɁA
 *	�@�@���ځA�E�ڕʁX�ɕ`�揈�����s�����Ƃ́A�V�X�e���̕��G���ɂȂ���A�ȒP�Ɍ����Ɩʓ|�Ȑ���ɂȂ��Ă��܂��B
 *
 *	�@-#�����ŁAGraphicsSystem�����ɁA1�̃t���[���o�b�t�@�ɑ΂���A���E����ʗp�̃R�}���h��ʁX�Ɋi�[����
 *	�@�@�R�}���h�o�b�t�@��ʁX�ɗp�ӂ��A�AEndRendering�̃^�C�~���O�ŁA
 *	�@�@�����ʁX�̃R�}���h����1�ɂ܂Ƃ߂āA�����_�����O�����s����V�X�e����p�ӂ��܂����B
 *
 *	�@-#���X�킩��ɂ����\���ɂȂ�܂����A���������������B
 *
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifndef __GFL_GRAPHICSSYSTEM_H__
#define __GFL_GRAPHICSSYSTEM_H__
#pragma once


#include <nn.h>
#include <nw.h>

#include <base/gfl_Singleton.h>

#include <debug/gfl_DebugPrint.h>


#include <gfl_Heap.h>
#include <gfl_Macros.h>

#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_DisplayBufferManager.h>
#include <grp/gfl_FrameBufferManager.h>
#include <grp/gfl_CommandListManager.h>

namespace gfl {
namespace grp {






//-----------------------------------------------------------------------------
/**
 *					�f�o�b�O�o��
*/
//-----------------------------------------------------------------------------
#define GFL_DEBUG_PRINT_GRAPHICSSYSTEM (gfl::debug::PRINT_FLAG_LEVEL1 | gfl::debug::PRINT_FLAG_CATEGORY_G3D)

// �_�u���o�b�t�@�����O�Ď�
#define GFL_DEBUG_PRINT_GRAPHICSSYSTEM_HIGH (gfl::debug::PRINT_FLAG_LEVEL4 | gfl::debug::PRINT_FLAG_CATEGORY_G3D)

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	���샂�[�h
//=====================================
typedef enum {

  MODE_DOUBLE_BUFFER,   // �_�u���o�b�t�@�����O���[�h�@�i�����j
  MODE_SINGLE_BUFFER,   // �V���O���o�b�t�@�����O���[�h

  MODE_ENUM_END,


  ENUM_FORCE_DWORD(GraphicsSystemBufferMode)

} GraphicsSystemBufferMode;


// �R�}���h���X�g�^�C�v
enum CommandListType
{
  COMMANDLIST_MAIN = 0,     // ���C���R�}���h���X�g �iOffScreen�̏����j
  COMMANDLIST_LEFT,         // ���ڗp�R�}���h���X�g (���ڗp����)
  COMMANDLIST_RIGHT,        // �E�ڗp�R�}���h���X�g (�E�ڗp����)
  COMMANDLIST_BOTTOM,       // ����ʗp�R�}���h���X�g (����ʗp�����j

  COMMANDLIST_NUM,          // �R�}���h���X�g��
};

enum{
  // �V�X�e�����ő�ŕێ�����o�b�t�@��
  GRAPHICSSYSTEM_USE_DISPLAYBUFFER_NUM = 6,   // �f�B�X�v���C�o�b�t�@
  GRAPHICSSYSTEM_USE_FRAMEBUFFER_NUM = DISPLAY_BOTH_MAX,     // �t���[���o�b�t�@
  GRAPHICSSYSTEM_USE_COMMANDLIST_NUM = COMMANDLIST_NUM*2, // �R�}���h���X�g
  GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM = 2, // �Z�[�u�p�R�}���h���X�g

  GRAPHICSSYSTEM_USER_WORK = 8, // ���[�U�[�����R�Ɏg����R�}���h���X�g��

  GRAPHICSSYSTEM_TOTAL_COMMANDLIST_MAX = GRAPHICSSYSTEM_USE_COMMANDLIST_NUM + GRAPHICSSYSTEM_USE_SAVECOMMANDLIST_NUM + GRAPHICSSYSTEM_USER_WORK,
};

// �V�X�e������������^�C�v
enum SystemMemoryMode{
  SYSTEM_MEM_UNSETTLED,       // �V�X�e���e�������̑O������m�� (�ϓ��̂��郁�����j
  SYSTEM_MEM_FIX,             // �V�X�e���e�������̌������m�ہi�ϓ��̂Ȃ��������j
};

// GPUProfileRESULT
enum GpuProfileResult{
  GPU_PROFILE_BUSY_MICRO_SECOND = 1,      // ���̃}�C�N���b�P�ʂɃr�W�[�`�F�b�N����

  // BUSY
  GPU_PROFILE_BUSY_SHADER0 = 0,           // �V�F�[�_�[�v���Z�b�T0
  GPU_PROFILE_BUSY_COMMAND_VERTEX_LOAD,   // �R�}���h�o�b�t�@����ђ��_�A���C���[�h���W���[��
  GPU_PROFILE_BUSY_RASTERIZE,             // ���X�^���C�Y
  GPU_PROFILE_BUSY_TRIANGLE_SETUP,        // �g���C�A���O���Z�b�g�A�b�v
  GPU_PROFILE_BUSY_FRAGMENT,              // �t���O�����g���C�e�B���O
  GPU_PROFILE_BUSY_TEXTURE_UNIT,          // �e�N�X�`�����j�b�g
  GPU_PROFILE_BUSY_PERFRAGMENT,           // �p�[�t���O�����g�I�y���[�V����
  GPU_PROFILE_BUSY_TEXTURE_COMB,          // �e�N�X�`���R���o�C�i

  // VERTEX_SHADER_0
  GPU_PROFILE_VERTEX0_PROGCOUNTER,        // ���_�V�F�[�_�[�O�@�v���O�����J�E���^
  GPU_PROFILE_VERTEX0_CMD_STOLE,          // ���_�V�F�[�_�[�O�@���߂̈ˑ��֌W�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX0_MOVA_CMD_STOLE,     // ���_�V�F�[�_�[�O�@�A�h���X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX0_CMP_CMD_STOLE,      // ���_�V�F�[�_�[�O�@�X�e�[�^�X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX0_FETCH_MISS_STOLE,   // ���_�V�F�[�_�[�O�@�v���t�F�b�`�̃~�X�ɂ��X�g�[�������N���b�N��

  // VERTEX_SHADER_1
  GPU_PROFILE_VERTEX1_PROGCOUNTER,        // ���_�V�F�[�_�[�O�@�v���O�����J�E���^
  GPU_PROFILE_VERTEX1_CMD_STOLE,          // ���_�V�F�[�_�[�O�@���߂̈ˑ��֌W�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX1_MOVA_CMD_STOLE,     // ���_�V�F�[�_�[�O�@�A�h���X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX1_CMP_CMD_STOLE,      // ���_�V�F�[�_�[�O�@�X�e�[�^�X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX1_FETCH_MISS_STOLE,   // ���_�V�F�[�_�[�O�@�v���t�F�b�`�̃~�X�ɂ��X�g�[�������N���b�N��

  // VERTEX_SHADER_2
  GPU_PROFILE_VERTEX2_PROGCOUNTER,        // ���_�V�F�[�_�[�O�@�v���O�����J�E���^
  GPU_PROFILE_VERTEX2_CMD_STOLE,          // ���_�V�F�[�_�[�O�@���߂̈ˑ��֌W�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX2_MOVA_CMD_STOLE,     // ���_�V�F�[�_�[�O�@�A�h���X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX2_CMP_CMD_STOLE,      // ���_�V�F�[�_�[�O�@�X�e�[�^�X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX2_FETCH_MISS_STOLE,   // ���_�V�F�[�_�[�O�@�v���t�F�b�`�̃~�X�ɂ��X�g�[�������N���b�N��

  // VERTEX_SHADER_3
  GPU_PROFILE_VERTEX3_PROGCOUNTER,        // ���_�V�F�[�_�[�O�@�v���O�����J�E���^
  GPU_PROFILE_VERTEX3_CMD_STOLE,          // ���_�V�F�[�_�[�O�@���߂̈ˑ��֌W�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX3_MOVA_CMD_STOLE,     // ���_�V�F�[�_�[�O�@�A�h���X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX3_CMP_CMD_STOLE,      // ���_�V�F�[�_�[�O�@�X�e�[�^�X���W�X�^�̍X�V�ɂ��X�g�[�������N���b�N��
  GPU_PROFILE_VERTEX3_FETCH_MISS_STOLE,   // ���_�V�F�[�_�[�O�@�v���t�F�b�`�̃~�X�ɂ��X�g�[�������N���b�N��

  // POLYGON
  GPU_PROFILE_POLYGON_INPUT_VERTEX,       // �g���C�A���O���Z�b�g�A�b�v�ւ̓��͒��_��
  GPU_PROFILE_POLYGON_INPUT_POLYGON,      // �g���C�A���O���Z�b�g�A�b�v�ւ̓��̓|���S����
  GPU_PROFILE_POLYGON_OUTPUT_POLYGON,     // �g���C�A���O���Z�b�g�A�b�v�ւ̏o�̓|���S����

  // FRAGMENT
  GPU_PROFILE_FRAGMENT_INPUT_FRAGMENT,    // ���̓t���O�����g���J�E���^�̒l

  // �������A�N�Z�X�J�E���g�i�o�C�g���j
  GPU_PROFILE_ACCESS_VRAMA_READ,                 // PICA �ɂ�� VRAM A �`���l���̃��[�h
  GPU_PROFILE_ACCESS_VRAMA_WRITE,                // PICA �ɂ�� VRAM A �`���l���̃��C�g
  GPU_PROFILE_ACCESS_VRAMB_READ,                 // PICA �ɂ�� VRAM B �`���l���̃��[�h
  GPU_PROFILE_ACCESS_VRAMB_WRITE,                // PICA �ɂ�� VRAM B �`���l���̃��C�g
  GPU_PROFILE_ACCESS_CMD_VERTEX,                 // �R�}���h�o�b�t�@�A���_�A���C�A�C���f�b�N�X�A���C�̃��[�h
  GPU_PROFILE_ACCESS_TEX_UNIT,                   // �e�N�X�`�����j�b�g�ɂ��@�e�N�X�`�����������[�h
  GPU_PROFILE_ACCESS_DEPTH_READ,                 // �t���O�����g�I�y���[�V�����@�f�v�X�o�b�t�@�@���[�h
  GPU_PROFILE_ACCESS_DEPTH_WRITE,                // �t���O�����g�I�y���[�V�����@�f�v�X�o�b�t�@�@���C�g
  GPU_PROFILE_ACCESS_COLOR_READ,                 // �t���O�����g�I�y���[�V�����@�J���[�o�b�t�@�@���[�h
  GPU_PROFILE_ACCESS_COLOR_WRITE,                // �t���O�����g�I�y���[�V�����@�J���[�o�b�t�@�@���C�g
  GPU_PROFILE_ACCESS_LCD_UP_DISP_READ,           // ����LCD�ɂ��@�f�B�X�v���C�o�b�t�@�@���[�h
  GPU_PROFILE_ACCESS_LCD_DOWN_DISP_READ,         // �����LCD�ɂ��@�f�B�X�v���C�o�b�t�@�@���C�g
  GPU_PROFILE_ACCESS_POST_READ,                  // �|�X�g�]�����W���[���ɂ�郊�[�h
  GPU_PROFILE_ACCESS_POST_WRITE,                 // �|�X�g�]�����W���[���ɂ�郉�C�g
  GPU_PROFILE_ACCESS_MEMFILL0_WRITE,             // �������t�B�����[�W���[���̃`���l���O�ɂ��o�b�t�@�̃��C�g
  GPU_PROFILE_ACCESS_MEMFILL1_WRITE,             // �������t�B�����[�W���[���̃`���l���P�ɂ��o�b�t�@�̃��C�g
  GPU_PROFILE_ACCESS_READPIXCEL,                // glReadPixels�Ȃǂɂ��VRAM���[�h
  GPU_PROFILE_ACCESS_DMA_VRAM_WRITE,            // DMA�]���ɂ��AVRAM���C�g

  GPU_PROFILE_RESULT_MAX,     // �ő�l
};


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�R�}���h���X�g�Z�b�g�A�b�v�@���ޯ����`
//=====================================
enum CommandListSetUpIndex{

  COMMANDLIST_SETUP_MAIN,     // �I�t�X�N���[���p�������i�[�����R�}���h�o�b�t�@�ݒ�
  COMMANDLIST_SETUP_UPPER,    // ���ʗp�������i�[�����R�}���h�o�b�t�@�ݒ�
  COMMANDLIST_SETUP_LOWER,    // ����ʗp�������i�[�����R�}���h�o�b�t�@�ݒ�

  COMMANDLIST_SETUP_NUM,
} ;

//-------------------------------------
///	�O���t�B�b�N�V�X�e���@�Z�b�g�A�b�v
//
//Initialize���s���ƁA�V�X�e�����Ŏ����ňȉ��̃o�b�t�@���m�ۂ��܂��B
//  ��buffer_mode��MODE_DOUBLE_BUFFER�ɂ����ꍇ
//    displayBuffer�𗧑̎�����̏ꍇ6�@�ʏ�̏ꍇ4��
//    commandList��8��
//    Save�pcommandList��2��
//    �m�ۂ��܂��B
//
//
//  ��buffer_mode��MODE_SINGLE_BUFFER�ɂ����ꍇ
//    displayBuffer�𗧑̎�����̏ꍇ3�@�ʏ�̏ꍇ2��
//    commandList��4��
//    Save�pcommandList��1��
//    �m�ۂ��܂��B
//=====================================
typedef struct {
  GraphicsSystemBufferMode  buffer_mode;        // �o�b�t�@�����O���[�h
  b32                       useUpperRight;      // ���̎����邩?�@��x�ł����̎��ɂ���Ƃ���true
  b32                       stereoOn;           // ���̎�On��Ԃŏ��������邩�B
  DisplayBufferSetUp        displaySetup[DISPLAY_BOTH_MAX];   // �e�B�X�v���C�o�b�t�@�������f�[�^ ���ԏd�v�ł��B
  RenderBufferSetUp         frameSetup[DISPLAY_BOTH_MAX];   // �t���[���o�b�t�@�������f�[�^
  CommandListSetUp          commandSetup[COMMANDLIST_SETUP_NUM];// �R�}���h���X�g�������f�[�^
  CommandListSetUp          saveCommandSetup;   // �Z�[�u�p�R�}���h���X�g�������f�[�^�i�R�}���h�̍ė��p�Ɏg�p�j
  FrameBufferClearColor     clearColor[DISPLAY_NORMAL_MAX];   // �t���[���o�b�t�@�N���A�J���[
  FrameBufferClearFlag      clearFlag[DISPLAY_NORMAL_MAX];   // �t���[���o�b�t�@�N���A�t���O
} GraphicsSystemSetUp;

// ��{�Z�b�g�A�b�v���
extern const GraphicsSystemSetUp GraphicsSystemSetupDefault;


//-------------------------------------
///	�o�b�t�@�Ǘ��N���X
//=====================================
class BufferControl;

//-------------------------------------
///	�O���t�B�b�N�������Ǘ��N���X
//=====================================
class MemoryControl;

//-----------------------------------------------------------------------------
/**
 *					�O���t�B�b�N�X�V�X�e���@�N���X�錾
*/
//-----------------------------------------------------------------------------
class GraphicsSystem : public base::Singleton<GraphicsSystem>
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(GraphicsSystem);

public:
  // ���̎��̎����̕␳�l�̒�`
  static const f32 PARALLAX_DEPTH_RANGE_ZERO = 0.0f;  // ����0(���̎��Ȃ�)
  static const f32 PARALLAX_DEPTH_RANGE_ONE  = 1.0f;  // ����1(���̎��W��)

public:
  //-----------------------------------------------------------------------------
  /**
   *    @brief  �f�t�H���g�Z�b�g�A�b�v�f�[�^���g�p��������
   *
   *    @param  heapAllocator       GraphicsSystem���g�p����q�[�v�������A���P�[�^
   *    @param  deviceAllocator     GraphicsSystem���g�p����f�o�C�X�������A���P�[�^
   *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
   *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
   */
  //-----------------------------------------------------------------------------
  GraphicsSystem( heap::NwAllocator* heapAllocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );
  //-----------------------------------------------------------------------------
  /**
   *    @brief  �w��Z�b�g�A�b�v�f�[�^���g�p��������
   *
   *    @param  heapAllocator       �q�[�v�������A���P�[�^
   *    @param  deviceAllocator     �f�o�C�X�������A���P�[�^
   *    @param  setup               �Z�b�g�A�b�v���
   *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
   *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
   */
  //-----------------------------------------------------------------------------
  GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );

  //-----------------------------------------------------------------------------
  /**
   *    @brief  �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  ~GraphicsSystem();

  //-----------------------------------------------------------------------------
  /**
   *    @brief  �O���t�B�b�N�X�V�X�e�������������݂��`�F�b�N
   *
   *    @retval true  �������ς�
   *    @retval false ��������
   */
  //-----------------------------------------------------------------------------
  static bool IsInitialize( void );

  //-----------------------------------------------------------------------------
  /**
   *    @brief  �Q�[���Ƃ��āA���̎����g�p����̂��̐ݒ�
   */
  //-----------------------------------------------------------------------------
  static void SetGameStereoFlag( bool flag );
  static bool IsGameStereoFlag( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ��ʍ\����ύX����B
   *
   *	@param	setup �ύX�����ʍ\���̃Z�b�g�A�b�v���
   *
   *	�t���[���o�b�t�@�A�f�B�X�v���C�o�b�t�@�A�R�}���h���X�g�S�Ĕj�����܂��B
   *	���̏����́A�A�v���P�[�V��������Ⴄ�A�v���P�[�V�����ւ̉�ʓ]�����Ȃǂ�
   *	�g���邱�Ƃ�z�肵�Ă��܂��B
   */
  //-----------------------------------------------------------------------------
  void ChangeSetUp( const GraphicsSystemSetUp* setup, bool appUpperRightMode = false );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�����p�̃f�o�C�X�������AVRAM�Ǘ���������
   *
   *	@param	deviceAllocator           �A�v���P�[�V�����̃O���t�B�b�N�Ŏg�p����f�o�C�X�������̃A���P�[�^
   *
   *	VRAM�̈�ɂ́ASystemMemory�ŗ\�񂳂�Ă��镔���ȊO�̑S�Ă����蓖�Ă��܂��B
   *
   *	���̊֐����ĂԂƁA�A�v���P�[�V�����������ɐ؂�ւ��܂��B
   *
   */
  //-----------------------------------------------------------------------------
  void InitializeApplicationMemory( heap::NwAllocator* deviceAllocator );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�����p�̃f�o�C�X�������AVRAM�Ǘ���j��
   *
   *	VRAM�ɉ���Y�ꂪ�����Assart�łƂ܂�܂��B
   *
   *	����ȍ~�ŁA�m�ۂ���郁�����́A�V�X�e������������m�ۂ���܂��B
   *
   *	@retval true  �j������
   *	@retval false �j��������
   */
  //-----------------------------------------------------------------------------
  void FinalizeApplicationMemory( void );


  //-----------------------------------------------------------------------------
  /**
   *    �A�v���P�[�V�����@���ƂɁA���̎��̃��������m�ہA�������ۂɎg�p����֐��Q
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  /**
   *    @brief  �w��Z�b�g�A�b�v�f�[�^���g�p��������  �A�v���P�[�V�����ʂɗ��̎��o�b�t�@�m�ۂ����
   *
   *    @param  heapAllocator       �q�[�v�������A���P�[�^
   *    @param  deviceAllocator     �f�o�C�X�������A���P�[�^
   *    @param  setup               �Z�b�g�A�b�v���
   *    @param  appUpperRightMode   �E�ڂ̃o�b�t�@�𐶐�����B�@TRUE
   *    @param  systemVramASize      �V�X�e�����g�p����VRAM-A�T�C�Y
   *    @param  systemVramBSize      �V�X�e�����g�p����VRAM-B�T�C�Y
   */
  //-----------------------------------------------------------------------------
  GraphicsSystem( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, const GraphicsSystemSetUp* setup, bool appUpperRightMode, u32 systemVramASize = 0x300000, u32 systemVramBSize = 0x300000 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�������ŁA�g���������̎��o�b�t�@�����邩�`�F�b�N
   */
  //-----------------------------------------------------------------------------
  bool IsInitializedAppUpperRightBuffer( void ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�������ŁA�g���ŁA���̎����n�m�ɂ���B
   */
  //-----------------------------------------------------------------------------
  void InitializeAppUpperRightBuffer( heap::NwAllocator* deviceAllocator );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�������ŁA�g���Ŋm�ۂ����o�b�t�@���������B
   */
  //-----------------------------------------------------------------------------
  void ReqDeleteAppUpperRightBuffer( bool is_stereo_off = true );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �A�v���P�[�V�������ŁA�g���Ŋm�ۂ����o�b�t�@��������ꂽ�����`�F�b�N
   *	@return
   */
  //-----------------------------------------------------------------------------
  bool IsDeleteAppUpperRightBuffer( void ) const;
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


  //----------------------------------------------------------------------------
  /**
   *	@brief  �V�X�e�����̃������m�ې���V�X�e���������ɐ؂�ւ���B
   *
   *	@param  mode    �V�X�e���������̃��[�h
   *
   *	SystemMemoryMode
   *   SYSTEM_MEM_UNSETTLED,       // �ϓ��̂��郁���� (�O������m�ہj
   *   SYSTEM_MEM_FIX,             // �ϓ��̂Ȃ��������i�������m�ہj
   */
  //-----------------------------------------------------------------------------
  void BindSystemMemory( SystemMemoryMode mode );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �V�X�e�����̃������m�ې���A�v���P�[�V�����������ɖ߂��B
   */
  //-----------------------------------------------------------------------------
  void UnBindSystemMemory( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �������m�ې悪�V�X�e���������ɂȂ��Ă��邩�m�F����B
   */
  //-----------------------------------------------------------------------------
  bool IsBindSystemMemory(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  ���̎���ON/OFF�����N�G�X�g���܂��B ���ۂ̔��f�́AVSyncAfter�ɂȂ�܂��B
   *
   *	@param	stereo_on   ON:true   OFF:false
   */
  //-----------------------------------------------------------------------------
  void SetStereoModeReq( bool stereo_on );
#if GFL_DEBUG
  //���̎����L���̎��������[�h���؂�ւ����Ȃ��̂Ńf�o�b�O�p�ɗp��
  void SetStereoModeReqDebug( bool stereo_on );
#endif
  // ���̎�������H
  bool IsUseRightDisplay( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  �V���O���o�b�t�@�[�ɂ���t���O
   *
   *	@param	single_buffer_on   ON:true   OFF:false
   */
  //-----------------------------------------------------------------------------
  void SetSingleBufferReq( bool single_buffer_on );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �ʏ탌���_�����O�J�n����
   *
   *	@param  msk   �`����s���ʐݒ�
   */
  //-----------------------------------------------------------------------------
  void StartRendering( DisplayMsk msk = DISPLAY_MSK_BOTH );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �����_�����O�㏈��
   *
   *  @param    parallaxDepthRange  ���̎��̎����̕␳�l
   *	                               (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
   *                                  gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
   *
   *	���J���[���̃f�B�X�v���C�o�b�t�@�]���A�X���b�v����f�B�X�v���C�o�b�t�@�A�R�}���h���X�gID�̕ۑ��Ȃǂ��s���܂��B
   *	��StartRendering��msk�Ŏw�肵���ʂ̂ݍX�V���܂��B
   */
  //-----------------------------------------------------------------------------
  void EndRendering( f32 parallaxDepthRange = PARALLAX_DEPTH_RANGE_ONE );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �e��ʂ̃R�}���h�o�b�t�@�̎��s�����w��t���@�����_�����O�㏈��
   *
   *  @param  first     1�ԂɃR�}���h���s�������
   *  @param  second    2�ԂɃR�}���h���s�������
   *  @param  third     3�ԂɃR�}���h���s�������
   *  @param  parallaxDepthRange  ���̎��̎����̕␳�l
   *	                             (gfl::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_��
   *                                gfl::grp::g3d::StereoCamera��GetDepthRange�֐��̖߂�l��n���ĉ������B)
   */
  //-----------------------------------------------------------------------------
  void EndRenderingEx( DisplayType first, DisplayType second, DisplayType third, f32 parallaxDepthRange = PARALLAX_DEPTH_RANGE_ONE );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �X���b�v�o�b�t�@�[����
   *
   *	*���̊֐����s������A������VSyncWait���s���Ă��������B
   *
   *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
   *	  �P�D�O�t���[���ŊJ�n�����̎��`�揈���̊�����҂��܂��B
   *	  �Q�D�O�̃t���[���ŕ`�悵���ʂ�LOD�ɔ��f���܂��B
   *
   *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
   *	�@�P�D���t���[���Ő��������R�}���h�����s���܂��B
   *	�@�Q�D�`��̊�����҂��܂��B
   *	�@�R�D�`�挋�ʂ�LOD�ɔ��f���܂��B
   *
   */
  //-----------------------------------------------------------------------------
  void SwapBuffer( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���X�g�̎��s����������܂ő҂�
   */
  //-----------------------------------------------------------------------------
  void WaitDone( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    VSync�҂��@����
   *
   *	@param  display   �f�B�X�v���C�^�C�v  �iVSync��҂�� DISPLAY_UPPER or DISPLAY_LOWER or DISPLAY_BOTH_MAX �j
   */
  //-----------------------------------------------------------------------------
  static void WaitVSync( DisplayType display = DISPLAY_BOTH_MAX );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �X���b�v�o�b�t�@�@���N�G�X�g���o�Ă��邩�`�F�b�N
   *
   *	@retval true  �o�Ă���B    �iWaitVSync���Ă���VSyncAfter���ĂԕK�v������B�j
   *	@retval false �o�Ă��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  bool IsSwapBuffer( void ) const ;

  //----------------------------------------------------------------------------
  /**
   *	@brief    VSync�㏈��
   *
   *	*���̊֐����ĂԑO�ɕK��VSyncWait���s���Ă��������B
   *
   *	���_�u���o�b�t�@�����O���[�h�̏ꍇ
   *	  �R�D���t���[���Ő�������3D�R�}���h�����s���܂��B
   *	  �S�D�f�B�X�v���C�o�b�t�@�A�R�}���h���X�g�̏������ݐ��ύX���܂��B
   *
   *	���V���O���o�b�t�@�����O���[�h�̏ꍇ
   *	�@���ɂȂ�
   *
   *	@param  skip    ����̃R�}���h���s���X�L�b�v
   */
  //-----------------------------------------------------------------------------
  void VSyncAfter( bool skip = false );

  //----------------------------------------------------------------------------
  /**
   *	@brief  �f�B�X�v���C�̃N���A���߂𔭍s����B
   *
   *	@param	display       �f�B�X�v���C
   *	@param  b_color       �J���[�̃N���A���s�����H(true:�s���j
   *	@param  clear_color   �N���A�J���[
   *	@param  b_depth       �f�v�X�̃N���A���s�����H(true:�s���j
   *	@param  clear_depth   �N���A�f�v�X�l
   *	@param  b_stencil     �X�e���V���̃N���A���s�����H(true:�s��)
   *	@param  clear_stencil �N���A�X�e���V���l
   */
  //-----------------------------------------------------------------------------
  void ClearBuffer( DisplayType display, b32 b_color, const gfl::grp::ColorF32& clear_color, b32 b_depth, f32 clear_depth, b32 b_stencil = false, u8 clear_stencil = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �V�X�e�������삵�Ă���J�����g�̃f�B�X�v���C�o�b�t�@ID���擾
   *
   *	@param    display   �ʎw��
   *
   *	@return   �J�����g�̃f�B�X�v���C�o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  DisplayBufferID GetCurrentDisplayBufferID( DisplayType display )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �V�X�e�������삵�Ă���J�����g�̃t���[���o�b�t�@ID���擾
   *
   *	@param    display   �ʎw��
   *
   *	@return   �J�����g�̃t���[���o�b�t�@ID
   */
  //-----------------------------------------------------------------------------
  FrameBufferID GetCurrentFrameBufferID( DisplayType display )const;
  RenderTarget* GetCurrentFrameBuffer( DisplayType display )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �V�X�e�������삵�Ă���J�����g�̃R�}���h���X�gID���擾
   *
   *	@param  displayType �@�擾�������R�}���h���X�g�ɑΉ�������
   *
   *	@return   �J�����g�̃R�}���h���X�gID
   *
   *	displayType
   *	  DISPLAY_UPPER           ���� or ���ʗp�̃R�}���h���X�g
   *	  DISPLAY_UPPER_RIGHT     �E�ڗp�̃R�}���h���X�g
   *	  DISPLAY_LOWER           ����ʗp�̃R�}���h���X�g
   *	  DISPLAY_NONE            �I�t�X�N���[�������p�̃R�}���h���X�g�i�e�Ȃǁj
   */
  //-----------------------------------------------------------------------------
  CommandListID GetCurrentCommandListID( DisplayType displayType = DISPLAY_NONE )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �V�X�e�������삵�Ă���J�����g�̃R�}���h���X�g���o�C���h����
   *
   *	@param	displayType �o�C���h�������R�}���h���X�g�ɑΉ�������
   *
   *	displayType
   *	  DISPLAY_UPPER           ���� or ���ʗp�̃R�}���h���X�g
   *	  DISPLAY_UPPER_RIGHT     �E�ڗp�̃R�}���h���X�g
   *	  DISPLAY_LOWER           ����ʗp�̃R�}���h���X�g
   *	  DISPLAY_NONE            �I�t�X�N���[�������p�̃R�}���h���X�g�i�e�Ȃǁj
   *
   *	@retval true  �R�}���h���X�g���o�C���h�ł����B
   *	@retval false �R�}���h���X�g���o�C���h�ł��Ȃ������I(���̎�����Ȃ��Ƃ���UPPER_RIGHT���w�肳�ꂽ�j
   */
  //-----------------------------------------------------------------------------
  bool BindCurrentCommandList( DisplayType displayType = DISPLAY_NONE );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �����_�[�^�[�Q�b�g�Ɗ֘A�����R�}���h�o�b�t�@���o�C���h����
   *
   *	@param	renderTarget �@�`���ɂ����������_�[�^�[�Q�b�g
   *
   *	@retval true  �R�}���h���X�g���o�C���h�ł����B
   *	@retval false �R�}���h���X�g���o�C���h�ł��Ȃ������I(���̎�����Ȃ��Ƃ���UPPER_RIGHT���w�肳�ꂽ�j
   */
  //-----------------------------------------------------------------------------
  bool BindCurrentCommandList( const gfl::grp::RenderTarget* renderTarget );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �V�X�e�������삵�Ă���J�����g�̃Z�[�u�p�R�}���h���X�gID���擾
   *
   *	@return   �J�����g�̃Z�[�u�p�R�}���h���X�gID
   */
  //-----------------------------------------------------------------------------
  CommandListID GetCurrentSaveCommandListID(void)const;


  //-----------------------------------------------------------------------------
  /**
   *  �R�}���h�L���b�V���i�Z�[�u�����R�}���h���ė��p�j���s���֐�
   *
   *  �g�p�p�r�@���̎��\���ł̍��E�ڗp�����_�����O�Ɏg�p����R�}���h���ė��p����B
   *
   *  �g�p���@
   *    ���̎��ł́A�J�����̏��̃R�}���h�ȊO�͉E�ځE���ڂœ����R�}���h�ɂȂ邽�߁A
   *    ���ʕ������͂��߂�Save�p�R�}���h���X�g�ɕۑ����A���E�̃����_�����O�ɍė��p���邱�Ƃ��\�ƂȂ�܂��B
   *
   *    1.���ʕ����̃R�}���h��~��
   *      StartCommandSave();
   *      {
   *        (�J�����̏��͐ݒ肵�Ȃ��j
   *        Scene->Draw();    // �V�[���̕`��
   *      }
   *      EndCommandSave();
   *
   *    2.���ۂ̕`��
   *
   *      StartRendering();
   *      {
   *        // ���ڂ̕`��
   *        LeftCamera->Set();  // ���ڗp�J�����ݒ�̃R�}���h�𔭍s
   *        LeftRenderTarget->Set();// �����_�����O�^�[�Q�b�g�̐ݒ�
   *        ReuseCommandSave(); // ���ʕ����̃R�}���h���ė��p
   *
   *        // �E�ڂ̕`��
   *        RightCamera->Set();  // �E�ڗp�J�����ݒ�̃R�}���h�𔭍s
   *        RightRenderTarget->Set();// �����_�����O�^�[�Q�b�g�̐ݒ�
   *        ReuseCommandSave(); // ���ʕ����̃R�}���h���ė��p
   *      }
   *      EndRendering();
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *	@brief    �Z�[�u�p�R�}���h���X�g�ւ̃R�}���h�~�ϊJ�n
   */
  //-----------------------------------------------------------------------------
  void StartCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �Z�[�u�p�R�}���h���X�g�ւ̃R�}���h�~�ϏI��
   */
  //-----------------------------------------------------------------------------
  void EndCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �Z�[�u�p�R�}���h���X�g�֒~�ς��ꂽ�ŐV�̓��e���J�����g�R�}���h���X�g�ɐݒ�
   */
  //-----------------------------------------------------------------------------
  void ReuseCommandSave( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �f�t�H���g�N���A�J���[�̐ݒ�
   *
   *  @param  display   �ʎw��@�iDISPLAY_UPPER or DISPLAY_LOWER�j
   *	@param	red       R   0.0�`1.0
   *	@param	green     G   0.0�`1.0
   *	@param	blue      B   0.0�`1.0
   *	@param	alpha     A   0.0�`1.0
   */
  //-----------------------------------------------------------------------------
  void SetClearColor( DisplayType display, f32 red, f32 green, f32 blue, f32 alpha );
  
  //-----------------------------------------------------------------------------
  /**
   * @brief     �f�t�H���g�N���A�t���O�̐ݒ�
   * @param display     �ʎw��@�iDISPLAY_UPPER or DISPLAY_LOWER�j
   * @param color       �J���[�o�b�t�@�N���A  �L���@or ����
   * @param depth       �[�x�o�b�t�@�N���A  �L���@or ����
   * @param stencil     �X�e���V���o�b�t�@�N���A  �L���@or ����
   */
  //-----------------------------------------------------------------------------
  void SetClearColorFlag( DisplayType display, b32 color, f32 depth, f32 stencil );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �f�B�X�v���C�o�b�t�@�}�l�[�W���̎擾
   */
  //-----------------------------------------------------------------------------
  DisplayBufferManager* GetDisplayBufferManager( void )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �t���[���o�b�t�@�}�l�[�W���̎擾
   */
  //-----------------------------------------------------------------------------
  FrameBufferManager* GetFrameBufferManager( void )const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �R�}���h���X�g�}�l�[�W���̎擾
   */
  //-----------------------------------------------------------------------------
  CommandListManager* GetCommandListManager( void )const;



  //----------------------------------------------------------------------------
  /**
   *	@brief  �O���t�B�b�N�X�Z�b�g�A�b�v��
   *
   *	@return �O���t�B�b�N�X�Z�b�g�A�b�v�f�[�^
   */
  //-----------------------------------------------------------------------------
  const GraphicsSystemSetUp& GetSetup( void ) const { return m_setup; }


  //----------------------------------------------------------------------------
  /**
   *	@brief  �R�}���h���X�g���s�񐔁i��荞�ݒ��ӁI�j
   *
   *	@return �R�}���h���X�g���s��
   */
  //-----------------------------------------------------------------------------
  u32 GetCommandListDoneCount( void ) const ;


  //----------------------------------------------------------------------------
  /**
   *	@brief    ReadPixels�ɕK�v�ȃ������T�C�Y���擾����B
   */
  //-----------------------------------------------------------------------------
  u32 GetReadPixelsMemorySize( RenderColorFormat colorFormat, u32 width, u32 height ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾Request
   *
   *	@param  memoryTop     ���ʍ��i�[��@�i�s�v�ȏꍇNULL�j
   *	@param  memoryBottom  ����ʊi�[��  �i�s�v�ȏꍇNULL�j
   *	@param  type          �t���[���o�b�t�@�^�C�v  (gfl::grp::RenderTarget::RENDERBUFFER_TYPE_COLOR or gfl::grp::RenderTarget::RENDERBUFFER_TYPE_DEPTH)
   *	@param  memoryTopRight���ʉE�i�[��@�i�s�v�ȏꍇNULL�j
   */
  //-----------------------------------------------------------------------------
  // BLOCK���[�h
  void RequestReadPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  // ���j�A���[�h
  void RequestReadLinerPixels( void* memoryTop, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );
  void RequestReadLinerPixels( void* memoryTop, void* memoryTopRight, void* memoryBottom, RenderTarget::RenderBufferType type = RenderTarget::RENDERBUFFER_TYPE_COLOR );

  //----------------------------------------------------------------------------
  /**
   *	@brief    �J���[�o�b�t�@�̃s�N�Z�����̎擾�����҂�
   *
   *	@retval true    ����
   *	@retval false   �r��
   */
  //-----------------------------------------------------------------------------
  bool WaitReadPixels( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief    VRAM�c��̈���擾
   */
  //-----------------------------------------------------------------------------
  u32 GetVramAAllocatableSize( void ) const;
  u32 GetVramBAllocatableSize( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief    �������m�ۊ֐�
   */
  //-----------------------------------------------------------------------------
  static void* AllocMemory( MemoryArea area, MemoryUseType type, u32 size );
  static void FreeMemory( MemoryArea area, void* p_memory );
  
  static u32 GetAlignment( MemoryUseType type );//@fix �L�򂳂�v�]�F�u�q�`�l�A�h���X�w��z�u�Ή�



#if GFL_DEBUG
  //----------------------------------------------------------------------------
  /**
   *	@brief    �R�}���h���X�g�̏�Ԃ��_���v
   */
  //-----------------------------------------------------------------------------
  void DumpCommandList(void)const;


  //----------------------------------------------------------------------------
  /**
   *	@brief  GpuProfile�t���O��ݒ�
   *
   *	@param	flag  �t���O
   */
  //-----------------------------------------------------------------------------
  void SetGpuProfileFlag( bool flag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  GpuProfile�t���O���擾
   */
  //-----------------------------------------------------------------------------
  bool IsGpuProfileFlag( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  1�O�̃����_�����O�ł�GPU�v���t�@�C�����O���ʎ擾
   *
   *	@param  GpuProfileResult
   *
   *	@return ���ʂ̐��l
   *  @lim    �J�E���^���I�[�o�[�t���[�����u�ԁ@�Ԉ�������l���A���Ă��܂��B
   */
  //-----------------------------------------------------------------------------
  u32 GetGpuProfileResult( GpuProfileResult result ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  1�O�̃����_�����O�ł�GPU�v���t�@�C�����O���ʎ擾
   *
   *	@param  GpuProfileResult
   *
   *	@return ���ʂ̐��l
   *  @lim    �J�E���^���I�[�o�[�t���[�����u�ԁ@�Ԉ�������l���A���Ă��܂��B
   */
  //-----------------------------------------------------------------------------
  const u32* GetGpuProfileResultPtr( GpuProfileResult result ) const;
  
#if GFL_DEBUG    
  static u32 GetDrawFrameCount();
private:
  static u32 s_DrawFrameCount;
#endif

#endif

private:


private:

  // �A���P�[�^�̕ۑ�
  gfl::heap::NwAllocator* m_heapAllocator;
  gfl::heap::NwAllocator* m_deviceAllocator;

  // �Z�b�g�A�b�v���
  GraphicsSystemSetUp m_setup;

  // �o�b�t�@�R���g���[���N���X
  BufferControl*        m_bufferControl;

  // �������Ǘ���
  static MemoryControl* m_memoryControl;

  // �������t���O
  static bool m_Initialize;
  static bool m_GameStereoFlag;
private:

  void Initialize( heap::NwAllocator* allocator, heap::NwAllocator* deviceAllocator, u32 systemVramASize, u32 systemVramBSize, const GraphicsSystemSetUp* setup, bool appUpperRight );
  void Finalize( void );

  static void* AllocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, GLsizei size);
  static void DeallocateGraphicsMemory(GLenum area, GLenum aim, GLuint id, void* addr);


};



} /* namespace end grp */
} /* namespace end gfl */


/** ---------------------------------------------------------------------
//! @brief  SINGLETON�̑O���Q�Ɛ錾
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( gfl::grp::GraphicsSystem );


#endif // __GFL_GRAPHICSSYSTEM_H__
