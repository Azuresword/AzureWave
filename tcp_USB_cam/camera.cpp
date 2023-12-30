
#include "camera.h"
int open_video_device(const char *device_name)
{
    int fd = 0;

    if (device_name == NULL)
    {
        return -1;
    }

    fd = open(device_name, O_RDWR);	/* 以读写方式打开；以只读方式打开导致内存映射出错 */
    if(fd < 0)
    {
        perror("open video device failed");
        return -1;
    };
    return fd;
}

int close_video_device(struct _v4l2_video *pdevice)
{
    int ret = 0;
    int i;

    for(i = 0; i<pdevice->mmap_buf_cnt; i++)
    {
        if (pdevice->mmap_buf[i].addr != 0x00)
        {
            ret = munmap(pdevice->mmap_buf[i].addr, pdevice->mmap_buf[i].size);
            if(ret < 0)
            {
                perror("munmap failed");
                continue;
            }
        }
    }

    if (pdevice->mmap_buf != 0x00)
    {
        free(pdevice->mmap_buf);
    }

    if (pdevice->fd != 0x00)
    {
        ret = close(pdevice->fd);
        if(ret < 0)
        {
            perror("close fd failed");
        }

    }
    return ret;
}


int query_video_device_cap(struct _v4l2_video *pdevice)
{
    int ret = 0;
    struct v4l2_capability 	cap = {0};
    struct v4l2_fmtdesc 	fmtdesc = {0};

    /* 查询摄像头信息 */
    ret = ioctl(pdevice->fd, VIDIOC_QUERYCAP, &cap);
    if(ret < 0)
    {
        perror("ioctl call \'VIDEO_QUERYCAP\' failed \n");
        return -1;
    }
    printf("video driver name:%s\n", cap.driver);
    printf("video device name:%s\n", cap.card);
    printf("video bus information:%s\n", cap.bus_info);
    printf("video driver version:%d\n", cap.version);
    printf("video capabilities:%x\n", cap.capabilities);

    /* 检查设备是否支持视频捕获 */
    if(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)
    {
        printf("the video device support capture\n");
    }

    /* 检查设备是否支持数据流 */
    if(!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        printf("the video device support stream\n");
    }

    /* 查询设备支持的输出格式 */
    fmtdesc.index = 0 ;
    fmtdesc.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("the video device support format:\n");
    while(ioctl(pdevice->fd,VIDIOC_ENUM_FMT,&fmtdesc) != -1)
    {
        printf("%d.%s\n", fmtdesc.index+1, fmtdesc.description);
        fmtdesc.index++;
    }
    return ret;
}


int set_video_device_par(struct _v4l2_video *pdevice)
{
    int ret = 0;
    struct v4l2_format format;


    /* 设置帧输出格式 */
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width = 640;						/* 像素宽度 */
    format.fmt.pix.height = 480;					/* 像素高度 */
    format.fmt.pix.pixelformat = V4L2_PIX_FMT_H264;	/* 输出格式，前提是摄像头支持该格式，V4L2_PIX_FMT_YYUV */
    format.fmt.pix.field = V4L2_FIELD_NONE;

    ret = ioctl(pdevice->fd, VIDIOC_S_FMT, &format);
    if (ret < 0)
    {
        perror("ioctl call \'VIDIOC_S_FMT\' failed");
    }
    return ret;
}


int set_video_device_mmap(struct _v4l2_video *pdevice)
{
    int ret = 0;
    int i = 0;
    struct v4l2_requestbuffers req_buf = {0};
    struct v4l2_buffer buf = {0};

    /* 申请内核缓存区 */
    pdevice->mmap_buf_cnt = 1;
    req_buf.count = 1; 	 			/* 缓存数目 */
    req_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req_buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(pdevice->fd, VIDIOC_REQBUFS, &req_buf);
    if(ret < 0)
    {
        perror("ioctl call \'VIDIOC_REQBUFS\' failed");
        return -1;
    }
    //此处做了风险未知的修改 142行
    pdevice->mmap_buf = static_cast<_mmap_buf *>(malloc(req_buf.count * sizeof(struct _mmap_buf)));
    if(pdevice->mmap_buf  == NULL)
    {
        perror("malloc memory failed");
        return -1;
    }

    /* 将内核态内存映射到用户态 */
    for(i=0; i<req_buf.count; i++)
    {
        buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index  = i;

        ret = ioctl(pdevice->fd, VIDIOC_QUERYBUF, &buf);
        if(ret < 0)
        {
            perror("ioctl call \'VIDIOC_QUERYBUF\' failed");
            return -1;
        }

        pdevice->mmap_buf[i].size = buf.length;
        pdevice->mmap_buf[i].addr = (char *)mmap(NULL, buf.length, PROT_READ|PROT_WRITE, MAP_SHARED, pdevice->fd, buf.m.offset);

        if(MAP_FAILED == pdevice->mmap_buf[i].addr)
        {
            perror("mmap failed");
            return -1;
        }
    }
    return 0;
}


int set_video_device_stream_queue(struct _v4l2_video *pdevice, int index)
{
    int ret = 0;
    struct v4l2_buffer buf = {0};

    /* 将内核缓存放入队列 */
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    buf.index = index;
    ret = ioctl(pdevice->fd, VIDIOC_QBUF, &buf);
    if(ret < 0)
    {
        perror("ioctl call \'VIDIOC_QBUF\' failed");
        return -1;
    }

}

int set_video_device_stream_on(struct _v4l2_video *pdevice)
{
    int ret = 0;
    int i;
    struct v4l2_buffer buf = {0};
    enum v4l2_buf_type type;

    /* 将内核缓存放入队列 */
    for (i=0; i<pdevice->mmap_buf_cnt; i++)
    {
        set_video_device_stream_queue(pdevice, i);
        if(ret < 0)
        {
            return -1;
        }
    }

    /* 开启数据流 */
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(pdevice->fd, VIDIOC_STREAMON, &type);
    if(ret < 0)
    {
        perror("ioctl call \'VIDIOC_STREAMON\' failed");
        return 0;
    }

    return 0;
}

int read_video_device_stream_frame(struct _v4l2_video *pdevice, int *out_buf_index)
{
    int ret = 0;
    int i;
    struct v4l2_buffer buf = {0};

    /* 从队列取出数据 */
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    ret = ioctl(pdevice->fd, VIDIOC_DQBUF, &buf);
    if(ret < 0)
    {
        perror("ioctl call \'VIDIOC_DQBUF\' failed");
        return -1;
    }
    if (buf.index > pdevice->mmap_buf_cnt)
    {
        printf("buf overflow[%d]\n", buf.index);
    }
    *out_buf_index = buf.index;

    return 0;
}

int set_video_device_stream_off(struct _v4l2_video *pdevice)
{
    int ret = 0;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(pdevice->fd, VIDIOC_STREAMOFF, &type);	/* 关闭数据流 */
    if(ret < 0)
    {
        perror("ioctl call \'VIDIOC_STREAMOFF\' failed");
        return -1;
    }
    return 0;
}
