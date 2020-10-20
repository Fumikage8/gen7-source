//=======================================================================================
/**
 * @file app_util.h
 * @brief アプリケーション用ユーティリティ関数群
 * @author obata_toshihiro
 * @date 2011.09.03
 */
//=======================================================================================
#if PM_DEBUG
#ifndef DEBUGPOKEMAKETEMP_APP_UTIL_H_INCLUDED
#define DEBUGPOKEMAKETEMP_APP_UTIL_H_INCLUDED

#include <heap/include/gfl2_Heap.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSet.h>

#include <arc_def_index/arc_def.h>



namespace debug {
  namespace PokeMakeTemp {

    class LytUtil
    {

      public:

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         *
         * @note 別途, InitLytUtil()を呼ぶ必要があります.
         */
        LytUtil( void );

        /**
         * @brief コンストラクタ
         * @param heap  初期化に使用するヒープ
         *
         * @note InitLytUtil()を呼びます。
         */
        LytUtil( gfl2::heap::HeapBase* heap );

        /**
         * @brief デストラクタ
         *
         * @note FinalizeLytUtil()を呼びます.
         */
        ~LytUtil( void );


        //---------------------------------------------------------------------
        // 前準備・後片付け
        //---------------------------------------------------------------------
        /**
         * @brief 使用準備
         * @param heap  初期化に使用するヒープ
         */
        void InitLytUtil( gfl2::heap::HeapBase* heap );

        /**
         * @brief 後片付け
         */
        void FinalizeLytUtil( void );


        //---------------------------------------------------------------------
        // 共有アニメーション
        //---------------------------------------------------------------------
        /**
         * @brief 共有アニメごとの設定
         *
         * @note
         * share_group_index に gfl2::lyt::LYTSYS_PANEINDEX_ERROR を指定すると
         * 全てのグループを対象とする
         *
         * @note
         * share_pane_index に gfl2::lyt::LYTSYS_PANEINDEX_ERROR を指定すると
         * share_group_index に属する全てのペインを対象とする
         */
        struct ShareAnimeDesc {
          u32 start_anime_index;                          // アニメ先頭インデックス
          u32 max_share_num;                              // 最大共有数
          gfl2::lyt::LytArcIndex anime_data;          // アニメデータインデックス
          gfl2::lyt::LytPaneIndex share_group_index;  // シェア対象のグループ
          gfl2::lyt::LytPaneIndex share_pane_index;   // シェア対象のペイン
        };

        /**
         * @brief 共有アニメを生成する
         * @param layout_work        共有アニメを生成するレイアウトワーク
         * @param share_anime_count  共有アニメの数
         * @param share_anime_table  共有アニメごとの設定テーブル
         * @return 生成したアニメの合計数
         */
        u32 CreateShareAnime(
            gfl2::lyt::LytWk* layout_work,
            u32 share_anime_count,
            const ShareAnimeDesc* share_anime_table );


        //---------------------------------------------------------------------
        // ペインの可視状態を変更する
        //---------------------------------------------------------------------
        /**
         * @brief ペインの可視状態を取得する
         * @retval true   表示中
         * @retval false  非表示
         */
        bool IsPaneVisible(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index ) const;

        /**
         * @brief ペインの可視状態を変更する
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   対象ペインのインデックス
         * @param visibility   trueなら見える, falseなら見えない
         */
        void SetPaneVisibility(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, bool visibility );

        /**
         * @brief ペインを表示する
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   対象ペインのインデックス
         */
        void SetPaneVisible(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index );

        /**
         * @brief ペインを非表示にする
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   対象ペインのインデックス
         */
        void SetPaneInvisible(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index );


        //-----------------------------------------------------------------------------
        // ペインの矩形サイズを取得する
        //-----------------------------------------------------------------------------
        /**
         * @brief ペインの矩形を取得する
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @return ペインの矩形サイズ
         */
        const nw::ut::Rect GetPaneRect( gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const;

        //---------------------------------------------------------------------
        // テキストペインに文字列をセットする
        //---------------------------------------------------------------------
        /**
         * @brief メッセージデータ内の文字列をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param message_data  使用するメッセージデータ
         * @param str_id        セットする文字列のID
         */
        void SetTextboxPaneMessage(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index,
            gfl2::str::MsgData* message_data, u32 str_id );
        void SetTextboxPaneMessage(
            gfl2::lyt::LytTextBox* pane,
            gfl2::str::MsgData* message_data, u32 str_id );

        /**
         * @brief   文字列をセットする ( 展開版 )
         *
         * @memo  WORDSETを使用した場合はこちら。
         */
        void SetTextboxPaneStringExpand(
            gfl2::lyt::LytWk* layout_work,
            gfl2::str::MsgData* message_data,
            print::WordSet* word_set,
            gfl2::lyt::LytPaneIndex pane_index,
            u32 str_id );
        void SetTextboxPaneStringExpand(
            gfl2::str::MsgData* message_data,
            print::WordSet* word_set,
            gfl2::lyt::LytTextBox* pane,
            u32 str_id );

        /**
         * @brief 文字列をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param strbuf        セットする文字列
         */
        void SetTextboxPaneString(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const gfl2::str::StrBuf* strbuf );
        void SetTextboxPaneString(
            gfl2::lyt::LytTextBox* pane, const gfl2::str::StrBuf* strbuf );

        /**
         * @brief 文字列をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param string        セットする文字列
         */
        void SetTextboxPaneString(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const gfl2::str::STRCODE* string );

#if 0
        /**
         * @brief 文字列をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param string        セットする文字列
         */
        void SetTextboxPaneStringMultiByte(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const char* string );
#endif


        /**
         * @brief 数値をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param message_data  使用するメッセージデータ
         * @param word_set      WordSet
         * @param pane_index    対象ペインのインデックス
         * @param str_id        文字列ID
         * @param value         数値
         * @param keta          桁数
         * @param buf_id        バッファID     ※デフォルト = 0
         * @param disp_type     左詰め/右詰め  ※デフォルト = print::NUM_DISP_SPACE
         * @param code_type     全角/半角      ※デフォルト = print::NUM_CODE_HANKAKU
         */
        void SetTextboxPaneNumber(
            gfl2::lyt::LytWk* layout_work,
            gfl2::str::MsgData* message_data,
            print::WordSet* word_set,
            gfl2::lyt::LytPaneIndex pane_index,
            u32 str_id,
            u32 value, u32 keta,
            u32 buf_id = 0,
            print::NumberDispType disp_type = print::NUM_DISP_SPACE,
            print::NumberCodeType code_type = print::NUM_CODE_HANKAKU );
        void SetTextboxPaneNumber(
            gfl2::str::MsgData* message_data,
            print::WordSet* word_set,
            gfl2::lyt::LytTextBox* pane,
            u32 str_id,
            u32 value, u32 keta,
            u32 buf_id = 0,
            print::NumberDispType disp_type = print::NUM_DISP_SPACE,
            print::NumberCodeType code_type = print::NUM_CODE_HANKAKU );

        /**
         * @brief モンスター名をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param monsno        モンスター番号( MONSNO_XXXX )
         */
        void SetTextboxPaneMonsName(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, MonsNo monsno );
        void SetTextboxPaneMonsName(
            gfl2::lyt::LytTextBox* pane, MonsNo monsno );

        /**
         * @brief 特性名をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param tokusei       特性番号
         */
        void SetTextboxPaneTokuseiName(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, TokuseiNo tokusei );

        /**
         * @brief 特性の説明文をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param tokusei       特性番号
         */
        void SetTextboxPaneTokuseiInfo(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, TokuseiNo tokusei );

        /**
         * @brief 技名をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param wazano        技番号
         */
        void SetTextboxPaneWazaName(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, WazaNo wazano );

#if 0
        /**
         * @brief アイテム名をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param itemno        アイテム番号
         * @param heap          一時バッファに使用するヒープ
         */
        void SetTextboxPaneItemName(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, u32 itemno, gfl2::heap::HeapBase* heap );
#endif

        /**
         * @brief アイテムの説明文をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param itemno        アイテム番号
         * @param heap          一時バッファに使用するヒープ
         */
        void SetTextboxPaneItemInfo(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, u32 itemno, gfl2::heap::HeapBase* heap );

#if 0
        //-----------------------------------------------------------------------------
        /**
         * @brief 文字色をセットする
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param topColor      上端の文字色
         * @param bottomColor   上端の文字色
         */
        //-----------------------------------------------------------------------------
        void SetTextboxPaneColor(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const gfl::grp::ColorU8& topColor, const gfl::grp::ColorU8& bottomColor );
#endif

        //---------------------------------------------------------------------
        // アニメーションを制御する
        //---------------------------------------------------------------------
        /**
         * @brief アニメーションを開始する
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  開始するアニメーション番号
         * @param is_auto_unbind　 one time アニメーションの場合、アニメーション完了後、unbindする
         * @param auto_anm_flag    オートアニメーションするかどうか（デフォルト：true）
         * @note
         * オートアニメーションを0フレーム目から開始する
         * IsBindAnime(), SetAutoAnime(true), SetAnimeFrame(0) をまとめた処理
         */
        void StartAnimation(
            gfl2::lyt::LytWk* layout_work, u32 animation_index, bool is_auto_unbind = true, bool auto_anm_flag = true );
        

        /**
         * @brief アニメーションをフレーム指定して開始する
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  開始するアニメーション番号
         * @param frame アニメーションフレーム
         * @param is_auto_unbind　 one time アニメーションの場合、アニメーション完了後、unbindする
         * @param auto_anm_flag    オートアニメーションするかどうか（デフォルト：true）
         * @note
         * オートアニメーションをframeフレーム目から開始する
         * IsBindAnime(), SetAutoAnime(true), SetAnimeFrame(frame) をまとめた処理
         */
        void StartAnimationFrame(
            gfl2::lyt::LytWk* layout_work, u32 animation_index, f32 frame, bool is_auto_unbind = true, bool auto_anm_flag = true );

        /**
         * @brief アニメーションを停止する
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  停止するアニメーション番号
         * @note
         * IsBindAnime(), UnBindAnime(), をまとめた処理
         */
        void StopAnimation(
            gfl2::lyt::LytWk* layout_work, u32 animation_index );

        /**
         * @brief アニメーションが終了したかどうかを調べる
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  対象のアニメーション番号
         * @retval true   終了した( または開始されていない )
         * @retval false  終了していない( 再生中 )
         */
        bool IsAnimationEnd(
            gfl2::lyt::LytWk* layout_work, u32 animation_index ) const;

        /**
         * @brief アニメーションが一時停止中か
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  アニメーション番号
         * @param pause            true  = 一時停止中
         *                         false = 再生中かバインドされていない
         */
        bool IsAnimationPause(
            gfl2::lyt::LytWk* layout_work, u32 animation_index );
        /**
         * @brief アニメーションを一時停止する
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  一時停止するアニメーション番号
         * @param pause            trueなら再開, falseなら停止
         */
        void PauseAnimation(
            gfl2::lyt::LytWk* layout_work, u32 animation_index, bool pause );

        /**
         * @brief アニメーションのフレーム数をセットする
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  制御対象のアニメーション番号
         * @param frame            フレーム数
         */
        void SetAnimationFrame(
            gfl2::lyt::LytWk* layout_work, u32 animation_index, u32 frame );

        /**
         * @brief アニメーションのフレーム数を返す
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  制御対象のアニメーション番号
         * @retval                 フレーム数
         */
        f32 GetAnimationFrame(
            gfl2::lyt::LytWk* layout_work, u32 animation_index );

        /**
         * @brief アニメーションの最大フレーム数を返す
         * @param layout_work      制御対象のレイアウトワーク
         * @param animation_index  制御対象のアニメーション番号
         * @retval                 最大フレーム数
         */
        f32 GetAnimationMaxFrame(
            gfl2::lyt::LytWk* layout_work, u32 animation_index );

        /**
         * @brief バインドする
         *
         * @param layout_work レイアウトワーク
         * @param animation_index アニメーション番号
         */
        void BindAnime( gfl2::lyt::LytWk* layout_work, u32 animation_index, bool is_auto_unbind = true );
        //---------------------------------------------------------------------
        // タッチ判定
        //---------------------------------------------------------------------
        /**
         * @brief ペインがタッチされたかどうかを調べる
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   対象ペインのインデックス
         * @retval true   タッチされた
         * @retval false  タッチされてない
         *
         * @attention 一度描画されるまでは必ず false が返ります.
         */
        bool IsPaneTouchTrigger(
            gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const;

        /**
         * @brief ペインがタッチされているかどうかを調べる
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   対象ペインのインデックス
         * @retval true   タッチされている
         * @retval false  タッチされていない
         *
         * @attention 一度描画されるまでは必ず false が返ります.
         */
        bool IsPaneTouchHold(
            gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const;

        /**
         * @brief タッチされたペインを取得する
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   判定対象のグループ
         *                     ( gfl2::lyt::LYTSYS_PANEINDEX_ERRORなら全てのグループと判定する )
         * @param bound_only   trueなら協会ペインののみ判定対象とする
         * @return タッチされたペインのインデックス
         * @retval gfl2::lyt::LYTSYS_PANEINDEX_ERROR  タッチされたペインがない場合
         *
         * @attention 一度描画されるまでは必ず gfl2::lyt::LYTSYS_PANEINDEX_ERROR が返ります.
         */
        gfl2::lyt::LytPaneIndex GetTouchTriggerPaneIndex(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex group_index = gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
            bool bound_only = false ) const;

        /**
         * @brief タッチされているペインを取得する
         * @param layout_work  ペインが所属するレイアウトワーク
         * @param pane_index   判定対象のグループ
         *                     ( gfl2::lyt::LYTSYS_PANEINDEX_ERRORなら全てのグループと判定する )
         * @param bound_only   trueなら協会ペインののみ判定対象とする
         * @return タッチされているペインのインデックス
         * @retval gfl2::lyt::LYTSYS_PANEINDEX_ERROR  タッチされているペインがない場合
         *
         * @attention 一度描画されるまでは必ず gfl2::lyt::LYTSYS_PANEINDEX_ERROR が返ります.
         */
        gfl2::lyt::LytPaneIndex GetTouchHoldPaneIndex(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex group_index = gfl2::lyt::LYTSYS_PANEINDEX_ERROR,
            bool bound_only = false ) const;


        //---------------------------------------------------------------------
        // 座標・サイズ
        //---------------------------------------------------------------------
        /**
         * @brief ペインのローカル座標を取得する
         * @param[in]  layout_work   ペインが所属するレイアウトワーク
         * @param[in]  pane_index    対象ペインのインデックス
         * @param[out] dest          座標の格納先
         *
         * @note 親ペインからのオフセット座標を返します.
         */
        void GetLocalPos(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const;

        /**
         * @brief ペインのローカル座標を変更する
         * @param[in] layout_work   ペインが所属するレイアウトワーク
         * @param[in] pane_index    対象ペインのインデックス
         * @param[in] pos           設定する座標
         *
         * @note 親ペインからのオフセット座標を変更します.
         */
        void SetLocalPos(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const gfl2::math::VEC3& pos );

        /**
         * @brief ペインの親子関係を解決した後の, 最終的な座標を取得する
         * @param[in]  layout_work   ペインが所属するレイアウトワーク
         * @param[in]  pane_index    対象ペインのインデックス
         * @param[out] dest          座標の格納先
         */
        void GetGlobalPos(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const;

        /**
         * @brief ペインの幅と高さを取得する
         * @param[in]  layout_work   ペインが所属するレイアウトワーク
         * @param[in]  pane_index    対象ペインのインデックス
         * @param[out] width          幅の格納先
         * @param[out] height         高さの格納先
         */
        void GetPaneSize(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, f32* width, f32* height ) const;

        /**
         * @brief ペインの回転を取得する
         * @param[in]  layout_work   ペインが所属するレイアウトワーク
         * @param[in]  pane_index    対象ペインのインデックス
         * @param[out] dest          回転の格納先
         *
         * @note 親ペインからのオフセット回転を返します.
         */
        void GetRotate(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const;

        /**
         * @brief ペインの回転を変更する
         * @param[in] layout_work   ペインが所属するレイアウトワーク
         * @param[in] pane_index    対象ペインのインデックス
         * @param[in] rotate        設定する回転
         *
         * @note 親ペインからのオフセット回転を変更します.
         */
        void SetRotate(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, const gfl2::math::VEC3& rotate );

        //-----------------------------------------------------------------------------
        /**
         * @brief ピクチャーペインの頂点カラー成分を設定する
         *
         * @param[in]  layout_work   ペインが所属するレイアウトワーク
         * @param[in]  pane_index    対象ペインのインデックス
         * @param[in]  color         設定カラー値
         */
        //-----------------------------------------------------------------------------
        void SetVtxColorElementOfPicturePane( gfl2::lyt::LytWk* layout_work,
                                              gfl2::lyt::LytPaneIndex pane_index,
                                              const nw::ut::Color8 color );

        //-----------------------------------------------------------------------------
        /**
         * @brief ペインのアルファ値を設定する
         * @param[in]  alpha  設定するアルファ値
         */
        //-----------------------------------------------------------------------------
        void SetAlpha(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, u8 alpha );

        //-----------------------------------------------------------------------------
        /**
         * @brief ペインのアルファ値を取得する
         * return ペインのアルファ値
         */
        //-----------------------------------------------------------------------------
        u8 GetAlpha(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index ) const;

        //-----------------------------------------------------------------------------
        /**
         * @brief タグプロセッサーの登録
         * @param layout_work   ペインが所属するレイアウトワーク
         * @param pane_index    対象ペインのインデックス
         * @param tagProc       タグプロセッサへのポインタ
         */
        //-----------------------------------------------------------------------------
        void SetTextboxPaneTagProcessor(
            gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index, nw::lyt::TextBox::TagProcessor* tagProc );

#if 0
        //-----------------------------------------------------------------------------
        /**
         * @brief ペインのアニメーションリストを取得する
         * @param[in] layout_work   ペインが所属するレイアウトワーク
         * @param[in] pane_index    対象ペインのインデックス
         *
         * @return 割り当てられているアニメーションリスト
         */
        //-----------------------------------------------------------------------------
        const nw::lyt::AnimationList& GetAnimationList( gfl2::lyt::LytWk* layout_work,
            gfl2::lyt::LytPaneIndex pane_index ) const;
#endif

        //---------------------------------------------------------------------
        // 共通データの取得
        //---------------------------------------------------------------------
        /**
         * @brief 一時文字列バッファを取得する
         * @param idx  バッファのインデックス[0, TEMP_STRBUF_NUM]
         *
         * @attention
         * SetTextboxPaneXXXX()内で使用しているため,
         * 関数を呼び出すとバッファの内容が上書きされます。
         */
        gfl2::str::StrBuf* GetTempStrBuf( u32 idx = 0 ) const;





        static const u32 TEMP_STRBUF_NUM = 2;
        static const u32 TEMP_STRBUF_LENGTH = 512;
        gfl2::str::StrBuf* m_temp_strbuf[ TEMP_STRBUF_NUM ];

        void InitTempStrbuf( void );
    };


  }  // namespace PokeMakeTemp
}  // namespace debug


#endif // DEBUGPOKEMAKETEMP_APP_UTIL_H_INCLUDED
#endif // PM_DEBUG
