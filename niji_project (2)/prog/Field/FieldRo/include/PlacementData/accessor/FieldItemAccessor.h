//======================================================================
/**
 * @file FieldItemAccessor.h
 * @date 2015/09/04 15:42:42
 * @author ikawa_hiroki
 * @brief 配置アイテム用アクセサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined(__FIELDITEMACCESSOR_H__)
#define __FIELDITEMACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>


GFL_NAMESPACE_BEGIN(Field)

class FieldItemAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldItemAccessor );

public:
  // アイテム配置情報
  struct FieldItemData
  {
    u32 type;               // 識別子
    f32 position[ 3 ];      // 座標
    f32 quaternion[ 4 ];    // 姿勢
    u32 romVersion;         // ROMバージョン
    u32 flagwork;           // フラグワークID
    u32 flagwork_num;       // ワーク値
    u32 eventID;            // イベントID
    u32 itemID;             // 入手アイテムID
    u16 item_num;           // アイテム個数
    u16 shapeType;          // 見た目タイプ
    u32 attentionID;        // 注目情報ID
    u32 collisionOffset;    // 衝突コリジョンへのオフセット
  }; // struct FieldItemData

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // アイテム配置情報ヘッダー
  struct FieldItemDataHeader
  {
    u32 num;
    FieldItemData data[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldItemAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldItemAccessor( void );

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
  const FieldItemData *GetData( u32 index ) const;

  /**
   *  @brief  衝突用コリジョン情報の取得
   */
  const void *GetCollisionData( const FieldItemData &data ) const;


  /**
   *  @brief スクリプトIDの取得
   */
  static u32 GetScriptId( const FieldItemData& data );

  /** 
   *  @brief shapeTypeが隠しアイテムかチェック
   */
  static b32 IsHiddenShapeType( u32 shapeType );

private:
  const FieldItemDataHeader *m_pData;

  static const u32 SCRIPT_ID_LIST_MAX = 21;
  static const u32 sc_SCRIPT_ID_LIST[SCRIPT_ID_LIST_MAX];

}; // class FieldItemAccessor

GFL_NAMESPACE_END(Field)

#endif // !defined(__FIELDITEMACCESSOR_H__)
