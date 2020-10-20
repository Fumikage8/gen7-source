#ifndef __GFL_G3DBLOOMFILTER_H__
#define __GFL_G3DBLOOMFILTER_H__
#pragma once
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_G3dBloomFilter.h
 *	@brief  3Dブルームフィルタ
 *	@author	Koji Kawada
 *	@date		2011.04.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_RenderTarget.h>

// gflib grp g3d
#include <grp/g3d/gfl_G3dResource.h>
#include <grp/g3d/gfl_G3dModel.h>
#include <grp/g3d/gfl_G3dTexture.h>
#include <grp/g3d/gfl_G3dCamera.h>


// 切り替え
#define GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE (1)  // これが1のとき、フレームバッファの縦横サイズが2のべき乗になっており
                                                              // フレームバッファを1度別のテクスチャにコピーすることなくそのままテクスチャとして使用できる。
                                                              // これが0のとき、フレームバッファの縦横サイズは2のべき乗にはなっておらず
                                                              // フレームバッファを1度別のテクスチャにコピーしなければならない。


namespace gfl {
namespace grp {
namespace g3d {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	3Dブルームフィルタクラス
//=====================================
class BloomFilter
{
  GFL_FORBID_COPY_AND_ASSIGN(BloomFilter);


  // 下記クラスからはprivate関数にもアクセスしてよい
  friend class Scene;
  friend class G3DPostEffectSystem;


  // 用語
  // plain_image           高輝度を抽出される元画像
  // high_luminance        plain_imageから高輝度を抽出する過程や結果に用いる
  // high_luminance_board  high_luminance板
  // bloom_board           「high_luminance_boardを描画してできた高輝度のみの画像」からつくった「溢れ出す光(ブルーム)の画像」を貼った板。ブルームを加算描画する板。
  // bloom_filter          総称
  // small_image           「high_luminance_boardを描画してできた高輝度のみの画像」の縮小画像


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
public:
#if 0
  //f32にしたのでボツ
  //---------------------------------------------------------------------------
  /**
   *           定数宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  /// bloom_boardの広がり 
  //=====================================
  enum BloomBoardExpanse
  {
    BLOOM_BOARD_EXPANSE_ZERO,
    BLOOM_BOARD_EXPANSE_SMALL,
    BLOOM_BOARD_EXPANSE_MIDDLE,
    BLOOM_BOARD_EXPANSE_LARGE
  };
#endif

  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	bloom_boardの設定内容
  //=====================================
  struct BloomBoardDescription
  {
    u32              small_image_count;   // 貼り付ける画像は縮小回数何回のものか(0のときm_high_luminance_board_render_target、
                                          // nのときm_small_image_array[n-1]を貼り付ける。n<=m_small_image_num)
    gfl::math::VEC2  uv;                  // UV座標(-1.0f<= <=1.0f)
    f32              rate;                // 加算する際の割合(0.0f<= <=1.0f)
  };

  // BloomBoardDescriptionの初期化関数
  // デフォルト値を設定する
  static void InitializeBloomBoardDescriptionForDefault(
      BloomBoardDescription*  description
  );

  // BloomBoardDescriptionの初期化関数
  static void InitializeBloomBoardDescription(
      BloomBoardDescription*  description,
      const u32              small_image_count,
      const gfl::math::VEC2& uv,
      const f32              rate
  );

  //-------------------------------------
  ///	設定内容
  //=====================================
  struct Description
  {
    s32                  high_luminance_board_render_target_width;   // テクスチャとして用いるレンダーターゲットなので、幅、高さどちらも次行の条件を満たすこと。長方形でもよい。
    s32                  high_luminance_board_render_target_height;  // 2のべき乗。8<= <=1024。FORMAT_HW_ETC1のときは16<= <=1024。nngxFilterBlockImageの説明を見てsmall_image_numにも注意すること。
    Texture::Format      high_luminance_board_render_target_format;  // レンダーターゲットに使えるフォーマットgfl::grp::RenderColorFormatと対応したものを渡すこと。これはsmall_imageのフォーマットにもなります。
    gfl::grp::MemoryArea high_luminance_board_render_target_color_memory_area;  // @note アドレス指定も必要か？
    s32                  high_luminance_board_scene_calculate_group_index;
    s32                  high_luminance_board_scene_draw_group_index;
    gfl::grp::ColorF32   high_luminance_color_weight;                    // 輝度計算に用いるRGBの比率
    f32                  high_luminance_min_value;                       // 高輝度とみなす値
    u32       small_image_num;          // 縮小回数(0のとき1回も縮小しない)
    s32                     bloom_board_scene_calculate_group_index;
    s32                     bloom_board_scene_draw_group_index;
    u32                     bloom_board_num;
    const BloomBoardDescription*  bloom_board_description_array;  // bloom_board_num分の配列もしくはNULLを渡して下さい。
        // 例
        // gfl::grp::g3d::BloomFilter::Description              bloom_filter_description;
        // gfl::grp::g3d::BloomFilter::BloomBoardDescription    bloom_board_description_array[3];
        // bloom_filter_description.bloom_board_num               = 3;
        // bloom_filter_description.bloom_board_description_array = bloom_board_description_array;
    s32       camera_scene_calculate_group_index;
    s32       camera_scene_camera_index;
    gfl::grp::MemoryArea  frame_buffer_copy_texture_memory_area;  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 のときのみ使用される値。
                                                                  // gfl::grp::MEMORY_AREA_NONEの場合は使用されないので、
                                                                  // plain_imageはフレームバッファではなくオフスクリーンバッファでなければならない。
  };

  // Descriptionの初期化関数
  // デフォルト値を設定する
  static void InitializeDescriptionForDefault(
      Description*  description
  );

  // Descriptionの初期化関数
  static void InitializeDescription(
      Description*  description,
      const s32                  high_luminance_board_render_target_width,
      const s32                  high_luminance_board_render_target_height,
      const Texture::Format      high_luminance_board_render_target_format,
      const gfl::grp::MemoryArea high_luminance_board_render_target_color_memory_area,
      const s32                  high_luminance_board_scene_calculate_group_index,
      const s32                  high_luminance_board_scene_draw_group_index,
      const gfl::grp::ColorF32&  high_luminance_color_weight,
      const f32                  high_luminance_min_value,
      const u32       small_image_num,
      const s32                     bloom_board_scene_calculate_group_index,
      const s32                     bloom_board_scene_draw_group_index,
      const u32                     bloom_board_num,
      const BloomBoardDescription*  bloom_board_description_array,
      const s32       camera_scene_calculate_group_index,
      const s32       camera_scene_camera_index,
      const gfl::grp::MemoryArea  frame_buffer_copy_texture_memory_area  // #if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE==0 のときのみ使用される値。
  ); 


  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // friendクラスからのみアクセスしてもよいprivateメンバ関数
private:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  BloomFilter(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~BloomFilter();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *
   *  @param[in]  heap_allocator            ヒープメモリのアロケータ
   *  @param[in]  device_allocator          デバイスメモリのアロケータ
   *  @param[in]  graphics_system           グラフィックスシステム
   *  @param[in]  frame_buffer_memory_area  オンスクリーンのフレームバッファのメモリエリア
   *  @param[in]  description               設定内容
   */
  //-----------------------------------------------------------------------------
  // コマンド蓄積
  void Create(
      gfl::heap::NwAllocator*         heap_allocator,
      gfl::heap::NwAllocator*         device_allocator,
      const gfl::grp::GraphicsSystem* graphics_system,
      const gfl::grp::MemoryArea      frame_buffer_memory_area,
      const Description*              description = NULL
  );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   */
  //-----------------------------------------------------------------------------
  virtual void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  Model*                   GetHighLuminanceBoardModel(void) const  {  return m_high_luminance_board_model;  }
  s32                      GetHighLuminanceBoardSceneCalculateGroupIndex(void) const  {  return m_high_luminance_board_scene_calculate_group_index;  }
  s32                      GetHighLuminanceBoardSceneDrawGroupIndex(void) const  {  return m_high_luminance_board_scene_draw_group_index;  }
  gfl::grp::RenderTarget*  GetHighLuminanceBoardRenderTarget(void) const  {  return m_high_luminance_board_render_target;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  u32  GetSmallImageNum(void) const  { return m_small_image_num; } 

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  s32     GetBloomBoardSceneCalculateGroupIndex(void) const  {  return m_bloom_board_scene_calculate_group_index;  }
  s32     GetBloomBoardSceneDrawGroupIndex(void) const  {  return m_bloom_board_scene_draw_group_index;  }
  u32     GetBloomBoardNum(void) const  {  return m_bloom_board_num;  }
  Model*  GetBloomBoardModel(const u32 bloom_board_index) const  {  return m_bloom_board_array[bloom_board_index].model;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         取得
   */
  //-----------------------------------------------------------------------------
  s32      GetCameraSceneCalculateGroupIndex(void) const  {  return m_dummy_camera_scene_calculate_group_index;  }
  s32      GetCameraSceneCameraIndex(void) const  {  return m_dummy_camera_scene_camera_index;  }
  Camera*  GetCamera(void) const  {  return m_dummy_camera;  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief         縮小コピーを行う
   */
  //-----------------------------------------------------------------------------
  // コマンド蓄積(を行う関数を呼ぶ)
  void CopySmallerAll(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         高輝度を抽出される元画像を設定する
   *                 オンスクリーンのフレームバッファを元画像とする場合
   */
  //-----------------------------------------------------------------------------
  // コマンド蓄積
  void SetPlainImageFrameBufferToHighLuminanceBoard(
      gfl::grp::GraphicsSystem*       graphics_system,
      const gfl::grp::DisplayType     plain_image_display_type
  );
  
  //-----------------------------------------------------------------------------
  /**
   *  @brief         高輝度を抽出される元画像を設定する
   *                 オフスクリーンを元画像とする場合
   */
  //-----------------------------------------------------------------------------
  // コマンド蓄積
  void SetPlainImageOffScreenBufferToHighLuminanceBoard(const gfl::grp::RenderTarget* plain_image_off_screen_buffer);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         設定 / 取得
   */
  //-----------------------------------------------------------------------------
  void SetHighLuminanceColorWeight(const gfl::grp::ColorF32& c);
  void GetHighLuminanceColorWeight(gfl::grp::ColorF32* c) const;

  void SetHighLuminanceMinValue(const f32 v);
  f32  GetHighLuminanceMinValue(void) const;

  // bloom_boardの広がりを設定/取得する
  void SetBloomBoardExpanse(const f32 bloom_board_expanse);
  f32  GetBloomBoardExpanse(void) const;
  // bloom_boardの強さを設定/取得する
  void SetBloomBoardIntensity(const f32 bloom_board_intensity);
  f32  GetBloomBoardIntensity(void) const;
  // bloom_boardに貼り付ける画像は縮小回数何回のものか(0のときm_high_luminance_board_render_target、
  // nのときm_small_image_array[n-1]を貼り付ける。n<=m_small_image_num)
  void SetBloomBoardSmallImageCount(const u32 bloom_board_index, const u32 small_image_count);
  u32  GetBloomBoardSmallImageCount(const u32 bloom_board_index) const;

  // @note 他に調整する項目を増やすとしたら、、、
  // 1個1個のbloom_boardの配置を動かせるようにする
  // 、、、くらいかなあ




  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // friendクラスからもアクセスしてはいけないprivateメンバ関数
private:
  // 縮小コピーを行う
  // コマンド蓄積
  void CopySmaller( const nw::gfx::res::ResTexture src_texture, nw::gfx::res::ResTexture dst_texture );


  ///////////////////////////////////////////////////////////////////////////////
  // friendクラスからもアクセスしてはいけないprivateメンバ変数
private:
  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	bloom_boardの情報をまとめた構造体
  //=====================================
  struct BloomBoard
  {
    Resource*        resource;
    Model*           model;               // 板ポリゴンモデル
    u32              small_image_count;
    gfl::math::VEC2  uv;
    f32              rate;
  };


private:
#if GFL_GRP_G3D_BLOOM_FILTER_FRAME_BUFFER_IS_TEXTURE
  gfl::grp::RenderTarget*    m_frame_buffer_render_target_array[gfl::grp::DISPLAY_BOTH_MAX];  // オンスクリーンバッファであるフレームバッファと同じカラーバッファを持つオフスクリーンバッファ
#else
  gfl::grp::g3d::Texture*    m_frame_buffer_copy_texture;  // オンスクリーンバッファであるフレームバッファの絵がコピーされるテクスチャ。
                                                           // フレームバッファは400*240だが、
                                                           // このテクスチャは512*256(テクスチャは2のべき乗でなければならないので)にしてある。
#endif

  Resource*               m_high_luminance_board_resource;
  Model*                  m_high_luminance_board_model;
  s32                     m_high_luminance_board_scene_calculate_group_index;
  s32                     m_high_luminance_board_scene_draw_group_index;
  gfl::grp::RenderTarget* m_high_luminance_board_render_target;

  u32        m_small_image_num;
  Texture**  m_small_image_array;  // m_small_image_array = GFL_NEW_ARRAY() Texture*[m_small_image_num];
                                   // m_small_image_array[n-1]はn回縮小画像(1<=n)
                                   // 0回縮小画像は、即ち1回も縮小していない画像は、m_high_luminance_board_render_target

  s32          m_bloom_board_scene_calculate_group_index;
  s32          m_bloom_board_scene_draw_group_index;
  u32          m_bloom_board_num;
  BloomBoard*  m_bloom_board_array;
  f32          m_bloom_board_expanse;
  f32          m_bloom_board_intensity;

  Camera*  m_dummy_camera;
  s32      m_dummy_camera_scene_calculate_group_index;
  s32      m_dummy_camera_scene_camera_index;




#if GFL_DEBUG
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  // check_boardはデバッグ用途

  ///////////////////////////////////////////////////////////////////////////////
  // friendクラスからもアクセスしてはいけないprivateメンバ変数
private:
  Resource* m_texture_check_board_resource;
  Model*    m_texture_check_board_model;
  
  ///////////////////////////////////////////////////////////////////////////////
  // friendクラスからもアクセスしてはいけないprivateメンバ関数
private:
  void CreateTextureCheckBoardModel(
      gfl::heap::NwAllocator*  heap_allocator,
      gfl::heap::NwAllocator*  device_allocator
  );
  void DestroyTextureCheckBoardModel(void);
  
  ///////////////////////////////////////////////////////////////////////////////
  // friendクラスからのみアクセスしてもよいprivateメンバ関数
private:
  Model* GetTextureCheckBoardModel(void) const  {  return m_texture_check_board_model;  }
#if 0
//うまく表示されないので、使用禁止。
  // コマンド蓄積
  void   SetImageFrameBufferToTextureCheckBoard(const gfl::grp::DisplayType image_display_type);
#endif
  // コマンド蓄積
  void   SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer);
  // コマンド蓄積
  void   SetImageOffScreenBufferToTextureCheckBoard(const gfl::grp::RenderTarget* image_off_screen_buffer, const gfl::math::VEC2& uv_scale, const gfl::math::VEC2& uv_translate);

  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////
#endif  // #if GFL_DEBUG

};

}  // namespace g3d
}  // namespace grp
}  // namespace gfl

#endif // __GFL_G3DBLOOMFILTER_H__
