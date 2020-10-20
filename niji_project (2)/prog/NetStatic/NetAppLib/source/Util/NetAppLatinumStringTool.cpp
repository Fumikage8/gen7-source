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
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_DebugPrint.h>
#include "NetStatic/NetAppLib/include/Util/NetAppLatinumStringTool.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


// <localize> 文字列の一致判定（大文字小文字を同一視）
//=============================================================================================
/**
 * 文字列の一致判定（大文字小文字を同一視）
 *
 * @param   src
 * @param   dst
 *
 * @retval  BOOL  一致していたらTRUE
 */
//=============================================================================================
bool NetAppLatinumStringTool::CompareBufferNoCaseNoAccent( const gfl2::str::StrBuf* srcbuf, const gfl2::str::StrBuf* dstbuf )
{
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();
  const gfl2::str::STRCODE* dst = dstbuf->GetPtr();

  while( *src != gfl2::str::EOM_CODE )
  {
    if( CodeToUpper(CodeToNoAccent(*src)) != CodeToUpper(CodeToNoAccent(*dst)) )
    {
      return false;
    }
    src++;
    dst++;
  }

  if( *dst != gfl2::str::EOM_CODE ){
    return false;
  }

  return true;

}

//=============================================================================================
/**
 * @brief	入力文字列を大文字変換＆ウムラウト文字のアルファベット化
 *        ※30文字以上の文字列がきたら危険！
 *
 * @param   srcbuf		入力文字列
 * @param   dstbuf		出力文字列
 */
//=============================================================================================
void NetAppLatinumStringTool::ToUpperNoAccent( const gfl2::str::StrBuf* srcbuf, gfl2::str::StrBuf* dstbuf )
{
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();
  gfl2::str::STRCODE tmp[30];
  gfl2::str::STRCODE* dst = tmp;
  gfl2::std::MemClear( tmp, 30 );

  while( *src != gfl2::str::EOM_CODE ){
    *dst = CodeToUpper(CodeToNoAccent( *src ));
    src++;
    dst++;
  }
  dstbuf->SetStr( tmp );
}

// <localize> 文字を大文字にする関数
//=============================================================================================
/**
 * 文字を大文字にする関数
 *
 * @param   c		文字
 *
 */
//=============================================================================================
gfl2::str::STRCODE NetAppLatinumStringTool::CodeToUpper( gfl2::str::STRCODE c )
{
  if (L'a' <= c && c <= L'z')
  {
	  return c + (L'A' - L'a');
  }
  else
  {
    if (!(L'A' <= c && c <= L'Z'))
    {
      // アルファベット以外が入っている
      GFL_PRINT("LatinumStringTool::ToUpper: 想定外の文字%02x\n", c);
    }
    return c;
  }
}


// <localize> 文字をアクセントなしにする関数
//=============================================================================================
/**
 * 文字をアクセントなしにする関数
 *
 * @param   c		文字
 *
 */
//=============================================================================================
gfl2::str::STRCODE NetAppLatinumStringTool::CodeToNoAccent( gfl2::str::STRCODE c )
{
  // 対応表の先頭に対応する文字
  static const unsigned char REMOVE_ACCENT_TABLE_STARTCODE =0xc0;
  // 対応表の末尾に対応する文字
  static const unsigned char REMOVE_ACCENT_TABLE_LASTCODE = 0xff;
  
  // アクセントつきアルファベット( U+0xc0〜U+0xff ) からアクセントなしアルファベットへの対応表
  // ※アクセントなしアルファベットのコードは128未満であるため、char で節約が可能
  static const unsigned char RemoveAccentTable[0x40] = 
  {
  	 'A',  'A',  'A',  'A',  'A',  'A', '\0',  'C',  // AAAAAA_C
  	 'E',  'E',  'E',  'E',  'I',  'I',  'I',  'I',  // EEEEIIII
  	 'D',  'N',  'O',  'O',  'O',  'O',  'O', '\0',  // ?NOOOOO_
  	 'O',  'U',  'U',  'U',  'U',  'Y',  'Y',  's',  // OUUUUYTs
  	 'a',  'a',  'a',  'a',  'a',  'a', '\0',  'c',  // aaaaaa_c
  	 'e',  'e',  'e',  'e',  'i',  'i',  'i',  'i',  // eeeeiiii
  	 'd',  'n',  'o',  'o',  'o',  'o',  'o', '\0',  // dnooooo_
  	 'o',  'u',  'u',  'u',  'u',  'y',  'y',  'y',  // ouuuuyty
  };

  if (REMOVE_ACCENT_TABLE_STARTCODE <= c && c <= REMOVE_ACCENT_TABLE_LASTCODE)
  {
    char result = RemoveAccentTable[c - REMOVE_ACCENT_TABLE_STARTCODE];
    return result != 0 ? static_cast<gfl2::str::STRCODE>(result) : c;
  }
  else
  {
    return c;
  }
}

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)
