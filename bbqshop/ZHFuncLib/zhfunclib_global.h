#ifndef ZHFUNCLIB_GLOBAL_H
#define ZHFUNCLIB_GLOBAL_H

//#include <QtCore/qglobal.h>

#ifdef ZHFUNCLIB_LIB
# define ZHFUNCLIB_EXPORT __declspec(dllexport)
#else
# define ZHFUNCLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // ZHFUNCLIB_GLOBAL_H
