#if !defined( __APP_BOX_SEARCH_UPPER_DRAW_H_INCLUDED__ )
#define __APP_BOX_SEARCH_UPPER_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    BoxSearchUpperDraw.h
 * @date    2015/07/27 12:58:24
 * @author  fukushima_yuya
 * @brief   ボックス検索：上画面
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <App/BoxSearch/include/BoxSearchAppParam.h>
#include <App/BoxSearch/source/BoxSearchConst.h>

#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )
class DrawerBase;
class UpperMsg;
GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( BoxSearch )

//------------------------------------------------------------------------------
// @brief   技教え・技忘れ下画面描画
//------------------------------------------------------------------------------
class BoxSearchUpperDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN( BoxSearchUpperDraw );

public:
  // 初期化パラメータ
  struct InitParam
  {
    APP_PARAM*          appParam;
    void*               lytBuff;
    gfl2::str::MsgData* msgData_Main;
    gfl2::str::MsgData* msgData_Tokusei;
    print::WordSet*     wordSet;
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  BoxSearchUpperDraw( app::util::Heap* pHeap,app::util::AppRenderingManager* pAppRanderingManager );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~BoxSearchUpperDraw( void ) { ; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   初期化処理
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Init( InitParam& param );

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
  bool IsExit( void ) { return true; }

private:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void Initialize2D( InitParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  void InitializeClass( InitParam& param );

public:
  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(検索項目リスト用)
   */
  //------------------------------------------------------------------
  void SetMessage_ItemList( void );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(頭文字選択用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  void SetMessage_Initial( u32 listID );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(ガイド用)
   *
   * @param   listID    リストの種類
   */
  //------------------------------------------------------------------
  void SetMessage_Guide( u32 listID );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(タイプ用)
   *
   * @param   listID  リストの種類
   * @param   type1   タイプ１
   * @param   type2   タイプ２
   */
  //------------------------------------------------------------------
  void SetMessage_Type( u32 listID, u8 type1, u8 type2 );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(特性用)
   *
   * @param   tokusei   特性No
   */
  //------------------------------------------------------------------
  void SetMessage_Tokusei( TokuseiNo tokusei );

  //------------------------------------------------------------------
  /**
   * @brief   メッセージの設定(性格用)
   *
   * @param   seikaku   性格ID
   */
  //------------------------------------------------------------------
  void SetMessage_Seikaku( pml::pokepara::Seikaku seikaku );

private:
  app::util::Heap*                  m_pHeap;        //!< ヒープ

  app::util::AppRenderingManager*   m_pAppRenderingManager;

  APP_PARAM*                        m_pAppParam;

private:
  DrawerBase*     m_pDrawerBase;

  UpperMsg*       m_pUpperMsg;

  u8              m_Seq;
};


GFL_NAMESPACE_END( BoxSearch )
GFL_NAMESPACE_END( App )

#endif // __APP_BOX_SEARCH_UPPER_DRAW_H_INCLUDED__