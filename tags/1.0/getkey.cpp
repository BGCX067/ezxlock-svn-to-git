#include "getkey.h"
#include <qwallpaper.h>

KeyDialog::KeyDialog(QWidget *parent,const char* name,int modal,WFlags f):
           ZBaseDialog(parent, name, modal, f)
{
    setFont(QFont::QFont("", 15, 75));
    QLabel *msg=new QLabel(tr("Press key"),this,"msg");
    msg->setAlignment(Qt::AlignCenter );
    msg->setGeometry(0,0,230,45);
}

int KeyDialog::getcode()
{
    return code;
}

void KeyDialog::setW(KeyWidget *kw)
{
    k = kw;
}

bool KeyDialog::qwsEvent(QWSEvent *e)
{
    //int code;
    QWSKeyEvent* KeyEvent =0;
    KeyEvent = (QWSKeyEvent*) e;
    if( e->type == QWSEvent::Key)
    {
        code = KeyEvent->simpleData.keycode;
        //printf("%d\n",code);
        k->setText(k->C2S(code));
        k->setKey(code);
        QDialog::accept();
        return true;
    }
    return false;
}

KeyWidget::KeyWidget(const QString & text, QWidget *parent, const char *name ,WFlags f):
         QLabel( text,parent, name,  f)
{
    setPalette( QPalette( QColor( 0, 0, 0) ) );
    setAlignment(Qt::AlignCenter );
}

int KeyWidget::getKey()
{
  return  key;
}

void KeyWidget::setKey(int n)
{
    key = n;
}

QString KeyWidget :: C2S(int c)
{
    QString s;
    switch(c)
    {
        case 4118:
            s = tr("VolUp");
            break;
        case 4119:
            s = tr("VolDown");
            break;
        case 4100:
            s = tr("OK");
            break;
        case 4115:
            s = tr("Up");
            break;
        case 4117:
            s = tr("Down");
            break;
        case 4114:
            s = tr("Left");
            break;
        case 4116:
            s = tr("Right");
            break;
        case 4144:
            s = tr("Green");
            break;
        case 4145:
            s = tr("Red");
            break;
        case 4147:
            s = tr("Music");
            break;
        case 4173:
            s = tr("Custom");
            break;
        case 4148:
            s = tr("Camera");
            break;
        case 4101:
            s = tr("Record");
            break;
        case 4149:
            s = tr("Voice");
            break;
        case 4169:
            s = tr("Play");
            break;
        case 4171:
            s = tr("Prev");
            break;
        case 4172:
            s = tr("Next");
            break;
        default:
            s = QString("%1").arg(c);
    }
    return s;
}

void KeyWidget::mouseReleaseEvent(QMouseEvent *e)
{
    dbg_printf("wstate is ok %d %d\n",e->x(),e->y());
    KeyDialog *kl = new KeyDialog(0, "dlg", true);
    kl->setW(this);
    kl->setGeometry(5,135,230,45);
    kl->show();
}
