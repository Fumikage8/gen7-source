#ifndef __GFL_G3DSHADOWSYSTEM_H__
#define __GFL_G3DSHADOWSYSTEM_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dShadowSystem.h
 *	@brief  3D影システム
 *	@author	Koji Kawada
 *	@date		2011.03.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

//  gflib grp
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dCamera.h>
#include <grp/g3d/gfl_G3dShadowGroup.h>


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	3D影システムクラス
//=====================================
class ShadowSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(ShadowSystem);

  
  // 下記クラスからはprivate関数にもアクセスしてよい
  friend class Scene;


public:
  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
  enum ReceiverType
  {
    RECEIVER_TYPE_UNKNOWN,       // 不明
    RECEIVER_TYPE_LAMBERT,       // Lambert
    RECEIVER_TYPE_BLINN,         // Blinn
    RECEIVER_TYPE_VERTEX_COLOR,  // 頂点カラー
    RECEIVER_TYPE_ADD,           // 乗算するマテリアル設定がされたレシーバー  // @note RECEIVER_TYPE_ADD→何かの覚え書き？
  };


  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	シャドウの設定内容
  //=====================================
  struct Description
  {
    u32                        shadow_group_num;
    ShadowGroup::Description*  shadow_group_description_array;  // shadow_group_num分の配列もしくはNULLを渡して下さい。
        // 例
        // gfl::grp::g3d::Shadow::Description         shadow_description;
        // gfl::grp::g3d::ShadowGroup::Description    shadow_group_description_array[3];
        // shadow_description.shadow_group_num               = 3;
        // shadow_description.shadow_group_description_array = shadow_group_description_array;
    
    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : shadow_group_num(1),
        shadow_group_description_array(NULL)
    {}
  };


  //---------------------------------------------------------------------------
  /**
   *           static関数宣言
   */
  //---------------------------------------------------------------------------
  static ReceiverType CheckShadowReceiverModel(
      const Model*                      model,
      const s32                         material_index
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief    リソースにあるテクスチャを影のテクスチャとして使ったプロジェクションシャドウのreceiverをセットアップする
   *            この影システム以外からも呼び出してもよいつくりになっている。
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  receiver_type
   *  @param[in]  shadow_intensity               0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
   *                                             shadow_intensity=1のときres_shadow_textureの色のままの影になる。
   *                                             shadow_intensity=0のとき影は出ない。
   *  @param[in]  projection_scene_camera_index
   *  @param[in]  texture_resource_pack          影のところは白でない色(黒とか)、影でないところは白になっている。
   *  @param[in]  texture_resource               影のところは白でない色(黒とか)、影でないところは白になっている。
   *  @param[in]  index_in_resource
   */
  //-----------------------------------------------------------------------------
  static void SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      ReceiverType receiver_type,
      f32          shadow_intensity,
      s32          projection_scene_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static void SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      ReceiverType receiver_type,
      f32          shadow_intensity,
      s32          projection_scene_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );

#if 0
  //-----------------------------------------------------------------------------
  /**
   *  @brief    リソースにあるテクスチャを影のテクスチャとして使ったプロジェクションシャドウのreceiverをOn/Offする
   *            この影システム以外からも呼び出してもよいつくりになっている。
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  shadow_intensity               0～1  // 1-( (1-texture_resource)*shadow_intensity ) が影の色になる。
   *                                             shadow_intensity=1のときtexture_resourceの色のままの影になる。
   *                                             shadow_intensity=0のとき影は出ない。
   *  @param[in]  projection_camera_index
   *  @param[in]  texture_resource_pack          影のところは白でない色(黒とか)、影でないところは白になっている。
   *  @param[in]  texture_resource               影のところは白でない色(黒とか)、影でないところは白になっている。
   *  @param[in]  index_in_resource
   */
  //-----------------------------------------------------------------------------
  static void SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      f32          shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static void SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      f32          shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static void SwitchOffProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );
#endif



  //-----------------------------------------------------------------------------
  /*
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   * 
   */
  //-----------------------------------------------------------------------------
  // gfx
  static b32 SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SetupProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 CleanupProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SwitchOnProjectionShadowReceiver(
      Model*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 SwitchOffProjectionShadowReceiver(
      Model*       model,
      s32          material_index
  );


#if GFL_GRP_G3D_H3D_USE
  // h3d
  static b32 SetupProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 CleanupProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index,
      const gfl::grp::ColorF32&  shadow_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 SwitchOffProjectionShadowReceiver(
      H3dModel*       model,
      s32          material_index
  );

#endif // GFL_GRP_G3D_H3D_USE

  //-----------------------------------------------------------------------------
  /**
   *  @brief    リソースにあるテクスチャを光のテクスチャとして使ったプロジェクションライトのreceiverをOn/Offする
   *            この影システム以外からも呼び出してもよいつくりになっている。
   *            Setupはデフォルトの設定を行いOnにする
   *            Cleanupは最低限の設定戻しを行いOffにする(完全に設定を戻すことはできません)
   *            SwitchOnは設定を変更せずOnにする
   *            SwitchOffは設定を変更せずにOffにする
   *
   *  @param[in]  model
   *  @param[in]  material_index
   *  @param[in]  light_intensity                0～1  // texture_resource*light_intensity が光の色になる。
   *                                             light_intensity=1のときtexture_resourceの色のままの光になる。
   *                                             light_intensity=0のとき光は出ない。
   *  @param[in]  projection_camera_index
   *  @param[in]  texture_resource_pack          加算するところは黒でない色(白とか)、加算しないところは黒になっている。
   *  @param[in]  texture_resource               加算するところは黒でない色(白とか)、加算しないところは黒になっている。
   *  @param[in]  index_in_resource
   *
   *  @retval     b32   On/Offできたらtrueを返す。On/Offできなかったらfalseを返す(ユーザデータのProjectionTypeが間違っている場合などfalseになる)
   */
  //-----------------------------------------------------------------------------
  /// gfx版
  static b32 SetupProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SetupProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  
  static b32 CleanupProjectionLightReceiver(
      Model*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource::TextureResourcePack* texture_resource_pack
  );
  static b32 SwitchOnProjectionLightReceiver(
      Model*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      Resource*    texture_resource,
      s32          index_in_resource
  );
  static b32 SwitchOffProjectionLightReceiver(
      Model*       model,
      s32          material_index
  );

#if GFL_GRP_G3D_H3D_USE
  /// H3d版
  static b32 SetupProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  
  static b32 CleanupProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index
  );

  static b32 SwitchOnProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index,
      //f32          light_intensity,
      const gfl::grp::ColorF32&  light_intensity,
      s32          projection_camera_index,
      H3dResTexture*    texture_resource
  );
  static b32 SwitchOffProjectionLightReceiver(
      H3dModel*       model,
      s32          material_index
  );

#endif // GFL_GRP_G3D_H3D_USE

  // friendクラスからのみアクセスしてもよいprivateメンバ関数
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  ShadowSystem(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~ShadowSystem();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成 / 破棄
   */
  //-----------------------------------------------------------------------------
  void Create(
      gfl::heap::NwAllocator*         heap_allocator,
      gfl::heap::NwAllocator*         device_allocator,
      const Description*              description = NULL );
  void Destroy(void);
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         影のreceiverとなるモデルをセットアップする
   */
  //-----------------------------------------------------------------------------
  void SetupShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         影のreceiverとしてセットアップ済みのモデル(SetupShadowReceiverModelが完了しているモデル)のOn/Off
   */
  //-----------------------------------------------------------------------------
  void SwitchShadowReceiverModel(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, b32 receive_on);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         影のreceiverとしてセットアップ済みのモデル(SetupShadowReceiverModelが完了しているモデル)の影の強度を設定する
   */
  //-----------------------------------------------------------------------------
  void SetShadowReceiverModelShadowIntensity(Model* model, s32 material_index, s32 shadow_group_index, ReceiverType receiver_type, f32 shadow_intensity);
      // @note 使っていない引数があるので、引数を精査したほうがいい。

  //-----------------------------------------------------------------------------
  /**
   *  @brief         シーン描画グループを影用にセットアップする
   */
  //-----------------------------------------------------------------------------
  void  SetupSceneDrawGroupForShadow(const gfl::grp::g3d::ShadowGroup::Type type, gfl::grp::g3d::SceneDrawGroup* scene_draw_group) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         影グループの数を取得
   */
  //-----------------------------------------------------------------------------
  u32  GetShadowGroupNum(void) const  { return m_shadow_group_num; }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         影グループの情報を取得
   *
   *  @param[in]     shadow_group_index  m_shadow_group_arrayのインデックス
   */
  //-----------------------------------------------------------------------------
  gfl::grp::g3d::ShadowGroup::Type  GetShadowGroupType(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetType();
  }
  s32                               GetShadowGroupSceneDrawGroupIndex(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetSceneDrawGroupIndex();
  }
  s32                               GetShadowGroupCameraIndex(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetCameraIndex();
  }
  gfl::grp::g3d::Camera*            GetShadowGroupCamera(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetCamera();
  }
  gfl::grp::RenderTarget*           GetShadowGroupRenderTarget(const s32 shadow_group_index) const
  {
    return m_shadow_group_array[shadow_group_index]->GetRenderTarget();
  }




private:
  enum
  {
    SHADOW_SETTING_RESOURCE_MODEL_PROJECTION_SHADOW   = 0,
    SHADOW_SETTING_RESOURCE_MODEL_SHADOW_MAP,
    SHADOW_SETTING_RESOURCE_MAX,
    // 影用のシェーダが2個になるのはもったいないので、
    // 1モデルに2マテリアル1シェーダか
    // 1モデルに1マテリアル0シェーダを2個とシェーダだけを1個にするか

    SHADOW_SETTING_MODEL_PROJECTION_SHADOW   = 0,
    SHADOW_SETTING_MODEL_SHADOW_MAP,
    SHADOW_SETTING_MODEL_MAX
  };

private:
  Resource** m_shadow_setting_resource_array;
  Model**    m_shadow_setting_model_array;

  u32                             m_shadow_group_num;
  gfl::grp::g3d::ShadowGroup**    m_shadow_group_array;




  // friendクラスからもアクセスしてはいけないprivateメンバ関数
  // 便利関数や処理内容を抽出した関数
private:
  static void SetupShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               shadow_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const f32                               shadow_intensity,  // 0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
                                                                 // shadow_intensity=1のときres_shadow_textureの色のままの影になる。
                                                                 // shadow_intensity=0のとき影は出ない。
      const ReceiverType                      receiver_type,
      const s32                               shadow_camera_index
  );
  static ReceiverType CheckShadowReceiverMaterial(
      const nw::gfx::Material*                material
  );
  static void SwitchOnShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               shadow_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const ReceiverType                      receiver_type
  );
  static void SwitchOffShadowReceiverMaterial(
      nw::gfx::Material*                      material,
      const nw::gfx::ResTexture               dummy_res_texture,  // 影のところは白でない色(黒とか)、影でないところは白になっている。
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const ReceiverType                      receiver_type
  );
  static void SetShadowReceiverMaterialShadowIntensity(
      nw::gfx::Material*                      material,
      const gfl::grp::g3d::ShadowGroup::Type  type,
      const f32                               shadow_intensity,  // 0～1  // 1-( (1-res_shadow_texture)*shadow_intensity ) が影の色になる。
                                                                 // shadow_intensity=1のときres_shadow_textureの色のままの影になる。
                                                                 // shadow_intensity=0のとき影は出ない。
      const ReceiverType                      receiver_type
  );  // @note 使っていない引数があるので、引数を精査したほうがいい。

  //-----------------------------------------------------------------------------
  /**
   *  @brief            プロジェクションシャドウ用のマテリアルとシェーダーを含むモデルのセットアップ
   *
   *  @param[in]        shadow_setting_model    セットアップするモデル
   */
  //-----------------------------------------------------------------------------
  static void SetupProjectionShadowSettingModel( nw::gfx::Model* shadow_setting_model );
  //-----------------------------------------------------------------------------
  /**
   *  @brief            シャドウマップ用のマテリアルとシェーダーを含むモデルのセットアップ
   *
   *  @param[in]        shadow_setting_model    セットアップするモデル
   */
  //-----------------------------------------------------------------------------
  static void SetupShadowMapSettingModel( nw::gfx::Model* shadow_setting_model );

  //----------------------------------------------------------------------------
  /**
   *	@brief  影の投影のセットアップ
   */
  //-----------------------------------------------------------------------------
  /// gfx版
  static void SetupProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource );
  static void CleanUpProjectionShadowModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color );

#if GFL_GRP_G3D_H3D_USE
  /// H3d版
  static void SetupProjectionShadowModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  shadow_intensity, s32 projection_camera_index, H3dResTexture* texture_resource );
  static void CleanUpProjectionShadowModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color );
#endif // GFL_GRP_G3D_H3D_USE
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  光の投影のセットアップ
   */
  //-----------------------------------------------------------------------------
  /// gfx版
  static void SetupProjectionLighModel( gfl::grp::g3d::Model* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, Resource* texture_resource, s32 index_in_resource );
  static void CleanUpProjectionLightModel( gfl::grp::g3d::Model* model, s32 material_index, const nw::ut::FloatColor& border_color );

#if GFL_GRP_G3D_H3D_USE
  /// H3d版
  static void SetupProjectionLighModel( H3dModel* model, s32 material_index, const gfl::grp::ColorF32&  light_intensity, s32 projection_camera_index, H3dResTexture* texture_resource );
  static void CleanUpProjectionLightModel( H3dModel* model, s32 material_index, const nw::ut::FloatColor& border_color );
#endif // GFL_GRP_G3D_H3D_USE
  
};


}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DSHADOWSYSTEM_H__
