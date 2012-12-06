#ifndef _TANKCANVAS_H_
#define _TANKCANVAS_H_

#include "wx_commonheader.h"
#include "constants.h"
#include "mainframe.h"

class TankCanvas:public wxWindow{
public:
    TankCanvas(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, int mapSizeN = N, int mapSizeM = M,const wxString& name = wxT("TankCanvas"));

    void OnPaint(wxPaintEvent&);
    void OnSize(wxSizeEvent&);
    void OnMotion(wxMouseEvent& event);

    void SetGrid(int iPosX, int iPosY, int iContent, int iValue = 0);
    void EraseGrid(int iPosX, int iPosY);
    void EraseGrid(int iContent);
    void MoveFish(int iPosX, int iPosY, int iPosX2, int iPosY2);
    void ActivateFish(int iPosX, int iPosY);
    void DeactivateFish(int iPosX, int iPosY);
    void Reset();

    void TestMove(wxKeyEvent& event);

private:
    int iMapSizeN, iMapSizeM;
    int iLeftBorder, iTopBorder;
    int iBottomReserved;
    int iWindowSizeX, iWindowSizeY;
    int iGridSizeX, iGridSizeY;
    int iFoodSizeX, iFoodSizeY;
    int iFishSizeX, iFishSizeY, iFishValueMaxLength, iFishValueMaxHeight;

    int ipMapContent[N+1][M+1];
    int ipValue[MAX_PLAYER+1];
    int iInAction;

    void DrawFish(wxDC& dc, int iPosX, int iPosY);
    void DrawFood(wxDC& dc, int iPosX, int iPosY);
    void DrawEmpty(wxDC& dc, int iPosX, int iPosY);
    void DrawGrid(wxDC& dc, int iPosX, int iPosY);

    void UpdateSize();

    class MoveTimer:public wxTimer{
    public:
        MoveTimer(TankCanvas* tf);
        void SetCoords(int x, int y, int x2, int y2);
        void Notify();
    private:
        int iPosX, iPosY, iPosX2, iPosY2;
        int iPrevContent, iID;
        TankCanvas* tankCanvas;
    } moveTimer;
    friend class MoveTimer;

    MainFrame* mainFrame;
    DECLARE_EVENT_TABLE()
};

#endif //_TANKCANVAS_H_
