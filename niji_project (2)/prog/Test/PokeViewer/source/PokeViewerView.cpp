//======================================================================
/**
 * @file PokeViewerView.cpp
 * @date 2015/12/01 13:23:44
 * @author araki_syo
 * @brief ポケビューアーのView
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <Debug/DebugWin/include/DebugWinSystem.h>

#include "../include/PokeViewerView.h"
#include "niji_conv_header/app/pokeviewer/res2d/down1.h"
#include "niji_conv_header/app/pokeviewer/res2d/down1_anim_list.h"

#if defined(GF_PLATFORM_WIN32)
#define POKEVIEWER_SPRINTF ::sprintf_s
#else
#define POKEVIEWER_SPRINTF ::std::sprintf
#endif

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @fn PokeViewerView
 * @brief ctor
 */
PokeViewerView::PokeViewerView(const Param& param)
: app::ui::UIView(param.pHeap, param.pHeap->GetDeviceAllocator(), param.pHeap->GetDeviceAllocator()),
  m_Param(param),
  m_pListener(NULL),
  m_Page(0)
{
  // initialize view listener
  m_pListener = GFL_NEW(m_Param.pHeap->GetDeviceHeap()) PokeViewerListener(this, m_Param.pPokeModelManager);
  this->SetInputListener(m_pListener);

  // initialize layout
  {
    SetLayoutTransferArea(gfl2::lyt::MEMORY_AREA_FCRAM);
    CreateGraphicsSystem(m_Param.pHeap, false);

    void* pRes2dDataBuf = m_Param.pDataManager->GetDataBuf(PokeViewerDataManager::DATA_ID_POKEVIEWER_RES2D);

    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res 0
      { pRes2dDataBuf, 0, app::util::G2DUtil::ATTACH_ROOT },
    };

    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      //wk 0
      { 0, LYTRES_DOWN1_DOWN1_BFLYT, LA_DOWN1_DOWN1___NUM, pRes2dDataBuf, true, app::util::G2DUtil::SETUP_LOWER, true },
    };

    Create2D(m_Param.pHeap, NULL, 2, resTbl, GFL_NELEMS(resTbl), wkTbl, GFL_NELEMS(wkTbl), app::ui::UIView::UNUSE_MESSAGE_DATA);
  }
}

/**
 * @fn ~PokeViewerView
 * @brief dtor
 */
PokeViewerView::~PokeViewerView()
{
  Delete2D();
  DeleteGraphicsSystem();

  GFL_SAFE_DELETE(m_pListener);
}

/**
 * @fn Update
 * @brief ViewのUpdate
 */
void PokeViewerView::Update()
{
  UpdatePageDisp();
  app::ui::UIView::Update();
}

/**
 * @fn Draw
 * @brief ViewのDraw
 */
void PokeViewerView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = GetG2DUtil();

  // 処理負荷
  {
    char buf[255];

    s64 updateTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
    s64 upnextTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
    s64 drawTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
    s64 gpuTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);

    POKEVIEWER_SPRINTF(buf, "%d", updateTick + upnextTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_00, buf);

    POKEVIEWER_SPRINTF(buf, "%d", drawTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_01, buf);

    POKEVIEWER_SPRINTF(buf, "%d", gpuTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_02, buf);
  }

  // カメラ設定
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%d", m_Param.pPokeModelManager->GetCameraFovy());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_CAMERA_FOV_VAL, buf);
  }

  // 表示内容の更新
  switch (m_Page)
  {
  case 0:
    this->UpdatePage1();
    break;
  case 1:
    this->UpdatePage2();
    break;
  case 2:
    this->UpdatePage3();
    break;
  case 3:
    this->UpdatePage4();
    break;
  case 4:
    this->UpdatePage5();
    break;
  case 5:
    this->UpdatePage6();
  default:
    break;
  }

  pG2DUtil->AddDrawableLytWkForOneFrame( m_Param.pAppRenderingManager, displayNo, 0 );  // wk_0
}

/**
 * @fn ReqEnd
 * @brief リスナーから呼び出される終了リクエスト
 */
void PokeViewerView::ReqEnd()
{
  m_Param.pFrameCell->ReqEnd();
}

void PokeViewerView::UpdatePage(b32 isUp)
{
  if(isUp)
  {
    ++m_Page;

    if(m_Page > MAX_PAGE)
    {
      m_Page = 0;
    }
  }
  else
  {
    --m_Page;
    
    if(m_Page < 0)
    {
      m_Page = MAX_PAGE;
    }
  }

  UpdatePageDisp();
}

/**
 * @fn UpdatePageDisp
 * @brief Viewのページを更新する
 */
void PokeViewerView::UpdatePageDisp()
{
  // ムーブペイン切り替え
  {
    gfl2::lyt::LytPane *pane = this->GetG2DUtil()->GetLayoutWork(0)->GetPane(PANENAME_DOWN1_PANE_MOVEPANE);
    gfl2::math::VEC3 pos = pane->GetTranslate();
    pos.x = m_Page * -320;
    pane->SetTranslate( pos );
  }

  // 下ロックペイン切り替え
  {
    gfl2::lyt::LytPane *pane = this->GetG2DUtil()->GetLayoutWork(0)->GetPane(PANENAME_DOWN1_PANE_LOCKPANE2);
    gfl2::math::VEC3 pos = pane->GetTranslate();

    m_Page < 2 ? pos.x = 0 : pos.x = -320;
    pane->SetTranslate( pos );
  }

  int paneArr[6] =
  {
    PANENAME_DOWN1_PANE_PAGE_BUT_1,
    PANENAME_DOWN1_PANE_PAGE_BUT_2,
    PANENAME_DOWN1_PANE_PAGE_BUT_3,
    PANENAME_DOWN1_PANE_PAGE_BUT_4,
    PANENAME_DOWN1_PANE_PAGE_BUT_5,
    PANENAME_DOWN1_PANE_PAGE_BUT_6,
  };

  gfl2::lyt::LytPicture *picPane;
  for( int i=0; i < 6; i++ )
  {
    picPane = this->GetG2DUtil()->GetLayoutWork(0)->GetPicturePane(paneArr[i]);
    u8 val = 255;
    if( m_Page == i )
    {
      val = 0;
    }
    SetPicPaneCol( paneArr[i] , 255, 255, val );
  }
}

void PokeViewerView::UpdatePage1()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 現在のモーションの名前を表示
  {
    char buf[255];
    m_Param.pPokeModelManager->GetMotionName(buf);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TEXTBOX_00, buf);
  }

  // 選択中のモーションのフレーム数を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%.1f", m_Param.pPokeModelManager->GetMotionMaxFrame());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MOTION_FRAME_MAX, buf);
  }

  // 選択中のモーションの現在のフレームを表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%.1f", m_Param.pPokeModelManager->GetCurrentMotionFrame());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MOTION_FRAME_STR, buf);
  }

  // モーションの再生速度を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%1.1f", m_Param.pPokeModelManager->GetMotionSpeed());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MOTION_SPEED_STR, buf);
  }

  // ループボタンの状態を更新
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_LOOP_00, m_Param.pPokeModelManager->GetMotionLoop());
  }

  // 再生/停止ボタンの状態を更新
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_MOTION_PAUSE, m_Param.pPokeModelManager->IsPlaying());
  }

  // ライトの垂直方向の回転角を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%3.0f", m_Param.pPokeModelManager->GetLightRotationV());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_LIGHT_V_STR, buf);
  }

  // ライトの水平方向の回転角を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%3.0f", m_Param.pPokeModelManager->GetLightRotationH());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_LIGHT_H_STR, buf);
  }
}

void PokeViewerView::UpdatePage2()
{
}

void PokeViewerView::UpdatePage3()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // カラーシェーダ設定
  {
    static const f32 MAX_LENGTH = 96.0f;
    gfl2::gfx::Color color = m_Param.pPokeModelManager->GetPokeShaderColor();
    
    nw::lyt::Size size_r = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_SHADER_C_R_BAR);
    size_r.width = MAX_LENGTH * color.GetR();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_SHADER_C_R_BAR, size_r);

    nw::lyt::Size size_g = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_SHADER_C_G_BAR);
    size_g.width = MAX_LENGTH * color.GetG();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_SHADER_C_G_BAR, size_g);

    nw::lyt::Size size_b = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_SHADER_C_B_BAR);
    size_b.width = MAX_LENGTH * color.GetB();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_SHADER_C_B_BAR, size_b);
  }

  // カラーシェーダ係り具合
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%1.1f", m_Param.pPokeModelManager->GetPokeShaderColor().GetA());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SHADER_POW_VAL, buf); 
  }

  // カラーシェーダON/OFF
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SHADER_C_BUT_ON, m_Param.pPokeModelManager->IsPokeShaderEnable());
  }
}

void PokeViewerView::UpdatePage4()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // ポケモンの名前を表示
  {
    // マジックナンバーでMONS_NAME_BUFFER_SIZEよりも数値が小さかったため、文字があふれていた
    pml::STRCODE strbuf[pml::MONS_NAME_BUFFER_SIZE];
    m_Param.pPokeModelManager->GetMonsName(strbuf);
    pG2DUtil->SetTextBoxPaneString(0, PANENAME_DOWN1_PANE_LOAD_ROM_NAME, strbuf);
  }

  // ポケモンの図鑑番号を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%3d", m_Param.pPokeModelManager->GetMonsNo());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MONSNO_VAL, buf);
  }

  // ポケモンのフォルム番号を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%2d", m_Param.pPokeModelManager->GetFormNo());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_FORMNO_VAL, buf);
  }

  // ポケモンの体長を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%.2f", m_Param.pPokeModelManager->GetPokeBaseHeight());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_HEIGHT_VAL, buf);
  }

  // ポケモンの表示体長を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%.2f", m_Param.pPokeModelManager->GetPokeAdjustHeight());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_ADJUST_VAL, buf);
  }

  // ポケモンのサイズカテゴリを表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%c", m_Param.pPokeModelManager->GetSizeCategory());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SIZE_VAL, buf);
  }

  // モーションの種類を表示
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_MOTION_TYPE_BTN_KW, (m_Param.pPokeModelManager->GetAnimeType() == PokeTool::MODEL_ANIMETYPE_KAWAIGARI));
  }

  // メスのモデルかどうかを表示
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SEX_BTN_ENABLE, (m_Param.pPokeModelManager->GetSex() == pml::SEX_FEMALE));
  }

  // 色違いかどうかを表示
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_RARE_BTN_ENABLE, m_Param.pPokeModelManager->IsRare());
  }
}

void PokeViewerView::LoadPokeModel()
{
  m_Param.pPokeModelManager->LoadPokeModel();
}

void PokeViewerView::UpdatePage5()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 影の表示設定
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_DISPBUTTON_03_ON, m_Param.pPokeModelManager->GetShadowVisibility());
  }

  // インフレーム設定
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_DISPBUTTON_06_CTR, m_Param.pPokeModelManager->GetCameraMode() == PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_CTR);
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_DISPBUTTON_06_BTM, m_Param.pPokeModelManager->GetCameraMode() == PokeViewerPokeModelManager::CAMERA_MODE_INFRAME_BTM);
  }

  // 背景色設定
  {
    static const f32 MAX_LENGTH = 128.0f;
    gfl2::gfx::Color color = m_Param.pPokeModelManager->GetClearColor();
    
    nw::lyt::Size size_r = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_RED_BAR);
    size_r.width = MAX_LENGTH * color.GetR();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_RED_BAR, size_r);

    nw::lyt::Size size_g = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_GREEN_BAR);
    size_g.width = MAX_LENGTH * color.GetG();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_GREEN_BAR, size_g);

    nw::lyt::Size size_b = pG2DUtil->GetSize(0, PANENAME_DOWN1_PANE_BLUE_BAR);
    size_b.width = MAX_LENGTH * color.GetB();
    pG2DUtil->SetSize(0, PANENAME_DOWN1_PANE_BLUE_BAR, size_b);
  }

  // エッジ色設定
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_DISPBUTTON_10_ON, m_Param.pPokeModelManager->GetEdgeColor() == gfl2::gfx::Color(0.0f, 0.0f, 0.0f));
  }
}

void PokeViewerView::UpdatePage6()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // キャプチャモードの表示
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_START_BTN_ENABLE, m_Param.pPokeModelManager->IsCaptureMode());
  }

  // 負荷処理計測モードの表示
  // キャプチャモードの一部として機能を作成しているので, キャプチャモードの表示に合わせる
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_START_MEASURE_BTN_ENA, m_Param.pPokeModelManager->IsLoadMeasurementMode());
  }

  // キャプチャを開始する図鑑番号を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%3d", m_Param.pPokeModelManager->GetStartMonsNo());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SS_START_VAL, buf);
  }

  // キャプチャを終了する図鑑番号を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%3d", m_Param.pPokeModelManager->GetEndMonsNo());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SS_END_VAL, buf);
  }

  // キャプチャする方向を表示
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_FRONT_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_FRONT));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_BACK_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_BACK));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_RIGHT_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_RIGHT));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_LEFT_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_LEFT));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_TOP_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_TOP));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_SS_DIR_BTN_BOTTOM_ON, m_Param.pPokeModelManager->GetCameraDirectionFlag(PokeViewerPokeModelManager::CAMERA_DIRECTION_BOTTOM));
  }

  // カメラ距離を表示
  {
    char buf[255];
    POKEVIEWER_SPRINTF(buf, "%.1f", m_Param.pPokeModelManager->GetCameraDistance());
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SS_LEN_VAL, buf);
  }
}

void PokeViewerView::SetPicPaneCol( u32 pane, u8 r, u8 g, u8 b, u8 a)
{
  nw::ut::Color8 col(r,g,b,a);
  gfl2::lyt::LytPicture* picPane = this->GetG2DUtil()->GetLayoutWork(0)->GetPicturePane(pane);

  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_LT,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_RT,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_LB,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_RB,col);
  
}

GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
