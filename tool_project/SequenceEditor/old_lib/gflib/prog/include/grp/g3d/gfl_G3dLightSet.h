#ifndef __GFL_G3DLIGHTSET_H__
#define __GFL_G3DLIGHTSET_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dLightSet.h
 *	@brief  3Dライトセット
 *	@author	Koji Kawada
 *	@date		2012.05.16
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
#include <grp/g3d/gfl_G3dLight.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dライトセットクラス
//=====================================
class LightSet
{
  GFL_FORBID_COPY_AND_ASSIGN(LightSet);

  friend class Scene;


public:
  // 設定内容
  struct Description
  {
    s32  max_vertex_lights;  // 頂点ライトの最大数

    Description(void)
      : max_vertex_lights(0)
    {}
  };


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  LightSet(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~LightSet();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     device_allocator   デバイスアロケータ
   *  @param[in]     description        設定内容
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           device_allocator,
      Description*                      description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);


public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         ライトを追加する
   *
   *  @param[in]     light  追加するライト
   *                        このライトのポインタを覚えておくだけです。
   *
   *  @retval  追加に成功したらtrueを返す。
   */
  //-----------------------------------------------------------------------------
  b32 AddLight(Light* light);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ライトを削除する
   *
   *  @param[in]     light  削除するライト
   *                        このライトのポインタを忘れるだけです。
   */
  //-----------------------------------------------------------------------------
  void RemoveLight(Light* light);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         ライトを全て削除する
   *                 覚えているライトのポインタを全て忘れるだけです。
   */
  //-----------------------------------------------------------------------------
  void RemoveAllLight(void);


private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         NW4Cのライトセットを得る
   *                 friendクラスからだけアクセス可能。
   *
   *  @retval  NW4Cのライトセット
   */
  //-----------------------------------------------------------------------------
  nw::gfx::LightSet* GetNwLightSet(void) const { return m_light_set; }


private:
  nw::gfx::LightSet* m_light_set;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DLIGHTSET_H__
