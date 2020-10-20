//=============================================================================
/**
 * @file   gfl2_UI_TouchPanel.h
 * @brief  タッチパネルのアクセッサ
 * @author toshihiro_obata
 * @date   2010.10.22
 */
//=============================================================================
#ifndef __gfl2_UI_TOUCHPANEL_H__
#define __gfl2_UI_TOUCHPANEL_H__
#pragma once

#include <math/include/gfl2_MathType.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>
#include <ui/include/gfl2_UI_Types.h>
#include "ui/include/gfl2_UI_Device.h"


namespace gfl2 {
  namespace ui {

    class DeviceImplementer;


    class TouchPanel : public Device 
    {
      GFL_FORBID_COPY_AND_ASSIGN( TouchPanel );
      friend class DeviceManager;

      public:

      // 検出データ
      struct DetectiveData 
      {
        u16  x;      // タッチされているx座標
        u16  y;      // タッチされているy座標
        bool touch;  // タッチされているか？
      };

      // 実効データ
      struct EffectiveData 
      {
        u16 last_x;    // 最後にタッチされたx座標
        u16 last_y;    // 最後にタッチされたy座標
        u16 prev_x;    // 前回タッチされたx座標
        u16 prev_y;    // 前回タッチされたy座標
        bool hold;     // タッチされているか？
        bool trigger;  // タッチされたか？
        bool release;  // タッチが離されたか？
        bool repeat;   // タッチされ続けているか？

        EffectiveData( void ) : 
          last_x( 0 ),
          last_y( 0 ),
          prev_x( 0 ),
          prev_y( 0 ),
          hold( false ),
          trigger( false ),
          release( false ),
          repeat( false ) { }
      };

      //-----------------------------------------------------------------------
      // タッチ判定
      //-----------------------------------------------------------------------
      /**
       * @brief タッチされているかどうかを調べる
       * @retval true   タッチされている
       * @retval false  タッチされていない
       */
      bool IsTouch( void ) const;

      /**
       * @brief タッチされたかどうかを調べる
       * @retval true   タッチされた
       * @retval false  タッチされてない
       */
      bool IsTouchTrigger( void ) const;

      /**
       * @brief タッチが離されたかどうかを調べる
       * @retval true   タッチが離された
       * @retval false  タッチが離されていない
       */
      bool IsTouchRelease( void ) const;

      /**
       * @brief タッチのリピート入力を検出したかどうかを調べる
       * @retval true   リピート入力を検出した
       * @retval false  リピート入力を検出していない
       */
      bool IsTouchRepeat( void ) const;

      /**
       * @brief ダブルクリックを検出したかどうかを調べる
       * @retval true   検出した
       * @retval false  検出していない
       */
      bool IsDoubleClick( void ) const; 

      //-----------------------------------------------------------------------
      // 現在タッチされている座標の取得
      //-----------------------------------------------------------------------
      /**
       * @brief タッチされているx座標を取得する
       * @retval 現在タッチされているx座標( タッチされていない場合は 0 )
       */
      u16 GetX( void ) const;

      /**
       * @brief タッチされているy座標を取得する
       * @retval 現在タッチされているy座標( タッチされていない場合は 0 )
       */
      u16 GetY( void ) const;

      /**
       * @brief タッチされている座標を取得する
       * @param[out] out_x  取得したx座標の格納先
       * @param[out] out_y  取得したy座標の格納先
       */
      void GetXY( u16* out_x, u16* out_y ) const;

      /**
       * @brief タッチされている座標を取得する
       * @param[out] vec  取得した座標の格納先
       *
       * @note vec->z は変更しません。
       */
      void GetXY( gfl2::math::VEC3* vec ) const;

      //-----------------------------------------------------------------------
      // 前回タッチされていた座標の取得
      //-----------------------------------------------------------------------
      /**
       * @brief 前回タッチされたx座標を取得する
       * @retval 現在タッチされているx座標( タッチされていない場合は 0 )
       */
      u16 GetPrevX( void ) const;

      /**
       * @brief 前回タッチされたy座標を取得する
       * @retval 現在タッチされているy座標( タッチされていない場合は 0 )
       */
      u16 GetPrevY( void ) const;

      /**
       * @brief 前回タッチされた座標を取得する
       * @param[out] out_x  取得したx座標の格納先
       * @param[out] out_y  取得したy座標の格納先
       */
      void GetPrevXY( u16* out_x, u16* out_y ) const;

      /**
       * @brief 前回タッチされた座標を取得する
       * @param[out] vec  取得した座標の格納先
       *
       * @note vec->z は変更しません。
       */
      void GetPrevXY( gfl2::math::VEC3* vec ) const;

      //-----------------------------------------------------------------------
      // タッチ座標の変化量を取得する
      //-----------------------------------------------------------------------
      /**
       * @brief タッチ座標のx変化量を取得する
       * @return 前回からのx座標の変化量
       *
       * @note
       * タッチされていない間の入力は座標(0,0)として扱われています.
       * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
       */
      int GetMoveX( void ) const;

      /**
       * @brief タッチ座標のy変化量を取得する
       * @return 前回からのy座標の変化量
       *
       * @note
       * タッチされていない間の入力は座標(0,0)として扱われています.
       * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
       */
      int GetMoveY( void ) const;

      /**
       * @brief タッチ座標の変化量を取得する
       * @param[out] out_x  x方向の変化量の格納先
       * @param[out] out_y  y方向の変化量の格納先
       *
       * @note
       * タッチされていない間の入力は座標(0,0)として扱われています.
       * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
       */
      void GetMoveXY( int* out_x, int* out_y ) const;

      /**
       * @brief タッチ座標の変化量を取得する
       * @param vec  取得した変化量の格納先
       *
       * @note
       * タッチされていない間の入力は座標(0,0)として扱われています.
       * よって, タッチされた瞬間は(0,0)からの変化量(=現在値)が返ります.
       */
      void GetMoveXY( gfl2::math::VEC3* vec ) const; 

      //-----------------------------------------------------------------------
      // ダブルクリックの設定
      //-----------------------------------------------------------------------
     
      // ダブルクリックとみなす領域の形
      enum DoubleClickRange {
        DCLICK_RANGE_RECT,    // 矩形( デフォルト )
        DCLICK_RANGE_CIRCLE,  // 円形
      };

      // ダブルクリックの判定条件
      struct DoubleClickCond 
      {
        DoubleClickRange range;  // 判定領域の形
        u32 radius;              // 判定領域の大きさ( RECT:一辺の長さの1/2, CIRCLE:半径 )
        u32 frame;               // クリック間のフレーム数
      };

      // デフォルトの設定
      static const u32 DEFAULT_DCLICK_RADIUS = 20;
      static const u32 DEFAULT_DCLICK_FRAME  = 20; 
      static const DoubleClickCond DEFAULT_DCLICK_COND;

      /**
       * @brief ダブルクリックの条件を設定する
       * @param cond 設定する条件
       */
      void SetDoubleClickCond( const DoubleClickCond& cond );

      /**
       * @brief ダブルクリックの条件を取得する
       */
      const DoubleClickCond& GetDoubleClickCond( void ) const;



      protected: 

      //-----------------------------------------------------------------------
      // コンストラクタ・デストラクタ
      //-----------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap         使用するヒープ
       * @param implementer  実際にタッチパネル入力の読み取りを行うインスタンス
       */
      TouchPanel( gfl2::heap::HeapBase* heap, DeviceImplementer* implementer );

      /**
       * @brief デストラクタ
       */
      virtual ~TouchPanel(); 

      //-----------------------------------------------------------------------
      // デバイスとしての動作を定義するメソッド
      //-----------------------------------------------------------------------
      /**
       * @brief 現在の入力と前回の入力を比較し, 同じデータが続いているかどうかを調べる
       * @retval true   前回と今回の入力は同じ
       * @retval false  前回と今回の入力は異なる
       */
      virtual bool IsRepeatingSameInput( void ) const;

      /**
       * @brief 実効データを蓄積する
       * @param[out] buffer               データの格納先
       * @param[in]  detective_data       今回の検出データ
       * @param[in]  prev_detective_data  前回の検出データ
       */
      virtual void StoreEffectiveData( 
          void* buffer,
          const void* detective_data,
          const void* prev_detective_data ) const;

      //-----------------------------------------------------------------------
      // タッチパネル固有の更新処理
      //-----------------------------------------------------------------------
      /**
       * @brief 更新処理
       */
      void UpdateTouchPanel( void );



      private: 

      struct DoubleClickState 
      {
        bool detected;   // ダブルクリックを検出したか？
        bool first;      // 1クリック目を検出済みか？
        u16 first_x;     // 1クリック目のx座標
        u16 first_y;     // 1クリック目のy座標
        u32 count;       // ダブルクリックとみなす残りフレーム数
      };

      DoubleClickCond m_dclick_cond;
      DoubleClickState m_dclick_state;

      void UpdateDClickState( void );
      bool CheckDClickRange( u16 x, u16 y ) const; 
      bool m_update_flag;//30,60をきりかえるためのフラグ

      const EffectiveData& GetEffectiveData( void ) const;

    };


  }  //namespace ui
}  //namespace gfl2

#endif //__gfl2_UI_TOUCHPANEL_H__
