//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSystem.cpp
 *	@brief  3D�V�X�e��
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

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dTexture.h>

#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/g3d_common.gaix>

#include <ro/gfl_RoManager.h>

//-----------------------------------------------------------------------------
/**
*    Singleton�C���X�^���X�̎��̂��`
*/
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::grp::g3d::System);
template class gfl::base::SingletonAccessor<gfl::grp::g3d::System>;
#endif

namespace gfl {
namespace grp {
namespace g3d {


static const char* GFL_GRP_G3D_DEFAULT_SHADER_FILE_NAME          = "rom:/""g3d_common/gfl_DefaultShader.bcsdr";
static const char* GFL_GRP_G3D_H3D_DEFAULT_SHADER_FILE_NAME      = "rom:/""g3d_common/gfl_DefaultShader.bchsdr";

static const char* GFL_GRP_G3D_PARTICLE_DEFAULT_SHADER_FILE_NAME = "rom:/""g3d_common/nwgfx_ParticleDefaultShader.bcsdr";
static const char* GFL_GRP_G3D_NW_DEFAULT_SHADER_FILE_NAME       = "rom:/""g3d_common/nwgfx_DefaultShader.bcsdr";

//�ǂݍ���ł��Ȃ��̂�arc������폜���Ă悢
static const char* GFL_GRP_G3D_SMALL_TEXTURE_FILE_NAME           = "rom:/""g3d_common/small_texture.bctex";  // L4 8x8 ����F�̃e�N�X�`��
static const char* GFL_GRP_G3D_H3D_SMALL_TEXTURE_FILE_NAME       = "";  // L4 8x8 ����F�̃e�N�X�`��



//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

// ���[�U�[�w�胊�\�[�X
void * System::m_pStaticResourceTbl[System::RESOURCE_MAX] = {NULL};


//-------------------------------------
///	�O���w�胊�\�[�X�e�[�u��
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  g3d���C�u�����Ŏg�p���郊�\�[�X���O���w��
 */
//-----------------------------------------------------------------------------
void System::SetStaticResource( void * buff, ResourceEnum resource_type )
{
  if( resource_type < RESOURCE_MAX ){
    m_pStaticResourceTbl[ resource_type ] = buff;
  }
  else{
    GFL_ASSERT_MSG( resource_type < RESOURCE_MAX, "resource_type orver  type=%d  Max = %d\n", resource_type, RESOURCE_MAX );
  }
}
// �擾
void* System::GetStaticResource( ResourceEnum resource_type )
{
  if( resource_type < RESOURCE_MAX ){
    return m_pStaticResourceTbl[ resource_type ];
  }
  else{
    GFL_ASSERT_MSG( resource_type < RESOURCE_MAX, "resource_type orver  type=%d  Max = %d\n", resource_type, RESOURCE_MAX );
  }
  return NULL;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �O���w�肳�ꂽg3d���C�u�����Ŏg�p���郊�\�[�X��ێ��Ă���e�[�u�������ׂăN���A
 *	*�������̉���͊O���ŁB
 */
//-----------------------------------------------------------------------------
void System::ClearStaticResourceAll( void )
{
  for( u32 i=0; i<RESOURCE_MAX; ++i ){
    m_pStaticResourceTbl[i] = NULL;
  }
}




//-------------------------------------
///	3D�V�X�e���N���X
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *
 *  @param[in]     heap_allocator    �q�[�v�������̃A���P�[�^
 *  @param[in]     device_allocator  �f�o�C�X�������̃A���P�[�^
 *  @param[in]     description       �ݒ���e
 */
//-----------------------------------------------------------------------------
System::System( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description )
  : m_scene_environment_setting_manager(NULL),
    m_scene_manager(NULL),
    m_model_manager(NULL),
    m_particle_manager(NULL),
    m_texture_manager(NULL),
    m_fog_manager(NULL),
    m_camera_manager(NULL),
    m_stereo_camera_manager(NULL),
    m_light_manager(NULL),
    m_res_anim_manager(NULL),
    m_animation_manager(NULL),
    m_unit_animation_manager(NULL),
    m_resource_manager(NULL),
    m_h3d_model_manager(NULL),
    m_is_initialized(false),
    m_auto_common_data_deal_bit(AUTO_COMMON_DATA_DEAL_BIT_NONE),
    m_small_texture_buffer(NULL),
    m_default_shader_resource(NULL),
    m_particle_default_shader_resource(NULL),
    m_nw_default_shader_resource(NULL),
    m_small_texture(NULL),
#if GFL_GRP_G3D_H3D_USE
    m_h3d_default_shader_buffer(NULL),
    m_h3d_default_shader_resource(NULL),
    m_h3d_small_texture(NULL),
    m_h3d_model_memory_manager(NULL),
#endif  // GFL_GRP_G3D_H3D_USE
    m_common_data_head_elem(NULL)
{
  //GFL_ASSERT_STOP_MSG( heap_allocator != NULL && device_allocator != NULL, "�������s���ł��B\n" );  // klocwork�ɕ����������̂ŃR�����g�A�E�g���A�`�F�b�N�̎������L�̒ʂ�ɂ����B
  GFL_ASSERT_STOP_MSG( heap_allocator, "�������s���ł��B\n" );
  GFL_ASSERT_STOP_MSG( device_allocator, "�������s���ł��B\n" );

  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  gfl::heap::HeapBase* heap_base = heap_allocator->GetHeapBase();

  m_scene_environment_setting_manager = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.scene_environment_setting_object_num_max, OBJECT_TYPE_SCENE_ENVIRONMENT_SETTING);
  m_scene_manager                     = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.scene_object_num_max,                     OBJECT_TYPE_SCENE);
  m_model_manager                     = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.model_object_num_max,                     OBJECT_TYPE_MODEL);
  m_particle_manager                  = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.particle_object_num_max,                  OBJECT_TYPE_PARTICLE);
  m_texture_manager                   = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.texture_object_num_max,                   OBJECT_TYPE_TEXTURE);
  m_fog_manager                       = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.fog_object_num_max,                       OBJECT_TYPE_FOG);
  m_camera_manager                    = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.camera_object_num_max,                    OBJECT_TYPE_CAMERA);
  m_stereo_camera_manager             = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.stereo_camera_object_num_max,             OBJECT_TYPE_STEREO_CAMERA);
  m_light_manager                     = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.light_object_num_max,                     OBJECT_TYPE_LIGHT);
  m_res_anim_manager                  = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.res_anim_object_num_max,                  OBJECT_TYPE_RES_ANIM);
  m_animation_manager                 = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.animation_object_num_max,                 OBJECT_TYPE_ANIMATION);
  m_unit_animation_manager            = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.unit_animation_object_num_max,            OBJECT_TYPE_UNIT_ANIMATION);
  m_resource_manager                  = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.resource_object_num_max,                  OBJECT_TYPE_RESOURCE);
  m_h3d_model_manager                 = GFL_NEW(heap_base) ObjectManager (heap_base, l_description.h3d_model_object_num_max,                 OBJECT_TYPE_H3D_MODEL);


#if GFL_GRP_G3D_H3D_USE
  m_h3d_model_memory_manager          = GFL_NEW(heap_base) H3dModelMemoryManager();
  m_h3d_model_memory_manager->Initialize( heap_base, l_description.h3d_model_object_num_max );
#endif // GFL_GRP_G3D_H3D_USE

  initialize(heap_allocator, device_allocator);

  // AutoCommonDataDeal
  {
    switch(l_description.auto_common_data_deal)
    {
    case AUTO_COMMON_DATA_DEAL_NONE:
      m_auto_common_data_deal_bit = AUTO_COMMON_DATA_DEAL_BIT_NONE;
      break;
    case AUTO_COMMON_DATA_DEAL_ALL:
      m_auto_common_data_deal_bit = 
          AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          
        | AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE 
        | AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       
        | AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE
#if GFL_GRP_G3D_H3D_USE
        | AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE
        | AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE
#endif  // GFL_GRP_G3D_H3D_USE
        ;
      break;
    }
    
    autoCreateCommonData(heap_allocator, device_allocator);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
System::~System()  // �p���͍l���Ă��Ȃ�
{
  // AutoCommonDataDeal
  {
    autoDestroyCommonData();
    m_auto_common_data_deal_bit = AUTO_COMMON_DATA_DEAL_BIT_NONE;
  }

  finalize();

#if GFL_GRP_G3D_H3D_USE
  GFL_DELETE  m_h3d_model_memory_manager;
#endif // GFL_GRP_G3D_H3D_USE
  GFL_DELETE m_h3d_model_manager;
  GFL_DELETE m_resource_manager;
  GFL_DELETE m_unit_animation_manager;
  GFL_DELETE m_animation_manager;
  GFL_DELETE m_res_anim_manager;
  GFL_DELETE m_light_manager;
  GFL_DELETE m_stereo_camera_manager;
  GFL_DELETE m_camera_manager;
  GFL_DELETE m_fog_manager;
  GFL_DELETE m_texture_manager;
  GFL_DELETE m_particle_manager;
  GFL_DELETE m_model_manager;
  GFL_DELETE m_scene_manager;
  GFL_DELETE m_scene_environment_setting_manager;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �X�V����  �iScene��Calculate��O�ɓǂ�ł��������j
 */
//-----------------------------------------------------------------------------
void System::Update(void)
{
#if GFL_GRP_G3D_H3D_USE
  m_h3d_model_memory_manager->Update();
#endif  // GFL_GRP_G3D_H3D_USE
}


//-----------------------------------------------------------------------------
/*
 *  @brief        �`���������������
 */
//-----------------------------------------------------------------------------
#if 0
//�g�p�֎~�ɂ��Ă���
void System::ResetRenderState(void)
{
  u32 scene_num = m_scene_manager->GetObjectNum();
  for( u32 i=0; i<scene_num; ++i )
  {
    Scene* scene;
    m_scene_manager->GetObjectByIndex(i, reinterpret_cast<Object**>(&scene));
    scene->ResetRenderState();
  }
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         ����������
 */
//-----------------------------------------------------------------------------
void System::initialize(gfl::heap::NwAllocator* /*heap_allocator*/, gfl::heap::NwAllocator* device_allocator)
{
  if( !m_is_initialized )
  {
    // �R�}���h�L���b�V���𓮓I�Ɋm�ۂ��邽�߂ɃA���P�[�^��n���܂��B
    // gfx �̃��C�u�������g���O�ɕK���A���P�[�^��ݒ肷��K�v������܂��B
    //�s�v�ɂȂ���nw::gfx::CommandCacheManager::SetAllocator( device_allocator );

    {
      // L4 8x8 ����F�̃e�N�X�`���o�b�t�@
      if( !m_small_texture_buffer )
      {
        m_small_texture_buffer = createCommonDataSmallTextureBuffer(device_allocator);
      }
    }

    m_is_initialized = true;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �I������
 */
//-----------------------------------------------------------------------------
void System::finalize(void)
{
  {
    // L4 8x8 ����F�̃e�N�X�`���o�b�t�@
    if( m_small_texture_buffer )
    {
      destroyCommonDataSmallTextureBuffer(m_small_texture_buffer);
      m_small_texture_buffer = NULL;
    }
  }

  m_is_initialized = false;

  // @note initialize�ōs�����������I����������@���s���Ȃ̂ŁA1�x������������I���ł��Ȃ��B
  //       ���̃N���X��j�����čĂт��̃N���X�𐶐�������A2��ڂ̏��������s���Ă��܂������v���낤���H���Â��L�q�A���͂���nw::gfx::CommandCacheManager::SetAllocator�̕K�v�͂Ȃ��̂ő��v�B
}




//-----------------------------------------------------------------------------
/*
 *  @brief         ���ʃf�[�^�������I�ɓǂݍ��ݐ�������A�j������
 */
//-----------------------------------------------------------------------------
void System::autoCreateCommonData(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          ) autoCreateCommonDataDefaultShaderResource(heap_allocator, device_allocator);
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE ) autoCreateCommonDataParticleDefaultShaderResource(heap_allocator, device_allocator);
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       ) autoCreateCommonDataNwDefaultShaderResource(heap_allocator, device_allocator);
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE                    ) autoCreateCommonDataSmallTexture(heap_allocator, device_allocator);  // L4 8x8 ����F�̃e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE      ) autoCreateCommonDataH3dDefaultShaderResource(heap_allocator, device_allocator);
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE                ) autoCreateCommonDataH3dSmallTexture(heap_allocator, device_allocator);  // L4 8x8 ����F�̃e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE
}
void System::autoDestroyCommonData(void)
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          ) autoDestoryCommonDataDefaultShaderResource();
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE ) autoDestroyCommonDataParticleDefaultShaderResource();
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       ) autoDestoryCommonDataNwDefaultShaderResource();
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE                    ) autoDestoryCommonDataSmallTexture();  // L4 8x8 ����F�̃e�N�X�`��
#if GFL_GRP_G3D_H3D_USE
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE      ) autoDestroyCommonDataH3dDefaultShaderResource();
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE                ) autoDestoryCommonDataH3dSmallTexture();  // L4 8x8 ����F�̃e�N�X�`��
#endif  // GFL_GRP_G3D_H3D_USE
}

void System::autoCreateCommonDataDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  m_default_shader_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  void * buff = System::GetStaticResource( GFL_DEFAULT_SHADER_BCSDR );
  if ( buff )
  {
    Resource::Description     desc;
    m_default_shader_resource->AttachBufferAndSetup(device_allocator, buff, &desc, false, Resource::COMMON_RESOURCE_NONE );
  }
  else
    m_default_shader_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_DEFAULT_SHADER_FILE_NAME);
}
void System::autoDestoryCommonDataDefaultShaderResource(void)
{
  if( m_default_shader_resource )
  {
    GFL_DELETE m_default_shader_resource;
    m_default_shader_resource          = NULL;
  }
}

void System::autoCreateCommonDataParticleDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  m_particle_default_shader_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  void * buff = System::GetStaticResource( NWGFX_PARTICLEDEFAULTSHADER_BCSDR );
  if ( buff )
  {
    Resource::Description     desc;
    m_particle_default_shader_resource->AttachBufferAndSetup(device_allocator, buff, &desc, false, Resource::COMMON_RESOURCE_NONE );
  }
  else
    m_particle_default_shader_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_PARTICLE_DEFAULT_SHADER_FILE_NAME);
}
void System::autoDestroyCommonDataParticleDefaultShaderResource(void)
{
  if( m_particle_default_shader_resource )
  {
    GFL_DELETE m_particle_default_shader_resource;
    m_particle_default_shader_resource = NULL;
  }
}

void System::autoCreateCommonDataNwDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  m_nw_default_shader_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  void * buff = System::GetStaticResource( NWGFX_DEFAULTSHADER_BCSDR );
  if ( buff )
  {
    Resource::Description     desc;
    m_nw_default_shader_resource->AttachBufferAndSetup(device_allocator, buff, &desc, false, Resource::COMMON_RESOURCE_NONE );
  }
  else
    m_nw_default_shader_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_NW_DEFAULT_SHADER_FILE_NAME);
}
void System::autoDestoryCommonDataNwDefaultShaderResource(void)
{
  if( m_nw_default_shader_resource )
  {
    GFL_DELETE m_nw_default_shader_resource;
    m_nw_default_shader_resource          = NULL;
  }
}

void System::autoCreateCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)  // L4 8x8 ����F�̃e�N�X�`��
{
  //m_small_texture_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
  //m_small_texture_resource->LoadAndSetup(device_allocator, GFL_GRP_G3D_SMALL_TEXTURE_FILE_NAME);

  m_small_texture = createCommonDataSmallTexture(heap_allocator, device_allocator, m_small_texture_buffer);
}
void System::autoDestoryCommonDataSmallTexture(void)  // L4 8x8 ����F�̃e�N�X�`��
{
  //if( m_small_texture_resource )
  //{
  //  GFL_DELETE m_small_texture_resource;
  //  m_small_texture_resource = NULL;
  //}

  if( m_small_texture )
  {
    GFL_DELETE m_small_texture;
    m_small_texture = NULL;
  }
}

#if GFL_GRP_G3D_H3D_USE
void System::autoCreateCommonDataH3dDefaultShaderResource(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  do 
  {
    void * buff = GetStaticResource( GFL_DEFAULTSHADER_BCHSDR );
    
    if ( buff == NULL)
    {
      // �t�@�C���ǂݍ���
      gfl::fs::ReadFile read_file;
      b32 open_ret = read_file.Open(GFL_GRP_G3D_H3D_DEFAULT_SHADER_FILE_NAME);
      if( !open_ret ) break;
      u32 size       = read_file.GetSize();
      u32 size_align = read_file.GetSizeAlign();  // �e�N�X�`�������[�h����ɂ�128Byte�A���C�����g���s���K�v������̂ŁA
      size_align     = 128;                       // �������[�h���邩�ŃA���C�����g�𕪂���K�v�����邩��
      void* buffer = GflHeapAllocMemoryAlign( heap_allocator->GetHeapBase(), size, size_align );
      s32 read_ret = read_file.Read( buffer, size );  // size��size_align�ɍ��킹���A���C�����g�ɂ��Ă����K�v�����邩��
      read_file.Close();
      if( read_ret < 0 )
      {
        GflHeapFreeMemory(buffer);
        break;
      }
      m_h3d_default_shader_buffer = buffer;
      m_h3d_default_shader_buffer_release_flag = true;
    }
    else
    {
      m_h3d_default_shader_buffer = buff;
      m_h3d_default_shader_buffer_release_flag = false;
    }
    m_h3d_default_shader_resource = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::H3dResource;
    m_h3d_default_shader_resource->AttachBuffer(m_h3d_default_shader_buffer);
    m_h3d_default_shader_resource->Setup(heap_allocator, device_allocator);
  }
  while(0);
}
void System::autoDestroyCommonDataH3dDefaultShaderResource(void)
{
  if( m_h3d_default_shader_resource )
  {
    m_h3d_default_shader_resource->DetachBuffer(true);
    GFL_DELETE m_h3d_default_shader_resource;
    m_h3d_default_shader_resource = NULL;
  }
  if( m_h3d_default_shader_buffer )
  {
    if(m_h3d_default_shader_buffer_release_flag){
      GflHeapFreeMemory(m_h3d_default_shader_buffer);
    }
    m_h3d_default_shader_buffer = NULL;
  }
}

void System::autoCreateCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)  // L4 8x8 ����F�̃e�N�X�`��
{
  m_h3d_small_texture = createCommonDataH3dSmallTexture(heap_allocator, device_allocator, m_small_texture_buffer);
}
void System::autoDestoryCommonDataH3dSmallTexture(void)  // L4 8x8 ����F�̃e�N�X�`��
{
  if( m_h3d_small_texture )
  {
    GFL_DELETE m_h3d_small_texture;
    m_h3d_small_texture = NULL;
  }
}
#endif  // GFL_GRP_G3D_H3D_USE


//-----------------------------------------------------------------------------
/*
 *  @brief         ���ʃf�[�^�̐����A�j��
 *                 CommonDataElement�̘g�́A�Ăяo�����Ő����A�j�����ĉ������B
 */
//-----------------------------------------------------------------------------
void* System::createCommonDataSmallTextureBuffer(gfl::heap::NwAllocator* device_allocator)  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@����
{
  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@
  void* buffer = GflHeapAllocMemoryAlign(
      device_allocator->GetHeapBase(),
      8*8/2,  // L4 8x8
      128
  );
  gfl::std::MemFill( buffer, 0xFF, 8*8/2 );
  return buffer;
}

void System::destroyCommonDataSmallTextureBuffer(void* buffer)  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@�j��
{
  // L4 8x8 ����F�̃e�N�X�`���o�b�t�@
  if( buffer )
  {
    GflHeapFreeMemory(buffer);
  }
}


Texture* System::createCommonDataSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer)  // L4 8x8 ����F�̃e�N�X�`��
{
  Texture* small_texture = NULL;

  gfl::grp::g3d::Texture::Description tex_desc;
  {
    tex_desc.type = gfl::grp::g3d::Texture::TYPE_IMAGE_TEXTURE;
    tex_desc.height = 8;
    tex_desc.width  = 8;
    tex_desc.memory_area = gfl::grp::MEMORY_AREA_FCRAM;
    tex_desc.memory_transfer = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP;
    tex_desc.location_address = reinterpret_cast<u32>(buffer);
    tex_desc.executing_memory_fill = false;
    tex_desc.is_dynamic_allocation = true;  // (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)�̂Ƃ��͂�����true�ɂ��Ă����Ȃ��ƁA���s����
        // gfx_ResTexture.cpp:238 Panic:NW:Failed assertion (loadFlag != (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)) || resImage.CheckMemoryLocation()
        // �Ƃ����G���[�Ɉ����|�����Ă��܂��B  // @note ������true�ɂ������ƂŐ������ȊO�ɂ����������m�ۂ��ꂽ��C�����Ȃ��B��Builder�̃����o�ɂ���isDynamicAllocation�͂Ȃ����琶�����ɂ����������m�ۂ͂���Ȃ��B
    tex_desc.mipmap_size = 1;
    tex_desc.format = gfl::grp::g3d::Texture::FORMAT_L4;
  }

  small_texture = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Texture;
  small_texture->Create(heap_allocator, device_allocator, &tex_desc);

  return small_texture;
}

#if GFL_GRP_G3D_H3D_USE
H3dDynamicTexture* System::createCommonDataH3dSmallTexture(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, void* buffer)  // L4 8x8 ����F�̃e�N�X�`��
{
  H3dDynamicTexture* small_texture = NULL;

  gfl::grp::g3d::H3dDynamicTexture::TextureInformation info;
  {
    info.physical_address = reinterpret_cast<void*>(nn::gx::CTR::GetPhysicalAddr(reinterpret_cast<uptr>(buffer)));
    info.width  = 8;
    info.height = 8;
    info.format = gfl::grp::g3d::TEXTURE_FORMAT_L4;
    info.mipmap = 1;
  }

  small_texture = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::H3dDynamicTexture;
  small_texture->Create(heap_allocator, device_allocator);
  small_texture->SetTextureInformation(info);

  return small_texture;
}
#endif  // GFL_GRP_G3D_H3D_USE


b32 System::createCommonData(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    CommonDataElement* elem
)
{
  b32 ret = false;

  switch(elem->arg.type)
  {
  case COMMON_DATA_TYPE_NONE:
    {
      // �������Ȃ�
      break;
    }
  case COMMON_DATA_TYPE_DEFAULT_SHADER:
  case COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER:
  case COMMON_DATA_TYPE_NW_DEFAULT_SHADER:
  case EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER:
  case EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE:
    {
      Resource* resource = GFL_NEW(heap_allocator->GetHeapBase()) Resource;
      ret = resource->AttachBufferAndSetup(device_allocator, elem->arg.ptr);
      if( ret )
      {
        elem->data = resource;
      }
      else
      {
        GFL_ASSERT(0);
      }
      break;
    }
  case COMMON_DATA_TYPE_SMALL_TEXTURE:
    {
      Texture* texture = createCommonDataSmallTexture(heap_allocator, device_allocator, m_small_texture_buffer);
      if( texture )
      {
        elem->data = texture;
        ret = true;
      }
      else
      {
        GFL_ASSERT(0);
      }
      break;
    }
#if GFL_GRP_G3D_H3D_USE
  case COMMON_DATA_TYPE_H3D_DEFAULT_SHADER:
    {
      H3dResource* resource = GFL_NEW(heap_allocator->GetHeapBase()) H3dResource;
      ret = resource->AttachBuffer(elem->arg.ptr);
      if( ret )
      {
        ret = resource->Setup(heap_allocator, device_allocator);
      }
      if( ret )
      {
        elem->data = resource;
      }
      else
      {
        GFL_ASSERT(0);
      }
      break;
    }
  case COMMON_DATA_TYPE_H3D_SMALL_TEXTURE:
    {
      H3dDynamicTexture* texture = createCommonDataH3dSmallTexture(heap_allocator, device_allocator, m_small_texture_buffer);
      if( texture )
      {
        elem->data = texture;
        ret = true;
      }
      else
      {
        GFL_ASSERT(0);
      }
      break;
    }
#endif  // GFL_GRP_G3D_H3D_USE
  case EX_COMMON_DATA_TYPE_RESOURCE_SHADER:
  case EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE:
    {
      Resource* resource = reinterpret_cast<Resource*>(elem->arg.ptr);
      if( resource )
      {
        elem->data = resource;
        ret = true;
      }
      else
      {
        GFL_ASSERT(0);
      }
      break;
    }
  }

  return ret;
}

void System::destroyCommonData(CommonDataElement* elem)
{
  switch(elem->arg.type)
  {
  case COMMON_DATA_TYPE_NONE:
    {
      // �������Ȃ�
      break;
    }
  case COMMON_DATA_TYPE_DEFAULT_SHADER:
  case COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER:
  case COMMON_DATA_TYPE_NW_DEFAULT_SHADER:
  case EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER:
  case EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE:
    {
      Resource* resource = reinterpret_cast<Resource*>(elem->data);
      // @note �y�d�v�zCleanup���Ă��邩�v�m�F��SafeCleanup���Ă�ł���
      resource->DetachBuffer();
      GFL_DELETE resource;
      elem->data = NULL;
      break;
    }
  case COMMON_DATA_TYPE_SMALL_TEXTURE:
    {
      Texture* texture = reinterpret_cast<Texture*>(elem->data);
      GFL_DELETE texture;
      elem->data = NULL;
      break;
    }
#if GFL_GRP_G3D_H3D_USE
  case COMMON_DATA_TYPE_H3D_DEFAULT_SHADER:
    {
      H3dResource* resource = reinterpret_cast<H3dResource*>(elem->data);
      // @note �y�d�v�zCleanup���Ă��邩�v�m�F��SafeCleanup���Ă�ł���
      resource->DetachBuffer();
      GFL_DELETE resource;
      elem->data = NULL;
      break;
    }
  case COMMON_DATA_TYPE_H3D_SMALL_TEXTURE:
    {
      H3dDynamicTexture* texture = reinterpret_cast<H3dDynamicTexture*>(elem->data);
      GFL_DELETE texture;
      elem->data = NULL;
      break;
    }
#endif  // GFL_GRP_G3D_H3D_USE
  case EX_COMMON_DATA_TYPE_RESOURCE_SHADER:
  case EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE:
    {
      Resource* resource = reinterpret_cast<Resource*>(elem->arg.ptr);
      elem->data = NULL;
      break;
    }
  }
}


//#######################################################################################
//
//           ���ʃf�[�^
//
//#######################################################################################

//---------------------------------------------------------------------------
/**
 *  @brief  ���ʃf�[�^�𐶐�����
 *  @retval  �����ł����Ƃ�true��Ԃ�
 */
//---------------------------------------------------------------------------
b32 System::CreateCommonData(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    CommonDataCreateArg&    common_data_create_arg
)
{
  b32 ret = false;

  CommonDataElement l_elem;
  l_elem.arg = common_data_create_arg;
 
  ret = createCommonData(
      heap_allocator,
      device_allocator,
      &l_elem
  );

  if( ret )
  {
    CommonDataElement* add_elem = GFL_NEW(heap_allocator->GetHeapBase()) CommonDataElement;
    *add_elem = l_elem;
 
    // �����ɒǉ�����
    CommonDataElement* elem = m_common_data_head_elem;
    if( elem )
    {
      // �擪�v�f�ɂ͂Ȃ�Ȃ�
      while(elem)
      {
        CommonDataElement* prev_elem = elem->prev;
        CommonDataElement* next_elem = elem->next;
        if( !next_elem )
        {
          elem->next = add_elem;
          add_elem->prev = elem;
          break;
        }
        elem = next_elem;
      }
    }
    else
    {
      // �擪�v�f
      m_common_data_head_elem = add_elem;
    }
  }
  
  return ret;
}

//---------------------------------------------------------------------------
/**
 *  @brief  ���ʃf�[�^��j������
 *          ptr���w�����̂��̂��̂̉���͌Ăяo�����ōs���ĉ������B
 *  @param[in]  ptr  CreateCommonData�ŗ^����CommonDataCreateArg��ptr
 */
//---------------------------------------------------------------------------
void System::DestroyCommonData(
    void* ptr
)
{
  CommonDataElement* elem = m_common_data_head_elem;
  while(elem)
  {
    CommonDataElement* prev_elem = elem->prev;
    CommonDataElement* next_elem = elem->next;
    if( elem->arg.ptr == ptr )
    {
      destroyCommonData(elem);
      GFL_DELETE elem;
      if(prev_elem) prev_elem->next = next_elem;
      else          m_common_data_head_elem = next_elem;  // �擪�v�f
      if(next_elem) next_elem->prev = prev_elem;
      break;
    }
    elem = next_elem;
  }
}

//---------------------------------------------------------------------------
/**
 *  @brief  �S�Ă̋��ʃf�[�^��j������
 *          ptr���w�����̂��̂��̂̉���͌Ăяo�����ōs���ĉ������B
 *          CreateCommonData�Ő����������̂����j�����܂��BAutoCommonDataDeal�Ő����������͔̂j�����܂���B
 */
//---------------------------------------------------------------------------
void System::DestroyAllCommonData(void)
{
  CommonDataElement* elem = m_common_data_head_elem;
  while(elem)
  {
    CommonDataElement* prev_elem = elem->prev;
    CommonDataElement* next_elem = elem->next;
    DestroyCommonData(elem->arg.ptr);
    elem = next_elem;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���ʃf�[�^���擾����
 *          CreateCommonData�Ő����������̂��AAutoCommonDataDeal�Ő����������̂��A�ǂ�����擾�ł��܂��B
 *  @retval       
 */
//-----------------------------------------------------------------------------
Resource*     System::GetCommonDataDefaultShaderResource(void) const
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_DEFAULT_SHADER_RESOURCE          )
    return m_default_shader_resource;
  else
    return reinterpret_cast<Resource*>(GetCommonData(COMMON_DATA_TYPE_DEFAULT_SHADER));
}
Resource*     System::GetCommonDataParticleDefaultShaderResource(void) const
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_PARTICLE_DEFAULT_SHADER_RESOURCE )
    return m_particle_default_shader_resource;
  else
    return reinterpret_cast<Resource*>(GetCommonData(COMMON_DATA_TYPE_PARTICLE_DEFAULT_SHADER));
}
Resource*     System::GetCommonDataNwDefaultShaderResource(void) const
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_NW_DEFAULT_SHADER_RESOURCE       )
    return m_nw_default_shader_resource;
  else
    return reinterpret_cast<Resource*>(GetCommonData(COMMON_DATA_TYPE_NW_DEFAULT_SHADER));
}
Texture*      System::GetCommonDataSmallTexture(void) const  // L4 8x8 ����F�̃e�N�X�`��
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_SMALL_TEXTURE                    )
    return m_small_texture;
  else
    return reinterpret_cast<Texture*>(GetCommonData(COMMON_DATA_TYPE_SMALL_TEXTURE));
}
#if GFL_GRP_G3D_H3D_USE
H3dResource*  System::GetCommonDataH3dDefaultShaderResource(void) const
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_DEFAULT_SHADER_RESOURCE      )
    return m_h3d_default_shader_resource;
  else
    return reinterpret_cast<H3dResource*>(GetCommonData(COMMON_DATA_TYPE_H3D_DEFAULT_SHADER));
}
H3dDynamicTexture*  System::GetCommonDataH3dSmallTexture(void) const  // L4 8x8 ����F�̃e�N�X�`��
{
  if( m_auto_common_data_deal_bit & AUTO_COMMON_DATA_DEAL_BIT_H3D_SMALL_TEXTURE                    )
    return m_h3d_small_texture;
  else
    return reinterpret_cast<H3dDynamicTexture*>(GetCommonData(COMMON_DATA_TYPE_H3D_SMALL_TEXTURE));
}
#endif  // GFL_GRP_G3D_H3D_USE


//-----------------------------------------------------------------------------
/**
 *  @brief  ���ʃf�[�^�𓾂�
 *  @param[in]  type  CreateCommonData�ŗ^����CommonDataCreateArg��type�ł��B����type�̃f�[�^����������ꍇ�́A��ԍŏ��ɐ����������̂�Ԃ��܂��B
 *  @retval  �������ꂽ�f�[�^�Btype�ɉ����ăL���X�g���Ďg�p���ĉ������B
 */
//-----------------------------------------------------------------------------
void* System::GetCommonData(CommonDataType type) const
{
  void* data = NULL;
  CommonDataElement* elem = m_common_data_head_elem;
  while(elem)
  {
    CommonDataElement* prev_elem = elem->prev;
    CommonDataElement* next_elem = elem->next;
    if( elem->arg.type == type )
    {
      data = elem->data;
      break;
    }
    elem = next_elem;
  }
  return data;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  ���ʃf�[�^�𓾂�
 *  @param[in]  ptr  CreateCommonData�ŗ^����CommonDataCreateArg��ptr
 *  @retval  �������ꂽ�f�[�^�Btype�ɉ����ăL���X�g���Ďg�p���ĉ������B
 */
//-----------------------------------------------------------------------------
void* System::GetCommonData(void* ptr) const
{
  void* data = NULL;
  CommonDataElement* elem = m_common_data_head_elem;
  while(elem)
  {
    CommonDataElement* prev_elem = elem->prev;
    CommonDataElement* next_elem = elem->next;
    if( elem->arg.ptr == ptr )
    {
      data = elem->data;
      break;
    }
    elem = next_elem;
  }
  return data;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �uEX_COMMON_DATA_TYPE_BUFFER_RESOURCE_���O�v�𓾂�
 *  @param[in]  param0  �uCommonDataCreateArg�v�́uparam0�v�ɓn�����uResource::EX_COMMON_RESOURCE_BIT_�ԍ��v
 *  @retval  Resource
 */
//-----------------------------------------------------------------------------
Resource* System::GetExCommonDataExCommonResource(u32 param0) const
{
  void* data = NULL;
  CommonDataElement* elem = m_common_data_head_elem;
  while(elem)
  {
    CommonDataElement* prev_elem = elem->prev;
    CommonDataElement* next_elem = elem->next;
    if(
           elem->arg.type == EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_SHADER
        || elem->arg.type == EX_COMMON_DATA_TYPE_BUFFER_RESOURCE_TEXTURE
        || elem->arg.type == EX_COMMON_DATA_TYPE_RESOURCE_SHADER
        || elem->arg.type == EX_COMMON_DATA_TYPE_RESOURCE_TEXTURE
    )
    {
      if( elem->arg.param0 == param0 )
      {
        data = elem->data;
        break;
      }
    }
    elem = next_elem;
  }    

  if( data )
  {
    return reinterpret_cast<Resource*>(data);
  }
  else
  {
    return NULL;
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

