#if defined( GF_PLATFORM_CTR )
//============================================================================================
/**
 * @file		NetappBossUtility.h
 *
 * @brief		属性生成ユーティリティ
 */
//============================================================================================
#if !defined( NETAPP_BOSS_UTILITY_H_INCLUDE )
#define NETAPP_BOSS_UTILITY_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class BossUtility
{
public:

  enum
  {
    ATTRIBUTE_BUFFER_SIZE = 10,//出力バッファに必要なサイズ
  };

  typedef enum
  {
    BOSS_APP_TYPE_gift = 0,            //不思議な贈り物
    BOSS_APP_TYPE_giftSerial,          //不思議な贈り物（シリアル用）
    BOSS_APP_TYPE_regulation,          //レギュレーション
    BOSS_APP_TYPE_regulationRate,      //レギュレーション(レート)
    BOSS_APP_TYPE_cup,                 //大会選手証
    BOSS_APP_TYPE_regulationDownload,  //配信レギュレーション
    BOSS_APP_TYPE_globalAttraction,    //グローバルアトラクション

    BOSS_APP_TYPE_MAX,
  }BOSS_APP_TYPE;

  enum
  {
    BOSS_APP_TYPE_ID_MIN_gift                 = 10000,    //不思議な贈り物
    BOSS_APP_TYPE_ID_MAX_gift                 = 19999,    //不思議な贈り物
    BOSS_APP_TYPE_ID_MIN_giftSerial           = 20000,    //不思議な贈り物（シリアル用）
    BOSS_APP_TYPE_ID_MAX_giftSerial           = 29999,    //不思議な贈り物（シリアル用）
    BOSS_APP_TYPE_ID_MIN_regulation           = 30000,    //レギュレーション
    BOSS_APP_TYPE_ID_MAX_regulation           = 39999,    //レギュレーション
    BOSS_APP_TYPE_ID_MIN_regulationRate       = 70000,    //レギュレーション（レート）
    BOSS_APP_TYPE_ID_MAX_regulationRate       = 79999,    //レギュレーション（レート）
    BOSS_APP_TYPE_ID_MIN_cup                  = 40000,    //大会選手証
    BOSS_APP_TYPE_ID_MAX_cup                  = 49999,    //大会選手証
    BOSS_APP_TYPE_ID_MIN_regulationDownload   = 50000,    //配信レギュレーション
    BOSS_APP_TYPE_ID_MAX_regulationDownload   = 59999,    //配信レギュレーション
    BOSS_APP_TYPE_ID_MIN_globalAttraction     = 60000,    //グローバルアトラクション
    BOSS_APP_TYPE_ID_MAX_globalAttraction     = 69999,    //グローバルアトラクション

  };

private:
  //--------------------------------------------------------------------------------------------
  /**
  * @brief		コンストラクタ(隠蔽化)
  */
  //--------------------------------------------------------------------------------------------
  BossUtility( void );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief		デストラクタ(隠蔽化)
  */
  //--------------------------------------------------------------------------------------------
  ~BossUtility(void);

public:


  //--------------------------------------------------------------------------------------------
  /**
  * @brief    BOSS用属性値文字列の作成
  */
  //--------------------------------------------------------------------------------------------
  static void CreateXYBossAttribute1( BOSS_APP_TYPE appType , char* pOutput );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    BOSS用属性値文字列の作成
  */
  //--------------------------------------------------------------------------------------------
  //static void CreateXYBossAttribute2( BOSS_APP_TYPE appType , char* pOutput );

  //--------------------------------------------------------------------------------------------
  /**
  * @brief    BOSS用属性値文字列の作成
  */
  //--------------------------------------------------------------------------------------------
  static void CreateXYBossAttribute3( BOSS_APP_TYPE appType , char* pOutput );

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最大）
  */
  //------------------------------------------------------------------
  static u32 GetSerialIdMax( BOSS_APP_TYPE appType );

  //------------------------------------------------------------------
  /**
  * @brief シリアルIDの範囲取得（最少）
  */
  //------------------------------------------------------------------
  static u32 GetSerialIdMin( BOSS_APP_TYPE appType );
  //------------------------------------------------------------------
  /**
  * @brief GlobalAttractionファイル名取得
  */
  //------------------------------------------------------------------
  static const char* GetGlobalAttractionFileName( void );

  //------------------------------------------------------------------
  /**
  * @brief フリーレギュレーション　ファイル名取得
  */
  //------------------------------------------------------------------
  static const char* GetFreeRagulationFileName( void );

  //------------------------------------------------------------------
  /**
  * @brief レートレギュレーション　ファイル名取得
  */
  //------------------------------------------------------------------
  static const char* GetRateRegulationFileName( void );

  //------------------------------------------------------------------
  /**
  * @brief インターネット大会　ファイル名取得
  */
  //------------------------------------------------------------------
  static const char* GetInternetMatchFileName( void );

  //------------------------------------------------------------------
  /**
  * @brief リージョン用URL取得
  */
  //------------------------------------------------------------------
  static const char* GetRegionURL( void );

  //------------------------------------------------------------------
  /**
  * @brief ファイルインデックス取得
  */
  //------------------------------------------------------------------
  static u32 GetFileNameIndex( void );

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)



#endif // NETAPP_BOSS_UTILITY_H_INCLUDE

#endif // defined( GF_PLATFORM_CTR )
