#ifndef __GFL_G3DANIMATION_H__
#define __GFL_G3DANIMATION_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dAnimation.h
 *	@brief  3Dアニメーション
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
#include <grp/g3d/gfl_G3dIAnimation.h>
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dResAnim.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dアニメーションクラス
//=====================================
class Animation : public IAnimation
{
  GFL_FORBID_COPY_AND_ASSIGN(Animation);

  friend class UnitAnimation;
  friend class AnimationSlot;
  friend class AnimationManager;

public:
  struct AnimationPack
  {
    Animation* animation;
    f32        weight;
  };

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  Animation(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~Animation();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを生成する
   *
   *  @param[in]     device_allocator         デバイスアロケータ
   *  @param[in]     res_anim                 アニメーションリソース
   *  @param[in]     resource                 リソース
   *  @param[in]     animation_resource_type  アニメーションリソースのタイプ
   *  @param[in]     index_in_resource        リソース内におけるインデックス
   *  @param[in]     max_members              アニメーション対象メンバの最大数
   *                                          TransformAnimEvaluator::Bind に渡す AnimGroup の AnimGroup::GetMemberCount の値を設定してください。
   *                                          複数の AnimGroup に Bind する場合は、最大値を設定してください。
   *  @param[in]     max_anim_members         実際にアニメーションするメンバの最大数を設定します。
   *                                          AnimData() に渡す anim::res::ResAnim の anim::res::ResAnim::GetMemberAnimSetCount の値を設定してください。
   *                                          AnimEvaluator::ChangeAnim で複数の ResAnim を切り替える場合は、最大値を設定してください。
   *                                          gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                          ここで渡したres_anim(resource)のGetMemberAnimSetCountを使用します。
   *                                          gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                          ここで渡したmax_membersにします。
   * 
   *  ここにあるChangeは、次のどれかの処理を行います。
   *  ・既に生成されているアニメーションと同一タイプであるなら、新たに生成することなくアニメーションを変更する(ここより後にあるChangeと同等の処理)。
   *  ・既に生成されているアニメーションと異なるタイプであるなら、Destroyした後にCreateする。
   *  ・アニメーションが生成されていないなら、Createする。
   */
  //-----------------------------------------------------------------------------
  void CreateAnimation(
    gfl::heap::NwAllocator*  device_allocator,
    ResAnim*                 res_anim,
    s32                      max_members,
    s32                      max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator*  device_allocator,
    ResAnim*                 res_anim,
    s32                      max_members,
    s32                      max_anim_members
  );
  void CreateAnimation(
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    s32                              max_members,
    s32                              max_anim_members
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator*          device_allocator,
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource,
    s32                              max_members,
    s32                              max_anim_members
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         補間ブレンドアニメーションを生成する
   *  @brief         加算ブレンドアニメーションを生成する
   *
   *  @param[in]     heap_allocator           デバイスアロケータ
   *  @param[in]     device_allocator         デバイスアロケータ
   *  @param[in]     type                     アニメーションリソースのタイプ
   *                                          pack_arrayで渡すアニメーションのタイプは全てこのタイプになっていなければならない
   *  @param[in]     num_max                  ブレンドするアニメーションの最大数
   *                                          1のとき何もブレンドされないのでブレンド用のAnimationを生成しないほうがよい
   *  @param[in]     num                      pack_arrayの要素数
   *                                          num<=num_max
   *                                          0を渡してもよい
   *  @param[in]     pack_array               num個の要素をもった配列
   *                                          この配列の順番にインデックスが割り当てられる(pack_array[n].animationならn番)
   *                                          numが0のときはNULLを渡してもよい
   *                                          pack_arrayに列挙したanimationは置き換えない限りこのクラスでポインタを保持しているので、
   *                                          呼び出し元で破棄しないで下さい。animationさえ残っていればpack_array配列は破棄しても構いません。
   *  @param[in]     ignore_no_anim_member    アニメーションの存在しないメンバを無視する場合はtrue
   *  @param[in]     normalization_enable     アニメーションのブレンド重みを正規化する場合はtrue
   *
   *  ここにあるChangeは、次のどれかの処理を行います。
   *  ・既に生成されているアニメーションと同一タイプであるなら、新たに生成することなくアニメーションを変更する(ここより後にあるChangeと同等の処理)。
   *  ・既に生成されているアニメーションと異なるタイプであるなら、Destroyした後にCreateする。
   *  ・アニメーションが生成されていないなら、Createする。
   */
  //-----------------------------------------------------------------------------
  void CreateBlendInterpolator(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array,
    b32                     ignore_no_anim_member = false,
    b32                     normalization_enable  = true
  );
  void ChangeBlendInterpolator(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array,
    b32                     ignore_no_anim_member = false,
    b32                     normalization_enable  = true
  );
  void CreateBlendAdder(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array
  );
  void ChangeBlendAdder(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max,
    s32                     num,
    AnimationPack*          pack_array
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを破棄する:
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *
   *  @param[in]     res_anim                 アニメーションリソース
   *  @param[in]     resource                 リソース
   *  @param[in]     animation_resource_type  アニメーションリソースのタイプ
   *  @param[in]     index_in_resource        リソース内におけるインデックス
   *
   *  ここにあるChangeは、既に生成されているアニメーションと同一タイプでなければ使用できません。
   */
  //-----------------------------------------------------------------------------
  void ChangeAnimation(
    ResAnim*                 res_anim
  );
  void ChangeAnimation(
    Resource*                        resource,
    Resource::AnimationResourceType  animation_resource_type,
    s32                              index_in_resource
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         補間ブレンドアニメーションを変更する
   *  @brief         加算ブレンドアニメーションを変更する
   *
   *  @param[in]     num                      pack_arrayの要素数
   *                                          num<=m_blend_num_max
   *                                          0を渡してもよい
   *  @param[in]     pack_array               num個の要素をもった配列
   *                                          この配列の順番にインデックスが割り当てられる(pack_array[n].animationならn番)
   *                                          numが0のときはNULLを渡してもよい
   *                                          pack_arrayに列挙したanimationは置き換えない限りこのクラスでポインタを保持しているので、
   *                                          呼び出し元で破棄しないで下さい。animationさえ残っていればpack_array配列は破棄しても構いません。
   *  @param[in]     normalization_enable     アニメーションのブレンド重みを正規化する場合はtrue
   * 
   *  ここにあるChangeは、既に生成されているアニメーションと同一タイプでなければ使用できません。
   */
  //-----------------------------------------------------------------------------
  void ChangeBlendInterpolator(
    s32                     num,
    AnimationPack*          pack_array,
    b32                     normalization_enable  = true
  );
  void ChangeBlendAdder(
    s32                     num,
    AnimationPack*          pack_array
  );
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのタイプを得る
   *
   *  @retval        Type     アニメーションのタイプ
   */
  //-----------------------------------------------------------------------------
  virtual ResAnim::Type GetType(void) const  {  return m_type;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを設定する
   *
   *  @param[in]     frame      フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを設定する
   *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
   *
   *  @param[in]     step_frame      更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを取得する
   *
   *  @retval        更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを設定する
   *
   *  @param[in]     frame      フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを設定する
   *
   *  @param[in]     frame      フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームと終了フレームを設定する
   *
   *  @param[in]     start_frame    開始フレーム
   *  @param[in]     end_frame      終了フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが開始フレームか否か判定する
   *
   *  @retval        bool    現在のフレームが開始フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
   *
   *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームがstep_frameから見て端フレームか否か判定する
   *
   *  @retval        bool    現在のフレームがstep_frameから見て端フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsFrameStepTerminalFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *
   *  @retval        b32       ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  virtual b32 IsLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *
   *  @param[in]     loop    ループのときtrue、ループでないときfalseを設定する
   */
  //-----------------------------------------------------------------------------
  virtual void SetLoop(b32 loop);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのブレンド重みを設定する
   *
   *  @param[in]     index   ブレンドしているアニメーションのインデックス
   *  @param[in]     weight  ウェイト
   *
   *  @retval        trueのとき設定できた、falseのとき設定できていない(理由はindexにアニメーションがないなど)
   */
  //-----------------------------------------------------------------------------
  b32 SetBlendWeight(s32 index, f32 weight);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのブレンド重みを取得する
   *
   *  @param[in]     index   ブレンドしているアニメーションのインデックス
   * 
   *  @retval        ウェイト
   */
  //-----------------------------------------------------------------------------
  f32 GetBlendWeight(s32 index) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ブレンドするアニメーションを追加する
   *
   *  @param[in]     index       animationを割り当てるインデックス
   *  @param[in]     animation   追加するアニメーション
   *                             animationは置き換えない限りこのクラスでポインタを保持しているので、
   *                             呼び出し元で破棄しないで下さい。
   *                             NULLのとき、indexに割り当てられているアニメーションをなしにする
   *  @param[in]     weight      ウェイト
   *                             animationがNULLのときはweightは使用しないのでどんな値でも構わない
   * 
   *  @retval        追加できたときtrueを返す
   */
  //-----------------------------------------------------------------------------
  b32 SetBlendAnimation(s32 index, Animation* animation, f32 weight);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         補間ブレンアニメーションのブレンド重みの正規化の設定/取得
   *
   *  @param[in]     enable      trueのとき正規化する
   * 
   *  @retval        trueのとき正規化する
   */
  //-----------------------------------------------------------------------------
  void SetBlendInterpolatorNormalizationEnable(b32 enable);
  b32  IsBlendInterpolatorNormalizationEnable(void) const;

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスが保持しているnw::gfx::AnimObject*を取得
   *
   *  @retval        nw::gfx::AnimObject*    このクラスが保持しているnw::gfx::AnimObject
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::AnimObject* GetNwAnimObject(void) const  {  return m_anim_object;  }

  // アニメーションの数(ブレンドアニメーションのときはその中身が空でも1を返す)
  virtual s32 GetAnimationNum(void) const; 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを関連付ける
   *
   *  @param[in]     nw_anim_group      アニメーショングループ
   *  @retval        b32           バインドに成功したらtrueを返す
   *
   *  m_nw_anim_objectがnw_anim_groupを覚えておくようになる。
   */
  //-----------------------------------------------------------------------------
  b32 Bind(nw::gfx::AnimGroup* nw_anim_group);

private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         max_anim_membersを求める
   *
   *  @param[in]     res_anim
   *  @param[in]     max_members
   *  @param[in]     max_anim_members  もともとの値
   *
   *  @retval        max_anim_members  求めなおした値
   */
  //-----------------------------------------------------------------------------
  static s32 GetMaxAnimMembers(ResAnim* res_anim, const s32 max_members, const s32 max_anim_members);

private:
  // 状態
  enum State
  {
    STATE_NONE,                          // アニメーションを生成していない
    STATE_EVALUATOR_SKELETAL,            // スケルタルアニメーションの評価クラスを生成している
    STATE_EVALUATOR_OTHERS,              // スケルタルアニメーション以外の評価クラスを生成している
    STATE_BLEND_INTERPOLATOR_SKELETAL,   // スケルタルアニメーションの補間ブレンドクラスを生成している
    STATE_BLEND_INTERPOLATOR_OTHERS,     // スケルタルアニメーション以外の補間ブレンドクラスを生成している
    STATE_BLEND_ADDER_SKELETAL,          // スケルタルアニメーションの加算ブレンドクラスを生成している
    STATE_BLEND_ADDER_OTHERS             // スケルタルアニメーション以外の加算ブレンドクラスを生成している
  };

private:
  // 全てで使用するメンバ変数
  State                            m_state;
  ResAnim::Type                    m_type;

  // m_stateが
  // STATE_EVALUATOR_SKELETAL
  // STATE_EVALUATOR_OTHERS
  // のとき使用するメンバ変数
  s32                              m_max_members;
  s32                              m_max_anim_members;

  // m_stateが
  // STATE_BLEND_INTERPOLATOR_SKELETAL
  // STATE_BLEND_INTERPOLATOR_OTHERS
  // STATE_BLEND_ADDER_SKELETAL
  // STATE_BLEND_ADDER_OTHERS
  // のとき使用するメンバ変数
  s32                              m_blend_num_max;
  Animation**                      m_blend_animation_array;  // 他のところで生成したアニメーションを覚えているだけのポインタ
                                                             // 覚えておいても使わないのでなくてもよさそう、なくすならheap_allocatorを渡さなくて済む
  
  // 全てで使用するメンバ変数
  nw::gfx::AnimObject*             m_anim_object;         // アニメーション評価クラス or アニメーションブレンドクラス
  nw::gfx::AnimGroup*              m_bind_anim_group;     // バインドしているアニメーショングループを覚えておく。NULLのときバインドしていない。
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DANIMATION_H__
