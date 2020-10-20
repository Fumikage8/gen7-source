// ============================================================================
/*
 * @file GTSFrameResult.h
 * @brief GTSフレームのリザルトコードを定義します。
 * @date 2015.03.25
 */
// ============================================================================
#if !defined( GTSFRAMERESULT_H_INCLUDE )
#define GTSFRAMERESULT_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)


enum StateConfirmResultCode
{
  STATE_CONFIRM_RESULT_POKEMON_NOT_UPLOAD,            // ポケモンをアップロードしていない
  STATE_CONFIRM_RESULT_POKEMON_UPLOAD,                // ポケモンがアップロードされている(GTSに預けられている状態)
  STATE_CONFIRM_RESULT_POKEMON_TRADE_SUCCESS,         // ポケモンの交換が成功した
  STATE_CONFIRM_RESULT_POKEMON_RETURN,                // ポケモンが戻ってきた
  STATE_CONFIRM_RESULT_ERROR,                         // エラーが発生
};

enum TopMenuResultCode
{
  TOP_MENU_RESULT_BACK,                               // 戻る
  TOP_MENU_RESULT_UPLOAD_POKEMON,                     // ポケモンを預ける
  TOP_MENU_RESULT_SEARCH_POKEMON,                     // ポケモンを探す
  TOP_MENU_RESULT_DOWNLOAD_MY_POKEMON,                // (預けた)ポケモンを引き取る
};

enum SearchPokemonResultCode
{
  SEARCH_POKEMON_RESULT_BACK,                         // 戻る
  SEARCH_POKEMON_RESULT_SUCCESS,                      // ポケモンの検索に成功
  SEARCH_POKEMON_RESULT_ERROR,                        // エラーが発生
  SEARCH_POKEMON_RESULT_NAME_INPUT,                   // ポケモンの名前入力ボタンを押した
};

enum UploadPokemonResultCode
{
  UPLOAD_POKEMON_RESULT_BACK,                         // 戻る
  UPLOAD_POKEMON_RESULT_SUCCESS,                      // ポケモンのアップロードに成功
  UPLOAD_POKEMON_RESULT_ERROR,                        // エラーが発生
  UPLOAD_POKEMON_RESULT_NAME_INPUT,                   // ポケモンの名前入力ボタンを押した
};

enum DownloadMyPokemonResultCode
{
  DOWNLOAD_MY_POKEMON_RESULT_BACK,                    // 戻る
  DOWNLOAD_MY_POKEMON_RESULT_POKEMON_TRADE_SUCCESS,   // ポケモンの交換が成功した
  DOWNLOAD_MY_POKEMON_RESULT_POKEMON_RETURN,          // ポケモンが戻ってきた
  DOWNLOAD_MY_POKEMON_RESULT_ERROR,                   // エラーが発生
};

enum TradePersonSelectResultCode
{
  TRADE_PERSON_SELECT_RESULT_BACK,                    // 戻る
  TRADE_PERSON_SELECT_RESULT_SELECT,                  // 交換相手を選択した
  TRADE_PERSON_SELECT_RESULT_REFINE_SEARCH,           // 絞り込み検索ボタンを押した
  TRADE_PERSON_SELECT_RESULT_ERROR,                   // エラーが発生
};

enum TradePersonRefineSearchResultCode
{
  TRADE_PERSON_REFINE_SEARCH_RESULT_BACK,             // 戻る
};

enum TradePokemonResultCode
{
  TRADE_POKEMON_RESULT_ALREADY_TRADE_POKEMON,         // 既にポケモンが他の人と交換されていた
  TRADE_POKEMON_RESULT_SUCCESS,                       // ポケモンの交換に成功した
  TRADE_POKEMON_RESULT_ERROR,                         // エラーが発生した
};


enum NameInputResultCode
{
  NAME_INPUT_RESULT_TO_SEARCH_POKEMON,
  NAME_INPUT_RESULT_TO_UPLOAD_POKEMON
};


GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSFRAMERESULT_H_INCLUDE
