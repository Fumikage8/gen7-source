//=============================================================================
/**
 * @file    FieldMenuSave.h
 * @brief   フィールドメニューセーブデータ
 * @author	Hiroyuki Nakamura
 * @date		2015.05.20
 */
//=============================================================================
#if !defined( FIELD_MENU_SAVE_H_INCLUDED )
#define FIELD_MENU_SAVE_H_INCLUDED

#pragma once

// gflib2
#include <str/include/gfl2_StrBuf.h>
#include <util/include/gfl2_std_string.h>
// niji
#include "SaveDataInterface.h"
#include "System/include/GameStrLen.h"


GFL_NAMESPACE_BEGIN(Savedata)

//=============================================================================
/**
 * @class FieldMenu
 * @brief フィールドメニューセーブデータクラス
 * @date  2011.10.20
 */
//=============================================================================
class FieldMenu
  : public SaveDataInterface
{
public:
  static const u32 PAGE_MAX = 2;                              //!< ページ数
  static const u32 PAGE_ICON_MAX = 6;                         //!< ページごとのアイコン数
  static const u32 MENU_ICON_MAX = PAGE_ICON_MAX * PAGE_MAX;  //!< 全アイコン数

  //! アイコンID
  enum IconID
  {
    ICON_ID_POKE_LIST = 0,  //!< ポケモンリスト
    ICON_ID_ZUKAN,          //!< 図鑑
    ICON_ID_BAG,            //!< バッグ
    ICON_ID_ALBUM,          //!< マイアルバム
    ICON_ID_NET_PLAY,       //!< 通信で遊ぶ
    ICON_ID_JOIN_FESTA,     //!< ジョインフェスタ

    ICON_ID_TR_CARD,        //!< トレーナーカード
    ICON_ID_POKE_PLANET,    //!< ポケプラネット
    ICON_ID_POKE_PARURE,    //!< ポケパルレ
    ICON_ID_QR_READ,        //!< QR読み込み
    ICON_ID_BATTLE_VIDEO,   //!< バトルビデオ
    ICON_ID_CONFIG,         //!< コンフィグ

    ICON_ID_MAX             //!< 最大値
  };

  //! 登録キー
  enum RideKeyID
  {
    KEY_ID_UP = 0,
    KEY_ID_DOWN,
    KEY_ID_LEFT,
    KEY_ID_RIGHT,
    KEY_ID_MAX
  };

  //! ライドID
  enum RideID
  {
    RIDE_ID_RUSH = 0,   //!< ケンタロス
    RIDE_ID_SEARCH,     //!< ムーランド
    RIDE_ID_CRASH,      //!< 新ポケ
    RIDE_ID_STRINGS,    //!< カイリキー
    RIDE_ID_SONAR,      //!< ラプラス
    RIDE_ID_JET,        //!< サメハダー
    RIDE_ID_FLY,        //!< リザードン

    RIDE_ID_MAX,        //!< 最大数

    RIDE_ID_NONE = 0xff //!< 登録なし
  };

  //! ロトム親密度ランク
  enum RotomRank
  {
    ROTOM_RANK_1 = 0,
    ROTOM_RANK_2,
    ROTOM_RANK_3,
    ROTOM_RANK_4,

    ROTOM_RANK_MAX,   //!< 定義最大値 @note ランクの最大値ではない
  };

  //! ロトム性格
  enum RotomCharacter
  {
    ROTOM_CHAR_SHY = 0,       //!< 内気
    ROTOM_CHAR_BELLIGERENT,   //!< 好戦的
    ROTOM_CHAR_FRIENDLY,      //!< 人懐っこい
    ROTOM_CHAR_OPTIMISTIC,    //!< 楽天的
  };

  //! ロトム性格変更データカウンタ数
  static const u32 ROTOM_CHAR_COUNTER = 3;

  //! ロトムお願い定義
  enum RotomRequest
  {
    ROTOM_REQ_NONE = 0,       //!< なし
    ROTOM_REQ_FINDER_STUDIO,  //!< ファインダースタジオ
    ROTOM_REQ_MANTAIN_SURF,   //!< マンタインサーフ
    ROTOM_REQ_LUNASOL_DIVE,   //!< ルナソルダイブ
    ROTOM_REQ_BATTLE_TREE,    //!< バトルツリー
    ROTOM_REQ_FES_CIRCLE,     //!< フェスサークル
    ROTOM_REQ_TUTORIAL,       //!< チュートリアル
    ROTOM_REQ_EVOLUTION,      //!< 進化
    ROTOM_REQ_MAX,            //!< 定義最大値
  };

  //! 設定されているニックネーム
  enum NicknameType
  {
    NICKNAME_TYPE_NONE = 0,   //!< 00: なし
    NICKNAME_TYPE_NAME,       //!< 01: 名前
    NICKNAME_TYPE_DEFAULT,    //!< 02: ○○ポン
    NICKNAME_TYPE_MASTER,     //!< 03: マスター
  };


private:
  //! ロトム親密度データ
  struct ROTOM_FRIENDSHIP_DATA
  {
    u16 rank_limit;     //!< 親密度上限値
    u16 rotopon_count;  //!< ロトポンが発生する親密度カウント
  };

  //! ロトム親密度データ数
  static const u32 ROTOM_FRIENDSHIP_DATA_MAX = 4;
  //! ロトム親密度データ
  static const ROTOM_FRIENDSHIP_DATA RotomFriendshipData[ROTOM_FRIENDSHIP_DATA_MAX];
  //! ロトムの性格が変化する親密度のオフセット値
  static const u32 ROTOM_FRIENDSHIP_CHAR_CHG_OFFSET = 100;
  //! ロトム性格変更データカウンタ最大値
  static const u32 ROTOM_CHAR_COUNTER_MAX = 0x00ffffff;
  //! ロトムの会話が発生するカウント
  static const u32 ROTOM_TALK_START_COUNT = 4;


public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //-----------------------------------------------------------------------------
  FieldMenu(void);

  //-----------------------------------------------------------------------------
  /**
   * @brief		デストラクタ
   */
  //-----------------------------------------------------------------------------
  virtual ~FieldMenu();


private:
  //! メニューデータ
  struct MenuData
  {
/*
    u32 mode:1;                           //!< 表示モード ( Menu/Map )  @note 未使用
    u32 page:1;                           //!< メニューのページ
    u32 is_page:2;                        //!< ページ存在フラグ
    u32 icon_new:12;                      //!< 配置アイコンのNewフラグ
    u32 icon_enable:12;                   //!< アイコン存在フラグ
    u32 is_visible_icon:1;                //!< アイコンを表示するか     @note 未使用
    u32 is_visible_rotom:1;               //!< ロトムメッセージを表示するか
    u32 _dmy1:2;
    // 4byte
    u8 icon_id[PAGE_MAX][PAGE_ICON_MAX];  //!< 配置アイコンID
    // 16byte
    u16 design_no;                        //!< メニューのデザイン番号 @note 未使用
    // 18byte
    u8 ride_short_cut[KEY_ID_MAX];        //!< ライドポケモンのショートカット登録
    u8 ride_new;                          //!< ライドポケモンのNewフラグ
    // 23byte
    u8 _dmy2;
    // 24byte
    f32 map_x;    //!< 屋内にいる時の下画面表示X座標
    f32 map_y;    //!< 屋内にいる時の下画面表示Y座標
    // 32byte
*/
    f32 map_x;    //!< 屋内にいる時の下画面表示X座標
    f32 map_y;    //!< 屋内にいる時の下画面表示Y座標
    // 8byte
    u8 icon_id[PAGE_MAX][PAGE_ICON_MAX];  //!< 配置アイコンID
    // 20byte
    u8 ride_short_cut[KEY_ID_MAX];  //!< ライドポケモンのショートカット登録
    u8 ride_new;                    //!< ライドポケモンのNewフラグ
    // 25byte
    u8 rotom_character;                           //!< ロトムの性格
    u16 rotom_friendship;                         //!< ロトムの親密度
    u32 rotom_char_counter[ROTOM_CHAR_COUNTER];   //!< ロトムの性格変更用カウンタ
    u8 rotom_char_change_count;                   //!< ロトムの性格変更を発生させるカウント
    u8 rotom_request_id;                          //!< ロトムのお願い
    u8 rotom_request_clear:1;                     //!< ロトムのお願いクリアフラグ
    u8 rotom_friendship_rotopon_flag:1;           //!< 親密度でロトポンを開始するフラグ
    u8 rotom_talk_start_count:6;                  //!< ロトムの会話が発生するカウント
    u8 rotom_friendship_rotopon_count;            //!< ロトポンを発生させるカウント
    // 44byte
    u32 page:1;               //!< メニューのページ
    u32 is_page:2;            //!< ページ存在フラグ
    u32 icon_new:12;          //!< 配置アイコンのNewフラグ
    u32 icon_enable:12;       //!< アイコン存在フラグ
    u32 is_visible_rotom:1;   //!< ロトムメッセージを表示するか
    u32 nickname_type:2;      //!< 設定されているニックネーム
    u32 is_rotopon_open:1;    //!< ロトポン開放フラグ
    u32 _dmy1:1;
    // 48byte
    gfl2::str::STRCODE nickname[System::STRLEN_PLAYER_NAME+System::EOM_LEN];  //!< ニックネーム (12+1)文字*2byte
    // 74byte
    u16 _dmy2;
    // 76byte
  };

  MenuData m_menuData;  //!< メニューデータ

public:
  //-----------------------------------------------------------------------------
  /**
   * @brief   読み込んだデータをセットする関数
   * @param   pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void SetData(void* pData){ gfl2::std::MemCopy(pData, reinterpret_cast<void*>(&m_menuData), sizeof(MenuData)); };
  //-----------------------------------------------------------------------------
  /**
   * @brief   データの先頭ポインタを得る関数
   * @return  先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void* GetData(void){ return reinterpret_cast<void*>(&m_menuData); };
  //-----------------------------------------------------------------------------
  /**
   * @brief   データサイズ
   * @return  バイト
  */
  //-----------------------------------------------------------------------------
  virtual size_t GetDataSize(void){ return sizeof(m_menuData); };

  //-----------------------------------------------------------------------------
  /**
   * @func    Clear
   * @brief   セーブデータクリア処理
   * @date    2015.05.20
   *
   * @param   heap  ヒープ
   */
  //-----------------------------------------------------------------------------
  void Clear( gfl2::heap::HeapBase * heap );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuPage
   * @brief   表示中のページ番号をセット
   * @date    2015.05.20
   *
   * @param   page  ページ番号
   */
  //-----------------------------------------------------------------------------
  void SetMenuPage( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMenuPage
   * @brief   表示中のページ番号を取得
   * @date    2015.05.20
   *
   * @return  ページ番号
   */
  //-----------------------------------------------------------------------------
  u32 GetMenuPage(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMoveMenuPage
   * @brief   切り替え先のページ番号を取得
   * @date    2015.05.20
   *
   * @param   mv  切り替え方向
   *
   * @return  ページ番号
   *
   * @note  ページ番号は変更されない
   */
  //-----------------------------------------------------------------------------
  u32 GetMoveMenuPage( int mv ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuPageEnable
   * @brief   ページを有効にする
   * @date    2015.05.29
   *
   * @param   page  ページ番号
   */
  //-----------------------------------------------------------------------------
  void SetMenuPageEnable( u32 page );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMenuPageEnable
   * @brief   ページが有効かを調べる
   * @date    2015.05.29
   *
   * @param   page  ページ番号
   *
   * @retval  true  = 有効
   * @retval  false = 無効
   */
  //-----------------------------------------------------------------------------
  bool IsMenuPageEnable( u32 page ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMenuIconPos
   * @brief   アイコンのページ内の位置を取得
   * @date    2015.06.03
   *
   * @param   id  アイコンID
   *
   * @return  位置
   */
  //-----------------------------------------------------------------------------
  u32 GetMenuIconPos( IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitMenuIconPos
   * @brief   アイコンの位置を初期化
   * @date    2015.08.21
   */
  //-----------------------------------------------------------------------------
  void InitMenuIconPos(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuIconID
   * @brief   アイコンIDをセット
   * @date    2015.05.20
   *
   * @param   page  ページ番号
   * @param   pos   配置位置
   * @param   id    アイコンID
   */
  //-----------------------------------------------------------------------------
  void SetMenuIconID( u32 page, u32 pos, IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMenuIconID
   * @brief   アイコンIDをセット
   * @date    2015.05.20
   *
   * @param   id    アイコンID
   *
   * @return  セットしたページ番号
   */
  //-----------------------------------------------------------------------------
  u32 SetMenuIconID( IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetMenuIconID
   * @brief   アイコンIDを取得
   * @date    2015.05.20
   *
   * @param   page  ページ番号
   * @param   pos   配置位置
   *
   * @return  アイコンID
   */
  //-----------------------------------------------------------------------------
  IconID GetMenuIconID( u32 page, u32 pos ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    IsMenuIconEnable
   * @brief   メニューアイコンが存在するか
   * @date    2015.05.20
   *
   * @param   id  アイコンID
   *
   * @retval  true  = 存在する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsMenuIconEnable( IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeMenuIconID
   * @brief   アイコン入れ替え
   * @date    2015.05.20
   *
   * @param   page1
   * @param   pos1
   * @param   page2
   * @param   pos2  
   */
  //-----------------------------------------------------------------------------
  void ChangeMenuIconID( u32 page1, u32 pos1, u32 page2, u32 pos2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    ChangeMenuIconID
   * @brief   アイコン入れ替え
   * @date    2015.06.03
   *
   * @param   id1   アイコンID１
   * @param   id2   アイコンID１
   */
  //-----------------------------------------------------------------------------
  void ChangeMenuIconID( IconID id1, IconID id2 );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsIconNew
   * @brief   新規追加されたアイコンか
   * @date    2015.05.20
   *
   * @param   id  アイコンID
   *
   * @retval  true  = 新規
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsIconNew( IconID id ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetIconNew
   * @brief   アイコンを新規追加状態にする
   * @date    2015.05.28
   *
   * @param   id  アイコンID
   *
   * @note  初期状態がNewなので、いらないかも。
   */
  //-----------------------------------------------------------------------------
  void SetIconNew( IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetIconNew
   * @brief   アイコンの新規追加状態をOFF
   * @date    2015.05.28
   *
   * @param   id  アイコンID
   */
  //-----------------------------------------------------------------------------
  void ResetIconNew( IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRideShortcut
   * @brief   登録されたライドポケモンを取得
   * @date    2015.07.15
   *
   * @param   key   ショートカットキー
   *
   * @retval  RIDE_ID_NONE = ショートカット未登録
   * @retval  RIDE_ID_NONE != 登録されたライドポケモン
   */
  //-----------------------------------------------------------------------------
  RideID GetRideShortcut( RideKeyID key );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRideShortcut
   * @brief   ライドポケモンを登録
   * @date    2015.07.15
   *
   * @param   key ショートカットキー
   * @param   id  登録する項目ID
   */
  //-----------------------------------------------------------------------------
  void SetRideShortcut( RideKeyID key, RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRideShortcut
   * @brief   登録されたショートカットキーを取得
   * @date    2015.08.05
   *
   * @param   id  ライドポケモンID
   *
   * @retval  KEY_ID_MAX = ショートカット未登録
   * @retval  KEY_ID_MAX != 登録されたショートカットキー
   */
  //-----------------------------------------------------------------------------
  RideKeyID GetRideShortcutKey( RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ResetRideNewFlag
   * @brief   ライドポケモンのNewフラグをOFFにする
   * @date    2015.08.05
   *
   * @param   id  ライドポケモンID
   */
  //-----------------------------------------------------------------------------
  void ResetRideNewFlag( RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRideNewFlag
   * @brief   ライドポケモンのNewフラグがONか
   * @date    2015.08.05
   *
   * @param   id  ライドポケモンID
   *
   * @retval  true  = ON
   * @retval  false = OFF
   */
  //-----------------------------------------------------------------------------
  bool IsRideNewFlag( RideID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsVisibleRotom
   * @brief   ロトムメッセージを表示するか
   * @date    2015.09.03
   *
   * @retval  true  = 表示
   * @retval  false = 非表示
   */
  //-----------------------------------------------------------------------------
  bool IsVisibleRotom(void) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetVisibleRotom
   * @brief   ロトムメッセージを表示するかを設定
   * @date    2015.09.03
   *
   * @param   flg   true = 表示, false = 非表示
   */
  //-----------------------------------------------------------------------------
  void SetVisibleRotom( bool flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMapPosition
   * @brief   屋内にいるときのマップ表示座標を設定
   * @date    2015.09.09
   *
   * @param   x   X座標
   * @param   y   Y座標
   */
  //-----------------------------------------------------------------------------
  void SetMapPosition( f32 x, f32 y );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetMapPosition
   * @brief   屋内にいるときのマップ表示座標を取得
   * @date    2015.09.09
   *
   * @param   x   X座標取得場所
   * @param   y   Y座標取得場所
   */
  //-----------------------------------------------------------------------------
  void GetMapPosition( f32 * x, f32 * y ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomRank
   * @brief   ロトムの親密度ランクを取得
   * @date    2016.11.25
   *
   * @param   index   データインデックス
   *
   * @return  親密度ランク
   */
  //-----------------------------------------------------------------------------
  RotomRank GetRotomRank( u32 index );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomFriendship
   * @brief   ロトムの親密度を取得
   * @date    2016.12.07
   *
   * @return  親密度
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomFriendship(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AddRotomFriendship
   * @brief   ロトムの親密度をアップ
   * @date    2016.11.25
   *
   * @param   index     データインデックス
   * @param   val       アップ値
   * @param   is_touch  true = 枠タッチで上昇した
   *
   * @retval  true  = 性格変化
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool AddRotomFriendship( u32 index, s32 val, bool is_touch );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomCharacter
   * @brief   ロトムの性格を設定
   * @date    2016.11.28
   *
   * @param   character   性格
   */
  //-----------------------------------------------------------------------------
  void SetRotomCharacter( RotomCharacter character );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomCharacter
   * @brief   ロトムの性格を取得
   * @date    2016.11.28
   *
   * @return  性格
   */
  //-----------------------------------------------------------------------------
  RotomCharacter GetRotomCharacter(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    AddRotomCharacterCounter
   * @brief   ロトムの性格変更用のカウンタをアップ
   * @date    2016.11.30
   *
   * @param   character   性格
   * @param   value       アップ値
   */
  //-----------------------------------------------------------------------------
  void AddRotomCharacterCounter( RotomCharacter character, u32 value );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomCharacterCounter
   * @brief   ロトムの性格変更用のカウンタを取得
   * @date    2016.11.30
   *
   * @param   character   性格
   *
   * @return  カウンタ
   */
  //-----------------------------------------------------------------------------
  u32 GetRotomCharacterCounter( RotomCharacter character );

  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomCharacterCounter
   * @brief   ロトムの性格変更用のカウンタを初期化
   * @date    2016.11.30
   */
  //-----------------------------------------------------------------------------
  void InitRotomCharacterCounter(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsFriendshipRotoponStart
   * @brief   ロトポンを開始できる親密度になったか
   * @date    2016.12.06
   *
   * @retval  true  = できる
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsFriendshipRotoponStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    ClearFriendshipRotopon
   * @brief   ロトポン開始の親密度をクリア
   * @date    2016.12.06
   */
  //-----------------------------------------------------------------------------
  void ClearFriendshipRotopon(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomRequest
   * @brief   ロトポンのお願いを設定
   * @date    2016.12.06
   *
   * @param   req   お願い
   */
  //-----------------------------------------------------------------------------
  void SetRotomRequest( RotomRequest req );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetRotomRequest
   * @brief   ロトポンのお願いを取得
   * @date    2016.12.06
   *
   * @return  お願い
   */
  //-----------------------------------------------------------------------------
  RotomRequest GetRotomRequest(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetRotomRequestClearFlag
   * @brief   ロトポンのお願い達成フラグを設定
   * @date    2016.12.06
   *
   * @param   flg   1 = 達成
   */
  //-----------------------------------------------------------------------------
  void SetRotomRequestClearFlag( u8 flg );

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomRequestClearFlag
   * @brief   ロトポンのお願い達成フラグを取得
   * @date    2016.12.06
   *
   * @retval  true  = 達成
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomRequestClearFlag(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    IsRotomTalkStart
   * @brief   ロトムの会話が発生するか
   * @date    2016.12.07
   *
   * @retval  true  = する
   * @retval  false = それ以外
   */
  //-----------------------------------------------------------------------------
  bool IsRotomTalkStart(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    InitRotomTalkStartCount
   * @brief   ロトムの会話開始カウントをクリア
   * @date    2016.12.07
   */
  //-----------------------------------------------------------------------------
  void InitRotomTalkStartCount(void);

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNickNameString
   * @brief   ニックネーム文字列設定
   * @date    2017.01.05
   *
   * @param   src   設定する文字列
   */
  //-----------------------------------------------------------------------------
  void SetNickNameString( const gfl2::str::StrBuf * src );

  //-----------------------------------------------------------------------------
  /**
   * @func    GetNickNameString
   * @brief   ニックネーム文字列取得
   * @date    2017.01.05
   *
   * @param   dst   文字列格納場所
   */
  //-----------------------------------------------------------------------------
  void GetNickNameString( gfl2::str::StrBuf * dst ) const;

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNicknameType
   * @brief   ニックネームタイプ設定
   * @date    2017.02.10
   *
   * @param   type  ニックネームタイプ
   */
  //-----------------------------------------------------------------------------
  void SetNicknameType( NicknameType type );

  //-----------------------------------------------------------------------------
  /**
   * @func    SetNicknameType
   * @brief   ニックネームタイプ取得
   * @date    2017.02.10
   *
   * @return  ニックネームタイプ
   */
  //-----------------------------------------------------------------------------
  NicknameType GetNicknameType(void) const;


#if PM_DEBUG
  void DebugInitMenuSort(void);
  void DebugSetRotomFriendship( u16 val );
  u32 DebugGetRotomFriendshipMax( u32 index );
  void DebugSetRotomCharacterCounter( RotomCharacter character, u32 val );
  u32 DebugGetRotomCharacterChangeOffsetCount(void);
  void DebugSetRotomCharacterChangeOffsetCount( u32 count );
  static u32 DebugGetRotomCharacterChangeOffset(void);
  u32 DebugGetRotomRotoponStartOffsetCount(void);
  void DebugSetRotomRotoponStartOffsetCount( u32 count );
  static u32 DebugGetRotomRotoponStartOffset(void);
#endif


private:
  //-----------------------------------------------------------------------------
  /**
   * @func    GetIconPos
   * @brief   アイコンの位置を取得
   * @date    2015.09.30
   *
   * @param   page  ページ番号格納場所
   * @param   pos   配置位置格納場所
   * @param   id    アイコンID
   *
   * @retval  true  = 成功
   * @retval  false = 失敗
   */
  //-----------------------------------------------------------------------------
  bool GetIconPos( u16 * page, u16 * pos, IconID id );

  //-----------------------------------------------------------------------------
  /**
   * @func    ClearNickNameString
   * @brief   ニックネーム文字列クリア
   * @date    2017.01.05
   */
  //-----------------------------------------------------------------------------
  void ClearNickNameString(void);
};

GFL_NAMESPACE_END(Savedata)

#endif  // FIELD_MENU_SAVE_H_INCLUDED
