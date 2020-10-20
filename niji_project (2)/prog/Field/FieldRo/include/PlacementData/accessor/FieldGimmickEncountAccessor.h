//======================================================================
/**
 * @file FieldGimmickEncountAccessor.h
 * @date 2015/08/19 21:47:14
 * @author saita_kazuki
 * @brief ギミックエンカウント配置情報アクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_ACCESSOR_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"

GFL_NAMESPACE_BEGIN(Field)

class FieldGimmickEncountAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldGimmickEncountAccessor );

public:

  // 配置情報
  struct Data
  {
    u32 type;                   // 識別子
    f32 position[3];            // 座標
    f32 quaternion[4];          // 姿勢
    u32 flagwork;               // フラグワークID
    u32 flagwork_num;           // ワーク値
    u32 eventID;                // イベントID
    u32 presetID;               // プリセットID = ギミックエンカウントテーブルのID
    u32 modelID;                // モデル/キャラクタID
    u32 encountDataID;          // エンカウントデータID
    u32 encountCollisionOffset; // エンカウント開始コリジョンへのオフセット 必ず1つ存在する
    u32 actionCollisionOffset;  // アクション実行コリジョンへのオフセット 巡回型は存在しないのでNULL
    u32 pathCoreOffset;         // パスデータへのオフセット 待機型、接近型は存在しないのでNULL
    u32 actionCollisionOffset2; // アクション実行コリジョンへのオフセット 逃走型以外はNULL
    u32 pathCoreOffset2;        // パスデータへのオフセット 逃走型以外はNULL
  };

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 配置情報（Zone単位）
  struct Header
  {
    u32    num;
    Data   data[];
  };

GFL_WARNING_WIN32_POP

public:

  /**
   *  @brief  コンストラクタ
   */
  FieldGimmickEncountAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldGimmickEncountAccessor( void );

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
  const Data *GetData( u32 index ) const;

  /**
   *  @brief  エンカウント開始コリジョン情報の取得
   */
  const void *GetEncountCollisionData( const Data &data ) const;

  /**
   *  @brief  アクション実行コリジョンの取得
   */
  const void *GetActionCollisionData( const Data &data ) const;

  /**
   *  @brief  アクション実行コリジョンの取得(2つめ)
   */
  const void *GetActionCollisionData2( const Data &data ) const;

  /**
   *  @brief  パスデータの取得
   */
  const Traffic::PathCoreData *GetPathCoreData( const Data &data ) const;

  /**
   *  @brief  パスデータの取得(2つめ)
   */
  const Traffic::PathCoreData *GetPathCoreData2( const Data &data ) const;

private:

  const Header *m_pData;
};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_GIMMICK_ENCOUNT_ACCESSOR_H_INCLUDED__
