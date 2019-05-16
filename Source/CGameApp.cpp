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
	_screenSize = Vec2(1920, 1080);

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

	loseScreen = new Sprite("data/losescreen.bmp", RGB(0xff, 0x00, 0xff));
	loseScreen->mPosition = Vec2(_screenSize.x / 2, _screenSize.y / 2);
	loseScreen->setBackBuffer(_Buffer);
	
	winScreen = new Sprite("data/winscreen.bmp", RGB(0xff, 0x00, 0xff));
	winScreen->mPosition = Vec2(_screenSize.x / 2, _screenSize.y / 2);
	winScreen->setBackBuffer(_Buffer);

	frameCounter = 0;

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	return true;
}

void CGameApp::SetupGameState()
{
	statements.prefixes.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(300, 400)));
	statements.prefixes.back()->name = "unit";
	statements.prefixes.back()->currentState = Unit::PUSH;
	
	statements.connectors.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(400, 400)));
	statements.connectors.back()->currentState = Unit::PUSH;
	
	statements.sufixes.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(500, 400)));
	statements.sufixes.back()->specialState = Unit::PUSH;
	statements.sufixes.back()->currentState = Unit::PUSH;

	statements.prefixes.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(300, 100)));
	statements.prefixes.back()->name = "player";
	statements.prefixes.back()->currentState = Unit::PUSH;

	statements.connectors.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(400, 100)));
	statements.connectors.back()->currentState = Unit::PUSH;

	statements.sufixes.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(500, 100)));
	statements.sufixes.back()->specialState = Unit::MOVE;
	statements.sufixes.back()->currentState = Unit::PUSH;

	units.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(500, 500)));
	units.back()->name = "player";
	units.back()->currentState = statements.GetState(units.back()->name);

	units.push_back(new Unit(_Buffer, "data/unit.bmp", Vec2(500, 600)));
	units.back()->name = "unit";
	units.back()->currentState = statements.GetState(units.back()->name);

	_gameState = GameState::ONGOING;

	ulCorner = Vec2(110.0, 40.0);
	drCorner = Vec2(1710.0, 940.0);
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
	KillUnits();
	CheckWin();
	PushUnits();
	UpdateStates();
}

void CGameApp::ProcessInput()
{
	static UCHAR	pKeyBuffer[256];
	ULONG			Direction = 0;
	POINT			CursorPos;
	float			X = 0.0f, Y = 0.0f;

	if (!GetKeyboardState(pKeyBuffer)) return;

	if (pKeyBuffer[0x57] & 0xF0) {
		Direction = Unit::DIR_FORWARD;
	}
	else if (pKeyBuffer[0x53] & 0xF0) {
		Direction = Unit::DIR_BACKWARD;
	}
	else if (pKeyBuffer[0x41] & 0xF0) {
		Direction = Unit::DIR_LEFT;
	}
	else if (pKeyBuffer[0x44] & 0xF0) {
		Direction = Unit::DIR_RIGHT;
	}
	
	if (Direction != 0)
	{
		for (auto& unit : units) {
			if (!CanMove(*unit, Direction)) {
				continue;
			}

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
	}

	statements.Update();
}

void CGameApp::DrawObjects()
{

	_Buffer->reset();

	m_imgBackground.Paint(_Buffer->getDC(), 0, 0);

	switch (_gameState) {
	case CGameApp::ONGOING:
		for (auto unit : units) {
			unit->Draw();
		}
		statements.Draw();
		break;

	case CGameApp::LOST:
		loseScreen->draw();
		break;

	case CGameApp::WON:
		winScreen->draw();
		break;

	default:
		break;
	}

	_Buffer->present();
}

bool CGameApp::holdInside(Unit& unit, ULONG dir) 
{
	switch (dir) {
	case Unit::DIR_RIGHT:
		if (unit.Position().x + 100 > drCorner.x) {
			return false;
		}
		break;

	case Unit::DIR_LEFT:
		if (unit.Position().x - 100 < ulCorner.x) {
			return false;
		}
		break;

	case Unit::DIR_BACKWARD:
		if (unit.Position().y + 100 > drCorner.y) {
			return false;
		}
		break;

	case Unit::DIR_FORWARD:
		if (unit.Position().y - 100 < ulCorner.y) {
			return false;
		}
		break;

	default:
		break;
	}

	return true;
}

bool CGameApp::CanMove(Unit& unit, ULONG dir)
{
	if (unit.currentState != Unit::MOVE) {
		return false;
	}

	if (!holdInside(unit, dir)) {
		return false;
	}

	for (auto& otherUnit : units) {
		if (otherUnit->Position() == unit.Position()) {
			continue;
		}

		if (!holdInside(*otherUnit, dir) && unit.WillColide(*otherUnit, (Unit::DIRECTION)dir)) {
			return false;
		}

		if (otherUnit->currentState != Unit::STOP) {
			continue;
		}

		if (unit.WillColide(*otherUnit, (Unit::DIRECTION)dir))
		{
			return false;
		}
	}

	return true;
}

void CGameApp::KillUnits()
{
	for (auto& unit : units) {
		if (unit->currentState == Unit::DEATH) {
			continue;
		}

		for (auto& otherUnit : units) {
			if (unit->Position() == otherUnit->Position()) {
				continue;
			}

			if (otherUnit->currentState != Unit::DEATH) {
				continue;
			}

			if (unit->IsColided(*otherUnit)) {
				unit->currentState = Unit::ISDEAD;
			}
		}
	}

	for (auto& unit : units) {
		if (unit->currentState == Unit::ISDEAD) {
			delete unit;
			units.remove(unit);
			break;
		}
	}

	int noAliveUnits = 0;

	for (auto& unit : units) {
		if (unit->currentState == Unit::MOVE) {
			++noAliveUnits;
		}
	}

	if (!noAliveUnits) {
		_gameState = CGameApp::LOST;
	}
}

void CGameApp::CheckWin()
{
	for (auto& unit : units) {
		if (unit->currentState != Unit::MOVE) {
			continue;
		}

		for (auto& otherUnit : units) {
			if (otherUnit->currentState != Unit::WIN) {
				continue;
			}

			if (unit->IsColided(*otherUnit)) {
				_gameState = CGameApp::WON;
			}
		}
	}
}

void CGameApp::PushUnits()
{
	for (auto& unit : units) {
		for (auto& otherUnit : units) {
			if (CanPush(*unit, *otherUnit)) {
				otherUnit->Move(unit->directionState);
			}
		}

		for (auto& prefix : statements.prefixes) {
			if (CanPush(*unit, *prefix)) {
				prefix->Move(unit->directionState);
			}
		}

		for (auto& con : statements.connectors) {
			if (CanPush(*unit, *con)) {
				con->Move(unit->directionState);
			}
		}

		for (auto& sufix : statements.sufixes) {
			if (CanPush(*unit, *sufix)) {
				sufix->Move(unit->directionState);
			}
		}
	}
}

bool CGameApp::CanPush(Unit& unitA, Unit& unitB)
{
	if (&unitA == &unitB) {
		return false;
	}
	if (unitB.currentState != Unit::PUSH) {
		return false;
	}

	if (unitA.WillColide(unitB, unitA.directionState)) {
		if (holdInside(unitB, unitA.directionState)) {
			return true;
		}
	}

	return false;
}

void CGameApp::UpdateStates()
{
	for (auto& unit : units) {
		unit->currentState = statements.GetState(unit->name);
	}
}
