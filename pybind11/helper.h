// =====================================================================================
//
//       Filename:  helper.h
//
//    Description:
//
//        Version:  1.0
//        Created:  03/22/2020 09:06:16 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  Dilawar Singh (), dilawar.s.rajput@gmail.com
//   Organization:  NCBS Bangalore
//
// =====================================================================================

#ifndef HELPER_H
#define HELPER_H

namespace py = pybind11;
using namespace std;

class Shell;

Id initShell();

ObjId createIdFromPath(string path, string type, unsigned int numData = 1);

Shell* getShellPtr();

bool mooseExists(const string& path);

ObjId mooseElement(const ObjId& oid);
ObjId mooseElement(const string& path);

ObjId loadModelInternal(const string& fname, const string& modelpath,
                        const string& solverclass);

ObjId getElementField(const ObjId objid, const string& fname);

ObjId getElementFieldItem(const ObjId& objid, const string& fname,
                          unsigned int index);

ObjId mooseConnect(const ObjId& src, const string& srcField, const ObjId& tgt,
                   const string& tgtField);

bool mooseDelete(const ObjId& oid);
bool mooseDelete(const string& path);

ObjId mooseCreate(const string type, const string& path, unsigned int numdata = 1);

ObjId mooseCopy(const Id& orig, ObjId newParent, string newName,
                unsigned int n, bool toGlobal, bool copyExtMsgs);

py::object mooseGetCwe();

void mooseSetClock(const unsigned int clockId, double dt);

map<string, string> mooseGetFieldDict(const string& className,
                                      const string& finfoType);

void mooseReinit();
void mooseStart(double runtime, bool notify);

py::list getElementFinfo(const ObjId& objid, const string& fname,
                         const Finfo* f);

py::object getValueFinfo(const ObjId& oid, const string& fname, const Finfo* f);

py::cpp_function getPropertyDestFinfo(const ObjId& oid, const string& fname,
                                      const Finfo* finfo);

py::object getLookupValueFinfo(const ObjId& oid, const string& fname,
                               const Finfo* f);

py::object getLookupValueFinfoItem(const ObjId& oid, const string& fname,
                                   const string& k, const Finfo* f);

py::cpp_function getFieldPropertyDestFinfo(const ObjId& oid, const string& fname, const Finfo* finfo);

#endif /* end of include guard: HELPER_H */
