#include "dapplication.h"
#include "slockengine.h"
#include "slockcanvas.h"

#define VERSION "Ver Beta 2.0 Build"

SLockEngine *engine;

void mainloop(int signo)
{
    if (SIGALRM == signo) {
        engine->checkprocess();
        signal(SIGALRM,mainloop);
        alarm(1);
    }
}

int main(int argc,char **argv)
{
    printf("sLocker by www.dnxh.cn %s %s, %s \n",VERSION,__TIME__,__DATE__);
    DApplication app(argc, argv);
    app.setDefaultCodec(QTextCodec::codecForName("GBK"));
    QString APPDIR(argv[0]);
    APPDIR.replace(strlen(argv[0])-7,7,"");
    QDir::setCurrent(APPDIR);
    putenv("TZ=GMT-08:00");

    SLockCanvas *mywidget= new SLockCanvas(NULL);
    engine = new SLockEngine( mywidget );
    mywidget->showFullScreen();
    engine->initial( );
    app.setPointerListener( (PointerListener*) engine );

    signal(SIGALRM,mainloop);
    alarm(1);
    return app.exec();
}


