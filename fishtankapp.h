#ifndef _FISHTANKAPP_H_
#define _FISHTANKAPP_H_

#include "wx_commonheader.h"
#include "mainframe.h"
#include "tankcanvas.h"

class FishTankApp:public wxApp{
public:
    virtual bool OnInit();

    MainFrame* GetMainFrame();
private:
    MainFrame* mainFrame;
    bool Closing;
};

DECLARE_APP(FishTankApp)

#endif // _FISHTANKAPP_H_
