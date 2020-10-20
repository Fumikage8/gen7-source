//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *  GAME FREAK inc.
 *
 *  @file   EvTime.cpp
 *  @brief  ゲーム内時間制御関連
 *  @author Takeda & Iwasawa
 *  @date 2015.06.01 
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include "Field/FieldStatic/include/Time/EvTime.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/NijiTime.h"
#include "PokeTool/include/PokeToolForm.h"
#include "Savedata/include/JoinFestaDataSave.h"
#include <Savedata/include/BattleFesSave.h>

#if defined(GF_PLATFORM_CTR)
  #include <nn/cfg/CTR/cfg_Api.h>
#endif

#include "niji_conv_header/field/flagwork/FlagDefine.h"

#include "Field/FieldStatic/include/Berry/FieldBerryManager.h"

#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"

// ロトムパワー
#include "GameSys/include/GameManager.h"
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

namespace Field{

#if PM_DEBUG
#define DEBUG_FIELD_EVTIME_ON (0)

#if DEBUG_FIELD_EVTIME_ON

  static void DEBUG_EVTIME_DEBUG_PRINT( const Savedata::GameTime* cp_gtime, const gfl2::system::Date* cp_now )
  {

    if( cp_gtime ){
      gfl2::system::Date date;
      cp_gtime->GetDate( &date );
      GFL_PRINT( "SaveTime %d/%d/%d  %02d:%02d:%02d\n",
          date.GetYear(),
          date.GetMonth(),
          date.GetDay(),
          date.GetHour(),
          date.GetMinute(),
          date.GetSecond() );
    }

    if( cp_now ){
      GFL_PRINT( "NowTime %d/%d/%d  %02d:%02d:%02d\n",
          cp_now->GetYear(),
          cp_now->GetMonth(),
          cp_now->GetDay(),
          cp_now->GetHour(),
          cp_now->GetMinute(),
          cp_now->GetSecond() );
    }

  }

#endif

#define DEBUG_EVTIME_DEBUG_PRINT(...) ((void)0)

#else

#define DEBUG_EVTIME_DEBUG_PRINT(...) ((void)0)

#endif

//-----------------------------------------------------------------------------
/**
 *          定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *          クラス宣言
*/
//-----------------------------------------------------------------------------
  // 最初の更新チェック
  b32 EvTime::m_FirstUpdate = true;

  // 一度に更新する分の最大値
  const u32 EvTime::MINUTE_UPDATE_MAX = (60 * 24 * 365) * 5;  // 一度の更新は5年まで

  // @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
  b8 EvTime::m_isUpdateMinuteEvent = false;
  b8 EvTime::m_isUpdateDayEvent = false;

//-----------------------------------------------------------------------------
/**
 *          実装
*/
//-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *  @brief  イベント時間更新
   */
  //-----------------------------------------------------------------------------
  void EvTime::Update(GameSys::GameData * gdata, b32 is_field /*=false*/)
  {
    // @fix MMCat[231][226]
    ClearUpdateEventFlag();

    GameSys::DeviceDate now_device_date;
    gfl2::system::Date sv_device_date;

    Savedata::GameTime* p_gtime = gdata->GetGameTime();


    if( !p_gtime->IsUseFlag() ){
      // 不正なセーブデータ状態なので、更新しない。
      return ;
    }

    //EvTimeで保存している本体時間を取得
    p_gtime->GetDate( &sv_device_date );

    // 本体時間の更新
    {
      s32 diff_day, diff_minute;
      // 更新作業
      if( !ChangeTimeCalc( p_gtime, &now_device_date, &diff_day, &diff_minute ) ){
        // 時間経過に異常があった場合はスルー
        return ;
      }
      if( (diff_minute == 0) && (diff_day != 0) ){
        GFL_ASSERT( 0 ); //, "分が進んでないのに日が進んだ。" );
        return ;
      }
      if( diff_minute )
      {
        // ペナルティ時間計算
        p_gtime->CountDownPenaltyTime( diff_minute );
        // 分更新時の処理
        UpdateMinuteEvent( gdata, diff_minute, &now_device_date, m_FirstUpdate, is_field );
        // @fix MMCat[231][226]
        m_isUpdateMinuteEvent = true;
      }
      if( diff_day ){
        // 日にち更新時のイベント
        UpdateDayEvent( gdata, diff_day, m_FirstUpdate, is_field );
        // @fix MMCat[231][226]
        m_isUpdateDayEvent = true;
      }
    }

    // 最初の更新終了
    m_FirstUpdate = false;
  }
  
  //------------------------------------------------------------------------------
  /**
   * @func        PenaltyTimeSet
   * @brief       PenaltyTimeSet関数.
   * @author      N.Takeda
   * @date        2016/04/14(木) 21:44:03
   *
   * @return      なし (none)
   */
  //------------------------------------------------------------------------------
  void  EvTime::PenaltyTimeSet( GameSys::GameData * gdata )
  {
    Field::EventWork* ev = gdata->GetSaveData()->GetEventWork();
    
    /// 経過日数のリセット.
    if( ev ){
      u16* p_work = ev->GetEventWorkAdrs( WK_OTHER_REAL_DAYS );
      if( p_work ) *p_work = 0;
    }
    
  }

  //----------------------------------------------------------------------------
  /**
   * @brief   イベント時間更新処理（ゲーム開始時の初回のみ）
   * @note    日付・曜日などで発生するイベントを初回ゲーム開始時のために設定する
   */
  //----------------------------------------------------------------------------
  void EvTime::DeviceUpdateForFirstDay( GameSys::GameData * gdata )
  {
    //gfl2::system::Date date;
    //gdata->GetGameTime()->GetDate( &date );

    //誕生日イベントフラグ更新
    updateBirthdayFlag( gdata );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲームデータから、現在の時間を取得する。
   */
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /**
   *  @brief  時の取得  (アローラ時間)
   */
  //-----------------------------------------------------------------------------
  s32 EvTime::GetAlolaHour( const GameSys::GameData * gdata )
  {
    GameSys::AlolaTime alola;
    SetupAlolaTime( gdata, &alola );
    return alola.GetHour();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  分の取得  (アローラ時間)
   */
  //-----------------------------------------------------------------------------
  s32 EvTime::GetAlolaMinute( const GameSys::GameData * gdata )
  {
    GameSys::AlolaTime alola;
    SetupAlolaTime( gdata, &alola );
    return alola.GetMinute();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  秒の取得    (アローラ時間))
   */
  //-----------------------------------------------------------------------------
  s32 EvTime::GetAlolaSecond( const GameSys::GameData * gdata )
  {
    GameSys::AlolaTime alola;
    SetupAlolaTime( gdata, &alola );
    return alola.GetMinute();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ゾーン時間を取得   (アローラ時間)
   */
  //-----------------------------------------------------------------------------
  GameSys::TimeZone::Zone EvTime::GetAlolaTimeZone(const GameSys::GameData * gdata)
  {
    GameSys::AlolaTime alola;
    SetupAlolaTime( gdata, &alola );
    return GameSys::TimeZone::ConvertHourToTimeZone( alola.GetHour() );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  タイムゾーンが昼間か夜間か？   (アローラ時間)
   */
  //-----------------------------------------------------------------------------
  bool EvTime::IsAlolaDayTime(const GameSys::GameData * gdata)
  {
    GameSys::TimeZone::Zone timezone = EvTime::GetAlolaTimeZone( gdata );

    switch ( timezone )
    {
    case GameSys::TimeZone::MORNING:
    case GameSys::TimeZone::NOON:
    case GameSys::TimeZone::EVENING:
      return true;
    case GameSys::TimeZone::NIGHT:
    case GameSys::TimeZone::MIDNIGHT:
      return false;
    default:
      break;
    }
    return false;
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  曜日の取得（本体時間)
   */
  //-----------------------------------------------------------------------------
  s32 EvTime::GetDeviceWeek( const GameSys::GameData * gdata )
  {
    GameSys::DeviceDate device;
    SetupDeviceDate( gdata, &device );
    return device.GetDateConst()->GetWeek();
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲーム開始時間の取得  (本体時間)
   *
   *  @param  gdata   ゲームデータ
   *  @param  date  開始時間可能先
   */
  //-----------------------------------------------------------------------------
  void EvTime::GetGameStartDeviceDateTime(const GameSys::GameData * gdata, gfl2::system::Date * date)
  {
    const Savedata::GameTime* p_gtime = gdata->GetGameTime();
    date->SetDateTimeToSecond( p_gtime->GetStartSec() );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲームクリア時間の取得   (本体時間)
   *
   *  @param  gdata   ゲームデータ
   *  @param  date  クリア時間可能先
   */
  //-----------------------------------------------------------------------------
  void EvTime::GetGameClearDeviceDateTime(const GameSys::GameData * gdata, gfl2::system::Date * date)
  {
    const Savedata::GameTime* p_gtime = gdata->GetGameTime();
    date->SetDateTimeToSecond( p_gtime->GetClearSec() );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  ゲームクリア時間の設定  (本体時間)
   */
  //-----------------------------------------------------------------------------
  void EvTime::SetGameClearDeviceDateTime(GameSys::GameData * gdata)
  {
    Savedata::GameTime* p_gtime = gdata->GetGameTime();
    gfl2::system::Date date;
    gfl2::system::Date::GetNow( &date );
    p_gtime->SetClearSec( date.GetDateTimeToSecond() );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在時間の取得  (本体時間：EvTime)
   */
  //-----------------------------------------------------------------------------
  void EvTime::GetDeviceDateTime(GameSys::GameData * gdata, gfl2::system::Date * date)
  {
    Savedata::GameTime* p_gtime = gdata->GetGameTime();
    p_gtime->GetDate( date );
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  現在時間の取得  (本体時間：現在進行形)
   */
  //-----------------------------------------------------------------------------
  void EvTime::GetDeviceRealDateTime( gfl2::system::Date * date)
  {
    gfl2::system::Date::GetNow( date );
  }

  //----------------------------------------------------------------------------
  /**
   * @brief
   */
  //----------------------------------------------------------------------------
  bool EvTime::IsBirthday( const GameSys::GameData * gdata )
  {
    return gdata->GetEventWork()->CheckEventFlag( SYS_FLAG_BIRTHDAY );
  }


  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  bool EvTime::IsBirthday( const GameSys::GameData * gdata, int birth_month, int birth_day )
  {
    const Savedata::GameTime* p_gtime = gdata->GetGameTime();
    gfl2::system::Date date;
    p_gtime->GetDate( &date );

    s32 month       = date.GetMonth();
    s32 day         = date.GetDay();

    if ( date.IsLeapYear() == false && birth_month == 2 && birth_day == 29 )
    { //2月29日生まれの場合、うるう年以外では誕生日を28日として判定
      birth_day = 28;
    }

    if ( ( month == birth_month ) && ( day == birth_day ) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  void EvTime::GetBirthday( const GameSys::GameData * gdata, s32* birth_month, s32* birth_day )
  {
#if defined(GF_PLATFORM_CTR)
    // 誕生日か判定
    nn::cfg::CTR::Birthday birthday;

    nn::cfg::Initialize();
    nn::cfg::GetBirthday( &birthday );
    nn::cfg::Finalize();

    *birth_month = birthday.month;
    *birth_day   = birthday.day;
#else
    *birth_month = 8;
    *birth_day   = 6;
#endif
  }

  //------------------------------------------------------------------
  ///   分更新イベント処理が発動したか否か
  //------------------------------------------------------------------
  bool EvTime::IsUpdateMinuteEvent()
  {
    return m_isUpdateMinuteEvent;
  }

  //------------------------------------------------------------------
  ///   日更新イベント処理が発動したか否か
  //------------------------------------------------------------------
  bool EvTime::IsUpdateDayEvent()
  {
    return m_isUpdateDayEvent;
  }

  //----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  bool EvTime::updateBirthdayFlag( GameSys::GameData * gdata )
  {
    s32 birth_month = 1;
    s32 birth_day   = 1;

    Savedata::GameTime* p_gtime = gdata->GetGameTime();
    gfl2::system::Date date;
    p_gtime->GetDate( &date );

    EvTime::GetBirthday(gdata, &birth_month, &birth_day);


    gdata->GetEventWork()->ResetEventFlag( SYS_FLAG_LEAPER_BIRTHDAY );
    if ( IsBirthday( gdata, birth_month, birth_day ) == true )
    {
      if ( date.IsLeapYear() == false && birth_month == 2 && birth_day == 29 )
      {// 誕生日判定されたのに2月29日生まれの人でうるう年以外の場合、特殊判定用にフラグを立てる
        gdata->GetEventWork()->SetEventFlag( SYS_FLAG_LEAPER_BIRTHDAY );
      }
      gdata->GetEventWork()->SetEventFlag( SYS_FLAG_BIRTHDAY );
      return true;
    }
    else
    {
      gdata->GetEventWork()->ResetEventFlag( SYS_FLAG_BIRTHDAY );
      return false;
    }
  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  日にち更新イベント
   *
   *  @param  * gdata   ゲームデータ
   *  @param  diff_days 変わった日にち
   *  @param  is_first      ゲーム開始最初の更新ならTRUE
   *                        セーブ復帰時の1回目の更新は、考慮したくないときに使用。
   *                        セーブ復帰した場合でも、セーブ日時から、1分以上の差がないとtrueにならないので注意
   */
  //-----------------------------------------------------------------------------
  void EvTime::UpdateDayEvent(GameSys::GameData * gdata, s32 diff_days, b32 is_first, b32 is_field )
  {
    bool is_penalty = gdata->GetGameTime()->IsPenaltyMode();

    // 時間を表示
    //TOMOYA_PRINT( "DayUpdate minute %d  second %d\n", GetMinute(gdata), GetSecond(gdata) );


    //下記のような感じで時間関連イベント更新処理を追加する
    //
    //TVData_Update(gdata, diff_days);
    //ZukanHyouka_Update
    //…

    /*
     * 1日毎にクリアするフラグ(逆ペナルティ版)
     * ペナルティに関係なく0時またぎでクリア。またペナルティ発生の瞬間にもクリアする
     */
    {
      gdata->GetEventWork()->ClearTimeFlagsPenaltyInverse();
    }

    {
      // 誕生日フラグを操作
      updateBirthdayFlag( gdata );

      // ポケセン初回フラグをリセット
      gdata->GetEventWork()->ResetEventFlag( SYS_FLAG_BD_POKECEN_FIRST );

    }


    // MyStatusに絡む
    {
      gdata->GetPlayerStatus()->ResetWithDaytime();
    }

    // kujira新規：トリミアンとフーパのフォルム処理
    PokeTool::form::UpdatePoodleForm( gdata->GetPlayerParty(), diff_days );

    // レコードデータの１日１回更新処理
    gdata->GetRecordSaveData()->DailyUpdate();

    // ポケルス感染の経過処理
    gdata->GetPlayerParty()->DecreasePokerusDayCount( diff_days );

    // ポケリフレ：
    gdata->GetMisc()->AddKawaigariPastDays( diff_days );    

    //↑GFBTS1657 ペナルティ不要のものを移動 ここまで
    /////////////////////////////////////////////////////

    //ペナルティタイムのときはここでリターン
    if(is_penalty)
    {
      return;
    }

    // きのみ復活
    gdata->GetFieldBerryManager(true)->BearBerrys();    // ここはJFのダミーではなく本物を制御

    // ジョインフェスタで1日毎にクリアする処理
    gdata->GetJoinFestaData()->ResetWithDaytime();

    // 今日のランダムの種を求める。
    // ゴミ箱エンカウントで使用予定だったがxyでは未使用。今後使う場合はペナルティをつけるかどうか要再判断 130427 by iwasawa
    // IDくじで使用。ペナルティタイムの影響をうけないと困る。130510 by hosaka
    // ランダム隠しアイテムでも使用 140107 by hosaka
    {
      gdata->GetSaveData()->GetRandomGroup()->UpdateRandSeed( diff_days );
    }

    // 1日毎にクリアするフラグ
    {
      gdata->GetEventWork()->ClearTimeFlags();
      gdata->GetEventWork()->ClearRealTimeFlags();
    }

    // バトルフェス１日毎に更新する処理
    gdata->GetSaveData()->GetBattleFesSave()->UpdateBattleFesDayEvent();

    {
      /// ゲーム開始からの開始時間.
      EventWork* ev = gdata->GetEventWork();
      if( ev ){
        u16* p_work = ev->GetEventWorkAdrs( WK_OTHER_REAL_DAYS );
        if( p_work ) *p_work = *p_work + diff_days;
      }
    }


    // 日数のカウントダウン
    {
      EventWork* ev = gdata->GetEventWork();
      if (ev)
      {
        u16* p_work = ev->GetEventWorkAdrs(WK_SYS_DAY_COUNTDOWN_01);
        if (p_work)
        {
          if ((*p_work) > 0 && diff_days > 0) // diff_daysは念のため
          {
            s32 work = static_cast<s32>(*p_work);
            if (diff_days >= work)
            {
              // 残り日数よりも経過日数が多いのであれば0に
              work = 0;
            }
            else
            {
              // 残り日数未満であれば減算(u16範囲内)
              work -= diff_days; 
            }

            (*p_work) = static_cast<u16>( work );
          }
        }
      }
    }




    if( is_field == false ) return;

    //ここから下はFieldmapのUpdate中からコールされた時のみ有効

  }

  //----------------------------------------------------------------------------
  /**
   *  @brief  分更新イベント
   *
   *  @param  gdata         ゲームデータ
   *  @param  diff_minute   変わった分数
   *  @param  now           今の時間
   *  @param  is_first      ゲーム開始最初の更新ならTRUE
   *                        セーブ復帰時の1回目の更新は、考慮したくないときに使用。
   *                        セーブ復帰した場合でも、セーブ日時から、1分以上の差がないとtrueにならないので注意
   */
  //-----------------------------------------------------------------------------
  void EvTime::UpdateMinuteEvent(GameSys::GameData * gdata, s32 diff_minute, const GameSys::INijiDate* pDate, b32 is_first, b32 is_field)
  {
    //TOMOYA_PRINT( "UpdateMinute is_first %d diff %d\n", is_first, diff_minute );

    GameSys::AlolaTime nowAlolaTime;
    nowAlolaTime.Initialize();

    //シェイミのフォルム監視＆元に戻す処理
    PokeTool::form::UpdateSheimiForm( gdata->GetPlayerParty(), nowAlolaTime.GetDateConst(), diff_minute );

    if( is_field == false ) return;

    //ここから下はFieldmapのUpdate中からコールされた時のみ有効
    gdata->GetFieldSound()->UpdateFieldBGM();

    // ロトムパワー効果更新
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->UpdateMinuteEvent();

    // ジョインフェスタで1分毎にクリアする処理
    // ※三社会議要望によりペナルティタイムより前にもってきた。
    gdata->GetJoinFestaData()->UpdateMinuteTimeAtField( diff_minute );

    bool is_penalty = gdata->GetGameTime()->IsPenaltyMode();
    //ペナルティタイムのときはここでリターン
    if(is_penalty)
    {
      return;
    }

    // ロトム関連
    if( is_first == false )
    {
      gdata->GetFieldMenuPlayData()->UpdateTime( diff_minute );
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief [本体時間] 経過時間の算出処理
   * @param tm            時間経過管理ワーク
   * @param now_date      現在の日付・時刻
   * @param diff_day      過去から現在への経過時間（日単位）
   * @param diff_minute   過去から現在への経過時間（分単位）
   * @return  BOOL  正常取得されたかどうか（TRUE=正常、FALSE=は異常なので更新処理を実行しない）
   *
   * @note  now_timeの秒を０に切り捨てて演算することでRTCと時間更新の誤差を
   * 初回以外１秒未満にすることが可能（だけど検証時間が足りないのでやらない）
   */
  //------------------------------------------------------------------
  bool EvTime::ChangeTimeCalc( Savedata::GameTime* tm, const GameSys::DeviceDate* pDate, s32 * diff_day, s32 * diff_minute )
  {
    gfl2::system::Date sv_date;
    tm->GetDate( &sv_date );

    const gfl2::system::Date* now_date = pDate->GetDateConst();

    // 秒を0に切り捨て、分と日にちの更新タイミングをより、正確にする。
    {
      sv_date.SetSecond(0);
      sv_date.SetMilliSecond(0);
    }


    // 差分を取得する
    b32 l_is_reverse;
    s64 l_diff_minute;
    s32 l_diff_day;
    CalcDateDiff( sv_date, *now_date, &l_is_reverse, &l_diff_minute, &l_diff_day );

    *diff_day = 0;
    *diff_minute = 0;

    if ( l_is_reverse ) {
      //現在　＜　過去　…ありえないはず
      //現在時間をセットするだけで戻る
      DEBUG_EVTIME_DEBUG_PRINT( tm, now_date );
      TOMOYA_PRINT(" Reverse Time!!\n" );
      tm->SetDate( now_date );
      return false;
    }

    // @note 秒をチェックしているイベントが存在するために対応　2013.03.08
    // セーブデータに情報を更新する。
    // ここで、設定することで、秒数の変更も
    // きちんと反映される。
    //
    // 分の差分は、sv_dataの秒とミリ秒を０クリアしているので、
    // ちゃんととれる。
    tm->SetDate( now_date );

    if (l_diff_minute > 0) {
      //分の更新があった場合
      *diff_minute = l_diff_minute;
      DEBUG_EVTIME_DEBUG_PRINT( tm, now_date );
      TOMOYA_PRINT(" Update Minute %d\n", *diff_minute);
      MUNAKATA_PRINT("---Update minute %d second %d---\n", *diff_minute, now_date->GetSecond());

      //分の更新があった時だけ、日付の更新チェックを行う
      if ( l_diff_day > 0 )
      {
        *diff_day = l_diff_day;
        TOMOYA_PRINT(" Update Day %d\n", *diff_day );
      }
    }
    return true;
  }


  /**
   *  @brief 差分の取得
   */
  void EvTime::CalcDateDiff( const gfl2::system::Date& sv_date, const gfl2::system::Date& now_date, b32 * is_reverse, s64* diff_minute, s32* diff_day )
  {
    s64  sv_sec = sv_date.GetDateTimeToSecond();
    s32  sv_day = sv_date.GetDateTimeToDay();
    s64 now_sec = now_date.GetDateTimeToSecond();
    s32 now_day = now_date.GetDateTimeToDay();

    if ( now_sec < sv_sec ) {
      //現在　＜　過去　…ありえないはず
      //現在時間をセットするだけで戻る
      *is_reverse = true;
    }
    else
    {
      *is_reverse = false;
    }

    *diff_minute = ( now_sec - sv_sec ) / 60;

    // もし、5年以上の時間が変わっていたら、、
    // 5年でストップさせる。、
    if( *diff_minute > MINUTE_UPDATE_MAX ){
      *diff_minute = MINUTE_UPDATE_MAX;
    }

    *diff_day = now_day - sv_day;
  }

  /**
   *  @brief イベントアローラ時間の取得
   */
  void EvTime::SetupAlolaTime( const GameSys::GameData * gdata, GameSys::AlolaTime * pAlola )
  {
    // フェイク時間中はフェイク時間を返す。
    if(pAlola->IsFixAlolaOnlyFakeTime())
    {
      pAlola->Initialize();
    }
    // そうでなければ、イベントの時間を返す。
    else
    {
      const Savedata::GameTime* p_gtime = gdata->GetGameTime();
      gfl2::system::Date date;
      p_gtime->GetDate( &date );
      if( !GameSys::AlolaTime::CheckAlolaOtherWorld( gdata ) )
      {
        pAlola->Initialize( date, p_gtime->GetAlolaOffset() );
      }
      else
      {
        pAlola->Initialize( date, p_gtime->GetAlolaOtherWorldOffset() );
      }
    }
  }

  /**
   *  @brief イベント本体時間の取得
   */
  void EvTime::SetupDeviceDate( const GameSys::GameData * gdata, GameSys::DeviceDate * pDeviceDate )
  {
    const Savedata::GameTime* p_gtime = gdata->GetGameTime();
    gfl2::system::Date date;
    p_gtime->GetDate( &date );
    pDeviceDate->SetDate( date );
  }

  /**
  *  @brief m_isUpdateMinuteEvent＆m_isUpdateDayEventのクリア処理
  *  @fix MMCat[231][226] マップジャンプ時にEvTimeの更新が行われてしまっているために仕方なく実装
  */
  void EvTime::ClearUpdateEventFlag()
  {
    m_isUpdateMinuteEvent = false;
    m_isUpdateDayEvent = false;
  }

} // namespace Field
