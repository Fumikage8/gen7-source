//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dFog.cpp
 *	@brief  3D�t�H�O
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
#include <grp/g3d/gfl_G3dSystem.h>

#include <grp/g3d/gfl_G3dFog.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�t�H�O�N���X
//=====================================

//-------------------------------------
///	�t�H�O�e�[�u���\����
//=====================================
void Fog::InitializeFogTableForDefault(
    FogTable*    fog_table
)
{
  for( u32 i=0; i<FOG_TABLE_VALUE_NUM; ++i )
  {
    fog_table->value[i] = 0.0f;
  }
}
 
void Fog::SetupFogTableUsingLookupTable(
    FogTable*    fog_table, 
    Resource*    lookup_table_resource,
    s32          lookup_table_set_index_in_resource,
    s32          lookup_table_index_in_lookup_table_set
)
{
  // ��`
  const u32 FIX12_ONE_U32_u32 = 4096;
  const f32 FIX12_ONE_U32_f32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128����Ȃ�������ȉ��̃\�[�X�����藧���Ȃ��̂ŃR���p�C�����Ƀ`�F�b�N
  STATIC_ASSERT(FOG_TABLE_VALUE_NUM == 128);

  // �ϐ�
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // �Q�ƃe�[�u����ǂ�
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(lookup_table_index_in_lookup_table_set);

    GFL_ASSERT(nw_res_image_lookup_table.IsAbs());  // ��Βl���t���O�������Ă���Ƃ��̃A�N�Z�X���@�����L�q���Ă��Ȃ��B  //@check �ǂ�Ȍ����ڂ̃t�H�O�ɂȂ邩�͕�����Ȃ�����������B

    const s32 LOOKUP_TABLE_DATA_NUM = 260;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 )
      {
        // �w�b�_
      }
      else if ( i>=257 )
      {
        // �t�b�^�[
      }
      else
      {
        // �L���ȃf�[�^
        // �e�[����256�i�K�ō���Ă���B
        // �t�H�O�e�[�u����128�i�K�ł悢�̂ŁA1/2�ɂ���
        b32 b_use = false;
        if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
        {
          b_use = true;
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF );
          if( value_down > FIX12_ONE_U32_u32 * 10 )
          {
            GFL_ASSERT_MSG(0, "�t�H�O�̎Q�ƃe�[�u���̒l%d���ُ�ɑ傫���ł��B\n", value_down);
            value_down = 0;
          }
          if( value_down > FIX12_ONE_U32_u32 ) value_down = FIX12_ONE_U32_u32;
          if(index < FOG_TABLE_VALUE_NUM)
          {
            fog_table->value[index] = static_cast<f32>(value_down / FIX12_ONE_U32_f32);
          }
          ++index;
        }
        ++lookup_table_data_index;

#if GFL_DEBUG
        static bool s_flag = false;
        
        if(s_flag){
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF ); 
          GFL_PRINT("fog lut GetCommandCache[%4d] %d = %f | %f\n", i, value, static_cast<f32>(value_up / FIX12_ONE_U32_f32), static_cast<f32>(value_down / FIX12_ONE_U32_f32));
        }
#endif // GFL_DEBUG
      }


    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
Fog::Fog(void)
  : SceneNode(),
    m_enable_flag(false),
    m_enable_min_depth(DISABLE_MIN_DEPTH),
    m_enable_max_depth(DISABLE_MAX_DEPTH),
    m_type(TYPE_NONE),
    m_fog_table(NULL),
    m_lookup_table_resource(NULL),
    m_lookup_table_set_index_in_resource(LOOKUP_TABLE_INDEX_NONE),
    m_lookup_table_index_in_lookup_table_set(LOOKUP_TABLE_INDEX_NONE),
    m_intensity(1.0f)
{
  GFL_SINGLETON_INSTANCE(System)->GetFogManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
Fog::~Fog()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetFogManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Fog::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // �A�j���[�V�����Ȃ�
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ�����
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_NORMAL;
  m_fog_table = NULL;
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �Q�ƃe�[�u�����g�p�����t�H�O�̐���
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     lookup_table_resource                   �Q�ƃe�[�u���̃��\�[�X
 *  @param[in]     lookup_table_set_index_in_resource      �Q�ƃe�[�u���Z�b�g�̃��\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     lookup_table_index_in_lookup_table_set  �Q�ƃe�[�u���̎Q�ƃe�[�u���Z�b�g���ɂ�����C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Fog::CreateFogUsingLookupTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource,
    Resource*                lookup_table_resource,
    s32                      lookup_table_set_index_in_resource,
    s32                      lookup_table_index_in_lookup_table_set
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // �A�j���[�V�����Ȃ�
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ�����
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_USE_LOOKUP_TABLE_RESOURCE;
  m_lookup_table_resource                  = lookup_table_resource;
  m_lookup_table_set_index_in_resource     = lookup_table_set_index_in_resource;
  m_lookup_table_index_in_lookup_table_set = lookup_table_index_in_lookup_table_set;
  m_intensity                 = 1.0f;

  // �Q�ƃe�[�u������t�H�O�e�[�u�����쐬
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  SetupFogTableUsingLookupTable( m_fog_table, m_lookup_table_resource, m_lookup_table_set_index_in_resource, m_lookup_table_index_in_lookup_table_set );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �t�H�O�e�[�u�����g�p�����t�H�O�̐���
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     device_allocator   �f�o�C�X�A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     fog_table          �t�H�O�e�[�u��(���e���R�s�[�������̂�ێ�����̂ŁA���̊֐�����߂��Ă�����A�Ăяo������fog_table���폜���Ă��悢)
 */
//-----------------------------------------------------------------------------
void Fog::CreateFogUsingFogTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource,
    const FogTable&          fog_table
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // �A�j���[�V�����Ȃ�
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ�����
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_CREATE_FOG_TABLE;
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  *m_fog_table = fog_table;
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

void Fog::CreateFogUsingFogTable(
    gfl::heap::NwAllocator*  allocator,
    gfl::heap::NwAllocator*  device_allocator,
    Resource*                resource,
    s32                      index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResFog res = nw_res_graphics_file->GetFogs(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // �A�j���[�V�����Ȃ�
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  // �Ǝ�����
  m_enable_flag = true;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_CREATE_FOG_TABLE;
  m_fog_table = GFL_NEW(allocator->GetHeapBase()) FogTable;
  InitializeFogTableForDefault(m_fog_table); 
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void Fog::Destroy(void)
{
  // �Ǝ�����
  m_enable_flag = false;
  m_enable_min_depth = DISABLE_MIN_DEPTH;
  m_enable_max_depth = DISABLE_MAX_DEPTH;
  m_type = TYPE_NONE;
  if( m_fog_table )
  {
    GFL_DELETE m_fog_table;
    m_fog_table = NULL;
  }
  m_lookup_table_resource                  = NULL;
  m_lookup_table_set_index_in_resource     = LOOKUP_TABLE_INDEX_NONE;
  m_lookup_table_index_in_lookup_table_set = LOOKUP_TABLE_INDEX_NONE;
  m_intensity                 = 1.0f;

  SceneNode::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �J���[
 *                 u8�̂Ƃ���0����&�����A255����&�s����
 *                 f32�̂Ƃ���0.0f����&�����A1.0f����&�s����
 */
//-----------------------------------------------------------------------------
void Fog::SetColor(f32 r, f32 g, f32 b)
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(r,g,b);
}
void Fog::SetColor(const gfl::grp::ColorF32& c)  // a�͂ǂ��g���邩�s��
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(c);
}
void Fog::SetColor(u8 r, u8 g, u8 b)
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(
    (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
void Fog::SetColor(const gfl::grp::ColorU8& c)   // a�͂ǂ��g���邩�s��
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  res_fog.SetColor(
    (static_cast<f32>(c.r))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.g))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.b))/gfl::grp::ColorU8::ELEMENT_MAX,
    (static_cast<f32>(c.a))/gfl::grp::ColorU8::ELEMENT_MAX
  );
}
gfl::grp::ColorF32 Fog::GetColor(void) const    // a�͂ǂ��g���邩�s��
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);
  nw::gfx::ResFog res_fog = GetNwResFog();
  c = res_fog.GetColor();
  return c;
}
void Fog::GetColor(gfl::grp::ColorF32* c) const  // a�͂ǂ��g���邩�s��
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  *c = res_fog.GetColor();
}
void Fog::GetColor(gfl::grp::ColorU8*  c) const  // a�͂ǂ��g���邩�s��
{
  nw::gfx::ResFog res_fog = GetNwResFog();
  gfl::grp::ColorF32 c_f32 = res_fog.GetColor();
  c->Set(
    static_cast<s32>(c_f32.r*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.g*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.b*gfl::grp::ColorU8::ELEMENT_MAX),
    static_cast<s32>(c_f32.a*gfl::grp::ColorU8::ELEMENT_MAX)
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 */
//-----------------------------------------------------------------------------
void     Fog::SetFunction(Function function)
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  res_fog_updater.SetFogUpdaterType( static_cast<nw::gfx::res::ResFogUpdater::FogUpdaterType>(function) );
}
gfl::grp::g3d::Fog::Function Fog::GetFunction(void) const
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  return static_cast<Function>( res_fog_updater.GetFogUpdaterType() );
}
void Fog::SetDensity(f32 value)
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  res_fog_updater.SetDensity(value); 
}
f32  Fog::GetDensity(void) const
{
  nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
  return res_fog_updater.GetDensity(); 
}
void Fog::SetMinDepth(f32 min)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMinFogDepth(min);
  }
  else
  {
    m_enable_min_depth = min;
  }
}
f32  Fog::GetMinDepth(void) const
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    return res_fog_updater.GetMinFogDepth(); 
  }
  else
  {
    return m_enable_min_depth;
  }
}
void Fog::SetMaxDepth(f32 max)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMaxFogDepth(max);
  }
  else
  {
    m_enable_max_depth = max;
  }
}
f32  Fog::GetMaxDepth(void) const
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    return res_fog_updater.GetMaxFogDepth(); 
  }
  else
  {
    return m_enable_max_depth;
  }
}
void Fog::SetDepth(f32 min, f32 max)
{
  if( m_enable_flag )
  {
    nw::gfx::ResFogUpdater res_fog_updater = GetNwResFogUpdater();
    res_fog_updater.SetMinFogDepth(min);
    res_fog_updater.SetMaxFogDepth(max);
  }
  else
  {
    m_enable_min_depth = min;
    m_enable_max_depth = max;
  }
}

void Fog::SetIntensity(const f32 intensity)  // m_type��TYPE_CREATE_FOG_TABLE��TYPE_USE_LOOKUP_TABLE_RESOURCE�̂Ƃ��̂ݗL���B  // 0.0f�őS���t�H�O���o�Ȃ��A1.0f�ŎQ�ƃe�[�u���̒ʂ�Ƀt�H�O���o��(0.0f<= <=1.0f)
{
  GFL_ASSERT_MSG( 0.0f<=intensity&&intensity<=1.0f, "intensity=%f�͔͈͊O�ł��B\n", intensity);

  m_intensity = intensity;
  if( m_intensity < 0.0f )      m_intensity = 0.0f;
  else if( m_intensity > 1.0f ) m_intensity = 1.0f;

  // @note �t�H�O�̍X�V��ύX���������Ƃ������s��Ȃ�����Ȃ�A
  //       ����m_intensity���ύX���ꂽ�Ƃ��͖Y�ꂸ�Ƀt�H�O�̍X�V������悤�ɂ��Ă������ƁB
}
f32  Fog::GetIntensity(void) const
{
  return m_intensity;
}

void Fog::SetFogTable(
    const FogTable&          fog_table  // �t�H�O�e�[�u��(���e���R�s�[�������̂�ێ�����̂ŁA���̊֐�����߂��Ă�����A�Ăяo������fog_table���폜���Ă��悢)
)
{
  *m_fog_table = fog_table;  // �z��́A���Ƃ��v�f�̐��������ł����Ă��A�u=�v�œ��e����������R�s�[���邱�Ƃ͂ł��܂���Bmemcpy�̂悤�Ȋ֐����g�����Ƃ��K�v�ł��B�\���̂̏ꍇ�́A�u=�v���g���ē��e���R�s�[���邱�Ƃ��ł��܂��B
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �L���t���O
 */
//-----------------------------------------------------------------------------
void Fog::SetEnable(bool enable)
{
  b32 old_enable = m_enable_flag;

  if( enable )
  {
    if( !old_enable )
    {
      m_enable_flag = true;
      // ���̎��_��true�ɂ��Ă�����Set�ł���悤�ɂ���
      this->SetMinDepth(m_enable_min_depth);
      this->SetMaxDepth(m_enable_max_depth);
      m_enable_min_depth = DISABLE_MIN_DEPTH;
      m_enable_max_depth = DISABLE_MAX_DEPTH;
    }
  }
  else
  {
    if( old_enable )
    {
      // ���̎��_�ł�true�̂܂܂ɂ��Ă�����Set�ł���悤�ɂ���
      m_enable_min_depth = this->GetMinDepth();
      m_enable_max_depth = this->GetMaxDepth();
      this->SetMinDepth(DISABLE_MIN_DEPTH);
      this->SetMaxDepth(DISABLE_MAX_DEPTH);
      m_enable_flag = false;
    }
  }
}

b32 Fog::IsEnable(void) const
{
  return m_enable_flag;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 */
//-----------------------------------------------------------------------------
#if 0

// gfx_Fog.h/.cpp
// SetupFogSampler�֐����̂܂܂������Ă݂Ă��܂������B
void Fog::Update(Camera* camera)
{
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  {
    const uint HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
    

    enum
    {
        REG_FOG_TABLE_INDEX = 0xe6,
        REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    uint index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (int i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
        if (l_WScale == 0.0f)
        {
            f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
            viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
            viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
        }
        else
        {
            viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
        }

        if (viewPos.z <= fogUpdater.GetMinFogDepth())
        {
            viewPos.z = 1.0f;
        }
        else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        {
            viewPos.z = 0.0f;
        }
        else
        {
            if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_LINEAR)
            {
                viewPos.z =
                    (fogUpdater.GetMaxFogDepth() - viewPos.z) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
            }
            else if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT)
            {
                viewPos.z = 
                    nw::math::FExp(
                    -fogUpdater.GetDensity() *
                    (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()));
            }
            else if (fogUpdater.GetFogUpdaterType() == nw::gfx::res::ResFogUpdater::FOG_UPDATER_TYPE_EXPONENT_SQUARE)
            {
                f32 viewDepth = fogUpdater.GetDensity() * 
                        (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth());
                viewPos.z =
                    nw::math::FExp( - viewDepth * viewDepth );
            }
            else
            {
                GFL_ASSERT_MSG(false, "Unknown FogUpdater type.");  //@check �����Ɉ����|����Ȃ��悤�Ɋm�F�ς̈�����Ă����B
            }
        }

        if ( i > 0 )
        {
            u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
            u32 commandIndex = (index == 0) ? 2 : index + 3;
            
            fogSampler.SetCommandCache(commandIndex, value );
            
            ++index;
        }
        
        prevLut = viewPos.z;
    }

  }

}

#elif 0

����͊ԈႢ�B
�t�H�O���S��������Ȃ��Ȃ�viewPos.z = 1.0f�A�t�H�O���������Ȃ��Ȃ�viewPos.z = 0.0f�łȂ���΂Ȃ�Ȃ��B
����́A�Q�ƃe�[�u���̉������Ђ�����Ԃ��Ă���̂��_���B
�������Ђ�����Ԃ��̂ł͂Ȃ��A�c�����Ђ�����Ԃ��̂������B
void Fog::Update(Camera* camera)
{
  if( !m_lookup_table_resource ) return;




  // ��`
  const f32 FIX12_ONE_U32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128����Ȃ�������ȉ��̃\�[�X�����藧���Ȃ��̂ŃR���p�C�����Ƀ`�F�b�N

  // �ϐ�
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // �Q�ƃe�[�u����ǂ�
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = m_lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(m_lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(m_lookup_table_index_in_lookup_table_set);

    const s32 LOOKUP_TABLE_DATA_NUM = 259;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 || i==131 )
      {
        // �w�b�_�݂����Ȃ��̂���
        // �������Ȃ�
      }
      else if( i==129 || i==259 )
      {
        // ��؂�ɂł��g���Ă�����̂���
        // �������Ȃ�
      }
      else if ( i>=260 )
      {
        // �����Ƃ����f�[�^�ł͂Ȃ�������
        // �������Ȃ�
      }
      else
      {
        // �L���ȃf�[�^
        
        //if (viewPos.z <= fogUpdater.GetMinFogDepth())
        //{
        //  viewPos.z = 1.0f;
        //}
        //else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        //{
        //  viewPos.z = 0.0f;
        //}
        //�ɍ��킹�邽�߂ɂЂ�����Ԃ��B
       
        b32 b_use = false;
        if( lookup_table_data_index < 8 )
        {
          if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
          {
            b_use = true;
          }
        }
        else
        {
          if( lookup_table_data_index % 2 == 1 )  // 9, 11, 13, 15, ... , 255
          {
            b_use = true;
          }
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF ); 
          lookup_table_data_value[HALF_TABLE_SIZE-1-index] = value_down;
          ++index;
        }
        ++lookup_table_data_index;
      }

#if 0
      {
        u32 value = nw_res_image_lookup_table.GetCommandCache(i);
        u32 value_up   = ( value & 0xFFF000 ) >> 12;
        u32 value_down = ( value & 0xFFF ); 
        GFL_PRINT("fog lut GetCommandCache[%4d] %d = %d | %d\n", i, value, value_up, value_down);
      }
#endif

    }
  }

#if 0
  {
    for(s32 i=0; i<HALF_TABLE_SIZE; ++i)
    {
      GFL_PRINT("fog lut[%4d] %d\n", i, lookup_table_data_value[i]);
    }
  }
#endif

  // ����
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // �J����
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // �t�H�O�̎Q�ƃe�[�u���ɒl��ݒ肷��
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        //viewPos.z = 1.0f;
        viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32;
            // �Q�ƃe�[�u���̊J�n�I���l���J�n0�I��1�ɂȂ��Ă���Ƃ͌���Ȃ��̂ŁA���ߑł��̒l�ł͂Ȃ��Q�ƃe�[�u���̒l���g���B
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        //viewPos.z = 0.0f;
        viewPos.z = lookup_table_data_value[HALF_TABLE_SIZE-1] / FIX12_ONE_U32;
            // �Q�ƃe�[�u���̊J�n�I���l���J�n0�I��1�ɂȂ��Ă���Ƃ͌���Ȃ��̂ŁA���ߑł��̒l�ł͂Ȃ��Q�ƃe�[�u���̒l���g���B
      }
      else
      {
        if( i == 0 )
        {
          viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32;
        }
        else  // if ( i > 0 )
        {
          viewPos.z = lookup_table_data_value[index] / FIX12_ONE_U32;
        }
      }

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;
        
        fogSampler.SetCommandCache(commandIndex, value );
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }

}

#elif 0

FogTable���g����悤�ɂ����̂ŁA����͌Â��Ȃ����̂ŃR�����g�A�E�g
void Fog::Update(Camera* camera)
{
  if( !m_lookup_table_resource ) return;




  // ��`
  const u32 FIX12_ONE_U32_u32 = 4096;
  const f32 FIX12_ONE_U32_f32 = 4096.0f;
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128����Ȃ�������ȉ��̃\�[�X�����藧���Ȃ��̂ŃR���p�C�����Ƀ`�F�b�N

  // �ϐ�
  u32 lookup_table_data_value[HALF_TABLE_SIZE];
  u32 index;

  // �Q�ƃe�[�u����ǂ�
  {
    nw::gfx::ResGraphicsFile* nw_res_graphics_file = m_lookup_table_resource->GetNwResGraphicsFile();
    nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(m_lookup_table_set_index_in_resource);
    nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(m_lookup_table_index_in_lookup_table_set);

    const s32 LOOKUP_TABLE_DATA_NUM = 259;
    s32 lookup_table_data_index = 0;
    index = 0;
    for( s32 i=0; i<LOOKUP_TABLE_DATA_NUM; ++i )
    {
      if( i==1 || i==131 )
      {
        // �w�b�_�݂����Ȃ��̂���
        // �������Ȃ�
      }
      else if( i==129 || i==259 )
      {
        // ��؂�ɂł��g���Ă�����̂���
        // �������Ȃ�
      }
      else if ( i>=260 )
      {
        // �����Ƃ����f�[�^�ł͂Ȃ�������
        // �������Ȃ�
      }
      else
      {
        // �L���ȃf�[�^
        
        //if (viewPos.z <= fogUpdater.GetMinFogDepth())
        //{
        //  viewPos.z = 1.0f;
        //}
        //else if (viewPos.z > fogUpdater.GetMaxFogDepth())
        //{
        //  viewPos.z = 0.0f;
        //}
        //�ɍ��킹�邽�߂ɁA�㉺���Ђ�����Ԃ��B
       
        b32 b_use = false;
        if( lookup_table_data_index < 8 )
        {
          if( lookup_table_data_index % 2 == 0 )  // 0, 2, 4, 6
          {
            b_use = true;
          }
        }
        else
        {
          if( lookup_table_data_index % 2 == 1 )  // 9, 11, 13, 15, ... , 255
          {
            b_use = true;
          }
        }

        if( b_use )
        {
          u32 value = nw_res_image_lookup_table.GetCommandCache(i);
          u32 value_up   = ( value & 0xFFF000 ) >> 12;
          u32 value_down = ( value & 0xFFF );
          if( value_down > FIX12_ONE_U32_u32 * 10 )
          {
            GFL_ASSERT_MSG(0, "�t�H�O�̎Q�ƃe�[�u���̒l%d���ُ�ɑ傫���ł��B\n", value_down);
            value_down = 0;
          }
          if( value_down > FIX12_ONE_U32_u32 ) value_down = FIX12_ONE_U32_u32;
          lookup_table_data_value[index] = FIX12_ONE_U32_u32-value_down;
          ++index;
        }
        ++lookup_table_data_index;
      }

#if 0
      {
        u32 value = nw_res_image_lookup_table.GetCommandCache(i);
        u32 value_up   = ( value & 0xFFF000 ) >> 12;
        u32 value_down = ( value & 0xFFF ); 
        GFL_PRINT("fog lut GetCommandCache[%4d] %d = %d | %d\n", i, value, value_up, value_down);
      }
#endif

    }
  }

#if 0
  {
    for(s32 i=0; i<HALF_TABLE_SIZE; ++i)
    {
      GFL_PRINT("fog lut[%4d] %d\n", i, lookup_table_data_value[i]);
    }
  }
#endif

  // ����
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // �J����
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // �t�H�O�̎Q�ƃe�[�u���ɒl��ݒ肷��
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 0.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if( !m_enable_flag )
      {
        viewPos.z = 1.0f;
            // �������A�t�H�O�������̂Ƃ��͑S���o���Ă͂����Ȃ��̂ŁA1.0f�ɂ���B
      }
      else
      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        //viewPos.z = 1.0f;
        viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32_f32;
            // �Q�ƃe�[�u���̊J�n�I���l���J�n0�I��1�ɂȂ��Ă���Ƃ͌���Ȃ��̂ŁA���ߑł��̒l�ł͂Ȃ��Q�ƃe�[�u���̒l���g���B
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        //viewPos.z = 0.0f;
        viewPos.z = lookup_table_data_value[HALF_TABLE_SIZE-1] / FIX12_ONE_U32_f32;
            // �Q�ƃe�[�u���̊J�n�I���l���J�n0�I��1�ɂȂ��Ă���Ƃ͌���Ȃ��̂ŁA���ߑł��̒l�ł͂Ȃ��Q�ƃe�[�u���̒l���g���B
      }
      else
      {
        /*
        ����͂��������B
        if( i == 0 )
        {
          viewPos.z = lookup_table_data_value[0] / FIX12_ONE_U32_f32;
        }
        else  // if ( i > 0 )
        {
          viewPos.z = lookup_table_data_value[index] / FIX12_ONE_U32_f32;
        }
        */
        
        s32 table_i = static_cast<s32>( (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()) * HALF_TABLE_SIZE );
        if( table_i < 0 ) table_i = 0;
        else if( table_i >= HALF_TABLE_SIZE ) table_i = HALF_TABLE_SIZE -1;
        viewPos.z = lookup_table_data_value[table_i] / FIX12_ONE_U32_f32;  // @note �l��⊮���ׂ���(table_i-1)��(i)�ŁB����ł͒i�X�ɂȂ肻���B
      }

      viewPos.z = 1.0f - ( 1.0f - viewPos.z ) * m_intensity;

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;
        
        fogSampler.SetCommandCache(commandIndex, value );
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }

}

#else

//----------------------------------------------------------------------------
/**
 *	@brief  �t�H�O�̍X�V�i���ۂɎg���Ă���֐��j
 */
//-----------------------------------------------------------------------------
void Fog::Update(Camera* camera)
{
  switch( m_type )
  {
  case TYPE_CREATE_FOG_TABLE:          UpdateUsingFogTable(camera);    break;
  case TYPE_USE_LOOKUP_TABLE_RESOURCE: UpdateUsingLookupTable(camera); break;
  }
}

#endif

//----------------------------------------------------------------------------
/**
 *	@brief  �w�肳�ꂽ�t�H�O�e�[�u�����g�p���Ă̍X�V
 */
//-----------------------------------------------------------------------------
void Fog::UpdateUsingFogTable(Camera* camera)
{
  if( !m_fog_table ) return;
  this->updateUsingFogTable( camera, *m_fog_table );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �w�肳�ꂽ�Q�ƃe�[�u�����g�p���Ă̍X�V
 */
//-----------------------------------------------------------------------------
void Fog::UpdateUsingLookupTable(Camera* camera)
{
  if( !m_lookup_table_resource ) return;

  // �t�H�O�e�[�u���𔽉f
  this->updateUsingFogTable( camera, *m_fog_table );
}


//----------------------------------------------------------------------------
/**
 *	@brief  �t�H�O�e�[�u�����g�p�����t�H�O�̍X�V
 */
//-----------------------------------------------------------------------------
void Fog::updateUsingFogTable(Camera* camera, const FogTable& fog_table )
{
  // ��`
  const u32 HALF_TABLE_SIZE = nw::gfx::FOG_TABLE_SIZE / 2;
  STATIC_ASSERT(HALF_TABLE_SIZE == 128);  // 128����Ȃ�������ȉ��̃\�[�X�����藧���Ȃ��̂ŃR���p�C�����Ƀ`�F�b�N
  STATIC_ASSERT(FOG_TABLE_VALUE_NUM == 128);

  // �ϐ�
  u32 index;

  // ����
  nw::gfx::res::ResImageLookupTable fogSampler = GetNwResFog().GetFogSampler();
  nw::gfx::res::ResFogUpdater fogUpdater = GetNwResFogUpdater();
  const gfl::math::MTX44& inverseProjectionMatrix = camera->GetNwCamera()->InverseProjectionMatrix();

  // �J����
  f32 l_Near = camera->GetNearClip();
  f32 l_Far = camera->GetFarClip();
  f32 l_WScale = camera->GetWScale();

  // �t�H�O�̎Q�ƃe�[�u���ɒl��ݒ肷��
  {
    enum
    {
      REG_FOG_TABLE_INDEX = 0xe6,
      REG_FOG_TABLE_PARAM = 0xe8
    };

    const u32 HEADER_FOG_TABLE_INDEX = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_INDEX, 1, false, 0xF);
    const u32 HEADER_FOG_TABLE_PARAM = nw::gfx::internal::MakeCommandHeader(REG_FOG_TABLE_PARAM, 128, false, 0xF);

    fogSampler.SetCommandCache(0, 0);
    fogSampler.SetCommandCache(1, HEADER_FOG_TABLE_INDEX);
    fogSampler.SetCommandCache(3, HEADER_FOG_TABLE_PARAM);

    index = 0;
    gfl::math::VEC4 viewPos;
    f32 prevLut = 1.0f;
    
    for (s32 i = 0; i <= HALF_TABLE_SIZE; ++i)
    {
      if (l_WScale == 0.0f)
      {
        f32 depth = -(f32)i / (f32)( HALF_TABLE_SIZE );
        viewPos.w = inverseProjectionMatrix.f._32 * depth + inverseProjectionMatrix.f._33;
        viewPos.z = -(inverseProjectionMatrix.f._22 * depth + inverseProjectionMatrix.f._23) / viewPos.w;
      }
      else
      {
        viewPos.z = ((f32)i / (f32)HALF_TABLE_SIZE) * (l_Far - l_Near) + l_Near;
      }

      if( !m_enable_flag )
      {
        viewPos.z = 0.0f;
      }
      else
      // �Q�ƃe�[�u���̊J�n�I���l���J�n0�I��1�ɂȂ��Ă���Ƃ͌���Ȃ��̂ŁA���ߑł��̒l�ł͂Ȃ��Q�ƃe�[�u���̒l���g���B
      if (viewPos.z <= fogUpdater.GetMinFogDepth())
      {
        viewPos.z = fog_table.value[0];
      }
      else if (viewPos.z > fogUpdater.GetMaxFogDepth())
      {
        viewPos.z = fog_table.value[HALF_TABLE_SIZE-1];
      }
      else
      {
        s32 table_i = static_cast<s32>( (viewPos.z - fogUpdater.GetMinFogDepth()) / (fogUpdater.GetMaxFogDepth() - fogUpdater.GetMinFogDepth()) * HALF_TABLE_SIZE );
        if( table_i < 0 ) table_i = 0;
        else if( table_i >= HALF_TABLE_SIZE ) table_i = HALF_TABLE_SIZE -1;
        viewPos.z = fog_table.value[table_i];
      }

      viewPos.z = 1.0f - (viewPos.z * m_intensity);

      if ( i > 0 )
      {
        u32 value = nw::ut::Fixed13::Float32ToFixed13(viewPos.z - prevLut) | (nw::ut::Fixed11::Float32ToFixed11(prevLut) << 13);
        u32 commandIndex = (index == 0) ? 2 : index + 3;

        fogSampler.SetCommandCache(commandIndex, value );

#if GFL_DEBUG
        static bool s_flag = false;
        
        if(s_flag){
          GFL_PRINT( "index %d value %f\n", commandIndex, prevLut );
        }
#endif // GFL_DEBUG
        
        ++index;
      }
      
      prevLut = viewPos.z;
    }
  }
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

