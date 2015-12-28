#include "slockengine.h"

extern "C" int  UTIL_GetIncomingCallStatus();
extern "C" int  UTIL_GetCallConnectedStatus(void);  
extern "C" int  UTIL_GetTimingPhoneLock();
extern "C" int  UTIL_GetPhoneInCall();
extern "C" int PM_setupLcdSleepTime(int sleepseconds);

void SLockEngine :: getParameter()
{
    SETUP_SET_Manager mgr("./sLocker.conf", true);//配置文件
    mgr.getSettingInfo("setup","S.reCallKey.1",reCallKey1,4115);
    mgr.getSettingInfo("setup","S.reCallKey.2",reCallKey2,4115);
    mgr.getSettingInfo("setup","S.reCallKey.n",keyRepeats,4);
    mgr.getSettingInfo("setup","S.screenBrightLevel",lock_brightness,10);
    mgr.getSettingInfo("setup","S.showTime",lock_light_timeout,5);
    mgr.getSettingInfo("setup","S.autoSec",autoSec,10);

    mgr.getSettingInfo("setup","S.exitArea.x0",exitX0,220);
    mgr.getSettingInfo("setup","S.exitArea.x1",exitX1,240);
    mgr.getSettingInfo("setup","S.exitArea.y0",exitY0,280);
    mgr.getSettingInfo("setup","S.exitArea.y1",exitY1,320);
    mgr.getSettingInfo("setup","S.configArea.x0",configX0,200);
    mgr.getSettingInfo("setup","S.configArea.x1",configX1,240);
    mgr.getSettingInfo("setup","S.configArea.y0",configY0,0);
    mgr.getSettingInfo("setup","S.configArea.y1",configY1,40);

    mgr.getSettingInfo("setup","L.ECOmode",eco,true);
}

void SLockEngine :: initial( )
{
    getParameter();
    startup = true;
    SETUP_SET_Manager cfg(AM_Global::ezx_system,true);//4D
    cfg.getSettingInfo("SYS_SYSTEM_SET","LcdSleepTime",syslcdsleeptime,0);
    cfg.getSettingInfo("SYS_SYSTEM_SET","Brightness",sys_brightness,30);
    sys_lcdsleeptime=lcdsleeptime_ref[syslcdsleeptime];
    sys_brightness=sys_brightness*2;

    backlightctrl(true,lock_brightness);
    if(eco)
    {
        PM_setupLcdSleepTime(sys_lcdsleeptime*2);
    }
    else
    {
        PM_setupLcdSleepTime(0);
    }

    dbg_printf("lcdsleeptime:%d\nbrightness:%d\n",sys_lcdsleeptime,sys_brightness);
}

void SLockEngine :: checkprocess( )
{
    if ((canvas->getcanvasSH())&&req_lightoff)
    {
        canvas->reDraw(0,0,240,320);
    }

    if (!(canvas->getcanvasSH())||(canvas->getEng())) {
        timeout = 0;    //disable backlight timeout
    }
    else
    {
        if(timeout > lock_light_timeout)
        {
            timeout = 0;
            backlightctrl(false);
        }
        if(req_lightoff) timeout ++;
    }

    if( true == keypressed)
    {
        if ( timecnt > keyRepeats){
            timecnt = 0;
            hidepressed = false;
            showpressed = false;
            keypressed = false;
        }
        timecnt ++;
    }
    autolock();
}

void SLockEngine :: QCopReceived(int message)
{
    static bool flag = false;
    if (message == 5) {
        dbg_printf("qmsg : %d %d %d %d\n",canvas->getcanvasSH(),canvas->isActiveWindow(),flag,startup);
        if ((canvas->getcanvasSH()) && (canvas->isActiveWindow()) && false == flag && false == startup) {
            backlightctrl(false);
            canvas->setcanvasSH(true);
            dbg_printf("force active:%d\n",canvas->isActiveWindow());
        }
        else
        {
            flag = false;
            startup = false;
        }
        if((canvas->getcanvasSH()) && (false == startup) ) {
            if(backlightstatus()) req_lightoff = true;
        }
    }
}

void SLockEngine :: PropertyReceived( )
{
    if ((canvas->isVisible()) && !startup)
    {
        dbg_printf("What's happend, let's see...\n");
        canvas->setcanvasSH(false);
        if(!backlightstatus()) backlightctrl(true,lock_brightness);
        timeout = 0;
    }
}

void SLockEngine :: pointerPressed( int x, int y )
{
    //dbg_printf("MousePressed %d %d\n",x,y);
    timeout = 0;
    if (!(canvas->getcanvasSH()))return;

    //配置区域
    if ((x>configX0)&&(x<configX1)&&(y>configY0)&&(y<configY1))
    {
        dbg_printf("setting...\n");
        PM_setupLcdSleepTime(sys_lcdsleeptime);
        backlightctrl(true,sys_brightness);
        canvas->setEng(true);
        canvas->setapp();
    }
    //退出区域
    if ((x>exitX0)&&(x<exitX1)&&(y>exitY0)&&(y<exitY1))
    {
        dbg_printf("exit...\n");
        backlightctrl(true,sys_brightness);
        PM_setupLcdSleepTime(sys_lcdsleeptime);
        canvas->setEng(true);
        canvas->exit();
    }
}

void SLockEngine :: pointerReleased( int x, int y )
{
    dbg_printf("MouseReleased %d %d\n",x,y);
}

void SLockEngine :: keyPressed(int keycode)
{
    timeout = 0;
    if ((canvas->getEng()))return;
    keypressed = true ;
    timecnt = 0;
    if (reCallKey1 == keycode)
    {
        showpressed = true;
    }
    else if(reCallKey2 == keycode && true == showpressed)
    {
        showpressed = false;
        if ((canvas->isVisible()))
        {
            if (backlightstatus())
            {
                backlightctrl(false,lock_brightness);
            }
            else
            {
                backlightctrl(true,lock_brightness);
            }
        }
        else
        {
            canvas->setcanvasSH(true);
        }
    }
    else
    {
        showpressed = false;
    }
}

void SLockEngine :: autolock()
{
    if ((canvas->getcanvasSH()) || !(canvas->getAutoLock())||(canvas->getEng())) {   //if autolock_interval equals 0, then autolock is disabled
        return;
    }

    static int cnt = 0;
    if (UTIL_GetTimingPhoneLock() || UTIL_GetPhoneInCall())
    {   //disable autolock while calling
        cnt = 0;
    }
    else
    {
        cnt ++;
    }
    if (cnt > autoSec)
    {
            dbg_printf("Screen is autolocked...\n");
            cnt = 0;
            backlightctrl(false);
            canvas->setcanvasSH(true);
    }
}


void SLockEngine :: backlightctrl(bool onoff,int brightness)
{
    int fbd = open("/dev/fb0", O_RDWR);
    if(fbd<0){
    dbg_printf("can not open fb0\n");
    return;
    }
    if(onoff){
        ioctl(fbd,FBIOBLANK,0);
        ioctl(fbd,FBIOSETBRIGHTNESS,brightness);
        ioctl(fbd,FBIOSETBKLIGHT,onoff);
        req_lightoff = true;
    }else{
        ioctl(fbd,FBIOSETBRIGHTNESS,0);
        ioctl(fbd,FBIOSETBKLIGHT,onoff);
        ioctl(fbd,FBIOBLANK,0x3);
        req_lightoff = false;
    }
    close(fbd);
    canvas->setLCD(req_lightoff);
    dbg_printf("Set backlight %s\n",onoff == true ?  "ON" : "OFF");
    keylightctrl(onoff);
}

void SLockEngine :: keylightctrl(bool onoff)
{
    int keyd = open("/dev/keylight", O_RDWR);
    if(keyd<0){
    dbg_printf("can not open keylight device\n");
    return;
    }
    if(onoff){
        ioctl(keyd,0xf0,0);
    }else{
        ioctl(keyd,0xf2,0);
    }
    close(keyd);

    dbg_printf("Set keylight %s\n",onoff == true ?  "ON" : "OFF");
}

int SLockEngine :: backlightstatus( )
{

    int fbd = open("/dev/fb0", O_RDWR);
    int status = 0;
    if(fbd<0){
    dbg_printf("can not open fb0\n");
    return false ;
    }
 
    if(ioctl(fbd,FBIOGETBKLIGHT,&status) < 0)
    {
        dbg_printf("backlight status : get status error\n");
    }

    close(fbd);
    dbg_printf("Backlight status %d\n",status);
    return status;
}
