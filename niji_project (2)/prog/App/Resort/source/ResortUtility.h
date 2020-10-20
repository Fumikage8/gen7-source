#if !defined( __RESORT_UTILITY_H__ )
#define __RESORT_UTILITY_H__
#pragma once
//======================================================================
/**
 * @file    ResortUtility.h
 * @date    2015/10/08 11:17:00
 * @author  fukushima_yuya
 * @brief   ポケリゾート：便利関数など
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/Resort/source/ResortConst.h>

#include <PokeTool/include/PokeTool.h>

// pml
#include <pml/include/pmlib.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
class DrawerBase;
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( ui )
class UIView;
GFL_NAMESPACE_END( ui )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN(Savedata)
class BoxPokemon;
GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )
GFL_NAMESPACE_BEGIN( util )

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンペインデータのセット
 *
 * @param   base      [in]描画共通処理
 * @param   paneData  [out]ペインデータ
 * @param   paneTbl   [in]ペインインデックス配列
 */
//------------------------------------------------------------------------------
extern void SetPokePaneData( DrawerBase* base, PokePaneData* paneData, const u32* paneTbl );

//------------------------------------------------------------------------------
/**
 * @brief   描画用ポケモンパラメータのセット
 *
 * @param   sp  [out]PokeTool::SimpleParam
 * @param   lv  [out]レベル値
 * @param   cp  [in]pml::pokepara::CoreParam
 *
 * @return  "true" == 設定成功
 * @return  "false" == それ以外
 */
//------------------------------------------------------------------------------
extern bool SetSimpleParam( PokeTool::SimpleParam* sp, u32* lv, pml::pokepara::CoreParam* cp );

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメ数の取得
 *
 * @param   resortSave    リゾートセーブデータ
 *
 * @return  ノーマルランクのポケマメに換算した数
 */
//------------------------------------------------------------------------------
extern u32 GetAllBeansCount( Savedata::ResortSave* resortSave );

//------------------------------------------------------------------------------
/**
 * @brief   ランクごとのポケマメ数の取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   dest        [out]格納先の配列
 *
 * @return  none
 */
//------------------------------------------------------------------------------
extern void GetRankBeansCount( Savedata::ResortSave* resortSave, u32* dest );

//------------------------------------------------------------------------------
/**
 * @brief   時間表示
 *
 * @param   view      UIViewクラス
 * @param   wordSet   WordSetクラス
 * @param   textBox   時間を表示するテキストボックス
 * @param   time      時間
 *
 * @return  none
 */
//------------------------------------------------------------------------------
extern void SetTextBoxTime( app::ui::UIView* view, print::WordSet* wordSet, gfl2::lyt::LytTextBox* textBox, TimeParam time );

//------------------------------------------------------------------------------
/**
 * @brief   Ａ／Ｂ表示
 *
 * @param   view          UIViewクラス
 * @param   wordSet       WordSetクラス
 * @param   textBox       時間を表示するテキストボックス
 * @param   numerator     分子(Ａ)
 * @param   denominator   分母(Ｂ)
 *
 * @return  none
 */
//------------------------------------------------------------------------------
extern void SetTextBoxFraction( app::ui::UIView* view, print::WordSet* wordSet, gfl2::lyt::LytTextBox* textBox, u8 numerator, u8 denominator );

//------------------------------------------------------------------------------
/**
 * @brief   リゾートのレベルを取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   resortID    [in]リゾートID
 *
 * @return  リゾートのレベル
 */
//------------------------------------------------------------------------------
extern u8 GetResortLevel( Savedata::ResortSave* resortSave, u8 resortID );

//------------------------------------------------------------------------------
/**
 * @brief   開放済みのリゾート数を取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 *
 * @return  開放済みのリゾート数
 */
//------------------------------------------------------------------------------
extern u8 GetDevelopedResortCount( Savedata::ResortSave* resortSave );

//------------------------------------------------------------------------------
/**
 * @brief   タマゴかどうか
 *
 * @param   cp    CoreParam
 *
 * @return  "true  = タマゴ"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
extern bool IsEgg( const pml::pokepara::CoreParam* cp );

//------------------------------------------------------------------------------
/**
 * @brief   表示する性別を取得
 *
 * @param   cp    CoreParam
 * 
 * @retval  "pml::SEX_MALE = ♂を表示"
 * @retval  "pml::SEX_FEMALE = ♀を表示"
 * @retval  "pml::SEX_UNNON = 性別を表示しない"
 */
//------------------------------------------------------------------------------
extern pml::Sex GetSex( const pml::pokepara::CoreParam* cp );

//------------------------------------------------------------------------------
/**
 * @brief   ボーナスタイムの取得
 *
 * @param   resortSave  [in]リゾートセーブデータ
 * @param   resortID    [in]リゾートID
 * @param   time        [out]時間
 *
 * @return  ボーナスタイム
 */
//------------------------------------------------------------------------------
extern void GetBonusTime( Savedata::ResortSave* resortSave, u32 resortID, TimeParam* time );

//------------------------------------------------------------------------------
/**
 * @brief   ボックスのポケモンの数を取得
 *
 * @param   boxPokemon    ボックスセーブデータ
 * @param   appHeap       アプリケーションヒープ
 *
 * @return  ボックスのポケモンの数
 */
//------------------------------------------------------------------------------
extern u32 GetBoxPokeNum( Savedata::BoxPokemon* boxPoke, app::util::Heap* appHeap );

//------------------------------------------------------------------------------
/**
 * @brief   ボックスのポケモンの数を取得
 *
 * @param   resortSave    リゾートセーブデータ
 * @param   boxPokemon    ボックスセーブデータ
 * @param   appHeap       アプリケーションヒープ
 *
 * @return  ボックスのポケモンの数
 */
//------------------------------------------------------------------------------
extern u32 GetBoxPokeAndResortPokeNum( Savedata::ResortSave* resortSave, Savedata::BoxPokemon* boxPoke, app::util::Heap* appHeap );

//------------------------------------------------------------------------------
/**
 * @brief   ポケマメの取得反映
 *
 * @param   resortSave  リゾートセーブデータ
 * @param   beansRank   マメの種類
 * @param   addNum      追加数
 */
//------------------------------------------------------------------------------
extern void AddPokeMame( Savedata::ResortSave* resortSave, Savedata::ResortSave::PokeBeansRank beansRank, u32 addNum );

GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_UTILITY_H__