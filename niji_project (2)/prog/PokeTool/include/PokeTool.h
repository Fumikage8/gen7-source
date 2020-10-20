//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_tool.h
 *	@brief  ポケモンパラムプロジェクト側処理
 *	@author	Toru=Nagihashi
 *	@date		2012.06.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __POKE_TOOL_H__)
#define __POKE_TOOL_H__

#include <pml/include/pmlib.h>

namespace Savedata { class ZukanData; }
namespace PokeTool { class ExtendData; }

GFL_NAMESPACE_BEGIN(PokeTool)

/**
 * @brief 描画用ポケモンパラメータ
 * @note  この構造体はポケモンの情報から
 *        描画に関わるものを抜き出したものです。
 *
 *        この構造体をPokeModelやPokeIconなどの
 *        ポケモンに関わる描画関数に渡す用に存在しています。
 *        
 *        以下理由により作成しました。
 *
 *        １．CoreParamはMonsNoの読み替えができないので、
 *            複数のリソースを読むときにnew,deleteが発生してしまう。
 *            
 *        ２．BWまでのようにMonsNoやFormNoなどを１つ１つ渡すと、
 *            新たなデータが必要になったときに置換が大変である。
 *            また、取得する数が多くコストがかかる。
 *            
 *        また、SimpleParamはプロジェクトごとに異なる可能性があるため
 *        pmlibには追加しません。
 */
struct SimpleParam
{
  MonsNo monsNo;
  pml::FormNo formNo;
  pml::Sex sex;
  bool isRare;
  bool isEgg;
  u32 perRand;    //パッチール用個性乱数

  SimpleParam() : 
    monsNo(MONSNO_NULL), formNo(0), sex(pml::SEX_MALE), 
    isRare(false), isEgg(false), perRand(0)
  {}

  SimpleParam( MonsNo monsno, pml::FormNo formNo, pml::Sex sex, bool isRare, bool isEgg, u32 perRand ) : 
    monsNo(monsno), formNo(formNo), sex(sex), 
    isRare(isRare), isEgg(isEgg), perRand(perRand)
  {}
};


/**
 * @brief SimpleParamをCoreParamから取得する
 *
 * @param param SimpleParam受け取り
 * @param core_param ポケモンパラム
 */
extern void GetSimpleParam( SimpleParam *param, const pml::pokepara::CoreParam *core_param );

/**
 * @brief 描画用ポケモンパラメータを比較する
 *
 * @param a 比較A
 * @param b 比較B
 *
 * @return trueならば同じ falseならば異なる
 */
extern bool CompareSimpleParam( const SimpleParam &a, const SimpleParam &b );

/**
 * @brief 演出用の重さカテゴリの取得。(ひこうタイプ・特性ふゆう・ふうせん持ちはNONE)
 *
 * @param core_param ポケモンパラム
 */
typedef enum
{
  POKE_EFF_WEIGHT_HEAVY,        //200kg以上
  POKE_EFF_WEIGHT_LIGHT_HEAVY,  //100kg以上
  POKE_EFF_WEIGHT_MIDDLE,       //50kg以上
  POKE_EFF_WEIGHT_LIGHT,        //50kg未満
  POKE_EFF_WEIGHT_NONE,         //ひこうタイプ・特性ふゆう・ふうせん持ち
}PokeEffWeight;
extern PokeEffWeight CheckPokemonEffectWeight( const pml::pokepara::CoreParam *core_param );


/**
 * @brief カロス地方で捕まえたポケモンかどうか
 *        （ステータスやBOXなどでのカロス地方捕獲マークにて使用）
 *
 * @param core_param  コアパラム
 * 
 */
//IsCalos無効になっているので消しておく
//extern bool IsCalosCaptured( const pml::pokepara::CoreParam *core_param );


/**
 * @brief 表示する性別を取得
 *
 * @param core_param  コアパラム
 * 
 * @retval "pml::SEX_MALE = ♂を表示"
 * @retval "pml::SEX_FEMALE = ♀を表示"
 * @retval "pml::SEX_UNNON = 性別を表示しない"
 */
extern pml::Sex GetDrawSex( const pml::pokepara::CoreParam * core_param );

/** 
 * @brief CoreParamで判定できない時用の性別表示判定関数
 *
 * @param mons         ポケモンNO
 * @param sex          性別
 * @param haveNickName ニックネームフラグ
 * @param isEgg        タマゴフラグ（CHECK_BOTH_EGGで渡す事）
 *
 * @retval "pml::SEX_MALE = ♂を表示"
 * @retval "pml::SEX_FEMALE = ♀を表示"
 * @retval "pml::SEX_UNNON = 性別を表示しない"
 */
extern pml::Sex GetDrawSex( MonsNo mons, pml::Sex sex, bool haveNickName, bool isEgg );


/**
 * @brief 表示する図鑑マークを取得
 *
 * @param core_param ポケモン
 * @param zukan 図鑑セーブデータ
 * @param extend  拡張データ
 *
 * @return ZukanMarkDisplay列挙
 */
enum DrawZukanMark
{
  ZUKAN_MARK_LOCAL_A, //マークAを表示すべき
  ZUKAN_MARK_LOCAL_B, //マークBを表示すべき
  ZUKAN_MARK_LOCAL_C, //マークCを表示すべき
  ZUKAN_MARK_INVISIBLE, //マークを表示しない
};
/*
誰も使っていないのでコメントアウト
extern DrawZukanMark GetDrawZukanMark( const pml::pokepara::CoreParam * core_param, const Savedata::ZukanData *zukan, const PokeTool::ExtendData *extend );
*/

/**
 * @brief 表示する図鑑番号を取得(全国図鑑番号orアローラ図鑑番号(nijiではアローラ図鑑番号しか返さない))
 *
 * @param core_param ポケモン
 * @param zukan 図鑑セーブデータ
 * @param extend  拡張データ
 * 
 * @return ZUKAN_NUMBER_UNKNOUWNならば???、それ以外ならばその数値を表示すべき
 */
static const s32 ZUKAN_NUMBER_UNKNOUWN  = -1; //？？？を表示すべき
extern s32 GetDrawZukanNumber( const pml::pokepara::CoreParam * core_param, const Savedata::ZukanData *zukan, const PokeTool::ExtendData *extend );

/**
 * @brief ポケモンをアプリで表示するとき可愛がりのモーションを使うか？
 *
 * @param core_param ポケモン
 * 
 * @return bool trueなら可愛がりモーションを使う
 */
extern bool CheckUsingKawaigariInApp( const pml::pokepara::CoreParam * core_param );
extern bool CheckUsingKawaigariInApp( MonsNo monsNo , pml::FormNo formNo );

/**
 * @brief Miiversse投稿可能なポケモンか？
 *
 * @param core_param ポケモン
 * 
 * @return bool falseだったらMiiverse投稿をふさぐ
 */
extern bool CheckCanPostMiiverse( const pml::PokeParty *party );
extern bool CheckCanPostMiiverse( const pml::pokepara::CoreParam *core_param );
extern bool CheckCanPostMiiverse( MonsNo monsNo , pml::FormNo formNo );

/**
 * @func    GetDrawID
 * @brief   表示用ID取得
 * @date    2015.11.26
 *
 * @param   cp    CoreParam
 * @param   id    ID格納場所
 *
 * @retval  true  = アローラのポケモン   6桁表示
 * @retval  false = それ以外             5桁表示
 */
extern bool GetDrawID( const pml::pokepara::CoreParam * cp, u32 * id );


GFL_NAMESPACE_END(PokeTool)
#endif		// __POKE_TOOL_H__
