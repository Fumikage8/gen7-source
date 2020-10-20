#if !defined(APP_UTIL_LYTMSTXTLISTACCESSOR_H_INCLUDED)
#define APP_UTIL_LYTMSTXTLISTACCESSOR_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_util_LytMstxtListAccessor.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.10
 @brief   gfl_lyt_mstxt_list_maker.rbが生成するmstxt_listファイルのアクセッサー 
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
 @class   LytMstxtListAccessor
 @brief   mstxt_listのアクセッサー 
 */
//==============================================================================
class LytMstxtListAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(LytMstxtListAccessor);

public:
  //! @brief  テキストボックスペインの情報
  struct TextBoxPaneInfo
  {
    u16  paneId;           //!< 「PANENAME_flyt名_ペイン名」で定義された番号のこと
    u16  messageArcId;     //!< テキストボックスペインが使っているメッセージgarcのARCID
    u16  messageArcDatId;  //!< テキストボックスペインが使っているメッセージgarcにおいて、そこから使用しているデータのARCDATID(mstxtのこと)
    u16  messageId;        //!< テキストボックスペインが使っているメッセージのID(messageArcIdのmessageArcDatIdで示すmstxt内のメッセージのIDのこと)
  };

public:
  //! @brief  コンストラクタ
  LytMstxtListAccessor(void);
  //! @brief  デストラクタ
  virtual ~LytMstxtListAccessor();

public:
  //! @brief  mstxt_listファイルを読み込んだバッファを渡す
  //! @param[in]  buf  通常メモリでもデバイスメモリでも、どちらに確保していてもいい。
  //                   4バイトアラインメントになっていること。
  void SetMstxtListBuf(const void* buf);

  //! @brief  設定してあるmstxt_listバッファを外す
  void UnsetMstxtListBuf(void);




  //! @brief  lytdatにレイアウトbflytが収録されているか
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @return  lytresIdが収録されている場合はtrueを返す
  bool DoesLytExist(u32 lytresId) const;




  //! @name  レイアウトごとの情報
  //@{


  //! @name  レイアウトごとのメッセージの情報
  //@{

  //! @brief  レイアウトが使っているメッセージgarcの個数を得る(部品ペインの子までは含めない)。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  u32 GetMessageArcNum(u32 lytresId) const;
  //! @breif  レイアウトが使っているメッセージgarcのARCIDを得る。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[in] index     何個目のメッセージgarcか
  //! @return  存在しないindexを渡したときはARCID_NULLを返す。
  u32 GetMessageArcId(u32 lytresId, u32 index) const;
  //! @breif  レイアウトが使っているメッセージgarcにおいて、そこから使用しているデータの個数を得る(即ち、mstxtの個数を得る)。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[in] index     何個目のメッセージgarcか
  //! @param[in] arcId     メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときは0を返す。
  u32 GetMessageArcDatNumByIndex(u32 lytresId, u32 index) const;
  u32 GetMessageArcDatNumByArcId(u32 lytresId, u32 arcId) const;
  //! @breif  レイアウトが使っているメッセージgarcにおいて、そこから使用しているデータのARCDATIDのテーブルを得る(即ち、mstxtのテーブルを得る)。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[in] index     何個目のメッセージgarcか
  //! @param[in] arcId     メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときはNULLを返す。
  //           個数が0のときはNULLを返す。
  const u16* GetMessageArcDatIdTableByIndex(u32 lytresId, u32 index) const;
  const u16* GetMessageArcDatIdTableByArcId(u32 lytresId, u32 arcId) const;

  //@}  // レイアウトごとのメッセージの情報


  //! @name  レイアウトごとのテキストボックスペインの情報
  //@{
  
  //! @brief  レイアウトに含まれるテキストボックスペインの個数を得る(部品ペインの子までは含めない)
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  u32 GetTextBoxPaneNum(u32 lytresId) const;
  //! @brief  レイアウトに含まれるテキストボックスペインの情報のテーブルを得る(部品ペインの子までは含めない)
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @return  TextBoxPaneInfoの配列(要素数GetTextBoxPaneNum)。
  //           要素数が0のときはNULLを返す。
  const TextBoxPaneInfo* GetTextBoxPaneInfoTable(u32 lytresId) const;

  //@}  // レイアウトごとのテキストボックスペインの情報


  //@}  // レイアウトごとの情報



  
  //! @name  lytdat全体の情報
  //@{

  //! @brief  lytdatに収録されているレイアウト全体で使っているメッセージgarcの個数を得る(部品ペインの子までは含めない)。
  u32 GetLytDatMessageArcNum(void) const;
  //! @breif  lytdatに収録されているレイアウト全体で使っているメッセージgarcのARCIDを得る。
  //! @param[in] index  何個目のメッセージgarcか
  //! @return  存在しないindexを渡したときはARCID_NULLを返す。
  u32 GetLytDatMessageArcId(u32 index) const;
  //! @breif  lytdatに収録されているレイアウト全体で使っているメッセージgarcにおいて、そこから使用しているデータの個数を得る(即ち、mstxtの個数を得る)。
  //! @param[in] index  何個目のメッセージgarcか
  //! @param[in] arcId  メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときは0を返す。
  u32 GetLytDatMessageArcDatNumByIndex(u32 index) const;
  u32 GetLytDatMessageArcDatNumByArcId(u32 arcId) const;
  //! @breif  lytdatに収録されているレイアウト全体で使っているメッセージgarcにおいて、そこから使用しているデータのARCDATIDのテーブルを得る(即ち、mstxtのテーブルを得る)。
  //! @param[in] index  何個目のメッセージgarcか
  //! @param[in] arcId  メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときはNULLを返す。
  //           個数が0のときはNULLを返す。
  const u16* GetLytDatMessageArcDatIdTableByIndex(u32 index) const;
  const u16* GetLytDatMessageArcDatIdTableByArcId(u32 arcId) const;

  //@}  // lytdat全体の情報




private:
  ////////////////////////////////////////////////////////////////////////////////
  //! @brief lytresIdのbflyt情報のバッファの先頭を得る。
  const u8* getBflytInfoBuf(u32 lytresId) const;
  //! @breif  レイアウトが使っているメッセージ情報のバッファの先頭を得る。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[in] index     何個目のメッセージgarcか
  //! @param[in] arcId     メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときはNULLを返す。
  //           個数が0のときはNULLを返す。
  const u8* getMessageInfoBufByIndex(u32 lytresId, u32 index) const;
  const u8* getMessageInfoBufByArcId(u32 lytresId, u32 arcId) const;
  //! @brief lytresIdのbflyt情報のバッファの先頭を得る。
  //! @breif  レイアウトが使っている「メッセージ情報の個数」と「メッセージ情報の開始バイト位置テーブル」を得る。
  //! @param[in]  lytresId                      「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[out] bflytInfoBuf                  bflyt情報のバッファ(lytresIdが存在しないときはNULL)
  //! @param[out] messageInfoNum                メッセージ情報の個数(lytresIdが存在しないときは0)
  //! @param[out] messageInfoStartBytePosTable  メッセージ情報の開始バイト位置テーブル(lytresIdが存在しないときはNULL、個数が0のときはNULL)
  void getBflytMessageInfo(u32 lytresId, const u8** bflytInfoBuf, u32* messageInfoNum, const u32** messageInfoStartBytePosTable) const;
  //! @brief  レイアウトに含まれるテキストボックスペインの個数を得る(部品ペインの子までは含めない)
  //! @brief  レイアウトに含まれるテキストボックスペインの情報のテーブルを得る(部品ペインの子までは含めない)
  //! @param[in] lytresId               「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと
  //! @param[out] textBoxPaneNum        テキストボックスペインの個数(lytresIdが存在しないときは0)
  //! @param[out] textBoxPaneInfoTable  テキストボックスペインの情報テーブル(TextBoxPaneInfoの配列(要素数textBoxPaneNum))(lytresIdが存在しないときはNULL、要素数が0のときはNULLを返す)
  void getTextBoxPaneNumAndTextBoxPaneInfoTable(u32 lytresId, u32* textBoxPaneNum, const TextBoxPaneInfo** textBoxPaneInfoTable) const;
  ////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  //! @brief lytdatに収録されているレイアウト全体で使っているメッセージ情報のバッファの先頭を得る。
  //! @param[in] index  何個目のメッセージgarcか
  //! @param[in] arcId  メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときはNULLを返す。
  //           個数が0のときはNULLを返す。
  const u8* getLytDatMessageInfoBufByIndex(u32 index) const;
  const u8* getLytDatMessageInfoBufByArcId(u32 arcId) const;
  ////////////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////////////
  //! メッセージ情報のバッファの先頭を得る。
  //! @param[in] buf                           バッファ
  //! @param[in] messageInfoNum                メッセージ情報の個数
  //! @param[in] messageInfoStartBytePosTable  メッセージ情報の開始バイト位置(要素数messageInfoNum)(bufの先頭からの位置)
  //! @param[in] index                         何個目のメッセージgarcか
  //! @param[in] arcId                         メッセージgarcのARCID
  //! @return  存在しないindex, arcIdを渡したときはNULLを返す。
  //           個数が0のときはNULLを返す。
  static const u8* getMessageInfoBufByIndexStatic(const u8* buf, u32 messageInfoNum, const u32* messageInfoStartBytePosTable, u32 index);
  static const u8* getMessageInfoBufByArcIdStatic(const u8* buf, u32 messageInfoNum, const u32* messageInfoStartBytePosTable, u32 arcId);
  //! @breif  メッセージ情報のバッファを渡し、そこから使用しているデータの個数を得る(即ち、mstxtの個数を得る)。
  //! @breif  メッセージ情報のバッファを渡し、そこから使用しているデータのARCDATIDのテーブルを得る(即ち、mstxtのテーブルを得る)。
  //! @param[in]  mesasgeInfoBuf メッセージ情報のバッファ
  //! @param[out] arcDatNum      使用しているデータの個数を得る(即ち、mstxtの個数を得る)(存在しないindex, arcIdを渡したときは0)
  //! @param[out] arcDatIdTable  使用しているデータのARCDATIDのテーブルを得る(即ち、mstxtのテーブルを得る)(存在しないindex, arcIdを渡したときはNULL、個数が0のときはNULL)
  static void getMessageArcDatNumAndArcDatIdTableStatic(const u8* messageInfoBuf, u32* arcDatNum, const u16** arcDatIdTable);
  ////////////////////////////////////////////////////////////////////////////////
 



private:
  const u8* m_buf;  // NULLでないとき正しく使える

  // アクセスを早くするために覚えておく
  u16         m_version;

  u32         m_bflytInfoNum;                // lytdatに収録されているレイアウトbflyt情報の個数
  const u32*  m_bflytInfoStartBytePosTable;  // bflyt情報の開始バイト位置をテーブルにしておく(要素数m_bflytInfoNum)

  u32         m_lytDatMessageInfoNum;                // lytdatに収録されているレイアウト全体で使っているメッセージ情報の個数
  const u32*  m_lytDatMessageInfoStartBytePosTable;  // メッセージ情報の開始バイト位置をテーブルにしておく(要素数m_lytDatMessageInfoNum)
                                                     // 要素数が0のときはNULL
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_LYTMSTXTLISTACCESSOR_H_INCLUDED

