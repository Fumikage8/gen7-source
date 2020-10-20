//==============================================================================
/**
 * @file    poke_tool_Mega.cpp
 * @brief   メガ進化関連ユーティリティ
 * @date    2013.01.26
 * @author  tamada
 *          2015.01.13 niji移植ariizumi
 */
//==============================================================================
#if !defined( __POKE_TOOL_MEGA_H__ )
#define __POKE_TOOL_MEGA_H__

#include <macro/include/gfl2_Macros.h>
#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN(PokeTool)
GFL_NAMESPACE_BEGIN(Mega)

//------------------------------------------------------------------------
/**
 * @brief メガ進化を見たか？フラグ列のサイズ（バイト単位）
 */
//------------------------------------------------------------------------
enum { MEGA_SEE_FLAG_BYTES = 8 };

//------------------------------------------------------------------------
/**
 * @brief メガ進化のインデックス番号を取得
 * @param[in]   monsno    対象のポケモンナンバー
 * @param[in]   formno    対象のフォルムナンバー
 * @return[out] int       対象のインデックス(-1で無効)
 */
//------------------------------------------------------------------------
extern int GetMegaIndex( MonsNo monsno, u8 formno );
extern int GetMegaIndex( u16 itemno );
extern int GetMegaIndexItem( MonsNo monsno, u16 itemno );

//nijiではLong分岐は無いので不要
#if 0
//------------------------------------------------------------------------
/**
 * @brief       「メガ進化を見た」フラグをセット
 * @param[in]   flagarea  メガ進化見たフラグ列へのポインタ
 * @param[in]   monsno    対象のポケモンナンバー
 * @param[in]   formno    対象のフォルムナンバー
 */
//------------------------------------------------------------------------
extern void SetSeeFlag( u8 * flagarea, MonsNo monsno, u8 formno );
extern void SetSeeFlag( u8 * flagarea, u16 itemno );
extern void SetSeeFlagItem( u8 * flagarea, MonsNo monsno, u16 itemno );

//------------------------------------------------------------------------
/**
 * @brief       「メガ進化を見た」フラグを取得
 * @param[in]   flagarea  メガ進化見たフラグ列へのポインタ
 * @param[in]   monsno    対象のポケモンナンバー
 * @param[in]   formno    対象のフォルムナンバー
 * @retval      true      見たことがある
 * @retval      false     見たことがない
 */
//------------------------------------------------------------------------
extern bool GetSeeFlag( const u8 * flagarea, MonsNo monsno, u8 formno );
extern bool GetSeeFlag( const u8 * flagarea, u16 itemno );
extern bool GetSeeFlagItem( const u8 * flagarea, MonsNo monsno, u16 itemno );

//------------------------------------------------------------------------
/**
 * @brief       「メガ進化を見た」全フラグを取得
 * @param[in]   flagarea  メガ進化見たフラグ列へのポインタ
 * @retval      true      一度でも見たことがある
 * @retval      false     一度も見たことがない
 */
//------------------------------------------------------------------------
extern bool IsAlreadySeen( const u8 * flagarea );
#endif

//------------------------------------------------------------------------
/**
 * @brief メガ進化状態のフォルムをノーマルに戻す
 * @param my_party  手持ちポケモンパーティー
 */
//------------------------------------------------------------------------
extern void NormalizeMegaForm( pml::PokeParty * my_party );

//------------------------------------------------------------------------
/**
 * @brief   メガ進化状態かどうかをチェック
 * @param   poke
 * @retval  true  メガ進化フォルムの状態
 * @retval  false メガ進化フォルムでない
 */
//------------------------------------------------------------------------
extern bool IsMegaForm( pml::pokepara::CoreParam * poke );

GFL_NAMESPACE_END(Mega)
GFL_NAMESPACE_END(PokeTool)

#endif  // __POKE_TOOL_MEGA_H__

