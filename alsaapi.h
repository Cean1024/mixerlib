#ifndef ALSAAPI_H
#define ALSAAPI_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <malloc.h>
#include "globaflags.h"

#define AUTOSETFRAME

//#undef INTERRUPUTEINPUT

//#define INTERRUPUTEINPUT

#define bufunit char
#define defaultframe 1152
#define n_buf 10


struct snd_pcm_params {
    snd_pcm_t *playback_handle; //PCM 设备句柄
    snd_pcm_hw_params_t *hw_params; //硬件信息和 PCM 流配置
    snd_pcm_sw_params_t *sw_params;
    snd_pcm_sframes_t frame_to_deliver;
    snd_pcm_stream_t streamRDWR;
    snd_pcm_access_t streamaccess;
    snd_pcm_uframes_t frames;
    snd_pcm_format_t pcm_format;
    int mode;
};

struct audioparam
{
    int RATE;       //采样频率
    int BITS;       //量化位数
    int CHANNELS;   //声道数目
    int DIR;
    int bytesperframe; //bytes per frame;
    int bufsize;
};


class alsaapi
{
public:
    alsaapi();
    ~alsaapi();


    r_status init (int sample_rate,int channels , int sample_bit);
    r_status write(char *buf ,int frames);
    r_status writei(char *buf ,int frames);
    void stop();
    int getbufsize();
    int getbytesperframe();
    int getframes();


protected:
    r_status audioinit ();



private:
    struct snd_pcm_params pcm_param;
    struct audioparam params;

};

#endif // ALSAAPI_H
