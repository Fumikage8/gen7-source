//======================================================================
/**
* @file DebugSymbolNameLoader.h
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief デバッグ機能用、ゾーン名文字列をロードする機能
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined __DEBUGSYMBOLNAMELOADER_H_INCLUDED__
#define __DEBUGZONENAMELOADER_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Debug)

/**
* @class デバッグ機能用、ゾーン名文字列をロードする機能
*/
class SymbolNameLoader
{
  GFL_FORBID_COPY_AND_ASSIGN( SymbolNameLoader );

public:

  struct SymbolType
  {
    enum Value
    {
      WORLD_ID = 0,
      AREA_ID,
      ZONE_ID,

      VALUE_MAX
    };
  };

  struct DataHeader;

  SymbolNameLoader( void );
  virtual ~SymbolNameLoader();

  /**
  * @breaf データをロードする 
  *
  * @param pHeap        ロード先を確保するためのヒープ
  * @param symbolType   ロードするシンボルデータの種類を指定するオプションです
  * @return 成否
  */
  b32 Load( gfl2::heap::HeapBase* pHeap, SymbolType::Value symbolType );

  /**
  * @breaf データを削除
  */
  void Delete( void );

  /**
  * @breaf 文字列から値を取得する
  * @note  同じ文字列が複数ある場合はデータ順先勝ちとなります
  * @param pName      文字列
  * @param pOut       値を格納するバッファです
  * @return 見当たらなかった場合、false
  **/
  b32 GetValue( const c8* pName, u32* pOut ) const;

  /**
* @breaf 値から文字列を取得します
* @note  同じ値が複数ある場合はデータ順先勝ちとなります
* @param value      値
* @param ppOut      文字列のアドレスを格納するバッファです、実体はこのクラスがロードされたデータです。末尾には\0が付いています。
* @return 見当たらなかった場合、false
**/
  b32 GetName( s32 value, c8** ppOut ) const;

  /**
  * @breaf データが存在するか取得
  * @return true 存在する
  */
  b32 Exists( void ) const { return (m_pDataBuf != NULL); }

  /**
  * @breaf データの内容を出力します
  * @return 無し
  */
  void Dump(void) const;

private:

  c8*     m_pDataBuf;         // データ、読み込み先
  c8*     m_pNameBuf;         // 名前データ、読み込み先(開始位置)
  
  s32     m_NameLengthLimit;  // 名前の長さ制限
  s32     m_NameNum;          // 名前の個数
  size_t  m_DataBufSize;      // 実データサイズ

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __DEBUGZONENAMELOADER_H_INCLUDED__
#endif