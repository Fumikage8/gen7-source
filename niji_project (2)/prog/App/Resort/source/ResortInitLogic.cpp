//=============================================================================================
/**
 * @file   ResortInitLogic.cpp
 * @brief  呼び出されたときに一回行う、再計算クラス
           何度呼び出されても同じ結果になるように、
           実時間一時間毎で変化するようにリゾート専用の乱数の種を用いる
       Initはいらなかったな・・・
 * @author k.ohno
 * @date   2015.9.03
 */
//=============================================================================================


#include "GameSys/include/NijiTime.h"

#include <pml/include/pmlib.h>

#include "System/include/HeapDefine.h"
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/ui/UIView.h>
#include <AppLib/include/ui/UIInputListener.h>
#include <pml/include/item/item.h>

#include "App/Resort/include/ResortProc.h"

#include "ResortInitLogic.h"
#include "System/include/GflUse.h"
#include "Savedata/include/MyStatus.h"
#include "Savedata/include/ResortSave.h"
#include "Savedata/include/BoxSave.h"
#include "Savedata/include/BoxPokemonSave.h"

#include <arc_def_index/arc_def.h>
#include <arc_index/islandbin.gaix>

#include "Savedata/include/EventWork.h"

#include "PokeTool/include/KawaigariParamCareCoreManager.h"
#include "PokeTool/include/NatsukiManager.h"
#include "PokeTool/include/PokeToolPokePlace.h"
#include "PokeTool/include/PokeToolForm.h"

#include "GameSys/include/GameData.h"

#include <Savedata/include/Record.h>

#include "ResortDebugInfo.h"



#define PARTS_NONE 0
#define PARTS_ITEM 1
#define REVO_ITEM 2
#define EXPENSIVE_ITEM 3




GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Resort )


ResortInitLogic::ResortInitLogic(void)  //コンストラクタ
  : mpLogicHeap(NULL),
    mpTempHeap(NULL)
{
  mAdventureItemTblMax = 0;
#if PM_DEBUG
  mDebugSpeed=1;
#endif
}


ResortInitLogic::~ResortInitLogic(void)  //デストラクタ
{
}


//------------------------------------------------------------------
/**
 * @brief   リゾート最初の処理
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
void ResortInitLogic::FirstFunc(void)
{

}






//------------------------------------------------------------------
/**
 * @brief   初期化を行う関数
 */
//------------------------------------------------------------------
void ResortInitLogic::InitFunc(Savedata::ResortSave* pResort,Field::EventWork* pEventSave, gfl2::heap::HeapBase* pHeap,gfl2::heap::HeapBase* pTempHeap,App::Resort::APP_PARAM* pAppParam,Savedata::BoxPokemon* pBoxPoke,
                               PokeTool::KawaigariParamCareCoreManager* /*使っていないのでコメントアウトpCare*/)
{
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_INIT_LOGIC_INIT_START);
#endif

  mpResortSave = pResort;
  mpLogicHeap = pHeap;
  mpTempHeap = pTempHeap;
  mpAppParam = pAppParam;
  mpEventSave = pEventSave;
  //使っていないのでコメントアウトmpCareCoreParamManager = pCare;
  mbWildPokemonUpdate=true;

#if PM_DEBUG
  if(mpAppParam->debugFullMode){
    mpAppParam->debugFullMode=false;
    OHNO_PRINT("ポケモン満タン\n");
    DebugFull();
  }
#endif
  
  if(!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ))
  {
    mpAppParam->pSave->SetBeansGetCount(20);  //最初かならずまめが地面に落ちるように をリゾート開始時に移動
  }
  
  {  //最初にかぞえる
    pml::pokepara::CoreParam* pPoke = GFL_NEW(mpTempHeap) pml::pokepara::CoreParam(mpTempHeap);
    mBoxPokeCount = pBoxPoke->GetPokemonCountAll( pPoke, false );
    mBoxPokeCountAll = mBoxPokeCount + mpResortSave->PokemonCountHotSpaAndGim(pPoke);
    GFL_DELETE( pPoke );
  }
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_00);
#endif

  //乱数初期化
  mRandom.Initialize(mpResortSave->GetRandomState());
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_01);
#endif

  mInterval=0;

  //抽選テーブル確保
  mpBoxLot = (u16*)GflHeapAllocMemory(mpLogicHeap, sizeof(u16) * Savedata::BoxPokemon::TRAY_MAX * Savedata::BoxPokemon::TRAY_POKE_MAX);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_02);
#endif

  // おんせんの初期時間の設定
  InitTimeHotspaGroup();
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_03);
#endif

  SecFunc( mpAppParam->firstOpenPos );//今までのマージ
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_04);
#endif

  {  //マージ後 ボーナスレベルを更新する 
    mPreviousBounusLevel = pResort->GetBonusLevel();   //古いボーナスレベルは台詞の変化に使用
    pResort->SetBonusLevel(  calcBonusLevel(mBoxPokeCountAll) );
  }
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_05);
#endif


  // ふれあい きのみ たんけんは BOXの配置が換わったら強制変更
  PokemonEventStartFunc(false);
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_06);
#endif
 
  if(
    (!mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_OPENING ))
    )
  {
    //mpResortSave->SetGroundBeans(0, GetDropBeansRandom());  //チュートリアル開始はひとつだけまめはいち
    mpResortSave->SetGroundBeans( 0, Savedata::ResortSave::POKEBEANS_NORMAL_01 );
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_07);
#endif
  }
  else {
    IvyTouchOnTheBeans( NULL );    // チュートリアル以外でリゾートに入った時にマメを配置する
#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
    ResortDebugInfo::s_ResortDebugInfo.SetCurrTimeOnce(ResortDebugInfo::TIME_PHASE_RESORT_STEP_E_08);
#endif
  }

#if NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE
  ResortDebugInfo::s_ResortDebugInfo.SetCurrTime(ResortDebugInfo::TIME_PHASE_RESORT_INIT_LOGIC_INIT_END);
#endif

  mbAdventureReturned = false;


}

bool ResortInitLogic::_pokemonComp(pml::pokepara::CoreParam* cp1,pml::pokepara::CoreParam* cp2)
{
  if(cp1->GetMonsNo()!=cp2->GetMonsNo()){
    OHNO_PRINT("MonsNoがちがう %d %d\n",cp1->GetMonsNo(),cp2->GetMonsNo());
    return false;
  }
  if(cp1->GetFormNo()!=cp2->GetFormNo()){
    OHNO_PRINT("formNoがちがう %d %d\n",cp1->GetFormNo(),cp2->GetFormNo());
    return false;
  }
  if(cp1->GetLevel()!=cp2->GetLevel()){
    OHNO_PRINT("levelがちがう %d %d\n",cp1->GetLevel(),cp2->GetLevel());
    return false;
  }
  if(cp1->GetSex()!=cp2->GetSex()){
    OHNO_PRINT("sexがちがう %d %d\n",cp1->GetSex(),cp2->GetSex());
    return false;
  }

  return true;

}



//-------------------------------------------------------------------------------------------
/**
 * @brief   ボックスのポケモンとふれあい、たんけん、きのみのポケモンに差異があるかを調べる
 */
//-------------------------------------------------------------------------------------------
bool ResortInitLogic::BoxDiffCheck(void)
{
  bool retcode = true;
  bool notpokemon = true;

  pml::pokepara::CoreParam* cp1 = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );
  pml::pokepara::CoreParam* cp2 = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );


  for(int i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
    mpAppParam->pSave->GetFriendShipPokemon(i, cp1 );  //
    if(cp1->IsNull()){
      continue;
    }
    notpokemon=false;
    u16 traypos = mpAppParam->pSave->GetFriendShipPokemonBoxPos(i);  //

    OHNO_PRINT("friend traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
    mpAppParam->pBoxPokemon->GetPokemon(cp2, traypos>>8, traypos&0xff);

    if( !_pokemonComp(cp1,cp2) ){
      retcode = false;
      break;
    }
  }
  if(retcode==true){
    for(int i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
      mpAppParam->pSave->GetFarmPokemon(i, cp1 );  //
      if(cp1->IsNull()){
        continue;
      }
      notpokemon=false;
      u16 traypos = mpAppParam->pSave->GetFarmPokemonBoxPos(i);
      
      OHNO_PRINT("farm traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
      mpAppParam->pBoxPokemon->GetPokemon(cp2, traypos>>8, traypos&0xff);

      if( !_pokemonComp(cp1,cp2) ){
        retcode = false;
        break;
      }
    }
  }
  if(retcode==true){
    for(int i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
      mpAppParam->pSave->GetAdventurePokemon(i, cp1 );  //
      if(cp1->IsNull()){
        continue;
      }
      notpokemon=false;
      u16 traypos = mpAppParam->pSave->GetAdventurePokemonBoxPos(i);  //

      OHNO_PRINT("adv traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
      mpAppParam->pBoxPokemon->GetPokemon(cp2, traypos>>8, traypos&0xff);

      if( !_pokemonComp(cp1,cp2) ){
        retcode= false;
        break;
      }
    }
  }
  GFL_DELETE(cp1);
  GFL_DELETE(cp2);

  if(notpokemon){ //ポケモン登録数が０のときはnot
    OHNO_PRINT("ぽけもんが０\n");
    return false;
  }
  return retcode;
}


#if PM_DEBUG

//-------------------------------------------------------------------------------------------
/**
 */
//-------------------------------------------------------------------------------------------
void ResortInitLogic::DebugBoxDiffCheck(void)
{
  bool retcode = true;
  bool notpokemon = true;
    u16 traypos;
  int i;
  pml::pokepara::CoreParam* cp1 = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );
  pml::pokepara::CoreParam* cp2 = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );


  for( i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
    mpAppParam->pSave->GetFriendShipPokemon(i, cp1 );  //
    if(cp1->IsNull()){
      continue;
    }
     traypos = mpAppParam->pSave->GetFriendShipPokemonBoxPos(i);  //
    OHNO_PRINT("friend traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
  }
  for( i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
    mpAppParam->pSave->GetFarmPokemon(i, cp1 );  //
    if(cp1->IsNull()){
      continue;
    }
     traypos = mpAppParam->pSave->GetFarmPokemonBoxPos(i);
    OHNO_PRINT("farm traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
  }
  for( i=0;i<Savedata::ResortSave::STOCKPOKEMON_MAX;i++){
    mpAppParam->pSave->GetAdventurePokemon(i, cp1 );  //
    if(cp1->IsNull()){
      continue;
    }
     traypos = mpAppParam->pSave->GetAdventurePokemonBoxPos(i);  //
    OHNO_PRINT("adv traypos %d %x m= %d\n",i,traypos, cp1->GetMonsNo());
  }
  GFL_DELETE(cp1);
  GFL_DELETE(cp2);

}


#endif //PM_DEBUG


//------------------------------------------------------------------
/**
 * @brief   抜ける時の処理
 */
//------------------------------------------------------------------
void ResortInitLogic::EndFunc(void)
{
  GflHeapFreeMemory(mpBoxLot);
  mpAppParam->pSave->SetRandomState( mRandom.SaveState() );
}


#if PM_DEBUG
void ResortInitLogic::DebugFull( void )
{
  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );
  int x;
  
  for(x=0;x<18;x++){
    mpAppParam->pBoxPokemon->GetPokemon(cp, 0, x);
    mpAppParam->pSave->SetHotSpaPokemon(x,cp,true);
    mpAppParam->pSave->SetGimPokemon(x,cp);
  }
  mpAppParam->pSave->SetPokemonEventTime(0,1,0);
  mpAppParam->pSave->SetBeansGetCount(255);


  for(x=0;x<18;x++){
    mpAppParam->pSave->SetFarmItemKind(x, ITEM_KURABONOMI+x);
  }


  GFL_DELETE(cp);

}
#endif






//------------------------------------------------------------------
/**
 * @brief   一秒毎に行う作業 -31535996
 * @param   resort_index  現在表示中のリゾート
 */
//------------------------------------------------------------------
void ResortInitLogic::SecFunc( int resort_index )
{

  if(mInterval==0){

#if defined(GF_PLATFORM_CTR)
  //  OHNO_PRINT("SecFuncST %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif

    
    //最終起動時間
    GameSys::DeviceDate date;

    u64 difftime = mpResortSave->GetDiffTimeDate(&date);
#if PM_DEBUG
    if(difftime!=0){
      difftime *= mDebugSpeed;
    }
#endif //PM_DEBUG

    difftime = difftime / 60;

    if(difftime > (360000/60)){
      difftime = (360000/60);   //タイマーは９９時間５９分ＭＡＸだからそれ以上まわすことはほぼ意味が無い @attention 寺地君と相談済み
    }
    
    if(difftime > 5){
      OHNO_PRINT("差分時間 %llu\n",difftime);
    }
    if(difftime > 0){
      mpResortSave->SetPlayTime(&date);  //常に更新していい 差分処理
    }
    for(u64 index=0; index < difftime ;index++){// １分一回更新へ変更します。160419 k.ohno
      FriendShipSecFunc();
      HotSpaSecFunc();
      GimSecFunc();
      FarmSecFunc();
      AdventureSecFunc( resort_index );
      BeansBottleSecFunc();
      PokemonEventSecFunc();
    }
    mInterval=_INTERVAL_FRAME;  //処理負荷軽減 GetDiffTimeDateをcallする回数を減らしている

#if defined(GF_PLATFORM_CTR)
   // OHNO_PRINT("SecFuncEnd %llu msec\n",nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds() );
#endif

  }
  else{
    mInterval--;
  }


}





bool ResortInitLogic::setPokemonData( int nextindex,pml::pokepara::CoreParam* cp,int i  )
{
  int count=0;
  int frilv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_FRIENDSHIP );
	int advlv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_ADVENTURE );
	int farlv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_FARM );
  int limit[]={0,6,12,18};
  int farmlimit[]={0,3,6,9};


  int fri_limit = limit[frilv];
  int adv_limit = limit[advlv];
  int far_limit = farmlimit[farlv];


  while(1){  //フレンド,たんけん,はたけ順番に配置
    int x = (nextindex+count) % 3;

    switch(x){
    case 0:
      if( mFri_index < fri_limit){
        mpAppParam->pSave->SetFriendShipPokemon(mFri_index, cp,mpBoxLot[i]>>8, mpBoxLot[i]&0xff);  //
        mFri_index++;
        return true;
      }
      break;
    case 1:
      if(mAdv_index < adv_limit){
        mpAppParam->pSave->SetAdventurePokemon(mAdv_index, cp,mpBoxLot[i]>>8, mpBoxLot[i]&0xff);   //
        mAdv_index++;
        return true;
      }
      break;
    case 2:
      if(mFar_index < far_limit){
        cp->SetResortEventStatus( pml::pokepara::RESORTEVENT_KINOMI_HELP );
        mpAppParam->pSave->SetFarmPokemon(mFar_index, cp,mpBoxLot[i]>>8, mpBoxLot[i]&0xff);        //
        mFar_index++;
        return true;
      }
      break;
    }
    count++;
    if(count==3){
      break;
    }
  }
  return false;
  
}


//------------------------------------------------------------------
/**
 * @brief   まめびんで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::BeansBottleSecFunc(void)
{
  if(mpResortSave->BeansBottleServiceTimeDownCount()){
    // リゾートの状態を確認し、出現率を決める
    u32 appearRate = 0;
    static const u32 APPEAR_RATE[Savedata::ResortSave::RESORT_KIND_MAX+1] =
    {
        0,
      100,
       75,//60,   //!< momiji対応0322：２つ以上開発されている場合は75%固定に
       75,//50,   //!< momiji対応0322：２つ以上開発されている場合は75%固定に
       75,//40,   //!< momiji対応0322：２つ以上開発されている場合は75%固定に
       75,//30,   //!< momiji対応0322：２つ以上開発されている場合は75%固定に
    };
    u32 resortNum = 0;
    for(int i=0;i<Savedata::ResortSave::RESORT_KIND_MAX;i++){
      int lv = mpResortSave->GetResortLevel( static_cast<Savedata::ResortSave::PokeResortKind>(i) );
      if(lv){
        ++resortNum;
      }
    }
    appearRate = APPEAR_RATE[resortNum];

    // リゾートごとに当たり/はずれを決める
    for(int i=0;i<Savedata::ResortSave::RESORT_KIND_MAX;i++){
      int lv = mpResortSave->GetResortLevel( static_cast<Savedata::ResortSave::PokeResortKind>(i) );

      if(lv){
        if(mRandom.Next( 100 ) <= appearRate){
        //  OHNO_PRINT("まめびんあたり %d\n",i);
          mpResortSave->SetBeansBottleGround( static_cast<Savedata::ResortSave::PokeResortKind>(i), true);
        }
      }
      
    }

    ///一日でフラグを落とす
    mpAppParam->pSave->SetUseBeansBottle(false);
  }
  
}


//------------------------------------------------------------------
/**
 * @brief   ポケモンイベントで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::PokemonEventSecFunc(void)
{

  if(!mpResortSave->PokemonEventTimeDownCount()){  // ポケモンイベント再抽選のためのタイマーだけまわし続ける
    return;
  }

}



//------------------------------------------------------------------
/**
 * @brief      ポケモンイベントで開始時に行う処理
 * @attention  この処理はpublicランドを使うこと  時間と無関係に呼ばれるようになったため
 */
//------------------------------------------------------------------
void ResortInitLogic::PokemonEventStartFunc(bool bforce)
{
  int i,j;
  mFri_index = 0;
  mAdv_index = 0;
  mFar_index = 0;
  int limit[]={6,12,18};
  u8 houru;
  u8 minu;
  u8 secu;
  bool check= false;
  
  if(mpAppParam->isStart || bforce){  //アプリ起動一回のみ  GFNMCat:[2370]
    mpResortSave->GetPokemonEventTime(&houru,&minu,&secu);
    if(houru==0 && minu==0 && secu==0){
      OHNO_PRINT("タイマー\n");
      check=true;
    }
  }
  
  if(!BoxDiffCheck()){
    OHNO_PRINT("boxに変更あり\n");
    check=true;
  }
  if(!check){  // ポケモンイベント再抽選しない
    return;
  }
  //ここから再抽選
  OHNO_PRINT("ここから再抽選\n");
  
  //ふれあい、たんけん、きのみのポケモンをＢＯＸから抽選

  pml::pokepara::CoreParam* cp = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam( mpTempHeap );

  cp->Clear();
  //---消す処理
  for(i = 0;i < Savedata::ResortSave::STOCKPOKEMON_MAX; i++){
    mpAppParam->pSave->SetFriendShipPokemon(i, cp,0,0);  //
    mpAppParam->pSave->SetAdventurePokemon(i, cp,0,0);   //
    mpAppParam->pSave->SetFarmPokemon(i, cp,0,0);        //
  }
  //---消す処理



  for(i=0;i<Savedata::BoxPokemon::TRAY_MAX;i++){  //抽選テーブルつくり
    for(j=0;j<Savedata::BoxPokemon::TRAY_POKE_MAX;j++){
      mpBoxLot[i * Savedata::BoxPokemon::TRAY_POKE_MAX + j] = (i<<8) + j;
    }
  }

  for(i=0;i< Savedata::BoxPokemon::TRAY_MAX * Savedata::BoxPokemon::TRAY_POKE_MAX;i++  ){
    int rand1= System::GflUse::GetPublicRand( Savedata::BoxPokemon::TRAY_MAX * Savedata::BoxPokemon::TRAY_POKE_MAX );
    int rand2= System::GflUse::GetPublicRand( Savedata::BoxPokemon::TRAY_MAX * Savedata::BoxPokemon::TRAY_POKE_MAX );
    u16 temp = mpBoxLot[rand1];
    mpBoxLot[rand1]=mpBoxLot[rand2];
    mpBoxLot[rand2]=temp;
  }


  int nextindex=0;
  
  for( i = 0;i < Savedata::BoxPokemon::TRAY_MAX * Savedata::BoxPokemon::TRAY_POKE_MAX ; i++){

    mpAppParam->pBoxPokemon->GetPokemon(cp, mpBoxLot[i]>>8, mpBoxLot[i]&0xff);
    if(cp->IsNull()){
      continue;
    }
    if(cp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)){
      continue;
    }
    if( mpAppParam->pBox->IsMeLock(mpBoxLot[i]>>8, mpBoxLot[i]&0xff)  ){//ロックされたポケモンは抽選しない
      continue;
    }

    {

      int eventno = GetTalkTypeRandom(0);  //ふれあい、たんけんきのみはテーブル０

      cp->SetResortEventStatus( eventno );
      if( setPokemonData( nextindex , cp , i ) ){
      //  OHNO_PRINT("BOX番号 %d %x  %d %d\n",i, mpBoxLot[i], eventno, cp->GetMonsNo());
        nextindex++;
      }
      else{
        break;
      }
    }
  }

  GFL_DELETE(cp);
  // 時間再度登録
  mpResortSave->SetPokemonEventTime(1,0,0);

#if PM_DEBUG
  DebugBoxDiffCheck();
#endif
}



//------------------------------------------------------------------
/**
 * @brief      ポケモンイベントで開始時にイベントを決める処理
 * @attention  この処理はpublicランドを使うこと  時間と無関係に呼ばれるようになったため
 */
//------------------------------------------------------------------
#pragma push 
#if defined(GF_PLATFORM_CTR)
#pragma Ospace   ///コンパイラ不具合による疑いがあった場所への抑制
#endif

int ResortInitLogic::GetTalkTypeRandom(int type)
{
  int max =0;
  int hotspalv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_HOTSPA );
  int gimlv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_GIM );
  int i;
  
  for( i=0;i<pml::pokepara::RESORT_EVENT_MAX;i++){
    if(type==0){
      max += mpTalkTypeBin[i].c2;
    }
    else{
      max += mpTalkTypeBin[i].c5;
    }
  }
  int eventrand = System::GflUse::GetPublicRand( max );

  max = 0;
  for( i=0;i<pml::pokepara::RESORT_EVENT_MAX;i++){
    if(type==0){
      max += mpTalkTypeBin[i].c2;
      if(eventrand < max){  //contact 588
        break;
      }
    }
    else{
      max += mpTalkTypeBin[i].c5;
      if(eventrand < max){  //contact 588
        break;
      }
    }
  }

  if(i==pml::pokepara::RESORTEVENT_GIM && gimlv==0){
    return 0;
  }
  if(i==pml::pokepara::RESORTEVENT_HOTSPA && hotspalv==0){
    return 0;
  }
  if(i<pml::pokepara::RESORT_EVENT_MAX){
    return i;
  }
  GFL_ASSERT(0);
  return 0;  //おもいで０でも特に問題ない
}
#pragma pop  //Ospace

void ResortInitLogic::WildPokemonArrive(void)
{
  gfl2::heap::HeapBase *heap = mpTempHeap;

  pml::pokepara::PokemonParam * pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );

  for(int index=0;index < Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX;index++){
    mpAppParam->pSave->GetWildPokemon(index, pp);

    if(pp->IsNull()){
      continue;
    }
		if(pml::pokepara::RESORTEVENT_WILD == pp->GetResortEventStatus()){
			pp->Clear();  //タッチして無い場合確実ににがす
			mpAppParam->pSave->SetWildPokemon(index, pp);
			continue;
		}
    if(pml::pokepara::RESORTEVENT_WILD_LIVE == pp->GetResortEventStatus()){ //確定の場合なにもしない
      continue;
    }

    int arrive =  mpAppParam->pSave->GetPlayPokemonKind(index);  //い付き率

    if(mBoxPokeCountAll<30){
    }
    else if(mBoxPokeCountAll<90){
      arrive+=5;
    }
    else if(mBoxPokeCountAll<150){
      arrive+=10;
    }
    else{
      arrive+=15;
    }

    /*
ボーナス　０：＋０％
ボーナス＋１：＋５％
ボーナス＋２：＋１０％
ボーナス＋３：＋１５％
     */
    if(arrive > mRandom.Next(100)){
      if(pml::pokepara::RESORTEVENT_WILD_LOVE == pp->GetResortEventStatus()){  //なでてる時は
        pp->SetResortEventStatus(pml::pokepara::RESORTEVENT_WILD_LIVE);  //獲得チャンス

        // kawa_resort_modify
        mpAppParam->pSave->SetWildPokemon(index, pp);  // セーブデータが保持しているポケパラに、イベントステータスが更新されたポケパラを反映する。

      }
    }
    else{
      pp->Clear();  //にがす
      mpAppParam->pSave->SetWildPokemon(index, pp);
    }
  }

  GFL_DELETE pp;


}






//------------------------------------------------------------------
/**
 * @brief   ふれあいリゾート 野生ポケで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_wildpokeSecFunc(void)
{
  int _CMAX=255;
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_FRIENDSHIP )-1;

  //
  if(mpResortSave->WildPokemonTimeDownCount()){


    u32 time = mpWildPokemonBin[lv].c1;   //次の抽選時間はテーブルから参照

    mpResortSave->SetWildPokemonTime(time / 3600,  (time / 60) % 60,time % 60);

    //野生ポケモンを残すかどうか判断する
    WildPokemonArrive();

    {

      // kawa_resort_modify
      gfl2::heap::HeapBase *heap = mpTempHeap;
      pml::pokepara::PokemonParam * pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap );

      for(int ind = 0; ind < Savedata::ResortSave::RESORT_FRIENDSHIP_PLAYPOKE_MAX; ind++){

        // kawa_resort_modify
        mpAppParam->pSave->GetWildPokemon(ind, pp);  // 野生ポケモンが残っているときは、新規ポケモンで上書きさせない。
        if(!pp->IsNull())
        {
          continue;
        }
        {
          int rand[]={30,40,50};  //ＬＶによる抽選
          if( rand[lv] < mRandom.Next(100)){
            continue;
          }
        }

        int tblindex = MakePokemon();

        MonsNo monsno = (MonsNo)mpStrn[tblindex].c1;
        int arrive = mpStrn[tblindex].c12;

      //  OHNO_PRINT("つくった野生ポケモン %d\n",monsno);

        mpAppParam->pSave->SetPlayPokemonKind(ind, arrive);

        {
          Field::EventWork * evwk = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
          int min_lv = 1;
          // ゲームクリア後
          if( evwk->CheckEventFlag(SYS_FLAG_GAME_CLEAR) )
          {
            min_lv = 49;
          }
          // 島４到着後
          else if( evwk->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN) )
          {
            min_lv = 37;
          }
          // 島３到着後
          else if( evwk->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN) )
          {
            min_lv = 21;
          }
          // 島２到着後
          else if( evwk->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN) )
          {
            min_lv = 11;
          }
          CreatePokemon(ind, monsno, min_lv+mRandom.Next(WILD_POKE_LV_RAND) );
        }
      }

      // kawa_resort_modify
      GFL_SAFE_DELETE(pp);

    }
    mbWildPokemonUpdate=true;

  }
}

//------------------------------------------------------------------
/**
 * @brief   ふれあいリゾートセーブで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_friendShipSecFunc(void)
{
	int _CMAX=255;
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_FRIENDSHIP )-1;

  
  if(mpResortSave->FriendShipTimeDownCount()){

    //一時間に一回 豆ぼっくすに数を入れる
    int bl=mpResortSave->GetBonusLevel();
    bl=bl+lv*4;
    GFL_ASSERT(bl<12);

    int band = mpPokeMameBin[bl].c4 - mpPokeMameBin[bl].c3;      //beans_env[lv][_MAX] - beans_env[lv][_MIN];
    int num = mRandom.Next(band+1) + mpPokeMameBin[bl].c3; //beans_env[lv][_MIN];

    GFL_ASSERT(num<11);

    int now = mpResortSave->GetBeansGetCount() + num;
    if(now>_CMAX){
      now =_CMAX;
    }
    mpResortSave->SetBeansGetCount(now);
  //  OHNO_PRINT("豆抽選の値 %d, %d\n",now, bl);

    // 時間再度登録
    mpResortSave->SetFriendShipTime(mpPokeMameBin[bl].c2 / 3600, (mpPokeMameBin[bl].c2 / 60) % 60, mpPokeMameBin[bl].c2 % 60);

  }
}


//------------------------------------------------------------------
/**
 * @brief   ふれあいリゾート  まめのき隠し仕様で一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_beansIvyHiddenSecFunc(void)
{
  if(mpResortSave->SetBeansIvyHiddenTimeDownCount()){
    //２４時間に一回リセット
    mpResortSave->ResetBeansIvyHiddenCount();
    // 時間再度登録
    mpResortSave->SetBeansIvyHiddenTime(24, 0, 0);
  }
}







//------------------------------------------------------------------
/**
 * @brief   ふれあいリゾートセーブで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::FriendShipSecFunc(void)
{

  if(mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU )){
    if(mpResortSave->WildPokemonServiceTimeDownCount()){
      _wildpokeSecFunc();
      _wildpokeSecFunc();
    }
    else{
      _wildpokeSecFunc();
    }
  }
  _friendShipSecFunc();
  _beansIvyHiddenSecFunc();

}



#pragma push 
#if defined(GF_PLATFORM_CTR)
#pragma Ospace   ///コンパイラ不具合による疑いがあった場所への抑制
#endif

int ResortInitLogic::GetDropBeansRandom(void)
{
  if( mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ) )
  {
    int max =0;

    for(int i=0;i<Savedata::ResortSave::POKEBEANS_KIND_MAX;i++){
      max += mpDropBeansBin[i].c1;
    }
    int eventrand = mRandom.Next( max );
    
    max = 0;
    for(int i=0;i<Savedata::ResortSave::POKEBEANS_KIND_MAX;i++){
      max += mpDropBeansBin[i].c1;
      if(eventrand <= max){
        return i;
      }
    }
    GFL_ASSERT(0);
    return mRandom.Next( Savedata::ResortSave::POKEBEANS_KIND_MAX );  //ストレートな抽選
  }
  else {
    // チュートリアル時はノーマルのポケマメだけを出す
    return System::GflUse::GetPublicRand( 7 ) + Savedata::ResortSave::POKEBEANS_NORMAL_01;
  }
}

#pragma pop //Ospace



//------------------------------------------------------------------
/**
 * @brief   木をタッチしたら豆をまく
 */
//------------------------------------------------------------------
void ResortInitLogic::_ivyTouchOnTheBeans(u8* buff)
{

  // ポケマメ入手カウント
  u8 stockCount = mpAppParam->pSave->GetBeansGetCount();
  // 落ちてるマメのカウント
  u8 putCount = 0;
  {
    u8 index = 0;

    for( int i=0; i<Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX; ++i )
    {
      if( mpAppParam->pSave->GetGroundBeans(i) != Savedata::ResortSave::POKEBEANS_NONE )
      {
      //  OHNO_PRINT("落ちてるたいぷ[%d] = %d\n",i,mpAppParam->pSave->GetGroundBeans(i));
        putCount++;
      }
    }
  }

  // 落ちるポケマメの数
  u8 dropCount = 0;
  {
    if( !mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ) )
    {
      dropCount = 7;    //!< チュートリアルでは落ちてくるマメの個数は固定
    }
    // マメカウントが２０以上の時、１０～２０のマメが落ちる
    else if( stockCount >= 20 )
    {
      dropCount = System::GflUse::GetPublicRand(11) + 10;
    }
    // マメカウントが２０未満の時、０～マメカウント数のマメが落ちる
    else if( stockCount > 0 )
    {
      dropCount = System::GflUse::GetPublicRand( stockCount + 1 );
    }
  }

  GFL_PRINT( "IvyTouchOnTheBeans : mameCount[%d] putCount[%d] dropCount[%d]", stockCount, putCount, dropCount );

  // 落としたポケマメ数
  u8 droppedCount = 0;
  u8 i = 0;

  while( i < dropCount )
  {
    // ２０個以上は置けない
    if( putCount == Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX )
    {
      break;
    }

    int putIndex = Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX;
    {
      while( putIndex == Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX )
      {
        int pos = System::GflUse::GetPublicRand(Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX);

        if( mpAppParam->pSave->GetGroundBeans(pos) == Savedata::ResortSave::POKEBEANS_NONE)
        {
          putIndex = pos;
        }
      }
    }

    //if( mpAppParam->pSave->GetGroundBeans(putIndex) == Savedata::ResortSave::POKEBEANS_NONE)
    {
      mpResortSave->SetGroundBeans(putIndex, GetDropBeansRandom());  //確率をテーブルから

      if( buff )
      {
        buff[putIndex] = 1;
      }

      putCount++;       // 置いたマメの数を増やす
      droppedCount++;   // 落としたポケマメの数を増やす

      ++i;
    }
  }

  GFL_PRINT( " droppedCount[%d]\n", droppedCount );

  if( mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_TOUCH_TREE ) )
  {
    // 置いた分だけポケマメ入手カウントを減らす
    mpAppParam->pSave->SetBeansGetCount( stockCount - droppedCount );
  }
}



//------------------------------------------------------------------
/**
 * @brief   木をタッチしたらかくし豆まき
 */
//------------------------------------------------------------------
void ResortInitLogic::_ivyTouchOnTheHiddenBeans(u8* buff)
{
  int beans;
  // 落ちてるマメのカウント
  u8 putCount = 0;

  if( !mpAppParam->pSave->GetResortProgress( Savedata::ResortSave::RESORT_TUTORIAL_STATE_MENU ) )
  {
    return;
  }

  
  mpAppParam->pSave->AddBeansIvyHiddenCount(250);//叩いた回数を増やす
  
  if(buff){
    for( int xn = 0 ; xn < Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX; xn++){
      buff[xn]=0;  //バッファの初期化
    }
  }
  {
    u8 index = 0;

    for( int i=0; i<Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX; ++i )
    {
      if( mpAppParam->pSave->GetGroundBeans(i) != Savedata::ResortSave::POKEBEANS_NONE )
      {
        putCount++;  //すでに落ちているまめを数える
      }
    }
  }
  if( putCount == Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX )
  {
    return;
  }


  // 落ちるポケマメの数
  {
    if( mpAppParam->pSave->GetBeansIvyHiddenCount()==15 )
    {
      beans = Savedata::ResortSave::POKEBEANS_NORMAL_01;
    }
    else if( mpAppParam->pSave->GetBeansIvyHiddenCount()==50 )
    {
      beans = Savedata::ResortSave::POKEBEANS_HEART_01;
    }
    else if( mpAppParam->pSave->GetBeansIvyHiddenCount()==100 )
    {
      beans = Savedata::ResortSave::POKEBEANS_NIJI;
    }
    else{
      return;
    }
  }
  // 落としたポケマメ数
  u8 droppedCount = 0;
  u8 i = 0;
	int putIndex=0;

  for( int pos=0; pos<Savedata::ResortSave::FRIENDSHIP_GROUND_BEANS_MAX;pos++ )
  {
    if( mpAppParam->pSave->GetGroundBeans(pos) == Savedata::ResortSave::POKEBEANS_NONE)
    {
      putIndex = pos;
      break;
    }
  }

  {
    mpResortSave->SetGroundBeans(putIndex, beans);  //確率をテーブルから

    if( buff )
    {
      buff[putIndex] = 1;
    }
  }

}


//------------------------------------------------------------------
/**
 * @brief   木をタッチしたら豆をまく
 */
//------------------------------------------------------------------
void ResortInitLogic::IvyTouchOnTheBeans(u8* buff)
{
  _ivyTouchOnTheHiddenBeans(buff);
  _ivyTouchOnTheBeans(buff);
}






//------------------------------------------------------------------
/**
 * @brief   探検リゾートで一秒毎に行う作業
 * @param   resort_index  現在表示中のリゾート
 */
//------------------------------------------------------------------
void ResortInitLogic::_adventureSecFunc( int resort_index )
{
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_ADVENTURE )-1;


  
  if(!mpResortSave->AdventureTimeDownCount()){  //探検終了してないばあい
    return;
  }
  int i,loop,j;
  int sum=0,lott=0,itemc=0;
  int cource = mpResortSave->GetAdventureCoursePattern();  //遊んでいたコースをえる
  u16 xitembuf[ADVENTURE_MAX_LOTT],xitemst=0;  //マックス抽選枠

  for(j=0;j<ADVENTURE_MAX_LOTT;j++){
    xitembuf[j] = ITEM_DUMMY_DATA;
  }

  
 // OHNO_PRINT("コースの番号 %d\n",cource);

  for( i = 0; i<mAdventureItemTblMax; i++ ){
    if(mpAdventureItemTblBin[i].c0==PARTS_NONE){
      break;  //テーブル検索終了
    }
    if(mpAdventureItemTblBin[i].c1==cource){  //コースが一致

      sum += mpAdventureItemTblBin[i].c3;  //確率を足しこむ
     // OHNO_PRINT("コースの一致 %d\n",sum);
      if(xitemst==0){
        xitemst = mpAdventureItemTblBin[i].c2;
      }
    }
  }
  loop = mpAdventureBin[lv].c2; //adventure_env[lv][_LOTTERY];
  lott = sum;

	GFL_ASSERT(loop<ADVENTURE_MAX_LOTT);
	if(loop>=ADVENTURE_MAX_LOTT){
		loop=ADVENTURE_MAX_LOTT-1;
	}
  if(lott>0){
    //OHNO_PRINT("抽選 %d\n",loop);
    for(j=0;j<loop;j++){
      itemc = mRandom.Next(lott);//アイテムセレクト

      // OHNO_PRINT("かくりつ %d \n",itemc);
      sum=0;
      //   xitembuf[j]=xitemst;
      for( i = 0; i<mAdventureItemTblMax; i++ ){
        if(mpAdventureItemTblBin[i].c0==PARTS_NONE ){
          break;  //テーブル検索終了
        }
        if(mpAdventureItemTblBin[i].c1==cource){  //コースが一致
          sum += mpAdventureItemTblBin[i].c3;  //確率をみる
          xitembuf[j] = mpAdventureItemTblBin[i].c2;
          if(itemc<sum){
            //  OHNO_PRINT("探検アイテム %d \n", xitembuf[j]);
            break;
          }
        }
      }
    }
  }
  //アイテム確定
  int max = mpAdventureBin[lv].c3; //adventure_env[lv][_LOTTERY_MAX];
  for(i=0;i < ADVENTURE_MAX_LOTT ; i++ ){
    if(xitembuf[i]==ITEM_DUMMY_DATA){
      break;
    }
//    OHNO_PRINT("探検でアイテム取得 %d \n",xitembuf[i]);
    mpResortSave->AddAdventureItem( xitembuf[i], max);
  }
  
  // コースを未選択状態に戻す
  mpResortSave->SetAdventureCoursePattern( Savedata::ResortSave::RESORT_ADV_COURSE_NONE );

  if( resort_index == Savedata::ResortSave::RESORT_KIND_ADVENTURE )
  {
    mpResortSave->SetUpdateFlg();
  }
  mbAdventureReturned = true;

}

//------------------------------------------------------------------
/**
 * @brief  探検リゾート一秒毎に行う作業
 * @param   resort_index  現在表示中のリゾート
 */
//------------------------------------------------------------------
void ResortInitLogic::AdventureSecFunc( int resort_index )
{
  if(mpResortSave->AdventureBonusTimeDownCount()){
    _adventureSecFunc( resort_index );
    _adventureSecFunc( resort_index );
  }
  else{
    _adventureSecFunc( resort_index );
  }

}






//------------------------------------------------------------------
/**
 * @brief   きのみリゾートで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_farmSecFunc(void)
{
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_FARM )-1;


  for(int index =0;index < Savedata::ResortSave::RESORT_FARM_MAX;index++){

    if(mpResortSave->FarmTimeDownCount( index )){

      int kindindex = item::ITEM_GetNutsNo( mpResortSave->GetFarmItemKind( index ));
      {

        int num = 0;
        switch(lv){
        case 0:
          num = mpKinomiBin[kindindex].c2;
          break;
        case 1:
          num = mpKinomiBin[kindindex].c3;
          break;
        case 2:
          num = mpKinomiBin[kindindex].c4;
          break;
        }
        mpResortSave->SetFarmNum(index,num);
        // 時間再度登録
       // OHNO_PRINT("とれた木の実個数 %d\n", num);
      }
    }
  }

}

//------------------------------------------------------------------
/**
 * @brief   木の実畑で一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::FarmSecFunc(void)
{
  if(mpResortSave->FarmFertilizerTimeDownCount()){
    _farmSecFunc();
    _farmSecFunc();
  }
  else{
    _farmSecFunc();
  }

}







//------------------------------------------------------------------
/**
 * @brief   温泉リゾートセーブで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_hotSpaSecFunc(void)
{
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_HOTSPA )-1;
  int buff[3]={0,0,0};
  bool checkflg=false;

  enum
  {
    BIT_NATSUKI_UP = 1,
    BIT_NOBOSE = 2,
  };

  for( int x=0; x<Savedata::ResortSave::RESORT_HOTSPA_KIND_MAX; x++ )
  {
    if( mpResortSave->HotSpaPokemonTimeDownCount( x ) )
    {
      buff[x] |= BIT_NATSUKI_UP;
    //  OHNO_PRINT("_hotSpaSecFunc HotSpaPokemonTimeDownCount 0 %d\n",x);
      // 時間を再設定
      mpResortSave->SetHotSpaPokemonTime(x,mpFamiliarityBin[lv].c1/3600, (mpFamiliarityBin[lv].c1/60)%60,mpFamiliarityBin[lv].c1%60);
      checkflg=true;
    }
    if( mpResortSave->HotSpaNoboseTimeDownCount( x ) )
    {
      buff[x] |= BIT_NOBOSE;
    }
  }
  if(!checkflg){
    return;  //処理負荷軽減
  }
  
	{

	pml::pokepara::CoreParam* pPoke = NULL;
	pPoke = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam(mpTempHeap);
	bool fastpass = pPoke->StartFastMode();

  for( int index=0; index<Savedata::ResortSave::STOCKPOKEMON_MAX; index++ )
  {
    bool is_update = false;

    if( !( buff[index/6] & BIT_NATSUKI_UP) ) {
      continue;  //なつきタイマーが終端＝０でないときは見る必要がない
    }
    

    if( ( buff[index/6] & BIT_NATSUKI_UP ) || mpResortSave->IsNewHotSpaPokemon(index) )
    {
			
      mpResortSave->GetHotSpaPokemon( index, pPoke );

      if( !pPoke->IsNull() )
      {
//        OHNO_PRINT("更新、もしくは時間\n");
//        OHNO_PRINT( "Group[%d] Poke[%d]：なつき度[旧]=[%d]\n",
   //       index / 6, index % 6, pPoke->GetOriginalFamiliarity() );
        // タマゴ
        if( pPoke->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG )){    // ダメタマゴはなにもしない GFNMCat[2123]
        }
        else if( pPoke->IsEgg(pml::pokepara::CHECK_ONLY_LEGAL_EGG) )
        {
          int num = mpEggWalkBin[lv].c2;  //walk_env[lv][_SUB];
          u32 oldVal = pPoke->GetOriginalFamiliarity();
          pPoke->SubOriginalFamiliarity( num );
          u32 newVal = pPoke->GetOriginalFamiliarity();
          // 時間再度登録
    //      OHNO_PRINT("歩数ダウン %d %d\n",index, num);
          if(newVal==0){
            pPoke->SetResortEventStatus( pml::pokepara::RESORTEVENT_HOTSPA_EGG_HATCHING );  // 生まれる時に ! にする
          }
          // レコード
          if( oldVal > newVal )
          {
            Savedata::IncRecord( Savedata::Record::RECID_POKERESORT_TAMAGOFUKA );   // レコード：孵化歩数減算回数


            
          }
          //mpResortSave->SetHotSpaPokemonTime(index,mpEggWalkBin[lv].c1/3600, (mpEggWalkBin[lv].c1/60)%60,mpEggWalkBin[lv].c1%60);
        }
        // ポケモン
        else
        {
          int num = mpFamiliarityBin[lv].c2;  //hotspa_env[lv][_ADD];
          // のぼせ
          if( pPoke->GetResortEventStatus() == pml::pokepara::RESORTEVENT_HOTSPA_DIZZY )
          {
            num *= -1;
          }
          u32 oldVal = pPoke->GetFamiliarity();
          PokeTool::NatsukiManager::CalcWhenResortHotSpa( pPoke, PokeTool::PokePlace::PLACE_NO_SPECIAL_POKERESORT, num );
          u32 newVal = pPoke->GetFamiliarity();
          // 時間再度登録
  //        OHNO_PRINT("なつきどアップ %d %d\n",index, num);
          // レコード
          if( oldVal < newVal )
          {
            Savedata::IncRecord( Savedata::Record::RECID_POKERESORT_NATSUKI );   // レコード：なつき度増加回数
          }
          //mpResortSave->SetHotSpaPokemonTime(index,mpFamiliarityBin[lv].c1/3600, (mpFamiliarityBin[lv].c1/60)%60,mpFamiliarityBin[lv].c1%60);
        }
  //      OHNO_PRINT( "Group[%d] Poke[%d]：なつき度[新]=[%d]\n",
        //  index / 6, index % 6, pPoke->GetOriginalFamiliarity() );
        is_update = true;
      }
    }

    // のぼせチェック
    if( ( buff[index/6] & BIT_NOBOSE ) )
    {
      if( pPoke == NULL )
      {
        pPoke = GFL_NEW_LOW(mpTempHeap) pml::pokepara::CoreParam(mpTempHeap);
        mpResortSave->GetHotSpaPokemon( index, pPoke );
      }
      if( pPoke->GetResortEventStatus() == pml::pokepara::RESORTEVENT_HOTSPA_STATE )
      {
        pPoke->SetResortEventStatus( pml::pokepara::RESORTEVENT_HOTSPA_DIZZY );
        is_update = true;
      }
    }

    if( is_update != false )
    {
      if( pPoke != NULL )
      {
        mpResortSave->SetHotSpaPokemon(index,pPoke,false);  //変更された値を反映
        mpResortSave->ResetNewHotSpaPokemon(index);   //設定変更フラグはリセットしてＯＫ
      }
      else
      {
        GFL_ASSERT( pPoke );
      }
    }
  }
	pPoke->EndFastMode( fastpass );
	GFL_SAFE_DELETE( pPoke );
	}
}

//------------------------------------------------------------------
/**
 * @brief   温泉リゾートセーブで一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::HotSpaSecFunc(void)
{
  if(mpResortSave->HotSpaBonusTimeDownCount()){
    _hotSpaSecFunc();
    _hotSpaSecFunc();
  }
  else{
    _hotSpaSecFunc();
  }

}







//------------------------------------------------------------------
/**
 * @brief   うんどう一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::_gimSecFunc(void)
{
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_GIM )-1;
  bool bsetTime=false;


  
  for(int index =0;index < Savedata::ResortSave::RESORT_GIM_KIND_MAX;index++){//３種類

    Savedata::ResortSave::PokeResortGimKind cource = mpResortSave->GetGimKind(index);

    if(mpResortSave->GimEndTimeDownCount(index)){

      pml::pokepara::PokemonParam* pPoke = GFL_NEW(mpTempHeap) pml::pokepara::PokemonParam(mpTempHeap);

      GFL_ASSERT(pPoke);

      u8 lest = mpResortSave->GetGimTrainingCount(index);//トレーニング回数の減算
      if(lest != 0){
        lest = lest - 1;
      }
      if(lest != 0){
        bsetTime = true;
      }
      else{
        bsetTime=false;//GFNMCat:[2272]
      }
//      OHNO_PRINT("運動残り回数 %d %d\n",lest,bsetTime);
      mpResortSave->SetGimTrainingCount(index, lest);//GFNMCat:[2272]

      if(lest==0){
        mpResortSave->SetGimState( index, Savedata::ResortSave::RESORT_GIM_STATE_END );
      }

      for(int num = 0;num < Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX; num++){

        mpResortSave->GetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);

        if(!pPoke->IsNull() && !pPoke->IsEgg( pml::pokepara::CHECK_ONLY_ILLEGAL_EGG ) )
        {    // ダメタマゴはなにもしない GFNMCat[2123]

          int addnum = mpGimPointupBin[lv].c2;  //gim_pointup_env[lv][_ADD];
          int timer = mpGimPointupBin[lv].c1;
          int nl = mpGimLvBin[lv].c2;  //gim_lv_env[lv][_ADD];

          u32 oldKiso = 0;
          u32 newKiso = 0;

          switch(cource)
          {
          case Savedata::ResortSave::RESORT_GIM_KIND_LV:
            {
              u8 b_lv = pPoke->GetLevel();
              pPoke->AddExp( nl );
              u8 a_lv = pPoke->GetLevel();
              // レベルアップ時の技をセット
              if( b_lv != a_lv )
              {
                // レコード
                Savedata::IncRecord( Savedata::Record::RECID_POKERESORT_LVUP );  //!< LVアップの累計回数

                // 技習得データをロード
                pml::personal::LoadWazaOboeData( pPoke->GetMonsNo(), pPoke->GetFormNo() );
                for( int i=0; i<pml::personal::GetWazaOboeNum(); i++ )
                {
                  u16 waza_lv = pml::personal::GetWazaOboeLevel( i );   // 技を習得するレベル
                  if( waza_lv > a_lv )
                  {
                    break;
                  }
                  if( waza_lv > b_lv )
                  {
                    pPoke->PushWaza( pml::personal::GetWazaOboeWazaNo(i) );
                  }
                }
              }
            }
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              timer = mpGimLvBin[lv].c1;
              mpResortSave->SetGimEndTime(index, timer/3600, (timer / 60) % 60,timer % 60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_HP:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_HP);
            pPoke->AddEffortPower(pml::pokepara::POWER_HP, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_HP);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_AT:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_ATK);
            pPoke->AddEffortPower(pml::pokepara::POWER_ATK, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_ATK);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_DF:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_DEF);
            pPoke->AddEffortPower(pml::pokepara::POWER_DEF, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_DEF);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_SA:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_SPATK);
            pPoke->AddEffortPower(pml::pokepara::POWER_SPATK, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_SPATK);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_SD:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_SPDEF);
            pPoke->AddEffortPower(pml::pokepara::POWER_SPDEF, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_SPDEF);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          case Savedata::ResortSave::RESORT_GIM_KIND_SP:
            oldKiso = pPoke->GetEffortPower(pml::pokepara::POWER_AGI);
            pPoke->AddEffortPower(pml::pokepara::POWER_AGI, addnum);
            newKiso = pPoke->GetEffortPower(pml::pokepara::POWER_AGI);
            mpResortSave->SetGimPokemon(index*Savedata::ResortSave::RESORT_GIM_KIND_NUM_MAX + num,pPoke);
            if(bsetTime){
              mpResortSave->SetGimEndTime(index,timer/3600, (timer/60)%60,timer%60);
            }
            break;
          }

          // レコード
          if( oldKiso < newKiso )
          {
            Savedata::IncRecord( Savedata::Record::RECID_POKERESORT_KISO ); // レコード：基礎ポイントアップ回数
          }

          // 時間再度登録
//          OHNO_PRINT("努力値アップ %d %d %d\n",index, mpResortSave->GetGimState(index), num);
        }
      }

      GFL_DELETE pPoke;
    }
  }

}




//------------------------------------------------------------------
/**
 * @brief   運動リゾートのドリンクによる残り時間（テーブルの値）の取得
 */
//------------------------------------------------------------------
int ResortInitLogic::_getGimLestTimeTable(int index)
{
  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_GIM )-1;
  bool bsetTime=false;
  Savedata::ResortSave::PokeResortGimKind cource = mpResortSave->GetGimKind(index);

  int addnum = mpGimPointupBin[lv].c2;  //gim_pointup_env[lv][_ADD];
  int timer = mpGimPointupBin[lv].c1;
  int nl = mpGimLvBin[lv].c2;  //gim_lv_env[lv][_ADD];
  int lvtimer = mpGimLvBin[lv].c1;

  switch(cource)
  {
  case Savedata::ResortSave::RESORT_GIM_KIND_LV:
    return lvtimer;
  default:
		break;
  }
  return timer;
}


//--------------------------------------------------------------
/**
 * @brief   うんどうスタート
 */
//--------------------------------------------------------------
void ResortInitLogic::StartGimTimer(int kind)
{
  int timer = _getGimLestTimeTable(kind);

  mpResortSave->SetGimEndTime(kind,timer/3600, (timer/60)%60,timer%60);
}





//--------------------------------------------------------------
/**
 * @brief    うんどう装置のトータルのこり時間
 * @param    時間、分、秒
 */
//--------------------------------------------------------------
void ResortInitLogic::GetGimTotalEndTime(int kind, u8* hour, u8* min, u8* sec)
{
  int timer = _getGimLestTimeTable(kind);

  int lest = static_cast<int>( mpResortSave->GetGimTrainingCount(kind) );//トレーニング回数をえる

  if(lest>0){
    timer = timer * (lest-1);
  }
  else
  {
    timer=0;
  }

  mpResortSave->GetGimEndTime(kind, hour, min, sec);

  {
    int math_hour = *hour;
    int math_min = *min;
    math_hour *= 3600;
    math_min *= 60;
    math_hour = math_hour + math_min + *sec + timer;
    *hour = static_cast<int>( math_hour/3600 );
    *min = static_cast<int>( (math_hour/60)%60 );
    *sec = static_cast<int>( math_hour%60 );
  }

}




//------------------------------------------------------------------
/**
 * @brief   うんどう一秒毎に行う作業
 */
//------------------------------------------------------------------
void ResortInitLogic::GimSecFunc(void)
{

  //ボーナス時は２倍
  if( mpResortSave->GimBonusTimeDownCount() ){
    _gimSecFunc();
    _gimSecFunc();
  }
  else{
    _gimSecFunc();
  }
}



//リゾート用野生ポケモン生成
//通常と変わらず 1/4096の確率でレアになる

void ResortInitLogic::CreatePokemon(int index, int monsno, int lv)
{
  gfl2::heap::HeapBase *heap = mpTempHeap;

  pml::pokepara::PokemonParam * pp = GFL_NEW( heap ) pml::pokepara::PokemonParam( heap, (MonsNo)monsno, lv, pml::pokepara::INIT_SPEC_ID_RANDOM );

  pp->SetGetBall( item::BALLID_MONSUTAABOORU );
  pp->SetResortEventStatus( pml::pokepara::RESORTEVENT_WILD );  //やせい用
  pp->SetCassetteVersion( System::GetVersion() );
  pp->SetLangId( System::GetLang() );

  pp->SetID( mpAppParam->pMyStatus->GetID() );
  mpAppParam->pSave->SetWildPokemon(index, pp);

  GFL_DELETE pp;
}


void ResortInitLogic::SetEggWalkData(void* pData)
{
  mpEggWalkBin = (walk_island_binstr*)pData;
}

void ResortInitLogic::SetFamiliarityData(void* pData)
{
  mpFamiliarityBin = (hotspa_island_binstr*)pData;
}


void ResortInitLogic::SetWildPokemonData(void* pData)
{
  mpWildPokemonBin = (wildpokemon_island_binstr*)pData;
}


void ResortInitLogic::SetGimPointupData(void* pData)
{
  mpGimPointupBin = (gim_pointup_island_binstr*)pData;
}

void ResortInitLogic::SetGimLvData(void* pData)
{
  mpGimLvBin = (gim_lv_island_binstr*)pData;
}

void ResortInitLogic::SetPokeMameData(void* pData)
{
  mpPokeMameBin = (pokemame_island_binstr*)pData;
}

void ResortInitLogic::SetBeansData(void* pData)
{
  mpBeansBin = (beans_island_binstr*)pData;
}

void ResortInitLogic::SetAdventureData(void* pData)
{
  mpAdventureBin = (adventure_island_binstr*)pData;
}

void ResortInitLogic::SetAdventureItemTblData(void* pData)
{
  mpAdventureItemTblBin = (adventure_itemtbl_island_binstr*)pData;
}

void ResortInitLogic::SetAdventureItemTblData( void* pData, u32 dataSize )
{
  mpAdventureItemTblBin = (adventure_itemtbl_island_binstr*)pData;
  // @fix MMCat[370]：リストの最大数を計算
  mAdventureItemTblMax = dataSize / sizeof( adventure_itemtbl_island_binstr );
}


void ResortInitLogic::SetKinomiData(void* pData)
{
  mpKinomiBin = (kinomi_island_binstr*)pData;
}


void ResortInitLogic::SetTalkTypeData(void* pData)
{
  mpTalkTypeBin = (talktype_island_binstr*)pData;
}

void ResortInitLogic::SetDropBeansData(void* pData)
{
  mpDropBeansBin = (dropbeans_island_binstr*)pData;
}




void ResortInitLogic::SetPokemonCreateData(void* pData)
{
  mpStrn = static_cast<App::Resort::wildcreate_island_binstr*>(pData);
}


int ResortInitLogic::GetPokemonApperData(int index)
{

  if(mpEventSave->CheckEventFlag(SYS_FLAG_GAME_CLEAR))
  {
    if( System::GetVersion() == VERSION_SUN2 ){  //クリア後
      return mpStrn[index].c10;
    }
    else{
      return mpStrn[index].c11;
    }
  }
  else if(mpEventSave->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND4_OPEN))
  {
    if( System::GetVersion() == VERSION_SUN2 ){  //４の島
      return mpStrn[index].c8;
    }
    else{
      return mpStrn[index].c9;
    }
  }
  else if(mpEventSave->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND3_OPEN))
  {
    if( System::GetVersion() == VERSION_SUN2 ){  //３の島
      return mpStrn[index].c6;
    }
    else{
      return mpStrn[index].c7;
    }
  }
  else if(mpEventSave->CheckEventFlag(SYS_FLAG_ZUKAN_ISLAND2_OPEN))
  {
    if( System::GetVersion() == VERSION_SUN2 ){  //２の島
      return mpStrn[index].c4;
    }
    else{
      return mpStrn[index].c5;
    }
  }
  if( System::GetVersion() == VERSION_SUN2 ){  //１の島
    return mpStrn[index].c2;
  }
  return mpStrn[index].c3;

}


int ResortInitLogic::MakePokemon(void)
{
  int perc = 0;
  int i;
  int ans;
  int poke=0;

  for( i = 0 ; i < 100 ; i++){
    if(mpStrn[i].c1 != 0){
      perc += GetPokemonApperData(i);
    }
    else{
      break;
    }
  }
  if(perc){
    ans = mRandom.Next( perc );
    perc=0;
    for( i = 0 ; i < 100 ; i++){
      if(mpStrn[i].c1 != 0){
        perc += GetPokemonApperData(i);
        if(perc > ans){
          //        mpAppParam->pSave->SetPlayPokemonKind();
          return i;
        }
      }
      else{
        break;
      }
    }
  }
  return 0;
}



//------------------------------------------------------------------
/**
 * @brief   きのみリゾートの植える処理
 */
//------------------------------------------------------------------
void ResortInitLogic::PlantKinomi(int index,int kinomino)
{
  u8 x = item::ITEM_GetNutsNo(kinomino);
  //int x = kinomino-ITEM_KURABONOMI;

  int time = mpKinomiBin[x].c1;

  mpResortSave->SetFarmItemKind(index, kinomino);
  //時間をセット
  mpResortSave->SetFarmTime(index, time/3600, (time/60)%60, time%60);

}



void ResortInitLogic::SetBonusTimer(int normal,int rare, int niji)
{
  int timer = _NORMAL_TIMER * normal + _RARE_TIMER * rare + _NIJI_TIMER * niji;

  switch((Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos){
  default:
  case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
    mpAppParam->pSave->SetWildPokemonServiceTime(timer/60 ,timer%60,0);
    break;
  case Savedata::ResortSave::RESORT_KIND_FARM:
    mpAppParam->pSave->SetFarmFertilizerTime(timer/60 ,timer%60,0);
    break;
  case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
    mpAppParam->pSave->SetAdventureBonusTime(timer/60 ,timer%60,0);
    break;
  case Savedata::ResortSave::RESORT_KIND_GIM:
    mpAppParam->pSave->SetGimBonusTime(timer/60 ,timer%60,0);
    break;
  case Savedata::ResortSave::RESORT_KIND_HOTSPA:
    mpAppParam->pSave->SetHotSpaBonusTime(timer/60 ,timer%60,0);
    break;
  }
}



bool ResortInitLogic::GetBonusTimerMove(void)
{

  switch((Savedata::ResortSave::PokeResortKind)mpAppParam->firstOpenPos){
  default:
  case Savedata::ResortSave::RESORT_KIND_FRIENDSHIP:
    return mpResortSave->IsWildPokemonServiceTime();
  case Savedata::ResortSave::RESORT_KIND_FARM:
    return mpResortSave->IsFarmFertilizerTime();
  case Savedata::ResortSave::RESORT_KIND_ADVENTURE:
    return mpResortSave->IsAdventureBonusTime();
  case Savedata::ResortSave::RESORT_KIND_GIM:
    return mpResortSave->IsGimBonusTime();
  case Savedata::ResortSave::RESORT_KIND_HOTSPA:
    return mpResortSave->IsHotSpaBonusTime();
  }
}




u8 ResortInitLogic::calcBonusLevel(int boxnum)
{
/*
  enum{  //ボーナス段階
    RESORT_BONUS0,   //  ０～２９					ボーナス０				
    RESORT_BONUS1, //３０～８９					ボーナス１				
    RESORT_BONUS2, //９０～１４９					ボーナス２				
    RESORT_BONUS3, ///１５０以上					ボーナス３				
  };
*/
  if(boxnum < 30){
    return Savedata::ResortSave::RESORT_BONUS0;
  }
  else if(boxnum < 90){
    return Savedata::ResortSave::RESORT_BONUS1;
  }
  else if(boxnum < 150){
    return Savedata::ResortSave::RESORT_BONUS2;
  }
  return Savedata::ResortSave::RESORT_BONUS3;


}



//------------------------------------------------------------------
/**
 * @brief  ポケモンの数によるボーナス段階を得る
 */
//------------------------------------------------------------------
u8 ResortInitLogic::GetBonusLevel(void)
{
  return mpResortSave->GetBonusLevel();
}
//------------------------------------------------------------------
/**
 * @brief  前回のボーナス段階を得る
 */
//------------------------------------------------------------------
u8 ResortInitLogic::GetPreviousBonusLevel(void)
{
  return mPreviousBounusLevel;
}


//


//------------------------------------------------------------------
/**
 * @brief   まめびんながす処理
 */
//------------------------------------------------------------------
void ResortInitLogic::SendBeansBottle(void)
{
  //７こへらす
  mpResortSave->SubPokeBeansNum( 7, 0, 0 );
  mpResortSave->SetUseBeansBottle( true );
  //時間をセット
  mpResortSave->SetBeansBottleServiceTime( 24, 0, 0);
 // OHNO_PRINT("まめびんながした\n");

}



//------------------------------------------------------------------
/**
 * @brief   おんせんの時間の初期化
 */
//------------------------------------------------------------------
void ResortInitLogic::InitTimeHotspaGroup( void )
{
  if( mpAppParam->seqReq != NEXTSEQ_BOX_HOTSPA ) return;

  bool updateTime = false;

  for( u8 i=0; i<6; ++i )
  {
    u8 index = (mpAppParam->selectGroup * 6) + i;

    if( mpResortSave->IsNewHotSpaPokemon(index) )
    {
      // Newフラグをリセット
      mpResortSave->ResetNewHotSpaPokemon(index);
      updateTime = true;
    }
  }

  // 時間を再設定
  if( updateTime )
  {
    u8 lv = mpResortSave->GetResortLevel(Savedata::ResortSave::RESORT_KIND_HOTSPA) - 1;

    mpResortSave->SetHotSpaPokemonTime(
      mpAppParam->selectGroup,
      mpFamiliarityBin[lv].c1/3600,
      (mpFamiliarityBin[lv].c1/60)%60,
      mpFamiliarityBin[lv].c1%60 );
  }
}


//------------------------------------------------------------------
/**
 * @brief   ボックスのみのポケモン数を得る
 */
//------------------------------------------------------------------
int ResortInitLogic::GetBoxPokemonNum( void )
{
  return mBoxPokeCount;
}

//------------------------------------------------------------------
/**
 * @brief   ボックス+温泉+運動のポケモン数を得る
 */
//------------------------------------------------------------------
int ResortInitLogic::GetBoxAndHotSpaAndGimPokemonNum( void )
{
  return mBoxPokeCountAll;
}



//------------------------------------------------------------------
/**
 * @brief   探検開始
 */
//------------------------------------------------------------------
void ResortInitLogic::StartAdventure(void)
{

  int lv = mpResortSave->GetResortLevel( Savedata::ResortSave::RESORT_KIND_ADVENTURE )-1;

  //次の冒険時間
  mpResortSave->SetAdventureTime(mpAdventureBin[lv].c1 / 3600, (mpAdventureBin[lv].c1 / 60) % 60,mpAdventureBin[lv].c1 % 60);
  mpResortSave->SetUpdateFlg();
}


void ResortInitLogic::StopAdventure(int step)
{
  if(step==0){
    mpResortSave->SetAdventureTime( 0, 0, 0 );
  }
  else{
    mpResortSave->SetAdventureCoursePattern( Savedata::ResortSave::RESORT_ADV_COURSE_NONE );
    mpResortSave->SetUpdateFlg();
  }
}


bool ResortInitLogic::IsAdventureCourseSet(void)
{
  int cource = mpResortSave->GetAdventureCoursePattern();  //遊んでいたコースをえる

  if(cource==Savedata::ResortSave::RESORT_ADV_COURSE_NONE ){
    return false;
  }
  return true;
}




GFL_NAMESPACE_END( Resort )
GFL_NAMESPACE_END( App )


