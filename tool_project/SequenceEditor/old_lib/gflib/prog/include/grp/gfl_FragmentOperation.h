#ifndef __GFL_FRAGMENTOPERATION_H__
#define __GFL_FRAGMENTOPERATION_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FragmentOperation.h
 *	@brief  フラグメントオペレーション　管理
 *	@author	tomoya takahashi
 *	@date		2010.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>
#include <GLES2/gl2.h>

#include <gfl_Math.h>
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
enum FragmentOperationEnum{
  // カリングモード
  CULL_NONE = 0,
  CULL_FRONT,
  CULL_BACK,
  CULL_MAX,  // システムが使用

  // ステンシルテスト・デプステスト::比較関数
  TEST_FUNC_NEVER    = PICA_DATA_DEPTH_TEST_NEVER,   // つねにフェイル
  TEST_FUNC_ALWAYS   = PICA_DATA_DEPTH_TEST_ALWAYS,  // つねにパス
  TEST_FUNC_LESS     = PICA_DATA_DEPTH_TEST_LESS,    // (ret & mask) < (stencil & mask)のときパス
  TEST_FUNC_LEQUAL   = PICA_DATA_DEPTH_TEST_LEQUAL,  // (ret & mask) <= (stencil & mask)のときパス
  TEST_FUNC_EQUAL    = PICA_DATA_DEPTH_TEST_EQUAL,   // (ret & mask) == (stencil & mask)のときパス
  TEST_FUNC_GREATER  = PICA_DATA_DEPTH_TEST_GREATER, // (ret & mask) > (stencil & mask)のときパス
  TEST_FUNC_GEQUAL   = PICA_DATA_DEPTH_TEST_GEQUAL,  // (ret & mask) >= (stencil & mask)のときパス
  TEST_FUNC_NOTEQUAL = PICA_DATA_DEPTH_TEST_NOTEQUAL,// (ret & mask) != (stencil & mask)のときパス

  // ステンシルテスト::操作設定
  STNCL_TEST_OP_KEEP       = PICA_DATA_STENCIL_OP_KEEP,    // ステンシル値は現在の値を保持
  STNCL_TEST_OP_ZERO       = PICA_DATA_STENCIL_OP_ZERO,    // ステンシル値は０に置き換わる
  STNCL_TEST_OP_REPLACE    = PICA_DATA_STENCIL_OP_REPLACE, // ステンシル値は参照値に置き換わる
  STNCL_TEST_OP_INCR       = PICA_DATA_STENCIL_OP_INCR,    // ステンシル値は1加算される　255を超える事はない
  STNCL_TEST_OP_DECR       = PICA_DATA_STENCIL_OP_DECR,    // ステンシル値は1減算される  0未満になることはない
  STNCL_TEST_OP_INVERT     = PICA_DATA_STENCIL_OP_INVERT,  // ステンシル値はビット反転する
  STNCL_TEST_OP_INCR_WRAP  = PICA_DATA_STENCIL_OP_INCR_WRAP,// ステンシル値は1加算される 255の次は0になる
  STNCL_TEST_OP_DECR_WRAP  = PICA_DATA_STENCIL_OP_DECR_WRAP,// ステンシル値は1減算される 0の次は255になる


  ENUM_FORCE_DWORD(FragmentOperationEnum)
};

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	シザーテスト　矩形情報
//=====================================
typedef struct {
  u16 x;
  u16 y;
  u16 width;
  u16 height;
} ScissorRect;

//-------------------------------------
///	ステンシルテスト　パラメータ
//=====================================
typedef struct {
  u32 func_type;        // 比較タイプ (TEST_FUNC_～)
  s32 func_ref;         // 比較参照値
  u32 func_msk;         // 比較マスク値
  u32 write_msk;        // ステンシルバッファへの書き込み有効・無効マスク
  
  u32 op_fail;          // ステンシルテストの結果フェイルした場合の操作(STNCL_TEST_OP_～)
  u32 op_zfail;         // ステンシルテストがパス、デプステストがフェイルした場合の操作(STNCL_TEST_OP_～)
  u32 op_zpass;         // ステンシルテストおよびデプステストがパスした場合の操作(STNCL_TEST_OP_～)
} StnclTestParam;


//-------------------------------------
///	フラグメントオペレーション
//=====================================
typedef struct {
  u16  cullmode;                        // カリングモード（CULL～）
  u16  scissor;                         // シザーテスト（true/false）
  ScissorRect scissor_rect;
  u16  arly_depth_test;                 // アーリーデプステスト(true/false)
  u16  stencil_test;                    // ステンシルテスト(true/false)
  StnclTestParam stencil_para;  // 
  f32  wscale;                          // Wバッファ設定（0.0fで無効）
  f32  depth_range_near;                // デプスレンジ Near
  f32  depth_range_far;                 // デプスレンジ Far
  f32  polygon_offset;                  // デプス値 分解最小値に対するスケール値
  u16  depth_test;                      // デプステスト(true/false)
  u16  depth_func;                      // デプステスト比較関数(TEST_FUNC_～)
  u8  depth_write;                      // デプスバッファへの書き込みが可能か(true/false)
  u8  colormsk_red;                     // カラーバッファへ赤を書き込むか（true/false）
  u8  colormsk_green;                   // カラーバッファへ赤を書き込むか（true/false）
  u8  colormsk_blue;                    // カラーバッファへ赤を書き込むか（true/false）
  u8  colormsk_alpha;                   // カラーバッファへ赤を書き込むか（true/false）
  u8  frame_color_read;                 // カラーバッファの読み込みが可能か（true/false）
  u8  frame_color_write;                // カラーバッファの書き込みが可能か（true/false）
  u8  frame_depth_read;                 // デプスバッファの読み込みが可能か（true/false）
  u8  frame_depth_write;                // デプスバッファの書き込みが可能か（true/false）
  u8  frame_stencil_read;               // ステンシルバッファの読み込みが可能か（true/false）
  u8  frame_stencil_write;              // ステンシルバッファの書き込みが可能か（true/false）

  
  u8  pad;
} FragmentOperationParam;

  
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class FragmentOperation
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(FragmentOperation);

public:
  //----------------------------------------------------------------------------
  /**
   *	@brief  環境デフォルト設定
   */
  //-----------------------------------------------------------------------------
  static const FragmentOperationParam DefaultParam;
  
public:
  //-----------------------------------------------------------------------------
  /**
   *      コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  FragmentOperation(void);
  ~FragmentOperation();

  //----------------------------------------------------------------------------
  /**
   *	@brief  パラメータセットアップ
   *
   *	@param	param
   */
  //-----------------------------------------------------------------------------
  void SetParam( const FragmentOperationParam& param );

  //----------------------------------------------------------------------------
  /**
   *	@brief  現在のパラメータを取得
   *
   *	@return 現在の設定
   */
  //-----------------------------------------------------------------------------
  const FragmentOperationParam& GetParam(void) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  3Dコマンドをカレントのコマンドリストに設定
   *
   *	@param	フレームバッファクラス
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( const RenderTarget& renderTarget ) const;
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  3Dコマンドをカレントのコマンドリストに設定
   *
   *  @param  vp_x            ビューポートX
   *  @param  vp_y            ビューポートY
   *	@param	vp_width        カラーバッファ　横幅
   *	@param	vp_height       カラーバッファ　縦幅
   *	@param  depth_format    デプスバッファ　フォーマット
   */
  //-----------------------------------------------------------------------------
  void Add3DCommand( u32 vp_x, u32 vp_y, u32 vp_width, u32 vp_height, RenderDepthFormat depth_format ) const;

private:

  FragmentOperationParam m_setup;
};




} /* namespace end grp */
} /* namespace end gfl */
#endif // __GFL_FRAGMENTOPERATION_H__
