#ifndef __GFL_G3DSCENEENVIRONMENTSETTING_H__
#define __GFL_G3DSCENEENVIRONMENTSETTING_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneEnvironmentSetting.h
 *	@brief  3Dシーン環境設定
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
#include <gfl_Heap.h>
#include <gfl_Macros.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dObject.h>
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
///	3Dシーン環境設定クラス
//=====================================
class SceneEnvironmentSetting : public Object
{
  GFL_FORBID_COPY_AND_ASSIGN(SceneEnvironmentSetting);

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]     allocator          アロケータ
   *  @param[in]     device_allocator   デバイスメモリのアロケータ
   *  @param[in]     resource           リソース
   *  @param[in]     index_in_resource  リソース内におけるインデックス
   */
  //-----------------------------------------------------------------------------
  virtual void Create(
      gfl::heap::NwAllocator*           allocator, 
      gfl::heap::NwAllocator*           device_allocator, 
      Resource*                         resource,
      s32                               index_in_resource
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
   *  @brief         このクラスが保持しているnw::gfx::SceneEnvironmentSetting*を取得
   *
   *  @retval        nw::gfx::SceneEnvironmentSetting*    このクラスが保持しているnw::gfx::SceneEnvironmentSetting
   */
  //-----------------------------------------------------------------------------
  nw::gfx::SceneEnvironmentSetting* GetNwSceneEnvironmentSetting(void)  {  return m_nw_scene_environment_setting;  }

public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  SceneEnvironmentSetting(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~SceneEnvironmentSetting();

protected:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         このクラスで保持するようnw::gfx::SceneEnvironmentSetting*を設定
   *
   *  @param[in]     nw_scene_environment_setting    シーン環境設定
   */
  //-----------------------------------------------------------------------------
  void SetNwSceneEnvironmentSetting(nw::gfx::SceneEnvironmentSetting* nw_scene_environment_setting)
  {
    m_nw_scene_environment_setting = nw_scene_environment_setting;
  }

private:
  nw::gfx::SceneEnvironmentSetting* m_nw_scene_environment_setting;
};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSCENEENVIRONMENTSETTING_H__
