/****************************************************************************
** Meta object code from reading C++ file 'viewgraph.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "viewgraph.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewgraph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ViewGraph[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      34,   10,   10,   10, 0x0a,
      52,   10,   10,   10, 0x08,
      66,   10,   10,   10, 0x08,
      77,   10,   10,   10, 0x08,
      85,   10,   10,   10, 0x08,
     102,   10,   10,   10, 0x08,
     123,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ViewGraph[] = {
    "ViewGraph\0\0statusMessage(QString)\0"
    "toggleAllCurves()\0moved(QPoint)\0"
    "selected()\0print()\0exportDocument()\0"
    "enableZoomMode(bool)\0zoomOut()\0"
};

void ViewGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ViewGraph *_t = static_cast<ViewGraph *>(_o);
        switch (_id) {
        case 0: _t->statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->toggleAllCurves(); break;
        case 2: _t->moved((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 3: _t->selected(); break;
        case 4: _t->print(); break;
        case 5: _t->exportDocument(); break;
        case 6: _t->enableZoomMode((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->zoomOut(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ViewGraph::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ViewGraph::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ViewGraph,
      qt_meta_data_ViewGraph, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ViewGraph::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ViewGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ViewGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ViewGraph))
        return static_cast<void*>(const_cast< ViewGraph*>(this));
    return QWidget::qt_metacast(_clname);
}

int ViewGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void ViewGraph::statusMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
