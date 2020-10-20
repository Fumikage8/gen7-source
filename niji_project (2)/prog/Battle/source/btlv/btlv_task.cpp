//======================================================================
/**
 * @file	btlv_object.cpp
 * @brief	バトル描画 モデル等の規定クラス
 * @author	ariizumi
 * @data	14/11/13
 */
//======================================================================

#include <Math/include/gfl2_TransporterTypes.h>

#include "btlv_core.h"
#include "btlv_camera.h"
#include "btlv_task.h"
#include "btlv_object_poke.h"
#include "btlv_seq_com_define.h"

#include <arc_index/wazaeff_resource.gaix>


GFL_NAMESPACE_BEGIN(btl)

//----------------------------------------------------------------------
//  ポケモン用
//----------------------------------------------------------------------
//ヒットバック
struct TaskWork_PokeHitBack
{
  BattleViewSystem* mCore;

  BtlvObject_PokeModel *mMdl;
  gfl2::math::TransBase<gfl2::math::Vector3> *mTransPos;
  int mCnt;
};

void TaskFunc_PokeHitBack(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_PokeHitBack *work = static_cast<TaskWork_PokeHitBack*>(userWork);

  gfl2::heap::HeapBase *heapMem = work->mCore->GetTempHeap();
  
  const gfl2::math::Vector3 ofs = work->mTransPos->Update();
  work->mMdl->SetOffset(ofs);
  if( work->mTransPos->IsEnd() == true )
  {
    GFL_DELETE work->mTransPos;
    if( work->mCnt == 0)
    {
      static const int HIT_BACK_FRAME = 11;
      gfl2::math::Vector3 end( 0,0,0);
      work->mTransPos = GFL_NEW(heapMem) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(ofs,end,HIT_BACK_FRAME,heapMem);
      work->mCnt++;
    }
    else
    {
      task->GetSystem()->Delete(task);
    }
  }  
}

void CreateTask_PokeHitBack( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvObject_PokeModel* poke )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_PokeHitBack,sizeof(TaskWork_PokeHitBack),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_PokeHitBack *work = static_cast<TaskWork_PokeHitBack*>(task->GetWork());

  work->mCore = btlvCore;

  work->mMdl = poke;
  work->mCnt = 0;
  
  //ポケモンの向きで移動
  gfl2::math::Vector3 ofs( 0,0,-60);
  gfl2::math::Vector3 start( 0,0,0);
  f32 rot = -poke->GetRotateY();
  BtlvRot_Deg( ofs.x,ofs.z,rot );
  ofs = ofs.MulPerElem(poke->GetAdjustScale());
  
  static const int HIT_BACK_FRAME = 3;
  work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(start,ofs,HIT_BACK_FRAME,heap);
 
}

//登場演出
struct TaskWork_PokeIntroMotion
{
  BattleViewSystem* mCore;

  BtlvPos mVpos;
  BtlvObject_PokeModel *mMdl;
  gfl2::math::TransBase<gfl2::math::Vector3> *mTransPos;
  gfl2::math::TransBase<gfl2::math::Vector3> *mTransOfs;
  gfl2::math::TransBase<gfl2::math::Vector3> *mTransScl;
  gfl2::math::TransBase<gfl2::math::Vector4> *mTransCol;
  f32 mCenterOfsY;
  int mCnt;
  int mEffCnt;
  int mStep;

  BtlvObject_Effect *mLandEffect;
};

void TaskFunc_PokeIntroMotion(DemoLib::Obj::Task *task, void *userWork,int step)
{
  enum
  {
    SEQ_SETUP = 0,

    SEQ_LAND_A,
    SEQ_LAND_B,
    SEQ_LAND_C,

    SEQ_LAND,

    SEQ_FINISH,
  };

  TaskWork_PokeIntroMotion *work = static_cast<TaskWork_PokeIntroMotion*>(userWork);

  gfl2::heap::HeapBase *heapMem = work->mCore->GetTempHeap();

  switch( work->mStep )
  {
  case SEQ_SETUP:
    {
      gfl2::math::Vector3 pos;
      int deg;
      work->mCore->GetDefaultPokePos(work->mVpos,&pos,&deg);

      work->mMdl->SetVisible(true);

      //処理分岐
      if( work->mMdl->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND_A) )
      {
        //着地がある
        work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_LAND_A);
        work->mMdl->GetPokeModel()->SetAnimationLoop(false);
        work->mStep = SEQ_LAND_A;
        pos.y += POKE_INTOR_HEIGHT - work->mCenterOfsY;

        work->mStep = SEQ_LAND_A;
      }
      else
      if( work->mMdl->GetPokeModel()->IsAvailableAnimation(PokeTool::MODEL_ANIME_LAND) )
      {
        //着地が無い
        work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_LAND);
        work->mMdl->GetPokeModel()->SetAnimationLoop(false);

        work->mStep = SEQ_LAND;
      }
      else
      {
        //モーションが無い(仮ポケモン
        work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
        work->mMdl->GetPokeModel()->SetAnimationLoop(true);

        work->mStep = SEQ_FINISH;
      }
      work->mMdl->SetPosition(pos);
    }
    break;
    //着地ありのセット
  case SEQ_LAND_A:
    if( work->mMdl->GetPokeModel()->IsAnimationLastFrame() )
    {
      //落下移動の設定
      gfl2::math::Vector3 startPos = work->mMdl->GetPosition();
      gfl2::math::Vector3 endPos;
      int deg;
      work->mCore->GetDefaultPokePos(work->mVpos,&endPos,&deg);
      //モーション変更
      work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_LAND_B);
      work->mMdl->GetPokeModel()->SetAnimationLoop(true);
      work->mTransPos = GFL_NEW(heapMem) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(startPos,endPos,5,heapMem);

      work->mStep = SEQ_LAND_B;
    }
    break;

  case SEQ_LAND_B:
    {
      const gfl2::math::Vector3 pos = work->mTransPos->Update();
      work->mMdl->SetPosition(pos);
      if( work->mTransPos->IsEnd() == true )
      {
        GFL_SAFE_DELETE( work->mTransPos );
        work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_LAND_C);
        work->mMdl->GetPokeModel()->SetAnimationLoop(false);
        work->mStep = SEQ_LAND_C;

        //着地SE
        BTLV_LAND_TYPE seType = work->mCore->CheckLandType(work->mVpos);
        int seId = -1;
        int arcIdx = -1;
        switch(seType)
        {
        case BTLV_LAND_HEAVY:        //200kg以上
          seId = SEQ_SE_BT_TOUJOU_LARGE;
          arcIdx = GARC_wazaeff_resource_ee007_land_smoke_m_lz_BPTCL;
          break;
        case BTLV_LAND_LIGHT_HEAVY:  //100kg以上
          seId = SEQ_SE_BT_TOUJOU_MIDLARGE;
          arcIdx = GARC_wazaeff_resource_ee007_land_smoke_s_lz_BPTCL;
          break;
        case BTLV_LAND_MIDDLE:       //50kg以上
          seId = SEQ_SE_BT_TOUJOU_MID;
          break;
        case BTLV_LAND_LIGHT:        //50kg未満
          seId = SEQ_SE_BT_TOUJOU_SMALL;
          break;
        case BTLV_LAND_NONE:         //ふうせん持ちか着地モーションがない
          break;
        }
        if( seId != -1 )
        {
          int grpNo = BTLV_EFF_SOUND_ACTOR_NUM + work->mVpos;
          gfl2::math::VEC3 sePos(pos.x,pos.y,pos.z);
          //SEのプレイヤーはGLOBALなので、多重再生可能
          //アクターは50個ある。技と被らないように大き目。
          if( work->mCore->IsRecordStartSkip() == false )
          {
            Sound::Play3DActor( grpNo , seId );
            Sound::Set3DActorVolume( grpNo , 1.0f );
            //Sound::Set3DActorPitch( BTLV_EFF_SOUND_ACTOR_NUM + work->mVpos , pitch );
            //Sound::Set3DActorPan( BTLV_EFF_SOUND_ACTOR_NUM + work->mVpos , pan );
            Sound::Set3DActorVelocity( grpNo , gfl2::math::VEC3::Zero() );
            Sound::Set3DActorPos( grpNo , sePos );
          }
        }
        //100 以上は煙が出る
        if( arcIdx != -1 )
        {

          int resIdx = work->mCore->GetResourceManager()->GetEffectResIdx(ARCID_WAZAEFF_RESOURCE,arcIdx);
          GFL_ASSERT_STOP( resIdx==-1 ? 0:1 );

          work->mLandEffect = GFL_NEW(work->mCore->GetTempHeap())BtlvObject_Effect(work->mCore);
          work->mLandEffect->CreateResIdx(work->mCore->GetEffectHeap(),resIdx);
          work->mLandEffect->AddRenderPath(SEQ_DEF_EFF_DRAWTYPE_NORAML);
          work->mLandEffect->SetPosition(pos);
          //work->mLandEffect->SetScale(0.8f,0.8f,0.8f);
          work->mCore->GetObjectManager()->AddObject(work->mLandEffect);
          work->mEffCnt = 0;
        }
      }
    }
    break;

  case SEQ_LAND_C:
  case SEQ_LAND:
    {
      if( work->mMdl->GetPokeModel()->IsAnimationLastFrame() )
      {
        work->mMdl->GetPokeModel()->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
        work->mMdl->GetPokeModel()->SetAnimationLoop(true);
        work->mStep = SEQ_FINISH;
      }
    }
    break;
  case SEQ_FINISH:
    if( work->mTransOfs == NULL && 
        work->mTransScl == NULL && 
        work->mTransCol == NULL && 
        work->mCnt == 100 &&
        work->mLandEffect == NULL )
    {
      task->GetSystem()->Delete(task);
    }

    break;

  }
  //色変え
  if( work->mCnt < 100 )
  {
    work->mCnt++;
    if( work->mCnt > 4 )
    {
      gfl2::math::Vector4 scol(1.0f,1.0f,1.0f,1.0f);
      gfl2::math::Vector4 ecol(1.0f,1.0f,1.0f,0.0f);
      work->mTransCol = GFL_NEW(heapMem) gfl2::math::TransVec4<gfl2::math::TransLiner<f32> >(scol,ecol,12,heapMem);
      work->mCnt = 100;
    }
  }
  if( work->mTransCol )
  {
    const gfl2::math::Vector4 col = work->mTransCol->Update();
    work->mMdl->GetPokeModel()->SetShaderColor(col);
    if( work->mTransCol->IsEnd() == true )
    {
      GFL_SAFE_DELETE( work->mTransCol );
    }
  }

  //中心からの拡縮処理
  if( work->mTransScl )
  {
    const gfl2::math::Vector3 scl = work->mTransScl->Update();
    work->mMdl->SetScale(scl);
    if( work->mTransScl->IsEnd() == true )
    {
      GFL_SAFE_DELETE( work->mTransScl );
    }
  }
  if( work->mTransOfs )
  {
    const gfl2::math::Vector3 ofs = work->mTransOfs->Update();
    work->mMdl->SetOffset(ofs);
    if( work->mTransOfs->IsEnd() == true )
    {
      GFL_SAFE_DELETE( work->mTransOfs );
    }
  }

  //エフェクトの管理
  if( work->mLandEffect )
  {
    work->mEffCnt++;
    if( work->mEffCnt > 30 )
    {
      if( work->mLandEffect->IsAddRenderPath() )
      {
        work->mLandEffect->RemoveRenderPath();
        work->mCore->GetObjectManager()->RemoveObject(work->mLandEffect);
      }
      else
      {
        work->mLandEffect->Delete();
        GFL_SAFE_DELETE(work->mLandEffect);
      }
    }
  }
}

void CreateTask_PokeIntroMotion( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos , BtlvObject_PokeModel* poke )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_PokeIntroMotion,sizeof(TaskWork_PokeIntroMotion),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_PokeIntroMotion *work = static_cast<TaskWork_PokeIntroMotion*>(task->GetWork());

  work->mCore = btlvCore;

  work->mVpos = vpos;
  work->mMdl = poke;
  work->mTransPos = NULL;
  work->mCnt = 0;
  work->mStep = 0;

  //中心から拡縮の処理
  gfl2::math::Vector3 centerPos;
  work->mMdl->GetNodePosition_Sequence(SEQ_DEF_NODE_CENTER,&centerPos);
  centerPos.x = 0;
  centerPos.z = 0;
  work->mCenterOfsY = centerPos.y;
  work->mTransOfs = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(centerPos,gfl2::math::Vector3::GetZero(),7,heap);

  gfl2::math::Vector3 scale(0.01f,0.01f,0.01f);
  gfl2::math::Vector3 scaleEnd(1.0f,1.0f,1.0f);
  work->mTransScl = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(scale,scaleEnd,7,heap);

  //白塗り処理
  gfl2::math::Vector4 col(1.0f,1.0f,1.0f,1.0f);
  work->mMdl->GetPokeModel()->SetShaderColor(col);
}

//ポケモン読み替え
struct TaskWork_ReloadPokemon
{
  BattleViewSystem* mCore;
  BtlvPos mVpos;
  BtlvPos mTargetVpos;
  bool isAura;
  bool isZenryoku;
  bool isHideMigawari;
  int mStep;
};

void TaskFunc_ReloadPokemon(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_ReloadPokemon *work = static_cast<TaskWork_ReloadPokemon*>(userWork);

  BtlvObject_PokeModel *mdl = work->mCore->GetPokeModel(work->mVpos);

  enum
  {
    SEQ_DELETE,
    SEQ_DELETE_WAIT,
    SEQ_LOAD,
    SEQ_LOAD_WAIT,
    SEQ_DRAW_WAIT,
    SEQ_FINISH
  };

  switch( work->mStep )
  {
  case SEQ_DELETE:
    if( mdl->IsEnable() == false )
    {
      //バトルレコーダースキップ時に死亡しているポケモン用
      work->mStep = SEQ_LOAD;
      break;
    }
    work->isAura = mdl->IsEnableFPShader();
    work->isZenryoku = mdl->IsFPZenryoku();
    work->isHideMigawari = mdl->IsHideMigawariZenryoku();
    mdl->SetVisible(false);
    mdl->RemoveRenderPath();
    work->mStep++;
    //break;
  case SEQ_DELETE_WAIT:
    if( mdl->GetPokeModel()->CanDelete() == false )
    {
      break;
    }
    if( work->mCore->GetRenderSys()->GetRenderingPipeLine()->CheckDeleteFPModel() == false )
    {
      break;
    }
    if( mdl->GetMigawariModel() &&
        mdl->GetMigawariModel()->GetBaseModel()->CanDestroy() == false )
    {
      break;
    }
    mdl->Delete();
    work->mStep++;
    //break;
  case SEQ_LOAD:
    mdl->StartLoad_Battle(work->mTargetVpos);
    work->mStep++;
    //break;
  case SEQ_LOAD_WAIT:
    if( mdl->IsFinishLoad() == false )
    {
      break;
    }
    {
      //VRAM転送中にやるとまずいのでここでOFFにする
      work->mCore->SuspendFrameSkip();

      mdl->Create(work->mCore->GetPokeHeap(work->mVpos),work->mCore->GetPokeGLAllocator(work->mVpos),true);
      mdl->AddRenderPath(0);
      mdl->SetVisible(false);
      mdl->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A);
      mdl->SetAnimationIsLoop(true);

      gfl2::math::Vector3 pos;
      int deg;

      work->mCore->GetDefaultPokePos(work->mVpos,&pos,&deg);
      mdl->SetPosition(pos);
      mdl->SetRotateY(deg);
      if( work->isAura )
      {
        mdl->AddFPShader(work->isZenryoku);
      }
      mdl->SetHideMigawariZenryoku(work->isHideMigawari);
    }
    work->mStep++;
    break;
  case SEQ_DRAW_WAIT:
    //VRAM転送待ち。
    work->mStep++;
    break;
  case SEQ_FINISH:
    work->mCore->SetIsLoadPokeModel(false);
    task->GetSystem()->Delete(task);
    //break;
  }
}

void CreateTask_ReloadPokemon( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_ReloadPokemon,sizeof(TaskWork_ReloadPokemon),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_ReloadPokemon *work = static_cast<TaskWork_ReloadPokemon*>(task->GetWork());

  work->mCore = btlvCore;
  work->mVpos = vpos;
  work->mTargetVpos = vpos;
  work->mStep = 0;

  btlvCore->SetIsLoadPokeModel(true);
}

void CreateTask_HenshinPokemon( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvPos vpos , BtlvPos target )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_ReloadPokemon,sizeof(TaskWork_ReloadPokemon),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_ReloadPokemon *work = static_cast<TaskWork_ReloadPokemon*>(task->GetWork());

  work->mCore = btlvCore;
  work->mVpos = vpos;
  work->mTargetVpos = target;
  work->mStep = 0;

  btlvCore->SetIsLoadPokeModel(true);
}

//レアエフェクト
struct TaskWork_PokeRareEffect
{
  BattleViewSystem* mCore;

  int mCnt;
  BtlvObject_Effect *mPtc[2];
  BtlvPos mTarget[2];
};

void TaskFunc_PokeRareEffect(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_PokeRareEffect *work = static_cast<TaskWork_PokeRareEffect*>(userWork);
  BattleViewSystem *core = work->mCore;
  if( core->IsPlayVoice() == true )
  {
    //鳴き声が出ている間は待つ
    return;
  }

  work->mCnt++;

  if( work->mCnt == 1 )
  {
    //背景フェード
    gfl2::math::Vector4 scol( 0,0,0,0 );
    gfl2::math::Vector4 ecol( 0,0,0,0.5f );

    BtlvObject_Gfmdl *fadeMdl = core->GetBgFadeModelObj();
    fadeMdl->SetVisible(true);
    fadeMdl->SetConstantColor(NULL,0,scol);

    gfl2::heap::HeapBase* heap = core->GetTempHeap();
    DemoLib::Obj::CreateTask_ObjectGfMdl_ChangeConstantColor(core->GetTaskSystemAlways(),heap,fadeMdl,"BG_PlaneMT",0,scol,ecol,7);

    //パーティクル生成
    for( int i=0;i<2;i++ )
    {
      if( work->mTarget[i] != BTL_VPOS_ERROR )
      {
        work->mPtc[i] = GFL_NEW(heap)BtlvObject_Effect(core);
        core->GetObjectManager()->AddObject(work->mPtc[i]);

        int resIdx = core->GetResourceManager()->GetEffectResIdx(ARCID_WAZAEFF_RESOURCE,GARC_wazaeff_resource_ee003_star_lz_BPTCL);
        GFL_ASSERT_STOP( resIdx==-1 ? 0:1 );

        work->mPtc[i]->CreateResIdx(core->GetEffectHeap(),resIdx);
        work->mPtc[i]->AddRenderPath(SEQ_DEF_EFF_DRAWTYPE_NORAML);

        DemoLib::Obj::CreateTask_DeleteObject_Effect(core->GetTaskSystemAlways(),heap,work->mPtc[i],33);

        BTLV_RELARIVE_POKE_OPTION opt;
        int trg;

        opt.node = SEQ_DEF_NODE_CENTER;
        opt.offset.Set(0,25.0f,0);
        opt.rate = 100;
        opt.adjustFlip = false;
        opt.adjustScale = false;
        opt.autoRotateModel = true;
        opt.autoRotatePos = true;
        opt.forceUpdate = false;

        opt.vpos = work->mTarget[i];
        opt.vposSub = work->mTarget[0];

        core->SeqComFunc_MoveRelativePoke(work->mPtc[i],0,opt);

        work->mPtc[i]->SetScale(1.55f);
      }
    }

    //3DSE設定
    const gfl2::sound::SoundSystem::Sound3DParam sysParam =
    {
      0,
      1.0f,
      11.30f
    };

    Sound::Set3DParam( sysParam );

    const gfl2::sound::SoundSystem::ListenerParam lisParam =
    {
      200.0f,
      200.0f,
      300.0f,
    };
    Sound::SetListenerParam( lisParam );

    //SE再生１
    {
      int grpNo = 0;
      int seNo = SEQ_SE_W241;
      const f32 pitch = ADJUST_SE_PITCH(30);
      const f32 pan = ADJUST_SE_PAN(0);
      const f32 vol = ADJUST_SE_VOL(64);
      nw::snd::SoundStartable::StartInfo seInfo;
      seInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
      seInfo.playerId = PLAYER_SE_1;

      BtlvObject_Sound *sndObj = core->GetSoundObj(grpNo);

      sndObj->SetSoundId(seNo);
      sndObj->SetIs3DSound(true);

      if( core->IsRecordStartSkip() == false )
      {
        Sound::Play3DActor( grpNo , seNo , &seInfo );
        Sound::Set3DActorVolume( grpNo , vol );
        Sound::Set3DActorPitch( grpNo , pitch );
        Sound::Set3DActorPan( grpNo , pan );
      }

      //追従
      sndObj->SetFollowObject( work->mPtc[0] ,NULL,true);
      sndObj->SetOffset( gfl2::math::Vector3(0,0,0) );
      sndObj->SetTargetGrp(0,SEQ_DEF_FOLLOW_PARTICLE);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(sndObj);
      core->GetObjectManager()->AddObject(sndObj,work->mPtc[0]->GetPriority()+1);
    }
    //SE再生２
    {
      int grpNo = 1;
      int seNo = SEQ_SE_BT_016;
      const f32 pitch = ADJUST_SE_PITCH(50);
      const f32 pan = ADJUST_SE_PAN(0);
      const f32 vol = ADJUST_SE_VOL(110);
      nw::snd::SoundStartable::StartInfo seInfo;
      seInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
      seInfo.playerId = PLAYER_SE_2;

      BtlvObject_Sound *sndObj = core->GetSoundObj(grpNo);

      sndObj->SetSoundId(seNo);
      sndObj->SetIs3DSound(true);

      if( core->IsRecordStartSkip() == false )
      {
        Sound::Play3DActor( grpNo , seNo , &seInfo );
        Sound::Set3DActorVolume( grpNo , vol );
        Sound::Set3DActorPitch( grpNo , pitch );
        Sound::Set3DActorPan( grpNo , pan );
      }

      //追従
      sndObj->SetFollowObject( work->mPtc[0] ,NULL,true);
      sndObj->SetOffset( gfl2::math::Vector3(0,0,0) );
      sndObj->SetTargetGrp(0,SEQ_DEF_FOLLOW_PARTICLE);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(sndObj);
      core->GetObjectManager()->AddObject(sndObj,work->mPtc[0]->GetPriority()+1);
    }
  }
  else
  if( work->mCnt == 7 )
  {
    //SE再生３
    {
      int grpNo = 2;
      int seNo = SEQ_SE_W005;
      const f32 pitch = ADJUST_SE_PITCH(32);
      const f32 pan = ADJUST_SE_PAN(0);
      const f32 vol = ADJUST_SE_VOL(30);
      nw::snd::SoundStartable::StartInfo seInfo;
      seInfo.enableFlag = nw::snd::SoundStartable::StartInfo::ENABLE_PLAYER_ID;
      seInfo.playerId = PLAYER_SE_3;

      BtlvObject_Sound *sndObj = core->GetSoundObj(grpNo);

      sndObj->SetSoundId(seNo);
      sndObj->SetIs3DSound(true);

      if( core->IsRecordStartSkip() == false )
      {
        Sound::Play3DActor( grpNo , seNo , &seInfo );
        Sound::Set3DActorVolume( grpNo , vol );
        Sound::Set3DActorPitch( grpNo , pitch );
        Sound::Set3DActorPan( grpNo , pan );
      }

      //追従
      sndObj->SetFollowObject( work->mPtc[0] ,NULL,true);
      sndObj->SetOffset( gfl2::math::Vector3(0,0,0) );
      sndObj->SetTargetGrp(0,SEQ_DEF_FOLLOW_PARTICLE);

      //優先順位の付け替え(親よりも低いもの)
      core->GetObjectManager()->RemoveObject(sndObj);
      core->GetObjectManager()->AddObject(sndObj,work->mPtc[0]->GetPriority()+1);
    }
  }
  else
  if( work->mCnt == 24 )
  {
    //背景フェード
    gfl2::math::Vector4 scol( 0,0,0,0.5f );
    gfl2::math::Vector4 ecol( 0,0,0,0 );

    BtlvObject_Gfmdl *fadeMdl = core->GetBgFadeModelObj();
    gfl2::heap::HeapBase* heap = core->GetTempHeap();
    DemoLib::Obj::CreateTask_ObjectGfMdl_ChangeConstantColor(core->GetTaskSystemAlways(),heap,fadeMdl,"BG_PlaneMT",0,scol,ecol,10);
  }
  else
  if( work->mCnt == 35 )
  {
    BtlvObject_Gfmdl *fadeMdl = core->GetBgFadeModelObj();
    fadeMdl->SetVisible(false);
  }
  else
  if( work->mCnt >= 38 )
  {
    for( int i=0;i<2;i++ )
    {
      core->GetObjectManager()->RemoveObject(work->mPtc[i]);
      GFL_SAFE_DELETE(work->mPtc[i]);
    }
    core->GetSoundObj(0)->SetFollowObject(NULL);
    core->GetSoundObj(1)->SetFollowObject(NULL);
    core->GetSoundObj(2)->SetFollowObject(NULL);
    task->GetSystem()->Delete(task);
  }
}

void CreateTask_PokeRareEffect( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_PokeRareEffect,sizeof(TaskWork_PokeRareEffect),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_PokeRareEffect *work = static_cast<TaskWork_PokeRareEffect*>(task->GetWork());

  work->mCore = btlvCore;
  work->mCnt = 0;

  int trgNum = 0;
  work->mTarget[0] = BTL_VPOS_ERROR;
  work->mTarget[1] = BTL_VPOS_ERROR;

  BtlvPos pos = btlvCore->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_ATK);
  if( pos != BTL_VPOS_ERROR && btlvCore->GetPokeModel(pos)->IsEnable() )
  {
    if( btlvCore->GetBattlePokeParam(pos)->GetViewSrcData()->IsRare() )
    {
      work->mTarget[trgNum] = pos;
      trgNum++;
    }
#if PM_DEBUG
    else 
    if( btlvCore->GetDebugParam()->mForceRareEffect )
    {
      work->mTarget[trgNum] = pos;
      trgNum++;
    }
#endif
  }
  pos = btlvCore->SeqComFunc_GetTargetPoke(SEQ_DEF_POS_DEF);
  if( pos != BTL_VPOS_ERROR && btlvCore->GetPokeModel(pos)->IsEnable() )
  {
    if( btlvCore->GetBattlePokeParam(pos)->GetViewSrcData()->IsRare() )
    {
      work->mTarget[trgNum] = pos;
      trgNum++;
    }
#if PM_DEBUG
    else 
    if( btlvCore->GetDebugParam()->mForceRareEffect )
    {
      work->mTarget[trgNum] = pos;
      trgNum++;
    }
#endif
  }
  if( trgNum == 0 )
  {
    //即終了
    work->mCnt = 40;
  }
}

//ポケモン振り向き
struct TaskWork_PokeLookAt
{
  BattleViewSystem* mCore;

  int mFrame;
  BtlvObject_PokeModel *mPoke;
  gfl2::math::Vector3 mPos;
};

void TaskFunc_PokeLookAt(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_PokeLookAt *work = static_cast<TaskWork_PokeLookAt*>(userWork);

  work->mFrame--;
  if( work->mFrame < -16 )
  {
    //注視の計算をやめる
    work->mPoke->SetEnableLookAt(false);
    task->GetSystem()->Delete(task);
  }
  else
  if( work->mFrame <= 0 )
  {
    //向くのをやめる
    work->mPoke->SetLookAtPos(NULL);
  }
  else
  {
    work->mPoke->SetLookAtPos(&work->mPos);
  }
}
void CreateTask_PokeLookAt( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , BtlvObject_PokeModel *poke , const gfl2::math::Vector3 &pos , int frame )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_PokeLookAt,sizeof(TaskWork_PokeLookAt),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_PokeLookAt *work = static_cast<TaskWork_PokeLookAt*>(task->GetWork());

  work->mCore = btlvCore;
  work->mFrame = frame;
  work->mPoke = poke;
  work->mPos = pos;

  poke->SetEnableLookAt(true);
}

//----------------------------------------------------------------------
//  Obj用
//----------------------------------------------------------------------
//カメラ追従移動
struct TaskWork_ObjectCameraAutoRotate
{
  BattleViewSystem* mCore;

  int mCnt;
  f32 mLength;
  DemoLib::Obj::Object *mObj;
};

void TaskFunc_ObjectCameraAutoRotate(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_ObjectCameraAutoRotate *work = static_cast<TaskWork_ObjectCameraAutoRotate*>(userWork);

  const gfl2::math::Vector3 ofs = work->mCore->GetCameraSys()->GetPositionAnime() - work->mCore->GetCameraSys()->GetTargetAnime();
  const f32 len =  gfl2::math::Vector2(ofs.x,ofs.z).Length();
    
  gfl2::math::Vector3 pos,rot;
  work->mCore->GetFollowCameraPos(work->mLength,&pos);
  work->mCore->GetFollowCameraRot(&rot);

  work->mObj->SetPosition(pos);
  work->mObj->SetRotate(rot);

  work->mCnt--;
  if( work->mCnt < 0 )
  {
    task->GetSystem()->Delete(task);
  }
}

void CreateTask_ObjectCameraAutoRotate( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , DemoLib::Obj::Object *obj, f32 length , int frame )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_ObjectCameraAutoRotate,sizeof(TaskWork_ObjectCameraAutoRotate),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_ObjectCameraAutoRotate *work = static_cast<TaskWork_ObjectCameraAutoRotate*>(task->GetWork());

  work->mCore = btlvCore;
  work->mCnt = frame;
  work->mLength = length;
  work->mObj = obj;
}


//----------------------------------------------------------------------
//  カメラ用
//----------------------------------------------------------------------
//カメラ移動
struct TaskWork_MoveCamera
{
  BattleViewSystem* mCore;

  gfl2::math::TransBase<gfl2::math::Vector3> *mTransPos;
  gfl2::math::TransBase<gfl2::math::Vector3> *mTransTrg;
  gfl2::math::TransLinerF32 *mTransFov;
  gfl2::math::TransLinerF32 *mTransTwist;
};

void TaskFunc_CameraMove(DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_MoveCamera *work = static_cast<TaskWork_MoveCamera*>(userWork);

  const gfl2::math::Vector3 pos = work->mTransPos->Update();
  const gfl2::math::Vector3 trg = work->mTransTrg->Update();
  const f32 fov = work->mTransFov->Update();
  const f32 twist = work->mTransTwist->Update();

  work->mCore->GetCameraSys()->SetPosition(pos,trg);

  work->mCore->GetCameraSys()->SetFov(fov);
  work->mCore->GetCameraSys()->SetTwist(twist);
  if( work->mTransPos->IsEnd() == true &&
      work->mTransTrg->IsEnd() == true &&
      work->mTransFov->IsEnd() == true &&
      work->mTransTwist->IsEnd() == true )
  {
    task->GetSystem()->Delete(task);
    GFL_DELETE work->mTransPos;
    GFL_DELETE work->mTransTrg;
    GFL_DELETE work->mTransFov;
    GFL_DELETE work->mTransTwist;
  }
}

void CreateTask_MoveCamera( DemoLib::Obj::TaskSystem *taskSys , gfl2::heap::HeapBase *heap, BattleViewSystem *btlvCore , const gfl2::math::Vector3 &pos ,const gfl2::math::Vector3 &trg ,const f32 fov , const f32 twist , const u32 step , DemoLib::Obj::TASK_MOVE_TYPE type )
{
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_CameraMove,sizeof(TaskWork_MoveCamera),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_MoveCamera *work = static_cast<TaskWork_MoveCamera*>(task->GetWork());

  work->mCore = btlvCore;

  gfl2::math::Vector3 sPos = btlvCore->GetCameraSys()->GetPosition();
  gfl2::math::Vector3 sTrg = btlvCore->GetCameraSys()->GetTarget();
  f32 sFov = btlvCore->GetCameraSys()->GetFov();
  f32 eFov;
  if( fov == 0 )
  {
    eFov = sFov;
  }
  else
  {
    eFov = fov;
  }

  switch(type)
  {
  case DemoLib::Obj::MOVE_LINER:
    work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(sPos,pos,step,heap);
    work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransLinerF32 >(sTrg,trg,step,heap);
    break;
  case DemoLib::Obj::MOVE_FIRST_FAST:
    work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveFastF32 >(sPos,pos,step,heap);
    work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveFastF32 >(sTrg,trg,step,heap);
    break;
  case DemoLib::Obj::MOVE_FIRST_SLOW:
    work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSlowF32 >(sPos,pos,step,heap);
    work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSlowF32 >(sTrg,trg,step,heap);
    break;
  case DemoLib::Obj::MOVE_SLOW_FAST_SLOW:
    work->mTransPos = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSFS<f32> >(sPos,pos,step,heap);
    work->mTransTrg = GFL_NEW(heap) gfl2::math::TransVec<gfl2::math::TransCurveSFS<f32> >(sTrg,trg,step,heap);
    break;
  }
  work->mTransFov = GFL_NEW(heap) gfl2::math::TransLinerF32(sFov,eFov,step);

  f32 startTwist = btlvCore->GetCameraSys()->GetTwist();
  if( startTwist - twist > PI  )
  {
    startTwist -= PI2;
  }
  else
    if( startTwist - twist < -PI )
  {
    startTwist += PI2;
  }
  work->mTransTwist = GFL_NEW(heap) gfl2::math::TransLinerF32(startTwist,twist,step);
}

//カメラ揺らし
struct TaskWork_ShakeCamera
{
  BattleViewSystem* mCore;

  gfl2::math::TransSin *mTransSin;
  gfl2::math::TransLinerF32 *mTransRate;
  DemoLib::Obj::TASK_AXIS_TYPE mAxis;
};

void TaskFunc_ShakeCamera( DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_ShakeCamera *work = static_cast<TaskWork_ShakeCamera*>(userWork);

  f32 sin = work->mTransSin->Update();
  const f32 rate = work->mTransRate->Update();

  sin *= rate;
  
  gfl2::math::Vector3 ofs = work->mCore->GetCameraSys()->GetOffset();
  
  switch( work->mAxis )
  {
  case DemoLib::Obj::AXIS_X:
    ofs.x = sin;
    break;
  case DemoLib::Obj::AXIS_Y:
    ofs.y = sin;
    break;
  case DemoLib::Obj::AXIS_Z:
    ofs.z = sin;
    break;
  }
  
  work->mCore->GetCameraSys()->SetOffset(ofs);
  if( work->mTransSin->IsEnd() == true &&
      work->mTransRate->IsEnd() == true )
  {
    GFL_DELETE work->mTransSin;
    GFL_DELETE work->mTransRate;
    task->GetSystem()->Delete(task);
  }
}

void CreateTask_ShakeCamera( 
      DemoLib::Obj::TaskSystem *taskSys , 
      gfl2::heap::HeapBase *heap, 
      BattleViewSystem *btlvCore ,
      f32 startRate , 
      f32 endRate , 
      const f32 dec, 
      const u32 step , 
      DemoLib::Obj::TASK_AXIS_TYPE axis)
{
  //カメラに追従するものがあり、それの対応のためカメラ揺らしを1段階早くする
  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_ShakeCamera,sizeof(TaskWork_ShakeCamera),DemoLib::Obj::TASK_PRI_LOWEST-1);
  TaskWork_ShakeCamera *work = static_cast<TaskWork_ShakeCamera*>(task->GetWork());

  work->mCore = btlvCore;
  work->mTransSin = GFL_NEW(heap) gfl2::math::TransSin(0,dec*360,step,1);
  work->mTransRate = GFL_NEW(heap) gfl2::math::TransLinerF32(startRate,endRate,step);
  work->mAxis = axis;
}


//-------------------------------------------------------------------------------
//カメラ回転タスク(Trg中心)
//-------------------------------------------------------------------------------
struct TaskWork_RotateCamera_CenterTrg
{
  BattleViewSystem* mCore;
  
  gfl2::math::Vector3 mTrg;
  f32 mPosY;
  f32 mLen;
  gfl2::math::TransBaseF32 *mTransAngle;
};

void TaskFunc_RotateCamera_CenterTrg( DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_RotateCamera_CenterTrg *work = static_cast<TaskWork_RotateCamera_CenterTrg*>(userWork);
  
  f32 rot = work->mTransAngle->Update();
  
  gfl2::math::Vector3 pos(0,work->mPosY,work->mLen);
  BtlvRot_Rad( pos.x,pos.z,rot );
  pos.x += work->mTrg.x;
  pos.z += work->mTrg.z;
  
  work->mCore->GetCameraSys()->SetPosition(pos,work->mTrg );

  if( work->mTransAngle->IsEnd() )
  {
    GFL_DELETE work->mTransAngle;
    task->GetSystem()->Delete(task);
  }
}
void CreateTask_RotateCamera_CenterTrg( 
                    DemoLib::Obj::TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap, 
                    BattleViewSystem *btlvCore ,
                    const gfl2::math::Vector3 &pos , 
                    const gfl2::math::Vector3 &trg , 
                    f32 rotRad , 
                    int opt , 
                    int frame  )
{
  gfl2::math::Vector3 ofs;
  ofs = pos-trg;
  f32 rotY = -gfl2::math::Atan2Rad(ofs.x,ofs.z);

  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_RotateCamera_CenterTrg,sizeof(TaskWork_RotateCamera_CenterTrg),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_RotateCamera_CenterTrg *work = static_cast<TaskWork_RotateCamera_CenterTrg*>(task->GetWork());

  work->mCore = btlvCore;

  work->mTrg = trg;
  work->mPosY = pos.y;
  work->mLen = gfl2::math::Vector2(ofs.x,ofs.z).Length();
  switch(opt)
  {
  case DemoLib::Obj::MOVE_LINER:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransLinerF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_FIRST_FAST:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveFastF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_FIRST_SLOW:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveSlowF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_SLOW_FAST_SLOW:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveSFSF32(rotY,rotY+rotRad,frame);
    break;
  }
}

//-------------------------------------------------------------------------------
//カメラ回転タスク(Pos中心)
//-------------------------------------------------------------------------------
struct TaskWork_RotateCamera_CenterPos
{
  BattleViewSystem* mCore;
  
  gfl2::math::Vector3 mPos;
  f32 mTrgY;
  f32 mLen;
  gfl2::math::TransBaseF32 *mTransAngle;
};

void TaskFunc_RotateCamera_CenterPos( DemoLib::Obj::Task *task, void *userWork,int step)
{
  TaskWork_RotateCamera_CenterPos *work = static_cast<TaskWork_RotateCamera_CenterPos*>(userWork);
  
  f32 rot = work->mTransAngle->Update();
  
  gfl2::math::Vector3 trg(0,work->mTrgY,work->mLen);
  BtlvRot_Rad( trg.x,trg.z,rot );
  trg.x += work->mPos.x;
  trg.z += work->mPos.z;
  
  work->mCore->GetCameraSys()->SetPosition(work->mPos,trg );

  if( work->mTransAngle->IsEnd() )
  {
    GFL_DELETE work->mTransAngle;
    task->GetSystem()->Delete(task);
  }
}
void CreateTask_RotateCamera_CenterPos( 
                    DemoLib::Obj::TaskSystem *taskSys , 
                    gfl2::heap::HeapBase *heap, 
                    BattleViewSystem *btlvCore ,
                    const gfl2::math::Vector3 &pos , 
                    const gfl2::math::Vector3 &trg , 
                    f32 rotRad , 
                    int opt , 
                    int frame  )
{
  gfl2::math::Vector3 ofs;
  ofs = trg-pos;
  f32 rotY = -gfl2::math::Atan2Rad(ofs.x,ofs.z);

  DemoLib::Obj::Task *task = taskSys->Create(TaskFunc_RotateCamera_CenterPos,sizeof(TaskWork_RotateCamera_CenterPos),DemoLib::Obj::TASK_PRI_LOWEST);
  TaskWork_RotateCamera_CenterPos *work = static_cast<TaskWork_RotateCamera_CenterPos*>(task->GetWork());

  work->mCore = btlvCore;

  work->mPos = pos;
  work->mTrgY = trg.y;
  work->mLen = gfl2::math::Vector2(ofs.x,ofs.z).Length();
  switch(opt)
  {
  case DemoLib::Obj::MOVE_LINER:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransLinerF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_FIRST_FAST:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveFastF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_FIRST_SLOW:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveSlowF32(rotY,rotY+rotRad,frame);
    break;
  case DemoLib::Obj::MOVE_SLOW_FAST_SLOW:
    work->mTransAngle = GFL_NEW(heap) gfl2::math::TransCurveSFSF32(rotY,rotY+rotRad,frame);
    break;
  }
}

GFL_NAMESPACE_END(btl)

