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

int vola=100;
int volb=100;
int kfd = 0;
struct termios cooked;


int volume_adjust(short  * in_buf, short  * out_buf, int in_vol)
{
    float weight;
    //static int vol_b;
   // static float weight_b;

    //if(in_vol != vol_b) {
    if(in_vol >=100)weight =1;
    else if(in_vol<=0)weight = 0;
    else {
        in_vol= (in_vol + 5)/10 ; /*四舍五入，取整*/
        weight = in_vol/10.0;       /*按照0 0.1 ，0.2 - 0.9，1的方式给数据取权重*/
    }
   // weight_b = weight;
    //vol_b = in_vol;
    //} else weight = weight_b;

    *out_buf = (*in_buf)*weight;

    return 0;
}

void Mix(char sourseFile[10][SIZE_AUDIO_FRAME],int number,char *objectFile)
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
        /*channal 2 vol adjust*/
        volume_adjust( (short*)(sourseFile[1] + i*2),(short*)(sourseFile[1] + i*2),volb);

        for (j=0;j<number;j++)
        {
            temp+=*(short*)(sourseFile[j]+i*2);
        }

        output=(int)(temp*f);
        if (output>MAX)
        {
            f=(double)MAX/(double)(output);
            output=MAX;
        }
        if (output<MIN)
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
            if(volb>=10) volb-=10;
            printf("LEFT volb:%d\n",volb);
            break;
        case KEYCODE_R:
            if(volb<=90) volb+=10;
            printf("RIGHT volb:%d\n",volb);
            break;
        case KEYCODE_U:
            if(vola<=90) vola+=10;
            printf("UP vola:%d\n",vola);
            break;
        case KEYCODE_D:
            if(vola>=10) vola-=10;
            printf("DOWN vola:%d\n",vola);
            break;
        default:
            printf("value: %c = 0x%02X = %d\n", inputchar, inputchar, inputchar);
        }
    }

}

void signal_handler(int sig)
{
    DEBUGLOG("in %s\n",__func__);
    tcsetattr(kfd, TCSANOW, &cooked);
    exit (0);
}

int main(int argc ,char * argv[])
{
    FILE * fp1,*fp2;//*fpm;
#ifdef OUTINPUTPCM
    if(argc < 2) {
        std::cout <<"input file_a file_b\n";
        exit (0);
    }
#endif

    int ret1,ret2;
    char sourseFile[3][SIZE_AUDIO_FRAME];
    pthread_t pid;
    alsaapi alsaobj;


    signal(SIGINT,signal_handler);
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
        ret1 = fread(buf,bufsize,1,fp1);
        if(ret1>0) alsaobj.writei(buf,frames);
        else exit(0);
        usleep(1000);
    }


#endif

#ifndef OUTINPUTPCM
    fp1 = fopen("/home/cean/work/test/mixerlib/room.wav","rb");
    if(fp1==NULL)
        perror("fopen");
    fp2 = fopen("/home/cean/work/test/mixerlib/zmjhb.wav","rb");
    if(fp2==NULL)
        perror("fopen");
#else
    fp1 = fopen(argv[1],"rb");
    if(fp1==NULL)
        perror("fopen");
    fp2 = fopen(argv[2],"rb");
    if(fp2==NULL)
        perror("fopen");
#endif
    ret1 = fread(sourseFile[0],56,1,fp1);
    ret2 = fread(sourseFile[1],56,1,fp2);
    //memset(buf,0,bufsize);
    DEBUGLOG("bufsize:%d\n",bufsize);
    pthread_create(&pid,NULL,threadbody,NULL);
    while(1)
    {
        ret1 = fread(sourseFile[0],SIZE_AUDIO_FRAME,1,fp1);
        ret2 = fread(sourseFile[1],SIZE_AUDIO_FRAME,1,fp2);
        //volume_adjust(&data1,&data1,vola);
        //volume_adjust(&data2,&data2,volb);


        if(ret1>0 && ret2>0)
        {
            Mix(sourseFile,2,sourseFile[2]);
            /*
            if( data1 < 0 && data2 < 0)
                date_mix = data1+data2 - (data1 * data2 / -(pow(2,16-1)-1));
            else
                date_mix = data1+data2 - (data1 * data2 / (pow(2,16-1)-1));*/

            //if(date_mix > pow(2,16-1) || date_mix < -pow(2,16-1))
            //    printf("mix error\n");
            alsaobj.write(sourseFile[2],frames);
            memset(sourseFile[2],0,bufsize);
        }
        else if( (ret1 > 0) && (ret2==0))
        {
            alsaobj.write(sourseFile[0],frames);
            memset(sourseFile[2],0,bufsize);
        }
        else if( (ret2 > 0) && (ret1==0))
        {
            alsaobj.write(sourseFile[1],frames);
            memset(sourseFile[2],0,bufsize);
        }
        else if( (ret1 == 0) && (ret2 == 0))
        {
            break;
        }


            //DEBUGLOG("index:%d\n",index);

    }
    fclose(fp1);
    fclose(fp2);

    printf("Done!\n");
}

