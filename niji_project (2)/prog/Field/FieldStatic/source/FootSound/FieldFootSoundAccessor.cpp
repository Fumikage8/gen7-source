//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldFootSoundAccessor.cpp
 *	@brief  足音データへのアクセサー
 *	@author	 tomoya takahashi
 *	@date		2015.07.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <Debug/include/gfl2_Assert.h>
#include <Debug/include/gfl2_DebugPrint.h>

#include "Field/FieldStatic/include/FootSound/FieldFootSoundAccessor.h"

#include "Sound/include/Sound.h"

#include "niji_conv_header/sound/SoundMiddleID.h" // SMID_NULL
#include "niji_conv_header/field/attribute/attribute_id.h" // ATTRIBUTE_MAX

GFL_NAMESPACE_BEGIN(Field)

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

/**
 * @class キーフレーム情報
 */
class FootSoundKeyFrame
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FootSoundKeyFrame );
public:
  /**
   *  @brief コンストラクタ
   */
  FootSoundKeyFrame() : 
     m_cpData(NULL)
    ,m_cpSeTable(NULL)
  {
  }

  /**
   *  @brief デストラクタ
   */
  virtual ~FootSoundKeyFrame()
  {
  }

  /**
   *  @brief 初期化
   */
  void Initialize( const void * cpBuff, const u32* cpSeTable )
  {
    m_cpData = reinterpret_cast<const KeyValue*>(cpBuff);
    m_cpSeTable = cpSeTable;
  }

  /**
   *  @brief 該当フレームのSEインデックスを取得
   */
  u32 GetFrameSeTable( u32 frame ) const
  {
    u8 u8_frame = static_cast<u8>(frame);

    for( u32 i=0; i<FS_KEY_MAX; ++i )
    {
      //TOMOYA_PRINT("frame %d value %d check frame %d\n", m_cpData->frame[i], m_cpData->value[i], u8_frame);
      if( (m_cpData->frame[i] != 0xFF) && 
        ((m_cpData->value[i] != 0xFF) && (m_cpData->value[i] != 0)) )  // 両方0xFFは無効 value は0でも無効
      {
        if( m_cpData->frame[i] == u8_frame )
        {
          if( (m_cpData->value[i] >= 1) && (m_cpData->value[i] <= FS_SE_LABEL_MAX) )
          {
            return m_cpSeTable[m_cpData->value[i] - 1];
          }
        }
      }
    }

    return SMID_NULL;
  }

public:
  /**
   *  @brief 足音キー情報
   */
  struct KeyValue
  {
    u8 frame[ FS_KEY_MAX ];
    u8 value[ FS_KEY_MAX ];
  };

private:
  const KeyValue* m_cpData;  ///< キー情報
  const u32 * m_cpSeTable;            ///< 再生するSEテーブル
};


/**
 *  @class FootSoundController
 *  @brief 足音のコントローラー
 */
class FootSoundController
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FootSoundController );
public:
  /**
   *  @brief コンストラクタ
   */
  FootSoundController( const void * cpBuff, bool is_man ) : 
     m_cpBuff(reinterpret_cast<const FootSound*>(cpBuff))
    ,m_isMan(is_man)
  {
  }

  /**
   *  @brief デストラクタ
   */
  virtual ~FootSoundController()
  {
  }

  /**
   *  @brief SEの再生
   *
   *  @param   before         ビフォアフレーム
   *  @param   after          アフターフレーム
   *  @param   animation_id   アニメーションID
   *  @param   attr           足元のアトリビュート
   */
  void Update( f32 before, f32 after, u32 animation_id, u32 attr ) const
  {
    u32 fs_se_index, fs_motion_index;
    const u32* cp_seLabel;
    const FootSoundKeyFrame::KeyValue* cp_keyValue;

    //TOMOYA_PRINT( "animation %d\n", animation_id );
    // animation_idが一致するデータがあるか取得
    if( !GetSeIndexAndMotionIndex( animation_id, &fs_se_index, &fs_motion_index ) )
    {
      return ;
    }

    // SEラベル取得
    //TOMOYA_PRINT( "attr %d\n", attr );
    if( !GetSeTable( attr, fs_se_index, &cp_seLabel ) )
    {
      return ;
    }

    // KeyValeuの取得
    //TOMOYA_PRINT( "man %d\n", m_isMan );
    
    if( !GetKeyValue( m_isMan, fs_se_index, fs_motion_index, &cp_keyValue ) )
    {
      return ;
    }

    // 再生
    FootSoundKeyFrame keyFrame;
    keyFrame.Initialize( cp_keyValue, cp_seLabel );

    s32 s32_after = (s32)(after);
    s32 s32_before = (s32)(before);
    s32 step = (before < after) ? 1 : -1;

    //TOMOYA_PRINT("before %f after %f\n", before, after);
    if( step > 0 )
    {
      for( s32 frame = s32_before + 1; frame <= s32_after; frame +=step )
      {
        //TOMOYA_PRINT("frame %d\n", frame);
        u32 seLabel = keyFrame.GetFrameSeTable((u8)(frame));
        if( seLabel != SMID_NULL )
        {
          //TOMOYA_PRINT("SEPlay %d\n", seLabel);
          Sound::PlaySE(seLabel);
        }
      }
    }
    else
    {
      for( s32 frame = s32_before - 1; frame >= s32_after; frame +=step )
      {
        //TOMOYA_PRINT("frame %d\n", frame);
        u32 seLabel = keyFrame.GetFrameSeTable((u8)(frame));
        if( seLabel != SMID_NULL )
        {
          //TOMOYA_PRINT("SEPlay %d\n", seLabel);
          Sound::PlaySE(seLabel);
        }
      }
    }
  }



private:

  /**
   *  @brief アニメーションIDからSEインデックス(歩き、走り等）と　モーションインデックス（その中の何番目のモーションか）を取得
   */
  bool GetSeIndexAndMotionIndex( u32 animation_id, u32 * p_fs_se_index, u32 * p_fs_motion_index ) const
  {
    for( u32 i=0; i<FS_SE_NUM; ++i )
    {
      for( u32 j=0; j<FOOT_SOUND_MOTION_MAX; ++j )
      {
        if( m_cpBuff->animationTable[i][j] == animation_id )
        {
          *p_fs_se_index = i;
          *p_fs_motion_index = j;
          return true;
        }
      }
    }
    return false;
  }

  /**
   *  @brief アトリビュートから特定のSEテーブルを取得する。
   */
  bool GetSeTable( u32 attr, u32 fs_se_index, const u32 ** pp_se_table ) const
  {
    // 逆順に抽選（Defaultが最後になるように）
    for( u32 i=m_cpBuff->num - 1; i>= 0; --i )
    {
      for( u32 j=0; j<FS_ATTR_MAX; ++j )
      {
        if( (m_cpBuff->fs_data[i].attr[j] == attr) || (m_cpBuff->fs_data[i].attr[j] == ATTRIBUTE_MAX) )
        {
          if( fs_se_index < FS_SE_NUM )
          {
            *pp_se_table = m_cpBuff->fs_data[i].seTable[fs_se_index];
            return true;
          }
        }
      }
    }
    return false;
  }

  /**
   *  @brief 男、女の指定とSEインデックス、モーションインデックスからKeyValue　を取得
   */
  bool GetKeyValue( bool is_man, u32 fs_se_index, u32 fs_motion_index, const FootSoundKeyFrame::KeyValue** cpp_keyvalue ) const
  {
    if( (fs_se_index < FS_SE_NUM) && (fs_motion_index < FOOT_SOUND_MOTION_MAX) )
    {
      if( is_man )
      {
        *cpp_keyvalue = &m_cpBuff->manKeyFrame[fs_se_index][fs_motion_index];
      }
      else
      {
        *cpp_keyvalue = &m_cpBuff->womanKeyFrame[fs_se_index][fs_motion_index];
      }
      return true;
    }

    return false;
  }


private:

  /** 
   *  @brief 足音アトリビュートデータ
   */
  struct FootSoundAttrData
  {
    u16 attr[FS_ATTR_MAX];								// 対応するアトリビュートテーブル
    u32 seTable[FS_SE_NUM][FS_SE_LABEL_MAX];		// 足音タイプごとのSEテーブル
  };

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)
  struct FootSound
  {
    u32 animationTable[FS_SE_NUM][FOOT_SOUND_MOTION_MAX];         // 足音タイプごとの対応アニメーションテーブル
    FootSoundKeyFrame::KeyValue manKeyFrame[FS_SE_NUM][FOOT_SOUND_MOTION_MAX];		// 足音タイプごとのキー情報テーブルの参照インデックス
    FootSoundKeyFrame::KeyValue womanKeyFrame[FS_SE_NUM][FOOT_SOUND_MOTION_MAX];	// 足音タイプごとのキー情報テーブルの参照インデックス
    u32 num;
    FootSoundAttrData fs_data[];  // 足音タイプごとのデータ 要素数はnum分
  };
GFL_WARNING_WIN32_POP

private:

  const FootSound * m_cpBuff;
  bool m_isMan;

};

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
/**
 *  @brief コンストラクタ
 */
FootSoundAccessor::FootSoundAccessor() : 
   m_cpBuff(NULL)
  ,m_isMan(true)
{
}

/**
 *  @brief デストラクタ
 */
FootSoundAccessor::~FootSoundAccessor()
{
}

/**
 *  @brief 初期化
 *  @param  cpBuff バッファ
 *  @param  is_man  男か　　true:男 false:女
 */
void FootSoundAccessor::Initialize( const void * cpBuff, bool is_man )
{
  m_cpBuff = cpBuff;
  m_isMan  = is_man;
}

/**
 *  @brief SEの再生
 *
 *  @param   before         ビフォアフレーム
 *  @param   after          アフターフレーム
 *  @param   animation_id   アニメーションID
 *  @param   attr           足元のアトリビュート
 */
void FootSoundAccessor::Update( f32 before, f32 after, u32 animation_id, u32 attr ) const
{
  FootSoundController controller( m_cpBuff, m_isMan );
  controller.Update( before, after, animation_id, attr );
}


GFL_NAMESPACE_END(Field)
