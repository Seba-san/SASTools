#ifndef V_REPEXTSHAREMEMORY_GLOBAL_H
#define V_REPEXTSHAREMEMORY_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(V_REPEXTSHAREMEMORY_LIBRARY)
#  define V_REPEXTSHAREMEMORYSHARED_EXPORT Q_DECL_EXPORT
#else
#  define V_REPEXTSHAREMEMORYSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // V_REPEXTSHAREMEMORY_GLOBAL_H
