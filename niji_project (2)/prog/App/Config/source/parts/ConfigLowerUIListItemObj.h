//=============================================================================
/**
 * @file ConfigLowerUIListItemObj.h
 * @brief コンフィグの下UIリスト項目
 * @author yuto_uchida
 * @date 2015.05.19
 */
//=============================================================================
#if !defined( NIJI_PROJECT_CONFIG_LOWER_UI_LIST_ITEM_OBJ_H_INCLUDED )
#define NIJI_PROJECT_CONFIG_LOWER_UI_LIST_ITEM_OBJ_H_INCLUDED
#pragma once

// gfl2のインクルード
#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

//  nijiのインクルード
#include "AppLib/include/Ui/UIResponder.h"

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( config )

  /** コンフィグ下UIリスト項目オブジェクト */
  class ConfigLowerUIListItemObj
  {
    GFL_FORBID_COPY_AND_ASSIGN( ConfigLowerUIListItemObj );  // コピーを禁止

    public:
      enum
      {
        SELECT_MAX  = 6,
      };

      enum PushAnimeType
      {
        PUSH_ANIME_TYPE_LEFT  = 0,
        PUSH_ANIME_TYPE_RIGHT
      };

      //  初期化パラメータ(内部でコピーするのでポインタなどがあると注意)
      typedef struct
      {
        //  項目のパーツペインid
        u32 root_parts_pane_id;

        //  アクティブアニメid
        u32 active_arrow_left_anime_id;
        u32 active_arrow_right_anime_id;

        //  パッシブアニメid
        u32 passive_arrow_left_anime_id;
        u32 passive_arrow_right_anime_id;

        //  項目ボタンデータ
        app::ui::ButtonInfoEx item_button_data;

        //  左矢印ボタンデータ
        app::ui::ButtonInfoEx left_arrow_button_data;

        //  右矢印ボタンデータ
        app::ui::ButtonInfoEx right_arrow_button_data;

      } INIT_PARAM;

      //  項目データパラメータ
      typedef struct
      {
        //  項目のテキストid
        u32 title_text_id;

        //  選択idx
        u32 select_idx;

        //  選択数
        u32 select_num;

        //  各選択のテキストid
        u32 a_select_text_id[ SELECT_MAX ];

      } DATA_PARAM;

      /**
      * @brief コンストラクタ
      */
      ConfigLowerUIListItemObj();

      /**
      * @brief デストラクタ
      */
      virtual ~ConfigLowerUIListItemObj();

      //------------------------------------------------------------------
      /**
       * @brief   初期化
       */
      //------------------------------------------------------------------
      void Initialize( const INIT_PARAM& rInitParma, app::ui::UIView* pRootView, u32 lytWkId, gfl2::lyt::LytMultiResID* pMultiResId );

      //------------------------------------------------------------------
      /**
       * @brief   終了処理
       */
      //------------------------------------------------------------------
      void Terminate();

      //------------------------------------------------------------------
      /**
       * @brief   更新
       */
      //------------------------------------------------------------------
      void Update();

      //------------------------------------------------------------------
      /**
       * @brief   アクティブ設定
       */
      //------------------------------------------------------------------
      void SetActive( const bool bFlg );

      //------------------------------------------------------------------
      /**
       * @brief   表示設定
       */
      //------------------------------------------------------------------
      void SetViaible( const bool bFlg );

      //------------------------------------------------------------------
      /**
       * @brief   項目データ設定
       */
      //------------------------------------------------------------------
      void SetData( const DATA_PARAM& rDataParam );

      //------------------------------------------------------------------
      /**
       * @brief   テキスト更新
       */
      //------------------------------------------------------------------
      void UpdateText();

      //------------------------------------------------------------------
      /**
       * @brief   項目データリストを移動
       * @param   moveNum : データリストをずらす移動する値
       * @return  データ変更成功 -> 変更結果のデータのアドレスを返す / データ変更失敗 -> NULL
       * @note
       *          SetData()関数であらかじめデータを設定しないと壊れる
       */
      //------------------------------------------------------------------
      const DATA_PARAM* MoveSelectData( const s32 moveNum );

      //------------------------------------------------------------------
      /**
       * @brief   初期化パラメータ取得
       */
      //------------------------------------------------------------------
      const INIT_PARAM& GetInitParam() const { return m_initParam; }

      //  カーソルペインアドレス取得
      gfl2::lyt::LytPane* GetCursorPosPane() const { return m_pCursorPosPane; }

    private:
      //  矢印ボタンの更新
      void _UpdateArrowButtonObj();

      //  選択テキストを指定
      void _SetSelectText( const u32 textId );

      void _Clear()
      {
        gfl2::std::MemClear( &m_initParam, sizeof( m_initParam ) );
        gfl2::std::MemClear( &m_dataParam, sizeof( m_dataParam ) );

        m_pRootView         = NULL;
        m_pTitleTextBoxPane = NULL;
        m_pDataTextBoxPane  = NULL;
        m_pCursorPosPane    = NULL;
        m_pRootPane         = NULL;

        m_rootLytWkId = 0;
      }

      //  変数宣言一覧
      INIT_PARAM              m_initParam;
      DATA_PARAM              m_dataParam;

      app::ui::UIView*        m_pRootView;
      u32                     m_rootLytWkId;

      gfl2::lyt::LytParts*    m_pRootPane;
      gfl2::lyt::LytTextBox*  m_pTitleTextBoxPane;
      gfl2::lyt::LytTextBox*  m_pDataTextBoxPane;
      gfl2::lyt::LytPane*     m_pCursorPosPane;
  };

GFL_NAMESPACE_END( config )
GFL_NAMESPACE_END( app )

#endif // NIJI_PROJECT_CONFIG_LOWER_UI_LIST_ITEM_OBJ_H_INCLUDED 
