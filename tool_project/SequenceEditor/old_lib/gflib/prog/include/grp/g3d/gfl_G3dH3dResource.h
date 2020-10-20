#ifndef __GFL_G3DH3DRESOURCE_H__
#define __GFL_G3DH3DRESOURCE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResource.h
 *	@brief  H3D���\�[�X
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*@note ���ӎ���
��xSetup�����o�b�t�@�͒��g������������Ă���Ǝv���B
�f�^�b�`��o�b�t�@���ė��p�������ꍇ�͒��ӂ��邱�ƁB
�o�b�t�@������������Ă��邱�Ƃ��m�F���Ă���킯�ł͂Ȃ����B



����
����H3dResource�ł�
nw::h3d::res::ResourceBinary
�������܂��B

�g����
(1)   AttachBuffer
(2)   Map
(3)   Setup(Map���Ă񂾏ꍇ�̓��������g��Ȃ�)
(3.5) VRAM�ւ̃R�s�[�R�}���h�̊����҂�
(4)   DeleteSomeSection


(1)   AttachBuffer
(2)   Setup(Map���Ă�ł��Ȃ��ꍇ�̓��������g��)
(3)   DeleteSomeSection


(1)   AttachBufferAndSetup(Map���Ă�ł��Ȃ��ꍇ�̓��������g��)
(2)   DeleteSomeSection
*/

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>
#include <bugfix_gflib.h>

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
///	H3D���\�[�X�N���X
//=====================================
class H3dResource
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dResource);


  friend class H3dModel;
  friend class H3dResSkeletalAnim;
  friend class H3dResMaterialAnim;
  friend class H3dResVisibilityAnim;
  friend class H3dResTexture;
  friend class H3dDynamicTexture;
  friend class H3dResShader;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �萔
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  enum
  {
    INDEX_NONE    = -1  // �C���f�b�N�X�����t����Ȃ��Ƃ�
  };

  //-------------------------------------
  ///	���ʃ��\�[�X�̃r�b�g
  //=====================================
  enum
  {
    COMMON_RESOURCE_NONE                     = 0,
    COMMON_RESOURCE_H3D_DEFAULT_SHADER_BIT   = 1 << 0
  };

  //-------------------------------------
  ///	Setup�̌���  �r�b�g��1�ɂȂ��Ă���Ƃ���ɖ�肠��
  //=====================================
  enum Result
  {
    RESULT_OK                             = 0,

    RESULT_BUFFER_TOO_SHORT               = 1 << 0,             // �o�b�t�@�����������܂��B 

    RESULT_RESOURCE_NOT_FOUND_TEXTURE     = 1 << 3,  
    RESULT_RESOURCE_NOT_FOUND_SHADER      = 1 << 4,  
    RESULT_RESOURCE_NOT_FOUND_LUT         = 1 << 5,

    //RESULT_ANIM_SOME_ELEMENT_NOT_BOUND    = 1 << 14,  // �������̃A�j���[�V�����v�f���o�C���h�ł��܂���ł����B 
    //RESULT_ANIM_ANY_ELEMENT_NOT_BOUND     = 1 << 15,   // �A�j���[�V�����v�f�� 1 ���o�C���h�ł��܂���ł����B 
    
    //RESULT_ANIM_SOME_TEXTURE_NOT_BOUND    = 1 << 14,  // �������̃e�N�X�`�����o�C���h�ł��܂���ł����B 
    //RESULT_ANIM_ANY_TEXTURE_NOT_BOUND     = 1 << 15
  };


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �\����
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  �������z�u
  struct MemoryLocation
  {
    // MemoryArea�ɂ���
    // gfl::grp::MEMORY_AREA_FCRAM�͉������Ȃ��A�ǂ��ɒu���Ă����Ă��������Ȃ��B
    // gfl::grp::MEMORY_AREA_VRAMA, gfl::grp::MEMORY_AREA_VRAMB��VRAM�ɃR�s�[����AVRAM�ɂ����Ă����Ă��R�s�[����B
    MemoryArea command_section;  // �R�}���h�Z�N�V�����̔z�u�ꏊ
    MemoryArea raw_section;      // RAW�Z�N�V����(�e�N�X�`���A���_�C���f�b�N�X�X�g���[���A���_�X�g���[��)�̔z�u�ꏊ
    
    MemoryLocation(void)
      : command_section(gfl::grp::MEMORY_AREA_FCRAM),
        raw_section(gfl::grp::MEMORY_AREA_FCRAM)
    {
      STATIC_ASSERT(gfl::grp::MEMORY_AREA_VRAMA==NN_GX_MEM_VRAMA);  // ���̒l�����̂܂܎g���Ă���ӏ�������̂ŁA
      STATIC_ASSERT(gfl::grp::MEMORY_AREA_VRAMB==NN_GX_MEM_VRAMB);  // �R���p�C�����ɕς���Ă��Ȃ����`�F�b�N����B
    }
    b32 IsCommandSectionVram(void) const
    {
      return (    command_section == gfl::grp::MEMORY_AREA_VRAMA
               || command_section == gfl::grp::MEMORY_AREA_VRAMB );
    }
    b32 IsRawSectionVram(void) const
    {
      return (    raw_section == gfl::grp::MEMORY_AREA_VRAMA
               || raw_section == gfl::grp::MEMORY_AREA_VRAMB );
    }
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
  H3dResource(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dResource();


  
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // ���[�h
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃��[�h(�����ǂݍ���)
   *
   *                 ���̊֐����I��������
   *                   AttachBuffer
   *                   SetOwnBufferFlag(true)
   *                 ���Ă���̂Ɠ�����ԂɂȂ�܂��B
   *                 SetOwnBufferFlag(true)�Ɠ�����ԂȂ̂ŁA�m�ۂ����o�b�t�@�̓f�X�g���N�^�Ŕj������܂��B
   *
   *  @param[in]  device_memory     �ŏI�I�Ɏc��o�b�t�@���m�ۂ���f�o�C�X������
   *  @param[in]  heap_memory_temp  �𓀂ȂǂɎg�p����ꎞ�I�ȃ�����
   *  @param[in]  path              �p�X�t���t�@�C����(��"rom:/field/FieldChar.bcsdr")
   *  @param[in]  arc_file          �A�[�J�C�u�t�@�C��
   *  @param[in]  arc_id            �A�[�NID
   *  @param[in]  arc_data_id       �A�[�J�C�u�t�@�C�����̃f�[�^ID
   *  @param[in]  is_compress       ���k�f�[�^�̏ꍇ��true
   *
   *  @retval  ����Ƀ��[�h�ł�����true��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, const char* path, b32 is_compress = false);
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, gfl::fs::ArcFile* arc_file, u32 arc_data_id, b32 is_compress = false);
  b32 Load(gfl::heap::HeapBase* device_memory, gfl::heap::HeapBase* heap_memory_temp, u32 arc_id, u32 arc_data_id, b32 is_compress = false);


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �A�^�b�` / �f�^�b�`
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �o�b�t�@�̃A�^�b�`
   *
   *  @param[in]     buff  �f�o�C�X��������...�ɔz�u���邱�ƁB  @note VRAM�ł�OK���낤�������Ă��Ȃ�(Map��VRAM�ɃR�s�[���邱�Ƃ������Ă���̂ł͂Ȃ�
   *                                                                  �ŏ�����VRAM�ɒu���Ă���ꍇ�̂��Ƃ�����@note�ł͌����Ă��܂��B)
   */
  //-----------------------------------------------------------------------------
  b32 AttachBuffer(void* buff);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �o�b�t�@�̃f�A�^�b�`
   */
  //-----------------------------------------------------------------------------
  void DetachBuffer( b32 fUnInit = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�^�b�`�����o�b�t�@�̃|�C���^�𓾂�
   *
   *  @retval        �o�b�t�@���A�^�b�`����Ă��Ȃ��Ƃ���NULL��Ԃ��B
   *                 �y���ӁzSetup�����S�Ɋ������Ă��邩�ۂ���Ԃ��킯�ł͂Ȃ��B
   */
  //-----------------------------------------------------------------------------
  const void* GetBuffer(void) const {  return m_buff;  }


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�^�b�`�����o�b�t�@�̊Ǘ������̃C���X�^���X�ōs�����ۂ��̃t���O�𓾂�
   *
   *  @retval        true�̂Ƃ��A�^�b�`���Ă���o�b�t�@�̊Ǘ������̃C���X�^���X�ōs���B
   */
  //-----------------------------------------------------------------------------
  b32 GetOwnBufferFlag(void) const
  { 
    return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_OWN_BUFF) != 0 );
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�^�b�`�����o�b�t�@�̊Ǘ������̃C���X�^���X�ōs�����ۂ��̃t���O��ݒ肷��
   *
   *  @param[in]     flag    true�̂Ƃ��A�^�b�`���Ă���o�b�t�@�̊Ǘ������̃C���X�^���X�ōs���B
   *                         �Ǘ����s���ꍇ��DetachBuffer(�f�X�g���N�^������Ă΂��)���Ƀo�b�t�@��GflHeapFreeMemory�ŉ������B
   */
  //-----------------------------------------------------------------------------
  void SetOwnBufferFlag(b32 flag)
  {
    SetBitFlag(&m_bit_flag, BIT_FLAG_OWN_BUFF, flag);
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �A�h���X�}�b�v
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X�̃A�h���X���蓖��
   *                 Setup������ł̓��\�[�X�̃A�h���X���蓖�Ă͂ł��Ȃ��̂ŁASetup�O�ɌĂ�ŉ������B
   *
   *  @param[in]     mem_loc      �������z�u
   *                              VRAM�ւ̃R�s�[���w�肳��Ă����ꍇ�́AVRAM�ւ̃R�s�[�R�}���h���J�����g�R�}���h���X�g�ɐς݂܂��B
   *                              ���̃R�}���h�����s�����܂�Delete����Section���Ȃ��悤�ɂ��ĉ������B
   *                              DeleteRelocationTableSection��VRAM�ւ̃R�s�[�Ɩ��֌W�Ȃ̂ŁASetup��Ȃ炢�ł��Ăяo���\�ł��B
   *
   *  @param[in]     vram_copy_heap_allocator     VRAM�ւ̃R�s�[���w�肳��Ă����ꍇ�́A�R�s�[�̍ۂɕK�v�Ȉꎞ�I�ȃ���������������m�ۂ���B
   *                                              NULL���w�肳��Ă���ꍇ�́Aheap_allocator���g���B
   *                                              ��������m�ۂ�����������DeleteCommandSection��������DeleteSomeSection���Ă񂾍ۂ�
   *                                              ��������(�Ă΂Ȃ���΁A���̃��\�[�X���폜����Ƃ��ɉ�������)�B
   */
  //-----------------------------------------------------------------------------
  void Map(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      const MemoryLocation* mem_loc=NULL,
      gfl::heap::NwAllocator* vram_copy_heap_allocator=NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����P�[�V�����e�[�u���Z�N�V�����̕��������������������
   *
   *                 Setup������Ȃ炢�ł��Ăяo���\�ł��B
   *
   *  @param[in]     allocator  AttachBuffer�œn����buff���m�ۂ��Ă���A���P�[�^
   */
  //-----------------------------------------------------------------------------
  void DeleteRelocationTableSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         RAW�Z�N�V�����̕��������������������
   *
   *                 RAW�Z�N�V����(�e�N�X�`���A���_�C���f�b�N�X�X�g���[���A���_�X�g���[��)��VRAM�ɃR�s�[���Ă���ꍇ��������ł��܂��B
   *                 Setup��������VRAM�ւ̃R�s�[�R�}���h������Ȃ炢�ł��Ăяo���\�ł��B
   *                 �y���ӁzVRAM�ւ̃R�s�[�R�}���h�����ォ�ǂ����̓`�F�b�N���Ă��Ȃ��̂ŌĂяo�����Œ��ӂ��ĉ������B
   *                
   *                 ��������
   *                 (1) DeleteRelocationTableSection���Ăяo���܂��B
   *                 (2) RAW�Z�N�V����������ł���ꍇ�͉�����܂��B
   *
   *  @param[in]     allocator  AttachBuffer�œn����buff���m�ۂ��Ă���A���P�[�^
   */
  //-----------------------------------------------------------------------------
  void DeleteRawSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R�}���h�Z�N�V�����̕��������������������
   *
   *                 �R�}���h�Z�N�V������RAW�Z�N�V������VRAM�ɃR�s�[���Ă���ꍇ��������ł��܂��B
   *                 Setup��������VRAM�ւ̃R�s�[�R�}���h������Ȃ炢�ł��Ăяo���\�ł��B
   *                 �y���ӁzVRAM�ւ̃R�s�[�R�}���h�����ォ�ǂ����̓`�F�b�N���Ă��Ȃ��̂ŌĂяo�����Œ��ӂ��ĉ������B
   *
   *                 ��������
   *                 (1) DeleteRawSection���Ăяo���܂��B�iDeleteRawSection�̓����������S�đ���܂��j
   *                 (2) �R�}���h�Z�N�V����������ł���ꍇ�͉�����܂��B
   *
   *  @param[in]     allocator  AttachBuffer�œn����buff���m�ۂ��Ă���A���P�[�^
   */
  //-----------------------------------------------------------------------------
  void DeleteCommandSection( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ����ł���Ƃ���̃��������������
   *
   *                 �R�}���h�Z�N�V������RAW�Z�N�V������VRAM�ɃR�s�[���Ă���ꍇ��������ł��܂��B
   *                 Setup��������VRAM�ւ̃R�s�[�R�}���h������Ȃ炢�ł��Ăяo���\�ł��B
   *                 �y���ӁzVRAM�ւ̃R�s�[�R�}���h�����ォ�ǂ����̓`�F�b�N���Ă��Ȃ��̂ŌĂяo�����Œ��ӂ��ĉ������B
   *
   *                 ��������
   *                 (1) DeleteCommandSection���Ăяo���܂��B�iDeleteCommandSection�̓����������S�đ���܂��j
   *
   *  @param[in]     allocator  AttachBuffer�œn����buff���m�ۂ��Ă���A���P�[�^
   */
  //-----------------------------------------------------------------------------
  void DeleteSomeSection( gfl::heap::NwAllocator* allocator );


  //----------------------------------------------------------------------------
  /**
   *	@brief  �q�����f�[�^�̐擪�A�h���X���擾
   */
  //-----------------------------------------------------------------------------
  uptr GetSrcRawAddress( void ) const;


  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �Z�b�g�A�b�v
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �Z�b�g�A�b�v
   */
  //-----------------------------------------------------------------------------
  b32 Setup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, H3dResource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  // @brief  name�̃e�N�X�`����~�������Ă���Ƃ����texture�����蓖�ĂăZ�b�g�A�b�v����
  // @param[in]  name     ���f�����~�������Ă���e�N�X�`�����Btexture�̖��O�ł͂Ȃ��B
  // @param[in]  texture  �e�N�X�`���B���O��name�łȂ��Ă������B
  //b32 Setup(const char* name, H3dResTexture* texture);
  //b32 Setup(const char* name, H3dDynamicTexture* texture);
 
  // @brief   gfl::grp::g3d::System���ێ����Ă���e�N�X�`�����g���āA�܂��e�N�X�`���Ɋւ��ăZ�b�g�A�b�v���������Ă��Ȃ��Ƃ��������������B
  //          �g���ꍇ�́A�K���ʏ��Setup����Ɏg���悤�ɂ��ĉ������B
  // @retval  �Z�b�g�A�b�v������������true��Ԃ��B
  //          �e�N�X�`���Ɋւ��ẴZ�b�g�A�b�v������Ŋ��������Ƃ��Ă��A���̃Z�b�g�A�b�v���܂��������Ă��Ȃ�������false��Ԃ��B
  b32 SetupUsingSystemTexture(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �Z�b�g�A�b�v�����S�Ɋ������Ă�����ture��Ԃ�
   */
  //-----------------------------------------------------------------------------
  b32 IsSetup(void) const { return (m_bit_flag & BIT_FLAG_IS_SETUP); }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���O�̃Z�b�g�A�b�v�̌��ʂ𓾂�
   *
   *  @retval        bit32   ���O�̃Z�b�g�A�b�v�̌���(Result�̃r�b�g�Ŕ��f���ĉ�����)
   */
  //-----------------------------------------------------------------------------
  bit32 GetResult(void) const { return m_result_desc; }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X���̑S�Ă̎Q�ƃI�u�W�F�N�g�����񂵂āA�������̎Q�ƃI�u�W�F�N�g�̖��O���o�͂���
   *                 GFL_DEBUG��0�̂Ƃ��͉������܂���B
   */
  //-----------------------------------------------------------------------------
  void PrintUnresolvedReference(void) const;

private:
  // @brief ���\�[�X���̑S�Ă̎Q�Ɖ������Ȃ���Ă�����true��Ԃ�
  b32 isAllReferenceResolved(void) const;


public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �֗��֐�  �A�^�b�` & �Z�b�g�A�b�v
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         AttachBuffer���Ă���Setup����
   *                 AttachBuffer��Setup�̃R�����g���悭�݂邱��
   */
  //-----------------------------------------------------------------------------
  b32 AttachBufferAndSetup(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buff, H3dResource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);




  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // �擾
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �擾
   */
  //-----------------------------------------------------------------------------
  s32 GetModelContentIndex(const char* name) const;
  s32 GetSkeletalAnimContentIndex(const char* name) const;
  s32 GetMaterialAnimContentIndex(const char* name) const;
  s32 GetVisibilityAnimContentIndex(const char* name) const;
  s32 GetTextureContentIndex(const char* name) const;
  
#ifdef BUGFIX_GFBTS1721_20130801
  s32 GetMaterialCount( s32 index_in_resource ) const;
#endif

  // NintendoWare/CTR/documents/API/nw/h3d/res/ShaderContent/Overview.html
  // nw::h3d::res::ShaderContent �\����
  // �Q�Ɨp�̖��O��"[�f�X�N���v�V����ID]@[�V�F�[�_��]"�ɂȂ�܂��B
  // ��: �C���f�b�N�X0�̖��O 0@DefaultShader
  //     �C���f�b�N�X1�̖��O 1@DefaultShader
  //     �C���f�b�N�X2�̖��O 0@UserShader
  //     �C���f�b�N�X3�̖��O 1@UserShader
  s32 GetShaderContentIndex(const char* name) const;
  const char* GetShaderContentName(s32 index) const;

  u32 GetModelContentCount(void) const;
  u32 GetSkeletalAnimContentCount(void) const;
  u32 GetMaterialAnimContentCount(void) const;
  u32 GetVisibilityAnimContentCount(void) const;
  u32 GetTextureContentCount(void) const;
  u32 GetShaderContentCount(void) const;

  // H3dResAnim::GetAnimElementNum�Ɠ����l��������
  // index_in_resource�Ԗڂ�SkeletalAnimContent�Ɋ܂܂�Ă���A�j���[�V�����v�f���𓾂�
  u32 GetSkeletalAnimElementNum(s32 index_in_resource) const;
  // index_in_resource�Ԗڂ�MaterialAnimContent�Ɋ܂܂�Ă���A�j���[�V�����v�f���𓾂�
  u32 GetMaterialAnimElementNum(s32 index_in_resource) const;
  // index_in_resource�Ԗڂ�VisibilityAnimContent�Ɋ܂܂�Ă���A�j���[�V�����v�f���𓾂�
  u32 GetVisibilityAnimElementNum(s32 index_in_resource) const;



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
  nw::h3d::res::ResourceBinary* GetNwResourceBinary(void) const { return m_resbin; }
  
  nw::h3d::res::ModelContent* GetNwModelContent(const char* name) const;
  nw::h3d::res::ModelContent* GetNwModelContent(const s32 index) const;
  nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(const char* name) const;
  nw::h3d::res::SkeletalAnimContent* GetNwSkeletalAnimContent(const s32 index) const;
  nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(const char* name) const;
  nw::h3d::res::MaterialAnimContent* GetNwMaterialAnimContent(const s32 index) const;
  nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(const char* name) const;
  nw::h3d::res::VisibilityAnimContent* GetNwVisibilityAnimContent(const s32 index) const;
  nw::h3d::res::TextureContent* GetNwTextureContent(const char* name) const;
  nw::h3d::res::TextureContent* GetNwTextureContent(const s32 index) const;
  nw::h3d::res::ShaderContent* GetNwShaderContent(const char* name) const;
  nw::h3d::res::ShaderContent* GetNwShaderContent(const s32 index) const;


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
  void init(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator);
  void linkOwn(void);
  void linkOther(H3dResource* other_resource);

private:
  enum
  {
    BIT_FLAG_NONE        =  0,
    BIT_FLAG_IS_INIT     =  1 <<  0,  // �Z�b�g�A�b�v�̏������܂Ŋ������Ă�����1
    BIT_FLAG_IS_LINK_OWN =  1 <<  1,  // �Z�b�g�A�b�v�̎��ȃ����N�܂Ŋ������Ă�����1(���ꂪ1�ɂȂ�Ƃ��͕K��BIT_FLAG_IS_INIT��1�ɂȂ��Ă���)
    BIT_FLAG_IS_SETUP    =  1 <<  2,  // �Z�b�g�A�b�v�����S�Ɋ������Ă�����1(���ꂪ1�ɂȂ�Ƃ��͕K��BIT_FLAG_IS_LINK_OWN��1�ɂȂ��Ă���)
    BIT_FLAG_OWN_BUFF    =  1 <<  3   // m_buff�̊Ǘ������̃C���X�^���X�ōs���ꍇ��1
                                      // �Ǘ����s���ꍇ��DetachBuffer(�f�X�g���N�^������Ă΂��)���Ƀo�b�t�@��GflHeapFreeMemory�ŉ������B
  };

private:
  MemoryLocation                           m_mem_loc;
  nw::h3d::res::ResourceBinary::AddressMap m_address_map;
  u32                                      m_srcRawAddress;
  void*                         m_buff;
  nw::h3d::res::ResourceBinary* m_resbin;
  bit32                         m_bit_flag;

  bit32 m_result_desc;  // Setup�̌��ʂ����Ă���
};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DRESOURCE_H__

