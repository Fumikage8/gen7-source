/**
 * @file btl_BattleAiSystem.h
 * @brief 戦闘AIシステム
 * @author obata_toshihiro
 * @date 2015.04.02
 */

#ifndef BTL_BATTLE_AI_SYSTEM_H_INCLUDED
#define BTL_BATTLE_AI_SYSTEM_H_INCLUDED

#include "../btl_common.h"


GFL_NAMESPACE_BEGIN( btl )


/**
 * @class BattleAiSystem
 * @brief すべての戦闘AIが共有するデータを管理する
 */
class BattleAiSystem
{
  GFL_FORBID_COPY_AND_ASSIGN( BattleAiSystem );

public:

  enum {
    BASIC_RAND_RANGE = 256,    ///< AIスクリプトが使用する乱数幅
  };


public:

  //---------------------------------------------------------------------------
  /**
   * @brief システムを初期化する
   * @param parentHeap  戦闘AI用ヒープの親となるヒープ
   */
  //---------------------------------------------------------------------------
  static void InitSystem( HeapHandle parentHeap );

  //---------------------------------------------------------------------------
  /**
   * @brief システムを終了する
   */
  //---------------------------------------------------------------------------
  static void QuitSystem( void );



  //---------------------------------------------------------------------------
  /**
   * @brief AI用ヒープハンドルを取得する
   */
  //---------------------------------------------------------------------------
  static HeapHandle GetHeapHandle( void );

#if PM_DEBUG
  //---------------------------------------------------------------------------
  /**
   * @brief AI用ヒープをダンプする
   */
  //---------------------------------------------------------------------------
  static void DumpHeap( void );
#endif



  //---------------------------------------------------------------------------
  /**
   * @brief 「とくせい」がオープンになったことの通知を受け取る
   * @param pokeID  「とくせい」オープンになったポケモンID
   */
  //---------------------------------------------------------------------------
  static void NotifyTokuseiOpen( u8 pokeID );

  //---------------------------------------------------------------------------
  /**
   * @brief 「とくせい」がオープンになっているポケモンかどうか判定
   * @param pokeID   チェックするポケモンID
   */
  //---------------------------------------------------------------------------
  static bool IsTokuseiOpened( u8 pokeID );



  //---------------------------------------------------------------------------
  /**
   * @brief 共通乱数を設定する
   * @param randValue  共通乱数として設定する値
   * 
   * @note 共通乱数とは
   *       同一ターン中は値が変化しない変数値のこと。
   *       ダブルバトルで、２体のポケモンが協調した行動をとるため、などに使用される。
   */
  //---------------------------------------------------------------------------
  static void SetCommonRand( u32 randValue );

  //---------------------------------------------------------------------------
  /**
   * @brief 共通乱数を取得する
   */
  //---------------------------------------------------------------------------
  static u32 GetCommonRand( void );

  





private:
  static HeapHandle s_heapHandle;
  static bool       s_isTokuseiOpened[ BTL_POKEID_MAX ];
  static u32        s_commonRandValue;
};


GFL_NAMESPACE_END( btl )


#endif // BTL_BATTLE_AI_SYSTEM_H_INCLUDED