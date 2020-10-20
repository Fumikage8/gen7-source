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
  * @param   デバッグ用フォント 
  */
  //------------------------------------------------------------------
  void initialize(gfl2::heap::HeapBase* pHeap, u32 bufferSize, gfl2::util::DrawUtilText* pDrawUtilText);

  //------------------------------------------------------------------
  /**
  * @brief   描画
  * @return  なし
  */
  //------------------------------------------------------------------
  void draw(void);

  //------------------------------------------------------------------
  /**
  * @brief   更新
  * @return  なし
  */
  //------------------------------------------------------------------
  void update(void);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの作成リクエスト
  * @return  なし
  */
  //------------------------------------------------------------------
  void createEditorFromInstace(c16* windowName,const type_info* pTypeInfo,void* pObj);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの削除リクエスト
  * @return  なし
  */
  //------------------------------------------------------------------
  void deleteEditorFromInstace(void* pObj);

  //------------------------------------------------------------------
  /**
  * @brief   エディタの座標を設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void setPosition(float x,float y){ _editorPosX=x; _editorPosY=y; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの座標を取得
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void getPosition(float* pOutPosX,float* pOutPosY){ *pOutPosX=_editorPosX; *pOutPosY=_editorPosY; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタを描画するかどうか設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void setEnableDraw(bool isDraw){ _isDraw = isDraw; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタを描画するかどうか主とｒ区
  * @return  なし
  */
  //------------------------------------------------------------------
  inline bool isEnableDraw(void){ return _isDraw; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの行数を設定
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void setMaxLine(int maxLine){ _editorLine=maxLine; }

  //------------------------------------------------------------------
  /**
  * @brief   エディタの行数を取得
  * @return  なし
  */
  //------------------------------------------------------------------
  inline int getMaxLine(void){ return _editorLine; }

  //------------------------------------------------------------------
  /**
  * @brief   次のタグを表示
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void nextTag(void){ _tagControl = TAG_NEXT; }

  //------------------------------------------------------------------
  /**
  * @brief   前のタグを表示
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void prevTag(void){ _tagControl = TAG_PREV; }

  //------------------------------------------------------------------
  /**
  * @brief   次の項目を編集する
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void nextParam(void){ _paramControl = PARAM_NEXT; }

  //------------------------------------------------------------------
  /**
  * @brief   前の項目を編集する
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void prevParam(void){ _paramControl = PARAM_PREV; }

  //------------------------------------------------------------------
  /**
  * @brief   編集中の項目に対し値を増やす
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void addValue(void){ _valueControl = VALUE_ADD; }

  //------------------------------------------------------------------
  /**
  * @brief   編集中の項目に対し値を減らす
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void pullValue(void){ _valueControl = VALUE_PULL; }

  //------------------------------------------------------------------
  /**
  * @brief   addValue,pullValueを呼んだ場合に増減する値を設定(デフォルトは1.0f)
  *          ATTR_ADD_SMALLEST属性のプロパティへの場合は 0.01f  * _valueCoefficient 分が増減
  *          ATTR_ADD_SMALL   属性のプロパティへの場合は 0.1f   * _valueCoefficient 分が増減
  *          ATTR_ADD_BIG     属性のプロパティへの場合は 10.0f  * _valueCoefficient 分が増減
  *          ATTR_ADD_BIGGEST 属性のプロパティへの場合は 100.0f * _valueCoefficient 分が増減
  * @return  なし
  */
  //------------------------------------------------------------------
  inline void setValueAcceleration(float valueCoefficient){ _valueCoefficient = valueCoefficient; }

  //------------------------------------------------------------------
  /**
  * @brief   addValue,pullValueを呼んだ場合に増減する値を取得
  * @return  増減値
  */
  //------------------------------------------------------------------
  inline float getValueAcceleration(void){ return _valueCoefficient; }

private:

  //------------------------------------------------------------------
  /**
  * @brief   エディタに何も表示しない状態にする
  * @return  なし
  */
  //------------------------------------------------------------------
  void clearEditor(void);

  //------------------------------------------------------------------
  /**
  * @brief   書式付き出力をワイド文字配列にコピー
  * @return  なし
  */
  //------------------------------------------------------------------
  void copyWstr( wchar_t *pOut,int* pOutSize,size_t size,const wchar_t* wstr, ... );

  gfl2::heap::HeapBase*                               _pHeap;                   // リストに利用するヒープ
  u32                                                 _bufferSize;              // リストに利用するヒープサイズ
  gfl2::util::DrawUtilText*                           _pDrawUtilText;           // デバッグ用フォント  
  gfl2::util::List<EditorRequest>                     _editorRequestList;       // エディタの作成リクエストリスト
  gfl2::util::List<RttiParamManager::MemberProperty*> _dispMemberPropertyList;  // エディタに表示するメンバプロパティリスト

  unsigned int               _scrollCnt;                 // エディタの現在のスクロール状況
  float                      _editorPosX;                // エディタの座標X
  float                      _editorPosY;                // エディタの座標Y
  unsigned int               _editorLine;                // エディタの行数

  bool                       _isDraw;
  bool                       _isChangeRequestList;       // エディタの作成リクエストに変更があったかどうか
  TAG_CONTROL                _tagControl;                // タグ選択      の操作状況
  PARAM_CONTROL              _paramControl;              // パラメータ選択の操作状況
  VALUE_CONTROL              _valueControl;              // パラメータ値  の操作状況
  float                      _valueCoefficient;          // パラメータ値  の操作係数

  static const int           TEXT_MAX_SIZE = 64;         // エディタに表示できる名前のサイズ
  c16                        _textBuffer[TEXT_MAX_SIZE]; // ワイド文字列を扱うためのテンポラリ
};

} }

#endif
