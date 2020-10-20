﻿//=============================================================================
/**
 * @file    TrainerTypeData.h
 * @brief   トレーナータイプ毎のデータ
 * @date    2012.08.29
 * @author  tamada
 */
//=============================================================================
#ifndef TRAINERTYPEDATA_H_INCLUDED
#define TRAINERTYPEDATA_H_INCLUDED

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>

#include <pml/include/item/item.h>

#include <niji_conv_header/trainer/trtype_def.h>
#include <niji_conv_header/trainer/trtype_group_def.h>
#include <niji_conv_header/battle/battle_effect_def.h>

GFL_NAMESPACE_BEGIN( trainer );


//-----------------------------------------------------------------------------
/**
 * @class TrainerTypeData
 * @brief   トレーナータイプごとのパラメータを保持しているクラス
 */
//-----------------------------------------------------------------------------
class TrainerTypeData
{
public:


  //---------------------------------------------------------------------------
  /**
   * @brief トレーナーアイコンのIDを取得する
   *
   * @param trtype  トレーナータイプ
   *
   * @note <arc/arc_index/trainer_icon.gaix> に定義されているＩＤを返します
   */
  //---------------------------------------------------------------------------
  static u8 GetTrainerIconID( TrType trtype );





public:
  ///constructor
  TrainerTypeData();
  ///destructor
  ~TrainerTypeData() {};

  //-----------------------------------------------------------------------------
  /**
   * @brief     指定トレーナータイプデータの読み込み
   * @param[in]   読み込み用テンポラリヒープ指定
   * @param[in]   トレーナータイプ指定
   */
  //-----------------------------------------------------------------------------
  void LoadData( gfl2::heap::HeapBase* pHeap, TrType trtype );

#if 0
  //-----------------------------------------------------------------------------
  /**
   * @brief     指定トレーナータイプデータの読み込み（ArcFile指定）
   * @param[in]   ArcFile
   * @param[in]   トレーナータイプ指定
   */
  //-----------------------------------------------------------------------------
  void LoadData( gfl2::fs::ArcFile * arc, TrType trtype );
#endif

  //-----------------------------------------------------------------------------
  /**
   * @brief   トレーナー性別の取得
   * @return  u8  性別(PM_MALE/PM_FEMALE/PM_NEUTRAL)
   */
  //-----------------------------------------------------------------------------
  u8 GetSex( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   トレーナー分類指定の取得
   * @return  u8  分類指定(include/trainer/trtype_group_def.h)
   *
   * @note  現状(2011.11.02)、バトル中のなつき度変化の判定にだけ使用されている
   */
  //-----------------------------------------------------------------------------
  u8 GetGroup( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief 手持ちポケモンのボールIDを取得する
   */
  //-----------------------------------------------------------------------------
  item::BALL_ID GetBallID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   複数トレーナーかどうかの取得
   * @note    ローカライズでの文法特性指定のために使用している
   * @retval  0   単数
   * @retval  1   複数
   */
  //-----------------------------------------------------------------------------
  u8 GetPlural( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   視線BGM指定の取得
   * @return  u32   視線BGM指定ID(include/sound/SoundMiddleID.h）
   */
  //-----------------------------------------------------------------------------
  u32 GetEyeBGM( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   視線SE指定の取得
   * @return  u32   視線BGM指定ID(include/sound/SoundMiddleID.h）
   */
  //-----------------------------------------------------------------------------
  u32 GetEyeSE( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   エンカウントエフェクト指定の取得
   * @return  u32 エンカウントエフェクト指定
   */
  //-----------------------------------------------------------------------------
  BattleEffectId GetBattleEffectID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief トレーナーアイコンのIDを取得する
   *
   * @note <arc/arc_index/trainer_icon.gaix> に定義されているＩＤを返します
   */
  //-----------------------------------------------------------------------------
  u8 GetTrainerIconID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief フィールドで表示するメインモデルのIDを取得する
   *
   * @note <conv_header/niji_conv_header/field/chara/chara_model_id.h> に定義されているＩＤを返します
   */
  //-----------------------------------------------------------------------------
  u16 GetFieldMainModelID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief フィールドで表示するサブモデルのIDを取得する
   *
   * @note <conv_header/niji_conv_header/field/chara/chara_model_id.h> に定義されているＩＤを返します
   */
  //-----------------------------------------------------------------------------
  u16 GetFieldSubModelID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief バトルで表示するメインモデルのIDを取得する
   *
   * @note <conv_header/niji_conv_header/battle/chara/chara_model_id.h> に定義されているＩＤを返します
   */
  //-----------------------------------------------------------------------------
  u16 GetBattleMainModelID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief バトルで表示するサブモデルのIDを取得する
   *
   * @note <conv_header/niji_conv_header/battle/chara/chara_model_id.h> に定義されているＩＤを返します
   */
  //-----------------------------------------------------------------------------
  u16 GetBattleSubModelID( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief   メガ進化アイテムID取得
   * @return  アイテムID
   *
   * @note  そのトレーナーがメガ進化に使用するキーストーンのアイテムIDを返す
   */
  //-----------------------------------------------------------------------------
  static u16 GetMegaItemId( TrType trtype );

  //-----------------------------------------------------------------------------
  /**
   * @brief 見せ場に使用されるトレーナータイプか？の判定
   * @param[in]   trtype
   * @return      true  見せ場に使用される
   *
   * @note    メガ進化を常にロングバージョンで見せるか？の判定に使用。
   * @note    もっと細かく指定したい場合はスクリプトからフラグを渡すこと
   */
  //-----------------------------------------------------------------------------
  static bool IsHighLightSceneTrainerType( TrType trtype );

  //-----------------------------------------------------------------------------
  /**
   * @brief   トレーナー分類がボスかどうか
   */
  //-----------------------------------------------------------------------------
  static bool IsBossGroup( TrTypeGroup trgroup );

private:
  //--------------------------------------------------------------
  /**
   * @brief トレーナータイプバイナリデータ
   * @caution この構造体はバイナリから読み込まれるため、変更禁止
   */
  //--------------------------------------------------------------
  struct TRTYPE_DATA
  {
    u8  sex    :2;            // 0x00 性別
    u8  plural :2;            // 0x00 単複　複数=1、単数=0
    u8         :4;            // 0x00 
    u8  group;                // 0x01 トレーナータイプグループ
    u8  ball_id;              // 0x02 手持ちポケモンのボールID( BALLID_XXXX )
    u8  btl_eff_id;           // 0x03 導入演出ID( BattleEffectId )
    u8  iconId;               // 0x04 トレーナーアイコンID
    u8  pad_1;                // 0x05 
    u8  pad_2;                // 0x06
    u8  pad_3;                // 0x07
    u16 fieldModelId_main;    // 0x08 フィールドのキャラモデルID( メイン )
    u16 fieldModelId_sub;     // 0x0a フィールドのキャラモデルID( サブ )
    u16 battleModelId_main;   // 0x0c バトルのキャラモデルID( メイン )
    u16 battleModelId_sub;    // 0x0e バトルのキャラモデルID( サブ )
    u32 eye_se;               // 0x10 nijiで視線曲あらため視線SEに変更になった
  };

  s32 mTrType;
  TRTYPE_DATA mCoreData;

};


GFL_NAMESPACE_END( trainer );

#endif  // TRAINERTYPEDATA_H_INCLUDED
