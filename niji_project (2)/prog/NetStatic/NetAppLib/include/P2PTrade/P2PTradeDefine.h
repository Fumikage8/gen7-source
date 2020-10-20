// ============================================================================
/*
 * @file P2PTradeDefine.h
 * @brief P2P交換アプリの定義ファイルです。
 * @date 2015.07.24
 * @author Akira endo
 */
// ============================================================================
#if !defined( P2PTRADEDEFINE_H_INCLUDE )
#define P2PTRADEDEFINE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(P2PTrade)


enum MessageID
{
  MESSAGE_ID_INVALID = - 1,

  MESSAGE_ID_NICE_TO_MEET_YOU,          //! よろしくおねがいします
  MESSAGE_ID_THANK_YOU,                 //! ありがとう
  MESSAGE_ID_GOODBYE,                   //! さようなら
  MESSAGE_ID_SORRY,                     //! すみません
  MESSAGE_ID_POKEMON_REQUEST,           //! いま見せている○○が欲しいです
  MESSAGE_ID_OTHER_POKEMON_REQUEST,     //! 他のポケモンを見たいです
  MESSAGE_ID_DO_NOT_HAVE_POKEMON,       //! もうポケモンがいません
  MESSAGE_ID_WANT_TO_SEE_POKEMON,       //! ポケモンを見せてください
  MESSAGE_ID_SEE_STATE_POKEMON,         //! ポケモンの様子を見ています
  MESSAGE_ID_TRADE_POKEMON              //! ○○を交換に出します
};


GFL_NAMESPACE_END(P2PTrade)
GFL_NAMESPACE_END(NetAppLib)

#endif // P2PTRADEDEFINE_H_INCLUDE
