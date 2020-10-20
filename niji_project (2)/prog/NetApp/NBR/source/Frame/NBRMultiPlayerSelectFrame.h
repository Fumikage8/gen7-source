// ============================================================================
/*
 * @file NBRMultiPlayerSelectFrame.h
 * @brief 通信対戦受付アプリのマルチチーム振り分けプロセスです。
 *        フレームID:NBR_FRAME_ID_MULTI_PLAYER_SELECT
 * @date 2015.07.03
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#if !defined( NBR_MULTIPLAYERSELECT_FRAME_H_INCLUDE )
#define NBR_MULTIPLAYERSELECT_FRAME_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetApp/NBR/source/NBRFrameBase.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"

// Net
#include "NetStatic/NetAppLib/include/NBR/NBRNet.h"
#include "NetStatic/NetAppLib/include/NBR/NBRNetRecvListener.h"
// View
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMultiSelect.h"
#include "NetApp/NBR/source/View/SubView/NBRLowerViewMultiSelectListener.h"

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)
GFL_NAMESPACE_BEGIN(Frame)

class NBRMultiPlayerSelectFrame : 
  public NBRFrameBase,
  public NBRNetRecvListener,
  public View::NBRLowerViewMultiSelectListener

{
  GFL_FORBID_COPY_AND_ASSIGN( NBRMultiPlayerSelectFrame );
public:
  // コンストラクタ
  NBRMultiPlayerSelectFrame( NetApp::NBR::ApplicationSystem::NBRWork* pNBRWork,
                             View::NBRParentView* pParentView,
                             NBRNet* pNet );
  // デストラクタ
  virtual ~NBRMultiPlayerSelectFrame();

private:

  // NetAppFrameBase
  virtual bool startup();
  virtual void setListener();
  virtual void removeListener();
  virtual void updateSequence();
  virtual bool cleanup();

public:
  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NetRecvListener
  //------------------------------------------------------------------
  /**
   * @brief  通信終了通知
   */
  //------------------------------------------------------------------
  virtual void OnNBRNetRecvEnd();

  //------------------------------------------------------------------
  //------------------------------------------------------------------
  // NBRLowerViewMultiSelectListener
  //--------------------------------------------------------------------------
  /**
   * @brief     指定番目のプレイヤー選択ボタン押下通知
   * @param[in] 1～4番目
   */
  //--------------------------------------------------------------------------
  virtual void OnPlayerSelectButtonAction( int no );

  //--------------------------------------------------------------------------
  /**
   * @brief   決定ボタン押下通知
   */
  //--------------------------------------------------------------------------
  virtual void OnDecideButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   戻るボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnBackButtonAction();


  //------------------------------------------------------------------
  // NBRLowerViewMessageMenuListener
  //------------------------------------------------------------------
  /**
   * @brief   はいボタン通知
   */
  virtual void OnYesButtonAction();

  /**
   * @brief   いいえボタン通知
   */
  virtual void OnNoButtonAction();

  //--------------------------------------------------------------------------
  /**
   * @brief   マルチ用プレイヤー選択ボタン通知
   */
  //--------------------------------------------------------------------------
  virtual void OnMutliButtonAction( u32 buttonIndex );

private:

  //! 基本画面にする
  void ChangeMainView();
  //! つうしん中の画面にする
  void ChangeNetworkWaitView();

  //! 立ち位置の更新
  bool PlayerPosUpdate();

  //! 全ての立ち位置決定したかチェック
  bool IsAllPlaceDecide();
  //! 空の立ち位置のNetIDをランダムで取得
  int GetEmptyPlaceNetIdAtRandom();

private:

  /*
   * @enum Sequence
   * シーケンス定義
   */
  enum Sequence{
    SEQ_GUIDE_MESSAGE_WAIT,     //! ガイドメッセージ終了待ち
    SEQ_TEAM_BUTTON_WAIT,       //! チームボタン押下待ち
    SEQ_MULTI_SELECT_SEND,      //! マルチ振り分け選択送信
    SEQ_MULTI_DECIDE_SEND,      //! マルチ振り分け決定送信

    SEQ_TIMING_TO_BATTLE_START, //! バトルへ同期
  };

  // SubView
  View::NBRLowerViewMultiSelect*      m_pLowerView;   //! 下画面 


  Sequence                            m_seq;          //! シーケンス
  int                                 m_targetPlace;  //! 決める立ち位置
  int                                 m_targetNetId;  //! 決めるNetId

  int                                 m_decideNetId[ BTL_CLIENT_NUM ]; //! 立ち位置が要素。立ち位置決定した場合NetIDが入っている。初期値-1
  bool                                m_randomSelectEnable;     //! 選択肢にお任せが有るかどうか。trueである
  
  enum{
    BUTTON_INDEX_RANDOM = 100,
    BUTTON_INDEX_RETURN = 200,
  };
  int                                 m_buttonIndex2NetIdTbl[6];  //! プレイヤー選択肢リストのボタンIndexからNetIdに変換するテーブル
  

};


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)

#endif // NBR_MULTIPLAYERSELECT_FRAME_H_INCLUDE
