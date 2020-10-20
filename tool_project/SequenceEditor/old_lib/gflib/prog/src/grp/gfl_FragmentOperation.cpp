//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_FragmentOperation.cpp
 *	@brief  フラグメントオペレーション　管理
 *	@author	tomoya takahashi
 *	@date		2010.10.27
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <grp/gfl_FragmentOperation.h>

#include <nw/font/font_RectDrawerCommand.h>

#include <debug/gfl_DebugPrint.h>
#include <debug/gfl_Assert.h>

#include <gfl_Std.h>

namespace gfl {
namespace grp {



//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *  2D描画環境基本設定
 */
//-----------------------------------------------------------------------------
const FragmentOperationParam FragmentOperation::DefaultParam = 
{
  CULL_NONE,             // カリングモード（ENV_CULL～）
  false,                        // シザーテスト（true/false）
  {0},
  false,                        // アーリーデプステスト(true/false)
  false,                        // ステンシルテスト(true/false)
  {0},                          // 
  0.0f,                         // Wバッファ設定（0.0fで無効）
  0.0f,                         // デプスレンジ Near
  1.0f,                         // デプスレンジ Far
  0,                            // デプス値 分解最小値に対するスケール値
  false,                        // デプステスト(true/false)
  0,                            // デプステスト比較関数(ENV_TEST_FUNC_～)
  true,                         // デプスバッファへの書き込みが可能か(true/false)
  true,                         // カラーバッファへ赤を書き込むか（true/false）
  true,                         // カラーバッファへ赤を書き込むか（true/false）
  true,                         // カラーバッファへ赤を書き込むか（true/false）
  true,                         // カラーバッファへ赤を書き込むか（true/false）
  true,                         // カラーバッファの読み込みが可能か（true/false）
  true,                         // カラーバッファの書き込みが可能か（true/false）
  false,                        // デプスバッファの読み込みが可能か（true/false）
  false,                        // デプスバッファの書き込みが可能か（true/false）
  false,                        // ステンシルバッファの読み込みが可能か（true/false）
  false,                        // ステンシルバッファの書き込みが可能か（true/false）
};

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  コンストラクタ  デフォルト設定
 */
//-----------------------------------------------------------------------------
FragmentOperation::FragmentOperation(void)
{
  std::MemCopy( &DefaultParam, &m_setup, sizeof(FragmentOperationParam) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  デストラクタ
 */
//-----------------------------------------------------------------------------
FragmentOperation::~FragmentOperation()
{
}

//----------------------------------------------------------------------------
/**
 *	@brief  パラメータセットアップ
 *
 *	@param	param
 */
//-----------------------------------------------------------------------------
void FragmentOperation::SetParam( const FragmentOperationParam& param )
{
  gfl::std::MemCopy( &param, &m_setup, sizeof(FragmentOperationParam) );
}


//----------------------------------------------------------------------------
/**
 *	@brief  現在のパラメータを取得
 *
 *	@return 現在の設定
 */
//-----------------------------------------------------------------------------
const FragmentOperationParam& FragmentOperation::GetParam(void) const
{
  return m_setup;
}

//----------------------------------------------------------------------------
/**
 *	@brief  3Dコマンドをカレントのコマンドリストに設定
 *
 *	@param	フレームバッファクラス
 */
//-----------------------------------------------------------------------------
void FragmentOperation::Add3DCommand( const RenderTarget& renderTarget ) const
{
  f32 left, right, top, bottom;
  renderTarget.GetViewport( &left, &top, &right, &bottom );

  Add3DCommand( left, top, 
      right - left, bottom - top, 
      renderTarget.GetRenderDepthFormat() );
}

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
void FragmentOperation::Add3DCommand( u32 vp_x, u32 vp_y, u32 vp_width, u32 vp_height, RenderDepthFormat depth_format ) const
{
  // PICA DEPTH FORMATに変換
  u32 pica_depth_format;
  switch( depth_format ){
  case RENDER_DEPTH_FORMAT_16:
    pica_depth_format = PICA_DATA_DEPTH_COMPONENT16;
    break;

  case RENDER_DEPTH_FORMAT_24:
    pica_depth_format = PICA_DATA_DEPTH_COMPONENT24_OES;
    break;

  case RENDER_DEPTH_FORMAT_24_STENCIL8:
    pica_depth_format = PICA_DATA_DEPTH24_STENCIL8_EXT;
    break;

  default:
    GFL_ASSERT(0);
    pica_depth_format = PICA_DATA_DEPTH24_STENCIL8_EXT; //Coverity[12753] 未初期化
    break;
  }

  nw::font::ColorBufferInfo colbuf_info = { vp_height, vp_width, pica_depth_format };
  
  // カリングモード
  {
    const u32 local_cmd[] = {
      PICA_CMD_SET_CULL_FACE( m_setup.cullmode ),
      NW_FONT_CMD_SET_VIEWPORT(vp_x, vp_y, colbuf_info.width, colbuf_info.height)
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
  

  
  // シザーテスト
  if( m_setup.scissor ){
    const u32 local_cmd[] = {
      PICA_CMD_DATA_SCISSOR( true ),
      PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 ),
      (m_setup.scissor_rect.x) | (m_setup.scissor_rect.y<<16),
      (m_setup.scissor_rect.x+m_setup.scissor_rect.width-1) | ((m_setup.scissor_rect.y+m_setup.scissor_rect.height-1)<<16),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }else{
    const u32 local_cmd[] = {
      PICA_CMD_DATA_SCISSOR( false ),
      PICA_CMD_HEADER_BURSTSEQ( PICA_REG_SCISSOR, 3 ),
      0,
      colbuf_info.height | (colbuf_info.width<<16),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // Wバッファ、デプスレンジ
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_WBUFFER_DEPTHRANGE_POLYGONOFFSET( 
          m_setup.wscale, m_setup.depth_range_near,
          m_setup.depth_range_far, m_setup.polygon_offset,
          colbuf_info ),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }


  // アーリーデプステスト 
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_ENABLE_EARLY_DEPTH_TEST( m_setup.arly_depth_test ),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // ステンシルテスト
  if( m_setup.stencil_test == false ){
    // OFF
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_DISABLE_STENCIL_TEST(),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }else{
    // ON
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_STENCIL_FUNC_MASK( m_setup.stencil_para.func_type, m_setup.stencil_para.func_ref, m_setup.stencil_para.func_msk, m_setup.stencil_para.write_msk ),
      NW_FONT_CMD_SET_STENCIL_OP( m_setup.stencil_para.op_fail, m_setup.stencil_para.op_zfail, m_setup.stencil_para.op_zpass ),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }

  // デプステスト・カラーマスク
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_DEPTH_FUNC_COLOR_MASK( 
          m_setup.depth_test, m_setup.depth_func,
          m_setup.depth_write, m_setup.colormsk_red,
          m_setup.colormsk_green, m_setup.colormsk_blue,
          m_setup.colormsk_alpha ),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
  
  // フレームバッファアクセス
  {
    const u32 local_cmd[] = {
      NW_FONT_CMD_SET_FBACCESS( 
          m_setup.frame_color_read, m_setup.frame_color_write,
          m_setup.frame_depth_read, m_setup.frame_depth_write,
          m_setup.frame_stencil_read, m_setup.frame_stencil_write ),
    };
    //コマンドリストの情報をバッファに格納
    nngxAdd3DCommand(local_cmd, sizeof(local_cmd), true);
    GFL_GRPGL_ERROR_CHECK();
  }
}




} /* namespace end grp */
} /* namespace end gfl */
