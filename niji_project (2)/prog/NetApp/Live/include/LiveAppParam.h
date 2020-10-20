
#if !defined(LIVEAPPPARAM_H_INCLUDED)
#define LIVEAPPPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveAppParam.h
 * @date   2015/07/09 Thu. 12:48:53
 * @author muto_yu
 * @brief  ライブ大会 process argment
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "NetStatic/NetAppLib/include/Live/LiveNet.h"
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameData.h"
#include  "Savedata/include/BoxSave.h"
#include  "Savedata/include/LiveMatchSave.h"
#include  "PokeTool/include/PokeToolBattleParty.h"
#include  "ExtSaveData/include/BattleRecorderSaveData.h"


namespace NetApp  {
namespace Live    {

enum  BootMode
{
  BOOTMODE_None,
  BOOTMODE_Top,                       /**<  トップメニューから（通常）            */
  BOOTMODE_CeckRegulation_FromQRRead, /**<  Regulation確認から（QRからの成功戻り）*/
  BOOTMODE_TeamSelect_Selected,       /**<  チーム選択：決定                      */
  BOOTMODE_TeamSelect_Canceled,       /**<  チーム選択：キャンセル                */
  BOOTMODE_BattleResult,              /**<  バトル終わり                          */
  BOOTMODE_MatchMenu,                 /**<  大会メニューから                      */

  BOOTMODE_
};



/**
  Proc結果
*/
enum  APP_RESULT
{
  APPRESULT_Initial,          /**<  初期値            */
  APPRESULT_Exit,             /**<  終了              */
  APPRESULT_Exit_Fatal,       /**<  終了：内部Fatalへ */

  APPRESULT_CallTeamSelect,   /**<  TeamSelect要求    */
  APPRESULT_CallBattle,       /**<  対戦要求          */
  APPRESULT_CallBattleReplay, /**<  バトル再生要求    */
  APPRESULT_CallQRReader,     /**<  QRReader要求      */

  APPRESULT_
};



// =============================================================================
/**/
//==============================================================================
struct  APP_PARAM {
  BootMode                              m_BootMode;
  LiveNet*                              m_pLiveNet;
  Savedata::LiveMatchData*              m_pLiveMatchData;       /**<  Liveセーブデータ  */
  ExtSavedata::BattleRecorderSaveData*  m_pBattleRecorderData;
  BATTLE_SETUP_PARAM*                   m_pBattleSetupParam;    /**<  リプレイ用SetupParam  */
  PokeTool::BattleParty*                m_pMyBattleParty;       /**<  登録したPartyにアクセスするためのUtility  */
  u16                                   m_SelectedPartyIndex;


  APP_RESULT                            m_Result;               /**<  [out] 結果    */

  /*  regulation作業領域  */
  Regulation*                           m_pSavedRegulation;     /**<  [in/out]  coreDataアクセス用インスタンス（savedata）  */
  Regulation*                           m_pReceivedRegulation;  /**<  [in]      Ir/QR経由で受信したRegulationData                     */


  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  APP_PARAM(void)
    : m_BootMode(BOOTMODE_None)
    , m_pLiveNet(NULL)
    , m_pLiveMatchData(NULL)
    , m_pBattleRecorderData(NULL)
    , m_pBattleSetupParam(NULL)
    , m_pMyBattleParty(NULL)
    , m_SelectedPartyIndex(0)
    , m_Result(APPRESULT_Initial)
    , m_pSavedRegulation(NULL)
    , m_pReceivedRegulation(NULL)
  {
  }


  /*
    work:0
    初期
  */
  void  SetLiveEntryWork_Initial(void)            {m_pLiveMatchData->SetLiveEntryWork_Initial();}

  /*
    work:1
    受信済み
  */
  void  SetLiveEntryWork_RegulationReceived(void)
  {
    m_pLiveMatchData->SetLiveEntryWork_RegulationReceived();
    /*  受信Regulationをセーブデータへ反映  */
    m_pReceivedRegulation->Serialize(m_pLiveMatchData->GetSerializedRegulationData());
    /*  現在のSaveDataからRegulationを構築  */
    m_pSavedRegulation->Deserialize(m_pLiveMatchData->GetSerializedRegulationData(), m_pLiveMatchData->GetSerializedRegulationDataSize());
  }

  /*
    work:2
    参加
  */
  void  SetLiveEntryWork_Participating(void)
  {
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX()->SetTeamLock(m_SelectedPartyIndex, true);
    m_pLiveMatchData->SetPartyIndex(m_SelectedPartyIndex);
    m_pLiveMatchData->ResetResult();
    m_pLiveMatchData->SetLiveEntryWork_Participating();
  }

  /*
    work:3
    完遂
  */
  void  SetLiveEntryWork_AllMatchOver(void)
  {
    /*  チームロック解除  */
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX()->SetTeamLock(m_pLiveMatchData->GetPartyIndex(), false);
    m_pLiveMatchData->InvalidatePartyIndex();
    m_pLiveMatchData->SetLiveEntryWork_AllMatchOver();
  }

  /*
    work:4
    参加解除
  */
  void  SetLiveEntryWork_NotParticipate(void)
  {
    if(m_pLiveMatchData->IsValidPartyIndex())
    {
      /*  チームロック解除  */
      GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX()->SetTeamLock(m_pLiveMatchData->GetPartyIndex(), false);
    }
    m_pLiveMatchData->InvalidatePartyIndex();
    m_pLiveMatchData->SetLiveEntryWork_NotParticipate();
  }

  //------------------------------------------------------------------
  /**
    *  @brief    「ライブ大会を始める」が有効か
    */
  //------------------------------------------------------------------
  bool  IsLiveMatchStartAvailable(void) const
        {
          const u8  liveEntryWork = m_pLiveMatchData->GetLiveEntryWork();

          return(false
            ||  (liveEntryWork == 0)
            ||  (liveEntryWork == 1)
            ||  (liveEntryWork == 2)
            ||  (liveEntryWork == 3)
            ||  (liveEntryWork == 4)
          );
        }

  //------------------------------------------------------------------
  /**
    *  @brief    「前回の成績を見る」が有効か
    */
  //------------------------------------------------------------------
  bool  IsShowLastLiveMatchInfoAvailable(void) const
        {
          const u8  liveEntryWork = m_pLiveMatchData->GetLiveEntryWork();
          
          return(false
            ||  (liveEntryWork == 3)
          );
        }


  //------------------------------------------------------------------
  /**
    *  @brief    選手証の受信が必要
    */
  //------------------------------------------------------------------
  bool  IsNeedReceiveNewRegulation(void) const
        {
          const u8  liveEntryWork = m_pLiveMatchData->GetLiveEntryWork();
          
          return(false
            ||  (liveEntryWork == 0)
            ||  (liveEntryWork == 3)
            ||  (liveEntryWork == 4)
          );
        };

  //------------------------------------------------------------------
  /**
    *  @brief    バトルチームの登録が必要か
    */
  //------------------------------------------------------------------
  bool  IsNeedBattleTeamRegistration(void) const
        {
          const u8  liveEntryWork = m_pLiveMatchData->GetLiveEntryWork();
          
          return(false
            ||  (liveEntryWork == 1)
          );
        }


  //------------------------------------------------------------------
  /**
    *  @brief    大会メニュー：「挑戦する/解除する」が有効か
    */
  //------------------------------------------------------------------
  bool  IsBattleOrRetireAvailable(void) const
        {
          const u8  liveEntryWork = m_pLiveMatchData->GetLiveEntryWork();
          
          return(false
            ||  (liveEntryWork == 1)
            ||  (liveEntryWork == 2)
          );
        }




  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  enum  RegulationCheckResult
  {
    REGULATION_CHECK_RESULT_OK,
    
    REGULATION_CHECK_RESULT_Error_CRCMismatch,
    REGULATION_CHECK_RESULT_Error_ROMVersion,
    REGULATION_CHECK_RESULT_Error_LanguageID,
    REGULATION_CHECK_RESULT_Error_SameID,

    REGULATION_CHECK_RESULT_
  };

  RegulationCheckResult CheckRegulation(void) const
  {
    GFL_ASSERT(m_pSavedRegulation);
    GFL_ASSERT(m_pReceivedRegulation);

    /*  CRC                   */
    {
//      const bool  bSavedRegCRCCheck     = m_pSavedRegulation->CheckCrc();
      const bool  bSavedRegCRCCheck     = true;   /*  初期状態はCRCがないので手元のデータはチェックしない  */
      const bool  bReceivedRegCRCCheck  = m_pReceivedRegulation->CheckCrc();
      
      
      if((!bSavedRegCRCCheck) || (!bReceivedRegCRCCheck))
      {
        GFL_PRINT("%s: crc check failed (%d, %d)\n", __FUNCTION__, bSavedRegCRCCheck, bReceivedRegCRCCheck);
        return REGULATION_CHECK_RESULT_Error_CRCMismatch;
      }
    }


    /*  ROMVersion            */
    {
      if(!m_pReceivedRegulation->CheckParamRom(GET_VERSION()))
      {
        GFL_PRINT("%s: illegal ROMVersion(%d)\n", __FUNCTION__, GET_VERSION());
        return REGULATION_CHECK_RESULT_Error_ROMVersion;
      }
    }


    /*  LanguageID            */
    {
      const u8  languageID = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetPokeLanguageId();
      if(!m_pReceivedRegulation->CheckParamLanguage(languageID))
      {
        GFL_PRINT("%s: illegal langID(%u)\n", __FUNCTION__, languageID);
        return REGULATION_CHECK_RESULT_Error_LanguageID;
      }
    }

    /*  CompetitionID(CupID)  */
    {
      if(CompareCompetitionID())
      {
        GFL_PRINT("%s: sameID\n", __FUNCTION__);
        return REGULATION_CHECK_RESULT_Error_SameID;
      }
    }


    /*  OK  */
    GFL_PRINT("%s: OK\n", __FUNCTION__);
    return REGULATION_CHECK_RESULT_OK;
  }


  //------------------------------------------------------------------
  /**
    *  @brief    SaveDataのRegulationのcup_idと受信Regulationのcup_idを比較
    */
  //------------------------------------------------------------------
  bool  CompareCompetitionID(void) const
  {
    GFL_ASSERT(m_pSavedRegulation);
    GFL_ASSERT(m_pReceivedRegulation);
    
    return  (GetCompetitionIDFromRegulation(*m_pSavedRegulation) == GetCompetitionIDFromRegulation(*m_pReceivedRegulation));
  }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  void  SetReceivedRegulation(const SerializedRegulation& rSerializedRegulation)
  {
    m_pReceivedRegulation->Deserialize(&rSerializedRegulation, sizeof(rSerializedRegulation));
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static u32 GetCompetitionIDFromRegulation(const Regulation& rRegulation)
  {
    return rRegulation.GetValueParam(Regulation::DISCRIMINANT_ID);
  }



  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  u32   GetRemainingMatches(void) const
        {
          const u32 matchesFromRegulation = m_pSavedRegulation->GetValueParam(Regulation::BATTLE_NUM);
          const u32 matchesFromResult     = m_pLiveMatchData->GetLiveResults()->battle_num;
          
          return ((matchesFromRegulation > matchesFromResult) ? (matchesFromRegulation - matchesFromResult) : 0);
        }


  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  bool  IsReturnedFromQRRead(void) const  {return (m_BootMode == BOOTMODE_CeckRegulation_FromQRRead);}
  void  ResetBootMode(void)               {m_BootMode = BOOTMODE_Top;}


};


} /*  namespace NetApp  */
} /*  namespace Live    */
#endif  /*  #if !defined(LIVEAPPPARAM_H_INCLUDED)  */

