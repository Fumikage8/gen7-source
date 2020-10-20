//======================================================================
/**
 * @file  AriTestDraw.cpp
 * @brief 15/04/20
 * @author  ariizumi
 * @data  描画テスト
 */
//======================================================================

#ifndef __ARI_TEST_DRAW_H__
#define __ARI_TEST_DRAW_H__
#pragma once
#if PM_DEBUG

#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"

#include "System/include/nijiAllocator.h"
#include "System/include/RenderPath/EdgeMapRenderPath.h"
#include "System/include/RenderPath/OutLinePostRenderPath.h"
#include "System/include/RenderPath/ModelRenderPath.h"

#include "PokeTool/include/PokeModelSystem.h"


GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)


class AriTestDraw : public GameSys::GameProc ,public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(AriTestDraw); //コピーコンストラクタ＋代入禁止

public:
  AriTestDraw(); //コンストラクタ
  virtual ~AriTestDraw(void);  //デストラクタ

  //============================================================================
  //GameProc
  //============================================================================
  //------------------------------------------------------------------
  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //------------------------------------------------------------------
  /**
    * @brief   PROCのメインを行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc( gfl2::proc::Manager* pManager );

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

  //============================================================================
  //RenderingPipeLine
  //============================================================================
	//----------------------------------------------------------------------------
	/**
	*  @brief  最初のパスから描画できる状態にする
	*/
	//-----------------------------------------------------------------------------
	virtual b32 StartRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  現状のパスにあったシーンを返す
	*  @return シーン
	*/
	//-----------------------------------------------------------------------------
	virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

	//----------------------------------------------------------------------------
	/**
	*  @brief  次のパスへ遷移
	*  @return trueなら次のパス、falseなら描画終了
	*/
	//-----------------------------------------------------------------------------
	virtual b32 NextRenderPath(void);
private:

  void UpdateCamera(void);
  void SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg);
  void UpdateCameraMatrix(void);

  int mInitSeq;
  int mEndSeq;
  int mMainSeq;

  gfl2::heap::HeapBase *mHeap;
  System::nijiAllocator *mAllocator;

  int mRenderPathCnt;
  gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
  gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  f32 mCamRotVertical;
  f32 mCamRotHorizonal;
  gfl2::math::Vector3 mCamPos;

  gfl2::Effect::GFHeap  *mEffectHeap;
  gfl2::Effect::System  *mEffectSystem;
  gfl2::util::List<u32> *mEffectGroupIDList;     ///< グループIDリスト
  gfl2::Effect::EffectRenderPath mEffRenderPath;

  void *mEffRes;
  int  mEffNum;
  gfl2::Effect::Handle **mEffectHandle;
};  //class BattleProc
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)



#endif  //PM_DEBUG
#endif  //__ARI_TEST_DRAW_H__