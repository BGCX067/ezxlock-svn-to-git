
#ifndef _SCREENLOCKENGINE_H_
#define _SCREENLOCKENGINE_H_

#include <ZMessageBox.h>
#include <qthread.h>
#include <qfile.h>
#include <qtextstream.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <fcntl.h>

#include "dapplication.h"
#include "slockcanvas.h"

/* added for backlight control */
#define FBIOSETBKLIGHT          0x4619
#define FBIOGETBKLIGHT          0x461A
#define FBIOSETBRIGHTNESS       0x461B 
#define FBIOGETBRIGHTNESS       0x461C 


class SLockEngine : public PointerListener
{
    private:
        SLockCanvas *canvas;
        bool keypressed,hidepressed,showpressed,ishide,req_lightoff,startup;
        int timecnt,timeout,keyBright,keyBrightAllow,reCallKey1,reCallKey2,keyRepeats;
        int sys_lcdsleeptime,syslcdsleeptime,sys_brightness,lock_brightness;
        int lock_light_timeout,autoSec;
        int exitX0,exitY0,exitX1,exitY1,configX0,configY0,configX1,configY1;//ÍË³ö¡¢ÅäÖÃµã»÷ÇøÓò
        bool isSet,isExit,eco;

        void backlightctrl(bool onoff,int brightness = 10);
        void keylightctrl(bool onoff);
        int backlightstatus( );

    public:
        SLockEngine( SLockCanvas *canvas_ )
        {
            canvas = canvas_;
            keypressed = false;
            hidepressed = false;
            showpressed = false;
            ishide = false;
            timecnt = 0;
            timeout = 0;
        }
        void initial( );
        void checkprocess( );
        void pointerPressed( int x, int y );
        void pointerReleased( int x, int y );
        void keyPressed(int keycode);
        void QCopReceived(int message);
        void PropertyReceived( );
        void autolock();
        void getParameter();
};

#endif

