//=============================================================================
/**
 * @file   btl_PokemonPosition.h
 * @date   2015/07/17 10:31:10
 * @author obata_toshihiro
 * @brief  ポケモンの立ち位置・陣営
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include "./btl_const.h"
#include "./btl_PokemonPosition.h"


GFL_NAMESPACE_BEGIN( btl )


  namespace
  {
    /**
     * @brief バトルロイヤル用　陣営　→　立ち位置　変換テーブル
     */
    static const BtlPokePos ROYAL_POS_BY_SIDE[ BTL_SIDE_NUM ] = 
    {
      /* BTL_SIDE_1ST */ BTL_POS_ROYAL_1ST,
      /* BTL_SIDE_2ND */ BTL_POS_ROYAL_2ND,
      /* BTL_SIDE_3RD */ BTL_POS_ROYAL_3RD,
      /* BTL_SIDE_4TH */ BTL_POS_ROYAL_4TH,
    };
  }



  /**
   * @brief コンストラクタ1
   */
  PokemonPosition::PokemonPosition( void )
  {
  }

  /**
   * @brief デストラクタ
   */
  PokemonPosition::~PokemonPosition()
  {
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定クライアントの担当する位置の数を取得する
   * @param rule       バトルのルール
   * @param multiMode  マルチモード
   * @param clientId   チェック対象クライアントのID
   */
  //-------------------------------------------------------------------------
  u32 PokemonPosition::GetClientCoverPosNum( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId )
  {
    u32 posNum = 0;
    for( u32 pos=0; pos<BTL_POS_NUM; ++pos )
    {
      BTL_CLIENT_ID posCoverClientId = PokemonPosition::GetPosCoverClientId( rule, multiMode, static_cast<BtlPokePos>(pos) );
      if( posCoverClientId == clientId ) {
        ++posNum;
      }
    }
    return posNum;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 立ち位置の担当クライアントを取得する
   * @param rule       バトルのルール
   * @param multiMode  マルチモード
   * @param pos        立ち位置
   */
  //-------------------------------------------------------------------------
  BTL_CLIENT_ID PokemonPosition::GetPosCoverClientId( BtlRule rule, BtlMultiMode multiMode, BtlPokePos pos )
  {
    switch( rule ) {
    case BTL_RULE_SINGLE:  return PokemonPosition::GetPosCoverClientId_Single( pos );
    case BTL_RULE_DOUBLE:  return PokemonPosition::GetPosCoverClientId_Double( multiMode, pos );
    case BTL_RULE_ROYAL:   return PokemonPosition::GetPosCoverClientId_Royal( pos );
    case BTL_RULE_INTRUDE: return PokemonPosition::GetPosCoverClientId_Intrude( pos );
    }

    GFL_ASSERT(0);
    return BTL_CLIENT_NULL;
  }

  /**
   * @brief 立ち位置の担当クライアントを取得する( シングルバトル )
   * @param pos 立ち位置
   */
  BTL_CLIENT_ID PokemonPosition::GetPosCoverClientId_Single( BtlPokePos pos )
  {
    static const BTL_CLIENT_ID POS_COVER_CLIENT_SINGLE[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_NULL,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_NULL,
    };

    if( BTL_POS_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return BTL_CLIENT_NULL;
    }

    return POS_COVER_CLIENT_SINGLE[ pos ];
  }

  /**
   * @brief 立ち位置の担当クライアントを取得する( ダブルバトル )
   * @param multiMode  マルチモード
   * @param pos        立ち位置
   */
  BTL_CLIENT_ID PokemonPosition::GetPosCoverClientId_Double( BtlMultiMode multiMode, BtlPokePos pos )
  {
    // 非マルチ
    static const BTL_CLIENT_ID POS_COVER_CLIENT_DOUBLE[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_ENEMY1,
    };

    // 通信マルチ・通信タッグマルチ・AIマルチ
    static const BTL_CLIENT_ID POS_COVER_CLIENT_DOUBLE_MULTI[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_PARTNER,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_ENEMY2,
    };

    // AIタッグマルチ
    static const BTL_CLIENT_ID POS_COVER_CLIENT_DOUBLE_AI_TAG[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_ENEMY2,
    };

    if( BTL_POS_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return BTL_CLIENT_NULL;
    }

    switch( multiMode )
    {
    default:
    case BTL_MULTIMODE_NONE:  // 通常のダブルバトル
      return POS_COVER_CLIENT_DOUBLE[ pos ];

    case BTL_MULTIMODE_PP_PP: // 通信マルチ
    case BTL_MULTIMODE_PP_AA: // 通信タッグマルチ
    case BTL_MULTIMODE_PA_AA: // AIマルチ
      return POS_COVER_CLIENT_DOUBLE_MULTI[ pos ];
    
    case BTL_MULTIMODE_P_AA:  // AIタッグマルチ
      return POS_COVER_CLIENT_DOUBLE_AI_TAG[ pos ];
    }
  }

  /**
   * @brief 立ち位置の担当クライアントを取得する( バトルロイヤル )
   * @param pos 立ち位置
   */
  BTL_CLIENT_ID PokemonPosition::GetPosCoverClientId_Royal( BtlPokePos pos )
  {
    static const BTL_CLIENT_ID POS_COVER_CLIENT_ROYAL[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_PARTNER,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_ENEMY2,
    };

    if( BTL_POS_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return BTL_CLIENT_NULL;
    }

    return POS_COVER_CLIENT_ROYAL[ pos ];
  }

  /**
   * @brief 立ち位置の担当クライアントを取得する( 乱入 )
   * @param pos 立ち位置
   */
  BTL_CLIENT_ID PokemonPosition::GetPosCoverClientId_Intrude( BtlPokePos pos )
  {
    static const BTL_CLIENT_ID POS_COVER_CLIENT_INTRUDE[ BTL_POS_NUM ] = 
    {
      /* BTL_POS_1ST_0 */ BTL_CLIENT_PLAYER,
      /* BTL_POS_2ND_0 */ BTL_CLIENT_ENEMY1,
      /* BTL_POS_1ST_1 */ BTL_CLIENT_NULL,
      /* BTL_POS_2ND_1 */ BTL_CLIENT_ENEMY1,
    };

    if( BTL_POS_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return BTL_CLIENT_NULL;
    }

    return POS_COVER_CLIENT_INTRUDE[ pos ];
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 立ち位置が存在するか？
   * @param rule       バトルのルール
   * @param multiMode  マルチモード
   * @param pos        立ち位置
   * @retval true   指定した立ち位置が存在する
   * @retval false  指定した立ち位置が存在しない
   */
  //-------------------------------------------------------------------------
  bool PokemonPosition::IsPokePosExist( BtlRule rule, BtlMultiMode multiMode, BtlPokePos pos )
  {
    BTL_CLIENT_ID clientId = PokemonPosition::GetPosCoverClientId( rule, multiMode, pos );
    return ( clientId != BTL_CLIENT_NULL );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 現在のルールに応じて、有効になる立ち位置の最大値を返す
   * @param rule  バトルのルール
   */
  //-------------------------------------------------------------------------
  BtlPokePos PokemonPosition::GetValidPosMax( BtlRule rule )
  {
    switch( rule )
    {
    case BTL_RULE_SINGLE:
      return BTL_POS_2ND_0;
    
    case BTL_RULE_DOUBLE:
    case BTL_RULE_INTRUDE:
      return BTL_POS_2ND_1;

    case BTL_RULE_ROYAL:
      return BTL_POS_ROYAL_MAX;
    
    default:
      GFL_ASSERT(0);
      return BTL_POS_2ND_1;
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 相手側のポケモン位置取得
   * @param rule               バトルのルール
   * @param myBasePos          自分側の基本位置
   * @param opponentPosIndex   相手側の何番目の位置を取得するか？
   */
  //-------------------------------------------------------------------------
  BtlPokePos PokemonPosition::GetOpponentPokePos( BtlRule rule, BtlPokePos myBasePos, u8 opponentPosIndex )
  {
    switch( rule )
    {
    case BTL_RULE_SINGLE:
      GFL_ASSERT( opponentPosIndex < 1 );
      break;

    case BTL_RULE_DOUBLE:
      GFL_ASSERT( opponentPosIndex < 2 );
      break;

    case BTL_RULE_ROYAL:
      GFL_ASSERT( opponentPosIndex < BTL_POS_ROYAL_MAX );
      break;

    case BTL_RULE_INTRUDE:
      GFL_ASSERT( opponentPosIndex < 2 );
      break;

    default:
      GFL_ASSERT(0);
      break;
    }

    if( rule == BTL_RULE_ROYAL )
    {
      return ( BTL_POS_ROYAL_MIN + ( ( myBasePos + 1 + opponentPosIndex ) % BTL_POS_ROYAL_NUM ) );
    }

    if( rule == BTL_RULE_INTRUDE )
    {
      if( myBasePos == BTL_POS_1ST_0 )
      {
        return ( BTL_POS_2ND_0 + ( opponentPosIndex * 2 ) );
      }
      else
      {
        return BTL_POS_1ST_0;
      }
    }

    if( ( myBasePos & 1 ) == BTL_POS_1ST_0 )
    {
      return BTL_POS_2ND_0 + ( opponentPosIndex * 2 );
    }
    else
    {
      return BTL_POS_1ST_0 + ( opponentPosIndex * 2 );
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定した陣営について、前衛( ワザがあたる場所 )の数を返す
   * @param rule  バトルのルール
   * @param side  陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   */
  //-------------------------------------------------------------------------
  u32 PokemonPosition::GetFrontPosNum( BtlRule rule, BtlSide side )
  {
    if( BTL_SIDE_NUM <= side )
    {
      GFL_ASSERT(0);
      return 0;
    }

    switch( rule )
    {
    case BTL_RULE_SINGLE:
      return 1;

    case BTL_RULE_DOUBLE:
      return 2;

    case BTL_RULE_ROYAL:
      return 1;

    case BTL_RULE_INTRUDE:
      return ( side == BTL_SIDE_1ST ) ? ( 1 ) : ( 2 );

    default:
      GFL_ASSERT(0);
      return 1;
    }
  }
  
  //-------------------------------------------------------------------------
  /**
   * @brief 陣営から立ち位置を取得する
   * @param rule      バトルのルール
   * @param side      陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   * @param posIndex  陣営内の位置インデックス
   */
  //-------------------------------------------------------------------------
  BtlPokePos PokemonPosition::GetSidePos( BtlRule rule, BtlSide side, u8 posIndex )
  {
    GFL_ASSERT( side     < BTL_SIDE_NUM   );
    GFL_ASSERT( posIndex < BTL_POSIDX_MAX );

    if( rule == BTL_RULE_ROYAL )
    {
      return ROYAL_POS_BY_SIDE[ side ];
    }

    return ( side & 1 ) + ( posIndex * 2 );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 立ち位置から陣営取得する
   * @param rule  バトルのルール
   * @param pos   立ち位置
   * @retval [ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   */
  //-------------------------------------------------------------------------
  BtlSide PokemonPosition::PosToSide( BtlRule rule, BtlPokePos pos )
  {
    if( rule == BTL_RULE_ROYAL )
    {
      for( u32 side=0; side<BTL_SIDE_NUM; ++side )
      {
        if( ROYAL_POS_BY_SIDE[ side ] == pos ) {
          return static_cast<BtlSide>( side );
        }
      }
      GFL_ASSERT(0);
      return BTL_SIDE_1ST;
    }

    return ( pos & 1 );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 指定した陣営が、プレイヤの陣営を含むかどうかチェックする
   * @param playerSide  プレイヤの陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   * @param checkSide   チェック対象の陣営
   * @retval true   プレイヤの陣営を含む
   * @retval false  プレイヤの陣営を含まない
   */
  //-------------------------------------------------------------------------
  bool PokemonPosition::IsPlayerSide( BtlSide playerSide, BtlSide checkSide )
  {
    if( !PokemonPosition::IsUnitSide( playerSide ) )
    {
      GFL_ASSERT(0);
      return false;
    }

    if( playerSide == checkSide )
    {
      return true;
    }

    switch( checkSide )
    {
      case BTL_MULTI_SIDE_ALL:           return true;
      case BTL_MULTI_SIDE_WITHOUT_1ST :  return ( playerSide != BTL_SIDE_1ST );
      case BTL_MULTI_SIDE_WITHOUT_2ND :  return ( playerSide != BTL_SIDE_2ND );
      case BTL_MULTI_SIDE_WITHOUT_3RD :  return ( playerSide != BTL_SIDE_3RD );
      case BTL_MULTI_SIDE_WITHOUT_4TH :  return ( playerSide != BTL_SIDE_4TH );
    }

    return false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 敵陣営を取得する
   * @param rule  バトルのルール
   * @param side  自陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   *
   * @retval [ BTL_SIDE_1ST,               BTL_SIDE_4TH               ] バトルロイヤル以外
   * @retval [ BTL_MULTI_SIDE_WITHOUT_1ST, BTL_MULTI_SIDE_WITHOUT_4TH ] バトルロイヤルの場合
   */
  //-------------------------------------------------------------------------
  BtlSide PokemonPosition::GetOpponentSide( BtlRule rule, BtlSide side )
  {
    if( !PokemonPosition::IsUnitSide( side ) )
    {
      GFL_ASSERT(0);
      return BTL_SIDE_NULL;
    }

    if( rule == BTL_RULE_ROYAL )
    {
      switch( side ) {
      case BTL_SIDE_1ST:  return BTL_MULTI_SIDE_WITHOUT_1ST;
      case BTL_SIDE_2ND:  return BTL_MULTI_SIDE_WITHOUT_2ND;
      case BTL_SIDE_3RD:  return BTL_MULTI_SIDE_WITHOUT_3RD;
      case BTL_SIDE_4TH:  return BTL_MULTI_SIDE_WITHOUT_4TH;
      }
    }

    return !side;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 陣営が存在するか？
   * @param rule  バトルのルール
   * @param side  チェック対象の陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   * @retval true   陣営が存在する
   * @retval false  陣営が存在しない
   */
  //-------------------------------------------------------------------------
  bool PokemonPosition::IsSideExist( BtlRule rule, BtlSide side )
  {
    if( !PokemonPosition::IsUnitSide( side ) )
    {
      GFL_ASSERT(0);
      return false;
    }

    if( rule == BTL_RULE_ROYAL ) {
      return true;
    }

    if( ( side == BTL_SIDE_1ST ) ||
        ( side == BTL_SIDE_2ND ) ) {
      return true;
    }

    return false;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 存在する陣営の数を取得する？
   * @param rule  バトルのルール
   */
  //-------------------------------------------------------------------------
  u8 PokemonPosition::GetSideNum( BtlRule rule )
  {
    u8 sideNum = 0;
    for( u32 i=0; i<BTL_SIDE_NUM; ++i )
    {
      BtlSide side = static_cast<BtlSide>( i );
      if( PokemonPosition::IsSideExist( rule, side ) ) {
        ++sideNum;
      }
    }
    return sideNum;
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 複数の陣営を表す BtlSide( BTL_MULTI_SIDE_xxxx ) を、単一の陣営を表す BtlSide( BTL_SIDE_xxxx )に展開する
   * @param[out] expandSide     展開後の陣営の格納先配列( 要素数 BTL_SIDE_NUM 必要 )
   * @param[out] expandSideNum  展開後の陣営の数
   * @param      rule           バトルのルール
   * @param      targetSide     展開する陣営
   */
  //-------------------------------------------------------------------------
  void PokemonPosition::ExpandSide( BtlSide* expandSide, u8* expandSideNum, BtlRule rule, BtlSide targetSide )
  {
    *expandSideNum = 0;

    // 単一陣営なら単純
    if( PokemonPosition::IsUnitSide( targetSide ) )
    {
      expandSide[0]  = targetSide;
      *expandSideNum = 1;
      return;
    }

    // 複数陣営なら、それぞれ展開
    switch( targetSide )
    {
    case BTL_MULTI_SIDE_ALL:
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_1ST );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_2ND );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_3RD );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_4TH );
      break;

    case BTL_MULTI_SIDE_WITHOUT_1ST:
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_2ND );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_3RD );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_4TH );
      break;

    case BTL_MULTI_SIDE_WITHOUT_2ND:
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_1ST );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_3RD );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_4TH );
      break;

    case BTL_MULTI_SIDE_WITHOUT_3RD:
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_1ST );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_2ND );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_4TH );
      break;

    case BTL_MULTI_SIDE_WITHOUT_4TH:
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_1ST );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_2ND );
      PokemonPosition::AddSideIfExist( expandSide, expandSideNum, rule, BTL_SIDE_3RD );
      break;

    default:
      GFL_ASSERT(0);
      *expandSideNum = 0;
      return;
    }
  }

  /**
   * @brief 指定した陣営が存在するなら、配列に追加する
   * @param[out] destArray  陣営の追加先の配列( 要素数 BTL_SIDE_NUM が必要 )
   * @param[out] count      陣営の数の格納先
   * @param      rule       バトルのルール
   * @param      side       チェック対象の陣営
   */
  void PokemonPosition::AddSideIfExist( BtlSide* destArray, u8* count, BtlRule rule, BtlSide side )
  {
    if( PokemonPosition::IsSideExist( rule, side ) )
    {
      destArray[ *count ] = side;
      ++( *count );
    }
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 単一の陣営を表す BtlSide かどうかチェックする
   * @param side  チェック対象の陣営
   * @retval true    単一の陣営を表す BtlSide である
   * @retval false   単一の陣営を表す BtlSide でない
   */
  //-------------------------------------------------------------------------
  bool PokemonPosition::IsUnitSide( BtlSide side )
  {
    return ( ( BTL_SIDE_MIN <= side ) && ( side <= BTL_SIDE_MAX ) );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 複数の陣営を表す BtlSide かどうかチェックする
   * @param side  チェック対象の陣営
   * @retval true    複数の陣営を表す BtlSide である
   * @retval false   複数の陣営を表す BtlSide でない
   */
  //-------------------------------------------------------------------------
  bool PokemonPosition::IsMultiSide( BtlSide side )
  {
    return ( ( BTL_MULTI_SIDE_MIN <= side ) && ( side <= BTL_MULTI_SIDE_MAX ) );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief クライアントの陣営を取得する
   * @param rule      バトルのルール
   * @param clientId  マルチモード
   * @retval [ BTL_SIDE_1ST, BTL_SIDE_4TH ]
   */
  //-------------------------------------------------------------------------
  BtlSide PokemonPosition::GetClientSide( BtlRule rule, BTL_CLIENT_ID clientId )
  {
    if( BTL_CLIENT_NUM <= clientId )
    {
      GFL_ASSERT(0);
      return BTL_SIDE_1ST;
    }

    if( rule == BTL_RULE_ROYAL )
    {
      static const BtlSide SIDE_BY_CLIENT_ROYAL[ BTL_CLIENT_ROYAL_NUM ] =
      {
        /* BTL_CLIENT_ROYAL1 */ BTL_SIDE_1ST,
        /* BTL_CLIENT_ROYAL2 */ BTL_SIDE_2ND,
        /* BTL_CLIENT_ROYAL3 */ BTL_SIDE_3RD,
        /* BTL_CLIENT_ROYAL4 */ BTL_SIDE_4TH,
      };
      return SIDE_BY_CLIENT_ROYAL[ clientId ];
    }

    return static_cast<BtlSide>( clientId & 1 );
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 立ち位置から、陣営内の位置インデックスを取得する
   * @param rule  バトルのルール
   * @param pos   立ち位置
   */
  //-------------------------------------------------------------------------
  u8 PokemonPosition::PosToSidePosIndex( BtlRule rule, BtlPokePos pos )
  {
    if( rule == BTL_RULE_ROYAL )
    {
      return 0;
    }

    return static_cast<u8>(pos / 2);
  }

  //-------------------------------------------------------------------------
  /**
   * @brief 味方の立ち位置を取得する
   * @param rule          バトルのルール
   * @param myBasePos     自身の基本位置
   * @param sidePosIndex  時陣営サイドの位置インデックス
   *
   * @attention
   * シングルバトル、バトルロイヤル　では、味方が存在しないため、必ず myBasePos を返します。
   */
  //-------------------------------------------------------------------------
  BtlPokePos PokemonPosition::GetFriendPokePos( BtlRule rule, BtlPokePos myBasePos, u8 sidePosIndex )
  {
    if( ( rule == BTL_RULE_SINGLE ) ||
        ( rule == BTL_RULE_ROYAL  ) )
    {
      return myBasePos;
    }

    if( rule == BTL_RULE_DOUBLE )
    {
      return ( myBasePos + ( sidePosIndex * 2 ) ) % BTL_POS_NUM;
    }

    if( rule == BTL_RULE_INTRUDE )
    {
      if( myBasePos == BTL_POS_1ST_0 )
      {
        return BTL_POS_1ST_0;
      }
      else
      {
        return ( myBasePos + ( sidePosIndex * 2 ) ) % BTL_POS_NUM;
      }
    }

    return myBasePos;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 味方同士の位置かどうかを判定する
   *
   * @param   rule  バトルのルール
   * @param   pos1  判定対象の位置1
   * @param   pos2  判定対象の位置2
   *
   * @retval  true   味方同士の場合
   * @retval  false  味方同士でない場合
   */
  //---------------------------------------------------------------------------
  bool PokemonPosition::IsFriendPokePos( BtlRule rule, BtlPokePos pos1, BtlPokePos pos2 )
  {
    return PokemonPosition::PosToSide( rule, pos1 ) == PokemonPosition::PosToSide( rule, pos2 );
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 真正面の相手にあたる位置を取得する
   * @param rule  バトルのルール
   * @param pos   立ち位置
   */
  //---------------------------------------------------------------------------
  BtlPokePos PokemonPosition::GetFacedPokePos( BtlRule rule, BtlPokePos pos )
  {
    switch( rule )
    {
    case BTL_RULE_SINGLE:
    default:
      return PokemonPosition::GetFacedPokePos_Single( pos );

    case BTL_RULE_DOUBLE:
      return PokemonPosition::GetFacedPokePos_Double( pos );

    case BTL_RULE_ROYAL:
      return PokemonPosition::GetFacedPokePos_Royal( pos );

    case BTL_RULE_INTRUDE:
      return PokemonPosition::GetFacedPokePos_Intrude( pos );

    }
  }

  /**
   * @brief 真正面の相手にあたる位置を取得する( シングルバトル )
   * @param pos   立ち位置
   */
  BtlPokePos PokemonPosition::GetFacedPokePos_Single( BtlPokePos pos )
  {
    return PokemonPosition::GetOpponentPokePos( BTL_RULE_SINGLE, pos, 0 );
  }

  /**
   * @brief 真正面の相手にあたる位置を取得する( ダブルバトル )
   * @param pos   立ち位置
   */
  BtlPokePos PokemonPosition::GetFacedPokePos_Double( BtlPokePos pos )
  {
    u8 idx = PokemonPosition::PosToSidePosIndex( BTL_RULE_DOUBLE, pos );
    idx ^= 1;
    return PokemonPosition::GetOpponentPokePos( BTL_RULE_DOUBLE, pos, idx );
  }

  /**
   * @brief 真正面の相手にあたる位置を取得する( ロイヤルバトル )
   * @param pos   立ち位置
   */
  BtlPokePos PokemonPosition::GetFacedPokePos_Royal( BtlPokePos pos )
  {
    static const BtlPokePos FACED_POS[ BTL_POS_ROYAL_NUM ] = 
    {
      BTL_POS_ROYAL_3RD,
      BTL_POS_ROYAL_4TH,
      BTL_POS_ROYAL_1ST,
      BTL_POS_ROYAL_2ND,
    };

    if( BTL_POS_ROYAL_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return pos;
    }

    return FACED_POS[ pos ];
  }

  /**
   * @brief 真正面の相手にあたる位置を取得する( 乱入 )
   * @param pos   立ち位置
   */
  BtlPokePos PokemonPosition::GetFacedPokePos_Intrude( BtlPokePos pos )
  {
    static const BtlPokePos FACED_POS[ BTL_POS_NUM ] = 
    {
      BTL_POS_2ND_1,
      BTL_POS_1ST_0,
      BTL_POS_2ND_0,
      BTL_POS_1ST_0,
    };

    if( BTL_POS_NUM <= pos ) 
    {
      GFL_ASSERT(0);
      return pos;
    }

    return FACED_POS[ pos ];
  }



GFL_NAMESPACE_END( btl )
