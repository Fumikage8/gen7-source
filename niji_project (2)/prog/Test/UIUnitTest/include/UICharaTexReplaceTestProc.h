#if PM_DEBUG

#ifndef __UI_CHARA_TEX_REPLACE_TEST_PROC_H__
#define __UI_CHARA_TEX_REPLACE_TEST_PROC_H__
#pragma once

#include "UIUnitTestBaseProc.h"

#include <AppLib/include/Ui/UIView.h>
#include <AppLib/include/Util/AppUtilFileControlState.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class AppToolTrainerIconRendering;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Test)

//  UIのキャラテクスチャ貼り付けのテスト
class UICharaTexReplaceTestProc : public UIUnitTestBaseProc {
  GFL_FORBID_COPY_AND_ASSIGN(UICharaTexReplaceTestProc); //コピーコンストラクタ＋代入禁止
public:
  UICharaTexReplaceTestProc(void) : UIUnitTestBaseProc() { _Clear(); }
  virtual ~UICharaTexReplaceTestProc(void) {}

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

  void SetEnableNPCOnly( const bool bFlg ) { m_bNPCOnly = bFlg; } 
  void SetEnableOutputTexture( const bool bFlg ) { m_bOutputTexture = bFlg; }

protected:
    //  テスト用のフレーム作成
    virtual applib::frame::CellSuper* OnCreateFrame( applib::frame::Manager* pFrameManager, GameSys::GameManager* pGameManager );

private:
    //  変数の初期化
    void _Clear()
    {
      m_pView                     = NULL;
      m_pFileLoadState            = NULL;
      m_pAppPlayerIconRendering     = NULL;
      m_pDrawEnvNodeOfOffScreenCamera  = NULL;
      m_pUppScreenCamera               = NULL;
      m_pOffScreenCamera               = NULL;
      m_pDrawEnvNodeOfUppScreenCamera  = NULL;
      m_bNPCOnly                       = false;
      m_bOutputTexture                 = false;
    }

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNodeOfOffScreenCamera;
    poke_3d::model::BaseCamera*                               m_pOffScreenCamera;

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNodeOfUppScreenCamera;
    poke_3d::model::BaseCamera*                               m_pUppScreenCamera;

    app::ui::UIView*    m_pView;
    app::util::AppUtilFileControlState*       m_pFileLoadState;
    app::tool::AppToolTrainerIconRendering*   m_pAppPlayerIconRendering;

    bool                                      m_bNPCOnly;
    bool                                      m_bOutputTexture;

    u32                                       m_npcTrainerId;

};  //class Proc

GFL_NAMESPACE_END(Test)

#endif // __UI_CHARA_TEX_REPLACE_TEST_PROC_H__

#endif // PM_DEBUG
