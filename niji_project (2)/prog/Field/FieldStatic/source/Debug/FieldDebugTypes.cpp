/**
* @brief  フィールド用デバッグ定義
* @file   FieldDebugTypes.cpp
* @author ikuta_junya
* @data   2015.03.08
*/

// @note フィールド上のデバック定義は可能な限りここに集める
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include <math/include/gfl2_Easing.h>

//
// PM_DEBUGが有効になっていたくでも使いたい仮コード
//


//
// PM_DEBUG
//

#if PM_DEBUG

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Debug );


// フォーカスカメラチェック用
DebugTypes::FOCUS_CAMERA_CHECK DebugTypes::s_FocusCameraCheck =
{
  false, // b32 camera_check_on;
  400 + 750, // f32 distance;
  100.0f,    // f32 geta;
};

// アトリビュート
s32 DebugTypes::s_AttrColorIndexTable[ ATTRIBUTE_MAX + 1 ] = { 0 };
s32 DebugTypes::s_AttrColorIndexAll( 1 );
gfl2::math::Vector4 DebugTypes::s_AttrColorTable[ ATTRIBUTE_MAX + 1 ] = { gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) };
f32 DebugTypes::s_AttrDrawOffset( 0.0f );

s32 DebugTypes::s_PosJumpBlockIndexX( 0 );
s32 DebugTypes::s_PosJumpBlockIndexZ( 0 );

f32 DebugTypes::s_CameraNear( 1.0f );
f32 DebugTypes::s_CameraFar( 1000.0f );

DebugTypes::CAMERA_TEST DebugTypes::s_CameraTestDefault =
{
  1, // enableDistance
  1,
  0,
  60,
  gfl2::math::Easing::INOUT_CUBIC,
  1, //    s32 inEnable;
  30,
  40,
  gfl2::math::Easing::IN_QUAD,
  1.2f,
  1.5f,
  1, // enableTargetOffest
  0.05f,
  0.3f,
  gfl2::math::Easing::OUT_SINE,
  1.2f,
};


s32 DebugTypes::s_CameraSupportDebugMode = 0;
s32 DebugTypes::s_CameraSupportEnable = 0;
f32 DebugTypes::s_SupportValueDefault = 1.0f;

Camera::Area::CAMERA_AREA_SUPPORT_SETTING_DATA DebugTypes::s_CameraSupportDefault = 
{
  Camera::Area::SupportType::ALL,
  1.45f,

  // out
  {
    0,
    60,
    gfl2::math::Easing::INOUT_SINE,
  },

  // in
  {
    90,
    135,
    gfl2::math::Easing::IN_QUAD,
  },

  false,
  0

};

Camera::Area::FOCUS_SETTING_DATA DebugTypes::s_CameraFocusDefault = 
{
  0,
  0, // distanceMode 0 fovy 1 pos
  0,
  0.75f,
  500.0f,
  600.0f,
  600.0f,
  gfl2::math::Vector3( 0.0f, 0.0f, 0.0f ),
  0.0f,
  1.0f,
  0.3f,
  
  // out
  {
    0,
    15,
    gfl2::math::Easing::OUT_QUAD,
  },

  // in
  {
    0,
    60,
    gfl2::math::Easing::OUT_SINE,
  },

  // move
  {
    0,
    60,
    gfl2::math::Easing::OUT_SINE,
  },

};

Camera::Area::FOCUS_SETTING_DATA DebugTypes::s_CameraFocus = s_CameraFocusDefault;
Camera::Area::CAMERA_AREA_SUPPORT_SETTING_DATA DebugTypes::s_CameraSupport = s_CameraSupportDefault;
u32 DebugTypes::s_BgPartsNo = 0;

DebugTypes::CAMERA_TEST DebugTypes::s_CameraTest = DebugTypes::s_CameraTestDefault;

DebugTypes::POKE_FINDER_INFO DebugTypes::s_PokeFinderInfo = 
{
  gfl2::math::Vector2( 0.0f, 0.0f )
};


// エンカウント用
bool  DebugTypes::s_isEncountDebugEnable = false;
u32   DebugTypes::s_encountRideAdjustRate = 0;
Encount::ENCPROB_PATTERN DebugTypes::s_encountProbPattern = { 0, 0, 0, 0 };


gfl2::util::DrawUtilText* DebugTypes::s_pDrawUtilTextUpper( NULL );


DebugTypes::FinderDebugParam DebugTypes::s_DebugFinderParam =
{
  0, // チュートリアル
  0, // 繰り返し
  0, -1, -1, -1, // ポケモン抽選固定化
  -1, // アピール
  -1, // アピール
  0, // 起動後停止
  0, // AllPokeFinder
  0, // 無限撮影
  0, 0, 0, 0, 0, 0, 0, // パス描画フラグ
  0, 200.0f, 700.0f, 0.45f, 0.2f, // ボイス減衰
  10.0f, 170.0f, 0.90f, // ボイスパン
  80, 5, 10, // ランダムボイス
  0, // 撮影時の一時停止
  -1, // スコアの強制上書き
  0, // 影チェック
  0,0,0,0,0,
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
  {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
  0.20f, // 面積閾値
  0, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, // AABB
};

DebugTypes::MyStatusDebugParam DebugTypes::s_DebugMyStatusParam =
{
  0, 0,     // 所持金・BP
  0, 0, 0, 0, 0, 0, // ゲーム開始時間
  0, 0, 0,  // プレイ時間
};

  // フィールド初期化時間
#if defined(GF_PLATFORM_CTR)
nn::os::Tick DebugTypes::s_DebugFieldStartTick;
nn::os::Tick DebugTypes::s_DebugFieldEndTick;
#endif// (GF_PLATFORM_CTR)

// 配置モデル
bool DebugTypes::s_isOutputStaticModelMemoryText = false;

// ギミックエンカウント
// @fix 調整がしにくいので常駐に移動
bool DebugTypes::m_isJumpMoveEnable = false;
f32 DebugTypes::m_jumpMoveSpeed = 10.0f;
f32 DebugTypes::m_pokeBaseScale = 1.0f;
bool DebugTypes::m_isEasingEnable = false;
u32 DebugTypes::m_easingFunc = gfl2::math::Easing::LINEAR;
bool DebugTypes::m_isPokeStartPosYEnable = false;
f32 DebugTypes::m_pokeStartPosYOffset = 100.0f;

//
// IDくじ
//
s32 DebugTypes::s_IdPresentForceHitNum = -1;
b32 DebugTypes::s_IdPresentForceNumber = false;
u32 DebugTypes::s_IdPresentForceNumber1 = 0;
u32 DebugTypes::s_IdPresentForceNumber2 = 0;
u32 DebugTypes::s_IdPresentForceNumber3 = 0;
u32 DebugTypes::s_IdPresentForceNumber4 = 0;
u32 DebugTypes::s_IdPresentForceNumber5 = 0;

s32 DebugTypes::s_battleInstDebugBtlInstTrainer1 = -1;
s32 DebugTypes::s_battleInstDebugBtlInstTrainer2 = -1;

s32 DebugTypes::s_battleInstDebugBtlInstForceCommError = -1;

s32 DebugTypes::s_battleRoyalInstDebugForceVsTrainerNo[3] = { -1, -1, -1 };

// アクションコマンド
u32 DebugTypes::m_isUseActionCommandCancelAssert = 0;

//
// 制御フラグ関連
//

// 制御フラグの実体
s32 DebugTypes::s_SwitchTable[ DebugTypes::Switch::MAX ] = { 

#define DEFINE(x, y, z) y,
#include "Field/FieldStatic/include/Debug/FieldDebugSwitch.h"
#undef DEFINE
};

// 制御フラグ定義名の文字列
static const c16* const sc_SwitchNameTable[ DebugTypes::Switch::MAX ] = {
#define DEFINE(x, y, z) L ## #x,
#include "Field/FieldStatic/include/Debug/FieldDebugSwitch.h"
#undef DEFINE
};

// 制御フラグの値最大値
static s32 sc_SwitchCountMaxTable[ DebugTypes::Switch::MAX ] = { 
#define DEFINE(x, y, z) z,
#include "Field/FieldStatic/include/Debug/FieldDebugSwitch.h"
#undef DEFINE
};

// 制御フラグの値の初期値
static s32 sc_SwitchInitialValueTable[ DebugTypes::Switch::MAX ] = { 
#define DEFINE(x, y, z) y,
#include "Field/FieldStatic/include/Debug/FieldDebugSwitch.h"
#undef DEFINE
};

/**
* @brief 初期化 
*
* @return　無し
*/ 
void DebugTypes::Initialize( void )
{
  for( u32 i = 0; i < GFL_NELEMS( s_AttrColorIndexTable ); i++ )
  {
    s_AttrColorIndexTable[ i ] = 1;
  }
}

/**
* @brief 制御フラグの定義値から定義名を取得する
*
* @param  swichValue    制御フラグの定義値
*
* @return 定義名(不正な場合はNULL)
*/ 
const c16* DebugTypes::GetSwitchLabel( Switch::Value swichValue )
{
  return sc_SwitchNameTable[ swichValue ];
}

/**
* @brief 制御フラグの定義値から値の最大値を取得する
*
* @param  swichValue    制御フラグの定義値
*
* @return 最大値(不正な場合は0)
*/ 
s32 DebugTypes::GetSwitchCountMax( Switch::Value swichValue )
{
  return sc_SwitchCountMaxTable[ swichValue ];
}

/**
* @brief 制御フラグの定義値から値の初期値を取得する
*
* @param  swichValue    制御フラグの定義値
*
* @return 初期値(不正な場合は0)
*/ 
s32 DebugTypes::GetSwitchInitialValue( Switch::Value swichValue )
{
  return sc_SwitchInitialValueTable[ swichValue ];
}

/**
* @brief 制御フラグの定義値から値を取得する
*
* @param  swichValue    制御フラグの定義値
*
* @return 初期値(不正な場合は0)
*/ 
s32 DebugTypes::GetSwitchValue( Switch::Value swichValue )
{
  return s_SwitchTable[ swichValue ];
}

/**
* @brief 制御フラグの定義値から値のポインタを取得する
*
* @param  swichValue    制御フラグの定義値
*
* @return ポインタ(不正な場合はNULL)
*/ 
s32* DebugTypes::GetSwitchValuePtr( Switch::Value swichValue )
{
  return &s_SwitchTable[ swichValue ];
}

/**
* @brief 制御フラグの値を設定する
*
* @param  swichValue  対象制御フラグの定義値
* @param  value       値
*
* @return 成否
*/ 
b32 DebugTypes::SetSwitchValue( Switch::Value swichValue, s32 value )
{
  if( value > GetSwitchCountMax( swichValue ) )
  {
    return false;
  }

  s_SwitchTable[ swichValue ] = value;
  return true;
}

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
b32 DebugTypes::SetSwitchInitialValue( Switch::Value swichValue, s32 value )
{
  if( value > GetSwitchCountMax( swichValue ) )
  {
    return false;
  }

  sc_SwitchInitialValueTable[ swichValue ] = value;
  return true;
}


GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif // #if PM_DEBUG