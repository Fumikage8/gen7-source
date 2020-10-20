#ifndef __GFL_G3DUNITANIMATION_H__
#define __GFL_G3DUNITANIMATION_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dUnitAnimation.h
 *	@brief  3Dアニメーションの単位となるもの
 *	@author	Koji Kawada
 *	@date		2011.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if 0
/*

用語解説


注意
  「Animation」となっているときは「Animationクラス」のことを指し、
  「アニメーション」となっているときは「一般的な動作やモーション」のことを指している。
  混同を避けるために「Animation」は「Animationクラス」と明記することにするが、
  「Animationクラス」と書いて「Animationクラスのインスタンス」のことを表していることもある。


IAnimationクラス
  AnimatonクラスとUnitAnimationクラスの基底クラス
  抽象クラス、インスタンスクラス


Animationクラス
  以下のどれか
  ・リソースそのままのアニメーション
  ・複数のAnimationクラスをブレンドしたアニメーション
  ・複数のAnimationクラスをブレンドしたアニメーションをブレンドしたアニメーション


UnitAnimationクラス
  Animationクラスを1個だけ持つ
  持っているAnimationクラスを変更するときは、何フレームかかけて前のAnimationクラスと重み合計1となる補間ブレンドを行いながら変更する
  (ブレンドを行うので、前のAnimationクラス、前の前のAnimationクラス、前の前の前のAnimationクラスというふうに
   実質複数のAnimationクラスを持っているのだが、外から見たら1個のAnimationクラスにしか見えないので単位(Unit)と呼んでいる)
  補間ブレンドでの変更が終わっているならAnimationクラスそのものになっている
  補間ブレンドで変更しなくてもいい場合は常にAnimationクラスそのものになる


AnimationSlotクラス
  複数のUnitAnimationクラスを任意の方式でブレンドしたアニメーションを1個だけ持つ
  ブレンドしていないならUnitAnimationクラスそのものになっている

  Animationクラスとほとんど同じなので、
  「AnimationクラスをIAnimationクラスでブレンドできるように即ちUnitAnimationクラスのブレンドができるように拡張」して、
  その「拡張したAnimationクラス」をAnimationSlotクラスの中身として持つようにしたらAnimationクラスと似通った記述がなくなるのだが、
  そうしなかった。なぜならそうしてしまうと、
  「UnitAnimationクラスが「拡張したAnimationクラス」を複数持つことができる即ちUnitAnimationクラスの部品にUnitAnimationクラスが来ることができてしまう」
  ため、考えが複雑になってしまうから。どう複雑になるか例を挙げると、
  ＜『「直立からパンチに変更中のUnitAnimationクラス」を「直立からキックに変更中のUnitAnimationクラス」に変更中のUnitAnimationクラス』を
  『直立しているAnimationクラス』に変更するUnitAnimationクラス＞ということが実現できてしまう。
  これくらい複雑でもいいかもという気になったら、
  「AnimationクラスのAnimationPackのメンバをAnimationクラスからIAnimationクラスに変更し、AnimationクラスにCalculateを持たせ、
  呼び出し元は毎フレームGetNwAnimObjectの結果でポインタを変更する」ということをしなければならない。

  AnimationSlotクラスは、nw::gfx::Modelのメンバ関数である
  SetSkeletalAnimObject, SetMaterialAnimObject, SetVisibilityAnimObject
  に渡すobjectIndexを、固定値で1つ割り当てられている。
  objectIndexは全AnimationSlotでの通し番号にしてある。
  同じアニメーショングループ(例：SkeletalとSkeletal)で部分アニメーション(PartialAnimation)を行う際には
  objectIndexを別の値にする必要があるが、
  異なるアニメーショングループ(例：SkeletalとMaterial)の場合は同じobjectIndexを使っても問題ない。
  しかし、Skeletalの何番目、Materialの何番目とやるのが嫌だったので、
  SkeletalもMaterialもVisibilityも含めて通し番号にすることにした。


AnimationManagerクラス
  AnimationSlotを複数持つ
  AnimationSlotクラスはお互いに干渉しないので、部分アニメーション(PartialAnimation)ができる
  AnimationSlotクラスの独立性をきっちり保つためにも、同じ部分を操作するようなアニメーションを複数のAnimationSlotクラスに割り当てないように
  ユーザは注意して下さい




設定解説

  nw::gfx::AnimInterpolator::Builder()
  nw::gfx::TransformAnimInterpolator::Builder()
      .IgnoreNoAnimMember(false)  // falseで
      IgnoreNoAnimMemberをtrueにすると、ノードAの動きのあるアニメbとノードAの動きのないアニメcをブレンドすると
      ノードAにはアニメbの動きのみが付く。即ち、動きのないアニメcとはブレンドされない。
      IgnoreNoAnimMemberをfalseにすると、ノードAの動きのあるアニメbとノードAの動きのないアニメcをブレンドすると
      ノードAにはアニメbの動きとOriginalValue（モデルのロード時の状態）でブレンドした動きが付く。

  nw::gfx::AnimAdder
  nw::gfx::TransformAnimAdder
      IgnoreNoAnimMemberの設定はないが、trueだと思ってもらばよい。

  nw::gfx::AnimInterpolator::
  nw::gfx::TransformAnimInterpolator::
      SetNormalizationEnabled(true);  // trueで
      ブレンド重みの合計が必ず1になるように重みの値をユーザが渡してくれるなら  
      falseでいいのだが、そうとは限らないだろうからtrueにしておく。
      ブレンド重みの合計を1以上や1未満にしたブレンドを意図的に行いたい、という要望があるかもしれないが、
      結果を理解しにくい(IgnoreNoAnimMemberと組み合わせると特に理解しにくい)だけなのでやめておく。
      Builder::IsOldMethod で true を指定していると、この設定は無視されます。
      デフォルト値は true です。
*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dIAnimation.h>
#include <grp/g3d/gfl_G3dAnimation.h>
#include <grp/g3d/gfl_G3dResAnim.h>
#include <grp/g3d/gfl_G3dResource.h>


namespace gfl {
namespace grp {
namespace g3d {


//-------------------------------------
///	アニメーションの単位となるもの
//=====================================
class UnitAnimation : public IAnimation
{
  GFL_FORBID_COPY_AND_ASSIGN(UnitAnimation);


  friend class AnimationSlot;
  friend class AnimationManager;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friendクラスからのみアクセスしてもいいメンバ関数
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // コンストラクタ
  UnitAnimation(void);
  // デストラクタ
  virtual ~UnitAnimation();

  // 生成
  virtual void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    s32                     num_max,
    Animation*              animation,
    u32                     change_frame
  );
  void ChangeAnimation(
    gfl::heap::NwAllocator* device_allocator,
    Animation*              animation,
    u32                     change_frame
  );

  // 破棄
  virtual void Destroy(void);

  // 計算処理
  // 必ず毎フレーム呼んで下さい。
  virtual void Calculate(void);

  // 現在のアニメーションを得る
  // 必ず毎フレームCalculateの後で呼び、呼び出し元で使用しているアニメーションのポインタを変更して下さい。
  // Calculateでそのアニメーションのポインタを手放してしまうかもしれないので、Calculateの前には呼ばないこと。
  virtual nw::gfx::AnimObject* GetNwAnimObject(void) const;

  b32 BindBlend(nw::gfx::AnimGroup* nw_anim_group);

  // アニメーションを変更する
  void ChangeAnimation(
      Animation*  animation,
      u32         change_frame
  );

  // アニメーションのタイプを得る
  virtual ResAnim::Type GetType(void) const;

  // 現在くっついているアニメーションの数(重み0でもくっついていれば数に入れる)
  virtual s32 GetAnimationNum(void) const;

  // フレーム制御
  virtual void SetFrame(const f32 frame);
  virtual f32  GetFrame(void) const;
  
  virtual void SetStepFrame(const f32 step_frame);
  virtual f32  GetStepFrame(void) const;
  
  virtual void SetStartFrame(const f32 frame);
  virtual f32  GetStartFrame(void) const;
  virtual void SetEndFrame(const f32 frame);
  virtual f32  GetEndFrame(void) const;
  virtual void SetStartAndEndFrame(const f32 start_frame, const f32 end_frame);
  virtual b32  IsFrameStartFrame(void) const;
  virtual b32  IsFrameEndFrame(void) const;
  virtual b32  IsFrameStepTerminalFrame(void) const;

  virtual void SetLoop(b32 loop);
  virtual b32  IsLoop(void) const;


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friendクラスからもアクセスしてはならない定数、型
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // m_type, m_num_max, m_blendを設定、生成する
  b32 CreateBlend( 
    gfl::heap::NwAllocator* device_allocator,
    ResAnim::Type           type,
    s32                     num_max
  );

  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friendクラスからもアクセスしてはならない定数、型
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  // 状態
  enum State
  {
    STATE_NONE,           // アニメーションがない
    STATE_ONE,            // m_pack_array[0]のアニメーションになっている
    STATE_OTHERS_BLEND,   // nw::gfx::AnimInterpolatorになっている
    STATE_SKELETAL_BLEND  // nw::gfx::TransformAnimInterpolatorになっている
  };

  // アニメーションの情報をまとめた構造体
  struct AnimationPack
  {
    u32               frame_count;  // ブレンドフレームのカウント
    u32               frame_max;    // 何フレームブレンドするか
          // (frame_count>=frame_max)が真になったらこのアニメーションになる
          // frame_max=0のときは、変更したフレームでこのアニメーションになる(0フレーム後に完全にこれになる)
          // frame_max=1のときは、変更したフレームでは何も変わらず、次のフレームでこのアニメーションになる(1フレーム後に完全にこれになる)
          // frame_max=2のときは、変更したフレームでは何も変わらず、次のフレームで中間のアニメーションになり、さらに次のフレームこのアニメーションになる(2フレーム後に完全にこれになる)
    Animation*        animation;    // 他のところで生成したアニメーションを覚えているだけのポインタ
    AnimationPack(void)
      : frame_count(0),
        frame_max(0),
        animation(NULL)
    {}
  };


  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  //
  // friendクラスからもアクセスしてはならないメンバ変数
  //
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
private:
  State          m_state;
  ResAnim::Type  m_type;

  s32             m_num_max;  // 最大数
  s32             m_num;      // 現在の個数
  AnimationPack*  m_pack_array;  // 最新[0][1]...[m_num_max-2][m_num_max-1]最古

  // ブレンド率はm_numをnとすると
  // 
  // n=2のとき
  // [0].weight=([0].frame_count)/([0].frame_max);
  // [1].weight=1-[0].weight;
  //
  // n=3のとき
  // [0].weight=([0].frame_count)/([0].frame_max);
  // [1].weight=(1-[0].weight)*(([1].frame_count)/([1].frame_max));
  // [2].weight=(1-[0].weight)*(1-([1].frame_count)/([1].frame_max))=1-[0].weight-[1].weight;
  //
  // [n].weight=wn, ([n].frame_count)/([n].frame_max)=fn とすると
  // 
  // n=2のとき
  // w0=f0
  // w1=1-f0=1-w0
  //
  // n=3のとき
  // w0=f0
  // w1=(1-f0)*f1=(1-w0)*f1
  // w2=(1-f0)*(1-f1)=(1-w0)*(1-f1)=1-w0-w1
  //
  // n=4のとき
  // w0=f0
  // w1=(1-f0)*f1
  // w2=(1-f0)*(1-f1)*f2
  // w3=(1-f0)*(1-f1)*(1-f2)=1-w0-w1-w2
  //
  // n=mのとき
  // w0=f0
  // 1<=y<mとすると
  // wy=(1-f0)*(1-f1)*(1-f2)*...*(1-f[y-1])*fy
  // fyはn=m-1のとき必ず1とする
  // 
  // となる
 
  nw::gfx::AnimInterpolator*           m_blend;                  // 最大m_num_max個のアニメーションをブレンドしたアニメーション
  nw::gfx::AnimGroup*                  m_blend_bind_anim_group;  // バインドしているアニメーショングループを覚えておく。NULLのときバインドしていない。
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DUNITANIMATION_H__
