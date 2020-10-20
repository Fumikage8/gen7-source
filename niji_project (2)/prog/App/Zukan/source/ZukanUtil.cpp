//==============================================================================
/**
 @file    ZukanUtil.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.09.04
 @brief   図鑑の便利機能
 */
//==============================================================================


// gfl2のインクルード
#include <ui/include/gfl2_UI.h>
#include <cx/include/gfl2_StreamingUncomp.h>
#include <fs/include/gfl2_Fs.h>
#include <Layout/include/gfl2_Layout.h>

#include <math/include/gfl2_math_convert.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_ScrollBar.h>

#include <AppLib/include/Tool/app_tool_MenuCursor.h>
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>
#include <AppLib/include/Tool/PokeIcon.h>

#include <AppLib/include/Ui/UIView.h>

#include <System/include/GflUse.h>
#include <PokeTool/include/PokeTool.h>

#include <Sound/include/Sound.h>

// 図鑑のインクルード
#include "ZukanDataManager.h"
#include "ZukanTool.h"

#include "ZukanUtil.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


static const f32 ZUKAN_ROTOM_VOICE_VOLUME = 0.4f;


//==============================================================================
//
//  ZukanUtil
//
//==============================================================================
void ZukanUtil::CreateButtonManager(
    app::util::Heap*                  appHeap,
    app::ui::UIView*                  uiView,
    u32                               res_i,
    u32                               wk_i,
    u32                               buttonIdNum,
    const ZukanType::ButtonConstInfo* buttonConstInfo,
    gfl2::lyt::LytWk**                lytWkArray,
    ZukanType::ButtonPaneInfo*        buttonPaneInfo
)
{
  app::util::G2DUtil* pG2DUtil = uiView->GetG2DUtil();
  gfl2::lyt::LytMultiResID lytRes = pG2DUtil->GetLayoutResourceID( res_i );  // res_?
 
  if(lytWkArray)
  {
    app::ui::ButtonInfoLytWk* buttonInfoLytWk = GFL_NEW_ARRAY(appHeap->GetDeviceHeap()->GetLowerHandle()) app::ui::ButtonInfoLytWk[buttonIdNum];

    for(u32 i=0; i<buttonIdNum; ++i)
    {
      gfl2::lyt::LytParts* buttonPartsPane = lytWkArray[i]->GetPartsPane(buttonConstInfo[i].parts_pane_index);
      gfl2::lyt::LytPane*  buttonBoundPane = lytWkArray[i]->GetPane(buttonPartsPane, buttonConstInfo[i].bound_pane_index, &lytRes);
      
      buttonInfoLytWk[i].button_id              = buttonConstInfo[i].button_id             ;
      buttonInfoLytWk[i].picture_pane           = buttonPartsPane                          ;
      buttonInfoLytWk[i].bound_pane             = buttonBoundPane                          ;
      buttonInfoLytWk[i].lyt_wk                 = lytWkArray[i]                            ;
      buttonInfoLytWk[i].touch_anime_index      = buttonConstInfo[i].touch_anime_index     ;
      buttonInfoLytWk[i].release_anime_index    = buttonConstInfo[i].release_anime_index   ;
      buttonInfoLytWk[i].cancel_anime_index     = buttonConstInfo[i].cancel_anime_index    ;
      buttonInfoLytWk[i].key_select_anime_index = buttonConstInfo[i].key_select_anime_index;
      buttonInfoLytWk[i].active_anime_index     = buttonConstInfo[i].active_anime_index    ;
      buttonInfoLytWk[i].passive_anime_index    = buttonConstInfo[i].passive_anime_index   ;
 
      buttonPaneInfo[i].partsPane = buttonPartsPane;
      buttonPaneInfo[i].boundPane = buttonBoundPane;
      if(buttonConstInfo[i].cursor_pane_index != gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      {
        buttonPaneInfo[i].cursorPane = lytWkArray[i]->GetPane(buttonPartsPane, buttonConstInfo[i].cursor_pane_index, &lytRes);
      }
    }
 
    uiView->CreateButtonManager(appHeap, buttonInfoLytWk, buttonIdNum);  // 親であるapp::ui::UIResponderのメンバ
    
    GFL_DELETE_ARRAY(buttonInfoLytWk);
  }
  else
  {
    gfl2::lyt::LytWk* lytWk = uiView->GetLayoutWork( wk_i );  // wk_?

    app::ui::ButtonInfoEx* buttonInfoEx = GFL_NEW_ARRAY(appHeap->GetDeviceHeap()->GetLowerHandle()) app::ui::ButtonInfoEx[buttonIdNum];
    
    for(u32 i=0; i<buttonIdNum; ++i)
    {
      gfl2::lyt::LytParts* buttonPartsPane = lytWk->GetPartsPane(buttonConstInfo[i].parts_pane_index);
      gfl2::lyt::LytPane*  buttonBoundPane = lytWk->GetPane(buttonPartsPane, buttonConstInfo[i].bound_pane_index, &lytRes);
      
      buttonInfoEx[i].button_id              = buttonConstInfo[i].button_id             ;
      buttonInfoEx[i].picture_pane           = buttonPartsPane                          ;
      buttonInfoEx[i].bound_pane             = buttonBoundPane                          ;
      buttonInfoEx[i].touch_anime_index      = buttonConstInfo[i].touch_anime_index     ;
      buttonInfoEx[i].release_anime_index    = buttonConstInfo[i].release_anime_index   ;
      buttonInfoEx[i].cancel_anime_index     = buttonConstInfo[i].cancel_anime_index    ;
      buttonInfoEx[i].key_select_anime_index = buttonConstInfo[i].key_select_anime_index;
      buttonInfoEx[i].active_anime_index     = buttonConstInfo[i].active_anime_index    ;
      buttonInfoEx[i].passive_anime_index    = buttonConstInfo[i].passive_anime_index   ;
 
      buttonPaneInfo[i].partsPane = buttonPartsPane;
      buttonPaneInfo[i].boundPane = buttonBoundPane;
      if(buttonConstInfo[i].cursor_pane_index != gfl2::lyt::LYTSYS_PANEINDEX_ERROR)
      {
        buttonPaneInfo[i].cursorPane = lytWk->GetPane(buttonPartsPane, buttonConstInfo[i].cursor_pane_index, &lytRes);
      }
    }
 
    uiView->CreateButtonManager(appHeap, lytWk, buttonInfoEx, buttonIdNum);  // 親であるapp::ui::UIResponderのメンバ
    
    GFL_DELETE_ARRAY(buttonInfoEx);
  }

  app::tool::ButtonManager* buttonManager = uiView->GetButtonManager();  // 親であるapp::ui::UIResponderのメンバ
  for(u32 i=0; i<buttonIdNum; ++i)
  {
    if(buttonConstInfo[i].bind_key_flag)
    {
      buttonManager->SetButtonBindKey(buttonConstInfo[i].button_id, buttonConstInfo[i].bind_key, buttonConstInfo[i].bind_key_mode);
    }
    buttonManager->SetButtonSelectSE(buttonConstInfo[i].button_id, buttonConstInfo[i].se_id);
  }
}

void ZukanUtil::DestroyButtonManager(
    app::ui::UIView*                  uiView
)
{
  uiView->ReleaseButtonManager();  // 親であるapp::ui::UIResponderのメンバ
}

app::tool::PaneListView* ZukanUtil::CreatePaneList(
    app::util::Heap*                        appHeap,
    app::ui::UIView*                        uiView,
    u32                                     res_i,
    u32                                     wk_i,
    app::tool::PaneListView::Listener*      paneListViewListener,
    u32                                     paneListElemNum,
    const ZukanType::PaneListElemConstInfo* paneListElemConstInfo,
    app::tool::PaneList::PANE_DATA*         paneListData,
    const ZukanType::PaneListConstInfo*     paneListConstInfo,
    gfl2::lyt::LytPane*                     listStartPane,
    app::tool::PaneList::CallBack*          paneListCallBack,
    app::tool::MenuCursor*                  menuCursor,
    gfl2::ui::DeviceManager*                uiDeviceManager,
    const ZukanType::ScrollBarConstInfo*    scrollBarConstInfo,
    u32                                     paneListSetupParamValMax,
    u32                                     paneListSetupParamListPos,
    f32                                     paneListSetupParamListScroll,
    bool                                    paneListEffectFlag
)
{
  app::tool::PaneListView* paneListView = NULL;

  gfl2::lyt::LytWk* lytWk = uiView->GetLayoutWork( wk_i );  // wk_?
  app::util::G2DUtil* pG2DUtil = uiView->GetG2DUtil();
  gfl2::lyt::LytMultiResID lytRes = pG2DUtil->GetLayoutResourceID( res_i );  // res_?
  
  paneListView = GFL_NEW(appHeap->GetDeviceHeap()) app::tool::PaneListView(appHeap, paneListEffectFlag);
  paneListView->SetListener(paneListViewListener);
  uiView->AddSubView(paneListView);

  u32 scrollBarScrollMax = 0;

  // app::tool::PaneList::SETUP_PARAMを使ってCreatePaneListする
  {
    // app::tool::PaneList::PANE_DATAに値を設定する
    for(u32 i=0; i<paneListElemNum; ++i)
    {
      gfl2::lyt::LytParts*    partsPane  = lytWk->GetPartsPane(paneListElemConstInfo[i].partsPaneIndex);
      gfl2::lyt::LytTextBox*  textPane   = (paneListElemConstInfo[i].textPaneIndex != gfl2::lyt::LYTSYS_PANEINDEX_ERROR) ? (lytWk->GetTextBoxPane(partsPane, paneListElemConstInfo[i].textPaneIndex, &lytRes)) : (NULL);
      gfl2::lyt::LytBounding* boundPane  = lytWk->GetBoundingPane(partsPane, paneListElemConstInfo[i].boundPaneIndex, &lytRes);
      gfl2::lyt::LytPane*     cursorPane = lytWk->GetPane(partsPane, paneListElemConstInfo[i].cursorPaneIndex, &lytRes);
 
      paneListData[i].base         = partsPane;
      paneListData[i].text         = textPane;
      paneListData[i].bound        = boundPane;
      paneListData[i].cursor_pos   = cursorPane;
      paneListData[i].holdAnime    = paneListElemConstInfo[i].holdAnime;
      paneListData[i].releaseAnime = paneListElemConstInfo[i].releaseAnime;
      paneListData[i].cancelAnime  = paneListElemConstInfo[i].cancelAnime;
      paneListData[i].selectAnime  = paneListElemConstInfo[i].selectAnime;
    }

    // app::tool::PaneList::SETUP_PARAMに値を設定する
    app::tool::PaneList::SETUP_PARAM paneListSetupParam;
    {
      paneListSetupParam.heap          = appHeap->GetDeviceHeap();
      paneListSetupParam.pLytWk        = lytWk;
      paneListSetupParam.paneList      = paneListData;
      paneListSetupParam.paneListMax   = paneListElemNum;
      paneListSetupParam.valMax        = paneListSetupParamValMax;
      paneListSetupParam.listPosMax    = paneListConstInfo->listPosMax;
      paneListSetupParam.listPos       = paneListSetupParamListPos;
      paneListSetupParam.listScroll    = paneListSetupParamListScroll;
      paneListSetupParam.defPY         = paneListConstInfo->defPY;
      paneListSetupParam.defSX         = paneListConstInfo->defSX;
      paneListSetupParam.defSY         = paneListConstInfo->defSY;
      paneListSetupParam.writeOffsetY  = paneListConstInfo->writeOffsetY;
      paneListSetupParam.listStartPane = listStartPane;  // @fix NMCat[2101] ポケモンナンバー上部に上のアイコンの影が被って表示される  ←ペインの並び順を変更できるように、リストの親ペインを渡す。
      paneListSetupParam.cbFunc        = paneListCallBack;
      paneListSetupParam.cbWork        = NULL;
      paneListSetupParam.uiDevMan      = uiDeviceManager;
      paneListSetupParam.menuCursor    = menuCursor;
    }
  
    paneListView->CreatePaneList(&paneListSetupParam);

    if(paneListSetupParam.valMax > paneListSetupParam.listPosMax)
    {
      scrollBarScrollMax = paneListSetupParam.valMax - paneListSetupParam.listPosMax;
    }

    {
      // @fix GFNMCat[712] 「ポケモン図鑑」の「検索&並び画面」で特定操作をすると枠外にカーソルが移動し、決定するとアサート
      //                   タッチできないはずの一番下のペインにタッチできてしまっていたので、そこをタッチ範囲から外すようにした。
      // タッチ範囲
      gfl2::math::VEC3    pos      = paneListSetupParam.paneList[0].bound->GetTranslate();  // Y方向に関してはわずかなずれを修正するためにローカル座標。
      gfl2::math::Vector3 worldPos = GetPaneWorldPosIgnoringSR(paneListSetupParam.paneList[0].bound);  // @fix NMCat[801] ポケモン図鑑：画面右側のタッチスクロールが効かない
                                                                                                       //                 X方向に関してワールド座標を取得していなかったのが原因。
      u32 pos_max = paneListSetupParam.listPosMax;
      if( paneListSetupParam.valMax < pos_max )
      {
        pos_max = paneListSetupParam.valMax;
      }
      f32 touchAreaLX = worldPos.x - paneListSetupParam.defSX/2.0f;
      f32 touchAreaRX = worldPos.x + paneListSetupParam.defSX/2.0f;
      f32 touchAreaUY = paneListSetupParam.defPY + pos.y + paneListSetupParam.defSY/2.0f;
      f32 touchAreaDY = touchAreaUY - paneListSetupParam.defSY * pos_max;
 
      paneListView->GetPaneList()->MakeTouchArea(
          touchAreaLX,
          touchAreaRX,
          touchAreaUY,
          touchAreaDY
      );
    }
  }

  // app::util::ScrollBar::SETUP_PARAMを使ってCreateScrollBarする
  {
    gfl2::lyt::LytParts* partsPane  = lytWk->GetPartsPane(scrollBarConstInfo->parts_index);
    gfl2::lyt::LytPane*  markPane   = lytWk->GetPane(partsPane, scrollBarConstInfo->mark_index  , &lytRes);
    gfl2::lyt::LytPane*  topPane    = lytWk->GetPane(partsPane, scrollBarConstInfo->top_index   , &lytRes);
    gfl2::lyt::LytPane*  bottomPane = lytWk->GetPane(partsPane, scrollBarConstInfo->bottom_index, &lytRes);

    app::util::ScrollBar::SETUP_PARAM scrollBarSetupParam;
    {
      scrollBarSetupParam.heap        = appHeap->GetDeviceHeap();
      scrollBarSetupParam.dir         = app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL;
      scrollBarSetupParam.max         = scrollBarScrollMax;
      
      scrollBarSetupParam.lytwk = NULL;
     
      // ↓使われないと思うが一応セットしておく
      scrollBarSetupParam.res_id       = lytRes;
      scrollBarSetupParam.parts_index  = scrollBarConstInfo->parts_index ;
      scrollBarSetupParam.mark_index   = scrollBarConstInfo->mark_index  ;
      scrollBarSetupParam.top_index    = scrollBarConstInfo->top_index   ;
      scrollBarSetupParam.bottom_index = scrollBarConstInfo->bottom_index;
      // ↑使われないと思うが一応セットしておく

      scrollBarSetupParam.parts_pane  = partsPane ;
      scrollBarSetupParam.mark_pane   = markPane  ;
      scrollBarSetupParam.top_pane    = topPane   ;
      scrollBarSetupParam.bottom_pane = bottomPane;
    }

    paneListView->CreateScrollBar(&scrollBarSetupParam);
  }

  app::tool::PaneList* paneList = paneListView->GetPaneList();
  paneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_KEY, true, false, true, false );  // 大森さん要望: PAGE_SKIP_LR_BUTTONから左右キーに変更
  paneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, app::tool::PaneList::SE_NONE );

  return paneListView;
}

void ZukanUtil::DestroyPaneList(
    app::ui::UIView*                        /*uiView*/,
    app::tool::PaneListView*                paneListView
)
{
  paneListView->DeleteScrollBar();
  paneListView->DeletePaneList();

  paneListView->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  GFL_SAFE_DELETE(paneListView);
}

void ZukanUtil::RemovePaneListViewFromSuperView(
    app::tool::PaneListView*                paneListView
)
{
  paneListView->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ
}

void ZukanUtil::MakePaneListSetupParam(
    const ZukanType::PaneListConstInfo*   paneListConstInfo,          
    u32                                   paneListSetupParamValMax,    
    u32                                   currVal,                     
    u32*                                  paneListSetupParamListPos,  
    f32*                                  paneListSetupParamListScroll
)
{
  GFL_ASSERT(paneListConstInfo->listPosMax > 0);   // 開発中に気付かせるためのASSERT
  GFL_ASSERT(currVal < paneListSetupParamValMax);  // 開発中に気付かせるためのASSERT

  s32 listPos    = 0;
  s32 listScroll = 0;
  if(paneListConstInfo->listPosMax >= paneListSetupParamValMax)
  {
    listPos    = currVal;
    listScroll = 0;
  }
  else
  {
    const s32 backNo    = paneListSetupParamValMax - currVal -1;  // 後ろから数えたときの「現在の項目」の番号(0開始)
    const s32 lastTopNo = paneListSetupParamValMax - paneListConstInfo->listPosMax;  // 最後までスクロールしたときの先頭にある項目の番号(0開始)

    listPos = ((paneListConstInfo->listPosMax%2)==0)?((paneListConstInfo->listPosMax-1)/2):(paneListConstInfo->listPosMax/2);  // 真ん中にしておく

    if(static_cast<s32>(currVal) < listPos) listPos = currVal;  // 真ん中より前にしかいかない
    else if(static_cast<s32>(currVal) > lastTopNo + listPos) listPos = paneListConstInfo->listPosMax - backNo -1;  // 真ん中より後ろにしかいなかい

    app::tool::PaneList::MakeListPos(
      static_cast<s32>(paneListSetupParamValMax),       // 項目の個数
      static_cast<s32>(paneListConstInfo->listPosMax),  // 画面に表示されている有効なペインの個数
      static_cast<s32>(currVal),                        // 現在の項目(0<= currVal < paneListSetupParamValMax)
      &listPos,                                         // [in,out] 現在のペイン位置(0<= listPos <paneListConstInfo->listPosMax)
      &listScroll                                       // [out]
    );
  }
  if(0<=listPos && 0<=listScroll)
  {
    *paneListSetupParamListPos    = static_cast<u32>(listPos);
    *paneListSetupParamListScroll = static_cast<f32>(listScroll);
  }
  else
  {
    GFL_ASSERT(0);    // 開発中に気付かせるためのASSERT
    *paneListSetupParamListPos    = 0;
    *paneListSetupParamListScroll = 0.0f;
  }
}

void* ZukanUtil::CreatePokeIconTexture(
    gfl2::heap::HeapBase*         devMem,
    gfl2::heap::HeapBase*         tempMem,
    gfl2::lyt::LytPicture*        pane,
    u32                           materialIndex,
    u32                           textureIndex,
    const PokeTool::SimpleParam&  sp,
    bool                          flip,
    const ZukanDataManager*       dataManager,
    gfl2::lyt::LytResTextureInfo* a_textureInfo
)
{
  gfl2::fs::ArcFile::ARCDATID datId = app::tool::PokeIcon::GetResourceIndex(&sp, flip);
  
  void*                    dataBuf = dataManager->GetDataBuf(ZukanDataManager::DATA_ID_POKE_ICON_RES2D);
  const gfl2::fs::ArcFile* arcFile = dataManager->GetArcFile(ZukanDataManager::DATA_ID_POKE_ICON_RES2D);

  // オフセット
  size_t arcOffsetHead = 0;
  arcFile->GetOffsetFromArchiveTop(&arcOffsetHead, 0);
  size_t arcOffsetCurr = 0;
  arcFile->GetOffsetFromArchiveTop(&arcOffsetCurr, datId);
  size_t bufOffset = arcOffsetCurr - arcOffsetHead;
 
  // サイズ
  //size_t compSize = 0;  // 展開にはリアルサイズのほうを使うので、こちらはコメントアウト。
  //arcFile->GetDataSize(&compSize, datId, tempMem);
  size_t realCompSize = 0; 
  arcFile->GetRealDataSize(&realCompSize, datId, tempMem);  // 一気読みしているので、ArcSrcを使いたくない。fs側で防ぐフラグがないのでこのままにしておく。

  // 圧縮データの先頭アドレス
  void* compBuf = reinterpret_cast<void*>( (reinterpret_cast<u32>(dataBuf)) + (static_cast<u32>(bufOffset)) );

  // 展開
  u32 uncompSize = gfl2::cx::GetUncompressedSize(compBuf, realCompSize);
  void* uncompBuf = GflHeapAllocMemoryAlign(devMem, uncompSize, 128);
  u32 uncompRes = gfl2::cx::SecureUncompress( compBuf, realCompSize, uncompBuf, tempMem );
  if(uncompRes > 0)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。解凍エラー。
    GflHeapSafeFreeMemory(uncompBuf);
    return NULL;
  }

  // テクスチャ情報
  gfl2::lyt::LytResTextureInfo textureInfo;
  bool loadTexRes = nw::lyt::LoadTexture(&textureInfo, uncompBuf, uncompSize);
  if(loadTexRes == false)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャをデバイスにロードするのに失敗。
    GflHeapSafeFreeMemory(uncompBuf);
    return NULL;
  }

  // ペインに貼る
  bool replaceTexRes = app::tool::LytTexReplaceSystem::ReplaceTextureStatic(
      &textureInfo,
      pane,
      materialIndex,
      textureIndex
  );
  if(replaceTexRes == false)
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。テクスチャを置き換えるのに失敗。
    GflHeapSafeFreeMemory(uncompBuf);
    return NULL;
  }

  if(a_textureInfo) a_textureInfo->Set(textureInfo);
  return uncompBuf;
}

gfl2::math::Vector3 ZukanUtil::GetPaneWorldPosIgnoringSR(const gfl2::lyt::LytPane* pane)
{
  gfl2::math::VEC3 pos(0,0,0);
  const gfl2::lyt::LytPane* p = pane;
  while(p)
  {
    pos = pos + p->GetTranslate();
    p = p->GetParent();
  }
  return gfl2::math::ConvertNwVec3ToGfVec3(pos);
}

void ZukanUtil::StartLytWaitAnim(app::util::G2DUtil* pG2DUtil, const u32* waitAnimIndexArray, u32 waitAnimIndexArraySize)
{
  for(u32 i=0; i<waitAnimIndexArraySize; ++i)
  {
    if(waitAnimIndexArray[i] != gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR)
    {
      pG2DUtil->StartAnime(
        i,  // iはwk_?
        waitAnimIndexArray[i]
      );
    }
  }
}
bool ZukanUtil::IsLytWaitAnimEnd(app::util::G2DUtil* pG2DUtil, u32* waitAnimIndexArray, u32 waitAnimIndexArraySize)  // waitAnimIndexArrayはアニメが終わったら初期化される
{
  bool ret = true;
  
  for(u32 i=0; i<waitAnimIndexArraySize; ++i)
  {
    if(waitAnimIndexArray[i] != gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR)
    {
      bool isAnimEnd = pG2DUtil->IsAnimeEnd(i, waitAnimIndexArray[i]);  // iはwk_?
      if(isAnimEnd)
      {
        waitAnimIndexArray[i] = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  // 終わったら初期化しておく
      }
      else
      {
        ret = false;
      }
    }
  }

  return ret;
}
void ZukanUtil::BindLytWaitAnim(app::util::G2DUtil* pG2DUtil, const u32* waitAnimIndexArray, u32 waitAnimIndexArraySize, f32 frame)
{
  for(u32 i=0; i<waitAnimIndexArraySize; ++i)
  {
    if(waitAnimIndexArray[i] != gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR)
    {
      BindLytAnim(pG2DUtil, i, waitAnimIndexArray[i], frame);  // iはwk_?
    }
  }
}
void ZukanUtil::BindLytAnim(app::util::G2DUtil* pG2DUtil, u32 lytWkId, u32 animIndex, f32 frame)
{
  // SetAnimeFrameする前にBindAnimeしておかなければならない
  pG2DUtil->BindAnime( lytWkId, animIndex );  // wk_(lytWkId)
  pG2DUtil->SetAnimeFrame( lytWkId, animIndex, frame );  // wk_(lytWkId)
}

void ZukanUtil::SetButtonVisible(app::tool::ButtonManager* buttonManager, app::tool::ButtonManager::ButtonId buttonId, bool visible)
{
  if(visible)
  {
    buttonManager->SetButtonVisible(buttonId);
  }
  else
  {
    buttonManager->SetButtonInvisible(buttonId);
  }
  buttonManager->SetButtonInputEnable(buttonId, visible);
}
bool ZukanUtil::IsButtonVisible(const app::tool::ButtonManager* buttonManager, app::tool::ButtonManager::ButtonId buttonId)
{
  return buttonManager->IsButtonVisible(buttonId);  // IsButtonInputEnableは同じbool値が返ってくるはずなので見ない。
}


void ZukanUtil::SetPokeModelVisible(ZukanTool* tool, bool visible)
{
  if( tool->IsPokeModelVisible() )
  {
    if(!visible)
    {
      tool->SetPokeModelVisible(false);
    }
  }
  else
  {
    if(visible)
    {
      tool->SetPokeModelVisible(true);
    }
  }
}




void ZukanRotomVoice::Update(void)
{
  if(m_enable)
  {
    bool randomPlayVoiceFlag = false;
    if(m_count <= 0)
    {
      bool playVoiceFlag = false;
      if(m_reserveFirstVoiceFlag)
      {
        playVoiceFlag = true;
        m_reserveFirstVoiceFlag = false;
      }
      else if(m_count <= -(this->getIntervalCount()))  // 2倍の間隔は空かないようにしておく
      {
        playVoiceFlag = true;
        randomPlayVoiceFlag = true;
      }
      else if( System::GflUse::GetPublicRand((this->getIntervalCount())/2) == 0 )
      {
        playVoiceFlag = true;
        randomPlayVoiceFlag = true;
      }
     
      if(playVoiceFlag)
      {
        if(    ( randomPlayVoiceFlag && m_randomPlayEnable )
            || ( !randomPlayVoiceFlag )                      )
        {
          if( Sound::IsVoiceFinished( 0 ) )
          {
            Sound::PlayVoice( 0, MONSNO_ROTOMU, 0 ); 
            Sound::ChangeVoiceVolume( 0, ZUKAN_ROTOM_VOICE_VOLUME );
          }
        }
        m_count = this->getIntervalCount();
      }
    }
    --m_count;
  }
}
void ZukanRotomVoice::Start(bool playFirstVoiceFlag, s32 firstCount)
{
  m_enable = true;
  if(playFirstVoiceFlag)
  {
    if(firstCount <= 0)
    {
      if( Sound::IsVoiceFinished( 0 ) )
      {
        Sound::PlayVoice( 0, MONSNO_ROTOMU, 0 ); 
        Sound::ChangeVoiceVolume( 0, ZUKAN_ROTOM_VOICE_VOLUME );
      }
      m_reserveFirstVoiceFlag = false;
      m_count = this->getIntervalCount();
    }
    else
    {
      m_reserveFirstVoiceFlag = true;
      m_count = firstCount;
    }
  }
  else
  {
    m_reserveFirstVoiceFlag = false;
    m_count = 0;
  }
}
void ZukanRotomVoice::Stop(void)
{
  m_enable = false;
}
void ZukanRotomVoice::SetRandomPlayEnable(bool enable)
{
  m_randomPlayEnable = enable;
}
s32 ZukanRotomVoice::getIntervalCount(void) const
{
  static const s32 INTERVAL_COUNT = 150;  // [frame]
  return INTERVAL_COUNT;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

