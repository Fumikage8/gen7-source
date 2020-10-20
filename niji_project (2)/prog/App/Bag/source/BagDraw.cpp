//=============================================================================
/**
 * @file    BagDraw.cpp
 * @brief   バッグ画面表示
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================

// module
#include "BagDraw.h"
#include "BagPokemon.h"
#include "App/FieldBag/include/BagInfoField.h"
#include "App/Bag/include/BagDrawListener.h"
#include "App/Bag/include/BagAppParam.h"
// gflib2
#include "model/include/gfl2_BaseCamera.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "Print/include/SystemFont.h"
#include "AppLib/include/Util/AppCamera.h"
#include "AppLib/include/Tool/app_tool_MenuWindow.h"
#include "AppLib/include/Tool/ItemIcon.h"
#include "AppLib/include/Tool/AppCommonGrpIconData.h"
#include "AppLib/include/Tool/AppToolItemEffect.h"
#include "Sound/include/Sound.h"
// resource
#include "arc_index/message.gaix"
#include "niji_conv_header/message/msg_bag.h"
#include "niji_conv_header/app/common_bag/CommonBag.h"
#include "niji_conv_header/app/common_bag/CommonBag_pane.h"
#include "niji_conv_header/app/common_bag/CommonBag_anim_list.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(bag)

//! フィールド用ポケットカラーアニメ
const BagDraw::PageColorAnime BagDraw::FieldPocketAnime[] =
{
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_ICON_15 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_ICON_07 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_ICON_08 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_ICON_10 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_ICON_11 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_ICON_12 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_ICON_13 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_ICON_09 },
};

//! バトル用ポケットカラーアニメ
const BagDraw::PageColorAnime BagDraw::BattlePocketAnime[] =
{
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_ICON_15 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_ICON_07 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_ICON_08 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_ICON_09 },
  { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_ICON_10 },
};

//! フィールド用リストカラーアニメ
const BagDraw::PageColorAnime BagDraw::FieldPaneListItemColorTable[][BagDraw::FIELD_POCKET_MAX] =
{
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_09 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_01, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_08 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_10 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_06, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_13 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_02, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_09 },
  },
};

//! バトル用リストカラーアニメ
const BagDraw::PageColorAnime BagDraw::BattlePaneListItemColorTable[][BagDraw::BATTLE_POCKET_MAX] =
{
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_ICON_10 },
  },
  {
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_14, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_15 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_00, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_07 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_04, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_11 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_05, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_12 },
    { LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_03, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_ICON_10 },
  },
};

//! ポケットペインテーブル
const gfl2::lyt::LytPaneIndex BagDraw::PocketBasePaneTable[FIELD_POCKET_MAX] =
{
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_07,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_00,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_01,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_02,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_03,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_04,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_05,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_06,
};

//! リスト項目ペインテーブル
const gfl2::lyt::LytPaneIndex BagDraw::ListBasePaneTable[LIST_ITEM_PANE_MAX] =
{
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_00,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_01,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_02,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_03,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_04,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_05,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_06,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_07,
  PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_08,
};

const f32 BagDraw::LIST_ITEM_PX = 78.0f;  //!< リスト開始X座標
const f32 BagDraw::LIST_ITEM_PY = 99.0f;  //!< リスト開始Y座標
const f32 BagDraw::LIST_ITEM_SX = 164.0f; //!< リスト項目Xサイズ
const f32 BagDraw::LIST_ITEM_SY = 28.0f;  //!< リスト項目Yサイズ


//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap              ヒープ
 * @param   rendering_manager 描画マネージャー
 * @param   is_demo_mode      デモモードフラグ
 */
//-----------------------------------------------------------------------------
BagDraw::BagDraw( app::util::Heap * heap, app::util::AppRenderingManager * rendering_manager, app::tool::AppCommonGrpIconData * common_icon, const bool * is_demo_mode )
  : app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
  , m_heap( heap )
  , m_renderingManager( rendering_manager )
  , m_commonIcon( common_icon )
  , m_appParam( NULL )
  , m_myStatus( NULL )
  , m_playData( NULL )
  , m_myItem( NULL )
  , m_nowPocketItem( NULL )
  , m_nowPocketItemMax( 0 )
  , m_uiListener( NULL )
  , m_pMsgData( NULL )
  , m_pWordSet( NULL )
  , m_pMenuCursor( NULL )
  , m_pPaneListView( NULL )
  , m_pPaneListData( NULL )
  , m_isPaneListChangeMode( false )
  , m_isPaneListCatchMode( false )
  , m_changeItem( ITEM_DUMMY_DATA )
  , m_pocket( 0 )
  , m_createNumFont( false )
  , m_inputMode( MODE_NORMAL )
  , m_msgFinishType( gfl2::str::StrWin::FINISH_NONE )
  , m_isMsgEnd( true )  // 終了状態にしておく
  , m_msgCallBackWrok( 0 )
  , m_isMsgWinPut( false )
  , m_isSelectWinPut( false )
  , m_lastTouchX( 0 )
  , m_lastTouchY( 0 )
  , m_itemPutPosID( 0 )
  , m_itemPutIndex( 0 )
  , m_itemCalcX( 0.0f )
  , m_itemCalcY( 0.0f )
  , m_listItemBaseY( 0.0f )
  , m_isDemoMode( is_demo_mode )
  , m_demoAnime( 0 )
  , m_itemEffect( NULL )
  , m_pAppCamera( NULL )
  , m_isDrawEnable( true )
  , m_pocketOnBit( NULL )
{
  GetGameData();
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagDraw::~BagDraw()
{
}

//-----------------------------------------------------------------------------
/**
 * @func    GetGameData
 * @brief   ゲームデータ関連取得
 * @date    2015.11.26
 */
//-----------------------------------------------------------------------------
void BagDraw::GetGameData(void)
{
  GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();

  m_myStatus = gd->GetPlayerStatus();
  m_playData = gd->GetBagPlayData();
  m_myItem   = gd->GetMyItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetInputEnabled
 * @brief   入力判定の切り替え
 * @date    2015.06.25
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagDraw::SetInputEnabled( bool flg )
{
  if( *m_isDemoMode != false )
  {
    flg = false;
  }
  app::ui::UIResponder::SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    Init
 * @brief   初期化処理
 * @date    2015.03.05
 *
 * @param   param         外部設定パラメータ
 * @param   res_buf       レイアウトリソースバッファ
 * @param   menu_cursor   メニューカーソル
 * @param   pocket_bit    ポケット表示フラグ
 */
//-----------------------------------------------------------------------------
void BagDraw::Init( APP_PARAM * param, void * res_buf, void * msg_buf, WordSetLoader * wset_loader, app::tool::MenuCursor * menu_cursor, const u8 * pocket_bit )
{
  m_appParam = param;
  m_pocketOnBit = pocket_bit;

  Initialize2D( res_buf, msg_buf, wset_loader );
//  InitializeText();

  CreateMenuCursorLayout( menu_cursor );

  SetMainControl();
  SetInputListener( this );
  SetInputEnabled( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    End
 * @brief   終了処理
 * @date    2015.03.05
 *
 * @retval  true  = 処理完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::End(void)
{
  m_isDrawEnable = false;
  if( IsDrawing() != false )
  {
    return false;
  }
  if( DeleteMenuCursorLayout() == false )
  {
    return false;
  }
  Terminate2D();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    Start
 * @brief   開始処理
 * @date    2015.03.23
 *
 * @retval  false = 処理中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::Start(void)
{

/*
  app::util::G2DUtil * g2d = GetG2DUtil();

  if( g2d->IsAnimeEnd(0,LA_COMMONBAG_BAG_MIN_LOW_000_IN) == false )
  {
    return false;
  }
*/
  SetInputEnabled( true );
  SetPaneListInputEnable( true );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    StartBattleMode
 * @brief   バトル用追加開始処理
 * @date    2015.06.09
 */
//-----------------------------------------------------------------------------
void BagDraw::StartBattleMode(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    GetG2DUtil()->SetLayoutDrawEnable( 0, true );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    StartShopMode
 * @brief   ショップ用追加開始処理
 * @date    2015.07.16
 */
//-----------------------------------------------------------------------------
void BagDraw::StartShopMode(void)
{
  if( m_appParam->call_mode == CALL_SHOP )
  {
    GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_COLOR_SHOP );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDrawListener
 * @brief   表示リスナーをセット
 * @date    2015.03.05
 *
 * @param   listener  表示リスナー
 */
//-----------------------------------------------------------------------------
void BagDraw::SetDrawListener( BagDrawListener * listener )
{
  m_uiListener = listener;
}

//-----------------------------------------------------------------------------
/**
 * @func    Update
 * @brief   更新
 * @date    2015.03.05
 */
//-----------------------------------------------------------------------------
void BagDraw::Update(void)
{
  app::ui::UIView::Update();
  UpdateMenuCursor();

  if( m_itemEffect != NULL )
  {
    m_itemEffect->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Draw
 * @brief   描画
 * @date    2015.03.05
 */
//-----------------------------------------------------------------------------
void BagDraw::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_isDrawEnable != false )
  {
    GetG2DUtil()->AddDrawableLytWkForOneFrame( m_renderingManager, displayNo, 0 );
    DrawMenuCursor( displayNo );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateNumFont
 * @brief   NUMフォント生成
 * @date    2015.07.18
 */
//-----------------------------------------------------------------------------
void BagDraw::CreateNumFont(void)
{
  if( print::SystemFont_GetNumFont() == NULL )
  {
    print::SystemFont_LoadNumFont( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap() );
    m_createNumFont = true;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNumFont
 * @brief   NUMフォントをセット
 * @date    2015.07.18
 *
 * @retval  true  = 完了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::SetNumFont(void)
{
  if( m_createNumFont == false )
  {
    return true;
  }
  if( print::SystemFont_IsLoadNumFont() == false )
  {
    return false;
  }
  print::SystemFont_SetLoadNumFont( m_heap->GetSystemHeap() );
  print::SystemFont_SetLytSystemNumFont();
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteNumFont
 * @brief   NUMフォント削除
 * @date    2015.07.18
 */
//-----------------------------------------------------------------------------
void BagDraw::DeleteNumFont(void)
{
  if( m_createNumFont == true )
  {
    print::SystemFont_ResetNumFontInfo();
    print::SystemFont_DeleteNumFont();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    Initialize2D
 * @brief   2D関連初期化
 * @date    2015.03.05
 *
 * @param   res_buf   レイアウトリソースバッファ
 */
//-----------------------------------------------------------------------------
void BagDraw::Initialize2D( void * res_buf, void * msg_buf, WordSetLoader * wset_loader )
{
/*
  if( print::SystemFont_GetNumFont() == NULL )
  {
    print::SystemFont_CreateNumFont( m_heap->GetSystemHeap(), m_heap->GetDeviceHeap() );
    print::SystemFont_SetLytSystemNumFont();
    m_createNumFont = true;
  }
*/

  // リソースデータ
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { res_buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };
  // レイアウトデータ
	const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
      LYTRES_COMMONBAG_BAG_MIN_LOW_000_BFLYT,
      LA_COMMONBAG_BAG_MIN_LOW_000___NUM,
      res_buf,
      true,
      app::util::G2DUtil::SETUP_LOWER,
      true
    },
  };

  m_pMsgData = GFL_NEW(m_heap->GetDeviceHeap()) gfl2::str::MsgData( msg_buf, m_heap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(m_heap->GetDeviceHeap()) print::WordSet( m_heap->GetDeviceHeap() );

  SetWordSetLoader( wset_loader );

  Create2D(
    m_heap,
    NULL,
    LYTID_MAX,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

  SetListItemBaseY();

  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    // 初期状態でレイアウトをOFFに
    GetG2DUtil()->SetLayoutDrawEnable( 0, false );
  }
  else
  {
    StartItemNewIconAnime();
  }

  CreateMessageWindow();
}

//-----------------------------------------------------------------------------
/**
 * @func    Terminate2D
 * @brief   2D関連削除
 * @date    2015.03.23
 */
//-----------------------------------------------------------------------------
void BagDraw::Terminate2D(void)
{
  DeleteMessageWindow();

  Delete2D();

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );

/*
  if( m_createNumFont == true )
  {
    print::SystemFont_ResetNumFontInfo();
    print::SystemFont_DeleteNumFont();
  }
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    StartAnime
 * @brief   アニメ開始
 * @date    2015.04.10
 *
 * @param   id  アニメID
 */
//-----------------------------------------------------------------------------
/*
void BagDraw::StartAnime( u32 id )
{
  GetG2DUtil()->StartAnime( 0, id );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    IsAnimeEnd
 * @brief   アニメ終了チェック
 * @date    2015.04.10
 *
 * @param   id  アニメID
 *
 * @retval  false = 再生中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsAnimeEnd( u32 id )
{
  return GetG2DUtil()->IsAnimeEnd( 0, id );
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMenuCursorLayout
 * @brief   メニューカーソル用レイアウト作成
 * @date    2015.04.10
 *
 * @param   menu_cursor   メニューカーソルクラス
 */
//-----------------------------------------------------------------------------
void BagDraw::CreateMenuCursorLayout( app::tool::MenuCursor * menu_cursor )
{
  m_pMenuCursor = menu_cursor;

  app::util::G2DUtil * g2d = GetG2DUtil();

  m_pMenuCursor->CreateLayoutWork(
    g2d->GetLayoutSystem(),
    g2d->GetLayoutWorkSetup( gfl2::lyt::DISPLAY_LOWER ),
    g2d->GetAppLytAccessor(),
    app::tool::MenuCursor::SIZE_SMALL,
    false );
  m_pMenuCursor->SetVisible( true );
  m_pMenuCursor->StartAnime();
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMenuCursorLayout
 * @brief   メニューカーソル用レイアウト削除
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
bool BagDraw::DeleteMenuCursorLayout(void)
{
  return m_pMenuCursor->DeleteLayoutWork();
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMenuCursor
 * @brief   メニューカーソル更新
 * @date    2015.03.25
 */
//-----------------------------------------------------------------------------
void BagDraw::UpdateMenuCursor(void)
{
  if( m_pMenuCursor != NULL )
  {
    m_pMenuCursor->Update();
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    DrawMenuCursor
 * @brief   メニューカーソル描画
 * @date    2015.03.25
 */
//-----------------------------------------------------------------------------
void BagDraw::DrawMenuCursor( gfl2::gfx::CtrDisplayNo displayNo )
{
  if( m_pMenuCursor != NULL )
  {
    m_pMenuCursor->Draw( m_renderingManager, displayNo, gfl2::lyt::DISPLAY_LOWER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitDisplay
 * @brief   初期画面表示設定
 * @date    2015.03.23
 *
 * @param   pocket      ポケット番号
 * @param   bg_no       背景番号
 * @param   is_select   true = ポケモン選択状態
 * @param   is_message  true = メッセージ表示状態
 */
//-----------------------------------------------------------------------------
void BagDraw::InitDisplay( u32 pocket, u32 bg_no, bool is_select, bool is_message )
{
  WriteDefaultString();
  InitPocketAnime();
  // @note 旧ポケットと現在のポケットが被らないようにする
  if( pocket == 0 )
  {
    SetNowPocketAnime( 1, pocket, true );
  }
  else
  {
    SetNowPocketAnime( 0, pocket, true );
  }

  app::util::G2DUtil * g2d = UIView::GetG2DUtil();

  static const u32 tbl[][2] = 
  { // 背景, 色
    { LA_COMMONBAG_BAG_MIN_LOW_000_BAG_FIELD, LA_COMMONBAG_BAG_MIN_LOW_000_COLOR_NORMAL },
    { LA_COMMONBAG_BAG_MIN_LOW_000_BAG_BATTLE, LA_COMMONBAG_BAG_MIN_LOW_000_COLOR_NORMAL },
  };
  g2d->StartAnime( 0, tbl[bg_no][0] );
  g2d->StartAnime( 0, tbl[bg_no][1] );

  if( is_select != false )
  {
//    g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_IN );
    PutMessageWindowSmall( true );
  }
/*
  else if( is_message == false )
  {
    g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_IN );
  }
*/
}

//-----------------------------------------------------------------------------
/**
 * @func    StartInAnime
 * @brief   バッグ画面開始アニメ開始
 * @date    2015.05.15
 */
//-----------------------------------------------------------------------------
void BagDraw::StartInAnime(void)
{
//  GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_IN );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartOutAnime
 * @brief   終了アニメ開始
 * @date    2015.06.02
 */
//-----------------------------------------------------------------------------
void BagDraw::StartOutAnime(void)
{
//  GetG2DUtil()->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_OUT );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsOutAnime
 * @brief   終了アニメ終了チェック
 * @date    2015.06.02
 *
 * @retval  false = 処理中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsOutAnime(void)
{
//  return GetG2DUtil()->IsAnimeEnd( 0, LA_COMMONBAG_BAG_MIN_LOW_000_OUT );
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    WriteDefaultString
 * @brief   デフォルト文字列書き込み
 * @date    2015.03.23
 */
//-----------------------------------------------------------------------------
void BagDraw::WriteDefaultString(void)
{
  SetTextboxPaneMessage( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTBOX_00, msg_bag_01_03 );
  if( m_appParam->call_mode == CALL_SHOP )
  {
    SetTextboxPaneMessage( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTBOX_02, msg_bag_01_26 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMainControl
 * @brief   入力設定
 * @date    2015.03.24
 */
//-----------------------------------------------------------------------------
void BagDraw::SetMainControl(void)
{
  static const gfl2::lyt::LytPaneIndex parts_tbl[] =
  {
    // 戻るボタン
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_BACK,
    // ポケット
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_07,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_00,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_01,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_02,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_03,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_04,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_05,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEMTYPE_06,
  };

  static const app::ui::ButtonInfoEx def_info[] =
  {
    { // 戻るボタン
      BUTTON_ID_RETURN, NULL, NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_BACK_TOUCH_RELEASE,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
    },
    { // ポケット
      BUTTON_ID_POCKET, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+1, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+2, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+3, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+4, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+5, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+6, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_PASSIVE,
    },
    { // ポケット
      BUTTON_ID_POCKET+7, NULL, NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      app::tool::ButtonManager::ANIMATION_NULL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_ACTIVE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_PASSIVE,
    },
  };

/*
  u16 max;
  u16 active = m_appParam->pocket_max;
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    max = BATTLE_BUTTON_ID_MAX;
  }
  else
  {
    max = FIELD_BUTTON_ID_MAX;
  }
*/

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  app::ui::ButtonInfoEx * info = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::ui::ButtonInfoEx[FIELD_BUTTON_ID_MAX];

  { // 戻るボタン
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[BUTTON_ID_RETURN] );
    info[BUTTON_ID_RETURN] = def_info[BUTTON_ID_RETURN];
    info[BUTTON_ID_RETURN].picture_pane = parts;
    info[BUTTON_ID_RETURN].bound_pane   = lytwk->GetBoundingPane( parts, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &res_id );
  }

  // ポケット
//  for( u32 i=BUTTON_ID_POCKET; i<max; i++ )
  for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( parts_tbl[i] );
    info[i] = def_info[i];
    info[i].picture_pane = parts;
    info[i].bound_pane   = lytwk->GetBoundingPane( parts, PANENAME_BAG_BTN_LOW_001_PANE_BOUND_00, &res_id );
  }
//  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, max );
  app::ui::UIResponder::CreateButtonManager( m_heap, lytwk, info, FIELD_BUTTON_ID_MAX );

  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  // 戻るボタン
  man->SetButtonBindKey( BUTTON_ID_RETURN, gfl2::ui::BUTTON_B, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER );
  man->SetButtonSelectSE( BUTTON_ID_RETURN, SEQ_SE_CANCEL1 );
  // ポケット
//  for( u32 i=BUTTON_ID_POCKET; i<max; i++ )
  for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
  {
    man->SetButtonSelectSE( info[i].button_id, SEQ_SE_PAGE2 );
  }
/*
  for( u32 i=BUTTON_ID_POCKET+active; i<max; i++ )
  {
    man->SetButtonPassive( info[i].button_id, false );
    info[i].picture_pane->SetAlpha( 0 );
  }
*/
  for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
  {
    if( ( *m_pocketOnBit & (1<<(i-BUTTON_ID_POCKET)) ) == 0 )
    {
      man->SetButtonPassive( info[i].button_id, false );
      info[i].picture_pane->SetAlpha( 0 );
    }
  }

  GFL_DELETE_ARRAY info;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetExitButtonEnable
 * @brief   戻るボタンの入力許可設定
 * @date    2015.04.15
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagDraw::SetExitButtonEnable( bool flg )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
  man->SetButtonInputEnable( BUTTON_ID_RETURN, flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPocketButtonEnable
 * @brief   ポケットボタンの入力許可設定
 * @date    2015.04.15
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPocketButtonEnable( bool flg )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();
//  for( u32 i=BUTTON_ID_POCKET; i<GetPocketIDMax(); i++ )
  for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
  {
    if( ( *m_pocketOnBit & (1<<(i-BUTTON_ID_POCKET)) ) != 0 )
    {
      man->SetButtonInputEnable( i, flg );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPocketButtonActive
 * @brief   ポケットボタンのアクティブ/パッシブ切り替え
 * @date    2015.04.30
 *
 * @param   flg   true = アクティブ, false = パッシブ
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPocketButtonActive( bool flg )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

  if( flg != false )
  {
//    for( u32 i=BUTTON_ID_POCKET; i<GetPocketIDMax(); i++ )
    for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
    {
      if( ( *m_pocketOnBit & (1<<(i-BUTTON_ID_POCKET)) ) != 0 )
      {
        man->SetButtonActive( i );
      }
    }
  }
  else
  {
//    for( u32 i=BUTTON_ID_POCKET; i<GetPocketIDMax(); i++ )
    for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
    {
      if( ( *m_pocketOnBit & (1<<(i-BUTTON_ID_POCKET)) ) != 0 )
      {
        man->SetButtonPassive( i );
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPocketButtonActive
 * @brief   ポケットボタンのアクティブ/パッシブ切り替え
 * @date    2015.05.08
 *
 * @param   bit   切り替えるポケット（ビット指定）
 * @param   flg   true = アクティブ, false = パッシブ
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPocketButtonActive( u32 bit, bool flg )
{
  app::tool::ButtonManager * man  = app::ui::UIResponder::GetButtonManager();

//  for( u32 i=BUTTON_ID_POCKET; i<GetPocketIDMax(); i++ )
  for( u32 i=BUTTON_ID_POCKET; i<FIELD_BUTTON_ID_MAX; i++ )
  {
    if( ( bit & (1<<(i-BUTTON_ID_POCKET)) ) != 0 )
    {
      if( flg == false )
      {
        if( man->IsButtonActive(i) != false )
        {
          man->SetButtonPassive( i );
        }
      }
      else
      {
        if( man->IsButtonActive(i) == false )
        {
          man->SetButtonActive( i );
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsPocketButtonActive
 * @brief   ポケットのボタンがアクティブか
 * @date    2015.05.08
 *
 * @param   pocket  ポケット番号
 *
 * @retval  true  = アクティブ
 * @retval  false = パッシブ
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsPocketButtonActive( u32 pocket )
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    GFL_ASSERT( 0 );
    return false;
  }

  return app::ui::UIResponder::GetButtonManager()->IsButtonActive( BUTTON_ID_POCKET + pocket );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPocketAnime
 * @brief   ポケットアニメ取得
 * @date    2015.03.24
 *
 * @param   max   アニメ数格納場所
 *
 * @return  アニメテーブル
 */
//-----------------------------------------------------------------------------
const BagDraw::PageColorAnime * BagDraw::GetPocketAnime( u32 * max )
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    *max = BATTLE_POCKET_MAX;
    return BattlePocketAnime;
  }
  else
  {
    *max = FIELD_POCKET_MAX;
    return FieldPocketAnime;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPocketAnime
 * @brief   ポケットアニメ初期化
 * @date    2015.03.24
 */
//-----------------------------------------------------------------------------
void BagDraw::InitPocketAnime(void)
{
  static const u32 style_field[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_VISIBLE_FIELD,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_VISIBLE_FIELD,
  };
  static const u32 style_battle[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_07_VISIBLE_BATTLE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_VISIBLE_BATTLE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_VISIBLE_BATTLE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_VISIBLE_BATTLE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_VISIBLE_BATTLE,
  };

  u32 max;
  const PageColorAnime * tbl = GetPocketAnime( &max );
  const u32 * style;
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    style = style_battle;
  }
  else
  {
    style = style_field;
  }

  app::util::G2DUtil * g2d = UIView::GetG2DUtil();
  for( u32 i=0; i<max; i++ )
  {
    g2d->BindAnime( 0, tbl[i].normal );
    g2d->StartAnimeFrame( 0, tbl[i].normal, g2d->GetAnimeMaxFrame(0,tbl[i].normal) );
    g2d->StartAnime( 0, style[i] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNowPocketAnime
 * @brief   選択状態のポケットアニメへ変更
 * @date    2015.03.24
 *
 * @param   old     前回のポケット
 * @param   now     現在のポケット
 * @param   is_end  true = 最終フレーム
 */
//-----------------------------------------------------------------------------
void BagDraw::SetNowPocketAnime( u32 old, u32 now, bool is_end )
{
  if( old == now )
  {
    return;
  }

  u32 max;
  const PageColorAnime * tbl = GetPocketAnime( &max );

  app::util::G2DUtil * g2d = UIView::GetG2DUtil();

  g2d->StopAnime( 0, tbl[old].select );
  g2d->StopAnime( 0, tbl[now].normal );
  if( is_end == false )
  {
    g2d->StartAnime( 0, tbl[old].normal );
    g2d->StartAnime( 0, tbl[now].select );
  }
  else
  {
    g2d->BindAnime( 0, tbl[old].normal );
    g2d->StartAnimeFrame( 0, tbl[old].normal, g2d->GetAnimeMaxFrame(0,tbl[old].normal) );

    g2d->BindAnime( 0, tbl[now].select );
    g2d->StartAnimeFrame( 0, tbl[now].select, g2d->GetAnimeMaxFrame(0,tbl[now].select) );
  }

  if( max == FIELD_POCKET_MAX )
  {
    if( now == (FIELD_POCKET_MAX-1) )
    {
      g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_ICON_LOOP_COLOR );
    }
    else
    {
      g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_ICON_LOOP_COLOR );
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetNowPocketTable
 * @brief   現在のポケットデータを設定
 * @date    2015.03.23
 *
 * @param   pocket      ポケット番号
 * @param   table       道具データテーブル
 * @param   table_max   道具データテーブルの要素数
 */
//-----------------------------------------------------------------------------
void BagDraw::SetNowPocketTable( u32 pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max )
{
  m_pocket           = pocket;
  m_nowPocketItem    = table;
  m_nowPocketItemMax = table_max;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePocket
 * @brief   ポケット切り替え（カーソル位置補正版）
 * @date    2015.03.24
 *
 * @param   old_pocket  切り替え前のポケット
 * @param   new_pocket  切り替え後のポケット
 * @param   table       道具データテーブル
 * @param   table_max   道具データテーブルの要素数
 * @param   item        選択位置の道具
 * @param   list_pos    リスト位置
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePocket( u32 old_pocket, u32 new_pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 item, u32 list_pos )
{
  SetNowPocketAnime( old_pocket, new_pocket );
  SetNowPocketTable( new_pocket, table, table_max );
  ResetItemList( item, list_pos );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePocket
 * @brief   ポケット切り替え（カーソル位置指定版）
 * @date    2015.05.12
 *
 * @param   old_pocket  切り替え前のポケット
 * @param   new_pocket  切り替え後のポケット
 * @param   table       道具データテーブル
 * @param   table_max   道具データテーブルの要素数
 * @param   list_pos    リスト位置
 * @param   list_scroll リストスクロール値
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePocket( u32 old_pocket, u32 new_pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 list_pos, f32 list_scroll )
{
  SetNowPocketAnime( old_pocket, new_pocket );
  SetNowPocketTable( new_pocket, table, table_max );

  // リスト初期化
  app::tool::PaneList::SETUP_PARAM list_setup;
//  AdjustPaneListPos( &cursor_pos, &list_scroll, m_nowPocketItem, m_nowPocketItemMax, ITEM_DUMMY_DATA );
  SetPaneListParam( &list_setup, list_pos, list_scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  SetPaneListConfig( &list_setup );
  ChangePaneListItemColor();
  PutListNoItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    IsInitPocketAnimeEnd
 * @brief   ポケット切り替えアニメが終了しているか
 * @date    2015.05.12
 *
 * @retval  true  = 終了
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsInitPocketAnimeEnd(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  u32 max;
  const PageColorAnime * tbl = GetPocketAnime( &max );

  for( u32 i=0; i<max; i++ )
  {
    if( g2d->IsAnimeEnd(0,tbl[i].normal) == false || g2d->IsAnimeEnd(0,tbl[i].select) == false )
    {
      return false;
    }
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListParam
 * @brief   ペインリスト初期化パラメータ設定
 * @date    2015.03.23
 *
 * @param   param   パラメータ生成場所
 * @param   pos     カーソル位置
 * @param   scroll  スクロール値
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPaneListParam( app::tool::PaneList::SETUP_PARAM * param, u32 pos, u32 scroll )
{
  gfl2::lyt::LytWk * lytwk = GetG2DUtil()->GetLayoutWork( 0 );

  param->heap = m_heap->GetSystemHeap();  // ヒープ

  param->pLytWk = lytwk;    // レイアウトワーク
  param->paneList = m_pPaneListData;   // ペインデータ
  param->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  param->valMax = m_nowPocketItemMax;					// 実際に使用するリストの項目数（ペインの数ではない）
  param->listPosMax = LIST_CURSOR_MOVE_MAX;   // カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  param->listPos = pos;				// 初期カーソル位置 ( 0 〜 listPosMax-1 の範囲 )
  param->listScroll = scroll;			// 初期スクロール位置 ( 0 〜 valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  param->defPY = LIST_ITEM_PY;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる
  param->defSX = LIST_ITEM_SX;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  param->defSY = LIST_ITEM_SY;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  param->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  param->listStartPane = lytwk->GetPane( PANENAME_BAG_MIN_LOW_000_PANE_ITEM_CHANGE_OFF );   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  param->cbFunc = this;	// コールバック関数
  param->cbWork = NULL;			// コールバック関数に渡すワーク

  param->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  param->menuCursor = m_pMenuCursor;		// カーソル
}

//-----------------------------------------------------------------------------
/**
 * @func    InitPaneList
 * @brief   ペインリスト初期化
 * @date    2015.04.10
 *
 * @param   view        リストビュークラス
 * @param   pocket      ポケット番号
 * @param   table       道具データテーブル
 * @param   table_max   道具データテーブルの要素数
 */
//-----------------------------------------------------------------------------
void BagDraw::InitPaneList( app::tool::PaneListView * view, u32 pocket, Savedata::MyItem::ITEM_ST ** table, u32 table_max )
{
  m_pPaneListView = view;
  SetNowPocketTable( pocket, table, table_max );

  static const u32 anm[LIST_ITEM_PANE_MAX][4] =
  {
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_TOUCH_RELEASE,
    },
    {
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_TOUCH,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_RELEASE,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_CANSEL,
      LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_TOUCH_RELEASE,
    },
  };

  m_pPaneListData = GFL_NEW_LOW_ARRAY(m_heap->GetSystemHeap()) app::tool::PaneList::PANE_DATA[LIST_ITEM_PANE_MAX];

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<LIST_ITEM_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListBasePaneTable[i] );
    m_pPaneListData[i].base       = parts;
    m_pPaneListData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_TEXTBOX_00, &res_id );
    m_pPaneListData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_BOUND_00, &res_id );
    m_pPaneListData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_NULL_CUR, &res_id );

    m_pPaneListData[i].holdAnime    = anm[i][0];
    m_pPaneListData[i].releaseAnime = anm[i][1];
    m_pPaneListData[i].cancelAnime  = anm[i][2];
    m_pPaneListData[i].selectAnime  = anm[i][3];
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  {
    u32 cursor_pos, list_scroll;
    GetListCursorPos( &cursor_pos, &list_scroll, pocket );
    AdjustPaneListPos( &cursor_pos, &list_scroll, table, table_max, m_appParam->select_item );
    SetPaneListParam( &list_setup, cursor_pos, list_scroll );
  }

  u32 scroll_bar_max;
  if( list_setup.valMax <= list_setup.listPosMax )
  {
    scroll_bar_max = 0;
  }
  else
  {
    scroll_bar_max = list_setup.valMax - list_setup.listPosMax;
  }

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    m_heap->GetSystemHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    scroll_bar_max,

    lytwk,
    res_id,
    PANENAME_BAG_MIN_LOW_000_PANE_PARTS_SCR,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL
  };
  
//  m_pPaneListView = GFL_NEW(m_heap->pHeap) app::tool::PaneListView( m_heap );
  m_pPaneListView->CreatePaneList( &list_setup );
  m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
  m_pPaneListView->SetListener( this );
  SetPaneListConfig( &list_setup );
  g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_SCR_COLOR_BAG );

  ChangePaneListItemColor();

  AddSubView( m_pPaneListView );

  PutListNoItem();

  SetPaneListInputEnable( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndPaneList
 * @brief   ペインリスト終了
 * @date    2015.04.10
 */
//-----------------------------------------------------------------------------
void BagDraw::EndPaneList(void)
{
  if( m_pPaneListView != NULL )
  {
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView = NULL;
  }
  GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListConfig
 * @brief   ペインリストの設定変更
 * @date    2015.03.30
 *
 * @param   setup   ペインリストのセットアップパラメータ
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPaneListConfig( app::tool::PaneList::SETUP_PARAM * setup )
{
  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  bool is_catch = true;
  if( m_appParam->call_mode == CALL_SHOP )
  {
    is_catch = false;
  }
  // サブ設定
  list->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, is_catch, true, false );
  // SE
  list->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );
  // 半透明エフェクト
//  list->SetAlphaEffect();
  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = setup->listPosMax;
    if( setup->valMax < pos_max )
    {
      pos_max = setup->valMax;
    }
    f32 lx = LIST_ITEM_PX - LIST_ITEM_SX / 2.0f;
    f32 rx = lx + LIST_ITEM_SX;
    f32 uy = LIST_ITEM_PY + pos.y + LIST_ITEM_SY / 2.0f;
    f32 dy = uy - LIST_ITEM_SY * pos_max;
    list->MakeTouchArea( lx, rx, uy, dy );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListEnable
 * @brief   ペインリストの入力許可設定（カーソル表示切り替え込み）
 * @date    2015.04.15
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPaneListEnable( bool flg )
{
  SetPaneListInputEnable( flg );
  // 道具がない場合はカーソルを強制的に非表示にする
  if( flg != false && m_nowPocketItemMax == 0 )
  {
    flg = false;
  }
  m_pMenuCursor->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPaneListInputEnable
 * @brief   ペインリストの入力許可設定
 * @date    2015.04.15
 *
 * @param   flg   true = 有効, false = 無効
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPaneListInputEnable( bool flg )
{
  if( *m_isDemoMode != false )
  {
    flg = false;
  }
  m_pPaneListView->SetInputEnabled( flg );
  m_pPaneListView->GetPaneList()->SetInputEnabled( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetListItemBaseY
 * @brief   ペインリストの基準Y座標を設定
 * @date    2015.05.08
 */
//-----------------------------------------------------------------------------
void BagDraw::SetListItemBaseY(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytPane * pane = lytwk->GetPane(
                                lytwk->GetPartsPane(PANENAME_BAG_MIN_LOW_000_PANE_PARTSITEM_00),
                                PANENAME_BAG_PLT_LOW_001_PANE_MAIN,
                                &res_id );

  gfl2::math::VEC3 pos = g2d->GetPaneLocalPos( pane );
  m_listItemBaseY = pos.y;
}

//-----------------------------------------------------------------------------
/**
 * @func    AdjustPaneListPos
 * @brief   リストのカーソル位置を補正
 * @date    2015.04.13
 *
 * @param   pos         カーソル位置
 * @param   scroll      スクロール値
 * @param   table       道具データテーブル
 * @param   table_max   道具データテーブルの要素数
 * @param   item        カーソル位置の道具
 */
//-----------------------------------------------------------------------------
void BagDraw::AdjustPaneListPos( u32 * pos, u32 * scroll, Savedata::MyItem::ITEM_ST ** table, u32 table_max, u32 item )
{
  // スクロール値補正
  u32 scroll_max = 0;
  if( table_max >= LIST_CURSOR_MOVE_MAX )
  {
    scroll_max = table_max - LIST_CURSOR_MOVE_MAX;
  }
  if( *scroll > scroll_max )
  {
    *scroll = scroll_max;
  }
  // カーソル位置補正
  u32 pos_max = 0;
  if( scroll_max != 0 )
  {
    pos_max = LIST_CURSOR_MOVE_MAX - 1;
  }
  else if( table_max != 0 )
  {
    pos_max = table_max - 1;
  }
  if( *pos > pos_max )
  {
    *pos = pos_max;
  }
  // カーソル位置のアイテムが違うアイテム
  // @fix MMCat[40]: NULLチェックを追加
  if( table[*scroll+*pos] != NULL && item != ITEM_DUMMY_DATA && item != table[*scroll+*pos]->id )
  {
    for( u32 i=0; i<table_max; i++ )
    {
      if( table[i]->id == item )
      {
        if( i >= (LIST_CURSOR_MOVE_MAX-1) )
        {
          *scroll = i - (LIST_CURSOR_MOVE_MAX-1);
          *pos    = i - *scroll;
        }
        else
        {
          *scroll = 0;
          *pos    = i;
        }
        return;
      }
    }
    *pos    = 0;
    *scroll = 0;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListCursorPos
 * @brief   道具リストのカーソル位置を取得
 * @date    2015.04.13
 *
 * @param   pos     カーソル位置格納場所
 * @param   scroll  スクロール値格納場所
 * @param   pocket  ポケット番号
 */
//-----------------------------------------------------------------------------
void BagDraw::GetListCursorPos( u32 * pos, u32 * scroll, u32 pocket )
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    *pos    = m_appParam->pocket[pocket].cursor_pos;
    *scroll = m_appParam->pocket[pocket].list_scroll;
  }
  else
  {
    const app::bag::PlayData::POCKET_DATA * pocket_data = m_playData->GetPocketData( pocket );

    *pos    = pocket_data->pos;
    *scroll = pocket_data->scroll;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeListPos
 * @brief   道具リストのカーソル位置を変更
 * @date    2015.04.24
 *
 * @param   sel_item  移動先の道具ID
 * @param   pos_item  現在の位置の道具ID
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangeListPos( u32 sel_item, u32 pos_item )
{
  if( sel_item == pos_item )
  {
    return;
  }

  u32 cursor_pos, list_scroll;
  GetListCursorPos( &cursor_pos, &list_scroll, m_pocket );
  AdjustPaneListPos( &cursor_pos, &list_scroll, m_nowPocketItem, m_nowPocketItemMax, sel_item );

  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  list->SetCursorData( cursor_pos, static_cast<f32>(list_scroll) );
  list->InitListPut();
}

//-----------------------------------------------------------------------------
/**
 * @func    PaneListItemSelectListener
 * @brief   ペインリスト決定時に呼ばれるリスナー
 * @date    2015.04.02
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void BagDraw::PaneListItemSelectListener( u32 pos )
{
  m_uiListener->SelectItem( m_nowPocketItem[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    PaneListItemCatchListener
 * @brief   ペインリスト項目キャッチ時に呼ばれるリスナー
 * @date    2015.05.12
 *
 * @param   pos   カーソル位置
 */
//-----------------------------------------------------------------------------
void BagDraw::PaneListItemCatchListener( u32 pos )
{
  m_uiListener->CatchItem( m_nowPocketItem[pos] );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePaneListItemColor
 * @brief   ペインリストの項目カラーを非選択状態に変更
 * @date    2015.03.27
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePaneListItemColor(void)
{
  for( u32 i=0; i<LIST_ITEM_PANE_MAX; i++ )
  {
    ChangePaneListItemColor( i, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePaneListItemColor
 * @brief   ペインリストの項目カラー変更
 * @date    2015.03.27
 *
 * @param   pos   項目位置
 * @param   flg   true = 選択状態, false = 非選択状態
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePaneListItemColor( u32 pos, bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  const PageColorAnime * tbl = NULL;

  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    // @fix MMCat[140]: 色変更アニメを全停止する
    for( u32 i=0; i<BagDraw::FIELD_POCKET_MAX; i++ )
    {
      g2d->StopAnime( 0, FieldPaneListItemColorTable[pos][i].normal );
      g2d->StopAnime( 0, FieldPaneListItemColorTable[pos][i].select );
    }
    tbl = &FieldPaneListItemColorTable[pos][m_pocket];
  }
  else
  {
    // @fix MMCat[140]: 色変更アニメを全停止する
    for( u32 i=0; i<BagDraw::BATTLE_POCKET_MAX; i++ )
    {
      g2d->StopAnime( 0, BattlePaneListItemColorTable[pos][i].normal );
      g2d->StopAnime( 0, BattlePaneListItemColorTable[pos][i].select );
    }
    tbl = &BattlePaneListItemColorTable[pos][m_pocket];
  }
  if( flg == false )
  {
    g2d->StartAnime( 0, tbl->normal );
  }
  else
  {
    g2d->StartAnime( 0, tbl->select );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePaneListItemColor
 * @brief   カーソル位置のペインリストの項目カラー変更
 * @date    2015.04.30
 *
 * @param   flg   true = 選択状態, false = 非選択状態
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePaneListItemColor( bool flg )
{
  u32 pos;
  f32 scroll;
  app::tool::PaneList * list = m_pPaneListView->GetPaneList();
  list->GetCursorData( &pos, &scroll );
  ChangePaneListItemColor( list->GetPosPaneIndex(pos), flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorMove
 * @brief   カーソル移動時のコールバック関数
 * @date    2015.03.16
 *
 * @param   work        外部から指定されたワーク
 * @param   cur_pane    ペインデータ
 * @param   pane_index  カーソル位置のペインデータインデックス
 * @param   pos         カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BagDraw::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  if( m_nowPocketItemMax != 0 )
  {
    m_uiListener->UpdateCursorItem( m_nowPocketItem[pos] );

    if( m_isPaneListCatchMode == false )
    {
      if( m_isPaneListChangeMode == false )
      {
        ChangePaneListItemColor( pane_index, false );
      }
      else
      {
        ChangePaneListItemColor( pane_index, true );
      }
    }
  }
  else
  {
    m_uiListener->UpdateCursorItem( NULL );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_CursorOff
 * @brief		カーソルを非表示にするコールバック関数
 * @date    2015.03.16
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	カーソル位置のペインデータインデックス
 * @param		pos					カーソル位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BagDraw::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ListWrite
 * @brief		リストの項目を描画するコールバック関数
 * @date    2015.03.16
 *
 * @param		work				外部から指定されたワーク
 * @param		pane				ペインデータ
 * @param		pane_index	項目位置のペインデータインデックス
 * @param		pos					項目位置
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BagDraw::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
/*
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<LIST_ITEM_PANE_MAX; i++ )
  {
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( ListBasePaneTable[i] );
    m_pPaneListData[i].base       = parts;
    m_pPaneListData[i].text       = lytwk->GetTextBoxPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_TEXTBOX_00, &res_id );
    m_pPaneListData[i].bound      = lytwk->GetBoundingPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_BOUND_00, &res_id );
    m_pPaneListData[i].cursor_pos = lytwk->GetPane( parts, PANENAME_BAG_PLT_LOW_001_PANE_NULL_CUR, &res_id );

    m_pPaneListData[i].holdAnime    = anm[i][0];
    m_pPaneListData[i].releaseAnime = anm[i][1];
    m_pPaneListData[i].cancelAnime  = anm[i][2];
    m_pPaneListData[i].selectAnime  = anm[i][3];
  }
*/

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  // 書き込み用
  gfl2::lyt::LytTextBox * text_w = pane[pane_index].text;
  // 非表示用
  gfl2::lyt::LytTextBox * text_v = lytwk->GetTextBoxPane( lytwk->GetPartsPane(ListBasePaneTable[pane_index]), PANENAME_BAG_PLT_LOW_001_PANE_TEXTBOX_01, &res_id );
  u32 text_index;

  WazaNo waza = static_cast<WazaNo>( item::ITEM_GetWazaNo( m_nowPocketItem[pos]->id ) );
  if( waza == WAZANO_NULL )
  {
    g2d->SetRegisterItemName( 0, m_nowPocketItem[pos]->id );
    text_index = msg_bag_01_01;
  }
  else
  {
    g2d->SetRegisterWazaName( 0, waza );
    text_index = msg_bag_01_02;
    if( m_appParam->call_mode == CALL_FIELD_POKELIST )
    {
      const pml::pokepara::PokemonParam * pp = m_appParam->pokemon->GetPoke( m_appParam->pokemon_index );
      if( pp->HaveWaza( waza ) != false ||
          pp->CheckWazaMachine( item::ITEM_GetWazaMashineNo(m_nowPocketItem[pos]->id) ) == false )
      {
        gfl2::lyt::LytTextBox * tmp = text_w;
        text_w = text_v;
        text_v = tmp;
      }
    }
  }
  g2d->SetTextBoxPaneStringExpand( text_w, text_index );
  text_w->SetVisible( true );
  text_v->SetVisible( false );

  {
    bool is_select = false;
    if( ( m_isPaneListChangeMode != false || m_isPaneListCatchMode != false ) && m_changeItem == m_nowPocketItem[pos]->id )
    {
      is_select = true;
    }
    ChangePaneListItemColor( pane_index, is_select );
  }

  if( m_appParam->call_mode != CALL_BATTLE_MENU )
  {
    PutItemNewIcon( pane_index, m_myItem->IsNew(m_nowPocketItem[pos]) );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CallBack_ItemChange
 * @brief		入れ替え用コールバック関数
 * @date    2015.03.16
 *
 * @param		work	外部から指定されたワーク
 * @param		pos1	入れ替え位置１
 * @param		pos2	入れ替え位置２
 *
 * @return	none
 *
 * @note  override PaneList::CallBack
 */
//-----------------------------------------------------------------------------
void BagDraw::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
  if( m_pocket == PlayData::FREE_SPACE_POCKET )
  {
    ItemChangeFreeSpace( pos1, pos2 );
  }
  else
  {
    ItemChange( pos1, pos2 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetItemList
 * @brief   道具リスト再設定
 * @date    2015.03.23
 *
 * @param   item    カーソル位置に指定する道具ID
 * @param   set_pos カーソル位置
 */
//-----------------------------------------------------------------------------
void BagDraw::ResetItemList( u32 item, u32 set_pos )
{
  // リスト初期化
  app::tool::PaneList::SETUP_PARAM list_setup;
  u32 cursor_pos, list_scroll;
  GetListCursorPos( &cursor_pos, &list_scroll, m_pocket );
  if( set_pos != RESET_LIST_POS_NORMAL )
  {
    cursor_pos = set_pos;
  }
  AdjustPaneListPos( &cursor_pos, &list_scroll, m_nowPocketItem, m_nowPocketItemMax, item );
  SetPaneListParam( &list_setup, cursor_pos, list_scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  SetPaneListConfig( &list_setup );
  ChangePaneListItemColor();
  PutListNoItem();
}

//-----------------------------------------------------------------------------
/**
 * @func    PutListNoItem
 * @brief   道具がない場合の表示
 * @date    2015.03.23
 */
//-----------------------------------------------------------------------------
void BagDraw::PutListNoItem(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( m_nowPocketItemMax == 0 )
  {
    g2d->SetPaneVisible( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTBOX_00, true );
  }
  else
  {
    g2d->SetPaneVisible( 0, PANENAME_BAG_MIN_LOW_000_PANE_TEXTBOX_00, false );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectPlate
 * @brief   ポケモンのプレート選択時に呼ばれるリスナー
 * @date    2015.04.13
 *
 * @param   id  動作ID
 *
 * @note  override BagPokemonListener
 */
//-----------------------------------------------------------------------------
void BagDraw::SelectPlate( u32 id )
{
  static const u32 tbl[] =
  {
    ACTION_TOUCH_ON_PLATE,
    ACTION_TOUCH_OFF_PLATE,
    ACTION_SELECT_PLATE,
    ACTION_GET_POKE_ITEM,
    ACTION_PUT_POKE_ITEM,
    ACTION_PUT_LIST_ITEM,
  };
  m_uiListener->OnAction( tbl[id] );
}

//-----------------------------------------------------------------------------
/**
 * @func    PokeItemCatch
 * @brief   ポケモンの持ち物取得動作時に呼ばれるリスナー
 * @date    2015.05.01
 *
 * @param   x         タッチX座標
 * @param   y         タッチY座標
 * @param   poke_pos  タッチ中のポケモン位置
 *
 * @note  override BagPokemonListener
 */
//-----------------------------------------------------------------------------
void BagDraw::PokeItemCatch( u16 x, u16 y, u32 poke_pos )
{
  m_lastTouchX = x;
  m_lastTouchY = y;

  // ポケモン
  if( poke_pos != pml::PokeParty::MAX_MEMBERS )
  {
    m_itemPutPosID = PUT_ID_PLATE;
    m_itemPutIndex = poke_pos;
    return;
  }

  { // ポケット
    u32 pocket = CheckPocketIconHit( x, y );
    if( pocket != FIELD_POCKET_MAX )
    {
      m_itemPutPosID = PUT_ID_POCKET;
      m_itemPutIndex = pocket;
//      m_uiListener->OnAction( ACTION_ITEM_CATCH_POCKET );
      return;
    }
  }

  { // リストスクロール
    int mv = CheckItemListScrollPane( x, y );
    if( mv != 0 )
    {
      m_itemPutPosID = PUT_ID_SCROLL;
      m_itemPutIndex = mv;
      return;
    }
  }

  { // リスト
    int pos = 0;
    if( GetTouchItemPos( x, y, &pos ) != false )
    {
      m_itemPutPosID = PUT_ID_LIST;
      m_itemPutIndex = pos;
      return;
    }
  }

  m_itemPutPosID = PUT_ID_CANCEL;
}

//-----------------------------------------------------------------------------
/**
 * @func    PutMessage
 * @brief   メッセージ表示（一括）
 * @date    2015.04.10
 *
 * @param   id    メッセージID
 * @param   item  道具データ
 * @param   pp    ポケモンデータ
 */
//-----------------------------------------------------------------------------
void BagDraw::PutMessage( MsgID id, const Savedata::MyItem::ITEM_ST * item, const pml::pokepara::PokemonParam * pp )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_ITEM_MENU:
    g2d->SetRegisterItemName( 0, item->id );
    g2d->GetStringExpand( str, msg_bag_04_01 );
    SetMessageWindow( str );
    break;
  case MSG_ID_ITEM_CATCH_MENU:
    g2d->SetRegisterItemName( 0, item->id );
    g2d->SetRegisterPokeNickName( 1, pp );
    g2d->GetStringExpand( str, msg_bag_04_02 );
    SetMessageWindow( str );
    break;
  case MSG_ID_SORT:
    g2d->GetString( str, msg_bag_04_03 );
    SetMessageWindow( str );
    break;
  case MSG_ID_USE_SELECT:
    g2d->GetString( str, msg_bag_08_01 );
    SetMessageWindowSmall( str );
    break;
  case MSG_ID_SET_SELECT:
    g2d->GetString( str, msg_bag_08_02 );
    SetMessageWindowSmall( str );
    break;
  case MSG_ID_USE_WAZAMACHINE_SELECT:
    g2d->GetString( str, msg_bag_08_03 );
    SetMessageWindowSmall( str );
    break;

  case MSG_ID_UNION_SELECT:
    g2d->GetString( str, msg_bag_08_04 );
    SetMessageWindowSmall( str );
    break;

  case MSG_ID_ZCUBE_MENU:
    g2d->SetRegisterItemName( 0, item->id );
    g2d->GetStringExpand( str, msg_bag_06_124 );
    SetMessageWindow( str );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMessageWindow
 * @brief   メッセージウィンドウに文字列を書き込む
 * @date    2015.04.03
 *
 * @param   str   文字列
 */
//-----------------------------------------------------------------------------
void BagDraw::SetMessageWindow( gfl2::str::StrBuf * str )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );
  g2d->SetPaneVisible( parts, true );

  gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2, &res_id );
  g2d->SetTextBoxPaneString( text, str );
  g2d->SetPaneVisible( text, true );

  g2d->SetPaneVisible( lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1,&res_id), false );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMessageWindowSmall
 * @brief   小メッセージウィンドウに文字列を書き込む
 * @date    2015.04.13
 *
 * @param   str   文字列
 */
//-----------------------------------------------------------------------------
void BagDraw::SetMessageWindowSmall( gfl2::str::StrBuf * str )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytTextBox * text = lytwk->GetTextBoxPane( PANENAME_BAG_MIN_LOW_000_PANE_TEXTBOX_01 );
  g2d->SetTextBoxPaneString( text, str );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutMessageWindowSmall
 * @brief   小メッセージウィンドウ表示切り替え
 * @date    2015.04.21
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagDraw::PutMessageWindowSmall( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  // 表示
  if( flg != false )
  {
    g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_OUT );
    if( m_isSelectWinPut == false )
    {
      g2d->BindAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_IN );
      g2d->StartAnimeFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_IN, g2d->GetAnimeMaxFrame(0,LA_COMMONBAG_BAG_MIN_LOW_000_MSG_IN) );
      m_isSelectWinPut = true;
    }
    m_inputMode = MODE_POKE_SELECT;
  }
  // 非表示
  else
  {
    g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_IN );
    if( m_isSelectWinPut != false )
    {
      g2d->BindAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_OUT );
      g2d->StartAnimeFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000_MSG_OUT, g2d->GetAnimeMaxFrame(0,LA_COMMONBAG_BAG_MIN_LOW_000_MSG_OUT) );
      m_isSelectWinPut = false;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateMessageWindow
 * @brief   メッセージウィンドウ生成
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagDraw::CreateMessageWindow(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );

  gfl2::lyt::LytTextBox * text1 = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1A, &res_id );
  gfl2::lyt::LytTextBox * text2 = lytwk->GetTextBoxPane( parts, PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_1B, &res_id );

  g2d->CreateMessageWindow(
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager(),
    0,
    text1,
    text2,
    m_heap );
  g2d->SetMessageCallback( this );

  g2d->CreateMsgCursor( m_heap, lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_CURSOR_00,&res_id) );
  g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT__PARTSMSG_01__CURSOR_00_KEEP );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteMessageWindow
 * @brief   メッセージウィンドウ削除
 * @date    2015.04.15
 */
//-----------------------------------------------------------------------------
void BagDraw::DeleteMessageWindow(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->DeleteMsgCursor();
  g2d->DeleteMessageWindow();
}

//-----------------------------------------------------------------------------
/**
 * @func    SetPrintMessage
 * @brief   メッセージ表示開始
 * @date    2015.04.15
 *
 * @param   str   文字列
 * @param   type  終了タイプ
 */
//-----------------------------------------------------------------------------
void BagDraw::SetPrintMessage( const gfl2::str::StrBuf * str, gfl2::str::StrWin::FinishType type )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  g2d->SetMessage( str );
  g2d->SetMessageFinishMode( type, 0 );
  m_msgFinishType = type;
  m_isMsgEnd = false;

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );
  g2d->SetPaneVisible( parts, true );

  g2d->SetPaneVisible( lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_NULL_TEXT1,&res_id), true );
  g2d->SetPaneVisible( lytwk->GetPane(parts,PANENAME_COMMON_MSG_LOW_000_PANE_TEXTBOX_2,&res_id), false );

  // 小ウィンドウを消す
  PutMessageWindowSmall( false );
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdatePrintMessage
 * @brief   メッセージ表示更新
 * @date    2015.04.15
 *
 * @retval  true  = 処理中
 * @retval  false = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::UpdatePrintMessage(void)
{
  if( m_isMsgEnd != false )
  {
    return false;
  }

  gfl2::str::StrWin::Result result = GetG2DUtil()->PrintMessage();
  if( m_msgFinishType == gfl2::str::StrWin::FINISH_NONE && result == gfl2::str::StrWin::RES_DONE )
  {
    m_isMsgEnd = true;
    return false;
  }
  if( m_msgFinishType == gfl2::str::StrWin::FINISH_USER && result == gfl2::str::StrWin::RES_FINISH )
  {
    m_isMsgEnd = true;
    Sound::PlaySE( SEQ_SE_MESSAGE );
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    UpdateMsgCallback
 * @brief   メッセージ表示コールバック関数
 * @date    2015.04.20
 *
 * @param   arg   コールバックID
 *
 * @retval  true  = 継続
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool BagDraw::UpdateMsgCallback( u16 arg )
{
  switch( arg )
  {
  case 0:   // SEウェイト
    if( Sound::IsSEFinished( m_msgCallBackWrok ) != 0 )
    {
      return false;
    }
    return true;

  case 1:   // MEウェイト
    if( Sound::IsMEFinished( m_msgCallBackWrok ) != 0 )
    {
      Sound::StartBaseBGMFromME();
      return false;
    }
    return true;

  case 2:   // 技マシン起動
    SetMsgCallBackSE( SEQ_SE_SYS_004 );
    return false;

  case 3:   // 技マシン忘れ（ポカン）
    SetMsgCallBackSE( SEQ_SE_WAZAWASURE );
    return false;

  case 4:   // 技を覚えた
    SetMsgCallBackME( STRM_ME_LVUP );
    return false;

  case 5:   // レベルアップ
    SetMsgCallBackME( STRM_ME_LVUP);
    return false;
  }

  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMsgCallBackSE
 * @brief   メッセージ表示コールバック中のSE再生
 * @date    2015.04.20
 *
 * @param   se  SE番号
 */
//-----------------------------------------------------------------------------
void BagDraw::SetMsgCallBackSE( u32 se )
{
  m_msgCallBackWrok = se;
  Sound::PlaySE( se );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetMsgCallBackME
 * @brief   メッセージ表示コールバック中のME再生
 * @date    2015.04.20
 *
 * @param   me  ME番号
 */
//-----------------------------------------------------------------------------
void BagDraw::SetMsgCallBackME( u32 me )
{
  m_msgCallBackWrok = me;
  Sound::StartME( me );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetDispActive
 * @brief   画面のアクティブ/パッシブ切り替え
 * @date    2015.04.10
 *
 * @param   flg   true = アクティブ, false = パッシブ
 *
 * @note  メッセージウィンドウの表示切り替え
 */
//-----------------------------------------------------------------------------
void BagDraw::SetDispActive( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  // メッセージ表示
  if( flg == false )
  {
    g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_OUT );
    if( m_isMsgWinPut == false )
    {
      {
        gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
        gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
        gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT );
        gfl2::lyt::LytPane * pane = lytwk->GetPane( parts, PANENAME_COMMON_WIN_LOW_002_PANE_ALL, &res_id );
        parts->SetVisible( true );  // ないと出ない
        pane->SetAlpha( 0 );        // パシるので透明にしておく
      }
      g2d->BindAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN );
      g2d->StartAnimeFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN, g2d->GetAnimeMaxFrame(0,LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN) );
      m_isMsgWinPut = true;
    }
  }
  // メッセージ非表示
  else
  {
    g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_IN );
//    GetG2DUtil()->SetPaneVisible( 0, PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT, false );
    if( m_isMsgWinPut != false )
    {
      g2d->BindAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_OUT );
      g2d->StartAnimeFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_OUT, g2d->GetAnimeMaxFrame(0,LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_COMMANDSELECT_OUT) );
      m_isMsgWinPut = false;
    }
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintMessage
 * @brief   メッセージ表示開始
 * @date    2015.04.15
 *
 * @param   id      メッセージID
 * @param   item1   道具１
 * @param   item2   道具２
 * @param   pp      ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintMessage( MsgID id, u32 item1, u32 item2, const pml::pokepara::PokemonParam * pp )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_NO_POKEMON:
    g2d->GetString( str, msg_bag_06_53 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SET_ITEM:
    g2d->SetRegisterItemName( 0, item1 );
    g2d->GetStringExpand( str, msg_bag_06_40 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SET_ERROR_ITEM:
    g2d->SetRegisterItemName( 0, item1 );
    g2d->GetStringExpand( str, msg_bag_06_43 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SET_ERROR_EGG:
    g2d->GetString( str, msg_bag_06_54 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SET_ERROR_MAX:
    g2d->GetString( str, msg_bag_06_52 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_CHANGE_CHECK:
    g2d->SetRegisterItemName( 0, item1 );
    g2d->SetRegisterPokeNickName( 1, pp );
    g2d->GetStringExpand( str, msg_bag_06_41 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
    break;

  case MSG_ID_CHANGE_ITEM:
    g2d->SetRegisterItemName( 0, item1 );
    g2d->SetRegisterItemName( 1, item2 );
    g2d->GetStringExpand( str, msg_bag_06_42 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_COLLECT_ITEM:
    g2d->GetString( str, msg_bag_06_00 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_ERROR:
    g2d->SetRegisterPlayerName( 0, m_myStatus );
    g2d->GetStringExpand( str, msg_bag_06_51 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_ITEM:
    g2d->SetRegisterPlayerName( 0, m_myStatus );
    g2d->SetRegisterItemName( 1, item1 );
    g2d->GetStringExpand( str, msg_bag_06_20 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_GAKUSYUUSOUTI_ON:
    g2d->GetString( str, msg_bag_06_30 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_GAKUSYUUSOUTI_OFF:
    g2d->GetString( str, msg_bag_06_31 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_ERR_SPRAY:
    g2d->GetString( str, msg_bag_06_21 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_ERROR_RCV:
    g2d->GetString( str, msg_bag_06_50 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_USE_ERROR_ROTOM_POWER:
    g2d->GetString( str, msg_bag_40_02 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintTokuseikapuseruMessage
 * @brief   特性カプセル用メッセージ表示開始
 * @date    2015.04.20
 *
 * @param   id        メッセージID
 * @param   pp        ポケモンパラメータ
 * @param   tokusei   特性番号
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintTokuseikapuseruMessage( MsgID id, const pml::pokepara::PokemonParam * pp, TokuseiNo tokusei )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_TOKUSEI_CHANGE_CHECK:
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterTokuseiName( 1, tokusei );
    g2d->GetStringExpand( str, msg_bag_04_13 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
    break;

  case MSG_ID_TOKUSEI_CHANGE:
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterTokuseiName( 1, tokusei );
    g2d->GetStringExpand( str, msg_bag_06_110 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintFormChangeMessage
 * @brief   フォルムチェンジ用メッセージ表示開始
 * @date    2015.04.20
 *
 * @param   id  メッセージID
 * @param   pp  ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintFormChangeMessage( MsgID id, const pml::pokepara::PokemonParam * pp )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_FORM_CHANGE:
    if( pp->GetMonsNo() == MONSNO_ZIGARUDE )
    {
      FormNo form = pp->GetFormNo();
      if( form == FORMNO_ZIGARUDE_SW50PER )
      {
        g2d->GetString( str, msg_bag_06_122 );
      }
      else if( form == FORMNO_ZIGARUDE_SW10PER )
      {
        g2d->GetString( str, msg_bag_06_121 );
      }
      else
      {
        GFL_ASSERT( 0 );
        g2d->GetString( str, msg_bag_06_121 );
      }
    }
    else
    {
      g2d->SetRegisterPokeNickName( 0, pp );
      g2d->GetStringExpand( str, msg_bag_06_91 );
    }
    break;

  case MSG_ID_UNION_ERR:
    g2d->GetString( str, msg_bag_06_94 );
    break;

  case MSG_ID_UNION_ERR_EGG:
    g2d->GetString( str, msg_bag_06_95 );
    break;

  case MSG_ID_UNION_ERR_OFF:
    g2d->GetString( str, msg_bag_06_96 );
    break;

  case MSG_ID_UNION_ERR_DEATH1:
    g2d->GetString( str, msg_bag_06_92 );
    break;

  case MSG_ID_UNION_ERR_DEATH2:
    g2d->GetString( str, msg_bag_06_93 );
    break;

  default:
    GFL_ASSERT( 0 );
    return;
  }
  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintUnionWazaSubMessage
 * @brief   分離技忘れメッセージ表示開始
 * @date    2017.01.27
 *
 * @param   pp          ポケモンパラメータ
 * @param   waza        技
 * @param   is_nenriki  true = ねんりきをセット @fix MMCat[441]
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintUnionWazaSubMessage( const pml::pokepara::PokemonParam * pp, WazaNo waza, bool is_nenriki )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  g2d->SetRegisterPokeNickName( 0, pp );
  g2d->SetRegisterWazaName( 1, waza );
  if( is_nenriki == false )
  {
    g2d->GetStringExpand( str, msg_bag_40_01 );
  }
  else
  {
    g2d->SetRegisterWazaName( 2, WAZANO_NENRIKI );
    g2d->GetStringExpand( str, msg_bag_40_03 );
  }
  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintWazaMachineMessage
 * @brief   技マシン用メッセージ表示開始
 * @date    2015.04.20
 *
 * @param   id      メッセージID
 * @param   pp      ポケモンパラメータ
 * @param   waza1   技１
 * @param   waza2   技２
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintWazaMachineMessage( MsgID id, const pml::pokepara::PokemonParam * pp, WazaNo waza1, WazaNo waza2 )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_WM_START:        // わざマシンを　きどうした
    g2d->GetString( str, msg_bag_07_01 );
    break;

  case MSG_ID_WM_ERR_BAD:      // あいしょうが　わるかった
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterWazaName( 1, waza1 );
    g2d->GetStringExpand( str, msg_bag_07_02 );
    break;

  case MSG_ID_WM_ERR_SAME:     // すでに  おぼえています
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterWazaName( 1, waza1 );
    g2d->GetStringExpand( str, msg_bag_07_03 );
    break;

  case MSG_ID_WM_SELECT:       // わすれる　わざを　えらんでください
    g2d->SetRegisterWazaName( 0, waza1 );
    g2d->GetStringExpand( str, msg_bag_07_04 );
    break;

  case MSG_ID_WM_COMP:         // おぼえた
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterWazaName( 1, waza1 );
    g2d->GetStringExpand( str, msg_bag_07_05 );
    break;

  case MSG_ID_WM_COMP_ERASE:   // わずれて　おぼえた
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterWazaName( 1, waza1 );
    g2d->SetRegisterWazaName( 2, waza2 );
    g2d->GetStringExpand( str, msg_bag_07_06 );
    break;

  case MSG_ID_WM_COMP_CANCEL:  // おぼえずに　おわった
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterWazaName( 1, waza1 );
    g2d->GetStringExpand( str, msg_bag_07_07 );
    break;

  default:
    GFL_ASSERT( 0 );
    return;
  }

  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintBeadsMessage
 * @brief   ビーズ用メッセージ表示開始
 * @date    2015.11.05
 *
 * @param   id      メッセージID
 * @param   pp      ポケモンパラメータ
 * @param   item1   道具１
 * @param   item2   道具２
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintBeadsMessage( MsgID id, const pml::pokepara::PokemonParam * pp, u32 item1, u32 item2 )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  // 全力技を覚えていない
  if( id == MSG_ID_BEADS_WAZA_NONE )
  {
    g2d->GetString( str, msg_bag_09_20 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
  }
  // もたせます
  else if( id == MSG_ID_BEADS_START )
  {
    g2d->SetRegisterItemName( 0, item1 );
    g2d->SetRegisterItemName( 1, item2 );
    g2d->GetStringExpand( str, msg_bag_09_30 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintZCubeMessage
 * @brief   ジガルデキューブ用メッセージ表示開始
 * @date    2015.11.27
 *
 * @param   id        メッセージID
 * @param   collect   回収した数
 * @param   cube      キューブに入っている数
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintZCubeMessage( MsgID id, u32 collect, u32 cube )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_ZCUBE_FORM_ERR: // この　とくせいでは　すがたを　かえられません
    g2d->GetString( str, msg_bag_06_123 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_ZCUBE_CHECK:    // @0/100こ　かいしゅう　ぜんぶで　@1こ
    g2d->SetRegisterNumber( 0, collect, 3 );
    g2d->SetRegisterNumber( 1, cube, 3 );
    g2d->GetStringExpand( str, msg_bag_06_120 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintSortMessage
 * @brief   ソート用メッセージ表示開始
 * @date    2015.04.27
 *
 * @param   id  メッセージID
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintSortMessage( MsgID id )
{
  if( !( id >= MSG_ID_SORT_TYPE && id <= MSG_ID_SORT_NEW ) )
  {
    GFL_ASSERT( 0 );
    return;
  }

  static const u32 tbl[] =
  {
    msg_bag_04_04,
    msg_bag_04_05,
    msg_bag_04_06,
    msg_bag_04_15,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->GetString( str, tbl[id-MSG_ID_SORT_TYPE] );
  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
//  SetMessageWindow( str );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintItemUseMessage
 * @brief   道具使用メッセージ表示開始
 * @date    2015.04.21
 *
 * @param   id        回復タイプID
 * @param   pp        ポケモンパラメータ
 * @param   prm_tbl   パラメータテーブル
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintItemUseMessage( u32 id, const pml::pokepara::PokemonParam * pp, s32 * prm_tbl )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case item::ITEM_TYPE_LV_UP:       // LvUp系
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->SetRegisterNumber( 1, prm_tbl[0], 3 );
    g2d->GetStringExpand( str, msg_bag_06_90 );
    break;
  case item::ITEM_TYPE_NEMURI_RCV:  // 眠り回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_07 );
    break;
  case item::ITEM_TYPE_DOKU_RCV:    // 毒回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_03 );
    break;
  case item::ITEM_TYPE_YAKEDO_RCV:  // 火傷回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_05 );
    break;
  case item::ITEM_TYPE_KOORI_RCV:   // 氷回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_06 );
    break;
  case item::ITEM_TYPE_MAHI_RCV:    // 麻痺回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_04 );
    break;
  case item::ITEM_TYPE_ALL_ST_RCV:  // 全快
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_08 );
    break;
  case item::ITEM_TYPE_HP_RCV:      // HP回復
    g2d->SetRegisterPokeNickName( 0, pp );
    if( prm_tbl[0] != 0 )
    {
      g2d->SetRegisterNumber( 1, prm_tbl[0], 3 );
      g2d->GetStringExpand( str, msg_bag_06_01 );
		}else{
      g2d->GetStringExpand( str, msg_bag_06_08 );
		}
    break;
  case item::ITEM_TYPE_ALLDETH_RCV: // 全員瀕死回復
  case item::ITEM_TYPE_DEATH_RCV:   // 瀕死回復
    g2d->SetRegisterPokeNickName( 0, pp );
    g2d->GetStringExpand( str, msg_bag_06_02 );
    break;

  case item::ITEM_TYPE_HP_UP:       // HP努力値UP
  case item::ITEM_TYPE_ATC_UP:      // 攻撃努力値UP
  case item::ITEM_TYPE_DEF_UP:      // 防御努力値UP
  case item::ITEM_TYPE_SPA_UP:      // 特攻努力値UP
  case item::ITEM_TYPE_SPD_UP:      // 特防努力値UP
  case item::ITEM_TYPE_AGI_UP:      // 素早さ努力値UP
    g2d->SetRegisterPokeNickName( 0, pp );
    {
      static const u32 msg_tbl[] =
      {
        msg_bag_20_06,
        msg_bag_20_01,
        msg_bag_20_02,
        msg_bag_20_03,
        msg_bag_20_04,
        msg_bag_20_05,
      };
      g2d->SetRegisterWord( 1, msg_tbl[id-item::ITEM_TYPE_HP_UP] );
    }
    g2d->GetStringExpand( str, msg_bag_06_10 );
    break;

  case item::ITEM_TYPE_HP_DOWN:     // HP努力値DOWN
  case item::ITEM_TYPE_ATC_DOWN:    // 攻撃努力値DOWN
  case item::ITEM_TYPE_DEF_DOWN:    // 防御努力値DOWN
  case item::ITEM_TYPE_SPA_DOWN:    // 特攻努力値DOWN
  case item::ITEM_TYPE_SPD_DOWN:    // 特防努力値DOWN
  case item::ITEM_TYPE_AGI_DOWN:    // 素早さ努力値DOWN
    g2d->SetRegisterPokeNickName( 0, pp );
    {
      static const u32 msg_tbl[] =
      {
        msg_bag_20_06,
        msg_bag_20_01,
        msg_bag_20_02,
        msg_bag_20_03,
        msg_bag_20_04,
        msg_bag_20_05,
      };
      g2d->SetRegisterWord( 1, msg_tbl[id-item::ITEM_TYPE_HP_DOWN] );
    }
    // なつき度アップ
    if( prm_tbl[0] > 0 )
    {
      // 努力値ダウン
      if( prm_tbl[1] < 0 )
      {
        g2d->GetStringExpand( str, msg_bag_06_11 );
      }
      // 努力値そのまま
      else
      {
        g2d->GetStringExpand( str, msg_bag_06_13 );
      }
    }
    // なつき度そのまま・努力値ダウン
    else
    {
      g2d->GetStringExpand( str, msg_bag_06_12 );
    }
    break;

  case item::ITEM_TYPE_EVO:         // 進化系
    // メッセージなし
    return;

  case item::ITEM_TYPE_PP_UP:       // ppUp系
  case item::ITEM_TYPE_PP_3UP:      // pp3Up系
    g2d->SetRegisterWazaName( 0, prm_tbl[0] );
    g2d->GetStringExpand( str, msg_bag_06_71 );
    break;

  case item::ITEM_TYPE_PP_RCV:      // pp回復系
    g2d->GetString( str, msg_bag_06_70 );
    break;

  default:
    g2d->GetString( str, msg_bag_06_50 );
  }

  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintLvupWazaOboeMessage
 * @brief   レベルアップ技覚え用メッセージ表示開始
 * @date    2015.04.21
 *
 * @param   pp    ポケモンパラメータ
 * @param   waza  技番号
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintLvupWazaOboeMessage( const pml::pokepara::PokemonParam * pp, WazaNo waza )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->SetRegisterPokeNickName( 0, pp );
  g2d->SetRegisterWazaName( 1, waza );
  g2d->GetStringExpand( str, msg_bag_07_08 );
  SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintFreeSpaceMessage
 * @brief   フリースペース用メッセージ表示開始
 * @date    2015.04.24
 *
 * @param   id      メッセージID
 * @param   item    道具
 * @param   pocket  ポケット
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintFreeSpaceMessage( MsgID id, u32 item, u32 pocket )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_ADD_FREESPACE:  // フリースペースへ　いどうしました
    g2d->SetRegisterItemName( 0, item );
    g2d->GetStringExpand( str, msg_bag_04_09 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SUB_FREESPACE:  // (ポケット名)へ　もどしました
    g2d->SetRegisterItemName( 0, item );
    g2d->GetWordSet()->RegisterFieldPocketName( 1, pocket );
    g2d->GetStringExpand( str, msg_bag_04_10 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintSaleMessage
 * @brief   道具売却用メッセージ表示開始
 * @date    2015.07.16
 *
 * @param   id    メッセージID
 * @param   item  道具
 * @param   num   個数
 * @param   gold  値段
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintSaleMessage( MsgID id, u32 item, u32 gold )
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );

  switch( id )
  {
  case MSG_ID_SALE_SELECT:  // @0を　いくつ　うりますか？
    g2d->SetRegisterItemName( 0, item );
    g2d->GetStringExpand( str, msg_bag_06_80 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
    break;

  case MSG_ID_SALE_NOT:     // @0を　かいとることは　できません
    g2d->SetRegisterItemName( 0, item );
    g2d->GetStringExpand( str, msg_bag_06_81 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SALE_YESNO:   // @0円で　おひきとり　いたしましょう
    g2d->SetRegisterNumber( 0, gold, Savedata::Misc::SELL_PRICE_KETA );
    g2d->GetStringExpand( str, msg_bag_06_82 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_NONE );
    break;

  case MSG_ID_SALE_COMP:    // @0を　わたして　@1円を　うけとった
    g2d->SetRegisterItemName( 0, item );
    g2d->SetRegisterNumber( 1, gold, Savedata::Misc::SELL_PRICE_KETA );
    g2d->GetStringExpand( str, msg_bag_06_83 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  case MSG_ID_SALE_COMP_2:  // @0を　わたして　@1円を　うけとった（複数用）
    g2d->SetRegisterItemName( 0, item );
    g2d->SetRegisterNumber( 1, gold, Savedata::Misc::SELL_PRICE_KETA );
    g2d->GetStringExpand( str, msg_bag_06_83_2 );
    SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
    break;

  default:
    GFL_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PrintSasiosaeMessage
 * @brief   さしおさえエラーメッセージ表示開始
 * @date    2015.12.24
 *
 * @param   pp  ポケモンパラメータ
 */
//-----------------------------------------------------------------------------
void BagDraw::PrintSasiosaeMessage( const pml::pokepara::PokemonParam * pp )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->SetRegisterPokeNickName( 0, pp );
  g2d->SetRegisterWazaName( 1, WAZANO_SASIOSAE );
  g2d->GetStringExpand( str, msg_bag_09_50 );
  SetPrintMessage( str, gfl2::str::StrWin::FINISH_USER );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetString
 * @brief   文字列取得
 * @date    2015.04.10
 *
 * @param   id  文字列ID
 *
 * @return  文字列
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * BagDraw::GetString( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 0 );
  g2d->GetString( str, id );
  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    SelectMenu
 * @brief   項目選択時のリスナー
 * @date    2015.03.10
 *
 * @param   id  メニュー項目のID
 *
 * @note  override MenuWindowListener
 */
//-----------------------------------------------------------------------------
void BagDraw::SelectMenu( u32 id )
{
  m_uiListener->OnAction( ACTION_MENU_00+id );
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeNormalMode
 * @brief   通常モードの切り替え
 * @date    2015.04.15
 *
 * @param   flg   true = モード有効, false = モード無効
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangeNormalMode( bool flg )
{
  SetPocketButtonEnable( flg );
  SetPaneListEnable( flg );
  if( flg != false )
  {
    m_inputMode = MODE_NORMAL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePokeSelectMode
 * @brief   ポケモン選択モードの切り替え
 * @date    2015.04.13
 *
 * @param   flg   true = モード有効, false = モード無効
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePokeSelectMode( bool flg )
{
  SetPocketButtonEnable( !flg );
  SetPaneListEnable( !flg );
  PutMessageWindowSmall( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    PutPocketNewIcon
 * @brief   ポケットのNewマーク表示
 * @date    2015.04.28
 *
 * @param   pos   ポケット位置
 * @param   max   Newアイテムの個数
 */
//-----------------------------------------------------------------------------
void BagDraw::PutPocketNewIcon( u32 pos, u32 max )
{
  static const gfl2::lyt::LytPaneIndex pane_tbl[] =
  {
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_07,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_00,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_01,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_02,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_03,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_04,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_05,
    PANENAME_BAG_MIN_LOW_000_PANE_NEW_ICON_06,
  };

  static const u32 anm_tbl[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_07,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_00,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_01,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_02,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_03,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_04,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_05,
    LA_COMMONBAG_BAG_MIN_LOW_000_NEWANIME_06,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();

  if( max == 0 )
  {
    g2d->SetPaneVisible( 0, pane_tbl[pos], false );
    g2d->StopAnime( 0, anm_tbl[pos] );
  }
  else
  {
    g2d->SetPaneVisible( 0, pane_tbl[pos], true );
    g2d->StartAnime( 0, anm_tbl[pos] );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    PutItemNewIcon
 * @brief   道具のNewマーク表示
 * @date    2015.04.28
 *
 * @param   pos   位置
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagDraw::PutItemNewIcon( u32 pos, bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(ListBasePaneTable[pos]),
                                  PANENAME_BAG_PLT_LOW_001_PANE_PARTS_NEW,
                                  &res_id );
  parts->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartItemNewIconAnime
 * @brief   道具のNewマークのアニメ開始
 * @date    2015.11.13
 */
//-----------------------------------------------------------------------------
void BagDraw::StartItemNewIconAnime(void)
{
  static const u32 tbl[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07__PARTS_NEW_LOOP,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08__PARTS_NEW_LOOP,
  };

  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(tbl); i++ )
  {
    g2d->StartAnime( 0, tbl[i] );

    // テクスチャ差し替え
    gfl2::lyt::LytParts * parts = lytwk->GetPartsPane( lytwk->GetPartsPane(ListBasePaneTable[i]), PANENAME_BAG_PLT_LOW_001_PANE_PARTS_NEW, &res_id );
    m_commonIcon->ReplacePaneTextureByNewIcon( lytwk->GetPicturePane(parts,PANENAME_COMMON_ICN_013_PANE_NEW_ICON_01,&res_id) );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    SetItemChangeMode
 * @brief   道具入れ替えモード切り替え
 * @date    2015.04.30
 *
 * @param   flg   true = モード開始, false = モード終了
 */
//-----------------------------------------------------------------------------
void BagDraw::SetItemChangeMode( bool flg )
{
  m_isPaneListChangeMode = flg;
  m_pPaneListView->GetPaneList()->SetChangeFlag( flg );
  if( flg != false )
  {
    u32 pos;
    f32 scroll;
    m_pPaneListView->GetPaneList()->GetCursorData( &pos, &scroll );
    m_changeItem = m_nowPocketItem[pos+static_cast<u32>(scroll)]->id;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ResetCatchItemChangeModeFlag
 * @brief   リストの道具のキャッチムーブフラグを下げる
 * @date    2015.05.12
 */
//-----------------------------------------------------------------------------
void BagDraw::ResetCatchItemChangeModeFlag(void)
{
  m_isPaneListCatchMode = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetCatchItemChangeMode
 * @brief   リストの道具キャッチムーブモード切り替え
 * @date    2015.05.12
 *
 * @param   flg   true = モード開始, false = モード終了
 */
//-----------------------------------------------------------------------------
void BagDraw::SetCatchItemChangeMode( bool flg )
{
  m_isPaneListCatchMode = flg;
  if( flg == false )
  {
    ChangePaneListItemColor();
  }
  else
  {
    ChangePaneListItemColor( flg );

    u32 pos;
    f32 scroll;
    m_pPaneListView->GetPaneList()->GetCursorData( &pos, &scroll );
    m_changeItem = m_nowPocketItem[pos+static_cast<u32>(scroll)]->id;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    IsItemChangeMode
 * @brief   道具入れ替えモードかをチェック
 * @date    2015.04.30
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsItemChangeMode(void)
{
  return GFL_BOOL_CAST(m_isPaneListChangeMode);
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemChange
 * @brief   道具入れ替え
 * @date    2015.04.30
 *
 * @param   pos1  位置１
 * @param   pos2  位置２
 */
//-----------------------------------------------------------------------------
void BagDraw::ItemChange( int pos1, int pos2 )
{
  if( pos1 == pos2 )
  {
    return;
  }

  Savedata::MyItem::ITEM_ST tmp = *m_nowPocketItem[pos1];
  if( pos1 > pos2 )
  {
    for( int i=pos1; i>pos2; i-- )
    {
      *m_nowPocketItem[i] = *m_nowPocketItem[i-1];
    }
  }
  else if( pos1 < pos2 )
  {
    for( int i=pos1; i<pos2; i++ )
    {
      *m_nowPocketItem[i] = *m_nowPocketItem[i+1];
    }
  }
  *m_nowPocketItem[pos2] = tmp;
}

//-----------------------------------------------------------------------------
/**
 * @func    ItemChangeFreeSpace
 * @brief   道具入れ替え（フリースペース用）
 * @date    2015.04.30
 *
 * @param   pos1  位置１
 * @param   pos2  位置２
 */
//-----------------------------------------------------------------------------
void BagDraw::ItemChangeFreeSpace( int pos1, int pos2 )
{
  if( pos1 == pos2 )
  {
    return;
  }

  Savedata::MyItem::ITEM_ST * tmp = m_nowPocketItem[pos1];
  if( pos1 > pos2 )
  {
    for( int i=pos1; i>pos2; i-- )
    {
      m_nowPocketItem[i] = m_nowPocketItem[i-1];
      m_nowPocketItem[i]->free_space = i+1;
    }
  }
  else if( pos1 < pos2 )
  {
    for( int i=pos1; i<pos2; i++ )
    {
      m_nowPocketItem[i] = m_nowPocketItem[i+1];
      m_nowPocketItem[i]->free_space = i+1;
    }
  }
  m_nowPocketItem[pos2] = tmp;
  m_nowPocketItem[pos2]->free_space = pos2+1;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemIcon
 * @brief   アイテムアイコンの部品ペインを取得
 * @date    2015.04.10
 *
 * @return  部品ペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytParts * BagDraw::GetItemIcon(void)
{
  return GetG2DUtil()->GetLayoutWork(0)->GetPartsPane( PANENAME_BAG_MIN_LOW_000_PANE_PARTS_ITEM );
}

//-----------------------------------------------------------------------------
/**
 * @func    VisibleItemIcon
 * @brief   アイテムアイコン表示切り替え
 * @date    2015.05.01
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagDraw::VisibleItemIcon( bool flg )
{
  GetItemIcon()->SetVisible( flg );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemIconPicture
 * @brief   アイテムアイコンのピクチャペインを取得
 * @date    2015.05.01
 *
 * @return  ピクチャペイン
 */
//-----------------------------------------------------------------------------
gfl2::lyt::LytPicture * BagDraw::GetItemIconPicture(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  return lytwk->GetPicturePane( GetItemIcon(), PANENAME_COMMON_ICN_006_PANE_ICON_ITEM, &res_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    StartItemCatch
 * @brief   道具キッチムーブ開始
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagDraw::StartItemCatch(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();

  g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_CATCH );

  // パシるのでスケールを最小にしておく
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytPane * pane = lytwk->GetPane(
                                GetItemIcon(),
                                PANENAME_COMMON_ICN_006_PANE_ALL,
                                &res_id );
  gfl2::math::VEC2 scale( 0.0f, 0.0f );
  pane->SetScale( scale );

  Sound::PlaySE( SEQ_SE_CATCH );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndItemCatch
 * @brief   道具キッチムーブ終了
 * @date    2015.05.01
 *
 * @param   mv_pos  アイコン移動座標
 */
//-----------------------------------------------------------------------------
void BagDraw::EndItemCatch( gfl2::math::VEC3 * mv_pos )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  g2d->StopAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_CATCH );
  g2d->StartAnime( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_RELEASE );

  if( mv_pos == NULL )
  {
    m_itemCalcX = 0.0f;
    m_itemCalcY = 0.0f;
  }
  else
  {
    f32 frame = g2d->GetAnimeMaxFrame( 0, LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_RELEASE );
    gfl2::math::VEC3 pos = GetItemIcon()->GetTranslate();
    m_itemCalcX = ( static_cast<f32>( mv_pos->x ) - pos.x ) / frame;
    m_itemCalcY = ( static_cast<f32>( mv_pos->y ) - pos.y ) / frame;
  }

  Sound::PlaySE( SEQ_SE_RELEASE );
}

//-----------------------------------------------------------------------------
/**
 * @func    ReleaseItemIcon
 * @brief   道具アイコンリリース動作
 * @date    2015.05.01
 *
 * @retval  true  = 動作中
 * @retval  false = 終了
 */
//-----------------------------------------------------------------------------
bool BagDraw::ReleaseItemIcon(void)
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytParts * pane = GetItemIcon();
  if( g2d->IsAnimeEnd(0,LA_COMMONBAG_BAG_MIN_LOW_000__PARTS_ITEM_RELEASE) == false )
  {
    gfl2::math::VEC3 pos = pane->GetTranslate();
    pos.x += m_itemCalcX;
    pos.y += m_itemCalcY;
    pane->SetTranslate( pos );
    return true;
  }
  pane->SetVisible( false );
  return false;
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveItemIcon
 * @brief   アイテムアイコンキャッチムーブ
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
void BagDraw::MoveItemIcon(void)
{
  MoveItemIcon( m_lastTouchX, m_lastTouchY );
}

//-----------------------------------------------------------------------------
/**
 * @func    MoveItemIcon
 * @brief   アイテムアイコンキャッチムーブ
 * @date    2016.02.06
 *
 * @param   tpx   タッチX座標
 * @param   tpy   タッチY座標
 */
//-----------------------------------------------------------------------------
void BagDraw::MoveItemIcon( u16 tpx, u16 tpy )
{
  gfl2::lyt::LytParts * pane = GetItemIcon();
  gfl2::math::VEC3 pos = pane->GetTranslate();
  pos.x = static_cast<f32>( m_lastTouchX - ( DISPLAY_LOWER_WIDTH / 2 ) );
  pos.y = static_cast<f32>( ( DISPLAY_LOWER_HEIGHT / 2 ) - m_lastTouchY );
  pane->SetTranslate( pos );

  m_lastTouchX = tpx;
  m_lastTouchY = tpy;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemPutPosID
 * @brief   道具をリリースした場所のIDを取得
 * @date    2015.05.01
 *
 * @return  リリース場所のID
 */
//-----------------------------------------------------------------------------
BagDraw::ItemPutPosID BagDraw::GetItemPutPosID(void)
{
  return static_cast<ItemPutPosID>( m_itemPutPosID );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetItemPutIndex
 * @brief   道具リリース時の場所内のインテックスを取得
 * @date    2015.05.01
 */
//-----------------------------------------------------------------------------
s32 BagDraw::GetItemPutIndex(void)
{
  return m_itemPutIndex;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckItemListScrollPane
 * @brief   指定座標がペインリストの外部スクロール実行箇所かをチェック
 * @date    2015.05.07
 *
 * @param   x   X座標
 * @param   y   Y座標
 *
 * @retval   1 = 下方向へスクロール
 * @retval  -1 = 上方向へスクロール
 * @retval   0 = スクロールしない
 */
//-----------------------------------------------------------------------------
int BagDraw::CheckItemListScrollPane( u16 x, u16 y )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  if( g2d->GetHitPane( 0, PANENAME_BAG_MIN_LOW_000_PANE_BOUND_01, x, y ) == true )
  {
    return 1;
  }
  if( g2d->GetHitPane( 0, PANENAME_BAG_MIN_LOW_000_PANE_BOUND_00, x, y ) == true )
  {
    return -1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
/**
 * @func    CheckPocketIconHit
 * @brief   指定座標にポケットアイコンがあるか
 * @date    2015.05.08
 *
 * @param   x   X座標
 * @param   y   Y座標
 *
 * @retval  FIELD_POCKET_MAX = ポケットなし
 * @retval  FIELD_POCKET_MAX != ポケット番号
 */
//-----------------------------------------------------------------------------
u32 BagDraw::CheckPocketIconHit( u16 x, u16 y )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  for( u32 i=0; i<GFL_NELEMS(PocketBasePaneTable); i++ )
  {
    gfl2::lyt::LytPane * pane = lytwk->GetPane(
                                  lytwk->GetPartsPane(PocketBasePaneTable[i]),
                                  PANENAME_BAG_BTN_LOW_001_PANE_BOUND_00,
                                  &res_id );
    if( g2d->GetHitPane(0,pane,x,y) != false )
    {
      return i;
    }
  }
  return FIELD_POCKET_MAX;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPocketIconPos
 * @brief   ポケットアイコンの座標を取得
 * @date    2015.05.08
 *
 * @param   id  ポケット
 *
 * @return  アイコンの座標
 */
//-----------------------------------------------------------------------------
gfl2::math::VEC3 BagDraw::GetPocketIconPos( u32 id )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );
  gfl2::lyt::LytPane * pane = lytwk->GetPane(
                                lytwk->GetPartsPane(PocketBasePaneTable[id]),
                                PANENAME_BAG_BTN_LOW_001_PANE_BOUND_00,
                                &res_id );
  return g2d->GetPanePos( pane );
}

//-----------------------------------------------------------------------------
/**
 * @func    GetTouchItemPos
 * @brief   タッチ座標の項目位置を取得
 * @date    2015.05.07
 *
 * @param   tpy       タッチX座標
 * @param   tpy       タッチY座標
 * @param   pos       項目位置格納場所
 *
 * @retval  true  = 成功
 * @retval  false = 失敗
 */
//-----------------------------------------------------------------------------
bool BagDraw::GetTouchItemPos( s32 tpx, s32 tpy, int * pos )
{
	f32	tpx_f = static_cast<f32>( tpx-160 );
  f32 tpy_f = static_cast<f32>( 120-tpy );

  f32 half_x = LIST_ITEM_SX / 2.0f;
  f32 half_y = LIST_ITEM_SY / 2.0f;

  { // タッチ範囲　※PaneListのタッチ範囲生成と同じ計算にすること
	  f32 lx = LIST_ITEM_PX - half_x;
	  f32 rx = LIST_ITEM_PX + half_x;
	  f32 uy = LIST_ITEM_PY + half_y - 1.0f;
	  f32 dy = uy - LIST_ITEM_SY * LIST_ITEM_PANE_MAX + 1.0f;
    if( !( tpx_f >= lx && tpx_f < rx && tpy_f > dy && tpy_f <= uy ) )
    {
      return false;
    }
  }

  f32 item_uy = LIST_ITEM_PY + m_listItemBaseY + half_y;

  for( u32 i=0; i<LIST_ITEM_PANE_MAX; i++ )
  {
    f32 item_dy = item_uy - LIST_ITEM_SY;
    if( tpy_f < item_uy && tpy_f >= item_dy )
    {
      f32 item_cy = item_uy - half_y;
      if( tpy_f < item_cy )
      {
        *pos = i + 1;
      }
      else
      {
        *pos = i;
      }
      break;
    }
    item_uy = item_dy;
  }

  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangeScrollPutData
 * @brief   キャッチムーブの外部スクロールIDを変更
 * @date    2015.05.08
 *
 * @li  道具リスト上ならリストIDへ, それ以外はキャンセルIDへ変更
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangeScrollPutData(void)
{
  int pos = 0;
  if( GetTouchItemPos( m_lastTouchX, m_lastTouchY, &pos ) != false )
  {
    m_itemPutPosID = PUT_ID_LIST;
    m_itemPutIndex = pos;
  }
  else
  {
    m_itemPutPosID = PUT_ID_CANCEL;
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePutDataCancel
 * @brief   キャッチムーブの配置IDをキャンセルIDにする
 * @date    2015.05.13
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePutDataCancel(void)
{
  m_itemPutPosID = PUT_ID_CANCEL;
}

//-----------------------------------------------------------------------------
/**
 * @func    ChangePutDataDelete
 * @brief   キャッチムーブの配置IDを削除IDにする
 * @date    2015.11.05
 */
//-----------------------------------------------------------------------------
void BagDraw::ChangePutDataDelete(void)
{
  m_itemPutPosID = PUT_ID_DELETE;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetListItemPos
 * @brief   道具リストの項目座標を取得
 * @date    2015.05.12
 *
 * @param   list_pos  項目位置
 *
 * @return  座標
 */
//-----------------------------------------------------------------------------
gfl2::math::VEC3 BagDraw::GetListItemPos( u32 list_pos )
{
  gfl2::math::VEC3 pos;
  pos.x = LIST_ITEM_PX;
  pos.y = LIST_ITEM_PY - LIST_ITEM_SY * static_cast<f32>(list_pos);
  pos.z = 0.0f;
  return pos;
}

//-----------------------------------------------------------------------------
/**
 * @func    GetPocketIDMax
 * @brief   ポケット数を取得
 * @date    2015.05.13
 *
 * @return  ポケット数
 */
//-----------------------------------------------------------------------------
/*
u32 BagDraw::GetPocketIDMax(void)
{
  if( m_appParam->call_mode == CALL_BATTLE_MENU )
  {
    return ( BUTTON_ID_POCKET + m_appParam->pocket_max );
  }
  return FIELD_BUTTON_ID_MAX;
}
*/


//-----------------------------------------------------------------------------
/**
 * @func    売値表示
 * @brief   PutPrice
 * @date    2015.07.17
 *
 * @param   val   値段
 */
//-----------------------------------------------------------------------------
void BagDraw::PutPrice( u32 val )
{
  if( val == 0 )
  {
    SetTextboxPaneMessage( 0, PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_03, msg_bag_01_28 );
  }
  else
  {
    GetG2DUtil()->SetTextBoxPaneNumber(
      0,
      PANENAME_BAG_BTN_LOW_003_PANE_TEXTBOX_03,
      msg_bag_01_27,
      val,
      Savedata::Misc::SELL_PRICE_KETA,
      0, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  }
}


//-----------------------------------------------------------------------------
/**
 * @func    OnLayoutPaneEvent
 * @brief   ペインイベントの検知
 * @date    2015.03.05
 *
 * @param   button_id   通知されたボタンID
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagDraw::OnLayoutPaneEvent( const u32 button_id )
{
  if( m_uiListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  if( button_id == BUTTON_ID_RETURN )
  {
    m_uiListener->OnAction( ACTION_B_BUTTON );
    return DISABLE_ACCESS;
  }

//  if( button_id >= BUTTON_ID_POCKET && button_id < GetPocketIDMax() )
  if( button_id >= BUTTON_ID_POCKET && button_id < FIELD_BUTTON_ID_MAX )
  {
    m_uiListener->OnAction( ACTION_POCKET_00+(button_id-BUTTON_ID_POCKET) );
    return DISABLE_ACCESS;
  }

  return DISABLE_ACCESS;
}

//-----------------------------------------------------------------------------
/**
 * @func    OnKeyAction
 * @brief   キー入力の検知
 * @date    2015.03.05
 *
 * @param   button  ボタン
 * @param   key     十字キー
 * @param   stick   アナログスティック
 *
 * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
 * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
 * @retval  ENABLE_ACCESS         = それ以外
 *
 * @note  override UIInputListener
 */
//-----------------------------------------------------------------------------
app::ui::UIInputListener::ListenerResult BagDraw::OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick )
{
  if( m_uiListener == NULL )
  {
    return ENABLE_ACCESS;
  }

  // ポケモン選択時
  if( m_inputMode == MODE_POKE_SELECT )
  {
    return ENABLE_ACCESS;
  }

  // ペインリスト動作中
  if( m_pPaneListView != NULL && m_pPaneListView->GetPaneList()->GetUpdateResult() != app::tool::PaneList::RET_NONE )
  {
    return ENABLE_ACCESS;
  }

  if( m_nowPocketItemMax != 0 )
  {
    if( m_appParam->call_mode != CALL_BATTLE_MENU )
    {
      if( button->IsTrigger(gfl2::ui::BUTTON_X) )
      {
        m_uiListener->OnAction( ACTION_SORT );
        return DISABLE_ACCESS;
      }
    }
    if( m_appParam->call_mode == CALL_FIELD_MENU || m_appParam->call_mode == CALL_FIELD_POKELIST || m_appParam->call_mode == CALL_BOX )
    {
      if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
      {
        m_uiListener->OnAction( ACTION_IREKAE );
        return DISABLE_ACCESS;
      }
    }
  }

  if( key->IsRepeat(gfl2::ui::DIR_LEFT) )
  {
    m_uiListener->OnAction( ACTION_POCKET_LEFT );
    return DISABLE_ACCESS;
  }

  if( key->IsRepeat(gfl2::ui::DIR_RIGHT) )
  {
    m_uiListener->OnAction( ACTION_POCKET_RIGHT );
    return DISABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}


//-----------------------------------------------------------------------------
/**
 * @func    StartSelectPocketAnime
 * @brief   捕獲デモ用：ポケット選択アニメを再生
 * @date    2015.06.25
 *
 * @param   pocket  ポケット番号
 */
//-----------------------------------------------------------------------------
/*
void BagDraw::StartSelectPocketAnime( u32 pocket )
{
  static const u32 tbl[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_00_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_01_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_02_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_03_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_04_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_05_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEMTYPE_06_TOUCH_RELEASE,
  };
  m_demoAnime = tbl[pocket];
  GetG2DUtil()->StartAnime( 0, m_demoAnime );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    StartSelectItemAnime
 * @brief   捕獲デモ用：項目選択アニメを再生
 * @date    2015.06.25
 */
//-----------------------------------------------------------------------------
void BagDraw::StartSelectItemAnime(void)
{
  static const u32 tbl[] =
  {
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_00_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_01_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_02_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_03_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_04_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_05_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_06_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_07_TOUCH_RELEASE,
    LA_COMMONBAG_BAG_MIN_LOW_000__PARTSITEM_08_TOUCH_RELEASE,
  };
  m_demoAnime = tbl[ m_pPaneListView->GetPaneList()->GetPosPaneIndex(0) ];
  GetG2DUtil()->StartAnime( 0, m_demoAnime );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsDemoAnimeEnd
 * @brief   捕獲デモ用：アニメ終了チェック
 * @date    2015.06.25
 *
 * @retval  false = 再生中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool BagDraw::IsDemoAnimeEnd(void)
{
  return GetG2DUtil()->IsAnimeEnd( 0, m_demoAnime );
}


//-----------------------------------------------------------------------------
/**
 * @func    GetWazaName
 * @brief   技名取得
 * @date    2015.11.30
 *
 * @param   waza  技番号
 *
 * @return  技名
 */
//-----------------------------------------------------------------------------
gfl2::str::StrBuf * BagDraw::GetWazaName( WazaNo waza )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::str::StrBuf * str = g2d->GetTempStrBuf( 1 );
  g2d->SetRegisterWazaName( 0, waza );
  g2d->GetStringExpand( str, msg_bag_01_02 );
  return str;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetVisibleMessageWindow
 * @brief   メッセージウィンドウ表示切り替え
 * @date    2015.11.30
 *
 * @param   flg   true = 表示, false = 非表示
 */
//-----------------------------------------------------------------------------
void BagDraw::SetVisibleMessageWindow( bool flg )
{
  app::util::G2DUtil * g2d = GetG2DUtil();
  gfl2::lyt::LytWk * lytwk = g2d->GetLayoutWork( 0 );
  gfl2::lyt::LytMultiResID res_id = g2d->GetLayoutResourceID( 0 );

  gfl2::lyt::LytParts * parts = lytwk->GetPartsPane(
                                  lytwk->GetPartsPane(PANENAME_BAG_MIN_LOW_000_PANE_PARTS_COMMANDSELECT),
                                  PANENAME_COMMON_WIN_LOW_002_PANE_PARTSMSG_01,
                                  &res_id );

  g2d->SetPaneVisible( parts, flg );
}


void BagDraw::SetItemEffect( App::Tool::ItemEffect * effect )
{
  m_itemEffect = effect;
  if( m_itemEffect != NULL )
  {
    m_pAppCamera = GFL_NEW(m_heap->GetSystemHeap()) app::util::AppCamera();
    m_pAppCamera->SetViewMatrix( gfl2::math::Vector3(0.0f,0.0f,80.0f), gfl2::math::Vector3(0.0f,0.0f,0.0f), 0.0f );
    m_pAppCamera->SetOrthoProjectionMatrix( 320.0f/240.0f, 240.0f, 0.01f, 1000.0f, true );
    m_renderingManager->SetEffectAppCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pAppCamera );
  }
}

void BagDraw::UnsetItemEffect(void)
{
  if( m_itemEffect != NULL )
  {
    m_renderingManager->UnsetEffectAppCamera( app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY );
    GFL_SAFE_DELETE( m_pAppCamera );
    m_itemEffect = NULL;
  }
}



GFL_NAMESPACE_END(bag)
GFL_NAMESPACE_END(app)
