#ifndef NIJI_PROJECT_FIELD_FINDER_PROC_H_INCLUDED
#define NIJI_PROJECT_FIELD_FINDER_PROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file    FinderProc.h
 * @author  fukushima_yuya
 * @date    2015.06.08
 * @brief   ポケファインダーUIプロック
 */
//==============================================================================

// niji
#include "App/Finder/include/FinderFrame.h"
#include "App/Finder/include/FinderAppParam.h"

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <Field/FieldRo/include/Subscreen/FieldSubScreenProc.h>
#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"


//----------------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )
class FinderFrame;
GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Finder )

//----------------------------------------------------------------------------
// @brief   FinderProc
//----------------------------------------------------------------------------
class FinderProc
  : public Field::SubScreen::FieldSubScreenProc
{
  GFL_FORBID_COPY_AND_ASSIGN( FinderProc ); //!< コピーコンストラクタ＋代入禁止

public:
  //------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //------------------------------------------------------------------
  FinderProc( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~FinderProc( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン更新を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief   PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);

  //------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @return Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
   * @brief  フィールド復帰　再稼働
   */
  //------------------------------------------------------------------
  virtual void Resume( void );

  //------------------------------------------------------------------
  /**
   * @brief  フィールド削除　一時停止
   */
  //------------------------------------------------------------------
  virtual void Suspend( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief  下画面の種類を返す
   */
  //-----------------------------------------------------------------------------
  virtual Field::SubScreen::SubWindowId GetSubWindowId() { return Field::SubScreen::WIN_FINDER; };

  /**
   * @brief   起動リクエストを返す
   *          フィールドでポーリングされます。
   */
  virtual Field::SubScreen::FieldMenuOutRequestID GetEventRequestID(){ return Field::SubScreen::OUT_REQ_ID_NONE; }

public:
  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーの生成
   */
  //------------------------------------------------------------------
  void CreatePokeFinder( void );

  //------------------------------------------------------------------
  /**
   * @brief   ポケファインダーフレームの取得
   * @return  FinderFrameクラスポインタ
   */
  //------------------------------------------------------------------
  FinderFrame* GetFinderFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   外部設定パラメータのセット
   * @param   pAppParam   外部設定パラメータ
   */
  //------------------------------------------------------------------
  void SetAppParam( APP_PARAM* pAppParam );

private:
  gfl2::heap::HeapBase * m_pManagerHeap;

  app::util::Heap   m_heap;

  app::util::AppRenderingManager * m_pRenderingManager;

  applib::frame::Manager * m_pFrameManager;

  FinderFrame * m_pFinderFrame;

  APP_PARAM*    m_pAppParam;
};


GFL_NAMESPACE_END( Finder )
GFL_NAMESPACE_END( App )


#endif  // #ifndef NIJI_PROJECT_FIELD_FINDER_PROC_H_INCLUDED
