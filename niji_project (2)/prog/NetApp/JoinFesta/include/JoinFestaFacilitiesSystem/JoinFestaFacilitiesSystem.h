//==============================================================================
/**
 * @file	JoinFestaFacilitiesSystem.h
 * @brief	ジョインフェスタ施設システム
 * @author	ichiraku_katsuhiko
 * @date	2015/10/22　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_FACILITIES_SYSTEM_H__
#define __JOIN_FESTA_FACILITIES_SYSTEM_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
// DebugWin
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

// shop
#include "App/Shop/include/ShopDef.h"

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"
// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModel.h"
#include "NetApp/JoinFesta/include/JoinFestaFacilitiesSystem/JoinFestaFacilitiesModelResourceManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 前方宣言
class   JoinFestaFacilitiesData;
class   JoinFestaFacilitiesModel;
class   JoinFestaFacilitiesMoveModel;
class   JoinFestaFacilitiesModelResourceManager;
struct  JoinFestaDataTable;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaFacilitiesSystem
  : public  App::Shop::IShopFeatureDelegate
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaFacilitiesSystem);
public:

  //-----------------------------------------------------------------------------
  /**
   *  定数
   */
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------


public:
  // コンストラクタ
  JoinFestaFacilitiesSystem( gfl2::heap::HeapBase* pHeap, bool isRoom );
  // デストラクタ
  virtual ~JoinFestaFacilitiesSystem();

  //=============================================================================
  /**
   *  publicメソッド
   */
  //=============================================================================
  //------------------------------------------------------------------
  /**
   * @brief   初期化
   * @param[in] pDataTable　データテーブルのポインタ
   */
  //------------------------------------------------------------------
  void Initialize( const JoinFestaDataTable* pDataTable );

  //------------------------------------------------------------------
  /**
   * @brief   初期化が完了したか
   * @return  trueで完了した
   */
  //------------------------------------------------------------------
  bool IsInitializeComplete();

  //------------------------------------------------------------------
  /**
   * @brief   破棄
   * @return  RESULT  関数の結果
   */
  //------------------------------------------------------------------
  RESULT Terminate();
  
  //------------------------------------------------------------------
  /**
   * @brief   モデルアニメーション更新
   */
  //------------------------------------------------------------------
  void UpdateAnime();
  
  //------------------------------------------------------------------
  /**
   * @brief   施設データ取得
   * @param[in] facilitiesIndex 施設インデックス
   * @return  施設データのポインタ
   */
  //------------------------------------------------------------------
  const JoinFestaFacilitiesData* GetFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex ) const;
  JoinFestaFacilitiesData* GetFacilitiesData( JoinFestaScript::FacilitiesIndex facilitiesIndex );


  //------------------------------------------------------------------
  /**
    *  @brief    施設Modelの取得
    *  @param    [in] facilitiesIndex 施設インデックス
    *  @return   JoinFestaFacilitiesModel
    */
  //------------------------------------------------------------------
  JoinFestaFacilitiesModel* GetFacilityModel(const u32 index) {return ((index < GFL_NELEMS(m_facilities)) ? m_facilities[index].pFacilitiesModel : NULL);}

  //------------------------------------------------------------------
  /**
    *  @brief    城Modelの取得
    *  @return   JoinFestaFacilitiesModel
    */
  //------------------------------------------------------------------
  JoinFestaFacilitiesModel* GetCastleModel(){return m_pCastleModel;}


  //------------------------------------------------------------------
  /**
   * @brief   施設データテーブル取得
   * @return  施設データテーブルのポインタ
   */
  //------------------------------------------------------------------
  const JoinFestaDataTable*  GetDataTable(){ return m_pJoinFestaDataTable; }
  
  
  /*  implement App::Shop::IShopFeatureDelegate  */
  virtual void  ShopFeatureDelegate_CreateShopItemList(App::Shop::SHOP_ITEM** ppShopItemList, u32& rShopItemNum, gfl2::heap::HeapBase* pHeapBase, const u32 tableIndex);
  virtual void  ShopFeatureDelegate_HandlingResult(const App::Shop::APP_PARAM& rAppParam);

  /************************************************************************/
  /* スクリプトで施設を変更する為、外部公開                              */
  /************************************************************************/
  //------------------------------------------------------------------
  /**
   * @brief   全ての施設データ作成
   */
  //------------------------------------------------------------------
  void CreateAllFacilitiesData();

  //------------------------------------------------------------------
  /**
   * @brief   全ての施設データ削除
   */
  //------------------------------------------------------------------
  void DeleteAllFacilitiesData();

  //------------------------------------------------------------------
  /**
   * @brief   全ての施設モデル作成
   */
  //------------------------------------------------------------------
  void CreateAllFacilitiesModel();

  //------------------------------------------------------------------
  /**
   * @brief   全ての施設モデル削除
   * @return  trueで完了
   */
  //------------------------------------------------------------------
  bool DeleteAllFacilitiesModel();

  //------------------------------------------------------------------
  /**
   * @brief   店員動作モデルのセットアップ
   */
  //------------------------------------------------------------------
  void SetupMoveModel();
  //------------------------------------------------------------------
  /**
   * @brief   店員動作モデルのセットアップ完了したか
   * @return  trueで完了
   */
  //------------------------------------------------------------------
  bool IsSetupMoveModel();
  //------------------------------------------------------------------
  /**
   * @brief   店員動作モデルの破棄
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool TerminateMoveModel();
  
  
  //=============================================================================
  /**
   *  privateメソッド
   */
  //=============================================================================
private:

  //------------------------------------------------------------------
  /**
   * @brief   城作成
   */
  //------------------------------------------------------------------
  void CreateCastle();

  //------------------------------------------------------------------
  /**
   * @brief   城削除
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool DeleteCastle();

  //------------------------------------------------------------------
  /**
   * @brief   地面作成
   */
  //------------------------------------------------------------------
  void CreateGround();

  //------------------------------------------------------------------
  /**
   * @brief   地面削除
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool DeleteGround();

  //------------------------------------------------------------------
  /**
   * @brief   RR団イベント用空モデル作成
   */
  //------------------------------------------------------------------
  void CreateRrSky();

  //------------------------------------------------------------------
  /**
   * @brief   RR団イベント用空モデル削除
   * @return  trueで破棄完了
   */
  //------------------------------------------------------------------
  bool DeleteRrSky();

  //------------------------------------------------------------------
  /**
   * @brief   チュートリアル中かどうか
   * @return  trueでチュートリアル中
   */
  //------------------------------------------------------------------
  bool IsTutorial();


private:

  //! 施設構造体
  struct Facilities{
    JoinFestaFacilitiesData*        pFacilitiesData;
    JoinFestaFacilitiesModel*       pFacilitiesModel;
    JoinFestaFacilitiesMoveModel*   pFacilitiesMoveModel;
  };


  gfl2::heap::HeapBase*                       m_pHeap;                //! ヒープ
  gfl2::heap::HeapBase*                       m_pFieldExtHeap;        //! ヒープ
  bool                                        m_isInitialize;         //! 初期化済みフラグ
  bool                                        m_isRoom;               //! 城内かどうか
  JoinFestaFacilitiesModelResourceManager     m_resourceManager;      //! リソースマネージャ
  JoinFestaFacilitiesModel*                   m_pCastleModel;         //! 城モデル
  JoinFestaFacilitiesModel*                   m_pGroundModel;         //! 地面モデル
  JoinFestaFacilitiesModel*                   m_pRrSkyModel;          //! RR団イベント用空モデル
  Facilities                                  m_facilities[ JoinFestaScript::FACILITIES_INDEX_MAX ];  //! 施設
  const JoinFestaDataTable*                   m_pJoinFestaDataTable;

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

#endif // __JOIN_FESTA_FACILITIES_SYSTEM_H__
