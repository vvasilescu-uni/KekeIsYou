//-----------------------------------------------------------------------------
// File: CGameApp.h
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CGAMEAPP_H_
#define _CGAMEAPP_H_

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "CTimer.h"
#include "Unit.h"
#include "BackBuffer.h"
#include "ImageFile.h"
#include "Bullet.h"
#include "MenuSprite.h"

#include <list>
#include <string>

//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp (Class)
// Desc : Central game engine, initialises the game and handles core processes.
//-----------------------------------------------------------------------------
class CGameApp
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
				CGameApp();
	virtual		~CGameApp();

	//-------------------------------------------------------------------------
	// Public Functions for This Class
	//-------------------------------------------------------------------------
	LRESULT	 DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam );
	bool		InitInstance( LPCTSTR lpCmdLine, int iCmdShow );
	int			BeginGame();
	bool		ShutDown();

	enum GameState {
		START,
		ONGOING,
		LOST,
		WON,
		PAUSE
	};
	
private:
	//-------------------------------------------------------------------------
	// Private Functions for This Class
	//-------------------------------------------------------------------------
	bool		BuildObjects();
	void		ReleaseObjects();
	void		FrameAdvance();
	bool		CreateDisplay();
	void		SetupGameState();
	void		AnimateObjects();
	void		DrawObjects();
	void		ProcessInput();
	bool		holdInside(Unit& unit, ULONG dir);
	bool		CanMove(Unit& unit, ULONG dir);
	
	//-------------------------------------------------------------------------
	// Private Static Functions For This Class
	//-------------------------------------------------------------------------
	static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

	//-------------------------------------------------------------------------
	// Private Variables For This Class
	//-------------------------------------------------------------------------
	CTimer						m_Timer;
	ULONG						m_LastFrameRate;
	
	HWND						m_hWnd;
	HICON						m_hIcon;
	HMENU						m_hMenu;
	
	bool						m_bActive;

	ULONG						m_nViewX;
	ULONG						m_nViewY;
	ULONG						m_nViewWidth;
	ULONG						m_nViewHeight;
	POINT						m_OldCursorPos;
	HINSTANCE					m_hInstance;
	CImageFile					m_imgBackground;
	
	BackBuffer*					_Buffer;
	Vec2						_screenSize;
	GameState					_gameState;
	
	int							frameCounter;

	std::list<Unit*>			units;

	Vec2						ulCorner;
	Vec2						drCorner;
};

#endif // _CGAMEAPP_H_