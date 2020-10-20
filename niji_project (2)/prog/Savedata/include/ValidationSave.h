//============================================================================================
/**
 * @file    ValidationSave.h
 * @author  suzuki_hisanao
 */
//============================================================================================

#ifndef __SV_VALIDATION_H__
#define __SV_VALIDATION_H__

#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>
#include "heap/include/gfl2_heap_manager.h"
#include "Savedata/include/SaveDataInterface.h"

namespace Savedata
{

/***
 * @brief 不正チェックサーバ公開鍵用用セーブデータ
 *
 * @note KUJIRAでは不正チェックサーバで利用する公開鍵をバージョンアップできる機能が準備されている。
 * @note 本セーブデータは不正チェックサーバよりダウンロードされた新しい公開鍵を保存する為に使用する。
 */
class ValidationSave : public SaveDataInterface
{
public:
  //--------------------------------------------------------------------------------------------
  /**
   * @brief   コンストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ValidationSave(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief   デストラクタ
   */
  //--------------------------------------------------------------------------------------------
  ~ValidationSave(void);

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 公開鍵のバージョンを取得
   *
   * @return 公開鍵のバージョン番号
   */
  //--------------------------------------------------------------------------------------------
  u16 GetVersion(void) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 公開鍵のサイズを取得
   *
   * @return 公開鍵のサイズ
   */
  //--------------------------------------------------------------------------------------------
  size_t GetPublicKeySize(void) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 公開鍵を取得
   *
   * @return 公開鍵
   */
  //--------------------------------------------------------------------------------------------
  const void* GetPublicKey(void) const;

  //--------------------------------------------------------------------------------------------
  /**
   * @brief 公開鍵をサーバから取得した情報で更新する
   *
   * @param[in] version       バージョン情報
   * @param[in] publicKey     公開鍵
   * @param[in] publicKeySize 公開鍵のサイズ
   */
  //--------------------------------------------------------------------------------------------
  void UpdatePublicKey( u16 version, const u8* publicKey, u32 publicKeySize );

  //=============================================================================
  //@brief SaveDataInterface実装
  //=============================================================================
public:
  /**
   *  @brief    読み込んだデータをセットする関数
    * @param[in]    pData   先頭のポインタ
   */
  virtual void SetData(void* data);
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  virtual void* GetData(void);
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  virtual size_t GetDataSize(void);
  /**
   * @brief セーブデータのクリア
   * @param[in] heap クリアに使用するテンポラリヒープ
   */
  virtual void Clear( gfl2::heap::HeapBase * heap );


  //=============================================================================
  //@brief 以下private
  //=============================================================================
private:
  static const u32   PUBLIC_KEY_MAX_SIZE = 392;       //!< 公開鍵のサイズ
  static const char* PUBLIC_CER;                      //!< デフォルトの公開鍵
  /**
   * @brief 写真アイコン管理情報
   */
  typedef struct
  {
    size_t            m_publicKeySize;   //!< 公開鍵のサイズ
    u16               m_version;         //!< バージョン
    //
    u8*               m_publicKey[PUBLIC_KEY_MAX_SIZE];  //!< 公開鍵
    u8                m_reserve[256];//予備
  }CoreData;



private:
  CoreData m_data;  //!< データ本体
};


} //namespace savedata


#endif // __SV_VALIDATION_H__
