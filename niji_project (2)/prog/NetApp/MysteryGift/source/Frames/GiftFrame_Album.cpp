//==============================================================================
/**
 * @file   GiftFrame_Album.cpp
 * @date   2015/08/03 Mon. 20:05:01
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Fade/include/gfl2_FadeManager.h>
#include  "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include  "../ApplicationSystem/GiftWork.h"
#include  "../GiftResource.h"

#include  "./GiftFrame_Album.h"

// =============================================================================
/**/
//==============================================================================
namespace {
  enum  {
    STATE_Entry = 0,
    STATE_CardSelect,


    /*  delete  */
    STATE_DeleteConfirm,
    STATE_DeleteStart,
    STATE_Delete,

    /*  swap  */
    STATE_SwapAStart,
    STATE_SwapA,
    STATE_SwapBStart,
    STATE_SwapB,

    STATE_CardDeleteOnCleanup,

    
    /*    */
    STATE_Deleted,
    STATE_NoCard,

    /*  save  */
    STATE_SaveStart,
    STATE_Saving,


    STATE_Exit,

    STATE_     /*  terminator  */
  };
} /*  namespace  */


namespace NetApp  {
namespace Gift {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     Frame_Album
  *  @brief    ctor
  */
//------------------------------------------------------------------
Frame_Album::Frame_Album(GiftWork* pWork)
  : NetAppLib::System::NetAppFrameBase(pWork)
  , m_pWork(pWork)
  , m_pViewContainer(pWork->GetViewContainer())
  , m_State(STATE_Entry)
  , m_Result(RESULT_INVALID)
  , m_bFrontFace(true)
  , m_IsDirty(false)
  , m_pPokepara(NULL)
  , m_pUpperView_Card(NULL)
  , m_pLowerView_Album(NULL)
  , m_SaveUtility()

{
  m_pUpperView_Card   = m_pViewContainer->m_pUpperView_Card;
  m_pLowerView_Album  = m_pViewContainer->m_pLowerView_Album;;
  
}


//------------------------------------------------------------------
/**
  *  @func     Frame_Album
  *  @brief    dtor
  */
//------------------------------------------------------------------
Frame_Album::~Frame_Album()
{
  GFL_SAFE_DELETE(m_pPokepara);
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
bool  Frame_Album::startup()
{
  m_pViewContainer->SetScene(GiftViewContainer::SCENE_Album);

  m_pUpperView_Card->SetCardFace(m_bFrontFace, true);

  /*  viewのresetにはdataProvが必要  */
  m_pLowerView_Album->SetDataProvider(this);
  m_pLowerView_Album->Reset();
  m_pLowerView_Album->SetDeleteMode(m_pWork->m_bCleanupMode);
  RefreshViews(true, true);
  return true;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Frame_Album::setListener()
{
  m_pLowerView_Album->SetEventHandler(this);
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Frame_Album::removeListener()
{
  m_pUpperView_Card->HidePokeModel();

  m_pLowerView_Album->SetEventHandler(NULL);
  m_pLowerView_Album->SetDataProvider(NULL);
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Frame_Album::updateSequence()
{
  switch(m_State)
  {
    case  STATE_Entry:
      m_pViewContainer->SetAllInputEnable(true);
      if(m_pWork->m_bCleanupMode)
      {
        m_pLowerView_Album->ShowMessage_Cleanup();
      }
      m_State = STATE_CardSelect;
      /*  fallthrough  */


    case  STATE_CardSelect:
      break;


    /*
      delete
    */
    case  STATE_DeleteStart:
    {
      m_pLowerView_Album->StartCardDeleteAnim(m_pLowerView_Album->GetSelectedIndex());
      m_State = STATE_Delete;
    }
      break;

    case  STATE_Delete:
      break;


    case STATE_CardDeleteOnCleanup:
      break;


    /*
      swap
    */
    case STATE_SwapAStart:
      m_pLowerView_Album->StartCardSwapAnimA(m_pLowerView_Album->GetPushedIndex(), m_pLowerView_Album->GetSelectedIndex());
      m_State = STATE_SwapA;
      break;

    case STATE_SwapA:
      break;

    case STATE_SwapBStart:
      m_pLowerView_Album->StartCardSwapAnimB(m_pLowerView_Album->GetPushedIndex(), m_pLowerView_Album->GetSelectedIndex());
      m_State = STATE_SwapB;
      break;

    case STATE_SwapB:
      break;


    case  STATE_NoCard:
      break;


    /*
      save
    */
    case STATE_SaveStart:
    {
      m_SaveUtility.SaveStart(m_pWork);
      m_State = STATE_Saving;
    }
      break;

    case STATE_Saving:
    {
      if(m_SaveUtility.SaveMain())
      {
        Sound::PlaySE(SEQ_SE_SAVE);
        m_pLowerView_Album->HideMessage();
        
        if(m_pWork->m_bCleanupMode)
        {
          m_pWork->m_pAppParam->procResult  = APP_PARAM::PROCRESULT_ReceiveDemo;
          ExitFrame(RESULT_CARDALBUM_CleanupDone);    /*  selectdataへ  */
        }
        else
        {
          ExitFrame();  /*  通常終了  */
        }
      }
    }
      break;


    case  STATE_Exit:
      /*  終了処理へ  */
      
      /*
        workにFrameResultを設定することでSwitcherが呼ばれる
      */
      m_pWork->SetFrameResult( m_Result );
      break;

    default:
      break;
  }
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
bool  Frame_Album::cleanup()
{
  m_pWork->m_bCleanupMode = false;
  return true;
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Album::LowerView_Album_OnEvent(LowerView_Album* pSender, const LowerView_Album::IEventHandler::EventCode eventCode)
{
  switch(eventCode)
  {
    case  LowerView_Album::IEventHandler::EVENT_MessageFinish:
    {
      if(m_State  == STATE_CardDeleteOnCleanup)
      {
         /*  選択中の受信データをセーブデータに反映  */
//        GetMysteryGiftSave()->PushBackGiftData(m_pWork->m_pAppParam->GetSelectedDeliveryData());
//        m_pWork->m_pAppParam->RemoveSelectedDeliveryData();      /*  ここで受け取りデータをリストから削除  */
        m_pLowerView_Album->ShowMessage_Saving();
        m_State = STATE_SaveStart;
      }
      else if(m_State  == STATE_Deleted)
      {
        /*  削除後、カードがなくなった  */
        if(!(GetMysteryGiftSave()->GetGiftDataNum()))
        {
          m_pLowerView_Album->ShowMessage_NoCard();
          m_State = STATE_NoCard;
        }
        else
        {
          m_State = STATE_CardSelect;
        }
      }
      else if(m_State  == STATE_NoCard)
      {
        /*  saveして終了  */
        m_pLowerView_Album->ShowMessage_Saving();
        m_State = STATE_SaveStart;
      }
    }
      break;


    case  LowerView_Album::IEventHandler::EVENT_MoveCursor:
    {
      RefreshViews(true, false);
    }
    break;
    
    case  LowerView_Album::IEventHandler::EVENT_SelectedItem:
      break;

    case  LowerView_Album::IEventHandler::EVENT_FlipCard:
    {
      m_bFrontFace  = !m_bFrontFace;
      m_pUpperView_Card->SetCardFace(m_bFrontFace);
    }
      break;

    case  LowerView_Album::IEventHandler::EVENT_SwapCard:       /**<  入れ替えの実行             */
    {
      m_pLowerView_Album->SetInputEnabled(false);
      m_State = STATE_SwapAStart;


#if 0
      const u32 index[2]  = {m_pLowerView_Album->GetPushedIndex(), m_pLowerView_Album->GetSelectedIndex()};
      
      GetMysteryGiftSave()->SwapGiftData(index[0], index[1]);
      m_IsDirty = true;
#endif
    }
      break;


    case  LowerView_Album::IEventHandler::EVENT_DeleteCard:     /**<  カード削除                */
    {
      m_pLowerView_Album->SetInputEnabled(false);
      m_State = STATE_DeleteStart;
    }
      break;


    case  LowerView_Album::IEventHandler::EVENT_CleanupCancel:
    {
      /*  アルバムがいっぱいなのに、カードを削除しなかった  */
//      ExitFrame();
      ExitFrame(RESULT_CARDALBUM_CleanupDone);    /*  selectdataへ  */
    }
      break;
    


    case  LowerView_Album::IEventHandler::EVENT_OnSwapAnimAEnd:
    {
      const u32 index[2]  = {m_pLowerView_Album->GetPushedIndex(), m_pLowerView_Album->GetSelectedIndex()};
      
      GetMysteryGiftSave()->SwapGiftData(index[0], index[1]);   /*  swap  */
      RefreshViews(true, true, true);
      m_IsDirty = true;
      m_State   = STATE_SwapBStart;
    }
      break;

    case  LowerView_Album::IEventHandler::EVENT_OnSwapAnimBEnd:
      m_pLowerView_Album->SetInputEnabled(true);
      m_State   = STATE_CardSelect;
      break;

    case  LowerView_Album::IEventHandler::EVENT_OnDeleteAnimEnd:

    {
      const u32 selectedIndex = m_pLowerView_Album->GetSelectedIndex();

      /*  実際に削除  */
      GetMysteryGiftSave()->DeleteGiftData(selectedIndex, true);
      m_IsDirty = true;
      RefreshViews(true, true);

      m_pLowerView_Album->SetInputEnabled(true);
      m_pLowerView_Album->ShowMessage_CardDeleted(m_pWork->m_bCleanupMode); /*  さくじょしました  */

      if(m_pWork->m_bCleanupMode)
      {
        m_State = STATE_CardDeleteOnCleanup;
      }
      else
      {
        m_State = STATE_Deleted;
      }
    }
      break;



    case  LowerView_Album::IEventHandler::EVENT_OnPushCard:
      m_pUpperView_Card->ShowMessage(UpperView_Card::MESSAGEMODE_Swap);
      break;

    case  LowerView_Album::IEventHandler::EVENT_OnPopCard:
      m_pUpperView_Card->HideMessage();
      break;

    case  LowerView_Album::IEventHandler::EVENT_Exit:
      OnExit();
      break;
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
void  Frame_Album::OnExit(void)
{
  if(m_pWork->m_bCleanupMode && (!GetMysteryGiftSave()->GetGiftDataVacancies()))
  {
    /*  削除モード且つ空きがない場合  */
    m_pLowerView_Album->ShowMessage_CleanupCancelConfirm();
  }
  else
  {
    if(m_IsDirty)
    {
      m_pLowerView_Album->ShowMessage_Saving();
      m_State = STATE_SaveStart;
    }
    else
    {
      ExitFrame();
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
u32   Frame_Album::AlbumDataProvider_DataNum(void) const
{
  return  GetMysteryGiftSave()->GetGiftDataNum();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_Album::AlbumDataProvider_GetCardInfo(CardInfo& rInfo, const u32 listIndex)
{
  bool                                                  result    = false;
  Savedata::MysteryGiftSave*                            pGiftSave = GetMysteryGiftSave();
  const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA*   pGiftData = pGiftSave->GetGiftData(listIndex);

  if(pGiftData)
  {
    rInfo.Clear();
    

    /*
      MYSTERY_GIFT_TYPE:  ポケモン / アイテム / BattlePoint / 豆 / ポケモン+アイテム
    */
    switch(pGiftSave->GetGiftDataType(listIndex))
    {
      case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke:       ///<  ポケモン
//      case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke_item:  ///<  ポケモン+アイテム
      {
        const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*  pPokeData = pGiftSave->GetPokeGiftData(listIndex);


        rInfo.giftType  = CardInfo::GIFTTYPE_Pokemon;
        GFL_SAFE_DELETE(m_pPokepara);
        m_pPokepara     = Savedata::MysteryGiftSave::CreatePokemonParamFromGiftData(pPokeData, m_pWork->GetDeviceHeap(), 1);
        rInfo.pPokeParaCoreParam  = m_pPokepara;
      }
        break;

      case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_item:       ///<  アイテム
      {
        const Savedata::MysteryGiftSave::MYSTERY_GIFT_ITEM_DATA*  pItemData = pGiftSave->GetItemGiftData(listIndex);
        
        rInfo.giftType  = CardInfo::GIFTTYPE_Item;
        rInfo.itemNo    = pItemData->itemNo1;
      }
        break;

      case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_bp:         ///<  BP
      case  Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_mame:       ///<  まめ
        rInfo.giftType  = CardInfo::GIFTTYPE_Others;
        break;
    }

    /*  date  */
    rInfo.year    = pGiftData->GetYear();
    rInfo.month   = pGiftData->GetMonth();
    rInfo.day     = pGiftData->GetDay();

    /*  flag  */
    rInfo.isReceived  = pGiftSave->GetRecvFlag(listIndex);
    
    /*  rarity  */
    switch(pGiftData->rarity)
    {
      case  2:  rInfo.rarity  = LowerView_Album::ICardAlbumDataProvider::CardInfo::RARITY_Gold;   break;
      case  1:  rInfo.rarity  = LowerView_Album::ICardAlbumDataProvider::CardInfo::RARITY_Silver; break;
      default:  rInfo.rarity  = LowerView_Album::ICardAlbumDataProvider::CardInfo::RARITY_Bronze; break;
    }
    
    result  = true;
  }

  
  return result;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool  Frame_Album::AlbumDataProvider_DeleteQuery(const u32 listIndex)
{
  return GetMysteryGiftSave()->DeleteGiftData(listIndex, false);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Album::ExitFrame(const FrameResult frameResult)
{
  m_Result  = frameResult;
  m_State = STATE_Exit;
  m_pViewContainer->SetAllInputEnable(false);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  Frame_Album::RefreshViews(const bool bRefreshUpper, const bool bRefreshLower, const bool isSkipLowerViewResetAnime)
{
  if(bRefreshUpper)
  {
    const u32   selectedIndex = m_pLowerView_Album->GetSelectedIndex();
    const bool  isValidData   = (selectedIndex < GetMysteryGiftSave()->GetGiftDataNum());
    const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA*   pGiftData = isValidData ? GetMysteryGiftSave()->GetGiftData(selectedIndex) : NULL;

    m_pUpperView_Card->UpdateCardInfo(pGiftData);
  }
  
  if(bRefreshLower)
  {
    m_pLowerView_Album->InvalidateView();
    m_pLowerView_Album->RefreshView(isSkipLowerViewResetAnime);
  }
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
