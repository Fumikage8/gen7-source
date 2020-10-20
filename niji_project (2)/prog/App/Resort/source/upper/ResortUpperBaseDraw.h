#if !defined( __RESORT_UPPER_BASE_DRAW_H_INCLUDED__ )
#define __RESORT_UPPER_BASE_DRAW_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    ResortUpperBaseDraw.h
 * @date    2016/02/10 18:52:30
 * @author  fukushima_yuya
 * @brief   ポケリゾート：上画面Drawのベースクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <AppLib/include/Ui/UIView.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)
class ResortDrawListener;
class DrawerBase;
class ResortInitLogic;
GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

GFL_NAMESPACE_BEGIN( Savedata )
class ResortSave;
class BoxPokemon;
class ZukanData;
GFL_NAMESPACE_END( Savedata )


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

//------------------------------------------------------------------------------
// @brief   ポケリゾート：上画面Drawのベースクラス
//------------------------------------------------------------------------------
class UpperBaseDraw
  : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(UpperBaseDraw);

public:
  // 生成パラメータ
  struct CreateParam
  {
    app::util::Heap*                lytHeap;
    app::util::Heap*                iconHeap;
    app::util::AppRenderingManager* renderMan;
    gfl2::fs::AsyncFileManager*     fileMan;
    ResortInitLogic*                initLogic;
    Savedata::ResortSave*           resortSave;
    Savedata::BoxPokemon*           boxPokeSave;
    Savedata::ZukanData*            zukanSave;
    
    CreateParam( void )
      : lytHeap(NULL)
      , iconHeap(NULL)
      , renderMan(NULL)
      , fileMan(NULL)
      , initLogic(NULL)
      , resortSave(NULL)
      , boxPokeSave(NULL)
      , zukanSave(NULL)
    { ; }
  };
  // 初期化パラメータ
  struct InitParam
  {
    void*                 lytBuff;    //!< リゾートレイアウト
    gfl2::str::MsgData*   msgSys;     //!< resort_system
    print::WordSet*       wordSet;
    
    InitParam( void )
      : lytBuff(NULL)
      , msgSys(NULL)
      , wordSet(NULL)
    { ; }
  };

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   生成パラメータ
   */
  //------------------------------------------------------------------
  UpperBaseDraw( CreateParam& param );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~UpperBaseDraw( void ) { ; }


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

public:
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
  
public:
  //------------------------------------------------------------------
  /**
   * @brief   破棄チェック
   *
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool IsExit( void ) { return true; }

  //------------------------------------------------------------------
  /**
   * @brief   このビュー専用のUIリスナー設定
   *
   * @param   listener   UIリスナー
   */
  //------------------------------------------------------------------
  void SetTitleViewUIListener( ResortDrawListener* listener ) { m_uiListener = listener; }


protected:
  //------------------------------------------------------------------
  /**
   * @brief   2D関連作成
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void Initialize2D( InitParam& param ) = 0;
  
  //------------------------------------------------------------------
  /**
   * @brief   クラスの初期化
   *
   * @param   param   初期化パラメータ
   */
  //------------------------------------------------------------------
  virtual void InitializeClass( InitParam& param ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   ペインの初期化
   */
  //------------------------------------------------------------------
  virtual void InitializePane( void ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   初期テキスト
   */
  //------------------------------------------------------------------
  virtual void InitializeText( void ) { ; }
  
  //------------------------------------------------------------------
  /**
   * @brief   初期アニメ
   */
  //------------------------------------------------------------------
  virtual void InitializeAnim( void ) { ; }


private:
  //------------------------------------------------------------------
  /**
   * @brief   終了処理
   */
  //------------------------------------------------------------------
  virtual bool Terminate( void ) { return true; }


protected:
  app::util::Heap*                  m_pLytHeap;

  app::util::Heap*                  m_pIconHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;
  
  gfl2::fs::AsyncFileManager*       m_pAsyncFileManager;

  ResortInitLogic*                  m_pInitLogic;

  Savedata::ResortSave*             m_pResortSave;
  
  Savedata::BoxPokemon*             m_pBoxPokeSave;
  
  Savedata::ZukanData*              m_pZukanSave;

  ResortDrawListener*               m_uiListener;

protected:
  DrawerBase*     m_pDrawerBase;
};

GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)

#endif // __RESORT_UPPER_BASE_DRAW_H_INCLUDED__