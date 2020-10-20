//======================================================================
/**
* @file FieldDowsingTool.h
* @date 2015/07/03 18:52:30
* @author ikuta_junya
* @brief フィールドダウジング関連の処理
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if !defined __FIELDDOWSINGTOOL_H_INCLUDED__
#define __FIELDDOWSINGTOOL_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Math.h>
#include <math/include/gfl2_Easing.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );

class FieldEventCheck;

GFL_NAMESPACE_BEGIN( Dowsing );

static const f32 NOTICE_ICON_STEP_FRAME( 2.0f ); 

struct SearchRank
{
  enum Value
  {
    RANK_A,     // 取得できる
    RANK_B,     // 10m以内
    RANK_LOST,
    RANK_NO_SEARCH,  // サーチしていない
    MAX,

    DISTANCE_RANK_START = RANK_B,
    DISTANCE_RANK_END = RANK_B,
    RANK_WORST = RANK_B,
  };
};

struct RANK_SETTING
{
  f32 border;
  f32 iconScale;
  s32 seValue;
  s32 loopSE;
  s32 noticeSE;
};

class Tool
{
public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    const FieldEventCheck*       m_pFieldEventCheck;
  };

  Tool( void );
  virtual ~Tool( void );

  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  // 最新の反応結果を取得
  SearchRank::Value Search( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range ) const;

  // 指定ランクの設定を取得
  static b32 GetSetting( SearchRank::Value value, RANK_SETTING* pOut );

private:
  
  // 一番近いアイテムまでの距離を取得
  static b32 GetNearestItemDistance( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range, f32* pOut );

  // 距離から近さのランクを取得
  static SearchRank::Value GetSerchRankByDistance( f32 distance );

  // 一番近いアイテムまでの距離を取得 (指定ゾーンチェック)
  static b32 GetNearestItemDistance( const gfl2::math::Vector3& rPos, const gfl2::math::Vector3& rDir, f32 range, u32 zoneId, f32* pOut );

  
  const FieldEventCheck*       m_pFieldEventCheck;

};

GFL_NAMESPACE_END( Dowsing );
GFL_NAMESPACE_END( Field );


#endif // __FIELDDOWSINGTOOL_H_INCLUDED__