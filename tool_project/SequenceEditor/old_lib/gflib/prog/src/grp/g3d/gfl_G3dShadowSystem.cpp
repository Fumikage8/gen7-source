//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowSystem.cpp
 *	@brief  3D�e�V�X�e��
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>
#include <grp/g3d/gfl_G3dShadowSystem.h>


namespace gfl {
namespace grp {
namespace g3d {

static const char* GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME = "rom:/kawada/shadow_resource/ShadowModel.bcmdl";

// �e�`��
static void BeginShadowDrawCommandInvoke(nw::gfx::RenderContext* render_context, void* work)
{
  nw::gfx::Material* shadow_setting_material = reinterpret_cast<nw::gfx::Material*>(work);
  render_context->SetMaterial(shadow_setting_material);
  render_context->ActivateContext();
  // 1Pass �ł� RENDERMODE_IGNORE_MATERIAL ��p���Ĉȍ~�̃}�e���A���ݒ���s���܂���B
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);

/*
  // ���̏��ԂȂ炤�܂�����
  render_context->SetMaterial(shadow_setting_material);
  render_context->ActivateContext();  // @note ���̒��g��ǂނ���
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);

  // ���̏��Ԃ��Ƃ��܂������Ȃ�
  render_context->SetMaterial(shadow_setting_material);
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_IGNORE_MATERIAL);
  render_context->ActivateContext();
*/
}
static void EndShadowDrawCommandInvoke(nw::gfx::RenderContext* render_context, void* work)
{
  GFL_UNUSED(work);

  // �����_�[���[�h���f�t�H���g�ɖ߂��A�}�e���A���ݒ肪�ʏ�ʂ�s����悤�ɂ��܂��B
  render_context->SetRenderMode(nw::gfx::RenderContext::RENDERMODE_DEFAULT);
}


//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�e�V�X�e���N���X
//=====================================

//-----------------------------------------------------------------------------
/**
 *					static�֐���`
 */
//-----------------------------------------------------------------------------
ShadowSystem::ReceiverType ShadowSystem::CheckShadowReceiverModel(
    const Model*                      model,
    const s32                         material_index
)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  return CheckShadowReceiverMaterial(
      material
  );
}

//-----------------------------------------------------------------------------
/*
 *  @brief    ���\�[�X�ɂ���e�N�X�`�����e�̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�����V���h�E��receiver���Z�b�g�A�b�v����
 *            ���̉e�V�X�e���ȊO������Ăяo���Ă��悢����ɂȂ��Ă���B
 *
 *  @param[in]  model
 *  @param[in]  material_index
 *  @param[in]  receiver_type
 *  @param[in]  shadow_intensity               0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
 *                                             shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
 *                                             shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
 *  @param[in]  projection_scene_camera_index
 *  @param[in]  texture_resource_pack          �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
 *  @param[in]  texture_resource               �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
 *  @param[in]  index_in_resource
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    ReceiverType receiver_type,
    f32          shadow_intensity,
    s32          projection_scene_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  SetupProjectionShadowReceiver(
    model,
    material_index,
    receiver_type,
    shadow_intensity,
    projection_scene_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

void ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    ReceiverType ,//receiver_type,  // receiver_type���������番�򂵂Ȃ���΂Ȃ�Ȃ�
    f32          shadow_intensity,
    s32          projection_scene_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
#if GFL_DEBUG
#define SETUP_PROJECTION_SHADOW_RECEIVER_PRINT (0)  // 1�̂Ƃ��f�o�b�O�o�͂���A0�̂Ƃ��Ȃ��B�K��0�ɂ��Ă������ƁB
#else
#define SETUP_PROJECTION_SHADOW_RECEIVER_PRINT (0)  // �f�o�b�O�łłȂ��Ƃ��͕K��0�ɂ���̂ŁA�����͕ύX���Ȃ����ƁB�ύX����Ȃ��̍s���B
#endif

#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  {
    nw::gfx::ResModel res_model = model->GetNwModel()->GetResModel();
    const char* name = res_model.GetName();
    if( name )
    {
      GFL_PRINT("model name = %s\n", name);
    }
    else
    {
      GFL_PRINT("model name = NULL\n");
    }
  }
#endif
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // �Z�b�g�A�b�v�ς݂��ۂ����肷��
  ///////////////////////////////////////////////////////
  {
    // �e�N�X�`���R���o�C�i��4�i�ڂ̃R���X�^���g�J���[��5�ł���A���A�\�[�XRGB��1�ł�����g�p���Ă���Ƃ��A�Z�b�g�A�b�v�ς݂Ɣ��肷��
    b32 b_setup = true;
    res_material = material->GetTextureCombinerResMaterial();
    nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
    if( res_tex_combiner.GetConstant() != nw::gfx::ResTextureCombiner::CONSTANT5 )
    {
      b_setup = false;
    }
    if( res_tex_combiner.GetSourceRgb(1) != nw::gfx::ResTextureCombiner::SOURCE_CONSTANT )
    {
      b_setup = false;
    }
    // �Z�b�g�A�b�v�ς݂̂Ƃ��A���������߂�
    if( b_setup ) return;
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0���󂯂�
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  {
    const char* name = res_material.GetName();
    if( name )
    {
      GFL_PRINT("material name = %s\n", name);
    }
    else
    {
      GFL_PRINT("material name = NULL\n");
    }
  }
#endif
  s32 tex_coord_count = res_material.GetTextureCoordinatorsCount();
  s32 active_tex_coord_count = res_material.GetActiveTextureCoordinatorsCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_coord_count=%d, active_tex_coord_count=%d\n", tex_coord_count, active_tex_coord_count);
  for( s32 i=0; i<tex_coord_count; ++i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(i);
    GFL_PRINT("v%d, e%d, d%d\n", res_tex_coord.IsValid(), res_tex_coord.IsEnabled(), res_tex_coord.IsDirty());
  }
#endif
  if(tex_coord_count == 0) return;  // �e�N�X�`���Ȃ��̓_���B
  GFL_ASSERT_STOP(tex_coord_count <= 3);  // @note �v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B
  if(active_tex_coord_count == 0) return;  // �e�N�X�`���Ȃ��̓_���B
  GFL_ASSERT_STOP(active_tex_coord_count <= tex_coord_count);  // @note �v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B
  //�e�R�L������for( s32 i=tex_coord_count -1; i>0; --i )  // gfx_MaterialState.cpp:362 Panic:Projection mapping can use coordinator0 or coordinator1.
  for( s32 i=active_tex_coord_count -1; i>0; --i )         // ��L�̂悤�ȃG���[�ɂȂ����̂ŁA�L���ȃe�N�X�`���R�[�f�B�l�[�^�̐������ōς܂����Ƃɂ����B
                                                           // �u�e�R�L�������v�����̑Ή����s�����ꏊ�ɕt���Ă���L�[���[�h�B
  {
    nw::gfx::ResTextureCoordinator res_tex_coord_a = res_material.GetTextureCoordinators(i);
    nw::gfx::ResTextureCoordinator res_tex_coord_b = res_material.GetTextureCoordinators(i-1);
    // res_tex_coord_a = res_tex_coord_b;  // res_tex_coord_b(i-1)��res_tex_coord_a(i)�ɃR�s�[����
    if( res_tex_coord_b.IsValid() )
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetScale           ( res_tex_coord_b.GetScale()            );
        res_tex_coord_a.SetRotate          ( res_tex_coord_b.GetRotate()           );
        res_tex_coord_a.SetTranslate       ( res_tex_coord_b.GetTranslate()        );
        res_tex_coord_a.SetSourceCoordinate( res_tex_coord_b.GetSourceCoordinate() );
        res_tex_coord_a.SetMappingMethod   ( res_tex_coord_b.GetMappingMethod()    );
        res_tex_coord_a.SetReferenceCamera ( res_tex_coord_b.GetReferenceCamera()  );
        res_tex_coord_a.SetMatrixMode      ( res_tex_coord_b.GetMatrixMode()       );
        res_tex_coord_a.SetDirty           ( res_tex_coord_b.IsDirty()             );  // �Z�b�g���Ȃ��Ă�OK����
        res_tex_coord_a.SetEnabled         ( res_tex_coord_b.IsEnabled()           );  // �Z�b�g���Ȃ��Ă�OK����
        res_tex_coord_a.SetTextureMatrix   ( res_tex_coord_b.GetTextureMatrix()    );
      }
      else
      {
        GFL_ASSERT_STOP( res_tex_coord_a.IsValid() );  // ���\�[�X���Ȃ��ꍇ�A������@���Ȃ��B
      }
    }
    else
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetEnabled         ( false                                 );  // �Z�b�g���Ȃ��Ă�OK����
      }
      else
      {
        // �������Ȃ�
      }
    }
  }
  //�e�R�L������active_tex_coord_count += 1;
  //�e�R�L������if(active_tex_coord_count > 3) active_tex_coord_count = 3;  // ���X3�e�N�X�`�����g�p���Ă����悤�Ȃ̂�1�㏑������Ă��܂����B
  //�e�R�L������                                                            // @note �v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B
  res_material.SetActiveTextureCoordinatorsCount(active_tex_coord_count);
  res_material.SetTextureCoordinateConfig(nw::gfx::res::ResMaterial::CONFIG_0120);  // Get���Ȃ�

  // �e�N�X�`���}�b�p�[
  // ���Ƃ�nw::gfx::ResPixelBasedTextureMapper���v���O�����Ő����ł��Ȃ������݂����_���������̂ŁA
  // �Ō�̃e�N�X�`�����_�~�[�ŃZ�b�g���Ă����Ă��炤���Ƃɂ����B
  res_material = material->GetTextureMapperResMaterial();
  s32 tex_mapper_count = res_material.GetTextureMappersCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_mapper_count=%d\n", tex_mapper_count);
  for( s32 i=0; i<tex_mapper_count; ++i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(i);
    GFL_PRINT("m%d\n", res_tex_mapper.IsValid());
    if( res_tex_mapper.IsValid() )
    {
      nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
      GFL_PRINT("s%d\n", res_tex_sampler.IsValid());
    }
  }
#endif
  GFL_ASSERT_STOP(tex_mapper_count == 3);  // @note 3�Œ�̂悤�����A�v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B

  b32 first = true;
  for( s32 i=3-1; i>0; --i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_a = res_material.GetTextureMappers(i);
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_b = res_material.GetTextureMappers(i-1);
    //res_tex_mapper_a = res_tex_mapper_b;  // res_tex_mapper_b(i-1)��res_tex_mapper_a(i)�ɃR�s�[����
    if( res_tex_mapper_b.IsValid() )
    {
      if( res_tex_mapper_a.IsValid() )
      {
        // �e�N�X�`��
        nw::gfx::ResTexture res_tex = res_tex_mapper_a.GetTexture();
        if( first )
        {
          if( res_tex.IsValid() )
          {
            res_tex = res_tex.Dereference();
            //res_tex.Cleanup();  // Cleanup�͂��Ȃ��̂ŁA���̃e�N�X�`����Setup���Ȃ��Ă��܂��g�p�ł���
          }
          first = false;
        }
        res_tex_mapper_a.SetTexture( res_tex_mapper_b.GetTexture().Dereference() );
      
        // �e�N�X�`���T���v���[
        nw::gfx::ResTextureSampler res_tex_sampler_a = res_tex_mapper_a.GetSampler();
        nw::gfx::ResTextureSampler res_tex_sampler_b = res_tex_mapper_b.GetSampler();
        if( res_tex_sampler_b.IsValid() )
        {
          res_tex_sampler_a.SetMinFilter( res_tex_sampler_b.GetMinFilter() );
          res_tex_sampler_a.SetMagFilter( res_tex_sampler_b.GetMagFilter() );
          res_tex_sampler_a.SetSamplerType( res_tex_sampler_b.GetSamplerType() );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_a = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_a);
          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_b = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_b);
          if( res_standard_tex_sampler_b.IsValid() )
          {
            res_standard_tex_sampler_a.SetBorderColor( res_standard_tex_sampler_b.GetBorderColor() );
            res_standard_tex_sampler_a.SetWrapS      ( res_standard_tex_sampler_b.GetWrapS() );
            res_standard_tex_sampler_a.SetWrapT      ( res_standard_tex_sampler_b.GetWrapT() );
            res_standard_tex_sampler_a.SetMinLod     ( res_standard_tex_sampler_b.GetMinLod() );
            res_standard_tex_sampler_a.SetLodBias    ( res_standard_tex_sampler_b.GetLodBias() );
            res_standard_tex_sampler_a.SetMinFilter  ( res_standard_tex_sampler_b.GetMinFilter() );
            res_standard_tex_sampler_a.SetMagFilter  ( res_standard_tex_sampler_b.GetMagFilter() );
          }
          else
          {
            // �������Ȃ�
          }
        }
        else
        {
          // �������Ȃ�
        }
      }
      else
      {
        // �������Ȃ�
      }
    }
    else
    {
      if( res_tex_mapper_a.IsValid() )
      {
        //res_tex_mapper_a.Cleanup();  // nw::gfx::ResPixelBasedTextureMapper��Cleanup���Ȃ�������ǂ��Ȃ�̂��낤��?
      }
    }
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i0�`3�i�ڂ̃e�N�X�`���̔ԍ���1�傫������
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  res_material = material->GetTextureCombinerResMaterial();
  nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
  GFL_ASSERT_STOP( res_fragment_shader.IsValid() );  // �t���O�����g�V�F�[�_��0�i�ڂ͕K������͂�
  s32 tex_combiner_count = res_fragment_shader.GetTextureCombinersCount();
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  GFL_PRINT("tex_combiner_count=%d\n", tex_combiner_count);
#endif
  GFL_ASSERT_STOP(tex_combiner_count == 6);  // 6�Œ�̂悤���B
  STATIC_ASSERT(
         nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1
      && nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2
  );  // SOURCE_TEXTURE0����SOURCE_TEXTURE2�̕��я����ς���Ă��Ȃ����Ƃ̊m�F�̂���
#if SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
  for( s32 i=0; i<6; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);
    GFL_PRINT("v%d\n", res_tex_combiner.IsValid());
  }
#endif
  for( s32 i=0; i<=3; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);

    for( s32 j=0; j<3; ++j )
    {
      nw::gfx::ResTextureCombiner::Source source;
      
      source = res_tex_combiner.GetSourceRgb(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner.SetSourceRgb(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        // �e�N�X�`����3���g�p���Ă���̂ŁA�e�e�N�X�`���ɒu���������炨�����Ȍ��ʂɂȂ��Ă��܂��B
        //GFL_ASSERT_STOP(0);  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
      }
      
      source = res_tex_combiner.GetSourceAlpha(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner.SetSourceAlpha(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        // �e�N�X�`����3���g�p���Ă���̂ŁA�e�e�N�X�`���ɒu���������炨�����Ȍ��ʂɂȂ��Ă��܂��B
        //GFL_ASSERT_STOP(0);  // @note �ꎞ�I�ɃR�����g�A�E�g�B�R�����g�A�E�g�͂�߂āA����ASSERT�������悤�ɖ߂��Ă������ق��������B
      }
    }
  }

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  // �}�e���A���J���[
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetReferenceCamera(projection_scene_camera_index);
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    }
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
    res_tex_coord.SetEnabled( true );  // �Z�b�g���Ȃ��Ă�OK����
  }

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
    
    // �e�N�X�`��
    model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);
    
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_LINEAR_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
      
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  {
    // 3�i�ڂ̏o�͌��ʂ��o�b�t�@����
    // GetBufferInputRgb��SetBufferInputRgb�ɂ�NW_MINMAX_ASSERT(index, 1, 4);�Ƃ�����������B
    // 0�i�ڂ̏o�͌��ʂ��o�b�t�@����ꍇ�́A1�i�ڂ�BUFFERINPUT_PREVIOUS��ݒ肷��B�����0�͔͈͊O�B
    // 4�i�ڂ̏o�͌��ʂ��o�b�t�@�������̂́A5�i�ڂőO�i�̃o�b�t�@�Ƃ��Ďg���Ă��O�i�̏o�͌��ʂƂ��Ďg���Ă������B�����5�͔͈͊O�B
    res_fragment_shader.SetBufferInputRgb(4, nw::gfx::res::ResFragmentShader::BUFFERINPUT_PREVIOUS);
    //res_fragment_shader.SetBufferInputAlpha(4, nw::gfx::res::ResFragmentShader::BUFFERINPUT_PREVIOUS);  // �A���t�@��3�i�ڂ̏o�͌��ʂ��ŏI�I�ɂ��̂܂܏o���̂ŃR�����g�A�E�g�B
  }
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
    res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // �e�N�X�`��0�𔽓]
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
    res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
    res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
    res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
    res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
    res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
    res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }

#if 0
  //�e�X�g�L�q�A�R�����g�A�E�g�̂܂�
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i5�i�ڂ�ύX����
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
  }
#endif

  ///////////////////////////////////////
  // �ύX�𔽉f����
  ///////////////////////////////////////
  res_material.SetTextureCoordinatorsHash(0x0);
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
  res_material.SetTextureCombinersHash(0x0);
  res_material.SetMaterialColorHash(0x0);

#undef SETUP_PROJECTION_SHADOW_RECEIVER_PRINT
}



//-----------------------------------------------------------------------------
/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 */
//-----------------------------------------------------------------------------
b32 ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SetupProjectionShadowReceiver(
    model,
    material_index,
    shadow_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SetupProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // �e�N�X�`���R���o�C�i
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // �e�N�X�`��0�𔽓]
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);


      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource, index_in_resource );
  
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionShadowReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // �e�N�X�`���R���o�C�i
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // �e�N�X�`��0�𔽓]
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);

      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SwitchOnProjectionShadowReceiver(
    model,
    material_index,
    shadow_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    Model*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource, index_in_resource );
    
    ret = true;
  }
  
  return ret;
}

b32 ShadowSystem::SwitchOffProjectionShadowReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
      
    ret = true;
  }

  return ret;
}


#if GFL_GRP_G3D_H3D_USE
// h3d
//----------------------------------------------------------------------------
/**
 *	@brief  H3d��
 */
//-----------------------------------------------------------------------------
b32 ShadowSystem::SetupProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      
      // �e�N�X�`���R���o�C�i
      {
        // 4�i��
        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // �e�N�X�`��0�𔽓]
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // �e�N�X�`��0

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂Ō��Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̐F�����̂܂܏o��)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5�i��
        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE );  // ��Z

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER);  // 3�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);  // 4�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);

        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource );
  
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      {
        // 4�i��
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // �e�N�X�`��0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // �e�N�X�`��0�𔽓]
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  

        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5�i��
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR );
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif
      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
    ret = true; 
  }

  return ret;
}


b32 ShadowSystem::SwitchOnProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index,
    const gfl::grp::ColorF32&  shadow_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionShadowModel( model, material_index, shadow_intensity, projection_camera_index, texture_resource );
    
    ret = true;
  }
  
  return ret;
}

b32 ShadowSystem::SwitchOffProjectionShadowReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionShadowModel( model, material_index, border_color ); 
      
    ret = true;
  }

  return ret;
}

#endif // GFL_GRP_G3D_H3D_USE



//-----------------------------------------------------------------------------
/*
 *  @brief    ���\�[�X�ɂ���e�N�X�`�������̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�������C�g��receiver��On/Off����
 *            ���̉e�V�X�e���ȊO������Ăяo���Ă��悢����ɂȂ��Ă���B
 *            Setup�̓f�t�H���g�̐ݒ���s��On�ɂ���
 *            Cleanup�͍Œ���̐ݒ�߂����s��Off�ɂ���(���S�ɐݒ��߂����Ƃ͂ł��܂���)
 *            SwitchOn�͐ݒ��ύX����On�ɂ���
 *            SwitchOff�͐ݒ��ύX������Off�ɂ���
 *
 *  @param[in]  model
 *  @param[in]  material_index
 *  @param[in]  light_intensity                0�`1  // texture_resource*light_intensity �����̐F�ɂȂ�B
 *                                             light_intensity=1�̂Ƃ�texture_resource�̐F�̂܂܂̌��ɂȂ�B
 *                                             light_intensity=0�̂Ƃ����͏o�Ȃ��B
 *  @param[in]  projection_camera_index
 *  @param[in]  texture_resource_pack          ���Z����Ƃ���͍��łȂ��F(���Ƃ�)�A���Z���Ȃ��Ƃ���͍��ɂȂ��Ă���B
 *  @param[in]  texture_resource               ���Z����Ƃ���͍��łȂ��F(���Ƃ�)�A���Z���Ȃ��Ƃ���͍��ɂȂ��Ă���B
 *  @param[in]  index_in_resource
 *
 *  @retval     b32   On/Off�ł�����true��Ԃ��BOn/Off�ł��Ȃ�������false��Ԃ�(���[�U�f�[�^��ProjectionType���Ԉ���Ă���ꍇ�Ȃ�false�ɂȂ�)
 */
//-----------------------------------------------------------------------------
/// gfx��
b32 ShadowSystem::SetupProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SetupProjectionLightReceiver(
    model,
    material_index,
    light_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}

b32 ShadowSystem::SetupProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      // �e�𗎂Ƃ��ݒ�����𗎂Ƃ��ݒ�ɕύX����

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // �e�N�X�`���R���o�C�i
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // �e�N�X�`��0
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂Ō��Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̐F�����̂܂܏o��)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
      }
      {
        // 5�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌���
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_ADD );  // ���Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT);

      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource, index_in_resource );
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionLightReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      nw::gfx::ResMaterial res_material;
      
      // �e�N�X�`���R���o�C�i
      res_material = material->GetTextureCombinerResMaterial();
      nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
      {
        // 4�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(4);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 );  // �e�N�X�`��0�𔽓]
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
        
        res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 );
      }
      {
        // 5�i��
        nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
        
        res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );

        res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
        res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
        
        res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );  // ��Z
        
        res_tex_combiner.SetSourceAlpha ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS );
        res_tex_combiner.SetOperandAlpha( 0, nw::gfx::ResTextureCombiner::OPERANDALPHA_SRC_ALPHA );
        res_tex_combiner.SetCombineAlpha( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
      }
      res_material.SetTextureCombinersHash(0x0);

      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);

      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
  
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource::TextureResourcePack* texture_resource_pack
)
{
  return SwitchOnProjectionLightReceiver(
    model,
    material_index,
    light_intensity,
    projection_camera_index,
    texture_resource_pack->resource,
    texture_resource_pack->index_in_resource
  );
}
b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    Model*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    Resource*    texture_resource,
    s32          index_in_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource, index_in_resource );
    ret = true;
  }
  
  return ret;
}
b32 ShadowSystem::SwitchOffProjectionLightReceiver(
    Model*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::UserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���ꍇ������̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
    ret = true;
  }

  return ret;
}


#if GFL_GRP_G3D_H3D_USE
// H3d��
b32 ShadowSystem::SetupProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif

  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW:
    {
      // �e�𗎂Ƃ��ݒ�����𗎂Ƃ��ݒ�ɕύX����

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      
      // �e�N�X�`���R���o�C�i
      {
        // 4�i��
        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // �e�N�X�`��0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  // �e�N�X�`��0

        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂Ō��Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̐F�����̂܂܏o��)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5�i��
        
        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_ADD ); // ��Z

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER);  // 3�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);  // 4�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);

        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT );
#endif
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource );
    ret = true;
  }

  return ret;
}

b32 ShadowSystem::CleanupProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;
#if 0  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }
#else
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  type_value = model->GetMaterialProjData( material_index );
#endif
  b32 type_value_check = false;
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      // ���𗎂Ƃ��ݒ���e�𗎂Ƃ��ݒ�ɖ߂�

      ///////////////////////////////////////////////////////
      // �e�N�X�`���R���o�C�i4�i�ځA5�i�ڂ�ݒ肷��
      ///////////////////////////////////////////////////////
      
      // �e�N�X�`���R���o�C�i
      {
        // 4�i��
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_TEXTURE0);  // �e�N�X�`��0
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR);  // �e�N�X�`��0�𔽓]
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_CONSTANT);  // �R���X�^���g�J���[5(0���Ƌ��x0�Ȃ̂ŉe�Ȃ��A1���Ƌ��x1�Ȃ̂Ńe�N�X�`��0�̉e�F�����̂܂܏o��)
        model->SetMaterialTextureCombinerOperandRgb( material_index, 4, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR);  

        model->SetMaterialTextureCombinerCombineRgb( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 4, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 4, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
        
        model->SetMaterialTextureCombinerConstant( material_index, 4, 5);
      }
      {
        // 5�i��
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS_BUFFER );  // 3�i�ڂ̏o�͌���
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_SRC_COLOR );
        
        model->SetMaterialTextureCombinerSourceRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);          // 4�i�ڂ̏o�͌��ʂ𔽓]
        model->SetMaterialTextureCombinerOperandRgb( material_index, 5, 1, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDRGB_ONE_MINUS_SRC_COLOR );

        model->SetMaterialTextureCombinerCombineRgb( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_MODULATE ); // ��Z
        
        model->SetMaterialTextureCombinerCombineAlpha( material_index, 5, gfl::grp::g3d::H3dTextureCombinerExConst::COMBINE_REPLACE );
        model->SetMaterialTextureCombinerSourceAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::SOURCE_PREVIOUS);
        model->SetMaterialTextureCombinerOperandAlpha( material_index, 5, 0, gfl::grp::g3d::H3dTextureCombinerExConst::OPERANDALPHA_SRC_ALPHA);
      }
#if 0
      user_data.SetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW);
#else
      model->SetMaterialProjData( material_index, gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW );
#endif
      border_color.Set(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B

      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
  
    ret = true; 
  }

  return ret;
}

b32 ShadowSystem::SwitchOnProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index,
    //f32          light_intensity,
    const gfl::grp::ColorF32&  light_intensity,
    s32          projection_camera_index,
    H3dResTexture*    texture_resource
)
{
  b32 ret = false;
  
  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    SetupProjectionLighModel( model, material_index, light_intensity, projection_camera_index, texture_resource );
    ret = true;
  }
  
  return ret;
}
b32 ShadowSystem::SwitchOffProjectionLightReceiver(
    H3dModel*       model,
    s32          material_index
)
{
  b32 ret = false;

  s32 user_data_index = model->GetMaterialUserDataIndex(material_index, GFL_GRP_G3D_MATERIAL_USER_DATA_PROJECTION_TYPE_NAME);
  gfl::grp::g3d::H3dUserData  user_data;
  s32 type_value = gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_NONE;
  if( user_data_index >= 0 )
  {
    model->GetMaterialUserData(material_index, user_data_index, &user_data);
    type_value = user_data.GetIntValue(gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_INDEX);
  }

  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���ꍇ������̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  b32 type_value_check = false;
  switch( type_value )
  {
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_LIGHT:
    {
      type_value_check = true;
    }
    break;
  case gfl::grp::g3d::MATERIAL_USER_DATA_PROJECTION_TYPE_TYPE_VALUE_DROP_SHADOW_LIGHT:
    {
      type_value_check = true;
    }
    break;
  }

  if( type_value_check )
  {
    CleanUpProjectionLightModel(model, material_index, border_color);
    ret = true;
  }

  return ret;
}
#endif // GFL_GRP_G3D_H3D_USE


// friend�N���X����̂݃A�N�Z�X���Ă��悢private�����o�֐�

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
ShadowSystem::ShadowSystem(void)
  : m_shadow_setting_resource_array(NULL),
    m_shadow_setting_model_array(NULL),
    m_shadow_group_num(0),
    m_shadow_group_array(NULL)
{
  // �������Ȃ�
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
ShadowSystem::~ShadowSystem()
{
  Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���� / �j��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::Create(
    gfl::heap::NwAllocator*         heap_allocator,
    gfl::heap::NwAllocator*         device_allocator,
    const Description*              description )
{
  Description  l_description;
  if( description )
  {
    l_description = *description;
  }

  {
    m_shadow_setting_resource_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource*[SHADOW_SETTING_RESOURCE_MAX];
    
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW]->LoadAndSetup(
      device_allocator,
      GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME
    );

    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Resource;
    m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP]->LoadAndSetup(
      device_allocator,
      GFL_GRP_G3D_SHADOW_SYSTEM_SHADOW_SETTING_MODEL_NAME
    );

    m_shadow_setting_model_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model*[SHADOW_SETTING_MODEL_MAX];
    
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->Create(
      heap_allocator,
      device_allocator,
      m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW],
      0
    );
    SetupProjectionShadowSettingModel( m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel() );
    
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::Model;
    m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP]->Create(
      heap_allocator,
      device_allocator,
      m_shadow_setting_resource_array[SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP],
      0
    );
    SetupShadowMapSettingModel( m_shadow_setting_model_array[SHADOW_SETTING_MODEL_SHADOW_MAP]->GetNwModel() );
  }

  m_shadow_group_num = l_description.shadow_group_num;
  m_shadow_group_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup*[m_shadow_group_num];
  for( u32 i=0; i<m_shadow_group_num; ++i )
  {
    if( l_description.shadow_group_description_array )
    {
      m_shadow_group_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup(
          heap_allocator,
          device_allocator,
          &l_description.shadow_group_description_array[i] );
    }
    else
    {
      m_shadow_group_array[i] = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::ShadowGroup(
          heap_allocator,
          device_allocator );
    }
  }
}

void ShadowSystem::Destroy(void)
{
  if( m_shadow_group_array )
  {
    for( u32 i=0; i<m_shadow_group_num; ++i )
    {
      if( m_shadow_group_array[i] )
      {
        GFL_DELETE m_shadow_group_array[i];
        m_shadow_group_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_group_array;
    m_shadow_group_array = NULL;
  }
  m_shadow_group_num = 0;

  if( m_shadow_setting_model_array )
  {
    for( u32 i=0; i<SHADOW_SETTING_MODEL_MAX; ++i )
    {
      if( m_shadow_setting_model_array[i] )
      {
        GFL_DELETE m_shadow_setting_model_array[i];
        m_shadow_setting_model_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_setting_model_array;
    m_shadow_setting_model_array = NULL;
  }

  if( m_shadow_setting_resource_array )
  {
    for( u32 i=0; i<SHADOW_SETTING_RESOURCE_MAX; ++i )
    {
      if( m_shadow_setting_resource_array[i] )
      {
        GFL_DELETE m_shadow_setting_resource_array[i];
        m_shadow_setting_resource_array[i] = NULL;
      }
    }
    GFL_DELETE_ARRAY m_shadow_setting_resource_array;
    m_shadow_setting_resource_array = NULL;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �e��receiver�ƂȂ郂�f�����Z�b�g�A�b�v����
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  s32 shadow_camera_index = GetShadowGroupCameraIndex(shadow_group_index);
  {
    nw::gfx::res::ResTexture res_texture = GetShadowGroupRenderTarget(shadow_group_index)->GetNwResTexture();

    SetupShadowReceiverMaterial(
      material,
      res_texture,
      type,
      shadow_intensity,
      receiver_type, 
      shadow_camera_index
    );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �e��receiver�Ƃ��ăZ�b�g�A�b�v�ς݂̃��f��(SetupShadowReceiverModel���������Ă��郂�f��)��On/Off
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SwitchShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, b32 receive_on)
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  s32 shadow_camera_index = GetShadowGroupCameraIndex(shadow_group_index);
  {
    nw::gfx::res::ResTexture res_texture = GetShadowGroupRenderTarget(shadow_group_index)->GetNwResTexture();

    if( receive_on )
    {
      SwitchOnShadowReceiverMaterial(
        material,
        res_texture,
        type,
        receiver_type
      );
    }
    else
    {
      SwitchOffShadowReceiverMaterial(
        material,
        res_texture,
        type,
        receiver_type
      );
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �e��receiver�Ƃ��ăZ�b�g�A�b�v�ς݂̃��f��(SetupShadowReceiverModel���������Ă��郂�f��)�̉e�̋��x��ݒ肷��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetShadowReceiverModelShadowIntensity(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity)
    // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  gfl::grp::g3d::ShadowGroup::Type type = GetShadowGroupType(shadow_group_index);
  {
    SetShadowReceiverMaterialShadowIntensity(
      material,
      type,
      shadow_intensity,
      receiver_type
    );  // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v���e�p�ɃZ�b�g�A�b�v����
 */
//-----------------------------------------------------------------------------
void  ShadowSystem::SetupSceneDrawGroupForShadow(const gfl::grp::g3d::ShadowGroup::Type type, gfl::grp::g3d::SceneDrawGroup* scene_draw_group) const
{
  switch( type )
  {
  case gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW:
    {
      nw::gfx::Material* material = m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel()->GetMaterial(0);
      
      scene_draw_group->SetBeginCommandInvokeCallback(
        BeginShadowDrawCommandInvoke,
        material
      );
      scene_draw_group->SetEndCommandInvokeCallback(
        EndShadowDrawCommandInvoke,
        NULL
      );
    }
    break;
  case gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP:
    {
      nw::gfx::Material* material = m_shadow_setting_model_array[SHADOW_SETTING_MODEL_PROJECTION_SHADOW]->GetNwModel()->GetMaterial(0);
      
      scene_draw_group->SetBeginCommandInvokeCallback(
        BeginShadowDrawCommandInvoke,
        material
      );
      scene_draw_group->SetEndCommandInvokeCallback(
        EndShadowDrawCommandInvoke,
        NULL
      );
    }
    break;
  }
}


// friend�N���X������A�N�Z�X���Ă͂����Ȃ�private�����o�֐�
// �֗��֐��⏈�����e�𒊏o�����֐�

void ShadowSystem::SetupShadowReceiverMaterial(
    nw::gfx::Material*                      material,
    const nw::gfx::ResTexture               shadow_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
    const gfl::grp::g3d::ShadowGroup::Type  type,
    const f32                               shadow_intensity,  // 0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
                                                               // shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
                                                               // shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
    const ReceiverType                      receiver_type,  // @note receiver_type���������番�򂵂Ȃ���΂Ȃ�Ȃ�
    const s32                               shadow_camera_index
)
{
  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0���󂯂�
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  s32 tex_coord_count = res_material.GetTextureCoordinatorsCount();
  s32 active_tex_coord_count = res_material.GetActiveTextureCoordinatorsCount();
#if 0
  GFL_PRINT("tex_coord_count=%d, active_tex_coord_count=%d\n", tex_coord_count, active_tex_coord_count);
  for( s32 i=0; i<tex_coord_count; ++i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(i);
    GFL_PRINT("v%d, e%d, d%d\n", res_tex_coord.IsValid(), res_tex_coord.IsEnabled(), res_tex_coord.IsDirty());
  }
#endif
  GFL_ASSERT_STOP(tex_coord_count > 0);  // �e�N�X�`���Ȃ��̓_���B
  GFL_ASSERT_STOP(tex_coord_count <= 3);  // @note �v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B
  GFL_ASSERT_STOP(active_tex_coord_count > 0);  // �e�N�X�`���Ȃ��̓_���B
  GFL_ASSERT_STOP(active_tex_coord_count <= tex_coord_count);  // @note �v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B
  for( s32 i=tex_coord_count -1; i>0; --i )
  {
    nw::gfx::ResTextureCoordinator res_tex_coord_a = res_material.GetTextureCoordinators(i);
    nw::gfx::ResTextureCoordinator res_tex_coord_b = res_material.GetTextureCoordinators(i-1);
    // res_tex_coord_a = res_tex_coord_b;
    if( res_tex_coord_b.IsValid() )
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetScale           ( res_tex_coord_b.GetScale()            );
        res_tex_coord_a.SetRotate          ( res_tex_coord_b.GetRotate()           );
        res_tex_coord_a.SetTranslate       ( res_tex_coord_b.GetTranslate()        );
        res_tex_coord_a.SetSourceCoordinate( res_tex_coord_b.GetSourceCoordinate() );
        res_tex_coord_a.SetMappingMethod   ( res_tex_coord_b.GetMappingMethod()    );
        res_tex_coord_a.SetReferenceCamera ( res_tex_coord_b.GetReferenceCamera()  );
        res_tex_coord_a.SetMatrixMode      ( res_tex_coord_b.GetMatrixMode()       );
        res_tex_coord_a.SetDirty           ( res_tex_coord_b.IsDirty()             );  // �Z�b�g���Ȃ��Ă�OK����
        res_tex_coord_a.SetEnabled         ( res_tex_coord_b.IsEnabled()           );  // �Z�b�g���Ȃ��Ă�OK����
        res_tex_coord_a.SetTextureMatrix   ( res_tex_coord_b.GetTextureMatrix()    );
      }
      else
      {
        GFL_ASSERT_STOP( res_tex_coord_a.IsValid() );  // ���\�[�X���Ȃ��ꍇ�A������@���Ȃ��B
      }
    }
    else
    {
      if( res_tex_coord_a.IsValid() )
      {
        res_tex_coord_a.SetEnabled         ( false                                 );  // �Z�b�g���Ȃ��Ă�OK����
      }
      else
      {
        // �������Ȃ�
      }
    }
  }
  //�e�R�L������res_material.SetActiveTextureCoordinatorsCount(active_tex_coord_count+1);
  res_material.SetTextureCoordinateConfig(nw::gfx::res::ResMaterial::CONFIG_0120);  // Get���Ȃ�

  // �e�N�X�`���}�b�p�[
  // ���Ƃ�nw::gfx::ResPixelBasedTextureMapper���v���O�����Ő����ł��Ȃ������݂����_���������̂ŁA
  // �Ō�̃e�N�X�`�����_�~�[�ŃZ�b�g���Ă����Ă��炤���Ƃɂ����B
  res_material = material->GetTextureMapperResMaterial();
  s32 tex_mapper_count = res_material.GetTextureMappersCount();
#if 0
  GFL_PRINT("tex_mapper_count=%d\n", tex_mapper_count);
  for( s32 i=0; i<tex_mapper_count; ++i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(i);
    GFL_PRINT("m%d\n", res_tex_mapper.IsValid());
    if( res_tex_mapper.IsValid() )
    {
      nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
      GFL_PRINT("s%d\n", res_tex_sampler.IsValid());
    }
  }
#endif
  GFL_ASSERT_STOP(tex_mapper_count == 3);  // @note 3�Œ�̂悤�����A�v���V�[�W�����e�N�X�`�����g����悤�ɂȂ�����Ή�����K�v������B

  b32 first = true;
  for( s32 i=3-1; i>0; --i )
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_a = res_material.GetTextureMappers(i);
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper_b = res_material.GetTextureMappers(i-1);
    //res_tex_mapper_a = res_tex_mapper_b;
    if( res_tex_mapper_b.IsValid() )
    {
      if( res_tex_mapper_a.IsValid() )
      {
        // �e�N�X�`��
        nw::gfx::ResTexture res_tex = res_tex_mapper_a.GetTexture();
        if( first )
        {
          if( res_tex.IsValid() )
          {
            res_tex = res_tex.Dereference();
            res_tex.Cleanup();
          }
          first = false;
        }
        res_tex_mapper_a.SetTexture( res_tex_mapper_b.GetTexture().Dereference() );
      
        // �e�N�X�`���T���v���[
        nw::gfx::ResTextureSampler res_tex_sampler_a = res_tex_mapper_a.GetSampler();
        nw::gfx::ResTextureSampler res_tex_sampler_b = res_tex_mapper_b.GetSampler();
        if( res_tex_sampler_b.IsValid() )
        {
          res_tex_sampler_a.SetMinFilter( res_tex_sampler_b.GetMinFilter() );
          res_tex_sampler_a.SetMagFilter( res_tex_sampler_b.GetMagFilter() );
          res_tex_sampler_a.SetSamplerType( res_tex_sampler_b.GetSamplerType() );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_a = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_a);
          nw::gfx::ResStandardTextureSampler res_standard_tex_sampler_b = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler_b);
          if( res_standard_tex_sampler_b.IsValid() )
          {
            res_standard_tex_sampler_a.SetBorderColor( res_standard_tex_sampler_b.GetBorderColor() );
            res_standard_tex_sampler_a.SetWrapS      ( res_standard_tex_sampler_b.GetWrapS() );
            res_standard_tex_sampler_a.SetWrapT      ( res_standard_tex_sampler_b.GetWrapT() );
            res_standard_tex_sampler_a.SetMinLod     ( res_standard_tex_sampler_b.GetMinLod() );
            res_standard_tex_sampler_a.SetLodBias    ( res_standard_tex_sampler_b.GetLodBias() );
            res_standard_tex_sampler_a.SetMinFilter  ( res_standard_tex_sampler_b.GetMinFilter() );
            res_standard_tex_sampler_a.SetMagFilter  ( res_standard_tex_sampler_b.GetMagFilter() );
          }
          else
          {
            // �������Ȃ�
          }
        }
        else
        {
          // �������Ȃ�
        }
      }
      else
      {
        // �������Ȃ�
      }
    }
    else
    {
      if( res_tex_mapper_a.IsValid() )
      {
        res_tex_mapper_a.Cleanup();
      }
    }
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i0�i�ځA1�i�ڂ��󂯂�
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  res_material = material->GetTextureCombinerResMaterial();
  nw::gfx::ResFragmentShader res_fragment_shader = res_material.GetFragmentShader();
  GFL_ASSERT_STOP( res_fragment_shader.IsValid() );  // �t���O�����g�V�F�[�_��0�i�ڂ͕K������͂�
  s32 tex_combiner_count = res_fragment_shader.GetTextureCombinersCount(); 
//  GFL_PRINT("tex_combiner_count=%d\n", tex_combiner_count);
  GFL_ASSERT_STOP(tex_combiner_count == 6);  // 6�Œ�̂悤���B
  STATIC_ASSERT(
         nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1
      && nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1+1 == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2
  );  // SOURCE_TEXTURE0����SOURCE_TEXTURE2�̕��я����ς���Ă��Ȃ����Ƃ̊m�F�̂���
#if 0
  for( s32 i=0; i<6; ++i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(i);
    GFL_PRINT("v%d\n", res_tex_combiner.IsValid());
  }
#endif
  for( s32 i=6-1-1; i>2; --i )  // GetBufferInputRgb��SetBufferInputRgb�ɂ�NW_MINMAX_ASSERT(index, 1, 4);�Ƃ�����������B
  {
    res_fragment_shader.SetBufferInputRgb(i, res_fragment_shader.GetBufferInputRgb(i-2));
    res_fragment_shader.SetBufferInputAlpha(i, res_fragment_shader.GetBufferInputAlpha(i-2));
  }
  for( s32 i=6-1; i>1; --i )
  {
    nw::gfx::ResTextureCombiner res_tex_combiner_a = res_fragment_shader.GetTextureCombiners(i);
    nw::gfx::ResTextureCombiner res_tex_combiner_b = res_fragment_shader.GetTextureCombiners(i-2);
    //res_tex_combiner_a = res_tex_combiner_b;
    res_tex_combiner_a.SetConstant    ( res_tex_combiner_b.GetConstant()     ); 
    res_tex_combiner_a.SetCombineRgb  ( res_tex_combiner_b.GetCombineRgb()   ); 
    res_tex_combiner_a.SetCombineAlpha( res_tex_combiner_b.GetCombineAlpha() ); 
    res_tex_combiner_a.SetScaleRgb    ( res_tex_combiner_b.GetScaleRgb()     ); 
    res_tex_combiner_a.SetScaleAlpha  ( res_tex_combiner_b.GetScaleAlpha()   ); 

    for( s32 j=0; j<3; ++j )
    {
      res_tex_combiner_a.SetSourceRgb   ( j, res_tex_combiner_b.GetSourceRgb(j)    ); 
      res_tex_combiner_a.SetSourceAlpha ( j, res_tex_combiner_b.GetSourceAlpha(j)  ); 
      res_tex_combiner_a.SetOperandRgb  ( j, res_tex_combiner_b.GetOperandRgb(j)   ); 
      res_tex_combiner_a.SetOperandAlpha( j, res_tex_combiner_b.GetOperandAlpha(j) ); 

      nw::gfx::ResTextureCombiner::Source source;
      
      source = res_tex_combiner_a.GetSourceRgb(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner_a.SetSourceRgb(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        GFL_ASSERT_STOP(0);
      }
      
      source = res_tex_combiner_a.GetSourceAlpha(j);
      if(    nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 <= source
          && source <= nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )
      {
        res_tex_combiner_a.SetSourceAlpha(j, static_cast<nw::gfx::ResTextureCombiner::Source>((static_cast<s32>(source))+1));
      }
      else if( source == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE2 )
      {
        GFL_ASSERT_STOP(0);
      }
    }
  }

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��  // �R���X�^���g�J���[0�łȂ��R���X�^���g�J���[5�ɂ���
  ///////////////////////////////////////////////////////
  // �}�e���A���J���[
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetReferenceCamera(shadow_camera_index);
    if( type == gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW )
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    }
    else if( type == gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP )
    {
      res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_SHADOW);
    }
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
    res_tex_coord.SetEnabled( true );  // �Z�b�g���Ȃ��Ă�OK����
  }

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
    
    // �e�N�X�`��
    res_tex_mapper.SetTexture(shadow_res_texture);
    
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    if( type == gfl::grp::g3d::ShadowGroup::TYPE_PROJECTION_SHADOW )
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
      
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
    else if( type == gfl::grp::g3d::ShadowGroup::TYPE_SHADOW_MAP )
    {
      res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_SHADOW );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

      //nw::gfx::ResShadowTextureSampler res_shadow_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResShadowTextureSampler>(res_tex_sampler);
    }
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i0�i�ځA1�i�ڂ�ݒ肷��
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(0);
    res_tex_combiner.SetConstant( nw::gfx::ResTextureCombiner::CONSTANT5 ); 
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0 ); 
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_CONSTANT );
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );
  }
  {
    if( receiver_type == RECEIVER_TYPE_ADD )  // @note RECEIVER_TYPE_ADD�������̊o�������H
    {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(1);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_REPLACE );
    }
    else
    {
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(1);
    res_tex_combiner.SetSourceRgb ( 0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 );
    res_tex_combiner.SetOperandRgb( 0, nw::gfx::ResTextureCombiner::OPERANDRGB_SRC_COLOR );
    res_tex_combiner.SetSourceRgb ( 1, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
    res_tex_combiner.SetOperandRgb( 1, nw::gfx::ResTextureCombiner::OPERANDRGB_ONE_MINUS_SRC_COLOR );
    res_tex_combiner.SetCombineRgb( nw::gfx::ResTextureCombiner::COMBINE_MODULATE );
    } 
  }

  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i2�i�ڂ�ύX����
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(2);
    for( s32 j=0; j<3; ++j )
    {
      if( res_tex_combiner.GetSourceRgb(j) == nw::gfx::ResTextureCombiner::SOURCE_TEXTURE1 )  // �e�N�X�`��1���O�i�̏o�͌���
      {
        res_tex_combiner.SetSourceRgb(j, nw::gfx::ResTextureCombiner::SOURCE_PREVIOUS);
      }
    }
  }

#if 0 
  //�e�X�g�L�q�A�R�����g�A�E�g�̂܂�
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i5�i�ڂ�ύX����
  ///////////////////////////////////////////////////////
  // �e�N�X�`���R���o�C�i
  { 
    nw::gfx::ResTextureCombiner res_tex_combiner = res_fragment_shader.GetTextureCombiners(5);
    res_tex_combiner.SetSourceRgb(0, nw::gfx::ResTextureCombiner::SOURCE_TEXTURE0);
  }
#endif

  ///////////////////////////////////////
  // �ύX�𔽉f����
  ///////////////////////////////////////
  res_material.SetTextureCoordinatorsHash(0x0);
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
  res_material.SetTextureCombinersHash(0x0);
  res_material.SetMaterialColorHash(0x0);
}


ShadowSystem::ReceiverType  ShadowSystem::CheckShadowReceiverMaterial(
    const nw::gfx::Material*                //material,
)
{
  // �e�N�X�`����2���ȉ������\���Ă��Ȃ����m�F(�v���V�[�W�����e�N�X�`������)

  // �e�N�X�`���R���o�C�i��4�i�ȉ������g���Ă��Ȃ����m�F(�@���ƒ��p�������瓊�e����ꍇ�ɓ����ɂ��鏈��������ꍇ��3�i�ȉ�)
  
  // �R���X�^���g�J���[��5�ȉ������g���Ă��Ȃ����m�F

  // �ݒ肪RECEIVER_TYPE_UNKNOWN�ȊO���m�F

  return RECEIVER_TYPE_UNKNOWN;
}

void ShadowSystem::SwitchOnShadowReceiverMaterial(
    nw::gfx::Material*                      ,//material,
    const nw::gfx::ResTexture               ,//shadow_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
    const gfl::grp::g3d::ShadowGroup::Type  ,//type,
    const ReceiverType                      //receiver_type
)
{
  // �e�N�X�`�������蓖�Ă�
}

void ShadowSystem::SwitchOffShadowReceiverMaterial(
    nw::gfx::Material*                      ,//material,
    const nw::gfx::ResTexture               ,//dummy_res_texture,  // �e�̂Ƃ���͔��łȂ��F(���Ƃ�)�A�e�łȂ��Ƃ���͔��ɂȂ��Ă���B
    const gfl::grp::g3d::ShadowGroup::Type  ,//type,
    const ReceiverType                      //receiver_type
)
{
  // �_�~�[�̃e�N�X�`�������蓖�Ă�ȊO�ɉ������@�͂Ȃ����H
}

void ShadowSystem::SetShadowReceiverMaterialShadowIntensity(
    nw::gfx::Material*                      material,
    const gfl::grp::g3d::ShadowGroup::Type  /*type*/,
    const f32                               shadow_intensity,  // 0�`1  // 1-( (1-res_shadow_texture)*shadow_intensity ) ���e�̐F�ɂȂ�B
                                                               // shadow_intensity=1�̂Ƃ�res_shadow_texture�̐F�̂܂܂̉e�ɂȂ�B
                                                               // shadow_intensity=0�̂Ƃ��e�͏o�Ȃ��B
    const ReceiverType                      /*receiver_type*/
)  // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B
{
  nw::gfx::ResMaterial res_material;

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��  // �R���X�^���g�J���[0�łȂ��R���X�^���g�J���[5�ɂ���
  ///////////////////////////////////////////////////////
  // �}�e���A���J���[
  res_material = material->GetMaterialColorResMaterial();
  {
    nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
    res_mat_color.SetConstant5(shadow_intensity, shadow_intensity, shadow_intensity, 1.0f);
  }

  ///////////////////////////////////////
  // �ύX�𔽉f����
  ///////////////////////////////////////
  res_material.SetMaterialColorHash(0x0);
}

//-----------------------------------------------------------------------------
/*
 *  @brief            �v���W�F�N�V�����V���h�E�p�̃}�e���A���ƃV�F�[�_�[���܂ރ��f���̃Z�b�g�A�b�v
 *
 *  @param[in]        shadow_setting_model    �Z�b�g�A�b�v���郂�f��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowSettingModel( nw::gfx::Model* shadow_setting_model )
{
/*
  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);

  // �e�̋��x�i�Z���j��ݒ肵�܂��B
  nw::gfx::ResMaterialColor materialColor = material->GetOriginal().GetMaterialColor();
  materialColor.SetDiffuse(0.0f, 0.0f, 0.0f);

  // �e�̐F��ݒ肵�܂��B
  // �R���X�^���g�J���[�ŉe�̐F��ݒ肷�邱�Ƃɂ����B
*/

  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  nw::gfx::ResMaterial res_material;
  res_material = material->GetMaterialColorResMaterial();
  nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
  res_mat_color.SetDiffuse(0.0f, 0.0f, 0.0f);
  res_material.SetMaterialColorHash(0x0);
}
//-----------------------------------------------------------------------------
/*
 *  @brief            �V���h�E�}�b�v�p�̃}�e���A���ƃV�F�[�_�[���܂ރ��f���̃Z�b�g�A�b�v
 *
 *  @param[in]        shadow_setting_model    �Z�b�g�A�b�v���郂�f��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupShadowMapSettingModel( nw::gfx::Model* shadow_setting_model )
{
/*
  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  
  // �V���h�E�p�̃}�e���A���ɃV���h�E���[�h��ݒ肵�܂��B
  // �V���h�E�}�b�v�ɃV���h�E���x�ƃf�v�X�����������ނ��߂ɕK�v�ƂȂ�܂��B
  nw::gfx::ResFragmentOperation fragmentOperation = material->GetOriginal().GetFragmentOperation();
  fragmentOperation.SetFragmentOperationMode(nw::gfx::ResFragmentOperation::FRAGMENT_OPERATION_MODE_SHADOW);

  // �V���h�E�I�u�W�F�N�g�̐F��ݒ肵�܂��B
  // �V���h�E�}�b�v�ł͕K����(0.0f, 0.0f, 0.0f ,0.0f) �ɐݒ肷��K�v������܂��B
  // �e�̐F�̐ݒ�͂̓R���o�C�i�ŃR���X�^���g�J���[�ɂ���čs���Ă��܂��B
  nw::gfx::ResMaterialColor materialColor = material->GetOriginal().GetMaterialColor();
  materialColor.SetDiffuse(0.0f, 0.0f, 0.0f, 0.0f);
*/

  nw::gfx::Material* material = shadow_setting_model->GetMaterial(0);
  nw::gfx::ResMaterial res_material;
  
  res_material = material->GetFragmentOperationResMaterial();
  nw::gfx::ResFragmentOperation res_fra_ope = res_material.GetFragmentOperation();
  res_fra_ope.SetFragmentOperationMode(nw::gfx::ResFragmentOperation::FRAGMENT_OPERATION_MODE_SHADOW);

  res_material = material->GetMaterialColorResMaterial();
  nw::gfx::ResMaterialColor res_mat_color = res_material.GetMaterialColor();
  res_mat_color.SetDiffuse(0.0f, 0.0f, 0.0f);

  res_material.SetFragmentOperationHash(0x0);
  res_material.SetMaterialColorHash(0x0);
}

/// gfx��
//----------------------------------------------------------------------------
/**
 *	@brief  �e���e�̐ݒ���s��  GFX��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource )
{
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, shadow_intensity.r, shadow_intensity.g, shadow_intensity.b, shadow_intensity.a);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(projection_camera_index);
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // �e�N�X�`��
  model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e���e�̐ݒ�̉���  GFX��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color )
{
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);
  
  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, 0.0f, 0.0f, 0.0f, 1.0f);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(0);  // @note 0�ԃJ�����Ȃ炫���Ƒ��݂��邾�낤����g���Ă��܂��đ��v���낤������Ȃ��ꍇ��GetActiveCamera�Ŏ擾�������̂��g���Ă��炦��B
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // �e�N�X�`��
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  {//KW13�C��
    gfl::grp::g3d::Texture   *p_texture = GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTexture(); 
    if ( p_texture )
      model->SetMaterialTextureMapperTexture(material_index, 0, p_texture);
  }

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_NEAREST_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

#if GFL_GRP_G3D_H3D_USE
/// H3d��
//----------------------------------------------------------------------------
/**
 *	@brief  �e���e�̐ݒ���s��  H3D��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionShadowModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, H3dResTexture* texture_resource )
{
  nw::ut::FloatColor border_color(1.0f, 1.0f, 1.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�𔒂ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͉e���`����Ȃ��B

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, shadow_intensity);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  
  // �e�N�X�`���R�[�f�B�l�[�^
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, projection_camera_index );
    model->SetMaterialTextureCoordinatorScaleUV( material_index, 0, gfl::math::VEC2(1.0f, 1.0f) );
    model->SetMaterialTextureCoordinatorRotateUV( material_index, 0, 0.0f );
    model->SetMaterialTextureCoordinatorTranslateUV( material_index, 0, gfl::math::VEC2(0.0f, 0.0f) );
  }

  // �e�N�X�`��
  model->SetMaterialTextureMapperTexture(material_index, 0, *texture_resource);

  // �e�N�X�`���}�b�p�[
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );// �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

    model->SetMaterialTextureMapperMinFilter( material_index, 0, H3dTextureSamplerExConst::MIN_FILTER_NEAREST_MIPMAP_LINEAR );
    model->SetMaterialTextureMapperMagFilter( material_index, 0, H3dTextureSamplerExConst::MAG_FILTER_LINEAR );
    

    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
    model->SetMaterialTextureMapperWrapU( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
    model->SetMaterialTextureMapperWrapV( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e���e�̐ݒ�̉���  H3D��
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionShadowModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color )
{
  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, gfl::grp::ColorF32(0.0f,0.0f,0.0f,1.0f));
  

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  
  // �e�N�X�`���R�[�f�B�l�[�^
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, 0 );
  }
  

  // �e�N�X�`��
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  model->SetMaterialTextureMapperTexture(material_index, 0, *GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataH3dSmallTexture());

  // �e�N�X�`���}�b�p�[
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );
    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
  }
}
#endif // GFL_GRP_G3D_H3D_USE


//----------------------------------------------------------------------------
/**
 *  @brief    ���\�[�X�ɂ���e�N�X�`�������̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�������C�g��receiver��On/Off����
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionLighModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource )
{
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  //model->SetMaterialColorConstant(material_index, 5, light_intensity, light_intensity, light_intensity, 1.0f);
  model->SetMaterialColorConstant(material_index, 5, light_intensity.r, light_intensity.g, light_intensity.b, light_intensity.a);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(projection_camera_index);
    res_tex_coord.SetScale( gfl::math::VEC2(1.0f, 1.0f) );
    res_tex_coord.SetRotate( 0.0f );
    res_tex_coord.SetTranslate( gfl::math::VEC2(0.0f, 0.0f) );
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  // �e�N�X�`��
  model->SetMaterialTextureMapperTexture(material_index, 0, texture_resource, index_in_resource);

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
    res_tex_sampler.SetMinFilter( nw::gfx::ResTextureSampler::MINFILTER_LINEAR_MIPMAP_LINEAR 	 );
    res_tex_sampler.SetMagFilter( nw::gfx::ResTextureSampler::MAGFILTER_LINEAR );
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
      res_standard_tex_sampler.SetWrapS( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
      res_standard_tex_sampler.SetWrapT( nw::gfx::ResStandardTextureSampler::WRAP_CLAMP_TO_BORDER );
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projection�̐ݒ�����ɖ߂��B
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionLightModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color )
{

  nw::gfx::Material* material = model->GetNwModel()->GetMaterial(material_index);

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, 0.0f, 0.0f, 0.0f, 1.0f);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  nw::gfx::ResMaterial res_material;
  
  // �e�N�X�`���R�[�f�B�l�[�^
  res_material = material->GetTextureCoordinatorResMaterial();
  {
    nw::gfx::ResTextureCoordinator res_tex_coord = res_material.GetTextureCoordinators(0);
    res_tex_coord.SetMappingMethod(nw::gfx::ResTextureCoordinator::MAPPINGMETHOD_PROJECTION);
    res_tex_coord.SetReferenceCamera(0);  // @note 0�ԃJ�����Ȃ炫���Ƒ��݂��邾�낤����g���Ă��܂��đ��v���낤������Ȃ��ꍇ��GetActiveCamera�Ŏ擾�������̂��g���Ă��炦��B
  }
  res_material.SetTextureCoordinatorsHash(0x0);

  {//KW14�C��
    // �e�N�X�`��
    //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
    gfl::grp::g3d::Texture    *pTexture = GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTexture();
    if ( pTexture )
      model->SetMaterialTextureMapperTexture(material_index, 0, pTexture);
  }

  // �e�N�X�`���}�b�p�[
  res_material = material->GetTextureMapperResMaterial();
  {
    nw::gfx::ResPixelBasedTextureMapper res_tex_mapper = res_material.GetTextureMappers(0);
   
    // �e�N�X�`���T���v���[
    nw::gfx::ResTextureSampler res_tex_sampler = res_tex_mapper.GetSampler();
    res_tex_sampler.SetSamplerType( nw::gfx::ResTextureSampler::SAMPLERTYPE_PROJECTION );  // �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B
    {
      nw::gfx::ResStandardTextureSampler res_standard_tex_sampler = nw::ut::ResDynamicCast<nw::gfx::ResStandardTextureSampler>(res_tex_sampler);
      res_standard_tex_sampler.SetBorderColor(border_color);
    }
  }
  res_material.SetTextureMappersHash(0x0);
  res_material.SetTextureSamplersHash(0x0);
}

#if GFL_GRP_G3D_H3D_USE
/// H3d��
//----------------------------------------------------------------------------
/**
 *  @brief    ���\�[�X�ɂ���e�N�X�`�������̃e�N�X�`���Ƃ��Ďg�����v���W�F�N�V�������C�g��receiver��On/Off����
 */
//-----------------------------------------------------------------------------
void ShadowSystem::SetupProjectionLighModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, H3dResTexture* texture_resource )
{
  nw::ut::FloatColor border_color(0.0f, 0.0f, 0.0f, 1.0f);  // �{�[�_�[�J���[�N�����v�Ńe�N�X�`����\���Ă���̂ŁA�{�[�_�[�J���[�����ɂ��Ă����B����Ńe�N�X�`���͈̔͊O�͌����`����Ȃ��B
  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, light_intensity);

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  
  // �e�N�X�`���R�[�f�B�l�[�^
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, projection_camera_index );
    model->SetMaterialTextureCoordinatorScaleUV( material_index, 0, gfl::math::VEC2(1.0f, 1.0f) );
    model->SetMaterialTextureCoordinatorRotateUV( material_index, 0, 0.0f );
    model->SetMaterialTextureCoordinatorTranslateUV( material_index, 0, gfl::math::VEC2(0.0f, 0.0f) );
  }

  // �e�N�X�`��
  model->SetMaterialTextureMapperTexture(material_index, 0, *texture_resource);

  // �e�N�X�`���}�b�p�[
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );// �e�N�X�`���T���v���[�^�C�v��ݒ肵�܂��B �e�N�X�`���T���v���[�^�C�v���ݒ�\�Ȃ̂͂O�Ԗڂ̃e�N�X�`���}�b�p�[�݂̂ł��B �P�A�Q�Ԗڂɐݒ肷��Ɛ��������삵�܂���B

    model->SetMaterialTextureMapperMinFilter( material_index, 0, H3dTextureSamplerExConst::MIN_FILTER_LINEAR_MIPMAP_LINEAR );
    model->SetMaterialTextureMapperMagFilter( material_index, 0, H3dTextureSamplerExConst::MAG_FILTER_LINEAR );
    

    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
    model->SetMaterialTextureMapperWrapU( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
    model->SetMaterialTextureMapperWrapV( material_index, 0, H3dTextureSamplerExConst::WRAP_CLAMP_TO_BORDER );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  Projection�̐ݒ�����ɖ߂��B
 */
//-----------------------------------------------------------------------------
void ShadowSystem::CleanUpProjectionLightModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color )
{

  ///////////////////////////////////////////////////////
  // �R���X�^���g�J���[5��ݒ肷��
  ///////////////////////////////////////////////////////
  model->SetMaterialColorConstant(material_index, 5, gfl::grp::ColorF32(0.0f,0.0f,0.0f,1.0f));
  

  ///////////////////////////////////////////////////////
  // �e�N�X�`��0��ݒ肷��
  ///////////////////////////////////////////////////////
  
  // �e�N�X�`���R�[�f�B�l�[�^
  {
    model->SetMaterialTextureCoordinatorMappingMethod( material_index, 0, H3dModel::TEXTURE_MAPPING_METHOD_PROJECTION );
    model->SetMaterialTextureCoordinatorReferenceSceneCameraIndex( material_index, 0, 0 );
  }
  

  // �e�N�X�`��
  //model->SetMaterialTextureMapperTexture(material_index, 0, GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataSmallTextureResource(), 0);
  model->SetMaterialTextureMapperTexture(material_index, 0, *GFL_SINGLETON_INSTANCE(gfl::grp::g3d::System)->GetCommonDataH3dSmallTexture());

  // �e�N�X�`���}�b�p�[
  {
    model->SetMaterialTextureMapperSamplerType( material_index, 0, H3dTextureSamplerExConst::SAMPLER_TYPE_PROJECTION );
    model->SetMaterialTextureMapperBorderColor( material_index, 0, border_color );
  }
}
#endif // GFL_GRP_G3D_H3D_USE



}  // namespace g3d
}  // namespace grp
}  // namespace gfl

