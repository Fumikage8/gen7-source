//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLight.cpp
 *	@brief  3D���C�g
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

#include <grp/g3d/gfl_G3dLight.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D���C�g�N���X
//=====================================

// ���\�[�X���烉�C�g�𐶐�����Ƃ��̐ݒ���e
void Light::InitializeResourceDescriptionForDefault(
    ResourceDescription*  description
)
{
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}

// �ݒ���e
void Light::InitializeDescriptionForDefault(
    Description*  description
)
{
  InitializeDescriptionForFragmentLight(
      description,
      KIND_DIRECTIONAL
  );
}

void Light::InitializeDescriptionForAmbientLight(
    Description*  description
)
{
  description->type = TYPE_AMBIENT;
  description->kind = KIND_NONE;
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}
void Light::InitializeDescriptionForFragmentLight(
    Description*  description,
    const Kind    kind
)
{
  description->type = TYPE_FRAGMENT;
  description->kind = kind;
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}
void Light::InitializeDescriptionForHemiSphereLight(
    Description*  description
)
{
  description->type = TYPE_HEMI_SPHERE;
  description->kind = KIND_NONE;
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}
void Light::InitializeDescriptionForVertexLight(
    Description*  description,
    const Kind    kind
)
{
  description->type = TYPE_VERTEX;
  description->kind = kind;
  description->animation_enable = false;
  description->animation_blend_unit_num = 0;
  description->animation_num_in_unit = 0;
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 *                 Create�͌Ă΂�܂���B
 */
//-----------------------------------------------------------------------------
Light::Light(void)
  : SceneNode(),
    m_type(TYPE_NONE),
    m_kind(KIND_NONE),
    //m_animation(NULL)
    m_animation_blend_unit_num(0),
    m_animation_num_in_unit(0),
    m_animation_manager(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetLightManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 *                 Destroy���Ăт܂��B
 */
//-----------------------------------------------------------------------------
Light::~Light()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetLightManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     allocator          �A���P�[�^
 *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
 *  @param[in]     resource           ���\�[�X
 *  @param[in]     index_in_resource  ���\�[�X���ɂ�����C���f�b�N�X
 *  @param[in]     description        ���\�[�X���烉�C�g�𐶐�����Ƃ��̐ݒ���e
 */
//-----------------------------------------------------------------------------
void Light::Create(
    gfl::heap::NwAllocator*           allocator,
    gfl::heap::NwAllocator*           device_allocator,
    Resource*                         resource,
    s32                               index_in_resource,
    const ResourceDescription*        description
)
{
  ResourceDescription l_description;
  if( description )
  {
    l_description = *description;
  }

  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "���\�[�X���s���̂��߃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n" );
    return;
  }
  resource->PrintUnresolvedReference();

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResLight res = nw_res_graphics_file->GetLights(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      (m_animation_slot_num>0)?(true):(false),  // ������false����nw::gfx::AnimGroup���擾�ł��Ȃ�  // ������O������ݒ�ł���悤�ɂ��邩�A�A�j���[�V�����Ȃ����C�g�N���X�����邩
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      m_animation_slot_num
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%d�̃V�[���I�u�W�F�N�g�������ł��܂���ł����B\n", index_in_resource );

  nw::gfx::SceneNode* scene_node = nw::ut::DynamicCast<nw::gfx::SceneNode*>(scene_object);
  SetNwSceneNode(scene_node);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);

  CheckTypeAndKind();

  //m_animation_manager = GFL_NEW(allocator->GetHeapBase()) AnimationManager;
  //m_animation_manager->NoSlot_Create(
  //    allocator,
  //    device_allocator,
  //    this
  //);  
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
 *  @param[in]     description        �ݒ���e
 */
//-----------------------------------------------------------------------------
void Light::Create(
    gfl::heap::NwAllocator*           device_allocator,
    const Description*                description
)
{
  Description  l_description;
  InitializeDescriptionForDefault(&l_description);
  if( description )
  {
    l_description = *description;
  }

  switch( l_description.type )
  {
  case TYPE_AMBIENT:     CreateAmbientLight   (device_allocator, l_description); break;
  case TYPE_FRAGMENT:    CreateFragmentLight  (device_allocator, l_description); break;
  case TYPE_HEMI_SPHERE: CreateHemiSphereLight(device_allocator, l_description); break;
  case TYPE_VERTEX:      CreateVertexLight    (device_allocator, l_description); break;
  }

  m_animation_slot_num = (l_description.animation_enable)?(1):(0);
  m_animation_blend_unit_num = l_description.animation_blend_unit_num;
  m_animation_num_in_unit = l_description.animation_num_in_unit;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �j��
 *                 ����Ă�ł����v�B�f�X�g���N�^�͌Ă΂�܂���B 
 */
//-----------------------------------------------------------------------------
void Light::Destroy(void)
{
  // �Ǝ����� ��������
#if 0
  if( m_animation )
  {
    GFL_DELETE m_animation;
    m_animation = NULL;
  }
#endif
  if( m_animation_manager )
  {
    GFL_DELETE m_animation_manager;
    m_animation_manager = NULL;
  }
  m_animation_slot_num = 0;
  m_animation_blend_unit_num = 0;
  m_animation_num_in_unit = 0;
  // �Ǝ����� �����܂�

  SceneNode::Destroy();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �v�Z����
 */
//-----------------------------------------------------------------------------
void Light::Calculate(void)
{
  // �A�j���[�V�����}�l�[�W��
  if(m_animation_manager)
  {
    m_animation_manager->Calculate();
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V������ύX����
 *                 ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
 *
 *  @param[in]     allocator               �A���P�[�^
 *  @param[in]     res_anim                �A�j���[�V�������\�[�X
 *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B
 *                                         ���߂Ă��̊֐����Ă񂾂Ƃ��́A�g�p�����B
 *                                         Resource::GetLightMemberAnimSetCount�œ����l��n���Ƃ悢�B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
 *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
 */
//-----------------------------------------------------------------------------
void Light::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  s32                     max_anim_members
)
{
#if 0
  // ���߂Ă��̊֐����Ă񂾂Ƃ��́AAnimation�𐶐�����B
  if( m_animation == NULL )
  {
    nw::gfx::Light* nw_light = GetNwLight();
    nw::gfx::AnimGroup* anim_group = nw_light->GetAnimGroup();
    GFL_ASSERT_STOP(anim_group);  // �A�j���[�V�����O���[�v������܂���B

    m_animation = GFL_NEW(allocator->GetHeapBase()) Animation(
      anim_group->GetMemberCount(),
      max_anim_members,
      false
    );

    // �A�j���[�V���������߂ēK�p����
    m_animation->Change(
      allocator,
      res_anim
    );

    nw::gfx::AnimObject* nw_anim_object = m_animation->GetNwAnimObject();
    GFL_ASSERT_STOP(nw_anim_object);

    // �A�j���[�V�����O���[�v�� AnimEvaluator �Ƀo�C���h���܂��B
    // ����ɂ��A�A�j���[�V�����Ώۃ����o�ɃA�j���[�V�����f�[�^���֘A�t�����܂��B
    bool bind_result = nw_anim_object->Bind(anim_group);
    GFL_ASSERT_STOP(bind_result);
    
    // AnimEvaluator �����f���ɐݒ肵�܂��B
    // AnimEvaluator �͈�̃��f���ɑ΂��ĕ����ݒ肷�邱�Ƃ��ł��A
    // ���̍ۂɂ́AAnimEvaluator ���� objectIndex ���w�肵�܂��B
    // �ڂ����́A PartialAnimationDemo ���Q�Ƃ��Ă��������B
    //
    // �A�j���[�V�����̑Ώۂ��ނɂ���ėp����֐����قȂ�܂��B
    // SkeletalModel::SetSkeletalAnimObject
    // Model::SetVisibilityAnimObject
    // Model::SetMaterialAnimObject
    // Light::SetAnimObject
    nw_light->SetAnimObject(nw_anim_object);
  }
  else
  {
    // �A�j���[�V�����𒼂��ɕύX����
    m_animation->Change(
      allocator,
      res_anim
    );
  }
#endif
  ChangeAnimation(
      allocator,
      res_anim,
      0,
      max_anim_members
  );
}


void Light::ChangeAnimation(
  gfl::heap::NwAllocator* allocator,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members
)
{
  ChangeUnitAnimation(
    allocator,
    allocator,
    0,
    res_anim,
    change_frame,
    max_anim_members
  );
}


//-----------------------------------------------------------------------------
/*
 *  @brief         slot_index��unit_index�Ɋ��蓖�Ă�A�j���[�V������ύX����
 *
 *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
 *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
 *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
 *  @param[in]     unit_index              �X���b�g���ɂ��������j�b�g�̃C���f�b�N�X
 *  @param[in]     res_anim                �A�j���[�V�������\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     resource                ���\�[�X(NULL��n���ƑS��(�X�P���^���A�}�e���A���A�r�W�r���e�B)�̃A�j���[�V�������������܂�)
 *  @param[in]     animation_resource_type �A�j���[�V�������\�[�X�̃^�C�v(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     index_in_resource       ���\�[�X���ɂ�����C���f�b�N�X(resource��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *  @param[in]     change_frame            ���t���[���ŕύX���邩(���̊ԃu�����h�����)
 *  @param[in]     max_anim_members        �A�j���[�V�������郁���o�̍ő吔�B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 *                                         Resource::Get???MemberAnimSetCount�œ����l��n���Ƃ悢�B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X��GetMemberAnimSetCount���g�p���܂��B
 *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS��n���ƁA
 *                                         �ŏ���Change�œn�����A�j���[�V�������\�[�X�̃A�j���[�V�����O���[�v�̃����o���ɁA
 *                                         �����A���̃A�j���[�V�����^�C�v�ɂ�����ő吔�ɂ��܂��B
 *  @param[in]     force                   �����I�ɃA�j���[�V�������֘A�t���܂��B(res_anim��NULL�̂Ƃ��͎g�p����Ȃ��̂łĂ��Ɓ[�Ȓl��OK)
 */
//-----------------------------------------------------------------------------
void Light::ChangeUnitAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  u32                     unit_index,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Light\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,  // 1�����Ȃ�
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_ChangeAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1�����Ȃ��̂ŃC���f�b�N�X��0
      unit_index,
      res_anim,
      change_frame,
      max_anim_members,
      false
    );
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Light::GetAnimationFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
 *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
 *
 *  @param[in]     step_frame      �X�V�t���[��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationStepFrame(f32 step_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStepFrame(step_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStepFrame(0, step_frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
 *
 *  @retval        �X�V�t���[��
 */
//-----------------------------------------------------------------------------
f32 Light::GetAnimationStepFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetStepFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStepFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationStartFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStepFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Light::GetAnimationStartFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetStartFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetStartFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
 *
 *  @param[in]     frame           �t���[��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationEndFrame(f32 frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetEndFrame(frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetEndFrame(0, frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Light::GetAnimationEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return 0.0f;
  }
  return m_animation->GetEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->GetEndFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return 0.0f;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
 *
 *  @param[in]     start_frame     �J�n�t���[��
 *  @param[in]     end_frame       �I���t���[��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetStartAndEndFrame(start_frame, end_frame);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetStartAndEndFrame(0, start_frame, end_frame);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
 *
 *  @retval        bool    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Light::IsAnimationFrameEndFrame(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return false;
  }
  return m_animation->IsFrameEndFrame();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsFrameEndFrame(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return false;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
 *
 *  @retval        bool            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
 */
//-----------------------------------------------------------------------------
bool Light::IsAnimationLoop(void) const
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return false;
  }
  return m_animation->IsLoop();
#endif
  if( m_animation_manager )
  {
    return m_animation_manager->IsLoop(0);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
    return false;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
 *
 *  @param[in]     loop            ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
 */
//-----------------------------------------------------------------------------
void Light::SetAnimationLoop(bool loop)
{
#if 0
  if( m_animation == NULL )
  {
    GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    return;
  }
  m_animation->SetLoop(loop);
#endif
  if( m_animation_manager )
  {
    m_animation_manager->SetLoop(0, loop);
  }
  else
  {
    GFL_ASSERT_MSG( m_animation_manager, "�A�j���[�V����������܂���B\n" );  //@check
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �t���[������
 */
//-----------------------------------------------------------------------------
void Light::SetUnitFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitFrame(0, unit_index, frame);
  }
}
f32  Light::GetUnitFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitFrame(0, unit_index);
  }
  return 0.0f;
}
void Light::SetUnitStepFrame(u32 unit_index, f32 step_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStepFrame(0, unit_index, step_frame);
  }
}
f32  Light::GetUnitStepFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStepFrame(0, unit_index);
  }
  return 0.0f;
}
void Light::SetUnitStartFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartFrame(0, unit_index, frame);
  }
}
f32  Light::GetUnitStartFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitStartFrame(0, unit_index);
  }
  return 0.0f;
}
void Light::SetUnitEndFrame(u32 unit_index, f32 frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitEndFrame(0, unit_index, frame);
  }
}
f32  Light::GetUnitEndFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_GetUnitEndFrame(0, unit_index);
  }
  return 0.0f;
}
void Light::SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitStartAndEndFrame(0, unit_index, start_frame, end_frame);
  }
}
b32  Light::IsUnitFrameEndFrame(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitFrameEndFrame(0, unit_index);
  }
  return false;
}
b32  Light::IsUnitLoop(u32 unit_index) const
{
  if( m_animation_manager )
  {
    return m_animation_manager->UseOwn_IsUnitLoop(0, unit_index);
  }
  return false;
}
void Light::SetUnitLoop(u32 unit_index, b32 loop)
{
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_SetUnitLoop(0, unit_index, loop);
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         slot_index���ɂ��������j�b�g�̃A�j���[�V�������u�����h����
 *
 *  @param[in]     heap_allocator          �q�[�v�A���P�[�^
 *  @param[in]     device_allocator        �f�o�C�X�A���P�[�^
 *  @param[in]     slot_index              �X���b�g�̃C���f�b�N�X
 *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_index�̃A�j���[�V�������Ȃ��ɂ���
 *                                         AnimationSlot::MODE_ANIMATION           �u�����h����߂�animation_unit_index�Ŏw�肵���A�j���[�V���������蓖�Ă�
 *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  ��ԃu�����h
 *                                         AnimationSlot::MODE_BLEND_ADDER         ���Z�u�����h
 *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
 *                                         ���j�b�g0�̃u�����h�d��
 *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATOR��AnimationSlot::MODE_BLEND_ADDER�̂Ƃ��̂ݎg�p�B
 *                                         ���j�b�g1�̃u�����h�d��
 *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
 *                                                      false�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�̓o�C���h���̒l���u�����h����܂��B
 *                                                      true�ɂ���ƁA�A�j���[�V���������݂��Ȃ������o�͏d�� 0 �Ƃ��ău�����h����܂��B
 *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATOR�̂Ƃ��̂ݎg�p�B
 *                                                      true�ɂ���ƁA�u�����h�d�݂����v�� 1 �ɂȂ�悤���K�����Ă���u�����h���s���܂��B
 *                                                      false�ɂ���ƁA�ݒ肵���d�݂����̂܂܃u�����h�Ɏg�p�����B
 *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATION�̂Ƃ��̂ݎg�p�B
 *                                                      �X���b�g���̂ǂ̃��j�b�g�ɂ���A�j���[�V���������蓖�Ă邩���w�肷��B
 */
//-----------------------------------------------------------------------------
void Light::BlendUnitAnimation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    AnimationSlot::Mode        mode,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  if(m_animation_slot_num == 0 )
  {
    GFL_ASSERT_MSG(0, "NOT Animatable Light\n");
    return;
  }

  if( !m_animation_manager )
  {
    m_animation_manager = GFL_NEW(heap_allocator->GetHeapBase()) AnimationManager;
    m_animation_manager->UseOwn_Create(
        heap_allocator,
        device_allocator,
        this,
        m_animation_slot_num,  // 1�����Ȃ�
        m_animation_blend_unit_num,
        m_animation_num_in_unit
    );
  }
  if( m_animation_manager )
  {
    m_animation_manager->UseOwn_BlendAnimation(
      heap_allocator,
      device_allocator,
      0,  // 1�����Ȃ��̂ŃC���f�b�N�X��0
      mode,
      interpolator_ignore_no_anim_member,
      interpolator_normalization_enable,
      animation_unit_index
    );
  }
}
void Light::BlendUnit2Animation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    AnimationSlot::Mode        mode,
    f32                        weight0,
    f32                        weight1,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  BlendUnitAnimation(
    heap_allocator,
    device_allocator,
    mode,
    interpolator_ignore_no_anim_member,
    interpolator_normalization_enable,
    animation_unit_index
  );
  { 
    if(    mode == AnimationSlot::MODE_BLEND_INTERPOLATOR
        || mode == AnimationSlot::MODE_BLEND_ADDER )
    {
      SetUnitBlendWeight(0, weight0);
      SetUnitBlendWeight(1, weight1);
    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �u�����h�d��
 */
//-----------------------------------------------------------------------------
void Light::SetUnitBlendWeight(u32 unit_index, f32 weight)
{
  if( m_animation_manager )
  {
    m_animation_manager->SetBlendWeight(0, unit_index, weight);
  }
}
f32  Light::GetUnitBlendWeight(u32 unit_index)
{
  if( m_animation_manager )
  {
    return m_animation_manager->GetBlendWeight(0, unit_index);
  }
  return 0.0f;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ResAnim::Type�̃A�j���[�V�����ŕύX�����|�[�Y�����Z�b�g����
 *
 *  �A�j���[�V���������蓖�Ă��Ă���Ƃ��́A���̃A�j���[�V�����������������o�̓��Z�b�g����Ȃ��B
 *  �A�j���[�V���������蓖�Ă��Ă��Ȃ��Ƃ��́A�S�Ẵ����o�����Z�b�g�����B
 */
//-----------------------------------------------------------------------------
void Light::ResetAnimationPose(void)
{
  if( m_animation_manager )
  {
    m_animation_manager->Reset(ResAnim::TYPE_LIGHT);
  }
}




//-----------------------------------------------------------------------------
/*
 *  @brief         ���s�ړ�
 */
//-----------------------------------------------------------------------------
void Light::SetTranslate(f32 x, f32 y, f32 z)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( x, y, z );
}
void Light::SetTranslate(const gfl::math::VEC3& t)
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().SetTranslate( t );
}
void Light::GetTranslate(gfl::math::VEC3* t) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  transform_node->Transform().GetTranslate( t );
}
gfl::math::VEC3 Light::GetTranslate(void) const
{
  nw::gfx::TransformNode* transform_node = GetNwTransformNode();
  return transform_node->Transform().GetTranslate();
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 */
//-----------------------------------------------------------------------------
void Light::SetDirection(f32 x, f32 y, f32 z)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͕����͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetDirection(x, y, z);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetDirection(x, y, z);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetDirection(x, y, z);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetDirection(const gfl::math::VEC3& d)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͕����͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetDirection(d);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetDirection(d);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetDirection(d);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::GetDirection(gfl::math::VEC3* d) const
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      const nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͕����͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      const nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      *d = res_fragment_light.GetDirection();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      const nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      *d = res_hemi_sphere_light.GetDirection();
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      const nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      *d = res_vertex_light.GetDirection();
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
gfl::math::VEC3 Light::GetDirection(void) const
{
  gfl::math::VEC3 d(0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      const nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͕����͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      const nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      d = res_fragment_light.GetDirection();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      const nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      d = res_hemi_sphere_light.GetDirection();
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      const nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      d = res_vertex_light.GetDirection();
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return d;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X���擾����
 *
 *  @param[out]    matrix          �擾�����}�g���N�X���i�[���ĕԂ�
 *
 *  @retval        bool            ����Ɏ擾�ł�����true
 */
//-----------------------------------------------------------------------------
bool Light::GetWorldMatrix(gfl::math::MTX34* matrix) const
{
  nw::gfx::Light* nw_light = GetNwLight();
  if( nw_light )
  {
    const gfl::math::MTX34& mtx = nw_light->WorldMatrix();
    *matrix = mtx;
    return true;
  }
  return false;
}
//-----------------------------------------------------------------------------
/*
 *  @brief         ���[���h�}�g���N�X��ݒ肷��
 *                 Scene��Calculate�̌�łȂ��Ɣ��f����܂���B
 *
 *  @param[in]     matrix          �ݒ肷��}�g���N�X
 *
 *  @retval        bool            ����ɐݒ�ł�����true
 */
//-----------------------------------------------------------------------------
bool Light::SetWorldMatrix(const gfl::math::MTX34& matrix)
{
  nw::gfx::Light* nw_light = GetNwLight();
  if( nw_light )
  {
    gfl::math::MTX34& mtx = nw_light->WorldMatrix();
    mtx = matrix;
    
    // ���[���h�s����X�V�����̂ŁA�t�s��̃L���b�V���𖳌������܂��B
    nw_light->InvalidateInverseWorldMatrix();

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �J���[
 */
//-----------------------------------------------------------------------------
void Light::SetAmbient(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      res_ambient_light.SetAmbient(r,g,b);
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetAmbient(r,g,b);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓A���r�G���g�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetAmbient(r,g,b);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetAmbient(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      res_ambient_light.SetAmbient(r,g,b,a);
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetAmbient(r,g,b,a);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓A���r�G���g�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetAmbient(r,g,b,a);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetAmbient(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      res_ambient_light.SetAmbient(c);
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetAmbient(c);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓A���r�G���g�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetAmbient(c);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetAmbient(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetAmbient(r_f32, g_f32, b_f32);
}
void Light::SetAmbient(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetAmbient(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetAmbient(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetAmbient(c_f32);
}
gfl::grp::ColorF32 Light::GetAmbient(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      c = res_ambient_light.GetAmbient();
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      c = res_fragment_light.GetAmbient();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓A���r�G���g�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      c = res_vertex_light.GetAmbient();
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetAmbient(gfl::grp::ColorF32* c) const
{
  *c = GetAmbient();
}
void Light::GetAmbient(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetAmbient();
  *c = ConvertColorF32ToColorU8(c_f32);
}

void Light::SetDiffuse(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetDiffuse(r,g,b);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetDiffuse(r,g,b);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetDiffuse(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetDiffuse(r,g,b,a);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetDiffuse(r,g,b,a);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetDiffuse(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetDiffuse(c);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetDiffuse(c);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetDiffuse(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetDiffuse(r_f32, g_f32, b_f32);
} 
void Light::SetDiffuse(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetDiffuse(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetDiffuse(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetDiffuse(c_f32);
}
gfl::grp::ColorF32 Light::GetDiffuse(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      c = res_fragment_light.GetDiffuse();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓f�B�t���[�Y�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      c = res_vertex_light.GetDiffuse();
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetDiffuse(gfl::grp::ColorF32* c) const
{
  *c = GetDiffuse();
}
void Light::GetDiffuse(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetDiffuse();
  *c = ConvertColorF32ToColorU8(c_f32);
}

void Light::SetSpecular0(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular0(r,g,b);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular0(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular0(r,g,b,a);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular0(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular0(c);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular0(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSpecular0(r_f32, g_f32, b_f32);
}
void Light::SetSpecular0(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSpecular0(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetSpecular0(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetSpecular0(c_f32);
}
gfl::grp::ColorF32 Light::GetSpecular0(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      c = res_fragment_light.GetSpecular0();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetSpecular0(gfl::grp::ColorF32* c) const
{
  *c = GetSpecular0();
}
void Light::GetSpecular0(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetSpecular0();
  *c = ConvertColorF32ToColorU8(c_f32);
}

void Light::SetSpecular1(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular1(r,g,b);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular1(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular1(r,g,b,a);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular1(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetSpecular1(c);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSpecular1(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSpecular1(r_f32, g_f32, b_f32);
}
void Light::SetSpecular1(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSpecular1(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetSpecular1(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetSpecular1(c_f32);
}
gfl::grp::ColorF32 Light::GetSpecular1(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      c = res_fragment_light.GetSpecular1();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ̓X�y�L�����J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetSpecular1(gfl::grp::ColorF32* c) const
{
  *c = GetSpecular1();
}
void Light::GetSpecular1(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetSpecular1();
  *c = ConvertColorF32ToColorU8(c_f32);
}

void Light::SetGroundColor(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetGroundColor(r,g,b);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetGroundColor(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetGroundColor(r,g,b,a);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetGroundColor(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetGroundColor(c);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetGroundColor(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetGroundColor(r_f32, g_f32, b_f32);
}
void Light::SetGroundColor(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetGroundColor(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetGroundColor(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetGroundColor(c_f32);
}
gfl::grp::ColorF32 Light::GetGroundColor(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      c = res_hemi_sphere_light.GetGroundColor();
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͒n�ʃJ���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetGroundColor(gfl::grp::ColorF32* c) const
{
  *c = GetGroundColor();
}
void Light::GetGroundColor(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetGroundColor();
  *c = ConvertColorF32ToColorU8(c_f32);
}

void Light::SetSkyColor(f32 r, f32 g, f32 b)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetSkyColor(r,g,b);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͋�J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSkyColor(f32 r, f32 g, f32 b, f32 a)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetSkyColor(r,g,b,a);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͋�J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSkyColor(gfl::grp::ColorF32& c)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetSkyColor(c);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͋�J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
void Light::SetSkyColor(u8 r, u8 g, u8 b)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSkyColor(r_f32, g_f32, b_f32);
}
void Light::SetSkyColor(u8 r, u8 g, u8 b, u8 a)
{
  f32 r_f32 = (static_cast<f32>(r))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 g_f32 = (static_cast<f32>(g))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 b_f32 = (static_cast<f32>(b))/gfl::grp::ColorU8::ELEMENT_MAX;
  f32 a_f32 = (static_cast<f32>(a))/gfl::grp::ColorU8::ELEMENT_MAX;
  SetSkyColor(r_f32, g_f32, b_f32, a_f32);
}
void Light::SetSkyColor(const gfl::grp::ColorU8& c)
{
  gfl::grp::ColorF32 c_f32 = ConvertColorU8ToColorF32(c);
  SetSkyColor(c_f32);
}
gfl::grp::ColorF32 Light::GetSkyColor(void) const
{
  gfl::grp::ColorF32 c(0.0f, 0.0f, 0.0f, 0.0f);

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      GFL_ASSERT_MSG( 0, "�t���O�����g���C�g�ɂ͋�J���[�͂���܂���B\n" );
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      c = res_hemi_sphere_light.GetSkyColor();
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͋�J���[�͂���܂���B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return c;
}
void Light::GetSkyColor(gfl::grp::ColorF32* c) const
{
  *c = GetSkyColor();
}
void Light::GetSkyColor(gfl::grp::ColorU8*  c) const
{
  gfl::grp::ColorF32 c_f32 = GetSkyColor();
  *c = ConvertColorF32ToColorU8(c_f32);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��������
 */
//-----------------------------------------------------------------------------
void Light::SetDistanceLookUpTable(
    Resource*    lookup_table_resource,
    s32          lookup_table_set_index_in_resource,
    s32          lookup_table_index_in_lookup_table_set
)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      GFL_ASSERT_MSG( 0, "�A���r�G���g���C�g�ɂ͋��������͂���܂���B\n" );
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();

      nw::gfx::ResGraphicsFile* nw_res_graphics_file = lookup_table_resource->GetNwResGraphicsFile();
      nw::gfx::res::ResLookupTableSet nw_res_lookup_table_set = nw_res_graphics_file->GetLutSets(lookup_table_set_index_in_resource);
      nw::gfx::res::ResImageLookupTable nw_res_image_lookup_table = nw_res_lookup_table_set.GetSamplers(lookup_table_index_in_lookup_table_set);

      res_fragment_light.SetDistanceSampler(nw_res_image_lookup_table);  // @note ForceSetupDistanceSampler�Ƃ̈Ⴂ�����܂����s��
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      GFL_ASSERT_MSG( 0, "�������C�g�ɂ͋��������͂���܂���B\n" );
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      GFL_ASSERT_MSG( 0, "���_���C�g�ɂ͋��������͂���̂����A�Q�ƃe�[�u���̎擾/�ݒ�͂ł��Ȃ��B\n" );
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �L���t���O
 */
//-----------------------------------------------------------------------------
void Light::SetEnable(bool enable)
{
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      res_ambient_light.SetLightEnabled(enable);
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      res_fragment_light.SetLightEnabled(enable);
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      res_hemi_sphere_light.SetLightEnabled(enable);
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      res_vertex_light.SetLightEnabled(enable);
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }
}
bool Light::IsEnable(void) const
{
  bool enable = false;

  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      nw::gfx::res::ResAmbientLight res_ambient_light = GetNwResAmbientLight();
      enable = res_ambient_light.IsLightEnabled();
    }
    break;
  case TYPE_FRAGMENT:
    {
      nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      enable = res_fragment_light.IsLightEnabled();
    } 
    break;
  case TYPE_HEMI_SPHERE:
    {
      nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = GetNwResHemiSphereLight();
      enable = res_hemi_sphere_light.IsLightEnabled();
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      enable = res_vertex_light.IsLightEnabled();
#endif
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    }
    break;
  }

  return enable;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 */
//-----------------------------------------------------------------------------
nw::gfx::res::ResAmbientLight     Light::GetNwResAmbientLight(void) const
{
  if( m_type == TYPE_AMBIENT )
  {
    nw::gfx::AmbientLight*        ambient_light     = GetNwAmbientLight();
    nw::gfx::res::ResAmbientLight res_ambient_light = ambient_light->GetResAmbientLight();
    return res_ambient_light;  
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���قȂ�܂�\n" );
    nw::gfx::res::ResAmbientLight res_ambient_light;
    return res_ambient_light;  
  }
}
nw::gfx::res::ResFragmentLight    Light::GetNwResFragmentLight(void) const
{
  if( m_type == TYPE_FRAGMENT )
  {
    nw::gfx::FragmentLight*        fragment_light     = GetNwFragmentLight();
    nw::gfx::res::ResFragmentLight res_fragment_light = fragment_light->GetResFragmentLight();
    return res_fragment_light;  
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���قȂ�܂�\n" );
    nw::gfx::res::ResFragmentLight res_fragment_light;
    return res_fragment_light;  
  }
}
nw::gfx::res::ResHemiSphereLight  Light::GetNwResHemiSphereLight(void) const
{
  if( m_type == TYPE_HEMI_SPHERE )
  {
    nw::gfx::HemiSphereLight*        hemi_sphere_light     = GetNwHemiSphereLight();
    nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light = hemi_sphere_light->GetResHemiSphereLight();
    return res_hemi_sphere_light;  
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���قȂ�܂�\n" );
    nw::gfx::res::ResHemiSphereLight res_hemi_sphere_light;
    return res_hemi_sphere_light;  
  }
}

#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
nw::gfx::res::ResVertexLight      Light::GetNwResVertexLight(void) const
{
  if( m_type == TYPE_VERTEX )
  {
    nw::gfx::VertexLight*        vertex_light     = GetNwVertexLight();
    nw::gfx::res::ResVertexLight res_vertex_light = vertex_light->GetResVertexLight();
    return res_vertex_light;  
  }
  else
  {
    GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���قȂ�܂�\n" );
    nw::gfx::res::ResVertexLight res_vertex_light;
    return res_vertex_light;  
  }
}
#endif

void Light::CheckTypeAndKind(void)
{
  m_type = TYPE_NONE;
  m_kind = KIND_NONE;
  
  // Type
  do
  {
    nw::gfx::AmbientLight*     ambient_light     = GetNwAmbientLight();
    if( ambient_light )     {  m_type = TYPE_AMBIENT;  break;  }
    nw::gfx::FragmentLight*    fragment_light    = GetNwFragmentLight();
    if( fragment_light )    {  m_type = TYPE_FRAGMENT;  break;  }
    nw::gfx::HemiSphereLight*  hemi_sphere_light = GetNwHemiSphereLight();
    if( hemi_sphere_light ) {  m_type = TYPE_HEMI_SPHERE;  break;  }
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
    nw::gfx::VertexLight*      vertex_light      = GetNwVertexLight();
    if( vertex_light )      {  m_type = TYPE_VERTEX;  break;  }
#endif
  }
  while(0);
  if( m_type == TYPE_NONE )
  {
    GFL_ASSERT_MSG( 0, "���C�g�̃^�C�v���s���ł��B\n" );
    return;
  }

  // Kind
  switch( m_type )
  {
  case TYPE_AMBIENT:
    {
      // Kind�Ȃ�
    }
    break;
  case TYPE_FRAGMENT:
    {
      const nw::gfx::res::ResFragmentLight res_fragment_light = GetNwResFragmentLight();
      s32 kind = res_fragment_light.GetLightKind();
      switch( kind )
      {
      case nw::gfx::res::ResLight::KIND_DIRECTIONAL: m_kind = KIND_DIRECTIONAL; break;
      case nw::gfx::res::ResLight::KIND_POINT:       m_kind = KIND_POINT;       break;
      case nw::gfx::res::ResLight::KIND_SPOT:        m_kind = KIND_SPOT;        break;
      }
    }
    break;
  case TYPE_HEMI_SPHERE:
    {
      // Kind�Ȃ�
    }
    break;
  case TYPE_VERTEX:
    {
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
      const nw::gfx::res::ResVertexLight res_vertex_light = GetNwResVertexLight();
      s32 kind = res_vertex_light.GetLightKind();
      switch( kind )
      {
      case nw::gfx::res::ResLight::KIND_DIRECTIONAL: m_kind = KIND_DIRECTIONAL; break;
      case nw::gfx::res::ResLight::KIND_POINT:       m_kind = KIND_POINT;       break;
      case nw::gfx::res::ResLight::KIND_SPOT:        m_kind = KIND_SPOT;        break;
      }
#endif
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ����
 *
 *  @param[in]     device_allocator   �f�o�C�X�������̃A���P�[�^
 *  @param[in]     description        �ݒ���e
 */
//-----------------------------------------------------------------------------
void Light::CreateAmbientLight(
    gfl::heap::NwAllocator*           device_allocator,
    const Description&                description 
)
{
  nw::gfx::AmbientLight* ambient_light = 
    nw::gfx::AmbientLight::DynamicBuilder()
      .IsFixedSizeMemory(false)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(device_allocator);

  // Kind�Ȃ�

  SetNwSceneNode(ambient_light);

  CheckTypeAndKind();
}

void Light::CreateFragmentLight(
    gfl::heap::NwAllocator*           device_allocator,
    const Description&                description 
)
{
  nw::gfx::FragmentLight* fragment_light =
    nw::gfx::FragmentLight::DynamicBuilder()
      .IsFixedSizeMemory(false)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(device_allocator);

  nw::gfx::res::ResFragmentLight res_fragment_light = fragment_light->GetResFragmentLight();
  s32 nw_kind = nw::gfx::res::ResLight::KIND_UNUSED;
  switch( description.kind )
  {
  case KIND_DIRECTIONAL: nw_kind = nw::gfx::res::ResLight::KIND_DIRECTIONAL; break;
  case KIND_POINT:       nw_kind = nw::gfx::res::ResLight::KIND_POINT;       break;
  case KIND_SPOT:        nw_kind = nw::gfx::res::ResLight::KIND_SPOT;        break;
  }
  res_fragment_light.SetLightKind(nw_kind);

  SetNwSceneNode(fragment_light);

  CheckTypeAndKind();
}

void Light::CreateHemiSphereLight(
    gfl::heap::NwAllocator*           device_allocator,
    const Description&                description 
)
{
  nw::gfx::HemiSphereLight* hemi_sphere_light = 
    nw::gfx::HemiSphereLight::DynamicBuilder()
      .IsFixedSizeMemory(false)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(device_allocator);

  // Kind�Ȃ�

  SetNwSceneNode(hemi_sphere_light);

  CheckTypeAndKind();
}

void Light::CreateVertexLight(
    gfl::heap::NwAllocator*           device_allocator,
    const Description&                description 
)
{
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
  nw::gfx::VertexLight* vertex_light =
    nw::gfx::VertexLight::DynamicBuilder()
      .IsFixedSizeMemory(false)
      .MaxChildren(0)
      .MaxCallbacks(0)
      .Create(device_allocator);

  nw::gfx::res::ResVertexLight res_vertex_light = vertex_light->GetResVertexLight();
  s32 nw_kind = nw::gfx::res::ResLight::KIND_UNUSED;
  switch( description.kind )
  {
  case KIND_DIRECTIONAL: nw_kind = nw::gfx::res::ResLight::KIND_DIRECTIONAL; break;
  case KIND_POINT:       nw_kind = nw::gfx::res::ResLight::KIND_POINT;       break;
  case KIND_SPOT:        nw_kind = nw::gfx::res::ResLight::KIND_SPOT;        break;
  }
  res_vertex_light.SetLightKind(nw_kind);

  SetNwSceneNode(vertex_light);

  CheckTypeAndKind();
#endif
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

