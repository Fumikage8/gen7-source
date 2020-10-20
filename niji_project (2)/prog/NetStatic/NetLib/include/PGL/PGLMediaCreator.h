//======================================================================
/**
 * @file  PGLMediaCreator.h
 *
 * @author  suzuki_hisanao
 */
//======================================================================
#if defined(GF_PLATFORM_CTR)
#ifndef _PGL_MEDIA_CREATOR_H_
#define _PGL_MEDIA_CREATOR_H_

#include <nex.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <heap/include/gfl2_Heap.h>

#include <Savedata/include/MyStatus.h>
#include <system/include/Date/gfl2_Date.h>

namespace savedata{
  class MyStatus;
}

GFL_NAMESPACE_BEGIN(NetLib)
GFL_NAMESPACE_BEGIN(pgl)


//--------------------------------------------------------------------------------------------
/**
 * @brief ＰＧＬに送信するためのメディアデータを生成する
 *
 * @par  本クラスでは下記のデータを生成または展開する
 *        - プレイヤーアイコンデータ（40x40pixel, RGB5A1, リニアイメージ）
 *        - カメラデータ（拡張セーブデータからロードしたもの）
 */
//--------------------------------------------------------------------------------------------
class PGLMediaCreator
{
public:
  static const nn::nex::qInt32 POST_BUFF_SIZE  = 8192;  //!< POSTバッファサイズ
  static const u32             URL_LENGTH      = 128;   //!< 最大URL文字数

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
   PGLMediaCreator( void );

   //--------------------------------------------------------------------------------------------
   /**
    * @brief デストラクタ
    */
   //--------------------------------------------------------------------------------------------
   virtual ~PGLMediaCreator();

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief 初期化処理（ブロック関数）
   *
   * @param[in] heap             データ生成に利用されるヒープ
   * @param[in] devHeap          データ生成に利用されるデバイスヒープ
   * @param[in] fileReadMgr      ファイルリードマネージャ
   * @param[in] myStatus         プレイヤーステータス
   * @param[in] isGameSyncSignIn ゲームシンクにサインインしているならtrueを指定
   * @param[in] iconMgr          外部で生成されたアイコンマネージャを使用する際に指定する。
   *                        @arg NULL指定時は自身でアイコンマネージャを生成する。
   *
   * @retval true  初期化成功
   * @retval false 初期化失敗
   */
  //--------------------------------------------------------------------------------------------
  bool Initialize( gfl2::heap::HeapBase *heap, gfl2::heap::HeapBase *devHeap,
                   //gfl2::fs::FileReadManager* fileReadMgr,
                   const Savedata::MyStatus* myStatus,
                   bool isGameSyncSignIn
                   //xy_system::player_icon::PlayerIconManager* iconMgr=NULL 
                   );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 終了処理
   */
  //--------------------------------------------------------------------------------------------
  void Finalize();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 更新処理
   *
   * @retval true  処理継続中
   * @retval false 処理終了
   */
  //--------------------------------------------------------------------------------------------
  bool Update();

  //--------------------------------------------------------------------------------------------
  /**
   * @brief ＰＧＬへのサインイン状態の変更
   *
   * @param[in] isSignIn サインイン状態に設定するならtrueを指定
   */
  //--------------------------------------------------------------------------------------------
  void ChageGameSyncSignIn( bool isSignIn );

public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief プレイヤーアイコンデータ(40x40, RGB5A1)を取得
   *
   * @return プレイヤーアイコンデータへのポインタ。存在しない場合はNULLを返却。
   */
  //--------------------------------------------------------------------------------------------
  const void* GetPlayerIconData( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief カメラデータを取得
   *
   * @return カメラデータへのポインタ。存在しない場合はNULLを返却
   */
  //--------------------------------------------------------------------------------------------
  //const extSavedata::CameraExtSaveData::CAMERA_EXT_SAVE_DATA* GetCameraData( void ) const;
  const void* GetCameraData( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief カメラ撮影時間を取得
   *
   * @return 日付情報へのポインタ。存在しない場合はNULLを返却
   */
  //--------------------------------------------------------------------------------------------
  gfl2::system::Date* GetDateOfCameraData( void ) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 写真データを本体セーブ領域へセーブする
   */
  //--------------------------------------------------------------------------------------------
  void StartCameraSave( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 送信済みフラグを巻き戻し
   */
  //--------------------------------------------------------------------------------------------
  void RollbackCameraSave( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 写真データ送信のコミット処理
   *
   * @note 以後はRollbackCameraSaveをコールしても巻き戻らない
   */
  //--------------------------------------------------------------------------------------------
  void CommitCameraSave( void );

private:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief ブロックフォーマットのアイコンをリニアフォーマットに変更
   */
  //--------------------------------------------------------------------------------------------
  void  ConvertBlock2LinierIcon( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 64x64アイコンデータを40x40のデータに変換
   */
  //--------------------------------------------------------------------------------------------
  void  ResizeIcon( void );

  //--------------------------------------------------------------------------------------------
  /**
   * @brief カメラデータの解析
   */
  //--------------------------------------------------------------------------------------------
  void  AnalyzeCameraData( void );

private:
  /**
   * @brief シーケンス情報
   */
  typedef enum
  {
    E_SEQ_CREATE_NONE,        //!< none
    E_SEQ_CREATE_ICON,        //!< アイコンデータの生成待ち
    E_SEQ_CONVERT_ICON_B2L,   //!< アイコンデータをBlockからリニアフォーマットに変換
    E_SEQ_LOAD_CAMERA,        //!< カメラデータロード開始
    E_SEQ_WAIT_CAMERA,        //!< カメラデータロード待ち
    E_SEQ_FINISH,             //!< 終了
  }SEQUENCE;

  static const u32 IMAGE_SIZE_RGB5551_64_64 = 64*64*2;                       //!< 64x64のアイコンテクスチャーサイズ

private:
  //xy_system::player_icon::PlayerIconManager*            m_playerIconMgr;     //!< プレイヤーアイコンマネージャ
  //common::resource::PlayerIconTex*                      m_playerIconTex;     //!< プレイヤーアイコンテクスチャ
  //app::ui::UIPlayerIcon*                                m_playerIcon;        //!< プレイヤーアイコン
  //
  u8*                                                   m_iconData;          //!< アイコンデータ用バッファ
  //
  gfl2::system::Date*                                   m_cameraDate;        //!< カメラ撮影日付
  //extSavedata::CameraExtSaveData::CAMERA_EXT_SAVE_DATA  m_cameraData;        //!< カメラーデータ
  //extSavedata::CameraExtSaveData*                       m_cameraSave;        //!< カメラセーブコントトーラ
  //
  u64                                                   m_nexUniqueId;       //!< NexユニークID
  //
  SEQUENCE                                              m_seq;               //!< シーケンス
  //
  bool                                                  m_isCreateIconMgr;   //!< アイコンマネージャ生成フラグ
  bool                                                  m_isGameSyncSignIn;  //!< ゲームシンクサインインフラグ
  bool                                                  m_isIconCreated;     //!< アイコン生成済みフラグ
  bool                                                  m_isCameraCreated;   //!< カメラデータ生成済みフラグ
};


GFL_NAMESPACE_END(pgl)
GFL_NAMESPACE_END(netapp)

#endif  // _PGL_MEDIA_CREATOR_H_
#endif