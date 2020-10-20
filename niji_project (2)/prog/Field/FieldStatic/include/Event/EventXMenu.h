//=================================================================================================
/**
 * @file   EventXMenu.h
 * @brief  アプリケーションのプロセスを呼び出すイベント
 * @author obata_toshihiro   ==> k.ohno
 * @date   2011.06.29  ->> 2015.06.09
 */
//=================================================================================================
#pragma once
#ifndef __EVENTXMENU_H__
#define __EVENTXMENU_H__
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
#include <GameSys/include/DllProc.h>

#include "Field/FieldStatic/include/Subscreen/SubscreenRequestDef.h"
#include "Field/FieldStatic/include/Event/EventXMenuListener.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)
class FieldMenuFrame;
class FieldMenuSubWindowProc;
GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)


GFL_NAMESPACE_BEGIN(Field)

class EventXMenu : public GameSys::GameEvent, public EventXMenuListener
{
  GFL_FORBID_COPY_AND_ASSIGN( EventXMenu ); // コピーを禁止

public:


  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap         workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  EventXMenu( gfl2::heap::HeapBase* heap );
  virtual ~EventXMenu();



  /* ------------------------------------------------------------------------- */
  /**
   * @brief 描画クラスの登録
   *
   * @param p_context
   */
  /* ------------------------------------------------------------------------- */
  void SetFieldMenuProc( App::FieldMenu::FieldMenuSubWindowProc* pProc );
  void SetFieldMenuProc( App::FieldMenu::FieldMenuSubWindowProc* pProc, Field::SubScreen::FieldMenuOutRequestID reqno  );
  
  /* ------------------------------------------------------------------------- */
  /**
   * @brief 初期化データを登録
   *
   * @param p_context
   */
  /* ------------------------------------------------------------------------- */
  void SetContext( void * p_context )
  {
    m_context = p_context;
 //   m_callback_work.p_context = p_context;
  }
  /* -------------------------------------------------------------------------*/
  /**
   * @brief 初期化データを取得
   *
   * @return 初期化データ
   */
  /* -------------------------------------------------------------------------*/
  void* GetContext( void )
  {
    return m_context;
  }

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アプリ切り替え時にBGMのプッシュ/ポップを行うか設定する
   *
   * @param is_push trueのときBGMをプッシュ/ポップする
   */
  /* -------------------------------------------------------------------------*/
  void SetBgmPushPopFlag(bool is_push );

  //--------------------------------------------------------------
  /**
   * @brief フィールドからアプリケーションに切り替える際 / アプリケーションからフィールドに戻る際、
   *        フェードアウトを行うかどうかを設定する。
   */
  //--------------------------------------------------------------
//  void SetFadeMode( FadeMode mode );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief フィールドをコントロールするか切り替える
   *
   * @param is_control フィールドコントロールフラグ
   */
  /* ------------------------------------------------------------------------- */
  void SetFieldControl( bool is_control );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief フィールドCloseのタイプ
   *
   * @param fieldCloseType FieldCloseType型
   *
   * @note  この関数を呼び出さない場合のデフォルトはFIELD_CLOSE_RESIDENT_APP_KEEP
   */
  /* ------------------------------------------------------------------------- */
 // void SetFieldCloseType( FieldCloseType fieldCloseType );

  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc( GameSys::GameManager* gmgr );

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc( GameSys::GameManager* gmgr );


  //  XMENU終了を伝えるListener
  virtual void SetXMenuEnd(void);
  //  次の挙動を伝達する
  virtual void SetNextFunction(Field::SubScreen::FieldMenuOutRequestID);

  /* -------------------------------------------------------------------------*/
  /**
   * @brief コールターゲットを取得
   *
   * @return
   */
  /* -------------------------------------------------------------------------*/
//  CallTarget GetCallTarget( void )
//  {
//    return m_call_target;
 // }

  /* -------------------------------------------------------------------------*/
  /**
   * @brief パラメータの取得
   *
   * @returnパラメータ
   */
  /* -------------------------------------------------------------------------*/
  inline void* GetParam( void ){ return m_param; }

  /* ------------------------------------------------------------------------- */
  /**
   * @brief アプリコール時の 2000年 1月 1日 00:00:00 からの経過秒数を取得
   */
  /* ------------------------------------------------------------------------- */
  inline s64 GetCallStartSecond( void ) const { return m_call_start_second; }

private:

  /* -------------------------------------------------------------------------*/
  /**
   * @brief アプリケーションの呼び出し
   *
   * @param game_manager
   */
  /* -------------------------------------------------------------------------*/
  void CallAppProc( GameSys::GameManager* game_manager );

  //----------------------------------------------------------------------------
  /**
   *  @brief  アプリケーションからの戻り値をチェック
   */
  //-----------------------------------------------------------------------------
  void RetAppProc( GameSys::GameManager* game_manager );

  //----------------------------------------------------------------------------
  /**
   *  @brief  BGMのボリュームを変更するかをチェック
   */
  //-----------------------------------------------------------------------------
  bool CheckVolumeControl(void);


  /* -------------------------------------------------------------------------*/
  /**
   * @brief デフォルトパラメータ設定関数
   *
   * @param p_work       コールバックワーク
   * @param p_gman       ゲームマネージャ
   *
   * @note  削除予定関数です
   */
  /* -------------------------------------------------------------------------*/
//  static void EventAppCall_DefaultParamSetup( EventAppCall::CALLBACK_WORK* p_work, GameSys::GameManager* p_gman );


  gfl2::heap::HeapBase* m_heap;          ///< ヒープ
 // CallTarget m_call_target;             ///< 呼び出すターゲット
 // FadeMode   m_fadeMode;                ///< フェード実行モード
  void* m_param;                        ///< プロセスに渡すパラメータ
  bool  m_param_alloc_flag;             ///< パラメータを確保したか？
  void* m_context;
  int m_mainSeq;
  bool mReportEnd;
  Field::SubScreen::FieldMenuOutRequestID mID; 

  App::FieldMenu::FieldMenuSubWindowProc* m_pFieldMenuProc;
  
 // App::FieldMenu::FieldMenuFrame* m_pFieldMenuFrame;

  Field::SubScreen::FieldMenuOutRequestID m_ReqNo;
  
 // CALLBACK_FUNC m_end_callback_func;                  ///< 終了コールバック関数
//  CALLBACK_FUNC m_before_field_open_callback_func;    ///< フィールドオープン前コールバック関数
//  CALLBACK_WORK m_callback_work;        ///< コールバックワーク

//  FieldCloseType  m_fieldCloseType;     ///< フィールドクローズタイプ(デフォルトはFIELD_CLOSE_RESIDENT_APP_KEEP)
  bool m_field_control; ///< trueの時フィールドのオープンクローズを行う
  bool m_is_bgm_push;   ///< BGMをプッシュするか
  u32 m_seq;            ///< メインシーケンス
  s64 m_call_start_second;

//  nn::ro::Module* m_sequence_module;

};


GFL_NAMESPACE_END(Field)


#endif // __EVENTXMENU_H__
