//======================================================================
/**
 * @file FinderStudioPokemon.cpp
 * @date 2016/09/01
 * @author harada_takumi
 * @brief ファインダースタジオポケモン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioPokemon.h"

// gfl2
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <Layout/include/gfl2_GraphicsType.h>

// PokeTool
#include "PokeTool/source/PokeModelLocal.h"

// DemoLib
#include "DemoLib/Object/include/Object_Poke.h"
#include "DemoLib/Object/include/ObjectRenderingPipeLine.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

// FinderStudioViewer
#include "../include/FinderStudioViewer.h"
#include "FinderStudioCameraSystem.h"
#include "FinderStudioPokemonDataTable.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/pokemon_model.gaix>

#if PM_DEBUG
// Debug
#include <gfx/include/gfl2_GFGL.h>
#include <gfx/include/gfl2_DrawUtil.h>
#endif

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// 持ち上げ時Y座標
const f32 FinderStudioPokemon::LIFT_Y_POS = 30.0f;

// ポケモンモデル最大数
const u32 FinderStudioPokemon::NUM_MAX_POKE_MODEL = 1;

// ヒープサイズ
const u32 FinderStudioPokemon::POKE_MOTION_HEAP_SIZE = 0x90000;   // 最大かわいがりモーションサイズ（Aキュウコン:0x85780）
const u32 FinderStudioPokemon::POKE_MOTION_AABB_HEAP_SIZE = 0x500;  // モーションAABBデータ１つ0x288バイト

// 使用モーション数
const u32 FinderStudioPokemon::NUM_USE_MOTION_BATTLE = 10;
const u32 FinderStudioPokemon::NUM_USE_MOTION_KAWAIGARI = 12;

// 使用モーションテーブル
const PokeTool::MODEL_ANIME FinderStudioPokemon::BATTLE_MOTION_TABLE[NUM_USE_MOTION_BATTLE] =
{
  PokeTool::MODEL_ANIME_BT_WAIT_B,
  PokeTool::MODEL_ANIME_BT_WAIT_C,
  PokeTool::MODEL_ANIME_BUTURI1,
  PokeTool::MODEL_ANIME_BUTURI2,
  PokeTool::MODEL_ANIME_BUTURI3,
  PokeTool::MODEL_ANIME_BUTURI4,
  PokeTool::MODEL_ANIME_TOKUSHU1,
  PokeTool::MODEL_ANIME_TOKUSHU2,
  PokeTool::MODEL_ANIME_TOKUSHU3,
  PokeTool::MODEL_ANIME_TOKUSHU4
};
const PokeTool::MODEL_ANIME FinderStudioPokemon::KAWAIGARI_MOTION_TABLE[NUM_USE_MOTION_KAWAIGARI] =
{
  PokeTool::MODEL_ANIME_HATE,
  PokeTool::MODEL_ANIME_QUESTION,
  PokeTool::MODEL_ANIME_HAPPY_A,
  PokeTool::MODEL_ANIME_HAPPY_B,
  PokeTool::MODEL_ANIME_HAPPY_C,
  PokeTool::MODEL_ANIME_MOVE_A,
  PokeTool::MODEL_ANIME_MOVE_B,
  PokeTool::MODEL_ANIME_MOVE_C,
  PokeTool::MODEL_ANIME_MOVE_D,
  PokeTool::MODEL_ANIME_PLAY_A,
  PokeTool::MODEL_ANIME_PLAY_B,
  PokeTool::MODEL_ANIME_MAD
};

// 待機中モーション再生までの最低フレーム数
const u32 FinderStudioPokemon::WAIT_MOTION_FRAME_MIN = 4 * 30; // 4秒
// 待機中モーション再生までの最高フレーム数
const u32 FinderStudioPokemon::WAIT_MOTION_FRAME_MAX = 6 * 30; // 6秒
// 眠りモーション再生までのフレーム数
const u32 FinderStudioPokemon::SLEEP_MOTION_FRAME_MAX = 20 * 30;  // 20秒

//コンストラクタ
FinderStudioPokemon::FinderStudioPokemon(const Desc& desc):
  m_pRenderPipeLine(NULL)
  ,m_pPhotoParam(NULL)
  ,m_IsUsePhotoParam(false)
  ,m_InitSeq(0)
  ,m_MainSeq(0)
  ,m_EndSeq(0)
  ,m_pPokeObj(NULL)
  ,m_pCameraSystem(NULL)
  ,m_pKawaigariMotionBuffer(NULL)
  ,m_pKawaigariAnimationResourceNode(NULL)
  ,m_pPokeDataTable(NULL)
  ,m_CaptureMode(CAPTURE_MODE_BOTH)
  ,m_Composition(COMPOSITION_1)
  ,m_CompositionDistance(0)
  ,m_DegY(0)
  ,m_IgnoreCollisionDegY(0)
  ,m_NowAnimationNo(0)
  ,m_SaveAnimationSpeed(0)
  ,m_NowMotionType(POKE_MOTION_TYPE_BATTLE)
  ,m_State(POKE_STATE_NULL)
  ,m_Value(0)
  ,m_RequestType(REQ_TYPE_NULL)
  ,m_IsReqPos(false)
  ,m_ReqDegY(m_DegY)
  ,m_IsReqDegY(false)
  ,m_ReqIgnColDegY(m_DegY)
  ,m_IsReqIgnColDegY(false)
  ,m_ReqMotionType(m_NowMotionType)
  ,m_IsEnableAutoMotion(true)
  ,m_WaitMotionCntMax(0)
  ,m_WaitMotionCnt(0)
  ,m_SleepMotionCnt(0)
  ,m_NextReq(REQ_TYPE_NULL)
  ,m_pPlayerAABB(NULL)
  ,m_CollisionPermit(0)
  ,m_LoadEndFrameFlag(false)
  ,m_MoveRange(240.0f)
  ,m_IsFullSize(false)
#if PM_DEBUG
  ,m_DebugIsEnableMotionBlend(false)
  ,m_DebugIsVisibleBoundingBox(false)
  ,m_DebugIsVisibleBoundingBoxUI(false)
  ,m_DebugIsIgnoreWaistXZ(true)
  ,m_DebugIsDebugAutoMotion(false)
  ,m_DebugAutoMotionMonsNo(1)
  ,m_DebugAutoMotionButuriNo(PokeTool::MODEL_ANIME_BUTURI1)
  ,m_DebugIsShot(false)
  ,m_DebugEnableAsing(false)
#endif
{
  GFL_ASSERT_STOP_MSG(desc.pHeap,"ヒープを指定してください\n");
  GFL_ASSERT_STOP_MSG(desc.pWorkHeap,"一時ヒープを指定してください\n");
  GFL_ASSERT_STOP_MSG(desc.pObjectRenderPipeLine,"パイプラインを指定してください\n");

  m_pNijiAllocator = desc.pAllocator;
  m_pRenderPipeLine = desc.pObjectRenderPipeLine;
  m_pPokeModelSystem = desc.pPokeModelSystem;
  m_pPokeDataTable = desc.pPokeDataTable;

  m_pPhotoParam = desc.pPhotoParam;
  m_IsUsePhotoParam = desc.isUsePhotoParam;

  // ヒープ作成
  m_pHeap = desc.pHeap;
  m_pWorkHeap = desc.pWorkHeap;
  m_pMotionHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,POKE_MOTION_HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioPokemonMotionHeap");
  m_pMotionAABBHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,POKE_MOTION_AABB_HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioPokemonMotionAABBHeap");

  // ポケモン見た目情報をパラメータから取得
  if(!m_IsUsePhotoParam)
  {
    // 再現パラメータを使わない場合
    GFL_ASSERT_STOP(desc.pSimpleParam);
    m_PokeSimpleParam = *desc.pSimpleParam;
  }
  else
  {
    // 再現パラメータを使う場合
    GFL_ASSERT_STOP(m_pPhotoParam);
    this->ApplyPhotoParamSimplePokeParam(*m_pPhotoParam);
  }
}

//デストラクタ
FinderStudioPokemon::~FinderStudioPokemon()
{
  GFL_DELETE_HEAP(m_pMotionHeap);
  GFL_DELETE_HEAP(m_pMotionAABBHeap);
}

// メイン処理用
enum{
  SEQ_MAIN_LOAD_MODEL,
  SEQ_MAIN_LOAD_MODEL_WAIT,
  SEQ_MAIN_LOAD_MOTION,
  SEQ_MAIN_LOAD_MOTION_WAIT,
  SEQ_INIT_LOAD_MOTION_AABB,
  SEQ_INIT_LOAD_MOTION_AABB_WAIT,
  SEQ_MAIN_MAIN,
  SEQ_MAIN_WAIT,  // 解放前の1フレーム待ち
  SEQ_MAIN_DELETE,
  SEQ_MAIN_DELETE_WAIT
};

// 初期化処理
bool FinderStudioPokemon::InitFunc()
{
  enum{
    SEQ_INIT_LOAD_POKEMODEL,
    SEQ_INIT_LOAD_POKEMODEL_WAIT,
    SEQ_INIT_LOAD_MOTION,   // かわいがりとフィールドモーションの読み込み
    SEQ_INIT_LOAD_MOTION_WAIT,
    SEQ_INIT_LOAD_MOTION_AABB,
    SEQ_INIT_LOAD_MOTION_AABB_WAIT,
    SEQ_INIT_END
  };

  switch(m_InitSeq)
  {
  case SEQ_INIT_LOAD_POKEMODEL:
    // ポケモデル作成
    m_pPokeObj = GFL_NEW(m_pHeap) DemoLib::Obj::Object_Poke(m_pRenderPipeLine);

    startLoadModel();

    m_InitSeq++;
    // fall through

  case SEQ_INIT_LOAD_POKEMODEL_WAIT:
    if(isFinishLoadModel())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_LOAD_MOTION:
    // バトル以外のモーションをロード
    m_pKawaigariAnimationResourceNode = GFL_NEW_ARRAY(m_pMotionHeap) gfl2::renderingengine::scenegraph::resource::ResourceNode*[PokeTool::MODEL_ANIME_KAWAIGARI_NUM];
//    m_pFieldAnimationResourceNode = GFL_NEW_ARRAY(m_pMotionHeap) gfl2::renderingengine::scenegraph::resource::ResourceNode*[PokeTool::MODEL_ANIME_FIELD_NUM];
    startLoadMotion();

    m_InitSeq++;
    // fall through

  case SEQ_INIT_LOAD_MOTION_WAIT:
    if(isFinishLoadMotion())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_LOAD_MOTION_AABB:
    m_PokeAABB.StartLoadData(m_pMotionAABBHeap,m_pWorkHeap,m_PokeSimpleParam,m_pPokeObj->GetPokeModel(),m_pPokeModelSystem);
    m_InitSeq++;
    // fall through

  case SEQ_INIT_LOAD_MOTION_AABB_WAIT:
    if(m_PokeAABB.IsFinishLoadData())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_END:
    // セットアップ
    setup();

    // 再現パラメータを使うなら残りのパラメータも適応
    if(m_IsUsePhotoParam)
    {
      if(m_pPhotoParam)
      {
        this->applyPhotoParamPokemonOther(*m_pPhotoParam);
      }
      else
      {
        GFL_ASSERT(0);
      }
    }

    // メインの更新から開始
    m_MainSeq = SEQ_MAIN_MAIN;

    //m_pHeap->Dump();
    //m_pMotionAABBHeap->Dump();
    //m_pWorkHeap->Dump();
    
    return true;
  }

  return false;
}

// モデルロード
void FinderStudioPokemon::startLoadModel()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  DemoLib::Obj::Object_Poke::LOAD_PARAM param;
  param.modelSys = m_pPokeModelSystem;
  param.dataHeap = NULL;    // 使っていないのでNULL
  param.workHeap = NULL;    // 使っていないのでNULL

  param.param = &m_PokeSimpleParam;
  
  PokeTool::PokeModel::SetupOption opt;
  opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
  opt.dataHeap = NULL;    // データヒープはPokeModelSystemの専用ヒープを使用
  opt.workHeap = m_pWorkHeap->GetLowerHandle();
  opt.useIdModel = false;
  opt.useShadow = false;
  param.opt = &opt;
  
  // 読み込み開始
  m_pPokeObj->StartLoad(fileManager,param);
}
bool FinderStudioPokemon::isFinishLoadModel()
{
  if(m_pPokeObj->IsFinishLoad())
  {
    // 使っていないのでNULL×２
    m_pPokeObj->Create(NULL,NULL,false); // カラーシェーダ不使用

    return true;
  }

  return false;
}

// バトル以外のモーションのロード
void FinderStudioPokemon::startLoadMotion()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // バトル以外のモーションの読み込み
  u32 arcId = ARCID_POKEMON_MODEL;
  // ファイルオープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId = arcId;
    openReq.heapForFile = m_pWorkHeap->GetLowerHandle();
    openReq.heapForReq = m_pWorkHeap->GetLowerHandle();
    fileManager->SyncArcFileOpen(openReq);
  }

  // arcのインデックスを計算
  const int mainDataIdx = m_pPokeModelSystem->GetDataIdx(m_PokeSimpleParam.monsNo,m_PokeSimpleParam.formNo,m_PokeSimpleParam.sex);
  const PokeTool::PokeModelSystem::POKE_FLG_DATA *flgData = m_pPokeModelSystem->GetFlgData(m_PokeSimpleParam.monsNo,m_PokeSimpleParam.formNo,m_PokeSimpleParam.sex);
  const int subDataIdx = m_pPokeModelSystem->GetDataIdx(m_PokeSimpleParam.monsNo,flgData->mSubFrom,0);
  u32 dataTop = mainDataIdx*9 + 1;
  u32 dataSubTop = subDataIdx*9 + 1;
  u32 motionDataIndex = dataTop;

  if(flgData->mFlgData & PokeTool::PokeModelSystem::POKE_DATA_FLG_SHARE_ANIME)
  {
    motionDataIndex = dataSubTop;
  }

  // 各種モーション読み込み
  {
  // かわいがりモーション読み込み
    gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
    req.arcId = arcId;
    req.datId = motionDataIndex + 5;
    req.align = 128;
    req.heapForBuf = m_pMotionHeap;
    req.heapForReq = m_pWorkHeap->GetLowerHandle();
    req.heapForCompressed = m_pWorkHeap->GetLowerHandle();
    req.ppBuf = &m_pKawaigariMotionBuffer;
    fileManager->AddArcFileLoadDataReq(req);
  }
}
bool FinderStudioPokemon::isFinishLoadMotion()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if(fileManager->IsArcFileLoadDataFinished(&m_pKawaigariMotionBuffer))
  {
    // かわいがりアニメーション作成
    {
      gfl2::fs::BinLinkerAccessor binLinkerAccessor;
      binLinkerAccessor.Initialize(m_pKawaigariMotionBuffer);
      for(int i = 0;i < PokeTool::MODEL_ANIME_KAWAIGARI_NUM;i++)
      {
        if(binLinkerAccessor.GetDataSize(i) > 0)
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anmRes;
          anmRes.SetMotionData((c8*)binLinkerAccessor.GetData(i));
          m_pKawaigariAnimationResourceNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_pNijiAllocator,&anmRes);
        }
      }
    }

    // ファイルクローズ
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
	  closeReq.arcId = ARCID_POKEMON_MODEL;
	  fileManager->SyncArcFileClose(closeReq);

    //m_pMotionHeap->Dump();

    return true;
  }

  return false;
}

// モデル読み込み後の初期化
void FinderStudioPokemon::setup()
{
  // 描画パイプライン登録
  m_pPokeObj->AddRenderPath();
  // 表示オン（Updateするとデフォルトオフが反映されるので）
  m_pPokeObj->SetVisible(true);
  // 一部のポケモンにAdjustHeightをセット
  this->setupAdjustHeight();
  // AABBセット
  this->setupAABB();
  // AABB判定のために待機を入れておく
  m_NowAnimationNo = PokeTool::MODEL_ANIME_BT_WAIT_A;
  // 待機状態にしておく
  this->transitionState(POKE_STATE_WAIT);
  // 待機モーション再生（遷移では再生されない可能性がある）
  this->changeAnimationWait(m_NowMotionType,true);
  // 表情のリセット
  /// @note 写真再生時、１フレームで口アニメが切り替わって欲しいので補間を切ってからリセットする
  m_pPokeObj->GetPokeModel()->SetFaceAnimeSmoothEnable(false);
  m_pPokeObj->GetPokeModel()->ResetFaceAnime_Eye();
  m_pPokeObj->GetPokeModel()->ResetFaceAnime_Mouth();
  m_pPokeObj->GetPokeModel()->SetFaceAnimeSmoothEnable(true);
  // 自動再生カウントのリセット
  this->resetAutoMotionCnt(true);

#if PM_DEBUG
  if(m_DebugEnableAsing)
  {
    this->debugCanPlayMotionListInit();
    // 再生できるモーションを格納する
    this->debugCanPlayMotionListStore();
  }
#endif
}

// 一部のポケモンにAdjustHeightをセット
void FinderStudioPokemon::setupAdjustHeight()
{
  // ファイヤーとイベルタルのみAdjustHeightを変更
  if(m_PokeSimpleParam.monsNo == MONSNO_FAIYAA)
  {
    m_pPokeObj->GetPokeModel()->SetAdjustHeight(1.9f);
  }
  else if(m_PokeSimpleParam.monsNo == MONSNO_IBERUTARU)
  {
    m_pPokeObj->GetPokeModel()->SetAdjustHeight(2.65f);
  }
}

// AABBの設定
void FinderStudioPokemon::setupAABB()
{
  // 待機モーションが再生できないとまずいため今のモーションと待機モーションのAABBをマージする
  m_AABB = m_PokeAABB.GetData(m_NowMotionType,m_NowAnimationNo);
  this->applyAABB(&m_AABB);
  gfl2::math::AABB waitAABB = m_PokeAABB.GetData(m_NowMotionType,0);
  this->applyAABB(&waitAABB);
  m_AABB = m_AABB.Merge(waitAABB);
}

// 終了処理
bool FinderStudioPokemon::EndFunc()
{
  enum{
    SEQ_END_WAIT,
    SEQ_END_DELETE_POKEMODEL,
    SEQ_END_END
  };

  switch(m_EndSeq)
  {
  case SEQ_END_WAIT:
    // モデル削除のため１フレーム待つ
    m_EndSeq++;
    break;

  case SEQ_END_DELETE_POKEMODEL:
    // パイプラインから除去
    // @note ライトの除去（自分のDrawEnvから元のPokeModelのDrawEnvへの付け替え）をやっておかないと
    // 後のヒープ解放でライトセットノードとライトノードが解放されていない（解放忘れ）
    m_pPokeObj->RemoveRenderPath();
    // ポケモンモデル削除
    m_pPokeObj->Delete();
    GFL_SAFE_DELETE(m_pPokeObj);

    // 各モーション解放
    GflHeapFreeMemory(m_pKawaigariMotionBuffer);
//    GflHeapFreeMemory(m_pFieldMotionBuffer);

    for(int i = 0;i < PokeTool::MODEL_ANIME_KAWAIGARI_NUM;i++)
    {
      GFL_SAFE_DELETE(m_pKawaigariAnimationResourceNode[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pKawaigariAnimationResourceNode);

    /*for(int i = 0;i < PokeTool::MODEL_ANIME_FIELD_NUM;i++)
    {
    GFL_SAFE_DELETE(m_pFieldAnimationResourceNode[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pFieldAnimationResourceNode);*/

    // モーションAABB解放
    m_PokeAABB.UnloadData();

    m_EndSeq++;
    // fall through
  
  case SEQ_END_END:
    return true;
  }
  return false;
}

// 更新
void FinderStudioPokemon::Update()
{
  switch(m_MainSeq)
  {
  case SEQ_MAIN_LOAD_MODEL:
    startLoadModel();

    m_MainSeq++;
    // fall through

  case SEQ_MAIN_LOAD_MODEL_WAIT:
    if(isFinishLoadModel())
    {
      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_LOAD_MOTION:
    startLoadMotion();

    m_MainSeq++;
    // fall through

  case SEQ_MAIN_LOAD_MOTION_WAIT:
    if(isFinishLoadMotion())
    {
      m_MainSeq++;
    }
    break;

  case SEQ_INIT_LOAD_MOTION_AABB:
    m_PokeAABB.StartLoadData(m_pMotionAABBHeap,m_pWorkHeap,m_PokeSimpleParam,m_pPokeObj->GetPokeModel(),m_pPokeModelSystem);
    m_MainSeq++;
    // fall through

  case SEQ_INIT_LOAD_MOTION_AABB_WAIT:
    if(m_PokeAABB.IsFinishLoadData())
    {
      // セットアップ
      setup();

#if PM_DEBUG
      // 自動モーション再生デバッグ
      if(m_DebugIsDebugAutoMotion)
      {
        changeAnimation(POKE_MOTION_TYPE_BATTLE,static_cast<PokeTool::MODEL_ANIME>(m_DebugAutoMotionButuriNo));
      }
#endif

      // 読み込み終了フレームオン
      m_LoadEndFrameFlag = true;

      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_MAIN:
    // 読み込み終了フレームオン
    m_LoadEndFrameFlag = false;

    // メイン処理
    updateMain();
    break;

  case SEQ_MAIN_WAIT:
    // 解放のため1フレーム待つ
    m_pPokeObj->SetVisible(false);
    m_MainSeq++;
    break;

  case SEQ_MAIN_DELETE:
    // パイプラインから除去
    // @note ライトの除去（自分のDrawEnvから元のPokeModelのDrawEnvへの付け替え）をやっておかないと
    // 後のヒープ解放でライトセットノードとライトノードが解放されていない（解放忘れ）
    m_pPokeObj->RemoveRenderPath();
    // ポケモンモデル削除
    m_pPokeObj->Delete();

    // 各モーション解放
    GflHeapFreeMemory(m_pKawaigariMotionBuffer);
//    GflHeapFreeMemory(m_pFieldMotionBuffer);

    for(int i = 0;i < PokeTool::MODEL_ANIME_KAWAIGARI_NUM;i++)
    {
      GFL_SAFE_DELETE(m_pKawaigariAnimationResourceNode[i]);
    }
    //for(int i = 0;i < PokeTool::MODEL_ANIME_FIELD_NUM;i++)
    //{
    //  GFL_SAFE_DELETE(m_pFieldAnimationResourceNode[i]);
    //}

    // モーションAABB解放
    m_PokeAABB.UnloadData();

    m_MainSeq++;
    // fall through

  case SEQ_MAIN_DELETE_WAIT:
    // ロードへ
    m_MainSeq = SEQ_MAIN_LOAD_MODEL;
    break;
  }
}

// 更新メイン処理
void FinderStudioPokemon::updateMain()
{
#if PM_DEBUG
  // ポケモンモーション連続再生デバッグ ======================================
  if(m_DebugIsDebugAutoMotion)
  {
    if(m_pPokeObj->IsFinishAnimation())
    {
      while(m_DebugAutoMotionButuriNo < PokeTool::MODEL_ANIME_MAX)
      {
        if(m_DebugAutoMotionButuriNo == PokeTool::MODEL_ANIME_BUTURI4)
        {
          m_DebugAutoMotionMonsNo++;
          PokeTool::SimpleParam param(
            static_cast<MonsNo>(m_DebugAutoMotionMonsNo),
            static_cast<pml::FormNo>(0),
            static_cast<pml::Sex>(0),
            0,0,0
            );
          StartReloadModel(param);
          m_DebugAutoMotionButuriNo = PokeTool::MODEL_ANIME_BUTURI1;
          return;
        }

        m_DebugAutoMotionButuriNo++;
        if(changeAnimation(POKE_MOTION_TYPE_BATTLE,static_cast<PokeTool::MODEL_ANIME>(m_DebugAutoMotionButuriNo)))
        {
          break;
        }
      }
    }
  }

  // =========================================================================
#endif

#if PM_DEBUG
  // 撮影中じゃなければ更新
  if(!m_DebugIsShot)
  {
#endif

    switch(m_State)
    {
    case POKE_STATE_WAIT:
      updateWait();
      break;
    case POKE_STATE_WAIT_MOTION:
      updateWaitMotion();
      break;
    case POKE_STATE_PAUSE_MOTION:
      updatePause();
      break;
    case POKE_STATE_SLEEP:
      updateSleep();
      break;
    case POKE_STATE_WAKE:
      updateWake();
      break;

    default:
      GFL_ASSERT(0);
      break;
    }

    // リクエストを消去
    m_RequestType = REQ_TYPE_NULL;

#if PM_DEBUG
  }
#endif

  // ポケモンAABBを設定
  this->setupAABB();

  // 拡縮更新・反映
  this->updateScale();

  // 回転更新・反映
  this->updateRotate();

  // アニメーション更新前の値を保存
  this->savePrevParam();

  // モデル更新
  // @note モーション更新を反映させるため座標更新の前に一度更新
  m_pPokeObj->Update();
  m_pPokeModelSystem->Update();

  // 座標更新・反映
  updatePosition();
}

// 拡縮更新・反映
void FinderStudioPokemon::updateScale()
{
  // 実物大表示をするなら計算
  if(m_IsFullSize) m_pPokeObj->SetScale(1.0f / m_pPokeObj->GetPokeModel()->GetAdjustScale());
  // 基本は１倍
  else m_pPokeObj->SetScale(1.0f);
}
// 回転更新・反映
void FinderStudioPokemon::updateRotate()
{
  // Y軸回転更新
  m_pPokeObj->SetRotate(0,m_DegY,0);
}
// 座標更新・反映
void FinderStudioPokemon::updatePosition()
{
  // 表示用座標
  gfl2::math::Vector3 drawPos = m_Pos;

  // XZ座標を補正（Waistが動くモーション用）
#if PM_DEBUG
  if(m_DebugIsIgnoreWaistXZ)
  {
#endif
    // Waistチェック
    s32 jointIdx = m_pPokeObj->GetPokeModel()->GetJointIndex("Waist");
    if(jointIdx != -1)
    {
      if((m_NowMotionType == POKE_MOTION_TYPE_KAWAIGARI && m_NowAnimationNo == PokeTool::MODEL_ANIME_DROWSE_B) ||
        (m_NowMotionType == POKE_MOTION_TYPE_KAWAIGARI && m_NowAnimationNo == PokeTool::MODEL_ANIME_DROWSE_C))
      {
        // @fix GFMMCat[228] 眠りBCのときはm_DefaultVecを更新しない
      }
      else
      {
        // @fix GFMMCat[162] 再現時にいきなりアニメーションフレームが設定されることを考慮して
        //                   毎フレーム最初のフレーム姿勢での座標を取りにいくようにする
        int saveFrame = m_PrevParam.mainFrame;
        bool saveIsUpdateAnimation = m_pPokeObj->GetPokeModel()->GetAnimationController()->IsUpdateFrameEnable();
        // 1フレームのアニメーションを反映
        m_pPokeObj->GetPokeModel()->SetAnimationFrame(0);
        m_pPokeObj->GetPokeModel()->SetAnimationUpdateFrameEnable(false); // アニメーションフレームの足しこみをしないようにする
        m_pPokeObj->GetPokeModel()->GetAnimationController()->Update();
        // 1フレームでの座標を取得
        m_DefaultVec = m_pPokeObj->GetPokeModel()->GetJointLocalSRT(jointIdx).translate;
        m_DefaultVec.y = 0; // Y座標は何もしない
        // フレームを元に戻して反映
        m_pPokeObj->GetPokeModel()->SetAnimationFrame(saveFrame);
        m_pPokeObj->GetPokeModel()->SetAnimationUpdateFrameEnable(saveIsUpdateAnimation); // アニメーションフレームの足しこみを元に戻す
        m_pPokeObj->GetPokeModel()->GetAnimationController()->Update();
      }
      gfl2::math::Vector3 vec = m_pPokeObj->GetPokeModel()->GetJointLocalSRT(jointIdx).translate;
      gfl2::math::Vector3 scaleDefaultVec = m_DefaultVec;
      if(!m_IsFullSize)
      {
        // 実物大で入っているので拡縮
        vec *= m_pPokeObj->GetPokeModel()->GetAdjustScale();
        scaleDefaultVec *= m_pPokeObj->GetPokeModel()->GetAdjustScale();
      }
      vec -= scaleDefaultVec;
      // 現在の角度で回転
      FinderStudioCameraSystem::RotateYVec(&vec,m_DegY);

      drawPos.x -= vec.x;
      drawPos.z -= vec.z;
    }
#if PM_DEBUG
  }
#endif

  // 座標更新
  // 座標のみを直接セットする
  m_pPokeObj->GetPokeModel()->GetModelInstanceNode()->SetLocalTranslate(drawPos);
}

// ポケモンの状態遷移
// 遷移した際の初期化も行う
void FinderStudioPokemon::transitionState(POKE_STATE state)
{
  switch(state)
  {
  case POKE_STATE_WAIT:
    // 待機モーション再生
    /*if(!this->changeAnimationWait(m_NowMotionType))
    {
      HARADA_PRINT("待機モーションが再生できません！\n");
    }*/
    break;

  case POKE_STATE_WAIT_MOTION:
    //if(!this->playMotion())
    //{
    //  // 失敗したら待機へ
    //  // 待機モーション再生
    //  this->changeAnimationWait(m_NowAnmType);
    //  state = POKE_STATE_WAIT;
    //}
    break;

  case POKE_STATE_PAUSE_MOTION:
    //if(!this->playMotion())
    //{
    //  // 失敗したら待機へ
    //  // 待機モーション再生
    //  this->changeAnimationWait(m_NowAnmType);
    //  state = POKE_STATE_WAIT;
    //}
    break;

  case POKE_STATE_SLEEP:
    // 眠りインモーション再生
    //GFL_ASSERT(m_NowAnmType == POKE_MOTION_TYPE_KAWAIGARI);
    //if(!this->changeAnimation(m_NowAnmType,PokeTool::MODEL_ANIME_DROWSE_A))
    //{
    //  // 失敗したら待機中モーションへ
    //  if(!this->playMotion())
    //  {
    //    // 失敗したら待機へ
    //    // 待機モーション再生
    //    this->changeAnimationWait(m_NowAnmType);
    //    state = POKE_STATE_WAIT;
    //  }
    //  else
    //  {
    //    state = POKE_STATE_WAIT_MOTION;
    //  }
    //}
    break;

  case POKE_STATE_WAKE:
    //// 眠りアウトモーション再生
    //GFL_ASSERT(m_NowAnmType == POKE_MOTION_TYPE_KAWAIGARI);
    //if(!this->changeAnimation(m_NowAnmType,PokeTool::MODEL_ANIME_DROWSE_C))
    //{
    //  GFL_ASSERT(0);  // 来ないはず
    //}
    break;

  default:
    GFL_ASSERT(0);
    break;
  }

  if(m_State != state)
  {
    m_State = state;
    m_Value = 0;
  }
}

void FinderStudioPokemon::requestPos(const gfl2::math::Vector3& pos)
{ 
  m_ReqPos = pos; 
  m_IsReqPos = true;
}
void FinderStudioPokemon::requestDegY(f32 degY)
{ 
  m_ReqDegY = degY; 
  m_IsReqDegY = true;
}

void FinderStudioPokemon::requestIgnoreCollisionDegY(f32 degY)
{
  m_ReqIgnColDegY = degY;
  m_IsReqIgnColDegY = true;
}

// 移動・回転リクエストを反映
void FinderStudioPokemon::acceptPosDegY()
{
  if(m_IsReqPos)
  {
    m_Pos.Set(m_ReqPos);
    m_IsReqPos = false;
  }
  if(m_IsReqDegY)
  {
    m_DegY = m_ReqDegY;
    m_IsReqDegY = false;
  }
  if(m_IsReqIgnColDegY)
  {
    m_IgnoreCollisionDegY = m_ReqIgnColDegY;
    m_IsReqIgnColDegY = false;
  }
}

void FinderStudioPokemon::updateWait()
{
  // 移動・回転リクエスト処理
  this->acceptPosDegY();

  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_PLAY_MOTION:
    if(this->playMotion())
    {
      // 再生成功ならステート変更
      this->transitionState(POKE_STATE_PAUSE_MOTION);
      // カウントをリセット
      this->resetAutoMotionCnt(true);
    }
    break;

  case REQ_TYPE_CHANGE_MOTION_TYPE:
    this->changeAnimationWait(m_ReqMotionType);
    // カウントをリセット
    this->resetAutoMotionCnt(true);
    break;

  case REQ_TYPE_RESET:
    this->changeAnimationWait(m_NowMotionType,true);
    // カウントをリセット
    this->resetAutoMotionCnt(true);
    break;

  case REQ_TYPE_PHOTO_REPRODUCT:
    this->applyPhotoParamPokemonOther(m_ReqPhotoParam);
    this->SetIsAnimationUpdate(false);
    break;

  case REQ_TYPE_NULL:
    // 時間でランダム再生
    if(m_IsEnableAutoMotion)
    {
      if(m_WaitMotionCnt >= m_WaitMotionCntMax)
      {
        // 眠り判定
        bool isSleep = false;
        // 眠りカウントが達しているかつかわいがりモーションなら
        if(m_SleepMotionCnt >= SLEEP_MOTION_FRAME_MAX &&
          m_NowMotionType == POKE_MOTION_TYPE_KAWAIGARI)
        {
          // 眠りモーション
          GFL_ASSERT(m_NowMotionType == POKE_MOTION_TYPE_KAWAIGARI);
          if(this->changeAnimation(m_NowMotionType,PokeTool::MODEL_ANIME_DROWSE_A))
          {
            // 再生成功ならステート変更
            this->transitionState(POKE_STATE_SLEEP);
            isSleep = true;
            // カウントをリセット
            this->resetAutoMotionCnt(true);
            break;
          }
        }

        // 眠りモーション再生に失敗したら
        if(!isSleep)
        {
          // 普通の自動モーション
          if(this->playMotion())
          {
            // 再生成功ならステート変更
            this->transitionState(POKE_STATE_WAIT_MOTION);
          }
          else
          {
            GFL_PRINT("自動モーションが一つも再生できませんでした\n");
          }
        }

        // カウントをリセット
        this->resetAutoMotionCnt(false);
        break;
      }

      // 待機中モーション用カウントを加算
      if(m_WaitMotionCnt <= WAIT_MOTION_FRAME_MAX)
      {
        m_WaitMotionCnt++;
      }

      // 眠るモーション用カウントを加算
      if(m_SleepMotionCnt <= SLEEP_MOTION_FRAME_MAX)
      {
        m_SleepMotionCnt++;
      }
    }
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}
void FinderStudioPokemon::updateWaitMotion()
{
  // 移動・回転リクエスト処理
  this->acceptPosDegY();

  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_CHANGE_MOTION_TYPE:
    this->changeAnimationWait(m_ReqMotionType);
    this->transitionState(POKE_STATE_WAIT);
    break;

  case REQ_TYPE_RESET:
    this->changeAnimationWait(m_NowMotionType);
    this->transitionState(POKE_STATE_WAIT);
    break;

  case REQ_TYPE_PHOTO_REPRODUCT:
    this->applyPhotoParamPokemonOther(m_ReqPhotoParam);
    this->SetIsAnimationUpdate(false);
    break;

  case REQ_TYPE_PLAY_MOTION:
    GFL_PRINT("モーション再生中は再生できません\n");
    // fall through
  case REQ_TYPE_NULL:
    // モーションが終わったら待機へ
    if(m_pPokeObj->IsFinishAnimation())
    {
      this->changeAnimationWait(m_NowMotionType);
      this->transitionState(POKE_STATE_WAIT);
    }
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}
void FinderStudioPokemon::updatePause()
{
  // updateWaitMotionと同じ
  this->updateWaitMotion();
}
void FinderStudioPokemon::updateSleep()
{
  // 移動・回転リクエスト処理
  this->acceptPosDegY();

  enum{
    SLEEP_SEQ_SLEEP_IN,
    SLEEP_SEQ_WAIT_WAKE
  };

  switch(m_Value)
  {
  case SLEEP_SEQ_SLEEP_IN:
    // リクエスト処理
    switch(m_RequestType)
    {
    case REQ_TYPE_CHANGE_MOTION_TYPE:
      this->changeAnimationWait(m_ReqMotionType);
      this->transitionState(POKE_STATE_WAIT);
      break;

    case REQ_TYPE_RESET:
      this->changeAnimationWait(m_NowMotionType);
      this->transitionState(POKE_STATE_WAIT);
      break;

    case REQ_TYPE_PHOTO_REPRODUCT:
      this->applyPhotoParamPokemonOther(m_ReqPhotoParam);
      this->SetIsAnimationUpdate(false);
      break;

    case REQ_TYPE_PLAY_MOTION:
    case REQ_TYPE_NULL:
      // 何もしない
      break;

    default:
      GFL_ASSERT(0);
      break;
    }

    // 眠りインが終わったら眠りループへ
    if(m_pPokeObj->IsFinishAnimation())
    {
      this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_B);
      m_Value++;
    }

    break;

  case SLEEP_SEQ_WAIT_WAKE:
    // リクエスト処理
    switch(m_RequestType)
    {
    case REQ_TYPE_PLAY_MOTION:
      // 起きる
      this->wake(REQ_TYPE_PLAY_MOTION);
      break;

    case REQ_TYPE_CHANGE_MOTION_TYPE:
      this->changeAnimationWait(m_ReqMotionType);
      this->transitionState(POKE_STATE_WAIT);
      break;

    case REQ_TYPE_RESET:
      this->changeAnimationWait(m_NowMotionType);
      this->transitionState(POKE_STATE_WAIT);
      break;

    case REQ_TYPE_PHOTO_REPRODUCT:
      this->applyPhotoParamPokemonOther(m_ReqPhotoParam);
      this->SetIsAnimationUpdate(false);
      break;

    case REQ_TYPE_NULL:
      // 起き待ち
      break;

    default:
      GFL_ASSERT(0);
      break;
    }

    break;
  }
}
void FinderStudioPokemon::updateWake()
{
  // 移動・回転リクエスト処理
  this->acceptPosDegY();

  // 起き中はタイプ変更しかリクエストを受け付けない
  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_CHANGE_MOTION_TYPE:
    this->changeAnimationWait(m_ReqMotionType);
    this->transitionState(POKE_STATE_WAIT);
    break;

  case REQ_TYPE_RESET:
    this->changeAnimationWait(m_NowMotionType);
    this->transitionState(POKE_STATE_WAIT);
    break;

  case REQ_TYPE_PHOTO_REPRODUCT:
    this->applyPhotoParamPokemonOther(m_ReqPhotoParam);
    this->SetIsAnimationUpdate(false);
    break;

  case REQ_TYPE_PLAY_MOTION:
  case REQ_TYPE_NULL:
    // 眠りアウトが終わったら保存していたステートへ
    if(m_pPokeObj->IsFinishAnimation())
    {
      switch(m_NextReq)
      {
      case REQ_TYPE_PLAY_MOTION:
        if(this->playMotion())
        {
          // 再生成功ならステート変更
          this->transitionState(POKE_STATE_PAUSE_MOTION);
        }
        else
        {
          // 再生失敗なら待機へ
          this->changeAnimationWait(m_NowMotionType);
          this->transitionState(POKE_STATE_WAIT);
        }
        break;

      default:
        GFL_ASSERT(0);
        break;
      }
    }
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
}

/// @brief 目を覚ます
/// @param nextState 目を覚ました後のステート
void FinderStudioPokemon::wake(REQ_TYPE nextReq)
{
  // 次の状態を保持して目覚め状態へ
  m_NextReq = nextReq;
  this->transitionState(POKE_STATE_WAKE);
  // 眠りアウトモーション再生
  GFL_ASSERT(m_NowMotionType == POKE_MOTION_TYPE_KAWAIGARI);
  this->changeAnimation(m_NowMotionType,PokeTool::MODEL_ANIME_DROWSE_C);
}

/// @brief 写真再現パラメータ適用（ポケモン見た目データ）
/// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
void FinderStudioPokemon::ApplyPhotoParamSimplePokeParam(const PhotoParam& photoParam)
{
  m_PokeSimpleParam.monsNo = static_cast<MonsNo>(photoParam.monsNo);
  m_PokeSimpleParam.formNo = photoParam.formNo;
  m_PokeSimpleParam.sex = static_cast<pml::Sex>(photoParam.sexPoke);
  m_PokeSimpleParam.isRare = photoParam.isRare;
  m_PokeSimpleParam.isEgg = false;
  m_PokeSimpleParam.perRand = 0;    // ここでは0にする。後からアニメーションフレームで再現
}
/// @brief 写真再現パラメータ適用（その他データ）
void FinderStudioPokemon::applyPhotoParamPokemonOther(const PhotoParam& photoParam)
{
  GFL_ASSERT_STOP(m_pPokeObj->GetPokeModel());

  // @fix GFMMCat[228] 写真再現時かつ、眠りBCのときは眠りAの基準座標をm_DefaultVecに入れておく
  if((static_cast<PokeMotionType>(photoParam.pokeMotionType) == POKE_MOTION_TYPE_KAWAIGARI && photoParam.pokeMotionNo == PokeTool::MODEL_ANIME_DROWSE_B) ||
    (static_cast<PokeMotionType>(photoParam.pokeMotionType) == POKE_MOTION_TYPE_KAWAIGARI && photoParam.pokeMotionNo == PokeTool::MODEL_ANIME_DROWSE_C))
  {
    s32 jointIdx = m_pPokeObj->GetPokeModel()->GetJointIndex("Waist");
    if(jointIdx != -1)
    {
      bool saveIsUpdateAnimation = m_pPokeObj->GetPokeModel()->GetAnimationController()->IsUpdateFrameEnable();
      // 1フレームのアニメーションを反映
      this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A);
      m_pPokeObj->GetPokeModel()->SetAnimationFrame(0);
      m_pPokeObj->GetPokeModel()->SetAnimationUpdateFrameEnable(false); // アニメーションフレームの足しこみをしないようにする
      m_pPokeObj->GetPokeModel()->GetAnimationController()->Update();
      // 1フレームでの座標を取得
      m_DefaultVec = m_pPokeObj->GetPokeModel()->GetJointLocalSRT(jointIdx).translate;
      m_DefaultVec.y = 0; // Y座標は何もしない
      m_pPokeObj->GetPokeModel()->SetAnimationUpdateFrameEnable(saveIsUpdateAnimation); // アニメーションフレームの足しこみを元に戻す
    }
  }

  m_NowMotionType = static_cast<PokeMotionType>(photoParam.pokeMotionType);
  m_NowAnimationNo = photoParam.pokeMotionNo;
  this->changeAnimation(m_NowMotionType,static_cast<PokeTool::MODEL_ANIME>(m_NowAnimationNo));
  m_pPokeObj->GetPokeModel()->SetAnimationFrame(photoParam.pokeMainMotionFrame);        // POKE_ANIME_SLOT_MAIN1
  // +1して取得したので-1する
  m_pPokeObj->GetPokeModel()->SetFaceAnime_Eye(static_cast<int>(photoParam.pokeEyeAnmPattern1) - 1,0);        // POKE_ANIME_SLOT_EYE1
  m_pPokeObj->GetPokeModel()->SetFaceAnime_Eye(static_cast<int>(photoParam.pokeEyeAnmPattern2) - 1,1);        // POKE_ANIME_SLOT_EYE2
  m_pPokeObj->GetPokeModel()->SetFaceAnime_Eye(static_cast<int>(photoParam.pokeEyeAnmPattern3) - 1,2);        // POKE_ANIME_SLOT_EYE3
  // 口アニメはプログラムで制御しない限り動かないので指定しない
  //m_pPokeObj->GetPokeModel()->SetFaceAnime_Mouth(photoParam.pokeMouthAnmpattern1,0);    // POKE_ANIME_SLOT_MOUTH1
  //m_pPokeObj->GetPokeModel()->SetFaceAnime_Mouth(photoParam.pokeMouthAnmpattern2,1);    // POKE_ANIME_SLOT_MOUTH2
  //m_pPokeObj->GetPokeModel()->SetFaceAnime_Mouth(photoParam.pokeMouthAnmpattern3,2);    // POKE_ANIME_SLOT_MOUTH3

  // ループアニメーション1は内部で管理している変数があるためそちらにセットする
  m_pPokeObj->GetPokeModel()->SetLoop1AnimationFrame(photoParam.pokeLoopAnmFrame1);
  //m_pPokeObj->GetPokeModel()->SetAnimationFrame(photoParam.pokeLoopAnmFrame1,8);        // POKE_ANIME_SLOT_LOOP1
  m_pPokeObj->GetPokeModel()->SetAnimationFrame(photoParam.pokeLoopAnmFrame2,PokeTool::POKE_ANIME_SLOT_LOOP2);       // POKE_ANIME_SLOT_LOOP2
  m_pPokeObj->GetPokeModel()->SetAnimationFrame(photoParam.pokeLoopAnmFrame3,PokeTool::POKE_ANIME_SLOT_LOOP3);       // POKE_ANIME_SLOT_LOOP3
  m_pPokeObj->GetPokeModel()->SetAnimationFrame(photoParam.pokeLoopAnmFrame4,PokeTool::POKE_ANIME_SLOT_LOOP4);       // POKE_ANIME_SLOT_LOOP4

  // @fix GFMMCat[129] パッチール例外対応
  if(m_PokeSimpleParam.monsNo == MONSNO_PATTIIRU)
  {
    m_pPokeObj->GetPokeModel()->SetMarkingFrames(photoParam.pokeLoopAnmFrame1,photoParam.pokeLoopAnmFrame2,photoParam.pokeLoopAnmFrame3,photoParam.pokeLoopAnmFrame4);
  }
}

/// @brief 写真再現パラメータ適用（その他データ）
/// @return リクエストした値で配置できるか
bool FinderStudioPokemon::ReqApplyPhotoParamPokemonOther(const PhotoParam& photoParam)
{
  gfl2::math::Vector3 paramPos(photoParam.pokePosX / 100.0f,0,photoParam.pokePosZ / 100.0f);

  // 移動できるか判定
  if(this->isCanMovePos(paramPos,photoParam.pokeDegY,false,
    static_cast<PokeMotionType>(photoParam.pokeMotionType),static_cast<PokeTool::MODEL_ANIME>(photoParam.pokeMotionNo)))
  {
    this->request(REQ_TYPE_PHOTO_REPRODUCT);
    this->requestPhotoParam(photoParam);
    this->requestPos(paramPos);
    this->requestDegY(photoParam.pokeDegY);
    this->requestIgnoreCollisionDegY(photoParam.pokeDegY);

    return true;
  }
  else
  {
    return false;
  }
}

/// @brief アニメーション更新のオンオフを切り替える
/// @param true オン false オフ
/// @note 写真再現後の更新のため
void FinderStudioPokemon::SetIsAnimationUpdate(bool isUpdate)
{
  if(!m_pPokeObj->GetPokeModel())
  {
    GFL_ASSERT(0);
    return;
  }

  m_pPokeObj->GetPokeModel()->SetAnimationUpdateFrameEnable(isUpdate);

  if(!isUpdate)
  {
    // ループアニメーションは↑の処理で止まらないためスロット0のアニメーションスピードを0にする
    m_SaveAnimationSpeed = m_pPokeObj->GetPokeModel()->GetAnimationStepFrame(0);
    m_pPokeObj->GetPokeModel()->SetAnimationStepFrame(0,0);
  }
  else
  {
    // 元に戻す
    m_pPokeObj->GetPokeModel()->SetAnimationStepFrame(m_SaveAnimationSpeed,0);
  }
}

/// @brief 自動目パチのオンオフを切り替える
/// @param true オン false オフ
void FinderStudioPokemon::SetIsAutoBlink(bool isBlink)
{
  if(!m_pPokeObj->GetPokeModel())
  {
    GFL_ASSERT(0);
    return;
  }

  if(isBlink)
  {
    // 自動目パチをオンにする
    m_pPokeObj->GetPokeModel()->SetAutoBlink(true);
  }
  else
  {
    // 自動目パチをオフにする
    m_pPokeObj->GetPokeModel()->SetAutoBlink(false);
  }
}

/// @brief 写真再現パラメータの出力が可能かどうか
/// @return true 可能 false 不可能
bool FinderStudioPokemon::IsCanGetPhotoParam()
{
  if(m_MainSeq == SEQ_MAIN_MAIN)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/// @brief 再現用パラメータ出力
/// @param[out] 再現用パラメータ 必要な情報だけを格納する
void FinderStudioPokemon::GetPhotoParamPokemon(PhotoParam* pPhotoParam)
{
  GFL_ASSERT(pPhotoParam);
  GFL_ASSERT_STOP(m_pPokeObj->GetPokeModel());

  pPhotoParam->monsNo = m_PokeSimpleParam.monsNo;
  pPhotoParam->formNo = m_PokeSimpleParam.formNo;
  pPhotoParam->sexPoke = m_PokeSimpleParam.sex;
  pPhotoParam->isRare = m_PokeSimpleParam.isRare;

  pPhotoParam->pokeMotionType = m_NowMotionType;
  pPhotoParam->pokeMotionNo = m_NowAnimationNo;

  // メインアニメ・ループアニメは見た目より１フレーム進んだ値が入っているので保存していた値を取得
  pPhotoParam->pokeMainMotionFrame = m_PrevParam.mainFrame;   // POKE_ANIME_SLOT_MAIN1

  // -1が帰ってくることがあるので+1する
  // プログラムで指定しない限り-1 ~ 2の値しか来ない
  pPhotoParam->pokeEyeAnmPattern1 = m_pPokeObj->GetPokeModel()->GetEyeState(0) + 1;         // POKE_ANIME_SLOT_EYE1
  pPhotoParam->pokeEyeAnmPattern2 = m_pPokeObj->GetPokeModel()->GetEyeState(1) + 1;         // POKE_ANIME_SLOT_EYE2
  pPhotoParam->pokeEyeAnmPattern3 = m_pPokeObj->GetPokeModel()->GetEyeState(2) + 1;         // POKE_ANIME_SLOT_EYE3
  // 口アニメはプログラムで制御しない限り動かないので取得しない
  //pPhotoParam->pokeMouthAnmpattern1 = m_pPokeObj->GetPokeModel()->GetMouseState(0);     // POKE_ANIME_SLOT_MOUTH1
  //pPhotoParam->pokeMouthAnmpattern2 = m_pPokeObj->GetPokeModel()->GetMouseState(1);     // POKE_ANIME_SLOT_MOUTH2
  //pPhotoParam->pokeMouthAnmpattern3 = m_pPokeObj->GetPokeModel()->GetMouseState(2);     // POKE_ANIME_SLOT_MOUTH3

  pPhotoParam->pokeLoopAnmFrame1 = m_PrevParam.loopFrame1;    // POKE_ANIME_SLOT_LOOP1
  pPhotoParam->pokeLoopAnmFrame2 = m_PrevParam.loopFrame2;    // POKE_ANIME_SLOT_LOOP2
  pPhotoParam->pokeLoopAnmFrame3 = m_PrevParam.loopFrame3;    // POKE_ANIME_SLOT_LOOP3
  pPhotoParam->pokeLoopAnmFrame4 = m_PrevParam.loopFrame4;    // POKE_ANIME_SLOT_LOOP4

  // 座標・角度
  pPhotoParam->pokePosX = m_Pos.x * 100;
  pPhotoParam->pokePosZ = m_Pos.z * 100;
  pPhotoParam->pokeDegY = m_DegY;
}

/// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
f32 FinderStudioPokemon::getPrevAnimationFrame(const poke_3d::model::BaseModel* pModel,u32 slotIndex) const
{
  GFL_ASSERT_STOP(pModel);

  f32 stepFrame = pModel->GetAnimationStepFrame(slotIndex);
  int frame = pModel->GetAnimationFrame(slotIndex) - stepFrame;
  if(frame < 0) frame = pModel->GetAnimationEndFrame(slotIndex);

  return frame;
}

/// @brief 撮影モード切替時処理
void FinderStudioPokemon::OnChangeCaptureMode(CaptureMode captureMode)
{
  m_CaptureMode = captureMode;
}

/// @brief 指定した距離で現在の構図に配置できるか
/// @param distance トレーナーとの距離
/// @return 配置できるか
bool FinderStudioPokemon::IsCanComposit(f32 distance)
{
  return this->IsCanComposit(m_Composition,distance);
}

/// @brief 指定した距離で構図に配置できるか
/// @param composition 構図の種類
/// @param distance トレーナーとの距離
/// @return 配置できるか
bool FinderStudioPokemon::IsCanComposit(Composition composition,f32 distance)
{
  gfl2::math::Vector3 pos;
  f32 degY;
  this->getCompositionPosDegY(&pos,&degY,composition,distance,m_NowMotionType);
  if(this->isCanMovePos(pos,degY,false,m_NowMotionType))
  {
    return true;
  }
  else
  {
    return false;
  }
}

/// @brief 構図用パラメータ適用
/// @param composition 構図の種類
/// @param distance トレーナーとの距離
void FinderStudioPokemon::ApplyComposition(Composition composition,u32 distance)
{
  m_Composition = composition;
  m_CompositionDistance = distance;
}

/// @brief ポケモンモデル再ロード（ポケモンパラメータは内部のものを使用）
/// @param isDebug デバッグからの呼び出しか
void FinderStudioPokemon::StartReloadModel(bool isDebug)
{
  this->StartReloadModel(m_PokeSimpleParam,isDebug);
}

/// @brief ポケモンモデル再ロード
/// @param pokeSimpleParam ポケモンデータ
void FinderStudioPokemon::StartReloadModel(const PokeTool::SimpleParam& pokeSimpleParam,bool isDebug)
{
  // デバッグからの呼び出しならASSERTで止めない
  if(isDebug)
  {
    // メイン処理のときしかリロード開始不可
    if(m_MainSeq != SEQ_MAIN_MAIN)
    {
      return;
    }
  }
  else
  {
    // メイン処理のときしかリロード開始不可
    if(m_MainSeq != SEQ_MAIN_MAIN)
    {
      GFL_ASSERT_STOP_MSG(0,"まだ読み込みが完了していません！\n");
      return;
    }
  }

  m_PokeSimpleParam = pokeSimpleParam;

  // メイン処理を削除へ
  m_MainSeq = SEQ_MAIN_WAIT;
}

/// @brief 再ロードが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioPokemon::IsFinishReloadModel() const
{
  if(m_MainSeq == SEQ_MAIN_MAIN)
  {
    return true;
  }

  return false;
}

/// @brief 時間帯によるカラー変更
/// @param timeZone 時間帯
/// @note 仕様削除
void FinderStudioPokemon::SetConstantColorByTime(TimeZone timeZone)
{
  static const PokeTool::POKE_ENV_TYPE PokeEnvType[3] = {
    PokeTool::POKE_ENV_NONE,
    PokeTool::POKE_ENV_EVENING,
    PokeTool::POKE_ENV_NIGHT
  };

  // 仕様削除
  //m_pPokeObj->GetPokeModel()->SetEnvColor(PokeEnvType[timeZone]);
}

/// @brief ライト方向変更
/// @param lightDir ライト方向
void FinderStudioPokemon::SetLightDirection(const gfl2::math::Vector3& lightDir)
{
  // 読み込み中は変更しない（ハング防止）
  if(m_MainSeq == SEQ_MAIN_MAIN)
  {
    m_pPokeObj->GetPokeModel()->SetLightDirection(lightDir);
  }
}

/// @brief モーション再生
void FinderStudioPokemon::PlayMotion()
{
  this->request(REQ_TYPE_PLAY_MOTION);
}

/// @brief モーション再生
/// @return 再生に成功したかどうか
/// @note 仕様にあるモーションリストの中からランダムに再生する
bool FinderStudioPokemon::playMotion()
{
  bool isCanPlay = false;
  switch(m_NowMotionType)
  {
  case POKE_MOTION_TYPE_BATTLE:
    isCanPlay = this->playMotionBattle();
    break;

  case POKE_MOTION_TYPE_KAWAIGARI:
    isCanPlay = this->playMotionKawaigari();
    break;

  default:
    GFL_ASSERT(0);  // 来ないはず
    break;
  }

  return isCanPlay;
}

/// @brief バトルモーション再生
/// @return 再生に成功したかどうか
/// @note 仕様にあるバトルのモーションリストの中からランダムに再生する
bool FinderStudioPokemon::playMotionBattle()
{
  // 再生できるモーションかどうか判定
  u32 numAvailableMotion = 0;
  PokeTool::MODEL_ANIME isAvailableMotionList[NUM_USE_MOTION_BATTLE] = {}; // とりあえず０初期化
  for(u32 i = 0;i < NUM_USE_MOTION_BATTLE;i++)
  {
    if(this->isCanPlayMotion(POKE_MOTION_TYPE_BATTLE,BATTLE_MOTION_TABLE[i]))
    {
      // アニメーション利用可能
      if(numAvailableMotion < NUM_USE_MOTION_BATTLE)   // 念のためチェック
      {
        // カウント用変数を添え字に使用
        isAvailableMotionList[numAvailableMotion] = BATTLE_MOTION_TABLE[i];
      }
      else GFL_ASSERT(0);
      numAvailableMotion++;
    }
  }
   
  if(numAvailableMotion == 0)
  {
    GFL_PRINT("再生できるバトルモーションがありません\n");
    return false;
  }

  // 再生できるモーションの中からランダムに再生
  int rnd = System::GflUse::GetPublicRand(numAvailableMotion);
  if(isAvailableMotionList[rnd] == PokeTool::MODEL_ANIME_BT_WAIT_A) GFL_ASSERT(0);  // 待機になるのはおかしい
  return this->changeAnimation(POKE_MOTION_TYPE_BATTLE,isAvailableMotionList[rnd]);
}

/// @brief かわいがりモーション再生
/// @return 再生に成功したかどうか
/// @note 仕様にあるかわいがりのモーションリストの中からランダムに再生する
bool FinderStudioPokemon::playMotionKawaigari()
{
  // 再生できるモーションかどうか判定
  u32 numAvailableMotion = 0;
  PokeTool::MODEL_ANIME isAvailableMotionList[NUM_USE_MOTION_KAWAIGARI] = {}; // とりあえず０初期化
  for(u32 i = 0;i < NUM_USE_MOTION_KAWAIGARI;i++)
  {
    if(this->isCanPlayMotion(POKE_MOTION_TYPE_KAWAIGARI,KAWAIGARI_MOTION_TABLE[i]))
    {
      // アニメーション利用可能
      if(numAvailableMotion < NUM_USE_MOTION_KAWAIGARI)   // 念のためチェック
      {
        // カウント用変数を添え字に使用
        isAvailableMotionList[numAvailableMotion] = KAWAIGARI_MOTION_TABLE[i];
      }
      else GFL_ASSERT(0);
      numAvailableMotion++;
    }
  }
  
  if(numAvailableMotion == 0)
  {
    GFL_PRINT("再生できるかわいがりモーションがありません\n");
    return false;
  }

  // 再生できるモーションの中からランダムに再生
  int rnd = System::GflUse::GetPublicRand(numAvailableMotion);
  if(isAvailableMotionList[rnd] == PokeTool::MODEL_ANIME_KW_WAIT) GFL_ASSERT(0);  // 待機になるのはおかしい
  return this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,isAvailableMotionList[rnd]);
}

/// @brief モーションが終わったかどうか
bool FinderStudioPokemon::IsFinishMotion() const
{
  if(m_State == POKE_STATE_WAIT) return true;
  
  return false;
}

/// @brief モーションタイプ変更がリセットせずに行えるかどうか
/// @param motionType モーションタイプ
/// @return true 押し出しのみで可能 false リセットしないと不可能
bool FinderStudioPokemon::IsCanChangeMotionTypeNoReset(PokeMotionType motionType)
{
  // 変更後の座標を取得
  gfl2::math::Vector3 pos = this->getPosChangeMotionType(motionType);

  // 移動できるか判定
  return this->isCanMovePos(pos,m_DegY,false,motionType);
}

/// @brief モーションタイプ変更後の座標を取得
gfl2::math::Vector3 FinderStudioPokemon::getPosChangeMotionType(PokeMotionType motionType) const
{
  gfl2::math::Vector3 pos = m_Pos;

  // ポケモンのみなら変更はない
  if(!m_pPlayerAABB) return pos;

  // 変更先の待機モーションとトレーナーがぶつかるかどうか判定
  gfl2::math::AABB waitAABB = m_PokeAABB.GetData(motionType,PokeTool::MODEL_ANIME_APP_WAIT);
  this->applyAABB(&waitAABB);
  if(m_pPlayerAABB->IsCross(waitAABB))
  {
    // ぶつかるので押し出し

    // 中心で比べてどっちよりか
    enum{ PUSH_LEFT,PUSH_RIGHT } pushLR;
    {
      f32 prCenterX = (m_pPlayerAABB->GetMax().x + m_pPlayerAABB->GetMin().x) / 2;
      f32 pkCenterX = (waitAABB.GetMax().x + waitAABB.GetMin().x) / 2;
      if(prCenterX > pkCenterX) pushLR = PUSH_RIGHT;
      else pushLR = PUSH_LEFT;
    }
    enum{ PUSH_FRONT,PUSH_BACK } pushFB;
    {
      f32 prCenterZ = (m_pPlayerAABB->GetMax().z + m_pPlayerAABB->GetMin().z) / 2;
      f32 pkCenterZ = (waitAABB.GetMax().z + waitAABB.GetMin().z) / 2;
      if(prCenterZ > pkCenterZ) pushFB = PUSH_BACK;
      else pushFB = PUSH_FRONT;
    }

    // めり込んだ長さを求める
    f32 lengthX = 0;
    if(pushLR == PUSH_RIGHT)
    {
      lengthX = gfl2::math::Abs(waitAABB.GetMax().x - m_pPlayerAABB->GetMin().x);
    }
    else if(pushLR == PUSH_LEFT)
    {
      lengthX = gfl2::math::Abs(waitAABB.GetMin().x - m_pPlayerAABB->GetMax().x);
    }
    else GFL_ASSERT(0);

    f32 lengthZ = 0;
    if(pushFB == PUSH_BACK)
    {
      lengthZ = gfl2::math::Abs(waitAABB.GetMax().z - m_pPlayerAABB->GetMin().z);
    }
    else if(pushFB == PUSH_FRONT)
    {
      lengthZ = gfl2::math::Abs(waitAABB.GetMin().z - m_pPlayerAABB->GetMax().z);
    }
    else GFL_ASSERT(0);

    // １余白をあけておく
    lengthX += 1;
    lengthZ += 1;

    // めり込んだ長さが短い方に押し出す
    if(lengthX < lengthZ)
    {
      if(pushLR == PUSH_RIGHT) pos.x -= lengthX;
      else if(pushLR == PUSH_LEFT) pos.x += lengthX;
      else GFL_ASSERT(0);
    }
    else
    {
      if(pushFB == PUSH_BACK) pos.z -= lengthZ;
      else if(pushFB == PUSH_FRONT) pos.z += lengthZ;
      else GFL_ASSERT(0);
    }
  }

  return pos;
}

/// @brief モーションタイプ変更
/// @param motionType モーションタイプ
/// @return 現在再生しているタイプと違うタイプかどうか
/// @note その場で待機モーションも変更させる
bool FinderStudioPokemon::ChangeMotionType(PokeMotionType motionType)
{
  if(m_NowMotionType == motionType) return false;

  // 変更した結果の座標を取得
  gfl2::math::Vector3 pos = this->getPosChangeMotionType(motionType);

  // 丸める
  pos = this->roundPosition(pos);


  // 押し出せる場合のみ座標移動
  if(this->isCanMovePos(pos,m_DegY,false,motionType))
  {
    this->requestPos(pos);
  }
  else
  {
    this->ResetPositionRotate(motionType);
  }

  // リセットでもリクエストはしておく
  // 全体リセット処理は外部から呼んでもらう
  this->requestMotionType(motionType);
  this->request(REQ_TYPE_CHANGE_MOTION_TYPE);

  return true;
}
/// @brief モーションタイプ変更
/// @param motionType モーションタイプ
/// @param isReset モーションタイプ変更時にリセットを行うか
/// @return 現在再生しているタイプと違うタイプかどうか
/// @note その場で待機モーションも変更させる
bool FinderStudioPokemon::ChangeMotionType(PokeMotionType motionType,bool isReset)
{
  if(m_NowMotionType == motionType) return false;

  // 変更した結果の座標を取得
  gfl2::math::Vector3 pos = this->getPosChangeMotionType(motionType);

  // 丸める
  pos = this->roundPosition(pos);

  // リセットするか押し出しか分岐させる
  if(isReset)
  {
    this->ResetPositionRotate(motionType);
  }
  else
  {
    // 念のためチェック
    if(this->isCanMovePos(pos,m_DegY,false,motionType))
    {
      this->requestPos(pos);
    }
    else
    {
      GFL_ASSERT(0);
      return true;
    }
  }

  // リセットでもリクエストはしておく
  // 全体リセット処理は外部から呼んでもらう
  this->requestMotionType(motionType);
  this->request(REQ_TYPE_CHANGE_MOTION_TYPE);

  return true;
}

/// @brief モーションタイプ変更が完了したか
/// @return 完了したか
bool FinderStudioPokemon::IsFinishChangeMotionType() const
{
//   if(m_NowAnimationNo == PokeTool::MODEL_ANIME_BT_WAIT_A ||
//     m_NowAnimationNo == PokeTool::MODEL_ANIME_KW_WAIT)
//   {
//     return true;
//   }
// 
//   return false;

  // 現状ポケモンのモーションタイプ変更を待つ処理がないので常にtrue
  // モーションタイプ変更リクエストを呼んだ後少なくとも１フレームたってから
  // この判定関数を呼ぶことを福島さんに確認済み

  return true;
}

/// @brief ポケモンが現在再生できるモーション数の割合を取得
/// @return 再生できるモーション割合(0 ~ 1)
f32 FinderStudioPokemon::GetRateCanPlayMotion() const
{
  u32 numCanPlayMotionMax = this->getNumCanPlayMotionMax(m_NowMotionType);
  u32 numCanPlayMotion = this->getNumCanPlayMotion(m_NowMotionType);

  // めり込み関係なく再生できるモーションが１つでもあるかチェック
  if(numCanPlayMotionMax == 0)
  {
    GFL_ASSERT_MSG(0,"めり込みに関係なく再生できるモーションがありません！\n");
    return 0;
  }

  // 念のため
  if(numCanPlayMotionMax < numCanPlayMotion) GFL_ASSERT_STOP(0);

  return numCanPlayMotion / static_cast<f32>(numCanPlayMotionMax);
}

/// @brief 現在再生できるモーション数を取得
/// @return 再生できるモーション数
u32 FinderStudioPokemon::GetNumCanPlayMotion() const
{
  return this->getNumCanPlayMotion(m_NowMotionType);
}

/// @brief 自動でモーションするかを切り替える
/// @param true 自動モーションオン  false 自動モーションオフ
void FinderStudioPokemon::SetEnableAutoMotion(bool isEnableAutoMotion)
{
  m_IsEnableAutoMotion = isEnableAutoMotion;
}

/// @brief ポケモン位置取得
/// @return 3D座標から算出した2D（下画面）でのポケモン座標
gfl2::math::Vector2 FinderStudioPokemon::GetPokePositionScreen() const
{
  // 2D→3Dとは逆の計算を行う
  gfl2::math::Vector3 pos = m_Pos;

  // カメラY角度で回転
  if(m_pCameraSystem)
  {
    FinderStudioCameraSystem::RotateYVec(&pos, -(m_pCameraSystem->GetDegY() + 180) );
  }

  // 2D座標に変換
  gfl2::math::Vector2 screenPos(-pos.x,-pos.z);

  // 3D座標での移動可能範囲
  const f32 MOVE_RANGE = m_MoveRange;
  // 3D→2Dの倍率
  f32 pow = Static::UI_MOVE_RADIUS / MOVE_RANGE;
  screenPos *= pow;

  // 移動の中心は画面の中心
  gfl2::math::Vector2 centerXY(DISPLAY_LOWER_WIDTH/2,DISPLAY_LOWER_HEIGHT/2);

  // 中心からのベクトル
  screenPos = centerXY + screenPos;

  return screenPos;
}

/// @brief ポケモン位置変更
/// @param lowerPosition 2D（下画面）でのポケモン座標
/// @param isLift 持ち上げるかどうか falseで下ろす
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioPokemon::SetPokePosition(const gfl2::math::Vector2& lowerPosition,bool isLift)
{
  // 移動の中心は画面の中心
  gfl2::math::Vector2 centerXY(DISPLAY_LOWER_WIDTH/2,DISPLAY_LOWER_HEIGHT/2);

  // 中心からのベクトル
  gfl2::math::Vector2 toVecXY = lowerPosition - centerXY;

  // 3D座標での移動可能範囲
  const f32 MOVE_RANGE = m_MoveRange;
  // 2D→3Dの倍率
  f32 pow = MOVE_RANGE / Static::UI_MOVE_RADIUS;
  toVecXY *= pow;

  // 3D座標に変換
  gfl2::math::Vector3 toPos(-toVecXY.x,m_Pos.y,-toVecXY.y);
  // カメラY角度で回転
  if(m_pCameraSystem)
  {
    FinderStudioCameraSystem::RotateYVec(&toPos,m_pCameraSystem->GetDegY() + 180);
  }

  // 丸める
  toPos = this->roundPosition(toPos);

  //この時点での座標を保存
  gfl2::math::Vector3 prevPos = m_Pos;
  // 判定のために一時的に座標をセット
  m_Pos.Set(toPos);

  // 衝突判定
  bool isHit = false;
  if(this->IsHitPokemonToTrainerUI())
  {
    isHit = true;
  }

  // 座標を戻す
  m_Pos.Set(prevPos);

  // 衝突しなければ座標をリクエスト
  if(!isHit) this->requestPos(toPos);
  
  return isHit;
}

/// @brief ポケモン角度変更（左回り）
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioPokemon::RotateLeft()
{
  static const float ROTATE_LEFT_DEG = -3;
  return this->SetRotate(m_IgnoreCollisionDegY + ROTATE_LEFT_DEG);
}
/// @brief ポケモン角度変更（右回り）
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioPokemon::RotateRight()
{
  static const float ROTATE_RIGHT_DEG = 3;
  return this->SetRotate(m_IgnoreCollisionDegY + ROTATE_RIGHT_DEG);
}

/// @brief 角度をセット
/// @param degY Y軸角度(0 <= degY < 360)
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioPokemon::SetRotate(float degY)
{
  GFL_ASSERT_STOP(abs(degY) < 720.0f);

  // 変更前の角度を保存
  f32 prevDegY = m_IgnoreCollisionDegY;

  // 0 ~ 360度に
  if(degY < 0) degY += 360.0f;
  if(degY >= 360.0f) degY -= 360.0f;

  // 丸める
  degY = this->roundDegree(degY);

  // 判定のために一時的に角度を変更
  m_IgnoreCollisionDegY = degY;

  // 衝突判定
  bool isHit = false;
  if(this->IsHitPokemonToTrainerUI())
  {
    isHit = true;
  }

  // 角度を戻す
  m_IgnoreCollisionDegY = prevDegY;

  // 当たっていなければ角度をリクエスト
  if(!isHit) this->requestDegY(degY);

  // UI上の角度は常にリクエスト
  this->requestIgnoreCollisionDegY(degY);

  return isHit;
}

/// @brief ポケモンがトレーナーにぶつかっているか判定
/// @return ぶつかっているかどうか
/// @note UI上の角度で判定（ビューワ上でぶつかることはありえないため）
bool FinderStudioPokemon::IsHitPokemonToTrainerUI() const
{
  // 衝突判定
  bool isHit = false;
  gfl2::math::AABB nowAABB = this->GetAABBIgnoreCollision();

  if(this->isHitMotionAABB(nowAABB))
  {
    isHit = true;
  }

  return isHit;
}

/// @brief 当たり判定を無視したAABBを取得
/// @return AABB
/// @note 待機モーションマージ済み
const gfl2::math::AABB FinderStudioPokemon::GetAABBIgnoreCollision() const
{
  // 待機モーションが再生できないとまずいため今のモーションと待機モーションのAABBをマージしたものを返す
  gfl2::math::AABB nowAABB = m_PokeAABB.GetData(m_NowMotionType,m_NowAnimationNo);
  this->applyIgnoreCollisionAABB(&nowAABB);
  gfl2::math::AABB waitAABB = m_PokeAABB.GetData(m_NowMotionType,0);
  this->applyIgnoreCollisionAABB(&waitAABB);
  nowAABB = nowAABB.Merge(waitAABB);

  return nowAABB;
}


#if PM_DEBUG
// 一時停止
void FinderStudioPokemon::Shot()
{
  m_DebugIsShot = true;
  // モーションストップ
  m_pPokeObj->SetAnimationSpeed(0);
}

// 一時停止解除
void FinderStudioPokemon::FreeShot()
{
  m_DebugIsShot = false;
  // アニメーションスピードを戻す
  m_pPokeObj->SetAnimationSpeed(1);
}
#endif

/// @brief アニメーション変更関数
/// @param type アニメーションタイプ
/// @param anime アニメーションNo
/// @param isForce 同じモーションだったときに最初から再生するか
/// @return true:変更成功 false:変更失敗
bool FinderStudioPokemon::changeAnimation(PokeMotionType type,PokeTool::MODEL_ANIME anime,bool isForce)
{
#if PM_DEBUG
  if(m_DebugIsEnableMotionBlend)
  {
    // オンならこっちつかう
    return changeAnimationSmooth(type,anime);
  }
#endif

  // 同じならアニメーション変更させない
  if(!isForce)
  {
    if(type == m_NowMotionType &&
      anime == m_NowAnimationNo)
    {
      return false;
    }
  }

  // 再生できるかどうかチェック
  if(!this->isCanPlayMotion(type,anime))
  {
    GFL_PRINT("再生できないモーションです\n");
    return false;
  }

  switch(type)
  {
  case POKE_MOTION_TYPE_BATTLE:
    m_pPokeObj->ChangeAnimation(anime,isForce);
    break;

  case POKE_MOTION_TYPE_KAWAIGARI:
    m_pPokeObj->ChangeAnimationResource(m_pKawaigariAnimationResourceNode[anime]);
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
  
  m_NowMotionType = type;
  m_NowAnimationNo = anime;

  return true;
}
bool FinderStudioPokemon::changeAnimationSmooth(PokeMotionType type,PokeTool::MODEL_ANIME anime,int smoothFrame)
{
  // 再生できるかどうかチェック
  if(!this->isCanPlayMotion(type,anime))
  {
    GFL_PRINT("再生できないモーションです\n");
    return false;
  }

  switch(type)
  {
  case POKE_MOTION_TYPE_BATTLE:
    m_pPokeObj->GetPokeModel()->ChangeAnimationSmooth(anime,smoothFrame,true);
    break;

  case POKE_MOTION_TYPE_KAWAIGARI:
    m_pPokeObj->GetPokeModel()->ChangeAnimationSmoothByResourceNode(m_pKawaigariAnimationResourceNode[anime],smoothFrame);
    break;

  default:
    GFL_ASSERT(0);
    break;
  }
  
  m_NowMotionType = type;
  m_NowAnimationNo = anime;

  return true;
}

/// @brief 現在のアニメーションタイプに応じた待機モーションを再生
/// @param isForce 同じモーションだった場合に１フレーム目から再生するか
/// @return 再生に成功したか
bool FinderStudioPokemon::changeAnimationWait(PokeMotionType motionType,bool isForce)
{
  PokeTool::MODEL_ANIME anmNo = static_cast<PokeTool::MODEL_ANIME>(0);
  switch(motionType)
  {
  case POKE_MOTION_TYPE_BATTLE:
    anmNo = PokeTool::MODEL_ANIME_BT_WAIT_A;
    break;
  case POKE_MOTION_TYPE_KAWAIGARI:
    anmNo = PokeTool::MODEL_ANIME_KW_WAIT;
    break;

  default:
    GFL_ASSERT(0);
    break;
  }

  bool isPlay = this->changeAnimation(motionType,anmNo,isForce);

  return isPlay;
}

/// @brief AABBに回転・移動・許容量を反映
/// @param [out] 反映させるAABB
/// @param isApplyScale 実物大→バトル用のスケールを反映させるか（デフォルトはオン）
void FinderStudioPokemon::applyAABB(gfl2::math::AABB* AABB) const
{
  if(!AABB)
  {
    GFL_ASSERT(AABB);
    return;
  }

  // 回転反映
  gfl2::math::Matrix mRot;
  gfl2::math::Quaternion qua;
  qua.RadianYToQuaternion(gfl2::math::ConvDegToRad(m_DegY));
  qua.QuaternionToMatrix(mRot);
  (*AABB) = (*AABB).Transform(mRot);
  // 座標反映
  (*AABB) = (*AABB).Offset(m_Pos);
  // 許容量反映
  gfl2::math::Vector4 min = (*AABB).GetMin();
  gfl2::math::Vector4 max = (*AABB).GetMax();
  // 最低保障サイズ
  static const int MIN_SIZE = 1;
  // 実際のサイズ
  f32 realSizeX = -(min.x + m_CollisionPermit) + (max.x - m_CollisionPermit);
  f32 realSizeZ = -(min.z + m_CollisionPermit) + (max.z - m_CollisionPermit);
  f32 realCollisionPermit = m_CollisionPermit;
  if(realSizeX < MIN_SIZE)
  {
    realCollisionPermit -= (MIN_SIZE - realSizeX) / 2.0f;
  }
  min.x += realCollisionPermit;
  max.x -= realCollisionPermit;

  realCollisionPermit = m_CollisionPermit;
  if(realSizeZ < MIN_SIZE)
  {
    realCollisionPermit -= (MIN_SIZE - realSizeZ) / 2.0f;
  }
  min.z += realCollisionPermit;
  max.z -= realCollisionPermit;

  (*AABB).SetMinMax(min,max);
}

/// @brief AABBに回転・移動・許容量を反映（回転角度は当たり判定を無視したもの）
/// @param [out] 反映させるAABB
/// @param isApplyScale 実物大→バトル用のスケールを反映させるか（デフォルトはオン）
void FinderStudioPokemon::applyIgnoreCollisionAABB(gfl2::math::AABB* AABB) const
{
  if(!AABB)
  {
    GFL_ASSERT(AABB);
    return;
  }

  // 回転反映
  gfl2::math::Matrix mRot;
  gfl2::math::Quaternion qua;
  qua.RadianYToQuaternion(gfl2::math::ConvDegToRad(m_IgnoreCollisionDegY));
  qua.QuaternionToMatrix(mRot);
  (*AABB) = (*AABB).Transform(mRot);
  // 座標反映
  (*AABB) = (*AABB).Offset(m_Pos);
  // 許容量反映
  gfl2::math::Vector4 min = (*AABB).GetMin();
  gfl2::math::Vector4 max = (*AABB).GetMax();
  // 最低保障サイズ
  static const int MIN_SIZE = 1;
  // 実際のサイズ
  f32 realSizeX = -(min.x + m_CollisionPermit) + (max.x - m_CollisionPermit);
  f32 realSizeZ = -(min.z + m_CollisionPermit) + (max.z - m_CollisionPermit);
  f32 realCollisionPermit = m_CollisionPermit;
  if(realSizeX < MIN_SIZE)
  {
    realCollisionPermit -= (gfl2::math::Abs(realSizeX) + MIN_SIZE) / 2.0f;
  }
  min.x += realCollisionPermit;
  max.x -= realCollisionPermit;

  realCollisionPermit = m_CollisionPermit;
  if(realSizeZ < MIN_SIZE)
  {
    realCollisionPermit -= (gfl2::math::Abs(realSizeZ) + MIN_SIZE) / 2.0f;
  }
  min.z += realCollisionPermit;
  max.z -= realCollisionPermit;

  (*AABB).SetMinMax(min,max);
}

/// @brief AABBに拡大・回転・移動を反映
/// @param [out] 反映させるAABB
void FinderStudioPokemon::applyAABB(gfl2::math::AABB* AABB,gfl2::math::Vector3 pos,f32 degY,f32 scale) const
{
  if(!AABB)
  {
    GFL_ASSERT(AABB);
    return;
  }
  gfl2::math::Matrix mScale = gfl2::math::Matrix::GetIdentity();
  mScale *= scale;
  mScale.SetElem(3,3,1);
  (*AABB) = (*AABB).Transform(mScale);

  // 回転反映
  gfl2::math::Matrix mRot;
  gfl2::math::Quaternion qua;
  qua.RadianYToQuaternion(gfl2::math::ConvDegToRad(degY));
  qua.QuaternionToMatrix(mRot);
  (*AABB) = (*AABB).Transform(mRot);
  // 座標反映
  (*AABB) = (*AABB).Offset(pos);
}

// ポケモン位置・角度リセット
void FinderStudioPokemon::ResetPositionRotate()
{
  this->ResetPositionRotate(m_NowMotionType);
}

// ポケモン位置・角度リセット
void FinderStudioPokemon::ResetPositionRotate(PokeMotionType motionType)
{
  gfl2::math::Vector3 toPos;
  f32 toDegY = 0;

  // リセット後の座標・角度を取得
  GetResetPosistionRotate(&toPos,&toDegY,motionType);

  // ポケモンAABBを更新
  this->setupAABB();
  // リクエスト
  this->requestPos(toPos);
  this->requestDegY(toDegY);
  this->requestIgnoreCollisionDegY(toDegY);

  // モーションタイプチェンジのリクエストが来ていたらそちら優先
  if(m_RequestType != REQ_TYPE_CHANGE_MOTION_TYPE)
  {
    this->request(REQ_TYPE_RESET);
  }
}

/// @brief ポケモンリセット位置・角度取得
/// @param[out] pPos 座標
/// @param[out] pDegY Y軸角度
/// @param[out] motionType モーションタイプ
void FinderStudioPokemon::GetResetPosistionRotate(gfl2::math::Vector3* pPos,f32* pDegY,PokeMotionType motionType)
{
  if(pPos == NULL || pDegY == NULL)
  {
    GFL_ASSERT(0);
    return;
  }

  // セットしていた構図にリセットする
  if(m_CaptureMode == CAPTURE_MODE_BOTH)
  {
    // 座標と角度を取得
    this->getCompositionPosDegY(pPos,pDegY,m_Composition,m_CompositionDistance,motionType);
  }
  else if(m_CaptureMode == CAPTURE_MODE_POKE)
  {
    // 中心
    (*pPos).Set(0,0,0);
    // 正面を向く
    (*pDegY) = 180;
  }
  else
  {
    GFL_ASSERT(0);   // ここにはこないはず
  }

  // 丸める
  (*pPos) = this->roundPosition((*pPos));
  (*pDegY) = this->roundDegree((*pDegY));

  // 移動可能チェック（現在のモーションタイプの待機モーションでチェック）
  if(!this->isCanMovePos((*pPos),(*pDegY),false,motionType,PokeTool::MODEL_ANIME_BT_WAIT_A))
  {
#if PM_DEBUG
    GFL_ASSERT_MSG(0,"構図を適用しましたが、移動不可能です\n");
#else
    GFL_ASSERT_STOP_MSG(0,"構図を適用しましたが、移動不可能です\n");
#endif
  }
}

// 表示・非表示切り替え
void FinderStudioPokemon::SetVisible(bool isVisible)
{
  m_pPokeObj->SetVisible(isVisible);
}

/// @brief 指定したタイプで現在再生できるモーション数を取得
/// @return 再生できるモーション数
u32 FinderStudioPokemon::getNumCanPlayMotion(PokeMotionType type) const
{
  u32 numCanPlayMotion = 0;

  switch(type)
  {
  case POKE_MOTION_TYPE_BATTLE:
    for(u32 i = 0;i < NUM_USE_MOTION_BATTLE;i++)
    {
      PokeTool::MODEL_ANIME motionNo = BATTLE_MOTION_TABLE[i];
      bool isCanPlay = this->isCanPlayMotion(POKE_MOTION_TYPE_BATTLE,motionNo);
      if(isCanPlay) numCanPlayMotion++;
    }
    break;
  case POKE_MOTION_TYPE_KAWAIGARI:
    for(u32 i = 0;i < NUM_USE_MOTION_KAWAIGARI;i++)
    {
      PokeTool::MODEL_ANIME motionNo = KAWAIGARI_MOTION_TABLE[i];
      bool isCanPlay = this->isCanPlayMotion(POKE_MOTION_TYPE_KAWAIGARI,motionNo);
      if(isCanPlay) numCanPlayMotion++;
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  return numCanPlayMotion;
}

/// @brief 指定したタイプで現在再生できるモーション数の最大を取得
/// @return 再生できるモーション数の最大
/// @note 存在しないモーション、データテーブルで使用不可に指定されたモーションは除く
u32 FinderStudioPokemon::getNumCanPlayMotionMax(PokeMotionType type) const
{
  u32 numCanPlayMotionMax = 0;

  switch(type)
  {
  case POKE_MOTION_TYPE_BATTLE:
    for(u32 i = 0;i < NUM_USE_MOTION_BATTLE;i++)
    {
      PokeTool::MODEL_ANIME motionNo = BATTLE_MOTION_TABLE[i];
      bool isCanPlay = true;
      if(!this->isCanPlayMotionExist(POKE_MOTION_TYPE_BATTLE,motionNo)) isCanPlay = false;
      if(!this->isCanPlayMotionDataTable(POKE_MOTION_TYPE_BATTLE,motionNo)) isCanPlay = false;
      if(isCanPlay) numCanPlayMotionMax++;
    }
    break;
  case POKE_MOTION_TYPE_KAWAIGARI:
    for(u32 i = 0;i < NUM_USE_MOTION_KAWAIGARI;i++)
    {
      PokeTool::MODEL_ANIME motionNo = KAWAIGARI_MOTION_TABLE[i];
      bool isCanPlay = true;
      if(!this->isCanPlayMotionExist(POKE_MOTION_TYPE_KAWAIGARI,motionNo)) isCanPlay = false;
      if(!this->isCanPlayMotionDataTable(POKE_MOTION_TYPE_KAWAIGARI,motionNo)) isCanPlay = false;
      if(isCanPlay) numCanPlayMotionMax++;
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }

  return numCanPlayMotionMax;
}

/// @brief 指定したモーションが再生できるかどうか
bool FinderStudioPokemon::isCanPlayMotion(PokeMotionType type,PokeTool::MODEL_ANIME anime) const
{
  // データテーブルでの使用可否判定
  if(!this->isCanPlayMotionDataTable(type,anime))
  {
    return false;
  }

  // モーションが存在するかチェック
  if(!this->isCanPlayMotionExist(type,anime))
  {
    return false;
  }
  
  // トレーナーにめり込んでしまわないかチェック
  if(!this->isCanPlayMotionHit(type,anime))
  {
    return false;
  }

  return true;
}

/// @brief データテーブルでのモーション使用可否判定
bool FinderStudioPokemon::isCanPlayMotionDataTable(PokeMotionType type,PokeTool::MODEL_ANIME anime) const
{
  if(m_pPokeDataTable)
  {
    if(!m_pPokeDataTable->IsCanUseMotion(m_PokeSimpleParam.monsNo,m_PokeSimpleParam.formNo,type,anime))
    {
      return false;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"ポケモンデータテーブルがありません\n");
    return false;
  }

  return true;
}
/// @brief モーションが存在するかどうか判定
bool FinderStudioPokemon::isCanPlayMotionExist(PokeMotionType type,PokeTool::MODEL_ANIME anime) const
{
  switch(type)
  {
  case POKE_MOTION_TYPE_BATTLE:
    if(!m_pPokeObj->GetPokeModel() || !m_pPokeObj->GetPokeModel()->IsAvailableAnimation(anime))
    {
      return false;
    }
    break;

  case POKE_MOTION_TYPE_KAWAIGARI:
    if(anime < PokeTool::MODEL_ANIME_KAWAIGARI_NUM)
    {
      if(!m_pKawaigariAnimationResourceNode[anime])
      {
        return false;
      }
    }
    else
    {
      GFL_ASSERT(0);
      return false;
    }
    break;

  default:
    GFL_ASSERT(0);
    return false;
  }

  return true;
}
/// @brief モーションがトレーナーにめり込むかどうか判定
bool FinderStudioPokemon::isCanPlayMotionHit(PokeMotionType type,PokeTool::MODEL_ANIME anime) const
{
  gfl2::math::AABB origAABB = m_PokeAABB.GetData(type,anime);
  this->applyAABB(&origAABB);
  if(this->isHitMotionAABB(origAABB))
  {
    return false;
  }

  return true;
}

/// @brief ポケモンがポーズするときにそのモーションのAABBがめり込んでいないかチェック
bool FinderStudioPokemon::isHitMotionAABB(gfl2::math::AABB& AABB) const
{
  if(m_pPlayerAABB)
  {
    return m_pPlayerAABB->IsCross(AABB);
  }

  return false;
}

/// @brief 指定した構図、距離での座標、角度を取得
/// @param [out] pos 取得した座標
/// @param [out] degY 取得した角度
/// @param composition 構図
/// @param distance 距離
void FinderStudioPokemon::getCompositionPosDegY(gfl2::math::Vector3* pVec,f32* pDegY,Composition composition,int distance,PokeMotionType motionType) const
{
  if(!pVec)
  {
    GFL_ASSERT(0);
    return;
  }
  if(!pDegY)
  {
    GFL_ASSERT(0);
    return;
  }

  switch(composition)
  {
  case COMPOSITION_1:
    // 正面を向く
    (*pDegY) = 180;
    break;
  case COMPOSITION_2:
    // 右を向く
    (*pDegY) = 270;
    break;
  default:
    GFL_ASSERT(0);
    return;
  }

  gfl2::math::AABB tmpAABB = m_PokeAABB.GetData(motionType,0);
  this->applyAABB(&tmpAABB,gfl2::math::Vector3(0,0,0),(*pDegY),1.0f);
  // ぎりぎりにすると重なってる判定になるため余白を作る
  if(m_pPlayerAABB)
  {
    f32 x = m_pPlayerAABB->GetMax().x + -tmpAABB.GetMin().x + 1;
    pVec->Set(x,0,0);
  }
  // 構図パラメータ適応
  pVec->x += distance;
}

/// @brief 指定した座標に移動できるかどうかチェック
/// @param pos 座標
/// @param degY 角度
/// @param isUseNowMotion 現在のモーションで判定するか
/// @param motionType isUseNowMotionがfalseのときに使用
/// @param motionNo isUseNowMotionがfalseのときに使用
bool FinderStudioPokemon::isCanMovePos(
  const gfl2::math::Vector3& pos,
  f32 degY,
  bool isUseNowMotion,
  PokeMotionType motionType,
  PokeTool::MODEL_ANIME motionNo)
{
  PokeMotionType useMotionType;
  PokeTool::MODEL_ANIME useMotionNo;
  if(isUseNowMotion)
  {
    useMotionType = m_NowMotionType;
    useMotionNo = static_cast<PokeTool::MODEL_ANIME>(m_NowAnimationNo);
  }
  else
  {
    useMotionType = motionType;
    useMotionNo = motionNo;
  }

  bool isCanMove = true;

  // プレイヤーとの衝突チェック
  //この時点での座標と角度を保存
  gfl2::math::Vector3 prevPos = m_Pos;
  f32 prevDegY = m_DegY;
  // 判定のために一時的に座標と角度をセット
  m_Pos.Set(pos);
  m_DegY = degY;
  gfl2::math::AABB tmpAABB = m_PokeAABB.GetData(useMotionType,useMotionNo);
  this->applyAABB(&tmpAABB);
  if(this->isHitMotionAABB(tmpAABB))
  {
    isCanMove = false;
  }
  // 座標と角度を戻す
  m_Pos.Set(prevPos);
  m_DegY = prevDegY;

  // 範囲外チェック
  // @fix GFMMCat[76] モーションタイプ変更時、ポケモンがトレーナーにめりこんでしまうことがある
  // 押し出した結果、ポケモンが範囲外の位置になったかの判定が間違っていた
  // 範囲外を許容した結果、UI側とビューア側でポケモンの座標に差異が出ていた
  gfl2::math::Vector3 checkPos = pos;
  checkPos.y = 0;
  f32 len = checkPos.Length();
  if(len > Static::VIEWER_MOVE_RADIUS)
  {
    isCanMove = false;
  }
  /*if(pos.x > Static::VIEWER_MOVE_RADIUS || pos.x < -Static::VIEWER_MOVE_RADIUS ||
    pos.z > Static::VIEWER_MOVE_RADIUS || pos.z < -Static::VIEWER_MOVE_RADIUS)
  {
    isCanMove = false;
  }*/

  return isCanMove;
}

/// @brief 自動再生カウントをクリア
void FinderStudioPokemon::resetAutoMotionCnt(bool isResetSleepCnt)
{
  m_WaitMotionCnt = 0;
  m_WaitMotionCntMax = WAIT_MOTION_FRAME_MIN + System::GflUse::GetPublicRand(WAIT_MOTION_FRAME_MAX - WAIT_MOTION_FRAME_MIN);

  if(isResetSleepCnt) m_SleepMotionCnt = 0;
}

/// @brief アニメーション更新前のパラメータ保存
void FinderStudioPokemon::savePrevParam()
{
  GFL_ASSERT_STOP(m_pPokeObj->GetPokeModel());

  m_PrevParam.mainFrame = m_pPokeObj->GetPokeModel()->GetAnimationFrame();
  m_PrevParam.loopFrame1 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP1);
  m_PrevParam.loopFrame2 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP2);
  m_PrevParam.loopFrame3 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP3);
  m_PrevParam.loopFrame4 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP4);

  // @fix GFMMCat[129] パッチール例外対応
  // パッチールのぶちは固定でセットされているので
  // 前フレームのアニメーション更新を考慮して-1してから取得する
  if(m_PokeSimpleParam.monsNo == MONSNO_PATTIIRU)
  {
    m_PrevParam.loopFrame1 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP1) - 1;
    m_PrevParam.loopFrame2 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP2) - 1;
    m_PrevParam.loopFrame3 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP3) - 1;
    m_PrevParam.loopFrame4 = m_pPokeObj->GetPokeModel()->GetAnimationFrame(PokeTool::POKE_ANIME_SLOT_LOOP4) - 1;
  }
}

/// @brief 座標を写真パラメータで保存できるよう丸める
/// @param position 座標
/// @return 丸めた座標
gfl2::math::Vector3 FinderStudioPokemon::roundPosition(gfl2::math::Vector3 position) const
{
  // 小数点第2位まで保存する

  int x = position.x * 100;
  int z = position.z * 100;
  // Y座標は保存しないのでそのまま

  gfl2::math::Vector3 retPos;
  retPos.x = x / 100.0f;
  retPos.y = position.y;
  retPos.z = z / 100.0f;

  return retPos;
}

#if PM_DEBUG
void FinderStudioPokemon::DebugDraw(gfl2::gfx::CtrDisplayNo displayNo)
{
  // 各種バッファを取得
  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[ displayNo ] );
	gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( System::GflUse::m_HandleList[ displayNo ] );
  // 画面サイズを取得
	u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth( System::GflUse::m_HandleList[ displayNo ] );
	u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( System::GflUse::m_HandleList[ displayNo ] );

  switch( displayNo )
	{
	// 左画面描画
	case gfl2::gfx::CtrDisplayNo::LEFT:

    gfl2::gfx::GFGL::BeginScene();
    gfl2::gfx::GFGL::SetRenderTarget(pBackBuffer);
    gfl2::gfx::GFGL::SetDepthStencil(pDepthBuffer);
	  gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);
	  gfl2::gfx::GFGL::SetScissor(0, 0, displayWidth, displayHeight);
    // プロジェクション行列設定
    gfl2::gfx::DrawUtil::SetProjectionMatrix(*m_pCameraSystem->GetProjMtx());
    // ビュー行列設定
    gfl2::gfx::DrawUtil::SetViewMatrix(*m_pCameraSystem->GetViewMtx());
    // モデル行列設定
    gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
    gfl2::gfx::DrawUtil::BeginRender();
    if(m_DebugIsVisibleBoundingBox)
    {
      gfl2::gfx::DrawUtil::DrawAABB(m_AABB,gfl2::math::Vector4(1,0,0,0.2f));
      if(m_pPlayerAABB) gfl2::gfx::DrawUtil::DrawAABB(*m_pPlayerAABB,gfl2::math::Vector4(0,0,1,0.2f));
    }
    if(m_DebugIsVisibleBoundingBoxUI)
    {
      gfl2::gfx::DrawUtil::DrawAABB(this->GetAABBIgnoreCollision(),gfl2::math::Vector4(0,1,0,0.2f));
    }

    //if(m_MainSeq == SEQ_MAIN_MAIN)
    //{
    //  u32 numJoint = m_pPokeObj->GetPokeModel()->GetJointCount();
    //  //for(u32 i = 0;i < numJoint;i++)
    //  {
    //    gfl2::gfx::DrawUtil::DrawCircle(m_pPokeObj->GetPokeModel()->GetJointLocalSRT(0).translate + m_Pos,3,gfl2::math::Vector4(1,0,0,1));
    //    gfl2::gfx::DrawUtil::DrawCircle(m_pPokeObj->GetPokeModel()->GetJointLocalSRT(1).translate + m_Pos,3,gfl2::math::Vector4(1,0,0,1));
    //    gfl2::gfx::DrawUtil::DrawCircle(m_pPokeObj->GetPokeModel()->GetJointLocalSRT(2).translate + m_Pos,3,gfl2::math::Vector4(1,0,0,1));
    //  }
    //}


    gfl2::gfx::DrawUtil::EndRender();
    gfl2::gfx::GFGL::EndScene();

    break;
  }
}

void FinderStudioPokemon::StartDebugAutoMotion(u32 monsNo)
{
  m_DebugAutoMotionMonsNo = monsNo;

  PokeTool::SimpleParam param(
    static_cast<MonsNo>(m_DebugAutoMotionMonsNo),
    static_cast<pml::FormNo>(0),
    static_cast<pml::Sex>(0),
    0,0,0
  );
  StartReloadModel(param);
  m_DebugIsDebugAutoMotion = true;
  m_IsEnableAutoMotion = false;
}
void FinderStudioPokemon::EndDebugAutoMotion()
{
  m_DebugIsDebugAutoMotion = false;
  m_IsEnableAutoMotion = true;
}

// デバッグ用即眠り
void FinderStudioPokemon::DebugForceSleep()
{
  this->transitionState(POKE_STATE_WAIT);
  this->request(REQ_TYPE_NULL);
  m_WaitMotionCnt = m_WaitMotionCntMax;
  m_SleepMotionCnt = SLEEP_MOTION_FRAME_MAX;
  m_NowMotionType = POKE_MOTION_TYPE_KAWAIGARI;
}

// アジャスト
f32 FinderStudioPokemon::DebugGetAdjustScale(){ if(m_pPokeObj->GetPokeModel()) return m_pPokeObj->GetPokeModel()->GetAdjustScale(); else return 0; }
f32 FinderStudioPokemon::DebugGetAdjustHeight(){ if(m_pPokeObj->GetPokeModel()) return m_pPokeObj->GetPokeModel()->GetAdjustHeight(); else return 0; }
void FinderStudioPokemon::DebugSetAdjustHeight(f32 adjustHeight){ if(m_pPokeObj->GetPokeModel()) m_pPokeObj->GetPokeModel()->SetAdjustHeight(adjustHeight); }

// 初期化
void FinderStudioPokemon::debugCanPlayMotionListInit()
{
  for(u32 i = 0;i < GFL_NELEMS(m_DebugCanPlayBattleMotionList);i++)
  {
    m_DebugCanPlayBattleMotionList[i] = PokeTool::MODEL_ANIME_ERROR;
  }
  for(u32 i = 0;i < GFL_NELEMS(m_DebugCanPlayKawaigariMotionList);i++)
  {
    m_DebugCanPlayKawaigariMotionList[i] = PokeTool::MODEL_ANIME_ERROR;
  }
}
// 再生できるモーションを格納する
void FinderStudioPokemon::debugCanPlayMotionListStore()
{
  u32 battleArrayIndex = 0;

  // ランダム再生で再生されるものを格納
  for(u32 i = 0;i < GFL_NELEMS(BATTLE_MOTION_TABLE);i++)
  {
    if(this->isCanPlayMotionExist(POKE_MOTION_TYPE_BATTLE,BATTLE_MOTION_TABLE[i]) &&
      this->isCanPlayMotionDataTable(POKE_MOTION_TYPE_BATTLE,BATTLE_MOTION_TABLE[i]))
    {
      GFL_ASSERT_STOP(battleArrayIndex < GFL_NELEMS(m_DebugCanPlayBattleMotionList));
      m_DebugCanPlayBattleMotionList[battleArrayIndex] = BATTLE_MOTION_TABLE[i];
      battleArrayIndex++;
    }
  }

  u32 kawaigariArrayIndex = 0;

  for(u32 i = 0;i < GFL_NELEMS(KAWAIGARI_MOTION_TABLE);i++)
  {
    if(this->isCanPlayMotionExist(POKE_MOTION_TYPE_KAWAIGARI,KAWAIGARI_MOTION_TABLE[i]) &&
      this->isCanPlayMotionDataTable(POKE_MOTION_TYPE_KAWAIGARI,KAWAIGARI_MOTION_TABLE[i]))
    {
      GFL_ASSERT_STOP(kawaigariArrayIndex < GFL_NELEMS(m_DebugCanPlayKawaigariMotionList));
      m_DebugCanPlayKawaigariMotionList[kawaigariArrayIndex] = KAWAIGARI_MOTION_TABLE[i];
      kawaigariArrayIndex++;
    }
  }
}

/// @brief ポケモンモーション指定再生
/// @return true 再生成功 false 再生失敗
bool FinderStudioPokemon::DebugPlayBattleMotion(PokeTool::MODEL_ANIME motionNo)
{
  bool isPlay = this->changeAnimation(POKE_MOTION_TYPE_BATTLE,motionNo);
  if(isPlay) this->transitionState(POKE_STATE_PAUSE_MOTION);
  return isPlay;
}
bool FinderStudioPokemon::DebugPlayKawaigariMotion(PokeTool::MODEL_ANIME motionNo)
{
  bool isPlay = this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,motionNo);
  if(isPlay) this->transitionState(POKE_STATE_PAUSE_MOTION);
  return isPlay;
}
/// @brief 眠りモーションがあるか判定
bool FinderStudioPokemon::DebugIsCanSleep() const
{
  return this->isCanPlayMotionExist(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A) && this->isCanPlayMotionDataTable(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A);
}
/// @brief 眠る
bool FinderStudioPokemon::DebugSleep()
{
  bool isPlay = this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_A);
  if(isPlay) this->transitionState(POKE_STATE_SLEEP);
  return isPlay;
}
/// @brief 起きれるか判定
bool FinderStudioPokemon::DebugIsCanWake() const
{
  if(m_State == POKE_STATE_SLEEP && m_Value == 1)
  {
    return true;
  }

  return false;
}
/// @brief 起きる
bool FinderStudioPokemon::DebugWake()
{
  bool isPlay = this->changeAnimation(POKE_MOTION_TYPE_KAWAIGARI,PokeTool::MODEL_ANIME_DROWSE_C);
  if(isPlay) this->transitionState(POKE_STATE_PAUSE_MOTION);
  return isPlay;
}

#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)