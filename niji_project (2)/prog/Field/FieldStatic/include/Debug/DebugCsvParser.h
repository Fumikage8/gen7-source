//======================================================================
/**
* @file DebugCsvParser.h
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief csvファイルのパーサー、CTR/Win32共通。デバッグ用。
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined __DEBUGCSVPARSER_H_INCLUDED__
#define __DEBUGCSVPARSER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
* @class csvファイルのパーサー、CTR/Win32共通。デバッグ用。
* @note コンマ形式のみ対応
*/
class CsvParser
{
  GFL_FORBID_COPY_AND_ASSIGN( CsvParser );

public:

  class RowObject
  {
    GFL_FORBID_COPY_AND_ASSIGN( RowObject );
  public:

    struct ColumnData;

    RowObject( gfl2::heap::HeapBase* pHeap, c8* pData );
    virtual ~RowObject();  

    s32 GetSize( void ) const { return m_Size; }
    const c8* GetColumnData( s32 columnNo ) const;
    u32 GetColumnTextLen( s32 columnNo ) const;   // @note 長さに\0は含まれない
    b32 Comparisons( s32 columnNo, gfl2::heap::HeapBase* pHeap, const wchar_t* pText ) const; // @note マルチバイト→ワイド変換用バッファにヒープを使用する

  private:

    s32 GetDataSize( c8* pData ) const;

    ColumnData* m_pColumnDataTable;
    s32 m_Size;
  };

  CsvParser( void );
  virtual ~CsvParser();
  CsvParser( gfl2::heap::HeapBase* pHeap, const c8* pFilePath );

  /**
  * @breaf 多バイト文字列 (multibyte string) をワイド文字列 (wide string) に変換します
  * @param wcstr      ド文字列を格納する場所
  * @param mbstr      変換する多バイト文字列
  * @param count      変換する文字列の文字数
  * @return 変換に成功したとき: 変換した文字数 (終端のナルワイド文字は除く)/失敗or不正な多バイト文字が含まれているとき: (size_t)(-1)
  */
  static size_t MbstoWcs( wchar_t *wcstr, const char *mbstr, size_t count );

  /**
  * @breaf データを設定
  * @note  既に設定されている場合は失敗します
  *
  * @param  pData コンマ形式で記述されて文字列(メモリ管理も渡す) 
  * @param  size  データのサイズ
  *
  * @return なし
  */
  void SetData( c8* pData, s32 size );

  /**
  * @breaf データを削除
  */
  void DeleteData( void );

  /**
  * @breaf 1行分のデータを取得
  * @param 1行分のデータ管理オブジェクト作成に使用するヒープ
  * @param index 行番号
  * @return 1行分のデータ管理オブジェクト( index値が不正な場合はNULL )
  */
  RowObject* GetRowData( gfl2::heap::HeapBase* pHeap, s32 index );

  /**
  * @breaf 行数を取得
  * @return 行数
  */
  s32 GetLineNum( void ) const { return m_LineNum; }

  /**
  * @breaf データが存在するか取得
  * @return true 存在する
  */
  b32 Exists( void ) const { return (m_pData != NULL); }

private:

  /**
  * @breaf 行の頭を取得
  * @param index 行番号
  * @return 行の頭のポインタ
  */
  c8* GetLineTop( s32 index );


  c8*  m_pData;       // 実データ
  s32  m_LineNum;     // 読み込んでいるデータの行数
};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __DEBUGCSVPARSER_H_INCLUDED__
#endif