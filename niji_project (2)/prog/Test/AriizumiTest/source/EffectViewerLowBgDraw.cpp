//======================================================================
/**
 * @file EffectViewerLowBgDraw.cpp
 * @date 2015/12/03 14:40:33
 * @author kijima_satoshi
 * @brief エフェクトビューワー用描画クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG


#include <System/include/GflUse.h>
#include <GameSys/include/DllProc.h>
#include <Debug/DebugKeyboard/include/DebugKeyboard.h>

//レイアウト関連
#include <niji_conv_header/app/debug/effect_viewer/res2d/eff_view.h>
#include <niji_conv_header/app/debug/effect_viewer/res2d/eff_view_anim_list.h>
#include <niji_conv_header/app/debug/effect_viewer/res2d/eff_view_pane.h>

// テキスト関連
#include "arc_index/message.gaix"

#include "arc_index/debug_message.gaix"
#include <niji_conv_header/message/debug/msg_debug_poke_make.h>

#include "EffectViewerLowBgDraw.h"


GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( Ariizumi )



//----------------------------------------------------------------------
// Constructor & Destructor
//----------------------------------------------------------------------
  EffectViewerLowBgDraw::EffectViewerLowBgDraw( app::util::Heap * heap, app::util::AppRenderingManager* pAppRenderingManager ) :
  app::ui::UIView( heap, heap->GetSystemAllocator(), heap->GetDeviceAllocator() )
  ,mpAppHeap( heap )
  ,mUiListener( NULL )
{
  mpAppRenderingManager = pAppRenderingManager;
}

EffectViewerLowBgDraw::~EffectViewerLowBgDraw(void)
{
}

//-------------------------------------------------------------------------
/**
 * @brief 各システムの初期化
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::Init(EFFECT_APP_PARAM* pAppParam, void* pAppLytBuff)
{
  SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );
  CreateGraphicsSystem( mpAppHeap, false );
  Initialize2D( mpAppHeap,pAppLytBuff );
  InitializeText(mpAppHeap);
  SetInputListener( this );

  // For Paramater & Action
  mpEffViewUiInfo = pAppParam->mpEffViewUiInfo;

  // Init Layout (EffViewUiInfoセット後に)
  InitLayoutParam();
  UpdateLayoutDisp_ALL();

  // Init Keyboad
  mIsWaitKeyBoard = false;
  mKeyBoardChoice = -1;
  mKeyboardMsgId = -1;
  mpTmpHeap = pAppParam->mpTmpHeap;
  mKeyBoardBuf = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, mpAppHeap->GetDeviceHeap() );


}

//-------------------------------------------------------------------------
/**
 * @brief 各システムの後処理
 */
//-------------------------------------------------------------------------
bool EffectViewerLowBgDraw::End(void)
{
  FinalizeText();
  Delete2D();
  DeleteGraphicsSystem();

  // Keyboad
  GFL_DELETE  mKeyBoardBuf;

  return true;
}

//-------------------------------------------------------------------------
/**
 * @brief リスナーの設定
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetUIListener( EffectViewerLowBgListener *listener )
{
  mUiListener = listener;
}

//-------------------------------------------------------------------------
/**
 * @brief 2D描画とボタンマネージャの初期化
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::Initialize2D( app::util::Heap * heap, void* pAppLytBuff )
{
  // アーカイブIDテーブル
  app::util::G2DUtil::LytResData resTbl[] = {
    {  
      pAppLytBuff,                       //!< リソースデータ    // applytバイナリ
      2,                                 //!< 指定のリソースに追加登録するリソース数の最大数
      app::util::G2DUtil::ATTACH_ROOT,   //!< ０番目のリソースに連結して一括転送対象にするか
                                         //!< （ATTACH_ROOT=連結元,ATTACH_RESOURCE=連結する共通リソース
    },
  };
                                        

  app::util::G2DUtil::LytwkData setupData[] = {

    {
      0,                                       //!< 使用するリソースの番号 (レイアウトID？)
      LYTRES_EFF_VIEW_EFF_VIEW_BFLYT,          //!< レイアウトデータID ( *_BFLYT )
      LA_EFF_VIEW_EFF_VIEW___NUM,              //!< アニメ数
      pAppLytBuff,                             //!< リソースデータ
      false,                                    //!< 非表示ペインに行列計算を行うか
      app::util::G2DUtil::SETUP_LOWER,         //!< ディスプレイ
      true                                     //!< 表示設定
    },

  };


  Create2D( 
           heap, 
           NULL,                                 // レイアウトシステム
           1,                                    // レイアウト数
           resTbl,                               // レイアウトリソースデータ
           GFL_NELEMS(resTbl),                   // 上記配列の数
           setupData,                            // レイアウトデータ
           GFL_NELEMS(setupData),                // レイアウトデータ数
           //GARC_debug_message_d_btlbgviewer_DAT  // メッセージデータID
           UNUSE_MESSAGE_DATA                    // メッセージデータ不使用
            );


  CreateEffectViewerButtonManager();   // レイアウトの情報を使うので、レイアウトの初期化後に呼ぶ

}

//-------------------------------------------------------------------------
/**
 * @brief テキスト関連の初期化
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::InitializeText( app::util::Heap * pheap )
{

  mpMsgData = GFL_NEW(pheap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId_Debug(), GARC_debug_message_debug_poke_make_DAT, pheap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );


  mpTempStrBuf = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );
  mpTempStrBuf2 = GFL_NEW( pheap->GetDeviceHeap() ) gfl2::str::StrBuf( TEMP_STRBUF_LENGTH, pheap->GetDeviceHeap() );

  mpWordSet = GFL_NEW( pheap->GetDeviceHeap() ) print::WordSet( pheap->GetDeviceHeap() );


}

//-------------------------------------------------------------------------
/**
 * @brief テキスト関連のメモリ開放
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::FinalizeText(void)
{
  GFL_DELETE  mpMsgData;
  GFL_DELETE  mpTempStrBuf;
  GFL_DELETE  mpTempStrBuf2;
  GFL_DELETE  mpWordSet;
}



//--------------------------------------------------------------------------
/**
 * @brief   更新
 */
//--------------------------------------------------------------------------
void EffectViewerLowBgDraw::Update(void)
{

  if( mIsWaitKeyBoard )
    {
      if( mKeyBoardChoice > 0 )
        {
          if( mKeyboardMsgId == GARC_message_wazaname_DAT )
            {
              //有効な技が選ばれた
              UpdateLayoutDisp_WazaName( mKeyBoardChoice );
            }

          if( mKeyboardMsgId == GARC_message_monsname_DAT )
            {
              //有効なポケモンが選ばれた
              UpdateLayoutDisp_SelectPokemon( (MonsNo)mKeyBoardChoice );
            }
        }

      if( mKeyboardMsgId == GARC_message_trtype_DAT )
        {
          // トレーナータイプが選ばれた
          // idx=0があるので有効かどうかはUpdateの中でクリップ
          UpdateLayoutDisp_TrainerType( mKeyBoardChoice );
        }

    
      mIsWaitKeyBoard = false;
    }

  // フレーム数表示の更新
  UpdateLayoutDisp_Frame( mpEffViewUiInfo->mDispFrame );

  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->PrintMessage();
  app::ui::UIView::Update();
}

//--------------------------------------------------------------------------
/**
 * @brief   描画
 */
//--------------------------------------------------------------------------
void EffectViewerLowBgDraw::Draw(gfl2::gfx::CtrDisplayNo no)
{
  app::util::G2DUtil* pG2DUtil  = UIView::GetG2DUtil();
  pG2DUtil->AddDrawableLytWkForOneFrame( mpAppRenderingManager, no, 0 );
}


//------------------------------------------------------------------------------
/**
 * @brief   ボタンマネージャの生成
 *
 * レイアウトの情報を使うので、レイアウトの初期化後に呼ぶ
 */
//--------------------------------------------------------------------------
void EffectViewerLowBgDraw::CreateEffectViewerButtonManager( void )
{
  //  static const int partsnum = 2;
  static const int layoutno = 0;


  // タッチ可能なペインの登録
  u32 paneID[] =
    {

      // BGM
      PANENAME_EFF_VIEW_PANE_BGM_PLAY,

      // Debug Print
      PANENAME_EFF_VIEW_PANE_FLG_SET_1_BUT,

      // Resources
      //      PANENAME_EFF_VIEW_PANE_RES_DEL_HIT,

      // Page
      PANENAME_EFF_VIEW_PANE_PAGE_U,
      PANENAME_EFF_VIEW_PANE_PAGE_D,
 
      // Waza
      PANENAME_EFF_VIEW_PANE_WAZA_PLAY,
      
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_UP_00,
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_UP_01,
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_UP_02,
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_DOWN_00,
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_DOWN_01,
      //      PANENAME_EFF_VIEW_PANE_NO_SEL_DOWN_02,

      PANENAME_EFF_VIEW_PANE_NO_SEL_SUB_UP,
      PANENAME_EFF_VIEW_PANE_NO_SEL_SUB_DOWN,

      PANENAME_EFF_VIEW_PANE_WAZA_HIT,


      // Pokemon No.
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_1,
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_2,
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_3,
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_1,
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_2,
      //      PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_3,

      // --- Pokemon Select ---
      PANENAME_EFF_VIEW_PANE_POKE_SEL_HIT,


      // Pokemon form
      PANENAME_EFF_VIEW_PANE_POKE_SEL_F_UP,
      PANENAME_EFF_VIEW_PANE_POKE_SEL_F_DOWN,

      // Battle Target
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_00,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_01,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_02,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_03,
      //      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_04,
      //      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_05,

      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_00,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_01,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_02,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_03,
      //      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_04,
      //      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_05,

      // バトルルール
      PANENAME_EFF_VIEW_PANE_RULE_HIT,
      PANENAME_EFF_VIEW_PANE_RULE_BTN,

      // 技範囲
      PANENAME_EFF_VIEW_PANE_RULE_RANGE_HIT,

      // Pokemon set
      PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_1,
      PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_2,
      PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_3,
      PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_4,

      // Rare
      PANENAME_EFF_VIEW_PANE_POKE_SEL_RARE,

      // Sex
      PANENAME_EFF_VIEW_PANE_POKE_SEL_SEX,

      // --- Page 2 ---

      PANENAME_EFF_VIEW_PANE_TR_ID_LBL_HIT,

      PANENAME_EFF_VIEW_PANE_TR_ID_UP_00,
      PANENAME_EFF_VIEW_PANE_TR_ID_UP_01,
      PANENAME_EFF_VIEW_PANE_TR_ID_UP_02,
      PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_00,
      PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_01,
      PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_02,

      // Ball ID
      PANENAME_EFF_VIEW_PANE_BALL_ID_U_0,
      PANENAME_EFF_VIEW_PANE_BALL_ID_D_0,
      PANENAME_EFF_VIEW_PANE_BALL_ID_U_1,
      PANENAME_EFF_VIEW_PANE_BALL_ID_D_1,
      PANENAME_EFF_VIEW_PANE_BALL_ID_SET,


    };

  gfl2::lyt::LytWk* pLytWk = UIView::GetLayoutWork( layoutno );

  // PaneIDのみ版
  this->CreateButtonManager( mpAppHeap, pLytWk, paneID, GFL_NELEMS(paneID) );

}

//--------------------------------------------------------------------------
/**
 * @brief   破棄チェック
 * @return  trueで破棄完了
 */
//--------------------------------------------------------------------------
bool EffectViewerLowBgDraw::IsExit( void )
{
  return true;
}


//------------------------------------------------------------------
/**
 * @brief   キー入力の検知
 *
 * @param  pKey    十字キー
 * @param  pStick  アナログスティック
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult EffectViewerLowBgDraw::OnKeyAction(  gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStic )
{
  if( mUiListener == NULL ){
    return ENABLE_ACCESS;
  }

  return ENABLE_ACCESS;
}

//-------------------------------------------------------------------------
/**
 * @brief レイアウトから取得するパラメータの初期化
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::InitLayoutParam ()
{
  // 参照回数が多そうなので
  static const int layoutno = 0;
  mUILytWk = UIView::GetLayoutWork( layoutno );

  mUI_Page = 0;

  mUI_IsDebugPrint = false;
  mUI_IsPlayBGM = false;
  mUI_ResourceNum = 0;

  mUI_SelectWazaNum = WAZANO_HATAKU;
  mUI_SelectWazaSubNum = 0;

  mUI_BattleRuleIdx = 0;
  mUI_BattleRangeIdx = 0;

  mUI_TrainerType = 0;

  mUI_AttackPokeIdx = 0;
  mUI_DefencePokeIdx = 1;

  //  mUI_PokemonNum = 2;

  mUI_Ball_ID[0] = 4;
  mUI_Ball_ID[1] = 4;

  //  mUI_NoSelID = 0;

  // -------------------------------------------
  // 上画面クラスの値で初期化するパラメーター

  // Pokemon
  int tidx = 0;

  // 選択中のポケモン
  StoreSelectPokemonParam (mpEffViewUiInfo->mUI_Pp[tidx]);

  // Pokemon List
  for (int i=0; i<EffectViewerUiInfo::POKEMON_MAX_NUM; i++)
    {
      mUI_PokemonList[i] = mpEffViewUiInfo->mUI_Pp[i]->GetMonsNo();
    }


  // Set status
  UpdateLayoutDisp_BgmStat( mUI_IsPlayBGM );
  SetDebugPrintStat( mUI_IsDebugPrint );

}

//-------------------------------------------------------------------------
/**
 * @brief 指定TextBoxへのポケモン名のセット
 *
 * @param[in] textBox : セットするテキストボックスのポインタ
 * @param[in] no : モンスターNo.
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetTextBoxToPokeName( gfl2::lyt::LytTextBox *textBox, MonsNo no )
{

  mpMsgData->GetString( label_waza_name, *mpTempStrBuf );
  mpWordSet->RegisterPokeMonsNameNo( 0, (MonsNo)no );
  mpWordSet->Expand( mpTempStrBuf2, mpTempStrBuf );
  
  textBox->SetString(mpTempStrBuf2->GetPtr());  

}


//-------------------------------------------------------------------------
/**
 * @brief 指定TextBoxへのトレーナータイプ名のセット
 *
 * @param[in] textBox : セットするテキストボックスのポインタ
 * @param[in] type : トレーナータイプ
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetTextBoxToTrainerTypeName( gfl2::lyt::LytTextBox *textBox, int type )
{

  mpMsgData->GetString( label_waza_name, *mpTempStrBuf );
  mpWordSet->RegisterTrTypeName( 0, type );
  mpWordSet->Expand( mpTempStrBuf2, mpTempStrBuf );



  int tb_sl = textBox->GetStringBufferLength();
  //  int dsl = textBox->GetDrawStringBufferLength();
  int str_len = mpTempStrBuf2->GetLength();

  // 文字列バッファが足りない場合は再確保
  if ( tb_sl < str_len )
    {
      textBox->AllocStringBuffer(str_len);
    }

  textBox->SetString(mpTempStrBuf2->GetPtr());  

}


//-------------------------------------------------------------------------
/**
 * @brief 指定TextBoxへの数字のセット
 *
 * @param[in] textBox : セットするテキストボックスのポインタ
 * @param[in] num : セットする数値
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetTextBoxToNumber( gfl2::lyt::LytTextBox *textBox, int num )
{
  wchar_t workStr[16];

  swprintf( workStr, 16, L"%d", num);
  textBox->SetString( workStr );
}

//-------------------------------------------------------------------------
/**
 * @brief ボールIDの更新
 *
 * @param[in] slot : セットするスロット
 * @param[in] ball_idx : リストのインデックス
 */
//-------------------------------------------------------------------------
MonsterBallData BALL_DATA[] =
  {
    {L"ヌルボール",        item::BALLID_NULL},          // Dummy
    {L"マスターボール",    item::BALLID_MASUTAABOORU},
    {L"ハイパーボール",    item::BALLID_HAIPAABOORU},
    {L"スーパーボール",    item::BALLID_SUUPAABOORU},
    {L"モンスターボール",  item::BALLID_MONSUTAABOORU},
    {L"サファリボール",    item::BALLID_SAFARIBOORU},
    {L"ネットボール",      item::BALLID_NETTOBOORU},
    {L"ダイブボール",      item::BALLID_DAIBUBOORU},
    {L"ネストボール",      item::BALLID_NESUTOBOORU},
    {L"リピートボール",    item::BALLID_RIPIITOBOORU},
    {L"タイマーボール",    item::BALLID_TAIMAABOORU},
    {L"ゴージャスボール",  item::BALLID_GOOZYASUBOORU},
    {L"プレミアボール",    item::BALLID_PUREMIABOORU},
    {L"ダークボール",      item::BALLID_DAAKUBOORU},
    {L"ヒールボール",      item::BALLID_HIIRUBOORU},
    {L"クイックボール",    item::BALLID_KUIKKUBOORU},
    {L"プレシャスボール",  item::BALLID_PURESYASUBOORU},
    {L"スピードボール",    item::BALLID_SUPIIDOBOORU},
    {L"レベルボール",      item::BALLID_REBERUBOORU},
    {L"ルアーボール",      item::BALLID_RUAABOORU},
    {L"ヘビーボール",      item::BALLID_HEBIIBOORU},
    {L"ラブラブボール",    item::BALLID_RABURABUBOORU},
    {L"フレンドボール",    item::BALLID_HURENDOBOORU},
    {L"ムーンボール",      item::BALLID_MUUNBOORU},
    {L"コンペボール",      item::BALLID_KONPEBOORU},
    {L"ドリームボール",    item::BALLID_DORIIMUBOORU},
    {L"ウルトラボール",    item::BALLID_URUTORABOORU},
  };


void EffectViewerLowBgDraw::UpdateLayoutDisp_Ball_ID( int slot, int ball_idx )
{
  // Min-Max check
  int max = GFL_NELEMS(BALL_DATA);
  if(ball_idx >= max)
    {
      ball_idx = (max-1);
    }

  if(ball_idx < 1)
    {
      ball_idx = 1;
    }

  // Store
  mUI_Ball_ID[slot] = ball_idx;

  gfl2::lyt::LytTextBox *textBox_name;
  gfl2::lyt::LytTextBox *textBox_id;

  if (slot == 0)
    {
      textBox_id = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_BALL_ID_NO_0 );
      textBox_name = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_BALL_ID_LBL_0 );
    }
  else
    {
      textBox_id = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_BALL_ID_NO_1 );
      textBox_name = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_BALL_ID_LBL_1 );
    }

  // Set Info
  SetTextBoxToNumber ( textBox_id, BALL_DATA[ball_idx].ball_num );
  textBox_name->SetString( BALL_DATA[ball_idx].ball_str );  
}

//-------------------------------------------------------------------------
/**
 * @brief Ball ID のセット
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetAction_SetBall_ID(void)
{

  mpEffViewUiInfo->mBall_ID[0] = mUI_Ball_ID[0];
  mpEffViewUiInfo->mBall_ID[1] = mUI_Ball_ID[1];

  // Set action
  mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_BALL_ID_SET );

}

//-------------------------------------------------------------------------
/**
 * @brief トレーナータイプの更新
 *
 * @param[in] トレーナータイプ
 */
//-------------------------------------------------------------------------
void  EffectViewerLowBgDraw::UpdateLayoutDisp_TrainerType( int tr_type )
{

  const int tr_type_max = 111;  // hard code

  // Min-Max check
  if(tr_type > tr_type_max)
    {
      tr_type = tr_type_max;
    }

  if(tr_type < 0)
    {
      tr_type = 0;
    }

  // Store
  mUI_TrainerType = tr_type;


  // ナンバーセット
  gfl2::lyt::LytTextBox *textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_TR_ID_NO );
  SetTextBoxToNumber (textBox, tr_type);

  // Update Trainer Name
  textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_TR_ID_LBL );
  SetTextBoxToTrainerTypeName( textBox, tr_type );

}


//-------------------------------------------------------------------------
/**
 * @brief 選択中のポケモンの初期化
 *
 * 上画面から渡されたパラメーターで初期化している
 */
//-------------------------------------------------------------------------

void EffectViewerLowBgDraw::StoreSelectPokemonParam (pml::pokepara::PokemonParam *poke_param)
{
  mUI_SelectPokemonNum = poke_param->GetMonsNo();
  mUI_SelectPokemonFormNum = poke_param->GetFormNo();
  mUI_SelectPokemonRare = poke_param->IsRare();
  mUI_SelectPokemonSex = poke_param->GetSex();
}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のポケモンの更新
 *
 * @param[in] ポケモン番号
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_SelectPokemon( MonsNo poke_num )
{

  // Min-Max check
  poke_num = (gfl2::math::Clamp( poke_num , MONSNO_HUSIGIDANE, MONSNO_MAX ));

  // Store
  mUI_SelectPokemonNum = poke_num;
  
  // Update Pokemon Number
  gfl2::lyt::LytTextBox *textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_POKE_SEL_NO );
  SetTextBoxToNumber (textBox, poke_num);


  // Update Pokemon Name
  textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_POKE_SEL_NAME );
  SetTextBoxToPokeName( textBox, poke_num );


  // Init other param
  UpdateLayoutDisp_PokemonForm( 0 );
  UpdateLayoutDisp_PokemonSex( 0 );
  UpdateLayoutDisp_PokemonRare( false );

}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のポケモンのフォームの更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_PokemonForm( int poke_form )
{
  // Min-Max check
  if(poke_form > 99)
    {
      poke_form = 99;
    }

  if(poke_form < 0)
    {
      poke_form = 0;
    }

  // ナンバーセット
  gfl2::lyt::LytTextBox *textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_POKE_SEL_F_NO );
  SetTextBoxToNumber (textBox, poke_form);

  // Store
  mUI_SelectPokemonFormNum = poke_form;

}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のポケモンの性別の更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_PokemonSex( int idx )
{

  static const int layoutno = 0;
  gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
  const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
  const nw::lyt::TextureInfo *info;

  mUI_SelectPokemonSex = idx;

  // 性別無しは考慮していない
  if ( idx == 1 )
    {
      // Female
      info = res->GetTexture( LYTRES_EFF_VIEW_DOT_BFLIM, rMultiResId.GetMultiID() );
    }
  else
    {
      // Male
      info = res->GetTexture( LYTRES_EFF_VIEW_EMPTY_BFLIM, rMultiResId.GetMultiID() );
    }

  gfl2::lyt::LytPicture * pPicPane = mUILytWk->GetPicturePane( PANENAME_EFF_VIEW_PANE_POKE_SEL_SEX );
  pPicPane->GetMaterial(0)->SetTexMap(0,info);

}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のポケモンのレア属性の更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_PokemonRare( bool flg )
{
  static const int layoutno = 0;
  gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
  const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
  const nw::lyt::TextureInfo *info;

  mUI_SelectPokemonRare = flg;

  if ( flg )
    {
      // Flag on : Rare
      info = res->GetTexture( LYTRES_EFF_VIEW_DOT_BFLIM, rMultiResId.GetMultiID() );
    }
  else
    {
      // Flag off : Normal
      info = res->GetTexture( LYTRES_EFF_VIEW_EMPTY_BFLIM, rMultiResId.GetMultiID() );
    }

  gfl2::lyt::LytPicture * pPicPane = mUILytWk->GetPicturePane( PANENAME_EFF_VIEW_PANE_POKE_SEL_RARE );
  pPicPane->GetMaterial(0)->SetTexMap(0,info);
}


//-------------------------------------------------------------------------
/**
 * @brief リソース表示の更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_ResourceNum( int num )
{

  mUI_ResourceNum = num;

  // ナンバーセット
  gfl2::lyt::LytTextBox *textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_RES_DEL_NO );
  SetTextBoxToNumber (textBox, num);


}


//-------------------------------------------------------------------------
/**
 * @brief フレーム数表示の更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_Frame( int frm )
{
  gfl2::lyt::LytTextBox *textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_FRAME_NO );
  SetTextBoxToNumber (textBox, frm);
}

//-------------------------------------------------------------------------
/**
 * @brief レイアウト表示の全更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_ALL ()
{
  UpdateLayoutDisp_Ball_ID( 0, mUI_Ball_ID[0] );
  UpdateLayoutDisp_Ball_ID( 1, mUI_Ball_ID[1] );
  UpdateLayoutDisp_TrainerType( mUI_TrainerType  );

  UpdateLayoutDisp_SelectPokemon( mUI_SelectPokemonNum );
  UpdateLayoutDisp_PokemonForm( mUI_SelectPokemonFormNum );
  UpdateLayoutDisp_PokemonSex( mUI_SelectPokemonSex );
  UpdateLayoutDisp_PokemonRare( mUI_SelectPokemonRare );

  UpdateLayoutDisp_WazaName( mUI_SelectWazaNum );
  UpdateLayoutDisp_WazaSubNumber( mUI_SelectWazaSubNum );

  UpdateLayoutDisp_BattleRule( mUI_BattleRuleIdx );
  UpdateLayoutDisp_BattleRange( mUI_BattleRangeIdx );

  UpdateLayoutDisp_ResourceNum( mUI_ResourceNum );

  UpdateLayoutDisp_AttackSide( mUI_AttackPokeIdx );
  UpdateLayoutDisp_DeffenceSide( mUI_DefencePokeIdx );

  UpdateLayoutDisp_PokemonList ();

}

//-------------------------------------------------------------------------
/**
 * @brief レイアウトページ切り替え
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::ChangeLayoutPage( int page )
{
  const int page_max = 1;

  if( page > page_max )
    {
      page = 0;
    }

  if( page < 0 )
    {
      page = page_max;
    }

  mUI_Page = page;

  // Move page
  {
    static const int layoutno = 0;
    gfl2::math::VEC3 pos;

    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
    gfl2::lyt::LytPaneIndex pi = PANENAME_EFF_VIEW_PANE_MOVE_BASE;

    pos = pG2DUtil->GetPanePos( layoutno, pi );
    pos.x = (mUI_Page * -320);
    pG2DUtil->SetPanePos( layoutno, pi, &pos );
  }
  
}


//-------------------------------------------------------------------------
/**
 * @brief ポケモンのレア属性チェックボックスのクリック処理
 */
//-------------------------------------------------------------------------
void  EffectViewerLowBgDraw::ClickPokemonRare(void)
{
  if ( mUI_SelectPokemonRare )
    {
      // Normal
      UpdateLayoutDisp_PokemonRare( false );
    }
  else
    {
      // Rare
      UpdateLayoutDisp_PokemonRare( true );
    }
} 

//-------------------------------------------------------------------------
/**
 * @brief ポケモンの性別チェックボックスのクリック処理
 */
//-------------------------------------------------------------------------
void  EffectViewerLowBgDraw::ClickPokemonSex(void)
{
  if ( mUI_SelectPokemonSex == 1 )
    {
      // Male
      UpdateLayoutDisp_PokemonSex( 0 );
    }
  else
    {
      // Female
      UpdateLayoutDisp_PokemonSex( 1 );
    }
} 

//-------------------------------------------------------------------------
/**
 * @brief BGMチェックボックスのクリック処理
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::ClickBgmPlay(void)
{
  if ( mUI_IsPlayBGM )
    {
      // Pause
      UpdateLayoutDisp_BgmStat (false);
      mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_BGM_STOP );
    }
  else
    {
      // Play
      UpdateLayoutDisp_BgmStat (true);
      mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_BGM_PLAY );
    }
}

//-------------------------------------------------------------------------
/**
 * @brief BGM再生ボタンの更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_BgmStat(bool st)
{
  static const int layoutno = 0;
  gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
  const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
  const nw::lyt::TextureInfo *info;

  if ( st )
    {
      // Status:Play
      mUI_IsPlayBGM = true;
      info = res->GetTexture( LYTRES_EFF_VIEW_PAUSE_BFLIM, rMultiResId.GetMultiID() ); // 表示はPause
    }
  else
    {
      // Status:Pause
      info = res->GetTexture( LYTRES_EFF_VIEW_PLAY_BFLIM, rMultiResId.GetMultiID() ); // 表示はPlay
      mUI_IsPlayBGM = false;
    }


  gfl2::lyt::LytPicture * pPicPane = mUILytWk->GetPicturePane( PANENAME_EFF_VIEW_PANE_BGM_PLAY );
  pPicPane->GetMaterial(0)->SetTexMap(0,info);

}

//-------------------------------------------------------------------------
/**
 * @brief デバッグプリントボタンのクリック処理
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::ClickDebugPrint(void)
{

  if ( mUI_IsDebugPrint )
    {
      // OFF
      SetDebugPrintStat (false);
      mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_DEBUG_PRINT_OFF );
    }
  else
    {
      // ON
      SetDebugPrintStat (true);
      mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_DEBUG_PRINT_ON );
    }

}

//-------------------------------------------------------------------------
/**
 * @brief デバッグプリント切り替えボタンの更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetDebugPrintStat(bool st)
{

  static const int layoutno = 0;
  gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
  const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
  const nw::lyt::TextureInfo *info;

  if ( st )
    {
      // Status:ON
      mUI_IsDebugPrint = true;
      info = res->GetTexture( LYTRES_EFF_VIEW_OFF_BFLIM, rMultiResId.GetMultiID() ); // 表示はOFF
    }
  else
    {
      // Status:OFF
      info = res->GetTexture( LYTRES_EFF_VIEW_ON_BFLIM, rMultiResId.GetMultiID() ); // 表示はON
      mUI_IsDebugPrint = false;
    }


  gfl2::lyt::LytPicture * pPicPane = mUILytWk->GetPicturePane( PANENAME_EFF_VIEW_PANE_FLG_SET_1_BUT );
  pPicPane->GetMaterial(0)->SetTexMap(0,info);


}

//-------------------------------------------------------------------------
/**
 * @brief 入力用キーボードの呼び出し
 *
 * @param[in] : 使用するメッセージセットのARC ID
 *
 * 別プロセスで走るので注意
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::CallKeyBoard( s32 msgId )
{
  mIsWaitKeyBoard = true;
  mKeyBoardBuf->Clear();

  debug::KeyboardProc* proc = GameSys::CallDebugKeyboardProc();
  proc->SetupParam( mpTmpHeap, mpTmpHeap, mKeyBoardBuf, TEMP_STRBUF_LENGTH, 0 );
  proc->SetDefaultInputMode( debug::KeyboardProc::INPUT_HIRAGANA );
  proc->SetLayoutSystem( UIView::GetG2DUtil()->GetLayoutSystem() );
  if( 0 <= msgId ) 
    {
      proc->SetSearchWords( msgId, &mKeyBoardChoice );
    }

  mKeyboardMsgId = msgId;
}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のワザ名の更新
 *
 * @param[in] waza_no : ワザ番号
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_WazaName( u32 waza_no )
{

  if( (waza_no > WAZANO_NULL) && (waza_no < WAZANO_MAX) )
    {

      mUI_SelectWazaNum = waza_no;


      mpMsgData->GetString( label_waza_name, *mpTempStrBuf );
      mpWordSet->RegisterWazaName( 0, waza_no );
      mpWordSet->Expand( mpTempStrBuf2, mpTempStrBuf );

      // Waza Name
      gfl2::lyt::LytTextBox *textBox;
      textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_WAZA_NAME );
      textBox->SetString(mpTempStrBuf2->GetPtr());  

      // Waza No.
      textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_NO_SEL_NO );
      SetTextBoxToNumber( textBox, waza_no );

    }

}

//-------------------------------------------------------------------------
/**
 * @brief 選択中のワザのサブナンバーの更新
 *
 * @param[in] waza_no : ワザ番号
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_WazaSubNumber( int sub_no )
{
  if( (sub_no < 0) || (sub_no > 9) )
    {
      return;
    }

  mUI_SelectWazaSubNum = sub_no;
  mpEffViewUiInfo->mWazaSubNo = mUI_SelectWazaSubNum;

  // Waza Sub No.
  gfl2::lyt::LytTextBox *textBox;
  textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_NO_SEL_SUB_NO );
  SetTextBoxToNumber( textBox, sub_no );
  
}

//-------------------------------------------------------------------------
/**
 * @brief ポケモン表示リストの更新
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_PokemonList(void)
{

  u32 ListPaneID[EffectViewerUiInfo::POKEMON_MAX_NUM] = 
    {
      PANENAME_EFF_VIEW_PANE_POKE_NAME_00,
      PANENAME_EFF_VIEW_PANE_POKE_NAME_01,
      PANENAME_EFF_VIEW_PANE_POKE_NAME_02,
      PANENAME_EFF_VIEW_PANE_POKE_NAME_03,
    };

  gfl2::lyt::LytTextBox *textBox;

  for (int i=0;i<EffectViewerUiInfo::POKEMON_MAX_NUM;i++)
    {
      MonsNo mn = mUI_PokemonList[i];
      textBox = mUILytWk->GetTextBoxPane( ListPaneID[i] );
      SetTextBoxToPokeName(textBox, mn);
    }
}

//-------------------------------------------------------------------------
/**
 * @brief 上画面へのポケモン設定アクション
 *
 * @param[in] slot : セットする位置
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetAction_SetPokemon( int slot )
{

  mpEffViewUiInfo->mTargetSlot = slot;          // セットするスロット番号

  // セットするポケモン情報
  mpEffViewUiInfo->mSelectPokemonNum = mUI_SelectPokemonNum;
  mpEffViewUiInfo->mSelectPokemonForm = mUI_SelectPokemonFormNum;
  mpEffViewUiInfo->mSelectPokemonRare = mUI_SelectPokemonRare;
  mpEffViewUiInfo->mSelectPokemonSex = mUI_SelectPokemonSex;

  // Set action
  mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_POKEMON_SET );

  // Update List
  mUI_PokemonList[slot] = mUI_SelectPokemonNum;
  UpdateLayoutDisp_PokemonList();

}

//-------------------------------------------------------------------------
/**
 * @brief 選択したワザの再生
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetAction_PlayWaza()
{

  mpEffViewUiInfo->mWazaNo = mUI_SelectWazaNum;
  mpEffViewUiInfo->mWazaSubNo = mUI_SelectWazaSubNum;

  // ターゲット情報
  mpEffViewUiInfo->mRangeType = mUI_BattleRangeIdx;
  mpEffViewUiInfo->mAttackPos = mUI_AttackPokeIdx;
  mpEffViewUiInfo->mDeffencePos = mUI_DefencePokeIdx;

  // Set action
  mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_WAZA_PLAY);

}

//-------------------------------------------------------------------------
/**
 * @brief バトルルールの更新
 *
 * @param[in] rule_idx : ルールリストのインデックス
 */
//-------------------------------------------------------------------------

BattleRuleData RULE_DATA [] = 
  {
    {L"シングル",  BTL_RULE_SINGLE},
    {L"ダブル",  BTL_RULE_DOUBLE},
    {L"バトルロイヤル",  BTL_RULE_ROYAL},
    {L"乱入",  BTL_RULE_INTRUDE},
  };

void EffectViewerLowBgDraw::UpdateLayoutDisp_BattleRule( int rule_idx )
{
  // Min-Max check
  int max_num = GFL_NELEMS(RULE_DATA);
  if (rule_idx < 0)
    {
      rule_idx = (max_num-1);
    }

  if (rule_idx >= max_num)
    {
      rule_idx = 0;
    }

  mUI_BattleRuleIdx = rule_idx;

  gfl2::lyt::LytTextBox *textBox;
  textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_RULE_STR );
  textBox->SetString( RULE_DATA[rule_idx].rule_str );  

}

//-------------------------------------------------------------------------
/**
 * @brief 上画面へのバトルルール設定アクション
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetAction_SetBattleRule(void)
{

  mpEffViewUiInfo->mBattleRule = (BtlRule)mUI_BattleRuleIdx;

  // Set action
  mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_BTL_RULE_SET );

  // 選択を初期化
  UpdateLayoutDisp_AttackSide( 0 );
  UpdateLayoutDisp_DeffenceSide( 1 );

}

//-------------------------------------------------------------------------
/**
 * @brief 攻撃側選択表示の更新
 *
 * @param[in] check_idx : 選択されているチェックボックス
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_AttackSide( int check_idx )
{

  int max = EffectViewerUiInfo::POKEMON_MAX_NUM;

  u32 ListPaneID[EffectViewerUiInfo::POKEMON_MAX_NUM] = 
    {
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_00,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_01,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_02,
      PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_03,
    };

  if ((check_idx >=0 ) && (check_idx < max)) 
    {

      mUI_AttackPokeIdx = check_idx;

      static const int layoutno = 0;
      gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
      const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
      const nw::lyt::TextureInfo *info;
      gfl2::lyt::LytPicture *pPicPane;


      for (int i=0; i<max; i++)
        {
          if (i == check_idx)
            {
              info = res->GetTexture( LYTRES_EFF_VIEW_DOT_BFLIM, rMultiResId.GetMultiID() );
            }
          else
            {
              info = res->GetTexture( LYTRES_EFF_VIEW_EMPTY_BFLIM, rMultiResId.GetMultiID() );
            }

          pPicPane = mUILytWk->GetPicturePane( ListPaneID[i] );
          pPicPane->GetMaterial(0)->SetTexMap(0,info);

          pPicPane->SetVisible (true);

          if( (RULE_DATA[mUI_BattleRuleIdx].rule_num == BTL_RULE_INTRUDE) && (i == 2) )
            {
              pPicPane->SetVisible (false);
            }

          if( (RULE_DATA[mUI_BattleRuleIdx].rule_num == BTL_RULE_SINGLE) && (i >= 2) )
            {
              pPicPane->SetVisible (false);
            }

        }

    }
  
}

//-------------------------------------------------------------------------
/**
 * @brief 防御側選択表示の更新
 *
 * @param[in] check_idx : 選択されているチェックボックス
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_DeffenceSide( int check_idx )
{

  int max = EffectViewerUiInfo::POKEMON_MAX_NUM;

  u32 ListPaneID[EffectViewerUiInfo::POKEMON_MAX_NUM] = 
    {
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_00,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_01,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_02,
      PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_03,
    };

  if ((check_idx >=0 ) && (check_idx < max)) 
    {

      mUI_DefencePokeIdx = check_idx;

      static const int layoutno = 0;
      gfl2::lyt::LytMultiResID& rMultiResId = UIView::GetG2DUtil()->GetLayoutResourceID( layoutno );
      const gfl2::lyt::LytRes *res =  mUILytWk->GetResource();
      const nw::lyt::TextureInfo *info;
      gfl2::lyt::LytPicture *pPicPane;


      for (int i=0; i<max; i++)
        {
          if (i == check_idx)
            {
              info = res->GetTexture( LYTRES_EFF_VIEW_DOT_BFLIM, rMultiResId.GetMultiID() );
            }
          else
            {
              info = res->GetTexture( LYTRES_EFF_VIEW_EMPTY_BFLIM, rMultiResId.GetMultiID() );
            }

          pPicPane = mUILytWk->GetPicturePane( ListPaneID[i] );
          pPicPane->GetMaterial(0)->SetTexMap(0,info);

          pPicPane->SetVisible (true);

          if( (RULE_DATA[mUI_BattleRuleIdx].rule_num == BTL_RULE_INTRUDE) && (i == 2) )
            {
              pPicPane->SetVisible (false);
            }

          if( (RULE_DATA[mUI_BattleRuleIdx].rule_num == BTL_RULE_SINGLE) && (i >= 2) )
            {
              pPicPane->SetVisible (false);
            }


        }

    }
}

//-------------------------------------------------------------------------
/**
 * @brief 選択したターゲットのセット
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::SetAction_TargetSet()
{

  // ターゲット情報
  mpEffViewUiInfo->mRangeType = mUI_BattleRangeIdx;
  mpEffViewUiInfo->mAttackPos = mUI_AttackPokeIdx;
  mpEffViewUiInfo->mDeffencePos = mUI_DefencePokeIdx;

  // Set action
  mpEffViewUiInfo->SetAction( EffectViewerUiInfo::ACTION_TARGET_SET);

}



//-------------------------------------------------------------------------
/**
 * @brief ワザ範囲の更新
 *
 * @param[in] range_idx : ワザ範囲リストのインデックス
 */
//-------------------------------------------------------------------------
void EffectViewerLowBgDraw::UpdateLayoutDisp_BattleRange( int range_idx )
{

  WazaRangeData RANGE_DATA[] = 
    {
      {L"単体", EffectViewerUiInfo::WAZA_RANGE_SINGLE},
      {L"両脇", EffectViewerUiInfo::WAZA_RANGE_SIDE},
      {L"敵全体", EffectViewerUiInfo::WAZA_RANGE_ENEMY},
      {L"全体", EffectViewerUiInfo::WAZA_RANGE_ALL},
    };

  int max_num = EffectViewerUiInfo::WAZA_RANGE_MAX;
  if (range_idx < 0)
    {
      range_idx = (max_num-1);
    }

  if (range_idx >= max_num)
    {
      range_idx = 0;
    }

  mUI_BattleRangeIdx = RANGE_DATA[range_idx].range_num;
  mpEffViewUiInfo->mRangeType = mUI_BattleRangeIdx;
  
  gfl2::lyt::LytTextBox *textBox;
  textBox = mUILytWk->GetTextBoxPane( PANENAME_EFF_VIEW_PANE_RULE_RANGE_STR );
  textBox->SetString( RANGE_DATA[range_idx].range_str );  

}



//--------------------------------------------------------------------------------------------
/**
 * @brief  タッチパネルイベントの検知
 *
 * @param  pTouchPanel タッチパネル
 * @param  isTouch     タッチ状態であるならtrue、タッチされていないならfalse
 *                     (タッチトリガを調べたい場合はTouchPanel::IsTouchTrigger()にて判断可能）
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
//--------------------------------------------------------------------------------------------
// 特定のタッチパネルがタッチされると呼ばれる。どこをタッチしたかどうかは関係ない模様
//::app::ui::UIInputListener::ListenerResult EffectViewerLowBgDraw::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
//{
//  if( mUiListener == NULL ){
//    return ENABLE_ACCESS;
//  }
//
//  // 決定
//  if( pTouchPanel->IsTouchTrigger() ){
//    //    mUiListener->OnButtonAction(0);
//    return DISABLE_ACCESS;
//  }
//  return ENABLE_ACCESS;
//}

//------------------------------------------------------------------
/**
 * @brief   ペインイベントの検知
 *
 * @param  layoutId 通知を受けるレイアウトＩＤ
 * @param  painId   通知を受けるペインＩＤ
 *
 * @return 同フレーム内での他入力イベントを許可するならtrue、
 *         他のイベントを排除するならfalseを返却すること。
 */
// 特定のペインがタッチされると呼ばれる
//------------------------------------------------------------------
::app::ui::UIInputListener::ListenerResult EffectViewerLowBgDraw::OnLayoutPaneEvent( const u32 paneId )
{

  // Frame側のメソッドに渡す？
  mUiListener->OnButtonAction(paneId);

  //  if( painId == BgViewerLowBgDrawListener::BTN_ID_CANCEL )
  //  {
  //    return DISABLE_ACCESS;
  //  }

  switch (paneId)
    {

    case PANENAME_EFF_VIEW_PANE_RES_DEL_HIT:
      // Resource Num
      break;

    case PANENAME_EFF_VIEW_PANE_FLG_SET_1_BUT:
      // Debug print On/Off
      { ClickDebugPrint(); }
      break;

    case PANENAME_EFF_VIEW_PANE_WAZA_HIT:
      // 技名のキーボード選択
      { CallKeyBoard( GARC_message_wazaname_DAT ); }
      break;

    case PANENAME_EFF_VIEW_PANE_POKE_SEL_HIT:
      // ポケモン名のキーボード選択
      { CallKeyBoard( GARC_message_monsname_DAT );  }
      break;

      // ページ切り替え
    case PANENAME_EFF_VIEW_PANE_PAGE_U:
      { ChangeLayoutPage((mUI_Page+1)); }
      break;
    case PANENAME_EFF_VIEW_PANE_PAGE_D:
      { ChangeLayoutPage((mUI_Page-1)); }
      break;

    case PANENAME_EFF_VIEW_PANE_WAZA_PLAY:
      // 技の再生
      { SetAction_PlayWaza(); }
      break;

      // ワザのサブナンバー
    case PANENAME_EFF_VIEW_PANE_NO_SEL_SUB_UP:
      {  UpdateLayoutDisp_WazaSubNumber( (mUI_SelectWazaSubNum+1) ); };
      break;
    case PANENAME_EFF_VIEW_PANE_NO_SEL_SUB_DOWN:
      {  UpdateLayoutDisp_WazaSubNumber( (mUI_SelectWazaSubNum-1) ); };
      break;

    case PANENAME_EFF_VIEW_PANE_BGM_PLAY:
      { ClickBgmPlay (); }
      break;

      // Ball ID
    case PANENAME_EFF_VIEW_PANE_BALL_ID_SET:
      { SetAction_SetBall_ID();}
      break;

    case PANENAME_EFF_VIEW_PANE_BALL_ID_U_0:
      { UpdateLayoutDisp_Ball_ID( 0, (mUI_Ball_ID[0]+1) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_BALL_ID_D_0:
      { UpdateLayoutDisp_Ball_ID( 0, (mUI_Ball_ID[0]-1) ); }
      break;

    case PANENAME_EFF_VIEW_PANE_BALL_ID_U_1:
      { UpdateLayoutDisp_Ball_ID( 1, (mUI_Ball_ID[1]+1) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_BALL_ID_D_1:
      { UpdateLayoutDisp_Ball_ID( 1, (mUI_Ball_ID[1]-1) ); }
      break;

      // Trainer ID
    case PANENAME_EFF_VIEW_PANE_TR_ID_UP_00:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType+100) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_TR_ID_UP_01:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType+10) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_TR_ID_UP_02:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType+1) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_00:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType-100) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_01:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType-10) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_TR_ID_DOWN_02:
      { UpdateLayoutDisp_TrainerType( (mUI_TrainerType-1) ); }
      break;

    case PANENAME_EFF_VIEW_PANE_TR_ID_LBL_HIT:
      { CallKeyBoard( GARC_message_trtype_DAT ); }
      break;


    case PANENAME_EFF_VIEW_PANE_RULE_HIT:
      // バトルルール切り替え
      { UpdateLayoutDisp_BattleRule( (mUI_BattleRuleIdx+1) ); }
      break;

    case PANENAME_EFF_VIEW_PANE_RULE_BTN:
      // バトルルール設定
      { SetAction_SetBattleRule(); }
      break;


    case PANENAME_EFF_VIEW_PANE_RULE_RANGE_HIT:
      // 技範囲切り替え
      { UpdateLayoutDisp_BattleRange ( (mUI_BattleRangeIdx+1) ); }
      break;


      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_1:
      //      { UpdateLayoutDisp_PokemonNum( mUI_SelectPokemonNum+100 ); }
      //      break;
      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_2:
      //      { UpdateLayoutDisp_PokemonNum( (mUI_SelectPokemonNum+10) ); }
      //      break;
      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_UP_3:
      //      { UpdateLayoutDisp_PokemonNum( (mUI_SelectPokemonNum+1) ); }
      //      break;
      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_1:
      //      { UpdateLayoutDisp_PokemonNum( (mUI_SelectPokemonNum-100) ); }
      //      break;
      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_2:
      //      { UpdateLayoutDisp_PokemonNum( (mUI_SelectPokemonNum-10) ); }
      //      break;
      //    case PANENAME_EFF_VIEW_PANE_POKE_SEL_DOWN_3:
      //      { UpdateLayoutDisp_PokemonNum( (mUI_SelectPokemonNum-1) ); }
      //      break;

      // Pokemon form
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_F_UP:
      { UpdateLayoutDisp_PokemonForm( (mUI_SelectPokemonFormNum+1) ); }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_F_DOWN:
      { UpdateLayoutDisp_PokemonForm( (mUI_SelectPokemonFormNum-1) ); }
      break;

      // Battle Target
    case PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_00:
      { 
	UpdateLayoutDisp_AttackSide( 0 );
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_01:
      { 
	UpdateLayoutDisp_AttackSide( 1 ); 
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_02:
      { 
	UpdateLayoutDisp_AttackSide( 2 ); 
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_ATK_BTN_03:
      { 
	UpdateLayoutDisp_AttackSide( 3 ); 
	SetAction_TargetSet();
      }
      break;

    case PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_00:
      { 
	UpdateLayoutDisp_DeffenceSide( 0 ); 
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_01:
      { 
	UpdateLayoutDisp_DeffenceSide( 1 ); 
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_02:
      { 
	UpdateLayoutDisp_DeffenceSide( 2 ); 
	SetAction_TargetSet();
      }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_DEF_BTN_03:
      { 
	UpdateLayoutDisp_DeffenceSide( 3 ); 
	SetAction_TargetSet();
      }
      break;

      // Set Pokemon
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_1:
      { SetAction_SetPokemon( 0 ); }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_2:
      { SetAction_SetPokemon( 1 ); }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_3:
      { SetAction_SetPokemon( 2 ); }
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_LOAD_4:
      { SetAction_SetPokemon( 3 ); }
      break;

    case PANENAME_EFF_VIEW_PANE_POKE_SEL_RARE:
      { ClickPokemonRare (); };
      break;
    case PANENAME_EFF_VIEW_PANE_POKE_SEL_SEX:
      { ClickPokemonSex (); };
      break;

    }

  // １フレーム１タッチまで
  return DISABLE_ACCESS;
}



GFL_NAMESPACE_END( Ariizumi )
GFL_NAMESPACE_END( Test )

#endif //PM_DEBUG
