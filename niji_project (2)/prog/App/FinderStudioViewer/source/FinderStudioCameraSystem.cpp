//======================================================================
/**
 * @file FinderStudioCameraSystem.cpp
 * @date 2016/08/30
 * @author harada_takumi
 * @brief ファインダースタジオカメラシステム
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioCameraSystem.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// @fix MMCat[670] カメラの上限角度を狭めることで反転してしまうことを防ぐ
// カメラ回転角度上限
const f32 FinderStudioCameraSystem::MAX_DEG_X = -73;
// カメラ回転スピード
const f32 FinderStudioCameraSystem::MOVE_SPEED = 3;
// 高さ最大値
const f32 FinderStudioCameraSystem::MAX_HEIGHT = 150;
// 高さ最小値
const f32 FinderStudioCameraSystem::MIN_HEIGHT = 11.0f;
// 最大視野角（ズーム度合い）
const f32 FinderStudioCameraSystem::MAX_FOV = 30;
// 最小視野角（ズーム度合い）
const f32 FinderStudioCameraSystem::MIN_FOV = 3;
// ニア/ファー
const f32 FinderStudioCameraSystem::Z_NEAR = 92;
const f32 FinderStudioCameraSystem::Z_FAR = 6000;
const f32 FinderStudioCameraSystem::FAR_Z_FAR = 15000;
const f32 FinderStudioCameraSystem::SKY_Z_FAR = 50000;
// ズーム値・高さ・注視点を何段階で保存するか
const u32 FinderStudioCameraSystem::PHOTO_PARAM_SAVE_MAX = 100;

//コンストラクタ
FinderStudioCameraSystem::FinderStudioCameraSystem():
  m_CaptureMode(CAPTURE_MODE_BOTH)
  ,m_CompositionFoV(20)
  ,m_pPokePos(NULL)
  ,m_pCharaPos(NULL)
  ,m_FocusFactor(0)
#if PM_DEBUG
  ,m_EnableDebugParam(true)
  ,m_DebugCameraSpeed(MOVE_SPEED)
  ,m_DebugCameraHeightMax(MAX_HEIGHT)
  ,m_DebugCameraHeightMin(MIN_HEIGHT)
  ,m_DebugFoVMax(MAX_FOV)
  ,m_DebugFoVMin(MIN_FOV)
  ,m_DebugNear(Z_NEAR)
  ,m_DebugFar(Z_FAR)
#endif
{
  // カメラ初期設定
  ResetCamera();
  // 更新して行列を作成しておく
  this->Update();
}
//デストラクタ
FinderStudioCameraSystem::~FinderStudioCameraSystem()
{
}

// カメラリセット
void FinderStudioCameraSystem::ResetCamera()
{
  // デフォルト高さ
  this->setHeight(75);

  // 座標は注視点からの角度・距離で算出する
  //m_EyePosition.Set(0,150,-520);
  //m_FocusPosition.Set(0,m_Height,0);
  m_UpDirection.Set(0,1,0);

  m_DegY = 180;
  m_DegX = -10;
  m_Length = 880;

  this->resetFocusFactor();

  this->setFoV(m_CompositionFoV);
}

// 更新
void FinderStudioCameraSystem::Update()
{
  // ビュー行列更新
  updateViewMtx();

  // プロジェクション行列更新
  updateProjMtx();

#if PM_DEBUG
  // MMCat[670] カメラの距離とポケモン最大距離は変えちゃいけないチェック
  f32 rad = gfl2::math::AcosRad(Static::VIEWER_MOVE_RADIUS / m_Length);
  f32 deg = gfl2::math::ConvRadToDeg(rad);
  if(deg <= -MAX_DEG_X)
  {
    GFL_ASSERT(0);
  }
#endif
}

void FinderStudioCameraSystem::updateViewMtx()
{
  // 0 ~ 360 に補正
  if(m_DegY > 360) m_DegY -= 360;
  if(m_DegY < 0) m_DegY += 360;

  // 角度制限
  if(m_DegX > -1) m_DegX = -1;
  if(m_DegX < MAX_DEG_X) m_DegX = MAX_DEG_X;

  // 高さ制限
  if(m_Height > getMaxHeight()) this->setHeight(getMaxHeight());
  if(m_Height < getMinHeight()) this->setHeight(getMinHeight());

  // 注視点からの角度・距離からカメラ座標を算出
  gfl2::math::Vector3 vec(0,0,m_Length);
  
  RotateXVec(&vec,m_DegX);
  RotateYVec(&vec,m_DegY);

  // 仕様削除
  // 座標切り替え
//   if(m_pPokePos && m_pCharaPos)
//   {
//     // キャラの座標を足す
//     vec += *m_pCharaPos;
// 
//     // ポケモンのY座標は０として考える
//     gfl2::math::Vector3 pokePos = *m_pPokePos;
//     pokePos.y = 0;
//     // ポケモンの座標を係数をかけて足す
//     vec += pokePos * m_FocusFactor;
//   }

  // 最後に高さを反映
  vec.y += m_Height;

  m_EyePosition.Set(vec);

  // 高さから中心点を算出
  gfl2::math::Vector3 focus(0,0,0);
  // 注視点切り替え
  if(m_pPokePos && m_pCharaPos)
  {
    // キャラの座標を足す（9/23現在は原点）
    focus += *m_pCharaPos;
    // ポケモンのY座標は０として考える
    gfl2::math::Vector3 pokePos = *m_pPokePos;
    pokePos.y = 0;
    // ポケモンの座標を係数をかけて足す
    focus += pokePos * m_FocusFactor;
  }
  // 最後に高さを反映
  focus.y += m_Height;
  
  m_FocusPosition.Set(focus);

  // ビュー行列作成
  m_ViewMtx = gfl2::math::Matrix34::GetLookAt(m_EyePosition,m_FocusPosition,m_UpDirection);
}
void FinderStudioCameraSystem::updateProjMtx()
{
  // ズームイン/アウト制限
  if(m_FoV < getMinFoV()) this->setFoV(getMinFoV());
  if(m_FoV > getMaxFoV()) this->setFoV(getMaxFoV());

  u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );
  u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ gfl2::gfx::CtrDisplayNo::LEFT ] );

  if(displayHeight == 0)
  {
    GFL_ASSERT(0);
    displayHeight = 400;
  }

  u32 nearValue = getNear();
  u32 farValue = getFar();

  m_ProjMtx = gfl2::math::Matrix44::GetPerspective(
    gfl2::math::ConvDegToRad(m_FoV),
    static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
    nearValue,							// 前方投影面までの距離
    farValue,true);							// 後方投影面までの距離
  
  m_FarProjMtx = gfl2::math::Matrix44::GetPerspective(
    gfl2::math::ConvDegToRad(m_FoV),
    static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
    nearValue,							// 前方投影面までの距離
    FAR_Z_FAR,true);							// 後方投影面までの距離

  m_SkyProjMtx = gfl2::math::Matrix44::GetPerspective(
    gfl2::math::ConvDegToRad(m_FoV),
    static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
    nearValue,							// 前方投影面までの距離
    SKY_Z_FAR,true);							// 後方投影面までの距離
}

/// @brief 再現用パラメータ適用
/// @param 再現用パラメータ
void FinderStudioCameraSystem::ApplyCameraPhotoParam(const PhotoParam& photoParam)
{
  m_DegX = -static_cast<f32>(photoParam.cameraDegX);
  m_DegY = photoParam.cameraDegY;
  m_Height = this->convToUseHeight(photoParam.cameraHeight);
  m_FoV = this->convToUseFoV(photoParam.cameraFov);
  m_FocusFactor = this->convToUseFocusRate(photoParam.cameraFocusFactor);
}

/// @brief 再現用パラメータ出力
/// @param[out] 再現用パラメータ 必要な情報だけを格納する
void FinderStudioCameraSystem::GetPhotoParamCamera(PhotoParam* pPhotoParam) const
{
  pPhotoParam->cameraDegX = static_cast<u32>(-m_DegX);
  pPhotoParam->cameraDegY = m_DegY;
  pPhotoParam->cameraHeight = this->convToPhotoParamHeight(m_Height);
  pPhotoParam->cameraFov = this->convToPhotoParamFoV(m_FoV);
  pPhotoParam->cameraFocusFactor = this->convToPhotoParamFocusRate(m_FocusFactor);
}

/// @brief 構図用パラメータ適用
/// @param zoomFov ズーム値(3 ~ 30)
void FinderStudioCameraSystem::ApplyComposition(u32 zoomFoV)
{
  m_CompositionFoV = zoomFoV;
}

/// @brief 撮影モード切替時処理
void FinderStudioCameraSystem::OnChangeCaptureMode(CaptureMode captureMode)
{
  m_CaptureMode = captureMode;
}

/// @brief カメラY軸回転（左方向）
void FinderStudioCameraSystem::RotateYLeft()
{
  this->setDegY(m_DegY + this->getMoveSpeed());
}
/// @brief カメラY軸回転（右方向）
void FinderStudioCameraSystem::RotateYRight()
{
  this->setDegY(m_DegY - this->getMoveSpeed());
}

/// @brief カメラX軸回転（上方向）
void FinderStudioCameraSystem::RotateXUp()
{
  this->setDegX(m_DegX - this->getMoveSpeed());
}
/// @brief カメラX軸回転（下方向）
void FinderStudioCameraSystem::RotateXDown()
{
  this->setDegX(m_DegX + this->getMoveSpeed());
}

/// @brief カメラズームイン
/// @return これ以上ズームインできるか
bool FinderStudioCameraSystem::ZoomIn()
{
  return this->setFoV(m_FoV - this->getZoomSpeed());
}
/// @brief カメラズームアウト
/// @return これ以上ズームアウトできるか
bool FinderStudioCameraSystem::ZoomOut()
{
  return this->setFoV(m_FoV + this->getZoomSpeed());
}

/// @brief カメラ高さ変更（高く）
/// @return これ以上高くできるか
bool FinderStudioCameraSystem::HeightUp()
{
  return this->setHeight(m_Height + this->getHeightSpeed());
}
/// @brief カメラ高さ変更（低く）
/// @return これ以上低くできるか
bool FinderStudioCameraSystem::HeightDown()
{
  return this->setHeight(m_Height - this->getHeightSpeed());
}

/// @brief 注視点変更（トレーナーに寄る）
/// @return これ以上寄れるか
bool FinderStudioCameraSystem::FocusToPlayer()
{
  return this->setFocusRate(m_FocusFactor - this->getFocusSpeed());
}
/// @brief 注視点変更（ポケモンに寄る）
/// @return これ以上寄れるか
bool FinderStudioCameraSystem::FocusToPokemon()
{
  return this->setFocusRate(m_FocusFactor + this->getFocusSpeed());
}

/// @brief カメラズーム割合セット
/// @param カメラズーム割合(0 ~ 1)
void FinderStudioCameraSystem::SetZoomRate(float zoomRate)
{
  if(zoomRate < 0 || zoomRate > 1.0f)
  {
    GFL_ASSERT(0);
    zoomRate = gfl2::math::Clamp(zoomRate,0.0f,1.0f);
  }

  f32 FoVMin = getMinFoV();
  f32 FoVMax = getMaxFoV();

  f32 zoomWidth = FoVMax - FoVMin;
  this->setFoV((zoomWidth * zoomRate) + FoVMin);
}

/// @brief カメラ高さ割合セット
/// @param カメラ高さ割合(0 ~ 1)
void FinderStudioCameraSystem::SetHeightRate(float heightRate)
{
  if(heightRate < 0 || heightRate > 1.0f)
  {
    GFL_ASSERT(0);
    heightRate = gfl2::math::Clamp(heightRate,0.0f,1.0f);
  }

  f32 heightMin = getMinHeight();
  f32 heightMax = getMaxHeight();

  f32 heightWidth = heightMax - heightMin;
  this->setHeight((heightWidth * heightRate) + heightMin);
}

/// @brief カメラ注視点割合セット
/// @param カメラ注視点割合(0 ~ 1)
void FinderStudioCameraSystem::SetFocusRate(float focusRate)
{
  if(focusRate < 0 || focusRate > 1.0f)
  {
    GFL_ASSERT(0);
    focusRate = gfl2::math::Clamp(focusRate,0.0f,1.0f);
  }

  this->setFocusRate(focusRate);
}

/// @brief 現在のカメラズーム度合い取得
/// @return ズーム度合い(0 ~ 1)
f32 FinderStudioCameraSystem::GetZoomRate() const
{
  f32 zoomWidth = getMaxFoV() - getMinFoV();
  f32 zoomRate = (m_FoV - getMinFoV()) / zoomWidth;

  return zoomRate;
}

/// @brief 現在のカメラフォーカス取得
/// @return 0 ~ 1 トレーナーに完全フォーカスなら０ ポケモンに完全フォーカスなら１
f32 FinderStudioCameraSystem::GetFocusRate() const
{
  return m_FocusFactor;
}

/// @brief 現在のカメラ高さ取得
/// @return 高さ(0 ~ 1)
f32 FinderStudioCameraSystem::GetHeightRate() const
{
  f32 heightWidth = getMaxHeight() - getMinHeight();
  f32 heightRate = (m_Height - getMinHeight()) / heightWidth;

  return heightRate;
}

// 注視点のリセット
void FinderStudioCameraSystem::resetFocusFactor()
{
  switch(m_CaptureMode)
  {
  case CAPTURE_MODE_POKE:
    // ポケモンに注視点が行くようにする
    this->setFocusRate(1.0f);
    break;
  case CAPTURE_MODE_BOTH:
    // トレーナーとポケモンの間に注視点が行くようにする
    this->setFocusRate(0.5f);
    break;
  default:
    GFL_ASSERT_STOP(0);
    break;
  }
}

/// @brief 角度のセット
void FinderStudioCameraSystem::setDegX(f32 degX)
{
  m_DegX = degX;

  // 角度制限
  if(m_DegX > -1) m_DegX = -1;
  if(m_DegX < MAX_DEG_X) m_DegX = MAX_DEG_X;
}
void FinderStudioCameraSystem::setDegY(f32 degY)
{
  GFL_ASSERT_STOP(abs(degY) < 720.0f);

  m_DegY = degY;

  // 0 ~ 360 に補正
  if(m_DegY > 360) m_DegY -= 360;
  if(m_DegY < 0) m_DegY += 360;
}

/// @brief 各パラメータのセット、丸め処理も行う
/// @return 値が範囲内に収まったかどうか
bool FinderStudioCameraSystem::setFoV(f32 FoV)
{
  bool isRange = true;
  m_FoV = FoV;

  if(m_FoV < getMinFoV())
  {
    m_FoV = getMinFoV();
    isRange = false;
  }
  else if(m_FoV > getMaxFoV())
  {
    m_FoV = getMaxFoV();
    isRange = false;
  }

  // 写真パラメータに保存できるように丸める
  m_FoV = roundFoV(m_FoV);

  HARADA_PRINT("FoV:%f\n",m_FoV);
  u32 paramFoV = this->convToPhotoParamFoV(m_FoV);
  HARADA_PRINT("FoVParam:%d\n",paramFoV);
  f32 useFoV = this->convToUseFoV(paramFoV);
  HARADA_PRINT("FoVReProduct:%f\n",useFoV);

  return isRange;
}
bool FinderStudioCameraSystem::setHeight(f32 height)
{
  bool isRange = true;
  m_Height = height;

  if(m_Height < getMinHeight())
  {
    m_Height = getMinHeight();
    isRange = false;
  }
  else if(m_Height > getMaxHeight())
  {
    m_Height = getMaxHeight();
    isRange = false;
  }

  // 写真パラメータに保存できるように丸める
  m_Height = roundHeight(m_Height);

  HARADA_PRINT("Height:%f\n",m_Height);
  u32 paramHeight = this->convToPhotoParamHeight(m_Height);
  HARADA_PRINT("HeightParam:%d\n",paramHeight);
  f32 useHeight = this->convToUseHeight(paramHeight);
  HARADA_PRINT("HeightReProduct:%f\n",useHeight);

  return isRange;
}
bool FinderStudioCameraSystem::setFocusRate(f32 focusRate)
{
  bool isRange = true;
  m_FocusFactor = focusRate;

  if(m_FocusFactor < 0)
  {
    m_FocusFactor = 0;
    isRange = false;
  }
  else if(m_FocusFactor > 1.0f)
  {
    m_FocusFactor = 1.0f;
    isRange = false;
  }

  // 写真パラメータに保存できるように丸める
  m_FocusFactor = roundFocusRate(m_FocusFactor);

  HARADA_PRINT("FocusRate:%f\n",m_FocusFactor);
  u32 paramFocusRate = this->convToPhotoParamFocusRate(m_FocusFactor);
  HARADA_PRINT("FocusRateParam:%d\n",paramFocusRate);
  f32 useFocusRate = this->convToUseFocusRate(paramFocusRate);
  HARADA_PRINT("FocusRateReProduct:%f\n",useFocusRate);

  return isRange;
}

/// @brief 写真パラメータに収まる範囲に丸める処理
f32 FinderStudioCameraSystem::roundFoV(f32 FoV) const
{
  u32 paramFoV = this->convToPhotoParamFoV(FoV);
  return this->convToUseFoV(paramFoV);
}
f32 FinderStudioCameraSystem::roundHeight(f32 height) const
{
  u32 paramHeight = this->convToPhotoParamHeight(height);
  return this->convToUseHeight(paramHeight);
}
f32 FinderStudioCameraSystem::roundFocusRate(f32 focusRate) const
{
  u32 paramFocusRate = this->convToPhotoParamFocusRate(focusRate);
  return this->convToUseFocusRate(paramFocusRate);
}

/// @brief 実際に使用する値から写真パラメータ用の値に変換
u32 FinderStudioCameraSystem::convToPhotoParamFoV(f32 FoV) const
{
  GFL_ASSERT_STOP(FoV >= getMinFoV());

  // 誤差解決のため四捨五入し整数同士で計算
  u32 iFoV = static_cast<u32>(((FoV - getMinFoV()) * 1000) + 0.5f);
  f32 per = getPerFoV();
  u32 iPer = static_cast<u32>((per * 1000) + 0.5f);
  return iFoV / iPer;
}
u32 FinderStudioCameraSystem::convToPhotoParamHeight(f32 height) const
{
  GFL_ASSERT_STOP(height >= 0);

  u32 iHeight = static_cast<u32>(((height - getMinHeight()) * 1000) + 0.5f);
  f32 per = getPerHeight();
  u32 iPer = static_cast<u32>((per * 1000) + 0.5f);
  return iHeight / iPer;
}
u32 FinderStudioCameraSystem::convToPhotoParamFocusRate(f32 focusRate) const
{
  GFL_ASSERT_STOP(focusRate >= 0);

  u32 iFocusRate = static_cast<u32>((focusRate * 1000) + 0.5f);
  f32 per = getPerFocusRate();
  u32 iPer = static_cast<u32>((per * 1000) + 0.5f);
  return iFocusRate / iPer;
}

/// @brief 写真パラメータから実際に使用する値に変換
f32 FinderStudioCameraSystem::convToUseFoV(u32 paramFoV) const
{
  f32 per = getPerFoV();
  return (paramFoV * per) + getMinFoV();
}
f32 FinderStudioCameraSystem::convToUseHeight(u32 paramHeight) const
{
  f32 per = getPerHeight();
  return paramHeight * per + getMinHeight();
}
f32 FinderStudioCameraSystem::convToUseFocusRate(u32 paramFocusRate) const
{
  f32 per = getPerFocusRate();
  return paramFocusRate * per;
}

/// @brief １段階あたりの数値を取得
f32 FinderStudioCameraSystem::getPerFoV() const
{
  return (getMaxFoV() - getMinFoV()) / PHOTO_PARAM_SAVE_MAX;   // 100段階で表現
}
f32 FinderStudioCameraSystem::getPerHeight() const
{
  return (getMaxHeight() - getMinHeight()) / PHOTO_PARAM_SAVE_MAX;   // 100段階で表現
}
f32 FinderStudioCameraSystem::getPerFocusRate() const
{
  return 1.0f / PHOTO_PARAM_SAVE_MAX;   // 100段階で表現
}

/// @brief 各定数の取得
f32 FinderStudioCameraSystem::getMoveSpeed() const
{
  f32 moveSpeed;
#if PM_DEBUG
  if(m_EnableDebugParam) moveSpeed = m_DebugCameraSpeed;
  else moveSpeed = MOVE_SPEED;
#else
  moveSpeed = MOVE_SPEED;
#endif
  return moveSpeed;
}
f32 FinderStudioCameraSystem::getZoomSpeed() const
{
  f32 zoomSpeed;
#if PM_DEBUG
  //if(m_EnableDebugParam) zoomSpeed = m_DebugCameraZoomSpeed;
  zoomSpeed = this->getPerFoV();
#else
  zoomSpeed = this->getPerFoV();
#endif
  return zoomSpeed;
}
f32 FinderStudioCameraSystem::getHeightSpeed() const
{
  f32 heightSpeed;
#if PM_DEBUG
  //if(m_EnableDebugParam) heightSpeed = m_DebugCameraHeightSpeed;
  heightSpeed = this->getPerHeight();
#else
  heightSpeed = this->getPerHeight();
#endif
  return heightSpeed;
}
f32 FinderStudioCameraSystem::getFocusSpeed() const
{
  f32 focusSpeed;
#if PM_DEBUG
  //if(m_EnableDebugParam) focusSpeed = m_DebugCameraFocusSpeed;
  focusSpeed = this->getPerFocusRate();
#else
  focusSpeed = this->getPerFocusRate();
#endif
  return focusSpeed;
}
f32 FinderStudioCameraSystem::getMaxHeight() const
{
  f32 maxHeight;
#if PM_DEBUG
  if(m_EnableDebugParam) maxHeight = m_DebugCameraHeightMax;
  else maxHeight = MAX_HEIGHT;
#else
  maxHeight = MAX_HEIGHT;
#endif
  return maxHeight;
}
f32 FinderStudioCameraSystem::getMinHeight() const
{
  f32 minHeight;
#if PM_DEBUG
  if(m_EnableDebugParam) minHeight = m_DebugCameraHeightMin;
  else minHeight = MIN_HEIGHT;
#else
  minHeight = MIN_HEIGHT;
#endif
  return minHeight;
}
f32 FinderStudioCameraSystem::getMaxFoV() const
{
  f32 maxFoV;
#if PM_DEBUG
  if(m_EnableDebugParam) maxFoV = m_DebugFoVMax;
  else maxFoV = MAX_FOV;
#else
  maxFoV = MAX_FOV;
#endif
  return maxFoV;
}
f32 FinderStudioCameraSystem::getMinFoV() const
{
  f32 minFoV;
#if PM_DEBUG
  if(m_EnableDebugParam) minFoV = m_DebugFoVMin;
  else minFoV = MIN_FOV;
#else
  minFoV = MIN_FOV;
#endif
  return minFoV;
}
f32 FinderStudioCameraSystem::getNear() const
{
  f32 nearValue;
#if PM_DEBUG
  if(m_EnableDebugParam) nearValue = m_DebugNear;
  else nearValue = Z_NEAR;
#else
  nearValue = Z_NEAR;
#endif
  return nearValue;
}
f32 FinderStudioCameraSystem::getFar() const
{
  f32 farValue;
#if PM_DEBUG
  if(m_EnableDebugParam) farValue = m_DebugFar;
  else farValue = Z_FAR;
#else
  farValue = Z_FAR;
#endif
  return farValue;
}

// 便利関数
// ベクトルの回転
void FinderStudioCameraSystem::RotateYVec(gfl2::math::Vector3* pVec,f32 deg)
{
  if(!pVec) GFL_ASSERT(0);

  gfl2::math::Vector4 tmpVec(pVec->x,pVec->y,pVec->z,0);
  gfl2::math::Quaternion quat(gfl2::math::Vector3(0,1,0),gfl2::math::ConvDegToRad(deg));
  gfl2::math::Matrix mRot;
  quat.QuaternionToMatrix(mRot);
  tmpVec = mRot.TransformNormal(tmpVec);

  pVec->Set(tmpVec.x,tmpVec.y,tmpVec.z);
}
void FinderStudioCameraSystem::RotateXVec(gfl2::math::Vector3* pVec,f32 deg)
{
  if(!pVec) GFL_ASSERT(0);

  gfl2::math::Vector4 tmpVec(pVec->x,pVec->y,pVec->z,0);
  gfl2::math::Quaternion quat(gfl2::math::Vector3(1,0,0),gfl2::math::ConvDegToRad(deg));
  gfl2::math::Matrix mRot;
  quat.QuaternionToMatrix(mRot);
  tmpVec = mRot.TransformNormal(tmpVec);

  pVec->Set(tmpVec.x,tmpVec.y,tmpVec.z);
}

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)
