//======================================================================
/**
 * @file FinderStudioCameraSystem.h
 * @date 2016/08/30
 * @author harada_takumi
 * @brief ファインダースタジオカメラシステム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_STUDIO_CAMERA_SYSTEM_H_INCLUDED__
#define __FINDER_STUDIO_CAMERA_SYSTEM_H_INCLUDED__
#pragma once

// System
#include "System/include/GflUse.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)
  struct PhotoParam;

class FinderStudioCameraSystem
{
  GFL_FORBID_COPY_AND_ASSIGN(FinderStudioCameraSystem); //コピーコンストラクタ＋代入禁止

public:
  FinderStudioCameraSystem();
  virtual ~FinderStudioCameraSystem();

  // 更新
  void Update();

  // カメラリセット
  void ResetCamera();

  // キャラ・ポケモン座標登録
  void RegistPos(const gfl2::math::Vector3* pCharaPos,const gfl2::math::Vector3* pPokePos)
  {
    m_pCharaPos = pCharaPos;
    m_pPokePos = pPokePos;
  }
  // カメラ係数設定
 // void SetFocusFactor(f32 focusFactor){ m_FocusFactor = focusFactor; }

  /// @brief 再現用パラメータ適用
  /// @param 再現用パラメータ
  void ApplyCameraPhotoParam(const PhotoParam& photoParam);

  /// @brief 再現用パラメータ出力
  /// @param[out] 再現用パラメータ 必要な情報だけを格納する
  void GetPhotoParamCamera(PhotoParam* pPhotoParam) const;

  /// @brief 構図用パラメータ適用
  /// @param zoomFov ズーム値(3 ~ 30)
  void ApplyComposition(u32 zoomFoV);

  /// @brief 撮影モード切替時処理
  void OnChangeCaptureMode(CaptureMode captureMode);

  // ビュー行列取得
  const gfl2::math::Matrix34* GetViewMtx() const { return &m_ViewMtx; }
  // プロジェクション行列取得
  const gfl2::math::Matrix44* GetProjMtx() const { return &m_ProjMtx; }
  const gfl2::math::Matrix44* GetFarProjMtx() const { return &m_FarProjMtx; }
  const gfl2::math::Matrix44* GetSkyProjMtx() const { return &m_SkyProjMtx; }

  // 角度取得
  f32 GetDegX() const { return m_DegX; }
  f32 GetDegY() const { return m_DegY; }
  // 座標取得
  gfl2::math::Vector3 GetPos() const { return m_EyePosition; }
  // 距離取得
  f32 GetLength() const { return m_Length; }


  /// @brief カメラY軸回転（左方向）
  void RotateYLeft();
  /// @brief カメラY軸回転（右方向）
  void RotateYRight();

  /// @brief カメラX軸回転（上方向）
  void RotateXUp();
  /// @brief カメラX軸回転（下方向）
  void RotateXDown();

  /// @brief カメラズームイン
  /// @return これ以上ズームインできるか
  bool ZoomIn();
  /// @brief カメラズームアウト
  /// @return これ以上ズームアウトできるか
  bool ZoomOut();

  /// @brief カメラ高さ変更（高く）
  /// @return これ以上高くできるか
  bool HeightUp();
  /// @brief カメラ高さ変更（低く）
  /// @return これ以上低くできるか
  bool HeightDown();

  /// @brief 注視点変更（トレーナーに寄る）
  /// @return これ以上寄れるか
  bool FocusToPlayer();
  /// @brief 注視点変更（ポケモンに寄る）
  /// @return これ以上寄れるか
  bool FocusToPokemon();

  /// @brief カメラズーム割合セット
  /// @param カメラズーム割合(0 ~ 1)
  void SetZoomRate(float zoomRate);

  /// @brief カメラ高さ割合セット
  /// @param カメラ高さ割合(0 ~ 1)
  void SetHeightRate(float heightRate);

  /// @brief カメラ注視点割合セット
  /// @param カメラ注視点割合(0 ~ 1)
  void SetFocusRate(float focusRate);

  /// @brief 現在のカメラズーム度合い取得
  /// @return ズーム度合い(0 ~ 1)
  f32 GetZoomRate() const;

  /// @brief 現在のカメラフォーカス取得
  /// @return 0 ~ 1 トレーナーに完全フォーカスなら０ ポケモンに完全フォーカスなら１
  f32 GetFocusRate() const;

  /// @brief 現在のカメラ高さ取得
  /// @return 高さ(0 ~ 1)
  f32 GetHeightRate() const;

  
  // 便利関数
  /// @brief ベクトルの回転
  /// @param[out] pVec 回転するベクトル
  /// @param deg 角度（度）
  static void RotateYVec(gfl2::math::Vector3* pVec,f32 deg);
  static void RotateXVec(gfl2::math::Vector3* pVec,f32 deg);

  // デバッグ用
#if PM_DEBUG
  void EnableDebugParam(bool isEnable){ m_EnableDebugParam = isEnable; }
  void DebugSetCameraMoveSpeed(f32 speed){ m_DebugCameraSpeed = speed; }
  void DebugSetCameraLength(f32 length){ m_Length = length; }
  void DebugSetCameraHeightMax(f32 heightMax){ m_DebugCameraHeightMax = heightMax; }
  void DebugSetCameraHeightMin(f32 heightMin){ m_DebugCameraHeightMin = heightMin; }
  void DebugSetCameraFoVMax(f32 FoVMax){ m_DebugFoVMax = FoVMax; }
  void DebugSetCameraFoVMin(f32 FoVMin){ m_DebugFoVMin = FoVMin; }
  void DebugSetCameraNear(int nearValue){ m_DebugNear = nearValue; }
  void DebugSetCameraFar(int farValue){ m_DebugFar = farValue; }

  int DebugGetCompositionFoV(){ return m_CompositionFoV; }
  void DebugSetCompositionFoV(int compositionFoV){ m_CompositionFoV = compositionFoV; }

  f32 DebugGetNowZoom(){ return m_FoV; }
#endif

private:
  // 各行列更新
  void updateViewMtx();
  void updateProjMtx();

  // 注視点のリセット
  void resetFocusFactor();

  /// @brief 角度のセット
  void setDegX(f32 degX);
  void setDegY(f32 degY);

  /// @brief 各パラメータのセット、丸め処理も行う
  /// @return 値が範囲内に収まったかどうか
  bool setFoV(f32 FoV);
  bool setHeight(f32 height);
  bool setFocusRate(f32 focusRate);

  /// @brief 写真パラメータに収まる範囲に丸める処理
  f32 roundFoV(f32 FoV) const;
  f32 roundHeight(f32 height) const;
  f32 roundFocusRate(f32 focusRate) const;
  /// @brief 実際に使用する値から写真パラメータ用の値に変換
  u32 convToPhotoParamFoV(f32 FoV) const;
  u32 convToPhotoParamHeight(f32 height) const;
  u32 convToPhotoParamFocusRate(f32 focusRate) const;
  /// @brief 写真パラメータから実際に使用する値に変換
  f32 convToUseFoV(u32 paramFoV) const;
  f32 convToUseHeight(u32 paramHeight) const;
  f32 convToUseFocusRate(u32 paramFocusRate) const;
  /// @brief １段階あたりの数値を取得
  f32 getPerFoV() const;
  f32 getPerHeight() const;
  f32 getPerFocusRate() const;

  /// @brief 各定数の取得
  f32 getMoveSpeed() const;
  f32 getZoomSpeed() const;
  f32 getHeightSpeed() const;
  f32 getFocusSpeed() const;
  f32 getMaxHeight() const;
  f32 getMinHeight() const;
  f32 getMaxFoV() const;
  f32 getMinFoV() const;
  f32 getNear() const;
  f32 getFar() const;

  // 撮影モード
  CaptureMode m_CaptureMode;

  // ビュー行列
  gfl2::math::Matrix34 m_ViewMtx;
  // プロジェクション行列
  gfl2::math::Matrix44 m_ProjMtx;
  gfl2::math::Matrix44 m_FarProjMtx;
  gfl2::math::Matrix44 m_SkyProjMtx;

  // カメラポジション
  gfl2::math::Vector3 m_EyePosition;
  // カメラフォーカス
  gfl2::math::Vector3 m_FocusPosition;
  // カメラ上方向
  gfl2::math::Vector3 m_UpDirection;

  // 注視点から見たＹ角度
  f32 m_DegY;
  // 注視点から見たＸ角度
  f32 m_DegX;

  // 注視点からの距離
  // 現状カメラの移動はしないため恐らく変更しない（視野角を変えてズームする）
  f32 m_Length;

  // カメラの高さ
  // カメラ座標と注視点に使用
  f32 m_Height;

  // 視野角（ズームの度合い）
  f32 m_FoV;

  // 構図で使用する視野角
  f32 m_CompositionFoV;

  // 注視するポケモン座標
  const gfl2::math::Vector3* m_pPokePos;
  // 注視するキャラクタ座標（11/15現在原点だが一応）
  const gfl2::math::Vector3* m_pCharaPos;
  // 係数（0.5でポケモンとキャラの真ん中にフォーカスできる）
  f32 m_FocusFactor;

  // カメラ回転角度上限
  static const f32 MAX_DEG_X;
  // カメラ回転スピード
  static const f32 MOVE_SPEED;
  // 高さ最大値
  static const f32 MAX_HEIGHT;
  // 高さ最小値
  static const f32 MIN_HEIGHT;
  // 最大視野角（ズーム度合い）
  static const f32 MAX_FOV;
  // 最小視野角
  static const f32 MIN_FOV;
  // ニア/ファー
  static const f32 Z_NEAR;
  static const f32 Z_FAR;
  static const f32 FAR_Z_FAR;   // 遠景用ファー値
  static const f32 SKY_Z_FAR;   // 天球用ファー値
  // ズーム値・高さ・注視点を何段階で保存するか
  static const u32 PHOTO_PARAM_SAVE_MAX;

  // デバッグ用
#if PM_DEBUG
  bool m_EnableDebugParam;
  f32 m_DebugCameraSpeed;
  f32 m_DebugCameraHeightMax;
  f32 m_DebugCameraHeightMin;
  f32 m_DebugFoVMax;
  f32 m_DebugFoVMin;
  u32 m_DebugNear;
  u32 m_DebugFar;
#endif
};

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)

#endif // __FINDER_STUDIO_CAMERA_SYSTEM_H_INCLUDED__