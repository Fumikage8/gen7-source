//=============================================================================
/**
 * @file BattleUISkillSelectLowerDraw.cpp
 * @brief バトル下画面 >> 「技選択画面」画面
 * @author yuto_uchida
 * @date 2015.03.13
 */
//=============================================================================
#if !defined( __BATTLE_UI_SKILL_SELECT_LOWER_DRAW_H__ )
#define __BATTLE_UI_SKILL_SELECT_LOWER_DRAW_H__
#pragma once

#include <Math/include/gfl2_math.h>

#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"

#include "Battle/source/btlv/app/parts/BattleUISkillButtonPartsLayout.h"
#include "Battle/source/btlv/app/parts/BattleUIZenryokuSkillButtonPartsLayout.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )

  class AppCommonGrpIconData;
  class CommonMessageObject;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )

//@memo  各ボタン項目のグラフィックフォントはレイアウトデータで対応

  /** バトル下画面の技選択画面の通知クラス */
  class BattleUISkillSelectLowerDrawDelegate
  {
  public:
    BattleUISkillSelectLowerDrawDelegate() {}
    virtual ~BattleUISkillSelectLowerDrawDelegate() {}

    //  決定入力用リスナ
    virtual bool OnEnterInput( class BattleUISkillSelectLowerDraw* pDrawInst, s32 buttonId )  = 0;

    /// キー入力通知用リスナ
    virtual bool OnKeyAction( class BattleUISkillSelectLowerDraw* pDrawInst, gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick ) = 0;
  };

  /**
  * @brief バトル下画面の技選択画面基本クラス
  */
  class BattleUISkillSelectLowerDraw : public ::app::ui::UIView, public ::app::ui::UIInputListener
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUISkillSelectLowerDraw );  // コピーを禁止

    public:
      // ステート
      enum StateType
      {
        STATE_TYPE_NONE         = 0,
        STATE_TYPE_IN,
        STATE_TYPE_INPUT_SKILL_SELECT_LIST,
        STATE_TYPE_INPUT_SPECIAL_BUTTON,
        STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST,
        STATE_TYPE_ENTER_ANIM_WAIT,
        STATE_TYPE_OUT,
        STATE_TYPE_END,
      };

      //  カーソル一種類
      enum CursorType
      {
        CURSOR_POS_TYPE_NONE     = 0,
        CURSOR_POS_TYPE_SKILL_00,
        CURSOR_POS_TYPE_SKILL_01,
        CURSOR_POS_TYPE_SKILL_02,
        CURSOR_POS_TYPE_SKILL_03,
        CURSOR_POS_TYPE_SPECIAL,

        CURSOR_POS_TYPE_MAX
      };

      //  ボタンIDリスト
      enum ButtonId
      {
        BUTTON_ID_SKILL_ITEM_00,
        BUTTON_ID_SKILL_ITEM_01,
        BUTTON_ID_SKILL_ITEM_02,
        BUTTON_ID_SKILL_ITEM_03,

        BUTTON_ID_SKILL_ITEM_INFO_00,
        BUTTON_ID_SKILL_ITEM_INFO_01,
        BUTTON_ID_SKILL_ITEM_INFO_02,
        BUTTON_ID_SKILL_ITEM_INFO_03,

        BUTTON_ID_ZENRYOKU_SKILL_ITEM_00,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_01,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_02,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_03,

        BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_00,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_01,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_02,
        BUTTON_ID_ZENRYOKU_SKILL_ITEM_INFO_03,

        BUTTON_ID_ZENRYOKU,
        BUTTON_ID_MEGA,
        BUTTON_ID_ULTRABURST, //momiji追加 ヒカリドラゴン専用

        BUTTON_ID_BACK,

        BUTTON_ID_MAX,
      };

      //  特別ボタンタイプ
      enum SpecialButtonType
      {
        SPECIAL_BUTTON_TYPE_NONE = 0,
        SPECIAL_BUTTON_TYPE_ZENRYOKU,
        SPECIAL_BUTTON_TYPE_MEGA,
        SPECIAL_BUTTON_TYPE_ULTRABURST,
      };

      enum
      {
        SKILL_LIST_MAX = 4,
      };

      //  わざ選択の基本パラメータ
      struct SKILL_SELECT_BASE_PARAM
      {
        //  わざid
        WazaID              wazaId;

        //  効果タイプ
        BtlTypeAffAbout koukaType;

        //  効果タイプ表示
        bool  bKoukaTypePutFlg;

        SKILL_SELECT_BASE_PARAM()
        {
          wazaId       = WAZANO_NULL;
          koukaType    = pml::battle::TypeAffinity::TYPEAFF_ABOUT_NONE;

          bKoukaTypePutFlg  = false;
        }
      };

      //  技選択項目データ
      typedef struct _tag_skill_select_param
      {
        //  通常用
        struct
        {
          SKILL_SELECT_BASE_PARAM base;

          //  PPの現在、最大値
          u8 ppNowNum, ppMaxNum;

        } normal;

        //  全力用
        struct
        {
          SKILL_SELECT_BASE_PARAM base;

          gfl2::str::StrBuf*  pName;

          //  全力技は技によってタイプが変わり可能性がある（今作ではないが次回作にはあるかも）
          PokeType_tag        type;
          //  全力の威力が高い
          bool  bNo1Power;

        } zenryoku;

        //  インフォアイコン表示
        bool  bInfoIconVisible;


        //  技タイプ
        PokeType_tag  skillType;

        _tag_skill_select_param()
        {
          normal.ppNowNum     = 0;
          normal.ppMaxNum     = 0;

          zenryoku.pName      = NULL;
          zenryoku.bNo1Power  = false;

          bInfoIconVisible  = false;
          skillType         = POKETYPE_NORMAL;
        }

      } SKILL_SELECT_PARAM;

      //  特殊項目データ
      typedef struct
      {
        SpecialButtonType type;

      } SPECIAL_SELECT_PARAM;

      //  クラス初期化時に必要なパラメータ
      typedef struct _tag_init_param
      {
        //  使用するメッセージのポインタ（GARC_message_btl_app_DATのデータでないとアウト）
        gfl2::str::MsgData* pBtlAppMsgData;
        print::WordSet*     pBtlWordSet;

        //  描画用ヒープ
        ::app::util::Heap* pUtilHeap;

        ::app::util::AppRenderingManager*     pRenderingManager;
        BattleUISkillSelectLowerDrawDelegate* pDelegate;

        //  アイコングラフィックデータ
        ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData;

        void* pLytBuff;
        void* pMenuCursorLytBuff;

        //  入力できるか
        bool  bControllable;

        //  特殊項目
        SPECIAL_SELECT_PARAM  specialSelectParam;

        //  初回カーソルタイプ
        CursorType  firstCursorType;

        //  技選択個数
        s32 skillSelectItemNum;

        //  入力SEを再生するか
        bool bUseInputSE;

        //  技選択項目
        SKILL_SELECT_PARAM aSkillList[ SKILL_LIST_MAX ];

        _tag_init_param()
        {
          pBtlAppMsgData        = NULL;
          pBtlWordSet           = NULL;
          pAppCommonGrpIconData = NULL;
          pUtilHeap             = NULL;
          pRenderingManager     = NULL;
          pDelegate             = NULL;
          pLytBuff              = NULL;
          pMenuCursorLytBuff    = NULL;
          bUseInputSE           = false;
          bControllable         = true;

          //  特殊選択関連のデータ初期化
          {
            specialSelectParam.type  = SPECIAL_BUTTON_TYPE_NONE;
          }

          //  技選択関連のデータ初期化
          {
            skillSelectItemNum    = 0;
            firstCursorType       = CURSOR_POS_TYPE_SKILL_00;
          }
        }

      } INIT_PARAM;

      //---------------------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //---------------------------------------------------------------------------------
      /**
        * @brief コンストラクタ
        * @note
        *    あらかじめ使う側がレイアウトデータを作成して渡す。
        */
      BattleUISkillSelectLowerDraw( const INIT_PARAM& param );

      /**
        * @brief デストラクタ
        */
      virtual ~BattleUISkillSelectLowerDraw();

      /**
      * @brief  初期化
      */        
      void Initialize();

      /**
      * @brief  終了
      */        
      void Terminate();

      /**
      * @brief  クラス破棄可能か
      */
      bool IsDelete();

      /**
        * @brief 開始
        */
      void Start() { _SetState( STATE_TYPE_IN ); }

      /**
        * @brief 終了
        */
      void End() { _SetState( STATE_TYPE_OUT ); }

      /**
        * @brief 更新処理
        */
      virtual void Update();

      /**
      * @brief 描画
      */
      virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

      /**
      * @brief  初期化データ取得
      */
      const INIT_PARAM& GetInitParam() const { return m_initParam; }

      /**
      * @brief  カーソル選択している項目を押す
      */
      void PushBtnCurrentCursorItem();

      /**
      * @brief  わざ選択カーソルしている項目のインフォアイコンを押す
      * @return インフォアイコンを押した : true / 押していない : false
      */
      bool PushBtnCurrentCursorItemInfoIcon();

      /**
      * @brief  指定したボタンを押す
      */
      void PushBtn( ButtonId buttonId );

      /**
      * @brief  特殊ボタンを発動状態にさせるかどうか
      */
      void InvokeSpecialBtn( SpecialButtonType type, bool bFlg );

      /**
      * @brief  ヘルプメッセージテキスト設定
      */
      void SetHelpMessageId( const s32 msgId );

      /**
      * @brief  現在のステート
      */
      StateType GetState() const { return m_state; }

      /**
      * @brief  技リストのカレント位置を更新
      * @return カーソル位置が変わった -> true / 変わっていない -> false
      */
      bool SetCurrentIdxSkillList( const s32 idx );

      //  わざ選択ステート状態か
      bool IsSkillSelectState()
      {
        if( m_state == STATE_TYPE_INPUT_SKILL_SELECT_LIST )
        {
          return  true;
        }

        if( m_state == STATE_TYPE_INPUT_ZENRYOKU_SKILL_SELECT_LIST )
        {
          return  true;
        }

        return  false;
      }

      //  技リストのカレントidx
      s32 GetSkillListCursorIdx() const { return m_skillListCursorIdx; }

      //  カレントカーソルタイプ
      CursorType  GetCurrentCursorType() const { return m_cursorSelectType; }

      /**
      * @brief  特殊ボタンへカーソル移動
      * @return 移動したら true / 移動していないのであれば false
      */
      bool  MoveCursorSpecialButton();

      /**
      * @brief  技リストへカーソル移動
      * @return 移動した -> true / 移動していない -> false
      */
      bool MoveCursorSkillList();

      //  スキル選択リストのカーソルが一番上か
      bool IsSkillSelectListCurrentUpperLimit();

      //  スキル選択リストのカーソルが一番下か
      bool IsSkillSelectListCurrentUnderLimit();

      //  ボタンidからカーソルリストidxを取得
      s32 GetButtonIdToCursorSelectListIdx( const ButtonId id );

      // ペインタッチ時を検知
      virtual ListenerResult OnLayoutPaneEvent( const u32 painId );

      // ペインタッチ時を検知
      // 主な用途  RELEASE判定だが、決定前時にカーソルを移動したい、とか
      virtual void OnLayoutPaneTouchTriggerEvent( const u32 buttonId );

      /// キー入力通知用リスナ
      virtual ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

    private:
      enum
      {
        STR_BUFFER_SIZE_MAX = 256,
      };

      //  レイアウトワーク一覧
      enum LytWkType
      {
        LYT_WK_TYPE_SCREEN  = 0,
        LYT_WK_TYPE_MAX,
      };

      //  全力わざ選択のステート
      enum SubStateZenryokuSelectType
      {
        SUB_STATE_ZENRYOKU_SELECT_TYPE_EVENT  = 0,
        SUB_STATE_ZENRYOKU_SELECT_TYPE_IN,
        SUB_STATE_ZENRYOKU_SELECT_TYPE_INPUT,
        SUB_STATE_ZENRYOKU_SELECT_TYPE_OUT,
        SUB_STATE_ZENRYOKU_SELECT_TYPE_END,
      };

      /** 特別ボタン基本クラス(メガ進化・全力ボタンに使用)*/
      class SpecialButtonBase
      {
      public:
         typedef struct _tag_init
        {
          //  ボタンID
          s32 button_id;

          //  ペインID
          s32 pane_id;

          //  2D制御
          ::app::util::G2DUtil* p_g2d_util;

          //  レイアウトid
          s32 lyt_wk_id;

          //  リソースid
          s32 res_id;
        } INIT_PARAM;

        SpecialButtonBase() { _Clear(); }
        virtual void Initialize( const INIT_PARAM& initParam ) { m_initParam  = initParam; }

        virtual void SetActive( const bool bFlg )  = 0;
        virtual void SetVislble( const bool bFlg ) = 0;

        virtual void InvokeSpecial( const bool bFlg ) = 0;

        //  ボタンデータ取得（ボタンマネージャーに登録時に必要）
        const ::app::ui::ButtonInfoEx& GetButtonInfoEx() const { return m_buttonInfoEx; };

        //  初期化データ取得
        const INIT_PARAM& GetInitParam() const { return m_initParam; }

        //  カーソルnullペインを取得
        gfl2::lyt::LytPane* GetCursorNullPane() { return m_pCursorNullPane; }

      protected:
        void _Clear()
        {
          m_bActive         = false;
          m_bInvoke         = false;
          m_pCursorNullPane = NULL;
          m_pAllPane        = NULL;

          m_buttonInfoEx.button_id  = BUTTON_ID_MAX;
          m_buttonInfoEx.picture_pane = NULL;
          m_buttonInfoEx.bound_pane   = NULL;
          m_buttonInfoEx.touch_anime_index        = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
          m_buttonInfoEx.release_anime_index      = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;     
          m_buttonInfoEx.cancel_anime_index       = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
          m_buttonInfoEx.key_select_anime_index   = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
          m_buttonInfoEx.active_anime_index       = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;      
          m_buttonInfoEx.passive_anime_index      = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;

          m_initParam.lyt_wk_id = 0;
          m_initParam.res_id    = 0;
        }

        ::app::ui::ButtonInfoEx m_buttonInfoEx;
        INIT_PARAM              m_initParam;
        gfl2::lyt::LytPane*     m_pCursorNullPane;
        gfl2::lyt::LytPane*     m_pAllPane;

        bool                    m_bActive;
        bool                    m_bInvoke;
      };

      //  わざボタン( 通常わざとぜんりょくわざの２種類あるので切り替えの管理をする )
      class SkillButton
      {
      public:
        enum ChangeType
        {
          CHANGE_TYPE_NORMAL  = 0,
          CHANGE_TYPE_ZENRYOKU,
        };

        typedef struct
        {
          parts::BattleUISkillButtonPartsLayout::INIT_PARAM       init;
          parts::BattleUISkillButtonPartsLayout::INIT_PANE_PARAM  pane;
        } NORMAL_SKILL_SETUP_DATA;

        typedef struct
        {
          parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PARAM       init;
          parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PANE_PARAM  pane;
        } ZENRYOKU_SKILL_SETUP_DATA;

        SkillButton() { _Clear(); }

        ~SkillButton();

        void Initialize( 
          NORMAL_SKILL_SETUP_DATA& rNormalSetupParam, ZENRYOKU_SKILL_SETUP_DATA& rZenryokuSetupParam, 
          const SKILL_SELECT_PARAM& rSkillParam, ::app::tool::AppCommonGrpIconData* pAppCommonGrpIconData,
          gfl2::heap::HeapBase* pHeap, ::app::util::G2DUtil* pG2DUtil );

        //  選択の種類を変える
        void Change( const ChangeType type, ::app::tool::ButtonManager* pButtonManager );

        //  カーソル位置のペイン取得
        gfl2::lyt::LytPane*  GetCursorNullPane();

        void SetActive( const bool bFlg );
        void SetVisible( const bool bFlg );

        //  インフォボタンを表示しているか
        bool IsInfoIconVisible();

        s32  GetPanelButtonId();
        s32  GetInfoButtonId();

        parts::BattleUISkillButtonPartsLayout*          GetNormal() const { return m_pNormalButtonInst; }
        parts::BattleUIZenryokuSkillButtonPartsLayout*  GetZenryoku() const { return m_pZenryokuButtonInst; }

      private:

        void _Clear()
        {
          m_pZenryokuButtonInst = NULL;
          m_pNormalButtonInst   = NULL;
          m_type                = CHANGE_TYPE_NORMAL;
        }

        parts::BattleUIZenryokuSkillButtonPartsLayout*    m_pZenryokuButtonInst;
        parts::BattleUISkillButtonPartsLayout*            m_pNormalButtonInst;
        SKILL_SELECT_PARAM                                m_skillParam;
        ChangeType                                        m_type;
      };

      /** 全力ボタン */
      class ZenryokuButton : public SpecialButtonBase
      {
      public:
        virtual void Initialize( const INIT_PARAM& initParam );
        virtual void SetActive( const bool bFlg );
        virtual void SetVislble( const bool bFlg );
        virtual void InvokeSpecial( const bool bFlg );
      };

      /** メガ進化ボタン */
      class MegaButton : public SpecialButtonBase
      {
      public:
        virtual void Initialize( const INIT_PARAM& initParam );
        virtual void SetActive( const bool bFlg );
        virtual void SetVislble( const bool bFlg );
        virtual void InvokeSpecial( const bool bFlg );

      private:
        void SetEnableEffect( const bool bFlg );
      };

      /** ヒカリドラゴン進化ボタン */
      class UltraBurstButton : public SpecialButtonBase
      {
      public:
        virtual void Initialize(const INIT_PARAM& initParam);
        virtual void SetActive(const bool bFlg);
        virtual void SetVislble(const bool bFlg);
        virtual void InvokeSpecial(const bool bFlg);

      private:
        void SetEnableEffect(const bool bFlg);
      };

      void _Initialize2D( void* pAppLytBuff );

      void _UpdateState();
      void _SetState( const StateType newStateType );
      void _SetSubStateZenryokuSelect( const SubStateZenryokuSelectType newStateType );

      /**
      * @brief  わざ選択のカレントidx設定
      * @return 移動した -> true / 移動していない -> false
      */
      bool _SetCurrentIdxSkillList( const s32 idx );

      //  SkillButtonクラスに必要ならパラメーターを出力
      void _OutputSkillItemInitParam( SkillButton::NORMAL_SKILL_SETUP_DATA* pOutputNormalButtonParam, SkillButton::ZENRYOKU_SKILL_SETUP_DATA* pOutputZenryokuButtonParam, const u32 selecListIdx );

      //  parts::BattleUISkillButtonPartsLayoutのインスタンス作成に必要なパラメータ出力(通常わざ用)
      void _OutputNormalSkillItemInitParam( parts::BattleUISkillButtonPartsLayout::INIT_PARAM* pOutInitParam, parts::BattleUISkillButtonPartsLayout::INIT_PANE_PARAM* pOutInitPaneParam, const u32 selecListIdx );

      //  parts::BattleUIZenryokuSkillButtonPartsLayoutのインスタンス作成に必要なパラメータ出力(全力わざ用)
      void _OutputZenryokuSkillItemInitParam( parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PARAM* pOutInitParam, parts::BattleUIZenryokuSkillButtonPartsLayout::INIT_PANE_PARAM* pOutInitPaneParam, const u32 selecListIdx );

      //  変数等の初期化
      void _Clear()
      {
        m_pZenryokuButtonInst         = NULL;
        m_pMegaButtonInst             = NULL;
        m_pUltraBurstButtonInst = NULL;
        m_pMenuCursor = NULL;
        m_pSceneLayoutWork            = NULL;
        m_pCommonMessageObject        = NULL;
        m_pProcSpecialButtonInst      = NULL;
        m_state                       = STATE_TYPE_NONE;
        m_cursorSelectType            = CURSOR_POS_TYPE_SKILL_00;
        m_subStateZenryokuSelectType  = SUB_STATE_ZENRYOKU_SELECT_TYPE_IN;

        m_skillListCursorIdx          = 0;
        m_bPutWazaInfoShortCutKeyText = true;
      }

      //  変数宣言一覧

      INIT_PARAM                  m_initParam;

      StateType                   m_state;
      SubStateZenryokuSelectType  m_subStateZenryokuSelectType;

      s32                         m_skillListCursorIdx;

      //  わざボタンリスト
      SkillButton                                       m_aSkillButtonInst[ SKILL_LIST_MAX ];

      ZenryokuButton*                                   m_pZenryokuButtonInst;
      MegaButton*                                       m_pMegaButtonInst;
      UltraBurstButton*                                 m_pUltraBurstButtonInst;

      //  メガ進化・全力が押せるのであれば、どちらかのインスタンスアドレスが代入される
      SpecialButtonBase*                                m_pProcSpecialButtonInst;

      ::app::tool::CommonMessageObject*                 m_pCommonMessageObject;
      CursorType                                        m_cursorSelectType;

      gfl2::lyt::LytWk*                                 m_pSceneLayoutWork;

      ::app::tool::MenuCursor*                          m_pMenuCursor;
      bool                                              m_bPutWazaInfoShortCutKeyText;

      static const CursorType                           m_saListCursorType[];
  };

GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BATTLE_UI_SKILL_SELECT_LOWER_DRAW_H__ 
