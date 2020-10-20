//======================================================================
/**
 * @file	Kawaigari3DObject.C++
 * @brief	Kawaigari Object Code
 * @author	PETE
 * @data	2015.12.01
 */
//======================================================================
#include "KawaigariMain.h"
#include "KawaigariRenderPipeline.h"

GFL_NAMESPACE_BEGIN(App)
  GFL_NAMESPACE_BEGIN(Kawaigari)

KawaigariObject_Gfmdl::KawaigariObject_Gfmdl(KawaigariMain *core,KawaigariRenderingPipeLine *pipe)
:DemoLib::Obj::Object_GfMdl(pipe)
,mCore(core)
{
	mDeleteCnt = 0;
	mWaitCreate = false;
	mType = 0;  
}

//デストラクタ
KawaigariObject_Gfmdl::~KawaigariObject_Gfmdl()
{
}

void KawaigariObject_Gfmdl::UpdateAlways(int step)
{
  //ビルボード対応
  if( mRenderingPipeLine->GetViewMatrix() )
  {
    mModel->GetModelInstanceNode()->SetBillboardRotation(*mRenderingPipeLine->GetViewMatrix());
  }
    
  mModel->UpdateAnimation();

}

void KawaigariObject_Gfmdl::Update(int step)
{
	DemoLib::Obj::Object_GfMdl::Update(step);

	//生成待ち
	if( mWaitCreate && IsFinishLoad() )
	{
		Create(mCore->GetHeap(),mCore->GetGLAllocator());
		AddRenderPath();
		SetIsUpdate();
		mWaitCreate = false;
	}
	//破棄待ち
	if( mDeleteCnt > 0 )
	{
		mDeleteCnt--;
		if( mDeleteCnt == 0 )
		{
			Delete();
		}
	}
}

//独自
void KawaigariObject_Gfmdl::ReserveCreate(void)
{
	mWaitCreate = true;
}
void KawaigariObject_Gfmdl::ReserveDelete(void)
{
	mDeleteCnt = 2;
}

// KawaigariObject_Effect ///////////////////////////////////////////////////////

//コンストラクタ
KawaigariObject_Effect::KawaigariObject_Effect(KawaigariMain *core)
  :DemoLib::Obj::Object_Effect(core->GetEffectSystem())
  ,mCore(core)	
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = 0;	
}

//デストラクタ
KawaigariObject_Effect::~KawaigariObject_Effect()
{
}

void KawaigariObject_Effect::Update(int step)
{
  DemoLib::Obj::Object_Effect::Update(step);

  //生成待ち
  if( mWaitCreate )
  {
    Create(mCore->GetEffectHeap(),0);
    AddRenderPath();
    SetIsUpdate();
    mWaitCreate = false;
  }
  //破棄待ち
  if( mDeleteCnt > 0 )
  {
    mDeleteCnt--;
    if( mDeleteCnt == 0 )
    {
      Delete();
    }
  }
}

//独自
void KawaigariObject_Effect::ReserveCreate(void)
{
  mWaitCreate = true;
}
void KawaigariObject_Effect::ReserveDelete(void)
{
  mDeleteCnt = 2;
}


  GFL_NAMESPACE_END(Kawaigari)
GFL_NAMESPACE_END(App)