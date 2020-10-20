//=============================================================================
/**
 * @brief 技データ
 * @file pml_WazaDataSystem.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <pml/include/pml_Type.h>
#include <pml/include/system/pml_Library.h>
#include <pml/include/waza/pml_WazaData.h>
#include <pml/include/waza/pml_WazaDataSystem.h>
#include "pml_WazaDataCache.h"
#include "pml_WazaDataLocal.h"

namespace pml {
  namespace wazadata {


    //static gfl2::str::MsgData* s_WazaName = NULL;
    //static gfl2::str::MsgData* s_WazaInfo = NULL;
    static WazaData* s_WazaData = NULL;
    static WazaDataCache* s_WazaDataCache = NULL;

    void Initialize( gfl2::heap::HeapBase* heap )
    {
      WazaData::Initialize( heap, Library::m_setting->garcPath_WazaData );
//      s_WazaName= GFL_NEW( heap ) gfl2::str::MsgData( Library::m_setting->garcPath_Message, Library::m_setting->msgDataID_WazaName, heap, gfl2::str::MsgData::LOAD_FULL );
      //s_WazaInfo= GFL_NEW( heap ) gfl2::str::MsgData( Library::m_setting->garcPath_Message, Library::m_setting->msgDataID_WazaInfo, heap, gfl2::str::MsgData::LOAD_FULL );
      s_WazaData = GFL_NEW( heap ) WazaData( heap );
      s_WazaDataCache = GFL_NEW( heap ) WazaDataCache( heap, Library::m_setting->cacheSize_WazaData );
    }

    void Finalize( void )
    {
  //    GFL_SAFE_DELETE( s_WazaName );
      //GFL_SAFE_DELETE( s_WazaInfo );
      GFL_SAFE_DELETE( s_WazaData );
      GFL_SAFE_DELETE( s_WazaDataCache );
      WazaData::Finalize();
    }
#if 0
    //-------------------------------------------------------------------------
    /**
     * @brief 技名を取得する
     * @param[in]  wazano  技No.
     * @param[out] strbuf  技名を格納するバッファ
     */
    //-------------------------------------------------------------------------
    void GetWazaName( WazaNo wazano, gfl2::str::StrBuf* strbuf )
    {
      if( WAZANO_MAX <= wazano ) {
        GFL_ASSERT(0);  //@check 引数異常の場合は何もしない
        return;
      }
      s_WazaName->GetString( wazano, *strbuf );
    }
#endif
#if 0
    //-------------------------------------------------------------------------
    /**
     * @brief 技の説明文を取得する
     * @param[in]  wazano  技No.
     * @param[out] strbuf  説明文を格納するバッファ
     */
    //-------------------------------------------------------------------------
    void GetWazaInfo( WazaNo wazano, gfl2::str::StrBuf* strbuf )
    {
      if( WAZANO_MAX <= wazano ) {
        GFL_ASSERT(0);
        return;
      }
      //s_WazaInfo->GetString( wazano, *strbuf );
    }
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief WAZA_DATA構造体のサイズを取得する
     */
    //-------------------------------------------------------------------------
    u32 GetWorkSize( void )
    {
      return sizeof( WAZA_DATA );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief キャッシュからデータを取得する
     * @param wazano  技No.
     * @return キャッシュにHITした場合はキャッシュのデータを返す.
     * @return キャッシュにHITしない場合は新たに読み込み, キャッシュに書き込んだデータを返す.
     */
    //-------------------------------------------------------------------------
    void LoadWazaData( WazaNo wazano, WazaData* buffer )
    {
      bool hit = s_WazaDataCache->GetData( wazano, buffer );
      if( !hit ) {
        buffer->LoadData( wazano );
        s_WazaDataCache->RegisterData( *buffer );
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のパラメータを取得する
     * @param wazano  技No.
     * @param paramID 取得するパラメータを指定
     * @return 指定したパラメータの値
     */
    //-------------------------------------------------------------------------
    int GetParam( WazaNo wazano, ParamID paramID )
    {
      GFL_ASSERT_MSG( wazano < WAZANO_MAX , "pml::WazaNo is over![%d]\n",wazano);  //@check さらに底部で引数異常を対応
      GFL_ASSERT( paramID < PARAM_ID_MAX ); //@check さらに底部で引数異常を対応
      LoadWazaData( wazano, s_WazaData );
      return s_WazaData->GetParam( paramID );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の各種フラグを調べる
     * @param wazano  技No.
     * @param flag    調べるフラグ
     * @return 指定したフラグの値
     */
    //-------------------------------------------------------------------------
    bool GetFlag( WazaNo wazano, WazaFlag flag )
    {
      GFL_ASSERT(wazano<WAZANO_MAX);  //@check さらに底部で引数異常を対応
      LoadWazaData( wazano, s_WazaData );
      return s_WazaData->GetFlag( flag );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のMaxPP値を取得する
     * @param wazano  技No.
     * @param upCnt  「マックスアップ」を使用した回数
     */
    //-------------------------------------------------------------------------
    u8 GetMaxPP( WazaNo wazano, u8 maxupcnt )
    {
      if( maxupcnt > 3 ){
        maxupcnt = 3;
      }
      u8  ppmax;
      ppmax = GetParam( wazano, PARAM_ID_BASE_PP );
      ppmax += ( ppmax * 20 * maxupcnt ) / 100;
      return  ppmax;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の威力値を取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    u16 GetPower( WazaNo wazano )
    {
      return GetParam( wazano, PARAM_ID_POWER );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のタイプを取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    PokeType GetType( WazaNo wazano )
    {
      int param = GetParam( wazano, PARAM_ID_TYPE );
      return static_cast<PokeType>( param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のダメージタイプを取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    WazaDamageType GetDamageType( WazaNo wazano )
    {
      int param = GetParam( wazano, PARAM_ID_DAMAGE_TYPE );
      return static_cast<WazaDamageType>( param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のカテゴリを取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    WazaCategory GetCategory( WazaNo wazano )
    {
      int param = GetParam( wazano, PARAM_ID_CATEGORY );
      return static_cast<WazaCategory>( param );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の必中フラグを取得する。
     * @note つばめがえしなどがTRUEを返す。
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    bool IsAlwaysHit( WazaNo wazano )
    {
      int flag = GetParam( wazano, PARAM_ID_MAST_HIT_FLAG );
      return (flag!=0?true:false);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の必ずクリティカルフラグを取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    bool IsMustCritical( WazaNo wazano )
    {
      int param = GetParam( wazano, PARAM_ID_CRITICAL_RANK );
      return (param == CRITICAL_MUST);
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技のダメージワザかどうかを取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    bool IsDamage( WazaNo wazano )
    {
      return GetParam( wazano, PARAM_ID_POWER ) != 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 技の天候効果を取得する
     * @param wazano  技No.
     */
    //-------------------------------------------------------------------------
    WazaWeather GetWeather( WazaNo wazano )
    {
      switch( wazano ){
      case WAZANO_AMAGOI:     return WAZA_WEATHER_RAIN;
      case WAZANO_NIHONBARE:  return WAZA_WEATHER_SHINE;
      case WAZANO_SUNAARASI:  return WAZA_WEATHER_SAND;
      case WAZANO_ARARE:      return WAZA_WEATHER_SNOW;
      default:                return WAZA_WEATHER_NONE;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 状態異常継続パラメータ取得を取得する
     * @param wazano  技No.
     * @retval SICKCONT_PARAM
     */
    //-------------------------------------------------------------------------
    SICKCONT_PARAM GetSickCont( WazaNo wazano )
    {
      SICKCONT_PARAM  param;
      int sick_cont = GetParam( wazano, PARAM_ID_SICK_CONT );
      if( sick_cont )
      {
        param.type = sick_cont;
        param.turnMin = GetParam( wazano, PARAM_ID_SICK_TURN_MIN );
        param.turnMax = GetParam( wazano, PARAM_ID_SICK_TURN_MAX );
      }
      return param;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ランク効果の種類数を取得する
     * @param[in]  wazano  ワザID
     * @param[out] volume  ランク効果の程度( +ならアップ, -ならダウン, 戻り値==RANKEFF_NULLの時のみ0 )
     * @retval WazaRankEffect  ランク効果ID
     */
    //-------------------------------------------------------------------------
    u8 GetRankEffectCount( WazaNo wazano )
    {
      u32 i, cnt = 0;
      for(i=0; i<RANK_STORE_MAX; ++i)
      {
        ParamID paramID = static_cast<ParamID>( PARAM_ID_RANKTYPE_1 + i );
        int rank_eff = GetParam( wazano, paramID );
        if( rank_eff != RANKEFF_NULL ){
          ++cnt;
        }
      }
      return cnt;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ランク効果を取得する
     * @param[in]  wazano  ワザID
     * @param[in]  idx     何番目のランク効果か？( 0〜 )
     * @param[out] volume  ランク効果の程度( +ならアップ, -ならダウン, 戻り値==RANKEFF_NULLの時のみ0 )
     * @retval WazaRankEffect ランク効果ID
     */
    //-------------------------------------------------------------------------
    WazaRankEffect GetRankEffect( WazaNo wazano, u32 idx, int* volume )
    {
      if(idx >= RANKEFF_NUM_MAX)
      {
        GFL_ASSERT( 0 );  // @fix ランク効果指定が無効値の場合はRANKEFF_NULL扱い
        *volume = 0;
        return RANKEFF_NULL;
      }
      ParamID ID_type = static_cast<ParamID>( PARAM_ID_RANKTYPE_1 + idx );
      int eff = GetParam( wazano, ID_type );
      if( eff != RANKEFF_NULL ) {
        ParamID ID_value = static_cast<ParamID>( PARAM_ID_RANKVALUE_1 + idx );
        *volume = GetParam( wazano, ID_value );
      }
      else{
        *volume = 0;
      }
      return static_cast<WazaRankEffect>( eff );
    }


    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、変化後の技の技No.を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaNo GetZenryokuWazaNo( WazaNo wazano )
    {
      u16 zenryoku_wazano = GetParam( wazano, PARAM_ID_ZENRYOKU_WAZANO );
      return static_cast<WazaNo>( zenryoku_wazano );
    }

    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、変化後の技の威力値を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    u16 GetZenryokuWazaPower( WazaNo wazano )
    {
      return GetParam( wazano, PARAM_ID_ZENRYOKU_POWER );
    }

    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、追加効果を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    ZenryokuWazaEffect GetZenryokuWazaEffect( WazaNo wazano )
    {
      u8 zenryoku_effect = GetParam( wazano, PARAM_ID_ZENRYOKU_EFFECT );
      return static_cast<ZenryokuWazaEffect>( zenryoku_effect );
    }


  } // namespace wazadata
} // namespace pml
