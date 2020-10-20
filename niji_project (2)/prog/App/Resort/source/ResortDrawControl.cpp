//======================================================================
/**
 * @file    ResortDrawControl.cpp
 * @brief   アニメの指示を出している所
 * @author  k.ohno
 * @data    16/04/02
 */
//======================================================================



#include <gfx/include/gfl2_GFGL.h>
#include <Sound/include/Sound.h>
#include <niji_conv_header/sound/SoundMiddleID.h>

#include "System/include/nijiAllocator.h"
#include "Savedata/include/ResortSave.h"
#include "GameSys/include/NijiTime.h"
#include <AppLib/include/Tool/PokeIcon.h>

// poke_3dのインクルード
#include <model/include/gfl2_BaseModel.h>

#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>

#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>

#include "ResortIslandDraw.h"
#include "ResortDrawControl.h"

#include <arc_def_index/arc_def.h>
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"
#include "System/include/SkyBox/Skybox.h"
#include <Fade/include/gfl2_FadeManager.h>
#include <model/include/gfl2_BaseCamera.h>
#include "Savedata/include/BoxPokemonSave.h"

#include "ResortChangeIslandListener.h"
#include "ResortInitLogic.h"



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)

ResortDrawControl::ResortDrawControl()
{
  m_ModeSeq = 0;
}

ResortDrawControl::~ResortDrawControl(void)
{

}



void ResortDrawControl::SetParam( ResortIslandDraw* pResortIslandDraw,ResortInitLogic* pLogic )
{
  mpResortIslandDraw = pResortIslandDraw;
  mpLogic = pLogic;
}





enum{
  BILLBOARD_MODE,
  TREE_BEANS_PUT,
  TREE_BEANS_PUT_FIRST,
  TREE_BEANS_PUT_WAIT,
  TREE_BEANS_PUT_WAIT_FIRST,
	BOX_ANIM_LOOP,
  POKEMON_ANIM_START,
  GIM_ANIM_LOOP,
  GIM_KANBAN_ANIM_LOOP,
  ADV_KANBAN_ANIM_LOOP,
  HOTSPA_KANBAN_ANIM_LOOP,
  FARM_KANBAN_ANIM_LOOP,
  ADV_TREASURE_BOX_ANIM_LOOP,
  BEANS_BOTTLE_ANIM_LOOP,
  ADVENTURE_IN_START,
  ADVENTURE_OUT_START,
  ADVENTURE_OUT_START_STEP2,
  FARM_HARVEST_LOOP,
};


void ResortDrawControl::UpdateFunc(int debugflg)
{
  mpResortIslandDraw->UpdateFunc(debugflg);
}

void ResortDrawControl::UpdateCameraMove( bool is_fadein, bool is_fadeout )
{
  mpResortIslandDraw->StartCameraMove( is_fadein );
  mpResortIslandDraw->EndCameraMove( is_fadeout );
  mpResortIslandDraw->UpdateCameraMove();
}



#if PM_DEBUG
bool ResortDrawControl::ControlCheck(bool bDebugCameraMode,bool bkeye)
#else
bool ResortDrawControl::ControlCheck(bool bkeye)
#endif
{
  int controlnum;

#if PM_DEBUG
  controlnum = mpResortIslandDraw->ControlFunc( bDebugCameraMode,bkeye );
#else
  controlnum = mpResortIslandDraw->ControlFunc( bkeye );
#endif

  switch(controlnum)
  {
  case ResortIslandDraw::CONTROL_TOUCH_TREE:
    m_ModeSeq=TREE_BEANS_PUT;
    mpLogic->IvyTouchOnTheBeans(mBeansBuff);
    break;
  case ResortIslandDraw::CONTROL_TOUCH_TREE_FIRST:
    m_ModeSeq=TREE_BEANS_PUT_FIRST;
    mpLogic->IvyTouchOnTheBeans(mBeansBuff);
    break;
  case ResortIslandDraw::CONTROL_TOUCH_BOX:
    m_ModeSeq=BOX_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_NONE:
    return true;
  case ResortIslandDraw::CONTROL_DISABLE:
    break;
  case ResortIslandDraw::CONTROL_TOUCH_ADV_KANBAN:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD1;
    m_ModeSeq=ADV_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_GIM:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD1;
    m_ModeSeq=GIM_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_GIM_KANBAN:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD1;
    m_ModeSeq=GIM_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_HOTSPA_KANBAN:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD1;
    m_ModeSeq=HOTSPA_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_FARM_KANBAN:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD1;
    m_ModeSeq=FARM_KANBAN_ANIM_LOOP;
    break;

  case ResortIslandDraw::CONTROL_TOUCH_GIM_KANBAN2:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD2;
    m_ModeSeq=GIM_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_HOTSPA_KANBAN2:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD2;
    m_ModeSeq=HOTSPA_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_FARM_KANBAN2:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD2;
    m_ModeSeq=FARM_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_GIM_KANBAN3:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD3;
    m_ModeSeq=GIM_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_HOTSPA_KANBAN3:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD3;
    m_ModeSeq=HOTSPA_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_FARM_KANBAN3:
    mBoardNo = ResortIslandDraw::CONTROL_BOARD3;
    m_ModeSeq=FARM_KANBAN_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_ADV_CHECK:
    m_ModeSeq = ADV_TREASURE_BOX_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_BEANS_BOTTLE:
    m_ModeSeq = BEANS_BOTTLE_ANIM_LOOP;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_POKEMON:
    m_ModeSeq=POKEMON_ANIM_START;
    mLocalCount=0;
    break;
  case ResortIslandDraw::CONTROL_TOUCH_FARM_HARVEST:
    m_ModeSeq= FARM_HARVEST_LOOP;
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  switch(controlnum){
  case ResortIslandDraw::CONTROL_NONE:
  case ResortIslandDraw::CONTROL_DISABLE:
    break;
  default:
    mpResortIslandDraw->ResortInputEnabled(false);
    mpResortIslandDraw->SetKeyEnable(false);//3D,pokemon
    break;
  }
  return false;
}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
//bool ResortDrawControl::ControlFunc(void)

#if PM_DEBUG
bool ResortDrawControl::ControlFunc(bool bDebugCameraMode,bool bkeye)
#else
bool ResortDrawControl::ControlFunc(bool bkeye)
#endif
{
  mpResortIslandDraw->UpdateFuncBefore();
  
  switch(m_ModeSeq){
  case BILLBOARD_MODE:
#if PM_DEBUG
    if(ControlCheck(bDebugCameraMode,bkeye)){
      return true;
    }
#else
    if( ControlCheck(bkeye)){
      return true;
    }
#endif
    break;
  case TREE_BEANS_PUT: //一個ふらす
    if(mpResortIslandDraw->BeansTreePut(mBeansBuff))
    {
      m_ModeSeq = TREE_BEANS_PUT_WAIT;
      m_AnimTimer = System::GflUse::GetPublicRand(3);
    }
    else
    {
      mpResortIslandDraw->ResortInputEnabled(false);
      mpResortIslandDraw->BeansTreePutEnd();
      mpResortIslandDraw->SetKeyEnable(true);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case TREE_BEANS_PUT_WAIT: //一定期間待つ
    if(m_AnimTimer<0)
    {
      m_ModeSeq=TREE_BEANS_PUT;
    }
    m_AnimTimer--;
    break;

  case TREE_BEANS_PUT_FIRST: //一個ふらす
    if(mpResortIslandDraw->BeansTreePut(mBeansBuff))
    {
      m_ModeSeq = TREE_BEANS_PUT_WAIT_FIRST;
      m_AnimTimer = System::GflUse::GetPublicRand(3);
    }
    else
    {
      mpResortIslandDraw->ResortInputEnabled(false);
      mpResortIslandDraw->BeansTreePutEnd();
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case TREE_BEANS_PUT_WAIT_FIRST: //一定期間待つ
    if(m_AnimTimer<0)
    {
      m_ModeSeq=TREE_BEANS_PUT_FIRST;
    }
    m_AnimTimer--;
    break;


  case GIM_ANIM_LOOP:
    if(mpResortIslandDraw->IsSignBoardTouchAnim(ResortIslandDraw::CONTROL_BOARD1))
    {
      mpResortIslandDraw->GimTouchEnd(ResortIslandDraw::CONTROL_BOARD1);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case GIM_KANBAN_ANIM_LOOP:
    if(mpResortIslandDraw->IsSignBoardTouchAnim(mBoardNo))
    {
      mpResortIslandDraw->GimSignBoardTouchEnd(mBoardNo);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case ADV_KANBAN_ANIM_LOOP:
    if(mpResortIslandDraw->IsSignBoardTouchAnim(mBoardNo))
    {
      mpResortIslandDraw->AdventureSignBoardTouchEnd(mBoardNo);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case HOTSPA_KANBAN_ANIM_LOOP:
    if(mpResortIslandDraw->IsSignBoardTouchAnim(mBoardNo))
    {
      mpResortIslandDraw->HotSpaSignBoardTouchEnd(mBoardNo);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case FARM_KANBAN_ANIM_LOOP:
    if(mpResortIslandDraw->IsSignBoardTouchAnim(mBoardNo))
    {
      mpResortIslandDraw->FarmSignBoardTouchEnd(mBoardNo);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case BOX_ANIM_LOOP:
    if(mpResortIslandDraw->IsBeansBoxTouchAnimLastFrame()){
			mpResortIslandDraw->BeansBoxTouchAnimEnd();
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;

  case ADV_TREASURE_BOX_ANIM_LOOP:
    if(mpResortIslandDraw->IsTreasureBoxTouchAnimLastFrame()){
			mpResortIslandDraw->AdventureTreasureBoxTouchEnd();
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;

  case BEANS_BOTTLE_ANIM_LOOP:
    if( mpResortIslandDraw->IsBeansBottleTouchLastFrame()){
      mpResortIslandDraw->BeansBottleTouchEnd();
      mpResortIslandDraw->ResortInputEnabled(true);
      m_ModeSeq=BILLBOARD_MODE;//もどる
    }
    break;
  case POKEMON_ANIM_START:
    if(mpResortIslandDraw->SetSelectPokemonMulScaleTable(mLocalCount)){
      mpResortIslandDraw->PokemonTouchAnimEnd();
      m_ModeSeq = BILLBOARD_MODE;//もどる
    }
    mLocalCount++;
    break;

  case ADVENTURE_IN_START:
    if(mpResortIslandDraw->TreasurePokemonAnimEnd()){
      mpLogic->StartAdventure();
      mpResortIslandDraw->SetFukidashiAllVisible(true);
      mpResortIslandDraw->ButtonDisplayTypeEnabled(true);
      mpResortIslandDraw->SetAdventureInEnd();
      m_ModeSeq = BILLBOARD_MODE;//もどる
    }
    break;
  case ADVENTURE_OUT_START:
    mpLogic->StopAdventure(0);
    mpResortIslandDraw->SimpleParamCreate();
    m_ModeSeq = ADVENTURE_OUT_START_STEP2;
    break;
  case ADVENTURE_OUT_START_STEP2:
    if(mpResortIslandDraw->TreasurePokemonAnimEnd()){
      mpLogic->StopAdventure(1);
      mpResortIslandDraw->SetFukidashiAllVisible(true);
      mpResortIslandDraw->TreasurePokemonNormalAnim();
      mpResortIslandDraw->ButtonDisplayTypeEnabled(true);
      m_ModeSeq = BILLBOARD_MODE;//もどる
    }
    break;
  case FARM_HARVEST_LOOP://ok
    if(mpResortIslandDraw->IsAnimEndGetEffect()){
      mpResortIslandDraw->SetKeyEnable(true);
      mpResortIslandDraw->ResortInputEnabled(true);
      m_ModeSeq = BILLBOARD_MODE;//もどる
    }
    break;
    
  }

  return false;
}


void ResortDrawControl::StartModelAnim(int type)
{
  switch(type){
  case ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_IN:
    if(m_ModeSeq == BILLBOARD_MODE){
      m_ModeSeq = ADVENTURE_IN_START;
      mpResortIslandDraw->TreasurePokemonInAnim();
      mpResortIslandDraw->SetFukidashiAllVisible(false);
      mpResortIslandDraw->ButtonDisplayTypeEnabled(false);
    }
    break;
  case ResortChangeIslandListener::ISLAND_MODELANIM_ADVENTURE_OUT:
    if(mpLogic->IsAdventureCourseSet()){
      mpResortIslandDraw->TreasurePokemonOutAnim();
      mpResortIslandDraw->SetFukidashiAllVisible(false);
      mpResortIslandDraw->ButtonDisplayTypeEnabled(false);
      m_ModeSeq = ADVENTURE_OUT_START;
    }
    else{
      //すでに帰り着いている場合なにもしない
    }
    break;
  }
}



GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)



