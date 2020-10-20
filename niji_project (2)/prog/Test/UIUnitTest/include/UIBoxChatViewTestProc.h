#if PM_DEBUG

#ifndef __UI_BOX_CHAT_VIEW_TEST_PROC_H__
#define __UI_BOX_CHAT_VIEW_TEST_PROC_H__
#pragma once

#include "UIUnitTestBaseProc.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/AppUtilFileControlState.h>
#include "App/Box/source/BoxUppChatDraw.h"

GFL_NAMESPACE_BEGIN(Test)

//  UIのBoxチャット表示のテスト
class UIBoxChatViewTestProc : public UIUnitTestBaseProc {
  GFL_FORBID_COPY_AND_ASSIGN(UIBoxChatViewTestProc); //コピーコンストラクタ＋代入禁止
public:
  UIBoxChatViewTestProc(void) : UIUnitTestBaseProc() {}
  virtual ~UIBoxChatViewTestProc(void) {}

  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

protected:
    //  レンダリングタイプ取得
    //  作成するレンダリングタイプを派生先で変更することが可能
    virtual app::util::AppRenderingManager::RenderOrderType::Tag _OnGetDispUppAppRenderOrderType()
    {
      return  app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART_POKEMODEL0_POKEMODEL1_POKEMODEL2;
    }

    //  テスト用のフレーム作成
    virtual applib::frame::CellSuper* OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager );

private:
    //  変数の初期化
    void _Claer()
    {
      m_pView                 = NULL;
      m_pFileLoadState        = NULL;
      m_pAppMsgData           = NULL;
      m_pPokeModelDrawEnvNode = NULL;
      m_pPokeModelCamera      = NULL;
    }

    //  ポケモンモデル描画の環境
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pPokeModelDrawEnvNode;
    //  ポケモンモデル描画のカメラ
    poke_3d::model::BaseCamera*                               m_pPokeModelCamera;

    App::Box::BoxUppChatDraw* m_pView;
    app::util::AppUtilFileControlState* m_pFileLoadState;
    gfl2::str::MsgData*                 m_pAppMsgData;

};  //class Proc

GFL_NAMESPACE_END(Test)

#endif // __UI_BOX_CHAT_VIEW_TEST_PROC_H__

#endif // PM_DEBUG
