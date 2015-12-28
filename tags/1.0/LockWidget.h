#include <zapplication.h>
#include <zmainwidget.h>
#include <ZDirectPixmap.h>
//#include <ZVRClient.h>
#include <SetupSetManager.h>
#include <ezxpm.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qtextcodec.h>
#include <qdatetime.h>
#include <qwmatrix.h>
#include <iostream>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <fcntl.h>

#include "Slocksetting.h"

#ifndef _SLOCKCANVAS_H_
#define _SLOCKCANVAS_H_

#define DEBUG_PRINT 1
#define G2U(s) ( QTextCodec::codecForName("GBK")->toUnicode(s) )
#define U2G(s) ( QTextCodec::codecForName("GBK")->fromUnicode(s) )
#define dbg_printf(fmt,args...) \
    ((void) ({ \
     if (DEBUG_PRINT) \
        printf(fmt,## args); \
     0; \
    })) 

/* added for backlight control */
#define FBIOSETBKLIGHT          0x4619
#define FBIOGETBKLIGHT          0x461A
#define FBIOSETBRIGHTNESS       0x461B 
#define FBIOGETBRIGHTNESS       0x461C 

class SLockCanvas:public ZMainWidget
{
public:
    SLockCanvas(QWidget *parent);
    void init();
    bool canvasSH();
    bool getAutoLock();
protected:
    void timerEvent(QTimerEvent *);
private slots:
    void drawClock(QPainter *);
private:
    double radians;
    unsigned char timeout;
    int rateClock,xClock,yClock,baseCR,baseCG,baseCB;
    int secCR,secCG,secCB,minCR,minCG,minCB,houCR,houCG,houCB;
    int exitX0,exitY0,exitX1,exitY1,configX0,configY0,configX1,configY1;
    int vare,varf,tapiHandle;
    int x,y,x0,y0,xOffset,delayTime,keyBright,keyBrightAllow,reCallKey1,reCallKey2,keyRepeats;
    int timex,timey,timefs,datex,datey,datefs,timeR,timeG,timeB,dateR,dateG,dateB;
    int timerID[7],sysValue,oldValue,oldFig,hasNotify,forceFlash,recallBlank,hasEvent;
    int drawLine,drawText,alpha,language,callFig,whoSmall,maxHeight;
    int heightB,widthB,rateF,slideMode,slideTime;
    int quickLock,hasTime,hasDate,hasRect,hasTimeBar,hasSlideBar,hasClocker,forceAutoLock;
    int btIcon,phoneIcon,lockIcon,ringIcon,batteryIcon,phoneOnOff,ringVolume;
    int showTime,sysshowTime;
    bool down,isBright,isShow,phoneOF,speakerOF,autoLock,notFit,hasWallPaper;
    bool checkEvents,checkKeyPress,isFinish,onlyButton,hasPadlock,hasBT;
    QPixmap basePic,buttonB,headBar,signleT,signleB,signleP,signleC,signleS,signleBT,unLockP,timeBar,tmpPic,tmpPicN;
    enum callstate{idle = 0,incomingcall = 1,connecting = 3,connected = 4,droped = 6};
    enum calltype{gsm = 2,gprs = 3};
    QPixmap clockPic,hourP,minuteP,rectP,rulerPic;
    QImage aImage,bImage,headPic,timePic,bottomPic;
    QString dateStr,timeStr,picDir,backPic;
    QString weekHZ[7],strLanguage[5];
    QStringList fileList;
    QStringList::Iterator it;
    QFont sf,sft,sfText;
    QPoint baseN[12],baseF[12];
    void getParameter();
    void getDateTime();
    void getNotifyEvent();
    void getBTstatus();
    void getBatteryLevel();
    void getSignleLevel();
    void setSpeakerOnOff();
    void setBackGround();
    bool alphaProcess(QPixmap *,QString,QString,int,int,bool,int);
    void reDraw(int,int,int,int);
    void paintEvent( QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
};

#endif
