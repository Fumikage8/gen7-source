//======================================================================
/**
 * @file ZoneDataFormat.h
 * @date 2015/07/10 11:46:06
 * @author saita_kazuki
 * @brief ゾーンデータ構造体定義
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __ZONE_DATA_FORMAT_H_INCLUDED__
#define __ZONE_DATA_FORMAT_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN( Field )

//------------------------------------------------------------------
/**
 * @brief  ゾーン毎に持つデータの構成
 */
//------------------------------------------------------------------
struct ZONE_DATA
{
  u32 bgm_id_day;                     ///< BGM指定のID 昼間 (SoundMiddleID.h参照)
  u32 bgm_id_night;                   ///< BGM指定のID 夜間 (SoundMiddleID.h参照)

  u16 event_data_id;                  ///< イベントデータファイルのアーカイブID
  u16 zone_group;                     ///< タウンマップ上での表示位置用グループ指定

  u16 msg_id;                         ///< メッセージファイルのアーカイブID
  u16 gimmick_id;                     ///< ギミックのID (FieldUniqueSeqId.h参照）

  s32 sx;                             ///< マップ開始位置X
  s32 sy;                             ///< マップ開始位置Y
  s32 sz;                             ///< マップ開始位置Z

  u16 placename_id;                   ///< 地名メッセージのID (FieldPlaceNameType.h参照）
  u8  placename_type;                 ///< 地名メッセージ用のウィンドウ
  u8  maprsc_id;                      ///< マップ種別指定

  u8  padding0;
  u8  zone_scenery;                   ///< ポケモンの記憶のＩＤ
  u8  weather_env_index;              ///< 天候環境インデックス
  u8  map_change_type;                ///< マップ切り替えタイプの指定 (FieldMapChangeType.h参照）
  u32 weather[5];                     ///< 天候
                                  
  u32 pokeride_run_flag:1;            ///< 走るポケライドできるかどうか
  u32 pokeride_fly_flag:1;            ///< そらをとぶポケライドができるかどうか
  u32 pokeride_dowsing_flag:1;        ///< ダウジングポケライドができるかどうか
  u32 bgm_change_flag:1;              ///< 自転車でBGMがかわるかどうか
  u32 escape_flag:1;                  ///< あなぬけできるかどうか
  u32 skybox_flag:1;                  ///< 天球有効フラグ
  u32 dash_flag:1;                    ///< ダッシュできるかどうか
  u32 flash_flag:1;                   ///< フラッシュフラグ
  u32 reverb_flag:1;                  ///< リバーブフラグ
  u32 breath_flag:1;                  ///< 自機の吐息のON/OFF
  u32 communication_flag:1;           ///< 通信の機能のON/OFF
  u32 bloom_flag:1;                   ///< ブルームフラグ
  u32 stereo_draw_flag:1;             ///< 立体視ON/OFF
  u32 escape_save_flag:1;             ///< 脱出先を保存を保存するかどうか
  u32 subscreen_map_mode_flag:1;      ///< 下画面がマップ表示モード
  u32 subscreen_sleep:1;              ///< 下画面をスリープにするか
  u32 townmap_mode:1;                 ///< タウンマップとフィールド下画面でプレイヤー座標を取るか、固定座標を取るかの条件が違うため用意
  u32 movemodel_draw_num_control:1;   ///< 動作モデルの表示数制御をする
  u32 disp_low_priority_for_traffic:1;///< 往来の描画優先度を下げる
  u32 chr_goggles_on:1;               ///< 異世界人がゴーグル着用
  u32 padding1:12;

  u8  environment_index;              ///< エリアデータが持っている環境データパックの番号
  u8  camera_default_index;           ///< このゾーンがデフォルトで使用するカメラデータのデータ番号(デフォルトデータローカル)
  u8  camera_support_default_index;   ///< このゾーンがデフォルトで使用するカメラサポートデータのデータ番号(デフォルトデータローカル)
  u8  island_index;                   ///< 島番号

  u16 attr_bg_near_pattern;           ///< アトリビュートから近景IDを指定するためのパターンID
  u16 battle_bg_far_type;             ///< 戦闘外背景の指定

  s32 subscreen_basis_pos_x;          ///< 下画面基準X座標
  s32 subscreen_basis_pos_z;          ///< 下画面基準Z座標

  u16 townmap_basis_pos_zone;         ///< ゾーンID。どのゾーンの座標(SubScreenBasisPosX/Z)を使うか
  u8  subscreen_map_type;             ///< 下画面マップ種類 subscreen_map_type.hの値が入る
  u8  rottom_effect_type;             ///< 下画面ロトム変化種類 FieldRottomEffectType.hの値が入る

  u8  reserve2[4];                    ///< 予備領域

};
// 84byte

GFL_NAMESPACE_END( Field )

#endif // __ZONE_DATA_FORMAT_H_INCLUDED__
