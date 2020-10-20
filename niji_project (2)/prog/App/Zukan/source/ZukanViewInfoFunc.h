#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFOFUNC_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFOFUNC_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewInfoFunc.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <str/include/gfl2_Font.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)
  class PokeSimpleModelInFrame;
GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanDataManager;
  class ZukanPokeIconTextureManager;
  class ZukanFrameInfoCell;
  class ZukanViewInfoListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewInfoFunc
 @brief     図鑑の情報画面のビュー
 */
//==============================================================================
class ZukanViewInfoFunc
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewInfoFunc);

public:
  ZukanViewInfoFunc(const ZukanSceneParam& param);
  virtual ~ZukanViewInfoFunc();

public:
  void SetPointer(gfl2::lyt::LytWk* lytWk_0, gfl2::lyt::LytMultiResID lytRes_0);
  void SetContents(
      app::util::G2DUtil*  pG2DUtil,
      gfl2::str::MsgData*  msgDataZukan,
      gfl2::str::MsgData*  msgDataPokeName,
      gfl2::str::MsgData*  msgDataPokeSpecies,
      gfl2::str::MsgData*  msgDataPokeForm,
      gfl2::str::MsgData*  msgDataPokeHeight,
      gfl2::str::MsgData*  msgDataPokeWeight,
      gfl2::str::MsgData*  msgDataPokeComment,
      ZukanType::ZukanLang zukanLangForType,
      gfl2::str::Font*     langFont
  );

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // ペインのポインタを覚えておく
  gfl2::lyt::LytTextBox*  m_pokeNoTextPane;       // 025
  gfl2::lyt::LytTextBox*  m_pokeNameTextPane;     // ピカチュウ
  gfl2::lyt::LytTextBox*  m_pokeSpeciesTextPane;  // ねずみポケモン
  gfl2::lyt::LytTextBox*  m_formNameTextPane;
  gfl2::lyt::LytTextBox*  m_heightLabelTextPane;
  gfl2::lyt::LytTextBox*  m_weightLabelTextPane;
  gfl2::lyt::LytTextBox*  m_heightValTextPane;
  gfl2::lyt::LytTextBox*  m_weightValTextPane;
  gfl2::lyt::LytTextBox*  m_commentTextPane;
  gfl2::lyt::LytPicture*  m_type1PicPane;
  gfl2::lyt::LytPicture*  m_type2PicPane;

  /*
  Controllerとともに削除
  // ポケモンモデル操作に関する変数
  app::tool::PokeSimpleModelInFrame*  m_pokeSimpleModelInFrame;
  PokeTool::SimpleParam               m_pokeModelSp;
  // 画面ごとに決定
  s32  m_pokeModelCenterX;  // 画面の左上が(0,0)、右下が(400,240)
  s32  m_pokeModelCenterY;
  s32  m_pokeModelBaseSize;  // 1辺の長さ。画面に最大の正方形で出したいなら240
  f32  m_pokeModelZoomDefault;
  // ポケモンごとに決定
  f32  m_pokeModelZoomMin;
  f32  m_pokeModelZoomMax;
  f32  m_pokeModelPitchMax;  // [radian]
  f32  m_pokeModelPitchMin;  // [radian]
  // 操作で変わるもの
  f32  m_pokeModelZoom;
  //f32  m_pokeModelYaw;  // [radian]  // 1周できる。1周したら正規化しておく。
  //f32  m_pokeModelPitch;  // [radian]
  */
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWINFOFUNC_H_INCLUDED

