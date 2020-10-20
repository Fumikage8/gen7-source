#if !defined(REGULATIONEXTSAVEDATA_H_INCLUDED)
#define REGULATIONEXTSAVEDATA_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   RegulationExtSaveData.h
 * @date   2015/11/27 Fri. 11:18:21
 * @author muto_yu
 * @note   sangoより移植及びnijiにあわせて修正
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./NijiExtSaveSystem.h"

#include <System/include/SystemEventListener.h>

class Regulation;


namespace ExtSavedata {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2015/11/27 Fri. 11:19:32
  */
//==============================================================================
class RegulationExtSaveData
  : public    System::SystemEventListener
{
  GFL_FORBID_COPY_AND_ASSIGN(RegulationExtSaveData);
public:
  /**
    ExtSave:  Regulation 種別
  */
  enum  REGULATION_TYPE {
    /*  BattleSpot  */
    REGULATION_TYPE_BATTLESPOT_RATE_SINGLE,         /**<  バトルスポット: レート：シングル          */
    REGULATION_TYPE_BATTLESPOT_RATE_DOUBLE,         /**<  バトルスポット: レート：ダブル            */
    REGULATION_TYPE_BATTLESPOT_RATE_SPECIAL,        /**<  バトルスポット: レート：スペシャル        */
    REGULATION_TYPE_BATTLESPOT_RATE_WCS,
    REGULATION_TYPE_BATTLESPOT_FREE_SINGLE,         /**<  バトルスポット: フリー：シングル伝説          */
    REGULATION_TYPE_BATTLESPOT_FREE_DOUBLE,         /**<  バトルスポット: フリー：ダブル伝説            */
    REGULATION_TYPE_BATTLESPOT_FREE_BATTLEROYALE,   /**<  バトルスポット: フリー：バトルロワイヤル 伝説 */
    REGULATION_TYPE_BATTLESPOT_FREE_SINGLE_NOT_LE,         /**<  バトルスポット: フリー：シングル          */
    REGULATION_TYPE_BATTLESPOT_FREE_DOUBLE_NOT_LE,         /**<  バトルスポット: フリー：ダブル            */
    REGULATION_TYPE_BATTLESPOT_FREE_BATTLEROYALE_NOT_LE,   /**<  バトルスポット: フリー：バトルロワイヤル  */
    REGULATION_TYPE_BATTLESPOT_INTERNET,            /**<  バトルスポット: インターネット大会        */
    REGULATION_TYPE_BATTLESPOT_INTERNET_FRIEND,     /**<  バトルスポット: なかま大会                */

    /*  配信Regulation  */
    REGULATION_TYPE_RESERVED_00,
    REGULATION_TYPE_RESERVED_01,
    REGULATION_TYPE_RESERVED_02,
    REGULATION_TYPE_RESERVED_03,
    REGULATION_TYPE_RESERVED_04,
    REGULATION_TYPE_RESERVED_05,
    REGULATION_TYPE_RESERVED_06,
    REGULATION_TYPE_RESERVED_07,
    REGULATION_TYPE_RESERVED_08,
    REGULATION_TYPE_RESERVED_09,
    REGULATION_TYPE_RESERVED_10,
    REGULATION_TYPE_RESERVED_11,

    REGULATION_TYPE_NUM
  };


  /*  ctor/dtor  */
  RegulationExtSaveData(gfl2::heap::HeapBase* pHeap);
  virtual ~RegulationExtSaveData();


  //------------------------------------------------------------------
  /**
    *  @brief   データロード（同期）
    */
  //------------------------------------------------------------------
  bool LoadRegulationData(const REGULATION_TYPE type, Regulation& rRegulation);

  //------------------------------------------------------------------
  /**
    *  @brief   データセーブ（同期）
    */
  //------------------------------------------------------------------
  bool SaveRegulationData(const REGULATION_TYPE type, const Regulation& rRegulation);

  //------------------------------------------------------------------
  /**
    *  @brief   ファイル存在確認（同期）
    */
  //------------------------------------------------------------------
  bool IsExistsRegulationData(const REGULATION_TYPE type);

  //------------------------------------------------------------------
  /**
    *  @brief   ファイル削除
    */
  //------------------------------------------------------------------
  bool DeleteRegulationData(const REGULATION_TYPE type);

  //------------------------------------------------------------------
  /**
    *  @brief    Fatalエラー発生したかどうか（true: 発生した 、false: 発生してない）
    */
  //------------------------------------------------------------------
  bool IsFatalError(void);



  //------------------------------------------------------------------
  /**
    *  @brief    データロード
    */
  //------------------------------------------------------------------
  bool StartLoadRegulationData(const REGULATION_TYPE type , Regulation& rRegulation);

  //------------------------------------------------------------------
  /**
    *  @brief    データセーブ（渡すデータはセーブ処理が終わるまでDeleteしないでください）
    */
  //------------------------------------------------------------------
  bool StartSaveRegulationData(const REGULATION_TYPE type , const Regulation& rRegulation);

  //------------------------------------------------------------------
  /**
    *  @brief    データ作成（渡すデータはセーブ処理が終わるまでDeleteしないでください）
    */
  //------------------------------------------------------------------
  bool StartCreateRegulationData(const REGULATION_TYPE type , const Regulation& rRegulation);

  //------------------------------------------------------------------
  /**
    *  @brief    データ削除
    */
  //------------------------------------------------------------------
  bool StartDeleteRegulationData(const REGULATION_TYPE type);

  //------------------------------------------------------------------
  /**
    *  @brief    更新　処理終了ならfalse、処理中ならtrue
    */
  //------------------------------------------------------------------
  bool Update(void);


  //------------------------------------------------------------------
  /**
    *  @brief    成功かどうか
    */
  //------------------------------------------------------------------
  bool IsSuccess( void ) const;


    //------------------------------------------------------------------
  /**
   * @brief   ホーム、スリープ、ゲーム終了処理発生時イベント
   *
   * @retval  true    終了処理完了
   * @retval  false   終了処理中
   *
   * @note    適切な終了処理を記述してください。
   * @attention  On～という名前なのに他のメンバとふるまいが違うので注意
   * @attention  Observerパターンとして名前が適切なのかは疑問の余地あり
   */
  //------------------------------------------------------------------
  virtual bool OnSuspendWithHomeSleepAndPower( void );

  //------------------------------------------------------------------
  /**
    *  @brief    結果コード取得
    */
  //------------------------------------------------------------------
  gfl2::fs::ExtSaveFile::Result GetResult( void ) const   { return m_result; };

private:

  bool serializeRegulationDataToStaticTemp( const Regulation& rRegulation );

private:

  typedef enum
  {
    SAVE_LOAD_MODE_none = 0,
    SAVE_LOAD_MODE_isExistsSave,
    SAVE_LOAD_MODE_isExistsLoad,
    SAVE_LOAD_MODE_create,
    SAVE_LOAD_MODE_load,
    SAVE_LOAD_MODE_save,
    SAVE_LOAD_MODE_waitDeleteFile,
    SAVE_LOAD_MODE_finish,
    SAVE_LOAD_MODE_error,
  }SAVE_LOAD_MODE;

  const Regulation*               m_pSaveData;
  Regulation*                     m_pLoadData;
  gfl2::fs::ExtSaveFile::Result   m_result;
  gfl2::heap::HeapBase*           m_pHeap;
  SAVE_LOAD_MODE                  m_mode;
  s32                             m_saveLoadIndex;
  u8*                             m_pSerializedRegulationData;
  bool                            m_bIsFatalError;

};


} /*  namespace ExtSavedata */
#endif  /*  #if !defined(REGULATIONEXTSAVEDATA_H_INCLUDED)  */
