#if !defined(__FINDER_STUDIO_SAVE_H_INCLUDED__)
#define __FINDER_STUDIO_SAVE_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioSave.h
 * @date    2017/01/11 18:15:26
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ用セーブデータ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "SaveDataInterface.h"

#include "System/include/DressUp.h"

// ファインダースタジオ定義
#include "App/FinderStudioStatic/include/FinderStudioDefine.h"


GFL_NAMESPACE_BEGIN(Savedata)

//////////////////////////////////////////////////////////////////////////
///
/// @brief    ファインダースタジオ用セーブデータ
///
//////////////////////////////////////////////////////////////////////////
class FinderStudioSave
  : public SaveDataInterface
{
public:
  /// チュートリアルステート
  enum TutorialState
  {
    TUTORIAL_STATE_NONE                       = 0x00,         //!< 初期状態
    TUTORIAL_STATE_CAPTURE_DIFFICULTY         = 0x01,         //!< 撮影：モード選択
    TUTORIAL_STATE_CAPTURE_PREPARE_EASY       = 0x02,         //!< 撮影：準備選択お手軽
    TUTORIAL_STATE_CAPTURE_PREPARE_AUTHENTIC  = 0x04,         //!< 撮影：準備選択本格
    TUTORIAL_STATE_CAPTURE_BACKGROUND         = 0x08,         //!< 撮影：背景
    TUTORIAL_STATE_CAPTURE_DRESSUP            = 0x10,         //!< 撮影：衣装
    TUTORIAL_STATE_CAPTURE_MAIN_EASY          = 0x20,         //!< 撮影：撮影メインお手軽
    TUTORIAL_STATE_CAPTURE_MAIN_AUTHENTIC     = 0x40,         //!< 撮影：撮影メイン本格
    TUTORIAL_STATE_CAPTURE_CAMERA_EASY        = 0x80,         //!< 撮影：カメラお手軽
    TUTORIAL_STATE_CAPTURE_CAMERA_AUTHENTIC   = 0x100,        //!< 撮影：カメラ本格
    TUTORIAL_STATE_CAPTURE_POKEMON_MOVE       = 0x200,        //!< 撮影：ポケモン移動
    TUTORIAL_STATE_CAPTURE_POKEMON_ROTATE     = 0x400,        //!< 撮影：ポケモン回転
    TUTORIAL_STATE_CAPTURE_TRAINER_ROTATE     = 0x800,        //!< 撮影：トレーナー回転
    TUTORIAL_STATE_CAPTURE_PHOTO_SELECT       = 0x1000,       //!< 撮影：写真保存選択
    TUTORIAL_STATE_MYALBUM_INFO               = 0x100000,     //!< マイアルバム：インフォウィンドウ初回表示
    TUTORIAL_STATE_DECORATION_INFO            = 0x1000000,    //!< デコアプリ：インフォウィンドウ初回表示
  };

  static const bool NEW_ICON_INVISIBLE = false;   //!< NEWアイコンを非表示
  static const bool NEW_ICON_VISIBLE = true;      //!< NEWアイコンを表示

  static const u8 BACKGROUND_MAX = 128;   //!< 背景の最大数 128個分確保しておく。
  static const u8 DRESSUP_SAVE_MAX = 3;   //!< 着せ替えデータの保存数。
  static const u8 DRESSUP_MAX = 7;        //!< 着せ替えの最大数

  static const u8 STAMP_S_FLG_MAX = 64;
  static const u8 STAMP_M_FLG_MAX = 64;
  static const u8 STAMP_L_FLG_MAX = 64;
  static const u8 STAMP_CHAR_FLG_MAX = 64;
  static const u8 FRAME_FLG_MAX = 32;

private:
  /// セーブデータ構造体
  struct SaveData
  {
    /// チュートリアルフラグ
    u32  tutorial_state;
    /// 着せ替えデータ
    System::DressUpParamSaveData dressup_param[DRESSUP_SAVE_MAX];   // 16x3 48byte
    /// 着せ替えNEW表示フラグ
    b8 dressup_new_flg[DRESSUP_MAX];                                // 1x7 7byte
    u8 pading1;
    /// 背景のNEW表示フラグ
    b8 bg_new_flg[BACKGROUND_MAX];                                  // 128byte
    
    //
    // ここまでで188byte
    //

    // スタンプ(S)：NEW表示フラグ
    b8 stamp_s_new_flg[STAMP_S_FLG_MAX];          //!< 64byte
    // スタンプ(M)：NEW表示フラグ
    b8 stamp_m_new_flg[STAMP_M_FLG_MAX];          //!< 64byte
    // スタンプ(L)：NEW表示フラグ
    b8 stamp_l_new_flg[STAMP_L_FLG_MAX];          //!< 64byte
    // スタンプ(文字)：NEW表示フラグ
    b8 stamp_char_new_flg[STAMP_CHAR_FLG_MAX];    //!< 64byte
    // フレーム：NEW表示フラグ
    b8 frame_new_flg[FRAME_FLG_MAX];              //!< 32byte

    //
    // スタンプ＆フレームで288byte
    // ここまでで476byte
    //

    /// 通信で共有する写真データ
    App::FinderStudio::Static::IntegratedData   sendData;           // 196byte
    // 通信データの有効フラグ
    b8                                          isSendDataEnable;
    

    /// 余り
    u8 reserve[351];
  };

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  FinderStudioSave( void );
  virtual ~FinderStudioSave( void ) { ; }

public:
  //-------------------------------------------------------
  // @brief   SaveDataInterface実装
  //-------------------------------------------------------
  virtual void   SetData( void* data );
  virtual void*  GetData( void );
  virtual size_t GetDataSize( void );
  virtual void   Clear( gfl2::heap::HeapBase * heap );

  //-------------------------------------------------------
  // @brief   データ 設定・取得
  //-------------------------------------------------------
  void InvisibleBackGroundNewFlg( u8 index );
  bool IsVisibleBackGroundNewFlg( u8 index );

  void SetTutorialState( TutorialState state );
  bool IsTutorialState( TutorialState state );

  void SetDressupParam( u8 index, poke_3d::model::DressUpParam& param );
  poke_3d::model::DressUpParam GetDressupParam( u8 index );
  void InitDressupParam( poke_3d::model::DressUpParam& param );

  void InvisibleDressupNewFlg( u8 index );
  bool IsVisibleDressupNewFlg( u8 index );

  /// スタンプ：Ｓサイズ
  void InvisibleStampSNewFlg( u8 index );
  bool IsVisibleStampSNewFlg( u8 index );

  /// スタンプ：Ｍサイズ
  void InvisibleStampMNewFlg( u8 index );
  bool IsVisibleStampMNewFlg( u8 index );

  /// スタンプ：Ｌサイズ
  void InvisibleStampLNewFlg( u8 index );
  bool IsVisibleStampLNewFlg( u8 index );

  /// スタンプ：文字
  void InvisibleStampCharNewFlg( u8 index );
  bool IsVisibleStampCharNewFlg( u8 index );

  /// フレーム
  void InvisibleFrameNewFlg( u8 index );
  bool IsVisibleFrameNewFlg( u8 index );


  /************************************************************************/
  /*  通信用                                                              */
  /************************************************************************/
  //! 通信で共有する写真データを設定する
  void SetSendPhotoData( const App::FinderStudio::Static::IntegratedData& sendData );
  //! 通信で共有する写真データが有効かどうか
  bool IsSendPhotoDataEnable() const;
  //! 通信で共有する写真データを取得する
  const App::FinderStudio::Static::IntegratedData& GetSendPhotoData() const;
  //! 通信で共有する写真データを解除する
  void RemoveSendPhotoData();


private:
  SaveData  m_Data;
};

GFL_NAMESPACE_END(Savedata)

#endif // __FINDER_STUDIO_SAVE_H_INCLUDED__
