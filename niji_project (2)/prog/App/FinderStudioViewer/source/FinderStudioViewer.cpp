//======================================================================
/**
 * @file FinderStudioViewer.cpp
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオビューワメイン
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "../include/FinderStudioViewer.h"

// System
#include "System/include/nijiAllocator.h"

// PokeTool
#include "PokeTool/include/PokeModelSystem.h"

// DemoLib
#include "DemoLib/Object/include/Object_GfMdl.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"

// FinderStudioViewer
#include "FinderStudioPokemon.h"
#include "FinderStudioPokemonAABB.h"
#include "FinderStudioDressUpChara.h"
#include "FinderStudioBackGround.h"
#include "FinderStudioRenderSystem.h"
#include "FinderStudioRenderPipeLine.h"
#include "FinderStudioCameraSystem.h"
#include "FinderStudioPokemonDataTable.h"

// arc
#include <arc_def_index/arc_def.h>
#include <arc_index/wazaeff_resource.gaix>

// Debug
#if PM_DEBUG
#include "FinderStudioDebug.h"
#endif

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

//コンストラクタ
FinderStudioViewer::FinderStudioViewer(gfl2::heap::HeapBase* pHeap,const PokeTool::SimpleParam& simpleParam,const poke_3d::model::DressUpParam& dressUpParam):
  m_InitSeq(0)
  ,m_EndSeq(0)
  ,m_pPokeModelSystem(NULL)
  ,m_pCharaModelFactory(NULL)
  ,m_pDressUpModelResourceManager(NULL)
  ,m_pPokeDataTable(NULL)
  ,m_pPokemon(NULL)
  ,m_pChara(NULL)
  ,m_pBackGround(NULL)
  ,m_pFloorMaskModel(NULL)
  ,m_pEdgeMaskModel(NULL)
  ,m_pCameraSystem(NULL)
  ,m_pRenderSystem(NULL)
  ,m_ViewerMode(VIEWER_MODE_NORMAL)
  ,m_CaptureMode(CAPTURE_MODE_BOTH)
  ,m_Composition(COMPOSITION_1)
  ,m_IsUsePhotoParam(false)
  ,m_SeqPhotoProduct(0)
  ,m_BurstCaptureFlag(false)
  ,m_BurstCaptureNum(0)
  ,m_BurstCaptureCnt(0)
  /*  deco  */
  ,m_DecoData()
  ,m_pDecoComponent(NULL)
{
  GFL_ASSERT_STOP(pHeap);

  // 初期化
  memset(m_pPhotoParamArray, 0, sizeof(m_pPhotoParamArray));
  this->setupCommon(pHeap);
  this->setupNormal(simpleParam,dressUpParam);

  int size = sizeof(PhotoParam);
}

FinderStudioViewer::FinderStudioViewer(gfl2::heap::HeapBase* pHeap,const PhotoParam& photoParam,ViewerMode viewerMode):
  m_InitSeq(0)
  ,m_EndSeq(0)
  ,m_pPokeModelSystem(NULL)
  ,m_pCharaModelFactory(NULL)
  ,m_pDressUpModelResourceManager(NULL)
  ,m_pPokeDataTable(NULL)
  ,m_pPokemon(NULL)
  ,m_pChara(NULL)
  ,m_pBackGround(NULL)
  ,m_pFloorMaskModel(NULL)
  ,m_pEdgeMaskModel(NULL)
  ,m_pCameraSystem(NULL)
  ,m_pRenderSystem(NULL)
  ,m_ViewerMode(VIEWER_MODE_NORMAL)
  ,m_CaptureMode(CAPTURE_MODE_BOTH)
  ,m_Composition(COMPOSITION_1)
  ,m_IsUsePhotoParam(false)
  ,m_SeqPhotoProduct(0)
  ,m_BurstCaptureFlag(false)
  ,m_BurstCaptureNum(0)
  ,m_BurstCaptureCnt(0)
  /*  deco  */
  ,m_DecoData()
  ,m_pDecoComponent(NULL)
{
  GFL_ASSERT_STOP(pHeap);

  // 初期化
  memset(m_pPhotoParamArray, 0, sizeof(m_pPhotoParamArray));
  this->setupCommon(pHeap);
  this->setupPhotoReproduction(photoParam,viewerMode);
}

//------------------------------------------------------------------
/**
  *  @brief    写真再現専用ctor
  */
//------------------------------------------------------------------
FinderStudioViewer::FinderStudioViewer(gfl2::heap::HeapBase* pHeap, app::util::AppRenderingManager* pRenderingManager, const PhotoParam& rPhotoParam, const Static::DecorationData& rDecoData)
  :m_InitSeq(0)
  ,m_EndSeq(0)
  ,m_pPokeModelSystem(NULL)
  ,m_pCharaModelFactory(NULL)
  ,m_pDressUpModelResourceManager(NULL)
  ,m_pPokeDataTable(NULL)
  ,m_pPokemon(NULL)
  ,m_pChara(NULL)
  ,m_pBackGround(NULL)
  ,m_pFloorMaskModel(NULL)
  ,m_pEdgeMaskModel(NULL)
  ,m_pCameraSystem(NULL)
  ,m_pRenderSystem(NULL)
  ,m_ViewerMode(VIEWER_MODE_PHOTO)  /*  専用  */
  ,m_CaptureMode(CAPTURE_MODE_BOTH)
  ,m_Composition(COMPOSITION_1)
  ,m_IsUsePhotoParam(false)
  ,m_SeqPhotoProduct(0)
  ,m_BurstCaptureFlag(false)
  ,m_BurstCaptureNum(0)
  ,m_BurstCaptureCnt(0)
  /*  deco  */
  ,m_DecoData()
  ,m_pDecoComponent(NULL)
{
  GFL_ASSERT_STOP(pHeap);
  GFL_ASSERT_STOP(pRenderingManager);

  // 初期化
  memset(m_pPhotoParamArray, 0, sizeof(m_pPhotoParamArray));
  this->setupCommon(pHeap);
  this->setupPhotoReproduction(rPhotoParam, m_ViewerMode);

  /*  deco  */
  m_DecoData  = rDecoData;
  m_pDecoComponent = GFL_NEW(m_pHeap) Deco::DecoComponent();
  m_pDecoComponent->Setup(m_pHeap,pRenderingManager);
}



/// @brief 共通初期化
void FinderStudioViewer::setupCommon(gfl2::heap::HeapBase* pHeap)
{
  m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pHeap,0x13f8000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioViewerHeap");

  // ローカルヒープ確保
  m_pPokeHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,0x5000 + FinderStudioPokemon::POKE_MOTION_HEAP_SIZE + FinderStudioPokemon::POKE_MOTION_AABB_HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioPokemonHeap");
  // トレーナー非常駐モーション使用メモリ 0x3AB00
  const u32 dynamicMotionHeap = 0x40000;
  m_pTrainerHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,dynamicMotionHeap + FinderStudioDressUpChara::MOTION_HEAP_SIZE + System::DressUp::BATTLE_HEAP_SIZE_ANIM + System::DressUp::BATTLE_HEAP_SIZE_MODEL + System::DressUp::BATTLE_HEAP_SIZE_INSTANCE + System::DressUp::BATTLE_HEAP_SIZE_ALLOCATOR,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioTrainerHeap");
  m_pTempHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap->GetLowerHandle(),0x500000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioTempHeap");
  m_pTempHeapSmall = NULL;
  m_pBgHeap = NULL;
  // フロアマスク使用メモリ 0x6660
  m_pFloorMaskHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,0x9000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioFloorMaskHeap");
  // データテーブル使用メモリ 0x2E80
  m_pPokeDataTableHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,0x4000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioPokeDataTableHeap");

  // アロケータ確保
  m_pNijiAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);
  m_pTrainerNijiAllocator = GFL_NEW(m_pTrainerHeap) System::nijiAllocator(m_pTrainerHeap);
  m_pPokemonNijiAllocator = GFL_NEW(m_pPokeHeap) System::nijiAllocator(m_pPokeHeap);
  m_pFloorMaskNijiAllocator = GFL_NEW(m_pFloorMaskHeap) System::nijiAllocator(m_pFloorMaskHeap);

#if PM_DEBUG
  // デバッグメニュー設定
  InitializeDebug(m_pHeap,this);
#endif
}
/// @brief 通常時初期化
void FinderStudioViewer::setupNormal(const PokeTool::SimpleParam& simpleParam,const poke_3d::model::DressUpParam& dressUpParam)
{
  // 初期化パラメータ設定
  m_simpleParam = simpleParam;
  m_dressUpParam = dressUpParam;
  // 再現パラメータは使わない
  m_IsUsePhotoParam = false;
  // 0初期化
  PhotoParam photoParam = {};
  m_PhotoParam = photoParam;

  // 念のためビューワモードは通常
  m_ViewerMode = VIEWER_MODE_NORMAL;
}
/// @brief 再現時初期化
void FinderStudioViewer::setupPhotoReproduction(const PhotoParam& photoParam,ViewerMode viewerMode)
{
  // 写真パラメータ保存
  m_PhotoParam = photoParam;
  // 再現パラメータを使用する
  m_IsUsePhotoParam = true;

  m_ViewerMode = viewerMode;
}

//デストラクタ
FinderStudioViewer::~FinderStudioViewer()
{
  GFL_SAFE_DELETE(m_pDecoComponent);
  GFL_SAFE_DELETE(m_pNijiAllocator);
  GFL_SAFE_DELETE(m_pTrainerNijiAllocator);
  GFL_SAFE_DELETE(m_pPokemonNijiAllocator);
  GFL_SAFE_DELETE(m_pFloorMaskNijiAllocator);

  GFL_DELETE_HEAP(m_pPokeHeap);
  GFL_DELETE_HEAP(m_pTrainerHeap);
  GFL_DELETE_HEAP(m_pBgHeap);
  GFL_DELETE_HEAP(m_pTempHeapSmall);
  GFL_DELETE_HEAP(m_pFloorMaskHeap);
  GFL_DELETE_HEAP(m_pPokeDataTableHeap);

  // デバッグ解放
#if PM_DEBUG
  TerminateDebug();
#endif

  GFL_DELETE_HEAP(m_pHeap);
}

/// @brief 初期化処理
/// @return 初期化処理が終わったかどうか
bool FinderStudioViewer::InitFunc()
{
  enum{
    SEQ_INIT_CREATE_RENDER_SYSTEM,
    SEQ_INIT_CREATE_RENDER_SYSTEM_WAIT,
    SEQ_INIT_CREATE_CAMERA_SYSTEM,
    SEQ_INIT_CREATE_CHARAMODEL_FACTORY,
    SEQ_INIT_CREATE_CHARA,
    SEQ_INIT_CREATE_CHARA_WAIT,
    SEQ_INIT_CREATE_POKEMODEL_SYSTEM,
    SEQ_INIT_CREATE_POKEMODEL_SYSTEM_WAIT,
    SEQ_INIT_READ_POKE_DATA_TABLE,
    SEQ_INIT_READ_POKE_DATA_TABLE_WAIT,
    SEQ_INIT_CREATE_POKEMON,
    SEQ_INIT_CREATE_POKEMON_WAIT,
    SEQ_INIT_CREATE_BG_SYSTEM,
    SEQ_INIT_CREATE_BG_SYSTEM_WAIT,
    SEQ_INIT_CREATE_FLOOR_MASK,
    SEQ_INIT_CREATE_FLOOR_MASK_WAIT,
    /*  deco  */
    SEQ_INIT_INITIALIZE_DECO,
    SEQ_INIT_CREATE_DECO,
    SEQ_INIT_END
  };

  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  switch(m_InitSeq)
  {
  case SEQ_INIT_CREATE_RENDER_SYSTEM:
    // 描画システム作成
    m_pRenderSystem = GFL_NEW(m_pHeap) FinderStudioRenderSystem(m_pHeap,m_pTempHeap,m_pNijiAllocator);
    m_InitSeq++;
    // fall through

  case SEQ_INIT_CREATE_RENDER_SYSTEM_WAIT:
    if(m_pRenderSystem->InitFunc())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_CREATE_CAMERA_SYSTEM:
    // カメラシステム作成
    m_pCameraSystem = GFL_NEW(m_pHeap) FinderStudioCameraSystem();
    // 描画システムにビュー行列、プロジェクション行列を登録
    m_pRenderSystem->RegistViewMtx(m_pCameraSystem->GetViewMtx());
    m_pRenderSystem->RegistProjMtx(m_pCameraSystem->GetProjMtx());
    // 再現用パラメータ適用
    // カメラだけは外から適用させる
    if(m_IsUsePhotoParam) m_pCameraSystem->ApplyCameraPhotoParam(m_PhotoParam);
    m_InitSeq++;
    // fall through

  

  case SEQ_INIT_CREATE_CHARAMODEL_FACTORY:
    // キャラモデルファクトリ初期化
    m_pCharaModelFactory = GFL_NEW(m_pHeap) poke_3d::model::CharaModelFactory();
    m_pCharaModelFactory->Initialize(fileManager,m_pTrainerHeap,ARCID_CHARA_MODEL_BATTLE_NORMAL);
    // ドレスアップモデルリソースマネージャ初期化
    m_pDressUpModelResourceManager = GFL_NEW(m_pHeap) poke_3d::model::DressUpModelResourceManager();
    m_pDressUpModelResourceManager->Initialize(fileManager,m_pTrainerHeap,System::DressUp::GetDressUpArcIdListBattle());
    /*  deco  */
    if(m_pDecoComponent)
    {
      m_pDecoComponent->Request(m_DecoData);
    }

    m_InitSeq++;
    // fall through

  case SEQ_INIT_CREATE_CHARA:
  {
    // 着せ替えデータ不正値チェック
    if(m_IsUsePhotoParam) this->checkDressUpCombination(&m_PhotoParam);

    // キャラ作成
    FinderStudioDressUpChara::Desc desc;
    desc.pHeap = m_pTrainerHeap;
    desc.pWorkHeap = m_pTempHeap;
    desc.pAllocator = m_pTrainerNijiAllocator;
    desc.pObjectRenderPipeLine = m_pRenderSystem->GetRenderPipeLine();
    desc.pCharaModelFactory = m_pCharaModelFactory;
    desc.pDressUpModelResourceManager = m_pDressUpModelResourceManager;
    desc.pDressUpParam = &m_dressUpParam;
    desc.pPhotoParam = &m_PhotoParam;
    desc.isUsePhotoParam = m_IsUsePhotoParam;
    m_pChara = GFL_NEW(m_pTrainerHeap) FinderStudioDressUpChara(desc);

    m_InitSeq++;
    // fall through
  }
  case SEQ_INIT_CREATE_CHARA_WAIT:
    if(m_pChara->InitFunc())
    {
      // ここで一時ヒープは削除
      GFL_DELETE_HEAP(m_pTempHeap);
      m_pTempHeap = NULL;

      // 一時ヒープを小さめに再確保
      m_pTempHeapSmall = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap->GetLowerHandle(),0x180000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioTempHeap");
      // 背景ヒープを確保
      m_pBgHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,0x200000,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioBgHeap");

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_CREATE_POKEMODEL_SYSTEM:
    // ポケモデルシステム作成
    m_pPokeModelSystem = GFL_NEW(m_pHeap) PokeTool::PokeModelSystem();
    m_pPokeModelSystem->InitSystemAsync(m_pHeap,m_pTempHeapSmall->GetLowerHandle(),fileManager,FinderStudioPokemon::GetNumPokeModel());

    m_InitSeq++;
    // fall through

  case SEQ_INIT_CREATE_POKEMODEL_SYSTEM_WAIT:
    if(m_pPokeModelSystem->IsFinishInitSystemAsync())
    {
      m_pPokeModelSystem->CreateSystem(NULL,false,false); // カラーシェーダ不使用
      // ポケモン用ヒープ作成
      PokeTool::PokeModelSystem::HeapOption opt;
      opt.animeType = PokeTool::MODEL_ANIMETYPE_BATTLE;
      opt.useIdModel = false;
      opt.useShadow = false;        // 影不使用
      opt.useColorShader = false;   // カラーシェーダ不使用
      m_pPokeModelSystem->CreatePokeModelHeap(m_pHeap,&opt);

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_READ_POKE_DATA_TABLE:
  {
    m_pPokeDataTable = GFL_NEW(m_pPokeDataTableHeap) PokemonDataTable();
    m_pPokeDataTable->StartLoadData(m_pPokeDataTableHeap,m_pTempHeapSmall);
    m_InitSeq++;
    // fall through
  }

  case SEQ_INIT_READ_POKE_DATA_TABLE_WAIT:
    if(m_pPokeDataTable->IsFinishLoadData())
    {
      //m_pPokeDataTableHeap->Dump();
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_CREATE_POKEMON:
  {
    if(m_IsUsePhotoParam)
    {
      // MonsNoとFormNo不正組み合わせチェック
      this->checkMonsNoFormNoCombination(&m_PhotoParam);
      // ポケモンモーション不正組合せチェック
      this->checkPokeMotion(&m_PhotoParam);
    }
    else
    {
      // MonsNoとFormNo不正組み合わせチェック
      this->checkMonsNoFormNoCombination(m_simpleParam.monsNo,&m_simpleParam.formNo);
    }


    // ポケモン作成
    FinderStudioPokemon::Desc desc;
    desc.pHeap = m_pPokeHeap;
    desc.pWorkHeap = m_pTempHeapSmall;
    desc.pAllocator = m_pPokemonNijiAllocator;
    desc.pObjectRenderPipeLine = m_pRenderSystem->GetRenderPipeLine();
    desc.pPokeModelSystem = m_pPokeModelSystem;
    desc.pPokeDataTable = m_pPokeDataTable;
    desc.pSimpleParam = &m_simpleParam;
    desc.pPhotoParam = &m_PhotoParam;
    desc.isUsePhotoParam = m_IsUsePhotoParam;
    m_pPokemon = GFL_NEW(m_pPokeHeap) FinderStudioPokemon(desc);

    m_InitSeq++;
    // fall through
  }

  case SEQ_INIT_CREATE_POKEMON_WAIT:
    if(m_pPokemon->InitFunc())
    {
      // トレーナーにポケモンのAABBを登録
      m_pChara->RegistPokemonAABB(m_pPokemon->GetAABB());
      // ポケモンにトレーナーのAABBを登録
      m_pPokemon->RegistTrainerAABB(m_pChara->GetAABB());

      m_InitSeq++;
    }
    break;

  

  case SEQ_INIT_CREATE_BG_SYSTEM:
  {
    // 背景システム作成
    FinderStudioBackGround::Desc desc;
    desc.pHeap = m_pBgHeap;
    desc.pWorkHeap = m_pTempHeapSmall;
    desc.pFinderStudioRenderPipeLine = m_pRenderSystem->GetRenderPipeLine();
    desc.pPhotoParam = &m_PhotoParam;
    desc.isUsePhotoParam = m_IsUsePhotoParam;
    m_pBackGround = GFL_NEW(m_pBgHeap) FinderStudioBackGround(desc);

    m_InitSeq++;
    // fall through

  }
  case SEQ_INIT_CREATE_BG_SYSTEM_WAIT:
    if(m_pBackGround->InitFunc())
    {
      // 背景システムにビュー行列、プロジェクション行列を登録（遠景・天球用）
      m_pBackGround->RegistViewMtx(m_pCameraSystem->GetViewMtx());
      m_pBackGround->RegistFarProjMtx(m_pCameraSystem->GetFarProjMtx());
      m_pBackGround->RegistSkyProjMtx(m_pCameraSystem->GetSkyProjMtx());

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_CREATE_FLOOR_MASK:
  {
    m_pFloorMaskModel = GFL_NEW(m_pFloorMaskHeap) DemoLib::Obj::Object_GfMdl(m_pRenderSystem->GetRenderPipeLine());
    m_pEdgeMaskModel = GFL_NEW(m_pFloorMaskHeap) DemoLib::Obj::Object_GfMdl(m_pRenderSystem->GetRenderPipeLine());
    this->startInitFloorMask();
    m_InitSeq++;
  }
  // fall through

  case SEQ_INIT_CREATE_FLOOR_MASK_WAIT:
    if(this->isFinishInitFloorMask())
    {
      m_InitSeq++;
    }
    break;

  case SEQ_INIT_INITIALIZE_DECO:
    if((!m_pDecoComponent) || m_pDecoComponent->InitFunc())
    {
      if(m_pDecoComponent)
      {
        m_pDecoComponent->Request(m_DecoData.deco_param, GFL_NELEMS(m_DecoData.deco_param));
      }
      ++m_InitSeq;
    }
    break;

  case  SEQ_INIT_CREATE_DECO:
    {
      if((!m_pDecoComponent) || m_pDecoComponent->IsLoaded())
      {
        ++m_InitSeq;
      }
    }
    break;

  case SEQ_INIT_END:
    // カメラに座標登録
    m_pCameraSystem->RegistPos(&m_pChara->GetPos(),&m_pPokemon->GetPos());
    // ポケモンにカメラシステム登録
    m_pPokemon->RegistCameraSystem(m_pCameraSystem);

    // ビューアモードによって初期化
    switch(m_ViewerMode)
    {
    case VIEWER_MODE_NORMAL:
      // 構図の適応
      this->ApplyDefaultComposition(false);
      // デフォルトでポケモンと一緒に設定
      this->ChangeCaptureMode(CAPTURE_MODE_BOTH);
      // 特定の背景のみ回転処理を適用
      this->rotateBackGround();
      break;

    case VIEWER_MODE_PHOTO:
      this->reproductPhoto();
      break;

    default:
      GFL_ASSERT_STOP(0);
      break;
    }

    // 次の終了処理に備えてSeq変数をリセット
    m_EndSeq = 0;

    //m_pHeap->Dump();

    return true;
  }

  return false;
}

/// @brief 特定の背景のとき回転処理を行う
void FinderStudioViewer::rotateBackGround()
{
  // リセットの直後に対応するためポケモンのReqPosで判定
  this->rotateBackGround(m_pPokemon->GetReqPos());
}

void FinderStudioViewer::rotateBackGround(gfl2::math::Vector3 pokemonPos)
{
  GFL_ASSERT_STOP(m_pBackGround);
  GFL_ASSERT_STOP(m_pCameraSystem);
  GFL_ASSERT_STOP(m_pPokemon);

  if(m_CaptureMode == CAPTURE_MODE_BOTH)
  {
    // @fix GFMMCat[879] m_pPokemon->GetReqPos()を算出するのに、変更が加わった場合は注意する
    m_pBackGround->RotateBackGround(m_pCameraSystem->GetLength(),pokemonPos);
  }
  else if(m_CaptureMode == CAPTURE_MODE_POKE)
  {
    // ポケモンのみなら元に戻す
    m_pBackGround->ResetRotateBackGround();
  }
  else
  {
    GFL_ASSERT(0);
  }
}

/// @brief 写真再現処理
void FinderStudioViewer::reproductPhoto()
{
  // 撮影モード適用
  this->changeCaptureMode(static_cast<CaptureMode>(m_PhotoParam.captureMode));

  // 構図適用
  bool isCanChange = this->ChangeComposition(static_cast<Composition>(m_PhotoParam.composition));

  // @fix GFMMCat[879] 回転する背景のときは、回転計算前に、写真パラメータのポケモンモーションタイプでリセットを行う
  if(m_pBackGround->IsRotateBackGround())
  {
    m_pPokemon->ResetPositionRotate(static_cast<PokeMotionType>(m_PhotoParam.pokeMotionType));
  }

  // 撮影モードと構図を加味して、特定の背景のみ回転処理を適用
  this->rotateBackGround();

  // 目パチをオフに
  /// @note ここで目パチがリセットされてしまうため反映の前に行う
  m_pPokemon->SetIsAutoBlink(false);
  m_pChara->SetIsAutoBlink(false);

  // 写真パラメータ適応
  // @fix GFMMCat[105] ポケモンの適応でトレーナーとのめり込み判定を行うので先にトレーナーの適応を行う
  m_pChara->ApplyPhotoParamTrainerOther(m_PhotoParam);
  bool isCanPlace = m_pPokemon->ReqApplyPhotoParamPokemonOther(m_PhotoParam);
  m_pBackGround->ApplyPhotoParamBgOther(m_PhotoParam);
  m_pCameraSystem->ApplyCameraPhotoParam(m_PhotoParam);

  // ポケモンが配置できなかった場合、または構図変更に失敗した場合、ビューアのリセットを行う
  if(!isCanPlace || !isCanChange)
  {
    this->ResetViewer();
  }

  // アニメーション更新をオフにする
  /// @note 適応時のアニメーション変更でアニメーションスピードが戻ってしまうので適応後に行う
  m_pChara->SetIsAnimationUpdate(false);
  /// @note ポケモンは通常更新時にオフにする
  //m_pPokemon->SetIsAnimationUpdate(false);
  m_pBackGround->SetIsAnimationUpdate(false);

  // 通常更新
  this->updateNormal();

  // アニメーション更新を元に戻す
  m_pChara->SetIsAnimationUpdate(true);
  m_pChara->SetIsAutoBlink(true);
  m_pPokemon->SetIsAnimationUpdate(true);
  m_pPokemon->SetIsAutoBlink(true);
  m_pBackGround->SetIsAnimationUpdate(true);

  // ビューアモードを変更しておく
  m_ViewerMode = VIEWER_MODE_PHOTO;
}

/// @brief MonsNoとFormNo不正組み合わせチェック
void FinderStudioViewer::checkMonsNoFormNoCombination(PhotoParam* pPhotoParam)
{
  MonsNo monsNo = static_cast<MonsNo>(pPhotoParam->monsNo);
  FormNo formNo = static_cast<FormNo>(pPhotoParam->formNo);
  this->checkMonsNoFormNoCombination(monsNo,&formNo);
  pPhotoParam->formNo = formNo;
}
void FinderStudioViewer::checkMonsNoFormNoCombination(MonsNo monsNo,FormNo* pFormNo)
{
  if(!pFormNo)
  {
    GFL_ASSERT(0);
    return;
  }

  // かわいがり不可ポケモンテーブル
  static const u32 NUM_NO_KAWAIGARI_POKE = 71;  // かわいがり不可ポケモン一覧.xlsxから算出
  static const u32 NO_KAWAIGARI_POKE_TABLE[NUM_NO_KAWAIGARI_POKE][2] = {
    {MONSNO_HUSIGIBANA,1},
    {MONSNO_RIZAADON,1},
    {MONSNO_RIZAADON,2},
    {MONSNO_KAMEKKUSU,1},
    {MONSNO_SUPIAA,1},
    {MONSNO_PIZYOTTO,1},
    {MONSNO_HUUDHIN,1},
    {MONSNO_YADORAN,1},
    {MONSNO_GENGAA,1},
    {MONSNO_GARUURA,1},
    {MONSNO_KAIROSU,1},
    {MONSNO_GYARADOSU,1},
    {MONSNO_PUTERA,1},
    {MONSNO_MYUUTUU,1},
    {MONSNO_MYUUTUU,2},

    {MONSNO_DENRYUU,1},
    {MONSNO_HAGANEERU,1},
    {MONSNO_HASSAMU,1},
    {MONSNO_HERAKUROSU,1},
    {MONSNO_HERUGAA,1},
    {MONSNO_BANGIRASU,1},

    {MONSNO_ZYUKAIN,1},
    {MONSNO_BASYAAMO,1},
    {MONSNO_RAGURAAZI,1},
    {MONSNO_SAANAITO,1},
    {MONSNO_YAMIRAMI,1},
    {MONSNO_KUTIITO,1},
    {MONSNO_BOSUGODORA,1},
    {MONSNO_TYAAREMU,1},
    {MONSNO_RAIBORUTO,1},
    {MONSNO_SAMEHADAA,1},
    {MONSNO_BAKUUDA,1},
    {MONSNO_TIRUTARISU,1},
    {MONSNO_POWARUN,1},
    {MONSNO_POWARUN,2},
    {MONSNO_POWARUN,3},
    {MONSNO_ZYUPETTA,1},
    {MONSNO_ABUSORU,1},
    {MONSNO_ONIGOORI,1},
    {MONSNO_BOOMANDA,1},
    {MONSNO_METAGUROSU,1},
    {MONSNO_RATHIASU,1},
    {MONSNO_RATHIOSU,1},
    {MONSNO_KAIOOGA,1},
    {MONSNO_GURAADON,1},
    {MONSNO_REKKUUZA,1},

    {MONSNO_THERIMU,1},
    {MONSNO_MIMIROPPU,1},
    {MONSNO_GABURIASU,1},
    {MONSNO_RUKARIO,1},
    {MONSNO_YUKINOOO,1},
    {MONSNO_ERUREIDO,1},

    {MONSNO_TABUNNE,1},
    {MONSNO_HIHIDARUMA,1},
    {MONSNO_MEROETTA,1},

    {MONSNO_GEKKOUGA,2},
    {MONSNO_GIRUGARUDO,1},
    {MONSNO_ZERUNEASU,1},
    {MONSNO_ZIGARUDE,4},
    {MONSNO_DHIANSII,1},

    {MONSNO_YOWASI,1},
    {MONSNO_NAGAREBOSI,0},
    {MONSNO_NAGAREBOSI,1},
    {MONSNO_NAGAREBOSI,2},
    {MONSNO_NAGAREBOSI,3},
    {MONSNO_NAGAREBOSI,4},
    {MONSNO_NAGAREBOSI,5},
    {MONSNO_NAGAREBOSI,6},
    {MONSNO_HORAA,1},
    {MONSNO_HORAA,3},

    {MONSNO_PURIZUMU,3},
  };

  // MonsNoとFormNo組み合わせチェック
  bool isError = false;
  for(u32 i = 0;i < NUM_NO_KAWAIGARI_POKE;i++)
  {
    // 抜けチェック
    if(NO_KAWAIGARI_POKE_TABLE[i][0] == 0 &&
      NO_KAWAIGARI_POKE_TABLE[i][1] == 0)
    {
      GFL_ASSERT(0);  // 開発中に気付けばよい
    }

    if(NO_KAWAIGARI_POKE_TABLE[i][0] == monsNo &&
      NO_KAWAIGARI_POKE_TABLE[i][1] == *pFormNo)
    {
      isError = true;
      break;
    }
  }

  if(isError)
  {
    // 不正な組み合わせだった場合FormNoを0に
    *pFormNo = 0;
    // メテノの場合のみ7にする
    if(monsNo == MONSNO_NAGAREBOSI)
    {
      *pFormNo = 7;
    }
  }
}

/// @brief 着せ替えデータ不正組み合わせチェック
void FinderStudioViewer::checkDressUpCombination(PhotoParam* pPhotoParam)
{
  GFL_ASSERT_STOP(pPhotoParam);
  GFL_ASSERT_STOP(m_pDressUpModelResourceManager);

  // 着せ替えデータ不正組み合わせチェック
  poke_3d::model::DressUpParam dressUpParam = FinderStudioDressUpChara::GetDressUpParamFromPhotoParam(*pPhotoParam);
  poke_3d::model::DressUpParam::Sex sex = static_cast<poke_3d::model::DressUpParam::Sex>(dressUpParam.bodyParams[poke_3d::model::DressUpParam::BODY_CATEGORY_SEX]);
  // @fix GFMMCat[72] FinderStudio用に作成したバグ修正済みの関数を呼ぶよう変更
  if(!m_pDressUpModelResourceManager->GetDressUpModelResourceManagerCore(sex)->CheckDressUpCombinationFinderStudio(dressUpParam))
  {
    // 不正な組み合わせだった場合デフォルト衣装へ戻す
    System::DressUp::GetDressUpParamDefault(sex,&dressUpParam);
    FinderStudioDressUpChara::SetDressUpParamToPhotoParam(pPhotoParam,dressUpParam);
  }
}
/// @brief ポケモンモーション不正組合せチェック
void FinderStudioViewer::checkPokeMotion(PhotoParam* pPhotoParam)
{
  GFL_ASSERT_STOP(pPhotoParam);

  PokeMotionType pokeMotionType = static_cast<PokeMotionType>(pPhotoParam->pokeMotionType);
  u32 pokeMotionNo = pPhotoParam->pokeMotionNo;
  if(!PokemonAABB::IsCanUseMotion(pokeMotionType,pokeMotionNo))
  {
    // 不正な値なら待機モーションにする
    pPhotoParam->pokeMotionType = POKE_MOTION_TYPE_BATTLE;
    pPhotoParam->pokeMotionNo = PokeTool::MODEL_ANIME_BT_WAIT_A;
  }
}

/// @brief フロアマスク初期化
void FinderStudioViewer::startInitFloorMask()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  // ファイルオープン
  gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
  openReq.arcId = ARCID_WAZAEFF_RESOURCE;
  openReq.heapForFile = m_pTempHeapSmall->GetLowerHandle();
  openReq.heapForReq = m_pTempHeapSmall->GetLowerHandle();
  fileManager->SyncArcFileOpen(openReq);

  // フロアマスクロード
  {
    DemoLib::Obj::Object_GfMdl::LOAD_PARAM param;
    param.arcId = ARCID_WAZAEFF_RESOURCE;
    param.mdlDataId = GARC_wazaeff_resource_mask_floor_lz_GFBMDLP;
    param.isComp = true;
    param.dataHeap = m_pFloorMaskHeap;
    param.workHeap = m_pTempHeapSmall->GetLowerHandle();

    m_pFloorMaskModel->StartLoad(fileManager,param);
  }

  // エッジマスクロード
  {
    DemoLib::Obj::Object_GfMdl::LOAD_PARAM param;
    param.arcId = ARCID_WAZAEFF_RESOURCE;
    param.mdlDataId = GARC_wazaeff_resource_edge_floor_lz_GFBMDLP;
    param.isComp = true;
    param.dataHeap = m_pFloorMaskHeap;
    param.workHeap = m_pTempHeapSmall->GetLowerHandle();

    m_pEdgeMaskModel->StartLoad(fileManager,param);
  }
}
bool FinderStudioViewer::isFinishInitFloorMask()
{
  gfl2::fs::AsyncFileManager* fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if(m_pFloorMaskModel->IsFinishLoad() &&
    m_pEdgeMaskModel->IsFinishLoad())
  {
    // ファイルクローズ
    gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
    closeReq.arcId = ARCID_WAZAEFF_RESOURCE;
    fileManager->SyncArcFileClose(closeReq);

    {
      m_pFloorMaskModel->Create(m_pFloorMaskHeap,m_pFloorMaskNijiAllocator);
      m_pRenderSystem->GetRenderPipeLine()->AddFloorMask(m_pFloorMaskModel->GetModelNode());
      m_pFloorMaskModel->SetVisible(true);
      //影が埋まるのでちょっと下げる
      m_pFloorMaskModel->SetPosition(0,-0.2f,0);
    }

    {
      m_pEdgeMaskModel->Create(m_pFloorMaskHeap,m_pFloorMaskNijiAllocator);
      m_pRenderSystem->GetRenderPipeLine()->AddEdgeMask(m_pEdgeMaskModel->GetModelNode());
#if defined(GF_PLATFORM_CTR)
      m_pEdgeMaskModel->GetModelNode()->CopyMaterialDepthStencilStateToStateObject();
#endif
      m_pEdgeMaskModel->SetVisible(true);
      //影が埋まるのでちょっと下げる
      m_pEdgeMaskModel->SetPosition(0,-0.2f,0);
    }

    //m_pFloorMaskHeap->Dump();

    return true;
  }

  return false;
}

/// @brief 終了処理
/// @return 終了処理が終わったかどうか
bool FinderStudioViewer::EndFunc()
{
  enum{
    SEQ_END_WAIT,
    SEQ_END_DELETE_POKEMON,
    SEQ_END_DELETE_CHARA,
    SEQ_END_DELETE_POKEMODEL_SYSTEM,
    SEQ_END_DELETE_POKEMODEL_SYSTEM_WAIT,
    SEQ_END_DELETE_CHARAMODEL_FACTORY,
    SEQ_END_DELETE_BG_SYSTEM,
    SEQ_END_DELETE_FLOOR_MASK,
    SEQ_END_DELETE_POKEMON_DATA,
    SEQ_END_DELETE_CAMERA_SYSTEM,
    SEQ_END_DELETE_RENDER_SYSTEM,
    /*  deco  */
    SEQ_END_RELEASE_DECO,
    SEQ_END_END
  };


  switch(m_EndSeq)
  {
  case SEQ_END_WAIT:
    // モデル削除のため１フレーム待つ
    m_EndSeq++;
    break;

  case SEQ_END_DELETE_POKEMON:
    // ポケモン削除
    if(m_pPokemon->EndFunc())
    {
      GFL_SAFE_DELETE(m_pPokemon);
      m_EndSeq++;
    }
    break;

  case SEQ_END_DELETE_CHARA:
    // キャラ削除
    if(m_pChara->EndFunc())
    {
      GFL_SAFE_DELETE(m_pChara);
      m_EndSeq++;
    }
    break;
  
  case SEQ_END_DELETE_POKEMODEL_SYSTEM:
    // ポケモン用ヒープ削除
    m_pPokeModelSystem->DeletePokeModelHeap();
    // ポケモンモデルシステム解放開始
    m_pPokeModelSystem->TermSystemAsync();

    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_POKEMODEL_SYSTEM_WAIT:
    if(m_pPokeModelSystem->IsFinishTermSystemAsync())
    {
      GFL_SAFE_DELETE(m_pPokeModelSystem);

      m_EndSeq++;
    }
    break;

  case SEQ_END_DELETE_CHARAMODEL_FACTORY:
    // キャラモデルファクトリ削除
    m_pCharaModelFactory->Finalize();
    GFL_SAFE_DELETE(m_pCharaModelFactory);

    // ドレスアップモデルリソースマネージャ削除
    m_pDressUpModelResourceManager->Finalize();
    GFL_SAFE_DELETE(m_pDressUpModelResourceManager);
    
    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_BG_SYSTEM:
    // 背景システム削除
    if(m_pBackGround->Terminate())
    {
      GFL_SAFE_DELETE(m_pBackGround);
      m_EndSeq++;
    }
    break;

  case SEQ_END_DELETE_FLOOR_MASK:
    // フロアマスク削除
    m_pRenderSystem->GetRenderPipeLine()->RemoveFloorMask(m_pFloorMaskModel->GetModelNode());
    m_pFloorMaskModel->Delete();
    GFL_SAFE_DELETE(m_pFloorMaskModel);

    m_pRenderSystem->GetRenderPipeLine()->RemoveEdgeMask(m_pEdgeMaskModel->GetModelNode());
    m_pEdgeMaskModel->Delete();
    GFL_SAFE_DELETE(m_pEdgeMaskModel);
    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_POKEMON_DATA:
    m_pPokeDataTable->DeleteData();
    GFL_SAFE_DELETE(m_pPokeDataTable);
    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_CAMERA_SYSTEM:
    // カメラシステム削除
    GFL_SAFE_DELETE(m_pCameraSystem);
    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_RENDER_SYSTEM:
    // 描画システム削除
    m_pRenderSystem->Terminate();
    GFL_SAFE_DELETE(m_pRenderSystem);
    m_EndSeq++;
    // fall through

  case  SEQ_END_RELEASE_DECO:
    if((!m_pDecoComponent) || m_pDecoComponent->EndFunc())
    {
      m_EndSeq++;
    }
    break;

  case SEQ_END_END:
    // 次の初期化に備えてSeq変数をリセット
    m_InitSeq = 0;
    return true;
  }
  return false;
}

// 更新
void FinderStudioViewer::Update()
{
#if PM_DEBUG
  // デバッグ更新
  UpdateDebug();
#endif

  switch(m_ViewerMode)
  {
  case VIEWER_MODE_NORMAL:
    // 通常更新
    this->updateNormal();
    break;

  case VIEWER_MODE_PHOTO:
    // 写真再現時更新
    this->updatePhotoReproduction();
    break;
  }

  /*  deco  */
  if(m_pDecoComponent)  m_pDecoComponent->Update();
}

/// @brief 通常更新
void FinderStudioViewer::updateNormal()
{
  // ポケモン更新
  m_pPokemon->Update();

  // キャラモデル更新
  m_pChara->Update();

  // カメラ更新
  m_pCameraSystem->Update();

  // 背景更新
  m_pBackGround->Update();

  // ライト更新
#if PM_DEBUG
  if(!IsUseDebugLight())
  {
#endif
    gfl2::math::Vector3 vec(0,0,-1);
    FinderStudioCameraSystem::RotateXVec(&vec,m_pCameraSystem->GetDegX());
    FinderStudioCameraSystem::RotateYVec(&vec,m_pCameraSystem->GetDegY());
    m_pRenderSystem->SetLightDirection(vec);
    m_pPokemon->SetLightDirection(vec);
#if PM_DEBUG
  }
#endif

  // 描画システム更新
  m_pRenderSystem->Update();

  // 撮影処理
  if(m_BurstCaptureFlag)
  {
    this->capture();
  }

  // 再読み込み終了時処理
  if(m_pPokemon->IsLoadEndFrame())this->onReloadedPokemon();
  if(m_pBackGround->IsLoadEndFrame()) this->onReloadedBackGround();
}

/// @brief ポケモン再読み込み終了時処理
void FinderStudioViewer::onReloadedPokemon()
{
  // デフォルトの構図を適用
  this->ApplyDefaultComposition();
  // 特定の背景のみ回転処理を適用
  this->rotateBackGround();
}

/// @brief 背景再読み込み終了時処理
void FinderStudioViewer::onReloadedBackGround()
{
  // 特定の背景のときのみリセットを行う
  if(m_pBackGround->IsRotateBackGround())
  {
    this->ResetViewer();
  }
  this->rotateBackGround();
}

/// @brief 撮影処理
void FinderStudioViewer::capture()
{
  // 撮影間隔フレーム
  static const u32 CAPTURE_INTERVAL = 5;

  m_BurstCaptureCnt++;

  // 5フレームに1枚撮る
  if(m_BurstCaptureCnt >= CAPTURE_INTERVAL)
  {
    // 撮り終わっていれば次の写真へ
    if(m_CaptureTexture[m_BurstCaptureNum].isFinishCapture)
    {
      m_BurstCaptureNum++;
      // カウントリセット
      m_BurstCaptureCnt = 0;
      // 最大枚数取り終えていれば終了
      if(m_BurstCaptureNum == BURST_CAPTURE_NUM)
      {
        m_BurstCaptureFlag = false;
        m_BurstCaptureNum = 0;
        return;
      }
      else
      {
        // 次の写真のキャプチャーリクエストを出す
        m_pRenderSystem->RequestTransferColorBuffer(&m_CaptureTexture[m_BurstCaptureNum]);

        // 次の写真パラメータ保存
        GFL_ASSERT_STOP(m_pPhotoParamArray[m_BurstCaptureNum]);
        this->SavePhotoParam(m_pPhotoParamArray[m_BurstCaptureNum]);

        return;
      }
    }
  }

}

/// @brief 写真パラメータ保存
bool FinderStudioViewer::savePhotoParam(PhotoParam* pPhotoParam)
{
  GFL_ASSERT_STOP(pPhotoParam);

  // 全てのパラメータが取得可能なら取得する
  if(m_pPokemon->IsCanGetPhotoParam() &&
    m_pChara->IsCanGetPhotoParam() &&
    m_pBackGround->IsCanGetPhotoParam())
  {
    m_pPokemon->GetPhotoParamPokemon(pPhotoParam);
    m_pChara->GetPhotoParamTrainer(pPhotoParam);
    m_pBackGround->GetPhotoParamBg(pPhotoParam);
    m_pCameraSystem->GetPhotoParamCamera(pPhotoParam);
    pPhotoParam->captureMode = m_CaptureMode;
    pPhotoParam->composition = m_Composition;
    return true;
  }
  else
  {
    return false;
  }
}

/// @brief 写真再現時更新
void FinderStudioViewer::updatePhotoReproduction()
{
  m_pRenderSystem->Update();
}

// 描画
void FinderStudioViewer::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pRenderSystem->Draw(displayNo);

  if(m_pDecoComponent)  m_pDecoComponent->Draw(displayNo);

#if PM_DEBUG
  m_pPokemon->DebugDraw(displayNo);
#endif
}

enum{
  SEQ_PHOTO_REPRODUCT_NONE,
  SEQ_PHOTO_REPRODUCT_LOAD_WAIT,
  SEQ_PHOTO_REPRODUCT_DECO_WAIT,
  SEQ_PHOTO_REPRODUCT_FINISH
};

/// @brief 写真再現開始判定
/// @return 写真再現処理が開始できるか
bool FinderStudioViewer::IsCanStartPhotoProduct() const
{
  if(m_SeqPhotoProduct == SEQ_PHOTO_REPRODUCT_NONE)
  {
    return true;
  }

  return false;
}
/// @brief 写真再現開始
/// @attention 必ずIsCanStartPhotoProduct関数がtrueのときに呼んでください
void FinderStudioViewer::StartPhotoReproduct(const PhotoParam& photoParam, const Static::DecorationData* pDecoData)
{
  if(m_SeqPhotoProduct != SEQ_PHOTO_REPRODUCT_NONE)
  {
    GFL_ASSERT_MSG(0,"今は再現を開始できません\n");
    return;
  }

  m_PhotoParam  = photoParam;
  // 不正値チェック
  this->checkMonsNoFormNoCombination(&m_PhotoParam);
  this->checkPokeMotion(&m_PhotoParam);
  this->checkDressUpCombination(&m_PhotoParam);
  // モデル・モーション更新のため、強制的に通常モードに
  m_ViewerMode = VIEWER_MODE_NORMAL;

  // 着せ替え変更
  poke_3d::model::DressUpParam dressUpParam = m_pChara->GetDressUpParamFromPhotoParam(m_PhotoParam);
  m_pChara->StartReloadDressUpModel(dressUpParam);

  // ポケモン変更
  m_pPokemon->ApplyPhotoParamSimplePokeParam(m_PhotoParam);
  m_pPokemon->StartReloadModel();

  // 背景変更
  m_pBackGround->ApplyPhotoParamBgSetIndexTimeZone(m_PhotoParam);
  m_pBackGround->StartReload();

  // deco
  if(m_pDecoComponent && pDecoData)
  {
    m_DecoData    = *pDecoData;
    m_pDecoComponent->Request(m_DecoData);
  }

  m_SeqPhotoProduct = SEQ_PHOTO_REPRODUCT_LOAD_WAIT;
}
/// @brief 写真再現完了
/// @return 写真再現が完了したかどうか
/// @attention StartPhotoReproduct関数を呼んだら必ずtrueが帰るまでこの関数を呼んでください
bool FinderStudioViewer::IsFinishPhotoReproduct()
{
  switch(m_SeqPhotoProduct)
  {
  case SEQ_PHOTO_REPRODUCT_LOAD_WAIT:
  {
    bool isFinishDressUp = m_pChara->IsFinishReloadDressUpModel();
    bool isFinishPokemon = m_pPokemon->IsFinishReloadModel();
    bool isFinishBg = m_pBackGround->IsFinishReloadModel();
    if(isFinishDressUp && isFinishPokemon && isFinishBg)
    {
      m_SeqPhotoProduct++;
    }
    break;
  }

  case SEQ_PHOTO_REPRODUCT_DECO_WAIT:
    if((!m_pDecoComponent) || m_pDecoComponent->IsLoaded())
    {
      m_SeqPhotoProduct++;
    }
    break;

  case SEQ_PHOTO_REPRODUCT_FINISH:
    this->reproductPhoto();
    m_SeqPhotoProduct = SEQ_PHOTO_REPRODUCT_NONE;
    
    return true;
  }

  return false;
}


/// @brief 写真再現パラメータ出力
/// @param[out] pPhotoParam 写真再現パラメータ
/// @note 内部の変数にもパラメータは保存される ViewerをDELETEしない場合は引数NULLで初期化しなおしでもOK
void FinderStudioViewer::SavePhotoParam(PhotoParam* pPhotoParam)
{
  this->savePhotoParam(&m_PhotoParam);

  if(pPhotoParam)
  {
    *pPhotoParam = m_PhotoParam;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

/// @brief ビューワのパラメータをリセット
/// @note ポケモンのモーション、位置、角度、トレーナー角度、カメラ情報をリセット
void FinderStudioViewer::ResetViewer()
{
  /// @note キャラのリセットを行ってからポケモンをリセットしないとAABBリセットの関係で座標がずれる
  m_pChara->ResetRotate();
  m_pPokemon->ResetPositionRotate();
  m_pCameraSystem->ResetCamera();
}


/// @brief 撮影モード切替
/// @param 撮影モード
void FinderStudioViewer::ChangeCaptureMode(CaptureMode captureMode)
{
  this->changeCaptureMode(captureMode);
  this->ResetViewer();
  // 特定の背景のみ回転処理を適用
  this->rotateBackGround();
}

/// @brief 撮影モード切替本体
/// @param captureMode 撮影モード
/// @note 全体のリセットは行わない
void FinderStudioViewer::changeCaptureMode(CaptureMode captureMode)
{
  m_CaptureMode = captureMode;

  // AABBの登録
  if(m_CaptureMode == CAPTURE_MODE_POKE)
  {
    m_pPokemon->ResetTrainerAABB();
  }
  else if(m_CaptureMode == CAPTURE_MODE_BOTH)
  {
    m_pPokemon->RegistTrainerAABB(m_pChara->GetAABB());
  }
  else
  {
    GFL_ASSERT(0);
  }

  // 座標の移動・表示非表示の切り替えなどを行う
  m_pPokemon->OnChangeCaptureMode(m_CaptureMode);
  m_pChara->OnChangeCaptureMode(m_CaptureMode);
  m_pCameraSystem->OnChangeCaptureMode(m_CaptureMode);
}

/// @brief ポケモン切り替え
/// @param pokeSimpleParam ポケモンデータ
void FinderStudioViewer::StartChangePokeModel(const PokeTool::SimpleParam& pokeSimpleParam)
{
  // 不正値チェック
  PokeTool::SimpleParam simpleParam = pokeSimpleParam;
  this->checkMonsNoFormNoCombination(simpleParam.monsNo,&simpleParam.formNo);

  m_pPokemon->StartReloadModel(simpleParam);
}

/// @brief ポケモン切り替えが終わったか
/// @return 終わったかどうか
bool FinderStudioViewer::IsFinishChangePokeModel()
{
  bool isFinish = m_pPokemon->IsFinishReloadModel();
  if(isFinish)
  {
    // カラーが初期化されているので時間帯によって変更する
    m_pPokemon->SetConstantColorByTime(m_pBackGround->GetTimeZone());
  }
  return isFinish;
}

/// @brief 背景切り替え
/// @param bgSetIndex 背景セットインデックス
void FinderStudioViewer::StartChangeBackGround(u32 bgSetIndex)
{
  m_pBackGround->StartReload(bgSetIndex);
}

/// @brief 背景切り替えが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioViewer::IsFinishChangeBackGround() const
{
  return m_pBackGround->IsFinishReloadModel();
}

/// @brief 時間帯変更
/// @param timeZone 時間帯
void FinderStudioViewer::ChangeTimeZone(TimeZone timeZone)
{
  m_pBackGround->SetTimeZone(timeZone);
  m_pPokemon->SetConstantColorByTime(timeZone);
  m_pChara->SetConstantColorByTime(timeZone);
}

/// @brief トレーナー衣装切り替え
/// @param dressUpParam 着せ替えデータ
void FinderStudioViewer::StartChangeDressUp(poke_3d::model::DressUpParam dressUpParam)
{
  m_pChara->StartReloadDressUpModel(dressUpParam);
}

/// @brief 衣装切り替えが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioViewer::IsFinishChangeDressUp()
{
  bool isFinish = m_pChara->IsFinishReloadDressUpModel();
  if(isFinish)
  {
    // カラーが初期化されているので時間帯によって変更する
    m_pChara->SetConstantColorByTime(m_pBackGround->GetTimeZone());
  }
  return isFinish;
}

/// @brief 構図使用可能判定
bool FinderStudioViewer::IsCanUseComposition(Composition composition) const
{
  u32 monsNo = m_pPokemon->GetSimpleParam().monsNo;
  u32 formNo = m_pPokemon->GetSimpleParam().formNo;
  const PokemonDataTable::PokemonData* data = m_pPokeDataTable->GetData(monsNo,formNo);
  GFL_ASSERT_STOP_MSG(data,"構図データが見つかりません\n");

  bool isUseComposition = false;
  switch(composition)
  {
  case COMPOSITION_1:
    isUseComposition = data->canUseComposition1;
    break;
  case COMPOSITION_2:
    isUseComposition = data->canUseComposition2;
    break;
  default:
    GFL_ASSERT_MSG(0,"存在しない構図です\n");
    isUseComposition = false;
    break;
  }
  return isUseComposition;
}

/// @brief 構図切り替え
/// @param composition 構図の種類
/// @param isReset ビューワの状態をその構図でリセットするか
/// @return 構図変更が成功したかどうか
bool FinderStudioViewer::ChangeComposition(Composition composition,bool isReset)
{
  bool isSuccess = true;

  // 使用禁止チェック
  if(!this->IsCanUseComposition(composition))
  {
    GFL_ASSERT_MSG(0,"使用禁止に設定されている構図です\n");
    //GFL_ASSERT_STOP_MSG(0,"使用禁止に設定されている構図です\n");
    // データ改ざんにより禁止チェックに引っかかった場合はもう片方の構図を使用する
    if(composition == COMPOSITION_1) composition = COMPOSITION_2;
    else if(composition == COMPOSITION_2) composition = COMPOSITION_1;

    isSuccess = false;
  }

  m_Composition = composition;

  u32 monsNo = m_pPokemon->GetSimpleParam().monsNo;
  u32 formNo = m_pPokemon->GetSimpleParam().formNo;
  const PokemonDataTable::PokemonData* data = m_pPokeDataTable->GetData(monsNo,formNo);
  GFL_ASSERT_STOP_MSG(data,"構図データが見つかりません\n");

  u32 distance = 0;
  u32 zoom = 0;
  switch(m_Composition)
  {
  case COMPOSITION_1:
    distance = data->distanceComposition1;
    zoom = data->zoomComposition1;
    break;
  case COMPOSITION_2:
    distance = data->distanceComposition2;
    zoom = data->zoomComposition2;
    break;
  default:
    GFL_ASSERT_STOP_MSG(0,"存在しない構図です\n");
    break;
  }

  // 各パラメータに適用
  m_pPokemon->ApplyComposition(m_Composition,distance);
  m_pChara->ApplyComposition(m_Composition);
  m_pCameraSystem->ApplyComposition(zoom);

  if(isReset)
  {
    this->ResetViewer();
    // 特定の背景のみ回転処理を適用
    this->rotateBackGround();
  }

  return isSuccess;
}

/// @param isReset ビューワの状態をその構図でリセットするか
/// @brief 適用できる最も上の構図パラメータを適応
/// @param isReset ビューワの状態をその構図でリセットするか（基本trueで大丈夫です）
void FinderStudioViewer::ApplyDefaultComposition(bool isReset)
{
  // 適用できる最も上の構図で座標、カメラの初期化を行う
  if(this->IsCanUseComposition(COMPOSITION_1))
  {
    this->ChangeComposition(COMPOSITION_1,isReset);
  }
  else if(this->IsCanUseComposition(COMPOSITION_2))
  {
    this->ChangeComposition(COMPOSITION_2,isReset);
  }
  else
  {
    GFL_ASSERT_STOP_MSG(0,"使用可能な構図がありません\n");
  }
}

/// @brief 現在の構図を取得
/// @return 構図
Composition FinderStudioViewer::GetComposition() const
{
  return m_Composition;
}

/// @brief Zモーション再生
/// @param ZMotionType Zモーション種
/// @return 再生に成功したら（すでに再生中なら失敗）
bool FinderStudioViewer::PlayZMotion(ZMotionType ZMotionType)
{
  return m_pChara->PlayZMotion(ZMotionType);
}

/// @brief Zモーション一時停止
void FinderStudioViewer::StopZMotion()
{
  m_pChara->StopZMotion();
}

/// @brief Zモーションが再生中かどうか
/// @return 再生中かどうか
bool FinderStudioViewer::IsPlayingZMotion() const
{
  return m_pChara->IsPlayingZMotion();
}

/// @brief Zモーションが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioViewer::IsFinishZMotion() const
{
  return m_pChara->IsFinishZMotion();
}

/// @brief トレーナーを待機状態に
/// @note 強制的に待機モーションへ移行する
void FinderStudioViewer::ChangeTrainerWaitState()
{
  m_pChara->ChangeWaitState();
}

/// @brief ポケモンモーション再生
void FinderStudioViewer::PlayPokeMotion()
{
  m_pPokemon->PlayMotion();
}

/// @brief ポケモンモーションが終わったかどうか
bool FinderStudioViewer::IsFinishPokeMotion() const
{
  return m_pPokemon->IsFinishMotion();
}

/// @brief ポケモンモーションタイプ変更がリセットせずに行えるかどうか
/// @param pokeMotionType ポケモンモーションタイプ
/// @return true 押し出しのみで可能 false リセットしないと不可能
bool FinderStudioViewer::IsCanChangePokeMotionTypeNoReset(PokeMotionType pokeMotionType)
{
  return m_pPokemon->IsCanChangeMotionTypeNoReset(pokeMotionType);
}

/// @brief ポケモンモーションタイプ変更
/// @param pokeMotionType ポケモンモーションタイプ
/// @param isReset モーションタイプ変更時にリセットを行うか
/// @return 現在再生しているタイプと同じタイプかどうか
bool FinderStudioViewer::ChangePokeMotionType(PokeMotionType pokeMotionType,bool isReset)
{
  // @fix GFMMCat[957] ポケモンモーションタイプ変更時にも背景の回転計算を行う
  //                   リセットをするしないに関わらず、リセットしたときの座標から回転角度を算出する
  if(m_pBackGround->IsRotateBackGround())
  {
    gfl2::math::Vector3 resetPos;
    f32 resetDegY = 0;
    m_pPokemon->GetResetPosistionRotate(&resetPos,&resetDegY,pokeMotionType);
    this->rotateBackGround(resetPos);
  }

  if(isReset) this->ResetViewer();
  return m_pPokemon->ChangeMotionType(pokeMotionType,isReset);
}

/// @brief ポケモンモーションタイプを取得
/// @return ポケモンモーションタイプ
PokeMotionType FinderStudioViewer::GetPokeMotionType() const
{
  return m_pPokemon->GetPokeMotionType();
}

/// @brief ポケモンモーションタイプが変更済みかどうか
bool FinderStudioViewer::IsChangedPokeMotionType() const
{
  return m_pPokemon->IsFinishChangeMotionType();
}

/// @brief ポケモンが現在再生できるモーション数の割合を取得
/// @return 再生できるモーション割合(0 ~ 1)
f32 FinderStudioViewer::GetRatePokeCanPlayMotion() const
{
  return m_pPokemon->GetRateCanPlayMotion();
}

/// @brief ポケモンが現在再生できるモーション数を取得
/// @return 再生できるモーション数
u32 FinderStudioViewer::GetNumPokeCanPlayMotion() const
{
  return m_pPokemon->GetNumCanPlayMotion();
}

/// @brief ポケモンが自動でモーションするかを切り替える
/// @param true 自動モーションオン  false 自動モーションオフ
void FinderStudioViewer::SetPokeEnableAutoMotion(bool isEnableAutoMotion)
{
  m_pPokemon->SetEnableAutoMotion(isEnableAutoMotion);
}

/// @brief カメラY軸回転（左方向）
void FinderStudioViewer::CameraRotateYLeft()
{
  m_pCameraSystem->RotateYLeft();
}
/// @brief カメラY軸回転（右方向）
void FinderStudioViewer::CameraRotateYRight()
{
  m_pCameraSystem->RotateYRight();
}

/// @brief カメラX軸回転（上方向）
void FinderStudioViewer::CameraRotateXUp()
{
  m_pCameraSystem->RotateXUp();
}
/// @brief カメラX軸回転（下方向）
void FinderStudioViewer::CameraRotateXDown()
{
  m_pCameraSystem->RotateXDown();
}

/// @brief カメラズームイン
/// return これ以上ズームインできるか
bool FinderStudioViewer::CameraZoomIn()
{
  return m_pCameraSystem->ZoomIn();
}
/// @brief カメラズームアウト
/// return これ以上ズームアウトできるか
bool FinderStudioViewer::CameraZoomOut()
{
  return m_pCameraSystem->ZoomOut();
}

/// @brief カメラ高さ変更（高く）
/// @return これ以上高くできるか
bool FinderStudioViewer::CameraHeightUp()
{
  return m_pCameraSystem->HeightUp();
}
/// @brief カメラ高さ変更（低く）
/// @return これ以上低くできるか
bool FinderStudioViewer::CameraHeightDown()
{
  return m_pCameraSystem->HeightDown();
}

/// @brief 注視点変更（トレーナーに寄る）
/// @return これ以上寄れるか
bool FinderStudioViewer::CameraFocusToPlayer()
{
  return m_pCameraSystem->FocusToPlayer();
}
/// @brief 注視点変更（ポケモンに寄る）
/// @return これ以上寄れるか
bool FinderStudioViewer::CameraFocusToPokemon()
{
  return m_pCameraSystem->FocusToPokemon();
}

/// @brief カメラズーム割合セット
/// @param カメラズーム割合(0 ~ 1)
void FinderStudioViewer::SetCameraZoomRate(float zoomRate)
{
  m_pCameraSystem->SetZoomRate(zoomRate);
}

/// @brief カメラ高さ割合セット
/// @param カメラ高さ割合(0 ~ 1)
void FinderStudioViewer::SetCameraHeightRate(float heightRate)
{
  m_pCameraSystem->SetHeightRate(heightRate);
}

/// @brief カメラ注視点割合セット
/// @param カメラ注視点割合(0 ~ 1)
void FinderStudioViewer::SetCameraFocusRate(float focusRate)
{
  m_pCameraSystem->SetFocusRate(focusRate);
}

/// @brief 現在のカメラズーム度合い取得
/// @return ズーム度合い(0 ~ 1)
f32 FinderStudioViewer::GetCameraZoomRate() const
{
  return m_pCameraSystem->GetZoomRate();
}

/// @brief 現在のカメラフォーカス取得
/// @return 0 ~ 1 トレーナーに完全フォーカスなら０ ポケモンに完全フォーカスなら１
f32 FinderStudioViewer::GetCameraFocusRate() const
{
  return m_pCameraSystem->GetFocusRate();
}

/// @brief 現在のカメラ高さ取得
/// @return 高さ(0 ~ 1)
f32 FinderStudioViewer::GetCameraHeightRate() const
{
  return m_pCameraSystem->GetHeightRate();
}

/// @brief 現在のカメラY角度取得
/// @return Y角度（度）
f32 FinderStudioViewer::GetCameraDegreeY() const
{
  return m_pCameraSystem->GetDegY();
}

/// @brief キャプチャー開始
/// @param[out] テクスチャバッファ（連射なので６枚分）
/// @param[out] 写真パラメータ（連射なので６枚分）
/// @attention テクスチャバッファを確保して渡さないと撮影を行いません！
void FinderStudioViewer::StartCapture(gfl2::gfx::Texture** textureBuffer,PhotoParam* pPhotoParamArray)
{
  if(m_ViewerMode == VIEWER_MODE_PHOTO)
  {
    GFL_ASSERT_MSG(0,"写真再現モードで写真は撮れません！\n");
    return;
  }

  if(textureBuffer)
  {
    for(u32 i = 0;i < BURST_CAPTURE_NUM;i++)
    {
      // テクスチャを受け取る
      m_CaptureTexture[i].pTextureBuffer = textureBuffer[i];

    }
    // とりあえず1枚目を送る
    m_pRenderSystem->RequestTransferColorBuffer(&m_CaptureTexture[0]);
    m_BurstCaptureFlag = true;
    m_BurstCaptureNum = 0;
    m_BurstCaptureCnt = 0;

    if(pPhotoParamArray)
    {
      for(u32 i = 0;i < BURST_CAPTURE_NUM;i++)
      {
        m_pPhotoParamArray[i] = &pPhotoParamArray[i];
      }

      // とりあえず1枚目を保存
      GFL_ASSERT_STOP(m_pPhotoParamArray[0]);
      this->SavePhotoParam(m_pPhotoParamArray[0]);
    }
    else
    {
      GFL_ASSERT_MSG(0,"格納する写真パラメータが指定されていません\n");
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"テクスチャバッファが確保されていません\n");
  }

  
}

/// @brief キャプチャー開始
/// @param[out] カラーバッファ（連射なので６枚分）
/// @param[out] 写真パラメータ（連射なので６枚分）
/// @attention カラーバッファを確保して渡さないと撮影を行いません！
void FinderStudioViewer::StartCapture(void** colorBuffer,PhotoParam* pPhotoParamArray)
{
  if(m_ViewerMode == VIEWER_MODE_PHOTO)
  {
    GFL_ASSERT_MSG(0,"写真再現モードで写真は撮れません！\n");
    return;
  }

  if(colorBuffer)
  {
    for(u32 i = 0;i < BURST_CAPTURE_NUM;i++)
    {
      // カラーバッファを受け取る
      m_CaptureTexture[i].pColorBuffer = colorBuffer[i];
    }
    // とりあえず1枚目を送る
    m_pRenderSystem->RequestTransferColorBuffer(&m_CaptureTexture[0]);
    m_BurstCaptureFlag = true;
    m_BurstCaptureNum = 0;
    m_BurstCaptureCnt = 0;

    if(pPhotoParamArray)
    {
      for(u32 i = 0;i < BURST_CAPTURE_NUM;i++)
      {
        m_pPhotoParamArray[i] = &pPhotoParamArray[i];
      }

      // とりあえず1枚目を保存
      GFL_ASSERT_STOP(m_pPhotoParamArray[0]);
      this->SavePhotoParam(m_pPhotoParamArray[0]);
    }
    else
    {
      GFL_ASSERT_MSG(0,"格納する写真パラメータが指定されていません\n");
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"カラーバッファが確保されていません\n");
  }
}

/// @brief キャプチャーが終わったか
/// @return 終わったかどうか
bool FinderStudioViewer::IsFinishCapture() const
{
  if(m_BurstCaptureFlag)
  {
    return false;
  }
  else
  {
    return true;
  }
}

/// @brief ポケモン位置取得(2D)
/// @return 3D座標から算出した2D（下画面）でのポケモン座標
gfl2::math::Vector2 FinderStudioViewer::GetPokePositionScreen() const
{
  return m_pPokemon->GetPokePositionScreen();
}

/// @brief ポケモン位置取得(3D)
/// @return 3Dでのポケモン座標
gfl2::math::Vector3 FinderStudioViewer::GetPokePosition() const
{
  return m_pPokemon->GetPokePosition();
}

/// @brief ポケモン位置変更
/// @param lowerPosition 2D（下画面）でのポケモン座標
/// @param isLift 持ち上げるかどうか falseで下ろす
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioViewer::SetPokePosition(const gfl2::math::Vector2& lowerPosition,bool isLift)
{
  return m_pPokemon->SetPokePosition(lowerPosition,isLift);
}

/// @brief ポケモン角度変更（左回り）
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioViewer::PokemonRotateLeft()
{
  return m_pPokemon->RotateLeft();
}
/// @brief ポケモン角度変更（右回り）
/// @return 変更した結果トレーナーとぶつかったかどうか
bool FinderStudioViewer::PokemonRotateRight()
{
  return m_pPokemon->RotateRight();
}

/// @brief ポケモンの角度をセット
/// @param degY ポケモンのY軸角度(0 <= degY < 360)
bool FinderStudioViewer::SetPokeRotate(float degY)
{
  return m_pPokemon->SetRotate(degY);
}

/// @brief ポケモンの角度を取得
/// @return 角度
f32 FinderStudioViewer::GetPokemonDegY() const
{
  return m_pPokemon->GetDegY();
}

/// @brief ポケモンの当たり判定を無視した角度を取得
/// @return 当たり判定を無視した角度
f32 FinderStudioViewer::GetPokemonDegYIgnoreCollision() const
{
  return m_pPokemon->GetDegYIgnoreCollision();
}

/// @brief トレーナー角度変更（左回り）
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioViewer::TrainerRotateLeft()
{
  return m_pChara->RotateLeft();
}
/// @brief トレーナー角度変更（右回り）
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioViewer::TrainerRotateRight()
{
  return m_pChara->RotateRight();
}

/// @brief トレーナーの角度をセット
/// @param degY トレーナーのY軸角度(0 <= degY < 360)
/// @return 変更した結果ポケモンとぶつかったかどうか
bool FinderStudioViewer::SetTrainerRotate(float degY)
{
  return m_pChara->SetRotate(degY);
}

/// @brief トレーナーの角度を取得
/// @return 角度
f32 FinderStudioViewer::GetTrainerDegY() const
{
  return m_pChara->GetDegY();
}

/// @brief トレーナーの当たり判定を無視した角度を取得
/// @return 当たり判定を無視した角度
f32 FinderStudioViewer::GetTrainerDegYIgnoreCollision() const
{
  return m_pChara->GetDegYIgnoreCollision();
}

/// @brief ポケモンの待機AABB取得
/// @return 待機モーションのAABB
const gfl2::math::AABB FinderStudioViewer::GetPokemonAABB() const
{
  return m_pPokemon->GetAABBIgnoreCollision();
}

/// @brief トレーナーのAABB取得
/// @return 現在のモーションのAABB
const gfl2::math::AABB FinderStudioViewer::GetTrainerAABB() const
{
  return m_pChara->GetAABBIgnoreCollision();
}

/// @brief ポケモンがトレーナーにぶつかっているか判定
/// @return ぶつかっているかどうか
/// @note UI上の角度で判定（ビューワ上でぶつかることはありえないため）
bool FinderStudioViewer::IsHitPokemonToTrainer() const
{
  return m_pPokemon->IsHitPokemonToTrainerUI();
}

/// @brief 撮影する準備ができているか
/// @return false このフレームで撮影するとまずい
bool FinderStudioViewer::IsPrepareCapture() const
{
  if(m_pBackGround->IsNowTimeAnimeInOut())
  {
    return false;
  }

  return true;
}

#if PM_DEBUG

// エージング用

/// @brief エージングセットアップ
/// @attention エージング開始前にこの関数を呼んでください
void FinderStudioViewer::DebugSetupAsing()
{
  m_pPokemon->DebugSetEnableAsing(true);
  this->SetPokeEnableAutoMotion(false);
}
/// @brief 再生可能モーションリスト取得（バトル）
/// @return 再生可能モーションリストの先頭ポインタ
/// @note 最大数はPokeTool::MODEL_ANIME_BATTLE_NUM(18)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
const PokeTool::MODEL_ANIME* FinderStudioViewer::DebugGetPokeCanPlayBattleMotionList() const { return m_pPokemon->DebugGetCanPlayBattleMotionList(); }
/// @brief 再生可能モーションリスト取得（かわいがり）
/// @return 再生可能モーションリストの先頭ポインタ
/// @note 最大数はPokeTool::MODEL_ANIME_KAWAIGARI_NUM(26)、空欄にはPokeTool::MODEL_ANIME_ERROR(-1)が入っています
const PokeTool::MODEL_ANIME* FinderStudioViewer::DebugGetPokeCanPlayKawaigariMotionList() const { return m_pPokemon->DebugGetCanPlayKawaigariMotionList(); }
/// @brief ポケモンモーション指定再生
/// @return true 再生成功 false 再生失敗
bool FinderStudioViewer::DebugPlayPokeBattleMotion(PokeTool::MODEL_ANIME motionNo){ return m_pPokemon->DebugPlayBattleMotion(motionNo); }
bool FinderStudioViewer::DebugPlayPokeKawaigariMotion(PokeTool::MODEL_ANIME motionNo){ return m_pPokemon->DebugPlayKawaigariMotion(motionNo); }
/// @brief 眠りモーションがあるか判定
bool FinderStudioViewer::DebugIsPokeCanSleep() const { return m_pPokemon->DebugIsCanSleep(); }
/// @brief 眠る
/// @return true 再生成功 false 再生失敗
bool FinderStudioViewer::DebugSleepPoke(){ return m_pPokemon->DebugSleep(); }
/// @brief 起きれるか判定
bool FinderStudioViewer::DebugIsPokeCanWake() const { return m_pPokemon->DebugIsCanWake(); }
/// @brief 起きる
/// @return true 再生成功 false 再生失敗
bool FinderStudioViewer::DebugWakePoke(){ return m_pPokemon->DebugWake(); }

#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)
