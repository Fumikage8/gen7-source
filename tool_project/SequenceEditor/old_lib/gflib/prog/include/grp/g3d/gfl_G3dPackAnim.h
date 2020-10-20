#ifndef __GFL_G3DPACKANIM_H__
#define __GFL_G3DPACKANIM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dPackAnim.h
 *	@brief  3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱う
 *	@author	Koji Kawada
 *	@date		2012.08.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if 0
/*[PackAnimのルール]
アニメーションファイルを1つにまとめたPackAnimファイルは次のルールを満たさなければならない。
・g3d_omake_toolのG3DTOOLANIM_MakePackAnimFileで1つにまとめたファイルをつくること。
・同じAnimationTypeのファイルだけでまとめること。
・個々のファイルの中身は、1個のアニメーションデータだけから成ること。
  言い換えると、ResourceからAnimationTypeを指定して0番で取得できなければならない。*/
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
///	3Dアニメーションファイルを1つにまとめたPackAnimファイルを扱うクラス
//=====================================
class PackAnim
{
  GFL_FORBID_COPY_AND_ASSIGN(PackAnim);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  PackAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief  デストラクタ
   *
   *          Destoryを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~PackAnim();

  //-----------------------------------------------------------------------------
  /**
   *  @brief  生成
   *
   *  @param[in] heap_allocator
   *  @param[in] device_allocator
   *  @param[in] type
   *  @param[in] buffer  アニメーションファイルを1つにまとめたPackAnimファイルを読み込んだバッファ。
   *                     VRAMかデバイスメモリに確保しておいて下さい。
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
    gfl::heap::NwAllocator* heap_allocator,
    gfl::heap::NwAllocator* device_allocator,
    AnimationType           type,
    void*                   buffer
  );
  //-----------------------------------------------------------------------------
  /**
   *  @brief  破棄
   *
   *          何度呼んでも大丈夫です。
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief  まとめられているアニメーションファイルのメンバ数の最大値を返す
   *
   *          これとモデルのメンバ数を比較して大きいほうを採用するとよい。
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetMaxAnimMembers(void) const;
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief  まとめられているアニメーションファイル数を返す
   *
   *          GetResAnimがNULLを返すときのものも1個としてカウントしている。
   */
  //-----------------------------------------------------------------------------
  virtual s32 GetAnimNum(void) const;
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief  まとめられているアニメーションファイルのanim_no番目からつくったResAnimを得る
   *
   *  @param[in]  anim_no  何番目のアニメーションファイルか
   *
   *  @retval  ResAnim
   *           anim_no番目のアニメーションファイルがないときはNULLを返す
   */
  //-----------------------------------------------------------------------------
  virtual ResAnim* GetResAnim(s32 anim_no) const;

private:
  void*         m_buffer;
  Resource**    m_resource_array;
  ResAnim**     m_res_anim_array;
  AnimationType m_type;
  s32           m_anim_num;
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DPACKANIM_H__

