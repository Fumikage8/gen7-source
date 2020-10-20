//======================================================================
/**
 * @file PokeFinderSave.h
 * @date 2015/11/30 18:09:00
 * @author miyachi_soichi
 * @brief ポケファインダーデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __POKE_FINDER_SAVE_H_INCLUDED__
#define __POKE_FINDER_SAVE_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <system/include/Date/gfl2_Date.h>
// savedata
#include <Savedata/include/SaveDataInterface.h>

GFL_NAMESPACE_BEGIN(Savedata)

/**
 *  @brief  ポケファインダー用セーブデータ
 */
class PokeFinderSave : public SaveDataInterface
{
public:
  enum
  {
    MAX_POKE_FINDER_SPOT = 60,            //!< ポケファインダーのスポット最大数
    POKE_FINDER_GYRO_DISABLE = 0,         //!< ジャイロ無効
    POKE_FINDER_GYRO_ENABLE = 1,          //!< ジャイロ有効
    MAX_POKE_FINDER_TOTALVALUE = 9999999, //!< 有効桁数7ケタ
    MAX_POKE_FINDER_LEVEL = 4,            //!< 最大解放段階
  };
  /**
   *  @brief  チュートリアル用フラグ
   */
  enum FinderTutorialFlag
  {
    FINDER_TUTORIAL_CLEAR   = 0,    //!< クリア用
    FINDER_TUTORIAL_STARTUP = 0x01 << 0,  //!< チュートリアル：ファインダー説明
    FINDER_TUTORIAL_SELECT  = 0x01 << 1,  //!< チュートリアル：選択画面説明
    FINDER_TUTORIAL_SCORE   = 0x01 << 2,  //!< チュートリアル：得点画面説明
    FINDER_TUTORIAL_ZOOM1   = 0x01 << 3,  //!< チュートリアル：ズーム１
    FINDER_TUTORIAL_CALL    = 0x01 << 4,  //!< チュートリアル：ポケモンを呼ぶ
    FINDER_TUTORIAL_ZOOM2   = 0x01 << 5,  //!< チュートリアル：ズーム２
    FINDER_TUTORIAL_ZOOM3   = 0x01 << 6,  //!< チュートリアル：ズーム３
  };
  /**
   *  @brief  スポット毎の保存する属性
   */
  enum FinderSpotFlag
  {
    FINDERSPOT_FLAG_CLEAR   = 0,          //!< クリア用
    FINDERSPOT_FLAG_KNOWN   = 0x01 << 0,  //!< スポットが既知
  };
  /**
   *  @brief  各スポット毎のデータ
   */
  struct PokeFinderSpotData
  {
    u32 flags;                      //!< スポットで保持する属性
    u16 reserve[22];                 // 予備枠
  };
  /**
   *  @brief  ファインダー全体で保存するデータ
   */
  struct PokeFinderData
  {
    u16 finder_level; //!< カメラアプリのバージョン
    u16 gyro_enable;  //!< ジャイロ有効/無効
    u32 snap_count;   //!< 撮影した総枚数
    u32 value_count;  //!< 評価した総枚数
    u32 total_value;  //!< 総評価得点
    u32 high_value;   //!< 最高評価得点
    u16 tutorial_flag;//!< チュートリアル状況
    u16 reserve[5];   // 予備枠
    
    PokeFinderSpotData spot_data[MAX_POKE_FINDER_SPOT];
    
  };

  // SaveDataInterface 実装 ここから
  /**
   *  @brief  読込んだデータをセットする関数
   *  @param  pData  先頭のポインタ
   */
  void SetData( void* pData );

  /**
   *  @brief  データの先頭ポインタを得る関数
   *  @return 先頭のポインタ
   */
  void* GetData( void );

  /**
   *  @brief  データサイズ
   *  @return バイト
   */
  size_t GetDataSize( void );

  /**
   *  @brief  セーブデータのクリア
   *  @param  pHeap ヒープ
   */
  void Clear( gfl2::heap::HeapBase *pHeap );
  // SaveDataInterface 実装 ここまで

  /**
   *  @brief  データの初期化
   */
  void InitData( void );
  /**
   *  @brief  カメラアプリのバージョンを変更
   *  @param  ver   設定するバージョンの段階
   */
  void SetCameraVersion( u16 ver );
  /**
   *  @brief  カメラアプリのバージョンを取得
   *  @return バージョンの段階を取得
   */
  u16 GetCameraVersion( void );
  /**
   *  @brief  ジャイロ状態切替
   *  @param  flag  true:On/false:Off
   */
  void SetGyroFlag( bool flag );
  /**
   *  @brief  ジャイロ状態取得
   *  @retval true:on
   *  @retval false:off
   */
  bool GetGyroFlag( void );
  /**
   *  @brief  撮影回数の加算
   *  @param  add   加算回数
   */
  void AddSnapCount( u32 add );
  /**
   *  @brief  撮影回数の取得
   *  @return 撮影回数
   */
  u32 GetSnapCount( void );
  /**
   *  @brief  評価回数の加算
   *  @param  add   加算回数
   */
  void AddValueCount( u32 add );
  /**
   *  @brief  評価回数の取得
   *  @return  評価回数
   */
  u32 GetValueCount( void );
  /**
   *  @brief  総評価得点の加算
   *  @param  add   加算する得点
   */
  void AddTotalValue( u32 add );
  /**
   *  @brief  総評価得点の取得
   *  @return 総評価得点
   */
  u32 GetTotalValue( void );
  /**
   *  @brief  最高評価点の設定
   *  @param  val 設定値
   */
  void SetHighValue( u32 val );
  /**
   *  @brief  最高評価点の取得
   */
  u32 GetHighValue( void );
  /**
   *  @brief  チュートリアル状況の設定
   *  @param  flag    操作するフラグ
   *  @param  enable  操作
   */
  void SetTutorialFlags( u16 flag, bool enable );
  /**
   *  @brief  チュートリアル状況の取得
   *  @return 状態を返却
   */
  u16 GetTutorialFlags( void );
  /**
   *  @brief  指定スポットの状態を操作
   *  @param  spot_id   スポットID
   *  @param  flag      操作するフラグ
   *  @param  enable    操作
   */
  void SetSpotFlags( u32 spot_id, u32 flag, bool enable );
  /**
   *  @brief  指定スポットの状態を取得
   *  @param  spot_id   スポットID
   *  @return 状態フラグ
   */
  u32 GetSpotFlags( u32 spot_id );
private:
  PokeFinderData data;

}; // class PokeFinderSave : public SaveDataInterface

GFL_NAMESPACE_END(Savedata)

#endif // __POKE_FINDER_SAVE_H_INCLUDED__
