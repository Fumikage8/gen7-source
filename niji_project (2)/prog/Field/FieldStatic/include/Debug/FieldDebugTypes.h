/**
* @brief  フィールド用デバッグ定義
* @file   FieldDebugTypes.h
* @author ikuta_junya
* @data   2015.03.08
*/

// @note フィールド上のデバック定義は可能な限りここに集める

#if !defined( __FIELDDEBUGTYPES_H_INCLUDED__ )
#define __FIELDDEBUGTYPES_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_DrawUtilText.h>

//
// PM_DEBUGが有効になっていたくでも使いたい仮コード
//


//
// PM_DEBUG
//

#if PM_DEBUG

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugTick.h>
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include <niji_conv_header/field/attribute/attribute_id.h>
#include "Field/FieldRo/include/Encount/FieldEncountData.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );

/**
* @brief フィールドデバッグ定義クラス、staticメンバーのみを持つ想定でいます
*/
class DebugTypes
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( DebugTypes );

public:

  // フォーカスカメラチェック用
  struct FOCUS_CAMERA_CHECK
  {
    b32 camera_check_on;
    f32 distance;
    f32 geta;
  };
  static FOCUS_CAMERA_CHECK s_FocusCameraCheck;

  // アトリビュート用
  static s32 s_AttrColorIndexTable[ ATTRIBUTE_MAX + 1 ];
  static s32 s_AttrColorIndexAll;
  static gfl2::math::Vector4 s_AttrColorTable[ ATTRIBUTE_MAX + 1 ];
  static f32 s_AttrDrawOffset;

  // 自機座標Jump用
  static s32 s_PosJumpBlockIndexX;
  static s32 s_PosJumpBlockIndexZ;
  static gfl2::util::DrawUtilText* s_pDrawUtilTextUpper;     // 上画面デバッグ表示用テキスト管理
  
  //
  // カメラ用
  //
  static f32 s_CameraNear;
  static f32 s_CameraFar;
  

  struct CAMERA_TEST
  {
    s32 enable;
    s32 dashOnly;
    s32 outWait;
    s32 outFrame;
    s32 outEaseFunc;
    s32 inEnable;
    s32 inWait;
    s32 inFrame;
    s32 inEaseFunc;
    f32 raideStartValue;
    f32 outValue;
    s32 enableTargetOffest;
    f32 targetOffsetValue1;
    f32 targetOffsetValue2;
    s32 ipoEaseFunc;
    f32 raideDistanceScale;
  };

  static CAMERA_TEST s_CameraTest;
  static CAMERA_TEST s_CameraTestDefault;

  struct CameraSupportDebugMode
  {
    enum Value 
    {
      OFF = 0,
      MIN_FIX,
      MAX_FIX,

      MAX
    };
  };

  static f32 s_SupportValueDefault;
  static s32 s_CameraSupportDebugMode;
  static s32 s_CameraSupportEnable;
  static Camera::Area::CAMERA_AREA_SUPPORT_SETTING_DATA s_CameraSupport;
  static Camera::Area::CAMERA_AREA_SUPPORT_SETTING_DATA s_CameraSupportDefault;
  static Camera::Area::FOCUS_SETTING_DATA s_CameraFocus;
  static Camera::Area::FOCUS_SETTING_DATA s_CameraFocusDefault;

  static u32 s_BgPartsNo;


  //
  // ポケファインダー用
  //

  // デバッグ表示情報
  struct POKE_FINDER_INFO
  {
    gfl2::math::Vector2 cameraRotationDegree;
  };

  static POKE_FINDER_INFO s_PokeFinderInfo;



  // エンカウント用
  static bool             s_isEncountDebugEnable;
  static u32               s_encountRideAdjustRate;
  static Encount::ENCPROB_PATTERN s_encountProbPattern;


  // フィールド初期化時間
#if defined(GF_PLATFORM_CTR)
  static nn::os::Tick s_DebugFieldStartTick;
  static nn::os::Tick s_DebugFieldEndTick;
#endif// (GF_PLATFORM_CTR)

  // ファインダーデバッグ用
  struct FinderDebugParam
  {
    s32  nTutorial;     //!< 強制チュートリアル
    s32  bLoop;         //!< 強制繰り返しフラグ
    s32  bLotteryLock;  //!< ポケモン抽選をロックする
    s32  nPokeIdx;      //!< 予約上書き
    s32  nPokeIdx2;      //!< 予約上書き
    s32  nSexIdx;       //!< 性別上書き
    s32  nAppealRatio;  //!< アピールを強制
    s32  nAppealIdx;    //!< あぴーるを固定化する
    s32  nForceStop;    //!< 起動後停止
    s32  nMaxPokeIdx;   //!< AllPokeFinder用
    s32  nInfinitPhoto; //!< 無限に撮影できる
    s32  bDrawWalkerA;  //!< 巡回パスAの描画
    s32  bDrawWalkerB;  //!< 巡回パスBの描画
    s32  bDrawStatue;   //!< 待機の描画
    s32  bDrawOneway;   //!< レアの描画
    s32  bDrawScore;    //!< スコアの描画
    s32  bDrawRect;     //!< スコアの描画
    s32  bDrawCom;      //!< コメント評価の描画
    s32  bVoice;        //!< ボイス調整フラグ
    f32  fLenStart;     //!< 減衰開始距離
    f32  fLenEnd;       //!< 減衰終了距離
    f32  fVolStart;     //!< 開始ボリューム
    f32  fVolEnd;       //!< 終了ボリューム
    f32  fPanStart;     //!< パン開始
    f32  fPanEnd;       //!< パン終了
    f32  fPanPow;       //!< パン係数
    s32  nNoAppRate;    //!< アピールなし鳴き声確率
    s32  nNoAppCMin;    //!< アピールなし鳴き声クールタイム最少
    s32  nNoAppCMax;    //!< アピールなし鳴き声クールタイム最大
    s32  bSnapLock;     //!< 撮影時の一時停止
    s32  nScoreLock;    //!< スコアの強制上書き
    s32  bShadowCheck;  //!< 影チェック

    s32  nCommentEnable;
    s32  nCommentSlot;
    s32  nCommentType;
    s32  nCommentID;
    s32  nCommentMsg;

    s32  aComDataType[15];
    s32  aComDataID[15];
    s32  aComDataMsg[15];
    f32  fThreshold;    //!< 面積閾値

    s32  bEnableAABB;
    f32  fAABB_min_x;
    f32  fAABB_min_y;
    f32  fAABB_min_z;
    f32  fAABB_max_x;
    f32  fAABB_max_y;
    f32  fAABB_max_z;
  };
  static FinderDebugParam s_DebugFinderParam;

  // MyStatus編集用
  struct MyStatusDebugParam
  {
    u32 nGold;  //!< 所持金編集
    u32 nBtlPt; //!< バトルポイント編集
    s32 year;   //!< ゲーム開始時間：年
    s32 month;  //!< ゲーム開始時間：月
    s32 day;    //!< ゲーム開始時間：日
    s32 hour;   //!< ゲーム開始時間：時
    s32 minute; //!< ゲーム開始時間：分
    s32 second; //!< ゲーム開始時間：秒
    u16 nHour;  //!< プレイ時間：時
    u8  nMin;   //!< プレイ時間：分
    u8  nSec;   //!< プレイ時間：秒
  };
  static MyStatusDebugParam s_DebugMyStatusParam;

  // 配置モデル
  static bool s_isOutputStaticModelMemoryText;

  // ギミックエンカウント
  // @fix 調整がしにくいので常駐に移動
  static bool             m_isJumpMoveEnable;
  static f32              m_jumpMoveSpeed;
  static f32              m_pokeBaseScale;
  static bool             m_isEasingEnable;
  static u32              m_easingFunc;       ///< イージング関数指定
  static bool             m_isPokeStartPosYEnable;
  static f32              m_pokeStartPosYOffset;

  //
  // IDくじ
  //
  static s32  s_IdPresentForceHitNum;
  static b32  s_IdPresentForceNumber;
  static u32  s_IdPresentForceNumber1;
  static u32  s_IdPresentForceNumber2;
  static u32  s_IdPresentForceNumber3;
  static u32  s_IdPresentForceNumber4;
  static u32  s_IdPresentForceNumber5;


  // バトル施設
  static s32 s_battleInstDebugBtlInstTrainer1;
  static s32 s_battleInstDebugBtlInstTrainer2;
  static s32 s_battleInstDebugBtlInstForceCommError;

  // バトルロイヤル施設
  static s32 s_battleRoyalInstDebugForceVsTrainerNo[3];

  // アクションコマンド
  static u32 m_isUseActionCommandCancelAssert;

  //
  // 制御フラグ関連
  //

  // 制御フラグの定義
  struct Switch
  {
    enum Value
    {
#define DEFINE(x, y, z) x,
#include "FieldDebugSwitch.h"
#undef DEFINE
      MAX
    };
  };

  /**
  * @brief 初期化 
  *
  * @return　無し
  */ 
  static void Initialize( void );

  /**
  * @brief 制御フラグの定義値から定義名を取得する
  *
  * @param  swichValue    制御フラグの定義値
  *
  * @return 定義名(不正な場合はNULL)
  */ 
  static const c16* GetSwitchLabel( Switch::Value swichValue );

  /**
  * @brief 制御フラグの定義値から値の最大値を取得する
  *
  * @param  swichValue    制御フラグの定義値
  *
  * @return 最大値(不正な場合は0)
  */ 
  static s32 GetSwitchCountMax( Switch::Value swichValue );

  /**
  * @brief 制御フラグの定義値から値の初期値を取得する
  *
  * @param  swichValue    制御フラグの定義値
  *
  * @return 初期値(不正な場合は0)
  */ 
  static s32 GetSwitchInitialValue( Switch::Value swichValue );

  /**
  * @brief 制御フラグの定義値から値を取得する
  *
  * @param  swichValue    制御フラグの定義値
  *
  * @return 初期値(不正な場合は0)
  */ 
  static s32 GetSwitchValue( Switch::Value swichValue );

  /**
  * @brief 制御フラグの定義値から値のポインタを取得する
  *
  * @param  swichValue    制御フラグの定義値
  *
  * @return ポインタ(不正な場合はNULL)
  */ 
  static s32* GetSwitchValuePtr( Switch::Value swichValue );

  /**
  * @brief 制御フラグの値を設定する
  *
  * @param  swichValue  対象制御フラグの定義値
  * @param  value       値
  *
  * @return 成否
  */ 
  static b32 SetSwitchValue( Switch::Value swichValue, s32 value );

  /**
  * @brief 制御フラグの初期値を再設定する
  *
  * @param  swichValue  対象制御フラグの定義値
  * @param  value       値
  *
  * @return 成否
  *
  * @attention これを使用するとデバッグフラグ操作→Resetの挙動が変わります。変更するときは注意してください
  */ 
  static b32 SetSwitchInitialValue( Switch::Value swichValue, s32 value );

private:

  static s32 s_SwitchTable[ Switch::MAX ];    // 制御フラグ

}; // class FieldDebugTypes


GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );


#endif // #if PM_DEBUG


#if PM_DEBUG

// 制御フラグアクセス用フラグ
#define FIELD_DEBUG_SWITCH( label ) Field::Debug::DebugTypes::GetSwitchValue( label )
#define FIELD_DEBUG_SWITCH_SET( label, val ) Field::Debug::DebugTypes::SetSwitchValue( label, val )

// ※ @attention これを使用するとデバッグフラグ操作→Resetの挙動が変わります。変更するときは注意してください
#define FIELD_DEBUG_SWITCH_INITIAL_SET( label, val ) Field::Debug::DebugTypes::SetSwitchInitialValue( label, val )

#define FIELD_DEBUG_VALUE_SET( name, val ) name = val

#else

// @todo 参照はコンパイルエラーでもいいかも
#define FIELD_DEBUG_SWITCH( label )
#define FIELD_DEBUG_SWITCH_SET( label, val )
#define FIELD_DEBUG_VALUE_SET( name, val )
#define FIELD_DEBUG_SWITCH_INITIAL_SET( label, val )


#endif // #if PM_DEBUG


#endif // !defined( __FIELDDEBUGTYPES_H_INCLUDED__ )
