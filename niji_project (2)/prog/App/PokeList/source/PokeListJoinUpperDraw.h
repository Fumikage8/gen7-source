#ifndef NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListJoinUpperDraw.h
 * @author  fukushima_yuya
 * @date    2015.05.13
 * @brief   ポケモンリスト参加選択上画面
 */
//==============================================================================

// niji
#include "App/PokeList/include/PokeListAppParam.h"
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/PokeList/source/PokeListConst.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   参加選択上画面
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class DrawerBase;   //!< 描画共通処理
class LocalWork;    //!< ローカルワーク
class StaticWork;   //!< スタティックワーク
class JoinTimeLimit;
class JoinPokePlate;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//------------------------------------------------------------------------------
// @brief   参加選択上画面
//------------------------------------------------------------------------------
class PokeListJoinUpperDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeListJoinUpperDraw );

public:
  // シーケンス
  enum Sequence
  {
    SEQ_INIT,
    SEQ_READY,
    SEQ_IN,
    SEQ_MAIN,
    SEQ_OUT,
    SEQ_END_READY,
    SEQ_END
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  PokeListJoinUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeListJoinUpperDraw( void ) { ; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   pStaticWork   スタティックワーク
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, StaticWork* pStaticWork, LocalWork* pLocalWork );

  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  bool End( void );

  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

  //------------------------------------------------------------------
  /**
   * @brief   描画
   */
  //------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のオープン
   */
  //------------------------------------------------------------------
  void PokeListOpen( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面のクローズ
   */
  //------------------------------------------------------------------
  void PokeListClose( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @param   in    インアニメかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( bool in );

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト表示
   */
  //------------------------------------------------------------------
  void InitializeText( void );

  //------------------------------------------------------------------
  /**
   * @brief   パーティの表示順の設定
   */
  //------------------------------------------------------------------
  void SetPartyOrder( void );

  //------------------------------------------------------------------
  /**
   * @brief   パーティの表示プレートの生成
   */
  //------------------------------------------------------------------
  void CreatePartyPlate( void );

  //------------------------------------------------------------------
  /**
   * @brief   待機アニメーションの再生
   */
  //------------------------------------------------------------------
  void StartWaitingAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   制限時間アラーとの表示
   */
  //------------------------------------------------------------------
  void StartAlartAnim( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メインシーケンスかどうか
   *
   * @return  "true  = メインシーケンス"
   * @return  "false = それ以外"
   */
  //------------------------------------------------------------------
  bool IsSeqMain( void ) const { return (m_Seq == SEQ_MAIN); }

private:
  static const u8 UPP_JOIN_PARTY_MAX = 3;

private:
  app::util::Heap*      m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*    m_pFadeManager;

  u8    m_Seq;

private:
  // 描画共通処理
  DrawerBase*       m_pDrawerBase;

  // スタティックワーク
  StaticWork*       m_pStaticWork;

  // ローカルワーク
  LocalWork*        m_pLocalWork;

  // 制限時間
  JoinTimeLimit*    m_pTimeLimit;

  // プレート
  JoinPokePlate*    m_pPokePlate[UPP_JOIN_PARTY_MAX];

  // パーティ表示順
  u8                m_PartyID[PARTY_NUM];

  // パーティの表示数
  u8                m_PartyCount;

  // 待機フラグ
  u8                m_WaitFlag[PARTY_NUM];

  // アラートフラグ
  bool              m_AlartFlag;
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif // #ifndef NIJI_PROJECT_APP_POKELIST_JOIN_UPPER_DRAW_H_INCLUDED
