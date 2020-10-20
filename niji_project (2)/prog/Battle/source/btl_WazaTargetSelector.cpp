//=============================================================================
/**
 * @file   btl_WazaTargetSelector.cpp
 * @date   2015/07/23 19:21:39
 * @author obata_toshihiro
 * @brief  技ターゲットの選出
 *
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include "./btl_mainmodule.h"
#include "./btl_pokecon.h"
#include "./btl_pokeparam.h"
#include "./btl_pokeset.h"
#include "./btl_WazaTargetSelector.h"

GFL_NAMESPACE_BEGIN( btl )


  //-------------------------------------------------------------------------
  /**
   * @brief 技ターゲットを選出する
   * 
   * @param      mainModule          参照するメインモジュール
   * @param      pokecon             ターゲット選出対象のポケモンコンテナ
   * @param      attacker            攻撃するポケモン
   * @param      targetPos           技ターゲットの位置
   * @param      wazaParam           使用する技のパラメータ
   * @param      intrPokeID         「カウンター」「ミラーコート」等により、
   *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
   * @param[out] rec                 選出したポケモンの格納先
   * @param[out] isTargetTemptEnable 選出後、技「このゆびとまれ」特性「よびみず」等による、技ターゲットの変更を許可するか？
   */
  //-------------------------------------------------------------------------
  void WazaTargetSelector::SelectWazaTargets(
    const MainModule*     mainModule, 
    POKECON*              pokecon, 
    BTL_POKEPARAM*        attacker, 
    BtlPokePos            targetPos,
    const SVFL_WAZAPARAM* wazaParam, 
    u8                    intrPokeID, 
    PokeSet*              rec,
    bool*                 isTargetTemptEnable )
  {
    switch( mainModule->GetRule() )
    {
    case BTL_RULE_SINGLE:
    default:
      SelectWazaTargets_Single( mainModule, pokecon, attacker, targetPos, wazaParam, intrPokeID, rec );
      break;

    case BTL_RULE_DOUBLE:
      SelectWazaTargets_Double( mainModule, pokecon, attacker, targetPos, wazaParam, intrPokeID, rec, isTargetTemptEnable );
      break;

    case BTL_RULE_ROYAL:
      SelectWazaTargets_Royal( mainModule, pokecon, attacker, targetPos, wazaParam, intrPokeID, rec, isTargetTemptEnable );
      break;

    case BTL_RULE_INTRUDE:
      SelectWazaTargets_Intrude( mainModule, pokecon, attacker, targetPos, wazaParam, intrPokeID, rec, isTargetTemptEnable );
      break;
    }
  }


  /**
   * @param 技ターゲットを選出する( シングル用 )
   *
   * @param      mainModule          参照するメインモジュール
   * @param      pokecon             ターゲット選出対象のポケモンコンテナ
   * @param      attacker            攻撃するポケモン
   * @param      targetPos           技ターゲットの位置
   * @param      wazaParam           使用する技のパラメータ
   * @param      intrPokeID         「カウンター」「ミラーコート」等により、
   *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
   * @param[out] rec                 選出したポケモンの格納先
   */
  void WazaTargetSelector::SelectWazaTargets_Single( 
      const MainModule*     mainModule, 
      POKECON*              pokecon, 
      BTL_POKEPARAM*        attacker,
      BtlPokePos            targetPos,
      const SVFL_WAZAPARAM* wazaParam,
      u8                    intrPokeID,
      PokeSet*              rec )
  {
    GFL_UNUSED( targetPos ); 

    const BtlPokePos attackerPos = mainModule->PokeIDtoPokePos( pokecon, attacker->GetID() );
  
    switch( wazaParam->targetType )
    {
    case pml::wazadata::TARGET_OTHER_SELECT:  ///< 自分以外の１体（選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:  ///< 敵１体（選択）
    case pml::wazadata::TARGET_ENEMY_RANDOM:  ///< 敵ランダム
    case pml::wazadata::TARGET_ENEMY_ALL:     ///< 敵側２体
    case pml::wazadata::TARGET_OTHER_ALL:     ///< 自分以外全部
      if( intrPokeID == BTL_POKEID_NULL )
      {
        rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0) );
      }
      else
      {
        rec->Add( pokecon->GetPokeParam( intrPokeID ) );
      }
      return;
  
    case pml::wazadata::TARGET_USER:                ///< 自分１体のみ
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体
    case pml::wazadata::TARGET_FRIEND_ALL:
      if( intrPokeID == BTL_POKEID_NULL )
      {
        rec->Add( attacker );
      }
      else
      {
        rec->Add( pokecon->GetPokeParam(intrPokeID) );
      }
      return;
  
    case pml::wazadata::TARGET_ALL:
      {
        rec->Add( attacker );
        rec->Add( GetOpponentPokemon(mainModule, pokecon, attackerPos, 0) );
      }
      return;
  
    case pml::wazadata::TARGET_UNKNOWN:
      if( intrPokeID != BTL_POKEID_NULL ) {
        rec->Add( pokecon->GetPokeParam(intrPokeID) );
      }
      return;
  
    default:
      return;
    }
  }

  /**
   * @param 技ターゲットを選出する( ダブル用 )
   *
   * @param      mainModule          参照するメインモジュール
   * @param      pokecon             ターゲット選出対象のポケモンコンテナ
   * @param      attacker            攻撃するポケモン
   * @param      targetPos           技ターゲットの位置
   * @param      wazaParam           使用する技のパラメータ
   * @param      intrPokeID         「カウンター」「ミラーコート」等により、
   *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
   * @param[out] rec                 選出したポケモンの格納先
   * @param[out] isTargetTemptEnable 選出後、技「このゆびとまれ」特性「よびみず」等による、技ターゲットの変更を許可するか？
   */
  void WazaTargetSelector::SelectWazaTargets_Double( 
      const MainModule*     mainModule, 
      POKECON*              pokecon, 
      BTL_POKEPARAM*        attacker, 
      BtlPokePos            targetPos,
      const SVFL_WAZAPARAM* wazaParam, 
      u8                    intrPokeID, 
      PokeSet*              rec, 
      bool*                 isTargetTemptEnable )
  {
    *isTargetTemptEnable = false;
  
    // 「のろい」ｘ「へんしょく」などで、途中で有効範囲と位置指定が食い違うことが起こりうるので補正
    targetPos = CorrectNoroiTarget( mainModule, pokecon, wazaParam, attacker, targetPos );


    const BtlPokePos attackerPos = mainModule->PokeIDtoPokePos( pokecon, attacker->GetID() );

    switch( wazaParam->targetType )
    {
    case pml::wazadata::TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
      rec->Add( pokecon->GetFrontPokeData( targetPos ) );
      *isTargetTemptEnable = true;
      return;

    case pml::wazadata::TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, calc::GetRand(2) ) );
      *isTargetTemptEnable = true;
      return;
  
    case pml::wazadata::TARGET_ENEMY_ALL:           ///< 敵側全体
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 1) );
      return;

    case pml::wazadata::TARGET_OTHER_ALL:           ///< 自分以外全部
      rec->Add( GetNextPokemon( mainModule, pokecon, attackerPos ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0 ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 1 ) );
      return;

    case pml::wazadata::TARGET_ALL:                ///< 全部
      rec->Add( attacker );
      rec->Add( GetNextPokemon( mainModule, pokecon, attackerPos ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0 ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 1 ) );
      return;
  
    case pml::wazadata::TARGET_USER:      ///< 自分１体のみ
      if( intrPokeID == BTL_POKEID_NULL ){
        rec->Add( attacker );
      }else{
        rec->Add( pokecon->GetPokeParam(intrPokeID) );
      }
      return;
  
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
      {
        BTL_POKEPARAM* target = pokecon->GetFrontPokeData( targetPos );
        if( ( target == NULL   ) || 
            ( target->IsDead() ) ) {
          target = attacker;
        }
        rec->Add( target );
      }
      return;

    case pml::wazadata::TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
      rec->Add( pokecon->GetFrontPokeData( targetPos ) );
      return;

    case pml::wazadata::TARGET_FRIEND_ALL:          ///< 味方全部
      rec->Add( attacker );
      rec->Add( GetNextPokemon( mainModule, pokecon, attackerPos) );
      return;
  
    case pml::wazadata::TARGET_UNKNOWN:
      if( intrPokeID != BTL_POKEID_NULL )
      {
        rec->Add( pokecon->GetPokeParam( intrPokeID ) );
        *isTargetTemptEnable = true;
      }
      return;
  
    default:
      return;
    }
  }

  /**
   * @param 技ターゲットを選出する( バトルロイヤル用 )
   *
   * @param      mainModule          参照するメインモジュール
   * @param      pokecon             ターゲット選出対象のポケモンコンテナ
   * @param      attacker            攻撃するポケモン
   * @param      targetPos           技ターゲットの位置
   * @param      wazaParam           使用する技のパラメータ
   * @param      intrPokeID         「カウンター」「ミラーコート」等により、
   *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
   * @param[out] rec                 選出したポケモンの格納先
   * @param[out] isTargetTemptEnable 選出後、技「このゆびとまれ」特性「よびみず」等による、技ターゲットの変更を許可するか？
   */
  void WazaTargetSelector::SelectWazaTargets_Royal( 
    const MainModule*     mainModule, 
    POKECON*              pokecon,
    BTL_POKEPARAM*        attacker, 
    BtlPokePos            targetPos,
    const SVFL_WAZAPARAM* wazaParam, 
    u8                    intrPokeID, 
    PokeSet*              rec,
    bool*                 isTargetTemptEnable )
  {
    *isTargetTemptEnable = false;
  
    // 「のろい」ｘ「へんしょく」などで、途中で有効範囲と位置指定が食い違うことが起こりうるので補正
    targetPos = CorrectNoroiTarget( mainModule, pokecon, wazaParam, attacker, targetPos );


    const BtlPokePos attackerPos = mainModule->PokeIDtoPokePos( pokecon, attacker->GetID() );
  
    switch( wazaParam->targetType )
    {
    case pml::wazadata::TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
      rec->Add( pokecon->GetFrontPokeData( targetPos ) );
      *isTargetTemptEnable = true;
      return;

    case pml::wazadata::TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
      if( targetPos != BTL_POS_NULL )
      {
        rec->Add( pokecon->GetFrontPokeData( targetPos ) );
        *isTargetTemptEnable = true;
      }
      return;

    case pml::wazadata::TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, calc::GetRand(2) ) );
      *isTargetTemptEnable = true;
      return;
  
    case pml::wazadata::TARGET_ENEMY_ALL:           ///< 敵側全体
    case pml::wazadata::TARGET_OTHER_ALL:           ///< 自分以外全部
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0 ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 1 ) );
      rec->Add( GetOpponentPokemon (mainModule, pokecon, attackerPos, 2 ) );
      return;
  
    case pml::wazadata::TARGET_ALL:                ///< 全部
      rec->Add( attacker );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 0 ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 1 ) );
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, 2 ) );
      return;
  
    case pml::wazadata::TARGET_USER:      ///< 自分１体のみ
      if( intrPokeID == BTL_POKEID_NULL ) {
        rec->Add( attacker );
      }else{
        rec->Add( pokecon->GetPokeParam( intrPokeID ) );
      }
      return;
  
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
    case pml::wazadata::TARGET_FRIEND_ALL:          ///< 味方全部
      rec->Add( attacker );
      return;
  
    case pml::wazadata::TARGET_UNKNOWN:
      if( intrPokeID != BTL_POKEID_NULL )
      {
        rec->Add( pokecon->GetPokeParam( intrPokeID ) );
        *isTargetTemptEnable = true;
      }
      return;
  
    default:
      return;
    }
  }

  /**
   * @param 技ターゲットを選出する( 乱入用 )
   *
   * @param      mainModule          参照するメインモジュール
   * @param      pokecon             ターゲット選出対象のポケモンコンテナ
   * @param      attacker            攻撃するポケモン
   * @param      targetPos           技ターゲットの位置
   * @param      wazaParam           使用する技のパラメータ
   * @param      intrPokeID         「カウンター」「ミラーコート」等により、
   *                                  技ターゲットとして割り込んだポケモンのID( 割り込みが無い場合は BTL_POKEID_NULL )
   * @param[out] rec                 選出したポケモンの格納先
   * @param[out] isTargetTemptEnable 選出後、技「このゆびとまれ」特性「よびみず」等による、技ターゲットの変更を許可するか？
   */
  void WazaTargetSelector::SelectWazaTargets_Intrude( 
      const MainModule*     mainModule, 
      POKECON*              pokecon, 
      BTL_POKEPARAM*        attacker, 
      BtlPokePos            targetPos,
      const SVFL_WAZAPARAM* wazaParam, 
      u8                    intrPokeID, 
      PokeSet*              rec, 
      bool*                 isTargetTemptEnable )
  {
    *isTargetTemptEnable = false;
  
    // 「のろい」ｘ「へんしょく」などで、途中で有効範囲と位置指定が食い違うことが起こりうるので補正
    targetPos = CorrectNoroiTarget( mainModule, pokecon, wazaParam, attacker, targetPos );


    const BtlPokePos attackerPos = mainModule->PokeIDtoPokePos( pokecon, attacker->GetID() );

    switch( wazaParam->targetType )
    {
    case pml::wazadata::TARGET_OTHER_SELECT:        ///< 自分以外の１体（選択）
    case pml::wazadata::TARGET_ENEMY_SELECT:        ///< 敵１体（選択）
      rec->Add( pokecon->GetFrontPokeData( targetPos ) );
      *isTargetTemptEnable = true;
      return;

    case pml::wazadata::TARGET_ENEMY_RANDOM:        ///< 敵ランダムに１体
      rec->Add( GetOpponentPokemon( mainModule, pokecon, attackerPos, calc::GetRand(2) ) );
      *isTargetTemptEnable = true;
      return;
  
    case pml::wazadata::TARGET_ENEMY_ALL:           ///< 敵側全体
      {
        BtlExPos exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, attackerPos );
        u8 pos[ BTL_POS_NUM ];
        u8 posNum = mainModule->ExpandBtlPos( exPos, pos );
        for( u8 i=0; i<posNum; ++i )
        {
          BtlPokePos addPos = static_cast<BtlPokePos>( pos[i] );
          rec->Add( pokecon->GetFrontPokeData( addPos ) );
        }
      }
      return;

    case pml::wazadata::TARGET_OTHER_ALL:           ///< 自分以外全部
      {
        BtlExPos exPos = EXPOS_MAKE( BTL_EXPOS_AREA_OTHERS, attackerPos );
        u8 pos[ BTL_POS_NUM ];
        u8 posNum = mainModule->ExpandBtlPos( exPos, pos );
        for( u8 i=0; i<posNum; ++i )
        {
          BtlPokePos addPos = static_cast<BtlPokePos>( pos[i] );
          rec->Add( pokecon->GetFrontPokeData( addPos ) );
        }
      }
      return;

    case pml::wazadata::TARGET_ALL:                ///< 全部
      {
        BtlExPos exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ALL, attackerPos );
        u8 pos[ BTL_POS_NUM ];
        u8 posNum = mainModule->ExpandBtlPos( exPos, pos );
        for( u8 i=0; i<posNum; ++i )
        {
          BtlPokePos addPos = static_cast<BtlPokePos>( pos[i] );
          rec->Add( pokecon->GetFrontPokeData( addPos ) );
        }
      }
      return;
  
    case pml::wazadata::TARGET_USER:      ///< 自分１体のみ
      if( intrPokeID == BTL_POKEID_NULL ){
        rec->Add( attacker );
      }else{
        rec->Add( pokecon->GetPokeParam(intrPokeID) );
      }
      return;
  
    case pml::wazadata::TARGET_FRIEND_USER_SELECT:  ///< 自分を含む味方１体（選択）
      {
        BTL_POKEPARAM* target = pokecon->GetFrontPokeData( targetPos );
        if( ( target == NULL   ) || 
            ( target->IsDead() ) ) {
          target = attacker;
        }
        rec->Add( target );
      }
      return;

    case pml::wazadata::TARGET_FRIEND_SELECT:       ///< 自分以外の味方１体
      if( targetPos != BTL_POS_NULL )
      {
        rec->Add( pokecon->GetFrontPokeData( targetPos ) );
      }
      return;

    case pml::wazadata::TARGET_FRIEND_ALL:          ///< 味方全部
      {
        BtlExPos exPos = EXPOS_MAKE( BTL_EXPOS_AREA_MYTEAM, attackerPos );
        u8 pos[ BTL_POS_NUM ];
        u8 posNum = mainModule->ExpandBtlPos( exPos, pos );
        for( u8 i=0; i<posNum; ++i )
        {
          BtlPokePos addPos = static_cast<BtlPokePos>( pos[i] );
          rec->Add( pokecon->GetFrontPokeData( addPos ) );
        }
      }
      return;
  
    case pml::wazadata::TARGET_UNKNOWN:
      if( intrPokeID != BTL_POKEID_NULL )
      {
        rec->Add( pokecon->GetPokeParam( intrPokeID ) );
        *isTargetTemptEnable = true;
      }
      return;
  
    default:
      return;
    }
  }


  /**
   * @param 指定位置から見て対戦相手のポケモンデータを取得する
   * @param mainModule        参照するメインモジュール
   * @param pokecon           取得先のポケモンコンテナ
   * @param myPoso            自分の位置
   * @param opponentPokeIndex 相手側の何番目のポケモンを取得するか？
   */
  BTL_POKEPARAM* WazaTargetSelector::GetOpponentPokemon( 
      const MainModule* mainModule, 
      POKECON*          pokecon,
      BtlPokePos        myPoso,
      u8                opponentPokeIndex )
  {
    u8 clientID, posIdx; 
    BtlPokePos targetPos = mainModule->GetOpponentPokePos( myPoso, opponentPokeIndex );
    mainModule->BtlPosToClientID_and_PosIdx( targetPos, &clientID, &posIdx );
    return pokecon->GetClientPokeData( clientID, posIdx );
  }

  /**
   * @param 指定位置の味方のポケモンデータを返す
   * @param mainModule   参照するメインモジュール
   * @param pokecon      取得先のポケモンコンテナ
   * @param myPoso       自分の位置
   */
  BTL_POKEPARAM* WazaTargetSelector::GetNextPokemon( const MainModule* mainModule, POKECON* pokecon, BtlPokePos myPoso )
  {
    u8 clientID, posIdx;
    BtlPokePos nextPos = mainModule->GetFriendPokePos( myPoso, 1 );
    mainModule->BtlPosToClientID_and_PosIdx( nextPos, &clientID, &posIdx );
    return pokecon->GetClientPokeData( clientID, posIdx );
  }

  /**
   * @brief 「のろい」を打とうとするポケモンが「へんしょく」などでタイプが変わった際に対象選択を補正する
   * @param mainModule  参照するメインモジュール
   * @param pokecon     参照するポケモンコンテナ
   * @param wazaParam   使用する技のパラメータ
   * @param attacker    攻撃するポケモン
   * @param targetPos   補正前の技ターゲット位置
   */
  BtlPokePos WazaTargetSelector::CorrectNoroiTarget( 
    const MainModule*     mainModule,
    const POKECON*        pokecon, 
    const SVFL_WAZAPARAM* wazaParam, 
    const BTL_POKEPARAM*  attacker,
    BtlPokePos            targetPos )
  {
    const BtlPokePos attackerPos = mainModule->PokeIDtoPokePos( pokecon, attacker->GetID() );

    if( wazaParam->wazaID == WAZANO_NOROI )
    {
      // 自分以外の１体（選択）：ゴースト以外で使用 -> ゴーストに変更した場合をチェック
      if( wazaParam->targetType == pml::wazadata::TARGET_OTHER_SELECT )
      {
        if( ( attackerPos == targetPos ) || 
            ( targetPos == BTL_POS_NULL ) ) {
          targetPos = calc::DecideWazaTargetAuto( mainModule, pokecon, attacker, WAZANO_NOROI );
        }
      }
      // 自分のみ：ゴーストで使用 -> ゴースト以外に変更した場合をチェック
      else if( wazaParam->targetType == pml::wazadata::TARGET_USER )
      {
        if( attackerPos != targetPos ) {
          targetPos = attackerPos;
        }
      }
    }
    return targetPos;
  }


GFL_NAMESPACE_END( btl )
