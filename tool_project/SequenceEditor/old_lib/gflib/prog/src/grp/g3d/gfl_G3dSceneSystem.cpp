//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneSystem.cpp
 *	@brief  3D�V�[���V�X�e��
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
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�V�[���V�X�e���N���X
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *
 *  @param[in]     allocator    �A���P�[�^
 *  @param[in]     description  �V�[���V�X�e���̐ݒ���e
 *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
 *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
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

  nw::gfx::WorldMatrixUpdater* world_matrix_updater = nw::gfx::WorldMatrixUpdater::Builder()   // SceneUpdater��j������ۂɈꏏ�ɔj�������
    .Create(allocator);
  m_world_matrix_updater = world_matrix_updater;
  nw::gfx::SkeletonUpdater* skeleton_updater = nw::gfx::SkeletonUpdater::Builder()             // SceneUpdater��j������ۂɈꏏ�ɔj�������
    .Create(allocator);
  m_skeleton_updater = skeleton_updater;
  nw::gfx::BillboardUpdater* billboard_updater = nw::gfx::BillboardUpdater::Create(allocator);  // SceneUpdater��j������ۂɈꏏ�ɔj�������
  m_billboard_updater = billboard_updater;
  m_scene_updater = nw::gfx::SceneUpdater::Builder()
    .WorldMatrixUpdaterPtr(world_matrix_updater)
    .SkeletonUpdaterPtr(skeleton_updater)
    .BillboardUpdaterPtr(billboard_updater)
    .Create(allocator);

#if 1
  // g3d��p�̃}�e���A��ID�����N���X
  nw::gfx::IMaterialIdGenerator* material_id_generator = gfl::grp::g3d::SortingMaterialIdGenerator::Builder()  // SceneInitializer��j������ۂɈꏏ�ɔj�������
    .IsFixedSizeMemory(l_description.is_fixed_size_memory)
    .MaxMaterials(l_description.max_materials)
    .Create(allocator);
#else
  // �f�t�H���g�̃}�e���A��ID�����N���X
  nw::gfx::IMaterialIdGenerator* material_id_generator = nw::gfx::SortingMaterialIdGenerator::Builder()  // SceneInitializer��j������ۂɈꏏ�ɔj�������
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
 *  @brief         �f�X�g���N�^
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
 *  @brief         �V�[���̏�����
 *
 *  @param[in]     scene_root                ����������V�[���̃��[�g�m�[�h
 *  @param[in]     scene_initialize_process  �����������̓��e
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
 *  @brief         ���[�g�m�[�h�̃g���o�[�X
 *
 *  @param[in]     scene_root   �Ώۂ̃��[�g�m�[�h
 */
//-----------------------------------------------------------------------------
void SceneSystem::TraverseScene(SceneCalculateGroup* scene_calculate_group)
{
  nw::gfx::SceneContext*  scene_context = scene_calculate_group->GetSceneContext();
  nw::gfx::TransformNode* scene_root    = scene_calculate_group->GetSceneRoot();

  m_scene_traverser->Begin(scene_context);
  scene_root->Accept(m_scene_traverser);
  m_scene_traverser->End();

  // �p�[�e�B�N���Z�b�g�̈ˑ����Ń\�[�g����
  gfl::std::Sort(
    scene_context->GetParticleSetBegin(),
    scene_context->GetParticleSetEnd(),
    nw::gfx::ParticleSetCompare());
}
 
#if GFL_DEBUG

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���̍X�V
 *  �f�o�b�O��
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

    // @note �����A�`��J�����O���s���ꍇ�͂����ŏ�������

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
 *  @brief         �V�[���̍X�V
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateScene(SceneCalculateGroup* scene_calculate_group, bit32 scene_calculate_group_enable_bit_flag)
{
  nw::gfx::SceneContext* scene_context = scene_calculate_group->GetSceneContext();
  //m_scene_updater->UpdateAll(scene_context);

  
  // m_scene_updater->UpdateAll(scene_context)�̒��g���ʂɌĂԂ��Ƃɂ����B
  // ��������
    //NW_NULL_ASSERT(sceneContext);

    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
    {
      m_scene_updater->EvaluateAnim(scene_context, nw::anim::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
    }
    if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
    {
      m_scene_updater->UpdateTransformNode(scene_context);
    }
    // @note �����A�`��J�����O���s���ꍇ�͂����ŏ�������
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
  // �����܂�


  if( scene_calculate_group_enable_bit_flag & Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_PARTICLE )
  {
    UpdateParticle(scene_calculate_group);
  }
}

#endif  // GFL_DEBUG

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���̍X�V�i�J�����O�O�j
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
 *  @brief         �V�[���̍X�V�i�J�����O��j
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
 *	@brief    UpdateScene�ɂ����������Ԃ����O�o��
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
 *  @brief         �p�[�e�B�N���̃m�[�h�Ɋւ���Calculate�����s��(SceneUpdater���������s���āAParticleSceneUpdater�����͍s��Ȃ�)
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
  // �p�[�e�B�N���̎q�m�[�h�ɃX�P���^�����f���͂Ȃ�
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
 *  @brief         �p�[�e�B�N���̍X�V(ParticleSceneUpdater����)
 */
//-----------------------------------------------------------------------------
void SceneSystem::UpdateParticle(SceneCalculateGroup* scene_calculate_group)
{
#if 1  // @note ���V�X�e���Ƃ��������Ƃ��끨����������NW4C�̃��b�Z�[�W����������o�͂���邪���͂��������Ă��Ȃ��H
  nw::gfx::SceneContext*    scene_context    = scene_calculate_group->GetSceneContext();
  nw::gfx::ParticleContext* particle_context = scene_calculate_group->GetParticleContext();
  b32 particle_enable_swap_buffer = scene_calculate_group->GetParticleEnableSwapBuffer();

  m_particle_scene_updater->UpdateNode(
      scene_context,
      particle_context,
      particle_enable_swap_buffer//m_particle_enable_swap_buffer//Scene����ϐ��ŗ^���邱�Ƃɂ����Btrue//���≺�L�R�����g���画�f����Ƃ�͂�true�ɂ��ׂ���false//ParticleContext particle_context��UseDoubleBuffer�̃f�t�H���g��false�������芸����false�ɁBtrue  // @note ������true/false�������Ő؂�ւ�����悤�ɂ���K�v�����遨SceneCalculateGroup����w�肵�Ă��炤�悤�ɂ����B

#if 0
/*�V�[���R���e�L�X�g���r���[�Ɋ�Â��čX�V���܂��B

�w�肳�ꂽ�V�[���R���e�L�X�g�Ɋ܂܂��G�~�b�^�ƃp�[�e�B�N�����f���̍X�V���s���܂��B

enableSwapBuffer��false�ɂ���ƁA�����Ńo�b�t�@�̃X���b�v���s���܂���B�ʏ��true�ɂ��Ă��������B�A�v���P�[�V�������ŃR�}���h�o�b�t�@���_�u���ɂ��Ă��āA����t���[����`�撆�ɕ�����UpdateNode���Ăԏꍇ�i�܂�R�}���������j�́A����t���[����2��ڈȍ~�̌Ăяo���ł�false�ɂ��Ă��������B

�ڂ�����ParticleDropFrameDemo���Q�Ƃ��Ă��������B*/
#endif

  );

  // �p�[�e�B�N���Z�b�g�����݂���ꍇ
  nw::gfx::ParticleSetArray::iterator begin = scene_context->GetParticleSetBegin();
  nw::gfx::ParticleSetArray::iterator end   = scene_context->GetParticleSetEnd();
  if( begin != end )
  {
    scene_calculate_group->SetParticleEnableSwapBuffer(false);
  }
#if 0
/*@note
�V�[���ɉ�����ǉ����V�[�������������V�[���̌v�Z
�Ƃ����̂𓯈�t���[���ŉ��������Ă���Ƃ��낪����B
�ŏ���1��Ƀp�[�e�B�N���������Ă���Ƃ͌���Ȃ����߁A�ŏ���1���m_particle_enable_swap_buffer��false�ɂ���킯�ɂ͂����Ȃ��B
gfx_ParticleSet.h:415 Warning:detected an illegal order(Update without SwapBuffer Prev:0)
����ɑ΂����Ƃ��ăp�[�e�B�N���Z�b�g�����݂��Ȃ��Ƃ���true����false�ɂł��Ȃ��悤�ɂ����B
�V�[���R���e�L�X�g�̕���������OK�����A�p�[�e�B�N���R���e�L�X�g�̕��������Ɋւ��Ă�OK��NG����������Ă��Ȃ����A
���ׂ������ł͑��v�����������i���p�[�e�B�N���Z�b�g�������Ă��邩�ɋ��肻�����������j�B*/
#endif

#if 0
/*@note
��SceneDrawGroup�̉���
SceneDrawGroup���ꂼ���SceneCalculateGroup�̌�����
�r�b�g����������B
SubmitView�Ńp�[�e�B�N����ǉ�������A
���̃p�[�e�B�N�����������Ă���SceneCalculateGroup�̔ԍ��̃r�b�g�𗧂Ă�B
SceneDrawGroup��Draw������SceneCalculateGroup�̃r�b�g�������Ă���Ƃ����
ParticleEnableSwapBuffer��true�ɂ��ASceneCalculateGroup�̃r�b�g�𗎂Ƃ��B
����Scene��Draw�ŕK���SSceneCalculateGroup�̂�true�ɂ��Ă��邪�A
��q�̂悤�ɉ�������̂��]�܂����B

��SceneCalculateGroup�̉���
�p�[�e�B�N���Z�b�g�̗L�������Ă��邪�A�����ł͂Ȃ�
gfl::grp::g3d�̃p�[�e�B�N���N���X�̗L��������悤�ɂ��悤�B*/
#endif

#endif
}

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

