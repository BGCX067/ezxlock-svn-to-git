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
    bool getcanvasSH();
    void setcanvasSH(bool s);
    void setLCD(bool s);
    void setapp();
    void setEng(bool s);
    bool getEng();
    bool getAutoLock();
    void exit();
    void reDraw(int,int,int,int);
protected:
    void timerEvent(QTimerEvent *);
private slots:
    void drawClock(QPainter *);
private:
    double radians;
    unsigned char timeout;
    bool isEng;
    int sys_lcdsleeptime,syslcdsleeptime,sys_brightness,lock_brightness;
    int rateClock,xClock,yClock,baseCR,baseCG,baseCB;//钟大小、位置、颜色
    int secCR,secCG,secCB,minCR,minCG,minCB,houCR,houCG,houCB;//指针颜色
    int exitX0,exitY0,exitX1,exitY1,configX0,configY0,configX1,configY1;//退出、配置点击区域
    //int vare,varf,tapiHandle;
    int x,y,x0,y0,xOffset;
    int timex,timey,timefs,datex,datey,datefs,timeR,timeG,timeB,dateR,dateG,dateB;//时间日期位置颜色
    int timerID[5],sysValue,oldValue,oldFig,hasNotify,forceFlash,recallBlank,hasEvent;
    int drawLine,drawText,alpha,language,whoSmall,maxHeight;
    int heightB,widthB,rateF,slideMode,slideTime;
    int quickLock,hasTime,hasDate,hasRect,hasTimeBar,hasSlideBar,hasClocker,forceAutoLock;
    int btIcon,phoneIcon,lockIcon,ringIcon,batteryIcon,phoneOnOff,ringVolume;
    bool down,isBright,isShow,phoneOF,speakerOF,autoLock,notFit,hasWallPaper;
    bool isFinish,onlyButton,hasPadlock,hasBT;
    QPixmap basePic,buttonB,headBar,signleT,signleB,signleP,signleC,signleS,signleBT,unLockP,timeBar,tmpPic,tmpPicN;
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
    void paintEvent( QPaintEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
};

static const int lcdsleeptime_ref[] = {
    30,60,60*5,60*10,
};

#endif
