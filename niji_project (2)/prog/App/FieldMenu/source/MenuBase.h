//=============================================================================
/**
 * @file    MenuBase.h
 * @brief   フィールドメニュー基本処理
 * @author  Hiroyuki Nakamura
 * @date    2015.05.20
 */
//=============================================================================
#if !defined( MENU_BASE_H_INCLUDED )
#define MENU_BASE_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "Savedata/include/FieldMenuSave.h"
#include "AppLib/include/Ui/UIView.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Savedata)
class Misc;
class MyStatus;
class MyItem;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN(Field)
class EventWork;
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(RotomPower)
class RotomPowerManager;
GFL_NAMESPACE_END(RotomPower)
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

// 前方宣言
class FrameListener;
class PlayData;
class RotomFriendship;

//=============================================================================
/**
 * @class MenuBase
 * @brief メニュー基本処理クラス
 * @date  2015.07.13
 */
//=============================================================================
class MenuBase
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! レイアウトID
  enum LytID
  {
    LYTID_UPPER = 0,    //!< 上画面
    LYTID_LOWER_BASE,   //!< 背景
    LYTID_LOWER_ROTOM,  //!< ロトム

    LYTID_MAX,          //!< 最大数
  };

  //! ボタンID
  enum ButtonID
  {
    BTNID_MESSAGE = 0,      //!< メッセージ
    BTNID_TOWNMAP,          //!< タウンマップ
    BTNID_ZUKAN,            //!< 図鑑
    BTNID_MOTION_01,        //!< モーション１
    BTNID_MOTION_02,        //!< モーション２
    BTNID_MOTION_03,        //!< モーション３
    BTNID_MOTION_04,        //!< モーション４
    BTNID_MOTION_05,        //!< モーション５
    BTNID_MOTION_06,        //!< モーション６
    BTNID_ROTOM_SELECT_01,  //!< ロトム選択肢１
    BTNID_ROTOM_SELECT_02,  //!< ロトム選択肢２
    BTNID_MAX               //!< 最大数
  };

  static const u32 ROTOM_EVENT_MSG_ID_NONE = 0xffffffff;  //!< イベント起動じゃない場合のロトムメッセージID

  static const u32 ROTOM_TEXT_LEN = 1024;   //!< ロトムメッセージ最大文字数

  static const u32 ROTOM_MSG_FINISH_FRAME = 30 * 2;   //!< メッセージ自動終了フレーム数

  //! ロトムパーツ定義
  enum RotomParts
  {
    ROTOM_PARTS_L_EYE = 0,  //!< 左目
    ROTOM_PARTS_R_EYE,      //!< 右目
    ROTOM_PARTS_L_UPPER,    //!< 左上
    ROTOM_PARTS_R_UPPER,    //!< 右上
    ROTOM_PARTS_L_LOWER,    //!< 左下
    ROTOM_PARTS_R_LOWER,    //!< 右下
    ROTOM_PARTS_MAX,        //!< 最大値
  };

  //! ロトムアニメパーツ
  struct ROTOM_ANIME_PARTS
  {
    const u32 * anime;  //!< アニメテーブル
    u32 max;            //!< テーブル数
  };

  //! ランク別アニメデータ
  struct ROTOM_ANIME_RANK
  {
    const ROTOM_ANIME_PARTS * parts[ROTOM_PARTS_MAX];   //!< アニメパーツテーブル
  };

  // ランク１のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R1_LEFT_EYE = 1;
  static const u32 ROTOM_ANIME_MAX_R1_RIGHT_EYE = 1;
  static const u32 ROTOM_ANIME_MAX_R1_LEFT_UPPER = 1;
  static const u32 ROTOM_ANIME_MAX_R1_RIGHT_UPPER = 1;
  static const u32 ROTOM_ANIME_MAX_R1_LEFT_LOWER = 1;
  static const u32 ROTOM_ANIME_MAX_R1_RIGHT_LOWER = 1;
  // ランク１のロトムアニメ
  static const u32 RotomAnime_R1_LeftEye[ROTOM_ANIME_MAX_R1_LEFT_EYE];
  static const u32 RotomAnime_R1_RightEye[ROTOM_ANIME_MAX_R1_RIGHT_EYE];
  static const u32 RotomAnime_R1_LeftUpper[ROTOM_ANIME_MAX_R1_LEFT_UPPER];
  static const u32 RotomAnime_R1_RightUpper[ROTOM_ANIME_MAX_R1_RIGHT_UPPER];
  static const u32 RotomAnime_R1_LeftLower[ROTOM_ANIME_MAX_R1_LEFT_LOWER];
  static const u32 RotomAnime_R1_RightLower[ROTOM_ANIME_MAX_R1_RIGHT_LOWER];
  // ランク１のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R1_LeftEye;
  static const ROTOM_ANIME_PARTS RotomParts_R1_RightEye;
  static const ROTOM_ANIME_PARTS RotomParts_R1_LeftUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R1_RightUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R1_LeftLower;
  static const ROTOM_ANIME_PARTS RotomParts_R1_RightLower;
  // ランク１のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank1;

  // ランク２のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R2_LEFT_EYE = 2;
  static const u32 ROTOM_ANIME_MAX_R2_RIGHT_EYE = 2;
  static const u32 ROTOM_ANIME_MAX_R2_LEFT_UPPER = 2;
  static const u32 ROTOM_ANIME_MAX_R2_RIGHT_UPPER = 2;
  static const u32 ROTOM_ANIME_MAX_R2_LEFT_LOWER = 2;
  static const u32 ROTOM_ANIME_MAX_R2_RIGHT_LOWER = 2;
  // ランク２のロトムアニメ
  static const u32 RotomAnime_R2_LeftEye[ROTOM_ANIME_MAX_R2_LEFT_EYE];
  static const u32 RotomAnime_R2_RightEye[ROTOM_ANIME_MAX_R2_RIGHT_EYE];
  static const u32 RotomAnime_R2_LeftUpper[ROTOM_ANIME_MAX_R2_LEFT_UPPER];
  static const u32 RotomAnime_R2_RightUpper[ROTOM_ANIME_MAX_R2_RIGHT_UPPER];
  static const u32 RotomAnime_R2_LeftLower[ROTOM_ANIME_MAX_R2_LEFT_LOWER];
  static const u32 RotomAnime_R2_RightLower[ROTOM_ANIME_MAX_R2_RIGHT_LOWER];
  // ランク２のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R2_LeftEye;
  static const ROTOM_ANIME_PARTS RotomParts_R2_RightEye;
  static const ROTOM_ANIME_PARTS RotomParts_R2_LeftUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R2_RightUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R2_LeftLower;
  static const ROTOM_ANIME_PARTS RotomParts_R2_RightLower;
  // ランク２のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank2;

  // ランク３のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R3_LEFT_EYE = 3;
  static const u32 ROTOM_ANIME_MAX_R3_RIGHT_EYE = 3;
  static const u32 ROTOM_ANIME_MAX_R3_LEFT_UPPER = 3;
  static const u32 ROTOM_ANIME_MAX_R3_RIGHT_UPPER = 3;
  static const u32 ROTOM_ANIME_MAX_R3_LEFT_LOWER = 3;
  static const u32 ROTOM_ANIME_MAX_R3_RIGHT_LOWER = 3;
  // ランク３のロトムアニメ
  static const u32 RotomAnime_R3_LeftEye[ROTOM_ANIME_MAX_R3_LEFT_EYE];
  static const u32 RotomAnime_R3_RightEye[ROTOM_ANIME_MAX_R3_RIGHT_EYE];
  static const u32 RotomAnime_R3_LeftUpper[ROTOM_ANIME_MAX_R3_LEFT_UPPER];
  static const u32 RotomAnime_R3_RightUpper[ROTOM_ANIME_MAX_R3_RIGHT_UPPER];
  static const u32 RotomAnime_R3_LeftLower[ROTOM_ANIME_MAX_R3_LEFT_LOWER];
  static const u32 RotomAnime_R3_RightLower[ROTOM_ANIME_MAX_R3_RIGHT_LOWER];
  // ランク３のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R3_LeftEye;
  static const ROTOM_ANIME_PARTS RotomParts_R3_RightEye;
  static const ROTOM_ANIME_PARTS RotomParts_R3_LeftUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R3_RightUpper;
  static const ROTOM_ANIME_PARTS RotomParts_R3_LeftLower;
  static const ROTOM_ANIME_PARTS RotomParts_R3_RightLower;
  // ランク３のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank3;

  // ランク４(内気)のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_EYE_SHY = 4;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_EYE_SHY = 4;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_UPPER_SHY = 4;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_UPPER_SHY = 4;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_LOWER_SHY = 4;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_LOWER_SHY = 4;
  // ランク４(内気)のロトムアニメ
  static const u32 RotomAnime_R4_LeftEye_Shy[ROTOM_ANIME_MAX_R4_LEFT_EYE_SHY];
  static const u32 RotomAnime_R4_RightEye_Shy[ROTOM_ANIME_MAX_R4_RIGHT_EYE_SHY];
  static const u32 RotomAnime_R4_LeftUpper_Shy[ROTOM_ANIME_MAX_R4_LEFT_UPPER_SHY];
  static const u32 RotomAnime_R4_RightUpper_Shy[ROTOM_ANIME_MAX_R4_RIGHT_UPPER_SHY];
  static const u32 RotomAnime_R4_LeftLower_Shy[ROTOM_ANIME_MAX_R4_LEFT_LOWER_SHY];
  static const u32 RotomAnime_R4_RightLower_Shy[ROTOM_ANIME_MAX_R4_RIGHT_LOWER_SHY];
  // ランク４(内気)のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftEye_Shy;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightEye_Shy;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftUpper_Shy;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightUpper_Shy;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftLower_Shy;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightLower_Shy;
  // ランク４(内気)のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank4_Shy;

  // ランク４(好戦的)のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_EYE_BELLIGERENT = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_EYE_BELLIGERENT = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_UPPER_BELLIGERENT = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_UPPER_BELLIGERENT = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_LOWER_BELLIGERENT = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_LOWER_BELLIGERENT = 3;
  // ランク４(好戦的)のロトムアニメ
  static const u32 RotomAnime_R4_LeftEye_Belligerent[ROTOM_ANIME_MAX_R4_LEFT_EYE_BELLIGERENT];
  static const u32 RotomAnime_R4_RightEye_Belligerent[ROTOM_ANIME_MAX_R4_RIGHT_EYE_BELLIGERENT];
  static const u32 RotomAnime_R4_LeftUpper_Belligerent[ROTOM_ANIME_MAX_R4_LEFT_UPPER_BELLIGERENT];
  static const u32 RotomAnime_R4_RightUpper_Belligerent[ROTOM_ANIME_MAX_R4_RIGHT_UPPER_BELLIGERENT];
  static const u32 RotomAnime_R4_LeftLower_Belligerent[ROTOM_ANIME_MAX_R4_LEFT_LOWER_BELLIGERENT];
  static const u32 RotomAnime_R4_RightLower_Belligerent[ROTOM_ANIME_MAX_R4_RIGHT_LOWER_BELLIGERENT];
  // ランク４(好戦的)のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftEye_Belligerent;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightEye_Belligerent;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftUpper_Belligerent;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightUpper_Belligerent;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftLower_Belligerent;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightLower_Belligerent;
  // ランク４(好戦的)のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank4_Belligerent;

  // ランク４(人懐っこい)のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_EYE_FRIENDRY = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_EYE_FRIENDRY = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_UPPER_FRIENDRY = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_UPPER_FRIENDRY = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_LOWER_FRIENDRY = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_LOWER_FRIENDRY = 3;
  // ランク４(人懐っこい)のロトムアニメ
  static const u32 RotomAnime_R4_LeftEye_Friendly[ROTOM_ANIME_MAX_R4_LEFT_EYE_FRIENDRY];
  static const u32 RotomAnime_R4_RightEye_Friendly[ROTOM_ANIME_MAX_R4_RIGHT_EYE_FRIENDRY];
  static const u32 RotomAnime_R4_LeftUpper_Friendly[ROTOM_ANIME_MAX_R4_LEFT_UPPER_FRIENDRY];
  static const u32 RotomAnime_R4_RightUpper_Friendly[ROTOM_ANIME_MAX_R4_RIGHT_UPPER_FRIENDRY];
  static const u32 RotomAnime_R4_LeftLower_Friendly[ROTOM_ANIME_MAX_R4_LEFT_LOWER_FRIENDRY];
  static const u32 RotomAnime_R4_RightLower_Friendly[ROTOM_ANIME_MAX_R4_RIGHT_LOWER_FRIENDRY];
  // ランク４(人懐っこい)のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftEye_Friendly;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightEye_Friendly;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftUpper_Friendly;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightUpper_Friendly;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftLower_Friendly;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightLower_Friendly;
  // ランク４(人懐っこい)のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank4_Friendly;

  // ランク４(楽天的)のロトムアニメ数
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_EYE_OPTIMISTIC = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_EYE_OPTIMISTIC = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_UPPER_OPTIMISTIC = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_UPPER_OPTIMISTIC = 3;
  static const u32 ROTOM_ANIME_MAX_R4_LEFT_LOWER_OPTIMISTIC = 3;
  static const u32 ROTOM_ANIME_MAX_R4_RIGHT_LOWER_OPTIMISTIC = 3;
  // ランク４(楽天的)のロトムアニメ
  static const u32 RotomAnime_R4_LeftEye_Optimistic[ROTOM_ANIME_MAX_R4_LEFT_EYE_OPTIMISTIC];
  static const u32 RotomAnime_R4_RightEye_Optimistic[ROTOM_ANIME_MAX_R4_RIGHT_EYE_OPTIMISTIC];
  static const u32 RotomAnime_R4_LeftUpper_Optimistic[ROTOM_ANIME_MAX_R4_LEFT_UPPER_OPTIMISTIC];
  static const u32 RotomAnime_R4_RightUpper_Optimistic[ROTOM_ANIME_MAX_R4_RIGHT_UPPER_OPTIMISTIC];
  static const u32 RotomAnime_R4_LeftLower_Optimistic[ROTOM_ANIME_MAX_R4_LEFT_LOWER_OPTIMISTIC];
  static const u32 RotomAnime_R4_RightLower_Optimistic[ROTOM_ANIME_MAX_R4_RIGHT_LOWER_OPTIMISTIC];
  // ランク４(楽天的)のロトムアニメパーツ
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftEye_Optimistic;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightEye_Optimistic;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftUpper_Optimistic;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightUpper_Optimistic;
  static const ROTOM_ANIME_PARTS RotomParts_R4_LeftLower_Optimistic;
  static const ROTOM_ANIME_PARTS RotomParts_R4_RightLower_Optimistic;
  // ランク４(楽天的)のロトムアニメデータ
  static const ROTOM_ANIME_RANK RotomAnime_Rank4_Optimistic;

  //! ロトムアニメ定義
  enum RotomAnime
  {
    ROTOM_ANIME_RANK_1 = 0,           //!< ランク１
    ROTOM_ANIME_RANK_2,               //!< ランク２
    ROTOM_ANIME_RANK_3,               //!< ランク３
    ROTOM_ANIME_RANK_4_SHY,           //!< ランク４：内気
    ROTOM_ANIME_RANK_4_BELLIGERENT,   //!< ランク４：好戦的
    ROTOM_ANIME_RANK_4_FRIENDLY,      //!< ランク４：人懐っこい
    ROTOM_ANIME_RANK_4_OPTIMISTIC,    //!< ランク４：楽観的
    ROTOM_ANIME_MAX,                  //!< 最大値
  };

  //! ランク別アニメデータテーブル
  static const ROTOM_ANIME_RANK * RotomAnime_RankTable[ROTOM_ANIME_MAX];

  static const u32 ROTOM_ANIME_NULL = 0xffffffff;   //!< ロトムアニメなし
  static const u32 ROTOM_SELECT_NONE = 0xffffffff;  //!< ロトムメニュー：選択されていない

  //! ロトム選択肢表示定義
  enum RotomSelectVisible
  {
    ROTOM_SELECT_BUTTON_OFF = 0,  //!< ボタン非表示
    ROTOM_SELECT_VIEW_ENABLE,     //!< ボタン表示（選択無効）
    ROTOM_SELECT_BUTTON_ENABLE,   //!< ボタン選択有効
    ROTOM_SELECT_BUTTON_ON = 3,   //!< ボタン表示
  };

  static const f32 ROTOM_FS_TALK_START_LOT;   //!< ロトム状況報告発生確率計算値

  //! ロトポンのシーケンスID
  enum RotoponSeqID
  {
    ROTOPON_SEQID_NONE = 0,
    ROTOPON_SEQID_INIT,
    ROTOPON_SEQID_INIT_WAIT,
    ROTOPON_SEQID_START_MESSAGE,
    ROTOPON_SEQID_START_REEL,
    ROTOPON_SEQID_MAIN,
    ROTOPON_SEQID_STOP,
    ROTOPON_SEQID_PRESENT_MESSAGE,
    ROTOPON_SEQID_ITEM_SCRIPT_WAIT,
    ROTOPON_SEQID_END_MESSAGE,
    ROTOPON_SEQID_END,
  };

  static const u32 ROTOM_FACE_NONE = 0xffffffff;  //!< ロトムの表情なし定義

  //! ロトポンで入手できる道具データ
  struct ROTOPON_ITEM
  {
    u16 id;     //!< アイテム番号
    u16 type;   //!< 分類
    u32 msg;    //!< メッセージ
  };

  static const u32 ROTOPON_ITEM_MAX = 11;                   //!< ロトポンアイテム数
  static const ROTOPON_ITEM RotoponItem[ROTOPON_ITEM_MAX];  //!< ロトポンアイテム

  static const u32 ROTOM_MSG_ANM_MAX = 4;                     //!< ロトムメッセージ用動作アニメ数
  static const u32 RotomMessageAnimeTable[ROTOM_MSG_ANM_MAX]; //!< ロトムメッセージ用動作アニメテーブル

  static const u32 ROTOM_POWER_GAUGE_FRAME = 300;   //!< ロトムパワーゲージのアニメフレーム数

  //! ロトム時間帯会話
  enum RotomTimeTalk
  {
    ROTOM_TIME_TALK_NOON = 0,   //!< 昼
    ROTOM_TIME_TALK_NIGHT,      //!< 夜
    ROTOM_TIME_TALK_NOME,       //!< なし
  };

  static const u32 TIME_COLOR_ANIME_FRAME = 7;  //!< ロトム時間帯カラーアニメフレーム数
  static const u32 TIME_COLOR_ANIME_MAX = 6;    //!< ロトム時間帯カラーアニメ数
  //! ロトム時間帯カラーアニメテーブル
  static const u32 RotomTimeColorAnimeTable[TIME_COLOR_ANIME_MAX];

  // @fix MMCat[297]: DebugとReleaseでf32=>u32のキャストで差が出たので計算を変更
  static const u32 ROTOM_REPORT_HP_RATE = 20;   //!< HP状況報告の割合 ( 20% )

public:
  //! ロトム選択メッセージデータ
  struct ROTOM_QUESTION_DATA
  {
    u16 label;          //!< 開始メッセージラベル 
    u16 select1;        //!< 選択肢１のメッセージラベル
    u16 select2;        //!< 選択肢２のメッセージラベル
    u16 result1;        //!< 選択肢１を選択したときの結果
    u16 result2;        //!< 選択肢２を選択したときの結果
    s8 result1_prm;     //!< 選択肢１を選択したときの親密度増減値
    s8 result2_prm;     //!< 選択肢２を選択したときの親密度増減値
    u16 start_wk_value; //!< 開始時のイベントワーク値
    u16 next_wk_value;  //!< 選択後のイベントワーク値
  };

  //! ロトム質問特殊処理データ
	struct ROTOM_QUESTION_FUNC_DATA
  {
	  u16 label;  //!< 選択メッセージ
	  u8 func1;   //!< 項目１が選択されたときの処理
	  u8 func2;   //!< 項目２が選択されたときの処理
  };


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   * @param   mode        表示モード
   * @param   is_passive  下画面入力禁止フラグ
   */
  //-----------------------------------------------------------------------------
  MenuBase( app::util::Heap * heap, app::util::AppRenderingManager * render_man, const s32 * mode, const bool * is_passive );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~MenuBase();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetFrameListener
   * @brief   リスナーをセット
   * @date    2015.05.22
   *
   * @param   listener リスナー
   */
  //-----------------------------------------------------------------------------
  void SetFrameListener( FrameListener * listener );

  //-----------------------------------------------------------------------------
  /**
   * @func    OpenLayoutFile
   * @brief   レイアウトリソースファイルを開く
   * @date    2016.04.19
   */
  //-----------------------------------------------------------------------------
  void OpenLayoutFile(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsOpenLayoutFile
   * @brief   レイアウトリソースファイルが開いたかをチェック
   * @date    2016.04.20
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsOpenLayoutFile(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize
   * @brief   初期化
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Initialize(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitializeSync
   * @brief   初期化（同期版）
   * @date    2016.04.20
   *
   * @return  true
   */
  //-----------------------------------------------------------------------------
  bool InitializeSync(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate
   * @brief   終了処理
   * @date    2016.01.08
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Terminate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleLayout
   * @brief   上画面レイアウト表示切り替え
   * @date    2015.06.04
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleLayout( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleCover
   * @brief   ロトムの枠レイアウト表示切り替え
   * @date    2016.01.08
   *
   * @param   flg       true = 表示, false = 非表示
   * @param   is_event  true = イベント起動時
   */
  //-----------------------------------------------------------------------------
  void SetVisibleCover( bool flg, bool is_event=false );

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishRotomEye
   * @brief   ロトムの目を非表示にする
   * @date    2016.04.25
   */
  //-----------------------------------------------------------------------------
  void VanishRotomEye(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMessageAnime
   * @brief   ロトムメッセージ開始/終了アニメセット
   * @date    2016.01.08
   *
   * @param   is_enable   true = 開始, false = 終了
   * @param   is_end      true = 最終フレームから開始
   */
  //-----------------------------------------------------------------------------
  void SetMessageAnime( bool is_enable, bool is_end );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMessageAnime
   * @brief   ロトムメッセージ開始/終了アニメが再生中か
   * @date    2016.01.08
   *
   * @retval  true  = 再生中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMessageAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomPowerOnAnime
   * @brief   ロトムの電源ONアニメ再生
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void SetRotomPowerOnAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomPowerOnAnimeEnd
   * @brief   ロトムの電源ONアニメ終了チェック
   * @date    2016.01.08
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomPowerOnAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomSleepAnime
   * @brief   ロトムのスリープアニメ再生
   * @date    2016.01.16
   */
  //-----------------------------------------------------------------------------
  void SetRotomSleepAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomSleepAnimeEnd
   * @brief   ロトムのスリープアニメ終了チェック
   * @date    2016.01.16
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomSleepAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisiblePassivePane
   * @brief   パッシブ表示用ペインの表示切り替え
   * @date    2015.06.24
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisiblePassivePane( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomAnime
   * @brief   ロトムメッセージ開始アニメ再生
   * @date    2015.09.04
   *
   * @param   is_enable   true = 開始, false = 終了
   * @param   is_end      true = 最終フレームから開始
   *
   * @note  口パク停止、メッセージ処理のシーケンスクリア
   */
  //-----------------------------------------------------------------------------
  void SetRotomAnime( bool is_enable, bool is_end );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomAnimeEnd
   * @brief   ロトムメッセージ開始/終了アニメが再生中か
   * @date    2016.01.08
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomAnimeEnd(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomMsgData
   * @brief   ロトムメッセージデータセット
   * @date    2015.09.04
   *
   * @parma   msg   メッセージデータ
   */
  //-----------------------------------------------------------------------------
  void SetRotomMsgData( gfl2::str::MsgData * msg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomMessageUpdateEnable
   * @brief   ロトムメッセージ更新制御フラグをセット
   * @date    2016.01.15
   *
   * @param   flg   true = 更新許可, false = 更新停止
   */
  //-----------------------------------------------------------------------------
  void SetRotomMessageUpdateEnable( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestRotomMessage
   * @brief   ロトムメッセージリクエストセット
   * @date    2015.09.04
   *
   * @param   index       ロトムメッセージインデックス（種類）
   * @param   text        テキスト
   * @param   work        ワーク
   * @param   is_rank_on  true = 親密度ランク補正をかける　※デフォルト = true
   */
  //-----------------------------------------------------------------------------
  void SetRequestRotomMessage( u32 index, u32 text, u32 work, bool is_rank_on = true );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestRotomMessageFreeTalk
   * @brief   ロトムの会話メッセージリクエストセット
   * @date    2016.12.07
   */
  //-----------------------------------------------------------------------------
  void SetRequestRotomMessageFreeTalk(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestRotomMessageRequestTalk
   * @brief   ロトムのお願いメッセージリクエスト
   * @date    2017.02.08
   *
   * @param   req_id  お願いID
   */
  //-----------------------------------------------------------------------------
  void SetRequestRotomMessageRequestTalk( Savedata::FieldMenu::RotomRequest req_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestRotomMessageEvolution
   * @brief   進化後のロトムメッセージリクエスト
   * @date    2017.04.19
   */
  //-----------------------------------------------------------------------------
  void SetRequestRotomMessageEvolution(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRequestRotomMessageReport
   * @brief   ロトムの状況報告メッセージリクエストセット
   * @date    2016.12.07
   */
  //-----------------------------------------------------------------------------
  void SetRequestRotomMessageReport(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRotomMsgWindow
   * @brief   ロトムメッセージウィンドウ生成
   * @date    2015.11.11
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void CreateRotomMsgWindow( app::util::Heap * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRotomMsgWindow
   * @brief   ロトムメッセージウィンドウ削除
   * @date    2015.11.11
   */
  //-----------------------------------------------------------------------------
  void DeleteRotomMsgWindow(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    EndPrintRotomMessage
   * @brief   ロトムメッセージ表示終了
   * @date    2015.11.14
   */
  //-----------------------------------------------------------------------------
  void EndPrintRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRotomMessage
   * @brief   特定のロトムメッセージを破棄
   * @date    2017.04.28
   *
   * @note  @fix MMCat[310]: 画面遷移時に特定のメッセージを消す
   */
  //-----------------------------------------------------------------------------
  void DeleteRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishRotomSelectButton
   * @brief   ロトム選択肢非表示
   * @date    2017.04.18
   */
  //-----------------------------------------------------------------------------
  void VanishRotomSelectButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomMainEventUpdate
   * @brief   ロトムメッセージがメインイベントを更新したか
   * @date    2017.02.14
   *
   * @retval  true  = 更新した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomMainEventUpdate(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetEnableButton
   * @brief   ボタン入力制御切り替え
   * @date    2016.01.16
   *
   * @param   flg   true = 入力可, false = 入力不可
   */
  //-----------------------------------------------------------------------------
  void SetEnableButton( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleButton
   * @brief   ボタン表示切り替え
   * @date    2016.01.16
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleButton( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMapCenterPane
   * @brief   マップ中央のペインを取得
   * @date    2016.01.12
   *
   * @return  マップ中央のペイン
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytPane * GetMapCenterPane(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetUpperLayout
   * @brief   上画面のレイアウトを取得
   * @date    2016.03.28
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytWk * GetUpperLayout(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMultiResID
   * @brief   マルチリソースIDを取得
   * @date    2016.03.28
   *
   * @return  マルチリソースID
   */
  //-----------------------------------------------------------------------------
  gfl2::lyt::LytMultiResID GetMultiResID(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotopon
   * @brief   ロトポン開始
   * @date    2016.12.08
   */
  //-----------------------------------------------------------------------------
  void StartRotopon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsUpdateRotopon
   * @brief   ロトポン中か
   * @date    2016.12.09
   *
   * @retval  true  = ロトポン中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsUpdateRotopon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotoponItem
   * @brief   ロトポンで獲得した道具を取得
   * @date    2016.12.12
   *
   * @return  アイテム番号
   */
  //-----------------------------------------------------------------------------
  u16 GetRotoponItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotoponItemScript
   * @brief   ロトポンの道具スクリプトが起動中かを設定
   * @date    2016.12.16
   *
   * @param   flg   true = 起動中
   */
  //-----------------------------------------------------------------------------
  void SetRotoponItemScript( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTimeRotomColor
   * @brief   ロトム時間帯アニメセット
   * @date    2017.03.21
   *
   * @param   is_room         true = 室内
   * @param   is_ex_weather   true = 特殊天候（暗黒天候）
   */
  //-----------------------------------------------------------------------------
  void SetTimeRotomColor( bool is_room, bool is_ex_weather );


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   ゲームデータ関連取得
   * @date    2015.09.09
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.05.22
   *
   * @param   displayNo   描画ディスプレイ番号
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateView2D
   * @brief   2D関連生成
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void CreateView2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteView2D
   * @brief   2D関連削除
   * @date    2016.01.08
   */
  //-----------------------------------------------------------------------------
  void DeleteView2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateButton
   * @brief   ボタン生成
   * @date    2015.05.22
   */
  //-----------------------------------------------------------------------------
  void CreateButton(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartCoverAnime
   * @brief   ロトムの枠関連のアニメを再生
   * @date    2016.01.08
   *
   * @param   anm     アニメ番号
   * @param   frame   開始フレーム
   */
  //-----------------------------------------------------------------------------
  void StartCoverAnime( u32 anm, f32 frame );

  //-----------------------------------------------------------------------------
  /**
   * @func    StopCoverAnime
   * @brief   ロトムの枠関連のアニメを停止
   * @date    2016.01.08
   *
   * @param   anm     アニメ番号
   */
  //-----------------------------------------------------------------------------
  void StopCoverAnime( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsCoverAnimeEnd
   * @brief   ロトムの枠関連のアニメ終了チェック
   * @date    2016.01.08
   *
   * @param   anm     アニメ番号
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsCoverAnimeEnd( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetCoverAnimeMaxFrame
   * @brief   ロトムの枠関連のアニメの最大フレーム数を取得
   * @date    2016.01.08
   *
   * @param   anm     アニメ番号
   *
   * @return  最大フレーム数
   */
  //-----------------------------------------------------------------------------
  f32 GetCoverAnimeMaxFrame( u32 anm );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageMoveAnime
   * @brief   ロトムメッセージ開始/終了アニメ取得
   * @date    2016.12.02
   *
   * @param   in    開始アニメ
   * @param   out   終了アニメ
   */
  //-----------------------------------------------------------------------------
//  void GetRotomMessageMoveAnime( u32 * in, u32 * out );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageStartAnime
   * @brief   ロトムメッセージ開始アニメ取得
   * @date    2016.12.19
   *
   * @return  開始アニメ
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomMessageStartAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomMessageEndAnime
   * @brief   ロトムメッセージ終了アニメ取得
   * @date    2016.12.19
   *
   * @return  終了アニメ
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomMessageEndAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotomAnime
   * @brief   ロトム関連のアニメを開始
   * @date    2016.01.08
   *
   * @param   anm     アニメ番号
   * @param   is_end  true = 終了フレームから開始
   */
  //-----------------------------------------------------------------------------
  void StartRotomAnime( u32 anm, bool is_end );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomMessage
   * @brief   ロトムメッセージ初期化
   * @date    2015.09.04
   */
  //-----------------------------------------------------------------------------
  void InitRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomReportPokemonHP
   * @brief   ロトム状況報告チェック：ポケモンのHP
   * @date    2016.12.12
   *
   * @param   rank  親密度ランク
   *
   * @return  ポケモンの位置
   */
  //-----------------------------------------------------------------------------
  u32 IsRotomReportPokemonHP( Savedata::FieldMenu::RotomRank rank );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckRotomReportAlolaTime
   * @brief   ロトム状況報告チェック：時間帯変化の会話が発生するか
   * @date    2016.12.12
   *
   * @param   rank  親密度ランク
   *
   * @return  会話の時間帯
   */
  //-----------------------------------------------------------------------------
  RotomTimeTalk CheckRotomReportAlolaTime( Savedata::FieldMenu::RotomRank rank );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomReportPlayTime
   * @brief   ロトム状況報告チェック：時間経過
   * @date    2016.12.12
   *
   * @param   rank  親密度ランク
   *
   * @retval  true  = 一定時間経過
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomReportPlayTime( Savedata::FieldMenu::RotomRank rank );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomReportPokemonHP
   * @brief   ロトムの状況報告：ポケモンのHP
   * @date    2016.12.07
   *
   * @param   level       親密度の状態
   * @param   poke_index  ポケモンのインデックス
   */
  //-----------------------------------------------------------------------------
  void SetRotomReportPokemonHP( u32 level, u32 poke_index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomReportAlolaTime
   * @brief   ロトムの状況報告：昼夜
   * @date    2016.12.07
   *
   * @param   level   親密度の状態
   * @param   time    時間帯
   */
  //-----------------------------------------------------------------------------
  void SetRotomReportAlolaTime( u32 level, RotomTimeTalk time );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomReportPlayTime
   * @brief   ロトムの状況報告：連続プレイ時間
   * @date    2016.12.07
   *
   * @param   level   親密度の状態
   */
  //-----------------------------------------------------------------------------
  void SetRotomReportPlayTime( u32 level );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomReportCharacter
   * @brief   ロトムの状況報告：性格別
   * @date    2016.12.07
   *
   * @param   chara   性格
   */
  //-----------------------------------------------------------------------------
  void SetRotomReportCharacter( Savedata::FieldMenu::RotomCharacter chara );

  //-----------------------------------------------------------------------------
  /**
   * @func    ClearRotomMessageWork
   * @brief   ロトムメッセージワーククリア
   * @date    2015.09.04
   */
  //-----------------------------------------------------------------------------
  void ClearRotomMessageWork(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotomMessage
   * @brief   ロトムメッセージ更新
   * @date    2015.09.04
   */
  //-----------------------------------------------------------------------------
  void UpdateRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AfterPrintRotomMessage
   * @brief   ロトムメッセージ表示終了後の処理
   * @date    2016.12.06
   */
  //-----------------------------------------------------------------------------
  void AfterPrintRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PrintRotomMessage
   * @brief   ロトムメッセージ表示
   * @date    2015.09.04
   *
   * @retval  true  = 表示開始
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool PrintRotomMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotomMessage
   * @brief   ロトムメッセージ開始
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void StartRotomMessage( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckSelectRotomMessage
   * @brief   ロトムメッセージが選択肢ありかをチェック
   * @date    2016.12.02
   *
   * @param   msg_index   メッセージインデックス
   *
   * @retval  true  = 選択肢あり
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool CheckSelectRotomMessage( u32 msg_index );

  //-----------------------------------------------------------------------------
  /**
   * @func    CheckAnswerRotomMessage
   * @brief   ロトムメッセージが質問の答えかをチェック
   * @date    2017.05.10
   *
   * @param   msg_index   メッセージインデックス
   *
   * @retval  true  = 質問の答え
   * @retval  false = それ以外
   *
   * @fix MMCat[333]
   */
  //-----------------------------------------------------------------------------
  bool CheckAnswerRotomMessage( u32 msg_index );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleRotomSelectButton
   * @brief   ロトム選択肢表示設定
   * @date    2016.12.05
   *
   * @param   id    項目ID
   * @param   flg   設定フラグ
   */
  //-----------------------------------------------------------------------------
  void SetVisibleRotomSelectButton( u32 id, RotomSelectVisible flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomMenuString
   * @brief   ロトム選択肢文字列書き込み
   * @date    2016.12.05
   *
   * @param   id    項目ID
   * @param   str   文字列ID
   */
  //-----------------------------------------------------------------------------
  void SetRotomMenuString( u32 id, u32 str );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomSelectMenu
   * @brief   ロトム選択肢セット
   * @date    2016.12.05
   */
  //-----------------------------------------------------------------------------
  void SetRotomSelectMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateMainEventWork
   * @brief   メインイベントを進める
   * @date    2017.01.31
   *
   * @param   now_value   現在のイベントワーク値
   * @param   next_value  新たに設定するイベントワーク値
   *
   * @retval  true  = 進行した
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UpdateMainEventWork( u16 now_value, u16 next_value );

  //-----------------------------------------------------------------------------
  /**
   * @func    USetRotomSelectMessageFunc
   * @brief   ロトム分岐メッセージ特殊処理セット
   * @date    2017.02.10
   *
   * @param   msg     元メッセージ
   * @param   select  選択された項目番号
   *
   * @retval  true  = 別画面を呼び出す
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool SetRotomSelectMessageFunc( u32 msg, u32 select );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotomMotion
   * @brief   ロトムモーション開始
   * @date    2016.01.16
   *
   * @param   id  モーションID
   */
  //-----------------------------------------------------------------------------
  void StartRotomMotion( u32 id );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotomMotion
   * @brief   ロトムモーション更新
   * @date    2016.01.20
   */
  //-----------------------------------------------------------------------------
  void UpdateRotomMotion(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomMotionAnime
   * @brief   ロトムモーション中かをチェック
   * @date    2016.01.16
   *
   * @retval  true  = モーション中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomMotionAnime(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    CreateRotomFriendship
   * @brief   ロトム親密度操作クラス生成
   * @date    2016.11.28
   */
  //-----------------------------------------------------------------------------
  void CreateRotomFriendship(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    DeleteRotomFriendship
   * @brief   ロトム親密度操作クラス削除
   * @date    2016.11.28
   */
  //-----------------------------------------------------------------------------
  void DeleteRotomFriendship(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon
   * @brief   ロトポン用アップデート処理
   * @date    2016.12.09
   *
   * @retval  true  = ロトポン中
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool UpdateRotopon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_None
   * @brief   ロトポン用アップデート：待機中
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_None(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_Init
   * @brief   ロトポン用アップデート：初期化
   * @date    2016.12.09
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_Init(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_InitWait
   * @brief   ロトポン用アップデート：初期化待ち
   * @date    2016.12.09
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_InitWait(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_StartMessage
   * @brief   ロトポン用アップデート：開始メッセージ処理
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_StartMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_StartReel
   * @brief   ロトポン用アップデート：回転開始
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_StartReel(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_Main
   * @brief   ロトポン用アップデート：メイン
   * @date    2016.12.09
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_Main(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_Stop
   * @brief   ロトポン用アップデート：停止
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_Stop(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_PresentMessage
   * @brief   ロトポン用アップデート：プレゼント用メッセージ処理
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_PresentMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_ItemScriptWait
   * @brief   ロトポン用アップデート：スクリプト待ち
   * @date    2016.12.16
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_ItemScriptWait(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_EndMessage
   * @brief   ロトポン用アップデート：終了メッセージ処理
   * @date    2016.12.12
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_EndMessage(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotopon_End
   * @brief   ロトポン用アップデート：終了処理
   * @date    2016.12.09
   */
  //-----------------------------------------------------------------------------
  void UpdateRotopon_End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotoponItem
   * @brief   ロトポンで入手するアイテムを設定
   * @date    2016.12.13
   */
  //-----------------------------------------------------------------------------
  void SetRotoponItem(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomFace
   * @brief   ロトムの表情設定
   * @date    2016.12.13
   */
  //-----------------------------------------------------------------------------
  void SetRotomFace(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotomPowerGauge
   * @brief   ロトムパワーゲージ表示
   * @date    2017.02.03
   *
   * @param   is_init   画面初期化時 = true
   */
  //-----------------------------------------------------------------------------
  void StartRotomPowerGauge( bool is_init );

  //-----------------------------------------------------------------------------
  /**
   * @func    UpdateRotomPowerGauge
   * @brief   ロトムパワーゲージ更新
   * @date    2017.02.03
   */
  //-----------------------------------------------------------------------------
  void UpdateRotomPowerGauge(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomPowerGauge
   * @brief   ロトムパワーゲージ設定
   * @date    2017.02.03
   */
  //-----------------------------------------------------------------------------
  void SetRotomPowerGauge(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    GetTimeRotomColor
   * @brief   ロトム時間帯アニメデータ取得
   * @date    2017.03.21
   *
   * @param   anime   アニメ番号取得場所
   * @param   frame   フレーム取得場所
   */
  //-----------------------------------------------------------------------------
  void GetTimeRotomColor( u32 * anime, u32 * frame );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnLayoutPaneEvent
   * @brief   ペインイベントの検知
   * @date    2015.05.22
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
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 button_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnKeyAction
   * @brief   キー入力の検知
   * @date    2015.05.22
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
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button * button, gfl2::ui::VectorDevice * key, gfl2::ui::VectorDevice * stick );

  //-----------------------------------------------------------------------------
  /**
   * @func    OnTouchEvent
   * @brief   タッチパネルイベントの検知
   * @date    2016.12.12
   *
   * @param   touch_panel タッチパネル
   * @param   is_touch    true = タッチされている
   *
   * @retval  MYSUBVIEW_ONLY_ACCESS = 他のUIViewの入力を禁止
   * @retval  DISABLE_ACCESS        = 以降の同フレームでの入力イベントを排除
   * @retval  ENABLE_ACCESS         = それ以外
   *
   * @note  override UIInputListener
   */
  //-----------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel * touch_panel, bool is_touch );


private:
  app::util::Heap * m_heap;   //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ
  const s32 * m_dispMode;   //!< 下画面表示モード

  FrameListener * m_frameListener;  //!< リスナー

  GameSys::GameData * m_gameData;         //!< ゲームデータ
  Savedata::FieldMenu * m_saveFieldMenu;  //!< フィールドメニューセーブデータ
  Savedata::Misc * m_saveMisc;            //!< 未分類セーブデータ
  const Savedata::MyStatus * m_myStatus;  //!< プレイヤーステータス
  PlayData * m_playData;                  //!< フィールドメニューゲームデータ
  gfl2::ui::TouchPanel * m_touchPanel;    //!< タッチパネル
  Field::EventWork * m_eventWork;         //!< イベントワーク

  void * m_pLytResBuff;             //!< レイアウトリソースバッファ
  print::WordSet * m_pWordSet;      //!< 単語セット
  gfl2::str::StrBuf * m_pStrBuf;    //!< ロトムメッセージ用文字列バッファ（確保用）
  gfl2::str::StrBuf * m_pExpBuf;    //!< ロトムメッセージ用文字列バッファ（展開用）

  bool m_isInitialized;   //!< true = イニシャライズ終了

  gfl2::str::MsgData * m_rotomMsgData;  //!< ロトムメッセージ
  u32 m_rotomMsgSeq;        //!< ロトムメッセージシーケンス
  bool m_isRotomUpdateStop; //!< true = ロトムの更新を停止
  u32 m_rotomMsgIndex;      //!< ロトムメッセージインデックス
  u32 m_rotomMsgStartAnime; //!< ロトムメッセージ開始時に再生したアニメ

  u32 m_rotomMotionSeq;     //!< ロトムモーションシーケンス

  u32 m_subSeq;   //!< サブシーケンス

  bool m_isDrawEnable;    //!< 描画許可フラグ

  u32 m_isCreateLayout;   //!< レイアウトを生成したか

  RotomFriendship * m_pRotomFriendship;   //!< ロトム親密度
  u32 m_rotomAnime;       //!< ロトムモーションアニメ
  u32 m_rotomFace;        //!< ロトムの表情
  bool m_isRotomFriendshipUp;   //!< true = 親密度がアップ

  const ROTOM_QUESTION_DATA * m_rotomQuestion;  //!< ロトム選択メッセージデータ
  u32 m_rotomMenuSelect;

  RotoponSeqID m_rotoponSeq;  //!< ロトポンのシーケンス
  bool m_isRotoponInputTrg;   //!< ロトポン中の入力フラグ
  bool m_isRotoponScript;     //!< true = ロトポンの道具スクリプト起動中
  const MenuBase::ROTOPON_ITEM * m_rotoponItem;   //!< ロトポンで手に入れたアイテム

  const pml::PokeParty * m_pokeParty;         //!< 手持ちポケモン
  const Savedata::ConfigSave * m_configSave;  //!< コンフィグデータ
  Savedata::MyItem * m_myItem;                //!< 持ち物セーブデータ
  const Field::RotomPower::RotomPowerManager * m_rotomPower;  //!< ロトムパワー

  bool m_isRotomPowerGaugeOn; //!< ロトムパワーのゲージ表示フラグ

  bool m_isEventUpdate;   //!< true = ロトムがメインイベントを更新した

  u32 m_rotomTimeColorAnime;   //!< 時間帯によるロトムカラーアニメ
  u32 m_rotomTimeColorFrame;   //!< 時間帯によるロトムカラーアニメフレーム

  const bool * m_isPassive;   //!< 下画面の入力が禁止されているか

#if PM_DEBUG
  u16 m_debugRotomTouchPos;
  u16 m_debugRotomTouchCount;
  bool m_debugIsTimeZoneCheck;
#endif  // PM_DEBUG

};


GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // MENU_BASE_H_INCLUDED
