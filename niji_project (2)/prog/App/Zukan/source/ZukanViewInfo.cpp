//==============================================================================
/**
 @file    ZukanViewInfo.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Tool/app_tool_ScrollBar.h>
#include <AppLib/include/Tool/RotomToneGenerator/app_tool_RotomToneGenerator.h>

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

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanContents.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"
#include "ZukanPokeIconTextureManager.h"
#include "ZukanTool.h"
#include "ZukanQRDisplay.h"
#include "ZukanFrameInfo.h"

#include "ZukanViewInfo.h"

#include "ZukanDebugWin.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewInfoListener
 @brief     図鑑の情報画面のビューのリスナー
 */
//==============================================================================
class ZukanViewInfoListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewInfoListener);

public:
  ZukanViewInfoListener(ZukanViewInfo* view);
  virtual ~ZukanViewInfoListener();

  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  virtual app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual void OnLayoutPaneTouchTriggerEvent( const u32 paneId );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewInfo* m_view;
  InputState     m_inputState;
};


//==============================================================================
//
//  ZukanViewInfoListener
//
//==============================================================================
ZukanViewInfoListener::ZukanViewInfoListener(ZukanViewInfo* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewInfoListener::~ZukanViewInfoListener()
{}
app::ui::UIInputListener::ListenerResult ZukanViewInfoListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
      bool buttonManagerIsWorking = false;
      if( m_view->GetButtonManager() )  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      {
        buttonManagerIsWorking = true;
        if( m_view->GetButtonManager()->GetUpdateState() == app::tool::ButtonManager::UPDATE_STATE_IDLE )  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
        {
          buttonManagerIsWorking = false;
        }
      }
      if( buttonManagerIsWorking == false )  // ボタンマネージャ動作中はキー入力させない
      {
        if( pButton->IsTrigger(gfl2::ui::BUTTON_L) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_TRIGGER_L);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_R) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_TRIGGER_R);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_L) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_REPEAT_L);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_R) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_REPEAT_R);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_LEFT) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_TRIGGER_LEFT);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_TRIGGER_RIGHT);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_LEFT) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_REPEAT_LEFT);
        }
        else if( pButton->IsRepeat(gfl2::ui::BUTTON_RIGHT) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_REPEAT_RIGHT);
        }
        else if( pButton->IsTrigger(gfl2::ui::BUTTON_X) )
        {
          m_view->Req(ZukanViewInfo::REQ_ID_KEY_TRIGGER_X);
        }
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
app::ui::UIInputListener::ListenerResult ZukanViewInfoListener::OnLayoutPaneEvent( const u32 painId )
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
      m_view->Req(ZukanViewInfo::REQ_ID_BUTTON_DECIDE, button_id);
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}
void ZukanViewInfoListener::OnLayoutPaneTouchTriggerEvent( const u32 paneId )
{
  switch(m_inputState)
  {
  case INPUT_STATE_ENABLE:
    {
      u32 button_id = paneId;
      m_view->Req(ZukanViewInfo::REQ_ID_BUTTON_TOUCH, button_id);
      break;
    }
  }
}
void ZukanViewInfoListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
  
  app::tool::ButtonManager* buttonManager = m_view->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  buttonManager->SetInputEnable( m_inputState == INPUT_STATE_ENABLE );
}




//==============================================================================
//
//  ZukanViewInfo
//
//==============================================================================
ZukanViewInfo::ZukanViewInfo(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_qrDevMem(NULL),
    m_viewListener(NULL),
    m_buttonPaneInfo(NULL),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0),
    m_modeIsFormVoice(false),
    m_modeChanging(false),
    m_isQRCodeDisp(false),
    m_gameLang(ZukanType::ZUKAN_LANG_JAPAN),
    m_currLangIndex(0),
    m_userLangIndex(0),
    m_pokeIconTexManager(NULL),
    m_scrollBar(NULL),
    m_pokeNoTextPane(NULL),
    m_pokeNameTextPane(NULL),
    m_pokeSpeciesTextPane(NULL),
    m_formNameTextPane(NULL),
    m_heightLabelTextPane(NULL),
    m_weightLabelTextPane(NULL),
    m_heightValTextPane(NULL),
    m_weightValTextPane(NULL),
    m_commentTextPane(NULL),
    m_type1PicPane(NULL),
    m_type2PicPane(NULL),
    m_QRButtonPane(NULL),
    m_QRLytTopPane(NULL),
    m_QRDispTopPane(NULL),
    m_NoQRTopPane(NULL),
    m_NoQRTextPane(NULL),
    m_QRTopPane(NULL),
    m_QRPicPane(NULL),
    m_pokeIconPicPaneForExplain(NULL),
    m_rarePaneForExplain(NULL),
    //m_gender1PaneForExplain(NULL),
    //m_gender2PaneForExplain(NULL),
    m_pokeIconPicPaneForFormVoice(NULL),
    m_rarePaneForFormVoice(NULL),
    //m_gender1PaneForFormVoice(NULL),
    //m_gender2PaneForFormVoice(NULL),
    m_infoFunc(param),
    m_QRDisplay(NULL),
    m_QRDisplayDummyVal(0),
    m_japanFont(NULL),
    m_chinaFont(NULL),
    m_taiwanFont(NULL)
{
  // 初期化
  {
    for(u32 i=0; i<LYT_WK_ID_NUM; ++i)
    {
      m_waitAnimIndex[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
    }
  }

  // メモリ
  m_qrDevMem = GFL_CREATE_LOCAL_HEAP(m_param.appHeap->GetDeviceHeap(), QR_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);

  // 十字キー(CrossKey)、ボタン(Button)にスティック（Stick）の操作を加えない
  this->SetStickEmulationMode(false);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ


  // 言語
  {
    m_gameLang = ZukanType::ConvertPokemonLang2ZukanLang(System::GetLang());

    /*
    ZukanTool::LangMsgDataManagerに任せたのでコメントアウト
    for(u32 i=0; i<ZukanTool::MSG_TYPE_NUM_FOR_INFO; ++i)
    {
      for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
      {
        if(m_gameLang != j)
        {
          m_param.tool->CreateMsgData(static_cast<ZukanTool::MsgType>(i), NULL, m_param.appHeap->GetDeviceHeap(), static_cast<ZukanType::ZukanLang>(j));  // 同期読み込み
        }
      }
    }
    */
  }

  // フォント
  {
    this->createLangFont();
  }

  // ポケアイコン
  m_pokeIconTexManager = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPokeIconTextureManager(
      m_param.appHeap, m_param.dataManager, 1 );

  // ZukanViewInfoListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewInfoListener(this);
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
        // LYT_WK_ID_LOWER  wk_0
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
        // LYT_WK_ID_UPPER  wk_1
        {
          // 上画面
          0,  // res_0
          LYTRES_INFO_ZUKAN_OTR_UPP_000_BFLYT,
          LA_INFO_ZUKAN_OTR_UPP_000___NUM,
          buf,
          true,
          app::util::G2DUtil::SETUP_UPPER,
          true
        },
      };
     
      this->Create2D(  // 親であるapp::ui::UIViewのメンバ
          m_param.appHeap,
          m_param.lytSys,
          LYT_WK_ID_NUM,  // wk_0の1個のみ 
          resTbl,
          GFL_NELEMS(resTbl), 
          wkTbl,
          GFL_NELEMS(wkTbl), 
          app::ui::UIView::UNUSE_MESSAGE_DATA
      );

      // ページ内容生成
      this->createPageOnce();

      // QRコード表示クラスの生成
      m_QRDisplay = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanQRDisplay(m_qrDevMem, m_QRPicPane);
      m_QRDisplay->SetVisible(true);  // 親ペインで表示非表示は制御するので、ピクチャペインは常に表示でよい。(生成中の非表示はZukanQRDisplayがやってくれる)

      // ボタンマネージャ
      this->createButtonManager(); 
     
      // スクロールバー
      this->createScrollBar();
      
      this->createPageForMode();
      this->setCurrDispPoke();

      // インアニメ
      this->startInSeq();
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
    }
  }

  //m_param.tool->SetPokeModelVisible(true);
  //m_param.tool->ChangePokeModel(m_param.contents->GetCurrDispPokeData()->sp);はsetCurrDispPoke内で呼んでいる
}
ZukanViewInfo::~ZukanViewInfo()
{
  // ポケモンモデル表示に合わせた鳴き声再生は「情報画面説明ページ」のみ
  m_param.tool->SetPokeModelVoiceFlag(false);  // @fix GFNMCat[5243] アローラ図鑑でリストからポケモンの詳細画面に入った時に鳴き声がなっていません。  →鳴らすフラグを下げる

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // スクロールバー
  this->destroyScrollBar();

  // ボタンマネージャ
  this->destroyButtonManager();

  // QRコード表示クラスの破棄
  GFL_SAFE_DELETE(m_QRDisplay);

  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewInfoListener
  GFL_SAFE_DELETE(m_viewListener);

  // ポケアイコン
  GFL_SAFE_DELETE(m_pokeIconTexManager);

  // フォント
  this->destroyLangFont();

  // 言語
  /*
  ZukanTool::LangMsgDataManagerに任せたのでコメントアウト
  for(u32 i=0; i<ZukanTool::MSG_TYPE_NUM_FOR_INFO; ++i)
  {
    for(s32 j=0; j<ZukanType::ZUKAN_LANG_NUM; ++j)
    {
      if(m_gameLang != j)
      {
        m_param.tool->DestroyMsgData(static_cast<ZukanTool::MsgType>(i), static_cast<ZukanType::ZukanLang>(j));
      }
    }
  }
  */

  // メモリ
  GFL_DELETE_HEAP(m_qrDevMem);
  m_qrDevMem = NULL;
}

void ZukanViewInfo::Update(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  bool pokeModelControlEnable = false;  // ポケモンモデル操作

  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_IN:
    {
      if( pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
      {
        if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
        {

          // 生成が間に合っていなかったら、ここで全部生成する。
          { 
            ZukanTool::LangMsgDataManager* langMsgDataManager = m_param.tool->GetLangMsgDataManager();
            while(!langMsgDataManager->IsCreated())
            {
              langMsgDataManager->UpdateCreate();
            }
            ZukanTool::LangFontManager* langFontManager = m_param.tool->GetLangFontManager();
            while(!langFontManager->IsCreated())
            {
              langFontManager->UpdateCreate();
            }

            m_japanFont  = m_param.tool->GetLangFontManager()->GetLangFont(ZukanTool::LangFontManager::LANG_FONT_TYPE_JAPAN );
            m_chinaFont  = m_param.tool->GetLangFontManager()->GetLangFont(ZukanTool::LangFontManager::LANG_FONT_TYPE_CHINA );
            m_taiwanFont = m_param.tool->GetLangFontManager()->GetLangFont(ZukanTool::LangFontManager::LANG_FONT_TYPE_TAIWAN);
          }

          // ポケモンモデル表示に合わせた鳴き声再生は「情報画面説明ページ」のみ
          m_param.tool->SetPokeModelVoiceFlag(true);  // @fix GFNMCat[5243] アローラ図鑑でリストからポケモンの詳細画面に入った時に鳴き声がなっていません。  →鳴らすフラグを立てる
          ZukanUtil::SetPokeModelVisible(m_param.tool, true);
          this->changeDispState(DISP_STATE_ON_WORK);
          m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_ENABLE);
        }
      }
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      pokeModelControlEnable = true;
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        //if(*m_param.nextSceneType == ZukanType::SCENE_TYPE_ALOLA_TOP || *m_param.nextSceneType == ZukanType::SCENE_TYPE_ISLAND_TOP)  // ポケモンモデル不要or場所変更のため、一旦消す。→どこへ遷移するときもポケモンモデルを初期化する
        {
          // QRコードを表示していてポケモンモデルの位置が変わっているかもしれないので、位置を戻す
          m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_CENTER);
          m_param.tool->ResetPokeModel();  // ポケモンモデルリセット
          ZukanUtil::SetPokeModelVisible(m_param.tool, false);
        }
        {
          if( *m_param.nextSceneType == ZukanType::SCENE_TYPE_ALOLA_TOP )
          {
            if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
            {
              pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す
            }
          }

          // QRコード表示クラス
          if( !(m_QRDisplay->IsCreated()) ) m_QRDisplay->SetVisible(false);  // もう終了するので、生成中のものは表示しない
         
          this->changeDispState(DISP_STATE_ON_FADE_OUT);
        }
      }
      break;
    }
  case DISP_STATE_ON_MODE_IN:
    {
      pokeModelControlEnable = true;
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        this->changeDispState(DISP_STATE_ON_WORK);
        m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_ENABLE);
      }
      break;
    }
  case DISP_STATE_ON_MODE_OUT:
    {
      pokeModelControlEnable = true;
      if( ZukanUtil::IsLytWaitAnimEnd(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM) )
      {
        // 前の画面
        this->destroyButtonManager();
        // 前→次
        m_modeIsFormVoice = !m_modeIsFormVoice;
        // ポケモンモデル表示に合わせた鳴き声再生は「情報画面説明ページ」のみ
        m_param.tool->SetPokeModelVoiceFlag(!m_modeIsFormVoice);  // @fix GFNMCat[5243] アローラ図鑑でリストからポケモンの詳細画面に入った時に鳴き声がなっていません。  →鳴らすフラグを適切に設定する
        // 次の画面
        this->createButtonManager();
        this->createPageForMode();
        m_modeChanging = true;
        this->setCurrDispPoke();
        m_modeChanging = false;

        this->startModeInSeq();
      }
      break;
    }
  case DISP_STATE_ON_FADE_OUT:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        this->changeDispState(DISP_STATE_WAIT_SAFETY);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      // QRコード表示クラスが破棄できるようになるのを待つ
      if( m_QRDisplay->CanDestroy() )
      {
        m_param.frameCell->Req(ZukanFrameInfoCell::REQ_ID_END);
        this->changeDispState(DISP_STATE_WAIT_END);
      }
      break;
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }

  // ポケモンモデル操作
  if(pokeModelControlEnable)
  {
    //m_infoFunc.updatePokeModelController(m_modeIsFormVoice == false);  // LRズーム、3Dパッド回転。このシーンでは十字キーと3Dパッドを切り離しておく。
    m_param.tool->UpdatePokeModelController(m_modeIsFormVoice == false);  // LRズーム、3Dパッド回転。このシーンでは十字キーと3Dパッドを切り離しておく。

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    if(m_modeIsFormVoice)
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
  } 

  // QRコード表示クラス
  m_QRDisplay->Update();

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanViewInfo::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_LOWER, gfl2::lyt::ORDER_NORMAL, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, LYT_WK_ID_UPPER, gfl2::lyt::ORDER_NORMAL, 1 );  // wk_1
}

void ZukanViewInfo::Req(ReqId reqId, s32 reqParam)
{
  if(m_modeIsFormVoice)
  {
    switch(reqId)
    {
    case REQ_ID_BUTTON_DECIDE:
      {
        switch(reqParam)
        {
        case BUTTON_ID_FORMVOICE_PREV_MONS:
          {
            m_param.contents->DecreaseCurrIndexOfDispPokeDataIndexListLimitedGet();
            this->setCurrDispPoke();
            *m_param.alolaTopSceneChangeFlag = true;
            break;
          }
        case BUTTON_ID_FORMVOICE_NEXT_MONS:
          {
            m_param.contents->IncreaseCurrIndexOfDispPokeDataIndexListLimitedGet();
            this->setCurrDispPoke();
            *m_param.alolaTopSceneChangeFlag = true;
            break;
          }
        case BUTTON_ID_FORMVOICE_PREV_FORM:
          {
            m_param.contents->DecreaseCurrDispDrawFigureDataIndex();
            this->setCurrDispForm();
            break;
          }
        case BUTTON_ID_FORMVOICE_NEXT_FORM:
          {
            m_param.contents->IncreaseCurrDispDrawFigureDataIndex();
            this->setCurrDispForm();
            break;
          }
        case BUTTON_ID_FORMVOICE_BACK:
          {
            this->startModeOutSeq();
            break;
          }
        case BUTTON_ID_FORMVOICE_VOICE:
          {
            this->playVoice();
            break;
          }
        case BUTTON_ID_FORMVOICE_MOTION:
          {
            this->changeMotion();
            break;
          }
        case BUTTON_ID_FORMVOICE_PAUSE:
          {
            this->switchPause();
            break;
          }
        }
        break;
      }
      case REQ_ID_BUTTON_TOUCH : break;  // 何もしない
    }
  }
  else
  {
    const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
    bool currPokeGetFlag = pokeData->getFlag;
    
    bool setCurrDispFormFlag = false;  // 容姿変更フラグ
    u32  setCurrDispFormBeforeIndex = m_param.contents->GetCurrDispDrawFigureDataIndex();
  
    bool setCurrLangFlag = false;  // 言語変更フラグ

    switch(reqId)
    {
    case REQ_ID_KEY_TRIGGER_L: setCurrLangFlag = this->decreaseCurrLangIndex(true ); break;
    case REQ_ID_KEY_TRIGGER_R: setCurrLangFlag = this->increaseCurrLangIndex(true ); break;
    case REQ_ID_KEY_REPEAT_L:  setCurrLangFlag = this->decreaseCurrLangIndex(false); break;
    case REQ_ID_KEY_REPEAT_R:  setCurrLangFlag = this->increaseCurrLangIndex(false); break;

    case REQ_ID_KEY_TRIGGER_LEFT:
      {
        if(currPokeGetFlag)
        {
          m_param.contents->DecreaseCurrDispDrawFigureDataIndex(true);
          setCurrDispFormFlag = true;
        }
        break;
      }
    case REQ_ID_KEY_TRIGGER_RIGHT:
      {
        if(currPokeGetFlag)
        {
          m_param.contents->IncreaseCurrDispDrawFigureDataIndex(true);
          setCurrDispFormFlag = true;
        }
        break;
      }
    case REQ_ID_KEY_REPEAT_LEFT:
      {
        if(currPokeGetFlag)
        {
          m_param.contents->DecreaseCurrDispDrawFigureDataIndex(false);
          setCurrDispFormFlag = true;
        }
        break;
      }
    case REQ_ID_KEY_REPEAT_RIGHT:
      {
        if(currPokeGetFlag)
        {
          m_param.contents->IncreaseCurrDispDrawFigureDataIndex(false);
          setCurrDispFormFlag = true;
        }
        break;
      }
    case REQ_ID_KEY_TRIGGER_X:
      {
        this->setQRCodeDisp(!m_isQRCodeDisp);
      
        // @fix NMCat[2023] Xボタンを押してQRコードを表示/非表示にする際にSEが鳴らないことについて  →SEを入れた。
        // SE
        { 
          Sound::PlayInfo playInfo;
          playInfo.effect_enable |= Sound::EFFECT_BIT_PITCH;
          playInfo.pitch = 2.0f;
          if(m_isQRCodeDisp)
          {
            // 開くときの音
//            Sound::PlaySE(SEQ_SE_SYS_NJ_033, playInfo);
            if(m_param.pRotomToneGenerator)
            {
              const PokeTool::SimpleParam& rSimpleParam = m_param.contents->GetCurrDispPokeSimpleParam();

              /**
                MMcat[614]   図鑑上のトーン音信号の再生で、マップ上のBGMと干渉し認識率が大きく下がってしまう。
                  音量調整機能を実装
              */
              m_param.pRotomToneGenerator->SetBGMVolume(0.2f, Sound::BGM_VOLUME_ZUKAN);
              m_param.pRotomToneGenerator->SetBGMVolumeEnable(true);
              m_param.pRotomToneGenerator->Request(rSimpleParam.monsNo, rSimpleParam.formNo, rSimpleParam.sex);
            }
          }
          else
          {
            // 閉じるときの音
            Sound::PlaySE(SEQ_SE_SYS_NJ_034, playInfo);
          }
        }
        break;
      }

    case REQ_ID_BUTTON_DECIDE:
      {
        switch(reqParam)
        {
        case BUTTON_ID_EXPLAIN_LANG00:
        case BUTTON_ID_EXPLAIN_LANG01:
        case BUTTON_ID_EXPLAIN_LANG02:
        case BUTTON_ID_EXPLAIN_LANG03:
        case BUTTON_ID_EXPLAIN_LANG04:
        case BUTTON_ID_EXPLAIN_LANG05:
        case BUTTON_ID_EXPLAIN_LANG06:
        case BUTTON_ID_EXPLAIN_LANG07:
        case BUTTON_ID_EXPLAIN_LANG08:
          {
            this->setCurrLangIndex(reqParam);
            this->setCurrLang();
            break;
          }
        case BUTTON_ID_EXPLAIN_PREV_MONS:
          {
            m_param.contents->DecreaseCurrIndexOfDispPokeDataIndexListLimitedSee();
            this->setCurrDispPoke();
            *m_param.alolaTopSceneChangeFlag = true;  // 分布図で変えられることもあるのでは？DispIndexを覚えておかねばならないかも。
            break;
          }
        case BUTTON_ID_EXPLAIN_NEXT_MONS:
          {
            m_param.contents->IncreaseCurrIndexOfDispPokeDataIndexListLimitedSee();
            this->setCurrDispPoke();
            *m_param.alolaTopSceneChangeFlag = true;
            break;
          }
        case BUTTON_ID_EXPLAIN_MAP:
          {
            *m_param.nextSceneType = ZukanType::SCENE_TYPE_MAP;
            this->startOutSeq();
            break;
          }
        case BUTTON_ID_EXPLAIN_FORMVOICE:
          {
            this->startModeOutSeq();
            break;
          }
        case BUTTON_ID_EXPLAIN_BACK:
          {
            if(m_param.contents->GetInfoType() == ZukanType::INFO_TYPE_ALOLA)
            {
              *m_param.nextSceneType = ZukanType::SCENE_TYPE_ALOLA_TOP;
            }
            else
            {
              *m_param.nextSceneType = ZukanType::SCENE_TYPE_ISLAND_TOP;
            }
            this->startOutSeq();
            break;
          }
        //case BUTTON_ID_EXPLAIN_QR:
        //  {
        //    this->setQRCodeDisp(!m_isQRCodeDisp);
        //    break;
        //  }
        }
      }
      break;
    case REQ_ID_BUTTON_TOUCH : break;  // 何もしない
    }

    // 容姿変更フラグが立っていたら容姿を変更する
    if(setCurrDispFormFlag)
    {
      u32  setCurrDispFormAfterIndex = m_param.contents->GetCurrDispDrawFigureDataIndex();
      if( setCurrDispFormBeforeIndex != setCurrDispFormAfterIndex )
      {
        Sound::PlaySE( SEQ_SE_SELECT1 );
        this->setCurrDispForm();
      }
    }
   
    // 言語変更フラグが立っていたら言語を変更する
    if( setCurrLangFlag )
    {
      Sound::PlaySE( SEQ_SE_SELECT1 );
      this->setCurrLang();
    }

  }
}

void ZukanViewInfo::createButtonManager(void)
{ 
  if(m_modeIsFormVoice)
  {
    u32                               BUTTON_ID_NUM     = BUTTON_ID_FORMVOICE_NUM;
    const ZukanType::ButtonConstInfo* BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_FORMVOICE;

    m_buttonPaneInfo = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()) ZukanType::ButtonPaneInfo[BUTTON_ID_NUM];

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

    ////////////////////////////
    // フォルム切り替えボタン
    ////////////////////////////
    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    buttonManager->SetButtonActiveForce(BUTTON_ID_FORMVOICE_PREV_FORM);  // @fix NMCat[1480] ポケモンの姿を切り替える左右の矢印アイコンの見た目が非アクティブの状態になる
    buttonManager->SetButtonActiveForce(BUTTON_ID_FORMVOICE_NEXT_FORM);  //      →「見た目パッシブ＆内部アクティブ」で始まることがあるので、「見た目アクティブ＆内部アクティブ」になるよう初期化する。

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    this->unbindKeyForPokeModelSRTModifyMode();
#endif
  }
  else
  {
    gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
    //gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
    
    u32                               BUTTON_ID_NUM     = BUTTON_ID_EXPLAIN_NUM;
    const ZukanType::ButtonConstInfo* BUTTON_CONST_INFO = BUTTON_CONST_INFO_FOR_EXPLAIN;
    
    m_buttonPaneInfo = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()) ZukanType::ButtonPaneInfo[BUTTON_ID_NUM];

    gfl2::lyt::LytWk** lytWkArray = GFL_NEW_ARRAY(m_param.appHeap->GetDeviceHeap()->GetLowerHandle()) gfl2::lyt::LytWk*[BUTTON_ID_NUM];  // 一時的に確保  // @fix cov_ctr[10961]: 配列として確保し、配列として解放する。
    for(u32 i=0; i<BUTTON_ID_NUM; ++i)
    {
      //if(i == BUTTON_ID_EXPLAIN_QR)
      //{
      //  lytWkArray[i] = lytWk_1;
      //}
      //else
      {
        lytWkArray[i] = lytWk_0;
      }
    }

    ZukanUtil::CreateButtonManager(
        m_param.appHeap,
        this,
        0,  // res_0
        ZUKAN_CONST_G2DUTIL_LYT_WK_ID_NULL,
        BUTTON_ID_NUM,
        BUTTON_CONST_INFO,
        lytWkArray,
        m_buttonPaneInfo
    );
    
    GFL_SAFE_DELETE_ARRAY(lytWkArray);
  }
}
void ZukanViewInfo::destroyButtonManager(void)
{
  ZukanUtil::DestroyButtonManager(this);
  GFL_SAFE_DELETE_ARRAY(m_buttonPaneInfo);
}

void ZukanViewInfo::createScrollBar(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  gfl2::lyt::LytParts* scrollBarPartsPane = lytWk_0->GetPartsPane(SCROLL_BAR_CONST_INFO.parts_index);
  m_scrollBar = GFL_NEW(m_param.appHeap->GetDeviceHeap()) app::tool::ScrollBar(
    scrollBarPartsPane,
    lytWk_0->GetPane(scrollBarPartsPane, SCROLL_BAR_CONST_INFO.mark_index, &lytRes_0),
    lytWk_0->GetPane(scrollBarPartsPane, SCROLL_BAR_CONST_INFO.top_index, &lytRes_0),
    lytWk_0->GetPane(scrollBarPartsPane, SCROLL_BAR_CONST_INFO.bottom_index, &lytRes_0),
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL  // スクロールバー部品を配置するときに90度回転して横向きにしているので、縦扱いでいい。
  );
}
void ZukanViewInfo::destroyScrollBar(void)
{
  GFL_SAFE_DELETE(m_scrollBar);
}

void ZukanViewInfo::startInSeq(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
  
  switch(m_param.prevSceneType)
  {
  case ZukanType::SCENE_TYPE_MAP:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_BACK;
    break;
  case ZukanType::SCENE_TYPE_ALOLA_TOP:
  case ZukanType::SCENE_TYPE_ISLAND_TOP:
  default:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_IN;
    break;
  }
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_IN);

  if( m_param.prevSceneType == ZukanType::SCENE_TYPE_ALOLA_TOP )
  {
    if( ZukanType::IsSubjectPaneListPosInfoReset(m_param.contents->GetPaneListPosInfoOfSearchSubject()) )
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS);
    }
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_OCCUPY);
}
void ZukanViewInfo::startOutSeq(void)
{
  switch(*m_param.nextSceneType)
  {
  case ZukanType::SCENE_TYPE_MAP:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_NEXT;
    break;
  case ZukanType::SCENE_TYPE_ALOLA_TOP:
  case ZukanType::SCENE_TYPE_ISLAND_TOP:
  default:
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_RETURN;
    break;
  }
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_OUT);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_OCCUPY);
}

void ZukanViewInfo::startModeInSeq(void)
{
  if(m_modeIsFormVoice)
  {
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_IN_CRIES;
  }
  else
  {
    m_waitAnimIndex[LYT_WK_ID_LOWER] = LA_INFO_ZUKAN_MIN_LOW_002_RETURN_CRIES;
  }
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_MODE_IN);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_OCCUPY);
}
void ZukanViewInfo::startModeOutSeq(void)
{
  m_waitAnimIndex[LYT_WK_ID_LOWER] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
  ZukanUtil::StartLytWaitAnim(this->GetG2DUtil(), m_waitAnimIndex, LYT_WK_ID_NUM);  // 親であるapp::ui::UIViewのメンバ
  this->changeDispState(DISP_STATE_ON_MODE_OUT);

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewInfoListener::INPUT_STATE_OCCUPY);
}

void ZukanViewInfo::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}

void ZukanViewInfo::createPageOnce(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytWk*        lytWk_0  = this->GetLayoutWork( LYT_WK_ID_LOWER );  // wk_0  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytWk*        lytWk_1  = this->GetLayoutWork( LYT_WK_ID_UPPER );  // wk_1  // 親であるapp::ui::UIViewのメンバ関数
  gfl2::lyt::LytMultiResID lytRes_0 = pG2DUtil->GetLayoutResourceID( 0 );  // res_0

  ////////////////////////////////////////////////////////////
  // ペインのポインタを覚えておく
  ////////////////////////////////////////////////////////////
  m_infoFunc.SetPointer(lytWk_0, lytRes_0); 
  //m_infoFunc.setupPokeModelController(m_param.tool->GetPokeSimpleModelInFrame(), 200, 120, 220, 1.0f);
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_CENTER);

  // QRコード
  m_QRButtonPane  = lytWk_1->GetPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_BTN_X);
  m_QRLytTopPane  = lytWk_1->GetPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_LYTTOP);
  m_QRDispTopPane = lytWk_1->GetPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_DISPTOP);
  m_NoQRTopPane   = lytWk_1->GetPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_NOQRTOP);
  m_NoQRTextPane  = lytWk_1->GetTextBoxPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_NOQRTEXT);
  m_QRTopPane     = lytWk_1->GetPane(PANENAME_ZUKAN_OTR_UPP_000_PANE_QRTOP);
  m_QRPicPane     = lytWk_1->GetPicturePane(PANENAME_ZUKAN_OTR_UPP_000_PANE_QRDUMMY);
  this->setQRCodeDisp(m_isQRCodeDisp);  // QRコードの表示を初期化する

  // ポケアイコンのペインのポインタを覚えておく(情報画面の説明ページ)
  gfl2::lyt::LytParts* pokeIconPartsPaneForExplain = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_INFO_POKEMON_ICON);
  m_pokeIconPicPaneForExplain = lytWk_0->GetPicturePane(pokeIconPartsPaneForExplain, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &lytRes_0);
  m_rarePaneForExplain        = lytWk_0->GetPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_STAR_ICON);

  // ポケアイコンのペインのポインタを覚えておく(情報画面の姿＆鳴き声ページ)
  gfl2::lyt::LytParts* pokeIconPartsPaneForFormVoice = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_POKEMON_ICON);
  m_pokeIconPicPaneForFormVoice = lytWk_0->GetPicturePane(pokeIconPartsPaneForFormVoice, PANENAME_COMMON_BTN_LOW_001_PANE_POKE_ICON, &lytRes_0);
  m_rarePaneForFormVoice        = lytWk_0->GetPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_STAR_ICON_00);

  ////////////////////////////////////////////////////////////
  // 説明ページ
  ////////////////////////////////////////////////////////////
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // ぶんぷボタン
    gfl2::lyt::LytParts* mapButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_Y4_INFO);
    gfl2::lyt::LytTextBox* mapButtonTextPane = lytWk_0->GetTextBoxPane(mapButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(mapButtonTextPane, msg_zkn_06_01);
    // 姿＆鳴き声ボタン
    gfl2::lyt::LytParts* voiceButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_A8);
    gfl2::lyt::LytTextBox* voiceButtonTextPane = lytWk_0->GetTextBoxPane(voiceButtonPartsPane, PANENAME_COMMON_BTN_LOW_019_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(voiceButtonTextPane, msg_zkn_06_02);
    // QRコードがないとき
    pG2DUtil->SetTextBoxPaneString(m_NoQRTextPane, msg_zkn_12_01);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
  

  // 言語アイコン
  {
    // 言語パーツペイン
    static const u32 LANG_PARTS_PANE[BUTTON_ID_EXPLAIN_LANG_NUM] =
    {
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_00,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_01,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_02,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_03,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_04,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_05,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_06,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_07,
      PANENAME_ZUKAN_MIN_LOW_002_PANE_TAB_08,
    };

    u32 j = 0;
    for(u32 i=BUTTON_ID_EXPLAIN_LANG_START; i<=BUTTON_ID_EXPLAIN_LANG_END; ++i)
    {
      gfl2::lyt::LytParts*   langPartsPane = lytWk_0->GetPartsPane( LANG_PARTS_PANE[ i - BUTTON_ID_EXPLAIN_LANG_START ] );
          // ボタンマネージャ生成より後にここに来るならボタンマネージャからペインが取得できるのだが、
          // ボタンマネージャ生成より前にここに来てしまうからそれはできない。
      gfl2::lyt::LytPicture* langPicPane   = lytWk_0->GetPicturePane( langPartsPane, PANENAME_ZUKAN_BTN_LOW_003_PANE_JPN, &lytRes_0 );
      //m_param.tool->ReplaceLangIconTextureByCommonGrpIcon(langPicPane, ZUKAN_LANG_ORDER_TABLE[m_gameLang][j]);
      this->replaceLangIconTexture(langPicPane, ZUKAN_LANG_ORDER_TABLE[m_gameLang][j]);
      ++j;
    }
  }


  ////////////////////////////////////////////////////////////
  // 姿＆鳴き声ページ
  ////////////////////////////////////////////////////////////
  pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
  {
    // なきごえボタン
    gfl2::lyt::LytParts* voiceButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_Y4_CRIES);
    gfl2::lyt::LytTextBox* voiceButtonTextPane = lytWk_0->GetTextBoxPane(voiceButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(voiceButtonTextPane, msg_zkn_09_03);
    
    // うごきボタン
    gfl2::lyt::LytParts* motionButtonPartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_BUTTON_X4);
    gfl2::lyt::LytTextBox* motionButtonTextPane = lytWk_0->GetTextBoxPane(motionButtonPartsPane, PANENAME_COMMON_BTN_LOW_013_PANE_TEXTBOX_00, &lytRes_0);
    pG2DUtil->SetTextBoxPaneString(motionButtonTextPane, msg_zkn_09_04);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
}

void ZukanViewInfo::createPageForMode(void)
{
  if(m_modeIsFormVoice)
  {
    m_QRLytTopPane->SetVisible(false);
    this->setQRCodeDisp(false);  // 姿＆鳴き声ページにいったら、QRコードは非表示にしておく
  }
  else
  {
    m_QRLytTopPane->SetVisible(true);
  }

  this->setCurrButtonStateForPage();
}

void ZukanViewInfo::setCurrDispPoke(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  const ZukanContents::PokeData* pokeData            = m_param.contents->GetCurrDispPokeData();


  // 表示する言語
  if(m_modeIsFormVoice)
  {
    // @fix GFNMCat[1211] 図鑑で姿＆鳴き声画面に移行したときに、自国語の言語に戻らない
    //                    自国語の言語に戻すようにした。

    // m_gameLangの言語[m_gameLang][0]にする
    m_currLangIndex = 0;
    // ユーザが操作した結果、姿＆鳴き声ページになっているので、ユーザが選択したことにしておく。
    m_userLangIndex = m_currLangIndex;
  }
  else
  {
    u32 langBitFlag = 0;
    pokeData->GetGetPokemonLang(&langBitFlag, m_gameLang);
 
    u32 pokemonLang = ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_userLangIndex];  // POKEMON_LANG_???
    if( langBitFlag & (1 << pokemonLang) )  // ユーザが選択した言語がある
    {
      m_currLangIndex = m_userLangIndex;
    }
    else  // ユーザが選択した言語がない→m_gameLangの言語[m_gameLang][0]にする
    {
      m_currLangIndex = 0;
      {
        // [m_gameLang][0]は必ずある言語のはず。その確認を行う。
        pokemonLang = ZUKAN_LANG_ORDER_TABLE[m_gameLang][0];  // POKEMON_LANG_???
        GFL_ASSERT( langBitFlag & (1 << pokemonLang) );  // 開発中に気付かせるためのASSERT。
      }
    }
  }


  // QRコード
  if(!m_modeIsFormVoice)
  {
    bool notExistBecauseOfNoQR = false;
    if( pokeData->DoesQRCodeExist(&notExistBecauseOfNoQR) )
    {
      m_NoQRTopPane->SetVisible(false);
      m_QRTopPane->SetVisible(true);
    }
    else
    {
      // QRコードがない理由
      pG2DUtil->SetMessageData( m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN), m_param.tool->GetWordSet() );
      {
        if( notExistBecauseOfNoQR )
        {
          // そもそもQRコードがないとき
          pG2DUtil->SetTextBoxPaneString(m_NoQRTextPane, msg_zkn_12_02);
        }
        else
        {
          // 捕まえていなくてQRコードがないとき
          pG2DUtil->SetTextBoxPaneString(m_NoQRTextPane, msg_zkn_12_01);
        }
      }
      pG2DUtil->SetMessageData(NULL, NULL);

      m_NoQRTopPane->SetVisible(true);
      m_QRTopPane->SetVisible(false);
    }
  }


  // モンスターボールのマーク
  s32 frame = 0;
  if(pokeData->getFlag != 0) frame = 2;
  else if(pokeData->figureSeeNum > 0) frame = 1;
  ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_INFO_ZUKAN_MIN_LOW_002_STATUS, frame);


  // ボタンの反応モードを変更する
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  if(m_modeIsFormVoice)
  {
    buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_PREV_MONS, gfl2::ui::BUTTON_UP,
        (m_param.contents->IsCurrIndexOfDispPokeDataIndexListHeadLimitedGet())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );
    buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_NEXT_MONS, gfl2::ui::BUTTON_DOWN,
        (m_param.contents->IsCurrIndexOfDispPokeDataIndexListTailLimitedGet())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    this->unbindKeyForPokeModelSRTModifyMode();
#endif
  }
  else 
  {
    buttonManager->SetButtonBindKey(BUTTON_ID_EXPLAIN_PREV_MONS, gfl2::ui::BUTTON_UP,
        (m_param.contents->IsCurrIndexOfDispPokeDataIndexListHeadLimitedSee())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );
    buttonManager->SetButtonBindKey(BUTTON_ID_EXPLAIN_NEXT_MONS, gfl2::ui::BUTTON_DOWN,
        (m_param.contents->IsCurrIndexOfDispPokeDataIndexListTailLimitedSee())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );
  } 


  this->setCurrButtonStateForDispPoke();

  this->setCurrDispForm();
}

void ZukanViewInfo::setCurrDispForm(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ


  const ZukanContents::PokeData* pokeData            = m_param.contents->GetCurrDispPokeData();
  u32                            drawFigureDataIndex = m_param.contents->GetCurrDispDrawFigureDataIndex();
  PokeTool::SimpleParam          sp                  = m_param.contents->GetCurrDispPokeSimpleParam();

  const ZukanContents::PokeData::FigureData* figureData = &(pokeData->figureData[drawFigureDataIndex]);


  // フォルムのスクロールバー
  f32 scrollBarSize  = 0.0f;
  f32 scrollBarVal   = 0.0f;
  u8  scrollBarAlpha = 255;
  if(pokeData->figureSeeNum > 1)
  {
    if(figureData->seeNo == 0)
    {
      scrollBarVal = 0.0f;
    }
    else if(figureData->seeNo == pokeData->figureSeeNum-1)
    {
      scrollBarVal = 1.0f;
    }
    else
    {
      scrollBarVal = (static_cast<f32>(figureData->seeNo)) / (pokeData->figureSeeNum-1);
    }
  }
  scrollBarSize = pokeData->figureSeeNum;
  m_scrollBar->SetMarkSize(scrollBarSize);
  m_scrollBar->Draw(scrollBarVal, scrollBarAlpha);


  // ポケモンアイコン
  {
    m_pokeIconTexManager->CreatePokeIconTexture2(
        0,
        m_pokeIconPicPaneForExplain,
        0,
        0,
        m_pokeIconPicPaneForFormVoice,
        0,
        0,
        sp,
        false
    );

    // @fix GFNMCat[1323] 図鑑で見ただけのポケモンのアイコンの色が暗くなっていないところがある  →対処した。
    // 捕まえた、見たフラグ
    if(pokeData->figureSeeNum>0)  // ポケアイコンのキラキラは止める。情報ページなので★が出ているから。
    {
      nw::ut::Color8 vtxColor = ZukanType::POKE_GET_COLOR;
      if(pokeData->getFlag==0)
      {
        vtxColor = ZukanType::POKE_SEE_COLOR;
      }
      for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx) m_pokeIconPicPaneForExplain->SetVtxColor(vtxIdx, vtxColor);  // 見ただけのときはForFormVoiceには行かないからこちらだけでいい。
    }


    // 性別
    // 上段=オス
    // 下段=メス
    // というふうに位置を固定して表示する

#define A1M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_00_ICON_MALE    // 下段
#define A1F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_00_ICON_FEMALE
#define A0M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_01_ICON_MALE    // 上段
#define A0F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_01_ICON_FEMALE
#define B1M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_02_ICON_MALE    // 下段
#define B1F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_02_ICON_FEMALE
#define B0M LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_03_ICON_MALE    // 上段
#define B0F LA_INFO_ZUKAN_MIN_LOW_002__PARTS_GENDER_03_ICON_FEMALE

    s32 frame=0;
    u32 animA0=A0M;  // 上段
    u32 animA1=A1M;  // 下段
    u32 animB0=B0M;  // 上段
    u32 animB1=B1M;  // 下段

    switch(figureData->figureGender)
    {
    case ZukanContents::PokeData::FIGURE_GENDER_MALE:        frame=3; animA0=A0M; animA1=A1M; animB0=B0M; animB1=B1M; break;
    case ZukanContents::PokeData::FIGURE_GENDER_FEMALE:      frame=1; animA0=A0F; animA1=A1F; animB0=B0F; animB1=B1F; break;
    case ZukanContents::PokeData::FIGURE_GENDER_UNKNOWN:     frame=0; animA0=A0M; animA1=A1M; animB0=B0M; animB1=B1M; break;
    case ZukanContents::PokeData::FIGURE_GENDER_MALE_FEMALE: frame=2; animA0=A0M; animA1=A1F; animB0=B0M; animB1=B1F; break;
    }

    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, LA_INFO_ZUKAN_MIN_LOW_002_GENDER, frame);
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A0M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A0F, false);  // ループアニメなので、UnBindするまでアニメはくっついたままになっている。
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A1M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, A1F, false);  // オスとメスの両方のアニメがくっついていて再生されているとき、
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, B0M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, B0F, false);  // 片方の結果が(今回はメスのほうの結果が)表示されてしまう。
    pG2DUtil->StopAnime(LYT_WK_ID_LOWER, B1M, false);  pG2DUtil->StopAnime(LYT_WK_ID_LOWER, B1F, false);  // だから、アニメを設定し直す前に、オスとメスの両方のアニメを一度UnBindしておく。
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animA0, 0);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animA1, 0);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animB0, 0);
    ZukanUtil::BindLytAnim(pG2DUtil, LYT_WK_ID_LOWER, animB1, 0);

#undef A0M
#undef A0F
#undef A1M
#undef A1F
#undef B0M
#undef B0F
#undef B1M
#undef B1F


    // レア
    m_rarePaneForExplain->SetVisible(figureData->rareFlag != 0);
    m_rarePaneForFormVoice->SetVisible(figureData->rareFlag != 0);
  }


  // ポケモンモデル
  m_param.tool->ChangePokeModel(sp);
  //m_infoFunc.setPokeModelControllerTarget(sp);


  // QRコード
  bool notExistBecauseOfNoQR = false;
  if( pokeData->DoesQRCodeExist(&notExistBecauseOfNoQR) )
  {
    u32 perRand[NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX] = { 0 };
    for(u32 j=0; j<NetApp::QR::QR_ZUKAN::SEE_FLAG_MAX; ++j)
    {
      u32 figureDataIndex = 0;
      pml::FormNo formNo = 0;
      switch(j)
      {
      case NetApp::QR::QR_ZUKAN::SEE_FLAG_MALE:   figureDataIndex = pokeData->GetFigureDataIndex(formNo, pml::SEX_MALE  , false); break;
      case NetApp::QR::QR_ZUKAN::SEE_FLAG_FEMALE: figureDataIndex = pokeData->GetFigureDataIndex(formNo, pml::SEX_FEMALE, false); break;
      case NetApp::QR::QR_ZUKAN::SEE_FLAG_M_RARE: figureDataIndex = pokeData->GetFigureDataIndex(formNo, pml::SEX_MALE  , true ); break;
      case NetApp::QR::QR_ZUKAN::SEE_FLAG_F_RARE: figureDataIndex = pokeData->GetFigureDataIndex(formNo, pml::SEX_FEMALE, true ); break;
      }
      perRand[j] = pokeData->GetPerRand(figureDataIndex);
    }
    m_QRDisplay->ReqCreateQR(m_param.appHeap->GetDeviceHeap()->GetLowerHandle(), pokeData, sp, figureData->figureGender==ZukanContents::PokeData::FIGURE_GENDER_MALE_FEMALE, perRand);  // ここで使用するメモリはこの関数内で完結している一時メモリ
  }

  // ポケモンモデルが変わったとき
  this->setPokeModelPosByQRCodeDisp(false);


  // セーブデータに設定するのは姿＆鳴き声ページで最後に見ていたフォルム
  if( m_modeIsFormVoice )
  {
    // 閲覧中の容姿データをセーブデータに設定する
    m_param.contents->SetCurrDispDrawFigureDataIndexToSaveData();
  }


  // ボタンの反応モードを変更する
  if(m_modeIsFormVoice)
  {
    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_PREV_FORM, gfl2::ui::BUTTON_LEFT,
        (m_param.contents->IsCurrDispDrawFigureDataIndexHead())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );
    buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_NEXT_FORM, gfl2::ui::BUTTON_RIGHT,
        (m_param.contents->IsCurrDispDrawFigureDataIndexTail())?(app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER):(app::tool::ButtonManager::BIND_KEY_MODE_REPEAT)
    );

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
    this->unbindKeyForPokeModelSRTModifyMode();
#endif
  }


  this->setCurrButtonStateForDispForm();
  
  this->setCurrLang();
}

bool ZukanViewInfo::increaseCurrLangIndex(bool isTrigger)  // インデックスが変わったらtrueを返す
{
  // 2言語以上のときしか処理しないようにすること。m_userLangIndexへの反映も。
  bool changeFlag = false;

  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  u32 langBitFlag = 0;
  pokeData->GetGetPokemonLang(&langBitFlag, m_gameLang);

  s32 langIndex = m_currLangIndex;

  for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM -1; ++i)
  {
    langIndex += 1;
    if(langIndex >= ZukanType::ZUKAN_LANG_NUM)
    {
      if(isTrigger) langIndex = langIndex % ZukanType::ZUKAN_LANG_NUM;
      else          break;
    }

    u32 pokemonLang = ZUKAN_LANG_ORDER_TABLE[m_gameLang][langIndex];  // POKEMON_LANG_???
    if( langBitFlag & (1 << pokemonLang) )
    {
      changeFlag = true;
      break;
    }
  }

  if(changeFlag)
  {
    m_currLangIndex = langIndex;
    // ユーザが操作した結果この関数が呼ばれているので、変わっていたらユーザが選択したことになる。
    m_userLangIndex = m_currLangIndex;
  }
  return changeFlag;
}
bool ZukanViewInfo::decreaseCurrLangIndex(bool isTrigger)  // インデックスが変わったらtrueを返す
{
  // 2言語以上のときしか処理しないようにすること。m_userLangIndexへの反映も。
  bool changeFlag = false;

  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  u32 langBitFlag = 0;
  pokeData->GetGetPokemonLang(&langBitFlag, m_gameLang);

  s32 langIndex = m_currLangIndex;

  for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM -1; ++i)
  {
    langIndex -= 1;
    if(langIndex < 0)
    {
      if(isTrigger) langIndex = (langIndex +ZukanType::ZUKAN_LANG_NUM) % ZukanType::ZUKAN_LANG_NUM;
      else          break;
    }

    u32 pokemonLang = ZUKAN_LANG_ORDER_TABLE[m_gameLang][langIndex];  // POKEMON_LANG_???
    if( langBitFlag & (1 << pokemonLang) )
    {
      changeFlag = true;
      break;
    }
  }

  if(changeFlag)
  {
    m_currLangIndex = langIndex;
    // ユーザが操作した結果この関数が呼ばれているので、変わっていたらユーザが選択したことになる。
    m_userLangIndex = m_currLangIndex;
  }
  return changeFlag;
}
void ZukanViewInfo::setCurrLangIndex(s32 buttonId)
{
  // 2言語以上のときしか処理しないようにすること。m_userLangIndexへの反映も。
  m_currLangIndex = buttonId - BUTTON_ID_EXPLAIN_LANG_START;

  // ユーザが操作した結果この関数が呼ばれているので、変わっていても変わっていなくてもユーザが選択したことになる。
  m_userLangIndex = m_currLangIndex;

  // ↑ボタンマネージャ管理のボタンが押されたときにこの関数が呼ばれるので、ユーザが押したときしかここには来ない。
  //   ユーザが押せたということなので、変わっていなくてもユーザが選択したことにしている。
}

void ZukanViewInfo::setCurrLang(void)
{
  // m_currLangIndexの言語で表示する

  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ

  gfl2::str::MsgData* msgDataZukan        = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN);
  gfl2::str::MsgData* msgDataPokeName     = NULL;
  gfl2::str::MsgData* msgDataPokeSpecies  = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES);
  gfl2::str::MsgData* msgDataPokeForm     = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_FORM);
  gfl2::str::MsgData* msgDataPokeHeight   = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT);
  gfl2::str::MsgData* msgDataPokeWeight   = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT);
  gfl2::str::MsgData* msgDataPokeComment  = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT);
  
  if( m_currLangIndex != 0 )  // m_gameLang以外のときは、この関数内でテキストボックスにセットする際に必要なテキストだけ読み込まれる  // 同期読み込み
  {
    msgDataZukan        = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_ZUKAN        , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeName     = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_NAME    , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeSpecies  = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_SPECIES , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeForm     = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_FORM    , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeHeight   = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_HEIGHT  , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeWeight   = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_WEIGHT  , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
    msgDataPokeComment  = m_param.tool->GetMsgData(ZukanTool::MSG_TYPE_POKE_COMMENT , ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]));
  }

  ZukanType::ZukanLang currLang = ZukanType::ConvertPokemonLang2ZukanLang(ZUKAN_LANG_ORDER_TABLE[m_gameLang][m_currLangIndex]);
  m_infoFunc.SetContents(
      pG2DUtil,
      msgDataZukan,      
      msgDataPokeName,
      msgDataPokeSpecies,
      msgDataPokeForm,
      msgDataPokeHeight,
      msgDataPokeWeight,
      msgDataPokeComment,
      currLang,
      this->getLangFont(currLang)
  );

  this->setCurrButtonStateForLang();
}

void ZukanViewInfo::setCurrButtonStateForPage(void)
{
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  
  //const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();

  if(m_modeIsFormVoice)
  {
    ////////////////////////////
    // ポケモン切り替えボタン
    ////////////////////////////
    if(m_param.contents->GetDispPokeDataIndexNumLimitedGet() < 2)  // こっちはget
    {
      buttonManager->SetButtonPassive(BUTTON_ID_FORMVOICE_PREV_MONS);
      buttonManager->SetButtonPassive(BUTTON_ID_FORMVOICE_NEXT_MONS);
    }
    else
    {
      buttonManager->SetButtonActive(BUTTON_ID_FORMVOICE_PREV_MONS);
      buttonManager->SetButtonActive(BUTTON_ID_FORMVOICE_NEXT_MONS);
    }

    ////////////////////////////
    // QRコードボタン
    ////////////////////////////
    m_QRButtonPane->SetVisible(false);  // こっちではボタンマネージャのボタンにはなっていない。ただのペイン。 
  }
  else 
  {
    ////////////////////////////
    // ポケモン切り替えボタン
    ////////////////////////////
    if(m_param.contents->GetDispPokeDataIndexNumLimitedSee() < 2)  // こっちはsee
    {
      buttonManager->SetButtonPassive(BUTTON_ID_EXPLAIN_PREV_MONS);
      buttonManager->SetButtonPassive(BUTTON_ID_EXPLAIN_NEXT_MONS);
    }
    else
    {
      buttonManager->SetButtonActive(BUTTON_ID_EXPLAIN_PREV_MONS);
      buttonManager->SetButtonActive(BUTTON_ID_EXPLAIN_NEXT_MONS);
    }

    ////////////////////////////
    // QRコードボタン
    ////////////////////////////
    m_QRButtonPane->SetVisible(true);  // こっちではボタンマネージャのボタンにはなっている。 
  } 
}

void ZukanViewInfo::setCurrButtonStateForDispPoke(void)
{
  app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    
  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  
  if(m_modeIsFormVoice)
  {
    ////////////////////////////
    // ポケモン切り替えボタン
    ////////////////////////////
    // もしループさせないなら、端に到達したときここでパッシブにする。ループさせるとき、リストではないので端でトリガーにするのは難しい。

    ////////////////////////////
    // フォルム切り替えボタン
    ////////////////////////////
    if(pokeData->figureSeeNum < 2)
    {
      buttonManager->SetButtonPassive(BUTTON_ID_FORMVOICE_PREV_FORM);
      buttonManager->SetButtonPassive(BUTTON_ID_FORMVOICE_NEXT_FORM);
    }
    else
    {
      buttonManager->SetButtonActive(BUTTON_ID_FORMVOICE_PREV_FORM);
      buttonManager->SetButtonActive(BUTTON_ID_FORMVOICE_NEXT_FORM);
    }
  }
  else 
  {
    ////////////////////////////
    // 姿＆鳴き声ページに行くボタン
    ////////////////////////////
    if(pokeData->getFlag == 0)
    {
      buttonManager->SetButtonPassive(BUTTON_ID_EXPLAIN_FORMVOICE);
    }
    else
    {
      buttonManager->SetButtonActive(BUTTON_ID_EXPLAIN_FORMVOICE);
    }

    ////////////////////////////
    // ポケモン切り替えボタン
    ////////////////////////////
    // もしループさせないなら、端に到達したときここでパッシブにする。ループさせるとき、リストではないので端でトリガーにするのは難しい。
  }
}

void ZukanViewInfo::setCurrButtonStateForDispForm(void)
{
  //app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
    
  //const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  
  if(m_modeIsFormVoice)
  {
    ////////////////////////////
    // フォルム切り替えボタン
    ////////////////////////////
    // もしループさせないなら、端に到達したときここでパッシブにする。ループさせるとき、リストではないので端でトリガーにするのは難しい。
  }
}

void ZukanViewInfo::setCurrButtonStateForLang(void)
{
  app::util::G2DUtil*      pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
    
  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  
  if(!m_modeIsFormVoice)
  {
    ////////////////////////////
    // 言語ボタン
    ////////////////////////////
    u32 langBitFlag = 0;
    pokeData->GetGetPokemonLang(&langBitFlag, m_gameLang);
  
    // 今表示中の言語
    {
      // 今表示中の言語はm_currLangIndexである
 
      // 選択中なのでタブを無効にする
      this->GetButtonManager()->SetButtonInputEnable(BUTTON_ID_EXPLAIN_LANG_START +m_currLangIndex, false);   // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      // タブを表示する
      this->GetButtonManager()->SetButtonVisible(BUTTON_ID_EXPLAIN_LANG_START +m_currLangIndex);   // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

      pG2DUtil->StartAnime( LYT_WK_ID_LOWER, BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_LANG_START +m_currLangIndex].other1_anime_index );  // wk_0
    }
 
    // 表示中でない言語
    for(s32 i=0; i<ZukanType::ZUKAN_LANG_NUM; ++i)
    {
      u32 pokemonLang = ZUKAN_LANG_ORDER_TABLE[m_gameLang][i];  // POKEMON_LANG_???
      if( langBitFlag & (1 << pokemonLang) )
      {
        
        if(i != m_currLangIndex)  // 今表示中の言語でないなら(選択中アニメを適用したところと同じところにアニメを適用しないようにするためのif)
        {
          // タブを有効にする
          //this->GetButtonManager()->SetButtonInputEnable(BUTTON_ID_EXPLAIN_LANG_START +i, true);   // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
          //pG2DUtil->StartAnime( LYT_WK_ID_LOWER, BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_LANG_START +i].other2_anime_index );  // wk_0
        
          // タブを有効無効ではなく、表示非表示にする
          // タブを表示する
          ZukanUtil::SetButtonVisible(this->GetButtonManager(), BUTTON_ID_EXPLAIN_LANG_START +i, true);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
        
          pG2DUtil->StartAnime( LYT_WK_ID_LOWER, BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_LANG_START +i].other2_anime_index );  // wk_0
        }
      }
      else
      {
        // タブを無効にする
        //this->GetButtonManager()->SetButtonInputEnable(BUTTON_ID_EXPLAIN_LANG_START +i, false);   // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
        //pG2DUtil->StartAnime( LYT_WK_ID_LOWER, BUTTON_CONST_INFO_FOR_EXPLAIN[BUTTON_ID_EXPLAIN_LANG_START +i].other3_anime_index );  // wk_0
        
        // タブを有効無効ではなく、表示非表示にする
        // タブを非表示する
        ZukanUtil::SetButtonVisible(this->GetButtonManager(), BUTTON_ID_EXPLAIN_LANG_START +i, false);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
      }
    }
  }
}

void ZukanViewInfo::setQRCodeDisp(bool isDisp)
{
  m_isQRCodeDisp = isDisp;
  m_QRDispTopPane->SetVisible(m_isQRCodeDisp);
  // QRコード表示モードが変わったとき
  this->setPokeModelPosByQRCodeDisp(true);
}
void ZukanViewInfo::setPokeModelPosByQRCodeDisp(bool qrCodeDispChange)  // ポケモンモデルが変わったとき、QRコード表示モードが変わったとき、に呼ぶ
{
  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  if(m_isQRCodeDisp)
  {
    bool notExistBecauseOfNoQR = false;
    if( pokeData->DoesQRCodeExist(&notExistBecauseOfNoQR) )
    {
      m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_LEFT, qrCodeDispChange);
    }
    else
    {
      m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_CENTER, qrCodeDispChange);
    }
  }
  else
  {
    if( !m_modeChanging )
    {
      m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_CENTER, qrCodeDispChange);
    }
  }
}

void ZukanViewInfo::playVoice(void)
{
  PokeTool::SimpleParam sp = m_param.contents->GetCurrDispPokeSimpleParam();
  Sound::PlayVoice( 0, sp.monsNo, sp.formNo );  // voice_indexは0でいいか？
}
void ZukanViewInfo::changeMotion(void)
{
  m_param.tool->SetNextPokeModelAnime();
}
void ZukanViewInfo::switchPause(void)
{
  m_param.tool->SwitchPokeModelAnimePause();
}

void ZukanViewInfo::createLangFont(void)
{
  gfl2::str::Font* gameFont = GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont();
  switch(m_gameLang)
  {
  case ZukanType::ZUKAN_LANG_CHINA:
    {
      m_chinaFont  = gameFont;
      break;
    }
  case ZukanType::ZUKAN_LANG_TAIWAN:
    {
      m_taiwanFont = gameFont;
      break;
    }
  default:
    {
      m_japanFont  = gameFont;
      break;
    }
  }

  /*
  ZukanTool::LangFontManagerに任せたのでコメントアウト
  gfl2::str::ResFont* resFont = NULL;
  if(m_japanFont == NULL)
  {
    resFont = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::ResFont();
    resFont->SetResource(m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_FONT_JAPAN));
    m_japanFont = resFont;
  }

  if(m_chinaFont == NULL)
  {
    resFont = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::ResFont();
    resFont->SetResource(m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_FONT_CHINA));
    m_chinaFont = resFont;
  }

  if(m_taiwanFont == NULL)
  {
    resFont = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::ResFont();
    resFont->SetResource(m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_FONT_TAIWAN));
    m_taiwanFont = resFont;
  }
  */
}
void ZukanViewInfo::destroyLangFont(void)
{
  /*
  ZukanTool::LangFontManagerに任せたのでコメントアウト
  gfl2::str::Font* gameFont = GFL_SINGLETON_INSTANCE(print::SystemFont)->GetFont();
  if(m_japanFont  != gameFont){ GFL_SAFE_DELETE(m_japanFont) ; } else { m_japanFont  = NULL; }
  if(m_chinaFont  != gameFont){ GFL_SAFE_DELETE(m_chinaFont) ; } else { m_chinaFont  = NULL; }
  if(m_taiwanFont != gameFont){ GFL_SAFE_DELETE(m_taiwanFont); } else { m_taiwanFont = NULL; }
  */

  m_japanFont  = NULL;
  m_chinaFont  = NULL;
  m_taiwanFont = NULL;
}
gfl2::str::Font* ZukanViewInfo::getLangFont(ZukanType::ZukanLang zukanLang) const
{
  gfl2::str::Font* langFont[ZukanType::ZUKAN_LANG_NUM] =
  {
    m_japanFont,
    m_japanFont,
    m_japanFont,
    m_japanFont,
    m_japanFont,
    m_japanFont,
    m_japanFont,
    m_chinaFont,
    m_taiwanFont,
  };
  return langFont[zukanLang];
}

void ZukanViewInfo::replaceLangIconTexture(gfl2::lyt::LytPicture* pane, u32 pokemonLang)
{
  // グラフィックフォント(手動差し替え)

  u32 tex = LYTRES_ZUKANGRPFONT_JPN_ZUKAN_BFLIM;
  switch(pokemonLang)
  {
  case POKEMON_LANG_JAPAN  :  tex = LYTRES_ZUKANGRPFONT_JPN_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_ENGLISH:  tex = LYTRES_ZUKANGRPFONT_ENG_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_FRANCE :  tex = LYTRES_ZUKANGRPFONT_FRA_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_ITALY  :  tex = LYTRES_ZUKANGRPFONT_ITA_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_GERMANY:  tex = LYTRES_ZUKANGRPFONT_GER_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_SPAIN  :  tex = LYTRES_ZUKANGRPFONT_SPA_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_KOREA  :  tex = LYTRES_ZUKANGRPFONT_KOR_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_CHINA  :  tex = LYTRES_ZUKANGRPFONT_CHS_ZUKAN_BFLIM;  break;
  case POKEMON_LANG_TAIWAN :  tex = LYTRES_ZUKANGRPFONT_CHT_ZUKAN_BFLIM;  break;
  }
  
  app::util::G2DUtil*      pG2DUtil    = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  gfl2::lyt::LytSys*       lytSys      = pG2DUtil->GetLayoutSystem();
  gfl2::lyt::LytMultiResID lytResID_1  = pG2DUtil->GetLayoutResourceID( 1 );  // res_1
  gfl2::lyt::LytRes*       lytRes_1    = lytSys->GetResource( &lytResID_1 );

  const gfl2::lyt::LytResTextureInfo* texInfo = lytRes_1->GetTexture( tex, lytResID_1.GetMultiID() );
  app::tool::LytTexReplaceSystem::ReplaceTextureStatic( texInfo, pane, 0, 0 );
}


#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE
void ZukanViewInfo::unbindKeyForPokeModelSRTModifyMode(void)
{
  // 十字キーをデバッグに用いたいので、ボタンペインはタッチしか受け付けないようにしておく。
  if(m_modeIsFormVoice)
  {
    app::tool::ButtonManager* buttonManager = this->GetButtonManager();  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
    if(buttonManager)
    {
      s32 pokeModelSRTModifyMode = 0;
#if PM_DEBUG
      pokeModelSRTModifyMode = ZukanDebugWin::GetPokeModelSRTModifyMode();
#endif
      if(pokeModelSRTModifyMode != 0)
      {
        buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_PREV_MONS, 0, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER);
        buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_NEXT_MONS, 0, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER);
        buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_PREV_FORM, 0, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER);
        buttonManager->SetButtonBindKey(BUTTON_ID_FORMVOICE_NEXT_FORM, 0, app::tool::ButtonManager::BIND_KEY_MODE_TRIGGER);
      }
    }
  }
}
#endif


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

