#include "dapplication.h"

bool DApplication :: qwsEventFilter(QWSEvent *event)
 {
     ZApplication::qwsEventFilter(event);
     QWSMouseEvent* mouse=0;
     QWSKeyEvent* KeyEvent =0;
     QWSFocusEvent* focus;
     QWSQCopMessageEvent *QCop;
     QWSPropertyNotifyEvent *propertyNotify;
     
     int x,y,z;
     int kcode;
     if (!event)
         return false;
         
     switch(event->type) {
         case QWSEvent::NoEvent:
             dbg_printf("no event\n");
         break;

         case QWSEvent::Connected:
             dbg_printf("connected\n");
         break;
         
         case QWSEvent::Mouse:
             mouse=event->asMouse();
             if (mouse==0)
                 return false;
              x=mouse->simpleData.x_root;
              y=mouse->simpleData.y_root;
              z=mouse->simpleData.state;
              
              if ( pointerListener != 0 )
              {
                 switch( z )
                 {
                     case 0:
                         pointerListener->pointerReleased( x, y );
                     break;
                     case 1:
                         if ( not press )
                             pointerListener->pointerPressed( x, y );
                     break;
                 }
                 
                 press = z;
              }
         break;
          
         case QWSEvent::Focus:
               focus=(QWSFocusEvent*)event;
               if (focus->simpleData.get_focus==0) {
               }
         break;
         
         case QWSEvent::Key:
         
             KeyEvent = (QWSKeyEvent*) event;
             if (KeyEvent==0)
                 return 0;
/*
             dbg_printf("key %d %d %d\n",
                 KeyEvent->simpleData.keycode,
                 KeyEvent->simpleData.is_press,
                 KeyEvent->simpleData.is_auto_repeat);
*/
             kcode = KeyEvent->simpleData.keycode;
             if (KeyEvent->simpleData.is_press && !KeyEvent->simpleData.is_auto_repeat) {
                 pointerListener->keyPressed(KeyEvent->simpleData.keycode);
             }
         break;
         
         case QWSEvent::RegionModified:
             //dbg_printf("RegionModified\n");
         break;
         
         case QWSEvent::Creation:
             dbg_printf("Creation\n");
         break;
         
         case QWSEvent::PropertyNotify:
             if (propertyNotify==0)
                 return 0;
             pointerListener->PropertyReceived();

             dbg_printf("propertyNotify %d %d %d\n",
                 propertyNotify->simpleData.window,
                 propertyNotify->simpleData.property,
                 propertyNotify->simpleData.state);

         break;
         
         case QWSEvent::PropertyReply:
             dbg_printf("PropertyReply\n");
         break;
         
         case QWSEvent::SelectionClear:
             dbg_printf("SelectionClear\n");
         break;
         
         case QWSEvent::SelectionRequest:
             dbg_printf("SelectionRequest\n");
         break;
         
         case QWSEvent::SelectionNotify:
             dbg_printf("SelectionNotify\n");
         break;
         
         case QWSEvent::MaxWindowRect:
             dbg_printf("MaxWindowRect\n");
         break;
         
         case QWSEvent::QCopMessage:
             QCop = (QWSQCopMessageEvent *)event;
             dbg_printf("QCopMessage %d %d %d\n",
                 QCop->simpleData.lchannel,
                 QCop->simpleData.ldata,
                 QCop->simpleData.lmessage);
             pointerListener->QCopReceived(QCop->simpleData.lmessage);
             if(QCop->simpleData.lmessage == 6) {
                 return true;
             }
         break;
         
         case QWSEvent::WindowOperation:
             dbg_printf("WindowOperation\n");
         break;
         
         case QWSEvent::IMEvent:
             dbg_printf("IMEvent\n");
         break;
         
         case QWSEvent::NEvent:
             dbg_printf("NEvent\n");
         break;
    }
    return false;
}

