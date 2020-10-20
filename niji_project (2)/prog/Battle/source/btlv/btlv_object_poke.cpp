//======================================================================
/**
 * @file	btlv_object_poke.cpp
 * @brief	バトル描画 ポケモンモデル
 * @author	ariizumi
 * @data	15/06/09
 */
//======================================================================

#include "System/include/GflUse.h"
#include "AppLib/include/Tool/app_tool_Gauge.h" //瀕死時にモーション速度下げる判定のため

#include "btlv_core.h"
#include "btlv_camera.h"
#include "btlv_object_poke.h"
#include "btlv_seq_com_define.h"

#include <niji_conv_header/battle/chara/chara_model_id.h>

GFL_NAMESPACE_BEGIN(btl)

//------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------
BtlvObject_PokeModel::BtlvObject_PokeModel(BattleViewSystem *btlvCore,DemoLib::Obj::ObjectRenderPipeLine *pipe,BtlvPos pos,int idx)
  :Object_Poke(pipe)
  ,mBtlvCore(btlvCore)
  ,mIdx(idx)
  ,mLightNo(idx+LIGHTNO_POKEMON_TOP)
{
  //ここで作って、ObjectManagerに登録しっぱなしにする
  mMigawariModel = GFL_NEW(mBtlvCore->GetPokeHeap(mIdx))DemoLib::Obj::Object_Chara(mBtlvCore->GetRenderSys()->GetRenderingPipeLine(),mBtlvCore->GetCharaModelFactory(),mBtlvCore->GetDressUpModelResourceManager());

}
//デストラクタ
BtlvObject_PokeModel::~BtlvObject_PokeModel()
{
  GFL_SAFE_DELETE(mMigawariModel);
}

void BtlvObject_PokeModel::StartLoad_Battle(void)
{
  StartLoad_Battle(BtlvPos(mIdx));
}

void BtlvObject_PokeModel::StartLoad_Battle(BtlvPos target)
{
  BTL_POKEPARAM *bpp = mBtlvCore->GetBattlePokeParam(target);
  const pml::pokepara::PokemonParam *pp = bpp->GetViewSrcData();

  PokeTool::PokeModel::SetupOption opt;
  opt.dataHeap = mBtlvCore->GetPokeHeap(mIdx);
  //opt.workHeap = mBtlvCore->GetTempHeap()->GetLowerHandle();
  opt.workHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_FILEREAD);
  opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
  opt.useShadow = true;

  PokeTool::SimpleParam pokeParam;
  PokeTool::GetSimpleParam(&pokeParam,pp);

  DemoLib::Obj::Object_Poke::LOAD_PARAM param;
  param.modelSys = mBtlvCore->GetPokeModelSys();
  param.param = &pokeParam;
  param.opt = &opt;
  param.dataHeap = mBtlvCore->GetPokeHeap(mIdx);
  param.workHeap = mBtlvCore->GetTempHeap()->GetLowerHandle();
  DemoLib::Obj::Object_Poke::StartLoad(mBtlvCore->GetAsyncFileManager(),param);

  mMonsNo = pokeParam.monsNo;
  mFormNo = pokeParam.formNo;
  // @fix MMCat[359] 通常色のメタモンが色違いの「ズガドーン/ビーストH」に「へんしん」した状態で「ビックリヘッド/はなびせんよう」を使用すると技演出が通常色のものになる
  mIsRare = pokeParam.isRare;

  mFpShaderCnt = 0;
  mIsHitBack = false;

  mIsPlayPinchSe = false;
  
  mSize = PokeTool::POKE_SIZE_S;

  //bppのほうは変身対象のポケモンが入っている場合がある。その場合状態異常までコピーしてしまう。
  BTL_POKEPARAM *selfBpp = mBtlvCore->GetBattlePokeParam((BtlvPos)mIdx);
  SetSick( selfBpp->GetPokeSick() );
  mPokeInfoData = mBtlvCore->getPokeExtraInfo(mMonsNo,mFormNo);

}

//ライト番号設定のため継承
void BtlvObject_PokeModel::Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader)
{
  DemoLib::Obj::Object_Poke::Create(heap,allocator,useShader);

  //@attencion ライト番号はMoveメンバーで変わっても固定！
  GetPokeModel()->SetLightSetNo(mLightNo);
  GetPokeModel()->SetAutoBlink(true);
  {
    int matNum = GetPokeModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      GetPokeModel()->SetMaterialLightSetIndex(i,mLightNo);
    }
    GetPokeModel()->SetStencilReferenceOffset(mIdx);
  }
  if( GetPokeModel()->GetShadowModel() )
  {
    int matNum = GetPokeModel()->GetShadowModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      GetPokeModel()->GetShadowModel()->SetMaterialLightSetIndex(i,mLightNo);
      //影の描画順更新
#if defined(GF_PLATFORM_CTR)
      GetPokeModel()->GetShadowModel()->GetModelInstanceNode()->CopyMaterialDepthStencilStateToStateObject();
#endif
      u32 testFunc = GetPokeModel()->GetShadowModel()->GetMaterialDepthTestFunction( i );
      if ( testFunc != poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_NEVER )
      {//非表示設定でなければ
        GetPokeModel()->GetShadowModel()->SetMaterialDepthTestFunction( i, poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_ALWAYS );
      }
      GetPokeModel()->GetShadowModel()->GetModelInstanceNode()->GetMaterialInstanceNode(i)->GetDepthStencilStateObject()->UpdateState();
      GetPokeModel()->GetShadowModel()->GetModelInstanceNode()->SetUpDrawTag();
    }
  }
  ChangeAnimationDirect(0,true);

  mSize = GetPokeModel()->GetPokeSize();

  //みがわりモデル作成
  {
    DemoLib::Obj::Object_Chara::LOAD_PARAM param;
    param.charaId = CHARA_MODEL_ID_BTOB0301_00;
    param.dressParam = NULL;
    mMigawariModel->CreateNoLoad( param , heap , allocator );
    mMigawariModel->SetVisible(false);

    int matNum = mMigawariModel->GetBaseModel()->GetMaterialCount();
    for( int i=0;i<matNum;i++ )
    {
      mMigawariModel->GetBaseModel()->SetMaterialLightSetIndex(i,mLightNo);
    }

    //身代わりのサイズ補正
    switch( mSize )
    {
    case PokeTool::POKE_SIZE_S:
      mMigawariModel->SetAdjustScale(1.0f,false);
      break;
    case PokeTool::POKE_SIZE_M:
    default:
      mMigawariModel->SetAdjustScale(1.5f,false);
      break;
    case PokeTool::POKE_SIZE_L:
      mMigawariModel->SetAdjustScale(2.0f,false);
      break;
    }
  }

  //ディグダ岩作成
  //対象はディグダ、ダグトリオ、メガゲンガー
  //追加でスナバァ、シロデスナ
  if( mMonsNo == MONSNO_DHIGUDA ||
      mMonsNo == MONSNO_DAGUTORIO ||
      mMonsNo == MONSNO_SUNABA1 ||
      mMonsNo == MONSNO_SUNABA2 ||
    (mMonsNo == MONSNO_GENGAA && mFormNo == 1 ) )
  {
    mDigudaStoneModel = GFL_NEW(heap)BtlvObject_Gfmdl(mBtlvCore,mBtlvCore->GetRenderSys()->GetRenderingPipeLine());
    mBtlvCore->GetObjectManager()->AddObject(mDigudaStoneModel,DemoLib::Obj::OBJ_DEFAULT_PRIORITY+16);
    mDigudaStoneModel->CreateBuffer(mBtlvCore->GetDigudaStoneResouce(),heap,allocator);
    switch(mMonsNo)
    {
    case MONSNO_DHIGUDA:
      mDigudaStoneScaleWidth = 0.16f;
      mDigudaStoneScaleHeight = 0.13f;
      break;
    case MONSNO_DAGUTORIO:
      mDigudaStoneScaleWidth = 0.3f;
      mDigudaStoneScaleHeight = 0.25f;
      break;
    case MONSNO_GENGAA:
      mDigudaStoneScaleWidth = 1.0f;
      mDigudaStoneScaleHeight = 1.0f;
      break;
    case MONSNO_SUNABA1:
      mDigudaStoneScaleWidth = 0.25f;
      mDigudaStoneScaleHeight = 0.2f;
      break;
    case MONSNO_SUNABA2:
      mDigudaStoneScaleWidth = 0.4f;
      mDigudaStoneScaleHeight = 0.33f;
      break;
    }
    mDigudaStoneModel->SetVisible(false);
#ifdef DEBUG_ONLY_FOR_ariizumi_nobuhiko
    mDhigudaStoneDummy = NULL;
#endif //DEBUG_ONLY_FOR_ariizumi_nobuhiko
  }
  else
  {
    mDigudaStoneModel = NULL;
#ifdef DEBUG_ONLY_FOR_ariizumi_nobuhiko
    //ディグダ岩を確保したつもり
    mDhigudaStoneDummy = GflHeapAllocMemory(heap,0x1C00);
#endif //DEBUG_ONLY_FOR_ariizumi_nobuhiko
  }

  mIsVisiblePoke = true;
  mIsVisibleShadow = true;
  mIsVisibleTame = true;
  mIsVisibleMigawari = false;
  mIsVisibleDigudaStone = false;
  mIsHideMigawariZenryoku = false;
  mOverrideEyeType = -1;
  UpdateVisible();
}

void BtlvObject_PokeModel::Delete(void)
{
#ifdef DEBUG_ONLY_FOR_ariizumi_nobuhiko
  //ディグダ岩を確保したつもり
  GflHeapSafeFreeMemory( mDhigudaStoneDummy );
#endif //DEBUG_ONLY_FOR_ariizumi_nobuhiko

  if( mDigudaStoneModel )
  {
    mBtlvCore->GetObjectManager()->RemoveObject(mDigudaStoneModel);
    mDigudaStoneModel->Delete();
    GFL_SAFE_DELETE(mDigudaStoneModel);
  }
  if( mMigawariModel )
  {
    mMigawariModel->Delete();
  }
  DemoLib::Obj::Object_Poke::Delete();

}

//IsUpdateが立っていたら来る関数。
//IsEnableはチェック済みなので座標更新はこちらで。
void BtlvObject_PokeModel::UpdateCore(int step)
{
  if( mIsVisibleMigawari == false &&
      mDigudaStoneModel != NULL &&
      mEnableFloat  == false )
  {
    //浮き禁止モデル
    gfl2::math::Vector3 pos = GetPosition();
    pos.y = 0;
    SetPosition(pos);
    gfl2::math::Vector3 ofs = GetOffset();
    ofs.y = 0;
    SetOffset(ofs);

    gfl2::math::Vector3 rot = GetRotate();
    rot.x = 0;
    rot.z = 0;
    SetRotate(rot);
  }
  //メガゲンガーは浮き状態でも回転は抑制する(胴体の赤い模様のシェーダーの仕様
  if(mMonsNo == MONSNO_GENGAA && mFormNo == 1 )
  {
    gfl2::math::Vector3 rot = GetRotate();
    rot.x = 0;
    rot.z = 0;
    SetRotate(rot);
  }

  if( mMigawariModel )
  {
    gfl2::math::Vector3 pos;
    gfl2::math::Vector3 scale;
    gfl2::math::Vector3 rot;
    GetCalcPosition(&pos);
    GetCalcRotate(&rot);
    //GetCalcScale(&scale); //scaleはAdjustが入るので元の値。
    scale = GetScale();

    mMigawariModel->SetPosition(pos);
    mMigawariModel->SetRotate(rot);
    mMigawariModel->SetScale(scale);
    if( mDigudaStoneModel )
    {
      scale.x *= mDigudaStoneScaleWidth;
      scale.y *= mDigudaStoneScaleHeight;
      scale.z *= mDigudaStoneScaleWidth;
      mDigudaStoneModel->SetPosition(pos);
      mDigudaStoneModel->SetRotate(rot);
      mDigudaStoneModel->SetScale(scale);
    }
  }
  DemoLib::Obj::Object_Poke::UpdateCore(step);
}

//毎フレーム呼ばれる
void BtlvObject_PokeModel::UpdateAlways(int step)
{
  DemoLib::Obj::Object_Poke::UpdateAlways();
  UpdateVisible();
}

//ObjectRenderPipeLine経由で追加する
//optにライト番号渡したいので追加
void BtlvObject_PokeModel::AddRenderPath(int opt)
{
  //optをこっちで指定するのでデフォルト以外は困るのでチェック
  GFL_ASSERT(opt==0);
  DemoLib::Obj::Object_Poke::AddRenderPath(mLightNo);
  if( mMigawariModel )
  {
    mMigawariModel->AddRenderPath(100+mLightNo);
  }
  if( mDigudaStoneModel )
  {
    mDigudaStoneModel->AddRenderPath(SEQ_DEF_DRAWTYPE_BEFORE_EDGE);
  }
  //ラスボスは常にオーラ状態
  if( mBtlvCore->IsBeastBattle() && mIdx == 1 )
  {
    AddFPShader(false);
  }
}

//全力シェーダーのフォロー用に追加
void BtlvObject_PokeModel::RemoveRenderPath(void)
{
  if( mFpShaderCnt > 0 )
  {
    mBtlvCore->GetRenderSys()->GetRenderingPipeLine()->RemovePoke_FullPowerShader( this );
    mFpShaderCnt = 0;
  }
  DemoLib::Obj::Object_Poke::RemoveRenderPath();
  if( mMigawariModel )
  {
    mMigawariModel->RemoveRenderPath();
  }
  if( mDigudaStoneModel )
  {
    mDigudaStoneModel->RemoveRenderPath();
  }
  //ラスボスは常にオーラ状態
  if( mBtlvCore->IsBeastBattle() && mIdx == 1 )
  {
    RemoveFpShader();
  }
}

void BtlvObject_PokeModel::ChangeAnimationDirect( int type , bool forceReset )
{
  if( IsEnable() == false )
  {
    return;
  }
  if( type == PokeTool::MODEL_ANIME_BT_WAIT_A )
  {
    StartWaitAnime(forceReset,false);
  }
  else
  {
    DemoLib::Obj::Object_Poke::ChangeAnimationDirect(type,forceReset);
    UpdateSickMotion();
  }
  UpdateFaceAnime();
}

//フェイシャル更新
void BtlvObject_PokeModel::UpdateFaceAnime(void)
{
  if( IsEnable() == false )
  {
    return;
  }
  bool defaultEye = true;
  if( GetSick() == pml::pokepara::SICK_NEMURI )
  {
    GetPokeModel()->SetFaceAnime_Eye(PokeTool::EYETYPE_CLOSE);
    defaultEye = false;
  }
  else
  if( mOverrideEyeType != -1 )
  {
    //シーケンスからの固定
    GetPokeModel()->SetFaceAnime_Eye(mOverrideEyeType);
    defaultEye = false;
  }
  else
  if (mBtlvCore->IsBeastBattle() && mIdx == 1)
  {
    //ビースト戦での目固定
    //ダウン以外は怒り目
    if (GetAnimationType() != PokeTool::MODEL_ANIME_DOWN)
    {
      GetPokeModel()->SetFaceAnime_Eye(PokeTool::EYETYPE_ANGRY);
      defaultEye = false;
    }
  }
  else
  if (mBtlvCore->GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NUSI) && mIdx == 1 && mMonsNo == MONSNO_NIZITYUU)
  {
    //ヌシ戦のトゲデマルは怒り目 怒り口
    if (GetAnimationType() != PokeTool::MODEL_ANIME_DOWN)
    {
      GetPokeModel()->SetFaceAnime_Eye(PokeTool::EYETYPE_ANGRY);
      GetPokeModel()->SetFaceAnime_Mouth(PokeTool::MOUTHTYPE_CLOSE3);
      defaultEye = false;
    }
  }

  if (defaultEye)
  {
    GetPokeModel()->SetAutoBlink(true);
    GetPokeModel()->ResetFaceAnime_Eye();
    if (mBtlvCore->GetSetupStatusFlag(BTL_STATUS_FLAG_VS_NUSI) && mIdx == 1 && mMonsNo == MONSNO_NIZITYUU)
    {
      GetPokeModel()->ResetFaceAnime_Mouth();
    }
  }
  else
  {
    GetPokeModel()->SetAutoBlink(false);
  }
}

//ウェイとアニメはランダム開始とWaitB対応を入れるため分ける
void BtlvObject_PokeModel::StartWaitAnime( bool isForce , bool inRand )
{
  PokeTool::MODEL_ANIME anime = GetAnimationType();
  if( anime != PokeTool::MODEL_ANIME_BT_WAIT_A || isForce == true )
  {
    DemoLib::Obj::Object_Poke::ChangeAnimationDirect(PokeTool::MODEL_ANIME_BT_WAIT_A,isForce);

    if( inRand )
    {
      const f32 maxFrame = GetAnimationMaxFrame();
      if( maxFrame != 0 )
      {
        const s32 frame = System::GflUse::GetPublicRand((u32)maxFrame);
        SetAnimationFrame((f32)frame);
      }
    }
    else
    {
      SetAnimationFrame(0);
    }
  }
  if( mEnableWiatB == true )
  {
    SetAnimationIsLoop(false);
  }
  else
  {
    SetAnimationIsLoop(true);
  }
  UpdateSickMotion();
}

//待機Bの許可
void BtlvObject_PokeModel::SetEnableWaitB(bool flg)
{
  if( mEnableWiatB != flg )
  {
    if( IsEnable() )
    {
      if( flg == true )
      {
        BTL_POKEPARAM* bpp = mBtlvCore->GetBattlePokeParam((BtlvPos)mIdx);
        ::app::tool::Gauge::GaugeColor gaugeCol = ::app::tool::Gauge::GetDottoColor( (f32)bpp->GetValue(BTL_POKEPARAM::BPP_HP) , (f32)bpp->GetValue(BTL_POKEPARAM::BPP_MAX_HP) );

        GFL_ASSERT_MSG( IsAvailableAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A)==false ||
                        GetAnimationType() == PokeTool::MODEL_ANIME_BT_WAIT_A,
                        "PokeModel Motion is not WaitA[%d][%d]\n",mIdx,GetAnimationType());

        if( mSick == pml::pokepara::SICK_NULL &&
          gaugeCol != ::app::tool::Gauge::GAUGE_COLOR_RED &&
          gaugeCol != ::app::tool::Gauge::GAUGE_COLOR_NULL )
        {
          mEnableWiatB = true;
          SetAnimationIsLoop(false);
        }
        else
        {
          SetAnimationIsLoop(true);
        }
      }
      else
      {
        mEnableWiatB = false;
      }
    }
  }
}

void BtlvObject_PokeModel::GetNodePosition_Sequence( int node , gfl2::math::Vector3 *pos )
{
  if( mIsVisibleMigawari == true )
  {
    GetNodePosition_Migawari(node,pos);
    return;
  }

  bool ret = false;
  PokeTool::MODEL_NODE tempNode = PokeTool::MODEL_NODE_FRONT;
  switch( node )
  {
  case SEQ_DEF_NODE_ORIGIN:
    *pos = GetPosition();
    ret = true;
    //nofunc
    break;
  case SEQ_DEF_NODE_BUTURI:
  case SEQ_DEF_NODE_SHOOT:
    if( GetAnimationType() >= PokeTool::MODEL_ANIME_BUTURI1 &&
        GetAnimationType() <= PokeTool::MODEL_ANIME_TOKUSHU4 )
    {
      int ofs = GetAnimationType() - PokeTool::MODEL_ANIME_BUTURI1;
      ret = GetNodePosition( static_cast<PokeTool::MODEL_NODE>(PokeTool::MODEL_NODE_ATTACK01+ofs),0,pos);
    }
    else
    {
      if( node == SEQ_DEF_NODE_BUTURI )
      {
        ret = GetNodePosition(PokeTool::MODEL_NODE_ATTACK01,0,pos);
      }
      else
      {
        ret = GetNodePosition(PokeTool::MODEL_NODE_SHOOT01,0,pos);
      }
    }
    break;
  case SEQ_DEF_NODE_HEAD:
    ret = GetNodePosition(PokeTool::MODEL_NODE_OVERHEAD,0,pos);
    break;
  case SEQ_DEF_NODE_FRONT: 
    ret = GetNodePosition(PokeTool::MODEL_NODE_FRONT,0,pos);
    break;
  case SEQ_DEF_NODE_FACE:
    ret = GetNodePosition(PokeTool::MODEL_NODE_HEAD,0,pos);
    break;
  case SEQ_DEF_NODE_EYE:
    {
      gfl2::math::Vector3 eye1(0,0,0);
      gfl2::math::Vector3 eye2(0,0,0);
      const bool ret1 = GetNodePosition( PokeTool::MODEL_NODE_EYE , 0 , &eye1 );
      const bool ret2 = GetNodePosition( PokeTool::MODEL_NODE_EYE , 1 , &eye2 );
      if( ret2 == true )
      {
        pos->x = (eye1.x+eye2.x)*0.5f;
        pos->y = (eye1.y+eye2.y)*0.5f;
        pos->z = (eye1.z+eye2.z)*0.5f;
        ret = (ret1||ret2);
      }
      else
      {
        *pos = eye1;
        ret = ret1;
      }
      tempNode = PokeTool::MODEL_NODE_HEAD;
    }
    break;
  case SEQ_DEF_NODE_MOUTH: 
    ret = GetNodePosition(PokeTool::MODEL_NODE_MOUTH,0,pos);
    tempNode = PokeTool::MODEL_NODE_HEAD;
    break;
  case SEQ_DEF_NODE_HORN:
    ret = GetNodePosition(PokeTool::MODEL_NODE_HORN,0,pos);
    tempNode = PokeTool::MODEL_NODE_HEAD;
    break;
  case SEQ_DEF_NODE_TAIL:
    ret = GetNodePosition(PokeTool::MODEL_NODE_TAIL,0,pos);
    break;
  case SEQ_DEF_NODE_HAND1: 
    ret = GetNodePosition(PokeTool::MODEL_NODE_HAND,0,pos);
    break;
  case SEQ_DEF_NODE_HAND2: 
    ret = GetNodePosition(PokeTool::MODEL_NODE_HAND,1,pos);
    break;
  case SEQ_DEF_NODE_FOOT1: 
    ret = GetNodePosition(PokeTool::MODEL_NODE_FOOT,0,pos);
    break;
  case SEQ_DEF_NODE_FOOT2:
    ret = GetNodePosition(PokeTool::MODEL_NODE_FOOT,1,pos);
    break;
  case SEQ_DEF_NODE_CENTER:
    ret = GetNodePosition(PokeTool::MODEL_NODE_CENTER,0,pos);
    break;
  }
  if( ret == false )
  {
    //なかった時分岐
    const bool ret3 = GetNodePosition(tempNode,0,pos);
    if( ret3 == false )
    {
      *pos = GetPosition();
      pos->y += 20.0f;
    }
  }
}

//身代わり専用の座標計算処理
void BtlvObject_PokeModel::GetNodePosition_Migawari(int node , gfl2::math::Vector3 *ret )
{
  switch( node )
  {
  case SEQ_DEF_NODE_ORIGIN:
    ret->Set(0,0,0);
    break;
  case SEQ_DEF_NODE_BUTURI:
  case SEQ_DEF_NODE_SHOOT:
  case SEQ_DEF_NODE_FRONT:
    //正面
    ret->Set(0,21,27);
    break;
  case SEQ_DEF_NODE_HEAD:
  case SEQ_DEF_NODE_HORN:
    ret->Set(0,58,0);
    break;
  case SEQ_DEF_NODE_FACE:
    ret->Set(0,42,12);
    break;
  case SEQ_DEF_NODE_EYE:
    //目、中心の位置
    ret->Set(0,48,25);
    break;
  case SEQ_DEF_NODE_MOUTH:
    ret->Set(0,40,30);
    break;
  case SEQ_DEF_NODE_TAIL:
    ret->Set(0,8,-44);
    break;
  case SEQ_DEF_NODE_HAND1://右
    ret->Set(-13,27,21);
    break;
  case SEQ_DEF_NODE_HAND2://左
    ret->Set( 13,27,21);
    break;
  case SEQ_DEF_NODE_FOOT1:
    ret->Set(-14,7,27);
    break;
  case SEQ_DEF_NODE_FOOT2:
    ret->Set( 14,7,27);
    break;
  case SEQ_DEF_NODE_WAIST:
  case SEQ_DEF_NODE_CENTER:
  default:
    ret->Set(0,29,0);
    break;
  }

  *ret = ret->MulPerElem(mMigawariModel->GetAdjustScale());

  //回転
  {
    gfl2::math::Vector3 rot = GetRotate();
    BtlvRot_Deg(ret->z,ret->y,-rot.x);
    BtlvRot_Deg(ret->x,ret->z,-rot.y);
    BtlvRot_Deg(ret->x,ret->y,rot.z);
  }

  *ret += GetPosition();
}


//SequenceEditorからの値をMODEL_NODEに変換
void BtlvObject_PokeModel::GetFollowNode( int seqNode , PokeTool::MODEL_NODE *retNode , int *retNodeIdx )
{
  //ノード無かった時用
  PokeTool::MODEL_NODE node = PokeTool::MODEL_NODE_FRONT;
  int nodeIdx = 0;

  switch( seqNode )
  {
  case SEQ_DEF_NODE_ORIGIN:
    //外でブロックして、obj自体に追従させる
    GFL_PRINT("非対応\n");
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

void BtlvObject_PokeModel::UpdatePositionForce(void)
{
  //ノード座標取得の更新のために、座標を強制更新する
  f32 spd = GetPokeModel()->GetAnimationStepFrame();
  GetPokeModel()->SetAnimationStepFrame(0);
  UpdateCore(0);
  UpdateAlwaysAfterCore(0);
  GetPokeModel()->SetAnimationStepFrame(spd);
}

//バトルのモデルなどにポケモンのサイズをかける
f32 BtlvObject_PokeModel::GetCamAdjustHeight(void)
{
  f32 height = GetPokeModel()->GetAdjustHeight();
  if( mIsVisibleMigawari == true )
  {
    height = GetMigawariAdjustHeight();
  }

  f32 diff = height - 1.0f;
  f32 rate = 1.0f + (diff/2);

  if( mPokeInfoData->mAdjustCamera != 0 )
  {
    rate *= mPokeInfoData->mAdjustCamera;
  }
  return rate;
}
f32 BtlvObject_PokeModel::GetAdjustHeight(void)
{
  if( mIsVisibleMigawari == true )
  {
    return GetMigawariAdjustHeight();
  }

  if( mPokeInfoData->mAdjustCamera != 0 )
  {
    return GetPokeModel()->GetAdjustHeight() * mPokeInfoData->mAdjustScale;
  }
  else
  {
    return GetPokeModel()->GetAdjustHeight();
  }
}

//みがわり用の補正倍率取得
f32 BtlvObject_PokeModel::GetMigawariAdjustHeight(void)
{
  //身代わりのサイズ補正
  switch( mSize )
  {
  case PokeTool::POKE_SIZE_S:
    return 0.65f; //ピカチュウ相当
  case PokeTool::POKE_SIZE_M:
  default:
    return 1.1f; //ドーブル相当
  case PokeTool::POKE_SIZE_L:
    return 1.4f; //マフォクシー相当
  }
}

bool BtlvObject_PokeModel::GetCollision(gfl2::math::AABB *box)
{
  if( GetPokeModel() )
  {
    bool ret = GetPokeModel()->GetMotionCollisionDirect(box);
    if (ret == true && 
        mMonsNo == MONSNO_PURIZUMU && 
        mFormNo == FORMNO_PURIZUMU_FORM_N)
    {
      //momiji例外処理
      //ヒカリドラゴンが大きすぎるので対処！
      gfl2::math::Vector4 minVec = box->GetMin();
      gfl2::math::Vector4 maxVec = box->GetMax();
      minVec.x = -200.0f;
      maxVec.x =  200.0f;
      box->SetMinMax(minVec, maxVec);
    }
    return ret;
  }
  return false;
}

//攻撃モーションのキーの位置を取得
int BtlvObject_PokeModel::GetAttackKeyFrame(PokeTool::MODEL_ANIME type)
{
  int frame = mModel->GetKeyTiming( type , "AK_EffectStart01" );
  return frame;
}

int BtlvObject_PokeModel::GetTikiType(void)const
{
  switch(mMonsNo)
  {
  case MONSNO_THIKITORI:
    return 0;
  case MONSNO_THIKITYOU:
    return 1;
  case MONSNO_THIKIUSI:
    return 2;
  case MONSNO_THIKISAKANA:
    return 3;
  }
  //念のため
  return 0;
}

void BtlvObject_PokeModel::AddFPShader(bool isZenryoku)
{
  if( mFpShaderCnt == 0 )
  {
    mIsFPZeryoku = isZenryoku;
    mBtlvCore->GetRenderSys()->GetRenderingPipeLine()->AddPoke_FullPowerShader( this , isZenryoku );
  }
  mFpShaderCnt++;
}
void BtlvObject_PokeModel::RemoveFpShader(void)
{
  //AddFPShaderが呼ばれずにRemoveFpShaderに来る事があった。
  //根本対処をしたが、保険で入れておく
  if( mFpShaderCnt > 0 )
  {
    mFpShaderCnt--;
    if( mFpShaderCnt == 0 )
    {
      mBtlvCore->GetRenderSys()->GetRenderingPipeLine()->RemovePoke_FullPowerShader( this );
    }
  }
}

//こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
void BtlvObject_PokeModel::UpdateAlwaysAfterCore(int step)
{
  //WaitB設定
  if( IsFinishAnimation() &&
    GetAnimationType() == PokeTool::MODEL_ANIME_BT_WAIT_A)
  {
    if( mEnableWiatB )
    {
      const PokeTool::PokeSettingData *data = GetPokeModel()->GetSettingData();
      s32 rand = System::GflUse::GetPublicRand( 1000 );
      if( rand < data->waitBRate + data->waitCRate )
      {
        if( rand < data->waitCRate &&
          IsAvailableAnimation( PokeTool::MODEL_ANIME_BT_WAIT_C) )
        {
          ChangeAnimation( PokeTool::MODEL_ANIME_BT_WAIT_C );
          SetReserveAnimation( PokeTool::MODEL_ANIME_BT_WAIT_A );
          SetAnimationIsLoop(false);
        }
        else
        if( IsAvailableAnimation( PokeTool::MODEL_ANIME_BT_WAIT_B ) )
        {
          ChangeAnimation( PokeTool::MODEL_ANIME_BT_WAIT_B );
          SetReserveAnimation( PokeTool::MODEL_ANIME_BT_WAIT_A );
          SetAnimationIsLoop(false);
        }
        else
        {
          StartWaitAnime(true,false);
        }
      }
      else
      {
        StartWaitAnime(true,false);
      }
    }
    else
    {
      StartWaitAnime(true,false);
    }
  }

  DemoLib::Obj::Object_Poke::UpdateAlwaysAfterCore(step);
  //バトルでは処理順の関係で個別でPokeModelのUpdateを呼ぶ。
  GetPokeModel()->Update();
  GetPokeModel()->UpdateAnimation();
  GetPokeModel()->UpdateAfter();
}

//表示系の制御
void BtlvObject_PokeModel::SetVisible(const bool flg)
{
  mIsVisiblePoke = flg;
  UpdateVisible();
}

//身代わり用に継承
bool BtlvObject_PokeModel::GetVisible(void)const
{
  return ((mIsVisiblePoke|mIsVisibleMigawari)&mIsVisibleTame);
}

void BtlvObject_PokeModel::SetShadowVisible(const bool flg)
{
  mIsVisibleShadow = flg;
  UpdateVisible();
}

void BtlvObject_PokeModel::SetVisibleMigawari(bool flg)
{
  mIsVisibleMigawari = flg;
  UpdateVisible();
}

void BtlvObject_PokeModel::SetVisibleTame(bool flg)
{
  mIsVisibleTame = flg;
  UpdateVisible();
}

void BtlvObject_PokeModel::SetVisibleDigudaStone(bool flg)
{
  mIsVisibleDigudaStone = flg;
  UpdateVisible();
}

void BtlvObject_PokeModel::UpdateVisible(void)
{
  //カメラヒットにより非表示
  if( mCamHitErace )
  {
    DemoLib::Obj::Object_Poke::SetVisible( false );
    if( mIsVisiblePoke == true &&
        mIsVisibleTame == true &&
        mIsVisibleShadow == true &&
        mIsVisibleMigawari == false ) //GFMNCat5635 みがわりチェックが抜けてた
    {
      Object_Poke::SetShadowVisible(true); //影までコリジョンに入っていないので消さない
    }
    else
    {
      Object_Poke::SetShadowVisible(false);
    }
    if( mMigawariModel )
    {
      mMigawariModel->SetVisible(false);
    }
    if( mDigudaStoneModel )
    {
      mDigudaStoneModel->SetVisible(false);
    } 
    return;
  }

  if( mIsVisibleMigawari == true )
  {
    DemoLib::Obj::Object_Poke::SetVisible( false );
    Object_Poke::SetShadowVisible(false);
    if( mMigawariModel )
    {
      if( mIsVisiblePoke == true &&
          mIsVisibleTame == true )
      {
        mMigawariModel->SetVisible(true);
      }
      else
      {
        mMigawariModel->SetVisible(false);
      }
    }
    if( mDigudaStoneModel )
    {
      mDigudaStoneModel->SetVisible(false);
    }
  }
  else
  if( mIsVisiblePoke == true &&
      mIsVisibleTame == true )
  {
    DemoLib::Obj::Object_Poke::SetVisible( true );
    Object_Poke::SetShadowVisible(mIsVisibleShadow);
    if( mMigawariModel )
    {
      mMigawariModel->SetVisible(false);
    }
    if( mDigudaStoneModel )
    {
      mDigudaStoneModel->SetVisible(mIsVisibleDigudaStone);
    }
  }
  else
  {
    DemoLib::Obj::Object_Poke::SetVisible( false );
    Object_Poke::SetShadowVisible(false);
    if( mMigawariModel )
    {
      mMigawariModel->SetVisible(false);
    }
    if( mDigudaStoneModel )
    {
      mDigudaStoneModel->SetVisible(false);
    } 
  }
}

void BtlvObject_PokeModel::SetEnableFloat(bool flg)
{
  mEnableFloat = flg;
}

void BtlvObject_PokeModel::SetSick( PokeSick sick )
{
  mSick = sick;
  UpdateSickMotion();
  UpdateFaceAnime();
}

f32 BtlvObject_PokeModel::UpdateSickMotion(void)
{
  if( IsEnable() )
  {
    //速度更新だけは常に行う
    //氷で速度を変えるとかはWaitの時だけするから
    PokeTool::MODEL_ANIME anime = GetAnimationType();
    if( anime == PokeTool::MODEL_ANIME_BT_WAIT_A )
    {
      f32 speed = 1.0f;
      switch(mSick)
      {
      case pml::pokepara::SICK_MAHI:    // まひ
        speed = 0.5f;
        break;
      case pml::pokepara::SICK_NEMURI:  // ねむり
        speed = 0.5f;
        break;
      case pml::pokepara::SICK_KOORI:   // こおり
        speed = 0.0f;
        break;
      case pml::pokepara::SICK_YAKEDO:  // やけど
        speed = 0.5f;
        break;
      case pml::pokepara::SICK_DOKU:    // どく
      case pml::pokepara::SICK_MAX:    // どくどく
        speed = 0.5f;
        break;
      default:
        {
          BTL_POKEPARAM* bpp = mBtlvCore->GetBattlePokeParam((BtlvPos)mIdx);
          ::app::tool::Gauge::GaugeColor gaugeCol = ::app::tool::Gauge::GetDottoColor( (f32)bpp->GetValue(BTL_POKEPARAM::BPP_HP) , (f32)bpp->GetValue(BTL_POKEPARAM::BPP_MAX_HP) );
          
          switch( gaugeCol )
          {
          case ::app::tool::Gauge::GAUGE_COLOR_RED:
          case ::app::tool::Gauge::GAUGE_COLOR_NULL:
          speed = 0.5f;
            break;
          }
        }
        break;
      }
      //ARIIZUMI_PRINT("WaitSpeed[%.f]\n",speed);
      SetAnimationSpeed(speed);
      return speed; //StartWaitAnimeで使ってるから返す
    }
    else
    {
      SetAnimationSpeed(1.0f);
    }
  }
  return 1.0f;
}

void BtlvObject_PokeModel::SetOverrideEyeType(int type)
{
  mOverrideEyeType = type;
  UpdateFaceAnime();
}

void BtlvObject_PokeModel::ResetOverrideEyeType(void)
{
  mOverrideEyeType = -1;
  UpdateFaceAnime();
}

//振り向き系
void BtlvObject_PokeModel::SetEnableLookAt(bool flg)
{
  if( IsEnable() == false )
  {
    return;
  }

  if( GetPokeModel()->IsEnableInterest() == false )
  {
    return;
  }

  mEnableLookAt = flg;
  if( mEnableLookAt == true )
  {
    int jntNum = GetPokeModel()->GetInterestJointCount();
    for( int i=0;i<jntNum;i++ )
    {
      GetPokeModel()->SetInterestSpeed(i,0.075f);
    }
  }
  else
  {
    SetLookAtPos(NULL);
  }
}


void BtlvObject_PokeModel::UpdateLookAt(void)  //TraverseModelFast の後
{
  if( IsEnable() == false )
  {
    return;
  }
  if( mEnableLookAt )
  {
    GetPokeModel()->UpdateInterest();
  }
}

void BtlvObject_PokeModel::SetLookAtPos(gfl2::math::Vector3 *pos)  //NULLでなし
{
  if( IsEnable() == false )
  {
    return;
  }
  if( mEnableLookAt )
  {
    GetPokeModel()->SetInterestPosition(pos);
  }
}

void BtlvObject_PokeModel::NoLoadFormChange(void)
{
  if( IsEnable() == false )
  {
    return;
  }
  //特殊フォルムチェンジ用
  BTL_POKEPARAM* bpp = mBtlvCore->GetBattlePokeParam(BtlvPos(mIdx));
  const pml::pokepara::PokemonParam *pp = bpp->GetViewSrcData();

  if( (mMonsNo == MONSNO_GIRUGARUDO ||
       mMonsNo == MONSNO_HORAA ||
       mMonsNo == MONSNO_NAGAREBOSI )&&
      mFormNo != bpp->GetFormNo() )
  {
    mFormNo = pp->GetFormNo();
    GetPokeModel()->NoLoadFormChange(mFormNo);
  }
}

//カメラとの当たり判定用
bool BtlvObject_PokeModel::CheckHitCamera(const gfl2::math::Vector3 &pos,f32 ofsSize)
{
  gfl2::math::AABB box;
  bool ret = GetCollision(&box);

  if( ret )
  {
    bool ret2 = CheckHitCollision(pos,ofsSize,this,box);
    if( ret2 == true )
    {
#if 0
      //GFL_PRINT("Hit! BOX[%f:%f:%f][%f:%f:%f] ROT[%f] POS[%f:%f:%f] CAM[%f:%f:%f] NEAR[%f]\n"
      GFL_PRINT("Hit! %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n"
        ,box.GetMin().x,box.GetMin().y,box.GetMin().z
        ,box.GetMax().x,box.GetMax().y,box.GetMax().z
        ,GetRotateY()
        ,GetPosition().x,GetPosition().y,GetPosition().z
        ,pos.x,pos.y,pos.z
        ,ofsSize);
#endif
    }
    return ret2;
  }
  return false;
}

void BtlvObject_PokeModel::UpdateCulling(void)
{
  //カメラめり込みチェック
  if( IsEnable() )
  {
    //コリジョンが待機A準拠なので、待機ABCでしか判定しない。
    bool isHit = false;
    bool isCulling = false;
    if( IsPlayAnimationResource(0) == false &&
        IsPlayAnimationResource(1) == false )
    {

      gfl2::math::Vector3 camPos = mBtlvCore->GetCameraSys()->GetPositionAnime();
      gfl2::math::Vector3 camTrg = mBtlvCore->GetCameraSys()->GetTargetAnime();
      f32 nearVal = mBtlvCore->GetCameraSys()->GetNearAnime_NMCat4000fix();    

      isHit = CheckHitCamera(camPos,nearVal);

      //画面外判定
      {
        // カリング用行列の取得
        gfl2::math::Matrix44 *cullingMatrixTable = mBtlvCore->GetRenderSys()->GetFrustumCullingTable();
        gfl2::math::AABB box;
        bool ret = GetCollision(&box);
        if( ret )
        {
          //回転の加味ができないので、最大サイズのコリジョンにしてしまう。
          CalcBoxMaxSize(box);
          gfl2::math::Vector3 calcPos;
          GetCalcPosition(&calcPos);
          box = box.Offset(calcPos);
          if( gfl2::math::AABB::Util::IsIntersectFrustum( cullingMatrixTable, box ) == false )
          {
            isCulling = true;
          }
        }
      }
    }

    if( (isHit || isCulling) != mCamHitErace )
    {
      if( (isHit || isCulling) )
      {
        ARIIZUMI_PRINT("PokeCamHit ON![%d][%d:%d]\n",mIdx,isHit,isCulling);
        mCamHitErace = true;
        UpdateVisible();
      }
      else
      {
        ARIIZUMI_PRINT("PokeCamHit OFF![%d]\n",mIdx);
        mCamHitErace = false;
        UpdateVisible();
      }
    }
  }  
}

#if PM_DEBUG
void BtlvObject_PokeModel::ReloadData(void)
{
  mPokeInfoData = mBtlvCore->getPokeExtraInfo(mMonsNo,mFormNo);
}
#endif //PM_DEBUG

GFL_NAMESPACE_END(btl)

