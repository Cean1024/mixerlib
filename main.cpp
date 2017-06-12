#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <cmath>
#include <termios.h>
#include <pthread.h>
#include <signal.h>

#include "alsaapi.h"
using namespace std;


#define SIZE_AUDIO_FRAME (BUFFSIZE * 4)
#define  VOL_STEP 2
#define BUFNUMBER 4

int vola=100;
int volb=100;
int kfd = 0;

struct termios cooked;


int volume_adjust(char sourseFile[BUFNUMBER][SIZE_AUDIO_FRAME],int index, int in_vol)
{
    float weight;
    short *p;
    if(in_vol >=100)weight =1;
    else if(in_vol<=0)weight = 0;
    else weight = in_vol/100.0;       /*按照0 0.1 ，0.2 - 0.9，1的方式给数据取权重*/
    p = (short *)sourseFile[index] ;

    for (int i=0;i<SIZE_AUDIO_FRAME/2;i++)
    {
        *(p + i) = (*(p + i))*weight;
    }

    return 0;
}

void Mix(char sourseFile[BUFNUMBER][SIZE_AUDIO_FRAME],int number,char *objectFile)
{
    //归一化混音
    int const MAX=32767;
    int const MIN=-32768;

    double f=1;
    int output;
    int i = 0,j = 0;
    for (i=0;i<SIZE_AUDIO_FRAME/2;i++)
    {
        int temp=0;


        for (j=0;j<number;j++)
        {
            temp+=*(short*)(sourseFile[j]+i*2);
        }

        output=(int)(temp*f);
        if (output>MAX)
        {
            f=(double)MAX/(double)(output);
            output=MAX;
        } else if (output<MIN)
        {
            f=(double)MIN/(double)(output);
            output=MIN;
        }
        if (f<1)
        {
            f+=((double)1-f)/(double)32;
        }
        *(short*)(objectFile+i*2)=(short)output;
    }
}


void * threadbody(void *arg)
{
    char inputchar;
    struct termios raw;

    pthread_t *pid=( pthread_t * )arg;
    pthread_detach(*pid);

    tcgetattr(kfd, &cooked); // 得到 termios 结构体保存，然后重新配置终端
    memcpy(&raw, &cooked, sizeof(struct termios));
    raw.c_lflag &=~ (ICANON | ECHO);
    // Setting a new line, then end of file
    raw.c_cc[VEOL] = 1;
    raw.c_cc[VEOF] = 2;
    tcsetattr(kfd, TCSANOW, &raw);

    puts("Reading from keyboard");
    puts("---------------------------");
    puts("Use arrow keys to move the robot.");
    puts("otherwise the key values will be printed");

    while(1){
        if(read(kfd, &inputchar, 1) < 0)
        {
            perror("read():");
            exit(-1);
        }
        switch(inputchar)
        {
        case KEYCODE_L:
            if(volb>=VOL_STEP *2) volb-=VOL_STEP;
            printf("LEFT  volb:%3d%%\r",volb);
            fflush(stdout);
            break;
        case KEYCODE_R:
            if(volb<=(100 - VOL_STEP)) volb+=VOL_STEP;
            printf("RIGHT volb:%3d%%\r",volb);
            fflush(stdout);
            break;
        case KEYCODE_U:
            if(vola<=(100 - VOL_STEP)) vola+=VOL_STEP;
            printf("UP    vola:%3d%%\r",vola);
            fflush(stdout);
            break;
        case KEYCODE_D:
            if(vola>=VOL_STEP*2) vola-=VOL_STEP;
            printf("DOWN  vola:%3d%%\r",vola);
            fflush(stdout);
            break;
        default: break;
        //    printf("value: %c = 0x%02X = %d\n", inputchar, inputchar, inputchar);
        }
    }

}

void signal_handler(int sig)
{
    DEBUGLOG("in %s\n",__func__);
    tcsetattr(kfd, TCSANOW, &cooked);
    exit (0);
}
void * showtest(void *p)
{
    pthread_t *pid=( pthread_t * )p;
    pthread_detach(*pid);
    int index;
    for(int n=0;n<SIZE_AUDIO_FRAME/2;n++){
        index = (n * 44100/48000);
        //printf("n:%d  index:%d\n",n,index);
    }

}

void resample(char sourseFile[BUFNUMBER][SIZE_AUDIO_FRAME],int insize,int input_srate,int output_srate)
{
    int *Pin = (int *)sourseFile[3];
    int *Pout = (int *)sourseFile[0];
    int index,n;
    for(n =0;n < insize/4;n++)
    {
        index = n * input_srate/output_srate;
        Pout[n] = Pin[index];// + (Pin[index +1] - Pin[index]) * (n *  input_srate/output_srate - index);
    }
    printf("n:%d index:%d\n",n,index);

}

int main(int argc ,char * argv[])
{
    int fd1,fd2;
#ifdef OUTINPUTPCM
    if(argc < 2) {
        std::cout <<"input file_a file_b\n";
        exit (0);
    }
#endif
    int ret1,ret2;
    char sourseFile[BUFNUMBER][SIZE_AUDIO_FRAME];
    pthread_t pid1,pid2;
    alsaapi alsaobj;

    signal(SIGINT,signal_handler);
    pthread_create(&pid1,NULL,threadbody,(void *)&pid1);
    pthread_create(&pid2,NULL,showtest,(void *)&pid2);

#ifndef OUTINPUTPCM
    fd1 = open("/home/cean/work/test/mixerlib/room.wav",O_RDONLY);
    if(fd1==NULL)
        perror("fopen");
    fd2 = open("/home/cean/work/test/mixerlib/shenshenman.wav",O_RDONLY);
    if(fd2==NULL)
        perror("fopen");
#else
    fd1 = open(argv[1],O_RDONLY);
    if(fd1==NULL)
        perror("fopen");
    fd2 = fopen(argv[2],O_RDONLY);
    if(fd2==NULL)
        perror("fopen");
#endif

    ret1=alsaobj.init(48000,2,16);
    if(ret1< 0) {
        perror("alsa");
        exit (-1);
    }
    int bufsize = alsaobj.getbufsize();
    int frames = alsaobj.getframes();
   // char * buf = (char *)malloc( bufsize );
    //short *data1 =(short *)malloc( bufsize );
   // short *data2 =(short *)malloc( bufsize );

#if 0

    while(1)
    {
        ret1 = read(fd1,buf,bufsize);
        if(ret1>0) alsaobj.writei(buf,frames);
        else exit(0);
        usleep(1000);
    }


#endif

    ret1 = read(fd1,sourseFile[0],56);
    ret2 = read(fd2,sourseFile[1],56);
    //memset(buf,0,bufsize);
    DEBUGLOG("bufsize:%d\n",bufsize);
    int framelow = frames * 44100/48000;
    int bufsizelow = framelow * alsaobj.getbytesperframe();
    int index ;


    while(1)
    {
        ret1 = read(fd1,sourseFile[3],bufsizelow);
        ret2 = read(fd2,sourseFile[1],SIZE_AUDIO_FRAME);

        if(ret1>0 && ret2>0)
        {   /*channal 1 vol adjust*/
            volume_adjust( sourseFile,0,vola);

            /*channal 2 vol adjust*/
            volume_adjust( sourseFile,1,volb);

            index =2;

            resample(sourseFile,SIZE_AUDIO_FRAME,44100,48000);

            Mix(sourseFile,index,sourseFile[2]);

        }
        else if( (ret1 > 0) && (ret2==0))
        {
            /*channal 1 vol adjust*/
            index =0;
            volume_adjust( sourseFile,index,vola);

        }
        else if( (ret2 > 0) && (ret1==0))
        {
            /*channal 2 vol adjust*/
            index =1;
            volume_adjust( sourseFile,1,volb);
        }
        else if( (ret1 == 0) && (ret2 == 0))
        {
            break;
        }
        alsaobj.write(sourseFile[index],frames);
        //memset(sourseFile[2],0,bufsize);

            //DEBUGLOG("index:%d\n",index);

    }
    close(fd1);
    close(fd2);

    printf("Done!\n");
}

