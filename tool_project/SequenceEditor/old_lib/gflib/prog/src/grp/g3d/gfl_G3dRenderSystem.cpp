//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dRenderSystem.cpp
 *	@brief  3D描画システム
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

// gflib grp g3d 非公開ヘッダ
#include "gfl_G3dFakeRenderQueue.h"
#include "gfl_G3dRenderElementAccessor.h"




// このマクロを有効にすると、ログ出力にレンダーキューのソート後の状態をレポートします。
//#define GFL_GRP_G3D_RENDER_SYSTEM_REPORT_RENDER_QUEUE_ENABLED

//メッシュの描画順を確認する場合に使用
//#define RENDER_MESH_DEBUG_PRINT

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					ボックステスト用ファイル内グローバル変数
 */
//-----------------------------------------------------------------------------
static bool RenderSystem_BoxTestFlag = false;            // ボックステスト有効・無効（デフォルト無効）
static s16 RenderSystem_BoxTestThrough = 0;         // ボックステスト　スルー数
static s16 RenderSystem_BoxTestCut = 0;             // ボックステスト　カット数

static void RenderSystemClearBoxTestCount(void);
static void RenderSystemClearBoxTestCount(void)
{
  RenderSystem_BoxTestThrough = 0;
  RenderSystem_BoxTestCut = 0;
}


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*
 *  @brief        ボックステストを行う。
 *
 *  @retval true    描画する
 *  @retval false   描画しない。
 */
//-----------------------------------------------------------------------------
static bool IsBoxTest( const nw::ut::ResOrientedBoundingBox& bounding, const nw::gfx::Camera& camera, const nw::gfx::Model& model );



//-----------------------------------------------------------------------------
/**
 *					staticメンバ関数
 */
//-----------------------------------------------------------------------------
#if GFL_GRP_G3D_H3D_USE
#if 0
 //if分岐をもしやめるなら、下記のように関数テーブルを用意し現在のelement_typeの変更に合わせて関数テーブルに登録する関数を変えるのがベストかな。

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
  // element_typeを変更しないし何もしない。
}
#endif

//----------------------------------------------------------------------------
/**
 *	@brief  描画開始設定
 */
//-----------------------------------------------------------------------------
void RenderSystem::BeginRenderScene( nw::gfx::RenderContext* render_context, RenderSceneParam* param )
{
  s32 active_camera_index = param->scene_draw_group->GetActiveCameraIndex();

  {
    // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
    render_context->ResetState();

    // ボックステストパラメータのクリア
    RenderSystemClearBoxTestCount();

    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。

    // GFSBTS[1805]: ビューポート矩形と同じ領域でシザーテストを設定
    nw::gfx::GraphicsDevice::ActivateScissor(true, param->render_target->GetNwViewport().GetBound());

    // ライト、環境マップ、WScale 用のカメラを設定します。
    // ActiveCamera は RenderContext::ResetState でリセットされますので、
    // 毎フレーム描画前に設定を行う必要があります。
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
  //uuuuuuuuuuuuuuuuuuuu初期化せず
  s32 max_cameras;
  s32 max_light_sets;
  s32 max_fogs;
  s32 max_vertex_lights;
  const nw::gfx::SceneEnvironment& scene_environment = param->render_system->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  // ライトやフォグがアニメーションしているかもしれないので毎フレーム設定する
  param->render_system->GetH3dEnvironment()->SetFromSceneEnvironment(scene_environment, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
#endif
  
  {//@fix GFBTS[1535] インターリーブ配列設定初期化（ホーム画面ではこの設定が崩される。3dはインターリーブ配列）
    H3dCommand* command = param->render_system->GetH3dCommand();
    static const u32 s_TagCommands[] ={ 
      0x00000000, PICA_CMD_HEADER( 0x206, 33-1, 0xF, 1 ),//0x206はロードアレイレジスタ gx_MacroReg.hに定義はなかった。
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

  // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
  render_context->ResetState();

  // ボックステストパラメータのクリア
  RenderSystemClearBoxTestCount();

  {
    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
   
    // GFSBTS[1805]: ビューポート矩形と同じ領域でシザーテストを設定
    nw::gfx::GraphicsDevice::ActivateScissor(true, param->render_target->GetNwViewport().GetBound());

    // ライト、環境マップ、WScale 用のカメラを設定します。
    // ActiveCamera は RenderContext::ResetState でリセットされますので、
    // 毎フレーム描画前に設定を行う必要があります。
    render_context->SetActiveCamera(active_camera_index);
    
    // コマンドセーブ開始
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
  // コマンドセーブ開始
  param->graphics_system->StartCommandSave();

  param->render_system->GetH3dCommand()->BegineDraw();
  
#if 0
  //uuuuuuuuuuuuuuuuuuuu初期化せず
  s32 max_cameras;
  s32 max_light_sets;
  s32 max_fogs;
  s32 max_vertex_lights;
  const nw::gfx::SceneEnvironment& scene_environment = param->render_system->GetSceneEnvironment(&max_cameras, &max_light_sets, &max_fogs, &max_vertex_lights);

  // ライトやフォグがアニメーションしているかもしれないので毎フレーム設定する
  param->render_system->GetH3dEnvironment()->SetFromSceneEnvironment(scene_environment, max_cameras, max_light_sets, max_fogs, max_vertex_lights);
#endif
  
  {//@fix GFBTS[1535] インターリーブ配列設定初期化（ホーム画面ではこの設定が崩される。H3dはインターリーブ配列）
    H3dCommand* command = param->render_system->GetH3dCommand();
    static const u32 s_TagCommands[] ={ 
      0x00000000, PICA_CMD_HEADER( 0x206, 33-1, 0xF, 1 ),//0x206はロードアレイレジスタ gx_MacroReg.hに定義はなかった。
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
  // コマンドセーブ終了
  param->graphics_system->EndCommandSave();
 
  // 左目用
  if( param->graphics_system->BindCurrentCommandList( param->render_target ) )
  {
    render_context->SetRenderTarget(param->render_target->GetNwRenderTarget(), param->render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
    render_context->SetCameraMatrix(param->nw_camera);
    // コマンド再生
    param->graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
 
  // 右目用
  if( param->graphics_system->BindCurrentCommandList( param->render_target_extension ) )
  {
    render_context->SetRenderTarget(param->render_target_extension->GetNwRenderTarget(), param->render_target_extension->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
    render_context->SetCameraMatrix(param->nw_camera_extension);
    // コマンド再生
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
  RenderSystem::RenderSceneParam* m_param;  // 他のところのものを覚えておくだけ
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
  RenderSystem::RenderSceneParam* m_param;  // 他のところのものを覚えておくだけ

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

      if( m_param )//KW230修正
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
  RenderSystem::RenderSceneParam* m_param;  // 他のところのものを覚えておくだけ

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

      //もう既にGFL_DEBUG内#if GFL_DEBUG    
        if ( H3dModel::IsSafeDeleteCheckEnable() )
          cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();
      //もう既にGFL_DEBUG内#endif

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
          // nw::gfx::ResShape::TYPE_INFO は、なさそう。調べたモデルは全部nw::gfx::ResSeparateDataShape::TYPE_INFOだった。
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
//もう既にGFL_DEBUG内#if GFL_DEBUG
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
//もう既にGFL_DEBUG内#endif
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
 *    ボックステストの設定,パラメータ取得
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  ボックステスト設定
 *
 *	@param	flag  フラグ
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetBoxTest( bool flag )
{
  RenderSystem_BoxTestFlag = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ボックステスト有無チェック
 *
 *	@retval true    ボックステスト有効
 *	@retval false   ボックステスト無効
 */
//-----------------------------------------------------------------------------
bool RenderSystem::IsBoxTest( void )
{
  return RenderSystem_BoxTestFlag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ボックステスト　カットShape数取得
 *
 *	@return ボックステストにより、カットされたShape数
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetBoxTestCutCount( void )
{
  return RenderSystem_BoxTestCut;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ボックステスト　スルーShape数取得
 *
 *	@return ボックステストを通り抜けたShape数
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetBoxTestThroughCount( void )
{
  return RenderSystem_BoxTestThrough;
}


//-----------------------------------------------------------------------------
/**
 *					クラス定義
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D描画システムクラス
//=====================================
//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *
 *  @param[in]     allocator  アロケータ
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
    // 半透明以外のメッシュで深度情報の量子化を行わなくするための
    // レンダーキー・ファクトリクラスを生成します。
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
    // サイズをコンパイル時にチェックしnw::gfx::ResMeshの変更に気付けるようにしておく。
#endif  // GFL_GRP_G3D_H3D_USE

}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
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

  // RenderQueue::Reset の引数に true を指定すると、ソート用キーをキャッシュする最適化機能が
  // 有効になります。
  // 半透明以外のソート用キーのみキャッシュされるようになります。
  render_queue->Reset( ((m_depth_sort_mode==nw::gfx::ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)?true:false) );
}

void RenderSystem::EndSubmitView(SceneDrawGroup* scene_draw_group)
{
  nw::gfx::RenderQueue* render_queue = scene_draw_group->GetRenderQueue();

  //----------------------------------------
  // レンダーキューをソートします。
  // RenderElementCompare() やレンダーキーファクトリーをカスタマイズすることで
  // 描画順を変更することができます。
  // 詳しくは最適化TIPSを参照してください。
  gfl::std::Sort(
      render_queue->Begin(),
      render_queue->End(),
      nw::gfx::RenderElementCompare());
  
  {
#ifdef GFL_GRP_G3D_RENDER_SYSTEM_REPORT_RENDER_QUEUE_ENABLED
  // レンダーキューのソート後の状態をレポートします。
  // リリース版ではなにも表示されなくなっています。
  gfl::std::ForEach(
      render_queue->Begin(),
      render_queue->End(),
      nw::gfx::RenderQueue::ReportFunctor());
#endif
  }

  // SubmitViewで、メッシュカウントをクリア
  this->ClearRenderMeshCount();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境設定からシーン環境を適用する
 *
 *  @param[in]     scene_environment_setting  適用するシーン環境
 *                                            (nw::gfx::SceneEnvironmentSetting::ResolveReferenceが済んでいること)
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetSceneEnvironmentSetting(const nw::gfx::SceneEnvironmentSetting& scene_environment_setting)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();

  {
    // カメラのインデックスがm_max_cameras以上の値を使っていないか確認する。
    // m_max_cameras以上はシステムで使うので、ユーザは使ってはならない。
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
        GFL_ASSERT_MSG(0, "camera_index=%dが%d<= <%dに収まっていません。\n", camera_index, 0, m_max_cameras);
      }
    }
  }

  scene_environment.ApplyFrom(scene_environment_setting);
  m_fragmentAndWBufferDirty = true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境にカメラを設定する
 *
 *  @param[in]     camera_index   カメラのインデックス
 *  @param[in]     camera         カメラ(camera_indexに設定する)
 *
 *  「camera=NULL, camera_index=インデックス」を渡すとcamera_indexに割り当てていたカメラをなしにする
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
    GFL_ASSERT_MSG(0, "camera_index=%dが%d<= <%dに収まっていません。\n", camera_index, 0, m_max_cameras);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境のカメラ配列からカメラを取得する
 *
 *  @param[in]     camera_index   カメラのインデックス
 *
 *  @retval        配列のこのインデックスに渡してあったカメラのポインタを得る。
 *                 NULLのときこのインデックスにはカメラを渡していなかった。
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
    GFL_ASSERT_MSG(0, "camera_index=%dが%d<= <%dに収まっていません。\n", camera_index, 0, m_max_cameras);
    return NULL;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境のカメラ配列からカメラのインデックスを取得する
 *
 *  @param[in]     camera   カメラ
 *
 *  @retval        配列にこのカメラがある場合そのインデックスを得る。
 *                 ない場合CAMERA_INDEX_NONEを返す。
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
 *  @brief         シーン環境のカメラ配列から一番最初にあるカメラのインデックスを取得する
 *
 *  @retval        配列に一つもカメラがない場合CAMERA_INDEX_NONEを返す。
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
 *  @brief         シーン環境にカメラをカメラ配列に設定していないカメラを設定する
 *
 *  @param[in]     camera         カメラ(GetNotSetCameraIndexに設定する)
 *
 *  「camera=NULL」を渡すとGetNotSetCameraIndexに割り当てていたカメラをなしにする
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetNotSetCamera(nw::gfx::Camera* camera)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
  scene_environment.SetCamera(this->GetNotSetCameraIndex(), camera);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         カメラをカメラ配列に設定していないときに使うカメラのインデックスを取得する
 *
 *  @retval        m_max_cameras+SYSTEM_CAMERA_NOT_SETを返す。
 */
//-----------------------------------------------------------------------------
s32 RenderSystem::GetNotSetCameraIndex(void) const
{
  return (m_max_cameras + SYSTEM_CAMERA_NOT_SET);
}


//-----------------------------------------------------------------------------
/*
 *  @brief         シーン環境にライトセットを設定する
 *
 *  @param[in]     light_set_index   設定先となるライトセットのインデックス 
 *  @param[in]     light_set         ライトセット。
 *                                   呼び出し元はこのライトセットを保持し続けて下さい。
 *                                   light_set_indexに設定しているライトセットを外す場合はNULLを渡して下さい。
 */
//-----------------------------------------------------------------------------
void RenderSystem::SetLightSet(s32 light_set_index, nw::gfx::LightSet* light_set)
{
  nw::gfx::SceneEnvironment& scene_environment = m_render_context->GetSceneEnvironment();
  scene_environment.SetLightSet(light_set_index, light_set);
  m_fragmentAndWBufferDirty = true;
 
#if 0
  /*メモ
  何も設定していないときは
  nw::gfx::LightSet* light_set = scene_environment.GetLightSet(light_set_index);
  で得られるlight_setはNULLになっている。
  scene_environment.ApplyFrom(scene_environment_setting)によって
  scene_environment_settingが保持しているLightSetのポインタがscene_environmentに設定される。
  だからscene_environment_settingは、ApplyFromが終わってからも破棄してはならない。*/
#endif
}

#if GFL_GRP_G3D_H3D_USE
//-----------------------------------------------------------------------------
/*
 *  @brief       h3d環境をgfx環境からつくる
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
 *  @brief        レンダーキューにモデル(nw::gfxのModel,SkeletalModel,ParticleModel)を追加
 *
 *  @param[in]    camera         レンダーキュー生成に使用するカメラ
 *  @param[in]    scene_system   描画するシーンコンテンツを含むシーンシステム
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
  // カメラに依存する処理と、描画要素をレンダーキューに積みます。

  // SORT_DEPTH_OF_TRANSLUCENT_MESHのとき、
  // 半透明メッシュの深度のみ計算して、半透明メッシュのみ深度によるソートがされるようになります。
  // 深度計算の処理負荷の低減を期待できます。

  // SORT_DEPTH_OF_ALL_MESHのとき、
  // すべてのメッシュの深度を計算して、深度によってもソートされるようになります。
  // 不透明物体等を手前から描画することで、GPUのフィル負荷の低減を期待できます。

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
 *  @brief        レンダーキューにH3Dモデルを追加
 *
 *  @param[in]    camera         レンダーキュー生成に使用するカメラ
 *  @param[in]    scene_system   描画するシーンコンテンツを含むシーンシステム
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
 *  @brief        レンダーキューにレンダーエレメントを追加
 *
 *  @param[in]    camera         レンダーキュー生成に使用するカメラ
 *  @param[in]    scene_system   描画するシーンコンテンツを含むシーンシステム
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
  // カメラに依存する処理と、描画要素をレンダーキューに積みます。

  // SORT_DEPTH_OF_TRANSLUCENT_MESHのとき、
  // 半透明メッシュの深度のみ計算して、半透明メッシュのみ深度によるソートがされるようになります。
  // 深度計算の処理負荷の低減を期待できます。

  // SORT_DEPTH_OF_ALL_MESHのとき、
  // すべてのメッシュの深度を計算して、深度によってもソートされるようになります。
  // 不透明物体等を手前から描画することで、GPUのフィル負荷の低減を期待できます。

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
 *  @brief        シーンを描画
 *
 *  @param[in]    camera              このシーンを描画するカメラ
 *  @param[in]    command_cache_dump  コマンドキャッシュをログ出力する
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

  // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
  m_render_context->ResetState();

  // ボックステストパラメータのクリア
  this->ClearBoxTestCount();

  //m_render_context->SetRenderTarget(render_target);  // nw::gfx::RenderContextが必要としているので一旦設定。
  m_render_context->SetRenderTarget(render_target->GetNwRenderTarget());  // nw::gfx::RenderContextが必要としているので一旦設定。

  // ライト、環境マップ、WScale 用のカメラを設定します。
  // ActiveCamera は RenderContext::ResetState でリセットされますので、
  // 毎フレーム描画前に設定を行う必要があります。
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
 *  @brief        シーンを描画
 *
 *  @param[in]    camera              このシーンを描画するカメラ
 *  @param[in]    command_cache_dump  コマンドキャッシュをログ出力する
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

    RenderSystem::BeginGfxRenderScene(m_render_context, m_mesh_renderer, &param);  // カメラの設定が必要なので

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
    // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
    m_render_context->ResetState();

    // ボックステストパラメータのクリア
    this->ClearBoxTestCount();

    m_render_context->SetRenderTarget(render_target->GetNwRenderTarget(), render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。

    // ライト、環境マップ、WScale 用のカメラを設定します。
    // ActiveCamera は RenderContext::ResetState でリセットされますので、
    // 毎フレーム描画前に設定を行う必要があります。
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
 *  @brief        ステレオディスプレイに対応したシーンの描画
 *                上画面にのみ対応
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
    


    // 描画開始
    BeginRenderScene( m_render_context, &param );
    {
      s32                    active_camera_index = param.scene_draw_group->GetActiveCameraIndex();

      // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
      m_render_context->ResetState();

      // ボックステストパラメータのクリア
      RenderSystemClearBoxTestCount();

      {
        m_render_context->SetRenderTarget(param.render_target->GetNwRenderTarget(), param.render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
       
        // ライト、環境マップ、WScale 用のカメラを設定します。
        // ActiveCamera は RenderContext::ResetState でリセットされますので、
        // 毎フレーム描画前に設定を行う必要があります。
        m_render_context->SetActiveCamera(active_camera_index);
      }
    }

    //RenderSystem::BeginGfxRenderStereoScene(m_render_context, m_mesh_renderer, &param);  // 初期設定が必要なので
    
#ifdef RENDER_MESH_DEBUG_PRINT
    //プリントしたい場合は、ここにブレークを張ってtrueにしてください。
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


  // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
  m_render_context->ResetState();

  // ボックステストパラメータのクリア
  this->ClearBoxTestCount();

  if( graphics_system->BindCurrentCommandList( render_target_left ) )
  {
    m_render_context->SetRenderTarget(render_target_left->GetNwRenderTarget(), render_target_left->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。

    // ライト、環境マップ、WScale 用のカメラを設定します。
    // ActiveCamera は RenderContext::ResetState でリセットされますので、
    // 毎フレーム描画前に設定を行う必要があります。
    m_render_context->SetActiveCamera(active_camera_index);

    // コマンドセーブ開始
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

    // コマンドセーブ終了
    graphics_system->EndCommandSave();
  }

  // 左目用
  if( graphics_system->BindCurrentCommandList( render_target_left ) )
  {
    m_render_context->SetRenderTarget(render_target_left->GetNwRenderTarget(), render_target_left->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
    m_render_context->SetCameraMatrix(camera_left);
    // コマンド再生
    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }

  // 右目用
  if( graphics_system->BindCurrentCommandList( render_target_right ) )
  {
    m_render_context->SetRenderTarget(render_target_right->GetNwRenderTarget(), render_target_right->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
    m_render_context->SetCameraMatrix(camera_right);
    // コマンド再生
    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
#endif  // GFL_GRP_G3D_H3D_USE
  m_fragmentAndWBufferDirty = false;
}

#if 0
//h3dでサポートしきれないのでコメントアウト。
//-----------------------------------------------------------------------------
/*
 *  @brief        コマンドセーブだけしてシーンの描画はしない
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

  // gfx ライブラリでの描画後に ResetState を呼び出すことで描画に用いたステートをリセットします。
  m_render_context->ResetState();

  // ボックステストパラメータのクリア
  this->ClearBoxTestCount();

  m_render_context->SetRenderTarget(render_target);  // nw::gfx::RenderContextが必要としているので一旦設定。

  // ライト、環境マップ、WScale 用のカメラを設定します。
  // ActiveCamera は RenderContext::ResetState でリセットされますので、
  // 毎フレーム描画前に設定を行う必要があります。
  m_render_context->SetActiveCamera(active_camera_index);

  // コマンドセーブ開始
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

  // コマンドセーブ終了
  graphics_system->EndCommandSave();
}

//-----------------------------------------------------------------------------
/*
 *  @brief        コマンドセーブしてあるものを使ってシーンの描画をする
 */
//-----------------------------------------------------------------------------
void RenderSystem::ReuseCommandSaveRenderScene(
  gfl::grp::GraphicsSystem*     graphics_system,
  gfl::grp::RenderTarget*       render_target,
  nw::gfx::Camera*              camera )
{

  if( graphics_system->BindCurrentCommandList( render_target ) )
  {
    m_render_context->SetRenderTarget(render_target->GetNwRenderTarget(), render_target->GetNwViewport());  // nw::gfx::RenderContextが必要としているので一旦設定。
    m_render_context->SetCameraMatrix(camera);

    graphics_system->ReuseCommandSave();
    GFL_GL_ASSERT();
  }
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief        ボックステスト　効果カウンタの値クリア
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

