//======================================================================
/**
 * @file CharaViewerCharaModelManager.h
 * @date 2015/07/24 20:05:03
 * @author takebe_makoto
 * @brief キャラビューア：キャラモデル管理：アニメーション
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined __CHARA_VIEWER_CHARA_MODEL_MANAGER__
#define __CHARA_VIEWER_CHARA_MODEL_MANAGER__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"
#include <system/include/nijiAllocator.h>

#include <model/include/gfl2_CharaModelFactory.h>
#include <model/include/gfl2_CharaModel.h>

#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

#include "CharaViewerHeaderId.h"
#include "CharaViewerCharaModelManager_Animation.h"
#include "CharaViewerRenderingPipeline.h"

#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

class GridModel;
class FactoryParam;

/**
 * @class CharaViewerCharaModelManager
 * @brief キャラモデル管理
 */
class CharaViewerCharaModelManager
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerCharaModelManager);

public:

  enum DressUpType
  {
    DRESS_UP_TYPE_NONE,
    DRESS_UP_TYPE_HERO,
    DRESS_UP_TYPE_HEROINE
  };

  enum ModelType
  {
    MODEL_TYPE_BATTLE,
    MODEL_TYPE_FIELD,

    MODEL_TYPE_COUNT
  };

  enum FovyType
  {
    FOVY_TYPE_FIELD,
    FOVY_TYPE_BATTLE
  };

  enum NearFarType
  {
    NEAR_FAR_TYPE_FIELD_NEAR,
    NEAR_FAR_TYPE_FIELD_MIDDLE,
    NEAR_FAR_TYPE_FIELD_FAR,
    NEAR_FAR_TYPE_BATTLE
  };

  enum
  {
    MODEL_TAB_LIMIT = 4
  };

  enum UpdateType
  {
    UPDATE_TYPE_NORMAL,
    UPDATE_TYPE_CAPTURE_ALL,
    UPDATE_TYPE_MEASURE_NORMAL_ALL,
    UPDATE_TYPE_MEASURE_DRESSUP_ALL
  };

public:

  CharaViewerCharaModelManager();
  ~CharaViewerCharaModelManager();

  void Initialize(GameSys::GameManager* pGameManager, gfl2::heap::HeapBase* pDeviceHeap, b32 fieldDebugModeEnabled, b32 measureModeEnabled);
  void Finalize();

  template <poke_3d::model::DressUpParam::BodyCategory bodyCategory>
  static void DebugMenuBodyUpdate(void* pUserWork, gfl2::debug::DebugWinItem* pItem);

  template <poke_3d::model::DressUpParam::BodyCategory bodyCategory>
  static wchar_t* DebugMenuBodyDraw(void* pUserWork, gfl2::debug::DebugWinItem* pItem);

  template <poke_3d::model::DressUpParam::ItemCategory itemCategory>
  static void DebugMenuItemUpdate(void* pUserWork, gfl2::debug::DebugWinItem* pItem);

  template <poke_3d::model::DressUpParam::ItemCategory itemCategory>
  static wchar_t* DebugMenuItemDraw(void* pUserWork, gfl2::debug::DebugWinItem* pItem);

  void InitializeDebugMenu();
  void FinalizeDebugMenu();

  void Update();
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  void UpdateNormal();
  void UpdateCaptureAll();
  void UpdateMeasureNormalAll();
  void UpdateMeasureDressupAll();

  void ResetCameraFovyNearFar();
  void ResetCameraPosition();
  void ResetLightPosition();
  void UpdateCameraPosition();
  void UpdateModelPosition();
  void UpdateLightPosition();

  // 1ページ目
  void SetLoadModelType(ModelType modelType);
  ModelType GetLoadModelType() const;
  u32 GetCurrentModel() const;
  void GetCurrentCharaName(const char** out_pCharaName, s32* out_pCharaId) const;
  f32 GetCurrentCharaScale() const;
  DressUpType GetCurrentCharaDressUpType() const;
  void MakeDressUpParamDefault(DressUpType dressUpType, const FactoryParam* pFactoryParam);
  bool IsCurrentCharaDressUpModel() const;
  void SlideModelSelector(bool isForward);
  void SlideCharaSelector(bool isForward);
  void LoadCharactor();
  void ReloadCharactor();
  void UnloadCharactor();
  void LocateOrigin();
  void SlideScaleSelector(bool isForward);
  u32 GetPolygonCount() const;
  u32 GetMaterialCount() const;
  u32 GetMeshCount() const;
  u32 GetVertexCount() const;
  u32 GetTextureCount() const;
  u32 GetBoneCount() const;
  u32 GetResModelSize() const;
  u32 GetResAnimSize() const;
  u32 GetResTotalSize() const;
  u32 GetInstanceSize() const;

  // 2ページ目
  bool IsClothColorHat() const;
  bool IsClothColorAccHair() const;
  bool IsClothColorAccEye() const;
  bool IsClothColorTops() const;
  bool IsClothColorOnepiece() const;
  bool IsClothColorBottoms() const;
  bool IsClothColorLegs() const;
  bool IsClothColorShoes() const;
  bool IsClothColorBag() const;
  bool IsClothColorBangle() const;
  void SetClothColorHat();
  void SetClothColorAccHair();
  void SetClothColorAccEye();
  void SetClothColorTops();
  void SetClothColorOnepiece();
  void SetClothColorBottoms();
  void SetClothColorLegs();
  void SetClothColorShoes();
  void SetClothColorBag();
  void SetClothColorBangle();
  void SetClothColorSituation();
  u32 GetSkinColor(u32 index) const;
  u32 GetHairColor(u32 index) const;
  u32 GetClothColor(u32 index) const;
  void ChangeSkinColor(u32 index);
  void ChangeHairColor(u32 index);
  void ChangeClothColor(u32 index);
  s32 GetSkinColorIndex() const;
  s32 GetHairColorIndex() const;
  s32 GetClothColorIndex() const;
  void GetCurrentPartsNameFace(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameHairFront(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameHairStyle(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameHat(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameAccHair(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameAccEye(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameTops(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameOnePiece(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameBottoms(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameSocks(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameShoes(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameBag(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void SlideSkinSelector(bool isForward);
  void SlideFrontHairSelector(bool isForward);
  void SlideHairStyleSelector(bool isForward);
  void SlideHeadWearSelector(bool isForward);
  void SlideHairAccessorySelector(bool isForward);
  void SlideEyeWearSelector(bool isForward);
  void SlideTopsSelector(bool isForward);
  void SlideOnepieceSelector(bool isForward);
  void SlideBottomsSelector(bool isForward);
  void SlideSocksSelector(bool isForward);
  void SlideShoesSelector(bool isForward);
  void SlideBagSelector(bool isForward);

  // 3ページ目
  u32 GetEyeblowColor(u32 index) const;
  u32 GetEyeColor(u32 index) const;
  u32 GetLipColor(u32 index) const;
  void ChangeEyeblowColor(u32 index);
  void ChangeEyeColor(u32 index);
  void ChangeLipColor(u32 index);
  u32 GetEyeblowColorIndex() const;
  u32 GetEyeColorIndex() const;
  u32 GetLipColorIndex() const;
  void GetCurrentPartsNameBngl(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameCharm(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameLip(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void GetCurrentPartsNameSituation(const char** out_ppPartsName, s32* out_pPartsIndex) const;
  void SlideBangleSelector(bool isForward);
  void SlideCharmSelector(bool isForward);
  void SlideLipSelector(bool isForward);
  void SlideSituationSelector(bool isForward);
  void ToggleBeadsOnOff();
  void SetBeadsOnOff(bool on);
  bool IsBeadsOn() const;
  void SetCharmOnOff(bool on);
  void ToggleCharmOnOff();
  bool IsCharmOn() const;
  void LoadCollisionCheckParts();
  void LoadDataSizeCheckParts();
  void SlideEyeSelector(bool isForward);
  void SlideMouthSelector(bool isForward);
  const char* GetCurrentEyeName() const;
  const char* GetCurrentMouthName() const;
  void ToggleAutoBlink();
  bool IsAutoBlinkOn() const;

  // 4ページ目
  bool IsFpsMode30() const;
  void GetCurrentMotionName(u32 index, const char** out_ppMotionName, s32* out_pListIndex, s32* out_pMotionIndex) const;
  u32 GetCurrentMotionTotalFrame(u32 index) const;
  u32 GetCurrentMotionFrame(u32 index) const;
  bool IsMotionEnabled(u32 index) const;
  bool IsMotionLoop() const;
  void ToggleLoopFlag();
  void PlaySingleFrame(bool isForward);
  bool IsMotionPlaying() const;
  void PlayMotion();
  void SlideMotionSelector(u8 index, bool isForward);
  void ToggleMotion(u8 index);
  void PlayTPose();
  void PlayStackCheckMotion();
  void ToggleAllPlayFlag();
  bool GetTranslateFlag() const;
  void ToggleTranslateFlag();
  void SlideFpsSelector(bool isForward);

  // 5ページ目
  bool IsControllModeCamera() const;
  void ToggleCameraControllMode();
  bool IsGridVisible() const;
  void ToggleGrid();
  void SlideCameraAngleSelector(bool isForward);
  u32 GetCameraFovy() const;
  void SetCameraFovyType(FovyType type);
  FovyType GetCameraFovyType() const;
  void SetCameraNearFarType(NearFarType type);
  NearFarType GetCameraNearFarType() const;
  void SetClearColorType(u32 clearColorType);
  u32 GetClearColorType() const;
  void Capture();
  void CaptureAll();
  void MeasureNormal();
  void MeasureNormalAll();
  void MeasureDressup();
  void MeasureDressupAll();
  void DressUpRand();
  void DressUpDump();
  void ResetAllParts();
  void SaveCurrentPartsSetting();

private:

  void LoadCharactorCore(u32 charaId, DressUpType dressUpType, const poke_3d::model::DressUpParam& dressUpParam, FactoryParam* pFactoryParam);
  void FixPartsCombination(poke_3d::model::DressUpParam::ItemCategory itemCategory, s32 nextItemIndex, poke_3d::model::DressUpModelResourceManagerCore* pDressUpModelResourceManager);
  void SelectNextParts(poke_3d::model::DressUpParam::ItemCategory itemCategory, bool isForward);
  const char* GetPartsName(poke_3d::model::DressUpParam::ItemCategory itemCategory, u32 index) const;
  void GetCurrentPartsName(poke_3d::model::DressUpParam::ItemCategory itemCategory, const char** out_ppPartsName, s32* out_pPartsIndex) const;

  FactoryParam* GetCurrentFactoryParam();
  const FactoryParam* GetCurrentFactoryParam() const;
  s32 GetSlideValue(bool isForward) const;

private:

  struct LoadedModel
  {
    LoadedModel() :
      m_pCharaModel(NULL),
      m_pSubObjectModel(NULL),
      m_CharaId(-1),
      m_SubObjectId(-1),
      m_ModelScale(100),
      m_DressUpType(DRESS_UP_TYPE_NONE),
      m_pFactoryParam(NULL),
      m_pDressUpModelResourceManager(NULL),
      m_pMotionIdList(NULL),
      m_MotionIdListCount(0),
      m_NormalResModelSize(0),
      m_NormalResAnimSize(0),
      m_NormalResTotalSize(0),
      m_DressUpResModelSize(0),
      m_DressUpResAnimSize(0),
      m_DressUpResTotalSize(0),
      m_InstanceSize(0),
      m_Position(0, 0, 0),
      m_Rotation(0, 0, 0)
    {
    }

    poke_3d::model::CharaModel* m_pCharaModel;
    poke_3d::model::CharaModel* m_pSubObjectModel;
    s32 m_CharaId;
    s32 m_SubObjectId;
    s32 m_ModelScale;
    DressUpType m_DressUpType;
    FactoryParam* m_pFactoryParam;
    poke_3d::model::DressUpModelResourceManagerCore* m_pDressUpModelResourceManager;
    CharaViewerCharaModelManager_Animation m_Animation;
    const HeaderIdList** m_pMotionIdList;
    u32 m_MotionIdListCount;
    u32 m_NormalResModelSize;
    u32 m_NormalResAnimSize;
    u32 m_NormalResTotalSize;
    u32 m_DressUpResModelSize;
    u32 m_DressUpResAnimSize;
    u32 m_DressUpResTotalSize;
    u32 m_InstanceSize;
    gfl2::math::Vector3 m_Position;
    gfl2::math::Vector3 m_Rotation;
  };

  struct MeasureDressupParam
  {
    s32 updateTick;
    s32 drawTick;
    s32 gpuTick;
    u32 resModelSize;
    u32 resAnimSize;
    u32 resTotalSize;
    u32 animHeapSize;
    u32 modelHeapSize;
    u32 instanceHeapSize;
    u32 allocatorHeapSize;
    u32 materialCount;
    u32 textureCount;
    u32 meshCount;
    u32 vertexCount;
    u32 polygonCount;
    u32 boneCount;
  };

  LoadedModel* GetLoadedModel()
  {
    return &m_LoadedModel[m_SelectedModel];
  }

  const LoadedModel* GetLoadedModel() const
  {
    return &m_LoadedModel[m_SelectedModel];
  }

  poke_3d::model::DressUpModelResourceManagerCore* GetDefaultDressUpResourceManager();
  const poke_3d::model::DressUpModelResourceManagerCore* GetDefaultDressUpResourceManager() const;

  void GetMaxInstanceSizeParts(s16* out_pInstanceIndex, s16* out_pAllocatorIndex, poke_3d::model::DressUpParam::ItemCategory itemCategory, bool hatEnable);

private:

	gfl2::heap::HeapBase* m_pDeviceHeap;
	gfl2::heap::HeapBase* m_pHeap;
	gfl2::heap::HeapBase* m_pHeap2; // メモリ足りなくなったので追加
	gfl2::heap::HeapBase* m_pHeap3; // メモリ足りなくなったので追加
	gfl2::heap::HeapBase* m_pDressUpAnimHeap;
	gfl2::heap::HeapBase* m_pDressUpModelHeap;
	gfl2::heap::HeapBase* m_pDressUpInstanceHeap;
	gfl2::heap::HeapBase* m_pDressUpAllocatorHeap;
  System::nijiAllocator* m_pNijiAllocator;
  System::nijiAllocator* m_pDressUpAllocator;

  u32 m_DressUpAnimHeapBeginSize;
	u32 m_DressUpModelHeapBeginSize;
	u32 m_DressUpInstanceHeapBeginSize;
	u32 m_DressUpAllocatorHeapBeginSize;

  s32 m_StartFreeSize;
  poke_3d::model::DressUpParam m_DressUpParam[MODEL_TAB_LIMIT];
  u32 m_InitSeq;
  u32 m_UpdateSeq;
  u32 m_EndSeq;

  s32 m_Counter;

	CharaViewerRenderingPipeline* m_pRenderingPipeLine;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::LightNode* m_pDirectionalLightNode;

  GameSys::GameManager* m_pGameManager;

  s32 m_CharaId;
  LoadedModel m_LoadedModel[MODEL_TAB_LIMIT];
  u32 m_SelectedModel;

  poke_3d::model::DressUpParam::ItemCategory m_ClothColorSelectedItemCategory;

  bool m_IsControllModeCamera;
  gfl2::math::Vector3 m_CameraPosition;
  gfl2::math::Vector3 m_CameraTarget;
  gfl2::math::Vector3 m_CameraRotate;
  f32 m_CameraLength;
  u32 m_CameraFovy;
  f32 m_CameraNear;
  f32 m_CameraFar;
  FovyType m_CameraFovyType;
  NearFarType m_CameraNearFarType;

  gfl2::math::Vector3 m_LightRotate;

  GridModel* m_pGridModel;
  ModelType m_LoadModelType;
  FactoryParam* m_pFactoryParamList[MODEL_TYPE_COUNT];

  b32 m_AllPlayFlag;

  GameSys::GameManager::BUFFER_CLEAR_SETTING m_oldBufferClearSettingList[gfl2::gfx::CtrDisplayNo::NumberOf];
  u32 m_ClearColorType;

  UpdateType m_UpdateType;
  s32 m_CaptureAnimationId;
  s32 m_MeasureDressupItemCategory;
  u32 m_MeasureDressupItemNo;
  MeasureDressupParam m_MeasureDressupBaseParam;

  s64 m_BaseUpdateTick;
  s64 m_BaseUpnextTick;
  s64 m_BaseDrawTick;
  s64 m_BaseGpuTick;

  s16* m_pCheckPartsList;
  void* m_BattleTrainerExtraDataBuf;

  void *m_BattleDataPackBuf;

  b32 m_FieldDebugModeEnabled;
  b32 m_MeasureModeEnabled;

  char* m_pMeasureDumpBuffer;
  s32 m_MeasureDumpRemainSize;
  const char* m_pMeasureCSVFileName;

  b32 m_DebugMenuReloadRequest;
  gfl2::debug::DebugWinGroup* m_pDressupGroup;
  gfl2::debug::DebugWinItem* m_pSaveDataItem;
};

GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // __HEADER_H_INCLUDED__

#endif // PM_DEBUG
