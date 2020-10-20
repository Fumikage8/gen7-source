//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   poke_tool_PokeMemory.h
 *  @brief  ポケモンの記憶、セット・取得クラス
 *  @author Toru=Nagihashi
 *  @date   2012.08.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#if !defined( __POKE_TOOL_POKEMEMORY_H__)
#define __POKE_TOOL_POKEMEMORY_H__

#include "GameSys/include/GameData.h"
#include "Print/include/WordSet.h"

namespace pml { namespace pokepara { class CoreParam; } }
namespace Savedata { class MyStatus; }
namespace gfl2 { namespace str { class StrBuf; } }
namespace Savedata { class BattleBox; }

namespace PokeTool {

class PokeMemory
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeMemory);
public:
#if 0 //@attention DEPEND_ON_OLD_MODULE
  //ヘッダ先のenumにnamespaceをいれてやる
  #include <assets/app/pokemon_memory/include/pokemon_memory.h>
#else
  enum Event {
    EVENT_NONE,
    EVENT_MEET_BY_TRADE,
    EVENT_MEET_BY_EVENT,
  };
  typedef void WordSet;
#endif
public:
  static const int MEMORY_LEVEL_MAX = 7;  //レベル最大
public:
  //---------------------------------------------------------------------------------
  // 設定
  //---------------------------------------------------------------------------------
  /**
   * @brief 記憶をセット
   *
   * @param event イベント番号(pokemon_memory,hにあるenum Event)
   * @param data  データ（イベント番号によっていれるものが異なる）
   * @param core_param  セットするポケモン
   * @param gamedata  簡易に使うためにゲームデータを渡している(MyStatusとZoneDataが主)  NULLだと内部でシングルトン取得する
   * @param force 強制セットフラグ
   */
  static void Set( Event event, int data, pml::pokepara::CoreParam * core_param, GameSys::GameData *gamedata = NULL, bool force = false, bool dont_data_modify = false );
  /**
   * @brief 記憶をセット（PokeParty版）
   *
   * @param event イベント番号(pokemon_memory,hにあるenum Event)
   * @param data  データ（イベント番号によっていれるものが異なる）
   * @param pokeparty ポケモンパーティ版
   * @param gamedata  簡易に使うためにゲームデータを渡している(MyStatusとZoneDataが主) NULLだと内部でシングルトン取得する
   * @param force 強制セットフラグ
   */
  static void Set( Event event, int data, pml::PokeParty * pokeparty, GameSys::GameData *gamedata = NULL, bool force = false );
  /**
   * @brief 記憶をセット（BattleBox版）
   *
   * @param event イベント番号(pokemon_memory,hにあるenum Event)
   * @param data  データ（イベント番号によっていれるものが異なる）
   * @param battlebox バトルBOX版
   * @param temporary_heap テンポラリ用ヒープ（内部でLowerHandleで使います）
   * @param gamedata  簡易に使うためにゲームデータを渡している(MyStatusとZoneDataが主) NULLだと内部でシングルトン取得する
   * @param force 強制セットフラグ基本falseです
   */
  static void Set( Event event, int data, Savedata::BattleBox * battlebox, gfl2::heap::HeapBase * temporary_heap, GameSys::GameData *gamedata = NULL, bool force = false );
  //---------------------------------------------------------------------------------
  // 取得
  //---------------------------------------------------------------------------------
  /**
   * @brief 親以外とのポケモンの記憶があるかチェック
   *
   * @param core_param  ポケモン
   *
   * @return trueある falseない
   */
  static bool IsExistOtherMemory( const pml::pokepara::CoreParam * core_param );

  /**
   * @brief 親との記憶  メッセージIDをセット
   *
   * @param core_param  ポケモン
   *
   * @return メッセージID
   */
  static u32 GetParentMemoryMessageID( const pml::pokepara::CoreParam * core_param );
  /**
   * @brief 親との記憶  メッセージに単語登録
   *
   * @param wordset 単語登録
   * @param core_param  ポケモン
   * @param heap  ヒープ
   * @param msg_data  メッセージデータ
   */
  static void WordSetParentMemory( WordSet * wordset, const pml::pokepara::CoreParam * core_param, gfl2::heap::HeapBase *heap, gfl2::str::MsgData * msg_data );
  
  /**
   * @brief 親以外との記憶  メッセージIDをセット
   *
   * @param core_param  ポケモン
   *
   * @return  メッセージID
   */
  static u32 GetOtherMemoryMessageID( const pml::pokepara::CoreParam * core_param );
  /**
   * @brief 親以外との記憶  メッセージに単語登録
   *
   * @param wordset 単語登録
   * @param core_param  ポケモン
   * @param mystatus  マイステータス
   * @param heap  ヒープ
   * @param msg_data  メッセージデータ
   */
  static void WordSetOtherMemory( WordSet * wordset, const pml::pokepara::CoreParam * core_param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *heap, gfl2::str::MsgData * msg_data );

  /**
   * @brief 親との記憶文字列を取得する
   * @param msg_data  メッセージデータ
   * @param core_param  記憶を取得するポケモン
   * @param heap  テンポラリ用ヒープ(内部でLowerHandle使用)
   * @param strbuf  メッセージ受け取り
   * 
   * @note 内部でmsgdata作成、wordset作成、wordset登録、expandをやっています。
   */
  static void GetParentMemoryStrings( const pml::pokepara::CoreParam * core_param, gfl2::heap::HeapBase *heap, gfl2::str::StrBuf *strbuf );
  /**
   * @brief 親以外との記憶文字列を取得する
   *
   * @param msg_data  メッセージデータ
   * @param core_param  記憶を取得するポケモン
   * @param mystatus 自分のステータス（単語登録用）
   * @param heap  テンポラリ用ヒープ(内部でLowerHandle使用)
   * @param strbuf  メッセージ受け取り
   * 
   * @note 内部でmsgdata作成、wordset作成、wordset登録、expandをやっています。
   */
  static void GetOtherMemoryStrings( const pml::pokepara::CoreParam * core_param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *heap, gfl2::str::StrBuf *strbuf );
private:
  static void SetCore( pml::pokepara::CoreParam * core_param, pml::pokepara::Memories code, pml::pokepara::Memories data, pml::pokepara::Memories feel, pml::pokepara::Memories level, Event new_event, int new_data, bool force, bool parent );
  static void GetStringsCore( const pml::pokepara::CoreParam * core_param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *heap, pml::pokepara::Memories code, pml::pokepara::Memories data, pml::pokepara::Memories feel, pml::pokepara::Memories level, gfl2::str::StrBuf *strbuf );
  static u32 GetMessageIdCore( const pml::pokepara::CoreParam * core_param, pml::pokepara::Memories code );
  static void WordSetCore( WordSet * wordset, const pml::pokepara::CoreParam * core_param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *heap, gfl2::str::MsgData * msg_data, pml::pokepara::Memories code_idx, pml::pokepara::Memories data_idx, pml::pokepara::Memories feel_idx, pml::pokepara::Memories level_idx );
private:
  static const int MEMORY_LEVEL_NONE  = 0;
#if PM_DEBUG
public:
  static int DEBUG_FORCE_FLAG;
#endif
};


}
#endif    // __POKE_TOOL_POKEMEMORY_H__
