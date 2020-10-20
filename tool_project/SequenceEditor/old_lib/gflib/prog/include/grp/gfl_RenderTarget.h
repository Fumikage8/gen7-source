#ifndef __GFL_RENDERTARGET_H__
#define __GFL_RENDERTARGET_H__
#pragma once

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_RenderTarget.h
 *	@brief  レンダーターゲット
 *	@author	Koji Kawada
 *	@date		2011.03.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// CTR_SDK

// NW4C
#include <nw/gfx.h>

// gflib
#include <gfl_fs.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>

// gflib grp
#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_Viewport.h>



namespace gfl {
namespace grp {


namespace g3d {
  class Scene;
  class RenderSystem;
}  // namespace g3d


//-----------------------------------------------------------------------------
/**
 *					クラス宣言
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	レンダーターゲットクラス
//=====================================
class RenderTarget
{
  GFL_FORBID_COPY_AND_ASSIGN(RenderTarget);


  // 下記クラスにだけprivateメンバ関数へのアクセスを許す
  friend class gfl::grp::g3d::Scene;
  friend class gfl::grp::g3d::RenderSystem;


public:
  //---------------------------------------------------------------------------
  /**
   *           定数
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	描画対象のタイプ
  //=====================================
  enum Type
  {
    TYPE_NONE,              // データなし
    TYPE_ON_SCREEN_BUFFER,  // 画面に表示させるための描画対象
    TYPE_OFF_SCREEN_BUFFER  // 画面に表示しない描画対象
  };

  //-------------------------------------
  ///	バッファタイプ
  //=====================================
  enum RenderBufferType
  {
    RENDERBUFFER_TYPE_COLOR, // カラーバッファ
    RENDERBUFFER_TYPE_DEPTH  // デプスバッファ
  };
  
  //---------------------------------------------------------------------------
  /**
   *           構造体宣言
   */
  //---------------------------------------------------------------------------
  //-------------------------------------
  ///	設定内容
  //=====================================
  struct Description 
  {
    Type                         type;           // 描画対象のタイプ
    s32                          width;          // 幅
    s32                          height;         // 高さ
    gfl::grp::RenderColorFormat  color_format;   // 色の形式
    gfl::grp::RenderDepthFormat  depth_format;   // 深度の形式
    gfl::grp::MemoryArea         color_area;     // カラーバッファのメモリ配置場所(color_addressが0以外のときは使用しない)
    gfl::grp::MemoryArea         depth_area;     // デプスバッファのメモリ配置場所(depth_addressが0以外のときは使用しない)
    u32                          color_address;  // カラーバッファのアドレス直接指定(0のときはcolor_areaを使用する)
    u32                          depth_address;  // デプスバッファのアドレス直接指定(0のときはdepth_areaを使用する)
    gfl::grp::ShadowType         shadow_kind;    // シャドウの種別

    // (type==TYPE_OFF_SCREEN_BUFFER) のとき使用する設定
    s32                          mipmap_size;            // ミップマップサイズ
                                                         // (ミップマップを使用しないときは1を、ミップマップを使用するときはミップマップ数+1を指定する)
    gfl::grp::MemoryTransfer     color_transfer;         // テクスチャのメモリ転送タイプ
    b32                          executing_memory_fill;  // 確保したピクセル領域を0でクリアする場合true
    b32                          is_dynamic_allocation;  // 動的にFCRAM上に領域を確保する場合はtrue
    // レンダーターゲットにする場合、テクスチャのフォーマットは
    // RGBA8,
    // RGB8,
    // RGB5_A1,
    // RGB565,
    // RGBA4
    // しか使えないので、gfl::grp::RenderColorFormatに一致する。

    // (type==TYPE_OFF_SCREEN_BUFFER && shadow_kind==SHADOW_KIND_TEXTURE) のとき使用する設定
    b32                          is_perspective_shadow;  // 投影シャドウの場合はtrue
    f32                          shadow_z_bias;          // シャドウテクスチャのZバイアス値
    f32                          shadow_z_scale;         // オフセット値に乗算するスケール
    gfl::grp::DisplayType        display_type;           // 描画面情報
    GLuint                       fob_id;                 // 確保済みGL用フレームバッファオブジェクト（ない場合０）
     
    //-----------------------------------------------------------------------------
    /**
     *  @brief         コンストラクタ
     */
    //-----------------------------------------------------------------------------
    Description(void)
      : type                  (TYPE_OFF_SCREEN_BUFFER),
        width                 (512),
        height                (256),
        color_format          (gfl::grp::RENDER_COLOR_FORMAT_RGBA8),
        depth_format          (gfl::grp::RENDER_DEPTH_FORMAT_NONE),
        color_area            (gfl::grp::MEMORY_AREA_VRAMA),
        depth_area            (gfl::grp::MEMORY_AREA_NONE),
        color_address         (0),
        depth_address         (0),
        shadow_kind           (gfl::grp::SHADOW_KIND_NONE),
        mipmap_size           (1),
        color_transfer        (gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP),
        executing_memory_fill (false),
        is_dynamic_allocation (false),
        is_perspective_shadow (true),
        shadow_z_bias         (0.0f),
        shadow_z_scale        (1.0f),
        display_type          (gfl::grp::DISPLAY_NONE),
        fob_id                (0)
    {}

  };


  //----------------------------------------------------------------------------
  /**
   *	@brief  OnScreenRenderTarget用Descriptionの初期化
   *
   *	通常のレンダーターゲットです。
   *
   *	カラーバッファあり。
   *	デプスバッファあり。
   *
   *  @param  p_desc                データ設定先
   *	@param	width                 幅
   *	@param	height                高さ
   *	@param	color_format          カラーフォーマット
   *	@param	depth_format          デプスフォーマット
   *	@param	color_area            カラーバッファのメモリ配置場所
   *	@param	depth_area            デプスバッファのメモリ配置場所
   *	@param	display_type          ディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  static void InitializeOnScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type );



  //----------------------------------------------------------------------------
  /**
   *	@brief  OffScreenRenderTarget用のDescriptionを初期化
   *
   * カラーバッファをテクスチャとして使用することができるタイプのレンダーターゲットです。
   * デプスバッファはありません。
   *
   *	カラーバッファを通常テクスチャとして用意
   *	デプスバッファなし。
   *
   *  @param  p_desc                データ設定先
   *	@param	width                 幅
   *	@param	height                高さ
   *	@param	color_format          カラーフォーマット
   *	@param	color_area            カラーバッファメモリエリア
   *	@param	display_type          ディスプレイタイプ
   *  @param  mipmap_size           ミップマップサイズ
   *	@param	color_transfer        テクスチャのメモリ転送タイプ
   *	@param  executing_memory_fill 確保したピクセルを0でクリアするか？ する場合true
   *	@param  is_dynamic_allocation 動的にFCRAM上に領域を確保するか？ する場合true
   */
  //-----------------------------------------------------------------------------
  static void InitializeOffScreenDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::MemoryArea color_area, gfl::grp::DisplayType display_type, s32 mipmap_size = 1, gfl::grp::MemoryTransfer color_transfer = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP, b32 executing_memory_fill = false, b32 is_dynamic_allocation = false );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  OffScreenDepthRenderTarget用のDescriptionを初期化
   *
   * カラーバッファをテクスチャとして使用することができるタイプのレンダーターゲットです。
   * デプスバッファも用意します。
   *
   *	カラーバッファを通常テクスチャとして用意
   *	デプスバッファあり。
   *
   *  @param  p_desc                データ設定先
   *	@param	width                 幅
   *	@param	height                高さ
   *	@param	color_format          カラーフォーマット
   *	@param	depth_format          デプスフォーマット
   *	@param	color_area            カラーバッファメモリエリア
   *	@param	depth_area            デプスバッファメモリエリア
   *	@param	display_type          ディスプレイタイプ
   *  @param  mipmap_size           ミップマップサイズ
   *	@param	color_transfer        テクスチャのメモリ転送タイプ
   *	@param  executing_memory_fill 確保したピクセルを0でクリアするか？ する場合true
   *	@param  is_dynamic_allocation 動的にFCRAM上に領域を確保するか？ する場合true
   */
  //-----------------------------------------------------------------------------
  static void InitializeOffScreenDepthDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, gfl::grp::MemoryArea color_area, gfl::grp::MemoryArea depth_area, gfl::grp::DisplayType display_type, s32 mipmap_size = 1, gfl::grp::MemoryTransfer color_transfer = gfl::grp::MEMORY_TRANSFER_NO_COPY_FCRAM_DMP, b32 executing_memory_fill = false, b32 is_dynamic_allocation = false );

  //----------------------------------------------------------------------------
  /**
   *	@brief  DirectAddressRenderTarget用のDescriptionを初期化
   *
   * カラーバッファとデプスバッファのアドレスを指定して、初期化するタイプのレンダーターゲットです。
   *
   *	カラーバッファあり。
   *	デプスバッファあり。
   *
   *  @param  p_desc                データ設定先
   *	@param	width                 幅
   *	@param	height                高さ
   *	@param	color_format          カラーフォーマット
   *	@param	depth_format          デプスフォーマット
   *	@param	color_address         カラーバッファアドレス
   *	@param	depth_address         デプスバッファアドレス
   *	@param	display_type          ディスプレイタイプ
   *	@param	fob_id                確保済みGL用フレームバッファオブジェクト（ない場合には0をいれてください）
   */
  //-----------------------------------------------------------------------------
  static void InitializeDirectAddressDescription( Description* p_desc, s32 width, s32 height, gfl::grp::RenderColorFormat color_format, gfl::grp::RenderDepthFormat depth_format, u32 color_address, u32 depth_address, gfl::grp::DisplayType display_type, GLuint fob_id );


  //----------------------------------------------------------------------------
  /**
   *	@brief  Descriptionにシャドウテクスチャに関する部分の設定を行う。
   *	        ＊必ず、InitializeOffScreen～Description関数後によんでください。
   *
   *	@param	p_desc                データ設定先
   *	@param  shadow_kind           シャドウの種別
   *	@param  is_perspective_shadow 投影シャドウの場合はtrue
   *	@param  shadow_z_bias         シャドウテクスチャのZバイアス値
   *	@param  shadow_z_scale        オフセット値に乗算するスケール
   */
  //-----------------------------------------------------------------------------
  static void SetShadowTextureDescription( Description* p_desc, gfl::grp::ShadowType shadow_kind, b32 is_perspective_shadow, f32 shadow_z_bias, f32 shadow_z_scale );



public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createは呼ばれません。
   */
  //-----------------------------------------------------------------------------
  RenderTarget(void);
 
  //-----------------------------------------------------------------------------
  /**
   *  @brief         コンストラクタ
   *                 Createを呼びます。
   */
  //-----------------------------------------------------------------------------
  RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );
  RenderTarget( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL);
  // @note gfl::grp::g3d::Textureを渡す関数をつくったら廃止予定
  RenderTarget( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description = NULL );
  // @note allocator, depth_buffer, description を引数に持つ関数作成予定
  // @note allocator, nw_res_texture, depth_buffer, description を引数に持つ関数作成予定

  //-----------------------------------------------------------------------------
  /**
   *  @brief         デストラクタ
   *                 Destroyを呼びます。
   */
  //-----------------------------------------------------------------------------
  virtual ~RenderTarget();

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成
   *  OffScreenBufferは、内部でテクスチャ領域を確保する。
   *  OnScreenBufferは、CreateOnScreenBufferと同等の結果になる。
   */
  //-----------------------------------------------------------------------------
  void Create( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, const Description* description = NULL );

  //-----------------------------------------------------------------------------
  /**
   *  @brief         オンスクリーンバッファの生成
   */
  //-----------------------------------------------------------------------------
  void CreateOnScreenBuffer( gfl::heap::NwAllocator* device_allocator, const Description* description = NULL);
  //-----------------------------------------------------------------------------
  /**
   *  @brief         オフスクリーンバッファの生成
   */
  //-----------------------------------------------------------------------------
  // @note gfl::grp::g3d::Textureを渡す関数をつくったら廃止予定
  void CreateOffScreenBuffer( gfl::heap::NwAllocator* heap_allocator, gfl::heap::NwAllocator* device_allocator, nw::gfx::res::ResTexture nw_res_texture, const Description* description = NULL );
  // @note allocator, depth_buffer, description を引数に持つ関数作成予定
  // @note allocator, nw_res_texture, depth_buffer, description を引数に持つ関数作成予定

  //-----------------------------------------------------------------------------
  /**
   *  @brief         破棄
   *                 何回呼んでも大丈夫。デストラクタは呼ばれません。 
   */
  //-----------------------------------------------------------------------------
  void Destroy(void);

  //-----------------------------------------------------------------------------
  /**
   *  @brief         生成済みか否か
   *
   *  @retval        b32  生成済みのときtrue、生成されていないときfalse
   */
  //-----------------------------------------------------------------------------
  b32 IsCreated(void) const  {  return (m_buffer_type != TYPE_NONE);  } 


public:
  // @note gfl::grp::g3d::Textureを返す関数をつくったら廃止予定
  nw::gfx::res::ResTexture  GetNwResTexture(void) const;  // バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。

public:
  // nw::gfx::FrameBufferObject相当の機能
  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレームバッファの設定用コマンドを設定する
   *
   *  @param  graphics      グラフィックス
   */
  //-----------------------------------------------------------------------------
  void ActivateBuffer( void ) const;

  //-----------------------------------------------------------------------------
  /**
   *  @brief         フレームバッファのクリアを実行する
   *                 クリアコマンドを積みたいコマンドリストをバインドしてから使用して下さい。
   *                 GraphicsSystemにあるクリア関数を使用することを推奨します。
   */
  //-----------------------------------------------------------------------------
  void ClearBuffer(
      b32 b_color,
      const gfl::grp::ColorF32& clear_color,
      b32 b_depth,
      f32 clear_depth,
      b32 b_stencil = false,
      u8 clear_stencil = 0 );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファの情報をメモリに出力
   *
   *  @param  graphics  グラフィックス
   *	@param	type      バッファタイプ
   *	@param  memory    出力先アドレス
   *
   *	*内部でカレントバッファを変更します。
   *  バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
   */
  //-----------------------------------------------------------------------------
  void OutputImage( RenderBufferType type, void* memory );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファの情報をメモリにリニア出力
   *
   *	@param	type      バッファタイプ
   *	@param  memory    出力先アドレス
   */
  //-----------------------------------------------------------------------------
  void OutputB2LImage( RenderBufferType type, void* memory );


  //----------------------------------------------------------------------------
  /**
   *	@brief  カラーバッファに情報をコピー（ブロックモード）
   *
   *	@param  memory  入力アドレス
   */
  //-----------------------------------------------------------------------------
  void InputImage( const void* memory );

#if GFL_HIO_ENABLE
	//----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファの情報をファイルに出力
   *
   *  @param  graphics  グラフィックス
	 *	@param	file_path    出力パス
   *	@param	type		バッファタイプ
   *	@param  allocator  ワークの確保先
   *
   *	*内部でカレントバッファを変更します。
   *  バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
   */
  //-----------------------------------------------------------------------------
  void OutputImageFile( const char* file_path, RenderBufferType type, gfl::heap::NwAllocator* allocator );
#endif

  //-----------------------------------------------------------------------------
  /**
   *  @brief         glのフレームバッファオブジェクトのIDを取得する
   */
  //-----------------------------------------------------------------------------
  GLuint GetFboID(void) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief        カラーバッファのアドレスを取得
   *	@return       カラーバッファのアドレス
   */
  //-----------------------------------------------------------------------------
  u32 GetColorAddress( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        デプスバッファのアドレスを取得
   *	@return       デプスバッファのアドレス
   */
  //-----------------------------------------------------------------------------
  u32 GetDepthAddress( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        幅の取得
   *	@return       幅
   */
  //-----------------------------------------------------------------------------
  s32 GetWidth( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief        高さの取得
   *	@return       高さ
   */
  //-----------------------------------------------------------------------------
  s32 GetHeight( void ) const;


  //----------------------------------------------------------------------------
  /**
   *	@brief      ディスプレイタイプの取得
   *
   *	@return     ディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::DisplayType GetDisplayType(void) const { return m_display_type; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      ディスプレイタイプの取得
   *
   *	@param   display_type  ディスプレイタイプ
   */
  //-----------------------------------------------------------------------------
  inline void SetDisplayType(const gfl::grp::DisplayType display_type) { m_display_type = display_type; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      デプスバッファフォーマットの取得
   *
   *	@return     デプスバッファフォーマット
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::RenderDepthFormat GetRenderDepthFormat(void) const { return m_depth_format; }

  //----------------------------------------------------------------------------
  /**
   *	@brief      カラーバッファフォーマットの取得
   *
   *	@return     カラーバッファフォーマット
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::RenderColorFormat GetRenderColorFormat(void) const { return m_color_format; }


  //----------------------------------------------------------------------------
  /**
   *	@brief      ビューポート情報の取得
   *
   *  @param[out]  left      スクリーン　左座標
   *  @param[out]  top       スクリーン　上座標
   *  @param[out]  right     スクリーン　右座標
   *  @param[out]  bottom    スクリーン　下座標
   */
  //-----------------------------------------------------------------------------
  void GetViewport( f32* left, f32* top, f32* right, f32* bottom ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief      ビューポート情報の設定
   *
   *  @param  left      スクリーン　左座標
   *  @param  top       スクリーン　上座標
   *  @param  right     スクリーン　右座標
   *  @param  bottom    スクリーン　下座標
   */
  //-----------------------------------------------------------------------------
  void SetViewport( f32 left, f32 top, f32 right, f32 bottom );
  
  //-----------------------------------------------------------------------------
  /**
   * デプスバッファのメモリ配置場所取得
   * @return デプスバッファのメモリ配置場所内容
   */
  //-----------------------------------------------------------------------------
  inline gfl::grp::MemoryArea GetDepthMemoryArea(){ return m_depth_area; }
  
  //-----------------------------------------------------------------------------
  /**
   * ピクセルフォーマット変換関数
   * @param p_data      変換したいデータ
   * @param format      入力フォーマット
   * @param w           幅 
   * @param h           高さ
   */
  //-----------------------------------------------------------------------------
  static void ConvertPixelFormat( char* p_data, GLenum format, int w, int h );


private:
  // friendクラスにだけアクセスを許すメンバ関数
  nw::gfx::IRenderTarget*  GetNwRenderTarget(void) const  { return m_nw_render_target; }
  const nw::gfx::Viewport& GetNwViewport(void) const  { return m_view_port.GetNwViewport(); };  // バッファをこのクラスにつくらせた場合は縦横が入れ替わった絵が得られます。
  
private:
  // このクラスからしかアクセスしないメンバ関数
	
  void SetDescriptionViewport( const Description* description, Viewport* viewport );

private:
  // @note depth_buffer_type も必要か？

  
  enum TextureOwnerType
  {
    TEXTURE_OWNER_NONE = 0,     // テクスチャ情報なし
    TEXTURE_OWNER_THIS,         // クラス内で生成したテクスチャを使用
    TEXTURE_OWNER_USER,         // ユーザーが生成したテクスチャを使用
  } ;

private:

  Type                          m_buffer_type;        // バッファタイプ
  DisplayType                   m_display_type;       // 描画面タイプ
  TextureOwnerType              m_texture_owner;      // テクスチャOwnerタイプ
  grp::RenderDepthFormat        m_depth_format;       // デプスフォーマット
  grp::RenderColorFormat        m_color_format;       // カラーフォーマット
  gfl::grp::MemoryArea          m_depth_area;         // デプスバッファのメモリ配置場所

  nw::gfx::IRenderTarget*   m_nw_render_target;  // NULLのときCreateされていない

  // m_buffer_typeがBUFFER_TYPE_OFF_SCREEN_BUFFRE_OWN_TEXTURE or BUFFER_TYPE_OFF_SCREEN_BUFFRE_USER_TEXTURE のとき有効
  nw::gfx::res::ResTexture* m_nw_res_texture;


  Viewport  m_view_port;  // ビューポート情報 

public:
  // @note 移行期にだけ使用を許可する関数(すぐに廃止予定)
  void SetNwRenderTarget(nw::gfx::IRenderTarget* nw_render_target)  { m_nw_render_target = nw_render_target; }
};

}  // namespace grp
}  // namespace gfl

#endif // __GFL_RENDERTARGET_H__
