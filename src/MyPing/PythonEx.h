#pragma once
#ifdef _DEBUG
#undef _DEBUG
#define USE_DEBUG
#endif

#include <Python.h>

#ifdef USE_DEBUG
#define _DEBUG
#endif
