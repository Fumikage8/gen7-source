//======================================================================
/**
 * @file FieldGimmickEncountDataAccessor.h
 * @date 2015/08/20 12:19:22
 * @author saita_kazuki
 * @brief ギミックエンカウントバイナリデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_GIMMICK_ENCOUNT_DATA_ACCESSOR_H_INCLUDED__
#define __FIELD_GIMMICK_ENCOUNT_DATA_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountData.h"
#include "Field/FieldRo/include/GimmickEncount/FieldGimmickEncountDefine.h"

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( GimmickEncount )

/**
 * @class ギミックエンカウントバイナリデータアクセサー
 */
class DataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( DataAccessor );

public:

  /**
   * @brief コンストラクタ
   */
  DataAccessor();
  DataAccessor( void* pAllPackData);

  /**
   * @brief デストラクタ
   */
  virtual ~DataAccessor();

  /**
   *  @brief パックデータセット
   */
  void SetData( void* pAllPackData);

  /**
   * @brief データ取得
   * @param presetID プリセットID (テーブルID)
   * @return 指定したデータバイナリ。FieldGimmickEncountData.hに定義してあるフォーマットに入れて使う
   * @note 不正なID指定をした場合はNULL
   */
  void* GetData( u32 presetID) const ;

  /**
   * @brief ポケモンデータ取得
   * @param monsNo モンスターNo
   * @return 指定したMonsNoのデータ
   * @note 見つからない場合はNULL
   */
  const PokeData* GetPokeData( u16 monsNo) const ;

  /**
   * @brief ギミックエンカウントの種類(型)取得
   * @param presetID プリセットID (テーブルID)
   * @return ギミックエンカウントの種類(型)取得
   */
  GimmickType GetGimmickType( u32 presetID) const ;

  /**
   * @brief ギミックエンカウント用シンボルエンカウントIDオフセット値の取得
   * @return ギミックエンカウント用シンボルエンカウントIDオフセット値
   * @note ここで返ってくる値以上がエンカウントIDに設定されていたら、そのエンカウントIDはシンボルエンカウントID
   */
  u32 GetSymbolEncountIDOffset() const ;

private:

  void initializeAccessor( void* pAllPackData);

private:

  static const u32 GIMMICK_ENCOUNT_TABLE_NUM = 4;   ///< ギミックエンカウントテーブルの数

  /**
   * @brief プリセットIDオフセット情報
   */
  struct PresetIDOffset
  {
    u32   startIndex;   ///< 開始インデックス
    u32   endIndex;     ///< 終了インデックス
  };

  /**
   * @brief ヘッダー情報
   */
  struct HeaderData
  {
    PresetIDOffset    presetIDOffset[ GIMMICK_ENCOUNT_TABLE_NUM ];  ///< プリセットIDオフセット
    u32               standbyNum;                                   ///< 待機型の数
    u32               chaseNum;                                     ///< 接近型の数
    u32               patrolNum;                                    ///< 巡回型の数
    u32               escapeNum;                                    ///< 逃走型の数
    u32               pokeNum;                                      ///< ポケモンの数
    u32               symbolEncountIDOfset;                         ///< ギミックエンカウント用シンボルエンカウントIDのオフセット (この値以上がシンボルエンカウントID)
  };

private:

  /**
   * @brief バイナリパックインデックス
   */
  enum DataIndex
  {
    DATA_INDEX_HEADER,    ///< ヘッダーデータ
    DATA_INDEX_STANDBY,   ///< 待機型データリスト
    DATA_INDEX_CHASE,     ///< 接近型データリスト
    DATA_INDEX_PATROL,    ///< 巡回型データリスト
    DATA_INDEX_ESCAPE,    ///< 逃走型データリスト
    DATA_INDEX_POKE,      ///< ポケモンデータリスト

    DATA_INDEX_MAX,
  };

  void*                       m_pAllPackData;   ///< 全てが含まれたパックデータ
  HeaderData*                 m_pHeaderData;    ///< ヘッダーデータ
  StandbyData*                m_pStandbyData;   ///< 待機型データリスト
  ChaseData*                  m_pChaseData;     ///< 接近型データリスト
  PatrolData*                 m_pPatrolData;    ///< 巡回型データリスト
  EscapeData*                 m_pEscapeData;    ///< 逃走型データリスト
  PokeData*                   m_pPokeData;      ///< ポケモンデータリスト

};

GFL_NAMESPACE_END( GimmickEncount )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_GIMMICK_ENCOUNT_DATA_ACCESSOR_H_INCLUDED__
