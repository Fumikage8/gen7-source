//======================================================================
/**
* @file CharaViewerView.cpp
* @date 2015/07/21 18:24:03
* @author araki_syo
* @brief CharaViewer
* @par Copyright
* (C)1995-2015 GAME FREAK inc. All Rights Reserved.
*/
//======================================================================

#if PM_DEBUG

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Debug/DebugWin/include/DebugWinSystem.h>

// CharaViewerのインクルード
#include <Test/CharaViewer/include/CharaViewerProcParam.h>
#include "../include/CharaViewerDataManager.h"
#include "../include/CharaViewerView.h"
#include "../include/CharaViewerFrame.h"
#include "../include/CharaViewerListener.h"
#include "../include/CharaViewerCharaModelManager.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(chara_viewer)

#if defined(GF_PLATFORM_WIN32)
#define CHARA_VIEWER_SPRINTF ::sprintf_s
#else
#define CHARA_VIEWER_SPRINTF ::std::snprintf
#endif
//==============================================================================
//
//  CharaViewerView
//
//==============================================================================
CharaViewerView::CharaViewerView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
  m_param(param),
  m_viewListener(NULL),
  m_page(0)
{
  // CharaViewerViewListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) CharaViewerListener(this);
    this->SetInputListener(m_viewListener);  // CharaViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ関数
    this->CreateGraphicsSystem( m_param.appHeap, false );  // 親であるapp::ui::UIViewのメンバ関数

    void* res2dDataBuf = m_param.dataManager->GetDataBuf(CharaViewerDataManager::DATA_ID_CHARA_VIEWER_RES2D);

    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        res2dDataBuf,
          0,
          app::util::G2DUtil::ATTACH_ROOT
      },
    };

    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      // wk_0
      {
        // 下画面
        0,  // res_0
          LYTRES_DOWN1_DOWN1_BFLYT,
          LA_DOWN1_DOWN1___NUM,
          res2dDataBuf,
          true,
          app::util::G2DUtil::SETUP_LOWER,
          true
      },
    };

    this->Create2D(  // 親であるapp::ui::UIViewのメンバ関数
      m_param.appHeap,
      NULL,
      2, 
      resTbl,
      GFL_NELEMS(resTbl), 
      wkTbl,
      GFL_NELEMS(wkTbl), 
      app::ui::UIView::UNUSE_MESSAGE_DATA
      );

  }

  // フィールドデバッグ用に起動されたなら着せ替えのページにしておく
  if (m_param.param->fieldDebugModeEnabled)
  {
    m_page = 1;
  }
}

CharaViewerView::~CharaViewerView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数

  // CharaViewerViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void CharaViewerView::Update(void)
{
  this->UpdatePageDisp();
  
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}

void CharaViewerView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 処理負荷
  {
    char buf[255];

    s32 updateTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPDATE);
    s32 upnextTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_UPNEXT);
    s32 drawTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_DRAW);
    s32 gpuTick = GFL_SINGLETON_INSTANCE(Debug::DebugWin::DebugWinWork)->GetTickCalcAve(Debug::DebugWin::DEBUGWIN_TICK_GPU);

    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%d", updateTick + upnextTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_00, buf);

    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%d", drawTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_01, buf);

    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%d", gpuTick);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TICKNUM_02, buf);
  }

  // 選択中のモデルを表示
  {
    u32 currentModel = m_param.charaModelManager->GetCurrentModel();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", currentModel);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_POKE_STR, buf);
  }

  // 表示内容の更新
  switch (m_page)
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
  default:
    break;
  }

  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0 );  // wk_0
}

void CharaViewerView::ReqEnd(void)
{
  m_param.frameCell->ReqEnd();
}

void CharaViewerView::UpdatePage(bool isUp)
{
  
  if( isUp )
  {
    m_page++;
    if( m_page > maxPage )
    {
      m_page = 0;
    }
  }
  else
  {
    m_page--;
    if( m_page < 0 )
    {
      m_page = maxPage;
    }
  }
  
  UpdatePageDisp();
}

void CharaViewerView::UpdatePageDisp()
{
  {
    gfl2::lyt::LytPane *pane = this->GetG2DUtil()->GetLayoutWork(0)->GetPane(PANENAME_DOWN1_PANE_MOVEPANE);
    gfl2::math::VEC3 pos = pane->GetTranslate();
    pos.x = m_page * -320;
    pane->SetTranslate( pos );
  }

  int paneArr[5] =
  {
    PANENAME_DOWN1_PANE_PAGE_BUT_1,
    PANENAME_DOWN1_PANE_PAGE_BUT_2,
    PANENAME_DOWN1_PANE_PAGE_BUT_3,
    PANENAME_DOWN1_PANE_PAGE_BUT_4,
    PANENAME_DOWN1_PANE_PAGE_BUT_5,
  };

  for( int i=0; i < 5; i++ )
  {
    u8 val = 255;
    if( m_page == i )
    {
      val = 0;
    }
    SetPicPaneCol( paneArr[i] , 255, 255, val );
  }
}

void CharaViewerView::UpdatePage1()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // モデルタイプ
  {
    if (m_param.charaModelManager->GetLoadModelType() == CharaViewerCharaModelManager::MODEL_TYPE_BATTLE)
    {
      this->SetPicPaneCol(PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_BT, 255, 255, 0);
      this->SetPicPaneCol(PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_FI, 255, 255, 255);
    }
    else
    {
      this->SetPicPaneCol(PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_BT, 255, 255, 255);
      this->SetPicPaneCol(PANENAME_DOWN1_PANE_CHARA_SELECTOR_BTN_FI, 255, 255, 0);
    }
  }

  // 選択中のキャラ名を表示
  {
    const char* pCharaName = NULL;
    s32 charaId = 0;
    m_param.charaModelManager->GetCurrentCharaName(&pCharaName, &charaId);
    char buf[255];
    //CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s [%d]", pCharaName, charaId);
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s", pCharaName);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_CHARA_SELECTOR_NAME, buf);
  }

  // 現在のスケールを表示
  {
    f32 scale = m_param.charaModelManager->GetCurrentCharaScale();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%.2f", scale);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_SCALE_SELECTOR_VALUE, buf);
  }

  // ポリゴン数
  {
    u32 polygonCount = m_param.charaModelManager->GetPolygonCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", polygonCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_POLYGONNUM, buf);
  }
  // マテリアル数
  {
    u32 materialCount = m_param.charaModelManager->GetMaterialCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", materialCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MATERIALNUM, buf);
  }
  // メッシュ数
  {
    u32 meshCount = m_param.charaModelManager->GetMeshCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", meshCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MESHNUM, buf);
  }
  // 頂点数
  {
    u32 vertexCount = m_param.charaModelManager->GetVertexCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", vertexCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_VERTEXNUM, buf);
  }
  // テクスチャ数
  {
    u32 textureCount = m_param.charaModelManager->GetTextureCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", textureCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TEXTURENUM, buf);
  }
  // ボーン数
  {
    u32 boneCount = m_param.charaModelManager->GetBoneCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", boneCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_BONENUM, buf);
  }
  // モデルリソースサイズ
  {
    f32 resModelSize = m_param.charaModelManager->GetResModelSize() / 1024.0f;
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%.2fKB", resModelSize);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_RESMODELMEMNUM, buf);
  }
  // アニメーションリソースサイズ
  {
    f32 resAnimSize = m_param.charaModelManager->GetResAnimSize() / 1024.0f;
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%.2fKB", resAnimSize);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_RESANIMMEMNUM, buf);
  }
  // 合計リソースサイズ
  {
    f32 resTotalSize = m_param.charaModelManager->GetResTotalSize() / 1024.0f;
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%.2fKB", resTotalSize);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_RESTOTALMEMNUM, buf);
  }
  // インスタンスサイズ
  {
    f32 instanceSize = m_param.charaModelManager->GetInstanceSize() / 1024.0f;
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%.2fKB", instanceSize);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_INSMEMNUM, buf);
  }
}

void CharaViewerView::UpdatePage2()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 選択中のアイテム
  {
    static const u32 sc_partsPaneLine[] =
    {
      PANENAME_DOWN1_PANE_SKIN_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_HAIR_STY_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_HEAD_W_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_HAIR_ACCE_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_EYE_W_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_TOPS_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_BOTTOMS_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_SOCKS_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_SHOES_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_BAG_SELECTOR_NAME,
    };

    static void (CharaViewerCharaModelManager::*sc_getPartsFuncList[])(const char**, s32*) const =
    {
      &CharaViewerCharaModelManager::GetCurrentPartsNameFace,
      &CharaViewerCharaModelManager::GetCurrentPartsNameHairStyle,
      &CharaViewerCharaModelManager::GetCurrentPartsNameHat,
      &CharaViewerCharaModelManager::GetCurrentPartsNameAccHair,
      &CharaViewerCharaModelManager::GetCurrentPartsNameAccEye,
      &CharaViewerCharaModelManager::GetCurrentPartsNameTops,
      &CharaViewerCharaModelManager::GetCurrentPartsNameBottoms,
      &CharaViewerCharaModelManager::GetCurrentPartsNameSocks,
      &CharaViewerCharaModelManager::GetCurrentPartsNameShoes,
      &CharaViewerCharaModelManager::GetCurrentPartsNameBag,
    };

    char buf[255];

    for (u32 i = 0; i < sizeof(sc_partsPaneLine) / sizeof(sc_partsPaneLine[0]); ++i)
    {
      u32 pane = sc_partsPaneLine[i];

      const char* pPartsName = NULL;
      s32 partsIndex = 0;

      (m_param.charaModelManager->*sc_getPartsFuncList[i])(&pPartsName, &partsIndex);

      //if (partsIndex < 0)
      {
        CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s", pPartsName);
      }
      //else
      //{
      //  CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s [%d]", pPartsName, partsIndex);
      //}

      pG2DUtil->SetTextboxPaneString(0, pane, buf);
    }
  }

  // 服ペインのカラー設定
  {
    static const u32 sc_clothPaneList[] =
    {
      PANENAME_DOWN1_PANE_CLOTH_PALLET_00,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_01,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_02,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_03,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_04,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_05,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_06,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_07,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_08,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_09,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_10,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_11,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_12,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_13,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_14,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_15,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_16,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_17,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_18,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_19,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_20,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_21,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_22,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_23,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_24,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_25,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_26,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_27,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_28,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_29,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_30,

      PANENAME_DOWN1_PANE_CLOTH_PALLET_31,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_32,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_33,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_34,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_35,
      PANENAME_DOWN1_PANE_CLOTH_PALLET_36,
    };

    for (u32 i = 0; i < sizeof(sc_clothPaneList) / sizeof(sc_clothPaneList[0]); ++i)
    {
      u32 pane = sc_clothPaneList[i];
      u32 color = m_param.charaModelManager->GetClothColor(i);
      pG2DUtil->SetVtxColorElementOfPicturePane(0, pane, color);
    }

    s32 clothColorIndex = m_param.charaModelManager->GetClothColorIndex();
    if (clothColorIndex < 0)
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_01, false);
    }
    else
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_01, true);

      gfl2::math::VEC3 panePos = pG2DUtil->GetPaneLocalPos(0, sc_clothPaneList[clothColorIndex]);
      pG2DUtil->SetPanePos(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_01, &panePos);
    }
  }

  // 色替え対象の服を青くする
  {
    static const u32 sc_clothPaneList[] =
    {
      PANENAME_DOWN1_PANE_HEAD_W_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_HAIR_ACCE_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_EYE_W_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_TOPS_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_BOTTOMS_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_SOCKS_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_SHOES_SELECTOR_TITLE,
      PANENAME_DOWN1_PANE_BAG_SELECTOR_TITLE,
    };
    static bool (CharaViewerCharaModelManager::*sc_funcList[])() const =
    {
      &CharaViewerCharaModelManager::IsClothColorHat,
      &CharaViewerCharaModelManager::IsClothColorAccHair,
      &CharaViewerCharaModelManager::IsClothColorAccEye,
      &CharaViewerCharaModelManager::IsClothColorTops,
      &CharaViewerCharaModelManager::IsClothColorBottoms,
      &CharaViewerCharaModelManager::IsClothColorLegs,
      &CharaViewerCharaModelManager::IsClothColorShoes,
      &CharaViewerCharaModelManager::IsClothColorBag,
    };

    for (u32 i = 0; i < sizeof(sc_clothPaneList) / sizeof(sc_clothPaneList[0]); ++i)
    {
      u32 pane = sc_clothPaneList[i];
      gfl2::lyt::ColorU8 top(0, 255, 255, 255);
      if ((m_param.charaModelManager->*sc_funcList[i])())
      {
        top.r = 0;
        top.g = 0;
      }
      gfl2::lyt::ColorU8 bottom(top);

      pG2DUtil->SetTextBoxPaneColor(0, pane, top, bottom);
    }
  }

  // ポリゴン数
  {
    u32 polygonCount = m_param.charaModelManager->GetPolygonCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", polygonCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_POLYGONNUM_00, buf);
  }
  // マテリアル数
  {
    u32 materialCount = m_param.charaModelManager->GetMaterialCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", materialCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MATERIALNUM_00, buf);
  }
  // メッシュ数
  {
    u32 meshCount = m_param.charaModelManager->GetMeshCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", meshCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_MESHNUM_00, buf);
  }
  // 頂点数
  {
    u32 vertexCount = m_param.charaModelManager->GetVertexCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", vertexCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_VERTEXNUM_00, buf);
  }
  // テクスチャ数
  {
    u32 textureCount = m_param.charaModelManager->GetTextureCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", textureCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_TEXTURENUM_00, buf);
  }
  // ボーン数
  {
    u32 boneCount = m_param.charaModelManager->GetBoneCount();
    char buf[255];
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", boneCount);
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_BONENUM_00, buf);
  }
}

void CharaViewerView::UpdatePage3()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 選択中のアイテム
  {
    static const u32 sc_partsPaneLine[] =
    {
      PANENAME_DOWN1_PANE_BANGLE_SELECTOR_NAME,
      PANENAME_DOWN1_PANE_SITUATION_NAME,
    };

    static void (CharaViewerCharaModelManager::*sc_getPartsFuncList[])(const char**, s32*) const =
    {
      &CharaViewerCharaModelManager::GetCurrentPartsNameBngl,
      &CharaViewerCharaModelManager::GetCurrentPartsNameSituation,
    };

    char buf[255];

    for (u32 i = 0; i < sizeof(sc_partsPaneLine) / sizeof(sc_partsPaneLine[0]); ++i)
    {
      u32 pane = sc_partsPaneLine[i];

      const char* pPartsName = NULL;
      s32 partsIndex = 0;

      (m_param.charaModelManager->*sc_getPartsFuncList[i])(&pPartsName, &partsIndex);

      //if (partsIndex < 0)
      {
        CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s", pPartsName);
      }
      //else
      //{
      //  CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s [%d]", pPartsName, partsIndex);
      //}

      pG2DUtil->SetTextboxPaneString(0, pane, buf);
    }
  }

  // 肌ペインのカラー設定
  {
    static const u32 sc_skinPaneList[] =
    {
      PANENAME_DOWN1_PANE_SKIN_PALLET_00,
      PANENAME_DOWN1_PANE_SKIN_PALLET_01,
      PANENAME_DOWN1_PANE_SKIN_PALLET_02,
      PANENAME_DOWN1_PANE_SKIN_PALLET_03,
    };

    for (u32 i = 0; i < sizeof(sc_skinPaneList) / sizeof(sc_skinPaneList[0]); ++i)
    {
      u32 pane = sc_skinPaneList[i];
      u32 color = m_param.charaModelManager->GetSkinColor(i);
      pG2DUtil->SetVtxColorElementOfPicturePane(0, pane, color);
    }

    s32 skinColorIndex = m_param.charaModelManager->GetSkinColorIndex();
    if (skinColorIndex < 0)
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU, false);
    }
    else
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU, true);

      gfl2::math::VEC3 panePos = pG2DUtil->GetPaneLocalPos(0, sc_skinPaneList[skinColorIndex]);
      pG2DUtil->SetPanePos(0, PANENAME_DOWN1_PANE_EMPTY_WAKU, &panePos);
    }
  }

  // 髪ペインのカラー設定
  {
    static const u32 sc_hairPaneList[] =
    {
      PANENAME_DOWN1_PANE_HAIR_PALLET_00,
      PANENAME_DOWN1_PANE_HAIR_PALLET_01,
      PANENAME_DOWN1_PANE_HAIR_PALLET_02,
      PANENAME_DOWN1_PANE_HAIR_PALLET_03,
      PANENAME_DOWN1_PANE_HAIR_PALLET_04,
      PANENAME_DOWN1_PANE_HAIR_PALLET_05,
      PANENAME_DOWN1_PANE_HAIR_PALLET_06,
      PANENAME_DOWN1_PANE_HAIR_PALLET_07,
      PANENAME_DOWN1_PANE_HAIR_PALLET_08,
    };

    for (u32 i = 0; i < sizeof(sc_hairPaneList) / sizeof(sc_hairPaneList[0]); ++i)
    {
      u32 pane = sc_hairPaneList[i];
      u32 color = m_param.charaModelManager->GetHairColor(i);
      pG2DUtil->SetVtxColorElementOfPicturePane(0, pane, color);
    }

    s32 hairColorIndex = m_param.charaModelManager->GetHairColorIndex();
    if (hairColorIndex < 0)
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_00, false);
    }
    else
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_00, true);

      gfl2::math::VEC3 panePos = pG2DUtil->GetPaneLocalPos(0, sc_hairPaneList[hairColorIndex]);
      pG2DUtil->SetPanePos(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_00, &panePos);
    }
  }

  // コンタクトのカラー設定
  {
    static const u32 sc_contactPaneList[] =
    {
      PANENAME_DOWN1_PANE_CONTACT_PALLET_00,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_01,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_02,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_03,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_04,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_05,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_06,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_07,
      PANENAME_DOWN1_PANE_CONTACT_PALLET_08,
    };

    for (u32 i = 0; i < sizeof(sc_contactPaneList) / sizeof(sc_contactPaneList[0]); ++i)
    {
      u32 pane = sc_contactPaneList[i];
      u32 color = m_param.charaModelManager->GetEyeColor(i);
      pG2DUtil->SetVtxColorElementOfPicturePane(0, pane, color);
    }

    s32 eyeColorIndex = m_param.charaModelManager->GetEyeColorIndex();
    if (eyeColorIndex < 0)
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_03, false);
    }
    else
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_03, true);

      gfl2::math::VEC3 panePos = pG2DUtil->GetPaneLocalPos(0, sc_contactPaneList[eyeColorIndex]);
      pG2DUtil->SetPanePos(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_03, &panePos);
    }
  }

  // リップのカラー設定
  {
    static const u32 sc_lipPaneList[] =
    {
      PANENAME_DOWN1_PANE_LIP_PALLET_00,
      PANENAME_DOWN1_PANE_LIP_PALLET_01,
      PANENAME_DOWN1_PANE_LIP_PALLET_02,
      PANENAME_DOWN1_PANE_LIP_PALLET_03,
      PANENAME_DOWN1_PANE_LIP_PALLET_04,
      PANENAME_DOWN1_PANE_LIP_PALLET_05,
      PANENAME_DOWN1_PANE_LIP_PALLET_06,
    };

    for (u32 i = 0; i < sizeof(sc_lipPaneList) / sizeof(sc_lipPaneList[0]); ++i)
    {
      u32 pane = sc_lipPaneList[i];
      u32 color = m_param.charaModelManager->GetLipColor(i);
      pG2DUtil->SetVtxColorElementOfPicturePane(0, pane, color);
    }

    s32 lipColorIndex = m_param.charaModelManager->GetLipColorIndex();
    if (lipColorIndex < 0)
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_04, false);
    }
    else
    {
      pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_04, true);

      gfl2::math::VEC3 panePos = pG2DUtil->GetPaneLocalPos(0, sc_lipPaneList[lipColorIndex]);
      pG2DUtil->SetPanePos(0, PANENAME_DOWN1_PANE_EMPTY_WAKU_04, &panePos);
    }
  }

  // 全力ビーズのON/OFF
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_MATERIAL_ZEN_RING_ON_OFF_CHECKBOX_OF_ZEN_RING_ON_OFF_CHECKBOX, !m_param.charaModelManager->IsBeadsOn());
  }
  // 証のON/OFF
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_MATERIAL_BAG_CHAR_ON_OFF_CHECK_OF_BAG_CHAR_ON_OFF_CHECK, !m_param.charaModelManager->IsCharmOn());
  }
}

void CharaViewerView::UpdatePage4()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // モーション再生有効
  {
    static const u32 sc_motionPaneList[] =
    {
      PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_07,
      PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_08,
      PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_09,
      PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_10,
      PANENAME_DOWN1_PANE_PLAYLIST_CHECKBOX_11,
    };

    for (u32 i = 0; i < sizeof(sc_motionPaneList) / sizeof(sc_motionPaneList[0]); ++i)
    {
      u32 pane = sc_motionPaneList[i];
      pG2DUtil->SetPaneVisible(0, pane, !m_param.charaModelManager->IsMotionEnabled(i));
    }
  }

  // モーション名
  {
    static const u32 sc_motionPaneList[] =
    {
      PANENAME_DOWN1_PANE_PLAYLIST_NAME_07,
      PANENAME_DOWN1_PANE_PLAYLIST_NAME_08,
      PANENAME_DOWN1_PANE_PLAYLIST_NAME_09,
      PANENAME_DOWN1_PANE_PLAYLIST_NAME_10,
      PANENAME_DOWN1_PANE_PLAYLIST_NAME_11,
    };

    char buf[255];
    for (u32 i = 0; i < sizeof(sc_motionPaneList) / sizeof(sc_motionPaneList[0]); ++i)
    {
      u32 pane = sc_motionPaneList[i];

      const char* pMotionName = NULL;
      s32 listIndex = 0;
      s32 motionIndex = 0;
      m_param.charaModelManager->GetCurrentMotionName(i, &pMotionName, &listIndex, &motionIndex);

      //if (listIndex < 0)
      {
        CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s", pMotionName);
      }
      //else
      //{
      //  CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%s [%d:%d]", pMotionName, listIndex, motionIndex);
      //}

      pG2DUtil->SetTextboxPaneString(0, pane, buf);
    }
  }

  // トータルフレーム数
  {
    static const u32 sc_motionPaneList[] =
    {
      PANENAME_DOWN1_PANE_PLAYLIST_TOTAL_FRAME_07,
      PANENAME_DOWN1_PANE_PLAYLIST_TOTAL_FRAME_08,
      PANENAME_DOWN1_PANE_PLAYLIST_TOTAL_FRAME_09,
      PANENAME_DOWN1_PANE_PLAYLIST_TOTAL_FRAME_10,
      PANENAME_DOWN1_PANE_PLAYLIST_TOTAL_FRAME_11,
    };
    char buf[255];

    for (u32 i = 0; i < sizeof(sc_motionPaneList) / sizeof(sc_motionPaneList[0]); ++i)
    {
      u32 pane = sc_motionPaneList[i];
      u32 frame = m_param.charaModelManager->GetCurrentMotionTotalFrame(i);

      CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", frame);

      pG2DUtil->SetTextboxPaneString(0, pane, buf);
    }
  }

  // 現在のフレーム数
  {
    static const u32 sc_motionPaneList[] =
    {
      PANENAME_DOWN1_PANE_PLAYLIST_CUR_FRAME_07,
      PANENAME_DOWN1_PANE_PLAYLIST_CUR_FRAME_08,
      PANENAME_DOWN1_PANE_PALYLIST_CUR_FRAME_09,
      PANENAME_DOWN1_PANE_PLAYLIST_CUR_FRAME_10,
      PANENAME_DOWN1_PANE_PLAYLIST_CUR_FRAME_11,
    };
    char buf[255];

    for (u32 i = 0; i < sizeof(sc_motionPaneList) / sizeof(sc_motionPaneList[0]); ++i)
    {
      u32 pane = sc_motionPaneList[i];
      u32 frame = m_param.charaModelManager->GetCurrentMotionFrame(i);

      CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", frame);

      pG2DUtil->SetTextboxPaneString(0, pane, buf);
    }
  }

  // 再生ボタン
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_MOTION_PLAY_00, !m_param.charaModelManager->IsMotionPlaying());
  }

  // ループボタン
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_MOTION_LOOP_00, m_param.charaModelManager->IsMotionLoop());
  }

  // 移動量ボタン
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_TRANSLATE_TOGGLE, !m_param.charaModelManager->GetTranslateFlag());
  }

  // FPS表示
  {
    const char* pFpsText = (m_param.charaModelManager->IsFpsMode30())
      ? "30FPS"
      : "15FPS";
    pG2DUtil->SetTextboxPaneString(0, PANENAME_DOWN1_PANE_FPS_TEXT, pFpsText);
  }

  // 表情
  {
    static const u32 sc_facePaneLine[] =
    {
      PANENAME_DOWN1_PANE_EYE_NAME,
      PANENAME_DOWN1_PANE_MOUTH_NAME,
    };

    static const char* (CharaViewerCharaModelManager::*sc_getFaceFuncList[])() const =
    {
      &CharaViewerCharaModelManager::GetCurrentEyeName,
      &CharaViewerCharaModelManager::GetCurrentMouthName,
    };

    for (u32 i = 0; i < sizeof(sc_facePaneLine) / sizeof(sc_facePaneLine[0]); ++i)
    {
      u32 pane = sc_facePaneLine[i];

      const char* pName = (m_param.charaModelManager->*sc_getFaceFuncList[i])();
      pG2DUtil->SetTextboxPaneString(0, pane, pName);
    }
  }

  // 自動目パチ
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_AUTO_BLINK_TOGGLE, !m_param.charaModelManager->IsAutoBlinkOn());
  }
}

void CharaViewerView::UpdatePage5()
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();

  // 操作モード
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_CTRLMODE_BTN_MDL, !m_param.charaModelManager->IsControllModeCamera());
  }

  // グリッド
  {
    pG2DUtil->SetPaneVisible(0,PANENAME_DOWN1_PANE_GRID_BTN_OFF, !m_param.charaModelManager->IsGridVisible());
  }

  // 画角：数値
  {
    char buf[255];
    u32 cameraFovy = m_param.charaModelManager->GetCameraFovy();
    CHARA_VIEWER_SPRINTF(buf, sizeof(buf), "%u", cameraFovy);
    pG2DUtil->SetTextboxPaneString(0,PANENAME_DOWN1_PANE_ANGLE_SELECTOR_VALUE, buf);
  }

  // 画角タイプ
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_FIELD_CHECKBOX, !(m_param.charaModelManager->GetCameraFovyType() == CharaViewerCharaModelManager::FOVY_TYPE_FIELD));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_BATTLE_CHECKBOX, !(m_param.charaModelManager->GetCameraFovyType() == CharaViewerCharaModelManager::FOVY_TYPE_BATTLE));
  }

  // near/farタイプ
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_NEAR_CHECKBOX, !(m_param.charaModelManager->GetCameraNearFarType() == CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_NEAR));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_MID_CHECKBOX, !(m_param.charaModelManager->GetCameraNearFarType() == CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_MIDDLE));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_FAR_CHECKBOX, !(m_param.charaModelManager->GetCameraNearFarType() == CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_FAR));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CAMERA_BATTLE_CHECKBO_00, !(m_param.charaModelManager->GetCameraNearFarType() == CharaViewerCharaModelManager::NEAR_FAR_TYPE_BATTLE));
  }

  // 背景クリアカラー
  {
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_0_CHECK, !(m_param.charaModelManager->GetClearColorType() == 0));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_1_CHECK, !(m_param.charaModelManager->GetClearColorType() == 1));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_2_CHECK, !(m_param.charaModelManager->GetClearColorType() == 2));
    pG2DUtil->SetPaneVisible(0, PANENAME_DOWN1_PANE_CLEAR_COLOR_3_CHECK, !(m_param.charaModelManager->GetClearColorType() == 3));
  }
}

void CharaViewerView::SetPicPaneCol( u32 pane, u8 r, u8 g , u8 b, u8 a)
{
  nw::ut::Color8 col(r,g,b,a);
  gfl2::lyt::LytPicture* picPane = this->GetG2DUtil()->GetLayoutWork(0)->GetPicturePane(pane);

  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_LT,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_RT,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_LB,col);
  picPane->SetVtxColor( nw::lyt::VERTEXCOLOR_RB,col);
  
}

void CharaViewerView::SlideModelSelector( bool isForward )
{
  m_param.charaModelManager->SlideModelSelector(isForward);
}

void CharaViewerView::SlideCharaSelector(bool isForward)
{
  m_param.charaModelManager->SlideCharaSelector(isForward);
}

void CharaViewerView::LoadCharactor()
{
  m_param.charaModelManager->LoadCharactor();
}

void CharaViewerView::UnloadCharactor()
{
  m_param.charaModelManager->UnloadCharactor();
}

void CharaViewerView::SetModelType( CharaViewerView::ModelType modelType )
{
  if (modelType == CharaViewerView::BATTLE_MODEL)
  {
    m_param.charaModelManager->SetLoadModelType(CharaViewerCharaModelManager::MODEL_TYPE_BATTLE);
  }
  else
  {
    m_param.charaModelManager->SetLoadModelType(CharaViewerCharaModelManager::MODEL_TYPE_FIELD);
  }
}

void CharaViewerView::LocateOrigin()
{
  m_param.charaModelManager->LocateOrigin();
}

void CharaViewerView::SlideScaleSelector( bool isForward )
{
  m_param.charaModelManager->SlideScaleSelector(isForward);
}

void CharaViewerView::SlideSkinSelector( bool isForward )
{
  m_param.charaModelManager->SlideSkinSelector(isForward);
}

void CharaViewerView::SlideFrontHairSelector( bool isForward )
{
  m_param.charaModelManager->SlideFrontHairSelector(isForward);
}

void CharaViewerView::SlideHairStyleSelector( bool isForward )
{
  m_param.charaModelManager->SlideHairStyleSelector(isForward);
}

void CharaViewerView::SlideHeadWearSelector( bool isForward )
{
  m_param.charaModelManager->SlideHeadWearSelector(isForward);
}

void CharaViewerView::SlideHairAccessorySelector( bool isForward )
{
  m_param.charaModelManager->SlideHairAccessorySelector(isForward);
}

void CharaViewerView::SlideEyeWearSelector( bool isForward )
{
  m_param.charaModelManager->SlideEyeWearSelector(isForward);
}

void CharaViewerView::SlideTopsSelector( bool isForward )
{
  m_param.charaModelManager->SlideTopsSelector(isForward);
}

void CharaViewerView::SlideOnepieceSelector( bool isForward )
{
  m_param.charaModelManager->SlideOnepieceSelector(isForward);
}

void CharaViewerView::SlideBottomsSelector( bool isForward )
{
  m_param.charaModelManager->SlideBottomsSelector(isForward);
}

void CharaViewerView::SlideSocksSelector( bool isForward )
{
  m_param.charaModelManager->SlideSocksSelector(isForward);
}

void CharaViewerView::SlideShoesSelector( bool isForward )
{
  m_param.charaModelManager->SlideShoesSelector(isForward);
}

void CharaViewerView::SlideBagSelector( bool isForward )
{
  m_param.charaModelManager->SlideBagSelector(isForward);
}

void CharaViewerView::SlideBangleSelector( bool isForward )
{
  m_param.charaModelManager->SlideBangleSelector(isForward);
}

void CharaViewerView::SlideCharmSelector( bool isForward )
{
  m_param.charaModelManager->SlideCharmSelector(isForward);
}

void CharaViewerView::SlideLipSelector( bool isForward )
{
  m_param.charaModelManager->SlideLipSelector(isForward);
}

void CharaViewerView::SlideSituationSelector( bool isForward )
{
  m_param.charaModelManager->SlideSituationSelector(isForward);
}

void CharaViewerView::SlideEyeSelector( bool isForward )
{
  m_param.charaModelManager->SlideEyeSelector(isForward);
}

void CharaViewerView::SlideMouthSelector( bool isForward )
{
  m_param.charaModelManager->SlideMouthSelector(isForward);
}

void CharaViewerView::ToggleAutoBlink()
{
  m_param.charaModelManager->ToggleAutoBlink();
}

void CharaViewerView::SetClothColorHat()
{
  m_param.charaModelManager->SetClothColorHat();
}

void CharaViewerView::SetClothColorAccHair()
{
  m_param.charaModelManager->SetClothColorAccHair();
}

void CharaViewerView::SetClothColorAccEye()
{
  m_param.charaModelManager->SetClothColorAccEye();
}

void CharaViewerView::SetClothColorTops()
{
  m_param.charaModelManager->SetClothColorTops();
}

void CharaViewerView::SetClothColorOnepiece()
{
  m_param.charaModelManager->SetClothColorOnepiece();
}

void CharaViewerView::SetClothColorBottoms()
{
  m_param.charaModelManager->SetClothColorBottoms();
}

void CharaViewerView::SetClothColorLegs()
{
  m_param.charaModelManager->SetClothColorLegs();
}

void CharaViewerView::SetClothColorShoes()
{
  m_param.charaModelManager->SetClothColorShoes();
}

void CharaViewerView::SetClothColorBag()
{
  m_param.charaModelManager->SetClothColorBag();
}

void CharaViewerView::SetClothColorBangle()
{
  m_param.charaModelManager->SetClothColorBangle();
}

void CharaViewerView::SelectSkinColor( u32 index )
{
  m_param.charaModelManager->ChangeSkinColor(index);
}

void CharaViewerView::SelectHairColor( u32 index )
{
  m_param.charaModelManager->ChangeHairColor(index);
}

void CharaViewerView::SelectClothColor( u32 index )
{
  m_param.charaModelManager->ChangeClothColor(index);
}

void CharaViewerView::SelectEyeblowColor( u32 index )
{
  m_param.charaModelManager->ChangeEyeblowColor(index);
}

void CharaViewerView::SelectContactColor( u32 index )
{
  m_param.charaModelManager->ChangeEyeColor(index);
}

void CharaViewerView::SelectLipColor( u32 index )
{
  m_param.charaModelManager->ChangeLipColor(index);
}

void CharaViewerView::ToggleBeadsOnOff()
{
  m_param.charaModelManager->ToggleBeadsOnOff();
}

void CharaViewerView::ToggleCharmOnOff()
{
  m_param.charaModelManager->ToggleCharmOnOff();
}

void CharaViewerView::LoadCollisionCheckParts()
{
  m_param.charaModelManager->LoadCollisionCheckParts();
}

void CharaViewerView::LoadDataSizeCheckParts()
{
  m_param.charaModelManager->LoadDataSizeCheckParts();
}

void CharaViewerView::ToggleLoopFlag()
{
  m_param.charaModelManager->ToggleLoopFlag();
}

void CharaViewerView::PlaySingleFrame( bool isForward )
{
  m_param.charaModelManager->PlaySingleFrame(isForward);
}

void CharaViewerView::PlayMotion()
{
  m_param.charaModelManager->PlayMotion();
}

void CharaViewerView::SlideMotionSelector( u8 index, bool isForward )
{
  m_param.charaModelManager->SlideMotionSelector(index, isForward);
}

void CharaViewerView::ToggleMotion( u8 index )
{
  m_param.charaModelManager->ToggleMotion(index);
}

void CharaViewerView::PlayTPose()
{
  m_param.charaModelManager->PlayTPose();
}

void CharaViewerView::PlayStackCheckMotion()
{
  m_param.charaModelManager->PlayStackCheckMotion();
}

void CharaViewerView::ToggleAllPlayFlag()
{
  m_param.charaModelManager->ToggleAllPlayFlag();
}

void CharaViewerView::ToggleTranslateFlag()
{
  m_param.charaModelManager->ToggleTranslateFlag();
}

void CharaViewerView::SlideFpsSelector( bool isForward )
{
  m_param.charaModelManager->SlideFpsSelector(isForward);
}

void CharaViewerView::ResetLight()
{
  m_param.charaModelManager->ResetLightPosition();
}

void CharaViewerView::ResetCamera()
{
  m_param.charaModelManager->ResetCameraPosition();
}

void CharaViewerView::SetCameraMode( CameraMode mode )
{
  // near/far
  CharaViewerCharaModelManager::NearFarType type = CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_NEAR;

  switch (mode)
  {
  case NF_FIELD_NEAR:
    type = CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_NEAR;
    break;
  case NF_FIELD_MID:
    type = CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_MIDDLE;
    break;
  case NF_FIELD_FAR:
    type = CharaViewerCharaModelManager::NEAR_FAR_TYPE_FIELD_FAR;
    break;
  case NF_BATTLE:
    type = CharaViewerCharaModelManager::NEAR_FAR_TYPE_BATTLE;
    break;
  }

  m_param.charaModelManager->SetCameraNearFarType(type);
}

void CharaViewerView::SetCameraAngle( CameraAngle angle )
{
  // fovy
  CharaViewerCharaModelManager::FovyType type = (angle == CharaViewerView::FIELD)
    ? CharaViewerCharaModelManager::FOVY_TYPE_FIELD
    : CharaViewerCharaModelManager::FOVY_TYPE_BATTLE;
  m_param.charaModelManager->SetCameraFovyType(type);
}

void CharaViewerView::SlideCameraAngleSelector( bool isForward )
{
  m_param.charaModelManager->SlideCameraAngleSelector(isForward);
}

void CharaViewerView::ToggleCameraControllMode()
{
  m_param.charaModelManager->ToggleCameraControllMode();
}

void CharaViewerView::ToggleGrid()
{
  m_param.charaModelManager->ToggleGrid();
}

void CharaViewerView::TogglePolygon()
{
}

void CharaViewerView::ToggleCollision()
{
}

void CharaViewerView::SetClearColorType(u32 clearColorType)
{
  m_param.charaModelManager->SetClearColorType(clearColorType);
}

void CharaViewerView::Capture()
{
  m_param.charaModelManager->Capture();
}

void CharaViewerView::CaptureAll()
{
  m_param.charaModelManager->CaptureAll();
}

void CharaViewerView::DressUpRand()
{
  m_param.charaModelManager->DressUpRand();
}

void CharaViewerView::DressUpDump()
{
  m_param.charaModelManager->DressUpDump();
}

void CharaViewerView::ResetAllParts()
{
  m_param.charaModelManager->ResetAllParts();
}

void CharaViewerView::SaveCurrentPartsSetting()
{
  m_param.charaModelManager->SaveCurrentPartsSetting();
}

GFL_NAMESPACE_END(chara_viewer)
  GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
