//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dSceneEnvironmentSetting.cpp
 *	@brief  3Dシーン環境設定
 *	@author	Koji Kawada
 *	@date		2010.10.12
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Heap.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dSystem.h>
#include <grp/g3d/gfl_G3dResource.h>

#include <grp/g3d/gfl_G3dSceneEnvironmentSetting.h>

namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス定義
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3Dシーン環境設定クラス
//=====================================

//-----------------------------------------------------------------------------
/*
 *  @brief         生成
 *
 *  @param[in]     allocator          アロケータ
 *  @param[in]     allocator          デバイスメモリのアロケータ
 *  @param[in]     resource           リソース
 *  @param[in]     index_in_resource  リソース内におけるインデックス
 */
//-----------------------------------------------------------------------------
void SceneEnvironmentSetting::Create(
    gfl::heap::NwAllocator*           allocator, 
    gfl::heap::NwAllocator*           device_allocator, 
    Resource*                         resource,
    s32                               index_in_resource
)
{
  if( resource == NULL )
  {
    GFL_ASSERT_MSG( 0, "リソースが不正のためシーンオブジェクトが生成できませんでした。\n" );
    return;
  }

  nw::gfx::ResGraphicsFile* nw_res_graphics_file = resource->GetNwResGraphicsFile();
  nw::gfx::ResSceneEnvironmentSetting res = nw_res_graphics_file->GetSceneEnvironmentSettings(index_in_resource);
  nw::gfx::SceneObject* scene_object = gfl::grp::g3d::Resource::CreateNwSceneObject(
      allocator,
      device_allocator,
      res,
      false,  // アニメーションなし
      nw::gfx::Model::FLAG_BUFFER_NOT_USE,
      0
  );
  GFL_ASSERT_MSG( scene_object, "index_in_resource=%dのシーンオブジェクトが生成できませんでした。\n", index_in_resource );

  nw::gfx::SceneEnvironmentSetting* scene_environment_setting =
      nw::ut::DynamicCast<nw::gfx::SceneEnvironmentSetting*>(scene_object);
  SetNwSceneEnvironmentSetting(scene_environment_setting);
  SetResourceIdAndIndexInResource(resource->GetId(), index_in_resource);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         破棄
 *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
 */
//-----------------------------------------------------------------------------
void SceneEnvironmentSetting::Destroy(void)
{
  nw::gfx::SafeDestroy(m_nw_scene_environment_setting);
}

//-----------------------------------------------------------------------------
/*
 *  @brief         コンストラクタ
 *                 Createは呼ばれません。
 */
//-----------------------------------------------------------------------------
SceneEnvironmentSetting::SceneEnvironmentSetting(void)
  : m_nw_scene_environment_setting(NULL)
{
  GFL_SINGLETON_INSTANCE(System)->GetSceneEnvironmentSettingManager()->AddObject(this);
}
//-----------------------------------------------------------------------------
/*
 *  @brief         デストラクタ
 *                 Destroyを呼びます。
 */
//-----------------------------------------------------------------------------
SceneEnvironmentSetting::~SceneEnvironmentSetting()
{
  Destroy();
  GFL_SINGLETON_INSTANCE(System)->GetSceneEnvironmentSettingManager()->RemoveObject(this);
}


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

