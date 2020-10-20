#ifndef __GFL_G3DRESOURCE_H__
#define __GFL_G3DRESOURCE_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResource.h
 *	@brief  3D���\�[�X
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

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
#include <grp/g3d/gfl_G3dObject.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X�錾
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�N���X�̑O���錾
//=====================================
class ResAnim;


//-------------------------------------
///	3D���\�[�X�N���X
//=====================================
class Resource : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(Resource);

public:
  //---------------------------------------------------------------------------
  /**
   *           �萔
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	���\�[�X�p�̒萔
  //=====================================
  enum
  {
    RESOURCE_INDEX_NOT_FOUND    = -1  // �w�肵�����O�������\�[�X�����݂��Ȃ��ꍇ�̃��\�[�X�̃C���f�b�N�X
  };

  //-------------------------------------
  ///	���ʃ��\�[�X�̃r�b�g
  //=====================================
  enum
  {
    COMMON_RESOURCE_NONE                           = 0,
    COMMON_RESOURCE_DEFAULT_SHADER_BIT             = 1 <<  0,
    COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT    = 1 <<  1,
    COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT          = 1 <<  2,

    // Ex���ʃf�[�^�����ʃ��\�[�X�Ƃ��ėp����(Ex���ʃ��\�[�X�ƌĂ�)
    EX_COMMON_RESOURCE_BIT_00  = 1 <<  3,
    EX_COMMON_RESOURCE_BIT_01  = 1 <<  4,
    EX_COMMON_RESOURCE_BIT_02  = 1 <<  5,
    EX_COMMON_RESOURCE_BIT_03  = 1 <<  6,
    EX_COMMON_RESOURCE_BIT_04  = 1 <<  7,
    EX_COMMON_RESOURCE_BIT_05  = 1 <<  8,
    EX_COMMON_RESOURCE_BIT_06  = 1 <<  9,
    EX_COMMON_RESOURCE_BIT_07  = 1 << 10,
  };

  //-------------------------------------
  ///	�A�j���[�V�������\�[�X�̃^�C�v
  //=====================================
  enum AnimationResourceType  // @note gfl::grp::g3d::AnimationType�����ɂ������̂ŁA����������B
  {
    ANIMATION_RESOURCE_TYPE_NONE        = ANIMATION_TYPE_NONE,        // �����ݒ肳��Ă��Ȃ��Ƃ��̃^�C�v
    ANIMATION_RESOURCE_TYPE_SKELETAL    = ANIMATION_TYPE_SKELETAL,
    ANIMATION_RESOURCE_TYPE_MATERIAL    = ANIMATION_TYPE_MATERIAL,
    ANIMATION_RESOURCE_TYPE_VISIBILITY  = ANIMATION_TYPE_VISIBILITY,
    ANIMATION_RESOURCE_TYPE_CAMERA      = ANIMATION_TYPE_CAMERA,
    ANIMATION_RESOURCE_TYPE_LIGHT       = ANIMATION_TYPE_LIGHT,
    ANIMATION_RESOURCE_TYPE_ALL         = ANIMATION_TYPE_ALL         // �A�j���[�V��������������Ƃ��̂ݓn����^�C�v
  };

  //---------------------------------------------------------------------------
  /**
   *           �\���̐錾
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	���\�[�X�̐ݒ���e
  //=====================================
  struct Description
  {
    u32 texture_location_flag;        // �e�N�X�`���̃������z�u�ꏊ
    u32 index_stream_location_flag;   // ���_�C���f�b�N�X�X�g���[���̃������z�u�ꏊ
    u32 vertex_stream_location_flag;  // ���_�X�g���[���̃������z�u�ꏊ

    //-----------------------------------------------------------------------------
    /**
     *  @brief         �R���X�g���N�^
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : texture_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        index_stream_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        vertex_stream_location_flag(gfl::grp::MEMORY_AREA_FCRAM | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP)
      //: texture_location_flag(gfl::grp::MEMORY_AREA_VRAMA | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
      //  index_stream_location_flag(gfl::grp::MEMORY_AREA_VRAMB | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
      //  vertex_stream_location_flag(gfl::grp::MEMORY_AREA_VRAMB | gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP)
    {}
  };

  //-------------------------------------
  ///	�A�j���[�V�������\�[�X�̏��p�b�N
  //=====================================
  struct AnimationResourcePack
  {
    Resource*             resource;
    AnimationResourceType animation_resource_type;
    s32                   index_in_resource;
  };

  //-------------------------------------
  ///	�e�N�X�`�����\�[�X�̏��p�b�N
  //=====================================
  struct TextureResourcePack
  {
    Resource*             resource;
    s32                   index_in_resource;
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�[�A�j���[�V�����̗v�f���ōő�̒l�𓾂�
   *
   *  @param[in]     array_element_num              animation_resource_pack_array�z��̗v�f�� 
   *  @param[in]     animation_resource_pack_array  AnimationResourcePack�̔z��̐擪�ւ̃|�C���^
   * 
   *  @retval        s32  animation_resource_pack_array�̃����o�[�A�j���[�V�����̗v�f���ōő�̒l
   *
   *  �g�p��
   *  gfl::grp::g3d::Resource::AnimationResourcePack  animation_resource_pack_array[]  =
   *  {
   *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
   *      { resource0, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
   *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 0 },
   *      { resource1, gfl::grp::g3d::Resource::ANIMATION_RESOURCE_TYPE_SKELETAL, 1 },
   *  };
   *  s32 max_member_anim_set_count = gfl::grp::g3d::Resource::GetMaxMemberAnimSetCount(
   *      sizeof(animation_resource_pack_array)/sizeof(animation_resource_pack_array[0]),
   *      animation_resource_pack_array
   *  );
   */
  //-----------------------------------------------------------------------------
  static s32 GetMaxMemberAnimSetCount(
      const u32                    array_element_num,
      const AnimationResourcePack* animation_resource_pack_array
  );


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     path         �O���t�B�b�N�X���\�[�X�t�@�C���̃p�X��
   *                              (allocator��path�̗�����NULL�łȂ��Ƃ��̓��[�h����B
   *                               GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s)
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (���[�h�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     do_setup     �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v���s���ꍇ��true
   *                              (���[�h�����Ƃ��̂ݎg�p����B
   *                               �Z�b�g�A�b�v�ł��Ă��邩�ۂ���IsSetup�Ŋm�F)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   */
  //-----------------------------------------------------------------------------
  Resource(gfl::heap::NwAllocator* allocator = NULL, const char* path = NULL, const Description* description = NULL, bool do_setup = true, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃A�^�b�`
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
   *                              (GetBuffer�̖߂�l��NULL�̂Ƃ��̓o�b�t�@�̃A�^�b�`���s)
   *                              @note const���t���Ă��܂���NintendoWare��const_cast���ꏑ���������܂��B
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     do_setup     �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v���s���ꍇ��true
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����B
   *                               �Z�b�g�A�b�v�ł��Ă��邩�ۂ���IsSetup�Ŋm�F)
   *  @param[in]     own_buffer   ture�̂Ƃ�buffer�̊Ǘ��͂��̃C���X�^���X���s���̂ŌĂяo������buffer��������Ă͂Ȃ�Ȃ��B
   *                              false�̂Ƃ�buffer�̊Ǘ��͌Ăяo�����ōs���ĉ������B
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   */
  //-----------------------------------------------------------------------------
  Resource(
      gfl::heap::NwAllocator* allocator,
      const void* buffer,
      const Description* description = NULL,
      bool do_setup = true,
      bool own_buffer = false,
      u32 common_resource_flag = COMMON_RESOURCE_NONE );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         �R���X�g���N�^
   *                 �A�[�J�C�u�t�@�C������f�[�^��ǂݍ���
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     arc_file     �A�[�J�C�u�t�@�C��
   *                              (GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s)
   *  @param[in]     arc_data_id  �A�[�J�C�u�t�@�C�����̃f�[�^ID
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�ǂݍ��񂾂Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     do_setup     �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v���s���ꍇ��true
   *                              (�ǂݍ��񂾂Ƃ��̂ݎg�p����B
   *                               �Z�b�g�A�b�v�ł��Ă��邩�ۂ���IsSetup�Ŋm�F)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   *  @param[in]     is_compress  ���k�f�[�^�̏ꍇ��true
   */
  //-----------------------------------------------------------------------------
  Resource(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile*       arc_file,
      u32                     arc_data_id,
      const Description* description = NULL,
      bool do_setup = true,
      u32 common_resource_flag = COMMON_RESOURCE_NONE,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �f�X�g���N�^
   */
  //-----------------------------------------------------------------------------
  virtual ~Resource(); 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃��[�h
   *                 ���[�h�����Ńt�@�C�����̃R���e���g�̃Z�b�g�A�b�v�͂��Ȃ��B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     path         �O���t�B�b�N�X���\�[�X�t�@�C���̃p�X��
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (���[�h�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *
   *  @retval        bool    false�̂Ƃ��̓��[�h���s�B
   *                         (���s�̗��R��
   *                          allocator���s�����Apath���s�����A���Ƀ��[�h�������̃f�[�^�����݂��邩�A
   *                          �������s���̂ǂꂩ)
   */
  //-----------------------------------------------------------------------------
  bool Load(gfl::heap::NwAllocator* allocator, const char* path, const Description* description = NULL);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�[�J�C�u�t�@�C������f�[�^��ǂݍ���
   *                 �ǂݍ��ނ����Ńt�@�C�����̃R���e���g�̃Z�b�g�A�b�v�͂��Ȃ��B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     arc_file     �A�[�J�C�u�t�@�C��
   *                              (GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s)
   *  @param[in]     arc_data_id  �A�[�J�C�u�t�@�C�����̃f�[�^ID
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�ǂݍ��񂾂Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     is_compress  ���k�f�[�^�̏ꍇ��true
   * 
   *  @retval        bool    false�̂Ƃ��̓��[�h���s�B
   *                         (���s�̗��R��
   *                          allocator���s�����Apath���s�����A���Ƀ��[�h�������̃f�[�^�����݂��邩�A
   *                          �������s���̂ǂꂩ)
   */
  //-----------------------------------------------------------------------------
  bool Load(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile* arc_file,
      u32 arc_data_id,
      const Description* description = NULL,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃A�����[�h
   *
   *  @retval        bool    false�̂Ƃ��̓A�����[�h���s�B
   *                         (���s�̗��R�̓��[�h�����f�[�^�����݂��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  bool Unload(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃A�^�b�`
   *                 �o�b�t�@�̃A�^�b�`�����Ńt�@�C�����̃R���e���g�̃Z�b�g�A�b�v�͂��Ȃ��B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
   *                              @note const���t���Ă��܂���NintendoWare��const_cast���ꏑ���������܂��B
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     own_buffer   ture�̂Ƃ�buffer�̊Ǘ��͂��̃C���X�^���X���s���̂ŌĂяo������buffer��������Ă͂Ȃ�Ȃ��B
   *                              false�̂Ƃ�buffer�̊Ǘ��͌Ăяo�����ōs���ĉ������B
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����)
   *
   *  @retval        bool    false�̂Ƃ��̓o�b�t�@�̃A�^�b�`���s�B
   *                         (���s�̗��R��
   *                          allocator���s�����Abuffer���s�����A���Ƀ��[�h�������̃f�[�^�����݂��邩�A
   *                          �������s���̂ǂꂩ)
   */
  //-----------------------------------------------------------------------------
  bool AttachBuffer(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description = NULL, bool own_buffer = false);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃f�^�b�`
   *
   *  @retval        bool    false�̂Ƃ��̓o�b�t�@�̃f�^�b�`���s�B
   *                         (���s�̗��R�̓o�b�t�@���A�^�b�`�����f�[�^�����݂��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  bool DetachBuffer(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v
   *                 �Z�b�g�A�b�v����������܂ŁAother_resource��ύX���ĉ��x�Ăяo���Ă��悢�B
   *
   *  @param[in]     allocator         �A���P�[�^
   *  @param[in]     other_resource    �Z�b�g�A�b�v�ɕK�v�ȑ��̃��\�[�X
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   *
   *  @retval        bool   true�̂Ƃ��̓Z�b�g�A�b�v�����B
   *                        false�̂Ƃ��̓Z�b�g�A�b�v���܂��������Ă��Ȃ��B���̗��R��GetResult�Ŋm�F�B
   */
  //-----------------------------------------------------------------------------
  bool Setup(gfl::heap::NwAllocator* allocator, Resource* other_resource = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃��[�h
   *                 �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v
   *                 Load�̌��Setup���s���B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     path         �O���t�B�b�N�X���\�[�X�t�@�C���̃p�X��
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (���[�h�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   *
   *  @retval        bool   true�̂Ƃ��̓��[�h�ɐ������A�Z�b�g�A�b�v���������Ă���B
   *                        false�̂Ƃ��̓��[�h�Ɏ��s�������Z�b�g�A�b�v���������Ă��Ȃ��B
   *                        GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s�ANULL�łȂ��Ƃ��̓Z�b�g�A�b�v���������Ă��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  bool LoadAndSetup(gfl::heap::NwAllocator* allocator, const char* path, const Description* description = NULL, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�[�J�C�u�t�@�C������f�[�^��ǂݍ���
   *                 �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v
   *                 Load�̌��Setup���s���B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     arc_file     �A�[�J�C�u�t�@�C��
   *                              (GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s)
   *  @param[in]     arc_data_id  �A�[�J�C�u�t�@�C�����̃f�[�^ID
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�ǂݍ��񂾂Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   *  @param[in]     is_compress  ���k�f�[�^�̏ꍇ��true
   *
   *  @retval        bool   true�̂Ƃ��̓��[�h�ɐ������A�Z�b�g�A�b�v���������Ă���B
   *                        false�̂Ƃ��̓��[�h�Ɏ��s�������Z�b�g�A�b�v���������Ă��Ȃ��B
   *                        GetBuffer�̖߂�l��NULL�̂Ƃ��̓��[�h���s�ANULL�łȂ��Ƃ��̓Z�b�g�A�b�v���������Ă��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  bool LoadAndSetup(
      gfl::heap::NwAllocator* allocator,
      gfl::fs::ArcFile* arc_file,
      u32 arc_data_id,
      const Description* description = NULL,
      u32 common_resource_flag = COMMON_RESOURCE_NONE,
      const b32 is_compress = false );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃A�^�b�`
   *                 �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v
   *                 Attach�̌��Setup���s���B
   *
   *  @param[in]     allocator    �A���P�[�^
   *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
   *                              @note const���t���Ă��܂���NintendoWare��const_cast���ꏑ���������܂��B
   *  @param[in]     description  ���\�[�X�̐ݒ���e
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����B
   *                               NULL�̂Ƃ��̓f�t�H���g�l�B
   *                               �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
   *  @param[in]     own_buffer   ture�̂Ƃ�buffer�̊Ǘ��͂��̃C���X�^���X���s���̂ŌĂяo������buffer��������Ă͂Ȃ�Ȃ��B
   *                              false�̂Ƃ�buffer�̊Ǘ��͌Ăяo�����ōs���ĉ������B
   *                              (�o�b�t�@���A�^�b�`�����Ƃ��̂ݎg�p����)
   *  @param[in]     common_resource_flag  �g�p���鋤�ʃ��\�[�X������ꍇ�̓r�b�g�𗧂ĂĂ���
   *
   *  @retval        bool   true�̂Ƃ��̓o�b�t�@�̃A�^�b�`�ɐ������A�Z�b�g�A�b�v���������Ă���B
   *                        false�̂Ƃ��̓o�b�t�@�̃A�^�b�`�Ɏ��s�������Z�b�g�A�b�v���������Ă��Ȃ��B
   *                        GetBuffer�̖߂�l��NULL�̂Ƃ��̓o�b�t�@�̃A�^�b�`���s�ANULL�łȂ��Ƃ��̓Z�b�g�A�b�v���������Ă��Ȃ��B
   */
  //-----------------------------------------------------------------------------
  bool AttachBufferAndSetup(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description = NULL, bool own_buffer = false, u32 common_resource_flag = COMMON_RESOURCE_NONE);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �Z�b�g�A�b�v���������Ă��邩
   *
   *  @retval        bool   true�̂Ƃ��̓Z�b�g�A�b�v�����B
   *                        false�̂Ƃ��̓Z�b�g�A�b�v���܂��������Ă��Ȃ��B���̗��R��GetResult�Ŋm�F�B
   */
  //-----------------------------------------------------------------------------
  bool IsSetup(void) const {  return m_is_setup;  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���O�̃Z�b�g�A�b�v�̌��ʂ𓾂�
   *
   *  @retval        int    ���O�̃Z�b�g�A�b�v�̌���(nw::gfx::Result::GetDescription)
   */
  //-----------------------------------------------------------------------------
  int  GetResult(void) const  {  return m_nw_result.GetDescription();  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X���̑S�Ă̎Q�ƃI�u�W�F�N�g�����񂵂āA�������̎Q�ƃI�u�W�F�N�g�̖��O���o�͂���
   *                 GFL_DEBUG��0�̂Ƃ��͉������܂���B
   */
  //-----------------------------------------------------------------------------
  void PrintUnresolvedReference(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���̃��\�[�X�̃������T�C�Y���A�w�b�_�[�������̃������T�C�Y�ɂ���
   *
   *                 ���f�[�^����VRAM�ɔz�u���Ă���̂Ńf�o�C�X�������ɂ̓w�b�_�[�������c���Ă����΂����A
   *                 ��������f�[�^�����f�o�C�X����������폜���悤�A�Ƃ����Ƃ��Ɏg�p���ĉ������B
   *
   *                 Setup������������ɌĂ�ŉ������B
   *                 ���̃��\�[�X�Ɋ܂܂��S�Ẵf�[�^��VRAM�ɔz�u���Ă���ꍇ�̂݁A�g�p�\�ł��B
   *                 1�ł�FCRAM�ɔz�u���Ă���ꍇ�͎g�p���Ȃ��ŉ������B
   *
   *                 �ǂݍ��݂����̃��\�[�X�N���X�ōs���Ă���Ƃ�(Load�n�̊֐��𗘗p�����Ƃ�)�́A
   *                 �ǂݍ��݂Ɏg�p�����A���P�[�^��n���ĉ������B
   *                 ���̃A���P�[�^����m�ۂ��Ă���o�b�t�@�̃T�C�Y���w�b�_�[���̃T�C�Y�ɂ܂ŏk�����܂��B
   *
   *                 �����Ńo�b�t�@���m�ۂ����̃��\�[�X�N���X�ɃA�^�b�`�����Ƃ�(AttachBuffer�n�̊֐��𗘗p�����Ƃ�)�́A
   *                 ���̃o�b�t�@���m�ۂ����A���P�[�^��n���ĉ������B
   *                 ���̃A���P�[�^����m�ۂ��Ă���o�b�t�@�̃T�C�Y���w�b�_�[���̃T�C�Y�ɂ܂ŏk�����܂��B
   *
   *  @param[in]     allocator    �A���P�[�^
   */
  //-----------------------------------------------------------------------------
  void DeleteImageBlockData( gfl::heap::NwAllocator* allocator );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���[�h�����o�b�t�@�ւ̃|�C���^�𓾂�
   *
   *  @retval        cosnt void*  ���[�h�����o�b�t�@�ւ̃|�C���^(NULL�̂Ƃ��̓��[�h���Ă��Ȃ�)
   *                              @note const���t���Ă��܂���NintendoWare��const_cast���ꏑ���������܂��B
   */
  //-----------------------------------------------------------------------------
  const void* GetBuffer(void) const {  return m_buffer;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���f�����\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    ���f�����\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetModelsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J�������\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �J�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetCamerasCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���C�g���\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    ���C�g���\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetLightsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �t�H�O���\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �t�H�O���\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetFogsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�[�����ݒ胊�\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �V�[�����ݒ胊�\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetSceneEnvironmentSettingsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �X�P���^���A�j���[�V�������\�[�X�̗v�f�����擾����
   *
   *  @retval        s32    �X�P���^���A�j���[�V�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetSkeletalAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �}�e���A���A�j���[�V�������\�[�X�̗v�f�����擾����
   *
   *  @retval        s32    �}�e���A���A�j���[�V�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetMaterialAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �r�W�r���e�B�A�j���[�V�������\�[�X�̗v�f�����擾����
   *
   *  @retval        s32    �r�W�r���e�B�A�j���[�V�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetVisibilityAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �J�����A�j���[�V�������\�[�X�̗v�f�����擾����
   *
   *  @retval        s32    �J�����A�j���[�V�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetCameraAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���C�g�A�j���[�V�������\�[�X�̗v�f�����擾����
   *
   *  @retval        s32    ���C�g�A�j���[�V�������\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetLightAnimsCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`�����\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �e�N�X�`�����\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetTexturesCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �V�F�[�_�[���\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �V�F�[�_�[���\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetShadersCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �G�~�b�^�[���\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    �G�~�b�^�[���\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetEmittersCount() const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���b�N�A�b�v�e�[�u���Z�b�g���\�[�X�̗v�f�����擾����
   * 
   *  @retval        s32    ���b�N�A�b�v�e�[�u���Z�b�g���\�[�X�̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32                 GetLutSetsCount() const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X�̃C���f�b�N�X�ԍ����擾����
   * 
   *  @param[in]     key  ���\�[�X�̖��O
   * 
   *  @retval        s32    ���\�[�X�̃C���f�b�N�X�ԍ�
   *                        �w�肵�����O�������\�[�X�����݂��Ȃ��ꍇ�ARESOURCE_INDEX_NOT_FOUND��Ԃ�
   *
   */
  //-----------------------------------------------------------------------------
  s32 GetModelsIndex(const char* key) const;
  s32 GetCamerasIndex(const char* key) const;
  s32 GetLightsIndex(const char* key) const;
  s32 GetFogsIndex(const char* key) const;
  s32 GetSceneEnvironmentSettingsIndex(const char* key) const;
  s32 GetSkeletalAnimsIndex(const char* key) const;
  s32 GetMaterialAnimsIndex(const char* key) const;
  s32 GetVisibilityAnimsIndex(const char* key) const;
  s32 GetCameraAnimsIndex(const char* key) const;
  s32 GetLightAnimsIndex(const char* key) const;
  s32 GetTexturesIndex(const char* key) const;
  s32 GetShadersIndex(const char* key) const;
  s32 GetEmittersIndex(const char* key) const;
  s32 GetLutSetsIndex(const char* key) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �����o�[�A�j���[�V�����̗v�f�����擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   * 
   *  @retval        s32    �����o�[�A�j���[�V�����̗v�f��
   */
  //-----------------------------------------------------------------------------
  s32 GetSkeletalMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetMaterialMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetVisibilityMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetCameraMemberAnimSetCount(const s32 index_in_resource) const;
  s32 GetLightMemberAnimSetCount(const s32 index_in_resource) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         �A�j���[�V�������\�[�X���擾����
   * 
   *  @param[in]     type               �A�j���[�V�������\�[�X�̃^�C�v
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @param[out]    res_anim           �擾�����A�j���[�V�������\�[�X�̑����
   */
  //-----------------------------------------------------------------------------
  void GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const;
  

#if GFL_DEBUG
  //-----------------------------------------------------------------------------
  /**
   *  @brief    �A�j���[�V�����̓��e�m�F
   */
  //-----------------------------------------------------------------------------
  void DEBUG_PrintAnimResult(const AnimationResourceType type, const s32 index_in_resource);
  s32  DEBUG_GetMemberAnimSetIndex(const AnimationResourceType type, const s32 index_in_resource, const char* key) const;
#endif


  // @note gfl::grp::g3d::Resource�N���X����剻����̂�h�����߂ɂ��A�e�N�X�`���̏��̎擾��gfl::grp::g3d::Texture�N���X��
  // �n���Ă���s�����ق���������������Ȃ�
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`���̍���(�c�̒���)���擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @retval        s32                �e�N�X�`���̍���(�c�̒���)
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureHeight(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`���̕�(���̒���)���擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @retval        s32                �e�N�X�`���̕�(���̒���)
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureWidth(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`���̃t�H�[�}�b�g���擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @retval        s32                gfl::grp::g3d::Texture::Format
   */
  //-----------------------------------------------------------------------------
  s32 GetTextureFormat(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`����1�s�N�Z��������̃r�b�g�����擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @retval        u32                �e�N�X�`����1�s�N�Z��������̃r�b�g��
   */
  //-----------------------------------------------------------------------------
  u32 GetTextureBitsPerPixel(const s32 index_in_resource) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �e�N�X�`���̃C���[�W�̃A�h���X���擾����
   * 
   *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
   *  @retval        void*              �e�N�X�`���̃C���[�W�̃A�h���X
   */
  //-----------------------------------------------------------------------------
  void* GetTextureImageAddress(const s32 index_in_resource) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X�̓��e���o�͂���
   */
  //-----------------------------------------------------------------------------
  void Dump(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ���\�[�X����V�[���I�u�W�F�N�g�𐶐�����
   *
   *  @param[in]     allocator             �A���P�[�^
   *  @param[in]     device_allocator      �f�o�C�X�������̃A���P�[�^
   *                                       (allocator��device_allocator�͓������̂ł��\��Ȃ�)
   *  @param[in]     resource              �V�[���I�u�W�F�N�g�̃��\�[�X
   *  @param[in]     is_animation_enabled  �A�j���[�V�������L�����ǂ������w��
   *  @param[in]     buffer_option         �o�b�t�@�̎��
   *  @param[in]     max_anim_objects      AnimBinding�����Ă�AnimGroup���Ƃ�AnimObject�̍ő吔
   * 
   *  @retval        nw::gfx::SceneObject*     ���������V�[���I�u�W�F�N�g
   *                                           (NULL�̂Ƃ��͐������s�B���s�̗��R��
   *                                            allocator���s�����Adevice_allocator���s�����ASceneBuilder��CreateObject�Ŏ��s�������̂ǂꂩ)
   */
  //-----------------------------------------------------------------------------
  static nw::gfx::SceneObject* CreateNwSceneObject(
      gfl::heap::NwAllocator*       allocator,
      gfl::heap::NwAllocator*       device_allocator,
      nw::gfx::ResSceneObject       resource,
      bool                          is_animation_enabled   /*= true*/,
      bit32                         buffer_option          /*= nw::gfx::Model::FLAG_BUFFER_NOT_USE*/,
      s32                           max_anim_objects       /*= 0*/
  );

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         �O���t�B�b�N�X�p�̃t�@�C����\���o�C�i�����\�[�X�N���X�ւ̃|�C���^�𓾂�
   *
   *  @retval        nw::gfx::ResGraphicsFile*  �O���t�B�b�N�X�p�̃t�@�C����\���o�C�i�����\�[�X�N���X�ւ̃|�C���^(NULL�̂Ƃ��̓��[�h���Ă��Ȃ�)
   */
  //-----------------------------------------------------------------------------
  nw::gfx::ResGraphicsFile* GetNwResGraphicsFile(void) const {  return m_nw_res_graphics_file;  }

private:
  nw::gfx::ResGraphicsFile*  m_nw_res_graphics_file;
  nw::gfx::Result            m_nw_result;  // ���O��nw::gfx::res::ResGraphicsFile::Setup�̌��ʂ��o���Ă���
  bool                       m_is_self_setup;  // �����̃Z�b�g�A�b�v�����Ă�����true(���S�ɃZ�b�g�A�b�v���������Ă��邩�ۂ���m_is_setup�Ŋm�F���ĉ�����)
  bool                       m_is_setup;   // �Z�b�g�A�b�v���������Ă�����true
  Description                m_description;
  const void*                m_buffer;  // @note const���t���Ă��܂���NintendoWare��const_cast���ꏑ���������܂��B
  bool                       m_own_buffer;  // true�̂Ƃ�buffer�̊Ǘ��͂��̃C���X�^���X�ōs��
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DRESOURCE_H__
