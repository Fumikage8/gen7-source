//==============================================================================
/**
 * @file	macroDialog.h
 * @brief	
 * @author	kimura_shin
 * @data	2011/07/29, 15:59
 */
// =============================================================================

#ifndef MACRODIALOG_H
#define	MACRODIALOG_H

#include <rapidxml/gfl_Xml.h>

class CommandDialog{};

// =============================================================================
// =============================================================================

class ExtDialog : public QPushButton {
public:

	ExtDialog( const QString& label, const QString& name ) : QPushButton( label ), m_Name( name ) {
	}

	QString GetName( ) const {
		return m_Name;
	}
private:
	QString m_Name;
};

/**
 * 
 */
class ComboBox : public QComboBox {
  Q_OBJECT

public:

	ComboBox( void ) {
    mCutStrNumLeft = 0;
    mCutStrNumRight = 0;
    mMode = 0;
	}

	virtual ~ComboBox( void ) {
	}

  QStringList* GetLabelList( ) {
  	return &m_LabelList;
  }

  void SetCutStrNumLeft(int val)
  {
    mCutStrNumLeft = val;
  }
  void SetCutStrNumRight(int val)
  {
    mCutStrNumRight = val;
  }

  //0はCOMBOBOX
  //1はファイル系(サーチ有り)
  void SetMode(int val) { mMode = val; }
  int GetMode(void) { return mMode; }

public Q_SLOTS:
	void SearchText(const QString &str );

private:
  int mMode;

  int mCutStrNumLeft;	
  int mCutStrNumRight;	
	QStringList m_LabelList;
};

class DoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT
public:
  DoubleSpinBox()
    :QDoubleSpinBox()
  {
  }

  void SetSyncSwitch(QCheckBox *cb){mSyncSwitch = cb;}
  void SetOtherBox(DoubleSpinBox *db1,DoubleSpinBox *db2){mOtherBox[0] = db1;mOtherBox[1] = db2;}
public Q_SLOTS:
  void UpdateValue(double val);
private:
  QCheckBox *mSyncSwitch;
  DoubleSpinBox *mOtherBox[2];
};

// =============================================================================
// =============================================================================

class MacroDialog : public QDialog {
	Q_OBJECT

public:
	MacroDialog( QWidget* parent, gfl::xml::Node* node, gfl::xml::Node* load_node , gfl::xml::Node* define_node  , gfl::xml::Node* path_def_node );

public slots:
	void accept( );
	void rejected( );
	void callColor( );

	void Clicked( bool );

public Q_SLOTS:
  void SetSearchStr(const QString &str ){ mSearchStr = str;}

private:

	CommandDialog* GetCommandDialog( void ) {
		return (CommandDialog*) parent( );
	}

	c8* GetInitValue( gfl::xml::Node* node, gfl::xml::Node* load_node, const s32 id = 0 , bool isStr = false);

	QMap<QString, QList<QWidget*> > m_WidgetMap;
	gfl::xml::Node* m_pCommandNode;
	gfl::xml::Node* m_pLoadNode;

  //フレームとコメント
  QSpinBox *m_frame[2];
  QTextEdit *m_comment;

  static QString mSearchStr;
  QColor mCol;
  QLabel *mColLbl;

};

#endif	/* MACRODIALOG_H */
