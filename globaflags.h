#ifndef GLOBAFLAGS_H
#define GLOBAFLAGS_H
#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <semaphore.h>


#define PLAYLISTFOLDER  "/home/samba/music"



/**************audio config***************/
#define CONFIG_AUDIOALSA
#define BUFFSIZE 3072
#define DOUBLECHANNEL
#define MAXLEN_AUDIODATACOUNT 1
#define DEFAULTCOUNTBYTE 4
#define SOUNDCARDNUM 2
#define MUSICECARD ( SOUNDCARDNUM - 1 )
#define INTERCUTCARD 0

#define TYPEOFPLAYLIST MUSICECARD
#define TYPEOFADDPLAY 0
/****************************************/


/***************http config****************/
#define HTTPLOGINURL            "/index.php?m=home&c=index&a=s_login&p=json"
#define HTTPGETPLAYLISTURL      "/index.php?m=music&c=playlist&a=s_playlist&type=sqlite&id="
#define HTTPGETFILESERVERURL    "/index.php?m=home&c=fileserver&a=s_action&p=json"
#define HTTPDOWNLOADFILEURL     "/index.php?m=music&c=file&a=s_down&key=key&token=token&format=mp3&fileid="
#define HTTPGETNOTICEURL        "/index.php?m=home&c=notify&a=s_reads"
#define HTTPUPLOADURL           "/index.php?m=home&c=log&a=s_logupload"
/******************************************/


/*************playlist config**************/
#define PLAYLISTCONTORLTABLE    "playlist_section"
#define PLAYLISTITEMTABLE       "playlist_item"
#define PLAYLISTNAMETABLE       "playlist"

#define ORDERPLAY   0
#define RANDOMPLAY  1

typedef void (*callbackofgetmusicnumber)(std::string &music , int type);

/******************************************/


/****************device config*************/
#define LOGFILE "/Log.d"
#define DBFILESTORAGEFOLDER "/playlist"
#define MUSICESTORAGEFOLDER "/download"

#define LOCALDBFILENAME "/var/run/localdbfile.db3"
#define LOCALDUPDATETIMEFILE "/UpdateTimeFile"

#define DEVICEIDFOLDER "/fileofdevice"
/*************************************/

#define DEBUG

#ifdef  DEBUG
#define DEBUGLOG(fmt,args...)  printf(fmt,##args)
#else
#define DEBUGLOG(fmt,args...)

#endif






enum keansthreadstatus {
    threadstatusexit=0,
    threadstatusnew,
    threadstatusruning
};
typedef  int r_status;
enum returnstatus {

    ERROR = -2,
    FAILED = -1,
    SUCCESS=0,
    PSUCCESS,
    PFAILED,
    FSUCCES,
    FFAILD,
    ISUCCESS,
    IFAILED,
    IGENOR,
    NEEDUPDATA,
    NOTNEEDUPDATA,
    EXIST,
    NOTEXIST,
    WAITING,
    YES,
    NO,
    PLAYSTART,
    PLAYSTOP,
    PLAYRUNING,
    PLAYWAITING
};

enum calltype {
    callwait = 0,
    callnotwait,
};

enum statusofnetwork {
    networkonline = 0,
    networkoffline,
    serveronline,
    serveroffline
};

enum statusofplaymusic {
    playok = 10,
    playerror,
    playback,
    playnext
};

enum platformselect {
    platformisrespberry = 0,
    platformisi386
};

enum user_signal{
    SwitchOverToNextMusic = 1024,
    SwitchOverToLastMusic
};



typedef struct Server_Status {
   int ServerStatusFlag;
   time_t ServerLoginTime;
   time_t heartbeatTime;
}Server_Status_t;


/*
 * playlist control struct
 *
 */
typedef struct timeformatdate {

    std::string timestart;
    std::string timestop;
    std::string localtime;
    std::string waittime;
    std::string intervaltime;
    int recordid;
    int prerecordid;
    int looptype;
    int loopcount;
    int volume;
    int datetype;
    int dateparam;
    int playtype;
    int typeofcard;
    int sleepafterplayover;

}timeformatdate_t;

//#define OUTINPUTPCM
#undef OUTINPUTPCM




#define KEYCODE_R 0x43
#define KEYCODE_L 0x44
#define KEYCODE_U 0x41
#define KEYCODE_D 0x42
#define KEYCODE_Q 0x71


typedef struct wave_tag

{

    char              ChunkID[4];       // "RIFF"标志

    unsigned  int ChunkSize;    // 文件长度(WAVE文件的大小, 不含前8个字节)

    char              Format[4];    // "WAVE"标志



    char             SubChunk1ID[4];   // "fmt "标志

    unsigned int    SubChunk1Size;    /*

                                    * 过渡字节(不定)

                                    * 16 for PCM. This is the size of the rest of the

                                    * Subchunk which follows this number.

                                    */

    unsigned short   AudioFormat;  /*

                                    * 格式类别(10H为PCM格式的声音数据)

                                    * PCM=1 (i.e. Linear quantization)

                                    * Values other than 1 indicate some form of compression.

                                    */

    unsigned short  NumChannels;  // 通道数(单声道为1, 双声道为2)

    unsigned int    SampleRate;       // 采样率(每秒样本数), 表示每个通道的播放速度

    unsigned int ByteRate;     /*

                                     * 波形音频数据传输速率, 其值为:通道数*每秒数据位数*每样本的数据位数/8

                                     * 播放软件可以利用该值估计缓冲区大小

                                     */

    unsigned short  BlockAlign;       /*

                                      * 每样本的数据位数(按字节算), 其值为:通道数*每样本的数据位值/8， 播放

                                     * 软件需要一次处理多个该值大小的字节数据, 以便将其值用于缓冲区的调整

                                     */

    unsigned short BitsPerSample;    /*

                                    * 每样本的数据位数, 表示每个声道中各个样本的数据位数. 如果有多个声道,

                                    * 对每个声道而言, 样本大小都一样

                                    */



    char              SubChunk2ID[4];   // 数据标记"data"

    unsigned int SubChunk2Size;    // 语音数据的长度

} WAVE;

#endif // GLOBAFLAGS_H

