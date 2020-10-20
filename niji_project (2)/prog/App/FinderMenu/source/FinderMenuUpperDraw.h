#if !defined( __FINDER_MENU_UPPER_DRAW_H__ )
#define __FINDER_MENU_UPPER_DRAW_H__
#pragma once
//======================================================================
/**
 * @file    FinderMenuUpperDraw.h
 * @date    2015/11/02 18:55:10
 * @author  fukushima_yuya
 * @brief   ファインダーメニュー上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/FinderMenu/include/FinderMenuAppParam.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)
class FinderMenuDrawListener;
class DrawerBase;
GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(Fade)
class FadeManager;
GFL_NAMESPACE_END(Fade)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
class GameData;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(TrialModel)
struct FinderPhotoData;
struct FinderPhotoSnap;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderMenu)

//------------------------------------------------------------------------------
// @brief   ファインダーメニュー上画面
//------------------------------------------------------------------------------
class FinderMenuUpperDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderMenuUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderMenuUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderMenuUpperDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( FinderMenuDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam     外部設定パラメータ
   * @param   pAppLytBuff   レイアウトバイナリ
   * @param   msgData       メッセージデータ
   * @param   wordSet       ワードセット
   */
  //------------------------------------------------------------------
  void Init( APP_PARAM* pAppParam, void* pAppLytBuff, gfl2::str::MsgData* msgData, print::WordSet* wordSet );

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
   * @brief   オープン
   */
  //------------------------------------------------------------------
  void Open( void );

  //------------------------------------------------------------------
  /**
   * @brief   クローズ
   */
  //------------------------------------------------------------------
  void Close( void );

  //------------------------------------------------------------------
  /**
   * @brief   イン・アウトアニメが終了したかどうか
   * @return  true = 終了、false = 継続
   */
  //------------------------------------------------------------------
  bool IsEndInOutAnim( void );

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
   * @brief   写真データのセットアップ
   */
  //------------------------------------------------------------------
  void SetupPhotoData( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   写真選択位置の設定
   * @param   index   選択位置
   */
  //------------------------------------------------------------------
  void SetSelectPhoto( u32 index );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  FinderMenuDrawListener*           m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  gfl2::Fade::FadeManager*          m_pFadeManager;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  APP_PARAM*                        m_pAppParam;

  u8    m_Seq;

private:
  DrawerBase*               m_pDrawerBase;

  GameSys::GameData*        m_pGameData;

private:
  static const u8   PHOTO_DATA_MAX = 6;
  static const u8   SNAP_BUF_NUM   = 2;

  gfl2::lyt::LytPicture*    m_pPhotoPicture;

  Field::TrialModel::FinderPhotoData*       m_pPhotoData[PHOTO_DATA_MAX];

  Field::TrialModel::FinderPhotoSnap*       m_pPhotoSnap[SNAP_BUF_NUM];

  u8    m_PhotoNum;

  u8    m_SnapFlag;
};

GFL_NAMESPACE_END(FinderMenu)
GFL_NAMESPACE_END(App)

#endif // __FINDER_MENU_UPPER_DRAW_H__