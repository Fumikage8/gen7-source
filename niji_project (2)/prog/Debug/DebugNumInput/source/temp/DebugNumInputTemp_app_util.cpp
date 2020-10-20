//=======================================================================================
/**
 * @file app_util.cpp
 * @brief アプリケーション用ユーティリティ関数群
 * @author obata_toshihiro
 * @date 2011.09.03
 *
 *
 *
 *
 * 注意！
 *
 * このコードは、
 * Sango から移植した DebugNumInput クラスを動かすためだけに存在しています。
 * 他のいかなる用途にも使用しないでください。
 *
 * また、
 * 新規実装する上で参考にしたりしないでください。
 *
 *
 */
//=======================================================================================
#if PM_DEBUG
#include <util/include/gfl2_std_string.h>
#include <GameSys/include/GameManager.h>
#include <Print/include/PrintSystem.h>
#include <Print/include/WordSet.h>

#include "./DebugNumInputTemp_app_util.h"


namespace debug {
  namespace NumInputTemp {



    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @note 別途, InitLytUtil()を呼ぶ必要があります.
     */
    //-------------------------------------------------------------------------
    LytUtil::LytUtil( void )
    {
      this->InitTempStrbuf();
    }

    /**
     * @brief 文字列のテンポラリバッファを初期化する
     */
    void LytUtil::InitTempStrbuf( void )
    {
      for( u8 i=0; i<TEMP_STRBUF_NUM; i++ ) {
        m_temp_strbuf[i] = NULL;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap  初期化に使用するヒープ
     *
     * @note InitLytUtil()を呼びます。
     */
    //-------------------------------------------------------------------------
    LytUtil::LytUtil( gfl2::heap::HeapBase* heap )
    {
      this->InitTempStrbuf();
      this->InitLytUtil( heap );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     *
     * @note FinalizeLytUtil()を呼びます.
     */
    //-------------------------------------------------------------------------
    LytUtil::~LytUtil( void )
    {
      this->FinalizeLytUtil();
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 使用準備
     * @param heap  インスタンスの生成に使用するヒープ
     */
    //-------------------------------------------------------------------------
    void LytUtil::InitLytUtil( gfl2::heap::HeapBase* heap )
    {
      for( u32 i=0; i<TEMP_STRBUF_NUM; i++ ) {
        if( m_temp_strbuf[i] == NULL ) {
          m_temp_strbuf[i] = GFL_NEW( heap )
            gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, heap );
        }
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 後片付け
     */
    //-------------------------------------------------------------------------
    void LytUtil::FinalizeLytUtil( void )
    {
      for( u32 i=0; i<TEMP_STRBUF_NUM; i++ ) {
        if( m_temp_strbuf[i] ) {
          GFL_DELETE m_temp_strbuf[i];
          m_temp_strbuf[i] = NULL;
        }
      }
    }





    //-----------------------------------------------------------------------------
    /**
     * @brief 一時バッファを取得する
     * @param idx  バッファのインデックス[0, TEMP_STRBUF_NUM]
     */
    //-----------------------------------------------------------------------------
    gfl2::str::StrBuf* LytUtil::GetTempStrBuf( u32 idx ) const
    {
      if( idx < TEMP_STRBUF_NUM )
      {
        GFL_ASSERT_MSG(
            m_temp_strbuf[ idx ],
            "LytUtil::InitLytUtil()を呼んでください.\n" );
        return m_temp_strbuf[ idx ];
      }
      else
      {
        GFL_ASSERT_MSG( 0, "インデックスエラー\n" );
        return m_temp_strbuf[0];
      }
    }








    //-----------------------------------------------------------------------------------
    /**
     * @brief 共有アニメを生成する
     * @param layout_work        共有アニメを生成するレイアウトワーク
     * @param share_anime_count  共有アニメの数
     * @param share_anime_table  共有アニメごとの設定テーブル
     * @return 生成したアニメの合計数
     */
    //-----------------------------------------------------------------------------------
    u32 LytUtil::CreateShareAnime(
        gfl2::lyt::LytWk* layout_work,
        u32 share_anime_count,
        const ShareAnimeDesc* share_anime_table )
    {
#if 0
      u32 total_count = 0;

      for( u32 i=0; i<share_anime_count; i++ )
      {
        u32 count =
          layout_work->CreateShareAnime(
              share_anime_table[i].start_anime_index,
              share_anime_table[i].anime_data,
              share_anime_table[i].max_share_num,
              share_anime_table[i].share_group_index,
              share_anime_table[i].share_pane_index );
        total_count += count;
#if PM_DEBUG
        if( count == 0 ) {
					GFL_PRINT( "[%d]: WARNING!! シェアアニメ失敗\n", i );
        }
#endif
      }
      return total_count;
#else
      return 0;
#endif
    }

    //-----------------------------------------------------------------------------
    /**
    * @brief ペインの矩形を取得する
    * @param layout_work   ペインが所属するレイアウトワーク
    * @param pane_index    対象ペインのインデックス
    * @return ペインの矩形サイズ
    */
    //-----------------------------------------------------------------------------
    const nw::ut::Rect LytUtil::GetPaneRect( gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const
    {
      gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      GFL_ASSERT( pane );
      return pane->GetPaneRect();
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの可視状態を取得する
     * @retval true   表示中
     * @retval false  非表示
     */
    //-----------------------------------------------------------------------------
    bool LytUtil::IsPaneVisible(
        gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const
    {
      gfl2::lyt::LytPane* pane;
      pane = layout_work->GetPane( pane_index );
      GFL_ASSERT( pane );
      if( pane ) {
        return pane->IsVisible();
      }
      return false;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの可視状態を変更する
     * @param layout_work  ペインが所属するレイアウトワーク
     * @param pane_index   対象ペインのインデックス
     * @param visibility   trueなら見える, falseなら見えない
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetPaneVisibility(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, bool visibility )
    {
      gfl2::lyt::LytPane* pane;
      pane = layout_work->GetPane( pane_index );
      GFL_ASSERT( pane );
      if( pane ) {
        pane->SetVisible( visibility );
      }
    }


    //-----------------------------------------------------------------------------
    /**
     * @brief ペインを表示する
     * @param layout_work  ペインが所属するレイアウトワーク
     * @param pane_index   対象ペインのインデックス
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetPaneVisible(
        gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index )
    {
      SetPaneVisibility( layout_work, pane_index, true );
    }


    //-----------------------------------------------------------------------------
    /**
     * @brief ペインを非表示にする
     * @param layout_work  ペインが所属するレイアウトワーク
     * @param pane_index   対象ペインのインデックス
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetPaneInvisible(
        gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index )
    {
      SetPaneVisibility( layout_work, pane_index, false );
    }





    //-----------------------------------------------------------------------------
    /**
     * @brief タグプロセッサーの登録
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param tagProc       タグプロセッサへのポインタ
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneTagProcessor(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, nw::lyt::TextBox::TagProcessor* tagProc )
    {
      gfl2::lyt::LytTextBox* text_box;
      text_box = layout_work->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );
      if( text_box ) {
        text_box->SetTagProcessor( tagProc );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief メッセージデータ内の文字列をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param message_data  使用するメッセージデータ
     * @param str_id        セットする文字列のID
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneMessage(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index,
        gfl2::str::MsgData* message_data, u32 str_id )
    {
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      message_data->GetString( str_id, *strbuf );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
    }
    void LytUtil::SetTextboxPaneMessage(
        gfl2::lyt::LytTextBox* pane,
        gfl2::str::MsgData* message_data, u32 str_id )
    {
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      message_data->GetString( str_id, *strbuf );
      this->SetTextboxPaneString( pane, strbuf );
    }

    //--------------------------------------------------------------------------------------------
    /**
     * @brief   文字列をセットする ( 展開版 )
     *
     * @memo  WORDSETを使用した場合はこちら。
     */
    //--------------------------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneStringExpand(
        gfl2::lyt::LytWk* layout_work,
        gfl2::str::MsgData* message_data,
        print::WordSet* word_set,
        gfl2::lyt::LytPaneIndex pane_index,
        u32 str_id )
    {
      gfl2::str::StrBuf* tmp = this->GetTempStrBuf(0);
      gfl2::str::StrBuf* exp = this->GetTempStrBuf(1);
      message_data->GetString( str_id, *tmp );
      word_set->Expand( exp, tmp );
      this->SetTextboxPaneString( layout_work, pane_index, exp );
    }
    void LytUtil::SetTextboxPaneStringExpand(
        gfl2::str::MsgData* message_data,
        print::WordSet* word_set,
        gfl2::lyt::LytTextBox* pane,
        u32 str_id )
    {
      gfl2::str::StrBuf* tmp = this->GetTempStrBuf(0);
      gfl2::str::StrBuf* exp = this->GetTempStrBuf(1);
      message_data->GetString( str_id, *tmp );
      word_set->Expand( exp, tmp );
      this->SetTextboxPaneString( pane, exp );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 文字列をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param strbuf        セットする文字列
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneString(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const gfl2::str::StrBuf* strbuf )
    {
      gfl2::lyt::LytTextBox* text_box = layout_work->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );

      if( text_box ) {
        print::SetStringForLytTextBox( text_box, strbuf );
      }
    }
    void LytUtil::SetTextboxPaneString(
        gfl2::lyt::LytTextBox* pane, const gfl2::str::StrBuf* strbuf )
    {
      print::SetStringForLytTextBox( pane, strbuf );
    }

#if 0
    //-----------------------------------------------------------------------------
    /**
     * @brief 文字色をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param topColor      上端の文字色（グラデーション可能）
     * @param bottomColor   下端の文字色（グラデーション可能）
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneColor(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const gfl::grp::ColorU8& topColor, const gfl::grp::ColorU8& bottomColor )
    {
      gfl2::lyt::LytTextBox* text_box;
      text_box = layout_work->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );
      if( text_box ) {
        text_box->SetTextColor( topColor, bottomColor );
      }
    }
#endif

    //-----------------------------------------------------------------------------
    /**
     * @brief 文字列をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param string        セットする文字列
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneString(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const gfl2::str::STRCODE* string )
    {
      gfl2::lyt::LytTextBox* text_box;
      text_box = layout_work->GetTextBoxPane( pane_index );
      GFL_ASSERT( text_box );
      if( text_box ) {
        print::SetStringForLytTextBox( text_box, string );
      }
    }

#if 0
    //-----------------------------------------------------------------------------------
    /**
     * @brief 文字列をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param string        セットする文字列
     */
    //-----------------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneStringMultiByte(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const char* string )
    {
      // char >> wchar_t 変換
      size_t length = strlen( string );
      wchar_t wc[ 256 + 1 ];
      if (length > 256 ) length = 256;
      gfl2::std::MemClear( wc, sizeof( wchar_t ) * ( length + 1 ) );
      length = mbstowcs( wc, string, length+1 ); // wc にワイド文字列が入る
      this->SetTextboxPaneString( layout_work, pane_index, wc );
    }
#endif

    //-----------------------------------------------------------------------------
    /**
     * @brief 数値をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param message_data  使用するメッセージデータ
     * @param word_set      WordSet
     * @param pane_index    対象ペインのインデックス
     * @param str_id        文字列ID
     * @param value         数値
     * @param keta          桁数
     * @param buf_id        バッファID
     * @param disp_type     左詰め/右詰め
     * @param code_type     全角/半角
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneNumber(
        gfl2::lyt::LytWk* layout_work,
        gfl2::str::MsgData* message_data,
        print::WordSet* word_set,
        gfl2::lyt::LytPaneIndex pane_index,
				u32 str_id,
				u32 value, u32 keta,
				u32 buf_id,
        print::NumberDispType disp_type, print::NumberCodeType code_type )
		{
			word_set->RegisterNumber( buf_id, value, keta, disp_type, code_type );
			this->SetTextboxPaneStringExpand( layout_work, message_data, word_set, pane_index, str_id );
		}
    void LytUtil::SetTextboxPaneNumber(
        gfl2::str::MsgData* message_data,
        print::WordSet* word_set,
        gfl2::lyt::LytTextBox* pane,
        u32 str_id,
        u32 value, u32 keta,
        u32 buf_id,
        print::NumberDispType disp_type,
        print::NumberCodeType code_type )
    {
			word_set->RegisterNumber( buf_id, value, keta, disp_type, code_type );
			this->SetTextboxPaneStringExpand( message_data, word_set, pane, str_id );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 技名をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param wazano        技番号
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneWazaName(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, WazaNo wazano )
    {
#if 0
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      pml::wazadata::GetWazaName( wazano, strbuf );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
#endif
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief モンスター名をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param monsno        モンスター番号( MONSNO_XXXX )
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneMonsName(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, MonsNo monsno )
    {
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      pml::personal::GetMonsName( strbuf, monsno );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
    }
    void LytUtil::SetTextboxPaneMonsName(
        gfl2::lyt::LytTextBox* pane, MonsNo monsno )
    {
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      pml::personal::GetMonsName( strbuf, monsno );
      this->SetTextboxPaneString( pane, strbuf );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 特性名をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param tokusei       特性番号
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneTokuseiName(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, TokuseiNo tokusei )
    {
#if 0
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      pml::personal::GetTokuseiName( strbuf, tokusei );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
#endif
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief 特性の説明文をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param tokusei       特性番号
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneTokuseiInfo(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, TokuseiNo tokusei )
    {
#if 0
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      pml::personal::GetTokuseiInfo( strbuf, tokusei );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
#endif
    }

#if 0
    //-----------------------------------------------------------------------------
    /**
     * @brief アイテム名をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param itemno        アイテム番号
     * @param heap          一時バッファに使用するヒープ
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneItemName(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, u32 itemno, gfl2::heap::HeapBase* heap )
    {
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      gfl2::heap::HeapBase* lower_heap = heap->GetLowerHandle();
      item::ITEM_DATA item_data( itemno, heap );
      item_data.ITEM_GetItemName( strbuf, lower_heap );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
    }
#endif

    //-----------------------------------------------------------------------------
    /**
     * @brief アイテムの説明文をセットする
     * @param layout_work   ペインが所属するレイアウトワーク
     * @param pane_index    対象ペインのインデックス
     * @param itemno        アイテム番号
     * @param heap          一時バッファに使用するヒープ
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetTextboxPaneItemInfo(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, u32 itemno, gfl2::heap::HeapBase* heap )
    {
#if 0
      gfl2::str::StrBuf* strbuf = this->GetTempStrBuf(0);
      gfl2::heap::HeapBase* lower_heap = heap->GetLowerHandle();
      item::ITEM_DATA item_data( itemno, heap );
      item_data.ITEM_GetInfo( strbuf, lower_heap );
      this->SetTextboxPaneString( layout_work, pane_index, strbuf );
#endif
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションを開始する
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  開始するアニメーション番号
     * @param is_auto_unbind　 one time アニメーションの場合、アニメーション完了後、unbindする
     * @param auto_anm_flag    オートアニメーションするかどうか（デフォルト：true）
     */
    //-----------------------------------------------------------------------------
    void LytUtil::StartAnimation( gfl2::lyt::LytWk* layout_work, u32 animation_index, bool is_auto_unbind, bool auto_anm_flag )
    {
      this->StartAnimationFrame( layout_work, animation_index, 0, is_auto_unbind, auto_anm_flag );
    }
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
    void LytUtil::StartAnimationFrame(
        gfl2::lyt::LytWk* layout_work, u32 animation_index, f32 frame, bool is_auto_unbind, bool auto_anm_flag )
    {
      if( layout_work->IsBindAnime( animation_index ) == false ) {
        layout_work->BindAnime( animation_index, is_auto_unbind );
      }
      layout_work->SetAutoAnime( animation_index, auto_anm_flag );
      layout_work->SetAnimeFrame( animation_index, frame );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションを停止する
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  停止するアニメーション番号
     */
    //-----------------------------------------------------------------------------
    void LytUtil::StopAnimation( gfl2::lyt::LytWk* layout_work, u32 animation_index )
    {
      if( layout_work->IsBindAnime( animation_index ) ) {
        layout_work->UnBindAnime( animation_index );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションが終了したかどうかを調べる
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  対象のアニメーション番号
     * @retval true   終了した( または開始されていない )
     * @retval false  終了していない( 再生中 )
     */
    //-----------------------------------------------------------------------------
    bool LytUtil::IsAnimationEnd(
        gfl2::lyt::LytWk* layout_work, u32 animation_index ) const
    {
      // return !( 終了していない条件 )
      return !(
          layout_work->IsAnime( animation_index ) &&
          layout_work->IsBindAnime( animation_index ) &&
          !layout_work->IsAnimeEnd( animation_index ) );
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションが一時停止中か
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  アニメーション番号
     * @param pause            true  = 一時停止中
     *                         false = 再生中かバインドされていない
     */
    //-----------------------------------------------------------------------------
    bool LytUtil::IsAnimationPause(
        gfl2::lyt::LytWk* layout_work, u32 animation_index )
    {
      bool ret = false;
      if( layout_work->IsBindAnime( animation_index ) ) {
        ret = !( layout_work->IsAutoAnime( animation_index ) );
      }
      return ret;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションを一時停止する
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  一時停止するアニメーション番号
     * @param pause            trueなら再開, falseなら停止
     */
    //-----------------------------------------------------------------------------
    void LytUtil::PauseAnimation(
        gfl2::lyt::LytWk* layout_work, u32 animation_index, bool pause )
    {
      if( layout_work->IsBindAnime( animation_index ) ) {
        layout_work->SetAutoAnime( animation_index, pause );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションのフレーム数をセットする
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  制御対象のアニメーション番号
     * @param frame            フレーム数
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetAnimationFrame(
        gfl2::lyt::LytWk* layout_work, u32 animation_index, u32 frame )
    {
      if( layout_work->IsBindAnime( animation_index ) ) {
        layout_work->SetAnimeFrame( animation_index, frame );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションのフレーム数を返す
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  制御対象のアニメーション番号
     * @retval                 フレーム数
     */
    //-----------------------------------------------------------------------------
    f32 LytUtil::GetAnimationFrame(
        gfl2::lyt::LytWk* layout_work, u32 animation_index )
    {
      if( layout_work->IsBindAnime( animation_index ) ) {
        return layout_work->GetAnimeFrame( animation_index );
      }
      return 0.0f;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief アニメーションの最大フレーム数を返す
     * @param layout_work      制御対象のレイアウトワーク
     * @param animation_index  制御対象のアニメーション番号
     * @retval                 最大フレーム数
     */
    //-----------------------------------------------------------------------------
    f32 LytUtil::GetAnimationMaxFrame(
        gfl2::lyt::LytWk* layout_work, u32 animation_index )
    {
      if( layout_work->IsBindAnime( animation_index ) ) {
        return layout_work->GetAnimeMaxFrame( animation_index );
      }
      return 0.0f;
		}


    
    /**
     * @brief バインドする
     *
     * @param layout_work レイアウトワーク
     * @param animation_index アニメーション番号
     */
    void LytUtil::BindAnime( gfl2::lyt::LytWk* layout_work, u32 animation_index, bool is_auto_unbind )
    {
      if( layout_work->IsBindAnime( animation_index ) == false ) {
        layout_work->BindAnime( animation_index, is_auto_unbind );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインがタッチされたかどうかを調べる
     * @param layout_work  ペインが所属するレイアウトワーク
     * @param pane_index   対象ペインのインデックス
     * @retval true   タッチされた
     * @retval false  タッチされてない
     *
     * @attention 一度描画されるまでは必ず false が返ります.
     */
    //-----------------------------------------------------------------------------
    bool LytUtil::IsPaneTouchTrigger(
        gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const
    {
      if( !layout_work->IsHitCheckOK() ) {  // 一度描画しないとチェック不可
        return false;
      }

      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      gfl2::ui::DeviceManager* device_manager = game_manager->GetUiDeviceManager();
      gfl2::ui::TouchPanel* touch_panel =
        device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

      if( touch_panel->IsTouchTrigger() ) {
        u16 touch_x = touch_panel->GetX();
        u16 touch_y = touch_panel->GetY();
        return layout_work->GetHitPane( touch_x, touch_y, pane_index );
      }
      return false;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインがタッチされているかどうかを調べる
     * @param layout_work  ペインが所属するレイアウトワーク
     * @param pane_index   対象ペインのインデックス
     * @retval true   タッチされている
     * @retval false  タッチされていない
     *
     * @attention 一度描画されるまでは必ず false が返ります.
     */
    //-----------------------------------------------------------------------------
    bool LytUtil::IsPaneTouchHold(
        gfl2::lyt::LytWk* layout_work, gfl2::lyt::LytPaneIndex pane_index ) const
    {
      if( !layout_work->IsHitCheckOK() ) {  // 一度描画しないとチェック不可
        return false;
      }

      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      gfl2::ui::DeviceManager* device_manager = game_manager->GetUiDeviceManager();
      gfl2::ui::TouchPanel* touch_panel =
        device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

      if( touch_panel->IsTouch() ) {
        u16 touch_x = touch_panel->GetX();
        u16 touch_y = touch_panel->GetY();
        return layout_work->GetHitPane( touch_x, touch_y, pane_index );
      }
      return false;
    }

    //-----------------------------------------------------------------------------
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
    //-----------------------------------------------------------------------------
    gfl2::lyt::LytPaneIndex LytUtil::GetTouchTriggerPaneIndex(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex group_index, bool bound_only ) const
    {
      if( !layout_work->IsHitCheckOK() ) {  // 一度描画しないとチェック不可
        return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
      }

      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      gfl2::ui::DeviceManager* device_manager = game_manager->GetUiDeviceManager();
      gfl2::ui::TouchPanel* touch_panel =
        device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

      if( touch_panel->IsTouchTrigger() ) {
        u16 touch_x = touch_panel->GetX();
        u16 touch_y = touch_panel->GetY();
        if( group_index == gfl2::lyt::LYTSYS_PANEINDEX_ERROR )
        {
          return layout_work->GetHitPaneIndexAll( touch_x, touch_y, bound_only );
        }
        else
        {
          return layout_work->GetHitPaneIndexGroup( group_index, touch_x, touch_y, bound_only );
        }
      }
      return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
    }

    //-----------------------------------------------------------------------------
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
    //-----------------------------------------------------------------------------
    gfl2::lyt::LytPaneIndex LytUtil::GetTouchHoldPaneIndex(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex group_index, bool bound_only ) const
    {
      if( !layout_work->IsHitCheckOK() ) {  // 一度描画しないとチェック不可
        return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
      }

      GameSys::GameManager* game_manager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
      gfl2::ui::DeviceManager* device_manager = game_manager->GetUiDeviceManager();
      gfl2::ui::TouchPanel* touch_panel =
        device_manager->GetTouchPanel( gfl2::ui::DeviceManager::TOUCHPANEL_STANDARD );

      if( touch_panel->IsTouch() ) {
        u16 touch_x = touch_panel->GetX();
        u16 touch_y = touch_panel->GetY();
        if( group_index == gfl2::lyt::LYTSYS_PANEINDEX_ERROR )
        {
          return layout_work->GetHitPaneIndexAll( touch_x, touch_y, bound_only );
        }
        else
        {
          return layout_work->GetHitPaneIndexGroup( group_index, touch_x, touch_y, bound_only );
        }
      }
      return gfl2::lyt::LYTSYS_PANEINDEX_ERROR;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ピクチャーペインの頂点カラー成分を設定する
     *
     * @param[in]  layout_work   ペインが所属するレイアウトワーク
     * @param[in]  pane_index    対象ペインのインデックス
     * @param[in]  color         設定カラー値
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetVtxColorElementOfPicturePane( gfl2::lyt::LytWk* layout_work,
                                                   gfl2::lyt::LytPaneIndex pane_index,
                                                   const nw::ut::Color8 color )
    {
      gfl2::lyt::LytPicture* pane = layout_work->GetPicturePane( pane_index );

      if( pane )
      {
        pane->SetVtxColor( nw::lyt::VERTEXCOLOR_LT, color );
        pane->SetVtxColor( nw::lyt::VERTEXCOLOR_RT, color );
        pane->SetVtxColor( nw::lyt::VERTEXCOLOR_LB, color );
        pane->SetVtxColor( nw::lyt::VERTEXCOLOR_RB, color );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインのローカル座標を取得する
     * @param[in]  layout_work   ペインが所属するレイアウトワーク
     * @param[in]  pane_index    対象ペインのインデックス
     * @param[out] dest          座標の格納先
     *
     * @note 親ペインからのオフセット座標を返します.
     */
    //-----------------------------------------------------------------------------
    void LytUtil::GetLocalPos(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const
    {
      const gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane ) {
        *dest = pane->GetTranslate();
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインのローカル座標を変更する
     * @param[in] layout_work   ペインが所属するレイアウトワーク
     * @param[in] pane_index    対象ペインのインデックス
     * @param[in] pos           設定する座標
     *
     * @note 親ペインからのオフセット座標を変更します.
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetLocalPos(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const gfl2::math::VEC3& pos )
    {
      gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane ) {
        pane->SetTranslate( pos );
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの親子関係を解決した後の, 最終的な座標を取得する
     * @param[in]  layout_work   ペインが所属するレイアウトワーク
     * @param[in]  pane_index    対象ペインのインデックス
     * @param[out] dest          座標の格納先
     */
    //-----------------------------------------------------------------------------
    void LytUtil::GetGlobalPos(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const
    {
      const gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane ) {
        const gfl2::math::MTX34& global_matrix = pane->GetGlobalMtx();
        dest->x = global_matrix._03;
        dest->y = global_matrix._13;
        dest->z = global_matrix._23;
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインのアルファ値を設定する
     * @param[in]  alpha  設定するアルファ値
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetAlpha(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, u8 alpha )
    {
      gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );

      if( pane ){
         pane->SetAlpha( alpha );
      }
    }


    //-----------------------------------------------------------------------------
    /**
     * @brief ペインのアルファ値を取得する
     * return ペインのアルファ値
     */
    //-----------------------------------------------------------------------------
    u8 LytUtil::GetAlpha(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index ) const
    {
      const gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );

      if( pane ){
         return pane->GetAlpha();
      }
      return 0;
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの幅と高さを取得する
     * @param[in]  layout_work   ペインが所属するレイアウトワーク
     * @param[in]  pane_index    対象ペインのインデックス
     * @param[out] width          幅の格納先
     * @param[out] height         高さの格納先
     */
    //-----------------------------------------------------------------------------
    void LytUtil::GetPaneSize(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, f32* width, f32* height )  const
    {
      const gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane )
      {
        const nw::lyt::Size& size = pane->GetSize();
        *width = size.width;
        *height = size.height;
      }
      else
      {
        *width = 0;
        *height = 0;
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの回転を取得する
     * @param[in]  layout_work   ペインが所属するレイアウトワーク
     * @param[in]  pane_index    対象ペインのインデックス
     * @param[out] dest          回転の格納先
     *
     * @note 親ペインからのオフセット回転を返します.
     */
    //-----------------------------------------------------------------------------
    void LytUtil::GetRotate(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, gfl2::math::VEC3* dest ) const
    {
      const gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane ) {
        *dest = pane->GetRotate();
      }
    }

    //-----------------------------------------------------------------------------
    /**
     * @brief ペインの回転を変更する
     * @param[in] layout_work   ペインが所属するレイアウトワーク
     * @param[in] pane_index    対象ペインのインデックス
     * @param[in] rotate        設定する回転
     *
     * @note 親ペインからのオフセット回転を変更します.
     */
    //-----------------------------------------------------------------------------
    void LytUtil::SetRotate(
        gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index, const gfl2::math::VEC3& rotate )
    {
      gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      if( pane ) {
        pane->SetRotate( rotate );
      }
    }

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
    const nw::lyt::AnimationList& LytUtil::GetAnimationList( gfl2::lyt::LytWk* layout_work,
        gfl2::lyt::LytPaneIndex pane_index ) const
    {
      gfl2::lyt::LytPane* pane = layout_work->GetPane( pane_index );
      GFL_ASSERT( pane );

      return pane->GetAnimationList();
    }
#endif


  }  // namespace NumInputTemp
}  // namespace debug

#endif // PM_DEBUG