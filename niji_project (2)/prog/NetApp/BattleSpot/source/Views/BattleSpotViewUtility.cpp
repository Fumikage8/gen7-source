//==============================================================================
/**
 * @file   BattleSpotViewUtility.cpp
 * @date   2015/10/14 Wed. 18:54:14
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "GameSys/include/GameData.h"
#include  "AppLib/include/Util/app_util_2d.h"
#include  "AppLib/include/Tool/app_tool_PokeIcon.h"
#include  "Print/include/PrintSystem.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppBattleParty.h"

#include  "../System/BattleSpotWork.h"
#include  "../BattleSpotResource.h"
#include  "./BattleSpotViewUtility.h"

// =============================================================================
/**/
//==============================================================================
namespace
{

/*
../../../../Print/include/PrintSystem.h"

enum NumberDispType {
  NUM_DISP_LEFT,    ///< 左詰め
  NUM_DISP_SPACE,   ///< 右詰め（スペース埋め）
  NUM_DISP_ZERO,    ///< 右詰め（ゼロ埋め）
};

enum NumberCodeType {
  NUM_CODE_ZENKAKU,   ///< 全角
  NUM_CODE_HANKAKU,   ///< 半角
  // ローカライズ時
  NUM_CODE_DEFAULT = NUM_CODE_HANKAKU,
};

*/
  enum  NumberStyleID
  {
    NUMSTYLE_PlayerRate,      /**<  プレイヤー：レート  */
    NUMSTYLE_PlayerMatches,   /**<  プレイヤー：対戦数  */
    NUMSTYLE_PlayerWins,      /**<  プレイヤー：勝利数  */
    NUMSTYLE_PlayerLoses,     /**<  プレイヤー：敗北数  */
    
    
    NUMSTYLE_MAX
  };

  static const struct NumberStyleDefinition
  {
    u32                     digits;
    print::NumberDispType   numType;
    print::NumberCodeType   codeType;
  }numberStyleDefinition[NUMSTYLE_MAX]  =  {
    { 4,   print::NUM_DISP_SPACE,  print::NUM_CODE_HANKAKU},  /*    NUMSTYLE_PlayerRate  */
    { 7,   print::NUM_DISP_SPACE,  print::NUM_CODE_HANKAKU},  //    NUMSTYLE_PlayerMatches,   /**<  プレイヤー：対戦数  */
    { 7,   print::NUM_DISP_SPACE,  print::NUM_CODE_HANKAKU},  //    NUMSTYLE_PlayerWins,      /**<  プレイヤー：勝利数  */
    { 7,   print::NUM_DISP_SPACE,  print::NUM_CODE_HANKAKU},  //    NUMSTYLE_PlayerLoses,     /**<  プレイヤー：敗北数  */
  };


  void  RegisterNumberToG2DUtil(app::util::G2DUtil* pG2DUtil, u32 bufID, u32 value, const NumberStyleID styleID)
  {
    pG2DUtil->SetRegisterNumber(
      bufID,
      value,
      numberStyleDefinition[styleID].digits,
      numberStyleDefinition[styleID].numType,
      numberStyleDefinition[styleID].codeType
    );
  }
}



namespace NetApp      {
namespace BattleSpot  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ViewUtility_Card::ViewUtility_Card(BattleSpotWork* pWork)
  : m_pWork(pWork)
  , m_pG2DUtil(NULL)
  , m_pQREncoder(NULL)
  , m_QRSourceData(pWork->GetDeviceHeap(), QR_SOURCE_DATA_MAXBYTE)
  , m_QRTexture(pWork)
{
  gfl2::heap::HeapBase*   pHeapBase = pWork->GetDeviceHeap();

  m_pQREncoder  = gfl2::qr::IQREncoder::CreateQREncoder(pHeapBase, pHeapBase, 0);   /*  thread不使用  */
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ViewUtility_Card::~ViewUtility_Card()
{
  GFL_SAFE_DELETE(m_pQREncoder);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ViewUtility_Card::SetPlayerStatus(PlayerStatusPaneInfo* pPaneInfo, const bool bSkipIconLoad)
{
  if(!m_pG2DUtil) return;

  const RANK_DATA&  rRankData   = m_pWork->GetRankData();
  const u32                         val_Rate    = rRankData.rate;
  const u32                         val_Wins    = rRankData.win;
  const u32                         val_Loses   = rRankData.lose;
  u32                         val_Matches = val_Wins + val_Loses + rRankData.draw;

  if( m_pWork->GetRule() == BattleSpotWork::RULE_FRIEND_CUP )
  {//仲間
    if( val_Matches < m_pWork->m_pBattleSpotSave->GetFriendMatchCount() )
    {
      val_Matches = m_pWork->m_pBattleSpotSave->GetFriendMatchCount();
    }
  }
  else if( m_pWork->GetRule() == BattleSpotWork::RULE_NET_CUP )
  {//インターネット
    if( val_Matches < m_pWork->m_pBattleSpotSave->GetInternetMatchCount() )
    {
      val_Matches = m_pWork->m_pBattleSpotSave->GetInternetMatchCount();
    }
  }

  GFL_PRINT("val_Matches[%d]\n",val_Matches);

  /*  label  */
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_LabelRate],    msg_bs_sys_21, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_LabelMatches], msg_bs_sys_23, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_LabelWins],    msg_bs_sys_25, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_LabelLoses],   msg_bs_sys_27, false);

  /*  status  */
  RegisterNumberToG2DUtil(m_pG2DUtil, 0, val_Rate,    NUMSTYLE_PlayerRate);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_Rate],         msg_bs_sys_22, true);

  RegisterNumberToG2DUtil(m_pG2DUtil, 0, val_Matches, NUMSTYLE_PlayerMatches);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_Matches],      msg_bs_sys_24, true);

  RegisterNumberToG2DUtil(m_pG2DUtil, 0, val_Wins,    NUMSTYLE_PlayerWins);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_Wins],         msg_bs_sys_26, true);

  RegisterNumberToG2DUtil(m_pG2DUtil, 0, val_Loses,   NUMSTYLE_PlayerLoses);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_Loses],        msg_bs_sys_28, true);

  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_Information],  msg_bs_sys_19, false);


  //大会用 
  do 
  {
    /** 参加状態によって分岐  */
    u32 work = 0;
    if( m_pWork->GetRule() == BattleSpotWork::RULE_FRIEND_CUP )
    {//仲間
      work = m_pWork->m_pBattleSpotSave->GetFriendEntryWork();
    }
    else if( m_pWork->GetRule() == BattleSpotWork::RULE_NET_CUP )
    {//インターネット
      work =m_pWork->m_pBattleSpotSave->GetInternetEntryWork();
    }
    else
    {
      break;//大会でなければ抜ける
    }

    switch( work )
    {
    case ENTRY_REGULATION:
    case ENTRY_BATTLEBOX:
    case ENTRY_UPLOAD:
      SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_1LineMessage],  msg_bs_win_44, false);    /**<  参加中の大会です  */
      break;
    case ENTRY_END:
      SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_1LineMessage],  msg_bs_win_45, false);    /**<  前回の大会です    */
      break;
    case ENTRY_RETIRE:
      SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_1LineMessage],  msg_bs_win_46, false);    /**<  リタイアしました  */
      break;
    case ENTRY_CANCEL:
      SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_1LineMessage],  msg_bs_win_47, false);    /**<  参加できません    */
      break;
    default://このワークは想定外
      GFL_ASSERT(0);//@fix
      SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Player_1LineMessage],  msg_bs_win_47, false);    /**<  参加できません    */
      break;
    }  
  } while(0);

   
  if(!bSkipIconLoad)
  {
    const Savedata::MyStatus*   pMyStatus = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Player);
    
    if(pMyStatus)
    {
      //if( m_pWork->m_pTrainerIconUtility->IsEndIconObjectData(TRAINER_ICON_Player) == false )
      {//まだロードしていなければ
        app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

        iconObjectData.ConvFromMyStatusIconData( pMyStatus->GetIcon() );

        iconObjectData.pReplacePane             = pPaneInfo->pIconPicture;
        iconObjectData.pSyncVisiblePane         = pPaneInfo->pIconSyncVisiblePane;
        iconObjectData.pInLoadVisibleChangePane = pPaneInfo->pIconControlPane;
        iconObjectData.pTimerIconRootPane       = NULL;

        m_pWork->m_pTrainerIconUtility->SetIconObjectData( TRAINER_ICON_Player, iconObjectData );
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  ViewUtility_Card::SetOpponentStatus(OpponentStatusPaneInfo* pPaneInfo)
{
  if(!m_pG2DUtil) return;

  const Savedata::MyStatus*   pMyStatus_Oppo0 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent0);
  const Savedata::MyStatus*   pMyStatus_Oppo1 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent1);
  const Savedata::MyStatus*   pMyStatus_Oppo2 = m_pWork->GetTargetStatus(BattleSpotWork::REGIONTARGET_Opponent2);

  const u32                   val_Rate        = m_pWork->GetRankData().rate;
  gfl2::str::StrBuf*          pStrBuf_Name    = m_pG2DUtil->GetTempStrBuf(0);


  /*  label  */
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_LabelName0], msg_bs_sys_41, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_LabelName1], msg_bs_sys_42, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_LabelName2], msg_bs_sys_43, false);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_LabelRate],  msg_bs_sys_21, false);

  /*  status  */
  if(pMyStatus_Oppo0)
  {
    pMyStatus_Oppo0->GetNameString(pStrBuf_Name);
    m_pG2DUtil->SetRegisterWord(0, *pStrBuf_Name);
    SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Name0], msg_bs_sys_29, true);
  }

  if(pMyStatus_Oppo1)
  {
    pMyStatus_Oppo1->GetNameString(pStrBuf_Name);
    m_pG2DUtil->SetRegisterWord(0, *pStrBuf_Name);
    SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Name1], msg_bs_sys_29, true);
  }

  if(pMyStatus_Oppo2)
  {
    pMyStatus_Oppo2->GetNameString(pStrBuf_Name);
    m_pG2DUtil->SetRegisterWord(0, *pStrBuf_Name);
    SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Name2], msg_bs_sys_29, true);
  }


  RegisterNumberToG2DUtil(m_pG2DUtil, 0, val_Rate,    NUMSTYLE_PlayerRate);
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Rate],            msg_bs_sys_22, true);

  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Country],         *(m_pWork->GetCountry(BattleSpotWork::REGIONTARGET_Opponent0)));
  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Region],          *(m_pWork->GetRegion(BattleSpotWork::REGIONTARGET_Opponent0)));

//  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_PrevTournament],  );
//  SetTextBoxString(pPaneInfo->pTextBoxes[TEXTBOX_Opponent_Rank],            );

  if(pPaneInfo->pIconPicture[0])
  {
    if(pMyStatus_Oppo0)
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( pMyStatus_Oppo0->GetIcon() );

      iconObjectData.pReplacePane             = pPaneInfo->pIconPicture[0];
      iconObjectData.pSyncVisiblePane         = NULL;
      iconObjectData.pInLoadVisibleChangePane = pPaneInfo->pIconControlPane[0];
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtility->SetIconObjectData( TRAINER_ICON_Opponent_0, iconObjectData, false );
    }
  }
  if(pPaneInfo->pIconPicture[1])
  {
    if(pMyStatus_Oppo1)
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( pMyStatus_Oppo1->GetIcon() );

      iconObjectData.pReplacePane             = pPaneInfo->pIconPicture[1];
      iconObjectData.pSyncVisiblePane         = NULL;
      iconObjectData.pInLoadVisibleChangePane = pPaneInfo->pIconControlPane[1];
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtility->SetIconObjectData( TRAINER_ICON_Opponent_1, iconObjectData, false );
    }
  }
  if(pPaneInfo->pIconPicture[2])
  {
    if(pMyStatus_Oppo2)
    {
      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( pMyStatus_Oppo2->GetIcon() );

      iconObjectData.pReplacePane             = pPaneInfo->pIconPicture[2];
      iconObjectData.pSyncVisiblePane         = NULL;
      iconObjectData.pInLoadVisibleChangePane = pPaneInfo->pIconControlPane[2];
      iconObjectData.pTimerIconRootPane       = NULL;

      m_pWork->m_pTrainerIconUtility->SetIconObjectData( TRAINER_ICON_Opponent_2, iconObjectData, false );
    }
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
void  ViewUtility_Card::SetTeamStatus(const TeamStatusPaneInfo& rPaneInfo)
{
  const u32                             selectedTeamIndex = m_pWork->m_pAppParam->m_pTeamSelectEventParam->out.teamIdx;
  gfl2::heap::HeapBase*                 pHeapBase         = m_pWork->GetDeviceHeap()->GetLowerHandle();
  NetAppLib::Util::NetAppBattleParty    battleParty(pHeapBase);
  GameSys::GameData*                    pGameData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();


  battleParty.CreateBattleParty(pGameData->GetBOX(), pGameData->GetBoxPokemon(), selectedTeamIndex, pHeapBase);

  m_pG2DUtil->SetRegisterWord(0, *(battleParty.GetTeamName()));
  SetTextBoxString(rPaneInfo.pTextBox_TeamName,  msg_bs_sys_22, true);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ViewUtility_Card::SetQR(QRPaneInfo* pQRPaneInfo)
{
  if(!m_QRTexture.IsBuilt())
  {
#if PM_DEBUG
    static const char  qrTestData[]  = "https://dev2-3ds.pokemon-gl.com/competitions/%d/regulation/";//開発環境
#else
    static const char  qrTestData[]  = "https://3ds.pokemon-gl.com/competitions/%d/regulation/";//本番環境
#endif

    char path[256];
    std::memset( path , 0 , sizeof(path) );
    u32 size = 0;
#if defined( GF_PLATFORM_CTR )
    size = nw::ut::snprintf( path, 256, 255, qrTestData, m_pWork->m_pAppParam->m_pRegulationDrawInfo->GetRegulation().GetValueParam(Regulation::URL_DATA) );
#endif
    /*
      NMCat[4041]   インターネット大会の「大会の情報」で表示されるQRコードが読み込めない
      3DSカメラにURLとして認識させるためには、SourceDataにNULを含めてはいけない
    */
    m_QRSourceData.CopyData( (const u8*)path, size);
    m_QRSourceData.SetVersion(0);
    m_QRTexture.Build(m_pQREncoder->Encode(&m_QRSourceData));

    m_QRTexture.AssignTextureToPicturePane(pQRPaneInfo->pQRPicture);
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
void  ViewUtility_Card::SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, const u32 messageID, const bool isExpand)
{
  if(!m_pG2DUtil || !pTarget || (messageID == UINT_MAX)) return;


  if(isExpand)  m_pG2DUtil->SetTextBoxPaneStringExpand(pTarget, messageID);
  else          m_pG2DUtil->SetTextBoxPaneString(pTarget, messageID);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ViewUtility_Card::SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, gfl2::str::StrBuf& rStr, const bool isExpand)
{
  if(!m_pG2DUtil || !pTarget) return;


  if(isExpand)  m_pG2DUtil->SetTextBoxPaneStringExpand(pTarget, rStr);
  else          m_pG2DUtil->SetTextBoxPaneString(pTarget, &rStr);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ViewUtility_Card::SetTextBoxString(gfl2::lyt::LytTextBox* pTarget, const gfl2::str::STRCODE& rStrCode)
{
  if(!m_pG2DUtil || !pTarget) return;


  m_pG2DUtil->SetTextBoxPaneString(pTarget, &rStrCode);
}







} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
