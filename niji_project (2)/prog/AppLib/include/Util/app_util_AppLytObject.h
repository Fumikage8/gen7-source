#if !defined(APP_UTIL_APPLYTOBJECT_H_INCLUDED)
#define APP_UTIL_APPLYTOBJECT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    app_util_AppLytObject.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.01.16
 @brief   applytバイナリからつくるオブジェクト郡
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Font.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class MsgData;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
  class WordSet;
GFL_NAMESPACE_END(print)
class WordSetLoader;

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class G2DUtil;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


//==============================================================================
/**
 @class     AppLytObject
 @brief     applytバイナリからつくるオブジェクト郡 
 */
//==============================================================================
class AppLytObject
{
  GFL_FORBID_COPY_AND_ASSIGN(AppLytObject);


private:
  //! @brief  メッセージバッファ情報
  class MsgBufInfo
  {
  public:
    gfl2::fs::ArcFile::ARCID    arcId;
    gfl2::fs::ArcFile::ARCID    arcIdLang;
    gfl2::fs::ArcFile::ARCDATID datId;
    void*                       msgBuf;
    gfl2::str::MsgData*         msgData;
  public:
    MsgBufInfo(void)
      : arcId(gfl2::fs::ArcFile::ARCID_NULL),
        arcIdLang(gfl2::fs::ArcFile::ARCID_NULL),
        datId(gfl2::fs::ArcFile::ARCDATID_NULL),
        msgBuf(NULL),
        msgData(NULL)
    {}
  };

  //! @brief  フォント生成情報
  class FontCreateInfo
  {
  public:
    enum
    {
      BIT_FLAG_FUNC_EXIST  = 1 <<  0,
      BIT_FLAG_CREATE      = 1 <<  1,
    };
  public:
    bit32 bitFlag;
    void (*CreateFunc)(gfl2::heap::HeapBase *heapMem , gfl2::heap::HeapBase *deviceMem);
    void (*DeleteFunc)(void);
    void (*SetFunc)(void);
    void (*ResetFunc)(void);
    gfl2::str::Font* (*GetFunc)(void);
  public:
    FontCreateInfo(void)
      : bitFlag(0),
        CreateFunc(NULL),
        DeleteFunc(NULL),
        SetFunc(NULL),
        ResetFunc(NULL),
        GetFunc(NULL)
    {}
  };


public:
  AppLytObject(void);
  virtual ~AppLytObject();

  //! @brief  同期生成
  //! @param[in]  appLytBuf  applytバイナリ。このクラスに記憶されるので呼び出し元は削除しないようにして下さい。
  //! @param[in]  heap       ヒープ。この関数を抜けても残るものはここから前方確保させてもらう。この関数内で一時的に使うものはここから後方確保させてもらう。
  //                         システムヒープでもデバイスヒープでもいいが、今作のヒープの使い方の方針に従うならデバイスヒープを渡すとよい。
  void SyncCreate(const void* appLytBuf, gfl2::heap::HeapBase* heap);

  //! @brief  G2DUtilにワードセットを設定する。
  //! @param[in]  g2DUtil  2D関連ユーティリティ。
  //! @param[in]  wordSet  ワードセット。
  //! @param[in]  ploader  g2DUtilにワードセットローダーも設定する場合はNULL以外を入れておく。
  //! @caution  g2DUtilのメンバ変数に次のような設定をするので注意すること！
  //                  m_bMessageCreate = false;
  //                  pMsgData = messageData;
  //! @note  g2DUtilしか変更しないのでstaticを付けてある。
  static void SetWordSetToG2DUtil(app::util::G2DUtil* g2DUtil, print::WordSet* wordSet, WordSetLoader* ploader);

  //! @brief  レイアウトのテキストボックスペインにメッセージを設定する。
  //! @param[in] g2DUtil   2D関連ユーティリティ。
  //! @param[in] wordSet   ワードセット。
  //! @param[in] lytID     g2DUtil内におけるレイアウトワークの番号。
  //! @param[in] lytresId  「LYTRES_lytdat名_..._BFLYT」で定義された番号のこと。
  //                       lytIDをつくったときのlytreIdと同じでなければならない。
  //! @note  g2DUtilしか変更しないのでconstを付けてある。wordSetを使わせてもらうのでそれも変わるかもしれない。
  void SetMessageToTextBoxPane(app::util::G2DUtil* g2DUtil, print::WordSet* wordSet, u32 lytID, u32 lytresId) const;

  //! @brief  メッセージデータを取得する。
  //! @param[in]  arcIdLang  print::GetMessageArcId, print::GetMessageArcId_Debug, print::GetMessageArcId_Scriptで取得したもの
  //! @param[in]  datId      GARC_message_..._DAT
  gfl2::str::MsgData* GetMsgData(gfl2::fs::ArcFile::ARCID arcIdLang, gfl2::fs::ArcFile::ARCDATID datId) const;

private:
  void destroy(void);


private:
  const void* m_appLytBuf;

  // メッセージバッファ情報
  MsgBufInfo* m_msgBufInfo;
  u32         m_msgBufInfoNumMax;
  u32         m_msgBufInfoNum;


  // フォント生成情報
  FontCreateInfo* m_fontCreateInfo;  // 要素数はDEFAULT_FONT_IDX_MAX
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)


#endif  // APP_UTIL_APPLYTOBJECT_H_INCLUDED

