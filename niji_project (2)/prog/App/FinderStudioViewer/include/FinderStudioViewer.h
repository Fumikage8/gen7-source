//======================================================================
/**
 * @file FinderStudioViewer.h
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオビューワメイン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_VIEWER_H_INCLUDED__
#define __FINDER_STUDIO_VIEWER_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// PokeTool
#include "PokeTool/include/PokeModel.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

#include  "../source/Deco/DecoComponent.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)
class Object_GfMdl;
GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioPokemon;
  class FinderStudioDressUpChara;
  class FinderStudioRenderSystem;
  class FinderStudioCameraSystem;
  class FinderStudioBackGround;
  class PokemonDataTable;

  // 撮影テクスチャ用構造体
  struct CaptureTexture
  {
    gfl2::gfx::Texture* pTextureBuffer;
    void* pColorBuffer;
    bool isFinishCapture;

    CaptureTexture():pTextureBuffer(NULL),pColorBuffer(NULL),isFinishCapture(false){}
  };

class FinderStudioViewer
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioViewer); //コピーコンストラクタ＋代入禁止

public:
  /// 初回呼び出し時に使用
  FinderStudioViewer(gfl2::heap::HeapBase* pHeap,const PokeTool::SimpleParam& simpleParam,const poke_3d::model::DressUpParam& dressUpParam);
  /// アプリ復帰・写真再現時に使用
  FinderStudioViewer(gfl2::heap::HeapBase* pHeap,const PhotoParam& photoParam,ViewerMode viewerMode);
  /// 写真再現専用ctor
  FinderStudioViewer(gfl2::heap::HeapBase* pHeap, app::util::AppRenderingManager* pRenderingManager, const PhotoParam& rPhotoParam, const Static::DecorationData& rDecoData);
  virtual ~FinderStudioViewer();

  /// @brief 初期化処理
  /// @return 初期化処理が終わったかどうか
  bool InitFunc();
  /// @brief 終了処理
  /// @return 終了処理が終わったかどうか
  bool EndFunc();
  /// @brief 更新
  void Update();
  /// @brief 描画
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  /// @brief 写真再現開始判定
  /// @return 写真再現処理が開始できるか
  bool IsCanStartPhotoProduct() const;
  /// @brief 写真再現開始
  /// @attention 必ずIsCanStartPhotoProduct関数がtrueのときに呼んでください
  void StartPhotoReproduct(const PhotoParam& photoparam, const Static::DecorationData* pDecoData = NULL);
  /// @brief 写真再現完了
  /// @return 写真再現が完了したかどうか
  /// @attention StartPhotoReproduct関数を呼んだら必ずtrueが帰るまでこの関数を呼んでください
  bool IsFinishPhotoReproduct();

  /// @brief 写真再現パラメータ出力
  /// @param[out] pPhotoParam 写真再現パラメータ
  /// @note 内部の変数にもパラメータは保存される DELETEしない場合は引数NULLで初期化しなおしでもOK
  void SavePhotoParam(PhotoParam* pPhotoParam);

  /// @brief ビューワのパラメータをリセット
  /// @note ポケモンのモーション、位置、角度、トレーナー角度、カメラ情報をリセット
  void ResetViewer();


  //////////////////////////////////////////////////////////////////////////
  // 準備画面で使用する機能
  //////////////////////////////////////////////////////////////////////////
  /// @brief 撮影モード切替
  /// @param captureMode 撮影モード
  void ChangeCaptureMode(CaptureMode captureMode);

  /// @brief ポケモン切り替え
  /// @param pokeSimpleParam ポケモンデータ
  /// @param isDebug trueにすると読み込み中にこの関数を呼んでもアサートで止まりません
  void StartChangePokeModel(const PokeTool::SimpleParam& pokeSimpleParam);
  /// @brief ポケモン切り替えが終わったか
  /// @return 終わったかどうか
  bool IsFinishChangePokeModel();

  /// @brief 背景切り替え
  /// @param bgSetIndex 背景セットインデックス
  void StartChangeBackGround(u32 bgSetIndex);
  /// @brief 背景切り替えが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishChangeBackGround() const;
  /// @brief 時間帯変更
  /// @param timeZone 時間帯
  void ChangeTimeZone(TimeZone timeZone);

  /// @brief トレーナー衣装切り替え
  /// @param dressUpParam 着せ替えデータ
  void StartChangeDressUp(poke_3d::model::DressUpParam dressUpParam);
  /// @brief 衣装切り替えが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishChangeDressUp();

  /// @brief 構図使用可能判定
  /// @param composition 構図の種類
  /// @return 使用可否
  bool IsCanUseComposition(Composition composition) const;
  /// @brief 構図切り替え
  /// @param composition 構図の種類
  /// @param isReset ビューワの状態をその構図でリセットするか（基本trueで大丈夫です）
  /// @return 構図変更が成功したかどうか
  bool ChangeComposition(Composition composition,bool isReset = true);
  /// @brief 適用できる最も上の構図パラメータを適応
  /// @param isReset ビューワの状態をその構図でリセットするか（基本trueで大丈夫です）
  void ApplyDefaultComposition(bool isReset = true);
  /// @brief 現在の構図を取得
  /// @return 構図
  Composition GetComposition() const;

  //////////////////////////////////////////////////////////////////////////
  // 撮影画面で使用する機能
  //////////////////////////////////////////////////////////////////////////

  /// @note モーション再生リクエストと移動・回転リクエストを同じフレームで呼んだ場合、移動・回転セットが優先されます
  ///       移動・回転をした結果、再生しようとしたモーションが衝突によって再生できない可能性があります

  /// @brief Zモーション再生
  /// @param ZMotionType Zモーション種
  /// @return 再生に成功したら（すでに再生中なら失敗）
  bool PlayZMotion(ZMotionType ZMotionType);

  /// @brief Zモーション一時停止
  void StopZMotion();

  /// @brief Zモーションが再生中かどうか
  /// @return 再生中かどうか
  bool IsPlayingZMotion() const;

  /// @brief Zモーションが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishZMotion() const;

  /// @brief トレーナーを待機状態に
  /// @note 強制的に待機モーションへ移行する
  void ChangeTrainerWaitState();

  /// @brief ポケモンモーション再生
  void PlayPokeMotion();

  /// @brief ポケモンモーションが終わったかどうか
  bool IsFinishPokeMotion() const;

  /// @brief ポケモンモーションタイプ変更がリセットせずに行えるかどうか
  /// @param pokeMotionType ポケモンモーションタイプ
  /// @return true 押し出しのみで可能 false リセットしないと不可能
  bool IsCanChangePokeMotionTypeNoReset(PokeMotionType pokeMotionType);

  // @fix GFMMCat[581] モーションタイプ変更時、リセットするかどうかをIsCanChangeMotionTypeNoResetの結果によって渡してもらうようにする
  /// @brief ポケモンモーションタイプ変更
  /// @param pokeMotionType ポケモンモーションタイプ
  /// @param isReset モーションタイプ変更時にリセットを行うか
  /// @return 現在再生しているタイプと同じタイプかどうか
  bool ChangePokeMotionType(PokeMotionType pokeMotionType,bool isReset);

  /// @brief ポケモンモーションタイプを取得
  /// @return ポケモンモーションタイプ
  PokeMotionType GetPokeMotionType() const;

  /// @brief ポケモンモーションタイプが変更済みかどうか
  bool IsChangedPokeMotionType() const;

  /// @brief ポケモンが現在再生できるモーション数の割合を取得
  /// @return 再生できるモーション割合(0 ~ 1)
  f32 GetRatePokeCanPlayMotion() const;

  /// @brief ポケモンが現在再生できるモーション数を取得
  /// @return 再生できるモーション数
  u32 GetNumPokeCanPlayMotion() const;

  /// @brief ポケモンが自動でモーションするかを切り替える
  /// @param true 自動モーションオン  false 自動モーションオフ
  void SetPokeEnableAutoMotion(bool isEnableAutoMotion);

  /// @brief カメラY軸回転（左方向）
  void CameraRotateYLeft();
  /// @brief カメラY軸回転（右方向）
  void CameraRotateYRight();

  /// @brief カメラX軸回転（上方向）
  void CameraRotateXUp();
  /// @brief カメラX軸回転（下方向）
  void CameraRotateXDown();

  /// @brief カメラズームイン
  /// @return これ以上ズームインできるか
  bool CameraZoomIn();
  /// @brief カメラズームアウト
  /// @return これ以上ズームアウトできるか
  bool CameraZoomOut();

  /// @brief カメラ高さ変更（高く）
  /// @return これ以上高くできるか
  bool CameraHeightUp();
  /// @brief カメラ高さ変更（低く）
  /// @return これ以上低くできるか
  bool CameraHeightDown();

  /// @brief 注視点変更（トレーナーに寄る）
  /// @return これ以上寄れるか
  bool CameraFocusToPlayer();
  /// @brief 注視点変更（ポケモンに寄る）
  /// @return これ以上寄れるか
  bool CameraFocusToPokemon();

  /// @brief カメラズーム割合セット
  /// @param カメラズーム割合(0 ~ 1)
  void SetCameraZoomRate(float zoomRate);

  /// @brief カメラ高さ割合セット
  /// @param カメラ高さ割合(0 ~ 1)
  void SetCameraHeightRate(float heightRate);

  /// @brief カメラ注視点割合セット
  /// @param カメラ注視点割合(0 ~ 1)
  void SetCameraFocusRate(float focusRate);

  /// @brief 現在のカメラズーム度合い取得
  /// @return ズーム度合い(0 ~ 1)
  f32 GetCameraZoomRate() const;

  /// @brief 現在のカメラフォーカス取得
  /// @return 0 ~ 1 トレーナーに完全フォーカスなら０ ポケモンに完全フォーカスなら１
  f32 GetCameraFocusRate() const;

  /// @brief 現在のカメラ高さ取得
  /// @return 高さ(0 ~ 1)
  f32 GetCameraHeightRate() const;

  /// @brief 現在のカメラY角度取得
  /// @return Y角度（度）
  f32 GetCameraDegreeY() const;

  /// @brief キャプチャー開始
  /// @param[out] テクスチャバッファ（連射なので６枚分）
  /// @param[out] 写真パラメータ（連射なので６枚分）
  void StartCapture(gfl2::gfx::Texture** textureBuffer,PhotoParam* pPhotoParamArray = NULL);

  /// @brief キャプチャー開始
  /// @param[out] カラーバッファ（連射なので６枚分）
  /// @param[out] 写真パラメータ（連射なので６枚分）
  void StartCapture(void** colorBuffer,PhotoParam* pPhotoParamArray = NULL);

  /// @brief キャプチャーが終わったか
  /// @return 終わったかどうか
  bool IsFinishCapture() const;

  /// @brief ポケモン位置取得(2D)
  /// @return 3D座標から算出した2D（下画面）でのポケモン座標
  gfl2::math::Vector2 GetPokePositionScreen() const;

  /// @brief ポケモン位置取得(3D)
  /// @return 3Dでのポケモン座標
  gfl2::math::Vector3 GetPokePosition() const;

  /// @brief ポケモン位置変更
  /// @param lowerPosition 2D（下画面）でのポケモン座標
  /// @param isLift 持ち上げるかどうか falseで下ろす → 1/31未使用化
  /// @return 変更した結果トレーナーとぶつかったかどうか
  /// @note この座標の更新は次のビューワのUpdate後に反映されます
  bool SetPokePosition(const gfl2::math::Vector2& lowerPosition,bool isLift = false);

  /// @brief ポケモン角度変更（左回り）
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool PokemonRotateLeft();
  /// @brief ポケモン角度変更（右回り）
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool PokemonRotateRight();

  /// @brief ポケモンの角度をセット
  /// @param degY ポケモンのY軸角度(0 <= degY < 360)
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool SetPokeRotate(float degY);

  /// @brief ポケモンの角度を取得
  /// @return 角度
  f32 GetPokemonDegY() const;

  /// @brief ポケモンの当たり判定を無視した角度を取得
  /// @return 当たり判定を無視した角度
  f32 GetPokemonDegYIgnoreCollision() const;

  /// @brief トレーナー角度変更（左回り）
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool TrainerRotateLeft();
  /// @brief トレーナー角度変更（右回り）
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool TrainerRotateRight();

  /// @brief トレーナーの角度をセット
  /// @param degY トレーナーのY軸角度(0 <= degY < 360)
  /// @return 変更した結果ポケモンとぶつかったかどうか
  bool SetTrainerRotate(float degY);

  /// @brief トレーナーの角度を取得
  /// @return 角度
  f32 GetTrainerDegY() const;

  /// @brief トレーナーの当たり判定を無視した角度を取得
  /// @return 当たり判定を無視した角度
  f32 GetTrainerDegYIgnoreCollision() const;

  /// @brief ポケモンのAABB取得
  /// @return 現在のモーションのAABB
  /// @note 移動・回転（UI上）・拡大反映済み
  const gfl2::math::AABB GetPokemonAABB() const;

  /// @brief トレーナーのAABB取得
  /// @return トレーナーの固定AABB
  /// @note 移動・回転（UI上）反映済み
  const gfl2::math::AABB GetTrainerAABB() const;

  /// @brief ポケモンがトレーナーにぶつかっているか判定
  /// @return ぶつかっているかどうか
  /// @note UI上の角度で判定（ビューワ上でぶつかることはありえないため）
  bool IsHitPokemonToTrainer() const;

  /// @brief 撮影する準備ができているか
  /// @return false このフレームで撮影するとまずい
  bool IsPrepareCapture() const;

  // デバッグメニュー用
#if PM_DEBUG
  FinderStudioPokemon* GetPokemon(){ return m_pPokemon; }
  FinderStudioDressUpChara* GetChara(){ return m_pChara; }
  FinderStudioBackGround* GetBackGround(){ return m_pBackGround; }
  FinderStudioRenderSystem* GetRenderSystem(){ return m_pRenderSystem; }
  FinderStudioCameraSystem* GetCameraSystem(){ return m_pCameraSystem; }

  // 現状使用禁止
  void DebugChangeViewerMode(ViewerMode viewerMode){ m_ViewerMode = viewerMode; }

  // エージング用

  /// @brief エージングセットアップ
  void DebugSetupAsing();
  /// @brief 再生可能モーションリスト取得（バトル）
  /// @return 再生可能モーションリストの先頭ポインタ
  /// @note 最大数はPokeTool::MODEL_ANIME_BATTLE_NUM(18)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
  const PokeTool::MODEL_ANIME* DebugGetPokeCanPlayBattleMotionList() const;
  /// @brief 再生可能モーションリスト取得（かわいがり）
  /// @return 再生可能モーションリストの先頭ポインタ
  /// @note 最大数はPokeTool::MODEL_ANIME_KAWAIGARI_NUM(26)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
  const PokeTool::MODEL_ANIME* DebugGetPokeCanPlayKawaigariMotionList() const;
  /// @brief ポケモンモーション指定再生
  /// @return true 再生成功 false 再生失敗
  bool DebugPlayPokeBattleMotion(PokeTool::MODEL_ANIME motionNo);
  bool DebugPlayPokeKawaigariMotion(PokeTool::MODEL_ANIME motionNo);
  /// @brief 眠りモーションがあるか判定
  bool DebugIsPokeCanSleep() const;
  /// @brief 眠る
  /// @return true 再生成功 false 再生失敗
  bool DebugSleepPoke();
  /// @brief 起きれるか判定
  bool DebugIsPokeCanWake() const;
  /// @brief 起きる
  /// @return true 再生成功 false 再生失敗
  bool DebugWakePoke();

  /// @brief かわいがり不可ポケモンチェック
  /// @note MonsNoとFormNoの組み合わせが不正なら（かわいがり不可ポケモンなら）FormNoを0に戻す（メテノは7）
  void DebugCheckMonsNoFormNoCombination(MonsNo monsNo,FormNo* pFormNo){ this->checkMonsNoFormNoCombination(monsNo,pFormNo); }
#endif

private:
  /// @brief 共通初期化
  void setupCommon(gfl2::heap::HeapBase* pHeap);
  /// @brief 通常時初期化
  void setupNormal(const PokeTool::SimpleParam& simpleParam,const poke_3d::model::DressUpParam& dressUpParam);
  /// @brief 再現時初期化
  void setupPhotoReproduction(const PhotoParam& photoParam,ViewerMode viewerMode);

  /// @brief フロアマスク初期化
  void startInitFloorMask();
  bool isFinishInitFloorMask();

  /// @brief 撮影モード切替本体
  /// @param captureMode 撮影モード
  /// @note 全体のリセットは行わない
  void changeCaptureMode(CaptureMode captureMode);

  /// @brief 特定の背景のとき回転処理を行う
  void rotateBackGround();
  void rotateBackGround(gfl2::math::Vector3 pokemonPos);

  /// @brief 通常更新
  void updateNormal();
  /// @brief 写真再現時更新
  void updatePhotoReproduction();

  /// @brief 写真再現処理
  void reproductPhoto();
  /// @brief MonsNoとFormNo不正組み合わせチェック
  void checkMonsNoFormNoCombination(PhotoParam* pPhotoParam);
  void checkMonsNoFormNoCombination(MonsNo monsNo,FormNo* pFormNo);
  /// @brief 着せ替えデータ不正組み合わせチェック
  void checkDressUpCombination(PhotoParam* pPhotoParam);
  /// @brief ポケモンモーション不正組合せチェック
  void checkPokeMotion(PhotoParam* pPhotoParam);

  /// @brief 撮影処理
  void capture();
  /// @brief 写真パラメータ保存
  /// @return 保存できたか
  bool savePhotoParam(PhotoParam* pPhotoParam);

  /// @brief ポケモン再読み込み終了時処理
  void onReloadedPokemon();
  /// @brief 背景再読み込み終了時処理
  void onReloadedBackGround();

  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  // アロケータ
  System::nijiAllocator* m_pNijiAllocator;
  System::nijiAllocator* m_pTrainerNijiAllocator;
  System::nijiAllocator* m_pPokemonNijiAllocator;
  System::nijiAllocator* m_pFloorMaskNijiAllocator;

  // ローカルヒープ
  gfl2::heap::HeapBase* m_pPokeHeap;
  gfl2::heap::HeapBase* m_pTrainerHeap;
  gfl2::heap::HeapBase* m_pTempHeap;
  gfl2::heap::HeapBase* m_pTempHeapSmall;
  gfl2::heap::HeapBase* m_pBgHeap;
  gfl2::heap::HeapBase* m_pFloorMaskHeap;
  gfl2::heap::HeapBase* m_pPokeDataTableHeap;

  // 初期化用
  u32 m_InitSeq;
  // 終了処理用
  u32 m_EndSeq;

  // ポケモンモデルシステム
  PokeTool::PokeModelSystem* m_pPokeModelSystem;
  // キャラモデルファクトリー
  poke_3d::model::CharaModelFactory* m_pCharaModelFactory;
  poke_3d::model::DressUpModelResourceManager* m_pDressUpModelResourceManager;

  // ポケモンデータテーブル
  PokemonDataTable* m_pPokeDataTable;

  // ポケモン
  FinderStudioPokemon* m_pPokemon;
  // 主人公キャラ
  FinderStudioDressUpChara* m_pChara;
  // 背景システム
  FinderStudioBackGround* m_pBackGround;

  /// @note フロアマスクはメガゲンガーでしか使用していないかもしれないが一応両方使用
  ///       有泉さんに確認済み
  // フロアマスク
  DemoLib::Obj::Object_GfMdl* m_pFloorMaskModel;
  // エッジマスク
  DemoLib::Obj::Object_GfMdl* m_pEdgeMaskModel;

  // カメラシステム
  FinderStudioCameraSystem* m_pCameraSystem;
  // 描画システム
  FinderStudioRenderSystem* m_pRenderSystem;

  // 初期化パラメータ
  PokeTool::SimpleParam m_simpleParam;
  poke_3d::model::DressUpParam m_dressUpParam;

  // ビューワモード
  ViewerMode m_ViewerMode;
  // 撮影モード
  CaptureMode m_CaptureMode;

  // 現在の構図
  Composition m_Composition;

  // 写真再現パラメータ
  // 各クラス初期化時に使用
  PhotoParam m_PhotoParam;
  // 再現パラメータを使用するかどうか
  bool m_IsUsePhotoParam;

  // 写真再現リクエストフラグ
  int m_SeqPhotoProduct;

  // 連写枚数
  static const u32 BURST_CAPTURE_NUM = 6;

  // 撮影用テクスチャ
  CaptureTexture m_CaptureTexture[BURST_CAPTURE_NUM];
  // 連写中フラグ
  bool m_BurstCaptureFlag;
  // 何枚目まで撮ったか
  u32 m_BurstCaptureNum;
  // 次撮るまでのカウント
  u32 m_BurstCaptureCnt;

  // 撮影時の写真パラメータを格納する変数のポインタ
  PhotoParam* m_pPhotoParamArray[BURST_CAPTURE_NUM];

  /*  deco  */
  Static::DecorationData  m_DecoData;
  Deco::DecoComponent*    m_pDecoComponent;
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_VIEWER_H_INCLUDED__