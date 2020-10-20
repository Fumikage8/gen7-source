#pragma once
//=================================================================================================
/**
*  @file   FieldMoveModelSave.h
*  @brief  フィールド用 動作モデルの定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Quaternion.h>
#include <Savedata/include/SaveDataInterface.h>
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"

GFL_NAMESPACE_BEGIN(Savedata)

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用のセーブデータ
  //-----------------------------------------------------------------------------
  class FieldMoveModelSave : public SaveDataInterface
  {
  public:

    //-----------------------------------------------------------------------------
    //! @brief 動作モデル用の復元データ
    //  @note  ここには変数宣言だけで実処理を書いていけません
    //-----------------------------------------------------------------------------
    struct ReturnDataForPlayer
    {
      Field::MoveModel::FIELD_MOVE_CODE_ID  moveCodeId;
      u32                                   eventId;
      gfl2::math::Vector3                   posForReturn;
      gfl2::math::Quaternion                quaForReturn;
      u32                                   rideEventId;
      u32                                   rideCharacterId;
      u16                                   zoneId;
      u32                                   moveCodeWork[Field::MoveModel::FIELD_MOVE_CODE_WORK_SIZE/4];
    };
    struct ReturnDataForNpc
    {
      Field::MoveModel::FIELD_MOVE_CODE_ID  moveCodeId;
      u32                                   eventId;
      gfl2::math::Vector3                   posForReturn;
      gfl2::math::Quaternion                quaForReturn;
      u16                                   zoneId;
      u32                                   moveCodeWork[Field::MoveModel::FIELD_MOVE_CODE_WORK_SIZE/4];
    };
    struct ReturnDataForMoveModel
    {
      ReturnDataForPlayer                   player;
      ReturnDataForNpc                      npc[Field::MoveModel::FIELD_MOVE_MODEL_MAX-2];  // player・rideの分を除外
    };

    //------------------------------------------------------------------------------------------
    /**
     * @brief    読み込んだデータをセットする関数
     *
     * @param    pData    先頭のポインタ
     *
     * @return  none
     */
    //------------------------------------------------------------------------------------------
    virtual void SetData( void* pData ){ gfl2::std::MemCopy( pData, reinterpret_cast<void*>(&returnDataForMoveModel), sizeof(ReturnDataForMoveModel) ); };

    //------------------------------------------------------------------------------------------
    /**
     * @brief    データの先頭ポインタを得る関数
     *
     * @param    none
     *
     * @return  先頭のポインタ
     */
    //------------------------------------------------------------------------------------------
    virtual void * GetData(void){ return reinterpret_cast<void*>(&returnDataForMoveModel); };

    //------------------------------------------------------------------------------------------
    /**
     * @brief    データサイズ
     *
     * @param    none
     *
     * @return  バイト
     */
    //------------------------------------------------------------------------------------------
    virtual size_t GetDataSize(void){ return sizeof(ReturnDataForMoveModel); };

    //-----------------------------------------------------------------------------
    /**
     * @brief セーブデータのクリア
     * @param heap　クリアに使用するテンポラリヒープ
     */
    //-----------------------------------------------------------------------------
    virtual void Clear( gfl2::heap::HeapBase * heap ) { gfl2::std::MemFill( &returnDataForMoveModel,0,sizeof(ReturnDataForMoveModel) ); }

    ReturnDataForMoveModel returnDataForMoveModel;
  };

  //-----------------------------------------------------------------------------
  //! @brief 動作モデル用の復元データからリクエスト内容を決定するファクトリー
  //-----------------------------------------------------------------------------
  struct FieldMoveModelHeaderFactoryFromReturnData
  {
    //------------------------------------------------------------------
    /**
    * @brief   復元データからプレイヤー用動作モデルヘッダーを作成する
    *
    * @retval  動作モデルヘッダー
    */
    //------------------------------------------------------------------
    static Field::MoveModel::FieldMoveModelHeaderWork CreatePlayerFieldMoveModelHeader(FieldMoveModelSave::ReturnDataForPlayer* pReturnDataForFieldMoveModelHeader )
    {
      Field::MoveModel::FieldMoveModelHeaderWork fieldMoveModelHeader;
      fieldMoveModelHeader.moveCodeId   = pReturnDataForFieldMoveModelHeader->moveCodeId;
      fieldMoveModelHeader.eventId      = pReturnDataForFieldMoveModelHeader->eventId;
      fieldMoveModelHeader.posForReturn = pReturnDataForFieldMoveModelHeader->posForReturn;
      fieldMoveModelHeader.quaForReturn = pReturnDataForFieldMoveModelHeader->quaForReturn;
      fieldMoveModelHeader.zoneId       = pReturnDataForFieldMoveModelHeader->zoneId;
      return fieldMoveModelHeader;
    }

    //------------------------------------------------------------------
    /**
    * @brief   復元データからNPC用動作モデルヘッダーを作成する
    *
    * @retval  動作モデルヘッダー
    */
    //------------------------------------------------------------------
    static Field::MoveModel::FieldMoveModelHeaderWork CreateNpcFieldMoveModelHeader(FieldMoveModelSave::ReturnDataForNpc* pReturnDataForFieldMoveModelHeader)
    {
      Field::MoveModel::FieldMoveModelHeaderWork fieldMoveModelHeader;
      fieldMoveModelHeader.moveCodeId   = pReturnDataForFieldMoveModelHeader->moveCodeId;
      fieldMoveModelHeader.eventId      = pReturnDataForFieldMoveModelHeader->eventId;
      fieldMoveModelHeader.posForReturn = pReturnDataForFieldMoveModelHeader->posForReturn;
      fieldMoveModelHeader.quaForReturn = pReturnDataForFieldMoveModelHeader->quaForReturn;
      fieldMoveModelHeader.zoneId       = pReturnDataForFieldMoveModelHeader->zoneId;
      return fieldMoveModelHeader;
    }
  };

GFL_NAMESPACE_END(Savedata)