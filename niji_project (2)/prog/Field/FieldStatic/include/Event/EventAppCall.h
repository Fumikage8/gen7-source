//=================================================================================================
/**
 * @file   EventAppCall.h
 * @brief  アプリケーションのプロセスを呼び出すイベント
 * @author obata_toshihiro   ==> k.ohno
 * @date   2011.06.29  ->> 2015.06.09
 */
//=================================================================================================
#pragma once
#ifndef __EVENTAPPCALL_H__
#define __EVENTAPPCALL_H__
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameEvent.h"
//#include <dll/DllProc.h>

#include "App\FieldMenu\include\FieldMenuFrame.h"


// field
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include/Item/FieldItemUse.h"
#include "Field/FieldStatic/include\Launcher/FieldApplicationLauncher.h"


GFL_NAMESPACE_BEGIN(Field)

class EventAppCall : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN( EventAppCall ); // コピーを禁止

public:

  struct CALLBACK_WORK
  {
    EventAppCall* p_app_call;
    void *p_param;
    void *p_context;
    u16   ret_wk1;
    u16   ret_wk2;
    u16   ret_wk3;
    u16   ret_wk4;
  };

  /**
   * @brief フェードモード設定
   */
  enum FadeMode
  {
    E_FADE_EXEC,                   //!< フェード実行
    E_FADE_SKIP_FADEIN,            //!< フェードインのみスキップ
    E_FADE_SKIP_FADEOUT,           //!< フェードアウトのみスキップ
    E_FADE_FLYSKY_FADEINT,         //!< 空をとぶ後専用フェードイン
    E_FADE_NONE,                   //!< フェードなし
  };

  typedef void (*CALLBACK_FUNC)( CALLBACK_WORK* p_work, GameSys::GameManager* p_gman );

  // 呼び出すアプリケーション
  enum CallTarget {
    CALL_TARGET_NULL,
    CALL_TARGET_POKELIST,           //!< ポケモンリスト
    CALL_TARGET_BAG,                //!< バッグ
    CALL_TARGET_ZUKAN,              //!< 図鑑
    CALL_TARGET_CONFIG,             //!< 設定画面
    CALL_TARGET_FINEDER,            //!< ファインダー
    CALL_TARGET_ISLAND,             //!< アイランド
    CALL_TARGET_TRAINERPASS,        //!< トレーナーパス
    CALL_TARGET_QR_READ,            //!< QR読み込み
    CALL_TARGET_ALBUM,              //!< マイアルバム
  };


  //--------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   * @param   heap         workをとるためのメモリクラス
   */
  //--------------------------------------------------------------
  EventAppCall( gfl2::heap::HeapBase* heap );
  virtual ~EventAppCall();

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 呼び出すアプリケーションを設定する（終了、フィールドオープン前コールバック設定）
   *
   * @param call_target
   * @param p_param
   * @param enc_callback_func
   * @param before_field_open_callback_func
   */
  /* -------------------------------------------------------------------------*/
  void SetCallTargetApplication(
    CallTarget call_target,
    void* p_param = NULL,
    CALLBACK_FUNC enc_callback_func=NULL,
    CALLBACK_FUNC before_field_open_callback_func=NULL );


  /* -------------------------------------------------------------------------*/
  /**
   * @brief 呼び出すアプリケーションを設定する(パラメータアロック番)
   *
   * @param call_target       呼び出すアプリケーション
   * @param size              パラメータサイズ
   * @param enc_callback_func 終了コールバック
   * @param before_field_open_callback_func フィールドオープン前コールバック
   *
   * @return パラメータポインタ
   */
  /* -------------------------------------------------------------------------*/
  void* SetCallTargetApplicationAndAllocParam(
    CallTarget call_target,
    u32 size,
    CALLBACK_FUNC enc_callback_func=NULL,
    CALLBACK_FUNC before_field_open_callback_func=NULL );

  /* -------------------------------------------------------------------------*/
  /**
   * @brief 呼び出すアプリケーションを設定する(パラメータアロック、フィールドをコントロールしない)
   *
   * @param call_target       呼び出すアプリケーション
   * @param size              パラメータサイズ
   * @param enc_callback_func 終了コールバック
   * @param before_field_open_callback_func フィールドオープン前コールバック
   *
   * @return パラメータポインタ
   */
  /* -------------------------------------------------------------------------*/
  void* SetCallTargetApplicationAndAllocParamNotFieldControl(
    CallTarget call_target,
    u32 size,
    CALLBACK_FUNC enc_callback_func,
    CALLBACK_FUNC before_field_open_callback_func );

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
    m_callback_work.p_context = p_context;
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
  void SetCallTargetApplicationNotFieldControl(
    CallTarget call_target,
    void* p_param = NULL,
    CALLBACK_FUNC enc_callback_func=NULL,
    CALLBACK_FUNC before_field_open_callback_func=NULL );

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
  void SetFadeMode( FadeMode mode );

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
  CallTarget GetCallTarget( void )
  {
    return m_call_target;
  }

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


  //----------------------------------------------------------------------------
  /**
   *  @brief  起動番号を保持
   *  @param  アプリ起動番号
   */
  //-----------------------------------------------------------------------------
  void SetAppNo(Field::SubScreen::FieldMenuOutRequestID no){ m_AppNo = no; };


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
  int CheckVolumeControl(void);


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
  static void EventAppCall_DefaultParamSetup( EventAppCall::CALLBACK_WORK* p_work, GameSys::GameManager* p_gman );


  /* -------------------------------------------------------------------------*/
  /**
   *  @brief イベントのセットアップを行なう
   *
   *  @param  pEvent  イベント
   */
  /* -------------------------------------------------------------------------*/
  void SetupEvent( GameSys::GameEvent * pEvent );

private:


  gfl2::heap::HeapBase* m_heap;          ///< ヒープ
  CallTarget m_call_target;             ///< 呼び出すターゲット
  FadeMode   m_fadeMode;                ///< フェード実行モード
  void* m_param;                        ///< プロセスに渡すパラメータ
  bool  m_param_alloc_flag;             ///< パラメータを確保したか？
  void* m_context;

  CALLBACK_FUNC m_end_callback_func;                  ///< 終了コールバック関数
  CALLBACK_FUNC m_before_field_open_callback_func;    ///< フィールドオープン前コールバック関数
  CALLBACK_WORK m_callback_work;        ///< コールバックワーク

 // FieldCloseType  m_fieldCloseType;     ///< フィールドクローズタイプ(デフォルトはFIELD_CLOSE_RESIDENT_APP_KEEP)
  bool m_field_control; ///< trueの時フィールドのオープンクローズを行う
  bool m_is_bgm_push;   ///< BGMをプッシュするか
  u32 m_seq;            ///< メインシーケンス
  s64 m_call_start_second;

  Field::SubScreen::FieldMenuOutRequestID m_AppNo;
  
  //nn::ro::Module* m_sequence_module;

  Location m_playerLocation;

  Field::Application::FieldmapData  m_fieldmapData; ///< フィールドマップから生成するデータ
  void * m_pAppParam; ///< EventAppCall内で生成するAPP_PARAM

  bool m_isFoceOut;
};


GFL_NAMESPACE_END(Field)


#endif // __EVENTAPPCALL_H__
