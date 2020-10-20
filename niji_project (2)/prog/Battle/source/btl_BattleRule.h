//=============================================================================
/**
 * @file   btl_BattleRule.h
 * @date   2015/07/22 17:51:00
 * @author obata_toshihiro
 * @brief  バトルルールに関する静的な定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#if !defined BTL_BATTLERULE_H_INCLUDED
#define BTL_BATTLERULE_H_INCLUDED
#pragma once
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_def.h>
#include <Battle/include/battle_SetupParam.h>

GFL_NAMESPACE_BEGIN( btl )


  /**
   * @class BattleRule
   * @brief バトルルールに関する静的な仕様を定義する
   */
  class BattleRule
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleRule );

  public:
    
    //-------------------------------------------------------------------------
    /**
     * @brief クライアントの数を取得する
     * @param rule       バトルのルール
     * @param multiMode  マルチモード
     */
    //-------------------------------------------------------------------------
    static u8 GetClientNum( BtlRule rule, BtlMultiMode multiMode );

    //-------------------------------------------------------------------------
    /**
     * @brief 敵クライアントの数を取得する
     * @param rule        バトルのルール
     * @param multiMode   マルチモード
     * @param myClientId  自分のクライアントのID
     */
    //-------------------------------------------------------------------------
    static u8 GetEnemyClientNum( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID myClientId );

    //-------------------------------------------------------------------------
    /**
     * @brief 有効なクライアントかどうか判定
     * @param rule       バトルのルール
     * @param multiMode  マルチモード
     * @param clientId   チェック対象クライアントのID
     */
    //-------------------------------------------------------------------------
    static bool IsClientExist( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId );

    //-------------------------------------------------------------------------
    /**
     * @brief クライアントが AI かどうかを取得する
     * @param rule       バトルのルール
     * @param commMode   通信モード
     * @param multiMode  マルチモード
     * @param clientId   チェック対象のクライアント
     * @retval true   クライアントは AI
     * @retval false  クライアントは AI でない
     */
    //-------------------------------------------------------------------------
    static bool IsClientAi( BtlRule rule, BtlCommMode commMode, BtlMultiMode multiMode, BTL_CLIENT_ID clientId );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定したクライアントが対戦相手同士かどうかを取得する
     * @param rule         バトルのルール
     * @param clientId_0   チェック対象クライアントのID
     * @param clientId_1   チェック対象クライアントのID
     * @retval true   クライアントは 対戦相手同士
     * @retval false  クライアントは 味方同士( あるいは両方自分自身 )
     */
    //-------------------------------------------------------------------------
    static bool IsOpponentClient( BtlRule rule, BTL_CLIENT_ID clientId_0, BTL_CLIENT_ID clientId_1 );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定クライアントの対戦相手クライアントのIDを取得する
     * @param rule           バトルのルール
     * @param multiMode      マルチモード
     * @param clientId       自クライアントのID
     * @param opponentIndex  対戦相手のインデックス
     */
    //-------------------------------------------------------------------------
    static BTL_CLIENT_ID GetOpponentClientId( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId, u8 opponentIndex );

    //-------------------------------------------------------------------------
    /**
     * @brief 厳密な勝敗判定を行うか？
     * @param rule        バトルのルール
     * @param competitor  対戦相手
     * @retval true   簡易的な勝敗判定を行う( 自陣営以外が全滅したら勝ち )
     * @retval false  厳密な勝敗判定を行う
     */
    //-------------------------------------------------------------------------
    static bool IsResultStrictJudge( BtlRule rule, BtlCompetitor competitor );

    //-------------------------------------------------------------------------
    /**
     * @brief 戦闘終了後の「お手入れ」が可能か？
     * @param commMode   通信モード
     * @param competitor  対戦相手
     * @retval true   戦闘後、「お手入れ( ポケパルレ )」への遷移が可能
     * @retval false  戦闘後、「お手入れ( ポケパルレ )」へ遷移してはいけない
     */
    //-------------------------------------------------------------------------
    static bool IsOteireEnable( BtlCommMode commMode, BtlCompetitor competitor );

    //-------------------------------------------------------------------------
    /**
     * @brief リモートクライアントとの通信が切断された際に、ダミークライアントへの切り替えを行うか？
     * @param rule  バトルのルール
     * @retval true   ダミークライアントに切り替える
     * @retval true   ダミークライアントに切り替えない( 通信エラーになる )
     */
    //-------------------------------------------------------------------------
    static bool IsDummyClientSwitchEnable( BtlRule rule );

    //-------------------------------------------------------------------------
    /**
     * @brief 決着が着いた後、そのターンの残りの戦闘処理をスキップするか？
     * @param rule  バトルのルール
     * @retval true   決着が着いた後は、残りの戦闘処理をスキップする
     * @retval false  決着が着いた後も、残りの戦闘処理を行う
     */
    //-------------------------------------------------------------------------
    static bool IsSkipBattleAfterShowdown( BtlRule rule );

    
    //-------------------------------------------------------------------------
    /**
     * @brief 乱入バトルが可能か？
     * @param rule        バトルのルール
     * @param competitor  対戦相手
     * @retval true    乱入できる
     * @retval false   乱入できない
     */
    //-------------------------------------------------------------------------
    static bool CanIntrudeBattle( BtlRule rule, BtlCompetitor competitor );



  private:
    static u8 GetClientNum_Double( BtlMultiMode multiMode );
    static bool IsClientAi_Single( BtlCommMode commMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Royal( BtlCommMode commMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Double( BtlCommMode commMode, BtlMultiMode multiMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Double_MultiMode_NONE( BtlCommMode commMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Double_MultiMode_PP_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Double_MultiMode_PA_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId );
    static bool IsClientAi_Double_MultiMode_P_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId );

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_BATTLERULE_H_INCLUDED