// call_function.c - A sample of calling 
// python functions from C code
// 
// #include <Python.h>
// #include <iostream>
#include "tis.h"


int tisRun(std::string filename)
{
    PyObject *pName, *pModule, *pDict, *pClass, *pInstance;

    // Initialize the Python interpreter
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"/xlabfs/qian/clang-llvm/llvm-project/clang-tools-extra/matcher\")");

    // Build the name object
    pName = PyUnicode_FromString("tis_run");
    // Load the module object
    pModule = PyImport_Import(pName);
    // pDict is a borrowed reference
    pDict = PyModule_GetDict(pModule);
    // Build the name of a callable class
    pClass = PyDict_GetItemString(pDict, "tis");


    // Create an instance of the class
    if (PyCallable_Check(pClass))
    {
        pInstance = PyObject_CallObject(pClass, NULL);
        PyObject* myResult = PyObject_CallMethod(pInstance, "RunTis", "(s)", filename);
        Py_INCREF(myResult);
        if(myResult == Py_True) return 1;
        return 0;


    }
    else
    {
        return 0;
        std::cout << "Cannot instantiate the Python class" << std::endl;
    }
    Py_Finalize();
}

/*
int main(){
	tisRun("/home/tis/auto-verification-suite/mock_generator/tis-mock-gen/loops/src/loop03_v.c");

}*/
