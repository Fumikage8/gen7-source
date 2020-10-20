//=============================================================================
/**
 * @brief キーリピートの挙動を制御するクラス
 * @file util_KeyRepeatController.h
 * @author 
 * @date 
 */
//=============================================================================
#ifndef __APP_UTIL_KEY_REPEAT_CONTROLLER_H__
#define __APP_UTIL_KEY_REPEAT_CONTROLLER_H__

#include <ui/include/gfl2_UI_VectorDevice.h>
#include <ui/include/gfl2_UI_Button.h>

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( util )

class KeyRepeatController
{
	GFL_FORBID_COPY_AND_ASSIGN(KeyRepeatController);
  public: 

    //-------------------------------------------------------------------------
    // コンストラクタ・デストラクタ
    //-------------------------------------------------------------------------
    /**
      * @brief コンストラクタ
      */
    KeyRepeatController( u32 cursorPosMaxY , u32 cursorPosMaxX );
    /**
      * @brief デストラクタ
      */
    virtual ~KeyRepeatController();

    //-------------------------------------------------------------------------
    /**
      * @brief マックスを再設定
      */
    //-------------------------------------------------------------------------
    void SetMaxPos( u32 cursorPosMaxY ,  u32 cursorPosMaxX );

    //-------------------------------------------------------------------------
    /**
      * @brief 判定用情報更新
      */
    //-------------------------------------------------------------------------
    void UpdateInfo( gfl2::ui::VectorDevice * key , u32 cursorPosY , u32 cursorPosX , gfl2::ui::Button * button = NULL );

    //-------------------------------------------------------------------------
    /**
      * @brief リピート判定取得
      */
    //-------------------------------------------------------------------------
    bool IsRepeat( u32 dir );

  private:
    gfl2::ui::VectorDevice* m_pKey;
    gfl2::ui::Button * m_pButton;//LRボタンを左右と同期するためのボタン
    u32 m_cursorPosX;
    u32 m_cursorPosY;
    u32 m_cursorPosMaxX;
    u32 m_cursorPosMaxY;
};


GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )


#endif  // __APP_UTIL_KEY_REPEAT_CONTROLLER_H__
