//======================================================================
/**
 * @file AlolaTime.h
 * @date 2015/10/21 11:54:37
 * @author takahashi_tomoya
 * @brief アローラ時間
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __ALOLA_TIME_H_INCLUDED__
#define __ALOLA_TIME_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <system/include/Date/gfl2_Date.h>

#include "niji_conv_header/poke_lib/monsno_def.h"

GFL_NAMESPACE_BEGIN(Field)
  class EvTime;
  class EventWork;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(GameSys)

  class GameData;


  /**
   *  @brief 時間インターフェース
   */
  class INijiDate
  {
    GFL_FORBID_COPY_AND_ASSIGN(INijiDate);

    // EvTimeではpriveteに参照することを許す。
    friend Field::EvTime;

  public:

    static const s32 DAY_SECOND;    // 1日の秒数
    static const s32 HOUR_SECOND;   // １時間の秒数
    static const s32 MINUTE_SECOND; // 1分の秒数


    INijiDate(){}
    virtual ~INijiDate(){}

    /**
     *  @brief アローラ時間 時間取得
     */
    virtual s32 GetHour() const = 0;

    /**
     *  @brief アローラ時間　分取得
     */
    virtual s32 GetMinute() const = 0;

    /**
     *  @brief アローラ時間　秒取得
     */
    virtual s32 GetSecond() const = 0;

    /**
     *  @brief アローラ時間　ミリ秒取得
     */
    virtual s32 GetMilliSecond() const = 0;

    /**
     *  @brief 現在時間の取得
     */
    virtual void Update() = 0;


#if PM_DEBUG
   /**
    *  @brief  フェイクモード開始
    */
    virtual void StartFakeTime(u32 rate)
    {
      gfl2::system::Date::StartFakeTime(rate);
    }

    /**
     * @brief  フェイクモード終了
     */
    virtual void StopFakeTime( void )
    {
      gfl2::system::Date::StopFakeTime();
    }

    /**
     * @brief  時間と分をFixする
     * @param  hour    時
     * @param  minute  分
     */
    virtual void FixFakeTime( s32 hour, s32 minute ) = 0;
#endif

    /**
     *  @brief Dateの内容をsecond秒数分引く
     */
    static void SubDateSecond( gfl2::system::Date * pDate, s32 second );

    /**
     *  @brief Dateの内容をsecond秒数分足す
     */
    static void AddDateSecond( gfl2::system::Date * pDate, s32 second );

    /**
     *  @brief DateとDateの引き算 (差分の秒数を返す)
     */
    static s64 SubDate( const gfl2::system::Date * cpDate0, const gfl2::system::Date * cpDate1 );

    // Date情報を秒に置き換える。 Win32 はうるう年　非対応なので注意
    static s64 GetDateToSecond( const gfl2::system::Date & cpDate );

    // 秒を元に、Dateを復元する。
    static void SetSecondToDate( gfl2::system::Date * pDate, s64 second );

#if PM_DEBUG
    // ログ出力
    void DebugLog(void) const;
    static void DebugLog(const gfl2::system::Date& date);
#endif // PM_DEBUG

  protected:

    // 大本のDateを取得。
    virtual const gfl2::system::Date* GetDateConst() const = 0;


  };


  /**
   * @class AlolaTime
   * @brief アローラー時間
   * @note 内部でGameManagerをグローバル参照します。 GameManager及びGameData生成前に実行できません。
   */
  class AlolaTime : public INijiDate
  {
    GFL_FORBID_COPY_AND_ASSIGN(AlolaTime);

    // EvTimeではpriveteに参照することを許す。
    friend Field::EvTime;

  public:

    static const u32 SUN_RESET_HOUR;    // サンバージョンの1日リセット時間
    static const u32 MOON_RESET_HOUR;   // ムーンバージョンの1日リセット時間

    enum AlolaOtherWorldType
    {
      ALOLA_OTHER_WOLRD_TYPE_NIKKOU,    ///< 別世界タイプ　6時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_GEKKOU,    ///< 別世界タイプ 18時ジャンプ
      ALOLA_OTHER_WOLRD_TYPE_MAX,       ///< 別世界最大値

      ALOLA_OTHER_WOLRD_TYPE_NORMAL = ALOLA_OTHER_WOLRD_TYPE_MAX, ///< 祭り後通常世界
      ALOLA_OTHER_WOLRD_TYPE_GAMESTART,                           ///< ゲーム開始時
    };

  public:

    AlolaTime();
    virtual ~AlolaTime();

    /**
     *  @brief 現在のカントー時間から初期化
     */
    void Initialize(void);

    /**
     *  @brief 現在のカントー時間から初期化
     *  @param  アローラオフセット指定版
     */
    void Initialize( s64 alola_offset );

    /**
     *  @brief 指定のカントー時間とアローラオフセットから初期化
     */
    void Initialize( const gfl2::system::Date & date, s64 alola_offset );

    /**
     *  @brief アローラ時間 時間取得
     */
    virtual s32 GetHour() const;

    /**
     *  @brief アローラ時間　分取得
     */
    virtual s32 GetMinute() const;

    /**
     *  @brief アローラ時間　秒取得
     */
    virtual s32 GetSecond() const;

    /**
     *  @brief アローラ時間　ミリ秒取得
     */
    virtual s32 GetMilliSecond() const;

    /**
     *  @brief 現在時間の取得
     */
    virtual void Update();

#if PM_DEBUG
    /**
     * @brief  時間と分をFixする(こちらはデバッグ限定。Alola時間を固定するためには、FixAlolaOnlyFakeTimeを使う）
     * @param  hour    時
     * @param  minute  分
     */
    virtual void FixFakeTime( s32 hour, s32 minute );
#endif

    /**
     *  @brief 製品版用　アローラ限定のFakeTime
     */
    void FixAlolaOnlyFakeTime( s32 hour, s32 minute );

    /**
     *  @brief 製品版用　アローラ限定のFakeTimeのクリア
     */
    void ClearAlolaOnlyFakeTime(void);

    // フェイク時間中かチェック
    b32 IsFixAlolaOnlyFakeTime( void ) const { return  m_sIsFakeDate; }

    // フェイク時間　セーブ復帰処理
    static b32 GetFixAlolaOnlyFakeTimeSaveParam( u8* hour, u8* minute, u8* second );
    static void SetFixAlolaOnlyFakeTimeSaveParam( b8 flag, u8 hour, u8 minute, u8 second );

    /**
     *  @brief アローラ時間補正オフセットを求める。
     *  @param  rom_version   ロムバージョン
     *  @param  type          別世界タイプ　
     */
    static s64 GetAlolaOffset( u32 rom_version, AlolaOtherWorldType type );

    /**
     *  @brief アローラ別世界時間のオフセットを設定する。
     */
    static void StartAlolaOtherWorldOffset( GameSys::GameData* p_gdate, AlolaOtherWorldType otherworld_type );


    /**
     *  @brief アローラ時間に戻す。
     */
    static void ClearAlolaOtherWorldOffset( GameSys::GameData* p_gdate );


    /**
     *  @brief アローラ時間が別世界状態かチェック
     */
    static b32 CheckAlolaOtherWorld( const GameSys::GameData* cp_gdata );
    static b32 CheckAlolaOtherWorld( const Field::EventWork* cp_evwork );

    /**
     *  @brief アローラ別世界時間に行く能力を使えるかチェック
     */
    static b32 IsAlolaTimeJumpOtherWorldSkillUse( MonsNo monsNo, const GameSys::GameData* cp_gdata );

    /**
     *  @brief ロムバージョンのゲーム時間開始　Hour取得
     */
    static s32 GetRomGameStartHour( u32 rom_version );

    /**
     *  @brief ロムバージョンのゲーム時間　Hour取得(通常状態)
     */
    static s32 GetRomAlolaHour( u32 rom_version, s32 now_hour );


    /**
     *  @brief 別世界時間タイプごとの開始 Hour取得:
     */
    static s32 GetOtherWorldTypeGameStartHour( AlolaOtherWorldType type );

  private:

    virtual const gfl2::system::Date* GetDateConst() const { return &m_Date; }
    
    enum
    {
      SYS_WK_ALOLA_TIME_TYPE_REVERSE = 0,
      SYS_WK_ALOLA_TIME_TYPE_NORMAL = 1,
    };

  private:

    gfl2::system::Date m_Date;
    s64                m_AlolaOffset;


    static b32 m_sIsFakeDate; ///< フェイク日時フラグ
    static gfl2::system::Date m_sFakeDate;  ///< フェイク日時
  };


  /**
   * @class DeviceDate
   * @brief カントー時間
   */
  class DeviceDate : public INijiDate
  {
    GFL_FORBID_COPY_AND_ASSIGN(DeviceDate);


    // EvTimeではpriveteに参照することを許す。
    friend Field::EvTime;

  public:
    DeviceDate() : INijiDate()
    {
      gfl2::system::Date::GetNow(&m_Date);
    }
    virtual ~DeviceDate()
    {
    }


    /**
     *  @brief カントー時間　年の取得
     */
    s32 GetYear() const
    {
      return m_Date.GetYear();
    }

    /**
     *  @brief カントー時間　月の取得
     */
    s32 GetMonth() const
    {
      return m_Date.GetMonth();
    }

    /**
     *  @brief カントー時間　日の取得
     */
    s32 GetDay() const
    {
      return m_Date.GetDay();
    }

    /**
     *  @brief カントー時間 時間取得
     */
    virtual s32 GetHour() const
    {
      return m_Date.GetHour();
    }
    

    /**
     *  @brief カントー時間　分取得
     */
    virtual s32 GetMinute() const
    {
      return m_Date.GetMinute();
    }

    /**
     *  @brief カントー時間　秒取得
     */
    virtual s32 GetSecond() const
    {
      return m_Date.GetSecond();
    }


    /**
     *  @brief カントー時間　ミリ秒取得
     */
    virtual s32 GetMilliSecond() const
    {
      return m_Date.GetMilliSecond();
    }

    /**
     *  @brief 現在カントー時間の取得
     */
    virtual void Update()
    {
      gfl2::system::Date::GetNow(&m_Date);
    }

#if PM_DEBUG
    /**
     * @brief  時間と分をFixする
     * @param  hour    時
     * @param  minute  分
     */
    virtual void FixFakeTime( s32 hour, s32 minute )
    {
      gfl2::system::Date::FixFakeTime( hour, minute );
    }
#endif


    gfl2::system::Date* GetDate() { return &m_Date; }
    virtual const gfl2::system::Date* GetDateConst() const { return &m_Date; }

  private:

    /**
     *  @brief  時間を設定
     */
    void SetDate( const gfl2::system::Date& date )
    {
      gfl2::system::Date::Copy(&date, &m_Date);
    }

    gfl2::system::Date m_Date;

  };




GFL_NAMESPACE_END(GameSys)

#endif // __ALOLA_TIME_H_INCLUDED__