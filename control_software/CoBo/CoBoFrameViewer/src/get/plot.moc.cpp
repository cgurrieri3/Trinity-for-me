/****************************************************************************
** Meta object code from reading C++ file 'plot.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "plot.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'plot.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Plot[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      61,    6,    5,    5, 0x0a,
     176,  131,    5,    5, 0x2a,
     280,  241,    5,    5, 0x2a,
     366,  338,    5,    5, 0x2a,
     438,  416,    5,    5, 0x2a,
     480,    5,    5,    5, 0x0a,
     502,    5,  498,    5, 0x0a,
     513,  509,    5,    5, 0x08,
     545,  542,    5,    5, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Plot[] = {
    "Plot\0\0numPoints,xData,yData,title,symbolName,color,yAxisLeft\0"
    "addCurve(int,const double*,const double*,QString,QString,QColor,bool)\0"
    "numPoints,xData,yData,title,symbolName,color\0"
    "addCurve(int,const double*,const double*,QString,QString,QColor)\0"
    "numPoints,xData,yData,title,symbolName\0"
    "addCurve(int,const double*,const double*,QString,QString)\0"
    "numPoints,xData,yData,title\0"
    "addCurve(int,const double*,const double*,QString)\0"
    "numPoints,xData,yData\0"
    "addCurve(int,const double*,const double*)\0"
    "toggleAllCurves()\0int\0size()\0,on\0"
    "showCurve(QwtPlotItem*,bool)\0,,\0"
    "showCurve(QVariant,bool,int)\0"
};

void Plot::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Plot *_t = static_cast<Plot *>(_o);
        switch (_id) {
        case 0: _t->addCurve((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const double*(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QColor(*)>(_a[6])),(*reinterpret_cast< const bool(*)>(_a[7]))); break;
        case 1: _t->addCurve((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const double*(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5])),(*reinterpret_cast< const QColor(*)>(_a[6]))); break;
        case 2: _t->addCurve((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const double*(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4])),(*reinterpret_cast< const QString(*)>(_a[5]))); break;
        case 3: _t->addCurve((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const double*(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 4: _t->addCurve((*reinterpret_cast< const int(*)>(_a[1])),(*reinterpret_cast< const double*(*)>(_a[2])),(*reinterpret_cast< const double*(*)>(_a[3]))); break;
        case 5: _t->toggleAllCurves(); break;
        case 6: { int _r = _t->size();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 7: _t->showCurve((*reinterpret_cast< QwtPlotItem*(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 8: _t->showCurve((*reinterpret_cast< const QVariant(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Plot::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Plot::staticMetaObject = {
    { &QwtPlot::staticMetaObject, qt_meta_stringdata_Plot,
      qt_meta_data_Plot, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Plot::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Plot::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Plot::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Plot))
        return static_cast<void*>(const_cast< Plot*>(this));
    return QwtPlot::qt_metacast(_clname);
}

int Plot::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QwtPlot::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
