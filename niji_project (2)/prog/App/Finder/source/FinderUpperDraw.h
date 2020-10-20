#ifndef NIJI_PROJECT_FIELD_FINDER_UPPER_DRAW_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_UPPER_DRAW_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderUpperDraw.h
 * @author  fukushima_yuya
 * @date    2015.05.26
 * @brief   ポケファインダー上画面
 */
//==============================================================================

// niji
#include "App/Finder/include/FinderAppParam.h"

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )
class FinderDrawListener;
class DrawerBase;
GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )

GFL_NAMESPACE_BEGIN(Savedata)
class PokeFinderSave;
GFL_NAMESPACE_END(Savedata)


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//------------------------------------------------------------------------------
// @brief   ポケファインダー上画面の描画環境
//------------------------------------------------------------------------------
class FinderUpperDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderUpperDraw );

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderUpperDraw( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRanderingManager, Savedata::PokeFinderSave* finderSave );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderUpperDraw( void ) { ; }

public:

  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   * @param   pListener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetUIListener( FinderDrawListener* pListener );

  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   * @param   pAppParam
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
   * @brief   準備完了か？
   */
  //------------------------------------------------------------------
  bool IsOpened( void ) const;

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   * @param   pHeap         ヒープ
   * @param   pAppLytBuff   レイアウトバイナリデータ
   */
  //------------------------------------------------------------------
  void Initialize2D( app::util::Heap* pHeap, void* pAppLytBuff );

public:
  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの再生
   */
  //------------------------------------------------------------------
  void StartCloseShutter( void );

  //------------------------------------------------------------------
  /**
   * @brief   シャッターアニメの再生
   */
  //------------------------------------------------------------------
  void OpenShutter( void );

  //------------------------------------------------------------------
  /**
   * @brief   照準の表示切替
   */
  //------------------------------------------------------------------
  void ChangeSight( void );

  //------------------------------------------------------------------
  /**
   * @brief   ピントの変更
   * @param   mode    ピントのモード
   */
  //------------------------------------------------------------------
  void SetPintMode( u8 mode );

  //------------------------------------------------------------------
  /**
   * @brief   ズーム処理
   */
  //------------------------------------------------------------------
  void CameraZoom( void );

  //------------------------------------------------------------------
  /**
   * @brief   情報表示の切り替え
   */
  //------------------------------------------------------------------
  void ChangeVisibleInfo( void );

private:
  //------------------------------------------------------------------
  /**
   * @brief   ロトムコメントの開始
   */
  //------------------------------------------------------------------
  void StartRotomComment( void );

  //------------------------------------------------------------------
  /**
   * @brief   ロトムコメントの更新
   */
  //------------------------------------------------------------------
  void UpdateRotomComment( void );

private:
  static const u32 SHUTTER_COUNT_MAX = 6;

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  FinderDrawListener*               m_uiListener;   //!< UIイベントリスナー

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  Savedata::PokeFinderSave*         m_pFinderSave;

  APP_PARAM*                        m_pAppParam;

  gfl2::str::MsgData*               m_pMsgData;

  print::WordSet*                   m_pWordSet;

  u8    m_Seq;
  
private:
  DrawerBase*               m_pDrawerBase;

  u8      m_Version;

  u8      m_SightID;

  u8      m_ShutterCount;

  u8      m_ShutterAnimSeq;

  u8      m_RotomSeq;

  u32     m_RotomAnim;

  u32     m_RotomFrame;

private:
  gfl2::lyt::LytPane*     m_pInfoVisible;

  gfl2::lyt::LytPane*     m_pShutterCntPane[SHUTTER_COUNT_MAX];

  gfl2::lyt::LytTextBox*  m_pRotomComment;
};

GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )


#endif  //#ifndef NIJI_PROJECT_FIELD_FINDER_UPPER_DRAW_H_INCLUDED
