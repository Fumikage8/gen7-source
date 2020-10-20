//======================================================================
/**
 * @file FieldPokeFinderAccessor.h
 * @date 2015/10/01 16:58:57
 * @author miyachi_soichi
 * @brief データアクセッサ：ポケファインダー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_POKEFINDER_ACCESSOR_H_INCLUDED__
#define __FIELD_POKEFINDER_ACCESSOR_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class  FieldPokeFinderAccessor
 * @brief  データアクセッサ：ポケファインダー
 */
class FieldPokeFinderAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(FieldPokeFinderAccessor);

public:

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // ポケファインダーイベント配置情報
  struct PokeFinderData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 spotID;           // スポットID
    s32 subjectID;        // 登場ポケモンID
    u32 base_score;       // 基礎特典
    u16 place_type;       // 属性
    u16 start_focus;      // 初期ピント
    f32 cam_pos[3];       // カメラ座標
    f32 cam_qua[4];       // カメラ初期角度
    f32 cam_lim_x_min;    // カメラX最少
    f32 cam_lim_x_max;    // カメラX最大
    f32 cam_lim_y_min;    // カメラY最少
    f32 cam_lim_y_max;    // カメラY最大
    u32 collisionOffset;  // 起動範囲データ
    u32 moveAOffset;      // 巡回Aデータ
    u32 moveBOffset;      // 巡回Bデータ
    u32 waitOffset;       // 待機データ
    u32 onewayOffset;     // 一方通行データ
  };
  // ポケファインダーイベント配置情報Header
  struct PokeFinderDataHeader
  {
    u32 num;
    PokeFinderData data[];
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldPokeFinderAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~FieldPokeFinderAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  最大数
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  情報取得
   */
  const PokeFinderData *GetData( u32 index ) const;

  /**
   *  @brief  コリジョン情報の取得
   */
  const void *GetCollisionData( const PokeFinderData &data ) const;

  /**
   *  @brief  巡回グラフA情報の取得
   */
  const void *GetMoveAData( const PokeFinderData &data ) const;

  /**
   *  @brief  巡回グラフB情報の取得
   */
  const void *GetMoveBData( const PokeFinderData &data ) const;

  /**
   *  @brief  待機位置情報の取得
   */
  const void *GetWaitData( const PokeFinderData &data ) const;

  /**
   *  @brief  一方通行パスの取得
   */
  const void *GetOnewayData( const PokeFinderData &data ) const;

private:
  const PokeFinderDataHeader *m_pData;

}; // class FieldPokeFinderAccessor

GFL_NAMESPACE_END(Field)

#endif // __FIELD_POKEFINDER_ACCESSOR_H_INCLUDED__
