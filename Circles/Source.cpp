#include <windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")
#include <dwrite.h>
#pragma comment(lib, "dwrite")
#include "basewin.h"
#include <ctime>
#include <cstdlib>

struct Circle {
	float x;
	float y;
	float speed;
	float radius;
	D2D1_COLOR_F color;
};

struct Player {
	float x;
	float y;
	float vx;
	float vy;
	float radius;
};


class MainWindow : public BaseWindow<MainWindow> {
private:
	ID2D1Factory* pFactory = 0;
	ID2D1HwndRenderTarget* pRenderTarget = 0;
	ID2D1SolidColorBrush* pBrush = 0;
	IDWriteFactory* pWriteFactory;
	IDWriteTextFormat* pTextFormat;

	int windowWidth = 800;
	int windowHeight = 600;
	Player player;
	int elements = 0;
	Circle circles[41];
	bool LeftKeyDown = false;
	bool RightKeyDown = false;
	bool UpKeyDown = false;
	bool DownKeyDown = false;
	bool gameOver = false;

public:

	PCWSTR  ClassName() const { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {

	case WM_SETCURSOR: {
		if (LOWORD(lParam) == HTCLIENT) {
			HCURSOR hCursor = LoadCursor(NULL, IDC_ARROW);
			SetCursor(hCursor);
			return TRUE;
		}
		break;
	}

	case WM_KEYDOWN: {
		if (wParam == VK_LEFT) {
			LeftKeyDown = true;
		} else if (wParam == VK_RIGHT) {
			RightKeyDown = true;
		} else if (wParam == VK_UP) {
			UpKeyDown = true;
		} else if (wParam == VK_DOWN) {
			DownKeyDown = true;
		} else if (wParam == VK_SPACE && gameOver == true) {
			gameOver = false;
			elements = 0;
			while (elements < 40) {
				elements++;
				circles[elements].radius = (rand() % 50) + 2;
				circles[elements].x = -(rand() % 1000) - 51;
				circles[elements].y = rand() % (windowHeight-52);
				circles[elements].speed = ((float)(rand() % 200)) / 100 + 1;
				circles[elements].color = D2D1::ColorF(((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255);
			}
			player.x = windowWidth / 2 - 8;
			player.y = windowHeight / 2 - 20;
			player.radius = 10;
			player.vx = 0;
			player.vy = 0;
			LeftKeyDown, RightKeyDown, UpKeyDown, DownKeyDown = false;
		}
		return 0;
	}

	case WM_KEYUP: {
		if (wParam == VK_LEFT) {
			LeftKeyDown = false;
		} else if (wParam == VK_RIGHT) {
			RightKeyDown = false;
		} else if (wParam == VK_UP) {
			UpKeyDown = false;
		} else if (wParam == VK_DOWN) {
			DownKeyDown = false;
		}
		return 0;
	}

	case WM_TIMER: { //Executes this every frame.
		if (gameOver == true) {
			return 0;
		}

		while (elements < 40) { //Creates all of the circles.
			elements++;
			circles[elements].radius = (rand() % 50) + 2;
			circles[elements].x = -(rand() % 1000) - 51;
			circles[elements].y = (rand() % windowHeight) - 30;
			circles[elements].speed = ((float)(rand() % 200))/100 + 1;
			circles[elements].color = D2D1::ColorF(((float)(rand() % 255))/255, ((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255);
		}

		for (int i = 0; i < elements; i++) { //Recreates a circle if it goes off-screen.
			circles[i].x += circles[i].speed;
			if (circles[i].x - circles[i].radius - 1 > windowWidth) {
				circles[i].radius = (rand() % 50) + 2;
				circles[i].x = -(rand() % 1000) - 51;
				circles[i].y = (rand() % windowHeight) - 30;
				circles[i].speed = ((float)(rand() % 200)) / 100 + 1;
				circles[i].color = D2D1::ColorF(((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255);
			}
		}

		if (LeftKeyDown) { //Add velocity, depending on which keys are being pressed.
			player.vx -= 0.125;
		}
		if (RightKeyDown) {
			player.vx += 0.125;
		}
		if (UpKeyDown) {
			player.vy -= 0.125;
		}
		if (DownKeyDown) {
			player.vy += 0.125;
		}

		if (player.x + player.vx + 17 > windowWidth || player.x + player.vx < 0) { //Bounces the player off an edge.
			player.vx *= -0.5;
		} else {
			player.x += player.vx;
		}
		if (player.y + player.vy + 40 > windowHeight || player.y + player.vy < 0) {
			player.vy *= -0.5;
		} else {
			player.y += player.vy;
		}

		for (int i = 0; i < elements; i++) {
			double distanceToCircle = pow(pow(abs(player.x - circles[i].x), 2) + pow(abs(player.y - circles[i].y), 2), 0.5);
			if (circles[i].radius + player.radius > distanceToCircle) {
				if (circles[i].radius <= player.radius) {
					player.radius += 1;
					circles[i].radius = (rand() % 50) + 2;
					circles[i].x = -(rand() % 1000) - 51;
					circles[i].y = (rand() % windowHeight) - 30;
					circles[i].speed = ((float)(rand() % 200)) / 100 + 1;
					circles[i].color = D2D1::ColorF(((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255, ((float)(rand() % 255)) / 255);
					if (player.radius >= 1000) {
						gameOver = true;
					}
				} else {
					gameOver = true;
				}
			}
		}

		pRenderTarget->BeginDraw();
		pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
		for (int i = 0; i < 40; i++) {
			pBrush->SetColor(circles[i].color);
			pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(circles[i].x, circles[i].y), circles[i].radius, circles[i].radius), pBrush);
		}
		pBrush->SetColor(D2D1::ColorF(0.0F, 0.0F, 0.0F));
		pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(player.x, player.y), player.radius, player.radius), pBrush);
		if (gameOver == true) {
			const wchar_t* wszText_;
			if (player.radius < 1000) {
				pBrush->SetColor(D2D1::ColorF(1.0f, 0.0f, 0.0f));
				wszText_ = L"Game over! Press SPACE to restart.";
			} else {
				pBrush->SetColor(D2D1::ColorF(0.0f, 1.0f, 0.0f));
				wszText_ = L"Congratulations! You won! Press SPACE to play again.";
			}
			UINT32 cTextLength_ = (UINT32)wcslen(wszText_);
			pRenderTarget->DrawText(
				wszText_,        // The string to render.
				cTextLength_,    // The string's length.
				pTextFormat,    // The text format.
				D2D1::RectF(-10, 0, 800, 530),       // The region of the window where the text will be rendered.
				pBrush     // The brush used to draw the text.
			);
		}
		pRenderTarget->EndDraw();
		return 0;
	}

	case WM_CREATE: {
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory))) {
			return -1;
		}

		DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pWriteFactory));
		pWriteFactory->CreateTextFormat(L"Bahnschrift", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 40.0f, L"en-us", &pTextFormat);
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		RECT rc;
		GetClientRect(m_hwnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
		pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &pRenderTarget);
		pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f), &pBrush);
		srand(time(0));

		player.x = rc.right/2;
		player.y = rc.bottom/2;
		player.radius = 10;
		player.vx = 0;
		player.vy = 0;

		SetTimer(m_hwnd, 101, 15, NULL);
		return 0;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	default: {
		return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
	}

	}
	return TRUE;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	MainWindow win;

	if (!win.Create(hInstance, L"CIRCLES", WS_TILEDWINDOW)) {
		return 0;
	}
	ShowWindow(win.Window(), nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
