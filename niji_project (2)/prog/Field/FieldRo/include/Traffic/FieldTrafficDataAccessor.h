//======================================================================
/**
 * @file FieldTrafficDataAccessor.h
 * @date 2015/07/17 11:08:29
 * @author miyachi_soichi
 * @brief フィールド往来用データアクセッサ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_TRAFFIC_DATAACCESSOR_H_INCLUDED__
#define __FIELD_TRAFFIC_DATAACCESSOR_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
// Field
#include "Field/FieldRo/include/Traffic/FieldTraffic_define.h"

GFL_NAMESPACE_BEGIN(Field)

/**
 * @class TrafficDataAccessor
 * @brief 往来データアクセッサ
 */
class TrafficDataAccessor
{
    GFL_FORBID_COPY_AND_ASSIGN(TrafficDataAccessor);

public:
  /**
   *  @brief  コンストラクタ
   */
  TrafficDataAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~TrafficDataAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  パスデータ最大数の取得
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  パスデータを取得する
   */
  const Traffic::TrafficPathData *GetTrafficPathData( u32 idx ) const;

  /**
   *  @brief  パスコアデータを取得する
   */
  const Traffic::PathCoreData *GetPathCoreData( const Traffic::TrafficPathData &data ) const;

  /**
   *  @brief  NPCテーブルデータを取得する
   */
  const void *GetNpcCoreData( const Traffic::TrafficPathData &data ) const;

private:
  const Traffic::TrafficPathDataHeader *m_pData;

};

/**
 *  @class  TrafficNpcDataAccessor
 *  @brief  往来NPCデータアクセッサ
 */
class TrafficNpcDataAccessor
{
  // コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN(TrafficNpcDataAccessor);

public:
  /**
   *  @brief  コンストラクタ
   */
  TrafficNpcDataAccessor( void )
  : m_pData( NULL )
  {
  }

  /**
   *  @brief  デストラクタ
   */
  ~TrafficNpcDataAccessor( void ){}

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  総数取得
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  Npcデータを取得する
   */
  const Traffic::NpcCoreData *GetNpcCoreData( u32 idx ) const;

private:
  const Traffic::NpcCoreDataHeader *m_pData;

};

GFL_NAMESPACE_END(Field)

#endif // __FIELD_TRAFFIC_DATAACCESSOR_H_INCLUDED__