//
// CTR - Ro Module : FieldDemoProc
//
#pragma  once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>
#include <heap/include/gfl2_Heap.h>

// niji
#include "GameSys/include/GameProc.h"
#include "System/include/GflUse.h"

// Render Pipeline
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>

// Sequence Player Test:
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"

namespace poke_3d{
  namespace model{
    struct DressUpParam;
  }
}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Demo)

class FieldDemoProc : public GameSys::GameProc
{

public:

	FieldDemoProc( int playSequence=1 );
	virtual ~FieldDemoProc( void );

  void SetSequence(int n) { mSequence = n; }
  void SetDemoType(int n) { mDemoType = n; }  //FieldPawnTypes.h enum DempType型
  void SetDressUpParam(poke_3d::model::DressUpParam *pDressUpParam, bool isMale) { mpDressUpParam = pDressUpParam; mDressUpParamIsMale = isMale; }
  void SetHeapSize(int heapSize, int devHeapSize) { mHeapSizeIsSet = true; mMemHeapSize = heapSize; mDevHeapSize = devHeapSize; }

	virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
	virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
	virtual void PreDrawFunc( gfl2::proc::Manager* pManager );
	virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo n);
	virtual gfl2::proc::Result NextFrameMainFunc(gfl2::proc::Manager* pManager);
	virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);
	
  
private:

	// State 処理：
	void SetState(u32 s) { mStateReg=s; mStateRegSub=0; }
	void StateInit_Heap(void);
	void StateInit_SeqPlayer(void);
	void StateInit_RenderPipeline(void);

	u32 mStateReg,mStateRegSub;
	gfl2::heap::HeapBase* mHeap;  
  System::nijiAllocator *mGLAllocator;
	gfl2::renderingengine::renderer::DefaultRenderingPipeLine* mRenderingPipeline;
	DemoLib::Sequence::SequenceViewSystem *mSequenceViewSys;

  int mSequence;
  int mDemoType;
  poke_3d::model::DressUpParam *mpDressUpParam;
  bool                          mDressUpParamIsMale;  //DressUpParamとセットで用いる。男ならtrue,　女ならfalse
  bool mHeapSizeIsSet;
  int  mMemHeapSize;    // mHeapSizeIsSetがtrueのときのみ使用する
  int  mDevHeapSize;    // mHeapSizeIsSetがtrueのときのみ使用する

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* mDrawEnvNode;
	
};

	GFL_NAMESPACE_END(Demo)
GFL_NAMESPACE_END(Field)


