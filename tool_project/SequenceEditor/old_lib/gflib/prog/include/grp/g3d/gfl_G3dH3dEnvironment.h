#ifndef __GFL_G3DH3DENVIRONMENT_H__
#define __GFL_G3DH3DENVIRONMENT_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dH3dEnvironment.h
 *	@brief  H3D環境
 *	@author	Koji Kawada
 *	@date		2012.04.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


/*@note 注意事項

カメラを用意したときは、次のことに注意すると効率がよくなりそう。

      nn::math::MTX44 projection = nn::math::MTX44::Identity();
      nn::math::MTX34 view = nn::math::MTX34::Identity();

      // すべてのシェーダのプロジェクションとビューの Uniform を同じ番号のレジスタにすることで、
      // 共通の BindSymbol で値を設定します。
      nn::gr::BindSymbolVSFloat projMtxBind;
      nn::gr::BindSymbolVSFloat viewMtxBind;
      projMtxBind.start = 86;
      projMtxBind.end = 89;
      viewMtxBind.start = 90;
      viewMtxBind.end = 92;

      bit32* command;
      nngxGetCmdlistParameteri(NN_GX_CMDLIST_CURRENT_BUFADDR, reinterpret_cast<GLint*>(&command));
      bit32* beginCommand = command;
      command = projMtxBind.MakeUniformCommand(command, projection);
      command = viewMtxBind.MakeUniformCommand(command, view);
      size_t commandSize = (command - beginCommand) * sizeof(bit32);
      nngxMoveCommandbufferPointer(commandSize);



説明

このH3dEnvironmentでは
nw::h3d::SceneState
を扱います。
*/

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



namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	H3D環境クラス
//=====================================
class H3dEnvironment
{
  GFL_FORBID_COPY_AND_ASSIGN(H3dEnvironment);


  friend class H3dModel;
  friend class RenderSystem;


public:
  
  //---------------------------------------------------------------------------
  //! @brief 同時適用できる頂点ライトのセットです。　@fix 渕野さん要望：バトル背景のライティング対応
  //---------------------------------------------------------------------------
  struct VertexLightSet
  {
      static const int MAX_VERTEX_LIGHT_QUANTITY = 8;

      nw::h3d::res::VertexLightContent vertexLights[MAX_VERTEX_LIGHT_QUANTITY];
      nw::h3d::res::HemiSphereLightContent hemisphereLight;
      u8 vertexLightSize;
      NN_PADDING3;

      VertexLightSet() : vertexLightSize(0) {}

      nn::math::VEC4 m_position;
      
      void MakeCommand(
          nn::gr::CTR::CommandBufferJumpHelper& dcs,
          const nn::gr::CTR::BindSymbolVSInteger& vertexLightCnt,
          const nn::gr::CTR::BindSymbolVSFloat& univMtxBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslGColBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslSColBind,
          const nn::gr::CTR::BindSymbolVSFloat& hslSDirBind) const;
  };
  typedef nw::h3d::fnd::DataArray<VertexLightSet> VertexLightSetArray;


  // レンダーモード
  enum
  {
    RENDER_MODE_BIT_FLAG_NONE             = 0,        // なし
    RENDER_MODE_BIT_FLAG_IGNORE_MATERIAL  = 1 <<  0,  // マテリアルの設定を無視する
    //RENDER_MODE_BIT_FLAG_IGNORE_SHADER    = 1 <<  1,  // シェーダの設定を無視する

    RENDER_MODE_BIT_FLAG_DEFAULT  = RENDER_MODE_BIT_FLAG_NONE    // デフォルト
  };

private:
  enum
  {
    RENDER_MODE_BIT_FLAG_IGNORE_SHADER    = 1 <<  1,  // シェーダの設定を無視する  // マテリアルの設定を無視したらシェーダの設定も無視するように
                                                                                   // しているので、これは不要な気がするので、今はアクセス不可。
  };

public:
  // 設定内容
  struct Description
  {
    s32 texture_projector_num_max;
    s32 light_set_num_max;
    s32 fog_num_max;

    Description()
      : texture_projector_num_max(0),
        light_set_num_max(0),
        fog_num_max(0)
    {}
  };


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
  H3dEnvironment(void);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~H3dEnvironment();



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 生成 / 破棄
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   */
  //-----------------------------------------------------------------------------
  void Create(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description& a_desc);
  void CreateFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene);
  //使用禁止void CreateAndSetFromScene(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Scene* scene);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // 設定
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////

  //-----------------------------------------------------------------------------
  /**
   *  @brief         Sceneから設定する
   *  @attention     この関数は毎フレーム呼ばないとGPUハングアップするかもしれません。
   */
  //-----------------------------------------------------------------------------
  void SetFromScene(const Scene* scene, const nw::gfx::Camera& camera);



  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  // レンダーモード
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // @brief  レンダーモードを設定する
  //         ビットを追加で立てるのではなく、メンバ変数にそのまま代入します。
  void SetRenderModeBitFlag(bit32 bit_flag);
  // @brief  レンダーモードを取得する
  bit32 GetRenderModeBitFlag(void) const;

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  //
  //　ライトカラー取得　@fix ともやさん、麻衣子さん要望：フィールドエンカウント草表示最適化のための対応
  //
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  const nn::gr::FragmentLight& GetFragmentLight( s32 lightSetNo ) const;
  const nw::h3d::fnd::ColorSet& GetLightColor( s32 lightSetNo, int lightIdx ) const;

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
  nw::h3d::SceneState* GetNwSceneState(void) { return (&m_scene_state); }
      // 「GetNwSceneState(void) const」だと戻り値の型を「const nw::h3d::SceneState*」にしないといけない
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   */
  //-----------------------------------------------------------------------------
  void CreateFromSceneEnvironment(gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const nw::gfx::SceneEnvironment& env, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         nw::gfx::SceneEnvironmentから設定する
   *  @attention     この関数は毎フレーム呼ばないとGPUハングアップするかもしれません。
   */
  //-----------------------------------------------------------------------------
  void SetFromSceneEnvironment(const nw::gfx::SceneEnvironment& env, const nw::gfx::Camera& view_camera, s32 max_cameras, s32 max_light_sets, s32 max_fogs, s32 max_vertex_lights);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         フォグを設定するコマンドを生成する
   */
  //-----------------------------------------------------------------------------
  bit32* MakeFogAllCommand(s32 index, bit32* command) const;
  bit32* MakeFogConfigCommand(s32 index, bit32* command) const;
  bit32* MakeFogTableCommand(s32 index, bit32* command) const;
  bit32* MakeFogDisableCommand(s32 index, bit32* command) const;


  VertexLightSet* GetVertexLightSet( s32 index ) const;

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
  static s32 getLutDataEnableIndex(s32 light_set_index, s32 light_index);
  b32 setLutData(nw::gfx::res::ResLookupTable lut, s32 light_set_index, s32 light_index);  // セットできたらtrueを返し、セットできなかったらfalseを返す

private:
  //
  void*  m_buff;
  nw::h3d::SceneState  m_scene_state;
  VertexLightSetArray  m_VertexLightSets;
  VertexLightSet       *m_pVertexLightSetBuffer;

  // ライト
  u32                     m_max_light_sets;   // ライトセット数
  gfl::base::Bit*         m_lut_data_enable;  // m_max_light_sets*nw::h3d::Constants::LIGHT_QUANTITYだけメモリを確保する
                                              // 順番はライトセット0の01234567,ライトセット1の01234567,ライトセット2の01234567,...
  nw::h3d::res::LutData** m_lut_data_array;   // m_lut_data_array[m_max_light_sets][nw::h3d::Constants::LIGHT_QUANTITY]となるようにメモリを確保する




  //aaaaaaaaaaaaaaaaa
  static const int LUTDATA_BUFFER_SIZE = 1032;
  static const int SUBROUTINE_JUMP_COMMAND_SIZE = 8;
  struct LutDataCache
  {
      u8 lutDataBuffer[LUTDATA_BUFFER_SIZE + SUBROUTINE_JUMP_COMMAND_SIZE];
      nw::h3d::res::LutData lutData;
  };
  LutDataCache*** m_lut_data_cache_array;




  // フォグ
  const u32** m_fog_command_cache_array;

  //
  bit32  m_render_mode_bit_flag;  // @note 将来のハードで並列処理で描画するようになったら、シーンに1個しかない環境にレンダーモードを持たせておいて
                                  //       メッシュ描画時にこれを変更できてしまうのは危険。レンダーモードをモデルごと(メッシュごと？)に持ったほうがいいかも。
};



}  // namespace g3d
}  // namespace grp
}  // namespace gfl



#endif  // GFL_GRP_G3D_H3D_USE



#endif // __GFL_G3DH3DENVIRONMENT_H__

