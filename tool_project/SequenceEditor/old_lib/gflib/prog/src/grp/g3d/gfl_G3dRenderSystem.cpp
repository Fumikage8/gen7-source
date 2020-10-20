//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderSystem.cpp
 *	@brief  3D�`��V�X�e��
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
#include <gfl_Std.h>
#include <gfl_Debug.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dRenderSystem.h>
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>
#include <grp/g3d/gfl_G3dSceneDrawGroup.h>
#include <grp/g3d/gfl_G3dLight.h>

// gflib grp g3d ����J�w�b�_
#include "gfl_G3dFakeRenderQueue.h"
#include "gfl_G3dRenderElementAccessor.h"




// ���̃}�N����L���ɂ���ƁA���O�o�͂Ƀ����_�[�L���[�̃\�[�g��̏�Ԃ����|�[�g���܂��B
//#define GFL_GRP_G3D_RENDER_SYSTEM_REPORT_RENDER_QUEUE_ENABLED

//���b�V���̕`�揇���m�F����ꍇ�Ɏg�p
//#define RENDER_MESH_DEBUG_PRINT

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�{�b�N�X�e�X�g�p�t�@�C�����O���[�o���ϐ�
 */
//-----------------------------------------------------------------------------
static bool RenderSystem_BoxTestFlag = false;            // �{�b�N�X�e�X�g�L���E�����i�f�t�H���g�����j
static s16 RenderSystem_BoxTestThrough = 0;         // �{�b�N�X�e�X�g�@�X���[��
static s16 RenderSystem_BoxTestCut = 0;             // �{�b�N�X�e�X�g�@�J�b�g��

static void RenderSystemClearBoxTestCount(void);
static void RenderSystemClearBoxTestCount(void)
{
  RenderSystem_BoxTestThrough = 0;
  RenderSystem_BoxTestCut = 0;
}


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 *  @brief        �{�b�N�X�e�X�g���s���B
 *
 *  @retval true    �`�悷��
 *  @retval false   �`�悵�Ȃ��B
 */
//-----------------------------------------------------------------------------
static bool IsBoxTest( const nw::ut::ResOrientedBoundingBox& bounding, const nw::gfx::Camera& camera, const nw::gfx::Model& model );



//-----------------------------------------------------------------------------
/**
 *					static�����o�֐�
 */
//-----------------------------------------------------------------------------
#if GFL_GRP_G3D_H3D_USE
#if 0
 //if�����������߂�Ȃ�A���L�̂悤�Ɋ֐��e�[�u����p�ӂ����݂�element_type�̕ύX�ɍ��킹�Ċ֐��e�[�u���ɓo�^����֐���ς���̂��x�X�g���ȁB

typedef void (*RenderSceneInsertFunction)(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);

  enum InsertFunctionPosition
  {
    INSERT_FUNCTION_POSITION_BEGIN,
    INSERT_FUNCTION_POSITION_END,
    INSERT_FUNCTION_POSITION_MAX
  };

  RenderSceneInsertFunction   insert_function[ELEMENT_TYPE_MAX][INSERT_FUNCTION_POSITION_MAX];

  param->insert_function[RenderSceneParam::ELEMENT_TYPE_NONE][INSERT_FUNCTION_POSITION_END]   = &;
  param->insert_function[RenderSceneParam::ELEMENT_TYPE_NONE][INSERT_FUNCTION_POSITION_BEGIN] = &;
  param->insert_function[RenderSceneParam::ELEMENT_TYPE_GFX][INSERT_FUNCTION_POSITION_END]    = &;
  param->insert_function[RenderSceneParam::ELEMENT_TYPE_GFX][INSERT_FUNCTION_POSITION_BEGIN]  = &;
  param->insert_function[RenderSceneParam::ELEMENT_TYPE_H3D][INSERT_FUNCTION_POSITION_END]    = &;
  param->insert_function[RenderSceneParam::ELEMENT_TYPE_H3D][INSERT_FUNCTION_POSITION_BEGIN]  = &;

static void NoChangeRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param);
static void NoChangeRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  // element_type��ύX���Ȃ����������Ȃ��B
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  �`��J�n�ݒ�
 */
//-----------------------------------------------------------------------------
void RenderSystem::BeginRenderScene( nw::gfx::RenderContext* render_context, RenderSceneParam* param )
{
  s32 active_camera_index = param->scene_draw_group->GetActiveCameraIndex();

  {
    // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
    render_context->ResetState();

    // �{�b�N�X�e�X�g�p�����[�^�̃N���A
    RenderSystemClearBoxTestCount();

    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B

    // GFSBTS[1805]: �r���[�|�[�g��`�Ɠ����̈�ŃV�U�[�e�X�g��ݒ�
    nw::gfx::GraphicsDevice::ActivateScissor(true, param->render_target->GetNwViewport().GetBound());

    // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
    // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
    // ���t���[���`��O�ɐݒ���s���K�v������܂��B
    render_context->SetActiveCamera(active_camera_index);
  }
}


void RenderSystem::BeginGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  {
    BeginRenderScene( render_context, param );

    param->scene_draw_group->BeginCommandInvokeCallback(render_context);

    render_context->SetCameraMatrix(param->nw_camera);

    if(param->command_cache_builder)
    {
      param->command_cache_builder->Begin();
    }
  }

  param->element_type = RenderSceneParam::ELEMENT_TYPE_GFX;
}

void RenderSystem::EndGfxRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  if(param->command_cache_builder)
  {
    param->command_cache_builder->End();
    param->command_cache_builder->Report();
  }

  param->scene_draw_group->EndCommandInvokeCallback(render_context);

  param->element_type = RenderSceneParam::ELEMENT_TYPE_NONE;
}

void RenderSystem::BeginH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  param->render_system->GetH3dCommand()->BegineDraw();
 
#if 0
  //uuuuuuuuuuuuuuuuuuuu����������
  s32 max_cameras;
  s32 max_light_sets;
  s32 max_fogs;
  s32 max_vertex_lights;
  const nw::gfx::SceneEnvironment& scene_environment = param->render_system->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  // ���C�g��t�H�O���A�j���[�V�������Ă��邩������Ȃ��̂Ŗ��t���[���ݒ肷��
  param->render_system->GetH3dEnvironment()->SetFromSceneEnvironment(scene_environment, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
#endif
  
  {//@fix GFBTS[1535] �C���^�[���[�u�z��ݒ菉�����i�z�[����ʂł͂��̐ݒ肪�������B3d�̓C���^�[���[�u�z��j
    H3dCommand* command = param->render_system->GetH3dCommand();
    static const u32 s_TagCommands[] ={ 
      0x00000000, PICA_CMD_HEADER( 0x206, 33-1, 0xF, 1 ),//0x206�̓��[�h�A���C���W�X�^ gx_MacroReg.h�ɒ�`�͂Ȃ������B
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
    gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
  }

  param->element_type = RenderSceneParam::ELEMENT_TYPE_H3D;
}

void RenderSystem::EndH3dRenderScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  param->render_system->GetH3dCommand()->EndDraw();
  
  param->element_type = RenderSceneParam::ELEMENT_TYPE_NONE;
}


void RenderSystem::BeginGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  s32                    active_camera_index = param->scene_draw_group->GetActiveCameraIndex();

  // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
  render_context->ResetState();

  // �{�b�N�X�e�X�g�p�����[�^�̃N���A
  RenderSystemClearBoxTestCount();

  {
    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
   
    // GFSBTS[1805]: �r���[�|�[�g��`�Ɠ����̈�ŃV�U�[�e�X�g��ݒ�
    nw::gfx::GraphicsDevice::ActivateScissor(true, param->render_target->GetNwViewport().GetBound());

    // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
    // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
    // ���t���[���`��O�ɐݒ���s���K�v������܂��B
    render_context->SetActiveCamera(active_camera_index);
    
    // �R�}���h�Z�[�u�J�n
    param->graphics_system->StartCommandSave();

    param->scene_draw_group->BeginCommandInvokeCallback(render_context);

    if(param->command_cache_builder)
    {
      param->command_cache_builder->Begin();
    }
  }

  param->element_type = RenderSceneParam::ELEMENT_TYPE_GFX;
}

void RenderSystem::EndGfxRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  if(param->command_cache_builder)
  {
    param->command_cache_builder->End();
    param->command_cache_builder->Report();
  }

  param->scene_draw_group->EndCommandInvokeCallback(render_context);

  endRenderStereoScene(render_context, mesh_renderer, param);

  param->element_type = RenderSceneParam::ELEMENT_TYPE_NONE;
}

void RenderSystem::BeginH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  // �R�}���h�Z�[�u�J�n
  param->graphics_system->StartCommandSave();

  param->render_system->GetH3dCommand()->BegineDraw();
  
#if 0
  //uuuuuuuuuuuuuuuuuuuu����������
  s32 max_cameras;
  s32 max_light_sets;
  s32 max_fogs;
  s32 max_vertex_lights;
  const nw::gfx::SceneEnvironment& scene_environment = param->render_system->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  // ���C�g��t�H�O���A�j���[�V�������Ă��邩������Ȃ��̂Ŗ��t���[���ݒ肷��
  param->render_system->GetH3dEnvironment()->SetFromSceneEnvironment(scene_environment, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
#endif
  
  {//@fix GFBTS[1535] �C���^�[���[�u�z��ݒ菉�����i�z�[����ʂł͂��̐ݒ肪�������BH3d�̓C���^�[���[�u�z��j
    H3dCommand* command = param->render_system->GetH3dCommand();
    static const u32 s_TagCommands[] ={ 
      0x00000000, PICA_CMD_HEADER( 0x206, 33-1, 0xF, 1 ),//0x206�̓��[�h�A���C���W�X�^ gx_MacroReg.h�ɒ�`�͂Ȃ������B
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000,
      0x00000000, 0x00000000, 0x00000000, 0x00000000
    };
    gfl::grp::util::DrawUtil::Debug::AddCommand( command->GetNwDrawCommandStream(), s_TagCommands, sizeof(s_TagCommands) );
  }

  param->element_type = RenderSceneParam::ELEMENT_TYPE_H3D;
}

void RenderSystem::EndH3dRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  param->render_system->GetH3dCommand()->EndDraw();

  endRenderStereoScene(render_context, mesh_renderer, param);

  param->element_type = RenderSceneParam::ELEMENT_TYPE_NONE;
}

void RenderSystem::endRenderStereoScene(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer, RenderSceneParam* param)
{
  // �R�}���h�Z�[�u�I��
  param->graphics_system->EndCommandSave();
 
  // ���ڗp
  if( param->graphics_system->BindCurrentCommandList( param->render_target ) )
  {
    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
    render_context->SetCameraMatrix(param->nw_camera);
    // �R�}���h�Đ�
    param->graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
 
  // �E�ڗp
  if( param->graphics_system->BindCurrentCommandList( param->render_target_extension ) )
  {
    render_context->SetRenderTarget(param->render_target_extension->GetNwRenderTarget(), param->render_target_extension->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
    render_context->SetCameraMatrix(param->nw_camera_extension);
    // �R�}���h�Đ�
    param->graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
}
#endif  // GFL_GRP_G3D_H3D_USE

struct RenderSceneInternalFunctor : public gfl::std::UnaryFunction<nw::gfx::RenderElement&, void>
{
  nw::gfx::RenderContext* m_render_context;
  nw::gfx::MeshRenderer*  m_mesh_renderer;
#if GFL_GRP_G3D_H3D_USE
  RenderSystem::RenderSceneParam* m_param;  // ���̂Ƃ���̂��̂��o���Ă�������
#endif  // GFL_GRP_G3D_H3D_USE

  RenderSceneInternalFunctor(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer
#if GFL_GRP_G3D_H3D_USE
      , RenderSystem::RenderSceneParam* param=NULL//bbbbbbbb
#endif  // GFL_GRP_G3D_H3D_USE
  )
      : m_render_context(render_context),
        m_mesh_renderer(mesh_renderer)
#if GFL_GRP_G3D_H3D_USE
        , m_param(param)
#endif  // GFL_GRP_G3D_H3D_USE
  {
    GFL_ASSERT_STOP(render_context);
    GFL_ASSERT_STOP(mesh_renderer);
#if GFL_GRP_G3D_H3D_USE
    //GFL_ASSERT_STOP(param);//bbbbbbbb
#endif  // GFL_GRP_G3D_H3D_USE
    
    H3dModel::ResetBeforeMaterialSetting();
  }

  void operator()(nw::gfx::RenderElement& element)
  {
    if (element.IsCommand())
    {
#if GFL_GRP_G3D_H3D_USE
      if( m_param )//bbbbbbbb
      {
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
            RenderSystem::EndH3dRenderScene(m_render_context, m_mesh_renderer, m_param);
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
            RenderSystem::BeginGfxRenderScene(m_render_context, m_mesh_renderer, m_param);
      }
#endif  // GFL_GRP_G3D_H3D_USE

      nw::gfx::RenderCommand* command = element.GetCommand();
      GFL_ASSERT_STOP(command);
      command->Invoke(m_render_context);
      
      H3dModel::ResetBeforeMaterialSetting();
    }
#if GFL_GRP_G3D_H3D_USE
    else if( RenderElementAccessor::IsH3dRenderElement(element) )
    {
      //GFL_PRINT("AAAAaaaa korega nakunaruto GPU HW STATE 0x100\n");

      if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_GFX )
          RenderSystem::EndGfxRenderScene(m_render_context, m_mesh_renderer, m_param);
      if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
          RenderSystem::BeginH3dRenderScene(m_render_context, m_mesh_renderer, m_param);

      nw::gfx::ResMesh mesh  = element.GetMesh();
      nw::gfx::Model*  model = element.GetModel();
 
      MeshTypeChanger mesh_type_changer;
      mesh_type_changer.mesh_ptr = mesh.ptr();

      ModelTypeChanger model_type_changer;
      model_type_changer.model = model;

      nw::h3d::res::MeshData* h3d_mesh  = mesh_type_changer.h3d_mesh;
      H3dModel*               h3d_model = model_type_changer.h3d_model;

      h3d_model->DrawMesh(m_param->render_system->GetH3dCommand(), m_param->render_system->GetH3dEnvironment(), *h3d_mesh, false
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
          ,
          m_param->view_mtx,
          m_param->inv_view_mtx,
          m_param->inv_exist
#endif 
      );
    }
#endif  // GFL_GRP_G3D_H3D_USE
    else
    {
#if GFL_GRP_G3D_H3D_USE
      if( m_param )//bbbbbbbb
      {
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
            RenderSystem::EndH3dRenderScene(m_render_context, m_mesh_renderer, m_param);
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
            RenderSystem::BeginGfxRenderScene(m_render_context, m_mesh_renderer, m_param);
      }
#endif  // GFL_GRP_G3D_H3D_USE
      
      nw::gfx::ResMesh mesh  = element.GetMesh();
      nw::gfx::Model*  model = element.GetModel();

      nw::gfx::res::ResShape shape = model->GetResModel().GetShapes(mesh.GetShapeIndex());
      nw::ut::ResOrientedBoundingBox boundingbox = shape.GetOrientedBoundingBox();

      {
        model->PreRenderSignal()(model, mesh, m_render_context);
        m_mesh_renderer->RenderMesh(mesh, model);
        model->PostRenderSignal()(model, mesh, m_render_context);

        H3dModel::ResetBeforeMaterialSetting();
      }
    }
    GFL_GL_ASSERT();
  }
};

#if GFL_GRP_G3D_H3D_USE
struct RenderStereoSceneInternalFunctor : public gfl::std::UnaryFunction<nw::gfx::RenderElement&, void>
{
  nw::gfx::RenderContext* m_render_context;
  nw::gfx::MeshRenderer*  m_mesh_renderer;
  RenderSystem::RenderSceneParam* m_param;  // ���̂Ƃ���̂��̂��o���Ă�������

  RenderStereoSceneInternalFunctor(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer
      , RenderSystem::RenderSceneParam* param=NULL//bbbbbbbb
  )
      : m_render_context(render_context),
        m_mesh_renderer(mesh_renderer)
        , m_param(param)
  {
    GFL_ASSERT_STOP(render_context);
    GFL_ASSERT_STOP(mesh_renderer);
    H3dModel::ResetBeforeMaterialSetting();
  }

  void operator()(nw::gfx::RenderElement& element)
  {
    if (element.IsCommand())
    {
      if( m_param )//bbbbbbbb
      {
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
            RenderSystem::EndH3dRenderStereoScene(m_render_context, m_mesh_renderer, m_param);
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
            RenderSystem::BeginGfxRenderStereoScene(m_render_context, m_mesh_renderer, m_param);
      }

      nw::gfx::RenderCommand* command = element.GetCommand();
      GFL_ASSERT_STOP(command);
      command->Invoke(m_render_context);
      
      H3dModel::ResetBeforeMaterialSetting();
    }
    else if( RenderElementAccessor::IsH3dRenderElement(element) )
    {
      //GFL_PRINT("AAAAaaaa korega nakunaruto GPU HW STATE 0x100\n");

      if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_GFX )
          RenderSystem::EndGfxRenderStereoScene(m_render_context, m_mesh_renderer, m_param);
      if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
          RenderSystem::BeginH3dRenderStereoScene(m_render_context, m_mesh_renderer, m_param);

      nw::gfx::ResMesh mesh  = element.GetMesh();
      nw::gfx::Model*  model = element.GetModel();
 
      MeshTypeChanger mesh_type_changer;
      mesh_type_changer.mesh_ptr = mesh.ptr();

      ModelTypeChanger model_type_changer;
      model_type_changer.model = model;

      nw::h3d::res::MeshData* h3d_mesh  = mesh_type_changer.h3d_mesh;
      H3dModel*               h3d_model = model_type_changer.h3d_model;

      h3d_model->DrawMesh(m_param->render_system->GetH3dCommand(), m_param->render_system->GetH3dEnvironment(), *h3d_mesh, false
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
          ,
          m_param->view_mtx,
          m_param->inv_view_mtx,
          m_param->inv_exist
#endif 
      );

      m_param->render_system->AddRenderMeshCount();
    }
    else
    {
      if( m_param )//bbbbbbbb
      {
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
            RenderSystem::EndH3dRenderStereoScene(m_render_context, m_mesh_renderer, m_param);
        if( m_param->element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_NONE )
            RenderSystem::BeginGfxRenderStereoScene(m_render_context, m_mesh_renderer, m_param);
      }
      
      nw::gfx::ResMesh mesh  = element.GetMesh();
      nw::gfx::Model*  model = element.GetModel();

      {
        model->PreRenderSignal()(model, mesh, m_render_context);
        m_mesh_renderer->RenderMesh(mesh, model);
        model->PostRenderSignal()(model, mesh, m_render_context);
      }

      if( m_param )//KW230�C��
        m_param->render_system->AddRenderMeshCount();
      
      H3dModel::ResetBeforeMaterialSetting();
    }
    GFL_GL_ASSERT();
  }
};
#endif  // GFL_GRP_G3D_H3D_USE

#if GFL_GRP_G3D_H3D_USE
struct PrintFunc : public gfl::std::UnaryFunction<nw::gfx::RenderElement&, void>
{
  nw::gfx::RenderContext* m_render_context;
  nw::gfx::MeshRenderer*  m_mesh_renderer;
  RenderSystem::RenderSceneParam* m_param;  // ���̂Ƃ���̂��̂��o���Ă�������

  PrintFunc(nw::gfx::RenderContext* render_context, nw::gfx::MeshRenderer* mesh_renderer
      , RenderSystem::RenderSceneParam* param=NULL//bbbbbbbb
  )
      : m_render_context(render_context),
        m_mesh_renderer(mesh_renderer)
        , m_param(param)
  {
    GFL_ASSERT_STOP(render_context);
    GFL_ASSERT_STOP(mesh_renderer);
    //GFL_ASSERT_STOP(param);//bbbbbbbb
  }
  
  void operator()(nw::gfx::RenderElement& element)
  {
    int     i = 0;
    
    if( RenderElementAccessor::IsH3dRenderElement(element) )
    {
      nw::gfx::ResMesh mesh  = element.GetMesh();
      nw::gfx::Model*  model = element.GetModel();
 
      MeshTypeChanger mesh_type_changer;
      mesh_type_changer.mesh_ptr = mesh.ptr();

      ModelTypeChanger model_type_changer;
      model_type_changer.model = model;

      nw::h3d::res::MeshData* h3d_mesh  = mesh_type_changer.h3d_mesh;
      H3dModel*               h3d_model = model_type_changer.h3d_model;
      
      const nw::h3d::res::ModelContent      *pModelContent = h3d_model->GetNwModelContent();
      const nw::h3d::res::MaterialContent   *pMaterialContent = pModelContent->materials[h3d_mesh->materialId].content;
      
      ISHIGURO_PRINT("%s\t\t%lld\t\t%d\t%d\t(%f, %f, %f)\n", h3d_model->GetMaterialName( h3d_mesh->materialId ), element.Key(), h3d_mesh->key, pMaterialContent->uid, h3d_mesh->center.x, h3d_mesh->center.y, h3d_mesh->center.z );
    }
    else
    {
      nw::gfx::ResMesh        mesh  = element.GetMesh();
      nw::gfx::Model          *model = element.GetModel();
      const s32               index = mesh.GetMaterialIndex();
      nw::gfx::Material       *target_material = model->GetMaterial( index );
      nw::gfx::res::ResShape  shape = model->GetResModel().GetShapes( mesh.GetShapeIndex() );
      nw::math::VEC3          pos = shape.GetCenterPosition();
      
      ISHIGURO_PRINT("%s\t\t%lld\t\tx\t(%f, %f, %f)\n", target_material->GetName(), element.Key(), pos.x, pos.y, pos.z );
    }
  }
};

#if GFL_DEBUG    
struct DrawFrameSetFunc : public gfl::std::UnaryFunction<nw::gfx::RenderElement&, void>
{
  DrawFrameSetFunc(){}
  
  void operator()(nw::gfx::RenderElement& element)
  {
    if (element.IsCommand())
    {
      
    }
    else if( RenderElementAccessor::IsH3dRenderElement(element) )
    {
      nw::gfx::Model*  model = element.GetModel();

      ModelTypeChanger model_type_changer;
      model_type_changer.model = model;

      H3dModel*   h3d_model = model_type_changer.h3d_model;
      u32         cnt = 0;

      //��������GFL_DEBUG��#if GFL_DEBUG    
        if ( H3dModel::IsSafeDeleteCheckEnable() )
          cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();
      //��������GFL_DEBUG��#endif

      h3d_model->SetDrawFrameCount( cnt );
    }
    else
    {
      nw::gfx::Model*  model = element.GetModel();
      nw::gfx::ResMesh mesh  = element.GetMesh();
      
      nw::gfx::ResShape shape = model->GetResModel().GetShapes(mesh.GetShapeIndex());

      switch ( shape.ref().typeInfo ){
      case nw::gfx::ResSeparateDataShape::TYPE_INFO:
        {
          // nw::gfx::ResShape::TYPE_INFO �́A�Ȃ������B���ׂ����f���͑S��nw::gfx::ResSeparateDataShape::TYPE_INFO�������B
          if ( Model::IsSafeDeleteCheckEnable() )
          {
            u32         cnt = 0;
            u32                     parentAddr = model->GetUserParameter<u32>();
            if ( parentAddr )
            {
              gfl::grp::g3d::Model *pG3dModel = reinterpret_cast<gfl::grp::g3d::Model*>( parentAddr );

              cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();
              pG3dModel->SetDrawFrameCount( cnt );
            }
          }
        }
        break;
      case nw::gfx::ResParticleShape::TYPE_INFO:
        {
//��������GFL_DEBUG��#if GFL_DEBUG
          if ( Particle::IsSafeDeleteCheckEnable() )
          {
            u32         cnt = 0;

            nw::gfx::ParticleModel* particleModel = static_cast<nw::gfx::ParticleModel*>( model );
            u32                     parentAddr = particleModel->GetUserParameter<u32>();

            if ( parentAddr )
            {
              gfl::grp::g3d::Particle *pParticle = reinterpret_cast<gfl::grp::g3d::Particle*>( parentAddr );

              cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();
              pParticle->SetDrawFrameCount( cnt );
            }
          }
//��������GFL_DEBUG��#endif
        }
        break;
      default:
        break;
      }

    }
  }
};
#endif
#endif  // GFL_GRP_G3D_H3D_USE
//-----------------------------------------------------------------------------
/**
 *    �{�b�N�X�e�X�g�̐ݒ�,�p�����[�^�擾
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �{�b�N�X�e�X�g�ݒ�
 *
 *	@param	flag  �t���O
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetBoxTest( bool flag )
{
  RenderSystem_BoxTestFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�b�N�X�e�X�g�L���`�F�b�N
 *
 *	@retval true    �{�b�N�X�e�X�g�L��
 *	@retval false   �{�b�N�X�e�X�g����
 */
//-----------------------------------------------------------------------------
bool RenderSystem::IsBoxTest( void )
{
  return RenderSystem_BoxTestFlag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�b�N�X�e�X�g�@�J�b�gShape���擾
 *
 *	@return �{�b�N�X�e�X�g�ɂ��A�J�b�g���ꂽShape��
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetBoxTestCutCount( void )
{
  return RenderSystem_BoxTestCut;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�b�N�X�e�X�g�@�X���[Shape���擾
 *
 *	@return �{�b�N�X�e�X�g��ʂ蔲����Shape��
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetBoxTestThroughCount( void )
{
  return RenderSystem_BoxTestThrough;
}


//-----------------------------------------------------------------------------
/**
 *					�N���X��`
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�`��V�X�e���N���X
//=====================================
//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *
 *  @param[in]     allocator  �A���P�[�^
 */
//-----------------------------------------------------------------------------
RenderSystem::RenderSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, Description* description)
  : m_render_context                     (NULL),
    m_max_cameras                        (0),
    m_prior_material_render_key_factory  (NULL),
    m_prior_depth_render_key_factory     (NULL),
    m_mesh_renderer                      (NULL),
    m_model_user_parameter_check_functor (NULL),
    m_render_sort_mode                   (nw::gfx::ISceneUpdater::ALL_MESH_BASE_SORT),
    m_depth_sort_mode                    (nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)
#if GFL_GRP_G3D_H3D_USE
    , m_h3d_command(NULL),
    m_h3d_environment(NULL)
#endif  // GFL_GRP_G3D_H3D_USE
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_max_cameras = l_description.max_cameras;
  m_max_cameras_plus_system_camera = m_max_cameras + SYSTEM_CAMERA_MAX;

  m_max_light_sets    = l_description.max_light_sets;
  m_max_fogs          = l_description.max_fogs;
  m_max_vertex_lights = l_description.max_vertex_lights;

  m_render_context = nw::gfx::RenderContext::Builder()
      .MaxCameras(m_max_cameras_plus_system_camera)
      .MaxLights(m_max_light_sets)
      .MaxFogs(m_max_fogs)
      .MaxVertexLights(m_max_vertex_lights)
      .Create(device_allocator);

  if( m_depth_sort_mode == nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH )
  {
    // �������ȊO�̃��b�V���Ő[�x���̗ʎq�����s��Ȃ����邽�߂�
    // �����_�[�L�[�E�t�@�N�g���N���X�𐶐����܂��B
    m_prior_material_render_key_factory =
        nw::gfx::CreatePriorMaterialAndZeroDepthRenderKeyFactory(device_allocator);
  }
  else
  {
    m_prior_material_render_key_factory =
        nw::gfx::CreatePriorMaterialRenderKeyFactory(device_allocator);
  }

  m_prior_depth_render_key_factory =
      nw::gfx::CreatePriorDepthReverseDepthRenderKeyFactory(device_allocator);

  m_mesh_renderer = nw::gfx::MeshRenderer::Create(device_allocator);
  m_mesh_renderer->SetRenderContext(m_render_context);

  m_model_user_parameter_check_functor = GFL_NEW(heap_allocator->GetHeapBase()) ModelUserParameterCheckFunctor();

  GFL_GL_ASSERT();

#if GFL_GRP_G3D_H3D_USE
  createH3dSystem(heap_allocator, device_allocator);
#endif  // GFL_GRP_G3D_H3D_USE

#if GFL_GRP_G3D_H3D_USE
  STATIC_ASSERT( sizeof(nw::gfx::ResMesh)==sizeof(nw::h3d::res::MeshData*) );  // MeshTypeChanger
  //STATIC_ASSERT( sizeof(void*)==sizeof(nw::h3d::res::MeshData*) );  // MeshTypeChanger
    // �T�C�Y���R���p�C�����Ƀ`�F�b�N��nw::gfx::ResMesh�̕ύX�ɋC�t����悤�ɂ��Ă����B
#endif  // GFL_GRP_G3D_H3D_USE

}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
RenderSystem::~RenderSystem()
{
#if GFL_GRP_G3D_H3D_USE
  destroyH3dSystem();
#endif  // GFL_GRP_G3D_H3D_USE

  if( m_model_user_parameter_check_functor )
  {
    GFL_DELETE m_model_user_parameter_check_functor;
    m_model_user_parameter_check_functor = NULL;
  }
  nw::gfx::SafeDestroy(m_mesh_renderer);
  nw::gfx::SafeDestroy(m_prior_depth_render_key_factory);
  nw::gfx::SafeDestroy(m_prior_material_render_key_factory);
  nw::gfx::SafeDestroy(m_render_context);
}

void RenderSystem::InitializeSceneDrawGroup(SceneDrawGroup* scene_draw_group)
{
  nw::gfx::RenderQueue* render_queue = scene_draw_group->GetRenderQueue();

  render_queue->Reset(
      m_prior_material_render_key_factory,
      m_prior_depth_render_key_factory,
      m_prior_material_render_key_factory,
      m_prior_material_render_key_factory,
      ((m_depth_sort_mode==nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)?true:false) );
}
void RenderSystem::BeginSubmitView(SceneDrawGroup* scene_draw_group)
{
  nw::gfx::RenderQueue* render_queue = scene_draw_group->GetRenderQueue();

  // RenderQueue::Reset �̈����� true ���w�肷��ƁA�\�[�g�p�L�[���L���b�V������œK���@�\��
  // �L���ɂȂ�܂��B
  // �������ȊO�̃\�[�g�p�L�[�̂݃L���b�V�������悤�ɂȂ�܂��B
  render_queue->Reset( ((m_depth_sort_mode==nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)?true:false) );
}

void RenderSystem::EndSubmitView(SceneDrawGroup* scene_draw_group)
{
  nw::gfx::RenderQueue* render_queue = scene_draw_group->GetRenderQueue();

  //----------------------------------------
  // �����_�[�L���[���\�[�g���܂��B
  // RenderElementCompare() �⃌���_�[�L�[�t�@�N�g���[���J�X�^�}�C�Y���邱�Ƃ�
  // �`�揇��ύX���邱�Ƃ��ł��܂��B
  // �ڂ����͍œK��TIPS���Q�Ƃ��Ă��������B
  gfl::std::Sort(
      render_queue->Begin(),
      render_queue->End(),
      nw::gfx::RenderElementCompare());
  
  {
#ifdef GFL_GRP_G3D_RENDER_SYSTEM_REPORT_RENDER_QUEUE_ENABLED
  // �����_�[�L���[�̃\�[�g��̏�Ԃ����|�[�g���܂��B
  // �����[�X�łł͂Ȃɂ��\������Ȃ��Ȃ��Ă��܂��B
  gfl::std::ForEach(
      render_queue->Begin(),
      render_queue->End(),
      nw::gfx::RenderQueue::ReportFunctor());
#endif
  }

  // SubmitView�ŁA���b�V���J�E���g���N���A
  this->ClearRenderMeshCount();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ݒ肩��V�[������K�p����
 *
 *  @param[in]     scene_environment_setting  �K�p����V�[����
 *                                            (nw::gfx::SceneEnvironmentSetting::ResolveReference���ς�ł��邱��)
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetSceneEnvironmentSetting(const nw::gfx::SceneEnvironmentSetting& scene_environment_setting)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();

  {
    // �J�����̃C���f�b�N�X��m_max_cameras�ȏ�̒l���g���Ă��Ȃ����m�F����B
    // m_max_cameras�ȏ�̓V�X�e���Ŏg���̂ŁA���[�U�͎g���Ă͂Ȃ�Ȃ��B
    nw::gfx::SceneEnvironmentSetting::CameraBinderArray::const_iterator camera_end = scene_environment_setting.GetCameraEnd();
    for( nw::gfx::SceneEnvironmentSetting::CameraBinderArray::const_iterator camera_i = scene_environment_setting.GetCameraBegin();
         camera_i != camera_end;
         ++camera_i )
    {
      s32 camera_index = (*camera_i).index;
      if( 0<=camera_index && camera_index<m_max_cameras )
      {
        // OK
      }
      else
      {
        GFL_ASSERT_MSG(0, "camera_index=%d��%d<= <%d�Ɏ��܂��Ă��܂���B\n", camera_index, 0, m_max_cameras);
      }
    }
  }

  scene_environment.ApplyFrom(scene_environment_setting);
  m_fragmentAndWBufferDirty = true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ɃJ������ݒ肷��
 *
 *  @param[in]     camera_index   �J�����̃C���f�b�N�X
 *  @param[in]     camera         �J����(camera_index�ɐݒ肷��)
 *
 *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����camera_index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetCamera(s32 camera_index, nw::gfx::Camera* camera)
{
  if( 0<=camera_index && camera_index<m_max_cameras )
  {
    nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
    scene_environment.SetCamera(camera_index, camera);
  }
  else
  {
    GFL_ASSERT_MSG(0, "camera_index=%d��%d<= <%d�Ɏ��܂��Ă��܂���B\n", camera_index, 0, m_max_cameras);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����̃J�����z�񂩂�J�������擾����
 *
 *  @param[in]     camera_index   �J�����̃C���f�b�N�X
 *
 *  @retval        �z��̂��̃C���f�b�N�X�ɓn���Ă������J�����̃|�C���^�𓾂�B
 *                 NULL�̂Ƃ����̃C���f�b�N�X�ɂ̓J������n���Ă��Ȃ������B
 */
//-----------------------------------------------------------------------------
nw::gfx::Camera* RenderSystem::GetCamera(const s32 camera_index) const
{
  if( 0<=camera_index && camera_index<m_max_cameras )
  {
    nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
    return scene_environment.GetCamera(camera_index);
  }
  else
  {
    GFL_ASSERT_MSG(0, "camera_index=%d��%d<= <%d�Ɏ��܂��Ă��܂���B\n", camera_index, 0, m_max_cameras);
    return NULL;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����̃J�����z�񂩂�J�����̃C���f�b�N�X���擾����
 *
 *  @param[in]     camera   �J����
 *
 *  @retval        �z��ɂ��̃J����������ꍇ���̃C���f�b�N�X�𓾂�B
 *                 �Ȃ��ꍇCAMERA_INDEX_NONE��Ԃ��B
 */
//-----------------------------------------------------------------------------
s32              RenderSystem::GetCameraIndex(const nw::gfx::Camera* camera) const
{
  for(s32 i=0; i<m_max_cameras; ++i)
  {
    if( this->GetCamera(i) == camera )
    {
      return i;
    }
  }
  return CAMERA_INDEX_NONE;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����̃J�����z�񂩂��ԍŏ��ɂ���J�����̃C���f�b�N�X���擾����
 *
 *  @retval        �z��Ɉ���J�������Ȃ��ꍇCAMERA_INDEX_NONE��Ԃ��B
 */
//-----------------------------------------------------------------------------
s32              RenderSystem::GetFirstCameraIndex(void) const
{
  for(s32 i=0; i<m_max_cameras; ++i)
  {
    if( this->GetCamera(i) )
    {
      return i;
    }
  }
  return CAMERA_INDEX_NONE;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ɃJ�������J�����z��ɐݒ肵�Ă��Ȃ��J������ݒ肷��
 *
 *  @param[in]     camera         �J����(GetNotSetCameraIndex�ɐݒ肷��)
 *
 *  �ucamera=NULL�v��n����GetNotSetCameraIndex�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetNotSetCamera(nw::gfx::Camera* camera)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
  scene_environment.SetCamera(this->GetNotSetCameraIndex(), camera);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �J�������J�����z��ɐݒ肵�Ă��Ȃ��Ƃ��Ɏg���J�����̃C���f�b�N�X���擾����
 *
 *  @retval        m_max_cameras+SYSTEM_CAMERA_NOT_SET��Ԃ��B
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetNotSetCameraIndex(void) const
{
  return (m_max_cameras + SYSTEM_CAMERA_NOT_SET);
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����Ƀ��C�g�Z�b�g��ݒ肷��
 *
 *  @param[in]     light_set_index   �ݒ��ƂȂ郉�C�g�Z�b�g�̃C���f�b�N�X 
 *  @param[in]     light_set         ���C�g�Z�b�g�B
 *                                   �Ăяo�����͂��̃��C�g�Z�b�g��ێ��������ĉ������B
 *                                   light_set_index�ɐݒ肵�Ă��郉�C�g�Z�b�g���O���ꍇ��NULL��n���ĉ������B
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetLightSet(s32 light_set_index, nw::gfx::LightSet* light_set)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
  scene_environment.SetLightSet(light_set_index, light_set);
  m_fragmentAndWBufferDirty = true;
 
#if 0
  /*����
  �����ݒ肵�Ă��Ȃ��Ƃ���
  nw::gfx::LightSet* light_set = scene_environment.GetLightSet(light_set_index);
  �œ�����light_set��NULL�ɂȂ��Ă���B
  scene_environment.ApplyFrom(scene_environment_setting)�ɂ����
  scene_environment_setting���ێ����Ă���LightSet�̃|�C���^��scene_environment�ɐݒ肳���B
  ������scene_environment_setting�́AApplyFrom���I����Ă�����j�����Ă͂Ȃ�Ȃ��B*/
#endif
}

#if GFL_GRP_G3D_H3D_USE
//-----------------------------------------------------------------------------
/*
 *  @brief       h3d����gfx���������
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetH3dEnvironmentFromSceneEnvironment(const nw::gfx::Camera& camera)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
  m_h3d_environment->SetFromSceneEnvironment(scene_environment, camera, m_max_cameras_plus_system_camera, m_max_light_sets, m_max_fogs, m_max_vertex_lights);
}
#endif  // GFL_GRP_G3D_H3D_USE

//-----------------------------------------------------------------------------
/*
 *  @brief        �����_�[�L���[�Ƀ��f��(nw::gfx��Model,SkeletalModel,ParticleModel)��ǉ�
 *
 *  @param[in]    camera         �����_�[�L���[�����Ɏg�p����J����
 *  @param[in]    scene_system   �`�悷��V�[���R���e���c���܂ރV�[���V�X�e��
 */
//-----------------------------------------------------------------------------
void RenderSystem::SubmitViewModel(
    nw::gfx::Camera*       camera,
    SceneSystem*           scene_system,
    nw::gfx::Model*        model,
    SceneDrawGroup*        scene_draw_group )
{
  nw::gfx::ISceneUpdater* scene_updater      = scene_system->GetSceneUpdater();
  nw::gfx::RenderQueue*   render_queue       = scene_draw_group->GetRenderQueue();
  s32                     render_queue_index = scene_draw_group->GetRenderQueueIndex();

  m_model_user_parameter_check_functor->SetRenderQueueIndex(render_queue_index);

  //----------------------------------------
  // �J�����Ɉˑ����鏈���ƁA�`��v�f�������_�[�L���[�ɐς݂܂��B

  // SORT_DEPTH_OF_TRANSLUCENT_MESH�̂Ƃ��A
  // ���������b�V���̐[�x�̂݌v�Z���āA���������b�V���̂ݐ[�x�ɂ��\�[�g�������悤�ɂȂ�܂��B
  // �[�x�v�Z�̏������ׂ̒ጸ�����҂ł��܂��B

  // SORT_DEPTH_OF_ALL_MESH�̂Ƃ��A
  // ���ׂẴ��b�V���̐[�x���v�Z���āA�[�x�ɂ���Ă��\�[�g�����悤�ɂȂ�܂��B
  // �s�������̓�����O����`�悷�邱�ƂŁAGPU�̃t�B�����ׂ̒ጸ�����҂ł��܂��B

  scene_updater->SetDepthSortMode(m_depth_sort_mode);

  SceneUpdateHelperSubmitViewModel(
      scene_system->GetSkeletonUpdater(),
      scene_system->GetBillboardUpdater(),
      render_queue,
      model,
      *camera,
      0,
      1,
      m_render_sort_mode,
      scene_system->GetDepthSortMode(),
      m_model_user_parameter_check_functor );
}

#if GFL_GRP_G3D_H3D_USE
//-----------------------------------------------------------------------------
/*
 *  @brief        �����_�[�L���[��H3D���f����ǉ�
 *
 *  @param[in]    camera         �����_�[�L���[�����Ɏg�p����J����
 *  @param[in]    scene_system   �`�悷��V�[���R���e���c���܂ރV�[���V�X�e��
 */
//-----------------------------------------------------------------------------
void RenderSystem::SubmitViewH3dModel(
    nw::gfx::Camera*       camera,
    SceneSystem*           scene_system,
    H3dModel*              model,
    SceneDrawGroup*        scene_draw_group )
{
  nw::gfx::RenderQueue* render_queue = scene_draw_group->GetRenderQueue();

  nw::gfx::RenderQueue::ElementListType* list = (reinterpret_cast<internal::FakeRenderQueue*>(render_queue))->GetList();

  model->SubmitView(list, *camera);
}
#endif  // GFL_GRP_G3D_H3D_USE

//-----------------------------------------------------------------------------
/*
 *  @brief        �����_�[�L���[�Ƀ����_�[�G�������g��ǉ�
 *
 *  @param[in]    camera         �����_�[�L���[�����Ɏg�p����J����
 *  @param[in]    scene_system   �`�悷��V�[���R���e���c���܂ރV�[���V�X�e��
 */
//-----------------------------------------------------------------------------
void RenderSystem::SubmitView(
    nw::gfx::Camera*       camera,
    SceneSystem*           scene_system,
    SceneCalculateGroup*   scene_calculate_group,
    SceneDrawGroup*        scene_draw_group )
{
  nw::gfx::ISceneUpdater* scene_updater      = scene_system->GetSceneUpdater();
  nw::gfx::SceneContext*  scene_context      = scene_calculate_group->GetSceneContext();
  nw::gfx::RenderQueue*   render_queue       = scene_draw_group->GetRenderQueue();
  s32                     render_queue_index = scene_draw_group->GetRenderQueueIndex();

  m_model_user_parameter_check_functor->SetRenderQueueIndex(render_queue_index);

  //----------------------------------------
  // �J�����Ɉˑ����鏈���ƁA�`��v�f�������_�[�L���[�ɐς݂܂��B

  // SORT_DEPTH_OF_TRANSLUCENT_MESH�̂Ƃ��A
  // ���������b�V���̐[�x�̂݌v�Z���āA���������b�V���̂ݐ[�x�ɂ��\�[�g�������悤�ɂȂ�܂��B
  // �[�x�v�Z�̏������ׂ̒ጸ�����҂ł��܂��B

  // SORT_DEPTH_OF_ALL_MESH�̂Ƃ��A
  // ���ׂẴ��b�V���̐[�x���v�Z���āA�[�x�ɂ���Ă��\�[�g�����悤�ɂȂ�܂��B
  // �s�������̓�����O����`�悷�邱�ƂŁAGPU�̃t�B�����ׂ̒ጸ�����҂ł��܂��B

  scene_updater->SetDepthSortMode(m_depth_sort_mode);

  scene_updater->SubmitView(
      render_queue,
      scene_context,
      *camera,
      0,
      1,
      m_model_user_parameter_check_functor,
      m_render_sort_mode);
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �V�[����`��
 *
 *  @param[in]    camera              ���̃V�[����`�悷��J����
 *  @param[in]    command_cache_dump  �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void RenderSystem::RenderScene(
    //nw::gfx::IRenderTarget*  render_target,
    gfl::grp::RenderTarget*  render_target,
    nw::gfx::Camera*         camera,
    SceneDrawGroup*          scene_draw_group,
    bool                     command_cache_dump
)
{
#if GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();

  RenderSceneParam param;
  {
    param.graphics_system       = NULL;
    param.render_system         = this;
    param.render_target         = render_target;
    param.nw_camera             = camera;
    param.scene_draw_group      = scene_draw_group;
    param.command_cache_builder = NULL;
    param.element_type          = RenderSceneParam::ELEMENT_TYPE_NONE;
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    param.view_mtx              = camera->ViewMatrix();
    param.inv_exist             = nn::math::MTX34Inverse(&param.inv_view_mtx, &param.view_mtx);
#endif
  }

#if GFL_DEBUG
    if ( H3dModel::IsSafeDeleteCheckEnable() || Particle::IsSafeDeleteCheckEnable() || Model::IsSafeDeleteCheckEnable() )
    {
      gfl::std::ForEach(
            render_queue->Begin(),
            render_queue->End(),
            DrawFrameSetFunc());
    }
#endif

  if(command_cache_dump)
  {
    nw::gfx::internal::CommandCacheBuilder builder;
  
    param.command_cache_builder = &builder;

    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
  }
  else
  {
    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
  }

  if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_GFX )
      RenderSystem::EndGfxRenderScene(m_render_context, m_mesh_renderer, &param);
  else if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
      RenderSystem::EndH3dRenderScene(m_render_context, m_mesh_renderer, &param);
#else  // GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();
  s32                    active_camera_index = scene_draw_group->GetActiveCameraIndex();

  // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
  m_render_context->ResetState();

  // �{�b�N�X�e�X�g�p�����[�^�̃N���A
  this->ClearBoxTestCount();

  //m_render_context->SetRenderTarget(render_target);  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
  m_render_context->SetRenderTarget(render_target->GetNwRenderTarget());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B

  // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
  // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
  // ���t���[���`��O�ɐݒ���s���K�v������܂��B
  m_render_context->SetActiveCamera(active_camera_index);

  scene_draw_group->BeginCommandInvokeCallback(m_render_context);

  m_render_context->SetCameraMatrix(camera);

  if(command_cache_dump)
  {
    nw::gfx::internal::CommandCacheBuilder builder;
    builder.Begin();

    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));

    builder.End();
    builder.Report();
  }
  else
  {
    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));
  }

  scene_draw_group->EndCommandInvokeCallback(m_render_context);
#endif  // GFL_GRP_G3D_H3D_USE
  m_fragmentAndWBufferDirty = false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �V�[����`��
 *
 *  @param[in]    camera              ���̃V�[����`�悷��J����
 *  @param[in]    command_cache_dump  �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void RenderSystem::RenderScene(
    gfl::grp::GraphicsSystem* graphics,
    gfl::grp::RenderTarget*   render_target,
    nw::gfx::Camera*          camera,
    SceneDrawGroup*           scene_draw_group,
    bool                      command_cache_dump
)
{
#if GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();
  
  if( graphics->BindCurrentCommandList( render_target ) )
  {
    RenderSceneParam param;
    {
      param.graphics_system       = graphics;
      param.render_system         = this;
      param.render_target         = render_target;
      param.nw_camera             = camera;
      param.scene_draw_group      = scene_draw_group;
      param.command_cache_builder = NULL;
      param.element_type          = RenderSceneParam::ELEMENT_TYPE_NONE;
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      param.view_mtx              = camera->ViewMatrix();
      param.inv_exist             = nn::math::MTX34Inverse(&param.inv_view_mtx, &param.view_mtx);
#endif
    }

    RenderSystem::BeginGfxRenderScene(m_render_context, m_mesh_renderer, &param);  // �J�����̐ݒ肪�K�v�Ȃ̂�

#if GFL_DEBUG
    if ( H3dModel::IsSafeDeleteCheckEnable() || Particle::IsSafeDeleteCheckEnable() || Model::IsSafeDeleteCheckEnable() )
    {
      gfl::std::ForEach(
            render_queue->Begin(),
            render_queue->End(),
            DrawFrameSetFunc());
    }
#endif

    if(command_cache_dump)
    {
      nw::gfx::internal::CommandCacheBuilder builder;
    
      param.command_cache_builder = &builder;

      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
    }
    else
    {
      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
    }

    if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_GFX )
        RenderSystem::EndGfxRenderScene(m_render_context, m_mesh_renderer, &param);
    else if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
        RenderSystem::EndH3dRenderScene(m_render_context, m_mesh_renderer, &param);
  }
#else  // GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();
  s32                    active_camera_index = scene_draw_group->GetActiveCameraIndex();

  if( graphics->BindCurrentCommandList( render_target ) )
  {
    // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
    m_render_context->ResetState();

    // �{�b�N�X�e�X�g�p�����[�^�̃N���A
    this->ClearBoxTestCount();

    m_render_context->SetRenderTarget(render_target->GetNwRenderTarget(), render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B

    // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
    // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
    // ���t���[���`��O�ɐݒ���s���K�v������܂��B
    m_render_context->SetActiveCamera(active_camera_index);

    scene_draw_group->BeginCommandInvokeCallback(m_render_context);

    m_render_context->SetCameraMatrix(camera);

    if(command_cache_dump)
    {
      nw::gfx::internal::CommandCacheBuilder builder;
      builder.Begin();

      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));

      builder.End();
      builder.Report();
    }
    else
    {
      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));
    }

    scene_draw_group->EndCommandInvokeCallback(m_render_context);
  }
#endif  // GFL_GRP_G3D_H3D_USE
  m_fragmentAndWBufferDirty = false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �X�e���I�f�B�X�v���C�ɑΉ������V�[���̕`��
 *                ���ʂɂ̂ݑΉ�
 *
 *  @param[in]
 *  @param[in]
 *  @param[in]
 */
//-----------------------------------------------------------------------------
void RenderSystem::RenderStereoScene(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       render_target_left,
    nw::gfx::Camera*              camera_left,
    gfl::grp::RenderTarget*       render_target_right,
    nw::gfx::Camera*              camera_right,
    SceneDrawGroup*               scene_draw_group,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    nw::gfx::Camera*              camera_base,
#endif
    bool                          command_cache_dump )
{
#if GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();

  if( graphics_system->BindCurrentCommandList( render_target_left ) )
  {
    RenderSceneParam param;
    {
      param.graphics_system         = graphics_system;
      param.render_system           = this;
      param.render_target           = render_target_left;
      param.nw_camera               = camera_left;
      param.render_target_extension = render_target_right;
      param.nw_camera_extension     = camera_right;
      param.scene_draw_group        = scene_draw_group;
      param.command_cache_builder   = NULL;
      param.element_type            = RenderSceneParam::ELEMENT_TYPE_NONE;         
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      param.view_mtx                = camera_base->ViewMatrix();
      param.inv_exist               = nn::math::MTX34Inverse(&param.inv_view_mtx, &param.view_mtx);
#endif
    }
    


    // �`��J�n
    BeginRenderScene( m_render_context, &param );
    {
      s32                    active_camera_index = param.scene_draw_group->GetActiveCameraIndex();

      // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
      m_render_context->ResetState();

      // �{�b�N�X�e�X�g�p�����[�^�̃N���A
      RenderSystemClearBoxTestCount();

      {
        m_render_context->SetRenderTarget(param.render_target->GetNwRenderTarget(), param.render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
       
        // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
        // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
        // ���t���[���`��O�ɐݒ���s���K�v������܂��B
        m_render_context->SetActiveCamera(active_camera_index);
      }
    }

    //RenderSystem::BeginGfxRenderStereoScene(m_render_context, m_mesh_renderer, &param);  // �����ݒ肪�K�v�Ȃ̂�
    
#ifdef RENDER_MESH_DEBUG_PRINT
    //�v�����g�������ꍇ�́A�����Ƀu���[�N�𒣂���true�ɂ��Ă��������B
    static b32      s_fPrint = false;
    
    if ( s_fPrint )
    {
      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          PrintFunc(m_render_context, m_mesh_renderer, &param));
      
      s_fPrint = false;
    }
#endif

#if GFL_DEBUG
    if ( H3dModel::IsSafeDeleteCheckEnable() || Particle::IsSafeDeleteCheckEnable() || Model::IsSafeDeleteCheckEnable() )
    {
      gfl::std::ForEach(
            render_queue->Begin(),
            render_queue->End(),
            DrawFrameSetFunc());
    }
#endif
    
    if(command_cache_dump)
    {
      nw::gfx::internal::CommandCacheBuilder builder;
    
      param.command_cache_builder = &builder;

      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderStereoSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
    }
    else
    {
      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderStereoSceneInternalFunctor(m_render_context, m_mesh_renderer, &param));
    }

    if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_GFX )
        RenderSystem::EndGfxRenderStereoScene(m_render_context, m_mesh_renderer, &param);
    else if( param.element_type == RenderSystem::RenderSceneParam::ELEMENT_TYPE_H3D )
        RenderSystem::EndH3dRenderStereoScene(m_render_context, m_mesh_renderer, &param);
  }
#else  // GFL_GRP_G3D_H3D_USE
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();
  s32                    active_camera_index = scene_draw_group->GetActiveCameraIndex();


  // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
  m_render_context->ResetState();

  // �{�b�N�X�e�X�g�p�����[�^�̃N���A
  this->ClearBoxTestCount();

  if( graphics_system->BindCurrentCommandList( render_target_left ) )
  {
    m_render_context->SetRenderTarget(render_target_left->GetNwRenderTarget(), render_target_left->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B

    // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
    // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
    // ���t���[���`��O�ɐݒ���s���K�v������܂��B
    m_render_context->SetActiveCamera(active_camera_index);

    // �R�}���h�Z�[�u�J�n
    graphics_system->StartCommandSave();

    scene_draw_group->BeginCommandInvokeCallback(m_render_context);

    if(command_cache_dump)
    {
      nw::gfx::internal::CommandCacheBuilder builder;
      builder.Begin();

      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));

      builder.End();
      builder.Report();
    }
    else
    {
      gfl::std::ForEach(
          render_queue->Begin(),
          render_queue->End(),
          RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));
    }

    scene_draw_group->EndCommandInvokeCallback(m_render_context);

    // �R�}���h�Z�[�u�I��
    graphics_system->EndCommandSave();
  }

  // ���ڗp
  if( graphics_system->BindCurrentCommandList( render_target_left ) )
  {
    m_render_context->SetRenderTarget(render_target_left->GetNwRenderTarget(), render_target_left->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
    m_render_context->SetCameraMatrix(camera_left);
    // �R�}���h�Đ�
    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }

  // �E�ڗp
  if( graphics_system->BindCurrentCommandList( render_target_right ) )
  {
    m_render_context->SetRenderTarget(render_target_right->GetNwRenderTarget(), render_target_right->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
    m_render_context->SetCameraMatrix(camera_right);
    // �R�}���h�Đ�
    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
#endif  // GFL_GRP_G3D_H3D_USE
  m_fragmentAndWBufferDirty = false;
}

#if 0
//h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
//-----------------------------------------------------------------------------
/*
 *  @brief        �R�}���h�Z�[�u�������ăV�[���̕`��͂��Ȃ�
 */
//-----------------------------------------------------------------------------
void RenderSystem::CommandSaveNotRenderScene(
  gfl::grp::GraphicsSystem*     graphics_system,
  nw::gfx::IRenderTarget*       render_target,
  SceneDrawGroup*               scene_draw_group,
  bool                          command_cache_dump)
{
  nw::gfx::RenderQueue*  render_queue        = scene_draw_group->GetRenderQueue();
  s32                    active_camera_index = scene_draw_group->GetActiveCameraIndex();

  // gfx ���C�u�����ł̕`���� ResetState ���Ăяo�����Ƃŕ`��ɗp�����X�e�[�g�����Z�b�g���܂��B
  m_render_context->ResetState();

  // �{�b�N�X�e�X�g�p�����[�^�̃N���A
  this->ClearBoxTestCount();

  m_render_context->SetRenderTarget(render_target);  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B

  // ���C�g�A���}�b�v�AWScale �p�̃J������ݒ肵�܂��B
  // ActiveCamera �� RenderContext::ResetState �Ń��Z�b�g����܂��̂ŁA
  // ���t���[���`��O�ɐݒ���s���K�v������܂��B
  m_render_context->SetActiveCamera(active_camera_index);

  // �R�}���h�Z�[�u�J�n
  graphics_system->StartCommandSave();

  scene_draw_group->BeginCommandInvokeCallback(m_render_context);

  if(command_cache_dump)
  {
    nw::gfx::internal::CommandCacheBuilder builder;
    builder.Begin();

    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));

    builder.End();
    builder.Report();
  }
  else
  {
    gfl::std::ForEach(
        render_queue->Begin(),
        render_queue->End(),
        RenderSceneInternalFunctor(m_render_context, m_mesh_renderer));
  }

  scene_draw_group->EndCommandInvokeCallback(m_render_context);

  // �R�}���h�Z�[�u�I��
  graphics_system->EndCommandSave();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        �R�}���h�Z�[�u���Ă�����̂��g���ăV�[���̕`�������
 */
//-----------------------------------------------------------------------------
void RenderSystem::ReuseCommandSaveRenderScene(
  gfl::grp::GraphicsSystem*     graphics_system,
  gfl::grp::RenderTarget*       render_target,
  nw::gfx::Camera*              camera )
{

  if( graphics_system->BindCurrentCommandList( render_target ) )
  {
    m_render_context->SetRenderTarget(render_target->GetNwRenderTarget(), render_target->GetNwViewport());  // nw::gfx::RenderContext���K�v�Ƃ��Ă���̂ň�U�ݒ�B
    m_render_context->SetCameraMatrix(camera);

    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief        �{�b�N�X�e�X�g�@���ʃJ�E���^�̒l�N���A
 */
//-----------------------------------------------------------------------------
void RenderSystem::ClearBoxTestCount( void )
{
  RenderSystemClearBoxTestCount();
}


#if GFL_GRP_G3D_H3D_USE
void RenderSystem::createH3dSystem(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator)
{
  m_h3d_command = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::H3dCommand;

  s32 max_cameras;
  s32 max_light_sets;
  s32 max_fogs;
  s32 max_vertex_lights;
  const nw::gfx::SceneEnvironment& scene_environment = this->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  m_h3d_environment = GFL_NEW(heap_allocator->GetHeapBase()) gfl::grp::g3d::H3dEnvironment;
  m_h3d_environment->CreateFromSceneEnvironment(heap_allocator, device_allocator, scene_environment, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
}

void RenderSystem::destroyH3dSystem(void)
{
  if( m_h3d_environment )
  {
    GFL_DELETE m_h3d_environment;
    m_h3d_environment = NULL;
  }

  if( m_h3d_command )
  {
    GFL_DELETE m_h3d_command;
    m_h3d_command = NULL;
  }
}
#endif  // GFL_GRP_G3D_H3D_USE


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

