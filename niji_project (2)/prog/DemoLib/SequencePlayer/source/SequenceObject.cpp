//======================================================================
/**
 * @file	SequenceObject.C++
 * @brief	SEQPLAYER描画 モデル等の規定クラス
 * @author	Pete
 * @data	15/5/12
 */
//======================================================================

#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/SequencePlayer/include/SequenceObject.h"
#include "DemoLib/SequencePlayer/include/SequenceRenderPipeline.h"

#include <niji_conv_header/demoseq/seq_com_define.h>
#include "sound/include/Sound.h"

#include <niji_conv_header/app/demoseq_lang/res2d/VS_Demo_MIN_UPP_00_pane.h>
#include <niji_conv_header/app/demoseq_lang/res2d/Battle_Royal_MIN_UPP_00_pane.h>
#include <niji_conv_header/app/demoseq_lang/res2d/Dendou_Apart_Up_00_pane.h>
#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_dendou_demo.h>

GFL_NAMESPACE_BEGIN(DemoLib)
	GFL_NAMESPACE_BEGIN(Sequence)

//コンストラクタ
SequenceObject_Gfmdl::SequenceObject_Gfmdl(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe)
:DemoLib::Obj::Object_GfMdl(pipe)
,mSeqCore(seqCore)
{
	mDeleteCnt = 0;
	mWaitCreate = false;
	mType = 0;  
  mTimeModel = false;
  mTimeType = 0;
}

//デストラクタ
SequenceObject_Gfmdl::~SequenceObject_Gfmdl()
{
}

void SequenceObject_Gfmdl::UpdateAlways(int step)
{
  //ビルボード対応
  if( mRenderingPipeLine->GetViewMatrix() )
  {
    mModel->GetModelInstanceNode()->SetBillboardRotation(*mRenderingPipeLine->GetViewMatrix());
  }

  if(!mModel->IsAnimationLoop() && mCreateFrame!=-1)
  {
    int f = GetSeqViewSystem()->GetTargetFrame();
    f-=mCreateFrame+1;
    mModel->SetAnimationFrame((f32)f);    
  }

  // Frame Skip for loop animation:
  else if(step!=1)
  {
    f32 curFrame = mModel->GetAnimationFrame();
    f32 frame = curFrame + (step-1);
    if(!mModel->IsAnimationLoop()) mModel->SetAnimationFrame(frame);
  }

  if(aniSlot1Loaded)
  {  
    if(!mModel->IsAnimationLoop(1) && mCreateFrame!=-1)
    {
      int f = GetSeqViewSystem()->GetTargetFrame();
      f-=mCreateFrame+1;
      mModel->SetAnimationFrame((f32)f,1);    
    }

    // Frame Skip for loop animation:
    else if(step!=1)
    {
      f32 curFrame = mModel->GetAnimationFrame(1);
      f32 frame = curFrame + (step-1);
      if(!mModel->IsAnimationLoop(1)) mModel->SetAnimationFrame(frame,1);
    }
  }

  // Override Animation if it is TimeModel
  if(mTimeModel)
  {
    int min=0,hour=0,sec=0;

    switch(mTimeType)
    {
    case 0: // MORNING:
      hour = 9;
      break;

    case 1: // NOON:
      hour = 12;
      break;

    case 2: // EVENING:
      hour = 17;
      min = 15;
      break;

    case 3: // NIGHT:
      hour = 21;
      break;

    case 4: // MIDNIGHT:
      hour = 24;
      break;

    case -1: // REAL-TIME:
      mSeqCore->GetTime(hour,min,sec);
      break;
    }

    mModel->SetAnimationStepFrame(0.0f,1);
        
    f32 frame = mSeqCore->CalculateMotionTimeFrame(hour,min,sec) * mModel->GetAnimationEndFrame(1);
    frame = (u32)frame;

    mModel->SetAnimationFrame(frame,1);
    GFL_PRINT("TimeModel (%i): Using Motion Frame %f / %f\n",mTimeType,frame,mModel->GetAnimationEndFrame(1));
  }


 if(!mModel->IsAnimationLoop() && !mTimeType) mModel->UpdateAnimation();

 
 {
    f32 curFrame = mModel->GetAnimationFrame();
  ///  GFL_PRINT("SeqCur Aniframe:%f\n",curFrame);
  }

}

void SequenceObject_Gfmdl::Update(int step)
{
	DemoLib::Obj::Object_GfMdl::Update(step);

	//生成待ち
	if( mWaitCreate && IsFinishLoad() )
	{
		Create(mSeqCore->GetMemHeap(),mSeqCore->GetGLAllocator());
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
void SequenceObject_Gfmdl::ReserveCreate(void)
{
	mWaitCreate = true;
}
void SequenceObject_Gfmdl::ReserveDelete(void)
{
	mDeleteCnt = 2;
}


void SequenceObject_Chara::UpdateAlways(int step)
{
///  Object_GfMdl::UpdateAlways(step);
  
  //ビルボード対応
  if( mRenderingPipeLine->GetViewMatrix() )
  {
    mModel->GetModelInstanceNode()->SetBillboardRotation(*mRenderingPipeLine->GetViewMatrix());
  }

  if(!mModel->IsAnimationLoop() && mCreateFrame!=-1)
  {
    int f = GetSeqViewSystem()->GetTargetFrame();
    f-=mCreateFrame+1;
    mModel->SetAnimationFrame((f32)f);
  }

  // Frame Skip for loop animation:
  else if(step!=1)
  {
    f32 curFrame = mModel->GetAnimationFrame();
    f32 frame = curFrame + (step-1);
    mModel->SetAnimationFrame(frame);
  }

  if(!mModel->IsAnimationLoop()) mModel->UpdateAnimation();
  

  {
    f32 curFrame = mModel->GetAnimationFrame();
   /// GFL_PRINT("CharaCur Aniframe:%f / %f  Target:%i\n",curFrame,mModel->GetAnimationEndFrame(),GetSeqViewSystem()->GetTargetFrame());
  }

/*  if( mReserveAnime != -1 )
  {
    if( mModel->IsAnimationLastFrame() )
    {
      ChangeAnimation(mReserveAnime);
      SetAnimationIsLoop(true);
      mReserveAnime = -1;
    }
  } */

  if(mUpdateEye)
  {  
    // 自動目パチ更新
    GetCharaModel()->UpdateEye();
  }

}

// SequenceObject_Poke ///////////////////////////////////////////////////////
SequenceObject_Poke::SequenceObject_Poke(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe,int index)
  :DemoLib::Obj::Object_Poke(pipe)
  ,mSeqCore(seqCore)
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = 0;
  mIdx = index;
  mIsVisibleShadow = true;
}

//デストラクタ
SequenceObject_Poke::~SequenceObject_Poke()
{
}

void SequenceObject_Poke::SetVisible(const bool flg)
{
  if( IsEnable() )
  {

    // Using Poke Inframe:
    switch(mSceneOpt)
    {
    case 1: case 4: // Inframe 1
      mSeqCore->Inframe_SetVisible(0,flg); break;
    case 2: case 5: // Inframe 2
      mSeqCore->Inframe_SetVisible(1,flg); break;
    }
        
    mModel->SetVisible(flg);
    Object::SetVisible(flg);    
  }
}

void SequenceObject_Poke::SetShadowVisible(const bool flg)
{
  mIsVisibleShadow = flg;
  DemoLib::Obj::Object_Poke::SetShadowVisible(mIsVisibleShadow);
}

void SequenceObject_Poke::UpdateAlways(int step)
{
  mModel->SetVisible(mIsVisible);
  if (mIsVisible)
  {
    DemoLib::Obj::Object_Poke::SetShadowVisible(mIsVisibleShadow);
  }
  /*
  if(step!=1)
  {
    {    
      f32 curFrame = mModel->GetAnimationFrame();
      f32 frame = curFrame + (step-1);
      if(frame>curFrame || !mModel->IsAnimationLoop()) mModel->SetAnimationFrame(frame);    
    }

    if(mSlotAnimation1Loaded)
    {
      f32 curFrame = mModel->GetAnimationFrame(1);
      f32 frame = curFrame + (step-1);
      if(frame>curFrame  || !mModel->IsAnimationLoop(1)) mModel->SetAnimationFrame(frame,1);    
    }
  }*/

  if(!mModel->IsAnimationLoop() && mCreateFrame!=-1 && !GetSeqViewSystem()->IsSystemSync())
  {
    int f = GetSeqViewSystem()->GetTargetFrame();
    f-=mCreateFrame;
    mModel->SetAnimationFrame((f32)f);
  }

  // Frame Skip for loop animation:
  else if(step!=1)
  {
    f32 curFrame = mModel->GetAnimationFrame();
    f32 frame = curFrame + step;//(step-1);
    mModel->SetAnimationFrame(frame);
  }

 /// if(!mModel->IsAnimationLoop()) mModel->UpdateAnimation();

  if( mReserveAnime != -1 )
  {
    if( IsFinishAnimation() )
    {
      ChangeAnimationDirect(mReserveAnime);
      SetAnimationIsLoop(true);      
    }
  
  }
    
}

void SequenceObject_Poke::Update(int step)
{
  DemoLib::Obj::Object_Poke::Update(step);

  //生成待ち
  if( mWaitCreate && IsFinishLoad() )
  {
    Create(mSeqCore->GetMemHeap(),mSeqCore->GetGLAllocator(),true);
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

void SequenceObject_Poke::RemoveRenderPath(void)
{

  // Using Poke Inframe:
  switch(mSceneOpt)
  {
  case 1: case 4: // Inframe 1
    mSeqCore->Inframe_RemovePokemon(0); break;
  case 2: case 5: // Inframe 2
    mSeqCore->Inframe_RemovePokemon(1); break;
  }

  DemoLib::Obj::Object_Poke::RemoveRenderPath();
   
}

//SequenceEditorからの値をMODEL_NODEに変換
void SequenceObject_Poke::GetFollowNode( int seqNode , PokeTool::MODEL_NODE *retNode , int *retNodeIdx )
{
  //ノード無かった時用
  PokeTool::MODEL_NODE node = PokeTool::MODEL_NODE_FRONT;
  int nodeIdx = 0;
  
  switch( seqNode )
  {
  case SEQ_DEF_NODE_ORIGIN:
    GFL_ASSERT("非対応\n");
    break;
  case SEQ_DEF_NODE_BUTURI:
    node = PokeTool::MODEL_NODE_ATTACK01;
    break;
  case SEQ_DEF_NODE_SHOOT: 
    node = PokeTool::MODEL_NODE_SHOOT01;
    break;
  case SEQ_DEF_NODE_HEAD:
    node = PokeTool::MODEL_NODE_OVERHEAD;
    break;
  case SEQ_DEF_NODE_FRONT: 
    node = PokeTool::MODEL_NODE_FRONT;
    break;
  case SEQ_DEF_NODE_FACE:
    node = PokeTool::MODEL_NODE_HEAD;
    break;
  case SEQ_DEF_NODE_EYE:
    node = PokeTool::MODEL_NODE_EYE;
    break;
  case SEQ_DEF_NODE_MOUTH: 
    node = PokeTool::MODEL_NODE_MOUTH;
    break;
  case SEQ_DEF_NODE_HORN:
    node = PokeTool::MODEL_NODE_HORN;
    break;
  case SEQ_DEF_NODE_TAIL:
    node = PokeTool::MODEL_NODE_TAIL;
    break;
  case SEQ_DEF_NODE_HAND1: 
    node = PokeTool::MODEL_NODE_HAND;
    nodeIdx = 0;
    break;
  case SEQ_DEF_NODE_HAND2: 
    node = PokeTool::MODEL_NODE_HAND;
    nodeIdx = 1;
    break;
  case SEQ_DEF_NODE_FOOT1: 
    node = PokeTool::MODEL_NODE_FOOT;
    nodeIdx = 0;
    break;
  case SEQ_DEF_NODE_FOOT2:
    node = PokeTool::MODEL_NODE_FOOT;
    nodeIdx = 1;
    break;
  case SEQ_DEF_NODE_CENTER:
    node = PokeTool::MODEL_NODE_CENTER;
    break;
  }
  *retNode = node;
  *retNodeIdx = nodeIdx;
}

void SequenceObject_Poke::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader)
{
  DemoLib::Obj::Object_Poke::Create(heap,allocator,useShader);
  GetPokeModel()->SetLightSetNo(LIGHTNO_POKEMON_TOP+mIdx);
  {
    int matNum = GetPokeModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      GetPokeModel()->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
    }
  }
  if( GetPokeModel()->GetShadowModel() )
  {
    int matNum = GetPokeModel()->GetShadowModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      GetPokeModel()->GetShadowModel()->SetMaterialLightSetIndex(i,LIGHTNO_POKEMON_TOP+mIdx);
    }
  }

}

//独自
void SequenceObject_Poke::ReserveCreate(void)
{
  mWaitCreate = true;
}
void SequenceObject_Poke::ReserveDelete(void)
{
  mDeleteCnt = 2;
}

void SequenceObject_Poke::SetRotate(const gfl2::math::Vector3 &rot)
{
  
  switch(mSceneOpt)
  {   
    case 1: case 4: // Inframe 1
      mSeqCore->Inframe_SetRotation(0,rot); break;
    case 2: case 5: // Inframe 2
      mSeqCore->Inframe_SetRotation(1,rot); break;  

    default:
      mRotate = rot;
      SetIsUpdate();
      break;
  }

};

void SequenceObject_Poke::SetRotateOffset(const gfl2::math::Vector3 rot )
{

  switch(mSceneOpt)
  {   
  case 1: case 4: // Inframe 1
    mSeqCore->Inframe_SetRotation(0,rot); break;
  case 2: case 5: // Inframe 2
    mSeqCore->Inframe_SetRotation(1,rot); break;  

  default:
    mRotateOffset = rot;
    SetIsUpdate();
    break;
  }

};


// SequenceObject_Layout ///////////////////////////////////////////////////////

//コンストラクタ
SequenceObject_Layout::SequenceObject_Layout(SequenceViewSystem *seqCore,int layoutIdx,int numAni)
:DemoLib::Obj::Object_Layout(seqCore->GetAppHeap(),seqCore->GetRenderSys()->GetAppRenderingManager(),layoutIdx,numAni)
,mSeqCore(seqCore)	
{
	mDeleteCnt = 0;
	mWaitCreate = false;
	mType = 0;	

  for(int z=0;z<4;z++)
    mVSNameBuf[z] = NULL;
}

//デストラクタ
SequenceObject_Layout::~SequenceObject_Layout()
{
  for(int z=0;z<4;z++)
    GFL_SAFE_DELETE(mVSNameBuf[z]);
}

void SequenceObject_Layout::Update(int step)
{
	DemoLib::Obj::Object::Update(step);

	//生成待ち
	if( mWaitCreate )
	{
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
void SequenceObject_Layout::ReserveCreate(void)
{
	mWaitCreate = true;
}
void SequenceObject_Layout::ReserveDelete(void)
{
	mDeleteCnt = 2;
}

void SequenceObject_Layout::VSBattleSet(int index)
{
  if(index<0 || index>=4) return; 

  // Create Name buf?
  if(mVSNameBuf[index]==NULL)
  {
    int bufLen = 64;
    mVSNameBuf[index] = GFL_NEW(mSeqCore->GetMemHeap()) gfl2::str::StrBuf( bufLen, mSeqCore->GetMemHeap() );    
    
    if(!mSeqCore->GetVSBattleName(mVSNameBuf[index],index) )
    {
      GFL_SAFE_DELETE(mVSNameBuf[index]);
      mVSNameBuf[index] = NULL;
    }
  }

  u8 *ballData = mSeqCore->GetVSBattleMonsterBallSetting(index);

  switch(index)
  {
    case 0:
      
      if(mVSNameBuf[index]!=NULL)
      {
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_00,mVSNameBuf[index]);
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_02,mVSNameBuf[index]);
      }
      else
      {      
#if PM_DEBUG
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_00,"PLAYER1");
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_02,"PLAYER1");
#endif
      }

      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT00_BALL_ICON_17,true);


      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT02_BALL_ICON_17,true);
      break;

    case 1:
      if(mVSNameBuf[index]!=NULL)
      {
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_01,mVSNameBuf[index]);
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_03,mVSNameBuf[index]);
      }
      else
      {      
#if PM_DEBUG
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_01,"PLAYER2");
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_03,"PLAYER2");
#endif
      }
      
      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT01_BALL_ICON_17,true);


      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT03_BALL_ICON_17,true);

      break;

    case 2:
      if(mVSNameBuf[index]!=NULL)      
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_04,mVSNameBuf[index]);              
      else
      {
#if PM_DEBUG
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_04,"PLAYER3");
#endif
      }

      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT04_BALL_ICON_17,true);
      break;

    case 3:
      if(mVSNameBuf[index]!=NULL)      
        GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_05,mVSNameBuf[index]);        
      else 
      {
#if PM_DEBUG
        GetG2DUtil()->SetTextboxPaneString(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_NAME_05,"PLAYER4");
#endif
      }

      if(ballData[0]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_00,true);
      if(ballData[1]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_01,true);
      if(ballData[2]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_02,true);
      if(ballData[3]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_03,true);
      if(ballData[4]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_04,true);
      if(ballData[5]==1)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_05,true);

      if(ballData[0]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_06,true);
      if(ballData[1]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_07,true);
      if(ballData[2]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_08,true);
      if(ballData[3]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_09,true);
      if(ballData[4]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_10,true);
      if(ballData[5]==2)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_11,true);

      if(ballData[0]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_12,true);
      if(ballData[1]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_13,true);
      if(ballData[2]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_14,true);
      if(ballData[3]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_15,true);
      if(ballData[4]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_16,true);
      if(ballData[5]==3)
        GetG2DUtil()->SetPaneVisible(0,PANENAME_VS_DEMO_MIN_UPP_00_PANE_PLT05_BALL_ICON_17,true);
      break;
  }
    
}

void SequenceObject_Layout::RoyalBattleSet(int index)
{
  if(index<0 || index>=4) return; 

  // Create Name buf?
  if(mVSNameBuf[index]==NULL)
  {
    int bufLen = 64;
    mVSNameBuf[index] = GFL_NEW(mSeqCore->GetMemHeap()) gfl2::str::StrBuf( bufLen, mSeqCore->GetMemHeap() );    

    if(!mSeqCore->GetVSBattleName(mVSNameBuf[index],index) )
    {
      GFL_SAFE_DELETE(mVSNameBuf[index]);
      mVSNameBuf[index] = NULL;
    }
  }

  u8 *ballData = mSeqCore->GetVSBattleMonsterBallSetting(index);

  switch(index)
  {
  case 0:

    if(mVSNameBuf[index]!=NULL)
    {
      GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_02,mVSNameBuf[index]);
    }
    else
    {      
#if PM_DEBUG
      GetG2DUtil()->SetTextboxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_02,"PLAYER1");
#endif
    }
  
    if(ballData[0]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_00,true);
    if(ballData[1]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_01,true);
    if(ballData[2]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_02,true);
    if(ballData[3]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_03,true);
    if(ballData[4]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_04,true);
    if(ballData[5]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_05,true);

    if(ballData[0]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_06,true);
    if(ballData[1]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_07,true);
    if(ballData[2]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_08,true);
    if(ballData[3]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_09,true);
    if(ballData[4]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_10,true);
    if(ballData[5]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_11,true);

    if(ballData[0]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_12,true);
    if(ballData[1]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_13,true);
    if(ballData[2]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_14,true);
    if(ballData[3]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_15,true);
    if(ballData[4]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_16,true);
    if(ballData[5]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT02_BALL_ICON_17,true);
    break;

  case 1:
    if(mVSNameBuf[index]!=NULL)
    {
      GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_03,mVSNameBuf[index]);
    }
    else
    {      
#if PM_DEBUG
      GetG2DUtil()->SetTextboxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_03,"PLAYER2");
#endif
    }
   
    if(ballData[0]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_00,true);
    if(ballData[1]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_01,true);
    if(ballData[2]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_02,true);
    if(ballData[3]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_03,true);
    if(ballData[4]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_04,true);
    if(ballData[5]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_05,true);

    if(ballData[0]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_06,true);
    if(ballData[1]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_07,true);
    if(ballData[2]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_08,true);
    if(ballData[3]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_09,true);
    if(ballData[4]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_10,true);
    if(ballData[5]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_11,true);

    if(ballData[0]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_12,true);
    if(ballData[1]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_13,true);
    if(ballData[2]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_14,true);
    if(ballData[3]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_15,true);
    if(ballData[4]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_16,true);
    if(ballData[5]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT03_BALL_ICON_17,true);

    break;

  case 2:
    if(mVSNameBuf[index]!=NULL)      
      GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_04,mVSNameBuf[index]);              
    else
    {
#if PM_DEBUG
      GetG2DUtil()->SetTextboxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_04,"PLAYER3");
#endif
    }

    if(ballData[0]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_00,true);
    if(ballData[1]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_01,true);
    if(ballData[2]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_02,true);
    if(ballData[3]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_03,true);
    if(ballData[4]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_04,true);
    if(ballData[5]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_05,true);

    if(ballData[0]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_06,true);
    if(ballData[1]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_07,true);
    if(ballData[2]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_08,true);
    if(ballData[3]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_09,true);
    if(ballData[4]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_10,true);
    if(ballData[5]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_11,true);

    if(ballData[0]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_12,true);
    if(ballData[1]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_13,true);
    if(ballData[2]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_14,true);
    if(ballData[3]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_15,true);
    if(ballData[4]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_16,true);
    if(ballData[5]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT04_BALL_ICON_17,true);
    break;

  case 3:
    if(mVSNameBuf[index]!=NULL)      
      GetG2DUtil()->SetTextBoxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_05,mVSNameBuf[index]);        
    else 
    {
#if PM_DEBUG
      GetG2DUtil()->SetTextboxPaneString(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_NAME_05,"PLAYER4");
#endif
    }

    if(ballData[0]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_00,true);
    if(ballData[1]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_01,true);
    if(ballData[2]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_02,true);
    if(ballData[3]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_03,true);
    if(ballData[4]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_04,true);
    if(ballData[5]==1)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_05,true);

    if(ballData[0]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_06,true);
    if(ballData[1]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_07,true);
    if(ballData[2]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_08,true);
    if(ballData[3]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_09,true);
    if(ballData[4]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_10,true);
    if(ballData[5]==2)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_11,true);

    if(ballData[0]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_12,true);
    if(ballData[1]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_13,true);
    if(ballData[2]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_14,true);
    if(ballData[3]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_15,true);
    if(ballData[4]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_16,true);
    if(ballData[5]==3)
      GetG2DUtil()->SetPaneVisible(0,PANENAME_BATTLE_ROYAL_MIN_UPP_00_PANE_PLT05_BALL_ICON_17,true);
    break;
  }

  }

void SequenceObject_Layout::DendouDemoSet()
{
  // Create Name buf?
  if(mVSNameBuf[0]==NULL)
  {
    int bufLen = 64;
    mVSNameBuf[0] = GFL_NEW(mSeqCore->GetMemHeap()) gfl2::str::StrBuf( bufLen, mSeqCore->GetMemHeap() );    
  }

  // create msgData and add to g2dutil
  gfl2::str::MsgData * msgData = GFL_NEW(mHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_dendou_demo_DAT, mHeap->GetDeviceHeap() );
  gfl2::str::MsgData * oldMsgData = GetG2DUtil()->GetMsgData();
  GetG2DUtil()->SetMessageData(msgData, GetG2DUtil()->GetWordSet());

  // Set string for level
  GetG2DUtil()->SetTextBoxPaneString(0, PANENAME_DENDOU_APART_UP_00_PANE_POKELV, dendo_demo_02 );

  // Pane Table
  static const struct
  {
    u32 nickname;
    u32 level;
  } PaneTable[DEMOSEQUENCE_MAX_POKEMON] =
  {
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_00,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_00,
    },
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_01,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_01,
    },
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_02,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_02,
    },
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_03,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_03,
    },
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_04,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_04,
    },
    {
      PANENAME_DENDOU_APART_UP_00_PANE_POKENAME_05,
      PANENAME_DENDOU_APART_UP_00_PANE_POKE999_05,
    },
  };

  // Set string for pokemon
  for ( int i = 0; i < DEMOSEQUENCE_MAX_POKEMON; ++i )
  {
    const pml::pokepara::PokemonParam * param = mSeqCore->GetPokemonParam(i);
    if( param )
    {
      param->GetNickName( mVSNameBuf[0] );
      int level = param->GetLevel();

      GetG2DUtil()->SetTextBoxPaneString(0, PaneTable[i].nickname, mVSNameBuf[0]);
      GetG2DUtil()->SetTextBoxPaneNumber(0, PaneTable[i].level, dendo_demo_03, level, 3,0, print::NUM_DISP_SPACE );
    }
#if PM_DEBUG
    else
    {
      // for debug DemoPlayer
      gfl2::str::STRCODE name[] = GFLSTR("ピカチュウ");
      GetG2DUtil()->SetTextBoxPaneString(0, PaneTable[i].nickname, name );
      GetG2DUtil()->SetTextBoxPaneNumber(0, PaneTable[i].level, dendo_demo_03, 100, 3,0, print::NUM_DISP_SPACE );
    }
#endif
  }

  // restore msgData
  GetG2DUtil()->SetMessageData(oldMsgData, GetG2DUtil()->GetWordSet());
  GFL_DELETE( msgData );
}

// SequenceObject_Light ///////////////////////////////////////////////////////

//コンストラクタ
SequenceObject_Light::SequenceObject_Light(SequenceViewSystem *seqCore,SequenceRenderingPipeLine *pipe,bool isTimeLight)
  :DemoLib::Obj::Object_Light(pipe)
  ,mSeqCore(seqCore)	
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = mTimeType = 0;	
  mTimeLight = isTimeLight;
}

//デストラクタ
SequenceObject_Light::~SequenceObject_Light()
{
}

void SequenceObject_Light::Update(int step)
{
  DemoLib::Obj::Object_Light::Update(step);

  //生成待ち
  if( mWaitCreate )
  {
    Create(mSeqCore->GetMemHeap(),mSeqCore->GetGLAllocator());
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

void SequenceObject_Light::UpdateAlways(int step)
{
  if(IsAddRenderPath() && GetAnimationController()!=NULL && IsEnable()) {}
  else return;

  gfl2::animation::LightAnimationSlot* pLightSlot = GetAnimationController()->GetLightSlot();

  if(pLightSlot!=NULL && step!=1)    
  {

    if(!pLightSlot->IsLoop() && mCreateFrame!=-1)
    {
      int f = GetSeqViewSystem()->GetTargetFrame();
      f-=mCreateFrame;///+1;
      pLightSlot->SetFrame((f32)f);
    }

    // Frame Skip for loop animation:
    else if(step!=1)
    {
      f32 curFrame = pLightSlot->GetFrame();
      f32 frame = curFrame + (step-1);
      pLightSlot->SetFrame(frame);
    }

  }

  // Override if TIME LIGHT:
  if(mTimeLight)
  {
    int min=0,hour=0,sec=0;

    switch(mTimeType)
    {
      case 0: // MORNING:
        hour = 9;
        break;

      case 1: // NOON:
        hour = 12;
        break;

      case 2: // EVENING:
        hour = 17;
        min = 15;
        break;

      case 3: // NIGHT:
        hour = 21;
        break;

      case 4: // MIDNIGHT:
        hour = 24;
        break;

      case -1: // REAL-TIME:
        mSeqCore->GetTime(hour,min,sec);
        break;
    }

    u32 frameMax = (u32)pLightSlot->GetFrameCount();
    const f32 ONE_HOUR_FRAME   = frameMax / 24.0f;
    const f32 ONE_MINUTE_FRAME = frameMax / 1440.0f;
    const f32 ONE_SECOND_FRAME = frameMax / 86400.0f;
    u32 frame = (u32)((hour * ONE_HOUR_FRAME) + (min * ONE_MINUTE_FRAME) + (sec * ONE_SECOND_FRAME));

    pLightSlot->SetStepFrame(0.0f);
    
    pLightSlot->SetFrame( (f32)frame);
    GFL_PRINT("TimeLight (%i): Using Motion Frame %i / %i\n",mTimeType,frame,pLightSlot->GetFrameCount());
  }

  GetAnimationController()->Update();
 // GetAnimationController()->GetLightSlot()->SetLoop(true);

  {
    f32 curFrame = GetAnimationController()->GetLightSlot()->GetFrame();
    GFL_PRINT("LightCur Aniframe:%f / %f  Target:%i\n",curFrame,GetAnimationController()->GetLightSlot()->GetLoopEndFrame(),GetSeqViewSystem()->GetTargetFrame());
  }


}

//独自
void SequenceObject_Light::ReserveCreate(void)
{
  mWaitCreate = true;
}
void SequenceObject_Light::ReserveDelete(void)
{
  mDeleteCnt = 2;
}


// SequenceObject_Effect ///////////////////////////////////////////////////////

//コンストラクタ
SequenceObject_Effect::SequenceObject_Effect(SequenceViewSystem *seqCore)
  :DemoLib::Obj::Object_Effect(seqCore->GetEffectSystem())
  ,mSeqCore(seqCore)	
{
  mDeleteCnt = 0;
  mWaitCreate = false;
  mType = 0;	
}

//デストラクタ
SequenceObject_Effect::~SequenceObject_Effect()
{
}

void SequenceObject_Effect::Update(int step)
{
  DemoLib::Obj::Object_Effect::Update(step);

  //生成待ち
  if( mWaitCreate )
  {
    Create(mSeqCore->GetEffectHeap(),0);
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
void SequenceObject_Effect::ReserveCreate(void)
{
  mWaitCreate = true;
}
void SequenceObject_Effect::ReserveDelete(void)
{
  mDeleteCnt = 2;
}


void SequenceObject_Sound::SetSoundId(int id)
{
  //ついでにリセットしてしまう。
  SetFollowObject(NULL);
  SetFollowPoke(NULL);

  mSoundId = id;
  if( id != -1 )
  {
    SetIsEnable(true);
  }
  else
  {
    SetIsEnable(false);
  }
}
void SequenceObject_Sound::UpdateAlways(int step)
{
  if( mIs3DSound )
  {
    if( mIsBefUpdate )
    {
      gfl2::math::Vector3 pos;
      GetCalcPosition( &pos );
      gfl2::math::Vector3 diff = pos - mBefPos;
      gfl2::math::VEC3 nwPos(diff.x,diff.y,diff.z);
      Sound::Set3DActorVelocity( mIndex , nwPos );
      mIsBefUpdate = false;
    }
    else
    {
      gfl2::math::VEC3 zero(0,0,0);
      Sound::Set3DActorVelocity( mIndex , zero );
    }
  }
}
void SequenceObject_Sound::UpdateCore(int step)
{
  if( mIs3DSound )
  {
    gfl2::math::Vector3 pos;
    GetCalcPosition( &pos );

    gfl2::math::VEC3 nwPos(pos.x,pos.y,pos.z);
    Sound::Set3DActorPos( mIndex , nwPos );
    mBefPos = pos;
    mIsBefUpdate = true;
  }
}

	GFL_NAMESPACE_END(Sequence)
GFL_NAMESPACE_END(DemoLib)

