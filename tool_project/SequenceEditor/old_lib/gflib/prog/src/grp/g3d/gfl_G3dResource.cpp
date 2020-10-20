//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dResource.cpp
 *	@brief  3D���\�[�X
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK
#include <nn/gx.h>

// NW4C
#include <nw/ut.h>
#include <nw/gfx.h>
#include <nw/anim.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Base.h>
#include <gfl_Heap.h>
#include <gfl_fs.h>
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dTexture.h>

#include "gfl_G3dErrorCheck.h"
#include <grp/g3d/gfl_G3dResource.h>


#define GFL_GRP_G3D_RESOURCE_PRINT (GFL_DEBUG && (1))  // ���ꂪ1�̂Ƃ��A�m�F�o�͂�����B���ꂪ0�̂Ƃ��A�Ȃ��B


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	�֐��I�u�W�F�N�g
//=====================================
class UnresolvedReferenceFunctor
{
public:
  UnresolvedReferenceFunctor(void){}
public:
  void operator()(nw::gfx::res::ResTexture refer)
  {
    GFL_PRINT("Texture Name=%s\n", refer.GetName());
  }
  void operator()(nw::gfx::res::ResReferenceShader refer)
  {
    GFL_PRINT("Shader Path=%s, Name=%s\n", refer.GetPath(), refer.GetName());
  }
  void operator()(nw::gfx::res::ResReferenceLookupTable refer)
  {
    GFL_PRINT("Lut Path=%s, TableName=%s\n", refer.GetPath(), refer.GetTableName());
  }
};




//-------------------------------------
///	3D���\�[�X�N���X
//=====================================

//-----------------------------------------------------------------------------
/*
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
s32 Resource::GetMaxMemberAnimSetCount(
    const u32                    array_element_num,
    const AnimationResourcePack* animation_resource_pack_array
)
{
  s32 max_count = 0;
  for( u32 i=0; i<array_element_num; ++i )
  {
    s32 count = 0;
    switch( animation_resource_pack_array[i].animation_resource_type )
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
        count = animation_resource_pack_array[i].resource->GetSkeletalMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
        count = animation_resource_pack_array[i].resource->GetMaterialMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
        count = animation_resource_pack_array[i].resource->GetVisibilityMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
        count = animation_resource_pack_array[i].resource->GetCameraMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
        count = animation_resource_pack_array[i].resource->GetLightMemberAnimSetCount(animation_resource_pack_array[i].index_in_resource);
        break;
    }
    if( count > max_count ) max_count = count;
  }
  return max_count;
}


//-----------------------------------------------------------------------------
/*
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
Resource::Resource(gfl::heap::NwAllocator* allocator, const char* path, const Description* description, bool do_setup, u32 common_resource_flag)
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && path != NULL )
  {
    Load(allocator, path, description);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃A�^�b�`
 *
 *  @param[in]     allocator    �A���P�[�^
 *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
 *                              (GetBuffer�̖߂�l��NULL�̂Ƃ��̓o�b�t�@�̃A�^�b�`���s)
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
Resource::Resource(
    gfl::heap::NwAllocator* allocator,
    const void* buffer,
    const Description* description,
    bool do_setup,
    bool own_buffer,
    u32 common_resource_flag )
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && buffer != NULL )
  {
    AttachBuffer(allocator, buffer, description, own_buffer);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
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
Resource::Resource(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile*       arc_file,
    u32                     arc_data_id,
    const Description* description,
    bool do_setup,
    u32 common_resource_flag,
    const b32 is_compress )
  : m_nw_res_graphics_file(NULL),
    m_nw_result(),
    m_is_self_setup(false),
    m_is_setup(false),
    m_description(),
    m_buffer(NULL),
    m_own_buffer(false)
{
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->AddObject(this);

  if( allocator != NULL && arc_file != NULL )
  {
    Load(allocator, arc_file, arc_data_id, description, is_compress);
    if(do_setup)
    {
      Setup(allocator, NULL, common_resource_flag);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
Resource::~Resource()
{
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
    m_nw_res_graphics_file = NULL;
  }
  if( m_buffer && m_own_buffer )
  {
    GflHeapFreeMemory( const_cast<void*>(m_buffer) );
    m_buffer = NULL;
  }
  
  GFL_SINGLETON_INSTANCE(System)->GetResourceManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
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
bool Resource::Load(gfl::heap::NwAllocator* allocator, const char* path, const Description* description)
{
  if( allocator              == NULL ) return false;
  if( path                   == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_file�Ŋm�F���Ă���̂�m_buffer�̊m�F�͗v��Ȃ��B
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();
  
  // �t�@�C�����[�h
  gfl::fs::ReadFile read_file;
  bool open_ret = read_file.Open(path);
  if( !open_ret ) return false;
  u32 size       = read_file.GetSize();
  u32 size_align = read_file.GetSizeAlign();  // �e�N�X�`�������[�h����ɂ�128Byte�A���C�����g���s���K�v������̂ŁA
  size_align     = 128;                       // �������[�h���邩�ŃA���C�����g�𕪂���K�v�����邩��
  void* buffer = GflHeapAllocMemoryAlign( heap_base, size, size_align );
  s32 read_ret = read_file.Read( buffer, size );  // size��size_align�ɍ��킹���A���C�����g�ɂ��Ă����K�v�����邩��
  read_file.Close();
  if( read_ret < 0 )
  {
    GflHeapFreeMemory(buffer);
    return false;
  }

  // �����܂ŗ����琳��ɓǂݍ��߂Ă���
  return AttachBuffer(allocator, buffer, description, true);
}

//-----------------------------------------------------------------------------
/*
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
bool Resource::Load(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile* arc_file,
    u32 arc_data_id,
    const Description* description,
    const b32 is_compress )
{

  if( allocator              == NULL ) return false;
  if( arc_file               == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_file�Ŋm�F���Ă���̂�m_buffer�̊m�F�͗v��Ȃ��B
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();

  // �t�@�C�����[�h
  size_t size;
  s32    size_align = 128;  // �e�N�X�`�������[�h����ɂ�128Byte�A���C�����g���s���K�v������̂ŁA
                            // �������[�h���邩�ŃA���C�����g�𕪂���K�v�����邩��
  void*  buffer = NULL;

  if( is_compress )
  {
    // ���k�f�[�^
    buffer = arc_file->LoadData( arc_data_id, heap_base, size_align, &size );
    if( !buffer )
    {
      return false;
    }
  }
  else
  {
    // ���k���Ă��Ȃ��f�[�^
    size = arc_file->GetDataSize( arc_data_id );
    buffer = GflHeapAllocMemoryAlign( heap_base, size, size_align );
    s32 read_ret = arc_file->LoadData( arc_data_id, buffer );
    if( read_ret < 0 )
    {
      GflHeapFreeMemory(buffer);
      return false;
    }
  }

  // �����܂ŗ����琳��ɓǂݍ��߂Ă���
  return AttachBuffer(allocator, buffer, description, true);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃A�����[�h
 *
 *  @retval        bool    false�̂Ƃ��̓A�����[�h���s�B
 *                         (���s�̗��R�̓��[�h�����f�[�^�����݂��Ȃ�)
 */
//-----------------------------------------------------------------------------
bool Resource::Unload(void)
{
  bool ret = true;
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
  }
  else
  {
    ret = false;
  }
  if( m_buffer )
  {
    GflHeapFreeMemory( const_cast<void*>(m_buffer) );
  }
  else
  {
    ret = false;
  }

  m_nw_res_graphics_file = NULL;
  m_nw_result = nw::gfx::Result();
  m_is_self_setup = false;
  m_is_setup = false;
  m_description = Description();
  m_buffer = NULL;
  m_own_buffer = false;

  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃A�^�b�`
 *                 �o�b�t�@�̃A�^�b�`�����Ńt�@�C�����̃R���e���g�̃Z�b�g�A�b�v�͂��Ȃ��B
 *
 *  @param[in]     allocator    �A���P�[�^
 *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
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
bool Resource::AttachBuffer(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description, bool own_buffer)
{
  if( allocator              == NULL ) return false;
  if( buffer                 == NULL ) return false;
  //if( m_buffer               != NULL ) return false;  m_nw_res_graphics_file�Ŋm�F���Ă���̂�m_buffer�̊m�F�͗v��Ȃ��B
  if( m_nw_res_graphics_file != NULL ) return false;

  gfl::heap::HeapBase* heap_base = allocator->GetHeapBase();
  
  // �����܂ŗ����琳��ɓǂݍ��߂Ă���
  m_nw_res_graphics_file = GFL_NEW(heap_base) nw::gfx::ResGraphicsFile(buffer);
  m_buffer               = buffer;
  m_own_buffer           = own_buffer;

  // �\�z
  if( description ) m_description = *description; 

  // ForeachTexture, ForeachIndexStream, ForeachVertexStream ��
  // ResGraphicsFile ���� ResTexture, ResIndexStream, ResVertexStream �Ɋ֐��I�u�W�F�N�g��ݒ肵�܂��B
  // �֐��I�u�W�F�N�g�� SetLocationFlag �Ń������̔z�u�ꏊ���w�肵�܂��B
  //
  m_nw_res_graphics_file->ForeachTexture(nw::gfx::TextureLocationFlagSetter(m_description.texture_location_flag));
  m_nw_res_graphics_file->ForeachIndexStream(nw::gfx::IndexStreamLocationFlagSetter(m_description.index_stream_location_flag));
  m_nw_res_graphics_file->ForeachVertexStream(nw::gfx::VertexStreamLocationFlagSetter(m_description.vertex_stream_location_flag));

  return true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C����ǂݍ��񂾃o�b�t�@�̃f�^�b�`
 *
 *  @retval        bool    false�̂Ƃ��̓o�b�t�@�̃f�^�b�`���s�B
 *                         (���s�̗��R�̓o�b�t�@���A�^�b�`�����f�[�^�����݂��Ȃ�)
 */
//-----------------------------------------------------------------------------
bool Resource::DetachBuffer(void)
{
  bool ret = true;
  if( m_nw_res_graphics_file )
  {
    nw::ut::SafeCleanup( *m_nw_res_graphics_file );
    GFL_DELETE m_nw_res_graphics_file;
  }
  else
  {
    ret = false;
  }

  GFL_ASSERT_MSG( !m_own_buffer, "���̃C���X�^���X���Ǘ����Ă���o�b�t�@���f�^�b�`���悤�Ƃ��Ă��܂��B\n" );
  if( m_buffer )
  {
    m_buffer = NULL;
  }
  else
  {
    ret = false;
  }

  m_nw_res_graphics_file = NULL;
  m_nw_result = nw::gfx::Result();
  m_is_self_setup = false;
  m_is_setup = false;
  m_description = Description();
  m_buffer = NULL;
  m_own_buffer = false;

  return ret;
}

//-----------------------------------------------------------------------------
/*
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
bool Resource::Setup(gfl::heap::NwAllocator* allocator, Resource* other_resource, u32 common_resource_flag)
{
  if( !m_is_setup )
  {
    // ���\�[�X�̃Z�b�g�A�b�v���s���܂��B���\�[�X�̃Z�b�g�A�b�v�ł͈ȉ��̂悤�ȏ������s���܂��B
    // �E�V�F�[�_�[��e�N�X�`���̎Q�Ɖ���
    // �E�e�N�X�`���Ⓒ�_��VRAM�]��
    // �E�}�e���A���̃R�}���h����
    // 
    // Setup �̖߂�l Result �� ResourceResult �ł���A�Q�Ɖ����G���[�Ȃǂ�Ԃ��܂��B
    // �G���[�R�[�h���Ԃ��Ă����ꍇ�͈����ɕK�v�� ResGraphicsFile ��^���čēx Setup ���邱�ƂŃG���[�������ł��܂��B
    // Setup ���s�������\�[�X�� Cleanup ���Ăяo���ē����Ŋm�ۂ����������Ȃǂ�j������K�v������܂��B
    //
    if(other_resource)
    {
      if( !m_is_self_setup )
      {
        m_nw_result = m_nw_res_graphics_file->Setup(allocator);
        m_is_self_setup = true;
      }
      m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(other_resource->GetNwResGraphicsFile()));
    }
    else
    {
      m_nw_result = m_nw_res_graphics_file->Setup(allocator);
      m_is_self_setup = true;
    }
    if( m_nw_result.IsSuccess() )
    {
      m_is_setup = true;
    }
    else
    {
      if( common_resource_flag )
      {
        do
        {
          Resource* common_resource;
          
          // ���ʃ��\�[�X
          {
            if( common_resource_flag & COMMON_RESOURCE_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "���ʃ��\�[�X0x%X(�r�b�g)������܂���B\n", COMMON_RESOURCE_DEFAULT_SHADER_BIT);
              }
            }
            if( common_resource_flag & COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataParticleDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "���ʃ��\�[�X0x%X(�r�b�g)������܂���B\n", COMMON_RESOURCE_PARTICLE_DEFAULT_SHADER_BIT);
              }
            }
            if( common_resource_flag & COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT )
            {
              common_resource = GFL_SINGLETON_INSTANCE(System)->GetCommonDataNwDefaultShaderResource();
              if(common_resource)
              {
                m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                if( m_nw_result.IsSuccess() )
                {
                  m_is_setup = true;
                  break;
                }
              }
              else
              {
                GFL_ASSERT_MSG(0, "���ʃ��\�[�X0x%X(�r�b�g)������܂���B\n", COMMON_RESOURCE_NW_DEFAULT_SHADER_BIT);
              }
            }
          }

          // Ex���ʃ��\�[�X
          {
            static const u32 ex_common_resource_bit_array[] =
            {
              EX_COMMON_RESOURCE_BIT_00,
              EX_COMMON_RESOURCE_BIT_01,
              EX_COMMON_RESOURCE_BIT_02,
              EX_COMMON_RESOURCE_BIT_03,
              EX_COMMON_RESOURCE_BIT_04,
              EX_COMMON_RESOURCE_BIT_05,
              EX_COMMON_RESOURCE_BIT_06,
              EX_COMMON_RESOURCE_BIT_07,
            };
            for(s32 i=0; i<sizeof(ex_common_resource_bit_array)/sizeof(ex_common_resource_bit_array[0]); ++i)
            {
              if( common_resource_flag & ex_common_resource_bit_array[i] )
              {
                common_resource =  GFL_SINGLETON_INSTANCE(System)->GetExCommonDataExCommonResource(ex_common_resource_bit_array[i]);
                if(common_resource)
                {
                  m_nw_result = m_nw_res_graphics_file->Setup(allocator, *(common_resource->GetNwResGraphicsFile()));
                  if( m_nw_result.IsSuccess() )
                  {
                    m_is_setup = true;
                    break;
                  }
                }
                else
                {
                  GFL_ASSERT_MSG(0, "���ʃ��\�[�X0x%X(�r�b�g)������܂���B\n", ex_common_resource_bit_array[i]);
                }
              }
            }
            // do{}while(0);�̍Ō�Ȃ̂ŁA�����ł�if(m_is_setup)break;�͗v��Ȃ��B
          }

        }
        while(0);
      }
    }
  }
  return m_is_setup;
}

//-----------------------------------------------------------------------------
/*
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
bool Resource::LoadAndSetup(gfl::heap::NwAllocator* allocator, const char* path, const Description* description, u32 common_resource_flag)
{
  bool ret = Load(allocator, path, description);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
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
bool Resource::LoadAndSetup(
    gfl::heap::NwAllocator* allocator,
    gfl::fs::ArcFile* arc_file,
    u32 arc_data_id,
    const Description* description,
    u32 common_resource_flag,
    const b32 is_compress )
{
  bool ret = Load(allocator, arc_file, arc_data_id, description, is_compress);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �O���t�B�b�N�X���\�[�X�t�@�C���̃A�^�b�`
 *                 �t�@�C�����̃R���e���g�̃Z�b�g�A�b�v
 *                 Attach�̌��Setup���s���B
 *
 *  @param[in]     allocator    �A���P�[�^
 *  @param[in]     buffer       �O���t�B�b�N�X���\�[�X��ǂݍ��񂾃o�b�t�@�̃A�h���X
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
bool Resource::AttachBufferAndSetup(gfl::heap::NwAllocator* allocator, const void* buffer, const Description* description, bool own_buffer, u32 common_resource_flag)
{
  bool ret = AttachBuffer(allocator, buffer, description, own_buffer);
  if( ret )
  {
    return Setup(allocator, NULL, common_resource_flag);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���\�[�X���̑S�Ă̎Q�ƃI�u�W�F�N�g�����񂵂āA�������̎Q�ƃI�u�W�F�N�g�̖��O���o�͂���
 *                 GFL_DEBUG��0�̂Ƃ��͉������܂���B
 */
//-----------------------------------------------------------------------------
void Resource::PrintUnresolvedReference(void) const
{
#if GFL_DEBUG
  //GFL_PRINT("Resource::PrintUnresolvedReference-----------\n");
  if( !m_is_setup )
  {
    int result_description = m_nw_result.GetDescription();
    GFL_PRINT("Result Code=%d, Description=%d\n", m_nw_result.GetCode(), result_description );
  
    // m_nw_res_graphics���Ƀ��f��������ꍇ�A����ɖ������̎Q�Ƃ�����ꍇ�A���̎|���o�͂���B  // @note �Q�ƃe�[�u�����g�p����}�e���A���A���C�g���Ώۂɉ��������B
    {
      struct ResultDescriptionPack
      {
        int bit_flag;
        const char* message;
      };
      static const ResultDescriptionPack result_description_pack[] =
      {
        { nw::gfx::RESOURCE_RESULT_OK,                                "Setup �������������Ƃ�\���܂��B" },
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_TEXTURE,                 "�e�N�X�`���Q�Ɖ��������s�������Ƃ�\���܂��B" },
                                                                      // ���̃G���[���o���̂�gfx_ResTextureMapper.cpp��anim_ResAnim.cpp
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_SHADER,                  "�V�F�[�_�[�̎Q�Ɖ��������s�������Ƃ�\���܂��B" },
        { nw::gfx::RESOURCE_RESULT_NOT_FOUND_LUT,                     "�Q�ƃe�[�u���̎Q�Ɖ��������s�������Ƃ�\���܂��B" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_LOCATION_LUT,           "�Q�ƃe�[�u���̗L���t���O�Ǝ��ۂɎQ�ƃe�[�u������v���Ȃ����Ƃ�\���܂��B" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_LOCATION_SHADER_SYMBOL, "�V�F�[�_�[�V���{�����V�F�[�_�[�o�C�i���ɑ��݂��Ȃ����Ƃ�\���܂��B" },
        { nw::gfx::RESOURCE_RESULT_IRRELEVANT_TEXTURE_MAPPING_METHOD, "�J�����L���[�u���W�̃}�b�s���O���@�ŃL���[�u�e�N�X�`�������݂��Ȃ����Ƃ�\���܂��B" },
      };
      s32 pack_num = sizeof(result_description_pack)/sizeof(result_description_pack[0]);
      for( s32 i=0; i<pack_num; ++i )
      {
        if(result_description & result_description_pack[i].bit_flag) GFL_PRINT("%s\n", result_description_pack[i].message);
      }

      // ���f��
      {
        s32 count = m_nw_res_graphics_file->GetModelsCount();
        for( s32 i=0; i<count; ++i )
        {
          nw::gfx::res::ResModel res_model = m_nw_res_graphics_file->GetModels(i);
          ec::ErrorCheck::CheckModelTextureReference(res_model);
          ec::ErrorCheck::CheckModelShaderReference(res_model);
          ec::ErrorCheck::CheckModelLookupTableReference(res_model);
        }
      }
    }

    // m_nw_res_graphics���Ƀe�N�X�`���A�V�F�[�_�A�Q�ƃe�[�u��������ꍇ�A���ꂪ�������̎Q�ƃI�u�W�F�N�g�Ȃ�A���̎|���o�͂���B
    {
      m_nw_res_graphics_file->ForeachUnresolvedReferenceTexture(UnresolvedReferenceFunctor());
      m_nw_res_graphics_file->ForeachUnresolvedReferenceShader(UnresolvedReferenceFunctor());
      m_nw_res_graphics_file->ForeachUnresolvedReferenceLut(UnresolvedReferenceFunctor());
    }
  }
  //GFL_PRINT("---------------------------------------------\n");
#endif
}

//-----------------------------------------------------------------------------
/*
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
void Resource::DeleteImageBlockData( gfl::heap::NwAllocator* allocator )
{
  GFL_ASSERT_MSG(m_is_self_setup, "Setup���������Ă��܂���B\n");  //@check �����N����ĂȂ����\�[�X�����邩������Ȃ����o��

  u32 start_addr;
  u32 image_addr;
  u32 header_size;
  u32 file_size;

  start_addr = reinterpret_cast<u32>(m_buffer);
  image_addr = reinterpret_cast<u32>(m_nw_res_graphics_file->GetImageBlockData());

  header_size = image_addr - start_addr;
  file_size = m_nw_res_graphics_file->GetFileSize();

#if GFL_GRP_G3D_RESOURCE_PRINT
  GFL_PRINT( "[DeleteImageBlockData] header_size=0x%x file_size=0x%x\n", header_size, file_size );
#endif

  allocator->GetHeapBase()->ResizeMemory( const_cast<void*>(m_buffer), header_size );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���f�����\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    ���f�����\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetModelsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetModelsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �J�������\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �J�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetCamerasCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetCamerasCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���C�g���\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    ���C�g���\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLightsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLightsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �t�H�O���\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �t�H�O���\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetFogsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetFogsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ݒ胊�\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �V�[�����ݒ胊�\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetSceneEnvironmentSettingsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetSceneEnvironmentSettingsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �X�P���^���A�j���[�V�������\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    �X�P���^���A�j���[�V�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetSkeletalAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetSkeletalAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �}�e���A���A�j���[�V�������\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    �}�e���A���A�j���[�V�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetMaterialAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetMaterialAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �r�W�r���e�B�A�j���[�V�������\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    �r�W�r���e�B�A�j���[�V�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetVisibilityAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetVisibilityAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �J�����A�j���[�V�������\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    �J�����A�j���[�V�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetCameraAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetCameraAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���C�g�A�j���[�V�������\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    ���C�g�A�j���[�V�������\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLightAnimsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLightAnimsCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`�����\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �e�N�X�`�����\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetTexturesCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetTexturesCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�F�[�_�[���\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �V�F�[�_�[���\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetShadersCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetShadersCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �G�~�b�^�[���\�[�X�̗v�f�����擾����
 * 
 *  @retval        s32    �G�~�b�^�[���\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetEmittersCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetEmittersCount();
  else
    return 0;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���b�N�A�b�v�e�[�u���Z�b�g���\�[�X�̗v�f�����擾����
 *
 *  @retval        s32    ���b�N�A�b�v�e�[�u���Z�b�g���\�[�X�̗v�f��
 */
//-----------------------------------------------------------------------------
s32                 Resource::GetLutSetsCount() const
{
  if( m_nw_res_graphics_file )
    return m_nw_res_graphics_file->GetLutSetsCount();
  else
    return 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���\�[�X�̃C���f�b�N�X�ԍ����擾����
 * 
 *  @param[in]     key  ���\�[�X�̖��O
 * 
 *  @retval        s32    ���\�[�X�̃C���f�b�N�X�ԍ�
 */
//-----------------------------------------------------------------------------
s32 Resource::GetModelsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetModelsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetCamerasIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetCamerasIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLightsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLightsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetFogsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetFogsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetSceneEnvironmentSettingsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetSceneEnvironmentSettingsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetSkeletalAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetSkeletalAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetMaterialAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetMaterialAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetVisibilityAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetVisibilityAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetCameraAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetCameraAnimsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLightAnimsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLightAnimsIndex(key);
    if( i >= 0 ) return i;
  }
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetTexturesIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetTexturesIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetShadersIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetShadersIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetEmittersIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetEmittersIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}
s32 Resource::GetLutSetsIndex(const char* key) const
{
  if( m_nw_res_graphics_file )
  {
    s32 i = m_nw_res_graphics_file->GetLutSetsIndex(key);
    if( i >= 0 ) return i;
  } 
  return RESOURCE_INDEX_NOT_FOUND;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �����o�[�A�j���[�V�����̗v�f�����擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 * 
 *  @retval        s32    �����o�[�A�j���[�V�����̗v�f��
 */
//-----------------------------------------------------------------------------
s32 Resource::GetSkeletalMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetMaterialMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetVisibilityMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetCameraMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}
s32 Resource::GetLightMemberAnimSetCount(const s32 index_in_resource) const
{
  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);
    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetCount();
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�������\�[�X���擾����
 * 
 *  @param[in]     type               �A�j���[�V�������\�[�X�̃^�C�v
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[out]    res_anim           �擾�����A�j���[�V�������\�[�X�̑����
 */
//-----------------------------------------------------------------------------
void Resource::GetResAnim(const AnimationResourceType type, const s32 index_in_resource, ResAnim* res_anim) const
{
  if( m_nw_res_graphics_file )
  {  
    nw::anim::ResAnim res;
    
    switch( type )
    {
    case ANIMATION_RESOURCE_TYPE_NONE:
      {
        // �������Ȃ�
      }
      break;
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      {
        res = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      {
        res = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      {
        res = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      {
        res = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      {
        res = m_nw_res_graphics_file->GetLightAnims(index_in_resource);
      }
      break;
    case ANIMATION_RESOURCE_TYPE_ALL:
    default:
      {
        GFL_ASSERT_MSG( 0, "�A�j���[�V�������\�[�X�̃^�C�v%d���s���ł��B\n", type );
      }
      break;
    }

    res_anim->SetNwResAnim(type, res);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "m_nw_res_graphics_file��NULL�ł��B\n" );
  }
}
 

#if GFL_DEBUG
//-----------------------------------------------------------------------------
/*
 *  @brief    �A�j���[�V�����̓��e�m�F
 */
//-----------------------------------------------------------------------------
void Resource::DEBUG_PrintAnimResult(const AnimationResourceType type, const s32 index_in_resource)
{
  GFL_PRINT("Resource::DEBUG_PrintAnimResult\n");

  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim;
    switch(type)
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);  break;
    }

    if( res_anim.IsValid() )
    {
      b32 full_baked_anim = res_anim.IsFullBakedAnim();
      s32 member_anim_set_count = res_anim.GetMemberAnimSetCount();
      f32 frame_size = res_anim.GetFrameSize();
      const char* target_anim_group_name = res_anim.GetTargetAnimGroupName();

      GFL_PRINT("full_baked_anim=%d\n", full_baked_anim);
      GFL_PRINT("member_anim_set_count=%d\n", member_anim_set_count);
      GFL_PRINT("frame_size=%f\n", frame_size);
      if( target_anim_group_name ) GFL_PRINT("target_anim_group_name=%s\n", target_anim_group_name);
      
      for( s32 i=0; i<member_anim_set_count; ++i )
      {
        nw::anim::res::ResMemberAnim res_member_anim = res_anim.GetMemberAnimSet(i);
        
        if( res_member_anim.IsValid() )
        {
          GFL_PRINT("member_anim_set_index[%d]\n", i);

          u32 flags = res_member_anim.GetFlags();
          const char* path = res_member_anim.GetPath();
          u32 primitive_type = res_member_anim.GetPrimitiveType();

          GFL_PRINT("flags=%d\n", flags);
          if( path ) GFL_PRINT("path=%s\n", path);
          GFL_PRINT("primitive_type=%d\n", primitive_type);

          if(    primitive_type != nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM
              && primitive_type != nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED )
          {
            // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
            // ResMemberAnim::GetPrimitiveSize
            // ResMemberAnim::EvaluateResultForType
            // ��
            // nw::anim::res::ResMemberAnim::PrimitiveType
            // �ɂ���Ă�NW_ASSERT�ɂЂ��������Ă��܂��̂�
            // �Ђ�������PrimitiveType�ł͎g��Ȃ��悤�ɂ��Ă����B

            s32 primitive_size = res_member_anim.GetPrimitiveSize();
            
            GFL_PRINT("primitive_size=%d\n", primitive_size);

            static s32 originalValueNone = 0;
 
            void* dstBuf        = NULL;
            bit32 dstFlags      = 0;
            void* originalValue = &originalValueNone;
 
            // NintendoWare/CTR/include/nw/anim/res/anim_ResAnim.h
            // NintendoWare/CTR/sources/libraries/anim/res/anim_ResAnim.cpp
            // ���Q�l�ɂ����B
            switch(primitive_type)
            {
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(float), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static float dstBufFloat;
                dstBuf = &dstBufFloat;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(s32), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static s32 dstBufInt;
                dstBuf = &dstBufInt;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(bool), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static bool dstBufBool;
                dstBuf = &dstBufBool;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::math::VEC2), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static nw::math::VEC2 dstBufVector2;
                dstBuf = &dstBufVector2;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::math::VEC3), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static nw::math::VEC3 dstBufVector3;
                dstBuf = &dstBufVector3;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::gfx::CalculatedTransform), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                // ���̌^�̂�OriginalValue���K�v
                static nw::gfx::CalculatedTransform dstBufTransform;
                static nw::math::Transform3         originalValueTransform(gfl::math::VEC3(1.0f,1.0f,1.0f), gfl::math::VEC3(0.0f,0.0f,0.0f), gfl::math::VEC3(0.0f,0.0f,0.0f));
                dstBuf = &dstBufTransform;
                originalValue = &originalValueTransform;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::ut::ResFloatColor), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static nw::ut::ResFloatColor dstBufRgbaColor;  // NintendoWare/CTR/include/nw/ut/ut_Color.h
                dstBuf = &dstBufRgbaColor;
              }
              break;
            case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE:
              {
                GFL_ASSERT_MSG(primitive_size==sizeof(nw::ut::Offset), "���C�u�����̌^���ύX���ꂽ�\��������܂��B\n");  //@check �f�o�b�O�p�֐���
                static nw::ut::Offset dstBufTexture;  // NintendoWare/CTR/include/nw/ut/ut_ResType.h
                dstBuf = &dstBufTexture;
              }
              break;
            default:
              {
                GFL_ASSERT_MSG(0, "primitive_type=%d�͕s���ȃ^�C�v�ł��B\n", primitive_type);
              }
              break;
            }
 
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              dstFlags = res_member_anim.EvaluateResultForType(
                  dstBuf,
                  dstFlags,
                  static_cast<f32>(frame_s32),
                  originalValue
              );
 
              switch(primitive_type)
              {
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FLOAT:
                {
                  float* dstBufFloat = reinterpret_cast<float*>(dstBuf);
                  GFL_PRINT("Frame=%3d FLOAT [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufFloat
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_INT:
                {
                  s32* dstBufInt = reinterpret_cast<s32*>(dstBuf);
                  GFL_PRINT("Frame=%3d INT [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufInt
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BOOL:
                {
                  bool* dstBufBool = reinterpret_cast<bool*>(dstBuf);
                  GFL_PRINT("Frame=%3d BOOL [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      *dstBufBool
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR2:
                {
                  nw::math::VEC2* dstBufVector2 = reinterpret_cast<nw::math::VEC2*>(dstBuf);
                  GFL_PRINT("Frame=%3d VECTOR2 [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufVector2->x,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufVector2->y
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_VECTOR3:
                {
                  nw::math::VEC3* dstBufVector3 = reinterpret_cast<nw::math::VEC3*>(dstBuf);
                  GFL_PRINT("Frame=%3d VECTOR3 [%d]%f, [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufVector3->x,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufVector3->y,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2,
                      dstBufVector3->z
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
                {
                  nw::gfx::CalculatedTransform* dstBufTransform = reinterpret_cast<nw::gfx::CalculatedTransform*>(dstBuf);
                 
                  gfl::math::VEC3          translate;               dstBufTransform->GetTranslate(&translate);
                  const gfl::math::VEC3&   scale                    = dstBufTransform->Scale();
                  nw::math::VEC3&          direct_scale             = dstBufTransform->DirectScale();  // gfl::math::VEC3&����
                      // a reference of type "gfl::math::VEC3 &" (not const-qualified) cannot be initialized with a value of type "nn::math::VEC3"
                      // �Ƃ����R���p�C���G���[�ɂȂ��Ă��܂��B
                  const gfl::math::MTX34&  transform_matrix         = dstBufTransform->TransformMatrix();
                  gfl::math::MTX34&        direct_transform_matrix  = dstBufTransform->DirectTransformMatrix();
                  
                  GFL_PRINT("Frame=%3d TRANSFORM [0x%X] Translate(%f,%f,%f),\n",
                      frame_s32,
                      dstFlags,
                      translate.x, translate.y, translate.z
                  );
                  GFL_PRINT("Scale(%f,%f,%f), DirectScale(%f,%f,%f)\n",
                      scale.x, scale.y, scale.z,
                      direct_scale.x, direct_scale.y, direct_scale.z
                  );
                  GFL_PRINT("TransformMatirx\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                      transform_matrix.f._00, transform_matrix.f._01, transform_matrix.f._02, transform_matrix.f._03,
                      transform_matrix.f._10, transform_matrix.f._11, transform_matrix.f._12, transform_matrix.f._13,
                      transform_matrix.f._20, transform_matrix.f._21, transform_matrix.f._22, transform_matrix.f._23
                  );
                  GFL_PRINT("DirectTransformMatirx\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                      direct_transform_matrix.f._00, direct_transform_matrix.f._01, direct_transform_matrix.f._02, direct_transform_matrix.f._03,
                      direct_transform_matrix.f._10, direct_transform_matrix.f._11, direct_transform_matrix.f._12, direct_transform_matrix.f._13,
                      direct_transform_matrix.f._20, direct_transform_matrix.f._21, direct_transform_matrix.f._22, direct_transform_matrix.f._23
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
                {
                  nw::ut::ResFloatColor* dstBufRgbaColor = reinterpret_cast<nw::ut::ResFloatColor*>(dstBuf);  // NintendoWare/CTR/include/nw/ut/ut_Color.h
                  GFL_PRINT("Frame=%3d RGBA_COLOR [%d]%f, [%d]%f, [%d]%f, [%d]%f\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufRgbaColor->r,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT1,
                      dstBufRgbaColor->g,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT2,
                      dstBufRgbaColor->b,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT3,
                      dstBufRgbaColor->a
                  );
                }
                break;
              case nw::anim::res::ResMemberAnim::PRIMITIVETYPE_TEXTURE:
                {
                  nw::ut::Offset* dstBufTexture = reinterpret_cast<nw::ut::Offset*>(dstBuf);  // NintendoWare/CTR/include/nw/ut/ut_ResType.h
                  GFL_PRINT("Frame=%3d TEXTURE [%d]%d\n",
                      frame_s32,
                      dstFlags&nw::anim::AnimResult::FLAG_VALID_COMPONENT0,
                      dstBufTexture->offset
                  );
                }
                break;
              default:
                {
                  GFL_ASSERT_MSG(0, "primitive_type=%d�͕s���ȃ^�C�v�ł��B\n", primitive_type);
                }
                break;
              }
            }  // for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
          }  // if(    primitive_type != 
          else if( primitive_type == nw::anim::res::ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM )
          {
            // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
            // nw::ut::ResDynamicCast
            // ���Q�l�ɂ����B
            nw::anim::res::ResBakedTransformAnim res_baked_transform_anim(res_member_anim.ptr());  
            
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              b32 scale_exist = false;
              b32 rotate_exist = false;
              b32 translate_exist = false;

              nw::math::VEC3  scale;
              nw::math::MTX34 rotate;
              nw::math::MTX34 translate;
              bit32 scale_flags = 0;
              bit32 rotate_flags = 0;
              bit32 translate_flags = 0;

              if( res_baked_transform_anim.GetScale() )
              {
                scale_exist = true;
                res_baked_transform_anim.EvaluateScale(&scale, &scale_flags, static_cast<f32>(frame_s32));
              }
              if( res_baked_transform_anim.GetRotate() )
              {
                rotate_exist = true;
                res_baked_transform_anim.EvaluateRotate(&rotate, &rotate_flags, static_cast<f32>(frame_s32));
              }
              if( res_baked_transform_anim.GetTranslate() )
              {
                translate_exist = true;
                res_baked_transform_anim.EvaluateTranslate(&translate, &translate_flags, static_cast<f32>(frame_s32));
              }

              GFL_PRINT("Frame=%3d BAKED_TRANSFORM,\n",
                  frame_s32
              );
              GFL_PRINT("Scale exist=%d [0x%X] (%f,%f,%f)\n",
                  scale_exist,
                  scale_flags,
                  scale.x, scale.y, scale.z
              );
              GFL_PRINT("Rotate exist=%d [0x%X]\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  rotate_exist,
                  rotate_flags,
                  rotate.f._00, rotate.f._01, rotate.f._02, rotate.f._03,
                  rotate.f._10, rotate.f._11, rotate.f._12, rotate.f._13,
                  rotate.f._20, rotate.f._21, rotate.f._22, rotate.f._23
              );
              GFL_PRINT("Translate exist=%d [0x%X]\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  translate_exist,
                  translate_flags,
                  translate.f._00, translate.f._01, translate.f._02, translate.f._03,
                  translate.f._10, translate.f._11, translate.f._12, translate.f._13,
                  translate.f._20, translate.f._21, translate.f._22, translate.f._23
              );
            }
          }  // else if( primitive_type != 
          else if( primitive_type == nw::anim::res::ResMemberAnim::PRIMITIVETYPE_FULL_BAKED )
          {
            // NintendoWare/CTR/include/nw/ut/ut_ResUtil.h
            // nw::ut::ResDynamicCast
            // ���Q�l�ɂ����B
            nw::anim::res::ResFullBakedAnim res_full_baked_anim(res_member_anim.ptr());
            
            s32 frame_size_s32 = static_cast<s32>(frame_size);
            for( s32 frame_s32=0; frame_s32<frame_size_s32; ++frame_s32 )
            {
              nw::math::MTX34 transform;
              res_full_baked_anim.EvaluateTransform(&transform, static_cast<f32>(frame_s32));

              GFL_PRINT("Frame=%3d FULL_BAKED,\n",
                  frame_s32
              );
              GFL_PRINT("Transform\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n""(%f,%f,%f,%f)\n",
                  transform.f._00, transform.f._01, transform.f._02, transform.f._03,
                  transform.f._10, transform.f._11, transform.f._12, transform.f._13,
                  transform.f._20, transform.f._21, transform.f._22, transform.f._23
              );
            }
          }  // else if( primitive_type != 
        }  // if( res_member_anim.IsValid() )
      }  // for( s32 i=0; i<member_anim_set_count; ++i )
    }  // if( res_anim.IsValid() )
  }  // if( m_nw_res_graphics_file )
}

s32 Resource::DEBUG_GetMemberAnimSetIndex(const AnimationResourceType type, const s32 index_in_resource, const char* key) const
{
  GFL_PRINT("Resource::DEBUG_GetMemberAnimSetIndex\n");

  enum
  {
    MEMBER_ANIM_SET_INDEX_NOT_FOUND  = -1  // �w�肵�����O���������o�[�A�j���[�V���������݂��Ȃ��ꍇ�̃C���f�b�N�X
  };

  if( m_nw_res_graphics_file )
  {
    nw::anim::res::ResAnim res_anim;
    switch(type)
    {
    case ANIMATION_RESOURCE_TYPE_SKELETAL:
      res_anim = m_nw_res_graphics_file->GetSkeletalAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_MATERIAL:
      res_anim = m_nw_res_graphics_file->GetMaterialAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_VISIBILITY:
      res_anim = m_nw_res_graphics_file->GetVisibilityAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_CAMERA:
      res_anim = m_nw_res_graphics_file->GetCameraAnims(index_in_resource);  break;
    case ANIMATION_RESOURCE_TYPE_LIGHT:
      res_anim = m_nw_res_graphics_file->GetLightAnims(index_in_resource);  break;
    }

    if( res_anim.IsValid() )
    {
      return res_anim.GetMemberAnimSetIndex(key);
    }
  }

  return MEMBER_ANIM_SET_INDEX_NOT_FOUND;
}
#endif


// @note gfl::grp::g3d::Resource�N���X����剻����̂�h�����߂ɂ��A�e�N�X�`���̏��̎擾��gfl::grp::g3d::Texture�N���X��
// �n���Ă���s�����ق���������������Ȃ�
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`���̍���(�c�̒���)���擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @retval        s32                �e�N�X�`���̍���(�c�̒���)
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureHeight(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  return nw_res_image_texture.GetHeight();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`���̕�(���̒���)���擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @retval        s32                �e�N�X�`���̕�(���̒���)
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureWidth(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  return nw_res_image_texture.GetWidth();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`���̃t�H�[�}�b�g���擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @retval        s32                gfl::grp::g3d::Texture::Format
 */
//-----------------------------------------------------------------------------
s32 Resource::GetTextureFormat(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedTexture::FormatHW format = static_cast<nw::gfx::res::ResPixelBasedTexture::FormatHW>(nw_res_image_texture.GetFormatHW());
  return (static_cast<gfl::grp::g3d::Texture::Format>(format));
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`����1�s�N�Z��������̃r�b�g�����擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @retval        u32                �e�N�X�`����1�s�N�Z��������̃r�b�g��
 */
//-----------------------------------------------------------------------------
u32 Resource::GetTextureBitsPerPixel(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedImage nw_res_pixel_based_image = nw_res_image_texture.GetImage();
  return nw_res_pixel_based_image.GetBitsPerPixel();
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �e�N�X�`���̃C���[�W�̃A�h���X���擾����
 * 
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @retval        void*              �e�N�X�`���̃C���[�W�̃A�h���X
 */
//-----------------------------------------------------------------------------
void* Resource::GetTextureImageAddress(const s32 index_in_resource) const
{
  nw::gfx::res::ResTexture      nw_res_texture       = m_nw_res_graphics_file->GetTextures(index_in_resource);
  nw::gfx::res::ResImageTexture nw_res_image_texture = nw::ut::ResDynamicCast<nw::gfx::res::ResImageTexture>(nw_res_texture);
  nw::gfx::res::ResPixelBasedImage nw_res_pixel_based_image = nw_res_image_texture.GetImage();
  return (reinterpret_cast<void*>(nw_res_pixel_based_image.GetImageAddress()));
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���\�[�X�̓��e���o�͂���
 */
//-----------------------------------------------------------------------------
void Resource::Dump(void) const
{
#if GFL_DEBUG
  if( m_nw_res_graphics_file )
  {
#define PRINT_COUNT(Item) GFL_PRINT("%s=%d\n", #Item, m_nw_res_graphics_file->Get##Item##Count());
    PRINT_COUNT(Models)
    PRINT_COUNT(Textures)
    PRINT_COUNT(LutSets)
    PRINT_COUNT(Shaders)
    PRINT_COUNT(Cameras)
    PRINT_COUNT(Lights)
    PRINT_COUNT(Fogs)
    PRINT_COUNT(SceneEnvironmentSettings)
    PRINT_COUNT(SkeletalAnims)
    PRINT_COUNT(MaterialAnims)
    PRINT_COUNT(VisibilityAnims)
    PRINT_COUNT(CameraAnims)
    PRINT_COUNT(LightAnims)
    PRINT_COUNT(FogAnims)
    PRINT_COUNT(Emitters)
#undef PRINT_COUNT
  }
#endif  // #if GFL_DEBUG
}

//-----------------------------------------------------------------------------
/*
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
nw::gfx::SceneObject* Resource::CreateNwSceneObject(
    gfl::heap::NwAllocator*       allocator,
    gfl::heap::NwAllocator*       device_allocator,
    nw::gfx::ResSceneObject       resource,
    bool                          is_animation_enabled,
    bit32                         buffer_option,
    s32                           max_anim_objects
)
{
  if(    allocator         == NULL
      || device_allocator  == NULL )
  {
    return NULL;
  }

  nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
      .Resource(resource)
      .IsFixedSizeMemory(true)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .BufferOption(buffer_option)
      .SharedMaterialModel(NULL)
      .SharedMeshNodeVisibiltiyModel(NULL)
      .MaxAnimObjectsPerGroup(max_anim_objects)
      .ParticleSetMarginCount(0)
      .IsAnimationEnabled(is_animation_enabled)
      .CreateObject(allocator, device_allocator);

  return scene_object;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

