#ifndef __GFL_G3DPACKANIMCONNECTOR_H__
#define __GFL_G3DPACKANIMCONNECTOR_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnimConnector.h
 *	@brief  3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱う
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[PackAnimConnectorのルール]
アニメーションファイルを1つにまとめたPackAnimファイルは次のルールを満たさなければならない。
・[PackAnimのルール]を満たすこと。
・StartFrame, EndFrameは使えないので、それらを使うこと前提のアニメーションデータは含まないこと。
・Loopは全部に対して同じ設定をするので、個々のファイルに設定しておいても使われない。
  モーションブレンド中でなく1個のアニメーションデータ再生中のときは、個々のファイルの設定を使わないことが確定している。
  【将来的な構想】しかし、アニメーションを切り替えるときのモーションブレンド中は、
  個々のファイルの設定を使うかもしれないので、個々のファイルにきちんと設定しておくことを勧める。
・StepFrameは全部に対して同じ設定をする。
・Frameは今再生中のアニメーションデータのフレーム番号になる。*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp g3d
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
///	1つのModelと1つのPackAnimを結び付けるクラス
//=====================================
class ModelPackAnimConnector
{
public:
  ModelPackAnimConnector(void);
  virtual ~ModelPackAnimConnector();

  //-----------------------------------------------------------------------------
  /**
   *  @brief  モデルを設定する
   *
   *          モデルを設定し直すと、PackAnimの設定もなくなります。
   */
  //-----------------------------------------------------------------------------
  virtual void SetModel(Model* model);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  PackAnimを設定する
   *
   *          SetModelをしてからでないと、PackAnimを設定できません。
   *          PackAnimを設定し直すと、それまで設定していたPackAnimの設定は破棄します。
   */
  //-----------------------------------------------------------------------------
  virtual void SetPackAnim(
      gfl::heap::NwAllocator* heap_allocator,
      gfl::heap::NwAllocator* device_allocator,
      s32 slot_index,
      PackAnim* pack_anim
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  設定していたPackAnimの設定を破棄する
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetPackAnim(void);


  virtual void ChangeAnimNo(s32 anim_no);

  virtual void SetFrame(f32 frame);
  virtual f32 GetFrame(void) const;
  virtual void SetStepFrame(f32 step_frame);
  virtual f32 GetStepFrame(void) const;
  virtual void SetLoop(b32 loop);
  virtual b32 IsLoop(void) const;

  virtual s32 GetSlotIndex(void) const  { return m_slot_index; }

private:
  ////////////////////////////////////////////
  // UnsetPackAnimで設定が破棄されるもの
  ////////////////////////////////////////////
  PackAnim*  m_pack_anim;
  s32        m_slot_index;
  s32        m_anim_no;
  b32        m_is_set;  // m_model!=NULLかつm_pack_anim!=NULLかつm_pack_anim->GetMaxAnimMembers()>0かつ
                        // m_model->ChangeAnimationでm_slot_indexに領域を確保済みのときtrue。それ以外のときfalse。
  // @note ChangeAnimationメモリ管理不備対応  // ChangeAnimationでNULLを渡す代わりに下記アニメの0フレームで止めておくことにする。
  ResAnim*   m_res_anim_change_animation_temp;

  ////////////////////////////////////////////
  // UnsetPackAnimで設定が破棄されないもの
  ////////////////////////////////////////////
  Model*     m_model;
  f32        m_step_frame;
  b32        m_loop;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DPACKANIMCONNECTOR_H__

