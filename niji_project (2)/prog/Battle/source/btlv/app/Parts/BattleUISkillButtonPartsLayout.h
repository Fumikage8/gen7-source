//=============================================================================
/**
 * @file BattleUISkillButtonPartsLayout.h
 * @brief パーツレイアウトスキル項目ボタンの制御
 * @author yuto_uchida
 * @date 2015.04.15
 */
//=============================================================================
#if !defined( __BTL_UI_SKILL_BUTTON_PARTS_LAYOUT_H__ )
#define __BTL_UI_SKILL_BUTTON_PARTS_LAYOUT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <layout/include/gfl2_LytSys.h>
#include <AppLib/include/Ui/UIResponder.h>

#include "Battle/source/btl_common.h"

//  前方宣言
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( tool )
  
    class AppCommonGrpIconData;

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

GFL_NAMESPACE_BEGIN( app )

GFL_NAMESPACE_BEGIN( util )
  class G2DUtil;
GFL_NAMESPACE_END( util )

GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( parts )

  //  効果アイコン
  class BattleUIKoukaIconPartsLayout;

  /**
    * @brief スキル項目ボタンパーツレイアウトの制御クラス
    * @note
    *    レイアウトファイル「Battle_PLT_LOW_002.flyt」のパーツの制御をしている
   */
  class BattleUISkillButtonPartsLayout
  {
  public:
    //  アニメ一覧
    enum AnimType
    {
      ANIM_TYPE_TOUCH,          //  「touch」した時
      ANIM_TYPE_RELEASE,        //  「release」した時
      ANIM_TYPE_CANSEL,         //  「cansel」した時
      ANIM_TYPE_PASSIVE,        //  「passive」状態の時
      ANIM_TYPE_ACTIVE,         //  「active」状態の時
      ANIM_TYPE_TOUCH_RELEASE,  //  「touch_release」状態の時
      ANIM_TYPE_COLOR_00,       //  ノーマル色
      ANIM_TYPE_COLOR_01,       //  かくとう色
      ANIM_TYPE_COLOR_02,       //  ひこう色
      ANIM_TYPE_COLOR_03,       //  どく色
      ANIM_TYPE_COLOR_04,       //  じめん色
      ANIM_TYPE_COLOR_05,       //  いわ色
      ANIM_TYPE_COLOR_06,       //  むし色
      ANIM_TYPE_COLOR_07,       //  ゴースト色
      ANIM_TYPE_COLOR_08,       //  はがね色
      ANIM_TYPE_COLOR_09,       //  ほのお色
      ANIM_TYPE_COLOR_10,       //  みず色
      ANIM_TYPE_COLOR_11,       //  くさ色
      ANIM_TYPE_COLOR_12,       //  でんき色
      ANIM_TYPE_COLOR_13,       //  エスパー色
      ANIM_TYPE_COLOR_14,       //  こおり色
      ANIM_TYPE_COLOR_15,       //  ドラゴン色
      ANIM_TYPE_COLOR_16,       //  あく色
      ANIM_TYPE_COLOR_17,       //  フェアリー色
      ANIM_TYPE_TEX_COLOR_00,   //  "PPの残りの割合に応じてテキストの色を変更するためのアニメーション通常表示
      ANIM_TYPE_TEX_COLOR_01,   //  "PPの残りの割合に応じてテキストの色を変更するためのアニメーション残り50%以下
      ANIM_TYPE_TEX_COLOR_02,   //  "PPの残りの割合に応じてテキストの色を変更するためのアニメーション残り25%以下
      ANIM_TYPE_TEX_COLOR_03,   //  "PPの残りの割合に応じてテキストの色を変更するためのアニメーション残り0%

      //  インフォ用
      ANIM_TYPE_INFO_TOUCH,
      ANIM_TYPE_INFO_RELEASE,
      ANIM_TYPE_INFO_CANSEL,
      ANIM_TYPE_INFO_PASSIVE,
      ANIM_TYPE_INFO_ACTIVE,
      ANIM_TYPE_INFO_TOUCH_RELEASE,

      //  各効果用
      ANIM_TYPE_KOUKA_ACTIVE,
      ANIM_TYPE_KOUKA_PASSIVE,
      ANIM_TYPE_KOUKA_KOUKA_00, //  効果ばつぐん
      ANIM_TYPE_KOUKA_KOUKA_01, //  効果いまひとつ
      ANIM_TYPE_KOUKA_KOUKA_02, //  効果なし
      ANIM_TYPE_KOUKA_KOUKA_03, //  効果普通

      ANIM_TYPE_MAX,
    };

    typedef struct _tag_init
    {
      //  ボタンID
      s32 panel_button_id;
      s32 info_button_id;

      //  アニメidリスト
      //  ※ 使う側で指定したのは項目ごとに用意アニメidがそれぞれ用意されているが、クラス内部では項目の行数を意識した作りにはしたくなかったので
      s32 a_anim_id_list[ ANIM_TYPE_MAX ];
    } INIT_PARAM;

    //  ペイン設定パラメータ
    typedef struct _tag_init_pane_param_
    {
      _tag_init_pane_param_()
      {
        pRootPartsPane        = NULL;
        pCursorNullPane       = NULL;
        pButtonHitBoxPane     = NULL;
        pInfoIconPartsPane    = NULL;
        pInfoIconHitBoxPane   = NULL;
        pNowPPTextBoxPane     = NULL;
        pMaxPPNumTextBoxPane  = NULL;
        pPPLavelTextBoxPane   = NULL;
        pPPSlashTextBoxPane   = NULL;
        pSkillNameTextBoxPane = NULL;
        pKoukaIconPartsPane   = NULL;
        pTypeIconPicPane      = NULL;
        pPPTitleTextPane      = NULL;
      }

      //  パーツルートペイン
      gfl2::lyt::LytParts*  pRootPartsPane;

       //  カーソルNULLペイン
      gfl2::lyt::LytPane* pCursorNullPane;

      //  ボタンのヒット判定のボックスペイン
      gfl2::lyt::LytBounding* pButtonHitBoxPane;
      //  インフォアイコンのパーツペイン
      gfl2::lyt::LytParts* pInfoIconPartsPane;

      //  インフォアイコンのヒット判定のボックスペイン
      gfl2::lyt::LytBounding* pInfoIconHitBoxPane;

      //  現在PP値のテキストペイン
      gfl2::lyt::LytTextBox*  pNowPPTextBoxPane;

      //  最大PP値のテキストペイン
      gfl2::lyt::LytTextBox*  pMaxPPNumTextBoxPane;

      /** @fix  MMCat[171]   つよさをみる画面のPP値の文字色に関する質問:  "PP","/"も色変更の対象とする  */
      //  "PP"
      gfl2::lyt::LytTextBox*  pPPLavelTextBoxPane;

      //  "/"
      gfl2::lyt::LytTextBox*  pPPSlashTextBoxPane;


      //  技名のテキストペイン
      gfl2::lyt::LytTextBox*  pSkillNameTextBoxPane;

      //  効果アイコンペイン
      gfl2::lyt::LytParts*  pKoukaIconPartsPane;

      //  タイプアイコン画像ペイン
      gfl2::lyt::LytPicture*  pTypeIconPicPane;

      //  PP表記のテキストペイン
      gfl2::lyt::LytTextBox*  pPPTitleTextPane;

    } INIT_PANE_PARAM;

    BattleUISkillButtonPartsLayout( ::app::util::G2DUtil* pG2DUtil, s32 lytWkId );
    ~BattleUISkillButtonPartsLayout();

    //  初期化処理
    void  Initialize( const INIT_PARAM& initParam, const INIT_PANE_PARAM& rInitPaneParam, ::app::tool::AppCommonGrpIconData*  pAppCommonGrpIconData, gfl2::heap::HeapBase* pHeap );

    //  初期化パラメータ取得
    const INIT_PARAM& GetInitParam() const { return m_initParam; }

    //  パネルボタンデータ取得（ボタンマネージャーに登録時に必要）
    const ::app::ui::ButtonInfoEx& GetPanelButtonInfoEx() const  { return m_panelInfoEx; }

    //  パネルのインフォアイコンボタンデータ取得（ボタンマネージャーに登録時に必要）
    const ::app::ui::ButtonInfoEx& GetPanelInfoIconButtonInfoEx() const  { return m_panelInfoIconInfoEx; }

    //  カーソル位置のペイン取得
    gfl2::lyt::LytPane* GetCursorNullPane() { return m_initPaneParam.pCursorNullPane; }

    //  アクティブ設定
    void SetActive( const bool bFlg );

    //  表示設定
    void SetVisible( const bool bFlg );

    //  表示しているか
    bool IsVisible();

    //  インフォアイコン表示設定
    void SetInfoIconVisible( const bool bFlg );

    //  インフォアイコン表示しているかどうか
    bool IsInfoIconVisible();

    //  技Id設定
    void SetId( const WazaID wazaId );
    void SetId( const WazaID wazaId, gfl2::str::StrBuf* pWazaName );

    //  PP値の設定
    void SetPPNum( const u8 nowNum, const u8 maxNum );

    //  PP値の取得
    u8 GetNowPPNum() const { return m_nowPPNum; }
    // @fix NMCat[496] ＰＰ最大値なのに現在値を返していたのを修正
    u8 GetMaxPPNum() const { return m_maxPPNum; }

    //  効果タイプ
    void SetKoukaType( const BtlTypeAffAbout koukaType );
    //  効果タイプ表示設定
    void SetKoukaTypeVisible( const bool bFlg );

    //  id取得
    WazaID GetId() const { return m_wazaId; }

  private:
    //  スキルタイプ
    void _SetSkillType( PokeType_tag skillType );

    void _Clear()
    {
      m_pKoukaIconPartsLayout = NULL;
      m_pG2DUtil              = NULL;
      m_pAppCommonGrpIconData = NULL;
      m_lytWkId               = -1;
      m_bAcitve               = true;
      m_wazaId                = WAZANO_NULL;
      m_nowPPNum              = 0;
      m_maxPPNum              = 0;

      m_initParam.panel_button_id = -1;
      m_initParam.info_button_id  = -1;

      {
        m_panelInfoEx.button_id     = 0;
        m_panelInfoEx.picture_pane  = NULL;
        m_panelInfoEx.bound_pane    = NULL;
        m_panelInfoEx.touch_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoEx.release_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoEx.cancel_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
        m_panelInfoEx.key_select_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoEx.active_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoEx.passive_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
      }

      {
        m_panelInfoIconInfoEx.button_id     = 0;
        m_panelInfoIconInfoEx.picture_pane  = NULL;
        m_panelInfoIconInfoEx.bound_pane    = NULL;
        m_panelInfoIconInfoEx.touch_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoIconInfoEx.release_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoIconInfoEx.cancel_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;  
        m_panelInfoIconInfoEx.key_select_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoIconInfoEx.active_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_panelInfoIconInfoEx.passive_anime_index = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
      }
    }

    //  パネルのボタンデータ
    ::app::ui::ButtonInfoEx m_panelInfoEx;

    //  パネルのインフォアイコンボタンデータ
    ::app::ui::ButtonInfoEx m_panelInfoIconInfoEx;

    //  アイコングラフィックデータ
    ::app::tool::AppCommonGrpIconData*  m_pAppCommonGrpIconData;

    INIT_PARAM              m_initParam;
    INIT_PANE_PARAM         m_initPaneParam;

    BattleUIKoukaIconPartsLayout* m_pKoukaIconPartsLayout;

    bool                    m_bAcitve;
    ::app::util::G2DUtil*   m_pG2DUtil;
    s32                     m_lytWkId;
    WazaID                  m_wazaId;
    u8                      m_nowPPNum;
    u8                      m_maxPPNum;

  };

GFL_NAMESPACE_END( parts )
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BTL_UI_SKILL_BUTTON_PARTS_LAYOUT_H__ 
