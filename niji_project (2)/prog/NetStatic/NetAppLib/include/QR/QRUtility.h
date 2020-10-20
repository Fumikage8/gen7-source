//======================================================================
/**
 * @file QRUtility.h
 * @brief QR系ユーティリティ
 */
//======================================================================

#if !defined __QR_UTILIRY_H_INCLUDED__
#define __QR_UTILIRY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <NetStatic/NetAppLib/include/QR/QRDefine.h>
#include "Battle/Regulation/include/Regulation.h" // バトルルール
#include <pml/include/pmlib.h>

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(QR)


//QRアプリ用
enum QR_TYPE_APP
{
  QR_TYPE_APP_ALOLA_NEW           = 0,  //アローラ新規
  QR_TYPE_APP_ALOLA_NEW_OFFICIAL,       //アローラ新規（20％チャージ）
  QR_TYPE_APP_ALOLA_UPDATE,             //アローラ更新
  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL,    //アローラ更新（20％チャージ）
  /*  マギアナ関連成功  */
  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_MAGIANA,                   /**<  マギアナOK: 図鑑未登録/マギアナイベント終了前  */
  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA,                /**<  マギアナOK: 図鑑登録済/マギアナイベント終了前  */
  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_MAGIANA_AFTER_EVENT,    /**<  マギアナOK: 図鑑登録済/マギアナイベント終了後  */
  /*  M20ピカチュウ関連  */
  QR_TYPE_APP_ALOLA_NEW_OFFICIAL_M20PIKATYU,                /**<  M20ピカチュウOK: 図鑑未登録/M20ピカチュウイベント終了前  */
  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU,             /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了前  */
  QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL_M20PIKATYU_AFTER_EVENT, /**<  M20ピカチュウOK: 図鑑登録済/M20ピカチュウイベント終了後  */

  QR_TYPE_APP_OFFICIAL,                 //アローラ外（公式QR）
  QR_TYPE_APP_NORMAL,                   //アローラ外（一般QR）
  QR_TYPE_APP_ROM_NG,                   //ROMコードがNG
  QR_TYPE_APP_REGION_NG,                //リージョンがNG
  QR_TYPE_APP_MAGIANA_NG,               //マギアナQRまだ読めない
  QR_TYPE_APP_M20PIKATYU_NG,            //M20PikatyuQRまだ読めない

  QR_TYPE_APP_INVALID
};

//バトルチームモード用
enum QR_TYPE_BATTLE_TEAM
{
  QR_TYPE_BATTLE_TEAM_OK = 0,
  QR_TYPE_BATTLE_TEAM_FRIEND_NG,  //仲間大会用なのでNG
  QR_TYPE_BATTLE_TEAM_LIVE_NG,    //ライブ大会用なのでNG
  QR_TYPE_BATTLE_TEAM_ZUKAN_NG,   //図鑑系なのでNG
  QR_TYPE_BATTLE_TEAM_ROM_NG,     //ROMコードがNG
  QR_TYPE_BATTLE_TEAM_REGION_NG,  //リージョンがNG
  QR_TYPE_BATTLE_TEAM_GAMESYNC_ID_NG, //ゲームシンクID一致せず


  QR_TYPE_BATTLE_TEAM_INVALID
};

//仲間大会モード用
enum QR_TYPE_FRIEND_CUP
{
  QR_TYPE_FRIEND_CUP_OK = 0,
  QR_TYPE_FRIEND_CUP_LIVE_NG,         //ライブ大会用なのでNG
  QR_TYPE_FRIEND_CUP_BATTLE_TEAM_NG,  //バトルチーム用なのでNG
  QR_TYPE_FRIEND_CUP_ZUKAN_NG,        //図鑑系なのでNG
  QR_TYPE_FRIEND_CUP_ROM_NG,          //ROMコードがNG
  QR_TYPE_FRIEND_CUP_REGION_NG,       //リージョンがNG
  QR_TYPE_FRIEND_CUP_GAMESYNC_ID_NG,   //ゲームシンクID一致せず

  QR_TYPE_FRIEND_CUP_INVALID
};

//ライブ大会モード用
enum QR_TYPE_LIVE_CUP
{
  QR_TYPE_LIVE_CUP_OK = 0,
  QR_TYPE_LIVE_CUP_FRIEND_CUP_NG,   //仲間大会用なのでNG
  QR_TYPE_LIVE_CUP_BATTLE_TEAM_NG,  //バトルチーム用なのでNG
  QR_TYPE_LIVE_CUP_ZUKAN_NG,        //図鑑系なのでNG
  QR_TYPE_LIVE_CUP_ROM_NG,          //ROMコードがNG
  QR_TYPE_LIVE_CUP_REGION_NG,       //リージョンがNG

  QR_TYPE_LIVE_CUP_INVALID
};

class QRUtility
{
public:

  enum
  {
    QR_POKE_TABLE_NUM = 542,            /**<  一般QR抽選テーブル：エントリ数                    */
    QR_POKE_TABLE_ELEM = 4,             /**<  一般QR抽選テーブル：エントリあたりの要素数        */
    QR_POKE_TABLE_TOTAL = 1942920,      /**<  一般QR抽選テーブル：重み付けパラメータの合計値    */
    QR_POKE_TABLE_MONSNO = 0,           /**<  一般QR抽選テーブル：monsNo要素へのアクセスIndex   */
    QR_POKE_TABLE_SEX = 1,              /**<  一般QR抽選テーブル：性別要素へのアクセスIndex     */
    QR_POKE_TABLE_FORMNO = 2,           /**<  一般QR抽選テーブル：FormNo要素へのアクセスIndex   */
    QR_POKE_TABLE_HIT_VALUE = 3,        /**<  一般QR抽選テーブル：重み付け要素へのアクセスIndex */
    ZUKAN_CHECK_SIZE = 23,              /**<  図鑑QR読み取り時、ネタバレ防止の為強制的に一般QR化するMonsNoのテーブル要素数 */
    AES_CRYPT_SIZE = 462 + 16//16はIV   /**<  暗号化バトルチームデータサイズ                    */
  };

public:

  enum QR_CRYPTO_TYPE
  {
    QR_CRYPTO_TYPE_0 = 0,  //  ０：仲間大会ＱＲ（ネット大会用） 
    QR_CRYPTO_TYPE_1,      //　１：仲間大会ＱＲ（ライブ大会用） 
    QR_CRYPTO_TYPE_2,      //　２：ＱＲバトルチーム 
    QR_CRYPTO_TYPE_3,      //　３：図鑑ＱＲ 
    QR_CRYPTO_TYPE_4,      //　４：ZOO
    QR_CRYPTO_TYPE_5,      //　５：ZOO映画連動枠
    QR_CRYPTO_TYPE_6,      //　６：国内ライセンシー商品枠
    QR_CRYPTO_TYPE_7,      //　７：他ライセンシー枠予備1
    QR_CRYPTO_TYPE_8,      //　８：他ライセンシー枠予備2
    QR_CRYPTO_TYPE_9,      //　９：momiji M20Pika
    QR_CRYPTO_TYPE_10,     //１０：他ライセンシー枠予備4
    QR_CRYPTO_TYPE_11,     //１１：他ライセンシー枠予備5
    QR_CRYPTO_TYPE_12,     //１２：他ライセンシー枠予備6
    QR_CRYPTO_TYPE_NONE,    // 該当無し
    QR_CRYPTO_TYPE_REG = QR_CRYPTO_TYPE_NONE
  };



private:
  QRUtility(){};
  ~QRUtility(){};

public:
  //------------------------------------------------------------------
  /**
   * @brief 図鑑用QRのセットアップ
   */
  //------------------------------------------------------------------
  static void SetUpZukanQRData( gfl2::heap::HeapBase* pHeap , QR_ZUKAN* pZukanQR );

  //------------------------------------------------------------------
  /**
   * @brief QRから読み取ったバイナリ解析（QRアプリ用）
   * @param [out] rIsSameFormBothGender 雌雄で見た目の変化がないポケモンの場合はtrue/それ以外はfalse
   */
  //------------------------------------------------------------------
  static QR_TYPE_APP AnalyzeQRBinaryForApp( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::pokepara::CoreParam* pOutput, bool& rIsSameFormBothGender );

  //------------------------------------------------------------------
  /**
   * @brief QRから読み取ったバイナリ解析（バトルチーム選択モード用）
   */
  //------------------------------------------------------------------
  static QR_TYPE_BATTLE_TEAM AnalyzeQRBinaryForBattleTeam( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::PokeParty* pOutput );

  //------------------------------------------------------------------
  /**
   * @brief QRから読み取ったバイナリ解析（なかま大会モード用）
   */
  //------------------------------------------------------------------
  static QR_TYPE_FRIEND_CUP AnalyzeQRBinaryForFriendCup( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , Regulation* pOutput );

  //------------------------------------------------------------------
  /**
   * @brief QRから読み取ったバイナリ解析（ライブ大会モード用）
   */
  //------------------------------------------------------------------
  static QR_TYPE_LIVE_CUP AnalyzeQRBinaryForLiveCup( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , Regulation* pOutput );

  //------------------------------------------------------------------
  /**
    * @brief 署名付加
    */
  //------------------------------------------------------------------
  static void SignBinary( void* signature , const void* message , u32 messageLen , gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
    * @brief 署名検証
    */
  //------------------------------------------------------------------
  static bool VerifyBinary( const void* signature , u32 signatureLen , void* recoveredMessage , gfl2::heap::HeapBase* pHeap , QRUtility::QR_CRYPTO_TYPE type = QRUtility::QR_CRYPTO_TYPE_3 );

private:
  //------------------------------------------------------------------
  /**
    *  @brief    
    */
  //------------------------------------------------------------------
  static u32  GetRomBit(const u32 romVersion);


  //図鑑範囲外チェック
  static bool CheckZukanQR( QR_ZUKAN* pZukan );
  //Zoo範囲外チェック
  static bool CheckZoo( QR_ZOO* pZoo );

  //------------------------------------------------------------------
  /**
    *  @brief    QR_ZOOからPokeparaを生成する
    *  @retval   QR_TYPE_APP_ALOLA_NEW_OFFICIAL     : 公式新規
    *  @retval   QR_TYPE_APP_ALOLA_UPDATE_OFFICIAL  : 公式既視
    *  @retval   QR_TYPE_APP_OFFICIAL               : 公式図鑑外
    */
  //------------------------------------------------------------------
  static QR_TYPE_APP CreatePoke_FromZooData(pml::pokepara::CoreParam* pOutput, gfl2::heap::HeapBase* const pHeapBase, const QR_ZOO* const pZooData);

  //------------------------------------------------------------------
  /**
    * @brief 図鑑QR用処理
    */
  //------------------------------------------------------------------
  static QR_TYPE_APP ZukanQRFunc( QR_ZUKAN* pZukanQR , pml::pokepara::CoreParam* pOutput , gfl2::heap::HeapBase* pHeap );

  //------------------------------------------------------------------
  /**
    * @brief QRタイプ判別
    */
  //------------------------------------------------------------------
  static QRUtility::QR_CRYPTO_TYPE CheckQRType( const void* pBinary , u32 size );

  //------------------------------------------------------------------
  /**
    * @brief フッターチェック
    */
  //------------------------------------------------------------------
  static QRUtility::QR_CRYPTO_TYPE CheckFooter( const void* pBinary , u32 size );

  //------------------------------------------------------------------
  /**
    * @brief 一般QRポケモン生成
    */
  //------------------------------------------------------------------
  static void CreateNormalQRPoke( gfl2::heap::HeapBase* pHeap , const void* pBinary , u32 size , pml::pokepara::CoreParam* pOutput );

  //------------------------------------------------------------------
  /**
    * @brief バトルチームQR用複合化
    */
  //------------------------------------------------------------------
  static void DecryptAesForBattleTeamQR( const void* cryptMessage , u32 cryptMessageSize , void* recoveredMessage );

};

GFL_NAMESPACE_END(QR)
GFL_NAMESPACE_END(NetApp)

#endif // __QR_UTILIRY_H_INCLUDED__
