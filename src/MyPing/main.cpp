#include "CPing.h"
#include "PythonEx.h"

#include <stdio.h>
#include <string>
#include <vector>

#include <process.h>
#include <Windows.h>

CRITICAL_SECTION g_cs;
PyObject*        g_pythonWriteFunction = NULL;

/// 初始化Python解释器
/// 初始化Python解释器 - 修复版本
bool InitializePython()
{
    /// 1. 初始化解释器
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        printf("Failed to initialize Python interpreter\n");
        return false;
    }

    /// 2. 🔥 关键修复：在主线程保存当前线程状态
    // 这确保了主线程拥有一个有效的线程状态
    PyThreadState* main_thread_state = PyThreadState_Get();

    /// 3. 🔥 关键修复：显式释放GIL，但保存线程状态
    PyEval_SaveThread(); // 这会释放GIL并返回当前线程状态

    /// 4. 现在GIL已释放，子线程可以获取它
    // 但我们还需要在主线程中操作Python，所以临时重新获取

    PyGILState_STATE gstate = PyGILState_Ensure(); // 重新获取GIL用于初始化

    /// 5. 添加Python路径
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(r'assert')");

    /// 6. 导入模块
    PyObject* pModuleName = PyUnicode_FromString("ping_excel_writer");
    if (!pModuleName)
    {
        PyErr_Print();
        PyGILState_Release(gstate);
        return false;
    }

    PyObject* pModule = PyImport_Import(pModuleName);
    Py_DECREF(pModuleName);

    if (!pModule)
    {
        printf("Failed to import ping_excel_writer module\n");
        PyErr_Print();
        PyGILState_Release(gstate);
        return false;
    }

    /// 7. 获取写入函数
    g_pythonWriteFunction = PyObject_GetAttrString(pModule, "write_ping_result");
    Py_DECREF(pModule);

    if (!g_pythonWriteFunction || !PyCallable_Check(g_pythonWriteFunction))
    {
        printf("Failed to get write_ping_result function\n");
        Py_XDECREF(g_pythonWriteFunction);
        g_pythonWriteFunction = NULL;
        PyGILState_Release(gstate);
        return false;
    }

    /// 8. 释放GIL，让子线程可以运行
    PyGILState_Release(gstate);

    printf("Python interpreter initialized successfully (thread-safe)\n");
    return true;
}

/// 线程安全的Python调用函数
void WriteToExcelViaPython(const char* szDestIP, DWORD bytes, DWORD roundTrip, DWORD ttl, bool success)
{
    if (!g_pythonWriteFunction)
        return;

    /// 准备Python函数参数
    PyObject* pArgs    = PyTuple_New(5);
    PyObject* pIP      = PyUnicode_FromString(szDestIP);
    PyObject* pBytes   = PyLong_FromUnsignedLong(bytes);
    PyObject* pTime    = PyLong_FromUnsignedLong(roundTrip);
    PyObject* pTTL     = PyLong_FromUnsignedLong(ttl);
    PyObject* pSuccess = success ? Py_True : Py_False;
    Py_INCREF(pSuccess);

    PyTuple_SetItem(pArgs, 0, pIP);
    PyTuple_SetItem(pArgs, 1, pBytes);
    PyTuple_SetItem(pArgs, 2, pTime);
    PyTuple_SetItem(pArgs, 3, pTTL);
    PyTuple_SetItem(pArgs, 4, pSuccess);

    /// 调用Python函数
    PyObject* pValue = PyObject_CallObject(g_pythonWriteFunction, pArgs);

    /// 清理
    Py_DECREF(pArgs);
    if (pValue)
    {
        Py_DECREF(pValue);
    }
    else
    {
        PyErr_Print(); /// 打印Python错误
    }
}

// 批量写入优化版本（减少Python调用次数）
void BatchWriteToExcel(const char* szDestIP, DWORD bytes, DWORD roundTrip, DWORD ttl, bool success)
{
    static thread_local std::vector<PyObject*> batchData;
    static thread_local int                    batchCount = 0;

    // 创建字典对象存储单条记录
    PyObject* pRecord = PyDict_New();
    PyDict_SetItemString(pRecord, "ip", PyUnicode_FromString(szDestIP));
    PyDict_SetItemString(pRecord, "bytes", PyLong_FromUnsignedLong(bytes));
    PyDict_SetItemString(pRecord, "time", PyLong_FromUnsignedLong(roundTrip));
    PyDict_SetItemString(pRecord, "ttl", PyLong_FromUnsignedLong(ttl));
    PyDict_SetItemString(pRecord, "success", success ? Py_True : Py_False);

    batchData.push_back(pRecord);
    batchCount++;

    // 每5条记录或线程结束时批量写入
    if (batchCount >= 5)
    {
        ::EnterCriticalSection(&g_cs);

        PyObject* pBatchList = PyList_New(batchCount);
        for (int i = 0; i < batchCount; i++)
        {
            PyList_SetItem(pBatchList, i, batchData[i]);
        }

        // 调用Python批量写入函数
        if (g_pythonWriteFunction)
        {
            PyObject* pArgs = PyTuple_New(1);
            PyTuple_SetItem(pArgs, 0, pBatchList);

            PyObject* pValue = PyObject_CallObject(g_pythonWriteFunction, pArgs);

            Py_DECREF(pArgs);
            if (pValue)
            {
                Py_DECREF(pValue);
            }
            else
            {
                PyErr_Print();
            }
        }

        // 清理批量数据
        for (auto& item : batchData)
        {
            Py_DECREF(item);
        }
        batchData.clear();
        batchCount = 0;

        ::LeaveCriticalSection(&g_cs);
    }
}

unsigned int __stdcall ThreadProc(void* lpParam)
{
    const char* szDestIP = (const char*)lpParam;
    CPing       objPing;
    PingReply   req;

    // 🔥 每个线程需要自己的Python线程状态
    PyGILState_STATE gstate;

    if (objPing.Ping(szDestIP, &req))
    {
        // 方案1：直接写入
        gstate = PyGILState_Ensure(); // 获取GIL
        WriteToExcelViaPython(szDestIP, req.m_dwBytes, req.m_dwRoundTrip, req.m_dwTTL, true);
        PyGILState_Release(gstate); // 释放GIL

        ::EnterCriticalSection(&g_cs);
        printf("[Success] %s: bytes=%d, time=%dms, TTL=%d\n", szDestIP, req.m_dwBytes, req.m_dwRoundTrip, req.m_dwTTL);
        ::LeaveCriticalSection(&g_cs);
    }
    else
    {
        gstate = PyGILState_Ensure(); // 获取GIL
        WriteToExcelViaPython(szDestIP, 0, 0, 0, false);
        PyGILState_Release(gstate); // 释放GIL

        ::EnterCriticalSection(&g_cs);
        printf("[Failed] %s\n", szDestIP);
        ::LeaveCriticalSection(&g_cs);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    const char* szDestIP = (argc > 1) ? argv[1] : "127.0.0.1";

    /// 初始化Python解释器
    if (!InitializePython())
    {
        printf("Warning: Python initialization failed. Excel logging disabled.\n");
    }

    printf("Pinging %s with %d threads...\n", szDestIP, 4);

    ::InitializeCriticalSection(&g_cs);

    const int    THREAD_COUNT = 4;
    HANDLE       hThreads[THREAD_COUNT];
    unsigned int threadIds[THREAD_COUNT];

    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        hThreads[i] = (HANDLE)::_beginthreadex(NULL, 0, ThreadProc, (void*)szDestIP, 0, &threadIds[i]);

        if (hThreads[i] == NULL)
        {
            printf("Error: Failed to create thread %d\n", i);
            for (int j = 0; j < i; ++j)
            {
                CloseHandle(hThreads[j]);
            }
            DeleteCriticalSection(&g_cs);

            /// 清理Python
            if (g_pythonWriteFunction)
            {
                Py_DECREF(g_pythonWriteFunction);
            }
            Py_Finalize();

            return 1;
        }
    }

    /// 等待所有线程
    ::WaitForMultipleObjects(THREAD_COUNT, hThreads, TRUE, INFINITE);

    /// 清理线程句柄
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        if (hThreads[i] != NULL)
        {
            ::CloseHandle(hThreads[i]);
        }
    }

    /// 确保所有数据写入Excel
    if (g_pythonWriteFunction)
    {
        /// 调用Python的保存函数
        PyGILState_STATE final_gstate = PyGILState_Ensure();
        PyObject*        pFunc = PyObject_GetAttrString(PyImport_ImportModule("ping_excel_writer"), "save_and_close");
        if (pFunc && PyCallable_Check(pFunc))
        {
            PyObject_CallObject(pFunc, NULL);
            Py_DECREF(pFunc);
        }
        PyGILState_Release(final_gstate);
    }

    /// 清理Python资源
    if (g_pythonWriteFunction)
    {
        /// 清理全局函数前也需要GIL
        PyGILState_STATE cleanup_gstate = PyGILState_Ensure();
        Py_DECREF(g_pythonWriteFunction);
        PyGILState_Release(cleanup_gstate);
        g_pythonWriteFunction = NULL;
    }
    Py_Finalize();

    ::DeleteCriticalSection(&g_cs);

    printf("\nAll ping threads completed. Results saved to ping_results.xlsx\n");
    printf("Press any key to exit...\n");
    getchar();

    return 0;
}
