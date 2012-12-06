#include "tankcanvas.h"
#include <cstring>
#include "notificationevents.h"
#include "fishtankapp.h"

static inline int min(int a, int b){
    return (a<b) ? a: b;
}

static inline int max(int a, int b){
    return (a>b) ? a :b;
}

TankCanvas::TankCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos,
                    const wxSize& size, int mapSizeN, int mapSizeM, const wxString& name):
    wxWindow(parent, id, pos, size, wxFULL_REPAINT_ON_RESIZE | wxBORDER_DOUBLE, name),
    iMapSizeN(mapSizeN), iMapSizeM(mapSizeM), moveTimer(this), mainFrame((MainFrame*)(GetParent()))
{
    iLeftBorder = iTopBorder = 5;
    iBottomReserved = 20;
    iWindowSizeX = size.GetWidth();
    iWindowSizeY = size.GetHeight();
    UpdateSize();

    Reset();

    SetBackgroundColour(*wxWHITE);
    Refresh();
}

void TankCanvas::Reset(){
    wxClientDC dc(this);
    for (int i = 1; i<=iMapSizeN; ++i)
    for (int j = 1; j<=iMapSizeM; ++j){
        ipMapContent[i][j] = EMPTY;
        DrawEmpty(dc, i, j);
    }

    for (int i = 0; i<MAX_PLAYER; ++i)
        ipValue[i] = 0;

    iInAction = 0;
}

void TankCanvas::OnPaint(wxPaintEvent& event){
    static wxFont InActionTextFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    wxPaintDC dc(this);
    wxRect rect = GetUpdateRegion().GetBox();

    //Draw lines
    dc.SetPen(*wxGREY_PEN);
    dc.SetBrush(*wxWHITE_BRUSH);
    for (int i = max(1, (rect.GetLeft()-iLeftBorder) / iGridSizeX),
             tar = min(iMapSizeM + 1, (rect.GetRight()-iLeftBorder) / iGridSizeX),
             top = max(iTopBorder + iGridSizeY, rect.GetTop()),
             bottom = min(iTopBorder + iGridSizeY * (iMapSizeN+1) , rect.GetBottom());
             i<=tar; ++i){
         dc.DrawLine(i * iGridSizeX + iLeftBorder, bottom, i * iGridSizeX + iLeftBorder, top);
    }
    for (int i = max(1, (rect.GetTop() - iLeftBorder) / iGridSizeY),
             tar = min(iMapSizeN + 1, (rect.GetBottom()-iTopBorder) / iGridSizeY),
             left = max(iLeftBorder + iGridSizeX, rect.GetLeft()),
             right = min(iTopBorder + iGridSizeX * (iMapSizeM+1), rect.GetRight());
             i<=tar; ++i){
        dc.DrawLine(left, i * iGridSizeY + iTopBorder, right, i * iGridSizeY + iTopBorder);
    }

    //Draw numbers
    dc.SetFont(*wxNORMAL_FONT);
    dc.SetTextForeground(*wxBLACK);
    dc.SetBackgroundMode(wxTRANSPARENT);
    wxString strToPrint;
    for (int i = 1; i<=iMapSizeM / 5; ++i){
        strToPrint.Printf(wxT("%d"), i*5);
        dc.DrawText(strToPrint, iLeftBorder + (i * 5) * iGridSizeX, iTopBorder);
    }
    for (int i = 1; i<=iMapSizeN / 5; ++i){
        strToPrint.Printf(wxT("%d"), i*5);
        dc.DrawText(strToPrint, iLeftBorder, iTopBorder + (i * 5) * iGridSizeY);
    }

    //Draw contents
    for (int i = max(1, (rect.GetLeft()-iLeftBorder) / iGridSizeX),
             tarx = min(iMapSizeM, (rect.GetRight()-iLeftBorder) / iGridSizeX),
             fromy = max(1, (rect.GetTop() - iLeftBorder) / iGridSizeY),
             tary = min(iMapSizeN, (rect.GetBottom()-iTopBorder) / iGridSizeY);
             i<=tarx; ++i)
    for (int j = fromy; j<=tary; ++j)
        DrawGrid(dc, i, j);

    //Print In action text
    dc.SetFont(InActionTextFont);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawRectangle(0, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1,
                     iWindowSizeX, iBottomReserved);
    if (iInAction!=0)
        strToPrint.Printf(wxT("Fish In Action: %d Author: %s"), iInAction, mainFrame->GetAuthor(iInAction).c_str());
    else
        strToPrint.Printf(wxT("Fish In Action: None"));
    dc.DrawText(strToPrint, iLeftBorder + iGridSizeX, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1);
}

void TankCanvas::OnSize(wxSizeEvent& event){
    UpdateSize();
}

void TankCanvas::OnMotion(wxMouseEvent& event){
    int x = (event.GetX() - iLeftBorder) / iGridSizeX;
    int y = (event.GetY() - iTopBorder) / iGridSizeY;
    if (x>=1 && x<=iMapSizeM && y>=1 && y<=iMapSizeN){
        wxString strToPrint;
        switch (ipMapContent[x][y]){
        case EMPTY:
            strToPrint.Printf(wxT("(%d, %d) NULL"), x, y);
            break;
        case FOOD:
            strToPrint.Printf(wxT("(%d, %d) FOOD"), x, y);
            break;
        default:
            strToPrint.Printf(wxT("(%d, %d) Fish %d Author: %s"), x, y, ipMapContent[x][y],
                              mainFrame->GetAuthor(ipMapContent[x][y]).c_str());
        }
        mainFrame->SetLeftStatusBar(strToPrint);
    }
}

void TankCanvas::DrawEmpty(wxDC& dc, int iPosX, int iPosY){
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxGREY_PEN);
    dc.DrawRectangle(iLeftBorder + (iPosX) * iGridSizeX,
                     iTopBorder + (iPosY) * iGridSizeY,
                     iGridSizeX + 1 ,
                     iGridSizeY + 1);
}

void TankCanvas::DrawFood(wxDC& dc, int iPosX, int iPosY){
    static wxBrush FoodBrush(wxT("GREEN"), wxSOLID);
    static wxPen FoodPen(wxT("GREEN"));
    dc.SetBrush(FoodBrush);
    dc.SetPen(FoodPen);
    dc.DrawRectangle(iLeftBorder + (iPosX) * iGridSizeX + (iGridSizeX - iFoodSizeX) / 2 + 1,
                     iTopBorder + (iPosY) * iGridSizeY + (iGridSizeY - iFoodSizeY) / 2 + 1,
                     iFoodSizeX,
                     iFoodSizeY);
}

void TankCanvas::DrawFish(wxDC& dc, int iPosX, int iPosY){
  //  wxLogMessage(wxString::Format(wxT("DrawFish %d, %d id = %d value = %d"), iPosX, iPosY, ipMapContent[iPosX][iPosY], ipValue[ipMapContent[iPosX][iPosY]]));
    static wxBrush FishBrush(wxT("BLUE"), wxSOLID);
    static wxPen FishPen(wxT("BLUE"), 1, wxSOLID);
    static wxBrush FishActiveBrush(wxT("RED"), wxSOLID);
    static wxPen FishActivePen(wxT("RED"), 1, wxSOLID);
    static wxPen FishHighValuePen(wxT("SEA GREEN"), 1, wxSOLID);
    static wxBrush FishHighValueBrush(wxT("SEA GREEN"), wxSOLID);
    static wxPen FishMidValuePen(wxColour(255, 120, 0), 1, wxSOLID);
    static wxBrush FishMidValueBrush(wxColour(255, 120, 0), wxSOLID);
    static wxPen FishLowValuePen(wxT("RED"), 1, wxSOLID);
    static wxBrush FishLowValueBrush(wxT("RED"), wxSOLID);

    if (ipValue[ipMapContent[iPosX][iPosY]] <=20){
        dc.SetBrush(FishLowValueBrush);
        dc.SetPen(FishLowValuePen);
    } else
    if (ipValue[ipMapContent[iPosX][iPosY]] <80){
        dc.SetBrush(FishMidValueBrush);
        dc.SetPen(FishMidValuePen);
    } else
    {
        dc.SetBrush(FishHighValueBrush);
        dc.SetPen(FishHighValuePen);
    }

    dc.DrawRectangle(iLeftBorder + (iPosX) * iGridSizeX + (iGridSizeX - iFishValueMaxLength) / 2 + 1,
                     iTopBorder + (iPosY) * iGridSizeY + 2,
                     iFishValueMaxLength * ipValue[ipMapContent[iPosX][iPosY]] / 100,
                     iFishValueMaxHeight);
    if (ipMapContent[iPosX][iPosY] == iInAction){
        dc.SetBrush(FishActiveBrush);
        dc.SetPen(FishActivePen);
    } else {
        dc.SetBrush(FishBrush);
        dc.SetPen(FishPen);
    }
    dc.DrawEllipse(iLeftBorder + (iPosX) * iGridSizeX + (iGridSizeX - iFishSizeX) / 2 + 1,
                   iTopBorder + (iPosY) * iGridSizeY + iFishValueMaxHeight + 2 + (iGridSizeY -iFishValueMaxHeight -1 - iFishSizeY) / 2,
                   iFishSizeX,
                   iFishSizeY);
}

void TankCanvas::DrawGrid(wxDC& dc, int iPosX, int iPosY){
    switch (ipMapContent[iPosX][iPosY]){
        case FOOD:
            DrawFood(dc, iPosX, iPosY);
            break;
        case EMPTY:
            DrawEmpty(dc, iPosX, iPosY);
            break;
        default:
            DrawFish(dc, iPosX, iPosY);
    }
}

void TankCanvas::UpdateSize(){
    iGridSizeX = (GetClientSize().GetWidth()-iLeftBorder) / (iMapSizeM+1);
    iGridSizeY = (GetClientSize().GetHeight()-iTopBorder-iBottomReserved) / (iMapSizeN+1);
    iBottomReserved = iWindowSizeY - iTopBorder - iGridSizeY * (iMapSizeN + 1);
    iFoodSizeX = iGridSizeX * 7 / 10;
    iFoodSizeY = iGridSizeY * 7 / 10;
    iFishValueMaxHeight = 2;
    iFishValueMaxLength = iGridSizeX * 4 / 5;
    iFishSizeX = iGridSizeX * 4 / 5;
    iFishSizeY = iGridSizeY * 3 / 5;
}

TankCanvas::MoveTimer::MoveTimer(TankCanvas* tf):tankCanvas(tf){}

void TankCanvas::MoveTimer::SetCoords(int x, int y, int x2, int y2){
    iPosX = x; iPosY = y; iPosX2 = x2; iPosY2 = y2;
    iPrevContent = EMPTY;
    iID = tankCanvas->ipMapContent[iPosX][iPosY];
}

void TankCanvas::MoveTimer::Notify(){
    wxClientDC dc(tankCanvas);
    if (iPosX < iPosX2){
        tankCanvas->ipMapContent[iPosX][iPosY] = iPrevContent;
        if (iPrevContent !=EMPTY) tankCanvas->DrawEmpty(dc, iPosX, iPosY);
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
        iPrevContent = tankCanvas->ipMapContent[++iPosX][iPosY];
        tankCanvas->ipMapContent[iPosX][iPosY] = iID;
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
    } else
    if (iPosX > iPosX2){
        tankCanvas->ipMapContent[iPosX][iPosY] = iPrevContent;
        if (iPrevContent !=EMPTY) tankCanvas->DrawEmpty(dc, iPosX, iPosY);
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
        iPrevContent = tankCanvas->ipMapContent[--iPosX][iPosY];
        tankCanvas->ipMapContent[iPosX][iPosY] = iID;
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
    } else
    if (iPosY < iPosY2){
        tankCanvas->ipMapContent[iPosX][iPosY] = iPrevContent;
        if (iPrevContent !=EMPTY) tankCanvas->DrawEmpty(dc, iPosX, iPosY);
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
        iPrevContent = tankCanvas->ipMapContent[iPosX][++iPosY];
        tankCanvas->ipMapContent[iPosX][iPosY] = iID;
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
    } else
    if (iPosY > iPosY2){
        tankCanvas->ipMapContent[iPosX][iPosY] = iPrevContent;
        if (iPrevContent !=EMPTY) tankCanvas->DrawEmpty(dc, iPosX, iPosY);
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
        iPrevContent = tankCanvas->ipMapContent[iPosX][--iPosY];
        tankCanvas->ipMapContent[iPosX][iPosY] = iID;
        tankCanvas->DrawGrid(dc, iPosX, iPosY);
    } else {
        ftEventWithID event(iID, iPosX, iPosY, 0, 0, ftEVT_FISH_MOVE_ANIMATION_FINISHED);
        tankCanvas->mainFrame->AddPendingEvent(event);
        Stop();
    }
}

void TankCanvas::SetGrid(int iPosX, int iPosY, int iContent, int iValue){
    wxClientDC dc(this);
    DrawEmpty(dc, iPosX, iPosY);
    ipMapContent[iPosX][iPosY] = iContent;
    if (iContent!=FOOD && iContent!=EMPTY) ipValue[iContent] = iValue;
    DrawGrid(dc, iPosX, iPosY);
}

void TankCanvas::EraseGrid(int iPosX, int iPosY){
    wxClientDC dc(this);
    ipMapContent[iPosX][iPosY] = EMPTY;
    DrawEmpty(dc, iPosX, iPosY);
}

void TankCanvas::EraseGrid(int iContent){
    wxClientDC dc(this);
    for (int i = 1; i<=iMapSizeN; ++i)
    for (int j = 1; j<=iMapSizeM; ++j)
    if (ipMapContent[i][j] == iContent){
        ipMapContent[i][j] = EMPTY;
        DrawEmpty(dc, i, j);
    }
}

void TankCanvas::MoveFish(int iPosX, int iPosY, int iPosX2, int iPosY2){
    moveTimer.SetCoords(iPosX, iPosY, iPosX2, iPosY2);
    moveTimer.Start(20);
}

void TankCanvas::ActivateFish(int iPosX, int iPosY){
    static wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    iInAction = ipMapContent[iPosX][iPosY];
    wxClientDC dc(this);
    DrawFish(dc, iPosX, iPosY);
    dc.SetFont(font);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawRectangle(0, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1,
                     iWindowSizeX, iBottomReserved);
    dc.SetPen(*wxBLACK_PEN);
    wxString strToPrint;
    strToPrint.Printf(wxT("Fish In Action: %d Author: %s"), ipMapContent[iPosX][iPosY],
                      mainFrame->GetAuthor(ipMapContent[iPosX][iPosY]).c_str());
    dc.DrawText(strToPrint, iLeftBorder + iGridSizeX, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1);
}

void TankCanvas::DeactivateFish(int iPosX, int iPosY){
    static wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    iInAction = 0;
    wxClientDC dc(this);
    DrawFish(dc, iPosX, iPosY);
    dc.SetFont(font);
    dc.SetBrush(*wxWHITE_BRUSH);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawRectangle(0, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1,
                     iWindowSizeX, iBottomReserved);
    dc.SetPen(*wxBLACK_PEN);
    wxString strToPrint;
    strToPrint.Printf(wxT("Fish In Action: None"));
    dc.DrawText(strToPrint, iLeftBorder + iGridSizeX, iTopBorder + iGridSizeY * (iMapSizeN + 1) + 1);
}

void TankCanvas::TestMove(wxKeyEvent& event){
    wxLogMessage(wxT("wxKeyEvent in Tankcanvas"));
}

BEGIN_EVENT_TABLE(TankCanvas, wxWindow)
    EVT_PAINT(TankCanvas::OnPaint)
    EVT_SIZE(TankCanvas::OnSize)
    EVT_MOTION(TankCanvas::OnMotion)
END_EVENT_TABLE()
