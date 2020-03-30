// =====================================================================================
//
//       Filename:  pymoose.cpp
//
//    Description:  Python bindings generated using PyBind11.
//
//        Version:  1.0
//        Created:  03/15/2020 04:03:58 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Dilawar Singh (), dilawar.s.rajput@gmail.com
//   Organization:  NCBS Bangalore
//
// =====================================================================================

#include <map>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

#include "../external/pybind11/include/pybind11/functional.h"
#include "../external/pybind11/include/pybind11/numpy.h"
#include "../external/pybind11/include/pybind11/pybind11.h"
#include "../external/pybind11/include/pybind11/stl.h"

// See
// https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html#binding-stl-containers
// #include "../external/pybind11/include/pybind11/stl_bind.h"

#include "../basecode/global.h"
#include "../basecode/header.h"
#include "../basecode/Cinfo.h"
#include "../builtins/Variable.h"
#include "../shell/Neutral.h"
#include "../shell/Shell.h"
#include "../shell/Wildcard.h"
#include "../utility/strutil.h"

#include "../basecode/global.h"

#include "helper.h"
#include "pymoose.h"

#include "Finfo.hpp"
#include "Vec.hpp"

using namespace std;
namespace py = pybind11;
using namespace pybind11::literals;

Id initModule(py::module& m)
{
    return initShell();
}


PYBIND11_MODULE(_cmoose, m)
{
    m.doc() = R"moosedoc(moose module.)moosedoc";

    initModule(m);

    // This is a wrapper around Field::get  and LookupField::get which may
    // return simple values or vector. Python scripts expect LookupField to
    // return either list of dict which can be queried by key and index. This
    // class bind both __getitem__ to the getter function call.
    // Note that both a.isA["Compartment"] and a.isA("Compartment") are valid
    // now.
    py::class_<__Finfo__>(m, "_Finfo")
        .def(py::init<const ObjId&, const string&, const Finfo*>())
        .def("__call__", &__Finfo__::operator())
        .def("__getitem__", &__Finfo__::operator());

    py::class_<Id>(m, "_Id")
        .def(py::init<>())
        .def(py::init<unsigned int>())
        .def(py::init<const string&, const string&>())
        .def(py::init<const ObjId&>())
        // properties
        .def_property_readonly("numIds", &Id::numIds)
        .def_property_readonly("path", &Id::path)
        .def_property_readonly("name",
                               [](const Id& id) { return ObjId(id).name(); })
        .def_property_readonly("id", &Id::value)
        .def_property_readonly("cinfo",
                               [](Id& id) { return id.element()->cinfo(); },
                               py::return_value_policy::reference)
        .def_property_readonly(
             "type", [](Id& id) { return id.element()->cinfo()->name(); })
        .def("__repr__", [](const Id& id) {
             return "<Id id=" + std::to_string(id.value()) + " path=" +
                    id.path() + " class=" + id.element()->cinfo()->name() + ">";
         });

    py::class_<ObjId>(m, "_ObjId")
        .def(py::init<>())
        .def(py::init<Id>())
        .def(py::init<Id, unsigned int>())
        .def(py::init<Id, unsigned int, unsigned int>())
        .def(py::init<const string&>())
        //---------------------------------------------------------------------
        //  Readonly properties.
        //---------------------------------------------------------------------
        .def_property_readonly("value",
                               [](const ObjId oid) { return oid.id.value(); })
        .def_property_readonly("path", &ObjId::path)
        .def_property_readonly(
             "parent", [](const ObjId& oid) { return Neutral::parent(oid); })
        .def_property_readonly("name", &ObjId::name)
        .def_property_readonly("className", [](const ObjId& oid) {
             return oid.element()->cinfo()->name();
         })
        .def_property_readonly("id", [](ObjId& oid) { return oid.id; })
        .def_property_readonly(
             "type", [](ObjId& oid) { return oid.element()->cinfo()->name(); })
        //--------------------------------------------------------------------
        // Set/Get
        //--------------------------------------------------------------------
        // Overload of Field::set
        .def("setField", &setProperty<double>)
        .def("setField", &setProperty<double>)
        .def("setField", &setProperty<vector<double>>)
        .def("setField", &setProperty<std::string>)
        .def("setField", &setProperty<bool>)

        // Overload for Field::get
        .def("getField", &getProperty)
        .def("getElementField", &getElementField)
        .def("getElementFieldItem", &getElementFieldItem)
        .def("getNumpy", &getFieldNumpy<double>)

        /**
        * Attributes.
        */
        .def("__getattr__", &getProperty)
        .def("__getattr__", &getPropertyDestFinfo)
        .def("__setattr__", &setProperty<double>)
        .def("__setattr__", &setProperty<vector<double>>)
        .def("__setattr__", &setProperty<std::string>)
        .def("__setattr__", &setProperty<ObjId>)
        .def("__setattr__", &setProperty<bool>)

        //---------------------------------------------------------------------
        //  Connect
        //---------------------------------------------------------------------
        .def("connect", &mooseConnect)

        //---------------------------------------------------------------------
        //  Extra
        //---------------------------------------------------------------------
        .def("__repr__", [](const ObjId& oid) {
             return "<" + oid.element()->cinfo()->name() + " id=" +
                    std::to_string(oid.id.value()) + " numData=" +
                    to_string(oid.eref().dataIndex()) + " path=" + oid.path() +
                    ">";
         });

    // Variable.
    py::class_<Variable>(m, "_Variable").def(py::init<>());

    // Cinfo.
    py::class_<Cinfo>(m, "_Cinfo")
        .def(py::init<>())
        .def_property_readonly("name", &Cinfo::name)
        .def_property_readonly("finfoMap", &Cinfo::finfoMap,
                               py::return_value_policy::reference)
        .def("findFinfo", &Cinfo::findFinfoWrapper)
        .def("baseCinfo", &Cinfo::baseCinfo, py::return_value_policy::reference)
        .def("isA", &Cinfo::isA);

    // Shell
    py::class_<Shell>(m, "_Shell")
        .def(py::init<>())
        .def("create", &Shell::doCreate2)
        .def("addMsg", &Shell::doAddMsg)
        .def("getCwe", &Shell::getCwe)
        .def("setClock", &Shell::doSetClock)
        .def("reinit", &Shell::doReinit)
        .def("delete", &Shell::doDelete)
        .def("start", &Shell::doStart, "runtime"_a, "notify"_a = false)
        .def("quit", &Shell::doQuit);

    // Vec
    py::class_<MooseVec>(m, "vec", py::dynamic_attr())
        .def(py::init<const string&, size_t, const string&>(), "path"_a,
             "n"_a = 1, "dtype"_a = "Neutral")  // Default
        .def("__len__", &MooseVec::len)
        .def("__getitem__", &MooseVec::getElem)
        .def("__setattr__", &MooseVec::setAttrOneToOne<double>)
        .def("__setattr__", &MooseVec::setAttrOneToAll<double>)
        .def("__getattr__", &MooseVec::getAttr)
        ;

    // Module functions.
    m.def("getShell",
          []() { return reinterpret_cast<Shell*>(Id().eref().data()); },
          py::return_value_policy::reference);

    m.def("seed", [](size_t a) { moose::mtseed(a); });
    m.def("rand", [](double a, double b) { return moose::mtrand(a, b); },
          "a"_a = 0, "b"_a = 1);
    m.def("wildcardFind", &wildcardFind2);
    m.def("delete", &mooseDelete);
    m.def("create", &mooseCreate);
    m.def("reinit", &mooseReinit);
    m.def("start", &mooseStart, "runtime"_a, "notify"_a = false);
    m.def("element", &mooseElement);
    m.def("exists", &mooseExists);
    m.def("connect", &mooseConnect);
    m.def("getCwe", &mooseGetCwe);
    m.def("setClock", &mooseSetClock);
    m.def("loadModelInternal", &loadModelInternal);
    m.def("getFieldDict", &mooseGetFieldDict, "className"_a,
          "finfoType"_a = "");
    m.def("copy", &mooseCopy, "orig"_a, "newParent"_a, "newName"_a, "num"_a = 1,
          "toGlobal"_a = false, "copyExtMsgs"_a = false);

    // Attributes.
    m.attr("NA") = NA;
    m.attr("PI") = PI;
    m.attr("FaradayConst") = FaradayConst;
    m.attr("GasConst") = GasConst;
    m.attr("__version__") = MOOSE_VERSION;
}
