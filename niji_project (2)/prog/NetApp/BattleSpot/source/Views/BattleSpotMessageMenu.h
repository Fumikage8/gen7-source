#if !defined(BATTLESPOTMESSAGEMENU_H_INCLUDED)
#define BATTLESPOTMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotMessageMenu.h
 * @date   2015/10/05 Mon. 18:17:19
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"



namespace NetApp      {
namespace BattleSpot  {
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/05 Mon. 18:21:49
  */
//==============================================================================
class MessageMenu
  : public  NetAppLib::UI::NetAppMessageMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(MessageMenu);
public:
  MessageMenu(BattleSpotWork* pWork);
  virtual ~MessageMenu();


  enum  Mode
  {
    MODE_None,

    /*  welcome  */
    MODE_Welcome_SelectMode,              /**<  モードを選んでください        */

    /*  freebattle  */
    MODE_FreeBattle_SelectRule,           /**<  どのルールで戦いますか？      */
    MODE_FreeBattle_PokemonRule,          /**<  どのルールで戦いますか？(特別なポケモン)      */


    /*  ratingbattle  */
    MODE_RatingBattle_SelectRule,         /**<  どのルールで戦いますか？      */

    /*  internetMatch  */
    MODE_InternetMatch_SelectMode,        /**<  なにをしますか？  */

    /*  friendMatch  */
    MODE_FriendMatch_SelectMode,          /**<  なにをしますか？  */

    /*  message  */
    MODE_Message_Connecting,              /**<  通信しています                */
    MODE_Message_SDCard,                  /**<  SDカードを確認しています                              */
    MODE_Message_RegulationUpdated,       /**<  レギュレーションが更新されました                      */
    MODE_Message_RegulationRepaired,      /**<  レギュレーションを修復しました                        */
    MODE_Message_Regulation_SDFull,       /**<  SDカードがいっぱいでレギュレーションを保存できません  */
    MODE_Message_ConnectionError,         /**<  通信エラーが発生しました                              */

    MODE_Message_SearchingOpponent,       /**<  対戦相手を探しています                        */
    MODE_Message_OpponentFound,           /**<  対戦相手が見つかりました                      */
    MODE_Message_OpponentNotFound,        /**<  対戦相手が見つかりませんでした                */

    MODE_Message_Checking,                /**<  大会の状態を確認しています          */
    MODE_Message_HowToEntry,              /**<  大会に参加するには                  */
    MODE_Message_NoTournament,            /**<  参加できる大会はありません          */
    MODE_Message_InvalidCupID,//ことなるたいかいです

    MODE_Message_SeasonIsOver,//シーズン終了しました
    MODE_Message_TournamentIsOver,        /**<  大会は終了しました                  */
    MODE_Message_TournamentIsOverUnlock,
    MODE_Message_CannotEntry,             /**<  この大会には参加できません          */
    MODE_Message_Unregistered,            /**<  大会の参加を解除しました            */
    MODE_Message_DownloadingCard,         /**<  選手証をダウンロードしています      */
    MODE_Message_DownloadedCard,          /**<  選手証のダウンロードが完了しました  */
    MODE_Message_TournamentNotStarted,    /**<  大会はまだ始まっていません          */
    MODE_Message_Retired,                 /**<  途中で参加を解除しました            */
    MODE_Message_GameSync,                /**<  バトルチームをロックしてゲームシンクを行いました  */
    MODE_Message_DifferentConsole,        /**<  登録したときの本体と異なります      */
    MODE_Message_NotAvailableTime,        /**<  対戦できない時間です                      */
    MODE_Message_PrescribedMatches,       /**<  対戦回数が決められた回数に達しました      */
    MODE_Message_QRRead,                  /**<  QRコードを撮影するためカメラを起動します  */
    MODE_Message_GetCard,                 /**<  デジタル選手証を取得しました              */

    MODE_Message_RetireConfirm,           /**<  リタイアとなります 参加を解除しますか？   */
    MODE_Message_RetireConfirm2,          /**<  本当によろしいですか？                    */

    MODE_Message_RetireConfirm_friend,
    MODE_Message_RetireConfirm2_friend,
    MODE_Message_RetireConfirm_no_reg,
    MODE_Message_Retire_NewCup,




    /*    */




    /*  others  */
    MODE_BattleVideoAutoUploadConfirm,    /**<  バトルビデオの自動送信を許可しますか？  */

    MODE_Message_ContinueConfirm,//つづけてあそびますか
    MODE_Message_ContinueConfirmRate,//つづけてあそびますかレート
    MODE_Message_ContinueConfirmCup,//つづけてあそびますかCUP

    MODE_Message_PGLNoRegisted,//PGL登録していません（レート）msg_bs_win_05

    MODE_Message_RegistPIDConfirm,//本体登録しますか
    MODE_Message_RegistPIDWait,//本体登録しています
    MODE_Message_RegistedPID,//本体登録しました

    MODE_Message_ResetRateConfirm,//リセットしますか
    MODE_Message_ResetRateWait,//リセットしています
    MODE_Message_ResetRate,//リセットしました

    MODE_MAX
  };
  void  Show(const Mode mode);
  void  Hide(void)  {HideMessage();}









protected:
  void  PreShow(void);
  void  ShowYN(const u32 messageID, const bool isImmediate);

  /*
    members
  */
  BattleSpotWork*   m_pWork;
};




} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTMESSAGEMENU_H_INCLUDED)  */
