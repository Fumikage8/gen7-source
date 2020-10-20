//==============================================================================
/**
 @file    ZukanPicScrollSheet.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.11.11
 @brief   図鑑の絵巻シート
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>
#include <Savedata/include/ZukanSave.h>

#include <PokeTool/include/PokeToolExtendData.h>

#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/alola_top.h>
#include <niji_conv_header/app/zukan/alola_top_pane.h>
#include <niji_conv_header/app/zukan/alola_top_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>

// 図鑑のインクルード
#include "ZukanType.h"
#include "ZukanContents.h"
#include "ZukanPokePicManager.h"
#include "ZukanPicScrollDataAccessor.h"
#include "ZukanPicScrollUtil.h"

#include "ZukanPicScrollSheet.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanPicScrollSheet
//
//==============================================================================

ZukanPicScrollSheet::ZukanPicScrollSheet(
  app::util::Heap*                    pAppHeap,
  app::util::G2DUtil*                 pG2DUtil,
  gfl2::lyt::LytMultiResID            lytRes,
  gfl2::lyt::LytWk*                   pLytWk,
  u32                                 lytWkId,
  const ZukanPicScrollDataAccessor*   pPicScrollDataAccessor,
  const PokeTool::ExtendData*         pPokeExtendData,
  gfl2::str::MsgData*                 pMsgData,
  print::WordSet*                     pWordSet,
  ZukanContents*                      pContents,
  ZukanPokePicManager*                pPokePicManager
)
  : m_pG2DUtil(pG2DUtil),
    m_lytRes(lytRes),
    m_pLytWk(pLytWk),
    m_lytWkId(lytWkId),
    m_pPicScrollDataAccessor(pPicScrollDataAccessor),
    m_pPokeExtendData(pPokeExtendData),
    m_pMsgData(pMsgData),
    m_pWordSet(pWordSet),
    m_pContents(pContents),
    m_pPokePicManager(pPokePicManager),
    m_placeGroupTopAlolaNo(0),
    m_state(STATE_WAIT_INIT),
    m_waitInFlag(false),
    m_waitOutFlag(false)
{
  m_sheetInfo = GFL_NEW(pAppHeap->GetDeviceHeap()) ZukanPicScrollUtil::SheetInfo();
  m_placeInfo = GFL_NEW(pAppHeap->GetDeviceHeap()) ZukanPicScrollUtil::PlaceInfo();
  m_sheetInfo->placeInfo = m_placeInfo;

  ZukanPicScrollUtil picScrollUtil(
    m_pG2DUtil,
    m_lytRes,
    m_pLytWk,
    m_lytWkId,
    m_pPicScrollDataAccessor,
    m_pPokeExtendData,
    m_pMsgData,
    m_pWordSet
  );
  
  picScrollUtil.SetupSheetInfo(m_sheetInfo);
  picScrollUtil.SetupPlaceInfo(m_sheetInfo->placeInfo, m_sheetInfo->placeTypeTopPane);

  // シートのトップペインの非表示
  m_sheetInfo->sheetTopPane->SetVisible(false);  // 値やテクスチャを設定する前に描画されないように非表示で初期化しておく
      // このシートを使用中かどうかはm_placeGroupTopAlolaNoで判断する。
}
  
ZukanPicScrollSheet::~ZukanPicScrollSheet()
{
  this->termSheet();

  GFL_SAFE_DELETE(m_placeInfo);
  GFL_SAFE_DELETE(m_sheetInfo);  
}

void ZukanPicScrollSheet::InitSheet(u32 placeGroupTopAlolaNo)
{
  // まず前の状態を終了させておく
  this->termSheet();

  // その後、今回の状態で初期化する
  m_placeGroupTopAlolaNo = placeGroupTopAlolaNo;

  if( !m_sheetInfo->sheetTopPane->IsVisible() )
  {
    // シートのトップペインの表示
    m_sheetInfo->sheetTopPane->SetVisible(true);
  }


  ZukanPicScrollUtil::PlaceInfo* placeInfo = NULL;
  u32 pokeNum = 0;

  {
    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);
 
    placeInfo = m_sheetInfo->placeInfo;
  
    pokeNum = ZukanPicScrollDataAccessor::PLACE_POS_NUM[ pokeData.placeType ];
  
    // placeTypeで使用するものだけ表示するようにする
    {
      for(u32 i=0; i<pokeNum; ++i)
      {
        if(placeInfo->spotInfo[i].pokeTopPane) placeInfo->spotInfo[i].pokeTopPane->SetVisible(true);
        if(placeInfo->spotInfo[i].ballTopPane) placeInfo->spotInfo[i].ballTopPane->SetVisible(true);
        if(placeInfo->spotInfo[i].nameTopPane) placeInfo->spotInfo[i].nameTopPane->SetVisible(true);
      }
  
      for(u32 i=pokeNum; i<ZukanPicScrollUtil::POKE_NUM_MAX; ++i)
      {
        if(placeInfo->spotInfo[i].pokeTopPane) placeInfo->spotInfo[i].pokeTopPane->SetVisible(false);
        if(placeInfo->spotInfo[i].ballTopPane) placeInfo->spotInfo[i].ballTopPane->SetVisible(false);
        if(placeInfo->spotInfo[i].nameTopPane) placeInfo->spotInfo[i].nameTopPane->SetVisible(false);
      }
    }
  }

  {
    for(u32 i=0; i<pokeNum; ++i)
    {
      u32 alolaNo = m_placeGroupTopAlolaNo + i;
     
      // ポケモンのデータ
      ZukanPicScrollDataAccessor::PokeData pokeData;
      m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);
     
      // このポケモンが入るスポット
      ZukanPicScrollUtil::SpotInfo* pSpotInfo = &(placeInfo->spotInfo[pokeData.placePos]);

      // 名前プレート待機アニメ
      m_pG2DUtil->StartAnime( m_lytWkId, pSpotInfo->namePlateWaitAnimId );
      // 捕獲前モンボマーク待機アニメ
      m_pG2DUtil->StartAnime( m_lytWkId, pSpotInfo->ballWaitAnimId[pokeData.gradeVal] );
      // ポケモン待機アニメ
      m_pG2DUtil->StartAnime( m_lytWkId, pSpotInfo->pokeWaitAnimId );

      // 名前プレート非表示
      pSpotInfo->nameTopPane->SetVisible(false);
      // 捕獲前モンボマーク表示
      pSpotInfo->ballTopPane->SetVisible(true);
      // ポケモン非表示 
      pSpotInfo->pokeTopPane->SetVisible(false);
    }
  }

  // 各スポットにポケモンを割り当てる
  this->setupPokeSpot(placeInfo, pokeNum);

  m_waitInFlag = false;
  m_waitOutFlag = false;
  m_state = STATE_WAIT_REQ_IN;
}

void ZukanPicScrollSheet::setupPokeSpot( ZukanPicScrollUtil::PlaceInfo* placeInfo, u32 pokeNum )
{
  //////////////////////////////////////////////
  // [今回のポケモンの処理]  位置や表示内容を設定する
  //////////////////////////////////////////////
  m_pG2DUtil->SetMessageData( m_pMsgData, m_pWordSet );  // <niji_conv_header/message/msg_zukan.h>
  
  for(u32 i=0; i<pokeNum; ++i)
  {
    u32 alolaNo = m_placeGroupTopAlolaNo + i;

    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);

    // このポケモンが入るスポット
    ZukanPicScrollUtil::SpotInfo* pSpotInfo = &(placeInfo->spotInfo[pokeData.placePos]);

    // コンテンツからポケモンデータを取得
    u32 indexOfAlolaNo = m_pContents->GetIndexOfDispPokeDataIndexForAlolaDefault(alolaNo);
    const ZukanContents::PokeData* contentsPokeData = m_pContents->GetDispPokeData(indexOfAlolaNo);
    u32 contentsFigureDataIndex = m_pContents->GetDispDrawFigureDataIndex(indexOfAlolaNo);
    PokeTool::SimpleParam sp = m_pContents->GetDispPokeSimpleParam(indexOfAlolaNo);

    // 位置
    ZukanPicScrollDataAccessor::TransData transData;
    m_pPicScrollDataAccessor->GetPokeTransData(alolaNo, sp.formNo, sp.sex, &transData);

    gfl2::math::VEC2 pokePos(transData.pokeX, transData.pokeY);
    gfl2::math::VEC2 ballPos(transData.ballX, transData.ballY);
    gfl2::math::VEC2 namePos(transData.nameX, transData.nameY);
    pSpotInfo->pokeTopPane->SetTranslate(pokePos);
    pSpotInfo->ballTopPane->SetTranslate(ballPos);
    pSpotInfo->nameTopPane->SetTranslate(namePos);

    // アローラ番号
    m_pG2DUtil->SetTextBoxPaneNumber(pSpotInfo->nameNoTextPane, msg_zkn_03_01, alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
    
    
    // ポケモン種族名
    
    // @fix NMCat[2143] 一部の伝説、準伝説のポケモン名プレートに光るエフェクトがつけられている  →アニメを付ける前に停止しておく
    for(s32 nameShineAnimIndex=0; nameShineAnimIndex<ZukanPicScrollDataAccessor::GRADE_VAL_NUM; ++nameShineAnimIndex)
    {
      m_pG2DUtil->StopAnime( m_lytWkId, pSpotInfo->nameShineAnimId[nameShineAnimIndex] );
    }

    if(contentsPokeData->figureSeeNum>0)
    {
      m_pG2DUtil->SetRegisterMonsNameNo(0, sp.monsNo);
      m_pG2DUtil->SetTextBoxPaneStringExpand(pSpotInfo->nameNameTextPane, msg_zkn_03_03);
     
      m_pG2DUtil->StartAnime( m_lytWkId, pSpotInfo->nameShineAnimId[pokeData.gradeVal] );
    }
    else
    {
      m_pG2DUtil->SetTextBoxPaneStringExpand(pSpotInfo->nameNameTextPane, msg_zkn_08_03);  // ??????を表示する

      m_pG2DUtil->StartAnime( m_lytWkId, pSpotInfo->nameShineAnimId[ZukanPicScrollDataAccessor::GRADE_VAL_NORMAL] );  // キラキラを出さないよう「キラキラ普通アニメ=キラキラなし」をあてる
    }

    // 名前プレート表示
    pSpotInfo->nameTopPane->SetVisible(contentsPokeData->figureSeeNum>0);

    // ポケモンのテクスチャを設定する
    {
      // 色
      nw::ut::Color8 vtxColor = ZukanType::POKE_GET_COLOR;
      if(contentsPokeData->getFlag == 0)
      {
        // 見ただけで捕まえていなかったら、色を暗くしておく
        vtxColor = ZukanType::POKE_SEE_COLOR;
      }
      for(s32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx)
      {
        // 残像３つ＋黒１つ
        pSpotInfo->pokePicPane[0]->SetVtxColor(vtxIdx, vtxColor);
        pSpotInfo->pokePicPane[1]->SetVtxColor(vtxIdx, vtxColor);
        pSpotInfo->pokePicPane[2]->SetVtxColor(vtxIdx, vtxColor);
        pSpotInfo->pokePicPane[3]->SetVtxColor(vtxIdx, vtxColor);
      }

      // テクスチャ
      // 残像３つ＋黒１つ

      // D要望対応：光ドラゴンのテクスチャを横長にしたい為、テクスチャサイズにペインも合わせる
      const bool isLarge = (sp.monsNo == MONSNO_PURIZUMU && sp.formNo == FORMNO_PURIZUMU_FORM_N) ? true : false;

      m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[0], contentsPokeData, contentsFigureDataIndex, isLarge);
      m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[1], contentsPokeData, contentsFigureDataIndex, isLarge);
      m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[2], contentsPokeData, contentsFigureDataIndex, isLarge);
      m_pPokePicManager->SetupPicPane(pSpotInfo->pokePicPane[3], contentsPokeData, contentsFigureDataIndex, isLarge);
    }
  }
 
  m_pG2DUtil->SetMessageData(NULL, NULL);
}

void ZukanPicScrollSheet::UnuseSheet(void)
{
  if( m_sheetInfo->sheetTopPane->IsVisible() )
  {
    // シートのトップペインの非表示
    m_sheetInfo->sheetTopPane->SetVisible(false);
  }
}

void ZukanPicScrollSheet::SetPosY(f32 posY)
{
  m_sheetInfo->sheetTopPane->SetTranslate(gfl2::math::VEC2(0,posY));
}

void ZukanPicScrollSheet::UpdateSheet(void)
{
  ZukanPicScrollDataAccessor::PokeData pokeData;
  //ZukanPicScrollUtil::PlaceInfo* placeInfo = NULL;
  
  if(m_state != STATE_WAIT_INIT)  // 初期化が済んでいなければならない
  {
    // ポケモンのデータ
    m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);
    // プレイスインフォ
    //placeInfo = m_sheetInfo->placeInfo;
  }


  switch(m_state)
  {
  case STATE_WAIT_INIT:
    {
      break;
    }
  case STATE_WAIT_REQ_IN:
    {
      if(m_waitInFlag)
      {
        m_state = STATE_WAIT_IN;
        this->startInAnim();
      }
      break;
    }
  case STATE_WAIT_IN:
    {
      this->startInAnim();
      break;
    }
  case STATE_IN:
    {
      m_state = STATE_LOOP;
      break;
    }
  case STATE_LOOP:
    {
      if(m_waitOutFlag)
      {
        this->startOutAnim();
      }
      break;
    }
  case STATE_OUT:
    {
      m_state = STATE_WAIT_REQ_IN;
      break;
    }
  }

  this->updatePokePicPane();
}

void ZukanPicScrollSheet::ReqInAnim(void)
{
  m_waitOutFlag = false;  // アウトアニメ待ちは解消しておく
  switch(m_state)
  {
  case STATE_WAIT_INIT:                                                                         break;  // 何もしない
  case STATE_WAIT_REQ_IN: m_waitInFlag = true;  m_state = STATE_WAIT_IN;  this->startInAnim();  break;  // インアニメを開始する
  case STATE_WAIT_IN:     m_waitInFlag = true;                            this->startInAnim();  break;  // インアニメを開始する
  case STATE_IN:                                                                                break;  // 既にインアニメ中なのでそのままそれを継続する
  case STATE_LOOP:                                                                              break;  // 既にインしているので何もしない
  case STATE_OUT:         m_waitInFlag = true;                                                  break;  // アウトアニメが終わってからインアニメが始まるようにフラグを立てておく
  }
}
void ZukanPicScrollSheet::ReqOutAnim(void)
{
  m_waitInFlag = false;  // インアニメ待ちは解消しておく
  switch(m_state)
  {
  case STATE_WAIT_INIT:                                                 break;  // 何もしない
  case STATE_WAIT_REQ_IN:                                               break;  // まだインアニメが始まっていないので何もしない
  case STATE_WAIT_IN:     m_state = STATE_WAIT_REQ_IN;                  break;  // まだインアニメが始まっていないので、インアニメのリクエスト待ちにする
  case STATE_IN:          m_waitOutFlag = true;                         break;  // インアニメが終わってからアウトアニメが始まるようにフラグを立てておく
  case STATE_LOOP:        m_waitOutFlag = true;  this->startOutAnim();  break;  // アウトアニメを開始する
  case STATE_OUT:                                                       break;  // 既にアウトアニメ中なのでそのままそれを継続する
  }
}

void ZukanPicScrollSheet::termSheet(void)
{
  if(m_placeGroupTopAlolaNo == 0) return;  // まだ未使用

  ZukanPicScrollUtil::PlaceInfo* placeInfo = NULL;
  u32 pokeNum = 0;

  // プレイスインフォの取得
  {
    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);
  
    // プレイスインフォ
    placeInfo = m_sheetInfo->placeInfo;
  
    pokeNum = ZukanPicScrollDataAccessor::PLACE_POS_NUM[ pokeData.placeType ];
  }

  // 各スポットに割り当てられたポケモンの読み込みが終わっていたら、ポケモンを表示する
  for(u32 i=0; i<pokeNum; ++i)
  {
    u32 alolaNo = m_placeGroupTopAlolaNo + i;
 
    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);
 
    // このポケモンが入るスポット
    ZukanPicScrollUtil::SpotInfo* pSpotInfo = &(placeInfo->spotInfo[pokeData.placePos]);
 
    // コンテンツからポケモンデータを取得
    u32 indexOfAlolaNo = m_pContents->GetIndexOfDispPokeDataIndexForAlolaDefault(alolaNo);
    const ZukanContents::PokeData* contentsPokeData = m_pContents->GetDispPokeData(indexOfAlolaNo);
    u32 contentsFigureDataIndex = m_pContents->GetDispDrawFigureDataIndex(indexOfAlolaNo);
    PokeTool::SimpleParam sp = m_pContents->GetDispPokeSimpleParam(indexOfAlolaNo);

    // ポケモンのテクスチャを元に戻す
    // 残像３つ＋黒１つ
    m_pPokePicManager->UnsetupPicPane(pSpotInfo->pokePicPane[0], contentsPokeData, contentsFigureDataIndex);
    m_pPokePicManager->UnsetupPicPane(pSpotInfo->pokePicPane[1], contentsPokeData, contentsFigureDataIndex);
    m_pPokePicManager->UnsetupPicPane(pSpotInfo->pokePicPane[2], contentsPokeData, contentsFigureDataIndex);
    m_pPokePicManager->UnsetupPicPane(pSpotInfo->pokePicPane[3], contentsPokeData, contentsFigureDataIndex);
  }
}

void ZukanPicScrollSheet::startInAnim(void)
{
  // この関数が呼ばれたら・・・
  //   ・ポケモンの読み込みが終わっていないなら、何もしないので次のフレームでまた呼んで下さい。
  //   ・ポケモンの読み込みが終わっているなら、直ちにアウトアニメが始まる。
  
  //ZukanPicScrollUtil::PlaceInfo* placeInfo = NULL;
  u32 pokeNum = 0;

  // プレイスインフォの取得
  {
    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);
 
    // プレイスインフォ
    //placeInfo = m_sheetInfo->placeInfo;
 
    pokeNum = ZukanPicScrollDataAccessor::PLACE_POS_NUM[ pokeData.placeType ];
  }

  // 各スポットに割り当てられたポケモンの読み込みが終わっているか？
  bool isReady = true;
  for(u32 i=0; i<pokeNum; ++i)
  {
    u32 alolaNo = m_placeGroupTopAlolaNo + i;

    // ポケモンのデータ
    ZukanPicScrollDataAccessor::PokeData pokeData;
    m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);

    // このポケモンが入るスポット
    //ZukanPicScrollUtil::SpotInfo* pSpotInfo = &(placeInfo->spotInfo[pokeData.placePos]);

    // コンテンツからポケモンデータを取得
    u32 indexOfAlolaNo = m_pContents->GetIndexOfDispPokeDataIndexForAlolaDefault(alolaNo);
    const ZukanContents::PokeData* contentsPokeData = m_pContents->GetDispPokeData(indexOfAlolaNo);
    u32 contentsFigureDataIndex = m_pContents->GetDispDrawFigureDataIndex(indexOfAlolaNo);
    PokeTool::SimpleParam sp = m_pContents->GetDispPokeSimpleParam(indexOfAlolaNo);

    if(!m_pPokePicManager->IsPokePicReady(contentsPokeData, contentsFigureDataIndex))
    {
      isReady = false;
      break;
    }
  }

  // ポケモンの読み込みが終わっているなら、プレイスインフォのインを開始する
  if(isReady)
  { 
    m_waitInFlag = false;
    m_state = STATE_IN;
  }
}
void ZukanPicScrollSheet::startOutAnim(void)
{
  // この関数が呼ばれたら直ちにアウトアニメが始まる

  // ポケモンのデータ
  ZukanPicScrollDataAccessor::PokeData pokeData;
  m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);

  // プレイスインフォのアウトを開始する
  //ZukanPicScrollUtil::PlaceInfo* placeInfo = m_sheetInfo->placeInfo;
  
  m_waitOutFlag = false;
  m_state = STATE_OUT;
}

void ZukanPicScrollSheet::updatePokePicPane(void)
{
  if(m_state != STATE_WAIT_INIT)
  {
    ZukanPicScrollUtil::PlaceInfo* placeInfo = NULL;
    u32 pokeNum = 0;

    // プレイスインフォの取得
    {
      // ポケモンのデータ
      ZukanPicScrollDataAccessor::PokeData pokeData;
      m_pPicScrollDataAccessor->GetPokeData(m_placeGroupTopAlolaNo, &pokeData);
  
      // プレイスインフォ
      placeInfo = m_sheetInfo->placeInfo;
  
      pokeNum = ZukanPicScrollDataAccessor::PLACE_POS_NUM[ pokeData.placeType ];
    }

    // 各スポットに割り当てられたポケモンの読み込みが終わっていたら、ポケモンを表示する
    for(u32 i=0; i<pokeNum; ++i)
    {
      u32 alolaNo = m_placeGroupTopAlolaNo + i;
 
      // ポケモンのデータ
      ZukanPicScrollDataAccessor::PokeData pokeData;
      m_pPicScrollDataAccessor->GetPokeData(alolaNo, &pokeData);
 
      // このポケモンが入るスポット
      ZukanPicScrollUtil::SpotInfo* pSpotInfo = &(placeInfo->spotInfo[pokeData.placePos]);
 
      // コンテンツからポケモンデータを取得
      u32 indexOfAlolaNo = m_pContents->GetIndexOfDispPokeDataIndexForAlolaDefault(alolaNo);
      const ZukanContents::PokeData* contentsPokeData = m_pContents->GetDispPokeData(indexOfAlolaNo);
      u32 contentsFigureDataIndex = m_pContents->GetDispDrawFigureDataIndex(indexOfAlolaNo);
      PokeTool::SimpleParam sp = m_pContents->GetDispPokeSimpleParam(indexOfAlolaNo);

      if(m_pPokePicManager->IsPokePicReady(contentsPokeData, contentsFigureDataIndex))
      {
        if(contentsPokeData->figureSeeNum>0)
        {
          if( !pSpotInfo->pokeTopPane->IsVisible() )
          {
            // 捕獲前モンボマーク非表示
            pSpotInfo->ballTopPane->SetVisible(false);
            // ポケモン表示
            pSpotInfo->pokeTopPane->SetVisible(true);
          }
        }
      }
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

