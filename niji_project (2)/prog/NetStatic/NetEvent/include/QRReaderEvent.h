
#if !defined(QRREADEREVENT_H_INCLUDED)
#define QRREADEREVENT_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderEvent.h
 * @date   2016/01/14 Thu. 19:56:04
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>
#include  "GameSys/include/GameManager.h"
#include  "GameSys/include/GameEvent.h"
#include  "NetApp/QRReader/include/QRReaderProcParam.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppClearSettingUtility.h"

/*  save  */
#include  "Savedata/include/MyItemSave.h"

/*
  forward declare
*/
namespace NetAppLib  {
namespace System {
  class ApplicationSystemBase;
}
}
namespace NetApp {
namespace QRReader {
  class QRReaderProc;
}
}

namespace NetEvent  {
namespace QRReader {
// =============================================================================
/**/
//==============================================================================
enum  QRReaderResult
{
  RESULT_Invalid,                 /**<  無効値  */

  /*  Live  */
  RESULT_Live_NotDecoded,         /**<  MODE_Live:デコードされなかった  */
  RESULT_Live_Decoded,            /**<  MODE_Live:デコードされた        */
  
  /*  FriendMatch  */
  RESULT_FriendMatch_NotDecoded,  /**<  MODE_FriendMatch:デコードされなかった */
  RESULT_FriendMatch_Decoded,     /**<  MODE_FriendMatch:デコードされた       */
  
  /*  Team  */
  RESULT_Team_NotDecoded,         /**<  MODE_Team:デコードされなかった  */
  RESULT_Team_Decoded,            /**<  MODE_Team:デコードされた        */
  
  
  RESULT_
};



/**
  Event起動パラメータ
  ポインタ参照先はEvent実行中は保持されること
*/
typedef struct  QRReaderEventParam
{
  enum  Mode
  {
    MODE_QRScan,        /**<  fieldからのQRScan   */

    MODE_Live,          /**<  ライブ大会          */
    MODE_FriendMatch,   /**<  なかま大会          */
    MODE_Team,          /**<  チーム選択          */

    MODE_
  };

  Mode              mode;               /**<  [in]  起動モード                   */

  Regulation*       pDst_Regulation;    /**<  [out] FriendMatch/LiveMatch時出力  */
  pml::PokeParty*   pDst_PokeParty;     /**<  [out] TeamSelect時出力             */
  QRReaderResult    result;             /**<  [out] Event結果                    */

  /*    */
  QRReaderEventParam(void)
    : mode(MODE_QRScan)
    , pDst_Regulation(NULL)
    , pDst_PokeParty(NULL)
    , result(RESULT_Invalid)
  {}
}QRReaderEventParam;



//==============================================================================
/**
  *  @class  QRReaderEvent
  */
//==============================================================================
/*
  2016/01/05
  通信エラーハンドリングが不要のため、指示によりBaseClassをNetEventBaseからGameEventに変更
*/
class QRReaderEvent
  : public    GameSys::GameEvent
  , protected NetAppLib::Util::NetAppClearSettingUtility
{
  GFL_FORBID_COPY_AND_ASSIGN( QRReaderEvent );

public:
  /*  イベント起動  */

  //------------------------------------------------------------------
  /**
    *  @brief    Field側からの呼び出し用
    *  @note     引数なし、本編との通信はSavedataを介して行う想定
    */
  //------------------------------------------------------------------
  static void StartEventFromField(GameSys::GameManager* pGameManager);


  static void StartEvent(GameSys::GameManager* pGameManager, QRReaderEventParam& rEventParam);

  QRReaderEvent( gfl2::heap::HeapBase* pHeap );
  virtual ~QRReaderEvent();

  /*
      GameEvent I/F
  */
  virtual bool  BootChk( GameSys::GameManager* pGameManager );
  virtual void  InitFunc( GameSys::GameManager* pGameManager );
  virtual void  EndFunc( GameSys::GameManager* pGameManager );
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* pGameManager );
  /*
      End GameEvent I/F
  */

private:
  /*
    funcs
  */
  void  ResultHandler(void);


  /*
      members
  */
  QRReaderEventParam*               m_pEventParam;
  NetApp::QRReader::APP_PARAM       m_ProcParam;
  NetApp::QRReader::QRReaderProc*   m_pProc;
};  /*  class QRReaderEvent  */


} /*  namespace QRReader  */
} /*  namespace NetApp       */
#endif  /*  #if !defined(QRREADEREVENT_H_INCLUDED)  */
