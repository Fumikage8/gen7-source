//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimation.cpp
 *	@brief  3Dアニメーション
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
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dアニメーションクラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
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
 *  @brief         デストラクタ
 */
//-----------------------------------------------------------------------------
Animation::~Animation()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetAnimationManager()->RemoveObject(this);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションを生成する
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
        GFL_ASSERT_MSG( 0, "res_animのアニメーションリソースタイプ%dが不正です。\n", type );
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
        // ResAnim から、AnimEvaluator::Builder を用いて AnimEvaluator を生成します。
        // スケルタルアニメーションの場合は、TransformAnimEvaluator::Builder を使用します。
        //
        // アニメーションを1つのモデルにのみ適用する場合や、
        // コマ形式データの場合は、 AllocCache を false にすると処理負荷が下がります。
        nw::gfx::TransformAnimEvaluator* evaluator = nw::gfx::TransformAnimEvaluator::Builder()
            .AnimData(res)
            .MaxMembers(m_max_members)
            .MaxAnimMembers(m_max_anim_members)
            .AllocCache(false)
            .Create(device_allocator);
       
        GFL_ASSERT_MSG( evaluator, "nw::gfx::TransformAnimEvaluatorを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
        m_anim_object = evaluator;
      }
      break;
    case STATE_EVALUATOR_OTHERS:
      {
        // アニメーションオブジェクトを生成します。
        nw::gfx::AnimEvaluator* evaluator = nw::gfx::AnimEvaluator::Builder()
            .AnimData(res)
            .MaxMembers(m_max_members)
            .MaxAnimMembers(m_max_anim_members)
            .AllocCache(false)
            .Create(device_allocator);
        
        GFL_ASSERT_MSG( evaluator, "nw::gfx::AnimEvaluatorを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
        m_anim_object = evaluator;
      }
      break;
    }

    if( m_anim_object )
    {
      SetResourceIdAndIndexInResource(res_anim->GetResourceId(), res_anim->GetIndexInResource());  // アニメーションリソースのタイプはObjectではなくこのクラスで覚えているだけ。
    }
    else
    {
      m_state = STATE_NONE;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "既に他のアニメーションが生成されています。\n");
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
 *  @brief         補間ブレンドアニメーションを生成する
 *  @brief         加算ブレンドアニメーションを生成する
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
          GFL_ASSERT_MSG( 0, "アニメーションリソースタイプ%dが不正です。\n", type );
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
          // スケルタルアニメーション補間を生成します。
          nw::gfx::TransformAnimInterpolator* blender = nw::gfx::TransformAnimInterpolator::Builder()
              .IgnoreNoAnimMember(ignore_no_anim_member)
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::TransformAnimInterpolatorを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
          interpolator = blender;
        }
        break;
      case STATE_BLEND_INTERPOLATOR_OTHERS:
        {
          // アニメーション補間を生成します。
          nw::gfx::AnimInterpolator* blender = nw::gfx::AnimInterpolator::Builder()
              .IgnoreNoAnimMember(ignore_no_anim_member)
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::AnimInterpolatorを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
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
              // NULL を指定しておいて、後で ReplaceAnimObject() で置き換えることも可能です。
          interpolator->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        for( s32 i=0; i<num; ++i )
        {
          if( pack_array[i].animation )
          {
            if( m_type == pack_array[i].animation->GetType() )  // アニメーションリソースのタイプが一致するものだけ追加する
            {
              interpolator->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
                  // NULL を指定した場合、アニメーションオブジェクトがない状態になります。
              interpolator->SetWeight(i, pack_array[i].weight);
              m_blend_animation_array[i] = pack_array[i].animation;
            }
            else
            {
              GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check 追加されないだけ。
                  "アニメーションリソースのタイプが異なります。全体のタイプ%d, pack_array[%d]のタイプ%d\n",
                  m_type, i, pack_array[i].animation->GetType());
            }
          }
        }

        m_anim_object = interpolator;
        SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // アニメーションリソースのタイプはObjectではなくこのクラスで覚えているだけ。
      }
      else
      {
        m_state = STATE_NONE;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "ブレンドするアニメーションの最大数が%dです。\n", num_max);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "既に他のアニメーションが生成されています。\n");
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
          GFL_ASSERT_MSG( 0, "アニメーションリソースタイプ%dが不正です。\n", type );
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
          // スケルタルアニメーション加算を生成します。
          nw::gfx::TransformAnimAdder* blender = nw::gfx::TransformAnimAdder::Builder()
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::TransformAnimAdderを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
          adder = blender;
        }
        break;
      case STATE_BLEND_ADDER_OTHERS:
        {
          // アニメーション加算を生成します。
          nw::gfx::AnimAdder* blender = nw::gfx::AnimAdder::Builder()
              .MaxAnimObjects(num_max)
              .Create(device_allocator);
         
          GFL_ASSERT_MSG( blender, "nw::gfx::AnimAdderを生成できませんでした。\n" );  //@fix 生成失敗時m_stateを無害なものにしておく。
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
              // NULL を指定しておいて、後で ReplaceAnimObject() で置き換えることも可能です。
          adder->SetWeight(i, 1.0f);
          m_blend_animation_array[i] = NULL;
        }

        for( s32 i=0; i<num; ++i )
        {
          if( pack_array[i].animation )
          {
            if( m_type == pack_array[i].animation->GetType() )  // アニメーションリソースのタイプが一致するものだけ追加する
            {
              adder->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
                  // NULL を指定した場合、アニメーションオブジェクトがない状態になります。
              adder->SetWeight(i, pack_array[i].weight);
              m_blend_animation_array[i] = pack_array[i].animation;
            }
            else
            {
              GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check 追加されないだけ。
                  "アニメーションリソースのタイプが異なります。全体のタイプ%d, pack_array[%d]のタイプ%d\n",
                  m_type, i, pack_array[i].animation->GetType());
            }
          }
        }

        m_anim_object = adder;
        SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // アニメーションリソースのタイプはObjectではなくこのクラスで覚えているだけ。
      }
      else
      {
        m_state = STATE_NONE;
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "ブレンドするアニメーションの最大数が%dです。\n", num_max);
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "既に他のアニメーションが生成されています。\n");
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
 *  @brief         アニメーションを破棄する
 */
//-----------------------------------------------------------------------------
void Animation::Destroy(void)
{
  SetResourceIdAndIndexInResource(OBJECT_RESOURCE_ID_NOT_USE, OBJECT_INDEX_IN_RESOURCE_NOT_USE);  // アニメーションリソースのタイプはObjectではなくこのクラスで覚えているだけ。

  if( m_anim_object )
  {
    if( m_bind_anim_group )
    {
      m_anim_object->Release();
      //アニメーションの取り外し
      //AnimObject::Release でアニメーション評価からアニメーショングループを解放します。
      //アニメーションの再生で述べた SetAnimObject や Set***AnimObject に NULL を渡して、アニメーションを解除します。
      //どちらの処理も必要ですので、注意してください。
      // @note この後のCalculateできちんと処理されると思うが、取り外し忘れがないかチェックするようにしたい
      m_bind_anim_group = NULL;
    }
    nw::gfx::SafeDestroy(m_anim_object);  // NULLを代入してくれる
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
 *  @brief         アニメーションを変更する
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
 
      // アニメーションを変更する
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
      
      SetResourceIdAndIndexInResource(res_anim->GetResourceId(), res_anim->GetIndexInResource());  // アニメーションリソースのタイプはObjectではなくこのクラスで覚えているだけ。
    }
    else
    {
      GFL_ASSERT_MSG(0, "アニメーションを変更できません。\n");
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
 *  @brief         補間ブレンドアニメーションを変更する
 *  @brief         加算ブレンドアニメーションを変更する
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

    // ブレンドを変更する
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
        if( m_type == pack_array[i].animation->GetType() )  // アニメーションリソースのタイプが一致するものだけ追加する
        {
          interpolator->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
              // NULL を指定した場合、アニメーションオブジェクトがない状態になります。
          interpolator->SetWeight(i, pack_array[i].weight);
          m_blend_animation_array[i] = pack_array[i].animation;
        }
        else
        {
          GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check 追加されないだけ。
              "アニメーションリソースのタイプが異なります。全体のタイプ%d, pack_array[%d]のタイプ%d\n",
              m_type, i, pack_array[i].animation->GetType());
        }
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "補間ブレンドアニメーションを変更できません。\n");
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

    // ブレンドを変更する
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
        if( m_type == pack_array[i].animation->GetType() )  // アニメーションリソースのタイプが一致するものだけ追加する
        {
          adder->ReplaceAnimObject(i, pack_array[i].animation->GetNwAnimObject());
              // NULL を指定した場合、アニメーションオブジェクトがない状態になります。
          adder->SetWeight(i, pack_array[i].weight);
          m_blend_animation_array[i] = pack_array[i].animation;
        }
        else
        {
          GFL_ASSERT_MSG(m_type != pack_array[i].animation->GetType(),  //@check 追加されないだけ。
              "アニメーションリソースのタイプが異なります。全体のタイプ%d, pack_array[%d]のタイプ%d\n",
              m_type, i, pack_array[i].animation->GetType());
        }
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "加算ブレンドアニメーションを変更できません。\n");
  }
}


//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのフレームを設定する
 *
 *  @param[in]     frame      フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのフレームを取得する
 *
 *  @retval        フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを設定する
 *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
 *
 *  @param[in]     step_frame      更新フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}
//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの更新フレームを取得する
 *
 *  @retval        更新フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを設定する
 *
 *  @param[in]     frame      フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームを取得する
 *
 *  @retval        フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを設定する
 *
 *  @param[in]     frame      フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの終了フレームを取得する
 *
 *  @retval        フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return frame;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの開始フレームと終了フレームを設定する
 *
 *  @param[in]     start_frame    開始フレーム
 *  @param[in]     end_frame      終了フレーム
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームが開始フレームか否か判定する
 *
 *  @retval        b32    現在のフレームが開始フレームの場合tureを返す
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
 *
 *  @retval        b32    現在のフレームが終了フレームの場合tureを返す
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションの現在のフレームがstep_frameから見て端フレームか否か判定する
 *
 *  @retval        bool    現在のフレームがstep_frameから見て端フレームの場合tureを返す
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
 *  @brief         アニメーションがループ再生か否か取得する
 *
 *  @retval        b32       ループのときtrue、ループでないときfalseを返す
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため取得できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return loop;
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションにループ再生を設定する
 *
 *  @param[in]     loop    ループのときtrue、ループでないときfalseを設定する
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションのため設定できません。\n" );
    }
    break;
  default:
    {
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
}

//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのブレンド重みを設定する
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
        GFL_ASSERT_MSG( 0, "indexが不正かアニメーションがないため設定できません。index=%d, m_blend_num_max=%d\n", index, m_blend_num_max ); \
      }

  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションではないため設定できません。\n" );
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
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;

#undef SET_WEIGHT
}


//-----------------------------------------------------------------------------
/*
 *  @brief         アニメーションのブレンド重みを取得する
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
        GFL_ASSERT_MSG( 0, "indexが不正かアニメーションがないため取得できません。index=%d, m_blend_num_max=%d\n", index, m_blend_num_max ); \
      }

  f32 weight = 0.0f;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションではないため取得できません。\n" );
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
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return weight;

#undef GET_WEIGHT
}

//-----------------------------------------------------------------------------
/*
 *  @brief         ブレンドするアニメーションを追加する
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
        GFL_ASSERT_MSG( 0, "タイプが異なるかindexが不正のため設定できません。m_type=%d, animationのタイプ=%d, index=%d, m_blend_num_max=%d\n", m_type, animation->GetType(), index, m_blend_num_max ); \
      }

  b32 ret = false;
  switch( m_state )
  {
  case STATE_EVALUATOR_SKELETAL:
  case STATE_EVALUATOR_OTHERS:
    {
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションではないため設定できません。\n" );
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
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return ret;

#undef SET_ANIMATION
}

//-----------------------------------------------------------------------------
/*
 *  @brief         補間ブレンアニメーションのブレンド重みの正規化の設定/取得
 *
 *  @param[in]     enable      trueのとき正規化する
 * 
 *  @retval        trueのとき正規化する
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
      GFL_ASSERT_MSG( 0, "補間ブレンドアニメーションではないため設定できません。\n" );
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
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
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
      GFL_ASSERT_MSG( 0, "ブレンドアニメーションではないため設定できません。\n" );
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
      GFL_ASSERT_MSG( 0, "アニメーションがありません。\n" );
    }
    break;
  }
  return enable;
}

// アニメーションの数(ブレンドアニメーションのときはその中身が空でも1を返す)
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
 *  @brief         アニメーションを関連付ける
 *
 *  @param[in]     nw_anim_group      アニメーショングループ
 *  @retval        b32           バインドに成功したらtrueを返す
 *
 *  m_anim_objectがnw_anim_groupを覚えておくようになる。
 */
//-----------------------------------------------------------------------------
b32 Animation::Bind(nw::gfx::AnimGroup* nw_anim_group)
{
  b32 bind_result = false;
  if( m_anim_object )
  {
    if( m_bind_anim_group )
    {
      // 既にバインド済み
      if( m_bind_anim_group != nw_anim_group )
      {
        // 異なるものがバインドされている
        m_anim_object->Release();
        m_bind_anim_group = NULL;
        bind_result = m_anim_object->Bind(nw_anim_group);
      }
      else
      {
        // 同じものがバインドされている
        // 何もしないが、バインド結果はtrueにしておく
        bind_result = true;
      }
    }
    else
    {
      // まだバインドされていない
      bind_result = m_anim_object->Bind(nw_anim_group);
      // アニメーショングループを AnimEvaluator にバインドします。
      // これにより、アニメーション対象メンバにアニメーションデータが関連付けられます。
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
 *  @brief         max_anim_membersを求める
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
      // 上記GFL_PRINTで MAX_MEMBERs=49, MAX_ANIM_MEMBERs=28 という結果を得た。
 
      //GFL_PRINT("m_max_members=%d, res=%d\n", m_max_members, res.GetMemberAnimSetCount());
      // 出力結果 m_max_members=52, res=62
      // モデルの骨の数よりアニメの骨の数のほうが多い場合、上記のような結果になり、下記のようなエラーで止まってしまう。
      // gfx_BaseAnimEvaluator.h:386 Panic:Animation member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxAnimMembers.
      // この場合はgfl::grp::g3d::Resource::GetMaxMemberAnimSetCountで全アニメから最大数を得て渡してもらうとよい。
      // そもそも、モデルの骨の数よりアニメの骨の数が多いって現象がいいのかどうか・・・
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

