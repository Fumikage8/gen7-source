//======================================================================
/**
 * @file LunaSolDiveEvent.cpp
 * @date 2016/11/22 19:21:11
 * @author saito_nozomu
 * @brief ルナソルダイブアプリ呼び出し
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "App/AppEvent/include/LunaSolDive/LunaSolDiveEvent.h"

#include  <Sound/include/Sound.h>
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"

#include "App/LunaSolDive/include/Proc.h"

// savedata
#include "Savedata/include/MiscSave.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
* @brief   コンストラクタ
*
* @param   heap  ヒープ
*/
//-----------------------------------------------------------------------------
LunaSolDiveEvent::LunaSolDiveEvent(gfl2::heap::HeapBase * heap)
: GameSys::GameEvent(heap)
{
  m_pParam = NULL;
  m_BgmFadeCounter = 0;
}

//-----------------------------------------------------------------------------
/**
* @brief   デストラクタ
*/
//-----------------------------------------------------------------------------
LunaSolDiveEvent::~LunaSolDiveEvent()
{
}


//-----------------------------------------------------------------------------
/**
* @func    StartEvent
* @brief   イベント起動
* @date    2015.06.11
*
* @param   man     ゲームマネージャクラス
*
* @return  LunaSolDiveEventクラス
*/
//-----------------------------------------------------------------------------
LunaSolDiveEvent * LunaSolDiveEvent::StartEvent(GameSys::GameManager * gmgr)
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, LunaSolDiveEvent >(ev_man);
}

//-----------------------------------------------------------------------------
/**
* @func    SetupAppParam
* @brief   パラメータ設定
* @date    2015.12.09
*
* @param   prm   外部設定パラメータ
*/
//-----------------------------------------------------------------------------
void LunaSolDiveEvent::SetupAppParam(LUNASOL_DIVE_PARAM * prm)
{
  m_pParam = prm;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @retval  true  = 起動してよい
* @retval  false = 起動しない
*/
//-----------------------------------------------------------------------------
bool LunaSolDiveEvent::BootChk(GameSys::GameManager * /*gmgr*/)
{
  return true;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void LunaSolDiveEvent::InitFunc(GameSys::GameManager * /*gmgr*/)
{
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*
* @return  イベント制御関数の戻り値
*/
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT LunaSolDiveEvent::MainFunc(GameSys::GameManager * gmgr)
{
  enum
  {
    EVSEQ_BGM_FADEOUT = 0,
    EVSEQ_PROC_CREATE,
    EVSEQ_PROC_WAIT,
    EVSEQ_PROC_END,
  };

  switch (GetSeqNo()){
  case EVSEQ_BGM_FADEOUT:
    if (Sound::IsBGMPlaying() && (!Sound::IsBGMFade()))
    {
      Sound::PushBGMReq();
      SetSeqNo(EVSEQ_PROC_CREATE);
    }
    else
    {
      //不測の事態に備えて、タイムアウトを設ける
      m_BgmFadeCounter++;
      if (m_BgmFadeCounter > 30 * 5) //5秒待っても処理が終了しないときはBGMのリクエストを開始する
      {
        GFL_ASSERT(0);
        Sound::PushBGMReq();
        SetSeqNo(EVSEQ_PROC_CREATE);
      }
    }
    break;
  case EVSEQ_PROC_CREATE:
  {
    LunaSolDive::Proc *proc = GameSys::CallLunaSolDiveProc();
    proc->SetIOParameter(m_pParam);
    SetSeqNo(EVSEQ_PROC_WAIT);
    break;
  }

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if (!gmgr->IsProcessExists())
    {
      NOZOMU_PRINT("distance=%f, dist_type=%d, rare=%d\n", m_pParam->out_distance, m_pParam->out_distortion_type, m_pParam->out_rarelity);
      //アプリからの戻り値を受けて、飛び先となるゾーンと座標を確定する

      if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__CLIFF)
      {
        //崖
        m_pParam->out_zone_id = Z_F0701PH0901;
        m_pParam->out_pos = gfl2::math::Vector3(1311, 4, 3569);
        m_pParam->out_monsno = DrawLotsMonster(gmgr);
      }
      else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__PLAINS)
      {
        //平地
        m_pParam->out_zone_id = Z_F0701PH1001;
        m_pParam->out_pos = gfl2::math::Vector3(1341, 4, 3574);
        m_pParam->out_monsno = DrawLotsMonster(gmgr);
      }
      else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__CAVE)
      {
        //洞窟
        m_pParam->out_zone_id = Z_F0701PH1101;
        m_pParam->out_pos = gfl2::math::Vector3(1360, 4, 3582);
        m_pParam->out_monsno = DrawLotsMonster(gmgr);
      }
      else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__WATER)
      {
        //水
        m_pParam->out_zone_id = Z_F0701PH1201;
        m_pParam->out_pos = gfl2::math::Vector3(1372, 4, 3629);
        m_pParam->out_monsno = DrawLotsMonster(gmgr);
      }
      else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__UB)
      {
        //UB世界：UB（ゾーンは複数）
        if (m_pParam->in_first_mode) //初回モード時
        {
          m_pParam->out_zone_id = Z_F0701PH0201;
          m_pParam->out_pos = gfl2::math::Vector3(2122, 4, 6239);
        }
        else                         //初回モード以外
        {
          SetUBZone();
        }
      }
      else
      {
        GFL_ASSERT(0);
        SetHomeZone();
      }
      NOZOMU_PRINT("zone=%d\n", m_pParam->out_zone_id);

      SetSeqNo(EVSEQ_PROC_END);
    }
    break;

  case EVSEQ_PROC_END:
    Sound::PopBGMOnlyReq();
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
* @func    BookChk
* @brief   呼び出される関数 純粋仮想関数
* @date    2015.06.11
*
* @param   gmgr  ゲームマネージャークラス
*/
//-----------------------------------------------------------------------------
void LunaSolDiveEvent::EndFunc(GameSys::GameManager * gmgr)
{
}

//-----------------------------------------------------------------------------
/**
* @brief   ホームゾーンへのセット(不測の事態が起きたとき用)
*/
//-----------------------------------------------------------------------------
void LunaSolDiveEvent::SetHomeZone(void)
{
  if (GET_VERSION() == VERSION_SUN2)
  {
    //サンバージョンのとき
    m_pParam->out_zone_id = Z_F0401D0302;
    m_pParam->out_pos = gfl2::math::Vector3(2444, 109, 4555);
  }
  else
  {
    //ムーンバージョンのとき
    m_pParam->out_zone_id = Z_F0401D0402;
    m_pParam->out_pos = gfl2::math::Vector3(2444, 109, 4555);
  }
}

//-----------------------------------------------------------------------------
/**
* @brief   UBゾーンへのセット
*/
//-----------------------------------------------------------------------------
void LunaSolDiveEvent::SetUBZone(void)
{
  const u32 UB_ZONE_NUM = 5;
  const u32 zone_list[UB_ZONE_NUM] = {
    Z_F0701PH0101,  //ウツロイド
#if PM_VERSION == VERSION_SUN2
    Z_F0701PH0701,  //マッシブーン
#else
    Z_F0701PH0501,  //フェローチェ
#endif
    Z_F0701PH0301,  //デンジュモク
#if PM_VERSION == VERSION_SUN2
    Z_F0701PH0601,  //カミツルギ
#else
    Z_F0701PH0401,  //テッカグヤ
#endif
    Z_F0701PH0801,  //アクジキング
  };
  const gfl2::math::Vector3 pos_list[UB_ZONE_NUM] = {
    gfl2::math::Vector3(1730.0f, 4.0f, 7048.0f),  //ウツロイド
#if PM_VERSION == VERSION_SUN2
    gfl2::math::Vector3(4377.0f, 4.0f, 9601.0f),  //マッシブーン
#else
    gfl2::math::Vector3(2932.0f, 4.0f, 11263.0f),  //フェローチェ
#endif
    gfl2::math::Vector3(1336.0f, 4.0f, 7470.0f),  //デンジュモク
#if PM_VERSION == VERSION_SUN2
    gfl2::math::Vector3(1725.0f, -43.0f, 7340.0f),  //カミツルギ
#else
    gfl2::math::Vector3(6066.0f, 38.0f, 5983.0f),  //テッカグヤ
#endif
    gfl2::math::Vector3(1738.0f, 4.0f, 1460.0f),  //アクジキング
  };
  const u32 rate[4][UB_ZONE_NUM] = {
    { 60, 95, 97, 99, 100 },//rare1
    { 2,  62, 97, 99, 100 },//rare2
    { 0,  2,  62, 97, 100 },//rare3
    { 0,  0 , 0 , 64, 100 },//rare4
  };
  const int fix[4][UB_ZONE_NUM] = {
    { -6, -2, 0, 0, 0 },//rare1
    { 0,  -6, -2, 0, 0 },//rare2
    { 0,  0, -6, -2, 0 },//rare3
    { 0,  0, 0, -6, 0 },//rare4
  };

  //距離による係数を求める
  u32 coefficient = 0;
  if (m_pParam->out_distance / 500 <= 0) coefficient = 0;
  else coefficient = (m_pParam->out_distance / 500) - 1;
  //  @fix GFMMCat[134] ウルトラワープライド 距離に応じたUB世界の抽選が正しく行われていない >>係数の天井を設定して修正
  if (coefficient > 9) coefficient = 9;

  u32 rnd = System::GflUse::GetPublicRand(100);
  u32 rare = m_pParam->out_rarelity;

  if (rare > LunaSolDive::DISTORTION_RARITY__3)
  {
    GFL_ASSERT(0);
    rare = LunaSolDive::DISTORTION_RARITY__0;
  }
  
  //初期化
  m_pParam->out_zone_id = zone_list[0];
  m_pParam->out_pos = pos_list[0];
  NOZOMU_PRINT("rnd=%d, rare=%d\n",rnd, rare);
  //抽選
  for (int i = 0; i < UB_ZONE_NUM; i++)
  {
    NOZOMU_PRINT("rate=%d\n",rate[rare][i]);
    if (rnd < rate[rare][i]+fix[rare][i]*coefficient){
      m_pParam->out_zone_id = zone_list[i];
      m_pParam->out_pos = pos_list[i];
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
* @brief   伝説、準伝ポケモンの抽選
*/
//-----------------------------------------------------------------------------
u32 LunaSolDiveEvent::DrawLotsMonster(GameSys::GameManager * gmgr)
{
  u32 monsno = 0;
  u32 rnd;
  u32 rare;
  const u32 rate[4] = { 3, 3, 7, 100 };// 伝説ポケ、準伝ポケが出る確率
  const u32 plus[4] = { 0, 1, 3, 0 };// 距離によって当たり易くなる加算確率

  //１００％レアフラグのリセット処理
  GameSys::GameData* p_gdata = gmgr->GetGameData();
  Savedata::Misc* p_misc = p_gdata->GetMisc();
  p_misc->ResetMustRareEncTrigger();

  rare = m_pParam->out_rarelity;
  if (rare >= 4)
  {
    GFL_ASSERT(0);
    rare = 0;
  }

  //距離による係数を求める
  u32 coefficient = 0;
  if (m_pParam->out_distance / 500 <= 0) coefficient = 0;
  else coefficient = (m_pParam->out_distance / 500) - 1;
  //  @fix GFMMCat[134] ウルトラワープライド 距離に応じたUB世界の抽選が正しく行われていない >>係数の天井を設定して修正
  if (coefficient > 9) coefficient = 9;

  rnd = System::GflUse::GetPublicRand(100);//0～99
  bool hit = true;//初期化時はアタリ状態
  if (rate[rare]+plus[rare]*coefficient <= rnd)
  {
    //テーブルにしたがってハズレにする
    hit = false;
  }

  if (hit)
  {
    //伝説、準伝の抽選
    monsno = DrawLotsLegendMonsNo(gmgr);
    NOZOMU_PRINT("LEG_MONSNO=%d\n", monsno);
  }

  //抽選が外れた OR 不測の事態
  if (monsno == 0)
  {
    //ギミックエンカウントで絶対にレアカラーがでるフラグを立てるかを抽選
    const u32 rare_rate[4] = { 1, 1, 1, 0 };// １００％レアフラグを引く確率
    const u32 rare_plus[4] = { 0, 1, 2, 4 };// １００％レアフラグを引く加算確率  @fix MMCat[497] UB世界のシングルエンカウントのポケモン抽選に関して >>　確率修正

    rare = m_pParam->out_rarelity;
    rnd = System::GflUse::GetPublicRand(100);//0～99
    if (rnd < rare_rate[rare]+rare_plus[rare]*coefficient)
    {
      p_misc->SetMustRareEncTrigger();//100%レア
      NOZOMU_PRINT("SET_RARE\n");
    }
  }

  return monsno;
  
}

u32 LunaSolDiveEvent::DrawLotsLegendMonsNo(GameSys::GameManager * gmgr)
{
  const AppPokeData CRIFF[APP_MONS_NUM] = { //sun:6+2=8 moon:6+2=8
    { SYS_FLAG_CAPTURE_HURIIZAA,   MONSNO_HURIIZAA,   { MONSNO_NULL, MONSNO_NULL }, false },             //1
    { SYS_FLAG_CAPTURE_SANDAA,     MONSNO_SANDAA,     { MONSNO_NULL, MONSNO_NULL }, false },             //2
    { SYS_FLAG_CAPTURE_FAIYAA,     MONSNO_FAIYAA,     { MONSNO_NULL, MONSNO_NULL }, false },             //3
    { SYS_FLAG_CAPTURE_REKKUUZA,   MONSNO_REKKUUZA,   { MONSNO_KAIOOGA, MONSNO_GURAADON }, true },       //4
    { SYS_FLAG_CAPTURE_KURESERIA,  MONSNO_KURESERIA,  { MONSNO_NULL, MONSNO_NULL }, false },             //5
    { SYS_FLAG_CAPTURE_RANDOROSU,  MONSNO_RANDOROSU,  { MONSNO_TORUNEROSU, MONSNO_BORUTOROSU }, false }, //6
#if PM_VERSION == VERSION_SUN2
    { SYS_FLAG_CAPTURE_TORUNEROSU, MONSNO_TORUNEROSU, { MONSNO_NULL, MONSNO_NULL }, false },             //7
    { SYS_FLAG_CAPTURE_HOUOU,      MONSNO_HOUOU,      { MONSNO_NULL, MONSNO_NULL }, true },              //8
#else
    { SYS_FLAG_CAPTURE_BORUTOROSU, MONSNO_BORUTOROSU, { MONSNO_NULL, MONSNO_NULL }, false },             //7
    { SYS_FLAG_CAPTURE_IBERUTARU,  MONSNO_IBERUTARU,  { MONSNO_NULL, MONSNO_NULL }, true },              //8
#endif
    { 0,                           MONSNO_NULL,       { MONSNO_NULL, MONSNO_NULL }, false },             //9
  };

  const AppPokeData PLAINS[APP_MONS_NUM] = { //sun:4+4=8 moon:4+2=6
    { SYS_FLAG_CAPTURE_MYUUTUU,   MONSNO_MYUUTUU,   { MONSNO_NULL, MONSNO_NULL }, true  }, //1
    { SYS_FLAG_CAPTURE_KOBARUON,  MONSNO_KOBARUON,  { MONSNO_NULL, MONSNO_NULL }, false }, //2
    { SYS_FLAG_CAPTURE_TERAKION,  MONSNO_TERAKION,  { MONSNO_NULL, MONSNO_NULL }, false }, //3
    { SYS_FLAG_CAPTURE_BIRIZION,  MONSNO_BIRIZION,  { MONSNO_NULL, MONSNO_NULL }, false }, //4
#if PM_VERSION == VERSION_SUN2
    { SYS_FLAG_CAPTURE_RAIKOU,    MONSNO_RAIKOU,    { MONSNO_NULL, MONSNO_NULL }, false }, //5
    { SYS_FLAG_CAPTURE_DHIARUGA,  MONSNO_DHIARUGA,  { MONSNO_NULL, MONSNO_NULL }, true  }, //6
    { SYS_FLAG_CAPTURE_RESIRAMU,  MONSNO_RESIRAMU,  { MONSNO_NULL, MONSNO_NULL }, true  }, //7
    { SYS_FLAG_CAPTURE_ZERUNEASU, MONSNO_ZERUNEASU, { MONSNO_NULL, MONSNO_NULL }, true  }, //8
    { 0,                          MONSNO_NULL,      { MONSNO_NULL, MONSNO_NULL }, false }, //9
#else
    { SYS_FLAG_CAPTURE_ENTEI,     MONSNO_ENTEI,     { MONSNO_NULL, MONSNO_NULL }, false }, //5
    { SYS_FLAG_CAPTURE_ZEKUROMU,  MONSNO_ZEKUROMU,  { MONSNO_NULL, MONSNO_NULL }, true  }, //6
    { 0,                          MONSNO_NULL,      { MONSNO_NULL, MONSNO_NULL }, false }, //7
    { 0,                          MONSNO_NULL,      { MONSNO_NULL, MONSNO_NULL }, false }, //8
    { 0,                          MONSNO_NULL,      { MONSNO_NULL, MONSNO_NULL }, false }, //9
#endif
  };

  const AppPokeData CAVE[APP_MONS_NUM] = { //sun:4+2=6 moon:4+2=6
    { SYS_FLAG_CAPTURE_REZIROKKU,  MONSNO_REZIROKKU,  { MONSNO_NULL, MONSNO_NULL }, false },        //1
    { SYS_FLAG_CAPTURE_REZIAISU,   MONSNO_REZIAISU,   { MONSNO_NULL, MONSNO_NULL }, false },        //2
    { SYS_FLAG_CAPTURE_REZISUTIRU, MONSNO_REZISUTIRU, { MONSNO_NULL, MONSNO_NULL }, false },        //3
    { SYS_FLAG_CAPTURE_GIRATHINA,  MONSNO_GIRATHINA,  { MONSNO_DHIARUGA, MONSNO_PARUKIA }, true },  //4
#if PM_VERSION == VERSION_SUN2
    { SYS_FLAG_CAPTURE_HIIDORAN,   MONSNO_HIIDORAN,   { MONSNO_NULL, MONSNO_NULL }, false },        //5
    { SYS_FLAG_CAPTURE_GURAADON,   MONSNO_GURAADON,   { MONSNO_NULL, MONSNO_NULL }, true },         //6
#else
    { SYS_FLAG_CAPTURE_REZIGIGASU, MONSNO_REZIGIGASU, { MONSNO_NULL, MONSNO_NULL }, false },        //5
    { SYS_FLAG_CAPTURE_PARUKIA,    MONSNO_PARUKIA,    { MONSNO_NULL, MONSNO_NULL }, true },         //6
#endif
    { 0,                           MONSNO_NULL,       { MONSNO_NULL, MONSNO_NULL }, false },        //7
    { 0,                           MONSNO_NULL,       { MONSNO_NULL, MONSNO_NULL }, false },        //8
    { 0,                           MONSNO_NULL,       { MONSNO_NULL, MONSNO_NULL }, false },        //9
  };

  const AppPokeData WATER[APP_MONS_NUM] = {//sun:5+1=6 moon:5+3=8
    { SYS_FLAG_CAPTURE_SUIKUN,   MONSNO_SUIKUN,   { MONSNO_RAIKOU, MONSNO_ENTEI }, false },      //1
    { SYS_FLAG_CAPTURE_YUKUSII,  MONSNO_YUKUSII,  { MONSNO_NULL, MONSNO_NULL }, false },         //2
    { SYS_FLAG_CAPTURE_EMURITTO, MONSNO_EMURITTO, { MONSNO_NULL, MONSNO_NULL }, false },         //3
    { SYS_FLAG_CAPTURE_AGUNOMU,  MONSNO_AGUNOMU,  { MONSNO_NULL, MONSNO_NULL }, false },         //4
    { SYS_FLAG_CAPTURE_KYUREMU,  MONSNO_KYUREMU,  { MONSNO_RESIRAMU, MONSNO_ZEKUROMU }, true },  //5
#if PM_VERSION == VERSION_SUN2
    { SYS_FLAG_CAPTURE_RATHIOSU, MONSNO_RATHIOSU, { MONSNO_NULL, MONSNO_NULL }, false },         //6
    { 0,                         MONSNO_NULL,     { MONSNO_NULL, MONSNO_NULL }, false },         //7
    { 0,                         MONSNO_NULL,     { MONSNO_NULL, MONSNO_NULL }, false },         //8
    { 0,                         MONSNO_NULL,     { MONSNO_NULL, MONSNO_NULL }, false },         //9
#else
    { SYS_FLAG_CAPTURE_RATHIASU, MONSNO_RATHIASU, { MONSNO_NULL, MONSNO_NULL }, false },         //6
    { SYS_FLAG_CAPTURE_RUGIA,    MONSNO_RUGIA,    { MONSNO_NULL, MONSNO_NULL }, true },          //7
    { SYS_FLAG_CAPTURE_KAIOOGA,  MONSNO_KAIOOGA,  { MONSNO_NULL, MONSNO_NULL }, true },          //8
    { 0,                         MONSNO_NULL,     { MONSNO_NULL, MONSNO_NULL }, false },         //9
#endif
  };


  const AppPokeData* target;

  u32 monsno_lists[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };//initialize
  u32 list_item_num = 0;

  if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__CLIFF)
  {
    target = CRIFF;
  }
  else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__PLAINS)
  {
    target = PLAINS;
  }
  else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__CAVE)
  {
    target = CAVE;
  }
  else if (m_pParam->out_distortion_type == LunaSolDive::DISTORTION_TYPE__WATER)
  {
    target = WATER;
  }
  else
  {
    GFL_ASSERT(0);
    target = CRIFF;
  }

  Field::EventWork * evwk = gmgr->GetGameData()->GetEventWork();

  for (int i = 0; i < APP_MONS_NUM; i++)
  {
    if (target[i].monsno == MONSNO_NULL) break;

    if ( !evwk->CheckEventFlag(target[i].sys_flag) )//まだ捕まえていないとき以下の処理をする
    {
      if (target[i].need_monsno[0] != MONSNO_NULL)//手持ち条件がある
      {
        pml::PokeParty* pPlayerParty = gmgr->GetGameData()->GetPlayerParty();
        if (
          (pPlayerParty->CheckPokeExist(target[i].need_monsno[0]) == false) ||
          (pPlayerParty->CheckPokeExist(target[i].need_monsno[1]) == false))
        {
          continue;//手持ち条件を満たしていないので、抽選リストに追加しない
        }
      }
      //初回モード時に出してはいけないポケモンをフック
      if (m_pParam->in_first_mode && target[i].first_ng)
      {
        continue;
      }
      monsno_lists[list_item_num++] = target[i].monsno;
    }
  }

  if (list_item_num > 0)
  {
    int rnd = System::GflUse::GetPublicRand(list_item_num);
    return monsno_lists[rnd];
  }
  else
  {
    //抽選するものがない
    return 0;
  }

}

GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
