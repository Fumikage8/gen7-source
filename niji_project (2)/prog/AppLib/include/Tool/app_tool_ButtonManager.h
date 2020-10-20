//=============================================================================
/**
 * @file app_tool_ButtonManager.h
 * @brief ボタン管理
 * @author obata_toshihiro -> Toru=Nagihashi
 * @date 2011.07.06 -> 2012.05.01
 * 
 *
 *    buttonManagerの問題点（名木橋記述　次回の反省点へ）
 *    １．通知タイミングが多く必要になる
 *          通知タイミングを増やせる仕組みが必要
 *          →通知タイミングを切り替える、だと複数の通知タイミングが必要なとき対応できない。
 *          →リスナー形式だと一応の解決
 *            (Get関数式だとその都度フラグと取得関数が必要になる)
 *
 *
 *    ２．ボタンごとに入力モードが異なる
 *          タッチ判定、リリース判定
 *          複数同時入力、排他入力
 *
 *          全ての機能をそなえたボタンマネージャーではなく、
 *          ボタン単品（ボタンごとに操作が違う）での継承をマネージャー（排他制御、同時制御）に登録する作りが望ましい？
 *            IButton -> TouchButton  ?
 *                    -> ReleaseButton?
 *                    
 *            IButtonManager  -> SingleButtonManager ?
 *                            -> MultipleButtonManager ?
 *
 *    ３．Managerが複数あるときの入力排他処理
 *          Managerが１つであれば、ボタンアニメ中に他のボタンが押せないようになるが、
 *          入力排他処理はManagerごとのため、他のManagerとの入力排他処理は外部でケアしなければならない
 *          
 *          Managerはシングルトンにしたり、別のマネージャー登録できる仕組みがあるべきだった。
 *          →若干UIViewの機能とかぶるかも
 *
 *
 *
 */
//=============================================================================
#ifndef __APPTOOLBUTTONMANAGER_H__
#define __APPTOOLBUTTONMANAGER_H__
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_Layout.h>
#include <ui/include/gfl2_UI.h>

namespace app {
  namespace tool {


    class ButtonManager
    {
      GFL_FORBID_COPY_AND_ASSIGN( ButtonManager );  // コピー禁止

      public:

      // ボタン識別子
      typedef u32 ButtonId;

      static const ButtonId BUTTON_ID_NULL;                   //!< ボタンの無効値
      static const u32 ANIMATION_NULL;                        //!< アニメーションの無効値
      static const u32 SE_NULL;  //!< 選択音の無効値
      static const u32 DEFAULT_LONG_PRESS_FRAME;                      //!< 長押しと判定するフレーム数
      static const f32 MOVE_PANE_DISTANCE;                          //!< タッチ操作キャンセル判定距離

      /**
       * @brief ボタンの状態コールバックを受け取るクラスのインターフェース
       *
       * @note
       * ボタン登録時に ButtonManager::AddButton() に渡すことで,
       * ボタンの状態に応じたメソッドがコールされます.
       */
      class IButtonCallback
      {
        public:
          /**
           * @brief ボタンが選択された際に呼ばれるコールバック関数
           * @param buttonID  選択されたボタンのID
           */
          virtual void CallbackFuncOnButtonSelected( u32 buttonID ) { }

          /**
           * @brief ボタンが長押しされた際に呼ばれるコールバック関数
           * @param buttonID  長押しされたボタンのID
           */
          virtual void CallbackFuncOnButtonLongPressed( u32 buttonID ) { }

          /**
           * @brief ボタンがキャンセルされた際に呼ばれるコールバック関数
           * @param buttonID キャンセルされたボタンのID
           * @note  キャンセルとは、ボタンタッチ→ボタン範囲外で離すことです
           *        そのためTriggerではありえません
           */
          virtual void CallbackFuncOnButtonCancel( u32 buttonID ) { }
          
          /**
           * @brief ボタン選択アニメ開始前に呼ばれるコールバック関数
           *
           * @param buttonID  アニメ開始するボタンID
           */
          virtual void CallbackFuncOnButtonSelectedBeforeAnime( u32 buttonID ) { }
          
          /**
           * @brief ボタンがタッチされた時に呼ばれるコールバック関数
           *
           * @param buttonID  タッチされた時のボタンID
           */
          virtual void CallBackFuncOnButtonTouchTrigger( u32 buttonID ) { }
      };


      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap               インスタンスの生成に使用するヒープ
       * @param ui_device_manager  操作に使用するUIデバイス(NULLをいれた場合はUpdateInputを使うことができます。これを使うと更新と入力を分けることが可能です)
       * @param max_button_num     管理可能なボタンの最大数
       * @param null_anime_index   アニメーションの無効値( 以降, この値のアニメーションは無効 )
       */
      ButtonManager(
          gfl2::heap::HeapBase* heap,
          const gfl2::ui::DeviceManager* ui_device_manager,
          u32 max_button_num,
          u32 null_anime_index = ANIMATION_NULL );

      /**
       * @brief デストラクタ
       */
      virtual ~ButtonManager();

      //-----------------------------------------------------------------------
      // ボタンの追加
      //    ボタンの追加は以下があります。
      //    １．ピクチャペインのみ指定版
      //    ２．ピクチャペインと境界ペインの指定版（こちら推奨です）
      //      もし１だとピクチャペインの絵がかわると当たり判定まで狭くなってしまうためです。
      //-----------------------------------------------------------------------
      /**
       * @brief ボタンを追加する
       * @param heap                    管理ワーク確保に使用するヒープ
       * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
       * @param layout_work             ボタンが所属するレイアウトワーク
       * @param pane_index              ボタンを表すペイン( 当たり判定に使用 )
       * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
       * @param release_anime_index     ボタンが離された際に再生するアニメーション
       * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
       * @param key_select_anime_index  キーで選択された際に再生するアニメーション
       * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
       */
      void AddButton(
          gfl2::heap::HeapBase* heap,
          ButtonId buttonID,
          gfl2::lyt::LytWk* layout_work,
          gfl2::lyt::LytPaneIndex pane_index,
          u32 touch_anime_index,
          u32 release_anime_index,
          u32 cancel_anime_index,
          u32 key_select_anime_index,
          IButtonCallback* callback_target );

      /**
       * @brief ボタンを追加する( 拡張版 )
       * @param heap                    管理ワーク確保に使用するヒープ
       * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
       * @param layout_work             ボタンが所属するレイアウトワーク
       * @param pane_index              ボタンを表すペイン( 当たり判定に使用 )
       * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
       * @param release_anime_index     ボタンが離された際に再生するアニメーション
       * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
       * @param key_select_anime_index  キーで選択された際に再生するアニメーション
       * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
       * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
       * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
       */
      void AddButton(
          gfl2::heap::HeapBase* heap,
          ButtonId buttonID,
          gfl2::lyt::LytWk* layout_work,
          gfl2::lyt::LytPaneIndex pane_index,
          u32 touch_anime_index,
          u32 release_anime_index,
          u32 cancel_anime_index,
          u32 key_select_anime_index,
          u32 active_anime_index,
          u32 passive_anime_index,
          IButtonCallback* callback_target );


      /**
       * @brief ボタンを追加する( 拡張版2 境界ペイン追加版 )
       * @param heap                    管理ワーク確保に使用するヒープ
       * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
       * @param layout_work             ボタンが所属するレイアウトワーク
       * @param picture_pane            ボタンの絵用ペイン( 非表示等使用 )
       * @param bound_pane              当たり判定用ペイン
       * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
       * @param release_anime_index     ボタンが離された際に再生するアニメーション
       * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
       * @param key_select_anime_index  キーで選択された際に再生するアニメーション
       * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
       * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
       * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
       */
      void AddButton(
          gfl2::heap::HeapBase* heap,
          ButtonId buttonID,
          gfl2::lyt::LytWk* layout_work,
          gfl2::lyt::LytPaneIndex picture_pane,
          gfl2::lyt::LytPaneIndex bound_pane,
          u32 touch_anime_index,
          u32 release_anime_index,
          u32 cancel_anime_index,
          u32 key_select_anime_index,
          u32 active_anime_index,
          u32 passive_anime_index,
          IButtonCallback* callback_target );

      /**
       * @brief ボタンを追加する( ペインのポインタ指定版 )
       * @param heap                    管理ワーク確保に使用するヒープ
       * @param buttonID                ボタンの識別子( 選択された際にUpdate()で返ってくる値 )
       * @param layout_work             ボタンが所属するレイアウトワーク
       * @param picture_pane            ボタンの絵用ペイン( 非表示等使用 )
       * @param bound_pane              当たり判定用ペイン
       * @param touch_anime_index       ボタンがタッチされた際に再生するアニメーション
       * @param release_anime_index     ボタンが離された際に再生するアニメーション
       * @param cancel_anime_index      ボタンから外れた際に再生するアニメーション
       * @param key_select_anime_index  キーで選択された際に再生するアニメーション
       * @param active_anime_index      選択可能な状態になった際に再生するアニメーション
       * @param passive_anime_index     選択不可能な状態になった際に再生するアニメーション
       * @param callback_target         ボタンが選択された際に通知されるコールバッククラス( 不要ならNULL )
       */
      void ButtonManager::AddButton(
          gfl2::heap::HeapBase* heap,
          ButtonId buttonID,
          gfl2::lyt::LytWk* layout_work,
          gfl2::lyt::LytPane* picture_pane,
          gfl2::lyt::LytPane* bound_pane,
          u32 touch_anime_index,
          u32 release_anime_index,
          u32 cancel_anime_index,
          u32 key_select_anime_index,
          u32 active_anime_index,
          u32 passive_anime_index,
          IButtonCallback* callback_target );

      //-----------------------------------------------------------------------
      // 動作設定
      //  以下の２つによりIsButtonSelectedやCallbackFuncOnButtonSelectedが呼ばれる
      //  タイミングを変えることができます。
      //  ・選択方法（入力検知タイミングの設定）
      //      通常はリリースです。
      //  ・通知方法（通知発行のタイミングの設定）
      //      通常はアニメ終了後に通知発行されます。
      //  
      //  それとはまた別に以下も設定可能です
      //  ・同時動作数
      //      通常はボタンは同時に１つしか動きません。これは複数押せる状態でのバグを懸念してのことです。
      //      ですが、名前入力など連続して複数同時にボタンを押したい場合は、そのモードに設定できます。
      //  ・ボタンモード
      //      スクロールするボタンはスクロールモードにしていしてください
      //
      //  キーとSEのバインド
      //  ボタンにキーとSEをバインドすることができます。
      //  ・キー
      //    ボタンがキーでも動く場合は、ここに設定することで、IsButtonSelected等で検知することができます。（現状Triggerのみです…）
      //  ・SE
      //    ボタン入力時にSEを鳴らす場合は、必ず自前で鳴らさずここで設定してください。
      //    決定アニメと同時にSEを再生します。
      //------------------------------------------------------------------------
      /**
       * @brief ボタンの選択方法
       */
      enum SelectType {
        SELECT_TYPE_TRIGGER,  // トリガ選択
        SELECT_TYPE_RELEASE,  // リリース選択( デフォルト )
        SELECT_TYPE_REPEATEDLY,  // 連打( デフォルト )
      };

      /**
       * @brief ボタンの選択方法を変更する
       * @param buttonID  設定を変更するボタン
       * @param type  設定する選択方法
       */
      void SetSelectType( ButtonId buttonID, SelectType type );

      /**
       * @brief すべてのボタンの選択方法を変更する
       * @param type  設定する選択方法
       */
      void SetSelectType( SelectType type );

      /**
       * @brief 入力を許可するかどうか
       *
       * @param is_enable true許可（デフォルト）  false不許可
       *
       * @note  falseの場合、アニメ等の更新は動くが、入力検知のみ動かなくなる
       * @note  SELECT_TYPE_RELEASE判定の場合、タッチ中に本関数をfalseにすると
       *        離されたと同様の挙動になります
       */
      void SetInputEnable( bool is_enable );

      /**
       * @brief 選択されたことを通知するタイミング
       */
      enum NotifyTimming {
        NOTIFY_START_ANIME,  // 選択アニメ開始時
        NOTIFY_AFTER_ANIME,  // 選択アニメ終了後( デフォルト )
      };

      /**
       * @brief 選択されたことを通知するタイミングを変更する
       * @param buttonID  設定を変更するボタン
       * @param timming  通知するタイミング
       */
      void SetNotifyTimming( ButtonId buttonID, NotifyTimming timming );

      /**
       * @brief すべてのボタンの選択されたことを通知するタイミングを変更する
       * @param timming  通知するタイミング
       */
      void SetNotifyTimming(NotifyTimming timming );

      enum ButtonProcess {
        BUTTON_PROCESS_SINGLE,  // ボタンが同時に動かず必ず１つだけ動くのモード（デフォルト）
        BUTTON_PROCESS_MULTIPLE,  // 登録ボタンすべて同時に動くモード。同時に動くということは、同時に押したりした際に受けて側できっちりと管理する必要があります。
                                  // また連打がきくようになり、NotifyTimmingがSTART_ANIMEになります。
      };
      /**
       * @brief ボタン同時動作数モード設定
       * @param processモード
       * @param heap ヒープ 内部でマネージャーを切り替えるためクラスを生成しなおしています
       * @note デフォルト動作では、１つのボタンが動作（タッチからアニメ終了まで）している間は他のボタンが押せないようになっています。これはバグを懸念してのことです。基本の動作はデフォルトのこのモードでよいですが、名前入力などボタンを連続で入力する箇所はMULTIPLEをつかってください。
       */
      void ChangeButtonProcessMode( ButtonProcess process, gfl2::heap::HeapBase * heap );

      enum ButtonMode {
        BUTTON_MODE_FIX,    //固定ボタンモード 固定ボタンモードはボタンをたっち→範囲外にドラッグし範囲内に戻して離しても決定可能。（デフォルト）
        BUTTON_MODE_SCROLL, //スクロールボタンモード スクロールボタンモードはボタンをタッチし範囲外にドラッグし範囲内に戻しても決定されない。また、トリガ時からボタンのペイン位置が一定距離動いていたらキャンセル扱いになる。
      };

      /**
       * @brief ボタン判定モードの設定
       *
       * @param mode 判定モード ButtonMode列挙参照
       *
       * @note SELECT_TYPE_RELEASEでないと上記設定は意味がありません。
       */
      void SetButtonMode( ButtonMode mode );
      
      /**
       * @brief ボタン判定モードの設定  ボタンごと版
       *
       * @param buttonID  判定モードを設定するボタン
       * @param mode 判定モード ButtonMode列挙参照
       *
       * @note SELECT_TYPE_RELEASEでないと上記設定は意味がありません。
       */
      void SetButtonMode( ButtonId buttonID, ButtonMode mode );

      /**
       * @brief ボタンの選択音(SE)を設定する
       * @param buttonID  選択音を設定するボタン
       * @param SEID      選択時に再生するＳＥ( SE_NULLなら再生しない )
       *
       * @note デフォルトは SE_NULL( 音なし )
       */
      void SetButtonSelectSE( ButtonId buttonID, u32 SEID );

      /**
       * @brief 長押しと判定するフレーム数を設定する
       * @param buttonID 設定するボタン
       * @param frame    長押しと判定するフレーム数
       */
      void SetButtonLongPressFrame( ButtonId buttonID, u32 frame );

      /**
       * @brief キー入力とバインドする
       * @param buttonID 設定するボタン
       * @param key      バインドするキー入力( gfl::ui::BUTTON_xxxx )
       */
      enum BindKeyMode
      {
        BIND_KEY_MODE_TRIGGER,
        BIND_KEY_MODE_HOLD,
        BIND_KEY_MODE_REPEAT,
        BIND_KEY_MODE_RELEASE,
      };
      void SetButtonBindKey( ButtonId buttonID, u32 key, BindKeyMode mode = BIND_KEY_MODE_TRIGGER );
      
      /**
       * @brief アニメーション中に非表示にした場合等に、アニメを初期値に戻す
       *
       * @param buttonID  ボタンID
       * 
       * @note 内部で ResetHoldState も呼ばれることに注意
       */
      void ResetButtonAnime( ButtonId buttonID );

      //-----------------------------------------------------------------------
      // 基本動作
      //-----------------------------------------------------------------------
      // 更新後の状態
      enum UpdateState {
        UPDATE_STATE_IDLE,                 // 何もしていない
        UPDATE_STATE_HOLDING,              // ボタンを押さえている
        UPDATE_STATE_SELECT_ANIME_PLAYING, // ボタン選択アニメ再生中
        UPDATE_STATE_SELECTED,             // ボタンが選択された
        UPDATE_STATE_CANCEL,              // ボタンがキャンセルされた
      };

      /**
       * @brief 更新処理( 毎フレーム呼んでください )
       * @return 選択されたボタンのID
       * @return 更新後の状態
       */
      UpdateState Update( void );
      
      /**
       * @brief 入力処理
       *
       * @param pUiButton     ボタン  NULLだとボタン検知をしない
       * @param pUiTouchPanel タッチ  NULLだとタッチ検知をしない
       * 
       * @note  この入力処理はコンストラクタのui_device_managerにNULLをいれたときだけ稼働します。
       *        毎フレーム、またはボタンやタッチに反応があったときだけUpdateInputをすることで、更新処理の中で入力処理が動きます。
       *        またこの関数は１フレームに何回使っても構いません。
       *        例えば、buttonの処理とtouchpanelの処理を分けたい場合、２回にわけて呼んでもかまいません
       *        
       */
      void UpdateInput( const gfl2::ui::Button* pUiButton, const gfl2::ui::TouchPanel* pUiTouchPanel );

      /**
       * @brief 全てのボタン動作をロックする（Updateを呼ばないのとほぼ同じ状態になります）
       *
       * @note  入力だけ不許可にしたい場合はSetInputEnableを使用してください
       */
      void LockButtonControl( void );

      /**
       * @brief 動作ロックを解除する
       */
      void UnlockButtonControl( void );

      /**
       * @brief ボタンのホールド状況をリセットする
       */
      void ResetHoldState( void );

      /**
       * @brief 更新状況を取得する
       * @note  他の操作と重複しないようにこの値を見てください。
       */
      UpdateState GetUpdateState( void ) const;

      //-----------------------------------------------------------------------
      // 選択状態の取得
      //  取得は以下があります。
      //  １．選択を取得（通常時はこちらを使ってください）SelectType、NotifyTimmingで設定された状態で選択が返ります。詳しくは「動作設定の欄を見てください」
      //  ２．トリガーを取得
      //  ３．押されているかを取得
      //  ４．キャンセルされたかを取得
      //-----------------------------------------------------------------------
      /**
       * @brief ボタンが選択されたかどうかを取得する（NotifyTimmingで指定した通知を行います）
       * @retval true  いずれかのボタンが選択された
       * @retval false どのボタンも選択されていない
       */
      bool IsButtonSelected( void ) const;

      /**
       * @brief 選択されたボタンを取得する（NotifyTimmingで指定した通知を行います）
       * @return 選択されたボタンのID
       * @retval BUTTON_ID_NULL  どのボタンも選択されていない場合
       */
      ButtonId GetSelectedButtonId( void ) const;

      /**
       * @brief ボタンが押されたかどうかを調べる
       * @retval true  いずれかのボタンが押された
       * @retval false どのボタンも押されていない
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      bool IsButtonTrigger( void ) const;

      /**
       * @brief 押されたボタンを取得する
       * @return 押されたボタンのID
       * @retval BUTTON_ID_NULL  どのボタンも押されていない場合
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      ButtonId GetTriggerButtonId( void ) const;

      /**
       * @brief ボタンが押されているかどうかを調べる
       * @retval true  いずれかのボタンが押されている
       * @retval false どのボタンも押されていない
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      bool IsButtonHolding( void ) const;

      /**
       * @brief 押されているボタンを取得する
       * @return 押されているボタンのID
       * @retval BUTTON_ID_NULL  どのボタンも押されていない場合
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      ButtonId GetHoldingButtonId( void ) const;

      /**
       * @brief ボタンがキャンセルされたかどうかを調べる
       * @retval true  いずれかのボタンがキャンセルされた
       * @retval false どのボタンもキャンセルされていない
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      bool IsButtonCancel( void ) const;

      /**
       * @brief キャンセルされたボタンを取得する
       * @return キャンセルされているボタンのID
       * @retval BUTTON_ID_NULL  どのボタンもキャンセルされてない場合
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      ButtonId GetCancelButtonId( void ) const;
      
      /**
       * @brief ボタンが長押しされたかどうかを調べる
       * @retval true  いずれかのボタンが長押しされた
       * @retval false どのボタンも長押しされていない
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      bool IsButtonLongHolding( void ) const;

      /**
       * @brief 長押しされたボタンを取得する
       * @return 長押しされているボタンのID
       * @retval BUTTON_ID_NULL  どのボタンも長押しされてない場合
       *
       * @note SELECT_TYPE_RELEASE の場合にのみ有効
       */
      ButtonId GetLongHoldingButtonId( void ) const;


      //-----------------------------------------------------------------------
      // ボタンの有効性 以下の３つを使い分けられます。
      //  仕様にあわせて使ってください。
      //  ただし「有効性の設定」に関しては見た目はそのままのため、
      //  何かしらの方法で押せないことをプレイヤーに知らせる必要があるはずです。
      //  ・アクティブ・パッシブ  （内部でアクティブ・パッシブ用にアニメになります）
      //  ・表示・非表示  （ボタン事態が消えます）
      //  ・有効性の設定  （ボタンの見え方は通常通りですが、入力検知しません）
      //
      //-----------------------------------------------------------------------
      /**
       * @brief ボタンが有効かどうかを取得する
       * @param buttonID  調べるボタン
       * @retval true   有効
       * @retval false  無効
       */
      bool IsButtonActive( ButtonId buttonID ) const;

      /**
       * @brief ボタンを有効な状態にする
       * @param buttonID  有効にするボタン
       * @param isAnime   true指定時にアニメーションを行う
       */
      void SetButtonActive( ButtonId buttonID, bool isAnime=true );
      void SetButtonActiveForce( ButtonId buttonID, bool isAnime=true );  // 内部状態がアクティブであってもアクティブ処理を行う

      /**
       * @brief ボタンを無効な状態にする
       * @param buttonID  無効にするボタン
       * @param isAnime   true指定時にアニメーションを行う
       */
      void SetButtonPassive( ButtonId buttonID, bool isAnime=true );
      void SetButtonPassiveForce( ButtonId buttonID, bool isAnime=true );  // 内部状態がパッシブであってもパッシブ処理を行う

      /**
       * @brief ボタンを有効な状態にし、アニメは最終フレームにする。
       * @param buttonID  有効にするボタン
       */
      void SetButtonActiveAnimeFrameLast( ButtonId buttonID );
      /**
       * @brief ボタンを無効な状態にし、アニメは最終フレームにする。
       * @param buttonID  無効にするボタン
       */
      void SetButtonPassiveAnimeFrameLast( ButtonId buttonID );

      /**
       * @brief ボタンを表示する
       * @param buttonID  表示(非表示)するボタン
       * @param isResetAnime  アニメーション中のボタンを非表示にした際にリセットするかどうか
       */
      void SetButtonVisible( ButtonId buttonID, bool isResetAnime=false );

      /**
       * @brief ボタンを非表示にする
       * @param buttonID  表示(非表示)するボタン
       */
      void SetButtonInvisible( ButtonId buttonID );
      
      /**
       * @brief すべてのボタンを表示する
       */
      void SetButtonVisible( void );

      /**
       * @brief すべてのボタンを非表示にする
       */
      void SetButtonInvisible( void );

      /**
       * @brief ボタンが表示かどうかを取得する
       * @param buttonID  調べるボタン
       * @retval true   表示
       * @retval false  非表示
       */
      bool IsButtonVisible( ButtonId buttonID ) const;
      
      /**
       * @brief ボタンの動作をさせなくする
       *
       * @param is_enable trueで動作  falseで動作しない
       *        SetInputEnableのボタン指定版です
       */
      void SetButtonInputEnable( ButtonId buttonID, bool is_enable );
      
      /**
       * @brief ボタン動作を取得
       *
       * @param buttonID  ボタンID
       *
       * @return true動作 false動作しない 
       */
      bool IsButtonInputEnable( ButtonId buttonID ) const;

      /**
       * @brief ボタン選択時の処理を実行する
       * @param buttonID  ボタンＩＤ
       *
       * @note
       * ボタンを選択状態にします。
       *
       * 本関数の仕様変更をしました。これを行うと、
       * 内部で選択決定処理を行います。
       * その後、内部でアニメ処理、SE再生処理、コールバック呼び出しをします。
       * アニメ処理の関係上、この関数を読んでから、IsButtonSelected等で押されたか検知してください。
       * この関数と同時にシーケンス切り替え等をするとアニメ待ちを考慮しない実装になります。
       */
      void StartSelectedAct( ButtonId buttonID );

      /**
       * @brief ボタンの座標を取得する（グローバルなレイアウト座標で取得する）
       * @param buttonID  ボタンＩＤ
       * @return  ボタンの座標
       * @note  レイアウト座標とは、画面の中心が(0,0,0)、下画面なら右下が(160,-120,0)、となるもの
       */
      gfl2::math::VEC3    GetButtonPos( ButtonId buttonID ) const;
      /**
       * @brief ボタンの座標をGFベクトル3の形式で取得する（グローバルなレイアウト座標で取得する）
       * @param buttonID  ボタンＩＤ
       * @return  ボタンの座標
       * @note  レイアウト座標とは、画面の中心が(0,0,0)、下画面なら右下が(160,-120,0)、となるもの
       */
      gfl2::math::Vector3 GetButtonPosByGfVec3( ButtonId buttonID ) const;

      /**
       * @brief ルートの表示設定を使用するかどうか
       *
       * @param flag  true使用する（ルートが非表示ならばボタンも動作しない）
       *              false使用しない（ルートに無関係に）
       */
      void SetUseVisibleOfRoot( bool flag );

      /**
       * @brief 入力モードをセット
       *
       * @param input_mode  入力方法
       */
      enum InputMode
      {
        INPUT_MODE_BOTH,        //タッチ、キー両方きく（デフォルト）
        INPUT_MODE_KEY_ONLY,    //キーのみきく
        INPUT_MODE_TOUCH_ONLY,  //タッチのみきく
      };
      void SetInputMode( InputMode input_mode );


      /**
       * @func  LinkButton
       * @brief ボタン同士をリンクさせる
       * @date  2015.04.06
       *
       * @param tbl   ボタンIDテーブル
       * @param len   テーブル長
       */
      void LinkButton( const ButtonId * tbl, u32 len );

      /**
       * @func    IsButtonHit
       * @brief   指定IDのボタンがタッチ座標にあるか
       * @date    2015.10.26
       *
       * @param   id        ボタンID
       * @param   touch_x   タッチx座標
       * @param   touch_y   タッチy座標
       *
       * @retval  true  = ある
       * @retval  false = ない
       */
      bool IsButtonHit( ButtonId id, u16 touch_x, u16 touch_y ) const;


      private:

      // 1つのボタンを表すクラス
      class Button
      {
        public:
          Button(
              ButtonId buttonID,
              gfl2::lyt::LytWk* layout_work,
              gfl2::lyt::LytPaneIndex pane_index,
              gfl2::lyt::LytPaneIndex bound_pane_index,
              u32 null_anime_index,
              u32 hold_anime_index,
              u32 release_anime_index,
              u32 cancel_anime_index,
              u32 select_anime_index,
              u32 active_anime_index,
              u32 passive_anime_index,
              u32 selectSE,
              IButtonCallback* callback_target );
          Button(
              ButtonId buttonID,
              gfl2::lyt::LytWk* layout_work,
              gfl2::lyt::LytPane* pane_index,
              gfl2::lyt::LytPane* bound_pane_index,
              u32 null_anime_index,
              u32 hold_anime_index,
              u32 release_anime_index,
              u32 cancel_anime_index,
              u32 select_anime_index,
              u32 active_anime_index,
              u32 passive_anime_index,
              u32 selectSE,
              IButtonCallback* callback_target );
          virtual ~Button();
          bool UpdateInput( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input = false );
          bool UpdateMove( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button );
          bool IsSelected( void ) const;
          bool IsHolding( void ) const;
          bool IsCancel( void ) const;
          ButtonId GetId( void ) const;
          void GetTranslate( gfl2::math::VEC3* p_pos ) const;
          void SetVisibility( bool visibility );
          bool IsVisible( void ) const;
          bool IsHit( u16 x, u16 y ) const;
          bool IsActive( void ) const;
          void SetActivity( bool activity, bool isAnime=true );
          void SetActivityForce( bool activity, bool isAnime=true );  // 内部状態がactivityフラグ状態であっても、指定したactivityフラグ状態の処理を行う
          void SetActivityAnimeFrameLast( bool activity );  // ボタンを有効/無効を切り替え、アニメは最終フレームにする。
          void SetSelectSE( u32 SEID );
          void SetLongPressFrame( u32 frame );
          void SetBindKey( u32 key, BindKeyMode mode );
          void SetSelectType( SelectType type );
          void SetNotifyTimming( NotifyTimming timming );
          bool RequestRelease( void );
          bool RequestSelect( void );
          void SetButtonMode( ButtonMode mode );
          bool IsAnimation( void ) const;
          void SetInputEnable( bool is_enable );
          bool IsInputEnable( void ) const;
          bool IsLongHolding( void ) const;
          void ResetAnime( void );
          void ResetAnime( u32 anime );

          void SetUseVisibleOfRoot( bool flag ) { m_use_visible_of_root = flag; }

          void Link( Button * button );


#if PM_DEBUG
          void Debug_RegisterBoundingPaneRect( u32 buttonIndex);
#endif


        private:
          enum State {
            STATE_HOLD,
            STATE_RELEASE,
          };
          enum Sequence
          {
            SEQ_INPUT_CHECK,  ///<入力検知（トリガー系）
            SEQ_INPUT_DECIDE, ///<入力検知（押し続け系）あるならば
            SEQ_ANIME_START,  ///<選択、アニメ開始
            SEQ_ANIME_WAIT,   ///<アニメ待ち
          };
          bool UpdateInput_TRIGGER( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input = false );
          bool UpdateMove_TRIGGER( const gfl2::ui::TouchPanel* touchPanel );
          bool UpdateInput_RELEASE( const gfl2::ui::TouchPanel* touchPanel, const gfl2::ui::Button* button, bool is_force_input = false );
          bool UpdateMove_RELEASE_FIX( const gfl2::ui::TouchPanel* touchPanel );
          bool UpdateMove_RELEASE_SCROLL( const gfl2::ui::TouchPanel* touchPanel );

          void StartAnime( u32 anime_index );
          void StopAnime( u32 anime_index );
          bool IsEndAnime( u32 anime_index );
          bool IsInputButton( const gfl2::ui::Button* button ) const;
          bool Hold( void );
          bool Release( void );
          void Select( void );
          void HoldAnime(void);
          void ReleaseAnime( void );
          void SelectAnime( bool is_key_select );
          void ResetLinkAnime(void);
          bool IsSelectAnimeEnd( void ) const;
          bool IsSelectAnimeEnd( u32 anime ) const;
          bool IsLeaveOldDistance( u16 x, u16 y );
          bool IsMovePaneDisitance( void );


          ButtonId m_ID;
          gfl2::lyt::LytWk* m_pLayoutWork;
//          gfl2::lyt::LytPaneIndex m_paneIndex;
//          gfl2::lyt::LytPaneIndex m_boundPaneIndex;
          gfl2::lyt::LytPane* m_paneIndex;
          gfl2::lyt::LytPane* m_boundPaneIndex;
          u32 m_nullAnimeIndex;
          u32 m_holdAnimeIndex;
          u32 m_releaseAnimeIndex;
          u32 m_cancelAnimeIndex;
          u32 m_selectAnimeIndex;
          u32 m_activeAnimeIndex;
          u32 m_passiveAnimeIndex;
          u32 m_selectSE;
          IButtonCallback* m_pCallbackTarget;
          State m_state;
          bool m_isActive;
          u32 m_holdCount;
          u32 m_longPressFrame;  // 長押しと判定するフレーム数
          u32 m_bindKey;  // バインド対象のキー入力
          BindKeyMode m_bindKeyMode;
          SelectType m_selectType;
          NotifyTimming m_notyfiTimming;
          Sequence m_seq;
          bool m_isSelected;
          bool m_isHolding;
          bool m_isCancel;
          bool m_isLongHolding;
          u16 m_old_touch_x;  //holdアニメとReleaseアニメが拡縮する場合のぷるぷる防止用
          u16 m_old_touch_y;
          gfl2::math::VEC3 m_trigger_pane_pos;
          ButtonMode m_button_mode;
          bool m_inputEnable;
          bool m_use_visible_of_root;
          bool m_isKeySelected;
          Button * m_linkButton;    // リンクされたボタン
          bool m_isPlayLinkAnime;   // リンク先からボタンが再生された
      };

      //ボタンプロセスマネージャー
      class IProcessManager
      {
        GFL_FORBID_COPY_AND_ASSIGN(IProcessManager);

        public:
        IProcessManager( Button** buttons, const u32 & button_num ) :
          m_buttons(buttons), m_buttonCount( button_num ) {}
        virtual ~IProcessManager(){}
        virtual void Update(const gfl2::ui::TouchPanel* pUiTouchPanel, const gfl2::ui::Button* pUiButton, bool isInputEnable ) = 0;
        virtual bool IsButtonAnimation( void ) const  = 0;

        virtual ButtonId GetHoldingButtonId( void ) const = 0;
        virtual ButtonId GetSelectedButtonId( void ) const = 0;
        virtual ButtonId GetCancelButtonId( void ) const = 0;
        virtual ButtonId GetLongHoldingButtonId( void ) const = 0;
        virtual bool IsButtonSelected( void ) const = 0;
        virtual bool IsButtonHolding( void ) const = 0;
        virtual bool IsButtonCancel( void ) const = 0;
        virtual bool IsButtonLongHolding( void ) const = 0;

        virtual void StartSelectAct( ButtonId buttonID )  = 0;
        virtual void ResetHoldState( void ) = 0;
        virtual void ResetHoldStateOne( ButtonId buttonID ) = 0;
        protected:
        Button** m_buttons;
        const u32 & m_buttonCount;
      };

      class SingleProcessManager : public IProcessManager
      {
        public:
        SingleProcessManager( Button** buttons, const u32 & button_num );
        virtual ~SingleProcessManager();
        virtual void Update(const gfl2::ui::TouchPanel* pUiTouchPanel, const gfl2::ui::Button* pUiButton, bool isInputEnable );
        virtual bool IsButtonAnimation( void ) const;

        virtual ButtonId GetHoldingButtonId( void ) const;
        virtual ButtonId GetSelectedButtonId( void ) const;
        virtual ButtonId GetCancelButtonId( void ) const;
        virtual ButtonId GetLongHoldingButtonId( void ) const;
        virtual bool IsButtonSelected( void ) const;
        virtual bool IsButtonHolding( void ) const;
        virtual bool IsButtonCancel( void ) const;
        virtual bool IsButtonLongHolding( void ) const;

        virtual void StartSelectAct( ButtonId buttonID );
        virtual void ResetHoldState( void );
        virtual void ResetHoldStateOne( ButtonId buttonID );
        private:
        enum Sequence {
          SEQ_INPUT_WAIT,  // 入力待ち
          SEQ_ANIME_WAIT,  // アニメ終了待ち
        };
        Sequence m_seq;
        u32 m_selectedButtonIndex;
      };
      class MultipleProcessManager : public IProcessManager
      {
        public:
        MultipleProcessManager( Button** buttons, const u32 & button_num );
        virtual ~MultipleProcessManager();
        virtual void Update(const gfl2::ui::TouchPanel* pUiTouchPanel, const gfl2::ui::Button* pUiButton, bool isInputEnable );
        virtual bool IsButtonAnimation( void ) const;

        virtual ButtonId GetHoldingButtonId( void ) const;
        virtual ButtonId GetSelectedButtonId( void ) const;
        virtual ButtonId GetCancelButtonId( void ) const;
        virtual ButtonId GetLongHoldingButtonId( void ) const;
        virtual bool IsButtonSelected( void ) const;
        virtual bool IsButtonHolding( void ) const;
        virtual bool IsButtonCancel( void ) const;
        virtual bool IsButtonLongHolding( void ) const;

        virtual void StartSelectAct( ButtonId buttonID );
        virtual void ResetHoldState( void );
        virtual void ResetHoldStateOne( ButtonId buttonID );
      };

      const gfl2::ui::DeviceManager* m_pUiDeviceManager;
      const gfl2::ui::TouchPanel* m_pUiTouchPanel;
      const gfl2::ui::Button* m_pUiButton;
      Button** m_buttons;
      u32 m_maxButtonCount;
      u32 m_buttonCount;
      u32 m_selectedButtonIndex;
      u32 m_nullAnimeIndex;
      bool m_isLocked;
      bool m_isInputEnable;
      IProcessManager *m_process_manager;
      InputMode m_input_mode;

      void UpdateButtons( void );

      Button * GetButton( ButtonId id );

#if PM_DEBUG
      void Debug_RegisterBoundingPaneRect( void);

      public:
      // デバッグメニューで切り替えます
      static u8 m_isVisibleBoundingPane;        // バウンディングペイン表示
#endif
    };


  } // namespace tool
} // namespace app


#endif // __APPTOOLBUTTONMANAGER_H__
