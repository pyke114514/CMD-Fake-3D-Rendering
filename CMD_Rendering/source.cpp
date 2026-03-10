#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include <queue>
#include <random>

using namespace std;

const WORD COL_SKY = FOREGROUND_BLUE | FOREGROUND_INTENSITY;										// 天空：亮蓝
const WORD COL_FLOOR = FOREGROUND_GREEN;															// 地面：绿
const WORD COL_WALL = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;	// 墙：亮白
const WORD COL_MAP_WALL = FOREGROUND_RED | FOREGROUND_INTENSITY;									// 小地图墙：亮红
const WORD COL_MAP_EMPTY = FOREGROUND_GREEN;														// 小地图空地：绿
const WORD COL_PLAYER = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;					// 玩家：黄

int nScreenWidth = 138;
int nScreenHeight = 46;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapWidth = 16;
int nMapHeight = 16;

float fFOV = 3.14 / 3.77f;
float fDepth = 16.0f;

float fRotateSpeed = 1.5f;
float fWalkSpeed = 3.0f;

static inline int Idx(int x, int y, int w) {
	return y * w + x;
}

int CountWallNeighbors8(const std::wstring& m, int w, int h, int x, int y) {
	int cnt = 0;
	for (int dy = -1; dy <= 1; ++dy) {
		for (int dx = -1; dx <= 1; ++dx) {
			if (dx == 0 && dy == 0) continue;
			int nx = x + dx, ny = y + dy;
			if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
			if (m[Idx(nx, ny, w)] == L'#') ++cnt;
		}
	}
	return cnt;
}

int FloodCountReachable(const std::wstring& m, int w, int h, int sx, int sy) {
	if (sx < 0 || sx >= w || sy < 0 || sy >= h) return 0;
	if (m[Idx(sx, sy, w)] == L'#') return 0;

	std::vector<char> vis(w * h, 0);
	std::queue<std::pair<int, int>> q;
	q.push({ sx, sy });
	vis[Idx(sx, sy, w)] = 1;
	int cnt = 0;

	const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };

	while (!q.empty()) {
		auto [x, y] = q.front();
		q.pop();
		++cnt;

		for (auto& d : dirs) {
			int nx = x + d[0], ny = y + d[1];
			if (nx < 0 || nx >= w || ny < 0 || ny >= h) continue;
			int id = Idx(nx, ny, w);
			if (!vis[id] && m[id] != L'#') {
				vis[id] = 1;
				q.push({ nx, ny });
			}
		}
	}
	return cnt;
}

std::wstring GenerateRandomMap16(
	int playerX, int playerY,
	double wallDensity = 0.18,
	unsigned int seed = std::random_device{}(),
	bool requireAllOpenConnected = true
) {
	const int W = 16, H = 16;
	std::wstring m(W * H, L' ');

	// 边界封墙
	for (int x = 0; x < W; ++x) {
		m[Idx(x, 0, W)] = L'#';
		m[Idx(x, H - 1, W)] = L'#';
	}
	for (int y = 0; y < H; ++y) {
		m[Idx(0, y, W)] = L'#';
		m[Idx(W - 1, y, W)] = L'#';
	}

	// 出生点周围保护 3x3
	auto inSafeZone = [&](int x, int y) {
		return std::abs(x - playerX) <= 1 && std::abs(y - playerY) <= 1;
		};

	std::vector<std::pair<int, int>> cells;
	for (int y = 1; y < H - 1; ++y) {
		for (int x = 1; x < W - 1; ++x) {
			if (inSafeZone(x, y)) continue;
			cells.push_back({ x, y });
		}
	}

	std::mt19937 rng(seed);
	std::shuffle(cells.begin(), cells.end(), rng);

	const int innerTotal = (W - 2) * (H - 2);
	const int targetWalls = static_cast<int>(innerTotal * wallDensity);

	int placed = 0;
	for (auto [x, y] : cells) {
		if (placed >= targetWalls) break;

		// 稀疏约束，防墙团聚
		int n8 = CountWallNeighbors8(m, W, H, x, y);
		if (n8 >= 4) continue;

		int id = Idx(x, y, W);
		if (m[id] == L'#') continue;

		m[id] = L'#';
		m[Idx(playerX, playerY, W)] = L' ';

		int openCount = 0;
		for (wchar_t c : m) if (c != L'#') ++openCount;
		int reach = FloodCountReachable(m, W, H, playerX, playerY);

		bool ok = requireAllOpenConnected
			? (reach == openCount)
			: (reach >= static_cast<int>(openCount * 0.9));

		if (ok) ++placed;
		else m[id] = L' '; // 回滚
	}

	m[Idx(playerX, playerY, W)] = L' ';
	return m;
}

int main() {
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	WORD* color = new WORD[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map = GenerateRandomMap16((int)fPlayerX, (int)fPlayerY, 0.10);

	//wstring map;

	//map += L"################";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"#              #";
	//map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1) {
		//通过计时来统一运行帧率
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		//输入处理
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			fPlayerA -= fRotateSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			fPlayerA += fRotateSpeed * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			fPlayerX += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * fWalkSpeed * fElapsedTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += sinf(fPlayerA) * fWalkSpeed * fElapsedTime;
				fPlayerY += cosf(fPlayerA) * fWalkSpeed * fElapsedTime;
			}
		}

		//简易射线检测
		for (int x = 0; x < nScreenWidth; ++x) {
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0.0f;
			bool bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth) {
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else if (map[nTestY * nMapWidth + nTestX] == '#') {
					bHitWall = true;

					//绘制墙体边缘
					vector<pair<float, float>> p;
					for (int tx = 0; tx < 2; ++tx) {
						for (int ty = 0; ty < 2; ++ty) {
							float vy = (float)nTestY + ty - fPlayerY;
							float vx = (float)nTestX + tx - fPlayerX;
							float d = sqrt(vx * vx + vy * vy);
							float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
							p.push_back(make_pair(d, dot));
						}
					}

					sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });

					float fBound = 0.0075f;
					if (acos(p.at(0).second) < fBound) bBoundary = true;
					if (acos(p.at(1).second) < fBound) bBoundary = true;
				}
			}
			int nCeiling = float(nScreenHeight / 2.0f) - nScreenHeight / (float)fDistanceToWall;
			int nFloor = nScreenHeight - nCeiling;

			//使用有渐变感的符号绘制图块
			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f)		nShade = 0x2588;
			else if (fDistanceToWall < fDepth / 3.0f)	nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.0f)	nShade = 0x2592;
			else if (fDistanceToWall < fDepth)			nShade = 0x2591;
			else										nShade = ' ';

			for (int y = 0; y < nScreenHeight; ++y) {
				int idx = y * nScreenWidth + x;

				if (y < nCeiling) {
					short nShade1 = ' ';
					float b = 1 - (nScreenHeight / 2.0f - (float)y) / ((float)nScreenHeight / 2.0f);
					if (b < 0.25f)		nShade1 = 0x2588;
					else if (b < 0.5f)	nShade1 = 0x2593;
					else if (b < 0.75f) nShade1 = 0x2592;
					else if (b < 0.9f)	nShade1 = 0x2591;
					else				nShade1 = ' ';
					screen[idx] = nShade1;
					color[idx] = COL_SKY;
				}
				else if (y == nCeiling) {
					screen[idx] = nShade;
					color[idx] = COL_MAP_WALL;
				}
				else if (y > nCeiling && y < nFloor) {
					screen[idx] = nShade;
					color[idx] = COL_WALL;
					if (bBoundary) color[idx] = COL_MAP_WALL;
				}
				else if (y == nFloor) {
					screen[idx] = nShade;
					color[idx] = COL_MAP_WALL;
				}
				else {
					short nShade1 = ' ';
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25f)		nShade1 = 0x2588;
					else if (b < 0.5f)	nShade1 = 0x2593;
					else if (b < 0.75f) nShade1 = 0x2592;
					else if (b < 0.9f)	nShade1 = 0x2591;
					else				nShade1 = ' ';
					screen[idx] = nShade1;
					color[idx] = COL_FLOOR;
				}
			}
		}

		//最终打印输出
		for (int nx = 0; nx < nMapWidth; ++nx) {
			for (int ny = 0; ny < nMapHeight; ++ny) {
				int idx = ny * nScreenWidth + nx;
				wchar_t c = map[ny * nMapWidth + nx];
				screen[idx] = 0x2588;
				color[idx] = (c == '#') ? COL_MAP_WALL : COL_MAP_EMPTY;
			}
		}

		int pidx = (int)fPlayerY * nScreenWidth + (int)fPlayerX;
		screen[pidx] = 0x2588;
		color[pidx] = COL_PLAYER;

		COORD origin = { 0, 0 };
		WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		WriteConsoleOutputAttribute(hConsole, color, nScreenWidth * nScreenHeight, origin, &dwBytesWritten);
	}

	return 0;
}