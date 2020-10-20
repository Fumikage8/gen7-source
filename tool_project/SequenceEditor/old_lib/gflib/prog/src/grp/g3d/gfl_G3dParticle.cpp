//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   gfl_G3dParticle.cpp
 *  @brief  3D�p�[�e�B�N��
 *  @author Koji Kawada
 *  @date   2011.05.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dParticle.h>


#if GFL_DEBUG
    #define GFL_GRP_G3D_PARTICLE_RECURSIVE_COUNT_MAX (16)  // �ċA�Ăяo���̉񐔏��
#endif


namespace gfl {
namespace grp {
namespace g3d {

#if GFL_DEBUG
  b32 Particle::s_SafeDeleteCheckEnable        = false;
  b32 Particle::s_SafeDeleteCheckEnableUserSet = false;
  b32 Particle::s_SafeDeleteCheckForce         = false;
  b32 Particle::s_SafeDeleteCheckAssert        = false;
#endif  

//-----------------------------------------------------------------------------
/**
 *     �N���X�錾
 */
//-----------------------------------------------------------------------------
//-------------------------------------
/// 3D�p�[�e�B�N���N���X
//=====================================

void Particle::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->max_children = 16;
  description->model_max_children = 16;
  description->emitter_max_children = 16;
}
/*
  �֐��Őݒ肷��̂͂�����߂�J�����̂悤�ȃ^�C�v�̂�����̂����֐��ݒ�ɂ��鑼�͒��ڑ����
 void Particle::InitializeDescription(
    Description*  description,
    s32 max_children,
    s32 model_max_children,
    s32 emitter_max_children
)
{
  description->max_children = max_children;
  description->model_max_children = model_max_children;
  description->emitter_max_children = emitter_max_children;
}*/

Particle::Particle(void)
  : SceneNode(),
    //m_tree_flag(true),
    m_is_setup(false),
    m_scene_node_array(NULL),
    m_model_array(NULL),
    m_emitter_array(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetParticleManager()->AddObject(this);
}
Particle::~Particle()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetParticleManager()->RemoveObject(this);
}

void Particle::Create(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  Resource*               resource,
  const Description*      description
)
{
  SceneNode::Create();
  
  // description
  Description l_description;
  if( description )
  {
    l_description = *description;
  }
  else
  {
    gfl::grp::g3d::Particle::InitializeDescriptionForDefault(&l_description);
  }

  // ���\�[�X
  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();

  // ���f���̌�
  s32 model_num = nw_res_graphics_file->GetModelsCount();
  // �G�~�b�^�[�̌�
  s32 emitter_num = nw_res_graphics_file->GetEmittersCount();
  // ���f���̌��ƃG�~�b�^�[�̌��̍��v
  s32 total_num = model_num+emitter_num;

  // ���������V�[���m�[�h���Ǘ����Ă����z��
  //m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::SceneNode*>(l_description.max_children, heap_allocator, nw::ut::ARRAY_WRAPPER);  // l_description.max_children�Ɠ���������ȏ�̌����K�v�Ȃ̂ŁAl_description.max_children���g���̂͂�߂�B
  //m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::SceneNode*>(total_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��
  m_scene_node_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::SceneNodeArray(total_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��
 
  // ���������p�[�e�B�N�����f�����Ǘ����Ă����z��
  //m_model_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::ParticleModel*>(model_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��
  m_model_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::ParticleModelArray(model_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��
  // ���������p�[�e�B�N���G�~�b�^�[���Ǘ����Ă����z��
  //m_emitter_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::ut::MoveArray<nw::gfx::ParticleEmitter*>(emitter_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��
  m_emitter_array = GFL_NEW(heap_allocator->GetHeapBase()) nw::gfx::ParticleEmitterArray(emitter_num, heap_allocator, nw::ut::ARRAY_WRAPPER);  // �M���M���̌������p��

  // ���f������
  for( s32 i=0; i<model_num; ++i )
  {
    nw::gfx::ResModel res = nw_res_graphics_file->GetModels(i);
    nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(l_description.model_max_children)
        .MaxCallbacks(0)
        .BufferOption(nw::gfx::Model::FLAG_BUFFER_NOT_USE)
        .SharedMaterialModel(NULL)
        .SharedMeshNodeVisibiltiyModel(NULL)
        .MaxAnimObjectsPerGroup(0)  // @note �p�[�e�B�N���̃}�e���A���A�j���[�V���������Ȃ炱����ݒ�ł���悤�ɂ��Ȃ���΂Ȃ�Ȃ��B
                                    // @note �A�j���[�V����������Ƃ�1�ł������H����Ƃ�AnimGroup���ꂼ��ɕ������悤�Ȃ��Ƃ͂���̂��H
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled(false)  // @note �p�[�e�B�N���̃}�e���A���A�j���[�V���������Ȃ炱����ݒ�ł���悤�ɂ��Ȃ���΂Ȃ�Ȃ��B
        .CreateObject(heap_allocator, device_allocator);
    
    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
    b32 push_back = m_scene_node_array->PushBack(scene_node);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  ���f�����m�ۂ��Ă������V�[���m�[�h�̗v�f��%d���z���܂����B\n", total_num );  //@check �o�Ȃ�����
    
    nw::gfx::ParticleModel* model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(scene_object);
    push_back = m_model_array->PushBack(model);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  ���f�����m�ۂ��Ă��������f���̗v�f��%d���z���܂����B\n", model_num );  //@check �o�Ȃ�����
  }

  // �G�~�b�^�[����
  for( s32 i=0; i<emitter_num; ++i )
  {
    nw::gfx::ResParticleEmitter res = nw_res_graphics_file->GetEmitters(i);
    nw::gfx::SceneObject* scene_object = nw::gfx::SceneBuilder()
        .Resource(res)
        .IsFixedSizeMemory(true)
        .MaxChildren(l_description.emitter_max_children)
        .MaxCallbacks(0)
        .BufferOption(nw::gfx::Model::FLAG_BUFFER_NOT_USE)
        .SharedMaterialModel(NULL)
        .SharedMeshNodeVisibiltiyModel(NULL)
        .MaxAnimObjectsPerGroup(0)  // @note �p�[�e�B�N���̃}�e���A���A�j���[�V����������ꍇ�ł�������0�ł����Ǝv�����ǂ����낤�B
        .ParticleSetMarginCount(0)
        .IsAnimationEnabled(false)  // @note �p�[�e�B�N���̃}�e���A���A�j���[�V����������ꍇ�ł�������false�ł����Ǝv�����ǂ����낤�B
        .CreateObject(heap_allocator, device_allocator);

    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
    b32 push_back = m_scene_node_array->PushBack(scene_node);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  �G�~�b�^�[���m�ۂ��Ă������V�[���m�[�h�̗v�f��%d���z���܂����B\n", total_num );  //@check �o�Ȃ�����

    nw::gfx::ParticleEmitter* emitter = nw::ut::DynamicCast<nw::gfx::ParticleEmitter*>(scene_object);
    push_back = m_emitter_array->PushBack(emitter);
    GFL_ASSERT_MSG( push_back, "gfl::grp::g3d::Particle  �G�~�b�^�[���m�ۂ��Ă������G�~�b�^�[�̗v�f��%d���z���܂����B\n", emitter_num );  //@check �o�Ȃ�����
  }

  // �K�w�č\�z
  nw::gfx::SceneHelper::ResolveReference(*m_scene_node_array);

  // �g�b�v�m�[�h����
  nw::gfx::TransformNode* transform_node = nw::gfx::TransformNode::DynamicBuilder()
    .IsFixedSizeMemory(true)
    .MaxChildren(l_description.max_children)  // @note m_scene_node_array�̂����Ŏq�ɂȂ�m�[�h�̌��𐔂��Ă����΁A���傤�ǃs�b�^�����蓖�Ă邱�Ƃ��ł���B
    .MaxCallbacks(0)
    .Create(device_allocator);

  // �g�b�v�m�[�h�ɃA�^�b�`
  {
    NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note ���f���ƃG�~�b�^�[��ʁX�Ɋo���Ă����悤�ɂ������B���f���A�G�~�b�^�[�ǂ�������炩����DynamicCast���Ă��������B
    {
      if(node->GetParent() == NULL)
      {
        b32 attach_child = transform_node->AttachChild(node);
        GFL_ASSERT_MSG( attach_child, "�p�[�e�B�N���̃g�b�v�m�[�h�Ɏq��ǉ��ł��܂���ł����B�m�ۂ��Ă������q�̌�%d\n", l_description.max_children );  //@check �o�Ȃ�����
      }
    }
  }

  // �e�N���X�ł���SceneNode, Object�̐ݒ�
  {
    nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(transform_node);
    SetNwSceneNode(scene_node);
    SetResourceIdAndIndexInResource(resource->GetId(), OBJECT_RESOURCE_ID_PARTICLE);
  }
  
#if GFL_DEBUG      
  if ( model_num )
  {
    nw::gfx::ResModel res = nw_res_graphics_file->GetModels(0);
    this->SetModelName( res.GetName() );//��ڂ̃��f���������ł��f�o�b�O�p�r�Ɏg���B����肩�̓}�V���낤�B
    
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      model->SetUserParameter( reinterpret_cast<u32>(this) );
    }
    
  }
#endif
  

  //m_tree_flag = true;
}

#if GFL_DEBUG    
void Particle::SetModelName( const char* pName )
{
  memset( m_ModelName, 0, sizeof(m_ModelName) );
  s32   len = strlen( pName );
  
  if ( len >= sizeof(m_ModelName) )
    len = sizeof(m_ModelName) - 1;//���[null�����L�[�v
  
  memcpy( m_ModelName, pName, len );
}
#endif

void Particle::Destroy(void)
{
 #if GFL_DEBUG
  if ( s_SafeDeleteCheckEnable )
  {
    u32   cnt = gfl::grp::GraphicsSystem::GetDrawFrameCount();

    if ( ( m_DrawFrameCount + 2 ) > cnt )
    {
      if( s_SafeDeleteCheckAssert )
      {
        GFL_ASSERT_MSG( 0, "�p�[�e�B�N����%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
      else
      {
        GFL_PRINT( "�p�[�e�B�N����%s�͕`�撆�ɉ������܂����B : �Ō�ɃR�}���h���ς܂ꂽ�t���[�� = %d : �j���t���[�� = %d\n\n", m_ModelName, m_DrawFrameCount, cnt );
      }
    }
  }
#endif
  
  // �Ǝ����� ��������
  if( m_emitter_array )
  {
    GFL_DELETE m_emitter_array;  // @note m_emitter_array�ɓo�^����Ă���e�V�[���m�[�h�̉���܂ōs���Ă��Ȃ����m�F���āI��������Ă��Ȃ�
    m_emitter_array = NULL;
  }
  if( m_model_array )
  {
    GFL_DELETE m_model_array;  // @note m_model_array�ɓo�^����Ă���e�V�[���m�[�h�̉���܂ōs���Ă��Ȃ����m�F���āI��������Ă��Ȃ�
    m_model_array = NULL;
  }
  if( m_scene_node_array )
  {
    GFL_DELETE m_scene_node_array;  // @note m_scene_node_array�ɓo�^����Ă���e�V�[���m�[�h�̉���܂ōs���Ă��Ȃ����m�F���āI��������Ă��Ȃ�
    m_scene_node_array = NULL;
  }
  //m_tree_flag = true;
  // �Ǝ����� �����܂�
 
  // �g�b�v�m�[�h�j��
  //SceneNode::Destroy();  // �q������̂ŁA�P�i�폜�ł̓_���B
  nw::gfx::SceneNode* scene_node = GetNwSceneNode();
  nw::gfx::SafeDestroyBranch( scene_node );  // �q���ꏏ�ɍ폜�B
  // ���̎��_�ł́A���[�J���ϐ�scene_node��NULL���������Ă��邾���ŁA���N���X��m_nw_scene_node�ɂ�NULL���������Ă��Ȃ��B
  SetNwSceneNode(NULL);  // ���N���X��m_nw_scene_node��NULL�������Ă����B

  m_is_setup = false;
  
  SceneNode::Destroy();  // ����m_nw_scene_node��NULL�ɂȂ��Ă���̂ŁA�����Őe��Destory���Ă��m_nw_scene_node�j���ȊO�̏�����������B
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���s�ړ�
 */
//-----------------------------------------------------------------------------
void Particle::SetTranslate(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
/*
    // ����O��m�F���Ă݂�
    GFL_PRINT(
        "[ConstraintNwCalculatedTransform]\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n",
        constraint_calculated_transform.TransformMatrix().f._00, constraint_calculated_transform.TransformMatrix().f._01,
            constraint_calculated_transform.TransformMatrix().f._02, constraint_calculated_transform.TransformMatrix().f._03,
        constraint_calculated_transform.TransformMatrix().f._10, constraint_calculated_transform.TransformMatrix().f._11,
            constraint_calculated_transform.TransformMatrix().f._12, constraint_calculated_transform.TransformMatrix().f._13,
        constraint_calculated_transform.TransformMatrix().f._20, constraint_calculated_transform.TransformMatrix().f._21,
            constraint_calculated_transform.TransformMatrix().f._22, constraint_calculated_transform.TransformMatrix().f._23
    );
*/
    constraint_calculated_transform.SetTranslate( x, y, z );
/*
    // ����O��m�F���Ă݂�
    GFL_PRINT(
        "[ConstraintNwCalculatedTransform]\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n"
        "(%f, %f, %f, %f)\n",
        constraint_calculated_transform.TransformMatrix().f._00, constraint_calculated_transform.TransformMatrix().f._01,
            constraint_calculated_transform.TransformMatrix().f._02, constraint_calculated_transform.TransformMatrix().f._03,
        constraint_calculated_transform.TransformMatrix().f._10, constraint_calculated_transform.TransformMatrix().f._11,
            constraint_calculated_transform.TransformMatrix().f._12, constraint_calculated_transform.TransformMatrix().f._13,
        constraint_calculated_transform.TransformMatrix().f._20, constraint_calculated_transform.TransformMatrix().f._21,
            constraint_calculated_transform.TransformMatrix().f._22, constraint_calculated_transform.TransformMatrix().f._23
    );
*/
  }
}
void Particle::SetTranslate(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( t );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetTranslate( t );
  }
}
void Particle::GetTranslate(gfl::math::VEC3* t) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.GetTranslate( t );
    return;
  }

/*
�ȉ���unreachable code�Ȃ̂ŃR�����g�A�E�g
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().GetTranslate( t );
*/
}
gfl::math::VEC3 Particle::GetTranslate(void) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    return constraint_calculated_transform.GetTranslate();
  }

/*
�ȉ���unreachable code�Ȃ̂ŃR�����g�A�E�g
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  return transform_node->Transform().GetTranslate();
*/
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��]
 *                 �P�ʂ̓��W�A���B
 */
//-----------------------------------------------------------------------------
void Particle::SetRotateXYZ(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetRotateXYZ( x, y, z );
  }
}
void Particle::SetRotateXYZ(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateXYZ( t.x, t.y, t.z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetRotateXYZ( t.x, t.y, t.z );
  }
}
void Particle::SetRotateMatrix(
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
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetRotateMatrix(
    m00,
    m01,
    m02,
    m10,
    m11,
    m12,
    m20,
    m21,
    m22
  );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetRotateMatrix(
      m00,
      m01,
      m02,
      m10,
      m11,
      m12,
      m20,
      m21,
      m22
    );
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �X�P�[��
 */
//-----------------------------------------------------------------------------
void Particle::SetScale(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( x, y, z );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetScale( x, y, z );
  } 
}
void Particle::SetScale(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetScale( t );

  {
    nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    constraint_calculated_transform.SetScale( t );
  } 
}
void Particle::GetScale(gfl::math::VEC3* t) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    *t = constraint_calculated_transform.Scale();
    return;
  } 

/*
�ȉ���unreachable code�Ȃ̂ŃR�����g�A�E�g
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  *t = transform_node->Transform().Scale();
*/
}
gfl::math::VEC3 Particle::GetScale(void) const
{
  {
    const nw::gfx::CalculatedTransform& constraint_calculated_transform = ConstraintNwCalculatedTransform();  // @note �h���N���X�ŃA�N�Z�X���āA�X�̔h���N���X�ŏ�������̂ł͂Ȃ��ASceneNode�ŏ����������B
    gfl::math::VEC3 s = constraint_calculated_transform.Scale();
    return s;
  } 

/*
�ȉ���unreachable code�Ȃ̂ŃR�����g�A�E�g
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  gfl::math::VEC3 t = transform_node->Transform().Scale();
  return t;
*/
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�揇��ݒ肷��
 *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID���p�[�e�B�N�����f���ɐݒ肷��
 *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
 *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
 *
 *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 *
 *  @param[in]    model_layer_id    ���f���̃��C���[ID
 */
//-----------------------------------------------------------------------------
void Particle::SetModelLayerId(u8 model_layer_id)
{
  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
  GFL_ASSERT(model_layer_id <= 127);  //@check �`�揇����������

  nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
  for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
       i != end;
       ++i )
  {
    nw::gfx::ParticleModel* model = (*i);
    model->SetLayerId((model_layer_id << 1)); 
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �`�揇���擾����
 *                 �v���O���}�������ݒ�ł��郂�f���̃��C���[ID���p�[�e�B�N������擾����
 *                 �ŏ��ɕ`�� 0 <= u8 <= 127 �Ō�ɕ`��
 *                 �ŗD�悳���`�揇 ModelLayerId > MaterialTranslucencyKind > ���̑� �ŗ�コ���`�揇
 *
 *                 **** Model��Particle�ɂ���S�������ɕ��ׂ�����ԍ��ł��B�f�t�H���g0000000�ł��B ****
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂̒��Ő擪�̂��̂̒l���擾���܂�(�擪�Ɛ擪�ȊO�ňقȂ�l���ݒ肳��Ă���Ƃ����������ƂɂȂ邩������܂���) ##
 *
 *  @retval       u8    ���f���̃��C���[ID
 */
//-----------------------------------------------------------------------------
u8 Particle::GetModelLayerId(void) const
{
  // u8�����炵�Ďg���B�Ō��1�r�b�g�ɂ�Model0�AParticle1�̃f�t�H���g�l������Ă���B
  GFL_ASSERT_MSG(GetModelNum()>0, "ParticleModelNum=0\n");  //@check ���f�����Ȃ��Ȃ�0��Ԃ�����

  u8 model_layer_id = 0;
  nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
  for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
       i != end;
       ++i )
  {
    nw::gfx::ParticleModel* model = (*i);
    model_layer_id = model->GetLayerId();
    break;  // �擪�̂��̂̒l���擾���ďI��
  }
  return (model_layer_id >> 1);
}

#if 0
//������
//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z�t���O
 *                 true�̂Ƃ��v�Z�����(@note �S���H�ꕔ�H)
 *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͌v�Z����Ȃ�
 */
//-----------------------------------------------------------------------------
void Particle::SetCalculateFlag(b32 flag)
{
  flag;
}
b32  Particle::GetCalculateFlag(void) const
{
  return true;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         �`��t���O
 *                 true�̂Ƃ��`�悳���
 *                 ���������ꂪtrue�̂Ƃ��ł��AGetTreeFlag��false�̂Ƃ��͕`�悳��Ȃ�
 */
//-----------------------------------------------------------------------------
void Particle::SetVisible(b32 visible)
{
  NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note ���f���ƃG�~�b�^�[��ʁX�Ɋo���Ă����悤�ɂ������B���f���A�G�~�b�^�[�ǂ�������炩����DynamicCast���Ă��������B
  {
    nw::gfx::ParticleModel* particle_model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(node);
    if( particle_model )
    {
      particle_model->SetVisible(visible);
    }
  }
}
b32  Particle::IsVisible(void) const
{
  b32 ret = false;
  NW_FOREACH(nw::gfx::SceneNode* node, *m_scene_node_array)  // @note ���f���ƃG�~�b�^�[��ʁX�Ɋo���Ă����悤�ɂ������B���f���A�G�~�b�^�[�ǂ�������炩����DynamicCast���Ă��������B
  {
    nw::gfx::ParticleModel* particle_model = nw::ut::DynamicCast<nw::gfx::ParticleModel*>(node);
    if( particle_model )
    {
      ret = particle_model->IsVisible();  // �S�Ẵp�[�e�B�N�����f���ɓ����ݒ�����Ă���̂ŁA1�����m�F����Ώ\���̂͂��B
      break;
    }
  }
  return ret;
}

#if 0
//-----------------------------------------------------------------------------
/*
 *  @brief         �V�[���c���[�t���O
 *                 true�̂Ƃ�Scene��Add����Ă�����V�[���c���[�ɓ������Afalse�̂Ƃ�Scene��Add����Ă��Ă��V�[���c���[����O�����
 *                 ���ꂪtrue�ł��邱�ƂƁA���[�U��Scene��Add�������Ƃ́A�����ł͂���܂���B
 */
//-----------------------------------------------------------------------------
void Particle::SetTreeFlag(b32 flag)
{
  m_tree_flag = flag;
}
b32  Particle::GetTreeFlag(void) const
{
  return m_tree_flag;
}
#endif

#if 0
//������
//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����邩�ǂ���
 */
//-----------------------------------------------------------------------------
Particle::CalculateState Particle::GetCalculateState(void) const
{
  return CALCULATE_STATE_NO;
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief         �`�悳��邩�ǂ���
 */
//-----------------------------------------------------------------------------
Particle::DrawState Particle::GetDrawState(void) const
{
  if( IsVisible() && GetTreeFlag() ) return DRAW_STATE_OK;
  return DRAW_STATE_NO;
}

//-----------------------------------------------------------------------------
/*
 *  @brief  �X�V�t���[����ݒ肷��
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 * 
 *  0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{��
 *  �p�[�e�B�N���͋t�Đ��͂ł��Ȃ��̂ŁA-1.0f�Ȃǂ̕��̒l�͐ݒ�ł��܂���B
 */
//-----------------------------------------------------------------------------
void Particle::SetStepFrame(const f32 step_frame)
{
  f32 l_step_frame = step_frame;
  if( l_step_frame < 0.0f )
  {
/*
�p�[�e�B�N����step_frame�Ƀ}�C�i�X��n���ƁA
���΂炭�ł܂��ē����Ȃ��Ȃ�����ŁA
gfx_ParticleSet.cpp
SetDefaultValues
        NN_WARNING(limitDefault > -16777216, "limitDefault is overflow");
�Ƃ���warning���o�āA�����悤�ɂȂ�B
*/
    GFL_ASSERT_MSG( 0, "�p�[�e�B�N���̃X�e�b�v�t���[���ɕ��̒l%f�͐ݒ�ł��܂���B\n", step_frame );
    l_step_frame = 0.0f;
  }

#if 0
  //�ċA�͎~�߂��B
  nw::gfx::SceneNode* scene_node = GetNwSceneNode();
  SetStepFrameRecursively(scene_node, l_step_frame
#if GFL_DEBUG
      , 0
#endif 
  );
#endif

  {
    //nw::ut::MoveArray<nw::gfx::ParticleModel*>::iterator end = m_model_array->end();
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    //for( nw::ut::MoveArray<nw::gfx::ParticleModel*>::iterator i = m_model_array->begin();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetStepFrame(l_step_frame);  // i->�_���Ai.�_���A(*i).�_��
    }
  }
  {
    //nw::ut::MoveArray<nw::gfx::ParticleEmitter*>::iterator end = m_emitter_array->end();
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    //for( nw::ut::MoveArray<nw::gfx::ParticleEmitter*>::iterator i = m_emitter_array->begin();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetStepFrame(l_step_frame);
    }
  }
}
#if 0
  //�ċA�͎~�߂��B
void Particle::SetStepFrameRecursively(
    nw::gfx::SceneNode*   nw_scene_node,
    const f32             step_frame
#if GFL_DEBUG
    , const s32           recursive_count 
#endif
)
{
#if GFL_DEBUG
  if( recursive_count >= GFL_GRP_G3D_PARTICLE_RECURSIVE_COUNT_MAX )
  {
    GFL_PRINT("�x��: �ċA�Ăяo���̉񐔂�%d��ɂȂ�܂����B\n", recursive_count);
  }
#endif 

  if(nw::ut::IsTypeOf<nw::gfx::ParticleModel>(nw_scene_node))
  {
    nw::gfx::ParticleModel* particle_model =
        nw::ut::DynamicCast<nw::gfx::ParticleModel*>(nw_scene_node);
    particle_model->ParticleAnimFrameController().SetStepFrame(step_frame);
  }
  else if(nw::ut::IsTypeOf<nw::gfx::ParticleEmitter>(nw_scene_node))
  {
    nw::gfx::ParticleEmitter* particle_emitter =
        nw::ut::DynamicCast<nw::gfx::ParticleEmitter*>(nw_scene_node);
    particle_emitter->ParticleAnimFrameController().SetStepFrame(step_frame);
  }

  nw::gfx::SceneNodeChildren::iterator end = nw_scene_node->GetChildEnd();
  for(nw::gfx::SceneNodeChildren::iterator child = nw_scene_node->GetChildBegin();
      child != end;
      ++child)
  {
    SetStepFrameRecursively(*child, step_frame
#if GFL_DEBUG
        , recursive_count +1
#endif 
    );
  }
}
#endif

//-----------------------------------------------------------------------------
/*
 *  @brief  �t���[����ݒ肷��
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 * 
 *  @param[in]     frame      �t���[��
 */
//-----------------------------------------------------------------------------
void Particle::SetFrame(const f32 frame)
{
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetFrame(frame);
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->ParticleAnimFrameController().SetFrame(frame);
    }
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief  �t���[�����擾����
 *
 *  @param[in]     frame      �t���[��
 */
//-----------------------------------------------------------------------------
f32 Particle::GetFrame( void ) const
{
  f32 frame = 0;
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      if(frame < (*i)->ParticleAnimFrameController().GetFrame() )
      {
        frame = (*i)->ParticleAnimFrameController().GetFrame();
      }
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      if( frame < (*i)->ParticleAnimFrameController().GetFrame() )
      {
        frame = (*i)->ParticleAnimFrameController().GetFrame();
      }
    }
  }
  
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief  ���Z�b�g���s���A������Ԃɖ߂�
 *          GPU�������̓R�[���ł��܂���B
 *          ���̂ǂ��炩�𖞂�����GPU�������ł͂���܂���B
 *          (1) �V�[���o�^���������Ă���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��̓V�[���o�^���������Ă���1�t���[���ȏ�o�߂��Ă���)
 *          (2) �I����Ԃł���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��͏I����ԂɂȂ��Ă���1�t���[���ȏ�o�߂��Ă���)
 */
//-----------------------------------------------------------------------------
void Particle::ResetEmitter(void)
{
  // @note ��L���ӂ�(1)�A(2)�𖞂����Ă��Ȃ��Ƃ���ASSERT�Ŏ~�߂����B
  // @note �G�~�b�^�[�̃��Z�b�g�����S�����ׂ�

  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->Reset();
    }
  }

}

//-----------------------------------------------------------------------------
/*
 *  @brief  ���Z�b�g���s���A������Ԃɖ߂�
 *          GPU�������̓R�[���ł��܂���B
 *          ���̂ǂ��炩�𖞂�����GPU�������ł͂���܂���B
 *          (1) �V�[���o�^���������Ă���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��̓V�[���o�^���������Ă���1�t���[���ȏ�o�߂��Ă���)
 *          (2) �I����Ԃł���B(�R�}���h�o�b�t�@���_�u���̂Ƃ��͏I����ԂɂȂ��Ă���1�t���[���ȏ�o�߂��Ă���)
 */
//-----------------------------------------------------------------------------
void Particle::Reset(void)
{
  // @note ��L���ӂ�(1)�A(2)�𖞂����Ă��Ȃ��Ƃ���ASSERT�Ŏ~�߂����B

  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      (*i)->Reset();
    }
  }
  
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      model->ForeachParticleSet(nw::gfx::ParticleSetsClear());
      model->ParticleAnimFrameController().SetFrame(0.f);
      // ���������̌����̂��߂̃q���g�������Z�b�g���܂�
      model->ResetDebugHint();
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   ���o�ʂ�ݒ肷��
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 *
 *  @param[in]     ratio  ���o��(0.0f�őS�����o����Ȃ�)
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionRatio(f32 ratio)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note Emitter��Parameter�̂ǂ���������ݒ�������Ă��邪�ǂ����ɐݒ肷��̂��������H�܂�Parameter�̂Ƃ���true��false���ǂ������������H���T���v���ɂ̂��Ƃ�Ƃ��̂܂܂ł����B
      res_particle_emitter_parameter.SetEmissionRatio(ratio);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   ���o�ʂ�ݒ肷��
 *
 *  @param[in]     name   �G�~�b�^�[�̖��O
 *  @param[in]     ratio  ���o��(0.0f�őS�����o����Ȃ�)
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionRatio(const char* name, f32 ratio)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( gfl::std::StringCmp(emitter->GetName(), name)==0 )
      {
        nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note Emitter��Parameter�̂ǂ���������ݒ�������Ă��邪�ǂ����ɐݒ肷��̂��������H�܂�Parameter�̂Ƃ���true��false���ǂ������������H���T���v���ɂ̂��Ƃ�Ƃ��̂܂܂ł����B
        res_particle_emitter_parameter.SetEmissionRatio(ratio);
      }
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   ���o�ʂ��擾����
 *
 *  @param[in]     name   �G�~�b�^�[�̖��O
 *
 *  @retval        f32    ���o��(0.0f�őS�����o����Ȃ�)
 */
//-----------------------------------------------------------------------------
f32 Particle::GetEmitterEmissionRatio(const char* name) const
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( gfl::std::StringCmp(emitter->GetName(), name)==0 )
      {
        nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note Emitter��Parameter�̂ǂ���������ݒ�������Ă��邪�ǂ����ɐݒ肷��̂��������H�܂�Parameter�̂Ƃ���true��false���ǂ������������H���T���v���ɂ̂��Ƃ�Ƃ��̂܂܂ł����B
        return res_particle_emitter_parameter.GetEmissionRatio();
      }
    }
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   ���݂̃p�[�e�B�N���̌����擾����
 *           �p�[�e�B�N�����\������S�Ă̂��̂Ō��ݑ��݂�����̂̌��̘a��Ԃ��܂�
 *
 *  @retval  s32  ���݂̃p�[�e�B�N���̌�
 */
//-----------------------------------------------------------------------------
s32  Particle::GetParticleCount(void) const
{
  s32 count = 0;

  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);

      u32 sets_num = model->GetParticleSetsCount();
      for(u32 j=0; j<sets_num; ++j)
      {
        nw::gfx::ParticleSet* particle_set = model->GetParticleSets(j);
        count += particle_set->GetParticleCollection()->GetCount();
      }
    }
  }

  return count;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   �I����Ԃ��ǂ������擾����
 *           �p�[�e�B�N�����\������S�Ă̂��̂��I����Ԃł����true��Ԃ��܂�
 *           1�ł��I����ԂłȂ����false��Ԃ��܂�
 *
 *  @retval  b32  �I����Ԃ̂Ƃ�true
 */
//-----------------------------------------------------------------------------
b32 Particle::IsDone(void) const
{
  {
    nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
    for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleModel* model = (*i);
      if( model->HasParticle() )
      {
        return false;
      }
    }
  }
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      if( emitter->IsAlive() )
      {
        return false;
      }
    }
  }

  // �����܂ŗ�����I�����
  return true;
}

//-----------------------------------------------------------------------------
/*
 *  @brief   �������o��ݒ肷��
 *           �G�~�b�^�[����̕��o�ɏI��肪���邩�ۂ��̐ݒ�ł��B1��1�̗��q�̎����̐ݒ�ł͂���܂���B
 * 
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 *
 *  @param[in]  infinity  �������o�ɂ���Ƃ�true
 */
//-----------------------------------------------------------------------------
void Particle::SetEmitterEmissionSpanInfinity(b32 infinity)
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note Emitter��Parameter�̂ǂ���������ݒ�������Ă��邪�ǂ����ɐݒ肷��̂��������H�܂�Parameter�̂Ƃ���true��false���ǂ������������H���T���v���ɂ̂��Ƃ�Ƃ��̂܂܂ł����B
      res_particle_emitter_parameter.SetEmissionSpanInfinity(infinity);
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   �������o���ǂ������擾����
 *           �p�[�e�B�N�����\������S�Ă̂��̂��������o�łȂ����false��Ԃ��܂�
 *           1�ł��������o�ł����true��Ԃ��܂�
 *
 *  @retval  b32  �������o�̂Ƃ�true
 */
//-----------------------------------------------------------------------------
b32 Particle::IsEmitterEmissionSpanInfinity(void) const
{
  {
    nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
    for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
         i != end;
         ++i )
    {
      nw::gfx::ParticleEmitter* emitter = (*i);
      const nw::gfx::res::ResParticleEmitterParameter res_particle_emitter_parameter = emitter->GetResParticleEmitterParameterCopy(false);  // @note Emitter��Parameter�̂ǂ���������ݒ�������Ă��邪�ǂ����ɐݒ肷��̂��������H�܂�Parameter�̂Ƃ���true��false���ǂ������������H���T���v���ɂ̂��Ƃ�Ƃ��̂܂܂ł����B
      if( res_particle_emitter_parameter.GetEmissionSpanInfinity() )
      {
        return true;
      }
    }
  }

  // �����܂ŗ����疳�����o�ł͂Ȃ�
  return false;
}


//-----------------------------------------------------------------------------
/*
 *  @brief   �Z�b�g�A�b�v
 *
 *  @param[in] particle_context �p�[�e�B�N���R���e�L�X�g
 */
//-----------------------------------------------------------------------------
void Particle::Setup(nw::gfx::ParticleContext* particle_context)
{
  if( !m_is_setup )
  {
    // �p�[�e�B�N���̎Q�Ɖ����E������
    nw::gfx::ParticleUtil::SetupParticleObject(m_scene_node_array, particle_context);
    // @note nw::gfx::ParticleUtil::SetupParticleObject�ɂ�particle_context�͗������擾���邱�Ƃɂ����g���Ă��Ȃ��B
    //       ����āAscene_calculate_group���ς��particle_context���ς�����Ƃ��Ă��A
    //       nw::gfx::ParticleUtil::SetupParticleObject���ēx��蒼���K�v�͂Ȃ��Ǝv����̂ŁA
    //       m_is_setup�t���O�𗧂Ă�1�񂵂�nw::gfx::ParticleUtil::SetupParticleObject���s���Ȃ��悤�ɂ��Ă����B
    //       ���̕��j��ύX���Ȃ��Ă��ςނ悤nw::gfx::ParticleUtil::SetupParticleObject�̒��g�����������Ȃ����Ƃ��肤�B
    //       ���������Anw::gfx::ParticleUtil::SetupParticleObject�͉�����Ăяo���Ă������̂Ȃ̂��낤���H
    //       ������Ăяo���Ă������̂Ȃ�A���̕��j���ύX�ł��Ĉ��S�Ȃ̂����E�E�E
    m_is_setup = true;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   �w�萔�����t���[������
 *
 *  ## �p�[�e�B�N�����\������S�Ă̂��̂ɑ΂��ē����l��ݒ肵�܂� ##
 *
 *  @param[in] times            �t���[�����񂷉�
 *  @param[in] particle_context �p�[�e�B�N���R���e�L�X�g
 */
//-----------------------------------------------------------------------------
void Particle::AddTimes(u32 times, nw::gfx::ParticleContext* particle_context)
{
  b32 particle_enable_swap_buffer = true;
   
  for(u32 k=0; k<times; ++k)
  {
    {
      nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
      for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleModel* model = (*i);
        model->UpdateParticleFrame();
      }
    }
    {
      nw::gfx::ParticleEmitterArray::iterator end = m_emitter_array->end();
      for( nw::gfx::ParticleEmitterArray::iterator i = m_emitter_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleEmitter* emitter = (*i);
        emitter->UpdateParticleFrame();
        emitter->Emission(particle_context);
      }
    }

    {
      nw::gfx::ParticleModelArray::iterator end = m_model_array->end();
      for( nw::gfx::ParticleModelArray::iterator i = m_model_array->begin();
           i != end;
           ++i )
      {
        nw::gfx::ParticleModel* model = (*i);
 
        u32 sets_num = model->GetParticleSetsCount();
        for(u32 j=0; j<sets_num; ++j)
        {
          nw::gfx::ParticleSet* particle_set = model->GetParticleSets(j);
          particle_set->UpdateParticles(particle_context, particle_enable_swap_buffer);
        }
      }
    }

    particle_enable_swap_buffer = false;  // @note ���L�������画�f����ƁA�R�}���h�o�b�t�@���_�u���łȂ��Ƃ���true�ɂ��Ă����ׂ����B���_�u��������false�ł����B
   
/*
�p�[�e�B�N���̍X�V�������s���܂��B

enableSwapBuffer��false�ɂ���ƁA�����Ńo�b�t�@�̃X���b�v���s���܂���B�ʏ��true�ɂ��Ă��������B�A�v���P�[�V�������ŃR�}���h�o�b�t�@���_�u���ɂ��Ă��āA����t���[����`�撆�ɕ�����UpdateNode���Ăԏꍇ�i�܂�R�}���������j�́A����t���[����2��ڈȍ~�̌Ăяo���ł�false�ɂ��Ă��������B

�ڂ�����ParticleDropFrameDemo���Q�Ƃ��Ă��������B
*/

/*
VBO�_�u���o�b�t�@
�p�[�e�B�N���͌v�Z�����A�`�揈������񉻂ł���悤�ɓ�����VBO���d�����Ă��܂��B
�o�b�t�@�̐؂�ւ��́AParticleSceneUpdater::UpdateNode����Ă΂��ParticleSet::UpdateParticles���Ŏ����I�ɍs���܂��B
*/

    // NintendoWare/CTR/sources/libraries/gfx/gfx_ParticleSceneUpdater.cpp 
    // ParticleSceneUpdater::UpdateNode
    // ���Q�l�ɂȂ�B
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief   �p�[�e�B�N�����f���̌��𓾂�
 *
 *  @retval    �p�[�e�B�N�����f���̌�
 */
//-----------------------------------------------------------------------------
s32 Particle::GetModelNum(void) const
{
  return m_model_array->size();  // size�͌��݂̗v�f���Acapacity�͕ێ��ł���v�f�̌��A����size<=capacity
}
//-----------------------------------------------------------------------------
/*
 *  @brief   �p�[�e�B�N�����f���𓾂�
 *
 *  @param[in] index  �C���f�b�N�X
 *  @retval    �p�[�e�B�N�����f��
 */
//-----------------------------------------------------------------------------
nw::gfx::ParticleModel* Particle::GetModel(s32 index) const
{
  nw::gfx::ParticleModel* ret = NULL;
  if( 0<=index && index<GetModelNum() )
  {
    ret = (*m_model_array)[index];
  }
  else
  {
    GFL_ASSERT_MSG(0, "index=%d, size=%d\n", index, GetModelNum());
  }
  return ret;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl
