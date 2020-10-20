/**
 *  GAME FREAK inc.
 *
 *  @file   FieldBgEventAccessor.h
 *  @brief  BGイベント用アクセッサ
 *  @author miyachi_soichi
 *  @date   2015.06.01
 */

#if !defined(__FIELD_BG_EVENT_ACCESSOR_H__)
#define __FIELD_BG_EVENT_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN(Field)

class FieldBgEventAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldBgEventAccessor );

public:
  // Bgイベント配置情報
  struct BgEventData
  {
    u32 type;               // 識別子
    f32 position[ 3 ];      // 座標
    f32 quaternion[ 4 ];    // 姿勢
    u32 romVersion;         // ROMバージョン
    u32 flagwork;           // フラグワークID
    u32 flagwork_num;       // ワーク値
    u32 eventID;            // イベントID
    u32 scriptID;           // スクリプトID
    u32 attentionID;        // 注目情報ID
    u32 collisionOffset;    // 衝突コリジョンへのオフセット
  }; // struct BgEventData

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // Bgイベント配置情報ヘッダー
  struct BgEventDataHeader
  {
    u32 num;
    BgEventData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldBgEventAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldBgEventAccessor( void );

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
  const BgEventData *GetData( u32 index ) const;

  /**
   *  @brief  衝突用コリジョン情報の取得
   */
  const void *GetCollisionData( const BgEventData &data ) const;

private:
  const BgEventDataHeader *m_pData;

}; // class FieldBgEventAccessor

GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELD_BG_EVENT_ACCESSOR_H__)
