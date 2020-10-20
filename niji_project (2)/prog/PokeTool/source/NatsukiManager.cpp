//=============================================================================
/**
 * @brief  ポケモンのなつき度更新管理
 * @file   NatsukiManager.cpp
 * @author tamada
 * @date   2012.06.09
 */
//=============================================================================
#include "PokeTool/include/NatsukiManager.h"

#include "niji_conv_header/poke_lib/item/monsball_id.h"
// ロトムパワー
#include <GameSys/include/GameManager.h>
#include "Field/FieldStatic/include/RotomPower/RotomPowerManager.h"

namespace PokeTool {

    //-------------------------------------------------------------------------
    /**
     * @brief なつき度変動テーブル
     * @note  テーブルの3つの値は、それぞれなつき度が100未満・200未満・200以上の場合となっている。
     */
    //-------------------------------------------------------------------------
    static	const	s8	CalcNatsukiTable[][3]={
      {  5,  4,  3 },		//レベルアップ
      {  0,  0,  0 },		//アイテム使用（アイテムデータから取得）
      {  5,  4,  3 },		//ボス戦に参加（ジムリーダー、四天王、チャンプ）
      {  2,  2,  1 },		//連れ歩き
      { -1, -1, -1 },		//瀕死
      { -5, -5,-10 },		//レベル３０差以上の瀕死
      {  0,  0,  0 },   //ぽかぽかリゾート（リゾートのデータから取得）
    };

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    NatsukiManager::NatsukiManager()
    {
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    NatsukiManager::~NatsukiManager()
    {
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::Calc( pml::pokepara::CoreParam * pp, NatsukiType type, u16 place_id )
    {
      CalcCore( pp, place_id, CalcNatsukiTable[type] );
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcWhenWalk( pml::pokepara::CoreParam * pp, u16 place_id )
    {
      CalcCore( pp, place_id, CalcNatsukiTable[NATSUKI_TYPE_TSUREARUKI] );
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcWhenUseItem(pml::pokepara::CoreParam * pp,
            u16 place_id, const item::ITEM_DATA * item_data )
    {
      s8 table[3];
      table[0] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND1_POINT );
      table[1] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND2_POINT );
      table[2] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND3_POINT );
      CalcCore( pp, place_id, table );
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcWhenUseItem(pml::pokepara::CoreParam * pp,
            u16 place_id, u16 itemno, itemman::ITEM_MANAGER * itemMan )
    {
      s8 table[3];
      table[0] = itemMan->GetParam( itemno, item::ITEM_DATA::PRM_ID_FRIEND1_POINT );
      table[1] = itemMan->GetParam( itemno, item::ITEM_DATA::PRM_ID_FRIEND2_POINT );
      table[2] = itemMan->GetParam( itemno, item::ITEM_DATA::PRM_ID_FRIEND3_POINT );
      CalcCore( pp, place_id, table );
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcWhenUseItem( pml::pokepara::CoreParam * pp,
        u16 place_id, u16 itemno, gfl2::heap::HeapBase * heap )
    {
      const item::ITEM_DATA * item_data = GFL_NEW( heap ) item::ITEM_DATA( itemno, heap );
      s8 table[3];
      table[0] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND1_POINT );
      table[1] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND2_POINT );
      table[2] = item_data->GetParam( item::ITEM_DATA::PRM_ID_FRIEND3_POINT );
      CalcCore( pp, place_id, table );
      GFL_DELETE item_data;
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcWhenResortHotSpa( pml::pokepara::CoreParam * pp, u16 place_id, s8 val )
    {
      s8 table[3];
      for( u32 i=0; i<3; i++ )
      {
        table[i] = val;
      }
      CalcCore( pp, place_id, table );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief なつき度操作：共通関数
     * @param heap      作業用ヒープ指定
     * @param place_id  現在の場所ID
     * @param table     なつき度変化量テーブル
		 * @param amp       アシストパワーマネージャ
     */
    //-------------------------------------------------------------------------
    void NatsukiManager::CalcCore( pml::pokepara::CoreParam * pp, u16 place_id, const s8 * table )
    {
      if ( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ) return;
//      TAMADA_PRINT("NatsukiTable[%d,%d,%d]\n", table[0], table[1], table[2]);
      int familiarity = pp->GetFamiliarity();
      if ( familiarity >= 100 ) table ++;
      if ( familiarity >= 200 ) table ++;
      int updown_value = *table;
      if ( updown_value > 0 )
      {
        int ball_id = pp->GetGetBall();
        //捕獲ボールがゴージャスボールだと1段階上がる
        if ( ball_id == item::BALLID_GOOZYASUBOORU ) updown_value ++;
        //生まれた場所、捕まえた場所だと1段階上がる
        if ( pp->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE ) == place_id ) updown_value ++;
        //なつき度アップどうぐ保持の場合は1.5倍アップする
        if ( IsNatsukiUpItem( pp->GetItem() ) == true )
        {
          updown_value = updown_value * 150 / 100;
        }
        // ロトムパワーの影響を加算する（減算時は影響しない）
        updown_value = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetRotomPowerManager()->CalcNatsuki(updown_value);
      }

//      TAMADA_PRINT("NatsukiManager::Calc %d\n", updown_value );
      familiarity += updown_value;
      if ( familiarity < 0 ) familiarity = 0;
      if ( familiarity > 255 ) familiarity = 255;

      pp->SetFamiliarity( familiarity );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アイテムがなつき度を変更するものかどうかの判定処理
     * @param itemno  判定するアイテムNo.
     * @caution 追加があった場合はアイテムのパラメータ変更だけでは実現できないことに注意。
     * @attention 本来はアイテムのパラメータで判断するべきだが…非同期読み込みにするためには仕組みが必要
     */
    //-------------------------------------------------------------------------
    bool NatsukiManager::IsNatsukiUpItem( u32 itemno )
    {
      if ( itemno == ITEM_YASURAGINOSUZU ) return true;
      return false;
    }

} //namespace PokeTool

