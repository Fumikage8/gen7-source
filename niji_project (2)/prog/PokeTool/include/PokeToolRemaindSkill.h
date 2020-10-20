//=============================================================================
/**
 * @file    PokeToolRemaindSkill.cpp
 * @brief   指定ポケモンが思い出せるかを取得する
 * @author  Toru=Nagihashi
 * @date    2012.07.27
 *
 * @author  Hiroyuki Nakamura
 * @date    2015.12.14
 * @note    sangoから移植
 */
//=============================================================================
#if !defined( POKE_TOOL_REMAINDSKILL_H_INCLUDED )
#define POKE_TOOL_REMAINDSKILL_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// pmlib
#include <pml/include/pmlib.h>


GFL_NAMESPACE_BEGIN(PokeTool)

class RemaindSkill
{
  GFL_FORBID_COPY_AND_ASSIGN(RemaindSkill);
public:
  //---------------------------------------------------------------------
  // コンストラクタ・デストラクタ
  //---------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param heap ヒープ
   * @param param ポケモン情報
   */
  RemaindSkill( gfl2::heap::HeapBase * heap, const pml::pokepara::CoreParam * param );

  /**
   * @brief   コンストラクタ（非同期読み込み用）
   *
   * @param   heap  ヒープ
   */
  RemaindSkill( gfl2::heap::HeapBase * heap );

  /**
   * @brief デストラクタ
   */
  virtual ~RemaindSkill();
  //---------------------------------------------------------------------
  // システム
  //---------------------------------------------------------------------
  /**
   * @brief リスト最大
   *
   * @return  リスト最大取得  0ならば覚えられない
   */
  u32 GetListMax( void ) const;
  /**
   * @brief インデックスから技番号取得
   *
   * @param index インデックス
   *
   * @return 技番号
   */
  WazaNo GetWazaNo( u32 index ) const;
  /**
   * @brief   データを読み込んでリストを生成
   *
   * @param   param   ポケモン情報
   * @param   heap    テンポラリヒープ
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  bool LoadRemaindWaza( const pml::pokepara::CoreParam * param, gfl2::heap::HeapBase * heap );
  
private:
  static const int REMAIND_WAZA_MAX_BUFFER  = 255;

private:
  void SetupRemaindWaza( const pml::pokepara::CoreParam * param );
  bool IsSame( WazaNo waza, WazaNo * tbl, u32 len );

private:
  u32 m_list_max;
  WazaNo m_list_data[ REMAIND_WAZA_MAX_BUFFER ];
  pml::personal::WazaOboeData * m_waza_oboe;
  u32 m_seq;
};

GFL_NAMESPACE_END(PokeTool)

#endif  // POKE_TOOL_REMAINDSKILL_H_INCLUDED
