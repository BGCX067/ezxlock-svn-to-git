#include "slockcanvas.h"
#include "getkey.h"

//extern "C" int  sendKey(int keycode,int times);
//extern "C" int  checkEvent(int key);
extern "C" int  checkKey(int key1,int key2,int pressTimes);
extern "C" void phoneof(int onORoff);
extern "C" int scrCtl(int level,int method);     //control screen bright,get screen status
extern "C" int UTIL_GetNotificationStatus();     //return 1 if have notifications
extern "C" int UTIL_GetBatteryLevel();     //return battery level 5,4,3,2,1,0...
extern "C" int UTIL_GetSignalQuality();     // return signal level 5,4,3,2,1,0...
extern "C" bool UTIL_GetKeypadLockStatus(void);    //获得硬件锁状态
extern "C" bool UTIL_GetBtStatus();    //获得狼牙状态
extern "C" int  UTIL_GetIncomingCallStatus();    //取呼入状态,无返回0，有返回1
extern "C" int  UTIL_GetTimingPhoneLock();
extern "C" int  UTIL_GetPhoneInCall();

extern "C" int TAPI_CLIENT_Init(int *,int);
extern "C" void TAPI_CLIENT_Fini();
extern "C" int TAPI_CALL_GetAllCallSessions(char *);
struct callsession
{
    int numofcall;
    int connectid;
    int connecttype;//gsm=2,gprs=3
    int connectstatus;//connecting==3,connected==4;
    char callno[];
};
char bufe[1024];
struct callsession *pcs = (struct callsession*)bufe;

SLockCanvas::SLockCanvas(QWidget *parent) : ZMainWidget(parent, NULL, 0)
{
    //char tmp[2048];
    setBackgroundMode(NoBackground);
    weekHZ[0] = "一";weekHZ[1] = "二";weekHZ[2] = "三";weekHZ[3] = "四";
    weekHZ[4] = "五";weekHZ[5] = "六";weekHZ[6] = "日";
    strLanguage[0] = G2U("向右滑动解锁");
    strLanguage[1] = "slide to unlock";
    x0 = 17;
    y0 = 320-48;
    x  = 17;
    down = false;
    checkEvents = false;
    checkKeyPress = false;
    hasWallPaper = true;
    isFinish = false;
    forceFlash = 0;
    callFig = 0;
    speakerOF = true;
    hasNotify = 0;
    hasEvent = 0;
    phoneOF = true;
    onlyButton = false;
    autoLock = true;
    hasClocker = 0;
    oldFig = 0;
    sysValue = scrCtl(0,2);
    isBright = true;
    isShow = true;
/*
    VR_PLAYTTS_T vrt;
    vrt=vr1;
    ZVRClient zvr= ZVRClient();
    zvr.LaunchVR();
    if(zvr.isVRSupport())zvr.PlayoutText(G2U("哈哈哈，中午休息"),vrt);
*/
    init();
}
void SLockCanvas::init()
{
    getParameter();
    hasPadlock = false;
    sfText = QApplication::font();
    sfText.setPixelSize(15);
    sf = QApplication::font();
    sf.setPixelSize(datefs);
    sft = QApplication::font();
    sft.setPixelSize(timefs);
    sft.setBold(true);
    if (phoneIcon==1)
    {
        signleT.load("./res/RSS5.gif");
        if (signleT.isNull())
            phoneIcon = 0;
    }
    if (batteryIcon==1)
    {
        signleB.load("./res/BatteryFull.gif");
        if (signleB.isNull())
            batteryIcon = 0;
    }
    if (btIcon==1)
    {
        signleBT.load("./res/BT.gif");
        if (signleBT.isNull())
            btIcon = 0;
    }
    signleP.load("./res/Notify.gif");
    rulerPic.load("./res/ruler.jpg");
    if (ringIcon==1)
    {
        signleS.load("./res/RingerOn.gif");
        if (signleS.isNull())
            ringIcon = 0;
        else
        {
            SETUP_SET_Manager cfg("/tmp/ezx_alertstyle.cfg",true);
            cfg.getSettingInfo("ALERTSTYLE_CONFIG_TABLE","AlertSystemVolume",ringVolume,5);
            if (ringVolume<2)
                ringVolume = 5;
        }
    }
    if (lockIcon==1)
    {
        if (forceAutoLock==0)
            unLockP.load("./res/Lock.gif");
        else
            unLockP.load("./res/unLock.gif");
        if (unLockP.isNull())
            lockIcon = 0;
    }
    if (hasSlideBar==1)
    {
        signleC.load("./res/slideButton.png");
        if (signleC.isNull())
            hasSlideBar = 0;
    }
    if(slideMode==1)
    {
        QDir slideDir(picDir);
        fileList = slideDir.entryList("*.jpg;*.bmp;*.png;*.gif");
        if (fileList.isEmpty())
        {
            slideMode = 0;
            backPic = "./res/wallPaper.jpg";
        }
        else
        {
            it = fileList.begin();
            backPic = picDir+*it;
            ++it;
        }
    }
    else
        backPic = "./res/wallPaper.jpg";
    setBackGround();
    if (hasClocker==1)
    {
        int x,y;
        radians = 3.1416/180;
        for ( int i = 1; i < 13; i++ )
        {
            double degree = 30*i*radians;
            if ((i%3)==0)
            {
                x = (int) (0*cos(degree) - 40*sin(degree));
                y = (int) (40*cos(degree) + 0*sin(degree));
            }
            else
            {
                x = (int) (0*cos(degree) - 43*sin(degree));
                y = (int) (43*cos(degree) + 0*sin(degree));
            }
            baseN[i-1].setX(x+xClock);baseN[i-1].setY(y+yClock);
            x = (int) (0*cos(degree) - 46*sin(degree));
            y = (int) (46*cos(degree) + 0*sin(degree));
            baseF[i-1].setX(x+xClock);baseF[i-1].setY(y+yClock);
        }
    }
    memset(bufe,0,1024);
    vare = 0;varf = 0;
    tapiHandle = TAPI_CLIENT_Init(&vare,varf);
    timerID[0] = startTimer(1000);
    if (slideMode==1)
        timerID[3] = startTimer(slideTime);
    if (hasClocker==1)
        timerID[4] = startTimer(1000);
}
void SLockCanvas::getParameter()
{
    SETUP_SET_Manager mgr("./sLocker.conf", true);//配置文件
    mgr.getSettingInfo("setup","L.drawLine",drawLine,0);
    mgr.getSettingInfo("setup","L.drawText",drawText,1);
    mgr.getSettingInfo("setup","L.showTimeBar",hasTimeBar,1);
    mgr.getSettingInfo("setup","L.showSlideBar",hasSlideBar,1);
    mgr.getSettingInfo("setup","L.showFrame",hasRect,0);
    mgr.getSettingInfo("setup","S.lockKey.c",keyBright,4117);
    mgr.getSettingInfo("setup","S.lockKey.n",keyBrightAllow,1);
    mgr.getSettingInfo("setup","S.reCallKey.1",reCallKey1,4115);
    mgr.getSettingInfo("setup","S.reCallKey.2",reCallKey2,4115);
    mgr.getSettingInfo("setup","S.reCallKey.n",keyRepeats,4);
    mgr.getSettingInfo("setup","L.recallBlank",recallBlank,1);
    mgr.getSettingInfo("setup","L.timeDisplay",hasTime,1);
    mgr.getSettingInfo("setup","S.timePosition.x",timex,0);
    mgr.getSettingInfo("setup","S.timePosition.y",timey,15);
    mgr.getSettingInfo("setup","S.timeFontSize",timefs,40);
    mgr.getSettingInfo("setup","S.timeColor.r",timeR,255);
    mgr.getSettingInfo("setup","S.timeColor.g",timeG,255);
    mgr.getSettingInfo("setup","S.timeColor.b",timeB,255);
    mgr.getSettingInfo("setup","L.dateDisplay",hasDate,1);
    mgr.getSettingInfo("setup","S.datePosition.x",datex,0);
    mgr.getSettingInfo("setup","S.datePosition.y",datey,90);
    mgr.getSettingInfo("setup","S.dateFontSize",datefs,14);
    mgr.getSettingInfo("setup","S.dateColor.r",dateR,255);
    mgr.getSettingInfo("setup","S.dateColor.g",dateG,255);
    mgr.getSettingInfo("setup","S.dateColor.b",dateB,255);
    mgr.getSettingInfo("setup","S.alphaValue",alpha,120);
    mgr.getSettingInfo("setup","L.hasClock",hasClocker,1);
    mgr.getSettingInfo("setup","S.clockSizePosition.s",rateClock,3);
    mgr.getSettingInfo("setup","S.clockSizePosition.x",xClock,120);
    mgr.getSettingInfo("setup","S.clockSizePosition.y",yClock,190);
    mgr.getSettingInfo("setup","S.clockRuleColor.r",baseCR,255);
    mgr.getSettingInfo("setup","S.clockRuleColor.g",baseCG,255);
    mgr.getSettingInfo("setup","S.clockRuleColor.b",baseCB,255);
    mgr.getSettingInfo("setup","S.hourColor.r",houCR,255);
    mgr.getSettingInfo("setup","S.hourColor.g",houCG,255);
    mgr.getSettingInfo("setup","S.hourColor.b",houCB,255);
    mgr.getSettingInfo("setup","S.minuteColor.r",minCR,255);
    mgr.getSettingInfo("setup","S.minuteColor.g",minCG,255);
    mgr.getSettingInfo("setup","S.minuteColor.b",minCB,255);
    mgr.getSettingInfo("setup","S.secondColor.r",secCR,255);
    mgr.getSettingInfo("setup","S.secondColor.g",secCG,255);
    mgr.getSettingInfo("setup","S.secondColor.b",secCB,255);
    mgr.getSettingInfo("setup","L.enableIconBattery",batteryIcon,1);
    mgr.getSettingInfo("setup","L.enableIconPhone",phoneIcon,1);
    mgr.getSettingInfo("setup","L.enableIconRing",ringIcon,1);
    mgr.getSettingInfo("setup","L.enableIconLock",lockIcon,1);
    mgr.getSettingInfo("setup","L.enableIconBT",btIcon,1);
    mgr.getSettingInfo("setup","L.enablePhoneOnOff",phoneOnOff,1);
    mgr.getSettingInfo("setup","L.quickLock",quickLock,1);
    mgr.getSettingInfo("setup","L.autoLock",forceAutoLock,0);
    mgr.getSettingInfo("setup","S.exitArea.x0",exitX0,220);
    mgr.getSettingInfo("setup","S.exitArea.x1",exitX1,240);
    mgr.getSettingInfo("setup","S.exitArea.y0",exitY0,280);
    mgr.getSettingInfo("setup","S.exitArea.y1",exitY1,320);
    mgr.getSettingInfo("setup","S.configArea.x0",configX0,200);
    mgr.getSettingInfo("setup","S.configArea.x1",configX1,240);
    mgr.getSettingInfo("setup","S.configArea.y0",configY0,0);
    mgr.getSettingInfo("setup","S.configArea.y1",configY1,40);
    mgr.getSettingInfo("setup","S.screenBrightLevel",oldValue,10);
    mgr.getSettingInfo("setup","S.language",language,0);
    mgr.getSettingInfo("setup","L.slideMode",slideMode,0);
    mgr.getSettingInfo("setup","S.slideInterval",slideTime,10);
    mgr.getSettingInfo("setup","P.BGroundPicDir",picDir,"./BGroundPic/",true);
    mgr.getSettingInfo("setup","S.showTime",showTime,5);

    slideTime = slideTime * 1000;
    reCallKey1 = reCallKey1 & 0x00ff;
    reCallKey2 = reCallKey2 & 0x00ff;
    if (rateClock<1)
        rateClock = 1;
    if (rateClock>5)
        rateClock = 5;
    xClock = (xClock-120)/rateClock*2;
    yClock = (yClock-160)/rateClock*2;
    if (lockIcon==1)
        quickLock = 1;
}
void SLockCanvas::getSignleLevel()
{
    int sLevel;
    if (phoneOF)
        sLevel = UTIL_GetSignalQuality(); 
    else
        sLevel = 9;
    switch(sLevel)
    {
        case 0:
            signleT.load("./res/RSS0.gif");
            break;
        case 1:
            signleT.load("./res/RSS1.gif");
            break;
        case 2:
            signleT.load("./res/RSS2.gif");
            break;
        case 3:
            signleT.load("./res/RSS3.gif");
            break;
        case 4:
            signleT.load("./res/RSS4.gif");
            break;
        case 5:
            signleT.load("./res/RSS5.gif");
            break;
        case 9:
            signleT.load("./res/RSSOff.gif");
    }
    if (signleT.isNull())
        phoneIcon = 0;
}

void SLockCanvas::getNotifyEvent()
{
    hasNotify = UTIL_GetNotificationStatus(); 
}
void SLockCanvas::getBTstatus()
{
    hasBT = UTIL_GetBtStatus();
}
void SLockCanvas::getBatteryLevel()
{
    int bLevel;
    bLevel = UTIL_GetBatteryLevel(); 
    switch(bLevel)
    {
        case 0:
            signleB.load("./res/Battery0.gif");
            break;
        case 1:
            signleB.load("./res/Battery0.gif");
            break;
        case 2:
            signleB.load("./res/Battery1.gif");
            break;
        case 3:
            signleB.load("./res/Battery1.gif");
            break;
        case 4:
            signleB.load("./res/Battery2.gif");
            break;
        case 5:
            signleB.load("./res/BatteryFull.gif");
            break;
        default:
            signleB.load("./res/BatteryInvalid.gif");
    }
    if (signleB.isNull())
        batteryIcon = 0;
}
void SLockCanvas::setSpeakerOnOff()
{
    if (speakerOF)
        signleS.load("./res/RingerOn.gif");
    else
        signleS.load("./res/RingerOff.gif");
    if (signleS.isNull())
        ringIcon = 0;
}
void SLockCanvas::getDateTime()
{
    if ((hasTime==0) && (hasDate==0))
        return;
    QDate idate;
    QTime itime;
    //int iyear,imonth,iday,iweek;
    int ihour,iminute;
    QString dtTmp;
    idate = QDate::currentDate();
    itime = QTime::currentTime();
    if (language==0)
    {
        dateStr = QString("%1年%2月%3日星期%4")
            .arg(idate.year())
            .arg(idate.month())
            .arg(idate.day())
            .arg(weekHZ[idate.dayOfWeek()-1]);
    }
    if (language==1)
        dateStr = idate.toString();
    dateStr = G2U(dateStr);

    ihour = itime.hour();
    iminute = itime.minute();
    dtTmp.setNum(ihour,10);
    timeStr = dtTmp+":";
    dtTmp.setNum(iminute,10);
    if (iminute<10)
        timeStr = timeStr+"0"+dtTmp;
    else
        timeStr = timeStr+dtTmp;
}
void SLockCanvas::setBackGround()
{
    bool rValue,hasFile;
    QImage bPic;
    QPixmap bPix;
    hasFile = QFile::exists(backPic);
    if (hasFile)
    {
        bPic.load(backPic);
        bPix.load(backPic);
        if ((bPix.height()!=320)||(bPix.width()!=240))
        {
            int i;
            if (bPix.height()>bPix.width())
            {
                i = 1;
                bPic = bPic.smoothScale(320*bPix.width()/bPix.height(),320);
            }
            else
            {
                i = 2;
                bPic = bPic.smoothScale(240,240*bPix.height()/bPix.width());
            }

            basePic.resize(240,320);
            basePic.fill(black);
            QPixmap bbPic;
            bbPic.convertFromImage(bPic);
            QPainter pb;
            pb.begin(&basePic,this);
            if (i==1)
                pb.drawPixmap((240-bbPic.width())/2,0,bbPic);
            else
                pb.drawPixmap(0,(320-bbPic.height())/2,bbPic);
            pb.end();
            notFit = true;
        }
        else
        {
            basePic.load(backPic);
            notFit = false;
        }
    }
    else
    {
        basePic.resize(240,320);
        basePic.fill(black);
        notFit = false;
    }

    alphaProcess(&headBar,"./res/headBar.png",backPic,0,0,true,alpha);
    if (hasSlideBar==1)
    {
        headPic.load("./res/bottomBar.png");
        rValue = alphaProcess(&buttonB,"headBar",backPic,0,320-65,true,alpha);
        if (!rValue)
        {
            hasSlideBar = 0;
            drawText = 0;
        }
        heightB = buttonB.height();
        widthB = buttonB.width();
        if (drawText==1)
        {
            QPainter p;
            p.begin(&buttonB,this);
            p.setPen( gray );
            p.setFont(sfText);
            p.drawText(80,12,160,40,Qt::AlignCenter,strLanguage[language]);
            p.end();
        }
    }
    whoSmall = QMIN(timey,datey);
    if (hasTimeBar==1)
    {
        if (timey>datey)
            maxHeight = 5 + timey + timefs - datey;
        else
            maxHeight = 5 + datey + datefs - timey;
        timeBar.load("./res/timeBar.png");
        if (maxHeight>timeBar.height())
        {
            rValue = alphaProcess(&timeBar,"./res/timeBar.png",backPic,0,timey-(timeBar.height()-timefs)/2,true,alpha);
            maxHeight = 1;
            if (!rValue)
                hasTimeBar = 0;
        }
        else
        {
            rValue = alphaProcess(&timeBar,"./res/timeBar.png",backPic,0,whoSmall,true,alpha);
            maxHeight = 0;
            if (!rValue)
                hasTimeBar = 0;
        }
    }
    if (hasRect==1)
    {
        rValue = alphaProcess(&rectP,"./res/rect.png",backPic,0,0,true,alpha);
        if (!rValue)
            hasRect = 0;
    }
    tmpPic = basePic;
    QPainter p;
    p.begin(&tmpPic,this);
    if (hasRect==1)
        p.drawPixmap(0,0,rectP);
    if (hasSlideBar==1)
        p.drawPixmap(0,320-heightB,buttonB);
    if (hasTimeBar==1)
        if (maxHeight==0)
            p.drawPixmap(0,whoSmall,timeBar);
        else
            p.drawPixmap(0,timey-(timeBar.height()-timefs)/2,timeBar);
    p.end();
    tmpPicN = tmpPic;
    reDraw(0,0,240,320);
}
bool SLockCanvas::canvasSH()
{
    return isShow;
}

bool SLockCanvas::getAutoLock()
{
    return autoLock;
}

void  SLockCanvas::timerEvent(QTimerEvent *te)
{
//------------------------------------------------------------------ */
    if (te->timerId() == timerID[4]) //更新时钟，每秒一次
    {
        //std::cout << "timerEvent" << std::endl;
        if ((!isShow)||(!isBright))
            return;
        if (slideMode==1)
        {
            update(xClock-45,yClock-45,90,90);
        }else{
            reDraw(0,0,240,320);
        }
        return;
    }
//------------------------------------------------------------------
    if (te->timerId() == timerID[3]) //定时换背景图
    {
        if ((!isShow)||(!isBright)) return;
        if(it != fileList.end() && QFile::exists(backPic))
        {
            backPic = picDir+*it;
        }
        else
        {
            it = fileList.begin();
            backPic = picDir+*it;
        }
        ++it;
        setBackGround();
        return;
    }
//------------------------------------------------------------------
    if (te->timerId() == timerID[2]) //退出时动画效果
    {
        heightB = heightB - 1;
        y0 = y0 + 1;
        timey = timey - rateF;
        datey = datey - rateF;
        whoSmall = whoSmall - rateF;
        int cordY = whoSmall + timeBar.height();
        if (cordY<0)
        {
             killTimers();
             TAPI_CLIENT_Fini();
             qApp->quit();
            return;
        }
        else
        {
            QPainter p;
            tmpPic = basePic;
            p.begin(&tmpPic,this);
            p.drawPixmap(0,319-heightB,buttonB);
            p.drawPixmap(0,whoSmall,timeBar);
            p.end();
            reDraw(0,0,240,320);
        }
        return;
    }
//........................................................................    
    if (te->timerId() == timerID[1]) //定时画滑动钮(自动退回原位)
    {
        x = x - 4;
        if (x<x0)
        {
            killTimer(timerID[1]);
            x = x0;
            reDraw(x0,280,x0+40,40);
        }
        else
            reDraw(x-10,280,x+40,40);
        return;
    }
//...........................................................................
    if (te->timerId() == timerID[0]) //定时检查系统运行情况
    {
        forceFlash = forceFlash + 1;
        if (isShow)
            grabKeyboard();
        else
            releaseKeyboard();

        //定期刷新屏幕
        if (forceFlash > 59)
        {
            forceFlash = 0;
            if (timey>datey)
                reDraw(0,0,240,timey+timefs);
            else
                reDraw(0,0,240,datey+datefs);
        }
    }
}
void SLockCanvas::mousePressEvent(QMouseEvent *e)
{
    down = false;
    QString volString;
    if (!isBright)
    {
        if (timey>datey)
            reDraw(0,0,240,timey+timefs);
        else
            reDraw(0,0,240,datey+datefs);
        return;
    }
     //press hide/show wallpaper area
     if ((e->y()>300)&&(e->x()<19))
    {
        hasWallPaper = not hasWallPaper;
        reDraw(0,0,240,320);
        return;
    }
     //press slide button icon area
     if ((e->y()>320-48)&&(e->x()<50)&&(e->x()>19))
    {
        xOffset = e->x() - x0;
        x = x0;
        y = e->y();
        down = true;
        onlyButton = true;
        return;
    }
    //press config area
    if ((e->x()>configX0)&&(e->x()<configX1)&&(e->y()>configY0)&&(e->y()<configY1)) 
    {
        if (ringIcon==1)
        {
            if (ringVolume==5)
                ringVolume = 0;
            volString.setNum(ringVolume);
            pid_t pid=fork();
            if (!pid)
            {
                const char *args[50];
                int i=0;
                args[i++]="./chgRingV.lin";
                args[i++]=volString;
                args[i++]=NULL;
                execvp(args[0],(char* const*)args);
                return;
            }
        }
        killTimers();
        TAPI_CLIENT_Fini();
        hide();
        isShow = false;
        isBright = true;
        PM_setupLcdSleepTime(sysshowTime);
        LockSetWidget *set = new LockSetWidget(tr("Setting..."),true,0,"set");
        set->setLockWidget(this);
        set->show();
        dbg_printf("setting...\n");


    }
    //press exit area
    if ((e->x()>exitX0)&&(e->x()<exitX1)&&(e->y()>exitY0)&&(e->y()<exitY1)) 
    {
        PM_setupLcdSleepTime(sysshowTime);
        if (ringIcon==1)
        {
            if (ringVolume==5)
                ringVolume = 0;
            volString.setNum(ringVolume);
            pid_t pid=fork();
            if (!pid)
            {
                const char *args[50];
                int i=0;
                args[i++]="./chgRingV.lin";
                args[i++]=volString;
                args[i++]=NULL;
                execvp(args[0],(char* const*)args);
                return;
            }
        }
        if ((hasSlideBar==0)||(timey>datey)||(hasTimeBar==0))
        {
             killTimers();
             TAPI_CLIENT_Fini();
             qApp->quit();
            return;
        }

        int juTop;
        if (timey>datey)
            juTop = timey+timefs;
        else
            juTop = datey+datefs;
        rateF = juTop / heightB;
        QPainter p;
        p.begin(&buttonB,this);
        p.drawPixmap(x0,16,signleC);
        p.end();
        if (hasTimeBar==1)
        {
            p.begin(&timeBar,this);
            QColor c;
            c.setRgb(timeR,timeG,timeB);
                p.setPen(c);
            p.setFont(sft);
                p.drawText(timex,timey-whoSmall,240,timefs,Qt::AlignCenter,timeStr);
            c.setRgb(dateR,dateG,dateB);
                p.setPen(c);
            p.setFont(sf);
                p.drawText(datex,datey-whoSmall,240,datefs,Qt::AlignCenter,dateStr);
            p.end();
        }
        isFinish = true;
        onlyButton = true;
        killTimer(timerID[0]);
        killTimer(timerID[1]);
        killTimer(timerID[3]);
        killTimer(timerID[4]);
        killTimer(timerID[5]);
        timerID[2] = startTimer(0);
        return;
    }
     //press signle icon area
    if ((e->x()<27)&&(e->y()<15)&&(phoneOnOff==1))
    {
        if (phoneOF)
            phoneof(0);
        else
            phoneof(1);
        phoneOF = not phoneOF;
        onlyButton = false;
        reDraw(0,0,40,40);
        return;
    }
    //press speaker icon area
    if ((e->x()>(240-27-27))&&(e->x()<240-27)&&(e->y()<15)&&(ringIcon==1))
    {
        speakerOF = not speakerOF;
        onlyButton = false;
        reDraw(150,0,30,40);
        if (speakerOF)
            volString.setNum(ringVolume);
        else
            volString.setNum(0);
        pid_t pid=fork();
        if (!pid)
        {
            const char *args[50];
            int i=0;
            args[i++]="./chgRingV.lin";
            args[i++]=volString;
            args[i++]=NULL;
            execvp(args[0],(char* const*)args);
            return;
        }
    }
    //press information icon area,hide
    if ((e->x()>(120))&&(e->x()<(120+27))&&(e->y()<15)&&(hasNotify==1))
    {
        checkEvents = false;
        onlyButton = false;
        PM_setupLcdSleepTime(sysshowTime);
        isShow = false;
        hide();
        checkKeyPress = true;
        return;
    }
    //press lock icon area,blank or hide
    if ((e->x()>120-27)&&(e->x()<(120))&&(e->y()<15)&&(quickLock==1))
    {
        checkEvents = false;
        onlyButton = false;
        isShow = false;
        hide();
        checkKeyPress = true;
        if (forceAutoLock==1)
        {
            autoLock = false;
            killTimer(timerID[5]);
        }
        else
        {
            autoLock = true;
            timerID[5] = startTimer(1000);
        }
        return;
    }
    // other area, flash time and status;
    onlyButton = false;
    if (timey>datey)
        reDraw(0,0,240,40);
    else
        reDraw(0,0,240,40);
    return;
}
void SLockCanvas::mouseReleaseEvent(QMouseEvent *e)
{
    if (!down) return;
    //button auto returned to init position
    x = e->x() - xOffset;
    if (x<175)
    {
        onlyButton = true;
        if (hasSlideBar==1)
            timerID[1] = startTimer(15);
        else
            x = x0;
    }
    // auto hide,unlock
    else 
    {
        checkEvents = false;
        x = x0;
        if (hasSlideBar==1)
            reDraw(0,0,240,320);
        onlyButton = false;
        isShow = false;
        hide();
        if (forceAutoLock==0)
        {
            autoLock = false;
            killTimer(timerID[5]);
        }
        else
        {
            autoLock = true;
            timerID[5] = startTimer(1000);
        }
        checkKeyPress = true;
    }
}
void SLockCanvas::mouseMoveEvent(QMouseEvent *e)
{
    if (down)
    {
        x = e->x() - xOffset;
        y = e->y();
        if (x>174) x = 174;
        if (x<17) x = 17;
        onlyButton = true;
        if (hasSlideBar==1)
            reDraw(x-20,280,80,40);
    }
}
void SLockCanvas::keyReleaseEvent(QKeyEvent *pkey)
{
    pkey->count();
    if (!isBright) return;
    onlyButton = false;
    if (timey>datey)
        reDraw(0,0,240,timey+timefs);
    else
        reDraw(0,0,240,datey+datefs);
}
bool SLockCanvas::alphaProcess(QPixmap *resultPic,QString aName,QString bName,int bPicX,int bPicY,bool alphaC,int alphaValue)
{
    QImage aPic,bPic;
    QPixmap aPix,bPix;
//    ZDirectPixmap *aPiz,*bPiz;
    bool hasAlphaC;
    int aPicH,aPicW,bPicH,bPicW;
    int bx,by,b1,b2,b3,o1,o2,o3,i,j,k,r0,g0,b0,alphaV;
    alphaV = alphaValue;
    if (aName=="headBar")
        aPic = headPic;
    else if (aName=="timeBar")
        aPic = timePic;
    else if (aName=="bottomBar")
        aPic = bottomPic;
    else
        aPic.load(aName);
    aPix.convertFromImage(aPic,QPixmap::Auto);
    bPic = basePic.convertToImage();
    bPix = basePic;
    aPicW = aPix.width();
    aPicH = aPix.height();
    bPicW = bPix.width();
    bPicH = bPix.height();
    if ((aPicW>bPicW)||(aPicH>bPicH))
        return false;
    if (aPic.hasAlphaBuffer() && alphaC)
        hasAlphaC = true;
    else
        hasAlphaC = false;
    int validW,validH;
    validW = bPicW-aPicW;
    validH = bPicH-aPicH;
    bPicX = QMIN(validW,bPicX);
    //bPicY = QMIN(validH,bPicY);
    i = bPicX;
    k = i;
    j = bPicY;
    QRgb pb,po;
    for (by=0;by<aPicH;by++)
    {
        for (bx=0;bx<aPicW;bx++)
        {
            if ((j>=0)&&(j<bPicH))
            {
                pb = aPic.pixel(bx,by);
                po = bPic.pixel(k,j);
                b1 = qRed(pb); b2 = qGreen(pb); b3 = qBlue(pb);
                o1 = qRed(po); o2 = qGreen(po); o3 = qBlue(po);
                //if (hasAlphaC)
                    alphaV = qAlpha(pb);
                r0 = (alphaV*b1+(255-alphaV)*o1) / 255;
                g0 = (alphaV*b2+(255-alphaV)*o2) / 255;
                b0 = (alphaV*b3+(255-alphaV)*o3) / 255;
                aPic.setPixel(bx,by,qRgb(r0,g0,b0));
            }
            k = k + 1;
        }
        k = i;
        j = j + 1;
    }
    resultPic->convertFromImage(aPic,QPixmap::Color);
    return true;
}
void SLockCanvas::reDraw(int x,int y,int width,int height)
{
    if (!onlyButton)
    {
        if (!isFinish) getDateTime();
        getNotifyEvent();
        if (btIcon==1)
            getBTstatus();
        if (batteryIcon==1)
            getBatteryLevel();
        if (phoneIcon==1)
            getSignleLevel();
        if (ringIcon==1)
            setSpeakerOnOff();
    }
    update(x,y,width,height);
    return;
}
void SLockCanvas::paintEvent( QPaintEvent *e )
{
    e->rect();
    if (!hasWallPaper)
    {
        bitBlt(this,0,0,&rulerPic);
        return;
    }
    if (!isFinish)
    {
        QPainter p;                       // 绘制工具
            p.begin(&tmpPic,this);
            p.drawPixmap(0,0,headBar);                  // 开始在窗口部件上绘制
        if (phoneIcon==1)
            p.drawPixmap(0,0,signleT); //draw signle icon
        if (lockIcon==1)
            p.drawPixmap(120-27,0,unLockP); //draw unlock icon
        if (hasBT==1)
            p.drawPixmap(27,0,signleBT);
        if (ringIcon==1)
            p.drawPixmap(240-27-27,0,signleS); //draw speaker icon
        if (batteryIcon==1)
            p.drawPixmap(240-27,0,signleB); //draw battery icon
        if (hasNotify==1)
            p.drawPixmap(120,0,signleP); //draw notify icon
        if (drawLine==1)
        {
            p.setPen(white);
            p.drawRect(0,15 ,240,1);
        }
        QColor c;
        if (hasTime==1)
        {
            c.setRgb(timeR,timeG,timeB);
                   p.setPen(c);
            p.setFont(sft);
                   p.drawText(timex,timey,240,timefs,Qt::AlignCenter,timeStr);
        }
        if (hasDate==1)
        {
            c.setRgb(dateR,dateG,dateB);
                   p.setPen(c);
            p.setFont(sf);
                   p.drawText(datex,datey,240,datefs,Qt::AlignCenter,dateStr);
        }
        if (hasSlideBar==1)
            p.drawPixmap(x,y0,signleC);
        if (hasClocker==1)
            drawClock(&p);
            p.end();
    }
    bitBlt(this,0,0,&tmpPic);
    tmpPic = tmpPicN;
}
void SLockCanvas::drawClock( QPainter *painter)
{    
    painter->save();
    QTime cltime = QTime::currentTime();
    double degree,radians;
    int xx1,yy1;
    radians = 3.1416/180;
    painter->setWindow( -240/rateClock, -320/rateClock, 480/rateClock, 640/rateClock );
    painter->setPen(QPen(QColor(baseCR,baseCG,baseCB),2));
    for ( int i = 0; i < 12; i++ )
        painter->drawLine(baseN[i],baseF[i]);
    //painter->drawEllipse(-46+xClock,-46+yClock,92,92);//圆盘
    painter->setPen(QColor(houCR,houCG,houCB));
    painter->setBrush(QColor(houCR,houCG,houCB));
    degree = (30 * (cltime.hour()%12 + 6) + cltime.minute()/2) * radians;
    xx1 = (int) (0*cos(degree) - 25*sin(degree));
    yy1 = (int) (25*cos(degree) + 0*sin(degree));
    QPointArray hh;
    if (xx1==0)
        hh.putPoints(0,3,xClock+2,yClock,xClock-2,yClock,xx1+xClock,yy1+yClock);
    if (yy1==0)
        hh.putPoints(0,3,xClock,yClock+2,xClock,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1>0)&&(yy1>0))
        hh.putPoints(0,3,xClock-2,yClock+2,xClock+2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1>0)&&(yy1<0))
        hh.putPoints(0,3,xClock+2,yClock+2,xClock-2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1<0)&&(yy1>0))
        hh.putPoints(0,3,xClock+2,yClock+2,xClock-2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1<0)&&(yy1<0))
        hh.putPoints(0,3,xClock-2,yClock+2,xClock+2,yClock-2,xx1+xClock,yy1+yClock);
    painter->drawPolygon(hh);
    painter->setPen(QColor(minCR,minCG,minCB));
    painter->setBrush(QColor(minCR,minCG,minCB));
    degree = 6 * (cltime.minute()+30) * radians;
    xx1 = (int) (0*cos(degree) - 30*sin(degree));
    yy1 = (int) (30*cos(degree) + 0*sin(degree));
    QPointArray mm;
    if (xx1==0)
        mm.putPoints(0,3,xClock+2,yClock,xClock-2,yClock,xx1+xClock,yy1+yClock);
    if (yy1==0)
        mm.putPoints(0,3,xClock,yClock+2,xClock,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1>0)&&(yy1>0))
        mm.putPoints(0,3,xClock-2,yClock+2,xClock+2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1>0)&&(yy1<0))
        mm.putPoints(0,3,xClock+2,yClock+2,xClock-2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1<0)&&(yy1>0))
        mm.putPoints(0,3,xClock+2,yClock+2,xClock-2,yClock-2,xx1+xClock,yy1+yClock);
    if ((xx1<0)&&(yy1<0))
        mm.putPoints(0,3,xClock-2,yClock+2,xClock+2,yClock-2,xx1+xClock,yy1+yClock);
    painter->drawPolygon(mm);
    painter->setPen(QColor(secCR,secCG,secCB));
    painter->setBrush(QColor(secCR,secCG,secCB));
    degree = 6 * (cltime.second()+30) * radians;
    xx1 = (int) (0*cos(degree) - 40*sin(degree));
    yy1 = (int) (40*cos(degree) + 0*sin(degree));
    painter->drawLine(xClock,yClock,xx1+xClock,yy1+yClock);
    painter->drawEllipse(xClock-4,yClock-4,8,8);
    painter->setPen(QColor(secCR,secCG,secCB));
    painter->setBrush(QColor(baseCR,baseCG,baseCB));
    painter->drawEllipse(xClock-2,yClock-2,4,4);
    painter->restore();
}
