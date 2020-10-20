//=============================================================================
/**
 * @file   btl_BattleRule.cpp
 * @date   2015/07/22 17:51:00
 * @author obata_toshihiro
 * @brief  バトルルールに関する静的な定義
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <Debug/include/gfl2_Assert.h>
#include "./btl_PokemonPosition.h"
#include "./btl_BattleRule.h"

GFL_NAMESPACE_BEGIN( btl )


  //---------------------------------------------------------------------------
  /**
   * @brief クライアントの数を取得する
   * @param rule       バトルのルール
   * @param multiMode  マルチモード
   */
  //---------------------------------------------------------------------------
  u8 BattleRule::GetClientNum( BtlRule rule, BtlMultiMode multiMode )
  {
    switch( rule )
    {
    case BTL_RULE_SINGLE:
      return 2;

    case BTL_RULE_DOUBLE:
      return BattleRule::GetClientNum_Double( multiMode );

    case BTL_RULE_ROYAL:
      return BTL_CLIENT_ROYAL_NUM;

    case BTL_RULE_INTRUDE:
      return 2;
      
    default:
      GFL_ASSERT_STOP(0);
      return 0;
    }
  }

  /**
   * @brief クライアントの数を取得する( ダブルバトル )
   * @param multiMode  マルチモード
   */
  u8 BattleRule::GetClientNum_Double( BtlMultiMode multiMode )
  {
    switch( multiMode )
    {
    case BTL_MULTIMODE_NONE:   ///< 非マルチ
      return 2;

    case BTL_MULTIMODE_PP_PP:  ///< プレイヤー２人 vs プレイヤー２人（通信マルチ）
      return 4;

    case BTL_MULTIMODE_PP_AA:  ///< プレイヤー２人 vs AI２人（AI通信マルチ）
      return 4;

    case BTL_MULTIMODE_PA_AA:  ///< プレイヤー１人＆AI１人 vs AI２人（AIマルチ）
      return 4;

    case BTL_MULTIMODE_P_AA:   ///< プレイヤー１人 VS AI２人（AIタッグ）
      return 3;

    default:
      GFL_ASSERT(0);
      return 2;
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 敵クライアントの数を取得する
   * @param rule        バトルのルール
   * @param multiMode   マルチモード
   * @param myClientId  自分のクライアントのID
   */
  //-------------------------------------------------------------------------
  u8 BattleRule::GetEnemyClientNum( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID myClientId )
  {
    const BtlSide mySide = PokemonPosition::GetClientSide( rule, myClientId );

    u8 enemyClientNum = 0;
    for( u32 i=0; i<BTL_CLIENT_NUM; ++i )
    {
      BTL_CLIENT_ID clientId = static_cast<BTL_CLIENT_ID>( i );
      if( ( BattleRule::IsClientExist( rule, multiMode, clientId ) ) &&
          ( PokemonPosition::GetClientSide( rule, clientId ) != mySide ) )
      {
        ++enemyClientNum;
      }
    }

    return enemyClientNum;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 有効なクライアントかどうか判定
   * @param rule       バトルのルール
   * @param multiMode  マルチモード
   * @param clientId   チェック対象クライアントのID
   */
  //-------------------------------------------------------------------------
  bool BattleRule::IsClientExist( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId )
  {
    if( clientId < BTL_CLIENT_NUM )
    {
      for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
      {
        BTL_CLIENT_ID posCoverClientId = PokemonPosition::GetPosCoverClientId( rule, multiMode, static_cast<BtlPokePos>(pos) );
        if( posCoverClientId == clientId ) {
          return true;
        }
      }
    }
    return false;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief クライアントが AI かどうかを取得する
   * @param rule       バトルのルール
   * @param commMode   通信モード
   * @param multiMode  マルチモード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  //---------------------------------------------------------------------------
  bool BattleRule::IsClientAi( BtlRule rule, BtlCommMode commMode, BtlMultiMode multiMode, BTL_CLIENT_ID clientId )
  {
    switch( rule )
    {
    case BTL_RULE_SINGLE:
    case BTL_RULE_INTRUDE:
      return BattleRule::IsClientAi_Single( commMode, clientId );

    case BTL_RULE_DOUBLE:
      return BattleRule::IsClientAi_Double( commMode, multiMode, clientId );

    case BTL_RULE_ROYAL:
      return BattleRule::IsClientAi_Royal( commMode, clientId );
      
    default:
      GFL_ASSERT_STOP(0);
      return 0;
    }
  }

  /**
   * @brief クライアントが AI かどうかを取得する( シングルバトル )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Single( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    // 非通信用
    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ true,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ false,
    };

    // 通信対戦用
    static const bool IS_AI_CLIENT_COMM[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ false,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ false,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return ( commMode == BTL_COMM_NONE ) ? ( IS_AI_CLIENT[ clientId ] ) : ( IS_AI_CLIENT_COMM[ clientId ] );
  }

  /**
   * @brief クライアントが AI かどうかを取得する( バトルロイヤル )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Royal( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    // 非通信用
    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_ROYAL1 */ false,
      /* BTL_CLIENT_ROYAL2 */ true,
      /* BTL_CLIENT_ROYAL3 */ true,
      /* BTL_CLIENT_ROYAL4 */ true,
    };

    // 通信対戦用
    static const bool IS_AI_CLIENT_COMM[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_ROYAL1 */ false,
      /* BTL_CLIENT_ROYAL2 */ false,
      /* BTL_CLIENT_ROYAL3 */ false,
      /* BTL_CLIENT_ROYAL4 */ false,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return ( commMode == BTL_COMM_NONE ) ? ( IS_AI_CLIENT[ clientId ] ) : ( IS_AI_CLIENT_COMM[ clientId ] );
  }

  /**
   * @brief クライアントが AI かどうかを取得する( ダブルバトル )
   * @param commMode   通信モード
   * @param multiMode  マルチモード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Double( BtlCommMode commMode, BtlMultiMode multiMode, BTL_CLIENT_ID clientId )
  {
    switch( multiMode )
    {
    case BTL_MULTIMODE_NONE:   ///< 非マルチ
      return BattleRule::IsClientAi_Double_MultiMode_NONE( commMode, clientId );

    case BTL_MULTIMODE_PP_PP:  ///< プレイヤー２人 vs プレイヤー２人（通信マルチ）
      return false;

    case BTL_MULTIMODE_PP_AA:  ///< プレイヤー２人 vs AI２人（AI通信マルチ）
      return BattleRule::IsClientAi_Double_MultiMode_PP_AA( commMode, clientId );

    case BTL_MULTIMODE_PA_AA:  ///< プレイヤー１人＆AI１人 vs AI２人（AIマルチ）
      return BattleRule::IsClientAi_Double_MultiMode_PA_AA( commMode, clientId );

    case BTL_MULTIMODE_P_AA:   ///< プレイヤー１人 VS AI２人（AIタッグ）
      return BattleRule::IsClientAi_Double_MultiMode_P_AA( commMode, clientId );

    default:
      GFL_ASSERT_STOP(0);
      return false;
    }
  }

  /**
   * @brief クライアントが AI かどうかを取得する( ダブルバトル・非マルチ )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Double_MultiMode_NONE( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    // 非通信用
    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ true,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ false,
    };

    // 通信対戦用
    static const bool IS_AI_CLIENT_COMM[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ false,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ false,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return ( commMode == BTL_COMM_NONE ) ? ( IS_AI_CLIENT[ clientId ] ) : ( IS_AI_CLIENT_COMM[ clientId ] );
  }

  /**
   * @brief クライアントが AI かどうかを取得する( ダブルバトル・プレイヤー２人 vs AI２人 )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Double_MultiMode_PP_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    GFL_UNUSED( commMode );

    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ true,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ true,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return IS_AI_CLIENT[ clientId ];
  }

  /**
   * @brief クライアントが AI かどうかを取得する( ダブルバトル・プレイヤー１人＆AI１人 vs AI２人 )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Double_MultiMode_PA_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    GFL_UNUSED( commMode );

    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ true,
      /* BTL_CLIENT_PARTNER */ true,
      /* BTL_CLIENT_ENEMY2  */ true,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return IS_AI_CLIENT[ clientId ];
  }

  /**
   * @brief クライアントが AI かどうかを取得する( ダブルバトル・プレイヤー１人 VS AI２人 )
   * @param commMode   通信モード
   * @param clientId   チェック対象のクライアント
   * @retval true   クライアントは AI
   * @retval false  クライアントは AI でない
   */
  bool BattleRule::IsClientAi_Double_MultiMode_P_AA( BtlCommMode commMode, BTL_CLIENT_ID clientId )
  {
    GFL_UNUSED( commMode );

    static const bool IS_AI_CLIENT[ BTL_CLIENT_NUM ] = 
    {
      /* BTL_CLIENT_PLAYER  */ false,
      /* BTL_CLIENT_ENEMY1  */ true,
      /* BTL_CLIENT_PARTNER */ false,
      /* BTL_CLIENT_ENEMY2  */ true,
    };

    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return false;
    }

    return IS_AI_CLIENT[ clientId ];
  }

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
  bool BattleRule::IsOpponentClient( BtlRule rule, BTL_CLIENT_ID clientId_0, BTL_CLIENT_ID clientId_1 )
  {
    if( rule == BTL_RULE_ROYAL )
    {
      return ( clientId_0 != clientId_1 );
    }

    return ( clientId_0 & 1 ) != ( clientId_1 & 1 );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定クライアントの対戦相手クライアントのIDを取得する
   * @param rule           バトルのルール
   * @param multiMode      マルチモード
   * @param clientId       自クライアントのID
   * @param opponentIndex  対戦相手のインデックス
   */
  //-------------------------------------------------------------------------
  BTL_CLIENT_ID BattleRule::GetOpponentClientId( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId, u8 opponentIndex )
  {
    // ロイヤルは隣から順に敵１
    if( rule == BTL_RULE_ROYAL )
    {
      return static_cast<BTL_CLIENT_ID>( ( clientId + 1 + opponentIndex ) % BTL_CLIENT_ROYAL_NUM );
    }

    u32 opponentClientId = clientId;
    opponentClientId &= 1;
    opponentClientId ^= 1;

    if( opponentIndex )
    {
      u32 nextClientID = opponentClientId + ( ( opponentIndex & 1 ) * 2 );
      if( BattleRule::IsClientExist( rule, multiMode, static_cast<BTL_CLIENT_ID>( nextClientID ) ) )
      {
        opponentClientId = nextClientID;
      }
    }

    return static_cast<BTL_CLIENT_ID>( opponentClientId );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 厳密な勝敗判定を行うか？
   * @param rule        バトルのルール
   * @param competitor  対戦相手
   * @retval true   簡易的な勝敗判定を行う( 自陣営以外が全滅したら勝ち )
   * @retval false  厳密な勝敗判定を行う
   */
  //-------------------------------------------------------------------------
  bool BattleRule::IsResultStrictJudge( BtlRule rule, BtlCompetitor competitor )
  {
    if( rule == BTL_RULE_ROYAL ) {
      return true;
    }

    return ( competitor == BTL_COMPETITOR_INST ) || ( competitor == BTL_COMPETITOR_COMM );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 戦闘終了後の「お手入れ」が可能か？
   * @param commMode   通信モード
   * @param competitor  対戦相手
   * @retval true   戦闘後、「お手入れ( ポケパルレ )」への遷移が可能
   * @retval false  戦闘後、「お手入れ( ポケパルレ )」へ遷移してはいけない
   */
  //-------------------------------------------------------------------------
  bool BattleRule::IsOteireEnable( BtlCommMode commMode, BtlCompetitor competitor )
  {
    if( commMode != BTL_COMM_NONE )
    {
      return false;
    }

    if( ( competitor == BTL_COMPETITOR_WILD ) || 
        ( competitor == BTL_COMPETITOR_TRAINER ) )
    {
      return true;
    }

    return false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief リモートクライアントとの通信が切断された際に、ダミークライアントへの切り替えを行うか？
   * @param rule  バトルのルール
   * @retval true   ダミークライアントに切り替える
   * @retval true   ダミークライアントに切り替えない( 通信エラーになる )
   */
  //-------------------------------------------------------------------------
  bool BattleRule::IsDummyClientSwitchEnable( BtlRule rule )
  {
    if( rule == BTL_RULE_ROYAL )
    {
      return true;
    }
    return false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 決着が着いた後、そのターンの残りの戦闘処理をスキップするか？
   * @param rule  バトルのルール
   * @retval true   決着が着いた後は、残りの戦闘処理をスキップする
   * @retval false  決着が着いた後も、残りの戦闘処理を行う
   */
  //-------------------------------------------------------------------------
  bool BattleRule::IsSkipBattleAfterShowdown( BtlRule rule )
  {
    if( rule == BTL_RULE_ROYAL )
    {
      return false;
    }

    return true;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 乱入バトルが可能か？
   * @param rule        バトルのルール
   * @param competitor  対戦相手
   * @retval true    乱入できる
   * @retval false   乱入できない
   */
  //-------------------------------------------------------------------------
  bool BattleRule::CanIntrudeBattle( BtlRule rule, BtlCompetitor competitor )
  {
    // 野生戦でのみ発生する
    if( competitor != BTL_COMPETITOR_WILD )
    {
      return false;
    }
    
    // シングルでのみ発生する( 一度乱入が発生した後は、以降も発生する )
    if( ( rule != BTL_RULE_SINGLE ) &&
        ( rule != BTL_RULE_INTRUDE ) )
    {
      return false;
    }

    return true;
  }



GFL_NAMESPACE_END( btl )