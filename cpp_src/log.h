#ifndef __LOG_H__
#define __LOG_H__

#define logb_(level,fmt,...) do{printf("<%s|%s|%s> " fmt,level,"log",__func__,##__VA_ARGS__);}while(0)

#define loge(fmt,...) logb_("Error",fmt,##__VA_ARGS__)
#define logw(fmt,...) logb_("Warn",fmt,##__VA_ARGS__)
#define logi(fmt,...) logb_("Info",fmt,##__VA_ARGS__)
#define logd(fmt,...) logb_("Debug",fmt,##__VA_ARGS__)

#endif
// python build.py debug base sds object StringObject dict 
// adlist intset skiplist ListObject 
// SetObject HashObject ZsetObject db