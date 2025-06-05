#ifndef __PM_LOG_H
#define __PM_LOG_H

#define PAGE_MANAGER_USE_LOG 1

#if PAGE_MANAGER_USE_LOG
#include <stdio.h>
#define _PM_LOG(format, ...) printf("[PageManager]-" format "\r\n", ##__VA_ARGS__)
#define PM_LOG_INFO(format, ...)  _PM_LOG("[Info] " format, ##__VA_ARGS__)
#define PM_LOG_WARN(format, ...)  _PM_LOG("[Warn] " format, ##__VA_ARGS__)
#define PM_LOG_ERROR(format, ...) _PM_LOG("[Error] " format, ##__VA_ARGS__)
#else
#  define PM_LOG_INFO(...)
#  define PM_LOG_WARN(...)
#  define PM_LOG_ERROR(...)
#endif

#endif