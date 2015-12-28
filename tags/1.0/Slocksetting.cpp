#include "Slocksetting.h"

LockSetWidget::LockSetWidget(const QString& title, bool editorMode, QWidget* parent,const char* name,WFlags f):
             ZMainWidget(title,editorMode,parent,name,f)
{
    readset();
    ZScrollView *sv = new ZScrollView(this, "sv");
    sv->setFont(QFont::QFont("", 14, 75));

    UTIL_CST *lockCST = new UTIL_CST(this, tr("Save"),EZX_CST_BTN_MENU,"CST_Back");
    setContentWidget(sv);
    setCSTWidget(lockCST);
    QObject::connect( lockCST->getMidBtn(), SIGNAL(clicked()), this, SLOT(saveset()));
    QObject::connect( lockCST->getRightBtn(), SIGNAL(clicked()), this, SLOT(closeWindow()));

    QLabel *recallkeylabel = new QLabel(tr("快捷键"), sv->viewport(), "recallkeylabel");
    recallkeylabel->resize(240, 20);
    sv->addChild(recallkeylabel,5,0,true);

    recallkey1 = new KeyWidget("",sv->viewport(), "recallkey1");
    recallkey1->setKey(reCallKey1);
    recallkey1->resize(70, 20);
    recallkey1->setText(recallkey1->C2S(reCallKey1));
    sv->addChild(recallkey1,5,22);
    recallkey2 = new KeyWidget("",sv->viewport(), "recallkey2");
    recallkey2->setKey(reCallKey2);
    recallkey2->resize(70, 20);
    recallkey2->setText(recallkey1->C2S(reCallKey2));
    sv->addChild(recallkey2,80,22);
    keyrepeats = new QSpinBox(sv->viewport(), "keyrepeats");
    keyrepeats->setMinValue(2);
    keyrepeats->setPrefix(tr("间隔"));
    keyrepeats->setSuffix(tr("秒"));
    keyrepeats->setValue(keyRepeats);
    keyrepeats->resize(85, 20);
    sv->addChild(keyrepeats,155,22,true);

    QLabel *screenbrightlevellabel = new QLabel(tr("屏幕亮度"), sv->viewport(), "screenbrightlevellabel");
    screenbrightlevellabel->resize(115, 20);
    sv->addChild(screenbrightlevellabel,5,22*2);
    screenbright = new QSpinBox(sv->viewport(), "screenbright");
    screenbright->setMinValue(5);
    screenbright->setMaxValue(100);
    screenbright->setValue(oldValue);
    screenbright->resize(115, 20);
    sv->addChild(screenbright,125,22*2,true);

    QLabel *showtimelabel = new QLabel(tr("亮屏时长"), sv->viewport(), "showtimelabel");
    showtimelabel->resize(115, 20);
    sv->addChild(showtimelabel,5,22*3);
    showtimes = new QSpinBox(sv->viewport(), "showtimes");
    showtimes->setMinValue(5);
    showtimes->setMaxValue(1000);
    showtimes->setSuffix(G2U("秒"));
    showtimes->setValue(showTime);
    showtimes->resize(115, 20);
    sv->addChild(showtimes,125,22*3,true);

    QLabel *autolockseclabel = new QLabel(tr("自动锁屏时间"), sv->viewport(), "autolockseclabel");
    autolockseclabel->resize(115, 20);
    sv->addChild(autolockseclabel,5,22*4);
    autolocksec = new QSpinBox(sv->viewport(), "autolocksec");
    autolocksec->setMinValue(5);
    autolocksec->setValue(autoSec);
    autolocksec->setSuffix(G2U("秒"));
    autolocksec->resize(115, 20);
    sv->addChild(autolocksec,125,22*4,true);

    ECOmode = new QCheckBox(tr("省电模式"), sv->viewport(), "ECOmode");
    ECOmode->setChecked(eco);
    ECOmode->resize(240, 20);
    sv->addChild(ECOmode,5,22*5,true);

    hasTimeBox = new QCheckBox(tr("显示时间"), sv->viewport(), "hasTimeBox");
    hasTimeBox->setChecked(hasTime);
    hasTimeBox->resize(240, 20);
    sv->addChild(hasTimeBox,5,22*6,true);

    hasDateBox = new QCheckBox(tr("显示日期"), sv->viewport(), "hasDateBox");
    hasDateBox->setChecked(hasDate);
    hasDateBox->resize(240, 20);
    sv->addChild(hasDateBox,5,22*7,true);

    hasClockerBox = new QCheckBox(tr("显示虚拟钟"), sv->viewport(), "hasClockerBox");
    hasClockerBox->setChecked(hasClocker);
    hasClockerBox->resize(240, 20);
    sv->addChild(hasClockerBox,5,22*8,true);

    slideModeBox = new QCheckBox(tr("幻灯片"), sv->viewport(), "slideModeBox");
    slideModeBox->setChecked(slideMode);
    slideModeBox->resize(240, 20);
    sv->addChild(slideModeBox,5,22*9,true);

    drawline = new QCheckBox(tr("显示分割线"), sv->viewport(), "drawline");
    drawline->setChecked(drawLine);
    drawline->resize(240, 20);
    sv->addChild(drawline,5,22*10,true);
    drawtext = new QCheckBox(tr("显示提示"), sv->viewport(), "drawtext");
    drawtext->setChecked(drawText);
    drawtext->resize(240, 20);
    sv->addChild(drawtext,5,22*11,true);
    showtimebar = new QCheckBox(tr("显示时间背景条"), sv->viewport(), "showtimebar");
    showtimebar->setChecked(hasTimeBar);
    showtimebar->resize(240, 20);
    sv->addChild(showtimebar,5,22*12,true);
    showslidebar = new QCheckBox(tr("显示滑块"), sv->viewport(), "showslidebar");
    showslidebar->setChecked(hasSlideBar);
    showslidebar->resize(240, 20);
    sv->addChild(showslidebar,5,22*13,true);
    showframe = new QCheckBox(tr("显示画框"), sv->viewport(), "showframe");
    showframe->setChecked(hasRect);
    showframe->resize(240, 20);
    sv->addChild(showframe,5,22*14,true);
    recallblank = new QCheckBox(tr("锁屏立即黑屏"), sv->viewport(), "recallblank");
    recallblank->setChecked(recallBlank);
    recallblank->resize(240, 20);
    sv->addChild(recallblank,5,22*15,true);
    autolock = new QCheckBox(tr("滑块解锁自动锁屏"), sv->viewport(), "autolock");
    autolock->setChecked(forceAutoLock);
    autolock->resize(240, 20);
    sv->addChild(autolock,5,22*16,true);
    enablebatteryicon = new QCheckBox(tr("显示电池图标"), sv->viewport(), "enablebatteryicon");
    enablebatteryicon->setChecked(batteryIcon);
    enablebatteryicon->resize(240, 20);
    sv->addChild(enablebatteryicon,5,22*17,true);
    enablephoneicon = new QCheckBox(tr("显示信号图标"), sv->viewport(), "enablephoneicon");
    enablephoneicon->setChecked(phoneIcon);
    enablephoneicon->resize(240, 20);
    sv->addChild(enablephoneicon,5,22*18,true);
    enableringicon = new QCheckBox(tr("显示声音图标"), sv->viewport(), "enableringicon");
    enableringicon->setChecked(ringIcon);
    enableringicon->resize(240, 20);
    sv->addChild(enableringicon,5,22*19,true);
    enablelockicon = new QCheckBox(tr("显示小锁图标"), sv->viewport(), "enablelockicon");
    enablelockicon->setChecked(lockIcon);
    enablelockicon->resize(240, 20);
    sv->addChild(enablelockicon,5,22*20,true);
    enableBTicon = new QCheckBox(tr("显示蓝牙图标"), sv->viewport(), "enableBTicon");
    enableBTicon->setChecked(btIcon);
    enableBTicon->resize(240, 20);
    sv->addChild(enableBTicon,5,22*21,true);
    enablephoneonoff = new QCheckBox(tr("允许开关电话功能"), sv->viewport(), "enablephoneonoff");
    enablephoneonoff->setChecked(phoneOnOff);
    enablephoneonoff->resize(240, 20);
    sv->addChild(enablephoneonoff,5,22*22,true);


}
void LockSetWidget::closeWindow()
{
    dbg_printf("set widget is close\n");
    p->setcanvasSH(true);
    p->setEng(false);
    close();
}

void LockSetWidget::setLockWidget(SLockCanvas *l)
{
    p = l;
}
void LockSetWidget::readset()
{
    SETUP_SET_Manager mgr("./sLocker.conf", true);//配置文件
    mgr.getSettingInfo("setup","L.drawLine",drawLine,0);
    mgr.getSettingInfo("setup","L.drawText",drawText,1);
    mgr.getSettingInfo("setup","L.showTimeBar",hasTimeBar,1);
    mgr.getSettingInfo("setup","L.showSlideBar",hasSlideBar,1);
    mgr.getSettingInfo("setup","L.showFrame",hasRect,0);
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
    mgr.getSettingInfo("setup","L.ECOmode",eco,true);
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
    mgr.getSettingInfo("setup","S.autoSec",autoSec,10);
    mgr.getSettingInfo("setup","S.language",language,0);
    mgr.getSettingInfo("setup","L.slideMode",slideMode,0);
    mgr.getSettingInfo("setup","S.slideInterval",slideTime,10);
    mgr.getSettingInfo("setup","P.BGroundPicDir",picDir,"./BGroundPic/",true);
    mgr.getSettingInfo("setup","S.showTime",showTime,5);
}
void LockSetWidget::saveset()
{
    SETUP_SET_Manager mgr("./sLocker.conf", true);//配置文件
    mgr.setSettingInfo("setup","L.drawLine",drawline->isChecked());            //是否显示分割线
    mgr.setSettingInfo("setup","L.drawText",drawtext->isChecked());            //是否在滑槽上显示提示
    mgr.setSettingInfo("setup","L.showTimeBar",showtimebar->isChecked());       //是否显示时间条
    mgr.setSettingInfo("setup","L.showSlideBar",showslidebar->isChecked());     //是否显示滑块
    mgr.setSettingInfo("setup","L.showFrame",showframe->isChecked());            //是否显示边框
    mgr.setSettingInfo("setup","L.recallBlank",recallblank->isChecked());      //锁屏是否立即黑屏
    mgr.setSettingInfo("setup","L.ECOmode",ECOmode->isChecked());
    mgr.setSettingInfo("setup","L.enableIconBattery",enablebatteryicon->isChecked());//电池图标
    mgr.setSettingInfo("setup","L.enableIconPhone",enablephoneicon->isChecked());    //是否显示信号图标
    mgr.setSettingInfo("setup","L.enableIconRing",enableringicon->isChecked());      //静音图标
    mgr.setSettingInfo("setup","L.enableIconLock",enablelockicon->isChecked());      //解锁图标
    mgr.setSettingInfo("setup","L.enableIconBT",enableBTicon->isChecked());          //蓝牙图标
    mgr.setSettingInfo("setup","L.enablePhoneOnOff",enablephoneonoff->isChecked());  //是否允许开关电话
    mgr.setSettingInfo("setup","L.autoLock",autolock->isChecked());       //自动锁
    mgr.setSettingInfo("setup","S.reCallKey.1",recallkey1->getKey());       //锁屏快捷键1
    mgr.setSettingInfo("setup","S.reCallKey.2",recallkey2->getKey());       //锁屏快捷键2
    mgr.setSettingInfo("setup","S.reCallKey.n",keyrepeats->value());       //按住时间
    mgr.setSettingInfo("setup","S.showTime",showtimes->value());            //熄灯时间
    mgr.setSettingInfo("setup","S.screenBrightLevel",screenbright->value());   //屏幕亮度
    mgr.setSettingInfo("setup","S.autoSec",autolocksec->value()); //自动锁屏时间

    mgr.setSettingInfo("setup","L.timeDisplay",hasTimeBox->isChecked());          //显示时间
    mgr.setSettingInfo("setup","S.timePosition.x",timex);         //时间位置X
    mgr.setSettingInfo("setup","S.timePosition.y",timey);         //时间位置Y
    mgr.setSettingInfo("setup","S.timeFontSize",timefs);          //文本大小
    mgr.setSettingInfo("setup","S.timeColor.r",timeR);            //时间颜色R
    mgr.setSettingInfo("setup","S.timeColor.g",timeG);            //时间颜色G
    mgr.setSettingInfo("setup","S.timeColor.b",timeB);            //时间颜色B
    mgr.setSettingInfo("setup","L.dateDisplay",hasDateBox->isChecked());          //显示日期
    mgr.setSettingInfo("setup","S.datePosition.x",datex);         //日期位置X
    mgr.setSettingInfo("setup","S.datePosition.y",datey);         //日期位置Y
    mgr.setSettingInfo("setup","S.dateFontSize",datefs);          //日期大小
    mgr.setSettingInfo("setup","S.dateColor.r",dateR);            //日期颜色R
    mgr.setSettingInfo("setup","S.dateColor.g",dateG);            //日期颜色G
    mgr.setSettingInfo("setup","S.dateColor.b",dateB);            //日期颜色B
    mgr.setSettingInfo("setup","L.hasClock",hasClockerBox->isChecked());          //显示时钟
    mgr.setSettingInfo("setup","S.clockSizePosition.s",rateClock);//时钟大小
    mgr.setSettingInfo("setup","S.clockSizePosition.x",xClock);   //时钟位置X
    mgr.setSettingInfo("setup","S.clockSizePosition.y",yClock);   //时钟位置Y
    mgr.setSettingInfo("setup","S.clockRuleColor.r",baseCR);      //钟盘颜色R
    mgr.setSettingInfo("setup","S.clockRuleColor.g",baseCG);      //钟盘颜色G
    mgr.setSettingInfo("setup","S.clockRuleColor.b",baseCB);      //钟盘颜色B
    mgr.setSettingInfo("setup","S.hourColor.r",houCR);            //时针颜色R
    mgr.setSettingInfo("setup","S.hourColor.g",houCG);            //时针颜色G
    mgr.setSettingInfo("setup","S.hourColor.b",houCB);            //时针颜色B
    mgr.setSettingInfo("setup","S.minuteColor.r",minCR);          //分针颜色R
    mgr.setSettingInfo("setup","S.minuteColor.g",minCG);          //分针颜色G
    mgr.setSettingInfo("setup","S.minuteColor.b",minCB);          //分针颜色B
    mgr.setSettingInfo("setup","S.secondColor.r",secCR);          //秒针颜色R
    mgr.setSettingInfo("setup","S.secondColor.g",secCG);          //秒针颜色G
    mgr.setSettingInfo("setup","S.secondColor.b",secCB);          //秒针颜色B
    mgr.setSettingInfo("setup","S.exitArea.x0",exitX0);           //退出区域
    mgr.setSettingInfo("setup","S.exitArea.x1",exitX1);           //
    mgr.setSettingInfo("setup","S.exitArea.y0",exitY0);           //
    mgr.setSettingInfo("setup","S.exitArea.y1",exitY1);           //
    mgr.setSettingInfo("setup","S.configArea.x0",configX0);       //设置区域
    mgr.setSettingInfo("setup","S.configArea.x1",configX1);       //
    mgr.setSettingInfo("setup","S.configArea.y0",configY0);       //
    mgr.setSettingInfo("setup","S.configArea.y1",configY1);       //
    mgr.setSettingInfo("setup","S.language",language);            //语言
    mgr.setSettingInfo("setup","L.slideMode",slideModeBox->isChecked());          //背景切换
    mgr.setSettingInfo("setup","S.slideInterval",slideTime);      //切换间隔
    mgr.setSettingInfo("setup","P.BGroundPicDir",picDir,true);    //图片目录
    mgr.setSettingInfo("setup","S.alphaValue",alpha);             //透明度
    p->setcanvasSH(true);
    p->setEng(false);
    p->init();
    close();
}
