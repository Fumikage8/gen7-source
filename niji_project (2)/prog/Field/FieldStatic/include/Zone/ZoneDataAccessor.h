//===================================================================
/**
 * @file    ZoneDataAccessor.h
 * @brief   ゾーンデータのアクセサー
 * @author  saita_kazuki
 * @date    2015.04.01
 */
//===================================================================

#if !defined( ZONE_DATA_ACCESSOR_H_INCLUDED )
#define ZONE_DATA_ACCESSOR_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector3.h>

#include "niji_conv_header/field/zone/zone_id.h"
#include "niji_conv_header/field/FieldRottomEffectType.h"


GFL_NAMESPACE_BEGIN( Field )

struct ZONE_DATA;

class ZoneDataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN(ZoneDataAccessor); //コピーコンストラクタ＋代入禁止

public:

  /**
   * @brief   熱いゾーンかどうか
   */
  static bool IsWarmZone(ZoneID zoneID)
  {
    if( zoneID == Z_F0301D0101_01 
     || zoneID == Z_F0301D0101_02 
     || zoneID == Z_F0301D0101_03
     || zoneID == Z_F0301D0101_04
     || zoneID == Z_F0301D0102_01
     || zoneID == Z_F0301D0102_02
     || zoneID == Z_F0301D0102_03
     || zoneID == Z_F0301D0102_04
     || zoneID == Z_F0201H0201
     || zoneID == Z_F0201H0202 )
    {
      return true;
    }
    return false;
  }

  /**
   * @brief   寒いゾーンかどうか
   */
  static bool IsCoolZone(ZoneID zoneID)
  {
    if( zoneID == Z_F0301D1003 
     || zoneID == Z_F0301D1004 
     || zoneID == Z_F0301D1201
     //@fix MMCat[422]:こおりの洞窟 チャンピオンロード洞窟マップ２にて自機が寒い場所アイドリングモーションを取らない >> 対象ゾーンの追加
     //↓ここから momoji追加
     || zoneID == Z_F0102D0502 //海繋ぎの洞穴
     || zoneID == Z_F0301D1001 //ラナキラマウンテン：山麓
     || zoneID == Z_F0301D1002 //ラナキラマウンテン：山腹
     || zoneID == Z_F0301D1005 //チャンピオンロード外マップ
     || zoneID == Z_F0301D1202 //こおりの洞窟：チャンピオンロード洞窟マップ１
     || zoneID == Z_F0301D1203 //こおりの洞窟：チャンピオンロード洞窟マップ２
     //↑ここまで
     )
    {
      return true;
    }
    return false;
  }

  /**
   * @brief   保護区ゾーンかどうか
   */
  static bool IsSanctuaryZone( ZoneID zoneID)
  {
    if( zoneID == Z_F0501I0201 )
    {
      return true;
    }
    return false;
  }

	/**
	* @brief   保護区ゾーンかどうか
	*/
	static bool IsJoinFestaZone(ZoneID zoneID)
	{
		if (zoneID == Z_F0601JF0101
			|| zoneID == Z_F0601JF0102
			|| zoneID == Z_F0601JF0103)
		{
			return true;
		}
		return false;
	}

	/**
	* @brief   暗黒天候になる可能性のあるゾーンかどうか(下画面ロトム用のため、実際の暗黒天候の場所とは一致しません)
	*/
  static bool IsDarkWeatherZoneForRotom( ZoneID zoneID )
  {
    if( zoneID == Z_F0401_S0101
     || zoneID == Z_F0401_S0201
     || zoneID == Z_F0401_S0301
     || zoneID == Z_F0401_T0401
     || zoneID == Z_F0401D0501
     || zoneID == Z_F0401D0201
     || zoneID == Z_F0401D0202
     || zoneID == Z_F0401D0301
     || zoneID == Z_F0401D0302
     || zoneID == Z_F0401D0401
     || zoneID == Z_F0401D0402
     || zoneID == Z_F0401_S0401
     || zoneID == Z_F0401_S0801 )
    {
      return true;
    }
    return false;
  }

  /**
  * @brief   暗黒天候になるゾーンかどうか
  *　@fix MMCat[379]:天候「暗黒」時のポニのビーチ、ナッシー・アイランドの状態について >>　Mcatコメント内有るaの案件対処
  */
  static bool IsDarkWeatherZone(ZoneID zoneID)
  {
    if (zoneID == Z_F0401_S0101
      || zoneID == Z_F0401_S0201
      || zoneID == Z_F0401_S0301
      || zoneID == Z_F0401_T0401
      || zoneID == Z_F0401D0501
      || zoneID == Z_F0401D0201
      || zoneID == Z_F0401D0202
      || zoneID == Z_F0401D0203
      || zoneID == Z_F0401D0204
      || zoneID == Z_F0401D0205
      || zoneID == Z_F0401H0701
      || zoneID == Z_F0401D0301
      || zoneID == Z_F0401D0302
      || zoneID == Z_F0401D0401
      || zoneID == Z_F0401D0402
      || zoneID == Z_F0401PC0101
      || zoneID == Z_F0401I0201_01
      || zoneID == Z_F0401I0201_02
      || zoneID == Z_F0401R0201
      || zoneID == Z_F0401R0101
      || zoneID == Z_F0401I0101
      || zoneID == Z_F0401R0301_01
      || zoneID == Z_F0401R0301_02
      || zoneID == Z_F0401_S0401
      || zoneID == Z_F0401_S0801 )
    {
      return true;
    }
    return false;
    
    
    
  }

  /**
   * @brief   コンストラクタ
   * @param   pZoneData ZONE_DATA構造体バイナリのポインタ
   */
  ZoneDataAccessor();
  ZoneDataAccessor( void* pZoneData);

  /**
   * @brief   デストラクタ
   */
  virtual ~ZoneDataAccessor();

  /**
   * @brief   アクセスするデータを設定
   * @param   pZoneData ZONE_DATA構造体バイナリのポインタ
   * @note    コンストラクタで設定した場合は呼ばなくてもOK
   */
  void SetData( void* pZoneData);

  /**
   * @brief   データを取得できる状態か
   * @retval  true 有効
   * @retval  false 無効
   */
  bool IsEnable() const ;

  /**
   * @brief   BGM指定IDの取得
   */
  u32 GetBGMIDDay() const ;
  u32 GetBGMIDNight() const ;

  /**
   * @brief   メッセージのアーカイブIDの取得
   */
  u16 GetMessageArcID() const ;

  /**
   * @brief   タウンマップ上での表示位置グループの取得
   */
  u16 GetZoneGroup() const ;

  /**
   * @brief   ギミックIDの取得
   */
  u16 GetGimmickID() const ;

  /**
   * @brief   戦闘背景遠景IDの取得
   */
  u16 GetBattleFarBGID() const ;

  /**
   * @brief   マップ切り替え種別の取得
   */
  u8 GetMapChangeType() const ;

  /**
   * @brief   地名メッセージIDの取得
   */
  u16 GetPlaceLabelID() const ;

  /**
   * brief  天候用environmentデータインデックスを取得
   * @return データインデックス
   */
  u8 GetWeatherEnvIndex() const;

  /**
   * @brief   マップ開始位置の取得
   */
  void GetStartPosition( gfl2::math::Vector3* pOutPos) const ;

  /**
   * @brief   走るポケライドができるか
   */
  bool IsPokeRideRunEnable() const ;

  /**
   * @brief   そらをとぶポケライドができるか
   */
  bool IsPokeRideFlyEnable() const ;

  /**
   * @brief   自転車でBGMが変わるか
   */
  bool IsBGMChangeEnable() const ;

  /**
   * @brief   あなぬけができるか
   */
  bool IsEscapeEnable() const ;

  /**
   * @brief   天球が有効なゾーンか
   */
  bool IsSkyboxEnable() const ;

  /**
   * @brief   リバーブが有効か
   */
  bool IsReverbEnable() const ;

  /**
   * @brief   通信機能が使えるか
   */
  bool IsCommunicationEnable() const ;

  /**
   * @brief   脱出先を保存するか
   */
  bool IsEscapeSaveEnable() const ;

  /**
   * @brief   下画面が外マップ表示モードか
   * @retval true 外マップ
   * @retval false ダンジョン/室内
   */
  bool IsSubscreenOutsideMapMode() const ;

  /**
   * @brief   下画面をスリープにするか
   * @retval true する
   * @retval false しない
   */
  bool IsSubscreenSleep() const ;

  /**
   * @brief   下画面座標設定がタウンマップ用か
   * @retval true タウンマップ用
   * @retval false それ以外
   */
  bool IsTownmapMode() const ;

  /**
   * @brief   動作モデルの表示数制御が有効か
   * @retval true 有効
   * @retval false 無効
   */
  bool IsMoveModelDrawNumControl() const ;

  /**
   * @brief   往来の描画優先度を下げる制御が有効か
   * @retval true 有効
   * @retval false 無効
   */
  bool IsDispLowPriorityForTraffic() const ;

  /**
   * @brief   エリアの環境データパックのどれを使うかの番号を取得
   */
  u8 GetEnvironmentIndex() const ;

  /**
  * @brief   このゾーンがデフォルトで使用するカメラデータのデータ番号を取得(デフォルトデータローカル)
  */
  u8 GetCameraDefaultIndex() const;

  /**
  * @brief   このゾーンがデフォルトで使用するカメラサポートデータのデータ番号を取得(デフォルトデータローカル)
  */
  u8 GetCameraSupportDefaultIndex() const;

  /**
  * @brief   島番号を取得
  * @return 1オリジンの島番号。島1なら1。1～５まで
  */
  u8 GetIslandIndex() const;

  /**
  * @brief  アトリビュートのバトル近景IDパターンを取得
  * @return バトル近景IDパターン
  * @note 取得したパターンはアトリビュートからバトル近景IDを取得する際に使用する
  */
  u16 GetAttributeBgNearPattern() const;

  /**
  * @brief  天候テーブル取得
  * @return 天候テーブル戦闘アドレス
  */
  u32 *GetWeatherTable() const;

  /**
  * @brief  下画面基準位置X座標の取得
  * @return 下画面基準位置X座標
  */
  s32 GetSubscreenBasisPositionX() const ;

  /**
  * @brief  下画面基準位置Z座標の取得
  * @return 下画面基準位置Z座標
  */
  s32 GetSubscreenBasisPositionZ() const ;

  /**
  * @brief  下画面マップ種類の取得
  * @return subscreen_map_type.hの値
  */
  u8 GetSubscreenMapType() const ;

  /**
  * @brief  下画面基準位置座標がどのゾーンIDかを取得
  * @return ゾーンID
  */
  u16 GetTownmapBasisPosZone() const ;

  /**
  * @brief   ポケモンセンターゾーンか？
  */
  bool IsPokemonCenter() const;

  /**
  * @brief   異世界人がゴーグルをつけるゾーンか？
  */
  bool IsChrGogglesOn() const;

  /**
  * @brief   下画面ロトム変化の種別
  *
  * @return 種別の識別子(FieldRottomEffectType.h)
  */
  u8 GetRottomEffectType() const;


#if 0
  // niji未使用
  /**
   * @brief   フラッシュが使えるか
   */
  bool IsFlashEnable() const ;

  /**
   * @brief   ダッシュできるか
   */
  bool IsDashEnable() const ;

  /**
   * @brief   自機の吐息が有効か
   */
  bool IsBreathEnable() const ;

  /**
   * @brief   ダウジングポケライドが使えるか
   */
  bool IsPokeRideDowsingEnable() const ;

  /**
   * @brief   ブルームが有効か
   */
  bool IsBloomEnable() const ;

  /**
   * @brief   立体視が有効か
   */
  bool IsStereoDrawEnable() const ;

  /**
   * @brief   イベントデータのアーカイブIDの取得
   */
  u16 GetEventDataID() const ;

  /**
   * @brief   ゾーンの風景IDの取得。
   * @note    ポケモンの記憶関連
   */
  u8 GetZoneSceneryID() const ;

  /**
   * @brief   地名メッセージウインドウ種別の取得
   */
  u8 GetPlaceNameType() const ;

  /**
   * @brief   マップ種別指定IDの取得
   */
  u8 GetMapResourceID() const ;

#endif

private:

  void*   m_pZoneData;      ///< ZONE_DATA構造体バイナリのポインタ


  ZONE_DATA* GetZoneData() const ;
};

GFL_NAMESPACE_END( Field )

#endif // ZONE_DATA_ACCESSOR_H_INCLUDED
