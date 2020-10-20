//======================================================================
/**
 * @file FinderStudioBackGround.h
 * @date 2016/09/21
 * @author harada_takumi
 * @brief ファインダースタジオ背景処理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_BACK_GROUND_H_INCLUDED__
#define __FINDER_STUDIO_BACK_GROUND_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// btl
#include "Battle/BackGround/include/BgSystem.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  class FinderStudioRenderPipeLine;
  struct PhotoParam;

class FinderStudioBackGround
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioBackGround); //コピーコンストラクタ＋代入禁止

public:
  struct Desc
  {
    gfl2::heap::HeapBase* pHeap;
    gfl2::heap::HeapBase* pWorkHeap;
    FinderStudioRenderPipeLine* pFinderStudioRenderPipeLine;
    PhotoParam* pPhotoParam;
    bool isUsePhotoParam;

    Desc():
      pHeap(NULL)
      ,pWorkHeap(NULL)
      ,pFinderStudioRenderPipeLine(NULL)
      ,pPhotoParam(NULL)
      ,isUsePhotoParam(false){}
  };

  FinderStudioBackGround(const Desc& desc);
  virtual ~FinderStudioBackGround();

  // 初期化処理
  bool InitFunc();
  // 終了処理
  bool Terminate();
  // 更新
  void Update();

  /// @brief ビュー行列を登録
  /// @param pViewMtx ビュー行列
  void RegistViewMtx(const gfl2::math::Matrix34* pViewMtx){ m_pViewMtx = pViewMtx; }
  /// @brief ビュー・プロジェクション行列を登録（遠景・天球用）
  /// @param pProjMtx プロジェクション行列
  void RegistFarProjMtx(const gfl2::math::Matrix44* pProjMtx){ m_pFarProjMtx = pProjMtx; }
  void RegistSkyProjMtx(const gfl2::math::Matrix44* pProjMtx){ m_pSkyProjMtx = pProjMtx; }

  /// @brief 写真再現パラメータの出力が可能かどうか
  /// @return true 可能 false 不可能
  bool IsCanGetPhotoParam();
  /// @brief 再現用パラメータ出力
  /// @param[out] 再現用パラメータ 必要な情報だけを格納する
  void GetPhotoParamBg(PhotoParam* pPhotoParam) const;

  /// @brief 写真再現パラメータ適用（背景セットインデックス・時間帯）
  /// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
  void ApplyPhotoParamBgSetIndexTimeZone(const PhotoParam& photoParam);
  /// @brief 写真再現パラメータ適用（その他データ）
  void ApplyPhotoParamBgOther(const PhotoParam& photoParam);

  /// @brief アニメーション更新のオンオフを切り替える
  /// @param true オン false オフ
  /// @note 写真再現後の更新のため
  void SetIsAnimationUpdate(bool isUpdate);

  /// @brief 背景セット再ロード（インデックスは内部のデータを使用）
  void StartReload();
  /// @brief 背景セット再ロード
  /// @param bgSetIndex 背景セットインデックス
  void StartReload(u32 bgSetIndex);
  // 個別版も残しておく
  void StartReload(int groundType,int nearType,int farType);
  /// @brief 再ロードが終わったかどうか
  /// @return 終わったかどうか
  bool IsFinishReloadModel() const;

  /// @brief 戦闘背景の時間帯指定
  /// @param timeZone 時間帯
  void SetTimeZone(TimeZone timeZone);
  // 個別版も残しておく
  void SetTime(int hour,int minute,int second);
  /// @brief 戦闘背景の時間帯取得
  /// @return 時間帯
  TimeZone GetTimeZone() const { return m_TimeZone; }

  /// @brief 特定の背景の回転処理
  /// @param pokemonPos ポケモンの座標
  /// @param cameraPos カメラの座標
  void RotateBackGround(const f32 cameraLength,const gfl2::math::Vector3 pokemonPos);
  /// @brief 背景の回転リセット
  void ResetRotateBackGround();
  /// @brief 現在の背景が回転させる背景かどうか
  bool IsRotateBackGround();

  /// @brief 読み込みが終わったフレームかどうか返す
  bool IsLoadEndFrame(){ return m_LoadEndFrameFlag; }

  /// @breif 現在の時間アニメがインアウトかどうか取得
  bool IsNowTimeAnimeInOut() const;

  // ヒープサイズ
  static const u32 BG_TEMP_HEAP_SIZE;

#if PM_DEBUG
  /// @brief 撮影する
  /// @note Mainから呼ばれる処理
  void Shot();

  /// @brief 撮影状態から解除する
  /// @note Mainから呼ばれる処理
  void FreeShot();

  /// @brief スカイボックス表示・非表示切り替え
  /// @param true:表示 false:非表示
  /// @note デバッグ用
  void SetIsVisibleSkyBox(b32 isVisible);

  /// @brief 地面表示・非表示切り替え
  /// @param true:表示 false:非表示
  /// @note デバッグ用
  void SetIsVisibleGround(b32 isVisible);

  /// @brief 近景表示・非表示切り替え
  /// @param true:表示 false:非表示
  /// @note デバッグ用
  void SetIsVisibleBackNear(b32 isVisible);

  /// @brief 遠景表示・非表示切り替え
  /// @param true:表示 false:非表示
  /// @note デバッグ用
  void SetIsVisibleBackFar(b32 isVisible);

  /// @brief 背景データ出力デバッグ開始
  void StartOutputBgData();

  /// @brief 背景データ出力デバッグ停止
  void StopOutputBgData();
#endif

private:
  /// @brief アニメーション更新前のパラメータ保存
  void savePrevParam();

  /// @brief BGセットインデックスから各要素を取得
  /// @param bgSetIndex 背景セットインデックス
  /// @param[out] groundType 地面No
  /// @param[out] nearType 近景No
  /// @param[out] farType 遠景No
  void getTypeFromBgSetIndex(u32 bgSetIndex,u32* groundType,u32* nearType,u32* farType);

  /// @brief 時間帯から時・分・秒を取得
  /// @param timeZone 時間帯
  /// @param[out] hour 時
  /// @param[out] mitute 分
  /// @param[out] secound 秒
  void getTimeFromTimeZone(TimeZone timeZone,u32* hour,u32* mitute,u32* secound);

  /// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
  f32 getPrevAnimationFrame(const btl::BgSystem* pBgSystem,btl::BgSystem::BG_OBJ_TYPE objType,int anmType) const;

  // ヒープ
  gfl2::heap::HeapBase* m_pHeap;
  // 一時ヒープ
  gfl2::heap::HeapBase* m_pWorkHeap;

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

  // 背景システム
  btl::BgSystem* m_pBgSystem;

  // ロードする背景セットインデックス
  u32 m_BgSetIndex;
  // ロードする地面・近景・遠景
  u32 m_GroundType;
  u32 m_NearType;
  u32 m_FarType;

  // 背景の時間帯
  TimeZone m_TimeZone;
  // 背景の時間
  u32 m_Hour;
  u32 m_Minutes;
  u32 m_Second;

  // Y軸角度
  f32 m_DegY;

  // アニメーションフラグ
  bool m_IsUpdateAnimation;

  // 再読み込み終了フレームフラグ
  // 再読み込みが終わったフレームのみtrueになる
  bool m_LoadEndFrameFlag;

  // 写真パラメータ用前フレーム情報
  struct PrevParam
  {
    f32 groundLoopAnmFrame;
    f32 nearLoopAnmFrame;
    f32 farLoopAnmFrame;
    f32 buildLoopAnmFrame;
    f32 timeAnmFrame;
    f32 skyCloudAnmFrame;
    f32 skySunAnmFrame;

    PrevParam():
      groundLoopAnmFrame(0),
      nearLoopAnmFrame(0),
      farLoopAnmFrame(0),
      buildLoopAnmFrame(0),
      timeAnmFrame(0),
      skyCloudAnmFrame(0),
      skySunAnmFrame(0){}
  } m_PrevParam;

  // 遠景・天球用プロジェクション行列
  // カメラシステムから受け取る
  const gfl2::math::Matrix44* m_pFarProjMtx;
  const gfl2::math::Matrix44* m_pSkyProjMtx;
  // ビュー行列
  // カメラシステムから受け取る
  const gfl2::math::Matrix34* m_pViewMtx;

  // カメラシステムが無い時用（固定カメラ）
  gfl2::math::Matrix34 m_ProvViewMtx;
  gfl2::math::Matrix44 m_ProvProjMtx;

  // ファインダースタジオ用描画パイプライン
  FinderStudioRenderPipeLine* m_pFinderStudioRenderPipeLine;

  // 時間テーブル
  static const u32 TIME_TABLE[][3];

  // 以下、デバッグ用
#if PM_DEBUG
  b32 m_DebugIsVisibleSkyBox;
  b32 m_DebugIsVisibleGround;
  b32 m_DebugIsVisibleBackNear;
  b32 m_DebugIsVisibleBackFar;

  b32 m_DebugOutputBgDataFlag;
  int m_DebugOutputNowObjType;
  int m_DebugOutputNowAnmType;
  gfl2::fs::DirectSdmcFile* m_DebugOutputFile;

  void outPutLog(u32 objIndex,u32 slotIndex);
#endif
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_BACK_GROUND_H_INCLUDED__