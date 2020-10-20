#ifndef __GFL2_AREAMAN_H__
#define __GFL2_AREAMAN_H__
//===========================================================================
/**
 * @file    gfl2_AreaMan.h
 * @brief   GAMEFREAK Library  String Print System ( 領域管理クラス )
 * @author  GAMEFREAK inc.
 * @date    2010.11.29
 */
//===========================================================================
#pragma once

#include  <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4245)  //C4200を抑止


namespace gfl2 {
namespace util {

#define _GFL_BASE_AREAMAN_DEBUG_ENABLE  (0)


/**
 *  領域管理クラス
 */
class AreaMan {
  GFL_FORBID_COPY_AND_ASSIGN( AreaMan );

public:
  enum {
    RESULT_FAILED = 0xffffffffffffffff, ///< 確保失敗時の結果コード
    POS_NOT_FOUND = 0xffffffff,         ///< 領域が確保できなかった場合の戻り値
  };

  /**
   *  領域確保結果コード
   */
  typedef  u64  ResultCode;

  //--------------------------------------------------------
  /**
   *  コンストラクタ
   *
   * @param[in]   numBlocks   管理するブロック数
   * @param[in]   pHeap       ヒープハンドラ
   */
  //--------------------------------------------------------
  AreaMan( u32 numBlocks, gfl2::heap::HeapBase* pHeap );

  // デストラクタ
  ~AreaMan();

  //------------------------------------------------------------------
  /**
   * 領域先頭から末尾まで、空いている所を探して確保（前方から検索）
   *
   * @param[in]   numBlocks   確保するブロック数
   *
   * @retval      AreaMan::ResultCode   結果コード／確保できなければ RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAuto( u32 numBlocks );

  //------------------------------------------------------------------
  /**
   * 領域先頭から末尾まで、空いている所を探して確保（後方から検索）
   *
   * @param[in]   numBlocks   確保するブロック数
   *
   * @retval      AreaMan::ResultCode   結果コード／確保できなければ RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAutoLo( u32 numBlocks );

  //------------------------------------------------------------------
  /**
   * 領域指定範囲内から、空いている位置を探して確保（前方から検索）
   *
   * @param[in]   numBlockReserve   確保するブロック数
   * @param[in]   startBlockPos     検索開始ブロック位置（最前=0, 最後=(最大ブロック数-1)）
   * @param[in]   numAreaBlocks     検索範囲ブロック数
   *
   * @retval      AreaMan::ResultCode   結果コード／確保できなければ RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveArea( u32 numBlocks, u32 startBlockPos, u32 numAreaBlocks );

  //------------------------------------------------------------------
  /**
   * 領域指定範囲内から、空いている位置を探して確保（後方から検索）
   *
   * @param[in]   numBlockReserve   確保するブロック数
   * @param[in]   startBlockPos     検索開始ブロック位置（最前=0, 最後=(最大ブロック数-1) ※ReserveArea()と同じ。0=最後ではありません）
   * @param[in]   numAreaBlocks     検索範囲ブロック数
   *
   * @retval      AreaMan::ResultCode   結果コード／確保できなければ RESULT_FAILED
   */
  //------------------------------------------------------------------
  ResultCode  ReserveAreaLo( u32 numBlocks, u32 startBlockPos, u32 numAreaBlocks );


  //------------------------------------------------------------------
  /**
   * 予約した領域を開放
   *
   * @param[in]   resultCode    確保時の結果コード
   */
  //------------------------------------------------------------------
  void Release( ResultCode  reserveResultCode );

  //------------------------------------------------------------------
  /**
   * 連続した空き領域の最大値を返す
   *
   * @retval    u32   連続空きブロック数
   */
  //------------------------------------------------------------------
  u32  GetContiniusFreeBlockNum( void ) const;

  //------------------------------------------------------------------
  /**
   * 連続した空き領域の最大値を返す（カウント開始位置を指定）
   *
   * @param[in]   startBlockPos     検索開始ブロック位置（最前=0, 最後=(最大ブロック数-1) ※ReserveArea()と同じ。0=最後ではありません）
   * @param[in]   numAreaBlocks     検索範囲ブロック数
   *
   * @retval    u32   連続空きブロック数
   */
  //------------------------------------------------------------------
  u32 GetContiniusFreeBlockNum( u32 startBlock, u32 numBlockArea ) const;

  //------------------------------------------------------------------
  /**
   * アラインメント設定（ブロック単位）
   *
   * @param[in]   alignment      アラインメント指定
   */
  //------------------------------------------------------------------
  void SetAlign( u32 alignment )
  {
    m_alignment = alignment;
  }

  //-----------------------------------------------------------
  /**
   *  結果コード -> 確保位置へ変換
   */
  //-----------------------------------------------------------
  u32  ResCodeToBlockPos( ResultCode resCode ) const
  {
    if( resCode != POS_NOT_FOUND ){
      resCode >>= 32;
      return static_cast<u32>( resCode & 0xffffffff );
    }
    return POS_NOT_FOUND;
  }


private:
  //----------------------------------------------
  // Bit計算省略用テーブル
  //-----------------------------------------------
  static const u8 EmptyCount[256];
  static const u8 OpenFwdCount[256];
  static const u8 OpenBackCount[256];
  static const u8 FwdFillBit[9];
  static const u8 BackFillBit[9];

  u32   m_numBlocks;
  u32   m_areaByteSize;
  u32   m_alignment;
  u8*   m_pArea;
  bool  m_bPrintEnable;

  u32 numBlocksToByteSize( u32 numBlocks ) const;

  ResultCode reserveHi( u32 startPos, u32 numBlockArea, u32 numBlockReserve );
  ResultCode reserveLo( u32 startPos, u32 numBlockArea, u32 numBlockReserve );
  bool       checkStartBitEnough( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const;
  bool       checkEndBitEnough( u32 bytePos, u32 endBitWidth ) const;
  bool       checkStartBitEnoughLo( u32 bytePos, u32 startBitIdx, u32 startBitWidth ) const;
  bool       checkEndBitEnoughLo( u32 bytePos, u32 endBitWidth ) const;
  void       release_bit( u8* area, u32 pos, u32 num );
  bool       reserve_area( int pos, u32 blockNum );

  /**
   * ブロック位置＆サイズ->結果コード作成
   */
  ResultCode  combineResCode( u32 pos, u32 size ) const
  {
    u64 result = pos;
    result <<= 32;
    result |= size;
    return result;
  }

  /**
   * 結果コード作成 -> ブロック位置＆サイズへ分割
   */
  void devideResCode( ResultCode code, u32* pos, u32* size ) const
  {
    *size = code & 0xffffffff;
    *pos  = (code >> 32) & 0xffffffff;
  }

  #if _GFL_BASE_AREAMAN_DEBUG_ENABLE
  void print_bit( u8 b ) const;
  #endif

  //----------------------------------------------
  // Debug Print Flag
  //-----------------------------------------------
  enum {
    PRINT_FLAG = (gfl2::debug::PRINT_FLAG_CATEGORY_HEAP) | (gfl2::debug::PRINT_FLAG_LEVEL2)
  };

};


} // end of namespace 'util'
} // end of namespace 'gfl'

GFL_WARNING_WIN32_POP

#endif // __GFL2_AREAMAN_H__
