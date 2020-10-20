//===========================================================================
/**
 * @file    NetAppLatinumStringTool.cpp
 * @brief   ラテン文字列用クラス（主に文字列比較）
 * @author  Ichibayashi / 移植 mori
 * @date    2010.10.28 / 2013.04.11
 *
 * src/netapp/gts/tool/フォルダからコピーしてきて処理追加しています。
 *
 */
//===========================================================================
#if !defined( NETAPP_LATINUM_STRING_TOOL_H_INCLUDE )
#define NETAPP_LATINUM_STRING_TOOL_H_INCLUDE
#pragma once

#include <str/include/gfl2_StrBuf.h>
#include <str/include/gfl2_StrSys.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class NetAppLatinumStringTool 
{

  public:
  // <localize> 文字列の一致判定（大文字小文字を同一視）
  //=============================================================================================
  /**
   * 文字列の一致判定（大文字・小文字・アクセントを同一視）
   *
   * @param   src
   * @param   dst
   *
   * @retval  BOOL  一致していたらTRUE
   */
  //=============================================================================================
  static bool CompareBufferNoCaseNoAccent( const gfl2::str::StrBuf* srcbuf, const gfl2::str::StrBuf* dstbuf );

  
  // <localize> 文字を大文字にする関数
  //=============================================================================================
  /**
   * 文字を大文字にする関数
   *
   * @param   c		文字
   *
   */
  //=============================================================================================
  static gfl2::str::STRCODE CodeToUpper( gfl2::str::STRCODE c );
  
  // <localize> 文字をアクセントなしにする関数
  //=============================================================================================
  /**
   * 文字をアクセントなしにする関数
   *
   * @param   c		文字
   *
   */
  //=============================================================================================
  static gfl2::str::STRCODE CodeToNoAccent( gfl2::str::STRCODE c );


  // <localize> 文字列をアクセントなし＆大文字化する関数
  //=============================================================================================
  /**
   * 文字列をアクセントなし＆大文字化する関数
   *
   * @param   srcbuf  変換前文字列
   * @param   dstbuf  変換後文字列
   *
   */
  //=============================================================================================
  static void ToUpperNoAccent( const gfl2::str::StrBuf* srcbuf, gfl2::str::StrBuf* dstbuf );

};

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_LATINUM_STRING_TOOL_H_INCLUDE
