#if !defined(GFL2_LYTINFOTABLEACCESSOR_H_INCLUDED)
#define GFL2_LYTINFOTABLEACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    gfl2_LytInfoTableAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.02.27
 @brief   gfl_lyt_table_maker.rbが生成するlytdatの情報テーブルのファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <heap/include/gfl2_heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)

//==============================================================================
/**
 @class     LytInfoTableAccessor
 @brief     lytdatの情報テーブルのアクセッサー
 */
//==============================================================================
class LytInfoTableAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(LytInfoTableAccessor);

public:
  //! @brief  定数
  enum
  {
    INFO_TABLE_ALIGN = 4,
  };
  enum
  {
    LYTRES_ID_NULL = 0xffff,  // レイアウトがないときの値(info_tableファイル内でも使われている)
  };

public:
  //! @brief  コンストラクタ
  LytInfoTableAccessor(void);
  //! @brief  デストラクタ
  virtual ~LytInfoTableAccessor();

public:
  //! @brief  info_tableファイルを読み込んだバッファを渡す
  //! @param[in]  buf  通常メモリでもデバイスメモリでも、どちらに確保していてもいい。
  //                   4バイトアラインメントになっていること。
  void SetInfoTableBuf(const void* buf);

  
  //! @brief  設定してあるinfo_tableバッファを外す
  void UnsetInfoTableBuf(void);


  //! @brief  lytdatにレイアウトbflytが収録されているか
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @return  lytresIdが収録されている場合はtrueを返す
  bool DoesLytExist(u32 lytresId) const;


  //! @brief  レイアウトに含まれる部品ペイン数を得る(部品ペインの子まではカウントしない)
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  u32 GetPartsPaneNum(u32 lytresId) const;


  //! @brief  レイアウトに含まれるアニメ数を得る(部品ペインの子まではカウントしない)
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  u32 GetAnimNum(u32 lytresId) const;


  //! @breif  レイアウトに含まれる部品ペインのテーブルを得る(部品ペインの子までは含めない)。
  //          部品ペインそのものを得る。
  //
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと。
  //
  //! @return  paneId(「PANENAME_flyt名_ペイン名」で定義された番号のこと)の配列(要素数GetPartsPaneNum)。
  //           GetPartsLytTableと対応している。
  const u16* GetPartsPaneTable(u32 lytresId) const;


  //! @breif  レイアウトに含まれる部品ペインのテーブルを得る(部品ペインの子までは含めない)。
  //          部品ペインとして使われている部品レイアウトを得る。
  //          その部品レイアウトがlytdatに収録されていないこともある。
  //
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと。
  //
  //! @return  lytresId(「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと)の配列(要素数GetPartsPaneNum)。
  //                      部品レイアウトがlytdatに収録されていないときはLYTRES_ID_NULLが入れてある。
  //           GetPartsPaneTableと対応している。
  const u16* GetPartsLytTable(u32 lytresId) const;


  //! @breif  レイアウトに含まれるアニメのテーブルを得る(部品ペインの子までは含めない)。
  //
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと。
  //
  //! @return  lytresId(「LYTRES_lytdat名_..._BFLAN」で定義された番号のこと)の配列(要素数GetAnimNum)。
  const u32* GetAnimTable(u32 lytresId) const;


private:
  //! @brief lytresIdのbflyt情報のバッファの先頭を得る。
  const u8* getBflytInfoBuf(u32 lytresId) const;

  //! @brief  u32配列のテーブルを得る。
  // 次のルールを満たしたものにこの関数は使える。
  //   ・bflytInfoBufのbyteCount目の位置に、startBytePosがu32で書かれている。
  //   ・startBytePosはbflytInfoBufのどこからテーブルが始まるかを示す値。
  //   ・startBytePosが0だったらテーブルなし。
  const u32* getU32Table(u32 lytresId, u32 byteCountOfStartBytePosInBflytInfoBuf) const;

  //! @brief  u16配列のテーブルを得る。
  // 次のルールを満たしたものにこの関数は使える。
  //   ・bflytInfoBufのbyteCount目の位置に、startBytePosがu32で書かれている。
  //   ・startBytePosはbflytInfoBufのどこからテーブルが始まるかを示す値。
  //   ・startBytePosが0だったらテーブルなし。
  const u16* getU16Table(u32 lytresId, u32 byteCountOfStartBytePosInBflytInfoBuf) const;
 

private:
  const u8* m_buf;  // NULLでないとき正しく使える

  // アクセスを早くするために覚えておく
  u16        m_version;
  u32        m_bflytInfoNum;                // lytdatに収録されているレイアウトbflyt情報の個数を得る
  const u32* m_bflytInfoStartBytePosTable;  // bflyt情報の開始バイト位置をテーブルにしておく(要素数m_bflytInfoNum)
};


GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)


#endif  // GFL2_LYTINFOTABLEACCESSOR_H_INCLUDED

