//======================================================================
/**
 * @file btlv_convention_win.cpp
 * @date 2016/01/19 15:31:56
 * @author kijima_satoshi
 * @brief 大会演出用プレイヤー情報ウィンドウ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <niji_conv_header/app/battle/upper/minupp_pane.h>       // For Layout Pane
#include <niji_conv_header/app/battle/upper/minupp_anim_list.h>  // For Layout Anime

#include <niji_conv_header/message/msg_btl_state.h>  // For btl_state.mstext
#include <niji_conv_header/message/msg_btl_ui.h> 

#include "Battle/source/btlv/btlv_ui.h"
#include "Battle/source/btlv/BtlvUiMsgWin.h"

#include <Savedata/include/MyStatus.h>              // For Savedata::MyStatus
//#include "Battle/include/battle_SetupParam.h"       // For BATTLE_CONVENTION_INFO

#include "Battle/source/btl_mainmodule.h"
#include "Battle/source/btl_BattleRule.h"
#include "Battle/source/btl_field.h"                // btl_fieldEff_def.hをクラス内でincludeしているので注意
#include "Battle/source/btl_pokeparam.h"            // btl_pokeparam_contflag.hをクラス内でincludeしているので注意

#include "Battle/source/btl_SideEffectStatus.h"
#include "Battle/source/btl_PosEffectstatus.h"


#include "./btlv_convention_win.h"


GFL_NAMESPACE_BEGIN(btl)



//-------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-------------------------------------------------------------------------
BtlvConventionWin::BtlvConventionWin( BattleViewSystem *btlvCore , ::app::util::G2DUtil *g2dUtil,  app::BtlvUiMsgWin *uiMsgWin)
  :mBtlvCore(btlvCore)
  ,mG2dUtil(g2dUtil)
  ,mUiMsgWin(uiMsgWin)
{
  mDispStatus = STATUS_UNINIT;
  mDataTypeStart = DATA_PLAYER_RATE;
}

//-------------------------------------------------------------------------
/**
 * @brief イニシャライズ
 */
//-------------------------------------------------------------------------
void BtlvConventionWin::Initialize(void)
{

  // プレイヤー数 = クライアント数
  // 「プレイヤー vs プレイヤー」しかないので基本的に2固定
  mPlayerNum = BattleRule::GetClientNum( mBtlvCore->GetBattleRule(), mBtlvCore->IsMultiMode() );



  // レート表示可能かどうか
  if( mBtlvCore->IsClientRatingDisplay() )
    {
      mDataTypeStart = DATA_PLAYER_RATE;
    }
  else
    {
      mDataTypeStart = DATA_TURN;
    }

  // 使用する窓は２つ（時間表示を隠してしまうため）
  mWindowMax = 2;


  // バトルルールによる設定
  // @fix NMCat[1887]
  BtlRule brule = mBtlvCore->GetBattleRule();
  switch(brule)
    {

    case BTL_RULE_SINGLE:
    default:
      mPokemonNum = 2;
      break;

    case BTL_RULE_DOUBLE:
    case BTL_RULE_ROYAL:
      mPokemonNum = 4;
      break;
      
    }

  mCurrentDataType = mDataTypeStart;
  mWaitFrameCount = 0;
  mCurrentPosition = 0;
  mSubSeq = 0;  



  // レイアウト関連の初期化
  mLytIndex = 0;        // 現在レイアウトは１つのみ
  mGaugeLyt = mUiMsgWin->GetLayoutWork(mLytIndex);


  // 配置は上側だけを使用（タイマーが隠れるため）
  // 0 1
  // (3 2)
  
  // Window pane
  mLytParts[0] = mGaugeLyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLAYER_INFO_02 ); // Up Left
  mLytParts[1] = mGaugeLyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLAYER_INFO_03 ); // Up Right
  mLytParts[2] = mGaugeLyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLAYER_INFO_01 ); // Down Right  
  mLytParts[3] = mGaugeLyt->GetPartsPane( PANENAME_BATTLE_MIN_UPP_000_PANE_PLAYER_INFO_00 ); // Down Left
  
  // Animation list
  mAnmTbl[ANM_DISP_PLAYER0_INFO] = LA_MINUPP_BATTLE_MIN_UPP_000_PLAYER_INFO_02;
  mAnmTbl[ANM_DISP_PLAYER1_INFO] = LA_MINUPP_BATTLE_MIN_UPP_000_PLAYER_INFO_03;
  mAnmTbl[ANM_DISP_PLAYER2_INFO] = LA_MINUPP_BATTLE_MIN_UPP_000_PLAYER_INFO_01;
  mAnmTbl[ANM_DISP_PLAYER3_INFO] = LA_MINUPP_BATTLE_MIN_UPP_000_PLAYER_INFO_00;
 

  // Hide all pane
  HideWindow();


  KIJIMA_PRINT(">!>>> BtlvConventionWin::Initialized. Client Num [%d], Poke Num [%d] \n", mPlayerNum, mPokemonNum );


}

//-------------------------------------------------------------------------
/**
 * @brief 開始、再開
 */
//-------------------------------------------------------------------------
void BtlvConventionWin::Start (void)
{
  KIJIMA_PRINT(">!>>> BtlvConventionWin::Start() \n");

  mDispStatus = STATUS_WAIT_START;
  mWaitFrameCount = 0;

}

//-------------------------------------------------------------------------
/**
 * @brief 毎フレーム更新
 */
//-------------------------------------------------------------------------
void BtlvConventionWin::Update (void)
{
  if( ( STATUS_UNINIT ==  mDispStatus ) || ( STATUS_IDLE ==  mDispStatus ) )
    {
      return;
    }

  //  KIJIMA_PRINT(">!>>> BtlvConventionWin::Update() \n");
  mWaitFrameCount++;
  
  switch( mDispStatus )
    {

    case STATUS_WAIT_START:
      {
        // 開始フレームまで待つ
        if( mWaitFrameCount >= WAIT_FRAME )
          {
            // 再開時にデータータイプが適正か？
            int dt = SearchNextDataType( mCurrentDataType );

            if( dt == mCurrentDataType )
              {
                if (mSubSeq >= mDispInfoCount)
                  {
                    KIJIMA_PRINT (">!!> Sub seq over.");
                    mSubSeq = (mDispInfoCount-1);
                  }
              }
            else
              {
                // データタイプが変わる場合
                mCurrentDataType = dt;
                mSubSeq = 0;
              }

            mDispStatus = STATUS_DISP;
            mWaitFrameCount = 0;

            SetWindowInfo();
          }
      }
      break;

    case STATUS_DISP:
      {

        // ボタンによる表示切替
        {
          const gfl2::ui::Button *uiButton = mBtlvCore->GetButton();
          if( uiButton->IsTrigger(gfl2::ui::BUTTON_X) )
            {
              ChangeNext();
              mWaitFrameCount = 0;
            }
        }

        if( mWaitFrameCount >= DISP_FRAME )
          {
            ChangeNext();
            mWaitFrameCount = 0;
          }
      }
      break;

    }

}

//-------------------------------------------------------------------------
/**
 * @brief 中断
 */
//-------------------------------------------------------------------------
void BtlvConventionWin::Break (void)
{
  KIJIMA_PRINT(">!>>> BtlvConventionWin::Break() \n");

  mDispStatus = STATUS_IDLE;
  HideWindow();
}

// ウィンドウの消去
void BtlvConventionWin::HideWindow(void)
{
  mLytParts[0]->SetVisible(false);
  mLytParts[1]->SetVisible(false);
  mLytParts[2]->SetVisible(false);
  mLytParts[3]->SetVisible(false);
}

// 表示情報の有無チェック
bool BtlvConventionWin::IsExistDispData( int dtype )
{
  switch( dtype )
    {

    case DATA_POKE_TYPE:
      // 数のカウントはするが、常に存在する
      { return ( IsExistPokeType() ); }
      //      break;

      //    case DATA_PLAYER_COUNTRY:
    case DATA_PLAYER_RATE:
      { 
        // 常に存在する
        mDispInfoCount = mPlayerNum;
        return true;
      }
      //      break;

    case DATA_TURN:
      { 
        // 常に存在する
        mDispInfoCount = 1;
        return true;
      }

    case DATA_WEATHER:
      { return ( IsExistWeather() ); }
      //      break;

    case DATA_GROUND_EFFECT:
      { return ( IsExistGroundEffect() ); }
      //      break;

    case DATA_SIDE_EFFECT:
      { return ( IsExistSideEffect() ); }
      //      break;

    case DATA_POSISION_EFFECT:
      { return ( IsExistPositionEffect() ); }
      //      break;

    case DATA_PLACE_EFFECT:
      { return ( IsExistPlaceEffect() ); }
      //      break;

    case DATA_POKE_SICK:
      { return ( IsExistPokeSick() ); }
      //      break;

    case DATA_POKE_EX_TYPE:
      { return ( IsExistPokeExType() ); }

    case DATA_POKE_CONTINUE:
      { return ( IsExistPokeContinue() ); }
      //      break;

//    default:
//      { return false; }
//      break;
    }

  return false;

}

// 表示可能なデータータイプを検索
int BtlvConventionWin::SearchNextDataType( int current_dtype )
{
  for (int i= current_dtype; i<DATA_MAX ;i++)
    {
      if( IsExistDispData( i ) )
        {
          return( i );
        }
    }

  // 他になければ先頭に戻る
  IsExistPokeType();    // Make Info list
  return( mDataTypeStart );
}

//-------------------------------------------------------------------------
/**
 * @brief 次の表示へ切り替え
 */
//-------------------------------------------------------------------------
void BtlvConventionWin::ChangeNext (void)
{
  
  HideWindow();

  // 各データータイプのサブシーケンス進行
  switch( mCurrentDataType )
    {

      // -----------------------------
      ///< プレイヤーに付随する情報
      // -----------------------------
      //    case DATA_PLAYER_COUNTRY:
    case DATA_PLAYER_RATE:
      ///< レート
      {
        mSubSeq++;
        mCurrentPosition = ( (mCurrentPosition+1) % mWindowMax );
        
        if( mSubSeq >= mPlayerNum )
          {
            mCurrentDataType = SearchNextDataType( (mCurrentDataType+1) );
            //            mCurrentPosition = (mSubSeq % mWindowMax);
            mSubSeq = 0;
          }

      }
      break;

      // --------------------------------
      ///< ポケモンに固定数付随する情報
      // --------------------------------
    case DATA_POKE_TYPE:
      ///< ポケモンのタイプ
      {
        mSubSeq++;
        mCurrentPosition = ( (mCurrentPosition+1) % mWindowMax );
        
        if( mSubSeq >= mDispInfoCount )
          {
            mCurrentDataType = SearchNextDataType( (mCurrentDataType+1) );
            mSubSeq = 0;
          }
      }
      break;

      // ---------------------
      // 単一なもの
      // ---------------------
    case DATA_TURN:
      // ターン数
    case DATA_WEATHER:
      // 天候
    case DATA_GROUND_EFFECT:
      // 地形効果
      {
        mCurrentDataType = SearchNextDataType( (mCurrentDataType+1) );
        mCurrentPosition = ( (mCurrentPosition +1) % mWindowMax );
        mSubSeq = 0;
      }
      break;

      // ---------------------
      // 不定数なもの
      // ---------------------
    case DATA_SIDE_EFFECT:
      // 陣営効果
    case DATA_POSISION_EFFECT:
      // 位置エフェクト
    case DATA_PLACE_EFFECT:
      // 場のエフェクト
    case DATA_POKE_SICK:
      // ポケモン状態異常
    case DATA_POKE_EX_TYPE:
      // ポケモンタイプ拡張
    case DATA_POKE_CONTINUE:
      // ポケモン継続フラグ
      {
        mSubSeq++;
        mCurrentPosition = ( (mCurrentPosition+1) % mWindowMax );
        
        if( mSubSeq >= mDispInfoCount )
          {
            mCurrentDataType = SearchNextDataType( (mCurrentDataType+1) );
            mSubSeq = 0;
          }
      }
      break;

    default:
      GFL_ASSERT(0);
      break;

    }

  // Set info
  SetWindowInfo();

} 

void BtlvConventionWin::SetWindowInfo(void)
{
  KIJIMA_PRINT(">!>>> StartWindowInfo : Dtype[%d] pos[%d] subseq[%d]\n",  mCurrentDataType, mCurrentPosition, mSubSeq);


  switch( mCurrentDataType )
    {

//    case DATA_PLAYER_COUNTRY:
//      {
//        ///< プレイヤー居住国
//        mCurrentPosition = (mSubSeq % mPlayerNum);
//        SetWindowInfo_PlayerCountry( mCurrentPosition );
//      }
//      break;

    case DATA_PLAYER_RATE:
      {
        ///< プレイヤーのレート
        mCurrentPosition = (mSubSeq % mPlayerNum);  // ウィンドウ位置の指定
        SetWindowInfo_PlayerRate( mCurrentPosition );
        //SetWindowInfo_Test( mCurrentPosition );      // For test
      }
      break;

    case DATA_POKE_TYPE:
      {
        ///< ポケモンのタイプ
        // mCurrentPosition = (mSubSeq % mPokemonNum);  // ウィンドウ位置の指定
        SetWindowInfo_PokeType( mCurrentPosition );
      }
      break;

    case DATA_TURN:
      {
        ///< ターン数
        SetWindowInfo_Turn( mCurrentPosition );
      }
      break;

    case DATA_WEATHER:
      // 天候
      SetWindowInfo_Weather( mCurrentPosition );
      break;

    case DATA_GROUND_EFFECT:
      // 地形効果
      SetWindowInfo_GroundEffect( mCurrentPosition );
      break;

    case DATA_SIDE_EFFECT:
      // 陣営効果
      SetWindowInfo_SideEffect( mCurrentPosition );
      break;

    case DATA_POSISION_EFFECT:
      // 位置エフェクト
      SetWindowInfo_PositionEffect( mCurrentPosition );
      break;

    case DATA_PLACE_EFFECT:
      // 場のエフェクト
      SetWindowInfo_PlaceEffect( mCurrentPosition );
      break;

    case DATA_POKE_SICK:
      // ポケモン状態異常
      SetWindowInfo_PokeSick( mCurrentPosition );
      break;

    case DATA_POKE_EX_TYPE:
      // タイプ拡張
      SetWindowInfo_PokeExType( mCurrentPosition );
      break;

    case DATA_POKE_CONTINUE:
      // ポケモン継続フラグ
      SetWindowInfo_PokeContinue( mCurrentPosition );
      break;
    }


  // ペインとアニメで配置が違うので、今はVisibleで切り替えている
  //  mG2dUtil->StartAnime( mLytIndex, mAnmTbl[player_num] );
  mLytParts[mCurrentPosition]->SetVisible(true);

}


void BtlvConventionWin::ClearWindowTextBox( int pos )
{

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::lyt::LytTextBox *textBox;

  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], PANENAME_BATTLE_PLT_LOW_007_PANE_NAME, &rMultiResId );
  textBox->SetString( L"" );

  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, &rMultiResId );
  textBox->SetString( L"" );

  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, &rMultiResId );
  textBox->SetString( L"" );

  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_02, &rMultiResId );
  textBox->SetString( L"" );

}


//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにプレイヤー名をセットする
 * @param pos       ウィンドウ位置
 * @param paneId    対象ペインのインデックス
 * @param myst      プレイヤーデータ
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPanePlayerName( int pos, const Savedata::MyStatus *myst, u32 paneId )
{

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);

  mBtlvCore->GetStringUi( msgStr, BTLSTR_PROFILE_NAME );

  // 指定バッファにプレイヤー名をセット
  if( myst == NULL )
    {
      gfl2::lyt::LytTextBox *textBox;
      textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
      textBox->SetString( L"-----" );
    }
  else
    {
      print::WordSet *wset = mBtlvCore->GetWordSet();
      wset->RegisterPlayerName( 0, myst );
      wset->Expand( expStr, msgStr );
      
      gfl2::lyt::LytTextBox *textBox;
      textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
      print::SetStringForLytTextBox( textBox, expStr );
    }

}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインに国名をセットする
 * @param pos       ウィンドウ位置
 * @param paneId    対象ペインのインデックス
 * @param myst      プレイヤーデータ
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneCountry( int pos, const Savedata::MyStatus *myst, u32 paneId )
{

#if 0
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);

  std::memset( m_countryName, 0, sizeof(m_countryName) );

  // 国名取得
  const System::CfgLanguageCode langCode = System::GetCTRLanguageCode();
  GameSys::GameData* pGameData = mBtlvCore->GetMainModule()->GetGameManager()->GetGameData();
#endif


  //  const GameSys::CountryNameCache* countryName = pGameData->GetCountryNameCacheConst();
  //  ::std::wcsncpy( m_countryName, countryName->GetCountryName( static_cast<nn::cfg::CTR::CfgCountryCode>(myst->GetCountry()) ), nn::cfg::CTR::CFG_SIMPLE_ADDRESS_NAME_LENGTH );
  //
  //
  //  // 国名が１０文字以上か判定
  //  bool   isCutString = false;
  //  size_t strCutLen   = 20;
  //  if( (nn::cfg::CFG_LANGUAGE_JAPANESE    ==langCode) || (nn::cfg::CFG_LANGUAGE_KOREAN      ==langCode) ||
  //      (nn::cfg::CFG_LANGUAGE_SIMP_CHINESE==langCode) || (nn::cfg::CFG_LANGUAGE_TRAD_CHINESE==langCode) )
  //  {
  //    strCutLen = 10;  // 全角文字言語の場合
  //  }
  //  // 一定文字数を超えた場合は、「国名…」と表示させる
  //  if( std::wcslen( m_countryName ) > strCutLen )
  //  {
  //    m_countryName[strCutLen] = '\0';
  //    isCutString = true;
  //  }
  //
  //  // 国名
  //  WordSet *wset = mBtlvCore->GetWordSet();
  //  msgStr->SetStr( m_countryName );
  //  wset->RegisterWord( 1, *msgStr, WordSet::GRAMMER_NEUTRAL, WordSet::FORM_DEFAULT );
  //
  //  str::GetUIString( msgStr, isCutString?BTLSTR_PROFILE_COUNTLY_DOT:BTLSTR_PROFILE_COUNTLY_NUM );
  //  wset->Expand( expStr, msgStr );
  //
  //  gfl::grp::g2d::LytTextBox *textPane;
  //  textPane = mLytWin.GetLytWk()->GetTextBoxPane( paneId );
  //  print::SetStringForLytTextBox( textPane, expStr );
}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにTAG展開でワザ名をセットする
 * @param pos ウィンドウ位置
 * @param messageId TAGの入ったメッセージID
 * @param paneId    対象ペインのインデックス
 * @param wazaID     ワザID
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneWaza( int pos, u32 messageId,u32 paneId, u32 wazaID, int bufNum )
{

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);

  mBtlvCore->GetStringUi( msgStr, messageId );


  // 指定バッファに数値をセット
  print::WordSet *wset = mBtlvCore->GetWordSet();

  wset->RegisterWazaName( bufNum, wazaID );
  wset->Expand( expStr, msgStr );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, expStr );

}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにTAG展開でポケモンタイプをセットする
 * @param pos ウィンドウ位置
 * @param messageId TAGの入ったメッセージID
 * @param paneId    対象ペインのインデックス
 * @param wazaID    タイプID
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPanePokeType( int pos, u32 messageId,u32 paneId, pml::PokeType typeID, int bufNum )
{

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);

  mBtlvCore->GetStringUi( msgStr, messageId );


  // 指定バッファに数値をセット
  print::WordSet *wset = mBtlvCore->GetWordSet();
  wset->RegisterPokeTypeName( bufNum, typeID );
  wset->Expand( expStr, msgStr );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, expStr );

}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにTAG展開で数字をセットする
 * @param pos ウィンドウ位置
 * @param messageId TAGの入ったメッセージID
 * @param paneId    対象ペインのインデックス
 * @param value     数値
 * @param keta      桁
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneNumber( int pos, u32 messageId,u32 paneId, u32 value, u32 keta, int bufNum )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);

  mBtlvCore->GetStringUi( msgStr, messageId );

  // 指定バッファに数値をセット
  print::WordSet *wset = mBtlvCore->GetWordSet();
  wset->RegisterNumber( bufNum, value, keta, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  wset->Expand( expStr, msgStr );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, expStr );
}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにメッセージをセットする（StrBuf用）
 * @param pos ウィンドウ位置
 * @param messageId メッセージID
 * @param paneId    対象ペインのインデックス
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneMessage( int pos, gfl2::str::StrBuf *pMsg, u32 paneId )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, pMsg );

}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにメッセージをセットする（btl_ui.mstext用）
 * @param pos ウィンドウ位置
 * @param messageId メッセージID
 * @param paneId    対象ペインのインデックス
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneMessageUI( int pos, u32 messageId, u32 paneId )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);

  mBtlvCore->GetStringUi( msgStr, messageId );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, msgStr );
}

//--------------------------------------------------------------------------
/**
 * @brief 指定ペインにメッセージをセットする（btl_state.mstext用）
 * @param pos ウィンドウ位置
 * @param messageId メッセージID
 * @param paneId    対象ペインのインデックス
 */
//--------------------------------------------------------------------------
void BtlvConventionWin::SetTextboxPaneMessageSTATE( int pos, u32 messageId, u32 paneId )
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);

  mBtlvCore->GetUiSys()->GetStatusMessageData()->GetString( messageId, *msgStr );

  gfl2::lyt::LytTextBox *textBox;
  textBox = mGaugeLyt->GetTextBoxPane( mLytParts[pos], paneId, &rMultiResId );
  print::SetStringForLytTextBox( textBox, msgStr );
}
  

void BtlvConventionWin::SetWindowInfo_PlayerCountry( int pos )
{

  // Clear text
  ClearWindowTextBox( pos );

  u8 client_id;

  if( pos == 0 )
    {
      // 自プレイヤーの場合
      client_id = mBtlvCore->GetClientID();
    }
  else
    {
      // 敵プレイヤーの場合
      client_id = mBtlvCore->GetEnemyClientID( (pos-1) );
    }

  // プレイヤーデータ取得
  const Savedata::MyStatus *myStatus;
  myStatus = mBtlvCore->GetClientPlayerData( client_id );

  // プレイヤー名
  SetTextboxPanePlayerName( pos, myStatus, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  SetTextboxPaneMessageUI( pos, BTLSTR_PROFILE_01_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00 ); // すんでいるばしょ
  SetTextboxPaneCountry( pos, myStatus, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01 );

}

void BtlvConventionWin::SetWindowInfo_PlayerRate( int pos )
{

  // Clear text
  ClearWindowTextBox( pos );

  u8 client_id;

  if( pos == 0 )
    {
      // 自プレイヤーの場合
      client_id = mBtlvCore->GetClientID();
    }
  else
    {
      // 敵プレイヤーの場合
      client_id = mBtlvCore->GetEnemyClientID( (pos-1) );
    }

  // プレイヤーデータ取得
  const Savedata::MyStatus *myStatus;
  myStatus = mBtlvCore->GetClientPlayerData( client_id );

#ifdef KIJIMA_LOCAL_TEST
  int rate = 9999;
#else
  int rate = mBtlvCore->GetClientRating( client_id );//  通信対戦でのみ有効です
#endif

  // プレイヤー名
  SetTextboxPanePlayerName( pos, myStatus, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );
  // レート
  SetTextboxPaneMessageUI( pos, BTLSTR_PROFILE_02_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00 );
  // 数字のみタグ
  SetTextboxPaneNumber( pos, BTLSTR_PROFILE_02_02, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, rate ,4, 1); 

}
// ----------------------------------------------------------
// 天候
bool BtlvConventionWin::IsExistWeather(void)
{

  const FieldStatus *fstatus = mBtlvCore->GetFieldStatus();
  GFL_ASSERT( fstatus );

  mDispInfoCount = 0;

  BtlWeather tenkou = fstatus->GetWeather();

  if( BTL_WEATHER_NONE == tenkou  )
    {
      return false;
    }

  mDispInfoCount = 1;
  return true;

}

void BtlvConventionWin::SetWindowInfo_Weather( int pos )
{
  // BtlWeatherと順番が対応していること
  u32 WEATHER_MSG[] = 
    {
      BTR_STATE_66_02,  ///< 天候無し（"-"）
      BTR_STATE_01_01,  ///< はれ
      BTR_STATE_02_01,  ///< あめ
      BTR_STATE_04_01,  ///< あられ
      BTR_STATE_03_01,  ///< すなあらし
      BTR_STATE_70_01,  ///< おおあめ
      BTR_STATE_69_01,  ///< おおひでり
      BTR_STATE_71_01,  ///< 乱気流
    };

  // Clear text
  ClearWindowTextBox( pos );

  const FieldStatus *fstatus = mBtlvCore->GetFieldStatus();
  GFL_ASSERT( fstatus );

  BtlWeather tenkou = fstatus->GetWeather();


  // 道具での延長に対応したターン数の取得
  // @fix NMCat[3360]
  u32 numerator, denominator;
  mBtlvCore->GetUiDisplay_Turn_Weather( &numerator, &denominator );

  u32 weather_msg_id = WEATHER_MSG[tenkou];


  // 天候名
  SetTextboxPaneMessageSTATE( pos, weather_msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  // 分子、分母供に０の場合は永続でターン数非表示
  // @fix NMCat[3386] 
  if( ( 0 == numerator  ) && (0 == denominator) )
    {
      return;
    }

  // （数字）ターン
  SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, numerator, 1); 

}


// ----------------------------------------------------------
// 地形効果
bool BtlvConventionWin::IsExistGroundEffect(void)
{

  const FieldStatus *fstatus = mBtlvCore->GetFieldStatus();
  GFL_ASSERT( fstatus );

  mDispInfoCount = 0;

  BtlGround gr_eff = fstatus->GetGround();

  if( BTL_GROUND_NONE == gr_eff  )
    {
      return false;
    }

  mDispInfoCount = 1;
  return true;

};

void BtlvConventionWin::SetWindowInfo_GroundEffect( int pos )
{
  // BtlGroundと順番が対応していること
  u32 GROUND_MSG[] = 
    {
      BTR_STATE_66_02,   ///< 無し（"-"）
      BTR_STATE_06_01,   ///< グラスフィールド
      BTR_STATE_07_01,   ///< ミストフィールド
      BTR_STATE_05_01,   ///< エレキフィールド
      BTR_STATE_72_01,   ///< サイコフィールド
      //      BTR_STATE_66_02,   ///< アクアフィールド(No use)
      //      BTR_STATE_66_02,   ///< アイスフィールド(No use)
    };

  // Clear text
  ClearWindowTextBox( pos );

  const FieldStatus *fstatus = mBtlvCore->GetFieldStatus();
  GFL_ASSERT( fstatus );

  BtlGround gr_eff = fstatus->GetGround();

  // @fix GFNMCat[5261]
  u32 numerator, denominator;
  mBtlvCore->GetUiDisplay_Turn_Ground( &numerator, &denominator );

  u32 msg_id = GROUND_MSG[gr_eff];

  // フィールド名
  SetTextboxPaneMessageSTATE( pos, msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  // 分子、分母供に０の場合は永続でターン数非表示
  if( ( 0 == numerator  ) && (0 == denominator) )
    {
      return;
    }

  // （数字）ターン
  SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, numerator, 1);

}

// ----------------------------------------------------------
// サイドエフェクト（陣営効果）

// チェック対象のサイドエフェクト
BtlvConventionWin::EffectInfo TARGET_SIDE_EFFECT[] = 
  {
    {BTL_SIDEEFF_REFRECTOR,     BTR_STATE_45_01},  ///< 物理攻撃を半減
    {BTL_SIDEEFF_HIKARINOKABE,  BTR_STATE_44_01},  ///< 特殊攻撃を半減
    {BTL_SIDEEFF_SINPINOMAMORI, BTR_STATE_31_01},  ///< ポケ系状態異常にならない
    {BTL_SIDEEFF_SIROIKIRI,     BTR_STATE_30_01},  ///< ランクダウン効果を受けない
    {BTL_SIDEEFF_OIKAZE,        BTR_STATE_19_01},  ///< すばやさ２倍
    {BTL_SIDEEFF_OMAJINAI,      BTR_STATE_20_01},  ///< 攻撃が急所に当たらない
    {BTL_SIDEEFF_MAKIBISI,      BTR_STATE_48_01},  ///< 入れ替えて出てきたポケモンにダメージ（３段階）
    {BTL_SIDEEFF_DOKUBISI,      BTR_STATE_37_01},  ///< 入れ替えて出てきたポケモンに毒（２段階）
    {BTL_SIDEEFF_STEALTHROCK,   BTR_STATE_32_01},  ///< 入れ替えて出てきたポケモンにダメージ（相性計算あり）
    {BTL_SIDEEFF_RAINBOW,       BTR_STATE_66_01},  ///< 合体ワザ効果・虹（追加効果の発生率が倍になる）
    {BTL_SIDEEFF_BURNING,       BTR_STATE_68_01},  ///< 合体ワザ効果・火の海（炎タイプ以外に毎ターン1/8ダメージ）
    {BTL_SIDEEFF_MOOR,          BTR_STATE_67_01},  ///< 合体ワザ効果・湿原（素早さ 1/4）
    {BTL_SIDEEFF_NEBANEBANET,   BTR_STATE_40_01},  ///< ねばねばネット
    {BTL_SIDEEFF_AURORAVEIL,    BTR_STATE_74_01},  ///< オーロラベール
  };

bool BtlvConventionWin::IsExistSideEffect(void)
{
  const SideEffectStatus* st;
  int eff_num = GFL_NELEMS(TARGET_SIDE_EFFECT);

  mDispInfoCount = 0;

  for( int s=BTL_SIDE_MIN; s<BTL_SIDE_NUM ; s++ )
    {
      for( int i=0; i<eff_num ; i++ )
        {
          st = mBtlvCore->GetSideEffectStatus( (BtlSide)s, (BtlSideEffect)(TARGET_SIDE_EFFECT[i].effect_num) );

          if( st != NULL )
            {
              if( st->IsEffective() && (mDispInfoCount < DISP_INFO_LIST_MAX) )
                {
                  mDispInfoList[mDispInfoCount].side = s;
                  mDispInfoList[mDispInfoCount].msg_id = TARGET_SIDE_EFFECT[i].message_id;

                  u32 numerator, denominator;

                  // @fix GFNMCat[5261]
                  switch( TARGET_SIDE_EFFECT[i].effect_num )
                    {
                    case BTL_SIDEEFF_DOKUBISI:
                    case BTL_SIDEEFF_MAKIBISI:
                      // どくびしとまきびしは重ねがけ数
                      {
                        mDispInfoList[mDispInfoCount].turn = st->GetAddCount();
                        mDispInfoList[mDispInfoCount].ex_type = 1;
                      }
                      break;

                    case BTL_SIDEEFF_REFRECTOR:
                      // リフレクター
                      {
                        mBtlvCore->GetUiDisplay_Turn_Reflector( &numerator, &denominator, s );
                        mDispInfoList[mDispInfoCount].turn = numerator;
                        mDispInfoList[mDispInfoCount].ex_type = 0;
                      }
                      break;

                    case BTL_SIDEEFF_HIKARINOKABE:
                      // ひかりのかべ
                      {
                        mBtlvCore->GetUiDisplay_Turn_Hikarinokabe( &numerator, &denominator, s );
                        mDispInfoList[mDispInfoCount].turn = numerator;
                        mDispInfoList[mDispInfoCount].ex_type = 0;
                      }
                      break;

                    case BTL_SIDEEFF_AURORAVEIL:
                      // オーロラベール
                      {
                        mBtlvCore->GetUiDisplay_Turn_AuroraVeil( &numerator, &denominator, s );
                        mDispInfoList[mDispInfoCount].turn = numerator;
                        mDispInfoList[mDispInfoCount].ex_type = 0;
                      }
                      break;

                    default:
                      {
                        u32 t = st->GetMaxTurnCount();
                        mDispInfoList[mDispInfoCount].turn = (t - st->GetCurrentTurnCount());
                        mDispInfoList[mDispInfoCount].ex_type = 0;
                      }
                      break;

                    }
                    
                  mDispInfoCount++;
                }
            }
        }
    }


  if( mDispInfoCount > 0 )
    {
      return true;
    }
    
  return false;
};

void BtlvConventionWin::SetWindowInfo_SideEffect( int pos )
{
  // Clear text
  ClearWindowTextBox( pos );

  int cur_idx = mSubSeq;

  u8 client_id;
  int side = mDispInfoList[cur_idx].side;


  { // @fix NMCat[850] 味方場強化の技のバトル大会演出で相手のトレーナー名が表示される

    // BtlSide -> BtlPokePos -> clientID　変換
    BtlPokePos   bp_pos = mBtlvCore->GetMainModule()->GetSidePos( (BtlSide)side, 0);
    client_id = mBtlvCore->GetMainModule()->BtlPosToClientID( bp_pos );

  }

  // プレイヤーデータ取得
  const Savedata::MyStatus *myStatus;
  myStatus = mBtlvCore->GetClientPlayerData( client_id );

  // プレイヤー名
  SetTextboxPanePlayerName( pos, myStatus, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  // サイドエフェクト名
  SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00 );

  if( 1 == mDispInfoList[cur_idx].ex_type )
    {
      // 重ねがけ数（＋数字)
      SetTextboxPaneNumber( pos, BTLSTR_UI_LVUP, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, mDispInfoList[cur_idx].turn, 2); 
    }
  else
    {
      // （数字）ターン
      int turn = mDispInfoList[cur_idx].turn;
      if( turn > 0 )
        {
          SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, turn, 1); 
        }
    }

  // 陣営番号
  //  SetTextboxPaneNumber( pos, BTLSTR_UI_GAUGE_HP_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_02, mDispInfoList[cur_idx].side, 1);

}

// ----------------------------------------------------------
// 位置エフェクト

// チェック対象の位置エフェクト
BtlvConventionWin::EffectInfo TARGET_POSITION_EFFECT[] = 
  {
    {BTL_POSEFF_NEGAIGOTO,    BTR_STATE_39_01},  // ねがいごと
    {BTL_POSEFF_DELAY_ATTACK, BTR_STATE_60_01},  // みらいにこうげき
  };

bool BtlvConventionWin::IsExistPositionEffect(void)
{
  const PosEffectStatus* st;
  int pf_num = GFL_NELEMS(TARGET_POSITION_EFFECT);

  mDispInfoCount = 0;

  for( int s=BTL_SIDE_MIN; s<BTL_SIDE_NUM ; s++ )
    {
      for( int i=0; i<pf_num ; i++ )
        {
          st = mBtlvCore->GetPosEffectStatus( (BtlSide)s, (BtlPosEffect)(TARGET_POSITION_EFFECT[i].effect_num) );

          if (st != NULL)
            {
              if (st->IsEffective() && (mDispInfoCount < DISP_INFO_LIST_MAX) )
                {
                  PosEffect::EffectParamType ptype = PosEffect::GetEffectParamType( (BtlPosEffect)(TARGET_POSITION_EFFECT[i].effect_num) );

                  if( ptype == PosEffect::PARAM_TYPE_DELAY_ATTACK )
                    {
                      const PosEffect::EffectParam &param = st->GetEffectParam();
                      mDispInfoList[mDispInfoCount].turn = param.DelayAttack.execTurnMax - param.DelayAttack.execTurnCount;
                      // 表示で最後のターンが１になるように+1
                      mDispInfoList[mDispInfoCount].turn++;
                    }
                  else
                    {
                      mDispInfoList[mDispInfoCount].turn = 0;
                    }

                  mDispInfoList[mDispInfoCount].side = s;
                  mDispInfoList[mDispInfoCount].msg_id = TARGET_POSITION_EFFECT[i].message_id;
                  
                  mDispInfoCount++;
                }
            }
        }
    }

  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;
};

void BtlvConventionWin::SetWindowInfo_PositionEffect( int pos )
{
  // Clear text
  ClearWindowTextBox( pos );

  int cur_idx = mSubSeq;
  {
    // 位置エフェクト名
    SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );
  }

  // （数字）ターン
  int turn = mDispInfoList[cur_idx].turn;
  if( turn > 0 )
    {
      SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, turn, 1); 
    }

  // 陣営番号
  //  SetTextboxPaneNumber( pos, BTLSTR_UI_GAUGE_HP_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, mDispInfoList[cur_idx].side, 1);

}

// ----------------------------------------------------------
// 場の効果

// チェック対象の場のエフェクト
BtlvConventionWin::EffectInfo TARGET_PLACE_EFFECT[] = 
  {
    {FieldStatus::EFF_TRICKROOM,     BTR_STATE_08_01},   ///< トリックルーム
    {FieldStatus::EFF_JURYOKU,       BTR_STATE_29_01},   ///< じゅうりょく
    {FieldStatus::EFF_FUIN,          BTR_STATE_46_01},   ///< ふういん
    {FieldStatus::EFF_MIZUASOBI,     BTR_STATE_49_01},   ///< みずあそび
    {FieldStatus::EFF_DOROASOBI,     BTR_STATE_38_01},   ///< どろあそび
    {FieldStatus::EFF_WONDERROOM,    BTR_STATE_10_01},   ///< ワンダールーム
    {FieldStatus::EFF_MAGICROOM,     BTR_STATE_09_01},   ///< マジックルーム
    {FieldStatus::EFF_FAIRY_LOCK,    BTR_STATE_65_01},   ///< フェアリーロック
  };


bool BtlvConventionWin::IsExistPlaceEffect(void)
{
  const FieldStatus *fstatus = mBtlvCore->GetFieldStatus();
  GFL_ASSERT( fstatus );

  int pl_num = GFL_NELEMS(TARGET_PLACE_EFFECT);
  mDispInfoCount = 0;

  for(int i=0; i<pl_num ;i++)
    {
      FieldStatus::EffectType et = (FieldStatus::EffectType)(TARGET_PLACE_EFFECT[i].effect_num); 

      if ( fstatus->CheckEffect( et ) )
        {
          mDispInfoList[mDispInfoCount].turn = fstatus->CheckRemainingTurn( et );
          mDispInfoList[mDispInfoCount].msg_id =TARGET_PLACE_EFFECT[i].message_id;
          mDispInfoList[mDispInfoCount].side = -1 ;

          // @fix NMCat[3600] フェアリーロックはターン数を表示しない
          if( FieldStatus::EFF_FAIRY_LOCK == et )
            {
              mDispInfoList[mDispInfoCount].turn = 0;
            }

          mDispInfoCount++;
        }
    }

  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;
}

void BtlvConventionWin::SetWindowInfo_PlaceEffect( int pos )
{

  // Clear text
  ClearWindowTextBox( pos );


  int cur_idx = mSubSeq;
  {
    // 効果名
    SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );
  }

  // （数字）ターン：０は非表示
  int turn = mDispInfoList[cur_idx].turn;
  if( turn > 0 )
    {
      SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, turn, 1); 
    }
}

// ----------------------------------------------------------
// ポケモン状態異常

// チェック対象のワザ状態異常
BtlvConventionWin::EffectInfo TARGET_WAZASICK_EFFECT[] = 
  {
    {pml::wazadata::WAZASICK_KONRAN,        BTR_STATE_12_01},     // こんらん
    {pml::wazadata::WAZASICK_MEROMERO,      BTR_STATE_13_01},     // メロメロ
    {pml::wazadata::WAZASICK_AKUMU,         BTR_STATE_14_01},     // あくむ
    {pml::wazadata::WAZASICK_AKUBI,         BTR_STATE_15_01},     // ねむけ(あくび)
    {pml::wazadata::WAZASICK_ENCORE,        BTR_STATE_16_01},     // アンコール
    {pml::wazadata::WAZASICK_IEKI,          BTR_STATE_17_01},     // いえき
    {pml::wazadata::WAZASICK_ICHAMON,       BTR_STATE_18_01},     // いちゃもん
    {pml::wazadata::WAZASICK_KAIHUKUHUUJI,  BTR_STATE_22_01},     // かいふくふうじ
    {pml::wazadata::WAZASICK_MIYABURU,      BTR_STATE_23_01},     // みやぶられている
    {pml::wazadata::WAZASICK_KANASIBARI,    BTR_STATE_24_01},     // かなしばり
    {pml::wazadata::WAZASICK_TOOSENBOU,     BTR_STATE_25_01},     // にげられない
    {pml::wazadata::WAZASICK_SASIOSAE,      BTR_STATE_27_01},     // さしおさえ
    {pml::wazadata::WAZASICK_TYOUHATSU,     BTR_STATE_34_01},     // ちょうはつ
    {pml::wazadata::WAZASICK_FLYING,        BTR_STATE_36_01},     // でんじふゆう
    {pml::wazadata::WAZASICK_NOROI,         BTR_STATE_42_01},     // のろい
    {pml::wazadata::WAZASICK_HOROBINOUTA,   BTR_STATE_47_01},     // ほろびのうた
    {pml::wazadata::WAZASICK_YADORIGI,      BTR_STATE_52_01},     // やどりぎのたね
    {pml::wazadata::WAZASICK_BIND,          BTR_STATE_54_01},     // バインド
    {pml::wazadata::WAZASICK_ABARERU,       BTR_STATE_57_01},     // あばれる
    {pml::wazadata::WAZASICK_SAWAGU,        BTR_STATE_61_01},     // さわぐ
    {pml::wazadata::WAZASICK_AQUARING,      BTR_STATE_62_01},     // アクアリング
    {pml::wazadata::WAZASICK_NEWOHARU,      BTR_STATE_41_01},     // ねをはる
    {pml::wazadata::WAZASICK_FLYING_CANCEL, BTR_STATE_64_01},     ///< ふゆう状態になれない（うちおとす）
    {pml::wazadata::WAZASICK_MUSTHIT,       BTR_STATE_26_01},     ///< ワザ必中状態（表示はロックオン）
    {pml::wazadata::WAZASICK_MUSTHIT_TARGET,BTR_STATE_26_01},     ///< 特定相手に対してのみ必中状態（表示はロックオン）
    {pml::wazadata::WAZASICK_ZIGOKUDUKI,    BTR_STATE_73_01},     ///< じごくづき( 音技を出せない )
    {pml::wazadata::WAZASICK_TELEKINESIS,   BTR_STATE_35_01},     ///< テレキネシス
    {pml::wazadata::WAZASICK_TOGISUMASU,    BTR_STATE_75_01},     ///< とぎすます
    {pml::wazadata::WAZASICK_SPECIAL_CODE,  BTR_STATE_33_01},     ///< 「たくわえる」専用例外処理

  };

bool BtlvConventionWin::IsExistPokeSick(void)
{

  BTL_POKEPARAM* pPoke_param;

  mDispInfoCount = 0;
  int ws_num = GFL_NELEMS(TARGET_WAZASICK_EFFECT);

  for(int i=0; i<mPokemonNum; i++)
    {

      pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)i );

      if( pPoke_param )
        {

          for(int j=0;j<ws_num; j++)
            {
              WazaSick sickType = (WazaSick)(TARGET_WAZASICK_EFFECT[j].effect_num);
              u32 msgId = TARGET_WAZASICK_EFFECT[j].message_id;

              if( (sickType == pml::wazadata::WAZASICK_SPECIAL_CODE) && (msgId == BTR_STATE_33_01) )
                {
                  // 「たくわえる」専用処理
                  u8 t_count = pPoke_param->COUNTER_Get( BTL_POKEPARAM::COUNTER_TAKUWAERU );
                  if( t_count > 0 )
                    {
                      mDispInfoList[mDispInfoCount].side = i;
                      mDispInfoList[mDispInfoCount].turn = t_count;
                      mDispInfoList[mDispInfoCount].msg_id = TARGET_WAZASICK_EFFECT[j].message_id;
                      mDispInfoList[mDispInfoCount].ex_type = 1;
              
                      mDispInfoCount++;
                    }
                }
              else
                {
                  if( pPoke_param->CheckSick( sickType ) )
                    {
                      u8 past_turn = pPoke_param->GetSickTurnCount( sickType );
                      const BTL_SICKCONT &scount = pPoke_param->GetSickCont( sickType );
                      u8 turn_max = SICCONT_GetTurnMax( scount );
 
                      mDispInfoList[mDispInfoCount].side = i;
                      //                      mDispInfoList[mDispInfoCount].turn = turn_max - past_turn;
                      mDispInfoList[mDispInfoCount].msg_id = TARGET_WAZASICK_EFFECT[j].message_id;
                      mDispInfoList[mDispInfoCount].ex_type = 0;

                      switch( sickType )
                        {
                        case pml::wazadata::WAZASICK_BIND:
                        case pml::wazadata::WAZASICK_KONRAN:
                        case pml::wazadata::WAZASICK_ABARERU:
                        case pml::wazadata::WAZASICK_MUSTHIT:
                        case pml::wazadata::WAZASICK_MUSTHIT_TARGET:
                        case pml::wazadata::WAZASICK_AKUBI:
                        case pml::wazadata::WAZASICK_TOGISUMASU:
                          // @fix NMCat[3421] BINDはターン数非表示
                          // @fix NMCat[3600] こんらん、ロックオン、こころのめ、あばれる、ねむけ（あくび） はターン数非表示
                          // @fix NMCat[3601] とぎすます はターン数非表示
                          {
                            mDispInfoList[mDispInfoCount].turn = 0;
                          }
                          break;

                        default:
                          {
                            mDispInfoList[mDispInfoCount].turn = turn_max - past_turn;
                          }
                          break;
                          
                        }
             
                      mDispInfoCount++;
                    }

                }

              // 最大数まできたら打ち切り
              if (mDispInfoCount >= DISP_INFO_LIST_MAX)
                {
                  return true;
                }

            }
        }
    }

  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;
};

void BtlvConventionWin::SetWindowInfo_PokeSick( int pos )
{

  // Clear text
  ClearWindowTextBox( pos );

  int cur_idx = mSubSeq;

  // Pokemon Nickname
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  int poke_pos = mDispInfoList[cur_idx].side;
  BTL_POKEPARAM *pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)poke_pos );
  const pml::pokepara::PokemonParam* ppView = pPoke_param->GetViewSrcData();
  ppView->GetNickName( msgStr );
  
  SetTextboxPaneMessage( pos, msgStr, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );


  // 異常名
  SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00);

  if( 1 == mDispInfoList[cur_idx].ex_type )
    {
      // たくわえた数（＋数字)
      SetTextboxPaneNumber( pos, BTLSTR_UI_LVUP, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, mDispInfoList[cur_idx].turn, 2); 
    }
  else
    {
      // （数字）ターン
      int turn = mDispInfoList[cur_idx].turn;
      if( turn > 0 )
        {
          SetTextboxPaneNumber( pos, BTLSTR_PROFILE_03_01, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, turn, 1); 
        }
    }

}

// ----------------------------------------------------------
// ポケモンタイプ拡張

// 文字列ID取得用：他と使い方が違うので注意
BtlvConventionWin::EffectInfo TARGET_TYPE_EX[] = 
  {
    {BTL_POKEPARAM::EXTYPE_CAUSE_NONE,            BTR_STATE_17_01},   // Dummy
    {BTL_POKEPARAM::EXTYPE_CAUSE_HALLOWEEN,       BTR_STATE_43_01},   // 技「ハロウィン」
    {BTL_POKEPARAM::EXTYPE_CAUSE_MORINONOROI,     BTR_STATE_51_01},   // 技「もりののろい」
  };


bool BtlvConventionWin::IsExistPokeExType(void)
{

  BTL_POKEPARAM* pPoke_param;

  mDispInfoCount = 0;

  for(int i=0; i<mPokemonNum; i++)
    {
      pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)i );

      if( pPoke_param )
        {
          // @fix NMCat[4067]
          // ひんしの場合は対象外
          if( pPoke_param->IsDead() == false )
            {
              if( pPoke_param->HaveExType() )
                {
                  
                  BTL_POKEPARAM::ExTypeCause tc= pPoke_param->GetExTypeCause();
                  
                  // HaveExType()==trueでもEXTYPE_CAUSE_NONEなら無いものとして扱う
                  if( BTL_POKEPARAM::EXTYPE_CAUSE_NONE == tc )
                    {
                      continue;
                    }
                  
                  mDispInfoList[mDispInfoCount].side = i;
                  mDispInfoList[mDispInfoCount].turn = -1;
                  mDispInfoList[mDispInfoCount].msg_id = TARGET_TYPE_EX[tc].message_id;
                  mDispInfoCount++;
                  
                }
            }
        }
    }


  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;

};

void BtlvConventionWin::SetWindowInfo_PokeExType( int pos )
{
  // Clear text
  ClearWindowTextBox( pos );

  int cur_idx = mSubSeq;

  // Pokemon Nickname
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  int poke_pos = mDispInfoList[cur_idx].side;
  BTL_POKEPARAM *pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)poke_pos );
  const pml::pokepara::PokemonParam* ppView = pPoke_param->GetViewSrcData();
  ppView->GetNickName( msgStr );
  
  SetTextboxPaneMessage( pos, msgStr, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

 
  // 原因名
  SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00 );


}

// ----------------------------------------------------------
// ポケモン継続フラグ

// チェック対象
BtlvConventionWin::EffectInfo TARGET_CONT_FLAG[] = 
  {
    {BTL_POKEPARAM::CONTFLG_ONNEN,      BTR_STATE_21_01},      ///< 技「おんねん」発動中
    {BTL_POKEPARAM::CONTFLG_MITIDURE,   BTR_STATE_50_01},      ///< 技「みちづれ」発動中
    {BTL_POKEPARAM::CONTFLG_JUDEN,      BTR_STATE_28_01},      ///< 技「じゅうでん」発動中
    {BTL_POKEPARAM::CONTFLG_GAMAN,      BTR_STATE_55_01},      ///< 技「がまん」発動中
    {BTL_POKEPARAM::CONTFLG_BODYPURGE,  BTR_STATE_63_01},      ///< 技「ボディパージ」により体重が軽くなっている
    {BTL_POKEPARAM::CONTFLG_KIAIDAME,   BTR_STATE_11_01},      ///< 技「きあいだめ」（表示は「きゅうしょアップ」）



  };

bool BtlvConventionWin::IsExistPokeContinue(void)
{

  BTL_POKEPARAM* pPoke_param;

  mDispInfoCount = 0;
  int cf_num = GFL_NELEMS(TARGET_CONT_FLAG);

  for(int i=0; i<mPokemonNum; i++)
    {
      pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)i );

      if( pPoke_param )
        {
          for (int j=0; j<cf_num; j++)
            {
              BTL_POKEPARAM::ContFlag f = (BTL_POKEPARAM::ContFlag)(TARGET_CONT_FLAG[j].effect_num);

              if( pPoke_param->CONTFLAG_Get( f ) )
                {
                  mDispInfoList[mDispInfoCount].side = i;
                  mDispInfoList[mDispInfoCount].turn = -1;
                  mDispInfoList[mDispInfoCount].msg_id = TARGET_CONT_FLAG[j].message_id;

                  mDispInfoCount++;
                  continue;
                }
            }
        }

    }

  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;

}

void BtlvConventionWin::SetWindowInfo_PokeContinue( int pos )
{
  // Clear text
  ClearWindowTextBox( pos );

  int cur_idx = mSubSeq;

  // Pokemon Nickname
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);

  int poke_pos = mDispInfoList[cur_idx].side;
  BTL_POKEPARAM *pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)poke_pos );

  const pml::pokepara::PokemonParam* ppView = pPoke_param->GetViewSrcData();
  ppView->GetNickName( msgStr );
  
  SetTextboxPaneMessage( pos, msgStr, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  
  // ワザ名
  SetTextboxPaneMessageSTATE( pos, mDispInfoList[cur_idx].msg_id, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00 );
   

}

// ----------------------------------------------------------
// ターン数

void BtlvConventionWin::SetWindowInfo_Turn( int pos )
{
  // Clear text
  ClearWindowTextBox( pos );


  // ターン数の取得
  int current_turn = mBtlvCore->GetTurnCount() + 1; // ターン数は０オリジン

  // げんざいのターンすう
  SetTextboxPaneMessageUI( pos, BTLSTR_PROFILE_TURN, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  // （数字３桁）ターン
  SetTextboxPaneNumber( pos, BTLSTR_PROFILE_TURN_00, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, current_turn, 3); 

}
 

// ----------------------------------------------------------
// ポケモンのタイプ

bool BtlvConventionWin::IsExistPokeType(void)
{

  BTL_POKEPARAM* pPoke_param;
  mDispInfoCount = 0;

  for(int i=0; i<mPokemonNum; i++)
    {
      pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)i );
      
      if( pPoke_param )
        {
          // @fix NMCat[4067]
          // ひんしの場合は対象外
          if( pPoke_param->IsDead() == false )
            {
              
              // ポケモン位置だけリストに入れている
              mDispInfoList[mDispInfoCount].side = i;
              
              mDispInfoCount++;
            }
        }
    }
  
  if( mDispInfoCount > 0 )
    {
      return true;
    }

  return false;

}

void BtlvConventionWin::SetWindowInfo_PokeType( int pos )
{

  // Clear text
  ClearWindowTextBox( pos );


  int cur_idx = mSubSeq;

  // Pokemon Nickname
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);

  int poke_pos = mDispInfoList[cur_idx].side;
  BTL_POKEPARAM *pPoke_param = mBtlvCore->GetBattlePokeParam( (BtlvPos)poke_pos );

  const pml::pokepara::PokemonParam* ppView = pPoke_param->GetViewSrcData();
  ppView->GetNickName( msgStr );

  // Set Nickname
  SetTextboxPaneMessage( pos, msgStr, PANENAME_BATTLE_PLT_LOW_007_PANE_NAME );

  // @fix NMCat[3318]   
  pml::PokeType  type1,type2;
  mBtlvCore->GetUiDisplay_PokeType( &type1, &type2, *pPoke_param );

  // @fix NMcat[3858]
  // タイプが無い場合は表示しない
  if( type1 >= POKETYPE_MAX )
    {
      return;
    }

  // タイプ名
  SetTextboxPanePokeType( pos, BTLSTR_PROFILE_TYPE, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_00, type1 );

  // タイプが１つの場合は両方同じタイプが入っているので表示しない
  if( type1 != type2 )
    {
      SetTextboxPanePokeType( pos, BTLSTR_PROFILE_TYPE, PANENAME_BATTLE_PLT_LOW_007_PANE_INFO_01, type2 );
    }

}

GFL_NAMESPACE_END(btl)
