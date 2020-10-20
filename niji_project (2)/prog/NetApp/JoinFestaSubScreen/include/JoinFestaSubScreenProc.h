//==============================================================================
/**
 * @file	JoinFestaSubScreenProc.h
 * @brief	ジョインフェスタ下画面Proc
 * @author	ichiraku_katsuhiko
 * @date	2015/06/10 14:21:03
 */
// =============================================================================

#if !defined( JOIN_FESTA_SUBSCREEN_PROC_H_INCLUDED )
#define JOIN_FESTA_SUBSCREEN_PROC_H_INCLUDED
#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Util/app_util_Heap.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProc.h"
#include "Field/FieldRo/include/Subscreen/FieldSubScreenProcManager.h"

// JoinFestaSubScreen
#include "NetApp/JoinFestaSubScreen/include/JoinFestaSubScreenAppParam.h"

// RotomFade
#include "NetApp/JoinFestaSubScreen/source/View/RotomFade.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)
GFL_NAMESPACE_BEGIN(ApplicationSystem)
class JoinFestaSubScreenWork;
GFL_NAMESPACE_END(ApplicationSystem)
GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSubScreen)

GFL_NAMESPACE_BEGIN(ApplicationSystem)
class JoinFestaSubScreenApplicationSystem;
GFL_NAMESPACE_END(ApplicationSystem)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaSubScreenProc : public Field::SubScreen::FieldSubScreenProc
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaSubScreenProc);  // コピーコンストラクタ＋代入禁止

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------

public:
  // コンストラクタ
  JoinFestaSubScreenProc();
  JoinFestaSubScreenProc( NetApp::JoinFestaSubScreen::APP_PARAM* pAppParam );
  // デストラクタ
  virtual ~JoinFestaSubScreenProc();


  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
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

  virtual void PreDrawFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド復帰　再稼働
   */
  //-----------------------------------------------------------------------------
  virtual void Resume( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド削除　一時停止
   */
  //-----------------------------------------------------------------------------
  virtual void Suspend( void );

  //-----------------------------------------------------------------------------
  /**
   *	@brief  下画面の種類を返す
   */
  //-----------------------------------------------------------------------------
  virtual Field::SubScreen::SubWindowId GetSubWindowId() { return Field::SubScreen::WIN_JOIN_FESTA; };

  /**
   * @brief	  起動リクエストを返す
   *          フィールドでポーリングされます。
   */
  virtual Field::SubScreen::FieldMenuOutRequestID GetEventRequestID();


  /**
   * @brief   ロトムフェードイン開始
   */
  void RotomFadeInStart();

  /**
   * @brief   ロトムフェードイン終了チェック
   * @return  trueで終了
   */
  bool IsRotomFadeInEnd();

  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================


private:
  NetApp::JoinFestaSubScreen::APP_PARAM*                                              m_pAppParam;
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenWork*              m_pJoinFestaSubScreenWork;
  NetApp::JoinFestaSubScreen::ApplicationSystem::JoinFestaSubScreenApplicationSystem* m_pApplicationSystem;

  View::RotomFade* m_pRotomFade;

};

GFL_NAMESPACE_END(JoinFestaSubScreen)
GFL_NAMESPACE_END(NetApp)

#endif // JOIN_FESTA_SUBSCREEN_PROC_H_INCLUDED
