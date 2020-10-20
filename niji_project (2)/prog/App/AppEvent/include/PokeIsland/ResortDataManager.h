#if !defined(NIJI_PROJECT_APP_RESORT_RESORTDATAMANAGER_H_INCLUDED)
#define NIJI_PROJECT_APP_RESORT_RESORTDATAMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ResortDataManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.04.11
 @brief   ポケリゾートのデータマネージャ
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


//! @brief  ポケリゾートのデータマネージャ
class ResortDataManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortDataManager);

public:
  //! @brief  ファイルID
  enum
  {
    FILE_ID_RESORT,    // resort.garc
    FILE_ID_POKEICON,  // PokeIcon3D.garc
    FILE_ID_NUM
  };
  //! @brief  データ
  enum
  {
    DATA_RESORT_APPLYT,
    DATA_FIELDWINDOW_APPLYT,
    DATA_ISLANDBIN_PACK,
    DATA_ISLANDSHADOW_PACK,
    DATA_ISLANDGROUND_PACK,
    DATA_POKEICON,
    DATA_NUM
  };
    
  //! @brief  islandground_島名_レベル_pack.pack内
  enum
  {
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_LAND_MODEL,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_LAND_COLLI,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_LAND_ANIM,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_OBJ_MODEL,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_OBJ_ANIM,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_POKE_MODEL,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_POKE_ANIM,
    ISLANDGROUND_ISLAND_LV_PACK_ELEM_NUM,
  };

public:
  ResortDataManager(void);
  virtual ~ResortDataManager();
  
  //! @brief  生成
  //! @param[in] parentDevMem  親デバイスメモリ。ここからデータマネージャ専用のデバイスメモリを生成する。
  //! @note  更新開始を待つ回数を0で初期化する
  void Create(gfl2::heap::HeapBase* parentDevMem);
  //! @brief  更新開始を待つ回数を設定する
  void SetWaitUpdateCount(u32 count);
  //! @brief  破棄
  void Destroy(void);
  //! @brief  破棄できるか
  //! @note  毎フレーム1回Updateを呼んでtrueが返ってくるのを待って下さい
  bool CanDestroy(void) const;

  //! @brief  ストップを解除する
  void ReleaseStop(void);
  //! @brief  更新(毎フレーム1回呼ぶこと)
  void Update(void);

  //! @brief  圧縮ファイルを読み込むためのワークヒープを取得する(使用中のときはNULLを返す)
  gfl2::heap::HeapBase* GetUncompWorkHeap(void) const;

  //! @brief  ファイルにアクセスできるか(ファイルがオープンしていないときはNULLを返す(オープン中クローズ中もNULLを返す))
  //! @param[in] fileId  FILE_ID_???
  bool IsFileAccessible(s32 fileId) const;
  //! @brief  ArcFileを取得する
  //! @param[in] fileId  FILE_ID_???
  const gfl2::fs::ArcFile* GetArcFile(s32 fileId) const;

  //! @brief  データ取得(まだデータが準備できていないときはNULLを返す)
  //! @param[in] dataId  DATA_???
  void* GetData(s32 dataId) const;

  //! @name  リゾートで使い易くするためにstaticインスタンスを用意しておく
  //@{
public:
  static void SetStaticInstance(ResortDataManager* instance);
  static void UnsetStaticInstance(void);
  static ResortDataManager* GetStaticInstance(void);
private:
  static ResortDataManager* s_instance;
  //@}

private:
  void init(void);
  void changeFileOp(s32 fileOp);

private:
  //! @brief  ファイルオペレーション(この順番でファイル操作する)(順番を変更するときはFILE_OP_で検索して、変更しても影響のないようにして下さい)
  enum
  {
    FILE_OP_RESORT_OPEN,         // resort.garcオープン
    FILE_OP_RESORT_PACK_LOAD,    // resort.garcからresort_pack.packを読み込む
    FILE_OP_POKEICON_OPEN_LOAD,  // PokeIcon3D.garcオープン＆ロード
    FILE_OP_STOP,                // 停止
    FILE_OP_CLOSE,               // このマネージャが扱っているファイルを全てクローズ
    FILE_OP_END,                 // 終了
    FILE_OP_NUM
  };

  //! @brief  バッファ 
  enum
  {
    BUF_RESORT_PACK,
    BUF_POKEICON,
    BUF_NUM
  };
  
  //! @brief  BinLinkerパック
  enum
  {
    // garc内
    PACK_RESORT,

    // 本クラス管理外のpackはコメントアウト
    //PACK_ISLANDGROUND_FUREAI_1,
    //PACK_ISLANDGROUND_FUREAI_2,
    //PACK_ISLANDGROUND_FUREAI_3,
    //PACK_ISLANDGROUND_KINOMI_1,
    //PACK_ISLANDGROUND_KINOMI_2,
    //PACK_ISLANDGROUND_KINOMI_3,
    //PACK_ISLANDGROUND_TANKEN_1,
    //PACK_ISLANDGROUND_TANKEN_2,
    //PACK_ISLANDGROUND_TANKEN_3,
    //PACK_ISLANDGROUND_UNDOU_1,
    //PACK_ISLANDGROUND_UNDOU_2,
    //PACK_ISLANDGROUND_UNDOU_3,
    //PACK_ISLANDGROUND_ONSEN_1,
    //PACK_ISLANDGROUND_ONSEN_2,
    //PACK_ISLANDGROUND_ONSEN_3,

    // resort_pack.pack内にあるBinLinkerパック
    PACK_ISLANDBIN,
    PACK_ISLANDSHADOW,
    PACK_ISLANDGROUND,

    PACK_NUM
  };

  //! @brief  ArcFile
  enum
  {
    ARC_FILE_POKEICON,
    ARC_FILE_NUM
  };

private:
  bool                         m_created;
  gfl2::heap::HeapBase*        m_devMem;
  gfl2::heap::HeapBase*        m_uncompWorkMem;
  u32                          m_waitUpdateCount;
  s32                          m_fileOp;
  s32                          m_subSeq;
  bool                         m_releaseStopFlag;
  void*                        m_buf[BUF_NUM];
  gfl2::fs::BinLinkerAccessor  m_pack[PACK_NUM];
  const gfl2::fs::ArcFile*     m_arcFile[ARC_FILE_NUM];
};


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // NIJI_PROJECT_APP_RESORT_RESORTDATAMANAGER_H_INCLUDED

