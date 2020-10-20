/**
  * @file   AnaiTestProc
  * @date   2015.07.23
  * @author Anai_Shigeo
  * @brief  戦闘背景テスト
  */


#if (PM_DEBUG)

#ifndef	__AnaiTestProc_H__
#define	__AnaiTestProc_H__

#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>


#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>
#include <System/include/GflUse.h>
#include <system/include/nijiAllocator.h>

#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include <model/include/gfl2_DressUpModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <macro/include/gfl2_Macros.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>

#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Util/AppRenderingManager.h"
#include <arc_index/islandground.gaix> 

#include "AnaiTestProcParam.h"

GFL_NAMESPACE_BEGIN(btl)
  class BgSystem;
GFL_NAMESPACE_END(btl)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(AnaiTest)




enum AnaiTestProcInitStat{
  INIT_START=0,
  PREPARE_SKYBOX,
  WAIT_SKYBOX_PROC,
  BGSYSTEM_INIT,
  WAIT_BGSYSTEM_INIT,
  FINISHED,
};


class AnaiTestProc : public GameSys::GameProc ,public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(AnaiTestProc); // forbid copy and assign

public:

  /**
    * コンストラクタ
    */
  AnaiTestProc();

  /**
    * デストラクタ
    */
  virtual ~AnaiTestProc();

  /**
    * @brief   PROCの初期化を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
    */
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);
  
  /**
    * @brief	  PROCのメイン更新を行う関数
    * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理(下準備)
  * @param pManager   プロセス管理マネージャー
  * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
  * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
  */
  virtual void PreDrawFunc(gfl2::proc::Manager* pManager);

  /**
  * @brief	PROCの描画処理
  * @param pManager   プロセス管理マネージャー
  * @param displayNo  描画対象画面
  * 登録したPROCによる、フレームバッファへの描画を実行します。
  * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
  */
  virtual void DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo);

  /**
    * @brief   PROCのメインを行う関数
    * @note 注意！!!!!! nijiでは未使用です。!!!!!!!!!!!!!
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
    */
  virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);

  /**
    * @brief   PROCの終了を行う関数
    * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
    */
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
  
  /**
    * @brief   FrameModeの取得
    * @retval   FrameMode
    *
    * フレームモードを切り替えるときは、戻り値を変えてください。
    */
  virtual GameSys::FrameMode GetFrameMode(void) const
  {
    return GameSys::FRAMEMODE_30;
  }

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


   btl::BgSystem* GetBgSystem(void)
   {
     return mBgSystem;
   }


  //DebugWinコールバックで使うから
  int mAnimeType;

private:

  void CreateMemory(void);
  void CreateDebugMenu(void);
  void DeleteDebugMenu(void);
  void UpdateCamera(void);
  void SetCameraPosTarget(const gfl2::math::Vector3 &pos,const gfl2::math::Vector3 &trg);
  void UpdateCameraMatrix(void);

  gfl2::heap::HeapBase* mMemHeap;
  gfl2::heap::HeapBase* mDevHeap;

  //@todo
  System::nijiAllocator *mGLAllocator;
  gfl2::gfx::IGLAllocator *mOldAllocator;     

  AnaiTestProcInitStat mInitSeq;
  int mEndSeq;
  int mMainSeq;

  gfl2::heap::HeapBase *mHeap;

  btl::BgSystem *mBgSystem;

  int mRenderPathCnt;
  gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
  gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列

  f32 mCamRotVertical;
  f32 mCamRotHorizonal;
  gfl2::math::Vector3 mCamPos;

  int mBgTypeGround;
  int mBgTypeNear;
  int mBgTypeFar;

  //int mFileSeq;
  int DebugTime;//60*60*12;

  Test::AnaiTest::APP_PARAM* mpAppParam;     //初期化パラメータ
  app::util::Heap* mpAppHeap;

/*
  static const int READ_DATA_NUM = GARC_islandground_DATA_NUM;
  app::util::FileAccessor::RequestData* m_pFrs[READ_DATA_NUM];

  System::Skybox::Skybox* mpSkybox;
  System::nijiAllocator* m_pNijiAllocator;  //レンダリングに必要なアロケータ
  app::util::AppRenderingManager* m_appRenderingManager;    ///APPレンダリングマネージャー
  app::util::AppRenderingManager::Description m_Description;  //APPレンダリングマネージャーの設定

  poke_3d::model::BaseCamera*                               m_camera;       // カメラ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;  // カメラを含む環境
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pDrawEnvNode;  // カメラを含む環境

  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pDrawEnvResourceRootNode;
  void* mpResourceBuffer[GARC_islandground_DATA_NUM];   //読み込んだ素材管理バッファ


  void fileReadStructSetup(app::util::FileAccessor::RequestData* pfrs, int arcid, int dataid, app::util::Heap* pAppHeap, void** pReadBuffAddr);
  bool fileReadSequece(void);
*/
};

GFL_NAMESPACE_END(Test)
GFL_NAMESPACE_END(AnaiTest)


#endif	// __AnaiTestProc_H__

#endif // PM_DEBUG