//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationManager.cpp
 *	@brief  3D�A�j���[�V���������蓖�Ă�X���b�g�̊Ǘ�
 *	@author	Koji Kawada
 *	@date		2011.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>
#include <nw/ut.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dUnitAnimation.h>
#include <grp/g3d/gfl_G3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dModel.h>

#include <grp/g3d/gfl_G3dAnimationManager.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	�A�j���[�V���������蓖�Ă�X���b�g�̊Ǘ�
//=====================================

s32 AnimationManager::GetMaxMembers( Object* object, ResAnim::Type type )
{
  s32 max_members = 0;
  nw::gfx::AnimGroup* anim_group = AnimationManager::GetNwAnimGroupAndMemberCount(object, type, &max_members);
  return max_members;
}

nw::gfx::AnimGroup* AnimationManager::GetNwAnimGroupAndMemberCount( Object* object, ResAnim::Type type, s32* member_count )
{
  nw::gfx::AnimGroup* anim_group = NULL;
  s32 max_members = 0;
  
  ObjectType object_type = object->GetObjectType();
  switch( object_type )
  {
  case OBJECT_TYPE_MODEL:
    {
      nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>((reinterpret_cast<Model*>(object))->GetNwModel());
      if( skeletal_model ) 
      {
        nw::gfx::ResSkeletalModel res_skeletal_model = skeletal_model->GetResSkeletalModel();
        nw::gfx::ResSkeleton      res_skeleton       = res_skeletal_model.GetSkeleton();
        s32 bones_count = res_skeleton.GetBonesCount();

        // ���f������A�j���[�V�����O���[�v���擾���܂��B
        //
        // �A�j���[�V�����O���[�v�̓A�j���[�V�����Ώۃ����o�ւ̃|�C���^��ێ����܂��B 
        // �Ώۃ����o�Ƃ́A���f�������A�j���[�V�����\�ȗv�f
        //�i�Ⴆ�΁A�}�e���A���A�j���[�V�����ł� Diffuse �� Texture �Ȃǁj�ł��B
        // �����ł́A�X�P���^�����f������A�{�[���̃g�����X�t�H�[�����܂�
        // �A�j���[�V�����O���[�v���擾���܂��B
        //
        // �A�j���[�V�����̑Ώۂ��ނɂ���ėp����֐����قȂ�܂��B
        // �ESkeletalModel::GetSkeletalAnimGroup
        // �EModel::GetVisibilityAnimGroup
        // �EModel::GetMaterialAnimGroup
        // �ELight::GetAnimGroup
        // �ECamera::GetAnimGroup
        switch( type )
        {
        case ResAnim::TYPE_SKELETAL:
          {
            anim_group = skeletal_model->GetSkeletalAnimGroup();
          }
          break;
        case ResAnim::TYPE_MATERIAL:
          {
            anim_group = skeletal_model->GetMaterialAnimGroup();
          }
          break;
        case ResAnim::TYPE_VISIBILITY:
          {
            anim_group = skeletal_model->GetVisibilityAnimGroup();
          }
          break;
        }

        if( anim_group )
        {
          max_members = anim_group->GetMemberCount();
          if( max_members < bones_count ) max_members = bones_count;  // �����ق����̗p����΂���
          //GFL_PRINT("anim_group_member_count=%d, bones_count=%d\n",anim_group_member_count, bones_count);
          // �o�͌��� anim_group_member_count=52, bones_count=52 �X�P���^���A�j���Ȃ炱�̗��҂͓����l�ɂȂ�Ǝv���Ă����̂��ȁH
        }
        else
        {
          GFL_ASSERT_MSG(0, "�A�j���[�V�����O���[�v������܂���B\n");
        }
      }
      else
      {
        nw::gfx::Model* model = nw::ut::DynamicCast<nw::gfx::Model*>((reinterpret_cast<Model*>(object))->GetNwModel());
        if( model )
        {
          switch( type )
          {
          case ResAnim::TYPE_MATERIAL:
            {
              anim_group = model->GetMaterialAnimGroup();
            }
            break;
          case ResAnim::TYPE_VISIBILITY:
            {
              anim_group = model->GetVisibilityAnimGroup();
            }
            break;
          }
          
          if( anim_group )
          {
            max_members = anim_group->GetMemberCount();
          }
          else
          {
            GFL_ASSERT_MSG(0, "�A�j���[�V�����O���[�v������܂���B\n");
          }
        }
      }
    }
    break;
  case OBJECT_TYPE_CAMERA:
  case OBJECT_TYPE_STEREO_CAMERA:
    {
      nw::gfx::Camera* camera = nw::ut::DynamicCast<nw::gfx::Camera*>((reinterpret_cast<Camera*>(object))->GetNwCamera());
      if( camera )
      {
        switch( type )
        {
        case ResAnim::TYPE_CAMERA:
          {
            anim_group = camera->GetAnimGroup();
          }
          break;
        }
        
        if( anim_group )
        {
          max_members = anim_group->GetMemberCount();
        }
        else
        {
          GFL_ASSERT_MSG(0, "�A�j���[�V�����O���[�v������܂���B\n");
        }
      }
    }
    break;
  case OBJECT_TYPE_LIGHT:
    {
      nw::gfx::Light* light = nw::ut::DynamicCast<nw::gfx::Light*>((reinterpret_cast<Light*>(object))->GetNwLight());
      if( light )
      {
        switch( type )
        {
        case ResAnim::TYPE_LIGHT:
          {
            anim_group = light->GetAnimGroup();
          }
          break;
        }
        
        if( anim_group )
        {
          max_members = anim_group->GetMemberCount();
        }
        else
        {
          GFL_ASSERT_MSG(0, "�A�j���[�V�����O���[�v������܂���B\n");
        }
      }
    }
    break;
  default:
    {
      GFL_ASSERT_MSG(0, "�I�u�W�F�N�g�̎��%d���ُ�ł��B", object_type);
    }
    break;
  }

  *member_count = max_members;
  return anim_group;
}

AnimationManager::AnimationManager(void)
  : m_object(NULL),
    m_slot_num(0),
    m_slot_array(NULL),
    m_use_own_animation(false),
    m_own_slot_blend_num_max(0),
    m_own_unit_animation_num_max(0),
    m_own_unit_animation_num_over(0),
    m_own_set_slot_array(NULL),
    m_own_animation_array(NULL),
    m_own_unit_animation_array(NULL),
    m_own_unit_animation_next_index_array(NULL)
{
}

AnimationManager::~AnimationManager()
{
  Destroy();
}

void AnimationManager::Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Object*                 object,
    Description*            description
)
{
  m_object = object;
  
  Description l_description;
  if( description )
  {
    l_description = *description;
  }

  m_slot_num = l_description.slot_num;
 
  if( m_slot_num > 0 )
  {
    m_slot_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) AnimationSlot[m_slot_num];
    for( u32 i=0; i<m_slot_num; ++i )
    {
      if( l_description.slot_description_array )
      {
        m_slot_array[i].Create(
            heap_allocator,
            device_allocator,
            &(l_description.slot_description_array[i])
        );
      }
      m_slot_array[i].SetAnimObjectIndex(i);
    }
  }

  // own
  if( l_description.use_own_animation )
  {
    m_own_slot_blend_num_max          = l_description.own_slot_blend_num_max;
    m_own_unit_animation_num_max      = l_description.own_unit_animation_num_max;
 
    if(
           m_slot_num > 0
        && m_own_slot_blend_num_max > 0
        && m_own_unit_animation_num_max > 0
    )
    {
      m_own_unit_animation_num_over = (m_own_unit_animation_num_max==1)?(1):(m_own_unit_animation_num_max+1);  // 1�����Ȃ��Ȃ��Ԃ��Ȃ���ύX�͂��Ȃ�
    
      m_own_set_slot_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) b32[m_slot_num];
      m_own_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Animation***[m_slot_num];
      m_own_unit_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) UnitAnimation**[m_slot_num];
      m_own_unit_animation_next_index_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) s32*[m_slot_num];
      for( u32 i=0; i<m_slot_num; ++i )
      {
        m_own_set_slot_array[i] = false;
        m_own_animation_array[i] = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Animation**[m_own_slot_blend_num_max];
        m_own_unit_animation_array[i] = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) UnitAnimation*[m_own_slot_blend_num_max];
        m_own_unit_animation_next_index_array[i] = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) s32[m_own_slot_blend_num_max];
        for( s32 j=0; j<m_own_slot_blend_num_max; ++j )
        {
          m_own_animation_array[i][j] = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Animation*[m_own_unit_animation_num_over];
          m_own_unit_animation_array[i][j] = NULL;
          m_own_unit_animation_next_index_array[i][j] = 0;
          for( s32 k=0; k<m_own_unit_animation_num_over; ++k )
          {
            m_own_animation_array[i][j][k] = NULL;
          }
        }
      }
      m_use_own_animation = true;
    }
    else
    {
      m_own_unit_animation_num_over = 0;
      m_use_own_animation = false;
    }
    
    // @note own���g�p����Ƃ��́A�O����Animation��UnitAnimation��n���Ȃ��悤�ɂ��邩�ǂ����悤��
  }
}

// UseOwn�p
void AnimationManager::UseOwn_Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    Object*                 object,
    u32                     slot_num,
    u32                     unit_num,
    u32                     animation_num_in_unit
)
{
  Description description;
  description.slot_num                   = slot_num;
  description.slot_description_array     = NULL;
  description.use_own_animation          = true;
  description.own_slot_blend_num_max     = (unit_num==0)?(1):(unit_num);
  description.own_unit_animation_num_max = (animation_num_in_unit==0)?(1):(animation_num_in_unit);  // 2
  
  Create(
    heap_allocator,
    device_allocator,
    object,
    &description
  );

  // �܂������A�j���[�V�����𐶐����Ă��Ȃ��̂ŌĂ΂Ȃ�
  //for( u32 i=0; i<m_slot_num; ++i ) m_slot_array[i].CreateAnimation(m_own_unit_animation_array[i][0]);
}

void AnimationManager::Destroy(void)
{
  SetAnimObjectAllSlot(false);

  if( m_own_unit_animation_next_index_array )
  {
    for( s32 i=0; i<m_slot_num; ++i )
    {
      GFL_DELETE_ARRAY m_own_unit_animation_next_index_array[i];
      m_own_unit_animation_next_index_array[i] = NULL;
    }
    GFL_DELETE_ARRAY m_own_unit_animation_next_index_array;
    m_own_unit_animation_next_index_array = NULL;
  }

  if( m_own_unit_animation_array )
  {
    for( s32 i=0; i<m_slot_num; ++i )
    {
      for( s32 j=0; j<m_own_slot_blend_num_max; ++j )
      {
        if( m_own_unit_animation_array[i][j] )
        {
          GFL_DELETE m_own_unit_animation_array[i][j];
          m_own_unit_animation_array[i][j] = NULL;
        }
      }
      GFL_DELETE_ARRAY m_own_unit_animation_array[i];
      m_own_unit_animation_array[i] = NULL;
    }
    GFL_DELETE_ARRAY m_own_unit_animation_array;
    m_own_unit_animation_array = NULL;
  }

  if( m_own_animation_array )
  {
    for( s32 i=0; i<m_slot_num; ++i )
    {
      for( s32 j=0; j<m_own_slot_blend_num_max; ++j )
      {
        for( s32 k=0; k<m_own_unit_animation_num_over; ++k )
        {
          if( m_own_animation_array[i][j][k] )
          {
            GFL_DELETE m_own_animation_array[i][j][k];
            m_own_animation_array[i][j][k] = NULL;
          }
        }
        GFL_DELETE_ARRAY m_own_animation_array[i][j];
        m_own_animation_array[i][j] = NULL;
      }
      GFL_DELETE_ARRAY m_own_animation_array[i];
      m_own_animation_array[i] = NULL;
    }
    GFL_DELETE_ARRAY m_own_animation_array;
    m_own_animation_array = NULL;
  }

  if( m_own_set_slot_array )
  {
    GFL_DELETE_ARRAY m_own_set_slot_array;
    m_own_set_slot_array = NULL;
  }

  if( m_slot_array )
  {
    GFL_DELETE_ARRAY m_slot_array;
    m_slot_array = NULL;
  }

  m_object = NULL;
  m_slot_num = 0;
  m_use_own_animation = false;
  m_own_slot_blend_num_max      = 0; 
  m_own_unit_animation_num_max  = 0; 
  m_own_unit_animation_num_over = 0;
}

void AnimationManager::Calculate(void)
{
  for( u32 i=0; i<m_slot_num; ++i )
  {
    m_slot_array[i].Calculate();
  }

  SetAnimObjectAllSlot(true);
}

void AnimationManager::CreateSlot(
  u32                         slot_index,
  gfl::heap::NwAllocator*     heap_allocator,
  gfl::heap::NwAllocator*     device_allocator,
  AnimationSlot::Description* slot_description_array
)
{
  if( slot_index < m_slot_num )
  {
    m_slot_array[slot_index].Create(
        heap_allocator,
        device_allocator,
        slot_description_array
    );
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}

ResAnim::Type AnimationManager::GetSlotType(
  u32                         slot_index
) const
{
  ResAnim::Type type = ResAnim::TYPE_NONE;
  if( slot_index < m_slot_num )
  { 
    type = m_slot_array[slot_index].GetType();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return type;
}

s32 AnimationManager::GetSlotAnimationNum(
  u32                         slot_index
) const
{
  s32 num = 0;
  if( slot_index < m_slot_num )
  { 
    num = m_slot_array[slot_index].GetAnimationNum();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return num;
}


b32 AnimationManager::IsSlotEnable(u32 slot_index) const
{
  if( slot_index < m_slot_num )
  { 
    return m_slot_array[slot_index].IsEnable();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return false;
}
void AnimationManager::SetSlotEnable(u32 slot_index, b32 enable)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetEnable(enable);
    SetAnimObject(slot_index, enable);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}


void AnimationManager::Reset(ResAnim::Type type)
{
  s32 max_members;
  nw::gfx::AnimGroup* anim_group = AnimationManager::GetNwAnimGroupAndMemberCount( m_object, type, &max_members );

  if( anim_group )
  {
    anim_group->Reset();

    ///////////////////////////////////////////////
#if 0
    /*
    @note ������  2011�N12��1������
    ������0�̂Ƃ��AReset�ŉ����t���O�𗧂ĂĂ�����ۂ��B
    ���̃t���O�̂����ŁA1�t���[���������ɖ߂��Ă��܂��Ă���悤���B
    ���̃t���O�͎��̃t���[���ɍs���܂ŒN�����Z�b�g���Ȃ��݂����B
    �ȉ��̎����ł́A����1�t���[�����Ȃ��Ȃ��������������Ȃ��Ȃ��Ă��܂����B
    ���Ȃ݂ɁA���̃t���O�Ƃ�FLAG_IS_TRANSLATE_ZERO�̂悤��_ZERO�n���ȁB
    �X�P���^���ȊO�̃A�j��(�}�e���A���A�r�W�r���e�B�A�J�����A���C�g)���Ώ�����K�v�����肻���B
    */
    {
      ObjectType object_type = m_object->GetObjectType();
      switch( object_type )
      {
      case OBJECT_TYPE_MODEL:
        {
          nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>((reinterpret_cast<Model*>(m_object))->GetNwModel());
          if( skeletal_model ) 
          {
            nw::gfx::Skeleton* skeleton = skeletal_model->GetSkeleton();
            //skeleton->SetUpdated(false);�Ӗ��Ȃ�������

            nw::gfx::Skeleton::TransformPose& local_pose = skeleton->LocalTransformPose();
            nw::gfx::Skeleton::TransformPose& world_pose = skeleton->WorldTransformPose();

            nw::gfx::ResSkeletalModel res_skeletal_model = skeletal_model->GetResSkeletalModel();
            nw::gfx::ResSkeleton      res_skeleton       = res_skeletal_model.GetSkeleton();
            s32 bones_count = res_skeleton.GetBonesCount();

            for( s32 i=0; i<bones_count; ++i )
            {
              nw::gfx::CalculatedTransform* local_t = local_pose.GetTransform(i);
              local_t->ResetFlags();
              local_t->EnableFlags(nw::gfx::CalculatedTransform::FLAG_IS_DIRTY);
              nw::gfx::CalculatedTransform* world_t = world_pose.GetTransform(i);
              world_t->ResetFlags();
              world_t->EnableFlags(nw::gfx::CalculatedTransform::FLAG_IS_DIRTY);
            }
          }
        }
        break;
      }
    }
#endif
    ///////////////////////////////////////////////
  }

/*nw::gfx::BaseAnimEvaluator::Reset �����o�֐�
����
�}�e���A���ɑ΂��čs���ꍇ�A�}�e���A���̃o�b�t�@�𐶐�����K�v������܂��B 
SceneBuilder �N���X�Ń��f�����쐬����ۂ̃I�v�V�����ŁA SceneBuilder::BufferOption ��ݒ肵�Ă��������B 
BufferOption �ɂ́A Model::MULTI_FLAG_ANIMATABLE_MATERIAL �̓��e���Œ���K�v�ł��B*/

/*CTR �O���t�B�b�N�X���╔��
[����][NW4C] Material �� Visibility �̃A�j���[�V�����̒�~�̕��@�ɂ���
�c����NTSC 2011-04-11 16:52:15  
>Material �� Visibility �A�j���ŕ\��ω���{��}�[�N�̕\�����s���Ă���̂ł����A������A�j���Đ��O�̊�{�̕\��ɖ߂������ꍇ�ABaseAnimEvaluator::Reset �����s����Ηǂ��̂ł��傤���H
�͂��B���̊֐��Ń��Z�b�g�\�ł��B
�A�j���[�V�����\�����o�ȊO�����Z�b�g�������̂ł����AnimGroup::Reset���g���������܂��B*/

/*���╔���ɂ͏�L�̂悤�ɉ񓚂��Ă�������
nw::gfx::BaseAnimEvaluator��Reset��
nw::gfx::AnimGroup��Reset��
���g�������������B

nw::gfx::BaseAnimEvaluator��Reset��AnimGroup��Bind���ĂȂ��Ǝg�p�ł��Ȃ��B
�A�j���[�V�������S��Bind����Ă��Ȃ��Ƃ���T�|�[�Y�ɖ߂����߂Ƀ��Z�b�g���邱�Ƃ��l������
nw::gfx::AnimGroup��Reset�̕����g�����Ƃɂ����B*/

#if 0
gfx_BaseAnimEvaluator.h
gfx_BaseAnimEvaluator.cpp

class BaseAnimEvaluator

    //! @brief ���ׂẴA�j���[�V�����Ώۃ����o�[�𐶐����̏�Ԃɖ߂��܂��B
    //!
    //! �}�e���A���ɑ΂��čs���ꍇ�A�}�e���A���̃o�b�t�@�𐶐�����K�v������܂��B
    //! SceneBuilder �N���X�Ń��f�����쐬����ۂ̃I�v�V�����ŁA
    //! SceneBuilder::BufferOption ��ݒ肵�Ă��������B
    //! BufferOption �ɂ́A Model::MULTI_FLAG_ANIMATABLE_MATERIAL �̓��e���Œ���K�v�ł��B
    void Reset()
    {
        for (int i = (m_AnimGroup->GetMemberCount() - 1); i >= 0; --i)
        {
            this->ResetMember(i);
        }
    }

void
BaseAnimEvaluator::ResetMember(int memberIdx)
{
    NW_NULL_ASSERT(m_AnimGroup);

    if (!m_AnimGroup->HasOriginalValue())
    {
        return;
    }

    anim::ResGraphicsAnimGroup data = m_AnimGroup->GetResGraphicsAnimGroup();
    NW_ASSERT(data.IsValid());

    const anim::ResAnimGroupMember resAnimGroupMember = m_AnimGroup->GetResAnimGroupMember(memberIdx);
    resAnimGroupMember.SetValueForType(
        m_AnimGroup->GetTargetObject(memberIdx),
        m_AnimGroup->GetOriginalValue(memberIdx));
    
    m_IsCacheDirty = true;
}
#endif

#if 0
gfx_AnimGroup.cpp

void
AnimGroup::Reset()
{
    if (!this->HasOriginalValue())
    {
        return;
    }

    for ( int memberIdx = 0 ; memberIdx < this->GetMemberCount() ; ++memberIdx )
    {
        const anim::ResAnimGroupMember resAnimGroupMember = this->GetResAnimGroupMember(memberIdx);
        resAnimGroupMember.SetValueForType(
            this->GetTargetObject(memberIdx),
            this->GetOriginalValue(memberIdx));
    }
}
#endif

}
void AnimationManager::ResetSlot(u32 slot_index)
{
  if( slot_index < m_slot_num )
  { 
    Reset(m_slot_array[slot_index].GetType());
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}

// UseOwn�p
void AnimationManager::UseOwn_ChangeAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  u32                     slot_index,
  u32                     unit_index,
  ResAnim*                res_anim,
  u32                     change_frame,
  s32                     max_anim_members,
  b32                     force
)
{
  if( res_anim )
  {
    s32 max_members = 0;
    nw::gfx::AnimGroup* anim_group = AnimationManager::GetNwAnimGroupAndMemberCount( m_object, res_anim->GetType(), &max_members );

    /////////////////////////////////////
    // �A�j���[�V����
    /////////////////////////////////////
    if( change_frame == 0 )
    {
      // change_frame�����0�ɂ��Ă���ꍇ�A�s�K�v�ȃA�j���[�V����������Ȃ��Ă��ςނ悤�ɁA�C���f�b�N�X0�΂�����g�p����B
      // change_frame��1�x�ł�0����ɂ����ꍇ�A�C���f�b�N�X��0�ȊO�ɂȂ��Ă���̂ŁA�C���f�b�N�X0�΂�����g�p����Ƃ����킯�ɂ͂����Ȃ��Ȃ��Ă���B
      if( m_own_unit_animation_next_index_array[slot_index][unit_index] == 1 )
      {
        m_own_unit_animation_next_index_array[slot_index][unit_index] = 0;
      }
    }
    // �K�v�ȂƂ��ɂȂ��ď��߂ă��������m�ۂ���
    if( !m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ] )
    {
      m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ] = \
          GFL_NEW(heap_allocator->GetHeapBase()) Animation;
    }
    m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ]->ChangeAnimation(
      device_allocator,
      res_anim,
      max_members,
      max_anim_members
    );
    // �A�j���[�V�����̊֘A�t��
    b32 bind_result = m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ]->Bind(anim_group);
    // Animation���um_object��anim_group�v�̂ǂ���������Ȃ��Ƃ��Abind_result��false�ɂȂ�B
    // �A�j���[�V����A�����f��b�A���f��c�A���f��d�Ƀo�C���h����Ƃ��A���f��d�����A�j���[�V����A�œ������̂��Ȃ������Ƃ���ƁA
    // ���f��d�Ƀo�C���h�����Ƃ�����bind_result��false�ɂȂ�B
    // �����悤�Ȃ���̃}�b�v�ɁA���炩���߂����Ă������S�}�b�v���ʂ̃A�j���[�V�������o�C���h����Ƃ��ɁA���̂悤�Ȃ��Ƃ��N���邱�Ƃ�����B
    // ���̗�̃��f��b�A���f��c�A���f��d�͓����ɑ��݂���킯�ł͂Ȃ��B
    // ���f��b�A���f��c�A���f��d�������ɑ��݂��A����3�ґS�ĂɗB��̃A�j���[�V����A���o�C���h�����Ƃ��ǂ��Ȃ邩�͎����Ă��Ȃ��̂ŕs���B
    // �Ō�Ƀo�C���h�������f���������A�j���[�V�����͊o���Ă������Ȃ̂ŁA�A�j���[�V�����𕡐��̃��f���Ƀo�C���h�ł���Ƃ͎v���Ȃ��B
    //
    // ����Answer
    // gfx_AnimBlender.h
    // virtual Result TryBind(AnimGroup* animGroup)
    // ��
    // NW_NULL_ASSERT(animGroup);
    // NW_ASSERT(m_AnimGroup == NULL);
    // �Ƃ���̂ŁA
    // �A�j���[�V������NULL���o�C���h�ł��Ȃ����A���Ƀo�C���h����Ă�ꍇ�͐V�������̂��o�C���h�ł��Ȃ��悤�ɂȂ��Ă���B
    if( !force )  // force��true�̂Ƃ��́A��L�̂悤�ȏ󋵂ŃA�T�[�g�ɂ��Ȃ��B
    {
      GFL_ASSERT_MSG(bind_result, "�A�j���[�V�����̊֘A�t���Ɏ��s���܂����B\n");  //@check �o�C���h�Ɏ��s���Ă���������B
    }
 
    /////////////////////////////////////
    // �A�j���[�V�����̒P�ʂƂȂ����
    /////////////////////////////////////
    // �K�v�ȂƂ��ɂȂ��ď��߂ă��������m�ۂ���
    if( !m_own_unit_animation_array[slot_index][unit_index] )
    {
      m_own_unit_animation_array[slot_index][unit_index] = \
          GFL_NEW(heap_allocator->GetHeapBase()) UnitAnimation;
    }
    m_own_unit_animation_array[slot_index][unit_index]->ChangeAnimation(
        heap_allocator,
        device_allocator,
        m_own_unit_animation_num_max,
        m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ],
        change_frame
    );
    // �A�j���[�V�����̊֘A�t��
    m_own_unit_animation_array[slot_index][unit_index]->BindBlend(anim_group);
 
    /////////////////////////////////////
    // �X���b�g
    /////////////////////////////////////
    // �X���b�g�̃u�����h�������w�肵�Ă��Ȃ������́A�Ō��ChangeAnimation�Ŋ��蓖�Ă��A�j���[�V�������X���b�g�̃A�j���[�V�����Ƃ���B
    // ����āA�ǂ�unit_index�̃A�j���[�V���������蓖�Ă��Ă��邩�́A�ǂ�unit_index����ݒ肵�Ă��������Ɉ˂�B
    if( !m_own_set_slot_array[slot_index] )
    {
      // �܂��X���b�g�̃u�����h�������w�肵�Ă��Ȃ�
      m_slot_array[slot_index].ChangeAnimation(
          m_own_unit_animation_array[slot_index][unit_index]
      );
    }
    // �A�j���[�V�����̊֘A�t��
    m_slot_array[slot_index].BindBlend(anim_group);
 
    m_own_unit_animation_next_index_array[slot_index][unit_index] = (m_own_unit_animation_next_index_array[slot_index][unit_index]+1)%(m_own_unit_animation_num_over);
  }
  else
  {
    /////////////////////////////////////
    // �X���b�g
    /////////////////////////////////////
    if(
           m_slot_array[slot_index].GetState() != AnimationSlot::STATE_NONE
        && m_slot_array[slot_index].GetState() != AnimationSlot::STATE_ONE
    )
    {
      m_slot_array[slot_index].SetBlendAnimation(unit_index, NULL, 1.0f);
    }

    /////////////////////////////////////
    // �A�j���[�V�����̒P�ʂƂȂ����
    /////////////////////////////////////
    if( m_own_unit_animation_array[slot_index][unit_index] )
    {
      m_own_unit_animation_array[slot_index][unit_index]->ChangeAnimation(
        NULL,
        0  // change_frame=0�Ƃ������ɃA�j���[�V������j������
      );
    }

    /////////////////////////////////////
    // �A�j���[�V����
    /////////////////////////////////////
    for( s32 k=0; k<m_own_unit_animation_num_over; ++k )
    {
      if( m_own_animation_array[slot_index][unit_index][k] )
      {
        m_own_animation_array[slot_index][unit_index][k]->ChangeAnimation(
            NULL
        );
      }
    }

    m_own_unit_animation_next_index_array[slot_index][unit_index] = 0;
  }

  SetAnimObjectAllSlot(true);
}

void AnimationManager::UseOwn_BlendAnimation(
    gfl::heap::NwAllocator*    heap_allocator,
    gfl::heap::NwAllocator*    device_allocator,
    u32                        slot_index,
    AnimationSlot::Mode        mode,
    b32                        interpolator_ignore_no_anim_member,
    b32                        interpolator_normalization_enable,
    u32                        animation_unit_index
)
{
  switch( mode )
  {
  case AnimationSlot::MODE_NONE:
    {
      m_slot_array[slot_index].ChangeAnimation(
          NULL
      );
      m_own_set_slot_array[slot_index] = false;  // �X���b�g�̃u�����h�������w�肵�Ă��Ȃ���Ԃɖ߂�
    }
    break;
  case AnimationSlot::MODE_ANIMATION:
    {
      m_slot_array[slot_index].ChangeAnimation(
          m_own_unit_animation_array[slot_index][animation_unit_index]
      );
      m_own_set_slot_array[slot_index] = true;
    }
    break;
  case AnimationSlot::MODE_BLEND_INTERPOLATOR:
  case AnimationSlot::MODE_BLEND_ADDER:
    {
      // �^�C�v���ŏ��Ɍ��t�������̂ɂ��Ă���
      ResAnim::Type type = ResAnim::TYPE_NONE;
      for( s32 j=0; j<m_own_slot_blend_num_max; ++j )
      {
        if( m_own_unit_animation_array[slot_index][j] )
        {
          type = m_own_unit_animation_array[slot_index][j]->GetType();
          if( type != ResAnim::TYPE_NONE ) break;
        }
      }
      
      if( type != ResAnim::TYPE_NONE )
      {
        s32 max_members = 0;
        nw::gfx::AnimGroup* anim_group = AnimationManager::GetNwAnimGroupAndMemberCount( m_object, type, &max_members );

        switch(mode)
        {
        case AnimationSlot::MODE_BLEND_INTERPOLATOR:
          {
            m_slot_array[slot_index].ChangeBlendInterpolator(
              heap_allocator,
              device_allocator,
              type,
              m_own_slot_blend_num_max,
              0,
              NULL,
              interpolator_ignore_no_anim_member,
              interpolator_normalization_enable
            );
          }
          break;
        case AnimationSlot::MODE_BLEND_ADDER:
          {
            m_slot_array[slot_index].ChangeBlendAdder(
              heap_allocator,
              device_allocator,
              type,
              m_own_slot_blend_num_max,
              0,
              NULL
            );
          }
          break;
        }

        for( s32 j=0; j<m_own_slot_blend_num_max; ++j )
        {
          if( m_own_unit_animation_array[slot_index][j] )
          {
            m_slot_array[slot_index].SetBlendAnimation(j, m_own_unit_animation_array[slot_index][j], 1.0f);
          }
        }

        // �A�j���[�V�����̊֘A�t��
        m_slot_array[slot_index].BindBlend(anim_group);

        m_own_set_slot_array[slot_index] = true;
      }
      else
      {
        GFL_ASSERT_MSG(0, "�u�����h����A�j���[�V����������܂���B\n");
      }
    }
    break;
  }

  SetAnimObjectAllSlot(true);
}

// �t���[������
void AnimationManager::SetFrame(u32 slot_index, f32 frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}
f32  AnimationManager::GetFrame(u32 slot_index) const
{
  f32 frame = 0.0f;
  if( slot_index < m_slot_num )
  { 
    frame = m_slot_array[slot_index].GetFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return frame;
}
void AnimationManager::SetStepFrame(u32 slot_index, f32 step_frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetStepFrame(step_frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}
f32  AnimationManager::GetStepFrame(u32 slot_index) const
{
  f32 step_frame = 0.0f;
  if( slot_index < m_slot_num )
  { 
    step_frame = m_slot_array[slot_index].GetStepFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return step_frame;
}
void AnimationManager::SetStartFrame(u32 slot_index, f32 frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetStartFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}
f32  AnimationManager::GetStartFrame(u32 slot_index) const
{
  f32 frame = 0.0f;
  if( slot_index < m_slot_num )
  { 
    frame = m_slot_array[slot_index].GetStartFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return frame;
}
void AnimationManager::SetEndFrame(u32 slot_index, f32 frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetEndFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}
f32  AnimationManager::GetEndFrame(u32 slot_index) const
{
  f32 frame = 0.0f;
  if( slot_index < m_slot_num )
  { 
    frame = m_slot_array[slot_index].GetEndFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return frame;
}
void AnimationManager::SetStartAndEndFrame(u32 slot_index, f32 start_frame, f32 end_frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetStartAndEndFrame(start_frame, end_frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}
b32  AnimationManager::IsFrameStartFrame(u32 slot_index) const
{
  b32 ret = false;
  if( slot_index < m_slot_num )
  { 
    ret = m_slot_array[slot_index].IsFrameStartFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return ret;
}
b32  AnimationManager::IsFrameEndFrame(u32 slot_index) const
{
  b32 ret = false;
  if( slot_index < m_slot_num )
  { 
    ret = m_slot_array[slot_index].IsFrameEndFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return ret;
}
b32  AnimationManager::IsFrameStepTerminalFrame(u32 slot_index) const
{
  b32 ret = false;
  if( slot_index < m_slot_num )
  { 
    ret = m_slot_array[slot_index].IsFrameStepTerminalFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return ret;
}
b32 AnimationManager::IsLoop(u32 slot_index) const
{
  b32 ret = false;
  if( slot_index < m_slot_num )
  { 
    ret = m_slot_array[slot_index].IsLoop();
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
  return ret;
}
void AnimationManager::SetLoop(u32 slot_index, b32 loop)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetLoop(loop);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%d��m_slot_num=%d�����ł͂���܂���B\n", slot_index, m_slot_num);
  }
}

// UseOwn�p
void AnimationManager::UseOwn_SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetFrame(frame);
  }
}
f32  AnimationManager::UseOwn_GetUnitFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->GetFrame();
  }
  else
  {
    return 0.0f;
  }
}
void AnimationManager::UseOwn_SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetStepFrame(step_frame);
  }
}
f32  AnimationManager::UseOwn_GetUnitStepFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->GetStepFrame();
  }
  else
  {
    return 0.0f;
  }
}
void AnimationManager::UseOwn_SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetStartFrame(frame);
  }
}
f32  AnimationManager::UseOwn_GetUnitStartFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->GetStartFrame();
  }
  else
  {
    return 0.0f;
  }
}
void AnimationManager::UseOwn_SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetEndFrame(frame);
  }
}
f32  AnimationManager::UseOwn_GetUnitEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->GetEndFrame();
  }
  else
  {
    return 0.0f;
  }
}
void AnimationManager::UseOwn_SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetStartAndEndFrame(start_frame, end_frame);
  }
}
b32  AnimationManager::UseOwn_IsUnitFrameStartFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->IsFrameStartFrame();
  }
  else
  {
    return false;
  }
}
b32  AnimationManager::UseOwn_IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->IsFrameEndFrame();
  }
  else
  {
    return false;
  }
}
b32  AnimationManager::UseOwn_IsUnitFrameStepTerminalFrame(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->IsFrameStepTerminalFrame();
  }
  else
  {
    return false;
  }
}
b32  AnimationManager::UseOwn_IsUnitLoop(u32 slot_index, u32 unit_index) const
{
  if( m_use_own_animation )
  {
    return m_own_unit_animation_array[slot_index][unit_index]->IsLoop();
  }
  else
  {
    return false;
  }
}
void AnimationManager::UseOwn_SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop)
{
  if( m_use_own_animation )
  {
    m_own_unit_animation_array[slot_index][unit_index]->SetLoop(loop);
  }
}

// �u�����h����
void AnimationManager::SetBlendWeight(u32 slot_index, u32 unit_index, f32 weight)
{
  m_slot_array[slot_index].SetBlendWeight(unit_index, weight);
}
f32  AnimationManager::GetBlendWeight(u32 slot_index, u32 unit_index)
{
  return m_slot_array[slot_index].GetBlendWeight(unit_index);
}

void AnimationManager::SetAnimObjectAllSlot(b32 is_not_null)
{
  if( m_object )
  {
    ObjectType object_type = m_object->GetObjectType();
    switch( object_type )
    {
    case OBJECT_TYPE_MODEL:
      {
        nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>((reinterpret_cast<Model*>(m_object))->GetNwModel());
        if( skeletal_model ) 
        {
          for( u32 i=0; i<m_slot_num; ++i )
          {
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
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_SKELETAL:
              {
                skeletal_model->SetSkeletalAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            case ResAnim::TYPE_MATERIAL:
              {
                skeletal_model->SetMaterialAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            case ResAnim::TYPE_VISIBILITY:
              {
                skeletal_model->SetVisibilityAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            }
          }
        }
        else
        {
          nw::gfx::Model* model = nw::ut::DynamicCast<nw::gfx::Model*>((reinterpret_cast<Model*>(m_object))->GetNwModel());
          if( model )
          {
            for( u32 i=0; i<m_slot_num; ++i )
            {
              switch( m_slot_array[i].GetType() )
              {
              case ResAnim::TYPE_MATERIAL:
                {
                  model->SetMaterialAnimObject(
                      ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                      m_slot_array[i].GetAnimObjectIndex()
                  );
                }
                break;
              case ResAnim::TYPE_VISIBILITY:
                {
                  model->SetVisibilityAnimObject(
                      ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                      m_slot_array[i].GetAnimObjectIndex()
                  );
                }
                break;
              }
            }
          }
        }
      }
      break;
    case OBJECT_TYPE_CAMERA:
    case OBJECT_TYPE_STEREO_CAMERA:
      {
        nw::gfx::Camera* camera = nw::ut::DynamicCast<nw::gfx::Camera*>((reinterpret_cast<Camera*>(m_object))->GetNwCamera());
        if( camera )
        {
          for( u32 i=0; i<m_slot_num; ++i )
          {
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_CAMERA:  // �J�����ƃ��C�g�̓X���b�g�����Ӗ����Ȃ�
              {
                camera->SetAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL
                );
              }
              break;
            }
          }
        }
      }
      break;
    case OBJECT_TYPE_LIGHT:
      {
        nw::gfx::Light* light = nw::ut::DynamicCast<nw::gfx::Light*>((reinterpret_cast<Light*>(m_object))->GetNwLight());
        if( light )
        {
          for( u32 i=0; i<m_slot_num; ++i )
          {
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_LIGHT:  // �J�����ƃ��C�g�̓X���b�g�����Ӗ����Ȃ�
              {
                light->SetAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL
                );
              }
              break;
            }
          }
        }
      }
      break;
    default:
      {
        GFL_ASSERT_MSG(0, "�ێ����Ă���I�u�W�F�N�g�̎��%d���ُ�ł��B", object_type);
      }
      break;
    }
  }
}

void AnimationManager::SetAnimObject(u32 slot_index, b32 is_not_null)
{
  if( m_object )
  {
    ObjectType object_type = m_object->GetObjectType();
    switch( object_type )
    {
    case OBJECT_TYPE_MODEL:
      {
        nw::gfx::SkeletalModel* skeletal_model = nw::ut::DynamicCast<nw::gfx::SkeletalModel*>((reinterpret_cast<Model*>(m_object))->GetNwModel());
        if( skeletal_model ) 
        {
          u32 i = slot_index;
          {
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
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_SKELETAL:
              {
                skeletal_model->SetSkeletalAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            case ResAnim::TYPE_MATERIAL:
              {
                skeletal_model->SetMaterialAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            case ResAnim::TYPE_VISIBILITY:
              {
                skeletal_model->SetVisibilityAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                    m_slot_array[i].GetAnimObjectIndex()
                );
              }
              break;
            }
          }
        }
        else
        {
          nw::gfx::Model* model = nw::ut::DynamicCast<nw::gfx::Model*>((reinterpret_cast<Model*>(m_object))->GetNwModel());
          if( model )
          {
            u32 i = slot_index;
            {
              switch( m_slot_array[i].GetType() )
              {
              case ResAnim::TYPE_MATERIAL:
                {
                  model->SetMaterialAnimObject(
                      ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                      m_slot_array[i].GetAnimObjectIndex()
                  );
                }
                break;
              case ResAnim::TYPE_VISIBILITY:
                {
                  model->SetVisibilityAnimObject(
                      ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL,
                      m_slot_array[i].GetAnimObjectIndex()
                  );
                }
                break;
              }
            }
          }
        }
      }
      break;
    case OBJECT_TYPE_CAMERA:
    case OBJECT_TYPE_STEREO_CAMERA:
      {
        nw::gfx::Camera* camera = nw::ut::DynamicCast<nw::gfx::Camera*>((reinterpret_cast<Camera*>(m_object))->GetNwCamera());
        if( camera )
        {
          u32 i = slot_index;
          {
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_CAMERA:  // �J�����ƃ��C�g�̓X���b�g�����Ӗ����Ȃ�
              {
                camera->SetAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL
                );
              }
              break;
            }
          }
        }
      }
      break;
    case OBJECT_TYPE_LIGHT:
      {
        nw::gfx::Light* light = nw::ut::DynamicCast<nw::gfx::Light*>((reinterpret_cast<Light*>(m_object))->GetNwLight());
        if( light )
        {
          u32 i = slot_index;
          {
            switch( m_slot_array[i].GetType() )
            {
            case ResAnim::TYPE_LIGHT:  // �J�����ƃ��C�g�̓X���b�g�����Ӗ����Ȃ�
              {
                light->SetAnimObject(
                    ((is_not_null)&&(m_slot_array[i].IsEnable()))?(m_slot_array[i].GetNwAnimObject()):NULL
                );
              }
              break;
            }
          }
        }
      }
      break;
    default:
      {
        GFL_ASSERT_MSG(0, "�ێ����Ă���I�u�W�F�N�g�̎��%d���ُ�ł��B", object_type);
      }
      break;
    }
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

