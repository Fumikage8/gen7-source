#ifndef __POWERSLIDER_H__
#define __POWERSLIDER_H__


#include <QtGui/QWidget>
#include <QtDesigner/QDesignerExportWidget>

#define USE_QDOM 0
#if USE_QDOM
#include <QtXml/QDomElement>
#endif

class QSlider;
class QDoubleSpinBox;

//class QDESIGNER_WIDGET_EXPORT  PowerSlider : public QWidget // for dll?
class PowerSlider : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double Min READ minimum WRITE setMinimum);
    Q_PROPERTY(double Max READ maximum WRITE setMaximum);
    Q_PROPERTY(double Value READ value WRITE setValue);
    Q_PROPERTY(double SingleStep READ singleStep WRITE setSingleStep);
public:
    PowerSlider(QWidget *parent=0, double alignmentValue=100.0);
    virtual ~PowerSlider();
#if USE_QDOM
    QDomElement 	domElement (const QString &name, QDomDocument &document) const;
    void 	initFromDOMElement (const QDomElement &element);
#endif
    double value();
    double minimum();
    double maximum();
    double singleStep();

	// ’Ç‰Á
	QDoubleSpinBox* spinBox( void ) {
		return spinBox_;
	}
	QSlider* slider( void ){
		return slider_;
	}
public slots:
    void setSingleStep(double);
    void setRange(double, double);
    void setMinimum(double);
    void setMaximum(double);
               
    void setValue(double);
signals:
    void valueChanged(double);
    
private:
    QSlider *slider_;
    QDoubleSpinBox *spinBox_;
public slots:
    void on_slider_valueChanged(int);
    void on_spinBox_valueChanged(double);	  
};

#endif // __POWERSLIDER_H__
