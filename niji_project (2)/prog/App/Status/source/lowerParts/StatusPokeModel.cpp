//======================================================================
/**
 * @file    StatusPokeModel.cpp
 * @date    2015/12/03 18:41:50
 * @author  fukushima_yuya
 * @brief   ポケモンステータス：ポケモンモデル(下)
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "StatusPokeModel.h"

#include <App/Status/source/StatusDrawerBase.h>

#include <AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h>
#include <PokeTool/include/PokeModelSystem.h>
#include <Sound/include/Sound.h>
#include <System/include/gflUse.h>

// arc
#include <niji_conv_header/app/field_status/field_status_pane.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Status)

//------------------------------------------------------------------------------
// @brief   定数宣言
//------------------------------------------------------------------------------
namespace {
  enum {
    STATE_INIT,
    STATE_WAIT,
    STATE_LOAD,
    STATE_CREATE,
    STATE_SETUP,
    STATE_UPDATE,
    STATE_REMOVE,
    STATE_DELETE,
    STATE_END,
    STATE_NUM,
  };

  static const PokeTool::MODEL_ANIME DEFAULT_MODEL_ANIM = PokeTool::MODEL_ANIME_BT_WAIT_A;
}

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
PokeModel::PokeModel( DrawerBase* pBase, gfl2::fs::AsyncFileManager* pAsyncFileManager, app::util::AppRenderingManager* pRenderingManager )
  : m_pBase(pBase)
  , m_pAsyncFileManager(pAsyncFileManager)
  , m_pAppRenderingManager(pRenderingManager)
  , m_pPokeModelSystem(NULL)
  , m_pPokeParam(NULL)
  , m_pInFrameModel(NULL)
  , m_ModeAnimID(DEFAULT_MODEL_ANIM)
  , m_InitSeq(0)
  , m_DeleteSeq(0)
  , m_bReqStartDefAnim(false)
  , m_bReqPlayVoice(false)
  , m_AvailableAnimNum(0)
  , m_bReqAvailableAnim(false)
  , m_pModelAnimArea(NULL)
{
  // モデルアニメ再生エリアペイン
  m_pModelAnimArea = m_pBase->GetPane( PANENAME_FIELDSTATUS_MIN_LOW_000_PANE_BOUND_04 );
}

//------------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//------------------------------------------------------------------------------
PokeModel::~PokeModel( void )
{
  ;
}

//------------------------------------------------------------------------------
/**
 * @brief   初期化
 */
//------------------------------------------------------------------------------
bool PokeModel::Initialize( void )
{
  enum {
    INIT_SEQ_CREATE,
    INIT_SEQ_READY,
    INIT_SEQ_INIT_SYSTEM,
    INIT_SEQ_MODEL_CREATE,
    INIT_SEQ_END,
  };

  switch( m_InitSeq )
  {
  case INIT_SEQ_CREATE:
    {
      // モデルシステムの生成
      CreateModelSystem();

      m_InitSeq++;
    } break;

  case INIT_SEQ_READY:
    {
      // モデルシステムの準備完了待ち
      if( !IsReadyModelSystem() ) break;

      m_InitSeq++;
    } break;

  case INIT_SEQ_INIT_SYSTEM:
    {
      // モデルシステムの初期化
      InitModelSystem();

      m_InitSeq++;
    } break;

  case INIT_SEQ_MODEL_CREATE:
    {
      // モデルの生成
      CreateModel();

      m_InitSeq++;
    } break;

  case INIT_SEQ_END:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルシステムの生成
 */
//------------------------------------------------------------------------------
void PokeModel::CreateModelSystem( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // モデルシステムの生成
  m_pPokeModelSystem = GFL_NEW(devHeap) PokeTool::PokeModelSystem();
  {
    GFL_ASSERT( m_pPokeModelSystem );

    // システムの初期化を開始
    m_pPokeModelSystem->InitSystemAsync( devHeap, devHeap, m_pAsyncFileManager, 1 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルシステムの準備完了か
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeModel::IsReadyModelSystem( void )
{
  if( m_pPokeModelSystem )
  {
    return m_pPokeModelSystem->IsFinishInitSystemAsync();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルシステムの初期化
 */
//------------------------------------------------------------------------------
void PokeModel::InitModelSystem( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  // モデルシステムの作成
  m_pPokeModelSystem->CreateSystem( false, NULL );
  {
    // ポケモン用ヒープ作成
    PokeTool::PokeModelSystem::HeapOption opt;
    {
      opt.animeType       = PokeTool::MODEL_ANIMETYPE_BATTLE;
      opt.useIdModel      = false;
      opt.useShadow       = false;
      opt.useColorShader  = false;
    }
    m_pPokeModelSystem->CreatePokeModelHeap( devHeap, &opt );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルの生成
 */
//------------------------------------------------------------------------------
void PokeModel::CreateModel( void )
{
  gfl2::heap::HeapBase* devHeap = m_pBase->GetDeviceHeap();

  app::tool::PokeSimpleModel::INIT_PARAM initParam;
  {
    PokeTool::PokeModel::SetupOption* pOpt = &initParam.setupOpt;
    {
      pOpt->dataHeap  = devHeap;
      pOpt->workHeap  = devHeap->GetLowerHandle();
      pOpt->animeType = PokeTool::MODEL_ANIMETYPE_APP;
    }

    initParam.pPokeModelSys       = m_pPokeModelSystem;
    initParam.pRenderingManager   = m_pAppRenderingManager;
    initParam.renderPlace         = app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY;
  }

  // モデルの生成
  m_pInFrameModel = GFL_NEW(devHeap) app::tool::PokeSimpleModelInFrame( initParam );
  {
    GFL_ASSERT( m_pInFrameModel );

    // モデルの表示座標
    gfl2::math::VEC3 pos = m_pBase->GetPanePos( m_pModelAnimArea );
    {
      pos.x = pos.x + 160.0f;
      pos.y = 120.0f - pos.y;
    }
    
    // インフレーム枠のサイズ
    nw::lyt::Size size = m_pModelAnimArea->GetSize();
    {
      size.width  /= 2.0f;
      size.height /= 2.0f;
    }

    // インフレームの初期化
    m_pInFrameModel->InitInFrame( System::Camera::CModelInFrame::SCREEN_LOWER, System::Camera::CModelViewerInFrame::TURN_DIRECTION_RIGHT );
    m_pInFrameModel->SetInFrameFrame(
      (pos.x - size.width),
      (pos.y - size.height),
      (pos.x + size.width),
      (pos.y + size.height) );

    //  アニメ設定
    m_pInFrameModel->SetAnime( PokeTool::MODEL_ANIME_BT_WAIT_A, true );
    m_pInFrameModel->SetVisible( false );
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   破棄
 */
//------------------------------------------------------------------------------
bool PokeModel::Terminate( void )
{
  switch( m_DeleteSeq )
  {
  case 0:
    {
      m_pInFrameModel->Terminate();
      m_DeleteSeq++;
    } break;

  case 1:
    {
      if( !m_pInFrameModel->IsDelete() ) break;

      m_DeleteSeq++;
    } break;

  case 2:
    {
      m_pPokeModelSystem->DeletePokeModelHeap();
      m_pPokeModelSystem->TermSystemAsync();

      m_DeleteSeq++;
    } break;

  case 3:
    {
      if( !m_pPokeModelSystem->IsFinishTermSystemAsync() ) break;

      m_DeleteSeq++;
    } break;

  case 4:
    {
      GFL_SAFE_DELETE( m_pInFrameModel );
      GFL_SAFE_DELETE( m_pPokeModelSystem );

      m_DeleteSeq++;
    } break;

  case 5:
    {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   更新
 */
//------------------------------------------------------------------------------
void PokeModel::Update( void )
{
  if( m_pInFrameModel )
  {
    // モデルの更新
    m_pInFrameModel->Update();
    // モデルシステムの更新
    m_pPokeModelSystem->Update();

    // 利用可能なアニメーションのセット
    SetAvailableAnim();
    // 生成後のデフォルトアニメの再生
    StartDefaultAnim();
    // ボイス再生
    PlayPokeVoice();
    // タッチアニメの更新
    UpdateTouchAnim();
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルのセット
 * @param   pp    ポケモンパラメータ
 */
//------------------------------------------------------------------------------
void PokeModel::SetPokeModel( pml::pokepara::PokemonParam* pp )
{
  if( m_pInFrameModel )
  {
    //if( m_pPokeParam != pp )
    {
      PokeTool::GetSimpleParam( &m_SimpleParam, pp );

      // モデルのロード開始
      m_pInFrameModel->StartLoad( m_SimpleParam );

      m_bReqStartDefAnim  = true;
      m_bReqPlayVoice     = true;
      m_bReqAvailableAnim = true;

      m_pPokeParam = pp;
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルの準備チェック
 */
//------------------------------------------------------------------------------
bool PokeModel::IsModelReady( void ) const
{
  if( m_pInFrameModel )
  {
    return m_pInFrameModel->IsReady();
  }

  return true;
}

//------------------------------------------------------------------------------
/**
 * @brief   モデルの表示
 * @param   visible   表示フラグ
 */
//------------------------------------------------------------------------------
void PokeModel::SetVisibleModel( bool visible )
{
  if( m_pInFrameModel )
  {
    m_pInFrameModel->SetVisible( visible );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   デフォルトアニメの再生
 */
//------------------------------------------------------------------------------
void PokeModel::StartDefaultAnim( void )
{
  // モデルの準備が出来ていなければ何もしない
  if( !IsModelReady() ) return;

  if( m_bReqStartDefAnim )
  {
    // デフォルトのアニメIDをセット
    m_ModeAnimID = DEFAULT_MODEL_ANIM;

    // デフォルトのアニメを再生
    m_pInFrameModel->SetAnime( m_ModeAnimID, true );

    m_bReqStartDefAnim = false;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケモンのボイス再生
 */
//------------------------------------------------------------------------------
void PokeModel::PlayPokeVoice( void )
{
  // モデルの準備が出来ていなければ何もしない
  if( !IsModelReady() ) return;

  if( m_bReqPlayVoice )
  {
    MonsNo monsNo = m_SimpleParam.monsNo;
    FormNo formNo = m_SimpleParam.formNo;
    bool egg = m_SimpleParam.isEgg;

    // ポケモンが設定されている場合は泣き声を再生
    if( (monsNo != MONSNO_NULL && monsNo != MONSNO_TAMAGO) && !egg )
    {
      Sound::PlayVoice( 0, monsNo, formNo );
    }

    m_bReqPlayVoice = false;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   モデルのタッチ判定
 *
 * @param   touch   タッチパネル
 *
 * @note    内部でStartTouchAnimを呼ぶための関数
 */
//------------------------------------------------------------------------------
void PokeModel::CheckTouchModel( gfl2::ui::TouchPanel* touch )
{
  if( touch->IsTouchTrigger() )
  {
    if( m_pBase->IsHoldPane( touch, m_pModelAnimArea ) )
    {
      // エリアがタッチされていたらアニメを再生
      StartTouchAnim();
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチアニメの再生
 */
//------------------------------------------------------------------------------
void PokeModel::StartTouchAnim( void )
{
  // モデルの準備が出来ていなければ何もしない
  if( !IsModelReady() ) return;
  if( m_AvailableAnimNum == 0 ) return;

  if( m_ModeAnimID == DEFAULT_MODEL_ANIM )
  {
    // テーブル内の個数からランダム
    u32 rand = System::GflUse::GetPublicRand( m_AvailableAnimNum );
    
    // テーブル内のアニメID
    m_ModeAnimID = m_AvailableAnimTable[rand];

    // アニメの再生
    m_pInFrameModel->SetAnime( m_ModeAnimID, false );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチアニメの更新
 */
//------------------------------------------------------------------------------
void PokeModel::UpdateTouchAnim( void )
{
  // モデルの準備が出来ていなければ何もしない
  if( !IsModelReady() ) return;

  if( m_ModeAnimID != DEFAULT_MODEL_ANIM )
  {
    if( IsTouchAnimEnd() )
    {
      // デフォルトのアニメIDをセット
      m_ModeAnimID = DEFAULT_MODEL_ANIM;

      // デフォルトのアニメを再生
      m_pInFrameModel->SetAnime( m_ModeAnimID, true );
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   タッチアニメの終了判定
 *
 * @return  "true  = 終了"
 * @return  "false = それ以外"
 */
//------------------------------------------------------------------------------
bool PokeModel::IsTouchAnimEnd( void )
{
  if( m_pInFrameModel->IsEndAnime() )
  {
    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
/**
 * @brief   利用可能なアニメのセット
 */
//------------------------------------------------------------------------------
void PokeModel::SetAvailableAnim( void )
{
  // モデルの準備が出来ていなければ何もしない
  if( !IsModelReady() ) return;
  // アニメのセットがリクエストされていなければ何もしない
  if( !m_bReqAvailableAnim ) return;

  // ランダムで出る可能性のある技、バトルモーション
  static const PokeTool::MODEL_ANIME c_UseBtlAnim[] = {
    PokeTool::MODEL_ANIME_BUTURI1,
    PokeTool::MODEL_ANIME_BUTURI2,
    PokeTool::MODEL_ANIME_BUTURI3,
    PokeTool::MODEL_ANIME_BUTURI4,
    PokeTool::MODEL_ANIME_TOKUSHU1,
    PokeTool::MODEL_ANIME_TOKUSHU2,
    PokeTool::MODEL_ANIME_TOKUSHU3,
    PokeTool::MODEL_ANIME_TOKUSHU4,
  };

  // かわいがりモーション
  static const PokeTool::MODEL_ANIME c_UseKawaigariAnim[] = {
    PokeTool::MODEL_ANIME_HAPPY_B,
    PokeTool::MODEL_ANIME_MOVE_D,
  };

  const PokeTool::MODEL_ANIME* useAnimTable;
  u32 useAnimLen;

  // アニメーションテーブルのセット
  if( PokeTool::CheckUsingKawaigariInApp( m_pPokeParam ) )
  {
    useAnimTable = c_UseKawaigariAnim;
    useAnimLen   = GFL_NELEMS( c_UseKawaigariAnim );
  }
  else {
    useAnimTable = c_UseBtlAnim;
    useAnimLen   = GFL_NELEMS( c_UseBtlAnim );
  }

#if 0
  // 出現禁止技
  static const struct {
    MonsNo monsNo;
    FormNo formNo;
    PokeTool::MODEL_ANIME fobidAnim;
  }
  c_ForbidAnim[] = {
    /**
     * @note
     *  ギルガルドは特殊２モーションがフォルムチェンジ用で待機モーションに
     *  戻らないため、使用しない
     */
    {
      MONSNO_GIRUGARUDO,
      FORMNO_GIRUGARUDO_TATE,
      PokeTool::MODEL_ANIME_TOKUSHU2,
    },
    // sango追加分
    {
      MONSNO_HUUPA,
      FORMNO_HUUPA_EARNEST,
      PokeTool::MODEL_ANIME_BUTURI1,
    },
  };

  // パラメータの初期化
  m_AvailableAnimNum = 0;

  for( u32 i=0; i<useAnimLen; ++i )
  {
    bool isFobidAnim = false;

    for( u32 j=0; j<GFL_NELEMS(c_ForbidAnim); ++j )
    {
      // 出現禁止技のチェック
      if( useAnimTable[i] == c_ForbidAnim[j].fobidAnim
        && m_pPokeParam->GetMonsNo() == c_ForbidAnim[j].monsNo
        && m_pPokeParam->GetFormNo() == c_ForbidAnim[j].formNo )
      {
        isFobidAnim = true;   //!< 出現禁止
      }
    }

    // 出現禁止
    if( isFobidAnim )
    {
      continue;
    }

    // アニメーションが使用可能かどうか
    if( m_pInFrameModel->IsAvailableAnimation( useAnimTable[i] ) )
    {
      GFL_ASSERT( m_AvailableAnimNum < AVAILABLE_ANIM_MAX );
      m_AvailableAnimTable[ m_AvailableAnimNum++ ] = useAnimTable[i];
    }
  }
#else // @fix GFNMCat[4049]
  // パラメータの初期化
  m_AvailableAnimNum = 0;

  for( u32 i=0; i<useAnimLen; ++i )
  {
    // アニメーションが使用可能かどうか
    if( m_pInFrameModel->CheckCanOneshotMotion( useAnimTable[i] ) )
    {
      GFL_ASSERT( m_AvailableAnimNum < AVAILABLE_ANIM_MAX );
      m_AvailableAnimTable[ m_AvailableAnimNum++ ] = useAnimTable[i];
    }
  }
#endif

  m_bReqAvailableAnim = false;
}


GFL_NAMESPACE_END(Status)
GFL_NAMESPACE_END(App)
