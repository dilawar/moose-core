/***
 *    Description:  Finfo Wrapper
 *
 *        Created:  2020-03-30

 *         Author:  Dilawar Singh <dilawar.s.rajput@gmail.com>
 *        License:  GPLv3
 */

#ifndef FINFO_H
#define FINFO_H

class __Finfo__ {
public:
    __Finfo__(const ObjId& oid, const Finfo* f, const string& finfoType);

    template <typename T>
    static py::object getLookupValueFinfoItemInner(const ObjId& oid,
                                                   const Finfo* f, const T& key,
                                                   const string& tgtType)
    {
        auto fname = f->name();
        if (tgtType == "bool")
            return py::cast(LookupField<T, bool>::get(oid, fname, key));
        if (tgtType == "double")
            return py::cast(LookupField<T, double>::get(oid, fname, key));
        if (tgtType == "vector<double>")
            return py::cast(
                LookupField<T, vector<double>>::get(oid, fname, key));
        if (tgtType == "vector<Id>")
            return py::cast(LookupField<T, vector<Id>>::get(oid, fname, key));
        if (tgtType == "vector<ObjId>")
            return py::cast(
                LookupField<T, vector<ObjId>>::get(oid, fname, key));

        py::print("Warning: Could not find", fname, "for key", key, "(type",
                  tgtType, ") on path ", oid.path());
        return py::none();
    }

    static py::cpp_function getDestFinfoSetterFunc(const ObjId& oid,
                                                   const Finfo* finfo);

    static py::cpp_function getDestFinfoSetterFunc1(const ObjId& oid,
                                                    const Finfo* finfo,
                                                    const string& srctype);
    static py::cpp_function getDestFinfoSetterFunc2(const ObjId& oid,
                                                    const Finfo* finfo,
                                                    const string& srctype,
                                                    const string& tgttype);

    static py::object getFieldValue(const ObjId& oid, const Finfo* f);

    static py::list getElementFinfo(const ObjId& objid, const Finfo* f);

    static py::object getElementFinfoItem(const ObjId& oid, const Finfo* f,
                                          unsigned int i);

    // Get attribute (python api);
    unsigned int getNumField();
    bool setNumField(unsigned int);

    static unsigned int getNumFieldStatic(const ObjId& oid, const Finfo* f);
    static bool setNumFieldStatic(const ObjId& oid, const Finfo* f,
                                  unsigned int i);

    // Exposed to python as __setitem__
    bool setItem(const py::object& key, const py::object& val);

    // Exposed to python as __getitem__
    py::object getItem(const py::object& key);

    static py::object getLookupValueFinfoItem(const ObjId& oid, const Finfo* f,
                                              const py::object& key);

    static bool setLookupValueFinfoItem(const ObjId& oid, const py::object& key,
                                        const py::object& val,
                                        const Finfo* finfo);

    py::object operator()(const py::object& key);

    string type() const;

public:
    ObjId oid_;
    const Finfo* f_;
    const std::string finfoType_;
    std::function<py::object(const py::object& key)> func_;
};

#endif /* end of include guard: FINFO_H */
