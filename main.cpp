#include "fishtankapp.h"
#include "host.h"

bool FishTankApp::OnInit(){
    mainFrame = new MainFrame();
    mainFrame->Show();
    RetrieveHost().FirstSetup();
    return true;
}

MainFrame* FishTankApp::GetMainFrame(){
    return mainFrame;
}

IMPLEMENT_APP(FishTankApp)
