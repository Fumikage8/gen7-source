//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUnitAnimation.cpp
 *	@brief  3Dアニメーションの単位となるもの
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
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

#include <grp/g3d/gfl_G3dUnitAnimation.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	アニメーションの単位となるもの
//=====================================

// コンストラクタ
UnitAnimation::UnitAnimation(void)
  : m_state(STATE_NONE),
    m_type(ResAnim::TYPE_NONE),
    m_num_max(0),
    m_num(0),
    m_pack_array(NULL),
    m_blend(NULL),
    m_blend_bind_anim_group(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetUnitAnimationManager()->AddObject(this);
}

// デストラクタ
UnitAnimation::~UnitAnimation()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetUnitAnimationManager()->RemoveObject(this);
}

// 生成
void UnitAnimation::Create(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max
)
{
  if( m_num_max == 0 )
  {
    GFL_ASSERT_MSG(m_state==STATE_NONE&&m_type==ResAnim::TYPE_NONE, "UnitAnimationが生成されていないはずなのに、状態%dかタイプ%dが異常です。\n", m_state, m_type);  //@check 異常だが生成することはできる。

    b32 create_animation_pack = CreateBlend( 
      device_allocator,
      type,
      num_max
    );
    if( create_animation_pack )
    {
      m_pack_array = GFL_NEW_ARRAY(heap_allocator->GetHeapBase()) AnimationPack[m_num_max];
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "既にUnitAnimationが生成されているため、新たに生成できません。m_num_max=%d\n", m_num_max);
  }
}

// m_type, m_num_max, m_blendを設定、生成する
b32 UnitAnimation::CreateBlend( 
  gfl::heap::NwAllocator* device_allocator,
  ResAnim::Type           type,
  s32                     num_max
)
{
  b32 create_animation_pack = false;

  if( num_max > 0 && 
      (
           type == ResAnim::TYPE_SKELETAL
        || type == ResAnim::TYPE_MATERIAL
        || type == ResAnim::TYPE_VISIBILITY
        || type == ResAnim::TYPE_CAMERA
        || type == ResAnim::TYPE_LIGHT
      )
  )
  {
    m_num_max = num_max;
    m_type = type;

    create_animation_pack = true;
   
    if( m_num_max > 1 )
    {
      if( m_type == ResAnim::TYPE_SKELETAL )
      {
        nw::gfx::TransformAnimInterpolator* skeletal_blend = nw::gfx::TransformAnimInterpolator::Builder()
          .IgnoreNoAnimMember(false)
          .MaxAnimObjects(m_num_max)
          .Create(device_allocator);
        m_blend = skeletal_blend;
      }
      else
      {
        nw::gfx::AnimInterpolator* others_blend = nw::gfx::AnimInterpolator::Builder()
          .IgnoreNoAnimMember(false)
          .MaxAnimObjects(m_num_max)
          .Create(device_allocator);
        m_blend = others_blend;
      }

      if( m_blend )
      {
        m_blend->SetNormalizationEnabled(true);
        for( s32 i=0; i<m_num_max; ++i )
        {
          m_blend->AddAnimObject(NULL);  // NULL を指定しておいて、後で ReplaceAnimObject() で置き換えることも可能です。
          m_blend->SetWeight(i, 1.0f);
        }
      }
    }
  }

  return create_animation_pack;
}
void UnitAnimation::ChangeAnimation(
  gfl::heap::NwAllocator* heap_allocator,
  gfl::heap::NwAllocator* device_allocator,
  s32                     num_max,
  Animation*              animation,
  u32                     change_frame
)
{
  if( m_num_max == 0 )
  {
    Create(
      heap_allocator,
      device_allocator,
      animation->GetType(),
      num_max
    );
    ChangeAnimation(
      animation,
      change_frame
    );
  }
  else
  {
    if( m_num_max == num_max )  // ( m_num_max >= num_max ) でもつくり直す。
                                // num_max個だけブレンドしたいのにそれ以上ブレンドするということになってしまうから。
    {
      ChangeAnimation(
        device_allocator,
        animation,
        change_frame
      );
    }
    else
    {
      Destroy();
      Create(
        heap_allocator,
        device_allocator,
        animation->GetType(),
        num_max
      );
      ChangeAnimation(
        animation,
        change_frame
      );
    }
  }
}

void UnitAnimation::ChangeAnimation(
  gfl::heap::NwAllocator* device_allocator,
  Animation*              animation,
  u32                     change_frame
)
{
  if( m_num_max > 0 )
  {
    ResAnim::Type type = animation->GetType();
    if( m_type == type )
    {
      ChangeAnimation(
        animation,
        change_frame
      );
    }
    else
    {
      // Destroy
      {
        if( m_blend )
        {
          if( m_blend_bind_anim_group )
          {
            m_blend->Release();
            // @note この後のCalculateできちんと処理されると思うが、取り外し忘れがないかチェックするようにしたい
            m_blend_bind_anim_group = NULL;
          }
          nw::gfx::SafeDestroy(m_blend);  // NULLを代入してくれる
        }
        if( m_pack_array )
        {
          for( s32 i=0; i<m_num_max; ++i )
          {
            m_pack_array[i] = AnimationPack();  // もう1回使うので中身を空にしておく
          }
        }

        m_state = STATE_NONE;
        m_type  = ResAnim::TYPE_NONE;
        m_num     = 0;
      }

      // Create
      {
        // AnimationPackは既にあるのでつくらなくてよい
        CreateBlend( 
          device_allocator,
          type,
          m_num_max
        );
      }

      // ChangeAnimation
      {
        ChangeAnimation(
          animation,
          change_frame
        );
      }
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの最大数が%dなので、変更できません。\n", m_num_max);
  }
}

// 破棄
void UnitAnimation::Destroy(void)
{
  if( m_blend )
  {
    if( m_blend_bind_anim_group )
    {
      m_blend->Release();
      // @note この後のCalculateできちんと処理されると思うが、取り外し忘れがないかチェックするようにしたい
      m_blend_bind_anim_group = NULL;
    }
    nw::gfx::SafeDestroy(m_blend);  // NULLを代入してくれる
  }
  if( m_pack_array )
  {
    GFL_DELETE_ARRAY m_pack_array;
    m_pack_array = NULL;
  }

  m_state = STATE_NONE;
  m_type  = ResAnim::TYPE_NONE;
  m_num_max = 0;
  m_num     = 0;
}

// 計算処理
// 必ず毎フレーム呼んで下さい。
void UnitAnimation::Calculate(void)
{
  switch( m_state )
  {
  case STATE_OTHERS_BLEND:
  case STATE_SKELETAL_BLEND:
    {
      f32 w = 1.0f;
      for( s32 i=0; i<m_num; ++i )
      {
        if( m_pack_array[i].frame_count >= m_pack_array[i].frame_max )
        {
          // [i]になったので、[i]より古いアニメーションは手放す
          for( s32 j=i+1; j<m_num; ++j )
          {
            m_pack_array[j] = AnimationPack();
            m_blend->ReplaceAnimObject(j, NULL);  // NULL を指定した場合、アニメーションオブジェクトがない状態になります。
            m_blend->SetWeight(j, 1.0f);
          }
        
          // [i]の設定
          if( i==0 )
          {
            // 1個だけになった
            m_blend->ReplaceAnimObject(i, NULL);  // 1個だけなのでブレンドしない
            m_blend->SetWeight(i, 1.0f);
            m_state = STATE_ONE;
          }
          else
          {
            // 2個以上ある
            f32 f = 1.0f;
            f32 weight = w * f;
            //weight = gfl::math::Clamp(weight, 0.0f, 1.0f);
            m_blend->SetWeight(i, weight);
          }
       
          m_num = i +1;
          break;
        }
        else
        {
          // [i]の設定
          f32 f = 1.0f;  // ( i == m_num -1 )のときはf=1
          if( i < m_num -1 )
          {
            f = static_cast<f32>(m_pack_array[i].frame_count) / static_cast<f32>(m_pack_array[i].frame_max);
          }
          f32 weight = w * f;
          //weight = gfl::math::Clamp(weight, 0.0f, 1.0f);
          m_blend->SetWeight(i, weight);
          
          m_pack_array[i].frame_count += 1;
          w *= (1.0f - f);
        }
      }
    }
    break;
  }
}

// 現在のアニメーションを得る
// 必ず毎フレームCalculateの後で呼び、呼び出し元で使用しているアニメーションのポインタを変更して下さい。
// Calculateでそのアニメーションのポインタを手放してしまうかもしれないので、Calculateの前には呼ばないこと。
nw::gfx::AnimObject* UnitAnimation::GetNwAnimObject(void) const
{
  nw::gfx::AnimObject* anim_object = NULL;
  switch( m_state )
  {
  case STATE_ONE:
    {
      anim_object = m_pack_array[0].animation->GetNwAnimObject();
    }
    break;
  case STATE_OTHERS_BLEND:
  case STATE_SKELETAL_BLEND:
    {
      anim_object = m_blend;
    }
    break;
  }
  return anim_object;
}

b32 UnitAnimation::BindBlend(nw::gfx::AnimGroup* nw_anim_group)
{
  b32 bind_result = false;
  if( m_blend )
  {
    if( m_blend_bind_anim_group )
    {
      // 既にバインド済み
      if( m_blend_bind_anim_group != nw_anim_group )
      {
        // 異なるものがバインドされている
        m_blend->Release();
        m_blend_bind_anim_group = NULL;
        bind_result = m_blend->Bind(nw_anim_group);
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
      bind_result = m_blend->Bind(nw_anim_group);
    }

    if( bind_result )
    {
      m_blend_bind_anim_group = nw_anim_group;
    }
  }
  return bind_result;
}

// アニメーションを変更する
void UnitAnimation::ChangeAnimation(
    Animation*  animation,
    u32         change_frame
)
{
  if( animation )
  {
    if( m_num_max > 0 )
    {
      ResAnim::Type type = animation->GetType();
      if( m_type == type )
      {
        if( change_frame == 0 || m_num_max == 1 || m_state == STATE_NONE ) 
        {
          // 直ちに変更する場合
          switch( m_state )
          {
          case STATE_NONE:
            {
              // 1個目のアニメーションなので、change_frameは使用せず直ちにアニメーションを適用する
              GFL_ASSERT_MSG(m_num == 0, "アニメーションの数が%dなので、0でないため異常です。\n", m_num);  //@check
            }
            break;
          case STATE_OTHERS_BLEND:
          case STATE_SKELETAL_BLEND:
            {
              for( s32 i=0; i<m_num; ++i )
              {
                m_blend->ReplaceAnimObject(i, NULL);  // 1個だけなのでブレンドしない
                m_blend->SetWeight(i, 1.0f);
              } 
            }
            break;
          }
       
          m_state = STATE_ONE;
          m_num = 1;
          m_pack_array[0].frame_count = 0;
          m_pack_array[0].frame_max   = 0;
          m_pack_array[0].animation   = animation;
        }
        else
        {
          // 何フレームかかけて変更する場合
          // ( change_frame > 0 && m_num_max > 1 && m_stateは既に1個以上ある状態 )
          switch( m_state )
          {
          case STATE_ONE:
          case STATE_OTHERS_BLEND:
          case STATE_SKELETAL_BLEND:
            {
              // 古いアニメーションを1つずつずらす
              s32 num = m_num +1;
              if( num > m_num_max ) num = m_num_max;
              for( s32 i=num-1; i>0; --i )
              {
                f32 weight = m_blend->GetWeight(i-1);
                m_pack_array[i] = m_pack_array[i-1];
                m_blend->ReplaceAnimObject(i, m_pack_array[i].animation->GetNwAnimObject());
                m_blend->SetWeight(i, weight);
              }
       
              // 新しいアニメーションを設定する
              m_pack_array[0].frame_count = 0;
              m_pack_array[0].frame_max   = change_frame;
              m_pack_array[0].animation   = animation;
              m_blend->ReplaceAnimObject(0, m_pack_array[0].animation->GetNwAnimObject());
              m_blend->SetWeight(0, 0.0f);
              
              m_num = num;
              if( m_type == ResAnim::TYPE_SKELETAL )
              {
                m_state = STATE_SKELETAL_BLEND;
              }
              else
              {
                m_state = STATE_OTHERS_BLEND;
              }
            }
            break;
          }
        }
      }
      else
      {
        GFL_ASSERT_MSG(0, "アニメーションのタイプが異なります。保持しているタイプ%d、追加するタイプ%dになっています。\n", m_type, type);
      }
    }
    else
    {
      GFL_ASSERT_MSG(0, "アニメーションの最大数が%dなので、変更できません。\n", m_num_max);
    }
  }
  else
  {
    // @note change_frameを考慮しながらアニメーションをはずすなんてことはできるのか？
    //       もしできるならやってみたいが。
    //       NULL=Tポーズという解釈で、Tポーズとのブレンドになるのだろうか？
    Destroy();
  }
}

// アニメーションのタイプを得る
ResAnim::Type UnitAnimation::GetType(void) const
{
  return m_type;
}

// 現在くっついているアニメーションの数(重み0でもくっついていれば数に入れる)
s32 UnitAnimation::GetAnimationNum(void) const
{
  return m_num;
}

// フレーム制御
void UnitAnimation::SetFrame(const f32 frame)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
f32  UnitAnimation::GetFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->GetFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return 0.0f;
}

void UnitAnimation::SetStepFrame(const f32 step_frame)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetStepFrame(step_frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
f32  UnitAnimation::GetStepFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->GetStepFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return 0.0f;
}

void UnitAnimation::SetStartFrame(const f32 frame)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetStartFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
f32  UnitAnimation::GetStartFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->GetStartFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return 0.0f;
}
void UnitAnimation::SetEndFrame(const f32 frame)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetEndFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
f32  UnitAnimation::GetEndFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->GetEndFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return 0.0f;
}
void UnitAnimation::SetStartAndEndFrame(const f32 start_frame, const f32 end_frame)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetStartAndEndFrame(start_frame, end_frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
b32  UnitAnimation::IsFrameStartFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->IsFrameStartFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return false;
}
b32  UnitAnimation::IsFrameEndFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->IsFrameEndFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return false;
}
b32  UnitAnimation::IsFrameStepTerminalFrame(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->IsFrameStepTerminalFrame();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return false;
}

void UnitAnimation::SetLoop(b32 loop)
{
  if( m_num > 0 )
  {
    m_pack_array[0].animation->SetLoop(loop);
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
}
b32  UnitAnimation::IsLoop(void) const
{
  if( m_num > 0 )
  {
    return m_pack_array[0].animation->IsLoop();
  }
  else
  {
    GFL_ASSERT_MSG(0, "アニメーションの数が%dなので、異常です。\n", m_num);
  }
  return false;
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

