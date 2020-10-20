//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dModel.cpp
 *	@brief  H3D���f��
 *	@author	Koji Kawada
 *	@date		2012.04.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*gfx�̉��L�̏ꍇ�ɂ������킹�Ă��Ȃ��B
����ȊO�̏ꍇ�ɂ����킹�����Ƃ��́A����h3d�N���X�ɂ��̕��̕����p�ӂ���K�v������B
gfl::grp::g3d::RenderSystem
    m_render_sort_mode = nw::gfx::ISceneUpdater::ALL_MESH_BASE_SORT
    m_depth_sort_mode  = nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH
    ���C���[0 = nw::gfx::CreatePriorMaterialAndZeroDepthRenderKeyFactory
    ���C���[1 = nw::gfx::CreatePriorDepthReverseDepthRenderKeyFactory
    ���C���[2 = nw::gfx::CreatePriorMaterialAndZeroDepthRenderKeyFactory
    ���C���[3 = nw::gfx::CreatePriorMaterialAndZeroDepthRenderKeyFactory
��L�̐ݒ�ɂ����
    ���C���[0�A���C���[2�A���C���[3��
        �EZ�[�x�����߂Ȃ�
    ���C���[1��
        �E���b�V���P�ʂ�Z�[�x�����߂�
�ƂȂ��Ă���B*/
#endif


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



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dEnvironment.h>
#include <grp/g3d/gfl_G3dH3dCommand.h>
#include <grp/g3d/gfl_G3dH3dAnimationManager.h>
#include <grp/g3d/gfl_G3dH3dEx.h>
#include <grp/g3d/gfl_G3dH3dModel.h>
#include <grp/g3d/gfl_G3dH3dUtil.h>
#include <grp/g3d/gfl_G3dH3dCommandUtil.h>
#include <grp/g3d/gfl_G3dH3dUserData.h>

// gflib grp g3d ����J�w�b�_
#include "gfl_G3dRenderElementAccessor.h"
#include "grp/util/gfl_UtilDrawUtil.h"



namespace gfl {
namespace grp {
namespace g3d {
  
//---------------------------------------------------------------------------------
/**
 * @brief GeometryShaderCommand�N���X
 */
//---------------------------------------------------------------------------------
class GeometryShaderCommand{
public:
  //! 0x200�ɐݒ肷��_�~�[�R�}���h�̐��ł��B        
  static const s32 DUMMY_DATA_NUM_200 = 30;
  
  //! 0x251�ɐݒ肷��_�~�[�R�}���h�̐��ł��B        
  static const s32 DUMMY_DATA_NUM_251 = 10;
  
  //! �_�~�[�R�}���h�ȂǁA�p�f�B���O�ɂ����l�ł��B         
  static const u32 PADDING_DATA = 0xead0fead;
  
  static bit32* MakeShaderModeCommand( bit32* command, const bool isEnableGeoShader )
  {
    // 0x25e[9:8] �ւ̐ݒ�
    { 
        if ( isEnableGeoShader )
        {
            *command++ = PICA_DATA_DRAW_GEOMETRY_PRIMITIVE << 8;
        }
        else
        {
            *command++ = PICA_DATA_DRAW_TRIANGLES << 8;
        }
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_OUT_REG_NUM3, 2 );
    }

    // 0x251 �ւ̃_�~�[�R�}���h
    {
        command = MakeDummyCommand_( command, PICA_REG_VS_OUT_REG_NUM2, DUMMY_DATA_NUM_251 );
    }

    // 0x200 �ւ̃_�~�[�R�}���h
    {
        command = MakeDummyCommand_( command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200 );
    }

    // 0x229[1:0] �W�I���g���V�F�[�_�[�̗L���A������ݒ�
    {
        *command++ = isEnableGeoShader ? 2 : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_DRAW_MODE0, 1 );
    }

    // 0x200 �ւ̃_�~�[�R�}���h
    {
        command = MakeDummyCommand_( command, PICA_REG_VERTEX_ATTR_ARRAYS_BASE_ADDR, DUMMY_DATA_NUM_200 );
    }

    // 0x244 �W�I���g���V�F�[�_�[�̐ݒ�𒸓_�V�F�[�_�[�Ƌ��L���邩�ǂ���
    { 
        *command++ = isEnableGeoShader ? 1 : 0;
        *command++ = PICA_CMD_HEADER_SINGLE_BE( PICA_REG_VS_COM_MODE, 1 );
    }

    return command;
  }

private:
  
  GeometryShaderCommand()
  {
    
  }

  ~GeometryShaderCommand()
  {

  }
  
  static bit32* MakeDummyCommand_( bit32* command, const bit32 load_reg, const u32 dataNum )
  {
    *command++ = 0;
    *command++ = PICA_CMD_HEADER_BURST_BE( load_reg, dataNum, 0 );

    for ( s32 i = 0; i < dataNum - ( dataNum & 1 ); ++i)
    {
        *command++ = PADDING_DATA;
    }

    return command;
  }
};  

//-----------------------------------------------------------------------------
/**
 *					�N���X����
*/
//-----------------------------------------------------------------------------
  
//-------------------------------------
///	H3D���f���N���XDescription�\����
//=====================================  
H3dModel::Description::Description(void) :
  delete_memory_control(true),
  maxFragmentLightNum( 4 )
{
  
}  

//-------------------------------------
///	H3D���f���N���X
//=====================================

#if GFL_DEBUG
b32 H3dModel::s_SafeDeleteCheckEnable        = false;
b32 H3dModel::s_SafeDeleteCheckEnableUserSet = false;
b32 H3dModel::s_SafeDeleteCheckForce         = false;
b32 H3dModel::s_SafeDeleteCheckAssert        = false;
#endif  

#if GFL_DEBUG
b32 H3dModel::s_DeleteMemoryControlEnable = true;
b32 H3dModel::s_DeleteMemoryControlAssert = false;
#endif  
  

#define MODEL_COORDINATE_ENABLED 0  // ���ꂪ1�̂Ƃ��A�X�P���g�����L���ł���B���ꂪ0�̂Ƃ��A�X�P���g�����L���ł��Ȃ��B

H3dModel::LastMaterialData          H3dModel::s_LastMaterialData;
bool                                H3dModel::s_DebugFlag = false;
//�L���b�V���𖳌�������e�X�g
//#define MATERIAL_CACHE_DISABLE

H3dModel::H3dModel(void)
{
  initParam();
  
  GFL_SINGLETON_INSTANCE(System)->GetH3dModelManager()->AddObject(this);

  // gfx�Ah3d���C�u�����̒l���ς���Ă���Ƃ����Ȃ��̂Ń`�F�b�N����
  STATIC_ASSERT(nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_OPAQUE      == nw::h3d::TranslucencyLayer::LAYER0);
  STATIC_ASSERT(nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_TRANSLUCENT == nw::h3d::TranslucencyLayer::LAYER1);
  STATIC_ASSERT(nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_SUBTRACTIVE == nw::h3d::TranslucencyLayer::LAYER2);
  STATIC_ASSERT(nw::gfx::res::ResMaterial::TRANSLUCENCY_KIND_ADDITIVE    == nw::h3d::TranslucencyLayer::LAYER3);
}

void H3dModel::initParam(void)
{
  m_res = NULL;
  m_cont_index = H3dResource::INDEX_NONE;
  m_cont = NULL;
  m_pMemoryObject = NULL;
  m_mate_constant_color_array = NULL;
  m_mate_ex_state_array = NULL;
  m_mate_translucency_kind_array = NULL;
  m_shader_cont_array = NULL;
  m_texture_info_array = NULL;
  m_bit_flag = BIT_FLAG_DEFAULT;
  m_scale_vec.Set(1.0f, 1.0f, 1.0f);
  m_rotate_vec.Set(0.0f, 0.0f, 0.0f);
  m_translate_vec.Set(0.0f, 0.0f, 0.0f);
  m_animation_manager = NULL;
  m_scene = NULL;
  m_pre_mesh_draw_listener_head = NULL;
  m_post_mesh_draw_listener_head = NULL;
  m_model_layer_id = 0;
  m_FragmentAndWBufferDirty = true;
  m_replace_bone_data_array = NULL;
  m_mate_proj_array = NULL;
  m_fVisible = true;
  
  m_world_mtx.SetupIdentity();
  m_normal_mtx.SetupIdentity();
  m_rotate_mtx.SetupIdentity();
}

H3dModel::~H3dModel()
{
  Destroy();

  GFL_SINGLETON_INSTANCE(System)->GetH3dModelManager()->RemoveObject(this);
}

b32 H3dModel::Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const H3dResource* resource,
    const char* name,
    const Description* description
)
{
  s32 index_in_resource = resource->GetModelContentIndex(name);
  return Create(heap_allocator, device_allocator, resource, index_in_resource, description);
}

b32 H3dModel::Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    const H3dResource* resource,
    s32 index_in_resource,
    const Description* description
)
{
  Description l_description;

  if( description ){ l_description = *description; }
  
  {
    // resource���o���Ă���
    if( resource )
    {
      GFL_ASSERT_MSG(resource->IsSetup(), "���\�[�X�����S�ɃZ�b�g�A�b�v����Ă��܂���B\n");  //@check �����N����Ȃ����̂��c�����܂ܐi�ށB

      m_res = resource;
      m_cont_index = index_in_resource;
      m_cont = m_res->GetNwModelContent(m_cont_index);
      if( !m_cont )
      {
        GFL_ASSERT_MSG(0, "index_in_resource=%d�Ŏ擾�����R���e���g��NULL�ł��B\n", index_in_resource);
        m_res = NULL;
        m_cont_index = H3dResource::INDEX_NONE;
        return false;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "resource��NULL�ł��B\n");
      return false;
    }
  }

  {
    // ���[���h�s��
    m_world_mtx = m_cont->worldMtx;
    if( !(m_world_mtx.IsIdentity()) )
    {
      // ���f���̃��[���h�s�񂪃��\�[�X�ɐݒ肳��Ă���Ƃ�
      SetBitFlagOff( &m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC );
      m_rotate_mtx = m_world_mtx;
      m_rotate_mtx.SetColumn(3, gfl::math::VEC3(0.0f, 0.0f, 0.0f));
      m_translate_vec = m_world_mtx.GetColumn(3);
    }
  }

  {
    // �K�v�ȃ������T�C�Y�̌v�Z�ƃ������̊m��
    nw::h3d::MaterialState::Description desc;

    desc.maxFragmentLightSize = l_description.maxFragmentLightNum;
    nw::h3d::res::ModelContent::StateSizeTable sizeTable;
    m_cont->CalcStateSize(&sizeTable, desc);
    
    m_pMemoryObject = GFL_NEW(heap_allocator->GetHeapBase()) MemoryObject;
    m_pMemoryObject->m_state_heap_mem   = GflHeapAllocMemoryAlign(heap_allocator->GetHeapBase(), sizeTable.GetTotalMemorySize(), 4);
    m_pMemoryObject->m_state_device_mem = GflHeapAllocMemoryAlign(device_allocator->GetHeapBase(), sizeTable.GetTotalDeviceMemorySize(), 4);
    #if GFL_DEBUG    
      if ( s_SafeDeleteCheckEnable )
      {
        m_pMemoryObject->m_HeapSize = sizeTable.GetTotalMemorySize();
        m_pMemoryObject->m_DeviceSize = sizeTable.GetTotalDeviceMemorySize();
        m_pMemoryObject->SetModelName( m_cont->name );
      }
    #endif
    
    // �X�e�[�g������
    nw::h3d::ResultWith<void*> result;
    result = m_cont->InitState(
        &m_mate_state_array, &m_skel_state, &m_visi_state,
        m_pMemoryObject->m_state_heap_mem, m_pMemoryObject->m_state_device_mem, sizeTable);

    GFL_ASSERT_MSG(result.GetResult().IsSuccess(), "���f���X�e�[�g���������s\n");  //@check
    if( !(result.GetResult().IsSuccess()) )
    {
      GFL_ASSERT_MSG(0, "���f���X�e�[�g���������s\n");
      Destroy();
      return false;
    }
  }

  u32 material_num = GetMaterialCount();
  
  {
    // �R���X�^���g�J���[
    if( material_num > 0 )
    {
      m_mate_constant_color_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::ColorU8*[material_num];
      for(s32 i=0; i<material_num; ++i)
      {
        m_mate_constant_color_array[i] = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::ColorU8[CONSTANT_COLOR_NUM];
        for(s32 j=0; j<CONSTANT_COLOR_NUM; ++j)
        {
          m_mate_constant_color_array[i][j] = gfl::grp::ColorU8(0,0,0,1);  // �f�U�C�i�[����c�[���̃f�t�H���g�l�ɍ��킹�Ă���
        }

        // �ݒ�ς݂̒l�𔽉f���Ă��� 
        {
          
          gfl::grp::ColorU8 constant_color;
          
          s32 constant_color_no = 0;
          
          for( constant_color_no = 0; constant_color_no < 6; ++constant_color_no )
            m_mate_constant_color_array[i][constant_color_no] = m_mate_state_array[i].GetConstantColor(constant_color_no);
          
#if 0    
        // �y���Ӂznw::h3d::MaterialState::GetCombiner0Constant�ƃf�U�C�i�[����c�[���̈Ⴂ�ɒ��ӁI
        // �R���o�C�i�O�p�R���X�^���g�J���[���擾���܂��B�R���X�^���g�J���[�O�ł͂Ȃ��̂Œ��ӂ��Ă��������B
        #define INITIALIZE_CONSTANT_COLOR(step) \
          constant_color_no = this->GetMaterialTextureCombinerConstant(i, step); \
          {\
            const nw::h3d::MaterialState& materialState = m_mate_state_array[i];\
            bit32 assignment = materialState.GetCombinerConstantAssignment();\
            constant_color_no = nw::h3d::fnd::Util::GetCombinerConstantSource(step,assignment);\
          }\
          constant_color = m_mate_state_array[i].GetConstantColor(constant_color_no); \
          if( 0<=constant_color_no && constant_color_no<CONSTANT_COLOR_NUM ) m_mate_constant_color_array[i][constant_color_no] = constant_color; \
          else GFL_ASSERT(0);
      
          INITIALIZE_CONSTANT_COLOR(0)
          INITIALIZE_CONSTANT_COLOR(1)
          INITIALIZE_CONSTANT_COLOR(2)
          INITIALIZE_CONSTANT_COLOR(3)
          INITIALIZE_CONSTANT_COLOR(4)
          INITIALIZE_CONSTANT_COLOR(5)

        #undef INITIALIZE_CONSTANT_COLOR
#endif
        }
      }
    }
    else
    {
      m_mate_constant_color_array = NULL;
    }
  }

  {
    // H3D�}�e���A���ǉ��X�e�[�g
    if( material_num > 0 )
    {
      m_mate_ex_state_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) H3dMaterialExState[material_num];
      for(s32 i=0; i<material_num; ++i)
      {
        m_mate_ex_state_array[i].Initialize(device_allocator, m_cont, i);

        nw::h3d::res::TextureContent      *pTextureContent[3] = {
          m_cont->materials[i].texture0,
          m_cont->materials[i].texture1,
          m_cont->materials[i].texture2
        };
        
        for(s32 no=0; no<H3dTextureSamplerExConst::TEXTURE_NUM; ++no)
        {
          s32     mipmapSize = 1;
          
          if ( pTextureContent[no] )
            mipmapSize = pTextureContent[no]->mipmapSize;
          
          m_mate_ex_state_array[i].InitializeTextureSampler(no, &(m_cont->materials[i].textureSamplers[no]), mipmapSize );
        }
      }
    }
    else
    {
      m_mate_ex_state_array = NULL;
    }
  }

  {
    //�v���W�F�N�V�������[�U�[�f�[�^�z��m��
    if( material_num > 0 )
    {
      m_mate_proj_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) u32[material_num];
      for(s32 i=0; i<material_num; ++i)
      {
        m_mate_proj_array[i] = MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
      }
    }
    else
    {
      m_mate_proj_array = NULL;
    }
  }

  {
    // �}�e���A���̕`�惌�C���[
    if( material_num > 0 )
    {
      m_mate_translucency_kind_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) MaterialTranslucencyKind[material_num];
      for(s32 i=0; i<material_num; ++i)
      {
        m_mate_translucency_kind_array[i] = MATERIAL_TRANSLUCENCY_KIND_LAYER0;
      }
      initializeMaterialTranslucencyKind();
    }
  }

  {
    // ���ݎg���Ă���V�F�[�_
    if( material_num > 0 )
    {
      m_shader_cont_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) const nw::h3d::res::ShaderContent*[material_num];
      for(s32 i=0; i<material_num; ++i)
      {
        const nw::h3d::res::MaterialData& material = m_cont->materials[i];
        m_shader_cont_array[i] = material.content->shaderContent;
      }
    }
  }

/*
  �V�F�[�_��ύX����x��SearchBindSymbol�͂�蒼���˂΂Ȃ�Ȃ��̂ŁA
  ���񂾂��ݒ肷�邱�̋L�q�̓R�����g�A�E�g�B
  {
    // �V�F�[�_�[�V���{���̃o�C���h
    s32 shaderBindSize = m_cont->materials.size() * sizeof(ShaderBind);
    m_shader_bind_array.SetUninitializedMemory(
        GflHeapAllocMemoryAlign(heap_allocator->GetHeapBase(), shaderBindSize, 4), shaderBindSize);

    ShaderBindArray::Range binds = m_shader_bind_array.GetRange();
    for(ShaderBindArray::iterator bind = binds.first; bind != binds.second; ++bind)
    {
      int idx = ::std::distance(binds.first, bind);
      nw::h3d::res::MaterialContent* material = m_cont->materials[idx].content;
      nn::gr::CTR::ShaderLite& shader = material->shaderContent->binding->shader;
 
      shader.SearchBindSymbol(&bind->worldMtx, "WrldMtx");
      shader.SearchBindSymbol(&bind->normalMtx, "NormMtx");
      shader.SearchBindSymbol(&bind->projMtx, "ProjMtx");
      shader.SearchBindSymbol(&bind->viewMtx, "ViewMtx");
      shader.SearchBindSymbol(&bind->univMtx, "UnivReg");
 
      shader.SearchBindSymbol(&bind->hslGCol, "HslGCol");
      shader.SearchBindSymbol(&bind->hslSCol, "HslSCol");
      shader.SearchBindSymbol(&bind->hslSDir, "HslSDir");
 
      shader.SearchBindSymbol(&bind->matAmbi, "MatAmbi");
      shader.SearchBindSymbol(&bind->matDiff, "MatDiff");
    }
  }
*/
  {
    // �V�F�[�_�[�V���{���̃o�C���h
    s32 shaderBindSize = m_cont->materials.size() * sizeof(ShaderBind);
    m_shader_bind_array.SetUninitializedMemory(
        GflHeapAllocMemoryAlign(heap_allocator->GetHeapBase(), shaderBindSize, 4), shaderBindSize);

    for(s32 i=0; i<material_num; ++i)
    {
      setShaderBind(i, m_shader_cont_array[i]);
    }
  }

  {
    // �e�N�X�`�����
    if( material_num > 0 )
    {
      m_texture_info_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) TextureInfo[material_num];
      
      // �����l���擾����
      for(s32 i=0; i<material_num; ++i)
      {
        for(s32 j=0; j<H3dTextureSamplerExConst::TEXTURE_NUM; ++j)
        {
          initializeModelTextureInfo(i, j);
        }
      }
    }
  }

  // ������Ƀ������R���g���[�����邩�H
  if( l_description.delete_memory_control ){
#if GFL_DEBUG
    if( s_DeleteMemoryControlEnable )
    {
      SetBitFlagOn( &m_bit_flag, BIT_FLAG_DEL_MEM_CONTROL );
    }
    m_pMemoryObject->SetModelName( m_cont->name );
    if( s_DeleteMemoryControlAssert )
    {
      GFL_ASSERT_MSG( 0, "���f����%s��delete_memory_control=true�Ő�������܂����B\n\n", m_pMemoryObject->GetModelName() );
    }
    else
    {
      GFL_PRINT( "���f����%s��delete_memory_control=true�Ő�������܂����B\n\n", m_pMemoryObject->GetModelName() );
    }
#else
    SetBitFlagOn( &m_bit_flag, BIT_FLAG_DEL_MEM_CONTROL );
#endif
  }

  // �`��L���b�V�������@�ݒ�
  SetBitFlagOn( &m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH );

  return true;
}

H3dModel::MemoryObject::MemoryObject() : H3dModelMemoryManager::IMemortObject(),
  m_state_heap_mem(NULL),
  m_state_device_mem(NULL),
  m_MaterialCommandChaches(NULL)
{
  
}

H3dModel::MemoryObject::~MemoryObject()
{
#if GFL_DEBUG
  if ( s_SafeDeleteCheckEnable )
  {
    u32   cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();

    if ( ( m_DrawFrameCount + 2 ) > cnt )
    {
      if( s_SafeDeleteCheckAssert )
      {
        GFL_ASSERT_MSG( 0, "���f����%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
      else
      {
        GFL_PRINT( "���f����%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
    }
  }
#endif
  
  if(m_state_heap_mem)
  {
#if GFL_DEBUG
    //����ɂ��̃��������ʃA�v�����ɂ���Ċm�ۂ��ꏑ��������ꂽ�Ƒz�肵�ď��������܂��B
    //�`�撆�ł���΂f�o�t�̓n���O����B
    if ( s_SafeDeleteCheckEnable )
      memset( m_state_heap_mem, 0, m_HeapSize );
#endif    
    GflHeapFreeMemory( m_state_heap_mem );
  }
  
  if(m_state_device_mem)
  {
#if GFL_DEBUG
    //����ɂ��̃��������ʃA�v�����ɂ���Ċm�ۂ��ꏑ��������ꂽ�Ƒz�肵�ď��������܂��B
    //�`�撆�ł���΂f�o�t�̓n���O����B
    if ( s_SafeDeleteCheckEnable )
      memset( m_state_device_mem, 0, m_DeviceSize );
#endif        
    GflHeapFreeMemory( m_state_device_mem );
  }
  m_state_heap_mem    = NULL;
  m_state_device_mem  = NULL;
  
  GFL_SAFE_DELETE_ARRAY( m_MaterialCommandChaches );
}

#if GFL_DEBUG    
void H3dModel::MemoryObject::SetModelName( const char* pName )
{
  memset( m_ModelName, 0, sizeof(m_ModelName) );
  s32   len = strlen( pName );
  
  if ( len >= sizeof(m_ModelName) )
    len = sizeof(m_ModelName) - 1;//���[null�����L�[�v
  
  memcpy( m_ModelName, pName, len );
}
#endif
    
void H3dModel::Destroy(void)
{
  DestroyReplaceBoneData();

  if( m_cont )  // �����Create����Ă���Ƃ���m_cont��NULL�łȂ�
  {

    // �V�[���ɓo�^����Ă���΁A�O��
    if( m_scene ){
      if( !( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_DEL_MEM_CONTROL ) ) ){
        GFL_ASSERT_STOP_MSG( 0, "H3dModel Scene�ɓo�^���ꂽ��ԂŁA�j�����Ă��܂�" );
      }
      m_scene->RemoveH3dModelFromCalcAndAllDraw(this);
    }

    u32 material_num = GetMaterialCount();
    
    if( !( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_DEL_MEM_CONTROL ) ) ){
      GFL_SAFE_DELETE( m_pMemoryObject );
    }
    else{
      // 1�t���[����ɉ��
      GFL_SINGLETON_INSTANCE(System)->GetH3dModelMemoryManager()->SetDeleteMemory( m_pMemoryObject );
    }
 
    if( m_mate_constant_color_array )
    {
      for(s32 i=0; i<material_num; ++i)
      {
        if( m_mate_constant_color_array[i] ) GFL_DELETE_ARRAY m_mate_constant_color_array[i];
      }
      GFL_DELETE_ARRAY m_mate_constant_color_array;
      m_mate_constant_color_array = NULL;
    }
 
    if( m_mate_ex_state_array )
    {
      GFL_DELETE_ARRAY m_mate_ex_state_array;
      m_mate_ex_state_array = NULL;
    }

    if( m_mate_proj_array )
    {
      GFL_DELETE_ARRAY m_mate_proj_array;
      m_mate_proj_array = NULL;
    }

    if( m_mate_translucency_kind_array )
    {
      GFL_DELETE_ARRAY m_mate_translucency_kind_array;
      m_mate_translucency_kind_array = NULL;
    }

    void* mem = m_shader_bind_array.ReleaseMemory();  if(mem) GflHeapFreeMemory(mem);
        // nw::h3d::fnd::DataArray��ReleaseMemory���Ă��΃R���X�g���N�^�Ăяo������Ɠ�����������ԂɂȂ�B

    if( m_shader_cont_array )
    {
      GFL_DELETE_ARRAY m_shader_cont_array;
      m_shader_cont_array = NULL;
    }

    if( m_texture_info_array )
    {
      GFL_DELETE_ARRAY m_texture_info_array;
      m_texture_info_array = NULL;
    }

    DestroyAnimationManager();


    // �R���X�g���N�^�Ɠ���������
    m_res = NULL;
    m_cont_index = H3dResource::INDEX_NONE;
    m_cont = NULL;
    m_pMemoryObject   = NULL;
 
    m_world_mtx.SetupIdentity();
    m_normal_mtx.SetupIdentity();
 
    m_bit_flag = BIT_FLAG_DEFAULT;

    m_scale_vec = gfl::math::VEC3(1.0f, 1.0f, 1.0f);
    m_rotate_vec = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
    m_translate_vec = gfl::math::VEC3(0.0f, 0.0f, 0.0f);
 
    m_animation_manager = NULL;

    m_scene = NULL;

    m_pre_mesh_draw_listener_head = NULL;
    m_post_mesh_draw_listener_head = NULL;

    m_model_layer_id = 0;

    m_mate_proj_array = NULL;
  }
  
  initParam();
}


#if 0
/*������͍l������gfx�ƍ���Ȃ�


void H3dModel::Calculate(void)
{
#if 0
//��Calculate������O�̏���

  calculateTransform();
  calculateAnimationManager();

#if MODEL_COORDINATE_ENABLED
  m_skel_state.UpdateWorldMtxs(NULL);
#else
  m_skel_state.ForceUpdateWorldMtxs(&m_world_mtx);  //       �X�P���^���A�j���[�V���������Ă��Ȃ��Ƃ���Force�ɂ��Ă����Ȃ��Ɠ������Ȃ��B
#endif

#else
  CalculateBeforeCulling();
  CalculateWorldMatrix();

  CalculateAfterCulling();
  CalculateLocalSkeletalAnimation();
  CalculateVisibilityAnimation();
  CalculateMaterialAnimation();
  CalculateAnimationFrame();
  CalculateWorldSkeletalAnimation();
#endif
}



void H3dModel::CalculateBeforeCulling(void)
{
  // �������Ȃ�
}
void H3dModel::CalculateAfterCulling(void)
{
  // �������Ȃ�
}

void H3dModel::CalculateWorldMatrix(void)
{
  calculateTransform();
}
void H3dModel::CalculateLocalSkeletalAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateLocalSkeletalAnimation();
}
void H3dModel::CalculateVisibilityAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateVisibilityAnimation();
}
void H3dModel::CalculateMaterialAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateMaterialAnimation();
}
void H3dModel::CalculateAnimationFrame(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateAnimationFrame();
}
void H3dModel::CalculateWorldSkeletalAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateWorldSkeletalAnimation();

#if MODEL_COORDINATE_ENABLED
  m_skel_state.UpdateWorldMtxs(NULL);
#else
  m_skel_state.UpdateWorldMtxs (&m_world_mtx,true);  //       �X�P���^���A�j���[�V���������Ă��Ȃ��Ƃ���Force�ɂ��Ă����Ȃ��Ɠ������Ȃ��B
#endif
}
*/
#endif



void H3dModel::Calculate(void)
{

// NintendoWare for CTR 2.5.3 for SDK 4.2.5 H3D�p�b�` 2013/03/28�ňȏ�ŗL�� 
#if NW_CURRENT_VERSION_NUMBER >= NW_VERSION_NUMBER(2,5,4,0)
  nw::h3d::MaterialStateHelper::SwapFragmentCmdBuffers( m_mate_state_array );
#endif // 

  // �h���N���X�ŉ����������Ȃ�A���̊֐��ɏ����ĉ������B
}



void H3dModel::CalculateScene(void)
{
  CalculateBeforeCulling();
  CalculateWorldMatrix();

  CalculateAfterCulling();
  CalculateLocalSkeletalAnimation();
  CalculateVisibilityAnimation();
  CalculateMaterialAnimation();
  CalculateAnimationFrame();
  CalculateWorldSkeletalAnimation();
}


void H3dModel::CalculateBeforeCulling(void)
{
  // �������Ȃ�
}
void H3dModel::CalculateAfterCulling(void)
{
  // �������Ȃ�
}

void H3dModel::CalculateWorldMatrix(void)
{
  calculateTransform();
}
void H3dModel::CalculateLocalSkeletalAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateLocalSkeletalAnimation();
}
void H3dModel::CalculateVisibilityAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateVisibilityAnimation();
}
void H3dModel::CalculateMaterialAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateMaterialAnimation();
}
void H3dModel::CalculateAnimationFrame(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateAnimationFrame();
}
void H3dModel::CalculateWorldSkeletalAnimation(void)
{
  if( m_animation_manager ) m_animation_manager->CalculateWorldSkeletalAnimation();

#if MODEL_COORDINATE_ENABLED
  m_skel_state.UpdateWorldMtxs(NULL);
#else
  m_skel_state.UpdateWorldMtxs(&m_world_mtx,true);  //       �X�P���^���A�j���[�V���������Ă��Ȃ��Ƃ���Force�ɂ��Ă����Ȃ��Ɠ������Ȃ��B
#endif
}




void H3dModel::SubmitView(
  nw::gfx::RenderQueue::ElementListType* list,
  const nw::gfx::Camera& camera
)
{
  const gfl::math::MTX34& view_mtx = camera.ViewMatrix();
  
  gfl::math::MTX34 inv_view_mtx;
  u32 inv_exist = nn::math::MTX34Inverse(&inv_view_mtx, &view_mtx);

#if MODEL_COORDINATE_ENABLED
  nn::math::MTX34InvTranspose(&m_normal_mtx, &m_world_mtx);      // @note �y�v�������zDraw��m_normal_mtx��r���{�[�h�̌v�Z������悤�ɂ��Ȃ��ƁA
  nn::math::MTX34Mult(&m_normal_mtx, &view_mtx, &m_normal_mtx);  //       �����̃V�[���`��O���[�v�ɏ������Ă��郂�f����`�悷��Ƃ��ɁA
#else                                                            //       ��ԍŌ��SubmitView�����J�����̒l��Draw���邱�ƂɂȂ��Ă������B��GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH�ɂđΏ����Ă��邪�A�������ׂ������邱�Ƃ����O���Ă���define�𖳌��ɂ��Ă���B
  nn::math::MTX34Copy(&m_normal_mtx, &view_mtx);             
#endif

  if( inv_exist )                                          // @note �y�v�������zDraw��m_normal_mtx��r���{�[�h�̌v�Z������悤�ɂ��Ȃ��ƁA
  {                                                        //       �����̃V�[���`��O���[�v�ɏ������Ă��郂�f����`�悷��Ƃ��ɁA
    m_skel_state.UpdateBillboard(view_mtx, inv_view_mtx);  //       ��ԍŌ��SubmitView�����J�����̒l��Draw���邱�ƂɂȂ��Ă������B��GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH�ɂđΏ����Ă��邪�A�������ׂ������邱�Ƃ����O���Ă���define�𖳌��ɂ��Ă���B
  }
  GFL_ASSERT(inv_exist);  // @note �r���[�}�g���N�X�̋t�s�񂪋��߂��Ȃ��悤�ȂƂ�������̂��m�F�������̂�ASSERT�ɂ��Ă����B  //@check

  enqueue(list, camera);
}

// ���������Ɏc���Ă�������
/*void H3dModel::DrawOpa(
    H3dCommand* command,
    H3dEnvironment* env,
    const gfl::math::MTX34* view_mtx,
    bool fragmentAndWBufferDirty
)
{
#if MODEL_COORDINATE_ENABLED
  nn::math::MTX34InvTranspose(&m_normal_mtx, &m_world_mtx);
  nn::math::MTX34Mult(&m_normal_mtx, view_mtx, &m_normal_mtx);
#else
  nn::math::MTX34Copy(&m_normal_mtx, view_mtx);
#endif

  // �� DrawOpa() �� DrawXlu() �̂S�Ԗڈ�����(FRAGMENT_COMMAND_CACHE_ENABLED == 1)�̏ꍇ�̂�
  //    �L���ɂȂ�܂��B
  //
  // �t���O�����g���C�g�̐��A�ʒu���� WBuffer �Ɋւ���ݒ��ύX�����ꍇ�A DrawOpa() �� DrawXlu() ��
  // �S�Ԗڈ����� true ���w�肵�Ă��������B
  // ���̈����� true ���w�肷��ƃ��f���̃R�}���h�쐬���Ƀt���O�����g���C�g�̃R�}���h���Đ�������܂��B
  // ���t���[���Đ������s���ƁA�������ׂ��オ���Ă��܂��܂��B

  nw::h3d::fnd::DataArray<nw::h3d::res::MeshData>::Range meshes =
      m_cont->layeredMeshes[nw::h3d::TranslucencyLayer::LAYER0];
  for(; meshes.first < meshes.second; ++meshes.first)
  {
    nw::h3d::res::MeshData& mesh = *meshes.first;
    if(mesh.IsVisible(m_visi_state))
    {
      drawMesh(command, env, mesh, m_normal_mtx);
    }
  }
}*/

// ���������Ɏc���Ă�������
/*void H3dModel::DrawXlu(
    H3dCommand* command,
    H3dEnvironment* env,
    const gfl::math::MTX34* view_mtx,
    bool fragmentAndWBufferDirty
)
{
#if MODEL_COORDINATE_ENABLED
  nn::math::MTX34InvTranspose(&m_normal_mtx, &m_world_mtx);
  nn::math::MTX34Mult(&m_normal_mtx, view_mtx, &m_normal_mtx);
#else
  nn::math::MTX34Copy(&m_normal_mtx, view_mtx);
#endif

  nw::h3d::fnd::DataArray<nw::h3d::res::MeshData>::Range meshes = ::std::make_pair(
      m_cont->layeredMeshes[nw::h3d::TranslucencyLayer::LAYER1].first,
      m_cont->layeredMeshes[nw::h3d::TranslucencyLayer::LAYER3].second);
  for(; meshes.first < meshes.second; ++meshes.first)
  {
    nw::h3d::res::MeshData& mesh = *meshes.first;
    if(mesh.IsVisible(m_visi_state))
    {
      drawMesh(command, env, mesh, m_normal_mtx);
    }
  }
}*/

#if 0
void H3dModel::DrawMesh(
    const nw::h3d::res::MeshData& mesh,
    bool fragmentAndWBufferDirty
)
{
  if( m_scene )
  {
    m_scene->GetH3dCommand()->BegineDraw();
    m_scene->GetH3dEnvironment()->SetFromScene(m_scene);  // ���C�g��t�H�O���A�j���[�V�������Ă��邩������Ȃ��̂Ŗ��t���[���ݒ肷��

    drawMesh(m_scene->GetH3dCommand(), m_scene->GetH3dEnvironment(), mesh, m_normal_mtx);
    
    m_scene->GetH3dCommand()->EndDraw();
  }
}
#endif

void H3dModel::DrawMesh(
    H3dCommand* command,
    H3dEnvironment* env,
    const nw::h3d::res::MeshData& mesh,
    bool fragmentAndWBufferDirty
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    ,
    const gfl::math::MTX34&  view_mtx,
    const gfl::math::MTX34&  inv_view_mtx,
    const u32                inv_exist
#endif
)
{
  drawMesh(command, env, mesh, m_normal_mtx
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      ,
      view_mtx,
      inv_view_mtx,
      inv_exist
#endif
  );
}

H3dModel::MaterialTranslucencyKind H3dModel::GetMaterialTranslucencyKind(s32 material_index) const
{
  return m_mate_translucency_kind_array[material_index];
}
void H3dModel::SetMaterialTranslucencyKind(s32 material_index, H3dModel::MaterialTranslucencyKind kind)
{
  m_mate_translucency_kind_array[material_index] = kind;
}
void H3dModel::initializeMaterialTranslucencyKind(void)
{
  // �������m�ۂƃ}�e���A�����S�����̏������͍ς܂��Ă������ƁB
  // �����ł̓��b�V���ɐݒ肵�Ă���}�e���A���Ɋւ��Ă̂ݏ��������s���B�����}�e���A���ɑ΂��ĉ��x���s���\������(�㏟��)�B
  u32 num = m_cont->meshes.size();
  for(u32 i=0; i<num; ++i)
  {
    const nw::h3d::res::MeshData* mesh = &(m_cont->meshes[i]);
    s32 material_index = mesh->materialId;
    nw::gfx::res::ResMaterial::TranslucencyKind kind =
        static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(
            (mesh->key >> nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_SHIFT) & nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_MASK
        );
    m_mate_translucency_kind_array[material_index] = static_cast<MaterialTranslucencyKind>(kind);
  }
}

void H3dModel::SetModelLayerId(u8 model_layer_id)
{
  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
  GFL_ASSERT(model_layer_id <= 127);  //@check �`�揇�����������B

  m_model_layer_id = model_layer_id;
}
u8 H3dModel::GetModelLayerId(void) const
{
  return m_model_layer_id;
}

s32 H3dModel::GetModelUserDataCount() const
{
  return H3dMetaDataMapUtil::GetMetaDataNum(getModelMetaDataMap());
}
s32 H3dModel::GetModelUserDataIndex(const char* name) const
{
  STATIC_ASSERT(INDEX_NONE==H3dMetaDataMapUtil::INDEX_NONE);
  return H3dMetaDataMapUtil::GetMetaDataIndex(getModelMetaDataMap(), name);
}
void H3dModel::GetModelUserData(const s32 user_data_index, H3dUserData* user_data) const
{
  user_data->SetNwMetaData( &( H3dMetaDataMapUtil::GetMetaData(getModelMetaDataMap(), user_data_index) ) );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool H3dModel::GetModelAABB( gfl::math::AABB *p_dest_aabb ) const 
{
  if ( this->GetModelUserDataCount() == 0 )
    return false;
  
  s32   user_data_index = this->GetModelUserDataIndex("$BBoxMinMax");

  if ( user_data_index < 0 )
    return false;
  
  H3dUserData    user_data;
  this->GetModelUserData( user_data_index, &user_data );

  p_dest_aabb->SetMinMax(gfl::math::VEC4( user_data.GetFloatValue(0), user_data.GetFloatValue(1), user_data.GetFloatValue(2), 0.0f), 
                         gfl::math::VEC4( user_data.GetFloatValue(3), user_data.GetFloatValue(4), user_data.GetFloatValue(5), 0.0f) );

  return true;
}

s32 H3dModel::GetMaterialUserDataCount(const s32 material_index) const
{
  return H3dMetaDataMapUtil::GetMetaDataNum(getMaterialMetaDataMap(material_index));
}
s32 H3dModel::GetMaterialUserDataIndex(const s32 material_index, const char* name) const
{
  STATIC_ASSERT(INDEX_NONE==H3dMetaDataMapUtil::INDEX_NONE);
  return H3dMetaDataMapUtil::GetMetaDataIndex(getMaterialMetaDataMap(material_index), name);
}
void H3dModel::GetMaterialUserData(const s32 material_index, const s32 user_data_index, H3dUserData* user_data) const
{
  user_data->SetNwMetaData( &( H3dMetaDataMapUtil::GetMetaData(getMaterialMetaDataMap(material_index), user_data_index) ) );
}

s32 H3dModel::GetMeshUserDataCount(const s32 mesh_index) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  return H3dMetaDataMapUtil::GetMetaDataNum(getMeshMetaDataMap(mesh_index));
}
s32 H3dModel::GetMeshUserDataIndex(const s32 mesh_index, const char* name) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  STATIC_ASSERT(INDEX_NONE==H3dMetaDataMapUtil::INDEX_NONE);
  return H3dMetaDataMapUtil::GetMetaDataIndex(getMeshMetaDataMap(mesh_index), name);
}
void H3dModel::GetMeshUserData(const s32 mesh_index, const s32 user_data_index, H3dUserData* user_data) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  user_data->SetNwMetaData( &( H3dMetaDataMapUtil::GetMetaData(getMeshMetaDataMap(mesh_index), user_data_index) ) );
}

nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* H3dModel::getModelMetaDataMap(void) const
{
  return m_cont->metaData;
}
nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* H3dModel::getMaterialMetaDataMap(const s32 material_index) const
{
  return m_cont->materials.At(material_index).content->metaData;
}
nw::h3d::fnd::PatriciaMap<nw::h3d::res::MetaData>* H3dModel::getMeshMetaDataMap(const s32 mesh_index) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  return m_cont->meshes.At(mesh_index).metaData;
}

void H3dModel::SetScale(f32 x, f32 y, f32 z)
{
  m_scale_vec = gfl::math::VEC3(x, y, z);
}
void H3dModel::SetScale(const gfl::math::VEC3& t)
{
  m_scale_vec = t;
}
void H3dModel::GetScale(gfl::math::VEC3* t) const
{
  *t = m_scale_vec;
}

void H3dModel::SetRotateXYZ(const gfl::math::VEC3& t)
{
  m_rotate_vec = t;
  SetBitFlagOn(&m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC);
}

void H3dModel::GetRotateXYZ(gfl::math::VEC3* t) const
{
  *t = m_rotate_vec;
}

void H3dModel::SetRotateMatrix(
    f32 m00,
    f32 m01,
    f32 m02,
    f32 m10,
    f32 m11,
    f32 m12,
    f32 m20,
    f32 m21,
    f32 m22
)
{
  m_rotate_mtx._00 = m00;
  m_rotate_mtx._01 = m01;
  m_rotate_mtx._02 = m02;
  m_rotate_mtx._03 = 0;  // @note m_translate_vec���������ɓ���������
  m_rotate_mtx._10 = m10;
  m_rotate_mtx._11 = m11;
  m_rotate_mtx._12 = m12;
  m_rotate_mtx._13 = 0;
  m_rotate_mtx._20 = m20;
  m_rotate_mtx._21 = m21;
  m_rotate_mtx._22 = m22;
  m_rotate_mtx._23 = 0;
  
  SetBitFlagOff(&m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC);
}

void H3dModel::SetTranslate(const gfl::math::VEC3& t)
{
  m_translate_vec = t;
}
void H3dModel::GetTranslate(gfl::math::VEC3* t) const
{
  *t = m_translate_vec;
}

void H3dModel::SetTreeFlag(b32 flag)
{
  SetBitFlag(&m_bit_flag, BIT_FLAG_TREE_FLAG, flag);
}
b32 H3dModel::GetTreeFlag(void) const
{
  return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_TREE_FLAG) != 0 );  // true/false�ŕԂ������̂�!=0���Ă��܂�
}

void H3dModel::SetVisible(b32 visible)
{
  SetBitFlag(&m_bit_flag, BIT_FLAG_VISIBLE, visible);
}
b32  H3dModel::IsVisible(void) const
{
  return ( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_VISIBLE) != 0 );  // true/false�ŕԂ������̂�!=0���Ă��܂�
}

s32 H3dModel::GetMaterialCount(void) const
{
  u32 num = 0;
  if( m_cont )
  {
    num = m_cont->materials.size();
  }
  return num;
}

s32 H3dModel::GetMaterialIndex(const char* name) const
{
  s32 ret = INDEX_NONE;
  u16 idx = m_cont->GetMaterialIdx(name);
  if( idx != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
  {
    ret = idx;
  }
  return ret;
}

const char* H3dModel::GetMaterialName(s32 material_index) const
{
  if( 0<=material_index && material_index<GetMaterialCount() )
  {
    return m_cont->materials[material_index].name;
  }
  else
  {
    GFL_ASSERT_MSG(0, "material_index=%d\n", material_index);
  }
  return NULL;
}

// �}�e���A���A�j���[�V�����ȂǂŕύX�t���O�����������̂��f�t�H���g�̗����Ă��Ȃ���Ԃɖ߂��B
void H3dModel::ResetMaterialState(s32 material_index)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.DisableFlags( static_cast<nw::h3d::MaterialFlag::enum_t>(~(nw::h3d::MaterialState::FLAG_DEFAULT)) );
  materialState.EnableFlags( static_cast<nw::h3d::MaterialFlag::enum_t>(nw::h3d::MaterialState::FLAG_DEFAULT) );
}
void H3dModel::ResetAllMaterialState(void)
{
  for(u32 i=0; i<GetMaterialCount(); ++i)
  {
    ResetMaterialState(i);
  }
}

// �}�e���A���̃e�N�X�`��0,1,2��UV��SRT�̕ύX�t���O�����Z�b�g����
void H3dModel::ResetMaterialStateForTextureCoordinatorUV(s32 material_index)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.DisableFlags(nw::h3d::MaterialFlag::TEXTURE_COORD0_MUTABLE);
  materialState.DisableFlags(nw::h3d::MaterialFlag::TEXTURE_COORD1_MUTABLE);
  materialState.DisableFlags(nw::h3d::MaterialFlag::TEXTURE_COORD2_MUTABLE);
}
void H3dModel::ResetAllMaterialStateForTextureCoordinatorUV(void)
{
  for(u32 i=0; i<GetMaterialCount(); ++i)
  {
    ResetMaterialStateForTextureCoordinatorUV(i);
  }
}

void H3dModel::SetMaterialColorAmbient(s32 material_index, const gfl::grp::ColorF32& c)
{
  gfl::grp::ColorU8 l_c = gfl::grp::ConvertColorF32ToColorU8(c);  // nw::ut::Color��nw::ut::Color8
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetAmbient(l_c);
}
void H3dModel::GetMaterialColorAmbient(s32 material_index, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  const gfl::grp::ColorU8& l_c = materialState.GetAmbient();  // nw::ut::Color��nw::ut::Color8
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}

void H3dModel::SetMaterialColorDiffuse(s32 material_index, const gfl::grp::ColorF32& c)
{
  gfl::grp::ColorU8 l_c = gfl::grp::ConvertColorF32ToColorU8(c);  // nw::ut::Color��nw::ut::Color8
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetDiffuse(l_c);
}
void H3dModel::GetMaterialColorDiffuse(s32 material_index, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  const gfl::grp::ColorU8& l_c = materialState.GetDiffuse();  // nw::ut::Color��nw::ut::Color8
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}

void H3dModel::GetMaterialColorSpecular0(s32 material_index, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  const gfl::grp::ColorU8& l_c = materialState.GetSpecular0();  // nw::ut::Color��nw::ut::Color8
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}

void H3dModel::GetMaterialColorSpecular1(s32 material_index, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  const gfl::grp::ColorU8& l_c = materialState.GetSpecular1();  // nw::ut::Color��nw::ut::Color8
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}

void H3dModel::SetMaterialColorConstant(s32 material_index, s32 no, const gfl::grp::ColorF32& c)
{
  gfl::grp::ColorU8 l_c = gfl::grp::ConvertColorF32ToColorU8(c);  // nw::ut::Color��nw::ut::Color8
  m_mate_constant_color_array[material_index][no] = l_c;
  
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  u32 constant_color_no;
  b32 mutable_flag = false;
  
#if 0
// �y���Ӂznw::h3d::MaterialState::GetCombiner0Constant�ƃf�U�C�i�[����c�[���̈Ⴂ�ɒ��ӁI
// �R���o�C�i�O�p�R���X�^���g�J���[���擾���܂��B�R���X�^���g�J���[�O�ł͂Ȃ��̂Œ��ӂ��Ă��������B
#define SET_CONSTANT_COLOR(step) \
  constant_color_no = materialState.GetCombinerConstantId(step); \
  if(constant_color_no == no) \
  { \
    materialState.SetCombiner##step##Constant(l_c); \
    mutable_flag = true; \
  }

  SET_CONSTANT_COLOR(0)
  SET_CONSTANT_COLOR(1)
  SET_CONSTANT_COLOR(2)
  SET_CONSTANT_COLOR(3)
  SET_CONSTANT_COLOR(4)
  SET_CONSTANT_COLOR(5)

#undef SET_CONSTANT_COLOR
#endif
    
  {// SDK 5.0�Ή�
    materialState.SetConstantColor(no, l_c);
    
    for( s32 step = 0; step < 6; ++ step )
    {
     constant_color_no = this->GetMaterialTextureCombinerConstant(material_index, step);
      if(constant_color_no == no)
      {
        m_mate_ex_state_array[material_index].SetMaterialTextureCombinerConstant(step, constant_color_no, l_c );
      } 
    }
  }
  
  if( mutable_flag )
  {
    materialState.EnableFlags(nw::h3d::MaterialFlag::COMBINER_CONSTANT_MUTABLE);
      // �A�j���[�V�����ŃR���X�^���g�J���[��ύX����Ƃ�nw::h3d�ł��̃t���O�𗧂ĂĂ���B
      // �A�j���[�V�����ŗ��Ă��̂��A����Set�֐��ŗ��Ă��̂��A��������Ȃ��ꍇ�́A
      // ��x�t���O�𗧂Ă��痧�Ă��ςȂ��ɂ��邵���Ȃ��B
  }
}
void H3dModel::GetMaterialColorConstant(s32 material_index, s32 no, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  gfl::grp::ColorU8 l_c = m_mate_constant_color_array[material_index][no];  // nw::ut::Color��nw::ut::Color8
  u32 constant_color_no;
  
  // �A�j���[�V�����ŃR���X�^���g�J���[��ύX���Ă��邩������Ȃ��̂ŁA
  // �Y����m_mate_constant_color_array���ŐV��ԂɍX�V���Ă���l��Ԃ��悤�ɂ���B

// �y���Ӂznw::h3d::MaterialState::GetCombiner0Constant�ƃf�U�C�i�[����c�[���̈Ⴂ�ɒ��ӁI
// �R���o�C�i�O�p�R���X�^���g�J���[���擾���܂��B�R���X�^���g�J���[�O�ł͂Ȃ��̂Œ��ӂ��Ă��������B
#if 0  
#define GET_CONSTANT_COLOR(step) \
  constant_color_no = materialState.GetCombinerConstantId(step); \
  if( constant_color_no == no ) \
  { \
    l_c = materialState.GetCombiner##step##Constant(); \
    m_mate_constant_color_array[material_index][no] = l_c; \
  }

  GET_CONSTANT_COLOR(0)
  GET_CONSTANT_COLOR(1)
  GET_CONSTANT_COLOR(2)
  GET_CONSTANT_COLOR(3)
  GET_CONSTANT_COLOR(4)
  GET_CONSTANT_COLOR(5)

#undef GET_CONSTANT_COLOR
#endif
    
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}

void H3dModel::SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dResTexture& res_texture)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  switch(no)
  {
  // @note NintendoWareForCtr-2_4_1-sdk4_2_1-20120809-patch2-H3d-ja �����łŊm�F
  case 0: materialState.SetTexture0Cmd(res_texture.GetNwTextureContent()->unit0Cmd,
             res_texture.GetNwTextureContent()->format == PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP ); break;
  case 1: materialState.SetTexture1Cmd(res_texture.GetNwTextureContent()->unit1Cmd,
             res_texture.GetNwTextureContent()->format == PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP ); break;
  case 2: materialState.SetTexture2Cmd(res_texture.GetNwTextureContent()->unit2Cmd,
             res_texture.GetNwTextureContent()->format == PICA_DATA_TEXTURE_FORMAT_ETC1_RGB8_NATIVE_DMP ); break;
  //case Proc: break;
  default: GFL_ASSERT_MSG(0, "no=%d\n", no);
  }
 
/*
  GFL_PRINT("unit0Cmd\n");
  H3dCommandUtil::DumpCommandEasy(res_texture.GetNwTextureContent()->unit0Cmd);
  GFL_PRINT("unit1Cmd\n");
  H3dCommandUtil::DumpCommandEasy(res_texture.GetNwTextureContent()->unit1Cmd);
  GFL_PRINT("unit2Cmd\n");
  H3dCommandUtil::DumpCommandEasy(res_texture.GetNwTextureContent()->unit2Cmd);
*/

  m_texture_info_array[material_index].SetTextureClassTypeAndPtr(no, TEXTURE_CLASS_TYPE_RES, &res_texture);
}

void H3dModel::SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dDynamicTexture& dynamic_texture)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  switch(no)
  {
  case 0: materialState.SetTexture0Cmd(dynamic_texture.GetNwCommand(0), dynamic_texture.GetFormat() == TEXTURE_FORMAT_ETC1 ); break;
  case 1: materialState.SetTexture1Cmd(dynamic_texture.GetNwCommand(1), dynamic_texture.GetFormat() == TEXTURE_FORMAT_ETC1 ); break;
  case 2: materialState.SetTexture2Cmd(dynamic_texture.GetNwCommand(2), dynamic_texture.GetFormat() == TEXTURE_FORMAT_ETC1 ); break;
  //case Proc: break;
  default: GFL_ASSERT_MSG(0, "no=%d\n", no);
  }
  
  m_texture_info_array[material_index].SetTextureClassTypeAndPtr(no, TEXTURE_CLASS_TYPE_DYNAMIC, &dynamic_texture);
}

void H3dModel::SetMaterialTextureMapperTexture(s32 material_index, s32 no, const H3dModel& src_model, s32 src_material_index, s32 src_no)
{
  s32 src_type = src_model.getMaterialTextureClassType(src_material_index, src_no);

  switch(src_type)
  {
  case TEXTURE_CLASS_TYPE_MODEL:
    {
      const nw::h3d::res::TextureContent* src_tex_cont = src_model.getMaterialTextureMapperTextureAsModelTexture(src_material_index, src_no);
      
      if ( src_tex_cont )
      {
        nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
        switch(no)
        {
        case 0:
          materialState.SetTexture0Cmd(src_tex_cont->unit0Cmd, src_tex_cont->format == TEXTURE_FORMAT_ETC1 );
          break;
        case 1:
          materialState.SetTexture1Cmd(src_tex_cont->unit1Cmd, src_tex_cont->format == TEXTURE_FORMAT_ETC1 );
          break;
        case 2:
          materialState.SetTexture2Cmd(src_tex_cont->unit2Cmd, src_tex_cont->format == TEXTURE_FORMAT_ETC1 );
          break;
        default:
          GFL_ASSERT_MSG(0, "no=%d\n", no);
        }

        m_texture_info_array[material_index].SetTextureClassTypeAndPtr(no, TEXTURE_CLASS_TYPE_MODEL, src_tex_cont);
      }
      else
      {
        GFL_ASSERT_MSG( 0, "�\�[�X���f���Ƀe�N�X�`������������܂���B\n" );
      }
      break;
    }
  case TEXTURE_CLASS_TYPE_RES:
    {
      const H3dResTexture* src_res_tex = src_model.getMaterialTextureMapperTextureAsResTexture(src_material_index, src_no);
      if( src_res_tex )
        SetMaterialTextureMapperTexture(material_index, no, *src_res_tex);
      else
        GFL_ASSERT_MSG( 0, "�\�[�X���f���Ƀe�N�X�`������������܂���B\n" );
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    {
      const H3dDynamicTexture* src_dynamic_tex = src_model.getMaterialTextureMapperTextureAsDynamicTexture(src_material_index, src_no);
      if( src_dynamic_tex)
        SetMaterialTextureMapperTexture(material_index, no, *src_dynamic_tex);
      else
        GFL_ASSERT_MSG( 0, "�\�[�X���f���Ƀe�N�X�`������������܂���B\n" );
      break;
    }
  }
}

void H3dModel::ResetMaterialTextureMapperTexture(s32 material_index, s32 no)
{
  // @note VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă���ƌ��ɖ߂��Ȃ���������܂��񂪁A�m�F�������Ƃ͂���܂���B
  
  nw::h3d::fnd::Command cmd;  // ��R�}���h
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  switch(no)
  {
  case 0: materialState.SetTexture0Cmd(cmd, false); break;
  case 1: materialState.SetTexture1Cmd(cmd, false); break;
  case 2: materialState.SetTexture2Cmd(cmd, false); break;
  //case Proc: break;
  default: GFL_ASSERT_MSG(0, "no=%d\n", no);
  }

  initializeModelTextureInfo(material_index, no);
}

b32 H3dModel::IsMaterialTextureMapperTextureValid(s32 material_index, s32 no) const
{
  return ( m_texture_info_array[material_index].GetTextureClassType(no) != TEXTURE_CLASS_TYPE_NONE );
}
void* H3dModel::GetMaterialTextureMapperTexturePhysicalAddress(s32 material_index, s32 no) const
{
  void* addr = NULL;
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  switch(type)
  {
  case TEXTURE_CLASS_TYPE_MODEL  :
    {
      addr = getModelTexturePhysicalAddress(material_index, no);
      break;
    }
  case TEXTURE_CLASS_TYPE_RES    :
    {
      const H3dResTexture*     tex = m_texture_info_array[material_index].GetPtrAsRes(no);
      addr = tex->GetPhysicalAddress();
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    {
      const H3dDynamicTexture* tex = m_texture_info_array[material_index].GetPtrAsDynamic(no);
      addr = tex->GetPhysicalAddress();
      break;
    }
  }
  return addr;
}
u16   H3dModel::GetMaterialTextureMapperTextureWidth(s32 material_index, s32 no) const
{
  u16 width = 0;
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  switch(type)
  {
  case TEXTURE_CLASS_TYPE_MODEL  :
    {
      width = getModelTextureWidth(material_index, no);
      break;
    }
  case TEXTURE_CLASS_TYPE_RES    :
    {
      const H3dResTexture*     tex = m_texture_info_array[material_index].GetPtrAsRes(no);
      width = tex->GetWidth();
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    { 
      const H3dDynamicTexture* tex = m_texture_info_array[material_index].GetPtrAsDynamic(no);
      width = tex->GetWidth();
      break;
    }
  }
  return width;
}
u16   H3dModel::GetMaterialTextureMapperTextureHeight(s32 material_index, s32 no) const
{
  u16 height = 0;
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  switch(type)
  {
  case TEXTURE_CLASS_TYPE_MODEL  :
    {
      height = getModelTextureHeight(material_index, no);
      break;
    }
  case TEXTURE_CLASS_TYPE_RES    :
    {
      const H3dResTexture*     tex = m_texture_info_array[material_index].GetPtrAsRes(no);
      height = tex->GetHeight();
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    {
      const H3dDynamicTexture* tex = m_texture_info_array[material_index].GetPtrAsDynamic(no);
      height = tex->GetHeight();
      break;
    }
  }
  return height;
}
gfl::grp::g3d::TextureFormat H3dModel::GeMaterialTextureMapperTexturetForamat(s32 material_index, s32 no) const
{
  gfl::grp::g3d::TextureFormat format = gfl::grp::g3d::TEXTURE_FORMAT_RGBA8;
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  switch(type)
  {
  case TEXTURE_CLASS_TYPE_MODEL  :
    {
      format = getModelTextureFormat(material_index, no);
      break;
    }
  case TEXTURE_CLASS_TYPE_RES    :
    {
      const H3dResTexture*     tex = m_texture_info_array[material_index].GetPtrAsRes(no);
      format = tex->GetFormat();
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    {
      const H3dDynamicTexture* tex = m_texture_info_array[material_index].GetPtrAsDynamic(no);
      format = tex->GetFormat();
      break;
    }
  }
  return format;
}
u8    H3dModel::GetMaterialTextureMapperTextureMipmap(s32 material_index, s32 no) const
{
  u8 mipmap = 1;
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  switch(type)
  {
  case TEXTURE_CLASS_TYPE_MODEL  :
    {
      mipmap = getModelTextureMipmap(material_index, no);
      break;
    }
  case TEXTURE_CLASS_TYPE_RES    :
    {
      const H3dResTexture*     tex = m_texture_info_array[material_index].GetPtrAsRes(no);
      mipmap = tex->GetMipmap();
      break;
    }
  case TEXTURE_CLASS_TYPE_DYNAMIC:
    {
      const H3dDynamicTexture* tex = m_texture_info_array[material_index].GetPtrAsDynamic(no);
      mipmap = tex->GetMipmap();
      break;
    }
  }
  return mipmap;
}

const char* H3dModel::GetMaterialTextureMapperTextureOriginalName(s32 material_index, s32 no) const
{
  /*
  // ����͍��\���Ă���e�N�X�`���̖��O�B
  const nw::h3d::res::MaterialData& material_data = m_cont->materials[material_index];
  const nw::h3d::res::TextureContent* tex_cont[H3dTextureSamplerExConst::TEXTURE_NUM] = 
  {
    material_data.texture0,
    material_data.texture1,
    material_data.texture2,
  };
  return tex_cont[no]->name;
  */


//NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
//Result
//ModelContent::Link(const char* name, TextureContent* textureContent, u32 srcCmdAddress, u32 cmdAddress)

  const nw::h3d::res::MaterialData& material_data = m_cont->materials[material_index];
  return material_data.textureNames[no];
}
b32 H3dModel::GetMaterialTextureMapperTextureOriginalIndex(s32* material_index, s32* no, const char* name) const
{
  b32 exist = false;
  s32 num = GetMaterialCount();
  for(s32 i=0; i<num; ++i)
  {
    for(s32 j=0; j<H3dTextureSamplerExConst::TEXTURE_NUM; ++j)
    {
      const char* mate_tex_name = GetMaterialTextureMapperTextureOriginalName(i, j);
      if( mate_tex_name )
      {
        if( strcmp( mate_tex_name, name ) == 0 )
        {
          *material_index = i;
          *no             = j;
          exist = true;
          break;
        }
      }
    }
    if( exist ) break;
  }
  return exist;
}

void H3dModel::initializeModelTextureInfo(s32 material_index, s32 no)
{
  const nw::h3d::res::MaterialData& material_data = m_cont->materials[material_index];
  const nw::h3d::res::TextureContent* tex_cont[H3dTextureSamplerExConst::TEXTURE_NUM] = 
  {
    material_data.texture0,
    material_data.texture1,
    material_data.texture2,
  };
  m_texture_info_array[material_index].SetTextureClassTypeAndPtr(no, (tex_cont[no])?TEXTURE_CLASS_TYPE_MODEL:TEXTURE_CLASS_TYPE_NONE, tex_cont[no]);

  // VRAM�ɔz�u����Delete����Section(DeleteSomeSection�Ȃ�)���Ă����
  // nw::h3d::res::TextureContent��unit0Cmd(1,2��)����͉����擾�ł��Ȃ��B
  // nw::h3d::MaterialState��GetTexture0Cmd(1,2��)������擾�ł��Ȃ������B

}

void* H3dModel::getModelTexturePhysicalAddress(s32 material_index, s32 no) const
{
  const nw::h3d::res::TextureContent* tex = m_texture_info_array[material_index].GetPtrAsModel(no);
  return H3dTextureCommandUtil::GetPhysicalAddress(tex->unit0Cmd);  // 0��1��2�������͂�
}
u16   H3dModel::getModelTextureWidth(s32 material_index, s32 no) const
{
  const nw::h3d::res::TextureContent* tex = m_texture_info_array[material_index].GetPtrAsModel(no);
  return H3dTextureCommandUtil::GetWidth(tex->unit0Cmd);  // 0��1��2�������͂�
}
u16   H3dModel::getModelTextureHeight(s32 material_index, s32 no) const
{
  const nw::h3d::res::TextureContent* tex = m_texture_info_array[material_index].GetPtrAsModel(no);
  return H3dTextureCommandUtil::GetHeight(tex->unit0Cmd);  // 0��1��2�������͂�
}
gfl::grp::g3d::TextureFormat H3dModel::getModelTextureFormat(s32 material_index, s32 no) const
{
  const nw::h3d::res::TextureContent* tex = m_texture_info_array[material_index].GetPtrAsModel(no);
  return static_cast<gfl::grp::g3d::TextureFormat>(tex->format);
}
u8    H3dModel::getModelTextureMipmap(s32 material_index, s32 no) const
{
  const nw::h3d::res::TextureContent* tex = m_texture_info_array[material_index].GetPtrAsModel(no);
  return tex->mipmapSize;
}

s32 H3dModel::getMaterialTextureClassType(s32 material_index, s32 no) const
{
  return m_texture_info_array[material_index].GetTextureClassType(no);
}

const nw::h3d::res::TextureContent* H3dModel::getMaterialTextureMapperTextureAsModelTexture(s32 material_index, s32 no) const
{
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  if( type == TEXTURE_CLASS_TYPE_MODEL )
  {
    return m_texture_info_array[material_index].GetPtrAsModel(no);
  }
  return NULL;
}
const H3dResTexture*                H3dModel::getMaterialTextureMapperTextureAsResTexture(s32 material_index, s32 no) const
{
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  if( type == TEXTURE_CLASS_TYPE_RES )
  {
    return m_texture_info_array[material_index].GetPtrAsRes(no);
  }
  return NULL;
}
const H3dDynamicTexture*            H3dModel::getMaterialTextureMapperTextureAsDynamicTexture(s32 material_index, s32 no) const
{
  s32 type = m_texture_info_array[material_index].GetTextureClassType(no);
  if( type == TEXTURE_CLASS_TYPE_DYNAMIC )
  {
    return m_texture_info_array[material_index].GetPtrAsDynamic(no);
  }
  return NULL;
}


void H3dModel::SetMaterialTextureMapperWrapU(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
  m_mate_ex_state_array[material_index].SetTextureSamplerWrapU(no, wrap);
}
H3dTextureSamplerExConst::Wrap  H3dModel::GetMaterialTextureMapperWrapU(s32 material_index, s32 no) const
{
  return m_mate_ex_state_array[material_index].GetTextureSamplerWrapU(no);
}
void H3dModel::SetMaterialTextureMapperWrapV(s32 material_index, s32 no, H3dTextureSamplerExConst::Wrap wrap)
{
  m_mate_ex_state_array[material_index].SetTextureSamplerWrapV(no, wrap);
}
H3dTextureSamplerExConst::Wrap  H3dModel::GetMaterialTextureMapperWrapV(s32 material_index, s32 no) const
{
  return m_mate_ex_state_array[material_index].GetTextureSamplerWrapV(no);
}

void H3dModel::SetMaterialTextureMapperSamplerType(s32 material_index, s32 no, H3dTextureSamplerExConst::SamplerType sampler_type)
{
  m_mate_ex_state_array[material_index].SetTextureSamplerType(no, sampler_type);
}
H3dTextureSamplerExConst::SamplerType H3dModel::GetMaterialTextureMapperSamplerType(s32 material_index, s32 no) const
{
  return m_mate_ex_state_array[material_index].GetTextureSamplerType(no);
}
//res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

void H3dModel::SetMaterialTextureMapperMagFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MagFilter mag_filter)
{
  m_mate_ex_state_array[material_index].SetTextureSamplerMagFilter(no, mag_filter);
}
H3dTextureSamplerExConst::MagFilter H3dModel::GetMaterialTextureMapperMagFilter(s32 material_index, s32 no) const
{
  return m_mate_ex_state_array[material_index].GetTextureSamplerMagFilter(no);
}

void H3dModel::SetMaterialTextureMapperMinFilter(s32 material_index, s32 no, H3dTextureSamplerExConst::MinFilter min_filter)
{
  m_mate_ex_state_array[material_index].SetTextureSamplerMinFilter(no, min_filter);
}
H3dTextureSamplerExConst::MinFilter H3dModel::GetMaterialTextureMapperMinFilter(s32 material_index, s32 no) const
{
  return m_mate_ex_state_array[material_index].GetTextureSamplerMinFilter(no);
}

// �~�b�v�}�b�v�ݒ�
void H3dModel::SetTextureMipMapLodMinLevel( s32 material_index, s32 no, s32 level )
{
  m_mate_ex_state_array[material_index].SetTextureMipMapLodMinLevel( no, level );
}

void H3dModel::SetTextureMipMapLodBias( s32 material_index, s32 no, s32 bias )
{
  m_mate_ex_state_array[material_index].SetTextureMipMapLodBias( no, bias );
}



void H3dModel::SetMaterialTextureMapperBorderColor(s32 material_index, s32 no, const gfl::grp::ColorF32& c)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  gfl::grp::ColorU8 l_c = gfl::grp::ConvertColorF32ToColorU8(c);  // nw::ut::Color��nw::ut::Color8
  materialState.SetBorderColor(no, c);
}
void H3dModel::GetMaterialTextureMapperBorderColor(s32 material_index, s32 no, gfl::grp::ColorF32* c) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  gfl::grp::ColorU8 l_c = materialState.GetBorderColor(no);  // nw::ut::Color��nw::ut::Color8
  *c = gfl::grp::ConvertColorU8ToColorF32(l_c);
}


void H3dModel::enableMaterialTextureCoordinatorFlag(s32 material_index, s32 no)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  static const u32 TEXTURE_COORD_MUTABLE_FLAG[] =
  {
    nw::h3d::MaterialFlag::TEXTURE_COORD0_MUTABLE,
    nw::h3d::MaterialFlag::TEXTURE_COORD1_MUTABLE,
    nw::h3d::MaterialFlag::TEXTURE_COORD2_MUTABLE,
  };
  materialState.EnableFlags( static_cast<nw::h3d::MaterialFlag::enum_t>(TEXTURE_COORD_MUTABLE_FLAG[no]) );
}

void H3dModel::SetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetTextureScale(no, t);
  enableMaterialTextureCoordinatorFlag(material_index, no);
}
void H3dModel::GetMaterialTextureCoordinatorScaleUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  *t = materialState.GetTextureScale(no);
}
  
void H3dModel::SetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no, const f32 t)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetTextureRotate(no, t);
  enableMaterialTextureCoordinatorFlag(material_index, no);
}
f32 H3dModel::GetMaterialTextureCoordinatorRotateUV(s32 material_index, s32 no) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  return materialState.GetTextureRotate(no);
}

void H3dModel::SetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, const gfl::math::VEC2& t)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetTextureTranslate(no, t);
  enableMaterialTextureCoordinatorFlag(material_index, no);
}
void H3dModel::GetMaterialTextureCoordinatorTranslateUV(s32 material_index, s32 no, gfl::math::VEC2* t) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  *t = materialState.GetTextureTranslate(no);
}

void H3dModel::SetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no, s32 scene_camera_index)
{
  nw::h3d::fnd::TextureCoord& textureCoord = getMaterialTextureCoordinator(material_index, no);
  textureCoord.referenceCameraIdx = scene_camera_index;
  textureCoord.flags |= nw::h3d::TextureCoordFlag::DIRTY;
}
s32  H3dModel::GetMaterialTextureCoordinatorReferenceSceneCameraIndex(s32 material_index, s32 no) const
{
  const nw::h3d::fnd::TextureCoord& textureCoord = getMaterialTextureCoordinator(material_index, no);
  return textureCoord.referenceCameraIdx;
}

void H3dModel::SetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no, TextureMappingMethod mapping_method)
{
#if 0  
  switch(mapping_method){
  case TEXTURE_MAPPING_METHOD_PROJECTION:// ���e���܂��B 
  case TEXTURE_MAPPING_METHOD_SHADOW:// �V���h�E�p�̓��e�ł��B 
  case TEXTURE_MAPPING_METHOD_SHADOW_CUBE:// �V���h�E�L���[�u�p�̓��e�ł��B
    //GFL_ASSERT_MSG( 0, "���얢�m�F" );
    return;
  }
#endif
  
  {//���_�V�F�[�_�[�ݒ�
    nw::h3d::res::MaterialContent *pMaterialContent = m_cont->materials[material_index].content;

    for (nw::h3d::DrawCommandIterator dci(pMaterialContent->paramCmd); dci.HasNext(); dci.Next())
    {
      if (dci.GetAddress() == PICA_REG_VS_FLOAT_ADDR) // ���������_���j�t�H�[�����ǂ���
      {
        if ((dci.GetData() & 0xff) == 10) // ���W�X�^�ԍ��� 10 �Ԃ��ǂ���
        {
          float             *data = reinterpret_cast<float*>(dci.GetBurstData());
          if ( data )
          {//KW11�C��
            static const s32  DataOffSet[] = { 3, 2, 1 };
            s32               offSet = DataOffSet[ no ];

            switch(mapping_method){
            case TEXTURE_MAPPING_METHOD_UV_COORDINATE:// �e�N�X�`�����W�ł��B 
              data[DataOffSet[no]] = no;// ���W�X�^ c10 �̃e�N�X�`�����j�b�g�p�̃}�b�s���O�̎��
              break;
            case TEXTURE_MAPPING_METHOD_CAMERA_CUBE_ENV:// �J�����L���[�u���W�ł��B 
              data[DataOffSet[no]] = 3.0f;// ���W�X�^ c10 �̃e�N�X�`�����j�b�g�p�̃}�b�s���O�̎��
              break;
            case TEXTURE_MAPPING_METHOD_CAMERA_SPHERE_ENV:// �J�����X�t�B�A���W�ł��B 
              data[DataOffSet[no]] = 4.0f;// ���W�X�^ c10 �̃e�N�X�`�����j�b�g�p�̃}�b�s���O�̎��
              break;
            }
          }
        }
      }
    }

    {
      const nw::h3d::fnd::DataArray<nw::h3d::res::MeshData> &rMeshes = m_cont->meshes;

      for( s32 i = 0; i < rMeshes.size(); ++i )
      {
        if ( rMeshes[i].materialId != material_index )
          continue;

        const nw::h3d::fnd::DataArray< nw::h3d::res::SubMeshData > rSubMeshes = rMeshes[i].subMeshes;

        for( s32 i2 = 0; i2 < rSubMeshes.size(); ++i2 )
        {
          nw::h3d::res::SubMeshData &subMesh = const_cast<nw::h3d::res::SubMeshData&>( rSubMeshes[i2] ); // �T�u���b�V���f�[�^�ł��B

          for (nw::h3d::DrawCommandIterator dci(subMesh.drawCmd); dci.HasNext(); dci.Next())
          {
            if (dci.GetAddress() == PICA_REG_VS_BOOL)
            {
              static const s32  DataOffSet[] = { 9, 10, 11 };

              if ( mapping_method != TEXTURE_MAPPING_METHOD_UV_COORDINATE )
                *dci.GetDataPtr() &= ~(1 << DataOffSet[no]); // �e�N�X�`�����j�b�g��UV���W�w��𖳌���
              else
                *dci.GetDataPtr() |= (1 << DataOffSet[no]); // �e�N�X�`�����j�b�g��UV���W�w���L����
            }
          }
        }
      }
    }
  }
  
  {//�}�e���A���ݒ�
    nw::h3d::MaterialState        &materialState = m_mate_state_array[material_index];

    nw::h3d::fnd::TextureCoord    textureCoord = materialState.GetTextureCoord(no);

    textureCoord.mappingType = mapping_method;
    textureCoord.flags |= nw::h3d::TextureCoordFlag::DIRTY;  

    materialState.SetTextureCoord( no, textureCoord );
  }
}

void H3dModel::SetVSBool(s32 material_index, s32 boolIndex, b32 value )
{
  const nw::h3d::fnd::DataArray<nw::h3d::res::MeshData> &rMeshes = m_cont->meshes;

  for( s32 i = 0; i < rMeshes.size(); ++i )
  {
    if ( rMeshes[i].materialId != material_index )
      continue;

    const nw::h3d::fnd::DataArray< nw::h3d::res::SubMeshData > rSubMeshes = rMeshes[i].subMeshes;

    for( s32 i2 = 0; i2 < rSubMeshes.size(); ++i2 )
    {
      nw::h3d::res::SubMeshData &subMesh = const_cast<nw::h3d::res::SubMeshData&>( rSubMeshes[i2] ); // �T�u���b�V���f�[�^�ł��B

      for (nw::h3d::DrawCommandIterator dci(subMesh.drawCmd); dci.HasNext(); dci.Next())
      {
        if (dci.GetAddress() == PICA_REG_VS_BOOL)
        {
          if ( value )
            *dci.GetDataPtr() |= (1 << boolIndex);
          else
            *dci.GetDataPtr() &= ~(1 << boolIndex);
        }
      }
    }
  }
}

H3dModel::TextureMappingMethod H3dModel::GetMaterialTextureCoordinatorMappingMethod(s32 material_index, s32 no) const
{
  const nw::h3d::fnd::TextureCoord& textureCoord = getMaterialTextureCoordinator(material_index, no);
  return static_cast<TextureMappingMethod>(textureCoord.mappingType);
}

nw::h3d::fnd::TextureCoord& H3dModel::getMaterialTextureCoordinator(s32 material_index, s32 no)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  nw::h3d::TextureMapState& textureMapping = materialState.GetTextureMapping();  // nw::h3d::TextureMapState���o�R���Ȃ���const�ł����Ȃ�
  nw::h3d::fnd::TextureCoord& textureCoord = textureMapping.GetTextureCoord(no);
  return textureCoord;
}

const nw::h3d::fnd::TextureCoord& H3dModel::getMaterialTextureCoordinator(s32 material_index, s32 no) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  //const nw::h3d::TextureMapState& textureMapping = materialState.GetTextureMapping();  // nw::h3d::TextureMapState���o�R�����const�Ȃ��ł����Ȃ�
  const nw::h3d::fnd::TextureCoord& textureCoord = materialState.GetTextureCoord(no);
  return textureCoord;
}



void H3dModel::SetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerCombineRgb(step, combine);
}
H3dTextureCombinerExConst::Combine H3dModel::GetMaterialTextureCombinerCombineRgb(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerCombineRgb(step);
}

void H3dModel::SetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Combine combine)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerCombineAlpha(step, combine);
}
H3dTextureCombinerExConst::Combine H3dModel::GetMaterialTextureCombinerCombineAlpha(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerCombineAlpha(step);
}

void H3dModel::SetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerScaleRgb(step, scale);
}
H3dTextureCombinerExConst::Scale H3dModel::GetMaterialTextureCombinerScaleRgb(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerScaleRgb(step);
}

void H3dModel::SetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step, H3dTextureCombinerExConst::Scale scale)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerScaleAlpha(step, scale);
}
H3dTextureCombinerExConst::Scale H3dModel::GetMaterialTextureCombinerScaleAlpha(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerScaleAlpha(step);
}

void H3dModel::SetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandRgb ope)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerOperandRgb(step, no, ope);
}
H3dTextureCombinerExConst::OperandRgb H3dModel::GetMaterialTextureCombinerOperandRgb(s32 material_index, s32 step, s32 no)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerOperandRgb(step, no);
}

void H3dModel::SetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::OperandAlpha ope)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerOperandAlpha(step, no, ope);
}
H3dTextureCombinerExConst::OperandAlpha H3dModel::GetMaterialTextureCombinerOperandAlpha(s32 material_index, s32 step, s32 no)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerOperandAlpha(step, no);
}

void H3dModel::SetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerSourceRgb(step, no, source);
}
H3dTextureCombinerExConst::Source H3dModel::GetMaterialTextureCombinerSourceRgb(s32 material_index, s32 step, s32 no)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerSourceRgb(step, no);
}

void H3dModel::SetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no, H3dTextureCombinerExConst::Source source)
{
  m_mate_ex_state_array[material_index].SetTextureCombinerSourceAlpha(step, no, source);
}
H3dTextureCombinerExConst::Source H3dModel::GetMaterialTextureCombinerSourceAlpha(s32 material_index, s32 step, s32 no)
{
  return m_mate_ex_state_array[material_index].GetTextureCombinerSourceAlpha(step, no);
}

void H3dModel::SetMaterialTextureCombinerConstant(s32 material_index, s32 step, s32 no)
{
  // @note SDK5.0.0�Ή�
  
  
  gfl::grp::ColorU8     l_c = m_mate_constant_color_array[material_index][no];
  m_mate_ex_state_array[material_index].SetMaterialTextureCombinerConstant(step, no, l_c );
  
  SetMaterialColorConstant( material_index, no, l_c );
  
  //m_mate_constant_color_array[material_index][no]
#if 0  
  // NintendoWare/CTR/include/nw/h3d/h3d_MaterialState.h
  // u32 GetCombinerConstantId(int idx) const { NW_MINMAXLT_ASSERT(idx, 0, Constants::COMBINER_QUANTITY); return (m_CombinerConstantIds >> (idx * 4)) & 0xf; } //!< �C�ӂ̃R���o�C�i�̃R���X�^���g�J���[�̔ԍ����擾���܂��B

  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  // @note SDK5.0.0�Ή�
  u32 ids = 0;//materialState.GetCombinerConstantId(step);
  ids &= ( ~( 0xf << (step * 4) ) );  // @note 4�r�b�g���������S�z�B��L�̔C�V�����܂̃\�[�X���ύX�ɂȂ��Ă��Ȃ����A����note�ɋC�t�����l�̓`�F�b�N���Ă݂āB���`�F�b�N�����B
  ids |= ( no << (step * 4) );
  materialState.SetCombinerConstantIds(ids);

			#define SET_CONSTANT_COLOR(number) \
			  if(number==step) materialState.SetCombiner##number##Constant(m_mate_constant_color_array[material_index][no]);

			  SET_CONSTANT_COLOR(0)
			  SET_CONSTANT_COLOR(1)
			  SET_CONSTANT_COLOR(2)
			  SET_CONSTANT_COLOR(3)
			  SET_CONSTANT_COLOR(4)
			  SET_CONSTANT_COLOR(5)

			#undef SET_CONSTANT_COLOR
	}
#endif
}
s32 H3dModel::GetMaterialTextureCombinerConstant(s32 material_index, s32 step) const
{
  s32   no = m_mate_ex_state_array[material_index].GetMaterialTextureCombinerConstant(step);
  
  if ( no == -1 )
  {
    const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
    
    bit32 assignment = materialState.GetCombinerConstantAssignment();
    return nw::h3d::fnd::Util::GetCombinerConstantSource(step,assignment);
  }
  return no;
  
#if 0  
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  return materialState.GetCombinerConstantId(step);
#endif
}


void H3dModel::SetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  m_mate_ex_state_array[material_index].SetFragmentShaderBufferInputRgb(step, input);
}
H3dFragmentShaderExConst::BufferInput H3dModel::GetMaterialFragmentShaderBufferInputRgb(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetFragmentShaderBufferInputRgb(step);
}

void H3dModel::SetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step, H3dFragmentShaderExConst::BufferInput input)
{
  m_mate_ex_state_array[material_index].SetFragmentShaderBufferInputAlpha(step, input);
}
H3dFragmentShaderExConst::BufferInput H3dModel::GetMaterialFragmentShaderBufferInputAlpha(s32 material_index, s32 step)
{
  return m_mate_ex_state_array[material_index].GetFragmentShaderBufferInputAlpha(step);
}

void H3dModel::SetMaterialShader(s32 material_index, const H3dResShader& res_shader)
{
  m_shader_cont_array[material_index] = res_shader.GetNwShaderContent();
  setShaderBind(material_index, m_shader_cont_array[material_index]);
}
void H3dModel::ResetMaterialShader(s32 material_index)
{
  const nw::h3d::res::MaterialData& material = m_cont->materials[material_index];
  m_shader_cont_array[material_index] = material.content->shaderContent;
  setShaderBind(material_index, m_shader_cont_array[material_index]);
}



// StencilOperation
b32 H3dModel::IsMaterialStencilTestEnable(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].IsStencilOperationTestEnable();
}
void H3dModel::SetMaterialStencilTestEnable(s32 material_index, b32 enable)
{
  m_mate_ex_state_array[material_index].SetStencilOperationTestEnable(enable);
}
H3dStencilOperationExConst::TestFunc H3dModel::GetMaterialStencilTestFunction(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetStencilOperationTestFunc();
}
void H3dModel::SetMaterialStencilTestFunction(s32 material_index, H3dStencilOperationExConst::TestFunc func_func)
{
  m_mate_ex_state_array[material_index].SetStencilOperationTestFunc(func_func);
}

void H3dModel::SetMaterialStencilTestReference(s32 material_index, u8 ref)
{
  m_mate_ex_state_array[material_index].SetStencilOperationTestFuncRef(ref);
}
u8 H3dModel::GetMaterialStencilTestReference(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetStencilOperationTestFuncRef();
}

// @note GetMaterialStencilTestMask��SetMaterialStencilTestMask�́AMask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��ł��B
//       �I�[�T�����O�c�[���̃X�e���V���̃}�X�N�͎g���Ă��Ȃ��炵���̂ŁA�ǂ��炪����ɊY������̂����؂��Ă��܂���B
//       ��ID�G�b�W�Ō��؂����̂Ŏg���đ��v�ł��B
u8 H3dModel::GetMaterialStencilTestMask(s32 material_index) const  // @note Mask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��H
{
  return m_mate_ex_state_array[material_index].GetStencilOperationTestMask();
}
void H3dModel::SetMaterialStencilTestMask(s32 material_index, u8 mask)  // @note Mask��FuncMask�ԈႦ�Ă��Ȃ������M�Ȃ��H
{
  m_mate_ex_state_array[material_index].SetStencilOperationTestMask(mask);
}

H3dStencilOperationExConst::StencilOp H3dModel::GetMaterialStencilTestFailOperation(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetStencilOperationOpFail();
}
void H3dModel::SetMaterialStencilTestFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_fail)
{
  m_mate_ex_state_array[material_index].SetStencilOperationOpFail(op_fail);
}
H3dStencilOperationExConst::StencilOp H3dModel::GetMaterialStencilTestZFailOperation(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetStencilOperationOpZFail();
}
void H3dModel::SetMaterialStencilTestZFailOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zfail)
{
  m_mate_ex_state_array[material_index].SetStencilOperationOpZFail(op_zfail);
}
H3dStencilOperationExConst::StencilOp H3dModel::GetMaterialStencilTestPassOperation(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetStencilOperationOpZPass();
}
void H3dModel::SetMaterialStencilTestPassOperation(s32 material_index, H3dStencilOperationExConst::StencilOp op_zpass)
{
  m_mate_ex_state_array[material_index].SetStencilOperationOpZPass(op_zpass);
}



// AlphaTest
b32 H3dModel::IsMaterialAlphaTestEnable(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].IsAlphaTestEnable();
}
void H3dModel::SetMaterialAlphaTestEnable(s32 material_index, b32 enable)  // true�̂Ƃ��A���t�@�e�X�g�L��
{
  m_mate_ex_state_array[material_index].SetAlphaTestEnable(enable);
}
H3dAlphaTestExConst::TestFunc H3dModel::GetMaterialAlphaTestFunction(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetAlphaTestFunc();
}
void H3dModel::SetMaterialAlphaTestFunction(s32 material_index, H3dAlphaTestExConst::TestFunc func)
{
  m_mate_ex_state_array[material_index].SetAlphaTestFunc(func);
}

void H3dModel::SetMaterialAlphaTestReference(s32 material_index, f32 ref)
{
  m_mate_ex_state_array[material_index].SetAlphaTestRef( static_cast<u8>( ref * 255.0f ) );
}
f32 H3dModel::GetMaterialAlphaTestReference(s32 material_index) const
{
  return ( static_cast<f32>( m_mate_ex_state_array[material_index].GetAlphaTestRef() ) / 255.0f );
}

void H3dModel::SetMaterialAlphaTestReferenceU8(s32 material_index, u8 ref)
{
  m_mate_ex_state_array[material_index].SetAlphaTestRef(ref);
}
u8 H3dModel::GetMaterialAlphaTestReferenceU8(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetAlphaTestRef();
}



// DepthTest
b32 H3dModel::IsMaterialDepthTestEnable(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].IsDepthTestEnable();
}
void H3dModel::SetMaterialDepthTestEnable(s32 material_index, b32 enable)  // true�̂Ƃ��f�v�X�e�X�g�L��
{
  m_mate_ex_state_array[material_index].SetDepthTestEnable(enable);
}
b32 H3dModel::IsMaterialDepthTestMaskEnable(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].IsDepthTestMaskEnable();
}
void H3dModel::SetMaterialDepthTestMaskEnable(s32 material_index, b32 enable)  // true�̂Ƃ��f�v�X�}�X�N�L��(true�̂Ƃ��f�v�X�o�b�t�@���X�V����)
{
  m_mate_ex_state_array[material_index].SetDepthTestMaskEnable(enable);
}
H3dDepthTestExConst::TestFunc H3dModel::GetMaterialDepthTestFunction(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetDepthTestFunc();
}
void H3dModel::SetMaterialDepthTestFunction(s32 material_index, H3dDepthTestExConst::TestFunc func)
{
  m_mate_ex_state_array[material_index].SetDepthTestFunc(func);
}



// �u�����_�[

// @attention �y���Ӂz�u�����h���[�h�ɂ���Đݒ�ł����肵�Ȃ������肷��悤�ɐ��䂵�Ă���̂ŁA�܂��ŏ��Ƀu�����h���[�h��ύX���ĉ������B
void H3dModel::SetMaterialBlendMode(s32 material_index, H3dBlendExConst::Mode mode)
{
  m_mate_ex_state_array[material_index].SetBlendMode(mode);
}
H3dBlendExConst::Mode H3dModel::GetMaterialBlendMode(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendMode();
}

H3dBlendExConst::LogicOperation H3dModel::GetMaterialBlendLogicOperation(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendLogicOperation();
}
void H3dModel::SetMaterialBlendLogicOperation(s32 material_index, H3dBlendExConst::LogicOperation operation)
{
  m_mate_ex_state_array[material_index].SetBlendLogicOperation(operation);
}

H3dBlendExConst::Factor H3dModel::GetMaterialBlendFunctionSourceRgb(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendFunctionSourceRgb();
}
void H3dModel::SetMaterialBlendFunctionSourceRgb(s32 material_index, H3dBlendExConst::Factor factor)
{
  m_mate_ex_state_array[material_index].SetBlendFunctionSourceRgb(factor);
}
H3dBlendExConst::Factor H3dModel::GetMaterialBlendFunctionSourceAlpha(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendFunctionSourceAlpha();
}
void H3dModel::SetMaterialBlendFunctionSourceAlpha(s32 material_index, H3dBlendExConst::Factor factor)
{
  m_mate_ex_state_array[material_index].SetBlendFunctionSourceAlpha(factor);
}

H3dBlendExConst::Factor H3dModel::GetMaterialBlendFunctionDestinationRgb(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendFunctionDestinationRgb();
}
void H3dModel::SetMaterialBlendFunctionDestinationRgb(s32 material_index, H3dBlendExConst::Factor factor)
{
  m_mate_ex_state_array[material_index].SetBlendFunctionDestinationRgb(factor);
}
H3dBlendExConst::Factor H3dModel::GetMaterialBlendFunctionDestinationAlpha(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendFunctionDestinationAlpha();
}
void H3dModel::SetMaterialBlendFunctionDestinationAlpha(s32 material_index, H3dBlendExConst::Factor factor)
{
  m_mate_ex_state_array[material_index].SetBlendFunctionDestinationAlpha(factor);
}

H3dBlendExConst::Equation H3dModel::GetMaterialBlendEquationRgb(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendEquationRgb();
}
void H3dModel::SetMaterialBlendEquationRgb(s32 material_index, H3dBlendExConst::Equation equation)
{
  m_mate_ex_state_array[material_index].SetBlendEquationRgb(equation);
}
H3dBlendExConst::Equation H3dModel::GetMaterialBlendEquationAlpha(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetBlendEquationAlpha();
}
void H3dModel::SetMaterialBlendEquationAlpha(s32 material_index, H3dBlendExConst::Equation equation)
{
  m_mate_ex_state_array[material_index].SetBlendEquationAlpha(equation);
}



// Rasterization
void H3dModel::SetMaterialRasterizationCullingMode(s32 material_index, H3dRasterizationExConst::CullingMode cull)
{
  m_mate_ex_state_array[material_index].SetRasterizationCullingMode(cull);
}
H3dRasterizationExConst::CullingMode  H3dModel::GetMaterialRasterizationCullingMode(s32 material_index) const
{
  return m_mate_ex_state_array[material_index].GetRasterizationCullingMode();
}




void H3dModel::SetMaterialFogEnable(s32 material_index, b32 enable)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  if( enable )
  {
    materialState.EnableFlags(nw::h3d::MaterialFlag::FOG_ENABLED);
  }
  else
  {
    materialState.DisableFlags(nw::h3d::MaterialFlag::FOG_ENABLED);
  }
}
b32 H3dModel::IsMaterialFogEnable(s32 material_index) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  return materialState.TestFlagsAnd(nw::h3d::MaterialFlag::FOG_ENABLED);  // true/false�ŕԂ��Ă���
}
void H3dModel::SetMaterialFogIndex(s32 material_index, s32 index)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetFogId(index);
}
s32  H3dModel::GetMaterialFogIndex(s32 material_index) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  return materialState.GetFogId();
}

void H3dModel::SetMaterialLightSetIndex(s32 material_index, s32 index)
{
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  materialState.SetLightSetId(index);
}
s32  H3dModel::GetMaterialLightSetIndex(s32 material_index) const
{
  const nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  return materialState.GetLightSetId();
}




s32 H3dModel::GetBonesCount(void) const
{
  s32 num = m_cont->bones.size();
  //GFL_ASSERT_MSG(num==GetBonesCountOfWorldMatrixPose(), "Bone=%d, Matrix=%d\n", num, GetBonesCountOfWorldMatrixPose());  // GetBonesCount�Ɠ����l���Ԃ��Ă���Ǝv�����A�����łȂ��Ƃ��͂���̂��낤���H  //@check �����ɂЂ�������Ȃ��悤�Ɋm�F�ς̈�B
  GFL_ASSERT_MSG(num==m_skel_state.GetWorldMtxSize(), "Bone=%d, WorldMatrix=%d\n", num, m_skel_state.GetWorldMtxSize());  // GetBonesCount�Ɠ����l���Ԃ��Ă���Ǝv�����A�����łȂ��Ƃ��͂���̂��낤���H  //@check �����̊m�F�p�B
  GFL_ASSERT_MSG(num==m_skel_state.GetLocalMtxSize(), "Bone=%d, LocalMatrix=%d\n", num, m_skel_state.GetLocalMtxSize());  // GetBonesCount�Ɠ����l���Ԃ��Ă���Ǝv�����A�����łȂ��Ƃ��͂���̂��낤���H  //@check �����̊m�F�p�B
  return num;
}
s32 H3dModel::GetBonesIndex(const char* key) const
{
  if( key )
  {
    u16 index = m_cont->GetBoneIdx(key);
    if( index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
    {
      return index;
    }
  }
  return INDEX_NONE;  // = gfl::grp::g3d::Model::BONE_INDEX_NOT_FOUND
}

const char* H3dModel::GetBonesName(s32 bone_index) const
{
  return m_cont->bones[bone_index].name;
}


// �g�p�֎~
//s32 H3dModel::GetBonesCountOfWorldMatrixPose(void) const  // GetBonesCount�Ɠ����l���Ԃ��Ă���Ǝv�����A�����łȂ��Ƃ��͂���̂��낤���H
//{
//  u32 num = m_skel_state.GetWorldMtxSize();
//  return num;
//}




b32 H3dModel::GetBoneMatrixOfWorldMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    *matrix = m_skel_state.GetWorldMtx(bone_index);
    return true;
  }
  return false;
}
gfl::math::MTX34* H3dModel::GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index)
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    gfl::math::MTX34& matrix = m_skel_state.GetWorldMtx(bone_index);  // @note �Q�Ƃ̃A�h���X��������v�ȋC�����邪�A���[�J���ϐ���Ԃ��Ă���C������B���v�H �m���O�҂������͂���������v�B
    return &matrix;
  }
  return NULL;
}
const gfl::math::MTX34* H3dModel::GetBoneMatrixPointerOfWorldMatrixPose(s32 bone_index) const
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    const gfl::math::MTX34& matrix = m_skel_state.GetWorldMtx(bone_index);  // @note �Q�Ƃ̃A�h���X��������v�ȋC�����邪�A���[�J���ϐ���Ԃ��Ă���C������B���v�H �m���O�҂������͂���������v�B
    return &matrix;
  }
  return NULL;
}
//gfl::math::MTX34& H3dModel::GetBoneMatrixReferenceOfWorldMatrixPose(s32 bone_index)
//{
//  return m_skel_state.GetWorldMtx(bone_index);
//}
b32 H3dModel::SetBoneMatrixOfWorldMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix)
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    gfl::math::MTX34& l_matrix = m_skel_state.GetWorldMtx(bone_index);
    l_matrix = matrix;
    return true;
  }
  return false;
}


b32 H3dModel::GetBoneMatrixOfLocalMatrixPose(s32 bone_index, gfl::math::MTX34* matrix) const
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    const nw::h3d::LocalMtx& lm = m_skel_state.GetLocalMtx(bone_index);
    *matrix = lm.mtxRT;
    return true;
  }
  return false;
}
gfl::math::MTX34* H3dModel::GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index)
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    nw::h3d::LocalMtx& lm = m_skel_state.DirectLocalMtx(bone_index);
    return &(lm.mtxRT);
  }
  return NULL;
}
void H3dModel::SetBoneDirtyFlagOfLocalMatrixPose(s32 bone_index)
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    m_skel_state.SetLocalDirtyFlagAt(bone_index);
  }
}
const gfl::math::MTX34* H3dModel::GetBoneMatrixPointerOfLocalMatrixPose(s32 bone_index) const
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    const nw::h3d::LocalMtx& lm = m_skel_state.GetLocalMtx(bone_index);
    const gfl::math::MTX34* matrix = &(lm.mtxRT);
    return matrix; 
  }
  return NULL;
}
b32 H3dModel::SetBoneMatrixOfLocalMatrixPose(s32 bone_index, const gfl::math::MTX34& matrix)
{
  s32 num = GetBonesCount();
  if( 0<=bone_index && bone_index<num )
  {
    nw::h3d::LocalMtx& lm = m_skel_state.DirectLocalMtx(bone_index);
    lm.mtxRT = matrix;
    m_skel_state.SetLocalDirtyFlagAt(bone_index);
    return true;
  }
  return false;
}


void H3dModel::CreateReplaceBoneData(gfl::heap::NwAllocator* heap_allocator, H3dModel* base_model)
{
  s32 bones_count = this->GetBonesCount();
  m_replace_bone_data_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) ReplaceBoneData[bones_count];

  for( s32 b=0; b<bones_count; ++b )
  {
    const nw::h3d::res::BoneData& bone = m_cont->bones[b];
    const char* bone_name = bone.name;

    s32 base_index = base_model->GetBonesIndex(bone_name);
    if( base_index != gfl::grp::g3d::H3dModel::INDEX_NONE )
    {
      m_replace_bone_data_array[b].base_index = base_index;
    }
    else
    {
      m_replace_bone_data_array[b].base_index = gfl::grp::g3d::H3dModel::INDEX_NONE;
      
      // ��c��T��
      m_replace_bone_data_array[b].base_ancestor_index = gfl::grp::g3d::H3dModel::INDEX_NONE;  // �����l���
      u16 parent_index = bone.parentIdx;  // �e
      if( parent_index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
      {
        // �e������Ƃ�
        if( parent_index < b )
        {
          if( m_replace_bone_data_array[parent_index].base_index >= 0 )
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_index;
          }
          else
          {
            m_replace_bone_data_array[b].base_ancestor_index = m_replace_bone_data_array[parent_index].base_ancestor_index;
          }
        }
        else
        {
          GFL_ASSERT_MSG(0, "�{�[���̃C���f�b�N�X�͐e�قǏ������A�q�قǑ傫���B����O��ł����Ă���̂�ASSERT�ɂ��Ă����܂��B\n");
        }
      }
      //else
      //{
        // �e�����Ȃ��Ƃ�(�g�b�v�m�[�h�̂Ƃ�)
        // m_replace_bone_data_array[b].base_index��
        // m_replace_bone_data_array[b].base_ancestor_index��
        // gfl::grp::g3d::H3dModel::INDEX_NONE�̂܂܂ɂ��Ă����B
      //}
    }
  }
}

void H3dModel::DestroyReplaceBoneData(void)
{
  if( m_replace_bone_data_array )
  {
    GFL_DELETE_ARRAY m_replace_bone_data_array;
    m_replace_bone_data_array = NULL;
  }
}

void H3dModel::SetWorldReplaceBone(H3dModel* base_model)
{
  if( m_replace_bone_data_array )
  {
    s32 bones_count = this->GetBonesCount();
    for( s32 b=0; b<bones_count; ++b )
    {
      if( m_replace_bone_data_array[b].base_index != gfl::grp::g3d::H3dModel::INDEX_IGNORE )
      {
        // �������Ȃ��{�[���̂Ƃ�

        if( m_replace_bone_data_array[b].base_index >= 0 )
        {
          this->SetBoneMatrixOfWorldMatrixPose(b, *(base_model->GetBoneMatrixPointerOfWorldMatrixPose(m_replace_bone_data_array[b].base_index)));
        }
        else  // if( m_replace_bone_data_array[b].base_index == gfl::grp::g3d::H3dModel::INDEX_NONE )
        {
          const nw::math::MTX34* parent_world_matrix = NULL;
          if( m_replace_bone_data_array[b].base_ancestor_index >= 0 )
          {
            const nw::h3d::res::BoneData& bone = m_cont->bones[b];
            u16 parent_index = bone.parentIdx;  // �e
            GFL_ASSERT( parent_index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX );  //@fix
            // �{�[���̃C���f�b�N�X�͐e�قǏ������A�q�قǑ傫���B����O��Ȃ̂ŁA
            // b�ɐ�c������Ȃ�Ab�̐e�ł���parent_index�̃��[���h�}�g���N�X�v�Z�͏I����Ă���B
            // �e�̃��[���h�}�g���N�X 
            parent_world_matrix = this->GetBoneMatrixPointerOfWorldMatrixPose(parent_index);
          }
          else
          {
            // �e�����Ȃ��Ƃ�(�g�b�v�m�[�h�̂Ƃ�)
            parent_world_matrix = &(this->m_world_mtx);
          }

          // �����̃��[�J���}�g���N�X
          nw::math::MTX34  local_matrix;
          local_matrix.SetupIdentity();
          {
            const nw::math::MTX34& local_RT_matrix = m_skel_state.GetLocalMtx(b).mtxRT;
            const nw::math::VEC3&  local_scale     = m_skel_state.GetLocalMtx(b).scale;
    
            internal::CopyTranslate(&local_matrix, local_RT_matrix);
            internal::MultScale(&local_matrix, &local_RT_matrix, local_scale);
          }
    
          // �����̃��[���h�}�g���N�X = �e�̃��[���h�}�g���N�X * �����̃��[�J���}�g���N�X
          nw::math::MTX34  world_matrix;
          if(parent_world_matrix)
          {
            nw::math::MTX34Mult( &world_matrix, parent_world_matrix, &local_matrix );
          }
          else
          {
            world_matrix.SetupIdentity();
          }
  
          this->SetBoneMatrixOfWorldMatrixPose(b, world_matrix);
        }
      }
    }
  }
}

void H3dModel::IgnoreParentReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bone_index = GetBonesIndex(name);
    if( bone_index >= 0 )
    {
      const nw::h3d::res::BoneData* bone = &(m_cont->bones[bone_index]);
      u16 parent_index = bone->parentIdx;
      while( parent_index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
      {
        m_replace_bone_data_array[parent_index].base_index = gfl::grp::g3d::H3dModel::INDEX_IGNORE;
        bone = &(m_cont->bones[parent_index]);
        parent_index = bone->parentIdx;
      }
    }
  }
}
void H3dModel::IgnoreChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bone_index = GetBonesIndex(name);
    if( bone_index >= 0 )
    {
      s32 bones_count = this->GetBonesCount();
      for(s32 b=0; b<bones_count; ++b)
      {
        if( isAncestorBone(bone_index, b) ) m_replace_bone_data_array[b].base_index = gfl::grp::g3d::H3dModel::INDEX_IGNORE;
      }
    }
  }
}
void H3dModel::IgnoreSelfChildReplaceBone(const char* name)
{
  if( m_replace_bone_data_array )
  {
    s32 bone_index = GetBonesIndex(name);
    if( bone_index >= 0 )
    {
      m_replace_bone_data_array[bone_index].base_index = gfl::grp::g3d::H3dModel::INDEX_IGNORE;
      IgnoreChildReplaceBone(name);
    }
  }
}

b32  H3dModel::isAncestorBone(s32 ancestor_index, s32 self_index)
{
  b32 ret = false;
  if( ancestor_index >= 0 && self_index >= 0)
  {
    u16 parent_index = self_index;
    while( parent_index != nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX )
    {
      const nw::h3d::res::BoneData& bone = m_cont->bones[parent_index];
      if( bone.parentIdx == ancestor_index )
      {
        ret = true;
        break;
      }
      else
      {
        // ����ɐe��
        parent_index = bone.parentIdx;
      }
    }
  }
  return ret;
}


s32 H3dModel::GetMeshCount(void) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̌��ł͂Ȃ��A���b�V���̌���Ԃ��B
  //return m_cont->meshNodeVisibilities.size();  // �����b�V���m�[�h�̏ꍇ�͂�����ɂȂ�
  return m_cont->meshes.size();  // ���b�V���m�[�h�ł͂Ȃ����b�V�������炱����ɂȂ�Bnw::h3d::res::MeshData�̃����o�ł���subMeshes���ĉ��H
}
s32 H3dModel::GetMeshIndex(const char* name) const  // ���b�V���̖��O�����Ȃ��̂łȂ�
{
  //return m_cont->meshNodesDic.FindIndex( name );
  GFL_ASSERT_MSG(0, "GetMeshIndex�͎g�p�֎~�BGetMeshNodeIndex���g�p���ĉ������B\n");
  return INDEX_NONE;
}

const char* H3dModel::GetMeshName(s32 mesh_index) const  // ���b�V���̖��O�����Ȃ��̂łȂ�
{
  //return m_cont->meshNodesDic.GetName( mesh_index );
  GFL_ASSERT_MSG(0, "GetMeshName�͎g�p�֎~�BGetMeshNodeName���g�p���ĉ������B\n");
  return NULL;
}

b32 H3dModel::IsMeshVisible(const s32 mesh_index) const
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  return m_cont->meshes[mesh_index].IsVisible(m_visi_state);
}
void H3dModel::SetMeshVisible(const s32 mesh_index, const b32 visible)
{
  // ���b�V���m�[�h(���b�V���m�[�h�r�W�r���e�B�Ŏg���Ă������)�̃C���f�b�N�X�ł͂Ȃ��A���b�V���̃C���f�b�N�X�ł��B
  s32 mesh_node_index = m_cont->meshes[mesh_index].meshNodeId;  // ���b�V���̃C���f�b�N�X���烁�b�V���m�[�h�̃C���f�b�N�X�����߂�
  m_visi_state.SetMeshNodeVisibility(mesh_node_index, visible);
}

s32 H3dModel::GetMaterialIndexOfMesh(const s32 mesh_index) const
{
  return m_cont->meshes[mesh_index].materialId;
}


s32 H3dModel::GetMeshNodeCount(void) const
{
  // 1���b�V��������ꍇ�ABindByIndex�ł�m_cont->meshNodeVisibilities.size()��1�Am_cont->meshNodesDic.GetDataCount()��0�ɂȂ�B
  return m_cont->meshNodeVisibilities.size();  // m_cont->meshNodesDic.GetDataCount();�Ƃ͈Ⴄ�l�ɂȂ邱�Ƃ�����B
}

s32 H3dModel::GetMeshNodeDicCount(void) const
{
  return m_cont->meshNodesDic.GetDataCount();
}

s32 H3dModel::GetMeshNodeIndex(const char* name) const  // ���b�V���m�[�h�̖��O�͎���
{
  s32 dic_count = m_cont->meshNodesDic.GetDataCount();
  if(dic_count > 0)  // NintendoWare\CTR\sources\libraries\h3d\h3d_FoundationImpl.h  u16 PatriciaDic::FindIndex(const char* str, size_t len) const  �ŃG���[�ɂȂ�̂ŉ���B
  {
    u16 index = m_cont->GetMeshNodeIdx(name);
    if( index == nw::h3d::fnd::PatriciaDic::NOT_FOUND_INDEX ){ return INDEX_NONE; }
    else                                                     { return index;      }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���O���S�����݂��܂���Bdic_count=%d\n", dic_count);
    return INDEX_NONE;
  }
}
const char* H3dModel::GetMeshNodeName(s32 mesh_node_index) const  // ���b�V���m�[�h�̖��O�͎���
{
  s32 dic_count = m_cont->meshNodesDic.GetDataCount();
  if( mesh_node_index < dic_count )
  {
    return m_cont->meshNodesDic.GetName(mesh_node_index);
  }
  else
  {
    GFL_ASSERT_MSG(0, "�C���f�b�N�X�����O�����z���Ă��܂��Bmesh_node_index=%d, dic_count=%d\n", mesh_node_index, dic_count);
    return NULL;
  }
}

b32 H3dModel::IsMeshNodeVisible(const s32 mesh_node_index) const
{
  return m_visi_state.GetMeshNodeVisibility(mesh_node_index);
}
void H3dModel::SetMeshNodeVisible(const s32 mesh_node_index, const b32 visible)
{
  m_visi_state.SetMeshNodeVisibility(mesh_node_index, visible);
}


b32 H3dModel::CreateShaderExState(
    gfl::heap::NwAllocator* heap_allocator, 
    gfl::heap::NwAllocator* device_allocator, 
    s32 material_index,
    const H3dShaderExState::Description* desc
)
{
  nw::h3d::res::MaterialContent* material = m_cont->materials[material_index].content;
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->Create(heap_allocator, device_allocator, material, desc);
  return true;
}
void H3dModel::SetShaderSymbolVSFloat(s32 material_index, const char* name, const gfl::math::VEC4& value)
{
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->SetShaderSymbolVSFloat(name, value);
}
void H3dModel::SetShaderSymbolVSFloat(s32 material_index, s32 index, const gfl::math::VEC4& value)
{
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->SetShaderSymbolVSFloat(index, value);
}
void H3dModel::GetShaderSymbolVSFloat(s32 material_index, const char* name, gfl::math::VEC4* value) const
{
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->GetShaderSymbolVSFloat(name, value);
}
void H3dModel::GetShaderSymbolVSFloat(s32 material_index, s32 index, gfl::math::VEC4* value) const
{
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->GetShaderSymbolVSFloat(index, value);
}

s32 H3dModel::GetShaderSymbolIndex(s32 material_index, const char* name) const
{
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  return shader_ex_state->GetShaderSymbolIndex(name);
}

void H3dModel::DumpShaderSymbolName(s32 material_index) const
{
  nw::h3d::res::MaterialContent* material = m_cont->materials[material_index].content;
  H3dShaderExState* shader_ex_state = m_mate_ex_state_array[material_index].GetShaderExState();
  shader_ex_state->DumpShaderSymbolName(material);
}



b32 H3dModel::CreateAnimationManagerAndAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_num, const H3dAnimationSlot::Description* slot_desc_array)
{
  m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) H3dAnimationManager;
  return m_animation_manager->CreateAnimationManagerAndAnimationSlot(heap_allocator, this, slot_num, slot_desc_array);
}

b32 H3dModel::CreateAnimationManager(gfl::heap::NwAllocator* heap_allocator, s32 slot_num)
{
  m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) H3dAnimationManager;
  return m_animation_manager->CreateAnimationManager(heap_allocator, slot_num);
}

b32 H3dModel::CreateAnimationSlot(gfl::heap::NwAllocator* heap_allocator, s32 slot_index, const H3dAnimationSlot::Description* slot_desc)
{
  if( m_animation_manager )
  {
    return m_animation_manager->CreateAnimationSlot(heap_allocator, this, slot_index, slot_desc);
  }
  return false;
}

b32 H3dModel::IsAnimationSlotCreated(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->IsAnimationSlotCreated(slot_index);
  return false;
}

void H3dModel::DestroyAnimationManager(void)
{
  if( m_animation_manager ) GFL_DELETE m_animation_manager;
  m_animation_manager = NULL;
}

void H3dModel::DestroyAnimationSlot(s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->DestroyAnimationSlot(slot_index);
}

H3dAnimationSlot::ChangeAnimationResult H3dModel::ChangeAnimationByResAnim(s32 slot_index, H3dResAnim* res_anim)
{
  if( m_animation_manager ) return m_animation_manager->ChangeAnimationByResAnim(slot_index, res_anim);
  return H3dAnimationSlot::CHANGE_ANIMATION_RESULT_ERROR;
}
H3dAnimationSlot::ChangeAnimationResult H3dModel::ChangeAnimationByResAnimListIndex(s32 slot_index, s32 list_index)
{
  if( m_animation_manager ) return m_animation_manager->ChangeAnimationByResAnimListIndex(slot_index, list_index);
  return H3dAnimationSlot::CHANGE_ANIMATION_RESULT_ERROR;
}

H3dResAnim* H3dModel::GetAnimationAsResAnim(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->GetAnimationAsResAnim(slot_index);
  return NULL;
}

void H3dModel::SetAnimationFrame(f32 frame, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationFrame(slot_index, frame);
}
f32 H3dModel::GetAnimationFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->GetAnimationFrame(slot_index);
  return 0.0f;
}

void H3dModel::SetAnimationStepFrame(f32 step_frame, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationStepFrame(slot_index, step_frame);
}
f32 H3dModel::GetAnimationStepFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->GetAnimationStepFrame(slot_index);
  return 0.0f;
}

void H3dModel::SetAnimationStartFrame(f32 start_frame, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationStartFrame(slot_index, start_frame);
}
f32 H3dModel::GetAnimationStartFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->GetAnimationStartFrame(slot_index);
  return false;
}

void H3dModel::SetAnimationEndFrame(f32 end_frame, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationEndFrame(slot_index, end_frame);
}
f32 H3dModel::GetAnimationEndFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->GetAnimationEndFrame(slot_index);
  return false;
}

void H3dModel::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationStartAndEndFrame(slot_index, start_frame, end_frame);
}

b32 H3dModel::IsAnimationFrameStartFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->IsAnimationFrameStartFrame(slot_index);
  return false;
}
b32 H3dModel::IsAnimationFrameEndFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->IsAnimationFrameEndFrame(slot_index);
  return false;
}

b32 H3dModel::IsAnimationFrameStepTerminalFrame(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->IsAnimationFrameStepTerminalFrame(slot_index);
  return false;
}

b32 H3dModel::IsAnimationLoop(s32 slot_index) const
{
  if( m_animation_manager ) return m_animation_manager->IsAnimationLoop(slot_index);
  return false;
}
void H3dModel::SetAnimationLoop(b32 loop, s32 slot_index)
{
  if( m_animation_manager ) m_animation_manager->SetAnimationLoop(slot_index, loop);
}

b32 H3dModel::AddPreMeshDrawListener(H3dMeshListener* listener)
{
  return addMeshListener(m_pre_mesh_draw_listener_head, listener);
}

void H3dModel::RemovePreMeshDrawListener(H3dMeshListener* listener)
{
  return removeMeshListener(m_pre_mesh_draw_listener_head, listener);
}

b32 H3dModel::AddPostMeshDrawListener(H3dMeshListener* listener)
{
  return addMeshListener(m_post_mesh_draw_listener_head, listener);
}

void H3dModel::RemovePostMeshDrawListener(H3dMeshListener* listener)
{
  return removeMeshListener(m_post_mesh_draw_listener_head, listener);
}

b32 H3dModel::addMeshListener(H3dMeshListener*& head, H3dMeshListener* listener)
{
  GFL_ASSERT_MSG(!(listener->next), "listener�����ɂǂ����ɓo�^����Ă��܂��B\n");  //@check listener->next���c���Ă��܂�����������B

  H3dMeshListener* prev = NULL;
  H3dMeshListener* curr = head;
  while(curr)
  {
    prev = curr;
    curr = curr->next;
  }
  if( prev ) prev->next = listener;
  else       head = listener;
  listener->next = NULL;

  return true;  // ���true��Ԃ��Ƃ͌���܂���
}

void H3dModel::removeMeshListener(H3dMeshListener*& head, H3dMeshListener* listener)
{
  H3dMeshListener* prev = NULL;
  H3dMeshListener* curr = head;
  while(curr)
  {
    if( curr == listener )
    {
      if( prev ) prev->next = listener->next;
      else       head = listener->next;
      break;
    }
    prev = curr;
    curr = curr->next;
  }
}

void H3dModel::callMeshListenerInvoke(H3dMeshListener*& head, s32 mesh_index, H3dCommand* command, H3dEnvironment* env)
{
  H3dMeshListener* curr = head;
  while(curr)
  {
    curr->Invoke(this, mesh_index, command, env);
    curr = curr->next;
  }
}

//---------------------------------------------------------------------------------
/**
 * @brief CommandSizePrint�N���X
 * @param dcs
 * @param pPrintStr
 */
//---------------------------------------------------------------------------------
#if GFL_DEBUG
class CommandSizePrint{
public:
  CommandSizePrint( nn::gr::CTR::CommandBufferJumpHelper* dcs, const char* pPrintStr ):
    m_dcs( dcs ),
    m_pPrintStr( pPrintStr )
  {
    m_StartSize = m_dcs->GetSize();
  }

  ~CommandSizePrint()
  {
    //if ( DrawUtil::Debug::G3DH3DMODEL_DEBUG_PRINT )
#if 0
    {
      m_EndSize = m_dcs->GetSize();
      s32     size = m_EndSize - m_StartSize;
      if ( size )
        ISHIGURO_PRINT( "%s is %d Byte\n", m_pPrintStr, size );
    }
#endif
  }

private:
  nn::gr::CTR::CommandBufferJumpHelper* m_dcs;
  const char* m_pPrintStr;

  s32         m_StartSize;
  s32         m_EndSize;
};
#endif

void H3dModel::ReleaseMaterialCommandCache()
{
  GFL_SAFE_DELETE_ARRAY( m_pMemoryObject->m_MaterialCommandChaches );
}

void H3dModel::MakeMaterialCommandCache(gfl::heap::NwAllocator* device_allocator)
{
  if ( this->m_pMemoryObject && this->m_pMemoryObject->m_MaterialCommandChaches )
    return;//�m�ۍς�
  
  struct LocalFunc{

    //MaterialData::MakeMutableCommand�֐���SceneState�Ɗ֘A�̂Ȃ������̃R�}���h�쐬
    //���s������t���O�𗎂Ƃ�
    static void TestMutableCommand(nn::gr::CommandBufferJumpHelper& dst, const nw::h3d::MaterialState& materialState, const nw::h3d::res::MaterialData &rMaterialData )
    {
      using namespace nw;
      using namespace nw::h3d;
      
      {// @note SDK5.0.0�Ή�
	      if (materialState.TestFlagsOr(MaterialFlag::COMBINER_CONSTANT_MUTABLE))
	      {
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV0, materialState.GetConstantColor(0)));
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV1, materialState.GetConstantColor(1)));
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV2, materialState.GetConstantColor(2)));
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV3, materialState.GetConstantColor(3)));
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV4, materialState.GetConstantColor(4)));
	          dst.Put(fnd::Util::MakeCombinerConstantCommand(dst.GetCurrentPtr(), PICA_REG_TEX_ENV5, materialState.GetConstantColor(5)));
	      }
	    }
	    
      if (materialState.TestFlagsOr(MaterialFlag::BLEND_COLOR_MUTABLE))
      {
        dst.Put(fnd::Util::MakeBlendColorCommand(dst.GetCurrentPtr(), materialState.GetBlendColor()));
      }
      if (materialState.TestFlagsOr(MaterialFlag::TEXTURE_BORDER_COLOR_MUTABLE))
      {
        dst.Put(fnd::Util::MakeTexture0BorderColorCommand(dst.GetCurrentPtr(), materialState.GetBorderColor(0)));
        dst.Put(fnd::Util::MakeTexture1BorderColorCommand(dst.GetCurrentPtr(), materialState.GetBorderColor(1)));
        dst.Put(fnd::Util::MakeTexture2BorderColorCommand(dst.GetCurrentPtr(), materialState.GetBorderColor(2)));
      }
      if (materialState.TestFlagsOr(MaterialFlag::LIGHTING_COLOR_MUTABLE))
      {
        math::VEC4 ambColor(ut::FloatColor(materialState.GetAmbient()));
        math::VEC4 difColor(ut::FloatColor(materialState.GetDiffuse()));
        ambColor.w = rMaterialData.content->vertexColorScale;

        dst.Put(rMaterialData.content->shaderContent->binding->bindSymbolMatAmbi.MakeUniformCommand(
            dst.GetCurrentPtr(), ambColor));
        dst.Put(rMaterialData.content->shaderContent->binding->bindSymbolMatDiff.MakeUniformCommand(
            dst.GetCurrentPtr(), difColor));
      }
    }
    
    static void PickUpCommand( nn::gr::CTR::CommandBufferJumpHelper *dcs, const nw::h3d::MaterialState &materialState, const nw::h3d::res::MaterialData &material, H3dMaterialExState* p_mate_ex_state )
    {//�R�}���h���W
      //�m���̃R�}���h�̓W�����v�łȂ����Ƃ��m�F������Ŏ��W�B
      LocalFunc::TestMutableCommand(*dcs, materialState, material);
      material.MakeTextureSamplerCommand(*dcs, materialState);
      p_mate_ex_state->MakeMutableCommand( dcs );
      p_mate_ex_state->MakeTextureSamplerCommand( dcs, &materialState);
    }
    
    static void DirtyBitOff(nw::h3d::MaterialState& materialState, H3dMaterialExState* p_mate_ex_state)
    {
      using namespace nw;
      using namespace nw::h3d;
      
      materialState.DisableFlags(MaterialFlag::COMBINER_CONSTANT_MUTABLE);
      materialState.DisableFlags(MaterialFlag::BLEND_COLOR_MUTABLE);
      materialState.DisableFlags(MaterialFlag::TEXTURE_BORDER_COLOR_MUTABLE);
      materialState.DisableFlags(MaterialFlag::LIGHTING_COLOR_MUTABLE);

      
      p_mate_ex_state->ResetDirtyBits();
    }
  };
  
  m_pMemoryObject->m_MaterialCommandChaches = GFL_NEW_ARRAY(device_allocator->GetHeapBase()) util::DrawUtil::CacheCommand[ GetMaterialCount() ];

  
  for( s32 i = 0; i < GetMaterialCount(); ++i )
  {
    nw::h3d::MaterialState            &materialState = m_mate_state_array[ i ];
    const nw::h3d::res::MaterialData  &material = m_cont->materials[ i ];
    s32                               writeSize = 0;
    
    {//�R�}���h�����邩�`�F�b�N
      gfl::grp::util::DrawUtil::CacheCommand  sizeCheckDummy;
      static const int                        dummySize = 1024;

      sizeCheckDummy.Init( device_allocator, dummySize );
      LocalFunc::PickUpCommand( sizeCheckDummy.GetDrawCommandStream(), materialState, material, &m_mate_ex_state_array[i] );
      if ( sizeCheckDummy.WriteSize() )
        sizeCheckDummy.SetCH1KickCommand();
      
      writeSize = sizeCheckDummy.WriteSize();
      GFL_ASSERT( writeSize < dummySize );
    }

    if ( writeSize )
    {//�R�}���h�����W����
      //ISHIGURO_PRINT("cache size = %d\n", writeSize );

      m_pMemoryObject->m_MaterialCommandChaches[i].Init( device_allocator, writeSize );

      LocalFunc::PickUpCommand( m_pMemoryObject->m_MaterialCommandChaches[i].GetDrawCommandStream(), materialState, material, &m_mate_ex_state_array[i] );
      m_pMemoryObject->m_MaterialCommandChaches[i].SetCH1KickCommand();
      
      LocalFunc::DirtyBitOff(materialState, &m_mate_ex_state_array[i]);
    }
  }

}

#if 1

void H3dModel::MakeMaterialCommand(
    H3dCommand* command,
    H3dEnvironment* env,
    s32 material_index,
    bool fragmentAndWBufferDirty
)
{
  if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_MATERIAL ) )
    return;

  nn::gr::CTR::CommandBufferJumpHelper                  *dcs          =  command->GetNwDrawCommandStream();
  nw::h3d::SceneState                         *sceneState   =  env->GetNwSceneState();

  const nw::h3d::res::MaterialData            &material = m_cont->materials[material_index];
  nw::h3d::MaterialState                      &materialState = m_mate_state_array[material_index];
  H3dMaterialExState                          *mate_ex_state = m_mate_ex_state_array?(&m_mate_ex_state_array[material_index]):NULL;

  // ���_�V�F�[�_�R�}���h�𔭍s����
  if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
  {
    makeShaderCommand(command, env, material_index);
  }
  
  fragmentAndWBufferDirty = fragmentAndWBufferDirty || 
                            ( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH ) ) || 
                            materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY);
  
  {//���C�g�Z�b�g�����I�ɕύX���ꂽ�ꍇ�Buid�ł̓`�F�b�N�ł��Ȃ��̂Ŏ��O����
    if ( s_LastMaterialData.m_LightSetId != materialState.GetLightSetId() )
      fragmentAndWBufferDirty = true;//���C�g�Z�b�g�����I�ɕύX���ꂽ�ꍇ�B
  }
                            
  {//�t�H�O�����I�ɕύX���ꂽ�ꍇ�Buid�ł̓`�F�b�N�ł��Ȃ��̂Ŏ��O����
    if ( s_LastMaterialData.m_FogId != materialState.GetFogId() )
      s_LastMaterialData.m_Content = NULL;
  }
  
  if ( s_LastMaterialData.m_Content && material.content->EqualsWeak(s_LastMaterialData.m_Content) )
  {
    if ( fragmentAndWBufferDirty )
    {
      // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
      material.MakeCommandAndCache(*dcs, materialState, *sceneState);
      materialState.DisableFlags(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY);
    }
    else
    {
      *dcs  << material.content->paramCmd 
            << material.textureFuncCmd
            << materialState.GetTexture0Cmd()
            << materialState.GetTexture1Cmd()
            << materialState.GetTexture2Cmd();
    }
  }
  else
  {//�}�e���A���ݒ�
    
    if ( fragmentAndWBufferDirty )
    {
      // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
      material.MakeCommandAndCache(*dcs, materialState, *sceneState);
      materialState.DisableFlags(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY);
    }
    else
    {
      // ���C�g�̌�����ʒu�̃R�}���h�͏㏑�����Ă���B
      material.MakeCommand(*dcs, materialState, *sceneState);
    }
    
    if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENTLIGHT_ENABLED) )
    {
      material.MakeLightLutCommand(*dcs, materialState, *sceneState);
    }

    {//�t�H�O
      if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FOG_ENABLED) )
        makeFogCommand(*dcs, materialState, *sceneState, nw::h3d::Constants::INVALID_FOG_ID, *env);
      else
        dcs->Put(nn::gr::Fog::MakeDisableCommand(dcs->GetCurrentPtr()));
    }
  }
  
  material.MakeMutableCommand(*dcs, materialState, *sceneState);
  
  {
    gfl::grp::util::DrawUtil::CacheCommand      *pMaterialCommandChache = NULL;

    if ( m_pMemoryObject && m_pMemoryObject->m_MaterialCommandChaches )
      pMaterialCommandChache = &m_pMemoryObject->m_MaterialCommandChaches[ material_index ];

    if ( pMaterialCommandChache && pMaterialCommandChache->IsEnable() && pMaterialCommandChache->WriteSize() )
    {
      dcs->Jump( reinterpret_cast<uptr>( pMaterialCommandChache->GetDrawCommandStream()->GetStartPtr() ), pMaterialCommandChache->GetDrawCommandStream()->GetSize() );
    }
    else
    {
      material.MakeTextureSamplerCommand(*dcs, materialState);//�L���b�V��������Ă���Δ��s���Ȃ��B�֐����Ń_�[�e�B�r�b�g�͎����Ȃ��̂ł����Ŕ���B
    }
  }
  
  if(mate_ex_state)
  {
    if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
      mate_ex_state->GetShaderExState()->MakeShaderCommand(command);
      
    mate_ex_state->MakeMutableCommand(dcs);
    //mate_ex_state->MakeCombinerColorCommand(dcs, &materialState);// SDK 5.0�Ή�
    
    {// SDK 5.0�Ή�
      static const u32      s_RegNos[] = { PICA_REG_TEX_ENV0, PICA_REG_TEX_ENV1, PICA_REG_TEX_ENV2, PICA_REG_TEX_ENV3, PICA_REG_TEX_ENV4, PICA_REG_TEX_ENV5 };
      gfl::grp::ColorU8     l_c;
      s32                   constantNo;
      
      for( s32 step = 0; step < H3dTextureCombinerExConst::STEP_NUM; ++step )
      {
        constantNo = mate_ex_state->GetMaterialTextureCombinerConstant( step );
        
        if ( constantNo == -1 )
          continue;//�ύX�̂��������̂݃R�}���h��ς�
        
        l_c = materialState.GetConstantColor( constantNo );
        dcs->Put( nw::h3d::fnd::Util::MakeCombinerConstantCommand(dcs->GetCurrentPtr(), s_RegNos[step], l_c ) );
      }
    }
    
    mate_ex_state->MakeTextureSamplerCommand(dcs, &materialState);
  }

  // �`�抮����t���O�͗��Ƃ�
  SetBitFlagOff( &m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH );
  
  s_LastMaterialData.m_Content = material.content;
  s_LastMaterialData.m_LightSetId = materialState.GetLightSetId();
  s_LastMaterialData.m_FogId = materialState.GetFogId();
  
#ifdef MATERIAL_CACHE_DISABLE  
  if ( s_DebugFlag == false )
    ResetBeforeMaterialSetting();
#endif
}

#else

void H3dModel::MakeMaterialCommand(
    H3dCommand* command,
    H3dEnvironment* env,
    s32 material_index,
    bool fragmentAndWBufferDirty  // DrawMesh�ɂČ���false�����n���Ă��Ȃ��̂Ŏg���Ȃ��悤�ɂ��Ă���
)
{
  if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_MATERIAL ) )
    return;
  

  ///////////////////////////////////////////////
  bool fragmentAndWBufferDirty = false;a  // DrawMesh�ɂČ���false�����n���Ă��Ȃ�  // @note �C�V������T���v����OptimizingInfo�̂悤�ɉ������邱�ƁBH3dEnvironment�Ɏ�������Ƃ����Ǝv���B �� �R�����g�A�E�g����Ă���ӏ��Ȃ̂œ��ɏC�����Ȃ��B�R�����g�A�E�g���畜�������Ƃ��ɔ�����a��������make�G���[�Ŏ~�߂�B


  ///////////////////////////////////////////////

  nn::gr::CTR::CommandBufferJumpHelper*  dcs          =  command->GetNwDrawCommandStream();
  nw::h3d::SceneState*         sceneState   =  env->GetNwSceneState();

  const nw::h3d::res::MaterialData& material = m_cont->materials[material_index];
  nw::h3d::MaterialState& materialState = m_mate_state_array[material_index];
  H3dMaterialExState* mate_ex_state = m_mate_ex_state_array?(&m_mate_ex_state_array[material_index]):NULL;
  
  if ( s_LastMaterialData )
  {
    bool        fHit = false;
    
    static bool s_TestFlagHit = false;
    if ( s_TestFlagHit )
    {
      if ( material.content->EqualsStrict(s_LastMaterialData.m_Content) )
        fHit = true;
    }
    else
    {
      if ( material.content->EqualsWeak(s_LastMaterialData.m_Content) )
        fHit = true;
    }
    
    if( fHit )
    {
      {// �V�F�[�_�R�}���h�𔭍s����
        CommandSizePrint    debugPrint( dcs, "makeShaderCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960001, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
          makeShaderCommand(command, material_index);
      }
  
      static bool s_TestFlag7 = false;
      if ( s_TestFlag7 )
      {//�}�e���A���ݒ�
        {
          static const u32 s_TagCommands[] ={ 0x14960002, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }

        fragmentAndWBufferDirty = fragmentAndWBufferDirty || ( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH ) );

        if (fragmentAndWBufferDirty || materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY))
        {
          // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
          material.MakeCommandAndCache(*dcs, materialState, *sceneState);
          materialState.DisableFlags(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY);
        }
        else
        {
          material.MakeCommand(*dcs, materialState, *sceneState);
        }
      }
      else
      {
        *dcs  << material.content->paramCmd 
              << material.textureFuncCmd
              << materialState.GetTexture0Cmd()
              << materialState.GetTexture1Cmd()
              << materialState.GetTexture2Cmd();
      }
    
      static bool s_TestFlag0 = true;
      if ( s_TestFlag0 )
      {
        CommandSizePrint    debugPrint( dcs, "material.MakeMutableCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960003, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        material.MakeMutableCommand(*dcs, materialState, *sceneState);
      }

      static bool s_TestFlag1 = false;
      if ( s_TestFlag1 )
      {
        static b32      s_fSetLutEnable = false;

        if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENTLIGHT_ENABLED) || s_fSetLutEnable )
        {
          CommandSizePrint    debugPrint( dcs, "material.MakeLightLutCommand" );
          {
            static const u32 s_TagCommands[] ={ 0x14960004, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
            gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
          }
          material.MakeLightLutCommand(*dcs, materialState, *sceneState);
        }
      }

      static bool s_TestFlag2 = true;
      if ( s_TestFlag2 )
      // �V�F�[�_�p�����[�^�̓}�e���A���Őݒ肷��
      if(mate_ex_state)
      {
        CommandSizePrint    debugPrint( dcs, "MakeShaderCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960005, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
          mate_ex_state->GetShaderExState()->MakeShaderCommand(command);
      }

      static bool s_TestFlag3 = false;
      if ( s_TestFlag3 )
      if( mate_ex_state )
      {
        CommandSizePrint    debugPrint( dcs, "mate_ex_state->MakeMutableCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960006, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        mate_ex_state->MakeMutableCommand(dcs);
      }

      static bool s_TestFlag4 = false;
      if ( s_TestFlag4 )
      {
        {
          static const u32 s_TagCommands[] ={ 0x14960007, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        static b32      s_fFogEnable = false;

        if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FOG_ENABLED) || s_fFogEnable )
        {
          CommandSizePrint    debugPrint( dcs, "makeFogCommand" );
          makeFogCommand(*dcs, materialState, *sceneState, nw::h3d::Constants::INVALID_FOG_ID, *env);
        }
        else
        {
          dcs->Put(nn::gr::Fog::MakeDisableCommand(dcs->GetCurrentPtr()));
        }
      }

      static bool s_TestFlag5 = true;
      if ( s_TestFlag5 )
      {
        CommandSizePrint    debugPrint( dcs, "MakeTextureSamplerCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960008, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        material.MakeTextureSamplerCommand(*dcs, materialState);  // @note NintendoWareForCtr-2_4_1-sdk4_2_1-20120809-patch2-H3d-ja �����łŊm�F
      }

      static bool s_TestFlag6 = true;
      if ( s_TestFlag6 )
      if(mate_ex_state)
      {
        CommandSizePrint    debugPrint( dcs, "MakeTextureSamplerCommand" );
        {
          static const u32 s_TagCommands[] ={ 0x14960009, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
          gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
        }
        mate_ex_state->MakeTextureSamplerCommand(dcs, &materialState);
      }
      
      return;
    }
  }
  
  s_LastMaterialData.m_Content = material.content;
  
  if ( s_DebugFlag == false )
    ResetBeforeMaterialSetting();
  
  {
    static b32                          s_fmate_ex_stateEnambe = true;

    if ( s_fmate_ex_stateEnambe == false )
      mate_ex_state = NULL;
  }

  {// �V�F�[�_�R�}���h�𔭍s����
    CommandSizePrint    debugPrint( dcs, "makeShaderCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960001, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
      makeShaderCommand(command, material_index);
  }

  {//�}�e���A���ݒ�
    {
      static const u32 s_TagCommands[] ={ 0x14960002, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    
    fragmentAndWBufferDirty = fragmentAndWBufferDirty || ( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH ) );

    {
      if (fragmentAndWBufferDirty || materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY))
      {
        // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
        material.MakeCommandAndCache(*dcs, materialState, *sceneState);
        materialState.DisableFlags(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY);
        //ISHIGURO_PRINT("MakeCommandAndCache %s\n", fragmentAndWBufferDirty ? "true" : "false" );
      }
      else
      {
        material.MakeCommand(*dcs, materialState, *sceneState);
      }
    }
  }


  {
    CommandSizePrint    debugPrint( dcs, "material.MakeMutableCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960003, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    material.MakeMutableCommand(*dcs, materialState, *sceneState);
  }

  {
    static b32      s_fSetLutEnable = false;

    if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENTLIGHT_ENABLED) || s_fSetLutEnable )
    {
      CommandSizePrint    debugPrint( dcs, "material.MakeLightLutCommand" );
      {
        static const u32 s_TagCommands[] ={ 0x14960004, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
        gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
      }
      material.MakeLightLutCommand(*dcs, materialState, *sceneState);
    }
  }

  // �V�F�[�_�p�����[�^�̓}�e���A���Őݒ肷��
  if(mate_ex_state)
  {
    CommandSizePrint    debugPrint( dcs, "MakeShaderCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960005, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    if( IsBitFlagOnOne( env->GetRenderModeBitFlag(), H3dEnvironment::RENDER_MODE_BIT_FLAG_IGNORE_SHADER ) == false )
      mate_ex_state->GetShaderExState()->MakeShaderCommand(command);
  }

  if( mate_ex_state )
  {
    CommandSizePrint    debugPrint( dcs, "mate_ex_state->MakeMutableCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960006, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    mate_ex_state->MakeMutableCommand(dcs);
  }
  
  {
    {
      static const u32 s_TagCommands[] ={ 0x14960007, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    static b32      s_fFogEnable = false;

    if( materialState.TestFlagsOr(nw::h3d::MaterialFlag::FOG_ENABLED) || s_fFogEnable )
    {
      CommandSizePrint    debugPrint( dcs, "makeFogCommand" );
      makeFogCommand(*dcs, materialState, *sceneState, nw::h3d::Constants::INVALID_FOG_ID, *env);
    }
    else
    {
      dcs->Put(nn::gr::Fog::MakeDisableCommand(dcs->GetCurrentPtr()));
    }
  }

  {
    CommandSizePrint    debugPrint( dcs, "MakeTextureSamplerCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960008, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    material.MakeTextureSamplerCommand(*dcs, materialState);  // @note NintendoWareForCtr-2_4_1-sdk4_2_1-20120809-patch2-H3d-ja �����łŊm�F
  }
  
  if(mate_ex_state)
  {
    CommandSizePrint    debugPrint( dcs, "MakeTextureSamplerCommand" );
    {
      static const u32 s_TagCommands[] ={ 0x14960009, PICA_CMD_HEADER( PICA_REG_VS_ATTR_IN_REG_MAP0, 0, 0, 0 ) };
      gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
    }
    mate_ex_state->MakeTextureSamplerCommand(dcs, &materialState);
  }

  // �`�抮����t���O�͗��Ƃ�
  SetBitFlagOff( &m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH );
}

#endif

b32   H3dModel::s_GeometryShaderSetUpCheck = false;

void H3dModel::makeShaderCommand(
    H3dCommand* command,
    H3dEnvironment* env,
    s32 material_index
)
{
  nn::gr::CTR::CommandBufferJumpHelper          *dcs          =  command->GetNwDrawCommandStream(); 
  const nw::h3d::res::ShaderContent   *shaderContent = m_shader_cont_array[material_index]; 
  ShaderBind                          &rBind = m_shader_bind_array[material_index];
  
  if ( shaderContent->geoShaderIdx >= 0 )
  {// �W�I���g���V�F�[�_�[�̗L���A������ݒ�
    if ( s_GeometryShaderSetUpCheck == false )
    {
      dcs->Put( GeometryShaderCommand::MakeShaderModeCommand(dcs->GetCurrentPtr(), true) );
      s_GeometryShaderSetUpCheck = true;
    }
  }
  else if ( s_GeometryShaderSetUpCheck )
  {
    dcs->Put( GeometryShaderCommand::MakeShaderModeCommand(dcs->GetCurrentPtr(), false) );
    s_GeometryShaderSetUpCheck = false;
  }

  shaderContent->MakeCommand(*dcs, *(command->GetNwShaderState()) );

  dcs->Put(rBind.normalMtx.MakeUniformCommand(dcs->GetCurrentPtr(), m_normal_mtx));
  if( (m_cont->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL) == false )
    dcs->Put(rBind.univMtx.MakeUniformCommand(dcs->GetCurrentPtr(), m_world_mtx));

  {//�Œ�p�����[�^�[
    dcs->Put(rBind.worldMtx.MakeUniformCommand(dcs->GetCurrentPtr(), nn::math::MTX34::Identity()));
  }
    
  {//@fix ���삳��v�]�F�o�g���w�i�̃��C�e�B���O�Ή�
    s32     setID = this->GetMaterialLightSetIndex( material_index );
    gfl::grp::g3d::H3dEnvironment::VertexLightSet    *pVertexLightSet = env->GetVertexLightSet( setID );
    if ( pVertexLightSet && (pVertexLightSet->vertexLightSize > 0) )
      pVertexLightSet->MakeCommand(*dcs, rBind.lightCt, rBind.univMtx, rBind.hslGCol, rBind.hslSCol, rBind.hslSDir);
  }
}


// �V�F�[�_�p�����[�^�̓}�e���A���Őݒ肷��̂ŁA�}�e���A���Ɉړ�������B
/*
�V�F�[�_�p�����[�^�� Vector* �^��
"nw::h3d::res::MaterialContent::paramCmd"
�ɃR�}���h�Ƃ��Ċi�[����Ă��܂��B

�V�F�[�_�p�����[�^�� Bool �^��
"nw::h3d::res::SubMeshData::drawCmd"
�ɃR�}���h�Ƃ��Ċi�[����Ă��܂��B
*/
//#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
//  if(mate_ex_state) mate_ex_state->GetShaderExState()->StartOneFrameCommandCount();
//#endif
//  if(mate_ex_state) mate_ex_state->GetShaderExState()->MakeShaderCommand(command);
//#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
//  if(mate_ex_state) mate_ex_state->GetShaderExState()->EndOneFrameCommandCount();
//#endif



void H3dModel::drawMesh(
    H3dCommand* command,
    H3dEnvironment* env,
    const nw::h3d::res::MeshData& mesh,
    const nn::math::MTX34& normal
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    ,
    const gfl::math::MTX34&  view_mtx,
    const gfl::math::MTX34&  inv_view_mtx,
    const u32                inv_exist
#endif
    )
{
  /// �C���X�^���X�͔j�����ꂽ���A�����_�[�L���[�ɐς܂�Ă����Ԃ����m
  if( m_cont == NULL ){
    GFL_ASSERT_MSG( 0, "H3dModel�͂��łɔj������܂������A�����_�[�L���[�ɐς܂�Ă��܂�\n" );
    return ;
  }
 
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH

  // GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH��L���ɂ���ƁA�S�Ẵ��b�V���ɂ��̕��ׂ��|���Ă��܂����ƂɂȂ�̂ŁA
  // Scene��SubmitView��Draw�̊ԂɁu���f�����ƂɃJ�����ˑ��̏������ł���悤�Ȏd�g�݁v������A�����̏������폜����̂��A�����Ǝv���B
  // ���̏ꍇ�̎g�p��
  // ���f��Z���`��O���[�v0�ƕ`��O���[�v1�ɑ����Ă�����̂Ɖ��肷��
  // SubmitView(�`��O���[�v0, �J����a)
  // SubmitView(�`��O���[�v1, �J����b)
  // ���f���̃J�����ˑ��̏���(���f��Z, �J����a)
  // Draw(�`��O���[�v0, �J����a)
  // ���f���̃J�����ˑ��̏���(���f��Z, �J����b)
  // Draw(�`��O���[�v1, �J����b)

#if MODEL_COORDINATE_ENABLED
  nn::math::MTX34InvTranspose(&m_normal_mtx, &m_world_mtx);
  nn::math::MTX34Mult(&m_normal_mtx, &view_mtx, &m_normal_mtx);
#else                                                            //       ��ԍŌ��SubmitView�����J�����̒l��Draw���邱�ƂɂȂ��Ă������B��GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH�ɂđΏ����Ă��邪�A�������ׂ������邱�Ƃ����O���Ă���define�𖳌��ɂ��Ă���B
  nn::math::MTX34Copy(&m_normal_mtx, &view_mtx);             
#endif
      
  if( inv_exist )                                         
  {                                                      
    m_skel_state.UpdateBillboard(view_mtx, inv_view_mtx);
  }

#endif
  
  callMeshListenerInvoke(m_pre_mesh_draw_listener_head, mesh.materialId, command, env);
  
  ShaderBind& bind = m_shader_bind_array[mesh.materialId];
  
  // �}�e���A���R�}���h�𔭍s����
  MakeMaterialCommand( command, env, mesh.materialId, m_FragmentAndWBufferDirty );
  
  nn::gr::CTR::CommandBufferJumpHelper*  dcs          =  command->GetNwDrawCommandStream();

  {
    // �X�P���^�����f���������łȂ����f�����H
    if(mesh.parent->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL)  // @note MakeMaterialCommand����MakeShaderCommand�ł�����if���򂵂Ă���
    {
      //////// �X�P���^�����f�� ////////
      mesh.MakeCommand(*dcs, m_skel_state, bind.univMtx);
    }
    else
    {
      //////// �����łȂ����f�� ////////
      mesh.MakeCommand(*dcs);
    }
  }

  callMeshListenerInvoke(m_post_mesh_draw_listener_head, mesh.materialId, command, env);
}

/*
����O��drawMesh�̋L�q���c���Ă����B�������drawMesh�����肵���炱���̋L�q�͍폜����B
�����AdrawMesh�̂Ƃ��̓R�}���h�o�b�t�@�W�����v�����A�ʂɃ}�e���A���R�}���h��V�F�[�_�R�}���h�𔭍s����Ƃ��͎��ۂɔ��s����A�Ƃ����ӂ���
�g�������邩������Ȃ��̂ŁA�������drawMesh�͍���X�ɕύX����邩������Ȃ��B

void H3dModel::drawMesh(
    H3dCommand* command,
    H3dEnvironment* env,
    const nw::h3d::res::MeshData& mesh,
    const nn::math::MTX34& normal,
    bool fragmentAndWBufferDirty
)
{
  callMeshListenerInvoke(m_pre_mesh_draw_listener_head, mesh.materialId, command, env);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
  bit32* start_command;
  bit32* end_command;
  s32 command_num;
#endif

  nn::gr::CTR::CommandBufferJumpHelper*  dcs          =  command->GetNwDrawCommandStream();
  nw::h3d::ShaderState*        shaderState  =  command->GetNwShaderState();
  nw::h3d::SceneState*         sceneState   =  env->GetNwSceneState();

  const nw::h3d::res::MaterialData& material = mesh.parent->materials[mesh.materialId];
  const nw::h3d::res::ShaderContent* shaderContent = material.content->shaderContent;
  nw::h3d::MaterialState& materialState = m_mate_state_array[mesh.materialId];
  H3dMaterialExState* mate_ex_state = m_mate_ex_state_array?(&m_mate_ex_state_array[mesh.materialId]):NULL;
  ShaderBind& bind = m_shader_bind_array[mesh.materialId];

  /// cash�Đ������킽���Ă�����A������x�쐬
  fragmentAndWBufferDirty = fragmentAndWBufferDirty || ( IsBitFlagOnOne( m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH ) );

  // �X�P���^�����f���������łȂ����f�����H
  if(mesh.parent->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL)
  {
    //////// �X�P���^�����f�� ////////
    bool changed = shaderContent->MakeCommand(*dcs, *shaderState); // �V�F�[�_�[���؂�ւ������ true
    //if (changed)  // �y�d�v�zgflib�ł̓��f���P�ʂŕ`�悷��킯�ł͂Ȃ��ASubmitView�Ń��b�V�����Ƃɕ��ёւ��ĕ`�悵�Ă���̂ŁA���񃏁[���h�}�g���N�X�𑗂�Ȃ��Ƃ����Ȃ��B
    {
#if MODEL_COORDINATE_ENABLED
      dcs->Put(bind.worldMtx.MakeUniformCommand(dcs->Get(), m_world_mtx));
#else
      dcs->Put(bind.worldMtx.MakeUniformCommand(dcs->Get(), nn::math::MTX34::Identity()));
#endif
      dcs->Put(bind.normalMtx.MakeUniformCommand(dcs->Get(), normal));

      dcs->Put(bind.hslGCol.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
      dcs->Put(bind.hslSCol.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
      dcs->Put(bind.hslSDir.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 1.0f, 0.0f, 0.0f)));
    }
#if FRAGMENT_COMMAND_CACHE_ENABLED
    if (fragmentAndWBufferDirty || materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY))
    {
      // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
      material.MakeCommandAndCache(*dcs, materialState, *sceneState);
    }
    else
    {
      material.MakeCommand(*dcs, materialState, *sceneState);
    }
#else
    // ���t���[�� nw::h3d::res::MaterialData::MakeFragmentCommandCache() �Ăяo���K�v������ꍇ�A
    // nw::h3d::res::MaterialData::MakeCommandWithoutCache() �̎g�p�𐄏����܂��B
    material.MakeCommandWithoutCache(*dcs, materialState, *sceneState);
#endif

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    if(mate_ex_state) mate_ex_state->GetShaderExState()->StartOneFrameCommandCount();
    if(mate_ex_state) mate_ex_state->StartOneFrameCommandCount();
#endif

    if(mate_ex_state) mate_ex_state->GetShaderExState()->MakeShaderCommand(command);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    start_command = dcs->Get();
#endif
    material.MakeMutableCommand(*dcs, materialState, *sceneState);
#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    end_command = dcs->Get();
    command_num = end_command - start_command;
    GFL_PRINT("[material.MakeMutableCommand] end=%p, start=%p, command_num = %d\n", end_command, start_command, command_num);
#endif

    if(mate_ex_state) mate_ex_state->MakeMutableCommand(command);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    start_command = dcs->Get();
#endif
    material.MakeTextureSamplerCommand(*dcs, materialState);  // @note NintendoWareForCtr-2_4_1-sdk4_2_1-20120809-patch2-H3d-ja �����łŊm�F
#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    end_command = dcs->Get();
    command_num = end_command - start_command;
    GFL_PRINT("[material.MakeTextureSamplerCommand] end=%p, start=%p, command_num = %d\n", end_command, start_command, command_num);
#endif
    
    if(mate_ex_state) mate_ex_state->MakeTextureSamplerCommand(command, &materialState, material.textureSamplers);

    mesh.MakeCommand(*dcs, m_skel_state, bind.univMtx);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    if(mate_ex_state) mate_ex_state->GetShaderExState()->EndOneFrameCommandCount();
    if(mate_ex_state) mate_ex_state->EndOneFrameCommandCount();
#endif
  }
  else
  {
    //////// �����łȂ����f�� ////////
    bool changed = shaderContent->MakeCommand(*dcs, *shaderState);
    //if (changed)  // �y�d�v�zgflib�ł̓��f���P�ʂŕ`�悷��킯�ł͂Ȃ��ASubmitView�Ń��b�V�����Ƃɕ��ёւ��ĕ`�悵�Ă���̂ŁA���񃏁[���h�}�g���N�X�𑗂�Ȃ��Ƃ����Ȃ��B
    {
      dcs->Put(bind.univMtx.MakeUniformCommand(dcs->Get(), m_world_mtx));
      dcs->Put(bind.normalMtx.MakeUniformCommand(dcs->Get(), normal));

      dcs->Put(bind.hslGCol.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
      dcs->Put(bind.hslSCol.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 0.0f, 0.0f, 1.0f)));
      dcs->Put(bind.hslSDir.MakeUniformCommand(dcs->Get(), nw::math::VEC4(0.0f, 1.0f, 0.0f, 0.0f)));
    }
#if FRAGMENT_COMMAND_CACHE_ENABLED
    if (fragmentAndWBufferDirty || materialState.TestFlagsOr(nw::h3d::MaterialFlag::FRAGMENT_LIGHT_POLYGON_OFFSET_DIRTY))
    {
      // �t���O�����g���C�g�̐��A�ʒu�Ȃǂ� WBuffer �֘A�̕ύX���������ꍇ�̓L���b�V������蒼���܂��B
      material.MakeCommandAndCache(*dcs, materialState, *sceneState);
    }
    else
    {
      material.MakeCommand(*dcs, materialState, *sceneState);
    }
#else
    // ���t���[�� nw::h3d::res::MaterialData::MakeFragmentCommandCache() �Ăяo���K�v������ꍇ�A
    // nw::h3d::res::MaterialData::MakeCommandWithoutCache() �̎g�p�𐄏����܂��B
    material.MakeCommandWithoutCache(*dcs, materialState, *sceneState);
#endif

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    if(mate_ex_state) mate_ex_state->GetShaderExState()->StartOneFrameCommandCount();
    if(mate_ex_state) mate_ex_state->StartOneFrameCommandCount();
#endif

    if(mate_ex_state) mate_ex_state->GetShaderExState()->MakeShaderCommand(command);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    start_command = dcs->Get();
#endif
    material.MakeMutableCommand(*dcs, materialState, *sceneState);
#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    end_command = dcs->Get();
    command_num = end_command - start_command;
    GFL_PRINT("[material.MakeMutableCommand] end=%p, start=%p, command_num = %d\n", end_command, start_command, command_num);
#endif
    
    if(mate_ex_state) mate_ex_state->MakeMutableCommand(command);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    start_command = dcs->Get();
#endif
    material.MakeTextureSamplerCommand(*dcs, materialState);  // @note NintendoWareForCtr-2_4_1-sdk4_2_1-20120809-patch2-H3d-ja �����łŊm�F
#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    end_command = dcs->Get();
    command_num = end_command - start_command;
    GFL_PRINT("[material.MakeTextureSamplerCommand] end=%p, start=%p, command_num = %d\n", end_command, start_command, command_num);
#endif
    
    if(mate_ex_state) mate_ex_state->MakeTextureSamplerCommand(command, &materialState, material.textureSamplers);

    mesh.MakeCommand(*dcs);

#if GFL_GRP_G3D_H3D_DEBUG_COMMAND_COUNT
    if(mate_ex_state) mate_ex_state->GetShaderExState()->EndOneFrameCommandCount();
    if(mate_ex_state) mate_ex_state->EndOneFrameCommandCount();
#endif
  }

  // �`�抮����t���O�͗��Ƃ�
  SetBitFlagOff( &m_bit_flag, BIT_FLAG_MAKE_DRAW_CASH );
  
  callMeshListenerInvoke(m_post_mesh_draw_listener_head, mesh.materialId, command, env);
}
*/

void H3dModel::setShaderBind(s32 material_index, const nw::h3d::res::ShaderContent* shader_content)
{
  ShaderBind& bind = m_shader_bind_array.At(material_index);
  nn::gr::CTR::ShaderLite& shader = shader_content->binding->shader;

  {
    shader.SearchBindSymbol(&(bind.worldMtx), "WrldMtx");
    shader.SearchBindSymbol(&(bind.normalMtx), "NormMtx");
    shader.SearchBindSymbol(&(bind.projMtx), "ProjMtx");
    shader.SearchBindSymbol(&(bind.viewMtx), "ViewMtx");
    
    shader.SearchBindSymbol(&(bind.lightCt), "LightCt");//@fix ���삳��v�]�F�o�g���w�i�̃��C�e�B���O�Ή�
    shader.SearchBindSymbol(&(bind.univMtx), "UnivReg");
 
    shader.SearchBindSymbol(&(bind.hslGCol), "HslGCol");
    shader.SearchBindSymbol(&(bind.hslSCol), "HslSCol");
    shader.SearchBindSymbol(&(bind.hslSDir), "HslSDir");
 
    shader.SearchBindSymbol(&(bind.matAmbi), "MatAmbi");
    shader.SearchBindSymbol(&(bind.matDiff), "MatDiff");
  }
}

void H3dModel::calculateTransform(void)
{
  m_world_mtx.SetupIdentity();

  // scale
  m_world_mtx._00 = m_scale_vec.x;
  m_world_mtx._11 = m_scale_vec.y;
  m_world_mtx._22 = m_scale_vec.z;

  // rotate
  if( IsBitFlagOnOne(m_bit_flag, BIT_FLAG_ENABLE_ROTATE_VEC) )
  {
    m_rotate_mtx.SetupRotateXyz(m_rotate_vec);
  }
  gfl::math::MTX34Mult(&m_world_mtx, &m_rotate_mtx, &m_world_mtx);
  // @note m_translate_vec���������ɓ���������

  // translate
  m_world_mtx._03 = m_translate_vec.x;
  m_world_mtx._13 = m_translate_vec.y;
  m_world_mtx._23 = m_translate_vec.z;
}

void H3dModel::calculateAnimationManager(void)
{
  if( m_animation_manager ) m_animation_manager->Calculate();
}

f32 H3dModel::calculateDepthOfSkeletalModel(
  const nw::h3d::res::MeshData& mesh,
  const nw::gfx::Camera&        camera
)
{
  // �Q�l
  // NintendoWare/CTR/include/nw/gfx/gfx_RenderQueue.h
  // struct CalculateDepthOfSkeletalModelFunctor
  // ��
  // NintendoWare/CTR/include/nw/gfx/gfx_RenderQueue.h
  // template<typename ModelType, typename CalculateDepth, typename IsCalculating>
  // struct BasicEnqueueModelTranslucentModelBaseFunctor : public std::unary_function<ResMesh, void>
  // typedef BasicEnqueueModelTranslucentModelBaseFunctor<SkeletalModel, CalculateDepthOfSkeletalModelFunctor, AlwaysCalculatingFunctor>
  //     EnqueueSkeletalModelTranslucentModelBaseFunctor;
  // ��
  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_SceneUpdateHelper.h
  // template<typename IsVisibleModelFunctor>
  // static void SubmitView(
  // ��
  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_SceneUpdateHelper.h

  // �T�u���b�V�����P�̏ꍇ�̓{�[���̃}�g���N�X���g�p����
  if( mesh.subMeshes.size() == 1 )
  {
    const nw::h3d::res::SubMeshData& sub_mesh = mesh.subMeshes[0];
    if( sub_mesh.boneIdxCount > 0 )
    {
      s32 boneIndex = sub_mesh.boneIdxTable[0];
      return nw::gfx::SceneHelper::CalculateDepth(mesh.center, m_skel_state.GetWorldMtx(boneIndex), camera);
    }
  }
  return nw::gfx::SceneHelper::CalculateDepth(mesh.center, m_world_mtx, camera);
}

f32 H3dModel::calculateDepth(
  const nw::h3d::res::MeshData& mesh,
  const nw::gfx::Camera&        camera
)
{
  // �Q�l
  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_SceneHelper.h
  // CalculateDepth
  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_RenderQueue.h
  // CalculateDepthFunctor

  return nw::gfx::SceneHelper::CalculateDepth(mesh.center, m_world_mtx, camera);
}

void H3dModel::enqueue(
  nw::gfx::RenderQueue::ElementListType* list,
  const nw::gfx::Camera& camera
)
{
  // �Q�l
  // xy_ctrlib/NintendoWare/CTR/include/nw/gfx/gfx_RenderQueue.h
  // BasicEnqueueModelFunctor
  // EnqueueMesh
  //�o�^���ꂽ���ԁB��������j�[�N�h�c�Ƃ��Ďg���B
  u32   modelUniqueID = list->size();
  
  ModelTypeChanger model_type_changer;
  model_type_changer.h3d_model = this;
  nw::gfx::Model* gfx_model = model_type_changer.model;
 
  MeshTypeChanger mesh_type_changer;

  static const b32 IS_DEPTH_CALCULATING[nw::h3d::TranslucencyLayer::QUANTITY] =
  {
    false,
    true,
    false,
    false,
  };

  for(s32 layer=0; layer<nw::h3d::TranslucencyLayer::QUANTITY; ++layer)
  {
    nw::h3d::fnd::DataArray<nw::h3d::res::MeshData>::Range meshes =
        m_cont->layeredMeshes[layer];
    for(; meshes.first < meshes.second; ++meshes.first)
    {
      nw::h3d::res::MeshData* mesh = meshes.first;
      if( mesh->meshNodeId == 0xFFFF )
        continue;
      
      if(mesh->IsVisible(m_visi_state) == false )
        continue;
      
      {
        mesh_type_changer.h3d_mesh = mesh;
        nw::gfx::ResMesh gfx_mesh(mesh_type_changer.mesh_ptr);
 
        //nw::gfx::RenderQueue::ElementType element(gfx_mesh, gfx_model);  // ���������Ă���������
        nw::gfx::RenderElement element(gfx_mesh, gfx_model);
        bool pushed = list->push_back(element);  // list->PushBackFast(element);�ɒu�������Ă�������������Ȃ��B  2014/05/15_kawada_koji
        if(!pushed)
        {
          GFL_ASSERT_MSG(0, "H3dModel��RenderQueue�ɐς߂܂���\n");  // �ς߂Ȃ��ƕ`�悳��Ȃ��Ȃ邪����ȊO�̕s��͂Ȃ��B�C�t���悤��ASSERT��������Ă����B  2014/05/15_kawada_koji
          return;
        } 
 
        //nw::gfx::RenderQueue::ElementType* back = &(list->back());  // ���������Ă���������
        nw::gfx::RenderElement* back = &(list->back());

        f32 depth = 0.0f;

        //nw::gfx::res::ResMaterial::TranslucencyKind kind =
        //    static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(
        //        (mesh->key >> nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_SHIFT) & nw::h3d::MeshKeyBitField::TRANSLUCENCY_LAYER_MASK
        //    );
        // ���\�[�X�ɐݒ肵�Ă���l���g���̂ł͂Ȃ��A���������\�ȃ����o�ϐ����g���悤�ɂ���B
        nw::gfx::res::ResMaterial::TranslucencyKind kind =
            static_cast<nw::gfx::res::ResMaterial::TranslucencyKind>(
                m_mate_translucency_kind_array[mesh->materialId]
            );
        
        if ( IS_DEPTH_CALCULATING[kind] )
        {
          // �X�P���^�����f���������łȂ����f�����H
          if(m_cont->flags & nw::h3d::ModelFlag::IS_SKELETAL_MODEL)
          {
            //////// �X�P���^�����f�� ////////
            depth = calculateDepthOfSkeletalModel(*mesh, camera );
          }
          else
          {
            //////// �����łȂ����f�� ////////
            depth = calculateDepth(*mesh, camera );
          }
        }

        u8                              priority = (mesh->key >> nw::h3d::MeshKeyBitField::PRIORITY_SHIFT) & nw::h3d::MeshKeyBitField::PRIORITY_MASK;
        u32                             materialID = SortingMaterialIdGenerator::H3D_MATERIAL_ID;
        nw::h3d::res::MaterialContent   *pMaterialContent = m_cont->materials[mesh->materialId].content;
        
        //materialID = (modelUniqueID << 8) + mesh->materialId;
        materialID = pMaterialContent->uid;
        
        back->Key() = RenderElementAccessor::CreateH3dMeshRenderKey(
          (m_model_layer_id << 1),  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
          kind,                     // ���C���[�ԍ�
          priority,
          depth,
          materialID
        );
      }
    }
  }
}

/*
NintendoWare/CTR/sources/libraries/h3d/h3d_ResourceImpl.h
u16
MaterialData::MakeFogCommand(
    DrawCommandStream& dcs,
    const MaterialState& materialState,
    const SceneState& sceneState,
    u16 currentFogId) const
*/
u16 H3dModel::makeFogCommand(
    nn::gr::CTR::CommandBufferJumpHelper& dcs,
    const nw::h3d::MaterialState& materialState,
    const nw::h3d::SceneState& sceneState,
    u16 currentFogId,
    const H3dEnvironment& env) const
{
    // MaterialData ���̃����o���Q�ƁE�ύX���Ă��܂��񂪁A�� Make*Command() �֐��ɂȂ����
    // MaterialData �̃����o�֐��Ƃ��Ď������Ă���܂��B
    u16 index = materialState.GetFogId();
    if (currentFogId == index)
    {
        dcs.Put(env.MakeFogConfigCommand(index, dcs.GetCurrentPtr()));
    }
    else
    {
        dcs.Put(env.MakeFogAllCommand(index, dcs.GetCurrentPtr()));
    }
    return index;
}

u32 H3dModel::GetMaterialProjData( const s32 material_index )
{
  if ( m_mate_proj_array != NULL )
  {
    if( m_mate_proj_array[material_index] == MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE )
    {
      s32 user_data_index = GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_H3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
      if( user_data_index >= 0 )
      {
        gfl::grp::g3d::H3dUserData  user_data;
        GetMaterialUserData(material_index, user_data_index, &user_data);
        return user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
      }
    }
    else
    {
      return m_mate_proj_array[material_index];
    }
  }
  return MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
}

void H3dModel::SetMaterialProjData( const s32 material_index, const u32 value )
{
  if ( m_mate_proj_array != NULL )
  {
    m_mate_proj_array[material_index] = value;
  }
}

#undef MODEL_COORDINATE_ENABLED


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE

