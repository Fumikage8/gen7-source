#ifndef NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_PARTS_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_PARTS_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListJoinUpperParts.h
 * @author  fukushima_yuya
 * @date    2015.05.13
 * @brief   ポケモンリスト参加選択上画面パーツ
 */
//==============================================================================

// niji
#include "App/PokeList/source/PokeListConst.h"
#include "App/PokeList/include/PokeListSetupParam.h"


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 描画共通処理
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   制限時間
//------------------------------------------------------------------------------
class JoinTimeLimit
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinTimeLimit );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  JoinTimeLimit( DrawerBase* pBase );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~JoinTimeLimit( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   制限時間の表示を更新
   */
  //------------------------------------------------------------------
  void UpdateTime( void );

private:
  // 描画共通処理
  DrawerBase*   m_pBase;

  // テキストボックス
  gfl2::lyt::LytTextBox*    m_pTitleTextBox;

  gfl2::lyt::LytTextBox*    m_pTimeTextBox;
};


//------------------------------------------------------------------------------
// @brief   ポケモンプレート
//------------------------------------------------------------------------------
class JoinPokePlate
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinPokePlate );

public:
  enum PlateColor
  {
    COL_BLUE,   //!< 自分 or 味方
    COL_RED,    //!< 相手 or 敵
    COL_NUM
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  JoinPokePlate( DrawerBase* pBase, PartyID partyID, IntermediateData* pParty, u8 index, JoinUppMode uppMode );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  ~JoinPokePlate( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   */
  //------------------------------------------------------------------
  void Init( void );

  //------------------------------------------------------------------
  /**
   * @brief   プレイヤー名のセット
   *
   * @param   partyID   パーティID
   */
  //------------------------------------------------------------------
  void SetPlayerName( PartyID partyID );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   */
  //------------------------------------------------------------------
  void SetPokeIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx );

  //------------------------------------------------------------------
  /**
   * @brief   道具アイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetItemIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   性別アイコンのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetGenderIcon( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンのレベルのセット
   *
   * @param   pp          ポケモンデータ
   * @param   memberIdx   メンバーインデックス
   * @param   isEgg       タマゴフラグ
   */
  //------------------------------------------------------------------
  void SetLevel( const pml::pokepara::PokemonParam* pp, u8 memberIdx, bool isEgg );

  //------------------------------------------------------------------
  /**
   * @brief   QRアイコンのセット
   */
  //------------------------------------------------------------------
  void SetQRIcon( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   プレートの色の設定
   * @param   color   PlateColorを指定(青 or 赤)
   */
  //------------------------------------------------------------------
  void SetPlateColor( PlateColor color );

private:
  // 描画共通処理
  DrawerBase*         m_pBase;

  PartyID             m_PartyID;

  IntermediateData*   m_pPartyData;

  u8    m_PlateIndex;

  u8    m_MemberCount;

  // プレイヤー名
  gfl2::lyt::LytTextBox*    m_pPlayerName;

  // セット
  gfl2::lyt::LytPane*       m_pPokeSet[PLATE_NUM];

  // ポケモンアイコン
  gfl2::lyt::LytParts*      m_pPokeIconParts[PLATE_NUM];
  gfl2::lyt::LytPicture*    m_pPokeIconPane[PLATE_NUM];

  // 道具アイコン
  gfl2::lyt::LytPane*       m_pItemIconPane[PLATE_NUM];

  // 性別アイコン
  gfl2::lyt::LytPane*       m_pGenderIcon[PLATE_NUM];

  // レベル
  gfl2::lyt::LytTextBox*    m_pLvMsgTextBox[PLATE_NUM];
  gfl2::lyt::LytTextBox*    m_pLvNumTextBox[PLATE_NUM];

  // QRアイコン
  gfl2::lyt::LytPane*       m_pQRIconPane;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_PARTS_H_INCLUDED
