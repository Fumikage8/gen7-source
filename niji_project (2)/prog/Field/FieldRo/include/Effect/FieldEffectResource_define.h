//======================================================================
/**
 * @file FieldEffectResource_define.h
 * @date 2015/08/28 19:55:00
 * @author miyachi_soichi
 * @brief gfl2::Effect::Systemへ登録するリソースID定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECTRESOURCE_DEFINE_H_INCLUDED__
#define __FIELD_EFFECTRESOURCE_DEFINE_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
 * @enum EffectResourceID
 * gfl2::Effect::Systemへ登録するリソースID定義
 */
enum EffectResourceID
{
  EFFECT_RESOURCE_DASH_SMOKE,   //!< ケンタロスけむり
  EFFECT_RESOURCE_SHARK_JET,    //!< サメハダーのジェット
  EFFECT_RESOURCE_ROCK_G,       //!< 岩(土)
  EFFECT_RESOURCE_ROCK_W,       //!< 岩(水)
  EFFECT_RESOURCE_RIDE_LIGHT,   //!< 光
  EFFECT_RESOURCE_SCATTER_GRASS_TREE_0,     //!< 葉が飛び散る(通常の木)
  EFFECT_RESOURCE_SCATTER_GRASS_PLANT_0,    //!< 葉が飛び散る(通常の植物)

  EFFECT_RESOURCE_SKY_RAIN,    //!< 天候雨　空
  EFFECT_RESOURCE_GROUND_RAIN,    //!< 天候雨　地面

  EFFECT_RESOURCE_RIDE_APPER_LAND, //!< ライド乗り降り演出陸用
  EFFECT_RESOURCE_RIDE_APPER_SEA,   //!< ライド乗り降り演出海用

  EFFECT_RESOURCE_BALL_DEMO,      //!< モンスターボール演出用
  EFFECT_RESOURCE_FOOT_WATER,     //!< 水しぶき用
  EFFECT_RESOURCE_FOOT_SAND,      //!< 水しぶき用

  EFFECT_RESOURCE_SKY_SNOW,    //!< 天候雪　空
  EFFECT_RESOURCE_SKY_SNOWSTORM,    //!< 天候吹雪　空
  EFFECT_RESOURCE_SKY_DIAMONDDUST,    //!< 天候ダイヤモンドダスト　空
  EFFECT_RESOURCE_GROUND_DIAMONDDUST,    //!< 天候ダイヤモンドダスト　地面

  EFFECT_RESOURCE_FESFIRE,  //!< マツリ演出
  EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE,  //!< カイリキ岩：押し
  EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN,  //!< カイリキ岩：おとし
  
  EFFECT_RESOURCE_ROBA2_RUN_SMOKE,  //!< 地面馬けむり(荒地アクセル)
  EFFECT_RESOURCE_ROBA2_WALK_SMOKE,  //!< 地面馬けむり(荒地通常)
  EFFECT_RESOURCE_ROBA2_ACCEL_SMOKE,  //!< 地面馬けむり(アクセル)

  EFFECT_RESOURCE_CHARA_BREATH,   //!< といき

  EFFECT_RESOURCE_SHARK_RUN,    //!< サメハダーの走りしぶき
  EFFECT_RESOURCE_LAPLACE_FLOW, //!< ラプラスの走りしぶき

  EFFECT_RESOURCE_CHARA_RIPPLE, // NPC泳ぎ波紋
  EFFECT_RESOURCE_DEMO_RIDE, // イベント初回ライド演出用
  EFFECT_RESOURCE_DEMO_TRIAL2, // 試練２演出用

  EFFECT_RESOURCE_JF_LEVELUP,       //!< JoinFestaレベルアップ（Sparkle）
  EFFECT_RESOURCE_JF_SHOP_OPEN,     //!< JoinFesta開店（紙ふぶき）
  EFFECT_RESOURCE_JF_START_SPLASH,  //!< JoinFestaアトラクションスタート(クラッカー様)
  EFFECT_RESOURCE_JF_WARP,          //!< JoinFestaプレイヤー出現

  EFFECT_RESOURCE_STEP_SMOKE,       //!< 段差ジャンプ演出用

  EFFECT_RESOURCE_DEMO_TRIAL7_1,    //!< 試練７演出用
  EFFECT_RESOURCE_DEMO_TRIAL3_1,    //!< 試練３演出用
  EFFECT_RESOURCE_DEMO_TRIAL3_2,    //!< 試練３演出用
  EFFECT_RESOURCE_DEMO_TRIAL3_3,    //!< 試練３演出用
  EFFECT_RESOURCE_DEMO_TRIAL3_4,    //!< 試練３演出用
  EFFECT_RESOURCE_DEMO_TRIAL5_3,    //!< 試練５演出用
  EFFECT_RESOURCE_DEMO_TRIAL5_4,    //!< 試練５演出用
  EFFECT_RESOURCE_DEMO_TRIAL5_5,    //!< 試練５演出用
  EFFECT_RESOURCE_DEMO_TRIAL5_6,    //!< 試練５演出用

  EFFECT_RESOURCE_DEMO_FLOWER_YELLOW,   //!< 花びら：黄
  EFFECT_RESOURCE_DEMO_FLOWER_PINK,     //!< 花びら：桃
  EFFECT_RESOURCE_DEMO_FIREWORK_YELLOW, //!< 花火：黄
  EFFECT_RESOURCE_DEMO_FIREWORK_PINK,   //!< 花火：桃
  EFFECT_RESOURCE_DEMO_FIREWORK_RED,    //!< 花火：赤
  EFFECT_RESOURCE_DEMO_FIREWORK_PURPLE, //!< 花火：紫
  EFFECT_RESOURCE_DEMO_FLARE_SUN,       //!< レンズフレア：太陽
  EFFECT_RESOURCE_DEMO_FLARE_MOON,      //!< レンズフレア：月

  EFFECT_RESOURCE_DENJUMOKU_THUNDER,    //!< デンジュモクマップ 雷用 : かみなり発生
  EFFECT_RESOURCE_DENJUMOKU_EXP,        //!< デンジュモクマップ 雷用 : かみなりヒット

  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_01_LINE,     //!< 電気試練用エフェクト : 直線電流
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_02_STR_LINE, //!< 電気試練用エフェクト : 直線強電流×2
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_03_STR_SLANT,//!< 電気試練用エフェクト : 斜め強電流×1
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CHARGE, //!< 電気試練用エフェクト : クワガノン変圧器の放電開始エフェクト
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_CHARGE, //!< 電気試練用エフェクト : トゲデマル(ぬし)の帯電エフェクト
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_TOGE_END,    //!< 電気試練用エフェクト : クワガノン変圧器の放電終了エフェクト
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_Z,           //!< 電気試練用エフェクト : Z強調演出※パーティクル
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_04_WEAK_SLANT,//!< 電気試練用エフェクト : 斜め弱電流
  EFFECT_RESOURCE_DEMO_NEW_TRIAL5_01_KUWA_CRASH,  //!< 電気試練用エフェクト : クワガノン本体の帯電エフェクト（暴走開始用）

  EFFECT_RESOURCE_KAIRIKY_ROCK_SMOKE_L,   //!< 巨大カイリキ岩：押し   フェローチェマップ用
  EFFECT_RESOURCE_KAIRIKY_ROCK_DOWN_L,    //!< 巨大カイリキ岩：おとし フェローチェマップ用

  EFFECT_RESOURCE_DENJUMOKU_THUNDER2,     //!< デンジュモクマップ 雷2-1
  EFFECT_RESOURCE_DENJUMOKU_THUNDER2_ROOT,//!< デンジュモクマップ 雷2-2

  EFFECT_RESOURCE_UB_KAMI_SLASH,          //!< カミツルギイベント用 斬撃エフェクト
  EFFECT_RESOURCE_UB_KAMI_BLACKOUT,       //!< カミツルギイベント用 暗転エフェクト

  EFFECT_RESOURCE_ROTOM_POWER,            //!< ロトムパワー

  EFFECT_RESOURCE_DEMO_TRIAL2_2,          //!< 試練２演出用その２

  EFFECT_RESOURCE_R_ROCKET_1,            //!< レインボーロケット団用エフェクト

  EFFECT_RESOURCE_EF_BTFES_WARP,         //!< バトルフェス用エフェクト

  //EFFECT_RESOURCE_DUMMY_01, //!< 配列確保用ダミー

  EFFECT_RESOURCE_MAX,
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECTRESOURCE_DEFINE_H_INCLUDED__
