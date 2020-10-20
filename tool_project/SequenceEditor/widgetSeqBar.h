/*
 * File:   widgetSeqBar.h
 * Author: ariizumi_nobuhiko
 *
 * Created on 2011/02/24, 13:21
 */

#ifndef WIDGETSEQBAR_H
#define	WIDGETSEQBAR_H

#include "localDef.h"
#include "dataCommand.h"

typedef enum
{
  HOLD_NONE,
  HOLD_START,
  HOLD_END,
}SEQBAR_HOLD_STATE;

typedef enum
{
  MODE_BAR,
  MODE_COMMENT,
}SEQBAR_MODE_STATE;

//Seqバーの基底(他のプログラムでも使いまわせるように・・・)
class WidSequenceBar : public QWidget
{
  Q_OBJECT
  Q_PROPERTY(QColor ChipColor READ GetChipCol WRITE SetChipCol)

public:
  WidSequenceBar(DataEditor *data , QWidget *parent = NULL);

  QColor GetChipCol(){return mChipCol;}
  void SetChipCol(const QColor col){mChipCol = col;}
	
  int GetStartFrame(){return mStartFrame;}
  void SetStartFrame(const int frame){SetFrame( frame , mEndFrame );}

  int GetEndFrame(){return mEndFrame;}
  void SetEndFrame(const int frame){SetFrame(mStartFrame,frame);}

	virtual void SetFrame( const int start , const int end , bool isUpdate = true );

	int GetFrameLen(){return mEndFrame-mStartFrame;}
  int CalcFrameToPosX(const int frame);
  int CalcPosxToFrame(const int posX);

  void SetColor( QColor &col );
  virtual QColor& GetColor();

  void SetIsFocus(bool flg){mIsFocus = flg;}

  void SetMode( SEQBAR_MODE_STATE mode ){mMode = mode;}

  //virtual void SetMinMax( int min , int max ){ mMinFrame = min; mMaxFrame = max;}

  virtual const QString* GetDrawStr(void){return NULL;}

  virtual bool IsMoveLock(){return false;}

protected:
  //継承
  virtual void paintEvent(QPaintEvent *ev);
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseMoveEvent(QMouseEvent *ev);
  virtual void mouseReleaseEvent(QMouseEvent *ev);
  virtual QSize sizeHint() const;

Q_SIGNALS:
	void ChangeValue();

protected:
  virtual bool UpdateFrame();
  void DrawGrid(QRect *drawArea);
  void DrawBar( const QRect *rect,int start , int end );
  void DrawChip( int x , int y , int w , int h );

protected:

  QColor mChipCol;

  int mStartFrame;
  int mEndFrame;
  SEQBAR_HOLD_STATE mHoldState;
  SEQBAR_MODE_STATE mMode;

	DataEditor *mDataEditor;

  int mMinFrame;
  int mMaxFrame;
  
  bool mIsFocus;
};

//SeqバーのSeqEditor用
class WidSequenceBarCommand : public WidSequenceBar
{
  Q_OBJECT
public:
  WidSequenceBarCommand(DataEditor *editorData , DataCommand *comData ,QWidget *parent = NULL );
  virtual QColor& GetColor();

  virtual const QString* GetDrawStr(void);
protected:
  virtual void mousePressEvent(QMouseEvent *ev);
  virtual void mouseReleaseEvent(QMouseEvent *ev);
  virtual void mouseDoubleClickEvent ( QMouseEvent *ev );
  virtual bool UpdateFrame();

  virtual bool IsMoveLock();

  DataCommand	*mComData;
  int mBefStartFrame;
  int mBefEndFrame;

private:
  	
};

class WidSequenceBarCommand_OneFrame : public WidSequenceBarCommand
{
  Q_OBJECT
public:
  WidSequenceBarCommand_OneFrame(DataEditor *editorData , DataCommand *comData ,QWidget *parent = NULL );

	virtual void SetFrame( const int start , const int end , bool isUpdate = true );

protected:
private:
};


//SeqバーのSeqEditor用 複数のバーの集合(グループ描画用)
class WidSequenceBarList : public WidSequenceBar
{
  Q_OBJECT
public:
  WidSequenceBarList(DataEditor *editorData , DataComList* comList , QWidget *parent = NULL);

protected:
  //継承
  void paintEvent(QPaintEvent *ev);

	virtual void mousePressEvent(QMouseEvent *ev){}
  virtual void mouseMoveEvent(QMouseEvent *ev){}
  virtual void mouseReleaseEvent(QMouseEvent *ev){}

public Q_SLOTS:
	void SetOpenGroup(bool flg);

private:
	DataComList *mComList;
	bool mIsOpenGroup;
};


#endif	/* WIDGETSEQBAR_H */

