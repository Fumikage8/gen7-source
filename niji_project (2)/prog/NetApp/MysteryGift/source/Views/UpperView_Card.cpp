

//==============================================================================
/**
  *  @file   UpperView_Card.cpp
  *  @brief  ふしぎなおくりもの カード閲覧 上画面描画
  *  @date   2015.05.18 Mon.
  *  @author Yu Muto
  */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <System/include/GflUse.h>
#include  <model/include/gfl2_BaseCamera.h>
#include  "AppLib/include/Util/app_util_GetPaneHelper.h"
#include  "../ApplicationSystem/GiftWork.h"

#include  "../GiftResource.h"
#include  "./UpperView_Card.h"



// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  {
    LAYOUTWORK_Card,

    LAYOUTWORK_MAX
  };




  /*
    カードの傾きを[0,360)で返す
  */
  inline u32  GetRandomCardRotation(void)
  {
    const u32 rotationRange = 2;    /*  カードの傾き幅（±）  */

    return (System::GflUse::GetPublicRand(rotationRange*2) + (360-rotationRange)) % 360;
  }

  /*
    カードの表書き
  */
  u32 GetCardDescriptionMessageID(const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* pGiftData)
  {
    STATIC_ASSERT(!(msg_mystery_card_max%2));         /**<  偶数個であること  */
    const u32 messageIDs  = msg_mystery_card_max/2;   /**<  メッセージ種別数：メッセージデータが、前半に受け取り前/後半に受け取り後の形で納められている  */
    const u32 messageID   = (pGiftData->cardMessageID < messageIDs) ? pGiftData->cardMessageID : 0;   /*  範囲外ならダミーメッセージ  */
    
    /*  受け取り済みならoffsetを足して返す  */
    return  (messageID + (pGiftData->GetRecvFlag() ? messageIDs : 0));
  }

  static const app::util::AppRenderingManager::RenderPlaceType::Tag  renderPlace       = app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY;
} /*  namespace  */





namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     UpperView_Card
  *  @brief    ctor
  */
//------------------------------------------------------------------
UpperView_Card::UpperView_Card(GiftWork* pWork)
  : NetAppLib::UI::NetAppEmptyView(pWork->GetAppHeap())
  ,  m_pWork(pWork)
  , m_IsFrontFace(true)
  , m_pPokeModelUtil(false)
  , m_DataType(Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item)
  /*  effect  */
  , m_EffectUtil()
  , m_pDrawEnvNode(NULL)
  , m_pBaseCamera(NULL)
{
  gfl2::heap::HeapBase* pHeapBase = m_pWork->GetDeviceHeap();

  /*  setupLayoutSystem  */
  {


    NetAppLib::System::ResourceManager* pResourceManager  = m_pWork->GetResourceManager();
    void*                               pLayoutBuffer     = pResourceManager->GetResourceBuffer(RESOURCE_Layout);
    app::util::Heap*                    pHeap             = m_pWork->GetAppHeap();

    const app::util::G2DUtil::LytResData  lytResDataTable[] = {
      { pLayoutBuffer, 0, app::util::G2DUtil::ATTACH_NONE },
    };

    const app::util::G2DUtil::LytwkData   layoutWorkTable[LAYOUTWORK_MAX] =
    {
      {
        0,                                    //!< 使用するリソースの番号         : lytResDataTable
        LYTRES_GIFT_GIFT_CRD_UPP_000_BFLYT,   //!< レイアウトデータID ( *_BFLYT ) : niji_conv_header/app/gift/Gift.h
        LA_GIFT_GIFT_CRD_UPP_000___NUM,       //!< アニメ数                       : niji_conv_header/app/gift/Gift_anim_list.h
        pLayoutBuffer,                        //!< リソースデータ                 : pWork->ResourceManager->ResourceBuffer
        true,                                 //!< 非表示ペインに行列計算を行うか
        app::util::G2DUtil::SETUP_UPPER,      //!< ディスプレイ
        true                                  //!< 表示設定
      },
    };


    /*
      UIView
    */
    SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
    CreateGraphicsSystem( pHeap, false );

    Create2D(
      pHeap,                            /*  app::util::Heap*    */
      NULL,                             /*  gfl2::lyt::LytSys                 (NULLなら内部で新規に生成する。既存のものを使用する場合にはここで指定する)  */
      LAYOUTWORK_MAX,                   /*  u32                               layoutを扱う数（capacity）。内部ワークの数  */
      lytResDataTable,                  /*  app::util::G2DUtil::LytResData*   リソーステーブル        */
      GFL_NELEMS(lytResDataTable),      /*  u32                               リソーステーブル用素数  */
      layoutWorkTable,                  /*  app::util::G2DUtil::LytwkData*    ワークテーブル          */
      GFL_NELEMS(layoutWorkTable),      /*  u32                               ワークテーブル用素数    */
      m_pWork->GetMessageData(),        /*  gfl2::str::MsgData *messageData   */
      m_pWork->GetWordSet()             /*  print::WordSet *wordset,          */
    );

    {
      /*  textbox取得  */
      app::util::GetPaneHelper  helper(GetG2DUtil()->GetLayoutWork(0), &(GetG2DUtil()->GetLayoutResourceID(0)));

      /*  title  */
      m_pTextBoxes[TEXTBOX_Title]                 = helper.Clear().GetTextBoxPane(PANENAME_GIFT_CRD_UPP_000_PANE_TEXTBOX_00);

      /*  message  */
      helper.Clear().Push(PANENAME_GIFT_CRD_UPP_000_PANE_MESSAGE);
      m_pTextBoxes[TEXTBOX_Message_Message]       = helper.GetTextBoxPane(PANENAME_COMMON_MSG_UPP_001_PANE_TEXTBOX_00);

      /*  card  */
      helper.Clear().Push(PANENAME_GIFT_CRD_UPP_000_PANE_CARD);
      m_pTextBoxes[TEXTBOX_CardTitle]             = helper.GetTextBoxPane(PANENAME_GIFT_CRD_UPP_000_PANE_TEXTBOX_00);    /**<  カードタイトル  */
      m_pTextBoxes[TEXTBOX_CardDescription]       = helper.GetTextBoxPane(PANENAME_GIFT_PLT_UPP_000_PANE_TEXTBOX_01);    /**<  カード説明      */
      m_pTextBoxes[TEXTBOX_CardReceiveDateLabel]  = helper.GetTextBoxPane(PANENAME_GIFT_PLT_UPP_000_PANE_TEXTBOX_02);    /**<  受信日項目名    */
      m_pTextBoxes[TEXTBOX_CardReceiveDate]       = helper.GetTextBoxPane(PANENAME_GIFT_PLT_UPP_000_PANE_TEXTBOX_03);    /**<  受信日          */
      m_pTextBoxes[TEXTBOX_CardMessage]           = helper.GetTextBoxPane(PANENAME_GIFT_PLT_UPP_000_PANE_TEXTBOX_04);    /**<  裏面メッセージ  */

      /*  pane  */
      helper.Clear();

      m_pPanes[PANE_Message]      = helper.GetPane(PANENAME_GIFT_CRD_UPP_000_PANE_MESSAGE);
      m_pPanes[PANE_GiftBoxIcon]  = helper.Push(PANENAME_GIFT_CRD_UPP_000_PANE_CARD).GetPane(PANENAME_GIFT_PLT_UPP_000_PANE_GIFT_ICON_GIFTBOX);
      m_pPanes[PANE_Null_Card]    = helper.Clear().GetPane(PANENAME_GIFT_CRD_UPP_000_PANE_NULL_CARD);
      m_pPanes[PANE_PokeFrame]    = helper.Clear().Push(PANENAME_GIFT_CRD_UPP_000_PANE_CARD).GetPane(PANENAME_GIFT_PLT_UPP_000_PANE_POKE_FRAME);
    }
  }

  /*  pokemodelUtil  */
  {
    const NetAppLib::Util::NetAppPokeModelUtility::InFrameParam inFrameParam  = 
    {
      System::Camera::CModelViewerInFrame::SCREEN_UPPER,
      288, 96,  384,  192,    /*  pPokeFramePaneで上書きされる  */
      System::Camera::CModelViewerInFrame::TURN_DIRECTION_FREE,   /*  TURN_DIRECTION_LEFT, TURN_DIRECTION_RIGHT  */
      System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM,  /*  DRAW_POSITION_CENTER  */    /*  centerは例外的Modeとのこと  */
      app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY,
    };

    m_pPokeModelUtil  = GFL_NEW(pHeapBase)  NetAppLib::Util::NetAppPokeModelUtility(m_pWork);
    m_pPokeModelUtil->Setup(1, &inFrameParam);
  }


  /*  emitterset  */
  {
    NetAppLib::System::ResourceManager*                         pResourceManager  = m_pWork->GetResourceManager();
    app::util::EffectUtil::Description                          desc;
    const u32                                                   emitterSetResourceNum       = 2;
    u32                                                         effectGroupIDNumMaxArray[1] = { emitterSetResourceNum };

    desc.layerNum                 = 1;
    desc.effectResNumMax          = emitterSetResourceNum;
    desc.mode                     = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
    desc.effectGroupIDNumMaxArray = effectGroupIDNumMaxArray;

    m_EffectUtil.Create(pHeapBase, desc);

    m_EffectUtil.EffectSystemEntryResource(pResourceManager->GetResourceBuffer(RESOURCE_EmitterSet_1), 0);
    m_EffectUtil.GetData(0)->effectGroupIDList->PushFront(0);

    m_EffectUtil.EffectSystemEntryResource(pResourceManager->GetResourceBuffer(RESOURCE_EmitterSet_2), 1);
    m_EffectUtil.GetData(0)->effectGroupIDList->PushFront(1);

    m_EffectUtil.CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();

    m_pWork->GetAppRenderingManager()->SetEffectRenderPathConfig(renderPlace, m_EffectUtil.GetData(0)->effectRenderPathConfig);

    {
      gfl2::Effect::Handle* pHandle[2]  =
      {
        m_EffectUtil.GetEffectHandleForMode1Res1EmitterSet1Group(0),
        m_EffectUtil.GetEffectHandleForMode1Res1EmitterSet1Group(1),
      };


      if(pHandle[0] && pHandle[0]->IsValid())
      {
        pHandle[0]->GetEmitterSet()->SetPos(nw::math::VEC3(0.0f ,0.0f, 6.0f));
      }
      if(pHandle[1] && pHandle[1]->IsValid())
      {
        pHandle[1]->GetEmitterSet()->SetPos(nw::math::VEC3(0.0f ,0.0f, 8.0f));
      }

      ShowEffect(false);
    }

    /*  cam  */
    app::util::AppRenderingManager::CreateDefaultCamera(m_pWork->GetAppHeap(), &m_pDrawEnvNode, &m_pBaseCamera, renderPlace);
    m_pWork->GetAppRenderingManager()->SetEffectCamera(renderPlace, m_pBaseCamera);
    m_pBaseCamera->SetupCameraLookAt(
      gfl2::math::Vector3(0.0f, 0.0f, 0.0f),  /*  pos */
      gfl2::math::Vector3(0.0f, 0.0f, 1.0f),  /*  at  */
      gfl2::math::Vector3(0.0f, 1.0f, 0.0f)   /*  up  */
    );
    m_pBaseCamera->SetFovy(gfl2::math::ConvDegToRad(60));
    m_pBaseCamera->SetNear(1.0f);
    m_pBaseCamera->SetFar(2048.0f);
  }



  /*  表示設定  */
  SetText(TEXTBOX_Title, msg_mystery_cap_09);
  SetText(TEXTBOX_CardReceiveDateLabel, msg_mystery_cap_10);

  m_pPanes[PANE_Message]->SetVisible(false);
  m_pPanes[PANE_GiftBoxIcon]->SetVisible(false);
  GetG2DUtil()->Update2D();



  /*
    演出要望により、カードに角度をつける
    2016/03/23  要望により回転を削除
  */
//  GetG2DUtil()->SetRotate( LAYOUTWORK_Card, PANENAME_GIFT_CRD_UPP_000_PANE_CARD, gfl2::math::VEC3(0, 0, GetRandomCardRotation()) );  
}


//--------------------------------------------------------------------------------------------
/**
* @brief    デストラクタ
*/
//--------------------------------------------------------------------------------------------
UpperView_Card::~UpperView_Card()
{
  /*  eset/cam  */
  {
    m_pWork->GetAppRenderingManager()->UnsetEffectRenderPathConfig(renderPlace);
    m_pWork->GetAppRenderingManager()->UnsetEffectCamera(renderPlace);

    GFL_SAFE_DELETE(m_pDrawEnvNode);
    GFL_SAFE_DELETE(m_pBaseCamera);

    m_EffectUtil.KillAllEmitterSetForMode1Res1EmitterSet1Group();
    m_EffectUtil.EffectSystemClearResource(0);
    m_EffectUtil.EffectSystemClearResource(1);
    m_EffectUtil.Destroy();
  }

  Delete2D();
  DeleteGraphicsSystem();
}


//------------------------------------------------------------------
/**
  *  @brief    状態更新
  */
//------------------------------------------------------------------
void  UpperView_Card::Update(void)
{
  m_EffectUtil.UpdateForMode1Res1EmitterSet1Group();
  NetAppLib::UI::NetAppEmptyView::Update();   /*  super  */
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->Update();
    UpdatePokemodelVisibility();
  }
}


//--------------------------------------------------------------------------------------------
/**
* @brief  描画処理
*         子ウインドウについても自動的にコールする(親→子の順）
*/
//--------------------------------------------------------------------------------------------
void  UpperView_Card::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  GetG2DUtil()->AddDrawableLytWkForOneFrame(m_pWork->GetAppRenderingManager(), displayNo, LAYOUTWORK_Card);
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->SetFramePosition(m_pPanes[PANE_PokeFrame], true);
    m_pPokeModelUtil->Draw(displayNo);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
#if PM_DEBUG
void  UpperView_Card::DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->DebugDraw(displayNo);
  }
}
#endif


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::UpdateCardInfo(const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA* pGiftData)
{
  app::util::G2DUtil*                   pG2DUtil      = GetG2DUtil();
  NetAppLib::Message::MessageUtility&   rMessageUtil  = m_pWork->GetMessageUtility();
  print::WordSet*                       pWordSet      = m_pWork->GetWordSet();


  if(!pGiftData)
  {
    m_pPanes[PANE_Null_Card]->SetVisible(false);
    UpdatePokemodelVisibility();
    return;
  }
  m_pPanes[PANE_Null_Card]->SetVisible(true);


  /*  rarity  */
  {
    pG2DUtil->StopAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_00);
    pG2DUtil->StopAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_01);
    pG2DUtil->StopAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_02);
    switch(pGiftData->rarity)
    {
      case  2:  pG2DUtil->StartAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_02);   break;  /*  gold    */
      case  1:  pG2DUtil->StartAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_00);   break;  /*  silver  */
      default:  pG2DUtil->StartAnime(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_CARD_TYPE_01);   break;  /*  bronze  */
    }
    ShowEffect(pGiftData->rarity == 2);
  }

  const gfl2::str::STRCODE* pEventName = static_cast<const gfl2::str::STRCODE*>(pGiftData->eventName);

  //PGLのおくりものだったら名前をデータから取得
  if( pGiftData->eventID == Savedata::MysteryGiftSave::PGL_ITEM_EVENT_ID )
  {
    gfl2::str::StrBuf*  pStrBuf       = m_pWork->GetString(GARC_message_mystery_DAT, msg_mystery_cap_34);
    if( pStrBuf )
    {
      pEventName = pStrBuf->GetPtr();
    }
  }

  /*  title  */
  SetText(TEXTBOX_CardTitle,            pEventName);


  /*  desc  */
//  SetCardText(TEXTBOX_CardDescription,  pGiftData->cardMessageID);
  SetCardText(TEXTBOX_CardDescription,  GetCardDescriptionMessageID(pGiftData));

  /*  date  */
  SetCardReceiveDate(
    (pGiftData->recvDate / 10000),
    (pGiftData->recvDate % 10000) / 100,
    (pGiftData->recvDate %   100)
  );

    /*
    裏
  */

  /*  データ  */
  m_DataType  = static_cast<Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE>(pGiftData->dataType);
  switch(pGiftData->dataType)
  {
    case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke:        /**<  ポケモン  */
//    case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke_item:   ///<  ポケモン+アイテム
    {
      const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*  pPokeData = reinterpret_cast<const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*>(pGiftData->body);
      pml::pokepara::PokemonParam*                              pPokepara;
      u32                                                       messageID = msg_mystery_cap_17;

      /*  pokepara  */
      pPokepara = Savedata::MysteryGiftSave::CreatePokemonParamFromGiftData( pPokeData , m_pWork->GetDeviceHeap()->GetLowerHandle(), 1);

      if(pPokepara->IsEgg(pml::pokepara::CHECK_BOTH_EGG))
      {
        /*  タマゴ  */
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_CardMessage], msg_mystery_cap_26);
      }
      else
      {
        /*
          buf
           0: name
           1: level
           (2:  setitem)
        */
        /*  name  */
        if(pPokeData->nickName[0] == L'\0')
        {
          /*  種族  */
          pWordSet->RegisterPokeMonsNameNo(0, static_cast<MonsNo>(pPokeData->monsNo));
        }
        else
        {
          /*  ニックネーム  */
          gfl2::str::StrBuf  tmpStr(pPokeData->nickName, m_pWork->GetDeviceHeap()->GetLowerHandle());

          pWordSet->RegisterWord(0, tmpStr);
        }


        /*  level  */
        rMessageUtil.RegisterPokeLevel(1, pPokeData->level);

        /*
          MYSTERY_GIFT_TYPE_poke_itemをなくしてMYSTERY_GIFT_TYPE_pokeに統合されたので、setItemの値を見て分岐
        */
  //      if(pGiftData->dataType == Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke)
        if(pPokeData->setItem == ITEM_DUMMY_DATA)
        {
          /*  ポケモンのみ  */
          messageID = msg_mystery_cap_17;
        }
        else
        {
          /*  ポケモン+アイテム  */
          rMessageUtil.GetWordSet()->RegisterItemName(2, pPokeData->setItem, 1);
          messageID = msg_mystery_cap_28;
        }

        /*  expand  */
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_CardMessage], &(rMessageUtil.GetExpandedString(GIFT_MESSAGE_GRAC_ID, messageID)));
      }


      /*  pokemodel  */
      {
        PokeTool::SimpleParam         pokeSimpleParam;
        
        
        PokeTool::GetSimpleParam(&pokeSimpleParam, pPokepara);
        m_pPokeModelUtil->RequestEx(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, 0, pokeSimpleParam);
      }

      /*  boxicon  */
      m_pPanes[PANE_GiftBoxIcon]->SetVisible(false);

      GFL_SAFE_DELETE(pPokepara);
    }
      break;


    case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item:        ///<  アイテム
    {
      const Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*  pItemData = reinterpret_cast<const Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*>(pGiftData->body);
      const u32                     digits    = 3;
      const print::NumberDispType   dispType  = print::NUM_DISP_LEFT;
      const print::NumberCodeType   codeType  = print::NUM_CODE_HANKAKU;
      u32   items     = 0;
      u32   words     = 0;
      u32   messageID = 0;

      
      if(pItemData->itemNum1) {pWordSet->RegisterItemName(words++, pItemData->itemNo1, pItemData->itemNum1); pWordSet->RegisterNumber(words++, pItemData->itemNum1, digits, dispType, codeType); messageID = msg_mystery_cap_16; ++items;}
      if(pItemData->itemNum2) {pWordSet->RegisterItemName(words++, pItemData->itemNo2, pItemData->itemNum2); pWordSet->RegisterNumber(words++, pItemData->itemNum2, digits, dispType, codeType); messageID = msg_mystery_cap_19; ++items;}
      if(pItemData->itemNum3) {pWordSet->RegisterItemName(words++, pItemData->itemNo3, pItemData->itemNum3); pWordSet->RegisterNumber(words++, pItemData->itemNum3, digits, dispType, codeType); messageID = msg_mystery_cap_20; ++items;}
      if(pItemData->itemNum4) {pWordSet->RegisterItemName(words++, pItemData->itemNo4, pItemData->itemNum4); pWordSet->RegisterNumber(words++, pItemData->itemNum4, digits, dispType, codeType); messageID = msg_mystery_cap_21; ++items;}
      if(pItemData->itemNum5) {pWordSet->RegisterItemName(words++, pItemData->itemNo5, pItemData->itemNum5); pWordSet->RegisterNumber(words++, pItemData->itemNum5, digits, dispType, codeType); messageID = msg_mystery_cap_22; ++items;}
      if(pItemData->itemNum6) {pWordSet->RegisterItemName(words++, pItemData->itemNo6, pItemData->itemNum6); pWordSet->RegisterNumber(words++, pItemData->itemNum6, digits, dispType, codeType); messageID = msg_mystery_cap_23; ++items;}
      
      if(items)
      {
        pG2DUtil->SetTextBoxPaneString(m_pTextBoxes[TEXTBOX_CardMessage], &(rMessageUtil.GetExpandedString(GIFT_MESSAGE_GRAC_ID, messageID)));
      }

      /*  pokemodel  */
      m_pPokeModelUtil->ReleaseRequest(0);

      /*  boxicon  */
      m_pPanes[PANE_GiftBoxIcon]->SetVisible(true);
    }
      break;


    case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_bp:          ///<  BP
    {
      const u32                     numDigits = 6;
      const print::NumberDispType   dispType  = print::NUM_DISP_LEFT;
      const print::NumberCodeType   codeType  = print::NUM_CODE_HANKAKU;
      
      /*  BP  */
      pG2DUtil->SetRegisterNumber(0, pGiftData->GetBPData(), numDigits, dispType, codeType);
      pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_CardMessage], msg_mystery_cap_18);

      /*  pokemodel  */
      m_pPokeModelUtil->ReleaseRequest(0);

      /*  boxicon  */
      m_pPanes[PANE_GiftBoxIcon]->SetVisible(true);
    }
      break;


    case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_mame:        ///<  まめ
    {
      const u32                     numDigits = 3;
      const print::NumberDispType   dispType  = print::NUM_DISP_LEFT;
      const print::NumberCodeType   codeType  = print::NUM_CODE_HANKAKU;


      /*  豆  */
      pG2DUtil->SetRegisterNumber(0, pGiftData->GetMameData(), numDigits, dispType, codeType);
      pG2DUtil->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_CardMessage], msg_mystery_cap_35);

      /*  pokemodel  */
      m_pPokeModelUtil->ReleaseRequest(0);

      /*  boxicon  */
      m_pPanes[PANE_GiftBoxIcon]->SetVisible(true);
    }
      break;
  }


  UpdatePokemodelVisibility();
  pG2DUtil->Update2D();
}


//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  UpperView_Card::SetText(const TextBoxID textBoxID, const u32 messageID)
{
  GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[textBoxID], messageID);
}



//------------------------------------------------------------------
/**
  *  @func     SetText
  *  @brief    TextBoxのTextを設定
  */
//------------------------------------------------------------------
void  UpperView_Card::SetText(const TextBoxID textBoxID, const gfl2::str::STRCODE* pStrCode)
{
  GetG2DUtil()->SetTextBoxPaneString(m_pTextBoxes[textBoxID], pStrCode);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::SetCardText(const TextBoxID textBoxID, const u32 messageID)
{
  SetText(textBoxID,  m_pWork->GetString(GIFT_MESSAGE_CARD_GRAC_ID, messageID)->GetPtr());
}


//------------------------------------------------------------------
/**
  *  @func     SetCardReceiveDate
  *  @brief    Cardの受信日を設定
  */
//------------------------------------------------------------------
void  UpperView_Card::SetCardReceiveDate(const u16 year, const u8 month, const u8 day)
{
  RegisterDate(year, month, day);
  GetG2DUtil()->SetTextBoxPaneStringExpand(m_pTextBoxes[TEXTBOX_CardReceiveDate], msg_mystery_cap_11);
}


//------------------------------------------------------------------
/**
  *  @func     Flip
  *  @brief    カードを裏返す
  *  @param    toMessageFace  [in]  （表側から）説明側へ裏返すか ※falseで逆方向
  */
//------------------------------------------------------------------
void  UpperView_Card::Flip(const bool toMessageFace, const bool isImmediate)
{
  app::util::G2DUtil*   pG2DUtil  = GetG2DUtil();
  const u32             animID    = toMessageFace ? LA_GIFT_GIFT_CRD_UPP_000__CARD_UPSIDE_DOWN : LA_GIFT_GIFT_CRD_UPP_000__CARD_DOWNSIDE_UP;
  
  pG2DUtil->StartAnime(LAYOUTWORK_Card, animID);
  if(isImmediate)
  {
    const f32 frameMax  = pG2DUtil->GetAnimeMaxFrame(LAYOUTWORK_Card, animID);    /*  bindされていないと取得できない  */
    pG2DUtil->SetAnimeFrame(LAYOUTWORK_Card, animID, frameMax);                   /*  setFrameすると停止する          */
    pG2DUtil->PauseAnime(LAYOUTWORK_Card, animID, true);                          /*  第3引数は bool notPause, trueでPause解除  */
  }
  else
  {
    Sound::PlaySE(SEQ_SE_OPEN2);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::RegisterDate(const u32 year, const u32 month, const u32 day)
{
  const u32  digits = 2;

  if(!m_g2dUtil)  return;

  m_g2dUtil->SetRegisterNumber( 0, year%100,  digits, print::NUM_DISP_ZERO );
  m_g2dUtil->SetRegisterNumber( 1, month,     digits, print::NUM_DISP_ZERO );
  m_g2dUtil->SetRegisterNumber( 2, day,       digits, print::NUM_DISP_ZERO );
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::ShowMessage(const MessageMode messageMode)
{
  m_pPanes[PANE_Message]->SetVisible(true);

  switch(messageMode)
  {
    case  MESSAGEMODE_Exit:                 /**<  終わる              */
      SetText(TEXTBOX_Message_Message, msg_mystery_cap_08);
      break;

    case  MESSAGEMODE_Swap:                 /**<  入れ替えるカードを選んで              */
      SetText(TEXTBOX_Message_Message, msg_mystery_win_31);
      break;

    /*  ※仕様から削除された  */
#if 0
    case  MESSAGEMODE_Information:          /**<  詳しく見る          */
      SetText(TEXTBOX_Message_Message, msg_mystery_win_28);   /**<  @attention: くわしくみる」がない  */
      break;

    case  MESSAGEMODE_Information_Exit:     /**<  詳しく見る/終わる   */
      SetText(TEXTBOX_Message_Message, msg_mystery_cap_26);
      break;
#endif
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::HideMessage(void)
{
  m_pPanes[PANE_Message]->SetVisible(false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  UpperView_Card::SetCardFace(const bool isFront, const bool isImmediate)
{
  if(m_IsFrontFace != isFront)
  {
    m_IsFrontFace = isFront;
    Flip(!isFront, isImmediate);
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  UpperView_Card::FinallizePokeModel(void)
{
  bool  isComplete  = false;

  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->CleanupRequest();
    m_pPokeModelUtil->Update();
    if(m_pPokeModelUtil->IsStable())
    {
      GFL_SAFE_DELETE(m_pPokeModelUtil);
    }
  }

  if(!m_pPokeModelUtil)
  {
    isComplete  = true;
  }

  return  isComplete;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::UpdatePokemodelVisibility(void)
{
  app::util::G2DUtil*   pG2DUtil                = GetG2DUtil();
  const bool            bFlipAnimPlaying        = ((!pG2DUtil->IsAnimeEnd(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_UPSIDE_DOWN)) || (!pG2DUtil->IsAnimeEnd(0, LA_GIFT_GIFT_CRD_UPP_000__CARD_DOWNSIDE_UP)));
  const bool            bDataTypeIsPoke         = (m_DataType == Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke);
  const bool            bCardVisibility         = m_pPanes[PANE_Null_Card]->IsVisible();
  const bool            bModelOrIconVisibility  = (bCardVisibility && bDataTypeIsPoke && m_IsFrontFace && (!bFlipAnimPlaying));

  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->SetVisibility_InFrame(0, bModelOrIconVisibility);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  UpperView_Card::HidePokeModel(void)
{
  if(m_pPokeModelUtil)
  {
    m_pPokeModelUtil->SetVisibility_InFrame(0, false);
    m_pPokeModelUtil->ReleaseRequest(0);
  }

  ShowEffect(false);
}


void  UpperView_Card::ShowEffect(const bool bShow)
{
  gfl2::Effect::Handle* pHandle;

  for(u32 index = 0; index < 2; ++index)
  {
    pHandle = m_EffectUtil.GetEffectHandleForMode1Res1EmitterSet1Group(index);
    if(pHandle && pHandle->IsValid())
    {
      pHandle->GetEmitterSet()->SetStopDraw(!bShow);
    }
  }
}


} /*  namespace Gift    */
} /*  namespace NetApp  */

