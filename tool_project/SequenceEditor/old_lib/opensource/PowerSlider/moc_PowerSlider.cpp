/****************************************************************************
** Meta object code from reading C++ file 'PowerSlider.h'
**
** Created: Fri Jul 22 12:36:12 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PowerSlider.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PowerSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PowerSlider[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       4,   54, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   12,   12,   12, 0x0a,
      58,   56,   12,   12, 0x0a,
      82,   12,   12,   12, 0x0a,
     101,   12,   12,   12, 0x0a,
     120,   12,   12,   12, 0x0a,
     137,   12,   12,   12, 0x0a,
     165,   12,   12,   12, 0x0a,

 // properties: name, type, flags
     204,  197, 0x06095003,
     208,  197, 0x06095003,
     212,  197, 0x06095103,
     218,  197, 0x06095103,

       0        // eod
};

static const char qt_meta_stringdata_PowerSlider[] = {
    "PowerSlider\0\0valueChanged(double)\0"
    "setSingleStep(double)\0,\0setRange(double,double)\0"
    "setMinimum(double)\0setMaximum(double)\0"
    "setValue(double)\0on_slider_valueChanged(int)\0"
    "on_spinBox_valueChanged(double)\0double\0"
    "Min\0Max\0Value\0SingleStep\0"
};

const QMetaObject PowerSlider::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_PowerSlider,
      qt_meta_data_PowerSlider, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PowerSlider::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PowerSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PowerSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PowerSlider))
        return static_cast<void*>(const_cast< PowerSlider*>(this));
    return QWidget::qt_metacast(_clname);
}

int PowerSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: setSingleStep((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 2: setRange((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 3: setMinimum((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 4: setMaximum((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 5: setValue((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 6: on_slider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: on_spinBox_valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = minimum(); break;
        case 1: *reinterpret_cast< double*>(_v) = maximum(); break;
        case 2: *reinterpret_cast< double*>(_v) = value(); break;
        case 3: *reinterpret_cast< double*>(_v) = singleStep(); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setMinimum(*reinterpret_cast< double*>(_v)); break;
        case 1: setMaximum(*reinterpret_cast< double*>(_v)); break;
        case 2: setValue(*reinterpret_cast< double*>(_v)); break;
        case 3: setSingleStep(*reinterpret_cast< double*>(_v)); break;
        }
        _id -= 4;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void PowerSlider::valueChanged(double _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
