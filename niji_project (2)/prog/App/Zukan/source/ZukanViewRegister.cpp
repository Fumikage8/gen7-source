//==============================================================================
/**
 @file    ZukanViewRegister.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

#include <Fade/include/gfl2_FadeManager.h>

// pmlのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <PokeTool/include/PokeModelSystem.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/info.h>
#include <niji_conv_header/app/zukan/info_pane.h>
#include <niji_conv_header/app/zukan/info_anim_list.h>

#include <niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zkn_type.h>
#include <niji_conv_header/message/msg_zkn_form.h>
#include <niji_conv_header/message/msg_zkn_height.h>
#include <niji_conv_header/message/msg_zkn_weight.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑登録チェッカーのインクルード
#include <PokeTool/include/PokeToolPokeGet.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanFrameRegister.h"

#include "ZukanViewRegister.h"

// デバッグ用
#include <debug/include/gfl2_DebugWinSystem.h>
#include <debug/include/gfl2_DebugWinItemTypes.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include "ZukanDebugWin.h"
#include "niji_conv_header/message/debug/msg_debug_menu.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterListener
 @brief     図鑑の登録画面のビューのリスナー
 */
//==============================================================================
class ZukanViewRegisterListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterListener);

public:
  ZukanViewRegisterListener(ZukanViewRegister* view);
  virtual ~ZukanViewRegisterListener();

  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewRegister* m_view;
  InputState         m_inputState;
};


//==============================================================================
//
//  ZukanViewRegisterListener
//
//==============================================================================
ZukanViewRegisterListener::ZukanViewRegisterListener(ZukanViewRegister* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewRegisterListener::~ZukanViewRegisterListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewRegisterListener::OnLayoutPaneEvent( const u32 painId )
{
  app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = painId;
      m_view->Req(ZukanViewRegister::REQ_ID_BUTTON_DECIDE, button_id);
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}

void ZukanViewRegisterListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
//
//  ZukanViewRegister
//
//==============================================================================
ZukanViewRegister::ZukanViewRegister(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_infoFunc(param)
{
  // 初期化
  {
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }

    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      m_buttonPaneInfo[i] = ZukanType::ButtonPaneInfo();
    }
  }

  // 十字キー(CrossKey)、ボタン(Button)にスティック（Stick）の操作を加えない
  this->SetStickEmulationMode(false);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

  // ZukanViewRegisterListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewRegisterListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
 
    void* buf = m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_INFO);
    void* grpFontBuf = m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_GRPFONT_RES2D);  // グラフィックフォント
    if(buf && grpFontBuf)
    {
      const app::util::G2DUtil::LytResData resTbl[] =
      {
        // res_0
        {
          buf,
          LYTRES_ZUKANGRPFONT_END,  // niji_conv_header/app/common/graphic_font/zukan_grpfont/res2d/ZukanGrpFont.h
          app::util::G2DUtil::ATTACH_ROOT
        },
        // res_1
        {
          grpFontBuf,
          0,
          app::util::G2DUtil::ATTACH_RESOURCE
        },
      };
    
      const app::util::G2DUtil::LytwkData wkTbl[] =
      {
        // wk_0
        {
          // 下画面
          0,  // res_0
          LYTRES_INFO_ZUKAN_MIN_LOW_002_BFLYT,
          LA_INFO_ZUKAN_MIN_LOW_002___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          1,  // wk_0の1個のみ 
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );

      // ボタンマネージャ
      this->createButtonManager();

      // ページ内容生成
      this->createPage();

      // インアニメ
      this->startFadeInSeq();
      this->startInSeq();  // 時間短縮のためフェードインとともにインも同時に呼ぶ
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  ZukanUtil::SetPokeModelVisible(m_param.tool, false);  // フェードイン前に非表示にしておく。
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_SMALL_CENTER);
  m_param.tool->ChangePokeModel(m_param.contents->GetCurrDispPokeSimpleParam());
  //m_infoFunc.setPokeModelControllerTarget(m_param.contents->GetCurrDispPokeSimpleParam());

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE
  this->debugCreateDebugWin();
#endif
}
ZukanViewRegister::~ZukanViewRegister()
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE
  this->debugDestroyDebugWin();
#endif

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // ボタンマネージャ
  this->destroyButtonManager();

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewRegisterListener
  GFL_SAFE_DELETE(m_viewListener);
}

void ZukanViewRegister::Update(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_FADE_IN:
    {
      if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
      {
        this->startInSeq();
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ON_IN:
    {
      if(    pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)
          && ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        ZukanUtil::SetPokeModelVisible(m_param.tool, true);
        
        this->changeDispState(DISP_STATE_ON_WORK);
        m_viewListener->SetInputState(ZukanViewRegisterListener::INPUT_STATE_ENABLE);
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      // ポケモンモデル操作
      //m_infoFunc.updatePokeModelController();  // LRズーム、3Dパッド回転。このシーンでは十字キーと3Dパッドを切り離しておく。
      m_param.tool->UpdatePokeModelController();  // LRズーム、3Dパッド回転。このシーンでは十字キーと3Dパッドを切り離しておく。

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
      {
        s32 pokeModelSRTModifyMode = 0;
#if PM_DEBUG
        pokeModelSRTModifyMode = ZukanDebugWin::GetPokeModelSRTModifyMode();
#endif
        if(pokeModelSRTModifyMode != 0)
        {
          m_param.tool->UpdatePokeModelControllerForPokeModelSRTModifyMode();
        }
      }
#endif
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        // フェードアウトを待つ。使っていないフェードアウトはtrueを返し続ける。
        if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
        {
          // ポケモンモデルは表示しっぱなしにしておくのでコメントアウト。
          //m_param.tool->ResetPokeModel();  // ポケモンモデルリセット
          //ZukanUtil::SetPokeModelVisible(m_param.tool, false);  // フェードアウト後に非表示にしておく。
         
          // しかし、終了するときだけは非表示にする
          if( *m_param.nextSceneType == ZukanType::SCENE_TYPE_END )
          {
            ZukanUtil::SetPokeModelVisible(m_param.tool, false);  // フェードアウト後に非表示にしておく。
          }

          this->changeDispState(DISP_STATE_WAIT_SAFETY);
        }
      }
      break;
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      m_param.frameCell->Req(ZukanFrameRegisterCell::REQ_ID_END);
      this->changeDispState(DISP_STATE_WAIT_END);
      break;
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanViewRegister::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_0
}

void ZukanViewRegister::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  case REQ_ID_BUTTON_DECIDE:
    {
      switch(reqParam)
      {
      case BUTTON_ID_OK:     this->startOutSeq();  break;
      case BUTTON_ID_VOICE:  this->playVoice();    break;
      case BUTTON_ID_MOTION: this->changeMotion(); break;
      case BUTTON_ID_PAUSE:  this->switchPause();  break;
      }
      break;
    }
  }
}

void ZukanViewRegister::createButtonManager(void)
{
  ZukanUtil::CreateButtonManager(
      m_param.appHeap,
      this,
      0,  // res_0
      LYT_WK_ID_LOWER,  // wk_0
      BUTTON_ID_NUM,
      BUTTON_CONST_INFO,
      NULL,
      m_buttonPaneInfo
  );
}
void ZukanViewRegister::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
}

void ZukanViewRegister::startFadeInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
  this->changeDispState(DISP_STATE_ON_FADE_IN);
  
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewRegisterListener::INPUT_STATE_OCCUPY);        
}
void ZukanViewRegister::startInSeq(void)
{
  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_IN_ENTRY;
  
  Sound::PlaySE( SEQ_SE_SYS_NJ_004 );  // 画面外からシュッっと入ってくる音
  
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewRegisterListener::INPUT_STATE_OCCUPY);
}
void ZukanViewRegister::startOutSeq(void)
{
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
  {
    s32 pokeModelSRTModifyMode = 0;
#if PM_DEBUG
    pokeModelSRTModifyMode = ZukanDebugWin::GetPokeModelSRTModifyMode();
#endif
    if(pokeModelSRTModifyMode != 0)
    {
#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE
      this->debugSetNextPoke();
      return;
#endif
    }
  }
#endif


  {
    if( PokeTool::PokeGetUtil::DoesGoToPokeNameInput(
                 m_param.param->zukanRegisterEventParam->pp,
                 m_param.param->zukanRegisterEventParam->callerType ) )
    {
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_POKE_NAME;
    }
    else if( PokeTool::PokeGetUtil::DoesGoToPokePutSelect(
                 m_param.param->zukanRegisterEventParam->pp,
                 m_param.param->zukanRegisterEventParam->callerType ) )
    {
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
    }
    else
    {
      if( PokeTool::PokeGetUtil::DoesPut_NotSetFlag(m_param.param->zukanRegisterEventParam->pp, m_param.param->zukanRegisterEventParam->callerType) )
      {
        *m_param.nextSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
      }
      else
      {
        if( PokeTool::PokeGetUtil::DoesSetFlag(m_param.param->zukanRegisterEventParam->pp, m_param.param->zukanRegisterEventParam->callerType) )
        {
          //PokeTool::PokeGetUtil::SetFlag(m_param.param->zukanRegisterEventParam->pp);  // SetFlagはZukanContentsをつくるときに済ませてあるのでコメントアウト。
        }
        *m_param.nextSceneType = ZukanType::SCENE_TYPE_END;
      }
    }
  }


  m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_RETURN_ENTRY;
  
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);
  
  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewRegisterListener::INPUT_STATE_OCCUPY);


  // フェードアウト
  {
    if( *m_param.nextSceneType == ZukanType::SCENE_TYPE_END )
    {
      gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
      pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
    }
  }
}

void ZukanViewRegister::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewRegister::createPage(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();                      // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );      // res_0

  {
    const ZukanContents::PokeData*             pokeData            = m_param.contents->GetCurrDispPokeData();
    u32                                        drawFigureDataIndex = m_param.contents->GetCurrDispDrawFigureDataIndex();
    const ZukanContents::PokeData::FigureData* figureData          = &(pokeData->figureData[drawFigureDataIndex]);

    // 性別
    // 左側=オス
    // 右側=メス
    // というふうに位置を固定して表示する

#define A1M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_01_ICON_MALE    // 左側
#define A1F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_01_ICON_FEMALE
#define A0M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_00_ICON_MALE    // 右側
#define A0F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_00_ICON_FEMALE

    s32 frame=0;
    u32 animA1=A1M;  // 左側
    u32 animA0=A0M;  // 右側

    switch(figureData->figureGender)
    {
    case ZukanContents::PokeData::FIGURE_GENDER_MALE:        frame=3; animA1=A1M; animA0=A0M; break;
    case ZukanContents::PokeData::FIGURE_GENDER_FEMALE:      frame=1; animA1=A1F; animA0=A0F; break;
    case ZukanContents::PokeData::FIGURE_GENDER_UNKNOWN:     frame=0; animA1=A1M; animA0=A0M; break;
    case ZukanContents::PokeData::FIGURE_GENDER_MALE_FEMALE: frame=2; animA1=A1M; animA0=A0F; break;
    }

    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_INFO_ZUKAN_MIN_LOW_002_GENDER, frame);
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A0M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A0F, false);  // ループアニメなので、UnBindするまでアニメはくっついたままになっている。
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A1M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A1F, false);  // だから一度UnBindしておく。
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animA0, 0);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animA1, 0);

#undef A0M
#undef A0F
#undef A1M
#undef A1F

    // レア
    lytWk_0->GetPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_STAR_ICON)->SetVisible(figureData->rareFlag != 0);
  }

  m_infoFunc.SetPointer(lytWk_0, lytRes_0);
  //m_infoFunc.setupPokeModelController(m_param.tool->GetPokeSimpleModelInFrame(), 200, 90, 160, 1.0f);
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_SMALL_CENTER);
  
  m_infoFunc.SetContents(
      pG2DUtil,
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN),
      NULL,
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES),
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_FORM),
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT),
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT),
      m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT),
      ZukanType::ZUKAN_LANG_DEFAULT,
      NULL
  );
         

  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // 次へボタン
    gfl2::lyt::LytParts* nextButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_A3);
    gfl2::lyt::LytTextBox* nextButtonTextPane = lytWk_0->GetTextBoxPane(nextButtonPartsPane, PANENAME_COMMON_BTN_LOW_018_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(nextButtonTextPane, msg_zkn_15_02);
    // なきごえボタン
    gfl2::lyt::LytParts* voiceButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_Y4_ENTRY);
    gfl2::lyt::LytTextBox* voiceButtonTextPane = lytWk_0->GetTextBoxPane(voiceButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(voiceButtonTextPane, msg_zkn_09_03);
    // うごきボタン
    gfl2::lyt::LytParts* motionButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_X4_00);
    gfl2::lyt::LytTextBox* motionButtonTextPane = lytWk_0->GetTextBoxPane(motionButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(motionButtonTextPane, msg_zkn_09_04);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
}

void ZukanViewRegister::playVoice(void)
{
  PokeTool::SimpleParam sp = m_param.contents->GetCurrDispPokeSimpleParam();
  Sound::PlayVoice( 0, sp.monsNo, sp.formNo );  // voice_indexは0でいいか？
}
void ZukanViewRegister::changeMotion(void)
{
  m_param.tool->SetNextPokeModelAnime();
}
void ZukanViewRegister::switchPause(void)
{
  m_param.tool->SwitchPokeModelAnimePause();
}

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE
ZukanViewRegister::ZukanViewRegister*  ZukanViewRegister::s_debugInst = NULL;
PokeTool::SimpleParam                  ZukanViewRegister::s_debugSp;

void ZukanViewRegister::debugUpdatePokeSetting(void* /*userWork*/, gfl2::debug::DebugWinItem* /*item*/)
{
  gfl2::ui::Button* uiButton = gfl2::debug::DebugWin_GetUiButton();

  s32 delta = 1;
  if( uiButton->IsHold( gfl2::ui::BUTTON_R ) )
  {
    delta = 100;
  }
  else if( uiButton->IsHold( gfl2::ui::BUTTON_L ) )
  {
    delta = 10;
  }

  s32 no = static_cast<s32>(s_debugSp.monsNo);
  
  if( uiButton->IsRepeat( gfl2::ui::BUTTON_LEFT ) )
  {
    no -= delta;
  }
  else if( uiButton->IsRepeat( gfl2::ui::BUTTON_RIGHT ) )
  {
    no += delta;
  }
  
  if(no <= MONSNO_NULL || no > MONSNO_END)
  {
    no = static_cast<s32>(MONSNO_HUSIGIDANE);
  }
 
  if( no != static_cast<s32>(s_debugSp.monsNo) )
  {
    s_debugInst->debugSetPoke(static_cast<MonsNo>(no));
  }
}
wchar_t* ZukanViewRegister::debugDrawPokeSetting(void* /*userWork*/, gfl2::debug::DebugWinItem* item)
{
  wchar_t* workStr = gfl2::debug::DebugWin_GetWorkStr();
  swprintf( workStr, gfl2::debug::DEBUGWIN_NAME_LEN, L"%ls[%03d][%02d][%d]", item->GetName(), s_debugSp.monsNo, s_debugSp.formNo, s_debugSp.sex );
  return workStr;
}

void ZukanViewRegister::debugCreateDebugWin(void)
{
  gfl2::str::MsgData* msgData = NULL;
  ::Debug::DebugWin::DebugWinWork* devWork = GFL_SINGLETON_INSTANCE( ::Debug::DebugWin::DebugWinWork );
  if( devWork )
  {
    msgData = devWork->GetMessageData();
  }
  
  gfl2::debug::DebugWinGroup* parentGroup = NULL;

  gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);

  m_debugPokeChangeGroup = gfl2::debug::DebugWin_AddGroup(heap, msgData, msg_dmenu_dev_zukan_prism_39, parentGroup);
    
  gfl2::debug::DebugWin_AddItemUpdateFunc(m_debugPokeChangeGroup, heap, msgData, msg_dmenu_dev_zukan_prism_39, NULL, debugUpdatePokeSetting, debugDrawPokeSetting);

  // 初期化処理
  {
    s_debugInst = this;
    
    s_debugSp.monsNo  = MONSNO_HUSIGIDANE;
    s_debugSp.formNo  = 0;
    s_debugSp.sex     = pml::SEX_MALE;
    s_debugSp.isRare  = false;
    s_debugSp.isEgg   = false;
    //s_debugSp.isEgg   = true;  // タマゴの確認をしたいときはこちら
    s_debugSp.perRand = 0;
  }
}
void ZukanViewRegister::debugDestroyDebugWin(void)
{
  gfl2::debug::DebugWin_RemoveGroup(m_debugPokeChangeGroup);
  m_debugPokeChangeGroup =  NULL;
}
void ZukanViewRegister::debugSetNextPoke(void)
{
  bool formNoUp = false;
  bool monsNoUp = false;

  // 性別
  pml::personal::LoadPersonalData( s_debugSp.monsNo, s_debugSp.formNo );
  pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );

  if(sexVec == pml::personal::SEX_VECTOR_ONLY_MALE || sexVec == pml::personal::SEX_VECTOR_ONLY_FEMALE || sexVec == pml::personal::SEX_VECTOR_UNKNOWN)
  {
    formNoUp = true;
  }
  else
  {
    // 同じ３Ｄモデルか？
    s32 maleDataIdx   = m_param.tool->GetPokeModelSys()->GetDataIdx(s_debugSp.monsNo, s_debugSp.formNo, pml::SEX_MALE  );
    s32 femaleDataIdx = m_param.tool->GetPokeModelSys()->GetDataIdx(s_debugSp.monsNo, s_debugSp.formNo, pml::SEX_FEMALE);
    if(maleDataIdx != femaleDataIdx)
    {
      if(s_debugSp.sex == pml::SEX_MALE)
      {
        s_debugSp.sex = pml::SEX_FEMALE;
      }
      else
      {
        formNoUp = true;
      }
    }
    else
    {
      formNoUp = true;
    }
  }

  // フォルム
  if( formNoUp )
  {
    ++s_debugSp.formNo;
    
    pml::personal::LoadPersonalData( s_debugSp.monsNo, 0 );
    u32 formNum = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_form_max);

    if(s_debugSp.formNo < formNum)
    {
      // フォルムが変わったので、性別を初期化する
      this->debugInitSex();
    }
    else
    {
      monsNoUp = true;
    }
  }

  // ポケモン全国図鑑番号
  if( monsNoUp )
  {
    s_debugSp.monsNo = static_cast<MonsNo>(static_cast<s32>(s_debugSp.monsNo) +1);

    if(s_debugSp.monsNo > MONSNO_END)
    {
      s_debugSp.monsNo = MONSNO_HUSIGIDANE;
    }

    // ポケモン全国図鑑番号が変わったので、フォルムを初期化する
    this->debugInitFormNo();
  }

  m_param.tool->ChangePokeModel(s_debugSp);
}
void ZukanViewRegister::debugSetPoke(MonsNo monsNo)
{
  s_debugSp.monsNo = monsNo;

  if(s_debugSp.monsNo <= MONSNO_NULL || s_debugSp.monsNo > MONSNO_END)
  {
    s_debugSp.monsNo = MONSNO_HUSIGIDANE;
  }

  // ポケモン全国図鑑番号が変わったので、フォルムを初期化する
  this->debugInitFormNo();
  
  m_param.tool->ChangePokeModel(s_debugSp);
}
void ZukanViewRegister::debugInitFormNo(void)
{
  // s_debugSp.monsNoに適切な値を入れた状態で呼び出すこと
  // s_debugSp.formNoを初期化して返す(更に、s_debugSp.sexを初期化して返す)
  s_debugSp.formNo = 0;

  // フォルムが変わったので、性別を初期化する
  this->debugInitSex();
}
void ZukanViewRegister::debugInitSex(void)
{
  // s_debugSp.monsNo, s_debugSp.formNoに適切な値を入れた状態で呼び出すこと
  // s_debugSp.sexを初期化して返す
  pml::personal::LoadPersonalData( s_debugSp.monsNo, s_debugSp.formNo );
  pml::personal::SexVector sexVec = static_cast<pml::personal::SexVector>( pml::personal::GetPersonalParam(pml::personal::PARAM_ID_sex) );
  
  if(sexVec == pml::personal::SEX_VECTOR_ONLY_MALE)
  {
    s_debugSp.sex = pml::SEX_MALE;
  }
  else if(sexVec == pml::personal::SEX_VECTOR_ONLY_FEMALE)
  {
    s_debugSp.sex = pml::SEX_FEMALE;
  }
  else if(sexVec == pml::personal::SEX_VECTOR_UNKNOWN)
  {
    s_debugSp.sex = pml::SEX_UNKNOWN;
  }
  else
  {
    s_debugSp.sex = pml::SEX_MALE;
  }
}
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

