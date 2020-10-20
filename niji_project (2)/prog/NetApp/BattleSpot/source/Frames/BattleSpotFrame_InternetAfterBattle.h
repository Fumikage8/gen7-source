#if !defined(BATTLESPOTFRAME_INTERNETAFTERBATTLE_H_INCLUDED)
#define BATTLESPOTFRAME_INTERNETAFTERBATTLE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotFrame_InternetAfterBattle.h
 * @date   2015/10/02 Fri. 13:31:47
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../BattleSpotFrameDef.h"
#include  "../Views/BattleSpotViewContainer.h"
#include "NetStatic/NetLib/source/P2P/NexRatingClient.h"

#include "NetStatic/NetAppLib/source/BattleVideoPlayer/RequestSequence/BattleVideoResponseListener.h"

namespace NetApp    {
namespace BattleSpot {
  class BattleSpotWork;
  class BattleSpotViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief
  */
//==============================================================================
class Frame_InternetAfterBattle
  : public    NetAppLib::System::NetAppFrameBase
  , public    NetApp::BattleVideo::BattleVideoResponseListener
  , protected LowerView_Selection::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_InternetAfterBattle );

public:
  Frame_InternetAfterBattle(BattleSpotWork* pWork);
  virtual ~Frame_InternetAfterBattle();

  //-------------------------------------
  /// シーケンス定義	
  //=====================================
  enum{
    SEQ_SEND_REPORT=0,    ///< 成績送信
    SEQ_WAIT,           ///< 待ち

    SEQ_ERROR_RETURN,   ///< 通信エラー以外のエラー
    SEQ_MESSAGE_WAIT,   ///< メッセージ待ち
  };

protected:
//  virtual void  exitFrame(NetAppLib::System::FrameResult result );

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  /*  implement LowerView_Selection::IEventHandler  */
  virtual void  LowerView_Selection_OnEvent(LowerView_Selection* pSender, const LowerView_Selection::IEventHandler::EventCode eventCode);

  void  ExitSelf(const FrameResult frameResult = RESULT_RatingBattleEntry_Back);

  bool DisconnectWait();

  void UpdateRankData();

    void SendBattleVideo();

  //
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   通信エラー
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnError( BattleVideoResponseListener::ERROR_TYPE errorType );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   同期成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnSyncSuccess( void ){};

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   削除成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnDeleteSuccess( void ){};


  //--------------------------------------------------------------------------------------------
  /**
   * @brief   公開取り消し失敗（公開当日だったら取り消しに失敗する）
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnCancelUploadFailed( void ){};

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   アップロード成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnUploadSuccess( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   バトルビデオが不正
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnUploadInvalid( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ダウンロード成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnDownloadSuccess( ExtSavedata::BattleRecorderSaveData::BATTLE_REC_UPLOAD_DATA* pDownloadData , u32 playCount , ExtSavedata::BattleRecorderSaveData::REC_DATE* pDate ){};

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   ダウンロードしたいファイルが見つからない
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnDownloadNotFound( void ){};

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   再生回数インクリメント成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnIncPlayCountSuccess( void ){};

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   再生完走通知成功
   */
  //--------------------------------------------------------------------------------------------
  virtual void OnPlayCompleteSuccess( void ){};


  /*
    members
  */
  BattleSpotWork*           m_pWork;
  BattleSpotViewContainer*  m_pViewContainer;
  u32                       m_State;
  FrameResult               m_Result;


  /*  views  */
//  UpperView_Demo*           m_pUpperView;
  LowerView_Selection*      m_pLowerView;

  u32 m_seq;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTFRAME_INTERNETAFTERBATTLE_H_INCLUDED)  */
