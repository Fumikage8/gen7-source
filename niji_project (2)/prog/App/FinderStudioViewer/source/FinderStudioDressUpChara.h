//======================================================================
/**
 * @file FinderStudioDressUpChara.h
 * @date 2016/09/01
 * @author harada_takumi
 * @brief ファインダースタジオ主人公モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_DRESS_UP_CHARA_H_INCLUDED__
#define __FINDER_STUDIO_DRESS_UP_CHARA_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)
  class Object_Chara;
  class ObjectRenderPipeLine;
GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
  class CharaModelFactory;
  class DressUpModelResourceManager;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  struct PhotoParam;

class FinderStudioDressUpChara
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioDressUpChara); //コピーコンストラクタ＋代入禁止

public:
  struct Desc
  {
    gfl2::heap::HeapBase* pHeap;
    gfl2::heap::HeapBase* pWorkHeap;
    System::nijiAllocator* pAllocator;
    DemoLib::Obj::ObjectRenderPipeLine* pObjectRenderPipeLine;
    poke_3d::model::CharaModelFactory* pCharaModelFactory;
    poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager;
    // pDressUpParamかpPhotoParamかどちらかでよい
    poke_3d::model::DressUpParam* pDressUpParam;
    PhotoParam* pPhotoParam;
    bool isUsePhotoParam;

    Desc():
      pHeap(NULL)
      ,pWorkHeap(NULL)
      ,pAllocator(NULL)
      ,pObjectRenderPipeLine(NULL)
      ,pCharaModelFactory(NULL)
      ,pDressUpModelResourceManager(NULL)
      ,pDressUpParam(NULL)
      ,pPhotoParam(NULL)
      ,isUsePhotoParam(false){}
  };

  FinderStudioDressUpChara(const Desc& desc);
  virtual ~FinderStudioDressUpChara();

  //! @brief 初期化処理
  //! @note 着せ替えモデルのロードからパイプラインへの登録まで全て行う
  //! @return trueなら初期化処理終了
  bool InitFunc();
  // 終了処理
  bool EndFunc();
  // 更新
  void Update();

  /// @brief ポケモンのバウンディングボックス登録
  void RegistPokemonAABB(const gfl2::math::AABB* pAABB){ m_pPokemonAABB = pAABB; }

  /// @brief 写真再現パラメータの出力が可能かどうか
  /// @return true 可能 false 不可能
  bool IsCanGetPhotoParam();
  /// @brief 再現用パラメータ出力
  /// @param[out] pPhotoParam 再現用パラメータ 必要な情報だけを格納する
  void GetPhotoParamTrainer(PhotoParam* pPhotoParam) const;

  /// @brief 写真再現パラメータ適用（着せ替えデータ）
  /// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
  void ApplyPhotoParamDressUpParam(const PhotoParam& photoParam);
  /// @brief 写真再現パラメータ適用（その他データ）
  void ApplyPhotoParamTrainerOther(const PhotoParam& photoParam);

  /// @brief 写真パラメータから着せ替えパラメータを生成
  /// @param photoParam 写真パラメータ
  /// @return 着せ替えパラメータ
  static poke_3d::model::DressUpParam GetDressUpParamFromPhotoParam(const PhotoParam& photoParam);
  /// @brief 着せ替えパラメータを写真パラメータに適応
  /// @param pPhotoParam 適応する写真パラメータ
  /// @param dressUpParam 着せ替えデータ
  static void SetDressUpParamToPhotoParam(PhotoParam* pPhotoParam,const poke_3d::model::DressUpParam& dressUpParam);

  /// @brief アニメーション更新のオンオフを切り替える
  /// @param true オン false オフ
  /// @note 写真再現後の更新のため
  void SetIsAnimationUpdate(bool isUpdate);
  /// @brief 自動目パチのオンオフを切り替える
  /// @param true オン false オフ
  void SetIsAutoBlink(bool isBlink);

  /// @brief 撮影モード切替時処理
  void OnChangeCaptureMode(CaptureMode captureMode);

  /// @brief 構図用パラメータ適用
  /// @param composition 構図の種類
  void ApplyComposition(Composition composition);

  /// @brief 着せ替えモデル再ロード（着せ替えパラメータは内部のものを使用）
  //void StartReloadDressUpModel();
  /// @brief 着せ替えモデル再ロード
  /// @param dressUpParam 着せ替えデータ
  void StartReloadDressUpModel(poke_3d::model::DressUpParam dressUpParam);
  /// @brief 再ロードが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishReloadDressUpModel();

  /// @brief 時間帯によるカラー変更
  /// @param timeZone 時間帯
  void SetConstantColorByTime(TimeZone timeZone);

  /// @brief 座標取得
  /// @return 座標
  const gfl2::math::Vector3& GetPos() const { return m_Pos; }

  /// @brief AABB取得
  const gfl2::math::AABB* GetAABB() const { return &m_AABB; }

  /// @brief Zモーション再生
  /// @param motionType Zモーションのタイプ
  /// @return 再生に成功したら（すでに再生中なら失敗）
  bool PlayZMotion(ZMotionType motionType);

  /// @brief Zモーション一時停止
  void StopZMotion();

  /// @brief Zモーションが再生中かどうか
  /// @return 再生中かどうか
  bool IsPlayingZMotion() const;

  /// @brief Zモーションが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishZMotion() const;

  /// @brief 待機状態に
  /// @note 強制的に待機モーションへ移行する
  void ChangeWaitState();

  /// @brief トレーナー角度変更（左回り）
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool RotateLeft();
  /// @brief トレーナー角度変更（右回り）
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool RotateRight();

  /// @brief 角度をセット
  /// @param degY Y軸角度(0 <= degY < 360)
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool SetRotate(float degY);

  /// @brief Y軸角度を取得
  /// @return 角度
  f32 GetDegY() const { return m_DegY; }

  /// @brief 当たり判定を無視したY軸角度を取得
  /// @return Y軸角度
  f32 GetDegYIgnoreCollision() const { return m_IgnoreCollisionDegY; }

  /// @brief 当たり判定を無視したAABBを取得
  /// @return AABB
  const gfl2::math::AABB GetAABBIgnoreCollision() const;

  /// @brief 角度をリセット
  void ResetRotate();

  // Zモーションのヒープサイズ
  static const u32 MOTION_HEAP_SIZE;

  // リクエストタイプ
  enum REQ_TYPE{
    REQ_TYPE_NULL,
    REQ_TYPE_PLAY_Z_MOTION,
    REQ_TYPE_STOP_Z_MOTION,
    REQ_TYPE_RESET,

    REQ_TYPE_MAX
  };

  // デバッグ用
#if PM_DEBUG
  void SetNowSelectAnmNo(u32 anmNo){
    // 待機中のみ反映
    if(m_State != CHARA_STATE_WAIT) return;
    m_ZMotionType = (ZMotionType)anmNo;
  }
  void SetVisible(bool isVisible);

  void SetCollisionPermit(f32 collisionPermit){ m_CollisionPermit = collisionPermit; }

  // デバッグで選択中のZモーション再生
  void DebugPlayZMotion();

#endif

private:
  // キャラの状態（モード）
  enum CHARA_STATE{
    CHARA_STATE_NULL,   /// 何もしない
    CHARA_STATE_WAIT,   /// 待機中
    CHARA_STATE_Z_MOTION,  /// Zモーション中
    CHARA_STATE_Z_MOTION_STOP  /// Zモーション一時停止中
  };

  // Zモーション以外のモーションインデックス
  enum OTHER_MOTION_INDEX{
    OTHER_MOTION_Z_OUT_NORMAL,
    OTHER_MOTION_Z_OUT_HONOO,
    OTHER_MOTION_Z_OUT_MIZU,
    OTHER_MOTION_Z_OUT_KUSA,
    OTHER_MOTION_Z_OUT_DENKI,
    OTHER_MOTION_Z_OUT_KOORI,
    OTHER_MOTION_Z_OUT_KAKUTOU,
    OTHER_MOTION_Z_OUT_DOKU,
    OTHER_MOTION_Z_OUT_ZIMEN,
    OTHER_MOTION_Z_OUT_HIKOU,
    OTHER_MOTION_Z_OUT_ESUPAA,
    OTHER_MOTION_Z_OUT_MUSI,
    OTHER_MOTION_Z_OUT_IWA,
    OTHER_MOTION_Z_OUT_GOOSUTO,
    OTHER_MOTION_Z_OUT_DORAGON,
    OTHER_MOTION_Z_OUT_AKU,
    OTHER_MOTION_Z_OUT_HAGANE,
    OTHER_MOTION_Z_OUT_FEARII,
    OTHER_MOTION_Z_OUT_PIKA,
    OTHER_MOTION_Z_IN,
    OTHER_MOTION_GREET,
    OTHER_MOTION_WAIT,

    OTHER_MOTION_MAX
  };

  // 非常駐モーション読み込み
  void startLoadDynamicAnimation();
  // 読み込みが終わったか
  bool isFinishLoadDynamicAnimation();
  // 非常駐モーション解放
  void unloadDynamicAniamtion();

  // モデルロード
  void startLoadModel();
  // 読み込みが終わったか
  bool isFinishLoadModel();

  // Z技モーション読み込み
  void loadZMotionAsync();
  // 読み込みが終わったか
  bool isFinishLoadZMotionAsync();

  // モデル読み込み後の初期化
  void setup();

  // AABBセット
  void setupAABB();
  // 元となるAABBを取得
  gfl2::math::AABB getOrigAABB() const;

  /// @brief アニメーション更新前のパラメータ保存
  void savePrevParam();

  // 更新メイン処理
  void updateMain();
  // キャラの状態遷移
  void transitionState(CHARA_STATE state);
  // 各状態関数
  void updateWait();
  void updateZMotion();
  void updateZMotionStop();
  void updateRotate();
  // 外部からのリクエスト
  void request(REQ_TYPE reqType){ m_RequestType = reqType; }
  void requestZMotionType(ZMotionType ZMotionType){ m_ReqestZMotionType = ZMotionType; }

  /// @brief Z技モーション再生
  /// @param type Zモーションタイプ
  /// @param isForce 同じモーションだった場合再生しなおすか
  void changeAnimationZ(ZMotionType type,bool isForce = true);

  /// @brief その他モーション再生
  /// @param otherMotionIndex モーションインデックス
  /// @param isForce 同じモーションだった場合再生しなおすか
  void changeAnimationOther(OTHER_MOTION_INDEX otherMotionIndex,bool isForce = true);

  /// @brief ZMotionTypeを対応したOTHER_MOTION_INDEXに変換
  /// @param type Zモーションのタイプ
  /// @return 対応したOTHER_MOTION_INDEX
  /// @note 例）Z_MOTION_TYPE_NORMAL→OTHER_MOTION_Z_OUT_NORMAL
  OTHER_MOTION_INDEX convertZMotionTypeToZOutMotion(ZMotionType type);

  /// @brief アニメーション種をOTHER_MOTION_INDEXに変換
  /// @param anmNo アニメーション種
  /// @return 対応したOTHER_MOTION_INDEX
  /// @note anmNoにはZMotionType→OTHER_MOTION_INDEXの順に数字が並んでいるはず
  OTHER_MOTION_INDEX convertAnimationNoToOtherMotion(u32 anmNo);
  /// @brief アニメーション種をZMotionTypeに変換
  /// @param anmNo アニメーション種
  /// @return 対応したZMotionType
  /// @note anmNoにはZMotionType→OTHER_MOTION_INDEXの順に数字が並んでいるはず
  ZMotionType convertAnimationNoToZMotionType(u32 anmNo);

  /// @brief 再生したアニメーション種の保存
  /// @note ZMotionType→OTHER_MOTION_INDEXの順に数字が並ぶようにする
  void saveNowAnimationNo(ZMotionType type);
  void saveNowAnimationNo(OTHER_MOTION_INDEX otherMotionIndex);
  /// @brief 保存したアニメーション種がZモーションかどうか判定
  /// @note saveNowAnimationNo関数と対応
  bool isZMotionAnimationNo(int animationNo);
  /// @brief 保存したアニメーション種が指定したアニメーション種と同じか判定
  bool isSameAnimationNo(ZMotionType type);
  bool isSameAnimationNo(OTHER_MOTION_INDEX otherMotionIndex);

  /// @brief AABBに回転・許容量を反映
  /// @param [out] 反映させるAABB
  void applyAABB(gfl2::math::AABB* AABB) const;
  /// @brief AABBに回転・移動・許容量を反映（回転角度は当たり判定を無視したもの）
  /// @param [out] 反映させるAABB
  void applyIgnoreCollisionAABB(gfl2::math::AABB* AABB) const;

  /// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
  f32 getPrevAnimationFrame(const poke_3d::model::BaseModel* pModel,u32 slotIndex) const;

  /// @brief 角度を写真パラメータで保存できるよう丸める
  /// @param deg 角度
  /// @return 丸めた角度
  int roundDegree(f32 deg) const { return static_cast<int>(deg); }

  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  // 一時ヒープ
  gfl2::heap::HeapBase* m_pWorkHeap;
  // アロケータ
  System::nijiAllocator* m_pNijiAllocator;
  // 描画パイプライン
  DemoLib::Obj::ObjectRenderPipeLine* m_pRenderPipeLine;

  // 追加モーション用ヒープ
  gfl2::heap::HeapBase* m_pMotionHeap;

  // 写真再現パラメータ
  const PhotoParam* m_pPhotoParam;
  // 再現パラメータを使用するかどうか
  b32 m_IsUsePhotoParam;

  // 現在の構図種
  Composition m_Composition;

  // 初期化用
  u32 m_InitSeq;
  // メイン処理用
  u32 m_MainSeq;
  // 終了処理用
  u32 m_EndSeq;

  // キャラモデル
  DemoLib::Obj::Object_Chara* m_pCharaObj;
  poke_3d::model::CharaModelFactory* m_pCharaModelFactory;
  poke_3d::model::DressUpModelResourceManager* m_pDressUpModelResourceManager;
  // 着せ替えパラメータ
  poke_3d::model::DressUpParam m_DressUpParam;
  // 次に読み込む着せ替えパラメータ
  poke_3d::model::DressUpParam m_NextDressUpParam;

  // Zモーションリソースバッファ
  void** m_pZMotionResourceBuffer;

  // 座標
  gfl2::math::Vector3 m_Pos;
  // Y軸角度
  f32 m_DegY;
  // UI上での角度（当たり判定を無視した角度）
  f32 m_IgnoreCollisionDegY;
  // バウンディングボックス
  gfl2::math::AABB m_AABB;
  // ポケモンのバウンディングボックス
  const gfl2::math::AABB* m_pPokemonAABB;
  // めり込み許容量
  f32 m_CollisionPermit;

  // 再生するZモーションタイプ
  ZMotionType m_ZMotionType;
  // 再生中のアニメーション番号（保存用）
  u32 m_NowAnimationNo;

  // 写真パラメータ用前フレーム情報
  struct PrevParam
  {
    f32 mainFrame;

    PrevParam():
      mainFrame(0){}
  } m_PrevParam;

  // 現在の状態
  CHARA_STATE m_State;
  // リクエストタイプ
  REQ_TYPE m_RequestType;
  // リクエストZモーションタイプ
  ZMotionType m_ReqestZMotionType;
  // 汎用変数
  // 各状態に入るたびに0で初期化
  int m_Value;
  // モーション一時停止時の保存値
  int m_SaveValue;

  // 有効かどうか（ポケモンだけ撮るかどうか）
  b32 m_IsEnable;

  // Zモーションの数
  static const u32 NUM_Z_MOTION;
  // Zモーションリソース読み込みテーブル
  static const u32 Z_MOTION_RESOURCE_TABLE[];
  // 男の子用モーションテーブル（Zモーション以外）
  static const u32 OTHER_MOTION_TABLE_MAIL[];
  // 女の子用モーションテーブル（Zモーション以外）
  static const u32 OTHER_MOTION_TABLE_FEMAIL[];
 
  // インモーションと待機モーションのインデックス
  static const u32 Z_IN_MOTION_INDEX;
  static const u32 WAIT_MOTION_INDEX;
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_DRESS_UP_CHARA_H_INCLUDED__