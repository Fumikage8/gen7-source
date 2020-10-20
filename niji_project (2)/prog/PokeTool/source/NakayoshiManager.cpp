//=============================================================================
/**
 * @brief  ポケモンの仲良し度更新管理
 * @file   NakayoshiManager.cpp
 * @author tamada
 * @date   2012.06.10
 */
//=============================================================================
#include "pokeTool/include/NakayoshiManager.h"


namespace PokeTool {

  namespace nakayoshi {

    static s32 ChangeFriendship( s32 friendship, s32 change_value );


    //-------------------------------------------------------------------------
    /**
     * @brief なかよし度取得処理
     * @param pp        対象となるポケモン
     * @param myStatus  対象となるトレーナーのMyStatus
     *
     * @return  u8    なかよし度
     */
    //-------------------------------------------------------------------------
    u8 GetFriendship( const pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus )
    {
      if ( pp->IsOwnedOriginalParent() )
      {
        return pp->GetFriendship();
      } else {
        return pp->GetOthersFriendship();
      }
    }


    //-------------------------------------------------------------------------
    /**
     * @brief 仲良し度削減処理
     * @note  ストッパー仕様のため、単純にマイナスしていない
     */
    //-------------------------------------------------------------------------
    s32 ChangeFriendship( s32 friendship, s32 change_value )
    {
      static const s32 KAWAIGARI_NUMBER_STOPPER[] = {
        200,
        150,
        100,
        75,
        50,
        25,
        0,
      };
      u32 stopper_index = 0;
      for( s32 j=0; j<GFL_NELEMS(KAWAIGARI_NUMBER_STOPPER); ++j ){
        if( KAWAIGARI_NUMBER_STOPPER[j] <= friendship ){
          break;
        }
        ++stopper_index;
      }
      if( change_value > 0 ) {
        friendship += change_value;
      }
      else if( GFL_NELEMS(KAWAIGARI_NUMBER_STOPPER) > stopper_index
          && friendship + change_value > KAWAIGARI_NUMBER_STOPPER[stopper_index] ){
        friendship += change_value;
      }
      if ( friendship > 255 ) friendship = 255;
      if ( friendship < 0 ) friendship = 0;

      return friendship;
    }

    //-------------------------------------------------------------------------
    //  なかよし度　更新処理
    //-------------------------------------------------------------------------
    void UpdateFriendship( pml::pokepara::CoreParam * pp, const Savedata::MyStatus * myStatus, s32 value )
    {
      if ( pp->IsOwnedOriginalParent() )
      { //　オリジナルの親なので、そのまま
        s32 friendship = pp->GetFriendship();
        friendship = ChangeFriendship( friendship, value );
        pp->SetFriendship( friendship );
      }
      else
      { //仲良しトレーナー用の操作
        s32 friendship = pp->GetOthersFriendship();
        friendship = ChangeFriendship( friendship, value );
        pp->SetOthersFriendship( friendship );
      }
    }


    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void UpdateFeedAndNadenadeOnField( pml::PokeParty * party )
    {
      enum{
        UPDATE_FEED_ON_FIELD = 1,
        UPDATE_NADENADE_ON_FIELD = 1,
      };
      u32 party_num = party->GetMemberCount();

      for( u32 i = 0; i < party_num;i++){
        pml::pokepara::CoreParam * pp = party->GetMemberPointer(i);

        if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ){
          continue; //タマゴはスキップ
        }
        // 満腹度-1
        pp->SubFeed( UPDATE_FEED_ON_FIELD );
        // なでなで度 -1
        pp->SubNadenadeValue( UPDATE_NADENADE_ON_FIELD );
      }
    }

    //-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    void UpdateFeedAndNadenadeAfterBattle( pml::PokeParty * party, const u8* p_fight_index_array )
    {
      u32 party_num = party->GetMemberCount();
      enum{
        UPDATE_FEED_AFTER_BATTLE = 25,
        UPDATE_NADENADE_AFTER_BATTLE = 25,
      };
      for( u32 i = 0; i < party_num;i++){
        pml::pokepara::CoreParam * pp = party->GetMemberPointer(i);

        if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) ||
            p_fight_index_array[i] == false ){
          continue; //タマゴ,もしくは戦闘不参加ならスキップ
        }
        // 満腹度-1
        pp->SubFeed( UPDATE_FEED_AFTER_BATTLE );
        // なでなで度 -1
        pp->SubNadenadeValue( UPDATE_NADENADE_AFTER_BATTLE );
      }
    }

  } //namespace nakayoshi
} //namespace poke_tool

