#ifndef _GETKEY_H_
#define _GETKEY_H_

#include <ZBaseDialog.h>
#include <qlabel.h>
#include <qstring.h>
#include "dapplication.h"

//#include "LockWidget.h"

class KeyWidget;
class KeyDialog : public ZBaseDialog
{
    Q_OBJECT
public:

    KeyDialog(QWidget *parent = 0,const char* name = 0,int modal = NO_MODAL,WFlags f = 0 );
    void setW(KeyWidget *kw);
    int getcode();

protected:
    bool qwsEvent(QWSEvent *);

private:

    int code;
    KeyWidget *k;
};

class KeyWidget : public QLabel
{
    Q_OBJECT
    public:
        KeyWidget(const QString & text, QWidget *parent, const char *name=0 ,WFlags f=0);
        int getKey();
        void setKey(int);
        QString C2S(int);
    protected:
        void mouseReleaseEvent(QMouseEvent *);

    private:
        int key;
};


#endif
