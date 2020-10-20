#if PM_DEBUG

#ifndef __TEST_FUKUSHIMA_TEST_POKE_MODEL_TEST_FRAME_H__
#define __TEST_FUKUSHIMA_TEST_POKE_MODEL_TEST_FRAME_H__
#pragma once
//======================================================================
/**
 * @file PokeModelTestFrame.h
 * @date 2015/09/07 11:21:37
 * @author fukushima_yuya
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include <Test/FukushimaTest/source/PokeModelTest/PokeModelTestDrawListener.h>

#include <AppLib/include/Frame/AppFrameCellSuper.h>


//------------------------------------------------------------------------------
// @brief   前方宣言
//------------------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )
class AppRenderingManager;
class Heap;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )
class PokeModelTestDraw;
GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)
class BaseCamera;
GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( str )
class MsgData;
GFL_NAMESPACE_END( str )
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(instance)
class DrawEnvNode;
GFL_NAMESPACE_END(instance)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( print )
class WordSet;
GFL_NAMESPACE_END( print )


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Fukushima )

//------------------------------------------------------------------------------
// @brief   アイコンテストフレーム
//------------------------------------------------------------------------------
class PokeModelTestFrame
  : public applib::frame::CellSuper
  , public PokeModelTestDrawListener
{
  GFL_FORBID_COPY_AND_ASSIGN( PokeModelTestFrame );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  コンストラクタ
   */
  //------------------------------------------------------------------
  PokeModelTestFrame( void );

  //------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //------------------------------------------------------------------
  virtual ~PokeModelTestFrame( void );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  動作に必要な引数を登録
   */
  //------------------------------------------------------------------
  void Setup( app::util::Heap* pHeap, app::util::AppRenderingManager* pAppRenderingManager );

public:
  //------------------------------------------------------------------
  /**
   * @brief	  PROCの初期化を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief	  PROCの終了を行う関数
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

public:   //!< リスナー
  //------------------------------------------------------------------
  /**
   * @brief	  終了通知
   */
  //------------------------------------------------------------------
  virtual void OnEnd( void );

protected:
  int   m_subseq;     //!< プロセス関数の動作シーケンス
  bool  m_bEnd;       //!< Endフラグ

private:
  app::util::Heap*                  m_pAppHeap;
  app::util::AppRenderingManager*   m_pAppRenderingManager;

  PokeModelTestDraw*     m_pPokeModelDraw;

  void*   m_pArcReadBuff;
  int     m_ReadSeq;

  gfl2::str::MsgData*   m_pMsgData;
  print::WordSet*       m_pWordSet;

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   m_pCameraDrawEnvNode;   //!< カメラを含む環境
  poke_3d::model::BaseCamera*                                 m_pCamera;              //!< カメラ
};

GFL_NAMESPACE_END( Fukushima )
GFL_NAMESPACE_END( Test )

#endif // __TEST_FUKUSHIMA_TEST_POKE_MODEL_TEST_FRAME_H__

#endif // PM_DEBUG