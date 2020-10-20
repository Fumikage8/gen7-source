//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationSlot.cpp
 *	@brief  3D�A�j���[�V���������蓖�Ă�X���b�g
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
#include <grp/g3d/gfl_G3dIAnimation.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dUnitAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>

#include <grp/g3d/gfl_G3dAnimationSlot.h>


namespace gfl {
namespace grp {
namespace g3d {

//-------------------------------------
///	�A�j���[�V���������蓖�Ă�X���b�g
//=====================================
void AnimationSlot::InitializeDescriptionForDefault(
    Description*  description
)
{
  description->mode                               = MODE_NONE;
  description->animation                          = NULL;
  description->blend_type                         = ResAnim::TYPE_NONE;
  description->blend_num_max                      = 0;
  description->blend_num                          = 0;
  description->blend_pack_array                   = NULL;
  description->interpolator_ignore_no_anim_member = false;
  description->interpolator_normalization_enable  = true;
}
void AnimationSlot::InitializeDescriptionForAnimation(
    Description*  description,
    IAnimation*   animation
)
{
  InitializeDescriptionForDefault(description);
  description->animation = animation;
}
void AnimationSlot::InitializeDescriptionForBlendInterpolator(
    Description*     description,
    ResAnim::Type    blend_type,
    s32              blend_num_max,
    s32              blend_num,
    AnimationPack*   blend_pack_array,
    b32              interpolator_ignore_no_anim_member,
    b32              interpolator_normalization_enable 
)
{
  InitializeDescriptionForDefault(description);
  description->blend_type                         = blend_type;
  description->blend_num_max                      = blend_num_max;
  description->blend_num                          = blend_num;
  description->blend_pack_array                   = blend_pack_array;
  description->interpolator_ignore_no_anim_member = interpolator_ignore_no_anim_member;
  description->interpolator_normalization_enable  = interpolator_normalization_enable;
}
void AnimationSlot::InitializeDescriptionForBlendAdder(
    Description*     description,
    ResAnim::Type    blend_type,
    s32              blend_num_max,
    s32              blend_num,
    AnimationPack*   blend_pack_array
)
{
  InitializeDescriptionForDefault(description);
  description->blend_type                  = blend_type;
  description->blend_num_max               = blend_num_max;
  description->blend_num                   = blend_num;
  description->blend_pack_array            = blend_pack_array;
}
  
AnimationSlot::AnimationSlot(void)
  : m_enable(1),
    m_state(STATE_NONE),
    m_type(ResAnim::TYPE_NONE),
    m_anim_object_index(ANIM_OBJECT_INDEX_NONE),
    m_animation(NULL),
    m_blend_num_max(0),
    m_blend_animation_array(NULL),
    m_anim_blender(NULL),
    m_anim_blender_bind_anim_group(NULL)
{
  // �������Ȃ�
}
AnimationSlot::~AnimationSlot()
{
  Destroy();
}

void AnimationSlot::Create(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  Description*            description
)
{
  Description  l_description;
  if( description )
  {
    l_description = *description;
  }
  switch(l_description.mode)
  {
  case MODE_ANIMATION:
    {
      CreateAnimation(
          l_description.animation
      );
    }
    break;
  case MODE_BLEND_INTERPOLATOR:
    {
      CreateBlendInterpolator(
          heap_allocator,
          device_allocator,
          l_description.blend_type,
          l_description.blend_num_max,
          l_description.blend_num,
          l_description.blend_pack_array,
          l_description.interpolator_ignore_no_anim_member,
          l_description.interpolator_normalization_enable
      );
    }
    break;
  case MODE_BLEND_ADDER:
    {
      CreateBlendAdder(
          heap_allocator,
          device_allocator,
          l_description.blend_type,
          l_description.blend_num_max,
          l_description.blend_num,
          l_description.blend_pack_array
      );
    }
    break;
  }
}

void AnimationSlot::CreateAnimation(
  IAnimation*              animation
)
{
  if( m_state == STATE_NONE )
  {
    ResAnim::Type type = animation->GetType();
    switch( type )
    {
    case ResAnim::TYPE_SKELETAL:
    case ResAnim::TYPE_MATERIAL:
    case ResAnim::TYPE_VISIBILITY:
    case ResAnim::TYPE_CAMERA:
    case ResAnim::TYPE_LIGHT:
      {  
        m_state = STATE_ONE;
        m_type  = type;
        m_animation = animation;
      }
      break;
    default:
      {
        GFL_ASSERT_MSG( 0, "animation�̃A�j���[�V�������\�[�X�^�C�v%d���s���ł��B\n", type );
        m_state = STATE_NONE;
        m_type  = ResAnim::TYPE_NONE;
      }
      break;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "���ɑ��̃A�j���[�V��������������Ă��܂��B\n");
  }
}

void AnimationSlot::CreateBlendInterpolator(
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
        m_blend_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) IAnimation*[m_blend_num_max];

        interpolator->SetNormalizationEnabled(normalization_enable);  
        for( s32 i=0; i<m_blend_num_max; ++i )
        {
          interpolator->AddAnimObject(NULL);
              // NULL ���w�肵�Ă����āA��� ReplaceAnimObject() �Œu�������邱�Ƃ��\�ł��B
          interpolator->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        {
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

        m_anim_blender = interpolator;
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
void AnimationSlot::ChangeBlendInterpolator(
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

void AnimationSlot::CreateBlendAdder(
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
          // �X�P���^���A�j���[�V������Ԃ𐶐����܂��B
          nw::gfx::TransformAnimAdder* blender = nw::gfx::TransformAnimAdder::Builder()
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::TransformAnimAdder�𐶐��ł��܂���ł����B\n" );  //@fix �������s��m_state�𖳊Q�Ȃ��̂ɂ��Ă����B
          adder = blender;
        }
        break;
      case STATE_BLEND_ADDER_OTHERS:
        {
          // �A�j���[�V������Ԃ𐶐����܂��B
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
        m_blend_animation_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) IAnimation*[m_blend_num_max];

        for( s32 i=0; i<m_blend_num_max; ++i )
        {
          adder->AddAnimObject(NULL);
              // NULL ���w�肵�Ă����āA��� ReplaceAnimObject() �Œu�������邱�Ƃ��\�ł��B
          adder->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        {
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

        m_anim_blender = adder;
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
void AnimationSlot::ChangeBlendAdder(
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

void AnimationSlot::Destroy(void)
{
  if( m_anim_blender )
  {
    if( m_anim_blender_bind_anim_group )
    {
      m_anim_blender->Release();
      // @note ���̌��Calculate�ł�����Ə��������Ǝv�����A���O���Y�ꂪ�Ȃ����`�F�b�N����悤�ɂ�����
      m_anim_blender_bind_anim_group = NULL;
    }
    nw::gfx::SafeDestroy(m_anim_blender);  // NULL�������Ă����
  }

  if( m_blend_animation_array )
  {
    GFL_DELETE_ARRAY m_blend_animation_array;
    m_blend_animation_array = NULL;
  }

  m_state = STATE_NONE;
  m_type  = ResAnim::TYPE_NONE;
  //m_anim_object_index = ANIM_OBJECT_INDEX_NONE;  // Destroy��Change�ł��Ă΂��̂ŁA���̒l�͐ݒ肵�����܂ŕύX���Ȃ����Ƃɂ���B
  m_animation = NULL;
  m_blend_num_max = 0;
}

void AnimationSlot::Calculate(void)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      if( m_animation->GetObjectType() == OBJECT_TYPE_UNIT_ANIMATION )
      {
        m_animation->Calculate();
      }
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      for( s32 i=0; i<m_blend_num_max; ++i )
      {
        if( m_blend_animation_array[i] )
        {
          IAnimation* animation = m_blend_animation_array[i];
          if( animation->GetObjectType() == OBJECT_TYPE_UNIT_ANIMATION )
          {
            animation->Calculate();
            m_anim_blender->ReplaceAnimObject(i, animation->GetNwAnimObject());
                // NULL ���w�肵���ꍇ�A�A�j���[�V�����I�u�W�F�N�g���Ȃ���ԂɂȂ�܂��B
            // �A�j���[�V�����I�u�W�F�N�g�̒u�����������ł��ăE�F�C�g�͕ύX���Ȃ��̂ŁA�E�F�C�g�̐ݒ肵�����͂����O�̂������p���B
          }
        }
      }
    }
    break;
  }
}

nw::gfx::AnimObject* AnimationSlot::GetNwAnimObject(void)
{
  nw::gfx::AnimObject* anim_object = NULL;
  switch( m_state )
  {
  case STATE_ONE:
    {
      anim_object = m_animation->GetNwAnimObject();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      anim_object = m_anim_blender;
    }
    break;
  }
  return anim_object;
}

b32 AnimationSlot::BindBlend(nw::gfx::AnimGroup* nw_anim_group)
{
  b32 bind_result = false;
  if( m_anim_blender )
  {
    if( m_anim_blender_bind_anim_group )
    {
      // ���Ƀo�C���h�ς�
      if( m_anim_blender_bind_anim_group != nw_anim_group )
      {
        // �قȂ���̂��o�C���h����Ă���
        m_anim_blender->Release();
        m_anim_blender_bind_anim_group = NULL;
        bind_result = m_anim_blender->Bind(nw_anim_group);
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
      bind_result = m_anim_blender->Bind(nw_anim_group);
    }

    if( bind_result )
    {
      m_anim_blender_bind_anim_group = nw_anim_group;
    }
  }
  return bind_result;
}

void AnimationSlot::ChangeAnimation(
  IAnimation*              animation
)
{
  // STATE_NONE�̂Ƃ���Destroy����Ƃ���GFL_ASSERT_MSG�ɂ��Ă��܂���B  //@check �����ɂЂ�������Ȃ��悤��ASSERT�m�F�ς݂̈������Ă����B
  // ���Ƀ������ɉe���͂��Ȃ��̂ŁB

  if( animation )
  {
    if( m_state == STATE_NONE )
    {
      CreateAnimation(animation);
    }
    else
    {
      if( m_state == STATE_ONE && m_type == animation->GetType() )
      {
        m_animation = animation;
      }
      else
      {
        Destroy();
        CreateAnimation(animation);
      }
    }
  }
  else
  {
    Destroy();
  }
}

void AnimationSlot::ChangeBlendInterpolator(
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
            nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
        interpolator = blender;
      }
      break;
    case STATE_BLEND_INTERPOLATOR_OTHERS:
      {
        nw::gfx::AnimInterpolator* blender = 
            nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
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

    {
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
  }
  else
  {
    GFL_ASSERT_MSG(0, "��ԃu�����h�A�j���[�V������ύX�ł��܂���B\n");
  }
}
void AnimationSlot::ChangeBlendAdder(
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
            nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_blender);
        adder = blender;
      }
      break;
    case STATE_BLEND_ADDER_OTHERS:
      {
        nw::gfx::AnimAdder* blender = 
            nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_blender);
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

    {
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
  }
  else
  {
    GFL_ASSERT_MSG(0, "��ԃu�����h�A�j���[�V������ύX�ł��܂���B\n");
  }
}


// �A�j���[�V�����̐�(�u�����h�A�j���[�V�����̂Ƃ��͂��̒��g�𐔂���)
s32 AnimationSlot::GetAnimationNum(void) const
{
  // @note STATE_NONE�̂Ƃ���0
  // STATE_ONE�̂Ƃ���m_animation�ɃA�j���[�V���������Ă����(�u�����h�̂Ƃ��̒��g����ł�)1
  // STATE_BLEND_...�̂Ƃ���m_blend_animation_array[i]�ɃA�j���[�V���������Ă����(�u�����h�̂Ƃ��̒��g����ł�)+1*i
  // �Ƃ����ӂ��ɂ��āAUnitAnimation�̂Ƃ��������Ԃ��Ă��Ă������1�Ƃ����ق���������������Ȃ��B
  // UnitAnimation�������Ԃ��̂͂��̂܂܂ł����Ǝv���̂ŁA���̉��߂��󂯎����AnimationSlot�����܂��ς��邱�ƁB

  s32 num = 0;
  switch( m_state )
  {
  case STATE_ONE:
    {
      if( m_animation )
      {
        // ����m_animation���u�����h�A�j���[�V�����ł��̒��g����̂��Ƃ����邩������Ȃ����A
        // ���̏ꍇ��1���Ԃ��Ă���Bm_animation�̒���[���[�����ǂ��Ă����悤�Ȃ��Ƃ͂��Ȃ��B
        // ����m_animation��UnitAnimation�̏ꍇ�́A�����ɂ������Ă���Animation�̐����Ԃ��Ă��邪�A
        // �������Ă���Animation�̒��g���u�����h�A�j���[�V�����ŋ󂩂ǂ��͌��Ă��Ȃ��B
        num = m_animation->GetAnimationNum();
      }
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
  case STATE_BLEND_INTERPOLATOR_OTHERS:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      for( s32 i=0; i<m_blend_num_max; ++i )
      {
        if( m_blend_animation_array[i] )
        {
          // ����m_blend_animation_array[i]���u�����h�A�j���[�V�����ł��̒��g����̂��Ƃ����邩������Ȃ����A
          // ���̏ꍇ��1���Ԃ��Ă���Bm_blend_animation_array[i]�̒���[���[�����ǂ��Ă����悤�Ȃ��Ƃ͂��Ȃ��B
          // ����m_blend_animation_array[i]��UnitAnimation�̏ꍇ�́A�����ɂ������Ă���Animation�̐����Ԃ��Ă��邪�A
          // �������Ă���Animation�̒��g���u�����h�A�j���[�V�����ŋ󂩂ǂ��͌��Ă��Ȃ��B
          num += m_blend_animation_array[i]->GetAnimationNum();
        }
      }
    }
    break;
  default:
    {
      // �A�j���[�V�������Ȃ��B
      // �������Ȃ��B
    }
    break;
  }
  return num;
}


void AnimationSlot::SetFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetFrame(frame);
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
f32 AnimationSlot::GetFrame(void) const
{
  f32 ret = 0.0f;
  switch( m_state )
  {
  case STATE_ONE:
    {
      ret = m_animation->GetFrame();
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

void AnimationSlot::SetStepFrame(f32 step_frame)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetStepFrame(step_frame);
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
f32 AnimationSlot::GetStepFrame(void) const
{
  f32 ret = 0.0f;
  switch( m_state )
  {
  case STATE_ONE:
    {
      ret = m_animation->GetStepFrame();
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

void AnimationSlot::SetStartFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetStartFrame(frame);
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
f32 AnimationSlot::GetStartFrame(void) const
{
  f32 frame = 0.0f;
  switch( m_state )
  {
  case STATE_ONE:
    {
      frame = m_animation->GetStartFrame();
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
void AnimationSlot::SetEndFrame(f32 frame)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetEndFrame(frame);
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
f32 AnimationSlot::GetEndFrame(void) const
{
  f32 frame = 0.0f;
  switch( m_state )
  {
  case STATE_ONE:
    {
      frame = m_animation->GetEndFrame();
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
void AnimationSlot::SetStartAndEndFrame(f32 start_frame, f32 end_frame)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetStartAndEndFrame(start_frame, end_frame);
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
bool AnimationSlot::IsFrameStartFrame(void) const
{
  bool ret = false;
  switch( m_state )
  {
  case STATE_ONE:
    {
      ret = m_animation->IsFrameStartFrame();
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
bool AnimationSlot::IsFrameEndFrame(void) const
{
  bool ret = false;
  switch( m_state )
  {
  case STATE_ONE:
    {
      ret = m_animation->IsFrameEndFrame();
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
bool AnimationSlot::IsFrameStepTerminalFrame(void) const
{
  bool ret = false;
  switch( m_state )
  {
  case STATE_ONE:
    {
      ret = m_animation->IsFrameStepTerminalFrame();
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
bool AnimationSlot::IsLoop(void) const
{

  bool loop = false;
  switch( m_state )
  {
  case STATE_ONE:
    {
      loop = m_animation->IsLoop();
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
void AnimationSlot::SetLoop(bool loop)
{
  switch( m_state )
  {
  case STATE_ONE:
    {
      m_animation->SetLoop(loop);
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

b32 AnimationSlot::SetBlendWeight(s32 index, f32 weight)
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
  case STATE_ONE:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_blender);
      SET_WEIGHT() 
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_blender);
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
f32 AnimationSlot::GetBlendWeight(s32 index) const
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
  case STATE_ONE:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߎ擾�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_blender);
      GET_WEIGHT()
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_blender);
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

b32 AnimationSlot::SetBlendAnimation(s32 index, IAnimation* animation, f32 weight)
{
#define SET_ANIMATION() \
      if( animation ) \
      { \
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
        } \
      } \
      else \
      { \
        m_blend_animation_array[index] = animation; \
        blender->SetWeight(index, weight); \
        blender->ReplaceAnimObject(index, NULL); \
        ret = true; \
      }

  b32 ret = false;
  switch( m_state )
  {
  case STATE_ONE:
    {
      GFL_ASSERT_MSG( 0, "�u�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_ADDER_SKELETAL:
    {
      nw::gfx::TransformAnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimAdder*>(m_anim_blender);
      SET_ANIMATION() 
    }
    break;
  case STATE_BLEND_ADDER_OTHERS:
    {
      nw::gfx::AnimAdder* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimAdder*>(m_anim_blender);
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

void AnimationSlot::SetBlendInterpolatorNormalizationEnable(b32 enable)
{
  switch( m_state )
  {
  case STATE_ONE:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "��ԃu�����h�A�j���[�V�����ł͂Ȃ����ߐݒ�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
      blender->SetNormalizationEnabled(enable);
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
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
b32  AnimationSlot::IsBlendInterpolatorNormalizationEnable(void) const
{
  b32 enable = false;
  switch( m_state )
  {
  case STATE_ONE:
  case STATE_BLEND_ADDER_SKELETAL:
  case STATE_BLEND_ADDER_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "��ԃu�����h�A�j���[�V�����ł͂Ȃ����ߎ擾�ł��܂���B\n" );
    }
    break;
  case STATE_BLEND_INTERPOLATOR_SKELETAL:
    {
      nw::gfx::TransformAnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::TransformAnimInterpolator*>(m_anim_blender);
      enable = blender->GetNormalizationEnabled();
    }
    break;
  case STATE_BLEND_INTERPOLATOR_OTHERS:
    {
      nw::gfx::AnimInterpolator* blender = 
          nw::ut::DynamicCast<nw::gfx::AnimInterpolator*>(m_anim_blender);
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

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

