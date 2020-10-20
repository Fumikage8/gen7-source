#if !defined(POKE_3D_LIB_MODEL_CHARA_MODEL_FACTORY_H_INCLUDED)
#define POKE_3D_LIB_MODEL_CHARA_MODEL_FACTORY_H_INCLUDED

#include "gfl2_CharaModel.h"
#include "gfl2_DressUpModel.h"

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>

#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <heap/include/gfl2_Heap.h>

namespace poke_3d { namespace model {

// ノーマルGARCの動的アニメオフセットリスト
class NormalGarcDyAnimOffsetList;

// ノーマルGARCのフッター
class NormalGarcFooter;

// 着せ替えパーツリスト
class DressUpPartsList;

// 着せ替えGARCのフッター
class DressUpGarcFooter;

// ノーマルGARCの動的アニメオフセットリスト
class DressUpGarcDyAnimOffsetList;

// カラーテーブル
class ColorTable;

// カラーテーブル例外
class ColorTableExt;

// キャラモデルリソースの管理単位
class CharaModelResourceUnit;

// 着せ替えモデルリソースの管理単位
class DressUpModelResourceUnit;

// 着せ替えモデルリソースマネージャー
class DressUpModelResourceManager;

// 着せ替えモデルリソースマネージャーコア
class DressUpModelResourceManagerCore;

// 着せ替えアイテムパラメータ
struct DressUpItemParam
{
  u8 partsType;
  u8 partsIndex;
  u8 colorType;
  u8 colorIndex;
  u8 option;
  u8 partsSex;
  u8 reserved1;
  u8 reserved2;

  // 着せ替えアイテムオプション：レッグ指定
  enum LegOption
  {
    LEG_OPTION_LONG,
    LEG_OPTIN_SHORT
  };

  // 着せ替えアイテムオプション：影指定
  enum ShadowOption
  {
    SHADOW_OPTION_ON,
    SHADOW_OPTION_OFF
  };
};


/**
 * @brief キャラクター用モデルクラスのファクトリクラス
 */
class CharaModelFactory
{
public:

  //---------------------------------------------------------------------------------
  // ■ 高速化用にあらかじめ常駐化させる

  // リロード用関数
  typedef void (*ReloadFunc)();

  /**
   * @brief 常駐化初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcId GARCのID
   */
  static void ResidentInitialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId);

  /**
   * @brief 常駐化終了処理
   *
   * @param arcId GARCのID
   */
  static void ResidentFinalize(u32 arcId);

  /**
   * @brief 常習GARCリロード予約
   *
   * @param reloadFunc リロード用関数
   */
  static void ResidentReloadRequest(ReloadFunc reloadFunc);

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  CharaModelFactory();

  /**
   * @brief デストラクタ
   */
  ~CharaModelFactory();

  /**
   * @brief 初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcId GARCのID
   */
  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId);

  /**
   * @brief 初期化：非同期版
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcId GARCのID
   */
  void InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, u32 arcId);

  /**
   * @brief 非同期版の初期化完了待ち
   *
   * @return true:初期化完了 false:初期化中
   */
  bool WaitInitializeAsync();

  /**
   * @brief 終了処理
   */
  void Finalize();

  /**
   * @brief データの同期読み込み
   *
   * @param charaId 読み込むキャラID
   */
  void LoadModelSync(u32 charaId);

  /**
   * @brief データの設定
   *
   * @param charaId 読み込むキャラID
   * @param pData   静的データ
   */
  void SetModel(u32 charaId, void * pData);
 
  /**
   * @brief データの非同期読み込み開始
   *
   * @param charaId 読み込むキャラID
   */
  void LoadModelAsync(u32 charaId);

  /**
   * @brief データの非同期読み込み開始
   *
   * @param pLoadHeap 読み込みヒープ
   * @param charaId 読み込むキャラID
   */
  void LoadModelAsync(gfl2::heap::HeapBase * pLoadHeap, u32 charaId);

  /**
   * @brief データの非同期読み込みの完了確認
   *
   * @param charaId 確認するキャラID
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsModelLoaded(u32 charaId);

  /**
   * @brief 読み込んだモデルデータをセットアップする
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param charaId セットアップするキャラID
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupModel(gfl2::gfx::IGLAllocator* pGfxAllocator, u32 charaId);

  /**
   * @brief 読み込んだデータを開放する
   *
   * @param charaId 開放するキャラID
   */
  void UnloadModel(u32 charaId);

  /**
   * @brief 読み込んだデータを開放できるか確認
   *
   * @param charaId 開放するキャラID
   *
   * @return true: 解放できる, false: 解放できない
   */
  bool CanUnloadModel(u32 charaId) const;

  /**
   * @brief 読み込んだデータを全て開放する
   */
  void UnloadModelAll();

  /**
   * @brief 読み込んだデータを全て開放できるか確認
   */
  bool CanUnloadModelAll() const;

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
  bool CreateModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, CharaModel* pCharaModel, u32 charaId);

  /**
   * @brief 読み込んだデータのモデルデータサイズ取得
   *
   * @param charaId 取得するキャラID
   *
   * @return データサイズ
   */
  u32 GetModelDataSize(u32 charaId) const;

  /**
   * @brief 読み込んだデータのアニメーションデータサイズ取得
   *
   * @param charaId 取得するキャラID
   *
   * @return データサイズ
   */
  u32 GetAnimationDataSize(u32 charaId) const;

  /**
   * @brief 読み込んだデータの合計データサイズ取得
   *
   * @param charaId 取得するキャラID
   *
   * @return データサイズ
   */
  u32 GetTotalDataSize(u32 charaId) const;

  /**
  * @brief 読み込んだモデルデータの数を取得
  *
  * @return 読み込んだモデルデータの数
  */
  u32 GetModelLoadedCount( void );


  //---------------------------------------------------------------------------------
  // ■ 着せ替え

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
  bool CreateDressUpModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, DressUpModel* pDressUpModel, DressUpModelResourceManager* pResourceManager, u32 charaId);

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
  bool CreateDressUpModel(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pAnimationHeap, DressUpModel* pDressUpModel, DressUpModelResourceManagerCore* pResourceManagerCore, u32 charaId);

  //---------------------------------------------------------------------------------
  // ■ アプリ用データ

  /**
   * @brief アプリ用データの取得
   *
   * @param charaId 取得するキャラID
   *
   * @return アプリ用データを返す。まだ読み込まれていないならNULL
   */
  const void* GetApplicationData(u32 charaId) const;

  //---------------------------------------------------------------------------------
  // ■ 非常駐アニメーション

  /**
   * @brief 非常駐アニメーションパックの同期読み込み
   *
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDynamicAnimationSync(u32 dynamicAnimationId);

  /**
   * @brief 非常駐アニメーションパックの同期読み込み
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, u32 dynamicAnimationId);
 
  /**
   * @brief 非常駐アニメーションパックの非同期読み込み開始
   *
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDynamicAnimationAsync(u32 dynamicAnimationId);
 
  /**
   * @brief 非常駐アニメーションパックの非同期読み込み開始
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, u32 dynamicAnimationId);

  /**
   * @brief 非常駐アニメーションパックの非同期読み込みの完了確認
   *
   * @param dynamicAnimationId 確認する非常駐アニメーションID
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDynamicAnimationLoaded(u32 dynamicAnimationId);

  /**
   * @brief 非常駐アニメーションパックのセットアップ
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pSetupHeap セットアップ用のヒープ
   * @param dynamicAnimationId セットアップする非常駐アニメーションID
   */
  void SetupDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, u32 dynamicAnimationId);

  /**
   * @brief 読み込んだ非常駐アニメーションパックを開放する
   *
   * @param dynamicAnimationId 開放する非常駐アニメーションID
   */
  void UnloadDynamicAnimation(u32 dynamicAnimationId);

  /**
   * @brief 自動目パチ用乱数取得関数のセットアップ
   *
   * @param pGetRandomFunc 乱数取得関数のポインタ
   */
  void SetupRandomFunc( u32 (*pGetRandomFunc)(u32) );

  //---------------------------------------------------------------------------------
  // ■ nijiバグ対処用の穴 ※nijiアッパーでは使用しないでください

  // @fix NMCat[3187] : ローリングドリーマー・アローラ：特定の手順でNPCがかくつく
  // スクリプト側で視線制御速度を書き換えているが元に戻していないのが原因
  // スクリプト側では変更前の速度も保存していないため戻せない
  // 
  // Factoryが保持している元リソースの値を取得する関数を追加し、この値に戻してもらう

  /**
   * @brief 注視用のジョイント数取得
   *
   * @param charaId 取得するキャラID
   *
   * @return 注目用ジョイント数
   */
  u32 GetModelData_InterestJointCount(u32 charaId) const;

  /**
   * @brief 注目速度取得
   *
   * @param charaId 取得するキャラID
   * @param index インデックス
   *
   * @return 注目速度を返す
   */
  f32 GetModelData_InterestSpeed(u32 charaId, u32 index) const;

private:

  /**
   * @brief リソースユニット取得
   *
   * @param charaId キャラ番号
   *
   * @return 対応するリソースユニットを返す。無効なIDならNULL
   */
  CharaModelResourceUnit* GetResourceUnit_(u32 charaId);

  /**
   * @brief リソースユニット取得
   *
   * @param charaId キャラ番号
   *
   * @return 対応するリソースユニットを返す。無効なIDならNULL
   */
  const CharaModelResourceUnit* GetResourceUnit_(u32 charaId) const;

  /**
   * @brief データの読み込み
   *
   * @param isAsync true:非同期読み込み false:同期読み込み
   * @param charaId 読み込むキャラID
   * @param pHeap   ヒープ
   */
  void LoadModel_(bool isAsync, u32 charaId, gfl2::heap::HeapBase * pHeap);

  /**
   * @brief データの設定
   *
   * @param charaId キャラID
   * @param pData   設定するデータ
   */
  void SetModel_(u32 charaId, void* pData);

  /**
   * @brief 非常駐アニメーションパックの読み込み
   *
   * @param isAsync true:非同期読み込み false:同期読み込み
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDynamicAnimation_(bool isAsync, u32 dynamicAnimationId);

private:

  u32 m_ArcId;                                                ///< 読み込み対象のアーカイブID
  gfl2::fs::AsyncFileManager* m_pFileManager;                 ///< ファイルマネージャ
  gfl2::heap::HeapBase* m_pFileHeap;                          ///< 読み込み用のヒープ

  CharaModelResourceUnit* m_pResourceUnitList;                ///< リソースユニットリスト

  NormalGarcDyAnimOffsetList* m_pNormalGarcDyAnimOffsetList;  ///< ノーマルGARCの動的アニメオフセットリスト
  NormalGarcFooter* m_pNormalGarcFooter;                      ///< ノーマルGARCのフッター

  u32 (*m_pGetRandomFunc)(u32);
};

/**
 * @brief 着せ替えモデルのリソース管理クラス：コア
 */
class DressUpModelResourceManagerCore
{
  friend class CharaModelFactory;

public:

  //---------------------------------------------------------------------------------
  // ■ 高速化用にあらかじめ常駐化させる

  /**
   * @brief 常駐化初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  static void ResidentInitialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList);

  /**
   * @brief 常駐化終了処理
   *
   * @param arcIdList GARCのIDリスト
   */
  static void ResidentFinalize(s32* arcIdList);

  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  DressUpModelResourceManagerCore();

  /**
   * @brief デストラクタ
   */
  ~DressUpModelResourceManagerCore();

  /**
   * @brief 初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   * @param isDataCheckMode false:通常モード, true:メモリ確保をや読み込みを一切やらないデータチェック用モード
   */
  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList, bool isDataCheckMode = false);

  /**
   * @brief 初期化：非同期版
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  void InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList);

  /**
   * @brief 非同期版の初期化完了待ち
   *
   * @return true:初期化完了 false:初期化中
   */
  bool WaitInitializeAsync();

  /**
   * @brief 終了処理
   */
  void Finalize();

  /**
   * @brief フィールド常駐化用の状態保存処理
   */
  void Suspend();

  /**
   * @brief フィールド常駐化用の状態復帰処理
   */
  void Resume();

  /**
   * @brief 有効な着せ替えの組み合わせか確認
   *
   * @param dressUpParam 確認する着せ替えパラメータ
   *
   * @return 有効な組み合わせならtrue, 無効ならfalse
   */
  bool CheckDressUpCombination(const DressUpParam& dressUpParam) const;

  /**
  * @brief 有効な着せ替えの組み合わせか確認（ファインダースタジオ用）
  *
  * @param dressUpParam 確認する着せ替えパラメータ
  *
  * @return 有効な組み合わせならtrue, 無効ならfalse
  * 
  * @note momiji追加 @fix GFMMCat[72] CheckDressUpCombination関数ではファインダースタジオのおいてバグが見つかったため関数を追加
  */
  bool CheckDressUpCombinationFinderStudio(const DressUpParam& dressUpParam) const;

  /**
   * @brief 着せ替えパーツの同期読み込み
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   */
  void LoadDressUpPartsSync(const DressUpParam& dressUpParam);
 
  /**
   * @brief 着せ替えパーツの非同期読み込み開始
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   */
  void LoadDressUpPartsAsync(const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツの非同期読み込みの完了確認
   *
   * @param dressUpParam 読み込み確認する着せ替えパラメータ
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDressUpPartsLoaded(const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツの読み込み
   *
   * @param isAsync true:非同期読み込み false:同期読み込み
   * @param dressUpParam 読み込む着せ替えパラメータ
   */
  void LoadDressUpParts_(bool isAsync, const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツを開放する
   *
   * @param dressUpParam 開放する着せ替えパーツ
   */
  void UnloadDressUpParts(const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツを開放できるか確認
   *
   * @param dressUpParam 開放する着せ替えパーツ
   *
   * @return true:解放できる false:解放できない
   */
  bool CanUnloadDressUpParts(const DressUpParam& dressUpParam) const;

  /**
   * @brief 読み込んだ着せ替えパーツを全て開放する
   */
  void UnloadDressUpPartsAll();

  /**
   * @brief 読み込んだ着せ替えパーツを全て開放できるか確認
   *
   * @return true:解放できる false:解放できない
   */
  bool CanUnloadDressUpPartsAll() const;

  /**
   * @brief 読み込んだ着せ替えパーツをセットアップする
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupDressUpParts(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツをセットアップする：擬似非同期版
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  void SetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツのセットアップ完了を待つ：擬似非同期版
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: セットアップ完了, false: セットアップ中
   */
  bool WaitSetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツのセットアップが完了済みか確認
   *
   * @param dressUpParam 確認する着せ替えパラメータ
   *
   * @return true: セットアップ完了, false: セットアップ中
   */
  bool IsSetupDressUpParts(const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えデータのモデルデータサイズ取得
   *
   * @param dressUpParam データサイズを取得する着せ替えパラメータ
   *
   * @return データサイズ
   */
  u32 GetDressUpModelDataSize(const DressUpParam& dressUpParam) const;

  /**
   * @brief 読み込んだ着せ替えデータのアニメーションデータサイズ取得
   *
   * @param dressUpParam データサイズを取得する着せ替えパラメータ
   *
   * @return データサイズ
   */
  u32 GetDressUpAnimationDataSize(const DressUpParam& dressUpParam) const;

  /**
   * @brief 読み込んだ着せ替えデータの合計データサイズ取得
   *
   * @param dressUpParam データサイズを取得する着せ替えパラメータ
   *
   * @return データサイズ
   */
  u32 GetDressUpTotalDataSize(const DressUpParam& dressUpParam) const;

  /**
   * @brief 着せ替えパーツのデータIDリストを作成
   *
   * @param pDatIdList 書き込み先のデータIDリスト
   * @param dressUpParam 読み込む着せ替えパラメータ
   *
   * @return 着せ替えパーツのデータIDリストを返す
   */
  void MakeDressUpPartsDatIdList_(s16* pDatIdList, const DressUpParam& dressUpParam);

  //---------------------------------------------------------------------------------
  // ■ 着せ替え非常駐アニメーション

  /**
   * @brief 着せ替え非常駐アニメーションパックの同期読み込み
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationSync(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの同期読み込み
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationAsync(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込みの完了確認
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 確認する非常駐アニメーションID
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDressUpDynamicAnimationLoaded(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え読み込んだ非常駐アニメーションパックを開放する
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 開放する非常駐アニメーションID
   */
  void UnloadDressUpDynamicAnimation(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックのセットアップ
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pSetupHeap セットアップ用のヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId セットアップする非常駐アニメーションID
   */
  void SetupDressUpDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

public: // ここから主にビューアー用

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：肌
   */
  gfl2::math::Vector3 GetDressUpColorSkin(u32 index) const;

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：髪
   */
  gfl2::math::Vector3 GetDressUpColorHair(u32 index) const;

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：服
   */
  gfl2::math::Vector3 GetDressUpColorCloth(u32 index) const;

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：眉
   */
  gfl2::math::Vector3 GetDressUpColorEyeblow(u32 index) const;

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：目
   */
  gfl2::math::Vector3 GetDressUpColorEye(u32 index) const;

  /**
   * @brief 着せ替えパーツのRGBを0.0～1.0で取得：リップ
   */
  gfl2::math::Vector3 GetDressUpColorLip(u32 index) const;

  /**
   * @brief 着せ替えアイテム数取得
   */
  u32 GetDressUpItemCount(DressUpParam::ItemCategory itemCategory) const;

  /**
   * @brief 着せ替えパーツ数取得
   */
  u32 GetDressUpPartsCount(DressUpParam::ItemCategory itemCategory) const;

  /**
   * @brief アイテムインデックスからパーツインデックス取得
   */
  u32 GetDressUpPartsIndex(DressUpParam::ItemCategory itemCategory, s32 itemIndex) const;

  /**
   * @brief アイテムインデックスからカラーインデックス取得
   */
  s32 GetDressUpColorIndex(DressUpParam::ItemCategory itemCategory, s32 itemIndex) const;

  /**
   * @brief パーツの有効なカラービットを取得
   */
  u64 GetDressUpPartsColorEnableBit(DressUpParam::ItemCategory itemCategory, u32 partsIndex) const;

  /**
   * @brief パーツの有効なカラーか確認
   */
  bool IsDressUpPartsColorEnabled(DressUpParam::ItemCategory itemCategory, u32 partsIndex, u32 colorIndex) const;

  /**
   * @brief パーツインデックスと色からアイテムインデックスを取得
   */
  s32 GetDressUpItemIndex(DressUpParam::ItemCategory itemCategory, u32 partsIndex, u32 colorIndex) const;

  /**
   * @brief パーツインデックスからアイテムインデックスを取得：色はデフォルトのものを自動で選択
   */
  s32 GetDressUpItemIndexDefault(DressUpParam::ItemCategory itemCategory, u32 partsIndex) const;

  /**
   * @brief 帽子をかぶることができる髪形か確認
   */
  bool CanDressUpHairWearHat(s32 hairItemIndex) const;

  /**
   * @brief 最大のメモリサイズの着せ替え組み合わせ取得
   *
   * @param out_pDressUpParam 書き込み先の着せ替えパラメータ
   */
  void GetMaxDataSizeDressUpParam(DressUpParam* out_pDressUpParam) const;

private:

  /**
   * @brief 最大のメモリサイズの着せ替え組み合わせ取得
   *
   * @param out_pDressUpParam 書き込み先の着せ替えパラメータ
   * @param hatPartsEnable 帽子パーツをつけるかつけないかのフラグ
   *
   * @return 必要なデータサイズ
   */
  u32 GetMaxDataSizeDressUpParamCore(DressUpParam* out_pDressUpParam, bool hatPartsEnable) const;

  /**
   * @brief 読み込んだ着せ替えパーツをセットアップする
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupDressUpPartsCore(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam, u32 partsModelIndex);

  /**
   * @brief 読み込んだ着せ替えパーツの色替えする
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   * @param partsModelIndex セットアップするパーツモデル番号
   * @param pMaskIndex セットアップするマスク番号
   * @param pSubmaskIndex セットアップするサブマスク番号
   * @param pTexturePixel セットアップするテクスチャのピクセル
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupDressUpPartsChangeTextureColor(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam, u32 partsModelIndex, u32* pMaskIndex, u32* pSubmaskIndex, u32* pTexturePixel);

private:
  s32 m_ArcIdColor;
  s32* m_ArcIdList;
  DressUpGarcFooter** m_pGarcFooterList;
  DressUpPartsList** m_pDressUpPartsList;
  DressUpGarcDyAnimOffsetList** m_pDressUpDyAnimOffsetList;
  ColorTable** m_pColorTableList;
  ColorTableExt* m_pColorTableExt;

  gfl2::fs::AsyncFileManager* m_pFileManager;
  gfl2::heap::HeapBase* m_pFileHeap;

  DressUpModelResourceUnit* m_pDressUpModelResourceUnitList;
  u32 m_CurrentSetupSeq;
  u32 m_CurrentSetupPartsIndex;
  u32 m_CurrentSetupMaskIndex;
  u32 m_CurrentSetupSubMaskIndex;
  u32 m_CurrentSetupTexturePixel;

  b8 m_HasBagCharm;
};

/**
 * @brief 着せ替えモデルのリソース管理クラス
 */
class DressUpModelResourceManager
{
public:
  //---------------------------------------------------------------------------------
  // ■ 作成と破棄

  /**
   * @brief コンストラクタ
   */
  DressUpModelResourceManager();

  /**
   * @brief デストラクタ
   */
  ~DressUpModelResourceManager();

  /**
   * @brief 初期化
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  void Initialize(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList);

  /**
   * @brief 初期化：非同期版
   *
   * @param pFileManager ファイル読み込みに使用するファイルマネージャ
   * @param pFileHeap ファイル読み込みに使用するヒープ
   * @param arcIdList GARCのIDリスト
   */
  void InitializeAsync(gfl2::fs::AsyncFileManager* pFileManager, gfl2::heap::HeapBase* pFileHeap, s32* arcIdList);

  /**
   * @brief 非同期版の初期化完了待ち
   *
   * @return true:初期化完了 false:初期化中
   */
  bool WaitInitializeAsync();

  /**
   * @brief 終了処理
   */
  void Finalize();

  /**
   * @brief フィールド常駐化用の状態保存処理
   */
  void Suspend();

  /**
   * @brief フィールド常駐化用の状態復帰処理
   */
  void Resume();

  /**
   * @brief 着せ替えパーツの同期読み込み
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   */
  void LoadDressUpPartsSync(const DressUpParam& dressUpParam);
 
  /**
   * @brief 着せ替えパーツの非同期読み込み開始
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   */
  void LoadDressUpPartsAsync(const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツの非同期読み込みの完了確認
   *
   * @param dressUpParam 読み込み確認する着せ替えパラメータ
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDressUpPartsLoaded(const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツを開放する
   *
   * @param dressUpParam 開放する着せ替えパーツ
   */
  void UnloadDressUpParts(const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツを開放できるか確認
   *
   * @param dressUpParam 開放する着せ替えパーツ
   *
   * @return true:解放できる false:解放できない
   */
  bool CanUnloadDressUpParts(const DressUpParam& dressUpParam) const;

  /**
   * @brief 読み込んだ着せ替えパーツを全て開放する
   */
  void UnloadDressUpPartsAll();

  /**
   * @brief 読み込んだ着せ替えパーツを全て開放できるか確認
   *
   * @return true:解放できる false:解放できない
   */
  bool CanUnloadDressUpPartsAll() const;

  /**
   * @brief 読み込んだ着せ替えパーツをセットアップする
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  bool SetupDressUpParts(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 読み込んだ着せ替えパーツをセットアップする：擬似非同期版
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: 正常にセットアップ完了, false: セットアップ失敗、読み込み完了していないorリソースが壊れているなど
   */
  void SetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツのセットアップ完了を待つ：擬似非同期版
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param dressUpParam セットアップする着せ替えパラメータ
   *
   * @return true: セットアップ完了, false: セットアップ中
   */
  bool WaitSetupDressUpPartsAsync(gfl2::gfx::IGLAllocator* pGfxAllocator, const DressUpParam& dressUpParam);

  /**
   * @brief 着せ替えパーツのセットアップが完了済みか確認
   *
   * @param dressUpParam 確認する着せ替えパラメータ
   *
   * @return true: セットアップ完了, false: セットアップ中
   */
  bool IsSetupDressUpParts(const DressUpParam& dressUpParam);

  //---------------------------------------------------------------------------------
  // ■ 着せ替え非常駐アニメーション

  /**
   * @brief 着せ替え非常駐アニメーションパックの同期読み込み
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationSync(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの同期読み込み
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationSync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationAsync(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込み開始
   *
   * @param pLoadHeap 読み込みに使用するヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 読み込む非常駐アニメーションID
   */
  void LoadDressUpDynamicAnimationAsync(gfl2::heap::HeapBase* pLoadHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックの非同期読み込みの完了確認
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 確認する非常駐アニメーションID
   *
   * @return 読み込み完了していたらtrue, まだならfalse
   */
  bool IsDressUpDynamicAnimationLoaded(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え読み込んだ非常駐アニメーションパックを開放する
   *
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId 開放する非常駐アニメーションID
   */
  void UnloadDressUpDynamicAnimation(const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替え非常駐アニメーションパックのセットアップ
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pSetupHeap セットアップ用のヒープ
   * @param dressUpParam 読み込む着せ替えパラメータ
   * @param dynamicAnimationId セットアップする非常駐アニメーションID
   */
  void SetupDressUpDynamicAnimation(gfl2::gfx::IGLAllocator* pGfxAllocator, gfl2::heap::HeapBase* pSetupHeap, const DressUpParam& dressUpParam, u32 dynamicAnimationId);

  /**
   * @brief 着せ替えリソースマネージャコア取得
   *
   * @param sex 性別
   */
  DressUpModelResourceManagerCore* GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::Sex sex);

  /**
   * @brief 着せ替えリソースマネージャコア取得
   *
   * @param sex 性別
   */
  const DressUpModelResourceManagerCore* GetDressUpModelResourceManagerCore(poke_3d::model::DressUpParam::Sex sex) const;

  /**
   * @brief セットアップ済み着せ替えリソースマネージャコア取得
   */
  DressUpModelResourceManagerCore* GetSetupDressUpModelResourceManagerCore();

  /**
   * @brief セットアップ済み着せ替えリソースマネージャコア取得
   */
  const DressUpModelResourceManagerCore* GetSetupDressUpModelResourceManagerCore() const;

private:

  DressUpModelResourceManagerCore m_DressUpModelResourceManagerList[poke_3d::model::DressUpParam::SEX_COUNT];
  u32 m_SetupIndex;
};

}}

#endif // POKE_3D_LIB_MODEL_CHARA_MODEL_FACTORY_H_INCLUDED
