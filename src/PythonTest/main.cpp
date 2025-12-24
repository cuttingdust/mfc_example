#include <Python.h>

int main(int argc, char *argv[])
{
    Py_SetPythonHome(LR"(C:\Program Files\Python314)");

    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('./assert')");
    PyRun_SimpleString("print('Hello from embedded Python!')");

    PyObject *pModule = NULL;
    PyObject *pFunc   = NULL;
    PyObject *pArg    = NULL;

    pModule = PyImport_ImportModule("test_add");
    if (pModule != NULL)
    {
        pFunc = PyObject_GetAttrString(pModule, "add");
        if (pFunc && PyCallable_Check(pFunc))
        {
            pArg             = PyTuple_Pack(2, PyLong_FromLong(5), PyLong_FromLong(7));
            PyObject *pValue = PyObject_CallObject(pFunc, pArg);
            if (pValue != NULL)
            {
                long result = PyLong_AsLong(pValue);
                printf("Result of add(5, 7): %ld\n", result);
                Py_DECREF(pValue);
            }
            else
            {
                PyErr_Print();
            }
            Py_XDECREF(pArg);
        }
        else
        {
            PyErr_Print();
        }
        Py_XDECREF(pFunc);
        Py_XDECREF(pModule);
    }
    else
    {
        PyErr_Print();
    }

    Py_Finalize();

    return 0;
}
