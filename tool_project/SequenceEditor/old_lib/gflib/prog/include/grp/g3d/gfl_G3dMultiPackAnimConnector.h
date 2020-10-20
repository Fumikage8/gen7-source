#ifndef __GFL_G3DMULTIPACKANIMCONNECTOR_H__
#define __GFL_G3DMULTIPACKANIMCONNECTOR_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dMultiPackAnimConnector.h
 *	@brief  3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱う
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*[MultiPackAnimConnectorのルール]
アニメーションファイルを1つにまとめたPackAnimファイルは次のルールを満たさなければならない。
・[PackAnimConnectorのルール]を満たす。
・同じ番号のアニメーションデータは同じ設定でなければならない。
  例：PackAnimがaaaa, bbbb, ccccの3つのとき
        aaaa[0] 15フレーム ワンタイム  aaaa[1] 30フレーム ループ  aaaa[2] 45フレーム ワンタイム 
        bbbb[0] 15フレーム ワンタイム  bbbb[1] 30フレーム ループ  bbbb[2] 45フレーム ワンタイム
        cccc[0] 15フレーム ワンタイム  cccc[1] 30フレーム ループ  cccc[2] 45フレーム ワンタイム
      というふうに同じ設定でなければならない。
  これは、常に同じ番号、同じフレームが再生されることに由来する。つまり、
        aaaaの1番の8フレーム目が再生されているときは、bbbbも1番の8フレーム目、ccccも1番の8フレーム目が再生されている
  ということに由来する。
・【将来的な構想】アニメーションを切り替えるときのモーションブレンドの仕方は、
  それぞれのスロットで設定できるようにするかもしれない。
  テクスチャUVアニメーションの表情アニメと、スケルタルアニメーションの全身アニメで、
  同じ補間の仕方をするわけにはいかないので。*/
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
///	1つのModelと複数のPackAnimを結び付けるクラス
//=====================================
class ModelMultiPackAnimConnector
{
  GFL_FORBID_COPY_AND_ASSIGN(ModelMultiPackAnimConnector);

public:
  ModelMultiPackAnimConnector(void);
  virtual ~ModelMultiPackAnimConnector();

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
   *          slot_indexが既にPackAnimを設定済みのスロットの場合、PackAnimを設定し直すと、それまで設定していたPackAnimの設定は破棄します。
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
   *  @brief  slot_indexが既にPackAnimを設定済みのスロットの場合、設定していたPackAnimの設定を破棄する
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetPackAnim(s32 slot_index);
  //-----------------------------------------------------------------------------
  /**
   *  @brief  PackAnimを設定済みの全スロットに対して、設定していたPackAnimの設定を破棄する
   */
  //-----------------------------------------------------------------------------
  virtual void UnsetAllPackAnim(void);

  virtual void ChangeAnimNo(s32 anim_no);
  
  virtual void SetStepFrame(f32 step_frame);
  virtual void SetLoop(b32 loop);

private:
  struct PackAnimElem
  {
    ModelPackAnimConnector*  pack_anim_connector;
    PackAnimElem*            prev;  // 先頭のときNULL
    PackAnimElem*            next;  // 末尾のときNULL
  };

private:
  PackAnimElem* getPackAnimElem(s32 slot_index) const;

private:
  ////////////////////////////////////////////
  // slot_indexが一致するとき、UnsetPackAnimで設定が破棄されるもの
  ////////////////////////////////////////////
  PackAnimElem*  m_pack_anim_list_head;  // 先頭のポインタ

  ////////////////////////////////////////////
  // UnsetPackAnimで設定が破棄された結果、m_pack_anim_list_headがNULLになったら設定が破棄されるもの
  ////////////////////////////////////////////
  s32            m_anim_no;
 
  ////////////////////////////////////////////
  // UnsetPackAnimで設定が破棄されないもの
  ////////////////////////////////////////////
  Model*         m_model;
  f32            m_step_frame;
  b32            m_loop;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DMULTIPACKANIMCONNECTOR_H__

