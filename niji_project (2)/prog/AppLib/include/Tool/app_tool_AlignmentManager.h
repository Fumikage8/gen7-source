//========================================================================================
/**
 * @brief 整列配置マネージャ
 * @file app_tool_AlignmentManager.h
 * @author obata_toshihiro
 * @date 2011.04.18
 */
//======================================================================================== 
#ifndef __APP_TOOL_ALIGNMENT_MANAGER_H__
#define __APP_TOOL_ALIGNMENT_MANAGER_H__

#include <math/include/gfl2_Vec.h>
#include <heap/include/gfl2_Heap.h>

namespace app {
  namespace tool { 


    class AlignmentManager
    {
      GFL_FORBID_COPY_AND_ASSIGN( AlignmentManager );

      public:

        /**
         * @brief 移動の定義関数
         * @param current_frame  現在のフレーム数
         * @param max_frame      最終フレーム数
         * @return 移動の進み具合
         * @retval [0, 1]
         */
        typedef f32 (*MoveFunc)( u32 current_frame, u32 max_frame );

        /**
         * @brief 動作を定義する設定
         */
        struct Settings {
          u32 move_frame;                        // 移動開始から完了までのフレーム数
          u32 pos_count;                         // 管理する座標の数 
          const gfl2::math::VEC3* base_position;  // 配置座標の配列
          gfl2::math::VEC3* control_position;     // 操作する座標の配列 
          MoveFunc move_func;                    // 移動を定義した関数
        };

        //--------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param heap      使用するヒープ
         * @param settings  初期化パラメータ
         */
        //--------------------------------------------------------------------------------
        AlignmentManager( gfl2::heap::HeapBase* heap, const Settings& settings );

        //--------------------------------------------------------------------------------
        /**
         * @brief デストラクタ
         */
        //--------------------------------------------------------------------------------
        virtual ~AlignmentManager();




        // 毎フレームの更新処理

        //--------------------------------------------------------------------------------
        /**
         * @brief 全ての座標を1フレーム分更新する
         */
        //--------------------------------------------------------------------------------
        void UpdatePositions( void );

        //--------------------------------------------------------------------------------
        /**
         * @brief UpdatePositions()による座標の更新を停止する
         * @param order  何番目の座標か
         */
        //--------------------------------------------------------------------------------
        void LockPositionUpdate( u32 order );

        //--------------------------------------------------------------------------------
        /**
         * @brief UpdatePositions()による座標の更新を有効にする
         * @param order  何番目の座標か
         */
        //--------------------------------------------------------------------------------
        void UnlockPositionUpdate( u32 order );




        // 並べ替え

        //--------------------------------------------------------------------------------
        /**
         * @brief 配置場所を変更する
         * @param target_order  順番を変更する場所
         * @param aim_order     変更先の場所
         */
        //--------------------------------------------------------------------------------
        void ChangeOrder( u32 target_order, u32 aim_order );

        //--------------------------------------------------------------------------------
        /**
         * @brief 配置場所を入れ替える
         * @param order1  入れ替える並び順
         * @param order2  入れ替える並び順
         */
        //--------------------------------------------------------------------------------
        void ExchangeOrder( u32 order1, u32 order2 );




        // 状態の取得

        //--------------------------------------------------------------------------------
        /**
         * @brief 並び順を取得する
         * @param actor_index  対象の座標インデックス
         * @return 指定した座標は並び順で何番目に位置しているか
         */
        //--------------------------------------------------------------------------------
        u32 GetOrder( u32 actor_index ) const;

        //--------------------------------------------------------------------------------
        /**
         * @brief 指定した順番に位置するアクターのインデックスを取得する
         * @param order  並び順( 何番目に位置するアクターか )
         * @retval 0以上なら, 指定した位置に結びついているアクターのインデックス
         * @retval -1なら, アクターが結びついていない( 想定外のエラー )
         */
        //--------------------------------------------------------------------------------
        s32 GetActorIndexByOrder( u32 order ) const;

        //--------------------------------------------------------------------------------
        /**
         * @brief すべての動作が完了したかチェック
         *
         * @return true移動していない false移動中
         */
        //--------------------------------------------------------------------------------
        bool IsEndMove( void ) const;


        // 初期化

        //--------------------------------------------------------------------------------
        /**
         * @brief 座標・並び順を初期化する
         */
        //--------------------------------------------------------------------------------
        void ResetOrderAndPositions( void );



      private:

        /**
         * @brief 動作オブジェクト
         */
        struct Actor {
          bool move_flag;                       // 動いているかどうか
          bool lock_flag;                       // 動きがロックされているかどうか
          u32 move_frame_count;                 // 動き出してから何フレームか
          gfl2::math::VEC3* current_position;    // 現在の座標
          gfl2::math::VEC3 move_start_position;  // 動き出した座標
          u32 order;                            // 並び順( 何番目か )
        };

        Actor* m_actor;
        const Settings m_settings;


        /**
         * @brief アクターの座標を計算する
         * @param actor  座標を計算するアクター
         * @param dest   計算結果の格納先
         */
        void CalculateActorPosition( const Actor& actor, gfl2::math::VEC3* dest );

        /**
         * @brief アクターの移動を開始する
         * @param actor_index  アクターのインデックス
         */
        void StartActorMove( u32 actor_index );
    };


  } // namespace tool
} // namespace app


#endif  // __APP_TOOL_ALIGNMENT_MANAGER_H__
