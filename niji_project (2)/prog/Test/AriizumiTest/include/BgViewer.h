//======================================================================
/**
 * @file  BgViewer.h
 * @brief 15/04/10
 * @author  ariizumi
 * @data  戦闘背景ビュワー
 */
//======================================================================

#ifndef __BG_VIEWER_H__
#define __BG_VIEWER_H__
#pragma once
#if PM_DEBUG

#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include "GameSys/include/GameProc.h"
#include "GameSys/include/GameManager.h"
#include "System/include/nijiAllocator.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "BgViewAppParam.h"
#include "BgViewerActionListener.h"

#include "AppLib/include/Util/app_util_heap.h"
#include "AppLib/include/Util/AppRenderingManager.h"





GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)


class BgViewDebugPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(BgViewDebugPipeLine); //コピーコンストラクタ＋代入禁止
public:
  BgViewDebugPipeLine(){};
  virtual ~BgViewDebugPipeLine(void){};  //デストラクタ

  void Init(btl::BgSystem *pBgSystem);
  
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


  void SetSkyboxDisp(bool bDisp){ mbSkyboxDisp = bDisp; };

  
private:
  int mRenderPathCnt;
  btl::BgSystem *mBgSystem;
  bool mbSkyboxDisp;

};





class BgViewerProc : public GameSys::GameProc , public BgViewerActionListener
{
  GFL_FORBID_COPY_AND_ASSIGN(BgViewerProc); //コピーコンストラクタ＋代入禁止

public:
  BgViewerProc(); //コンストラクタ
  virtual ~BgViewerProc(void);  //デストラクタ

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


  virtual int OnActionStart( int actionno );


  
public: //デバッグで呼ぶためpublic
  void AppParamInit(gfl2::heap::HeapBase *devHeap);
  bool AppParamEnd(void);
  void CreateDebugMenu(void);
  void DeleteDebugMenu(void);
  void UpdateCamera(void);
  void SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg);
  void SetFovDeg(f32 fov);
  void UpdateCameraMatrix(void);
  void SetFinderCameraMode(void);

  gfl2::heap::HeapBase* mMemHeap;
  gfl2::heap::HeapBase* mDevHeap;

  //@todo
  System::nijiAllocator *mGLAllocator;

  int mInitSeq;
  int mEndSeq;
  int mMainSeq;

  BgViewDebugPipeLine* mpRenderPipeLine;
  applib::frame::Manager *mpFrameManager;
  APP_PARAM   mBgViewAppParam;
  app::util::AppRenderingManager* mpAppRenderingManager;
  app::util::Heap* mpAppHeap;
  
  gfl2::heap::HeapBase *mHeap;
  gfl2::heap::HeapBase *mBgSysHeap;

  btl::BgSystem *mBgSystem;

  gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
  gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  int mLoadStartHeap; //サイズチェック用にロード前の値確保

  int mCameraMode;  //0バトル：1ファインダー

  f32 mCamRotVertical;
  f32 mCamRotHorizonal;
  f32 mCamLength;
  f32 mCamFovDeg;
  gfl2::math::Vector3 mCamPos;

  int mBgTypeGround;
  int mBgTypeNear;
  int mBgTypeFar;

  int mHour;
  int mMinite;
  int mSecond;
  
  bool mbSkyboxDisp;
  bool mbAnimStart;

  bool mDebugPrintTick;
  int mDebugPrintTickCnt;

  int mBtlCamSize[2];
  int mBtlRule;

 public:
   bool mDebugCloud;
   bool mDebugSunMoon;

   btl::BgSystem* GetBgSystem(void){return mBgSystem;}
  //DebugWinコールバックで使うから
  int mAnimeType;
  int mBgClearType;

};  //class BattleProc
GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)



#endif  //PM_DEBUG
#endif  //__BG_VIEWER_H__