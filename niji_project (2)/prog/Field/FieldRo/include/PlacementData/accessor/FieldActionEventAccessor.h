/**
 *  GAME FREAK inc.
 *
 *  @file   FieldActionEventAccessor.h
 *  @brief  データアクセッサ
 *  @author miyachi_soichi
 *  @date   2015.05.26
 */

#if !defined(__FIELD_ACTION_EVENT_ACCESSOR_H__)
#define __FIELD_ACTION_EVENT_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field )

class FieldActionEventAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldActionEventAccessor );

public:
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 岩壊しアクション固有データ (GetActionDataで取得)
  struct ActionRockBreakerData
  {
    u32 dropItemID;       // ドロップアイテムID
  };

  // 配置情報
  struct ActionEventData
  {
    u32 type;             // 識別子
    f32 position[3];      // 座標
    f32 quaternion[4];    // 姿勢
    u32 romVersion;       // ROMバージョン
    u32 flagwork;         // フラグワークID
    u32 flagwork_num;     // ワーク値
    u32 eventID;          // イベントID
    u32 actionID;         // アクションID
    u32 actionDataOffset; // アクションデータへのオフセット
    u32 collisionOffset;  // コリジョンオフセット
  }; // struct ActionEventData

  // 配置情報（Zone単位）
  struct ActionEventDataHeader
  {
    u32               num;
    ActionEventData   data[];
  }; // struct ActionEventDataHeader
//C4200を抑止したい場合

GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldActionEventAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldActionEventAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  最大数取得
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  情報取得
   */
  const ActionEventData *GetData( u32 index ) const;

  /**
   *  @brief  アクションデータの取得
   *  @note 取得したvoid*はactionIDごとに固有のフォーマットにキャストして使用してください
   */
  const void *GetActionData( const ActionEventData &data ) const;

  /**
   *  @brief  コリジョン情報の取得
   */
  const void *GetCollisionData( const ActionEventData &data ) const;

private:

  const ActionEventDataHeader *m_pData;

}; // class FieldActionEventAccessor

GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_ACTION_EVENT_ACCESSOR_H__)