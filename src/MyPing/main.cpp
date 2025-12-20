#include "CPing.h"
#include <stdio.h>
#include <process.h>

CRITICAL_SECTION g_cs;

// 优化建议1：线程函数使用const正确性
unsigned int __stdcall ThreadProc(void* lpParam)
{
    const char* szDestIP = (const char*)lpParam;
    CPing       objPing;
    PingReply   req;

    // 执行ping操作
    if (objPing.Ping(szDestIP, &req))
    {
        ::EnterCriticalSection(&g_cs);
        printf("Reply from %s: bytes=%d, time=%dms, TTL=%d\n", szDestIP, req.m_dwBytes, req.m_dwRoundTrip, req.m_dwTTL);
        ::LeaveCriticalSection(&g_cs);
    }
    else
    {
        ::EnterCriticalSection(&g_cs);
        printf("Ping failed for %s\n", szDestIP);
        ::LeaveCriticalSection(&g_cs);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    // 优化建议2：支持命令行参数或默认值
    const char* szDestIP = (argc > 1) ? argv[1] : "127.0.0.1";

    printf("Pinging %s with %d threads...\n", szDestIP, 4);

    // 初始化临界区
    ::InitializeCriticalSection(&g_cs);

    // 优化建议3：正确存储线程句柄和ID
    const int    THREAD_COUNT = 4;
    HANDLE       hThreads[THREAD_COUNT];
    unsigned int threadIds[THREAD_COUNT];

    // 创建线程
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        // 关键修正：传递正确的参数，而不是NULL
        hThreads[i] = (HANDLE)::_beginthreadex(NULL,            /// 安全属性
                                               0,               /// 栈大小（默认）
                                               ThreadProc,      /// 线程函数
                                               (void*)szDestIP, /// 传递给线程的参数（之前是NULL）
                                               0,               /// 创建标志（0=立即运行）
                                               &threadIds[i]    /// 线程ID
        );

        if (hThreads[i] == NULL || hThreads[i] == (HANDLE)0)
        {
            printf("Error: Failed to create thread %d\n", i);
            // 清理已创建的线程
            for (int j = 0; j < i; ++j)
            {
                CloseHandle(hThreads[j]);
            }
            DeleteCriticalSection(&g_cs);
            return 1;
        }
    }

    // 优化建议4：等待所有线程完成（修正WaitForSingleObject使用）
    // 使用WaitForMultipleObjects等待所有线程
    ::WaitForMultipleObjects(THREAD_COUNT, // 要等待的对象数量
                             hThreads,     // 句柄数组
                             TRUE,         // 等待所有对象
                             INFINITE      // 无限等待
    );

    // 优化建议5：正确清理资源
    // 关闭所有线程句柄
    for (int i = 0; i < THREAD_COUNT; ++i)
    {
        if (hThreads[i] != NULL && hThreads[i] != (HANDLE)0)
        {
            ::CloseHandle(hThreads[i]);
        }
    }

    // 删除临界区
    ::DeleteCriticalSection(&g_cs);

    printf("\nAll ping threads completed. Press any key to exit...\n");
    getchar();
    return 0;
}
