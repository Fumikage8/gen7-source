//======================================================================
/**
 * @file FinderStudioBackGround.cpp
 * @date 2016/09/21
 * @author harada_takumi
 * @brief ファインダースタジオ背景処理
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "FinderStudioBackGround.h"

// gfl2
#include <fs/source/Win32/gfl2_FsWin32Device.h>

// System
#include "System/include/Skybox/Skybox.h"

// btl
#include "Battle/BackGround/include/BgSystem.h"
#include "Battle/BackGround/source/BgObject.h"

// FinderStudioStatic
#include "App/FinderStudioStatic/include/FinderStudioViewerDefine.h"
#include "App/FinderStudioStatic/include/FinderStudioBGDataAccessor.h"

// FinderStudioViewer
#include "FinderStudioRenderPipeLine.h"
#include "FinderStudioCameraSystem.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FinderStudio)
GFL_NAMESPACE_BEGIN(Viewer)

// 時間テーブル
const u32 FinderStudioBackGround::TIME_TABLE[][3] = {
  {12,0,0},
  {17,0,0},
  {21,0,0}
};

// ヒープサイズ
const u32 FinderStudioBackGround::BG_TEMP_HEAP_SIZE = 0x100000;

//コンストラクタ
FinderStudioBackGround::FinderStudioBackGround(const Desc& desc):
  m_pPhotoParam(NULL)
  ,m_IsUsePhotoParam(false)
  ,m_pBgSystem(NULL)
  ,m_pViewMtx(NULL)
  ,m_pFarProjMtx(NULL)
  ,m_pSkyProjMtx(NULL)
  ,m_InitSeq(0)
  ,m_MainSeq(0)
  ,m_EndSeq(0)
  ,m_BgSetIndex(0)
  ,m_GroundType(1)
  ,m_NearType(1)
  ,m_FarType(1)
  ,m_TimeZone(TIME_ZONE_NOON)
  ,m_Hour(12)
  ,m_Minutes(0)
  ,m_Second(0)
  ,m_DegY(0)
  ,m_IsUpdateAnimation(true)
  ,m_LoadEndFrameFlag(false)
#if PM_DEBUG
  ,m_DebugIsVisibleSkyBox(true)
  ,m_DebugIsVisibleGround(true)
  ,m_DebugIsVisibleBackNear(true)
  ,m_DebugIsVisibleBackFar(true)
  ,m_DebugOutputBgDataFlag(false)
  ,m_DebugOutputNowObjType(0)
  ,m_DebugOutputNowAnmType(0)
#endif
{
  GFL_ASSERT_STOP_MSG(desc.pHeap,"ヒープを指定してください\n");
  GFL_ASSERT_STOP_MSG(desc.pWorkHeap,"一時ヒープを指定してください\n");
  GFL_ASSERT_STOP_MSG(desc.pFinderStudioRenderPipeLine,"パイプラインを指定してください\n");

  m_pHeap = desc.pHeap;
  m_pWorkHeap = desc.pWorkHeap;
  m_pFinderStudioRenderPipeLine = desc.pFinderStudioRenderPipeLine;
  m_pPhotoParam = desc.pPhotoParam;
  m_IsUsePhotoParam = desc.isUsePhotoParam;

  //// 一時ヒープ作成
  //m_pWorkHeap = GFL_CREATE_LOCAL_HEAP_NAME(m_pHeap,BG_TEMP_HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioBgTempHeap");

  // 背景データをパラメータから取得
  if(!m_IsUsePhotoParam)
  {
    // 再現パラメータを使わない場合
    m_BgSetIndex = 0;
    m_TimeZone = TIME_ZONE_NOON;
  }
  else
  {
    // 再現パラメータを使う場合
    GFL_ASSERT_STOP(m_pPhotoParam);
    ApplyPhotoParamBgSetIndexTimeZone(*m_pPhotoParam);
  }

  // 各Noをセット
  getTypeFromBgSetIndex(m_BgSetIndex,&m_GroundType,&m_NearType,&m_FarType);

  // 時間をセット
  getTimeFromTimeZone(m_TimeZone,&m_Hour,&m_Minutes,&m_Second);
}
//デストラクタ
FinderStudioBackGround::~FinderStudioBackGround()
{
  // 一時ヒープ削除
  //GFL_DELETE_HEAP(m_pWorkHeap);
}

// 更新用
enum{
  SEQ_MAIN_INITIALIZE,
  SEQ_MAIN_INITIALIZE_WAIT,
  SEQ_MAIN_LOAD,
  SEQ_MAIN_LOAD_WAIT,
  SEQ_MAIN_MAIN,
  SEQ_MAIN_DELETE,
  SEQ_MAIN_DELETE_WAIT,
  SEQ_MAIN_TERMINATE_SYSTEM,
  SEQ_MAIN_TERMINATE_SYSTEM_WAIT
};

// 初期化処理
bool FinderStudioBackGround::InitFunc()
{
  enum{
    SEQ_INIT_CREATE_BG_SYSTEM,
    SEQ_INIT_CREATE_BG_SYSTEM_WAIT,
    SEQ_INIT_LOAD_BG_WAIT,
    SEQ_INIT_END
  };

  switch(m_InitSeq)
  {
  case SEQ_INIT_CREATE_BG_SYSTEM:
  {
    // 背景システム作成
    m_pBgSystem = GFL_NEW(m_pHeap) btl::BgSystem();
    btl::BgSystem::INIT_PARAM initParam;
    initParam.heap = m_pHeap;
    initParam.workHeap = m_pWorkHeap->GetLowerHandle();
    initParam.fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    m_pBgSystem->Initialize(initParam,m_Hour,m_Minutes,m_Second);
  }

  m_InitSeq++;
  // fall through

  case SEQ_INIT_CREATE_BG_SYSTEM_WAIT:
    if(m_pBgSystem->IsFinishInitialize())
    {
      // パイプラインに登録
      m_pFinderStudioRenderPipeLine->SetBgRenderPath(m_pBgSystem);

      // ロード開始
      btl::BgSystem::SETUP_PARAM setupParam;
      setupParam.groundType = m_GroundType;
      setupParam.nearType = m_NearType;
      setupParam.farType = m_FarType;
      setupParam.useSkyBox = true;
      m_pBgSystem->StartLoad(setupParam);

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_LOAD_BG_WAIT:
    m_pBgSystem->LoadUpdate();
    if(m_pBgSystem->IsFinishLoad())
    {
      m_pBgSystem->Create();
      m_pBgSystem->SetVisible(true);

      m_InitSeq++;
    }
    break;

  case SEQ_INIT_END:
    // 上画面前提
    u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]);
    u32	displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT]);
    float nearVal = 10.0f;
    float farVal = 2000.0f;

    if(displayHeight == 0)
    {
      GFL_ASSERT(0);
      displayHeight = 400;
    }

    // 射影変換行列(パースペクティブ(透視法)射影)
    // カメラシステムがないときのために用意
    m_ProvProjMtx = gfl2::math::Matrix44::GetPerspective(
      0.35f,//gfl2::math::ConvDegToRad(35.0f),		// 視野角30°
      static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
      nearVal,							// 前方投影面までの距離
      farVal,true);							// 後方投影面までの距離
    

    // ビュー行列
    // カメラシステムがないときのために用意
    gfl2::math::Vector3 eyePosition(0,250,-500);
    gfl2::math::Vector3 focusPosition(0,0,0);
    gfl2::math::Vector3 upDirection(0,1,0);
    m_ProvViewMtx = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

    // 再現パラメータを使うなら残りのパラメータも適応
    if(m_IsUsePhotoParam)
    {
      if(m_pPhotoParam)
      {
        this->ApplyPhotoParamBgOther(*m_pPhotoParam);
      }
      else
      {
        GFL_ASSERT(0);
      }
    }

    // メインの更新から開始
    m_MainSeq = SEQ_MAIN_MAIN;

    //m_pHeap->Dump();

    return true;
  }

  return false;
}
// 終了処理
bool FinderStudioBackGround::Terminate()
{
  enum{
    SEQ_END_DELETE_BG_SYSTEM,
    SEQ_END_DELETE_BG_SYSTEM_WAIT,
    SEQ_END_TERMINATE_BG_SYSTEM,
    SEQ_END_TERMINATE_BG_SYSTEM_WAIT,
    SEQ_END_END
  };

  switch(m_EndSeq)
  {
  case SEQ_END_DELETE_BG_SYSTEM:
    // 背景システム削除
    m_pBgSystem->Delete();

    m_EndSeq++;
    // fall through

  case SEQ_END_DELETE_BG_SYSTEM_WAIT:
    if(m_pBgSystem->IsFinishDelete())
    {
      m_EndSeq++;
    }
    break;

  case SEQ_END_TERMINATE_BG_SYSTEM:
    // 背景システム削除
    // 初期化済みなら破棄
    if(m_pBgSystem->IsFinishInitialize())
    {
      m_pBgSystem->Terminate();
    }
  
    m_EndSeq++;
    // fall through
  
  case SEQ_END_TERMINATE_BG_SYSTEM_WAIT:
    if(m_pBgSystem->IsFinishTerminate())
    {
      GFL_SAFE_DELETE(m_pBgSystem);
      m_EndSeq++;
    }
    break;
  case SEQ_END_END:
    return true;
  }

  return false;
}

// 更新
void FinderStudioBackGround::Update()
{
  switch(m_MainSeq)
  {
  case SEQ_MAIN_INITIALIZE:
  {
    // 再初期化
    btl::BgSystem::INIT_PARAM initParam;
    initParam.heap = m_pHeap;
    initParam.workHeap = m_pWorkHeap->GetLowerHandle();
    initParam.fileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
    m_pBgSystem->Initialize(initParam,m_Hour,m_Minutes,m_Second);

    m_MainSeq++;
  }
  // fall throught

  case SEQ_MAIN_INITIALIZE_WAIT:
    if(m_pBgSystem->IsFinishInitialize())
    {
      // パイプラインに登録
      m_pFinderStudioRenderPipeLine->SetBgRenderPath(m_pBgSystem);

      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_LOAD:
    {
      // ロード開始
      btl::BgSystem::SETUP_PARAM setupParam;
#if PM_DEBUG
      setupParam.groundType = m_DebugIsVisibleGround ? m_GroundType : 0;
      setupParam.nearType = m_DebugIsVisibleBackNear ? m_NearType : 0;
      setupParam.farType = m_DebugIsVisibleBackFar ? m_FarType : 0;
#else
      setupParam.groundType = m_GroundType;
      setupParam.nearType = m_NearType;
      setupParam.farType = m_FarType;
#endif
      setupParam.useSkyBox = true;
      m_pBgSystem->StartLoad(setupParam);
    }

    m_MainSeq++;
    // fall through

  case SEQ_MAIN_LOAD_WAIT:
    m_pBgSystem->LoadUpdate();
    if( m_pBgSystem->IsFinishLoad() )
    {
      m_pBgSystem->Create();
      m_pBgSystem->SetVisible(true);

      // 読み込み終了フレームオン
      m_LoadEndFrameFlag = true;

      //m_pHeap->Dump();
      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_MAIN:
  {
    // 読み込み終了フレームオフ
    m_LoadEndFrameFlag = false;

    // アニメーション更新前の値を保存
    this->savePrevParam();

    // 回転反映
    m_pBgSystem->RotateY_Deg(m_DegY);

    // ビュー行列・プロジェクション行列セット
    GFL_ASSERT(m_pViewMtx);
    GFL_ASSERT(m_pFarProjMtx);
    GFL_ASSERT(m_pSkyProjMtx);
    gfl2::math::Matrix34 viewMtx = m_pViewMtx ? *m_pViewMtx : m_ProvViewMtx;
    gfl2::math::Matrix44 farProjMtx = m_pFarProjMtx ? *m_pFarProjMtx : m_ProvProjMtx;
    gfl2::math::Matrix44 skyProjMtx = m_pSkyProjMtx ? *m_pSkyProjMtx : m_ProvProjMtx;
    m_pBgSystem->SetFarMtx(farProjMtx,viewMtx);
    m_pBgSystem->SetSkyMtx(skyProjMtx,viewMtx);
    // 背景更新
    m_pBgSystem->Update(m_Hour,m_Minutes,m_Second);

    // @fix GFMMCat[549] 時間アニメのインアウト時に撮影されるのを防ぐために最終フレームまで飛ばす処理を入れる
    // 時間インアウトなら最終フレームにする
    int anmType = m_pBgSystem->GetNowTimeAnimation();
    switch(anmType)
    {
    case btl::BgSystem::ANM_MORNING_IN:
    case btl::BgSystem::ANM_MORNING_OUT:
    case btl::BgSystem::ANM_NOON_IN:
    case btl::BgSystem::ANM_NOON_OUT:
    case btl::BgSystem::ANM_EVENING_IN:
    case btl::BgSystem::ANM_EVENING_OUT:
    case btl::BgSystem::ANM_NIGHT_IN:
    case btl::BgSystem::ANM_NIGHT_OUT:
      for(u32 i = 0;i < btl::BgSystem::OBJ_MAX;i++)
      {
        int lastFrame = m_pBgSystem->GetLastAnimationFrame(static_cast<btl::BgSystem::BG_OBJ_TYPE>(i),anmType);
        m_pBgSystem->SetAnimationFrame(static_cast<btl::BgSystem::BG_OBJ_TYPE>(i),anmType,lastFrame);
      }
      break;
    default:
      // 何もしない
      break;
    }

    // ファイルに出力するデバッグ
#if PM_DEBUG
    if(m_DebugOutputBgDataFlag)
    {
      for(m_DebugOutputNowObjType = 0;m_DebugOutputNowObjType < 4;m_DebugOutputNowObjType++)
      {
        for(m_DebugOutputNowAnmType = 0;m_DebugOutputNowAnmType < 13;m_DebugOutputNowAnmType++)
        {
          // ファイルに出力
          outPutLog(m_DebugOutputNowObjType,m_DebugOutputNowAnmType);
        }
      }

      // 次の背景へ
      if(m_TimeZone == TIME_ZONE_NOON) m_TimeZone = TIME_ZONE_EVENING;
      else if(m_TimeZone == TIME_ZONE_EVENING) m_TimeZone = TIME_ZONE_NIGHT;
      else
      {
        m_TimeZone = TIME_ZONE_NOON;
        m_BgSetIndex++;
      }

      // 全BgSet分完了
      if(m_BgSetIndex >= BG_ID_NUM)
      {
        this->StopOutputBgData();
        break;
      }

      this->StartReload(m_BgSetIndex);
    }
#endif

  }
  break;

  case SEQ_MAIN_DELETE:
    // ここにきたときは外部から再ロードが行われたときである
    // 解放するため非表示に
    m_pBgSystem->SetVisible(false);
    m_pBgSystem->Delete();

    m_MainSeq++;
    // fall through

  case SEQ_MAIN_DELETE_WAIT:
    if(m_pBgSystem->IsFinishDelete())
    {
      m_MainSeq++;
    }
    break;

  case SEQ_MAIN_TERMINATE_SYSTEM:
    m_pBgSystem->Terminate();
    // パイプラインから削除
    m_pFinderStudioRenderPipeLine->ResetBgRenderpath();
    m_MainSeq++;
    // fall through

  case SEQ_MAIN_TERMINATE_SYSTEM_WAIT:
    if(m_pBgSystem->IsFinishTerminate())
    {
      m_MainSeq = SEQ_MAIN_INITIALIZE;
    }
    break;
  }
}

/// @brief 写真再現パラメータ適用（背景セットインデックス・時間帯）
/// @attention この関数を読んだ後モデルの再読み込みを行う必要がある
void FinderStudioBackGround::ApplyPhotoParamBgSetIndexTimeZone(const PhotoParam& photoParam)
{
  m_BgSetIndex = photoParam.bgSetIndex;
  m_TimeZone = static_cast<TimeZone>(photoParam.timeZone);
}
/// @brief 写真再現パラメータ適用（その他データ）
void FinderStudioBackGround::ApplyPhotoParamBgOther(const PhotoParam& photoParam)
{
  // 背景ループアニメーション
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_GROUND,btl::BgSystem::ANM_LOOP,photoParam.bgGroundLoopAnmFrame);  // 地面
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_NEAR,btl::BgSystem::ANM_LOOP,photoParam.bgNearLoopAnmFrame);  // 近景
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_FAR,btl::BgSystem::ANM_LOOP,photoParam.bgFarLoopAnmFrame);  // 遠景
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_BUILD,btl::BgSystem::ANM_LOOP,photoParam.bgBuildLoopAnmFrame);  // 頂点アニメ

  // 背景時間アニメーション
  int anmType = btl::BgSystem::ANM_MORNING_IN + photoParam.bgTimeAnmType;
  m_pBgSystem->PlayAnime(anmType);
  // 時間アニメのフレーム数は共通
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_GROUND,anmType,photoParam.bgTimeAnmFrame);
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_NEAR,anmType,photoParam.bgTimeAnmFrame);
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_FAR,anmType,photoParam.bgTimeAnmFrame);
  m_pBgSystem->SetAnimationFrame(btl::BgSystem::OBJ_BUILD,anmType,photoParam.bgTimeAnmFrame);

  // 天球アニメーション
  // 雲位置適用
  // UVアニメなのでスロットは1、ビットはマテリアル
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_CLOUD)->SetAnimationFrame(photoParam.skyCloudAnmFrame,1,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  // 太陽のギラギラ適用
  // UVアニメなのでスロットは0、ビットはマテリアル
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_SUN)->SetAnimationFrame(photoParam.skySunAnmFrame,0,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  // 月の満ち欠け適用
  // UVアニメなのでスロットは1、ビットはマテリアル
  // パターンがそのままフレームになっている
  /// @note 月はUpdateAnimationを呼んでくれない
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_MOON)->SetAnimationFrame(photoParam.skyMoonPattern,1,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_MOON)->UpdateAnimation();
}

/// @brief アニメーション更新のオンオフを切り替える
/// @param true オン false オフ
/// @note 写真再現後の更新のため
void FinderStudioBackGround::SetIsAnimationUpdate(bool isUpdate)
{
  if(m_IsUpdateAnimation != isUpdate)
  {
    m_IsUpdateAnimation = isUpdate;

    // 天球の雲、太陽のアニメーションを切り替え
    m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_CLOUD)->SetAnimationUpdateFrameEnable(m_IsUpdateAnimation);
    m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_SUN)->SetAnimationUpdateFrameEnable(m_IsUpdateAnimation);

    // 戦闘背景のアニメーション切り替え
    if(m_IsUpdateAnimation)
    {
      m_pBgSystem->SetAnimationUpdateFrameEnable(true);
    }
    else
    {
      m_pBgSystem->SetAnimationUpdateFrameEnable(false);
    }
  }
}

/// @brief 写真再現パラメータの出力が可能かどうか
/// @return true 可能 false 不可能
bool FinderStudioBackGround::IsCanGetPhotoParam()
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
void FinderStudioBackGround::GetPhotoParamBg(PhotoParam* pPhotoParam) const
{
  if(m_MainSeq != SEQ_MAIN_MAIN) return;

  GFL_ASSERT(pPhotoParam);

  pPhotoParam->bgSetIndex = m_BgSetIndex;
  pPhotoParam->timeZone = m_TimeZone;

  {
    // 背景ループアニメーション
    int frame = m_PrevParam.groundLoopAnmFrame;
    pPhotoParam->bgGroundLoopAnmFrame = frame == -1 ? 0 : frame;
    frame = m_PrevParam.nearLoopAnmFrame;
    pPhotoParam->bgNearLoopAnmFrame = frame == -1 ? 0 : frame;
    frame = m_PrevParam.farLoopAnmFrame;
    pPhotoParam->bgFarLoopAnmFrame = frame == -1 ? 0 : frame;
    frame = m_PrevParam.buildLoopAnmFrame;
    pPhotoParam->bgBuildLoopAnmFrame = frame == -1 ? 0 : frame;
  }

  {
    // 背景時間アニメーション
    pPhotoParam->bgTimeAnmType = m_pBgSystem->GetNowTimeAnimation() - btl::BgSystem::ANM_MORNING_IN;
    // 背景時間アニメーションフレーム
    pPhotoParam->bgTimeAnmFrame = m_PrevParam.timeAnmFrame;
  }

  // 天球アニメーション
  // 雲位置取得
  pPhotoParam->skyCloudAnmFrame = m_PrevParam.skyCloudAnmFrame;
  // 太陽のギラギラ取得
  pPhotoParam->skySunAnmFrame = m_PrevParam.skySunAnmFrame;
  
  // 月の満ち欠け適用
  // UVアニメなのでスロットは1、ビットはマテリアル
  // パターンがそのままフレームになっている
  pPhotoParam->skyMoonPattern = m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_MOON)->GetAnimationFrame(1,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
}

/// @brief 指定したスロットの１フレーム前のアニメーションフレームを取得
f32 FinderStudioBackGround::getPrevAnimationFrame(const btl::BgSystem* pBgSystem,btl::BgSystem::BG_OBJ_TYPE objType,int anmType) const
{
  GFL_ASSERT_STOP(pBgSystem);

  f32 stepFrame = m_pBgSystem->GetAnimationStepFrame(objType,anmType);
  int frame = m_pBgSystem->GetAnimationFrame(objType,anmType) - stepFrame;
  if(frame < 0) frame = m_pBgSystem->GetLastAnimationFrame(objType,anmType);

  return frame;
}

/// @brief 背景セット再ロード（インデックスは内部のデータを使用）
void FinderStudioBackGround::StartReload()
{
  this->StartReload(m_BgSetIndex);
}
/// @brief 背景セット再ロード
/// @param bgSetIndex 背景セットインデックス
void FinderStudioBackGround::StartReload(u32 bgSetIndex)
{
  m_BgSetIndex = bgSetIndex;

  this->getTypeFromBgSetIndex(m_BgSetIndex,&m_GroundType,&m_NearType,&m_FarType);

  this->StartReload(m_GroundType,m_NearType,m_FarType);
}
// 再ロード：個別指定版
void FinderStudioBackGround::StartReload(int groundType,int nearType,int farType)
{
  // メイン処理のときしかリロード開始不可
  if(m_MainSeq != SEQ_MAIN_MAIN)
  {
    GFL_ASSERT_STOP_MSG(0,"まだ読み込みが完了していません！\n");
    return;
  }

  m_GroundType = groundType;
  m_NearType = nearType;
  m_FarType = farType;

  // 時間をセット
  getTimeFromTimeZone(m_TimeZone,&m_Hour,&m_Minutes,&m_Second);

  // 再ロードするために削除へ
  m_MainSeq = SEQ_MAIN_DELETE;
}

/// @brief 再ロードが終わったかどうか
/// @return 終わったかどうか
bool FinderStudioBackGround::IsFinishReloadModel() const
{
  if(m_MainSeq == SEQ_MAIN_MAIN)
  {
    return true;
  }

  return false;
}

/// @brief 戦闘背景の時間帯指定
/// @param timeZone 時間帯
void FinderStudioBackGround::SetTimeZone(TimeZone timeZone)
{
  m_TimeZone = timeZone;

  u32 hour = 0;
  u32 minute = 0;
  u32 second = 0;
  getTimeFromTimeZone(m_TimeZone,&hour,&minute,&second);

  this->SetTime(hour,minute,second);
}

// 時間指定：個別版
void FinderStudioBackGround::SetTime(int hour,int minute,int second)
{
  m_Hour = hour;
  m_Minutes = minute;
  m_Second = second;
}

/// @brief BGセットインデックスから各要素を取得
/// @param bgSetIndex 背景セットインデックス
/// @param[out] groundType 地面No
/// @param[out] nearType 近景No
/// @param[out] farType 遠景No
void FinderStudioBackGround::getTypeFromBgSetIndex(u32 bgSetIndex,u32* groundType,u32* nearType,u32* farType)
{
  if(groundType)
  {
    *groundType = BGDataAccessor::GetViewerData(bgSetIndex).groundID;
  }
  if(nearType)
  {
    *nearType = BGDataAccessor::GetViewerData(bgSetIndex).nearID;
  }
  if(farType)
  {
    *farType = BGDataAccessor::GetViewerData(bgSetIndex).farID;
  }
}

/// @brief 時間帯から時・分・秒を取得
/// @param timeZone 時間帯
/// @param[out] hour 時
/// @param[out] mitute 分
/// @param[out] secound 秒
void FinderStudioBackGround::getTimeFromTimeZone(TimeZone timeZone,u32* hour,u32* mitute,u32* secound)
{
  if(timeZone >= 0 && timeZone < GFL_NELEMS(TIME_TABLE))
  {
    if(hour)
    {
      *hour = TIME_TABLE[timeZone][0];
    }
    if(mitute)
    {
      *mitute = TIME_TABLE[timeZone][1];
    }
    if(secound)
    {
      *secound = TIME_TABLE[timeZone][2];
    }
  }
  else
  {
    GFL_ASSERT_MSG(0,"TIME_TABLE Over!\n");
    if(hour)
    {
      *hour = 0;
    }
    if(mitute)
    {
      *mitute = 0;
    }
    if(secound)
    {
      *secound = 0;
    }
  }
}

/// @brief アニメーション更新前のパラメータ保存
void FinderStudioBackGround::savePrevParam()
{
  // 背景ループアニメ
  m_PrevParam.groundLoopAnmFrame = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_GROUND,btl::BgSystem::ANM_LOOP);
  m_PrevParam.nearLoopAnmFrame = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_NEAR,btl::BgSystem::ANM_LOOP);
  m_PrevParam.farLoopAnmFrame = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_FAR,btl::BgSystem::ANM_LOOP);
  m_PrevParam.buildLoopAnmFrame = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_BUILD,btl::BgSystem::ANM_LOOP);

  // 背景時間アニメ
  // @fix GFMMCat[549] 時間アニメのインアウトのときはフレームを保存しない
  //                   インアウトになるのは時間帯変更時の一瞬だけなのでその間に撮影されることはない
  if(this->IsNowTimeAnimeInOut())
  {
    // 0を入れておく
    m_PrevParam.timeAnmFrame = 0;
  }
  else
  {
    // このフレーム数は総フレーム数が全て同じであることを井部さんに依頼済みなので一番大きいフレームを取りに行く
    // それが共通のフレームになる
    int maxFrame = 0;
    int frame[4];
    frame[0] = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_GROUND,m_pBgSystem->GetNowTimeAnimation());
    if(maxFrame < frame[0]) maxFrame = frame[0];
    frame[1] = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_NEAR,m_pBgSystem->GetNowTimeAnimation());
    if(maxFrame < frame[1]) maxFrame = frame[1];
    frame[2] = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_FAR,m_pBgSystem->GetNowTimeAnimation());
    if(maxFrame < frame[2]) maxFrame = frame[2];
    frame[3] = m_pBgSystem->GetAnimationFrame(btl::BgSystem::OBJ_BUILD,m_pBgSystem->GetNowTimeAnimation());
    if(maxFrame < frame[3]) maxFrame = frame[3];

    // 全フレームが一致しているか一応確認
    for(u32 i = 0;i < 4;i++)
    {
      if(frame[i] == -1) continue;
      if(frame[i] != maxFrame) GFL_ASSERT_MSG(0,"時間アニメーションのフレームが一致していません！\n");
    }

    m_PrevParam.timeAnmFrame = maxFrame;
  }
  
  // UVアニメなのでスロットは1、ビットはマテリアル
  m_PrevParam.skyCloudAnmFrame = m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_CLOUD)->GetAnimationFrame(1,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
  // UVアニメなのでスロットは0、ビットはマテリアル
  m_PrevParam.skySunAnmFrame = m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_SUN)->GetAnimationFrame(0,poke_3d::model::BaseModel::ANIMATION_BIT_MATERIAL);
}

/// @brief 特定の背景の回転処理
/// @param pokemonPos ポケモンの座標
void FinderStudioBackGround::RotateBackGround(const f32 cameraLength,const gfl2::math::Vector3 pokemonPos)
{
  // 特定の背景のみ
  if(this->IsRotateBackGround())
  {
    // 真ん中にしたい対象までの距離を定義
    f32 bgLength = 0;
    if(m_BgSetIndex == BG_ID_61)
    {
      // ハートまでの距離
      bgLength = 1137.0f;
    }
    else if(m_BgSetIndex == BG_ID_65)
    {
      // 旗までの距離
      bgLength = 1168.0f;
    }
    else GFL_ASSERT_STOP(0);

    // カメラが接地していると仮定しての座標
    gfl2::math::Vector3 cameraPos(0,0,-cameraLength);
    // カメラ→トレーナーとカメラ→真ん中の角度を求める
    f32 deg;
    {
      // カメラからトレーナーへのベクトル
      gfl2::math::Vector3 vCamToTrainer = (gfl2::math::Vector3(0,0,0) - cameraPos).Normalize();
      // トレーナーとポケモンの間の座標
      gfl2::math::Vector3 harfTrainerToPoke = pokemonPos / 2.0f;
      // カメラからトレーナーとポケモンの間の座標へのベクトル
      gfl2::math::Vector3 vCamToPokemon = (harfTrainerToPoke - cameraPos).Normalize();

      f32 dot = vCamToTrainer.Dot(vCamToPokemon);
      deg = gfl2::math::AcosDeg(dot);
      gfl2::math::Vector3 cross = vCamToTrainer.Cross(vCamToPokemon);
      if(cross.y < 0) deg = -deg;
    }

    // カメラからハートまでのベクトル
    gfl2::math::Vector3 vCameraToHeart = gfl2::math::Vector3(0,0,bgLength + cameraLength);
    // さっき求めた角度で回転させる
    FinderStudioCameraSystem::RotateYVec(&vCameraToHeart,deg);
    // カメラの座標に回転したベクトルを足す
    gfl2::math::Vector3 toHeartPos = cameraPos + vCameraToHeart;

    // さっき求めた座標との角度を求める
    gfl2::math::Vector3 vToHeart = toHeartPos.Normalize();
    f32 dot = gfl2::math::Vector3(0,0,1).Dot(vToHeart);
    f32 resultDeg = gfl2::math::AcosDeg(dot);
    gfl2::math::Vector3 cross = gfl2::math::Vector3(0,0,1).Cross(vToHeart);
    if(cross.y < 0) resultDeg = -resultDeg;

    // その角度で背景を回転
    m_DegY = resultDeg;
  }
  else
  {
    // その他の背景なら元に戻す
    this->ResetRotateBackGround();
  }
}

/// @brief 背景の回転リセット
void FinderStudioBackGround::ResetRotateBackGround()
{
  m_DegY = 0;
}

/// @brief 現在の背景が回転させる背景かどうか
bool FinderStudioBackGround::IsRotateBackGround()
{
  // おめでとう背景 / ハート大好きのときは回転させる
  if(m_BgSetIndex == BG_ID_61 ||
    m_BgSetIndex == BG_ID_65)
  {
    return true;
  }
  else
  {
    return false;
  }
}

/// @breif 現在の時間アニメがインアウトかどうか取得
bool FinderStudioBackGround::IsNowTimeAnimeInOut() const
{
  int anmType = m_pBgSystem->GetNowTimeAnimation();
  switch(anmType)
  {
  case btl::BgSystem::ANM_MORNING_IN:
  case btl::BgSystem::ANM_MORNING_OUT:
  case btl::BgSystem::ANM_NOON_IN:
  case btl::BgSystem::ANM_NOON_OUT:
  case btl::BgSystem::ANM_EVENING_IN:
  case btl::BgSystem::ANM_EVENING_OUT:
  case btl::BgSystem::ANM_NIGHT_IN:
  case btl::BgSystem::ANM_NIGHT_OUT:
    return true;

  default:
    return false;
  }
}

#if PM_DEBUG
/// @brief 撮影する
/// @note Mainから呼ばれる処理
void FinderStudioBackGround::Shot()
{
  m_pBgSystem->StopAnime(0);
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_CLOUD)->GetAnimationController()->SetUpdateFrameEnable(false);

  //m_pBgSystem->mObject[btl::BgSystem::OBJ_GROUND]->SetAnimationFrame(btl::BgSystem::ANM_MORNING_IN,0);
}

/// @brief 撮影状態から解除する
/// @note Mainから呼ばれる処理
void FinderStudioBackGround::FreeShot()
{
  m_pBgSystem->PlayAnime(0);
  m_pBgSystem->GetSkyBox()->GetPartsModel(System::Skybox::Skybox::PARTS_TYPE_CLOUD)->GetAnimationController()->SetUpdateFrameEnable(true);
}

/// @brief スカイボックス表示・非表示切り替え
/// @param true:表示 false:非表示
/// @note デバッグ用
void FinderStudioBackGround::SetIsVisibleSkyBox(b32 isVisible)
{
  if(m_DebugIsVisibleSkyBox != isVisible)
  {
    if(!isVisible)
    {
      m_pFinderStudioRenderPipeLine->ResetSkyBoxRenderPath();
    }
    else
    {
      m_pFinderStudioRenderPipeLine->SetBgRenderPath(m_pBgSystem);
    }

    m_DebugIsVisibleSkyBox = isVisible;
  }
}

/// @brief 地面表示・非表示切り替え
/// @param true:表示 false:非表示
/// @note デバッグ用
void FinderStudioBackGround::SetIsVisibleGround(b32 isVisible)
{
  if(m_DebugIsVisibleGround != isVisible)
  {
    m_DebugIsVisibleGround = isVisible;
    StartReload(m_GroundType,m_NearType,m_FarType);
  }
}

/// @brief 近景表示・非表示切り替え
/// @param true:表示 false:非表示
/// @note デバッグ用
void FinderStudioBackGround::SetIsVisibleBackNear(b32 isVisible)
{
  if(m_DebugIsVisibleBackNear != isVisible)
  {
    m_DebugIsVisibleBackNear = isVisible;
    StartReload(m_GroundType,m_NearType,m_FarType);
  }
}

/// @brief 遠景表示・非表示切り替え
/// @param true:表示 false:非表示
/// @note デバッグ用
void FinderStudioBackGround::SetIsVisibleBackFar(b32 isVisible)
{
  if(m_DebugIsVisibleBackFar != isVisible)
  {
    m_DebugIsVisibleBackFar = isVisible;
    StartReload(m_GroundType,m_NearType,m_FarType);
  }
}

// 背景データ出力デバッグ開始
void FinderStudioBackGround::StartOutputBgData()
{
  m_DebugOutputFile = gfl2::fs::System::CreateDirectSdmcFile(m_pHeap,"BgData.log",gfl2::fs::Define::OpenMode::WRITE);
  m_DebugOutputNowObjType = 0;
  m_DebugOutputNowAnmType = 0;
  m_DebugOutputBgDataFlag = true;
}

// 背景データ出力デバッグ停止
void FinderStudioBackGround::StopOutputBgData()
{
  if(m_DebugOutputBgDataFlag)
  {
    gfl2::fs::System::DestroyDirectSdmcFile(m_DebugOutputFile);
    m_DebugOutputBgDataFlag = false;
  }
}

void FinderStudioBackGround::outPutLog(u32 objIndex,u32 slotIndex)
{
  char* OBJ_TYPE_STR[4] = {
    "GROUND",
    "NEAR",
    "FAR",
    "BUILD"
  };

  char* ANM_TYPE_STR[13] = {
    "LOOP",
    "MORNING_IN",
    "MORNING_LOOP",
    "MORNING_OUT",
    "NOON_IN",
    "NOON_LOOP",
    "NOON_OUT",
    "EVENING_IN",
    "EVENING_LOOP",
    "EVENING_OUT",
    "NIGHT_IN",
    "NIGHT_LOOP",
    "NIGHT_OUT"
  };

  char* TIME_ZONE_STR[3] = {
    "NOON",
    "EVENING",
    "NIGHT"
  };

  btl::BgSystem::BG_OBJ_TYPE OBJ_TYPE[4] = {
    btl::BgSystem::OBJ_GROUND,
    btl::BgSystem::OBJ_NEAR,
    btl::BgSystem::OBJ_FAR,
    btl::BgSystem::OBJ_BUILD
  };

  btl::BgSystem::BG_ANM_TYPE ANM_TYPE[13] = {
    btl::BgSystem::ANM_LOOP,
    btl::BgSystem::ANM_MORNING_IN,
    btl::BgSystem::ANM_MORNING_LOOP,
    btl::BgSystem::ANM_MORNING_OUT,
    btl::BgSystem::ANM_NOON_IN,
    btl::BgSystem::ANM_NOON_LOOP,
    btl::BgSystem::ANM_NOON_OUT,
    btl::BgSystem::ANM_EVENING_IN,
    btl::BgSystem::ANM_EVENING_LOOP,
    btl::BgSystem::ANM_EVENING_OUT,
    btl::BgSystem::ANM_NIGHT_IN,
    btl::BgSystem::ANM_NIGHT_LOOP,
    btl::BgSystem::ANM_NIGHT_OUT
  };

  int lastFrame = m_pBgSystem->GetLastAnimationFrame(OBJ_TYPE[objIndex],ANM_TYPE[slotIndex]);

  if(lastFrame == -1) return;

  char buf[512];
#if defined(GF_PLATFORM_WIN32)
  sprintf_s(buf,512,"BgSet[%02d][%s],OBJ[%s],Slot[%s],%04d\n",m_BgSetIndex,TIME_ZONE_STR[m_TimeZone],OBJ_TYPE_STR[objIndex],ANM_TYPE_STR[slotIndex],lastFrame);
#elif defined(GF_PLATFORM_CTR)
  sprintf(buf,"BgSet[%02d][%s],OBJ[%s],Slot[%s],%04d\n",m_BgSetIndex,TIME_ZONE_STR[m_TimeZone],OBJ_TYPE_STR[objIndex],ANM_TYPE_STR[slotIndex],lastFrame);
#endif
  int len = strlen(buf);
  m_DebugOutputFile->Write(buf,len);
}
#endif

GFL_NAMESPACE_END(Viewer)
GFL_NAMESPACE_END(FinderStudio)
GFL_NAMESPACE_END(App)
