#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

//GUI Library
#include <gtk/gtk.h>

#define SIZE 1024
//enabling GTK
#define GTK
#define SUBSYSTEM "usb"


static void print_device(struct udev_device *device, const char *source, int prop)
{

    char target[ SIZE ];
       const char* action;
       const char* vendor;
       const char* product;
       const char* manufacturer;
       const char* nproduct;

       if (prop) {
                struct udev_list_entry *list_entry;

                udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(device))
		if(!strcmp(udev_list_entry_get_name(list_entry), "ID_VENDOR")){
                      manufacturer=udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_MODEL")){
                      nproduct = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_MODEL_ID")){
                      product = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_VENDOR_ID")){
                      vendor = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ACTION")){
                      action = udev_list_entry_get_value(list_entry);
                }
        }

        snprintf( target, sizeof( target ), "%s%s%s%s%s%s%s%s%s%s","action:", action, "  VID:", vendor, " PID:", product , "\r         \nmanufacturer:" , manufacturer,"\r\nproduct:", nproduct );

    
       printf("%s\r\n",target);
}

static void gtk_notify(struct udev_device *device, const char *source, int prop) {
       char target[ SIZE ];
       const char* action;
       const char* vendor;
       const char* product;
       const char* manufacturer;
       const char* nproduct;

       if (prop) {
                struct udev_list_entry *list_entry;

                udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(device))
		if(!strcmp(udev_list_entry_get_name(list_entry), "ID_VENDOR")){
                      manufacturer=udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_MODEL")){
                      nproduct = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_MODEL_ID")){
                      product = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ID_VENDOR_ID")){
                      vendor = udev_list_entry_get_value(list_entry);
                }else if(!strcmp(udev_list_entry_get_name(list_entry), "ACTION")){
                      action = udev_list_entry_get_value(list_entry);
                }
        }

        snprintf( target, sizeof( target ), "%s%s%s%s%s%s%s%s", "VID:", vendor, " PID:", product , "\r\nmanufacturer:" , manufacturer
              ,"\r\nproduct:", nproduct );

        GApplication *application = g_application_new ("usb.mon", G_APPLICATION_FLAGS_NONE);
        GNotification *notification;
        g_application_register (application, NULL, NULL);
	if(!strcmp(action, "add")){
        notification = g_notification_new ("USB Detected");
	}else if(!strcmp(action, "remove")){
        notification = g_notification_new ("USB Removed");
        }
        g_notification_set_body (notification, target);
        GIcon *icon = g_themed_icon_new ("dialog-information");
        g_notification_set_icon (notification, icon);


        g_application_send_notification (application, NULL, notification);


        g_object_unref (icon);
        g_object_unref (notification);
        g_object_unref (application); 

}


static void process_device(struct udev_device* dev)
{
    if (dev) {
        if (udev_device_get_devnode(dev))
#ifndef GTK
           print_device(dev, "UDEV", 1);
#else
            gtk_notify(dev, "UDEV", 1);
#endif
        udev_device_unref(dev);
    }
}


static void monitor_devices(struct udev* udev)
{
    struct udev_monitor* mon = udev_monitor_new_from_netlink(udev, "udev");

    udev_monitor_filter_add_match_subsystem_devtype(mon, SUBSYSTEM, NULL);
    udev_monitor_enable_receiving(mon);

    int fd = udev_monitor_get_fd(mon);

    while (1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);

        int ret = select(fd+1, &fds, NULL, NULL, NULL);
        if (ret <= 0)
            break;

        if (FD_ISSET(fd, &fds)) {
            struct udev_device* dev = udev_monitor_receive_device(mon);
            process_device(dev);
        }
    }
}



int main(void)
{
    struct udev* udev = udev_new();
    if (!udev) {
        fprintf(stderr, "udev_new() failed\n");
        return 1;
    }

    monitor_devices(udev);

    udev_unref(udev);
    return 0;
}
