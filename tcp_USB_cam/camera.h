//
// Created by fsr on 23-4-14.
//

#ifndef USBCAM_PROJ_CAMERA_H
#define USBCAM_PROJ_CAMERA_H


#include <linux/videodev2.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/types.h>
#include <malloc.h>
#include <math.h>
#include <errno.h>
#include <assert.h>

struct _mmap_buf
{
    void *addr;
    int size;
};

struct _v4l2_video
{
    int fd;
    struct _mmap_buf *mmap_buf;
    int	mmap_buf_cnt;
};


int open_video_device(const char *device_name);
int close_video_device(struct _v4l2_video *pdevice);
int query_video_device_cap(struct _v4l2_video *pdevice);
int set_video_device_par(struct _v4l2_video *pdevice);
int set_video_device_mmap(struct _v4l2_video *pdevice);
int set_video_device_stream_queue(struct _v4l2_video *pdevice, int index);
int set_video_device_stream_on(struct _v4l2_video *pdevice);
int read_video_device_stream_frame(struct _v4l2_video *pdevice, int *out_buf_index);

int set_video_device_stream_off(struct _v4l2_video *pdevice);

#endif //USBCAM_PROJ_CAMERA_H
