//===========================================================================
/**
 * @file    RttiEditor.h
 * @brief   送られてきたRtti情報を解析しエディタを作成する
 * @date    2014.11.19
 * @author  yy-ikeuchi
 */
//===========================================================================
#pragma once

#include "RttiManager.h"
#include <util/include/gfl2_List.h>
#include <util/include/gfl2_Map.h>

namespace gfl2{ namespace util {
  class DrawUtilText;
}}

#if USE_RTTI_DEBUG

namespace gfl2{ namespace rttidebug{

class RttiEditor
{
public:

  //===========================================================================
  // エディタのリクエストプロパティ
  //===========================================================================
  struct EditorRequest
  {
    static const int DISP_NAME_SIZE = 24;

    //------------------------------------------------------------------
    /**
    * @brief   コンストラクタ
    */
    //------------------------------------------------------------------
    EditorRequest()
    : _pTypeInfo(NULL), _pInstance(NULL)
    {
      wideCharaCopy( _dispName, DISP_NAME_SIZE, L"default" );
    }

    //------------------------------------------------------------------
    /**
    * @brief   コピーコンストラクタ
    */
    //------------------------------------------------------------------
    EditorRequest(c16* dispName, const type_info* pTypeInfo, void* pInstance)
    : _pTypeInfo(pTypeInfo)
    , _pInstance(pInstance)
    {
      wideCharaCopy( _dispName, DISP_NAME_SIZE, dispName );
    }

    c16                   _dispName[DISP_NAME_SIZE]; // 表題
    const type_info*      _pTypeInfo;                // 表示したいクラスの型情報
    void*                 _pInstance;                // 表示したいクラスのインスタンス
  };

  //===========================================================================
  // カラー(他プロジェクトに依存しないようカラー構造体を定義しています)
  //===========================================================================
  struct ColorRGB
  {
    ColorRGB()
    : _r(0),_g(0),_b(0){}
    ColorRGB(float r, float g, float b)
    : _r(r),_g(g),_b(b){}

    float _r;
    float _g;
    float _b;
  };

  //===========================================================================
  // エディタ操作のリクエスト
  //===========================================================================

  // タグ選択の操作
  enum TAG_CONTROL
  {
    TAG_NONE,
    TAG_NEXT,
    TAG_PREV,
  };
  // パラメータ選択の操作
  enum PARAM_CONTROL
  {
    PARAM_NONE,
    PARAM_NEXT,
    PARAM_PREV,
  };
  // パラメータ値の操作
  enum VALUE_CONTROL
  {
    VALUE_NONE,
    VALUE_ADD,
    VALUE_PULL,
  };

  //------------------------------------------------------------------
  /**
  * @brief   シングルトンインスタンスを取得するアクセサ
  * @return  RTTIエディタへのポインタ
  */
  //------------------------------------------------------------------
  static RttiEditor* getInstance() 
  {
    static RttiEditor instance;
    return &instance;
  }

private:

  //------------------------------------------------------------------
  /**
  * @brief   コンストラクタ(使用禁止)
  */
  //------------------------------------------------------------------
  RttiEditor();
  RttiEditor(const RttiEditor &other){}

public:

  //------------------------------------------------------------------
  /**
  * @brief   初期化
  * @param   ヒープ
  * @param   バッファサイズ
  */
  //------------------------------------------------------------------
  void Initialize(gfl2::heap::HeapBase* pHeap, u32 bufferSize);

  //------------------------------------------------------------------
  /**
  * @brief   破棄
  */
  //------------------------------------------------------------------
  void Terminate(void);

  //------------------------------------------------------------------
  /**
  * @brief   描画
  * @return  なし
  */
  //------------------------------------------------------------------
  void Draw(void);

  //------------------------------------------------------------------
  /**
  * @brief   更新
  * @return  なし
  */
  //------------------------------------------------------------------
  void Update(void);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの作成リクエスト
  * @return  なし
  */
  //------------------------------------------------------------------
  void CreateEditorFromInstace(c16* windowName,const type_info* pTypeInfo,void* pObj);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの削除リクエスト
  * @return  なし
  */
  //------------------------------------------------------------------
  void DeleteEditorFromInstace(void* pObj);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの描画テキストを設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void SetDrawUtilText(gfl2::util::DrawUtilText* pDrawUtilText){ m_pDrawUtilText=pDrawUtilText; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの座標を設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void SetPosition(float x,float y){ m_editorPosX=x; m_editorPosY=y; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの座標を取得
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void GetPosition(float* pOutPosX,float* pOutPosY){ *pOutPosX=m_editorPosX; *pOutPosY=m_editorPosY; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタを描画するかどうか設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void SetEnableDraw(bool isDraw){ m_isDraw = isDraw; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタを描画するかどうか主とｒ区
  * @return  なし
  */
  //------------------------------------------------------------------
  inline bool IsEnableDraw(void){ return m_isDraw; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの行数を設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void SetMaxLine(int maxLine){ m_editorLine=maxLine; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの行数を取得
  * @return  なし
  */
  //------------------------------------------------------------------
  inline int GetMaxLine(void){ return m_editorLine; }

  //------------------------------------------------------------------
  /**
  * @brief   次のタグを表示
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void NextTag(void){ m_tagControl = TAG_NEXT; }

  //------------------------------------------------------------------
  /**
  * @brief   前のタグを表示
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void PrevTag(void){ m_tagControl = TAG_PREV; }

  //------------------------------------------------------------------
  /**
  * @brief   次の項目を編集する
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void NextParam(void){ m_paramControl = PARAM_NEXT; }

  //------------------------------------------------------------------
  /**
  * @brief   前の項目を編集する
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void PrevParam(void){ m_paramControl = PARAM_PREV; }

  //------------------------------------------------------------------
  /**
  * @brief   編集中の項目に対し値を増やす
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void AddValue(void){ m_valueControl = VALUE_ADD; }

  //------------------------------------------------------------------
  /**
  * @brief   編集中の項目に対し値を減らす
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void PullValue(void){ m_valueControl = VALUE_PULL; }

  //------------------------------------------------------------------
  /**
  * @brief   AddValue,PullValueを呼んだ場合に増減する値を設定(デフォルトは1.0f)
  *          ATTR_ADD_SMALLEST属性のプロパティへの場合は 0.01f  * m_valueCoefficient 分が増減
  *          ATTR_ADD_SMALL   属性のプロパティへの場合は 0.1f   * m_valueCoefficient 分が増減
  *          ATTR_ADD_BIG     属性のプロパティへの場合は 10.0f  * m_valueCoefficient 分が増減
  *          ATTR_ADD_BIGGEST 属性のプロパティへの場合は 100.0f * m_valueCoefficient 分が増減
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void SetValueAcceleration(float valueCoefficient){ m_valueCoefficient = valueCoefficient; }

  //------------------------------------------------------------------
  /**
  * @brief   AddValue,PullValueを呼んだ場合に増減する値を取得
  * @return  増減値
  */
  //------------------------------------------------------------------
  inline float GetValueAcceleration(void){ return m_valueCoefficient; }

private:

  //------------------------------------------------------------------
  /**
  * @brief   エディタに何も表示しない状態にする
  * @return  なし
  */
  //------------------------------------------------------------------
  void ClearEditor(void);

  //------------------------------------------------------------------
  /**
  * @brief   書式付き出力をワイド文字配列にコピー
  * @return  なし
  */
  //------------------------------------------------------------------
  void CopyWstr( wchar_t *pOut,int* pOutSize,size_t size,const wchar_t* wstr, ... );

  gfl2::heap::HeapBase*                               m_pHeap;                   // リストに利用するヒープ
  u32                                                 m_bufferSize;              // リストに利用するヒープサイズ
  gfl2::util::DrawUtilText*                           m_pDrawUtilText;           // デバッグ用フォント  
  gfl2::util::List<EditorRequest>                     m_editorRequestList;       // エディタの作成リクエストリスト
  gfl2::util::List<RttiParamManager::MemberProperty*> m_dispMemberPropertyList;  // エディタに表示するメンバプロパティリスト

  unsigned int               m_scrollCnt;                 // エディタの現在のスクロール状況
  float                      m_editorPosX;                // エディタの座標X
  float                      m_editorPosY;                // エディタの座標Y
  unsigned int               m_editorLine;                // エディタの行数

  bool                       m_isDraw;
  bool                       m_isChangeRequestList;       // エディタの作成リクエストに変更があったかどうか
  TAG_CONTROL                m_tagControl;                // タグ選択      の操作状況
  PARAM_CONTROL              m_paramControl;              // パラメータ選択の操作状況
  VALUE_CONTROL              m_valueControl;              // パラメータ値  の操作状況
  float                      m_valueCoefficient;          // パラメータ値  の操作係数

  static const int           TEXT_MAX_SIZE = 64;          // エディタに表示できる名前のサイズ
  c16                        m_textBuffer[TEXT_MAX_SIZE]; // ワイド文字列を扱うためのテンポラリ
};

} }

#endif
