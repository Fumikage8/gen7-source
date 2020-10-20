#ifndef NIJI_PROJECT_APP_POKELIST_PROC_H_INCLUDED
#define NIJI_PROJECT_APP_POKELIST_PROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    PokeListProc.h
 * @author  fukushima_yuya
 * @date    2015.03.23
 * @brief   ポケモンリスト画面プロック
 */
//==============================================================================


// niji
#include "App/PokeList/include/PokeListAppParam.h"
#include "App/PokeList/include/PokeListSetupParam.h"
#include "App/PokeList/source/PokeListConst.h"

#include <App/Status/include/StatusAppParam.h>
#include <App/Bag/include/BagAppParam.h>
#include <App/StrInput/include/StrInputAppParam.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
//#include <Battle/include/battle_SetupParam.h> // Regulation
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include <Savedata/include/MyStatus.h>

// pml
#include <pml/include/pmlib.h>


//----------------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )
class PokeListFrame;
class PokeListJoinFrame;
class PokeListSimpleFrame;
class PokeListWazaSodateyaFrame;
GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( PokeList )

//----------------------------------------------------------------------------
// @brief   Proc
//----------------------------------------------------------------------------
class Proc
  : public applib::frame::AppFrameListener
  , public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( Proc ); //!< コピーコンストラクタ＋代入禁止

private:
  app::util::Heap*    m_pAppHeap;

  app::util::AppRenderingManager*   m_pAppRenderingManager;
  
  applib::frame::Manager*   m_pFrameManager;

  gfl2::heap::HeapBase*   m_pManagerHeap;


private:
  static const int SIZE_PROC_MANAGER = 0x1000;   //!< 4Kバイト マネージャーのヒープは荒れるので別途管理

  static const int SIZE_MY_APP = 0x200000;       //!< 2Mバイト アプリメモリも容量管理のため分けて管理
  //static const int SIZE_MY_APPDEV = 0x400000;    //!< 4Mバイト アプリメモリも容量管理のため分けて管理
  static const int SIZE_MY_APPDEV = 0x800000;    //!< 8Mバイト アプリメモリも容量管理のため分けて管理

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  Proc( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~Proc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  
  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメインを行う関数
   *  1/30フレームの②フレーム目の処理です。
   *  1/60フレーム動作時には、使われません。
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result NextFrameMainFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );

  //------------------------------------------------------------------
  /**
   * @brief	  EndFuncが完了した直後に呼び出される関数
   */
  //------------------------------------------------------------------
  virtual void EndFuncFinish( applib::frame::CellSuper* pEndCell );

public:
  //------------------------------------------------------------------
  /**
   * @brief   フレームマネージャの取得
   */
  //------------------------------------------------------------------
  applib::frame::Manager* GetFrameManager( void ) { return m_pFrameManager; }

  //------------------------------------------------------------------
  /**
   * @brief   アプリケーションレンダリングマネージャの取得
   */
  //------------------------------------------------------------------
  app::util::AppRenderingManager* GetAppRenderingManager( void ) { return m_pAppRenderingManager; }

  //------------------------------------------------------------------
  /**
   * @brief   アプリケーションヒープの取得
   */
  //------------------------------------------------------------------
  app::util::Heap* GetAppHeap( void ) { return m_pAppHeap; }


public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面の生成
   */
  //------------------------------------------------------------------
  void CreatePokeListFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケモンリスト画面で使用する外部設定パラメータのセット
   */
  //------------------------------------------------------------------
  void SetAppParam( GameSys::GameManager * pGameMan, CONTEXT_PARAM * pContext, app::bag::APP_PARAM * pBagAppParam, Status::APP_PARAM * pStatusParam );


private:
  APP_PARAM                   m_PokeListAppParam;

  CONTEXT_PARAM*              m_pPokeListContextParam;

  PokeListFrame*              m_pFieldFrame;          //!< フィールド用ポケモンリストフレーム

  PokeListJoinFrame*          m_pJoinFrame;           //!< 参加選択用ポケモンリストフレーム

  PokeListSimpleFrame*        m_pSimpleFrame;         //!< 対象選択用ポケモンリストフレーム

  PokeListWazaSodateyaFrame*  m_pWazaSodateyaFrame;   //!< 技、育て屋用ポケモンリストフレーム

  Status::APP_PARAM*          m_pStatusAppParam;      //!< ポケモンステータス用設定パラメータ

  app::bag::APP_PARAM*        m_pBagAppParam;         //!< バッグ用設定パラメータ

  app::tool::AppCommonGrpIconData*    m_pCommonIcon;  //!< 状態異常アイコン
};


GFL_NAMESPACE_END( PokeList )
GFL_NAMESPACE_END( App )

#endif  // #ifndef NIJI_PROJECT_APP_POKELIST_PROC_H_INCLUDED
