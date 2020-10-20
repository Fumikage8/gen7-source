#if !defined( __RESORT_LOWER_BEANS_SELECT_DRAW_H__ )
#define __RESORT_LOWER_BEANS_SELECT_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    ResortLowerBeansSelectDraw.h
 * @date    2015/09/10 13:37:39
 * @author  fukushima_yuya
 * @brief   ポケリゾート：ポケマメ選択画面(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "ResortLowerBaseDraw.h"
#include <App/Resort/source/ResortConst.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   カウンタークラス
//------------------------------------------------------------------------------
class DialCounter
{
  GFL_FORBID_COPY_AND_ASSIGN(DialCounter);

public:
  enum DialType {
    DIAL_TYPE_BONUS,
    DIAL_TYPE_TRADE,
    DIAL_TYPE_NUM,
  };
  enum DialBtnType {
    DIAL_BTN_TYPE_UP_10,
    DIAL_BTN_TYPE_UP_1,
    DIAL_BTN_TYPE_DW_10,
    DIAL_BTN_TYPE_DW_1,
    DIAL_BTN_TYPE_NUM,
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   base    描画共通処理
   * @param   index   カウンターのインデックス
   */
  //------------------------------------------------------------------
  DialCounter( DrawerBase* base, u32 index );

  //------------------------------------------------------------------
  /**
   * @brief    デストラクタ
   */
  //------------------------------------------------------------------
  ~DialCounter( void ) { ; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   *
   * @param   index   カウンターのインデックス
   */
  //------------------------------------------------------------------
  void InitializePane( u32 index );

public:
  //------------------------------------------------------------------
  /**
   * @brief   パラメータのリセット
   */
  //------------------------------------------------------------------
  void ResetParam( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ダイアルタイプの設定
   */
  //------------------------------------------------------------------
  inline void SetDialType( DialType type ) { m_DialType = type; }

  //------------------------------------------------------------------
  /**
   * @brief   所持ポケマメ数の設定
   *
   * @param   num   所持ポケマメ数
   */
  //------------------------------------------------------------------
  inline void SetHaveBeansNum( u32 num ) {
    m_HaveNumMax = num;
    m_HaveNum = num;
  }

  //------------------------------------------------------------------
  /**
   * @brief   全体でセットできるポケマメ数の設定
   *
   * @param   num   全体でセットできるポケマメ数
   */
  //------------------------------------------------------------------
  inline void SetUseBeansNum( u32 num ) { m_UseNumMax = num; }

  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ使用数を取得
   *
   * @return  ポケマメ使用数
   */
  //------------------------------------------------------------------
  inline u32 GetUseNum( void ) const { return m_UseNum; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   ダイヤル操作
   *
   * @param   type      ダイヤルボタンのタイプ
   * @param   useNum    全体の使用数
   */
  //------------------------------------------------------------------
  void Dial( DialBtnType type, u32 useNum );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ダイヤル操作
   *
   * @param   useMax    セットできる最大数
   */
  //------------------------------------------------------------------
  void Dial_Up_10( u32 useMax );
  void Dial_Up_1( u32 useMax );
  void Dial_Dw_10( u32 useMax );
  void Dial_Dw_1( u32 useMax );

public:
  //------------------------------------------------------------------
  /**
   * @brief   テキストのセット
   */
  //------------------------------------------------------------------
  void SetText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   テキストのセット
   */
  //------------------------------------------------------------------
  void SetText_HaveNum( void );
  void SetText_CounterNum( void );

private:
  static const u32 COUNTER_NUM_MAX = 99;
  static const u32 KETA_NUM = 2;

private:
  DrawerBase*     m_pBase;

  DialType        m_DialType;

  u32             m_UseNumMax;

  u32             m_HaveNumMax;

  u32             m_HaveNum;

  u32             m_UseNum;

private:
  gfl2::lyt::LytTextBox*      m_pTextHaveNum;

  gfl2::lyt::LytTextBox*      m_pTextCounterNum[KETA_NUM];
};


//------------------------------------------------------------------------------
// @brief   ポケリゾート：ポケマメ選択画面(下)
//------------------------------------------------------------------------------
class LowerBeansSelectDraw
  : public LowerBaseDraw
{
  GFL_FORBID_COPY_AND_ASSIGN(LowerBeansSelectDraw);

public:
  enum BeansKind {
    BEANS_KIND_NORMAL,
    BEANS_KIND_GARA,
    BEANS_KIND_NIJI,
    BEANS_KIND_NUM,
  };

  enum DialKind {
    DIAL_KIND_UP_10,
    DIAL_KIND_DW_10,
    DIAL_KIND_UP_1,
    DIAL_KIND_DW_1,
    DIAL_KIND_NUM,
  };

  enum CounterResult {
    RES_FAILED    = 0,
    RES_ADD_OK    = 1,
    RES_HAVE_OK   = 1<<1,
    RES_USE_OK    = 1<<2,
    RES_ALL_OK    = (RES_ADD_OK | RES_HAVE_OK | RES_USE_OK),
  };

public:
  // 更新用のパラメータ
  struct UpdateParam
  {
    BeansKind beansKind;
    DialKind  dialKind;
    s32       nowVal;
    s32       addVal;
    s32       haveVal;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   param   生成パラメータ
   */
  //------------------------------------------------------------------
  LowerBeansSelectDraw( LowerBaseDraw::CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~LowerBeansSelectDraw( void ) { ; }


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
   * @brief   初期テキストの設定
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ選択のタイプを設定
   *
   * @param   type   ポケマメ選択のタイプ
   */
  //------------------------------------------------------------------
  void SetBeansSelectType( u8 type ) { m_BeansSelectType = static_cast<BeansSelectType>(type); }

  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ選択を開く
   */
  //------------------------------------------------------------------
  virtual void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケマメ選択を閉じる
   */
  //------------------------------------------------------------------
  virtual void Close( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   決定処理
   */
  //------------------------------------------------------------------
  void DecideFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief   決定処理
   */
  //------------------------------------------------------------------
  void DecideFunc_Trade( void );      //!< ポケマメ交換用
  void DecideFunc_BonusTime( void );  //!< ボーナスタイム用

private:
  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup_Bonus( void );

  //------------------------------------------------------------------
  /**
   * @brief   セットアップ
   */
  //------------------------------------------------------------------
  void Setup_Trade( void );


  //------------------------------------------------------------------
  /**
   * @brief   所持ポケマメのセット
   */
  //------------------------------------------------------------------
  void SetHaveBeansNum( void );

  //------------------------------------------------------------------
  /**
   * @brief   選択画面のセット
   */
  //------------------------------------------------------------------
  void SetBeansSelectAnim( void );

  //------------------------------------------------------------------
  /**
   * @brief   イメージのセット
   */
  //------------------------------------------------------------------
  void SetImageAnim( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   カウンターの更新
   *
   * @param   beansKind   ポケマメの種類
   * @param   btnTYpe     ダイヤルボタンのタイプ
   */
  //------------------------------------------------------------------
  void UpdateCounter( BeansKind beansKind, DialCounter::DialBtnType btnType );

  //------------------------------------------------------------------
  /**
   * @brief   カウンターの更新(テキスト)
   *
   * @param   param   更新用パラメータ
   */
  //------------------------------------------------------------------
  void UpdateCounter_Text( UpdateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   追加する値の取得
   *
   * @param   dialKind    ダイヤルの種類
   */
  //------------------------------------------------------------------
  s32 GetAddValue( DialKind dialKind );

private:
  //------------------------------------------------------------------
  /**
   * @brief   テキストの更新
   */
  //------------------------------------------------------------------
  void UpdateText( void );

  //------------------------------------------------------------------
  /**
   * @brief   テキストの更新（使用数）
   */
  //------------------------------------------------------------------
  void UpdateText_UseBeansNum( void );

  //------------------------------------------------------------------
  /**
   * @brief   テキストの更新（追加タイム）
   */
  //------------------------------------------------------------------
  void UpdateText_BonusTime( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   決定ボタンの更新
   */
  //------------------------------------------------------------------
  void UpdateDecideBtn( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ポケマメの種類を取得
   *
   * @param   btnID   ボタンID
   *
   * @return  ポケマメの種類(ノーマル・ガラ・ニジ)
   */
  //------------------------------------------------------------------
  BeansKind GetBeansKind( u32 btnID );

  //------------------------------------------------------------------
  /**
   * @brief   ダイヤルの種類を取得
   *
   * @param   btnID   ボタンID
   *
   * @return  ダイヤルの種類
   */
  //------------------------------------------------------------------
  DialCounter::DialBtnType GetDialBtnType( u32 btnID );

public:
  //------------------------------------------------------------------
  /**
   * @brief   交換したポケマメ数取得
   *
   * @return  ポケマメの数
   */
  //------------------------------------------------------------------
  u32 GetTradePokeMameCount( void ) const { return m_UsedBeansNum; }


private:   //!< UIInputListener
  //------------------------------------------------------------------
  /**
   * @brief   ペインアクション通知用リスナ
   *
   * @param   paneID
   */
  //------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 paneID );


private:   //!< ボタンマネージャ
  //------------------------------------------------------------------
  /**
   * @brief   ボタンの個別セットアップ
   *
   * @param   btnMan    ボタンマネージャ
   */
  //------------------------------------------------------------------
  virtual void SetupButton( app::tool::ButtonManager* btnMan );

private:
  DialCounter*        m_pCounter[BEANS_KIND_NUM];

  BeansSelectType     m_BeansSelectType;

  s32                 m_HaveBeansNum[BEANS_KIND_NUM];

  s32                 m_UsedBeansNum;

  s32                 m_UsedBeansMax;

private:
  gfl2::lyt::LytTextBox*      m_pTextUseBeansNum;

  gfl2::lyt::LytTextBox*      m_pTextBonusTime;

  gfl2::lyt::LytTextBox*      m_pTextTitle;

  gfl2::lyt::LytTextBox*      m_pTextUseBeansTitle;

  gfl2::lyt::LytTextBox*      m_pTextBonusTimeTitle;

  gfl2::lyt::LytPane*         m_pPaneVisibleImage;
};

GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )

#endif // __RESORT_LOWER_BEANS_SELECT_DRAW_H__