//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimation.cpp
 *	@brief  3D�A�j���[�V����
 *	@author	Koji Kawada
 *	@date		2010.10.12
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
#include <gfl_debug.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

#include <grp/g3d/gfl_G3dAnimation.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					�N���X��`
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D�A�j���[�V�����N���X
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         �R���X�g���N�^
 */
//-----------------------------------------------------------------------------
Animation::Animation(void)
  : m_state(STATE_NONE),
    m_type(ResAnim::TYPE_NONE),
    m_max_members(0),
    m_max_anim_members(0),
    m_blend_num_max(0),
    m_blend_animation_array(NULL),
    m_anim_object(NULL),
    m_bind_anim_group(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetAnimationManager()->AddObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �f�X�g���N�^
 */
//-----------------------------------------------------------------------------
Animation::~Animation()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetAnimationManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����𐶐�����
 */
//-----------------------------------------------------------------------------
void Animation::CreateAnimation(
  gfl::heap::NwAllocator*  device_allocator,
  ResAnim*                 res_anim,
  s32                      max_members,
  s32                      max_anim_members
)
{
  if( m_state == STATE_NONE )
  {
    ResAnim::Type type = res_anim->GetType();
    switch( type )
    {
    case ResAnim::TYPE_SKELETAL:
      {
        m_state = STATE_EVALUATOR_SKELETAL;
        m_type  = type;
      }
      break;
    case ResAnim::TYPE_MATERIAL:
    case ResAnim::TYPE_VISIBILITY:
    case ResAnim::TYPE_CAMERA:
    case ResAnim::TYPE_LIGHT:
      {  
        m_state = STATE_EVALUATOR_OTHERS;
        m_type  = type;
      }
      break;
    default:
      {
        GFL_ASSERT_MSG( 0, "res_anim�̃A�j���[�V�������\�[�X�^�C�v%d���s���ł��B\n", type );
        m_state = STATE_NONE;
        m_type  = ResAnim::TYPE_NONE;
      }
      break;
    }

    nw::anim::ResAnim res;
    switch( m_state )
    {
    case STATE_EVALUATOR_SKELETAL:
    case STATE_EVALUATOR_OTHERS:
      {
        res = res_anim->GetNwResAnim();
        m_max_members = max_members;
        m_max_anim_members = Animation::GetMaxAnimMembers(res_anim, max_members, max_anim_members);
      }
      break;
    }

    switch( m_state )
    {
    case STATE_EVALUATOR_SKELETAL:
      {
        // ResAnim ����AAnimEvaluator::Builder ��p���� AnimEvaluator �𐶐����܂��B
        // �X�P���^���A�j���[�V�����̏ꍇ�́ATransformAnimEvaluator::Builder ���g�p���܂��B
        //
        // �A�j���[�V������1�̃��f���ɂ̂ݓK�p����ꍇ��A
        // �R�}�`���f�[�^�̏ꍇ�́A AllocCache �� false �ɂ���Ə������ׂ�������܂��B
        nw::gfx::TransformAnimEvaluator* evaluator = nw::gfx::TransformAnimEvaluator::Builder()
            .AnimData(res)
            .MaxMembers(m_max_members)
            .MaxAnimMembers(m_max_anim_members)
            .AllocCache(false)
            .Create(device_allocator);
       
        GFL_ASSERT_MSG( evaluator, "nw::gfx::TransformAnimEvaluator�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
        m_anim_object = evaluator;
      }
      break;
    case STATE_EVALUATOR_OTHERS:
      {
        // �A�j���[�V�����I�u�W�F�N�g�𐶐����܂��B
        nw::gfx::AnimEvaluator* evaluator = nw::gfx::AnimEvaluator::Builder()
            .AnimData(res)
            .MaxMembers(m_max_members)
            .MaxAnimMembers(m_max_anim_members)
            .AllocCache(false)
            .Create(device_allocator);
        
        GFL_ASSERT_MSG( evaluator, "nw::gfx::AnimEvaluator�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
        m_anim_object = evaluator;
      }
      break;
    }

    if( m_anim_object )
    {
      SetResourceIdAndIndexInResource(res_anim->GetResourceId(), res_anim->GetIndexInResource());  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B
    }
    else
    {
      m_state = STATE_NONE;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���ɑ��̃A�j���[�V��������������Ă��܂��B\n");
  }
}

void Animation::ChangeAnimation(
  gfl::heap::NwAllocator*  device_allocator,
  ResAnim*                 res_anim,
  s32                      max_members,
  s32                      max_anim_members
)
{
  if( m_state == STATE_NONE )
  {
    CreateAnimation(device_allocator, res_anim, max_members, max_anim_members);
  }
  else
  {
    b32 can_change = false;

    switch( m_state )
    {
    case STATE_EVALUATOR_SKELETAL:
    case STATE_EVALUATOR_OTHERS:
      {
        s32 l_max_anim_members = Animation::GetMaxAnimMembers(res_anim, max_members, max_anim_members);

        ResAnim::Type type = res_anim->GetType();
        if(
               m_type == type
            && m_max_members >= max_members
            && m_max_anim_members >= l_max_anim_members
        )
        {
          can_change = true;
        }
      }
      break;
    }
    
    if( can_change )
    {
      ChangeAnimation(res_anim);
    }
    else
    {
      Destroy();
      CreateAnimation(device_allocator, res_anim, max_members, max_anim_members);
    }
  }
}

void Animation::CreateAnimation(
  gfl::heap::NwAllocator*          device_allocator,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  s32                              max_members,
  s32                              max_anim_members
)
{
  ResAnim res_anim;
  resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
  CreateAnimation(device_allocator, &res_anim, max_members, max_anim_members);
}

void Animation::ChangeAnimation(
  gfl::heap::NwAllocator*          device_allocator,
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource,
  s32                              max_members,
  s32                              max_anim_members
)
{
  ResAnim res_anim;
  resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
  ChangeAnimation(device_allocator, &res_anim, max_members, max_anim_members);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��ԃu�����h�A�j���[�V�����𐶐�����
 *  @brief         ���Z�u�����h�A�j���[�V�����𐶐�����
 */
//-----------------------------------------------------------------------------
void Animation::CreateBlendInterpolator(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max,
  s32                     num,
  AnimationPack*          pack_array,
  b32                     ignore_no_anim_member,
  b32                     normalization_enable
)
{
  if( m_state == STATE_NONE )
  {
    if( num_max > 0 )
    {
      switch( type )
      {
      case ResAnim::TYPE_SKELETAL:
        {
          m_state = STATE_BLEND_INTERPOLATOR_SKELETAL;
          m_type  = type;
        }
        break;
      case ResAnim::TYPE_MATERIAL:
      case ResAnim::TYPE_VISIBILITY:
      case ResAnim::TYPE_CAMERA:
      case ResAnim::TYPE_LIGHT:
        {  
          m_state = STATE_BLEND_INTERPOLATOR_OTHERS;
          m_type  = type;
        }
        break;
      default:
        {
          GFL_ASSERT_MSG( 0, "�A�j���[�V�������\�[�X�^�C�v%d���s���ł��B\n", type );
          m_state = STATE_NONE;
          m_type  = ResAnim::TYPE_NONE;
        }
        break;
      }

      nw::gfx::AnimInterpolator* interpolator = NULL;
      switch( m_state )
      {
      case STATE_BLEND_INTERPOLATOR_SKELETAL:
        {
          // �X�P���^���A�j���[�V������Ԃ𐶐����܂��B
          nw::gfx::TransformAnimInterpolator* blender = nw::gfx::TransformAnimInterpolator::Builder()
              .IgnoreNoAnimMember(ignore_no_anim_member)
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::TransformAnimInterpolator�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
          interpolator = blender;
        }
        break;
      case STATE_BLEND_INTERPOLATOR_OTHERS:
        {
          // �A�j���[�V������Ԃ𐶐����܂��B
          nw::gfx::AnimInterpolator* blender = nw::gfx::AnimInterpolator::Builder()
              .IgnoreNoAnimMember(ignore_no_anim_member)
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::AnimInterpolator�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
          interpolator = blender;
        }
        break;
      }

      if( interpolator )
      {
        m_blend_num_max = num_max;
        m_blend_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Animation*[m_blend_num_max];

        interpolator->SetNormalizationEnabled(normalization_enable);  
        for( s32 i=0; i<m_blend_num_max; ++i )
        {
          interpolator->AddAnimObject(NULL);
              // NULL ���w�肵�Ă����āA��� ReplaceAnimObject() �Œu�������邱�Ƃ��\�ł��B
          interpolator->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        for( s32 i=0; i<num; ++i )
        {
          if( pack_array[i].animation )
          {
            if( m_type == pack_array[i].animation->GetType() )  // �A�j���[�V�������\�[�X�̃^�C�v����v������̂����ǉ�����
            {
              interpolator->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
                  // NULL ���w�肵���ꍇ�A�A�j���[�V�����I�u�W�F�N�g���Ȃ���ԂɂȂ�܂��B
              interpolator->SetWeight(i, pack_array[i].weight);
              m_blend_animation_array[i] = pack_array[i].animation;
            }
            else
            {
              GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check �ǉ�����Ȃ������B
                  "�A�j���[�V�������\�[�X�̃^�C�v���قȂ�܂��B�S�̂̃^�C�v%d, pack_array[%d]�̃^�C�v%d\n",
                  m_type, i, pack_array[i].animation->GetType());
            }
          }
        }

        m_anim_object = interpolator;
        SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B
      }
      else
      {
        m_state = STATE_NONE;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "�u�����h����A�j���[�V�����̍ő吔��%d�ł��B\n", num_max);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���ɑ��̃A�j���[�V��������������Ă��܂��B\n");
  }
}

void Animation::ChangeBlendInterpolator(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max,
  s32                     num,
  AnimationPack*          pack_array,
  b32                     ignore_no_anim_member,
  b32                     normalization_enable
)
{
  if( m_state == STATE_NONE )
  {
    CreateBlendInterpolator(
      heap_allocator,
      device_allocator,
      type,
      num_max,
      num,
      pack_array,
      ignore_no_anim_member,
      normalization_enable
    );
  }
  else
  {
    b32 can_change = false;

    switch( m_state )
    {
    case STATE_BLEND_INTERPOLATOR_SKELETAL:
    case STATE_BLEND_INTERPOLATOR_OTHERS:
      {
        if(
               m_type == type
            && m_blend_num_max >= num_max
        )
        {
          can_change = true;
        }
      }
      break;
    }
    
    if( can_change )
    {
      ChangeBlendInterpolator(num, pack_array, normalization_enable);
    }
    else
    {
      Destroy();
      CreateBlendInterpolator(
        heap_allocator,
        device_allocator,
        type,
        num_max,
        num,
        pack_array,
        ignore_no_anim_member,
        normalization_enable
      );
    }
  }
}

void Animation::CreateBlendAdder(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max,
  s32                     num,
  AnimationPack*          pack_array
)
{
  if( m_state == STATE_NONE )
  {
    if( num_max > 0 )
    {
      switch( type )
      {
      case ResAnim::TYPE_SKELETAL:
        {
          m_state = STATE_BLEND_ADDER_SKELETAL;
          m_type  = type;
        }
        break;
      case ResAnim::TYPE_MATERIAL:
      case ResAnim::TYPE_VISIBILITY:
      case ResAnim::TYPE_CAMERA:
      case ResAnim::TYPE_LIGHT:
        {  
          m_state = STATE_BLEND_ADDER_OTHERS;
          m_type  = type;
        }
        break;
      default:
        {
          GFL_ASSERT_MSG( 0, "�A�j���[�V�������\�[�X�^�C�v%d���s���ł��B\n", type );
          m_state = STATE_NONE;
          m_type  = ResAnim::TYPE_NONE;
        }
        break;
      }

      nw::gfx::AnimAdder* adder = NULL;
      switch( m_state )
      {
      case STATE_BLEND_ADDER_SKELETAL:
        {
          // �X�P���^���A�j���[�V�������Z�𐶐����܂��B
          nw::gfx::TransformAnimAdder* blender = nw::gfx::TransformAnimAdder::Builder()
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::TransformAnimAdder�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
          adder = blender;
        }
        break;
      case STATE_BLEND_ADDER_OTHERS:
        {
          // �A�j���[�V�������Z�𐶐����܂��B
          nw::gfx::AnimAdder* blender = nw::gfx::AnimAdder::Builder()
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::AnimAdder�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
          adder = blender;
        }
        break;
      }

      if( adder )
      {
        m_blend_num_max = num_max;
        m_blend_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) Animation*[m_blend_num_max];

        for( s32 i=0; i<m_blend_num_max; ++i )
        {
          adder->AddAnimObject(NULL);
              // NULL ���w�肵�Ă����āA��� ReplaceAnimObject() �Œu�������邱�Ƃ��\�ł��B
          adder->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        for( s32 i=0; i<num; ++i )
        {
          if( pack_array[i].animation )
          {
            if( m_type == pack_array[i].animation->GetType() )  // �A�j���[�V�������\�[�X�̃^�C�v����v������̂����ǉ�����
            {
              adder->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
                  // NULL ���w�肵���ꍇ�A�A�j���[�V�����I�u�W�F�N�g���Ȃ���ԂɂȂ�܂��B
              adder->SetWeight(i, pack_array[i].weight);
              m_blend_animation_array[i] = pack_array[i].animation;
            }
            else
            {
              GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check �ǉ�����Ȃ������B
                  "�A�j���[�V�������\�[�X�̃^�C�v���قȂ�܂��B�S�̂̃^�C�v%d, pack_array[%d]�̃^�C�v%d\n",
                  m_type, i, pack_array[i].animation->GetType());
            }
          }
        }

        m_anim_object = adder;
        SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B
      }
      else
      {
        m_state = STATE_NONE;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "�u�����h����A�j���[�V�����̍ő吔��%d�ł��B\n", num_max);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���ɑ��̃A�j���[�V��������������Ă��܂��B\n");
  }
}

void Animation::ChangeBlendAdder(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max,
  s32                     num,
  AnimationPack*          pack_array
)
{
  if( m_state == STATE_NONE )
  {
    CreateBlendAdder(
      heap_allocator,
      device_allocator,
      type,
      num_max,
      num,
      pack_array
    );
  }
  else
  {
    b32 can_change = false;

    switch( m_state )
    {
    case STATE_BLEND_ADDER_SKELETAL:
    case STATE_BLEND_ADDER_OTHERS:
      {
        if(
               m_type == type
            && m_blend_num_max >= num_max
        )
        {
          can_change = true;
        }
      }
      break;
    }
    
    if( can_change )
    {
      ChangeBlendAdder(num, pack_array);
    }
    else
    {
      Destroy();
      CreateBlendAdder(
        heap_allocator,
        device_allocator,
        type,
        num_max,
        num,
        pack_array
      );
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V������j������
 */
//-----------------------------------------------------------------------------
void Animation::Destroy(void)
{
  SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B

  if( m_anim_object )
  {
    if( m_bind_anim_group )
    {
      m_anim_object->Release();
      //�A�j���[�V�����̎��O��
      //AnimObject::Release �ŃA�j���[�V�����]������A�j���[�V�����O���[�v��������܂��B
      //�A�j���[�V�����̍Đ��ŏq�ׂ� SetAnimObject �� Set***AnimObject �� NULL ��n���āA�A�j���[�V�������������܂��B
      //�ǂ���̏������K�v�ł��̂ŁA���ӂ��Ă��������B
      // @note ���̌��Calculate�ł�����Ə��������Ǝv�����A���O���Y�ꂪ�Ȃ����`�F�b�N����悤�ɂ�����
      m_bind_anim_group = NULL;
    }
    nw::gfx::SafeDestroy(m_anim_object);  // NULL�������Ă����
  }

  if( m_blend_animation_array )
  {
    GFL_DELETE_ARRAY m_blend_animation_array;
    m_blend_animation_array = NULL;
  }

  m_state = STATE_NONE;
  m_type  = ResAnim::TYPE_NONE;
  m_max_members = 0;
  m_max_anim_members = 0;
  m_blend_num_max = 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V������ύX����
 */
//-----------------------------------------------------------------------------
void Animation::ChangeAnimation(
  ResAnim*                 res_anim
)
{
  if( res_anim )
  {
    b32 can_change = false;
    switch( m_state )
    {
    case STATE_EVALUATOR_SKELETAL:
    case STATE_EVALUATOR_OTHERS:
      {
        ResAnim::Type type = res_anim->GetType();
        if(
               m_type == type
        )
        {
          can_change = true;
        }
      }
      break;
    }
 
    if( can_change )
    {
      nw::anim::ResAnim res = res_anim->GetNwResAnim();
 
      // �A�j���[�V������ύX����
      switch( m_state )
      {
      case STATE_EVALUATOR_SKELETAL:
        {
          nw::gfx::TransformAnimEvaluator* evaluator =
              nw::ut::DynamicCast<nw::gfx::TransformAnimEvaluator*>(m_anim_object);
          evaluator->ChangeAnim(res);
        }
        break;
      case STATE_EVALUATOR_OTHERS:
        {
          nw::gfx::AnimEvaluator* evaluator =
              nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(m_anim_object);
          evaluator->ChangeAnim(res);
        }
        break;
      }
      
      SetResourceIdAndIndexInResource(res_anim->GetResourceId(), res_anim->GetIndexInResource());  // �A�j���[�V�������\�[�X�̃^�C�v��Object�ł͂Ȃ����̃N���X�Ŋo���Ă��邾���B
    }
    else
    {
      GFL_ASSERT_MSG(0, "�A�j���[�V������ύX�ł��܂���B\n");
    }
  }
  else
  {
    Destroy();
  }
}
void Animation::ChangeAnimation(
  Resource*                        resource,
  Resource::AnimationResourceType  animation_resource_type,
  s32                              index_in_resource
)
{
  ResAnim res_anim;
  resource->GetResAnim(animation_resource_type, index_in_resource, &res_anim);
  ChangeAnimation(&res_anim);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��ԃu�����h�A�j���[�V������ύX����
 *  @brief         ���Z�u�����h�A�j���[�V������ύX����
 */
//-----------------------------------------------------------------------------
void Animation::ChangeBlendInterpolator(
  s32                     num,
  AnimationPack*          pack_array,
  b32                     normalization_enable
)
{
  b32 can_change = false;
  switch( m_state )
  {
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      if(
          m_blend_num_max >= num
      )
      {
        can_change = true;
      }
    }
    break;
  }
  
  if( can_change )
  {
    nw::gfx::AnimInterpolator* interpolator = NULL;

    // �u�����h��ύX����
    switch( m_state )
    {
    case STATE_BLEND_INTERPOLATOR_SKELETAL:
      {
        nw::gfx::TransformAnimInterpolator* blender = 
            nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
        interpolator = blender;
      }
      break;
    case STATE_BLEND_INTERPOLATOR_OTHERS:
      {
        nw::gfx::AnimInterpolator* blender = 
            nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
        interpolator = blender;
      }
      break;
    }

    if( !interpolator )
    {
      GFL_ASSERT_MSG(0, "interpolator=NULL\n");
      return;
    }

    interpolator->SetNormalizationEnabled(normalization_enable);  
    for( s32 i=0; i<m_blend_num_max; ++i )
    {
      interpolator->ReplaceAnimObject(i, NULL);
      interpolator->SetWeight(i, 1.0f);
      m_blend_animation_array[i] = NULL;
    }

    for( s32 i=0; i<num; ++i )
    {
      if( pack_array[i].animation )
      {
        if( m_type == pack_array[i].animation->GetType() )  // �A�j���[�V�������\�[�X�̃^�C�v����v������̂����ǉ�����
        {
          interpolator->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
              // NULL ���w�肵���ꍇ�A�A�j���[�V�����I�u�W�F�N�g���Ȃ���ԂɂȂ�܂��B
          interpolator->SetWeight(i, pack_array[i].weight);
          m_blend_animation_array[i] = pack_array[i].animation;
        }
        else
        {
          GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check �ǉ�����Ȃ������B
              "�A�j���[�V�������\�[�X�̃^�C�v���قȂ�܂��B�S�̂̃^�C�v%d, pack_array[%d]�̃^�C�v%d\n",
              m_type, i, pack_array[i].animation->GetType());
        }
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "��ԃu�����h�A�j���[�V������ύX�ł��܂���B\n");
  }
}
void Animation::ChangeBlendAdder(
  s32                     num,
  AnimationPack*          pack_array
)
{
  b32 can_change = false;
  switch( m_state )
  {
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      if(
          m_blend_num_max >= num
      )
      {
        can_change = true;
      }
    }
    break;
  }
  
  if( can_change )
  {
    nw::gfx::AnimAdder* adder = NULL;

    // �u�����h��ύX����
    switch( m_state )
    {
    case STATE_BLEND_ADDER_SKELETAL:
      {
        nw::gfx::TransformAnimAdder* blender = 
            nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_object);
        adder = blender;
      }
      break;
    case STATE_BLEND_ADDER_OTHERS:
      {
        nw::gfx::AnimAdder* blender = 
            nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_object);
        adder = blender;
      }
      break;
    }

    if( !adder )
    {
      GFL_ASSERT_MSG(0, "adder=NULL\n");
      return;
    }

    for( s32 i=0; i<m_blend_num_max; ++i )
    {
      adder->ReplaceAnimObject(i, NULL);
      adder->SetWeight(i, 1.0f);
      m_blend_animation_array[i] = NULL;
    }

    for( s32 i=0; i<num; ++i )
    {
      if( pack_array[i].animation )
      {
        if( m_type == pack_array[i].animation->GetType() )  // �A�j���[�V�������\�[�X�̃^�C�v����v������̂����ǉ�����
        {
          adder->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
              // NULL ���w�肵���ꍇ�A�A�j���[�V�����I�u�W�F�N�g���Ȃ���ԂɂȂ�܂��B
          adder->SetWeight(i, pack_array[i].weight);
          m_blend_animation_array[i] = pack_array[i].animation;
        }
        else
        {
          GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check �ǉ�����Ȃ������B
              "�A�j���[�V�������\�[�X�̃^�C�v���قȂ�܂��B�S�̂̃^�C�v%d, pack_array[%d]�̃^�C�v%d\n",
              m_type, i, pack_array[i].animation->GetType());
        }
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���Z�u�����h�A�j���[�V������ύX�ł��܂���B\n");
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[����ݒ肷��
 *
 *  @param[in]     frame      �t���[��
 */
//-----------------------------------------------------------------------------
void Animation::SetFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
    {
      nw::gfx::TransformAnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::TransformAnimEvaluator*>(m_anim_object);
      evaluator->SetFrame(frame);
    }
    break;
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::AnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(m_anim_object);
      evaluator->SetFrame(frame);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Animation::GetFrame(void) const
{
  f32 ret = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
    {
      nw::gfx::TransformAnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::TransformAnimEvaluator*>(m_anim_object);
      ret = evaluator->GetFrame();
    }
    break;
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::AnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(m_anim_object);
      ret = evaluator->GetFrame();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[����ݒ肷��
 *                 0.0f�Œ�~�A1.0f�œ��{�A2.0f��2�{���A-1.0f�ŋt���{�Đ�
 *
 *  @param[in]     step_frame      �X�V�t���[��
 */
//-----------------------------------------------------------------------------
void Animation::SetStepFrame(f32 step_frame)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
    {
      nw::gfx::TransformAnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::TransformAnimEvaluator*>(m_anim_object);
      evaluator->SetStepFrame(step_frame);
    }
    break;
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::AnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(m_anim_object);
      evaluator->SetStepFrame(step_frame);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̍X�V�t���[�����擾����
 *
 *  @retval        �X�V�t���[��
 */
//-----------------------------------------------------------------------------
f32 Animation::GetStepFrame(void) const
{
  f32 ret = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
    {
      nw::gfx::TransformAnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::TransformAnimEvaluator*>(m_anim_object);
      ret = evaluator->GetStepFrame();
    }
    break;
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::AnimEvaluator* evaluator =
          nw::ut::DynamicCast<nw::gfx::AnimEvaluator*>(m_anim_object);
      ret = evaluator->GetStepFrame();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[����ݒ肷��
 *
 *  @param[in]     frame      �t���[��
 */
//-----------------------------------------------------------------------------
void Animation::SetStartFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      evaluator->SetStartFrame(frame);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Animation::GetStartFrame(void) const
{
  f32 frame = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      frame = evaluator->GetStartFrame();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[����ݒ肷��
 *
 *  @param[in]     frame      �t���[��
 */
//-----------------------------------------------------------------------------
void Animation::SetEndFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      evaluator->SetEndFrame(frame);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̏I���t���[�����擾����
 *
 *  @retval        �t���[��
 */
//-----------------------------------------------------------------------------
f32 Animation::GetEndFrame(void) const
{
  f32 frame = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      frame = evaluator->GetEndFrame();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̊J�n�t���[���ƏI���t���[����ݒ肷��
 *
 *  @param[in]     start_frame    �J�n�t���[��
 *  @param[in]     end_frame      �I���t���[��
 */
//-----------------------------------------------------------------------------
void Animation::SetStartAndEndFrame(f32 start_frame, f32 end_frame)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      f32 curr_start_frame = evaluator->GetStartFrame();
      f32 curr_end_frame   = evaluator->GetEndFrame();

      if( start_frame <= curr_end_frame )
      {
        evaluator->SetStartFrame(start_frame);
        evaluator->SetEndFrame(end_frame);
      }
      else
      {
        evaluator->SetEndFrame(end_frame);
        evaluator->SetStartFrame(start_frame);
      }
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����J�n�t���[�����ۂ����肷��
 *
 *  @retval        b32    ���݂̃t���[�����J�n�t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
b32 Animation::IsFrameStartFrame(void) const
{
  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      ret = ( evaluator->GetFrame() == evaluator->GetStartFrame() );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[�����I���t���[�����ۂ����肷��
 *
 *  @retval        b32    ���݂̃t���[�����I���t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
b32 Animation::IsFrameEndFrame(void) const
{
  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      ret = ( evaluator->GetFrame() == evaluator->GetEndFrame() );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̌��݂̃t���[����step_frame���猩�Ē[�t���[�����ۂ����肷��
 *
 *  @retval        bool    ���݂̃t���[����step_frame���猩�Ē[�t���[���̏ꍇture��Ԃ�
 */
//-----------------------------------------------------------------------------
b32 Animation::IsFrameStepTerminalFrame(void) const
{
  b32 ret = false;
  f32 step_frame = GetStepFrame();
  if( step_frame < 0.0f )
  {
    ret = ( GetFrame() == GetStartFrame() );
  }
  else if( step_frame > 0.0f )
  {
    ret = ( GetFrame() == GetEndFrame() );
  }
  else
  {
    ret = ( GetFrame() == GetStartFrame() || GetFrame() == GetEndFrame() );
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V���������[�v�Đ����ۂ��擾����
 *
 *  @retval        b32       ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��Ԃ�
 */
//-----------------------------------------------------------------------------
b32 Animation::IsLoop(void) const
{
  b32 loop = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      nw::anim::AnimFrameController::PlayPolicy play_policy = evaluator->GetPlayPolicy();
      if( play_policy == nw::anim::PlayPolicy_Loop ) loop = true;
      else                                           loop = false;
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߎ擾�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return loop;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����Ƀ��[�v�Đ���ݒ肷��
 *
 *  @param[in]     loop    ���[�v�̂Ƃ�true�A���[�v�łȂ��Ƃ�false��ݒ肷��
 */
//-----------------------------------------------------------------------------
void Animation::SetLoop(b32 loop)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      nw::gfx::BaseAnimEvaluator* evaluator = nw::ut::DynamicCast<nw::gfx::BaseAnimEvaluator*>(m_anim_object);
      if(loop) evaluator->SetPlayPolicy(nw::anim::PlayPolicy_Loop);
      else     evaluator->SetPlayPolicy(nw::anim::PlayPolicy_Onetime);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����̂��ߐݒ�ł��܂���B\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃u�����h�d�݂�ݒ肷��
 */
//-----------------------------------------------------------------------------
b32 Animation::SetBlendWeight(s32 index, f32 weight)
{
#define SET_WEIGHT() \
      if( index<m_blend_num_max && blender->GetAnimObject(index) ) \
      { \
        blender->SetWeight(index, weight); \
        ret = true; \
      } \
      else \
      { \
        GFL_ASSERT_MSG( 0, "index���s�����A�j���[�V�������Ȃ����ߐݒ�ł��܂���Bindex=%d, m_blend_num_max=%d\n", index, m_blend_num_max ); \
      }

  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_object);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_object);
      SET_WEIGHT() 
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;

#undef SET_WEIGHT
}


//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�����̃u�����h�d�݂��擾����
 */
//-----------------------------------------------------------------------------
f32 Animation::GetBlendWeight(s32 index) const
{
#define GET_WEIGHT() \
      if( index<m_blend_num_max && blender->GetAnimObject(index) ) \
      { \
        weight = blender->GetWeight(index); \
      } \
      else \
      { \
        GFL_ASSERT_MSG( 0, "index���s�����A�j���[�V�������Ȃ����ߎ擾�ł��܂���Bindex=%d, m_blend_num_max=%d\n", index, m_blend_num_max ); \
      }

  f32 weight = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߎ擾�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_object);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_object);
      GET_WEIGHT()
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return weight;

#undef GET_WEIGHT
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �u�����h����A�j���[�V������ǉ�����
 */
//-----------------------------------------------------------------------------
b32 Animation::SetBlendAnimation(s32 index, Animation* animation, f32 weight)
{
#define SET_ANIMATION() \
      if( m_type==animation->GetType() && index<m_blend_num_max ) \
      { \
        blender->ReplaceAnimObject(index, animation->GetNwAnimObject()); \
        blender->SetWeight(index, weight); \
        m_blend_animation_array[index] = animation; \
        ret = true; \
      } \
      else \
      { \
        GFL_ASSERT_MSG( 0, "�^�C�v���قȂ邩index���s���̂��ߐݒ�ł��܂���Bm_type=%d, animation�̃^�C�v=%d, index=%d, m_blend_num_max=%d\n", m_type, animation->GetType(), index, m_blend_num_max ); \
      }

  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_object);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_object);
      SET_ANIMATION() 
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return ret;

#undef SET_ANIMATION
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ��ԃu�����A�j���[�V�����̃u�����h�d�݂̐��K���̐ݒ�/�擾
 *
 *  @param[in]     enable      true�̂Ƃ����K������
 * 
 *  @retval        true�̂Ƃ����K������
 */
//-----------------------------------------------------------------------------
void Animation::SetBlendInterpolatorNormalizationEnable(b32 enable)
{
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "��ԃu�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
      blender->SetNormalizationEnabled(enable);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
      blender->SetNormalizationEnabled(enable);
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
}
b32  Animation::IsBlendInterpolatorNormalizationEnable(void) const
{
  b32 enable = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_object);
      enable = blender->GetNormalizationEnabled();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_object);
      enable = blender->GetNormalizationEnabled();
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "�A�j���[�V����������܂���B\n" );
    }
    break;
  }
  return enable;
}

// �A�j���[�V�����̐�(�u�����h�A�j���[�V�����̂Ƃ��͂��̒��g����ł�1��Ԃ�)
s32 Animation::GetAnimationNum(void) const
{
  if( m_anim_object )
  {
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         �A�j���[�V�������֘A�t����
 *
 *  @param[in]     nw_anim_group      �A�j���[�V�����O���[�v
 *  @retval        b32           �o�C���h�ɐ���������true��Ԃ�
 *
 *  m_anim_object��nw_anim_group���o���Ă����悤�ɂȂ�B
 */
//-----------------------------------------------------------------------------
b32 Animation::Bind(nw::gfx::AnimGroup* nw_anim_group)
{
  b32 bind_result = false;
  if( m_anim_object )
  {
    if( m_bind_anim_group )
    {
      // ���Ƀo�C���h�ς�
      if( m_bind_anim_group != nw_anim_group )
      {
        // �قȂ���̂��o�C���h����Ă���
        m_anim_object->Release();
        m_bind_anim_group = NULL;
        bind_result = m_anim_object->Bind(nw_anim_group);
      }
      else
      {
        // �������̂��o�C���h����Ă���
        // �������Ȃ����A�o�C���h���ʂ�true�ɂ��Ă���
        bind_result = true;
      }
    }
    else
    {
      // �܂��o�C���h����Ă��Ȃ�
      bind_result = m_anim_object->Bind(nw_anim_group);
      // �A�j���[�V�����O���[�v�� AnimEvaluator �Ƀo�C���h���܂��B
      // ����ɂ��A�A�j���[�V�����Ώۃ����o�ɃA�j���[�V�����f�[�^���֘A�t�����܂��B
    }

    if( bind_result )
    {
      m_bind_anim_group = nw_anim_group;
    }
  }
  return bind_result;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         max_anim_members�����߂�
 */
//-----------------------------------------------------------------------------
s32 Animation::GetMaxAnimMembers(ResAnim* res_anim, const s32 max_members, const s32 max_anim_members)
{
  s32 retval = max_anim_members;
  nw::anim::ResAnim res = res_anim->GetNwResAnim();
  if( max_anim_members == gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST )
  {
    retval = res.GetMemberAnimSetCount();
  }
  else if( max_anim_members == gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERS )
  {
    retval = max_members;
  }
  return retval;

      //GFL_PRINT("MAX_MEMBERs=%d, MAX_ANIM_MEMBERs=%d\n", m_max_members, res.GetMemberAnimSetCount());
      // ��LGFL_PRINT�� MAX_MEMBERs=49, MAX_ANIM_MEMBERs=28 �Ƃ������ʂ𓾂��B
 
      //GFL_PRINT("m_max_members=%d, res=%d\n", m_max_members, res.GetMemberAnimSetCount());
      // �o�͌��� m_max_members=52, res=62
      // ���f���̍��̐����A�j���̍��̐��̂ق��������ꍇ�A��L�̂悤�Ȍ��ʂɂȂ�A���L�̂悤�ȃG���[�Ŏ~�܂��Ă��܂��B
      // gfx_BaseAnimEvaluator.h:386 Panic:Animation member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxAnimMembers.
      // ���̏ꍇ��gfl::grp::g3d::Resource::GetMaxMemberAnimSetCount�őS�A�j������ő吔�𓾂ēn���Ă��炤�Ƃ悢�B
      // ���������A���f���̍��̐����A�j���̍��̐����������Č��ۂ������̂��ǂ����E�E�E
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

