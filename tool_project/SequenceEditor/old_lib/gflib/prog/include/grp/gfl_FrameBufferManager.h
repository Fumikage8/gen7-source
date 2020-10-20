#ifndef __GFL_FRAMEBUFFERMANAGER_H__
#define __GFL_FRAMEBUFFERMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FrameBufferManager.h
 *	@brief  フレームバッファマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>

#include <grp/gfl_GraphicsType.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>

#include <grp/gfl_RenderTarget.h>

namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
/// フレームバッファーID
//=====================================
typedef u32 FrameBufferID;
const FrameBufferID FRAMEBUFFER_ID_ERROR = 0xffffffff;


//-------------------------------------
///	レンダリングバッファ　セットアップ情報
//=====================================
typedef struct{

  DisplayType           targetDisplay;  // どの面を対象にしているのか。
  s32                   width;          // バッファの横幅です。
  s32                   height;         // バッファの縦幅です。
  RenderColorFormat     colorFormat;    // カラーバッファフォーマット
  RenderDepthFormat     depthFormat;    // 深度バッファフォーマット

  MemoryArea            colorArea;      // 描画対象のカラーバッファの配置場所です。
  MemoryArea            depthArea;      // 描画対象のデプスバッファの配置場所です。
  DisplayType           shareBuffer;    // レンダーターゲットをshareする。
  f32                   left;           // Viewport左です。
  f32                   top;            // Viewport上です。
  f32                   right;          // Viewport右です。
  f32                   bottom;         // Viewport下です。

} RenderBufferSetUp;

//-------------------------------------
///	フレームバッファクリアカラー
//=====================================
typedef struct {
  f32   red;      // 0.0～1.0
  f32   green;    // 0.0～1.0
  f32   blue;     // 0.0～1.0
  f32   alpha;    // 0.0～1.0
  f32   depth;    // 0.0～1.0
  f32   stencil;  // 0.0～1.0
} FrameBufferClearColor;

//-------------------------------------
///	フレームバッファクリアフラグ
//=====================================
struct FrameBufferClearFlag{
  b32   f_color;
  b32   f_depth;
  b32   f_stencil;
  
  FrameBufferClearFlag():
    f_color( true ),
    f_depth( true ),
    f_stencil( true )
  {
    
  }
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
/// フレームバッファーマネージャクラス
//=====================================
class FrameBufferManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(FrameBufferManager);

public:


  //-----------------------------------------------------------------------------
  /*
   * コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  FrameBufferManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~FrameBufferManager();


  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファ　生成
   *
   *	@param    setup	　セットアップ情報
   *
   *	@return フレームバッファID
   */
  //-----------------------------------------------------------------------------
  FrameBufferID CreateBuffer( heap::NwAllocator* allocator, const RenderBufferSetUp& setup );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファ　破棄
   *
   *	@param	id    フレームバッファID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( FrameBufferID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファをカレントに設定
   *
   *	@param	  id  フレームバッファＩＤ
   */
  //-----------------------------------------------------------------------------
  void Bind( FrameBufferID id );

  //----------------------------------------------------------------------------
  /**
   *	@brief  バインドしてからフレームバッファをクリアする
   *
   *	@param	  id  フレームバッファＩＤ
   *	@param    clearColoer クリアカラー設定
   */
  //-----------------------------------------------------------------------------
  void BindAndClear( FrameBufferID id, const FrameBufferClearColor& clearColor, const FrameBufferClearFlag& clearFlag );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファの情報をメモリにコピー
   *
   *	@param	id      フレームバッファID
   *	@param  type    バッファタイプ
   *	@param  memory  コピー先アドレス
   *
   *	*内部でカレントバッファを変更します。
   */
  //-----------------------------------------------------------------------------
  void OutputImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory );

  //----------------------------------------------------------------------------
  /**
   *  @brief  メモリからフレームバッファへ情報をコピー
   *
   *  @param  id      フレームバッファID
   *  @param  memory  コピー元アドレス
   *
   *  *内部でカレントバッファを変更します。
   */
  //-----------------------------------------------------------------------------
  void InputImage( FrameBufferID id, void* memory );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファの情報をメモリにコピー (Linerフォーマット）
   *
   *	@param	id      フレームバッファID
   *	@param  type    バッファタイプ
   *	@param  memory  コピー先アドレス
   *
   *	*内部でカレントバッファを変更します。
   */
  //-----------------------------------------------------------------------------
  void OutputB2LImage( FrameBufferID id, RenderTarget::RenderBufferType type, void* memory );


  //----------------------------------------------------------------------------
  /**
   *	@brief  レンダーターゲットの取得
   *
   *	@param	id      フレームバッファID
   *
   *	@return IDに対応したレンダーターゲット取得
   */
  //-----------------------------------------------------------------------------
  gfl::grp::RenderTarget* GetRenderTarget( FrameBufferID id ) const;

private:
  RenderTarget*   m_array;
  u16             m_arrayMax;

private:
  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize(void);
  FrameBufferID GetClearBuffer( void ) const;
  FrameBufferID GetDisplayTypeBuffer( DisplayType displayType ) const;

};



} /* namespace end grp */
} /* namespace end gfl */
#endif // __GFL_FRAMEBUFFERMANAGER_H__
