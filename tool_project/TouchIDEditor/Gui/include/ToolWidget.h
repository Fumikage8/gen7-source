//==============================================================================
/**
 * @file ToolWidget.h
 * @brief
 * @author kimura_shin
 * @data 2012/03/21, 15:41
 */
// =============================================================================

#if !defined( __TOOLWIDGET_H__ )
#define __TOOLWIDGET_H__

#include <gflib.h>

/**
 * Icon Button
 */
class IconButton : public QPushButton {
public:

  IconButton( const QString& name, const s32 id ) : QPushButton(QIcon(name),""), m_Id(id) 
  {
  }

  virtual ~IconButton( void ) 
  {
  }

  s32 GetId( ) const 
  {
    return m_Id;
  }

private:
  s32 m_Id;
};


/**
 * Tool Widget
 */
class ToolWidget : public QGroupBox {
  Q_OBJECT

public:
  enum {
    TOOL_NOT_SELECT=-1,
    TOOL_POINT,
    TOOL_BRUSH,
    TOOL_BUCKET,
    TOOL_SYRINGE,
    TOOL_SELECT,
    TOOL_MAX
  };

  ToolWidget( void );
  virtual ~ToolWidget( void );

  /**
   * 現在選択されているツールを取得
   * @return TOOL_* 
   */
  s32 GetDevice( ) const 
  {
    return m_Device;
  }

  /**
   * 線の太さを取得
   * @return 線の太さ
   */
  s32 GetLine()
  {
    return m_pLine->currentText().toInt();
  }

  /**
   * 許容値を取得
   * @return 許容値
   */
  s32 GetTolerance()
  {
    return m_pTolerance->currentIndex();
  }

  /**
   * 貫通塗りさせるか
   * @return 0 貫通しない　0以外　貫通する
   */
  Qt::CheckState CheckPenetrate()
  {
    return m_pPenetrate->checkState();
  }

  void SetPenetrateBySetting();

private:

  IconButton* m_pDevice[TOOL_MAX];
  QComboBox* m_pLine;
  QComboBox* m_pTolerance;
  QCheckBox* m_pPenetrate;
  s32 m_Device;

  void SetPenetrate(s32 val);

  public slots:
    void Clicked();
    void LineChanged(int);
    void ToleranceChanged(int);
    void PenetrateChanged(int);
};


#endif /* __TOOLWIDGET_H__ */
