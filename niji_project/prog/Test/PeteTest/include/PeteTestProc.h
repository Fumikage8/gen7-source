#if PM_DEBUG
//
// CTR - Ro Module : PeteTestProc
//
#pragma  once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>
#include <system/include/nijiAllocator.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"

// Text
#include <str/include/gfl2_Font.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>

// Render Pipeline
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

// Sequence Player Test:
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"


GFL_NAMESPACE_BEGIN(Test)
	GFL_NAMESPACE_BEGIN(Pete)

  const int MAX_DEBUG_POKEMON_SETTINGS = DemoLib::Sequence::MAX_DEBUGSEQ_POKEMON_SETTINGS;

const int SEQ_MAX_FRAME = 10000;

typedef struct  
{
  u32 monsNo;
  u32 formNo;
  u32 sex;
  bool enable;
} PokemonDebugSetting;

class PeteTestProc : public GameSys::GameProc
{

public:

	PeteTestProc( void );
	virtual ~PeteTestProc( void );


	virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
	virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
	virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
	virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo n);
	virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
	virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
	
  
private:
  void CreateDebugMenu(void);
  void DeleteDebugMenu(void);

	// State 処理：
	void SetState(u32 s) { mStateReg=s; mStateRegSub=0; }
	void StateInit_Heap(void);
	void StateInit_SeqPlayer(void);
	void StateInit_RenderPipeline(void);

	u32 mStateReg,mStateRegSub,mSequenceID;
	gfl2::heap::HeapBase* mHeap;
	gfl2::renderingengine::renderer::DefaultRenderingPipeLine* mRenderingPipeline;
	DemoLib::Sequence::SequenceViewSystem *mSequenceViewSys;

	gfl2::heap::NwAllocator *mMemAllocator;
	gfl2::heap::NwAllocator *mDevAllocator;
  System::nijiAllocator *mGLAllocator;

	// Text Rendering:
	gfl2::util::UtilTextRenderingPipeLine *mRenderingPipelineText;
	gfl2::str::ResFont *mFontResource;
	gfl2::util::DrawUtilText *mDrawUtilText;
	
  // DemoSetting Params:
  u32 mPlayerGender,mCurrentPokemon,mTimezone;
  PokemonDebugSetting mPokemonSettings[MAX_DEBUG_POKEMON_SETTINGS];

  // Draw Environment:
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode *mDrawEnvNode;

  // DemoType:
  int mDemoType;

  // VSDemoType
  int mVSDemoType;

  // Debug Info:
  void PrintFrameLog(void);
  bool mDebugInfo;
  bool mUseHIO;
  int mFrameLogPos;
  int mSkipFrames;
  int mTotalFrames;
  int mLastFrame;
  unsigned short *mFrameLogF;
  unsigned short *mFrameLogC;
  unsigned short *mFrameLogG;

  //デバッグメニュー用
  gfl2::debug::DebugWinGroup *mDebugGroup;

  int mSkipMilliSec;

  // 連続再生機能用ここから
  u32 mContinuePlaySeq;         // UpdateFuncのseq操作用seq
  bool mIsContinuePlay;         // 連続再生モードＯＮ／ＯＦＦ
  u32 mFirstContinuePlaySeqID;  // 連続再生開始ＩＤ
  u32 mEndContinuePlaySeqID;    // 連続再生終了ＩＤ
  // 連続再生機能用ここまで
};

	GFL_NAMESPACE_END(Pete)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
