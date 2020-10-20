//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationManager.cpp
 *	@brief  3Dアニメーションを割り当てるスロットの管理
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
///	アニメーションを割り当てるスロットの管理
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

        // モデルからアニメーショングループを取得します。
        //
        // アニメーショングループはアニメーション対象メンバへのポインタを保持します。 
        // 対象メンバとは、モデルがもつアニメーション可能な要素
        //（例えば、マテリアルアニメーションでは Diffuse や Texture など）です。
        // ここでは、スケルタルモデルから、ボーンのトランスフォームを含む
        // アニメーショングループを取得します。
        //
        // アニメーションの対象や種類によって用いる関数が異なります。
        // ・SkeletalModel::GetSkeletalAnimGroup
        // ・Model::GetVisibilityAnimGroup
        // ・Model::GetMaterialAnimGroup
        // ・Light::GetAnimGroup
        // ・Camera::GetAnimGroup
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
          if( max_members < bones_count ) max_members = bones_count;  // 多いほうを採用すればいい
          //GFL_PRINT("anim_group_member_count=%d, bones_count=%d\n",anim_group_member_count, bones_count);
          // 出力結果 anim_group_member_count=52, bones_count=52 スケルタルアニメならこの両者は同じ値になると思っていいのかな？
        }
        else
        {
          GFL_ASSERT_MSG(0, "アニメーショングループがありません。\n");
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
            GFL_ASSERT_MSG(0, "アニメーショングループがありません。\n");
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
          GFL_ASSERT_MSG(0, "アニメーショングループがありません。\n");
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
          GFL_ASSERT_MSG(0, "アニメーショングループがありません。\n");
        }
      }
    }
    break;
  default:
    {
      GFL_ASSERT_MSG(0, "オブジェクトの種類%dが異常です。", object_type);
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
      m_own_unit_animation_num_over = (m_own_unit_animation_num_max==1)?(1):(m_own_unit_animation_num_max+1);  // 1個しかないなら補間しながら変更はしない
    
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
    
    // @note ownを使用するときは、外からAnimationやUnitAnimationを渡せないようにするかどうしようか
  }
}

// UseOwn用
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

  // まだ何もアニメーションを生成していないので呼ばない
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    @note 未解決  2011年12月1日現在
    成分が0のとき、Resetで何かフラグを立てているっぽい。
    そのフラグのせいで、1フレームだけ元に戻ってしまっているようだ。
    そのフラグは次のフレームに行くまで誰もリセットしないみたい。
    以下の実験では、その1フレームもなくなったが何も動かなくなってしまった。
    ちなみに、そのフラグとはFLAG_IS_TRANSLATE_ZEROのような_ZERO系かな。
    スケルタル以外のアニメ(マテリアル、ビジビリティ、カメラ、ライト)も対処する必要がありそう。
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
            //skeleton->SetUpdated(false);意味なしだった

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

/*nw::gfx::BaseAnimEvaluator::Reset メンバ関数
説明
マテリアルに対して行う場合、マテリアルのバッファを生成する必要があります。 
SceneBuilder クラスでモデルを作成する際のオプションで、 SceneBuilder::BufferOption を設定してください。 
BufferOption には、 Model::MULTI_FLAG_ANIMATABLE_MATERIAL の内容が最低限必要です。*/

/*CTR グラフィックス質問部屋
[質問][NW4C] Material と Visibility のアニメーションの停止の方法について
田中＠NTSC 2011-04-11 16:52:15  
>Material と Visibility アニメで表情変化や怒りマークの表示を行っているのですが、これをアニメ再生前の基本の表情に戻したい場合、BaseAnimEvaluator::Reset を実行すれば良いのでしょうか？
はい。その関数でリセット可能です。
アニメーション可能メンバ以外もリセットしたいのであればAnimGroup::Resetを使う手もあります。*/

/*質問部屋には上記のように回答してあったが
nw::gfx::BaseAnimEvaluatorのResetと
nw::gfx::AnimGroupのResetは
中身が同じだった。

nw::gfx::BaseAnimEvaluatorのResetはAnimGroupがBindしてないと使用できない。
アニメーションが全くBindされていないときにTポーズに戻すためにリセットすることも考慮して
nw::gfx::AnimGroupのResetの方を使うことにした。*/

#if 0
gfx_BaseAnimEvaluator.h
gfx_BaseAnimEvaluator.cpp

class BaseAnimEvaluator

    //! @brief すべてのアニメーション対象メンバーを生成時の状態に戻します。
    //!
    //! マテリアルに対して行う場合、マテリアルのバッファを生成する必要があります。
    //! SceneBuilder クラスでモデルを作成する際のオプションで、
    //! SceneBuilder::BufferOption を設定してください。
    //! BufferOption には、 Model::MULTI_FLAG_ANIMATABLE_MATERIAL の内容が最低限必要です。
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
  }
}

// UseOwn用
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
    // アニメーション
    /////////////////////////////////////
    if( change_frame == 0 )
    {
      // change_frameを常に0にしている場合、不必要なアニメーションをつくらなくても済むように、インデックス0ばかりを使用する。
      // change_frameを1度でも0より大にした場合、インデックスは0以外になっているので、インデックス0ばかりを使用するというわけにはいかなくなっている。
      if( m_own_unit_animation_next_index_array[slot_index][unit_index] == 1 )
      {
        m_own_unit_animation_next_index_array[slot_index][unit_index] = 0;
      }
    }
    // 必要なときになって初めてメモリを確保する
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
    // アニメーションの関連付け
    b32 bind_result = m_own_animation_array[slot_index][unit_index][ m_own_unit_animation_next_index_array[slot_index][unit_index] ]->Bind(anim_group);
    // Animationが「m_objectのanim_group」のどれも動かさないとき、bind_resultはfalseになる。
    // アニメーションAをモデルb、モデルc、モデルdにバインドするとき、モデルdだけアニメーションAで動くものがなかったとすると、
    // モデルdにバインドしたときだけbind_resultはfalseになる。
    // 同じようなつくりのマップに、あらかじめつくっておいた全マップ共通のアニメーションをバインドするときに、このようなことが起こることがある。
    // この例のモデルb、モデルc、モデルdは同時に存在するわけではない。
    // モデルb、モデルc、モデルdが同時に存在し、その3者全てに唯一のアニメーションAをバインドしたときどうなるかは試していないので不明。
    // 最後にバインドしたモデルだけをアニメーションは覚えていそうなので、アニメーションを複数のモデルにバインドできるとは思えない。
    //
    // ↑のAnswer
    // gfx_AnimBlender.h
    // virtual Result TryBind(AnimGroup* animGroup)
    // に
    // NW_NULL_ASSERT(animGroup);
    // NW_ASSERT(m_AnimGroup == NULL);
    // とあるので、
    // アニメーションにNULLもバインドできないし、既にバインドされてる場合は新しいものをバインドできないようになっている。
    if( !force )  // forceがtrueのときは、上記のような状況でアサートにしない。
    {
      GFL_ASSERT_MSG(bind_result, "アニメーションの関連付けに失敗しました。\n");  //@check バインドに失敗しても続けられる。
    }
 
    /////////////////////////////////////
    // アニメーションの単位となるもの
    /////////////////////////////////////
    // 必要なときになって初めてメモリを確保する
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
    // アニメーションの関連付け
    m_own_unit_animation_array[slot_index][unit_index]->BindBlend(anim_group);
 
    /////////////////////////////////////
    // スロット
    /////////////////////////////////////
    // スロットのブレンド方式を指定していないうちは、最後にChangeAnimationで割り当てたアニメーションをスロットのアニメーションとする。
    // よって、どのunit_indexのアニメーションが割り当てられているかは、どのunit_indexから設定していったかに依る。
    if( !m_own_set_slot_array[slot_index] )
    {
      // まだスロットのブレンド方式を指定していない
      m_slot_array[slot_index].ChangeAnimation(
          m_own_unit_animation_array[slot_index][unit_index]
      );
    }
    // アニメーションの関連付け
    m_slot_array[slot_index].BindBlend(anim_group);
 
    m_own_unit_animation_next_index_array[slot_index][unit_index] = (m_own_unit_animation_next_index_array[slot_index][unit_index]+1)%(m_own_unit_animation_num_over);
  }
  else
  {
    /////////////////////////////////////
    // スロット
    /////////////////////////////////////
    if(
           m_slot_array[slot_index].GetState() != AnimationSlot::STATE_NONE
        && m_slot_array[slot_index].GetState() != AnimationSlot::STATE_ONE
    )
    {
      m_slot_array[slot_index].SetBlendAnimation(unit_index, NULL, 1.0f);
    }

    /////////////////////////////////////
    // アニメーションの単位となるもの
    /////////////////////////////////////
    if( m_own_unit_animation_array[slot_index][unit_index] )
    {
      m_own_unit_animation_array[slot_index][unit_index]->ChangeAnimation(
        NULL,
        0  // change_frame=0とし直ちにアニメーションを破棄する
      );
    }

    /////////////////////////////////////
    // アニメーション
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
      m_own_set_slot_array[slot_index] = false;  // スロットのブレンド方式を指定していない状態に戻す
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
      // タイプを最初に見付けたものにしておく
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

        // アニメーションの関連付け
        m_slot_array[slot_index].BindBlend(anim_group);

        m_own_set_slot_array[slot_index] = true;
      }
      else
      {
        GFL_ASSERT_MSG(0, "ブレンドするアニメーションがありません。\n");
      }
    }
    break;
  }

  SetAnimObjectAllSlot(true);
}

// フレーム操作
void AnimationManager::SetFrame(u32 slot_index, f32 frame)
{
  if( slot_index < m_slot_num )
  { 
    m_slot_array[slot_index].SetFrame(frame);
  }
  else
  {
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
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
    GFL_ASSERT_MSG(0, "slot_index=%dがm_slot_num=%d未満ではありません。\n", slot_index, m_slot_num);
  }
}

// UseOwn用
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

// ブレンド操作
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
            // AnimEvaluator をモデルに設定します。
            // AnimEvaluator は一つのモデルに対して複数設定することができ、
            // その際には、AnimEvaluator 毎に objectIndex を指定します。
            // 詳しくは、 PartialAnimationDemo を参照してください。
            //
            // アニメーションの対象や種類によって用いる関数が異なります。
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
            case ResAnim::TYPE_CAMERA:  // カメラとライトはスロットを持つ意味がない
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
            case ResAnim::TYPE_LIGHT:  // カメラとライトはスロットを持つ意味がない
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
        GFL_ASSERT_MSG(0, "保持しているオブジェクトの種類%dが異常です。", object_type);
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
            // AnimEvaluator をモデルに設定します。
            // AnimEvaluator は一つのモデルに対して複数設定することができ、
            // その際には、AnimEvaluator 毎に objectIndex を指定します。
            // 詳しくは、 PartialAnimationDemo を参照してください。
            //
            // アニメーションの対象や種類によって用いる関数が異なります。
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
            case ResAnim::TYPE_CAMERA:  // カメラとライトはスロットを持つ意味がない
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
            case ResAnim::TYPE_LIGHT:  // カメラとライトはスロットを持つ意味がない
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
        GFL_ASSERT_MSG(0, "保持しているオブジェクトの種類%dが異常です。", object_type);
      }
      break;
    }
  }
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

