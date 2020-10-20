//======================================================================
/**
* @file DebugZoneNameLoader.h
* @date 2016/10/04 16:18:19
* @author ikuta_junya
* @brief デバッグ機能用、ゾーン名文字列をロードする機能
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

#if !defined __DEBUGZONENAMELOADER_H_INCLUDED__
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
class ZoneNameLoader
{
  GFL_FORBID_COPY_AND_ASSIGN( ZoneNameLoader );

public:

  ZoneNameLoader( void );
  virtual ~ZoneNameLoader();

  /**
  * @breaf データをロードする 
  *
  * @param pHeap ロード先を確保するためのヒープ
  * @return 成否
  */
  b32 Load( gfl2::heap::HeapBase* pHeap );

  /**
  * @breaf データを削除
  */
  void Delete( void );

  /**
  * @breaf ゾーンラベル文字列からゾーンIDを取得する
  * @param pName      ゾーンラベル文字列
  * @param pOut       ゾーンID格納先
  * @return ゾーンIDが見当たらなかった場合、false
  **/
  b32 GetZoneId( const c8* pName, u32* pOut ) const;

  /**
  * @breaf データが存在するか取得
  * @return true 存在する
  */
  b32 Exists( void ) const { return (m_pZoneNameBuf != NULL); }

private:


  c8*     m_pZoneNameBuf;         // ゾーン名データ、読み込み先
  size_t  m_ZoneNameDataSize;     // ゾーン名データ実データサイズ

};

GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_END(Field)

#endif // __DEBUGZONENAMELOADER_H_INCLUDED__
#endif