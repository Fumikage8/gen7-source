//=============================================================================
/**
 * @file   btl_ServerCommandCalculator.h
 * @date   2015/09/03 15:15:25
 * @author obata_toshihiro
 * @brief  サーバーコマンドの計算
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_SERVERCOMMANDCALCULATOR_H_INCLUDED
#define BTL_SERVERCOMMANDCALCULATOR_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include "./btl_ServerFlow.h"

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class ServerCommandCalculator
   * @brief サーバーコマンドを計算する
   */
  class ServerCommandCalculator
  {
    GFL_FORBID_COPY_AND_ASSIGN( ServerCommandCalculator );

  public:

    /**
     * @brief コンストラクタ
     */
    ServerCommandCalculator( void );

    /**
     * @brief デストラクタ
     */
    virtual ~ServerCommandCalculator();


    //-------------------------------------------------------------------------
    // 初期ポケモン投入コマンド
    //-------------------------------------------------------------------------

    /**
     * @brief 初期ポケモン投入コマンド計算のための入力データ
     */
    struct Input_BattleInCommand
    {
      ServerFlow*  serverFlow;   // コマンド計算を行う ServerFlow
      const SCQUE* commandQueue; // serverFlow が計算したコマンドが格納されるコマンドキュー
    };

    /**
     * @brief 初期ポケモン投入コマンド計算の結果
     */
    struct Output_BattleInCommand
    {
      bool        isAnyEventOccured;   // コマンド計算時に、何らかのイベントが発生したか？
      const void* pCommandData;        // コマンドデータへのポインタ
      u32         commandDataSize;     // コマンドデータのサイズ
    };

    /**
     * @brief 初期ポケモン投入コマンドを計算する
     * @param[out] output  コマンド計算の結果
     * @param      input   コマンド計算のための入力データ
     */
    void CalcBattleInCommand( Output_BattleInCommand* output, const Input_BattleInCommand& input );


    //-------------------------------------------------------------------------
    // 行動選択後の一連のコマンド
    //
    // 必ず、
    // StartCalc_AfterActionSelect　→　UpdateCalc_AfterActionSelect　
    // の順に、セットで呼んでください。
    //-------------------------------------------------------------------------

    /**
     * @brief 行動選択後の一連のコマンド計算のための入力データ
     */
    struct Input_AfterActionSelect
    {
      ServerFlow*        serverFlow;   // コマンド計算を行う ServerFlow
      const SCQUE*       commandQueue; // serverFlow が計算したコマンドが格納されるコマンドキュー
      const SVCL_ACTION* clientAction; // 全クライアントの行動パラメータ
    };

    /**
     * @brief 行動選択後の一連のコマンド計算の結果
     */
    struct Output_AfterActionSelect
    {
      ServerFlow::Result flowResult;          // コマンド計算の結果
      const void*        pCommandData;        // コマンドデータへのポインタ
      u32                commandDataSize;     // コマンドデータのサイズ
    };

    /**
     * @brief 行動選択後の一連のコマンド計算を開始する
     * @param input  コマンド計算のための入力データ
     */
    void StartCalc_AfterActionSelect( const Input_AfterActionSelect& input );

    /**
     * @brief 行動選択後の一連のコマンド計算を更新する
     * @param[out] output  コマンド計算の結果
     * @param      input   コマンド計算のための入力データ
     */
    void UpdateCalc_AfterActionSelect( Output_AfterActionSelect* output, const Input_AfterActionSelect& input );


    //-------------------------------------------------------------------------
    // カバーポケモン投入後の一連のコマンド
    //
    // 必ず、
    // StartCalc_AfterCoverPokeIn　→　UpdateCalc_AfterCoverPokeIn　
    // の順に、セットで呼んでください。
    //-------------------------------------------------------------------------

    /**
     * @brief カバーポケモン投入後の一連のコマンド計算のための入力データ
     */
    struct Input_AfterCoverPokeIn
    {
      ServerFlow*        serverFlow;   // コマンド計算を行う ServerFlow
      const SCQUE*       commandQueue; // serverFlow が計算したコマンドが格納されるコマンドキュー
      const SVCL_ACTION* clientAction; // 全クライアントの行動パラメータ
    };

    /**
     * @brief カバーポケモン投入後の一連のコマンド計算の結果
     */
    struct Output_AfterCoverPokeIn
    {
      ServerFlow::Result flowResult;          // コマンド計算の結果
      const void*        pCommandData;        // コマンドデータへのポインタ
      u32                commandDataSize;     // コマンドデータのサイズ
    };

    /**
     * @brief カバーポケモン投入後の一連のコマンド計算を開始する
     * @param input  コマンド計算のための入力データ
     */
    void StartCalc_AfterCoverPokeIn( const Input_AfterCoverPokeIn& input );

    /**
     * @brief カバーポケモン投入後の一連のコマンド計算を更新する
     * @param[out] output  コマンド計算の結果
     * @param      input   コマンド計算のための入力データ
     */
    void UpdateCalc_AfterCoverPokeIn( Output_AfterCoverPokeIn* output, const Input_AfterCoverPokeIn& input );


    //-------------------------------------------------------------------------
    // ターン途中でのポケモン入れ替え後の一連のコマンド
    //
    // 必ず、
    // StartCalc_AfterIntervalPokeChange　→　UpdateCalc_AfterIntervalPokeChange　
    // の順に、セットで呼んでください。
    //-------------------------------------------------------------------------

    /**
     * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算のための入力データ
     */
    struct Input_AfterIntervalPokeChange
    {
      ServerFlow*        serverFlow;   // コマンド計算を行う ServerFlow
      const SCQUE*       commandQueue; // serverFlow が計算したコマンドが格納されるコマンドキュー
      const SVCL_ACTION* clientAction; // 全クライアントの行動パラメータ
    };

    /**
     * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算の結果
     */
    struct Output_AfterIntervalPokeChange
    {
      ServerFlow::Result flowResult;          // コマンド計算の結果
      const void*        pCommandData;        // コマンドデータへのポインタ
      u32                commandDataSize;     // コマンドデータのサイズ
    };

    /**
     * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算を開始する
     * @param input  コマンド計算のための入力データ
     */
    void StartCalc_AfterIntervalPokeChange( const Input_AfterIntervalPokeChange& input );

    /**
     * @brief ターン途中でのポケモン入れ替え後の一連のコマンド計算を更新する
     * @param[out] output  コマンド計算の結果
     * @param      input   コマンド計算のための入力データ
     */
    void UpdateCalc_AfterIntervalPokeChange( Output_AfterIntervalPokeChange* output, const Input_AfterIntervalPokeChange& input );



    //-------------------------------------------------------------------------
    // 乱入ポケモン入場コマンド
    //
    // 必ず、
    // StartCalc_IntrudePokeIn　→　UpdateCalc_IntrudePokeIn　
    // の順に、セットで呼んでください。
    //-------------------------------------------------------------------------

    /**
     * @brief 乱入ポケモン入場コマンド計算のための入力データ
     */
    struct Input_IntrudePokeIn
    {
      ServerFlow*   serverFlow;    // コマンド計算を行う ServerFlow
      const SCQUE*  commandQueue;  // serverFlow が計算したコマンドが格納されるコマンドキュー
    };

    /**
     * @brief 乱入ポケモン入場コマンド計算の結果
     */
    struct Output_IntrudePokeIn
    {
      const void*  pCommandData;     // コマンドデータへのポインタ
      u32          commandDataSize;  // コマンドデータのサイズ
    };

    /**
     * @brief 乱入ポケモン入場コマンドを計算する
     * @param[out] output  コマンド計算の結果
     * @param      input   コマンド計算のための入力データ
     */
    void CalcIntrudePokeInCommand( Output_IntrudePokeIn* output, const Input_IntrudePokeIn& input );





  private:

    bool m_isCommandCalculating_AfterActionSelect;
    bool m_isCommandCalculating_AfterCoverPokeIn;
    bool m_isCommandCalculating_AfterIntervalPokeChange;

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_SERVERCOMMANDCALCULATOR_H_INCLUDED