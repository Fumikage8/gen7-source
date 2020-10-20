#ifndef __GFL_G3DANIMATIONMANAGER_H__
#define __GFL_G3DANIMATIONMANAGER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimationManager.h
 *	@brief  3Dアニメーションを割り当てるスロットの管理
 *	@author	Koji Kawada
 *	@date		2011.10.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>
#include <gfl_Math.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dUnitAnimation.h>
#include <grp/g3d/gfl_G3dAnimationSlot.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	アニメーションを割り当てるスロットの管理
//=====================================
class AnimationManager
{
  GFL_FORBID_COPY_AND_ASSIGN(AnimationManager);

  friend class Model;
  friend class Camera;
  friend class StereoCamera;
  friend class Light;


  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  // 
  // どこからでもアクセスできるメンバ
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
public:
  // 定数
  static const u32 DEFAULT_SLOT_NUM = 3;
  
  static const s32 DEFAULT_OWN_SLOT_BLEND_NUM_MAX     = 2;
  static const s32 DEFAULT_OWN_UNIT_ANIMATION_NUM_MAX = 2; 

public:
  // 設定内容
  struct Description
  {
    u32                         slot_num;
    AnimationSlot::Description* slot_description_array;
    b32                         use_own_animation;
    s32                         own_slot_blend_num_max;
    s32                         own_unit_animation_num_max;
    Description(void)
      : slot_num(DEFAULT_SLOT_NUM),
        slot_description_array(NULL),
        use_own_animation(true),
        own_slot_blend_num_max(DEFAULT_OWN_SLOT_BLEND_NUM_MAX),
        own_unit_animation_num_max(DEFAULT_OWN_UNIT_ANIMATION_NUM_MAX)
    {
      // 何もしない
    }
  };

public:
  // objectのmax_membersを得る
  // objectがSkeletalModelのときはボーンの数とも比較している
  static s32 GetMaxMembers( Object* object, ResAnim::Type type );




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friendクラスからしかアクセスしてはならないメンバ関数
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  static nw::gfx::AnimGroup* GetNwAnimGroupAndMemberCount( Object* object, ResAnim::Type type, s32* member_count );

private:
  // コンストラクタ
  AnimationManager(void);
  // デストラクタ
  virtual ~AnimationManager();

  // 生成
  void Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Object*                 object,
      Description*            description = NULL
  );

  // UseOwn用
  void UseOwn_Create(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      Object*                 object,
      u32                     slot_num,
      u32                     unit_num,
      u32                     animation_num_in_unit
  );

  // 破棄
  virtual void Destroy(void);

  // 計算処理
  // 必ず毎フレーム呼んで下さい。
  virtual void Calculate(void);

  // スロット数を取得
  u32 GetSlotNum(void) const
  {
    return m_slot_num;
  }

private:
  // スロット
  void CreateSlot(
    u32                         slot_index,
    gfl::heap::NwAllocator*     heap_allocator,
    gfl::heap::NwAllocator*     device_allocator,
    AnimationSlot::Description* slot_description_array = NULL
  );
  ResAnim::Type GetSlotType(
    u32                         slot_index
  ) const;

  // @note AnimationSlot::GetAnimationNumの戻り値を修正しようかと思っているので、
  // privateメンバで。
  // この戻り値を直接使うのは、その修正が終わるまで待って下さい。
  s32 GetSlotAnimationNum(
    u32                         slot_index
  ) const;


  // スロットの有効/無効
  b32 IsSlotEnable(u32 slot_index) const;
  void SetSlotEnable(u32 slot_index, b32 enable);


  // ResAnim::Typeのアニメーションが割り当てられるAnimGroupをオリジナル値にリセットする。
  // ResAnim::Typeのアニメーションが割り当てられるAnimGroupにアニメーションが存在する場合、そのアニメーションが動かすメンバはリセットされない。
  // ResAnim::Typeのアニメーションが割り当てられるAnimGroupにアニメーションが存在しない場合、全てのメンバがリセットされる。
  void Reset(ResAnim::Type type);
  // スロットにアニメーションが割り当てられている場合、そのアニメーションのResAnim::Typeに対してResetを行う。
  // スロットにアニメーションが割り当てられていない場合、何もしない。
  void ResetSlot(u32 slot_index);


  // UseOwn用
  void UseOwn_ChangeAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     slot_index,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members,
    b32                     force = false
  );
  void UseOwn_BlendAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      u32                        slot_index,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );

  // フレーム操作
  void SetFrame(u32 slot_index, f32 frame);
  f32  GetFrame(u32 slot_index) const;
  void SetStepFrame(u32 slot_index, f32 step_frame);
  f32  GetStepFrame(u32 slot_index) const;
  void SetStartFrame(u32 slot_index, f32 frame);
  f32  GetStartFrame(u32 slot_index) const;
  void SetEndFrame(u32 slot_index, f32 frame);
  f32  GetEndFrame(u32 slot_index) const;
  void SetStartAndEndFrame(u32 slot_index, f32 start_frame, f32 end_frame);
  b32  IsFrameStartFrame(u32 slot_index) const;
  b32  IsFrameEndFrame(u32 slot_index) const;
  b32  IsFrameStepTerminalFrame(u32 slot_index) const;
  b32  IsLoop(u32 slot_index) const;
  void SetLoop(u32 slot_index, b32 loop);

  // UseOwn用
  void UseOwn_SetUnitFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStepFrame(u32 slot_index, u32 unit_index, f32 step_frame);
  f32  UseOwn_GetUnitStepFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStartFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitStartFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitEndFrame(u32 slot_index, u32 unit_index, f32 frame);
  f32  UseOwn_GetUnitEndFrame(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitStartAndEndFrame(u32 slot_index, u32 unit_index, f32 start_frame, f32 end_frame);
  b32  UseOwn_IsUnitFrameStartFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitFrameEndFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitFrameStepTerminalFrame(u32 slot_index, u32 unit_index) const;
  b32  UseOwn_IsUnitLoop(u32 slot_index, u32 unit_index) const;
  void UseOwn_SetUnitLoop(u32 slot_index, u32 unit_index, b32 loop);

  // ブレンド操作
  void SetBlendWeight(u32 slot_index, u32 unit_index, f32 weight);
  f32  GetBlendWeight(u32 slot_index, u32 unit_index);




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friendクラスからもアクセスしてはならないメンバ関数
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  void SetAnimObjectAllSlot(b32 is_not_null);  // is_not_nullがfalseのときNULLを設定する
  void SetAnimObject(u32 slot_index, b32 is_not_null);  // is_not_nullがfalseのときNULLを設定する
      // SetAnimObjectAllSlotとSetAnimObjectは似ているがスピード重視のため用意した。




  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  //  
  // friendクラスからもアクセスしてはならないメンバ変数
  // 
  /////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////
private:
  Object*         m_object;  // このAnimationManagerをメンバに持つオブジェクトのポインタを覚えておく


  // AnimationSlot
  u32             m_slot_num;
  AnimationSlot*  m_slot_array;


  // マネージャ所有アニメーション(マネージャが生成、破棄を行う)
  b32              m_use_own_animation;
  s32              m_own_slot_blend_num_max;  // 各SlotにあるUnitAnimationの個数
  s32              m_own_unit_animation_num_max;  // 各SlotにあるUnitAnimationに登録できるAnimationの個数
  s32              m_own_unit_animation_num_over;  // = m_own_unit_animation_num_max+1
                                                   // 各UnitAnimationに登録できるAnimationの個数より大きい値
                                                   // UnitAnimationにてAnimationの入れ替えができるように、登録済みがm_own_unit_animation_num_max個、
                                                   // オーバー分がこれから登録して追い出す役目になるようにしている。
  b32*             m_own_set_slot_array;   // スロットのブレンド方式を指定したことがある場合はtrue、ない場合はfalse
  Animation****    m_own_animation_array;  // 全Slot、全UnitAnimationを合計したAnimationを多次元配列で区切る
                                           // 各UnitAnimationに登録できるAnimationの個数より1多くしUnitAnimationにて入れ替えができるようにしている。
                                           // m_own_animation_array[m_slot_num][m_own_slot_blend_num_max][m_own_unit_animation_num_over];
  UnitAnimation*** m_own_unit_animation_array;  // 全SlotのUnitAnimationを多次元配列で区切る
                                                // m_own_unit_animation_array[m_slot_num][m_own_slot_blend_num_max];
                                                // m_own_unit_animation_array[i][j]にはm_own_unit_animation_num_max個のAnimationが登録できる。
  s32**            m_own_unit_animation_next_index_array;  // 各UnitAnimationに登録するAnimationが次はどれかを示すインデックス
                                                           // m_own_unit_animation_array[m_slot_num][m_own_slot_blend_num_max];
                                                           // m_own_unit_animation_array[i][j]に登録する次のアニメーションは
                                                           // m_own_animation_array[i][j][ m_own_unit_animation_next_index_array[i][j] ]であり
                                                           // それを登録したらm_own_unit_animation_next_index_array[i][j]は
                                                           // (m_own_unit_animation_next_index_array[i][j]+1)%(m_own_unit_animation_num_over)になる。
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DANIMATIONMANAGER_H__

