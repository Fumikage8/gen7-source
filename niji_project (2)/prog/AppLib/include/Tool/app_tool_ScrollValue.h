//=============================================================================
/**
 * @brief スクロールする値の管理
 * @file app_tool_ScrollValue.h
 * @author obata_toshihiro
 * @date 2011.04.25
 */
//=============================================================================
#ifndef __APP_TOOL_SCROLL_VALUE_H__
#define __APP_TOOL_SCROLL_VALUE_H__

#include "AppLib/include/Tool/app_tool_Interpolation.h"

namespace app {
  namespace tool {

    /**
     * 減速計算用テンプレート
     * @param T 減速対象パラメータ
     * @return 減速結果パラメータ
     */
    template<class T> T ReduceScrollValue( T val )
    {
      return  val * 14 / 15;
    }

    class ScrollValue : public Interpolation
    {
      public:

        /**
         * @brief 動作を定義するデータ
         */
        struct Settings {
          f32 init_value;        // 初期値[valud]
          f32 min_value;         // 最小値[value]
          f32 max_value;         // 最大値[value]
          f32 default_decel;     // 減速度[value/frame*frame]
          bool overrun_enable;   // 限界値突破を許すかどうか
          u32 overrun_frame;     // 限界値突破時に収束するまでのフレーム数
        };

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         * @param settings  動作を定義するデータ
         */
        ScrollValue( const Settings& settings );

        /**
         * @brief デストラクタ
         */
        virtual ~ScrollValue();



        //---------------------------------------------------------------------
        // 毎フレームの更新
        //---------------------------------------------------------------------
        /**
         * @brief 1フレーム分だけ値を更新する
         */
        void UpdateScroll( void );

        /**
         * @brief 値の更新を停止する
         */
        void LockScroll( void );

        /**
         * @brief 値の更新を再開する
         */
        void UnlockScroll( void );

        /**
         * @brief 値の更新が停止しているかどうかを調べる
         * @retval true   更新が停止されている
         * @retval false  更新が有効
         */
        bool IsScrollLocked( void ) const;



        //---------------------------------------------------------------------
        // スクロールの値
        //---------------------------------------------------------------------
        /**
         * @brief スクロールの最小値を設定する
         */
        inline void SetMinValue( f32 value ) { m_min_value = value; }

        /**
         * @brief スクロールの最小値を取得する
         */
        inline f32 GetMinValue( void ) const { return m_min_value; }

        /**
         * @brief スクロールの最大値を設定する
         */
        inline void SetMaxValue( f32 value ) { m_max_value = value; }

        /**
         * @brief スクロールの最大値を取得する
         */
        inline f32 GetMaxValue( void ) const { return m_max_value; }

        /**
         * @brief スクロールの現在値を取得する
         */
        inline f32 GetCurrentValue( void ) const { return m_value; }

        /**
         * @brief スクロールの現在値を設定する
         * @param value  セットする値
         */
        void SetCurrentValue( f32 value );




        //---------------------------------------------------------------------
        // スクロールの開始
        //---------------------------------------------------------------------
        /**
         * @brief スクロールを開始する
         * @param init_speed  スクロールの初速度[value/frame]
         */
        void StartScroll( f32 init_speed );

        /**
         * @brief スクロールを開始する
         * @param init_speed      スクロールの初速度[value/frame]
         * @param end_value_unit  終点の最低単位[value]
         *
         * @note end_value_unit の整数倍の位置で停止します
         */
        void StartScroll( f32 init_speed, f32 end_value_unit );

        /**
         * @brief スクロールを開始する
         * @param end_value  終点の値
         * @param end_frame  終点までのフレーム数
         */
        void StartBindScroll( f32 end_value, u32 end_frame );

        /**
         * @brief 循環計算でのスクロールを開始する
         *        (任意の値aからbまでを+でも-でも到達できるようにする）
         * @param end_value  終点の値
         * @param end_frame  終点までのフレーム数
         * @param operate    加算の符号（+1か-1)
         */
        void StartBindScrollCircular( f32 end_value, u32 end_frame, int operate );




        //---------------------------------------------------------------------
        // スクロールの制御
        //---------------------------------------------------------------------
        /**
         * @brief スクロールを停止する
         * @note 即座に停止します
         */
        void StopScroll( void );

        /**
         * @brief スクロール停止リクエスト
         * @note end_value_unit の整数倍の位置で停止します
         */
        void StopRequest( u32 end_value_unit );

        /**
         * @brief スクロール停止リクエストを受けたかどうかを調べる
         * @retval true   リクエストを受けた
         * @retval false  リクエストを受けていない
         *
         * @note
         * StopRequest()を呼んだ直後から停止するまでの間, true を返します.
         */
        bool HaveStopRequest( void ) const;

        /**
         * @brief スクロール中かどうかを調べる
         * @retval true   スクロール中
         * @retval false  停止中
         */
        bool IsScroll( void ) const;

        /**
         * @brief 限界値を超えているかどうかを調べる
         * @retval true   超えている
         * @retval false  超えていない
         */
        bool IsOverrun( void ) const;




      private:

        enum State {
          STATE_STOP,         ///< 停止中
          STATE_MOVE,         ///< 等加速度運動によりスクロール中
          STATE_HERMITE,      ///< エルミート補間によりスクロール中
          STATE_CIRCULAR_DIV, ///< 数値循環方式のスクロール中( min-maxの間を+でも-でも向かう事ができる）
        };



        // 設定
        f32 m_min_value;              // スクロール最小値
        f32 m_max_value;              // スクロール最大値
        f32 m_default_accel;          // デフォルト加速度[value/frame*frame]
        bool m_overrun_enable;        // 限界値の突破を許すか
        u32 m_overrun_frame;          // 限界値を突破した際に収束するまでのフレーム数

        // 状態
        bool m_locked;                // スクロール動作が固定されているかどうか
        State m_state;                // 状態

        // スクロール共通パラメータ
        u32 m_now_frame;              // スクロール開始からの経過フレーム数
        u32 m_end_frame;              // スクロール終了フレーム数
        f32 m_end_value;              // スクロール先の値

        // 等加速度運動パラメータ
        f32 m_start_value;            // スクロール開始時の値
        f32 m_start_speed;            // スクロール開始時の初速度
        f32 m_accel;                  // スクロール中の加速度
        f32 m_value;                  // 現在値
        f32 m_speed;                  // 移動速度
        bool m_stop_request;          // 停止リクエストの有無
        f32 m_stop_value;             // 停止リクエストによる停止位置



        bool IsScrollStartEnable( f32 now_value, f32 init_speed ) const;
        void UpdateScrollValue( void );
        void UpdateScrollValue_MOVE( void );
        void UpdateScrollValue_HERMITE( void );
        void UpdateScrollValue_CIRCULAR( void );
        bool IsLimitOver( f32 value ) const;
        void StartOverAct( void );
        void StartScrollCore_V0_A( f32 init_speed, f32 accel );
        void StartScrollCore_V0( f32 init_speed, f32 accel, f32 end_value_unit );
        void StartScrollCore_X1_T( f32 end_value, u32 end_frame );
        void StartScrollCore_Circular( f32 end_value, u32 end_frame, int operate );
        void SetScrollParams(
            u32 end_frame, f32 end_value, f32 start_value, f32 start_speed, f32 accel, bool loopFlag = false );
        f32 CalcValue( f32 start_value, f32 start_speed, f32 accel, u32 frame ) const;
    };


  } // namespace tool
} // namespace app

#endif  // __APP_TOOL_SCROLL_VALUE_H__
