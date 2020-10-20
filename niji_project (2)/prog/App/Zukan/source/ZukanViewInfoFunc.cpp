//==============================================================================
/**
 @file    ZukanViewInfoFunc.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の情報画面のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h>

#include <Sound/include/Sound.h>


// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/zukan/info.h>
#include <niji_conv_header/app/zukan/info_pane.h>
#include <niji_conv_header/app/zukan/info_anim_list.h>

#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zkn_type.h>
#include <niji_conv_header/message/msg_zkn_form.h>
#include <niji_conv_header/message/msg_zkn_height.h>
#include <niji_conv_header/message/msg_zkn_weight.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanType.h"
#include "ZukanContents.h"
#include "ZukanUtil.h"
#include "ZukanDataManager.h"
#include "ZukanPokeIconTextureManager.h"
#include "ZukanTool.h"
#include "ZukanFrameInfo.h"

#include "ZukanViewInfoFunc.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanViewInfoFunc
//
//==============================================================================
ZukanViewInfoFunc::ZukanViewInfoFunc(const ZukanSceneParam& param)
  : m_param(param),
    m_pokeNoTextPane(NULL),
    m_pokeNameTextPane(NULL),
    m_pokeSpeciesTextPane(NULL),
    m_formNameTextPane(NULL),
    m_heightLabelTextPane(NULL),
    m_weightLabelTextPane(NULL),
    m_heightValTextPane(NULL),
    m_weightValTextPane(NULL),
    m_commentTextPane(NULL),
    m_type1PicPane(NULL),
    m_type2PicPane(NULL)/*,
    m_pokeSimpleModelInFrame(NULL),
    m_pokeModelSp(),
    m_pokeModelCenterX(200),
    m_pokeModelCenterY(120),
    m_pokeModelBaseSize(200),
    m_pokeModelZoomDefault(1.0f),
    m_pokeModelZoomMin(0.5f),
    m_pokeModelZoomMax(1.2f),
    m_pokeModelPitchMax(1.5708f),
    m_pokeModelPitchMin(0.0f),
    m_pokeModelZoom(1.0f)//,
    //m_pokeModelYaw(0.0f),
    //m_pokeModelPitch(0.0f)
    */
{
  // タイプの番号とアルセウス、シルヴァディのフォルムの番号の並び順は同じ  // @fix GFNMCat[3114] シルヴァディのタイプアイコンが「ノーマル」のまま
  STATIC_ASSERT( POKETYPE_NORMAL  == FORMNO_ARUSEUSU_NORMAL   );
  STATIC_ASSERT( POKETYPE_KAKUTOU == FORMNO_ARUSEUSU_BATTLE   );  
  STATIC_ASSERT( POKETYPE_HIKOU   == FORMNO_ARUSEUSU_HIKOU    );
  STATIC_ASSERT( POKETYPE_DOKU    == FORMNO_ARUSEUSU_POISON   );
  STATIC_ASSERT( POKETYPE_JIMEN   == FORMNO_ARUSEUSU_JIMEN    );
  STATIC_ASSERT( POKETYPE_IWA     == FORMNO_ARUSEUSU_IWA      );
  STATIC_ASSERT( POKETYPE_MUSHI   == FORMNO_ARUSEUSU_MUSHI    );
  STATIC_ASSERT( POKETYPE_GHOST   == FORMNO_ARUSEUSU_GHOST    );
  STATIC_ASSERT( POKETYPE_HAGANE  == FORMNO_ARUSEUSU_METAL    );
  STATIC_ASSERT( POKETYPE_HONOO   == FORMNO_ARUSEUSU_FIRE     );
  STATIC_ASSERT( POKETYPE_MIZU    == FORMNO_ARUSEUSU_WATER    );
  STATIC_ASSERT( POKETYPE_KUSA    == FORMNO_ARUSEUSU_KUSA     );
  STATIC_ASSERT( POKETYPE_DENKI   == FORMNO_ARUSEUSU_ELECTRIC );
  STATIC_ASSERT( POKETYPE_ESPER   == FORMNO_ARUSEUSU_ESPER    );
  STATIC_ASSERT( POKETYPE_KOORI   == FORMNO_ARUSEUSU_KOORI    );
  STATIC_ASSERT( POKETYPE_DRAGON  == FORMNO_ARUSEUSU_DRAGON   );
  STATIC_ASSERT( POKETYPE_AKU     == FORMNO_ARUSEUSU_AKU      );
  STATIC_ASSERT( POKETYPE_FAIRY   == FORMNO_ARUSEUSU_FAIRY    );

  STATIC_ASSERT( POKETYPE_NORMAL  == FORMNO_GURIPUSU2_NORMAL   );
  STATIC_ASSERT( POKETYPE_KAKUTOU == FORMNO_GURIPUSU2_BATTLE   );  
  STATIC_ASSERT( POKETYPE_HIKOU   == FORMNO_GURIPUSU2_HIKOU    );
  STATIC_ASSERT( POKETYPE_DOKU    == FORMNO_GURIPUSU2_POISON   );
  STATIC_ASSERT( POKETYPE_JIMEN   == FORMNO_GURIPUSU2_JIMEN    );
  STATIC_ASSERT( POKETYPE_IWA     == FORMNO_GURIPUSU2_IWA      );
  STATIC_ASSERT( POKETYPE_MUSHI   == FORMNO_GURIPUSU2_MUSHI    );
  STATIC_ASSERT( POKETYPE_GHOST   == FORMNO_GURIPUSU2_GHOST    );
  STATIC_ASSERT( POKETYPE_HAGANE  == FORMNO_GURIPUSU2_METAL    );
  STATIC_ASSERT( POKETYPE_HONOO   == FORMNO_GURIPUSU2_FIRE     );
  STATIC_ASSERT( POKETYPE_MIZU    == FORMNO_GURIPUSU2_WATER    );
  STATIC_ASSERT( POKETYPE_KUSA    == FORMNO_GURIPUSU2_KUSA     );
  STATIC_ASSERT( POKETYPE_DENKI   == FORMNO_GURIPUSU2_ELECTRIC );
  STATIC_ASSERT( POKETYPE_ESPER   == FORMNO_GURIPUSU2_ESPER    );
  STATIC_ASSERT( POKETYPE_KOORI   == FORMNO_GURIPUSU2_KOORI    );
  STATIC_ASSERT( POKETYPE_DRAGON  == FORMNO_GURIPUSU2_DRAGON   );
  STATIC_ASSERT( POKETYPE_AKU     == FORMNO_GURIPUSU2_AKU      );
  STATIC_ASSERT( POKETYPE_FAIRY   == FORMNO_GURIPUSU2_FAIRY    );
  // 前提が崩れているといけないので、チェックしておく。
}
ZukanViewInfoFunc::~ZukanViewInfoFunc()
{}

void ZukanViewInfoFunc::SetPointer(gfl2::lyt::LytWk* lytWk_0, gfl2::lyt::LytMultiResID lytRes_0)
{
  ////////////////////////////////////////////////////////////
  // ペインのポインタを覚えておく
  ////////////////////////////////////////////////////////////
  // ポケモン、フォルム 
  m_pokeNoTextPane      = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_09);  // 025
  m_pokeNameTextPane    = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_10);  // ピカチュウ
  m_pokeSpeciesTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_00);  // ねずみポケモン
  m_formNameTextPane    = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_01);

  // たかさ、おもさラベル
  m_heightLabelTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_02);
  m_weightLabelTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_03);
  // たかさ、おもさ
  m_heightValTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_04);
  m_weightValTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_05);
  // コメント
  m_commentTextPane = lytWk_0->GetTextBoxPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_TEXTBOX_06);

  // タイプアイコン
  gfl2::lyt::LytParts* type1PartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_PARTSTYPE_00);
  gfl2::lyt::LytParts* type2PartsPane = lytWk_0->GetPartsPane(PANENAME_ZUKAN_MIN_LOW_002_PANE_PARTSTYPE_01);
  m_type1PicPane = lytWk_0->GetPicturePane(type1PartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &lytRes_0);
  m_type2PicPane = lytWk_0->GetPicturePane(type2PartsPane, PANENAME_COMMON_ICN_004_PANE_ICON_SKILLTYPE, &lytRes_0);
}

void ZukanViewInfoFunc::SetContents(
    app::util::G2DUtil*  pG2DUtil,
    gfl2::str::MsgData*  msgDataZukan,
    gfl2::str::MsgData*  msgDataPokeName,
    gfl2::str::MsgData*  msgDataPokeSpecies,
    gfl2::str::MsgData*  msgDataPokeForm,
    gfl2::str::MsgData*  msgDataPokeHeight,
    gfl2::str::MsgData*  msgDataPokeWeight,
    gfl2::str::MsgData*  msgDataPokeComment,
    ZukanType::ZukanLang zukanLangForType,
    gfl2::str::Font*     langFont
)
{
  const ZukanContents::PokeData* pokeData = m_param.contents->GetCurrDispPokeData();
  PokeTool::SimpleParam sp = m_param.contents->GetCurrDispPokeSimpleParam();
  u32 tableIndex = m_param.tool->GetPokeExtendData()->GetTableIndex(sp.monsNo, sp.formNo);


  ////////////////
  // テキスト
  ////////////////
  // ひょっとして中国語とか韓国語はフォントを読み直さないと出ない
  //////// 固定 ////////
  pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
  {
    // たかさ
    if(langFont) m_heightLabelTextPane->SetFont(langFont);
    pG2DUtil->SetTextBoxPaneString(m_heightLabelTextPane, msg_zkn_08_01);
    // おもさ
    if(langFont) m_weightLabelTextPane->SetFont(langFont);
    pG2DUtil->SetTextBoxPaneString(m_weightLabelTextPane, msg_zkn_08_02);
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  //////// monsNo固定 ////////
  pG2DUtil->SetMessageData( msgDataZukan, m_param.tool->GetWordSet() );
  {
    // 025
    if(langFont) m_pokeNoTextPane->SetFont(langFont);
    pG2DUtil->SetTextBoxPaneNumber(m_pokeNoTextPane, msg_zkn_01_02, pokeData->alolaNo, 3, 0, print::NUM_DISP_ZERO, print::NUM_CODE_HANKAKU);
    
    // ピカチュウ
    if(langFont) m_pokeNameTextPane->SetFont(langFont);
    if( msgDataPokeName == NULL )
    {
      pG2DUtil->SetRegisterMonsNameNo(0, sp.monsNo);
    }
    else
    {
      pG2DUtil->SetRegisterMonsNameMsgData(0, msgDataPokeName, sp.monsNo);
    }
    pG2DUtil->SetTextBoxPaneStringExpand(m_pokeNameTextPane, msg_zkn_03_03);
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  //////// monsNo固定(例外あり) ////////
  pG2DUtil->SetMessageData( msgDataPokeSpecies, m_param.tool->GetWordSet() );
  {
    // ねずみポケモン
    if(langFont) m_pokeSpeciesTextPane->SetFont(langFont);
    u32 msgIndex = ZKN_TYPE_000;
    if(pokeData->getFlag != 0)
    {
      msgIndex = ZKN_TYPE_000 + sp.monsNo;
    }
    if(msgIndex >= msgDataPokeSpecies->GetStrCount())
    {
      msgIndex = 0;
    }
    else
    {
      if(sp.monsNo == MONSNO_HUUPA)
      {
        if(sp.formNo == FORMNO_HUUPA_EARNEST) msgIndex = ZKN_TYPE_720_001;  // フーパのフォルム違いの○○ポケモン名のラベルを直書き。
      }
    }
    pG2DUtil->SetTextBoxPaneString(m_pokeSpeciesTextPane, msgIndex);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
 
  //////// formNoで異なる ////////
  pG2DUtil->SetMessageData( msgDataPokeForm, m_param.tool->GetWordSet() );
  {
    // フォルム名
    if(langFont) m_formNameTextPane->SetFont(langFont);
    u32 msgIndex = ZKN_FORM_000_000;
    if(pokeData->getFlag != 0)
    {
      msgIndex = ZKN_FORM_000_000 + tableIndex;
    }
    if(msgIndex >= msgDataPokeForm->GetStrCount()) msgIndex = 0;
    pG2DUtil->SetTextBoxPaneString(m_formNameTextPane, msgIndex);
  }
  pG2DUtil->SetMessageData(NULL, NULL);
         
  pG2DUtil->SetMessageData( msgDataPokeHeight, m_param.tool->GetWordSet() );
  {
    // たかさ値
    if(langFont) m_heightValTextPane->SetFont(langFont);
    u32 msgIndex = ZKN_HEIGHT_000_000;
    if(pokeData->getFlag != 0)
    {
      msgIndex = ZKN_HEIGHT_000_000 + tableIndex;
    }
    if(msgIndex >= msgDataPokeHeight->GetStrCount()) msgIndex = 0;
    pG2DUtil->SetTextBoxPaneString(m_heightValTextPane, msgIndex);
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  pG2DUtil->SetMessageData( msgDataPokeWeight, m_param.tool->GetWordSet() );
  {
    // おもさ値
    if(langFont) m_weightValTextPane->SetFont(langFont);
    u32 msgIndex = ZKN_WEIGHT_000_000;
    if(pokeData->getFlag != 0)
    {
      msgIndex = ZKN_WEIGHT_000_000 + tableIndex;
    }
    if(msgIndex >= msgDataPokeWeight->GetStrCount()) msgIndex = 0;
    pG2DUtil->SetTextBoxPaneString(m_weightValTextPane, msgIndex);
  }
  pG2DUtil->SetMessageData(NULL, NULL);

  pG2DUtil->SetMessageData( msgDataPokeComment, m_param.tool->GetWordSet() );
  {
    // コメント
    if(langFont) m_commentTextPane->SetFont(langFont);
    u32 msgIndex = 0;
    if(pokeData->getFlag != 0)
    {
      msgIndex = 0 + tableIndex;
    }
    if(msgIndex >= msgDataPokeComment->GetStrCount()) msgIndex = 0;
    pG2DUtil->SetTextBoxPaneString(m_commentTextPane, msgIndex);
  }
  pG2DUtil->SetMessageData(NULL, NULL);


  ////////////////
  // タイプアイコン
  ////////////////
  //////// formNoで異なる ////////
  // パーソナルデータ
  pml::personal::LoadPersonalData( sp.monsNo, sp.formNo );
  {
    u32 type1_u32 = POKETYPE_NULL;
    u32 type2_u32 = POKETYPE_NULL;
    //if(pokeData->getFlag != 0)  // nijiではタイプアイコンを伏せることはなくなったのでコメントアウト。
    {
      if( sp.monsNo == MONSNO_ARUSEUSU || sp.monsNo == MONSNO_GURIPUSU2 )
      {
        type1_u32 = type2_u32 = sp.formNo;  // タイプの番号とアルセウス、シルヴァディのフォルムの番号の並び順は同じ  // @fix GFNMCat[3114] シルヴァディのタイプアイコンが「ノーマル」のまま
      }
      else
      {
        type1_u32 = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type1);
        type2_u32 = pml::personal::GetPersonalParam(pml::personal::PARAM_ID_type2);
      }
    }
    m_param.tool->ReplaceTypeIconTextureByCommonGrpIcon(
        m_type1PicPane, static_cast<PokeType_tag>(type1_u32),
        m_type2PicPane, static_cast<PokeType_tag>(type2_u32),
        zukanLangForType
    );
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

