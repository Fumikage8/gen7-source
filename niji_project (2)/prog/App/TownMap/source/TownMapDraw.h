//=============================================================================
/**
 * @file    TownMapDraw.h
 * @brief   タウンマップ画面描画処理
 * @author  Hiroyuki Nakamura
 * @date    2015.10.13
 */
//=============================================================================
#if !defined( TOWNMAP_DRAW_H_INCLUDED )
#define TOWNMAP_DRAW_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
// niji
#include "AppLib/include/Ui/UIView.h"
#include "AppLib/include/Ui/UIInputListener.h"
// module
#include "App/TownMap/include/TownMapDef.h"
// resource
#include "niji_conv_header/field/zone/zone_id.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class EventWork;
class ZoneDataLoader;
GFL_NAMESPACE_END(Field)


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(TownMap)

//=============================================================================
/**
 * @class TownMapDraw
 * @brief タウンマップ画面描画クラス
 * @date  2015.10.02
 */
//=============================================================================
class TownMapDraw
  : public app::ui::UIView
  , public app::ui::UIInputListener
{
private:
  //! レイアウトID
  enum LyoutID
  {
    LYTID_MAP = 0,        //!< 上画面マップ
    LYTID_INFO,           //!< 上画面情報
    LYTID_UPPER_ROTOM,    //!< 上画面ロトム演出
    LYTID_LOWER_ROTOM,    //!< 下画面ロトム演出
    LYTID_MAX             //!< 最大数
  };

  static const u32 INFO_PUT_FRAME = 10;   //!< 上画面情報表示開始フレーム

  //! 島ごとのデータ
  struct LAND_DATA
  {
    ZoneID zone_id[LAND_ZONE_DATA_MAX];   //!< ゾーンIDテーブル
  };

  static const LAND_DATA LandData[LAND_DATA_MAX];   //!< 島ごとのデータ

  static const u32 EVENT_NONE = 0xffff;   //!< イベント表示なし


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   *
   * @param   heap        ヒープ
   * @param   render_man  描画マネージャ
   */
  //-----------------------------------------------------------------------------
  TownMapDraw( app::util::Heap * heap, app::util::AppRenderingManager * render_man );

  //-----------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~TownMapDraw();

  //-----------------------------------------------------------------------------
  /**
   * @func    SetTownmapData
   * @brief   タウンマップデータセット
   * @date    2015.10.29
   *
   * @param   zone      ゾーンデータリソース
   * @param   zone_len  ゾーンリソース長
   * @param   icon      アイコンデータリソース
   * @param   icon_len  アイコンリソース長
   */
  //-----------------------------------------------------------------------------
  void SetTownmapData( ZONE_RESOURCE * zone, u32 zone_len, ICON_RESOURCE * icon, u32 icon_len );

  //-----------------------------------------------------------------------------
  /**
   * @func    Init
   * @brief   初期化処理
   * @date    2015.10.13
   *
   * @param   res_buf       レイアウトリソースバッファ
   * @param   rotom_buf     ロトムレイアウトリソースバッファ
   * @param   msg_buf       メッセージリソースバッファ
   * @param   wset_loader   WordSetLoader
   */
  //-----------------------------------------------------------------------------
  void Init( void * res_buf, void * rotom_buf, void * msg_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    End
   * @brief   終了処理
   * @date    2015.10.13
   *
   * @retval  true  = 処理完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool End(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Start
   * @brief   開始処理
   * @date    2015.10.13
   *
   * @param   land_no   島番号
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool Start( u32 land_no );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsStart
   * @brief   開始処理が終了したかをチェック
   * @date    2015.10.14
   *
   * @retval  true  = 完了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeLand
   * @brief   島選択アニメを変更
   * @date    2015.10.22
   *
   * @param   land_no   島番号
   */
  //-----------------------------------------------------------------------------
  void ChangeLand( u32 land_no );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetWarpPointMessage
   * @brief   ワープポイントのメッセージを取得
   * @date    2015.10.29
   *
   * @param   id              文字列ID
   * @param   is_place_name   1 = place_name.mstxtから取得
   *
   * @return  文字列
   */
  //-----------------------------------------------------------------------------
  gfl2::str::StrBuf * GetWarpPointMessage( s32 id, u32 is_place_name );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutZoneInfo
   * @brief   ゾーン情報表示
   * @date    2015.10.29
   *
   * @param   zone_id   ゾーンID
   */
  //-----------------------------------------------------------------------------
  void PutZoneInfo( ZoneID zone_id );

  //-----------------------------------------------------------------------------
  /**
   * @func    PutEventInfo
   * @brief   イベント情報表示
   * @date    2015.11.03
   *
   * @param   title   タイトルのメッセージ番号
   * @param   info    情報のメッセージ番号
   *
   * @note  titleとinfoがEVENT_NONEの場合は非表示
   */
  //-----------------------------------------------------------------------------
  void PutEventInfo( u16 title = EVENT_NONE, u16 info = EVENT_NONE );

  //-----------------------------------------------------------------------------
  /**
   * @func    StartRotom
   * @brief   ロトムアニメ開始
   * @date    2016.01.12
   */
  //-----------------------------------------------------------------------------
  void StartRotom(void);


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetGameData
   * @brief   GameData関連取得
   * @date    2015.10.30
   */
  //-----------------------------------------------------------------------------
  void GetGameData(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Update
   * @brief   更新
   * @date    2015.10.13
   */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    Draw
   * @brief   描画
   * @date    2015.10.13
   *
   * @param   displayNo   描画面
   */
  //-----------------------------------------------------------------------------
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  //-----------------------------------------------------------------------------
  /**
   * @func    Initialize2D
   * @brief   2D関連初期化
   * @date    2015.10.13
   *
   * @param   res_buf       レイアウトリソースバッファ
   * @param   rotom_buf     ロトムレイアウトリソースバッファ
   * @param   msg_buf       メッセージリソースバッファ
   * @param   wset_loader   WordSetLoader
   */
  //-----------------------------------------------------------------------------
  void Initialize2D( void * res_buf, void * rotom_buf, void * msg_buf, WordSetLoader * wset_loader );

  //-----------------------------------------------------------------------------
  /**
   * @func    Terminate2D
   * @brief   2D関連削除
   * @date    2015.10.13
   */
  //-----------------------------------------------------------------------------
  void Terminate2D(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteDefaultSrring
   * @brief   デフォルト文字列描画
   * @date    2015.10.22
   */
  //-----------------------------------------------------------------------------
  void WriteDefaultString(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    WriteZoneInfo
   * @brief   ゾーン情報書き込み
   * @date    2015.11.02
   */
  //-----------------------------------------------------------------------------
  void WriteZoneInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    VanishZoneInfo
   * @brief   ゾーン情報の表示を消す
   * @date    2015.11.03
   */
  //-----------------------------------------------------------------------------
  void VanishZoneInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    PutEventInfo
   * @brief   イベント情報書き込み
   * @date    2015.11.03
   */
  //-----------------------------------------------------------------------------
//  void WriteEventInfo(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotom
   * @brief   ロトムアニメ終了チェック
   * @date    2016.01.12
   *
   * @retval  true  = 終了
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotom(void);


private:
  app::util::Heap * m_heap;                             //!< ヒープ
  app::util::AppRenderingManager * m_renderingManager;  //!< 描画マネージャ

  const Field::EventWork * m_eventWork;             //!< イベントワーク
  const Field::ZoneDataLoader * m_zoneDataLoader;   //!< ゾーンデータローダー

  ZONE_RESOURCE * m_resZoneData;    //!< タウンマップデータ：ゾーン
  u32 m_resZoneDataLen;             //!< ゾーンデータ数

  ICON_RESOURCE * m_resIconData;    //!< タウンマップデータ：アイコン
  u32 m_resIconDataLen;             //!< アイコンデータ数

  gfl2::str::MsgData * m_pMsgData;  //!< メッセージデータ
  print::WordSet * m_pWordSet;      //!< 単語セット

  ZoneID m_infoZoneID;    //!< 表示している情報のゾーンID
  u32 m_infoPutCount;     //!< 情報表示開始フレーム

  bool m_isInfoChange;    //!< 情報変更フラグ

  u16 m_eventTitle;       //!< イベントタイトルメッセージID
  u16 m_eventInfo;        //!< イベント情報メッセージID
  bool m_isEventChange;   //!< イベント変更フラグ

  u32 m_subSeq;         //!< サブシーケンス
  bool m_isDrawEnable;  //!< 描画フラグ
};


GFL_NAMESPACE_END(TownMap)
GFL_NAMESPACE_END(App)

#endif  // TOWNMAP_DRAW_H_INCLUDED
