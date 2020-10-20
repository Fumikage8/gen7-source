#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPOKEPICMANAGER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPOKEPICMANAGER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanPokePicManager.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.30
 @brief   図鑑の絵巻のポケモン画像のマネージャ
 @note    zukan_poke_pic_accessor_maker.rbが生成したzukan_poke_pic_accessor.binの中身にアクセスする。
          画像を読み込む。
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <fs/include/gfl2_Fs.h>
#include <Layout/include/gfl2_Layout.h>

// 図鑑のインクルード
#include "ZukanContents.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPokePicManager
 @brief     図鑑の絵巻のポケモン画像のマネージャ
 */
//==============================================================================
class ZukanPokePicManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPokePicManager);

private:
  static const u32 PIC_NUM_MAX          = 15;  // 同時に表示する画像の枚数(イーブイ9匹+前のシートに3匹+後のシートに3匹)  // @caution 今回はこの数でOKだが、イーブイの前後が4匹になったら増やさないといけないので注意して下さい。
  static const u32 LOADING_PIC_NUM_MAX  =  4;  // 同時に読み込み中になる画像の枚数

  static const u32 ZUKAN_POKE_PIC_ARC_FILE_MEM_SIZE = 256000;  // zukan_poke_pic.garcをオープンしっぱなしにするためのメモリサイズ。
  static const u32 PIC_MEM_SIZE                     = 131840;  // 画像1枚のメモリサイズ。128の倍数
  static const u32 PIC_COMP_MEM_SIZE                =  65536;  // 圧縮時のメモリサイズ。4の倍数

  static const u32 PIC_PANE_NUM_MAX =          4;  // 同じ画像を貼ることができるピクチャペインの最大数(残像３つ＋黒１つ)
  static const u32 FREE_COUNT_MAX   = 2000000000;  // 画像を使わなくなってから何フレーム経過したかのカウントの最大値

  /* デフォルトのテクスチャサイズ */
  static const u32 DEFAULT_WIDTH = 256;
  static const u32 DEFAULT_HEIGHT = 256;
  /* 光ドラゴン専用のテクスチャサイズ */
  static const u32 LARGE_WIDTH = 512;
  static const u32 LARGE_HEIGHT = 256;

public:
  ZukanPokePicManager(gfl2::heap::HeapBase* parentDevHeap);
  virtual ~ZukanPokePicManager();

  // 生成
  // trueを返すまで呼び続ける。trueを返したら生成完了。
  bool CreatingManager(void);
  // 破棄
  // trueを返すまで呼び続ける。trueを返したら破棄完了。
  bool DestroyingManager(void);

  void SetPokePicAccessorBin(const void* pokePicAccessorBin) { m_pokePicAccessorBin = pokePicAccessorBin; }

  // 更新
  // 毎フレーム呼ぶ
  void Update(void);

  // ピクチャペインにポケモン画像を貼る
  // ポケモン画像がまだないときはポケモン画像の読み込みをリクエストする。Updateを呼んでいればそのうち貼られる。
  void SetupPicPane( gfl2::lyt::LytPicture* pane, const ZukanContents::PokeData* pokeData, u32 figureDataIndex, const bool isLarge = false );
  // ピクチャペインに貼ってあったポケモン画像をはがす
  // ポケモン画像が貼られていようが貼られる前だろうが呼んで構わない。
  void UnsetupPicPane(gfl2::lyt::LytPicture* pane, const ZukanContents::PokeData* pokeData, u32 figureDataIndex);
  // ポケモン画像の準備ができているか
  bool IsPokePicReady(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const;

private:
  // PicDataの状態
  class PicDataState
  {
  public:
    enum Tag
    {
      NONE,                 // 何もない
      LOADING,              // ロード中
      LOADING_THEN_UNLOAD,  // ロード中、その後アンロードする
      EXIST,                // 存在している
    };
  };
  // ピクチャペインの情報
  class PicPaneInfo
  {
  public:
    gfl2::lyt::LytPicture* pane;
    gfl2::lyt::LytResTextureInfo texInfo;  // 元々のテクスチャ情報
  public:
    PicPaneInfo(void)
      : pane(NULL),
        texInfo()
    {}
    void Set(gfl2::lyt::LytPicture* a_pane)
    {
      if(a_pane)
      {
        pane = a_pane;
        a_pane->GetMaterial(0)->GetTexMap(0).GetTextureInfo(&texInfo);
      }
      else
      {
        pane = a_pane;
        texInfo.Set(gfl2::lyt::LytResTextureInfo());
      }
    }
  };
  // PicData
  class PicData
  {
  public:
    // 固定
    gfl2::heap::HeapBase* devHeap;
    // 変動
    const ZukanContents::PokeData* pokeData;
    u32                            figureDataIndex;
    gfl2::fs::ArcFile::ARCDATID    arcDatId;  // 使っているか使っていないかの判定、同じものか否かの判定に使う
    void*                          buf;
    size_t                         realReadSize;
    bool                           texLoaded;
    gfl2::lyt::LytResTextureInfo   texInfo;
    PicDataState::Tag              state;
    PicPaneInfo                    picPaneInfo[PIC_PANE_NUM_MAX];
    u32                            picPaneNum;
    u32                            freeCount;
  public:
    PicData(void)
      : devHeap(NULL),
        pokeData(NULL),
        figureDataIndex(0),
        arcDatId(gfl2::fs::ArcFile::ARCDATID_NULL),
        buf(NULL),
        realReadSize(0),
        texLoaded(false),
        texInfo(),
        state(PicDataState::NONE),
        picPaneNum(0),
        freeCount(0)
    {}
  };
  // 読み込み中の情報
  class LoadingPicInfo
  {
  public:
    // 固定
    gfl2::heap::HeapBase* compDevHeap;
    // 変動
    const PicData* pLoadingPicData;
  public:
    LoadingPicInfo(void)
      : compDevHeap(NULL),
        pLoadingPicData(NULL)
    {}
  };

private:
  // マネージャの状態
  class ManagerState
  {
  public:
    enum Tag
    {
      NONE,
      OPENING_ARC_FILE,
      WORKING,
      CLOSING_ARC_FILE,
    };
  };

private:
  // zukan_poke_pic_accessor.binを使ってpokeDataとfigureDataIndexからARCDATIDを得る
  gfl2::fs::ArcFile::ARCDATID getArcDatId(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const;
  // figureDataIndexの容姿の画像のインデックス(同じmonsNoの先頭画像からのインデックス)を得る
  u32 getPicIndexOfFigureData(const ZukanContents::PokeData* pokeData, u32 figureDataIndex) const;
 
  // PicDataの更新
  void updatePicData(PicData* picData);

  // テクスチャロード
  void loadTex(PicData* picData);
  // テクスチャ破棄
  void deleteTexDestroyBuf(PicData* picData);

  // テクスチャをピクチャペインに設定する
  void setTexToPicPane(PicData* picData, u32 picPaneIndex);
  // ピクチャペインの設定を元に戻す
  void resetTexToPicPane(PicData* picData, u32 picPaneIndex);

  // 空いている読み込み枠をゲットする
  LoadingPicInfo* getBlankLoadingPicInfo(void);
  // 占有していた読み込み枠をリリースする
  void releaseLoadingPicInfo(const PicData* picData);  // Blankにする

private:
  bool                  m_destroyReq;
  ManagerState::Tag     m_managerState;
  const void*           m_pokePicAccessorBin;
  gfl2::heap::HeapBase* m_arcFileDevHeap;
  LoadingPicInfo        m_loadingPicInfo[LOADING_PIC_NUM_MAX];  // 読み込み中のPicDataを覚えておく
  PicData               m_picData[PIC_NUM_MAX];
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPOKEPICMANAGER_H_INCLUDED

