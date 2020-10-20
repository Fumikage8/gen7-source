//======================================================================
/**
 * @file	btlv_seq_com.h
 * @brief	バトル描画 シーケンスコマンド
 * @author	ariizumi
 * @data	15/4/22
 */
//======================================================================
#include <heap/include/gfl2_Heap.h>
#include <fade/include/gfl2_FadeManager.h>

#include "sound/include/Sound.h"
#include "AppLib/include/Fade/DemoFade.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#include "DemoLib/Object/include/TaskFunc.h"
#include "GameSys/include/GameData.h"
#include "PokeTool/include/PokeToolRotomSound.h"

#include "../btl_mainmodule.h"
#include "btlv_local_def.h"
#include "btlv_core.h"
#include "btlv_camera.h"
#include "btlv_seq_sys.h"
#include "btlv_seq_com.h"
#include "btlv_task.h"
#include "btlv_ui.h"
#include "btlv_seq_com_define.h"
#include "btlv_trainer_data.h"

#include <arc_def_index/arc_def.h>
#include <niji_conv_header/message/msg_btl_attack.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_set.h>
// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/p1_base.h>  //モーション種類

namespace btl
{

#if( BTLV_DEBUG && !defined( DEBUG_ONLY_FOR_obata_toshihiro ) )
#define USE_DEBUG 1
#else
#define USE_DEBUG 0
#endif
  
  
#if USE_DEBUG
#define DUMP_COMMAND if(((btl::BattleViewSystem*)(userWork))->IsPrintSequence()){ GFL_PRINT("Command[%d][%s] [%d]-[%d] grp[%d][%d]\n",param.commandNo,comName[param.commandNo],param.startFrame,param.endFrame,param.groupNo,param.groupOpt);}
#else
#define DUMP_COMMAND //Dummy
#endif

#define BTL_SEQ_DEF(name) #name ,

static const char *const comName[BTLV_SEQCOM_MAX]=
{
#include "btlv_seq_com.cdat"
};

#define BTL_SEQ_FUNC_DEF(name) void BTLV_SEQCOM_ ## name ( DemoLib::SeqEditor::SeqFile *file , void *userWork , const DemoLib::SeqEditor::CommandParam &param )

//------------------------------------------------------------------------------------------------------------
//ポケモン関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelPokemon )
{
  DUMP_COMMAND;
}

//ポケモン移動(座標)
BTL_SEQ_FUNC_DEF( PokemonMovePosition )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 tempPos;
  int trgPoke;
  bool isRelative,isRot,isFlip;

  file->LoadDataInt( &trgPoke );
  file->LoadDataVec3( &tempPos );
  file->LoadDataBool( &isRelative );
  file->LoadDataBool( &isRot );
  file->LoadDataBool( &isFlip );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvPos vPosMain = core->SeqComFunc_GetTargetPoke(trgPoke,0); //反転がまちまちにならないように・・・
  BtlvPos vPosSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);

  if( isFlip &&
    core->SeqComFunc_IsFlipEffect(vPosMain,vPosSub) )
  {
    tempPos.x *= -1;
  }

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  for( int i=0;i<trgNum;i++)
  {
    gfl2::math::Vector3 pos = tempPos;
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    if( isRot == true )
    {
      BtlvRot_Deg( pos.x,pos.z,-poke->GetRotateY() );
    }

    if( isRelative == true )
    {
      pos += poke->GetPosition();
    }
    if( frame == 0 )
    {
      poke->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,poke,poke->GetPosition(),pos,frame);
    }
  }
}

//ポケモン移動(ポケモン相対)
BTL_SEQ_FUNC_DEF( PokemonMoveRelativePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;


  BTLV_RELARIVE_POKE_OPTION opt;
  int movePoke,trgPoke;

  file->LoadDataInt( &movePoke );
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.rate );
  file->LoadDataInt( &opt.adjustFlip );
  //file->LoadDataInt( &opt.adjustScale );
  opt.adjustScale = false;  //ポケモンの場合は補正スケールは要らない！
  file->LoadDataInt( &opt.autoRotateModel );
  file->LoadDataInt( &opt.autoRotatePos );
  file->LoadDataBool( &opt.forceUpdate );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  for( int i=0;i<trgNum;i++)
  {
    opt.vpos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);

    BtlvObject_PokeModel *poke = core->GetPokeModel(core->SeqComFunc_GetTargetPoke(movePoke,i));
    core->SeqComFunc_MoveRelativePoke(poke,frame,opt);
  }
}

//ポケモン位置初期化
BTL_SEQ_FUNC_DEF( PokemonMoveReset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;

  file->LoadDataInt( &trgPoke );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++ )
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    core->ResetPokemon(vPos,frame);
  }
}

//ポケモン位置初期化(全部)
BTL_SEQ_FUNC_DEF( PokemonMoveResetAll )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    core->ResetPokemon((BtlvPos)i,frame);
  }
}

//ポケモン回転
BTL_SEQ_FUNC_DEF( PokemonRotate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  gfl2::math::Vector3 rotBase;
  bool isRelative;
  bool isFlip;

  file->LoadDataInt( &trgPoke );
  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );
  file->LoadDataBool( &isFlip );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvPos vPosSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      if( isFlip &&
          core->SeqComFunc_IsFlipEffect(vPos,vPosSub) )
      {
        rot *= -1;
      }
      rot += poke->GetRotate();
    }

    if( frame == 0 )
    {
      poke->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,poke,poke->GetRotate(),rot,frame);
    }
  }
}

//ポケモン回転ポケモン指定
BTL_SEQ_FUNC_DEF( PokemonRotatePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke,dirPoke;
  int dirNode;
  f32 ofs;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &dirPoke );
  file->LoadDataInt( &dirNode );
  file->LoadDataF32( &ofs );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  BtlvPos dirVPos = core->SeqComFunc_GetTargetPoke(dirPoke);

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    gfl2::math::Vector3 rot = poke->GetRotate();

    core->SeqComFunc_CalcPokeDir(poke->GetPosition(),dirVPos,dirNode,&rot,false);

    rot.y += ofs;
    if( frame == 0 )
    {
      poke->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,poke,poke->GetRotate(),rot,frame);
    }
  }
}

//ポケモン回転(特殊座標)
BTL_SEQ_FUNC_DEF( PokemonRotateSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;

  int trgPoke,posTrg;
  f32 rotOfs;
  bool isRotate,isFlip;
  gfl2::math::Vector3 posOfs;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &posTrg );
  file->LoadDataVec3( &posOfs );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRotate );
  file->LoadDataF32( &rotOfs );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  gfl2::math::Vector3 pos,rot;
  core->SeqComFunc_GetSpecialPos(posTrg,&pos,&rot);
  BtlvPos targetVpos;
  switch( posTrg )
  {
  case SEQ_DEF_SPPOS_SELF_FIELD:
    targetVpos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    break;
  case SEQ_DEF_SPPOS_ENEMY_FIELD:
  case SEQ_DEF_SPPOS_ENEMY_CENTER:
    targetVpos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
    break;
  }

  if( isFlip && core->SeqComFunc_IsFlipEffect(targetVpos,targetVpos) )
  {
    posOfs.x *= -1;
  }
  if( isRotate )
  {
    BtlvRot_Deg( posOfs.x,posOfs.z,-rot.y );
  }
  pos += posOfs;

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    gfl2::math::Vector3 retRot;
    const gfl2::math::Vector3 pokeRot = poke->GetRotate();

    core->SeqComFunc_CalcPosDir(poke->GetPosition(),pos,&retRot,false);
    retRot.y += rotOfs;

    if( retRot.y - pokeRot.y > 180 )
    {
      retRot.y -= 360;
    }
    if( retRot.y - pokeRot.y < -180 )
    {
      retRot.y += 360;
    }

    if( frame == 0 )
    {
      poke->SetRotate(retRot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,poke,pokeRot,retRot,frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( PokemonRotateOrder )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  int rotOrder;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &rotOrder );

  //シーケンスでCOMBO_TEXTのデフォルトがうまく動いていない・・・
  if( rotOrder == SEQ_DEF_ROTATE_ORDER_DEF )
  {
    rotOrder = gfl2::math::EulerRotation::RotationOrderZYX;
  }
  else
  {
    rotOrder--;
  }

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    poke->SetRotationOrder(static_cast<gfl2::math::EulerRotation::RotationOrder>(rotOrder));
  }

}

//ポケモン拡縮
BTL_SEQ_FUNC_DEF( PokemonScale )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataInt( &trgPoke );
  file->LoadDataVec3( &sclBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative == true )
    {
      scl = scl.MulPerElem(poke->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 )
    {
      poke->SetScale(scl);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,poke,poke->GetScale(),scl,frame);
    }
  }
}

//ポケモン表示切替
BTL_SEQ_FUNC_DEF( PokemonVisible )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  bool isDisp;

  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &isDisp );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    if( vPos != BTL_VPOS_ERROR )
    {
       BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

       poke->SetVisible(isDisp);
    }
  }
}

//ポケモン表示切替(非対象者)
BTL_SEQ_FUNC_DEF( PokemonVisibleOther )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  bool isVisible;
  file->LoadDataBool( &isVisible );

  bool checkArr[POKE_MODEL_NUM];
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( core->GetPokeModel(static_cast<BtlvPos>(i))->IsEnable() )
    {
      checkArr[i] = true;
    }
    else
    {
      checkArr[i] = false;
    }
  }
  
  //攻撃側
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    if( vPos != BTL_VPOS_ERROR )
    {
      checkArr[static_cast<int>(vPos)] = false;
    }
  }
  //防御側
  int trgNum = core->SeqComFunc_GetTargetPokeNum(false);
  for( int i=0;i<trgNum;i++ )
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF,i);
    if( vPos != BTL_VPOS_ERROR )
    {
      checkArr[static_cast<int>(vPos)] = false;
    }
  }
  
  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    if( checkArr[i] )
    {
      BtlvObject_PokeModel *mdl = core->GetPokeModel(static_cast<BtlvPos>(i));
      if( mdl && mdl->IsEnable() )
      {
        mdl->SetVisible( isVisible ); 
      }
    }
  }

}

//ポケモン表示切替(全部)
BTL_SEQ_FUNC_DEF( PokemonVisibleAll )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  bool isDisp;
  file->LoadDataBool( &isDisp );

  for( int i=0;i<POKE_MODEL_NUM;i++ )
  {
    BtlvObject_PokeModel *poke = core->GetPokeModel(static_cast<BtlvPos>(i));
    if( poke && poke->IsEnable() )
    {
      poke->SetVisible(isDisp);
    }
  }
}


//ポケモン表示切替(影)
BTL_SEQ_FUNC_DEF( PokemonVisibleShadow )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  bool isDisp;

  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &isDisp );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    poke->SetShadowVisible(isDisp);
  }
}

//ポケモンモーション切り替え
BTL_SEQ_FUNC_DEF( PokemonMotion )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  int animeType;
  int isLoop;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &animeType );
  file->LoadDataInt( &isLoop );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    //攻撃形態指定モーション
    if( animeType >= POKE_MOTION_ATK_BUTSURI )
    {
      BTLV_WAZA_EFF_PARAM *wazaParam = core->GetWazaParam();
      int keyFrame = param.startFrame;

      animeType = core->SeqComFunc_GetWazaMotionType( poke->GetMonsNo() , 
                  poke->GetFormNo() , 
                  core->GetWazaParam()->mId,
                  animeType-POKE_MOTION_ATK_BUTSURI );

      if( animeType != 10 )
      {
        animeType += PokeTool::MODEL_ANIME_BUTURI1;
      }
      else
      {
        animeType = PokeTool::MODEL_ANIME_BT_WAIT_A;
      }
    }
    if( animeType == PokeTool::MODEL_ANIME_BT_WAIT_A )
    {
      poke->ChangeAnimationDirect(animeType,false);
    }
    else
    {
      poke->ChangeAnimationDirect(animeType,true);
    }
    if( isLoop )
    {
      poke->SetAnimationIsLoop(true);
    }
    else
    {
      if( animeType != MOTION_DOWN )
      {
        poke->SetReserveAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
      }
    }
  }
}

//ポケモンモーション設定(外部
BTL_SEQ_FUNC_DEF( PokemonMotionResource )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  int arcIdx;
  int isLoop;
  int slot;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &arcIdx );
  file->LoadDataInt( &isLoop );
  file->LoadDataInt( &slot );

  GFL_ASSERT_MSG(slot<2,"SeqCommand 外部モーション再生のスロットは１まで！\n");

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_WAZAEFF_RESOURCE,arcIdx);

    if( slot != 0 )
    {
      //@attention 9末対応 モーション無いのにSlot1にアニメ流すとまずい
      if( poke->GetPokeModel()->IsAvailableAnimationDirect(0) == false )
      {
        return;
      }
    }

    poke->ChangeAnimationResource(anmRes,slot);

    if( isLoop )
    {
      poke->SetAnimationIsLoop(true,slot);
    }
    else
    {
      poke->SetAnimationIsLoop(false,slot);
    }
  }
}

//ポケモンモーション設定(外部 ティキ用
BTL_SEQ_FUNC_DEF( PokemonMotionResourceTiki )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  int arcIdx[4];
  int isLoop;
  int slot;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &arcIdx[0] );
  file->LoadDataInt( &arcIdx[1] );
  file->LoadDataInt( &arcIdx[2] );
  file->LoadDataInt( &arcIdx[3] );
  file->LoadDataInt( &isLoop );
  file->LoadDataInt( &slot );

  GFL_ASSERT_MSG(slot<2,"SeqCommand 外部モーション再生のスロットは１まで！\n");

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  int tikiType = 0;
  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    tikiType = poke->GetTikiType();
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_WAZAEFF_RESOURCE,arcIdx[tikiType]);

    if( slot != 0 )
    {
      //@attention 9末対応 モーション無いのにSlot1にアニメ流すとまずい
      if( poke->GetPokeModel()->IsAvailableAnimationDirect(0) == false )
      {
        return;
      }
    }

    poke->ChangeAnimationResource(anmRes,slot);

    if( isLoop )
    {
      poke->SetAnimationIsLoop(true,slot);
    }
    else
    {
      poke->SetAnimationIsLoop(false,slot);
    }
  }
}

//ポケモンモーション破棄(外部
BTL_SEQ_FUNC_DEF( PokemonMotionStopResource )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  int slot;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &slot );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
    for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    poke->ChangeAnimationResource(NULL,slot);
  }
}

//ポケモン攻撃モーション
BTL_SEQ_FUNC_DEF( PokemonAttackMotion )
{
  DUMP_COMMAND;

  //ここで再生処理は無い。
  //実際は外で再生が予約されている。
}

//ポケモンモーション速度変更
BTL_SEQ_FUNC_DEF( PokemonSetMotionSpeed )
{
  DUMP_COMMAND;
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  f32 speed;
  
  file->LoadDataInt( &trgPoke );
  file->LoadDataF32( &speed );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    poke->SetAnimationSpeed(speed);
  }

}

//着地演出一式
BTL_SEQ_FUNC_DEF( PokemonIntroMotion )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;


  int trgPoke;
  
  file->LoadDataInt( &trgPoke );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  
  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    if( poke && poke->IsEnable() )
    {
      CreateTask_PokeIntroMotion( core->GetTaskSystemAlways(),heap,core,vPos,poke );
    }
  }
}

//ポケモンエッジ切り替え
BTL_SEQ_FUNC_DEF( PokemonEdgeEnable )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool enable;
  
  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &enable );
  
  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    if( enable )
    {
      core->GetRenderSys()->GetRenderingPipeLine()->AddEdge_Poke(poke->GetPokeModel());
    }
    else
    {
      core->GetRenderSys()->GetRenderingPipeLine()->RemoveEdge_Poke(poke->GetPokeModel());
    }
  }
}

//ポケモンカラーシェーダー設定
BTL_SEQ_FUNC_DEF( PokemonShaderCol )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  gfl2::math::Vector3 sColTemp;
  f32 sAlpha;
  gfl2::math::Vector3 eColTemp;
  f32 eAlpha;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  file->LoadDataInt( &trgPoke );
  file->LoadDataVec3( &sColTemp );
  file->LoadDataF32( &sAlpha );
  file->LoadDataVec3( &eColTemp );
  file->LoadDataF32( &eAlpha );

  gfl2::math::Vector4 sCol( sColTemp.x,sColTemp.y,sColTemp.z,sAlpha);
  gfl2::math::Vector4 eCol( eColTemp.x,eColTemp.y,eColTemp.z,eAlpha);

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    if( frame == 0 )
    {
      // バトルの処理落ち時の色の設定は, 処理落ちを考慮して, 3フレーム実行するように設定
      static const s32 s_BattleColorShaderExecuteFrame = 3;
      poke->SetShaderColor(eCol, s_BattleColorShaderExecuteFrame);
    }
    else
    {
      DemoLib::Obj::CreateTask_ChangePokeColorShader(core->GetTaskSystem(),heap,poke,sCol,eCol,frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( PokemonShaderFullPower )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  int isEnable;
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &isEnable );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  
  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    if( isEnable )
    {
      poke->AddFPShader(core->GetZenryokuParam()->isZenryoku);
    }
    else
    {
      poke->RemoveFpShader();
    }
  }
}

//ポケモンモデル追従
BTL_SEQ_FUNC_DEF( PokemonFollowMode )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
    
  int trgPoke;
  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  
  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<trgNum;i++)
    {
      BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
      BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
      poke->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    BtlvObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( trgNum != mdlNum )
    {
      GFL_ASSERT_MSG(0,"WazaEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[%d]\n",trgNum,mdlNum);
      return;
    }

    for( int i=0;i<trgNum;i++)
    {
      BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
      BtlvPos vPosSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);
      BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

      BtlvObject_Gfmdl *mdl = mdlArr[i];
      poke->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      if( core->SeqComFunc_IsFlipEffect(vPos,vPosSub) == true )
      {
        posOfs.x *= -1;
      }

      poke->SetOffset(posOfs);
      poke->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(poke);
      core->GetObjectManager()->AddObject(poke,mdl->GetPriority()+1);
    }
  }
}

//ポケモン特殊移動
BTL_SEQ_FUNC_DEF( PokemonSpMoveReset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgPoke;
  file->LoadDataInt( &trgPoke );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  
  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    gfl2::math::Vector3 pos(0,0,0);
    if( frame == 0 )
    {
      poke->SetOffset(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMoveOffset(core->GetTaskSystem(),heap,poke,poke->GetOffset(),pos,frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( PokemonSpMoveShake )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgPoke;
  f32 sRate,eRate;
  f32 sDec,eDec;
  int tempAxis;
  bool isFlip;
  
  file->LoadDataInt( &trgPoke );
  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &sDec );
  file->LoadDataF32( &eDec );
  file->LoadDataInt( &tempAxis );
  file->LoadDataBool( &isFlip );

  BtlvPos vPosMain = core->SeqComFunc_GetTargetPoke(trgPoke,0); //反転がまちまちにならないように・・・
  BtlvPos vPosSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);

  if( isFlip &&
    core->SeqComFunc_IsFlipEffect(vPosMain,vPosSub) )
  {
    if( tempAxis == SEQ_DEF_AXIS_X ||
        tempAxis == SEQ_DEF_AXIS_X_ROT )
    {
      //反転フラグを渡すのではなくsinの波をずらして対応
      sDec += 0.5f;
      eDec += 0.5f;
    }
  }


  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  case SEQ_DEF_AXIS_X_ROT:
    axis = DemoLib::Obj::AXIS_X_ROT;
    break;
  case SEQ_DEF_AXIS_Z_ROT:
    axis = DemoLib::Obj::AXIS_Z_ROT;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
  }

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  for( int i=0;i<trgNum;i++)
  {

    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    DemoLib::Obj::CreateTask_ObjMoveShake(core->GetTaskSystem(),heap,poke,sRate,eRate,sDec*360,eDec*360,frame,axis );
  }
}

//ポケモン原寸スケール設定
BTL_SEQ_FUNC_DEF( PokemonSetOriginScale )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataInt( &trgPoke );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    const f32 adjustScale = poke->GetPokeModel()->GetAdjustScale();
    const f32 scale = 1.0f/adjustScale;

    poke->SetScale(scale);
  }
}

//ポケモン浮き許可(ディグダ)
BTL_SEQ_FUNC_DEF( PokemonSetEnableFloat )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool isEnable;
  bool isDisp;

  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &isEnable );
  file->LoadDataBool( &isDisp );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    poke->SetEnableFloat(isEnable);
    if( isEnable == true )
    {
      poke->SetVisibleDigudaStone(isDisp);
    }
    else
    {
      poke->SetVisibleDigudaStone(false);
    }
  }
}

//ポケモンレアエフェクト再生
BTL_SEQ_FUNC_DEF( PokemonRareEffect )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;

  file->LoadDataInt( &trgPoke );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();
  CreateTask_PokeRareEffect( core->GetTaskSystemAlways(),heap,core );
}

//ポケモン振り向き
BTL_SEQ_FUNC_DEF( PokemonLookAt )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgPoke,node;
  gfl2::math::Vector3 ofs;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &node );
  file->LoadDataVec3( &ofs );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    BTLV_RELARIVE_POKE_OPTION opt;
    opt.node = node;
    opt.offset = ofs;
    opt.rate = 100;
    opt.adjustFlip = true;
    opt.adjustScale = false;
    opt.autoRotateModel = false;
    opt.autoRotatePos = true;
    opt.forceUpdate = false;
    opt.vpos = vPos;
    opt.vposSub = core->SeqComFunc_GetTargetPokeSub(vPos);

    opt.offset = ofs;
    gfl2::math::Vector3 retPos;
    gfl2::math::Vector3 retTrg;
    gfl2::math::Vector3 retRot;
    gfl2::math::Vector3 retScale;

    core->SeqComFunc_GetPokeRelativePos( opt , &retPos , &retRot , &retScale , true );

    CreateTask_PokeLookAt( core->GetTaskSystemAlways(),heap,core,poke,retPos,frame );
  }
}

//ポケモン表情制御
BTL_SEQ_FUNC_DEF( PokemonSetEye )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke,type;
  bool isSomePoke;  //一部ポケモンのみ対象

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &type );
  file->LoadDataBool( &isSomePoke );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    bool enable = true;
    if( isSomePoke == true )
    {
      int monsNo = poke->GetMonsNo();
      if( monsNo == MONSNO_PIKATYUU ||
          monsNo == MONSNO_TORA1 || monsNo == MONSNO_TORA2 || monsNo == MONSNO_TORA3 ||
          monsNo == MONSNO_ASIKA1 || monsNo == MONSNO_ASIKA2 || monsNo == MONSNO_ASIKA3 || 
          monsNo == MONSNO_HUKUROU1 || monsNo == MONSNO_HUKUROU2 || monsNo == MONSNO_HUKUROU3 )
      {
        enable = true;
      }
      else
      {
        enable = false;
      }
    }

    if( enable )
    {
      if( type == POKE_EYE_RESET )
      {
        poke->ResetOverrideEyeType();
      }
      else
      {
        poke->SetOverrideEyeType(type);
      }
    }
  }
}
//------------------------------------------------------------------------------------------------------------
//カメラ関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelCamera )
{
  DUMP_COMMAND;
}

BTL_SEQ_FUNC_DEF( CameraMovePosition )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int isRelative,opt;

  file->LoadDataVec3( &pos );
  file->LoadDataVec3( &trg );
  file->LoadDataF32( &fov );
  file->LoadDataInt( &opt );
  file->LoadDataInt( &isRelative );

  if( isRelative )
  {
    pos += core->GetCameraSys()->GetPosition();
    trg += core->GetCameraSys()->GetTarget();
  }

  if( frame == 0 )
  {
    //全力のポーズの解除
    core->ResetZenryokuTrainerMotion();
    core->GetCameraSys()->SetPosition(pos,trg);
    if(fov != 0)
    {
      core->GetCameraSys()->SetFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetTempHeap(),core,pos,trg,fov,0,frame,(DemoLib::Obj::TASK_MOVE_TYPE)opt);
  }
}

BTL_SEQ_FUNC_DEF( CameraMoveRelativePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  BTLV_RELARIVE_POKE_OPTION opt;
  gfl2::math::Vector3 pos,trg;
  f32 fov;
  int trgPoke,moveOpt;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &pos );
  file->LoadDataVec3( &trg );
  file->LoadDataF32( &fov );
  file->LoadDataInt( &opt.rate );
  file->LoadDataInt( &opt.adjustFlip );
  file->LoadDataInt( &opt.adjustScale );
  opt.autoRotateModel = false;
  file->LoadDataInt( &opt.autoRotatePos );
  file->LoadDataBool( &opt.forceUpdate );
  file->LoadDataInt( &moveOpt );

  opt.vpos = core->SeqComFunc_GetTargetPoke(trgPoke);
  opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trgPoke);

  gfl2::math::Vector3 retPos;
  gfl2::math::Vector3 retTrg;
  gfl2::math::Vector3 retRot;
  gfl2::math::Vector3 retScale;
  opt.offset = pos;
  core->SeqComFunc_GetPokeRelativePos( opt , &retPos , &retRot , &retScale , true );
  opt.offset = trg;
  core->SeqComFunc_GetPokeRelativePos( opt , &retTrg , &retRot , &retScale , true );

  if(opt.rate != 100)
  {
    gfl2::math::Vector3 camPos = core->GetCameraSys()->GetPosition();
    gfl2::math::Vector3 camTrg = core->GetCameraSys()->GetTarget();

    gfl2::math::Vector3 posOfs = retPos - camPos;
    posOfs *= (opt.rate*0.01f);
    retPos = camPos + posOfs;
    gfl2::math::Vector3 trgOfs = retTrg - camTrg;
    trgOfs *= (opt.rate*0.01f);
    retTrg = camTrg + trgOfs;
  }

  if( frame == 0 )
  {
    //全力のポーズの解除
    core->ResetZenryokuTrainerMotion();
    core->GetCameraSys()->SetPosition(retPos,retTrg);
    if(fov != 0)
    {
      core->GetCameraSys()->SetFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetTempHeap(),core,retPos,retTrg,fov,0,frame,(DemoLib::Obj::TASK_MOVE_TYPE)moveOpt);
  }

}

//カメラ移動(特殊座標)
BTL_SEQ_FUNC_DEF( CameraMoveSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 camPos,camTrg;
  f32 fov;
  int posTrg,moveOpt;
  bool isFlip,isRotate;

  file->LoadDataInt( &posTrg );
  file->LoadDataVec3( &camPos );
  file->LoadDataVec3( &camTrg );
  file->LoadDataF32( &fov );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRotate );
  file->LoadDataInt( &moveOpt );


  gfl2::math::Vector3 pos,rot;
  core->SeqComFunc_GetSpecialPos(posTrg,&pos,&rot);
  BtlvPos targetVpos;
  switch( posTrg )
  {
  case SEQ_DEF_SPPOS_SELF_FIELD:
    targetVpos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
    break;
  case SEQ_DEF_SPPOS_ENEMY_FIELD:
  case SEQ_DEF_SPPOS_ENEMY_CENTER:
    targetVpos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
    break;
  }

  if( isFlip && core->SeqComFunc_IsFlipEffect(targetVpos,targetVpos) )
  {
    camPos.x *= -1;
    camTrg.x *= -1;
  }
  if( isRotate )
  {
    BtlvRot_Deg( camPos.x,camPos.z,-rot.y );
    BtlvRot_Deg( camTrg.x,camTrg.z,-rot.y );
  }
  camPos += pos;
  camTrg += pos;

  if( frame == 0 )
  {
    //全力のポーズの解除
    core->ResetZenryokuTrainerMotion();
    core->GetCameraSys()->SetPosition(camPos,camTrg);
    if(fov != 0)
    {
      core->GetCameraSys()->SetFov(fov);
    }
  }
  else
  {
    CreateTask_MoveCamera(core->GetTaskSystem(),core->GetTempHeap(),core,camPos,camTrg,fov,0,frame,(DemoLib::Obj::TASK_MOVE_TYPE)moveOpt);
  }

}

//カメラリセット
BTL_SEQ_FUNC_DEF( CameraReset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int opt;

  file->LoadDataInt( &opt );

  core->ResetDefaultCamera(frame);
}

//カメラ回転(Trg中心)
BTL_SEQ_FUNC_DEF( CameraRotateTrg )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;

  int rotDeg,moveOpt;
  int isFlip,flipTrg;
  
  file->LoadDataInt( &rotDeg );
  file->LoadDataInt( &moveOpt );
  file->LoadDataInt( &isFlip );
  file->LoadDataInt( &flipTrg );
  
  if( isFlip )
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(flipTrg);
    if( core->SeqComFunc_IsFlipEffect(vPos,vPos) == false )
    {
      rotDeg *= -1;
    }
  }
  
  gfl2::math::Vector3 pos = core->GetCameraSys()->GetPosition();
  gfl2::math::Vector3 trg = core->GetCameraSys()->GetTarget();

  CreateTask_RotateCamera_CenterTrg( core->GetTaskSystem(),core->GetTempHeap(),core,pos,trg,GFL_MATH_DEG_TO_RAD(rotDeg),moveOpt,frame);
}

//カメラ回転(Pos中心)
BTL_SEQ_FUNC_DEF( CameraRotatePos )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;

  int rotDeg,moveOpt;
  int isFlip,flipTrg;
  
  file->LoadDataInt( &rotDeg );
  file->LoadDataInt( &moveOpt );
  file->LoadDataInt( &isFlip );
  file->LoadDataInt( &flipTrg );
  
  if( isFlip )
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(flipTrg);
    if( core->SeqComFunc_IsFlipEffect(vPos,vPos) == false )
    {
      rotDeg *= -1;
    }
  }
  
  gfl2::math::Vector3 pos = core->GetCameraSys()->GetPosition();
  gfl2::math::Vector3 trg = core->GetCameraSys()->GetTarget();

  CreateTask_RotateCamera_CenterPos( core->GetTaskSystem(),core->GetTempHeap(),core,pos,trg,GFL_MATH_DEG_TO_RAD(rotDeg),moveOpt,frame);
}

//カメラ回転(Pos中心)(ポケモン指定)T
BTL_SEQ_FUNC_DEF( CameraRotatePosPoke )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;

  int posTrg,moveOpt;

  file->LoadDataInt( &posTrg );
  file->LoadDataInt( &moveOpt );

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(posTrg);
  
  gfl2::math::Vector3 rot;
  gfl2::math::Vector3 pos;

  gfl2::math::Vector3 camPos = core->GetCameraSys()->GetPosition();
  gfl2::math::Vector3 camTrg = core->GetCameraSys()->GetTarget();

  //カメラの位置からポケモンへの回転を取る
  core->SeqComFunc_CalcPokeDir( camPos , vPos , SEQ_DEF_NODE_ORIGIN , &rot , false );

  //カメラの位置から注視点への回転を取る
  const gfl2::math::Vector3 ofs = camTrg - camPos;
  f32 comRotY = gfl2::math::Atan2Deg(ofs.x,ofs.z);

  //差分が必要な回転値
  f32 rotY = -(rot.y-comRotY);
  
  //回転方向の調整(180度基準)
  if( rotY > 180.0f )
  {
    rotY -= 180*2;
  }
  if( rotY < -180.0f )
  {
    rotY += 180*2;
  }

  /*
  ofs = camTrg-pos;
  BtlvRot_Rad( ofs.x,ofs.z,rot.y + PI);
  ofs.x*=-1;
  trg = pos+ofs;
  */  

  CreateTask_RotateCamera_CenterPos( core->GetTaskSystem(),core->GetTempHeap(),core,camPos,camTrg,GFL_MATH_DEG_TO_RAD(rotY),moveOpt,frame);

}

//カメラ揺らし
BTL_SEQ_FUNC_DEF( CameraShake )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  f32 sRate,eRate;
  f32 dec;
  int tempAxis;
  
  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &dec );
  file->LoadDataInt( &tempAxis );

  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
    break;
  }

  CreateTask_ShakeCamera(core->GetTaskSystem(),core->GetTempHeap(),core,sRate,eRate,dec,frame,axis );
}

//カメラアニメーション
BTL_SEQ_FUNC_DEF( CameraAnimetionPoke )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int envFile,motFile;
  BTLV_RELARIVE_POKE_OPTION opt;
  int trg;

  file->LoadDataInt( &envFile );
  file->LoadDataInt( &motFile );

  file->LoadDataInt( &trg );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.adjustFlip );
  file->LoadDataInt( &opt.adjustScale );
  file->LoadDataInt( &opt.autoRotatePos );
  opt.autoRotateModel = opt.autoRotatePos;
  file->LoadDataBool( &opt.forceUpdate );

  opt.rate = 0;
  opt.vpos = core->SeqComFunc_GetTargetPoke(trg);
  opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trg);

  envFile = core->CheckWazaDataId_Env(envFile);
  motFile = core->CheckWazaDataId_Camera(motFile);

  gfl2::math::Vector3 pos,rot,scale;

  core->SeqComFunc_GetPokeRelativePos(opt,&pos,&rot,&scale,true);

  void *envBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envFile);
  void *motBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,motFile);
  core->GetCameraSys()->StartCameraAnime(envBuf,motBuf);
  core->GetCameraSys()->SetCameraAnimeOffset(pos);

  if( opt.adjustFlip )
  {
    core->GetCameraSys()->SetCameraAnimeFlip(core->SeqComFunc_IsFlipEffect(opt.vpos,opt.vposSub));
  }
  core->GetCameraSys()->SetCameraAnimeRotateY_Rad( gfl2::math::ConvDegToRad(-rot.y));
  core->GetCameraSys()->SetCameraAnimeScale(scale);
  //全力のポーズの解除
  core->ResetZenryokuTrainerMotion();

}

//カメラアニメーション(座標
BTL_SEQ_FUNC_DEF( CameraAnimetionPosition )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int envFile,motFile;
  bool isFlip;
  gfl2::math::Vector3 pos;

  file->LoadDataInt( &envFile );
  file->LoadDataInt( &motFile );
  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isFlip );

  envFile = core->CheckWazaDataId_Env(envFile);
  motFile = core->CheckWazaDataId_Camera(motFile);

  void *envBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envFile);
  void *motBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,motFile);
  core->GetCameraSys()->StartCameraAnime(envBuf,motBuf);
  core->GetCameraSys()->SetCameraAnimeOffset(pos);
  core->GetCameraSys()->SetCameraAnimeFlip(isFlip);
}

BTL_SEQ_FUNC_DEF( CameraAnimetionTrainer )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int envFile,motFile;
  int  trgTrainer;
  char nodeName[32];
  gfl2::math::Vector3 posOfs;
  bool adjustFlip,adjustScale,adjustRotate,enableLight,forceUpdate;

  file->LoadDataInt( &envFile );
  file->LoadDataInt( &motFile );
  file->LoadDataInt( &trgTrainer );
  file->LoadDataString(nodeName,32);
  file->LoadDataVec3( &posOfs );
  file->LoadDataBool( &adjustFlip );
  file->LoadDataBool( &adjustScale );
  file->LoadDataBool( &adjustRotate );
  file->LoadDataBool( &forceUpdate );

  envFile = core->CheckWazaDataId_Env(envFile,trgTrainer);
  motFile = core->CheckWazaDataId_Camera(motFile,trgTrainer);

  void *envBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envFile);
  void *motBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,motFile);

  //追従
  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trgTrainer);
  if( chara )
  {
    if( forceUpdate )
    {
      chara->UpdatePositionForce();
      //@attencion TraverseModelFastFromChildだと、親のnode自体は更新されないのでSetPositionの座標が反映されない
      //専用関数用意予定→見送り
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();
    }
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* node = chara->GetJointNode(nodeName);
    if( node )
    {
      BtlvPos vPosSub = BTL_VPOS_ERROR;
      if( trgTrainer == SEQ_DEF_TRAINER_SEQ_TARGET1 )
      {
        vPosSub = core->SeqComFunc_GetTargetPoke_Org(1);
      }
      else
      if( trgTrainer == SEQ_DEF_TRAINER_SEQ_TARGET2 )
      {
        vPosSub = core->SeqComFunc_GetTargetPoke_Org(0);
      }

      const gfl2::math::Matrix34 mtx = node->GetWorldMatrix();
      gfl2::math::Vector3 pos = mtx.GetElemPosition() + posOfs;
      core->GetCameraSys()->StartCameraAnime(envBuf,motBuf);
      core->GetCameraSys()->SetCameraAnimeOffset( pos );
      if( adjustFlip &&
          core->SeqComFunc_IsFlipEffect(core->GetClientIdToBtlvPos(chara->GetClientID()),vPosSub) )
      {
        core->GetCameraSys()->SetCameraAnimeFlip(true);
      }
      if( adjustScale )
      {
        const f32 baseScale = chara->GetTrainerData()->mCameraScale;
        gfl2::math::Vector3 scale(baseScale,baseScale,baseScale);

        core->GetCameraSys()->SetCameraAnimeScale(scale);
      }
      if( adjustRotate )
      {
        core->GetCameraSys()->SetCameraAnimeRotateY_Rad( gfl2::math::ConvDegToRad(chara->GetRotateY()*-1) );
      }
    }
  }
}

BTL_SEQ_FUNC_DEF( CameraAnimetionDoubleIntro )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int envFile,motFile[3];
  BTLV_RELARIVE_POKE_OPTION opt;
  int trg[2];

  file->LoadDataInt( &envFile );
  file->LoadDataInt( &motFile[0] );
  file->LoadDataInt( &motFile[1] );
  file->LoadDataInt( &motFile[2] );

  file->LoadDataInt( &trg[0] );
  file->LoadDataInt( &trg[1] );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.adjustFlip );
  opt.adjustScale = false;
  file->LoadDataInt( &opt.autoRotatePos );
  opt.autoRotateModel = opt.autoRotatePos;
  file->LoadDataBool( &opt.forceUpdate );

  //アニメ有無をチェックしてどのモーションを使うか調べる
  BtlvPos vpos1 = core->SeqComFunc_GetTargetPoke(trg[0]);
  BtlvPos vpos2 = core->SeqComFunc_GetTargetPoke(trg[1]);
  BtlvObject_PokeModel *poke1 = core->GetPokeModel(vpos1);
  BtlvObject_PokeModel *poke2 = core->GetPokeModel(vpos2);

  int camIdx = 0;
  if( poke1->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) )
  {
    if( poke2->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) )
    {
      //高:高
      camIdx = 1;
    }
    else
    {
      //高:低
      camIdx = 2;
    }
  }
  else
  {
    if( poke2->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) )
    {
      //低:高
      camIdx = 0;
    }
    else
    {
      //低:低
      camIdx = 1;
    }
  }

  GFL_PRINT("DoubleIntroCamera vpos[%d:%d]camIdx[%d]\n",vpos1,vpos2,camIdx);

  //サイズ補正は大きいほう
  f32 camAdjust = gfl2::math::max(poke1->GetCamAdjustHeight(),poke2->GetCamAdjustHeight());

  opt.rate = 0;
  opt.vpos = vpos1;
  opt.vposSub = vpos1;

  envFile = core->CheckWazaDataId_Env(envFile);
  motFile[camIdx] = core->CheckWazaDataId_Camera(motFile[camIdx]);

  gfl2::math::Vector3 pos,rot,scale;

  core->SeqComFunc_GetPokeRelativePos(opt,&pos,&rot,&scale,true);

  void *envBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envFile);
  void *motBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,motFile[camIdx]);
  core->GetCameraSys()->StartCameraAnime(envBuf,motBuf);
  core->GetCameraSys()->SetCameraAnimeOffset(pos);

  if( opt.adjustFlip )
  {
    core->GetCameraSys()->SetCameraAnimeFlip(core->SeqComFunc_IsFlipEffect(opt.vpos,opt.vposSub));
  }
  core->GetCameraSys()->SetCameraAnimeRotateY_Rad( gfl2::math::ConvDegToRad(-rot.y));
  scale.Set(camAdjust,camAdjust,camAdjust);
  core->GetCameraSys()->SetCameraAnimeScale(scale);
  //全力のポーズの解除
  core->ResetZenryokuTrainerMotion();
}

BTL_SEQ_FUNC_DEF( CameraAnimetionEnd )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  bool keepPos;
  file->LoadDataBool( &keepPos );

  core->GetCameraSys()->EndCameraAnime(keepPos);
}

BTL_SEQ_FUNC_DEF( CameraAnimetionScale )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 scale;
  file->LoadDataVec3( &scale );

  core->GetCameraSys()->SetCameraAnimeScale(scale);
}

BTL_SEQ_FUNC_DEF( CameraAnimetionRotate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  f32 rotY;
  file->LoadDataF32( &rotY );

  core->GetCameraSys()->SetCameraAnimeRotateY_Rad(gfl2::math::ConvDegToRad(rotY));
}

//カメラアニメーション速度変更
BTL_SEQ_FUNC_DEF( CameraAnimetionChangeSpeed )
{
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  f32 speed;
  file->LoadDataF32( &speed );

  core->GetCameraSys()->SetCameraAnimeSpeed(speed);
}

//------------------------------------------------------------------------------------------------------------
//パーティクル関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelParticle )
{
  DUMP_COMMAND;
}

//パーティクル生成
BTL_SEQ_FUNC_DEF( ParticleCreate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdx;
  int isMulti;
  int drawType;
  int index;
  int rotOrder;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &index );
  file->LoadDataInt( &isMulti );
  file->LoadDataInt( &drawType );
  file->LoadDataInt( &rotOrder );

  //シーケンスでCOMBO_TEXTのデフォルトがうまく動いていない・・・
  if( rotOrder == SEQ_DEF_ROTATE_ORDER_DEF )
  {
    rotOrder = gfl2::math::EulerRotation::RotationOrderZYX;
  }
  else
  {
    rotOrder--;
  }

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  arcIdx = core->CheckWazaDataId_Effect(arcIdx,index);

  int objNum = 1;
  if( isMulti == true )
  {
    objNum = core->SeqComFunc_GetTargetPokeNum();
  }
  BtlvObject_Effect **objArr = GFL_NEW_ARRAY(heap)BtlvObject_Effect*[objNum];

  core->GetEffectObjManager()->ClearBuffer(core->GetObjectManager());
  core->GetEffectObjManager()->SetObjectArr(objArr,objNum,grpNo);
  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)BtlvObject_Effect(core);
    core->GetObjectManager()->AddObject(objArr[i]);

    int resIdx = core->GetResourceManager()->GetEffectResIdx(ARCID_WAZAEFF_RESOURCE,arcIdx);
    GFL_ASSERT_STOP( resIdx==-1 ? 0:1 );

    objArr[i]->CreateResIdx(core->GetEffectHeap(),resIdx);
    objArr[i]->AddRenderPath(drawType);
    objArr[i]->SetRotationOrder(static_cast<gfl2::math::EulerRotation::RotationOrder>(rotOrder));

    if( frame > 0 )
    {
      DemoLib::Obj::CreateTask_DeleteObject_Effect(core->GetTaskSystem(),heap,objArr[i],frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( ParticleStop )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    obj->StopParticle();
  }
}

//パーティクル削除
BTL_SEQ_FUNC_DEF( ParticleDelete )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    DemoLib::Obj::CreateTask_DeleteObject_Effect(core->GetTaskSystem(),heap,objArr[i],0);
  }
}

//パーティクル移動(座標)
BTL_SEQ_FUNC_DEF( ParticleMovePosition )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 tempPos;
  bool isRelative;

  file->LoadDataVec3( &tempPos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    gfl2::math::Vector3 pos = tempPos;
    BtlvObject_Effect *obj = objArr[i];

    if( isRelative == true )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 )
    {
      obj->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
    }
  }
}

//パーティクル移動(ポケモン相対)
BTL_SEQ_FUNC_DEF( ParticleMoveRelativePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  BTLV_RELARIVE_POKE_OPTION opt;
  int trg;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.rate );
  file->LoadDataInt( &opt.adjustFlip );
  file->LoadDataInt( &opt.adjustScale );
  file->LoadDataInt( &opt.autoRotateModel );
  file->LoadDataInt( &opt.autoRotatePos );
  file->LoadDataBool( &opt.forceUpdate );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    opt.vpos = core->SeqComFunc_GetTargetPoke(trg,i);
    opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trg);

    core->SeqComFunc_MoveRelativePoke(obj,frame,opt);
  }
}

//パーティクル移動(特殊座標)
BTL_SEQ_FUNC_DEF( ParticleMoveSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int trg;
  gfl2::math::Vector3 offset;
  bool isFlip,isRot;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &offset );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRot );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];

    core->SeqComFunc_MoveSpecialPos(obj,frame,trg,offset,isRot,isFlip);
  }
}

//パーティクル移動モデル相対(捕獲演出量
BTL_SEQ_FUNC_DEF( ParticleMoveRelativeModel )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgGrp;
  char nodeName[32];

  file->LoadDataInt( &trgGrp );
  file->LoadDataString(nodeName,32);

  int objNum;
  BtlvObject_Effect **effObjArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  BtlvObject_Gfmdl  **mdlObjArr = core->GetModelObjManager()->GetObjectArr(trgGrp,&objNum);
  
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *effObj = effObjArr[i];
    BtlvObject_Gfmdl  *mdlObj = mdlObjArr[i];

    gfl2::math::Vector3 pos;

    gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
    jointNode = mdlObj->GetJointNode(nodeName);
    if( jointNode )
    {
      const gfl2::math::Matrix34 mtx = jointNode->GetWorldMatrix();

      pos = mtx.GetElemPosition();
    }
    else
    {
      pos = mdlObj->GetPosition();
    }

    DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,effObj,effObj->GetPosition(),pos,frame);

  }

}

//パーティクルスケール
BTL_SEQ_FUNC_DEF( ParticleScale )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataVec3( &sclBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative == true )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 )
    {
      obj->SetScale(scl);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);
    }
  }
}

//パーティクル回転
BTL_SEQ_FUNC_DEF( ParticleRotate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}

//パーティクル回転(ポケモン指定
BTL_SEQ_FUNC_DEF( ParticleRotatePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int dirPoke;
  int dirNode;
  f32 ofs;
  bool isVertical;

  file->LoadDataInt( &dirPoke );
  file->LoadDataInt( &dirNode );
  file->LoadDataF32( &ofs );
  file->LoadDataBool( &isVertical );
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];

    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(dirPoke,i);

    gfl2::math::Vector3 rot = obj->GetRotate();
    core->SeqComFunc_CalcPokeDir(obj->GetPosition(),vPos,dirNode,&rot,isVertical);

    rot.y += ofs;
    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}


//パーティクル回転(Offset)
BTL_SEQ_FUNC_DEF( ParticleRotateOffset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotateOffset();
    }

    if( frame == 0 )
    {
      obj->SetRotateOffset(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotateOffset(core->GetTaskSystem(),heap,obj,obj->GetRotateOffset(),rot,frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( ParticleFollowModel )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    BtlvObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_ASSERT_MSG(0,"WazaEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      BtlvObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,mdl->GetPriority()+1);
    }
  }
}

BTL_SEQ_FUNC_DEF( ParticleFollowPoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int isEnable;
  int trgPoke;
  int trgNode;
  gfl2::math::Vector3 ofs;
  bool  followRot,followScale;

  file->LoadDataInt( &isEnable );
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( isEnable == false )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      obj->SetFollowPoke(NULL);
    }
  }
  else
  {
    BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trgPoke);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vpos);

    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      if( trgNode == SEQ_DEF_NODE_ORIGIN )
      {
        //原点なのでObj自体に追従させる
        obj->SetFollowObject(poke,"",true,false,false);
      }
      else
      {
        PokeTool::MODEL_NODE node;
        int nodeIdx = 0;
        poke->GetFollowNode(trgNode,&node,&nodeIdx);
        obj->SetFollowPoke(poke,node,nodeIdx,followRot,followScale);
      }
      obj->SetOffset(ofs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);
    }
  }
}

BTL_SEQ_FUNC_DEF( ParticleFollowTrainer )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int isEnable;
  int  trgTrainer;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &isEnable );
  file->LoadDataInt( &trgTrainer );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);

  if( isEnable == false )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trgTrainer);

    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Effect *obj = objArr[i];
      obj->SetFollowObject(chara,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,chara->GetPriority()+1);
    }
  }
}

//パーティクル移動方向自動回転
BTL_SEQ_FUNC_DEF( ParticleSetAutoRotate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  bool isEnable;
  file->LoadDataBool( &isEnable );

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    obj->SetAutoRotate(isEnable);
  }
}

//パーティクル カメラ固定移動
BTL_SEQ_FUNC_DEF( ParticlCameraAutoRotate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  f32 length;

  file->LoadDataF32( &length );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    CreateTask_ObjectCameraAutoRotate(core->GetTaskSystem(),heap,core,obj,length,frame);
  }
}

BTL_SEQ_FUNC_DEF( ParticleSpMoveReset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  gfl2::math::Vector3 pos(0,0,0);
  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    if( frame == 0 )
    {
      obj->SetOffset(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMoveOffset(core->GetTaskSystem(),heap,obj,obj->GetOffset(),pos,frame);
    }
  }
}
BTL_SEQ_FUNC_DEF( ParticleSpMoveShake )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  f32 sRate,eRate;
  f32 sDec,eDec;
  int tempAxis;
  
  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &sDec );
  file->LoadDataF32( &eDec );
  file->LoadDataInt( &tempAxis );

  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  case SEQ_DEF_AXIS_X_ROT:
    axis = DemoLib::Obj::AXIS_X_ROT;
    break;
  case SEQ_DEF_AXIS_Z_ROT:
    axis = DemoLib::Obj::AXIS_Z_ROT;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
  }

  int objNum;
  BtlvObject_Effect **objArr = core->GetEffectObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Effect *obj = objArr[i];
    DemoLib::Obj::CreateTask_ObjMoveShake(core->GetTaskSystem(),heap,obj,sRate,eRate,sDec*360,eDec*360,frame,axis );
  }
}

//------------------------------------------------------------------------------------------------------------
//モデル関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelModel )
{
  DUMP_COMMAND;
}

//モデル生成
BTL_SEQ_FUNC_DEF( ModelCreate )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdx;
  int isMulti;
  int drawType;
  int rotOrder;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &isMulti );
  file->LoadDataInt( &drawType );
  file->LoadDataInt( &rotOrder );

  //シーケンスでCOMBO_TEXTのデフォルトがうまく動いていない・・・
  if( rotOrder == SEQ_DEF_ROTATE_ORDER_DEF )
  {
    rotOrder = gfl2::math::EulerRotation::RotationOrderZYX;
  }
  else
  {
    rotOrder--;
  }

  arcIdx = core->CheckWazaDataId_Model(arcIdx,0);

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum = 1;
  if( isMulti == true )
  {
    objNum = core->SeqComFunc_GetTargetPokeNum();
  }
  BtlvObject_Gfmdl **objArr = GFL_NEW_ARRAY(heap)BtlvObject_Gfmdl*[objNum];

  core->GetModelObjManager()->ClearBuffer(core->GetObjectManager());
  core->GetModelObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)BtlvObject_Gfmdl(core,core->GetRenderSys()->GetRenderingPipeLine());
    core->GetObjectManager()->AddObject(objArr[i]);

    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,arcIdx);

    objArr[i]->CreateBuffer(buf,heap,core->GetTempGLAllocator());
    objArr[i]->AddRenderPath(drawType);
    objArr[i]->SetRotationOrder(static_cast<gfl2::math::EulerRotation::RotationOrder>(rotOrder));

    if( frame > 0 )
    {
      DemoLib::Obj::CreateTask_DeleteObject_GfMdl(core->GetTaskSystem(),heap,objArr[i],frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( ModelCreateBall )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int ballIdx;
  int drawType;

  file->LoadDataInt( &ballIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &drawType );

  //0はBALLID_NULLなので注意
  int ballType = core->SeqComFunc_GetTargetBall(ballIdx);

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum = 1;
  BtlvObject_Gfmdl **objArr = GFL_NEW_ARRAY(heap)BtlvObject_Gfmdl*[1];

  core->GetModelObjManager()->ClearBuffer(core->GetObjectManager());
  core->GetModelObjManager()->SetObjectArr(objArr,1,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)BtlvObject_Gfmdl(core,core->GetRenderSys()->GetRenderingPipeLine());
    core->GetObjectManager()->AddObject(objArr[i]);

    void *buf = core->GetResourceManager()->GetDataBuf_BinPack(ARCID_CHARA_MODEL_BATTLE_NORMAL,CHARA_MODEL_ID_BTOB0201_00+ballType-1)->GetData(0);

    objArr[i]->CreateBuffer(buf,heap,core->GetTempGLAllocator());
    objArr[i]->AddRenderPath(drawType);
    int matNum = objArr[i]->GetBaseModel()->GetMaterialCount();
    for( int matNo=0;matNo<matNum;matNo++ )
    {
      objArr[i]->GetBaseModel()->SetMaterialLightSetIndex(matNo,0);
    }

    if( frame > 0 )
    {
      DemoLib::Obj::CreateTask_DeleteObject_GfMdl(core->GetTaskSystem(),heap,objArr[i],frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( ModelDelete )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    DemoLib::Obj::CreateTask_DeleteObject_GfMdl(core->GetTaskSystem(),heap,obj,0);
  }
}

//モデル移動(座標)
BTL_SEQ_FUNC_DEF( ModelMovePosition )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 tempPos;
  bool isRelative;

  file->LoadDataVec3( &tempPos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    gfl2::math::Vector3 pos = tempPos;
    BtlvObject_Gfmdl *obj = objArr[i];
    if( isRelative == true )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 )
    {
      obj->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
    }
  }
}

//モデル移動(ポケモン相対)
BTL_SEQ_FUNC_DEF( ModelMoveRelativePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  BTLV_RELARIVE_POKE_OPTION opt;
  int trg;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.rate );
  file->LoadDataInt( &opt.adjustFlip );
  file->LoadDataInt( &opt.adjustScale );
  file->LoadDataInt( &opt.autoRotateModel );
  file->LoadDataInt( &opt.autoRotatePos );
  file->LoadDataBool( &opt.forceUpdate );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    opt.vpos = core->SeqComFunc_GetTargetPoke(trg,i);
    opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trg);
    
    core->SeqComFunc_MoveRelativePoke(obj,frame,opt);

  }
}

//モデル移動(特殊座標)
BTL_SEQ_FUNC_DEF( ModelMoveSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int trg;
  gfl2::math::Vector3 offset;
  bool isFlip,isRot;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &offset );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRot );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    core->SeqComFunc_MoveSpecialPos(obj,frame,trg,offset,isRot,isFlip);
  }
}

//モデル拡縮
BTL_SEQ_FUNC_DEF( ModelScale )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataVec3( &sclBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative == true )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 )
    {
      obj->SetScale(scl);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);
    }
  }
}

//モデル回転
BTL_SEQ_FUNC_DEF( ModelRotate )
{  
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}

//モデル回転(ポケモン指定
BTL_SEQ_FUNC_DEF( ModelRotatePoke )
{  
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  int dirPoke;
  int dirNode;
  f32 ofs;
  bool isVertical;

  file->LoadDataInt( &dirPoke );
  file->LoadDataInt( &dirNode );
  file->LoadDataF32( &ofs );
  file->LoadDataBool( &isVertical );
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(dirPoke,i);
    gfl2::math::Vector3 rot = obj->GetRotate();
    core->SeqComFunc_CalcPokeDir(obj->GetPosition(),vPos,dirNode,&rot,isVertical);

    rot.y += ofs;

    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}

//モデル回転(オフセット
BTL_SEQ_FUNC_DEF( ModelRotateOffset )
{  
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotateOffset();
    }

    if( frame == 0 )
    {
      obj->SetRotateOffset(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotateOffset(core->GetTaskSystem(),heap,obj,obj->GetRotateOffset(),rot,frame);
    }
  }
}

//モデル表示切替
BTL_SEQ_FUNC_DEF( ModelVisible )
{  
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  bool isVisible;

  file->LoadDataBool( &isVisible );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    obj->SetVisible(isVisible);
  }
}

//モデル、モデル追従
BTL_SEQ_FUNC_DEF( ModelFollowModel )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
    
  int  targetModel;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;

  file->LoadDataInt( &targetModel );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( targetModel == -1 )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    int mdlNum = 0;
    BtlvObject_Gfmdl **mdlArr = core->GetModelObjManager()->GetObjectArr(targetModel,&mdlNum);
    if( objNum != mdlNum )
    {
      GFL_ASSERT_MSG(0,"WazaEffectCommand:追従対象の個数が一致しません。追従元[%d] 追従先[]\n",objNum,mdlNum);
      return;
    }

    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Gfmdl *obj = objArr[i];
      BtlvObject_Gfmdl *mdl = mdlArr[i];
      obj->SetFollowObject(mdl,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,mdl->GetPriority()+1);

    }
  }
}

//モデル ポケモン追従
BTL_SEQ_FUNC_DEF( ModelFollowPoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int isEnable;
  int trgPoke;
  int trgNode;
  gfl2::math::Vector3 ofs;
  bool  followRot,followScale;

  file->LoadDataInt( &isEnable );
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( isEnable == false )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Gfmdl *obj = objArr[i];
      obj->SetFollowPoke(NULL);
    }
  }
  else
  {
    for( int i=0;i<objNum;i++)
    {
      BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
      BtlvObject_PokeModel *poke = core->GetPokeModel(vpos);

      BtlvObject_Gfmdl *obj = objArr[i];
      if( trgNode == SEQ_DEF_NODE_ORIGIN )
      {
        //原点なのでObj自体に追従させる
        obj->SetFollowObject(poke,"",true,false,false);
      }
      else
      {
        PokeTool::MODEL_NODE node;
        int nodeIdx = 0;
        poke->GetFollowNode(trgNode,&node,&nodeIdx);
        obj->SetFollowPoke(poke,node,nodeIdx,followRot,followScale);
      }
      obj->SetOffset(ofs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);
    }
  }
}

BTL_SEQ_FUNC_DEF( ModelFollowTrainer )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int isEnable;
  int  trgTrainer;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;
  bool lightChange;
  bool checkVisible;

  file->LoadDataInt( &isEnable );
  file->LoadDataInt( &trgTrainer );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &followPos );
  file->LoadDataBool( &followRot );
  file->LoadDataBool( &followScale );
  file->LoadDataVec3( &posOfs );
  file->LoadDataVec3( &rotOfs );
  file->LoadDataBool( &lightChange );
  file->LoadDataBool( &checkVisible );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  if( isEnable == false )
  {
    //解除
    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(NULL);
    }
  }
  else
  {
    //追従
    BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trgTrainer);

    for( int i=0;i<objNum;i++)
    {
      BtlvObject_Gfmdl *obj = objArr[i];
      obj->SetFollowObject(chara,nodeName,followPos,followRot,followScale);

      obj->SetOffset(posOfs);
      obj->SetRotateOffset(rotOfs);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(obj);
      core->GetObjectManager()->AddObject(obj,chara->GetPriority()+1);

      //ライト番号の変更
      if( lightChange )
      {
        int charaIdx = chara->GetIndex();
        int matNum = obj->GetBaseModel()->GetMaterialCount();
        for( int ii=0;ii<matNum;ii++ )
        {
          obj->GetBaseModel()->SetMaterialLightSetIndex(ii,LIGHTNO_CHARA_TOP+charaIdx);
        }
      }
      //ノードが無かったら非表示にする
      if( checkVisible )
      {
        gfl2::renderingengine::scenegraph::instance::JointInstanceNode* jointNode;
        jointNode = chara->GetJointNode(nodeName);
        if( jointNode == NULL )
        {
          obj->SetVisible(false);
        }
      }
    }
  }
}

BTL_SEQ_FUNC_DEF( ModelAttachTrainer )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int isEnable;
  int  trgTrainer;
  char nodeName[32];
  bool  followPos,followRot,followScale;
  gfl2::math::Vector3 posOfs;
  gfl2::math::Vector3 rotOfs;
  bool lightChange;

  file->LoadDataInt( &isEnable );
  file->LoadDataInt( &trgTrainer );
  file->LoadDataString(nodeName,32);
  file->LoadDataBool( &lightChange );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  //追従
  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trgTrainer);

  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    gfl2::renderingengine::scenegraph::instance::JointInstanceNode *joint = chara->GetJointNode(nodeName);
    if( joint )
    {
      if( isEnable )
      {
        joint->AddChild(obj->GetBaseModel()->GetModelInstanceNode() );
      }
      else
      {
        joint->RemoveChild(obj->GetBaseModel()->GetModelInstanceNode() );
      }
      //ライト番号の変更
      if( lightChange )
      {
        int charaIdx = chara->GetIndex();
        int matNum = obj->GetBaseModel()->GetMaterialCount();
        for( int ii=0;ii<matNum;ii++ )
        {
          obj->GetBaseModel()->SetMaterialLightSetIndex(ii,LIGHTNO_CHARA_TOP+charaIdx);
        }
      }
    }
  }
}

//モデル移動方向自動回転
BTL_SEQ_FUNC_DEF( ModelSetAutoRotate )
{
  DUMP_COMMAND;

  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  bool isEnable;
  file->LoadDataBool( &isEnable );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    obj->SetAutoRotate(isEnable);
  }
}


//モデル カメラ固定移動
BTL_SEQ_FUNC_DEF( ModelAutoCameraRotate )
{  
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  f32 length;

  file->LoadDataF32( &length );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    CreateTask_ObjectCameraAutoRotate(core->GetTaskSystem(),heap,core,obj,length,frame);
  }
}

//モデルコンスタンとカラー操作
BTL_SEQ_FUNC_DEF( ModelSetConstantColor )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  char matName[33];
  int constantNo;
  gfl2::math::Vector3 colTemp;
  f32 alpha;
  
  file->LoadDataString( matName , 32 );
  file->LoadDataInt( &constantNo );
  file->LoadDataVec3( &colTemp );
  file->LoadDataF32( &alpha );
  
  gfl2::math::Vector4 col( colTemp.x,colTemp.y,colTemp.z,alpha);

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);

  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];

    if( frame == 0 )
    {
      obj->SetConstantColor(matName,constantNo,col);
    }
    else
    {
      gfl2::math::Vector4 startCol = obj->GetConstantColor(matName,constantNo);
      DemoLib::Obj::CreateTask_ObjectGfMdl_ChangeConstantColor(core->GetTaskSystem(),heap,obj,matName,constantNo,startCol,col,frame);
    }
  }
}

//モデルライト方向
BTL_SEQ_FUNC_DEF( ModelLightDir )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  gfl2::math::Vector3 dir;
  bool isFlip;
  int flipTarget;
  file->LoadDataVec3( &dir );
  file->LoadDataBool( &isFlip );
  file->LoadDataInt( &flipTarget );

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(flipTarget);
  BtlvPos vPosSub = core->SeqComFunc_GetTargetPokeSub(flipTarget);
  if( isFlip &&
    core->SeqComFunc_IsFlipEffect(vPos,vPosSub) )
  {
    dir.x *= -1;
  }

  core->GetRenderSys()->GetLightNode()->SetDirection(dir);
  
}

//モデルアニメーション設定
BTL_SEQ_FUNC_DEF( ModelSetAnimation )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdx;
  bool isLoop;
  int slot;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataBool( &isLoop );
  file->LoadDataInt( &slot );

  void *buf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,arcIdx);

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    obj->SetAnimation(core->GetTempGLAllocator(),buf,slot);
    obj->SetAnimationIsLoop(isLoop,slot);
  }
}

//モデルアニメーション停止
BTL_SEQ_FUNC_DEF( ModelAnimationStop )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int slot;

  file->LoadDataInt( &slot );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    obj->RemoveAnimation(slot);
  }
}

//モデルアニメーション速度変更
BTL_SEQ_FUNC_DEF( ModelSetAnimationSpeed )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  f32 speed;
  int slot;

  file->LoadDataF32( &speed );
  file->LoadDataInt( &slot );

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    obj->SetAnimationSpeed(speed,slot);
  }
}

//モデルテクスチャ差し替え
BTL_SEQ_FUNC_DEF( ModelReplaceTexture )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int arcIdx;
  char matName[32];
  int texNo;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataString(matName,32);
  file->LoadDataInt( &texNo );

  arcIdx = core->CheckWazaDataId_Texture(arcIdx,0);

  gfl2::renderingengine::scenegraph::resource::TextureResourceNode *texRes = core->GetResourceManager()->GetDataBuf_Texture(ARCID_WAZAEFF_RESOURCE,arcIdx);

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    s32 matNo = obj->GetBaseModel()->GetMaterialIndex(matName);
    if( matNo != -1 )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = obj->GetModelNode()->GetMaterialInstanceNode(matNo);
      matNode->SetTextureResourceNode(texNo,texRes);
    }
    else
    {
      GFL_ASSERT_MSG(0,"マテリアル名[%s]が見つかりませんでした。\n",matName); //@check スルー可
    }
  }
}

//モデルテクスチャ差し替え(トレーナー)
BTL_SEQ_FUNC_DEF( ModelReplaceTextureTrainer )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int trg;    //先読み処理の関係上先頭で！
  int type;
  char matName[32];
  int texNo;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &type );
  file->LoadDataString(matName,32);
  file->LoadDataInt( &texNo );

  u32 arcIdx = core->CheckWazaDataId_TrainerTexture(trg,type);

  gfl2::renderingengine::scenegraph::resource::TextureResourceNode *texRes = core->GetResourceManager()->GetDataBuf_Texture(ARCID_WAZAEFF_RESOURCE,arcIdx);

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    s32 matNo = obj->GetBaseModel()->GetMaterialIndex(matName);
    if( matNo != -1 )
    {
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode *matNode = obj->GetModelNode()->GetMaterialInstanceNode(matNo);
      matNode->SetTextureResourceNode(texNo,texRes);
    }
    else
    {
      GFL_ASSERT_MSG(0,"マテリアル名[%s]が見つかりませんでした。\n",matName); //@check スルー可
    }
  }
}
BTL_SEQ_FUNC_DEF( ModelSpMoveReset )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  gfl2::math::Vector3 pos(0,0,0);
  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    if( frame == 0 )
    {
      obj->SetOffset(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMoveOffset(core->GetTaskSystem(),heap,obj,obj->GetOffset(),pos,frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( ModelSpMoveShake )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  f32 sRate,eRate;
  f32 sDec,eDec;
  int tempAxis;
  
  file->LoadDataF32( &sRate );
  file->LoadDataF32( &eRate );
  file->LoadDataF32( &sDec );
  file->LoadDataF32( &eDec );
  file->LoadDataInt( &tempAxis );

  DemoLib::Obj::TASK_AXIS_TYPE axis;
  switch( tempAxis )
  {
  case SEQ_DEF_AXIS_X:
    axis = DemoLib::Obj::AXIS_X;
    break;
  case SEQ_DEF_AXIS_Y:
    axis = DemoLib::Obj::AXIS_Y;
    break;
  case SEQ_DEF_AXIS_Z:
    axis = DemoLib::Obj::AXIS_Z;
    break;
  case SEQ_DEF_AXIS_X_ROT:
    axis = DemoLib::Obj::AXIS_X_ROT;
    break;
  case SEQ_DEF_AXIS_Z_ROT:
    axis = DemoLib::Obj::AXIS_Z_ROT;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",tempAxis);
    axis = DemoLib::Obj::AXIS_X;
  }

  int objNum;
  BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Gfmdl *obj = objArr[i];
    DemoLib::Obj::CreateTask_ObjMoveShake(core->GetTaskSystem(),heap,obj,sRate,eRate,sDec*360,eDec*360,frame,axis );
  }
}

//------------------------------------------------------------------------------------------------------------
//サウンド関係
//------------------------------------------------------------------------------------------------------------

BTL_SEQ_FUNC_DEF( LabelSound )
{
  DUMP_COMMAND;
}

//効果音再生
BTL_SEQ_FUNC_DEF( SoundPlaySe )
{
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int soundId;
  int player,tempPan,tempPitch,tempVol,panType,tempTempo;
  file->LoadDataInt( &soundId );


  file->LoadDataInt( &player );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPan );
  file->LoadDataInt( &panType );
  file->LoadDataInt( &tempPitch );
  file->LoadDataInt( &tempTempo );

 
  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 pan = ADJUST_SE_PAN(tempPan); //-1～1
  const f32 vol = ADJUST_SE_VOL(tempVol);
  const f32 tempo = ADJUST_SE_TEMPO(tempTempo);

  core->GetSoundObj(grpNo)->SetSoundId(soundId);
  core->GetSoundObj(grpNo)->SetIs3DSound(false);

  f32 ofsPan = core->SeqComFunc_GetPanOffset( panType );
  //ボールの鳴らし分け対応
  soundId = core->CheckWazaDataId_Se(soundId,0);
  
  if( core->IsRecordStartSkip() == false )
  {
    Sound::PlaySE(soundId,0,grpNo,(player==-1 ? Sound::DEFAULT_PLAYER : player));
    Sound::ChangeSEVolume( soundId , vol , 0 , grpNo );
    Sound::ChangeSEPitch( soundId , pitch ,  grpNo );
    Sound::ChangeSEPan( soundId , pan+ofsPan ,  grpNo );
    Sound::ChangeSETempo( soundId , tempo ,  grpNo );
  }
}
//効果音ボリューム操作
BTL_SEQ_FUNC_DEF( SoundCtrlVolume )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );
    
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 )
    {
      const f32 vol = ADJUST_SE_VOL(eTemp);
      Sound::ChangeSEVolume( core->GetSoundObj(grpNo)->GetSoundId() , vol , 0 , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_VOL(sTemp);
      const f32 end   = ADJUST_SE_VOL(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_VOLUME );
    }
  }
}

//効果音パン操作
BTL_SEQ_FUNC_DEF( SoundCtrlPan )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );
    
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 )
    {
      const f32 vol = ADJUST_SE_PAN(eTemp);
      Sound::ChangeSEPan( core->GetSoundObj(grpNo)->GetSoundId() , vol , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_PAN(sTemp);
      const f32 end   = ADJUST_SE_PAN(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_PAN );
    }
  }
}

//効果音ピッチ操作
BTL_SEQ_FUNC_DEF( SoundCtrlPitch )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );
    
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 )
    {
      const f32 vol = ADJUST_SE_PITCH(eTemp);
      Sound::ChangeSEPitch( core->GetSoundObj(grpNo)->GetSoundId() , vol , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_PITCH(sTemp);
      const f32 end   = ADJUST_SE_PITCH(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_PITCH );
    }
  }
}
//効果音テンポ操作
BTL_SEQ_FUNC_DEF( SoundCtrlTempo )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int sTemp,eTemp;
  file->LoadDataInt( &sTemp );
  file->LoadDataInt( &eTemp );
    
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0 )
    {
      const f32 vol = ADJUST_SE_TEMPO(eTemp);
      Sound::ChangeSETempo( core->GetSoundObj(grpNo)->GetSoundId() , vol , grpNo );
    }
    else
    {
      const f32 start = ADJUST_SE_TEMPO(sTemp);
      const f32 end   = ADJUST_SE_TEMPO(eTemp);
      DemoLib::Obj::CreateTask_SeCtrl( core->GetTaskSystem() , heap , start , end , frame , core->GetSoundObj(grpNo)->GetSoundId(),grpNo,DemoLib::Obj::SE_CTRL_TEMPO );
    }
  }
}

//鳴き声再生
BTL_SEQ_FUNC_DEF( SoundPlayVoice )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg;
  file->LoadDataInt( &trg );

  BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trg);
  core->PlayVoice( vpos );
}

//鳴き声再生
BTL_SEQ_FUNC_DEF( SoundPlayVoiceId )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int id;
  int trg;
  file->LoadDataInt( &trg );
  file->LoadDataInt( &id );

  BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trg);
  core->PlayVoiceId( vpos,id );
}

//3Dサウンド設定
BTL_SEQ_FUNC_DEF( Sound3DSetParam )
{
  DUMP_COMMAND;

  int priReducition;
  f32 panRange,sonic,interiaSize,maxVolDist,unitDist;
  
  file->LoadDataInt( &priReducition );
  file->LoadDataF32( &panRange );
  file->LoadDataF32( &sonic );
  file->LoadDataF32( &interiaSize );
  file->LoadDataF32( &maxVolDist );
  file->LoadDataF32( &unitDist );
  
  const gfl2::sound::SoundSystem::Sound3DParam sysParam =
  {
    priReducition,
    panRange,
    sonic
  };
  
  Sound::Set3DParam( sysParam );
  
  const gfl2::sound::SoundSystem::ListenerParam lisParam =
  {
    interiaSize,
    maxVolDist,
    unitDist,
  };
  Sound::SetListenerParam( lisParam );

}
//3Dサウンド再生
BTL_SEQ_FUNC_DEF( Sound3DPlay )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  const u32 grpNo = param.groupNo;
  
  int seNo,player,tempPan,tempPitch,tempVol,isDebug;

  file->LoadDataInt( &seNo );
  file->LoadDataInt( &player );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPan );
  file->LoadDataInt( &tempPitch );
  file->LoadDataInt( &isDebug );

  GFL_ASSERT_MSG(grpNo<BTLV_EFF_SOUND_NUM,"3Dサウンドの管理数を超えています。[%d]\n",grpNo); //@check エディタ上の数値のチェック、確認時に必ず発覚する

  //ボールの鳴らし訳対応
  seNo = core->CheckWazaDataId_Se(seNo,0);
  core->GetSoundObj(grpNo)->SetSoundId(seNo);
  core->GetSoundObj(grpNo)->SetIs3DSound(true);

  nw::snd::SoundStartable::StartInfo seInfo;
  if( player != -1 )
  {
    seInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
    seInfo.playerId = player;
  }
  
  if( core->IsRecordStartSkip() == false )
  {
    const f32 pitch = ADJUST_SE_PITCH(tempPitch);
    const f32 pan = ADJUST_SE_PAN(tempPan);
    const f32 vol = ADJUST_SE_VOL(tempVol);
    Sound::Play3DActor( grpNo , seNo , &seInfo );
    Sound::Set3DActorVolume( grpNo , vol );
    Sound::Set3DActorPitch( grpNo , pitch );
    Sound::Set3DActorPan( grpNo , pan );
  }
}

//3Dサウンド停止
BTL_SEQ_FUNC_DEF( Sound3DStop )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int fade;
  file->LoadDataInt( &fade );

  if( core->IsRecordStartSkip() == false )
  {
    Sound::Stop3DActor( grpNo , fade );
  }
}

//3Dサウンド移動(絶対座標)
BTL_SEQ_FUNC_DEF( Sound3DMovePosition )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  gfl2::math::Vector3 pos;
  bool isRelative;

  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvObject_Sound *obj = core->GetSoundObj(grpNo);

  if( isRelative == true )
  {
    pos += obj->GetPosition();
  }

  if( frame == 0 )
  {
    obj->SetPosition(pos);
  }
  else
  {
    DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
  }
}

BTL_SEQ_FUNC_DEF( Sound3DMoveRelativePoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  BTLV_RELARIVE_POKE_OPTION opt;
  int trg;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.rate );
  opt.adjustFlip = false;
  opt.adjustScale = false;
  opt.autoRotateModel = false;
  opt.autoRotatePos = false;
  opt.forceUpdate = false;
  opt.vpos = core->SeqComFunc_GetTargetPoke(trg,0);
  opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trg);

  BtlvObject_Sound *obj = core->GetSoundObj(grpNo);
  core->SeqComFunc_MoveRelativePoke(obj,frame,opt);
}

//3Dサウンド移動(特殊座標)
BTL_SEQ_FUNC_DEF( Sound3DMoveSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int trg;
  gfl2::math::Vector3 offset;
  bool isFlip,isRot;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &offset );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRot );

  BtlvObject_Sound *obj = core->GetSoundObj(grpNo);
  core->SeqComFunc_MoveSpecialPos(obj,frame,trg,offset,isRot,isFlip);

}

//3Dサウンドポケモン追従
BTL_SEQ_FUNC_DEF( Sound3DFollowPoke )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;
  
  int trgPoke;
  int trgNode;
  gfl2::math::Vector3 ofs;

  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &trgNode );
  file->LoadDataVec3( &ofs );

  BtlvObject_Sound *obj = core->GetSoundObj(grpNo);

  BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trgPoke);
  BtlvObject_PokeModel *poke = core->GetPokeModel(vpos);
  PokeTool::MODEL_NODE node;
  int nodeIdx = 0;

  poke->GetFollowNode(trgNode,&node,&nodeIdx);

  obj->SetFollowPoke(poke,node,nodeIdx);
  obj->SetOffset(ofs);

  //優先順位の付け替え(親よりも低いもの)
  core->GetObjectManager()->RemoveObject(obj);
  core->GetObjectManager()->AddObject(obj,poke->GetPriority()+1);
}

//3Dサウンドオブジェクト追従
BTL_SEQ_FUNC_DEF( Sound3DFollowObj )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;
  
  int trgType,trgIdx;
  gfl2::math::Vector3 ofs;

  file->LoadDataInt( &trgType );
  file->LoadDataInt( &trgIdx );
  file->LoadDataVec3( &ofs );

  BtlvObject_Sound *sndObj = core->GetSoundObj(grpNo);

  DemoLib::Obj::Object *obj = NULL;
  if( trgType == SEQ_DEF_FOLLOW_PARTICLE )
  {
    if( core->GetEffectObjManager()->GetObjectArr(trgIdx,NULL) )
    {
      int mdlNum = 0;
      obj = core->GetEffectObjManager()->GetObjectArr(trgIdx,&mdlNum)[0];
    }
  }
  else
  if( trgType == SEQ_DEF_FOLLOW_MODEL )
  {
    if( core->GetModelObjManager()->GetObjectArr(trgIdx,NULL) )
    {
      int mdlNum = 0;
      obj = core->GetModelObjManager()->GetObjectArr(trgIdx,&mdlNum)[0];
    }
  }
  
  if( obj )
  {
    sndObj->SetFollowObject( obj ,NULL,true);
    sndObj->SetOffset( ofs );
    sndObj->SetTargetGrp(trgIdx,trgType);

    //優先順位の付け替え(親よりも低いもの)
    core->GetObjectManager()->RemoveObject(sndObj);
    core->GetObjectManager()->AddObject(sndObj,obj->GetPriority()+1);
  }

}

//3D鳴き声再生
BTL_SEQ_FUNC_DEF( Sound3DVoicePlay )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg;

  int tempPitch,tempVol;
  bool isDead;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPitch );
  file->LoadDataBool( &isDead );
  
  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trg);
  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 vol = ADJUST_SE_VOL(tempVol);
  
  if( isDead )
  {
    core->PlayDeadVoice3D( vPos );
    //ダウン鳴き声はプログラムでピッチをつけている
  }
  else
  {
    core->PlayVoice3D( vPos );
    Sound::ChangeVoice3DActorVolume( vPos/2 , vol );
    Sound::ChangeVoice3DActorPitch( vPos/2 , pitch );
  }
}

//3D鳴き声再生(ID指定
BTL_SEQ_FUNC_DEF( Sound3DVoicePlayId )
{
  DUMP_COMMAND

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg,Id;

  int tempPitch,tempVol;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &Id );
  file->LoadDataInt( &tempVol );
  file->LoadDataInt( &tempPitch );

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trg);
  const f32 pitch = ADJUST_SE_PITCH(tempPitch);
  const f32 vol = ADJUST_SE_VOL(tempVol);

  core->PlayVoiceId3D( vPos , Id );

  Sound::ChangeVoice3DActorVolume( vPos/2 , vol );
  Sound::ChangeVoice3DActorPitch( vPos/2 , pitch );

}

BTL_SEQ_FUNC_DEF(SoundBGMVolume)
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;

  int vol;
  file->LoadDataInt(&vol);

  f32 fVol = vol * 0.01f;

  Sound::ChangeBGMVolume(fVol, frame);
}

BTL_SEQ_FUNC_DEF(SoundZukanSE)
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg,idx;
  file->LoadDataInt(&trg);
  file->LoadDataInt(&idx);

  //@check 開発中に気づく
  GFL_ASSERT_MSG(idx < 4, "Se index is invalid!![%d]\n", idx);

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trg);
  BTL_POKEPARAM *bpp = core->GetBattlePokeParam(vPos);

  PokeTool::PlayRotomSound(static_cast<MonsNo>(bpp->GetMonsNo()), bpp->GetFormNo(), bpp->GetViewSrcData()->GetSex(), idx);
}


//------------------------------------------------------------------------------------------------------------
//表示制御
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelDisp )
{
  DUMP_COMMAND;
}

BTL_SEQ_FUNC_DEF( GaugeDisp )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;
  bool isDisp,isUpdate;

  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &isDisp );
  file->LoadDataBool( &isUpdate );

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    if( isDisp == true )
    {
      core->GetUiSys()->DispGauge( vPos , isUpdate );
    }
    else
    {
      core->GetUiSys()->HideGauge( vPos );
    }
  }
}

BTL_SEQ_FUNC_DEF( GaugeDispAll )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  bool isDisp;
  file->LoadDataBool( &isDisp );

  if( isDisp == true )
  {
    core->GetUiSys()->DispGaugeAll();
  }
  else
  {
    core->GetUiSys()->HideGaugeAll();
  }

}

BTL_SEQ_FUNC_DEF( GaugeDamage )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trgPoke;

  file->LoadDataInt( &trgPoke );

  if( frame == 0 )
  {
    frame = BtlvGaugeBase::GAUGE_ANIME_DEF_FRAME;
  }

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    core->GetUiSys()->StartGaugeDamage( vPos , frame );
    if( core->GetWazaParam()->mDmageAbout == pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE )
    {
      core->GetUiSys()->PlayBatugunAnime(vPos);
    }
  }

  if( core->IsRecordStartSkip() == false )
  {
    //ダメージ音
    switch( core->GetWazaParam()->mDmageAbout )
    {
    case pml::battle::TypeAffinity::TYPEAFF_ABOUT_NORMAL:
      Sound::PlaySE( SEQ_SE_BT_KOUKA_M , 0,Sound::NOT_CONTROL );
      break;
    case pml::battle::TypeAffinity::TYPEAFF_ABOUT_ADVANTAGE:
      //core->GetTaskSys()->CreateTaskDelaySe( 10 , SEQ_SE_BT_KOUKA_H , PLAYER_SE_3 );
      Sound::PlaySE( SEQ_SE_BT_KOUKA_H , 0,Sound::NOT_CONTROL );
      break;
    case pml::battle::TypeAffinity::TYPEAFF_ABOUT_DISADVANTAGE:
      //core->GetTaskSys()->CreateTaskDelaySe( 10 , SEQ_SE_BT_KOUKA_L , PLAYER_SE_3 );
      Sound::PlaySE( SEQ_SE_BT_KOUKA_L , 0,Sound::NOT_CONTROL );
      break;
    }
  }

}

//ボールバー表示
BTL_SEQ_FUNC_DEF( BallBarDisp )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;

  file->LoadDataInt( &trgPoke );

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke);
  core->GetUiSys()->StrtBallBarAnime(vPos);
}

BTL_SEQ_FUNC_DEF( HitBack )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke,isDefault;
  
  file->LoadDataInt( &trgPoke );
  file->LoadDataInt( &isDefault );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int trgNum = 1;
  if( trgPoke == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }
  
  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trgPoke,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);
    
    //ヒットバック！
    if( poke && poke->IsEnable() )
    {
      if( isDefault )
      {
        CreateTask_PokeHitBack( core->GetTaskSystem(),heap,core,poke );
      }
      else
      {
        static const int HIT_BACK_FRAME =6;
        gfl2::math::Vector3 ofs( 0,0,-60);
        gfl2::math::Vector3 start( 0,0,0);
        f32 rot = -poke->GetRotateY();
        BtlvRot_Deg( ofs.x,ofs.z,rot );
        ofs = ofs.MulPerElem(poke->GetAdjustScale());
        
        DemoLib::Obj::CreateTask_ObjectMoveOffset(core->GetTaskSystem(),heap,poke,start,ofs,HIT_BACK_FRAME);
        poke->SetHitBackFlg(true);
      }
    }
  }
}


//------------------------------------------------------------------------------------------------------------
//特殊処理
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelEffectSp )
{
  DUMP_COMMAND;
}

//背景カラーフェード(ON/OFF)
BTL_SEQ_FUNC_DEF( EffSpBackColFlg )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  bool isEnable;
  gfl2::math::Vector3 colTemp;
  f32 alpha;

  file->LoadDataBool( &isEnable );
  file->LoadDataVec3( &colTemp );
  file->LoadDataF32( &alpha );

  gfl2::math::Vector4 col( colTemp.x,colTemp.y,colTemp.z,alpha);

  BtlvObject_Gfmdl *fadeMdl = core->GetBgFadeModelObj();
  fadeMdl->SetVisible(isEnable);
  fadeMdl->SetConstantColor(NULL,0,col);

}

//背景カラーフェード(色操作)
BTL_SEQ_FUNC_DEF( EffSpBackColSet )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  gfl2::math::Vector3 colTemp;
  f32 alpha;

  file->LoadDataVec3( &colTemp );
  file->LoadDataF32( &alpha );

  gfl2::math::Vector4 col( colTemp.x,colTemp.y,colTemp.z,alpha);

  BtlvObject_Gfmdl *fadeMdl = core->GetBgFadeModelObj();

  if( frame == 0 )
  {
    fadeMdl->SetConstantColor(NULL,0,col);
  }
  else
  {
    gfl2::heap::HeapBase* heap = core->GetTempHeap();
    gfl2::math::Vector4 start = fadeMdl->GetConstantColor("BG_PlaneMT",0);
    DemoLib::Obj::CreateTask_ObjectGfMdl_ChangeConstantColor(core->GetTaskSystem(),heap,fadeMdl,"BG_PlaneMT",0,start,col,frame);

  }
}

BTL_SEQ_FUNC_DEF( EffDispBg )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  bool isDisp;
  bool maskLink;
  file->LoadDataBool( &isDisp );
  file->LoadDataBool( &maskLink );

  core->GetBgSystem()->SetVisible(isDisp);
  core->SetVisibleWeather(isDisp);
  if( maskLink )
  {
    core->GetEdgeMaskModelObj()->SetVisible(isDisp);
    core->GetFloorMaskModelObj()->SetVisible(isDisp);
  }
}

BTL_SEQ_FUNC_DEF( EffSpFadeIn )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int dispType;
  file->LoadDataInt( &dispType );

  gfl2::Fade::DISP disp;
  switch( dispType )
  {
  case SEQ_DEF_FADE_UPPER://上画面
    disp = gfl2::Fade::DISP_UPPER;
    break;
  case SEQ_DEF_FADE_LOWER://上画面
    disp = gfl2::Fade::DISP_LOWER;
    break;
  case SEQ_DEF_FADE_BOTH://両画面
    disp = gfl2::Fade::DISP_DOUBLE;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid axis type[%d]\n",dispType);
    disp = gfl2::Fade::DISP_DOUBLE;
  }
  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0.0f )
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(disp, gfl2::Fade::FADE_TYPE_ALPHA);
    }
    else
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(disp, gfl2::Fade::FADE_TYPE_ALPHA, frame);
    }
  }
}

BTL_SEQ_FUNC_DEF( EffSpFadeOut )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 col;
  int dispType;
  file->LoadDataInt( &dispType );
  file->LoadDataVec3( &col );

  gfl2::Fade::DISP disp;
  switch( dispType )
  {
  case SEQ_DEF_FADE_UPPER://上画面
    disp = gfl2::Fade::DISP_UPPER;
    break;
  case SEQ_DEF_FADE_LOWER://上画面
    disp = gfl2::Fade::DISP_LOWER;
    break;
  case SEQ_DEF_FADE_BOTH://両画面
    disp = gfl2::Fade::DISP_DOUBLE;
    break;
  default:
    GFL_ASSERT_MSG(0,"Invalid fade type[%d]\n",dispType);
    disp = gfl2::Fade::DISP_DOUBLE;
  }

  gfl2::math::Vector4 sCol(col);
  gfl2::math::Vector4 eCol(col);

  sCol.w = 0;
  eCol.w = 255;

  if( core->IsRecordStartSkip() == false )
  {
    if( frame == 0.0f )
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(disp, gfl2::Fade::FADE_TYPE_ALPHA,&sCol,&eCol);
    }
    else
    {
      GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestOut(disp, gfl2::Fade::FADE_TYPE_ALPHA,&sCol,&eCol,frame);
    }
  }
}

//ライト方向変更
BTL_SEQ_FUNC_DEF( EffLightDirection )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 dir;

  file->LoadDataVec3( &dir );

  core->GetRenderSys()->GetLightNode()->SetDirection( dir );
}

//キャラライト方向変更
BTL_SEQ_FUNC_DEF( EffCharaLightDirection )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isEnable;
  gfl2::math::Vector3 dir;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &isEnable );
  file->LoadDataVec3( &dir );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);
  
  chara->SetUseEffectLight(isEnable);
  
  BtlvRot_Rad( dir.x,dir.z,chara->GetRotateY());

  chara->SetEffectLightDir(dir);

}

//キャラライト変更(Env)
BTL_SEQ_FUNC_DEF( EffCharaLightEnv )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg,lightType;
  bool isEnable;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &lightType );
  file->LoadDataBool( &isEnable );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);
  chara->SetUseEffectLight(isEnable);
  if( isEnable )
  {
    int envIdx = core->GetTrainerLightEnv(lightType,trg);
    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envIdx);

    gfl2::renderingengine::scenegraph::resource::ResourceNode *envResNode;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode  *drawEnvNode;

    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
    envData.SetBinaryEnvData( static_cast<c8*>(buf) );
    envResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(core->GetTempGLAllocator(),&envData );
    drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(core->GetTempGLAllocator(),envResNode);

    if( drawEnvNode->GetLightSet(2) )
    {
      if( drawEnvNode->GetLightSet(2)->GetLightNode(0) )
      {
        gfl2::math::Vector3 dir = drawEnvNode->GetLightSet(2)->GetLightNode(0)->GetDirection();
        chara->SetEffectLightDir( dir );
        GFL_PRINT("SetTrainerLight[%f][%f][%f]\n",dir.x,dir.y,dir.z);
      }
    }

    GFL_SAFE_DELETE(drawEnvNode);
    GFL_SAFE_DELETE(envResNode);
  }
}


//キャラライト変更(ファイル)
BTL_SEQ_FUNC_DEF( EffCharaLightFile )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg,envIdx;
  bool isEnable;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &envIdx );
  file->LoadDataBool( &isEnable );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);
  chara->SetUseEffectLight(isEnable);
  if( isEnable )
  {
    void *buf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,envIdx);

    gfl2::renderingengine::scenegraph::resource::ResourceNode *envResNode;
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode  *drawEnvNode;

    gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
    envData.SetBinaryEnvData( static_cast<c8*>(buf) );
    envResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(core->GetTempGLAllocator(),&envData );
    drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(core->GetTempGLAllocator(),envResNode);

    if( drawEnvNode->GetLightSet(2) )
    {
      if( drawEnvNode->GetLightSet(2)->GetLightNode(0) )
      {
        gfl2::math::Vector3 dir = drawEnvNode->GetLightSet(2)->GetLightNode(0)->GetDirection();
        chara->SetEffectLightDir( dir );
        GFL_PRINT("SetTrainerLight[%f][%f][%f]\n",dir.x,dir.y,dir.z);
      }
    }

    GFL_SAFE_DELETE(drawEnvNode);
    GFL_SAFE_DELETE(envResNode);
  }
}

//ポケモンライト変更
BTL_SEQ_FUNC_DEF( EffPokeLightDirection )
{
DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isEnable;
  gfl2::math::Vector3 dir;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &isEnable );
  file->LoadDataVec3( &dir );

  int trgNum = 1;
  if( trg == SEQ_DEF_POS_DEF )
  {
    trgNum = core->SeqComFunc_GetTargetPokeNum();
  }

  for( int i=0;i<trgNum;i++)
  {
    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(trg,i);
    BtlvObject_PokeModel *poke = core->GetPokeModel(vPos);

    poke->SetUseEffectLight(isEnable);

    BtlvRot_Rad( dir.x,dir.z,poke->GetRotateY());

    poke->SetEffectLightDir(dir);
  }
}

//------------------------------------------------------------------------------------------------------------
//群体関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelCluster )
{
  DUMP_COMMAND
  
  //BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
}

//群体オブジェクト作成
BTL_SEQ_FUNC_DEF( ClusterCreate )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdxMdl,arcIdxAnm,drawType,rotOrder;
  int maxNum,time,interval,num,rate,life,type,axis,degS,degE,len;
  int isMulti;
  bool useAnm;

  gfl2::math::Vector3 size;
  int drawMode;

  file->LoadDataInt( &arcIdxMdl ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &arcIdxAnm ); //先読み処理の関係上先頭で！
  file->LoadDataBool( &useAnm ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &drawMode );
  file->LoadDataInt( &rotOrder );
  //以下共通
  file->LoadDataInt( &maxNum );
  file->LoadDataInt( &time );
  file->LoadDataInt( &interval );
  file->LoadDataInt( &num );
  file->LoadDataInt( &rate );
  file->LoadDataInt( &life );
  file->LoadDataInt( &type );
  file->LoadDataInt( &axis );
  file->LoadDataVec3( &size );
  file->LoadDataInt( &degS );
  file->LoadDataInt( &degE );
  file->LoadDataInt( &len );
  file->LoadDataInt( &isMulti );

  //シーケンスでCOMBO_TEXTのデフォルトがうまく動いていない・・・
  if( rotOrder == SEQ_DEF_ROTATE_ORDER_DEF )
  {
    rotOrder = gfl2::math::EulerRotation::RotationOrderZYX;
  }
  else
  {
    rotOrder--;
  }

  GFL_ASSERT_MSG(degS-degE != 0 , "角度指定は1以上！[%d][%d]\n",degS,degE); //@check エディタ上の数値のチェック、確認時に必ず発覚する
  GFL_ASSERT_MSG(interval != 0 , "生成感覚は1以上！[%d]\n",interval); //@check エディタ上の数値のチェック、確認時に必ず発覚する

  arcIdxMdl = core->CheckWazaDataId_Model(arcIdxMdl,0);
  arcIdxAnm = core->CheckWazaDataId_Model(arcIdxAnm,0);

  //パラメーター初期化(モデル専用
  BtlvObject_Cluster::ModelParam mdlParam;
  mdlParam.heap = core->GetTempHeap();
  mdlParam.allocator = core->GetTempGLAllocator();
  mdlParam.pipe = core->GetRenderSys()->GetRenderingPipeLine();
  mdlParam.drawOpt = drawMode;
  mdlParam.rotOrder = rotOrder;
  mdlParam.modelBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,arcIdxMdl);
  if( useAnm )
  {
    mdlParam.anmBuf = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,arcIdxAnm);
  }
  else
  {
    mdlParam.anmBuf = NULL;
  }


  //パラメーター初期化(共通
  BtlvObject_Cluster::SetupParam opt;
  opt.heap= core->GetTempHeap();
  opt.objManager = core->GetObjectManager();
  opt.objectMax = maxNum;

  opt.spawnType = DemoLib::Obj::Object_Cluster::SPAWN_PARENT;
  opt.spawnTime = time;
  opt.spawnInterval = interval;
  opt.spawnNum = num;
  opt.spawnRate = rate;

  opt.childLifeTime = life;

  opt.emitterType = type;
  opt.emitterAxis = axis;
  opt.emitterSize = size;
  opt.emitterStartAngle = degS;
  opt.emitterEndAngle = degE;
  opt.emitterCavityRate = len;
  
  //生成処理
  gfl2::heap::HeapBase* heap = core->GetTempHeap();
  int objNum = 1;
  if( isMulti == true )
  {
    objNum = core->SeqComFunc_GetTargetPokeNum();
  }
  BtlvObject_Cluster **objArr = GFL_NEW_ARRAY(heap)BtlvObject_Cluster*[objNum];

  core->GetClusterObjManager()->ClearBuffer(core->GetObjectManager());
  core->GetClusterObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)BtlvObject_Cluster();
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->CreateModel(opt,mdlParam);

    DemoLib::Obj::CreateTask_DeleteObject_Cluster(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

//群体オブジェクト作成(パーティクル
BTL_SEQ_FUNC_DEF( ClusterCreateEffect )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int arcIdx,drawType,rotOrder;
  int maxNum,time,interval,num,rate,life,type,axis,degS,degE,len;
  int isMulti;

  gfl2::math::Vector3 size;

  file->LoadDataInt( &arcIdx ); //先読み処理の関係上先頭で！
  file->LoadDataInt( &drawType ); 
  file->LoadDataInt( &rotOrder );
  //以下共通
  file->LoadDataInt( &maxNum );
  file->LoadDataInt( &time );
  file->LoadDataInt( &interval );
  file->LoadDataInt( &num );
  file->LoadDataInt( &rate );
  file->LoadDataInt( &life );
  file->LoadDataInt( &type );
  file->LoadDataInt( &axis );
  file->LoadDataVec3( &size );
  file->LoadDataInt( &degS );
  file->LoadDataInt( &degE );
  file->LoadDataInt( &len );
  file->LoadDataInt( &isMulti );

  GFL_ASSERT_MSG(degS-degE != 0 , "角度指定は1以上！[%d][%d]\n",degS,degE); //@check エディタ上の数値のチェック、確認時に必ず発覚する
  GFL_ASSERT_MSG(interval != 0 , "生成感覚は1以上！[%d]\n",interval); //@check エディタ上の数値のチェック、確認時に必ず発覚する

  //シーケンスでCOMBO_TEXTのデフォルトがうまく動いていない・・・
  if( rotOrder == SEQ_DEF_ROTATE_ORDER_DEF )
  {
    rotOrder = gfl2::math::EulerRotation::RotationOrderZYX;
  }
  else
  {
    rotOrder--;
  }

  arcIdx = core->CheckWazaDataId_Effect(arcIdx,0);

  //パラメーター初期化(エフェクト専用
  BtlvObject_Cluster::EffectParam effParam;
  effParam.heap = core->GetTempHeap();
  effParam.effHeap = core->GetEffectHeap();
  effParam.effSystem = core->GetEffectSystem();
  effParam.drawOpt = drawType;
  effParam.rotOrder = rotOrder;
  effParam.effResIdx = core->GetResourceManager()->GetEffectResIdx(ARCID_WAZAEFF_RESOURCE,arcIdx);

  //パラメーター初期化(共通
  BtlvObject_Cluster::SetupParam opt;
  opt.heap= core->GetTempHeap();
  opt.objManager = core->GetObjectManager();
  opt.objectMax = maxNum;

  opt.spawnType = DemoLib::Obj::Object_Cluster::SPAWN_PARENT;
  opt.spawnTime = time;
  opt.spawnInterval = interval;
  opt.spawnNum = num;
  opt.spawnRate = rate;

  opt.childLifeTime = life;

  opt.emitterType = type;
  opt.emitterAxis = axis;
  opt.emitterSize = size;
  opt.emitterStartAngle = degS;
  opt.emitterEndAngle = degE;
  opt.emitterCavityRate = len;
  
  //生成処理
  gfl2::heap::HeapBase* heap = core->GetTempHeap();
  int objNum = 1;
  if( isMulti == true )
  {
    objNum = core->SeqComFunc_GetTargetPokeNum();
  }
  BtlvObject_Cluster **objArr = GFL_NEW_ARRAY(heap)BtlvObject_Cluster*[objNum];

  core->GetClusterObjManager()->ClearBuffer(core->GetObjectManager());
  core->GetClusterObjManager()->SetObjectArr(objArr,objNum,grpNo);

  for( int i=0;i<objNum;i++)
  {
    objArr[i] = GFL_NEW(heap)BtlvObject_Cluster();
    core->GetObjectManager()->AddObject(objArr[i]);

    objArr[i]->CreateEffect(opt,effParam);

    DemoLib::Obj::CreateTask_DeleteObject_Cluster(core->GetTaskSystem(),heap,objArr[i],frame);
  }
}

//群体オブジェクト移動
BTL_SEQ_FUNC_DEF( ClusterMovePosition )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  gfl2::math::Vector3 tempPos;
  bool isRelative;

  file->LoadDataVec3( &tempPos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    gfl2::math::Vector3 pos = tempPos;
    BtlvObject_Cluster *obj = objArr[i];
    if( isRelative == true )
    {
      pos += obj->GetPosition();
    }

    if( frame == 0 )
    {
      obj->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,obj,obj->GetPosition(),pos,frame);
    }
  }
}

//群体オブジェクト移動(ポケモン相対)
BTL_SEQ_FUNC_DEF( ClusterMoveRelativePoke )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  
  BTLV_RELARIVE_POKE_OPTION opt;
  int trg;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &opt.node );
  file->LoadDataVec3( &opt.offset );
  file->LoadDataInt( &opt.rate );
  file->LoadDataInt( &opt.adjustFlip );
  file->LoadDataInt( &opt.adjustScale );
  file->LoadDataInt( &opt.autoRotateModel );
  file->LoadDataInt( &opt.autoRotatePos );
  file->LoadDataBool( &opt.forceUpdate );

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    opt.vpos = core->SeqComFunc_GetTargetPoke(trg,i);
    opt.vposSub = core->SeqComFunc_GetTargetPokeSub(trg);
    
    core->SeqComFunc_MoveRelativePoke(obj,frame,opt);

  }
}

//群体オブジェクト移動(特殊座標)
BTL_SEQ_FUNC_DEF( ClusterMoveSpecialPos )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int trg;
  gfl2::math::Vector3 offset;
  bool isFlip,isRot;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &offset );
  file->LoadDataBool( &isFlip );
  file->LoadDataBool( &isRot );

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];

    core->SeqComFunc_MoveSpecialPos(obj,frame,trg,offset,isRot,isFlip);
  }

}

//群体オブジェクトスケール
BTL_SEQ_FUNC_DEF( ClusterScale )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 sclBase;
  bool isRelative;

  file->LoadDataVec3( &sclBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];

    gfl2::math::Vector3 scl = sclBase;
    if( isRelative == true )
    {
      scl = scl.MulPerElem(obj->GetScale());
    }

    GFL_ASSERT_MSG(scl.x!=0&&scl.y!=0&&scl.z!=0 , "スケールを0にしないでください！\n" );

    if( frame == 0 )
    {
      obj->SetScale(scl);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectScale(core->GetTaskSystem(),heap,obj,obj->GetScale(),scl,frame);
    }
  }
}

//群体オブジェクト回転
BTL_SEQ_FUNC_DEF( ClusterRotate )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  

  gfl2::math::Vector3 rotBase;
  bool isRelative;

  file->LoadDataVec3( &rotBase );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];

    gfl2::math::Vector3 rot = rotBase;
    if( isRelative == true )
    {
      rot += obj->GetRotate();
    }

    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}
//群体オブジェクト回転(ポケモン指定
BTL_SEQ_FUNC_DEF( ClusterRotatePoke )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int dirPoke;
  int dirNode;
  f32 ofs;
  bool isVertical;

  file->LoadDataInt( &dirPoke );
  file->LoadDataInt( &dirNode );
  file->LoadDataF32( &ofs );
  file->LoadDataBool( &isVertical );
  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];

    BtlvPos vPos = core->SeqComFunc_GetTargetPoke(dirPoke,i);
    gfl2::math::Vector3 rot = obj->GetRotate();
    core->SeqComFunc_CalcPokeDir(obj->GetPosition(),vPos,dirNode,&rot,isVertical);

    rot.y += ofs;
    if( frame == 0 )
    {
      obj->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,obj,obj->GetRotate(),rot,frame);
    }
  }
}

//群体オブジェクト 初速設定(拡散
BTL_SEQ_FUNC_DEF( ClusterSpeedDiffuse )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int type;
  gfl2::math::Vector3 ofs;
  f32 spd,acc;

  file->LoadDataInt( &type );
  file->LoadDataVec3( &ofs );
  file->LoadDataF32( &spd );
  file->LoadDataF32( &acc );
  
  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();

    setupParam->spdType = BtlvObject_Cluster::SPEED_DIFFUSE;
    setupParam->spdAxis = type;
    setupParam->spdVec = ofs;
    setupParam->spdSpd = spd;
    setupParam->spdAcc = acc;
  }
}

//群体オブジェクト 初速設定(指定方向
BTL_SEQ_FUNC_DEF( ClusterSpeedDir )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 rot;
  int rotRand;
  f32 spd,acc;

  file->LoadDataVec3( &rot );
  file->LoadDataInt( &rotRand );
  file->LoadDataF32( &spd );
  file->LoadDataF32( &acc );

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();
      
    setupParam->spdType = BtlvObject_Cluster::SPEED_DIR;
    setupParam->spdVec = rot;
    setupParam->spdDecRand = rotRand;
    setupParam->spdSpd = spd;
    setupParam->spdAcc = acc;
  }
}

//群体オブジェクト 加速設定(重力
BTL_SEQ_FUNC_DEF( ClusterAccelGravity )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int type;
  bool isAbs;
  gfl2::math::Vector3 pos;
  f32 pow;

  file->LoadDataInt( &type );
  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isAbs );
  file->LoadDataF32( &pow );

  
  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();
      
    setupParam->accelType = BtlvObject_Cluster::ACCEL_GRAVITY;
    setupParam->accelAxis = type;
    setupParam->accelPos = pos;
    setupParam->accelIsAbsolute = isAbs;
    setupParam->accelRate = pow;
  }
}

//群体オブジェクト 初期スケール
BTL_SEQ_FUNC_DEF( ClusterStartScale )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  gfl2::math::Vector3 min,max;
  bool isSync;
  file->LoadDataVec3( &min );
  file->LoadDataVec3( &max );
  file->LoadDataBool( &isSync );
  

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();
      
    setupParam->initSclMin = min;
    setupParam->initSclMax = max;
    setupParam->syncInitScl = isSync;
  }
}

//群体オブジェクト 初期回転
BTL_SEQ_FUNC_DEF( ClusterStartRotate )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;
  gfl2::math::Vector3 min,max;
  file->LoadDataVec3( &min );
  file->LoadDataVec3( &max );
  
  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();
      
    setupParam->initRotMin = min;
    setupParam->initRotMax = max;
  }
}

//群体オブジェクト 拡縮アニメ
BTL_SEQ_FUNC_DEF( ClusterAnimeScale )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
//  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int frame[BtlvObject_Cluster::ANIME_PARAM_NUM];
  gfl2::math::Vector3 scale[BtlvObject_Cluster::ANIME_PARAM_NUM];

  for( int i=0;i<BtlvObject_Cluster::ANIME_PARAM_NUM;i++ )
  {
    file->LoadDataInt( &frame[i] );
    file->LoadDataVec3( &scale[i] );
  }

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();

    for( int ii=0;ii<BtlvObject_Cluster::ANIME_PARAM_NUM;ii++ )
    {
      setupParam->animScaleFrame[ii] = frame[ii];
      setupParam->animScaleValue[ii] = scale[ii];
    }
  }
}

//群体オブジェクト 回転アニメ
BTL_SEQ_FUNC_DEF( ClusterAnimeRotate )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
//  const u32 frame = param.endFrame - param.startFrame;
  const u32 grpNo = param.groupNo;

  int frame[BtlvObject_Cluster::ANIME_PARAM_NUM];
  gfl2::math::Vector3 rotate[BtlvObject_Cluster::ANIME_PARAM_NUM];

  for( int i=0;i<BtlvObject_Cluster::ANIME_PARAM_NUM;i++ )
  {
    file->LoadDataInt( &frame[i] );
    file->LoadDataVec3( &rotate[i] );
  }

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();

    for( int ii=0;ii<BtlvObject_Cluster::ANIME_PARAM_NUM;ii++ )
    {
      setupParam->animRotateFrame[ii] = frame[ii];
      setupParam->animRotateValue[ii].x = rotate[ii].x;
      setupParam->animRotateValue[ii].y = rotate[ii].y;
      setupParam->animRotateValue[ii].z = rotate[ii].z;
    }
  }
}

//群体オブジェクト 反射設定
BTL_SEQ_FUNC_DEF( ClusterSetRefrect )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int axis,type;
  bool isAbs;
  f32 pos,brake;
  
  file->LoadDataF32( &pos );
  file->LoadDataBool( &isAbs );
  file->LoadDataInt( &axis );
  file->LoadDataInt( &type );
  file->LoadDataF32( &brake );


   int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::RefrectParam *setupParam = obj->GetRefrectParam();

    setupParam->pos = pos;
    setupParam->isAbs = isAbs;
    setupParam->axis = axis;
    setupParam->type = type;
    setupParam->brake = brake;
  }
}

//群体オブジェクト 生成時間変更
BTL_SEQ_FUNC_DEF( ClusterSetSpawnTime )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int time;
  
  file->LoadDataInt( &time );


   int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();

    setupParam->spawnTime = time;
  }
}
//群体オブジェクト 子挙動設定
BTL_SEQ_FUNC_DEF( ClusterSetChild )
{
  DUMP_COMMAND
  
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 grpNo = param.groupNo;

  int parentGrp,type;
  bool isFollow;
  
  file->LoadDataInt( &parentGrp );
  file->LoadDataInt( &type );
  file->LoadDataBool( &isFollow );

  int objNum;
  BtlvObject_Cluster **objArr = core->GetClusterObjManager()->GetObjectArr(grpNo,&objNum);
  int parentObjNum;
  BtlvObject_Cluster **parentObjArr = core->GetClusterObjManager()->GetObjectArr(parentGrp,&parentObjNum);
  for( int i=0;i<objNum;i++)
  {
    BtlvObject_Cluster *obj = objArr[i];
    BtlvObject_Cluster *parentObj;
    BtlvObject_Cluster::SetupParam *setupParam = obj->GetSetupParam();

    setupParam->spawnType = type;
    setupParam->isFollowParent = isFollow;
    if( parentObjNum == 1 )
    {
      parentObj = parentObjArr[0];
    }
    else
    {
      parentObj = parentObjArr[i];
    }
    obj->SetParent(parentObj);
    parentObj->SetChild(obj);
  }
}


//------------------------------------------------------------------------------------------------------------
//トレーナー関係
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelTrainer )
{
 DUMP_COMMAND;
}

//トレーナー移動
BTL_SEQ_FUNC_DEF( TrainerMove )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isRelative;
  gfl2::math::Vector3 pos;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &pos );
  file->LoadDataBool( &isRelative );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    if( isRelative == true )
    {
      pos += chara->GetPosition();
    }

    if( frame == 0 )
    {
      chara->SetPosition(pos);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,chara,chara->GetPosition(),pos,frame);
    }
  }
}

//トレーナー一初期化
BTL_SEQ_FUNC_DEF( TrainerMoveReset )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isOrigin;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &isOrigin );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);
  int idx = core->SeqComFunc_GetTargetCharaIndex(trg);
  GFL_ASSERT_STOP( idx==-1 ? 0:1 );

  if( chara )
  {
    int degY;
    gfl2::math::Vector3 pos(0,0,0);

    core->GetDefaultTrainerPos(idx,&pos,&degY,isOrigin);
    gfl2::math::Vector3 rot(0,degY,0);

    if( frame == 0 )
    {
      chara->SetPosition(pos);
      chara->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectMove(core->GetTaskSystem(),heap,chara,chara->GetPosition(),pos,frame);
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,chara,chara->GetRotate(),rot,frame);
    }
  }
}

//トレーナー回転
BTL_SEQ_FUNC_DEF( TrainerRotate )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isRelative;
  gfl2::math::Vector3 rot;

  file->LoadDataInt( &trg );
  file->LoadDataVec3( &rot );
  file->LoadDataBool( &isRelative );


  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    if( isRelative == true )
    {
      rot += chara->GetRotate();
    }

    if( frame == 0 )
    {
      chara->SetRotate(rot);
    }
    else
    {
      DemoLib::Obj::CreateTask_ObjectRotate(core->GetTaskSystem(),heap,chara,chara->GetRotate(),rot,frame);
    }
  }
}

//トレーナー表示切替
BTL_SEQ_FUNC_DEF( TrainerDisp )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isDisp;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &isDisp );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    chara->SetVisible(isDisp);
  }
}

//トレーナー表示切替(全部)
BTL_SEQ_FUNC_DEF( TrainerDispAll )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  bool isDisp;

  file->LoadDataBool( &isDisp );

  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    BtlvObject_Chara *chara = core->GetTrainerModelObj(i);
    if( chara )
    {
      chara->SetVisible(isDisp);
    }
  }
}

//トレーナー表示切替(非対象者)
BTL_SEQ_FUNC_DEF( TrainerDispOther )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  bool isDisp;

  file->LoadDataBool( &isDisp );

  for( int i=0;i<TRAINER_MODEL_NUM;i++ )
  {
    BtlvObject_Chara *chara = core->GetTrainerModelObj(i);
    if( chara )
    {
      bool isSet = true;
      if( i == core->SeqComFunc_GetTargetCharaIndex(SEQ_DEF_TRAINER_SEQ_TARGET1) )
      {
        isSet = false;
      }
      if( i == core->SeqComFunc_GetTargetCharaIndex(SEQ_DEF_TRAINER_SEQ_TARGET2) )
      {
        isSet = false;
      }
      if( isSet == true )
      {
        chara->SetVisible(isDisp);
      }
    }
  }
}

//トレーナー表示切替(影)
BTL_SEQ_FUNC_DEF( TrainerDispShadow )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg;
  bool isDisp;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &isDisp );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    chara->SetShadowVisible(isDisp);
  }
}
//トレーナーモーション変更
BTL_SEQ_FUNC_DEF( TrainerChangeMotion )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg,motNo;
  bool isLoop;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &motNo );
  file->LoadDataBool( &isLoop );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    if( chara->GetDressUpModel() )
    {
      if( chara->GetIsDressUpAnime() )
      {
        chara->FreeDressUpAnimationNodes();
        chara->ClearDressUpAnimation();
      }
    }

    bool isChageAnime = false;
    if( motNo == SEQ_DEF_TR_MOTION_BALL_M )
    {
      //アニメの有無の判定ができないので、アニメを変更して、成功したかのチェック
      chara->ChangeAnimation(motNo);
      if( chara->GetBaseModel()->GetAnimationResourceNode() == NULL )
      {
        //マルチ投げが無い場合は通常投げに変更
        motNo = SEQ_DEF_TR_MOTION_BALL;
      }
      else
      {
        isChageAnime = true;
      }
    }

    if( motNo == SEQ_DEF_TR_MOTION_BALL )
    {
      if( chara->GetDynamicAnimeType() != 0 )
      {
        if (chara->GetThrowType() == 8)
        {
          //momiji追加のためIndexが飛んでる
          chara->ChangeAnimation((0xb) * 0x00010000);
        }
        else
        {
          chara->ChangeAnimation((chara->GetThrowType() + 1) * 0x00010000);
        }
      }
      else
      {
        chara->ChangeAnimation(motNo);
      }
    }
    else
    if( motNo == SEQ_DEF_TR_MOTION_POSE )
    {
      if( chara->GetDynamicAnimeType() != 0 )
      {
        if (chara->GetThrowType() == 8)
        {
          //momiji追加のためIndexが飛んでる
          chara->ChangeAnimation((0xb) * 0x00010000 + 1);
        }
        else
        {
          chara->ChangeAnimation((chara->GetThrowType() + 1) * 0x00010000 + 1);
        }
      }
      else
      {
        chara->ChangeAnimation(TRAINER_ANIME_WAIT1);
      }
    }
    else
    if (motNo == SEQ_DEF_TR_MOTION_BALL_M0)
    {
      //マルチ投げの前半。プレイヤーとNPCでアニメ番号が違う
      if (chara->GetDressUpModel() != NULL)
      {
        if (chara->GetThrowType() == 8)
        {
          //momiji追加のためIndexが飛んでる
          chara->ChangeAnimation((0xb) * 0x00010000 + 2);
        }
        else
        {
          chara->ChangeAnimation((chara->GetThrowType() + 1) * 0x00010000 + 2);
        }
      }
      else
      {
        chara->ChangeAnimation(TRAINER_ANIME_MULTIBALL00);
      }
    }
    else
    {
      if( isChageAnime == false )
      {
        chara->ChangeAnimation(motNo);
      }
    }

    if( motNo == SEQ_DEF_TR_MOTION_WAIT1 )
    {
      //待機の場合はループを外で制御するのでそのままにする
      isLoop = false;
    }

    chara->SetAnimationIsLoop(isLoop);
    //BALL01はカメラの尺と合わずに戻ってしまうので、手動で戻すようにする
    if( motNo == SEQ_DEF_TR_MOTION_WAIT1 )
    {
      chara->SetAutoResetAniation(true);
    }
    else
    {
      chara->SetAutoResetAniation(false);
    }
  }
}

//トレーナーモーション変更(外部)
BTL_SEQ_FUNC_DEF( TrainerChangeMotionResource )
{
 DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg,arcIdx;
  bool isLoop;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &arcIdx );
  file->LoadDataBool( &isLoop );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes = core->GetResourceManager()->GetDataBuf_Anime(ARCID_WAZAEFF_RESOURCE,arcIdx);

    chara->ChangeAnimationResource(anmRes);
    chara->SetAnimationIsLoop(isLoop);
    chara->SetAutoResetAniation(false);
  }
}

//トレーナーモーション変更(着せ替え)
BTL_SEQ_FUNC_DEF( TrainerChangeMotionKisekae )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int trg,arcIdx;
  bool isLoop;

  file->LoadDataInt( &trg );
  file->LoadDataInt( &arcIdx );
  file->LoadDataBool( &isLoop );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    void *anmRes = core->GetResourceManager()->GetDataBuf(ARCID_WAZAEFF_RESOURCE,arcIdx);

    chara->ChangeAnimationResourceDressUp(anmRes,core->GetTempGLAllocator());
    chara->SetIsDressUpAnime(true);
    chara->SetAnimationIsLoop(isLoop);
    chara->SetAutoResetAniation(false);
  }
}

//トレーナーモーション速度変更
BTL_SEQ_FUNC_DEF( TrainerChangeMotionSpeed )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg;
  f32 speed;
  
  file->LoadDataInt( &trg );
  file->LoadDataF32( &speed );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    chara->SetAnimationSpeed(speed);
  }
}

//トレーナーモーション フレーム指定
BTL_SEQ_FUNC_DEF( TrainerSetMotionFrame )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trg;
  f32 frame;

  file->LoadDataInt( &trg );
  file->LoadDataF32( &frame );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    chara->SetAnimationFrame(frame);
  }
}

//トレーナーエッジ切り替え
BTL_SEQ_FUNC_DEF( TrainerEdgeEnable )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  int trg;
  bool enable;

  file->LoadDataInt( &trg );
  file->LoadDataBool( &enable );

  BtlvObject_Chara *chara = core->SeqComFunc_GetTargetChara(trg);

  if( chara )
  {
    if( chara->GetDressUpModel() )
    {
      //着せ替え
      if( enable )
      {
        core->GetRenderSys()->GetRenderingPipeLine()->AddEdge_DressUpChara(chara->GetDressUpModel());
      }
      else
      {
        core->GetRenderSys()->GetRenderingPipeLine()->RemoveEdge_DressUpChara(chara->GetDressUpModel());
      }
    }
    else
    {
      //通常
      if( enable )
      {
        core->GetRenderSys()->GetRenderingPipeLine()->AddEdge_Chara(chara->GetCharaModel());
      }
      else
      {
        core->GetRenderSys()->GetRenderingPipeLine()->RemoveEdge_Chara(chara->GetCharaModel());
      }
    }
  }
}

//------------------------------------------------------------------------------------------------------------
//特殊処理
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( LabelSpecial )
{
  DUMP_COMMAND;

  //BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
}

//メッセージ表示(btl_std
BTL_SEQ_FUNC_DEF( MessageDispStd )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  int msgId,option;
  file->LoadDataInt( &msgId );
  file->LoadDataInt( &option );

  //core->GetUiSys()->GetWorkStr(0)->

  if( core->GetBtlvMode() != BTLV_MODE_BATTLE )
  {
    core->SeqComFunc_SetEffectMessage_Std(msgId,core->GetWorkStr(0));
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
  }
  else
  {
    switch( msgId )
    {
    //------------------------------------
    //エンカウント系
    //------------------------------------
    case BTL_STRID_STD_Encount_Wild1:
    case BTL_STRID_STD_Encount_Wild2:
    case BTL_STRID_STD_Encount_Wild_Move:
    case BTL_STRID_STD_Encount_Wild_Talk:
    case BTL_STRID_STD_Encount_Wild_Ride:
    case BTL_STRID_STD_Encount_Wild_Legend:
    case BTL_STRID_STD_Encount_Band:
    case BTL_STRID_STD_Encount_Nusi:
      {
        BtlvPos defPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
        BTL_POKEPARAM *defBpp = core->GetBattlePokeParam(defPos);
        //野生の○○が現れた
        str::MakeStringStd( core->GetWorkStr(0) , msgId , 1 , defBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      break;
    case BTL_STRID_STD_Encount_NPC1:
    case BTL_STRID_STD_Encount_Player1:
      {
        //ミニスカートの○○が現れた
        str::MakeStringStd( core->GetWorkStr(0) , msgId , 1 , core->GetEnemyClientID(0) );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      break;
    case BTL_STRID_STD_Encount_NPC2:
    case BTL_STRID_STD_Encount_Player2:
      {
        u8 clientId = core->GetEnemyClientID(0);
        if( core->IsClientNpc(clientId) == false )
        {
          //プレイヤー ○○と○○が現れた
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_Encount_Player2 , 2 , core->GetEnemyClientID(0) , core->GetEnemyClientID(1) );
          core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
        }
        else
        {
          u16 trtype1 = core->GetTrainerType(core->GetEnemyClientID(0));
          u16 trtype2 = core->GetTrainerType(core->GetEnemyClientID(1));
          int tagType = trainer::GetTagTrainerType(trtype1,trtype2);
          if( tagType != -1 )
          {
            //NPC ふたごちゃんの○○と○○が現れた
            str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_Encount_Tag , 3 , tagType , core->GetEnemyClientID(0) , core->GetEnemyClientID(1) );
            core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
          }
          else
          {
            //NPC ミニスカートの○○とミニスカートの○○が現れた
            str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_Encount_NPC2 , 2 , core->GetEnemyClientID(0) , core->GetEnemyClientID(1) );
            core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
          }
        }
      }
      break;

    //------------------------------------
    //繰り出し系
    //------------------------------------
    case BTL_STRID_STD_PutSingle://行けっ○○
    case BTL_STRID_STD_PutSingle_Player:
    case BTL_STRID_STD_PutSingle_NPC:
      //自分側繰り出し
      {
        BtlvPos vpos;
        if( option == 0 )
        {
          vpos = static_cast<BtlvPos>(core->SeqComFunc_GetTargetCharaIndex( SEQ_DEF_TRAINER_SEQ_TARGET1 ));
        }
        else
        {
          vpos = static_cast<BtlvPos>(core->SeqComFunc_GetTargetCharaIndex( SEQ_DEF_TRAINER_SEQ_TARGET2 ));
        }

        BTL_POKEPARAM *atkBpp1 = core->GetBattlePokeParam(vpos);

        u8 clientId = core->GetBtlvPosToClientId(vpos);
        if( clientId == core->GetClientID() )
        {
          //自分の投げ
          //仲良し度分岐
          if( core->GetBtlvMode() == BTLV_MODE_BATTLE && core->GetMainModule()->GetPokeFriendship(atkBpp1) >= 100 )
          {
            str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_FR_PutPoke , 1 , atkBpp1->GetID() );
          }
          else
          {
            str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutSingle , 1 , atkBpp1->GetID() );
          }
        }
        else
        if( core->IsClientNpc(clientId) == false )
        {
          //プレイヤー
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutSingle_Player , 2 , clientId , atkBpp1->GetID() );
        }
        else
        {
          //○○は○○を繰り出した
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutSingle_NPC , 2 , clientId ,atkBpp1->GetID() );
        }

        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      break;

    case BTL_STRID_STD_PutDouble://行けっ○○
    case BTL_STRID_STD_PutDouble_Player:
    case BTL_STRID_STD_PutDouble_NPC:
      {
        BtlvPos vpos;
        if( option == 0 )
        {
          vpos = static_cast<BtlvPos>(core->SeqComFunc_GetTargetCharaIndex( SEQ_DEF_TRAINER_SEQ_TARGET1 ));
        }
        else
        {
          vpos = static_cast<BtlvPos>(core->SeqComFunc_GetTargetCharaIndex( SEQ_DEF_TRAINER_SEQ_TARGET2 ));
        }
        BTL_POKEPARAM *atkBpp1 = core->GetBattlePokeParam(core->SeqComFunc_GetTargetPoke_Org(0));
        BTL_POKEPARAM *atkBpp2 = core->GetBattlePokeParam(core->SeqComFunc_GetTargetPoke_Org(1));

        u8 clientId = core->GetBtlvPosToClientId(vpos);
        if( clientId == core->GetClientID() )
        {
          //自分側繰り出し
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutDouble , 2 , atkBpp1->GetID(), atkBpp2->GetID() );
        }
        else
        if( core->IsClientNpc(clientId) == false )
        {
          //プレイヤー
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutDouble_Player , 3 , clientId , atkBpp1->GetID(), atkBpp2->GetID() );
        }
        else
        {
          //"[Name:TrainerType ]の　[Name:TrainerName Idx=""1"" ]は
          //[Name:PokemonNickname Idx=""2"" ]と▽
          //[Name:PokemonNickname Idx=""3"" ]を　くりだした！"
          str::MakeStringStd( core->GetWorkStr(0) , msgId , 3 , clientId,atkBpp1->GetID(),atkBpp2->GetID() );
        }

        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      break;

      //ロイヤル用繰り出し(自他分岐、Clientで色分岐
    case BTL_STRID_STD_PutRoyal_bl_NPC:
      {
        BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
        BTL_POKEPARAM *atkBpp = core->GetBattlePokeParam(atkPos);
        u8 clientId = core->GetBtlvPosToClientId(atkPos);

        if( core->IsClientNpc(clientId) == false )
        {
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutRoyal_bl_player+clientId , 2 , clientId , atkBpp->GetID() );
        }
        else
        {
          str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutRoyal_bl_NPC+clientId , 2 , clientId , atkBpp->GetID() );
        }
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
        core->GetUiSys()->SetAutoStepMessage(true);
        //NMCat4176 メッセージ速度が遅いの場合切れることがある
        core->GetUiSys()->SetMessageSpeedForce_Normal();
      }
      break;
    //------------------------------------
    //その他
    //------------------------------------
    case BTL_STRID_STD_Intrude_Call:
    case BTL_STRID_STD_Intrude_Appear:
      {
        BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
        BTL_POKEPARAM *atkBpp = core->GetBattlePokeParam(atkPos);
        str::MakeStringStd( core->GetWorkStr(0) , msgId , 1 , atkBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) );
      }
      break;
      //ぬし登場時の能力アップ
    case BTL_STRID_STD_Nusi_DefaultPowerUp_Atk_1:
    case BTL_STRID_STD_DefaultPowerUp_Atk_1:
      {
        u32 strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Atk_1;
        //能力の種類で分岐する
        const PartyParam *partyParam = core->GetPartySetupParam( core->GetEnemyClientID(0) );
        if( partyParam->defaultPowerUpDesc[0].rankUp_Attack > 0 &&
            partyParam->defaultPowerUpDesc[0].rankUp_Defense > 0 &&
            partyParam->defaultPowerUpDesc[0].rankUp_SpAttack > 0 &&
            partyParam->defaultPowerUpDesc[0].rankUp_SpDefense > 0 &&
            partyParam->defaultPowerUpDesc[0].rankUp_Agility > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_All_1+partyParam->defaultPowerUpDesc[0].rankUp_Attack-1;
        }
        else
        if( partyParam->defaultPowerUpDesc[0].rankUp_Attack > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Atk_1+partyParam->defaultPowerUpDesc[0].rankUp_Attack-1;
        }
        else
        if( partyParam->defaultPowerUpDesc[0].rankUp_Defense > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Def_1+partyParam->defaultPowerUpDesc[0].rankUp_Defense-1;
        }
        else
        if( partyParam->defaultPowerUpDesc[0].rankUp_SpAttack > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Spatk_1+partyParam->defaultPowerUpDesc[0].rankUp_SpAttack-1;
        }
        else
        if( partyParam->defaultPowerUpDesc[0].rankUp_SpDefense > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Spdef_1+partyParam->defaultPowerUpDesc[0].rankUp_SpDefense-1;
        }
        else
        if( partyParam->defaultPowerUpDesc[0].rankUp_Agility > 0 )
        {
          strId = BTL_STRID_STD_Nusi_DefaultPowerUp_Agi_1+partyParam->defaultPowerUpDesc[0].rankUp_Agility-1;
        }
        //汎用能力アップ時の対応
        if( msgId == BTL_STRID_STD_DefaultPowerUp_Atk_1 )
        {
          strId = (strId-BTL_STRID_STD_Nusi_DefaultPowerUp_Atk_1)+BTL_STRID_STD_DefaultPowerUp_Atk_1;
        }

        BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
        BTL_POKEPARAM *atkBpp = core->GetBattlePokeParam(atkPos);
        str::MakeStringStd( core->GetWorkStr(0) , strId , 1 , atkBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) );
      }
      break;
    case BTL_STRID_STD_Royal_Opening: //ロイヤル導入
      //メッセージ自動送りのためだけなので、ここ以外にあればシーケンスのフラグにするかも
      str::MakeStringStd( core->GetWorkStr(0) , msgId , 0 );
      core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) );
      core->GetUiSys()->SetAutoStepMessage(true);
      break;
    default:
      {
        BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
        BTL_POKEPARAM *bpp = core->GetBattlePokeParam(atkPos);
        str::MakeStringStd( core->GetWorkStr(0) , msgId , 1 , bpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) );
        ARIIZUMI_PRINT("DefaultMessageParam.\n");
      }
      break;
    }
  }
}

//メッセージ表示(btl_set
BTL_SEQ_FUNC_DEF( MessageDispSet )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  int msgId,option;
  file->LoadDataInt( &msgId );
  file->LoadDataInt( &option );

  if( core->GetBtlvMode() != BTLV_MODE_BATTLE )
  {
    core->SeqComFunc_SetEffectMessage_Set(msgId,core->GetWorkStr(0));
  }
  else
  {
    switch( msgId )
    {
    case BTL_STRID_SET_AuraEffect_None:
      {
        //外からもらっている
        str::MakeStringSet( core->GetWorkStr(0) , core->GetZenryokuParam()->aulaMsgId , core->GetZenryokuParam()->aulaMsgParam );

      }
      break;
    default:
      {
        BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
        BTL_POKEPARAM *bpp = core->GetBattlePokeParam(atkPos);
        int arr[1] = {bpp->GetID()};
        str::MakeStringSet( core->GetWorkStr(0) , msgId , arr );
        ARIIZUMI_PRINT("DefaultMessageParam.\n");
      }
      break;
    }
  }
  core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) );

  if( msgId == BTL_STRID_SET_AuraEffect_None ||
    msgId == BTL_STRID_SET_ZenryokuPower )
  {
    core->GetUiSys()->SetMessageSpeedForce_Normal();
  }
}

//メッセージ消去
BTL_SEQ_FUNC_DEF( MessageHide )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  core->GetUiSys()->HideMessage();
}

//シーケンス一時停止
BTL_SEQ_FUNC_DEF( SpecialWaitSequence )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  int type;
  file->LoadDataInt( &type );
  core->SetSequenceWaitFunc(type);
}

BTL_SEQ_FUNC_DEF( SpecialSyncDemoFade )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
  if( core->GetBtlvMode() != BTLV_MODE_BATTLE )
  {
    //バトル以外なのでMainmoduleが無い
    gfl2::Fade::FadeSuper *fadeSys = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade(gfl2::Fade::DISP_CUSTOM_UPPER);
    if( fadeSys )
    {
      AppLib::Fade::DemoFade *demoFade = reinterpret_cast<AppLib::Fade::DemoFade*>(fadeSys);
      demoFade->GetSequence()->SystemSync();
    }
  }
  else
  if(core->GetMainModule()->GetBattleEncountData()->enceff_fld.eff_kind == 0)
  {
    gfl2::Fade::FadeSuper *fadeSys = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->GetFade(gfl2::Fade::DISP_CUSTOM_UPPER);
    if( fadeSys )
    {
      AppLib::Fade::DemoFade *demoFade = reinterpret_cast<AppLib::Fade::DemoFade*>(fadeSys);
      if( demoFade && demoFade->GetSequence() )
      {
        demoFade->GetSequence()->SystemSync();
      }
    }
  }
}

//天候切り替え
BTL_SEQ_FUNC_DEF( SpecialChangeWeather )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int type;
  file->LoadDataInt( &type );

  core->StartWeather(type);
}
//ポケモン読み替え
BTL_SEQ_FUNC_DEF( SpecialChangePokemon )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool isNoLoad;
  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &isNoLoad );

  gfl2::heap::HeapBase* heap = core->GetTempHeap();

  BtlvPos henshinPos = core->SeqComFunc_GetTargetPoke(trgPoke);
  if( isNoLoad )
  {
    BtlvObject_PokeModel *poke = core->GetPokeModel(henshinPos);
    poke->NoLoadFormChange();
  }
  else
  {
    BtlvPos targetPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);

    CreateTask_HenshinPokemon( core->GetTaskSystemAlways(),heap,core,henshinPos,targetPos );
  }
}
//ポケモン溜め表示
BTL_SEQ_FUNC_DEF( SpecialTameVisible )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool visible;
  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &visible );

  BtlvPos pos = core->SeqComFunc_GetTargetPoke(trgPoke);
  core->GetPokeModel(pos)->SetVisibleTame(visible);
}
//ポケモン身代わり表示
BTL_SEQ_FUNC_DEF( SpecialMigawariVisible )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool visible;
  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &visible );

  BtlvPos pos = core->SeqComFunc_GetTargetPoke(trgPoke);
  core->GetPokeModel(pos)->SetVisibleMigawari(visible);
}

//ポケモン身代わり表示(Z技用一時非表示)
BTL_SEQ_FUNC_DEF( SpecialMigawariVisibleZenryoku )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int trgPoke;
  bool visible;
  file->LoadDataInt( &trgPoke );
  file->LoadDataBool( &visible );

  BtlvPos vpos = core->SeqComFunc_GetTargetPoke(trgPoke);
  BtlvObject_PokeModel * poke = core->GetPokeModel(vpos);

  if( visible )
  {
    if( poke->IsHideMigawariZenryoku() )
    {
      poke->SetVisibleMigawari(true);
      poke->SetHideMigawariZenryoku(false);
    }
  }
  else
  {
    if( poke->IsVisibleMigawari() )
    {
      poke->SetVisibleMigawari(false);
      poke->SetHideMigawariZenryoku(true);
    }
  }
}

//連続技定義
BTL_SEQ_FUNC_DEF( SpecialChainAttakDefine )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int startFrame[6];
  int endFrame[6];

  for( int i=0;i<6;i++ )
  {
    file->LoadDataInt( &startFrame[i] );
    file->LoadDataInt( &endFrame[i] );
  }

  BTLV_WAZA_EFF_PARAM* wazaParam = core->GetWazaParam();
  if( startFrame[wazaParam->mTurnType] != 0 )
  {
    file->SkipFrame( startFrame[wazaParam->mTurnType] );
  }
  file->SetMaxFrame( endFrame[wazaParam->mTurnType] );

}

//フィールド技のフィールド生成Pa
BTL_SEQ_FUNC_DEF( SpecialFieldEffectCreate )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  int type;
  file->LoadDataInt( &type );

  core->StartLoadFieldWaza(type);
}


BTL_SEQ_FUNC_DEF( SpecialDeleteCamera )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int envFile,motFile;
  file->LoadDataInt( &envFile );
  file->LoadDataInt( &motFile );

  envFile = core->CheckWazaDataId_Env(envFile);
  motFile = core->CheckWazaDataId_Camera(motFile);

  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,envFile,core->GetEffectSystem(),core->GetParticleIndexManager());
  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,motFile,core->GetEffectSystem(),core->GetParticleIndexManager());
}

BTL_SEQ_FUNC_DEF( SpecialDeleteParticle )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  arcIdx = core->CheckWazaDataId_Effect(arcIdx);
  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,arcIdx,core->GetEffectSystem(),core->GetParticleIndexManager());

}

BTL_SEQ_FUNC_DEF( SpecialDeleteModel )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  arcIdx = core->CheckWazaDataId_Model(arcIdx,0);
  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,arcIdx,core->GetEffectSystem(),core->GetParticleIndexManager());

}

BTL_SEQ_FUNC_DEF( SpecialDeleteModelAnime )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int arcIdx;
  file->LoadDataInt( &arcIdx );

  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,arcIdx,core->GetEffectSystem(),core->GetParticleIndexManager());
}

BTL_SEQ_FUNC_DEF( SpecialDeleteKisekaeAnime )
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  
  int trg,arcIdx;
  file->LoadDataInt( &arcIdx );

  core->GetResourceManager()->DeleteResource(ARCID_WAZAEFF_RESOURCE,arcIdx,core->GetEffectSystem(),core->GetParticleIndexManager());
}

BTL_SEQ_FUNC_DEF(SpecialFesTrainerSetup)
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  bool isDisp;
  file->LoadDataBool(&isDisp);

  if (core->GetBtlvMode() != BTLV_MODE_BATTLE)
  {
    return;
  }

  core->DispFesTrainer(isDisp);
}

BTL_SEQ_FUNC_DEF(MessageDispSFes)
{
  DUMP_COMMAND;
  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  gfl2::heap::HeapBase* heap = core->GetTempHeap();
  /*
  int opt;
  file->LoadDataInt(&opt);

  gfl2::str::MsgData *msgData;
  msgData = GFL_NEW(heap)gfl2::str::MsgData(core->GetResourceManager()->GetDataBuf(print::GetMessageArcId(), GARC_message_jf_phrase_DAT));
  msgData->GetString(0, core->GetWorkStr(0));

  core->GetUiSys()->StartDispMessage(core->GetWorkStr(0));
  */
}


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF(LabelRotom)
{
  DUMP_COMMAND;
}

BTL_SEQ_FUNC_DEF(RotomStartEvent)
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  int type;
  file->LoadDataInt(&type);

  BtlvPos vPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
  BTL_POKEPARAM *bpp = core->GetBattlePokeParam(vPos);

  core->GetUiSys()->Rotom_StartAnimation(type);
  core->GetUiSys()->Rotom_SetData(bpp);
}

BTL_SEQ_FUNC_DEF(RotomStartMessage)
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  int type,opt;
  file->LoadDataInt(&type);
  file->LoadDataInt(&opt);

  int msgId = BTL_STRID_STD_Rotom_01;
  switch (core->GetRotomRank())
  {
  case Savedata::FieldMenu::ROTOM_RANK_1:
    msgId = BTL_STRID_STD_Rotom_01;
    break;
  case Savedata::FieldMenu::ROTOM_RANK_2:
    msgId = BTL_STRID_STD_Rotom_02;
    break;
  case Savedata::FieldMenu::ROTOM_RANK_3:
    msgId = BTL_STRID_STD_Rotom_03;
    break;
  case Savedata::FieldMenu::ROTOM_RANK_4:
    msgId = BTL_STRID_STD_Rotom_04;
    break;
  }

  if (core->GetBtlvMode() != BTLV_MODE_BATTLE)
  {
    core->SeqComFunc_SetEffectMessage_Std(msgId, core->GetWorkStr(0));
    core->GetUiSys()->StartDispMessage(core->GetWorkStr(0), false);
  }
  else
  {
    if (msgId == BTL_STRID_STD_Rotom_04)
    {
      //ニックネームタグのため専用処理
      gfl2::str::StrBuf *msgStr = core->GetWorkStr(1);
      gfl2::str::StrBuf *expStr = core->GetWorkStr(0);

      str::MakeStringStdPlane(core->GetWorkStr(1), msgId);
      core->GetWordSet()->RegisterPlayerNickName(0, core->GetClientPlayerData(0), core->GetGameData()->GetFieldMenu());
      core->GetWordSet()->Expand(expStr, msgStr);
    }
    else
    {
      str::MakeStringStd(core->GetWorkStr(0), msgId, 0);
    }
    core->GetUiSys()->StartDispMessage(core->GetWorkStr(0), false);
  }
}

BTL_SEQ_FUNC_DEF(RotomDelete)
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;

  core->GetUiSys()->Rotom_RemoveLayout();
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
BTL_SEQ_FUNC_DEF( TestLabel )
{
  DUMP_COMMAND;

  //BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
}

BTL_SEQ_FUNC_DEF( TestSpecialCommand )
{
  DUMP_COMMAND;

  BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;

  int type,val1;
  gfl2::math::Vector3 val2;

  file->LoadDataInt( &type );
  file->LoadDataInt( &val1 );
  file->LoadDataVec3( &val2 );

  if( core->GetBtlvMode() != BTLV_MODE_BATTLE )
  {
    return;
  }
  
  switch( type )
  {
  case 1:
    {
      BtlvPos atkPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
      BtlvPos defPos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
      BTL_POKEPARAM *atkBpp = core->GetBattlePokeParam(atkPos);
      BTL_POKEPARAM *defBpp = core->GetBattlePokeParam(defPos);

      BTLV_STRPARAM strParam;
      if( val1 == 0 )
      {
        //必殺のピカチュート
        BTLV_STRPARAM_Setup(&strParam,BTL_STRTYPE_WAZA,ATKMSG_M_658);
        BTLV_STRPARAM_AddArg(&strParam,atkBpp->GetID());  //@仮 PokeID
        BTLV_STRPARAM_AddArg(&strParam,658);
        core->GetUiSys()->StartDispMessage( &strParam , false );
      }
      else if( val1 == 1 )
      {
        //オーラをまとった！
        BTLV_STRPARAM_Setup(&strParam,BTL_STRTYPE_SET,BTL_STRID_SET_AuraEffect_None);
        BTLV_STRPARAM_AddArg(&strParam,atkBpp->GetID());  //@仮 PokeID
        core->GetUiSys()->StartDispMessage( &strParam , false );
      }
      else if( val1 == 2 )
      {
        //野生の○○が現れた
        str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_Encount_Wild1 , 1 , defBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      else if( val1 == 3 )
      {
        //ミニスカートの○○が現れた
        str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_Encount_NPC1 , 1 , core->GetEnemyClientID(0) );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      else if( val1 == 4 )
      {
        //行けっ○○
        str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutSingle , 1 , atkBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
      else if( val1 == 5 )
      {
        //○○は○○を繰り出した
        str::MakeStringStd( core->GetWorkStr(0) , BTL_STRID_STD_PutSingle_NPC , 2 , core->GetEnemyClientID(0),atkBpp->GetID() );
        core->GetUiSys()->StartDispMessage( core->GetWorkStr(0) , false );
      }
    }
    break;
  case 2:
    {
      core->GetUiSys()->HideMessage();
    }
    break;
  case 3:
    {
      BtlvPos pos;
      if( val1 == 0 )
      {
        pos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
      }
      else
      {
        pos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
      }
      BTL_POKEPARAM *bpp = core->GetBattlePokeParam(pos);
      Sound::PlayVoice( 0 , static_cast<MonsNo>(bpp->GetMonsNo()) , bpp->GetFormNo() ,Sound::VOICE_TYPE_BATTLE );
    }
    break;

  case 5://死亡時仮鳴き声
    {
      BtlvPos pos;
      pos = core->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);

      core->PlayDeadVoice3D( pos );
    }
    break;
  case 100: //リソース全消しコマンド
    {
      core->SeqComFunc_CleanAllWazaResource();

      //ついでにライトを全部リセット
      for( int i=0;i<POKE_MODEL_NUM;i++ )
      {
        core->GetPokeModel((BtlvPos)i)->SetUseEffectLight(false);
      }
      for( int i=0;i<TRAINER_MODEL_NUM;i++ )
      {
        if( core->GetTrainerModelObj(i) )
        {
          core->GetTrainerModelObj(i)->SetUseEffectLight(false);
          core->GetTrainerModelObj(i)->SetVisible(true);
          core->ResetTrainer(i);
        }
      }
    }
    break;
  case 101: //モーションフレーム強制指定
    {
      const u32 frame = param.endFrame - param.startFrame;
      const u32 grpNo = param.groupNo;
      int objNum;
      BtlvObject_Gfmdl **objArr = core->GetModelObjManager()->GetObjectArr(grpNo,&objNum);
      for( int i=0;i<objNum;i++)
      {
        BtlvObject_Gfmdl *obj = objArr[i];
        
        obj->GetBaseModel()->SetAnimationFrame(val1);
      }
    }
    break;

  }
}

BTL_SEQ_FUNC_DEF( DummyLabel )
{
  DUMP_COMMAND;

  //BattleViewSystem *core = (btl::BattleViewSystem*)userWork;
  //const u32 frame = param.endFrame - param.startFrame;
  //const u32 grpNo = param.groupNo;
  
}

#undef BTL_SEQ_FUNC_DEF

}//namespace btl
