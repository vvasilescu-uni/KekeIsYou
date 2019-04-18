//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "../Includes/CGameApp.h"

#include <ctime>
#include <cstdlib>
#include <fstream>

extern	HINSTANCE g_hInst;

CGameApp::CGameApp()
{
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	_Buffer			= NULL;
	m_LastFrameRate = 0;
}

CGameApp::~CGameApp()
{
	ShutDown();
}

bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	if (!CreateDisplay()) { ShutDown(); return false; }

	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	SetupGameState();

	return true;
}

bool CGameApp::CreateDisplay()
{
	HMONITOR hM = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mInfo = { sizeof(mInfo) };

	if (!GetMonitorInfo(hM, &mInfo)) return NULL;

	_screenSize = Vec2(mInfo.rcMonitor.right, mInfo.rcMonitor.bottom);

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= (USHORT)_screenSize.x;
	USHORT			Height			= (USHORT)_screenSize.y;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	ShowWindow(m_hWnd, SW_SHOW);

	return true;
}

int CGameApp::BeginGame()
{
	MSG		msg;

	while(true) 
	{
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			FrameAdvance();
		}
	} 

	return 0;
}

bool CGameApp::ShutDown()
{
	ReleaseObjects ( );
	
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	return true;
}


LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT	fTimer;	

	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				m_bActive = false;
			}
			else
			{
				m_bActive = true;

				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
			}

			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				if (_gameState == GameState::ONGOING) _gameState = GameState::PAUSE;
				else PostQuitMessage(0);
				break;
			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	}
	
	return 0;
}

bool CGameApp::BuildObjects()
{
	_Buffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);

	frameCounter = 0;

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	return true;
}

void CGameApp::SetupGameState()
{
	units.push_back(new Unit(_Buffer, "data/ship1.bmp", Vec2(500, 500)));
	units.back()->currentState = Unit::STATE::MOVE;

	units.push_back(new Unit(_Buffer, "data/ship1.bmp", Vec2(500, 600)));
	units.back()->currentState = Unit::STATE::MOVE;

	_gameState = GameState::START;
}

void CGameApp::ReleaseObjects()
{
	if(_Buffer != NULL)
	{
		delete _Buffer;
		_Buffer = NULL;
	}
}

void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[50];
	static TCHAR TitleBuffer[255];

	m_Timer.Tick( );

	if ( !m_bActive ) return;
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );
	}

	ProcessInput();
	AnimateObjects();
	DrawObjects();
}

void CGameApp::ProcessInput()
{
	static UCHAR	pKeyBuffer[256];
	ULONG			Direction = 0;
	POINT			CursorPos;
	float			X = 0.0f, Y = 0.0f;

	if (!GetKeyboardState(pKeyBuffer)) return;

	if (pKeyBuffer[0x57] & 0xF0) {
		Direction |= Unit::DIR_FORWARD;
	}
	else if (pKeyBuffer[0x53] & 0xF0) {
		Direction |= Unit::DIR_BACKWARD;
	}
	else if (pKeyBuffer[0x41] & 0xF0) {
		Direction |= Unit::DIR_LEFT;
	}
	else if (pKeyBuffer[0x44] & 0xF0) {
		Direction |= Unit::DIR_RIGHT;
	}

	for (auto& unit : units) {
		if (unit->currentState == Unit::MOVE && CanMove(*unit, Direction)) {
			unit->Move(Direction);
		}
	}

	if ( GetCapture() == m_hWnd )
	{
		SetCursor( NULL );
		GetCursorPos( &CursorPos );
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );
	}
}

void CGameApp::AnimateObjects()
{
	(frameCounter > 200) ? frameCounter = 0 : ++frameCounter;

	for (auto & unit : units) {
		unit->Update();
		holdInside(*unit);
		CheckCollision(*unit);
	}
}

void CGameApp::DrawObjects()
{

	_Buffer->reset();

	m_imgBackground.Paint(_Buffer->getDC(), 0, 0);

	for (auto unit : units) {
		unit->Draw();
	}

	_Buffer->present();
}

void CGameApp::holdInside(Unit& unit) 
{
	if (unit.Position().x + (unit.getSize().x / 2) >= _screenSize.x) {
		unit.Position().x = _screenSize.x - (unit.getSize().x / 2);
		unit.newPosition.x = _screenSize.x - (unit.getSize().x / 2);
	}

	if (unit.Position().x - (unit.getSize().x / 2) <= 0) {
		unit.Position().x = unit.getSize().x / 2;
		unit.newPosition.x = unit.getSize().x / 2;
	}

	if (unit.Position().y + (unit.getSize().y / 2) >= _screenSize.y) {
		unit.Position().y = _screenSize.y - (unit.getSize().y / 2);
		unit.newPosition.y = _screenSize.y - (unit.getSize().y / 2);
	}

	if (unit.Position().y - (unit.getSize().y / 2) <= 0) {
		unit.Position().y = unit.getSize().y / 2;
		unit.newPosition.y = unit.getSize().y / 2;
	}
}

bool CGameApp::HasColided(Unit& unit)
{
	return false;
}

void CGameApp::CheckCollision(Unit& unit)
{
	if (!HasColided(unit)) {
		return;
	}
}

bool CGameApp::CanMove(Unit& unit, ULONG dir)
{
	for (auto& otherUnit : units) {
		if (otherUnit == &unit) {
			continue;
		}

		switch (dir) {
		case Unit::DIR_FORWARD:
			if (unit.Position().y == otherUnit->Position().y + 100) {
				return false;
			}
			break;
		case Unit::DIR_BACKWARD:
			if (unit.Position().y == otherUnit->Position().y - 100) {
				return false;
			}
			break;
		case Unit::DIR_LEFT:
			if (unit.Position().x == otherUnit->Position().x + 100) {
				return false;
			}
			break;
		case Unit::DIR_RIGHT:
			if (unit.Position().x == otherUnit->Position().x - 100) {
				return false;
			}
			break;
		}
	}

	return true;
}
