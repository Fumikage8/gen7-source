#if PM_DEBUG

//=============================================================================
/**
 * @file DebugPokeMakeCore.h
 * @brief デバッグポケモン作成画面
 * @author obata_toshihiro
 * @date 2011.07.11
 */
//=============================================================================
#ifndef __DEBUGPOKEMAKECORE_H__
#define __DEBUGPOKEMAKECORE_H__ 

#include <str/include/gfl2_Str.h>

#include <Print/include/WordSet.h>
#include <AppLib/include/Tool/app_tool_ButtonManager.h>
#include <AppLib/include/Util/app_util_Controller.h>
#include <Debug/DebugNumInput/include/DebugNumInput.h>

#include <System/include/nijiAllocator.h>
#include <System/include/AppRenderingPipeLine.h>

#include "./temp/DebugPokeMakeTemp_app_util.h"


namespace debug {
namespace PokeMake {


  class PokeMakeCore : 
    public PokeMakeTemp::LytUtil,
#if 0
    public app::util::LayoutContainer,
#endif
    public app::util::Controller,
    public app::tool::ButtonManager::IButtonCallback
  {
    GFL_FORBID_COPY_AND_ASSIGN( PokeMakeCore );

    public:

    PokeMakeCore( void );
    virtual ~PokeMakeCore();
    virtual bool Update( void );
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
    virtual void CallbackFuncOnButtonSelected( 
        app::tool::ButtonManager::ButtonId selected_button );
    virtual bool GetResult( void );
    virtual bool IsDrawing( void ) const;

    /// 数値入力初期化構造体
    struct NuminputParam{
      bool  input;
      s32   init;
      s32   min;
      s32   max;
      const wchar_t* title;

      NuminputParam( void )
      {
        input = false;
        init  = 0;
        min   = 0;
        max   = 0;
        title = NULL;
      }
    };

    private:


    static const u32 REGION_MAX = 7;

    enum Sequence {
      SEQ_FADEIN,
      SEQ_FADEIN_WAIT,
      SEQ_WAIT,
      SEQ_NUM_INPUT,
      SEQ_KEYBOARD,
      SEQ_NUM,
      SEQ_EXIT,
      SEQ_EXIT_WAIT,
    };

    enum ArrowIndex {
      ARROW_MONSNO_L,
      ARROW_MONSNO_R,
      ARROW_FORMNO_L,
      ARROW_FORMNO_R,
      ARROW_SEIKAKU_L,
      ARROW_SEIKAKU_R,
      ARROW_ITEM_L,
      ARROW_ITEM_R,
      ARROW_TOKUSEI_L,
      ARROW_TOKUSEI_R,
      ARROW_WAZA1_L,
      ARROW_WAZA1_R,
      ARROW_WAZA2_L,
      ARROW_WAZA2_R,
      ARROW_WAZA3_L,
      ARROW_WAZA3_R,
      ARROW_WAZA4_L,
      ARROW_WAZA4_R,
      ARROW_VERSION_L,
      ARROW_VERSION_R,
      ARROW_LANGUAGE_L,
      ARROW_LANGUAGE_R,
      ARROW_CAPTUREBALL_L,
      ARROW_CAPTUREBALL_R,
      ARROW_REGION_L,
      ARROW_REGION_R,
      ARROW_BALL_L,
      ARROW_BALL_R,
      ARROW_EGG_WAZA1_L,
      ARROW_EGG_WAZA1_R,
      ARROW_EGG_WAZA2_L,
      ARROW_EGG_WAZA2_R,
      ARROW_EGG_WAZA3_L,
      ARROW_EGG_WAZA3_R,
      ARROW_EGG_WAZA4_L,
      ARROW_EGG_WAZA4_R,
      ARROW_DEFAULT_POWERUP_REASON_L,
      ARROW_DEFAULT_POWERUP_REASON_R,
      ARROW_NUM,
    };

    enum SwitchIndex {
      SWITCH_SEX,
      SWITCH_SICK,
      SWITCH_TOKUSEI1,
      SWITCH_TOKUSEI2,
      SWITCH_TOKUSEI3,
      SWITCH_EGG,
      SWITCH_RARE,
      SWITCH_DEFAULT_WAZA,
      SWITCH_HATAKU,
      SWITCH_HANERU,
      SWITCH_OYA_SET,
      SWITCH_OYA_SEX,
      SWITCH_EVENT,
      SWITCH_RND_MAX,
      SWITCH_DEL_WAZA2,
      SWITCH_DEL_WAZA3,
      SWITCH_DEL_WAZA4,
      SWITCH_OTHER_OYASEX,
      SWITCH_TRAINING_FLAG_HP,
      SWITCH_TRAINING_FLAG_ATTACK,
      SWITCH_TRAINING_FLAG_DEFENSE,
      SWITCH_TRAINING_FLAG_AGILITY,
      SWITCH_TRAINING_FLAG_SP_ATTACK,
      SWITCH_TRAINING_FLAG_SP_DEFENSE,
      SWITCH_NUM,
    };

    enum StringBoxIndex {
      STRBOX_MONSNAME,
      STRBOX_SEIKAKU,
      STRBOX_ITEM,
      STRBOX_TOKUSEI,
      STRBOX_WAZA1,
      STRBOX_WAZA2,
      STRBOX_WAZA3,
      STRBOX_WAZA4,
      STRBOX_NICKNAME,
      STRBOX_VERSION,
      STRBOX_LANGUAGE,
      STRBOX_OYANAME,
      STRBOX_CONTACT_PLACE,
      STRBOX_EGG_TAKEN_PLACE,
      STRBOX_CAPTURE_BALL,
      STRBOX_REGION,
      STRBOX_OTHER_OYANAME,
      STRBOX_EGG_WAZA1,
      STRBOX_EGG_WAZA2,
      STRBOX_EGG_WAZA3,
      STRBOX_EGG_WAZA4,
      STRBOX_POWERUP_REASON,
      STRBOX_NUM,
    };

    enum NumberBoxIndex {
      NUMBOX_FORMNO,
      NUMBOX_LEVEL,
      NUMBOX_EXP,
      NUMBOX_HP_RND,
      NUMBOX_HP_EXP,
      NUMBOX_HP_VAL,
      NUMBOX_ATK_RND,
      NUMBOX_ATK_EXP,
      NUMBOX_DEF_RND,
      NUMBOX_DEF_EXP,
      NUMBOX_AGI_RND,
      NUMBOX_AGI_EXP,
      NUMBOX_SPATK_RND,
      NUMBOX_SPATK_EXP,
      NUMBOX_SPDEF_RND,
      NUMBOX_SPDEF_EXP,
      NUMBOX_WAZA1_UP,
      NUMBOX_WAZA1_PP,
      NUMBOX_WAZA2_UP,
      NUMBOX_WAZA2_PP,
      NUMBOX_WAZA3_UP,
      NUMBOX_WAZA3_PP,
      NUMBOX_WAZA4_UP,
      NUMBOX_WAZA4_PP,
      NUMBOX_FAMILY,
      NUMBOX_FRIEND,
      NUMBOX_FEED,
      NUMBOX_PLAY,
      NUMBOX_POKERUS,
      NUMBOX_CAPTURE_LEVEL,
      NUMBOX_CONTACT_PLACE,
      NUMBOX_EGG_TAKEN_PLACE,
      NUMBOX_CONTACT_DAY,
      NUMBOX_CONTACT_MONTH,
      NUMBOX_CONTACT_YEAR,
      NUMBOX_EGG_TAKEN_DAY,
      NUMBOX_EGG_TAKEN_MONTH,
      NUMBOX_EGG_TAKEN_YEAR,
      NUMBOX_MEMORY_CODE_OYA,
      NUMBOX_MEMORY_LEVEL_OYA,
      NUMBOX_MEMORY_DATA_OYA,
      NUMBOX_MEMORY_FEEL_OYA,
      NUMBOX_MEMORY_CODE_OTHER,
      NUMBOX_MEMORY_LEVEL_OTHER,
      NUMBOX_MEMORY_DATA_OTHER,
      NUMBOX_MEMORY_FEEL_OTHER,
      NUMBOX_COUNTRY,
      NUMBOX_COUNTRY_AREA,
      NUMBOX_OYA_MEM_CODE,
      NUMBOX_OYA_MEM_LEVEL,
      NUMBOX_OYA_MEM_DATA,
      NUMBOX_OYA_MEM_FEEL,
      NUMBOX_OTHEROYA_MEM_CODE,
      NUMBOX_OTHEROYA_MEM_LEVEL,
      NUMBOX_OTHEROYA_MEM_DATA,
      NUMBOX_OTHEROYA_MEM_FEEL,
      NUMBOX_OTHER_FAMILY,
      NUMBOX_OTHER_FRIEND,
      NUMBOX_PAST_COUNTRY_00,
      NUMBOX_PAST_AREA_00,
      NUMBOX_PAST_COUNTRY_01,
      NUMBOX_PAST_AREA_01,
      NUMBOX_PAST_COUNTRY_02,
      NUMBOX_PAST_AREA_02,
      NUMBOX_PAST_COUNTRY_03,
      NUMBOX_PAST_AREA_03,
      NUMBOX_PAST_COUNTRY_04,
      NUMBOX_PAST_AREA_04,
      NUMBOX_TRAINER_ID_00,
      NUMBOX_TRAINER_ID_01,
      NUMBOX_RARE_RND_00,
      NUMBOX_RARE_RND_01,
      NUMBOX_TRIMIAN_HAIR,
      NUMBOX_TRIMIAN_COUNT,
      NUMBOX_TRIMIAN_COUNT_MAX,
      NUMBOX_NUM,
    }; 

    enum ButtonID {
      BUTTON_ARROW_MONSNO_L,
      BUTTON_ARROW_MONSNO_R,
      BUTTON_ARROW_FORMNO_L,
      BUTTON_ARROW_FORMNO_R,
      BUTTON_ARROW_SEIKAKU_L,
      BUTTON_ARROW_SEIKAKU_R,
      BUTTON_ARROW_ITEM_L,
      BUTTON_ARROW_ITEM_R,
      BUTTON_ARROW_TOKUSEI_L,
      BUTTON_ARROW_TOKUSEI_R,
      BUTTON_SWITCH_SEX,
      BUTTON_SWITCH_SICK,
      BUTTON_SWITCH_RND_MAX,
      BUTTON_SWITCH_TOKUSEI1,
      BUTTON_SWITCH_TOKUSEI2,
      BUTTON_SWITCH_TOKUSEI3,
      BUTTON_SWITCH_EGG,
      BUTTON_SWITCH_RARE,
      BUTTON_STRBOX_MONSNAME,
      BUTTON_STRBOX_SEIKAKU,
      BUTTON_STRBOX_ITEM,
      BUTTON_STRBOX_TOKUSEI,
      BUTTON_NUMBOX_FORMNO,
      BUTTON_NUMBOX_LEVEL,
      BUTTON_NUMBOX_EXP,
      BUTTON_ARROW_WAZA1_L,
      BUTTON_ARROW_WAZA1_R,
      BUTTON_ARROW_WAZA2_L,
      BUTTON_ARROW_WAZA2_R,
      BUTTON_ARROW_WAZA3_L,
      BUTTON_ARROW_WAZA3_R,
      BUTTON_ARROW_WAZA4_L,
      BUTTON_ARROW_WAZA4_R,
      BUTTON_SWITCH_DEL_WAZA2,
      BUTTON_SWITCH_DEL_WAZA3,
      BUTTON_SWITCH_DEL_WAZA4,
      BUTTON_SWITCH_DEFAULT_WAZA,
      BUTTON_SWITCH_HATAKU,
      BUTTON_SWITCH_HANERU,
      BUTTON_STRBOX_WAZA1,
      BUTTON_STRBOX_WAZA2,
      BUTTON_STRBOX_WAZA3,
      BUTTON_STRBOX_WAZA4,
      BUTTON_STRBOX_NICKNAME,
      BUTTON_NUMBOX_HP_RND,
      BUTTON_NUMBOX_HP_EXP,
      BUTTON_NUMBOX_HP_VAL,
      BUTTON_NUMBOX_ATK_RND,
      BUTTON_NUMBOX_ATK_EXP,
      BUTTON_NUMBOX_DEF_RND,
      BUTTON_NUMBOX_DEF_EXP,
      BUTTON_NUMBOX_AGI_RND,
      BUTTON_NUMBOX_AGI_EXP,
      BUTTON_NUMBOX_SPATK_RND,
      BUTTON_NUMBOX_SPATK_EXP,
      BUTTON_NUMBOX_SPDEF_RND,
      BUTTON_NUMBOX_SPDEF_EXP,
      BUTTON_NUMBOX_WAZA1_UP,
      BUTTON_NUMBOX_WAZA1_PP,
      BUTTON_NUMBOX_WAZA2_UP,
      BUTTON_NUMBOX_WAZA2_PP,
      BUTTON_NUMBOX_WAZA3_UP,
      BUTTON_NUMBOX_WAZA3_PP,
      BUTTON_NUMBOX_WAZA4_UP,
      BUTTON_NUMBOX_WAZA4_PP,
      BUTTON_NUMBOX_FAMILY,
      BUTTON_NUMBOX_FRIEND,
      BUTTON_NUMBOX_FEED,
      BUTTON_NUMBOX_NADENADE,

      BUTTON_ARROW_VERSION_L,
      BUTTON_ARROW_VERSION_R,
      BUTTON_ARROW_LANGUAGE_L,
      BUTTON_ARROW_LANGUAGE_R,
      BUTTON_ARROW_CAPTUREBALL_L,
      BUTTON_ARROW_CAPTUREBALL_R,
      BUTTON_SWITCH_OYA_SET,
      BUTTON_SWITCH_OYA_SEX,
      BUTTON_SWITCH_EVENT,
      BUTTON_STRBOX_VERSION,
      BUTTON_STRBOX_LANGUAGE,
      BUTTON_STRBOX_OYANAME,
      BUTTON_STRBOX_CAPTURE_BALL,
      BUTTON_NUMBOX_POKERUS,
      BUTTON_NUMBOX_CAPTURE_LEVEL,
      BUTTON_NUMBOX_CONTACT_PLACE,
      BUTTON_NUMBOX_EGG_TAKEN_PLACE,
      BUTTON_NUMBOX_CONTACT_DAY,
      BUTTON_NUMBOX_CONTACT_MONTH,
      BUTTON_NUMBOX_CONTACT_YEAR,
      BUTTON_NUMBOX_EGG_TAKEN_DAY,
      BUTTON_NUMBOX_EGG_TAKEN_MONTH,
      BUTTON_NUMBOX_EGG_TAKEN_YEAR,
      
      BUTTON_NUMBOX_MEMORY_CODE_OYA,
      BUTTON_NUMBOX_MEMORY_LEVEL_OYA,
      BUTTON_NUMBOX_MEMORY_DATA_OYA,
      BUTTON_NUMBOX_MEMORY_FEEL_OYA,
      BUTTON_NUMBOX_MEMORY_CODE_OTHER,
      BUTTON_NUMBOX_MEMORY_LEVEL_OTHER,
      BUTTON_NUMBOX_MEMORY_DATA_OTHER,
      BUTTON_NUMBOX_MEMORY_FEEL_OTHER,

      BUTTON_ARROW_REGION_L,
      BUTTON_ARROW_REGION_R,
      BUTTON_STRBOX_REGION,
      BUTTON_NUMBOX_COUNTRY,
      BUTTON_NUMBOX_COUNTRY_AREA,
      BUTTON_NUMBOX_OTHER_FAMILY,
      BUTTON_NUMBOX_OTHER_FRIEND,

      BUTTON_STRBOX_OTHER_OYANAME,
      BUTTON_SWITCH_OTHER_OYASEX,
      BUTTON_NUMBOX_PAST_COUNTRY_00,
      BUTTON_NUMBOX_PAST_AREA_00,
      BUTTON_NUMBOX_PAST_COUNTRY_01,
      BUTTON_NUMBOX_PAST_AREA_01,
      BUTTON_NUMBOX_PAST_COUNTRY_02,
      BUTTON_NUMBOX_PAST_AREA_02,
      BUTTON_NUMBOX_PAST_COUNTRY_03,
      BUTTON_NUMBOX_PAST_AREA_03,
      BUTTON_NUMBOX_PAST_COUNTRY_04,
      BUTTON_NUMBOX_PAST_AREA_04,
      BUTTON_NUMBOX_TRAINER_ID_00,
      BUTTON_NUMBOX_TRAINER_ID_01,
      BUTTON_NUMBOX_RARE_RND_00,
      BUTTON_NUMBOX_RARE_RND_01,
      BUTTON_NUMBOX_TRIMIAN_HAIR,
      BUTTON_NUMBOX_TRIMIAN_COUNT,
      BUTTON_NUMBOX_TRIMIAN_COUNT_MAX,
      BUTTON_NUMBOX_CONDITION_01,
      BUTTON_NUMBOX_CONDITION_02,
      BUTTON_NUMBOX_CONDITION_03,
      BUTTON_NUMBOX_CONDITION_04,
      BUTTON_NUMBOX_CONDITION_05,

      BUTTON_NUMBOX_EGG_WAZA1_L,
      BUTTON_NUMBOX_EGG_WAZA1_R,
      BUTTON_NUMBOX_EGG_WAZA1_NAME,
      BUTTON_NUMBOX_EGG_WAZA2_L,
      BUTTON_NUMBOX_EGG_WAZA2_R,
      BUTTON_NUMBOX_EGG_WAZA2_NAME,
      BUTTON_NUMBOX_EGG_WAZA3_L,
      BUTTON_NUMBOX_EGG_WAZA3_R,
      BUTTON_NUMBOX_EGG_WAZA3_NAME,
      BUTTON_NUMBOX_EGG_WAZA4_L,
      BUTTON_NUMBOX_EGG_WAZA4_R,
      BUTTON_NUMBOX_EGG_WAZA4_NAME,

      BUTTON_DEFAULT_POWERUP_REASON_L,
      BUTTON_DEFAULT_POWERUP_REASON_R,
      BUTTON_DEFAULT_POWERUP_RANK_ATK,
      BUTTON_DEFAULT_POWERUP_RANK_DEF,
      BUTTON_DEFAULT_POWERUP_RANK_SPATK,
      BUTTON_DEFAULT_POWERUP_RANK_SPDEF,
      BUTTON_DEFAULT_POWERUP_RANK_AGI,

      BUTTON_SWITCH_TRAINING_FLAG_HP,
      BUTTON_SWITCH_TRAINING_FLAG_ATTACK,
      BUTTON_SWITCH_TRAINING_FLAG_DEFENSE,
      BUTTON_SWITCH_TRAINING_FLAG_AGILITY,
      BUTTON_SWITCH_TRAINING_FLAG_SP_ATTACK,
      BUTTON_SWITCH_TRAINING_FLAG_SP_DEFENSE,
      BUTTON_NUM,
    };

    enum AnimeDataIndex {
      ANIME_DATA_ARROW_HOLD,
      ANIME_DATA_ARROW_RELEASE,
      ANIME_DATA_ARROW_SELECT,
      ANIME_DATA_SWITCH_HOLD,
      ANIME_DATA_SWITCH_RELEASE,
      ANIME_DATA_SWITCH_SELECT,
      ANIME_DATA_STRBOX_HOLD,
      ANIME_DATA_STRBOX_RELEASE,
      ANIME_DATA_STRBOX_SELECT,
      ANIME_DATA_NUMBOX_HOLD,
      ANIME_DATA_NUMBOX_RELEASE,
      ANIME_DATA_NUMBOX_SELECT,
      ANIME_DATA_NUM,
    };


    enum AnimeIndex {
      DUMMY_ANIME_ARROW_HOLD,
      DUMMY_ANIME_ARROW_RELEASE,
      DUMMY_ANIME_ARROW_SELECT,
      DUMMY_ANIME_SWITCH_HOLD,
      DUMMY_ANIME_SWITCH_RELEASE,
      DUMMY_ANIME_SWITCH_SELECT,
      DUMMY_ANIME_STRBOX_HOLD,
      DUMMY_ANIME_STRBOX_RELEASE,
      DUMMY_ANIME_STRBOX_SELECT,
      DUMMY_ANIME_NUMBOX_HOLD,
      DUMMY_ANIME_NUMBOX_RELEASE,
      DUMMY_ANIME_NUMBOX_SELECT,
      // 以下, 共有アニメーション
      ANIME_ARROW_HOLD,
      ANIME_ARROW_RELEASE  = ANIME_ARROW_HOLD     + ARROW_NUM,
      ANIME_ARROW_SELECT   = ANIME_ARROW_RELEASE  + ARROW_NUM,
      ANIME_SWITCH_HOLD    = ANIME_ARROW_SELECT   + ARROW_NUM,
      ANIME_SWITCH_RELEASE = ANIME_SWITCH_HOLD    + SWITCH_NUM,
      ANIME_SWITCH_SELECT  = ANIME_SWITCH_RELEASE + SWITCH_NUM,
      ANIME_STRBOX_HOLD    = ANIME_SWITCH_SELECT  + SWITCH_NUM,
      ANIME_STRBOX_RELEASE = ANIME_STRBOX_HOLD    + STRBOX_NUM,
      ANIME_STRBOX_SELECT  = ANIME_STRBOX_RELEASE + STRBOX_NUM,
      ANIME_NUMBOX_HOLD    = ANIME_STRBOX_SELECT  + STRBOX_NUM,
      ANIME_NUMBOX_RELEASE = ANIME_NUMBOX_HOLD    + NUMBOX_NUM,
      ANIME_NUMBOX_SELECT  = ANIME_NUMBOX_RELEASE + NUMBOX_NUM,
      ANIME_NUM            = ANIME_NUMBOX_SELECT  + NUMBOX_NUM,
      ANIME_NULL,
    };

    enum ShareAnimeIndex {
      SHARE_ANIME_ARROW_HOLD,
      SHARE_ANIME_ARROW_RELEASE,
      SHARE_ANIME_ARROW_SELECT,
      SHARE_ANIME_SWITCH_HOLD,
      SHARE_ANIME_SWITCH_RELEASE,
      SHARE_ANIME_SWITCH_SELECT,
      SHARE_ANIME_STRBOX_HOLD,
      SHARE_ANIME_STRBOX_RELEASE,
      SHARE_ANIME_STRBOX_SELECT,
      SHARE_ANIME_NUMBOX_HOLD,
      SHARE_ANIME_NUMBOX_RELEASE,
      SHARE_ANIME_NUMBOX_SELECT,
      SHARE_ANIME_NUM,
    };






    enum UpperAnimeDataIndex {
      UPPER_ANIME_DATA_START_NUM_DISPLAY,
      UPPER_ANIME_DATA_INFO_SCROLL,
      UPPER_ANIME_DATA_PAGE_INDEX,
      UPPER_ANIME_DATA_NUM,
    };

    enum UpperAnimeIndex 
    {
      UPPER_ANIME_START_NUM_DISPLAY,
      UPPER_ANIME_INFO_SCROLL,
      UPPER_ANIME_PAGE_INDEX,
      UPPER_ANIME_NUM,
      UPPER_ANIME_NULL,
    };





    struct ButtonDesc {
      gfl2::lyt::LytPaneIndex pane_index;
      AnimeIndex hold_anime;
      AnimeIndex release_anime;
      AnimeIndex select_anime;
    };

    static const gfl2::lyt::LytWkSetUp LAYOUT_WORK_SETUP_DATA;
    static const gfl2::lyt::LytWkSetUp LAYOUT_WORK_SETUP_DATA_UPPER;
#if 0
    static const gfl::grp::ProjectionParam PROJECTION_PARAM[2];
    static const gfl::grp::ViewParam VIEW_PARAM[2];
    static const gfl::grp::FragmentOperationParam OPERATION_PARAM;
#endif
    static const gfl2::lyt::LytArcIndex ANIME_DATA_TABLE[ ANIME_DATA_NUM ];
    static const gfl2::lyt::LytArcIndex ANIME_DATA_TABLE_UPPER[ UPPER_ANIME_DATA_NUM ];
#if 0
    static const app::util::LayoutContainer::ShareAnimeDesc SHARE_ANIME_TABLE[ SHARE_ANIME_NUM ];
#endif
    static const gfl2::lyt::LytPaneIndex PANE_INDEX_OF_WAZA_NAME[ pml::MAX_WAZA_NUM ];
    static const gfl2::lyt::LytPaneIndex PANE_INDEX_OF_WAZA_UP_VAL[ pml::MAX_WAZA_NUM ];
    static const gfl2::lyt::LytPaneIndex PANE_INDEX_OF_WAZA_PP_VAL[ pml::MAX_WAZA_NUM ];
    static const gfl2::lyt::LytPaneIndex PANE_INDEX_OF_WAZA_PP_MAX[ pml::MAX_WAZA_NUM ];
    static const gfl2::lyt::LytPaneIndex PANE_INDEX_OF_EGG_WAZA_NAME[ pml::MAX_WAZA_NUM ];
    static const ButtonDesc BUTTON_DESC[ BUTTON_NUM ];
    static const gfl2::str::STRCODE* SICK_NAME[ pml::pokepara::SICK_MAX ]; 
    static const gfl2::str::STRCODE* TYPE_NAME[ POKETYPE_MAX ];
    static const gfl2::str::STRCODE* SEX_NAME[ pml::SEX_NUM ]; 
    static const gfl2::str::STRCODE* LANGUAGE_NAME[ 12 ]; 
    static const gfl2::str::STRCODE* REGION_NAME[ REGION_MAX ];
    static const gfl2::str::STRCODE* VERSION_NAME[ 42 ]; 
    static const gfl2::str::STRCODE* DEFAULT_POWERUP_REASON_NAME[ DEFAULT_POWERUP_REASON_NUM ];

    gfl2::str::MsgData* m_msgData;
    print::WordSet* m_wordset;
    app::tool::ButtonManager* m_button_manager;
    Sequence m_seq; 
    NumInput* m_num_input;
    ButtonID m_number_input_target;
    ButtonID m_keyboad_input_target;
    gfl2::str::StrBuf* m_keyboard_buf;
    s32 m_keyboard_choice;
    u8 m_page;
    u8 m_page_old;
    bool     m_makeResult; ///< デバッグポケモン作成の結果を反映させるか？(true:はい  false:いいえ）

    AppRenderingPipeLine*        m_renderingPipeLine;
    gfl2::lyt::LytWk*            m_lytWk;
    gfl2::lyt::LytWk*            m_lytWk_Upper;
    gfl2::lyt::LytMultiResID     m_lytResID;
    void*                        m_lytDataBuffer;
    bool                         m_isDrawEnable;

    void CreateRenderingPipeLine( gfl2::heap::HeapBase* systemHeap, gfl2::heap::HeapBase* deviceHeap );
    void DeleteRenderingPipeLine( void );

    void OnStringInputFinished( ButtonID, const gfl2::str::StrBuf*, pml::pokepara::PokemonParam* );
    void OnNumberInputFinished( ButtonID, pml::pokepara::PokemonParam*, DefaultPowerUpDesc* );
    void UpdateAllOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateMonsNameOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateNickNameOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateFormNoOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateTypeOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateSexOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateSickOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEggOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateRareOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateSeikakuOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateItemOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateTokuseiOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateLevelOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateExpOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateHpOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateAtkOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateDefOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateAgiOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateSpatkOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateSpdefOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateAllWazaOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateWazaOnDisplay( const pml::pokepara::PokemonParam*, u32 ); 
    void UpdateFamiliarityOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateFriendshipOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateFeedOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdatePlayOnDisplay( const pml::pokepara::PokemonParam* );

    void UpdatePokerusOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEventOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateVersionOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateLanguageOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateOyaNameOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateOyaSexOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateCaptureBallOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateCaptureLevelOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEggTakenPlaceOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEggTakenYearOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEggTakenMonthOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateEggTakenDayOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateContactPlaceOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateContactYearOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateContactMonthOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateContactDayOnDisplay( const pml::pokepara::PokemonParam* );
    void UpdateMemoryCodeWithParent( const pml::pokepara::PokemonParam* );
    void UpdateMemoryLevelWithParent( const pml::pokepara::PokemonParam* );
    void UpdateMemoryDataWithParent( const pml::pokepara::PokemonParam* );
    void UpdateMemoryFeelWithParent( const pml::pokepara::PokemonParam* );
    void UpdateMemoryCodeWithOther( const pml::pokepara::PokemonParam* );
    void UpdateMemoryLevelWithOther( const pml::pokepara::PokemonParam* );
    void UpdateMemoryDataWithOther( const pml::pokepara::PokemonParam* );
    void UpdateMemoryFeelWithOther( const pml::pokepara::PokemonParam* );
    void UpdateRegion( const pml::pokepara::PokemonParam* );
    void UpdateCountryCode( const pml::pokepara::PokemonParam* );
    void UpdateCountryRegionCode( const pml::pokepara::PokemonParam* );
    void UpdateOtherFriendshipOnDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateOtherFamiliarityOnDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateOtherOyaNameOnDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdatePastOyaSexOnDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdatePastParentCountryOnDisplay( const pml::pokepara::PokemonParam* pokemon_param, int index );
    void UpdatePastParentAreaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param, int index );
    void UpdatePersonalRandDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateTrainerIdDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateRareRandomDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateTrimianHairDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateTrimianCountDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateTrimianCountMaxDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateTokuseiIndexSwitchTrans( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateConditionDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void UpdateEggWazaOnDisplay( const pml::pokepara::PokemonParam* pokemon_param,int idx );
    void UpdateDefaultPowerUpDisplay( const DefaultPowerUpDesc& desc );
    void UpdateTrainingFlagDisplay( const pml::pokepara::PokemonParam* pokemon_param );
    void SwitchTrainingFlag( pml::pokepara::PokemonParam* pokemon_param, pml::pokepara::PowerID power_id ) const;


    void StartNumberDisplayAnime( StringBoxIndex, u32 );
    void UpdatePage( void );
    pml::FormNo GetFormMax( const pml::pokepara::CoreParam *core );
    void CallNumInputPastCountry( pml::pokepara::PokemonParam* param, int index, NuminputParam *num_param );
    void CallNumInputPastArea( pml::pokepara::PokemonParam* param, int index, NuminputParam *num_param );
    void SetPastCountry( pml::pokepara::PokemonParam* pokemon_param, int index, u8 country );
    void SetPastArea( pml::pokepara::PokemonParam* pokemon_param, int index, u8 area );
    void CheckTokuseiNo( pml::pokepara::PokemonParam* pokemon_param );
    void PrintHighLowNumString( u32 value, int high_pane, int low_pane );
    u32 CorrectBeadsItemNo( u32 inputItemNo ) const;

    void PrintDebugMakeFlag( const pml::pokepara::PokemonParam * pp );

  };

} // namespace PokeMake
} // namespace debug 


#endif // __DEBUGPOKEMAKECORE_H__

#endif // PM_DEBUG
