#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDATAMANAGER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDATAMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDataManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のデータマネージャ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// 図鑑のインクルード
#include "ZukanType.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
  class AsyncFileManager;
  class BinLinkerAccessor;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanDataManager
 @brief     図鑑のデータマネージャ
 */
//==============================================================================
class ZukanDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDataManager);

public:
  //! @brief  モード
  enum Mode
  {
    MODE_ZUKAN,
    MODE_ZUKAN_REGISTER,
  };

  //! @brief  データID
  enum DataId
  {
    DATA_ID_ZUKAN_RES2D          = 0,
    DATA_ID_ZUKAN_GRPFONT_RES2D,
    DATA_ID_ZUKAN_DISTRIBUTION,
    DATA_ID_TALK_WINDOW_RES2D,
    DATA_ID_MENU_WINDOW_RES2D,
    DATA_ID_FONT_DUMMY_OPEN,    // ↓ここからフォント。必ず最初はOPENにすること。
    DATA_ID_FONT_JAPAN,
    DATA_ID_FONT_CHINA,
    DATA_ID_FONT_TAIWAN,
    DATA_ID_FONT_DUMMY_CLOSE,   // ↑ここからフォント。必ず最後はCLOSEにすること。
    DATA_ID_POKE_ICON_RES2D,
    DATA_ID_ZUKAN_MSTXT,                 //<! 図鑑メインテキスト
    //DATA_ID_POKE_NAME_MSTXT,             //<! ポケモン種族名(ピカチュウ)テキスト
    DATA_ID_POKE_SPECIES_MSTXT,          //<! ポケモン分類名(ねずみポケモン)テキスト
    DATA_ID_POKE_FORM_MSTXT,             //<! ポケモンフォルム名テキスト
    DATA_ID_POKE_HEIGHT_MSTXT,           //<! ポケモン高さテキスト
    DATA_ID_POKE_WEIGHT_MSTXT,           //<! ポケモン重さテキスト
    DATA_ID_POKE_COMMENT_MSTXT,          //<! ポケモンコメントテキスト(バージョン違いあり)
    DATA_ID_INITIAL_MSTXT,               //<! 頭文字
    DATA_ID_ZUKAN_HYOUKA_MSTXT,          //<! 図鑑評価テキスト
    
    DATA_ID_NUM,                         //!< データ数

    DATA_ID_END   = DATA_ID_NUM,         //!< データのオープン～ロード～クローズが、全データに対して全て完了していることを示すデータID。
                                         //   このデータIDにならない例: データのロードは全て終わっているがクローズが終わっていないデータが残っているとき。
    DATA_ID_START = 0                    //!< 最初のデータID。最初のデータはスキップしない必須データにしておくこと。  // DATA_ID_ZUKAN_RES2D
  };

  //! @brief  ZukanRes2dパック要素ID
  enum ZukanRes2dPackElemId
  {
    ZUKAN_RES2D_PACK_ELEM_ID_LAUNCH            = 0,  //!< 起動画面
    ZUKAN_RES2D_PACK_ELEM_ID_ALOLA_TOP,              //!< アローラ図鑑トップ画面
    ZUKAN_RES2D_PACK_ELEM_ID_ISLAND_TOP,             //!< 島図鑑トップ画面
    ZUKAN_RES2D_PACK_ELEM_ID_INFO,                   //!< 情報画面(登録画面)
    ZUKAN_RES2D_PACK_ELEM_ID_REGISTER_COMMENT,       //!< 登録画面に出すコメント層
    ZUKAN_RES2D_PACK_ELEM_ID_MAP,                    //!< 分布画面
    ZUKAN_RES2D_PACK_ELEM_ID_SEARCH,                 //!< 検索画面
    ZUKAN_RES2D_PACK_ELEM_ID_REGISTER_FADE,          //!< 登録画面で使うフェード層
    ZUKAN_RES2D_PACK_ELEM_ID_BG,                     //!< 背景
    ZUKAN_RES2D_PACK_ELEM_ID_ROTOM_FADE,             //!< ロトムフェード
    ZUKAN_RES2D_PACK_ELEM_ID_MENU_WINDOW2,           //!< common_menu_2, MenuWindow2, MENU_WINDOW2
    ZUKAN_RES2D_PACK_ELEM_ID_PIC_SCROLL_DATA,        //!< 登録画面の絵巻データ
    ZUKAN_RES2D_PACK_ELEM_ID_POKE_PIC_ACCESSOR,      //!< 図鑑で使うポケモン画像のアクセッサ
    ZUKAN_RES2D_PACK_ELEM_ID_NUM,                    //!< パックされた要素数
  };

  //! @brief  ZukanDistributionパック要素ID
  enum ZukanDistributionPackElemId
  {
    ZUKAN_DISTRIBUTION_PACK_ELEM_ID_POKE_DATA,      //!< ポケモンの生息地データ
    ZUKAN_DISTRIBUTION_PACK_ELEM_ID_POSITION_DATA,  //!< 生息地の位置データ
    ZUKAN_DISTRIBUTION_PACK_ELEM_ID_ZONE_DATA,      //!< 生息地のゾーンデータ
    ZUKAN_DISTRIBUTION_PACK_ELEM_ID_NUM,            //!< パックされた要素数
  };

public:
  //! @param[in]  heap  随時使うヒープ
  ZukanDataManager(gfl2::heap::HeapBase* heap, gfl2::fs::AsyncFileManager* romAsyncFileManager, Mode mode);
  virtual ~ZukanDataManager();

  //! @brief  破棄できるか
  //! @return  破棄できるときはtrueを返す。Endを呼んでから数フレームUpdateを呼び続ければそのうちtrueが返ってくるようになる。
  bool CanDestroy(void) const;

  //! @brief  更新
  //! @note  毎フレーム呼ぶこと
  void Update(void);

  //! @brief  開始する/再開する
  void Start(void);

  //! @brief  停止する
  //! @note  すぐに停止するわけではない。キリのいいところまでは処理を続ける。
  void Stop(void);

  //! @brief  終了処理を始める
  //! @note  すぐに終了処理が始まるわけではない。キリのいいところまでは処理が進んでから終了処理が始まる。
  void End(void);

  //! @brief  データバッファを取得する
  //! @return  読み込みが完了していないときはNULLを返す。
  //! @note  データバッファの破棄はこのクラスで行うので、呼び出し元では破棄してはならない。
  void* GetDataBuf(DataId dataId) const;

  //! @brief  ZukanRes2dパック要素のバッファを取得する
  //! @return  読み込みが完了していないときはNULLを返す。
  //! @note  バッファの破棄はこのクラスで行うので、呼び出し元では破棄してはならない。
  void* GetZukanRes2dPackElemBuf(ZukanRes2dPackElemId packElemId) const;

  //! @brief  ZukanDistributionパック要素のバッファを取得する
  //! @return  読み込みが完了していないときはNULLを返す。
  //! @note  バッファの破棄はこのクラスで行うので、呼び出し元では破棄してはならない。
  void* GetZukanDistributionPackElemBuf(ZukanDistributionPackElemId packElemId) const;

  const gfl2::fs::ArcFile* GetArcFile(DataId dataId) const;

private:
  //! @brief  (全体の)状態
  enum State
  {
    STATE_STOP,  //!< 停止中
    STATE_WORK,  //!< 実行中
    STATE_END,   //!< 終了している(全データに対する処理が終了している。m_currDataIdはDATA_ID_ENDになっている)
  };

  //! @brief  データの種類
  enum DataType
  {
    DATA_TYPE_APPLYT_PACK,
    DATA_TYPE_APPLYT,
    DATA_TYPE_GARC_BUF,
    DATA_TYPE_MSTXT,
    DATA_TYPE_FONT_OPEN,
    DATA_TYPE_FONT_LOAD,
    DATA_TYPE_FONT_CLOSE,
  };
  //! @brief  データのシーケンス
  enum DataSeq
  {
    DATA_SEQ_START_OPEN,
    DATA_SEQ_WAIT_OPEN,
    DATA_SEQ_START_LOAD,
    DATA_SEQ_WAIT_LOAD,
    DATA_SEQ_START_CLOSE,
    DATA_SEQ_WAIT_CLOSE,
    DATA_SEQ_END,           //!< データの処理が終了している。読み込んだものはバッファに保持したままになっている。
  };

private:
  //! @brief  データ
  class DataInfo
  {
  public:
    // 入力として与えられる固定値
    DataId                      dataId;
    u32                         dataIdOrderIndex;
    DataType                    dataType;
    gfl2::fs::ArcFile::ARCID    arcId;
    gfl2::fs::ArcFile::ARCDATID datId;
    gfl2::fs::ArcFile::ARCDATID endDatId;  // datId<= <=endDatId  // dataTypeがDATA_TYPE_GARC_BUFのときしか使用しない
    u32                         align;
    bool                        closeSoon;   // すぐに閉じるときtrue
    bool                        compressed;  // 解凍するときtrue
    bool                        skip;  // 処理せずスキップするときtrue

    // 読み込み作業で使うもの
    DataSeq                     dataSeq;  // 今行うべきシーケンスが入っている
    void*                       dataBuf;
    const gfl2::fs::ArcFile*    arcFile;  // dataTypeがDATA_TYPE_GARC_BUFのときしか使用しない

  public:
    DataInfo(void)
      : dataId(DATA_ID_END),
        dataIdOrderIndex(0),
        dataType(DATA_TYPE_APPLYT_PACK),
        arcId(gfl2::fs::ArcFile::ARCID_NULL),
        datId(gfl2::fs::ArcFile::ARCDATID_NULL),
        endDatId(gfl2::fs::ArcFile::ARCDATID_NULL),
        align(4),
        closeSoon(false),
        compressed(false),
        skip(false),
        dataSeq(DATA_SEQ_START_OPEN),
        dataBuf(NULL),
        arcFile(NULL)
    {}
  };

private:
  void updateData_StartOpen(DataInfo* dataInfo);
  void updateData_WaitOpen(DataInfo* dataInfo);
  void updateData_StartLoad(DataInfo* dataInfo);
  void updateData_WaitLoad(DataInfo* dataInfo);
  void updateData_StartClose(DataInfo* dataInfo);
  void updateData_WaitClose(DataInfo* dataInfo);
  void updateData_End(DataInfo* dataInfo);

  void initDataInfo(void);
  u32  getDataIdOrderIndex(DataId dataId) const;
  void doAtLoadFinished(DataInfo* dataInfo);
  void doAtCloseNotStarted(DataInfo* dataInfo);
  void doAtCloseFinished(DataInfo* dataInfo);
  void doAtEnd(DataInfo* dataInfo);

  bool isFontAlreadyRead(ZukanType::ZukanLang zukanLang) const;

private:
  gfl2::heap::HeapBase*        m_heap;
  gfl2::fs::AsyncFileManager*  m_romAsyncFileManager;
  Mode                         m_mode;
  ZukanType::ZukanLang         m_gameLang;  // ZUKAN_LANG_  ZUKAN_LANG_を入れる  // ゲーム開始時に選んだ言語。図鑑中に変更されることはない。
  State                        m_state;
  bool                         m_stopReq;  // 停止するようリクエストが出ているときtrue。trueのときm_stateはまだSTATE_STOPになっていない。
  bool                         m_endReq;   // 終了処理を始めるようリクエストが出ているときtrue。trueのときm_stateはまだSTATE_ENDになっていない。
  bool                         m_remainIsCloseAtEnd;  // 残りの処理が終了処理で行うクローズだけになっているときtrue
  DataId                       m_dataIdOrderList[DATA_ID_NUM+1];  // DATA_ID_ENDも入れるので+1  // DataId currDataId = m_dataIdOrderList[m_currDataIdOrderIndex];
  u32                          m_currDataIdOrderIndex;  // 現在処理中のインデックスが入っている。処理中でないときは次に処理すべきインデックスが入っている。
  DataInfo                     m_dataInfo[DATA_ID_NUM];
  gfl2::fs::BinLinkerAccessor* m_zukanRes2dPack;
  gfl2::fs::BinLinkerAccessor* m_zukanDistributionPack;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDATAMANAGER_H_INCLUDED

