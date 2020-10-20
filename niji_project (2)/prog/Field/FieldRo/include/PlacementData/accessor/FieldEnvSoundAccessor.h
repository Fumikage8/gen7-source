//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldEnvSoundAccessor.h
 *	@brief  環境音データアクセサー
 *	@author	 tomoya takahashi
 *	@date		2015.06.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_PLACEMENT_DATA_ENV_SOUND_ACCESSOR_H__ )
#define __FIELD_PLACEMENT_DATA_ENV_SOUND_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include <niji_conv_header/field/FieldEnvSoundPlayerFormType.h>

GFL_NAMESPACE_BEGIN( Field )

class FieldEnvSoundAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldEnvSoundAccessor );

public:
   enum EnvSoundTimeZone
   {
     IDX_MORNING=0, ///< 朝
     IDX_NOON,      ///< 昼
     IDX_EVENING,   ///< 夕方
     IDX_NIGHT,     ///< 夜
     IDX_MIDNIGHT,  ///< 深夜

     IDX_MAX,       ///< 最大
   };

   enum
   {
     PAN_TYPE_CAMERA, ///< カメラ依存
     PAN_TYPE_PLAYER, ///< キャラクター依存
     PAN_TYPE_NONE,   ///< パン操作なし

     PAN_TYPE_MAX,    ///< パンタイプ最大値
   };


   enum
   {
     CURVE_TBL_NUM = 128,   ///< Curveテーブル要素数
     CURVE_PARAM_MAX = 128, ///< Curveテーブル内の値の最大値
     CURVE_PARAM_PITCH_MAX = CURVE_PARAM_MAX / 4, ///< Curveテーブル内の値の最大値(Pitch用　0〜1.0fの間に0から4.0までの値を入れる）
   };

  // 配置情報
  struct EnvSoundPlacement
  {
    u32 rom_version;      // ロムバージョン
    u32 collisionOffset;  // コリジョンオフセット
    u8  data_index[IDX_MAX];  // データインデックス
    u8  priority;         // 優先度
    u16 pan_type;         // パンの依存タイプ
  }; // struct EnvSoundData


  // 音定義
  struct EnvSoundParam
  {
    u32   SE;
    u32   player_from_type;       ///< 対応するプレイヤーフォルムタイプ
    u32   volume[CURVE_TBL_NUM];
    u32   pitch[CURVE_TBL_NUM];
    u32   lawpass[CURVE_TBL_NUM];
    u32   highpass[CURVE_TBL_NUM];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 配置情報
  struct EnvSoundDataHeader
  {
    u32                     placement_num;
    EnvSoundPlacement       placement[];

    // 以降
    // u32 param_num
    // EnvSoundParam param[]
    // コリジョンデータ
  }; // struct EnvSoundDataHeader
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldEnvSoundAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldEnvSoundAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  配置情報最大数取得
   */
  u32 GetPlacementMax( void ) const;

  /**
   *  @brief  配置情報取得
   */
  const EnvSoundPlacement *GetPlacement( u32 index ) const;

  /** 
   *  @brief  音情報最大数取得
   */
  u32 GetParamMax() const;

  /**
   *  @brief  音情報取得
   */
  const EnvSoundParam * GetParam( u32 index ) const;

  /**
   *  @brief  コリジョン情報の取得
   */
  const void *GetCollisionData( const EnvSoundPlacement &data ) const;

private:

  const EnvSoundDataHeader *m_pData;

}; // class FieldEnvSoundAccessor

GFL_NAMESPACE_END( Field )


#endif	// __FIELD_PLACEMENT_DATA_ENV_SOUND_ACCESSOR_H__

