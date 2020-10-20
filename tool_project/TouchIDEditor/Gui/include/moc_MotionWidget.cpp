/****************************************************************************
** Meta object code from reading C++ file 'MotionWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "MotionWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MotionWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MotionWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x0a,
      43,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MotionWidget[] = {
    "MotionWidget\0\0fname\0MotionChanged(QString)\0"
    "FrameChanged(int)\0"
};

void MotionWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MotionWidget *_t = static_cast<MotionWidget *>(_o);
        switch (_id) {
        case 0: _t->MotionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->FrameChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MotionWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MotionWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_MotionWidget,
      qt_meta_data_MotionWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MotionWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MotionWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MotionWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MotionWidget))
        return static_cast<void*>(const_cast< MotionWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int MotionWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
