/****************************************************************************
** Meta object code from reading C++ file 'DisplayWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "DisplayWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DisplayWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DisplayWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      33,   14,   14,   14, 0x0a,
      59,   14,   14,   14, 0x0a,
      79,   14,   14,   14, 0x0a,
      96,   14,   14,   14, 0x0a,
     112,   14,   14,   14, 0x0a,
     128,   14,   14,   14, 0x0a,
     145,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_DisplayWidget[] = {
    "DisplayWidget\0\0LightChanged(int)\0"
    "OriginalAlphaChanged(int)\0IdAlphaChanged(int)\0"
    "SetCameraFront()\0SetCameraBack()\0"
    "SetCameraLeft()\0SetCameraRight()\0"
    "ShowAll()\0"
};

void DisplayWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DisplayWidget *_t = static_cast<DisplayWidget *>(_o);
        switch (_id) {
        case 0: _t->LightChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->OriginalAlphaChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->IdAlphaChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->SetCameraFront(); break;
        case 4: _t->SetCameraBack(); break;
        case 5: _t->SetCameraLeft(); break;
        case 6: _t->SetCameraRight(); break;
        case 7: _t->ShowAll(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DisplayWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DisplayWidget::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_DisplayWidget,
      qt_meta_data_DisplayWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DisplayWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DisplayWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DisplayWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DisplayWidget))
        return static_cast<void*>(const_cast< DisplayWidget*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int DisplayWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
