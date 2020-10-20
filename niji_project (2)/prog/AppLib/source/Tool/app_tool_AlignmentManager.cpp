//========================================================================================
/**
 * @brief 整列マネージャ
 * @file app_tool_AlignmentManager.cpp
 * @author obata_toshihiro
 * @date 2011.04.18
 */
//======================================================================================== 
#include "AppLib/include/Tool/app_tool_AlignmentManager.h"

#include <debug/include/gfl2_assert.h>

namespace app {
  namespace tool { 


    //------------------------------------------------------------------------------------
    // コンストラクタ
    //------------------------------------------------------------------------------------
    AlignmentManager::AlignmentManager( gfl2::heap::HeapBase* heap, const Settings& settings ) :
      m_actor( NULL ),
      m_settings( settings )
    {
      m_actor = GFL_NEW( heap ) Actor[ m_settings.pos_count ];
      for( u32 i=0; i<m_settings.pos_count; i++ ) {
        m_actor[i].lock_flag = false;
        m_actor[i].move_frame_count = 0;
        m_actor[i].current_position = &m_settings.control_position[i];
        m_actor[i].order = i;
      }
    }


    //------------------------------------------------------------------------------------
    // デストラクタ
    //------------------------------------------------------------------------------------
    AlignmentManager::~AlignmentManager()
    {
      GFL_DELETE_ARRAY( m_actor );
    }






    //------------------------------------------------------------------------------------
    // 座標・並び順を初期化する
    //------------------------------------------------------------------------------------
    void AlignmentManager::ResetOrderAndPositions( void )
    {
      for( u32 i=0; i<m_settings.pos_count; i++ ) {
        m_actor[i].move_flag = false;
        m_actor[i].move_frame_count = 0;
        *( m_actor[i].current_position ) = m_settings.base_position[i];
        m_actor[i].order = i;
      }
    }





    //------------------------------------------------------------------------------------
    // 全ての座標を1フレーム分更新する
    //------------------------------------------------------------------------------------
    void AlignmentManager::UpdatePositions( void )
    {
      for( u32 i=0; i<m_settings.pos_count; i++ ) 
      {
        if( m_actor[i].lock_flag == false && m_actor[i].move_flag ) {
          this->CalculateActorPosition( m_actor[i], m_actor[i].current_position ); 
          m_actor[i].move_frame_count++;
          if( m_settings.move_frame < m_actor[i].move_frame_count ) {
            m_actor[i].move_flag = false;
          }
        }
      }
    }

    // アクターの座標を計算する
    void AlignmentManager::CalculateActorPosition( const Actor& actor, gfl2::math::VEC3* dest )
    {
      const gfl2::math::VEC3& next_pos = m_settings.base_position[ actor.order ];
      const gfl2::math::VEC3& prev_pos = actor.move_start_position;

      f32 next_ratio = m_settings.move_func( actor.move_frame_count, m_settings.move_frame );
      *(dest) = ( next_pos * next_ratio ) + ( prev_pos * ( 1.0f - next_ratio ) );
    }



    //------------------------------------------------------------------------------------
    // 配置場所を変更する
    //------------------------------------------------------------------------------------
    void AlignmentManager::ChangeOrder( u32 target_order, u32 aim_order )
    {
      if( target_order == aim_order ) {
        return;
      }

      s32 actor_index = this->GetActorIndexByOrder( target_order );
      if( actor_index < 0 ) {
        return;
      } 

      // 押し出される
      u32 head_index = ( m_actor[actor_index].order < aim_order ) ? ( m_actor[actor_index].order + 1 ) : aim_order;
      u32 tail_index = ( m_actor[actor_index].order < aim_order ) ? aim_order : ( m_actor[actor_index].order - 1 );
      s32 move_dir = ( m_actor[actor_index].order < aim_order ) ? ( -1 ) : ( 1 ); 
      for( u32 i=0; i<m_settings.pos_count; i++ ) {
        if( (head_index <= m_actor[i].order) && (m_actor[i].order <= tail_index) ) {
          m_actor[i].order += move_dir;
          this->StartActorMove(i);
        }
      }

      // 移動する
      m_actor[actor_index].order = aim_order;
      this->StartActorMove( actor_index );
    }

    // アクターの移動を開始する
    void AlignmentManager::StartActorMove( u32 actor_index )
    {
      m_actor[actor_index].move_flag = true;
      m_actor[actor_index].move_frame_count = 0;
      m_actor[actor_index].move_start_position = *m_actor[actor_index].current_position;
    } 



    //------------------------------------------------------------------------------------
    // 配置場所を入れ替える
    //------------------------------------------------------------------------------------
    void AlignmentManager::ExchangeOrder( u32 order1, u32 order2 )
    {
      for( u32 i=0; i<m_settings.pos_count; i++ ) {
        if( m_actor[i].order == order1 ) {
          m_actor[i].order = order2;
          this->StartActorMove(i);
        }
        else if( m_actor[i].order == order2 ) {
          m_actor[i].order = order1;
          this->StartActorMove(i);
        }
      }
    }



    //------------------------------------------------------------------------------------
    // UpdatePositions()による座標の更新を停止する
    //------------------------------------------------------------------------------------
    void AlignmentManager::LockPositionUpdate( u32 order )
    {
      s32 actor_index = this->GetActorIndexByOrder( order );
			if( actor_index < 0 ){
				GFL_ASSERT( 0 );
				return;
			}
      m_actor[actor_index].lock_flag = true; 
    }


    //------------------------------------------------------------------------------------
    // UpdatePositions()による座標の更新を有効にする
    //------------------------------------------------------------------------------------
    void AlignmentManager::UnlockPositionUpdate( u32 order )
    {
      s32 actor_index = this->GetActorIndexByOrder( order );
			if( actor_index < 0 ){
				GFL_ASSERT( 0 );
				return;
			}
      m_actor[actor_index].lock_flag = false;
      this->StartActorMove( actor_index );
    } 



    //------------------------------------------------------------------------------------
    // 並び順を取得する
    //------------------------------------------------------------------------------------
    u32 AlignmentManager::GetOrder( u32 actor_index ) const
    {
      return m_actor[ actor_index ].order;
    }


    //------------------------------------------------------------------------------------
    // 基点位置のインデックスから, アクターのインデックスを取得する
    //------------------------------------------------------------------------------------
    s32 AlignmentManager::GetActorIndexByOrder( u32 order ) const
    {
      for( u32 i=0; i<m_settings.pos_count; i++ ) {
        if( m_actor[i].order == order ) {
          return i;
        }
      }
      GFL_ASSERT(0);
      return -1;
    }
    

    //--------------------------------------------------------------------------------
    /**
     * @brief すべての動作が完了したかチェック
     *
     * @return true移動していない false移動中
     */
    //--------------------------------------------------------------------------------
    bool AlignmentManager::IsEndMove( void ) const
    {
      bool is_end = true;
      for( u32 i=0; i<m_settings.pos_count; ++i ) {
        is_end &= (!m_actor[i].move_flag);
      }
      return is_end;
    }



  } // namespace tool
} // namespace app
