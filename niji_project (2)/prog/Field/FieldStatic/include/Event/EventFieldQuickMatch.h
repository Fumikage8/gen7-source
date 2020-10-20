//=================================================================================================
/**
 * @file   EventFieldQuickMatch.h.h
 * @brief  フィールドでレポート書くためのイベント
 * @author k.ohno
 * @date    2015.8.18
 */
//=================================================================================================
#pragma once
#ifndef __EVENT_FIELD_QUICK_MATCH_H__
#define __EVENT_FIELD_QUICK_MATCH_H__
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"

#include "App\FieldMenu\include\FieldMenuFrame.h"

// field
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include/Event/EventXMenuListener.h"


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)
class FieldMenuSubWindowProc;
GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)



GFL_NAMESPACE_BEGIN(Field)



class EventFieldQuickMatch : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventFieldQuickMatch ); // コピーを禁止

public:


  static void StartEvent( GameSys::GameManager* p_gameman );

  
  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap         workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  EventFieldQuickMatch( gfl2::heap::HeapBase* heap );
  virtual ~EventFieldQuickMatch();

  /* ------------------------------------------------------------------------- */
  /**
   * @brief 戻り値の設定
   *
   * @param ret_wk1 戻り値1（イベントワーク）
   * @param ret_wk2 戻り値2（イベントワーク）
   * @param ret_wk3 戻り値3（イベントワーク）
   * @param ret_wk4 戻り値4（イベントワーク）
   */
  /* ------------------------------------------------------------------------- */
  void SetRetWork( u16 ret_wk1, u16 ret_wk2 = NULL, u16 ret_wk3 = NULL, u16 ret_wk4 = NULL );

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
//    m_callback_work.p_context = p_context;
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

  //--------------------------------------------------------------
  /**
   * @brief 呼び出すアプリケーションを設定する
   *        フィールドマップの操作を行わない場合。
   */
  //--------------------------------------------------------------
 // void SetCallTargetApplicationNotFieldControl(
 //   CallTarget call_target,
  //  void* p_param = NULL,
  //  CALLBACK_FUNC enc_callback_func=NULL,
  //  CALLBACK_FUNC before_field_open_callback_func=NULL );

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
 // void SetFadeMode( FadeMode mode );

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
  //void SetFieldCloseType( FieldCloseType fieldCloseType );

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

  /* -------------------------------------------------------------------------*/
  /**
   * @brief コールターゲットを取得
   *
   * @return
   */
  /* -------------------------------------------------------------------------*/
//  CallTarget GetCallTarget( void )
 // {
 //   return m_call_target;
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


  void SetInit( App::FieldMenu::FieldMenuSubWindowProc* pProc,EventXMenuListener* pListener );
  
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


  //----------------------------------------------------------------------------
  /**
   *  @brief  プロセスのパラメータを破棄
   */
  //-----------------------------------------------------------------------------
  void FreeParam( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  プロセスのパラメータを生成
   *
   *  @param  size  サイズ
   */
  //-----------------------------------------------------------------------------
  void* AllocParam( u32 size );


  gfl2::heap::HeapBase* m_heap;          ///< ヒープ
  void* m_param;                        ///< プロセスに渡すパラメータ
  void* m_context;


  App::FieldMenu::FieldMenuSubWindowProc* m_pFieldMenuProc;

  bool m_field_control; ///< trueの時フィールドのオープンクローズを行う
  bool m_is_bgm_push;   ///< BGMをプッシュするか
  u32 m_mainSeq;            ///< メインシーケンス
  s64 m_call_start_second;

  Location m_playerLocation;
  
  EventXMenuListener* mpListener;
  
};


GFL_NAMESPACE_END(Field)


#endif // __EVENT_FIELD_QUICK_MATCH_H__
