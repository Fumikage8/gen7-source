/**
 *  GAME FREAK inc.
 *
 *  @file   FieldTrainerAccessor.h
 *  @brief  トレーナー配置データアクセサ
 *  @author saita_kazuki
 *  @date   2015.06.04
 */

#if !defined(__FIELD_TRAINER_DATA_ACCESSOR_H__)
#define __FIELD_TRAINER_DATA_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"

GFL_NAMESPACE_BEGIN( Field );

class FieldTrainerAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldTrainerAccessor );

public:

  // 配置情報
  struct TrainerData
  {
    u32 type;                 // 識別子
    f32 position[3];          // 座標
    f32 quaternion[4];        // 姿勢
    u32 romVersion;           // ROMバージョン
    u32 flagwork;             // フラグワークID
    u32 flagwork_num;         // ワーク値
    u32 eventID;              // イベントID
    u32 characterId;          // キャラクターID
    u32 scriptID;             // スクリプトID
    u32 alias;                // エイリアス
    u32 moveDataOffset;       // 動作データへのオフセット
    u32 talkCollisionOffset;  // 話しかけコリジョンへのオフセット
    u32 collisionOffset;      // コリジョンオフセット
    u32 signalOffset;         // シグナルデータへのオフセット
    u32 pathCoreOffset;       // パスデータへのオフセット
    u32 pathPointOffset;      // 中継点演出データへのオフセット
  }; // struct TrainerData

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 配置情報（Zone単位）
  struct TrainerDataHeader
  {
    u32           num;
    TrainerData   data[];
  }; // struct TrainerDataHeader
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


  // 動作データ
  struct MoveData
  {
    u32 typeID;             // MoveCodeが格納されている
    u32 motionID;           // 視線モーションのID
    f32 startFrame;         // 開始フレーム
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldTrainerAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldTrainerAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  最大数取得
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  エイリアスデータ最大数
   */
  u32 GetAliasDataMax( void ) const;

  /**
   *  @brief  エイリアスではないデータ最大数
   */
  u32 GetNotAliasDataMax( void ) const;
  

  /**
   *  @brief  情報取得
   */
  const TrainerData *GetData( u32 index ) const;

  /**
   *  @brief 動作データの取得
   */
  const MoveData * GetMoveData( const TrainerData & data ) const;

  /**
   *  @brief 話しかけコリジョン情報の取得
   */
  const void * GetTalkCollisionData( const TrainerData & data ) const;

  /**
   *  @brief  コリジョン情報の取得
   */
  const void *GetCollisionData( const TrainerData &data ) const;

  /**
   *  @brief  シグナルデータの取得
   */
  const void *GetSignalData( const TrainerData &data ) const;

  /**
   *  @brief  パスデータの取得
   */
  const Traffic::PathCoreData *GetPathCoreData( const TrainerData &data ) const;

  /**
   *  @brief  中継点演出データの取得
   */
  const Traffic::PathPointDataHeader *GetPathPointData( const TrainerData &data ) const;

private:

  const TrainerDataHeader *m_pData;

}; // class FieldTrainerAccessor

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_TRAINER_DATA_ACCESSOR_H__)
