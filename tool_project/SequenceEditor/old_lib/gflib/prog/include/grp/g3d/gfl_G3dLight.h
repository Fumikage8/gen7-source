#ifndef __GFL_G3DLIGHT_H__
#define __GFL_G3DLIGHT_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLight.h
 *	@brief  3Dライト
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>
#include <grp/g3d/gfl_G3dSceneNode.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dAnimationManager.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dライトクラス
//=====================================
class Light : public SceneNode
{
  GFL_FORBID_COPY_AND_ASSIGN(Light);

  friend class AnimationManager;
  friend class RenderSystem;
  friend class LightSet;

public:
  enum Type
  {
    TYPE_NONE,
    TYPE_AMBIENT,
    TYPE_FRAGMENT,
    TYPE_HEMI_SPHERE,
    TYPE_VERTEX
  };
  enum Kind
  {
    KIND_NONE,
    KIND_DIRECTIONAL,
    KIND_POINT,
    KIND_SPOT
  };

  // リソースからライトを生成するときの設定内容
  struct ResourceDescription
  {
    b32  animation_enable;          // アニメーションを行う場合はtrue(アニメーションを割り当てるスロットを1個つくります)
    u32  animation_blend_unit_num;  // スロット内でアニメーションのブレンドを行う場合2以上、行わない場合0か1(0でも1でもスロット内に1つユニットをつくります)
    u32  animation_num_in_unit;     // ユニット内で補間ブレンドしながらアニメーションを変更する場合2以上、行わない場合0か1(0でも1でもユニット内に1つつくります)
    ResourceDescription(void)
      : animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  static void InitializeResourceDescriptionForDefault(
      ResourceDescription*  description
  );

  // 設定内容
  struct Description
  {
    Type  type;
    Kind  kind;
    b32 animation_enable;          // アニメーションを行う場合はtrue(アニメーションを割り当てるスロットを1個つくります)
    u32 animation_blend_unit_num;  // スロット内でアニメーションのブレンドを行う場合2以上、行わない場合0か1(0でも1でもスロット内に1つユニットをつくります)
    u32 animation_num_in_unit;     // ユニット内で補間ブレンドしながらアニメーションを変更する場合2以上、行わない場合0か1(0でも1でもユニット内に1つつくります)
    Description(void)
      : type(TYPE_NONE),
        kind(KIND_NONE),
        animation_enable(false),
        animation_blend_unit_num(0),
        animation_num_in_unit(0)
    {}
  };
  static void InitializeDescriptionForDefault(
      Description*  description
  );
  static void InitializeDescriptionForAmbientLight(
      Description*  description
  );
  static void InitializeDescriptionForFragmentLight(
      Description*  description,
      const Kind    kind
  );
  static void InitializeDescriptionForHemiSphereLight(
      Description*  description
  );
  static void InitializeDescriptionForVertexLight(
      Description*  description,
      const Kind    kind
  );


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  Light(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~Light();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   *  @param[in]     description        リソースからライトを生成するときの設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator,
      gfl::heap::NwAllocator*           device_allocator,
      Resource*                         resource,
      s32                               index_in_resource,
      const ResourceDescription*        description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     description        設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           device_allocator,
      const Description*                description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         計算処理
   */
  //-----------------------------------------------------------------------------
  virtual void Calculate(void);


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションを変更する
   *                 初めてこの関数を呼んだときは、Animationを生成する。
   *
   *  @param[in]     allocator               アロケータ
   *  @param[in]     res_anim                アニメーションリソース
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。
   *                                         初めてこの関数を呼んだときは、使用される。
   *                                         Resource::GetLightMemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   */
  //-----------------------------------------------------------------------------
  virtual void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );

  void ChangeAnimation(
    gfl::heap::NwAllocator* allocator,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_indexのunit_indexに割り当てるアニメーションを変更する
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     slot_index              スロットのインデックス
   *  @param[in]     unit_index              スロット内につくったユニットのインデックス
   *  @param[in]     res_anim                アニメーションリソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     resource                リソース(NULLを渡すと全て(スケルタル、マテリアル、ビジビリティ)のアニメーションを解除します)
   *  @param[in]     animation_resource_type アニメーションリソースのタイプ(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     index_in_resource       リソース内におけるインデックス(resourceがNULLのときは使用されないのでてきとーな値でOK)
   *  @param[in]     change_frame            何フレームで変更するか(この間ブレンドされる)
   *  @param[in]     max_anim_members        アニメーションするメンバの最大数。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   *                                         Resource::Get???MemberAnimSetCountで得た値を渡すとよい。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRSTを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのGetMemberAnimSetCountを使用します。
   *                                         gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_MAX_MEMBERSを渡すと、
   *                                         最初にChangeで渡したアニメーションリソースのアニメーショングループのメンバ数に、
   *                                         即ち、そのアニメーションタイプにおける最大数にします。
   *  @param[in]     force                   強制的にアニメーションを関連付けます。(res_animがNULLのときは使用されないのでてきとーな値でOK)
   */
  //-----------------------------------------------------------------------------
  void ChangeUnitAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    u32                     unit_index,
    ResAnim*                res_anim,
    u32                     change_frame,
    s32                     max_anim_members = gfl::grp::g3d::ANIMATION_MAX_ANIM_MEMBERS_USE_FIRST
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションのフレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを設定する
   *                 0.0fで停止、1.0fで等倍、2.0fで2倍速、-1.0fで逆等倍再生
   *
   *  @param[in]     step_frame      更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStepFrame(f32 step_frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの更新フレームを取得する
   *
   *  @retval        更新フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStepFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationStartFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを設定する
   *
   *  @param[in]     frame           フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationEndFrame(f32 frame);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの終了フレームを取得する
   *
   *  @retval        フレーム
   */
  //-----------------------------------------------------------------------------
  virtual f32 GetAnimationEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの開始フレームと終了フレームを設定する
   *
   *  @param[in]     start_frame     開始フレーム
   *  @param[in]     end_frame       終了フレーム
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationStartAndEndFrame(f32 start_frame, f32 end_frame);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションの現在のフレームが終了フレームか否か判定する
   *
   *  @retval        bool    現在のフレームが終了フレームの場合tureを返す
   */
  //-----------------------------------------------------------------------------
  bool IsAnimationFrameEndFrame(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションがループ再生か否か取得する
   *
   *  @retval        bool            ループのときtrue、ループでないときfalseを返す
   */
  //-----------------------------------------------------------------------------
  virtual bool IsAnimationLoop(void) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         アニメーションにループ再生を設定する
   *
   *  @param[in]     loop            ループのときtrue、ループでないときfalseを設定する
   */
  //-----------------------------------------------------------------------------
  virtual void SetAnimationLoop(bool loop);



  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレーム操作
   */
  //-----------------------------------------------------------------------------
  void SetUnitFrame(u32 unit_index, f32 frame);
  f32  GetUnitFrame(u32 unit_index) const;
  void SetUnitStepFrame(u32 unit_index, f32 step_frame);
  f32  GetUnitStepFrame(u32 unit_index) const;
  void SetUnitStartFrame(u32 unit_index, f32 frame);
  f32  GetUnitStartFrame(u32 unit_index) const;
  void SetUnitEndFrame(u32 unit_index, f32 frame);
  f32  GetUnitEndFrame(u32 unit_index) const;
  void SetUnitStartAndEndFrame(u32 unit_index, f32 start_frame, f32 end_frame);
  b32  IsUnitFrameEndFrame(u32 unit_index) const;
  b32  IsUnitLoop(u32 unit_index) const;
  void SetUnitLoop(u32 unit_index, b32 loop);


  //-----------------------------------------------------------------------------
  /**
   *  @brief         slot_index内につくったユニットのアニメーションをブレンドする
   *
   *  @param[in]     heap_allocator          ヒープアロケータ
   *  @param[in]     device_allocator        デバイスアロケータ
   *  @param[in]     slot_index              スロットのインデックス
   *  @param[in]     mode                    AnimationSlot::MODE_NONE                slot_indexのアニメーションをなしにする
   *                                         AnimationSlot::MODE_ANIMATION           ブレンドをやめてanimation_unit_indexで指定したアニメーションを割り当てる
   *                                         AnimationSlot::MODE_BLEND_INTERPOLATOR  補間ブレンド
   *                                         AnimationSlot::MODE_BLEND_ADDER         加算ブレンド
   *  @param[in]     weight0                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット0のブレンド重み
   *  @param[in]     weight1                 AnimationSlot::MODE_BLEND_INTERPOLATORかAnimationSlot::MODE_BLEND_ADDERのときのみ使用。
   *                                         ユニット1のブレンド重み
   *  @param[in]     interpolator_ignore_no_anim_member   AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      falseにすると、アニメーションが存在しないメンバはバインド時の値がブレンドされます。
   *                                                      trueにすると、アニメーションが存在しないメンバは重み 0 としてブレンドされます。
   *  @param[in]     interpolator_normalization_enable    AnimationSlot::MODE_BLEND_INTERPOLATORのときのみ使用。
   *                                                      trueにすると、ブレンド重みが合計で 1 になるよう正規化してからブレンドを行います。
   *                                                      falseにすると、設定した重みがそのままブレンドに使用される。
   *  @param[in]     animation_unit_index                 AnimationSlot::MODE_ANIMATIONのときのみ使用。
   *                                                      スロット内のどのユニットにあるアニメーションを割り当てるかを指定する。
   */
  //-----------------------------------------------------------------------------
  void BlendUnitAnimation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );
  void BlendUnit2Animation(
      gfl::heap::NwAllocator*    heap_allocator,
      gfl::heap::NwAllocator*    device_allocator,
      AnimationSlot::Mode        mode,
      f32                        weight0,
      f32                        weight1,
      b32                        interpolator_ignore_no_anim_member = false,
      b32                        interpolator_normalization_enable = true,
      u32                        animation_unit_index = 0
  );


  //-----------------------------------------------------------------------------
  /**
   *  @brief         ブレンド重み
   */
  //-----------------------------------------------------------------------------
  void SetUnitBlendWeight(u32 unit_index, f32 weight);
  f32  GetUnitBlendWeight(u32 unit_index);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ResAnim::Typeのアニメーションで変更したポーズをリセットする
   *
   *  アニメーションが割り当てられているときは、そのアニメーションが動かすメンバはリセットされない。
   *  アニメーションが割り当てられていないときは、全てのメンバがリセットされる。
   */
  //-----------------------------------------------------------------------------
  void ResetAnimationPose(void);




public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         平行移動
   */
  //-----------------------------------------------------------------------------
  void SetTranslate(f32 x, f32 y, f32 z);
  void SetTranslate(const gfl::math::VEC3& t);
  void GetTranslate(gfl::math::VEC3* t) const;
  gfl::math::VEC3 GetTranslate(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         方向
   */
  //-----------------------------------------------------------------------------
  void SetDirection(f32 x, f32 y, f32 z);
  void SetDirection(const gfl::math::VEC3& d);
  void GetDirection(gfl::math::VEC3* d) const;
  gfl::math::VEC3 GetDirection(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを取得する
   *
   *  @param[out]    matrix          取得したマトリクスを格納して返す
   *
   *  @retval        bool            正常に取得できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool GetWorldMatrix(gfl::math::MTX34* matrix) const;
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ワールドマトリクスを設定する
   *                 SceneのCalculateの後でないと反映されません。
   *
   *  @param[in]     matrix          設定するマトリクス
   *
   *  @retval        bool            正常に設定できたらtrue
   */
  //-----------------------------------------------------------------------------
  bool SetWorldMatrix(const gfl::math::MTX34& matrix);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         カラー
   */
  //-----------------------------------------------------------------------------
  void SetAmbient(f32 r, f32 g, f32 b);
  void SetAmbient(f32 r, f32 g, f32 b, f32 a);
  void SetAmbient(gfl::grp::ColorF32& c);
  void SetAmbient(u8 r, u8 g, u8 b);
  void SetAmbient(u8 r, u8 g, u8 b, u8 a);
  void SetAmbient(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetAmbient(void) const;
  void GetAmbient(gfl::grp::ColorF32* c) const;
  void GetAmbient(gfl::grp::ColorU8*  c) const;

  void SetDiffuse(f32 r, f32 g, f32 b);
  void SetDiffuse(f32 r, f32 g, f32 b, f32 a);
  void SetDiffuse(gfl::grp::ColorF32& c);
  void SetDiffuse(u8 r, u8 g, u8 b);
  void SetDiffuse(u8 r, u8 g, u8 b, u8 a);
  void SetDiffuse(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetDiffuse(void) const;
  void GetDiffuse(gfl::grp::ColorF32* c) const;
  void GetDiffuse(gfl::grp::ColorU8*  c) const;

  void SetSpecular0(f32 r, f32 g, f32 b);
  void SetSpecular0(f32 r, f32 g, f32 b, f32 a);
  void SetSpecular0(gfl::grp::ColorF32& c);
  void SetSpecular0(u8 r, u8 g, u8 b);
  void SetSpecular0(u8 r, u8 g, u8 b, u8 a);
  void SetSpecular0(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSpecular0(void) const;
  void GetSpecular0(gfl::grp::ColorF32* c) const;
  void GetSpecular0(gfl::grp::ColorU8*  c) const;

  void SetSpecular1(f32 r, f32 g, f32 b);
  void SetSpecular1(f32 r, f32 g, f32 b, f32 a);
  void SetSpecular1(gfl::grp::ColorF32& c);
  void SetSpecular1(u8 r, u8 g, u8 b);
  void SetSpecular1(u8 r, u8 g, u8 b, u8 a);
  void SetSpecular1(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSpecular1(void) const;
  void GetSpecular1(gfl::grp::ColorF32* c) const;
  void GetSpecular1(gfl::grp::ColorU8*  c) const;

  void SetGroundColor(f32 r, f32 g, f32 b);
  void SetGroundColor(f32 r, f32 g, f32 b, f32 a);
  void SetGroundColor(gfl::grp::ColorF32& c);
  void SetGroundColor(u8 r, u8 g, u8 b);
  void SetGroundColor(u8 r, u8 g, u8 b, u8 a);
  void SetGroundColor(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetGroundColor(void) const;
  void GetGroundColor(gfl::grp::ColorF32* c) const;
  void GetGroundColor(gfl::grp::ColorU8*  c) const;

  void SetSkyColor(f32 r, f32 g, f32 b);
  void SetSkyColor(f32 r, f32 g, f32 b, f32 a);
  void SetSkyColor(gfl::grp::ColorF32& c);
  void SetSkyColor(u8 r, u8 g, u8 b);
  void SetSkyColor(u8 r, u8 g, u8 b, u8 a);
  void SetSkyColor(const gfl::grp::ColorU8& c);
  gfl::grp::ColorF32 GetSkyColor(void) const;
  void GetSkyColor(gfl::grp::ColorF32* c) const;
  void GetSkyColor(gfl::grp::ColorU8*  c) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         距離減衰
   */
  //-----------------------------------------------------------------------------
  void SetDistanceLookUpTable(
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         角度減衰
   */
  //-----------------------------------------------------------------------------
#if 0
  //角度減衰用の参照テーブルはGetはできてもSetはできない。
  void SetAngleLookUpTable(
      Resource*    lookup_table_resource,
      s32          lookup_table_set_index_in_resource,
      s32          lookup_table_index_in_lookup_table_set
  );
#endif


  //-----------------------------------------------------------------------------
  /**
   *  @brief         有効フラグ
   */
  //-----------------------------------------------------------------------------
  void SetEnable(bool enable);
  bool IsEnable(void) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         説明
   */
  //-----------------------------------------------------------------------------
  virtual Type GetType(void) const  {  return m_type;  }
  virtual Kind GetKind(void) const  {  return m_kind;  }

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         説明
   */
  //-----------------------------------------------------------------------------
  virtual nw::gfx::Light*            GetNwLight(void) const            {  return nw::ut::DynamicCast<nw::gfx::Light*>( GetNwSceneNode() );  }
  virtual nw::gfx::AmbientLight*     GetNwAmbientLight(void) const     {  return nw::ut::DynamicCast<nw::gfx::AmbientLight*>( GetNwSceneNode() );  }
  virtual nw::gfx::FragmentLight*    GetNwFragmentLight(void) const    {  return nw::ut::DynamicCast<nw::gfx::FragmentLight*>( GetNwSceneNode() );  }
  virtual nw::gfx::HemiSphereLight*  GetNwHemiSphereLight(void) const  {  return nw::ut::DynamicCast<nw::gfx::HemiSphereLight*>( GetNwSceneNode() );  }
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
  virtual nw::gfx::VertexLight*      GetNwVertexLight(void) const      {  return nw::ut::DynamicCast<nw::gfx::VertexLight*>( GetNwSceneNode() );  }
#endif 

  virtual nw::gfx::res::ResAmbientLight     GetNwResAmbientLight(void) const;
  virtual nw::gfx::res::ResFragmentLight    GetNwResFragmentLight(void) const;
  virtual nw::gfx::res::ResHemiSphereLight  GetNwResHemiSphereLight(void) const;
#if GFL_GRP_G3D_VERTEX_LIGHT_ENABLE
  virtual nw::gfx::res::ResVertexLight      GetNwResVertexLight(void) const;
#endif 

  virtual void CheckTypeAndKind(void);


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     description        設定内容
   */
  //-----------------------------------------------------------------------------
  void CreateAmbientLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateFragmentLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateHemiSphereLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );
  void CreateVertexLight(
      gfl::heap::NwAllocator*           device_allocator,
      const Description&                description 
  );


private:
  Type   m_type;
  Kind   m_kind;
  //Animation*  m_animation;
  u32                m_animation_slot_num;  // 0か1  // 0のときアニメーションを付けられない、1のときアニメーションを付けられる(実際に付けているかどうかはm_animation_managerで確認して下さい)。
  u32                m_animation_blend_unit_num;
  u32                m_animation_num_in_unit;
  AnimationManager*  m_animation_manager;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DLIGHT_H__
