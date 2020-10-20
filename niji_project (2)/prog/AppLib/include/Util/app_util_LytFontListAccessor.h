#if !defined(APP_UTIL_LYTFONTLISTACCESSOR_H_INCLUDED)
#define APP_UTIL_LYTFONTLISTACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_util_LytFontListAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.14
 @brief   gfl_lyt_font_list_maker.rbが生成するfont_listファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class   LytFontListAccessor
 @brief   font_listのアクセッサー 
 */
//==============================================================================
class LytFontListAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(LytFontListAccessor);

public:
  //! @brief  コンストラクタ
  LytFontListAccessor(void);
  //! @brief  デストラクタ
  virtual ~LytFontListAccessor();

public:
  //! @brief  font_listファイルを読み込んだバッファを渡す
  //! @param[in]  buf  通常メモリでもデバイスメモリでも、どちらに確保していてもいい。
  //                   4バイトアラインメントになっていること。
  void SetFontListBuf(const void* buf);

  //! @brief  設定してあるfont_listバッファを外す
  void UnsetFontListBuf(void);

  //! @brief  lytdatに収録されているレイアウト全体で使っているフォントの個数を得る(部品ペインの子までは含めない)。
  u32 GetLytDatFontNum(void) const;
  //! @brief  lytdatに収録されているレイアウト全体で使っているフォントの番号のテーブルを得る(部品ペインの子までは含めない)。
  //! @return  フォントの番号(DEFAULT_FONT_IDX_KUJIRA_DEFAULTとか)の配列(要素数GetLytDatFontNum)。
  //           要素数が0のときはNULLを返す。
  const u16* GetLytDatFontIdTable(void) const;

  //! @brief  lytdatにレイアウトbflytが収録されているか
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @return  lytresIdが収録されている場合はtrueを返す
  bool DoesLytExist(u32 lytresId) const;
  //! @brief  レイアウトが使っているフォントの個数を得る(部品ペインの子までは含めない)。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  u32 GetFontNum(u32 lytresId) const;
  //! @breif  レイアウトが使っているフォントの番号のテーブルを得る(部品ペインの子までは含めない)。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと。
  //! @return  フォントの番号(DEFAULT_FONT_IDX_KUJIRA_DEFAULTとか)の配列(要素数GetFontNum)。
  //           要素数が0のときはNULLを返す。
  const u16* GetFontIdTable(u32 lytresId) const;

private:
  //! @brief lytresIdのbflyt情報のバッファの先頭を得る。
  const u8* getBflytInfoBuf(u32 lytresId) const;

private:
  const u8* m_buf;  // NULLでないとき正しく使える

  // アクセスを早くするために覚えておく
  u16         m_version;
  
  u32         m_bflytInfoNum;                // lytdatに収録されているレイアウトbflyt情報の個数
  const u32*  m_bflytInfoStartBytePosTable;  // bflyt情報の開始バイト位置をテーブルにしておく(要素数m_bflytInfoNum)

  u32         m_lytDatFontNum;               // lytdatに収録されているレイアウト全体で使っているフォントの個数
  const u16*  m_lytDatFontIdTable;           // フォントの番号(DEFAULT_FONT_IDX_KUJIRA_DEFAULTとか)をテーブルにしておく(要素数m_lytDatFontNum)
                                             // 要素数が0のときはNULL
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_LYTFONTLISTACCESSOR_H_INCLUDED

