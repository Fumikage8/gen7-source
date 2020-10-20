//======================================================================
/**
 * @file CharaViewerView.h
 * @date 2015/07/17 16:09:29
 * @author araki_syo
 * @brief キャラビューワー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_VIEW_H_INCLUDED
#define CHARA_VIEWER_VIEW_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIView.h>

#include "CharaViewerProcParam.h"
#include "CharaViewerProc.h"
#include "CharaViewerListener.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)
  class CharaViewerDataManager;
  class CharaViewerFrameCell;
  class CharaViewerView;
  class CharaViewerListener;
  class CharaViewerCharaModelManager;
GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)

//==============================================================================
/**
 @class     CharaView
 @brief     CharaViewerのビュー
 */
//==============================================================================
class CharaViewerView : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerView);

public:
  class Param
  {
  public:
    CharaViewerProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    CharaViewerDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    CharaViewerFrameCell*                 frameCell;
    CharaViewerCharaModelManager*   charaModelManager;

  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        frameCell(NULL),
        charaModelManager(NULL)
    {}
  };

public:

  enum ModelType {
    FIELD_MODEL,
    BATTLE_MODEL,
  };

  enum LightDirection {
    CENTER
  };

  enum CameraMode {
    NF_FIELD_NEAR,
    NF_FIELD_MID,
    NF_FIELD_FAR,
    NF_BATTLE
  };

  enum CameraAngle {
    FIELD,
    BATTLE
  };

  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  CharaViewerView(const Param& param);
  
  virtual ~CharaViewerView();

  void Init();
  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  void SetPage(u8 page){m_page = page;}

  void UpdatePage(bool isUp);
  void UpdatePageDisp(void);
  void UpdatePage1();
  void UpdatePage2();
  void UpdatePage3();
  void UpdatePage4();
  void UpdatePage5();
  void SlideModelSelector( bool isForward );

  void SetPicPaneCol( u32 pane , u8 r = 255, u8 g = 255, u8 b = 255,u8 a = 255);
  
  void SlideCharaSelector( bool isForward );
  void LoadCharactor();
  void UnloadCharactor();
  void SetModelType(ModelType modelType);
  void LocateOrigin();
  void SlideScaleSelector( bool isForward );
  void ResetAllParts();
  void SaveCurrentPartsSetting();

  void SlideSkinSelector( bool isForward );
  void SlideFrontHairSelector( bool isForward );
  void SlideHairStyleSelector( bool isForward );
  void SlideHeadWearSelector( bool isForward );
  void SlideHairAccessorySelector( bool isForward );
  void SlideEyeWearSelector( bool isForward );
  void SlideTopsSelector( bool isForward );
  void SlideOnepieceSelector( bool isForward );
  void SlideBottomsSelector( bool isForward );
  void SlideSocksSelector( bool isForward );
  void SlideShoesSelector( bool isForward );
  void SlideBagSelector( bool isForward );
  void SlideBangleSelector( bool isForward );
  void SlideCharmSelector( bool isForward );
  void SlideLipSelector( bool isForward );
  void SlideSituationSelector( bool isForward );

  void SlideEyeSelector( bool isForward );
  void SlideMouthSelector( bool isForward );
  void ToggleAutoBlink();

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

  void SelectSkinColor( u32 index );
  void SelectHairColor( u32 index );
  void SelectClothColor( u32 index );
  void SelectEyeblowColor( u32 index );
  void SelectContactColor( u32 index );
  void SelectLipColor( u32 index );

  void ToggleBeadsOnOff();
  void ToggleCharmOnOff();
  void LoadCollisionCheckParts();
  void LoadDataSizeCheckParts();

  void ToggleLoopFlag();
  void PlaySingleFrame( bool isForward );
  void PlayMotion();
  void ToggleAllPlayFlag();
  void ToggleTranslateFlag();
  void SlideFpsSelector( bool isForward );

  void SlideMotionSelector( u8 index, bool isForward );
  void ToggleMotion( u8 index );

  void PlayTPose();
  void PlayStackCheckMotion();

  void ResetLight();
  void ResetCamera();
  void SetCameraMode( CameraMode mode);
  void SetCameraAngle( CameraAngle angle);
  void SlideCameraAngleSelector( bool isForward);
  void ToggleCameraControllMode();
  void ToggleGrid();
  void TogglePolygon();
  void ToggleCollision();
  void SetClearColorType(u32 clearColorType);
  void Capture();
  void CaptureAll();
  void DressUpRand();
  void DressUpDump();

public:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void ReqEnd(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  CharaViewerListener* m_viewListener;

  s8 m_page;

  static const int maxPage = 4;
};


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_VIEW_H_INCLUDED

#endif // PM_DEBUG
