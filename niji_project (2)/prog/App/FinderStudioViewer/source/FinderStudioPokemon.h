//======================================================================
/**
 * @file FinderStudioPokemon.h
 * @date 2016/09/01
 * @author harada_takumi
 * @brief ファインダースタジオポケモン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_POKEMON_H_INCLUDED__
#define __FINDER_STUDIO_POKEMON_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// PokeTool
#include "PokeTool/include/PokeModel.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

// FinderStudioViewer
#include "FinderStudioPokemonAABB.h"


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)
  class Object_Poke;
  class ObjectRenderPipeLine;
GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioViewer;
  class FinderStudioCameraSystem;
  struct PhotoParam;
  class PokemonDataTable;

class FinderStudioPokemon
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioPokemon); //コピーコンストラクタ＋代入禁止

public:
  struct Desc
  {
    gfl2::heap::HeapBase* pHeap;
    gfl2::heap::HeapBase* pWorkHeap;
    System::nijiAllocator* pAllocator;
    DemoLib::Obj::ObjectRenderPipeLine* pObjectRenderPipeLine;
    PokeTool::PokeModelSystem* pPokeModelSystem;
    PokemonDataTable* pPokeDataTable;
    // pSimpleParamかpPhotoParamかどちらかでよい
    PokeTool::SimpleParam* pSimpleParam;
    const PhotoParam* pPhotoParam;
    bool isUsePhotoParam;

    Desc():
      pHeap(NULL)
      ,pWorkHeap(NULL)
      ,pAllocator(NULL)
      ,pObjectRenderPipeLine(NULL)
      ,pPokeModelSystem(NULL)
      ,pPokeDataTable(NULL)
      ,pSimpleParam(NULL)
      ,pPhotoParam(NULL)
      ,isUsePhotoParam(false){}
  };

  FinderStudioPokemon(const Desc& desc);
  virtual ~FinderStudioPokemon();

  /// @brief 初期化処理
  /// @note ポケモンモデルのロードからパイプラインへの登録まで全て行う
  /// @return trueなら初期化処理終了
  bool InitFunc();
  /// @brief 終了処理
  /// @return trueなら終了処理終了 
  bool EndFunc();
  /// @brief 更新
  void Update();

  /// @brief トレーナーのバウンディングボックス登録
  void RegistTrainerAABB(const gfl2::math::AABB* pAABB){ m_pPlayerAABB = pAABB; }
  /// @brief トレーナーのバウンディングボックスリセット
  void ResetTrainerAABB(){ m_pPlayerAABB = NULL; }

  /// @brief 写真再現パラメータの出力が可能かどうか
  /// @return true 可能 false 不可能
  bool IsCanGetPhotoParam();
  /// @brief 再現用パラメータ出力
  /// @param[out] pPhotoParam 再現用パラメータ 必要な情報だけを格納する
  void GetPhotoParamPokemon(PhotoParam* pPhotoParam);

  /// @brief 写真再現パラメータ適用（ポケモン見た目データ）
  /// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
  void ApplyPhotoParamSimplePokeParam(const PhotoParam& photoParam);
  /// @brief 写真再現パラメータ適用（その他データ）
  /// @return リクエストした値で配置できるか
  bool ReqApplyPhotoParamPokemonOther(const PhotoParam& photoParam);

  /// @brief アニメーション更新のオンオフを切り替える
  /// @param true オン false オフ
  /// @note 写真再現後の更新のため
  void SetIsAnimationUpdate(bool isUpdate);
  /// @brief 自動目パチのオンオフを切り替える
  /// @param true オン false オフ
  void SetIsAutoBlink(bool isBlink);

  /// @brief ポケモン位置・角度リセット
  /// @note 現在のモーションタイプでリセット
  void ResetPositionRotate();
  /// @brief ポケモン位置・角度リセット
  /// @param motionType どのモーションタイプでリセットするか
  void ResetPositionRotate(PokeMotionType motionType);

  /// @brief ポケモンリセット位置・角度取得
  /// @param[out] pPos 座標
  /// @param[out] pDegY Y軸角度
  /// @param[out] motionType モーションタイプ
  void GetResetPosistionRotate(gfl2::math::Vector3* pPos,f32* pDegY,PokeMotionType motionType);

  /// @brief 撮影モード切替時処理
  void OnChangeCaptureMode(CaptureMode captureMode);

  /// @brief 指定した距離で現在の構図に配置できるか
  /// @param distance トレーナーとの距離
  /// @return 配置できるか
  bool IsCanComposit(f32 distance);
  /// @brief 指定した距離で指定した構図に配置できるか
  /// @param composition 構図の種類
  /// @param distance トレーナーとの距離
  /// @return 配置できるか
  bool IsCanComposit(Composition composition,f32 distance);

  /// @brief 構図用パラメータ適用
  /// @param composition 構図の種類
  /// @param distance トレーナーとの距離
  void ApplyComposition(Composition composition,u32 distance);

  /// @brief ポケモンモデル再ロード（ポケモンパラメータは内部のものを使用）
  /// @param isDebug デバッグからの呼び出しか
  void StartReloadModel(bool isDebug = false);
  /// @brief ポケモンモデル再ロード
  /// @param pokeSimpleParam ポケモンデータ
  /// @param isDebug デバッグからの呼び出しか
  void StartReloadModel(const PokeTool::SimpleParam& pokeSimpleParam,bool isDebug = false);
  /// @brief 再ロードが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishReloadModel() const;

  /// @brief 時間帯によるカラー変更
  /// @param timeZone 時間帯
  /// @note 仕様削除
  void SetConstantColorByTime(TimeZone timeZone);

  /// @brief ライト方向変更
  /// @param lightDir ライト方向
  void SetLightDirection(const gfl2::math::Vector3& lightDir);

  /// @brief 座標取得
  const gfl2::math::Vector3& GetPos() const { return m_Pos; }
  /// @brief リクエスト中座標取得
  const gfl2::math::Vector3& GetReqPos() const { return m_ReqPos; }

  /// @brief AABB取得
  const gfl2::math::AABB* GetAABB() const { return &m_AABB; }

  /// @brief モーション再生
  /// @note 仕様にあるモーションリストの中からランダムに再生する
  void PlayMotion();

  /// @brief モーションが終わったかどうか
  bool IsFinishMotion() const;

  /// @brief モーションタイプ変更がリセットせずに行えるかどうか
  /// @param motionType モーションタイプ
  /// @return true 押し出しのみで可能 false リセットしないと不可能
  bool IsCanChangeMotionTypeNoReset(PokeMotionType motionType);

  /// @brief モーションタイプ変更
  /// @param motionType モーションタイプ
  /// @return 現在再生しているタイプと違うタイプかどうか
  /// @note その場で待機モーションも変更させる
  bool ChangeMotionType(PokeMotionType motionType);

  // @fix GFMMCat[581] モーションタイプ変更時、リセットするかどうかをIsCanChangeMotionTypeNoResetの結果によって渡してもらうようにする
  /// @brief モーションタイプ変更
  /// @param motionType モーションタイプ
  /// @param isReset モーションタイプ変更時にリセットを行うか
  /// @return 現在再生しているタイプと違うタイプかどうか
  /// @note その場で待機モーションも変更させる
  bool ChangeMotionType(PokeMotionType motionType,bool isReset);

  /// @brief ポケモンモーションタイプを取得
  /// @return ポケモンモーションタイプ
  PokeMotionType GetPokeMotionType() const{ return m_NowMotionType; }

  /// @brief モーションタイプ変更が完了したか
  /// @return 完了したか
  bool IsFinishChangeMotionType() const;

  /// @brief ポケモンが現在再生できるモーション数の割合を取得
  /// @return 再生できるモーション割合(0 ~ 1)
  f32 GetRateCanPlayMotion() const;

  /// @brief 現在再生できるモーション数を取得
  /// @return 再生できるモーション数
  u32 GetNumCanPlayMotion() const;

  /// @brief 自動でモーションするかを切り替える
  /// @param true 自動モーションオン  false 自動モーションオフ
  void SetEnableAutoMotion(bool isEnableAutoMotion);

  /// @brief ポケモン位置取得(2D)
  /// @return 3D座標から算出した2D（下画面）でのポケモン座標
  gfl2::math::Vector2 GetPokePositionScreen() const;

  /// @brief ポケモン位置取得(3D)
  /// @return ポケモン3D座標
  gfl2::math::Vector3 GetPokePosition() const{ return m_Pos; }

  /// @brief ポケモン位置変更
  /// @param lowerPosition 2D（下画面）でのポケモン座標
  /// @param isLift 持ち上げるかどうか falseで下ろす
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool SetPokePosition(const gfl2::math::Vector2& lowerPosition,bool isLift = false);

  /// @brief ポケモン角度変更（左回り）
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool RotateLeft();
  /// @brief ポケモン角度変更（右回り）
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool RotateRight();

  /// @brief 角度をセット
  /// @param degY Y軸角度(0 <= degY < 360)
  /// @return 変更した結果トレーナーとぶつかったかどうか
  bool SetRotate(float degY);

  /// @brief ポケモンがトレーナーにぶつかっているか判定
  /// @return ぶつかっているかどうか
  /// @note UI上の角度で判定（ビューワ上でぶつかることはありえないため）
  bool IsHitPokemonToTrainerUI() const;

  /// @brief Y軸角度を取得
  /// @return 角度
  f32 GetDegY() const { return m_DegY; }

  /// @brief 当たり判定を無視したY軸角度を取得
  /// @return Y軸角度
  f32 GetDegYIgnoreCollision() const { return m_IgnoreCollisionDegY; }

  /// @brief 当たり判定を無視したAABBを取得
  /// @return AABB
  /// @note 待機モーションマージ済み
  const gfl2::math::AABB GetAABBIgnoreCollision() const;

  /// @brief ポケモン見た目データ取得
  const PokeTool::SimpleParam& GetSimpleParam() const { return m_PokeSimpleParam; }

  /// @brief ポケモンモデル最大数取得
  static u32 GetNumPokeModel(){ return NUM_MAX_POKE_MODEL; }

  // ポケモンの状態（モード）
  enum POKE_STATE{
    POKE_STATE_NULL,  /// 何もしない
    POKE_STATE_WAIT,  /// 待機中
    POKE_STATE_WAIT_MOTION, /// 待機モーション中
    POKE_STATE_PAUSE_MOTION, /// ポーズモーション再生中
    POKE_STATE_SLEEP,   /// 眠り中
    POKE_STATE_WAKE,   /// 目覚め中
    POKE_STATE_MAX
  };

  // リクエストタイプ
  enum REQ_TYPE{
    REQ_TYPE_NULL,
    REQ_TYPE_PLAY_MOTION,
    REQ_TYPE_CHANGE_MOTION_TYPE,
    REQ_TYPE_LIFT_UP,
    REQ_TYPE_RESET,
    REQ_TYPE_PHOTO_REPRODUCT,
    REQ_TYPE_MAX
  };

  /// @brief カメラシステム登録
  /// @note ポケモン移動時に使用
  void RegistCameraSystem(FinderStudioCameraSystem* pCameraSystem){ m_pCameraSystem = pCameraSystem; }

  /// @brief 読み込みが終わったフレームかどうか返す
  bool IsLoadEndFrame() const { return m_LoadEndFrameFlag; }

  // ヒープサイズ
  static const u32 POKE_MOTION_HEAP_SIZE;
  static const u32 POKE_MOTION_AABB_HEAP_SIZE;

  // デバッグ用
#if PM_DEBUG
  void DebugDraw(gfl2::gfx::CtrDisplayNo displayNo);

  void SetDebugIsEnableMotionBlend(bool isEnable){ m_DebugIsEnableMotionBlend = isEnable; }
  void SetDebugIsVisibleBoundingBox(bool isVisible){ m_DebugIsVisibleBoundingBox = isVisible; }
  void SetDebugIsVisibleBoundingBoxUI(bool isVisible){ m_DebugIsVisibleBoundingBoxUI = isVisible; }
  void SetDebugIsIgnoreWaistXZ(bool isIgnore){ m_DebugIsIgnoreWaistXZ = isIgnore; }
  void SetDebugIsFullSize(bool isFullSize){ m_IsFullSize = isFullSize; }

  /// @brief モーション連続再生デバッグ開始
  void StartDebugAutoMotion(u32 monsNo = 1);
  /// @brief モーション連続再生デバッグ終了
  void EndDebugAutoMotion();

  // 一時停止
  void Shot();
  // 一時停止解除
  void FreeShot();

  void ChangeAnimation(PokeMotionType type,PokeTool::MODEL_ANIME anime,bool isForce = true){ changeAnimation(type,anime,isForce); }
  void DebugTransitionState(POKE_STATE state){ transitionState(state); }

  u32 GetNowAnmNo() const { return m_NowAnimationNo; }
  void SetCollisionParmit(f32 collisionPermit){ m_CollisionPermit = collisionPermit; }

  PokemonAABB* GetPokemonAABB(){ return &m_PokeAABB; }

  int DebugGetCompositionDistance(){ return m_CompositionDistance; }
  void DebugSetCompositionDistance(int compositionDistance){ m_CompositionDistance = compositionDistance; }

  // デバッグ用即眠り
  void DebugForceSleep();

  // デバッグ用モーション再生可能関数呼び出し
  bool DebugIsCanPlayMotionDataTable(PokeMotionType type,PokeTool::MODEL_ANIME anime){ return isCanPlayMotionDataTable(type,anime); }
  bool DebugIsCanPlayMotionExist(PokeMotionType type,PokeTool::MODEL_ANIME anime){ return isCanPlayMotionExist(type,anime); }
  bool DebugIsCanPlayMotionHit(PokeMotionType type,PokeTool::MODEL_ANIME anime){ return isCanPlayMotionHit(type,anime); }

  // アジャスト
  f32 DebugGetAdjustScale();
  f32 DebugGetAdjustHeight();
  void DebugSetAdjustHeight(f32 adjustHeight);

  // ポケモンモデルモーションエージング用

  /// @brief エージングフラグ切り替え
  void DebugSetEnableAsing(bool enable){ m_DebugEnableAsing = enable; }
  /// @brief 再生可能モーションリスト取得（バトル）
  /// @return 再生可能モーションリストの先頭ポインタ
  /// @note 最大数はPokeTool::MODEL_ANIME_BATTLE_NUM(18)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
  const PokeTool::MODEL_ANIME* DebugGetCanPlayBattleMotionList() const { return m_DebugCanPlayBattleMotionList; }
  /// @brief 再生可能モーションリスト取得（かわいがり）
  /// @return 再生可能モーションリストの先頭ポインタ
  /// @note 最大数はPokeTool::MODEL_ANIME_KAWAIGARI_NUM(26)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
  const PokeTool::MODEL_ANIME* DebugGetCanPlayKawaigariMotionList() const { return m_DebugCanPlayKawaigariMotionList; }
  /// @brief ポケモンモーション指定再生
  /// @return true 再生成功 false 再生失敗
  bool DebugPlayBattleMotion(PokeTool::MODEL_ANIME motionNo);
  bool DebugPlayKawaigariMotion(PokeTool::MODEL_ANIME motionNo);
  /// @brief 眠りモーションがあるか判定
  bool DebugIsCanSleep() const;
  /// @brief 眠る
  /// @return true 再生成功 false 再生失敗
  bool DebugSleep();
  /// @brief 起きれるか判定
  bool DebugIsCanWake() const;
  /// @brief 起きる
  /// @return true 再生成功 false 再生失敗
  bool DebugWake();
#endif

  void SetVisible(bool isVisible);
  void SetMoveRange(f32 moveRange){ m_MoveRange = moveRange; }

private:
  // モデルロード
  void startLoadModel();
  bool isFinishLoadModel();
  // バトル以外のモーションのロード
  void startLoadMotion();
  bool isFinishLoadMotion();
  // モデル読み込み後の初期化
  void setup();
  // 一部のポケモンにAdjustHeightをセット
  void setupAdjustHeight();
  // AABBの設定
  void setupAABB();

  /// @brief アニメーション更新前のパラメータ保存
  void savePrevParam();

  // 更新メイン処理
  void updateMain();

  // ポケモンの状態遷移
  // 遷移した際の初期化も行う
  void transitionState(POKE_STATE state);
  // 外部からのリクエスト
  void request(REQ_TYPE reqType){ m_RequestType = reqType; }
  void requestPos(const gfl2::math::Vector3& pos);
  void requestDegY(f32 degY);
  void requestIgnoreCollisionDegY(f32 degY);
  void requestMotionType(PokeMotionType motionType){ m_ReqMotionType = motionType; }
  void requestPhotoParam(PhotoParam photoParam){ m_ReqPhotoParam = photoParam; }
  // リクエスト処理
  void acceptPosDegY(); // 移動・回転リクエストを反映させる
  // 各状態関数
  void updateWait();
  void updateWaitMotion();
  void updatePause();
  void updateSleep();
  void updateWake();

  /// @brief 目を覚ます
  /// @param nextState 目を覚ました後のリクエスト
  void wake(REQ_TYPE nextReq);

  /// @brief モーションタイプ変更後の座標を取得
  gfl2::math::Vector3 getPosChangeMotionType(PokeMotionType motionType) const;

  /// @brief モーション再生
  /// @return 再生に成功したかどうか
  /// @note 仕様にあるモーションリストの中からランダムに再生する
  bool playMotion();
  /// @brief バトルモーション再生
  /// @return 再生に成功したかどうか
  /// @note 仕様にあるバトルのモーションリストの中からランダムに再生する
  bool playMotionBattle();
  /// @brief かわいがりモーション再生
  /// @return 再生に成功したかどうか
  /// @note 仕様にあるかわいがりのモーションリストの中からランダムに再生する
  bool playMotionKawaigari();

  /// @brief アニメーション変更関数
  /// @param type アニメーションタイプ
  /// @param anime アニメーションNo
  /// @param isForce 同じモーションだったときに最初から再生するか
  /// @return true:変更成功 false:変更失敗
  bool changeAnimation(PokeMotionType type,PokeTool::MODEL_ANIME anime,bool isForce = true);
  // 現在使用予定無し
  bool changeAnimationSmooth(PokeMotionType type,PokeTool::MODEL_ANIME anime,int smoothFrame = 10);

  /// @brief 現在のアニメーションタイプに応じた待機モーションを再生
  /// @param isForce 同じモーションだった場合に１フレーム目から再生するか
  /// @return 再生に成功したか
  bool changeAnimationWait(PokeMotionType motionType,bool isForce = false);

  // 拡縮更新・反映
  void updateScale();
  // 回転更新・反映
  void updateRotate();
  // 座標更新・反映
  void updatePosition();

  /// @brief AABBに回転・移動・許容量を反映
  /// @param [out] 反映させるAABB
  void applyAABB(gfl2::math::AABB* AABB) const;
  /// @brief AABBに回転・移動・許容量を反映（回転角度は当たり判定を無視したもの）
  /// @param [out] 反映させるAABB
  void applyIgnoreCollisionAABB(gfl2::math::AABB* AABB) const;
  /// @brief AABBに拡大・回転・移動を反映
  /// @param [out] 反映させるAABB
  void applyAABB(gfl2::math::AABB* AABB,gfl2::math::Vector3 pos,f32 degY,f32 scale) const;

  /// @brief 指定したタイプで現在再生できるモーション数を取得
  /// @return 再生できるモーション数
  u32 getNumCanPlayMotion(PokeMotionType type) const;
  /// @brief 指定したタイプで現在再生できるモーション数の最大を取得
  /// @return 再生できるモーション数の最大
  /// @note 存在しないモーション、データテーブルで使用不可に指定されたモーションは除く
  u32 getNumCanPlayMotionMax(PokeMotionType type) const;
  /// @brief 指定したモーションが再生できるかどうか
  bool isCanPlayMotion(PokeMotionType type,PokeTool::MODEL_ANIME anime) const;

  /// @brief データテーブルでのモーション使用可否判定
  bool isCanPlayMotionDataTable(PokeMotionType type,PokeTool::MODEL_ANIME anime) const;
  /// @brief モーションが存在するかどうか判定
  bool isCanPlayMotionExist(PokeMotionType type,PokeTool::MODEL_ANIME anime) const;
  /// @brief モーションがトレーナーにめり込むかどうか判定
  bool isCanPlayMotionHit(PokeMotionType type,PokeTool::MODEL_ANIME anime) const;

  /// @brief ポケモンがポーズするときにそのモーションのAABBがめり込んでいないかチェック
  bool isHitMotionAABB(gfl2::math::AABB& AABB) const;

  /// @brief 指定した構図、距離での座標、角度を取得
  /// @param [out] pos 取得した座標
  /// @param [out] degY 取得した角度
  /// @param composition 構図
  /// @param distance 距離
  void getCompositionPosDegY(gfl2::math::Vector3* pVec,f32* pDegY,Composition composition,int distance,PokeMotionType motionType) const;

  /// @brief 指定した座標に移動できるかどうかチェック
  /// @param pos 座標
  /// @param degY 角度
  /// @param isUseNowMotion 現在のモーションで判定するか
  /// @param motionType isUseNowMotionがfalseのときに使用
  /// @param motionNo isUseNowMotionがfalseのときに使用
  bool isCanMovePos(
    const gfl2::math::Vector3& pos,
    f32 degY,
    bool isUseNowMotion = true,
    PokeMotionType motionType = POKE_MOTION_TYPE_BATTLE,
    PokeTool::MODEL_ANIME motionNo = PokeTool::MODEL_ANIME_BT_WAIT_A);

  /// @brief 自動再生カウントをクリア
  void resetAutoMotionCnt(bool isResetSleepCnt);

  // リクエストで処理するようにしたのでprivateへ
  void applyPhotoParamPokemonOther(const PhotoParam& photoParam);

  /// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
  f32 getPrevAnimationFrame(const poke_3d::model::BaseModel* pModel,u32 slotIndex) const;

  /// @brief 座標を写真パラメータで保存できるよう丸める
  /// @param position 座標
  /// @return 丸めた座標
  gfl2::math::Vector3 roundPosition(gfl2::math::Vector3 position) const;

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
  // モーションAABB用ヒープ
  gfl2::heap::HeapBase* m_pMotionAABBHeap;

  // 写真再現パラメータ
  const PhotoParam* m_pPhotoParam;
  // 再現パラメータを使用するかどうか
  b32 m_IsUsePhotoParam;

  // 初期化用
  u32 m_InitSeq;
  // メイン処理用
  u32 m_MainSeq;
  // 終了処理用
  u32 m_EndSeq;

  // ポケモン見た目情報
  PokeTool::SimpleParam m_PokeSimpleParam;

  // ポケモンモデル
  DemoLib::Obj::Object_Poke* m_pPokeObj;
  // モデルシステム（メインから持ってくる）
  PokeTool::PokeModelSystem* m_pPokeModelSystem;

  // カメラシステム（メインから持ってくる）
  FinderStudioCameraSystem* m_pCameraSystem;

  // かわいがりモーションバッファ
  void* m_pKawaigariMotionBuffer;
  // かわいがりモーション分のリソースノード
  gfl2::renderingengine::scenegraph::resource::ResourceNode** m_pKawaigariAnimationResourceNode;

  // ポケモンデータテーブル
  PokemonDataTable* m_pPokeDataTable;

  // 撮影モード
  CaptureMode m_CaptureMode;
  // 現在の構図種
  Composition m_Composition;
  // 構図で使用するポケモンとトレーナーの余白
  int m_CompositionDistance;

  // 座標
  gfl2::math::Vector3 m_Pos;
  // アニメーションのデフォルトの移動ベクトル
  gfl2::math::Vector3 m_DefaultVec;
  // Y軸角度
  f32 m_DegY;
  // 当たり判定を無視したY軸角度（UI上の角度）
  f32 m_IgnoreCollisionDegY;
  // 現在のアニメタイプ
  PokeMotionType m_NowMotionType;
  // 現在のアニメーション番号
  int m_NowAnimationNo;
  // アニメーションスピード保存用
  f32 m_SaveAnimationSpeed;

  // 現在の状態
  POKE_STATE m_State;
  // 汎用変数
  // 各状態に入るたびに0で初期化
  int m_Value;
  // リクエストタイプ
  REQ_TYPE m_RequestType;
  // リクエストモーションタイプ
  PokeMotionType m_ReqMotionType;
  // リクエスト座標
  gfl2::math::Vector3 m_ReqPos;
  bool m_IsReqPos;
  // リクエスト角度
  f32 m_ReqDegY;
  bool m_IsReqDegY;
  f32 m_ReqIgnColDegY;
  bool m_IsReqIgnColDegY;
  // リクエスト写真パラメータ
  PhotoParam m_ReqPhotoParam;

  // ランダム時間再生が有効か
  b32 m_IsEnableAutoMotion;
  // ランダム時間再生最大カウント
  int m_WaitMotionCntMax;
  // ランダム時間再生用カウント
  int m_WaitMotionCnt;
  // 眠るモーション用カウント
  int m_SleepMotionCnt;
  // 目覚め用状態保持変数
  REQ_TYPE m_NextReq;

  // ポケモンAABB管理クラス
  PokemonAABB m_PokeAABB;
  // バウンディングボックス
  gfl2::math::AABB m_AABB;
  // プレイヤーバウンディングボックス
  const gfl2::math::AABB* m_pPlayerAABB;
  // めり込み許容量
  f32 m_CollisionPermit;

  // 写真パラメータ用前フレーム情報
  struct PrevParam
  {
    f32 mainFrame;
    f32 loopFrame1;
    f32 loopFrame2;
    f32 loopFrame3;
    f32 loopFrame4;

    PrevParam():
      mainFrame(0),
      loopFrame1(0),
      loopFrame2(0),
      loopFrame3(0),
      loopFrame4(0){}
  } m_PrevParam;

  // 再読み込み終了フレームフラグ
  // 再読み込みが終わったフレームのみtrueになる
  bool m_LoadEndFrameFlag;

  // 移動範囲係数
  f32 m_MoveRange;

  // 等身大フラグ
  bool m_IsFullSize;

  // 持ち上げ時Y座標
  static const f32 LIFT_Y_POS;

  // ポケモンモデル最大数
  static const u32 NUM_MAX_POKE_MODEL;

  // 使用モーション数
  static const u32 NUM_USE_MOTION_BATTLE;
  static const u32 NUM_USE_MOTION_KAWAIGARI;

  // 使用モーションテーブル
  static const PokeTool::MODEL_ANIME BATTLE_MOTION_TABLE[];
  static const PokeTool::MODEL_ANIME KAWAIGARI_MOTION_TABLE[];

  // 待機中モーション再生までの最低フレーム数
  static const u32 WAIT_MOTION_FRAME_MIN;
  // 待機中モーション再生までの最高フレーム数
  static const u32 WAIT_MOTION_FRAME_MAX;
  // 眠りモーション再生までのフレーム数
  static const u32 SLEEP_MOTION_FRAME_MAX;

  // 以下、デバッグ用
#if PM_DEBUG
  // モーションブレンド（デバッグ用）
  b32 m_DebugIsEnableMotionBlend;
  // バウンディングボックスを表示するか
  b32 m_DebugIsVisibleBoundingBox;
  // UI上のバウンディングボックスを表示するか
  b32 m_DebugIsVisibleBoundingBoxUI;
  // モーションのWaistのXZ移動量を無視するかどうか
  b32 m_DebugIsIgnoreWaistXZ;
  // ポケモンモーション自動再生デバッグ中かどうか
  b32 m_DebugIsDebugAutoMotion;
  u32 m_DebugAutoMotionMonsNo;
  u32 m_DebugAutoMotionButuriNo;
  // 撮影中かどうか
  b32 m_DebugIsShot;

  // ポケモンモデルモーションエージング用
  
  // エージング有効フラグ
  bool m_DebugEnableAsing;
  // ポケモンが再生できるモーションリスト
  PokeTool::MODEL_ANIME m_DebugCanPlayBattleMotionList[PokeTool::MODEL_ANIME_BATTLE_NUM];
  PokeTool::MODEL_ANIME m_DebugCanPlayKawaigariMotionList[PokeTool::MODEL_ANIME_KAWAIGARI_NUM];
  // 初期化
  void debugCanPlayMotionListInit();
  // 再生できるモーションを格納する
  void debugCanPlayMotionListStore();
#endif

};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_POKEMON_H_INCLUDED__