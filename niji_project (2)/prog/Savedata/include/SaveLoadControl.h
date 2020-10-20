
#ifndef __SAVELOADCONTROL_H__
#define __SAVELOADCONTROL_H__
//===================================================================
/**
 * @file    SaveLoadControl.h
 * @brief   セーブロードのコントロールを行うクラス
 * @author  k.ohno
 * @date    2011.3.8
 */
//===================================================================
#pragma once

#if   defined(GF_PLATFORM_WIN32)
namespace gfl2
{
  namespace fs
  {
    class FsCtrBackupSystem
    {
public:
        enum Result {
          RESULT_OK,                     // 成功
          RESULT_CONTINUE,               // 未完了
          RESULT_NG_MEDIA_NOT_EXIST,     // 失敗：メディアが存在しない
          RESULT_NG_NOT_FORMATTED,       // 失敗：セーブデータ領域がフォーマットされていない
          RESULT_NG_BAD_FORMAT,          // 失敗：フォーマットが不正な状態になっている
          RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
          RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
          RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
          RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
          RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
          RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
          RESULT_NG_UNMOUNT,             // 失敗：セーブデータアーカイブがマウントされていない
          RESULT_NG_ALREADY,             // 失敗：( セーブ or ロードを )既に実行中
          RESULT_NG_NOT_START,           // 失敗：( セーブ or ロードを )実行していない
        };
    };

    class FsCtrBackupFile
    {
    public:
              enum Result {
          ACCESS_RESULT_OK,                     // 成功
          ACCESS_RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
          ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
          ACCESS_RESULT_NG_BAD_FORMAT,          // 失敗：フォーマットが不正な状態になっている
          ACCESS_RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
          ACCESS_RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
          ACCESS_RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
          ACCESS_RESULT_OUT_OF_RESOURCE,        // 失敗：リソース不足
          ACCESS_RESULT_ACCESS_DENIED,          // 失敗：アクセス権がない
          ACCESS_RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
        };
    };


  }
}
#else
#include <fs/include/CTR/gfl2_FsCtrBackupSystem.h>
#include <fs/include/CTR/gfl2_FsCtrBackupFile.h>
#endif

GFL_NAMESPACE_BEGIN(Savedata)

  class ISaveDataTotal;

//--------------------------------------------------------------
/**
 * @class SaveLoadControl
 */
//--------------------------------------------------------------
class SaveLoadControl
{
public:

  enum ARC_STATUS {
    ARC_STATUS_OK,          ///<正常
    ARC_STATUS_NEED_INIT,   ///<工場出荷or削除、初期化が必要
    ARC_STATUS_INCORRECT,   ///<異常、初期化が必要
  };

public:

  SaveLoadControl(){};
  virtual ~SaveLoadControl(){};

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータの登録
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @param[in]   name          セーブデータファイル名
   *
   * @note  ゲーム開始直後のみに呼び出します。
   */
  //--------------------------------------------------------------
  virtual void AddBackupEntry( ISaveDataTotal * save_total, const wchar_t * name ) = 0;


  //--------------------------------------------------------------
  /**
   * @brief   セーブデータアーカイブの非同期初期化処理開始
   */
  //--------------------------------------------------------------
  virtual void FormatStart( gfl2::heap::HeapBase * pTempHeap ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブデータアーカイブの非同期初期化処理メイン
   * @param[out]  res     処理結果を受け取るポインタ
   * @retval      true    処理終了
   * @retval      false   処理継続中
   */
  //--------------------------------------------------------------
  virtual bool FormatMain( gfl2::fs::FsCtrBackupSystem::Result * res ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   ロードを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   */
  //--------------------------------------------------------------
  virtual bool LoadStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   ロードを実行：メイン
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupSystem::RESULT_OK           ロード完了
   * -  gfl2::fs::FsCtrBackupSystem::RESULT_NG_xxxx      ロード失敗
   */
  //--------------------------------------------------------------
  virtual bool LoadMain( gfl2::fs::FsCtrBackupFile::Result * result ) = 0;

  virtual gfl2::fs::FsCtrBackupFile::Result LoadAtOnce( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @return  bool  trueのとき、完了  falseのとき実行中
   */
  //--------------------------------------------------------------
  virtual bool SaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを実行：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupSystem::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupSystem::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool SaveMain( gfl2::fs::FsCtrBackupFile::Result * result ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   *
   * @note  LastSave～と組みにして使用します。
   */
  //--------------------------------------------------------------
  virtual bool FirstSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中まで実行：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool FirstSaveMain( gfl2::fs::FsCtrBackupFile::Result * result ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief セーブを途中まで実行：キャンセル
   *
   * @note  セーブデータ変更禁止処理の解除を行っている
   * @caution FirstSaveMainを完了してから呼び出してください。
   */
  //--------------------------------------------------------------
  virtual void FirstSaveCancel( void ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理
   * @param[in]   save_total    セーブデータ全体指定クラス
   * @note    エラーはすべて製品版では発生してはならないもの
   */
  //--------------------------------------------------------------
  virtual void LastSave( ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理：開始
   * @param[in]   pHeap         作業用ヒープ
   * @param[in]   save_total    セーブデータ全体指定クラス
   *
   * @note  FirstSave～と組みにして使用します。
   * @note  FirstSaveMain完了後、通信同期をとって呼び出すことで
   * @note  人為的なデータの増殖などを予防するための機能です。
   */
  //--------------------------------------------------------------
  virtual bool LastSaveStart( gfl2::heap::HeapBase * pHeap, ISaveDataTotal * save_total ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief   セーブを途中から完了処理：メイン
   * @param[out]  result  結果を受け取るポインタ
   * @return  bool  trueのとき、完了  falseのとき実行中
   *
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_OK       セーブ完了
   * -  gfl2::fs::FsCtrBackupFile::ACCESS_RESULT_NG_xxxx  セーブ失敗
   */
  //--------------------------------------------------------------
  virtual bool LastSaveMain( gfl2::fs::FsCtrBackupFile::Result * result ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief セーブデータの状態を取得する
   * @param[in] pTempHeap 作業用ヒープ
   * @return  ARC_STATUS
   */
  //--------------------------------------------------------------
  virtual ARC_STATUS CheckArchiveStatus( gfl2::heap::HeapBase * pTempHeap ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief セーブデータを書き込むためのバッファサイズを取得する
   *
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual u32 GetSaveDataBufferSize( ISaveDataTotal * save_total, bool isEnablePokemonBox ) = 0;

  //--------------------------------------------------------------
  /**
   * @brief バッファへセーブデータ書き込み
   *
   * @param   pBuffer            書き込み先バッファアドレス
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual void SaveToBuffer( u8* pBuffer, ISaveDataTotal* save_total, gfl2::heap::HeapBase* pHeap, bool isEnablePokemonBox ) = 0;

#if PM_DEBUG

  //--------------------------------------------------------------
  /**
   * @brief バッファからセーブデータを読み込む（PGLデバッグ用）
   *
   * @param   pBuffer            読み込み先バッファアドレス
   * @param   save_total         セーブデータアクセスクラス
   * @param   isEnablePokemonBox ポケモンボックスデータを含めるならtrueを指定
   */
  //--------------------------------------------------------------
  virtual void LoadFromBuffer( const u8* pBuffer, ISaveDataTotal * save_total, gfl2::heap::HeapBase * pHeap, bool isEnablePokemonBox ) = 0;


  //--------------------------------------------------------------
  /**
   * @brief テスト用にセーブ容量をダミーで最大サイズまで埋める
   */
  //--------------------------------------------------------------
  virtual void SetDebugDummySizeEnable( bool isEnable ) = 0;
#if   defined(GF_PLATFORM_CTR)
  virtual nn::Result GetLastNNResult( void ) = 0;
#endif
#endif

}; //class SaveLoadControl

GFL_NAMESPACE_END(Savedata)

#endif //__SAVELOADCONTROL_H__
