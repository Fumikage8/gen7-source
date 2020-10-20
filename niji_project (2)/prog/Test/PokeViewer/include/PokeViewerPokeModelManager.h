//======================================================================
/**
 * @file PokeViewerPokeModelManager.h
 * @date 2015/12/03 16:10:59
 * @author araki_syo
 * @brief ポケビューア：ポケモンモデル管理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined POKEVIEWER_POKEMODELMANAGER_H_INCLUDED
#define POKEVIEWER_POKEMODELMANAGER_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Poketool/include/PokeModelSystem.h"
#include "PokeViewerRenderingPipeline.h"
#include "System/include/Camera/ModelViewerInFrame.h"
#include "System/include/Camera/InFrameCamera.h"
#include "LoadMeasurementData.h"

#include <Applib/include/util/app_util_heap.h>
#include <NetStatic/NetAppLib/include/Debug/NetAppDebugTextDrawer.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

class PokeViewerPokeModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeViewerPokeModelManager);

public:

  enum UPDATE_TYPE
  {
    UPDATE_TYPE_NORMAL,              // 通常(初期状態)
    UPDATE_TYPE_CAPTURE,             // ポケモンキャプチャ
    UPDATE_TYPE_CAPTURE_ALL,         // 指定した範囲の番号のポケモン全キャプチャ
    UPDATE_TYPE_DIMENSIONAL_CAPTURE, // 三面図撮影
    UPDATE_TYPE_LOAD_MEASUREMENT,    // 負荷計測
  };

  enum CAMERA_TYPE
  {
    CAMERA_TYPE_DEFAULT,
    CAMERA_TYPE_STATUS,
    CAMERA_TYPE_KAWAIGARI,
  };

  enum CAMERA_DIRECTION
  {
    CAMERA_DIRECTION_FRONT,
    CAMERA_DIRECTION_BACK,
    CAMERA_DIRECTION_RIGHT,
    CAMERA_DIRECTION_LEFT,
    CAMERA_DIRECTION_TOP,
    CAMERA_DIRECTION_BOTTOM,
    CAMERA_DIRECTION_NUM,
  };

  enum CAMERA_MODE
  {
    CAMERA_MODE_FREE,
    CAMERA_MODE_INFRAME_CTR,
    CAMERA_MODE_INFRAME_BTM,
    CAMERA_MODE_NUMBEROF,
  };

  enum FILENAME_PATTERN
  {
    FILENAME_PATTERN_POKEMONCENTER,
    FILENAME_PATTERN_POKEDEX,
  };

private:

  enum STATUS
  {
    STATE_NONE,
    STATE_LOAD_POKEMODEL,
    STATE_UNLOAD_POKEMODEL,
  };

  /**
   * @brief 負荷計測時の状態
   */
  enum MEASUREMENT_STATUS
  {
    MEASUREMENT_STATUS_NONE,                    // 負荷計測していない
    MEASUREMENT_STATUS_START_CALC_REF_VALUE,    // 基準値計測開始
    MEASUREMENT_STATUS_WORKING_CALC_REF_VALUE,  // 基準値計測中
    MEASUREMENT_STATUS_END_CALC_REF_VALUE,      // 基準値計測終了
    MEASUREMENT_STATUS_START,                   // 負荷計測開始
    MEASUREMENT_STATUS_CALC_COMMAND_SIZE,       // コマンドサイズの計算
    MEASUREMENT_STATUS_ADJUST_CAMERA,           // 負荷計測のカメラ設定
    MEASUREMENT_STATUS_WORKING,                 // 負荷計測中
    MEASUREMENT_STATUS_END                      // 負荷計測終了
  };

public:

  PokeViewerPokeModelManager();
  ~PokeViewerPokeModelManager();

  void Initialize(GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* pHeap);
  void Finalize();

  void Update();
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  // カメラ関係
  CAMERA_MODE GetCameraMode() const;
  void SetCameraMode(CAMERA_MODE mode);
  void UpdateCameraPosition();
  u32 GetCameraFovy() const;
  void SetCameraFovyType(CAMERA_TYPE type);
  void SetCameraNearFarType(CAMERA_TYPE type);
  void SetPresetCamera(CAMERA_TYPE type);
  
  void  SetCameraFovy(const u32 fovy) {m_CameraFovy = fovy;}



  // カメラ関係：HIO用
  void SetCameraPosition(const gfl2::math::Vector3& rRHS)   {m_CameraPosition = rRHS;}
  void SetCameraTarget(const gfl2::math::Vector3& rRHS)     {m_CameraTarget = rRHS;}
  void SetCameraRotation(const gfl2::math::Vector3& rRHS)   {m_CameraRotate = rRHS;}
  void SetCameraPolarCoodRadius(const f32 RHS)              {m_CameraLength = RHS;}


  // モデル位置設定
  void SetPokeModelPosition(CAMERA_TYPE type);

  // ライト関係
  void UpdateLightPosition();
  void ResetLightPosition();
  f32 GetLightRotationV() const;
  void SetLightRotationV(f32 RotV);
  f32 GetLightRotationH() const;
  void SetLightRotationH(f32 RotH);
  void SetLightRotation(f32 RotV, f32 RotH);

  // モデルの読み込み/解放
  bool LoadPokeModel();
  bool UnloadPokeModel();
  b32 IsDisposable() const;

  // 各種表示設定
  b32 GetShadowVisibility() const;
  void SetShadowVisibility(b32 visibility);

  // 背景色設定
  void SetClearColor(const gfl2::gfx::Color color);
  gfl2::gfx::Color GetClearColor() const;

  // エッジ色設定
  gfl2::gfx::Color GetEdgeColor() const;
  void SetEdgeColor(const gfl2::gfx::Color color);

  // カラーシェーダ設定
  gfl2::gfx::Color GetPokeShaderColor() const;
  void SetPokeShaderColor(const gfl2::gfx::Color color);
  b32 IsPokeShaderEnable() const;
  void SetPokeShaderEnable(b32 isEnable);

  // モーション関係
  void GetMotionName(char* buf) const;
  u32 GetMotionIndex() const;
  void SetMotionIndex(s32 index);
  f32 GetMotionMaxFrame() const;
  f32 GetCurrentMotionFrame() const;
  b32 GetMotionLoop() const;
  void SetMotionLoop(b32 isEnable);
  f32 GetMotionSpeed() const;
  void SetMotionSpeed(f32 speed);
  void PlayMotion(b32 isEnable);
  b32 IsPlaying() const;
  void PlaySingleFrame(b32 isForward);
  void SetMotionFrame(const f32 frame);
  PokeTool::MODEL_ANIMETYPE GetAnimeType() const;
  void SetAnimeType(PokeTool::MODEL_ANIMETYPE);

  // キャプチャ関係
  u32 GetStartMonsNo() const;
  void SetStartMonsNo(u32 monsNo);
  u32 GetEndMonsNo() const;
  void SetEndMonsNo(u32 monsNo);
  b32 IsCaptureMode() const;
  f32 GetCameraDistance() const;
  void SetCameraDistance(f32 distance);

  bool GetCameraDirectionFlag(CAMERA_DIRECTION dir) const;
  void ToggleCameraDirectionFlag(CAMERA_DIRECTION dir);

  void PrintScreen() const;

  /**
   * @brief 負荷計測モードかどうかの判定
   */
  b32 IsLoadMeasurementMode() const;
  void SetCaptureMode(UPDATE_TYPE type);

  // ポケモンのパラメータ関係
  void GetMonsName(pml::STRCODE* buf) const;
  u32 GetMonsNo() const;
  void SetMonsNo(s32 monsNo);
  u32 GetFormNo() const;
  void ChangeFormNo(s32 value);
  pml::Sex GetSex() const;
  pml::personal::SexVector GetSexVector() const;
  void ToggleSex();
  b32 IsRare() const;
  void ToggleRare();
  f32 GetPokeBaseHeight() const;
  f32 GetPokeAdjustHeight() const;
  char GetSizeCategory() const;
  void SetSpindaPattern();

  // デバッグメニュー
  void InitDebug();
  void TermDebug();






  bool IsStable(void) const {return (m_State == STATE_NONE);}
  void SetGuideRectSize(const u32 pixels = 0) {m_GuideRectPixelSize  = pixels;}
  u32  GetGuideRectSize(void) const           {return m_GuideRectPixelSize;}

private:

  // ポケモン情報
  PokeTool::MODEL_ANIMETYPE m_AnimeType; //! 現在読み込まれているモデルのANIMETYPE
  PokeTool::MODEL_ANIMETYPE m_NextAnimeType; //! 次に読み込まれるモデルのANIMETYPE
  PokeTool::SimpleParam m_PokeParam;

  // 各種表示設定
  b32 m_ShadowVisibility;

  // カラーシェーダ設定
  b32 m_IsPokeShaderEnable;
  gfl2::gfx::Color m_PokeShaderColor;

  // 背景色設定
  gfl2::gfx::Color m_ClearColor;

  // エッジ色設定
  gfl2::gfx::Color m_EdgeColor;

  // フォルム違い・雌雄差取得
  u32 GetFormMax() const;
  b32 ExistFemale() const;

  pml::personal::PersonalData* m_pPersonalData;

  void ResetCameraPosition();
  void ResetCameraFovyNearFar();

  // Private Update
  void Update_Normal();
  void Update_Capture();
  void Update_CaptureAll();
  void Update_DimensionalCapture();

  void Update_LoadPokeModel();
  void Update_UnloadPokeModel();

  /** 
   * @brief 次のキャプチャパラメータを設定
   * @return bool 次のパラメータが存在しない(現在のポケモンのキャプチャが終了した)ときはfalse
   */
  bool SetNextPokeParam();

  bool SetNextCameraDirection();

  /**
   * @brief ポケモンをキャプチャ
   * @return bool キャプチャが完了するとtrueを返す
   */
  bool CapturePokeModel();

  /**
   * @brief 負荷計測モードの更新処理
   */
  void Update_LoadMeasurement();

  /**
   * @brief 負荷計測の実行
   */
  void ExecuteLoadMeasurement();

  /**
   * @brief AABBを画面内収めるカメラの調整
   * @return bool 成功/失敗
   */
  bool AdjustCamera( f32 cameraScale, CAMERA_DIRECTION dir = CAMERA_DIRECTION_FRONT );

  void AdjustLight( CAMERA_DIRECTION dir );

  /**
   * @brief 処理負荷の計測
   * @return bool 計測終了/計測中
   */
  bool CalculateLoadMeasurement();

  // update shader color
  void UpdatePokeShaderColor();

  bool CheckedForDimensionalCapture();

  gfl2::heap::HeapBase* m_pHeap;
  System::nijiAllocator* m_pNijiAllocator;

  u32 m_State;
  s32 m_Counter;
  UPDATE_TYPE m_UpdateType;

  PokeViewerRenderingPipeline* m_pRenderingPipeline;

  GameSys::GameManager* m_pGameManager;

  PokeTool::PokeModelSystem* m_pPokeModelSys;
  PokeTool::PokeModel* m_pPokeModel; //! LoadされていないときはNULL

  // カメラ関係
  CAMERA_MODE m_CameraMode;
  gfl2::math::Vector3 m_CameraPosition;   // 視点
  gfl2::math::Vector3 m_CameraTarget;     // 注視点
  gfl2::math::Vector3 m_CameraRotate;
  f32 m_CameraLength;
  u32 m_CameraFovy;
  f32 m_CameraNear;
  f32 m_CameraFar;
  CAMERA_TYPE m_CameraFovyType;
  CAMERA_TYPE m_CameraNearFarType;
  
  // インフレーム
  System::Camera::CModelViewerInFrame* m_pInframeSys;
  System::Camera::InFrameCamera* m_pInframeCamera;

  // ライト関係
  gfl2::math::Vector3 m_LightRotate;

  // モーション関係
  u32 m_MotionIndex;
  b32 m_MotionIsLoop;
  f32 m_MotionSpeed;
  b32 m_IsPlaying; //! 再生中はtrueが返ってくる
  f32 m_MotionFrame; //! モデルを読み込みなおしたときに設定する値

  // キャプチャ関係
  u32 m_StartMonsNo; //! 自動キャプチャ開始位置
  u32 m_EndMonsNo; //! 自動キャプチャ停止位置
  u32 m_CurrentMonsNo; //! 自動キャプチャ時のMonsNo
  u32 m_CaptureSeq;
  f32 m_CameraDistance; //! キャプチャ用カメラ距離

  CAMERA_DIRECTION m_CurrentDirection;

  bool m_CameraDirectionFlags[CAMERA_DIRECTION_NUM];

  FILENAME_PATTERN m_FileNamePattern; //! キャプチャファイル命名規則

  // パッチールぶち
  int m_LeftEarValue;
  int m_RightEarValue;
  int m_LeftEyeValue;
  int m_RightEyeValue;

  //!< @brief 負荷計測の状態
  MEASUREMENT_STATUS m_MeasurementStatus;
  //!< @brief 負荷計測時のフレームカウンタ
  u32 m_MeasurementFrameCount;
  //!< @brief 負荷計測の時間(単位 : フレーム)
  static const u32 MEASUREMENT_FRAME_COUNT_MAX = 300;
  //!< @brief 負荷計測の情報格納先
  LoadMeasurementData m_LoadMeasurementData;

  app::util::Heap                         m_DebugTextDrawerHeap;
  NetAppLib::Debug::NetAppDebugTextDrawer m_DebugTextDrawer;
  u32                                     m_GuideRectPixelSize;
  void  DrawGuideRect(const u32 pixels=0);
};

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // POKEVIEWER_POKEMODELMANAGER_H_INCLUDED

#endif // PM_DEBUG
