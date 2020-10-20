#include <model/include/gfl2_CharaModelFactory.h>
#include "gfl2_CharaModelResourceUnit.h"
#include "gfl2_DressUpModelResourceUnit.h"

// renderingengine
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfMdlResourceNodeFactory.h>

#include <fs/include/gfl2_BinLinkerAccessor.h>

// 仮
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>

#include <thread/include/gfl2_ThreadStatic.h>

namespace poke_3d { namespace model {

// 2タイプ持っているパーツの判定
static bool HasDressUpParts2Type(DressUpParam::ItemCategory itemCategory)
{
  // FACEは影のON/OFFで2種類
  // HAIRは影のON/OFFで2種類
  // LEGSはSHORT/LONGで2種類
  return ((itemCategory == DressUpParam::ITEM_CATEGORY_FACE) || 
    (itemCategory == DressUpParam::ITEM_CATEGORY_HAIR_STYLE) ||
    (itemCategory == DressUpParam::ITEM_CATEGORY_LEGS));
}

// 仕様的に削除されたもう要らないタイプの判定：メモリ配置が換わるのでデータ的にはそのまま残してはいる
static bool IsDressUpPartsDeleted(DressUpParam::ItemCategory itemCategory)
{
  return ((itemCategory == DressUpParam::ITEM_CATEGORY_ACCBADGE) || (itemCategory == DressUpParam::ITEM_CATEGORY_BODY));
}

// 着せ替えセットアップシーケンス
enum DressUpSetupSeq
{
  DRESSUP_SETUP_SEQ_CREATE_PARTS, // パーツ作成
  DRESSUP_SETUP_SEQ_CHANGE_COLOR, // 色替え
  DRESSUP_SETUP_SEQ_END           // 終了
};

// ノーマルGARCのフッター
class NormalGarcFooter
{
public:

  // モデル数取得
  u32 GetModelCount() const
  {
    return m_ModelCount;
  }

  // 動的アニメ数取得
  u32 GetDyAnimCount() const
  {
    return m_DyAnimCount;
  }

private:

  u32 m_ModelCount;   ///< モデル数
  u32 m_DyAnimCount;  ///< 動的アニメ数
};

// ノーマルGARCの動的アニメオフセットリスト
class NormalGarcDyAnimOffsetList
{
public:

  // 動的アニメオフセット取得
  u32 GetDyAnimOffset(u32 index) const
  {
    return m_OffsetList[index];
  }

private:

  u32 m_OffsetList[1];  ///< オフセットリスト
};

// 着せ替えGARCのフッターパラメータ
struct DressUpGarcFooterParam
{
  u16 modelIndex;     ///< 何番のアイテムが何番のモデルを使うかのリスト
  u16 partsIndex;

  s8 colorType;
  s8 colorIndex;
  u8 optionBit;
  s8 reserved1;
};

// 着せ替えパーツリスト
class DressUpPartsList
{
public:

  // パーツ数取得
  u32 GetPartsCount() const
  {
    return m_PartsCount;
  }

  // カラービット取得
  u64 GetColorBit(u32 partsIndex) const
  {
    return m_ColorBitList[partsIndex];
  }

private:

  u32 m_PartsCount;             ///< パーツ数
  u32 m_Reserved0;              ///< いつか使うかも予約
  u64 m_ColorBitList[1];        ///< 有効なカラービットリスト
};

// 着せ替えGARCのフッター
class DressUpGarcFooter
{
public:

  // アイテム数取得
  u32 GetItemCount() const
  {
    return m_ItemCount;
  }

  // アイテムインデックスからモデルインデックス取得
  u32 GetModelIndex(s32 itemIndex) const
  {
    return GetFooterParam(itemIndex)->modelIndex;
  }

  // アイテムインデックスからパーツインデックス取得
  u32 GetPartsIndex(s32 itemIndex) const
  {
    return GetFooterParam(itemIndex)->partsIndex;
  }

  // アイテムインデックスからカラータイプ取得
  s8 GetColorType(s32 itemIndex) const
  {
    return GetFooterParam(itemIndex)->colorType;
  }

  // アイテムインデックスからカラーインデックス取得
  s8 GetColorIndex(s32 itemIndex) const
  {
    return GetFooterParam(itemIndex)->colorIndex;
  }

  // アイテムインデックスからオプションビット取得
  u8 GetOptionBit(s32 itemIndex) const
  {
    return GetFooterParam(itemIndex)->optionBit;
  }

private:

  const DressUpGarcFooterParam* GetFooterParam(s32 itemIndex) const
  {
    u32 uItemIndex = (u32)itemIndex;
    if (uItemIndex >= m_ItemCount)
    {
      GFL_ASSERT(uItemIndex >= m_ItemCount);
      return NULL;
    }

    return &m_FooterParamList[itemIndex];
  }

private:

  u32 m_ItemCount;                          ///< アイテム数
  DressUpGarcFooterParam m_FooterParamList[1];  ///< フッターパラメータリスト
};

// 着せ替えGARCの動的アニメオフセットリスト
class DressUpGarcDyAnimOffsetList
{
public:

  // 動的アニメオフセット取得
  u32 GetDyAnimOffset(u32 index) const
  {
    return m_OffsetList[index];
  }

private:

  u32 m_OffsetList[1];  ///< オフセットリスト
};


// カラーパラメータ
struct ColorParam
{
  u8 r;         ///< R値 0 ~ 255
  u8 g;         ///< G値 0 ~ 255
  u8 b;         ///< B値 0 ~ 255
  u8 reserved;  ///< いつか使うかも予約


  gfl2::math::Vector3 GetRgb() const
  {
    return gfl2::math::Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
  }
};

// 例外カラーパラメータ
struct ColorParamExt
{
  ColorParam param;   ///< カラーパラメータ
  u8 changeFromType;  ///< 置き換え先のカラータイプ
  u8 changeFromIndex; ///< 置き換え先のカラーインデックス
  u8 reserved0;       ///< いつか使うかも予約
  u8 reserved1;       ///< いつか使うかも予約
};

// カラーテーブル
class ColorTable
{
public:

  // カラー数取得
  u32 GetColorCount() const
  {
    return m_ColorCount;
  }

  // カラーパラメータ取得
  const ColorParam* GetColorParam(u32 colorIndex) const
  {
    return &m_ColorParams[colorIndex];
  }

private:

  u32 m_ColorCount;             ///< カラー数
  ColorParam m_ColorParams[1];  ///< カラーパラメータ
};

// カラーテーブル例外
class ColorTableExt
{
public:

  // カラー数取得
  u32 GetColorCount() const
  {
    return m_ColorCount;
  }

  // カラーパラメータ取得
  const ColorParamExt* GetColorParam(u32 colorIndex) const
  {
    return &m_ColorParams[colorIndex];
  }

private:

  u32 m_ColorCount;                 ///< カラー数
  ColorParamExt m_ColorParams[1];   ///< カラーパラメータ
};

// ドレスアップパラメータのラッパー
class DressUpParamWrapper
{
public:

  // コンストラクタ
  DressUpParamWrapper(DressUpGarcFooter** pFooterList, const DressUpParam* pDressUpParam) :
    m_pFooterList(pFooterList),
    m_pDressUpParam(pDressUpParam)
  {
  }

  // ボディインデックス取得
  s16 GetBodyIndex(DressUpParam::BodyCategory bodyCategory) const
  {
    return m_pDressUpParam->bodyParams[bodyCategory];
  }

  // アイテムインデックス取得
  s16 GetItemIndex(DressUpParam::ItemCategory itemCategory) const
  {
    s16 itemIndex = m_pDressUpParam->itemParams[itemCategory];

    if (itemIndex < 0)
    {
      return -1;
    }

    // faceとhair用の特殊対処
    if ((itemCategory == DressUpParam::ITEM_CATEGORY_FACE) || (itemCategory == DressUpParam::ITEM_CATEGORY_HAIR_STYLE))
    {
      // _ON
      // _OFF
      // の順番で格納されている
      itemIndex *= 2;

      // hatをかぶっていなければ_OFFに変更
      s16 hatIndex = this->GetItemIndex(DressUpParam::ITEM_CATEGORY_HAT);
      if (hatIndex < 0)
      {
        itemIndex += 1;
      }
    }
    // legs用の特殊対処
    if (itemCategory == DressUpParam::ITEM_CATEGORY_LEGS)
    {
      // _LONG
      // _SHORT
      // の順番で格納されている
      itemIndex *= 2;

      // bottomsのオプションがshortなら_SHORTに変更
      s16 bottomsIndex = this->GetItemIndex(DressUpParam::ITEM_CATEGORY_BOTTOMS);
      if (bottomsIndex >= 0)
      {
        // ここがNULLだとGARCが開かれていないのにファイルにアクセスしようとしている
        GFL_ASSERT(m_pFooterList[DressUpParam::ITEM_CATEGORY_BOTTOMS] != NULL);
        u8 optionBit = m_pFooterList[DressUpParam::ITEM_CATEGORY_BOTTOMS]->GetOptionBit(bottomsIndex);
        if (optionBit & (1 << DressUpModelResourceUnit::OPTION_BIT_LEGS))
        {
          itemIndex += 1;
        }
      }
    }

    return itemIndex;
  }

  // モデルインデックス取得
  s16 GetModelIndex(DressUpParam::ItemCategory itemCategory) const
  {
    s16 itemIndex = this->GetItemIndex(itemCategory);
    if (itemIndex < 0)
    {
      return -1;
    }

    // ここがNULLだとGARCが開かれていないのにファイルにアクセスしようとしている
    GFL_ASSERT(m_pFooterList[itemCategory] != NULL);
    return m_pFooterList[itemCategory]->GetModelIndex(itemIndex);
  }

private:
  DressUpGarcFooter** m_pFooterList;
  const DressUpParam* m_pDressUpParam;
};

// 常駐してGARCを管理するためのクラス
class ResidentCharaModelFactory
{
public:

  ResidentCharaModelFactory() :
    m_ArcId(0),
    m_pFileManager(NULL),
    m_pFileHeap(NULL),

    m_pNormalGarcDyAnimOffsetList(NULL),
    m_pNormalGarcFooter(NULL),

    m_InitSeq(0),
    m_UseFlag(false)
  {
  }

  /**
   * @brief 初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcId GARCのID
   */
  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
  {
    // 初期化開始
    this->InitializeAsync(pFileManager, pFileHeap, arcId);

    while (!this->WaitInitializeAsync())
    {
      // 初期化完了待ち
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
  }

  /**
   * @brief 初期化：非同期版
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcId GARCのID
   */
  void InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
  {
    m_UseFlag = true;

    // ファイルマネージャとヒープ登録
    m_pFileManager = pFileManager;
    m_pFileHeap = pFileHeap;

    m_ArcId = arcId;
    m_InitSeq = 0;
  }

  /**
   * @brief 非同期版の初期化完了待ち
   *
   * @return true:初期化完了 false:初期化中
   */
  bool WaitInitializeAsync()
  {
    enum
    {
      SEQ_OPEN,
      SEQ_LOAD,
      SEQ_SETUP,
      SEQ_END
    };

    switch (m_InitSeq)
    {
    case SEQ_OPEN:
      // キャラモデルのGARCを開く
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
        req.arcId				= m_ArcId;
        req.heapForFile	= m_pFileHeap;
        req.heapForReq	= m_pFileHeap->GetLowerHandle();

        m_pFileManager->AddArcFileOpenReq(req);

        ++m_InitSeq;
      }
      break;

    case SEQ_LOAD:
      {
        // ファイルオープン待ち
        if (!m_pFileManager->IsArcFileOpenFinished(m_ArcId))
        {
          break;
        }

        u32 normalGarcDataCount = 0;
        // ノーマルGARCのアーカイブ数読み取り
        {
          const gfl2::fs::ArcFile* pArcFile = m_pFileManager->GetArcFile(m_ArcId);
          pArcFile->GetDataCount(&normalGarcDataCount);
        }

        // ノーマルGARCの動的アニメオフセットリスト
        {
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
          req.arcId      = m_ArcId;
          req.datId      = normalGarcDataCount - 2;
          req.ppBuf      = (void**)&m_pNormalGarcDyAnimOffsetList;
          req.heapForBuf = m_pFileHeap;
          req.align      = 4;
          req.heapForReq = m_pFileHeap->GetLowerHandle();
          req.heapForCompressed = NULL;

          m_pFileManager->AddArcFileLoadDataReq(req);
        }

        // ノーマルGARCの詳細読み取り
        {
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
          req.arcId      = m_ArcId;
          req.datId      = normalGarcDataCount - 1;
          req.ppBuf      = (void**)&m_pNormalGarcFooter;
          req.heapForBuf = m_pFileHeap;
          req.align      = 4;
          req.heapForReq = m_pFileHeap->GetLowerHandle();
          req.heapForCompressed = NULL;

          m_pFileManager->AddArcFileLoadDataReq(req);
        }

        ++m_InitSeq;
      }
      break;

    case SEQ_SETUP:
      {
        // ファイル読み取り待ち
        if (!(m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pNormalGarcDyAnimOffsetList) && m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pNormalGarcFooter)))
        {
          break;
        }

        ++m_InitSeq;
      }
      // そのまま下へ
      //break;

    case SEQ_END:
      {
        return true;
      }
    }

    return false;
  }

  /**
   * @brief 終了処理
   */
  void Finalize()
  {
    m_UseFlag = false;

    // キャラモデルのGARCを閉じる
    gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
    req.arcId				= m_ArcId;

    m_pFileManager->SyncArcFileClose(req);

    // 動的アニメオフセットリスト解放
    GflHeapSafeFreeMemory(m_pNormalGarcDyAnimOffsetList);

    // フッター解放
    GflHeapSafeFreeMemory(m_pNormalGarcFooter);
  }

  bool IsUsed()
  {
    return m_UseFlag;
  }

  u32 GetArcId()
  {
    return m_ArcId;
  }

  NormalGarcDyAnimOffsetList* GetNormalGarcDyAnimOffsetList()
  {
    return m_pNormalGarcDyAnimOffsetList;
  }

  NormalGarcFooter* GetNormalGarcFooter()
  {
    return m_pNormalGarcFooter;
  }

private:

  u32 m_ArcId;                                                ///< 読み込み対象のアーカイブID
  gfl2::fs::AsyncFileManager* m_pFileManager;                 ///< ファイルマネージャ
  gfl2::heap::HeapBase* m_pFileHeap;                          ///< 読み込み用のヒープ

  NormalGarcDyAnimOffsetList* m_pNormalGarcDyAnimOffsetList;  ///< ノーマルGARCの動的アニメオフセットリスト
  NormalGarcFooter* m_pNormalGarcFooter;                      ///< ノーマルGARCのフッター

  u32 m_InitSeq;                                              ///< 初期化シーケンス
  bool m_UseFlag;                                             ///< 使用中フラグ
};

// 常駐してGARCを管理するためのクラス：着せ替え
class ResidentDressUpModelResourceManager
{
public:

  ResidentDressUpModelResourceManager() :
    m_pFileManager(NULL),
    m_pFileHeap(NULL),

    m_ArcIdColor(-1),
    m_ArcIdList(NULL),

    m_pGarcFooterList(NULL),
    m_pDressUpPartsList(NULL),
    m_pDressUpDyAnimOffsetList(NULL),
    m_pColorTableList(NULL),
    m_pColorTableExt(NULL),
    m_pColorTableBuf(NULL),

    m_InitSeq(0),
    m_UseFlag(false)
  {
  }

  /**
   * @brief 初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
  {
    // 初期化開始
    this->InitializeAsync(pFileManager, pFileHeap, arcIdList);

    while (!this->WaitInitializeAsync())
    {
      // 初期化完了待ち
      gfl2::thread::ThreadStatic::CurrentSleep(1);
    }
  }

  /**
   * @brief 初期化：非同期版
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  void InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
  {
    m_UseFlag = true;
    m_InitSeq = 0;

    m_pFileManager = pFileManager;
    m_pFileHeap = pFileHeap;

    m_ArcIdList = GFL_NEW_ARRAY(m_pFileHeap) s32[DressUpModel::PARTS_MODEL_COUNT];
    m_pGarcFooterList = GFL_NEW_ARRAY(m_pFileHeap) DressUpGarcFooter*[DressUpModel::PARTS_MODEL_COUNT];
    m_pDressUpPartsList = GFL_NEW_ARRAY(m_pFileHeap) DressUpPartsList*[DressUpModel::PARTS_MODEL_COUNT];
    m_pDressUpDyAnimOffsetList = GFL_NEW_ARRAY(m_pFileHeap) DressUpGarcDyAnimOffsetList*[DressUpModel::PARTS_MODEL_COUNT];

    // カラーテーブル
    m_ArcIdColor = *arcIdList;

    // 各パーツ
    ++arcIdList;
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      m_ArcIdList[i] = -1;
      m_pGarcFooterList[i] = NULL;
      m_pDressUpPartsList[i] = NULL;
      m_pDressUpDyAnimOffsetList[i] = NULL;

      // もう要らないデータ
      if (IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
      {
        continue;
      }

      // アーカイブID保存
      m_ArcIdList[i] = arcIdList[i];
    }
  }

  /**
   * @brief 非同期版の初期化完了待ち
   *
   * @return true:初期化完了 false:初期化中
   */
  bool WaitInitializeAsync()
  {
    enum
    {
      SEQ_OPEN,
      SEQ_LOAD,
      SEQ_SETUP,
      SEQ_END
    };

    switch (m_InitSeq)
    {
    case SEQ_OPEN:
      {
        // デバッグ用チェックでリクエスト数があふれるのでチェックを無効に
        bool fsMatchCheckFlag = m_pFileManager->GetMatchCheckFlag();
        m_pFileManager->SetMatchCheckFlag(false);

        // カラーテーブルを開く
        {
          gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
          req.arcId				= m_ArcIdColor;
          req.heapForFile	= m_pFileHeap;
          req.heapForReq	= m_pFileHeap->GetLowerHandle();

          m_pFileManager->AddArcFileOpenReq(req);
        }

        // 全パーツを開く
        for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
        {
          if (m_ArcIdList[i] < 0)
          {
            continue;
          }

          // GARCを開く
          gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
          req.arcId				= m_ArcIdList[i];
          req.heapForFile	= m_pFileHeap;
          req.heapForReq	= m_pFileHeap->GetLowerHandle();

          m_pFileManager->AddArcFileOpenReq(req);
        }

        // デバッグ用チェックを戻す
        m_pFileManager->SetMatchCheckFlag(fsMatchCheckFlag);
        ++m_InitSeq;
      }
      break;

    case SEQ_LOAD:
      {
        // カラーテーブルオープン待ち
        if (!m_pFileManager->IsArcFileOpenFinished(m_ArcIdColor))
        {
          break;
        }
        // 全パーツオープン待ち
        bool isPartsOpen = true;
        for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
        {
          if (m_ArcIdList[i] < 0)
          {
            continue;
          }

          if (!m_pFileManager->IsArcFileOpenFinished(m_ArcIdList[i]))
          {
            isPartsOpen = false;
            break;
          }
        }
        if (!isPartsOpen)
        {
          break;
        }
        
        // デバッグ用チェックでリクエスト数があふれるのでチェックを無効に
        bool fsMatchCheckFlag = m_pFileManager->GetMatchCheckFlag();
        m_pFileManager->SetMatchCheckFlag(false);

        // カラーテーブル読み込み
        {
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
          req.arcId      = m_ArcIdColor;
          req.datId      = 0;
          req.ppBuf      = &m_pColorTableBuf;
          req.pRealReadSize = NULL;
          req.heapForBuf = m_pFileHeap;
          req.align      = 4;
          req.heapForReq = m_pFileHeap->GetLowerHandle();
          req.heapForCompressed = NULL;

          m_pFileManager->AddArcFileLoadDataReq(req);
        }

        // 全パーツ読み込み
        for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
        {
          if (m_ArcIdList[i] < 0)
          {
            continue;
          }

          u32 garcDataCount = 0;
          // GARCのアーカイブ数読み取り
          {
            const gfl2::fs::ArcFile* pArcFile = m_pFileManager->GetArcFile(m_ArcIdList[i]);
            pArcFile->GetDataCount(&garcDataCount);
          }

          // 動的アニメオフセットリスト読み取り
          {
            gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
            req.arcId      = m_ArcIdList[i];
            req.datId      = garcDataCount - 3;
            req.ppBuf      = (void**)&m_pDressUpDyAnimOffsetList[i];
            req.pRealReadSize = NULL;
            req.heapForBuf = m_pFileHeap;
            req.align      = 8;
            req.heapForReq = m_pFileHeap->GetLowerHandle();
            req.heapForCompressed = NULL;

            m_pFileManager->AddArcFileLoadDataReq(req);
          }

          // パーツリスト読み取り
          {
            gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
            req.arcId      = m_ArcIdList[i];
            req.datId      = garcDataCount - 2;
            req.ppBuf      = (void**)&m_pDressUpPartsList[i];
            req.pRealReadSize = NULL;
            req.heapForBuf = m_pFileHeap;
            req.align      = 8;
            req.heapForReq = m_pFileHeap->GetLowerHandle();
            req.heapForCompressed = NULL;

            m_pFileManager->AddArcFileLoadDataReq(req);
          }

          // フッター読み取り
          {
            gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
            req.arcId      = m_ArcIdList[i];
            req.datId      = garcDataCount - 1;
            req.ppBuf      = (void**)&m_pGarcFooterList[i];
            req.pRealReadSize = NULL;
            req.heapForBuf = m_pFileHeap;
            req.align      = 4;
            req.heapForReq = m_pFileHeap->GetLowerHandle();
            req.heapForCompressed = NULL;

            m_pFileManager->AddArcFileLoadDataReq(req);
          }
        }

        // デバッグ用チェックを戻す
        m_pFileManager->SetMatchCheckFlag(fsMatchCheckFlag);
        ++m_InitSeq;
      }
      break;

    case SEQ_SETUP:
      {
        // カラーテーブル読み込み待ち
        if (!m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pColorTableBuf))
        {
          break;
        }

        // 全パーツ読み込み待ち
        bool isPartsLoad = true;
        for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
        {
          if (m_ArcIdList[i] < 0)
          {
            continue;
          }

          if (!(m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pDressUpDyAnimOffsetList[i]) && 
            m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pDressUpPartsList[i]) &&
            m_pFileManager->IsArcFileLoadDataFinished((void**)&m_pGarcFooterList[i])))
          {
            isPartsLoad = false;
            break;
          }
        }
        if (!isPartsLoad)
        {
          break;
        }

        // カラーテーブルセットアップ
        gfl2::fs::BinLinkerAccessor binLinkerAccessor(m_pColorTableBuf);
        u16 dataCount = binLinkerAccessor.GetDataMax();

        // 拡張カラーと合わせて必ず2以上はあるはず
        GFL_ASSERT(dataCount >= 2);
        if (dataCount >= 2)
        {
          u16 colorTableCount = dataCount - 1U;
          m_pColorTableList = GFL_NEW_ARRAY(m_pFileHeap) ColorTable*[colorTableCount];
          for (u32 i = 0; i < colorTableCount; ++i)
          {
            m_pColorTableList[i] = (ColorTable*)binLinkerAccessor.GetData(i);
          }
          // 拡張カラーは通常カラーの後に格納されている
          m_pColorTableExt = (ColorTableExt*)binLinkerAccessor.GetData(colorTableCount);
        }

        ++m_InitSeq;
      }
      // そのまま下へ
      //break;

    case SEQ_END:
      {
        return true;
      }
    }

    return false;
  }

  /**
   * @brief 終了処理
   */
  void Finalize()
  {
    m_UseFlag = false;

    // カラーテーブルのGARCを閉じる
    {
      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= m_ArcIdColor;

      m_pFileManager->SyncArcFileClose(req);
    }

    // キャラモデルのGARCを閉じる
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      if (m_ArcIdList[i] < 0)
      {
        continue;
      }

      gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
      req.arcId				= m_ArcIdList[i];

      m_pFileManager->SyncArcFileClose(req);
    }

    // パーツリスト解放
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      GflHeapSafeFreeMemory(m_pDressUpPartsList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pDressUpPartsList);

    // GARCのフッター解放
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      GflHeapSafeFreeMemory(m_pGarcFooterList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pGarcFooterList);

    // 動的アニメのオフセットリスト解放
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      GflHeapSafeFreeMemory(m_pDressUpDyAnimOffsetList[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pDressUpDyAnimOffsetList);

    // カラーテーブル解放
    GFL_SAFE_DELETE_ARRAY(m_pColorTableList);
    GflHeapSafeFreeMemory(m_pColorTableBuf);

    // アーカイブID解放
    GFL_SAFE_DELETE_ARRAY(m_ArcIdList);
  }

  bool IsUsed() const
  {
    return m_UseFlag;
  }

  s32 GetArcIdColor()
  {
    return m_ArcIdColor;
  }

  s32* GetArcIdList()
  {
    return m_ArcIdList;
  }

  DressUpGarcFooter** GetGarcFooterList()
  {
    return m_pGarcFooterList;
  }

  DressUpPartsList** GetDressUpPartsList()
  {
    return m_pDressUpPartsList;
  }

  DressUpGarcDyAnimOffsetList** GetDressUpDyAnimOffsetList()
  {
    return m_pDressUpDyAnimOffsetList;
  }

  ColorTable** GetColorTableList()
  {
    return m_pColorTableList;
  }

  ColorTableExt* GetColorTableExt()
  {
    return m_pColorTableExt;
  }

private:

  gfl2::fs::AsyncFileManager* m_pFileManager;
  gfl2::heap::HeapBase* m_pFileHeap;

  s32 m_ArcIdColor;
  s32* m_ArcIdList;

  DressUpGarcFooter** m_pGarcFooterList;
  DressUpPartsList** m_pDressUpPartsList;
  DressUpGarcDyAnimOffsetList** m_pDressUpDyAnimOffsetList;
  ColorTable** m_pColorTableList;
  ColorTableExt* m_pColorTableExt;
  void* m_pColorTableBuf;

  u32 m_InitSeq;
  bool m_UseFlag;
};

static CharaModelFactory::ReloadFunc s_ReloadFunc = NULL;                             ///< リロード用の関数

static const u32 RESIDENT_CHARA_MODEL_FACTORY_COUNT = 2; // battle, field
static ResidentCharaModelFactory s_ResidentCharaModelFactoryList[RESIDENT_CHARA_MODEL_FACTORY_COUNT];

static const u32 RESIDENT_DRESSUP_RESOURCE_MANAGER_COUNT = 4; // battle_hero, battle_heroine, field_hero, field_heroine
static ResidentDressUpModelResourceManager s_ResidentDressUpResourceManagerList[RESIDENT_DRESSUP_RESOURCE_MANAGER_COUNT];

/**
 * @brief 常駐化初期化
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcId GARCのID
 */
void CharaModelFactory::ResidentInitialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
{
  ResidentCharaModelFactory* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_CHARA_MODEL_FACTORY_COUNT; ++i)
  {
    if (s_ResidentCharaModelFactoryList[i].IsUsed())
    {
      continue;
    }

    pResident = &s_ResidentCharaModelFactoryList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  pResident->Initialize(pFileManager, pFileHeap, arcId);

  s_ReloadFunc = NULL;
}

/**
 * @brief 常駐化終了処理
 *
 * @param arcId GARCのID
 */
void CharaModelFactory::ResidentFinalize(u32 arcId)
{
  ResidentCharaModelFactory* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_CHARA_MODEL_FACTORY_COUNT; ++i)
  {
    if (s_ResidentCharaModelFactoryList[i].GetArcId() != arcId)
    {
      continue;
    }

    pResident = &s_ResidentCharaModelFactoryList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  pResident->Finalize();

  s_ReloadFunc = NULL;
}

/**
 * @brief 常習GARCリロード予約
 *
 * @param reloadFunc リロード用関数
 */
void CharaModelFactory::ResidentReloadRequest(ReloadFunc reloadFunc)
{
  s_ReloadFunc = reloadFunc;
}

/**
 * @brief コンストラクタ
 */
CharaModelFactory::CharaModelFactory() :
  m_ArcId(0),
  m_pFileManager(NULL),
  m_pFileHeap(NULL),
  m_pResourceUnitList(NULL),
  m_pNormalGarcFooter(NULL),
  m_pNormalGarcDyAnimOffsetList(NULL),
  m_pGetRandomFunc(NULL)
{
}

/**
 * @brief デストラクタ
 */
CharaModelFactory::~CharaModelFactory()
{
  this->Finalize();
}

/**
 * @brief 初期化
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcId GARCのID
 */
void CharaModelFactory::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
{
  // リロード
  if (s_ReloadFunc != NULL)
  {
    s_ReloadFunc();
    s_ReloadFunc = NULL;
  }

  // ファイルマネージャとヒープ登録
  m_pFileManager = pFileManager;
  m_pFileHeap = pFileHeap;

  // アーカイブ番号保存
  m_ArcId = arcId;

  // 常駐領域からフッター取得
  ResidentCharaModelFactory* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_CHARA_MODEL_FACTORY_COUNT; ++i)
  {
    if (s_ResidentCharaModelFactoryList[i].GetArcId() != arcId)
    {
      continue;
    }

    pResident = &s_ResidentCharaModelFactoryList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  m_pNormalGarcFooter = pResident->GetNormalGarcFooter();
  m_pNormalGarcDyAnimOffsetList = pResident->GetNormalGarcDyAnimOffsetList();

  // 含まれるファイル数分リソースユニットを作成
  m_pResourceUnitList = GFL_NEW_ARRAY(m_pFileHeap) CharaModelResourceUnit[m_pNormalGarcFooter->GetModelCount()];

  // アニメーションパック数を設定する
  for (u32 i = 0; i < m_pNormalGarcFooter->GetModelCount(); ++i)
  {
    u32 packCount = m_pNormalGarcDyAnimOffsetList->GetDyAnimOffset(i + 1) - m_pNormalGarcDyAnimOffsetList->GetDyAnimOffset(i);
    ++packCount; // 常駐パック
    m_pResourceUnitList[i].Initialize(m_pFileHeap, packCount);
  }
}

/**
 * @brief 初期化：非同期版
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcId GARCのID
 */
void CharaModelFactory::InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId)
{
  // 同期版と同じ
  this->Initialize(pFileManager, pFileHeap, arcId);
}

/**
 * @brief 非同期版の初期化完了待ち
 *
 * @return true:初期化完了 false:初期化中
 */
bool CharaModelFactory::WaitInitializeAsync()
{
  return true;
}

/**
 * @brief 終了処理
 */
void CharaModelFactory::Finalize()
{
  // リソースユニットがNULLならすでに終了処理は通っているはず
  if (m_pResourceUnitList == NULL)
  {
    return;
  }

  // 全リソース開放
  this->UnloadModelAll();

  // アニメーションパック終了処理
  for (u32 i = 0; i < m_pNormalGarcFooter->GetModelCount(); ++i)
  {
    m_pResourceUnitList[i].Finalize();
  }

  // リソースユニット削除
  GFL_SAFE_DELETE_ARRAY(m_pResourceUnitList);
}

/**
 * @brief データの同期読み込み
 *
 * @param charaId 読み込むキャラID
 */
void CharaModelFactory::LoadModelSync(u32 charaId)
{
  this->LoadModel_(false, charaId, m_pFileHeap);
}


/**
 * @brief データの設定
 *
 * @param charaId 読み込むキャラID
 * @param pData   静的データ
 */
void CharaModelFactory::SetModel(u32 charaId, void * pData)
{
  this->SetModel_( charaId, pData );
}

/**
 * @brief データの非同期読み込み開始
 *
 * @param charaId 読み込むキャラID
 */
void CharaModelFactory::LoadModelAsync(u32 charaId)
{
  this->LoadModel_(true, charaId, m_pFileHeap);
}

/**
 * @brief データの非同期読み込み開始
 *
 * @param pLoadHeap 読み込みヒープ
 * @param charaId 読み込むキャラID
 */
void CharaModelFactory::LoadModelAsync(gfl2::heap::HeapBase * pLoadHeap, u32 charaId)
{
  this->LoadModel_(true, charaId, pLoadHeap);
}

/**
 * @brief データの非同期読み込みの完了確認
 *
 * @param charaId 確認するキャラID
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool CharaModelFactory::IsModelLoaded(u32 charaId)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);
  if (pResourceUnit == NULL)
  {
    return false;
  }

  // 読み込み完了確認
  return pResourceUnit->IsStaticDataLoaded(m_pFileManager);
}

/**
 * @brief 読み込んだモデルデータをセットアップする
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param charaId セットアップするキャラID
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelFactory::SetupModel(gfl2::gfx::IGLAllocator* pGfxAllocator, u32 charaId)
{
  // 念のため読み込み完了チェック
  if (!this->IsModelLoaded(charaId))
  {
    return false;
  }

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  // モデルリソースノードセットアップ
  if (!pResourceUnit->SetupModelResourceNode(m_pFileManager, pGfxAllocator))
  {
    // 読み込み終わってないので生成できない
    return false;
  }

  // モーションコンフィグセットアップ
  if (!pResourceUnit->SetupMotionConfigResourceNode(m_pFileManager, pGfxAllocator))
  {
    // モーションコンフィグが無い？
    return false;
  }

  // アニメーションパックリストセットアップ
  if (!pResourceUnit->SetupAnimationPackList(m_pFileManager, pGfxAllocator, m_pFileHeap))
  {
    // アニメーションパックリストが無い？
    return false;
  }

  // 目パチモーションセットアップ
  if (!pResourceUnit->SetupEyeMotionResourceNode(m_pFileManager, pGfxAllocator))
  {
    // 目パチモーションが無い？
    return false;
  }

  // 口パクモーションセットアップ
  if (!pResourceUnit->SetupMouthMotionResourceNode(m_pFileManager, pGfxAllocator))
  {
    // 口パクモーションが無い？
    return false;
  }

  // 正常にセットアップ完了
  return true;
}

/**
 * @brief 読み込んだデータを開放する
 *
 * @param charaId 開放するキャラID
 */
void CharaModelFactory::UnloadModel(u32 charaId)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  pResourceUnit->UnloadStaticData();
}

/**
 * @brief 読み込んだデータを開放できるか確認
 *
 * @param charaId 開放するキャラID
 *
 * @return true: 解放できる, false: 解放できない
 */
bool CharaModelFactory::CanUnloadModel(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  return pResourceUnit->CanUnloadStaticData();
}

/**
 * @brief 読み込んだデータを全て開放する
 */
void CharaModelFactory::UnloadModelAll()
{
  for (u32 i = 0; i < m_pNormalGarcFooter->GetModelCount(); ++i)
  {
    this->UnloadModel(i);
  }
}

/**
 * @brief 読み込んだデータを全て開放できるか確認
 */
bool CharaModelFactory::CanUnloadModelAll() const
{
  for (u32 i = 0; i < m_pNormalGarcFooter->GetModelCount(); ++i)
  {
    if (!this->CanUnloadModel(i))
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief 読み込んだデータのモデルデータサイズ取得
 *
 * @param charaId 取得するキャラID
 *
 * @return データサイズ
 */
u32 CharaModelFactory::GetModelDataSize(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  return pResourceUnit->GetBinLinkerDataSize(m_pFileManager, BaseModelResourceUnit::BASE_BIN_LINKER_INDEX_MODEL);
}

/**
 * @brief 読み込んだデータのアニメーションデータサイズ取得
 *
 * @param charaId 取得するキャラID
 *
 * @return データサイズ
 */
u32 CharaModelFactory::GetAnimationDataSize(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  return 
    pResourceUnit->GetBinLinkerDataSize(m_pFileManager, BaseModelResourceUnit::BASE_BIN_LINKER_INDEX_ANIMATION) +
    pResourceUnit->GetBinLinkerDataSize(m_pFileManager, CharaModelResourceUnit::CHARA_BIN_LINKER_INDEX_MCNF) +
    pResourceUnit->GetBinLinkerDataSize(m_pFileManager, CharaModelResourceUnit::CHARA_BIN_LINKER_INDEX_EYE_ANIM) +
    pResourceUnit->GetBinLinkerDataSize(m_pFileManager, CharaModelResourceUnit::CHARA_BIN_LINKER_INDEX_MOUTH_ANIM);
}

/**
 * @brief 読み込んだデータの合計データサイズ取得
 *
 * @param charaId 取得するキャラID
 *
 * @return データサイズ
 */
u32 CharaModelFactory::GetTotalDataSize(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  u32 dataSize = 0;
  u32 binCount = pResourceUnit->GetBinLinkerDataCount(m_pFileManager);
  for (u32 binIndex = 0; binIndex < binCount; ++binIndex)
  {
    dataSize += pResourceUnit->GetBinLinkerDataSize(m_pFileManager, binIndex);
  }
  return dataSize;
}

/**
* @brief 読み込んだモデルデータの数を取得
*
* @return 読み込んだモデルデータの数
*/
u32 CharaModelFactory::GetModelLoadedCount( void )
{
  u32 count = 0;
  u32 modelIdMax = m_pNormalGarcFooter->GetModelCount();
  for( u32 i = 0; i < modelIdMax; i++ )
  {
    if( this->IsModelLoaded( i ) )
    {
      count++;
    }
  }

  return count;
}

/**
 * @brief 読み込んだデータからキャラモデルを作成する
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pCharaModel 作成するキャラモデルへのポインタ
 * @param charaId 作成するキャラID
 *
 * @return true: 正常にモデル生成完了, false: モデル生成失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelFactory::CreateModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, CharaModel* pCharaModel, u32 charaId)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  // モデルリソースノード取得
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceNode = pResourceUnit->GetModelResourceNode();
  if (pModelResourceNode == NULL)
  {
    // 読み込み終わってないので生成できない
    return false;
  }

  // モーションコンフィグ設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResourceNode = pResourceUnit->GetMotionConfigResourceNode();
  if (pMotionConfigResourceNode == NULL)
  {
    // モーションコンフィグが無い？
    return false;
  }

  // アニメーションパックリスト設定
  gfl2::animation::AnimationPackList* pAnimationPackList = pResourceUnit->GetAnimationPackList();
  if (pAnimationPackList == NULL)
  {
    // アニメーションパックリストが無い？
    return false;
  }

  // キャラモデル生成
  if (!pCharaModel->Create(pGfxAllocator, pAnimationHeap, pModelResourceNode, pMotionConfigResourceNode))
  {
    // キャラモデル生成失敗
    return false;
  }

  // 自動目パチ用乱数取得関数設定
  pCharaModel->SetupRandomFuc(m_pGetRandomFunc);

  // アニメーションパック設定
  pCharaModel->SetAnimationPackList(pAnimationPackList);

  // 目パチモーション設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pEyeMotionResourceNode = pResourceUnit->GetEyeMotionResourceNode();
  pCharaModel->SetEyeMotionResourceNode(pEyeMotionResourceNode);

  // 口パクモーション設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMouthMotionResourceNode = pResourceUnit->GetMouthMotionResourceNode();
  pCharaModel->SetMouthMotionResourceNode(pMouthMotionResourceNode);

  // 目パチ、口パク用のメモリを確定させる
  pCharaModel->AllocateEye();
  pCharaModel->AllocateMouth();

  // 作成成功
  return true;
}

/**
 * @brief 読み込んだデータから着せ替えモデルを作成する
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pDressUpModel 作成する着せ替えモデルへのポインタ
 * @param pResourceManager 着せ替えリソースマネージャーへのポインタ
 * @param charaId 作成するキャラID(着せ替えのベースモデルのキャラID)
 *
 * @return true: 正常にモデル生成完了, false: モデル生成失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelFactory::CreateDressUpModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, DressUpModel* pDressUpModel, DressUpModelResourceManager* pResourceManager, u32 charaId)
{
  return this->CreateDressUpModel(pGfxAllocator, pAnimationHeap, pDressUpModel, pResourceManager->GetSetupDressUpModelResourceManagerCore(), charaId);
}

/**
 * @brief 読み込んだデータから着せ替えモデルを作成する
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pAnimationHeap Animationライブラリ用のヒープ
 * @param pDressUpModel 作成する着せ替えモデルへのポインタ
 * @param pResourceManager 着せ替えリソースマネージャーへのポインタ
 * @param charaId 作成するキャラID(着せ替えのベースモデルのキャラID)
 *
 * @return true: 正常にモデル生成完了, false: モデル生成失敗、読み込み完了していないorリソースが壊れているなど
 */
bool CharaModelFactory::CreateDressUpModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, DressUpModel* pDressUpModel, DressUpModelResourceManagerCore* pResourceManager, u32 charaId)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  // モデルリソースノード取得
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceNode = pResourceUnit->GetModelResourceNode();
  if (pModelResourceNode == NULL)
  {
    // 読み込み終わってないので生成できない
    return false;
  }

  // モーションコンフィグ設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResourceNode = pResourceUnit->GetMotionConfigResourceNode();
  if (pMotionConfigResourceNode == NULL)
  {
    // モーションコンフィグが無い？
    return false;
  }

  // アニメーションパックリスト設定
  gfl2::animation::AnimationPackList* pAnimationPackList = pResourceUnit->GetAnimationPackList();
  if (pAnimationPackList == NULL)
  {
    // アニメーションパックリストが無い？
    return false;
  }

  // キャラモデル生成
  if (!pDressUpModel->Create(pGfxAllocator, pAnimationHeap, pResourceManager->m_pDressUpModelResourceUnitList, pModelResourceNode, pMotionConfigResourceNode))
  {
    // キャラモデル生成失敗
    return false;
  }

  // 自動目パチ用乱数取得関数設定
  pDressUpModel->SetupRandomFuc(m_pGetRandomFunc);

  // アニメーションパック設定
  pDressUpModel->SetAnimationPackList(pAnimationPackList);

  // 目パチモーション設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pEyeMotionResourceNode = pResourceUnit->GetEyeMotionResourceNode();
  pDressUpModel->SetEyeMotionResourceNode(pEyeMotionResourceNode);

  // 口パクモーション設定
  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMouthMotionResourceNode = pResourceUnit->GetMouthMotionResourceNode();
  pDressUpModel->SetMouthMotionResourceNode(pMouthMotionResourceNode);

  // 目パチ、口パク用のメモリを確定させる
  pDressUpModel->AllocateEye();
  pDressUpModel->AllocateMouth();

  // サンダル系の例外処理：靴下をlegsのテクスチャから参照
  if (pResourceManager->m_pDressUpModelResourceUnitList[DressUpModel::PARTS_MODEL_SHOES].IsItemOption(DressUpModelResourceUnit::OPTION_BIT_SANDAL))
  {
    BaseModel* pShoesModel = pDressUpModel->GetPartsModel(DressUpModel::PARTS_MODEL_SHOES);
    BaseModel* pLegsModel = pDressUpModel->GetPartsModel(DressUpModel::PARTS_MODEL_LEGS);

    if ((pShoesModel != NULL) && (pLegsModel != NULL))
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pShoesModelNode = pShoesModel->GetModelInstanceNode();
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pLegsModelNode = pLegsModel->GetModelInstanceNode();

      // legsから_legs_が含まれるテクスチャを検索
      const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pLegsTextureNode = NULL;
      for (u32 i = 0; i < pLegsModelNode->GetMaterialNum(); ++i)
      {
        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pLegsModelNode->GetMaterialInstanceNode(i);
        for (u32 j = 0; j < pMaterialNode->GetTextureNum(); ++j)
        {
          const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureNode = pMaterialNode->GetTextureResourceNode(j);

          if (pTextureNode == NULL)
          {
            continue;
          }

          if (::std::strstr(pTextureNode->GetName(), "_legs_") != NULL)
          {
            pLegsTextureNode = pTextureNode;
            break;
          }
        }

        if (pLegsTextureNode != NULL)
        {
          break;
        }
      }

      // shoesから_legs_が含まれるテクスチャを全て差し替え
      for (u32 i = 0; i < pShoesModelNode->GetMaterialNum(); ++i)
      {
        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* pMaterialNode = pShoesModelNode->GetMaterialInstanceNode(i);
        for (u32 j = 0; j < pMaterialNode->GetTextureNum(); ++j)
        {
          const gfl2::renderingengine::scenegraph::resource::TextureResourceNode* pTextureNode = pMaterialNode->GetTextureResourceNode(j);

          if (pTextureNode == NULL)
          {
            continue;
          }

          if (::std::strstr(pTextureNode->GetName(), "_legs_") != NULL)
          {
            pMaterialNode->SetTextureResourceNode(j, pLegsTextureNode);
          }
        }
      }
    }
  }

  // 島巡りの証のON/OFF
  pDressUpModel->SetBagCharmVisible(pResourceManager->m_HasBagCharm);

  // 作成成功
  return true;
}

/**
 * @brief アプリ用データの取得
 *
 * @param charaId 取得するキャラID
 *
 * @return アプリ用データを返す。まだ読み込まれていないならNULL
 */
const void* CharaModelFactory::GetApplicationData(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);

  return pResourceUnit->GetApplicationData(m_pFileManager);
}

/**
 * @brief 非常駐アニメーションパックの同期読み込み
 *
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void CharaModelFactory::LoadDynamicAnimationSync(u32 dynamicAnimationId)
{
  this->LoadDynamicAnimationSync(m_pFileHeap, dynamicAnimationId);
}

/**
 * @brief 非常駐アニメーションパックの同期読み込み
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void CharaModelFactory::LoadDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, u32 dynamicAnimationId)
{
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(packedDynamicAnimationId.charaId);
  u32 dynamicMotionOffset = m_pNormalGarcDyAnimOffsetList->GetDyAnimOffset(packedDynamicAnimationId.charaId);
  u32 datId = packedDynamicAnimationId.packId - 1 + dynamicMotionOffset;  // 0番は常駐用
  pResourceUnit->LoadDynamicAnimationDataSync(m_ArcId, datId, packedDynamicAnimationId.packId, pLoadHeap, m_pFileManager);
}

/**
 * @brief 非常駐アニメーションパックの非同期読み込み開始
 *
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void CharaModelFactory::LoadDynamicAnimationAsync(u32 dynamicAnimationId)
{
  this->LoadDynamicAnimationAsync(m_pFileHeap, dynamicAnimationId);
}

/**
 * @brief 非常駐アニメーションパックの非同期読み込み開始
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void CharaModelFactory::LoadDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, u32 dynamicAnimationId)
{
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(packedDynamicAnimationId.charaId);
  u32 dynamicMotionOffset = m_pNormalGarcDyAnimOffsetList->GetDyAnimOffset(packedDynamicAnimationId.charaId);
  u32 datId = packedDynamicAnimationId.packId - 1 + dynamicMotionOffset;  // 0番は常駐用
  pResourceUnit->LoadDynamicAnimationDataAsync(m_ArcId, datId, packedDynamicAnimationId.packId, pLoadHeap, m_pFileManager);
}

/**
 * @brief 非常駐アニメーションパックの非同期読み込みの完了確認
 *
 * @param dynamicAnimationId 確認する非常駐アニメーションID
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool CharaModelFactory::IsDynamicAnimationLoaded(u32 dynamicAnimationId)
{
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(packedDynamicAnimationId.charaId);
  return pResourceUnit->IsDynamicAnimationDataLoaded(packedDynamicAnimationId.packId, m_pFileManager);
}

/**
 * @brief 非常駐アニメーションパックのセットアップ
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pSetupHeap セットアップ用のヒープ
 * @param dynamicAnimationId セットアップする非常駐アニメーションID
 */
void CharaModelFactory::SetupDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, u32 dynamicAnimationId)
{
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(packedDynamicAnimationId.charaId);
  pResourceUnit->SetupDynamicAnimationData(packedDynamicAnimationId.packId, pGfxAllocator, pSetupHeap);
}

/**
 * @brief 読み込んだ非常駐アニメーションパックを開放する
 *
 * @param dynamicAnimationId 開放する非常駐アニメーションID
 */
void CharaModelFactory::UnloadDynamicAnimation(u32 dynamicAnimationId)
{
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(packedDynamicAnimationId.charaId);
  pResourceUnit->UnloadDynamicAnimationData(packedDynamicAnimationId.packId);
}

void CharaModelFactory::SetupRandomFunc(u32 (*pGetRandomFunc)(u32))
{
  m_pGetRandomFunc = pGetRandomFunc;
}

/**
 * @brief 注視用のジョイント数取得
 *
 * @param charaId 取得するキャラID
 *
 * @return 注目用ジョイント数
 */
u32 CharaModelFactory::GetModelData_InterestJointCount(u32 charaId) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);
  const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode* pMotionConfigResourceNode = 
    static_cast<const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode*>(pResourceUnit->GetMotionConfigResourceNode());

  GFL_ASSERT(pMotionConfigResourceNode != NULL);
  return pMotionConfigResourceNode->GetInterestCount();
}

/**
 * @brief 注目速度取得
 *
 * @param charaId 取得するキャラID
 * @param index インデックス
 *
 * @return 注目速度を返す
 */
f32 CharaModelFactory::GetModelData_InterestSpeed(u32 charaId, u32 index) const
{
  const CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);
  const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode* pMotionConfigResourceNode = 
    static_cast<const gfl2::renderingengine::scenegraph::resource::MotionConfigResourceNode*>(pResourceUnit->GetMotionConfigResourceNode());

  GFL_ASSERT(pMotionConfigResourceNode != NULL);
  GFL_ASSERT(this->GetModelData_InterestJointCount(charaId) > index);
  return pMotionConfigResourceNode->GetInterest(index)->Speed;
}

/**
 * @brief リソースユニット取得
 *
 * @param charaId キャラ番号
 *
 * @return 対応するリソースユニットを返す。無効なIDならNULL
 */
CharaModelResourceUnit* CharaModelFactory::GetResourceUnit_(u32 charaId)
{
  // 範囲チェック
  if (charaId >= m_pNormalGarcFooter->GetModelCount())
  {
    GFL_ASSERT(0);

    return NULL;
  }

  return &m_pResourceUnitList[charaId];
}

/**
 * @brief リソースユニット取得
 *
 * @param charaId キャラ番号
 *
 * @return 対応するリソースユニットを返す。無効なIDならNULL
 */
const CharaModelResourceUnit* CharaModelFactory::GetResourceUnit_(u32 charaId) const
{
  // 範囲チェック
  if (charaId >= m_pNormalGarcFooter->GetModelCount())
  {
    GFL_ASSERT(0);

    return NULL;
  }

  return &m_pResourceUnitList[charaId];
}

/**
 * @brief データの読み込み
 *
 * @param isAsync true:非同期読み込み false:同期読み込み
 * @param charaId 読み込むキャラID
 * @param pHeap   ヒープ
 */
void CharaModelFactory::LoadModel_(bool isAsync, u32 charaId, gfl2::heap::HeapBase* pHeap)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);
  if (pResourceUnit == NULL)
  {
    return;
  }

  // 同期読み込み
  u32 datId = charaId;
  pResourceUnit->LoadStaticData(isAsync, m_ArcId, datId, pHeap, m_pFileManager);
}


/**
 * @brief データの設定
 *
 * @param charaId キャラID
 * @param pData   設定するデータ
 */
void CharaModelFactory::SetModel_(u32 charaId, void* pData)
{
  CharaModelResourceUnit* pResourceUnit = this->GetResourceUnit_(charaId);
  if (pResourceUnit == NULL)
  {
    return;
  }

  // データの設定
  pResourceUnit->LoadStaticData(pData);
}

/**
 * @brief 常駐化初期化
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcIdList GARCのIDリスト
 */
void DressUpModelResourceManagerCore::ResidentInitialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
{
  ResidentDressUpModelResourceManager* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_DRESSUP_RESOURCE_MANAGER_COUNT; ++i)
  {
    if (s_ResidentDressUpResourceManagerList[i].IsUsed())
    {
      continue;
    }

    pResident = &s_ResidentDressUpResourceManagerList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  pResident->Initialize(pFileManager, pFileHeap, arcIdList);
}

/**
 * @brief 常駐化終了処理
 *
 * @param arcIdList GARCのIDリスト
 */
void DressUpModelResourceManagerCore::ResidentFinalize(s32* arcIdList)
{
  ResidentDressUpModelResourceManager* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_DRESSUP_RESOURCE_MANAGER_COUNT; ++i)
  {
    if (s_ResidentDressUpResourceManagerList[i].GetArcIdColor() != *arcIdList)
    {
      continue;
    }

    pResident = &s_ResidentDressUpResourceManagerList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  pResident->Finalize();
}

/**
 * @brief コンストラクタ
 */
DressUpModelResourceManagerCore::DressUpModelResourceManagerCore():
  m_ArcIdColor(0),
  m_ArcIdList(NULL),
  m_pGarcFooterList(NULL),
  m_pDressUpPartsList(NULL),
  m_pDressUpDyAnimOffsetList(NULL),
  m_pColorTableList(NULL),
  m_pColorTableExt(NULL),
  m_pFileManager(NULL),
  m_pFileHeap(NULL),
  m_pDressUpModelResourceUnitList(NULL),
  m_CurrentSetupSeq(0),
  m_CurrentSetupPartsIndex(0),
  m_CurrentSetupMaskIndex(0),
  m_CurrentSetupSubMaskIndex(0),
  m_CurrentSetupTexturePixel(0),
  m_HasBagCharm(false)
{
}

/**
 * @brief デストラクタ
 */
DressUpModelResourceManagerCore::~DressUpModelResourceManagerCore()
{
  this->Finalize();
}

/**
 * @brief 初期化
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcIdList GARCのIDリスト
 * @param isDataCheckMode false:通常モード, true:メモリ確保をや読み込みを一切やらないデータチェック用モード
 */
void DressUpModelResourceManagerCore::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList, bool isDataCheckMode)
{
  // ファイルマネージャとヒープ保存
  m_pFileManager = pFileManager;
  m_pFileHeap = pFileHeap;

  // フィールド用に常駐部分の初期化をSuspend()とResume()に分けた
  this->Suspend();
  // カラーのARC_IDだけはここで保存
  m_ArcIdColor = *arcIdList;
  this->Resume();

  GFL_ASSERT(m_ArcIdList != NULL);

  // データチェックだけならここで抜ける
  if (isDataCheckMode)
  {
    return;
  }

  // 全パーツセットアップ
  m_pDressUpModelResourceUnitList = GFL_NEW_ARRAY(m_pFileHeap) DressUpModelResourceUnit[DressUpModel::PARTS_MODEL_COUNT];
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }

    u32 garcDataCount = 0;
    // GARCのアーカイブ数読み取り
    {
      const gfl2::fs::ArcFile* pArcFile = m_pFileManager->GetArcFile(m_ArcIdList[i]);
      pArcFile->GetDataCount(&garcDataCount);
    }

    // 着せ替えモデルのリソースユニット作成
    u32 partsCount = m_pDressUpPartsList[i]->GetPartsCount();
    // 2タイプ持っているパーツ用の特殊対処
    if (HasDressUpParts2Type(static_cast<DressUpParam::ItemCategory>(i)))
    {
      partsCount *= 2;
    }
    u32 dyAnimCount = garcDataCount - (3 + partsCount);

    u32 dressUpPartsDynamicLimit = dyAnimCount / partsCount;
    ++dressUpPartsDynamicLimit; // 常駐パック
    m_pDressUpModelResourceUnitList[i].Initialize(m_pFileHeap, dressUpPartsDynamicLimit);
  }
}

/**
 * @brief 初期化：非同期版
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcIdList GARCのIDリスト
 */
void DressUpModelResourceManagerCore::InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
{
  // 同期版と同じ
  this->Initialize(pFileManager, pFileHeap, arcIdList);
}

/**
 * @brief 非同期版の初期化完了待ち
 *
 * @return true:初期化完了 false:初期化中
 */
bool DressUpModelResourceManagerCore::WaitInitializeAsync()
{
  return true;
}

/**
 * @brief 終了処理
 */
void DressUpModelResourceManagerCore::Finalize()
{
  // リソースユニットがNULLならすでに終了処理は通っているはず
  if (m_pDressUpModelResourceUnitList == NULL)
  {
    return;
  }

  // 全着せ替えリソース開放
  this->UnloadDressUpPartsAll();

  // 着せ替えモデルのリソースユニット終了処理
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    m_pDressUpModelResourceUnitList[i].Finalize();
  }

  // 着せ替えモデルのリソースユニット削除
  GFL_SAFE_DELETE_ARRAY(m_pDressUpModelResourceUnitList);
}

/**
 * @brief フィールド常駐化用の状態保存処理
 */
void DressUpModelResourceManagerCore::Suspend()
{
  m_ArcIdList = NULL;
  m_pGarcFooterList = NULL;
  m_pDressUpPartsList = NULL;
  m_pDressUpDyAnimOffsetList = NULL;
  m_pColorTableList = NULL;
  m_pColorTableExt = NULL;
}

/**
 * @brief フィールド常駐化用の状態復帰処理
 */
void DressUpModelResourceManagerCore::Resume()
{
  // 常駐している領域から必要なデータを取得
  ResidentDressUpModelResourceManager* pResident = NULL;

  for (u32 i = 0; i < RESIDENT_DRESSUP_RESOURCE_MANAGER_COUNT; ++i)
  {
    if (s_ResidentDressUpResourceManagerList[i].GetArcIdColor() != m_ArcIdColor)
    {
      continue;
    }

    pResident = &s_ResidentDressUpResourceManagerList[i];
    break;
  }

  GFL_ASSERT(pResident != NULL);
  if (pResident == NULL)
  {
    return;
  }

  m_ArcIdList = pResident->GetArcIdList();
  m_pGarcFooterList = pResident->GetGarcFooterList();
  m_pDressUpPartsList = pResident->GetDressUpPartsList();
  m_pDressUpDyAnimOffsetList = pResident->GetDressUpDyAnimOffsetList();
  m_pColorTableList = pResident->GetColorTableList();
  m_pColorTableExt = pResident->GetColorTableExt();
}

/**
 * @brief 有効な着せ替えの組み合わせか確認
 *
 * @param dressUpParam 確認する着せ替えパラメータ
 *
 * @return 有効な組み合わせならtrue, 無効ならfalse
 */
bool DressUpModelResourceManagerCore::CheckDressUpCombination(const DressUpParam& dressUpParam) const
{
  // 削除済みのアイテムが付けられていないかチェック
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (!IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
    {
      continue;
    }

    if (dressUpParam.itemParams[i] >= 0)
    {
      return false;
    }
  }

  // 全てのアイテムがインデックスオーバーしていないかチェック
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
    {
      continue;
    }

    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }

    GFL_ASSERT(m_pGarcFooterList != NULL);
    if (dressUpParam.itemParams[i] >= m_pGarcFooterList[i]->GetItemCount())
    {
      return false;
    }
  }

  // シチュエーション着替えチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_SITU] >= 0)
  {
    // FACEがあるかチェック
    if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_FACE] < 0)
    {
      return false;
    }

    // 腕輪以外何か付けていないかチェック
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      if (IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
      {
        continue;
      }

      if (i == DressUpModel::PARTS_MODEL_BNGL)
      {
        continue;
      }

      if (dressUpParam.itemParams[i] >= 0)
      {
        return false;
      }
    }

    // 正常
    return true;
  }

  // 通常着替えチェック
  
  // FACEがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_FACE] < 0)
  {
    return false;
  }

  // HAIRがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR] < 0)
  {
    return false;
  }

  // BAGがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_BAG] < 0)
  {
    return false;
  }

  // BOTTOMSがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_BOTTOMS] < 0)
  {
    return false;
  }

  // LEGSがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_LEGS] < 0)
  {
    return false;
  }

  // TOPSがあるかチェック
  if (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_TOPS] < 0)
  {
    return false;
  }

  // HATがあるなら帽子がかぶれるHAIRなのかチェック
  if ((dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAT] >= 0) && (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR] >= 0))
  {
    if (!this->CanDressUpHairWearHat(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR]))
    {
      return false;
    }
  }

  // @fix GFMCat[1448] 帽子とヘアアクセサリを同時に着けていないかチェック
  if ((dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAT] >= 0) && (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_ACCHAIR] >= 0))
  {
    return false;
  }

  // 男でリップが指定されていないかチェック
  if ((dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_MALE) && (dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] != 0))
  {
    return false;
  }

  // 正常
  return true;
}

/**
* @brief 有効な着せ替えの組み合わせか確認（ファインダースタジオ用）
*
* @param dressUpParam 確認する着せ替えパラメータ
*
* @return 有効な組み合わせならtrue, 無効ならfalse
*
* @note momiji追加 @fix GFMMCat[72] CheckDressUpCombination関数ではファインダースタジオのおいてバグが見つかったため関数を追加
*/
bool DressUpModelResourceManagerCore::CheckDressUpCombinationFinderStudio(const DressUpParam& dressUpParam) const
{
  // 削除済みのアイテムが付けられていないかチェック
  for(u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if(!IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
    {
      continue;
    }

    if(dressUpParam.itemParams[i] >= 0)
    {
      return false;
    }
  }

  // 全てのアイテムがインデックスオーバーしていないかチェック
  for(u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if(IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
    {
      continue;
    }

    if(dressUpParam.itemParams[i] < 0)
    {
      continue;
    }

    GFL_ASSERT(m_pGarcFooterList != NULL);
    if(dressUpParam.itemParams[i] >= m_pGarcFooterList[i]->GetItemCount())
    {
      return false;
    }
  }

  // シチュエーション着替えチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_SITU] >= 0)
  {
    // FACEがあるかチェック
    if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_FACE] < 0)
    {
      return false;
    }

    // 腕輪、顔、ライド衣装以外何か付けていないかチェック
    for(u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      if(IsDressUpPartsDeleted(static_cast<DressUpParam::ItemCategory>(i)))
      {
        continue;
      }

      // momoji修正
      if(i == DressUpModel::PARTS_MODEL_BNGL ||
        i == DressUpModel::PARTS_MODEL_FACE ||
        i == DressUpModel::PARTS_MODEL_SITU)
      {
        continue;
      }

      if(dressUpParam.itemParams[i] >= 0)
      {
        return false;
      }
    }

    // 正常
    return true;
  }

  // 通常着替えチェック

  // FACEがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_FACE] < 0)
  {
    return false;
  }

  // HAIRがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR] < 0)
  {
    return false;
  }

  // BAGがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_BAG] < 0)
  {
    return false;
  }

  // BOTTOMSがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_BOTTOMS] < 0)
  {
    return false;
  }

  // LEGSがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_LEGS] < 0)
  {
    return false;
  }

  // TOPSがあるかチェック
  if(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_TOPS] < 0)
  {
    return false;
  }

  // HATがあるなら帽子がかぶれるHAIRなのかチェック
  if((dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAT] >= 0) && (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR] >= 0))
  {
    if(!this->CanDressUpHairWearHat(dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAIR]))
    {
      return false;
    }
  }

  // @fix GFMCat[1448] 帽子とヘアアクセサリを同時に着けていないかチェック
  if((dressUpParam.itemParams[DressUpModel::PARTS_MODEL_HAT] >= 0) && (dressUpParam.itemParams[DressUpModel::PARTS_MODEL_ACCHAIR] >= 0))
  {
    return false;
  }

  // 男でリップが指定されていないかチェック
  if((dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX] == DressUpParam::SEX_MALE) && (dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] != 0))
  {
    return false;
  }

  // 正常
  return true;
}

/**
 * @brief 着せ替えパーツの同期読み込み
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 */
void DressUpModelResourceManagerCore::LoadDressUpPartsSync(const DressUpParam& dressUpParam)
{
  this->LoadDressUpParts_(false, dressUpParam);
}

/**
 * @brief 着せ替えパーツの非同期読み込み開始
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 */
void DressUpModelResourceManagerCore::LoadDressUpPartsAsync(const DressUpParam& dressUpParam)
{
  this->LoadDressUpParts_(true, dressUpParam);
}

/**
 * @brief 着せ替えパーツの非同期読み込みの完了確認
 *
 * @param dressUpParam 読み込み確認する着せ替えパラメータ
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool DressUpModelResourceManagerCore::IsDressUpPartsLoaded(const DressUpParam& dressUpParam)
{
  // 読み込みを共通化するためにテーブルを変換
  s16 datIdList[DressUpModel::PARTS_MODEL_COUNT];
  this->MakeDressUpPartsDatIdList_(datIdList, dressUpParam);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    // 読み込んだ着せ替えアイテム番号と同じか確認
    if (pDressUpModelResourceUnit->GetItemIndex() != dressUpParam.itemParams[i])
    {
      return false;
    }

    // ０以下は無効な値
    if (datIdList[i] < 0)
    {
      continue;
    }

    if (!pDressUpModelResourceUnit->IsStaticDataLoaded(m_pFileManager))
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief 着せ替えパーツの読み込み
 *
 * @param isAsync true:非同期読み込み false:同期読み込み
 * @param dressUpParam 読み込む着せ替えパラメータ
 */
void DressUpModelResourceManagerCore::LoadDressUpParts_(bool isAsync, const DressUpParam& dressUpParam)
{
  GFL_ASSERT(m_ArcIdList != NULL);

  // 読み込みを共通化するためにテーブルを変換
  s16 datIdList[DressUpModel::PARTS_MODEL_COUNT];
  this->MakeDressUpPartsDatIdList_(datIdList, dressUpParam);

  // モデルデータ読み込み
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    // ０以下は無効な値
    if (datIdList[i] < 0)
    {
      continue;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    pDressUpModelResourceUnit->LoadStaticData(isAsync, m_ArcIdList[i], datIdList[i], m_pFileHeap, m_pFileManager);

    // 読み込んだ着せ替えアイテム番号を保存
    pDressUpModelResourceUnit->SetItemIndex(dressUpParam.itemParams[i]);
  }

  // 島巡りの証
  this->m_HasBagCharm = dressUpParam.hasBagCharm;
}

/**
 * @brief 読み込んだ着せ替えパーツを開放する
 *
 * @param dressUpParam 開放する着せ替えパーツ
 */
void DressUpModelResourceManagerCore::UnloadDressUpParts(const DressUpParam& dressUpParam)
{
  // 個別に解放は対応していないので全パーツを解放
  this->UnloadDressUpPartsAll();
}

/**
 * @brief 読み込んだ着せ替えパーツを開放できるか確認
 *
 * @param dressUpParam 開放する着せ替えパーツ
 *
 * @return true:解放できる false:解放できない
 */
bool DressUpModelResourceManagerCore::CanUnloadDressUpParts(const DressUpParam& dressUpParam) const
{
  // 個別に解放は対応していないので全パーツを確認
  return this->CanUnloadDressUpPartsAll();
}

/**
 * @brief 読み込んだ着せ替えパーツを全て開放する
 */
void DressUpModelResourceManagerCore::UnloadDressUpPartsAll()
{
  // データ開放
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    pDressUpModelResourceUnit->UnloadStaticData();
    pDressUpModelResourceUnit->SetItemIndex(-1);
  }
  m_CurrentSetupSeq = 0;
}

/**
 * @brief 読み込んだ着せ替えパーツを全て開放できるか確認
 *
 * @return true:解放できる false:解放できない
 */
bool DressUpModelResourceManagerCore::CanUnloadDressUpPartsAll() const
{
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    if (!pDressUpModelResourceUnit->CanUnloadStaticData())
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief 読み込んだ着せ替えパーツをセットアップする
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool DressUpModelResourceManagerCore::SetupDressUpParts(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  this->SetupDressUpPartsAsync(pGfxAllocator, dressUpParam);
  while (!this->WaitSetupDressUpPartsAsync(pGfxAllocator, dressUpParam))
  {
    // 終了待ち
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  return true;
}

/**
 * @brief 読み込んだ着せ替えパーツをセットアップする
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool DressUpModelResourceManagerCore::SetupDressUpPartsCore(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam, u32 partsModelIndex)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  DressUpParamWrapper wp(m_pGarcFooterList, &dressUpParam);

  // データセットアップ
  for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    // 読み込んだ着せ替えアイテム番号と違うなら落としてしまう
    GFL_ASSERT(pDressUpModelResourceUnit->GetItemIndex() == dressUpParam.itemParams[i]);

    // オプション設定
    pDressUpModelResourceUnit->SetItemOptionBit(0);
    s16 itemIndex = wp.GetItemIndex(static_cast<DressUpParam::ItemCategory>(i));
    if (itemIndex < 0)
    {
      continue;
    }
    u8 optionBit = m_pGarcFooterList[i]->GetOptionBit(itemIndex);
    pDressUpModelResourceUnit->SetItemOptionBit(optionBit);

    // アニメーションパックリストセットアップ
    if (i != DressUpParam::ITEM_CATEGORY_TOPS) // トップスはアニメーション生成しない
    {
      pDressUpModelResourceUnit->SetupAnimationPackList(m_pFileManager, pGfxAllocator, m_pFileHeap);
    }

    bool hasAnimation = false;
    gfl2::animation::AnimationPackList* pAnimationPackList = pDressUpModelResourceUnit->GetAnimationPackList();
    gfl2::animation::AnimationPack* pAnimationPack = pAnimationPackList->GetAnimationPack(0);
    if (pAnimationPack != NULL)
    {
      for (u32 j = 0; j < pAnimationPack->GetResourceCount(); ++j)
      {
        if (pAnimationPack->GetResourceNode(j) != NULL)
        {
          hasAnimation = true;
          break;
        }
      }
    }

    static const gfl2::renderingengine::scenegraph::resource::IResourceData::Type sc_resType("gfmodel");
    gfl2::renderingengine::scenegraph::resource::IResourceNodeFactory* pResourceNodeFactory = gfl2::renderingengine::scenegraph::resource::ResourceManager::GetResourceNodeFactory(sc_resType);
    gfl2::renderingengine::scenegraph::resource::GfMdlResourceNodeFactory* pGfMdlFactory = static_cast<gfl2::renderingengine::scenegraph::resource::GfMdlResourceNodeFactory*>(pResourceNodeFactory);

    // モデルリソースノードのセットアップ
    pGfMdlFactory->SetCreateJointFlag(hasAnimation); // アニメーションが無いならジョイントを生成しない
    bool resultSetup = pDressUpModelResourceUnit->SetupModelResourceNode(m_pFileManager, pGfxAllocator);
    pGfMdlFactory->SetCreateJointFlag(true); // ジョイント生成フラグを戻す

    if (!resultSetup)
    {
      // 読み込まれていない
      GFL_ASSERT(0);
      return false;
    }

    // 着せ替えパーツのセットアップ
    pDressUpModelResourceUnit->SetupDressUpParts(m_pFileManager, pGfxAllocator);
  }

  return true;
}

/**
 * @brief 読み込んだ着せ替えパーツの色替えする
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 * @param partsModelIndex セットアップするパーツモデル番号
 * @param maskIndex セットアップするマスク番号
 * @param submaskIndex セットアップするサブマスク番号
 * @param pTexturePixel セットアップするテクスチャのピクセル
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool DressUpModelResourceManagerCore::SetupDressUpPartsChangeTextureColor(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam, u32 partsModelIndex, u32* pMaskIndex, u32* pSubmaskIndex, u32* pTexturePixel)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  GFL_ASSERT(m_pColorTableList != NULL);
  GFL_ASSERT(m_pColorTableExt != NULL);
  DressUpParamWrapper wp(m_pGarcFooterList, &dressUpParam);
  bool hasLipColor = (wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR) != 0); 

  gfl2::math::Vector3 colorTable[] =
  {
    m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_SKIN]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_BODY_COLOR))->GetRgb(),  // 肌
    gfl2::math::Vector3(),                                                                                                                                        // 服
    m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_HAIR]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_HAIR_COLOR))->GetRgb(),  // 髪
    m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_EYE]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_EYE_COLOR))->GetRgb(),    // 目
    m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_EYEBLOW]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_HAIR_COLOR))->GetRgb(), // 眉
    // リップはデフォルトは肌の色と同じ
    (hasLipColor)
      ? m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_LIP]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR))->GetRgb() // リップ
      : m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_SKIN]->GetColorParam(wp.GetBodyIndex(DressUpParam::BODY_CATEGORY_BODY_COLOR))->GetRgb(), // 肌
  };
  gfl2::math::Vector3 clothTable[DressUpParam::ITEM_CATEGORY_COUNT];
  for (u32 i = DressUpParam::ITEM_CATEGORY_ACCBADGE; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }

    // アイテムインデックス
    s16 itemIndex = wp.GetItemIndex(static_cast<DressUpParam::ItemCategory>(i));
    if (itemIndex < 0)
    {
      continue;
    }

    // カラータイプとインデックス取得
    s16 colorType = m_pGarcFooterList[i]->GetColorType(itemIndex);
    if (colorType < 0)
    {
      continue;
    }

    s16 colorIndex = m_pGarcFooterList[i]->GetColorIndex(itemIndex);

    // 例外色のチェック
    if (colorType >= DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT)
    {
      const ColorParamExt* pExt = m_pColorTableExt->GetColorParam(colorIndex);
      clothTable[i] = pExt->param.GetRgb();
    }
    else
    {
      clothTable[i] = m_pColorTableList[colorType]->GetColorParam(colorIndex)->GetRgb();
    }
  }

  // 色替え
  {
    if (m_ArcIdList[partsModelIndex] < 0)
    {
      return false;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[partsModelIndex];

    // 色変え
    if (partsModelIndex == DressUpModel::PARTS_MODEL_FACE)
    {
      // face専用処理
      bool result = false;
      do {
        bool complete = false;
        result = pDressUpModelResourceUnit->ChangeTextureColorFace(colorTable, hasLipColor, (DressUpModelResourceUnit::SubMaskTextureId)*pSubmaskIndex, pTexturePixel, &complete);

        // 次のサブマスク
        if (complete)
        {
          *pTexturePixel = 0;

          ++(*pSubmaskIndex);
          if (*pSubmaskIndex >= DressUpModelResourceUnit::SUBMASK_TEXTURE_ID_COUNT)
          {
            *pSubmaskIndex = 0;

            // 次のマスク
            *pMaskIndex = DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT;
            return result;
          }
        }

        // 色替え行われたら抜ける
      } while (!result);

      return true;
    }
    else
    {
      // face以外の処理
      colorTable[DressUpModelResourceUnit::MASK_TEXTURE_ID_CLOTH] = clothTable[partsModelIndex];
      bool result = false;
      do {
        bool complete = false;
        result = pDressUpModelResourceUnit->ChangeTextureColor((DressUpModelResourceUnit::MaskTextureId)*pMaskIndex, (DressUpModelResourceUnit::SubMaskTextureId)*pSubmaskIndex, colorTable[*pMaskIndex], pTexturePixel, &complete);

        // 次のサブマスク
        if (complete)
        {
          *pTexturePixel = 0;

          ++(*pSubmaskIndex);
          if (*pSubmaskIndex >= DressUpModelResourceUnit::SUBMASK_TEXTURE_ID_COUNT)
          {
            *pSubmaskIndex = 0;

            // 次のマスク
            ++(*pMaskIndex);
            if (*pMaskIndex >= DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT)
            {
              return result;
            }
          }
        }

        // 色替え行われたら抜ける
      } while (!result);

      return true;
    }
  }

}

/**
 * @brief 読み込んだ着せ替えパーツをセットアップする：擬似非同期版
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
void DressUpModelResourceManagerCore::SetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  // 最初からスタート
  m_CurrentSetupSeq = 0;
  m_CurrentSetupPartsIndex = 0;
  m_CurrentSetupMaskIndex = 0;
  m_CurrentSetupSubMaskIndex = 0;
  m_CurrentSetupTexturePixel = 0;
}

/**
 * @brief 着せ替えパーツのセットアップ完了を待つ：擬似非同期版
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: セットアップ完了, false: セットアップ中
 */
bool DressUpModelResourceManagerCore::WaitSetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  switch (m_CurrentSetupSeq)
  {
  case DRESSUP_SETUP_SEQ_CREATE_PARTS: // パーツ作成
    {
      if (!this->SetupDressUpPartsCore(pGfxAllocator, dressUpParam, 0))
      {
        // ここで失敗するのはおかしい、メモリ破壊？読み込み中？
        GFL_ASSERT(0);
      }

      // 次へ
      ++m_CurrentSetupSeq;
    }
    break;

  case DRESSUP_SETUP_SEQ_CHANGE_COLOR: // 色替え
    {
      // 空パーツ判定
      while (m_ArcIdList[m_CurrentSetupPartsIndex] < 0)
      {
        // 次のパーツへ
        ++m_CurrentSetupPartsIndex;
      }

      // 色替え
      bool result = false;
      do {
        result = this->SetupDressUpPartsChangeTextureColor(pGfxAllocator, dressUpParam, m_CurrentSetupPartsIndex, &m_CurrentSetupMaskIndex, &m_CurrentSetupSubMaskIndex, &m_CurrentSetupTexturePixel);

        // m_CurrentSetupMaskIndexは上の関数でインクリメントされる
        if (m_CurrentSetupMaskIndex >= DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT)
        {
          // 次のパーツへ
          ++m_CurrentSetupPartsIndex;
          m_CurrentSetupMaskIndex = 0;
          m_CurrentSetupSubMaskIndex = 0;
          m_CurrentSetupTexturePixel = 0;
          if (m_CurrentSetupPartsIndex >= DressUpModel::PARTS_MODEL_COUNT)
          {
            // セットアップ完了
            ++m_CurrentSetupSeq;
            return true;
          }

          // 空パーツ判定
          while (m_ArcIdList[m_CurrentSetupPartsIndex] < 0)
          {
            // 次のパーツへ
            ++m_CurrentSetupPartsIndex;
          }
        }

        // 1フレームに一回だけテクスチャを処理する
      } while (!result);

      // セットアップ継続
      return false;
    }

  case DRESSUP_SETUP_SEQ_END: // セットアップ完了
    {
      // セットアップ完了
      return true;
    }
  }

  // セットアップ継続
  return false;
}

/**
 * @brief 着せ替えパーツのセットアップが完了済みか確認
 *
 * @param dressUpParam 確認する着せ替えパラメータ
 *
 * @return true: セットアップ完了, false: セットアップ中
 */
bool DressUpModelResourceManagerCore::IsSetupDressUpParts(const DressUpParam& dressUpParam)
{
  // 読み込み済みか確認
  if (!this->IsDressUpPartsLoaded(dressUpParam))
  {
    return false;
  }

  // セットアップが完了しているか確認
  if (m_CurrentSetupSeq < DRESSUP_SETUP_SEQ_END)
  {
    return false;
  }

  return true;
}

/**
 * @brief 読み込んだ着せ替えデータのモデルデータサイズ取得
 *
 * @param dressUpParam データサイズを取得する着せ替えパラメータ
 *
 * @return データサイズ
 */
u32 DressUpModelResourceManagerCore::GetDressUpModelDataSize(const DressUpParam& dressUpParam) const
{
  u32 dataSize = 0;
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    dataSize += pDressUpModelResourceUnit->GetBinLinkerDataSize(m_pFileManager, BaseModelResourceUnit::BASE_BIN_LINKER_INDEX_MODEL);

    // マスクテクスチャ
    u32 binCount = pDressUpModelResourceUnit->GetBinLinkerDataCount(m_pFileManager);
    for (u32 binIndex = DressUpModelResourceUnit::DRESSUP_BIN_LINKER_INDEX_MASK; binIndex < binCount; ++binIndex)
    {
      dataSize += pDressUpModelResourceUnit->GetBinLinkerDataSize(m_pFileManager, binIndex);
    }
  }
  return dataSize;
}

/**
 * @brief 読み込んだ着せ替えデータのアニメーションデータサイズ取得
 *
 * @param dressUpParam データサイズを取得する着せ替えパラメータ
 *
 * @return データサイズ
 */
u32 DressUpModelResourceManagerCore::GetDressUpAnimationDataSize(const DressUpParam& dressUpParam) const
{
  u32 dataSize = 0;
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    dataSize += pDressUpModelResourceUnit->GetBinLinkerDataSize(m_pFileManager, BaseModelResourceUnit::BASE_BIN_LINKER_INDEX_ANIMATION);
  }
  return dataSize;
}

/**
 * @brief 読み込んだ着せ替えデータの合計データサイズ取得
 *
 * @param dressUpParam データサイズを取得する着せ替えパラメータ
 *
 * @return データサイズ
 */
u32 DressUpModelResourceManagerCore::GetDressUpTotalDataSize(const DressUpParam& dressUpParam) const
{
  u32 dataSize = 0;
  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];

    u32 binCount = pDressUpModelResourceUnit->GetBinLinkerDataCount(m_pFileManager);
    for (u32 binIndex = 0; binIndex < binCount; ++binIndex)
    {
      dataSize += pDressUpModelResourceUnit->GetBinLinkerDataSize(m_pFileManager, binIndex);
    }
  }
  return dataSize;
}

/**
 * @brief 着せ替えパーツのデータIDリストを作成
 *
 * @param pDatIdList 書き込み先のデータIDリスト
 * @param dressUpParam 読み込む着せ替えパラメータ
 *
 * @return 着せ替えパーツのデータIDリストを返す
 */
void DressUpModelResourceManagerCore::MakeDressUpPartsDatIdList_(s16* pDatIdList, const DressUpParam& dressUpParam)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  DressUpParamWrapper wp(m_pGarcFooterList, &dressUpParam);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i){ pDatIdList[i] = -1; }

  // アイテム
  for (u32 i = 0; i < DressUpParam::ITEM_CATEGORY_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }

    pDatIdList[i] = wp.GetModelIndex(static_cast<DressUpParam::ItemCategory>(i));
  }
}

/**
 * @brief 着せ替え非常駐アニメーションパックの同期読み込み
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::LoadDressUpDynamicAnimationSync(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  this->LoadDressUpDynamicAnimationSync(m_pFileHeap, dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの同期読み込み
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::LoadDressUpDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  GFL_ASSERT(m_pDressUpDyAnimOffsetList != NULL);
  DressUpParamWrapper wp(m_pGarcFooterList, &dressUpParam);
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    if (m_pDressUpDyAnimOffsetList[i] == NULL)
    {
      continue;
    }
    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }
    // トップスは読み込まない
    if (i == DressUpModel::PARTS_MODEL_TOPS)
    {
      continue;
    }

    s16 modelIndex = wp.GetModelIndex(static_cast<DressUpParam::ItemCategory>(i));
    u32 dyAnimOffset = m_pDressUpDyAnimOffsetList[i]->GetDyAnimOffset(modelIndex);
    u32 datId = packedDynamicAnimationId.packId - 1 + dyAnimOffset;  // 0番は常駐用

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    pDressUpModelResourceUnit->LoadDynamicAnimationDataSync(m_ArcIdList[i], datId, packedDynamicAnimationId.packId, pLoadHeap, m_pFileManager);
  }
}

/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::LoadDressUpDynamicAnimationAsync(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  this->LoadDressUpDynamicAnimationAsync(m_pFileHeap, dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::LoadDressUpDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  GFL_ASSERT(m_pDressUpDyAnimOffsetList != NULL);
  DressUpParamWrapper wp(m_pGarcFooterList, &dressUpParam);
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    if (m_pDressUpDyAnimOffsetList[i] == NULL)
    {
      continue;
    }
    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }
    // トップスは読み込まない
    if (i == DressUpModel::PARTS_MODEL_TOPS)
    {
      continue;
    }

    s16 modelIndex = wp.GetModelIndex(static_cast<DressUpParam::ItemCategory>(i));
    u32 dyAnimOffset = m_pDressUpDyAnimOffsetList[i]->GetDyAnimOffset(modelIndex);
    u32 datId = packedDynamicAnimationId.packId - 1 + dyAnimOffset;  // 0番は常駐用

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    pDressUpModelResourceUnit->LoadDynamicAnimationDataAsync(m_ArcIdList[i], datId, packedDynamicAnimationId.packId, pLoadHeap, m_pFileManager);
  }
}


/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込みの完了確認
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 確認する非常駐アニメーションID
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool DressUpModelResourceManagerCore::IsDressUpDynamicAnimationLoaded(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pDressUpDyAnimOffsetList != NULL);
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    if (m_pDressUpDyAnimOffsetList[i] == NULL)
    {
      continue;
    }
    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    if (!pDressUpModelResourceUnit->IsDynamicAnimationDataLoaded(packedDynamicAnimationId.packId, m_pFileManager))
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief 着せ替え読み込んだ非常駐アニメーションパックを開放する
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 開放する非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::UnloadDressUpDynamicAnimation(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pDressUpDyAnimOffsetList != NULL);
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    if (m_pDressUpDyAnimOffsetList[i] == NULL)
    {
      continue;
    }
    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    pDressUpModelResourceUnit->UnloadDynamicAnimationData(packedDynamicAnimationId.packId);
  }
}

/**
 * @brief 着せ替え非常駐アニメーションパックのセットアップ
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pSetupHeap セットアップ用のヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId セットアップする非常駐アニメーションID
 */
void DressUpModelResourceManagerCore::SetupDressUpDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pDressUpDyAnimOffsetList != NULL);
  gfl2::animation::PackedDynamicAnimationId packedDynamicAnimationId(dynamicAnimationId);

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    if (m_pDressUpDyAnimOffsetList[i] == NULL)
    {
      continue;
    }
    if (dressUpParam.itemParams[i] < 0)
    {
      continue;
    }

    DressUpModelResourceUnit* pDressUpModelResourceUnit = &m_pDressUpModelResourceUnitList[i];
    pDressUpModelResourceUnit->SetupDynamicAnimationData(packedDynamicAnimationId.packId, pGfxAllocator, pSetupHeap);
  }
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：肌
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorSkin(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_SKIN]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：髪
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorHair(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_HAIR]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：服
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorCloth(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_CLOTH]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：眉
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorEyeblow(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_EYEBLOW]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：目
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorEye(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_EYE]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えパーツのRGBを0.0～1.0で取得：リップ
 */
gfl2::math::Vector3 DressUpModelResourceManagerCore::GetDressUpColorLip(u32 index) const
{
  GFL_ASSERT(m_pColorTableList != NULL);
  return m_pColorTableList[DressUpModelResourceUnit::MASK_TEXTURE_ID_LIP]->GetColorParam(index)->GetRgb();
}

/**
 * @brief 着せ替えアイテム数取得
 */
u32 DressUpModelResourceManagerCore::GetDressUpItemCount(DressUpParam::ItemCategory itemCategory) const
{
  GFL_ASSERT(m_ArcIdList != NULL);
  if (m_ArcIdList[itemCategory] < 0)
  {
    return 0;
  }

  GFL_ASSERT(m_pGarcFooterList != NULL);
  u32 itemCount = m_pGarcFooterList[itemCategory]->GetItemCount();  

  // 2タイプ持っているパーツ用の特殊対処
  if (HasDressUpParts2Type(itemCategory))
  {
    itemCount /= 2;
  }

  return itemCount;
}

/**
 * @brief 着せ替えパーツ数取得
 */
u32 DressUpModelResourceManagerCore::GetDressUpPartsCount(DressUpParam::ItemCategory itemCategory) const
{
  GFL_ASSERT(m_ArcIdList != NULL);
  if (m_ArcIdList[itemCategory] < 0)
  {
    return 0;
  }

  GFL_ASSERT(m_pDressUpPartsList != NULL);
  return m_pDressUpPartsList[itemCategory]->GetPartsCount();
}

/**
 * @brief アイテムインデックスからパーツインデックス取得
 */
u32 DressUpModelResourceManagerCore::GetDressUpPartsIndex(DressUpParam::ItemCategory itemCategory, s32 itemIndex) const
{
  // 2タイプ持っているパーツ用の特殊対処
  if (HasDressUpParts2Type(itemCategory))
  {
    itemIndex *= 2;
  }

  GFL_ASSERT(itemIndex >= 0);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  return m_pGarcFooterList[itemCategory]->GetPartsIndex(itemIndex);
}

/**
 * @brief アイテムインデックスからカラーインデックス取得
 */
s32 DressUpModelResourceManagerCore::GetDressUpColorIndex(DressUpParam::ItemCategory itemCategory, s32 itemIndex) const
{
  // 2タイプ持っているパーツ用の特殊対処
  if (HasDressUpParts2Type(itemCategory))
  {
    itemIndex *= 2;
  }

  GFL_ASSERT(itemIndex >= 0);
  GFL_ASSERT(m_pGarcFooterList != NULL);
  s16 colorType = m_pGarcFooterList[itemCategory]->GetColorType(itemIndex);
  s16 colorIndex = m_pGarcFooterList[itemCategory]->GetColorIndex(itemIndex);

  if (colorType < DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT)
  {
    return colorIndex;
  }

  // 例外の色
  GFL_ASSERT(m_pColorTableExt != NULL);
  const ColorParamExt* pExt = m_pColorTableExt->GetColorParam(colorIndex);
  return pExt->changeFromIndex;
}

/**
 * @brief パーツの有効なカラービットを取得
 */
u64 DressUpModelResourceManagerCore::GetDressUpPartsColorEnableBit(DressUpParam::ItemCategory itemCategory, u32 partsIndex) const
{
  GFL_ASSERT(m_pDressUpPartsList != NULL);
  return m_pDressUpPartsList[itemCategory]->GetColorBit(partsIndex);
}

/**
 * @brief パーツの有効なカラーか確認
 */
bool DressUpModelResourceManagerCore::IsDressUpPartsColorEnabled(DressUpParam::ItemCategory itemCategory, u32 partsIndex, u32 colorIndex) const
{
  u64 colorBit = this->GetDressUpPartsColorEnableBit(itemCategory, partsIndex);
  return (colorBit & (1ULL << colorIndex)) != 0;
}

/**
 * @brief パーツインデックスと色からアイテムインデックスを取得
 */
s32 DressUpModelResourceManagerCore::GetDressUpItemIndex(DressUpParam::ItemCategory itemCategory, u32 partsIndex, u32 colorIndex) const
{
  // 対応するカラーか確認
  GFL_ASSERT(m_pDressUpPartsList != NULL);
  u64 colorBit = m_pDressUpPartsList[itemCategory]->GetColorBit(partsIndex);
  if (!(colorBit & (1ULL << colorIndex)))
  {
    return -1;
  }

  u32 step = 1;
  // 2タイプ持っているパーツ用の特殊対処
  if (HasDressUpParts2Type(itemCategory))
  {
    step = 2;
  }

  // アイテムを探す
  GFL_ASSERT(m_pGarcFooterList != NULL);
  GFL_ASSERT(m_pColorTableExt != NULL);
  const DressUpGarcFooter* pGarcFooter = m_pGarcFooterList[itemCategory];
  for (u32 i = 0; i < pGarcFooter->GetItemCount(); i += step)
  {
    // パーツインデックス確認
    if (pGarcFooter->GetPartsIndex(i) != partsIndex)
    {
      continue;
    }

    // カラータイプとインデックス取得
    s16 colorType = pGarcFooter->GetColorType(i);
    if (colorType < 0)
    {
      continue;
    }

    // 例外色のチェック
    if (colorType >= DressUpModelResourceUnit::MASK_TEXTURE_ID_COUNT)
    {
      s16 extColorIndex = pGarcFooter->GetColorIndex(i);
      const ColorParamExt* pExt = m_pColorTableExt->GetColorParam(extColorIndex);

      if (pExt->changeFromIndex != colorIndex)
      {
        continue;
      }
    }
    else
    {
      if ((u32)pGarcFooter->GetColorIndex(i) != colorIndex)
      {
        continue;
      }
    }

    // 見つけた
    return i / step;
  }

  // 見つからない？
  return -1;
}

/**
 * @brief パーツインデックスからアイテムインデックスを取得：色はデフォルトのものを自動で選択
 */
s32 DressUpModelResourceManagerCore::GetDressUpItemIndexDefault(DressUpParam::ItemCategory itemCategory, u32 partsIndex) const
{
  u32 step = 1;
  // 2タイプ持っているパーツ用の特殊対処
  if (HasDressUpParts2Type(itemCategory))
  {
    step = 2;
  }

  // アイテムを探す
  GFL_ASSERT(m_pGarcFooterList != NULL);
  const DressUpGarcFooter* pGarcFooter = m_pGarcFooterList[itemCategory];
  for (u32 i = 0; i < pGarcFooter->GetItemCount(); i += step)
  {
    // パーツインデックス確認
    if (pGarcFooter->GetPartsIndex(i) != partsIndex)
    {
      continue;
    }

    // 見つけた
    return i / step;
  }

  // 見つからない？
  return -1;
}

/**
 * @brief 帽子をかぶることができる髪形か確認
 */
bool DressUpModelResourceManagerCore::CanDressUpHairWearHat(s32 hairItemIndex) const
{
  // 髪がないならOKにする
  if (hairItemIndex < 0)
  {
    return true;
  }

  GFL_ASSERT(m_pGarcFooterList != NULL);
  u8 optionBit = m_pGarcFooterList[DressUpParam::ITEM_CATEGORY_HAIR_STYLE]->GetOptionBit(hairItemIndex * 2);
  return !(optionBit & (1 << DressUpModelResourceUnit::OPTION_BIT_NO_HAT));
}

/**
 * @brief 最大のメモリサイズの着せ替え組み合わせ取得
 *
 * @param out_pDressUpParam 書き込み先の着せ替えパラメータ
 */
void DressUpModelResourceManagerCore::GetMaxDataSizeDressUpParam(DressUpParam* out_pDressUpParam) const
{
  DressUpParam hatOnParam;
  DressUpParam hatOffParam;

  u32 hatOnSize = this->GetMaxDataSizeDressUpParamCore(&hatOnParam, true);
  u32 hatOffSize = this->GetMaxDataSizeDressUpParamCore(&hatOffParam, false);

  if (hatOnSize > hatOffSize)
  {
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      out_pDressUpParam->itemParams[i] = hatOnParam.itemParams[i];
    }
  }
  else
  {
    for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
    {
      out_pDressUpParam->itemParams[i] = hatOffParam.itemParams[i];
    }
  }
}

/**
 * @brief 最大のメモリサイズの着せ替え組み合わせ取得
 *
 * @param out_pDressUpParam 書き込み先の着せ替えパラメータ
 * @param hatPartsEnable 帽子パーツをつけるかつけないかのフラグ
 *
 * @return 必要なデータサイズ
 */
u32 DressUpModelResourceManagerCore::GetMaxDataSizeDressUpParamCore(DressUpParam* out_pDressUpParam, bool hatPartsEnable) const
{
  GFL_ASSERT(m_ArcIdList != NULL);
  GFL_ASSERT(m_pDressUpPartsList != NULL);
  u32 totalSize = 0;

  for (u32 i = 0; i < DressUpModel::PARTS_MODEL_COUNT; ++i)
  {
    out_pDressUpParam->itemParams[i] = -1;

    if (m_ArcIdList[i] < 0)
    {
      continue;
    }
    // シチュエーション着替えは除外
    if (i == DressUpModel::PARTS_MODEL_SITU)
    {
      continue;
    }
    // 帽子をかぶらないなら帽子を除外
    if ((i == DressUpModel::PARTS_MODEL_HAT) && !hatPartsEnable)
    {
      continue;
    }
    // 帽子をかぶるならヘアアクセサリを除外
    if ((i == DressUpModel::PARTS_MODEL_ACCHAIR) && hatPartsEnable)
    {
      continue;
    }

    u32 rate = 1;
    u32 partsCount = m_pDressUpPartsList[i]->GetPartsCount();
    // 2タイプ持っているパーツ用の特殊対処
    if (HasDressUpParts2Type(static_cast<DressUpParam::ItemCategory>(i)))
    {
      rate = 2;
    }

    u32 maxSize = 0;
    u32 maxIndex = 0;
    for (u32 j = 0; j < partsCount; ++j)
    {
      // 帽子をかぶる場合は、かぶれない髪型を除外する
      if ((i == DressUpModel::PARTS_MODEL_HAIR) && hatPartsEnable)
      {
        if (!this->CanDressUpHairWearHat(j))
        {
          continue;
        }
      }

      const gfl2::fs::ArcFile* pArcFile = m_pFileManager->GetArcFile(m_ArcIdList[i]);
      u32 dataSize = 0;
      pArcFile->GetDataSize(&dataSize, j * rate, m_pFileHeap);

      if (maxSize < dataSize)
      {
        maxSize = dataSize;
        maxIndex = j;
      }
    }

    totalSize += maxSize;
    out_pDressUpParam->itemParams[i] = this->GetDressUpItemIndexDefault((DressUpParam::ItemCategory)i, maxIndex);
  }

  return totalSize;
}

/**
 * @brief コンストラクタ
 */
DressUpModelResourceManager::DressUpModelResourceManager() :
  m_SetupIndex(0)
{
}

/**
 * @brief デストラクタ
 */
DressUpModelResourceManager::~DressUpModelResourceManager()
{
  this->Finalize();
}

/**
 * @brief 初期化
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcIdList GARCのIDリスト
 */
void DressUpModelResourceManager::Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].Initialize(pFileManager, pFileHeap, arcIdList);

    arcIdList += DressUpModel::PARTS_MODEL_COUNT + 1; // +1は先頭のカラー
  }
}

/**
 * @brief 初期化：非同期版
 *
 * @param pFileManager ファイル読み込みに使用するファイルマネージャ
 * @param pFileHeap ファイル読み込みに使用するヒープ
 * @param arcIdList GARCのIDリスト
 */
void DressUpModelResourceManager::InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList)
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].InitializeAsync(pFileManager, pFileHeap, arcIdList);

    arcIdList += DressUpModel::PARTS_MODEL_COUNT + 1; // +1は先頭のカラー
  }
}

/**
 * @brief 非同期版の初期化完了待ち
 *
 * @return true:初期化完了 false:初期化中
 */
bool DressUpModelResourceManager::WaitInitializeAsync()
{
  bool result = true;

  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    if (!m_DressUpModelResourceManagerList[i].WaitInitializeAsync())
    {
      result = false;
    }
  }

  return result;
}

/**
 * @brief 終了処理
 */
void DressUpModelResourceManager::Finalize()
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].Finalize();
  }
}

/**
 * @brief フィールド常駐化用の状態保存処理
 */
void DressUpModelResourceManager::Suspend()
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].Suspend();
  }
}

/**
 * @brief フィールド常駐化用の状態復帰処理
 */
void DressUpModelResourceManager::Resume()
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].Resume();
  }
}

/**
 * @brief 着せ替えパーツの同期読み込み
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 */
void DressUpModelResourceManager::LoadDressUpPartsSync(const DressUpParam& dressUpParam)
{
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpPartsSync(dressUpParam);
}

/**
 * @brief 着せ替えパーツの非同期読み込み開始
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 */
void DressUpModelResourceManager::LoadDressUpPartsAsync(const DressUpParam& dressUpParam)
{
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpPartsAsync(dressUpParam);
}

/**
 * @brief 着せ替えパーツの非同期読み込みの完了確認
 *
 * @param dressUpParam 読み込み確認する着せ替えパラメータ
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool DressUpModelResourceManager::IsDressUpPartsLoaded(const DressUpParam& dressUpParam)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].IsDressUpPartsLoaded(dressUpParam);
}

/**
 * @brief 読み込んだ着せ替えパーツを開放する
 *
 * @param dressUpParam 開放する着せ替えパーツ
 */
void DressUpModelResourceManager::UnloadDressUpParts(const DressUpParam& dressUpParam)
{
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].UnloadDressUpParts(dressUpParam);
}

/**
 * @brief 読み込んだ着せ替えパーツを開放できるか確認
 *
 * @param dressUpParam 開放する着せ替えパーツ
 *
 * @return true:解放できる false:解放できない
 */
bool DressUpModelResourceManager::CanUnloadDressUpParts(const DressUpParam& dressUpParam) const
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].CanUnloadDressUpParts(dressUpParam);
}

/**
 * @brief 読み込んだ着せ替えパーツを全て開放する
 */
void DressUpModelResourceManager::UnloadDressUpPartsAll()
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    m_DressUpModelResourceManagerList[i].UnloadDressUpPartsAll();
  }
}

/**
 * @brief 読み込んだ着せ替えパーツを全て開放できるか確認
 *
 * @return true:解放できる false:解放できない
 */
bool DressUpModelResourceManager::CanUnloadDressUpPartsAll() const
{
  for (u32 i = 0; i < DressUpParam::SEX_COUNT; ++i)
  {
    if (!m_DressUpModelResourceManagerList[i].CanUnloadDressUpPartsAll())
    {
      return false;
    }
  }

  return true;
}

/**
 * @brief 読み込んだ着せ替えパーツをセットアップする
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
bool DressUpModelResourceManager::SetupDressUpParts(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  m_SetupIndex = dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX];
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].SetupDressUpParts(pGfxAllocator, dressUpParam);
}

/**
 * @brief 読み込んだ着せ替えパーツをセットアップする：擬似非同期版
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
 */
void DressUpModelResourceManager::SetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  m_SetupIndex = dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX];
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].SetupDressUpPartsAsync(pGfxAllocator, dressUpParam);
}

/**
 * @brief 着せ替えパーツのセットアップ完了を待つ：擬似非同期版
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param dressUpParam セットアップする着せ替えパラメータ
 *
 * @return true: セットアップ完了, false: セットアップ中
 */
bool DressUpModelResourceManager::WaitSetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].WaitSetupDressUpPartsAsync(pGfxAllocator, dressUpParam);
}

/**
 * @brief 着せ替えパーツのセットアップが完了済みか確認
 *
 * @param dressUpParam 確認する着せ替えパラメータ
 *
 * @return true: セットアップ完了, false: セットアップ中
 */
bool DressUpModelResourceManager::IsSetupDressUpParts(const DressUpParam& dressUpParam)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].IsSetupDressUpParts(dressUpParam);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの同期読み込み
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManager::LoadDressUpDynamicAnimationSync(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpDynamicAnimationSync(dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの同期読み込み
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManager::LoadDressUpDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpDynamicAnimationSync(pLoadHeap, dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManager::LoadDressUpDynamicAnimationAsync(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpDynamicAnimationAsync(dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
 *
 * @param pLoadHeap 読み込みに使用するヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 読み込む非常駐アニメーションID
 */
void DressUpModelResourceManager::LoadDressUpDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].LoadDressUpDynamicAnimationAsync(pLoadHeap, dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックの非同期読み込みの完了確認
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 確認する非常駐アニメーションID
 *
 * @return 読み込み完了していたらtrue, まだならfalse
 */
bool DressUpModelResourceManager::IsDressUpDynamicAnimationLoaded(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  return m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].IsDressUpDynamicAnimationLoaded(dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え読み込んだ非常駐アニメーションパックを開放する
 *
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId 開放する非常駐アニメーションID
 */
void DressUpModelResourceManager::UnloadDressUpDynamicAnimation(const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].UnloadDressUpDynamicAnimation(dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替え非常駐アニメーションパックのセットアップ
 *
 * @param pGfxAllocator Gfxライブラリ用のアロケータ
 * @param pSetupHeap セットアップ用のヒープ
 * @param dressUpParam 読み込む着せ替えパラメータ
 * @param dynamicAnimationId セットアップする非常駐アニメーションID
 */
void DressUpModelResourceManager::SetupDressUpDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId)
{
  m_DressUpModelResourceManagerList[dressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX]].SetupDressUpDynamicAnimation(pGfxAllocator, pSetupHeap, dressUpParam, dynamicAnimationId);
}

/**
 * @brief 着せ替えリソースマネージャコア取得
 *
 * @param sex 性別
 */
DressUpModelResourceManagerCore* DressUpModelResourceManager::GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::Sex sex)
{
  return &m_DressUpModelResourceManagerList[sex];
}

/**
 * @brief 着せ替えリソースマネージャコア取得
 *
 * @param sex 性別
 */
const DressUpModelResourceManagerCore* DressUpModelResourceManager::GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::Sex sex) const
{
  return &m_DressUpModelResourceManagerList[sex];
}

/**
 * @brief セットアップ済み着せ替えリソースマネージャコア取得
 */
DressUpModelResourceManagerCore* DressUpModelResourceManager::GetSetupDressUpModelResourceManagerCore()
{
  return &m_DressUpModelResourceManagerList[m_SetupIndex];
}

/**
 * @brief セットアップ済み着せ替えリソースマネージャコア取得
 */
const DressUpModelResourceManagerCore* DressUpModelResourceManager::GetSetupDressUpModelResourceManagerCore() const
{
  return &m_DressUpModelResourceManagerList[m_SetupIndex];
}

}}
