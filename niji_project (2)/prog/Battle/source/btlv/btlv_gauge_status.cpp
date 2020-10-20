//======================================================================
/**
 * @file btvl_gauge_status.cpp
 * @date 15/06/11
 * @author ariizumi_nobuhiko
 * @brief バトル描画 上画面ゲージ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>

#include <niji_conv_header/app/battle/upper/minupp_pane.h>
#include <niji_conv_header/message/msg_btl_std.h>
#include <niji_conv_header/message/msg_btl_ui.h>

#include "AppLib/include/Tool/app_tool_Gauge.h"
#include "PokeTool/include/PokeTool.h"

#include "app/BattleUIDataManager.h"
#include "./btlv_gauge_status.h"
#include "./btlv_core.h"

#include "../btl_pokeparam.h"

#if KANTAIHANTAI_TEST
#include <arc_def_index/arc_def.h>
#include "System/include/HeapDefine.h"
#include "arc_index/message_Simp_Chinese.gaix"
#include "arc_index/message_Trad_Chinese.gaix"
#endif

#if PM_DEBUG
#include "../btl_mainmodule.h"
#include  "Battle/include/battle_SetupParam.h"
#endif

GFL_NAMESPACE_BEGIN(btl)

BtlvGaugeBase::BtlvGaugeBase( BattleViewSystem *btlvCore , const INIT_PARAM &param , int idx )
:mBtlvCore(btlvCore)
,mDataManager(param.dataManager)
,mG2dUtil(param.g2dUtil)
,mLytIndex(param.lytIdx)
,mGaugeLyt(param.lyt)
,mGaugeParts(param.partsLyt)
,mBaseLyt(param.baseLyt)
,mIdx(idx)
,mIsGaugeIn(false)
,mIsUpdateExp(false)
{
  mParam = NULL;

  for( int i=0;i<ANM_MAX;i++ )
  {
    mAnmTbl[i] = param.anmTbl[i];
  }
  for( int i=0;i<PARTS_MAX;i++ )
  {
    mSubPartsLyt[i] = param.subPartsLyt[i];
  }
  
  mRoyalGloveLyt = param.royalGloveLyt;

  mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_SICK_LOOP] );
  mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_HP_VALUE]);


#if KANTAIHANTAI_TEST
  gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
  mpAppHeap = GFL_NEW( devHeap ) ::app::util::Heap();
	mpAppHeap->LocalHeapCreate(devHeap,devHeap,0x10000,0x10000);
#endif //KANTAIHANTAI_TEST

#if PM_DEBUG

  if( mBtlvCore->GetBtlvMode() == BTLV_MODE_BATTLE )
  {
    bool isLiveRecPlayMode = mBtlvCore->GetMainModule()->GetBattleSetupParam()->isLiveRecPlayMode;
    if( isLiveRecPlayMode )
    {//観戦モード用にいくつか非表示にする
      gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
      gfl2::lyt::LytParts*  partsLyt  = NULL;

      if( mSubPartsLyt[PARTS_EXP_GAUGE] != -1 )
      {
        partsLyt = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_EXP_GAUGE] , &rMultiResId );
        mG2dUtil->SetPaneVisible(partsLyt,false);
      }

      if( mSubPartsLyt[PARTS_NUM_HP_MAX] != -1 )
      {
        partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_GAUGE1] , &rMultiResId );
        gfl2::lyt::LytTextBox *text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_MAX], &rMultiResId );
        text->SetVisible( false );
        text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_MAX], &rMultiResId );
        text->SetVisible( false );
        text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_MIN], &rMultiResId );
        text->SetVisible( false );
        text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_SLASH], &rMultiResId );
        text->SetVisible( false );
      }
    }
  }
#endif
}

BtlvGaugeBase::~BtlvGaugeBase()
{
#if KANTAIHANTAI_TEST
  GFL_SAFE_DELETE(mpAppHeap);
#endif
}




void BtlvGaugeBase::Update(void)
{
  if( mParam )
  {
    if( mUpdateReq )
    {
      UpdateParam();
      mUpdateReq = false;
    }

    if( mIsDamageAnime )
    {

      int diff = mDispTrgHp - mDispAnmHp;
      //０除算注意！
      f32 val = ((f32)diff*mDamageAnimeCnt)/(f32)mDamageAnimeCntMax;
      f32 dispHpF32 = mDispAnmHp + val;
      mDispHp = mDispAnmHp + val;

      //ARIIZUMI_PRINT("[%d->%d]:[%d(+%f)]\n",mDispAnmHp,mDispTrgHp,mDispHp,val);
        
      const int maxhp = mParam->GetValue(BTL_POKEPARAM::BPP_MAX_HP);

      if( mDamageAnimeCnt < mDamageAnimeCntMax )
      {
        mDamageAnimeCnt++;
      }

      if( mDamageAnimeCnt >= mDamageAnimeCntMax )
      {
        mDispHp = mDispTrgHp;
        dispHpF32 = mDispTrgHp;
        mIsDamageAnime = false;
        UpdateParam();
      }

      SetHpGauge( maxhp , mDispHp , dispHpF32 );
    }

    if( mIsUpdateExp )
    {
      mDispExpRate+=2;
      if( mDispExpRate >= mDispTrgExpRate )
      {
        mDispExpRate = mDispTrgExpRate;
      }
      UpdateExpGauge();
      
      if( mExpAnimCnt <= 0 )
      {
        if( mDispExpRate == mDispTrgExpRate )
        {
          mIsUpdateExp = false;
          mBtlvCore->StopSe( SEQ_SE_BT_EXP );
          if( mDispExpRate == 100 )
          {
            //満タンの時は1フレーム待つ
            mBtlvCore->PlaySe( SEQ_SE_BT_EXPMAX );
            mDispExpRate = 0;
	    //            mReserveUpdate = true;
            //ここでUpdateReqを出すと、次のパラメータがちょっと見えるから
          }
        }
      }
      else
      {
        mExpAnimCnt--;
      }
    }
  }
}

void BtlvGaugeBase::SetPokeParam(BTL_POKEPARAM *param)
{
  mParam = param;

  if( mParam->IsDead() == true )
  {
    HideGauge();
  }
  else
  {
    DispGauge();
  }

  UpdateReq();
}

#if KANTAIHANTAI_TEST

void BtlvGaugeBase::TestChainaMessage(int monsno)
{
  //::app::util::G2DUtil* pG2dUtil = mG2dUtil;
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
 // gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );


	{
    gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DEBUG );
    print::SystemFont_CreateBattleFont(devHeap,devHeap);
    print::SystemFont_SetLytSystemBattleFont();
  }


  gfl2::lyt::LytTextBox *btlnamepane = mGaugeLyt->GetTextBoxPane( mGaugeParts, PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_00, &rMultiResId );

  gfl2::str::MsgData* pMsgData = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::MsgData( ARCID_MESSAGE_TWN,
                                                                                            GARC_message_Trad_Chinese_monsname_DAT,
                                                                                            mpAppHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

  gfl2::str::StrBuf* strbuf = GFL_NEW( mpAppHeap->GetDeviceHeap() ) gfl2::str::StrBuf( 200, mpAppHeap->GetDeviceHeap() );

  pMsgData->GetString(monsno, *strbuf);

 // GFL_PRINT("mojicode  %x %x %x %x\n",strbuf->getCode(0),strbuf->getCode(1),strbuf->getCode(2),strbuf->getCode(3));

  print::SetStringForLytTextBox( btlnamepane, strbuf );

	GFL_DELETE strbuf;
  GFL_DELETE pMsgData;
  
}
#endif //KANTAIHANTAI_TEST

void BtlvGaugeBase::UpdateParam(void)
{
  const pml::pokepara::PokemonParam* ppView = mParam->GetViewSrcData();
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  if( ppView )
  {
    //名前の反映
    {
      gfl2::str::StrBuf str( pml::MONS_NAME_BUFFER_SIZE+10 , mBtlvCore->GetTempHeap()->GetLowerHandle());
      ppView->GetNickName( &str );

      gfl2::lyt::LytTextBox *text1 = mGaugeLyt->GetTextBoxPane( mGaugeParts, PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_00, &rMultiResId );

      print::SetStringForLytTextBox( text1 , &str );

#if KANTAIHANTAI_TEST  //中国語のテスト
      const pml::pokepara::PokemonParam* ppSrc = mParam->GetSrcData();
      TestChainaMessage(  ppSrc->GetMonsNo()  );
#endif//KANTAIHANTAI_TEST
    }
    //性別指定
    {
      gfl2::lyt::LytParts*  partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_GENDER_ICON] , &rMultiResId );
      if( PokeTool::GetDrawSex(ppView) == pml::SEX_MALE )
      {
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_GENDER_MALE] );
        mG2dUtil->SetPaneVisible(partsLyt,true);
      }
      else
      if( PokeTool::GetDrawSex(ppView) == pml::SEX_FEMALE )
      {
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_GENDER_FEMALE] );
        mG2dUtil->SetPaneVisible(partsLyt,true);
      }
      else
      {
        mG2dUtil->SetPaneVisible(partsLyt,false);
      }
    }
    //レベルの反映
    {
      const int lv = mParam->GetValue(BTL_POKEPARAM::BPP_LEVEL);
      gfl2::lyt::LytTextBox *text = mGaugeLyt->GetTextBoxPane( mGaugeParts, PANENAME_BATTLE_PLT_UPP_004_PANE_TEXTBOX_01, &rMultiResId );
      SetNumberText( text , BTLSTR_UI_GAUGE_LV , lv , 3 , false );
    }
    //HPバー反映
    {
      const int hp = mParam->GetValue(BTL_POKEPARAM::BPP_HP);
      const int maxhp = mParam->GetValue(BTL_POKEPARAM::BPP_MAX_HP);
      SetHpGauge( maxhp , hp , hp );
      mDispTrgHp = hp;
      mDispHp = hp;
    }
    //経験値バー反映
    if( mSubPartsLyt[PARTS_EXP_GAUGE] != -1 )
    {
      const pml::pokepara::PokemonParam* ppSrc = mParam->GetSrcData();
      const int lv = mParam->GetValue(BTL_POKEPARAM::BPP_LEVEL);
      pml::personal::LoadGrowTable( ppSrc->GetMonsNo() , ppSrc->GetFormNo() );

      const int nowLvExp = pml::personal::GetMinExp( lv );
      const int nextLvExp = pml::personal::GetMinExp( lv+1 );
      const int nowExp = mParam->GetValue(BTL_POKEPARAM::BPP_EXP);
      const int nowExpVal = nowExp - nowLvExp;
      const int expSub = nextLvExp - nowLvExp;

      if( expSub == 0 )
      {
        mDispExpRate = 0;
      }
      else
      {
      	mDispExpRate = nowExpVal*100/expSub;
      }

      mDispTrgExpRate = mDispExpRate;
      
      gfl2::lyt::LytParts*  partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_EXP_GAUGE] , &rMultiResId );
      gfl2::lyt::LytPicture *gaugeLyt = mGaugeLyt->GetPicturePane( partsLyt, PANENAME_BATTLE_GAG_001_PANE_GAUGE_EXP, &rMultiResId );

      nw::lyt::Size size = gaugeLyt->GetSize();
      size.width = mDispExpRate*EXP_GAUGE_SIZE*0.01;
      gaugeLyt->SetSize(size);
    }
    //状態異常反映
    {
      PokeSick sick = mParam->GetPokeSick();
      gfl2::lyt::LytParts*  partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_SICK_ICON] , &rMultiResId );
      if( sick == pml::pokepara::SICK_NULL )
      {
        mG2dUtil->SetPaneVisible(partsLyt,false);
      }
      else
      {
        mG2dUtil->SetPaneVisible(partsLyt,true);

        gfl2::lyt::LytPicture *sickPane = mGaugeLyt->GetPicturePane( partsLyt, PANENAME_COMMON_ICN_002_PANE_ICON_SICK, &rMultiResId );
        if( mParam->CheckMoudoku() )
        {
          mDataManager->GetAppCommonGrpIconData().ReplacePaneTextureByPokeDokudokuIcon(sickPane);
        }
        else
        {
          mDataManager->GetAppCommonGrpIconData().ReplacePaneTextureByPokeSickIcon(sick,sickPane);
        }
      }
    }

    //アイコン系
    bool visibleCaptureIcon = false;
    if( mBtlvCore->GetBattleCompetitor() == BTL_COMPETITOR_WILD &&
        mBtlvCore->IsPlayerSide(static_cast<BtlvPos>(mIdx)) == false )
    {
      if( mBtlvCore->IsZukanRegistered(mParam) )
      {
        visibleCaptureIcon = true;
      }
    }
    gfl2::lyt::LytPicture* ballPane  = mGaugeLyt->GetPicturePane( mGaugeParts , mSubPartsLyt[PARTS_CAPTURE_ICON] , &rMultiResId );
    mG2dUtil->SetPaneVisible(ballPane,visibleCaptureIcon);

    gfl2::lyt::LytParts* partsPane  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_MEGA_ICON] , &rMultiResId );
    if( ppView && ppView->IsMegaEvolved() ) // @fix cov_ctr[11026] ppViewがNULLになる可能性があるため
    {
      mG2dUtil->SetPaneVisible(partsPane,true);
      mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_ICON_MEGA_EVOLVE] );
    }
    else if( ppView->GetMonsNo() == MONSNO_GURAADON &&
         ppView->GetFormNo() == FORMNO_GURAADON_OMEGA )
    {
      mG2dUtil->SetPaneVisible(partsPane,true);
      mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_ICON_GENSIKAIKI_OMEGA] );
    }
    else if( ppView->GetMonsNo() == MONSNO_KAIOOGA &&
          ppView->GetFormNo() == FORMNO_KAIOOGA_ALPHA )
    {
      mG2dUtil->SetPaneVisible(partsPane,true);
      mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_ICON_GENSIKAIKI_ALPHA] );
    }
    else
    {
      mG2dUtil->SetPaneVisible(partsPane,false);
    }
  }

  mBtlvCore->CheckPinchBgmReq();
}

void BtlvGaugeBase::SetNumberText( gfl2::lyt::LytTextBox *textPane , u16 strId , int val , int keta , bool isSmall )
{
  gfl2::str::StrBuf *msgStr = mBtlvCore->GetWorkStr(0);
  gfl2::str::StrBuf *expStr = mBtlvCore->GetWorkStr(1);
  print::WordSet *wset = mBtlvCore->GetWordSet();
      
  mBtlvCore->GetStringUi( msgStr , strId );
  if( isSmall )
  {
    wset->RegisterNumber( 0 , val , keta , print::NUM_DISP_LEFT , print::NUM_CODE_ZENKAKU );
  }
  else
  {
    wset->RegisterNumber( 0 , val , keta , print::NUM_DISP_LEFT , print::NUM_CODE_HANKAKU );
  }
  wset->Expand( expStr , msgStr );

  print::SetStringForLytTextBox( textPane , expStr );
}

void BtlvGaugeBase::DispGauge(void)
{
  if( mParam->IsDead() == false ||
      mDispHp != 0 )
  {
    mG2dUtil->SetPaneVisible(mBaseLyt,true);
  }
}

void BtlvGaugeBase::HideGauge(void)
{
  mG2dUtil->SetPaneVisible(mBaseLyt,false);
}

bool BtlvGaugeBase::StartGaugeInAnime(void)
{
  if( mIsGaugeIn == false )
  {
    mIsGaugeIn = true;
    mG2dUtil->StartAnime( 0,mAnmTbl[ANM_GAUGE_IN] );
    return true;  //音の判定のため、実際にアニメしたかを返す
  }
  return false;
}

bool BtlvGaugeBase::WaitGaugeInAnime(void)
{
  return mG2dUtil->IsAnimeEnd( 0,mAnmTbl[ANM_GAUGE_IN] );
}


void BtlvGaugeBase::StartDamageAnime(int cnt)
{
  const int hp = mParam->GetValue(BTL_POKEPARAM::BPP_HP);
  const int maxhp = mParam->GetValue(BTL_POKEPARAM::BPP_MAX_HP);
  
  if( mBtlvCore->IsRecordSkipMode() )
  {
    //バトルレコーダースキップ中は最後にアップデートするから無視。
    return;
  }
  
  if( mDispTrgHp != hp )
  {
    DispGauge();
    mIsDamageAnime = true;
    mDamageAnimeCnt = 0;
    mDamageAnimeCntMax = cnt;
    mDamageAnimeDelayCnt = 5;
    //UpdateParam();

    //const int rate = hp*100/maxhp;
    ARIIZUMI_PRINT("[%d][%d]\n",mDispTrgHp,hp);
    mDispAnmHp = mDispHp;
    mDispTrgHp = hp;

    //momijiでゲージの移動量を少ないときに早くする対応
    static const f32 HP_GAUGE_SIZE = 86;  //ゲージサイズ
    f32 diffHP = gfl2::math::FAbs(mDispAnmHp - mDispTrgHp);
    f32 diffDot = HP_GAUGE_SIZE * diffHP / maxhp;
    if (diffDot<mDamageAnimeCntMax)
    {
      mDamageAnimeCntMax = diffDot;
    }
  }
}

//効果抜群のアニメ
void BtlvGaugeBase::PlayBatugunAnime(void)
{
  mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_BATSUGUN] );
}

void BtlvGaugeBase::SetHpGauge( int maxHp , int hp , f32 hpF32)
{
  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  gfl2::lyt::LytParts*  partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_GAUGE1] , &rMultiResId );


  //ゲージ
  {
    static const int HP_GAUGE_CNT = 100;
  
    f32 frame = HP_GAUGE_CNT - ::app::tool::Gauge::GetDottoNumF32( hpF32 , maxHp , HP_GAUGE_CNT );

    mG2dUtil->SetAnimeFrame(mLytIndex,mAnmTbl[ANM_HP_VALUE],frame );

    ::app::tool::Gauge::GaugeColor gaugeCol = ::app::tool::Gauge::GetDottoColor( hp , maxHp );
    nw::ut::Color8 col;
  
    switch( gaugeCol )
    {
      case ::app::tool::Gauge::GAUGE_COLOR_GREEN:
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_HP_GREEN] );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_YELLOW:
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_HP_YELLOW] );
        break;
      case ::app::tool::Gauge::GAUGE_COLOR_RED:
      case ::app::tool::Gauge::GAUGE_COLOR_NULL:
        mG2dUtil->StartAnime( mLytIndex,mAnmTbl[ANM_HP_RED] );
        break;
    }
  }

  //数字
  if( mSubPartsLyt[PARTS_NUM_HP_MAX] != -1 )
  {
    {
      gfl2::lyt::LytTextBox *text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_MAX], &rMultiResId );
      SetNumberText( text , BTLSTR_UI_GAUGE_HP_03 , maxHp , 3 , true );
    }
    {
      gfl2::lyt::LytTextBox *text = mGaugeLyt->GetTextBoxPane( partsLyt, mSubPartsLyt[PARTS_NUM_HP_MIN], &rMultiResId );
      SetNumberText( text , BTLSTR_UI_GAUGE_HP_01 , hp , 3 , false );
    }
  }
}

// 経験値バーのアニメ
void BtlvGaugeBase::ExpAnime(u32 value)
{

  const pml::pokepara::PokemonParam* ppSrc = mParam->GetSrcData();
  const int lv = mParam->GetValue(BTL_POKEPARAM::BPP_LEVEL);
  pml::personal::LoadGrowTable( ppSrc->GetMonsNo() , ppSrc->GetFormNo() );

  const int nowLvExp = pml::personal::GetMinExp( lv );
  const int nextLvExp = pml::personal::GetMinExp( lv+1 );
  const int nowExp = mParam->GetValue(BTL_POKEPARAM::BPP_EXP);
  const int nowExpVal = nowExp - nowLvExp;
  const int expSub = nextLvExp - nowLvExp;

  mDispTrgExpRate = nowExpVal*100/expSub;
  if( nowExpVal != 0 && mDispTrgExpRate == 0 )
  {
    mDispTrgExpRate = 1;
  }

  mIsUpdateExp = true;

  //SE停止タイミングの兼ね合いでgauge_statusで制御
  mBtlvCore->PlaySe( SEQ_SE_BT_EXP );
  mExpAnimCnt = MIN_EXP_ANIME_CNT;
}

void BtlvGaugeBase::ExpAnime_Lvup(void)
{
  mDispTrgExpRate = 100;
  mIsUpdateExp = true;

  //SE停止タイミングの兼ね合いでgauge_statusで制御
  mBtlvCore->PlaySe( SEQ_SE_BT_EXP );
  mExpAnimCnt = MIN_EXP_ANIME_CNT;
}

void BtlvGaugeBase::UpdateExpGauge(void)
{

  gfl2::lyt::LytMultiResID& rMultiResId = mG2dUtil->GetLayoutResourceID( mLytIndex );
  
  gfl2::lyt::LytParts*  partsLyt  = mGaugeLyt->GetPartsPane( mGaugeParts , mSubPartsLyt[PARTS_EXP_GAUGE] , &rMultiResId );
  gfl2::lyt::LytPicture *gaugeLyt = mGaugeLyt->GetPicturePane( partsLyt, PANENAME_BATTLE_GAG_001_PANE_GAUGE_EXP, &rMultiResId );

  nw::lyt::Size size = gaugeLyt->GetSize();
  size.width = mDispExpRate*EXP_GAUGE_SIZE*0.01;
  gaugeLyt->SetSize(size);

}

bool BtlvGaugeBase::IsAnime(void)
{
  if( ( mIsUpdateExp ) 
      || ( mIsDamageAnime)
      || ( mIsUpdateExp)
     )
  {
    return true;
  }

  return false;
}

bool BtlvGaugeBase::IsPinch(void)
{
  const int maxhp = mParam->GetValue(BTL_POKEPARAM::BPP_MAX_HP);
  if( mDispTrgHp == 0 )
  {
    return false;
  }
  if( ::app::tool::Gauge::GetDottoColor( mDispTrgHp , maxhp ) == ::app::tool::Gauge::GAUGE_COLOR_RED )
  {
    return true;
  }
  return false;
}

void BtlvGaugeBase::InitGlove( CornerColor col )
{

  if( mBtlvCore->GetBattleRule() != BTL_RULE_ROYAL )
    {
      return;
    }

  s32 anime_id = ANM_GLOVE_GREEN;

  switch (col)
    {
    case CORRER_COLOR_GREEN:
      { anime_id = ANM_GLOVE_GREEN; }
      break;
    case CORRER_COLOR_YELLOW:
      { anime_id = ANM_GLOVE_YELLOW; }
      break;
    case CORRER_COLOR_RED:
      { anime_id = ANM_GLOVE_RED; }
      break;
    case CORRER_COLOR_BLUE: 
      { anime_id = ANM_GLOVE_BLUE; }
      break;
    }
  
  mG2dUtil->StartAnime( mLytIndex,mAnmTbl[anime_id] );
  mGaugeLyt->GetPane( mRoyalGloveLyt )->SetVisible(false);

}

void BtlvGaugeBase::DispGlove(void)
{
  mGaugeLyt->GetPane( mRoyalGloveLyt )->SetVisible(true);
}
void BtlvGaugeBase::HideGlove(void)
{
  mGaugeLyt->GetPane( mRoyalGloveLyt )->SetVisible(false);
}

GFL_NAMESPACE_END(btl)
