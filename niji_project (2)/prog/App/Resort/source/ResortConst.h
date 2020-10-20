#if !defined( __RESORT_CONST_H__ )
#define  __RESORT_CONST_H__
#pragma once
//======================================================================
/**
 * @file ResortConst.h
 * @date 2015/09/29 15:38:49
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Savedata/include/ResortSave.h>
#include <Savedata/include/PokeDiarySave.h>
#include <Savedata/include/Record.h>

// gflib2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// arc
#include <niji_reference_files/savedata/PokeDiaryDefine.h>


#define RESORT_DEBUG_SAVE_DATA 1    //!< デバッグ用セーブデータの設定フラグ

#if RESORT_DEBUG_SAVE_DATA
#include <System/include/gflUse.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

#define GET_RAND( max ) System::GflUse::GetPublicRand() % max

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)
#endif

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
/**
 * @brief   定数
 */
//------------------------------------------------------------------------------
//! @brief  リゾートの種類
#define RESORT_ID_FRIENDSHIP   Savedata::ResortSave::RESORT_KIND_FRIENDSHIP
#define RESORT_ID_FARM         Savedata::ResortSave::RESORT_KIND_FARM
#define RESORT_ID_ADVENTURE    Savedata::ResortSave::RESORT_KIND_ADVENTURE
#define RESORT_ID_GIM          Savedata::ResortSave::RESORT_KIND_GIM
#define RESORT_ID_HOTSPA       Savedata::ResortSave::RESORT_KIND_HOTSPA
#define RESORT_ID_MAX          Savedata::ResortSave::RESORT_KIND_MAX

//! @brief  リゾートレベル
enum {
  RESORT_LV_0,    //!< 未開発
  RESORT_LV_1,
  RESORT_LV_2,
  RESORT_LV_3,
  RESORT_LV_NUM,

  RESORT_DEV_LV_NUM = RESORT_LV_NUM - 1,
};

//! @brief  ポケマメ選択の種類
enum BeansSelectType
{
  BEANS_SEL_TYPE_FRIENDSHIP,    //!< ふれあい
  BEANS_SEL_TYPE_FARM,          //!< きのみ
  BEANS_SEL_TYPE_ADVENTURE,     //!< たんけん
  BEANS_SEL_TYPE_GIM,           //!< うんどう
  BEANS_SEL_TYPE_HOT_SPRINGS,   //!< おんせん
  BEANS_SEL_TYPE_TRADE,         //!< 交換
  BEANS_SEL_TYPE_NUM,
};

//! @brief  下画面のリザルト
enum LowerResult
{
  RESULT_NONE = 0,    //!< 
  RESULT_CONTINUE,    //!< 継続
  RESULT_CANCEL,      //!< 戻る
  RESULT_DECIDE,      //!< 決定
  RESULT_PARURE,      //!< ポケモンイベント：パルレへ
  RESULT_STATUS,      //!< ポケモンイベント：ステータスへ
  RESULT_DEVELOP,     //!< かいはつ
  RESULT_TRADE,       //!< こうかん
  RESULT_MAME_SEND,   //!< ポケマメながし
  RESULT_YES,         //!< はい
  RESULT_NO,          //!< いいえ
  RESULT_GIM_TAKEBACK,//!< 引き取る
  RESULT_GIM_CONTINUE,//!< つづける
  RESULT_NUM,
};

//! @brief  コース選択ID
enum CourseSelectID
{
  COURSE_SELECT_ADV,    //!< たんけんリゾート用コース選択
  COURSE_SELECT_GIM,    //!< うんどうリゾート用コース選択
  COURSE_SELECT_DEVELOP,//!< かいはつ内容選択
  COURSE_SELECT_NUM,
};

//! ポケモンペインID
enum {
  POKE_PANE_ID_NULL,          //!< NULLペイン
  POKE_PANE_ID_ICON_PARTS,    //!< アイコン用部品ペイン
  POKE_PANE_ID_ICON_PIC,      //!< アイコン用ピクチャペイン
  POKE_PANE_ID_TEXT_LV,       //!< レベルアイコン用テキスト
  POKE_PANE_ID_TEXT_LV_NUM,   //!< レベル数値用テキスト
  POKE_PANE_ID_GENDER,        //!< 性別用ペイン
  POKE_PANE_ID_NAME,          //!< 名前用テキスト
  POKE_PANE_ID_SEIKAKU,       //!< 性格用テキスト
  POKE_PANE_ID_NUM,
};


static const u32 ITEM_POKET_KINOMI = 3;     //!< アイテムポケット番号
static const u8 GIM_GROUP_POKE_MAX = Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX;

//! 開発に必要なアイテム数
static const u32 c_NeedItemNum[RESORT_ID_MAX][4][2] = {
  //{
  //  { ポケモン, マメ }
  //},
  { // Friendship
    {  0,   0 },  //!< LV.0
    { 15,  30 },  //!< LV.1
    { 30,  90 },  //!< LV.2
    {  0,   0 },  //!< LV.3
  },
  { // Farm
    { 15,  15 },  //!< LV.0
    { 30,  60 },  //!< LV.1
    { 45, 135 },  //!< LV.2
    {  0,   0 },  //!< LV.3
  },
  { // Adventure
    { 30,  30 },  //!< LV.0
    { 45,  90 },  //!< LV.1
    { 60, 180 },  //!< LV.2
    {  0,   0 },  //!< LV.3
  },
  { // Gim
    { 45,  45 },  //!< LV.0
    { 60, 120 },  //!< LV.1
    { 75, 225 },  //!< LV.2
    {  0,   0 },  //!< LV.3
  },
  { // HotSpa
    { 60,  60 },  //!< LV.0
    { 75, 150 },  //!< LV.1
    { 90, 270 },  //!< LV.2
    {  0,   0 },  //!< LV.3
  },
};


//------------------------------------------------------------------------------
/**
 * @brief   構造体
 */
//------------------------------------------------------------------------------
//! 時間構造体
struct TimeParam
{
  u8 hour;
  u8 min;
  u8 sec;

  TimeParam( void )
    : hour(0xFF)
    , min(0xFF)
    , sec(0xFF)
  { ; }

  bool IsInvalid( void ) const {
    if( hour == 0xFF && min == 0xFF && sec == 0xFF ) return true;

    return false;
  }

  void SetTotalSec( u32 totalSec )
  {
    if( totalSec == 0 )
    {
      hour = 0;
      min  = 0;
      sec  = 0;
    }
    else {
      u32 val = totalSec;

      hour = static_cast<u32>( val / 3600 );
      val = val - (hour * 3600);
      min  = static_cast<u32>( val / 60 );
      val = val - (min * 60);
      sec  = val;
    }
  }

  u32 GetTotalSec( void )
  {
    u32 val = static_cast<u32>( sec );
    val += static_cast<u32>( min * 60 );
    val += static_cast<u32>( hour * 60 * 60 );

    return val;
  }
};

//! ポケモンペインデータ
struct PokePaneData
{
  gfl2::lyt::LytPane*     null;
  gfl2::lyt::LytParts*    iconParts;
  gfl2::lyt::LytPicture*  iconPic;
  gfl2::lyt::LytTextBox*  textLv;
  gfl2::lyt::LytTextBox*  textLvNum;
  gfl2::lyt::LytPane*     gender;
  gfl2::lyt::LytTextBox*  name;
  gfl2::lyt::LytTextBox*  seikaku;

  PokePaneData( void )
    : null(NULL)
    , iconParts(NULL)
    , iconPic(NULL)
    , textLv(NULL)
    , textLvNum(NULL)
    , gender(NULL)
    , name(NULL)
    , seikaku(NULL)
  { ; }
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_CONST_H__