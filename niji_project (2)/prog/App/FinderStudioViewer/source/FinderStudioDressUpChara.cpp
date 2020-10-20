//======================================================================
/**
 * @file FinderStudioDressUpChara.h
 * @date 2016/09/01
 * @author harada_takumi
 * @brief ファインダースタジオ主人公モデル
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioDressUpChara.h"

// System
#include "System/include/nijiAllocator.h"

// DemoLib
#include "DemoLib/Object/include/Object_Chara.h"
#include "DemoLib/Object/include/ObjectRenderingPipeLine.h"

// arc
#include <arc_def_index/arc_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>
#include <niji_conv_header/battle/chara/p1_base.h>
#include <niji_conv_header/battle/chara/p2_base.h>
// Zインアウトモーション用（非常駐）
#include <niji_conv_header/battle/chara/chara_model_dynamic_motion.h>
#include <niji_conv_header/battle/chara/btpc0001_00_finder1.h>
#include <niji_conv_header/battle/chara/btpc0002_00_finder1.h>
// Zモーション用
#include <arc_index/wazaeff_resource.gaix>

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// Zモーション数
const u32 FinderStudioDressUpChara::NUM_Z_MOTION = 19;  // 各タイプ + ピカチュウZ

// Zモーションリソース読み込みテーブル
const u32 FinderStudioDressUpChara::Z_MOTION_RESOURCE_TABLE[NUM_Z_MOTION] = {
  GARC_wazaeff_resource_ez_cmn_normal_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_honoo_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_mizu_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_kusa_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_denki_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_koori_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_kakutou_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_doku_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_jimen_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_hikou_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_esper_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_mushi_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_iwa_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_ghost_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_dragon_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_aku_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_hagane_lz_PCBIN,
  GARC_wazaeff_resource_ez_cmn_fairy_lz_PCBIN,
  GARC_wazaeff_resource_ez_2_pika_action01_lz_PCBIN
};

// 男の子用モーションテーブル（Zモーション以外）
const u32 FinderStudioDressUpChara::OTHER_MOTION_TABLE_MAIL[OTHER_MOTION_MAX] = {   // Zモーションの数 + Zモーションインモーション + 待機モーション
  P1_BASE_EZ_CMN_NORMAL_END,
  P1_BASE_EZ_CMN_HONOO_END,
  P1_BASE_EZ_CMN_MIZU_END,
  P1_BASE_EZ_CMN_KUSA_END,
  P1_BASE_EZ_CMN_DENKI_END,
  P1_BASE_EZ_CMN_KOORI_END,
  P1_BASE_EZ_CMN_KAKUTOU_END,
  P1_BASE_EZ_CMN_DOKU_END,
  P1_BASE_EZ_CMN_JIMEN_END,
  P1_BASE_EZ_CMN_HIKOU_END,
  P1_BASE_EZ_CMN_ESPER_END,
  P1_BASE_EZ_CMN_MUSHI_END,
  P1_BASE_EZ_CMN_IWA_END,
  P1_BASE_EZ_CMN_GHOST_END,
  P1_BASE_EZ_CMN_DRAGON_END,
  P1_BASE_EZ_CMN_AKU_END,
  P1_BASE_EZ_CMN_HAGANE_END,
  P1_BASE_EZ_CMN_FAIRY_END,
  P1_BASE_EZ_2_PIKA_END,
  P1_BASE_EZ_CMN_00_START,
  P1_BASE_BA051_GREET01,
  P1_BASE_BA005_WAIT01
};

// 女の子用モーションテーブル（Zモーション以外）
const u32 FinderStudioDressUpChara::OTHER_MOTION_TABLE_FEMAIL[OTHER_MOTION_MAX] = {   // Zモーションの数 + Zモーションインモーション + 待機モーション
  P2_BASE_EZ_CMN_NORMAL_END,
  P2_BASE_EZ_CMN_HONOO_END,
  P2_BASE_EZ_CMN_MIZU_END,
  P2_BASE_EZ_CMN_KUSA_END,
  P2_BASE_EZ_CMN_DENKI_END,
  P2_BASE_EZ_CMN_KOORI_END,
  P2_BASE_EZ_CMN_KAKUTOU_END,
  P2_BASE_EZ_CMN_DOKU_END,
  P2_BASE_EZ_CMN_JIMEN_END,
  P2_BASE_EZ_CMN_HIKOU_END,
  P2_BASE_EZ_CMN_ESPER_END,
  P2_BASE_EZ_CMN_MUSHI_END,
  P2_BASE_EZ_CMN_IWA_END,
  P2_BASE_EZ_CMN_GHOST_END,
  P2_BASE_EZ_CMN_DRAGON_END,
  P2_BASE_EZ_CMN_AKU_END,
  P2_BASE_EZ_CMN_HAGANE_END,
  P2_BASE_EZ_CMN_FAIRY_END,
  P2_BASE_EZ_2_PIKA_END,
  P2_BASE_EZ_CMN_00_START,
  P2_BASE_BA051_GREET01,
  P2_BASE_BA005_WAIT01
};

const u32 FinderStudioDressUpChara::Z_IN_MOTION_INDEX = NUM_Z_MOTION * 2; // アウトモーションを含むため2倍
const u32 FinderStudioDressUpChara::WAIT_MOTION_INDEX = Z_IN_MOTION_INDEX + 1;  // インモーションの次

// Zモーションのヒープサイズ
// 2017/3/17 Zモーションのパーツに合わせて増分
// 2017/3/28 Zモーションのパーツ更新に合わせて増分
//           Zモーションの追加・更新はもうないことを稲葉さんに確認済み
const u32 FinderStudioDressUpChara::MOTION_HEAP_SIZE = 0x700000 + 0x80000 + 0x8000;

//コンストラクタ
FinderStudioDressUpChara::FinderStudioDressUpChara(const Desc& desc):
  m_pPhotoParam(NULL)
  ,m_IsUsePhotoParam(false)
  ,m_Composition(COMPOSITION_1)
  ,m_InitSeq(0)
  ,m_MainSeq(0)
  ,m_EndSeq(0)
  ,m_pCharaObj(NULL)
  ,m_DegY(0)
  ,m_IgnoreCollisionDegY(0)
  ,m_pPokemonAABB(NULL)
  ,m_CollisionPermit(0)
  ,m_pZMotionResourceBuffer(NULL)
  ,m_ZMotionType(Z_MOTION_TYPE_DENKI)
  ,m_ReqestZMotionType(Z_MOTION_TYPE_NORMAL)
  ,m_NowAnimationNo(0)
  ,m_State(CHARA_STATE_NULL)
  ,m_RequestType(REQ_TYPE_NULL)
  ,m_Value(0)
  ,m_SaveValue(0)
  ,m_IsEnable(true)
{
  GFL_ASSERT_MSG(desc.pHeap,"ヒープを指定してください\n");
  GFL_ASSERT_MSG(desc.pWorkHeap,"一時ヒープを指定してください\n");
  GFL_ASSERT_MSG(desc.pObjectRenderPipeLine,"パイプラインを指定してください\n");

  m_pHeap = desc.pHeap;
  m_pWorkHeap = desc.pWorkHeap;
  m_pNijiAllocator = desc.pAllocator;
  m_pRenderPipeLine = desc.pObjectRenderPipeLine;
  m_pCharaModelFactory = desc.pCharaModelFactory;
  m_pDressUpModelResourceManager = desc.pDressUpModelResourceManager;
  m_pPhotoParam = desc.pPhotoParam;
  m_IsUsePhotoParam = desc.isUsePhotoParam;

  // ヒープ作成
  m_pMotionHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,MOTION_HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioTrainerMotionHeap");

  // 着せ替えデータをパラメータから取得
  if(!m_IsUsePhotoParam)
  {
    // 再現パラメータを使わない場合
    GFL_ASSERT_STOP(desc.pDressUpParam);
    m_DressUpParam = *desc.pDressUpParam;
  }
  else
  {
    // 再現パラメータを使う場合
    GFL_ASSERT_STOP(m_pPhotoParam);
    this->ApplyPhotoParamDressUpParam(*m_pPhotoParam);
  }
}
//デストラクタ
FinderStudioDressUpChara::~FinderStudioDressUpChara()
{
  GFL_DELETE_HEAP(m_pMotionHeap);
}

// メイン処理用
enum{
  SEQ_MAIN_LOAD_DYNAMIC_ANIMATION,
  SEQ_MAIN_LOAD_DYNAMIC_ANIMATION_WAIT,
  SEQ_MAIN_LOAD_MODEL,
  SEQ_MAIN_LOAD_MODEL_WAIT,
  SEQ_MAIN_MAIN,
  SEQ_MAIN_WAIT,  // 解放前の1フレーム待ち
  SEQ_MAIN_DELETE,
};

// 初期化処理
bool FinderStudioDressUpChara::InitFunc()
{
  enum{
    SEQ_INIT_LOAD_CHARAMODEL,
    SEQ_INIT_LOAD_CHARAMODEL_WAIT,
    SEQ_INIT_LOAD_Z_INOUT_MOTION,
    SEQ_INIT_LOAD_Z_INOUT_MOTION_WAIT,
    SEQ_INIT_LOAD_Z_MOTION,
    SEQ_INIT_LOAD_Z_MOTION_WAIT,
    SEQ_INIT_END
  };

  switch(m_InitSeq)
  {
  case SEQ_INIT_LOAD_CHARAMODEL:
  {
    // キャラモデル作成
    m_pCharaObj = GFL_NEW(m_pHeap) DemoLib::Obj::Object_Chara(m_pRenderPipeLine,m_pCharaModelFactory,m_pDressUpModelResourceManager);
    startLoadModel();

    m_InitSeq++;
  }
  // fall through

  case SEQ_INIT_LOAD_CHARAMODEL_WAIT:
    if(isFinishLoadModel())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_LOAD_Z_INOUT_MOTION:
    // Zインアウトモーション読み込み（非常駐）
    this->startLoadDynamicAnimation();

    m_InitSeq++;
    // fall through

  case SEQ_INIT_LOAD_Z_INOUT_MOTION_WAIT:
    if(this->isFinishLoadDynamicAnimation())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_LOAD_Z_MOTION:
    loadZMotionAsync();
    m_InitSeq++;
    // fall through

  case SEQ_INIT_LOAD_Z_MOTION_WAIT:
    if(isFinishLoadZMotionAsync())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_END:
    // モデル読み込み後の初期化
    setup();

    // 再現パラメータを使うなら残りのパラメータも適応
    if(m_IsUsePhotoParam)
    {
      if(m_pPhotoParam)
      {
        this->ApplyPhotoParamTrainerOther(*m_pPhotoParam);
      }
      else
      {
        GFL_ASSERT(0);
      }
    }

    // メインの更新から開始
    m_MainSeq = SEQ_MAIN_MAIN;

    //GFL_PRINT("初期化後ヒープ\n");
    //m_pHeap->Dump();
    //m_pWorkHeap->Dump();
    //m_pMotionHeap->Dump();

    return true;
  }

  return false;
}

// 非常駐モーション読み込み
void FinderStudioDressUpChara::startLoadDynamicAnimation()
{
  poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);
  //m_Sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);

  // Zインアウトモーション読み込み（非常駐）
  if(sex == poke_3d::model::DressUpParam::SEX_MALE)
  {
    m_pCharaModelFactory->LoadDynamicAnimationAsync(BTPC0001_00_FINDER1);
    m_pDressUpModelResourceManager->LoadDressUpDynamicAnimationAsync(m_DressUpParam,BTPC0001_00_FINDER1);
  }
  else if(sex == poke_3d::model::DressUpParam::SEX_FEMALE)
  {
    m_pCharaModelFactory->LoadDynamicAnimationAsync(BTPC0002_00_FINDER1);
    m_pDressUpModelResourceManager->LoadDressUpDynamicAnimationAsync(m_DressUpParam,BTPC0002_00_FINDER1);
  }
  else
  {
    GFL_ASSERT(0);   // ここには来ないはず
    m_pCharaModelFactory->LoadDynamicAnimationAsync(BTPC0001_00_FINDER1);
    m_pDressUpModelResourceManager->LoadDressUpDynamicAnimationAsync(m_DressUpParam,BTPC0001_00_FINDER1);
  }
}
// 読み込みが終わったか
bool FinderStudioDressUpChara::isFinishLoadDynamicAnimation()
{
  poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);
  if(sex == poke_3d::model::DressUpParam::SEX_MALE)
  {
    if(m_pDressUpModelResourceManager->IsDressUpDynamicAnimationLoaded(m_DressUpParam,BTPC0001_00_FINDER1) &&
      m_pCharaModelFactory->IsDynamicAnimationLoaded(BTPC0001_00_FINDER1))
    {
      m_pDressUpModelResourceManager->SetupDressUpDynamicAnimation(m_pNijiAllocator,m_pHeap,m_DressUpParam,BTPC0001_00_FINDER1);
      m_pCharaModelFactory->SetupDynamicAnimation(m_pNijiAllocator,m_pHeap,BTPC0001_00_FINDER1);

      return true;
    }
  }
  else if(sex == poke_3d::model::DressUpParam::SEX_FEMALE)
  {
    if(m_pDressUpModelResourceManager->IsDressUpDynamicAnimationLoaded(m_DressUpParam,BTPC0002_00_FINDER1) &&
      m_pCharaModelFactory->IsDynamicAnimationLoaded(BTPC0002_00_FINDER1))
    {
      m_pDressUpModelResourceManager->SetupDressUpDynamicAnimation(m_pNijiAllocator,m_pHeap,m_DressUpParam,BTPC0002_00_FINDER1);
      m_pCharaModelFactory->SetupDynamicAnimation(m_pNijiAllocator,m_pHeap,BTPC0002_00_FINDER1);

      return true;
    }
  }
  else
  {
    if(m_pDressUpModelResourceManager->IsDressUpDynamicAnimationLoaded(m_DressUpParam,BTPC0001_00_FINDER1) &&
      m_pCharaModelFactory->IsDynamicAnimationLoaded(BTPC0001_00_FINDER1))
    {
      GFL_ASSERT(0);   // ここには来ないはず
      m_pDressUpModelResourceManager->SetupDressUpDynamicAnimation(m_pNijiAllocator,m_pHeap,m_DressUpParam,BTPC0001_00_FINDER1);
      m_pCharaModelFactory->SetupDynamicAnimation(m_pNijiAllocator,m_pHeap,BTPC0001_00_FINDER1);

      return true;
    }
  }

  return false;
}

// 非常駐モーション解放
void FinderStudioDressUpChara::unloadDynamicAniamtion()
{
  poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);
  if(sex == poke_3d::model::DressUpParam::SEX_MALE)
  {
    m_pDressUpModelResourceManager->UnloadDressUpDynamicAnimation(m_DressUpParam,BTPC0001_00_FINDER1);
    m_pCharaModelFactory->UnloadDynamicAnimation(BTPC0001_00_FINDER1);
  }
  else if(sex == poke_3d::model::DressUpParam::SEX_FEMALE)
  {
    m_pDressUpModelResourceManager->UnloadDressUpDynamicAnimation(m_DressUpParam,BTPC0002_00_FINDER1);
    m_pCharaModelFactory->UnloadDynamicAnimation(BTPC0002_00_FINDER1);
  }
  else
  {
    GFL_ASSERT_STOP(0);
  }
}

// モデルロード
void FinderStudioDressUpChara::startLoadModel()
{
  poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);

  DemoLib::Obj::Object_Chara::LOAD_PARAM param;
  if(sex == poke_3d::model::DressUpParam::SEX_MALE)        param.charaId = CHARA_MODEL_ID_P1_BASE;
  else if(sex == poke_3d::model::DressUpParam::SEX_FEMALE) param.charaId = CHARA_MODEL_ID_P2_BASE;
  else
  {
    GFL_ASSERT(0);   // ここには来ないはず
    param.charaId = CHARA_MODEL_ID_P1_BASE;
  }
  param.dressParam = &m_DressUpParam;

  m_pCharaObj->StartLoad(param);
}
// 読み込みが終わったか
bool FinderStudioDressUpChara::isFinishLoadModel()
{
  if(m_pCharaObj->IsFinishLoad())
  {
    m_pCharaObj->Create(m_pHeap,m_pNijiAllocator);
    // バトルモデルとしてセット
    m_pCharaObj->SetIsBattleModel();
    return true;
  }

  return false;
}

// Z技モーション読み込み
void FinderStudioDressUpChara::loadZMotionAsync()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  const u32 ARC_ID = ARCID_WAZAEFF_RESOURCE;

  // ARCファイルオープン
  {
    gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
    openReq.arcId	= ARC_ID;
    openReq.heapForFile = m_pWorkHeap->GetLowerHandle();
    openReq.heapForReq = m_pWorkHeap->GetLowerHandle();
    fileManager->SyncArcFileOpen(openReq);
  }

  // バッファ確保
  m_pZMotionResourceBuffer = GFL_NEW_ARRAY(m_pMotionHeap) void*[NUM_Z_MOTION];

  // 読み込み開始
	{
		gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
    loadReq.arcId = ARC_ID;
		loadReq.align = 128;
    loadReq.heapForBuf = m_pMotionHeap;
    loadReq.heapForReq = m_pWorkHeap->GetLowerHandle();
    loadReq.heapForCompressed = m_pWorkHeap->GetLowerHandle();  // 解凍が必要

    for(u32 i = 0;i < NUM_Z_MOTION;i++)
    {
		  loadReq.datId = Z_MOTION_RESOURCE_TABLE[i];
      loadReq.ppBuf = &m_pZMotionResourceBuffer[i];
		  fileManager->AddArcFileLoadDataReq(loadReq);
    }
	}
}
// 読み込みが終わったか
bool FinderStudioDressUpChara::isFinishLoadZMotionAsync()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  for(u32 i = 0;i < NUM_Z_MOTION;i++)
  {
    if(!fileManager->IsArcFileLoadDataFinished(&m_pZMotionResourceBuffer[i]))
    {
      // １つでも終わってなかったらfalse
      return false;
    }
  }

  // 読み込み完了したらClose
  gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
	closeReq.arcId = ARCID_WAZAEFF_RESOURCE;
	fileManager->SyncArcFileClose(closeReq);

  return true;
}

// モデル読み込み後の初期化
void FinderStudioDressUpChara::setup()
{
  // 角度リセット
  this->ResetRotate();
  // 描画パイプライン登録
  m_pCharaObj->AddRenderPath();
  // 初期角度
  m_pCharaObj->SetRotate(0,m_DegY,0);
  // 初期状態へ
  this->transitionState(CHARA_STATE_WAIT);
  // 待機モーション再生（遷移では再生されない可能性があるため）
  this->changeAnimationOther(OTHER_MOTION_WAIT,true);
  // 自動目パチオン
  m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_BATTLE);
  m_pCharaObj->GetDressUpModel()->SetupRandomFuc(System::GflUse::GetPublicRand);
}

// AABBセット
void FinderStudioDressUpChara::setupAABB()
{
  m_AABB = this->getOrigAABB();
  this->applyAABB(&m_AABB);
}

// 元となるAABBを取得
gfl2::math::AABB FinderStudioDressUpChara::getOrigAABB() const
{
  return gfl2::math::AABB(gfl2::math::Vector3(-62,0,-50),gfl2::math::Vector3(78,150,100));
}

// 終了処理
bool FinderStudioDressUpChara::EndFunc()
{
  enum{
    SEQ_END_WAIT,
    SEQ_END_DELETE_CHARAMODEL,
    SEQ_END_RELEASE_ZMOTION,
    SEQ_END_END
  };


  switch(m_EndSeq)
  {
  case SEQ_END_WAIT:
    // モデル削除のため１フレーム待つ
    m_pCharaObj->SetVisible(false);
    m_EndSeq++;
    break;

  case SEQ_END_DELETE_CHARAMODEL:
    // キャラモデル削除
    m_pCharaObj->Delete();
    GFL_SAFE_DELETE(m_pCharaObj);

    m_EndSeq++;
    // fall through

  case SEQ_END_RELEASE_ZMOTION:
    for(u32 i = 0;i < NUM_Z_MOTION;i++)
    {
      GflHeapFreeMemory(m_pZMotionResourceBuffer[i]);
    }
    GFL_SAFE_DELETE_ARRAY(m_pZMotionResourceBuffer);

    // 非常駐モーション削除
    this->unloadDynamicAniamtion();

    m_EndSeq++;
    break;

  case SEQ_END_END:
    return true;
  }
  return false;
}

// 更新
void FinderStudioDressUpChara::Update()
{
  switch(m_MainSeq)
  {
  case SEQ_MAIN_LOAD_DYNAMIC_ANIMATION:
    // @note CharaObjのDeleteで非常駐モーションまで消してしまうようなので再ロード
    this->startLoadDynamicAnimation();
    m_MainSeq++;
    // fall throught

  case SEQ_MAIN_LOAD_DYNAMIC_ANIMATION_WAIT:
    if(this->isFinishLoadDynamicAnimation())
    {
      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_LOAD_MODEL:
    startLoadModel();
    m_MainSeq++;
    // fall through

  case SEQ_MAIN_LOAD_MODEL_WAIT:
    if(isFinishLoadModel())
    {
      setup();
      //GFL_PRINT("読込後ヒープ\n");
      //m_pHeap->Dump();
      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_MAIN:
    updateMain();
    break;

  case SEQ_MAIN_WAIT:
    // 解放のため1フレーム待つ
    m_pCharaObj->SetVisible(false);
    m_MainSeq++;
    break;

  case SEQ_MAIN_DELETE:
    m_pCharaObj->RemoveRenderPath();
    m_pCharaObj->Delete();
    // @fix GFMMCat[40] 非常駐モーションのリソースバッファが解放できていなかったので解放処理を追加
    this->unloadDynamicAniamtion();
    // 削除後、次の着せ替えパラメータをセット
    /// @attention 削除時は最初に渡した着せ替えパラメータの性別のパーツが削除されるため
    ///            男女切り替え時にも解放されるよう、削除し終わってから着せ替えパラメータを切り替える
    m_DressUpParam = m_NextDressUpParam;
    //GFL_PRINT("解放後ヒープ\n");
    //m_pHeap->Dump();
    // ロードへ
    m_MainSeq = SEQ_MAIN_LOAD_DYNAMIC_ANIMATION;
  }
}

// 更新メイン処理
void FinderStudioDressUpChara::updateMain()
{
  if(!m_IsEnable) return;

  switch(m_State)
  {
  case CHARA_STATE_WAIT:
    updateWait();
    break;
  case CHARA_STATE_Z_MOTION:
    // Zモーション時更新
    updateZMotion();
    break;
  case CHARA_STATE_Z_MOTION_STOP:
    updateZMotionStop();
    break;
  default:
    GFL_ASSERT(0);
  }

  // リクエストを消去
  m_RequestType = REQ_TYPE_NULL;

  // AABBを設定
  this->setupAABB();

  // Y軸回転更新
  m_pCharaObj->SetRotate(0,m_DegY,0);

  // 目の更新
  m_pCharaObj->GetDressUpModel()->UpdateEye();

  // アニメーション更新前の値を保存
  this->savePrevParam();

  // キャラモデル更新
  m_pCharaObj->Update();
}

// トレーナーの状態遷移
void FinderStudioDressUpChara::transitionState(CHARA_STATE state)
{
  switch(state)
  {
  case CHARA_STATE_NULL:
    // リクエスト無しなら何もしない
    return;

  case CHARA_STATE_WAIT:
    this->changeAnimationOther(OTHER_MOTION_WAIT,false);
    // 目パチ上書き再開
    m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_BATTLE);
    break;

  case CHARA_STATE_Z_MOTION:
    // 挨拶モーション例外対応
    if(m_ZMotionType == Z_MOTION_TYPE_GREET)
    {
      this->changeAnimationOther(OTHER_MOTION_GREET);
    }
    else
    {
      this->changeAnimationOther(OTHER_MOTION_Z_IN);
    }
    // 目パチ上書き停止
    m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_NONE);
    break;

  case CHARA_STATE_Z_MOTION_STOP:
    m_pCharaObj->SetAnimationSpeed(0);
    // 目パチ上書き再開
    m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_BATTLE);
    break;
  }

  m_State = state;
  m_Value = 0;
}

// 各状態関数
void FinderStudioDressUpChara::updateWait()
{
  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_NULL:
    // 何もしない
    break;
  case REQ_TYPE_PLAY_Z_MOTION:
    m_ZMotionType = m_ReqestZMotionType;
    this->transitionState(CHARA_STATE_Z_MOTION);
    break;
  case REQ_TYPE_STOP_Z_MOTION:
    // 何もしない
    break;
  case REQ_TYPE_RESET:
    // モーションリセット
    this->changeAnimationOther(OTHER_MOTION_WAIT);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}
void FinderStudioDressUpChara::updateZMotion()
{
  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_NULL:
  case REQ_TYPE_PLAY_Z_MOTION:
    // 再生続行
    enum{
      MOTION_SEQ_START,  // 待機→Zへのモーション中
      MOTION_SEQ_Z,      // Zモーション中
      MOTION_SEQ_END     // Z→待機へのモーション中
    };

    switch(m_Value)
    {
    case MOTION_SEQ_START:
      // 終わったらZモーションへ
      if(m_pCharaObj->IsFinishAnimation())
      {
        // 挨拶モーション例外対応
        if(m_ZMotionType == Z_MOTION_TYPE_GREET)
        {
          // 挨拶は１モーションで完結しているので終了次第待機へ
          this->transitionState(CHARA_STATE_WAIT);
        }
        else
        {
          this->changeAnimationZ(m_ZMotionType);
          m_Value++;
        }
      }
      break;

    case MOTION_SEQ_Z:
      // 終わったら終了モーションへ
      if(m_pCharaObj->IsFinishAnimation())
      {
        OTHER_MOTION_INDEX otherMotionIndex = convertZMotionTypeToZOutMotion(m_ZMotionType);
        changeAnimationOther(otherMotionIndex);
        m_Value++;
      }
      break;

    case MOTION_SEQ_END:
      // 終わったら待機へ
      if(m_pCharaObj->IsFinishAnimation())
      {
        this->transitionState(CHARA_STATE_WAIT);
      }
      break;
    }

    break;

  case REQ_TYPE_STOP_Z_MOTION:
    // モーション進捗を保存
    m_SaveValue = m_Value;
    this->transitionState(CHARA_STATE_Z_MOTION_STOP);
    break;
  case REQ_TYPE_RESET:
    // モーションリセット
    this->transitionState(CHARA_STATE_WAIT);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

void FinderStudioDressUpChara::updateZMotionStop()
{
  // リクエスト処理
  switch(m_RequestType)
  {
  case REQ_TYPE_NULL:
    // 何もしない
    break;
  case REQ_TYPE_PLAY_Z_MOTION:
    // 同じモーションであれば再開する
    if(m_ReqestZMotionType == m_ZMotionType)
    {
      // 一時停止からの再生
      m_pCharaObj->SetAnimationSpeed(1.0f);
      // 目パチ上書き停止
      m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_NONE);
      // 例外で直接ステート変更
      m_State = CHARA_STATE_Z_MOTION;
      // 保存していた値（モーション進捗）で上書き
      m_Value = m_SaveValue;
      break;
    }

  case REQ_TYPE_STOP_Z_MOTION:
    // 何もしない
    break;
  case REQ_TYPE_RESET:
    // モーションリセット
    this->transitionState(CHARA_STATE_WAIT);
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
}

/// @brief 写真再現パラメータ適用（着せ替えデータ）
/// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
void FinderStudioDressUpChara::ApplyPhotoParamDressUpParam(const PhotoParam& photoParam)
{
  m_DressUpParam = this->GetDressUpParamFromPhotoParam(photoParam);
}
/// @brief 写真再現パラメータ適用（その他データ）
void FinderStudioDressUpChara::ApplyPhotoParamTrainerOther(const PhotoParam& photoParam)
{
  if(isZMotionAnimationNo(photoParam.trainerMotionType))
  {
    ZMotionType type = this->convertAnimationNoToZMotionType(photoParam.trainerMotionType);
    this->changeAnimationZ(type);
  }
  else
  {
    OTHER_MOTION_INDEX index = this->convertAnimationNoToOtherMotion(photoParam.trainerMotionType);
    this->changeAnimationOther(index);
  }
  m_pCharaObj->SetAnimationFrame(photoParam.trainerMotionFrame);

  // 目パチ適用
  poke_3d::model::CharaModel::EyeIndex eyeIndex = static_cast<poke_3d::model::CharaModel::EyeIndex>(photoParam.trainerEyeAnmPattern);
  m_pCharaObj->GetCharaModel()->SetEyeIndex(eyeIndex);

  m_DegY = photoParam.trainerDegY;
  m_IgnoreCollisionDegY = m_DegY;

  // @fix GFMMCat[105] 後にトレーナーとポケモンのめり込み判定を行うのでここでAABBを更新しておく
  this->setupAABB();

  // リクエストを削除
  this->request(REQ_TYPE_NULL);
}

/// @brief 写真パラメータから着せ替えパラメータを生成
poke_3d::model::DressUpParam FinderStudioDressUpChara::GetDressUpParamFromPhotoParam(const PhotoParam& photoParam)
{
  poke_3d::model::DressUpParam dressUpParam;

  dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX] = photoParam.sexTrainer;
  dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR] = photoParam.skinColor;
  dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR] = photoParam.eyeColor;
  dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR] = photoParam.hairColor;
  dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = photoParam.lipColor;

  // 保存時に+1にして保存したため、-1して適応
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_FACE] = 0;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = photoParam.hairStyle - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] = photoParam.eyeAccessory - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] = photoParam.hairAccessory - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG] = photoParam.bag - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BNGL] = 0;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BODY] = -1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS] = photoParam.bottoms - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] = photoParam.hat - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS] = photoParam.leg - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES] = photoParam.shoes - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS] = photoParam.tops - 1;
  dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SITU] = photoParam.situ - 1;

  dressUpParam.hasBagCharm = true;
  dressUpParam.hasZRing = true;

  return dressUpParam;
}

/// @brief 着せ替えパラメータを写真パラメータに適応
/// @param pPhotoParam 適応する写真パラメータ
/// @param dressUpParam 着せ替えデータ
void FinderStudioDressUpChara::SetDressUpParamToPhotoParam(PhotoParam* pPhotoParam,const poke_3d::model::DressUpParam& dressUpParam)
{
  // 着せ替え
  pPhotoParam->sexTrainer = dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX];
  pPhotoParam->skinColor = dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_BODY_COLOR];
  pPhotoParam->eyeColor = dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_EYE_COLOR];
  pPhotoParam->hairColor = dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_HAIR_COLOR];
  pPhotoParam->lipColor = dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR];

  // つけていない場合-1になるので+1して保存
  pPhotoParam->hairStyle = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAIR_STYLE] + 1;
  pPhotoParam->eyeAccessory = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCEYE] + 1;
  pPhotoParam->hairAccessory = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_ACCHAIR] + 1;
  pPhotoParam->bag = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BAG] + 1;
  pPhotoParam->bottoms = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_BOTTOMS] + 1;
  pPhotoParam->hat = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_HAT] + 1;
  pPhotoParam->leg = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_LEGS] + 1;
  pPhotoParam->shoes = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SHOES] + 1;
  pPhotoParam->tops = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_TOPS] + 1;
  pPhotoParam->situ = dressUpParam.itemParams[poke_3d::model::DressUpParam::ITEM_CATEGORY_SITU] + 1;
}

/// @brief アニメーション更新のオンオフを切り替える
/// @param true オン false オフ
/// @note 写真再現後の更新のため
void FinderStudioDressUpChara::SetIsAnimationUpdate(bool isUpdate)
{
  m_pCharaObj->GetBaseModel()->SetAnimationUpdateFrameEnable(isUpdate);
}

/// @brief 自動目パチのオンオフを切り替える
/// @param true オン false オフ
void FinderStudioDressUpChara::SetIsAutoBlink(bool isBlink)
{
  GFL_ASSERT_STOP(m_pCharaObj->GetDressUpModel());

  if(isBlink)
  {
    // 自動目パチをオンにする
    m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_BATTLE);
  }
  else
  {
    // 自動目パチをオフにする
    m_pCharaObj->GetDressUpModel()->SetAutoBlinkMode(poke_3d::model::DressUpModel::AUTO_BLINK_MODE_NONE);
  }
}

/// @brief 写真再現パラメータの出力が可能かどうか
/// @return true 可能 false 不可能
bool FinderStudioDressUpChara::IsCanGetPhotoParam()
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
void FinderStudioDressUpChara::GetPhotoParamTrainer(PhotoParam* pPhotoParam) const
{
  if(m_MainSeq != SEQ_MAIN_MAIN) return;

  GFL_ASSERT(pPhotoParam);

  // 着せ替えデータ
  this->SetDressUpParamToPhotoParam(pPhotoParam,m_DressUpParam);

  // モーション
  pPhotoParam->trainerMotionType = m_NowAnimationNo;
  // 目パチ以外のモーションフレームは１フレーム先の値が入っているので保存していた値を取得
  pPhotoParam->trainerMotionFrame = m_PrevParam.mainFrame;
  // 目パチ
  pPhotoParam->trainerEyeAnmPattern = m_pCharaObj->GetCharaModel()->GetEyeIndex();
  // 角度
  pPhotoParam->trainerDegY = m_DegY;
}

/// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
f32 FinderStudioDressUpChara::getPrevAnimationFrame(const poke_3d::model::BaseModel* pModel,u32 slotIndex) const
{
  GFL_ASSERT_STOP(pModel);

  f32 stepFrame = pModel->GetAnimationStepFrame(slotIndex);
  int frame = pModel->GetAnimationFrame(slotIndex) - stepFrame;
  if(frame < 0) frame = pModel->GetAnimationEndFrame(slotIndex);

  return frame;
}

/// @brief 撮影モード切替時処理
void FinderStudioDressUpChara::OnChangeCaptureMode(CaptureMode captureMode)
{
  if(captureMode == CAPTURE_MODE_POKE)
  {
    m_IsEnable = false;
    m_pCharaObj->SetVisible(false);
  }
  else if(captureMode == CAPTURE_MODE_BOTH)
  {
    m_IsEnable = true;
    m_pCharaObj->SetVisible(true);
  }
  else
  {
    GFL_ASSERT(0);  // ここには来ないはず
  }
}

/// @brief 構図用パラメータ適用
/// @param composition 構図の種類
void FinderStudioDressUpChara::ApplyComposition(Composition composition)
{
  m_Composition = composition;
}

/// @brief 着せ替えモデル再ロード（着せ替えパラメータは内部のものを使用）
//void FinderStudioDressUpChara::StartReloadDressUpModel()
//{
//  this->StartReloadDressUpModel(m_DressUpParam);
//}

/// @brief 着せ替えモデル再ロード
/// @param dressUpParam 着せ替えデータ
void FinderStudioDressUpChara::StartReloadDressUpModel(poke_3d::model::DressUpParam dressUpParam)
{
  // メイン処理のときしかリロード開始不可
  if(m_MainSeq != SEQ_MAIN_MAIN)
  {
    GFL_ASSERT_STOP_MSG(0,"まだ読み込みが完了していません！\n");
    return;
  }

  m_NextDressUpParam = dressUpParam;

  m_MainSeq = SEQ_MAIN_WAIT;
}

/// @brief 再ロードが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioDressUpChara::IsFinishReloadDressUpModel()
{
  if(m_MainSeq == SEQ_MAIN_MAIN)
  {
    return true;
  }

  return false;
}

/// @brief Zモーション再生
/// @param motionType Zモーションのタイプ
/// @return 再生に成功したら（すでに再生中なら失敗）
bool FinderStudioDressUpChara::PlayZMotion(ZMotionType motionType)
{
  this->request(REQ_TYPE_PLAY_Z_MOTION);
  this->requestZMotionType(motionType);

  bool result = false;
  switch(m_State)
  {
  case CHARA_STATE_WAIT:
    result = true;
    break;

  case CHARA_STATE_Z_MOTION:
    // 再生中は失敗
    result = false;
    break;

  case CHARA_STATE_Z_MOTION_STOP:
    // 停止中は同じモーションなら続きから再開
    if(m_ZMotionType == motionType)
    {
      result = true;
    }
    else
    {
      // 違うモーションを選んでいれば失敗
      result = false;
    }
    break;

  default:
    // ここを通るのはありえない
    GFL_ASSERT(0);
    break;
  }

  return result;
}

#if PM_DEBUG
// デバッグで選択中のZモーション再生
void FinderStudioDressUpChara::DebugPlayZMotion()
{
  this->request(REQ_TYPE_PLAY_Z_MOTION);
  this->requestZMotionType(m_ZMotionType);
}
#endif

/// @brief Z技モーション再生
/// @param type モーションインデックス
/// @param isForce 同じモーションだった場合再生しなおすか
void FinderStudioDressUpChara::changeAnimationZ(ZMotionType type,bool isForce)
{
  if(!isForce)
  {
    if(this->isSameAnimationNo(type)) return;
  }

  if(type >= 0 && type < NUM_Z_MOTION)
  {
    // リソースバッファにはZMotionTypeと同じタイプの順番でリソースが入っている
    m_pCharaObj->ChangeAnimationResourceDressUp(m_pZMotionResourceBuffer[type],m_pNijiAllocator);
    this->saveNowAnimationNo(type);
  }
  else
  {
    GFL_ASSERT_MSG(0,"Resource Buffer Over!\n");
  }
}

/// @brief その他モーション再生
/// @param otherMotionIndex モーションインデックス
/// @param isForce 同じモーションだった場合再生しなおすか
void FinderStudioDressUpChara::changeAnimationOther(OTHER_MOTION_INDEX otherMotionIndex,bool isForce)
{
  if(!isForce)
  {
    if(this->isSameAnimationNo(otherMotionIndex)) return;
  }

  if(otherMotionIndex >= 0 && otherMotionIndex < OTHER_MOTION_MAX)
  {
    poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(m_DressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);
    if(sex == poke_3d::model::DressUpParam::SEX_MALE) m_pCharaObj->ChangeAnimation(OTHER_MOTION_TABLE_MAIL[otherMotionIndex]);
    else if(sex == poke_3d::model::DressUpParam::SEX_FEMALE) m_pCharaObj->ChangeAnimation(OTHER_MOTION_TABLE_FEMAIL[otherMotionIndex]);
    else GFL_ASSERT(0);

    this->saveNowAnimationNo(otherMotionIndex);
  }
  else
  {
    GFL_ASSERT_MSG(0,"Motion Table Over!\n");
  }
}

/// @brief ZMotionTypeを対応したOTHER_MOTION_INDEXに変換
/// @param type Zモーションのタイプ
/// @return 対応したOTHER_MOTION_INDEX
/// @note 例）Z_MOTION_TYPE_NORMAL→OTHER_MOTION_Z_OUT_NORMAL
FinderStudioDressUpChara::OTHER_MOTION_INDEX FinderStudioDressUpChara::convertZMotionTypeToZOutMotion(ZMotionType type)
{
  if(type >= 0 && type < NUM_Z_MOTION)
  {
    // そのまま変換すればアウトモーションになる
    return static_cast<OTHER_MOTION_INDEX>(type);
  }
  else
  {
    GFL_ASSERT(0);
    return OTHER_MOTION_WAIT;
  }
}

/// @brief アニメーション種をOTHER_MOTION_INDEXに変換
/// @param anmNo アニメーション種
/// @return 対応したOTHER_MOTION_INDEX
/// @note anmNoにはZMotionType→OTHER_MOTION_INDEXの順に数字が並んでいるはず
FinderStudioDressUpChara::OTHER_MOTION_INDEX FinderStudioDressUpChara::convertAnimationNoToOtherMotion(u32 anmNo)
{
  if(anmNo < NUM_Z_MOTION || anmNo >= NUM_Z_MOTION + OTHER_MOTION_MAX) GFL_ASSERT_STOP(0);
  return static_cast<FinderStudioDressUpChara::OTHER_MOTION_INDEX>(anmNo - NUM_Z_MOTION);
}
/// @brief アニメーション種をZMotionTypeに変換
/// @param anmNo アニメーション種
/// @return 対応したZMotionType
/// @note anmNoにはZMotionType→OTHER_MOTION_INDEXの順に数字が並んでいるはず
ZMotionType FinderStudioDressUpChara::convertAnimationNoToZMotionType(u32 anmNo)
{
  if(anmNo >= NUM_Z_MOTION) GFL_ASSERT_STOP(0);
  return static_cast<ZMotionType>(anmNo);
}

/// @brief 再生したアニメーション種の保存
void FinderStudioDressUpChara::saveNowAnimationNo(ZMotionType type)
{
  if(type >= NUM_Z_MOTION) GFL_ASSERT_STOP(0);
  m_NowAnimationNo = type;
}
void FinderStudioDressUpChara::saveNowAnimationNo(OTHER_MOTION_INDEX otherMotionIndex)
{
  m_NowAnimationNo = NUM_Z_MOTION + otherMotionIndex;
}

/// @brief 保存したアニメーション種がZモーションかどうか判定
bool FinderStudioDressUpChara::isZMotionAnimationNo(int animationNo)
{
  if(animationNo >= 0 && animationNo < NUM_Z_MOTION)
  {
    return true;
  }
  else if(animationNo >= NUM_Z_MOTION && animationNo < NUM_Z_MOTION + OTHER_MOTION_MAX)
  {
    return false;
  }
  else
  {
    GFL_ASSERT(0);
    return false;
  }
}

/// @brief 保存したアニメーション種が指定したアニメーション種と同じか判定
bool FinderStudioDressUpChara::isSameAnimationNo(ZMotionType type)
{
  return m_NowAnimationNo == static_cast<u32>(type);
}
bool FinderStudioDressUpChara::isSameAnimationNo(OTHER_MOTION_INDEX otherMotionIndex)
{
  return m_NowAnimationNo == static_cast<u32>(NUM_Z_MOTION + otherMotionIndex);
}

/// @brief Zモーション一時停止
void FinderStudioDressUpChara::StopZMotion()
{
  this->request(REQ_TYPE_STOP_Z_MOTION);
}

/// @brief Zモーションが再生中かどうか
/// @return 再生中かどうか
bool FinderStudioDressUpChara::IsPlayingZMotion() const
{
  if(m_State == CHARA_STATE_Z_MOTION) return true;
  else return false;
}

/// @brief Zモーションが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioDressUpChara::IsFinishZMotion() const
{
  if(m_State == CHARA_STATE_WAIT) return true;
  else return false;
}

/// @brief 待機状態に
/// @note 強制的に待機モーションへ移行する
void FinderStudioDressUpChara::ChangeWaitState()
{
  this->request(REQ_TYPE_RESET);
}

/// @brief トレーナー角度変更（左回り）
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioDressUpChara::RotateLeft()
{
  static const float ROTATE_LEFT_DEG = -3;
  return this->SetRotate(m_IgnoreCollisionDegY + ROTATE_LEFT_DEG);
}
/// @brief トレーナー角度変更（右回り）
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioDressUpChara::RotateRight()
{
  static const float ROTATE_RIGHT_DEG = 3;
  return this->SetRotate(m_IgnoreCollisionDegY + ROTATE_RIGHT_DEG);
}

/// @brief 角度をセット
/// @param degY Y軸角度(0 <= degY < 360)
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioDressUpChara::SetRotate(float degY)
{
  GFL_ASSERT_STOP(abs(degY) < 720.0f);

  // 変更前の角度を保存
  f32 prevDegY = m_DegY;

  // 0 ~ 360度に
  if(degY < 0) degY += 360.0f;
  if(degY >= 360.0f) degY -= 360.0f;

  this->roundDegree(degY);

  // 判定のために一時的に角度を変更
  m_DegY = degY;

  gfl2::math::AABB nowAABB = this->getOrigAABB();
  this->applyAABB(&nowAABB);

  // ポケモンAABBとの判定
  bool isHit = false;
  if(m_pPokemonAABB)
  {
    if(nowAABB.IsCross(*m_pPokemonAABB))
    {
      isHit = true;
    }
  }
  else GFL_ASSERT(0);

  // 角度を戻す
  m_DegY = prevDegY;

  if(!isHit)
  {
    // 当たっていなければ角度更新
    m_DegY = degY;
    // AABBを更新
    this->setupAABB();
  }

  // UI上の角度は常に更新
  m_IgnoreCollisionDegY = degY;

  return isHit;
}

/// @brief 当たり判定を無視したAABBを取得
/// @return AABB
const gfl2::math::AABB FinderStudioDressUpChara::GetAABBIgnoreCollision() const
{
  gfl2::math::AABB origAABB = this->getOrigAABB();
  this->applyIgnoreCollisionAABB(&origAABB);
  return origAABB;
}

/// @brief 角度をリセット
void FinderStudioDressUpChara::ResetRotate()
{ 
  switch(m_Composition)
  {
  case COMPOSITION_1:
    // 正面を向く
    m_DegY = 180;
    m_IgnoreCollisionDegY = m_DegY;
    break;
  case COMPOSITION_2:
    // 左を向く
    m_DegY = 90;
    m_IgnoreCollisionDegY = m_DegY;
    break;
  default:
    break;
  }

  // 丸める
  this->roundDegree(m_DegY);

  this->setupAABB();

  this->request(REQ_TYPE_RESET);
}

/// @brief 時間帯によるカラー変更
/// @param timeZone 時間帯
void FinderStudioDressUpChara::SetConstantColorByTime(TimeZone timeZone)
{
  // 仕様削除

//   static const f32 constantColor[3] = {
//     1.0f,
//     0.9f,
//     0.7f
//   };

  
//   int matNum = m_pCharaObj->GetCharaModel()->GetMaterialCount();
//   gfl2::math::Vector4 color(constantColor[timeZone],constantColor[timeZone],constantColor[timeZone],1.0f);
//   for(int mat = 0; mat<matNum; mat++)
//   {
//     m_pCharaObj->GetCharaModel()->SetMaterialColorConstant(mat,5,color);
//   }
// 
//   for(int i = 0;i<poke_3d::model::DressUpModel::PARTS_MODEL_COUNT;i++)
//   {
//     const poke_3d::model::DressUpModel::PartsModel type = static_cast<poke_3d::model::DressUpModel::PartsModel>(i);
//     if(m_pCharaObj->GetDressUpModel()->GetPartsModel(type))
//     {
//       int matNumDress = m_pCharaObj->GetDressUpModel()->GetPartsModel(type)->GetMaterialCount();
//       for(int mat = 0; mat<matNumDress; mat++)
//       {
//         m_pCharaObj->GetDressUpModel()->GetPartsModel(type)->SetMaterialColorConstant(mat,5,color);
//       }
//     }
//   }
}

/// @brief AABBに回転・許容量を反映
/// @param [out] 反映させるAABB
void FinderStudioDressUpChara::applyAABB(gfl2::math::AABB* AABB) const
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
void FinderStudioDressUpChara::applyIgnoreCollisionAABB(gfl2::math::AABB* AABB) const
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

/// @brief アニメーション更新前のパラメータ保存
void FinderStudioDressUpChara::savePrevParam()
{
  GFL_ASSERT_STOP(m_pCharaObj->GetDressUpModel());

  m_PrevParam.mainFrame = m_pCharaObj->GetDressUpModel()->GetAnimationFrame();
}

#if PM_DEBUG
// 表示・非表示切り替え
void FinderStudioDressUpChara::SetVisible(bool isVisible)
{
  m_pCharaObj->SetVisible(isVisible);
}
#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)
