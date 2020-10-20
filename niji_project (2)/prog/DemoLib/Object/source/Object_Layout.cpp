//======================================================================
/**
 * @file	Object_Layout.C++
 * @brief	オブジェクト Layout
 * @author  Pete
 * @data  15/05/19
 */
//======================================================================


#include <math/include/gfl2_math.h>
#include <debug/DebugWin/include/DebugWinSystem.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

#include <model/include/gfl2_BaseModel.h>

#include <DemoLib/Object/include/Object_Layout.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include <system/include/Timer/gfl2_PerformanceCounter.h>

#include <Print/include/SystemFont.h>

#include <GameSys/include/GameManager.h>

#include "arc_index/debug_message.gaix"
#include "niji_conv_header/message/debug/msg_debug_item_make.h"


GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

Object_Layout::Object_Layout(app::util::Heap *heap, app::util::AppRenderingManager *renderManager,int layoutIdx,int numAni)
:Object()
, mAniNum(-1)
, mLayoutIdx(layoutIdx)
, mNumAni(numAni)
, mHeap(heap)
, app::ui::UIView( heap, heap->GetDeviceAllocator(), heap->GetDeviceAllocator() )
{
  mIsAddRenderPath = false;
  mRenderingManager = renderManager;
}

Object_Layout::~Object_Layout()
{
  Delete2D();

  GFL_SAFE_DELETE( m_pWordSet );
  GFL_SAFE_DELETE( m_pMsgData );
}

void Object_Layout :: LoadLayout(void *buf)
{
	GFL_PRINT("\t\tLoadLayout\n");

  resourceBuf = buf;
	
  // Resource Data:
  const app::util::G2DUtil::LytResData res_tbl[] =
  {
    { buf, 0, app::util::G2DUtil::ATTACH_ROOT },
  };

  // Layout Data:
  const app::util::G2DUtil::LytwkData data_tbl[] =
  {
    {
      0,
        mLayoutIdx,
        mNumAni,
        buf,
        false,
        app::util::G2DUtil::SETUP_UPPER,
        true
    },   
  };

  m_pMsgData = GFL_NEW(mHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), 0, mHeap->GetDeviceHeap() );
  m_pWordSet = GFL_NEW(mHeap->GetDeviceHeap()) print::WordSet( mHeap->GetDeviceHeap() );

  Create2D(
    mHeap,
    NULL,
    1,
    res_tbl,
    GFL_NELEMS(res_tbl),
    data_tbl,
    GFL_NELEMS(data_tbl),
    m_pMsgData,
    m_pWordSet );

	SetIsEnable(true);
  mIsAddRenderPath = true;
}

void Object_Layout::SetAnimation(int index)
{
  if( !mIsAddRenderPath) return;

	mAniNum = index;
	GFL_PRINT("\t\tLayout AnimationSet(%i)\n",mAniNum);

  GetG2DUtil()->StartAnime(0,index);
	tickStart = gfl2::system::Timer::PerformanceCounter::GetMicroSecond();
}

void Object_Layout::SetPanePosition(gfl2::math::Vector3 pos,int paneIndex)
{
  if( !mIsAddRenderPath) return;
  
  gfl2::math::VEC3 v;

  v.x = pos.x;
  v.y = pos.y;
  v.z = pos.z; 

  GetG2DUtil()->SetPanePos(0,paneIndex,&v);

}

void Object_Layout::GetPanePosition(gfl2::math::Vector3 &pos,int paneIndex)
{
  if( !mIsAddRenderPath) return;
   
  gfl2::math::VEC3 v;
  v = GetG2DUtil()->GetPanePos(0,paneIndex);

  pos.x = v.x;
  pos.y = v.y;
  pos.z = v.z; 
  
}


void Object_Layout::SetPaneScale(gfl2::math::Vector3 pos,int paneIndex)
{
  if( !mIsAddRenderPath) return;

  gfl2::math::VEC2 v;
 
  v.x = pos.x;
  v.y = pos.y;
 
  GetG2DUtil()->SetScale(0,paneIndex,v);  

}

void Object_Layout::GetPaneScale(gfl2::math::Vector3 &pos,int paneIndex)
{
  if( !mIsAddRenderPath) return;
  
  gfl2::math::VEC2 v;

  GetG2DUtil()->GetScale(0,paneIndex,&v);
  
  pos.x = v.x;
  pos.y = v.y;
  pos.z = 1.0f;       
}

void Object_Layout::SetPaneAlpha(u8 alpha,int paneIndex)
{  
  if( !mIsAddRenderPath) return;
  GetG2DUtil()->SetPaneAlpha(0,paneIndex,alpha);
}

void Object_Layout::GetPaneAlpha(u8 &alpha,int paneIndex)
{
  if( !mIsAddRenderPath) return;
  alpha = GetG2DUtil()->GetPaneAlpha(0,paneIndex);
}

void Object_Layout::SetPaneVisible(bool visible,int paneIndex)
{
  if( !mIsAddRenderPath) return;  
  GetG2DUtil()->SetPaneVisible(0,paneIndex,visible);  
}

void Object_Layout::UpdateAnimation(int step)
{
  UpdateAlways(step);
}

void Object_Layout::UpdateAlways(int step)
{
	if(!mIsAddRenderPath) return;
	/// GFL_PRINT("\t\tLayout Update Always(Ani:%i)\n",mAniNum);
  
  // Animation Sync
  if(mAniNum!=-1)
  {
    f32 curFrame = GetG2DUtil()->GetAnimeFrame(0, mAniNum);
    f32 maxFrame = GetG2DUtil()->GetAnimeMaxFrame(0, mAniNum);

    f32 nextFrame = curFrame + step;
    nextFrame = nextFrame < maxFrame ? nextFrame : maxFrame;

    GFL_PRINT("LayoutAni: %f/%f FRAME (next = %f)\n", curFrame, maxFrame, nextFrame);

    GetG2DUtil()->SetAnimeFrame(0, mAniNum, nextFrame);
  }

  app::ui::UIView::Update();

  if(mAniNum!=-1 &&  GetG2DUtil()->IsAnimeEnd(0,mAniNum) )
  {			
    //		mAniNum = -1;
    GetG2DUtil()->StopAnime(0,mAniNum,false); //SetAnimeFrameを使うと AutoUnBind されないため、StopAnime( …, false)　が必要
  }

  UpdateTree();
}

void Object_Layout::UpdateCore(int step)
{

}

void Object_Layout::DrawLayout(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(!mIsAddRenderPath) return;
  DrawTree(displayNo);
}

void Object_Layout::Delete(void)
{
	if( IsEnable() )
	{
		SetIsEnable(false);       
	}

}

void Object_Layout::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{  
  if(mIsAddRenderPath)
    GetG2DUtil()->AddDrawableLytWkForOneFrame( mRenderingManager, displayNo, 0 );  
}

//ObjectRenderPipeLine経由で追加する
void Object_Layout::AddRenderPath(int opt)
{
  if( IsEnable() && IsAddRenderPath() == false )
  {
    mSceneOpt = opt;  //自動削除タスクのため覚えておく 
    mIsAddRenderPath = true;
  }
}

void Object_Layout::RemoveRenderPath(void)
{
  if( IsEnable() && IsAddRenderPath() )
  {
//    gfl2::lyt::LayoutRenderPath* pRenderPath =	mRenderingPipeLine->GetLayoutPath();
 //   pRenderPath->ClearDrawableLytWk();
    mIsAddRenderPath = false;
    app::ui::UIView::SetVisible(false);    
  }
}


GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
