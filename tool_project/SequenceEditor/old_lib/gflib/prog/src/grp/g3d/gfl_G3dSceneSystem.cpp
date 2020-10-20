//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneSystem.cpp
 *	@brief  3Dシーンシステム
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

// gflib grp g3d
#include <grp/g3d/gfl_G3dScene.h>
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dSceneCalculateGroup.h>


namespace gfl {
namespace grp {
namespace g3d {


GFL_DEBUG_TICK_SETUP(SCENESYSTEM, 10);


//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dシーンシステムクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *
 *  @param[in]     allocator    アロケータ
 *  @param[in]     description  シーンシステムの設定内容
 *                              (NULLのときはデフォルト値。
 *                              呼び出し元はこの関数から戻ってきたらdescriptionとして渡したものを削除してよい)
 */
//-----------------------------------------------------------------------------
SceneSystem::SceneSystem(gfl::heap::NwAllocator* allocator, const Description* description)
  : m_scene_traverser(NULL),
    m_world_matrix_updater(NULL),
    m_skeleton_updater(NULL),
    m_billboard_updater(NULL),
    m_scene_updater(NULL),
    m_scene_initializer(NULL),
    m_particle_scene_updater(NULL)
{
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_scene_traverser = nw::gfx::SceneTraverser::Builder()
    .Create(allocator);

  nw::gfx::WorldMatrixUpdater* world_matrix_updater = nw::gfx::WorldMatrixUpdater::Builder()   // SceneUpdaterを破棄する際に一緒に破棄される
    .Create(allocator);
  m_world_matrix_updater = world_matrix_updater;
  nw::gfx::SkeletonUpdater* skeleton_updater = nw::gfx::SkeletonUpdater::Builder()             // SceneUpdaterを破棄する際に一緒に破棄される
    .Create(allocator);
  m_skeleton_updater = skeleton_updater;
  nw::gfx::BillboardUpdater* billboard_updater = nw::gfx::BillboardUpdater::Create(allocator);  // SceneUpdaterを破棄する際に一緒に破棄される
  m_billboard_updater = billboard_updater;
  m_scene_updater = nw::gfx::SceneUpdater::Builder()
    .WorldMatrixUpdaterPtr(world_matrix_updater)
    .SkeletonUpdaterPtr(skeleton_updater)
    .BillboardUpdaterPtr(billboard_updater)
    .Create(allocator);

#if 1
  // g3d専用のマテリアルID生成クラス
  nw::gfx::IMaterialIdGenerator* material_id_generator = gfl::grp::g3d::SortingMaterialIdGenerator::Builder()  // SceneInitializerを破棄する際に一緒に破棄される
    .IsFixedSizeMemory(l_description.is_fixed_size_memory)
    .MaxMaterials(l_description.max_materials)
    .Create(allocator);
#else
  // デフォルトのマテリアルID生成クラス
  nw::gfx::IMaterialIdGenerator* material_id_generator = nw::gfx::SortingMaterialIdGenerator::Builder()  // SceneInitializerを破棄する際に一緒に破棄される
    .IsFixedSizeMemory(l_description.is_fixed_size_memory)
    .MaxMaterials(l_description.max_materials)
    .Create(allocator);
#endif
  m_scene_initializer = nw::gfx::SceneInitializer::Builder()
    .MaterialIdGenerator(material_id_generator)
    .Create(allocator);

  m_particle_scene_updater =  nw::gfx::ParticleSceneUpdater::Builder()
    .Create(allocator);

  GFL_GL_ASSERT();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 */
//-----------------------------------------------------------------------------
SceneSystem::~SceneSystem()
{
  nw::gfx::SafeDestroy(m_particle_scene_updater);
  nw::gfx::SafeDestroy(m_scene_initializer);
  nw::gfx::SafeDestroy(m_scene_traverser);
  nw::gfx::SafeDestroy(m_scene_updater);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         シーンの初期化
 *
 *  @param[in]     scene_root                初期化するシーンのルートノード
 *  @param[in]     scene_initialize_process  初期化処理の内容
 */
//-----------------------------------------------------------------------------
void SceneSystem::InitializeScene(SceneCalculateGroup* scene_calculate_group, const SceneInitializeProcess scene_initialize_process)
{
  switch( scene_initialize_process )
  {
  case SCENE_INITIALIZE_PROCESS_ALL:
    {
      nw::gfx::TransformNode* scene_root    = scene_calculate_group->GetSceneRoot();
      
      m_scene_initializer->Begin();
      scene_root->Accept(m_scene_initializer);
      m_scene_initializer->End();
    
      TraverseScene(scene_calculate_group);
    }
    break;
  case SCENE_INITIALIZE_PROCESS_TRAVERSE:
    {
      TraverseScene(scene_calculate_group);
    }
    break;
  }
}
 
//-----------------------------------------------------------------------------
/*
 *  @brief         ルートノードのトラバース
 *
 *  @param[in]     scene_root   対象のルートノード
 */
//-----------------------------------------------------------------------------
void SceneSystem::TraverseScene(SceneCalculateGroup* scene_calculate_group)
{
  nw::gfx::SceneContext*  scene_context = scene_calculate_group->GetSceneContext();
  nw::gfx::TransformNode* scene_root    = scene_calculate_group->GetSceneRoot();

  m_scene_traverser->Begin(scene_context);
  scene_root->Accept(m_scene_traverser);
  m_scene_traverser->End();

  // パーティクルセットの依存順でソートする
  gfl::std::Sort(
    scene_context->GetParticleSetBegin(),
    scene_context->GetParticleSetEnd(),
    nw::gfx::ParticleSetCompare());
}
 
#if GFL_DEBUG

//-----------------------------------------------------------------------------
/*
 *  @brief         シーンの更新
 *  デバッグ版
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateScene(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();
  
  GFL_DEBUG_TICK_SET_FLAG(SCENESYSTEM, true);
  
  GFL_DEBUG_TICK_START(SCENESYSTEM, 0, "all");
  {
    GFL_DEBUG_TICK_START(SCENESYSTEM, 1, "before world update anime");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
      {
        m_scene_updater->EvaluateAnim(scene_context, nw::anim::res::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
      }
    }
    GFL_DEBUG_TICK_END(SCENESYSTEM, 1);

    GFL_DEBUG_TICK_START(SCENESYSTEM, 2, "UpdateTransform");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
      {
        m_scene_updater->UpdateTransformNode(scene_context);
      }
    }
    GFL_DEBUG_TICK_END(SCENESYSTEM, 2);

    // @note 将来、描画カリングを行う場合はここで処理する

    GFL_DEBUG_TICK_START(SCENESYSTEM, 3, "AfterSceneCulling");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
      {
        m_scene_updater->EvaluateAnim(scene_context, nw::anim::res::ResGraphicsAnimGroup::EVALUATION_AFTER_SCENE_CULLING);
      }
    }
    GFL_DEBUG_TICK_END(SCENESYSTEM, 3);

    GFL_DEBUG_TICK_START(SCENESYSTEM, 4, "UpdateSkeletal");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
      {
        m_scene_updater->UpdateSkeletalModel(scene_context);
      }
    }
    GFL_DEBUG_TICK_END(SCENESYSTEM, 4);
    
    GFL_DEBUG_TICK_START(SCENESYSTEM, 5, "UpdateAnime");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM )
      {
        m_scene_updater->UpdateAnim(scene_context);
      }
    }
    GFL_DEBUG_TICK_END(SCENESYSTEM, 5);

    GFL_DEBUG_TICK_START(SCENESYSTEM, 6, "UpdateParticle");
    {
      if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE )
      {
        UpdateParticle(scene_calculate_group);
      }
    }
  }
  GFL_DEBUG_TICK_END(SCENESYSTEM, 0);
}

#else  // GFL_DEBUG

//-----------------------------------------------------------------------------
/*
 *  @brief         シーンの更新
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateScene(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();
  //m_scene_updater->UpdateAll(scene_context);

  
  // m_scene_updater->UpdateAll(scene_context)の中身を個別に呼ぶことにした。
  // ここから
    //NW_NULL_ASSERT(sceneContext);

    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
    {
      m_scene_updater->EvaluateAnim(scene_context, nw::anim::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
    }
    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
    {
      m_scene_updater->UpdateTransformNode(scene_context);
    }
    // @note 将来、描画カリングを行う場合はここで処理する
    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
    {
      m_scene_updater->EvaluateAnim(scene_context, nw::anim::ResGraphicsAnimGroup::EVALUATION_AFTER_SCENE_CULLING);
    }
    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
    {
      m_scene_updater->UpdateSkeletalModel(scene_context);
    }
    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM )
    {
      m_scene_updater->UpdateAnim(scene_context);
    }
  // ここまで


  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE )
  {
    UpdateParticle(scene_calculate_group);
  }
}

#endif  // GFL_DEBUG

//-----------------------------------------------------------------------------
/*
 *  @brief         シーンの更新（カリング前）
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateSceneBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();
  
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
  {
    m_scene_updater->EvaluateAnim(scene_context, nw::anim::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
  }
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
  {
    m_scene_updater->UpdateTransformNode(scene_context);
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         シーンの更新（カリング後）
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateSceneAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();

  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
  {
    m_scene_updater->EvaluateAnim(scene_context, nw::anim::ResGraphicsAnimGroup::EVALUATION_AFTER_SCENE_CULLING);
  }
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
  {
    m_scene_updater->UpdateSkeletalModel(scene_context);
  }
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM )
  {
    m_scene_updater->UpdateAnim(scene_context);
  }

  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE )
  {
    UpdateParticle(scene_calculate_group);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    UpdateSceneにかかった時間をログ出力
 */
//-----------------------------------------------------------------------------
void SceneSystem::DebugUpdateSceneTickOutPut(void)
{
#if GFL_DEBUG
  GFL_DEBUG_TICK_DUMP_ALL( SCENESYSTEM );
  GFL_DEBUG_TICK_RESET( SCENESYSTEM );
#endif  // GFL_DEBUG
}

//-----------------------------------------------------------------------------
/**
 *  @brief         パーティクルのノードに関するCalculateだけ行う(SceneUpdater部分だけ行って、ParticleSceneUpdater部分は行わない)
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateParticleNode(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle)
{
  UpdateParticleNodeBeforeCulling(scene_calculate_group, scene_calculate_group_enable_bit_flag, particle);
  UpdateParticleNodeAfterCulling(scene_calculate_group, scene_calculate_group_enable_bit_flag, particle);
}
void SceneSystem::UpdateParticleNodeBeforeCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();
 
  // SceneUpdater::EvaluateAnim  // AnimatableNode
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
  {
    nw::gfx::SceneNodeChildren::iterator nodeEnd = particle->GetNwSceneNode()->GetChildEnd();
    for(nw::gfx::SceneNodeChildren::iterator node = particle->GetNwSceneNode()->GetChildBegin();
        node != nodeEnd; ++node)
    {
      if( (*node)->GetResSceneNode().GetTypeInfo() == nw::gfx::ResParticleModel::TYPE_INFO
          && (*node)->GetAnimBinding() )
      {
        (*node)->GetAnimBinding()->Evaluate(nw::anim::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
      }
    }
  }

  // SceneUpdater::UpdateTransformNode  // SceneNode
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
  {
    {
      nw::gfx::SceneNode* node = particle->GetNwSceneNode();
      (node)->PreUpdateSignal()(node, scene_context);
      (node)->InheritTraversalResults();
      (node)->UpdateTransform(m_world_matrix_updater, scene_context);
    }

    nw::gfx::SceneNodeChildren::iterator nodeEnd = particle->GetNwSceneNode()->GetChildEnd();
    for(nw::gfx::SceneNodeChildren::iterator node = particle->GetNwSceneNode()->GetChildBegin();
        node != nodeEnd; ++node)
    {
      (*node)->PreUpdateSignal()(*node, scene_context);
      (*node)->InheritTraversalResults();
      (*node)->UpdateTransform(m_world_matrix_updater, scene_context);
    }
  }
}
void SceneSystem::UpdateParticleNodeAfterCulling(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag, Particle* particle)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();

  // SceneUpdater::EvaluateAnim  // AnimatableNode
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
  {
    nw::gfx::SceneNodeChildren::iterator nodeEnd = particle->GetNwSceneNode()->GetChildEnd();
    for(nw::gfx::SceneNodeChildren::iterator node = particle->GetNwSceneNode()->GetChildBegin();
        node != nodeEnd; ++node)
    {
      if( (*node)->GetResSceneNode().GetTypeInfo() == nw::gfx::ResParticleModel::TYPE_INFO
          && (*node)->GetAnimBinding() )
      {
        (*node)->GetAnimBinding()->Evaluate(nw::anim::ResGraphicsAnimGroup::EVALUATION_AFTER_SCENE_CULLING);
      }
    }
  }

  // SceneUpdater::UpdateSkeletalModel  // SkeletalModel
  // パーティクルの子ノードにスケルタルモデルはない
  //if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
  //{
  //}

  // SceneUpdater::UpdateAnim  // AnimatableNode
  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM )
  {
    nw::gfx::SceneNodeChildren::iterator nodeEnd = particle->GetNwSceneNode()->GetChildEnd();
    for(nw::gfx::SceneNodeChildren::iterator node = particle->GetNwSceneNode()->GetChildBegin();
        node != nodeEnd; ++node)
    {
      if( (*node)->GetResSceneNode().GetTypeInfo() == nw::gfx::ResParticleModel::TYPE_INFO
          && (*node)->GetAnimBinding() )
      {
        (*node)->UpdateFrame();
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         パーティクルの更新(ParticleSceneUpdater部分)
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateParticle(SceneCalculateGroup* scene_calculate_group)
{
#if 1  // @note 旧システムとかちあうところ→かちあうとNW4Cのメッセージがたくさん出力されるが今はかちあっていない？
  nw::gfx::SceneContext*    scene_context    = scene_calculate_group->GetSceneContext();
  nw::gfx::ParticleContext* particle_context = scene_calculate_group->GetParticleContext();
  b32 particle_enable_swap_buffer = scene_calculate_group->GetParticleEnableSwapBuffer();

  m_particle_scene_updater->UpdateNode(
      scene_context,
      particle_context,
      particle_enable_swap_buffer//m_particle_enable_swap_buffer//Sceneから変数で与えることにした。true//いや下記コメントから判断するとやはりtrueにすべきだfalse//ParticleContext particle_contextのUseDoubleBufferのデフォルトがfalseだから取り敢えずfalseに。true  // @note ここのtrue/falseを自動で切り替えられるようにする必要がある→SceneCalculateGroupから指定してもらうようにした。

#if 0
/*シーンコンテキストをビューに基づいて更新します。

指定されたシーンコンテキストに含まれるエミッタとパーティクルモデルの更新を行います。

enableSwapBufferをfalseにすると、内部でバッファのスワップを行いません。通常はtrueにしてください。アプリケーション側でコマンドバッファをダブルにしていて、同一フレームを描画中に複数回UpdateNodeを呼ぶ場合（つまりコマ落ち処理）は、同一フレームの2回目以降の呼び出しではfalseにしてください。

詳しくはParticleDropFrameDemoを参照してください。*/
#endif

  );

  // パーティクルセットが存在する場合
  nw::gfx::ParticleSetArray::iterator begin = scene_context->GetParticleSetBegin();
  nw::gfx::ParticleSetArray::iterator end   = scene_context->GetParticleSetEnd();
  if( begin != end )
  {
    scene_calculate_group->SetParticleEnableSwapBuffer(false);
  }
#if 0
/*@note
シーンに何かを追加→シーンを初期化→シーンの計算
というのを同一フレームで何回もやっているところがある。
最初の1回にパーティクルが入っているとは限らないため、最初の1回でm_particle_enable_swap_bufferをfalseにするわけにはいかない。
gfx_ParticleSet.h:415 Warning:detected an illegal order(Update without SwapBuffer Prev:0)
それに対する策としてパーティクルセットが存在しないときはtrueからfalseにできないようにした。
シーンコンテキストの複数持ちはOKだが、パーティクルコンテキストの複数持ちに関してはOKかNGか明言されていないが、
調べた感じでは大丈夫そうだった（いつパーティクルセットがつくられているかに拠りそうだったが）。*/
#endif

#if 0
/*@note
■SceneDrawGroupの改造
SceneDrawGroupそれぞれにSceneCalculateGroupの個数だけ
ビットを持たせる。
SubmitViewでパーティクルを追加したら、
そのパーティクルが所属していたSceneCalculateGroupの番号のビットを立てる。
SceneDrawGroupをDrawしたらSceneCalculateGroupのビットが立っているところの
ParticleEnableSwapBufferをtrueにし、SceneCalculateGroupのビットを落とす。
今はSceneのDrawで必ず全SceneCalculateGroupのをtrueにしているが、
上述のように改造するのが望ましい。

■SceneCalculateGroupの改造
パーティクルセットの有無を見ているが、そうではなく
gfl::grp::g3dのパーティクルクラスの有無を見るようにしよう。*/
#endif

#endif
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

