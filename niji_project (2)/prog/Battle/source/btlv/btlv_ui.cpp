//======================================================================
/**
 * @file  btlv_ui.cpp
 * @brief バトル描画 ユーザーインターフェース管理
 * @author  ariizumi
 * @data  10/11/15
 */
//======================================================================

#include "btlv_core.h"
#include "btlv_ui.h"

#include "AppLib/include/Frame/AppFrameManager.h"

#include "Battle/source/btlv/render/btlv_RenderSystem.h"
#include "Battle/source/btl_tables.h"
#include "Battle/source/btlv/BtlvUiMsgWin.h"
#include "Battle/source/btlv/BtlvUIRotom.h"

#include <Print/include/SystemFont.h>

#include <Fade/include/gfl2_FadeManager.h>

#include <arc_index/message.gaix>
#include <niji_conv_header/message/msg_btl_app.h>


#include <niji_conv_header/app/battle/upper/minupp_pane.h>  //ゲージ類初期化のため
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  //ゲージ類初期化のため

GFL_NAMESPACE_BEGIN(btl)

BtlvUi::BtlvUi(BattleViewSystem* core)
  : m_btlvCore(core)
  , m_pokeListProc( this )
  , m_itemProc( this )
  {
    m_isInitialize              = false;
    m_bSetup                    = false;
    m_cleanUpState              = CLEANUP_STATE_FADE_OUT;
    m_pUILowerManagerLocalHeap  = NULL;
    m_pUILowerFrontFrameManager = NULL;
    m_pUIActSelectFrame         = NULL;
    m_pUIYesNoFrame             = NULL;
    m_pUISkillSelectFrame       = NULL;
    m_pUIBGFrame                = NULL;
    m_pUITargetSelectFrame      = NULL;
    m_pUIPokeListFrame          = NULL;
    m_pUIItemConfirmFrame       = NULL;
    m_pBtlParamAccesser         = NULL;
    m_pUIBattlePokeListFrame    = NULL;
    m_pWazaOshieFrame           = NULL;
#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
    m_pUIIntroEventFrame        = NULL;
#endif
    m_subStep                   = 0;
    m_bCleanUp                  = false;
    m_gaugeLevelUp              = NULL;
    m_gaugeTokusei              = NULL;
    m_bNotifyOteire             = false;
    m_bLowerFadeReset           = false;
    
    m_conventionWin             = NULL;
    m_gaugeTimer                = NULL;

    for( int i=0;i<4;i++ )
      {
        m_gaugeShotDown[i] = NULL;
      }

    gfl2::std::MemClear( m_pPosBackup, sizeof( m_pPosBackup ) );

    //  非常駐用ヒープ
    {
      m_pHeapTempAllocator    = GFL_NEW( m_btlvCore->GetResidentHeap() ) gfl2::heap::NwAllocator( m_btlvCore->GetTempHeap() );
      m_pDevHeapTempAllocator = GFL_NEW( m_btlvCore->GetResidentHeap() ) gfl2::heap::NwAllocator( m_btlvCore->GetTempHeap() );

      m_pGLHeapTempAllocator    = GFL_NEW( m_btlvCore->GetResidentHeap() ) gfl2::util::GLHeapAllocator( m_btlvCore->GetTempHeap() );
      m_pDevGLHeapTempAllocator = GFL_NEW( m_btlvCore->GetResidentHeap() ) gfl2::util::GLHeapAllocator( m_btlvCore->GetTempHeap() );
      m_appTempHeap.Create( m_btlvCore->GetTempHeap(), m_btlvCore->GetTempHeap(), m_pHeapTempAllocator, m_pDevHeapTempAllocator, m_pGLHeapTempAllocator, m_pDevGLHeapTempAllocator );
    }

   // ローカルヒープを作る
    m_AppHeap.LocalHeapCreate(m_btlvCore->GetResidentHeap(),m_btlvCore->GetResidentHeap(), 0xa000, 0x100000);  //160128 実測値0x00083fc8

    m_AppHeap.SetFileReadHeap(m_btlvCore->GetResidentHeap());
    m_pUiMsgWin = GFL_NEW(m_btlvCore->GetResidentHeap()) app::BtlvUiMsgWin(m_btlvCore, &m_AppHeap);
    m_pUiRotom = GFL_NEW(m_btlvCore->GetResidentHeap()) app::BtlvUiRotom(m_btlvCore);

    m_workStr[0] = GFL_NEW(m_btlvCore->GetResidentHeap()) gfl2::str::StrBuf(BTL_STRBUF_STD_LENGTH,m_btlvCore->GetResidentHeap());
}

BtlvUi::~BtlvUi()
{
}

//行動選択開始
bool BtlvUi::InitializeActSelect(void)
{
  enum
  {
    STEP_START = 0,
    STEP_WAIT
  };

  if( m_subStep == STEP_START )
  {
    // @fix NMCat[2481] 行動終了時にフラグをリセットしていたが、
    //                  行動終了(フラグリセット) ー＞ ひんしポケモン入れ替えー＞通信待機（フラグが設定）のケースを見逃していた
    //  フラグリセットは行動選択の段階でも入れるべき
    m_bLowerFadeReset = false;

    //  ターン終了後の最新の配置ポケモンリストを作成
    m_pBtlParamAccesser->CreatePokeList( m_pBtlParamAccesser->GetBattleRule() );

    //  行動選択フレーム作成
    _CreateSelectActFrame( false );

    //  天候、バトルルール
    m_pUIBGFrame->SetWeather( m_pBtlParamAccesser->GetWeather() );
    m_pUIBGFrame->SetRule( m_pBtlParamAccesser->GetBattleRule() );

    m_pBtlParamAccesser->ClearAllUniquPushBtnFlg();

    ++m_subStep;
  }

  if( m_subStep == STEP_WAIT )
  {
    GFL_ASSERT( m_pUIActSelectFrame );

    if( m_pUIActSelectFrame->IsReady() )
    {
      //  終わったら必ず初期化（これは他でも共通で使うから）
      m_subStep = 0;

      return true;
    }
  }

  return  false;
}

//------------------------------------------------------------------
/**
  * @brief   行動選択終了
  * @return  true -> 選択終了 / false -> 選択が終了していない
  * @note
  *          選択終了時には背景以外のフレームは死んでいないとだめ
  *          ここできちんと画面の後始末ができているかチェックする
  */
//------------------------------------------------------------------
bool BtlvUi::FinalizeActSelect()
{
  enum
  {
    FINALIZE_ACT_SELECT_STATE_END_FRONT_FRAME,
    FINALIZE_ACT_SELECT_STATE_FREE_ALIVE_FRAME,
    FINALIZE_ACT_SELECT_STATE_CHK_ALIVE_FRAME,
    FINALIZE_ACT_SELECT_STATE_FREE_FILE_DATA,
    FINALIZE_ACT_SELECT_STATE_CHK_FREE_FILE_DATA,
    FINALIZE_ACT_SELECT_STATE_FADE_RESET,
    FINALIZE_ACT_SELECT_STATE_END
  };

  //  暗転から解放する
  //  制限時間切れによる「通信待機中」表示している暗転フィルターが残っている可能性があるのでここで非表示設定するのが確実
  SetEnableBGFilter( false );

  //  手前に出しているフレームは最初に消す
  //  ポケモンリスト・バッグが該当する
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_END_FRONT_FRAME )
  {
    if( m_pUILowerFrontFrameManager->End() == false )
    {
      m_subStep = FINALIZE_ACT_SELECT_STATE_FREE_ALIVE_FRAME;
    }
  }

  //  消さなければいけないフレームが存在していれば消す
  //  画面トップは終了演出があるので、frameのendを呼んで１フレームで消すのはだめ
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_FREE_ALIVE_FRAME )
  {
    //  行動選択が生きているのでばあれば殺す
    if( m_pUIActSelectFrame != NULL )
    {
      if( m_uiActSelectFrameAppParam.out_param.bDie == false )
      {
        //  終了を呼ぶと内部でメモリ開放している
        m_pUIActSelectFrame->End();
      }
    }

    m_subStep = FINALIZE_ACT_SELECT_STATE_CHK_ALIVE_FRAME;
  }

  //  行動終了時には消えてなければいけないフレームが存在しているかチェック
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_CHK_ALIVE_FRAME )
  {
    //  消えてなければいけないフレーム一覧(BGフレームは残す)
    applib::frame::CellSuper* pEndChkFrameList[]  =
    {
      m_pUIActSelectFrame,
      m_pUISkillSelectFrame,
      m_pUIYesNoFrame,
      m_pUITargetSelectFrame,
      m_pUIPokeListFrame,
      m_pUIBattlePokeListFrame,
      m_pUIItemConfirmFrame,
      m_pUIBagFrame,
      m_pWazaOshieFrame,
#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
      m_pUIIntroEventFrame
#endif
    };

    u32 loopNum = GFL_NELEMS( pEndChkFrameList );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( pEndChkFrameList[ i ] != NULL )
      {
        return  false;
      }
    }

    m_subStep = FINALIZE_ACT_SELECT_STATE_FREE_FILE_DATA;
  }

  //  フレームで参照しているファイルデータを削除
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_FREE_FILE_DATA )
  {
    //  フレーム参照しているファイルデータはすべて削除しているかどうか
    //  現在すべてのファイルのチェックおよび開放をしている。
    //  理由としてはこのファイルデータをつかうケースが行動選択中で使うフレームしか参照しないから
    //  そうでない場合はバグる
    if( m_pBtlParamAccesser->IsFreeAllFileLoad() == false )
    {
      m_pBtlParamAccesser->FreeAllFileLoad();
    }

    m_subStep = FINALIZE_ACT_SELECT_STATE_CHK_FREE_FILE_DATA;
  }

  //  フレームで参照しているファイルデータがちゃんと消えているかチェック
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_CHK_FREE_FILE_DATA )
  {
    if( m_pBtlParamAccesser->IsFreeAllFileLoad() )
    {
      m_subStep = FINALIZE_ACT_SELECT_STATE_FADE_RESET;
    }
  }

  if( m_subStep == FINALIZE_ACT_SELECT_STATE_FADE_RESET )
  {
    //  フェードアウト途中で終了するケースが通信対戦の制限時間切れであるので、ここで黒画面にならないようにフェードアウトを解除
    //  ここでやっている場合はＢＧ以外のフレームがすべて破棄された時に
    //  なぜならフレーム終了処理でフェード処理をしている可能性があるから
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->Reset( gfl2::Fade::DISP_LOWER );

    //  行動選択が終った時は下フェードの強制リセットをなしに
    m_bLowerFadeReset = false;

    m_subStep = FINALIZE_ACT_SELECT_STATE_END;
  }

  //  行動選択が正しく終了できる
  if( m_subStep == FINALIZE_ACT_SELECT_STATE_END )
  {
    m_subStep = 0;

    //  ポケモンリストで確保しているポケパーティーのヒープ解放
    //  @fix GFNMCat[3207]  時間制限の場合のことを考えてここにヒープ解放を入れる
    //                      ひんしの入れ替え時など
    {
      m_uiPokeListFrameContextParam.friend_party.DeleteBattleData();
      m_uiPokeListFrameContextParam.my_party.DeleteBattleData();
      m_uiBagFrameAppParam.pokemon->DeleteBattleData();
      m_uiWazaOshieFrameSetupParam.partyData->DeleteBattleData();
    }

    return  true;
  }

  return false;
}


//初期化
bool BtlvUi::Initialize( Savedata::ConfigSave::MSG_SPEED msgSpeed )
{
  GFL_ASSERT( m_isInitialize == false );
  //ココでは変数の初期化とか

  enum
  {
    SEQ_LOAD_NUMFONT,
    SEQ_LOAD_NUMFONT_WAIT,
    SEQ_INIT_PARTS,
    SEQ_INIT_PARTS_WAIT,
    SEQ_OTHERS,
  };

  gfl2::heap::HeapBase *residentHeap   = m_btlvCore->GetResidentHeap();

  switch( m_subStep )
  {
  case SEQ_LOAD_NUMFONT:
    //  数字フォントは常駐化(サイズが少ないので問題なし)
    print::SystemFont_LoadNumFont( residentHeap, residentHeap );

    //一緒にメッセージのリソースの先読み登録もしておく
    {
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.arcId = print::GetMessageArcId();
      req.align = 128;
      req.heapForBuf = residentHeap;
      req.heapForReq = residentHeap->GetLowerHandle();
      req.datId = GARC_message_btl_app_DAT;
      req.ppBuf = &m_appMessageBuf;
      m_btlvCore->GetAsyncFileManager()->AddArcFileLoadDataReq(req);
    }
    m_subStep++;
    //break; //through
  case SEQ_LOAD_NUMFONT_WAIT:
    if( print::SystemFont_IsLoadNumFont() == false )
    {
      return false;
    }
    print::SystemFont_SetLoadNumFont(residentHeap);
    print::SystemFont_SetLytSystemNumFont();
    m_subStep++;
    //break; //through
  case SEQ_INIT_PARTS:
    if( m_btlvCore->GetAsyncFileManager()->IsArcFileLoadDataFinished( &m_appMessageBuf ) == false )
    {
      return false;
    }
    //  フレーム事前にメモリ確保できるのは確保しておく(何度も繰り返すのは速度として問題と思った)
    {
      m_uiBagFrameAppParam.pokemon            = GFL_NEW( residentHeap ) App::PokeList::IntermediateData();
      m_uiWazaOshieFrameSetupParam.partyData  = GFL_NEW( residentHeap ) App::PokeList::IntermediateData();
    }

    //  UIフレーム管理作成
    {
      // このマネージャーに必要なメモリを作成
      // フレームは生成・削除を何度も繰り返すのでヒープの穴を開けないようにするために専用に用意した
      m_pUILowerManagerLocalHeap = GFL_CREATE_LOCAL_HEAP( residentHeap, 4096, gfl2::heap::HEAP_TYPE_EXP, false );

      m_pUILowerFrameManager  = GFL_NEW( residentHeap ) applib::frame::Manager( m_pUILowerManagerLocalHeap );
      m_pUILowerFrontFrameManager = GFL_NEW( residentHeap ) applib::frame::Manager( m_pUILowerManagerLocalHeap );

      m_pUILowerFrontFrameManager->SetAppFrameListener( this );
      m_pUILowerFrameManager->SetAppFrameListener( this );
    }

    //  バトルUIデータ作成
    {  
      m_pBtlParamAccesser  = GFL_NEW( residentHeap ) app::BattleUIDataManager( m_btlvCore );
      //  デバイスヒープからクラス内部で使用するローカルヒープを作成
      m_pBtlParamAccesser->Initialize( residentHeap , m_appMessageBuf );
    }
    m_subStep++;
    //break; //through
  case SEQ_INIT_PARTS_WAIT:
    if( m_pBtlParamAccesser->InitializeWait(residentHeap) == false )
    {
      return false;
    }
    m_subStep++;
    //break; //through
  case SEQ_OTHERS:
    //メッセージ速度の設定
    m_pUiMsgWin->SetMessageSpeed(msgSpeed);

    m_isInitialize = true;
    //  終わったら必ず初期化（これは他でも共通で使うから）
    m_subStep = 0;
    return true;
    //break; //through
  }
  return false;
}

//解放して良いか？
bool BtlvUi::CanFinalize(void)
{
  //通信エラー時にInitializeは通るがSetupは通らない場合がある。
  //その時にm_pBtlParamAccesserでこけるのでここで対応する。
  if( m_bReadlBtlParamAccesser == false )
  {
    switch( m_forceFinalizeStep )
    {
    case 0:
      //  バトルUI共通パラメータが使えるか見る
      if( m_pBtlParamAccesser->IsReady() == false )
      {
        return false;
      }
      m_forceFinalizeStep++;
      //break; //through
    case 1:
      m_pBtlParamAccesser->DeletePocketData( m_uiBagFrameAppParam.pocket );
      m_pBtlParamAccesser->Terminate();
      m_forceFinalizeStep++;
      //break; //through
    case 2:
      if( m_pBtlParamAccesser->IsDelete() == false )
      {
        return false;
      }
      m_forceFinalizeStep++;
      //break; //through
    case 3:
      return true;
    }
  }
  return true;
}

//解放
bool BtlvUi::Finalize(void)
{
  //  ロードしたフォントを開放
  print::SystemFont_ResetDefaultFontInfo();
  print::SystemFont_DeleteNumFont();
  GflHeapSafeFreeMemory( m_appMessageBuf );
  GFL_SAFE_DELETE( m_uiBagFrameAppParam.pokemon );
  GFL_SAFE_DELETE( m_uiWazaOshieFrameSetupParam.partyData );
  GFL_SAFE_DELETE( m_pBtlParamAccesser );
  GFL_SAFE_DELETE( m_pUILowerFrontFrameManager );
  GFL_SAFE_DELETE( m_pUILowerFrameManager );

  for( int i=0;i<4;i++ )
  {
    GFL_SAFE_DELETE( m_gaugeBall[i] );
  }
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    GFL_SAFE_DELETE( m_gaugeStatus[i] );
  }

  // LevelUp Display
  GFL_SAFE_DELETE( m_gaugeLevelUp );

  // 特性表示
  GFL_SAFE_DELETE( m_gaugeTokusei );

  // 大会演出ウィンドウ表示
  GFL_SAFE_DELETE( m_conventionWin );

  // 対戦時間表示
  GFL_SAFE_DELETE( m_gaugeTimer );

  // ロイヤル用撃墜数表示
  for( int i=0;i<4;i++ )
    {
      GFL_SAFE_DELETE( m_gaugeShotDown[i] );
    }
  


  m_pUiMsgWin->Delete();
  GFL_SAFE_DELETE( m_pUiMsgWin );
  GFL_SAFE_DELETE(m_pUiRotom);

  GFL_SAFE_DELETE(m_workStr[0]);

  //  非常駐用のアロケーターを削除
  {
    GFL_SAFE_DELETE( m_pHeapTempAllocator );
    GFL_SAFE_DELETE( m_pDevHeapTempAllocator );

    GFL_SAFE_DELETE( m_pGLHeapTempAllocator );
    GFL_SAFE_DELETE( m_pDevGLHeapTempAllocator );
  }

  GFL_DELETE_HEAP( m_pUILowerManagerLocalHeap );

  m_isInitialize  = false;
  m_bSetup        = false;

  return true;
}

//開始時( バトル開始時に呼ばれる )
bool BtlvUi::Setup(void)
{
  GFL_ASSERT( m_bSetup == false );

  //ココでは共通リソースの読み込みとか

  //  バトルUI共通パラメータが使えるか見る
  if( m_pBtlParamAccesser->IsReady() == false )
  {
    return  false;
  }
  m_bReadlBtlParamAccesser = true;
  //  下画面の背景フレーム(常に表示するので常駐Heapを使用)
  if( m_pUIBGFrame == NULL )
  {
    gfl2::heap::HeapBase *pResidentHeap   = m_btlvCore->GetResidentHeap();

    m_pUIBGFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIBGFrame>( m_pUILowerFrameManager );
    GFL_ASSERT( m_pUIBGFrame );

    //  行動選択AppParamに背景フレームをアクセスする（開始演出を同期させるために）
    m_uiActSelectFrameAppParam.in_param.pBGFrame  = m_pUIBGFrame;
    {
      m_uiBGFrameAppParam.in_param.pParentAppHeap     = pResidentHeap;
      m_uiBGFrameAppParam.in_param.pParentDevHeap     = pResidentHeap;
      m_uiBGFrameAppParam.in_param.pAsyncFileManager  = m_btlvCore->GetAsyncFileManager();
      m_uiBGFrameAppParam.in_param.pRenderingManager  = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
      m_uiBGFrameAppParam.in_param.pTmpDevHeap        = pResidentHeap->GetLowerHandle();
      m_uiBGFrameAppParam.in_param.pBtlParamAccessror = m_pBtlParamAccesser;
    }
    m_pUIBGFrame->Initialize( &m_uiBGFrameAppParam );

    m_pUIBGFrame->SetWeather( m_pBtlParamAccesser->GetWeather() );
    m_pUIBGFrame->SetRule( m_pBtlParamAccesser->GetBattleRule() );
  }

  if( m_pUIBGFrame->IsReady() == false )
  {
    //  背景は準備が整うまで待つ
    return  false;
  }

#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  //  画面導入演出フレーム作成
  if( m_pUIIntroEventFrame == NULL )
  {
    m_pUIIntroEventFrame = MENU_FRAME_CALL_MODELESS<applib::frame::Manager, BattleUIIntroEventFrame>( m_pUILowerFrameManager );
    GFL_ASSERT( m_pUIIntroEventFrame );

    m_uiIntroEventFrameAppParam.in_param.pAsyncFileManager  = m_btlvCore->GetAsyncFileManager();
    //  メモリは後方確保にして断片化を防ぐ
    m_uiIntroEventFrameAppParam.in_param.pParentAppHeap     = m_appTempHeap.GetSystemHeap()->GetLowerHandle();
    m_uiIntroEventFrameAppParam.in_param.pParentDevHeap     = m_appTempHeap.GetDeviceHeap()->GetLowerHandle();
    m_uiIntroEventFrameAppParam.in_param.pRenderingManager  = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
    m_uiIntroEventFrameAppParam.in_param.pTmpDevHeap        = m_appTempHeap.GetDeviceHeap();

    m_pUIIntroEventFrame->Initialize( &m_uiIntroEventFrameAppParam );
  }
#endif

#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  if( m_pUIIntroEventFrame->IsReady() == false )
  {
    //  導入演出の準備が整うまで待つ
    return  false;
  }
#endif

  if(!m_pUiMsgWin->Setup(m_btlvCore->GetAsyncFileManager()))
  {
    return  false;
  }

  m_pUiMsgWin->Create();
  //MsgWinの持っているレイアウトからゲージ初期化
  {
    BtlvGaugeBase::INIT_PARAM param;
    param.lytIdx = 0;
    param.g2dUtil = m_pUiMsgWin->GetG2DUtil();
    param.lyt = m_pUiMsgWin->GetLayoutWork(param.lytIdx);
    param.dataManager = m_pBtlParamAccesser;

    //自分のゲージ
    param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_LB );
    param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_LB );
    param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_LB;
    param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__PARTS_GENDER_ICON_MALE;
    param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__PARTS_GENDER_ICON_FEMALE;
    param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__GAUGE_00__BATTLE_HPGAUGE_COLOR_NORMAL;
    param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__GAUGE_00__BATTLE_HPGAUGE_COLOR_HALF;
    param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__GAUGE_00__BATTLE_HPGAUGE_COLOR_FEW;
    param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__GAUGE_00__BATTLE_HPGAUGE_GAUGE_HP;
    param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB_KEEP_SICK;
    param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_LB;
    param.anmTbl[BtlvGaugeBase::ANM_GLOVE_BLUE   ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_01_BLUE;
    param.anmTbl[BtlvGaugeBase::ANM_GLOVE_YELLOW ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_01_YELLOW;
    param.anmTbl[BtlvGaugeBase::ANM_GLOVE_GREEN  ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_01_GREEN;
    param.anmTbl[BtlvGaugeBase::ANM_GLOVE_RED    ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_01_RED;
    param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__PARTS_MEGA_ICONPATERN_00;
    param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__PARTS_MEGA_ICONPATERN_01;
    param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LB__PARTS_MEGA_ICONPATERN_02;

    param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_GENDER;
    param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_SICK;
    param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_MEGA;
    param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_BALL;
    param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_00;
    param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = PANENAME_BATTLE_GAG_003_PANE_BATTLE_HPGAUGE;
    param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_01;
    param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_00;
    param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_02;
    param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_01;

    param.royalGloveLyt = PANENAME_BATTLE_MIN_UPP_000_PANE_GROVE_01;

    m_gaugeStatus[0] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,0);
    m_gaugeStatus[0]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_NEAR_1) );

    if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
    {
      m_gaugeStatus[0]->InitGlove( GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)m_btlvCore->GetClientID()) );
    }


    if( m_btlvCore->GetBattleRule() != BTL_RULE_ROYAL )
    {
      //シングル・ダブル
      //敵のゲージ
      param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_RT );
      param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_RT );
      param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_RT;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_GENDER_ICON_MALE;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_GENDER_ICON_FEMALE;
      param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_NORMAL;
      param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_HALF;
      param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_FEW;
      param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_GAUGE_HP;
      param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_KEEP_SICK;
      param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_RT;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_00;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_01;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_02;

      param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
      param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
      param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
      param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;

      m_gaugeStatus[1] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,1);
      m_gaugeStatus[1]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_FAR_1) );

      if( m_btlvCore->GetBattleRule() == BTL_RULE_DOUBLE )
      {
        //ダブル 自分２のゲージ
        param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_RB );
        param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_RB );
        param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_RB;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__PARTS_GENDER_ICON_MALE;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__PARTS_GENDER_ICON_FEMALE;
        param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__GAUGE_00__BATTLE_HPGAUGE_COLOR_NORMAL;
        param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__GAUGE_00__BATTLE_HPGAUGE_COLOR_HALF;
        param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__GAUGE_00__BATTLE_HPGAUGE_COLOR_FEW;
        param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__GAUGE_00__BATTLE_HPGAUGE_GAUGE_HP;
        param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB_KEEP_SICK;
        param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_RB;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__PARTS_MEGA_ICONPATERN_00;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__PARTS_MEGA_ICONPATERN_01;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RB__PARTS_MEGA_ICONPATERN_02;

        param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_GENDER;
        param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_SICK;
        param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_MEGA;
        param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_098_PANE_PARTS_BALL;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_00;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = PANENAME_BATTLE_GAG_003_PANE_BATTLE_HPGAUGE;
        param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = PANENAME_BATTLE_PLT_UPP_098_PANE_GAUGE_01;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_00;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_02;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = PANENAME_BATTLE_GAG_003_PANE_TEXTBOX_01;
        m_gaugeStatus[2] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,2);
        m_gaugeStatus[2]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_NEAR_2) );

        //ダブル 敵２のゲージ
        param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_LT );
        param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_LT );
        param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_LT;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_GENDER_ICON_MALE;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_GENDER_ICON_FEMALE;
        param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_NORMAL;
        param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_HALF;
        param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_FEW;
        param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_GAUGE_HP;
        param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_KEEP_SICK;
        param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_LT;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_00;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_01;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_02;

        param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
        param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
        param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
        param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;
        m_gaugeStatus[3] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,3);
        m_gaugeStatus[3]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_FAR_2) );
      }
      else
      if( m_btlvCore->GetBattleCompetitor() == BTL_COMPETITOR_WILD )
      {
        //乱入用 敵２のゲージ
        param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_LT );
        param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_LT );
        param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_LT;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_GENDER_ICON_MALE;
        param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_GENDER_ICON_FEMALE;
        param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_NORMAL;
        param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_HALF;
        param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_COLOR_FEW;
        param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__GAUGE_00_GAUGE_HP;
        param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_KEEP_SICK;
        param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_LT;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_00;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_01;
        param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT__PARTS_MEGA_ICONPATERN_02;

        param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
        param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
        param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
        param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
        param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
        param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;
        m_gaugeStatus[3] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,3);
        m_gaugeStatus[3]->SetPokeParam( m_btlvCore->GetBattlePokeParam((BtlvPos)0) );
      }
    }
    else
    {
      //ロイヤルの表示
      //敵１(右)
      param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_RT_00 );
      param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_RB_ROYAL );
      param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_RB_ROYAL;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__PARTS_GENDER_ICON_MALE;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__PARTS_GENDER_ICON_FEMALE;
      param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__GAUGE_00_COLOR_NORMAL;
      param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__GAUGE_00_COLOR_HALF;
      param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__GAUGE_00_COLOR_FEW;
      param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__GAUGE_00_GAUGE_HP;
      param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00_KEEP_SICK;
      param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_RB_ROYAL;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_BLUE   ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_02_BLUE;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_YELLOW ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_02_YELLOW;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_GREEN  ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_02_GREEN;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_RED    ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_02_RED;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__PARTS_MEGA_ICONPATERN_00;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__PARTS_MEGA_ICONPATERN_01;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_00__PARTS_MEGA_ICONPATERN_02;

      param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
      param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
      param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
      param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;

      param.royalGloveLyt = PANENAME_BATTLE_MIN_UPP_000_PANE_GROVE_02;

      m_gaugeStatus[1] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,1);
      m_gaugeStatus[1]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_ROYAL_RIGHT) );
      m_gaugeStatus[1]->InitGlove( GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)m_btlvCore->GetEnemyClientID(0) ) );

      //敵２(向かい)
      param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_RT );
      param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_RT );
      param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_RT;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_GENDER_ICON_MALE;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_GENDER_ICON_FEMALE;
      param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_NORMAL;
      param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_HALF;
      param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_COLOR_FEW;
      param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__GAUGE_00_GAUGE_HP;
      param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT_KEEP_SICK;
      param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_RT;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_BLUE   ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_03_BLUE;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_YELLOW ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_03_YELLOW;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_GREEN  ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_03_GREEN;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_RED    ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_03_RED;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_00;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_01;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_RT__PARTS_MEGA_ICONPATERN_02;

      param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
      param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
      param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
      param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;

      param.royalGloveLyt = PANENAME_BATTLE_MIN_UPP_000_PANE_GROVE_03;

      m_gaugeStatus[2] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,2);
      m_gaugeStatus[2]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_ROYAL_FAR) );
      m_gaugeStatus[2]->InitGlove( GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)m_btlvCore->GetEnemyClientID(1) ) );


      //敵３(左)
      param.partsLyt = param.lyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_POKEPLATE_LT_00 );
      param.baseLyt  = param.lyt->GetPane( PANENAME_BATTLE_MIN_UPP_000_PANE_HPNDBALL_LT_ROYAL );
      param.anmTbl[BtlvGaugeBase::ANM_GAUGE_IN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_IN_HP_LT_ROYAL;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_MALE  ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__PARTS_GENDER_ICON_MALE;
      param.anmTbl[BtlvGaugeBase::ANM_GENDER_FEMALE] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__PARTS_GENDER_ICON_FEMALE;
      param.anmTbl[BtlvGaugeBase::ANM_HP_GREEN     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__GAUGE_00_COLOR_NORMAL;
      param.anmTbl[BtlvGaugeBase::ANM_HP_YELLOW    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__GAUGE_00_COLOR_HALF;
      param.anmTbl[BtlvGaugeBase::ANM_HP_RED       ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__GAUGE_00_COLOR_FEW;
      param.anmTbl[BtlvGaugeBase::ANM_HP_VALUE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__GAUGE_00_GAUGE_HP;
      param.anmTbl[BtlvGaugeBase::ANM_SICK_LOOP    ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00_KEEP_SICK;
      param.anmTbl[BtlvGaugeBase::ANM_BATSUGUN     ] = LA_MINUPP_BATTLE_MIN_UPP_000_HPHIT_LT_ROYAL;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_BLUE   ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_00_BLUE;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_YELLOW ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_00_YELLOW;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_GREEN  ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_00_GREEN;
      param.anmTbl[BtlvGaugeBase::ANM_GLOVE_RED    ] = LA_MINUPP_BATTLE_MIN_UPP_000__GROVE_00_RED;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_MEGA_EVOLVE     ] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__PARTS_MEGA_ICONPATERN_00;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_ALPHA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__PARTS_MEGA_ICONPATERN_01;
      param.anmTbl[BtlvGaugeBase::ANM_ICON_GENSIKAIKI_OMEGA] = LA_MINUPP_BATTLE_MIN_UPP_000__POKEPLATE_LT_00__PARTS_MEGA_ICONPATERN_02;


      param.subPartsLyt[BtlvGaugeBase::PARTS_GENDER_ICON ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_GENDER;
      param.subPartsLyt[BtlvGaugeBase::PARTS_SICK_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_SICK;
      param.subPartsLyt[BtlvGaugeBase::PARTS_MEGA_ICON   ] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_MEGA;
      param.subPartsLyt[BtlvGaugeBase::PARTS_CAPTURE_ICON] = PANENAME_BATTLE_PLT_UPP_097_PANE_PARTS_BALL;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE1      ] = PANENAME_BATTLE_PLT_UPP_097_PANE_GAUGE_00;
      param.subPartsLyt[BtlvGaugeBase::PARTS_GAUGE2      ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_EXP_GAUGE   ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MIN  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_MAX  ] = -1;
      param.subPartsLyt[BtlvGaugeBase::PARTS_NUM_HP_SLASH] = -1;

      param.royalGloveLyt = PANENAME_BATTLE_MIN_UPP_000_PANE_GROVE_00;

      m_gaugeStatus[3] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBase(m_btlvCore,param,3);
      m_gaugeStatus[3]->SetPokeParam( m_btlvCore->GetBattlePokeParam(BTL_VPOS_ROYAL_LEFT) );
      m_gaugeStatus[3]->InitGlove( GetCornerColorFromBtlClientId( (BTL_CLIENT_ID)m_btlvCore->GetEnemyClientID(2) ) );

    }
  }
  //ボールゲージ
  {
    for( int i=0;i<4;i++ )
    {
      m_gaugeBall[i] = NULL;
    }

    BtlvGaugeBall::INIT_PARAM param;
    param.lytIdx = 0;
    param.g2dUtil = m_pUiMsgWin->GetG2DUtil();
    param.lyt = m_pUiMsgWin->GetLayoutWork(param.lytIdx);

    if( m_btlvCore->GetBattleRule() != BTL_RULE_ROYAL )
    {
      //シングル・ダブル
      param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LB );
      param.bgLyt = NULL;
      param.anmTbl[BtlvGaugeBall::ANM_DISP]  = LA_MINUPP_BATTLE_MIN_UPP_000_IN_BALL_LB;
      param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN;
      param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN_00;
      param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN_01;
      param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN_02;
      param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN_03;
      param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_BALL_IN_04;
      m_gaugeBall[0] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,0);

      if( m_btlvCore->GetBattleCompetitor() != BTL_COMPETITOR_WILD )
      {
        param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RT );
        param.bgLyt = NULL;
        param.anmTbl[BtlvGaugeBall::ANM_DISP]  = LA_MINUPP_BATTLE_MIN_UPP_000_IN_BALL_RT;
        param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN;
        param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN_00;
        param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN_01;
        param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN_02;
        param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN_03;
        param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_BALL_IN_04;
        m_gaugeBall[1] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,1);
      }
      else
      {
        m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RT ),false);
      }
      
      //ロイヤル用は非表示
      gfl2::lyt::LytParts *partsuPane;
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LT_ROYAL );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RT_ROYAL );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LB_ROYAL );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RB_ROYAL );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
    }
    else
    {
      //ロイヤル
      param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LB_ROYAL );
      param.bgLyt    = NULL;
      param.anmTbl[BtlvGaugeBall::ANM_DISP]  = -1;
      param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN;
      param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN_00;
      param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN_01;
      param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN_02;
      param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN_03;
      param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LB_ROYAL_BALL_IN_04;
      m_gaugeBall[0] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,0);

      param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RB_ROYAL );
      param.bgLyt    = NULL;
      param.anmTbl[BtlvGaugeBall::ANM_DISP]  = -1;
      param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN;
      param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN_00;
      param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN_01;
      param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN_02;
      param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN_03;
      param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RB_ROYAL_BALL_IN_04;
      m_gaugeBall[1] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,1);

      param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RT_ROYAL );
      param.bgLyt    = NULL;
      param.anmTbl[BtlvGaugeBall::ANM_DISP]  = -1;
      param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN;
      param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN_00;
      param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN_01;
      param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN_02;
      param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN_03;
      param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_RT_ROYAL_BALL_IN_04;
      m_gaugeBall[2] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,2);

      param.partsLyt = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LT_ROYAL );
      param.bgLyt    = NULL;
      param.anmTbl[BtlvGaugeBall::ANM_DISP]  = -1;
      param.anmTbl[BtlvGaugeBall::ANM_BALL1] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN;
      param.anmTbl[BtlvGaugeBall::ANM_BALL2] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN_00;
      param.anmTbl[BtlvGaugeBall::ANM_BALL3] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN_01;
      param.anmTbl[BtlvGaugeBall::ANM_BALL4] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN_02;
      param.anmTbl[BtlvGaugeBall::ANM_BALL5] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN_03;
      param.anmTbl[BtlvGaugeBall::ANM_BALL6] = LA_MINUPP_BATTLE_MIN_UPP_000__BALLPLATE_LT_ROYAL_BALL_IN_04;
      m_gaugeBall[3] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeBall(m_btlvCore,param,3);

      //ダブル用を非表示
      gfl2::lyt::LytParts *partsuPane;
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_LB );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_BALLPLATE_RT );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);

    }

    UpdateBallBarAll();
    HideBallBarAll();
  }

  // ロイヤル用撃墜数ゲージ
  {
    if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
      {
        for(int i=0; i<4; i++)
          {
            m_gaugeShotDown[i] = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeShotDown(m_btlvCore, m_pUiMsgWin->GetG2DUtil(), m_pUiMsgWin);
            m_gaugeShotDown[i]->Initialize( i );
          }

        HideShotDownBarAll();

      }
    else
      {
        // ロイヤル以外は非表示
        gfl2::lyt::LytParts *partsuPane;
        partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_LT );
        m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
        partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_LB );
        m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
        partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_RT );
        m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
        partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_STARPLATE_RB );
        m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,false);
      }
  }


  //6末アニメ用
  {
    mGaugeG2dUtil = m_pUiMsgWin->GetG2DUtil();
    mGaugeLyt = m_pUiMsgWin->GetLayoutWork(0);
  }

  //  導入演出開始
  {
    //  導入演出とBGで画面全体表示しているのでフェードアウトを解除
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->Reset( gfl2::Fade::DISP_LOWER );

#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
    m_pUIIntroEventFrame->Start( 0 );
#endif

    //  下画面のＢＧを開始して暗転状態にする
    m_pUIBGFrame->Start();
    m_pUIBGFrame->SetEnableDimFilterLayout( true );
  }

  // レベルアップ表示
  {
    m_gaugeLevelUp = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeLevelUp(m_btlvCore, m_pUiMsgWin->GetG2DUtil(), m_pUiMsgWin);
    m_gaugeLevelUp->Initialize ();
  }

  // 特性表示
  {
    m_gaugeTokusei = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeTokusei(m_btlvCore, m_pUiMsgWin->GetG2DUtil(), m_pUiMsgWin);
    m_gaugeTokusei->Initialize();
  }

  // 大会演出ウィンドウ表示
  {
    // NMCat [882] バトル大会演出の非表示機能
    bool disp_flag = true;
    const gfl2::ui::Button *pButton = m_btlvCore->GetButton();

    if( pButton->IsHold( gfl2::ui::BUTTON_L ) && 
        pButton->IsHold( gfl2::ui::BUTTON_R ) )
    {
      disp_flag = false;
    }

    if( m_btlvCore->IsConventionDirection() && disp_flag )
      {
        m_conventionWin = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvConventionWin(m_btlvCore, m_pUiMsgWin->GetG2DUtil(), m_pUiMsgWin);
        m_conventionWin->Initialize();
      }
  }
  // 対戦時間表示
  {
    m_gaugeTimer = GFL_NEW(m_btlvCore->GetResidentHeap())BtlvGaugeTimer(m_btlvCore, m_pUiMsgWin->GetG2DUtil(), m_pUiMsgWin);
    m_gaugeTimer->Initialize();
  }


  m_bSetup  = true;

  return m_bSetup;
}

//終了時( バトル終了時に呼ばれる )
bool BtlvUi::CleanUp(void)
{
  m_bCleanUp  = true;

  //  初回で強制フェードアウトで下画面が真っ暗に
  if( m_cleanUpState == CLEANUP_STATE_FADE_OUT )
  {
    //外で上とあわせてフェードします。 Ariizumi160225
    //gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    //pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_LOWER );

    m_cleanUpState  = CLEANUP_STATE_FRONT_FRAME_END;
  }

  //  すべて破棄してから終了遷移するようにする
  //  この段階で行動選択と背景のフレームは残っているので消すようにする
  if( m_cleanUpState == CLEANUP_STATE_FRONT_FRAME_END )
  {
    if( m_pUILowerFrontFrameManager->End() == false )
    {
      m_cleanUpState  = CLEANUP_STATE_NOTICE_OTEIRE;
    }
  }
  
  if( m_cleanUpState == CLEANUP_STATE_NOTICE_OTEIRE )
  {
    m_cleanUpState  = CLEANUP_STATE_END_FRAME;
  }
  else
  if( m_cleanUpState == CLEANUP_STATE_END_FRAME )
  {
    //  フレームが残っている可能性があるのでここでまとめて破棄
    if( m_pUILowerFrameManager->End() == false )
    {
      //  ここにきたらすべてのフレームが破棄されていないとアウト
      GFL_SAFE_DELETE( m_pUILowerFrameManager );

      //  UIデータ管理を開放待ち
      {
        m_pBtlParamAccesser->DeletePocketData( m_uiBagFrameAppParam.pocket );
        m_pBtlParamAccesser->Terminate();

        m_cleanUpState  = CLEANUP_STATE_DEL_UI_DATA;
      }
    }
  }
  else
  if( m_cleanUpState == CLEANUP_STATE_DEL_UI_DATA )
  {
    if( m_pBtlParamAccesser->IsDelete() )
    {
      return  true;
    }
  }

  return false;
}

//更新
void BtlvUi::Update(void)
{
  if( m_isInitialize  == false )
  {
    return;
  }

#if PM_DEBUG
  //デバッグ背景色変え
  if( m_btlvCore->GetDebugParam() && 
      m_btlvCore->GetDebugParam()->mWazaCheckMode )
  {
    if( m_pUIBGFrame )
    {
      m_pUIBGFrame->UpdateDebugBgColorCahnge();
    }
  }
#endif //PM_DEBUG

  //  お手入れ通知チェック
  {
    if( m_bNotifyOteire == false )
    {
      if( m_pUIBGFrame != NULL )
      {
        //  おていれボタンを押していたら押したことを通知する
        if( m_pUIBGFrame->IsPushOteire() == true )
        {
          m_btlvCore->NotifyOteireButtonPressed();
          m_bNotifyOteire = true;
        }
      }      
    }
  }

  if( m_pBtlParamAccesser != NULL )
  {
    m_pBtlParamAccesser->Update();
  }

  //  下UI画面のフレーム更新
  {
    bool  bFrontFrameManagerRun = false;
    if( m_pUILowerFrameManager != NULL )
    {
      if( m_pUILowerFrontFrameManager != NULL )
      {
        bFrontFrameManagerRun = m_pUILowerFrontFrameManager->IsMainHandle();
      }

      if( bFrontFrameManagerRun == false )
      {
        m_pUILowerFrameManager->Main();
      }
    }

    if( m_pUILowerFrontFrameManager != NULL )
    {
      m_pUILowerFrontFrameManager->Main();
    }
  }


  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] )
    {
      m_gaugeStatus[i]->Update();
    }
  }
  for( int i=0;i<4;i++ )
  {
    if( m_gaugeBall[i] )
    {
      m_gaugeBall[i]->Update();
    }
  }

  // 大会演出用ウィンドウ
  if( m_conventionWin )
    {
      m_conventionWin->Update();
    }

  // 対戦時間表示
  if( m_gaugeTimer )
    {
      m_gaugeTimer->Update();
    }

  // 撃墜数ゲージ
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
    {
      for( int i=0;i<4;i++ )
        {
          if( m_gaugeShotDown[i] )
            {
              m_gaugeShotDown[i]->Update();
            }
        }
    }

   UpdateMessage(); //メッセージ関連
  
  //  時間制限が発生などで下画面で走ったフェード処理をなかったことにするフラグがたっているか
  if( m_bLowerFadeReset == true )
  {
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    if( pFadeManager->IsEndStatus( gfl2::Fade::DISP_LOWER ) == false )
    {
      pFadeManager->Reset( gfl2::Fade::DISP_LOWER );
    }
  }
}

//更新
void BtlvUi::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if( m_isInitialize  == false )
  {
    return;
  }

  if( m_bSetup == false )
  {
    return;
  }

  //  クリーンアップ中はもう描画は呼ばない
  if( m_bCleanUp )
  {
    return;
  }

  //  下UI画面の描画
  if( m_pUILowerFrameManager != NULL )
  {
    bool  bFrontFrameManagerDraw  = false;

    if( m_pUILowerFrontFrameManager != NULL )
    {
      if( m_pUILowerFrontFrameManager->GetBaseProcessInitialized() == true )
      {
        // @fix GFNMCat[2863] フロント（全画面表示用）フレームが描画しているのであればバックの描画はしない
        applib::frame::CellSuper* pFrontFrameProcess = m_pUILowerFrontFrameManager->GetBaseProcess();
        if( pFrontFrameProcess == NULL )
        {
        }
        else
        if( pFrontFrameProcess->IsDrawEnable() == true )
        {
          bFrontFrameManagerDraw  = true;
        }
      }
    }

    if( bFrontFrameManagerDraw == false )
    {
      m_pUILowerFrameManager->Draw( displayNo );
    }
  }

  m_pUiRotom->Draw(displayNo);

  if( m_pUILowerFrontFrameManager != NULL )
  {
    m_pUILowerFrontFrameManager->Draw( displayNo );
  }

  //  バトル再生のみ専用描画
  {
    app::BattleUIRecPlayLowerView*  pRecPlayLowerView = m_pBtlParamAccesser->GetRecPlayLowerView();
    if( pRecPlayLowerView != NULL )
    {
      pRecPlayLowerView->DrawTree( displayNo );
    }
  }

  DrawMessage(displayNo); //メッセージ関連
}

//メッセージ読み込み
bool BtlvUi::LoadMessage(void)
{
  gfl2::heap::HeapBase *residentHeap   = m_btlvCore->GetResidentHeap();

  return m_pBtlParamAccesser->LoadMessage(residentHeap);
}

//動作系
//----------------------------------------------------
//  行動選択のフレーム状態を取得
BtlvUi::UIFrameState BtlvUi::GetStateActSelectFrame()
{
  if( m_pUIActSelectFrame == NULL )
  {
    return  UI_FRAME_STAET_NOT_EXIST;
  }
  else
  if( m_pUIActSelectFrame->IsEvent() == true )
  {
    return  UI_FRAME_STAET_EVENT_RUN;
  }

  //  フレームが存在してイベント中でない
  return  UI_FRAME_STAET_IDLE;
}

//  わざ選択のフレーム状態を取得
BtlvUi::UIFrameState BtlvUi::GetStateSkillSelectFrame()
{
  if( m_pUISkillSelectFrame == NULL )
  {
    return  UI_FRAME_STAET_NOT_EXIST;
  }
  else
  if( m_pUISkillSelectFrame->IsEvent() == true )
  {
    return  UI_FRAME_STAET_EVENT_RUN;
  }

  //  フレームが存在してイベント中でない
  return  UI_FRAME_STAET_IDLE;
}

//  bagフレーム状態を取得
BtlvUi::UIFrameState BtlvUi::GetStateBagFrame()
{
  if( m_itemProc.IsExistFrames() == false )
  {
    //  フレームが存在していない
    return  UI_FRAME_STAET_NOT_EXIST;
  }
  else
  if( m_itemProc.IsEventFrames() == true )
  {
    //  イベント演出中
    return  UI_FRAME_STAET_EVENT_RUN;
  }

  //  フレームが存在してイベント中でない
  return  UI_FRAME_STAET_IDLE;
}

//----------------------------------------------------
//コマンド選択
//-----------------------------------------------------
void BtlvUi::Start_SelectCommand( u8 poke_index, bool returnable )
{
  GFL_ASSERT( m_pUIActSelectFrame );

  //  フェードアウトしてここにくるケースがある（ダブルなどの複数戦）ので、ここで黒画面にならないようにフェードアウトを解除
  {
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->Reset( gfl2::Fade::DISP_LOWER );
  }

  //  @fix GFNMCat[17]
  //  現在のターンで入力中のポケモンのユニークボタンをクリアする
  //  メガ進化・全力技のボタン入力をターン内で一度のみ入力するため
  m_pBtlParamAccesser->ClearUniquPushBtnFlg( poke_index );

  //  行動選択開始時に必要な設定をする
  {
    m_uiActSelectFrameAppParam.in_param.bActReturn = returnable;

    BattleUIActSelectFrame::StateType state = m_pUIActSelectFrame->GetState();

    if( state == BattleUIActSelectFrame::STATE_TYPE_IDLE )
    {
      //  フレーム作成して待機状態から開始に
      m_pUIActSelectFrame->Start(poke_index);
      m_uiItemConfirmFrameAppParam.in_param.targetPokeIndex = poke_index;   // @fix MMCat[225]: 行動選択中のスロット番号をセット
    }
    else
    if( state == BattleUIActSelectFrame::STATE_TYPE_SUSPEND )
    {
      //  中断からの復帰
      m_pUIActSelectFrame->Restart(poke_index);
      m_uiItemConfirmFrameAppParam.in_param.targetPokeIndex = poke_index;   // @fix MMCat[225]: 行動選択中のスロット番号をセット
    }
    else
    {
      //  待機でもなく中断でもない場合は論理バグになる
      GFL_ASSERT( 0 );
    }
  }
}

void BtlvUi::StartAnime_SelectCommand(void)
{
}

BTLV_COMMAND_RESULT BtlvUi::WaitAnime_SelectCommand(void)
{
  //  行動選択終了
  if( m_uiActSelectFrameAppParam.out_param.bEnd )
  {
    //  たたかう選択
    if( m_uiActSelectFrameAppParam.out_param.resultData.type == btl::BattleUIActSelectFrame::RESULT_TYPE_FIGHT )
    {
      m_pBtlParamAccesser->SetTopActViewCursorMemory( btl::app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_FIGHT );
      return BCR_SEL_SKILL;
    }
    //  にげる選択
    else
    if( m_uiActSelectFrameAppParam.out_param.resultData.type == btl::BattleUIActSelectFrame::RESULT_TYPE_RUN )
    {
      m_pBtlParamAccesser->SetTopActViewCursorMemory( btl::app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_RUN );
      return BCR_SEL_RUN;
    }
    //  バッグ選択
    else
    if( m_uiActSelectFrameAppParam.out_param.resultData.type == btl::BattleUIActSelectFrame::RESULT_TYPE_BAG )
    {
      m_pBtlParamAccesser->SetTopActViewCursorMemory( btl::app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_BAG );
      m_itemProc.PreStart( ItemProc::ITEM_FLOW_TYPE_BAG, m_pBtlParamAccesser->GetBattleParty_Self() );
      return BCR_SEL_ITEM;
    }
    //  ポケモン選択
    else
    if( m_uiActSelectFrameAppParam.out_param.resultData.type == btl::BattleUIActSelectFrame::RESULT_TYPE_POKE )
    {
      m_pBtlParamAccesser->SetTopActViewCursorMemory( btl::app::BattleUIActSelectLowerDraw::CURSOR_POS_TYPE_POKE );
      return BCR_SEL_POKE;
    }
    //  ボール使用確認画面へ
    else
    if( m_uiActSelectFrameAppParam.out_param.resultData.type == btl::BattleUIActSelectFrame::RESULT_TYPE_MONS_BALL )
    {
      //  モンスターボールはカーソルがないので記憶は必要ない

      m_itemProc.PreStart( ItemProc::ITEM_FLOW_TYPE_USE_BALL_SHORT_CUT, m_pBtlParamAccesser->GetBattleParty_Self() );
      return BCR_SEL_ITEM;
    }
  }

  return BCR_SEL_NONE;
}
void BtlvUi::EndAnime_SelectCommand(void)
{
  //  不要( 行動選択フレームクラス内でやっているの )
}

//------------------------------------------------------------------
/**
  * @brief   行動選択のコマンド通知
  * @return  true -> 通知成功 / false -> 通知失敗
  * @note
  *   フレームが待機中のみ有効
  */
//------------------------------------------------------------------
bool BtlvUi::PushCmdActSelectCommnad( const BattleUIActSelectFrame::CmdType type )
{
  UIFrameState  state = GetStateActSelectFrame();
  if( state == UI_FRAME_STAET_IDLE )
  {
    GFL_ASSERT( m_pUIActSelectFrame );

    return  m_pUIActSelectFrame->PushCmd( type );
  }

  return  false;
}

/**
 * @brief 行動選択画面を強制終了
 */
void BtlvUi::ForceQuit_SelectCommand(void)
{
  _ForceQuitActFrame();
}

//-----------------------------------------------------
// わざ選択
//-----------------------------------------------------
void BtlvUi::Start_SelectSkill( u8 poke_index )
{
  // @fix GFNMCat[937] ここでユニークボタンの結果をクリア
  // ダブルバトルで対象選択から戻った時にクリアしていないと、メガ進化や全力ができない
  m_pBtlParamAccesser->ClearUniquPushBtnFlg( poke_index );

  if( m_pUISkillSelectFrame == NULL )
  {
    m_pUISkillSelectFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUISkillSelectFrame>( m_pUILowerFrameManager );

    //  入れ替えをしているならカーソル記憶はリセット
    {
      u32 loopNum = m_pBtlParamAccesser->GetPokeCursorMemoryLength();
      //  バトルに配置しているポケモンは必ずパーティーidxの0から開始する。
      //  ポケモン入れ替え時にパーティー配置順が変わる
      //  手持ちポケモンで最大で出せるのはダブルの２体までダブルならパーティーidxの0,1をチェックする
      for( u32 i = 0; i < loopNum; ++i )
      {
        if( IsExchangeOccurred( i ) == true )
        {
          m_pBtlParamAccesser->GetPokeCursorMemory( i ).Reset();
        }
      }
      //  入れ替えデータを最新へ
      UpdateExchangeInfo();
    }

    //  フレーム設定
    {
      ::app::util::Heap*  pUtilHeap = &m_appTempHeap;

      BattleUISkillSelectFrame::APP_PARAM::_tag_in_param* pInParam  = &m_uiSkillSelectFrameAppFrame.in_param;

      pInParam->pAsyncFileManager   = m_btlvCore->GetAsyncFileManager();
      pInParam->pRenderingManager   = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
      pInParam->pParentAppHeap      = pUtilHeap->GetSystemHeap();
      pInParam->pParentDevHeap      = pUtilHeap->GetDeviceHeap();
      pInParam->pBtlParamAccessror  = m_pBtlParamAccesser;
      pInParam->pTmpDevHeap         = pUtilHeap->GetDeviceHeap();

      //  以前のわざ選択画面のカーソル位置を設定(指定したパーティポケモンが変わっている場合は初期化)
      app::PokeCursorMemory&  rCursorMemory = m_pBtlParamAccesser->GetPokeCursorMemory( poke_index );
      m_uiSkillSelectFrameAppFrame.in_param.firstSkillListCursorType  = (app::BattleUISkillSelectLowerDraw::CursorType)rCursorMemory.GetWazaCursorPos();

      //  自身のパーティーidx
      m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx  = poke_index;
    }

    m_pUISkillSelectFrame->Initialize( &m_uiSkillSelectFrameAppFrame );
  }
  else
  if( m_uiSkillSelectFrameAppFrame.out_param.bEnterSuspend == true )
  {
    //  一時停止状態なら復帰させる
    m_pUISkillSelectFrame->Resume();
  }
  else
  {
    //  論理バグ
    GFL_ASSERT( 0 );
  }

  //  トップ画面の手前にに暗転フィルター表示設定
  if( m_pUIBGFrame != NULL )
  {
    m_pUIBGFrame->SetEnableDimFilterLayout( true );
  }
}

/**
 * @brief 技選択画面を開始する
 * @param poke_index  技を選択するポケモンのインデックス
 */
void BtlvUi::StartAnime_SelectSkill( u8 poke_index )
{
  //  不要
}
 
//  入れ替えポケモン更新 
void BtlvUi::UpdateExchangeInfo()
{
  u32 loopNum = GFL_NELEMS( m_pPosBackup );
  for(u32 i = 0; i < loopNum; ++i)
  {
    m_pPosBackup[ i ] = m_btlvCore->GetBattleParty_Self()->GetMemberData(i);
  }
}

//  指定したポケモンが入れ替えしているか
bool BtlvUi::IsExchangeOccurred(int pos)
{
  return m_pPosBackup[ pos ] != m_btlvCore->GetBattleParty_Self()->GetMemberData(pos);
}

// 指定位置のポケモンが選択済みか（瀕死入れ替え用）
bool BtlvUi::IsExchangeOccurredAll(int index)
{
  return false;
}

//  アクション用の全フレームを終了させる
void BtlvUi::_ForceQuitActFrame()
{
  // @fix GFNMCat[1645] 汎用インフォウィンドウが開いている場合があるので強制閉じる処理を入れる
  {
    ::app::tool::InfoWindowSheetType* pCommonInfoWindowView = m_pBtlParamAccesser->GetInfoWindowView();
    GFL_ASSERT( pCommonInfoWindowView );
    pCommonInfoWindowView->CloseManually();
    pCommonInfoWindowView->RemoveFromSuperView();
  }

  if( m_pUIActSelectFrame != NULL )
  {
    //  画面を閉じて強制終了(子フレームのみ)
    //  通信情報画面・ポケモン詳細ステータス
    //  トップフレーム終了は行動選択終了時に呼ばれる
    m_pUIActSelectFrame->EndChildFrame();
    // @fix GFNMCat[5269] フレーム終了時に入力はOFFに
    m_pUIActSelectFrame->SetEnableInput( false );
  }  

  if( m_pUISkillSelectFrame != NULL )
  {
    m_pUISkillSelectFrame->End();
  }

  if( m_pUITargetSelectFrame != NULL )
  {
    m_pUITargetSelectFrame->End();
  }

  if( m_pUIYesNoFrame != NULL )
  {
    m_pUIYesNoFrame->End();
  }

  m_pokeListProc.Die();

  m_itemProc.Die();
}

/**
 * @brief 技選択画面を更新する
 * @retval true   技選択が完了
 * @retval false  技選択中
 */
bool BtlvUi::WaitAnime_SelectSkill(void)
{
  bool  bRet  = m_uiSkillSelectFrameAppFrame.out_param.bEnterSuspend;
  return bRet;
}

void BtlvUi::Reset_SelectSkill(void) //選びなおし
{
}

//------------------------------------------------------------------
/**
  * @brief   わざ選択のコマンド通知
  * @return  true -> 通知成功 / false -> 通知失敗
  * @note
  *   わざ選択フレームが待機中のみ有効
  */
//------------------------------------------------------------------
bool BtlvUi::PushCmdSkillSelectCommnad( const BattleUISkillSelectFrame::CmdType type )
{
  UIFrameState  state = GetStateSkillSelectFrame();
  if( state == UI_FRAME_STAET_IDLE )
  {
    GFL_ASSERT( m_pUISkillSelectFrame );

    return  m_pUISkillSelectFrame->PushCmd( type );
  }

  return  false;
}

/**
 * @brief 技選択画面の結果を取得する
 * @param[out] pOutResult      : 結果の格納先
 * @param[out] pOutUseMega     : メガ進化するか？
 * @param[out] pOutUseAuraWaza : オーラ技を使用するか
 * @param[out] pOutUseUltraBurst : ウルトラバーストするか
 */
void BtlvUi::GetResult_SelectSkill(BTLV_COMMAND_RESULT* pOutResult, bool* pOutUseMega, bool* pOutUseAuraWaza, bool *pOutUseUltraBurst)
{
  GFL_ASSERT( pOutResult );
  GFL_ASSERT( pOutUseMega );
  GFL_ASSERT( pOutUseAuraWaza );

  *pOutResult = BCR_SEL_NONE;

  if( m_uiSkillSelectFrameAppFrame.out_param.bEnterSuspend == true )
  {
    *pOutUseMega      = m_uiSkillSelectFrameAppFrame.out_param.resultData.bMega;
    *pOutUseAuraWaza  = m_uiSkillSelectFrameAppFrame.out_param.resultData.bZenryoku;
    *pOutUseUltraBurst= m_uiSkillSelectFrameAppFrame.out_param.resultData.bUltraBurst;

    //  メガ進化と全力技を押した情報を記憶
    if( *pOutUseMega == true )
    {
      m_pBtlParamAccesser->PushMegaBtn( m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx );
    }

    if( *pOutUseAuraWaza == true )
    {
      m_pBtlParamAccesser->PushZenryokuBtn( m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx );
    }
    if (*pOutUseUltraBurst == true)
    {
      m_pBtlParamAccesser->PushUltraBurstBtn(m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx);
    }


    app::PokeCursorMemory& rPokeCursorMemory  = m_pBtlParamAccesser->GetPokeCursorMemory( m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx );
    //  押した技選択カーソル位置を保存
    {
      rPokeCursorMemory.SetWazaCursorPos( m_uiSkillSelectFrameAppFrame.out_param.resultData.endCurrentCursorType );
    }

    //  もどる選択をした
    if( m_uiSkillSelectFrameAppFrame.out_param.resultData.type == BattleUISkillSelectFrame::RESULT_TYPE_BACK )
    {
      //  戻るの時はユニークボタン入力をクリア
      m_pBtlParamAccesser->ClearUniquPushBtnFlg( m_uiSkillSelectFrameAppFrame.in_param.selfPartyPokeIdx );
      *pOutResult = BCR_SEL_CANCEL;
    }
    //  わざ選択を押した
    else
    if( m_uiSkillSelectFrameAppFrame.out_param.resultData.type == BattleUISkillSelectFrame::RESULT_TYPE_SKILL_SELECT )
    {
      s32 skillListIdx  = m_uiSkillSelectFrameAppFrame.out_param.resultData.push_skill_select.list_idx;
      const BTLV_COMMAND_RESULT aCommandTable[] =
      {
        BCR_SEL_IDX1,
        BCR_SEL_IDX2,
        BCR_SEL_IDX3,
        BCR_SEL_IDX4
      };

      GFL_ASSERT( skillListIdx < GFL_NELEMS( aCommandTable ) );
      *pOutResult = aCommandTable[ skillListIdx ];
    }
    else
    {
      //  強制終了した場合の設定
      *pOutResult = BCR_SEL_IDX1;
    }
  }
}

/**
 * @brief 技選択画面の結果を取得する（技情報を表示するか）
 */
bool BtlvUi::GetResult_SelectInfo(void)
{
  //  不要（技詳細表示はわざ選択フレーム内で行っている）
  return false;
}


/**
 * @brief 技選択画面を終了する
 */
bool BtlvUi::EndAnime_SelectSkill(void)
{
  enum
  {
    SEQ_INIT  = 0,
    SEQ_WAIT,
    SEQ_END
  };

  if( m_subStep == SEQ_INIT )
  {
    if( m_pUISkillSelectFrame != NULL )
    {
      m_pUISkillSelectFrame->End();

      ++m_subStep;
    }
  }

  if( m_subStep == SEQ_WAIT )
  {
    if( m_pUISkillSelectFrame == NULL )
    {
      ++m_subStep;
    }
  }

  if( m_subStep == SEQ_END )
  {
    m_subStep = 0;

    return  true;
  }
  
  return  false;
}

/**
 * @brief 技選択画面を強制終了する
 */
void BtlvUi::ForceQuit_SelectSkill(void)
{
  _ForceQuitActFrame();
}


//技忘れ
void BtlvUi::Start_WazaWasure( u8 pos, WazaID waza )
{
  //  全画面用フレーム管理に登録
  m_pWazaOshieFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::WazaOshie::WazaOshieFrame>( m_pUILowerFrontFrameManager );

  ::app::util::Heap*  pUtilHeap = &m_appTempHeap;

  //  フレーム起動時に必要な設定
  {
    m_uiWazaOshieFrameAppParam.mode               = App::WazaOshie::MODE_WAZA_OBOE;
    m_uiWazaOshieFrameAppParam.waza_no            = waza;
    m_uiWazaOshieFrameAppParam.select_poke_index  = pos;

    m_uiWazaOshieFrameSetupParam.heap           = pUtilHeap;
    m_uiWazaOshieFrameSetupParam.renderMan      = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
    m_uiWazaOshieFrameSetupParam.cmnIcon        = &m_pBtlParamAccesser->GetAppCommonGrpIconData();
    m_uiWazaOshieFrameSetupParam.fadeMan        = NULL;
    m_uiWazaOshieFrameSetupParam.createNumFont  = false;
    m_uiWazaOshieFrameSetupParam.partyData->SetParameter( m_btlvCore->GetBattleParty_Self(), m_btlvCore->GetTempHeap() );
  }

  m_pWazaOshieFrame->Setup( &m_uiWazaOshieFrameAppParam, m_uiWazaOshieFrameSetupParam );
}

BTLV_COMMAND_RESULT BtlvUi::WaitAnime_WazaWasure(void)
{
  if( m_pWazaOshieFrame == NULL )
  {
    m_uiWazaOshieFrameSetupParam.partyData->DeleteBattleData();

    // @fix NMCat[3649] 暗転を消す
    SetEnableBGFilter( false );

    //  わざ忘れのフレーム終了
    if( m_uiWazaOshieFrameAppParam.end_mode == App::WazaOshie::END_MODE_CANCEL )
    {
      return  BCR_SEL_CANCEL;
    }
    else
    if( m_uiWazaOshieFrameAppParam.end_mode == App::WazaOshie::END_MODE_DECIDE )
    {
      //  わすれる技を決めた
      if( m_uiWazaOshieFrameAppParam.select_index == 0 )
      {
        return  BCR_SEL_IDX1;
      }
      else
      if( m_uiWazaOshieFrameAppParam.select_index == 1 )
      {
        return  BCR_SEL_IDX2;
      }
      else
      if( m_uiWazaOshieFrameAppParam.select_index == 2 )
      {
        return  BCR_SEL_IDX3;
      }
      else
      if( m_uiWazaOshieFrameAppParam.select_index == 3 )
      {
        return  BCR_SEL_IDX4;
      }
      else
      if( m_uiWazaOshieFrameAppParam.select_index == 4 )
      {
        //  新しく覚えた技を選択
        return  BCR_SEL_IDX5;
      }

      return  BCR_SEL_CANCEL;
    }
  }

  return BCR_SEL_NONE;  // まだ終わってない
}

void BtlvUi::WaitAnime_EndWasure(void)
{
}

//-----------------------------------------------------
// ターゲット選択
//-----------------------------------------------------

/**
 * @brief 対象選択を開始する
 * @param poke_index  技を使用するポケモンのインデックス
 * @param poke_param  ポケモンパラメータ
 * @param wazano      使用する技
 */
void BtlvUi::StartAnime_Target( u8 poke_index, const BTL_POKEPARAM* poke_param, WazaNo wazano )
{
  GFL_ASSERT( m_pUITargetSelectFrame == NULL );

  //  FrameをNewして登録
  {
    m_pUITargetSelectFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUITargetSelectFrame>( m_pUILowerFrameManager );
    GFL_ASSERT( m_pUITargetSelectFrame );

    ::app::util::Heap*  pUtilHeap = &m_appTempHeap;

    BattleUITargetSelectFrame::APP_PARAM::_tag_in_param*  pInParam  = &m_uiTargetSelectFrameAppParam.in_param;

    pInParam->pAsyncFileManager     = m_btlvCore->GetAsyncFileManager();
    pInParam->pRenderingManager     = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
    pInParam->pParentAppHeap        = pUtilHeap->GetSystemHeap();
    pInParam->pParentDevHeap        = pUtilHeap->GetDeviceHeap();
    pInParam->pBtlParamAccessror    = m_pBtlParamAccesser;
    pInParam->pokeIndex             = poke_index;
    pInParam->wazaNo                = wazano;
    pInParam->pPokeParam            = poke_param;
    /**
      @fix  MMcat[240]   立ち位置Cのポケモンが「味方場」「味方全ポケ」の効果範囲の技を使用した後の、カーソル位置がおかしい
            立ち位置Cのポケモンで「味方場」の技（リフレクターなど）によりカーソル位置を記憶（立ち位置A）した後、「通常ポケ」が対象の技（でんじはなど）を選択すると、
            立ち位置Aが選択可能であった場合にはそれが採用されてしまう（でんじはを味方Aに撃とうとする）
            目的の違う別の技の情報を使用して初期カーソル位置を決定していることに問題があるため、カーソル位置の記憶に技Noを紐付けて管理するようにした
    */
    pInParam->firstCursorPos        = m_pBtlParamAccesser->GetPokeCursorMemory( pInParam->pokeIndex ).GetTargetCursorPos(wazano);
    pInParam->pTmpDevHeap           = pUtilHeap->GetDeviceHeap();
  }

  //  フレーム内部で画面
  m_pUITargetSelectFrame->Initialize( &m_uiTargetSelectFrameAppParam );
}

/**
 * @brief 対象選択を更新する
 * @retval BCR_NONE                    選択中
 * @retval BCR_CANCEL                  選択せずに戻った
 * @retval BCR_SEL_IDX1〜BCR_SEL_IDX6  対象を選択した
 */
BTLV_COMMAND_RESULT BtlvUi::WaitAnime_Target(void)
{
  if( m_uiTargetSelectFrameAppParam.out_param.bEnd == true )
  {
    /**
      @fix  MMcat[240]   立ち位置Cのポケモンが「味方場」「味方全ポケ」の効果範囲の技を使用した後の、カーソル位置がおかしい
            立ち位置Cのポケモンで「味方場」の技（リフレクターなど）によりカーソル位置を記憶（立ち位置A）した後、「通常ポケ」が対象の技（でんじはなど）を選択すると、
            立ち位置Aが選択可能であった場合にはそれが採用されてしまう（でんじはを味方Aに撃とうとする）
            目的の違う別の技の情報を使用して初期カーソル位置を決定していることに問題があるため、カーソル位置の記憶に技Noを紐付けて管理するようにした
    */
    //  カーソル位置を記憶
    {
      m_pBtlParamAccesser->GetPokeCursorMemory( m_uiTargetSelectFrameAppParam.in_param.pokeIndex ).SetTargetCursorPos( m_uiTargetSelectFrameAppParam.out_param.selectItemPos, m_uiTargetSelectFrameAppParam.in_param.wazaNo );
    }

    if( m_uiTargetSelectFrameAppParam.out_param.btlvPos == BTL_VPOS_ERROR )
    {
      return  BCR_SEL_CANCEL;
    }
    else
    {
      //  対象選択を返す
      return  (BTLV_COMMAND_RESULT)( BCR_SEL_IDX1 + m_uiTargetSelectFrameAppParam.out_param.btlvPos );
    }
  }

  return BCR_SEL_NONE;
}

/**
 * @brief 対象選択を終了する
 */
void BtlvUi::EndAnime_Target(void)
{
}

/**
 * @brief 対象選択画面を強制終了する
 */
void BtlvUi::ForceQuit_Target(void)
{
  _ForceQuitActFrame();
}

//-----------------------------------------------------
//タイマー
//-----------------------------------------------------
void BtlvUi::SetTimerTime( int game_time, int client_time, int command_time )
{
  m_gaugeTimer->SetTime( game_time, client_time, command_time );
}

void BtlvUi::DispTimer( BTLV_TIMER_TYPE type , bool isReset, bool isCountDownStart )
{
  m_gaugeTimer->DispTimer( type, isReset, isCountDownStart );
}

void BtlvUi::HideTimer( BTLV_TIMER_TYPE type )
{
  m_gaugeTimer->HideTimer( type );
}

bool BtlvUi::IsFinishTimer( BTLV_TIMER_TYPE type )
{
  return( m_gaugeTimer->IsFinishTimer( type ) );
}

u32 BtlvUi::GetClientTime( u8 clientID )
{
  return( m_gaugeTimer->GetClientTime( clientID ) );
}

void BtlvUi::SetClientTime( u8 clientID, u32 time )
{
  m_gaugeTimer->SetClientTime( clientID, time );
}


//-----------------------------------------------------
//特殊
//-----------------------------------------------------
void BtlvUi::StartNaderuCheck(void)
{
}

bool BtlvUi::EndNaderuCheck(void)
{
  return false;
}

bool BtlvUi::IsNaderuCheck(void)
{
  return false;
}

void BtlvUi::UpdateNaderuCheck(void)
{

}


//-----------------------------------------------------
//ポケモンリスト
//-----------------------------------------------------

/**
 * @brief リスト画面を開始する
 * @param param
 * @param canEsc
 */
void BtlvUi::StartAnime_PokeList( const PokeSelParam* param, bool canEsc, const BTL_POKEPARAM* outMemberPokeParam, u8 poke_index, bool* sashiosae )
{
  GFL_UNUSED( param );

  //  交代対象poke_indexを設定
  //  ポケモン入れ替えができないのは「canEsc」で判別できる
  m_pokeListProc.StartBySelectChange( poke_index, outMemberPokeParam, canEsc );
}


/**
 * @brief リスト画面を更新する
 * @param select_result  選択結果の格納先
 * @retval BCR_SEL_NONE                  リスト画面が継続中
 * @retval BCR_SEL_CANCEL                何も決定せずに戻った
 * @retval BCR_SEL_ITEM                  使用する道具を選択した
 * @retval BCR_SEL_IDX1 〜 BCR_SEL_IDX6  入れ替えるポケモンを選択した
 */
BTLV_COMMAND_RESULT BtlvUi::WaitAnime_PokeList( PokeSelResult* select_result )
{
  BTLV_COMMAND_RESULT commandRet  = m_pokeListProc.Update( select_result );
  if( commandRet != BCR_SEL_NONE )
  {
    //  @fix GFNMCat[3207] 時間制限がない場合は必ずここを通る
    m_uiPokeListFrameContextParam.friend_party.DeleteBattleData();
    m_uiPokeListFrameContextParam.my_party.DeleteBattleData();
  }

  return  commandRet;
}


/**
 * @brief リスト画面を終了する
 */
void BtlvUi::EndAnime_PokeList(void)
{
  //  ロジック側から呼ばれない

  m_pokeListProc.End();
}

/**
 * @brief リスト画面を強制終了する
 */
void BtlvUi::ForceQuit_PokeList(void)
{
  _ForceQuitActFrame();
}

void BtlvUi::StartAnime_WazaInfoView(u8 poke_index, u8 waza_index)
{
}


//-----------------------------------------------------
//ポケモン選択( ポケモン気絶やとんば返りなどで入れ替えが発生した時に呼ばれる )
//-----------------------------------------------------
void BtlvUi::StartAnime_SelectPoke(const PokeSelParam* param, bool canEsc, int outMemberIndex, PokeSelResult* pResult )
{
  //  気絶時の交代対象poke_indexを設定
  m_pokeListProc.StartByForceChange( outMemberIndex, canEsc, true, pResult );
}

//-----------------------------------------------------
//ポケモン選択更新
//-----------------------------------------------------
BTLV_COMMAND_RESULT BtlvUi::WaitAnime_SelectPoke( PokeSelResult* result )
{
  BTLV_COMMAND_RESULT commandRet  = m_pokeListProc.Update( result );
  if( commandRet != BCR_SEL_NONE )
  {
    //  @fix GFNMCat[3207] 時間制限がない場合は必ずここを通る
    m_uiPokeListFrameContextParam.friend_party.DeleteBattleData();
    m_uiPokeListFrameContextParam.my_party.DeleteBattleData();

    // @fix NMCat[3649] アイテムや技による途中入れ替えの場合に暗転が残るケースがあるので、ポケモン入れ替え終了で暗転を消す
    if( ( m_pUIBGFrame != NULL ) && ( m_pUIBGFrame->IsOteireDisp() ) )
    {
      SetEnableBGFilter( false );
    }

    /*  この後は
        ・行動選択開始
        ・通信待機中(通信対戦限定)
        ・戦闘終了待機(表示は背景のみ)

        これらのフローに遷移
    */
  }

  return  commandRet;

}

void BtlvUi::EndAnime_SelectPoke(void)
{
  //  ロジック側から呼ばれない

  m_pokeListProc.End();
}

/**
 * @brief ポケモン選択画面を強制終了
 */
void BtlvUi::ForceQuit_SelectPoke(void)
{
  _ForceQuitActFrame();
}





//-----------------------------------------------------
// バッグ
//-----------------------------------------------------
/**
 * @brief バッグ画面を開始する
 */
void BtlvUi::StartAnime_Bag( const ItemBallUseStatus ballusemode, bool* sashiosae )
{
  GFL_UNUSED( sashiosae );

  m_itemProc.Start( ballusemode, m_pUIActSelectFrame );
}

/**
 * @brief バッグ画面の終了を待つ
 * @retval BCR_SEL_NONE    終了していない
 * @retval BCR_SEL_CANCEL  キャンセルした
 * @retval BCR_SEL_DECIDE  使用する道具を決定した
 */
BTLV_COMMAND_RESULT BtlvUi::WaitAnime_Bag(void)
{
  BTLV_COMMAND_RESULT result  = m_itemProc.Update();

  //  道具を決定したのでアイテム結果を更新
  if( result != BCR_SEL_NONE )
  {
    m_itemResultData  = m_itemProc.GetResult();
  }

  return  result;
}


/**
 * @brief バッグ画面を終了する
 */
void BtlvUi::EndAnime_Bag(void)
{
  m_itemProc.End();
}

/**
 * @brief バッグ画面を強制終了
 */
void BtlvUi::ForceQuit_Bag(void)
{
  _ForceQuitActFrame();
}



//-----------------------------------------------------
//下画面２択
//-----------------------------------------------------
/**
 * @brief 二択画面を開始する
 * @param yes_msg
 * @param no_msg
 * @param b_cancel
 * @param yesno_mode  二択の表示モード
 */
void BtlvUi::StartAnime_SelectYesNo( bool b_cancel, YesNoMode yesno_mode )
{
  //  トップ画面の手前にに暗転フィルター表示設定
  SetEnableBGFilter( true );

  //  FrameをNewして登録
  BattleUIYesNoFrame::APP_PARAM::_tag_in_param* pInParam  = &m_uiYesNoFrameAppParam.in_param;
  {
    m_pUIYesNoFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIYesNoFrame>( m_pUILowerFrameManager );
    pInParam->p_async_file_manager      = m_btlvCore->GetAsyncFileManager();
    pInParam->p_rendering_manager       = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
    pInParam->p_parent_app_heap         = m_appTempHeap.GetSystemHeap();
    pInParam->p_parent_dev_heap         = m_appTempHeap.GetDeviceHeap();
    pInParam->p_menu_cursor_lyt_buffer  = m_pBtlParamAccesser->GetMenuCusrorLytResourceBuffer();
    pInParam->b_use_input_se            = m_pBtlParamAccesser->IsPlayInputSE();
    pInParam->p_tmp_dev_heap            = m_appTempHeap.GetDeviceHeap();
    pInParam->b_user_input              = m_btlvCore->IsControllable();
  }

  //  ２選択が表示する文字を設定
  {
    pInParam->p_btl_app_msg_data  = m_pBtlParamAccesser->GetAppMesssageData();
    pInParam->p_btl_word_set      = m_pBtlParamAccesser->GetWordSet();
    pInParam->first_select_type   = BattleUIYesNoFrame::SELECT_TYPE_YES;

    //「はい」                「いいえ」
    if( yesno_mode == YESNO_MODE_YESNO )
    {
      //  にげる時に呼ばれる
      {
        if( m_pBtlParamAccesser->GetBattleCompetitor() == BTL_COMPETITOR_COMM )
        {
          //  通信対戦の時は初期位置をいいえに
          pInParam->first_select_type   = BattleUIYesNoFrame::SELECT_TYPE_NO;
        }
      }

      pInParam->b_use_b_button  = true;
      pInParam->a_select_item_text_id_list[ 0 ] = str_btl_yesno_00_01;
      pInParam->a_select_item_text_id_list[ 1 ] = str_btl_yesno_00_02;
    }
    //「次のポケモンを使う」  「にげる」
    else
    if( yesno_mode == YESNO_MODE_NEXT_POKE )
    {
      pInParam->a_select_item_text_id_list[ 0 ] = str_btl_yesno_02_01;
      pInParam->a_select_item_text_id_list[ 1 ] = str_btl_yesno_02_02;

      //  Bボタンを使えない
      pInParam->b_use_b_button  = false;
    }
    //「ポケモンを入れ替える」「そのまま使う」
    else if( yesno_mode == YESNO_MODE_POKE_CHANGE ) 
    {
      pInParam->a_select_item_text_id_list[ 0 ] = str_btl_yesno_01_01;
      pInParam->a_select_item_text_id_list[ 1 ] = str_btl_yesno_01_02;

      pInParam->b_use_b_button  = true;
    }
    //「わすれさせる」        「わすれさせない」
    else if( yesno_mode == YESNO_MODE_FORGET )
    {
      pInParam->a_select_item_text_id_list[ 0 ] = str_btl_yesno_03_01;
      pInParam->a_select_item_text_id_list[ 1 ] = str_btl_yesno_03_02;

      // @fix ディレクターさん要望　技忘れの選択にＢキャンセルは使えないようにする
      pInParam->b_use_b_button  = false;
    }
    else
    {
      pInParam->a_select_item_text_id_list[ 0 ] = str_btl_yesno_00_01;
      pInParam->a_select_item_text_id_list[ 1 ] = str_btl_yesno_00_02;

      pInParam->b_use_b_button  = true;
    }
  }

  //  フレーム内部で画面が自動で開き、選択後に自動で閉じてメモリも開放される
  m_pUIYesNoFrame->Initialize( &m_uiYesNoFrameAppParam );
}



BTLV_COMMAND_RESULT BtlvUi::WaitAnime_SelectYesNo( void )
{
  //  画面挙動待ち
  if( m_uiYesNoFrameAppParam.out_param.bEnd )
  {
    // @fix NMCat[3649] おていれ表示中はお手入れ入力を可能にするために、暗転を表示から非表示に
    if( ( m_pUIBGFrame != NULL ) && ( m_pUIBGFrame->IsOteireDisp() ) )
    {
      SetEnableBGFilter( false );
    }

    if( m_uiYesNoFrameAppParam.out_param.resultSelectItemIdx == BattleUIYesNoFrame::SELECT_TYPE_YES )
    {
      return BCR_SEL_YES;
    }
    else
    if( m_uiYesNoFrameAppParam.out_param.resultSelectItemIdx == BattleUIYesNoFrame::SELECT_TYPE_NO )
    {
      return BCR_SEL_NO;
    }

    /*  この後は
        ・行動選択再開
        ・ポケモンリスト
        ・技忘れ
        ・通信待機中(通信対戦限定)
        ・戦闘終了待機(表示は背景のみ)

        これらのフローに遷移
    */
  }

  return BCR_SEL_NONE;
}


void BtlvUi::ForceQuit_SelectYestNo( void )
{
  _ForceQuitActFrame();
}

void BtlvUi::EndAnime_SelectYesNo( void )
{
  //  時間制限が切れたときのみ呼ばれる(意図不明)
}



//レベルアップ
void BtlvUi::Start_LevelUp( const BTL_POKEPARAM* bpp, const BTL_LEVELUP_INFO* lvupInfo )
{
  m_gaugeLevelUp->SetPokeParam( bpp );
  m_gaugeLevelUp->SetLevelUpInfo( lvupInfo );
  m_gaugeLevelUp->StartLevelUp();
}

bool BtlvUi::IsDispLevelUp(void)
{
  return (m_gaugeLevelUp->IsAnimeEnd());
}

void BtlvUi::Step_LevelUp( void )
{
  m_gaugeLevelUp->StartParamAdd();
}


void BtlvUi::End_LevelUp( void )
{
  m_gaugeLevelUp->HidePane();
}

//-----------------------------------------------------
//パーツ制御
//-----------------------------------------------------

//6末用
void BtlvUi::AllInGauge(void)
{
  bool isIn = false;
  isIn |= m_gaugeStatus[BTL_VPOS_NEAR_1]->StartGaugeInAnime();
  isIn |= m_gaugeStatus[BTL_VPOS_FAR_1]->StartGaugeInAnime();

  if( m_btlvCore->GetBattleRule() == BTL_RULE_DOUBLE )
  {
    isIn |= m_gaugeStatus[BTL_VPOS_NEAR_2]->StartGaugeInAnime();
    isIn |= m_gaugeStatus[BTL_VPOS_FAR_2]->StartGaugeInAnime();
  }
  else
  if( m_btlvCore->GetBattleRule() == BTL_RULE_INTRUDE )
  {
    isIn |= m_gaugeStatus[BTL_VPOS_FAR_2]->StartGaugeInAnime();
  }
  else
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    isIn |= m_gaugeStatus[BTL_VPOS_NEAR_2]->StartGaugeInAnime();
    isIn |= m_gaugeStatus[BTL_VPOS_FAR_2]->StartGaugeInAnime();
  }
  if( isIn )
  {
    m_btlvCore->PlaySe(SEQ_SE_SYS_NJ_011);
  }
}

bool BtlvUi::WaitAllInGauge(void)
{
  if( !m_gaugeStatus[BTL_VPOS_NEAR_1]->WaitGaugeInAnime() ||
      !m_gaugeStatus[BTL_VPOS_FAR_1]->WaitGaugeInAnime() )
  {
    return false;
  }
  if( m_btlvCore->GetBattleRule() == BTL_RULE_DOUBLE )
  {
    if( !m_gaugeStatus[BTL_VPOS_NEAR_2]->WaitGaugeInAnime() ||
        !m_gaugeStatus[BTL_VPOS_FAR_2]->WaitGaugeInAnime() )
    {
      return false;
    }
  }
  if( m_btlvCore->GetBattleRule() == BTL_RULE_INTRUDE )
  {
    if( !m_gaugeStatus[BTL_VPOS_FAR_2]->WaitGaugeInAnime() )
    {
      return false;
    }
  }
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    if( !m_gaugeStatus[BTL_VPOS_NEAR_2]->WaitGaugeInAnime() ||
        !m_gaugeStatus[BTL_VPOS_FAR_2]->WaitGaugeInAnime() )
    {
      return false;
    }
  }

  return true;
}

//ゲージ全体位置の制御
void BtlvUi::SetGaugePosStart(void)
{
}

void BtlvUi::SetGaugePosDispSelf(void)
{
}

void BtlvUi::SetGaugePosDispEnemy(void)
{
}

void BtlvUi::SetGaugePosBaseSelf(void)
{
}

void BtlvUi::SetGaugePosBaseEnemy(void)
{
}


void BtlvUi::DispGauge( BtlvPos viewPos , bool isUpdate )
{
  //@attention
  //BTS6816 捕獲後に敵側のゲージを出さないためのフラグ
  //  if( mBtlvCore->IsAfterCapture() == false || 
  //      BtlvPosIsEnemySide(viewPos) == false )
  {
    if( m_gaugeStatus[viewPos] != NULL )
    {
      m_gaugeStatus[viewPos]->DispGauge();
      if( isUpdate == true )
      {
        m_gaugeStatus[viewPos]->UpdateForce();
      }
    }
  }
}

void BtlvUi::HideGauge( BtlvPos viewPos )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->HideGauge();
  }
}

void BtlvUi::DispGaugeAll(void)
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      m_gaugeStatus[i]->DispGauge();
    }
  }
}

void BtlvUi::HideGaugeAll(void)
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      m_gaugeStatus[i]->HideGauge();
    }
  }
}

bool BtlvUi::IsDispGauge( BtlvPos viewPos )
{
  return false;
}

//メッセージウィンドウの表示と合わせて制御するよう
void BtlvUi::SetDispGauge_MsgWin(bool flg )
{
}

void BtlvUi::StrtBallBarAnime(BtlvPos viewPos )
{
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    if( m_gaugeBall[viewPos] )
    {
      m_gaugeBall[viewPos]->StartDispAnime();
    }
  }
  else
  {
    if( m_gaugeBall[viewPos%2] )
    {
      m_gaugeBall[viewPos%2]->StartDispAnime();
    }
  }
}

void BtlvUi::DispBallBarAll(void)
{
  for( int i=0;i<4;i++ )
  {
    if( m_gaugeBall[i] )
    {
      m_gaugeBall[i]->DispBar();
    }
  }

  // ボールゲージと連動表示
  DispShotDownBarAll();
}

void BtlvUi::HideBallBarAll(void)
{
  for( int i=0;i<4;i++ )
  {
    if( m_gaugeBall[i] )
    {
      m_gaugeBall[i]->HideBar();
    }
  }

  // ボールゲージと連動表示
  HideShotDownBarAll();
}

void BtlvUi::UpdateBallBarAll(void)
{
  for( int i=0;i<4;i++ )
  {
    if( m_gaugeBall[i] )
    {
      m_gaugeBall[i]->UpdateBall();
    }
  }
}

// ロイヤル用撃墜数表示
void BtlvUi::DispShotDownBarAll(void)
{
  // バトルロイヤル専用の表示
  if( m_btlvCore->GetBattleRule() != BTL_RULE_ROYAL )
    {
      return;
    }

  // 撃墜数のセット
  for( int i=0;i<4;i++ )
  {
    u16 kill_count;

    if( i == 0 )
      {
        kill_count = m_btlvCore->GetBattleParty_Self()->GetTotalKillCount();
      }
    else
      {
        // 敵は自分を基準に反時計回り
        kill_count = m_btlvCore->GetBattleParty_Enemy(i-1)->GetTotalKillCount();
      }

    m_gaugeShotDown[i]->SetShotDownCount( kill_count );
 
  }


  for( int i=0;i<4;i++ )
  {
    if ( m_gaugeShotDown[i] )
      {
        m_gaugeShotDown[i]->DispBar();
      }
  }
}
void BtlvUi::HideShotDownBarAll(void)
{
  // バトルロイヤル専用の表示
  if( m_btlvCore->GetBattleRule() != BTL_RULE_ROYAL )
    {
      return;
    }

  for( int i=0;i<4;i++ )
  {
    if ( m_gaugeShotDown[i] )
      {
        m_gaugeShotDown[i]->HideBar();
      }
  }
}

//ゲージ強制更新
void BtlvUi::GaugeUpdateReq( void )
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      m_gaugeStatus[i]->UpdateReq();
    }
  }
}

void BtlvUi::GaugeUpdateForce( void )
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      m_gaugeStatus[i]->UpdateForce();
    }
  }
}

void BtlvUi::GaugeUpdate_Anime( void )
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      m_gaugeStatus[i]->StartDamageAnime(BtlvGaugeBase::GAUGE_ANIME_DEF_FRAME);
    }
  }
}


void BtlvUi::ResetGaugeParam( BtlvPos viewPos  )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->SetPokeParam( m_btlvCore->GetBattlePokeParam(viewPos) );
  }
}

void BtlvUi::StartGaugeDamage( BtlvPos viewPos , int cnt )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->StartDamageAnime(cnt);
  }
}

bool BtlvUi::IsGaugeDamage(void)
{
  for( int i=0;i<BTL_EXIST_POS_MAX;i++ )
  {
    if( m_gaugeStatus[i] != NULL )
    {
      if( m_gaugeStatus[i]->IsDamageAnime() )
      {
        return true;
      }
    }
  }
  return false;
}

void BtlvUi::PlayBatugunAnime( BtlvPos viewPos )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->PlayBatugunAnime();
  }
}

void BtlvUi::GaugeExpAnime( BtlvPos viewPos , u32 value )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->ExpAnime(value);
  }
}

void BtlvUi::GaugeExpAnime_Lvup( BtlvPos viewPos )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
    m_gaugeStatus[viewPos]->ExpAnime_Lvup();
  }
}

void BtlvUi::GaugeSetSick( BtlvPos viewPos , PokeSick sick)
{
}

bool BtlvUi::IsAnimeGauge(void)
{
  for(int i=0; i<BTL_EXIST_POS_MAX; i++)
    {
      if( m_gaugeStatus[i] != NULL )
	{
	  if ( m_gaugeStatus[i]->IsAnime() )
	    {
	      return true;
	    }
	}
    }

  return false;
}

bool BtlvUi::IsPinch( BtlvPos viewPos )
{
  if( m_gaugeStatus[viewPos] != NULL )
  {
     return m_gaugeStatus[viewPos]->IsPinch();
  }
  return false;
}

//特性Win
void BtlvUi::DispTokWin( const BtlvPos viewPos )
{
  const BtlvGaugeTokusei::COMBAT_SIDE side = (BtlvPosIsEnemySide(viewPos)?(BtlvGaugeTokusei::SIDE_ENEMY) : (BtlvGaugeTokusei::SIDE_ALLY));
  m_gaugeTokusei->StartDispAnime( side, viewPos ,m_btlvCore->GetBattlePokeParam(viewPos) );
}

bool BtlvUi::WaitDispTokWin(void)
{
  return (m_gaugeTokusei->WaitDispAnime());
}

void BtlvUi::HideTokWin( const BtlvPos viewPos )
{
  const BtlvGaugeTokusei::COMBAT_SIDE side = (BtlvPosIsEnemySide(viewPos)?(BtlvGaugeTokusei::SIDE_ENEMY) : (BtlvGaugeTokusei::SIDE_ALLY));
  m_gaugeTokusei->StartHideAnime( side );
}

bool BtlvUi::WaitHideTokWin(void)
{
  return (m_gaugeTokusei->WaitHideAnime());
}

bool BtlvUi::IsDispTokWin( const BtlvPos viewPos )
{
  const BtlvGaugeTokusei::COMBAT_SIDE side = (BtlvPosIsEnemySide(viewPos)?(BtlvGaugeTokusei::SIDE_ENEMY) : (BtlvGaugeTokusei::SIDE_ALLY));
  return m_gaugeTokusei->IsDisp( side );
}

BtlvPos BtlvUi::GetTokWinDispPos(const BtlvPos viewPos)
{
  const BtlvGaugeTokusei::COMBAT_SIDE side = (BtlvPosIsEnemySide(viewPos)?(BtlvGaugeTokusei::SIDE_ENEMY) : (BtlvGaugeTokusei::SIDE_ALLY));
  return m_gaugeTokusei->GetDispPos( side );
}


//-----------------------------------------------------------------------------
//Message
//-----------------------------------------------------------------------------

// 技説明取得
gfl2::str::MsgData * BtlvUi::GetWazaInfoMessage(void)
{
  return m_pBtlParamAccesser->GetWazaInfoMesssageData();
}

void BtlvUi::StartDispMessage( const BTLV_STRPARAM *param, bool isKeyWait )
{
  StrParamToString( param , m_workStr[0] );

  if( param->strID == BTL_STRID_STD_SelectAction ||
      param->strID == BTL_STRID_STD_SelectRotation )
  {
    m_pUiMsgWin->DispMessage( *m_workStr[0] , true, isKeyWait, param->wait, false );
  }
  else if( param->strID == BTL_STRID_STD_GetMoney )
  {
    // Finish待ちを即時終了にする
    m_pUiMsgWin->DispMessage( *m_workStr[0] , false, isKeyWait, param->wait, true );
  }
  else
  {
    m_pUiMsgWin->DispMessage( *m_workStr[0] , false, isKeyWait, param->wait, false );
  }
}

void BtlvUi::StartDispMessage( const gfl2::str::StrBuf *str, bool isKeyWait )
{
  m_pUiMsgWin->DispMessage(*str, false, isKeyWait,false, false);
}

void BtlvUi::StartDispMessageEx( const gfl2::str::StrBuf *str )
{
  m_pUiMsgWin->DispMessage(*str, false, true,false, false);
}

void BtlvUi::StartDispMessageDummy( wchar_t *msg )
{
}

void BtlvUi::SetMessageCallBack( BTL_CLIENT* callbackTarget )
{
  m_pUiMsgWin->SetMessageCallBack(callbackTarget);
}

//メッセージ自動送り　StartDispMessageの次に呼ぶ！
void BtlvUi::SetAutoStepMessage( bool flg )
{
  m_pUiMsgWin->SetAutoStepMessage(flg);
}

//今表示しているメッセージだけに有効なメッセージ速度変更
//トレーナーの繰り出しメッセージの時に、遅いの時だけ普通にする。
void BtlvUi::SetMessageSpeedForce_Normal(void)
{
  m_pUiMsgWin->SetMessageSpeedForce_Normal();
}

// メッセージカーソルの表示・非表示
void BtlvUi::SetVisibleMsgCursor( bool isVisible )
{
  m_pUiMsgWin->SetVisibleMsgCursor(isVisible);
}


void BtlvUi::HideMessage(void)
{
  m_pUiMsgWin->HideMessage();
}

bool BtlvUi::IsUpdateMessage(void)
{
  return m_pUiMsgWin->IsUpdateMessage();
}

bool BtlvUi::IsVisibleMessage(void)const
{
  return m_pUiMsgWin->IsVisible();
}

bool BtlvUi::IsOneLineMessage(void)const
{
  return m_pUiMsgWin->IsOneLine();
}


void BtlvUi::UpdateMessage(void)
{
  m_pUiMsgWin->UpdateTree();
}

void BtlvUi::DrawMessage(gfl2::gfx::CtrDisplayNo displayNo)
{
	  m_pUiMsgWin->Draw(displayNo);
}




//----------------------------------------------------------------------------------
/**
 * STRPARAMの展開(移植)
 *
 * @param   param
 * @param   dst
 */
//----------------------------------------------------------------------------------
void BtlvUi::StrParamToString( const BTLV_STRPARAM* param, gfl2::str::StrBuf* dst )
{
  if( m_btlvCore->IsViewerMode() )
  {
    dst->SetStr(L"Dummy\n");
    return;
  }

  switch( param->strType ){
  case BTL_STRTYPE_STD:
    str::MakeStringStdWithArgArray( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_SET:
    str::MakeStringSet( dst, param->strID, param->args );
    break;
  case BTL_STRTYPE_UI:
    str::GetUIString( dst, param->strID );
    break;
    /*
  case BTL_STRTYPE_YESNO:
    str::MakeStringYesNoWithArgArray( dst, param->strID, param->args );
    break;
    */
  case BTL_STRTYPE_WAZA:
    str::MakeStringWaza( dst, param->args[0], param->args[1] );
    break;
  default:
    GFL_ASSERT(0);
    return;
  }
}

//  カーソルリセット
void BtlvUi::ResetCursorPos( const u32 pokeIdx )
{
  GFL_ASSERT( m_pBtlParamAccesser );
  m_pBtlParamAccesser->ResetPokeCursorMemory( pokeIdx );
}

//敵側かどうか？
bool BtlvUi::BtlvPosIsEnemySide( BtlvPos viewPos )
{

  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    // バトルロイヤルは自分以外すべて敵
    return (viewPos == 0 ? false : true);
  }
  else
  {
    // バトルロイヤル以外は奇数番号が敵
    return ( viewPos&1 ? true : false  );
  }

}

void BtlvUi::ForceSelect_Attack()
{
}

//  モンスターボールアイテムリストを開く(捕獲デモ用)
void BtlvUi::ForceSelect_Bag()
{
  if( GetStateBagFrame() != btl::BtlvUi::UI_FRAME_STAET_IDLE )
  {
    return;
  }

  GFL_ASSERT( m_pUIBagFrame );
  m_pUIBagFrame->InitDemoMode();
  m_pUIBagFrame->SetDemoModeRequest( ::app::bag::BagFrame::REQ_SELECT_BALL_POCKET );
}

//  バッグのボールカテゴリー選択( 捕獲デモ用 )
void BtlvUi::ForceSelect_BallCategory()
{
  if( GetStateBagFrame() != btl::BtlvUi::UI_FRAME_STAET_IDLE )
  {
    return;
  }

  GFL_ASSERT( m_pUIBagFrame );
  m_pUIBagFrame->InitDemoMode();
  m_pUIBagFrame->SetDemoModeRequest( ::app::bag::BagFrame::REQ_SELECT_ITEM_BALL );
}

void BtlvUi::ForceSelect_Skill()
{
}

void BtlvUi::ForceSelect_ItemTopLeft()
{
}

//  アイテム使用確認画面の使うボタンを押す( 捕獲デモ用 )
void BtlvUi::ForceSelect_ItemUse()
{
  if( GetStateBagFrame() != btl::BtlvUi::UI_FRAME_STAET_IDLE )
  {
    return;
  }

  GFL_ASSERT( m_pUIItemConfirmFrame );
  m_pUIItemConfirmFrame->PushCmd( BattleUIItemConfirmFrame::CMD_TYPE_PUSH_BUTTON_ITEM_USE );
}



void BtlvUi::SetPassiveAnime( u32 anm )
{
}

void BtlvUi::DispWaitCommStr( bool flg)
{
  if( m_pUIBGFrame == NULL )
  {
    return;
  }

  //  通信待機中の表示を出すか
  if( flg == true )
  {
    //  通信待機中の表示にフェードで隠れてみえなくなるのを防ぐため
    m_bLowerFadeReset  = true;

    //  トップ画面の手前に暗転フィルター表示設定
    SetEnableBGFilter( true );

    // @fix GFNMCat[5269] トップ画面の入力をＯＦＦにする（他の画面入力はＯＦＦにする必要はない。破棄コマンドがすでに呼ばれているので）
    if( m_pUIActSelectFrame != NULL )
    {
      m_pUIActSelectFrame->SetEnableInput( false );
    }

    m_pUIBGFrame->ShowWaitComMesssageWindow();
  }
  else
  {
    m_pUIBGFrame->HideWaitComMesssageWindow();
  }
}

//-----------------------------------------------------------------------------
//おていれ
//-----------------------------------------------------------------------------
void BtlvUi::DispOteire()
{
  //  おていれボタンを表示
  GFL_ASSERT( m_pUIBGFrame != NULL );

  // @fix NMCat[3649] お手入れ表示する時はトップ画面手前の暗転フィルター非表示
  SetEnableBGFilter( false );

  m_pUIBGFrame->EventOteireDisp();
}

//-----------------------------------------------------
//ロトム演出
//-----------------------------------------------------
void BtlvUi::Rotom_StartLoad(void)
{
  m_pUiRotom->StartLoad();
}
bool BtlvUi::Rotom_IsFinishLoad(void)
{
  return m_pUiRotom->IsFinishLoad();
}

void BtlvUi::Rotom_RemoveLayout(void)
{
  m_pUiRotom->RemoveLayout();
}
void BtlvUi::Rotom_Delete(void)
{
  m_pUiRotom->Delete();
}

void BtlvUi::Rotom_StartAnimation(int type)
{
  m_pUiRotom->StartAnimation(type);
}

void BtlvUi::Rotom_SetData(const BTL_POKEPARAM *bpp)
{
  m_pUiRotom->SetData(bpp);
}

//-----------------------------------------------------
//バトル開始導入イベント
//-----------------------------------------------------
//------------------------------------------------------------------
/**
  * @brief   導入イベント開始
  */
//------------------------------------------------------------------
void BtlvUi::StartFirstIntroEvent()
{
  //  フェードアウトしてここにくるケースがあるのでフェードアウトをリセット
  {
    gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
    pFadeManager->Reset( gfl2::Fade::DISP_LOWER );
  }
#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  if( m_pUIIntroEventFrame != NULL )
  {
#if 0
  //  上画面のゲージ演出途中で下画面のイントロ演出を出す
  //  開始から数フレーム間を空ける
  static const u32 s_introStartDelay  = 5;

  //  導入演出は終了したら自動でフレームが破棄する
  //  もう使わないのでメモリがもったいないから
  m_pUIIntroEventFrame->Start( s_introStartDelay );
#endif
  }
#endif
}

//------------------------------------------------------------------
/**
  * @brief   導入イベント待ち
  * @return  true -> 待ち継続 / false -> 待ち終了
  */
//------------------------------------------------------------------
bool BtlvUi::WaitFirstIntroEvent()
{
#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  bool bRet = false;
  if( m_pUIIntroEventFrame != NULL )
  {
    bRet  = true;
  }

  return  bRet;
#else
  return  false;
#endif
}

//-----------------------------------------------------
//大会演出
//-----------------------------------------------------
void BtlvUi::StartConventionDirection( void )
{
  if( m_conventionWin )
    {
      m_conventionWin->Start();
    }
}
void BtlvUi::EndConventionDirection( void )
{

  if( m_conventionWin )
    {
      m_conventionWin->Break();
    }
}

//-----------------------------------------------------
//ロイヤル用順位表示
//-----------------------------------------------------
void BtlvUi::StartDispRoyalRanking(void)
{
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    //ペインの表示
    {
      gfl2::lyt::LytParts *partsuPane;
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_RANK_00 );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,true);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_RANK_01 );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,true);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_RANK_02 );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,true);
      partsuPane = m_pUiMsgWin->GetLayoutWork(0)->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_ROYAL_RANK_03 );
      m_pUiMsgWin->GetG2DUtil()->SetPaneVisible(partsuPane,true);
    }
    static const u32 rankArr[4][4] =
    {
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_1ST_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_2ND_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_3RD_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_4TH_IN,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_1ST_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_2ND_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_3RD_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_4TH_IN,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_1ST_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_2ND_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_3RD_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_4TH_IN,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_1ST_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_2ND_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_3RD_IN,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_4TH_IN,
      },
    };

    u8 royalRank[4];

    royalRank[0] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetClientID() );
    royalRank[1] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(0) );
    royalRank[2] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(1) );
    royalRank[3] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(2) );

    for( int i=0;i<4;i++ )
    {
      //計算とかテーブルとかメンドイので確保しておく
      m_rolayRankingAnime[i] = rankArr[i][royalRank[i]];
      mGaugeG2dUtil->StartAnime( 0,m_rolayRankingAnime[i] );
    }

    // グローブの表示
    for( int i=0;i<4;i++ )
    {
      m_gaugeStatus[i]->DispGlove();
    }

  }
}

bool BtlvUi::IsFinishDispRoyalRanking(void)
{
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    for( int i=0;i<4;i++ )
    {
      if( mGaugeG2dUtil->IsAnimeEnd( 0,m_rolayRankingAnime[i] ) == false )
      {
        return false;
      }
    }
  }
  return true;
}

void BtlvUi::StartHideRoyalRanking(void)
{
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {


    //アニメーションのテーブル
    static const u32 rankArr[4][4] =
    {
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_1ST_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_2ND_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_3RD_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_01_4TH_OUT,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_1ST_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_2ND_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_3RD_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_02_4TH_OUT,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_1ST_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_2ND_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_3RD_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_03_4TH_OUT,
      },
      {
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_1ST_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_2ND_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_3RD_OUT,
        LA_MINUPP_BATTLE_MIN_UPP_000__ROYAL_RANK_00_4TH_OUT,
      },
    };

    u8 royalRank[4];

    royalRank[0] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetClientID() );
    royalRank[1] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(0) );
    royalRank[2] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(1) );
    royalRank[3] = m_btlvCore->GetBattleRoyalRanking( m_btlvCore->GetEnemyClientID(2) );

    for( int i=0;i<4;i++ )
    {
      //計算とかテーブルとかメンドイので確保しておく
      m_rolayRankingAnime[i] = rankArr[i][royalRank[i]];
      mGaugeG2dUtil->StartAnime( 0,m_rolayRankingAnime[i] );
    }

    // グローブの非表示
    for( int i=0;i<4;i++ )
    {
      m_gaugeStatus[i]->HideGlove();
    }

  }
}

bool BtlvUi::IsFinishHideRoyalRanking(void)
{
  if( m_btlvCore->GetBattleRule() == BTL_RULE_ROYAL )
  {
    for( int i=0;i<4;i++ )
    {
      if( mGaugeG2dUtil->IsAnimeEnd( 0,m_rolayRankingAnime[i] ) == false )
      {
        return false;
      }
    }
  }
  return true;
}

//  行動選択フレーム作成
void BtlvUi::_CreateSelectActFrame( const bool bActReturnFlg )
{
  //  行動選択画面のフレーム登録
  if( m_pUIActSelectFrame == NULL )
  {
    m_pUIActSelectFrame = MENU_FRAME_CALL_MODELESS<applib::frame::Manager, btl::BattleUIActSelectFrame>( m_pUILowerFrameManager );
    {
      ::app::util::Heap*  pUtilHeap = &m_appTempHeap;

      BattleUIActSelectFrame::APP_PARAM::_tag_in_param* pInParam  = &m_uiActSelectFrameAppParam.in_param;

      pInParam->pAsyncFileManager  = m_btlvCore->GetAsyncFileManager();
      pInParam->pRenderingManager  = m_btlvCore->GetRenderSys()->GetAppRenderingManager();
      pInParam->pParentAppHeap     = pUtilHeap->GetSystemHeap();
      pInParam->pParentDevHeap     = pUtilHeap->GetDeviceHeap();
      pInParam->bAutoDelete        = false;
      pInParam->bActReturn         = bActReturnFlg;
      pInParam->pBtlParamAccessror = m_pBtlParamAccesser;
      pInParam->pTmpDevHeap        = pUtilHeap->GetDeviceHeap();
      
      //  初回カーソル設定
      {
        m_uiActSelectFrameAppParam.config_param.selectCursorType  = (app::BattleUIActSelectLowerDraw::CursorType)m_pBtlParamAccesser->GetTopActViewCursorMemory();
      }

      m_pUIActSelectFrame->Initialize( &m_uiActSelectFrameAppParam );
    }
  }
}

//------------------------------------------------------------------
/**
  * @brief	  FrameMangerに登録して実行中のFrameのEndFuncが完了した直後に呼び出される関数
  */
//------------------------------------------------------------------
void BtlvUi::EndFuncFinish( applib::frame::CellSuper* pEndCell )
{
  //  EndFuncが呼ばれた段階で登録しているFraemはメモリ開放している
  if( m_pUIActSelectFrame == pEndCell )
  {
    m_pUIActSelectFrame  = NULL;
  }
  else
  if( m_pUISkillSelectFrame == pEndCell )
  {
    m_pUISkillSelectFrame  = NULL;
  }
  else
  if( m_pUIYesNoFrame == pEndCell )
  {
    m_pUIYesNoFrame  = NULL;
  }
  else
  if( m_pUIBGFrame == pEndCell )
  {
    m_pUIBGFrame = NULL;
    m_uiActSelectFrameAppParam.in_param.pBGFrame  = NULL;
  }
  else
  if( m_pUITargetSelectFrame == pEndCell )
  {
    m_pUITargetSelectFrame  = NULL;
  }
  else
  if( m_pUIPokeListFrame == pEndCell )
  {
    m_pUIPokeListFrame  = NULL;
  }
  else
  if( m_pUIBattlePokeListFrame == pEndCell )
  {
    m_pUIBattlePokeListFrame  = NULL;
  }
  else
  if( m_pUIItemConfirmFrame == pEndCell )
  {
    m_pUIItemConfirmFrame = NULL;
  }
  else
  if( m_pUIBagFrame == pEndCell )
  {
    m_pUIBagFrame = NULL;
  }
  else
  if( m_pWazaOshieFrame == pEndCell )
  {
    m_pWazaOshieFrame = NULL;
  }
#ifdef BTL_UI_LOWER_INTRO_EVENT_ON
  else
  if( m_pUIIntroEventFrame == pEndCell )
  {
    m_pUIIntroEventFrame  = NULL;
  }
#endif
}

//  バトル再生下画面Viewを取得
app::BattleUIRecPlayLowerView*  BtlvUi::GetUIRecPlayLowerView()
{
  app::BattleUIRecPlayLowerView*  pView = m_pBtlParamAccesser->GetRecPlayLowerView();
  GFL_ASSERT( pView );

  return  pView;
}

//  下画面の暗転表示設定
void BtlvUi::SetEnableBGFilter( const bool bFlg )
{
  if( m_pUIBGFrame == NULL )
  {
    GFL_ASSERT( 0 );
    return;
  }

  m_pUIBGFrame->SetEnableDimFilterLayout( bFlg );
}

//  ここから先はItemProcクラスの定義

//  処理開始の事前処理(すでにアイテム指定対象のポケモンが選択されてる用 ポケモンリスト詳細からの遷移時に呼び出す )
//  pSelectPartyはポケモンリストだが、アイテム指定対象のポケモン１体しか設定していない。
void BtlvUi::ItemProc::PreStart( const btl::BTL_PARTY* pSelectParty, const u8 in_pokeListIdx )
{
  GFL_ASSERT( pSelectParty );

  _Reset();

  m_pSelectParty  = pSelectParty;
  m_flowType      = ITEM_FLOW_TYPE_TARGET_POKE;

  //  ポケモン対象がすでに決まっているの一体しかない
  GFL_ASSERT( m_pSelectParty->GetMemberCount() == 1 );

  //  指定対象シーケンスタイプなので対象のポケモンアドレスがない場合はNG
  const btl::BTL_POKEPARAM* pTargetPokeParam  = m_pSelectParty->GetMemberDataConst( 0 );
  GFL_ASSERT( pTargetPokeParam );

  //  あらかじめポケモンリストのidxを取得
  m_activePokeListIdx = in_pokeListIdx;
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pTargetPokeParam      = pTargetPokeParam;
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pSingleEntryPokeParam = NULL;
  // @fix MMCat[225]: ダブル用のポインタをクリア
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pDoubleEntryPokeParam[0] = NULL;
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pDoubleEntryPokeParam[1] = NULL;
}

//  処理開始の事前処理
//  こちらはトップ画面のバッグボタンを押した時に呼び出す
//  pSelectPartyには味方の全ポケモンパーティーが入っている
void BtlvUi::ItemProc::PreStart( const ITEM_FLOW_TYPE floeType, const btl::BTL_PARTY* pSelectParty )
{
  GFL_ASSERT( pSelectParty );

  _Reset();

  m_pSelectParty  = pSelectParty;
  m_flowType      = floeType;

  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pTargetPokeParam = NULL;

  // @fix MMCat[225]: ルールごとの設定
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pSingleEntryPokeParam = NULL;
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pDoubleEntryPokeParam[0] = NULL;
  m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pDoubleEntryPokeParam[1] = NULL;
  {
    BtlRule rule = m_pParentUI->m_pBtlParamAccesser->GetBattleRule();
    // シングルモード
    if( rule == BTL_RULE_SINGLE || rule == BTL_RULE_INTRUDE )
    {
      //  単体しかでない場合は必ず場にでているポケモンは一番最初になる。
      m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pSingleEntryPokeParam = pSelectParty->GetMemberDataConst( 0 );
    }
    // ダブルモード
    else if( rule == BTL_RULE_DOUBLE )
    {
      /*
        @note
          ・マルチバトルについて
              ・AIマルチ
                  立ち位置固定、ポケモンもダブルと同じ並びなので、そのまま入れても問題ない
              ・通信マルチ
                  立ち位置変動、ポケモンの並びも特殊なため、そのままでは問題があるが、
                  道具を使用できないため、momijiでは無視する
      */
      app::BattleUIDataManager::PokeListType list_type = m_pParentUI->m_pBtlParamAccesser->GetPokeListType();
      if( list_type == app::BattleUIDataManager::MEMBER_SINGLE_PARTY )
      {
        u32 max = pSelectParty->GetMemberCount();
        if( max > 2 )
        {
          max = 2;
        }
        for( u32 i=0; i<max; i++ )
        {
          m_pParentUI->m_uiItemConfirmFrameAppParam.in_param.pDoubleEntryPokeParam[i] = pSelectParty->GetMemberDataConst( i );
        }
      }
    }
  }
}

//  処理開始
void BtlvUi::ItemProc::Start( const ItemBallUseStatus ballusemode, BattleUIActSelectFrame* pRootFrameActSelect )
{
  GFL_ASSERT_STOP( m_flowType != ITEM_FLOW_TYPE_NONE );

  //  バトルで使うアイテム管理のインスタンス作成(一時的に使用)
  m_pParentUI->m_pBtlParamAccesser->CreateInstanceItemManager();

  BattleUIItemConfirmFrame::APP_PARAM*  pItemConfirmAppParam = &m_pParentUI->m_uiItemConfirmFrameAppParam;

  m_resutlData.Clear();

  m_pRootFrameActSelect = pRootFrameActSelect;

  //  トップ画面の手前にに暗転フィルター表示設定
  if( m_pParentUI->m_pUIBGFrame != NULL )
  {
    m_pParentUI->m_pUIBGFrame->SetEnableDimFilterLayout( true );
  }

  //  ボール使用状態
  {
    //  使用可能
    if( ballusemode == BBAG_BALLUSE_NORMAL )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = -1;
    }
    //  先頭のポケモン行動時しか投げられない
    else
    if( ballusemode == BBAG_BALLUSE_NOT_FIRST )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_10;
    }
    // 手持ち・ボックスが満杯で投げられない
    else
    if( ballusemode == BBAG_BALLUSE_POKEMAX )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_09;
    }
    // 場に２体以上いるなら投げられない
    else
    if( ballusemode == BBAG_BALLUSE_DOUBLE )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_08;
    }
    // 場のポケモンが（そらをとぶなどで）消えてるなら投げられない
    else
    if( ballusemode == BBAG_BALLUSE_POKE_NONE )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_11;
    }
    else
    if( ballusemode == BBAG_BALLUSE_VS_NUSI )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_14;
    }
    else
    if( ballusemode == BBAG_BALLUSE_SIREN )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_14;
    }
    else
    if( ballusemode == BBAG_BALLUSE_SANCTUARY )
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_17;
    }
    else
    if (ballusemode == BBAG_BALLUSE_NO_CAPTURE)
    {
      pItemConfirmAppParam->in_param.useBallErrorMessageId = str_btl_bag_19;
    }
  }

  ::app::bag::APP_PARAM*  pBagAppParam = &m_pParentUI->m_uiBagFrameAppParam;

  if( m_flowType == ITEM_FLOW_TYPE_USE_BALL_SHORT_CUT )
  {
    //  ボールのショートカットでの遷移

    //  トップ画面から呼ばれるので背景マスクは不要（トップのマスクと一緒に表示してしまう）
    pItemConfirmAppParam->in_param.bBGMaskDraw  = false;

    //  ボールアイテムのみポケットデータ作成
    {
      m_pParentUI->m_pBtlParamAccesser->OutputPocketData( &m_pParentUI->m_uiBagFrameAppParam.pocket[ item::BTLPOCKET_BALL ], item::BTLPOCKET_BALL );
    }

    //  フレーム生成して呼び出す
    {
      GFL_ASSERT( m_pParentUI->m_pUIItemConfirmFrame == NULL );

      m_pParentUI->m_pUIItemConfirmFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIItemConfirmFrame>( m_pParentUI->m_pUILowerFrameManager );
      GFL_ASSERT( m_pParentUI->m_pUIItemConfirmFrame );

      {
        ::app::util::Heap*  pUtilHeap = &m_pParentUI->m_appTempHeap;

        pItemConfirmAppParam->in_param.pParentAppHeap       = pUtilHeap->GetSystemHeap();
        pItemConfirmAppParam->in_param.pParentDevHeap       = pUtilHeap->GetDeviceHeap();
        pItemConfirmAppParam->in_param.pAsyncFileManager    = m_pParentUI->m_btlvCore->GetAsyncFileManager();
        pItemConfirmAppParam->in_param.pRenderingManager    = m_pParentUI->m_btlvCore->GetRenderSys()->GetAppRenderingManager();
        pItemConfirmAppParam->in_param.pBattleParamAcceesor = m_pParentUI->m_pBtlParamAccesser;
        pItemConfirmAppParam->in_param.pPocketDataList      = &m_pParentUI->m_uiBagFrameAppParam.pocket[ item::BTLPOCKET_BALL ];
        // @fix 大森さん要望　最後に使ったボールアイテムidを初回選択に
        pItemConfirmAppParam->in_param.currentItemId        = m_pParentUI->m_pBtlParamAccesser->GetLastShortcutUseBallItemId();
        pItemConfirmAppParam->in_param.pTmpDevHeap          = pUtilHeap->GetDeviceHeap();

        //  アイテムデータ設定
        {
          //  ボール専用なので、カテゴリーは固定
          pItemConfirmAppParam->in_param.itemPocketType  = item::BTLPOCKET_BALL;
        }
      }

      m_pParentUI->m_pUIItemConfirmFrame->Initialize( &m_pParentUI->m_uiItemConfirmFrameAppParam );
    }
  }
  else
  if( m_flowType == ITEM_FLOW_TYPE_BAG )
  {
    //  ポケット数を設定
    pBagAppParam->pocket_max   = GFL_NELEMS( pBagAppParam->pocket );

    //  bagフレーム作成
    _CreateBagFrame();

    //  通常のバッグ画面の遷移
    _SetUpdateSubStateByBag( BAG_STATE_START );
  }
  //  すでにターゲット選択しているポケモン
  else
  if( m_flowType == ITEM_FLOW_TYPE_TARGET_POKE )
  {
    //  回復用のみポケットを選択
    pBagAppParam->pocket_max = 3;

    //  バッグフレーム作成
    _CreateBagFrame();

    //  通常のバッグ画面の遷移
    _SetUpdateSubStateByBag( BAG_STATE_START );
  }
}

//  終了
void BtlvUi::ItemProc::End()
{
  //  アイテム管理を削除
  m_pParentUI->m_pBtlParamAccesser->DeleteInstanceItemManager();

  //  アイテムポケットのリストを削除
  m_pParentUI->m_pBtlParamAccesser->DeletePocketData( m_pParentUI->m_uiBagFrameAppParam.pocket );

  //  ポケモンリストの削除
  m_pParentUI->m_uiBagFrameAppParam.pokemon->DeleteBattleData();

  //  フローを初期化して終了済み判定として扱う
  m_flowType  = ITEM_FLOW_TYPE_NONE;
}

//  処理強制終了
void BtlvUi::ItemProc::Die()
{
  //  バッグが動いているか
  if( m_flowType == ITEM_FLOW_TYPE_NONE )
  {
    return;
  }
  else
  if( m_flowType == ITEM_FLOW_TYPE_DIE )
  {
    return;
  }

  m_updateSubState  = 0;
  m_flowType  = ITEM_FLOW_TYPE_DIE;

  //  すぐに破棄はしない
  //  アイテム使用確認があればそれを先に破棄して、その後にバッグの破棄をする
  if( m_pParentUI->m_pUIItemConfirmFrame != NULL )
  {
    m_pParentUI->m_pUIItemConfirmFrame->End();
  }
}

//  更新
BTLV_COMMAND_RESULT BtlvUi::ItemProc::Update()
{
  if( m_flowType == ITEM_FLOW_TYPE_USE_BALL_SHORT_CUT )
  {
    BattleUIItemConfirmFrame::APP_PARAM*  pAppParam = &m_pParentUI->m_uiItemConfirmFrameAppParam;

    //  ボールのショートカットでの遷移
    if( ( pAppParam->out_param.bEnd ) && ( m_pParentUI->m_pUIItemConfirmFrame == NULL ) )
    {
      //  終了
      if( pAppParam->out_param.bUse )
      {
        //  使うアイテムidを指定
        m_resutlData.useItemId = pAppParam->out_param.useItemId;
        m_resutlData.targetPokeIndex  = 0;
        m_resutlData.targetWazaIndex  = 0;

        // @fix 大森さん要望　使ったボールアイテムidを記憶して次に初回選択するボールidを記憶
        m_pParentUI->m_pBtlParamAccesser->SetLastShortcutUseBallItemId( m_resutlData.useItemId );

        return  BCR_SEL_DECIDE;
      }
      else
      {
        return  BCR_SEL_CANCEL;
      }
    }
  }
  else
  if( ( m_flowType == ITEM_FLOW_TYPE_BAG ) || ( m_flowType == ITEM_FLOW_TYPE_TARGET_POKE ) )
  {
    //  行動選択画面からバッグ画面への遷移

    GFL_ASSERT( m_updateSubState != -1 );

    //  バッグ画面への遷移の準備
    if( m_updateSubState == BAG_STATE_START )
    {
      _SetUpdateSubStateByBag( BAG_STATE_FRAME );
    }

    //  通常のバッグ画面の遷移
    if( m_updateSubState == BAG_STATE_FRAME )
    {
      if( m_pParentUI->m_pUIBagFrame == NULL )
      {
        ::app::bag::APP_PARAM*  pAppParam = &m_pParentUI->m_uiBagFrameAppParam;

        if( pAppParam->end_mode == ::app::bag::END_SELECT )
        {
          //  ドラッグアンドドロップでアイテム使用

          //  使うアイテムidを指定
          m_resutlData.useItemId        = pAppParam->select_item;
          m_resutlData.targetPokeIndex  = pAppParam->select_pokemon_pos;
          m_resutlData.targetWazaIndex  = pAppParam->select_skill_pos;

          _SetUpdateSubStateByBag( BAG_STATE_ITEM_USE );
        }
        else
        if( pAppParam->end_mode == ::app::bag::END_CANCEL )
        {
          //  キャンセル
          _SetUpdateSubStateByBag( BAG_STATE_PROC_CANCEL );
        }
        else
        {
          GFL_ASSERT_MSG( 0, "Error BattleBag end_mode = %d\n", pAppParam->end_mode );
          _SetUpdateSubStateByBag( BAG_STATE_PROC_CANCEL );
        }
      }
      else
      if( m_pParentUI->m_pUIBagFrame->IsStopUpdate() )
      {
        //  カーソル選択でアイテム選択

        //  道具使用確認へ遷移
        _SetUpdateSubStateByBag( BAG_STATE_ITEM_CONFOIFM );
      }
    }
    else
    //  選択したアイテムの使用確認
    if( m_updateSubState == BAG_STATE_ITEM_CONFOIFM )
    {
      BattleUIItemConfirmFrame::APP_PARAM*  pItemConfirmAppParam = &m_pParentUI->m_uiItemConfirmFrameAppParam;

      ::app::bag::APP_PARAM*  pBagAppParam = &m_pParentUI->m_uiBagFrameAppParam;
      GFL_ASSERT( pBagAppParam->pokemon );

      //  アイテム説明画面終了
      if( m_pParentUI->m_pUIItemConfirmFrame == NULL )
      {
        //  選択したアイテムidを設定(バッグとリザルト共に)
        m_resutlData.useItemId    = pItemConfirmAppParam->out_param.useItemId;
        pBagAppParam->select_item = m_resutlData.useItemId;

        //  終了
        if( pItemConfirmAppParam->out_param.bUse )
        {
          //  アイテムがHP、状態回復、わざ回復だとバッグ側で更に選択する
          bool  bBattleItem = _IsBattleItem( m_resutlData.useItemId );

          if( bBattleItem )
          {
            //  バトル用であれば、即使用
            _SetUpdateSubStateByBag( BAG_STATE_ITEM_USE );
          }
          else
          {
            //  それ以外であれば回復用でBag側でアイテム選択してもらう
            _SetUpdateSubSteteByBagPokeTargetSelect();
          }
        }
        else
        {
          //  バッグ画面でカーソル位置を説明画面でアイテム切り替えたのに合わせる

          //  使用説明画面で選択したアイテムidを設定してカーソル位置を合わせる
          _UpdateBagItemCursorPos( m_resutlData.useItemId, pBagAppParam );

          //  前の画面へ戻る
          _SetUpdateSubStateByBag( BAG_STATE_FRAME );
        }
      }
    }

    if( m_updateSubState == BAG_STATE_PROC_CANCEL )
    {
      if( m_pParentUI->m_pUIBagFrame == NULL )
      {
        m_resutlData.Clear();
        return  BCR_SEL_CANCEL;
      }
    }

    if( m_updateSubState == BAG_STATE_ITEM_USE )
    {
      if( m_pParentUI->m_pUIBagFrame == NULL )
      {
        if( m_flowType == ITEM_FLOW_TYPE_TARGET_POKE )
        {
          //  バッグ側ではあくまでリストは１つしかないので pAppParam->select_pokemon_pos == 0 が返る
          //  しかしここではポケモンリスト上でのリストidxを設定する必要があるので、あらかじめ設定してリストidxを設定
          //m_resutlData.targetPokeIndex  = pAppParam->select_pokemon_pos;
          m_resutlData.targetPokeIndex  = m_activePokeListIdx;
        }

        return  BCR_SEL_DECIDE;
      }
    }
  }
  else
  if( m_flowType == ITEM_FLOW_TYPE_DIE )
  {
    enum
    {
      SEQ_DIE_ITEM_CONFIRM  = 0,
      SEQ_DIE_BAG
    };

    //  強制破棄時
    if( m_updateSubState == SEQ_DIE_ITEM_CONFIRM )
    {
      if( m_pParentUI->m_pUIItemConfirmFrame == NULL )
      {
        if( m_pParentUI->m_pUIBagFrame != NULL )
        {
          m_pParentUI->m_pUIBagFrame->RecoverUpdate( ::app::bag::BagFrame::RCV_END );
        }

        ++m_updateSubState;
      }
    }

    if( m_updateSubState == SEQ_DIE_BAG )
    {
      if( m_pParentUI->m_pUIBagFrame == NULL )
      {
        return BCR_SEL_CANCEL;
      }
    }
  }
  else
  {
    //  ここにきたら論理バグ
    GFL_ASSERT( 0 );
  }

  return BCR_SEL_NONE;
}

//  アイテム関連のフレームが存在しているか
bool  BtlvUi::ItemProc::IsExistFrames()
{
  //  アイテム関連のフレームはバッグとアイテム使用確認のフレーム
  if( m_pParentUI->m_pUIBagFrame != NULL )
  {
    return  true;
  }

  if( m_pParentUI->m_pUIItemConfirmFrame != NULL )
  {
    return  true;
  }

  return  false;
}

//  アイテム関連のフレームで演出か
bool  BtlvUi::ItemProc::IsEventFrames()
{
  if( m_pParentUI->m_pUIBagFrame != NULL )
  {
    if( m_pParentUI->m_pUIBagFrame->GetSubSeq() != 1 )
    {
      return  true;
    }
  }

  if( m_pParentUI->m_pUIItemConfirmFrame != NULL )
  {
    if( m_pParentUI->m_pUIItemConfirmFrame->GetState() != BattleUIItemConfirmFrame::STATE_TYPE_UPDATE )
    {
      return  true;
    }
  }

  return  false;
}

//  バッグフレーム作成
void BtlvUi::ItemProc::_CreateBagFrame()
{
  ::app::bag::APP_PARAM*  pAppParam = &m_pParentUI->m_uiBagFrameAppParam;
  GFL_ASSERT( pAppParam->pokemon );

  //  バッグで使うポケットのリスト作成
  m_pParentUI->m_pBtlParamAccesser->OutputPocketData( pAppParam->pocket );

  GFL_ASSERT( m_pParentUI->m_uiBagFrameAppParam.pocket );

  //  パラメータ設定
  {
    pAppParam->call_mode    = ::app::bag::CALL_BATTLE_MENU;
    pAppParam->item_manager = m_pParentUI->m_pBtlParamAccesser->GetInstanceItemManager();

    //  さしおさえをしているパーティーのidxを設定
    {
      // @fix MMCat[338]: ダブルバトルがあるのでビット管理に変更
      pAppParam->sasiosae_index = 0;

      const BTL_PARTY* pParty     = m_pParentUI->m_pBtlParamAccesser->GetBattleParty_Self();
      const u32 members           = pParty->GetMemberCount();
      const BTL_POKEPARAM*  pBpp  = NULL;

      for( u32 i = 0; i < members; ++i )
      {
        pBpp = pParty->GetMemberDataConst( i );
        if( pBpp != NULL )
        {
          if( pBpp->CheckSick( pml::wazadata::WAZASICK_SASIOSAE ) )
          {
            pAppParam->sasiosae_index |= (1<<i);
          }
        }
      }
    }

    //  参照しているポケットnoがポケット最大数以上なら０に戻す
    if( pAppParam->pocket_max <= pAppParam->pocket_no )
    {
      pAppParam->pocket_no = 0;
    }

    //  カーソルリストidx設定
    {
      //  前のターンで選択したアイテムは初期化
      pAppParam->select_item  = ITEM_DUMMY_DATA;

      //  基本カーソルリストidxは残すが、
      //  リスト個数が変わった場合は初期化する
      u32 loopNum = GFL_NELEMS( m_pParentUI->m_uiBagFrameAppParam.pocket );
      ::app::bag::POCKET_DATA*  pCurrentPokeData  = NULL;

      for( u32 i = 0; i < loopNum; ++i )
      {
        pCurrentPokeData  = &pAppParam->pocket[ pAppParam->pocket_no ];

        //  現在のカレントidxがリスト最大数を超えているとバグるので初期化
        if( pCurrentPokeData->max <= ( pCurrentPokeData->cursor_pos + pCurrentPokeData->list_scroll ) )
        {
          pCurrentPokeData->cursor_pos   = 0;
          pCurrentPokeData->list_scroll  = 0;
        }
      }
    }

    //  ポケモンリストからのバッグではすでにポケモンが選択されて、アイテム選択へ遷移する
    if( m_flowType == ITEM_FLOW_TYPE_TARGET_POKE )
    {
      pAppParam->pokemon_index  = 0;
    }

    pAppParam->pokemon->SetParameter( m_pSelectParty, m_pParentUI->m_appTempHeap.GetDeviceHeap() );
  }

  //  フレーム作成
  {
    GFL_ASSERT( m_pParentUI->m_pUIBagFrame == NULL );

    m_pParentUI->m_pUIBagFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, ::app::bag::BagFrame>( m_pParentUI->m_pUILowerFrontFrameManager );
    GFL_ASSERT( m_pParentUI->m_pUIBagFrame );

    //  捕獲チュートリアルならバッグはデモ用に設定
    if( m_pParentUI->m_btlvCore->GetBtlvMode() == BTLV_MODE_CAPTURE )
    {
      m_pParentUI->m_pUIBagFrame->InitDemoMode();
    }

    m_pParentUI->m_pUIBagFrame->Setup( &m_pParentUI->m_uiBagFrameAppParam, &m_pParentUI->m_appTempHeap, m_pParentUI->m_btlvCore->GetRenderSys()->GetAppRenderingManager() );
  }
}

//  アイテムがバトル用か判定
bool BtlvUi::ItemProc::_IsBattleItem( const u32 itemId )
{
  //  バトル用のアイテム判定(即時使用対象アイテムか)

  //  モンスターボールかどうか
  if( item::CheckBtlPocketInclude( item::BTLPOCKET_BALL, itemId ) )
  {
    return  true;
  }

  //  バトル用でかつシングルバトルであるか(必要なの？、要調査かも)
  if( item::CheckBtlPocketInclude( item::BTLPOCKET_BATTLE, itemId ) )
  {
    //  シングル
    if( m_pParentUI->m_pBtlParamAccesser->GetBattleRule() == BTL_RULE_SINGLE )
    {
      return  true;
    }
    else
    //  乱入も味方は一人のみなのでシングルとする
    if( m_pParentUI->m_pBtlParamAccesser->GetBattleRule() == BTL_RULE_INTRUDE )
    {
      return  true;
    }
  }

  //  ターゲット対象がいない
  if( btl::tables::IsNoTargetItem(itemId) || btl::tables::IsRotoponItem(itemId) )
  {
    return  true;
  }

  return  false;
}

//  バッグフレームのAppParamに指定したアイテムidに合わせたカーソル位置更新
void BtlvUi::ItemProc::_UpdateBagItemCursorPos( u32 itemId, ::app::bag::APP_PARAM* pBagAppParam )
{
  GFL_ASSERT( pBagAppParam );

  ::app::bag::POCKET_DATA*  pCurrentPocketData  = &pBagAppParam->pocket[ pBagAppParam->pocket_no ];
  for( s32 i = 0; i < pCurrentPocketData->max; ++i )
  {
    if( pCurrentPocketData->table[ i ].id == itemId )
    {
      pCurrentPocketData->cursor_pos  = i;
    }
  }
}

//  バッグ更新用のサブステート
void BtlvUi::ItemProc::_SetUpdateSubStateByBag( const BagState newState )
{
  m_updateSubState  = newState;

  if( newState == BAG_STATE_START )
  {
  /*  メモリに余力があれば残すが、今は余力があるかどうかわからないので演出優先
    //  バトルのレイアウトを終了させる
    if( ( m_pParentUI->m_uiActSelectFrameAppParam.out_param.bDie == false ) && m_pParentUI->m_pUIActSelectFrame )
    {
      m_pParentUI->m_pUIActSelectFrame->End();
    }
  */
  }
  //  バッグリストからアイテム選択
  else
  if( newState == BAG_STATE_FRAME )
  {
    if( m_pParentUI->m_pUIBagFrame->IsStopUpdate() )
    {
      m_pParentUI->m_pUIBagFrame->RecoverUpdate( ::app::bag::BagFrame::RCV_CANCEL );
    }

  }
  //  バッグで選んだ道具の使用説明
  else
  if( newState == BAG_STATE_ITEM_CONFOIFM )
  {
    BattleUIItemConfirmFrame::APP_PARAM*  pItemConfirmAppParam = &m_pParentUI->m_uiItemConfirmFrameAppParam;

    ::app::bag::APP_PARAM*  pBagFrameAppParam = &m_pParentUI->m_uiBagFrameAppParam;

    //  結果を更新
    m_resutlData.useItemId        = pBagFrameAppParam->select_item;
    m_resutlData.targetPokeIndex  = pBagFrameAppParam->select_pokemon_pos;
    m_resutlData.targetWazaIndex  = pBagFrameAppParam->select_skill_pos;

    //  フレーム生成して呼び出す
    {
      GFL_ASSERT( m_pParentUI->m_pUIItemConfirmFrame == NULL );

      m_pParentUI->m_pUIItemConfirmFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIItemConfirmFrame>( m_pParentUI->m_pUILowerFrontFrameManager );
      GFL_ASSERT( m_pParentUI->m_pUIItemConfirmFrame );

      {
        ::app::util::Heap*  pUtilHeap = &m_pParentUI->m_appTempHeap;

        pItemConfirmAppParam->in_param.pParentAppHeap       = pUtilHeap->GetSystemHeap();
        pItemConfirmAppParam->in_param.pParentDevHeap       = pUtilHeap->GetDeviceHeap();
        pItemConfirmAppParam->in_param.pAsyncFileManager    = m_pParentUI->m_btlvCore->GetAsyncFileManager();
        pItemConfirmAppParam->in_param.pRenderingManager    = m_pParentUI->m_btlvCore->GetRenderSys()->GetAppRenderingManager();
        pItemConfirmAppParam->in_param.pBattleParamAcceesor = m_pParentUI->m_pBtlParamAccesser;
        pItemConfirmAppParam->in_param.pPocketDataList      = &m_pParentUI->m_uiBagFrameAppParam.pocket[ pBagFrameAppParam->pocket_no ];
        pItemConfirmAppParam->in_param.currentItemId        = (u32)pBagFrameAppParam->select_item;
        pItemConfirmAppParam->in_param.itemPocketType       = (item::BtlPocket)pBagFrameAppParam->pocket_no;
        pItemConfirmAppParam->in_param.pTmpDevHeap          = pUtilHeap->GetDeviceHeap();
        pItemConfirmAppParam->in_param.bBGMaskDraw          = true;
      }

      m_pParentUI->m_pUIItemConfirmFrame->Initialize( &m_pParentUI->m_uiItemConfirmFrameAppParam );
    }
  }
  else
  if( newState == BAG_STATE_PROC_CANCEL )
  {
    //  キャンセルしたので今まで設定した結果値をクリア
    m_resutlData.Clear();

    //  バッグ画面を閉じる
    if( m_pParentUI->m_pUIBagFrame != NULL )
    {
      m_pParentUI->m_pUIBagFrame->RecoverUpdate( ::app::bag::BagFrame::RCV_END );
    }
  }
  else
  if( newState == BAG_STATE_ITEM_USE )
  {
    //  バッグ画面を閉じる
    if( m_pParentUI->m_pUIBagFrame != NULL )
    {
      m_pParentUI->m_pUIBagFrame->RecoverUpdate( ::app::bag::BagFrame::RCV_END );
    }
  }
}

//  使用アイテム対象選択サブステート
void BtlvUi::ItemProc::_SetUpdateSubSteteByBagPokeTargetSelect()
{
  ::app::bag::APP_PARAM*  pAppParam = &m_pParentUI->m_uiBagFrameAppParam;
  GFL_ASSERT( pAppParam->pokemon != NULL );

  if( m_pParentUI->m_pUIBagFrame->IsStopUpdate() )
  {
    m_pParentUI->m_pUIBagFrame->RecoverUpdate( ::app::bag::BagFrame::RCV_USE );
  }

  m_updateSubState = BAG_STATE_FRAME;
}

//  ここから先はPokeListProcクラスの定義

//  処理開始
void BtlvUi::PokeListProc::StartBySelectChange( const u8 outPokeIdx, const BTL_POKEPARAM* pOutputPokeParam, const bool bPokeChange )
{
  _Reset();

  m_procMode      = PROC_MODE_LIST;

  m_outPokeIdx    = outPokeIdx;
  m_bProcCancel   = true;
  m_bPokeChange   = bPokeChange;
  m_pFirstExChangeSourcePokeParam = pOutputPokeParam;

  _StartBase( NULL );
}

void BtlvUi::PokeListProc::StartByForceChange( const u8 outPokeIdx, const bool bProcCancel, const bool bPokeChange, PokeSelResult* pResult )
{
  _Reset();

  m_procMode      = PROC_MODE_CHANGE;

  m_outPokeIdx    = outPokeIdx;
  m_bProcCancel   = bProcCancel;
  m_bPokeChange   = bPokeChange;

  _StartBase( pResult );
}

//  処理終了
void BtlvUi::PokeListProc::End()
{
}

//  処理を殺す
void BtlvUi::PokeListProc::Die()
{
  if( m_pParentBtlvUI->m_pUIPokeListFrame != NULL )
  {
    m_pParentBtlvUI->m_uiPokeListFrameContextParam.list_break = true;
  }

  if( m_pParentBtlvUI->m_pUIBattlePokeListFrame != NULL )
  {
    m_pParentBtlvUI->m_pUIBattlePokeListFrame->End();
  }

  //  ポケモンリストはフェードを使っているのでフェードをなかったことにしないといけない
  m_pParentBtlvUI->m_bLowerFadeReset  = true;

  End();
}

//  処理を更新
BTLV_COMMAND_RESULT BtlvUi::PokeListProc::Update( PokeSelResult* pPokeSelectResult )
{
  GFL_ASSERT_STOP( pPokeSelectResult );

  if( m_state == POKE_LIST_STATE_SELECT )
  {
    bool  bNextPokeStatusListState  = false;

    App::PokeList::CONTEXT_PARAM* pContextParam = &m_pParentBtlvUI->m_uiPokeListFrameContextParam;
    if( ( m_pParentBtlvUI->m_pUIPokeListFrame == NULL ) && ( pContextParam->pokelist_wait ) )
    {
      if( m_pParentBtlvUI->m_uiPokeListFrameContextParam.end_mode == App::PokeList::END_MODE_CANCEL )
      {
        //  複数入れ替え値があってはならない
        GFL_ASSERT( m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt <= 0 );
        _SetState( POKE_LIST_STATE_PROC_CANCEL, pPokeSelectResult );
      }
      else
      if( m_pParentBtlvUI->m_uiPokeListFrameContextParam.end_mode == App::PokeList::END_MODE_SELECT )
      {
        bNextPokeStatusListState  = true;
      }
    }
    else
    //  ポーズしているのでこちらで処理判定をする
    if( ( m_pParentBtlvUI->m_pUIPokeListFrame != NULL ) && ( m_pParentBtlvUI->m_pUIPokeListFrame->IsPause() == true ) )
    {
      //  フレームはまだ生きているが終了
      {
        if( m_pParentBtlvUI->m_uiPokeListFrameContextParam.end_mode == App::PokeList::END_MODE_CANCEL )
        {
          //  複数入れ替えならもう一度リストを呼びなおす
//          GFL_ASSERT( 1 <= m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt );

          //  入れ替えクリア
          {
            //  複数入れ替えパラメータをクリア
            //  ダブルの複数配置しかないのでいっきにクリアする
            {
              BattleUIPokeListFrame::APP_PARAM::_tag_out_param* pOutData  = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param;
              u32 loopNum = GFL_NELEMS( pOutData->resutlData.aExchangeOccurred );
              for( u32 i = 0; i < loopNum; ++i )
              {
                pOutData->resutlData.aExchangeOccurred[ i ].Clear();
              }
              m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt  = 0;
            }
          }

          //  １体目選択に戻る
          pContextParam->cancel_flag        = m_bProcCancel;
          pContextParam->btl_change_mode    = App::PokeList::BTL_CHANGE_SINGLE;
          m_pParentBtlvUI->m_pUIPokeListFrame->RebootPokeList( *pContextParam );
        }
      }
    }

    if( bNextPokeStatusListState == true )
    {
      //  ポケモンステータスリストのステートの切り替え
      _SetState( POKE_LIST_STATE_PRE_STATUS, pPokeSelectResult );
    }
  }

  if( m_state == POKE_LIST_STATE_PRE_STATUS )
  {
    _SetState( POKE_LIST_STATE_STATUS, pPokeSelectResult );
  }

  if( m_state == POKE_LIST_STATE_STATUS )
  {
    //  リスト終了
    BattleUIPokeListFrame::APP_PARAM::_tag_out_param* pOutData  = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param;

    if( pOutData->bEnd && ( m_pParentBtlvUI->m_pUIBattlePokeListFrame == NULL ) )
    {
      m_selectPokeParty.Initialize();
      //  const_castはしたくないがしないと登録できないのでこれで
      m_selectPokeParty.AddMember( const_cast<btl::BTL_POKEPARAM*>( pOutData->resutlData.data.changePoke.pParam ) );

      //  キャンセルならリストに戻す
      if( pOutData->resutlData.type == BattleUIPokeListFrame::RESULT_TYPE_CANCEL )
      {
        //  リストに戻る
        _SetState( POKE_LIST_STATE_SELECT, pPokeSelectResult );
      }

      //  入れ替えなら入れ替え処理をするとの返り値
      if( pOutData->resutlData.type == BattleUIPokeListFrame::RESULT_TYPE_CHANGE_POKE )
      {
        //  選択指定したポケモンidx
        u8  selectPokeIdx = pOutData->resutlData.data.changePoke.pokeIdx;
        m_pParentBtlvUI->m_uiPokeListFrameContextParam.poke_index = selectPokeIdx;

        //  入れ替えしたのでカーソルをリセット
        {
          m_pParentBtlvUI->m_pBtlParamAccesser->GetPokeCursorMemory( m_outPokeIdx );
        }

        //  選択結果を設定
        pPokeSelectResult->SetCancel( false );
        pPokeSelectResult->Push( m_outPokeIdx, selectPokeIdx );

        return (BTLV_COMMAND_RESULT)( BCR_SEL_IDX1 + selectPokeIdx );
      }
      //  複数選択の入れ替えなら入れかえ回数を見る
      else
      if( pOutData->resutlData.type == BattleUIPokeListFrame::RESULT_TYPE_EXCHANGE_POKE )
      {
        bool  bEnd  = false;
        if( m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt <= 1 )
        {
          //  マルチでない場合は２つを選択していないとアウト！

          //  １回目までならまだポケモン選択に戻る
          _SetState( POKE_LIST_STATE_SELECT, pPokeSelectResult );
        }
        else
        if( m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt == 2 )
        {
          //  ダブルで複数同時配置限定
          bEnd  = true;
        }

        if( bEnd == true )
        {
          //  入れ替えをしたら終了

          pPokeSelectResult->SetCancel( false );
          pPokeSelectResult->Push( 0, pOutData->resutlData.aExchangeOccurred[ BattleUIPokeListFrame::LISTPOS_UL ].selectPokeIdx );
          pPokeSelectResult->Push( 1, pOutData->resutlData.aExchangeOccurred[ BattleUIPokeListFrame::LISTPOS_UR ].selectPokeIdx );

          //  同時入れ替え分カーソルクリアする
          {
            for( u32 i = 0; i < pPokeSelectResult->GetSelectMax(); ++i )
            {
              m_pParentBtlvUI->m_pBtlParamAccesser->GetPokeCursorMemory( i );
            }
          }

          return (BTLV_COMMAND_RESULT)( BCR_SEL_IDX1 );
        }
      }

      //  回復ならバッグ画面へ
      if( pOutData->resutlData.type == BattleUIPokeListFrame::RESULT_TYPE_USE_ITEM )
      {
        _SetState( POKE_LIST_STATE_PROC_ITEM, pPokeSelectResult );
      }
    }
  }

  if( m_state == POKE_LIST_STATE_PROC_ITEM )
  {
    BTLV_COMMAND_RESULT result  = m_itemProc.Update();

    m_pParentBtlvUI->m_itemResultData  = m_itemProc.GetResult();

    //  道具を決定したのでアイテム結果を更新
    if( result == BCR_SEL_DECIDE )
    {
      pPokeSelectResult->SetCancel( false );
      pPokeSelectResult->SetItemUse( (u8)m_pParentBtlvUI->m_itemResultData.targetPokeIndex, (u16)m_pParentBtlvUI->m_itemResultData.useItemId, m_pParentBtlvUI->m_itemResultData.targetWazaIndex );

      m_itemProc.End();

      return  BCR_SEL_ITEM;
    }
    else
    if( result == BCR_SEL_CANCEL )
    {
      m_itemProc.End();
      _SetState( POKE_LIST_STATE_STATUS, pPokeSelectResult );
    }
  }

  if( m_state == POKE_LIST_STATE_PROC_CANCEL )
  {
    pPokeSelectResult->SetCancel( true );
    return BCR_SEL_CANCEL;
  }

  return BCR_SEL_NONE;  // まだ終わってない
}

void BtlvUi::PokeListProc::_StartBase( PokeSelResult* pResult )
{
  //  トップ画面の手前にに暗転フィルター表示設定
  if( m_pParentBtlvUI->m_pUIBGFrame != NULL )
  {
    m_pParentBtlvUI->m_pUIBGFrame->SetEnableDimFilterLayout( true );
  }

  //  複数入れ替えパラメータをクリア
  {
    BattleUIPokeListFrame::APP_PARAM::_tag_out_param* pOutData  = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param;
    for( u32 i = 0; i < GFL_NELEMS( pOutData->resutlData.aExchangeOccurred ); ++i )
    {
      pOutData->resutlData.aExchangeOccurred[ i ].Clear();
    }
    m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt  = 0;
  }

  _SetState( POKE_LIST_STATE_SELECT, pResult );
}

//  ステート変更
void BtlvUi::PokeListProc::_SetState( const ProcListState newState, PokeSelResult* pPokeSelectResult )
{
  ProcListState oldState  = m_state;

  m_state = newState;

  if( newState == POKE_LIST_STATE_SELECT )
  {
    //  ポケモンリストフレーム作成
    {
      GFL_ASSERT( m_pParentBtlvUI->m_pUIPokeListFrame == NULL );
      m_pParentBtlvUI->m_pUIPokeListFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::PokeList::PokeListSimpleFrame>( m_pParentBtlvUI->m_pUILowerFrontFrameManager );

      gfl2::heap::HeapBase *deviceMem = m_pParentBtlvUI->m_appTempHeap.GetDeviceHeap();

      // 必要なパラメータ設定
      {
        // APPPARAM
        {
          App::PokeList::APP_PARAM* pAppParam = &m_pParentBtlvUI->m_uiPokeListFrameAppParam;

          pAppParam->mpAppHeap             = &m_pParentBtlvUI->m_appTempHeap;
          pAppParam->mpAppRenderingManager = m_pParentBtlvUI->m_btlvCore->GetRenderSys()->GetAppRenderingManager();
          pAppParam->mpAsyncFileManager    = m_pParentBtlvUI->m_btlvCore->GetAsyncFileManager();
          pAppParam->mpCmnIcon             = &m_pParentBtlvUI->m_pBtlParamAccesser->GetAppCommonGrpIconData();
        }

        // CONTEXT_PARAM
        {
          //  ポケモンステータスリストで選択したポケモンパラメータ
          const BTL_POKEPARAM*  pCurrentBtlParam  = m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param.resutlData.pCurrentPokeParam;

          App::PokeList::CONTEXT_PARAM* pContextParam = &m_pParentBtlvUI->m_uiPokeListFrameContextParam;

          pContextParam->list_break       = false;
          pContextParam->mode             = App::PokeList::MODE_BATTLE;
          pContextParam->cancel_flag      = m_bProcCancel;
          pContextParam->pokelist_wait    = false;
          pContextParam->from_top         = (oldState != POKE_LIST_STATE_STATUS);

          // @fix GFNMCat[1363] ポケモンリスト画面で通信対戦時には入力ＳＥをしないように
          pContextParam->play_sound       = m_pParentBtlvUI->m_pBtlParamAccesser->IsPlayInputSE();

          // @fix MCT要望 AIコントロールターン時はカーソルを初期化
          //  こうしないとハングする
          if( m_pParentBtlvUI->m_pBtlParamAccesser->IsAICtrlTurn() == true )
          {
            pContextParam->poke_index = 0;
          }

          pContextParam->btl_change_mode   = App::PokeList::BTL_CHANGE_SINGLE;
          if( m_procMode == PROC_MODE_LIST )
          {
            //  行動選択からの入れ替え
            pContextParam->btl_mode         = App::PokeList::BTL_MODE_NORMAL;
          }
          else
          if( m_procMode == PROC_MODE_CHANGE )
          {
            //  気絶時からの入れ替え
            pContextParam->btl_mode         = App::PokeList::BTL_MODE_EXCHANGE;

            //  気絶時で複数対応同時に入れ替え
            if( 1 < pPokeSelectResult->GetSelectMax() )
            {
              if( m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.m_ExchangeOccurredCnt <= 0 )
              {
                //  1体目選択
              }
              else
              {
                //  ２体目以降の選択ではポケモンリスト画面にキャンセルを出す
                pContextParam->btl_change_mode   = App::PokeList::BTL_CHANGE_DOUBLE;
                pContextParam->cancel_flag  = true;
              }
            }
          }
          else
          {
            //  ここには絶対にこない（きたら論理バグ）
            GFL_ASSERT( 0 );
          }

          if( pCurrentBtlParam )
          {
            pContextParam->select_poke_addr = pCurrentBtlParam->GetSrcData();
          }
          else
          {
            pContextParam->select_poke_addr = NULL;
          }

          //  バトルモード設定
          {
            //  プレイヤー自身のパーティー配置
            app::BattleUIDataManager::PokeListType  listType  = m_pParentBtlvUI->m_pBtlParamAccesser->GetPokeListType();
            if( listType == app::BattleUIDataManager::MEMBER_MULTI_PARTY_L )
            {
              pContextParam->btl_mode = App::PokeList::BTL_MODE_MULTI_LEFT;

              //    味方は右に配置
              pContextParam->friend_party.SetParameter( m_pParentBtlvUI->m_btlvCore->GetBattleParty_Friend(), deviceMem );
            }
            else
            if( listType == app::BattleUIDataManager::MEMBER_MULTI_PARTY_R )
            {
              pContextParam->btl_mode = App::PokeList::BTL_MODE_MULTI_RIGHT;

              //  味方は左に配置
              pContextParam->friend_party.SetParameter( m_pParentBtlvUI->m_btlvCore->GetBattleParty_Friend(), deviceMem );
            }
          }

          pContextParam->my_party.SetParameter( m_pParentBtlvUI->m_btlvCore->GetBattleParty_Self(), deviceMem );
        }
      }

      m_pParentBtlvUI->m_pUIPokeListFrame->Setup( &m_pParentBtlvUI->m_uiPokeListFrameAppParam, &m_pParentBtlvUI->m_uiPokeListFrameContextParam );
    }
  }
  else
  if( newState == POKE_LIST_STATE_PRE_STATUS )
  {
  }
  else
  if( newState == POKE_LIST_STATE_STATUS )
  {
    //  ポケモンリストのアプリ結果から初回選択ポケモン指定をする
    App::PokeList::CONTEXT_PARAM* pAppListContextParam = &m_pParentBtlvUI->m_uiPokeListFrameContextParam;

    //  バトルポケモンリストフレーム作成
    GFL_ASSERT( m_pParentBtlvUI->m_pUIBattlePokeListFrame == NULL );
    m_pParentBtlvUI->m_pUIBattlePokeListFrame  = MENU_FRAME_CALL_MODAL<applib::frame::Manager, BattleUIPokeListFrame>( m_pParentBtlvUI->m_pUILowerFrontFrameManager );

    //  必要なパラメータ設定
    {
      BattleUIPokeListFrame::APP_PARAM::_tag_in_param* pInParam = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.in_param;

      ::app::util::Heap*  pUtilHeap = &m_pParentBtlvUI->m_appTempHeap;

      pInParam->pAsyncFileManager                   = m_pParentBtlvUI->m_btlvCore->GetAsyncFileManager();
      pInParam->pBtlParamAccessror                  = m_pParentBtlvUI->m_pBtlParamAccesser;
      pInParam->pParentAppHeap                      = pUtilHeap->GetSystemHeap();
      pInParam->pParentDevHeap                      = pUtilHeap->GetDeviceHeap();
      pInParam->pRenderingManager                   = m_pParentBtlvUI->m_btlvCore->GetRenderSys()->GetAppRenderingManager();
      pInParam->bCanEsc                             = m_bPokeChange;
      pInParam->pPokeSelectResult                   = pPokeSelectResult;
      pInParam->pTmpDevHeap                         = pUtilHeap->GetDeviceHeap();
      pInParam->m_pFirstExChangeSourcePokeParam     = m_pFirstExChangeSourcePokeParam;
      
      if( m_procMode == PROC_MODE_LIST )
      {
        //  行動選択からの入れ替え
        pInParam->frameType = BattleUIPokeListFrame::FRAME_TYPE_NORMAL;
      }
      else
      if( m_procMode == PROC_MODE_CHANGE )
      {
        //  気絶時からの入れ替え
        pInParam->frameType = BattleUIPokeListFrame::FRAME_TYPE_REPLACE;
      }
      else
      {
        //  ここには絶対にこない（きたら論理バグ）
        GFL_ASSERT( 0 );
      }

      if( oldState == POKE_LIST_STATE_PROC_ITEM )
      {
        //  バッグ画面から戻った時は選択するポケモンカーソルを復帰させる
        pInParam->pFirstCurrentPokeParam  = m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param.resutlData.pCurrentPokeParam->GetSrcData();

        //  前のステートがバッグであればボタンカーソルをかいふくに
        pInParam->firstButtonCursorPos  = BattleUIPokeListFrame::FIRST_BUTTON_CURSOR_POS_CURE;
      }
      else
      {
        //  ポケモンカーソル位置をポケモン選択で選択したポケモンに
        pInParam->pFirstCurrentPokeParam  = pAppListContextParam->select_poke_addr;

        //  通常はいれかえボタン位置にカーソルを
        pInParam->firstButtonCursorPos  = BattleUIPokeListFrame::FIRST_BUTTON_CURSOR_POS_NONE;
      }
    }

    m_pParentBtlvUI->m_pUIBattlePokeListFrame->Initialize( &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam );
  }
  else
  if( newState == POKE_LIST_STATE_PROC_ITEM )
  {
    BattleUIPokeListFrame::APP_PARAM::_tag_out_param* pOutParam = &m_pParentBtlvUI->m_uiBattlePokeListFrameAppParam.out_param;

    //  アイテム使用画面を開く
    m_itemProc.PreStart( &m_selectPokeParty, pOutParam->resutlData.data.changePoke.pokeIdx );
    m_itemProc.Start( BBAG_BALLUSE_NORMAL, NULL );
  }
  else
  if( newState == POKE_LIST_STATE_PROC_CANCEL )
  {
  }
}

GFL_NAMESPACE_END(btl)
