//======================================================================
/**
 * @file	btlv_debug.cpp
 * @brief	バトル描画デバッグ
 * @author	ariizumi
 * @data	15/03/22
 */
//======================================================================
#if PM_DEBUG

#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <DemoLib/Object/include/ObjectDebug.h>

#include <GameSys\include\GameManager.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "sound/include/Sound.h"
// メッセージのインデックス
#include "niji_conv_header/message/debug/msg_debug_menu.h"

#include "test/FieldTest/include/Debug/FieldDebugTypes.h"

#include "btlv_core.h"
#include "render/btlv_RenderSystem.h"
#include "render/btlv_RenderPipeLine.h"
#include "btlv_object.h"
#include "btlv_debug.h"
#include "btlv_camera.h"
#include "../btl_mainmodule.h"


#include <arc_index/message.gaix>

#define _BTLV_WAITCAMERA_INFO_ONLY_
#include "niji_conv_header/battle/wait_camera.cdat"  // For wait camera data

GFL_NAMESPACE_BEGIN(btl)

extern void BtlvDebug_ObjGroupUpdateCallback_Preset( void* work );
extern void BtlvDebug_CameraGroupUpdateCallback( void *work );
extern void BtlvDebug_DrawGroupUpdateCallback( void *work );

extern void AddPartyDebugGroup(BattleViewSystem *btlvCore, gfl2::debug::DebugWinGroup *top, int clientIdx);

void RomPrint( const char *fmt, ... )
{
#ifdef GF_PLATFORM_CTR

  char buffer[1024];

  va_list vlist;
  va_start(vlist, fmt);

  size_t length = std::min<size_t>(nn::nstd::TVSNPrintf(buffer, 1024, fmt, vlist), 1024);
  nn::dbg::detail::PutString(buffer, length);

  va_end(vlist);
#endif //GF_PLATFORM_CTR
}

gfl2::str::StrBuf *sBtlvDebugWorkStr = NULL;
gfl2::str::MsgData *sTokuseiMsgData = NULL;
gfl2::str::MsgData *sWazaMsgData = NULL;
gfl2::str::MsgData *sItemMsgData = NULL;

void InitBtlvDebug( BattleViewSystem *btlvCore )
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  gfl2::str::MsgData* pMsgData = devWork->GetMessageData();

  btlvCore->mDebugDispLyt = true;
  btlvCore->mDebugPrintTick = false;
  btlvCore->mDebugPrintTickCnt = 0;
  btlvCore->mDebugPrintTickInterval = 60;
  btlvCore->mDebugTickCheck = false;
  btlvCore->mDebugTickCheckSe = false;
  btlvCore->mDebugTickLimitCPU = -1;
  btlvCore->mDebugTickLimitGPU = 32000;

  btlvCore->mDebugObjPreset = 0;
  btlvCore->mDebugEdgeMode = 3;
  btlvCore->mIsEnableWaitCamera = true;
  btlvCore->mDebugEdgeMode = 3;
  btlvCore->mDebugUseLight = false;
  btlvCore->mDebugDispBg = true;
  btlvCore->mDebugDispWeather = true;
  btlvCore->mDebugDispMask = true;
  btlvCore->mDebugRenderPathCheck = -1;

  // For Wait camera
  btlvCore->mDebugWaitCamDataIdx = -1;
  btlvCore->mDebugWaitCamHeatLevel = -1;
  btlvCore->mDebugWaitCamTargetPokemon = -1;
  btlvCore->mDebugWaitCamTargetTrainer = -1;
  btlvCore->mDebugWaitCamTargetPokemon_end = -1;
  btlvCore->mDebugWaitCamTargetTrainer_end = -1;
  btlvCore->mDebugWaitCamReverse = -1;
  btlvCore->mDebugWaitCamStartWaitFrame = -1;
  btlvCore->mDebugWaitCamFirstPlayDataIdx = -1;

  btlvCore->mDebugFPEnableSetting = false;
  btlvCore->mDebugFPRimColorR  = 1.0f;
  btlvCore->mDebugFPRimColorG  = 1.0f;
  btlvCore->mDebugFPRimColorB  = 0.4f;
  btlvCore->mDebugFPRimPow     = 4.0f;
  btlvCore->mDebugFPRimWidth   = 0.4f;
  btlvCore->mDebugFPRimMoveSpeed = 2048;
  btlvCore->mDebugFPRimMoveWidth = 0.2f;
  btlvCore->mDebugFPAuraColorR   = 1.0f;
  btlvCore->mDebugFPAuraColorG   = 1.0f;
  btlvCore->mDebugFPAuraColorB   = 0.4f;
  btlvCore->mDebugFPAuraScaleX   = 1.0f;
  btlvCore->mDebugFPAuraScaleY   = 1.0f;
  btlvCore->mDebugFPAuraDensity  = 9024;
  btlvCore->mDebugFPAuraBaseScaleX = 1.2f;
  btlvCore->mDebugFPAuraBaseScaleY = 1.0f;

  sBtlvDebugWorkStr = GFL_NEW(heap)gfl2::str::StrBuf(128, heap);
  sTokuseiMsgData = GFL_NEW(heap)gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_tokusei_DAT, heap,gfl2::str::MsgData::LOAD_FULL);
  sWazaMsgData = GFL_NEW(heap)gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_wazaname_DAT, heap, gfl2::str::MsgData::LOAD_FULL);
  sItemMsgData = GFL_NEW(heap)gfl2::str::MsgData(print::GetMessageArcId(), GARC_message_itemname_DAT, heap, gfl2::str::MsgData::LOAD_FULL);

  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_battle_top,NULL);
  gfl2::debug::DebugWinItem* item;
  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *objGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_battle_obj,topGroup);
  gfl2::debug::DebugWinGroup *retGroup;

  item = gfl2::debug::DebugWin_AddItemValueInt(objGroup,heap, pMsgData, msg_dmenu_battle_obj_preset,&btlvCore->mDebugObjPreset,0,3);
  item->SetChangedValueFunc(BtlvDebug_ObjGroupUpdateCallback_Preset,btlvCore);
  
  gfl2::debug::DebugWin* devWin = GFL_SINGLETON_INSTANCE(gfl2::debug::DebugWin);
  wchar_t*  workStr = devWin->GetWorkStr();
  gfl2::str::StrBuf* workStrBuf = devWin->GetWorkStrBuf();
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    pMsgData->GetString(msg_dmenu_battle_obj_poke, *workStrBuf);
    swprintf( workStr, 8, L"%ls%d\0", workStrBuf->GetPtr(), 1+i);
    retGroup = DemoLib::Obj::AddDebugWin_Object(heap,objGroup, btlvCore->GetPokeModelObj(i) ,workStr);
  }
  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    pMsgData->GetString(msg_dmenu_battle_obj_trainer, *workStrBuf);
    swprintf( workStr, 12, L"%ls%d\0", workStrBuf->GetPtr(), 1+i);
    retGroup = DemoLib::Obj::AddDebugWin_Object(heap,objGroup, btlvCore->GetTrainerModelObj(i) ,workStr);
    pMsgData->GetString(msg_dmenu_battle_obj_trainer_shadow, *workStrBuf);
    swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", workStrBuf->GetPtr());
    retGroup = DemoLib::Obj::AddDebugWin_Object(heap,retGroup, btlvCore->GetTrainerModelObj(i)->GetShadowModel() ,workStr);
  }

  pMsgData->GetString(msg_dmenu_battle_obj_fade, *workStrBuf);
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls", workStrBuf->GetPtr());
  retGroup = DemoLib::Obj::AddDebugWin_Object(heap,objGroup, btlvCore->GetBgFadeModelObj() ,workStr);

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *camGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_battle_camera,topGroup);

  item = gfl2::debug::DebugWin_AddItemValueVector3(camGroup,heap, pMsgData,msg_dmenu_battle_camera_pos,btlvCore->GetCameraSys()->GetCameraPosPointer(),-10000.0f,10000.0f);
  item->SetChangedValueFunc(BtlvDebug_CameraGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueVector3(camGroup,heap, pMsgData,msg_dmenu_battle_camera_trg,btlvCore->GetCameraSys()->GetCameraTrgPointer(),-10000.0f,10000.0f);
  item->SetChangedValueFunc(BtlvDebug_CameraGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueFloat(camGroup,heap, pMsgData,msg_dmenu_battle_camera_trg,btlvCore->GetCameraSys()->GetCameraTwistPointer(),-100.0f,100.0f,0.1f);
  item->SetChangedValueFunc(BtlvDebug_CameraGroupUpdateCallback,btlvCore);

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *drawGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_battle_draw,topGroup);
  
  item = gfl2::debug::DebugWin_AddItemValueInt(drawGroup,heap, pMsgData, msg_dmenu_battle_draw_edge,&btlvCore->mDebugEdgeMode,0,3);
  item->SetChangedValueFunc(BtlvDebug_DrawGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueBool(drawGroup,heap, pMsgData, msg_dmenu_battle_draw_disp,&btlvCore->mDebugDispLyt);
  item = gfl2::debug::DebugWin_AddItemValueBool(drawGroup,heap, L"DispBg",&btlvCore->mDebugDispBg);
  item->SetChangedValueFunc(BtlvDebug_DrawGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueBool(drawGroup,heap, L"DispWeather",&btlvCore->mDebugDispWeather);
  item->SetChangedValueFunc(BtlvDebug_DrawGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueBool(drawGroup,heap, L"DispMask",&btlvCore->mDebugDispMask);
  item->SetChangedValueFunc(BtlvDebug_DrawGroupUpdateCallback,btlvCore);
  item = gfl2::debug::DebugWin_AddItemValueInt( drawGroup ,heap, L"RenderPath", &btlvCore->mDebugRenderPathCheck , -1, 50 );
  
  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *enshutuGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData, msg_dmenu_battle_enshutu,topGroup);
  gfl2::debug::DebugWin_AddItemValueBool(enshutuGroup,heap, pMsgData,msg_dmenu_battle_enshutu_waitcam,&btlvCore->mIsEnableWaitCamera);

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *lightGroup = gfl2::debug::DebugWin_AddGroup(heap, pMsgData,msg_dmenu_battle_light,topGroup);
  gfl2::debug::DebugWin_AddItemValueBool(lightGroup,heap, pMsgData, msg_dmenu_battle_light_enable,&btlvCore->mDebugUseLight);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, L"Model",&btlvCore->mDebugModelLightVec,-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_poke1,&btlvCore->mDebugPokeLightVec[0],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_poke2,&btlvCore->mDebugPokeLightVec[1],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_poke3,&btlvCore->mDebugPokeLightVec[2],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_poke4,&btlvCore->mDebugPokeLightVec[3],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_trainer1,&btlvCore->mDebugCharaLightVec[0],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_trainer2,&btlvCore->mDebugCharaLightVec[1],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_trainer3,&btlvCore->mDebugCharaLightVec[2],-10.0f,10.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueVector3(lightGroup,heap, pMsgData,msg_dmenu_battle_light_trainer4,&btlvCore->mDebugCharaLightVec[3],-10.0f,10.0f,0.1f);

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *tickGroup = gfl2::debug::DebugWin_AddGroup(heap, L"Tick",topGroup);
  gfl2::debug::DebugWin_AddItemValueBool(tickGroup,heap, pMsgData, msg_dmenu_battle_print_tick,&btlvCore->mDebugPrintTick);
  gfl2::debug::DebugWin_AddItemValueInt( tickGroup,heap, L"PrintInterval", &btlvCore->mDebugPrintTickInterval, 1, 6000 );
  gfl2::debug::DebugWin_AddItemValueBool(tickGroup,heap, L"CheckTick",&btlvCore->mDebugTickCheck);
  gfl2::debug::DebugWin_AddItemValueBool(tickGroup,heap, L"CheckTickSe",&btlvCore->mDebugTickCheckSe);
  gfl2::debug::DebugWin_AddItemValueInt( tickGroup,heap, L"LimitCPU", &btlvCore->mDebugTickLimitCPU, -1, 70000 );
  gfl2::debug::DebugWin_AddItemValueInt( tickGroup,heap, L"LimitGPU", &btlvCore->mDebugTickLimitGPU, -1, 70000 );

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *waitCamGroup = gfl2::debug::DebugWin_AddGroup( heap, L"Wait Camera", topGroup );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Data Index number : ", &btlvCore->mDebugWaitCamDataIdx , -1, WAIT_CAMERA_DATA_NUM-1 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Heat Level : ", &btlvCore->mDebugWaitCamHeatLevel, -1, 5 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Target Pokemon : ", &btlvCore->mDebugWaitCamTargetPokemon, -1, POKE_MODEL_NUM-1 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Target Trainer : ", &btlvCore->mDebugWaitCamTargetTrainer, -1, TRAINER_MODEL_NUM-1 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Target Pokemon(end) : ", &btlvCore->mDebugWaitCamTargetPokemon_end, -1, POKE_MODEL_NUM-1 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Target Trainer(end) : ", &btlvCore->mDebugWaitCamTargetTrainer_end, -1, TRAINER_MODEL_NUM-1 );
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Reverse : ", &btlvCore->mDebugWaitCamReverse , -1, 1);
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"Camera Start Wait Frame : ", &btlvCore->mDebugWaitCamStartWaitFrame , -1, 500);
  gfl2::debug::DebugWin_AddItemValueInt( waitCamGroup ,heap, L"First play Data Index number : ", &btlvCore->mDebugWaitCamFirstPlayDataIdx, -1, WAIT_CAMERA_DATA_NUM-1 );

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *zGroup = gfl2::debug::DebugWin_AddGroup( heap, L"ZAura", topGroup );
  gfl2::debug::DebugWin_AddItemValueBool(zGroup,heap, L"Enable",&btlvCore->mDebugFPEnableSetting);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"RimR",&btlvCore->mDebugFPRimColorR,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"RimG",&btlvCore->mDebugFPRimColorG,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"RimB",&btlvCore->mDebugFPRimColorB,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"RimPow",&btlvCore->mDebugFPRimPow,0,255.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"RimWidth",&btlvCore->mDebugFPRimWidth,0,255.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueU32(zGroup,heap,L"RimSpeed",&btlvCore->mDebugFPRimMoveSpeed,0,65535);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"mRimMoveWidth",&btlvCore->mDebugFPRimMoveWidth,0,255.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraR",&btlvCore->mDebugFPAuraColorR,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraG",&btlvCore->mDebugFPAuraColorG,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraB",&btlvCore->mDebugFPAuraColorB,0,1.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraScaleX",&btlvCore->mDebugFPAuraScaleX,0,32.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraScaleY",&btlvCore->mDebugFPAuraScaleY,0,32.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueU32(zGroup,heap,L"AuraDensity",&btlvCore->mDebugFPAuraDensity,0,65535);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraScaleX(Base)",&btlvCore->mDebugFPAuraBaseScaleX,0,32.0f,0.1f);
  gfl2::debug::DebugWin_AddItemValueFloat(zGroup,heap,L"AuraScaleY(Base)",&btlvCore->mDebugFPAuraBaseScaleY,0,32.0f,0.1f);

  //----------------------------------------------------------------------------------------------
  gfl2::debug::DebugWinGroup *pokeDataGroup = gfl2::debug::DebugWin_AddGroup(heap, L"PokeData", topGroup);

  if (btlvCore->GetMainModule())
  {
    for (int i = 0; i < 4; i++)
    {
      if (btlvCore->GetMainModule()->IsExistClient(i))
      {
        AddPartyDebugGroup(btlvCore , pokeDataGroup, i);
      }
    }
  }

	//----------------------------------------------------------------------------------------------
	gfl2::debug::DebugWinGroup *optimizeGroup = gfl2::debug::DebugWin_AddGroup(heap, L"Optimize", topGroup);
	gfl2::debug::DebugWin_AddItemValueBool(optimizeGroup, heap, L"momijiOptimize", &BATTLE_SETUP_PARAM::isDebugUseMomijiOptimize);
}

void ExitBtlvDebug( BattleViewSystem *btlvCore )
{
  GFL_SAFE_DELETE(sBtlvDebugWorkStr);
  GFL_SAFE_DELETE(sTokuseiMsgData);
  GFL_SAFE_DELETE(sWazaMsgData);
  GFL_SAFE_DELETE(sItemMsgData);
  gfl2::debug::DebugWin_RemoveGroupByName(L"Battle");
}

void UpdateBtlvDebug(BattleViewSystem *btlvCore )
{
  if( btlvCore->mDebugPrintTick )
  {
    btlvCore->mDebugPrintTickCnt++;
    if( btlvCore->mDebugPrintTickCnt > btlvCore->mDebugPrintTickInterval )
    {
      btlvCore->mDebugPrintTickCnt = 0;
      const int update = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
      const int upnext = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
      const int draw   = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
      const int gpu    = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
      RomPrint("Tick[%6d][%6d][%6d][%6d]\n",update,upnext,draw,gpu);
    }
  }
  if( btlvCore->mDebugTickCheck || 
      btlvCore->mDebugTickCheckSe )
  {
    bool isOver = false;
    const int update = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
    const int upnext = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
    const int draw   = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
    const int gpu    = GFL_SINGLETON_INSTANCE( Debug::DebugWin::DebugWinWork )->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);
    if( btlvCore->mDebugTickLimitCPU != -1 &&
        update + upnext + draw >= btlvCore->mDebugTickLimitCPU )
    {
      isOver = true;
    }
    if( btlvCore->mDebugTickLimitGPU != -1 &&
        gpu >= btlvCore->mDebugTickLimitGPU )
    {
      isOver = true;
    }

    if( isOver == true )
    {
      if( btlvCore->mDebugTickCheck )
      {
        RomPrint("TickOver![%6d][%6d][%6d][%6d]\n",update,upnext,draw,gpu);
      }
      if( btlvCore->mDebugTickCheckSe )
      {
        Sound::PlaySE( SEQ_SE_BEEP );
      }
    }
  }
}

void BtlvDebug_ObjGroupUpdateCallback_Preset( void* work )
{
  BattleViewSystem *btlvCore = static_cast<BattleViewSystem*>(work);
  switch(btlvCore->mDebugObjPreset)
  {
    //シングル
  case 0:
    btlvCore->GetPokeModelObj(0)->SetPosition( 0,0, 220 );
    btlvCore->GetPokeModelObj(0)->SetRotateY(180);
    btlvCore->GetPokeModelObj(1)->SetPosition( 0,0,-220 );
    btlvCore->GetPokeModelObj(1)->SetRotateY(0);

    btlvCore->GetTrainerModelObj(0)->SetPosition( -70,0, 500 );
    btlvCore->GetTrainerModelObj(0)->SetRotateY(180);
    btlvCore->GetTrainerModelObj(1)->SetPosition(  70,0,-500 );
    btlvCore->GetTrainerModelObj(1)->SetRotateY(  0);

    btlvCore->GetCameraSys()->SetPosition( gfl2::math::Vector3(256,106,465),gfl2::math::Vector3(-45,70,-20) );
    break;
    //ダブル
  case 1:
    btlvCore->GetPokeModelObj(0)->SetPosition( 150,0, 220 );
    btlvCore->GetPokeModelObj(0)->SetRotateY(180);
    btlvCore->GetPokeModelObj(1)->SetPosition(-150,0,-220 );
    btlvCore->GetPokeModelObj(1)->SetRotateY(0);
    btlvCore->GetPokeModelObj(2)->SetPosition(-150,0, 220 );
    btlvCore->GetPokeModelObj(2)->SetRotateY(180);
    btlvCore->GetPokeModelObj(3)->SetPosition( 150,0,-220 );
    btlvCore->GetPokeModelObj(3)->SetRotateY(0);

    btlvCore->GetTrainerModelObj(0)->SetPosition(-100,0, 500 );
    btlvCore->GetTrainerModelObj(0)->SetRotateY(180);
    btlvCore->GetTrainerModelObj(1)->SetPosition( 100,0,-500 );
    btlvCore->GetTrainerModelObj(1)->SetRotateY(  0);
    btlvCore->GetTrainerModelObj(2)->SetPosition( 100,0, 500 );
    btlvCore->GetTrainerModelObj(2)->SetRotateY(180);
    btlvCore->GetTrainerModelObj(3)->SetPosition(-100,0,-500 );
    btlvCore->GetTrainerModelObj(3)->SetRotateY(  0);

    btlvCore->GetCameraSys()->SetPosition( gfl2::math::Vector3(260,180,630),gfl2::math::Vector3(-40,80,0) );
    break;
    //正面
  case 2:
    btlvCore->GetCameraSys()->SetPosition( gfl2::math::Vector3(0,100,300),gfl2::math::Vector3(0,100,0) );
    break;
  }
}

void BtlvDebug_CameraGroupUpdateCallback( void *work )
{
  BattleViewSystem *btlvCore = static_cast<BattleViewSystem*>(work);
  btlvCore->GetCameraSys()->SetUpdateCamera();
}

void BtlvDebug_DrawGroupUpdateCallback( void *work )
{
  BattleViewSystem *btlvCore = static_cast<BattleViewSystem*>(work);
  //btlvCore->GetRenderSys()->GetRenderingPipeLine()->SetEdgeMode((MyRenderingPipeLine::EdgeMode)btlvCore->mDebugEdgeMode);
  btlvCore->GetBgSystem()->SetVisible(btlvCore->mDebugDispBg);
  btlvCore->SetVisibleWeather(btlvCore->mDebugDispWeather);
  btlvCore->GetEdgeMaskModelObj()->SetVisible(btlvCore->mDebugDispMask);
  btlvCore->GetFloorMaskModelObj()->SetVisible(btlvCore->mDebugDispMask);
}

extern wchar_t* BtlvDebug_DrawPokeName(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeTokusei(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeItem(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeWaza1(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeWaza2(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeWaza3(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeWaza4(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_HP(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_ATK(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_DEF(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_S_A(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_S_D(void* work, gfl2::debug::DebugWinItem *item);
extern wchar_t* BtlvDebug_DrawPokeParam_SPD(void* work, gfl2::debug::DebugWinItem *item);

void AddPartyDebugGroup(BattleViewSystem *btlvCore, gfl2::debug::DebugWinGroup *top, int clientIdx)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG);

  gfl2::debug::DebugWinItem* item;

  wchar_t grpName1[7] = { L'P', L'a', L'r', L't', L'y', L'1', L'\0' };
  grpName1[5] += clientIdx;
  gfl2::debug::DebugWinGroup *partyGroup = gfl2::debug::DebugWin_AddGroup(heap, grpName1, top);

  BTL_PARTY *party = btlvCore->GetBattleContainer()->GetPartyData(clientIdx);
  for (int i = 0; i < party->GetMemberCount(); i++)
  {
    wchar_t grpName[6] = { L'P', L'o', L'k', L'e', L'1', L'\0' };
    grpName[4] += i;

    gfl2::debug::DebugWinGroup *pokeGroup = gfl2::debug::DebugWin_AddGroup(heap, grpName, partyGroup);

    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeName);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeTokusei);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeItem);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeWaza1);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeWaza2);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeWaza3);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeWaza4);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_HP);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_ATK);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_DEF);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_S_A);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_S_D);
    item = gfl2::debug::DebugWin_AddItemUpdateFunc(pokeGroup, heap, L"", party->GetMemberData(i), NULL, BtlvDebug_DrawPokeParam_SPD);
  }

}

wchar_t* BtlvDebug_DrawPokeName(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::personal::GetMonsName(sBtlvDebugWorkStr, static_cast<MonsNo>(bpp->GetMonsNo()));

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"MonsNo:%3d[%ls]"
    , bpp->GetMonsNo()
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeTokusei(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sTokuseiMsgData->GetString(bpp->GetViewSrcData()->GetTokuseiNo(), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Tokusei:%3d[%ls]"
    , bpp->GetViewSrcData()->GetTokuseiNo()
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeItem(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sItemMsgData->GetString(bpp->GetViewSrcData()->GetItem(), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Item:%3d[%ls]"
    , bpp->GetViewSrcData()->GetTokuseiNo()
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeWaza1(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sWazaMsgData->GetString(bpp->GetViewSrcData()->GetWazaNo(0), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Waza1:%3d[%ls]"
    , bpp->GetViewSrcData()->GetWazaNo(0)
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeWaza2(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sWazaMsgData->GetString(bpp->GetViewSrcData()->GetWazaNo(1), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Waza2:%3d[%ls]"
    , bpp->GetViewSrcData()->GetWazaNo(1)
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeWaza3(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sWazaMsgData->GetString(bpp->GetViewSrcData()->GetWazaNo(2), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Waza3:%3d[%ls]"
    , bpp->GetViewSrcData()->GetWazaNo(2)
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeWaza4(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  sWazaMsgData->GetString(bpp->GetViewSrcData()->GetWazaNo(3), *sBtlvDebugWorkStr);

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"Waza4:%3d[%ls]"
    , bpp->GetViewSrcData()->GetWazaNo(3)
    , sBtlvDebugWorkStr->GetPtr());

  return workStr;
}

wchar_t* BtlvDebug_DrawPokeParam_HP(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_HP;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"HP :%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}

extern wchar_t* BtlvDebug_DrawPokeParam_ATK(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_ATK;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"ATK:%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}


extern wchar_t* BtlvDebug_DrawPokeParam_DEF(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_DEF;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"DEF:%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}


extern wchar_t* BtlvDebug_DrawPokeParam_S_A(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_SPATK;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"S_A:%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}


extern wchar_t* BtlvDebug_DrawPokeParam_S_D(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_SPDEF;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"S_D:%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}


extern wchar_t* BtlvDebug_DrawPokeParam_SPD(void* work, gfl2::debug::DebugWinItem *item)
{
  BTL_POKEPARAM *bpp = static_cast<BTL_POKEPARAM*>(work);

  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();

  pml::pokepara::PowerID dispID = pml::pokepara::POWER_AGI;

  swprintf(workStr, gfl2::debug::DEBUGWIN_NAME_LEN
    , L"AGI:%3d[%2d][%3d]"
    , bpp->GetViewSrcData()->GetPower(dispID)
    , bpp->GetViewSrcData()->GetTalentPower(dispID)
    , bpp->GetViewSrcData()->GetEffortPower(dispID));

  return workStr;
}





#if BTLV_TICK_CALC

BtlvTickCalc sBtlvTickArr[20];

void _BtlvTickStart(int idx , char *name )
{
  sBtlvTickArr[idx].Start(name);
}
void _BtlvTickEnd(int idx , char *name)
{
  sBtlvTickArr[idx].End();
}

void _BtlvTickPrint(void)
{
  for( int i=0;i<GFL_NELEMS(sBtlvTickArr);i++ )
  {
    RomPrint("[%2d][%16s][%5d]\n",i,sBtlvTickArr[i].mName,(int)sBtlvTickArr[i].mTick);
  }
}
#endif //BTLV_TICK_CALC

GFL_NAMESPACE_END(btl)

#endif //PM_DEBUG
