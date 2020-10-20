#ifndef __GFL_DISPLAYBUFFERMANAGER_H__
#define __GFL_DISPLAYBUFFERMANAGER_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_DisplayBufferManager.h
 *	@brief  ディスプレイバッファマネージャ
 *	@author	tomoya takahashi
 *	@date		2010.10.21
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <nn.h>
#include <nw.h>

#include <grp/gfl_GraphicsType.h>
#include <grp/gfl_FrameBufferManager.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>


namespace gfl {
namespace grp {


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	バッファ転送モード
//=====================================
typedef enum 
{
  TRANSFER_MODE_ANTIALIASE_NOT_USED,  // アンチエイリアスは行いません。
  TRANSFER_MODE_ANTIALIASE_2x1,       // 2×1アンチエイリアスで転送します。
  TRANSFER_MODE_ANTIALIASE_2x2,       // 2×2アンチエイリアスで転送します。
  TRANSFER_MODE_ANTIALIASE_COUNT,      // 転送モードの数を表します。
    
  ENUM_FORCE_DWORD(DisplayTransferMode)
} DisplayTransferMode;
  
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	ディスプレイバッファ設定データ
//=====================================
typedef struct {
  DisplayType           displayType;    // ディスプレイの種類です。
  s32                   width;          // ディスプレイの横幅です。
  s32                   height;         // ディスプレイの縦幅です。
  DisplayTransferMode   transMode;      // ティスプレイバッファの転送モードです。
  MemoryArea            memArea;        // メモリ確保エリアです。
  RenderColorFormat     colorFormat;    // カラーフォーマットです
  bool                  isTransferFlipX;// 転送時にＸ方向にフリップするかどうかを表します。
  s32                   displayOffsetX; // 表示を行う始点Ｘ座標です。
  s32                   displayOffsetY; // 表示を行う始点Ｙ座標です。
} DisplayBufferSetUp;


//-------------------------------------
/// ディスプレイバッファーID
//=====================================
typedef u32 DisplayBufferID;
const DisplayBufferID DISPLAYBUFFER_ID_ERROR = 0xffffffff;
  
//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------


//-------------------------------------
/// ディスプレイバッファークラス
//=====================================
class DisplayBuffer;



//-------------------------------------
/// ディスプレイバッファーマネージャクラス
//=====================================
class DisplayBufferManager
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(DisplayBufferManager);

public:
  //-----------------------------------------------------------------------------
  /*
   * コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  DisplayBufferManager( heap::NwAllocator* allocator, u32 arrayMax );
  ~DisplayBufferManager();

  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ディスプレイバッファの確保
   *
   *	@param	setup   登録情報
   *
   *	@return ディスプレイバッファID
   */
  //-----------------------------------------------------------------------------
  DisplayBufferID CreateBuffer( const DisplayBufferSetUp& setup );
  
  //----------------------------------------------------------------------------
  /**
   *	@brief  ディスプレイバッファの破棄
   *  
   *	@param	displayBufferID  ディスプレイバッファID
   */
  //-----------------------------------------------------------------------------
  void DeleteBuffer( DisplayBufferID displayBufferID );


  //----------------------------------------------------------------------------
  /**
   *	@brief    ディスプレイバッファをアクティブ状態にする
   *
   *	@param	  displayBufferID ディスプレイバッファ
   */
  //-----------------------------------------------------------------------------
  void ActivateBuffer( DisplayBufferID displayBufferID );
  void ActivateBuffer( DisplayBufferID displayBufferID, gfl::grp::DisplayType display_type );


  //----------------------------------------------------------------------------
  /**
   *	@brief  フレームバッファのカラー情報を転送
   *
   *	@param  displayBufferID   ディスプレイバッファID  
   *	@param	renderTarget      カラーの転送元となるレンダーターゲット
   */
  //-----------------------------------------------------------------------------
  void TransferRenderImage( DisplayBufferID displayBufferID, const RenderTarget& renderTarget );


  //----------------------------------------------------------------------------
  /**
   *	@brief  セットアップ情報の取得
   *
   *	@param	displayBufferID   ディスプレイバッファＩＤ
   *
   *	@return セットアップ情報
   */
  //-----------------------------------------------------------------------------
  const DisplayBufferSetUp& GetSetupData( DisplayBufferID displayBufferID )const;
  

private:

  //メンバ変数
  DisplayBuffer*  m_array;
  u32             m_arrayMax;
  
private:
  // 隠蔽メソッド
  void Initialize( heap::NwAllocator* allocator, u32 arrayMax );
  void Finalize( void );
  DisplayBufferID GetClearBuffer( void ) const;

  

};






} /* end namespace grp */
} /* end namespace gfl */

#endif // __GFL_DISPLAYBUFFERMANAGER_H__
