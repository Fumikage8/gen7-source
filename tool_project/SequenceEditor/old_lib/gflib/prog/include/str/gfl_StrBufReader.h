#ifndef __GFL_STRBUFREADER_H__
#define __GFL_STRBUFREADER_H__
//===========================================================================
/**
 * @file  gfl_StrBufReader.h
 * @brief GAMEFREAK Library String System ( String Buffer Reader)
 * @author  GAMEFREAK inc.
 * @date  2010.10.28
 */
//===========================================================================
#pragma once

#include <gfl_Common.h>
#include <gfl_Heap.h>
#include <str/gfl_StrBuf.h>

namespace gfl {
namespace str {


//==============================================================================================
/**
 *  文字列バッファリーダークラス
 *
 *  指定された StrBuf の内容を１文字ずつ増やしながら読み出す機構
 */
//==============================================================================================
class StrBufReader
{
  // コピーコンストラクタ＆代入演算子の禁止
  GFL_FORBID_COPY_AND_ASSIGN( StrBufReader );

private:
  enum {
    TEMP_BUFFER_SIZE_MIN = 512   ///< 最低限、これだけの文字数を格納するだけのバッファを確保する
  };

public:
  //---  コンストラクタ／デストラクタ -----------
  StrBufReader( gfl::heap::HeapBase* pHeap );

  ~StrBufReader();


  //----------------------------------------------------------------------
  /**
   * @brief     コンストラクタによる正常なインスタンス構築が完了したか判定
   *
   * @retval    bool    正常に構築されていたら true
   */
  //----------------------------------------------------------------------
  bool   IsAvirable( void ) const { return m_bAvailable; }

  //----------------------------------------------------------------------
  /**
   * @brief   このメソッドを呼び出すごとに１文字ずつ内容が増えた StrBuf への参照が返る
   *
   * @retval  const StrBuf&
   */
  //----------------------------------------------------------------------
  const StrBuf& ReadNext( void );

  //----------------------------------------------------------------------
  /**
   * 次に読み出す文字コードを取得
   *
   * @retval  STRCODE
   */
  //----------------------------------------------------------------------
  STRCODE  GetNextCode( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief     最後まで読み出したか判定
   *
   * @retval    bool    最後まで呼んでいたら true
   */
  //----------------------------------------------------------------------
  bool  IsEnd( void ) const;

  //----------------------------------------------------------------------
  /**
   * @brief       読み出す文字列を切り替える
   *
   * @param[in]   srcBuf    読み出したい文字列
   * @param[in]   pHeap     内包するバッファが srcBuf を格納しきれない場合、
   *                        新たにバッファを構築し直すためのヒープ
   *
   */
  //----------------------------------------------------------------------
  void  SetString( const StrBuf& srcBuf, gfl::heap::HeapBase* pHeap );

  void  SetString( const STRCODE* str, u32 strLen, gfl::heap::HeapBase* pHeap );


  //----------------------------------------------------------------------
  /**
   * @brief     読み出しポインタをリセット（次に ReadNext() を呼ぶと、最初の１文字分が返る）
   */
  //----------------------------------------------------------------------
  void  Reset( void );

  //----------------------------------------------------------------------
  /**
   * @brief     最後に処理されたタグコードを取得
   *
   * @param[out]   dst      最後に処理されたタグコードを受け取る
   *
   * @retval       bool     dstに有効な値を設定した場合true, それ以外 false
   */
  //----------------------------------------------------------------------
  bool  GetLatestTagCode( STRCODE* dst ) const;


    u16           m_srcLength;
    u16           m_readPtr;

private:
    StrBuf*       m_buffer;
    u16           m_bufLength;
    STRCODE       m_replaceCode;
    STRCODE       m_latestTagCode;
    bool          m_tagProceeded;
    bool          m_bAvailable;

    u16           m_latestTagParamCount;
    u16           m_latestTagParam[ NUM_TAG_PARAM_MAX ];
};



} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_STRBUFREADER_H__
