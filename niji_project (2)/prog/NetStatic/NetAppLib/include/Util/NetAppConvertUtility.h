//==============================================================================
/**
 * @file	ConvertUtility.h
 * @brief	汎用のコンバート系クラス
 * @author	ichiraku_katsuhiko -> yuto_uchida が移植
 * @date	2013/01/28, 18:21
 */
// =============================================================================

#if !defined( __CONVERTUTILITY_H__ )
#define	__CONVERTUTILITY_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <str/include/gfl2_StrBuf.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(Util)

/// 汎用のデータコンバート
class ConvertUtility
{
public:

  static const u32 STR_MAX = 16;            ///< 最大文字数
  static const u32 STR_MAX_WITH_HYPHEN = 19;///< ハイフンありの最大文字数

  static const int RESULT_ERROR = 0xFFFF;   ///< 変換失敗

private:
  /// コンストラクタ (隠蔽化)
  ConvertUtility( void );
  /// デストラクタ (隠蔽化)
  ~ConvertUtility( void );

public:

  //--------------------------------------------------------------------------
  /**
   * @brief  u64の数値を特定の規則に従って文字列に変換
   *
   * @param   [in]id        変換元数値
   * @param   [out]outStr   変換後文字列
   * @param   [in]addHyphenCode        ハイフンコードを4文字区切りでつけるかどうか
   */
  //--------------------------------------------------------------------------
  static void U64ToStr( const u64 id, gfl2::str::StrBuf *outStr , bool addHyphenCode = false );

  //--------------------------------------------------------------------------
  /**
   * @brief   U64ToStrで生成された文字列をu64の数値に変換
   *
   * @param   [in]inStr   変換元文字列
   * @return  id          変換後数値
   * @li      　変換失敗した場合はRESULT_ERRORが返る
   */
  //--------------------------------------------------------------------------
  static u64 StrToU64( gfl2::str::StrBuf *inStr );

private:
  // ゲームシンクの数値を文字変換
  static wchar_t _GetGameSyncCode( u16 num );
  static u16     _GetGameSyncNum( wchar_t ch );

};

GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetApp)

#endif	/* __CONVERTUTILITY_H__ */

