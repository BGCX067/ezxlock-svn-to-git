#ifndef _SLOCKSETTING_H_
#define _SLOCKSETTING_H_
#include "slockcanvas.h"
#include "dapplication.h"
#include "getkey.h"

#include <ezxutilcst.h>
#include <qcheckbox.h>
#include <ZScrollView.h>
#include <ZPushButton.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qspinbox.h> 

class LockSetWidget : public ZMainWidget
{
    Q_OBJECT
    public:
        LockSetWidget(const QString& title, bool editorMode, QWidget* parent,const char* name = 0,WFlags f = 0 );
        void setLockWidget(SLockCanvas*);

    public slots:
        void saveset();
        void readset();
        void closeWindow();

    private:
    int drawLine,drawText,hasTimeBar,hasSlideBar,hasRect,reCallKey1,reCallKey2,keyRepeats,recallBlank;
    int hasTime,timex,timey,timefs,timeR,timeG,timeB;
    int hasDate,datex,datey,datefs,dateR,dateG,dateB;
    int alpha,autoSec;
    int hasClocker,rateClock,xClock,yClock;
    int baseCR,baseCG,baseCB,houCR,houCG,houCB,minCR,minCG,minCB,secCR,secCG,secCB;
    int batteryIcon,phoneIcon,ringIcon,lockIcon,btIcon,phoneOnOff,quickLock,forceAutoLock;
    int exitX0,exitX1,exitY0,exitY1,configX0,configX1,configY0,configY1;
    int oldValue,language,slideMode,slideTime,showTime;
    bool eco;
    QString picDir;
    SLockCanvas *p;
    QSpinBox *keyrepeats,*screenbright,*showtimes,*autolocksec;
    KeyWidget *recallkey1,*recallkey2;
    QCheckBox *drawline,*drawtext,*showtimebar,*showslidebar,*showframe,*recallblank,*enablebatteryicon,*autolock;
    QCheckBox *enablephoneicon,*enableringicon,*enablelockicon,*enableBTicon,*enablephoneonoff;
    QCheckBox *hasTimeBox,*hasDateBox,*hasClockerBox,*slideModeBox,*ECOmode;
};

#endif
