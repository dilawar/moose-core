// =====================================================================================
//
//       Filename:  Finfo.cpp
//
//    Description:
//
//         Author:  Dilawar Singh (), dilawar.s.rajput@gmail.com
//   Organization:  NCBS Bangalore
//
// =====================================================================================

#include "../external/pybind11/include/pybind11/pybind11.h"
#include "../external/pybind11/include/pybind11/stl.h"
#include "../external/pybind11/include/pybind11/numpy.h"
#include "../external/pybind11/include/pybind11/functional.h"

namespace py = pybind11;

#include "../basecode/header.h"
#include "../utility/print_function.hpp"
#include "../utility/strutil.h"
#include "../builtins/Variable.h"
#include "pymoose.h"
#include "helper.h"
#include "Finfo.h"

__Finfo__::__Finfo__(const ObjId& oid, const Finfo* f, const string& finfoType)
    : oid_(oid), f_(f), finfoType_(finfoType)
{
    // cout << " __Finfo__ type " << finfoType << endl;

    if (finfoType == "DestFinfo")
        func_ = [oid, f, this](const py::object& key) {
            return getLookupValueFinfoItem(oid, f, key);
        };
    else if (finfoType == "FieldElementFinfo")
        func_ = [oid, f, this](const py::object& index) {
            // this is essential of make this function static.
            return getElementFinfoItem(oid, f, py::cast<unsigned int>(index));
        };
    else if (finfoType == "LookupValueFinfo")
        func_ = [oid, f, this](const py::object& key) {
            // Assigning is essential or make these functions static.
            return this->getLookupValueFinfoItem(oid, f, key);
        };
    else
        func_ = [this](const py::object& key) {
            throw runtime_error("Not supported for Finfo type '" + finfoType_ +
                                "'");
            return py::none();
        };
}

// Exposed to python as __setitem__ on Finfo
bool __Finfo__::setItem(const py::object& key, const py::object& val)
{
    return __Finfo__::setLookupValueFinfoItem(oid_, key, val, f_);
}

bool __Finfo__::setLookupValueFinfoItem(const ObjId& oid, const py::object& key,
                                        const py::object& val,
                                        const Finfo* finfo)
{
    auto rttType = finfo->rttiType();
    auto fieldName = finfo->name();

    vector<string> srcDestType;
    moose::tokenize(rttType, ",", srcDestType);
    assert(srcDestType.size() == 2);

    auto srcType = srcDestType[0];
    auto destType = srcDestType[1];

    if (srcType == "unsigned int") {
        if (destType == "double")
            return LookupField<unsigned int, double>::set(
                oid, fieldName, py::cast<unsigned int>(key),
                py::cast<double>(val));
    }

    py::print("NotImplemented::setLookupValueFinfoItem:", key, "to value", val,
              "for object", oid.path(), "and fieldName=", fieldName,
              "rttiType=", rttType, srcDestType);
    throw runtime_error("NotImplemented");
    return true;
}

py::object __Finfo__::getLookupValueFinfoItem(const ObjId& oid, const Finfo* f,
                                              const py::object& key)
{
    auto rttType = f->rttiType();
    auto fname = f->name();
    vector<string> srcDestType;
    moose::tokenize(rttType, ",", srcDestType);
    string srcType = srcDestType[0];
    string tgtType = srcDestType[1];

    py::object r;

    if (srcType == "string") {
        auto k = py::cast<string>(key);
        r = getLookupValueFinfoItemInner<string>(oid, f, k, tgtType);
    } else if (srcType == "unsigned int") {
        auto k = py::cast<unsigned int>(key);
        r = getLookupValueFinfoItemInner<unsigned int>(oid, f, k, tgtType);
    }

    if (r.is(py::none())) {
        py::print("getLookupValueFinfoItem::NotImplemented for key:", key,
                  "srcType:", srcType, "and tgtType:", tgtType, "path: ",
                  oid.path());
        throw runtime_error("getLookupValueFinfoItem::NotImplemented error");
    }
    return r;
}

py::object __Finfo__::getItem(const py::object& key)
{
    // py::print("Fetching value for", key, finfoType_);
    return func_(key);
}

py::object __Finfo__::operator()(const py::object& key)
{
    return func_(key);
}

py::cpp_function __Finfo__::getDestFinfoSetterFunc(const ObjId& oid,
                                                   const Finfo* finfo)
{
    const auto rttType = finfo->rttiType();
    vector<string> types;
    moose::tokenize(rttType, ",", types);

    if (types.size() == 1) return getDestFinfoSetterFunc1(oid, finfo, types[0]);

    assert(types.size() == 2);
    return getDestFinfoSetterFunc2(oid, finfo, types[0], types[1]);
}

// Get DestFinfo2
py::cpp_function __Finfo__::getDestFinfoSetterFunc2(const ObjId& oid,
                                                    const Finfo* finfo,
                                                    const string& ftype1,
                                                    const string& ftype2)
{
    const auto fname = finfo->name();
    if (ftype1 == "double") {
        if (ftype2 == "unsigned int") {
            std::function<bool(double, unsigned int)> func = [oid, fname](
                const double a, const unsigned int b) {
                return SetGet2<double, unsigned int>::set(oid, fname, a, b);
            };
            return func;
        }
        if (ftype2 == "long") {
            std::function<bool(double, long)> func = [oid, fname](
                const double a, const long b) {
                return SetGet2<double, long>::set(oid, fname, a, b);
            };
            return func;
        }
    }
    throw runtime_error("getFieldPropertyDestFinfo2::NotImplemented " + fname +
                        " for rttType " + finfo->rttiType() + " for oid " +
                        oid.path());
}


// Get DestFinfo1.
py::cpp_function __Finfo__::getDestFinfoSetterFunc1(const ObjId& oid,
                                                    const Finfo* finfo,
                                                    const string& ftype)
{
    const auto fname = finfo->name();
    if (ftype == "void") {
        std::function<bool()> func = [oid, fname]() {
            return SetGet0::set(oid, fname);
        };
        return func;
    }

    if (ftype == "double") 
        return getSetGetFunc1<double>(oid, fname);
    if (ftype == "Id") 
        return getSetGetFunc1<Id>(oid, fname);
    if (ftype == "ObjId") 
        return getSetGetFunc1<ObjId>(oid, fname);
    if (ftype == "vector<Id>") 
        return getSetGetFunc1<vector<Id>>(oid, fname);
    if (ftype == "vector<ObjId>") 
        return getSetGetFunc1<vector<ObjId>>(oid, fname);
    if (ftype == "vector<double>") 
        return getSetGetFunc1<vector<double>>(oid, fname);

    throw runtime_error("getFieldPropertyDestFinfo1::NotImplemented " + fname +
                        " for rttType " + ftype + " for oid " + oid.path());
}

py::object __Finfo__::getFieldValue(const ObjId& oid, const Finfo* f)
{
    auto rttType = f->rttiType();
    auto fname = f->name();
    py::object r = py::none();

    if (rttType == "double" or rttType == "float")
        r = pybind11::float_(getField<double>(oid, fname));
    else if (rttType == "vector<double>") {
        // r = py::cast(getField<vector<double>>(oid, fname));
        r = getFieldNumpy<double>(oid, fname);
    } else if (rttType == "vector<unsigned int>") {
        // r = pybind11::cast(getField<vector<unsigned int>>(oid, fname));
        r = getFieldNumpy<unsigned int>(oid, fname);
    } else if (rttType == "string")
        r = pybind11::str(getField<string>(oid, fname));
    else if (rttType == "char")
        r = pybind11::int_(getField<char>(oid, fname));
    else if (rttType == "int")
        r = pybind11::int_(getField<int>(oid, fname));
    else if (rttType == "unsigned int")
        r = pybind11::int_(getField<unsigned int>(oid, fname));
    else if (rttType == "unsigned long")
        r = pybind11::int_(getField<unsigned long>(oid, fname));
    else if (rttType == "bool")
        r = pybind11::bool_(getField<bool>(oid, fname));
    else if (rttType == "Id")
        r = py::cast(getField<Id>(oid, fname));
    else if (rttType == "ObjId")
        r = py::cast(getField<ObjId>(oid, fname));
    else if (rttType == "Variable")
        r = py::cast(getField<Variable>(oid, fname));
    else if (rttType == "vector<Id>")
        r = py::cast(getField<vector<Id>>(oid, fname));
    else if (rttType == "vector<ObjId>")
        r = py::cast(getField<vector<ObjId>>(oid, fname));
    else if (rttType == "vector<string>")
        r = py::cast(getField<vector<string>>(oid, fname));
    else {
        MOOSE_WARN("Warning: getValueFinfo:: Unsupported type '" + rttType +
                   "'");
        r = py::none();
    }
    return r;
}

py::list __Finfo__::getElementFinfo(const ObjId& objid, const Finfo* f)
{
    auto fname = f->name();
    auto oid = ObjId(objid.path() + '/' + fname);
    auto len = Field<unsigned int>::get(oid, "numField");
    vector<ObjId> res(len);
    for (unsigned int i = 0; i < len; i++)
        res[i] = ObjId(oid.path(), oid.dataIndex, i);
    return py::cast(res);
}

py::object __Finfo__::getElementFinfoItem(const ObjId& oid, const Finfo* f,
                                          unsigned int index)
{
    // cout << "Fetching at index " << index << " " << getNumField() << endl ;
    if (index >= getNumFieldStatic(oid, f)) {
        throw py::index_error("Index " + to_string(index) + " out of range.");
    }
    auto o = ObjId(oid.path() + '/' + f->name());
    return py::cast(ObjId(o.path(), o.dataIndex, index));
}

string __Finfo__::type() const
{
    return finfoType_;
}

// py::object __Finfo__::getAttr(const string& key)
// {
// std::cout << "Accessing " << key << std::endl;
// }

unsigned int __Finfo__::getNumField()
{
    return __Finfo__::getNumFieldStatic(oid_, f_);
}

bool __Finfo__::setNumField(unsigned int num)
{
    return __Finfo__::setNumFieldStatic(oid_, f_, num);
}

unsigned int __Finfo__::getNumFieldStatic(const ObjId& oid, const Finfo* f)
{
    auto o = __Finfo__::getObjIdStatic(oid, f);
    return Field<unsigned int>::get(o, "numField");
}

bool __Finfo__::setNumFieldStatic(const ObjId& oid, const Finfo* f,
                                  unsigned int num)
{
    auto o = __Finfo__::getObjIdStatic(oid, f);
    return Field<unsigned int>::set(o, "numField", num);
}

// Static function.
ObjId __Finfo__::getObjIdStatic(const ObjId& oid, const Finfo* f)
{
    return ObjId(oid.path() + '/' + f->name());
}

// Non static function.
ObjId __Finfo__::getObjId() const
{
    return ObjId(oid_.path() + '/' + f_->name());
}
