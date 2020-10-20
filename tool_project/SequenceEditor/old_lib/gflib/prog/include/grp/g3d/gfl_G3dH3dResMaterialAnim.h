#ifndef __GFL_G3DH3DRESMATERIALANIM_H__
#define __GFL_G3DH3DRESMATERIALANIM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dResMaterialAnim.h
 *	@brief  H3Dリソースアニメーション
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#if 0
/*説明
このH3dResMaterialAnimでは
nw::h3d::res::MaterialAnimContent
を扱います。*/
#endif


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>
#include <gfl_Macros.h>
#include <gfl_fs.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dDefine.h>



#if GFL_GRP_G3D_H3D_USE

// NW4C
#include <nw/h3d.h>
// gflib grp g3dh3d
#include <grp/g3d/gfl_G3dH3dResource.h>
#include <grp/g3d/gfl_G3dH3dContent.h>
#include <grp/g3d/gfl_G3dH3dResAnim.h>



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3Dマテリアルアニメーションコンテントクラス
//=====================================
class H3dResMaterialAnim : public H3dResAnim
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dResMaterialAnim);

  friend class H3dAnimationSlot;

public:
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // コンストラクタ / デストラクタ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  H3dResMaterialAnim(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dResMaterialAnim();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  // 基底クラスのまま



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // テクスチャパターンアニメーション
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         テクスチャパターンアニメーションで使用するテクスチャをセットする。
   *                 trueを返したら全てのテクスチャがセット済みということ。
   *                 何回呼んでもいいがtrueを返すようになってからは何もしない。
   */
  //-----------------------------------------------------------------------------
  b32 SetupTexturePatternAnimTexture(H3dResource* other_resource);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendには公開している非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  // テクスチャパターンアニメーション
  // @brief  テクスチャパターンアニメーションの、テクスチャバインドステートを得る
  //         テクスチャパターンアニメーションがない場合はNULLを返す
  const nw::h3d::TexturePatternAnimBindState* GetNwTexturePatternAnimBindState(void) const
  {
    if( m_texpat_buff ) return &m_texpat_bind_state;
    else                return NULL;
  }



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // friendにも公開していない非公開メンバ
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
private:
  // @brief  生成内部用
  virtual b32 createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, const char* name);
  virtual b32 createInternal(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const H3dResource* resource, s32 index_in_resource);
  // @brief  破棄内部用
  virtual void destroyInternal(void);

private:
  // テクスチャパターンアニメーション
  // @brief  テクスチャパターンアニメーションで使用するテクスチャの数を得る
  //         0のときはテクスチャパターンアニメーションなし
  u32 getTexturePatternAnimTextureNum(void) const;

private:
  // テクスチャパターンアニメーション
  void* m_texpat_buff;  // NULLのときはテクスチャパターンアニメーションなし
  nw::h3d::TexturePatternAnimBindState m_texpat_bind_state;  // m_texpat_buffがNULLでないときだけ有効な値が入っている
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DRESMATERIALANIM_H__

