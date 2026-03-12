#include <algorithm>

#include "olcConsoleGameEngine.h"

using namespace std;

constexpr auto PI = 3.14159f;

class ConsoleFPS :public olcConsoleGameEngine {
public:
	ConsoleFPS() {
		m_sAppName = L"CMD FPS";
	}

	virtual bool OnUserCreate() {

		map += L"################################";
		map += L"#                              #";
		map += L"#   ###  ###          ###  ### #";
		map += L"#   #      #    ##    #      # #";
		map += L"#          ####    ####        #";
		map += L"#   #      #    ##    #      # #";
		map += L"#   #  ####  #    #   ####  ## #";
		map += L"#                              #";
		map += L"# #######    ######    ######  #";
		map += L"# #     ##    #    #    ##     #";
		map += L"# #          ##    ##          #";
		map += L"# ######      #    #      ######";
		map += L"#                              #";
		map += L"#  #######            ######## #";
		map += L"#  #     #    #####    #     # #";
		map += L"#  #     #    #   #    #     # #";
		map += L"#  #     ######   ######     # #";
		map += L"#  #                         # #";
		map += L"#  #####  #####   #####  ##### #";
		map += L"#                              #";
		map += L"#  ######      ####      #######";
		map += L"#  #    #      #  #      #     #";
		map += L"#  #    ####   #  #   ####     #";
		map += L"#  #           ####            #";
		map += L"#  ####  ##            ##  #####";
		map += L"#                              #";
		map += L"#   ########    ####    ###### #";
		map += L"#   #      #    #  #    #      #";
		map += L"#   #      ######  ######      #";
		map += L"#   #                          #";
		map += L"#                              #";
		map += L"################################";
		
		spriteWall = new olcSprite(L"fps_wall.spr");
		spriteLamp = new olcSprite(L"fps_lamp.spr");
		spriteFireBall = new olcSprite(L"fps_fireball.spr");

		fDepthBuffer = new float[ScreenWidth()];

		listObjects = {
			{ 2.5f,  2.5f, 0.0f, 0.0f, false, spriteLamp},
			{15.5f,  2.5f, 0.0f, 0.0f, false, spriteLamp},
			{26.5f,  2.5f, 0.0f, 0.0f, false, spriteLamp},

			{ 6.5f,  7.5f, 0.0f, 0.0f, false, spriteLamp},
			{16.5f,  7.5f, 0.0f, 0.0f, false, spriteLamp},
			{26.5f,  7.5f, 0.0f, 0.0f, false, spriteLamp},

			{ 6.5f, 10.5f, 0.0f, 0.0f, false, spriteLamp},
			{17.5f, 10.5f, 0.0f, 0.0f, false, spriteLamp},
			{27.5f, 10.5f, 0.0f, 0.0f, false, spriteLamp},

			{ 2.5f, 13.5f, 0.0f, 0.0f, false, spriteLamp},
			{15.5f, 13.5f, 0.0f, 0.0f, false, spriteLamp},

			{ 8.5f, 17.5f, 0.0f, 0.0f, false, spriteLamp},
			{16.5f, 17.5f, 0.0f, 0.0f, false, spriteLamp},
			{24.5f, 17.5f, 0.0f, 0.0f, false, spriteLamp},

			{ 5.5f, 19.5f, 0.0f, 0.0f, false, spriteLamp},
			{15.5f, 19.5f, 0.0f, 0.0f, false, spriteLamp},
			{25.5f, 19.5f, 0.0f, 0.0f, false, spriteLamp},

			{10.5f, 23.5f, 0.0f, 0.0f, false, spriteLamp},
			{28.5f, 23.5f, 0.0f, 0.0f, false, spriteLamp},

			{27.5f, 28.5f, 0.0f, 0.0f, false, spriteLamp},
		};

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) {
		//输入处理
		if (m_keys[L'J'].bHeld) {
			fPlayerA -= fRotateSpeed * fElapsedTime;
		}

		if (m_keys[L'L'].bHeld) {
			fPlayerA += fRotateSpeed * fElapsedTime;
		}

		if (m_keys[L'W'].bHeld) {
			fPlayerX += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		if (m_keys[L'S'].bHeld) {
			fPlayerX -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		if (m_keys[L'A'].bHeld) {
			fPlayerX -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		if (m_keys[L'D'].bHeld) {
			fPlayerX += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		if (m_keys[VK_SPACE].bReleased) {
			sObject o;
			o.x = fPlayerX;
			o.y = fPlayerY;

			float fNoise = (((float)rand() / (float)RAND_MAX) - 0.5f) * 0.01f;
			o.vx = sinf(fPlayerA + fNoise) * 8.0f;
			o.vy = cosf(fPlayerA + fNoise) * 8.0f;

			o.sprite = spriteFireBall;
			o.bRemove = false;
			listObjects.push_back(o);
		}

		//简易射线检测
		for (int x = 0; x < ScreenWidth(); ++x) {
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)ScreenWidth()) * fFOV;

			float fDistanceToWall = 0.0f;
			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			float fSampleX = 0.0f;

			while (!bHitWall && fDistanceToWall < fDepth) {
				fDistanceToWall += fStepSize;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else if (map.c_str()[nTestY * nMapWidth + nTestX] == '#') {
					bHitWall = true;

					float fBlockMidX = (float)nTestX + 0.5f;
					float fBlockMidY = (float)nTestY + 0.5f;

					float fTestPointX = fPlayerX + fEyeX * fDistanceToWall;
					float fTestPointY = fPlayerY + fEyeY * fDistanceToWall;

					float fTestAngle = atan2f((fTestPointY - fBlockMidY), (fTestPointX - fBlockMidX));

					if (fTestAngle >= -PI * 0.25f && fTestAngle < PI * 0.25f) {
						fSampleX = 1 - (fTestPointY - (float)nTestY);
					}
					if (fTestAngle >= PI * 0.25f && fTestAngle < PI * 0.75f) {
						fSampleX = 1 - (fTestPointX - (float)nTestX);
					}
					if (fTestAngle < -PI * 0.25f && fTestAngle >= -PI * 0.75f) {
						fSampleX = fTestPointX - (float)nTestX;
					}
					if (fTestAngle >= PI * 0.75f || fTestAngle < -PI * 0.75f) {
						fSampleX = fTestPointY - (float)nTestY;
					}
				}
			}

			int nCeiling = float(ScreenHeight() / 2.0f) - ScreenHeight() / (float)fDistanceToWall;
			int nFloor = ScreenHeight() - nCeiling;

			fDepthBuffer[x] = fDistanceToWall;

			for (int y = 0; y < ScreenHeight(); ++y) {

				if (y <= nCeiling) {
					Draw(x, y, PIXEL_HALF, FG_DARK_CYAN);
				}
				else if (y > nCeiling && y <= nFloor) {
					if (fDistanceToWall < fDepth) {
						float fSampleY = ((float)y - (float)nCeiling) / ((float)nFloor - (float)nCeiling);
						wchar_t g = spriteWall->SampleGlyph(fSampleX, fSampleY);
						short col = spriteWall->SampleColour(fSampleX, fSampleY);

						// 非常远时，降低字形密度（模拟距离衰减）
						if (fDistanceToWall > fDepth / 2.0f)		g = PIXEL_QUARTER;
						else if (fDistanceToWall > fDepth / 3.0f)	g = PIXEL_HALF;
						else if (fDistanceToWall > fDepth / 4.0f)	g = PIXEL_THREEQUARTERS;

						Draw(x, y, g, col);
					}
					else {
						Draw(x, y, PIXEL_HALF, FG_DARK_CYAN);
					}
				}
				else {
					float b = ((float)y - ScreenHeight() * 0.5f) / (ScreenHeight() * 0.5f);
					short col = FG_DARK_GREEN;
					wchar_t g;

					if (b < 0.25f)		g = PIXEL_QUARTER;
					else if (b < 0.5f)	g = PIXEL_HALF;
					else if (b < 0.75f) g = PIXEL_THREEQUARTERS;
					else				g = PIXEL_SOLID;

					Draw(x, y, g, col);
				}
			}
		}

		for (auto& object : listObjects) {
			object.x += object.vx * fElapsedTime;
			object.y += object.vy * fElapsedTime;

			if (map.c_str()[(int)object.y * nMapWidth + (int)object.x] == '#') {
				object.bRemove = true;
			}

			float fVecX = object.x - fPlayerX;
			float fVecY = object.y - fPlayerY;
			float fDistanceFromPlayer = sqrtf(fVecX * fVecX + fVecY * fVecY);

			float fEyeX = sinf(fPlayerA);
			float fEyeY = cosf(fPlayerA);
			float fObjectAngle = atan2f(fEyeY, fEyeX) - atan2f(fVecY, fVecX);
			if (fObjectAngle < -PI)
				fObjectAngle += 2.0f * PI;
			if (fObjectAngle > PI)
				fObjectAngle -= 2.0f * PI;

			bool bInPlayerFOV = fabs(fObjectAngle) < fFOV / 2.0f;

			if (bInPlayerFOV && fDistanceFromPlayer >= 0.5f && fDistanceFromPlayer < fDepth) {
				float fObjectCeiling = (float)(ScreenHeight() / 2.0) - ScreenHeight() / ((float)fDistanceFromPlayer);
				float fObjectFloor = ScreenHeight() - fObjectCeiling;
				float fObjectHeight = fObjectFloor - fObjectCeiling;
				float fObjectAspectRatio = (float)object.sprite->nHeight / (float)object.sprite->nWidth;
				float fObjectWidth = fObjectHeight / fObjectAspectRatio;

				float fMiddleOfObject = (0.5f * (fObjectAngle / (fFOV / 2.0f)) + 0.5f) * (float)ScreenWidth();

				for (float lx = 0; lx < fObjectWidth; ++lx) {
					for (float ly = 0; ly < fObjectHeight; ++ly) {
						float fSampleX = lx / fObjectWidth;
						float fSampleY = ly / fObjectHeight;
						wchar_t c = object.sprite->SampleGlyph(fSampleX, fSampleY);
						int nObjectColumn = (int)(fMiddleOfObject + lx - (fObjectWidth / 2.0f));
						if (nObjectColumn >= 0 && nObjectColumn < ScreenWidth()) {
							if (c != L' ' && fDepthBuffer[nObjectColumn] >= fDistanceFromPlayer) {
								if (fDistanceFromPlayer > fDepth / 2.0f)		c = PIXEL_QUARTER;
								else if (fDistanceFromPlayer > fDepth / 3.0f)	c = PIXEL_HALF;
								else if (fDistanceFromPlayer > fDepth / 4.0f)	c = PIXEL_THREEQUARTERS;
								Draw(nObjectColumn, fObjectCeiling + ly, c, object.sprite->SampleColour(fSampleX, fSampleY));
								fDepthBuffer[nObjectColumn] = fDistanceFromPlayer;
							}
						}
					}
				}
			}
		}

		listObjects.remove_if([](sObject& o) {return o.bRemove; });

		//最终打印输出
		for (int nx = 0; nx < nMapWidth; ++nx) {
			for (int ny = 0; ny < nMapHeight; ++ny) {
				Draw(nx, ny, map[ny * nMapWidth + nx]);
			}
		}

		Draw((int)fPlayerX, (int)fPlayerY, 'P');

		return true;
	}

private:
	float fPlayerX = 16.0f;
	float fPlayerY = 16.0f;
	float fPlayerA = 0.0f;

	wstring map;
	int nMapWidth = 32;
	int nMapHeight = 32;

	float fFOV = PI / 4.0f;
	float fDepth = 16.0f;
	float fStepSize = 0.01f;

	float fRotateSpeed = 1.25f;
	float fWalkSpeed = 2.25f;

	olcSprite* spriteWall;
	olcSprite* spriteLamp;
	olcSprite* spriteFireBall;

	float* fDepthBuffer = nullptr;

	struct sObject {
		float x;
		float y;
		float vx;
		float vy;
		bool bRemove;
		olcSprite* sprite;
	};

	list<sObject> listObjects;
	float Clamp01(float v) {
		return max(0.0f, min(1.0f, v));
	}
};

int main() {
	ConsoleFPS game;
	game.ConstructConsole(256, 144, 4, 4);
	game.Start();

	return 0;
}