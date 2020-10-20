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
#if !defined BTL_POKEMONPOSITION_H_INCLUDED
#define BTL_POKEMONPOSITION_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <Battle/include/battle_def.h>
#include <Battle/include/battle_SetupParam.h> // for BTL_CLIENT_ID

GFL_NAMESPACE_BEGIN( btl )


  //---------------------------------------------------------------------------
  /**
   * @brief ポケモン立ち位置( 画面上の見た目ではなく、概念上の位置 )
   */
  //---------------------------------------------------------------------------
  typedef enum 
  {
    BTL_POS_1ST_0,    ///< スタンドアローンならプレイヤー側、通信対戦ならサーバ側
    BTL_POS_2ND_0,
    BTL_POS_1ST_1,
    BTL_POS_2ND_1,
    BTL_POS_NUM,
    BTL_POS_NULL = BTL_POS_NUM,

    // バトルロイヤルのための定義
    BTL_POS_ROYAL_1ST = BTL_POS_1ST_0,
    BTL_POS_ROYAL_2ND = BTL_POS_2ND_0,
    BTL_POS_ROYAL_3RD = BTL_POS_1ST_1,
    BTL_POS_ROYAL_4TH = BTL_POS_2ND_1,
    BTL_POS_ROYAL_MIN = BTL_POS_ROYAL_1ST,                          ///< バトルロイヤルでの最小値
    BTL_POS_ROYAL_MAX = BTL_POS_ROYAL_4TH,                          ///< バトルロイヤルでの最大値
    BTL_POS_ROYAL_NUM = BTL_POS_ROYAL_MAX - BTL_POS_ROYAL_MIN + 1,  ///< バトルロイヤルでの位置の数

    BTL_EXIST_POS_MAX        = 6, ///< １度のバトルで有効になる立ち位置の最大数( トリプルで６になる )

  }BtlPokePos_tag;

  typedef u8  BtlPokePos;



  //---------------------------------------------------------------------------
  /**
   * @brief 陣営を現すID
   */
  //---------------------------------------------------------------------------
  typedef enum 
  {
    // 単一の陣営を表すID
    BTL_SIDE_1ST,   ///< スタンドアローンならプレイヤー側、通信対戦ならサーバ側
    BTL_SIDE_2ND,   
    BTL_SIDE_3RD,   ///< バトルロイヤルでのみ有効
    BTL_SIDE_4TH,   ///< バトルロイヤルでのみ有効

    BTL_SIDE_MIN  = BTL_SIDE_1ST,                    ///< 単一の陣営を表すIDの最小値
    BTL_SIDE_MAX  = BTL_SIDE_4TH,                    ///< 単一の陣営を表すIDの最大値
    BTL_SIDE_NUM  = BTL_SIDE_MAX - BTL_SIDE_MIN + 1, ///< 陣営の総数
    BTL_SIDE_NULL = BTL_SIDE_NUM,                    ///< 無効値

    // 複数の陣営を表すID
    BTL_MULTI_SIDE_ALL,           ///< 全ての陣営
    BTL_MULTI_SIDE_WITHOUT_1ST,   ///< BTL_SIDE_1ST 以外の全陣営
    BTL_MULTI_SIDE_WITHOUT_2ND,   ///< BTL_SIDE_2ND 以外の全陣営
    BTL_MULTI_SIDE_WITHOUT_3RD,   ///< BTL_SIDE_3RD 以外の全陣営
    BTL_MULTI_SIDE_WITHOUT_4TH,   ///< BTL_SIDE_4TH 以外の全陣営

    BTL_MULTI_SIDE_MIN = BTL_MULTI_SIDE_ALL,         ///< 複数の陣営を表すIDの最小値
    BTL_MULTI_SIDE_MAX = BTL_MULTI_SIDE_WITHOUT_4TH, ///< 複数の陣営を表すIDの最大値

  }BtlSide_tag;

  typedef u8 BtlSide;





  /**
   * @class PokemonPosition
   * @brief ポケモンの立ち位置( BtlPokePos ), 陣営( BtlSide )に関する静的な定義
   */
  class PokemonPosition
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokemonPosition );

  public:

    //-------------------------------------------------------------------------
    /**
     * @brief 指定クライアントの担当する位置の数を取得する
     * @param rule       バトルのルール
     * @param multiMode  マルチモード
     * @param clientId   チェック対象クライアントのID
     */
    //-------------------------------------------------------------------------
    static u32 GetClientCoverPosNum( BtlRule rule, BtlMultiMode multiMode, BTL_CLIENT_ID clientId );

    //-------------------------------------------------------------------------
    /**
     * @brief 立ち位置の担当クライアントを取得する
     * @param rule       バトルのルール
     * @param multiMode  マルチモード
     * @param pos        立ち位置
     */
    //-------------------------------------------------------------------------
    static BTL_CLIENT_ID GetPosCoverClientId( BtlRule rule, BtlMultiMode multiMode, BtlPokePos pos );

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
    static bool IsPokePosExist( BtlRule rule, BtlMultiMode multiMode, BtlPokePos pos );

    //-------------------------------------------------------------------------
    /**
     * @brief 現在のルールに応じて、有効になる立ち位置の最大値を返す
     * @param rule  バトルのルール
     */
    //-------------------------------------------------------------------------
    static BtlPokePos GetValidPosMax( BtlRule rule );

    //-------------------------------------------------------------------------
    /**
     * @brief 相手側のポケモン位置取得
     * @param rule               バトルのルール
     * @param myBasePos          自分側の基本位置
     * @param opponentPosIndex   相手側の何番目の位置を取得するか？
     */
    //-------------------------------------------------------------------------
    static BtlPokePos GetOpponentPokePos( BtlRule rule, BtlPokePos myBasePos, u8 opponentPosIndex );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した陣営について、前衛( ワザがあたる場所 )の数を返す
     * @param rule  バトルのルール
     * @param side  陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     */
    //-------------------------------------------------------------------------
    static u32 GetFrontPosNum( BtlRule rule, BtlSide side );

    //-------------------------------------------------------------------------
    /**
     * @brief 陣営から立ち位置を取得する
     * @param rule      バトルのルール
     * @param side      陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     * @param posIndex  陣営内の位置インデックス
     */
    //-------------------------------------------------------------------------
    static BtlPokePos GetSidePos( BtlRule rule, BtlSide side, u8 posIndex );

    //-------------------------------------------------------------------------
    /**
     * @brief 陣営が存在するか？
     * @param rule  バトルのルール
     * @param side  チェック対象の陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     * @retval true   陣営が存在する
     * @retval false  陣営が存在しない
     */
    //-------------------------------------------------------------------------
    static bool IsSideExist( BtlRule rule, BtlSide side );

    //-------------------------------------------------------------------------
    /**
     * @brief 存在する陣営の数を取得する？
     * @param rule  バトルのルール
     */
    //-------------------------------------------------------------------------
    static u8 GetSideNum( BtlRule rule );

    //-------------------------------------------------------------------------
    /**
     * @brief 複数の陣営を表す BtlSide( BTL_MULTI_SIDE_xxxx ) を、単一の陣営を表す BtlSide( BTL_SIDE_xxxx )に展開する
     * @param[out] expandSide     展開後の陣営の格納先配列( 要素数 BTL_SIDE_NUM 必要 )
     * @param[out] expandSideNum  展開後の陣営の数
     * @param      rule           バトルのルール
     * @param      targetSide     展開する陣営
     */
    //-------------------------------------------------------------------------
    static void ExpandSide( BtlSide* expandSide, u8* expandSideNum, BtlRule rule, BtlSide targetSide );

    //-------------------------------------------------------------------------
    /**
     * @brief 単一の陣営を表す BtlSide かどうかチェックする
     * @param side  チェック対象の陣営
     * @retval true    単一の陣営を表す BtlSide である
     * @retval false   単一の陣営を表す BtlSide でない
     */
    //-------------------------------------------------------------------------
    static bool IsUnitSide( BtlSide side );

    //-------------------------------------------------------------------------
    /**
     * @brief 複数の陣営を表す BtlSide かどうかチェックする
     * @param side  チェック対象の陣営
     * @retval true    複数の陣営を表す BtlSide である
     * @retval false   複数の陣営を表す BtlSide でない
     */
    //-------------------------------------------------------------------------
    static bool IsMultiSide( BtlSide side );

    //-------------------------------------------------------------------------
    /**
     * @brief クライアントの陣営を取得する
     * @param rule      バトルのルール
     * @param clientId  マルチモード
     * @retval [ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     */
    //-------------------------------------------------------------------------
    static BtlSide GetClientSide( BtlRule rule, BTL_CLIENT_ID clientId );

    //-------------------------------------------------------------------------
    /**
     * @brief 立ち位置から陣営取得する
     * @param rule  バトルのルール
     * @param pos   立ち位置
     * @retval [BTL_SIDE_1ST, BTL_SIDE_4TH]
     */
    //-------------------------------------------------------------------------
    static BtlSide PosToSide( BtlRule rule, BtlPokePos pos );

    //-------------------------------------------------------------------------
    /**
     * @brief 指定した陣営が、プレイヤの陣営を含むかどうかチェックする
     * @param playerSide  プレイヤの陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     * @param checkSide   チェック対象の陣営
     * @retval true   プレイヤの陣営を含む
     * @retval false  プレイヤの陣営を含まない
     */
    //-------------------------------------------------------------------------
    static bool IsPlayerSide( BtlSide playerSide, BtlSide checkSide );

    //-------------------------------------------------------------------------
    /**
     * @brief 敵陣営を取得する
     * @param rule  バトルのルール
     * @param side  自陣営[ BTL_SIDE_1ST, BTL_SIDE_4TH ]
     *
     * @retval [ BTL_SIDE_1ST,         BTL_SIDE_4TH         ] バトルロイヤル以外
     * @retval [ BTL_MULTI_SIDE_WITHOUT_1ST, BTL_MULTI_SIDE_WITHOUT_4TH ] バトルロイヤルの場合
     * @retval BTL_SIDE_NULL  引数が不正値の場合
     */
    //-------------------------------------------------------------------------
    static BtlSide GetOpponentSide( BtlRule rule, BtlSide side );

    //-------------------------------------------------------------------------
    /**
     * @brief 立ち位置から、陣営内の位置インデックスを取得する
     * @param rule  バトルのルール
     * @param pos   立ち位置
     */
    //-------------------------------------------------------------------------
    static u8 PosToSidePosIndex( BtlRule rule, BtlPokePos pos );

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
    static BtlPokePos GetFriendPokePos( BtlRule rule, BtlPokePos myBasePos, u8 sidePosIndex );

    //-------------------------------------------------------------------------
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
    //-------------------------------------------------------------------------
    static bool IsFriendPokePos( BtlRule rule, BtlPokePos pos1, BtlPokePos pos2 );

    //-------------------------------------------------------------------------
    /**
     * @brief 真正面の相手にあたる位置を取得する
     * @param rule  バトルのルール
     * @param pos   立ち位置
     */
    //-------------------------------------------------------------------------
    static BtlPokePos GetFacedPokePos( BtlRule rule, BtlPokePos pos );

    



  private:
    static BTL_CLIENT_ID GetPosCoverClientId_Single( BtlPokePos pos );
    static BTL_CLIENT_ID GetPosCoverClientId_Double( BtlMultiMode multiMode, BtlPokePos pos );
    static BTL_CLIENT_ID GetPosCoverClientId_Royal( BtlPokePos pos );
    static BTL_CLIENT_ID GetPosCoverClientId_Intrude( BtlPokePos pos );
    static BtlPokePos GetFacedPokePos_Single( BtlPokePos pos );
    static BtlPokePos GetFacedPokePos_Double( BtlPokePos pos );
    static BtlPokePos GetFacedPokePos_Royal( BtlPokePos pos );
    static BtlPokePos GetFacedPokePos_Intrude( BtlPokePos pos );
    static void AddSideIfExist( BtlSide* destArray, u8* count, BtlRule rule, BtlSide side );


  private:
    PokemonPosition( void );
    ~PokemonPosition();

  };


GFL_NAMESPACE_END( btl )

#endif // BTL_POKEMONPOSITION_H_INCLUDED