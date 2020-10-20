#if !defined( __RESORT_UPPER_DRAW_H__ )
#define __RESORT_UPPER_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortUpperDraw.h
 * @date    2015/09/08 19:15:25
 * @author  fukushima_yuya
 * @brief   ポケリゾート：メイン画面(上)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortUpperBaseDraw.h"

#include <App/Resort/source/ResortConst.h>

#include <PokeTool/include/PokeTool.h>
#include <Savedata/include/ResortSave.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   定数
//------------------------------------------------------------------------------
static const u32 FUREAI_WILD_MAX  = Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX;
static const u32 FARM_MAX         = Savedata::ResortSave::RESORT_FARM_MAX;
static const u32 GIM_KIND_MAX     = Savedata::ResortSave::RESORT_KIND_GIM;
static const u32 GIM_POKE_MAX     = Savedata::ResortSave::STOCKPOKEMON_MAX;
static const u32 HOTSPA_POKE_MAX  = Savedata::ResortSave::STOCKPOKEMON_MAX;


//------------------------------------------------------------------------------
// @brief   ポケリゾート：メイン画面(上)
//------------------------------------------------------------------------------
class UpperDraw
  : public UpperBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperDraw);

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  UpperDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperDraw( void ) { ; }


private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param );
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void );
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化：各リゾート
   */
  //------------------------------------------------------------------
  void InitializePane_Friendship( void );     //!< ふれあい
  void InitializePane_Farm( void );           //!< きのみ
  void InitializePane_Adventure( void );      //!< たんけん
  void InitializePane_Gim( void );            //!< うんどう
  void InitializePane_Hotspa( void );         //!< おんせん

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化：ベースペイン
   *
   * @param   resortID    リゾートID
   */
  //------------------------------------------------------------------
  void InitializePane_BasePane( u8 resortID );

  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化：ボーナスタイム未設定テキスト
   *
   * @param   resortID    リゾートID
   */
  //------------------------------------------------------------------
  void InitializePane_BonusTimeAlert( u8 resortID );


public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートのセットアップ
   *
   * @param   resortID    リゾートID
   */
  //------------------------------------------------------------------
  void SetupResort( u8 resortID );

  //------------------------------------------------------------------
  /**
   * @brief   リゾートのセットアップ：各リゾート
   */
  //------------------------------------------------------------------
  void SetupResort_Friendship( void );      //!< ふれあい
  void SetupResort_Farm( void );            //!< きのみ
  void SetupResort_Adventure( void );       //!< たんけん
  void SetupResort_Gim( void );             //!< うんどう
  void SetupResort_Hotspa( void );          //!< おんせん

public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートのセットアップ以外のセットアップ
   */
  //------------------------------------------------------------------
  void SetupResortOther_Adventure( void );


public:
  //------------------------------------------------------------------
  /**
   * @brief   更新
   */
  //------------------------------------------------------------------
  virtual void Update( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートの更新
   *
   * @param   resortID    リゾートID
   *
   * @note    デフォルトはRESORT_ID_MAXを指定
   *          RESORT_ID_MAXの場合は特定のリゾート以外は更新を行わない
   *          それ以外のIDが指定された場合は指定されたリゾートを強制的に更新する
   */
  //------------------------------------------------------------------
  void UpdateResort( u8 resortID = RESORT_ID_MAX );

private:
  //------------------------------------------------------------------
  /**
   * @brief   時間の更新
   */
  //------------------------------------------------------------------
  void UpdateTime( void );

  //------------------------------------------------------------------
  /**
   * @brief   時間の更新：各リゾート
   */
  //------------------------------------------------------------------
  void UpdateTime_Friendship( void );     //!< ふれあい
  void UpdateTime_Farm( void );           //!< きのみ
  void UpdateTime_Adventure( void );      //!< たんけん
  void UpdateTime_Gim( void );            //!< うんどう
  void UpdateTime_Hotspa( void );         //!< おんせん


public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾートIDの設定
   *
   * @param   resortID    最初に開くリゾートID
   */
  //------------------------------------------------------------------
  void SetFirstResortID( u8 resortID ) { m_ResortID = resortID; }

  //------------------------------------------------------------------
  /**
   * @brief   リゾートの変更
   * @param   index リゾートインデックス
   * @param   dir   移動方向(0: 左、1: 右)
   */
  //------------------------------------------------------------------
  void FeedResort( u8 index, u8 dir );


private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンペインデータのセット
   */
  //------------------------------------------------------------------
  void SetPokePaneData( PokePaneData* paneData, const u32* paneTbl );

  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモンの出現数の取得
   *
   * @return  "野生ポケモンの出現数"
   */
  //------------------------------------------------------------------
  u8 GetWildPokeAppearCount( void );

  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモンがいるかどうか
   *
   * @param   index   インデックス
   *
   * @return  "true  == いる"
   * @return  "false == いない"
   */
  //------------------------------------------------------------------
  bool IsWildPokemon( u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   リゾートのタイプ設定
   *
   * @param   resortID    リゾートID
   */
  //------------------------------------------------------------------
  void SetResortType( u8 resortID );

  //------------------------------------------------------------------
  /**
   * @brief   各リゾートのポケモンパラメータを生成
   *
   * @param   resortID    リゾートID
   * @param   index       インデックス
   */
  //------------------------------------------------------------------
  pml::pokepara::CoreParam* CreatePokeParam( u8 resortID, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンアイコンのセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   resortID  リゾートID
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetPokeIcon( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモン種族名のセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetMonsName( const pml::pokepara::CoreParam* cp, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの性格のセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetSeikaku( const pml::pokepara::CoreParam* cp, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンの性別のセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   resortID  リゾートID
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetGender( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンのレベルのセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   resortID  リゾートID
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetLevel( const pml::pokepara::CoreParam* cp, u8 resortID, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   野生ポケモンのイベント状態のセット
   *
   * @param   cp        ポケモンパラメータ
   * @param   index     インデックス
   */
  //------------------------------------------------------------------
  void SetWildPokeEventState( const pml::pokepara::CoreParam* cp, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   ペインデータの取得
   *
   * @param   resortID    リゾートID
   * @param   index       インデックス
   */
  //------------------------------------------------------------------
  PokePaneData* GetPaneData( u8 resortID, u32 index );


private:
  //------------------------------------------------------------------
  /**
   * @brief   タイムの表示
   *
   * @param   time      表示する時間
   * @param   timeType  表示箇所
   */
  //------------------------------------------------------------------
  void SetTimeText( TimeParam time, u8 timeType );

  //------------------------------------------------------------------
  /**
   * @brief   タイムのテキストカラー変更
   *
   * @param   bonusTime   ボーナスタイム
   * @param   timeType    変更箇所
   */
  //------------------------------------------------------------------
  void SetTextColor( TimeParam bonusTime, u8 timeType );

  //------------------------------------------------------------------
  /**
   * @brief   次に終わるグループ名の表示：うんどうリゾート
   *
   * @param   index   グループインデックス
   */
  //------------------------------------------------------------------
  void SetNextGimGroupText( u32 index );

private:
  //------------------------------------------------------------------
  /**
   * @brief   一番小さい時間の取得
   *
   * @param   resort    [in]リゾートの種類
   * @param   time      [out]一番小さい時間
   * @param   index     [out]一番小さい時間のインデックス
   */
  //------------------------------------------------------------------
  void GetMinimumTime( u32 resort, TimeParam* time, u32* index );

  //------------------------------------------------------------------
  /**
   * @brief   最小時間と比較する時間の取得
   *
   * @param   resort  リゾートインデックス
   * @param   index   インデックス
   *
   * @return  比較する時間
   */
  //------------------------------------------------------------------
  TimeParam GetCheckMinTime( u32 resort, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief   最小時間の更新かどうか
   *
   * @param   minTime   最小時間
   * @param   time      比較する時間
   *
   * @return  "true  == 更新"
   * @return  "false == それ以外"
   */
  //------------------------------------------------------------------
  bool IsMinimumTime( TimeParam minTime, TimeParam time ) const;


public:
  //------------------------------------------------------------------
  /**
   * @brief   リゾート画面のレベル更新
   */
  //------------------------------------------------------------------
  void UpdateResortLevel( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンパラメータの取得
   * @param   cp    [in]コアパラメータ
   * @param   sp    [out]描画用パラメータ(PokeTool::SimpleParam)
   * @param   lv    [out]レベル値
   * @return  "true" = パラメータあり
   * @return  "false" = パラメータなし
   */
  //------------------------------------------------------------------
  bool GetPokeParam( pml::pokepara::CoreParam* cp, PokeTool::SimpleParam* sp, u32* lv );


private:
  // タイム表示箇所の種類
  enum {
    TIME_KIND_HUREAI_BONUS,
    TIME_KIND_KINOMI_NEXT,
    TIME_KIND_KINOMI_BONUS,
    TIME_KIND_TANKEN_BACK,
    TIME_KIND_TANKEN_BONUS,
    TIME_KIND_UNDOU_FINISH,
    TIME_KIND_UNDOU_BONUS,
    TIME_KIND_ONSEN_BONUS,
    TIME_KIND_NUM,
  };
  // 次の～アラートの種類
  enum {
    NEXT_ALERT_FARM,
    NEXT_ALERT_ADV,
    NEXT_ALERT_GIM,
    NEXT_ALERT_NUM,
  };

private:
  static const u32 ADV_VISIBLE_NUM   = 2;

private:
  u8      m_ResortID;

  u32     m_NextFarmIndex;    //!< 次に収穫する木の実のインデックス

  bool    m_AdventureEnable;

private:
  PokeTool::SimpleParam   m_SimpleParam_Gim[GIM_POKE_MAX];

  u32                     m_LevelNum_Gim[GIM_POKE_MAX];

private:
  PokePaneData    m_FureaiPokePaneData[FUREAI_WILD_MAX];    //!< ふれあい用

  PokePaneData    m_GimPokePaneData[GIM_POKE_MAX];          //!< 運動用

  PokePaneData    m_HotSpaPokePaneData[HOTSPA_POKE_MAX];    //!< おんせん用

private:
  gfl2::lyt::LytPane*     m_pPaneResortBase[RESORT_ID_MAX];         //!< 各リゾートの親ペイン

  gfl2::lyt::LytTextBox*  m_pTextTime[TIME_KIND_NUM];               //!< 時間表記用テキストボックス

  gfl2::lyt::LytTextBox*  m_pTextAlertBonusTime[RESORT_ID_MAX];     //!< ポケマメアラートテキスト

  gfl2::lyt::LytTextBox*  m_pTextAlertNext[NEXT_ALERT_NUM];         //!< つぎの～アラートテキスト

private:
  gfl2::lyt::LytTextBox*  m_pTextAlert_Friendship;

private:
  gfl2::lyt::LytPicture*  m_pPicNextIcon_Farm;

  gfl2::lyt::LytTextBox*  m_pTextNextName_Farm;

  gfl2::lyt::LytPane*     m_pPaneNextVisible_Farm;

private:
  gfl2::lyt::LytTextBox*  m_pTextGuide_Adv;

  gfl2::lyt::LytTextBox*  m_pTextCourseName_Adv;

  gfl2::lyt::LytPane*     m_pPaneVisible_Adv[ADV_VISIBLE_NUM];

private:
  gfl2::lyt::LytTextBox*  m_pTextNextName_Gim;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_UPPER_DRAW_H__