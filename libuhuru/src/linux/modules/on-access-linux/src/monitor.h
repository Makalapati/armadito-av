#ifndef _ONACCESS_MONITOR_H_
#define _ONACCESS_MONITOR_H_

struct access_monitor;

struct access_monitor *access_monitor_new(struct uhuru *uhuru);

int access_monitor_enable(struct access_monitor *m, int enable);

int access_monitor_enable_permission(struct access_monitor *m, int enable_permission);

int access_monitor_enable_removable_media(struct access_monitor *m, int enable_removable_media);

int access_monitor_add_mount(struct access_monitor *m, const char *mount_point);

int access_monitor_add_path(struct access_monitor *m, const char *path);

void access_monitor_start(struct access_monitor *m);

void access_monitor_stop(struct access_monitor *m);

#endif
