#if !defined(APP_UTIL_APPLYTACCESSOR_H_INCLUDED)
#define APP_UTIL_APPLYTACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_util_AppLytAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.01.16
 @brief   applytファイルのアクセッサー 
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>


// nijiのインクルード
#include <AppLib/include/Util/app_util_LytMstxtListAccessor.h>
#include <AppLib/include/Util/app_util_LytFontListAccessor.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class     AppLytAccessor
 @brief     applytファイルのアクセッサー 

 @note      gfl2::lyt::LYTSYS_PANEINDEX_ERRORを使っているところだけ、gfl2::lytのものに依存している。
 */
//==============================================================================
class AppLytAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(AppLytAccessor);

public:
  //! @brief  定数
  enum
  {
    APPLYT_ALIGN = 128,
  };

public:
  //! @brief  コンストラクタ
  AppLytAccessor(void);
  //! @brief  デストラクタ
  virtual ~AppLytAccessor();

public:
  //! @brief  applytファイルを読み込んだバッファを渡す
  //! @param[in]  buf  デバイスメモリに確保していること。
  //                   128バイトアラインメントになっていること。
  void SetAppLytBuf(const void* buf);

  //! @brief  設定してあるapplytバッファを外す
  void UnsetAppLytBuf(void);

  //! @brief  info_tableファイル部分を得る
  const void* GetLytInfoTable(void) const;
  //! @brief  mstxt_listファイル部分を得る
  const void* GetLytMstxtList(void) const;
  //! @brief  font_listファイル部分を得る
  const void* GetLytFontList(void) const;
  //! @brief  lytdatファイル部分を得る
  const void* GetLytDat(void) const;

public:
  //! @name  自動生成のための関数(info_tableを使用)
  //@{
  //! @brief  部品ペインの総数を取得 ( parts_max )
  u32 GetPartsMax( u32 lytresId ) const;
  //! @brief  アニメの総数を取得 ( anime_max )
  u32 GetAnimeMax( u32 lytresId ) const;
  //! @brief  アニメテーブルを取得 ( anime_tbl )
  const gfl2::lyt::LytArcIndex * GetAnimeTable( u32 lytresId ) const;
  //! @brief  部品ペインのインデックステーブルを取得 ( parts_pane_tbl )
  const u16 * GetPartsPaneTable( u32 lytresId ) const;
  //! @brief  部品ペインの本体のレイアウトインデックステーブルを取得 ( parts_layout_tbl )
  const u16 * GetPartsLayoutTable( u32 lytresId ) const;
  //@}

private:
  const u8* getBlockBuf(u32 blockIndex) const;

private:
  const u8* m_buf;  // NULLでないとき正しく使える

  gfl2::lyt::LytInfoTableAccessor m_infoTableAccessor;
  LytMstxtListAccessor            m_mstxtListAccessor;
  LytFontListAccessor             m_fontListAccessor;

  // アクセスを早くするために覚えておく
  u16       m_version;
  const u8* m_infoTableBlockBuf;
  const u8* m_mstxtListBlockBuf;
  const u8* m_fontListBlockBuf;
  const u8* m_lytdatBlockBuf;
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_APPLYTACCESSOR_H_INCLUDED

