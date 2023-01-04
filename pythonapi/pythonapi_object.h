#ifndef PYTHONAPI_OBJECT_H
#define PYTHONAPI_OBJECT_H

#undef HAVE_IEEEFP_H
#define PY_SSIZE_T_CLEAN

#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")

#include "Python.h"
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION


#include "pythonapi_extension.h"

typedef quint64 IlwisTypes;

namespace pythonapi{

    class Object{
    public:
        Object(){}
        virtual ~Object(){}
        virtual bool __bool__() const = 0;
        virtual std::string __str__() = 0;
        virtual IlwisTypes ilwisType() = 0;
    };

}
#endif // PYTHONAPI_OBJECT_H
