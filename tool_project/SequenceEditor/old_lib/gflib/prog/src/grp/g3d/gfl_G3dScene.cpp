//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dScene.cpp
 *	@brief  3D�V�X�e��
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// include
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Base.h>
#include <gfl_debug.h>

// gflib grp
#include <grp/gfl_GraphicsSystem.h>
#include <grp/gfl_FrameBufferManager.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSceneSystem.h>
#include <grp/g3d/gfl_G3dRenderSystem.h>
#include <grp/g3d/gfl_G3dShadowSystem.h>
#include <grp/g3d/gfl_G3dParticle.h>
#include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dLight.h>
#include <grp/g3d/gfl_G3dLightSet.h>
#include <grp/g3d/gfl_G3dFog.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dResource.h>

#include <grp/g3d/gfl_G3dScene.h>

// gflib grp g3d ����J�w�b�_
#include "gfl_G3dFakeRenderQueue.h"




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// define
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GFL_G3D_SCENE_GROUP_ASSERT_LEVEL ((GFL_DEBUG) && (0))  // 0=�������Ȃ��A1=���b�Z�[�W���o���A2=GFL_ASSERT(GFL_ASSERT_MSG)���Ă�  @check �����łЂ�������Ȃ��悤�Ɋm�F�ς̈������



namespace gfl {
namespace grp {
namespace g3d {


f32         Scene::s_CullBoxScale = 1.1f;

// �f�o�b�O
GFL_DEBUG_TICK_SETUP(GFL_G3DSCENE, 15);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// extern�֐���`
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         ���L�҂�ݒ肷��
 *                 ���L�҂�ID��ݒ肷��
 *                 class Object�̃t�����h�֐�
 *
 *  @param[in]     object       �I�u�W�F�N�g 
 *  @param[in]     a_owner      �I�u�W�F�N�g�̏��L��
 *  @param[in]     a_owner_id   ���L�҂�ID
 */
//-----------------------------------------------------------------------------
void Object_SetOwnerAndOwnerId(Object* object, ObjectOwner a_owner, s32 a_owner_id)
{
  object->m_owner    = a_owner;
  object->m_owner_id = a_owner_id;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[�U���C�ӂŕt������ID��ݒ肷��(�����I)
 *                 ���[�U���Ǘ����Ă��Ȃ��I�u�W�F�N�g�ł��A���[�U���C�ӂŕt������ID��ݒ�ł���B
 *
 *  @param[in]     object     �I�u�W�F�N�g 
 *  @param[in]     a_user_id  ���[�U���C�ӂŕt������ID
 */
//-----------------------------------------------------------------------------
void Object_SetUserIdForcedly(Object* object, s32 a_user_id)
{
  object->m_user_id = a_user_id;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//################################################################################################################
// 
// 
// �N���X��`
// 
// 
//################################################################################################################
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------------------
///	3D�V�[���N���X
//=====================================

////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// 
// ���Jstatic�֐�
// 
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/* 
 * @brief �J�����O����X�P�[��
 */
//-----------------------------------------------------------------------------
void Scene::SetViewCullBoxScale( f32 scale )
{
  s_CullBoxScale = scale;
}

//-----------------------------------------------------------------------------
/* 
 * @brief ���f���̎���J�����O
 */
//-----------------------------------------------------------------------------
bool Scene::ViewCullingModel( const Model &r_model, const Camera &r_camera )
{
  gfl::math::AABB   modelAABB;  
  nn::math::MTX34   worldMat;
  
  if( r_model.GetModelAABB( &modelAABB ) )
  {
    r_model.GetWorldMatrix( &worldMat );
    
    return ViewCullingAABB( modelAABB, r_camera, worldMat );
  }

  return true;  // �����̂ŁA�`�悷��B
}

//-----------------------------------------------------------------------------
/* 
 * @brief AABB�̎���J�����O
 */
//-----------------------------------------------------------------------------
bool Scene::ViewCullingAABB( const gfl::math::AABB &aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat )
{
  float              area = 1.0f * s_CullBoxScale;
  static const gfl::math::AABB    clip( gfl::math::VEC4(-area, -area, -area, 0.0f), 
                                        gfl::math::VEC4( area,  area,  area, 0.0f) );
  gfl::math::AABB                 plane;
    
 Scene::GetScreanCoordinatesAABB( &plane, aabb, r_camera, worldMat );

  // �X�N���[���Ƃ̌�������
  if( plane.IsCross( clip ) ){
    return true;
  }
  
  return false;
}

//-----------------------------------------------------------------------------
/* 
 * @brief �X�N���[�����W�n�ł̂`�`�a�a�擾
 */
//-----------------------------------------------------------------------------
void Scene::GetScreanCoordinatesAABB( gfl::math::AABB *pDestAABB, const gfl::math::AABB &r_src_aabb, const Camera &r_camera, const nn::math::MTX34 &worldMat )
{
  nn::math::MTX44   worldViewProjMat;
  {
    nn::math::MTX34 viewMat;

    r_camera.GetProjectionMatrix( &worldViewProjMat );
    r_camera.GetViewMatrix( &viewMat );

    worldViewProjMat *= nn::math::MTX44( viewMat );
    worldViewProjMat *= nn::math::MTX44( worldMat );
  }
  
  nn::math::VEC3    pos;
  gfl::math::VEC4   minPos;
  gfl::math::VEC4   maxPos;
  
  for( u32 i=0; i<2; ++i )
  {  
    if( i==0 ){
      pos.x = r_src_aabb.GetMin().x;
    }else{
      pos.x = r_src_aabb.GetMax().x;
    }
    
    for( u32 j=0; j<2; ++j )
    {  
      if( j==0 ){
        pos.y = r_src_aabb.GetMin().y;
      }else{
        pos.y = r_src_aabb.GetMax().y;
      }
      
      for( u32 k=0; k<2; ++k )
      {  
        if( k==0 ){
          pos.z = r_src_aabb.GetMin().z;
        }else{
          pos.z = r_src_aabb.GetMax().z;
        }
        
        // ���_�̍ő�l�A�ŏ��l�����߂�B
        nn::math::VEC4 ans;
        nn::math::VEC3Transform( &ans, &worldViewProjMat, &pos );
        f32     length = ans.w;
        
        if ( length > 0.0f )
        {
          ans.x /= length;
          ans.y /= length;
          ans.z /= length;
        }
        else
        {
          length = -length + 1.0f;
          ans.x /= length;
          ans.y /= length;
          ans.z /= length; 
        }
        
        // ������
        if( (i==j) && (j==k) && (i==0) ){
          minPos= ans;
          maxPos = ans;
        }else{
          minPos.Minimize( minPos, ans );
          maxPos.Maximize( maxPos, ans );
        }
      }
    }
  }
  
  // �����`�F�b�N����ʏ��
  pDestAABB->SetMinMax( minPos, maxPos );
}

//-----------------------------------------------------------------------------
/* 
 * @brief �X�N���[�����W�n�ł̃��f���`�`�a�a�擾
 */
//-----------------------------------------------------------------------------
bool Scene::GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const Model &r_model, const Camera &r_camera )
{
  gfl::math::AABB   modelAABB;
  nn::math::MTX34   worldMat;
  
  if( r_model.GetModelAABB( &modelAABB ) ){
    
    r_model.GetWorldMatrix( &worldMat );
    
    Scene::GetScreanCoordinatesAABB( p_dest_aabb, modelAABB, r_camera, worldMat );
    return true;
  }

  return false;
}

bool Scene::GetScreanCoordinatesModelAABB( gfl::math::AABB *p_dest_aabb, const H3dModel &r_h3d_model, const Camera &r_camera )
{
  gfl::math::AABB   modelAABB;
  nn::math::MTX34   worldMat;
  
  if( r_h3d_model.GetModelAABB( &modelAABB ) ){
    
    r_h3d_model.GetWorldMatrix( &worldMat );
    
    Scene::GetScreanCoordinatesAABB( p_dest_aabb, modelAABB, r_camera, worldMat );
    return true;
  }

  return false;
}


#if GFL_GRP_G3D_H3D_USE
//-----------------------------------------------------------------------------
/* 
 * @brief ���f���̎���J�����O
 */
//-----------------------------------------------------------------------------
bool Scene::ViewCullingH3dModel( const H3dModel &r_model, const Camera &r_camera )
{
  // �����̎������肢���܂�
  return true;
}
#endif  // GFL_GRP_G3D_H3D_USE


////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
// 
// ���J�֐�
// 
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �R���X�g���N�^ / �f�X�g���N�^
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
Scene::Scene(void)
  : m_scene_calculate_group_num(0),
    m_scene_draw_group_num(0),
    m_scene_calculate_group_pack_array(NULL),
    m_model_node_property_function_work(NULL),
#if GFL_GRP_G3D_H3D_USE
    m_h3d_model_property_function_work(NULL),
#endif  // GFL_GRP_G3D_H3D_USE
    m_scene_draw_group_pack_array(NULL),
    m_render_queue_TEMP(NULL),
    m_scene_own_object_pack(NULL),
    m_scene_system(NULL),
    m_render_system(NULL),
    m_shadow_system(NULL),
    m_g3d_edge_map_system(NULL),
    m_g3d_post_effect_system(NULL),
    m_G3DDepthOfFieldSystem(NULL)
#if GFL_DEBUG
    , m_debug_tick_enable_flag(false),
    m_debug_tick_dump_and_reset_flag(false)
#endif
{
  GFL_SINGLETON_INSTANCE(System)->GetSceneManager()->AddObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create���Ăт܂��B
 *
 *  @param[in]     device_allocator    �f�o�C�X�A���P�[�^
 *  @param[in]     description  �V�[���̐ݒ���e
 *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
 *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
 *  @param[in]     heap_allocator      �q�[�v�A���P�[�^(NULL�̂Ƃ���device_allocator���g�p���܂�)
 */
//-----------------------------------------------------------------------------
Scene::Scene( gfl::heap::NwAllocator* device_allocator, const Description* description, gfl::heap::NwAllocator* heap_allocator )
  : m_scene_calculate_group_num(0),
    m_scene_draw_group_num(0),
    m_scene_calculate_group_pack_array(NULL),
    m_model_node_property_function_work(NULL),
#if GFL_GRP_G3D_H3D_USE
    m_h3d_model_property_function_work(NULL),
#endif  // GFL_GRP_G3D_H3D_USE
    m_scene_draw_group_pack_array(NULL),
    m_render_queue_TEMP(NULL),
    m_scene_own_object_pack(NULL),
    m_scene_system(NULL),
    m_render_system(NULL),
    m_shadow_system(NULL),
    m_g3d_edge_map_system(NULL),
    m_g3d_post_effect_system(NULL),
    m_G3DDepthOfFieldSystem(NULL)
#if GFL_DEBUG
    , m_debug_tick_enable_flag(false),
    m_debug_tick_dump_and_reset_flag(false)
#endif
{
  GFL_SINGLETON_INSTANCE(System)->GetSceneManager()->AddObject(this);
  
  Create(device_allocator, description, heap_allocator);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
Scene::~Scene()
{
  Destroy();

  GFL_SINGLETON_INSTANCE(System)->GetSceneManager()->RemoveObject(this);
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �f�o�b�O�@�\
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void Scene::DebugTickSetEnableFlag(b32 flag)  // true�̂Ƃ��v������Afalse�̂Ƃ��v���Ȃ�
{
#if GFL_DEBUG
  m_debug_tick_enable_flag = flag;
  GFL_DEBUG_TICK_SET_FLAG(GFL_G3DSCENE, m_debug_tick_enable_flag);
#endif
}
void Scene::DebugTickSetDumpAndResetFlag(b32 flag)  // true�̂Ƃ��_���v�����Z�b�g����Afalse�̂Ƃ��Ȃ�
{
#if GFL_DEBUG
  m_debug_tick_dump_and_reset_flag = flag;
#endif
}
b32  Scene::DebugTickGetDumpAndResetFlag(void)
{
#if GFL_DEBUG
  return m_debug_tick_dump_and_reset_flag;
#else
  return false;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief    UpdateScene�ɂ����������Ԃ����O�o��
 */
//-----------------------------------------------------------------------------
void Scene::DebugUpdateSceneTickOutPut(void)
{
#if GFL_DEBUG
  m_scene_system->DebugUpdateSceneTickOutPut();
#endif  // GFL_DEBUG
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// ���� / �j��
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     device_allocator    �f�o�C�X�A���P�[�^
 *  @param[in]     description  �V�[���̐ݒ���e
 *                              (NULL�̂Ƃ��̓f�t�H���g�l�B
 *                              �Ăяo�����͂��̊֐�����߂��Ă�����description�Ƃ��ēn�������̂��폜���Ă悢)
 *  @param[in]     heap_allocator      �q�[�v�A���P�[�^(NULL�̂Ƃ���device_allocator���g�p���܂�)
 */
//-----------------------------------------------------------------------------
void Scene::Create(gfl::heap::NwAllocator* device_allocator, const Description* description, gfl::heap::NwAllocator* heap_allocator )
{
  // ������
  gfl::heap::HeapBase*    l_heap_memory      = NULL;
  gfl::heap::HeapBase*    l_device_memory    = NULL;
  gfl::heap::NwAllocator* l_heap_allocator   = NULL;
  gfl::heap::NwAllocator* l_device_allocator = NULL;
 
  if( heap_allocator )
  {
    l_heap_memory    = heap_allocator->GetHeapBase();
    l_heap_allocator = heap_allocator;
  }
  else
  {
    l_heap_memory    = device_allocator->GetHeapBase();
    l_heap_allocator = device_allocator;
  }
  l_device_memory    = device_allocator->GetHeapBase();
  l_device_allocator = device_allocator;

  // �f�B�X�N���v�V����
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  // �K�{�V�X�e��
  m_scene_system  = GFL_NEW(l_heap_memory) gfl::grp::g3d::SceneSystem(l_device_allocator, l_description.scene_system_description);
  m_render_system = GFL_NEW(l_heap_memory) gfl::grp::g3d::RenderSystem(l_heap_allocator, l_device_allocator, l_description.render_system_description);

  createSceneCalculateGroupPack(l_heap_allocator, l_device_allocator, &l_description);
  createSceneDrawGroupPack(l_heap_allocator, l_device_allocator, &l_description);
  createSceneOwnObjectPack(l_heap_allocator, l_device_allocator, &l_description);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void Scene::Destroy(void)
{
  RemoveAllNodeFromAllCalcAndAllDraw();  // ������Ă�����Ȃ�Đe�؂������ɁA������Ă��Ȃ����̂���������ASSERT�ɂ����ق������������B
#if GFL_GRP_G3D_H3D_USE
  RemoveAllH3dModelFromAllCalcAndAllDraw();  // @note �����RemoveAllNodeFromAllCalcAndAllDraw����ĂԂ悤�ɂ����ق����������낤�B
#endif  // GFL_GRP_G3D_H3D_USE

  DestroyDepthOfFieldSystem();
  DestroyPostEffectSystem();
  DestroyEdgeMapSystem();
  DestroyShadowSystem();

  DestroyOwnObject();

  destroySceneOwnObjectPack();
  destroySceneDrawGroupPack();
  destroySceneCalculateGroupPack();

  // �K�{�V�X�e��
  if( m_render_system )
  {
    GFL_DELETE m_render_system;
    m_render_system = NULL;
  }
  if( m_scene_system )
  {
    GFL_DELETE m_scene_system;
    m_scene_system = NULL;
  }

  // �R���X�g���N�^�Ɠ���������
  constructorInitialize();
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Initialize
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         ����������
 *                 (�I�������͂Ȃ�)
 */
//-----------------------------------------------------------------------------
void Scene::Initialize(const SceneInitializeProcess scene_initialize_process)
{ 
  GFL_ASSERT_MSG(scene_initialize_process>=SCENE_INITIALIZE_PROCESS_NONE, "SceneInitializeProcess�̒l%d���ُ�ł��B\n", scene_initialize_process);  //@check

  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    Initialize(i, scene_initialize_process);  // �O������Ă΂�鏉��������
  }
}

void Scene::Initialize(s32 scene_calculate_group_index, const SceneInitializeProcess scene_initialize_process)
{
  SceneCalculateGroup* calc_group = m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group;

#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
  if( calc_group->GetSceneInitializeProcessNeed() == SCENE_INITIALIZE_PROCESS_NONE )
  {
    GFL_PRINT("Scene��Initialize�֐����ĂԕK�v���i�C�J������Ȃ��Ƃ��ɌĂ΂�܂����B(%d)\n", scene_calculate_group_index);
    GFL_PRINT("Add or Remove�����Ă��Ȃ��Ă��A���f���̃}�e���A����ύX���Ă���ꍇ�͌ĂԕK�v���A���J������܂���B\n");
  }
#endif

  initializeInternal(calc_group, scene_initialize_process);  // �����Ŏg�p���鏉��������
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �v�Z����
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����
 */
//-----------------------------------------------------------------------------
void Scene::Calculate(void)  // �S��(�p�[�e�B�N�����܂�)�̌v�Z�������s��
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    Calculate(i);
  }
}

void Scene::Calculate(s32 scene_calculate_group_index)  // scene_calculate_group_index�ɂ���S��(�p�[�e�B�N�����܂�)�̌v�Z�������s��
{
  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);
  bit32                    calc_bit_flag = calc_pack->scene_calculate_group_enable_bit_flag;
  SceneInitializeProcess   init_process  = calc_group->GetSceneInitializeProcessNeed();

  if( init_process != SCENE_INITIALIZE_PROCESS_NONE )
  {
    initializeInternal(calc_group, init_process);  // �����Ŏg�p���鏉��������
#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
    GFL_PRINT("[Scene::Calculate(%d)] �V�[���c���[�ɕύX���������̂ŁA���Initizalize���Ăт܂����B\n", scene_calculate_group_index);
    GFL_PRINT("�V�[���c���[��ύX������Initialize������Calculate���������ꍇ�́A�V�X�e���v���O���}�ɑ��k���ĉ������B\n");
#endif
#if (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2)
    GFL_ASSERT(0);
#endif
  }

  // �A�j���[�V�����A�m�[�h�A�X�P���g���A�J�����̍X�V�������s���܂��B
  updateObjectBeforeCulling(calc_pack);
  m_scene_system->UpdateSceneBeforeCulling(calc_group, calc_bit_flag);
  updateObjectAfterCulling(calc_pack);
  m_scene_system->UpdateSceneAfterCulling(calc_group, calc_bit_flag);
 
#if GFL_GRP_G3D_H3D_USE
  calculateH3dModelBeforeCulling(scene_calculate_group_index, calc_bit_flag);
  calculateH3dModelAfterCulling(scene_calculate_group_index, calc_bit_flag);
#endif

  calc_group->SetState(SceneCalculateGroup::STATE_AFTER_CALCULATE);
}

// �p�[�e�B�N���̃m�[�h�Ɋւ���Calculate�����s��(SceneUpdater���������s���āAParticleSceneUpdater�����͍s��Ȃ�)
void Scene::CalculateParticleNode(Particle* particle)  // particle��NULL�̂Ƃ�Scene�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    CalculateParticleNode(i, particle);
  }
}
void Scene::CalculateParticleNode(s32 scene_calculate_group_index, Particle* particle)  // particle��NULL�̂Ƃ�scene_calculate_group_index�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
{
  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);
  bit32                    calc_bit_flag = calc_pack->scene_calculate_group_enable_bit_flag;
  SceneInitializeProcess   init_process  = calc_group->GetSceneInitializeProcessNeed();

  if( init_process != SCENE_INITIALIZE_PROCESS_NONE )
  {
    initializeInternal(calc_group, init_process);  // �����Ŏg�p���鏉��������
#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
    GFL_PRINT("[Scene::Calculate(%d)] �V�[���c���[�ɕύX���������̂ŁA���Initizalize���Ăт܂����B\n", scene_calculate_group_index);
    GFL_PRINT("�V�[���c���[��ύX������Initialize������Calculate���������ꍇ�́A�V�X�e���v���O���}�ɑ��k���ĉ������B\n");
#endif
#if (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2)
    GFL_ASSERT(0);
#endif
  }

  if( particle )
  {
    // �w�肵���p�[�e�B�N������
    ModelNodePropertyElement* element = getParticleElement(particle, property_pack);
    if(element)
    {
      // �J�����O�̃`�F�b�N�͂��Ă��Ȃ�
      //if( IsBitFlagOnOne(property_pack->particle_array[index].bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TREE_FLAG) )  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
      {
        Particle* l_particle = reinterpret_cast<Particle*>(element->data.scene_node);
        l_particle->Calculate();  // BeforeCulling
        m_scene_system->UpdateParticleNodeBeforeCulling(calc_group, calc_bit_flag, l_particle);
                                  // AfterCulling  // ������邱�Ƃ��Ȃ�
        m_scene_system->UpdateParticleNodeAfterCulling(calc_group, calc_bit_flag, l_particle);
      }
    }
  }
  else
  {
    // �S�Ẵp�[�e�B�N��
    const ModelNodePropertyElement* sentinel = getParticleSentinelElement(property_pack);
    ModelNodePropertyElement*       element  = getParticleElementHead(property_pack);
    while(element != sentinel)
    {
      // �J�����O�̃`�F�b�N�͂��Ă��Ȃ�
      //if( IsBitFlagOnOne(property_pack->particle_array[i].bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TREE_FLAG) )  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
      {
        Particle* l_particle = reinterpret_cast<Particle*>(element->data.scene_node);
        l_particle->Calculate();  // BeforeCulling
        m_scene_system->UpdateParticleNodeBeforeCulling(calc_group, calc_bit_flag, l_particle);
                                  // AfterCulling  // ������邱�Ƃ��Ȃ�
        m_scene_system->UpdateParticleNodeAfterCulling(calc_group, calc_bit_flag, l_particle);
      }
      element = element->next;
    }
  }
}

void Scene::RepeatCalculateParticle(s32 repeat_count, Particle* particle)  // �p�[�e�B�N�������v�Z�������s��  // particle��NULL�̂Ƃ�Scene�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    RepeatCalculateParticle(repeat_count, i, particle);
  }
}
void Scene::RepeatCalculateParticle(s32 repeat_count, s32 scene_calculate_group_index, Particle* particle)  // �p�[�e�B�N�������v�Z�������s��  // particle��NULL�̂Ƃ�scene_calculate_group_index�ɓo�^����Ă���S�Ẵp�[�e�B�N���ɑ΂��čs��  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
{
  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);
  SceneInitializeProcess   init_process  = calc_group->GetSceneInitializeProcessNeed();

  if( init_process != SCENE_INITIALIZE_PROCESS_NONE )
  {
    initializeInternal(calc_group, init_process);  // �����Ŏg�p���鏉��������
#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
    GFL_PRINT("[Scene::Calculate(%d)] �V�[���c���[�ɕύX���������̂ŁA���Initizalize���Ăт܂����B\n", scene_calculate_group_index);
    GFL_PRINT("�V�[���c���[��ύX������Initialize������Calculate���������ꍇ�́A�V�X�e���v���O���}�ɑ��k���ĉ������B\n");
#endif
#if (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2)
    GFL_ASSERT(0);
#endif
  }

  if( particle )
  {
    // �w�肵���p�[�e�B�N������
    ModelNodePropertyElement* element = getParticleElement(particle, property_pack);
    if(element)
    {
      // �J�����O�̃`�F�b�N�͂��Ă��Ȃ�
      //if( IsBitFlagOnOne(property_pack->particle_array[index].bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TREE_FLAG) )  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
      {
        Particle* l_particle = reinterpret_cast<Particle*>(element->data.scene_node);
        // �w�萔�����t���[������
        l_particle->AddTimes(repeat_count, calc_group->GetParticleContext());
      }
    }
  }
  else
  {
    // �S�Ẵp�[�e�B�N��
    const ModelNodePropertyElement* sentinel = getParticleSentinelElement(property_pack);
    ModelNodePropertyElement*       element  = getParticleElementHead(property_pack);
    while(element != sentinel)
    {
      // �J�����O�̃`�F�b�N�͂��Ă��Ȃ�
      //if( IsBitFlagOnOne(property_pack->particle_array[i].bit_flag, SCENE_NODE_PROPERTY_BIT_FLAG_TREE_FLAG) )  // �V�[���c���[����O��Ă��Ă�(Particle�N���X�Ƀ��[�U���w�肵�ĊO��Ă���ꍇ���A�J�����O�ŊO��Ă���ꍇ��)�������s���܂��B
      {
        Particle* l_particle = reinterpret_cast<Particle*>(element->data.scene_node);
        // �w�萔�����t���[������
        l_particle->AddTimes(repeat_count, calc_group->GetParticleContext());
      }
      element = element->next;
    }
  }
}

void Scene::CalculateCulling(Camera* camera, bool update_camera_matrix)
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    calculateBeforeCulling(i);
  }

  if( update_camera_matrix )
  {
    camera->UpdateCameraMatrix();
  }

  culling(camera);

  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    calculateAfterCulling(i);
  }
}

void Scene::CalculateCulling(s32 scene_calculate_group_index, Camera* camera, bool update_camera_matrix)
{
  calculateBeforeCulling(scene_calculate_group_index);

  if( update_camera_matrix )
  {
    camera->UpdateCameraMatrix();
  }

  culling(scene_calculate_group_index, camera);

  calculateAfterCulling(scene_calculate_group_index);
}

void Scene::ResetCalculateCullingFlag(void)
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    ResetCalculateCullingFlag(i);
  }
}

void Scene::ResetCalculateCullingFlag(s32 scene_calculate_group_index)
{
  SceneNodePropertyPack* property_pack = &(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack);

  // ���f��
  resetCalculateCullingFlag(scene_calculate_group_index, property_pack->model_array);
  // �p�[�e�B�N��
  resetCalculateCullingFlag(scene_calculate_group_index, property_pack->particle_array);

#if GFL_GRP_G3D_H3D_USE
  resetH3dCalculateCullingFlag(scene_calculate_group_index, property_pack->h3d_model_array);
#endif
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
 *
 *  @param[in]  scene_calculate_group_enable_bit_flag_array  �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O�̔z��(�V�[���v�Z�O���[�v�S�Ă̕�)
 *
 *      bit32 bit_flag_array[calc_group_num] =
 *      {
 *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE,                       // calc_group_0
 *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE,      // calc_group_1
 *           gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
 *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
 *         | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM,            // calc_group_2
 *         gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT                     // calc_group_3
 *      };
 */
//-----------------------------------------------------------------------------
void Scene::SetSceneCalculateGroupEnableBitFlagArray(bit32* scene_calculate_group_enable_bit_flag_array)
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    SetSceneCalculateGroupEnableBitFlag(i, scene_calculate_group_enable_bit_flag_array[i]);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
 *
 *  @param[in]  scene_calculate_group_index            �V�[���v�Z�O���[�v
 *  @param[in]  scene_calculate_group_enable_bit_flag  �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O
 *                  ��
 *                  scene_calculate_group_enable_bit_flag = \
 *                        gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE \
 *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM           \
 *                      | gfl::grp::g3d::Scene::SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM
 */
//-----------------------------------------------------------------------------
void Scene::SetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag)
{
  if( 0<=scene_calculate_group_index&&scene_calculate_group_index<m_scene_calculate_group_num)
  {
    m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group_enable_bit_flag = scene_calculate_group_enable_bit_flag;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_calculate_group_index=%d, scene_calculate_group_num=%d\n", scene_calculate_group_index, m_scene_calculate_group_num);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O���擾����
 *
 *  @param[in]  scene_calculate_group_index            �V�[���v�Z�O���[�v
 *
 *  @retval     �V�[���v�Z�O���[�v�𐧌䂷��r�b�g�t���O
 */
//-----------------------------------------------------------------------------
bit32 Scene::GetSceneCalculateGroupEnableBitFlag(s32 scene_calculate_group_index) const
{
  bit32 ret = SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_NONE;
  if( 0<=scene_calculate_group_index&&scene_calculate_group_index<m_scene_calculate_group_num)
  {
    ret = m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group_enable_bit_flag;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_calculate_group_index=%d, scene_calculate_group_num=%d\n", scene_calculate_group_index, m_scene_calculate_group_num);
  }
  return ret;
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// SubmitView
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////    ��SubmitView����x�ɍs�����߂̊֐���    //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
/*
 *  @brief         �t�H�O�̍X�V���s��
 *
 *  @param[in]    camera    �����œn�����J��������̋����Ńt�H�O�̔Z���𒲐����܂�
 *
 *  �y���ӎ����z
 *      (1) �t�H�O�N���X�̊��N���X�ł���V�[���m�[�h�̍X�V�Ɋւ���
 *          �t�H�O�N���X�̊��N���X�ł���V�[���m�[�h�̍X�V��Calculate�ōs����̂ŁA���̊֐����Ăяo���O��Calculate�͍ς܂��Ă����ĉ������B
 *      (2) �J�����Ɋւ���
 *          �t�H�O�̍X�V�Ɏg���J�����̃J�����}�g���N�X�̍X�V�́A���̊֐����Ăяo���O�ɍς܂��Ă����ĉ������B(�J�����̃}�g���N�X�̍X�V�̑O�ɁACalculate���ς܂��Ă����K�v������܂��B)
 */
//-----------------------------------------------------------------------------
void Scene::UpdateFog(Camera* camera)
{
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[i]);
    nw::gfx::SceneContext*   scene_context = calc_pack->scene_calculate_group->GetSceneContext();

    // NW4C�t�H�O���X�V���܂��B
    {
      nw::gfx::FogArray::iterator fog_end = scene_context->GetFogEnd();
      for( nw::gfx::FogArray::iterator fog_i = scene_context->GetFogBegin(); fog_i != fog_end; ++fog_i )
      {
        if( *fog_i )
        {
          nw::gfx::Fog* nw_fog = static_cast<nw::gfx::Fog*>(*fog_i);
          nw_fog->Update(camera->GetNwCamera());
        }
      }
    }

    // g3d�t�H�O���X�V���܂��B
    {
      SceneNodePropertyPack*  property_pack  = &(calc_pack->scene_node_property_pack);
      const SceneNodePropertyElement* sentinel = getFogSentinelElement(property_pack);
      SceneNodePropertyElement*       element  = getFogElementHead(property_pack); 
      while(element != sentinel)
      {
        Fog* fog = reinterpret_cast<Fog*>(element->data.scene_node);
        fog->Update(camera);
        element = element->next;
      }
    }
  }


#if GFL_GRP_G3D_H3D_USE
  // @attention aaaaaaaaaaaaaaaaaaaaaaa ��芸�����V�[�������f�̃e�X�g�������ł��Ă݂�
  {
    // �V�[����
    m_render_system->SetH3dEnvironmentFromSceneEnvironment(*(camera->GetNwCamera()));
  }
#endif  // GFL_GRP_G3D_H3D_USE

}


//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�Ŏg�p����J������S�Đݒ肷��
 *
 *  @param[in]    scene_draw_group_camera_array    �V�[���`��O���[�v�̔z��ʂ�ɃJ������ݒ肵���z��(�V�[���`��O���[�v�S�Ă̕�)
 *                                                 
 *                                                 ��
 *                                                 Camera* camera_a;  // for draw_group_0, draw_group_1
 *                                                 Camera* camera_b;  // for draw_group_2
 *
 *                                                 Camera* camera_array[draw_group_num] =
 *                                                 {
 *                                                   camera_a,  // draw_group_0
 *                                                   camera_a,  // draw_group_1
 *                                                   camera_b   // draw_group_2
 *                                                 };
 *
 *  �y���ӎ����z
 *      (1) �����Őݒ肵���J�������g�p�����̂́E�E�E
 *          �����Őݒ肵���J�������g�p�����̂́ASubmitViewAll�ł݂̂ł��B
 *          �ݒ肵�Ă������J�������j������Ă�SubmitViewAll���Ă΂Ȃ���Εs��������邱�Ƃ͂���܂��񂪁A
 *          �ݒ肵�Ă������J������j������Ƃ��͈��S�̂���NULL��ݒ肷��悤�ɂ��ĉ������B
 */
//-----------------------------------------------------------------------------
void Scene::SetSubmitViewCameraArray(const Camera** scene_draw_group_camera_array)
{
  for( s16 i=0; i<m_scene_draw_group_num; ++i )
  {
    SetSubmitViewCamera(i, scene_draw_group_camera_array[i]);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�Ŏg�p����J������ݒ肷��
 *
 *  @param[in]    scene_draw_group_index    scene_draw_group_camera��ݒ肷��V�[���`��O���[�v
 *  @param[in]    scene_draw_group_camera   scene_draw_group_index�ɐݒ肷��J����
 *
 *  �y���ӎ����z
 *      (1) �����Őݒ肵���J�������g�p�����̂́E�E�E
 *          �����Őݒ肵���J�������g�p�����̂́ASubmitViewAll�ł݂̂ł��B
 *          �ݒ肵�Ă������J�������j������Ă�SubmitViewAll���Ă΂Ȃ���Εs��������邱�Ƃ͂���܂��񂪁A
 *          �ݒ肵�Ă������J������j������Ƃ��͈��S�̂���NULL��ݒ肷��悤�ɂ��ĉ������B
 */
//-----------------------------------------------------------------------------
void Scene::SetSubmitViewCamera(s32 scene_draw_group_index, const Camera* scene_draw_group_camera)
{
  if( 0<=scene_draw_group_index&&scene_draw_group_index<m_scene_draw_group_num )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group_camera = scene_draw_group_camera;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_draw_group_index=%d, scene_draw_group_num=%d\n", scene_draw_group_index, m_scene_draw_group_num);
  }
}

// �V�[���`��O���[�v�Ŏg�p����J�������擾����
const Camera* Scene::GetSubmitViewCamera(s32 scene_draw_group_index) const
{
  const Camera* ret = NULL;
  if( 0<=scene_draw_group_index&&scene_draw_group_index<m_scene_draw_group_num)
  {
    ret = m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group_camera;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_draw_group_index=%d, scene_draw_group_num=%d\n", scene_draw_group_index, m_scene_draw_group_num);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
 *
 *  @param[in]  scene_draw_group_enable_bit_flag_array  �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O�̔z��(�V�[���`��O���[�v�S�Ă̕�)
 *
 *      ��
 *      bit32 bit_flag_array[draw_group_num] =
 *      {
 *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE,                                                                   // draw_group_0
 *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE,                                                                // draw_group_1
 *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT,  // draw_group_2
 *         gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT                                                                 // draw_group_3
 *      };
 *
 *  �y���ӎ����z
 *      (1) �����Őݒ肵���t���O���g�p�����̂́E�E�E
 *          �����Őݒ肵���t���O���g�p�����̂́ASubmitViewAll�ł݂̂ł��B
 *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
 */
//-----------------------------------------------------------------------------
void Scene::SetSceneDrawGroupEnableBitFlagArray(bit32* scene_draw_group_enable_bit_flag_array)
{
  for( s16 i=0; i<m_scene_draw_group_num; ++i )
  {
    SetSceneDrawGroupEnableBitFlag(i, scene_draw_group_enable_bit_flag_array[i]);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O��ݒ肷��
 *
 *  @param[in]  scene_draw_group_index            �V�[���`��O���[�v
 *  @param[in]  scene_draw_group_enable_bit_flag  �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O
 *                  ��
 *                  scene_draw_group_enable_bit_flag = \
 *                      gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE | gfl::grp::g3d::Scene::SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT;
 *
 *  �y���ӎ����z
 *      (1) �����Őݒ肵���t���O���g�p�����̂́E�E�E
 *          �����Őݒ肵���t���O���g�p�����̂́ASubmitViewAll�ł݂̂ł��B
 *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
 */
//-----------------------------------------------------------------------------
void Scene::SetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index, bit32 scene_draw_group_enable_bit_flag)
{
  if( 0<=scene_draw_group_index&&scene_draw_group_index<m_scene_draw_group_num )
  {
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group_enable_bit_flag = scene_draw_group_enable_bit_flag;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_draw_group_index=%d, scene_draw_group_num=%d\n", scene_draw_group_index, m_scene_draw_group_num);
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O���擾����
 *
 *  @param[in]  scene_draw_group_index            �V�[���`��O���[�v
 *
 *  @retval     �V�[���`��O���[�v�𐧌䂷��r�b�g�t���O
 *
 *  �y���ӎ����z
 *      (1) �����Ŏ擾�����t���O���g�p����Ă���̂́E�E�E
 *          �����Ŏ擾�����t���O���g�p����Ă���̂́ASubmitViewAll�ł݂̂ł��B
 *              **** �ʏ��SubmitView�̂ɂ��K�p���Ă��悢���ǂ����悤�E�E�E ****
 */
//-----------------------------------------------------------------------------
bit32 Scene::GetSceneDrawGroupEnableBitFlag(s32 scene_draw_group_index) const
{
  bit32 ret = SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_NONE;
  if( 0<=scene_draw_group_index&&scene_draw_group_index<m_scene_draw_group_num)
  {
    ret = m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group_enable_bit_flag;
  }
  else
  {
    GFL_ASSERT_MSG(0, "scene_draw_group_index=%d, scene_draw_group_num=%d\n", scene_draw_group_index, m_scene_draw_group_num);
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�S�Ă�SubmitView����x�ɍs��
 *
 *  �y���ӎ����z
 *      (1) �J�����Ɋւ���
 *          ���̊֐����Ăяo���O�ɁASetSubmitViewCameraArray��SetSubmitViewCamera�ŁA�V�[���`��O���[�v�S�Ăɑ΂��Ďg�p����J������ݒ肵�Ă����ĉ������B
 *          ��L�ɂĐݒ肵�Ă������J�����̃J�����}�g���N�X�̍X�V�́A���̊֐����Ăяo���O�ɍς܂��Ă����ĉ������B(�J�����̃}�g���N�X�̍X�V�̑O�ɁACalculate���ς܂��Ă����K�v������܂��B)
 *      (2) �t�H�O�Ɋւ���
 *          ���̊֐����Ăяo���O�ɁAUpdateFog�Ńt�H�O�̍X�V���ς܂��Ă����ĉ������B
 */
//-----------------------------------------------------------------------------
void Scene::SubmitViewAll(void)
{
  // �O����
  for( s16 draw_i=0; draw_i<m_scene_draw_group_num; ++draw_i )
  {
    SceneDrawGroupPack* draw_pack     = &(m_scene_draw_group_pack_array[draw_i]);
    SceneDrawGroup*     draw_group    = draw_pack->scene_draw_group;
    bit32               draw_bit_flag = draw_pack->scene_draw_group_enable_bit_flag;
    if( IsBitFlagOnOne(draw_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
    {
      m_render_system->BeginSubmitView(draw_group);
    }
  }

  // �`�悷�郂�f���A�p�[�e�B�N���������_�[�L���[�ɐς�
  for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[calc_i]);
    SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
    SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);

    // ���f��
    submitViewAllModel(property_pack);
    // �p�[�e�B�N�� 
    submitViewAllParticle(property_pack);
#if GFL_GRP_G3D_H3D_USE
    // H3d���f��
    submitViewAllH3dModel(property_pack);
#endif  // GFL_GRP_G3D_H3D_USE
  }
 
  // �㏈��
  for( s16 draw_i=0; draw_i<m_scene_draw_group_num; ++draw_i )
  {
    SceneDrawGroupPack* draw_pack     = &(m_scene_draw_group_pack_array[draw_i]);
    SceneDrawGroup*     draw_group    = draw_pack->scene_draw_group;
    bit32               draw_bit_flag = draw_pack->scene_draw_group_enable_bit_flag;
    if( IsBitFlagOnOne(draw_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_SORT) )
    {
      m_render_system->EndSubmitView(draw_group);
    }
    if( IsBitFlagOnOne(draw_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
    {
      draw_group->SetState(SceneDrawGroup::STATE_AFTER_SUBMIT_VIEW);
    }
  }
}




//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�̃����_�[�L���[���}�[�W����
 * 
 *  @param[in]  scene_draw_group_index_A  �V�[���`��O���[�v�BB���ǉ�������̂ŕύX����܂��B
 *  @param[in]  scene_draw_group_index_B  �V�[���`��O���[�v�B�ύX����܂���B
 *  @param[in]  method                    �}�[�W���@
 *
 *  �y���ӎ����z
 *      (1) A��B�𓯂��ɂ��Ȃ��ŉ������B
 *      (2)�u�����_�[�L���[B�v���u�����_�[�L���[A�v�Ɂu���@method�v�Ń}�[�W����B
 *          �}�[�W��A�y�u�VA�v�z�́y�u��A�v�ƁuB�v�����킹�����́z�ɂȂ�A�uB�v�́uB�v�̂܂ܕύX����Ȃ��B
 */
//-----------------------------------------------------------------------------
void Scene::MergeRenderQueue(s32 scene_draw_group_index_A, const s32 scene_draw_group_index_B, const SceneDrawGroupMergeMethod method)
{
  STATIC_ASSERT( sizeof(nw::gfx::RenderQueue)==sizeof(internal::FakeRenderQueue) );
      // �T�C�Y���R���p�C�����Ƀ`�F�b�N��nw::gfx::RenderQueue�̕ύX�ɋC�t����悤�ɂ��Ă����B
      // m_List�̈ʒu���ς���Ă��邩�ǂ����܂ł͕�����Ȃ��̂ł��ꂾ���ł͕s�\���A�����d�����Ȃ��B

  // A��B�𓯂��ɂ��Ȃ��ŉ������B
  if( scene_draw_group_index_A == scene_draw_group_index_B )
  {
    GFL_ASSERT_MSG(0, "A=B=%d\n", scene_draw_group_index_A);
    return;
  }

  if(    0<=scene_draw_group_index_A&&scene_draw_group_index_A<m_scene_draw_group_num
      && 0<=scene_draw_group_index_B&&scene_draw_group_index_B<m_scene_draw_group_num )
  {
    SceneDrawGroup* draw_group_A = m_scene_draw_group_pack_array[scene_draw_group_index_A].scene_draw_group;
    SceneDrawGroup* draw_group_B = m_scene_draw_group_pack_array[scene_draw_group_index_B].scene_draw_group;

    nw::gfx::RenderQueue* render_queue_A = draw_group_A->GetRenderQueue();
    nw::gfx::RenderQueue* render_queue_B = draw_group_B->GetRenderQueue();

    nw::gfx::RenderQueue::ElementListType* list_A = (reinterpret_cast<internal::FakeRenderQueue*>(render_queue_A))->GetList();
    nw::gfx::RenderQueue::ElementListType* list_B = (reinterpret_cast<internal::FakeRenderQueue*>(render_queue_B))->GetList();

//#if GFL_DEBUG
    // m_List�̈ʒu���ς���Ă��邩������Ȃ��̂Ń`�F�b�N�ł��邱�Ƃ͂���Ă����A����������ł��܂��s�\���A�����d�����Ȃ��B
    if( !(
              render_queue_A->Size()==list_A->size()
           && render_queue_B->Size()==list_B->Size()
           && render_queue_A->Begin()->IsCommand()==list_A->begin()->IsCommand()
           && render_queue_B->Begin()->IsCommand()==list_B->begin()->IsCommand()
        )
    )
    {
      // @note GFL_ASSERT_STOP(GFL_ASSERT_STOP_MSG)�̂ق����ӂ��킵�������B
      GFL_ASSERT_MSG(0, "Size: render_queue_A=%d, list_A=%d, render_queue_B=%d, list_B=%d\n", render_queue_A->Size(), list_A->size(), render_queue_B->Size(), list_B->Size());
      GFL_ASSERT_MSG(0, "Begin: render_queue_A=%d, list_A=%d, render_queue_B=%d, list_B=%d\n", render_queue_A->Begin()->IsCommand(), list_A->begin()->IsCommand(), render_queue_B->Begin()->IsCommand(), list_B->begin()->IsCommand());
      return;
    }
//#endif  // #if GFL_DEBUG

    switch(method)
    {
    case SCENE_DRAW_GROUP_MERGE_METHOD_HEAD:
      {
        mergeRenderQueueMethodHead(list_A, list_B);
        break;
      }
    case SCENE_DRAW_GROUP_MERGE_METHOD_TAIL:
      {
        mergeRenderQueueMethodTail(list_A, list_B);
        break;
      }
    case SCENE_DRAW_GROUP_MERGE_METHOD_SORTED_MIX:
      {
        mergeRenderQueueMethodSortedMix(list_A, list_B);
        break;
      }
    case SCENE_DRAW_GROUP_MERGE_METHOD_MIX_NEED_SORT:
      {
        mergeRenderQueueMethodMixNeedSort(draw_group_A, list_A, list_B);
        break;
      }
    default:
      {
        GFL_ASSERT_MSG(0, "method=%d\n", method);
        break;
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "A=%d,B=%d\n", scene_draw_group_index_A, scene_draw_group_index_B);
  }
}

//-----------------------------------------------------------------------------
// SubmitView
//-----------------------------------------------------------------------------
void Scene::SubmitView(
    Camera* camera,
    bool update_camera_matrix,
    bool update_fog,
    s32 scene_draw_group_index )
{
  if( update_camera_matrix )
  {
    camera->UpdateCameraMatrix();
  }

  // �S�ẴV�[���v�Z�O���[�v�̃t�H�O�̍X�V���s��
  if( update_fog )
  {
    UpdateFog(camera);
  }

  SceneDrawGroup* draw_group = m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group;
  nw::gfx::Camera* nw_camera = camera->GetNwCamera();

  // ���_�Ɉˑ�����X�V������ RenderQueue �̍\�z���s���܂��B
  m_render_system->BeginSubmitView(draw_group);
  
#if 1
  for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    // �`�悷�郂�f���A�p�[�e�B�N���͕\����On�ɂ��A�`�悵�Ȃ����f���A�p�[�e�B�N���͕\����Off�ɂ���
    SceneNodePropertyPack*   property_pack = &(m_scene_calculate_group_pack_array[calc_i].scene_node_property_pack);
   
    // �`�悷�郂�f���͕\����On�ɂ��A�`�悵�Ȃ����f���͕\����Off�ɂ���
    storeSceneNodeVisible(property_pack->model_array, scene_draw_group_index);
    // �`�悷��p�[�e�B�N���͕\����On�ɂ��A�`�悵�Ȃ��p�[�e�B�N���͕\����Off�ɂ���
    storeSceneNodeVisible(property_pack->particle_array, scene_draw_group_index);

    // SubmitView
    m_render_system->SubmitView(
      nw_camera,
      m_scene_system,
      m_scene_calculate_group_pack_array[calc_i].scene_calculate_group,
      draw_group );

    // ���f���̕`��t���O��߂�
    restoreSceneNodeVisible(property_pack->model_array);
    // �p�[�e�B�N���̕`��t���O��߂�
    restoreSceneNodeVisible(property_pack->particle_array);

#if GFL_GRP_G3D_H3D_USE
    {
      H3dModelPropertyArray* property_array = property_pack->h3d_model_array;
   
      const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
      H3dModelPropertyElement*       element  = property_array->GetElementHead();
      while(element != sentinel)
      {
        H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
        
        H3dModel* h3d_model = property->scene_node;
     
        //���\�������悤�ɂȂ邩�͂킩��Ȃ��̂ŁA���ׂẴm�[�h�ɑ΂��Ă����Ń_�[�e�B�t���O�𗧂Ă�B
        //�_�[�e�B�t���O�́A�R�}���h������i�`���j�I�t�ɂ����B
        if ( h3d_model )
        {//KW255�C��
          h3d_model->SetFragmentAndWBufferDirty( m_render_system->IsFragmentAndWBufferDirty() );
        
          if( isH3dModelPropertyInSceneTreeEasy(*property) && h3d_model->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
          {
            if( property->scene_draw_group_bit->IsOn(scene_draw_group_index) )
            {
              m_render_system->SubmitViewH3dModel(
                nw_camera,
                m_scene_system,
                h3d_model,
                draw_group );
            }
          }
        }

        element = element->next;
      }
    }
#endif  // GFL_GRP_G3D_H3D_USE
  }
#else
  for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    SceneNodePropertyPack*   property_pack = &(m_scene_calculate_group_pack_array[calc_i].scene_node_property_pack);
    
    // ���f��
    {
      const ModelNodePropertyElement* sentinel = getModelSentinelElement(property_pack);
      ModelNodePropertyElement*       element  = getModelElementHead(property_pack);
      while(element != sentinel)
      {
        ModelNodeProperty* model_property = &(element->data);
        if( model_property->scene_draw_group_bit->IsOn(scene_draw_group_index) )
        {
          if( isSceneNodePropertyInSceneTreeEasy(*model_property) && model_property->scene_node->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
          {
            m_render_system->SubmitViewModel(
                nw_camera,
                m_scene_system,
                (reinterpret_cast<Model*>(model_property->scene_node))->GetNwModel(),
                draw_group );
          }
        }
        element = element->next;
      }
    }

    // �p�[�e�B�N��
    {
      const ModelNodePropertyElement* sentinel = getParticleSentinelElement(property_pack);
      ModelNodePropertyElement*       element  = getParticleElementHead(property_pack);
      while(element != sentinel) 
      {
        ModelNodeProperty* particle_property = &(element->data);
        if( particle_property->scene_draw_group_bit->IsOn(scene_draw_group_index) )
        {
          if( isSceneNodePropertyInSceneTreeEasy(*particle_property) && particle_property->scene_node->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
          {
            Particle* particle = reinterpret_cast<Particle*>(particle_property->scene_node);
     
            s32 particle_model_num = particle->GetModelNum();
            for( s32 j=0; j<particle_model_num; ++j )
            {
              m_render_system->SubmitViewModel(
                  nw_camera,
                  m_scene_system,
                  particle->GetModel(j),
                  draw_group );
            }
          }
        }
        element = element->next;
      }
    }
  }
#endif

  m_render_system->EndSubmitView(draw_group);

  draw_group->SetState(SceneDrawGroup::STATE_AFTER_SUBMIT_VIEW);
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// Draw
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

#define GFL_G3D_SCENE_DRAW_FOOTER() \
  m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetState(SceneDrawGroup::STATE_AFTER_DRAW); \
  for( s16 i=0; i<m_scene_calculate_group_num; ++i ) \
  { \
    m_scene_system->SetParticleEnableSwapBuffer(m_scene_calculate_group_pack_array[i].scene_calculate_group, true); \
  }

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�揈��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     display_type           �\����
 *  @param[in]     camera                 ���̃V�[����`�悷��J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::Draw(
  gfl::grp::GraphicsSystem*     graphics_system,
  gfl::grp::DisplayType         display_type,
  Camera*                       camera,
  bool                          command_cache_dump 
)
{
  const s32 default_scene_draw_group = 0;
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(camera, default_scene_draw_group, &is_assign_camera_index, &is_set_active_camera_index);

  gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
  gfl::grp::RenderTarget* render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(display_type) );

  Draw(graphics_system, camera, render_target, default_scene_draw_group, command_cache_dump);

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(camera, default_scene_draw_group, is_assign_camera_index, is_set_active_camera_index);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �X�e���I�`�揈��
 *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::StereoDraw(
  gfl::grp::GraphicsSystem*    graphics_system,
  StereoCamera*                camera,
  bool                         command_cache_dump 
)
{
  const s32 default_scene_draw_group = 0;
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(camera, default_scene_draw_group, &is_assign_camera_index, &is_set_active_camera_index);

  gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
  gfl::grp::RenderTarget* left_render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER) );
  gfl::grp::RenderTarget* right_render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER_RIGHT) );
  CommandSaveStereoDraw(
      graphics_system,
      camera->GetLeftCamera(),  left_render_target,
      camera->GetRightCamera(), right_render_target,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
      default_scene_draw_group,
      camera->GetBaseCamera(),
      command_cache_dump
#else
      default_scene_draw_group,
      command_cache_dump
#endif
  );

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(camera, default_scene_draw_group, is_assign_camera_index, is_set_active_camera_index);
}


// Draw
void Scene::Draw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index,
      bool command_cache_dump )
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  m_render_system->RenderScene(
      graphics_system,
      render_target,
      camera->GetNwCamera(),
      m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group,
      command_cache_dump );

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()

#if GFL_GRP_G3D_H3D_USE
  //drawH3dModel(
  //  graphics_system, 
  //  camera,
  //  render_target,
  //  scene_draw_group_index
  //);
#endif  // GFL_GRP_G3D_H3D_USE
}

#if 0
//h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
// �R�}���h�Z�[�u��������Draw���Ȃ�
void Scene::CommandSaveNotDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      gfl::grp::RenderTarget* render_target,
      s32 scene_draw_group_index,
      bool command_cache_dump )
{
  nw::gfx::IRenderTarget* nw_render_target = render_target->GetNwRenderTarget();
  
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(NULL, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  m_render_system->CommandSaveNotRenderScene(
      graphics_system,
      nw_render_target,
      m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group,
      command_cache_dump );

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(NULL, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()
}

// �R�}���h�Z�[�u���Ă�����̂��g����Draw
void Scene::ReuseCommandSaveDraw(
      gfl::grp::GraphicsSystem* graphics_system,
      Camera* camera,
      gfl::grp::RenderTarget* render_target )
{
  // Reuse�̂Ƃ��̓V�X�e���Ŗ��ݒ���T�|�[�g����SetupNotSetCamera�ACleanupNotSetCamera�͕K�v�Ȃ��B

  m_render_system->ReuseCommandSaveRenderScene(
      graphics_system,
      render_target,
      camera->GetNwCamera() );
}
#endif

// �R�}���h�Z�[�u���Ȃ���StereoDraw
void Scene::StereoDraw(
    GraphicsSystem* graphics_system,
    Camera* left_camera, 
    gfl::grp::RenderTarget* left_render_target,
    Camera* right_camera,
    gfl::grp::RenderTarget* right_render_target,
    s32 scene_draw_group_index,
    bool command_cache_dump)
{
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
 
  // left
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  setupNotSetCamera(left_camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  Draw(graphics_system, left_camera, left_render_target, scene_draw_group_index, command_cache_dump);

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(left_camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  // right
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  setupNotSetCamera(right_camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  Draw(graphics_system, right_camera, right_render_target, scene_draw_group_index, command_cache_dump);

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(right_camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()
}

void Scene::StereoDraw(
    GraphicsSystem* graphics_system,
    StereoCamera* camera,
    gfl::grp::RenderTarget* left_render_target,
    gfl::grp::RenderTarget* right_render_target,
    s32 scene_draw_group_index,
    bool command_cache_dump)
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  Draw(graphics_system, camera->GetLeftCamera(), left_render_target, scene_draw_group_index, command_cache_dump);
  Draw(graphics_system, camera->GetRightCamera(), right_render_target, scene_draw_group_index, command_cache_dump);

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()
}

// �R�}���h�Z�[�u����StereoDraw
void Scene::CommandSaveStereoDraw(
    gfl::grp::GraphicsSystem* graphics_system, 
    Camera* left_camera, 
    gfl::grp::RenderTarget* left_render_target,
    Camera* right_camera,
    gfl::grp::RenderTarget* right_render_target,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    s32 scene_draw_group_index,
    Camera* base_camera,
    bool command_cache_dump
#else
    s32 scene_draw_group_index,
    bool command_cache_dump
#endif
)
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(left_camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);  // @note ���ݒ�̂Ƃ���left_camera���g�����Ƃɂ����B

  m_render_system->RenderStereoScene(
    graphics_system,
    left_render_target,
    left_camera->GetNwCamera(),
    right_render_target,
    right_camera->GetNwCamera(),
    m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group, 
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    base_camera->GetNwCamera(),
#endif
    command_cache_dump);

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(left_camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()


#if GFL_GRP_G3D_H3D_USE
  //commandSaveStereoDrawH3dModel(
  //  graphics_system,
  //  left_camera,
  //  left_render_target,
  //  right_camera,
  //  right_render_target,
  //  scene_draw_group_index
  //);
#endif  // GFL_GRP_G3D_H3D_USE
}

void Scene::CommandSaveStereoDraw(
    gfl::grp::GraphicsSystem* graphics_system, 
    StereoCamera* camera,
    gfl::grp::RenderTarget* left_render_target,
    gfl::grp::RenderTarget* right_render_target,
    s32 scene_draw_group_index,
    bool command_cache_dump)
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  b32 is_assign_camera_index;
  b32 is_set_active_camera_index;
  setupNotSetCamera(camera, scene_draw_group_index, &is_assign_camera_index, &is_set_active_camera_index);

  CommandSaveStereoDraw(
    graphics_system,
    camera->GetLeftCamera(),
    left_render_target,
    camera->GetRightCamera(),
    right_render_target,
#if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
    scene_draw_group_index,
    camera->GetBaseCamera(),
    command_cache_dump
#else
    scene_draw_group_index,
    command_cache_dump
#endif
  );

  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  cleanupNotSetCamera(camera, scene_draw_group_index, is_assign_camera_index, is_set_active_camera_index);

  GFL_G3D_SCENE_DRAW_FOOTER()
}

#if 0
//h3d�ŃT�|�[�g������Ȃ��̂ŃR�����g�A�E�g�B
// �R�}���h�Z�[�u���Ă�����̂��g����StereoDraw
void Scene::ReuseCommandSaveStereoDraw(
    gfl::grp::GraphicsSystem* graphics_system, 
    Camera* left_camera, 
    gfl::grp::RenderTarget* left_render_target,
    Camera* right_camera,
    gfl::grp::RenderTarget* right_render_target)
{
  // Reuse�̂Ƃ��̓V�X�e���Ŗ��ݒ���T�|�[�g����SetupNotSetCamera�ACleanupNotSetCamera�͕K�v�Ȃ��B
  
  ReuseCommandSaveDraw(graphics_system, left_camera, left_render_target );
  ReuseCommandSaveDraw(graphics_system, right_camera, right_render_target );
}

void Scene::ReuseCommandSaveStereoDraw(
    gfl::grp::GraphicsSystem* graphics_system, 
    StereoCamera* camera,
    gfl::grp::RenderTarget* left_render_target,
    gfl::grp::RenderTarget* right_render_target)
{
  // Reuse�̂Ƃ��̓V�X�e���Ŗ��ݒ���T�|�[�g����SetupNotSetCamera�ACleanupNotSetCamera�͕K�v�Ȃ��B
  
  ReuseCommandSaveDraw(graphics_system, camera->GetLeftCamera(), left_render_target );
  ReuseCommandSaveDraw(graphics_system, camera->GetRightCamera(), right_render_target );
}
#endif

#undef GFL_G3D_SCENE_DRAW_FOOTER


#if GFL_GRP_G3D_H3D_USE
void Scene::drawH3dModel(
    gfl::grp::GraphicsSystem* graphics_system, 
    Camera* camera,
    gfl::grp::RenderTarget* render_target,
    s32 scene_draw_group_index
)
{
#if 0
  /*aaaaaaaaaaaaa�����_�[�L���[�ɐςނ悤�ɂ����̂ł���͎g��Ȃ��Ȃ�܂����B���A�Q�l�Ɏc���Ă���܂��Baaaaaaaaaaaaa
  gfl::math::MTX34 view_mtx;
  camera->GetViewMatrix(&view_mtx);

  if( graphics_system->BindCurrentCommandList( render_target ) )
  {
    m_h3d_command->BegineDraw();
 
    m_h3d_environment->SetFromScene(this);  // ���C�g��t�H�O���A�j���[�V�������Ă��邩������Ȃ��̂Ŗ��t���[���ݒ肷��

    for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
    {
      SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[calc_i]);
      SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
      H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;
   
      const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
      H3dModelPropertyElement*       element  = property_array->GetElementHead();
      while(element != sentinel)
      {
        H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
        
        if( property->scene_draw_group_bit->IsOn(scene_draw_group_index) )
        {
          property->scene_node->DrawOpa(m_h3d_command, m_h3d_environment, &view_mtx, false);
          property->scene_node->DrawXlu(m_h3d_command, m_h3d_environment, &view_mtx, false);
        }

        element = element->next;
      }
    }
      
    m_h3d_command->EndDraw();
  }*/
#endif
}

void Scene::commandSaveStereoDrawH3dModel(
    gfl::grp::GraphicsSystem* graphics_system, 
    Camera* left_camera,
    gfl::grp::RenderTarget* left_render_target,
    Camera* right_camera,
    gfl::grp::RenderTarget* right_render_target,
    s32 scene_draw_group_index
)
{
  drawH3dModel(
      graphics_system, 
      left_camera,
      left_render_target,
      scene_draw_group_index
  );

  drawH3dModel(
      graphics_system,
      right_camera,
      right_render_target,
      scene_draw_group_index
  );
}
#endif




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �V�[���m�[�h�� �ǉ� / �폜
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

/////////////////////////////////////////////
/////////////////////////////////////////////
// 
// �����ߊ֐�
// 
/////////////////////////////////////////////
/////////////////////////////////////////////
b32 Scene::AddNodeToCalcAndDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index)
{
  b32 ret = false;
  ret = AddNodeToCalc(scene_node, scene_calculate_group_index);
  if( ret )
  {
    ret = AddNodeToDraw(scene_node, scene_draw_group_index);
  }
  return ret;
}
b32 Scene::AddNodeToCalcAndExDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index)
{
  b32 ret = false;
  ret = AddNodeToCalc(scene_node, scene_calculate_group_index);
  if( ret )
  {
    ret = AddNodeToExDraw(scene_node, scene_draw_group_index);
  }
  return ret;
}
b32 Scene::AddNodeToCalcAndAllDraw(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index)
{
  b32 ret = false;
  ret = AddNodeToCalc(scene_node, scene_calculate_group_index);
  if( ret )
  {
    ret = AddNodeToAllDraw(scene_node);
  }
  return ret;
}

b32 Scene::AddNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index)
{
  b32 ret = false;

  s32 old_scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  ObjectType object_type = OBJECT_TYPE_UNKNOWN;
  void* old_element = getNodeElement(scene_node, &old_scene_calculate_group_index, &object_type);
  // @note scene_node�̐e���擾���Ċm�F����΃��[�v���񂳂Ȃ��Ă悢
  
  if( old_element )
  {
    // ����scene_node�̓V�[���v�Z�O���[�v�̂ǂꂩ�ɓo�^����Ă���
    if( old_scene_calculate_group_index != scene_calculate_group_index )
    {
      ret = moveNodeFromCalcToCalc(old_element, object_type, old_scene_calculate_group_index, scene_calculate_group_index);
    }
    else
    {
      // ����scene_node�͖ړI�̃V�[���v�Z�O���[�v�ɓo�^����Ă���
      ret = true;
    }
  }
  else
  {
    // scene_node�͂܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ�
    void* element = NULL;
    ret = addNodeToCalc(scene_node, scene_calculate_group_index, &element);
  }
  return ret;
}


#define GFL_G3D_SCENE_GET_NODE_ELEMENT() \
  s32                scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE; \
  ObjectType         object_type                 = OBJECT_TYPE_UNKNOWN; \
  void* element = getNodeElement(scene_node, &scene_calculate_group_index, &object_type);

b32 Scene::AddNodeToDraw(SceneNode* scene_node, s32 scene_draw_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    ModelNodeProperty* property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
    return addModelNodePropertyToDraw(property, scene_draw_group_index);
  }
  return false;
}
b32 Scene::AddNodeToExDraw(SceneNode* scene_node, s32 scene_draw_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    ModelNodeProperty* property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
    removeModelNodePropertyFromAllDraw(property);
    return addModelNodePropertyToDraw(property, scene_draw_group_index);
  }
  return false;
}
b32 Scene::AddNodeToAllDraw(SceneNode* scene_node)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    ModelNodeProperty* property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
    return addModelNodePropertyToAllDraw(property);
  }
  return false;
}

void Scene::RemoveNodeFromCalcAndAllDraw(SceneNode* scene_node)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    removeNodeFromCalcAndAllDraw(element, object_type, scene_calculate_group_index);
  }
}
void Scene::RemoveNodeFromCalc(SceneNode* scene_node)
{
  RemoveNodeFromCalcAndAllDraw(scene_node);
}
void Scene::RemoveNodeFromDraw(SceneNode* scene_node, s32 scene_draw_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    ModelNodeProperty* property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
    removeModelNodePropertyFromDraw(property, scene_draw_group_index);
  }
}
void Scene::RemoveNodeFromAllDraw(SceneNode* scene_node)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT()
  if(element)
  {
    ModelNodeProperty* property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
    removeModelNodePropertyFromAllDraw(property);
  }
}

#undef GFL_G3D_SCENE_GET_NODE_ELEMENT


void Scene::RemoveAllNodeFromCalcAndAllDraw(s32 scene_calculate_group_index)
{
  // �S�V�[���v�Z�O���[�v�ɓo�^����Ă���S�V�[���m�[�h���`�F�b�N����̂ŁA�������ׂ������ł��B
  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);

  removeAllModelNodeFromCalcAndAllDraw(property_pack->model_array, OBJECT_TYPE_MODEL, scene_calculate_group_index);
  removeAllSceneNodeFromCalcAndAllDraw(property_pack->light_array, OBJECT_TYPE_LIGHT, scene_calculate_group_index);
  removeAllSceneNodeFromCalcAndAllDraw(property_pack->camera_array, OBJECT_TYPE_CAMERA, scene_calculate_group_index);
  removeAllSceneNodeFromCalcAndAllDraw(property_pack->fog_array, OBJECT_TYPE_FOG, scene_calculate_group_index);
  removeAllModelNodeFromCalcAndAllDraw(property_pack->particle_array, OBJECT_TYPE_PARTICLE, scene_calculate_group_index);
}
void Scene::RemoveAllNodeFromDraw(s32 scene_draw_group_index)
{
  // �S�V�[���v�Z�O���[�v�ɓo�^����Ă���S�V�[���m�[�h���`�F�b�N����̂ŁA�������ׂ������ł��B
  for( s32 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[calc_i]);
    SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);
  
    removeAllModelNodeFromDraw(property_pack->model_array, scene_draw_group_index);
    removeAllModelNodeFromDraw(property_pack->particle_array, scene_draw_group_index);
  }
}
void Scene::RemoveAllNodeFromAllCalcAndAllDraw(void)
{
  for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    RemoveAllNodeFromCalcAndAllDraw(calc_i);
  }
}

#if GFL_GRP_G3D_H3D_USE
b32  Scene::AddH3dModelToCalc(H3dModel* scene_node, s32 scene_calculate_group_index)
{
  // ���ɑ���Scene�ɓo�^����Ă���Ƃ��́A�o�^�ł��Ȃ��B
  {
    Scene* curr_scene = scene_node->GetScene();
    if( curr_scene )
    {
      if( curr_scene != this )
      {
        GFL_ASSERT_MSG(0, "���ɑ���Scene�ɓo�^����Ă��܂��B\n");
        return false;
      }
    }
  }

  b32 ret = false;

  SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
  H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;

  s32 old_scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  void* old_element = getH3dModelElement(scene_node, &old_scene_calculate_group_index);
  // @note SceneNode�h����scene_node�Ȃ�e���擾���Ċm�F����΃��[�v���񂳂Ȃ��Ă悢���AH3dModel�͂܂��e���o���Ă��Ȃ��̂ł���͂ł��Ȃ��B
  
  if( old_element )
  {
    // ����scene_node�̓V�[���v�Z�O���[�v�̂ǂꂩ�ɓo�^����Ă���
    if( old_scene_calculate_group_index != scene_calculate_group_index )
    {
      H3dModelProperty* old_property = &((reinterpret_cast<H3dModelPropertyElement*>(old_element))->data);

      // Remove�O�Ɉ�UAdd���Ă���
      void* element = property_array->AddGetSpaceElementTail();  
      if( element )
      { 
        H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
        property->scene_node = old_property->scene_node;
        property->bit_flag   = old_property->bit_flag;
        property->scene_draw_group_index_one = old_property->scene_draw_group_index_one;
        property->scene_draw_group_bit->Copy(old_property->scene_draw_group_bit);
      }

      // Remove
      RemoveH3dModelFromCalcAndAllDraw(scene_node);

      // Add
      if( element )
      {
        ret = true;
      }
      else
      {
        scene_node->SetScene(NULL);  // Scene�o�^����
        ret = false;
      }
    }
    else
    {
      // ����scene_node�͖ړI�̃V�[���v�Z�O���[�v�ɓo�^����Ă���
      ret = true;
    }
  }
  else
  {
    // scene_node�͂܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ�
    void* element = property_array->AddGetSpaceElementTail();  
    if( element )
    {
      H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
      property->scene_node = scene_node;
      scene_node->SetScene(this);  // Scene�o�^
      ret = true;
    }
  }

  GFL_ASSERT_MSG(ret, "H3dModel��Calc�O���[�v%d�ɓo�^�ł��܂���ł����B\n", scene_calculate_group_index);  //@check �v�Z�A�`�悪����Ȃ������s�\�B
  return ret;
}

b32  Scene::AddH3dModelToDraw(H3dModel* scene_node, s32 scene_draw_group_index)
{
  b32 ret = false;

  s32 scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  void* element = getH3dModelElement(scene_node, &scene_calculate_group_index);
  
  if( element )
  {
    H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);

    property->scene_draw_group_bit->On(scene_draw_group_index);
    u32 on_bit_num = property->scene_draw_group_bit->GetOnBitNum();
    if( on_bit_num == 0 )
    {
      property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
      GFL_ASSERT_MSG(0, "scene_draw_group_index=%d��H3dModel��ǉ��ł��܂���B\n", scene_draw_group_index); 
    }
    else if( on_bit_num == 1 )
    {
      u32 index = 0;
      property->scene_draw_group_bit->GetOnBitMinIndex(&index);
      property->scene_draw_group_index_one = index;
      ret = true;
    }
    else
    {
      property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
      ret = true;
    }
  }

  return ret;
}

void Scene::RemoveH3dModelFromCalcAndAllDraw(H3dModel* scene_node)
{
  s32 scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  void* element = getH3dModelElement(scene_node, &scene_calculate_group_index);
  if( element )
  {
    SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
    SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
    H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;
    property_array->RemoveElement(reinterpret_cast<H3dModelPropertyElement*>(element));
    scene_node->SetScene(NULL);  // Scene�o�^����
  }
}

void Scene::RemoveH3dModelFromDraw(H3dModel* scene_node, s32 scene_draw_group_index)
{
  s32 scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  void* element = getH3dModelElement(scene_node, &scene_calculate_group_index);
  if( element )
  {
    H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
    property->scene_draw_group_bit->Off(scene_draw_group_index);
    u32 on_bit_num = property->scene_draw_group_bit->GetOnBitNum();
    if( on_bit_num == 0 )
    {
      property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_NONE;
    }
    else if( on_bit_num == 1 )
    {
      u32 index = 0;
      property->scene_draw_group_bit->GetOnBitMinIndex(&index);
      property->scene_draw_group_index_one = index;
    }
    else
    {
      property->scene_draw_group_index_one = SCENE_DRAW_GROUP_INDEX_MULTI;
    }
  }
}

void Scene::RemoveAllH3dModelFromCalcAndAllDraw(s32 scene_calculate_group_index)
{
  SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
  H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;

  const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
  H3dModelPropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    H3dModelPropertyElement* temp = element->next;
    {
      H3dModelProperty* property = &(element->data);
      property->scene_node->SetScene(NULL);  // Scene�o�^����
    }
    property_array->RemoveElement(element);
    element = temp;
  }   
}

void Scene::RemoveAllH3dModelFromAllCalcAndAllDraw(void)
{
  for( s16 calc_i=0; calc_i<m_scene_calculate_group_num; ++calc_i )
  {
    RemoveAllH3dModelFromCalcAndAllDraw(calc_i);
  }
}
#endif  // GFL_GRP_G3D_H3D_USE


/////////////////////////////////////////////
/////////////////////////////////////////////
// 
// �]���ʂ�̊֐�
// 
/////////////////////////////////////////////
/////////////////////////////////////////////
b32  Scene::AddSceneNode( SceneNode* scene_node, s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(scene_node, scene_calculate_group_index );
}
b32  Scene::RemoveSceneNode( SceneNode* scene_node )
{
  return removeSceneNodeFromSceneCalculateGroup(scene_node);
}
b32  Scene::RemoveAllSceneNodes(void)
{
  return removeAllSceneNodeFromSceneCalculateGroup();
}

b32 Scene::AddModelToSceneCalculateGroup( Model*  model,  s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(model, scene_calculate_group_index );
}
b32 Scene::AddLightToSceneCalculateGroup( Light*  light,  s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(light, scene_calculate_group_index );
}
b32 Scene::AddCameraToSceneCalculateGroup( Camera* camera, s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(camera, scene_calculate_group_index );
}
b32 Scene::AddFogToSceneCalculateGroup( Fog*    fog,    s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(fog, scene_calculate_group_index );
}
b32 Scene::AddParticleToSceneCalculateGroup( Particle*  particle,  s32 scene_calculate_group_index )
{
  return addSceneNodeToSceneCalculateGroup(particle, scene_calculate_group_index );
}

b32 Scene::RemoveModelFromSceneCalculateGroup( Model*  model  )
{
  return removeSceneNodeFromSceneCalculateGroup(model);
}
b32 Scene::RemoveLightFromSceneCalculateGroup( Light*  light  )
{
  return removeSceneNodeFromSceneCalculateGroup(light);
}
b32 Scene::RemoveCameraFromSceneCalculateGroup( Camera* camera )
{
  return removeSceneNodeFromSceneCalculateGroup(camera);
}
b32 Scene::RemoveFogFromSceneCalculateGroup( Fog*    fog    )
{
  return removeSceneNodeFromSceneCalculateGroup(fog);
}
b32 Scene::RemoveParticleFromSceneCalculateGroup( Particle*  particle  )
{
  return removeSceneNodeFromSceneCalculateGroup(particle);
}

b32 Scene::AddModelToSceneCalculateGroupAndSceneDrawGroup( Model*  model, s32 scene_calculate_group_index, s32 scene_draw_group_index )
{
  return addModelNodeToSceneCalculateGroupAndSceneDrawGroup(model, scene_calculate_group_index, scene_draw_group_index);
}
b32 Scene::AddParticleToSceneCalculateGroupAndSceneDrawGroup( Particle* particle, s32 scene_calculate_group_index, s32 scene_draw_group_index )
{
  return addModelNodeToSceneCalculateGroupAndSceneDrawGroup(particle, scene_calculate_group_index, scene_draw_group_index);
}

b32  Scene::RemoveAllSceneNodesFromSceneCalculateGroup(void)
{
  return removeAllSceneNodeFromSceneCalculateGroup();
}

b32 Scene::AddModelToAllSceneDrawGroup(Model* model)
{
  return addModelNodeToAllSceneDrawGroup(model);
}
b32 Scene::AddModelToSceneDrawGroup(Model* model, s32 scene_draw_group_index)
{
  return addModelNodeToSceneDrawGroup(model, scene_draw_group_index);
}
b32 Scene::RemoveModelFromAllSceneDrawGroup(Model* model)
{
  return removeModelNodeFromAllSceneDrawGroup(model);
}
b32 Scene::RemoveModelFromSceneDrawGroup(Model* model, s32 scene_draw_group_index)
{
  return removeModelNodeFromSceneDrawGroup(model, scene_draw_group_index);
}

b32 Scene::AddParticleToAllSceneDrawGroup(Particle* particle)
{
  return addModelNodeToAllSceneDrawGroup(particle);
}
b32 Scene::AddParticleToSceneDrawGroup(Particle* particle, s32 scene_draw_group_index)
{
  return addModelNodeToSceneDrawGroup(particle, scene_draw_group_index);
}
b32 Scene::RemoveParticleFromAllSceneDrawGroup(Particle* particle)
{
  return removeModelNodeFromAllSceneDrawGroup(particle);
}
b32 Scene::RemoveParticleFromSceneDrawGroup(Particle* particle, s32 scene_draw_group_index)
{
  return removeModelNodeFromSceneDrawGroup(particle, scene_draw_group_index);
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �V�[����
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[������ݒ肷��
 *
 *  @param[in]     scene_environment_setting  �ݒ肷��V�[����
 */
//-----------------------------------------------------------------------------
void Scene::SetSceneEnvironmentSetting( SceneEnvironmentSetting* scene_environment_setting )
{
#if 0
  for( u32 i=0; i<m_scene_calculate_group_num; ++i )
  {
    // �V�[�����̎Q�Ɖ������s���ݒ肵�܂��B
    nw::gfx::SceneContext*     scene_context     = m_scene_calculate_group_array[i]->GetSceneContext();
    scene_environment_setting->GetNwSceneEnvironmentSetting()->ResolveReference(*scene_context);

    // �\�z���͂�����
    // ���C�g�Z�b�g�̓N���A���Ă���ݒ肵������Ă���
    // �J�����ƃt�H�O�͂ǂ����낤
    nw::gfx::SceneEnvironmentSetting::CameraBinderArray::iterator cam_begin = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetCameraBegin();
    nw::gfx::SceneEnvironmentSetting::CameraBinderArray::iterator cam_end   = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetCameraEnd();
    if( cam_begin == cam_end ) GFL_PRINT("cam_begin == cam_end\n");
    else                       GFL_PRINT("cam_begin != cam_end\n");

    nw::gfx::SceneEnvironmentSetting::FogBinderArray::iterator fog_begin = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetFogBegin();
    nw::gfx::SceneEnvironmentSetting::FogBinderArray::iterator fog_end   = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetFogEnd();
    if( fog_begin == fog_end ) GFL_PRINT("fog_begin == fog_end\n");
    else                       GFL_PRINT("fog_begin != fog_end\n");

    nw::gfx::SceneEnvironmentSetting::LightSetBinderArray::iterator l_begin = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetLightSetBegin();
    nw::gfx::SceneEnvironmentSetting::LightSetBinderArray::iterator l_end   = scene_environment_setting->GetNwSceneEnvironmentSetting()->GetLightSetEnd();
    if( l_begin == l_end ) GFL_PRINT("l_begin == l_end\n");
    else                   GFL_PRINT("l_begin != l_end\n");
  
    //m_render_system->SetSceneEnvironmentSetting( *(scene_environment_setting->GetNwSceneEnvironmentSetting()) );
  }

  m_render_system->SetSceneEnvironmentSetting( *(scene_environment_setting->GetNwSceneEnvironmentSetting()) );
#else
  // ���C�g�A�J�����A�t�H�O�̓V�[���v�Z�O���[�v0�Ԃɂ��������悤�ɂ��Ă��炤�B
  u32 i=0;
  {
    // �V�[�����̎Q�Ɖ������s���ݒ肵�܂��B
    nw::gfx::SceneContext*     scene_context     = m_scene_calculate_group_pack_array[i].scene_calculate_group->GetSceneContext();
    scene_environment_setting->GetNwSceneEnvironmentSetting()->Clear();
        // �Q�Ɖ��������V�[�����ݒ���N���A���܂��B SceneContext ��ύX���čēx ResolveReference ���s���ꍇ�ɗp���ĉ������B
        // ����scene_environment_setting�ɑ΂��ĉ��x��ResolveReference���ĂԂƕs����N����̂ŁA���̑΍�Ƃ���Clear�𖈉��ɌĂ�ł������Ƃɂ���B
        // �s��̗�F2��ResolveReference���Ă񂾏ꍇ�A2��ڂ�1��ڂƓ������̂�o�^����̂�1��ڂ��㏑��������2��ڂ�ǉ����Ă���B
        // �������A�o�^�̈��nw::gfx::SceneEnvironmentSetting::CreateEnvironmentArray�Ŋm�ۂ����������Ȃ��̂ŁA2��ڂ�ǉ��ł���ꏊ�͗]���Ă��Ȃ��B
        // ����āAnw::gfx::SceneEnvironmentSetting::ResolveReference����
        // bool isSuccess = this->m_Fogs.push_back(fogBinder);
        // NW_ASSERT(isSuccess);
        // ��ASSERT�Ɉ���������B
    scene_environment_setting->GetNwSceneEnvironmentSetting()->ResolveReference(*scene_context);
  }
  
  m_render_system->SetSceneEnvironmentSetting( *(scene_environment_setting->GetNwSceneEnvironmentSetting()) );
#endif

  GFL_GL_ASSERT();
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �J�����ݒ�
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ɃJ������ݒ肷��
 *
 *  @param[in]     camera         �J����(camera_index�ɐݒ肷��)
 *                                StereoCamera�^����
 *  @param[in]     camera_index   �J�����̃C���f�b�N�X
 *
 *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����camera_index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
 */
//-----------------------------------------------------------------------------
void Scene::AssignCameraIndex( Camera* camera, s32 camera_index )
{
  if( camera )
  {
    m_render_system->SetCamera( camera_index, camera->GetNwCamera() );
  }
  else
  {
    m_render_system->SetCamera( camera_index, NULL );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �J������ݒ肷��
 *
 *  @param[in]     index      �ݒ肷��C���f�b�N�X
 *  @param[in]     camera     �ݒ肷��J����
 *   
 *  �ucamera=NULL, camera_index=�C���f�b�N�X�v��n����index�Ɋ��蓖�ĂĂ����J�������Ȃ��ɂ���
 */
//-----------------------------------------------------------------------------
void Scene::SetCamera( s32 index, Camera* camera )
{
  AssignCameraIndex( camera, index );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ɐݒ肵���J�����̃C���f�b�N�X���擾����
 *
 *  @param[in]     camera         �J����
 *                                StereoCamera�^����
 *
 *  @retval        s32    �J�����̃C���f�b�N�X
 *                        �J�����ɃC���f�b�N�X������U���Ă��Ȃ�������CAMERA_INDEX_NONE��Ԃ�
 */
//-----------------------------------------------------------------------------
s32  Scene::GetAssignedCameraIndex( const Camera* camera ) const
{
  if( camera )
  {
    return m_render_system->GetCameraIndex(camera->GetNwCamera()); 
  }
  else
  {
    return CAMERA_INDEX_NONE;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����ɐݒ肵���J�������擾����
 *
 *  @param[in]     camera_index         �J�����̃C���f�b�N�X
 *
 *  @retval        Camera*    �J����
 *                            StereoCamera�^�ł����Ă�Camera�^�ŕԂ��̂ŁA���̊֐��Ŏ擾��ɔ��ʂ���K�v������
 *                            camera_index�ɃJ���������蓖�ĂĂ��Ȃ�������NULL��Ԃ�
 */
//-----------------------------------------------------------------------------
Camera* Scene::GetAssignedCamera( const s32 camera_index ) const
{
  nw::gfx::Camera* nw_camera =  m_render_system->GetCamera(camera_index); 
  if( nw_camera )
  {
    for(s32 i=0; i<m_scene_calculate_group_num; ++i)
    {
      const SceneNodePropertyElement* sentinel = getCameraSentinelElement(&(m_scene_calculate_group_pack_array[i].scene_node_property_pack));
      SceneNodePropertyElement*       element  = getCameraElementHead(&(m_scene_calculate_group_pack_array[i].scene_node_property_pack));
      while(element != sentinel)
      {
        if( nw_camera == element->data.scene_node->GetNwSceneNode() )
        {
          return reinterpret_cast<Camera*>(element->data.scene_node);
        }
        element = element->next;
      }
    }
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�̃A�N�e�B�u�J�����̃J�����C���f�b�N�X��ݒ肷��
 *
 *  @param[in]     camera_index             �J�����̃C���f�b�N�X
 *  @param[in]     scene_draw_group_index   �V�[���`��O���[�v�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void Scene::SetActiveCameraIndex( s32 camera_index, s32 scene_draw_group_index )
{
  m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->SetActiveCamera( camera_index );
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�̃A�N�e�B�u�J�����̃J�����C���f�b�N�X���擾����
 *
 *  @param[in]     scene_draw_group_index   �V�[���`��O���[�v�̃C���f�b�N�X
 */
//-----------------------------------------------------------------------------
s32  Scene::GetActiveCameraIndex( const s32 scene_draw_group_index ) const
{
  return m_scene_draw_group_pack_array[scene_draw_group_index].scene_draw_group->GetActiveCameraIndex();
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// ���C�g�Z�b�g�ݒ�
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����Ƀ��C�g�Z�b�g��ݒ肷��
 *
 *  @param[in]     light_set_index   �ݒ��ƂȂ郉�C�g�Z�b�g�̃C���f�b�N�X 
 *  @param[in]     light_set         ���C�g�Z�b�g�B
 *                                   �Ăяo�����͂��̃��C�g�Z�b�g��ێ��������ĉ������B
 *                                   light_set_index�ɐݒ肵�Ă��郉�C�g�Z�b�g���O���ꍇ��NULL��n���ĉ������B
 *                                   ���̃��C�g�Z�b�g�ɒǉ����Ă��郉�C�g�́A���̃V�[���ɒǉ�����Ă��Ȃ���΂Ȃ�܂���B
 */
//-----------------------------------------------------------------------------
void Scene::SetLightSet(s32 light_set_index, LightSet* light_set)
{
  if( light_set )
  {
    m_render_system->SetLightSet(light_set_index, light_set->GetNwLightSet());
  }
  else
  {
    m_render_system->SetLightSet(light_set_index, NULL);
  }
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// ��
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         �����o�͂���(Release�ł͉������Ȃ�)
 */
//-----------------------------------------------------------------------------
void Scene::PrintNum(void) const
{
#if GFL_DEBUG
  GFL_PRINT("--------------------------------\n");
  GFL_PRINT("gfl::grp::g3d::Scene::PrintNum\n");

  GFL_PRINT("SceneCalculateGroupNum = %d\n", GetSceneCalculateGroupNum());
  GFL_PRINT("SceneDrawGroupNum      = %d\n", GetSceneDrawGroupNum());
  
  for(s32 i=0; i<m_scene_calculate_group_num; ++i)
  {
    GFL_PRINT("ModelNumInSceneCalculateGroup[%d]    = %d\n", i, GetModelNumInSceneCalculateGroup(i));
    GFL_PRINT("LightNumInSceneCalculateGroup[%d]    = %d\n", i, GetLightNumInSceneCalculateGroup(i));
    GFL_PRINT("CameraNumInSceneCalculateGroup[%d]   = %d\n", i, GetCameraNumInSceneCalculateGroup(i));
    GFL_PRINT("FogNumInSceneCalculateGroup[%d]      = %d\n",  i, GetFogNumInSceneCalculateGroup(i));
    GFL_PRINT("ParticleNumInSceneCalculateGroup[%d] = %d\n",  i, GetParticleNumInSceneCalculateGroup(i));
#if GFL_GRP_G3D_H3D_USE
    GFL_PRINT("H3dNumInSceneCalculateGroup[%d]      = %d\n",  i, GetH3dModelNumInSceneCalculateGroup(i));
#endif  // GFL_GRP_G3D_H3D_USE
  }

  for(s32 i=0; i<m_scene_draw_group_num; ++i)
  {
    GFL_PRINT("ModelNumInSceneDrawGroup[%d]    = %d\n", i, GetModelNumInSceneDrawGroup(i));
    GFL_PRINT("ParticleNumInSceneDrawGroup[%d] = %d\n", i, GetParticleNumInSceneDrawGroup(i));
  }

  GFL_PRINT("ModelNum    = %d\n", GetModelNum());
  GFL_PRINT("LightNum    = %d\n", GetLightNum());
  GFL_PRINT("CameraNum   = %d\n", GetCameraNum());
  GFL_PRINT("FogNum      = %d\n", GetFogNum());
  GFL_PRINT("ParticleNum = %d\n", GetParticleNum());
#if GFL_GRP_G3D_H3D_USE
  GFL_PRINT("H3DModelNum = %d\n", GetH3dModelNum());
#endif  // GFL_GRP_G3D_H3D_USE
  
  

  GFL_PRINT("ModelMaterialCount = %d\n", GetModelMaterialCount());

  for(u32 i=0; i<m_scene_calculate_group_num; ++i)
  {
    GFL_PRINT("NwSceneNodeNumInNwSceneContext[%d]       = %d\n", i, GetNwSceneNodeNumInNwSceneContext(i));
    GFL_PRINT("NwUserRenderNodeNumInNwSceneContext[%d]  = %d\n", i, GetNwUserRenderNodeNumInNwSceneContext(i));
    GFL_PRINT("NwModelNumInNwSceneContext[%d]           = %d\n", i, GetNwModelNumInNwSceneContext(i));
    GFL_PRINT("NwSkeletalModelNumInNwSceneContext[%d]   = %d\n", i, GetNwSkeletalModelNumInNwSceneContext(i));
    GFL_PRINT("NwLightNumInNwSceneContext[%d]           = %d\n", i, GetNwLightNumInNwSceneContext(i));
    GFL_PRINT("NwFragmentLightNumInNwSceneContext[%d]   = %d\n", i, GetNwFragmentLightNumInNwSceneContext(i));
    GFL_PRINT("NwVertexLightNumInNwSceneContext[%d]     = %d\n", i, GetNwVertexLightNumInNwSceneContext(i));
    GFL_PRINT("NwHemiSphereLightNumInNwSceneContext[%d] = %d\n", i, GetNwHemiSphereLightNumInNwSceneContext(i));
    GFL_PRINT("NwAmbientLightNumInNwSceneContext[%d]    = %d\n", i, GetNwAmbientLightNumInNwSceneContext(i));
    GFL_PRINT("NwCameraNumInNwSceneContext[%d]          = %d\n", i, GetNwCameraNumInNwSceneContext(i));
    GFL_PRINT("NwFogNumInNwSceneContext[%d]             = %d\n", i, GetNwFogNumInNwSceneContext(i));
    GFL_PRINT("NwParticleSetNumInNwSceneContext[%d]     = %d\n", i, GetNwParticleSetNumInNwSceneContext(i));
    GFL_PRINT("NwParticleEmitterNumInNwSceneContext[%d] = %d\n", i, GetNwParticleEmitterNumInNwSceneContext(i));
    GFL_PRINT("NwParticleModelNumInNwSceneContext[%d]   = %d\n", i, GetNwParticleModelNumInNwSceneContext(i));
    GFL_PRINT("NwAnimatableNodeNumInNwSceneContext[%d]  = %d\n", i, GetNwAnimatableNodeNumInNwSceneContext(i));

    GFL_PRINT("NwModelMaterialCountInNwSceneContext[%d]              = %d\n", i, GetNwModelMaterialCountInNwSceneContext(i));
    GFL_PRINT("NwSkeletalModelMaterialCountInNwSceneContext[%d]      = %d\n", i, GetNwSkeletalModelMaterialCountInNwSceneContext(i));
    GFL_PRINT("NwParticleModelMaterialNodeCountInNwSceneContext[%d]  = %d\n", i, GetNwParticleModelMaterialCountInNwSceneContext(i));
  }
  
  GFL_PRINT("--------------------------------\n");
#endif
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���v�Z�O���[�v�ɒǉ�����Ă���V�[���m�[�h�̌����擾����
 */
//-----------------------------------------------------------------------------
u32 Scene::GetModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.model_array->GetElementNum();
} 
u32 Scene::GetLightNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.light_array->GetElementNum();
}
u32 Scene::GetCameraNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.camera_array->GetElementNum();
}
u32 Scene::GetFogNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.fog_array->GetElementNum();
}
u32 Scene::GetParticleNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.particle_array->GetElementNum();
}

#if GFL_GRP_G3D_H3D_USE
u32 Scene::GetH3dModelNumInSceneCalculateGroup(s32 scene_calculate_group_index) const
{
  return m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack.h3d_model_array->GetElementNum();
}
#endif 

//-----------------------------------------------------------------------------
/*
 *  @brief         �S�V�[���v�Z�O���[�v�ɒǉ�����Ă���V�[���m�[�h�̌������v���Ď擾����
 */
//-----------------------------------------------------------------------------
#define GFL_G3D_SCENE_GET_SCENE_NODE_NUM(T_SCENE_NODE_CLASS) \
  u32 num = 0; \
  for(s32 i=0; i<m_scene_calculate_group_num; ++i ) \
  { \
    num += Get##T_SCENE_NODE_CLASS##NumInSceneCalculateGroup(i); \
  } \
  return num;

u32 Scene::GetModelNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(Model)
}
u32 Scene::GetLightNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(Light)
}
u32 Scene::GetCameraNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(Camera)
}
u32 Scene::GetFogNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(Fog)
}
u32 Scene::GetParticleNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(Particle)
}
u32 Scene::GetH3dModelNum(void) const
{
  GFL_G3D_SCENE_GET_SCENE_NODE_NUM(H3dModel)
}

#undef GFL_G3D_SCENE_GET_SCENE_NODE_NUM

//-----------------------------------------------------------------------------
/*
 *  @brief         �S�V�[���v�Z�O���[�v�ɒǉ�����Ă��郂�f���̃}�e���A���̌������v���Ď擾����
 */
//-----------------------------------------------------------------------------
u32 Scene::GetModelMaterialCount(void) const
{
  u32 num = 0;
  for(s32 i=0; i<m_scene_calculate_group_num; ++i)
  {
    SceneNodePropertyPack* scene_node_property_pack = &(m_scene_calculate_group_pack_array[i].scene_node_property_pack);
    ModelNodePropertyArray* property_array = scene_node_property_pack->model_array;

    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      gfl::grp::g3d::Model* model = reinterpret_cast<gfl::grp::g3d::Model*>(element->data.scene_node);
      num += model->GetMaterialCount();
      element = element->next;
    }
  }
  return num;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         Scene�����L���Ǘ�������̂̌����擾����(�V�[���v�Z�O���[�v�ɓo�^����Ă��邩�ǂ����͌��Ă��Ȃ�)
 */
//-----------------------------------------------------------------------------
u32 Scene::GetOwnModelNum(void) const
{
  return m_scene_own_object_pack->model_num;
}
u32 Scene::GetOwnLightNum(void) const
{
  return m_scene_own_object_pack->light_num;
}
u32 Scene::GetOwnCameraNum(void) const
{
  return m_scene_own_object_pack->camera_num;
}
u32 Scene::GetOwnFogNum(void) const
{
  return m_scene_own_object_pack->fog_num;
}
u32 Scene::GetOwnSceneEnvironmentSettingNum(void) const
{
  return m_scene_own_object_pack->scene_environment_setting_num;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���`��O���[�v�ɓo�^����Ă���V�[���m�[�h�̌����擾����
 *                 �S�V�[���v�Z�O���[�v�𑖍�����̂Ŏ��Ԃ��|����܂��B
 */
//-----------------------------------------------------------------------------
#define G3D_SCENE_GET_NUM_IN_SCENE_DRAW_GROUP(T_SCENE_NODE_CLASS) \
  u32 num = 0; \
  for( u32 i=0; i<m_scene_calculate_group_num; ++i ) \
  { \
    SceneNodePropertyPack* scene_node_property_pack = &(m_scene_calculate_group_pack_array[i].scene_node_property_pack); \
    const ModelNodePropertyElement* sentinel = get##T_SCENE_NODE_CLASS##SentinelElement(scene_node_property_pack); \
    ModelNodePropertyElement*       element  = get##T_SCENE_NODE_CLASS##ElementHead(scene_node_property_pack); \
    while(element != sentinel) \
    { \
      if( element->data.scene_draw_group_bit->IsOn(scene_draw_group_index) ) \
      { \
        ++num; \
      } \
      element = element->next; \
    } \
  } \
  return num;

u32 Scene::GetModelNumInSceneDrawGroup(s32 scene_draw_group_index) const
{
  G3D_SCENE_GET_NUM_IN_SCENE_DRAW_GROUP(Model)
}
u32 Scene::GetParticleNumInSceneDrawGroup(s32 scene_draw_group_index) const
{
  G3D_SCENE_GET_NUM_IN_SCENE_DRAW_GROUP(Particle)
}

#undef G3D_SCENE_GET_NUM_IN_SCENE_DRAW_GROUP



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �e
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         ShadowSystem
 */
//-----------------------------------------------------------------------------
// �e�̐���
// CreateShadowSystem���Ă񂾌�͕K��Scene::Initialize���Ă�ŉ������B
void Scene::CreateShadowSystem(
    gfl::heap::NwAllocator*                               heap_allocator,
    gfl::heap::NwAllocator*                               device_allocator,
    const gfl::grp::g3d::ShadowSystem::Description* description )
{
  if( !m_shadow_system )
  {
    m_shadow_system = GFL_NEW(heap_allocator->GetHeapBase()) ShadowSystem;
    m_shadow_system->Create(
        heap_allocator,
        device_allocator,
        description );

    for( u32 i=0; i<m_shadow_system->GetShadowGroupNum(); ++i)
    {
      s32                    shadow_scene_draw_group_index = m_shadow_system->GetShadowGroupSceneDrawGroupIndex(i);
      s32                    shadow_camera_index           = m_shadow_system->GetShadowGroupCameraIndex(i);
      gfl::grp::g3d::Camera* shadow_camera                 = m_shadow_system->GetShadowGroupCamera(i);

      m_shadow_system->SetupSceneDrawGroupForShadow(
          m_shadow_system->GetShadowGroupType(i),
          m_scene_draw_group_pack_array[shadow_scene_draw_group_index].scene_draw_group
      );
 
      AddSceneNode( shadow_camera );

      AssignCameraIndex(shadow_camera, shadow_camera_index);
      SetActiveCameraIndex(shadow_camera_index, shadow_scene_draw_group_index);
    }
  }
}

// �e�̔j��
void Scene::DestroyShadowSystem(void)
{
  if( m_shadow_system )
  {
    for( u32 i=0; i<m_shadow_system->GetShadowGroupNum(); ++i)
    {
      gfl::grp::g3d::Camera* shadow_camera                 = m_shadow_system->GetShadowGroupCamera(i);
      RemoveSceneNode( shadow_camera );
    }

    m_shadow_system->Destroy();
    GFL_DELETE m_shadow_system;
    m_shadow_system = NULL;
  }
}

// �e�̃L���X�^�[��On/Off
void Scene::SwitchShadowCasterModel(Model* model, const b32 cast_on, const s32 shadow_group_index)
{
  if( m_shadow_system )
  {
    s32 scene_draw_group_index = m_shadow_system->GetShadowGroupSceneDrawGroupIndex(shadow_group_index);
    if( cast_on )
    {
      AddModelToSceneDrawGroup(model, scene_draw_group_index );
    }
    else
    {
      RemoveModelFromSceneDrawGroup(model, scene_draw_group_index );
    }
  }
}

// �e�̃��V�[�o�[�̃Z�b�g�A�b�v
void Scene::SetupShadowReceiverModel(
    Model*                                 model,
    const s32                              material_index,
    const f32                              shadow_intensity,
    const ShadowSystem::ReceiverType receiver_type,
    const s32                              shadow_group_index)
{
  if( m_shadow_system )
  {
    m_shadow_system->SetupShadowReceiverModel(model, material_index, shadow_group_index, receiver_type, shadow_intensity);
  }
}

/*
  @note ���쒆
// �e�̃��V�[�o�[��On/Off
void Scene::SwitchShadowReceiverModel(
    Model*                                 model,
    const s32                              material_index,
    const ShadowSystem::ReceiverType receiver_type,
    const b32                              receive_on,
    const s32                              shadow_group_index);
)
{
  if( m_shadow_system )
  {
    m_shadow_system->SwitchShadowReceiverModel(model, material_index, shadow_group_index, receiver_type, shadow_intensity, receive_on);
  }
}
*/

// �e�̃��V�[�o�[�̉e�̋��x��ݒ肷��
void Scene::SetShadowReceiverModelShadowIntensity(
    Model*                                 model,
    const s32                              material_index,
    const f32                              shadow_intensity,
    const ShadowSystem::ReceiverType receiver_type,
    const s32                              shadow_group_index)  // @note �g���Ă��Ȃ�����������̂ŁA�����𐸍������ق��������B
{
  if( m_shadow_system )
  {
    m_shadow_system->SetShadowReceiverModelShadowIntensity(model, material_index, shadow_group_index, receiver_type, shadow_intensity);
  }
}

// �e��SubmitView
void Scene::SubmitViewOfShadow(const b32 update_camera_matrix)
{
  if( m_shadow_system )
  {
    for( u32 j=0; j<m_shadow_system->GetShadowGroupNum(); ++j )
    {
      s32                    shadow_scene_draw_group_index = m_shadow_system->GetShadowGroupSceneDrawGroupIndex(j);
      s32                    shadow_camera_index           = m_shadow_system->GetShadowGroupCameraIndex(j);
      gfl::grp::g3d::Camera* shadow_camera                 = m_shadow_system->GetShadowGroupCamera(j);

      if( update_camera_matrix )  // �e�O���[�v�S���̃J�����̃}�g���b�N�X���X�V����B
      {
        // @note gfx_LookAtTargetViewUpdater.cpp:121 Panic:NW:Failed assertion cameraPosition != targetPosition
        // �ɂȂ�u�Ԃ�����B���́H
        // �f�o�b�O���j���[�̒����ւ����I���������ɂȂ邱�Ƃ�����B���Ώ������邱�Ƃɂ������A�����˂��~�߂����B
        gfl::math::VEC3 camera_position;
        gfl::math::VEC3 target_position;
        shadow_camera->GetCameraAndTargetPosition(camera_position, target_position);
        if( !(    camera_position.x == target_position.x
               && camera_position.y == target_position.y
               && camera_position.z == target_position.z ) )
        {
          shadow_camera->UpdateCameraMatrix();
        }
      }

      SubmitView(
          shadow_camera,
          false,
          false,
          shadow_scene_draw_group_index
      );
    }
  }
}

// �e��Draw
void Scene::DrawShadow(const b32 command_cache_dump)
{
  if( m_shadow_system )
  {
    for( u32 i=0; i<m_shadow_system->GetShadowGroupNum(); ++i )
    {
      s32                     shadow_scene_draw_group_index = m_shadow_system->GetShadowGroupSceneDrawGroupIndex(i);
      s32                     shadow_camera_index           = m_shadow_system->GetShadowGroupCameraIndex(i);
      gfl::grp::g3d::Camera*  shadow_camera                 = m_shadow_system->GetShadowGroupCamera(i);
      gfl::grp::RenderTarget* shadow_render_target          = m_shadow_system->GetShadowGroupRenderTarget(i);

      shadow_render_target->ClearBuffer(
          true,
          gfl::grp::ColorF32(1.0f, 1.0f, 1.0f, 1.0f),
          //gfl::grp::ColorF32(0.0f, 0.0f, 0.0f, 0.0f),  // ������ɂ���Ɖe�̔̌`��������Ղ��Ȃ�܂��B
          false,
          1.0f
      );

      // �e�V�X�e���͂�����Ǝg���Ă���̂ŁA�V�X�e���Ŗ��ݒ���T�|�[�g����SetupNotSetCamera�ACleanupNotSetCamera�͕K�v�Ȃ��B

      m_render_system->RenderScene(
          //shadow_render_target->GetNwRenderTarget(),
          shadow_render_target,
          shadow_camera->GetNwCamera(),
          m_scene_draw_group_pack_array[shadow_scene_draw_group_index].scene_draw_group,
          command_cache_dump
      );
    
      m_scene_draw_group_pack_array[shadow_scene_draw_group_index].scene_draw_group->SetState(SceneDrawGroup::STATE_AFTER_DRAW);
    }
    
    for( u32 i=0; i<m_scene_calculate_group_num; ++i )
    {
      m_scene_system->SetParticleEnableSwapBuffer(m_scene_calculate_group_pack_array[i].scene_calculate_group, true);
    }
  }
}

// �e�̏����擾
gfl::grp::g3d::Camera* Scene::GetShadowGroupCamera(const s32 shadow_group_index) const
{
  if( m_shadow_system )
  {
    return m_shadow_system->GetShadowGroupCamera(shadow_group_index);
  }
  return NULL;
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �G�b�W
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�V�X�e�������֐�
   *
   * @param heap_allocator          ���������Ɋm�ۂ��郁����������������܂�
   * @param device_allocator        ���������Ɋm�ۂ��郁����������������܂�
   * @param scene_draw_group_index  �\���O���[�v
   * @param p_desc                  �����_�[�^�[�Q�b�g�ݒ���e
   */
  //-----------------------------------------------------------------------------
void Scene::CreateEdgeMapSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, s32 scene_draw_group_index, gfl::grp::RenderTarget::Description* p_desc, gfl::grp::RenderTarget::Description* p_low_desc, b32 f_delete_memory_control )
{
  if ( m_g3d_edge_map_system )    return;

  m_g3d_edge_map_system = GFL_NEW(heap_allocator->GetHeapBase()) G3DEdgeMapSystem();
  m_g3d_edge_map_system->CreateWork( heap_allocator, device_allocator, p_desc, p_low_desc, f_delete_memory_control );

  m_g3d_edge_map_system->SetGroupIndex( scene_draw_group_index );
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W�}�b�v�V�X�e���j��
 */
//-----------------------------------------------------------------------------
void Scene::DestroyEdgeMapSystem(void)
{
  if ( m_g3d_edge_map_system == NULL )    return;
  
  GFL_SAFE_DELETE( m_g3d_edge_map_system );
}

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��ݒ�
   *
   * @param model     ���f���I�u�W�F�N�g
   * @param cast_on   �L������
   */
  //-----------------------------------------------------------------------------
void Scene::SwitchEdgeMapModel( Model* model, const b32 cast_on , const s32 group_index )
{
  if( m_g3d_edge_map_system )
  {
    const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );

    if ( cast_on )
    {
      AddModelToSceneDrawGroup( model, group );
      model->GetNwModel()->PreRenderSignal().Connect( m_g3d_edge_map_system->GetPreRenderCallbackSlot() );
      model->GetNwModel()->PostRenderSignal().Connect( m_g3d_edge_map_system->GetPostRenderCallbackSlot() );
    }
    else
    {
      RemoveModelFromSceneDrawGroup( model, group );
      model->GetNwModel()->PreRenderSignal().Disconnect( m_g3d_edge_map_system->GetPreRenderCallbackSlot() );
      model->GetNwModel()->PostRenderSignal().Disconnect( m_g3d_edge_map_system->GetPostRenderCallbackSlot() );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreateEdgeMapSystem�֐���EdgeMapSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
/**
  * @brief �G�b�W�}�b�v�`��ݒ�
  *
  * @param model     H3d���f���I�u�W�F�N�g
  * @param cast_on   �L������
  */
//-----------------------------------------------------------------------------
void Scene::SwitchEdgeMapModel( H3dModel* model, const b32 cast_on , const s32 group_index )
{
  if( m_g3d_edge_map_system )
  {
    const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );

    if ( cast_on )
    {
      AddH3dModelToDraw( model, group );
      model->AddPreMeshDrawListener( m_g3d_edge_map_system->GetPreRenderCallbackSlot() );
      model->AddPostMeshDrawListener( m_g3d_edge_map_system->GetPostRenderCallbackSlot() );
    }
    else
    {
      RemoveH3dModelFromDraw( model, group );
      model->RemovePreMeshDrawListener( m_g3d_edge_map_system->GetPreRenderCallbackSlot() );
      model->RemovePostMeshDrawListener( m_g3d_edge_map_system->GetPostRenderCallbackSlot() );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreateEdgeMapSystem�֐���EdgeMapSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �G�b�W�`�揈��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     display_type           �\����
 *  @param[in]     camera                 ���̃V�[����`�悷��J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::EdgeMapDraw(
  gfl::grp::GraphicsSystem*     graphics_system,
  gfl::grp::DisplayType         display_type,
  Camera*                       camera,
  const s32                     group_index,
  bool                          command_cache_dump
)
{
  if( m_g3d_edge_map_system && m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

    const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );

    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(camera, group, &is_assign_camera_index, &is_set_active_camera_index);

    gfl::grp::RenderTarget* render_target = m_g3d_edge_map_system->GetEdgeRenderTarget( display_type );

    if( graphics_system->BindCurrentCommandList( render_target ) )
    {
      render_target->ClearBuffer( true, gfl::grp::ColorF32(0.5f, 0.5f, 0.0f, 1.0f), true, 1.0f, true, 0xFF );

      m_g3d_edge_map_system->RenderCallbackEnable(true);
      Draw(graphics_system, camera, render_target, group, command_cache_dump);  
      m_g3d_edge_map_system->RenderCallbackEnable(false);
    }

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(camera, group, is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "PostEffectSystem�AEdgeMapSystem�����������Ă��������B\n" );
  }
}

void Scene::EdgeMapDraw(
    gfl::grp::GraphicsSystem*     graphics_system,
    gfl::grp::RenderTarget*       command_target,
    Camera*                       camera,
    const s32                     group_index,
    bool                          command_cache_dump
  )
{
  if( m_g3d_edge_map_system && m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

    if ( command_target == NULL )
      return;

    const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );

    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(camera, group, &is_assign_camera_index, &is_set_active_camera_index);

    gfl::grp::RenderTarget* render_target = m_g3d_edge_map_system->GetEdgeRenderTarget( 
        //graphics_system->GetCurrentFrameBufferID(command_target->GetDisplayType())
        gfl::grp::DISPLAY_NONE );

    if( graphics_system->BindCurrentCommandList( render_target ) )
    {
      render_target->ClearBuffer( true, gfl::grp::ColorF32(0.5f, 0.5f, 0.0f, 1.0f), true, 1.0f, true, 0xFF );

      {
        f32   left, top, right, bottom;
        command_target->GetViewport( &left, &top, &right, &bottom );
        render_target->SetViewport( left, top, right, bottom );
      }

      m_g3d_edge_map_system->RenderCallbackEnable(true);
      Draw(graphics_system, camera, render_target, group, command_cache_dump);  
      m_g3d_edge_map_system->RenderCallbackEnable(false);
    }

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(camera, group, is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "PostEffectSystem�AEdgeMapSystem�����������Ă��������B\n" );
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �G�b�W�X�e���I�`�揈��
 *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
 *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
   *  @param[in]     group_index            �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   *  @param[in]     clear_buffer           �o�b�t�@���N���A���邩
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::StereoEdgeMapDraw(
  gfl::grp::GraphicsSystem*    graphics_system,
  StereoCamera*                camera,
  const s32                    group_index,
  bool                         clear_buffer,
  bool                         command_cache_dump
)
{
  EdgeMapDrawInfo     info;
  
  info.graphics_system = graphics_system;
  info.camera = camera;
  info.group_index = group_index;
  info.clear_color = clear_buffer;
  info.clear_zbuffer = clear_buffer;
  info.command_cache_dump = command_cache_dump;
  info.left_render_target  = m_g3d_edge_map_system->GetEdgeRenderTarget( gfl::grp::DISPLAY_UPPER );
  info.right_render_target = m_g3d_edge_map_system->GetEdgeRenderTarget( gfl::grp::DISPLAY_UPPER_RIGHT );
    
  StereoEdgeMapDraw( info );
}

void Scene::StereoEdgeMapDraw( const EdgeMapDrawInfo &rInfo )
{
  if( m_g3d_edge_map_system && m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

    const s32 group = ( rInfo.group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : rInfo.group_index );

    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(rInfo.camera, group, &is_assign_camera_index, &is_set_active_camera_index);

    if( rInfo.clear_color || rInfo.clear_zbuffer )
    {
      if( rInfo.graphics_system->BindCurrentCommandList( rInfo.left_render_target ) )
      {
        rInfo.left_render_target->ClearBuffer( rInfo.clear_color, gfl::grp::ColorF32(0.5f, 0.5f, 0.0f, 1.0f), rInfo.clear_zbuffer, 1.0f, rInfo.clear_zbuffer, 0xFF );
      }

      if( rInfo.graphics_system->BindCurrentCommandList( rInfo.right_render_target ) )
      {
        rInfo.right_render_target->ClearBuffer( rInfo.clear_color, gfl::grp::ColorF32(0.5f, 0.5f, 0.0f, 1.0f), rInfo.clear_zbuffer, 1.0f, rInfo.clear_zbuffer, 0xFF );
      }
    }

    m_g3d_edge_map_system->RenderCallbackEnable(true);
    CommandSaveStereoDraw(
        rInfo.graphics_system,
        rInfo.camera->GetLeftCamera(),  rInfo.left_render_target,
        rInfo.camera->GetRightCamera(), rInfo.right_render_target,
  #if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
        group,
        rInfo.camera->GetBaseCamera(),
        rInfo.command_cache_dump
  #else
        group,
        rInfo.command_cache_dump
  #endif
    );
    m_g3d_edge_map_system->RenderCallbackEnable(false);

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(rInfo.camera, group, is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "PostEffectSystem�AEdgeMapSystem�����������Ă��������B\n" );
  }
}

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �|�X�g����
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
  /**
   * @brief �|�X�g�����V�X�e������
   * @param heap_allocator      �����ɗp����A���P�[�^�[
   * @param device_allocator    �����ɗp����A���P�[�^�[
   * @param p_desc              �������ݒ�
   */
  //-----------------------------------------------------------------------------
void Scene::CreatePostEffectSystem(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc,
    const gfl::grp::GraphicsSystem* graphics_system,
    const gfl::grp::MemoryArea      frame_buffer_memory_area
)
{
  if ( m_g3d_post_effect_system == NULL )
  {
    m_g3d_post_effect_system = GFL_NEW(heap_allocator->GetHeapBase()) G3DPostEffectSystem();
    m_g3d_post_effect_system->CreateWork( heap_allocator, device_allocator, p_desc,
        graphics_system, frame_buffer_memory_area );
    
    SetUpEdgeFilter( heap_allocator, graphics_system, p_desc );
    
    // �u���[���t�B���^
    {
      CreateBloomFilter();
    }
  }
}

//-----------------------------------------------------------------------------
  /**
   * @brief �|�X�g�����V�X�e���j��
   */
  //-----------------------------------------------------------------------------
// �����1�t���[���x�������ł��Ă��Ȃ��̂ŁA���ӂ��Ďg�p���ĉ������B
void Scene::DestroyPostEffectSystem()
{
  // �u���[���t�B���^
  {
    DestroyBloomFilter();
  }

  if ( m_g3d_post_effect_system == NULL )    return;
  GFL_SAFE_DELETE( m_g3d_post_effect_system );
}

//-----------------------------------------------------------------------------
/// @brief
//-----------------------------------------------------------------------------
void Scene::SetPostEffectOrder()
{
   //�܂������
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W�L�������ݒ�
 */
//-----------------------------------------------------------------------------
void Scene::SetEdgeEnable( b32 f)
{
   if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;
   m_g3d_post_effect_system->SetEdgeEnable(f);
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W���L�����H
 * @return �L�������t���O
 */
//-----------------------------------------------------------------------------
b32 Scene::IsEdgeEnable()
{
  if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return false;
  return m_g3d_post_effect_system->IsEdgeEnable();
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W�^�C�v�w��
 * @param type     �^�C�v
 */
//-----------------------------------------------------------------------------
void Scene::SetEdgeType( G3DEdgeFilter::EdgeType type )
{
  if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;
  m_g3d_post_effect_system->m_pG3DEdgeFilter->SetEdgeType(type);
}

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�̑�������
   * @param scale     �X�P�[���l
   */
  //-----------------------------------------------------------------------------
void Scene::SetEdgeWidthScale( f32 scale )
{
  if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;
  m_g3d_post_effect_system->m_pG3DEdgeFilter->ScaleEdgeWidth(scale);
}

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�̐F�ݒ�
   * @param scale     �X�P�[���l
   */
  //-----------------------------------------------------------------------------
void Scene::SetEdgeColor( f32 r, f32 g, f32 b, f32 a )
{
  if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;
  m_g3d_post_effect_system->m_pG3DEdgeFilter->SetEdgeColor(r, g, b, a);
}

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��O���[�v�pSubmitView�֐�
   *
   * @param camera �J����
   * @param update_camera_matrix
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
void Scene::SubmitViewOfEdge( StereoCamera* camera, bool update_camera_matrix , const s32 group_index )
{
  if( m_g3d_post_effect_system )
  {  
    if ( m_g3d_edge_map_system )
    {
      const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );
      SubmitView( camera, false, false, group );
    }

    if ( m_g3d_post_effect_system->IsEdgeEnable() )
    {    
      SubmitView( camera, update_camera_matrix, false, m_g3d_post_effect_system->GetEdgeGroupIndex() );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
  /**
   * @brief �G�b�W�}�b�v�`��O���[�v�pSubmitView�֐�
   *
   * @param camera �J����
   * @param update_camera_matrix
   * @param group_index �O���[�v�ԍ�(SCENE_DRAW_GROUP_INDEX_NONE�ŃV�X�e���̒l���g��)
   */
  //-----------------------------------------------------------------------------
void Scene::SubmitViewOfEdge( Camera* camera, bool update_camera_matrix , const s32 group_index )
{
  if( m_g3d_post_effect_system && m_g3d_edge_map_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() )
    {
      const s32 group = ( group_index==SCENE_DRAW_GROUP_INDEX_NONE ? m_g3d_edge_map_system->GetGroupIndex() : group_index );
      SubmitView( camera, update_camera_matrix, false, m_g3d_post_effect_system->GetEdgeGroupIndex() );
      SubmitView( camera, false, false, group );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "EdgeMapSystem, PostEffectSystem�����������Ă��������B\n" );
  }
}
  
//-----------------------------------------------------------------------------
/*
 *  @brief         �|�X�g�G�t�F�N�g�`�揈��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     display_type           �\����
 *  @param[in]     camera                 ���̃V�[����`�悷��J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::PostEffectDraw( gfl::grp::GraphicsSystem* graphics_system, gfl::grp::DisplayType display_type, Camera* camera, bool command_cache_dump )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() )
    {
      DrawPostEdge( graphics_system, display_type, camera, command_cache_dump );
    }

    // �u���[���t�B���^
    {
      DrawBloomPlainImageFrameBuffer(
        graphics_system,
        display_type,
        graphics_system->GetCurrentFrameBuffer(display_type),
        command_cache_dump
      );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �|�X�g�G�t�F�N�g�`�揈��
 *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::StereoPostEffectDraw( gfl::grp::GraphicsSystem* graphics_system, StereoCamera* camera, bool command_cache_dump )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() )
    {
      StereoDrawPostEdge( graphics_system, camera, command_cache_dump );
    }

    // �u���[���t�B���^
    {
      StereoDrawBloomPlainImageFrameBuffer(
        graphics_system,
        graphics_system->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER),
        graphics_system->GetCurrentFrameBuffer(gfl::grp::DISPLAY_UPPER_RIGHT),
        command_cache_dump
      );
    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �G�b�W�`�揈��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     display_type           �\����
 *  @param[in]     camera                 ���̃V�[����`�悷��J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::DrawPostEdge( gfl::grp::GraphicsSystem* graphics_system, gfl::grp::DisplayType display_type, Camera* camera,  bool command_cache_dump )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), &is_assign_camera_index, &is_set_active_camera_index);

    gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
    gfl::grp::RenderTarget* render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(display_type) );

    m_g3d_post_effect_system->m_pG3DEdgeFilter->SetDisplayType(display_type);

    Draw(graphics_system, camera, render_target, m_g3d_post_effect_system->GetEdgeGroupIndex(), command_cache_dump);  

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
  /*
   *  @brief         �|�X�g�G�t�F�N�g�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     render_target          �\���^�[�Q�b�g
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
   */
  //-----------------------------------------------------------------------------
void Scene::DrawPostEdge( gfl::grp::GraphicsSystem* graphics_system, gfl::grp::RenderTarget* render_target, Camera* camera,  bool command_cache_dump )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), &is_assign_camera_index, &is_set_active_camera_index);

    {
      f32   left, top, right, bottom;
      render_target->GetViewport( &left, &top, &right, &bottom );
      m_g3d_post_effect_system->m_pG3DEdgeFilter->ScaleEdgeWidth( right - left, bottom - top );
    }

    Draw(graphics_system, camera, render_target, m_g3d_post_effect_system->GetEdgeGroupIndex(), command_cache_dump);  

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �G�b�W�X�e���I�`�揈��
 *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
 *  @param[in]     command_cache_dump     �R�}���h�L���b�V�������O�o�͂���
 */
//-----------------------------------------------------------------------------
void Scene::StereoDrawPostEdge( gfl::grp::GraphicsSystem* graphics_system, StereoCamera* camera, bool command_cache_dump )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->IsEdgeEnable() == false )
      return;

  #if 0
  /*  �V�[���`��O���[�v0�Ԃ͊Ԉ���Ă���Ǝv��
    AssignCameraIndex( camera, 0 );
    SetActiveCameraIndex( 0, 0 );*/
  #endif
    // �V�X�e���Ŗ��ݒ���T�|�[�g����
    b32 is_assign_camera_index;
    b32 is_set_active_camera_index;
    setupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), &is_assign_camera_index, &is_set_active_camera_index);

    gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
    gfl::grp::RenderTarget* left_render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER) );
    gfl::grp::RenderTarget* right_render_target = frame_buffer_manager->GetRenderTarget( graphics_system->GetCurrentFrameBufferID(gfl::grp::DISPLAY_UPPER_RIGHT) );

    m_g3d_post_effect_system->m_pG3DEdgeFilter->SetDisplayType( gfl::grp::DISPLAY_UPPER );

    CommandSaveStereoDraw(
        graphics_system,
        camera->GetLeftCamera(),  left_render_target,
        camera->GetRightCamera(), right_render_target,
  #if GFL_GRP_G3D_H3D_CALCULATE_BILLBOARD_WHEN_DRAW_MESH
        m_g3d_post_effect_system->GetEdgeGroupIndex(),
        camera->GetBaseCamera(),
        command_cache_dump
  #else
        m_g3d_post_effect_system->GetEdgeGroupIndex(),
        command_cache_dump
  #endif
    );

    // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
    cleanupNotSetCamera(camera, m_g3d_post_effect_system->GetEdgeGroupIndex(), is_assign_camera_index, is_set_active_camera_index);
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// ��ʊE�[�x
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/**
 * @brief ��ʊE�[�x�V�X�e�������֐�
 *
 * @param heap_allocator          ���������Ɋm�ۂ��郁����������������܂�
 * @param device_allocator        ���������Ɋm�ۂ��郁����������������܂�
 * @param scene_draw_group_index  �\���O���[�v
 * @param p_desc                  �����_�[�^�[�Q�b�g�ݒ���e
 */
//-----------------------------------------------------------------------------
void Scene::CreateDepthOfFieldSystem( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, gfl::grp::GraphicsSystem* graphics_system )
{
  if ( m_G3DDepthOfFieldSystem )    return;
  
  gfl::grp::RenderTarget::Description   desc;
  
  if ( m_g3d_edge_map_system )
  {
    gfl::grp::RenderTarget                *render_target;
    render_target  = m_g3d_edge_map_system->GetEdgeRenderTarget( gfl::grp::DISPLAY_UPPER );

    desc.color_address = render_target->GetColorAddress();
    desc.depth_address = render_target->GetDepthAddress();
    desc.depth_area = gfl::grp::MEMORY_AREA_VRAMB;
  }

  m_G3DDepthOfFieldSystem = GFL_NEW(heap_allocator->GetHeapBase()) G3DDepthOfFieldSystem();
  m_G3DDepthOfFieldSystem->CreateWork( graphics_system, heap_allocator, device_allocator, &desc );
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ʊE�[�x�V�X�e���j��
 */
//-----------------------------------------------------------------------------
void Scene::DestroyDepthOfFieldSystem(void)
{
  if ( m_G3DDepthOfFieldSystem == NULL )    return;
  
  GFL_SAFE_DELETE( m_G3DDepthOfFieldSystem );
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ʊE�[�x�^�C�v�w��
 */
//-----------------------------------------------------------------------------
void Scene::SetDofType( DofType::Enum type )
{
  if ( m_G3DDepthOfFieldSystem == NULL )    return;
  
  m_G3DDepthOfFieldSystem->SetDofType( type );
}

//-----------------------------------------------------------------------------
/**
 * @brief ��ʊE�[�x�t�H�[�J�X�����w��
 */
//-----------------------------------------------------------------------------
void Scene::SetFocusLength( f32 focus )
{
  if ( m_G3DDepthOfFieldSystem == NULL )    return;
  
  m_G3DDepthOfFieldSystem->SetFocusLength( focus );
}

//-----------------------------------------------------------------------------
  /**
   * @brielf    �ڂ₯���C���[�ݒ�i�ő�4�i�K�j
   * @param layer             �ڂ₯���C���[�ԍ�
   * @param rangeDistance     �e���͈�
   * @param blurred           �ڂ₯�( �ŏ�0.0f ? 1.0f�ő� )
   */
  //-----------------------------------------------------------------------------
  void Scene::SetFocusRange( u32 layer, f32 rangeDistance, f32 blurred )
  {
    if ( m_G3DDepthOfFieldSystem == NULL )    return;
    
    m_G3DDepthOfFieldSystem->SetFocusRange( layer, rangeDistance, blurred );
  }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   �ڂ₯�ݒ胊�Z�b�g
   */
  //-----------------------------------------------------------------------------
  void Scene::ReSetFocusRange()
  {
    if ( m_G3DDepthOfFieldSystem == NULL )    return;
    
    m_G3DDepthOfFieldSystem->ReSetFocusRange();
  }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief   
   */
  //-----------------------------------------------------------------------------
  void Scene::SetBlurScale( f32 blurScale )
  {
    if ( m_G3DDepthOfFieldSystem == NULL )    return;
    
    m_G3DDepthOfFieldSystem->SetBlurScale( blurScale );
  }
  
  //-----------------------------------------------------------------------------
  /*
   *  @brief         ��ʊE�[�x�`�揈��
   *
   *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
   *  @param[in]     display_type           �\����
   *  @param[in]     camera                 ���̃V�[����`�悷��J����
   */
  //-----------------------------------------------------------------------------
  void Scene::DrawPostDepthOfField( gfl::grp::GraphicsSystem* graphics_system, gfl::grp::DisplayType display_type, Camera* camera )
  {
     if ( m_G3DDepthOfFieldSystem == NULL )    return;
 
#if 0
    /*�ȈՕ`�悵�����ĂȂ�����������ݒ肵�Ȃ��Ă����Ǝv��(g3d����ȈՕ`����ĂԂ͔̂@���Ȃ��̂�)
    AssignCameraIndex( camera, 0 );
    SetActiveCameraIndex( 0, 0 );*/
#endif

    gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
    m_G3DDepthOfFieldSystem->DrawDepthOfField( display_type, graphics_system, frame_buffer_manager, camera );
  }

//-----------------------------------------------------------------------------
/*
 *  @brief         �G�b�W�X�e���I�`�揈��
 *                 �㍶�ډ��(DISPLAY_UPPER)�A��E�ډ��(DISPLAY_UPPER_RIGHT)�ɕ`�悷��
 *
 *  @param[in]     graphics_system        �O���t�B�b�N�X�V�X�e��
 *  @param[in]     camera                 ���̃V�[����`�悷��X�e���I�J����
 */
//-----------------------------------------------------------------------------
void Scene::StereoDrawPostDepthOfField( gfl::grp::GraphicsSystem* graphics_system, StereoCamera* camera )
{
  if ( m_G3DDepthOfFieldSystem == NULL )    return;
  
#if 0
  /*�ȈՕ`�悵�����ĂȂ�����������ݒ肵�Ȃ��Ă����Ǝv��(g3d����ȈՕ`����ĂԂ͔̂@���Ȃ��̂�)
  AssignCameraIndex( camera, 0 );
  SetActiveCameraIndex( 0, 0 );*/
#endif

  gfl::grp::FrameBufferManager* frame_buffer_manager = graphics_system->GetFrameBufferManager();
  
  m_G3DDepthOfFieldSystem->DrawDepthOfField( gfl::grp::DISPLAY_UPPER, graphics_system, frame_buffer_manager, camera );
  m_G3DDepthOfFieldSystem->DrawDepthOfField( gfl::grp::DISPLAY_UPPER_RIGHT, graphics_system, frame_buffer_manager, camera );
}

//-----------------------------------------------------------------------------
/*
  *  @brief         ��ʊE�[�x�p�`��o�b�t�@�擾
  *
  *  @param[in]     DisplayType        DisplayType
  */
//-----------------------------------------------------------------------------
gfl::grp::RenderTarget* Scene::GetDepthOfFieldRenderTarget(DisplayType type)
{
  if ( m_G3DDepthOfFieldSystem == NULL )
    return NULL;
  
  return m_G3DDepthOfFieldSystem->m_render_target[type];
}

//-----------------------------------------------------------------------------
/**
 * @brief �G�b�W�t�B���^�̃Z�b�g�A�b�v
 */
//-----------------------------------------------------------------------------
void Scene::SetUpEdgeFilter( gfl::heap::NwAllocator* heap_allocator, const gfl::grp::GraphicsSystem* graphics_system, gfl::grp::g3d::G3DPostEffectSystem::Description* p_desc )
{
  if( m_g3d_post_effect_system )
  {
    if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;

    if ( m_g3d_edge_map_system )
    {
      nw::gfx::res::ResTexture res_texture = m_g3d_edge_map_system->GetEdgeRenderTarget(gfl::grp::DISPLAY_UPPER)->GetNwResTexture();
      m_g3d_post_effect_system->m_pG3DEdgeFilter->SetColorTarget( res_texture );
    }

    {
      if ( graphics_system )
      {
        m_g3d_post_effect_system->m_pG3DEdgeFilter->CreateDepthTarget( heap_allocator, graphics_system );
      }

      for( s32 i = 0; i < G3DEdgeFilter::TYPE_MAX; ++i )
      {
        for( s32 no = 0; no < G3DEdgeFilter::MAX_PATH; ++no )
        {//���f���𔲂��o���V�[���ɓo�^
          gfl::grp::g3d::Model*   pModel = m_g3d_post_effect_system->m_pG3DEdgeFilter->GetShaderModel( i, no );
          if ( pModel == NULL )   continue;

          AddSceneNode( pModel );
          RemoveModelFromSceneDrawGroup(pModel, 0);
          AddModelToSceneDrawGroup( pModel, m_g3d_post_effect_system->GetEdgeGroupIndex() );
        }
      }

      m_g3d_post_effect_system->m_pG3DEdgeFilter->SetEdgeType( p_desc->edge_type );

    }
  }
  else
  {
    GFL_ASSERT_MSG( 0, "Scene::CreatePostEffectSystem�֐���PostEffectSystem�����������Ă��������B\n" );
  }
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// BloomFilter
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         BloomFilter
 */
//-----------------------------------------------------------------------------
// CreateBloomFilter���Ă񂾌�͕K��Scene::Initialize���Ă�ŉ������B
void Scene::CreateBloomFilter(void)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �����A�j���͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B

  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();
  
  if( l_bloom_filter )
  {
    AddCameraToSceneCalculateGroup( l_bloom_filter->GetCamera(), l_bloom_filter->GetCameraSceneCalculateGroupIndex() );
    // �_�~�[�J����������A�J�����C���f�b�N�X�͊���U��Ȃ��Ă������ȁB
    AssignCameraIndex( l_bloom_filter->GetCamera(), l_bloom_filter->GetCameraSceneCameraIndex() );
    SetActiveCameraIndex( l_bloom_filter->GetCameraSceneCameraIndex(), l_bloom_filter->GetHighLuminanceBoardSceneDrawGroupIndex() );
    SetActiveCameraIndex( l_bloom_filter->GetCameraSceneCameraIndex(), l_bloom_filter->GetBloomBoardSceneDrawGroupIndex() );

    AddModelToSceneCalculateGroup ( l_bloom_filter->GetHighLuminanceBoardModel(), l_bloom_filter->GetHighLuminanceBoardSceneCalculateGroupIndex() );
    RemoveModelFromAllSceneDrawGroup( l_bloom_filter->GetHighLuminanceBoardModel() );
    AddModelToSceneDrawGroup( l_bloom_filter->GetHighLuminanceBoardModel(), l_bloom_filter->GetHighLuminanceBoardSceneDrawGroupIndex() );

    for( u32 i=0; i<l_bloom_filter->GetBloomBoardNum(); ++i )
    {
      AddModelToSceneCalculateGroup ( l_bloom_filter->GetBloomBoardModel(i), l_bloom_filter->GetBloomBoardSceneCalculateGroupIndex() );
      RemoveModelFromAllSceneDrawGroup( l_bloom_filter->GetBloomBoardModel(i) );
      AddModelToSceneDrawGroup( l_bloom_filter->GetBloomBoardModel(i), l_bloom_filter->GetBloomBoardSceneDrawGroupIndex() );
    }
  }
}

// �����1�t���[���x�������ł��Ă��Ȃ��̂ŁA���ӂ��Ďg�p���ĉ������B
void Scene::DestroyBloomFilter(void)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �����A�j���͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();
  
  if( l_bloom_filter )
  {
    RemoveCameraFromSceneCalculateGroup( l_bloom_filter->GetCamera() );
    
    RemoveModelFromSceneCalculateGroup( l_bloom_filter->GetHighLuminanceBoardModel() );

    for( u32 i=0; i<l_bloom_filter->GetBloomBoardNum(); ++i )
    {
      RemoveModelFromSceneCalculateGroup( l_bloom_filter->GetBloomBoardModel(i) );
    }
  }
}
void Scene::SubmitViewOfBloom(void)
{
  if( !m_g3d_post_effect_system ) return;
  if( !m_g3d_post_effect_system->IsBloomEnable() ) return;
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    SubmitView(
      l_bloom_filter->GetCamera(),
      true,
      false,  // �t�H�O�̒l���u���[���̃J�����ōX�V���Ȃ��悤��false�ɂ��Ă���
      l_bloom_filter->GetHighLuminanceBoardSceneDrawGroupIndex()
    );
 
    SubmitView(
      l_bloom_filter->GetCamera(),
      false,
      false,
      l_bloom_filter->GetBloomBoardSceneDrawGroupIndex()
    );
  }
}

void Scene::DrawBloomPlainImageFrameBuffer(
    gfl::grp::GraphicsSystem*   graphics_system,
    const gfl::grp::DisplayType plain_image_display_type,
    gfl::grp::RenderTarget*     render_target,
    const b32 command_cache_dump
)
{
  GFL_ASSERT_STOP_MSG( plain_image_display_type<gfl::grp::DISPLAY_BOTH_MAX, "�f�B�X�v���C�^�C�v%d>=%d���ُ�ł��B\n", plain_image_display_type, gfl::grp::DISPLAY_BOTH_MAX );

  if( !m_g3d_post_effect_system ) return;
  if( !m_g3d_post_effect_system->IsBloomEnable() ) return;
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    if( graphics_system->BindCurrentCommandList(plain_image_display_type) )
    {

      //nngxSplitDrawCmdlist();  // ���̌�t���[���o�b�t�@���e�N�X�`���Ƃ��Ďg�p����̂ŕK�{
      
      l_bloom_filter->SetPlainImageFrameBufferToHighLuminanceBoard(graphics_system, plain_image_display_type);
   
      l_bloom_filter->GetHighLuminanceBoardRenderTarget()->SetDisplayType(plain_image_display_type);

      l_bloom_filter->GetHighLuminanceBoardRenderTarget()->ClearBuffer(
          true,
          gfl::grp::ColorF32(0.0f, 0.0f, 0.0f, 0.0f),
          false, 
          0.0f,
          false,
          0 );
      Draw(
        graphics_system,
        l_bloom_filter->GetCamera(),
        l_bloom_filter->GetHighLuminanceBoardRenderTarget(),
        l_bloom_filter->GetHighLuminanceBoardSceneDrawGroupIndex(),
        command_cache_dump
      );
      //nngxSplitDrawCmdlist();  // ���̌�e�N�X�`�����R�s�[����̂ŕK�{
      
      l_bloom_filter->CopySmallerAll();

      Draw(
        graphics_system,
        l_bloom_filter->GetCamera(),
        render_target,
        l_bloom_filter->GetBloomBoardSceneDrawGroupIndex(),
        command_cache_dump );
    }
  }
}

void Scene::DrawBloomPlainImageOffScreenBuffer(
    gfl::grp::GraphicsSystem*     graphics_system,
    const gfl::grp::RenderTarget* plain_image_off_screen_buffer,
    gfl::grp::RenderTarget*       render_target,
    const b32 command_cache_dump
)
{
  if( !m_g3d_post_effect_system ) return;
  if( !m_g3d_post_effect_system->IsBloomEnable() ) return;
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    gfl::grp::DisplayType display_type = plain_image_off_screen_buffer->GetDisplayType();

    if( graphics_system->BindCurrentCommandList(display_type) )
    {

      //nngxSplitDrawCmdlist();  // ���̌�t���[���o�b�t�@���e�N�X�`���Ƃ��Ďg�p����̂ŕK�{
      
      l_bloom_filter->SetPlainImageOffScreenBufferToHighLuminanceBoard(plain_image_off_screen_buffer);
    
      l_bloom_filter->GetHighLuminanceBoardRenderTarget()->SetDisplayType(display_type);

      l_bloom_filter->GetHighLuminanceBoardRenderTarget()->ClearBuffer(
          true,
          gfl::grp::ColorF32(0.0f, 0.0f, 0.0f, 0.0f),
          false, 
          0.0f,
          false,
          0 );
      Draw(
        graphics_system,
        l_bloom_filter->GetCamera(),
        l_bloom_filter->GetHighLuminanceBoardRenderTarget(),
        l_bloom_filter->GetHighLuminanceBoardSceneDrawGroupIndex(),
        command_cache_dump
      );
      //nngxSplitDrawCmdlist();  // ���̌�e�N�X�`�����R�s�[����̂ŕK�{
   
      l_bloom_filter->CopySmallerAll();

      Draw(
        graphics_system,
        l_bloom_filter->GetCamera(),
        render_target,
        l_bloom_filter->GetBloomBoardSceneDrawGroupIndex(),
        command_cache_dump );
    }
  }
}

void Scene::StereoDrawBloomPlainImageFrameBuffer(
    gfl::grp::GraphicsSystem* graphics_system,
    // plain_image��gfl::grp::DISPLAY_UPPER
    gfl::grp::RenderTarget* left_render_target,
    // plain_image��gfl::grp::DISPLAY_UPPER_RIGHT
    gfl::grp::RenderTarget* right_render_target,
    const b32 command_cache_dump
)
{
  DrawBloomPlainImageFrameBuffer(
      graphics_system,
      gfl::grp::DISPLAY_UPPER,
      left_render_target,
      command_cache_dump
  );
  DrawBloomPlainImageFrameBuffer(
      graphics_system,
      gfl::grp::DISPLAY_UPPER_RIGHT,
      right_render_target,
      command_cache_dump
  );
}

u32  Scene::GetBloomSmallImageNum(void) const
{
  if( !m_g3d_post_effect_system ) return 0;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetSmallImageNum();
  }
  return 0;
}

u32  Scene::GetBloomBloomBoardNum(void) const
{
  if( !m_g3d_post_effect_system ) return 0;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetBloomBoardNum();
  }
  return 0;
}

void Scene::SetBloomHighLuminanceColorWeight(const gfl::grp::ColorF32& c)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetHighLuminanceColorWeight(c);
  }
}
void Scene::GetBloomHighLuminanceColorWeight(gfl::grp::ColorF32* c) const
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->GetHighLuminanceColorWeight(c);
  }
}
void Scene::SetBloomHighLuminanceMinValue(const f32 v)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetHighLuminanceMinValue(v);
  }
}
f32  Scene::GetBloomHighLuminanceMinValue(void) const
{
  if( !m_g3d_post_effect_system ) return 0.0f;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return 0.0f;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetHighLuminanceMinValue();
  }
  return 0.0f;
}

void Scene::SetBloomBloomBoardExpanse(const f32 bloom_board_expanse)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetBloomBoardExpanse(bloom_board_expanse);
  }
}
f32  Scene::GetBloomBloomBoardExpanse(void) const
{
  if( !m_g3d_post_effect_system ) return 0.0f;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetBloomBoardExpanse();
  }
  return 0.0f;
}

void Scene::SetBloomBloomBoardIntensity(const f32 bloom_board_intensity)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetBloomBoardIntensity(bloom_board_intensity);
  }
}
f32  Scene::GetBloomBloomBoardIntensity(void) const
{
  if( !m_g3d_post_effect_system ) return 0.0f;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetBloomBoardIntensity();
  }
  return 0.0f;
}

void Scene::SetBloomBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetBloomBoardSmallImageCount(bloom_board_index, small_image_count);
  }
}
u32  Scene::GetBloomBloomBoardSmallImageCount(const u32 bloom_board_index) const
{
  if( !m_g3d_post_effect_system ) return 0;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetBloomBoardSmallImageCount(bloom_board_index);
  }
  return 0;
}


#if GFL_DEBUG
Model* Scene::GetBloomTextureCheckBoardModel(void) const
{
  if( !m_g3d_post_effect_system ) return NULL;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return NULL;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    return l_bloom_filter->GetTextureCheckBoardModel();
  }
  return NULL;
}
#if 0
//���܂��\������Ȃ��̂ŁA�g�p�֎~�B
void   Scene::SetImageFrameBufferToBloomTextureCheckBoard(const gfl::grp::DisplayType image_display_type)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    nngxSplitDrawCmdlist();  // ���̌�t���[���o�b�t�@���e�N�X�`���Ƃ��Ďg�p����̂ŕK�{
    
    l_bloom_filter->SetImageFrameBufferToTextureCheckBoard(image_display_type);
  }
}
#endif
void   Scene::SetImageOffScreenBufferToBloomTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetImageOffScreenBufferToTextureCheckBoard(image_off_screen_buffer);
  }
}
void   Scene::SetBloomHighLuminanceImageToBloomTextureCheckBoard(void)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetImageOffScreenBufferToTextureCheckBoard(l_bloom_filter->GetHighLuminanceBoardRenderTarget());
  }
}
void   Scene::SetBloomHighLuminanceImageToBloomTextureCheckBoard(const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate)
{
  if( !m_g3d_post_effect_system ) return;
  //if( !m_g3d_post_effect_system->IsBloomEnable() ) return;  // �ݒ�A�擾�͗L�������Ɋ֌W�Ȃ��A���Ԃ�����΍s���B
  
  BloomFilter* l_bloom_filter = m_g3d_post_effect_system->GetBloomFilter();

  if( l_bloom_filter )
  {
    l_bloom_filter->SetImageOffScreenBufferToTextureCheckBoard(l_bloom_filter->GetHighLuminanceBoardRenderTarget(), uv_scale, uv_translate);
  }
}
#endif  // #if GFL_DEBUG

//-----------------------------------------------------------------------------
/**
 * @brief �u���[���L�������ݒ�
 */
//-----------------------------------------------------------------------------
void Scene::SetBloomEnable( b32 f)
{
   if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return;
   m_g3d_post_effect_system->SetBloomEnable(f);
}

//-----------------------------------------------------------------------------
/**
 * @brief �u���[�����L�����H
 * @return �L�������t���O
 */
//-----------------------------------------------------------------------------
b32 Scene::IsBloomEnable()
{
  if ( m_g3d_post_effect_system->m_pG3DEdgeFilter == NULL )     return false;
  return m_g3d_post_effect_system->IsBloomEnable();
}



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �ȈՏ���
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
/*
 *  @brief         ���\�[�X����V�[�����\�z����
 *                 ���������I�u�W�F�N�g��Scene���Ǘ�����B
 *  
 *  @param[in]     heap_allocator    �q�[�v�A���P�[�^
 *  @param[in]     device_allocator  �f�o�C�X�������̃A���P�[�^
 *  @param[in]     Resource*         �V�[���\�z�Ɏg�p���郊�\�[�X
 *                                   nw::gfx::res::ResSceneNode�̐e�q�K�w�𔽉f����B
 *                                   ����Ɋ܂܂��ȉ��̃��\�[�X���琶�������I�u�W�F�N�g��
 *                                   �V�[���ɊǗ�����A�V�[���ɓo�^�����B
 *                                   nw::gfx::res::ResSceneNode(nw::gfx::res::ResModel, nw::gfx::res::Light)
 *                                   ����Ɋ܂܂��ȉ��̃��\�[�X���琶�������I�u�W�F�N�g��
 *                                   �V�[���ɊǗ������B
 *                                   nw::gfx::res::ResSceneEnvironmentSetting
 *  @param[in]     user_id           ���������I�u�W�F�N�g�ɑ΂��āA���[�U���C�ӂŕt������ID
 */
//-----------------------------------------------------------------------------
// �e���v���[�g����̊֐��I�u�W�F�N�g��e���v���[�g����̊֐������������A
// nw::ut::FixedSizeArray<TSceneNode*, TSize>* own_array
// �������œn�����Ƃ��Ă��܂������Ȃ������̂ŁA�}�N���ɂ����B

void  Scene::BuildSceneUsingResource(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Resource*               resource,
    s32                     user_id
)
{
#define GFL_G3D_SCENE_BUILD_HEADER(T_SCENE_NODE,T_SCENE_NODE_CLASS) \
    for( s32 i=0; i<resource->Get##T_SCENE_NODE_CLASS##sCount(); ++i ) \
    { \
      if( m_scene_own_object_pack->T_SCENE_NODE##_num < m_scene_own_object_pack->T_SCENE_NODE##_num_max ) \
      { \
        T_SCENE_NODE_CLASS* T_SCENE_NODE = GFL_NEW(heap_allocator->GetHeapBase()) T_SCENE_NODE_CLASS;


#if GFL_DEBUG

#define GFL_G3D_SCENE_BUILD_FOOTER(T_SCENE_NODE,T_SCENE_NODE_CLASS) \
        GFL_ASSERT_MSG( ret, "%s[%d]���V�[���ɒǉ��ł��܂���ł����B\n", #T_SCENE_NODE_CLASS, i ); /* //@check */ \
        m_scene_own_object_pack->T_SCENE_NODE##_array[m_scene_own_object_pack->T_SCENE_NODE##_num] = T_SCENE_NODE; \
        ++(m_scene_own_object_pack->T_SCENE_NODE##_num); \
        Object_SetOwnerAndOwnerId(T_SCENE_NODE, OBJECT_OWNER_SCENE, GetId()); \
        Object_SetUserIdForcedly(T_SCENE_NODE, user_id); \
      } \
      else \
      { \
        GFL_ASSERT_MSG(0, "%s[%d]���V�[���ŊǗ����邱�Ƃ��ł��Ȃ����߁A�������܂���ł����Bnum%d >= num_max%d\n", #T_SCENE_NODE_CLASS, i, \
            m_scene_own_object_pack->T_SCENE_NODE##_num, m_scene_own_object_pack->T_SCENE_NODE##_num_max ); \
      } \
    }

#else

#define GFL_G3D_SCENE_BUILD_FOOTER(T_SCENE_NODE,T_SCENE_NODE_CLASS) \
        m_scene_own_object_pack->T_SCENE_NODE##_array[m_scene_own_object_pack->T_SCENE_NODE##_num] = T_SCENE_NODE; \
        ++(m_scene_own_object_pack->T_SCENE_NODE##_num); \
        Object_SetOwnerAndOwnerId(T_SCENE_NODE, OBJECT_OWNER_SCENE, GetId()); \
        Object_SetUserIdForcedly(T_SCENE_NODE, user_id); \
      } \
    }

#endif


  // ���f��
  {
    gfl::grp::g3d::Model::Description model_description;
    model_description.animation_slot_num = 3;
    GFL_G3D_SCENE_BUILD_HEADER(model, Model)
        model->Create(heap_allocator, device_allocator, resource, i, &model_description);
        b32 ret = AddSceneNode(model);
    GFL_G3D_SCENE_BUILD_FOOTER(model, Model)
  }
    
  // �J����
  {
    gfl::grp::g3d::Camera::ResourceDescription camera_description;
    camera_description.animation_enable = true;
    GFL_G3D_SCENE_BUILD_HEADER(camera, Camera)
        camera->Create(heap_allocator, device_allocator, resource, i, &camera_description);
        b32 ret = AddSceneNode(camera);
    GFL_G3D_SCENE_BUILD_FOOTER(camera, Camera)
  }

  // ���C�g
  {
    GFL_G3D_SCENE_BUILD_HEADER(light, Light)
        light->Create(heap_allocator, device_allocator, resource, i);
        b32 ret = AddSceneNode(light);
    GFL_G3D_SCENE_BUILD_FOOTER(light, Light)
  }

  // �t�H�O
  {
    GFL_G3D_SCENE_BUILD_HEADER(fog, Fog)
        fog->Create(heap_allocator, device_allocator, resource, i);
        b32 ret = AddSceneNode(fog);
    GFL_G3D_SCENE_BUILD_FOOTER(fog, Fog)
  }

  // �V�[�����ݒ�
  {
    GFL_G3D_SCENE_BUILD_HEADER(scene_environment_setting, SceneEnvironmentSetting)
        scene_environment_setting->Create(heap_allocator, device_allocator, resource, i);
        b32 ret = true;  // GFL_ASSERT_MSG�ɂЂ�������Ȃ��悤�ɁB  //@check �����Ώۂ���O���悤�Ɋm�F�ς̈��t���Ă����B
    GFL_G3D_SCENE_BUILD_FOOTER(scene_environment_setting, SceneEnvironmentSetting)
  }

  GFL_GL_ASSERT();


  // @note scene_draw_group->SetState(SceneDrawGroup::STATE_AFTER_ADD_OR_REMOVE);���ĕK�v���H�ǂ����Ŏg���Ă��邩�H��AddSceneNode�����ꍇ�͂��̒��ŌĂ�ł���̂ŁA�����ŒP�ƂŌĂԕK�v�͂Ȃ��B

  // @note �e�q�K�w�̍\�z�͂���Ă��Ȃ����K�v���H���ꍇ�̓V�[�����Ǘ�����I�u�W�F�N�g�����łȂ��A���[�U�����������I�u�W�F�N�g�ɂ��Ă���肽���Ȃ��B�����[�U�����������I�u�W�F�N�g�̂Ƃ��ɂ���Ă��Ȃ��̂ŁA���킹�邽�߂ɂ��Ȃ��B
  // �t�@�C�����Ɋ܂܂�� SceneNode �p���N���X�����ׂēn�����ƂŁA
  // ResSceneNode �̐e�q�K�w�� SceneNode �ō\�z���܂��B
  // nw::gfx::SceneHelper::ResolveReference(scene_node_array);
  // ResolveReference�̓V�[�����[�g��ʃ��\�[�X�t�@�C���ɂ��Ă����Ă��A���ǂ��čs����̂ł͂Ȃ����Ǝv���BInitialize�ł����������Ƃ����邩���B


#undef GFL_G3D_SCENE_BUILD_FOOTER
#undef GFL_G3D_SCENE_BUILD_HEADER
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g���g���āA�V�[������ݒ肷��
 */
//-----------------------------------------------------------------------------
void  Scene::SetSceneEnvironmentUsingOwnObject(void)
{
  for( s32 i=0; i<m_scene_own_object_pack->scene_environment_setting_num; ++i )
  {
    SetSceneEnvironmentSetting(m_scene_own_object_pack->scene_environment_setting_array[i]);
  }
  GFL_GL_ASSERT();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g��j������
 */
//-----------------------------------------------------------------------------
void  Scene::DestroyOwnObject(void)
{
#define GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE(T_SCENE_NODE) \
  for( s32 i=0; i<m_scene_own_object_pack->T_SCENE_NODE##_num; ++i ) \
  { \
    RemoveSceneNode(m_scene_own_object_pack->T_SCENE_NODE##_array[i]); \
    GFL_DELETE m_scene_own_object_pack->T_SCENE_NODE##_array[i]; \
    m_scene_own_object_pack->T_SCENE_NODE##_array[i] = NULL; \
  } \
  m_scene_own_object_pack->T_SCENE_NODE##_num = 0;

  if( m_scene_own_object_pack )
  {
    for( s32 i=0; i<m_scene_own_object_pack->scene_environment_setting_num; ++i )
    {
      GFL_DELETE m_scene_own_object_pack->scene_environment_setting_array[i];
      m_scene_own_object_pack->scene_environment_setting_array[i] = NULL;
    }
    m_scene_own_object_pack->scene_environment_setting_num = 0;
 
    // �V�[���c���[����̍폜���s��
    GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE(fog)
    GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE(camera)
    GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE(light)
    GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE(model)
  }

#undef GFL_G3D_SCENE_DESTROY_OWN_SCENE_NODE
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[�����Ǘ�����I�u�W�F�N�g���擾����
 *                 �I�u�W�F�N�g���擾�͂��邪�A�V�[�����Ǘ���������̂ŁA������Ă͂Ȃ�Ȃ��B
 *  
 *  @param[in]     a_resource_id        ���\�[�X��ID
 *  @param[in]     a_index_in_resource  ���\�[�X���ɂ����邱�̃I�u�W�F�N�g�̃C���f�b�N�X
 *  @param[in]     a_user_id            ���[�U���C�ӂŕt����ID
 *
 *  @retval        NULL�̂Ƃ��Ȃ�
 */
//-----------------------------------------------------------------------------
#define GFL_G3D_SCENE_GET_OWN_SCENE_NODE(T_SCENE_NODE,T_SCENE_NODE_CLASS) \
  for( s32 i=0; i<m_scene_own_object_pack->T_SCENE_NODE##_num; ++i ) \
  { \
    T_SCENE_NODE_CLASS* T_SCENE_NODE = m_scene_own_object_pack->T_SCENE_NODE##_array[i]; \
    if(    T_SCENE_NODE->GetResourceId()      == a_resource_id \
        && T_SCENE_NODE->GetIndexInResource() == a_index_in_resource ) \
    { \
      if(    ( a_user_id          == OBJECT_USER_ID_ANYTHING ) \
          || ( T_SCENE_NODE->GetUserId() == a_user_id ) ) \
      { \
        return T_SCENE_NODE; \
      } \
    } \
  } \
  return NULL;

Model*  Scene::GetOwnModel( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id )
{
  GFL_G3D_SCENE_GET_OWN_SCENE_NODE(model, Model)
}
SceneEnvironmentSetting*  Scene::GetOwnSceneEnvironmentSetting( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id )
{
  GFL_G3D_SCENE_GET_OWN_SCENE_NODE(scene_environment_setting, SceneEnvironmentSetting)
}
Camera*  Scene::GetOwnCamera( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id )
{
  GFL_G3D_SCENE_GET_OWN_SCENE_NODE(camera, Camera)
}
Light*  Scene::GetOwnLight( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id )
{
  GFL_G3D_SCENE_GET_OWN_SCENE_NODE(light, Light)
}
Fog*  Scene::GetOwnFog( s32 a_resource_id, s32 a_index_in_resource, s32 a_user_id )
{
  GFL_G3D_SCENE_GET_OWN_SCENE_NODE(fog, Fog)
}

#undef GFL_G3D_SCENE_GET_OWN_SCENE_NODE



/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//
// ����J�֐�
//
///////////////////////////////////////////////////// 
///////////////////////////////////////////////////// 
///////////////////////////////////////////////////// 

//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// friend�N���X�ł���SceneCalculateGroup���炾���A�N�Z�X���Ă��悢�֐�
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

// ���݂̃V�[���c���[�t���O�ŃV�[���c���[�̏�Ԃ��X�V����
void Scene::RefreshNodeTreeFlag(SceneNode* scene_node)
{
  // SceneNode�̃����o�ϐ��ł���V�[���c���[�t���O���ς�����Ƃ��ɌĂяo�����֐�

  // scene_node��Scene�ɒǉ�����Ă���Ƃ��E�E�E
  //     �V�[���c���[�ɒǉ����� or �V�[���c���[�����菜��
  //     SceneNode::SetParent or SceneNode::UnsetParent�́AScene�ɑ΂���Add or Remove�����Ƃ��ɂ����Ă΂��̂ŁA
  //     SceneNode�̃����o�ϐ��ł���V�[���c���[�t���O���ς�����Ƃ���Parent�̐ݒ�͉��̉e�����󂯂Ȃ��B�܂�A
  //     Parent�����Ă�Scene�ɒǉ�����Ă��Ă��A�V�[���c���[�ɂ͒ǉ�����Ă��Ȃ����Ƃ����蓾��A�Ƃ������ƁB

  // scene_node��Scene�ɒǉ�����Ă��Ȃ��Ƃ��E�E�E
  //     �������Ȃ�

  // @note ������Ăяo��SceneCalculateGroup�͔������Ă���̂�
  //       SceneCalculateGroup�������ɓn���Ƃ��������B
  //       ��������΁A�������炾��scene_node��T���΂����̂ŁA
  //       ���������̕������ł���B

  // @note m_scene_calculate_group_index_borrow����v���Ă��邩�m�F�����ق������������B
  // @note Refresh�֐�1�ł͂Ȃ��A�t���OOn�֐��ƃt���OOff�֐���2��p�ӂ����ق������򂪌��点�邩���B

  s32        scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE;
  ObjectType object_type                 = OBJECT_TYPE_UNKNOWN;
  void* element = getNodeElement(scene_node, &scene_calculate_group_index, &object_type);

  if(element)
  {
    //////////////// scene_node��Scene�ɒǉ�����Ă���Ƃ� ////////////////
    
    SceneNodeProperty* scene_node_property = NULL;
    
    switch(object_type)
    {
    case OBJECT_TYPE_MODEL:
    case OBJECT_TYPE_PARTICLE:
      {
        scene_node_property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
        break;
      }
    case OBJECT_TYPE_LIGHT:
    case OBJECT_TYPE_CAMERA:
    case OBJECT_TYPE_STEREO_CAMERA:
    case OBJECT_TYPE_FOG:
      {
        scene_node_property = &((reinterpret_cast<SceneNodePropertyElement*>(element))->data);
        break;
      }
    }
  
    SceneCalculateGroup* calc_group = m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group;

    // SceneNode�̃V�[���c���[�t���O�ɕύX���������̂ŁA
    // Scene::isSceneNodePropertyInSceneTree �� Scene::isSceneNodePropertyInSceneTreeEasy ��
    // ���肵�Ă͂Ȃ�Ȃ��B
    // SceneNode::GetTreeFlag�Ŕ��肵�Ȃ���΂Ȃ�Ȃ��B
    if( scene_node->GetTreeFlag() )
    {
      //////// �V�[���c���[�ɐV���ɒǉ����ꂽ or ���X�ǉ�����Ă��� ////////

      if( scene_node->GetNwSceneNode()->GetParent() )
      {
        //// ���X�ǉ�����Ă��� ////

        // NW4C�̐e������̂ŁAscene_node�͊��ɉ����ɓo�^����Ă���̂ŁA�������Ȃ��B

        // @note SceneNode::SetParent��SceneNode::SetTreeFlag�̉e�����󂯂Ȃ��̂ŁA
        //       SceneNode::SetParent�ŉ��ɓo�^����Ă��邩���ׂĈ�v���Ă��邩�m�F�����ق������������B
        //       NW4C�̐e�����Ȃ��Ă��ASceneNode::SetParent�̐e��Scene�ɒǉ�����Ă���΂���B
      }
      else
      {
        //// �V���ɒǉ����ꂽ ////

        // NW4C�̐e�����Ȃ��̂ŁAscene_node�͂܂��o�^����Ă��Ȃ��B
       
        // @note SceneNode::SetParent��SceneNode::SetTreeFlag�̉e�����󂯂Ȃ��̂ŁA
        //       SceneNode::SetParent�ŉ��ɓo�^����Ă��邩���ׂĈ�v���Ă��邩�m�F�����ق������������B
        //       NW4C�̐e�����Ȃ��Ă��ASceneNode::SetParent�̐e��Scene�ɒǉ�����Ă���΂���B

        // scene_node���V�[���c���[�֓o�^����
        if( isSceneNodePropertyNotCullingFlagOn(*scene_node_property) )
        {
          // �J�����O����Ă��Ȃ��̂œo�^�ł���
          b32 ret_attach = calc_group->GetSceneRoot()->AttachChild(scene_node->GetNwSceneNode());
          if( ret_attach )
          {
            calc_group->SetSceneInitializeProcessNeedOnByTreeFlag(true);  // �ǉ�����true
          }
          else
          {
            GFL_ASSERT_MSG( 0, "�V�[���m�[�h���V�[�����[�g�̎q�ɂł��܂���ł����B\n" );
          }
        }
      }
    }
    else
    {
      //////// �V�[���c���[����V���Ɏ�菜���ꂽ or ���X�O��Ă��� ////////

      if( scene_node->GetNwSceneNode()->GetParent() )
      {
        //// �V���Ɏ�菜���ꂽ ////
        
        // NW4C�̐e������̂ŁAscene_node�͂܂��o�^����Ă���B
     
        // @note SceneNode::SetParent��SceneNode::SetTreeFlag�̉e�����󂯂Ȃ��̂ŁA
        //       SceneNode::SetParent�ŉ��ɓo�^����Ă��邩���ׂĈ�v���Ă��邩�m�F�����ق������������B
        //       NW4C�̐e�����Ȃ��Ă��ASceneNode::SetParent�̐e��Scene�ɒǉ�����Ă���΂���B

        // scene_node���V�[���c���[�����������
        checkErrorDetach( calc_group->GetSceneRoot(), scene_node->GetNwSceneNode(), "RefreshNodeTreeFlag" );
        calc_group->GetSceneRoot()->DetachChild(scene_node->GetNwSceneNode());
        calc_group->SetSceneInitializeProcessNeedOnByTreeFlag(false);  // ��������false
      }
      else
      {
        //// ���X�O��Ă��� ////

        // NW4C�̐e�����Ȃ��̂ŁAscene_node�͊��ɓo�^����Ă��Ȃ��̂ŁA�������Ȃ��B

        // @note SceneNode::SetParent��SceneNode::SetTreeFlag�̉e�����󂯂Ȃ��̂ŁA
        //       SceneNode::SetParent�ŉ��ɓo�^����Ă��邩���ׂĈ�v���Ă��邩�m�F�����ق������������B
        //       NW4C�̐e�����Ȃ��Ă��ASceneNode::SetParent�̐e��Scene�ɒǉ�����Ă���΂���B
      }
    }
  }

    //////////////// scene_node��Scene�ɒǉ�����Ă��Ȃ��Ƃ� ////////////////
    
    // Scene�ɂ܂�Add����Ă��Ȃ�scene_node�Ȃ̂ŁA
    // scene_node���ێ����Ă���V�[���c���[�t���O�����ύX����΂����̂�(���̊֐����Ăяo�����O�ɂ���͍s���Ă���͂�)�A
    // �����ł͉������Ȃ��B
}

#if GFL_GRP_G3D_H3D_USE
//void Scene::RefreshNodeTreeFlag(H3dModel* scene_node)
//{
//  // h3d��gfx�V�[���c���[�ɓ����Ă���킯�ł͂Ȃ�����A����͕s�v�ł��B
//}
#endif  // GFL_GRP_G3D_H3D_USE

void Scene::createSceneCalculateGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description)
{
  // ������
  gfl::heap::HeapBase* heap_memory   = heap_allocator->GetHeapBase();
  gfl::heap::HeapBase* device_memory = heap_allocator->GetHeapBase();

  // ModelNodePropertyFunctionWork
  {
    m_model_node_property_function_work = GFL_NEW(heap_memory) ModelNodePropertyFunctionWork;
    m_model_node_property_function_work->heap_memory          = heap_memory;
    m_model_node_property_function_work->scene_draw_group_num = description->scene_draw_group_num;
  } 

#if GFL_GRP_G3D_H3D_USE
  // H3dModelPropertyFunctionWork
  {
    m_h3d_model_property_function_work = GFL_NEW(heap_memory) H3dModelPropertyFunctionWork;
    m_h3d_model_property_function_work->heap_memory          = heap_memory;
    m_h3d_model_property_function_work->scene_draw_group_num = description->scene_draw_group_num;
  } 
#endif  // GFL_GRP_G3D_H3D_USE

  // �V�[���v�Z�O���[�v
  m_scene_calculate_group_num = description->scene_calculate_group_num;
  m_scene_calculate_group_pack_array = GFL_NEW_ARRAY(heap_memory) SceneCalculateGroupPack[m_scene_calculate_group_num];
  {
    for( s16 i=0; i<m_scene_calculate_group_num; ++i )
    {
      SceneCalculateGroup::Description l_calc_desc;
      if( description->scene_calculate_group_description_array )
      {
        l_calc_desc = description->scene_calculate_group_description_array[i];
      }

      m_scene_calculate_group_pack_array[i].scene_calculate_group = \
          GFL_NEW(heap_memory) gfl::grp::g3d::SceneCalculateGroup(device_allocator, &l_calc_desc);
      m_scene_calculate_group_pack_array[i].scene_calculate_group->SetScene(this, i);

      u32 model_num_max     = l_calc_desc.max_models + l_calc_desc.max_skeletal_models;
      u32 light_num_max     =   l_calc_desc.max_fragment_lights   \
                              + l_calc_desc.max_vertex_lights     \
                              + l_calc_desc.max_hemisphere_lights \
                              + l_calc_desc.max_ambient_lights;
      u32 camera_num_max    = l_calc_desc.max_cameras;
      u32 fog_num_max       = l_calc_desc.max_fogs;
      u32 particle_num_max  = l_calc_desc.max_particles;
#if GFL_GRP_G3D_H3D_USE
      u32 h3d_model_num_max = l_calc_desc.max_h3d_models;
#endif  // GFL_GRP_G3D_H3D_USE

      m_scene_calculate_group_pack_array[i].scene_node_property_pack.model_array     = createModelNodePropertyArray(heap_memory, model_num_max);
      m_scene_calculate_group_pack_array[i].scene_node_property_pack.light_array     = createSceneNodePropertyArray(heap_memory, light_num_max);
      m_scene_calculate_group_pack_array[i].scene_node_property_pack.camera_array    = createSceneNodePropertyArray(heap_memory, camera_num_max);
      m_scene_calculate_group_pack_array[i].scene_node_property_pack.fog_array       = createSceneNodePropertyArray(heap_memory, fog_num_max);
      m_scene_calculate_group_pack_array[i].scene_node_property_pack.particle_array  = createModelNodePropertyArray(heap_memory, particle_num_max);
#if GFL_GRP_G3D_H3D_USE
      m_scene_calculate_group_pack_array[i].scene_node_property_pack.h3d_model_array = createH3dModelPropertyArray(heap_memory, h3d_model_num_max);
#endif  // GFL_GRP_G3D_H3D_USE
      
      m_scene_calculate_group_pack_array[i].scene_calculate_group_enable_bit_flag = SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_DEFAULT;  // �f�t�H���g
    }
  }
}

Scene::SceneNodePropertyArray* Scene::createSceneNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num)
{
  return GFL_NEW(heap_memory) SceneNodePropertyArray(heap_memory, max_element_num,
      CreateSceneNodePropertyFunction,
      DestroySceneNodePropertyFunction,
      InitializeDefaultSceneNodePropertyFunction,
      CompareSceneNodePropertyFunction,
      CopySceneNodePropertyFunction,
      NULL );
}
Scene::ModelNodePropertyArray* Scene::createModelNodePropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num)
{
  return GFL_NEW(heap_memory) ModelNodePropertyArray(heap_memory, max_element_num,
      CreateModelNodePropertyFunction,
      DestroyModelNodePropertyFunction,
      InitializeDefaultModelNodePropertyFunction,
      CompareModelNodePropertyFunction,
      CopyModelNodePropertyFunction,
      m_model_node_property_function_work );
}

#if GFL_GRP_G3D_H3D_USE
Scene::H3dModelPropertyArray* Scene::createH3dModelPropertyArray(gfl::heap::HeapBase* heap_memory, s32 max_element_num)
{
  return GFL_NEW(heap_memory) H3dModelPropertyArray(heap_memory, max_element_num,
      CreateH3dModelPropertyFunction,
      DestroyH3dModelPropertyFunction,
      InitializeDefaultH3dModelPropertyFunction,
      CompareH3dModelPropertyFunction,
      CopyH3dModelPropertyFunction,
      m_h3d_model_property_function_work );
}
#endif

void Scene::createSceneDrawGroupPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description)
{
  // ������
  gfl::heap::HeapBase* heap_memory   = heap_allocator->GetHeapBase();
  gfl::heap::HeapBase* device_memory = heap_allocator->GetHeapBase();

  // �V�[���`��O���[�v
  m_scene_draw_group_num = description->scene_draw_group_num;
  m_scene_draw_group_pack_array = GFL_NEW_ARRAY(heap_memory) SceneDrawGroupPack[m_scene_draw_group_num];
  {
    s32 max_render_elements = 0;
    for( s16 i=0; i<m_scene_draw_group_num; ++i )
    {
      SceneDrawGroup::Description l_draw_desc;
      if( description->scene_draw_group_description_array )
      {
        l_draw_desc = description->scene_draw_group_description_array[i];
      }
      if( max_render_elements < l_draw_desc.max_render_elements ) max_render_elements = l_draw_desc.max_render_elements;

      m_scene_draw_group_pack_array[i].scene_draw_group = \
          GFL_NEW(heap_memory) gfl::grp::g3d::SceneDrawGroup(device_allocator, i, &l_draw_desc);
      m_render_system->InitializeSceneDrawGroup(m_scene_draw_group_pack_array[i].scene_draw_group);
   
      m_scene_draw_group_pack_array[i].scene_draw_group_camera = NULL;
      
      m_scene_draw_group_pack_array[i].scene_draw_group_enable_bit_flag = SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_DEFAULT;  // �f�t�H���g
    }
    
    m_render_queue_TEMP = nw::gfx::RenderQueue::Builder()
        .MaxRenderElements(max_render_elements)  // �V�[���`��O���[�v�S�Ă̒��ōő�̒l�ō쐬����
        .IsFixedSizeMemory(true)
        .Create(device_allocator);
  }
}

void Scene::createSceneOwnObjectPack(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description)
{
#define GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(T_SCENE_NODE,T_NUM_MAX,T_SCENE_NODE_CLASS) \
    m_scene_own_object_pack->T_SCENE_NODE##_num_max = T_NUM_MAX; \
    m_scene_own_object_pack->T_SCENE_NODE##_num = 0; \
    m_scene_own_object_pack->T_SCENE_NODE##_array = GFL_NEW_ARRAY(heap_memory) T_SCENE_NODE_CLASS*[m_scene_own_object_pack->T_SCENE_NODE##_num_max]; \
    gfl::std::MemClear( m_scene_own_object_pack->T_SCENE_NODE##_array, sizeof(T_SCENE_NODE_CLASS*) * m_scene_own_object_pack->T_SCENE_NODE##_num_max );

  // ������
  gfl::heap::HeapBase* heap_memory   = heap_allocator->GetHeapBase();
  gfl::heap::HeapBase* device_memory = heap_allocator->GetHeapBase();

  // Scene�����L���Ǘ��������
  m_scene_own_object_pack = GFL_NEW(heap_memory) ObjectPack;
  {
    GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(model, description->own_model_num_max, Model)
    GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(light, description->own_light_num_max, Light)
    GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(camera, description->own_camera_num_max, Camera)
    GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(fog, description->own_fog_num_max, Fog)
    GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER(scene_environment_setting, description->own_scene_environment_setting_num_max, SceneEnvironmentSetting)
  }

#undef GFL_G3D_SCENE_CREATE_SCENE_OWN_OBJECT_MEMBER
}

void Scene::destroySceneCalculateGroupPack(void)
{
  // �V�[���v�Z�O���[�v
  {
    for( s16 i=0; i<m_scene_calculate_group_num; ++i )
    {
#if GFL_GRP_G3D_H3D_USE
      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.h3d_model_array;
#endif  // GFL_GRP_G3D_H3D_USE

      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.particle_array;
      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.fog_array;
      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.camera_array;
      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.light_array;
      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_node_property_pack.model_array;

      GFL_DELETE m_scene_calculate_group_pack_array[i].scene_calculate_group;
    }
  }
  GFL_DELETE_ARRAY m_scene_calculate_group_pack_array;
  m_scene_calculate_group_pack_array = NULL;
  m_scene_calculate_group_num = 0;

  // ModelNodePropertyFunctionWork
  {
    GFL_DELETE m_model_node_property_function_work;
    m_model_node_property_function_work = NULL;
  }

#if GFL_GRP_G3D_H3D_USE
  // H3dModelPropertyFunctionWork
  {
    GFL_DELETE m_h3d_model_property_function_work;
    m_h3d_model_property_function_work = NULL;
  } 
#endif  // GFL_GRP_G3D_H3D_USE

}

void Scene::destroySceneDrawGroupPack(void)
{
  // �V�[���`��O���[�v
  {
    nw::gfx::SafeDestroy(m_render_queue_TEMP);  // nw::gfx::SafeDestroy��NULL�`�F�b�N�����Ă���Ă���
    m_render_queue_TEMP = NULL;
    
    for( s16 i=0; i<m_scene_draw_group_num; ++i )
    {
      GFL_DELETE m_scene_draw_group_pack_array[i].scene_draw_group;
    }
  }
  GFL_DELETE_ARRAY m_scene_draw_group_pack_array;
  m_scene_draw_group_pack_array = NULL;
  m_scene_draw_group_num = 0;
}

void Scene::destroySceneOwnObjectPack(void)
{
#define GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(T_SCENE_NODE) \
    if(m_scene_own_object_pack->T_SCENE_NODE##_array) \
    { \
      GFL_DELETE_ARRAY m_scene_own_object_pack->T_SCENE_NODE##_array; \
      m_scene_own_object_pack->T_SCENE_NODE##_array = NULL; \
    } 
   
  if( m_scene_own_object_pack )
  {
    // Scene�����L���Ǘ��������
    {
      GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(scene_environment_setting)
      GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(fog)
      GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(camera)
      GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(light)
      GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER(model)
    }
    GFL_DELETE m_scene_own_object_pack;
    m_scene_own_object_pack = NULL;
  }

#undef GFL_G3D_SCENE_DESTROY_SCENE_OWN_OBJECT_MEMBER
}

void Scene::constructorInitialize(void)
{
  m_scene_calculate_group_num = 0;
  m_scene_draw_group_num = 0;
  m_scene_calculate_group_pack_array = NULL;
  m_scene_draw_group_pack_array = NULL;
  m_render_queue_TEMP = NULL;
  m_scene_own_object_pack = NULL;
  m_scene_system = NULL;
  m_render_system = NULL;
  m_shadow_system = NULL;
  m_g3d_edge_map_system = NULL;
  m_g3d_post_effect_system = NULL;
  m_G3DDepthOfFieldSystem = NULL;
#if GFL_DEBUG
  m_debug_tick_enable_flag = false;
  m_debug_tick_dump_and_reset_flag = false;
#endif
}

#if GFL_DEBUG
void Scene::checkErrorDetach(nw::gfx::SceneNode* parent_node, nw::gfx::SceneNode* child_node, const char* msg)
{
  GFL_ASSERT_STOP_MSG(parent_node, "Detach Error: %s Parent\n", msg);
  GFL_ASSERT_STOP_MSG(child_node, "Detach Error: %s Child\n", msg);
  GFL_ASSERT_STOP_MSG(child_node->GetParent(), "Detach Error: %s Child's parent\n", msg);
  GFL_ASSERT_STOP_MSG(parent_node == child_node->GetParent(), "Detach Error: %s Parent != Child's parent\n", msg);
}
#else
void Scene::checkErrorDetach(nw::gfx::SceneNode*, nw::gfx::SceneNode*, const char*)
{
  // �������Ȃ�
}
#endif

void Scene::initializeInternal(const SceneInitializeProcess scene_initialize_process)
{ 
  for( s16 i=0; i<m_scene_calculate_group_num; ++i )
  {
    SceneCalculateGroup* calc_group = m_scene_calculate_group_pack_array[i].scene_calculate_group;
    initializeInternal(calc_group, scene_initialize_process);  // �����Ŏg�p���鏉��������
  }
}

void Scene::initializeInternal(SceneCalculateGroup* calc_group, const SceneInitializeProcess scene_initialize_process)
{
  // SceneInitializer �Ń}�e���A���\�[�g�̏��������肷�� MaterialId ��ݒ肵�܂��B
  // SceneTraverser �ŃV�[���m�[�h�̎��W�ƒ��񉻂����� SceneContext �ɒǉ����s���܂��B
  m_scene_system->InitializeScene( calc_group, scene_initialize_process );

  calc_group->SetState(SceneCalculateGroup::STATE_AFTER_INITIALIZE);
  
  //Initialize, Calculate, Initialize, Calculate, ... �Ƃ����ӂ���Draw��S���Ă΂��ɍs���l������̂ŁAInitialize��true�ɂ͂��Ȃ��Bm_scene_system->SetParticleEnableSwapBuffer(true);
}

void Scene::updateObjectBeforeCulling(SceneCalculateGroupPack* scene_calculate_group_pack)
{
#define GFL_G3D_SCENE_UPDATE_OBJECT(T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
    { \
      const T_NODE_ELEMENT* sentinel = get##T_SCENE_NODE_CLASS##SentinelElement(&(scene_calculate_group_pack->scene_node_property_pack)); \
      T_NODE_ELEMENT*       element  = get##T_SCENE_NODE_CLASS##ElementHead(&(scene_calculate_group_pack->scene_node_property_pack)); \
      while(element != sentinel) \
      { \
        if( element->data.scene_node->GetTreeFlag() ) \
        { \
          element->data.scene_node->Calculate(); \
        } \
        element = element->next; \
      } \
    }

  SceneNodePropertyPack* property_pack = &(scene_calculate_group_pack->scene_node_property_pack);
  bit32                  calc_bit_flag = scene_calculate_group_pack->scene_calculate_group_enable_bit_flag;
  
  if( IsBitFlagOnOne(calc_bit_flag, SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE) )
  {
    GFL_G3D_SCENE_UPDATE_OBJECT(Model, ModelNodePropertyElement)
    GFL_G3D_SCENE_UPDATE_OBJECT(Light, SceneNodePropertyElement)
    GFL_G3D_SCENE_UPDATE_OBJECT(Camera, SceneNodePropertyElement)
    GFL_G3D_SCENE_UPDATE_OBJECT(Fog, SceneNodePropertyElement)
    GFL_G3D_SCENE_UPDATE_OBJECT(Particle, ModelNodePropertyElement)
  }

#undef GFL_G3D_SCENE_UPDATE_OBJECT
}

void Scene::updateObjectAfterCulling(SceneCalculateGroupPack* scene_calculate_group_pack)
{
#if 0
//1/3
#define GFL_G3D_SCENE_UPDATE_OBJECT(T_SCENE_NODE) \
    for(u16 i=0; i<property_pack->T_SCENE_NODE##_num; ++i) \
    { \
      if( isSceneNodePropertyInSceneTreeEasy(property_pack->T_SCENE_NODE##_array[i]) )\
      { \
        property_pack->T_SCENE_NODE##_array[i].scene_node->�֐����܂��Ȃ�; \
      } \
    }
#endif

  SceneNodePropertyPack* property_pack = &(scene_calculate_group_pack->scene_node_property_pack);
  bit32                  calc_bit_flag = scene_calculate_group_pack->scene_calculate_group_enable_bit_flag;
 
  if( IsBitFlagOnOne(calc_bit_flag, SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING) )
  {
#if 0
//2/3
    GFL_G3D_SCENE_UPDATE_OBJECT(model)
    GFL_G3D_SCENE_UPDATE_OBJECT(light)
    GFL_G3D_SCENE_UPDATE_OBJECT(camera)
    GFL_G3D_SCENE_UPDATE_OBJECT(fog)
    GFL_G3D_SCENE_UPDATE_OBJECT(particle)
#endif
  }

#if 0
//3/3
#undef GFL_G3D_SCENE_UPDATE_OBJECT
#endif
}

void Scene::resetCalculateCullingFlag(s32 scene_calculate_group_index, ModelNodePropertyArray* property_array)
{
  const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
  ModelNodePropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    setCalculateCullingFlag( scene_calculate_group_index, &(element->data), true );
    element = element->next;
  }
}

void Scene::setCalculateCullingFlag(s32 scene_calculate_group_index, SceneNodeProperty* scene_node_property, b32 not_culling_flag)  // false�̂Ƃ��J�����O����ăV�[���c���[����O���
{
  b32 is_different = false;  // �t���O�ɕύX������ꍇtrue
  b32 is_change = false;
  b32 is_add    = false;

  // �t���O�ɕύX�����邩
  if( not_culling_flag )
  {
    if( isSceneNodePropertyNotCullingFlagOn(*scene_node_property) )
    {
      is_different = false;
    }
    else
    {
      is_different = true;
    }
  }
  else
  {
    if( isSceneNodePropertyNotCullingFlagOn(*scene_node_property) )
    {
      is_different = true;
    }
    else
    {
      is_different = false;
    }
  }

  if( is_different )  // if( isSceneNodePropertyNotCullingFlagOn(*scene_node_property) != not_culling_flag ) �̓_���B
  {                   // isSceneNodePropertyNotCullingFlagOn�̖߂�l��true, false�ł͂Ȃ��̂ŁB
    //// �t���O�ɕύX������ꍇ ////
    SceneCalculateGroup* calc_group = m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group;
    SceneNode*           scene_node = scene_node_property->scene_node;

    if( not_culling_flag )
    {
      //// �V�[���c���[�ɂ��Ȃ����V�[���c���[�ɂ��� ////
      if( scene_node->GetTreeFlag() )
      {
        // �c���[�t���O��1�Ȃ̂ŁA�V�[���c���[�ɓ���Ă��悢
        b32 ret_attach = calc_group->GetSceneRoot()->AttachChild(scene_node->GetNwSceneNode());
        if( ret_attach )
        {
          is_change = true;
          is_add    = true;
        }
        else
        {
          GFL_ASSERT_MSG( 0, "�V�[���m�[�h���V�[�����[�g�̎q�ɂł��܂���ł����B\n" );
        }
      }
    }
    else
    {
      //// �V�[���c���[�ɂ��遨�V�[���c���[�ɂ��Ȃ� ////
      if( scene_node->GetTreeFlag() )
      {
        // �c���[�t���O��1�Ȃ̂ŁA�V�[���c���[�ɓ����Ă���
        checkErrorDetach( calc_group->GetSceneRoot(), scene_node->GetNwSceneNode(), "setCalculateCullingFlag" ); \
        calc_group->GetSceneRoot()->DetachChild(scene_node->GetNwSceneNode());
        is_change = true;
        is_add    = false;
      }
    }

    if( is_change )
    {
      calc_group->SetSceneInitializeProcessNeedOnByNotCullingFlag(is_add);
    }

    setSceneNodePropertyNotCullingFlag(scene_node_property, not_culling_flag);
  }
}

#if GFL_GRP_G3D_H3D_USE
void Scene::resetH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelPropertyArray* property_array)
{
  const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
  H3dModelPropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    setH3dCalculateCullingFlag( scene_calculate_group_index, &(element->data), true );
    element = element->next;
  }
}
void Scene::setH3dCalculateCullingFlag(s32 scene_calculate_group_index, H3dModelProperty* h3d_model_property, b32 not_culling_flag)  // false�̂Ƃ��J�����O����ăV�[���c���[����O���
{
  // h3d��gfx�̃V�[���c���[�ɓ����Ă���킯�ł͂Ȃ��̂ŁA
  // �V�[���c���[�ɒǉ�������V�[���c���[����O������͂��Ȃ��Ă悢���A
  // gfx�̃V�[���c���[��Initialize�����Ȃ��Ă悢�B
  setSceneNodePropertyNotCullingFlag(h3d_model_property, not_culling_flag);
}
#endif  // GFL_GRP_G3D_H3D_USE

void Scene::calculateBeforeCulling(s32 scene_calculate_group_index)
{
  // ������ResetCalculateCullingFlag��Initialize���āA�V�[���c���[����O�������̂�߂�
  ResetCalculateCullingFlag(scene_calculate_group_index);

  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  bit32                    calc_bit_flag = calc_pack->scene_calculate_group_enable_bit_flag;

  if( calc_group->GetSceneInitializeProcessNeed() != SCENE_INITIALIZE_PROCESS_NONE )
  {
    initializeInternal(calc_group, calc_group->GetSceneInitializeProcessNeed());  // �����Ŏg�p���鏉��������
        // �J�����O�ɂ��A�ύX�����V�[���c���[�����ɖ߂����߂ɁAInitialize���Ă񂾂�����������Ȃ��̂ŁAGetSceneInitializeProcessNeed���m�F����B
#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
    GFL_PRINT("[Scene::CalculateBeforeCulling(%d)] �V�[���c���[�ɕύX���������̂ŁA���Initizalize���Ăт܂����B\n", scene_calculate_group_index);
    GFL_PRINT("�V�[���c���[��ύX������Initialize������Calculate���������ꍇ�́A�V�X�e���v���O���}�ɑ��k���ĉ������B\n");
    GFL_PRINT("�J�����O�ɂ��A�ύX�����V�[���c���[�����ɖ߂����߂ɁAInitizalize���Ă񂾂�����������܂���B\n");
#endif
#if (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2)
    GFL_ASSERT(0);
#endif
  }

  // �A�j���[�V�����A�m�[�h�A�X�P���g���A�J�����̍X�V�������s���܂��B
  updateObjectBeforeCulling(calc_pack);
  m_scene_system->UpdateSceneBeforeCulling(calc_group, calc_bit_flag);

#if GFL_GRP_G3D_H3D_USE
  calculateH3dModelBeforeCulling(scene_calculate_group_index, calc_bit_flag);
#endif
}

void Scene::calculateAfterCulling(s32 scene_calculate_group_index)
{
  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  bit32                    calc_bit_flag = calc_pack->scene_calculate_group_enable_bit_flag;

  if( calc_group->GetSceneInitializeProcessNeed() != SCENE_INITIALIZE_PROCESS_NONE )
  {
    initializeInternal(calc_group, SCENE_INITIALIZE_PROCESS_TRAVERSE);  // �����Ŏg�p���鏉��������
#if ((GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==1) || (GFL_G3D_SCENE_GROUP_ASSERT_LEVEL==2))
    GFL_PRINT("[Scene::CalculateAfterCulling(%d)] �J�����O�ɂ��A�V�[���c���[��ύX����̂ŁAInitizalize���Ăт܂����B\n", scene_calculate_group_index);
#endif
  }

  updateObjectAfterCulling(calc_pack);
  m_scene_system->UpdateSceneAfterCulling(calc_group, calc_bit_flag);
 
#if GFL_GRP_G3D_H3D_USE
  calculateH3dModelAfterCulling(scene_calculate_group_index, calc_bit_flag);
#endif

  // ������ResetCalculateCullingFlag��Initialize����ƁA���������V�[���c���[����O�������̂��V�[���c���[�ɖ߂��Ă��܂��A
  // �`�悪����Ă��܂��̂ŁA�����ł͂܂�not_culling_flag��߂��Ȃ���Initialize�����Ȃ��B

  calc_group->SetState(SceneCalculateGroup::STATE_AFTER_CALCULATE);
}

void Scene::culling(Camera* camera)
{
  for( u32 i=0; i<m_scene_calculate_group_num; ++i )
  {
    culling(i, camera);
  }
}

#if GFL_GRP_G3D_H3D_USE
void Scene::calculateH3dModel(s32 scene_calculate_group_index)
{
  SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
  H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;

  const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
  H3dModelPropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);

    if( property->scene_node->GetTreeFlag() )
    {
      property->scene_node->Calculate();
      property->scene_node->CalculateScene();
    }

    element = element->next;
  }
}

void Scene::calculateH3dModelBeforeCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag)
{
  SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
  H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;

  const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
  H3dModelPropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);

    if( property->scene_node->GetTreeFlag() )
    {
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE )
      {
          property->scene_node->Calculate();
          property->scene_node->CalculateBeforeCulling();
      }
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_TRANSFORM_NODE )
          property->scene_node->CalculateWorldMatrix();
    }

    element = element->next;
  }
}

void Scene::calculateH3dModelAfterCulling(s32 scene_calculate_group_index, bit32 scene_calculate_group_enable_bit_flag)
{
  SceneCalculateGroupPack* calc_pack      = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneNodePropertyPack*   property_pack  = &(calc_pack->scene_node_property_pack);
  H3dModelPropertyArray*   property_array = property_pack->h3d_model_array;

  const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
  H3dModelPropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    H3dModelProperty* property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);

    if( property->scene_node->GetTreeFlag() )
    {
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_AFTER_SCENE_CULLING )
          property->scene_node->CalculateAfterCulling();
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
          property->scene_node->CalculateLocalSkeletalAnimation();
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_BEFORE_WORLD_UPDATE )
          property->scene_node->CalculateVisibilityAnimation();
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
          property->scene_node->CalculateMaterialAnimation();
      if( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_ANIM )
          property->scene_node->CalculateAnimationFrame();
      if(
             ( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_G3D_OBJECT_BEFORE_WORLD_UPDATE )
          && ( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_UPDATE_SKELETAL_MODEL )
          && ( scene_calculate_group_enable_bit_flag & SCENE_CALCULATE_GROUP_ENABLE_BIT_FLAG_EVALUATE_ANIM_AFTER_SCENE_CULLING )
      )  // @note �����璷���H
          property->scene_node->CalculateWorldSkeletalAnimation();
    }

    element = element->next;
  }
}
#endif  // GFL_GRP_G3D_H3D_USE



//-----------------------------------------------------------------------------
// �J�����O
//-----------------------------------------------------------------------------
void Scene::culling(s32 scene_calculate_group_index, Camera* camera)
{
  SceneCalculateGroup*   calc_group    = m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_calculate_group;
  SceneNodePropertyPack* property_pack = &(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack);

  // ���f��
  {
    ModelNodePropertyArray* property_array = property_pack->model_array;

    const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
    ModelNodePropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      gfl::grp::g3d::Model* model = reinterpret_cast<gfl::grp::g3d::Model*>(element->data.scene_node);
      setCalculateCullingFlag( scene_calculate_group_index, &(element->data), ViewCullingModel( *model, *camera ) );  // �h���N���X�̃I�u�W�F�N�g�����N���X�̎Q�ƁA�|�C���^�ɓn�����Ƃ͉\
      element = element->next;
    }
  }

  // �p�[�e�B�N��
  // �p�[�e�B�N�����J�����O����Ȃ�1���q1���q�̈ʒu��c������K�v������H
  // ����A�S���q�̍L���蕝���擾���ăJ�����O����悤�ɂ������B
 
#if GFL_GRP_G3D_H3D_USE
  // H3dModel
  {
    H3dModelPropertyArray* property_array = property_pack->h3d_model_array;

    const H3dModelPropertyElement* sentinel = property_array->GetSentinelElement();
    H3dModelPropertyElement*       element  = property_array->GetElementHead();
    while(element != sentinel)
    {
      gfl::grp::g3d::H3dModel* model = reinterpret_cast<gfl::grp::g3d::H3dModel*>(element->data.scene_node);
      setH3dCalculateCullingFlag( scene_calculate_group_index, &(element->data), ViewCullingH3dModel( *model, *camera ) );  // �h���N���X�̃I�u�W�F�N�g�����N���X�̎Q�ƁA�|�C���^�ɓn�����Ƃ͉\
      element = element->next;
    }
  }
#endif  // GFL_GRP_G3D_H3D_USE
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// SubmitView
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

// ���f��
void Scene::submitViewAllModel(SceneNodePropertyPack* property_pack)
{
  const ModelNodePropertyElement* sentinel = getModelSentinelElement(property_pack);
  ModelNodePropertyElement*       element  = getModelElementHead(property_pack);
  while(element != sentinel)
  {
    ModelNodeProperty* model_property = &(element->data);
  
    if( isSceneNodePropertyInSceneTreeEasy(*model_property) && model_property->scene_node->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
    {
      nw::gfx::Model* nw_model = (reinterpret_cast<Model*>(model_property->scene_node))->GetNwModel();
      
      s16 draw_i_one = model_property->scene_draw_group_index_one;
      if( draw_i_one >= 0 )
      {
        SceneDrawGroupPack* draw_pack = &(m_scene_draw_group_pack_array[draw_i_one]);
        
        if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
        {
          m_render_system->SubmitViewModel(
            draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
            m_scene_system,
            nw_model,
            draw_pack->scene_draw_group );
        }
      }
      else if( draw_i_one == SCENE_DRAW_GROUP_INDEX_MULTI )
      {
        for( s16 draw_i=0; draw_i<m_scene_draw_group_num; ++draw_i )
        {
          SceneDrawGroupPack* draw_pack = &(m_scene_draw_group_pack_array[draw_i]);
   
          if( model_property->scene_draw_group_bit->IsOn(draw_i) )
          {
            if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
            {
              m_render_system->SubmitViewModel(
                draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
                m_scene_system,
                nw_model,
                draw_pack->scene_draw_group );
            }
          }
        }
      }
    }
      
    element = element->next;
  }
}
    
// �p�[�e�B�N��
void Scene::submitViewAllParticle(SceneNodePropertyPack* property_pack)
{
  const ModelNodePropertyElement* sentinel = getParticleSentinelElement(property_pack);
  ModelNodePropertyElement*       element  = getParticleElementHead(property_pack);
  while(element != sentinel)
  {
    ModelNodeProperty* particle_property = &(element->data);
 
    if( isSceneNodePropertyInSceneTreeEasy(*particle_property) && particle_property->scene_node->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
    {
      Particle* particle = reinterpret_cast<Particle*>(particle_property->scene_node);
      
      s16 draw_i_one = particle_property->scene_draw_group_index_one;
      if( draw_i_one >= 0 )
      {
        SceneDrawGroupPack* draw_pack = &(m_scene_draw_group_pack_array[draw_i_one]);
        
        if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
        {
          s32 particle_model_num = particle->GetModelNum();
          for( s32 j=0; j<particle_model_num; ++j )
          {
            nw::gfx::ParticleModel* nw_model = particle->GetModel(j);
            m_render_system->SubmitViewModel(
              draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
              m_scene_system,
              nw_model,
              draw_pack->scene_draw_group );
          }
        }
      }
      else if( draw_i_one == SCENE_DRAW_GROUP_INDEX_MULTI )
      {
        for( s16 draw_i=0; draw_i<m_scene_draw_group_num; ++draw_i )
        {
          SceneDrawGroupPack* draw_pack     = &(m_scene_draw_group_pack_array[draw_i]);

          if( particle_property->scene_draw_group_bit->IsOn(draw_i) )
          {
            if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
            {
              s32 particle_model_num = particle->GetModelNum();
              for( s32 j=0; j<particle_model_num; ++j )
              {
                nw::gfx::ParticleModel* nw_model = particle->GetModel(j);
                m_render_system->SubmitViewModel(
                  draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
                  m_scene_system,
                  nw_model,
                  draw_pack->scene_draw_group );
              }
            }
          }
        }
      }
    }
      
    element = element->next;
  }
}

#if GFL_GRP_G3D_H3D_USE
void Scene::submitViewAllH3dModel(SceneNodePropertyPack* property_pack)
{
  const H3dModelPropertyElement* sentinel = getH3dModelSentinelElement(property_pack);
  H3dModelPropertyElement*       element  = getH3dModelElementHead(property_pack);
  while(element != sentinel)
  {
    H3dModelProperty* h3d_model_property = &((reinterpret_cast<H3dModelPropertyElement*>(element))->data);
    H3dModel* h3d_model = h3d_model_property->scene_node;
     
    //���\�������悤�ɂȂ邩�͂킩��Ȃ��̂ŁA���ׂẴm�[�h�ɑ΂��Ă����Ń_�[�e�B�t���O�𗧂Ă�B
    //�_�[�e�B�t���O�́A�R�}���h������i�`���j�I�t�ɂ����B
    if ( h3d_model )
      h3d_model->SetFragmentAndWBufferDirty( m_render_system->IsFragmentAndWBufferDirty() );
  
    //KW256�C��
    if( isH3dModelPropertyInSceneTreeEasy(*h3d_model_property) && h3d_model && h3d_model->IsVisible() )  // �V�[���c���[�ɂ��āA�\������
    {
      s16 draw_i_one = h3d_model_property->scene_draw_group_index_one;
      if( draw_i_one >= 0 )
      {
        SceneDrawGroupPack* draw_pack = &(m_scene_draw_group_pack_array[draw_i_one]);
        
        if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
        {
          m_render_system->SubmitViewH3dModel(
            draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
            m_scene_system,
            h3d_model,
            draw_pack->scene_draw_group );
        }
      }
      else if( draw_i_one == SCENE_DRAW_GROUP_INDEX_MULTI )
      {
        for( s16 draw_i=0; draw_i<m_scene_draw_group_num; ++draw_i )
        {
          SceneDrawGroupPack* draw_pack = &(m_scene_draw_group_pack_array[draw_i]);
   
          if( h3d_model_property->scene_draw_group_bit->IsOn(draw_i) )
          {
            if( IsBitFlagOnOne(draw_pack->scene_draw_group_enable_bit_flag, SCENE_DRAW_GROUP_ENABLE_BIT_FLAG_ENQUEUE) )
            {
              m_render_system->SubmitViewH3dModel(
                draw_pack->scene_draw_group_camera->GetNwCamera(),  // ���̑O�ɃJ������NULL�łȂ����Ƃ��m�F���邩�ۂ�
                m_scene_system,
                h3d_model,
                draw_pack->scene_draw_group );
            }
          }
        }
      }
    }
      
    element = element->next;
  }
}
#endif  // GFL_GRP_G3D_H3D_USE

// �}�[�W
void Scene::mergeRenderQueueMethodHead(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B)
{
  // nw::ut::MoveArray�ɂ�push_front���Ȃ��̂ŁA�ꎞ�I�ȃ��X�g�����炴��𓾂Ȃ��B
  // nw::ut::MoveArray�ɂ�insert���Ȃ��B
  // nw::ut::MoveArray�ɂ͐擪�}���C�e���[�^front_insert_iterator���Ȃ�(�Ƃ������}���C�e���[�^���Ȃ�)�B
  // �R���e�i���Ăǂ��R�s�[����̂����ʂȂ񂾂낤��? std::copy�ŃR�s�[�����ق��������񂾂낤��?
  
  nw::gfx::RenderQueue::ElementListType* list_TEMP = (reinterpret_cast<internal::FakeRenderQueue*>(m_render_queue_TEMP))->GetList();
  list_TEMP->clear();

  // A��TEMP�ɔ�����
  nw::gfx::RenderElement* end_A = list_A->end();
  for( nw::gfx::RenderElement* i_A = list_A->begin();
       i_A != end_A;
       ++i_A )
  {
    list_TEMP->PushBackFast(*i_A);//push_back(*i_A);
  }

  // A���N���A����
  list_A->clear();

  // B��A�ɐς�
  nw::gfx::RenderElement* end_B = list_B->end();
  for( nw::gfx::RenderElement* i_B = list_B->begin();
       i_B != end_B;
       ++i_B )
  {
    list_A->PushBackFast(*i_B);//push_back(*i_B);
  }

  // TEMP��A�ɐς�
  nw::gfx::RenderElement* end_TEMP = list_TEMP->end();
  for( nw::gfx::RenderElement* i_TEMP = list_TEMP->begin();
       i_TEMP != end_TEMP;
       ++i_TEMP )
  {
    list_A->PushBackFast(*i_TEMP);//push_back(*i_TEMP);
  }
}
void Scene::mergeRenderQueueMethodTail(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B)
{
  // B��A�ɐς�
  nw::gfx::RenderElement* end_B = list_B->end();
  for( nw::gfx::RenderElement* i_B = list_B->begin();
       i_B != end_B;
       ++i_B )
  {
    list_A->PushBackFast(*i_B);//push_back(*i_B);
  }
}
void Scene::mergeRenderQueueMethodSortedMix(nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B)
{
  nw::gfx::RenderQueue::ElementListType* list_TEMP = (reinterpret_cast<internal::FakeRenderQueue*>(m_render_queue_TEMP))->GetList();
  list_TEMP->clear();

  // A��TEMP�ɔ�����
  nw::gfx::RenderElement* end_A = list_A->end();
  for( nw::gfx::RenderElement* i_A = list_A->begin();
       i_A != end_A;
       ++i_A )
  {
    list_TEMP->PushBackFast(*i_A);//push_back(*i_A);
  }

  // A���N���A����
  list_A->clear();

  nw::gfx::RenderElement* i_TEMP = list_TEMP->begin();
  nw::gfx::RenderElement* i_B = list_B->begin();
  u32 index_TEMP = 0;
  u32 index_B = 0;
  u32 size_TEMP = list_TEMP->size();  // size()�����ݎ��ۂɎg���Ă��镪
  u32 size_B = list_B->size();        // capacity()���Ƃ肠�����ꊇ�Ŋm�ۂ��Ă��镪
  b32 continue_TEMP = (index_TEMP<size_TEMP)?true:false;
  b32 continue_B = (index_B<size_B)?true:false;
  
  // TEMP��B�������Ƃ��c���Ă���
  while( index_TEMP<size_TEMP && index_B<size_B )
  {
    if( i_TEMP->Key() <= i_B->Key() )  // �����ꍇ��TEMP���ɕ`�悷��悤�ɂ���
    {
      list_A->PushBackFast(*i_TEMP);//push_back(*i_TEMP);
      ++i_TEMP;
      ++index_TEMP;
    }
    else
    {
      list_A->PushBackFast(*i_B);//push_back(*i_B);
      ++i_B;
      ++index_B;
    }
  }

  // �Е������c���Ă��Ȃ�
  // TEMP���c���Ă��� 
  while( index_TEMP<size_TEMP )
  {
    list_A->PushBackFast(*i_TEMP);//push_back(*i_TEMP);
    ++i_TEMP;
    ++index_TEMP;
  }
  // B���c���Ă��� 
  while( index_B<size_B )
  {
    list_A->PushBackFast(*i_B);//push_back(*i_B);
    ++i_B;
    ++index_B;
  }
}
void Scene::mergeRenderQueueMethodMixNeedSort(SceneDrawGroup* draw_group_A,
    nw::gfx::RenderQueue::ElementListType* list_A, nw::gfx::RenderQueue::ElementListType* list_B)
{
  // B��A�ɐς�
  nw::gfx::RenderElement* end_B = list_B->end();
  for( nw::gfx::RenderElement* i_B = list_B->begin();
       i_B != end_B;
       ++i_B )
  {
    list_A->PushBackFast(*i_B);//push_back(*i_B);
  }

  // �\�[�g
  m_render_system->EndSubmitView(draw_group_A);
}

void Scene::storeSceneNodeVisible(ModelNodePropertyArray* property_array, s32 scene_draw_group_index)
{
  const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
  ModelNodePropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    ModelNodeProperty* property = &(element->data);
    SceneNode* scene_node = property->scene_node;
    b32 visible = scene_node->IsVisible();
    b32 is_on   = property->scene_draw_group_bit->IsOn(scene_draw_group_index);
    scene_node->SetVisible( visible && is_on );
    setSceneNodePropertyTempVisible(property, visible);
    element = element->next;
  }
}
void Scene::restoreSceneNodeVisible(ModelNodePropertyArray* property_array)
{
  const ModelNodePropertyElement* sentinel = property_array->GetSentinelElement();
  ModelNodePropertyElement*       element  = property_array->GetElementHead();
  while(element != sentinel)
  {
    ModelNodeProperty* property = &(element->data);
    SceneNode* scene_node = property->scene_node;
    scene_node->SetVisible( isSceneNodePropertyTempVisibleOn(*property) );
    setSceneNodePropertyTempVisible(property, false);
    element = element->next;
  }
}




//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �V�[���m�[�h�� �ǉ� / �폜
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

/////////////////////////////////////////////
/////////////////////////////////////////////
// 
// �����ߊ֐�
// 
/////////////////////////////////////////////
/////////////////////////////////////////////

b32 Scene::moveNodeFromCalcToCalc(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index, s32 scene_calculate_group_index)
{
#define GFL_G3D_SCENE_MOVE_NODE(T_SCENE_NODE,T_SCENE_NODE_PROPERTY,T_NODE_ELEMENT) \
      T_NODE_ELEMENT*         old_node_element = reinterpret_cast<T_NODE_ELEMENT*>(old_element);  /* Move&Remove */ \
      T_SCENE_NODE_PROPERTY*  old_property     = &(old_node_element->data);  /* Move&Remove */ \
      T_NODE_ELEMENT*         node_element     = NULL;  /* Move&Add */ \
      T_SCENE_NODE_PROPERTY*  property         = NULL;  /* Move&Add */ \
      node_element = property_pack->T_SCENE_NODE##_array->AddGetSpaceElementTail();  /* Remove�O�Ɉ�UAdd���Ă��� */ \
      if( node_element ) \
      { \
        property = &(node_element->data); \
        copy##T_SCENE_NODE_PROPERTY(property, old_property); \
      } \
      removeNodeFromCalcAndAllDraw(old_element, object_type, old_scene_calculate_group_index);  /* Remove */ \
      if( node_element )  /* Add */ \
      { \
        b32 attach_ret = true; \
        if( property->scene_node->GetTreeFlag() ) \
        { \
          attach_ret = calc_group->GetSceneRoot()->AttachChild(property->scene_node->GetNwSceneNode()); \
          calc_group->SetState(SceneCalculateGroup::STATE_AFTER_ADD_OR_REMOVE); \
        } \
        if( attach_ret )  /* �c���[�t���O��0�̂Ƃ���NW4C�̃V�[���ɂ͓o�^����Ȃ���g3d�̃V�[���ɂ͓o�^�����B */ \
        { \
          property->scene_node->SetParent(calc_group);  /* Add�����B��UAdd���Ă������̂ɐ����Ȓl�𔽉f���Ă��� */ \
          ret = true; \
        } \
        else \
        { \
          property_pack->T_SCENE_NODE##_array->RemoveElement(node_element);  /* Add���s�B��UAdd���Ă������̂�Remove���Ă��� */ \
        } \
      }

  b32 ret = false;

  SceneCalculateGroupPack* old_calc_pack     = &(m_scene_calculate_group_pack_array[old_scene_calculate_group_index]);
  SceneCalculateGroup*     old_calc_group    = old_calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   old_property_pack = &(old_calc_pack->scene_node_property_pack);

  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);

  switch( object_type )
  {
  case OBJECT_TYPE_MODEL:
    {
      GFL_G3D_SCENE_MOVE_NODE(model, ModelNodeProperty, ModelNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_LIGHT:
    {
      GFL_G3D_SCENE_MOVE_NODE(light, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_CAMERA:
  case OBJECT_TYPE_STEREO_CAMERA:
    {
      GFL_G3D_SCENE_MOVE_NODE(camera, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_FOG:
    {
      GFL_G3D_SCENE_MOVE_NODE(fog, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_PARTICLE:
    {
      GFL_G3D_SCENE_MOVE_NODE(particle, ModelNodeProperty, ModelNodePropertyElement)
      break;
    }
  }

  return ret;

#undef GFL_G3D_SCENE_MOVE_NODE
}

b32 Scene::addNodeToCalc(SceneNode* scene_node, s32 scene_calculate_group_index, void** element)
{
#define GFL_G3D_SCENE_ADD_NODE(T_SCENE_NODE,T_NODE_ELEMENT) \
      T_NODE_ELEMENT* node_element = NULL; \
      node_element = property_pack->T_SCENE_NODE##_array->AddGetSpaceElementTail();  /* ��UAdd���Ă��� */ \
      if( node_element ) \
      { \
        b32 attach_ret = true; \
        if( scene_node->GetTreeFlag() ) \
        { \
          attach_ret = calc_group->GetSceneRoot()->AttachChild(scene_node->GetNwSceneNode()); \
          calc_group->SetState(SceneCalculateGroup::STATE_AFTER_ADD_OR_REMOVE); \
        } \
        if( attach_ret )  /* �c���[�t���O��0�̂Ƃ���NW4C�̃V�[���ɂ͓o�^����Ȃ���g3d�̃V�[���ɂ͓o�^�����B */ \
        { \
          scene_node->SetParent(calc_group);  /* Add�����B��UAdd���Ă������̂ɐ����Ȓl�𔽉f���Ă��� */ \
          node_element->data.scene_node = scene_node; \
          *element = node_element; \
          ret = true; \
        } \
        else \
        { \
          property_pack->T_SCENE_NODE##_array->RemoveElement(node_element);  /* Add���s�B��UAdd���Ă������̂�Remove���Ă��� */ \
        } \
      }

  b32 ret = false;

  SceneCalculateGroupPack* calc_pack     = &(m_scene_calculate_group_pack_array[scene_calculate_group_index]);
  SceneCalculateGroup*     calc_group    = calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   property_pack = &(calc_pack->scene_node_property_pack);
  
  ObjectType object_type = scene_node->GetObjectType();
  switch( object_type )
  {
  case OBJECT_TYPE_MODEL:
    {
      GFL_G3D_SCENE_ADD_NODE(model, ModelNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_LIGHT:
    {
      GFL_G3D_SCENE_ADD_NODE(light, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_CAMERA:
  case OBJECT_TYPE_STEREO_CAMERA:
    {
      GFL_G3D_SCENE_ADD_NODE(camera, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_FOG:
    {
      GFL_G3D_SCENE_ADD_NODE(fog, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_PARTICLE:
    {
      GFL_G3D_SCENE_ADD_NODE(particle, ModelNodePropertyElement)
      if( ret )
      {
        (reinterpret_cast<Particle*>(scene_node))->Setup( calc_group->GetParticleContext() );
      }
      break;
    }
  }

  return ret;

#undef GFL_G3D_SCENE_ADD_NODE
}

void Scene::removeNodeFromCalcAndAllDraw(void* old_element, ObjectType object_type, s32 old_scene_calculate_group_index)
{
#define GFL_G3D_SCENE_REMOVE_NODE(T_SCENE_NODE,T_SCENE_NODE_PROPERTY,T_NODE_ELEMENT) \
      T_NODE_ELEMENT*        old_node_element = reinterpret_cast<T_NODE_ELEMENT*>(old_element); \
      T_SCENE_NODE_PROPERTY* old_property     = &(old_node_element->data); \
      if( isSceneNodePropertyInSceneTreeEasy(*old_property) ) \
      { \
        checkErrorDetach( old_calc_group->GetSceneRoot(), old_property->scene_node->GetNwSceneNode(), "remove " #T_SCENE_NODE ); \
        old_calc_group->GetSceneRoot()->DetachChild(old_property->scene_node->GetNwSceneNode()); \
        { /* @note �J������؂�ւ��O�Ɏg���Ă����J������j�������Ƃ��Ɏ~�܂����B�ً}�Ώ��B��ŕK�������Ώ����邱�ƁI�����̑Ώ��ő��v�B */ \
          nw::gfx::Camera* camera = nw::ut::DynamicCast<nw::gfx::Camera*>(old_property->scene_node->GetNwSceneNode()); \
          if( camera ) \
          { \
            s32 camera_index = m_render_system->GetCameraIndex(camera); \
            if( camera_index != CAMERA_INDEX_NONE ) \
            { \
              m_render_system->SetCamera( camera_index, NULL ); \
            } \
          } \
        } \
        old_calc_group->SetState(SceneCalculateGroup::STATE_AFTER_ADD_OR_REMOVE); \
      } \
      old_property->scene_node->UnsetParent(); \
      old_property_pack->T_SCENE_NODE##_array->RemoveElement(old_node_element);

  SceneCalculateGroupPack* old_calc_pack     = &(m_scene_calculate_group_pack_array[old_scene_calculate_group_index]);
  SceneCalculateGroup*     old_calc_group    = old_calc_pack->scene_calculate_group;
  SceneNodePropertyPack*   old_property_pack = &(old_calc_pack->scene_node_property_pack);

  switch( object_type )
  {
  case OBJECT_TYPE_MODEL:
    {
      GFL_G3D_SCENE_REMOVE_NODE(model, ModelNodeProperty, ModelNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_LIGHT:
    {
      GFL_G3D_SCENE_REMOVE_NODE(light, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_CAMERA:
  case OBJECT_TYPE_STEREO_CAMERA:
    {
      GFL_G3D_SCENE_REMOVE_NODE(camera, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_FOG:
    {
      GFL_G3D_SCENE_REMOVE_NODE(fog, SceneNodeProperty, SceneNodePropertyElement)
      break;
    }
  case OBJECT_TYPE_PARTICLE:
    {
      GFL_G3D_SCENE_REMOVE_NODE(particle, ModelNodeProperty, ModelNodePropertyElement)
      break;
    }
  }

#undef GFL_G3D_SCENE_REMOVE_NODE
}




/////////////////////////////////////////////
/////////////////////////////////////////////
// 
// �]���ʂ�̊֐�
// 
/////////////////////////////////////////////
/////////////////////////////////////////////

// @note �y�L�q�������z�����̊���̋L�q�́A�����L�q���܂Ƃ߂邱�Ƃ��ł��邵�A���̊֐����Ăяo���đΉ����ł���͂��B
//       �]���ʂ�̊֐��Ŏg�p�𐄏����Ă�����̂ł͂Ȃ��̂ŁA�Ȃ�ׂ����̊֐��Ăяo���ɂ��Ă������ƁB
//       �L�q���O�Ƃ��ẮA�]���ʂ�̊֐��̌��J�ł͂��̂܂܂Œ��g�͂����ɂ���]���ʂ�̊֐��̔���J�ł��ĂԂ����A
//       �����ɂ���]���ʂ�̊֐��̔���J�ł́A�����֐��̌��J�ł�����J�ł��g��(�ǂ��炩�ɓ���ł���Ƃ����̂����A����Ȃ瓝�ꂵ�Ȃ��Ă悢)

#define GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX() \
  s32                old_scene_calculate_group_index = SCENE_CALCULATE_GROUP_INDEX_NONE; \
  ObjectType         object_type                     = OBJECT_TYPE_UNKNOWN; \
  void* old_element = getNodeElement(scene_node, &old_scene_calculate_group_index, &object_type);

b32 Scene::addSceneNodeToSceneCalculateGroup(SceneNode* scene_node, s32 scene_calculate_group_index)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    ret = moveNodeFromCalcToCalc(old_element, object_type, old_scene_calculate_group_index, scene_calculate_group_index);
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    void* element = NULL;
    ret = addNodeToCalc(scene_node, scene_calculate_group_index, &element);
    if( ret )
    {
      // �V�[���`��O���[�v0�Ԃɂ��o�^����
      switch( scene_node->GetObjectType() )  // old_element��NULL�Ȃ̂�object_type�ɂ͂ǂ�Ȓl�������Ă��邩������Ȃ��̂ŁA�����ł�����Ǝ�蒼���Ă���
      {
      case OBJECT_TYPE_MODEL:
      case OBJECT_TYPE_PARTICLE:
        {
          ModelNodeProperty* model_node_property = &((reinterpret_cast<ModelNodePropertyElement*>(element))->data);
          if( model_node_property->scene_draw_group_index_one == SCENE_DRAW_GROUP_INDEX_NONE )
          {
            model_node_property->scene_draw_group_index_one = 0;
            model_node_property->scene_draw_group_bit->On(0);
          }
          break;
        }
      }
    }
  }
  return ret;
}
b32 Scene::removeSceneNodeFromSceneCalculateGroup(SceneNode* scene_node)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    removeNodeFromCalcAndAllDraw(old_element, object_type, old_scene_calculate_group_index);
    ret = true;
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    ret = false;
  }
 
  return ret;
}
b32 Scene::removeAllSceneNodeFromSceneCalculateGroup(void)
{
  RemoveAllNodeFromAllCalcAndAllDraw();
  return true;
}

b32 Scene::addModelNodeToSceneCalculateGroupAndSceneDrawGroup(SceneNode* scene_node, s32 scene_calculate_group_index, s32 scene_draw_group_index)
{
  return AddNodeToCalcAndDraw(scene_node, scene_calculate_group_index, scene_draw_group_index);
}

b32 Scene::addModelNodeToAllSceneDrawGroup(SceneNode* scene_node)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    ret = true;
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    ret = AddNodeToCalc(scene_node, 0); 
  }
  ret = AddNodeToAllDraw(scene_node);

  return ret;
}
b32 Scene::addModelNodeToSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    ret = true;
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    ret = AddNodeToCalc(scene_node, 0); 
  }
  ret = AddNodeToDraw(scene_node, scene_draw_group_index);

  return ret;
}
b32 Scene::removeModelNodeFromAllSceneDrawGroup(SceneNode* scene_node)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    ret = true;
    RemoveNodeFromAllDraw(scene_node);
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    ret = false; 
  }

  return ret; 
}
b32 Scene::removeModelNodeFromSceneDrawGroup(SceneNode* scene_node, s32 scene_draw_group_index)
{
  b32 ret = false;

  GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX()

  if( old_element )
  {
    // ���ɂǂꂩ���̃V�[���v�Z�O���[�v�ɓo�^����Ă���ꍇ
    ret = true;
    RemoveNodeFromDraw(scene_node, scene_draw_group_index);
  }
  else
  {
    // �܂��ǂ̃V�[���v�Z�O���[�v�ɂ��o�^����Ă��Ȃ��ꍇ
    ret = false; 
  }

  return ret; 
}

#undef GFL_G3D_SCENE_GET_OLD_SCENE_NODE_INDEX


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// �擾
//
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

void* Scene::getNodeElement(const SceneNode* scene_node, s32* scene_calculate_group_index, ObjectType* object_type)
{
#define GFL_G3D_SCENE_GET_NODE_ELEMENT(T_SCENE_NODE_CLASS,T_SCENE_NODE_TYPE) \
  case OBJECT_TYPE_##T_SCENE_NODE_TYPE: \
      return get##T_SCENE_NODE_CLASS##Element(reinterpret_cast<const T_SCENE_NODE_CLASS*>(scene_node), scene_calculate_group_index);

  *object_type = scene_node->GetObjectType();
  switch( *object_type )
  {
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Model, MODEL)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Light, LIGHT)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Camera, CAMERA)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Camera, STEREO_CAMERA)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Fog, FOG)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Particle, PARTICLE)
#if GFL_GRP_G3D_H3D_USE
  //GFL_G3D_SCENE_GET_NODE_ELEMENT(H3dModel, H3D_MODEL)  // @note H3dModel��SceneNode�̔h���N���X�ł͂Ȃ��̂ŋN���蓾�Ȃ��A���̊֐��g���Ȃ��ƕs�ւ��ȁB
#endif  // GFL_GRP_G3D_H3D_USE
  }
  return NULL;

#undef GFL_G3D_SCENE_GET_NODE_ELEMENT
}


#define GFL_G3D_SCENE_GET_NODE_ELEMENT(T_SCENE_NODE,T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
  { \
    T_NODE_ELEMENT* element = NULL; \
    for( s16 i=0; i<m_scene_calculate_group_num; ++i ) \
    { \
      element = m_scene_calculate_group_pack_array[i].scene_node_property_pack.T_SCENE_NODE##_array->GetElement(const_cast<T_SCENE_NODE_CLASS*>(scene_node));  /* const�����Ă���Ƃ��܂������Ȃ������B@note ���́H typedef SceneNode* SceneNodePointer;��const�ł��Ȃ����炾�낤���H�����������Ă���킯�ł͂Ȃ��̂Ŏg�����Ⴄ�B */ \
      if(element) \
      { \
        *scene_calculate_group_index = i; \
        break; \
      } \
    } \
    return element; \
  } 

Scene::ModelNodePropertyElement* Scene::getModelElement(const Model* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(model, Model, ModelNodePropertyElement)
}
Scene::SceneNodePropertyElement* Scene::getLightElement(const Light* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(light, Light, SceneNodePropertyElement)
}
Scene::SceneNodePropertyElement* Scene::getCameraElement(const Camera* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(camera, Camera, SceneNodePropertyElement)
}
Scene::SceneNodePropertyElement* Scene::getFogElement(const Fog* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(fog, Fog, SceneNodePropertyElement)
}
Scene::ModelNodePropertyElement* Scene::getParticleElement(const Particle* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(particle, Particle, ModelNodePropertyElement)
}
#if GFL_GRP_G3D_H3D_USE
Scene::H3dModelPropertyElement* Scene::getH3dModelElement(const H3dModel* scene_node, s32* scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(h3d_model, H3dModel, H3dModelPropertyElement)
}
#endif  // GFL_GRP_G3D_H3D_USE

#undef GFL_G3D_SCENE_GET_NODE_ELEMENT


void* Scene::getNodeElement(const SceneNode* scene_node, const SceneNodePropertyPack* scene_node_property_pack, ObjectType* object_type)
{
#define GFL_G3D_SCENE_GET_NODE_ELEMENT(T_SCENE_NODE_CLASS,T_SCENE_NODE_TYPE) \
  case OBJECT_TYPE_##T_SCENE_NODE_TYPE: \
      return get##T_SCENE_NODE_CLASS##Element(reinterpret_cast<const T_SCENE_NODE_CLASS*>(scene_node), scene_node_property_pack);  /* const�����Ă���Ƃ��܂������Ȃ������B@note ���́H typedef SceneNode* SceneNodePointer;��const�ł��Ȃ����炾�낤���H�����������Ă���킯�ł͂Ȃ��̂Ŏg�����Ⴄ�B */

  *object_type = scene_node->GetObjectType();
  switch( *object_type )
  {
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Model, MODEL)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Light, LIGHT)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Camera, CAMERA)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Camera, STEREO_CAMERA)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Fog, FOG)
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Particle, PARTICLE)
#if GFL_GRP_G3D_H3D_USE
  //GFL_G3D_SCENE_GET_NODE_ELEMENT(H3dModel, H3D_MODEL)  // @note H3dModel��SceneNode�̔h���N���X�ł͂Ȃ��̂ŋN���蓾�Ȃ��A���̊֐��g���Ȃ��ƕs�ւ��ȁB
#endif  // GFL_GRP_G3D_H3D_USE
  }
  return NULL;

#undef GFL_G3D_SCENE_GET_NODE_ELEMENT
}

#define GFL_G3D_SCENE_GET_NODE_ELEMENT(T_SCENE_NODE,T_SCENE_NODE_CLASS) \
  return scene_node_property_pack->T_SCENE_NODE##_array->GetElement(const_cast<T_SCENE_NODE_CLASS*>(scene_node));

Scene::ModelNodePropertyElement* Scene::getModelElement(const Model* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(model, Model)
}
Scene::SceneNodePropertyElement* Scene::getLightElement(const Light* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(light, Light)
}
Scene::SceneNodePropertyElement* Scene::getCameraElement(const Camera* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(camera, Camera)
}
Scene::SceneNodePropertyElement* Scene::getFogElement(const Fog* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(fog, Fog)
}
Scene::ModelNodePropertyElement* Scene::getParticleElement(const Particle* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(particle, Particle)
}
#if GFL_GRP_G3D_H3D_USE
Scene::H3dModelPropertyElement* Scene::getH3dModelElement(const H3dModel* scene_node, const SceneNodePropertyPack* scene_node_property_pack)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(h3d_model, H3dModel)
}
#endif  // GFL_GRP_G3D_H3D_USE

#undef GFL_G3D_SCENE_GET_NODE_ELEMENT


void* Scene::getNodeElement(const SceneNode* scene_node, const s32 scene_calculate_group_index, ObjectType* object_type)
{
  return getNodeElement(scene_node, &(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack), object_type);
}

#define GFL_G3D_SCENE_GET_NODE_ELEMENT(T_SCENE_NODE_CLASS) \
  return get##T_SCENE_NODE_CLASS##Element(scene_node, &(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack));

Scene::ModelNodePropertyElement* Scene::getModelElement(const Model* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Model)
}
Scene::SceneNodePropertyElement* Scene::getLightElement(const Light* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Light)
}
Scene::SceneNodePropertyElement* Scene::getCameraElement(const Camera* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Camera)
}
Scene::SceneNodePropertyElement* Scene::getFogElement(const Fog* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Fog)
}
Scene::ModelNodePropertyElement* Scene::getParticleElement(const Particle* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(Particle)
}
#if GFL_GRP_G3D_H3D_USE
Scene::H3dModelPropertyElement* Scene::getH3dModelElement(const H3dModel* scene_node, const s32 scene_calculate_group_index)
{
  GFL_G3D_SCENE_GET_NODE_ELEMENT(H3dModel)
}
#endif  // GFL_GRP_G3D_H3D_USE
#undef GFL_G3D_SCENE_GET_NODE_ELEMENT


#define GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(T_SCENE_NODE,T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
Scene::T_NODE_ELEMENT* Scene::get##T_SCENE_NODE_CLASS##ElementHead(const SceneNodePropertyPack* scene_node_property_pack) const\
{ \
  return scene_node_property_pack->T_SCENE_NODE##_array->GetElementHead(); \
}
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(model, Model, ModelNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(light, Light, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(camera, Camera, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(fog, Fog, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(particle, Particle, ModelNodePropertyElement)
#if GFL_GRP_G3D_H3D_USE
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK(h3d_model, H3dModel, H3dModelPropertyElement)
#endif  // GFL_GRP_G3D_H3D_USE
#undef GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_PROPERTY_PACK


#define GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
Scene::T_NODE_ELEMENT* Scene::get##T_SCENE_NODE_CLASS##ElementHead(const s32 scene_calculate_group_index) const\
{ \
  return get##T_SCENE_NODE_CLASS##ElementHead(&(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack)); \
}
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(Model, ModelNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(Light, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(Camera, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(Fog, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(Particle, ModelNodePropertyElement)
#if GFL_GRP_G3D_H3D_USE
GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX(H3dModel, H3dModelPropertyElement)
#endif  // GFL_GRP_G3D_H3D_USE
#undef GFL_G3D_SCENE_GET_NODE_ELEMENT_HEAD_CALC_INDEX


#define GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(T_SCENE_NODE,T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
const Scene::T_NODE_ELEMENT* Scene::get##T_SCENE_NODE_CLASS##SentinelElement(const SceneNodePropertyPack* scene_node_property_pack) const\
{ \
  return scene_node_property_pack->T_SCENE_NODE##_array->GetSentinelElement(); \
}
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(model, Model, ModelNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(light, Light, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(camera, Camera, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(fog, Fog, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(particle, Particle, ModelNodePropertyElement)
#if GFL_GRP_G3D_H3D_USE
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK(h3d_model, H3dModel, H3dModelPropertyElement)
#endif  // GFL_GRP_G3D_H3D_USE
#undef GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_PROPERTY_PACK


#define GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(T_SCENE_NODE_CLASS,T_NODE_ELEMENT) \
const Scene::T_NODE_ELEMENT* Scene::get##T_SCENE_NODE_CLASS##SentinelElement(const s32 scene_calculate_group_index) const\
{ \
  return get##T_SCENE_NODE_CLASS##SentinelElement(&(m_scene_calculate_group_pack_array[scene_calculate_group_index].scene_node_property_pack)); \
}
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(Model, ModelNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(Light, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(Camera, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(Fog, SceneNodePropertyElement)
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(Particle, ModelNodePropertyElement)
#if GFL_GRP_G3D_H3D_USE
GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX(H3dModel, H3dModelPropertyElement)
#endif  // GFL_GRP_G3D_H3D_USE
#undef GFL_G3D_SCENE_GET_NODE_SENTINEL_ELEMENT_CALC_INDEX



//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//
// �J�����ݒ�
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void Scene::assignNotSetCameraIndex( Camera* camera )
{
  if( camera )
  {
    m_render_system->SetNotSetCamera(camera->GetNwCamera());
  }
  else
  {
    m_render_system->SetNotSetCamera(NULL);
  }
}
s32  Scene::getNotSetCameraIndex(void)
{
  return m_render_system->GetNotSetCameraIndex();
}

s32  Scene::getFirstAssignedCameraIndex(void)
{
  return m_render_system->GetFirstCameraIndex();
}

// SetupNotSetCamera��CleanupNotSetCamera�͕K���΂Ŏg�p������������n���ĉ������B
void Scene::setupNotSetCamera(Camera* camera, s32 scene_draw_group_index, b32* is_assign_camera_index, b32* is_set_active_camera_index)
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g����
  *is_assign_camera_index     = false;
  *is_set_active_camera_index = false;

  s32 assigned_camera_index = CAMERA_INDEX_NONE;
  if( camera )
  {
    assigned_camera_index = this->GetAssignedCameraIndex(camera);
    if( assigned_camera_index == CAMERA_INDEX_NONE )
    {
      this->assignNotSetCameraIndex(camera);
      assigned_camera_index = this->getNotSetCameraIndex();
      *is_assign_camera_index = true;
    }
  }
  if( this->GetActiveCameraIndex(scene_draw_group_index) == CAMERA_INDEX_NONE )
  {
    if( assigned_camera_index == CAMERA_INDEX_NONE )
    {
      assigned_camera_index = this->getFirstAssignedCameraIndex();  // @note �d���Ȃ���ԍŏ��̃J�������g���Ă���̂ŁAASSERT���K�v��������Ȃ��B
                                                                    //       �f�t�H���g�ł��������ݒ肾�A�Ƃ����l���̂ق����������肭�邩�ȁB
    }
    if( assigned_camera_index >= 0 )
    {
      this->SetActiveCameraIndex( assigned_camera_index, scene_draw_group_index );
      *is_set_active_camera_index = true;
    }
    else
    {
      GFL_ASSERT_MSG(0, "�J����������V�[�����ɐݒ肳��Ă��܂���B\n");
    }
  }
}
void Scene::cleanupNotSetCamera(Camera* /*camera*/, s32 scene_draw_group_index, const b32 is_assign_camera_index, const b32 is_set_active_camera_index)
{
  // �V�X�e���Ŗ��ݒ���T�|�[�g�����Ƃ��̌�n��
  if( is_assign_camera_index )
  {
    this->assignNotSetCameraIndex(NULL);
  }
  if( is_set_active_camera_index )
  {
    this->SetActiveCameraIndex( CAMERA_INDEX_NONE, scene_draw_group_index );
  }
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// 
// �f�o�b�O�@�\
// 
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
void Scene::debugTickDumpAndReset(const char* message)  // message�͍Ō�̉��s�Ȃ���OK
{
#if GFL_DEBUG
  if( m_debug_tick_enable_flag && m_debug_tick_dump_and_reset_flag )
  {
    GFL_PRINT("-start---------------------------------\n");
    GFL_PRINT("%s\n", message);
    GFL_DEBUG_TICK_DUMP_ALL( GFL_G3DSCENE );
    GFL_DEBUG_TICK_RESET( GFL_G3DSCENE );
    GFL_PRINT("-end-----------------------------------\n");
  }
#endif
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

