#ifndef __LOG_H__
#define __LOG_H__

#define log(level,fmt,...) do{printf("<%s|%s|%s> " fmt,level,"log",__func__,##__VA_ARGS__);}while(0)

#define loge(fmt,...) log("Error",fmt,##__VA_ARGS__)
#define logw(fmt,...) log("Warn",fmt,##__VA_ARGS__)
#define logi(fmt,...) log("Info",fmt,##__VA_ARGS__)
#define logd(fmt,...) log("Debug",fmt,##__VA_ARGS__)

#endif
