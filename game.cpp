#pragma once
#include "Animation.h"
#include "Actor.h"
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"MSIMG32.LIB")
using namespace std;
IMAGE Background;
IMAGE test;
IMAGE EMPTYBLOOD;
IMAGE SOUL;
IMAGE OPENGROUND;
IMAGE TITLE;
IMAGE LOGO;
IMAGE PRESS_TO_START;
IMAGE PLAYAGAIN; 
IMAGE COIN; 
IMAGE MONEY;



//返回单位化的向量，p1指向p2
Vector2 get_normal_direction(Point p1, Point p2)
{
	double Module = sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
	return Vector2((p2.x - p1.x) / Module, (p2.y - p1.y) / Module);
}
//返回向量
Vector2 get_direction(Point p1, Point p2)
{
	return Vector2(p2.x - p1.x, p2.y - p1.y);
}



IMAGE RotateImage_Alpha(IMAGE* pImg, double radian, COLORREF bkcolor = BLACK)
{
	radian = -radian;														// 由于 y 轴翻转，旋转角度需要变负
	float fSin = (float)sin(radian), fCos = (float)cos(radian);				// 存储三角函数值
	float fNSin = (float)sin(-radian), fNCos = (float)cos(-radian);
	int left = 0, top = 0, right = 0, bottom = 0;							// 旋转后图像顶点
	int w = pImg->getwidth(), h = pImg->getheight();
	DWORD* pBuf = GetImageBuffer(pImg);
	POINT points[4] = { {0, 0}, {w, 0}, {0, h}, {w, h} };					// 存储图像顶点
	for (int j = 0; j < 4; j++)												// 旋转图像顶点，搜索旋转后的图像边界
	{
		points[j] = {
			(int)(points[j].x * fCos - points[j].y * fSin),
			(int)(points[j].x * fSin + points[j].y * fCos)
		};
		if (points[j].x < points[left].x)	left = j;
		if (points[j].y > points[top].y)	top = j;
		if (points[j].x > points[right].x)	right = j;
		if (points[j].y < points[bottom].y)	bottom = j;
	}

	int nw = points[right].x - points[left].x;								// 旋转后的图像尺寸
	int nh = points[top].y - points[bottom].y;
	int nSize = nw * nh;
	int offset_x = points[left].x < 0 ? points[left].x : 0;					// 旋转后图像超出第一象限的位移（据此调整图像位置）
	int offset_y = points[bottom].y < 0 ? points[bottom].y : 0;

	IMAGE img(nw, nh);
	DWORD* pNewBuf = GetImageBuffer(&img);
	if (bkcolor != BLACK)													// 设置图像背景色
		for (int i = 0; i < nSize; i++)
			pNewBuf[i] = BGR(bkcolor);

	for (int i = offset_x, ni = 0; ni < nw; i++, ni++)						// i 用于映射原图像坐标，ni 用于定位旋转后图像坐标
	{
		for (int j = offset_y, nj = 0; nj < nh; j++, nj++)
		{
			int nx = (int)(i * fNCos - j * fNSin);							// 从旋转后的图像坐标向原图像坐标映射
			int ny = (int)(i * fNSin + j * fNCos);
			if (nx >= 0 && nx < w && ny >= 0 && ny < h)						// 若目标映射在原图像范围内，则拷贝色值
				pNewBuf[nj * nw + ni] = pBuf[ny * w + nx];
		}
	}

	return img;
}
/////////////////////////////////////////////



//绘图处理函数
void putimage_1(int x, int y, IMAGE* img)
{
	int w = img->getwidth();
	int h = img->getheight();
	AlphaBlend(GetImageHDC(NULL), x, y, w, h,
		GetImageHDC(img), 0, 0, w, h, { AC_SRC_OVER,0,255,AC_SRC_ALPHA });
}



//AnimationA::~AnimationA()
//{
//	for (int i = 0; i < frame_num; i++)
//	{
//		delete frame_list[i];
//	}
//}







//class EffectPlayer
//{
//public:Vector2 RelativeLocation;
//	  IMAGE Effect;
//	  int StartFrame = 0;
//	  int EndFrame = 0;
//	  EffectPlayer() { RelativeLocation.vx = 0; RelativeLocation.vy = 0; };
//	  void LoadEffect(LPCTSTR path, int x, int y, int start, int end) {
//		  TCHAR path_file[256]; _stprintf_s(path_file, path);
//		  loadimage(&Effect, path_file);
//		  RelativeLocation.vx = x; RelativeLocation.vy = y; StartFrame = start; EndFrame = end;
//	  }
//};

Actor Player(200, 680);
Actor testenemy(1000, 585);



Animation PLAYER_IDLE[2];
Animation PLAYER_WALK[2];
Animation PLAYER_DASH[2];
Animation PLAYER_JUMPSTART[2];
Animation PLAYER_ATTACK[2];
Animation PLAYER_UPATTACK[2];
Animation PLAYER_JUMPLOOP[2];
Animation PLAYER_JUMPLAND;
Animation PLAYER_ATTACKTWICE[2];
Animation PLAYER_DOUBLEJUMP[2];
Animation PLAYER_DOWNATTACK[2];
Animation PLAYER_DAMAGE[2];
Animation PLAYER_ATTACKHIT[2];
Animation PLAYER_DEATH;



Animation B_IDLE[2];
Animation B_JUMPDASH[2];
Animation B_WALK[2];
Animation B_THROWSIDE[2];
Animation B_JUMP[2];
Animation B_DASH[2];
Animation B_LAND[2];
Animation B_JUMPFINAL[2];



Animation BLOOD;
Animation BLOOD_DAMAGE;
Animation ARROR[2];



Animation CurrentEnemyAnimation;
Animation CurrentPlayerAnimation;
Animation CurrentPlayerLoopAnimation;







bool attack_judger(Actor& player, Actor& enemy)
{
	if (enemy.UnHurtableTime == 0)
	{
		AnimationName Playeranimname = CurrentPlayerAnimation.animationname;
		if (CurrentEnemyAnimation.animationname == NU)
		{
			return false;
		}
		if (Playeranimname == PLAYER_ATTACK0 || Playeranimname == PLAYER_UPATTACK0
			|| Playeranimname == PLAYER_ATTACKTWICE0)
		{
			Vector2 RelativeVector = get_direction(player.position, enemy.position);
			Point EffectCenter, EnemyCenter;
			int w1 = CurrentPlayerAnimation.Effects[0]->getwidth();
			int h1 = CurrentPlayerAnimation.Effects[0]->getheight();
			int w2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth() - 50;
			int h2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getheight() - 50;
			EffectCenter.x = player.position.x + CurrentPlayerAnimation.dmove[0][0] + (2 * player.Facing_Right - 1) * w1 / 2;
			EffectCenter.y = player.position.y + CurrentPlayerAnimation.dmove[0][1] + (2 * player.Facing_Right - 1) * h1 / 2;
			EnemyCenter.x = enemy.position.x + w2 / 2;
			EnemyCenter.y = enemy.position.y + h2 / 2;
			if ((player.Facing_Right * 2 - 1) * RelativeVector.vx >= 0)
			{
				int dx = abs(EnemyCenter.x - EffectCenter.x);
				int dy = abs(EnemyCenter.y - EffectCenter.y);
				if (dx < (w1 + w2) / 2 &&
					dy < (h1 + h2) / 2 && enemy.UnHurtableTime == 0)
				{
					cout << "ok!" << endl;
					return true;
				}
				return false;
			}

			return false;
		}
		else if (Playeranimname == PLAYER_DOWNATTACK0)
		{
			if (player.position.y - enemy.position.y >= 0 && player.position.y - enemy.position.y <= 550
				&& player.position.x - enemy.position.x >= -100 && player.position.x - enemy.position.x <= 150)
			{
				 return true;
			}return false;
		}
		return  false;
	}
	return false;
}


Animation nullanimation; IMAGE nullimg; Vector2 shootdirection, normal_shootdirection;
void drawplayer(Actor& Player, Animation& CurrentPlay)
{
	Player.position.x += CurrentPlay.dmove[CurrentPlay.current_frameidx][0]; Player.position.y += CurrentPlay.dmove[CurrentPlay.current_frameidx][1];
	CurrentPlay.tmp_interval[CurrentPlay.current_frameidx] -= 1;
	putimage_1(Player.position.x, Player.position.y, CurrentPlay.frame_list[CurrentPlay.current_frameidx]);//对每个动画类，在当前位置播放当前帧数动画
	if (CurrentPlay.Effects.size() > 0 && CurrentPlay.current_frameidx >= CurrentPlay.StartFrame)//判断是否存在特效
	{   //防越界
		int m = min(CurrentPlay.Effects.size() - 1, CurrentPlay.current_frameidx - CurrentPlay.StartFrame);
		putimage_1(Player.position.x + CurrentPlay.Erelative[m].vx, Player.position.y + CurrentPlay.Erelative[m].vy,
			CurrentPlay.Effects[m]);
	}
	
	if (CurrentPlay.PlayHitAnimation==true&&CurrentPlay.HitEffects.size()>0)
	{
		int m = min(CurrentPlay.HitEffects.size() - 1, CurrentPlay.current_frameidx - CurrentPlay.HitStartFrame);
		putimage_1(Player.position.x + CurrentPlay.HitErelative[m].vx, Player.position.y + CurrentPlay.HitErelative[m].vy,
			CurrentPlay.HitEffects[m]);
	}

	if (CurrentPlay.tmp_interval[CurrentPlay.current_frameidx] == 0)
	{
		Player.position.x += CurrentPlay.relative[CurrentPlay.current_frameidx].vx; Player.position.y += CurrentPlay.relative[CurrentPlay.current_frameidx].vy;

		CurrentPlay.current_frameidx += 1;
		if (CurrentPlay.current_frameidx == CurrentPlay.frame_num && CurrentPlay.canloop == true)
		{
			CurrentPlay.tmp_interval = CurrentPlay.frameinterval;
			CurrentPlay.current_frameidx = CurrentPlay.loop_index; return;
		}
		if (CurrentPlay.current_frameidx == CurrentPlay.frame_num && CurrentPlay.canloop == false)
		{
			CurrentPlay = nullanimation; CurrentPlay.Effects.clear();
			return;
		}
	}
	//	FlushBatchDraw();
}


void drawUI(Animation& CurrentPlay, int x, int y)
{
	CurrentPlay.tmp_interval[CurrentPlay.current_frameidx] -= 1;
	putimage_1(x + CurrentPlay.dmove[CurrentPlay.current_frameidx][0], y + CurrentPlay.dmove[CurrentPlay.current_frameidx][1], CurrentPlay.frame_list[CurrentPlay.current_frameidx]);//对每个动画类，在当前位置播放当前帧数动画
	if (CurrentPlay.tmp_interval[CurrentPlay.current_frameidx] == 0)
	{
		CurrentPlay.current_frameidx += 1;
		if (CurrentPlay.current_frameidx == CurrentPlay.frame_num && CurrentPlay.canloop == true)
		{
			CurrentPlay.current_frameidx = 0;
			CurrentPlay.tmp_interval = CurrentPlay.frameinterval;
			CurrentPlay.current_frameidx = CurrentPlay.loop_index; return;
		}
		if (CurrentPlay.current_frameidx == CurrentPlay.frame_num && CurrentPlay.canloop == false)
		{
			CurrentPlay.current_frameidx = 0;
			CurrentPlay.tmp_interval = CurrentPlay.frameinterval;
			return;
		}
	}
}


//有面部朝向判断，主要以玩家为视角
//bool attack_judger(Actor player1, Actor player2, int mindx, int maxdx, int mindy, int maxdy)
//{
//	Vector2 RelativeVector = get_direction(player1.position, player2.position);
//	int dx = (player1.Facing_Right * 2 - 1) * RelativeVector.vx;//正确朝向时非负
//	int dy = RelativeVector.vy;
//	if (dx >= 0 && dx >= mindx && dx <= maxdx && dy >= mindy && dy <= maxdy)
//	{
//		return true;
//	}
//	return false;
//}



bool attacked_judger(Actor& player, Actor& enemy)
{
	Point PlayerCenter; Point EnemyCenter;
	if (CurrentPlayerAnimation.animationname == NU&&CurrentPlayerLoopAnimation.animationname!=NU)
	{
		int w1 = CurrentPlayerLoopAnimation.frame_list[CurrentPlayerLoopAnimation.current_frameidx]->getwidth();
		int w2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth();
		int h1 = CurrentPlayerLoopAnimation.frame_list[CurrentPlayerLoopAnimation.current_frameidx]->getheight();
		int h2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getheight();
		PlayerCenter.x = player.position.x + w1 / 2;
		PlayerCenter.y = player.position.y + h1 / 2;
		EnemyCenter.x = enemy.position.x + w2 / 2;
		EnemyCenter.y = enemy.position.y + h2 / 2;
		if (CurrentEnemyAnimation.animationname == B_JUMPFINAL0 && CurrentEnemyAnimation.current_frameidx >= 21 && CurrentEnemyAnimation.current_frameidx <= 36)
		{
			if (abs(PlayerCenter.x - EnemyCenter.x) < (w1 + w2) / 2 + 100 &&
				abs(PlayerCenter.y - EnemyCenter.y) < (h1 + h2) / 2 + 110 && player.UnHurtableTime == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (abs(PlayerCenter.x - EnemyCenter.x) < (w1 + w2) / 2 - 90 &&
				abs(PlayerCenter.y - EnemyCenter.y) < (h1 + h2) / 2 - 80 && player.UnHurtableTime == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	if (CurrentPlayerAnimation.animationname != NU)
	{
		int w1 = CurrentPlayerAnimation.frame_list[CurrentPlayerAnimation.current_frameidx]->getwidth();
		int w2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth();
		int h1 = CurrentPlayerAnimation.frame_list[CurrentPlayerAnimation.current_frameidx]->getheight();
		int h2 = CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getheight();
		PlayerCenter.x = player.position.x + w1 / 2;
		PlayerCenter.y = player.position.y + h1 / 2;
		EnemyCenter.x = enemy.position.x + w2 / 2;
		EnemyCenter.y = enemy.position.y + h2 / 2;
		if (CurrentEnemyAnimation.animationname == B_JUMPFINAL0 && CurrentEnemyAnimation.current_frameidx >= 21 && CurrentEnemyAnimation.current_frameidx <= 36)
		{
			if (abs(PlayerCenter.x - EnemyCenter.x) < (w1 + w2) / 2 + 100 &&
				abs(PlayerCenter.y - EnemyCenter.y) < (h1 + h2) / 2 + 110 && player.UnHurtableTime == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		else
		{
			if (abs(PlayerCenter.x - EnemyCenter.x) < (w1 + w2) / 2 - 90 &&
				abs(PlayerCenter.y - EnemyCenter.y) < (h1 + h2) / 2 - 80 && player.UnHurtableTime == 0)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;

}




void gravity_collide(Actor& player, Actor& enemy) {
	CurrentPlayerAnimation.animationname;
	player.speed.vy += player.gravity;
	player.position.y += player.speed.vy / 30;
	enemy.speed.vy += enemy.gravity;
	enemy.position.y += enemy.speed.vy / 30;
	if (player.position.y > 680)
	{
		player.speed.vy = 0;
		player.position.y = 680;
	}
	if (enemy.position.y > 585)
	{
		enemy.position.y = 585;
	}
	if (CurrentPlayerAnimation.animationname == PLAYER_DASH0)
	{
		player.speed.vy = 0;
	}
	if (CurrentPlayerAnimation.animationname == PLAYER_DOWNATTACK0 && player.position.y == 680)
	{
		player.speed.vy = -30;
	}
	if (CurrentEnemyAnimation.animationname == B_JUMPFINAL0 && CurrentEnemyAnimation.current_frameidx >= 15)
	{
		if (CurrentEnemyAnimation.current_frameidx <= 33)
		{
			enemy.gravity = 0; enemy.speed.vy = 0; enemy.speed.vx = 0;
		}
		else { enemy.speed.vy = 0; enemy.gravity = 4; }
	}
}

void MusicPlayer(Animation& CurrentPlayerAnimation, Animation& CurrentEnemyAnimation, int& CD)
{
	if (CurrentPlayerAnimation.animationname != NU && CurrentPlayerAnimation.tmp_interval[0] == CurrentPlayerAnimation.frameinterval[0])
	{
		if (CurrentPlayerAnimation.animationname == PLAYER_ATTACK0)
		{
			if (attack_judger(Player, testenemy))
			{
				mciSendString(_T("play attackhit from 0"), NULL, 0, NULL);
			}
			else { mciSendString(_T("play attack1 from 0"), NULL, 0, NULL); }

		}
		else if (CurrentPlayerAnimation.animationname == PLAYER_ATTACKTWICE0)
		{
			mciSendString(_T("play attack2 from 0"), NULL, 0, NULL);
		}
		else if (CurrentPlayerAnimation.animationname == PLAYER_UPATTACK0)
		{
			mciSendString(_T("play attackup from 0"), NULL, 0, NULL);
		}
		else if (CurrentPlayerAnimation.animationname == PLAYER_DOWNATTACK0)
		{
			mciSendString(_T("play attack1 from 0"), NULL, 0, NULL);
		}
		else if (CurrentPlayerAnimation.animationname == PLAYER_DAMAGE0)
		{
			mciSendString(_T("play playerdamage from 0"), NULL, 0, NULL);
		}
	}


	if (CD == 0 && CurrentEnemyAnimation.animationname != NU && CurrentEnemyAnimation.tmp_interval[0] == CurrentEnemyAnimation.frameinterval[0])
	{
		srand((unsigned)time(NULL)); int idx = rand() % 2;
		if (CurrentEnemyAnimation.animationname == B_WALK0 ||
			CurrentEnemyAnimation.animationname == B_IDLEL0 ||
			CurrentEnemyAnimation.animationname == B_IDLER0 ||
			CurrentEnemyAnimation.animationname == B_LAND0)
		{
			return;
		}
		else if (CurrentEnemyAnimation.animationname == B_JUMP0)
		{
			if (idx == 0)
			{
				mciSendString(_T("play haha from 0"), NULL, 0, NULL); CD = 90; return;
			}
			else
			{
				mciSendString(_T("play henhen from 0"), NULL, 0, NULL); CD = 90; return;
			}
		}
		else
		{
			idx = rand() % 6;
			switch (idx)
			{
			case 0:mciSendString(_T("play aidito from 0"), NULL, 0, NULL); CD = 90; break;
			case 1:mciSendString(_T("play ha from 0"), NULL, 0, NULL); CD = 90; break;
			case 2:mciSendString(_T("play gadama from 0"), NULL, 0, NULL); CD = 90; break;
			case 3:mciSendString(_T("play heigali from 0"), NULL, 0, NULL); CD = 90; break;
			case 4:mciSendString(_T("play higali from 0"), NULL, 0, NULL); CD = 90; break;
			case 5:mciSendString(_T("play xiao from 0"), NULL, 0, NULL); CD = 90; break;
			default:
				break;
			}return;
		}

	}
}

int scenemanager = 0;


//总体：角色类不再继承动画类，二者独立
int main() {
	loadimage(&COIN, _T("UI/coin.png"));
	loadimage(&MONEY, _T("UI/money.png"));
	loadimage(&PLAYAGAIN,_T("UI/playagain.png"));
	loadimage(&test, _T("img/hollow knight/Damage/00.png"));


	mciSendString(_T("open music/Hornet.mp3 alias bgm"), NULL, 0, NULL);
	mciSendString(_T("open music/Player/sword_1.wav alias attack1"), NULL, 0, NULL);
	mciSendString(_T("open music/Player/sword_2.wav alias attack2"), NULL, 0, NULL);
	mciSendString(_T("open music/Player/sword_hit.wav alias attackhit"), NULL, 0, NULL);
	mciSendString(_T("open music/Player/sword_up.wav alias attackup"), NULL, 0, NULL);

	mciSendString(_T("open music/Player/player_damage.wav alias playerdamage"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/aidito.mp3 alias aidito"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/dagama.mp3 alias dagama"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/ha.mp3 alias ha"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/haha.mp3 alias haha"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/heigali.mp3 alias heigali"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/henhen.mp3 alias henhen"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/higali.mp3 alias higali"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/xiao.mp3 alias xiao"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/open.mp3 alias open"), NULL, 0, NULL);
	mciSendString(_T("open music/Boss/hea.mp3 alias hea"), NULL, 0, NULL);




	PLAYER_IDLE[0].LoadAnimation(_T("img/hollow knight/Idle/%d.PNG"), 9, PLAYER_IDLE0); PLAYER_IDLE[0].canloop = 1; PLAYER_IDLE[0].loop_index = 0;
	for (int i = 0; i < 9; i++) { PLAYER_IDLE[0].frameinterval[i] = 4; PLAYER_IDLE[0].tmp_interval[i] = PLAYER_IDLE[0].frameinterval[i]; }

	PLAYER_IDLE[1].LoadAnimation(_T("img/hollow knight/IdleR/%d.PNG"), 9, PLAYER_IDLE0); PLAYER_IDLE[1].canloop = 1; PLAYER_IDLE[1].loop_index = 0;
	for (int i = 0; i < 9; i++) { PLAYER_IDLE[1].frameinterval[i] = 4; PLAYER_IDLE[1].tmp_interval[i] = PLAYER_IDLE[1].frameinterval[i]; }


	PLAYER_WALK[0].LoadAnimation(_T("img/hollow knight/Walk/%d.PNG"), 8, PLAYER_WALKL0); PLAYER_WALK[0].loop_index = 3; PLAYER_WALK[0].canloop = 1;
	for (int i = 0; i < 8; i++) { PLAYER_WALK[0].frameinterval[i] = 60; }
	for (int i = 0; i < 8; i++) {
		PLAYER_WALK[0].dmove[i][0] = 0; PLAYER_WALK[0].dmove[i][1] = 0;
		PLAYER_WALK[0].frameinterval[i] = 2; PLAYER_WALK[0].tmp_interval[i] = PLAYER_WALK[0].frameinterval[i];
	}

	PLAYER_WALK[1].LoadAnimation(_T("img/hollow knight/WalkR/%d.PNG"), 8, PLAYER_WALKR0); PLAYER_WALK[1].loop_index = 3; PLAYER_WALK[1].canloop = 1;
	for (int i = 0; i < 8; i++) { PLAYER_WALK[1].frameinterval[i] = 60; }
	for (int i = 0; i < 8; i++) {
		PLAYER_WALK[1].dmove[i][0] = 0; PLAYER_WALK[1].dmove[i][1] = 0;
		PLAYER_WALK[1].frameinterval[i] = 2; PLAYER_WALK[1].tmp_interval[i] = PLAYER_WALK[1].frameinterval[i];
	}


	PLAYER_DASH[0].LoadAnimation(_T("img/hollow knight/Dash/%d.PNG"), 5, PLAYER_DASH0);
	PLAYER_DASH[0].LoadEffect(_T("img/hollow knight/DashEffect/%d.png"), 0, 4, 4);
	for (int i = 0; i < 4; i++)
	{
		PLAYER_DASH[0].Effects[i]->Resize(400, 600);
		PLAYER_DASH[0].Erelative[i].vx = 20 + 4 * i; PLAYER_DASH[0].Erelative[i].vy = -100;
	}PLAYER_DASH[0].canloop = false;
	for (int i = 0; i < 5; i++) {
		PLAYER_DASH[0].dmove[i][0] = -45; PLAYER_DASH[0].dmove[i][1] = 0; PLAYER_DASH[0].frameinterval[i] = 1;
		if (i == 3 || i == 4) { PLAYER_DASH[0].frameinterval[i] = 3; }
		PLAYER_DASH[0].tmp_interval[i] = PLAYER_DASH[0].frameinterval[i];
	}
	PLAYER_DASH[0].CD = 0;

	PLAYER_DASH[1].LoadAnimation(_T("img/hollow knight/DashR/%d.PNG"), 5, PLAYER_DASH0);
	PLAYER_DASH[1].LoadEffect(_T("img/hollow knight/DashEffectR/%d.png"), 0, 4, 4);
	for (int i = 0; i < 4; i++)
	{
		PLAYER_DASH[1].Effects[i]->Resize(600, 800);
		PLAYER_DASH[1].Erelative[i].vx = -430 + 4 * i; PLAYER_DASH[1].Erelative[i].vy = -100;
	}PLAYER_DASH[1].canloop = false;
	for (int i = 0; i < 5; i++) {
		PLAYER_DASH[1].dmove[i][0] = 45; PLAYER_DASH[1].dmove[i][1] = 0; PLAYER_DASH[1].frameinterval[i] = 1;
		if (i == 3 || i == 4) { PLAYER_DASH[1].frameinterval[i] = 3; }
		PLAYER_DASH[1].tmp_interval[i] = PLAYER_DASH[1].frameinterval[i];
	}PLAYER_DASH[1].relative[0].vx = -75;
	PLAYER_DASH[1].relative[1].vx = 15; PLAYER_DASH[1].relative[2].vx = 15;
	PLAYER_DASH[1].relative[3].vx = 45; PLAYER_DASH[1].relative[4].vx = 25;
	PLAYER_DASH[1].CD = 0;


	//PLAYER_STOP.LoadAnimation(_T("img/hollow knight/Stop/%d.PNG"), 5, "PLAYER_STOP"); PLAYER_STOP.loop_index = 2; for (int i = 0; i < 5; i++) { PLAYER_STOP.frameinterval[i] = 2; }


	PLAYER_ATTACK[0].LoadAnimation(_T("img/hollow knight/Attack/Attack/1/%d.PNG"), 5, PLAYER_ATTACK0); PLAYER_ATTACK[0].canloop = 0;
	PLAYER_ATTACK[0].LoadEffect(_T("img/hollow knight/Attack/Attack/1/0%d.png"), 3, 4, 2);
	for (int i = 0; i < 5; i++) {
		PLAYER_ATTACK[0].dmove[i][0] = 0; PLAYER_ATTACK[0].dmove[i][1] = 0;
		if (i == 1)
		{
			PLAYER_ATTACK[0].frameinterval[i] = 1; PLAYER_ATTACK[0].tmp_interval[i] = PLAYER_ATTACK[0].frameinterval[i];
		}
		PLAYER_ATTACK[0].frameinterval[i] = 2; PLAYER_ATTACK[0].tmp_interval[i] = PLAYER_ATTACK[0].frameinterval[i];
		if (i == 3) { PLAYER_ATTACK[0].dmove[i][0] = -2; }
	}PLAYER_ATTACK[0].CD = 45;
	PLAYER_ATTACK[0].Erelative[0].vy = 20; PLAYER_ATTACK[0].Erelative[0].vx = -165;
	PLAYER_ATTACK[0].Erelative[1].vx = -135; PLAYER_ATTACK[0].Erelative[1].vy = 30;

	PLAYER_ATTACK[1].LoadAnimation(_T("img/hollow knight/AttackR/Attack/1/%d.PNG"), 5, PLAYER_ATTACK0); PLAYER_ATTACK[1].canloop = 0;
	PLAYER_ATTACK[1].LoadEffect(_T("img/hollow knight/AttackR/Attack/1/0%d.png"), 3, 4, 2);
	for (int i = 0; i < 5; i++) {
		PLAYER_ATTACK[1].dmove[i][0] = 0; PLAYER_ATTACK[1].dmove[i][1] = 0;
		if (i == 1)
		{
			PLAYER_ATTACK[1].frameinterval[i] = 1; PLAYER_ATTACK[1].tmp_interval[i] = PLAYER_ATTACK[1].frameinterval[i];
		}
		PLAYER_ATTACK[1].frameinterval[i] = 2; PLAYER_ATTACK[1].tmp_interval[i] = PLAYER_ATTACK[1].frameinterval[i];
		if (i == 3) { PLAYER_ATTACK[1].dmove[i][0] = 2; }
	}PLAYER_ATTACK[1].CD = 30;
	PLAYER_ATTACK[1].Erelative[0].vx = 90; PLAYER_ATTACK[1].Erelative[0].vy = 20;
	PLAYER_ATTACK[1].Erelative[1].vx = 50; PLAYER_ATTACK[1].Erelative[1].vy = 25;


	PLAYER_ATTACKTWICE[0].LoadAnimation(_T("img/hollow knight/Attack/Attack/2/%d.png"), 5, PLAYER_ATTACKTWICE0);
	PLAYER_ATTACKTWICE[0].LoadEffect(_T("img/hollow knight/Attack/Attack/2/0%d.png"), 2, 3, 2);
	for (int i = 0; i < 5; i++)
	{
		if (i == 1)
		{
			PLAYER_ATTACKTWICE[0].frameinterval[i] = 1; PLAYER_ATTACKTWICE[0].tmp_interval[i] = PLAYER_ATTACKTWICE[0].frameinterval[i];
		}
		PLAYER_ATTACKTWICE[0].frameinterval[i] = 2; PLAYER_ATTACKTWICE[0].tmp_interval[i] = PLAYER_ATTACKTWICE[0].frameinterval[i];
		if (i == 3) { PLAYER_ATTACKTWICE[0].dmove[i][0] = -2; }
	}PLAYER_ATTACKTWICE[0].CD = 45; PLAYER_ATTACKTWICE[0].canloop = false;
	PLAYER_ATTACKTWICE[0].Erelative[0].vx = -145; PLAYER_ATTACKTWICE[0].Erelative[0].vy = 5;
	PLAYER_ATTACKTWICE[0].Erelative[1].vx = -60; PLAYER_ATTACKTWICE[0].Erelative[1].vy = -55;

	PLAYER_ATTACKTWICE[1].LoadAnimation(_T("img/hollow knight/AttackR/Attack/2/%d.png"), 5, PLAYER_ATTACKTWICE0);
	PLAYER_ATTACKTWICE[1].LoadEffect(_T("img/hollow knight/AttackR/Attack/2/0%d.png"), 2, 3, 2);
	for (int i = 0; i < 5; i++)
	{
		if (i == 1)
		{
			PLAYER_ATTACKTWICE[1].frameinterval[i] = 1; PLAYER_ATTACKTWICE[1].tmp_interval[i] = PLAYER_ATTACKTWICE[1].frameinterval[i];
		}
		PLAYER_ATTACKTWICE[1].frameinterval[i] = 2; PLAYER_ATTACKTWICE[1].tmp_interval[i] = PLAYER_ATTACKTWICE[1].frameinterval[i];
		if (i == 3) { PLAYER_ATTACKTWICE[1].dmove[i][0] = -2; }
	}PLAYER_ATTACKTWICE[1].CD = 45; PLAYER_ATTACKTWICE[1].canloop = false;
	PLAYER_ATTACKTWICE[1].Erelative[0].vx = 5; PLAYER_ATTACKTWICE[1].Erelative[0].vy = -5;
	PLAYER_ATTACKTWICE[1].Erelative[1].vx = -60; PLAYER_ATTACKTWICE[1].Erelative[1].vy = -65;


	PLAYER_UPATTACK[0].LoadAnimation(_T("img/hollow knight/Attack/AttackUp/%d.PNG"), 5, PLAYER_UPATTACK0);
	PLAYER_UPATTACK[0].LoadEffect(_T("img/hollow knight/Attack/AttackUp/0%d.png"), 2, 4, 2);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_UPATTACK[0].frameinterval[i] = 2;
		PLAYER_UPATTACK[0].tmp_interval[i] = PLAYER_UPATTACK[0].frameinterval[i];
	}PLAYER_UPATTACK[0].CD = 20;
	PLAYER_UPATTACK[0].Erelative[0].vx = -50; PLAYER_UPATTACK[0].Erelative[0].vy = -145;
	PLAYER_UPATTACK[0].Erelative[1].vx = 10; PLAYER_UPATTACK[0].Erelative[1].vy = -95;

	PLAYER_UPATTACK[1].LoadAnimation(_T("img/hollow knight/AttackR/AttackUp/%d.PNG"), 5, PLAYER_UPATTACK0);
	PLAYER_UPATTACK[1].LoadEffect(_T("img/hollow knight/AttackR/AttackUp/0%d.png"), 2, 4, 2);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_UPATTACK[1].frameinterval[i] = 2;
		PLAYER_UPATTACK[1].tmp_interval[i] = PLAYER_UPATTACK[1].frameinterval[i];
	}PLAYER_UPATTACK[1].CD = 20;
	PLAYER_UPATTACK[1].Erelative[0].vx = -50; PLAYER_UPATTACK[1].Erelative[0].vy = -145;
	PLAYER_UPATTACK[1].Erelative[1].vx = -110; PLAYER_UPATTACK[1].Erelative[1].vy = -95;


	PLAYER_JUMPSTART[0].LoadAnimation(_T("img/hollow knight/Jump/Start/%d.PNG"), 9, PLAYER_JUMPSTART0);
	for (int i = 0; i < 9; i++)
	{
		PLAYER_JUMPSTART[0].frameinterval[i] = 2;
		PLAYER_JUMPSTART[0].tmp_interval[i] = PLAYER_JUMPSTART[0].frameinterval[i];
	}PLAYER_JUMPSTART[0].canloop = false;

	PLAYER_JUMPSTART[1].LoadAnimation(_T("img/hollow knight/JumpR/Start/%d.PNG"), 9, PLAYER_JUMPSTART0);
	for (int i = 0; i < 9; i++)
	{
		PLAYER_JUMPSTART[1].frameinterval[i] = 2;
		PLAYER_JUMPSTART[1].tmp_interval[i] = PLAYER_JUMPSTART[1].frameinterval[i];
	}PLAYER_JUMPSTART[1].canloop = false;


	PLAYER_JUMPLOOP[0].LoadAnimation(_T("img/hollow knight/Jump/Loop/%d.PNG"), 3, PLAYER_JUMPLOOP0);
	for (int i = 0; i < 3; i++)
	{
		PLAYER_JUMPLOOP[0].frameinterval[i] = 2;
		PLAYER_JUMPLOOP[0].tmp_interval[i] = PLAYER_JUMPLOOP[0].frameinterval[i];
	}PLAYER_JUMPLOOP[0].canloop = true; PLAYER_JUMPLOOP[0].loop_index = 0;

	PLAYER_JUMPLOOP[1].LoadAnimation(_T("img/hollow knight/JumpR/Loop/%d.PNG"), 3, PLAYER_JUMPLOOP0);
	for (int i = 0; i < 3; i++)
	{
		PLAYER_JUMPLOOP[1].frameinterval[i] = 2;
		PLAYER_JUMPLOOP[1].tmp_interval[i] = PLAYER_JUMPLOOP[1].frameinterval[i];
	}PLAYER_JUMPLOOP[1].canloop = true; PLAYER_JUMPLOOP[1].loop_index = 0;
	/*PLAYER_JUMPLAND.LoadAnimation(_T("img/hollow knight/Jump/Land/%d.PNG"), 3, "PLAYER_JUMPLAND");
	for (int i = 0; i < 3; i++)
	{
		PLAYER_JUMPLAND.frameinterval[i] = 20;
	}*/

	PLAYER_DOWNATTACK[0].LoadAnimation(_T("img/hollow knight/Attack/AttackDown/%d.PNG"), 5, PLAYER_DOWNATTACK0);
	PLAYER_DOWNATTACK[1].LoadAnimation(_T("img/hollow knight/AttackR/AttackDown/%d.PNG"), 5, PLAYER_DOWNATTACK0);
	PLAYER_DOWNATTACK[0].LoadEffect(_T("img/hollow knight/Attack/AttackDown/0%d.PNG"), 2, 4, 2);
	PLAYER_DOWNATTACK[1].LoadEffect(_T("img/hollow knight/AttackR/AttackDown/0%d.PNG"), 2, 4, 2);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_DOWNATTACK[0].frameinterval[i] = 2;
	}PLAYER_DOWNATTACK[0].tmp_interval = PLAYER_DOWNATTACK[0].frameinterval;
	PLAYER_DOWNATTACK[1].tmp_interval = PLAYER_DOWNATTACK[0].frameinterval;
	PLAYER_DOWNATTACK[1].frameinterval = PLAYER_DOWNATTACK[0].frameinterval;
	PLAYER_DOWNATTACK[0].CD = 20; PLAYER_DOWNATTACK[1].CD = 20;
	PLAYER_DOWNATTACK[0].Erelative[0].vx = -35; PLAYER_DOWNATTACK[0].Erelative[1].vx = 5;
	PLAYER_DOWNATTACK[1].Erelative[1].vx = -50;


	PLAYER_DAMAGE[0].LoadAnimation(_T("img/hollow knight/Damage/%d.PNG"), 6, PLAYER_DAMAGE0);
	PLAYER_DAMAGE[1].LoadAnimation(_T("img/hollow knight/DamageR/%d.PNG"), 6, PLAYER_DAMAGE0);
	PLAYER_DAMAGE[0].LoadEffect(_T("img/hollow knight/Damage/0%d.PNG"), 0, 6, 3);
	PLAYER_DAMAGE[1].LoadEffect(_T("img/hollow knight/Damage/0%d.PNG"), 0, 6, 3);
	for (int i = 0; i < 3; i++)
	{
		*PLAYER_DAMAGE[0].Effects[i] = RotateImage_Alpha(PLAYER_DAMAGE[0].Effects[i], -3.1415926 / 6);
		*PLAYER_DAMAGE[1].Effects[i] = RotateImage_Alpha(PLAYER_DAMAGE[1].Effects[i], 3.1415926 / 6);
		PLAYER_DAMAGE[0].Erelative[i].vx = -290; PLAYER_DAMAGE[0].Erelative[i].vy = -150;
		PLAYER_DAMAGE[1].Erelative[i].vx = -290; PLAYER_DAMAGE[1].Erelative[i].vy = -150;
	}
	for (int i = 0; i < 6; i++)
	{
		PLAYER_DAMAGE[0].frameinterval[i] = 4;
	}
	PLAYER_DAMAGE[0].tmp_interval = PLAYER_DAMAGE[0].frameinterval;
	PLAYER_DAMAGE[1].tmp_interval = PLAYER_DAMAGE[0].frameinterval;
	PLAYER_DAMAGE[1].frameinterval = PLAYER_DAMAGE[0].frameinterval;
	PLAYER_DAMAGE[0].CD = 45; PLAYER_DAMAGE[1].CD = 45;



	PLAYER_ATTACK[0].LoadHitEffect(_T("img/hollow knight/AttackEffect/Attack/0%d.png"), 0, 4, 5);
	PLAYER_ATTACK[1].LoadHitEffect(_T("img/hollow knight/AttackEffect/AttackR/0%d.png"), 0, 4, 5);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_ATTACK[0].HitErelative[i].vx = -340; PLAYER_ATTACK[0].HitErelative[i].vy = -80;
		PLAYER_ATTACK[1].HitErelative[i].vx = -60; PLAYER_ATTACK[1].HitErelative[i].vy = -80;
	}

	PLAYER_ATTACKTWICE[0].LoadHitEffect(_T("img/hollow knight/AttackEffect/Attack/0%d.png"), 0, 4, 5);
	PLAYER_ATTACKTWICE[1].LoadHitEffect(_T("img/hollow knight/AttackEffect/AttackR/0%d.png"), 0, 4, 5);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_ATTACKTWICE[0].HitErelative[i].vx = -340; PLAYER_ATTACKTWICE[0].HitErelative[i].vy = -80;
		PLAYER_ATTACKTWICE[1].HitErelative[i].vx = -60; PLAYER_ATTACKTWICE[1].HitErelative[i].vy = -80;
	}
	PLAYER_DOWNATTACK[0].LoadHitEffect(_T("img/hollow knight/AttackEffect/Attack/0%d.png"), 0, 4, 5);
	PLAYER_DOWNATTACK[1].LoadHitEffect(_T("img/hollow knight/AttackEffect/AttackR/0%d.png"), 0, 4, 5);
	for (int i = 0; i < 5; i++)
	{
		PLAYER_DOWNATTACK[0].HitErelative[i].vx = -240; PLAYER_DOWNATTACK[0].HitErelative[i].vy = 70;
		PLAYER_DOWNATTACK[1].HitErelative[i].vx = -200; PLAYER_DOWNATTACK[1].HitErelative[i].vy = 70;
	}


	PLAYER_DEATH.LoadAnimation(_T("img/hollow knight/Death/%d.png"), 10, PLAYER_DEATH0);




	//B_TEST.LoadAnimation(_T("img/hollow knight/test.png"), 1, "B_TEST");
	//CurrentEnemyAnimation = B_TEST;
	B_WALK[0].LoadAnimation(_T("img/hollow knight/Boss/Walk/%d.png"), 11, B_WALK0);
	B_WALK[1].LoadAnimation(_T("img/hollow knight/Boss/WalkR/%d.png"), 11, B_WALK0);
	for (int i = 0; i < 11; i++)
	{
		B_WALK[0].frameinterval[i] = 2; B_WALK[0].dmove[i][0] = -12;
		B_WALK[1].dmove[i][0] = 12;
	}B_WALK[0].tmp_interval = B_WALK[0].frameinterval; B_WALK[1].tmp_interval = B_WALK[0].tmp_interval;
	B_WALK[1].frameinterval = B_WALK[0].frameinterval; //B_WALK[0].CD = 60; B_WALK[1].CD = 60;



	B_JUMPDASH[0].LoadAnimation(_T("img/hollow knight/Boss/JumpDash/%d.png"), 29, B_JUMPDASH0);
	for (int i = 0; i < 29; i++)
	{
		B_JUMPDASH[0].frameinterval[i] = 1;
		if (i >= 0 && i <= 3) { B_JUMPDASH[0].frameinterval[i] = 2; }
		if (i >= 13 && i <= 21) { B_JUMPDASH[0].frameinterval[i] = 3; }
	}B_JUMPDASH[0].frameinterval[22] = 20; B_JUMPDASH[0].tmp_interval = B_JUMPDASH[0].frameinterval;
	//B_JUMPDASH[0].CD = 120; 
	B_JUMPDASH[0].canloop = false;

	B_JUMPDASH[1].LoadAnimation(_T("img/hollow knight/Boss/JumpDashR/%d.png"), 29, B_JUMPDASH0);
	for (int i = 0; i < 29; i++)
	{
		B_JUMPDASH[1].frameinterval[i] = 1;
		if (i >= 0 && i <= 3) { B_JUMPDASH[1].frameinterval[i] = 2; }
		if (i >= 13 && i <= 21) { B_JUMPDASH[1].frameinterval[i] = 2; }
	}B_JUMPDASH[1].frameinterval[22] = 15;
	B_JUMPDASH[1].tmp_interval = B_JUMPDASH[1].frameinterval;
	//B_JUMPDASH[1].CD = 120; 
	B_JUMPDASH[1].canloop = false;


	B_IDLE[0].LoadAnimation(_T("img/hollow knight/Boss/Idle/%d.png"), 2, B_IDLEL0);
	B_IDLE[1].LoadAnimation(_T("img/hollow knight/Boss/IdleR/%d.png"), 2, B_IDLER0);
	for (int i = 0; i < 2; i++)
	{
		B_IDLE[0].frameinterval[i] = 4; B_IDLE[1].frameinterval[i] = 4;
		B_IDLE[0].loop_index = 0; B_IDLE[1].loop_index = 0;
	}B_IDLE[0].tmp_interval = B_IDLE[0].frameinterval; B_IDLE[1].tmp_interval = B_IDLE[1].frameinterval;
	B_IDLE[0].canloop = true; B_IDLE[1].canloop = true;


	B_LAND[0].LoadAnimation(_T("img/hollow knight/Boss/Land/%d.png"), 6, B_LAND0);
	B_LAND[1].LoadAnimation(_T("img/hollow knight/Boss/LandR/%d.png"), 6, B_LAND0);
	for (int i = 0; i < 6; i++)
	{
		B_LAND[0].frameinterval[i] = 2; B_LAND[1].frameinterval[i] = 2;
	}
	B_LAND[0].tmp_interval = B_LAND[0].frameinterval; B_LAND[1].tmp_interval = B_LAND[1].frameinterval;


	B_JUMP[0].LoadAnimation(_T("img/hollow knight/Boss/Jump/%d.png"), 29, B_JUMP0);
	B_JUMP[1].LoadAnimation(_T("img/hollow knight/Boss/JumpR/%d.png"), 29, B_JUMP0);
	for (int i = 0; i < 29; i++)
	{
		B_JUMP[0].frameinterval[i] = 2;
		if (i >= 13 && i <= 17) { B_JUMP[0].frameinterval[i] = 1; }
	}
	//B_JUMP[0].CD = 120; B_JUMP[1].CD = 120;
	B_JUMP[0].tmp_interval = B_JUMP[0].frameinterval; B_JUMP[1].tmp_interval = B_JUMP[0].frameinterval;
	B_JUMP[1].frameinterval = B_JUMP[0].frameinterval;

	B_DASH[0].LoadAnimation(_T("img/hollow knight/Boss/Dash/%d.png"), 12, B_DASH0);
	B_DASH[1].LoadAnimation(_T("img/hollow knight/Boss/DashR/%d.png"), 12, B_DASH0);
	B_DASH[0].LoadEffect(_T("img/hollow knight/Boss/Dash/0%d.png"), 9, 11, 4);
	B_DASH[1].LoadEffect(_T("img/hollow knight/Boss/DashR/0%d.png"), 9, 11, 4);
	for (int i = 0; i < 4; i++)
	{
		B_DASH[0].Erelative[i].vx = 140; B_DASH[1].Erelative[i].vx = -250;
		B_DASH[0].Erelative[i].vy = -20; B_DASH[1].Erelative[i].vy = -20;
		if (i == 0)
		{
			B_DASH[0].Erelative[i].vx = 140; B_DASH[1].Erelative[i].vx = -120;
			B_DASH[0].Erelative[i].vy = 90; B_DASH[1].Erelative[i].vy = 90;
		}
		if (i == 2 || i == 3)
		{
			B_DASH[0].Erelative[i].vx = 200;
		}
	}
	for (int i = 0; i < 12; i++)
	{
		//B_DASH[0].dmove[i][1] = 30; B_DASH[1].dmove[i][1] = 30;
		B_DASH[0].frameinterval[i] = 2; if (i == 11)
		{
			B_DASH[0].frameinterval[i] = 20; B_DASH[0].dmove[i][0] = -35;
			B_DASH[1].dmove[i][0] = 35;
		}
	}B_DASH[0].tmp_interval = B_DASH[0].frameinterval; B_DASH[1].tmp_interval = B_DASH[0].tmp_interval;
	B_DASH[1].frameinterval = B_DASH[0].frameinterval;// B_DASH[0].CD = 300; B_DASH[1].CD = 300;

	B_JUMPFINAL[0].LoadAnimation(_T("img/hollow knight/Boss/JumpFinal/%d.png"), 45, B_JUMPFINAL0);
	B_JUMPFINAL[1].LoadAnimation(_T("img/hollow knight/Boss/JumpFinalR/%d.png"), 45, B_JUMPFINAL0);
	B_JUMPFINAL[0].LoadEffect(_T("img/hollow knight/Boss/JumpFinal/Effects/0%d.png"), 20, 38, 18);
	B_JUMPFINAL[1].LoadEffect(_T("img/hollow knight/Boss/JumpFinalR/Effects/0%d.png"), 20, 38, 18);
	for (int i = 0; i < 45; i++)
	{
		B_JUMPFINAL[1].frameinterval[i] = 2; B_JUMPFINAL[0].frameinterval[i] = 2;
		if (i >= 13 && i <= 17) { B_JUMPFINAL[1].frameinterval[i] = 1; B_JUMPFINAL[0].frameinterval[i] = 1; }
	}B_JUMPFINAL[1].CD = 30; B_JUMPFINAL[1].tmp_interval = B_JUMPFINAL[1].frameinterval;
	B_JUMPFINAL[0].tmp_interval = B_JUMPFINAL[1].frameinterval; B_JUMPFINAL[0].frameinterval = B_JUMPFINAL[1].frameinterval;
	B_JUMPFINAL[0].CD = 30;
	for (int i = 0; i < 18; i++)
	{
		B_JUMPFINAL[0].Erelative[i].vx = -80; B_JUMPFINAL[0].Erelative[i].vy = -50;
		B_JUMPFINAL[1].Erelative[i].vx = -80; B_JUMPFINAL[1].Erelative[i].vy = -50;
	}

	loadimage(&Background, _T("img/hollow knight/background.png"), 1440, 900);






	BLOOD.LoadAnimation(_T("img/hollow knight/UI/Blood/%d.png"), 6, BLOOD0);
	for (int i = 0; i < 6; i++)
	{
		BLOOD.frameinterval[i] = 3; if (i == 0)
		{
			BLOOD.frameinterval[i] = 120;
		}
	}BLOOD.tmp_interval = BLOOD.frameinterval;
	BLOOD.canloop = true; BLOOD.loop_index = 0;
	loadimage(&EMPTYBLOOD, _T("img/hollow knight/UI/Blood/empty.png"));


	BLOOD_DAMAGE.LoadAnimation(_T("img/hollow knight/UI/Blood/Damage/%d.png"), 6, BLOOD_DAMAGE0);
	for (int i = 0; i < 6; i++)
	{
		BLOOD_DAMAGE.frameinterval[i] = 3; if (i == 0) {
			BLOOD_DAMAGE.frameinterval[i] = 15;
		}
	}BLOOD_DAMAGE.tmp_interval = BLOOD_DAMAGE.frameinterval; BLOOD_DAMAGE.canloop = false;
	BLOOD_DAMAGE.dmove[0][0] = -7; BLOOD_DAMAGE.dmove[0][1] = -27;
	BLOOD_DAMAGE.dmove[1][0] = -3; BLOOD_DAMAGE.dmove[1][1] = -27;
	BLOOD_DAMAGE.dmove[2][0] = -4; BLOOD_DAMAGE.dmove[2][1] = -10;
	BLOOD_DAMAGE.dmove[3][0] = -2;	BLOOD_DAMAGE.dmove[4][0] = -3;


	loadimage(&SOUL, _T("img/hollow knight/UI/Blood/00.png"));
	initgraph(1440, 900);
	while (true)
	{
		HWND hwnd = GetHWnd();
		SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		if (scenemanager==0)
		{
			ExMessage msg;
			mciSendString(_T("open UI/Title.mp3 alias Title"), NULL, 0, NULL);
			mciSendString(_T("open UI/Confirm.mp3 alias Confirm"), NULL, 0, NULL);
			loadimage(&OPENGROUND, _T("UI/openground.png"), 1440, 900); loadimage(&TITLE, _T("UI/title_chinese.png"), 696, 340);//696 340
			loadimage(&LOGO, _T("UI/Team Cherry Logo_large.png"), 118, 97);//48,121
			loadimage(&PRESS_TO_START, _T("UI/press_to_start.png"));
			ARROR[0].LoadAnimation(_T("UI/Arror/%d.png"), 10, ARROR0);
			ARROR[1].LoadAnimation(_T("UI/ArrorR/%d.png"), 10, ARROR0);
			//for (int i = 0; i < 10; i++) { ARROR[0].frameinterval[i] = 2; }
			
			BeginBatchDraw();
			mciSendString(_T("play Title repeat from 0"), NULL, 0, NULL);
			while (1)
			{
				cleardevice();
				putimage(0, 0, &OPENGROUND);	putimage_1(720 - (TITLE.getwidth()) / 2, 100, &TITLE); putimage_1(1180, 720, &LOGO);
				putimage_1(720 - (PRESS_TO_START.getwidth()) / 2, 720, &PRESS_TO_START);
				FlushBatchDraw();
				if (peekmessage(&msg, EX_KEY))
				{
					break;
				}
			}
			mciSendString(_T("play Confirm from 0"), NULL, 0, NULL);
			for (int i = 0; i < 10; i++)
			{

				DWORD start = GetTickCount();
				putimage(0, 0, &OPENGROUND);	putimage_1(720 - (TITLE.getwidth()) / 2, 100, &TITLE); putimage_1(1180, 720, &LOGO);
				putimage_1(720 - (PRESS_TO_START.getwidth()) / 2, 720, &PRESS_TO_START);
				putimage_1(720 - PRESS_TO_START.getwidth() / 2 - ARROR[0].frame_list[0]->getwidth(), 705, ARROR[0].frame_list[i]);
				putimage_1(720 + PRESS_TO_START.getwidth() / 2, 705, ARROR[1].frame_list[i]); FlushBatchDraw(); DWORD end = GetTickCount();
				if (end - start < 1000 / 30)
				{
					Sleep(1000 / 30 - end + start);
				}
			}
			Sleep(300);
			EndBatchDraw();
			mciSendString(_T("stop Title"), NULL, 0, NULL); scenemanager = 1;
		}
		if (scenemanager==1)
		{
			Player.HP = 7; testenemy.HP =50;
			int MusicCD = 90;
			int PublicCD = 20; int index;
			nullanimation.animationname = NU; nullanimation.Effects.resize(0); CurrentPlayerAnimation = nullanimation; CurrentPlayerLoopAnimation = nullanimation;
			CurrentEnemyAnimation = nullanimation;
			testenemy.gravity = 0;

			

			//	Animation PLAYER_STAND;
			//	PLAYER_STAND.LoadAnimation(_T("img/test1right-%d.png"), 8,"PLAYER_STAND");
			//	Player_Ani_Vector.push_back(PLAYER_STAND);
			Actor lastenemy;
			mciSendString(_T("play bgm repeat from 0"), NULL, 0, NULL);
			mciSendString(_T("play open from 0"), NULL, 0, NULL);
			while (Player.HP&&testenemy.HP)
			{

				lastenemy = testenemy;
				/*if (get_direction(texstenemy.position, Player.position).vx >= 0) { testenemy.Facing_Right = true; }
				else { testenemy.Facing_Right = false; }*/

				DWORD Start_Time = GetTickCount();
				if (Player.UnHurtableTime > 0) { Player.UnHurtableTime -= 1; }
				if (PublicCD > 0) { PublicCD -= 1; }
				if (B_WALK[0].CD > 0) { B_WALK[0].CD -= 1; }
				if (B_JUMPDASH[0].CD > 0) { B_JUMPDASH[0].CD -= 1; }
				if (B_JUMP[0].CD > 0) { B_JUMP[0].CD -= 1; }
				if (B_DASH[0].CD > 0) { B_DASH[0].CD -= 1; }
				if (testenemy.UnHurtableTime > 0) { testenemy.UnHurtableTime -= 1; }
				if (PLAYER_DASH[0].CD > 0) { PLAYER_DASH[0].CD -= 1; }
				if (PLAYER_ATTACK[0].CD > 0) { PLAYER_ATTACK[0].CD -= 1; }
				if (PLAYER_UPATTACK[0].CD > 0) { PLAYER_UPATTACK[0].CD -= 1; }
				if (PLAYER_ATTACKTWICE[0].CD > 0) { PLAYER_ATTACKTWICE[0].CD -= 1; }
				if (PLAYER_DOWNATTACK[0].CD > 0) { PLAYER_DOWNATTACK[0].CD -= 1; }
				if (PLAYER_DAMAGE[0].CD > 0) { PLAYER_DAMAGE[0].CD -= 1; }

				if (B_JUMPFINAL[0].CD > 0) { B_JUMPFINAL[0].CD -= 1; }
				if (MusicCD > 0) { MusicCD -= 1; }

				//或许需要存储上一帧的玩家坐标数据！
			//	Actor LastFrame = Player;
				//gravity_collide(Player);
				int operation = Player.messagedealer();//zcccccccccccccccccccccccccccccccccc
				if (CurrentPlayerAnimation.animationname == PLAYER_DAMAGE0 && CurrentPlayerAnimation.current_frameidx <= 3)
				{
				}
				else {
					if (operation == 0)
					{
						if (CurrentPlayerLoopAnimation.animationname != PLAYER_IDLE0)
						{
							CurrentPlayerLoopAnimation = PLAYER_IDLE[Player.Facing_Right];
						}
					}
					else if (operation == 1)
					{
						if (CurrentPlayerLoopAnimation.animationname != PLAYER_WALKR0 && CurrentPlayerLoopAnimation.animationname != PLAYER_WALKL0 ||
							CurrentPlayerLoopAnimation.animationname == PLAYER_WALKR0 && Player.Facing_Right == 0 ||
							CurrentPlayerLoopAnimation.animationname == PLAYER_WALKL0 && Player.Facing_Right == 1)
						{
							CurrentPlayerLoopAnimation = PLAYER_WALK[Player.Facing_Right];
						}
					}

					bool isok = CurrentPlayerAnimation.animationname != PLAYER_ATTACK0 && CurrentPlayerAnimation.animationname != PLAYER_DOWNATTACK0 &&
						CurrentPlayerAnimation.animationname != PLAYER_DASH0 && CurrentPlayerAnimation.animationname != PLAYER_UPATTACK0 && CurrentPlayerAnimation.animationname != PLAYER_DOWNATTACK0;
					if (Player.speed.vy == 0 && (CurrentPlayerAnimation.animationname == PLAYER_ATTACK0 || CurrentPlayerAnimation.animationname == PLAYER_UPATTACK0
						|| CurrentPlayerAnimation.animationname == PLAYER_ATTACKTWICE0 || CurrentPlayerAnimation.animationname == PLAYER_DOWNATTACK0))
					{
						Player.speed.vx = 0;//地面行走时攻击则停止行走
					}
					if (Player.speed.vy > 0 && isok)//y速度大于0且没有其他操作则自由下落
					{
						CurrentPlayerLoopAnimation = PLAYER_JUMPLOOP[Player.Facing_Right];
					}

					if (operation == 2)
					{
						if (Player.position.y <= 680 && isok && PLAYER_ATTACK[0].CD == 0 || CurrentPlayerAnimation.animationname == NU && PLAYER_ATTACK[0].CD == 0)
						{
							CurrentPlayerAnimation = PLAYER_ATTACK[Player.Facing_Right]; PLAYER_ATTACK[0].CD = 30;
							PLAYER_ATTACKTWICE[0].CD = 45;
						}
						else
						{
							if (PLAYER_ATTACKTWICE[0].CD > 0 && PLAYER_ATTACKTWICE[0].CD <= 30)
							{
								CurrentPlayerAnimation = PLAYER_ATTACKTWICE[Player.Facing_Right]; PLAYER_ATTACKTWICE[0].CD = 0;
							}
						}
					}
					else if (operation == 4 && PLAYER_DASH[0].CD == 0 || operation == 3 && PLAYER_DASH[0].CD == 0)//////////cccccccccc
					{
						if (Player.position.y < 680 && isok || CurrentPlayerAnimation.animationname == NU)
						{
							CurrentPlayerAnimation = PLAYER_DASH[Player.Facing_Right]; PLAYER_DASH[0].CD = 30;
						}
					}
					else if (operation == 3 && PLAYER_DASH[0].CD > 0 && CurrentPlayerAnimation.animationname != PLAYER_DASH0)
					{
						if (CurrentPlayerLoopAnimation.animationname != PLAYER_WALKR0 && CurrentPlayerLoopAnimation.animationname != PLAYER_WALKL0 ||
							CurrentPlayerLoopAnimation.animationname == PLAYER_WALKR0 && Player.Facing_Right == 0 ||
							CurrentPlayerLoopAnimation.animationname == PLAYER_WALKL0 && Player.Facing_Right == 1)
						{
							CurrentPlayerLoopAnimation = PLAYER_WALK[Player.Facing_Right];
						}
						Player.speed.vx = 10 * (Player.Facing_Right * 2 - 1);
					}
					else if (operation == 5 && PLAYER_UPATTACK[0].CD == 0)
					{
						if (Player.position.y < 680 && isok || CurrentPlayerAnimation.animationname == NU)
						{
							CurrentPlayerAnimation = PLAYER_UPATTACK[Player.Facing_Right]; PLAYER_UPATTACK[0].CD = 20;
						}
					}
					else if (operation == 6)
					{
						if (CurrentPlayerAnimation.animationname == NU && Player.speed.vy == 0)
						{
							Player.speed.vy = -43;
							CurrentPlayerAnimation = PLAYER_JUMPSTART[Player.Facing_Right];
						}
					}
					else if (operation == 7 && PLAYER_DOWNATTACK[0].CD == 0)
					{
						if (Player.position.y <= 680 && isok || CurrentPlayerAnimation.animationname == NU)
						{
							CurrentPlayerAnimation = PLAYER_DOWNATTACK[Player.Facing_Right]; PLAYER_DOWNATTACK[0].CD = 20;
						}
					}
					if (CurrentPlayerAnimation.animationname == PLAYER_DASH0)
					{
						Player.speed.vx = 0;
					}
				}

				Player.position.x += Player.speed.vx; Player.position.y += Player.speed.vy;
				/*	if (Player.position.y == 500 && LastFrame.position.y < 500)
					{
						CurrentPlayerAnimation = PLAYER_JUMPLAND;
					}	*/


					//boss


				if (PublicCD == 0)
				{
					srand((unsigned)time(NULL));

					index = rand() % 4;//0小跳 1大跳 2大冲 3跳脸开大
					while (CurrentEnemyAnimation.animationname == NU || CurrentEnemyAnimation.animationname == B_IDLEL0 || CurrentEnemyAnimation.animationname == B_IDLER0)
					{
						if (B_JUMP[0].CD > 0 && B_JUMPDASH[0].CD > 0 && B_DASH[0].CD > 0 && B_WALK[0].CD > 0)
						{
							break;
						}
						if (abs(get_direction(testenemy.position, Player.position).vx) < 150 && B_WALK[0].CD == 0)
						{
							CurrentEnemyAnimation = B_WALK[get_direction(testenemy.position, Player.position).vx > 0 ? 0 : 1]; B_WALK[0].CD = 75;
							PublicCD = 30; break;
						}
						else if (abs(get_direction(testenemy.position, Player.position).vx) > 500 && B_WALK[0].CD == 0)
						{
							CurrentEnemyAnimation = B_WALK[get_direction(testenemy.position, Player.position).vx > 0 ? 1 : 0]; B_WALK[0].CD = 75;
							PublicCD = 30; break;
						}
						index = (rand() % (3 + 1));
						if (index == 0 && B_JUMP[0].CD == 0)
						{
							testenemy.gravity = 3; testenemy.speed.vy = -60;
							normal_shootdirection = get_normal_direction(testenemy.position, Player.position);
							for (int i = 3; i < 29; i++)
							{
								B_JUMP[0].dmove[i][0] = 10 * normal_shootdirection.vx;
								B_JUMP[1].dmove[i][0] = 10 * normal_shootdirection.vx;
							}
							if (get_direction(testenemy.position, Player.position).vx >= 0) { testenemy.Facing_Right = true; }
							else { testenemy.Facing_Right = false; }
							CurrentEnemyAnimation = B_JUMP[testenemy.Facing_Right]; B_JUMP[0].CD = 60; PublicCD = 50; break;
						}
						else if (index == 1 && B_JUMPDASH[0].CD == 0)
						{
							testenemy.gravity = 0; testenemy.speed.vy = 0;
							shootdirection = get_direction(testenemy.position, Player.position); normal_shootdirection = get_normal_direction(testenemy.position, Player.position);
							for (int i = 3; i < 8; i++)
							{
								B_JUMPDASH[0].dmove[i][0] = 10 * normal_shootdirection.vx;
								B_JUMPDASH[1].dmove[i][0] = 10 * normal_shootdirection.vx;
								B_JUMPDASH[0].dmove[i][1] = -75; B_JUMPDASH[1].dmove[i][1] = -75;
							}
							B_JUMPDASH[0].dmove[22][1] = 30; B_JUMPDASH[1].dmove[22][1] = 30;
							B_JUMPDASH[0].dmove[22][0] = shootdirection.vx / 22;
							B_JUMPDASH[1].dmove[22][0] = shootdirection.vx / 22;
							if (get_direction(testenemy.position, Player.position).vx >= 0) { testenemy.Facing_Right = true; }
							else { testenemy.Facing_Right = false; }
							CurrentEnemyAnimation = B_JUMPDASH[testenemy.Facing_Right]; B_JUMPDASH[0].CD = 120; PublicCD = 50;
						}
						else if (index == 2 && B_DASH[0].CD == 0)
						{
							if (get_direction(testenemy.position, Player.position).vx >= 0) { testenemy.Facing_Right = true; }
							else { testenemy.Facing_Right = false; }
							CurrentEnemyAnimation = B_DASH[testenemy.Facing_Right]; B_DASH[0].CD = 50; PublicCD = 50; break;
						}
						else if (index == 3 && B_JUMPFINAL[0].CD == 0)
						{
							testenemy.gravity = 4; testenemy.speed.vy = -60;
							normal_shootdirection = get_normal_direction(testenemy.position, Player.position);
							if (get_direction(testenemy.position, Player.position).vx >= 0) { testenemy.Facing_Right = true; }
							else { testenemy.Facing_Right = false; }
							for (int i = 3; i < 15; i++)
							{
								B_JUMPFINAL[testenemy.Facing_Right].dmove[i][0] = 25 * normal_shootdirection.vx;
							}
							CurrentEnemyAnimation = B_JUMPFINAL[testenemy.Facing_Right]; B_JUMPFINAL[0].CD = 240; PublicCD = 120; break;
						}

					}

				}
				else if (CurrentEnemyAnimation.animationname == NU)
				{
					CurrentEnemyAnimation = B_IDLE[get_direction(testenemy.position, Player.position).vx > -0 ? 1 : 0];

				}
				if (CurrentPlayerAnimation.animationname != NU)
				{
					CurrentPlayerLoopAnimation = nullanimation;
				}
				if (attack_judger(Player, testenemy))
				{
					testenemy.UnHurtableTime = 12; Player.position.x += -(2 * Player.Facing_Right - 1) * 60;
					//testenemy.position.x += (2 * Player.Facing_Right - 1) * 80; //testenemy.gravity = 4;
					if (CurrentPlayerAnimation.animationname == PLAYER_DOWNATTACK0)
					{
						Player.speed.vy = -40;
					}
					mciSendString(_T("play attackhit from 0"), NULL, 0, NULL);
					if (CurrentPlayerAnimation.animationname == PLAYER_ATTACK0 || CurrentPlayerAnimation.animationname == PLAYER_ATTACKTWICE0 || CurrentPlayerAnimation.animationname == PLAYER_DOWNATTACK0)
					{
						CurrentPlayerAnimation.PlayHitAnimation = 1;
					}
					testenemy.HP -= 1;
					
				}
				if (attacked_judger(Player, testenemy))
				{
					Player.UnHurtableTime = 45;
					if (CurrentPlayerAnimation.animationname == PLAYER_DASH0 || CurrentEnemyAnimation.animationname == B_DASH0
						|| CurrentEnemyAnimation.animationname == B_JUMP0 || CurrentEnemyAnimation.animationname == B_JUMPDASH0)
					{
						CurrentPlayerAnimation = PLAYER_DAMAGE[get_direction(testenemy.position, Player.position).vx < 60 ? 0 : 1];
					}
					else { CurrentPlayerAnimation = PLAYER_DAMAGE[get_direction(testenemy.position, Player.position).vx > 0 ? 1 : 0]; }
					Player.speed.vy = -25; Player.HP -= 1;// testenemy.speed.vy = -15;
					//cout << "000" << endl;
				}
				testenemy.position.x += testenemy.speed.vx; testenemy.position.y += testenemy.speed.vy;
				MusicPlayer(CurrentPlayerAnimation, CurrentEnemyAnimation, MusicCD);

				BeginBatchDraw();
				//先玩家绘图
				putimage(0, 0, &Background);
				putimage_1(210, 140, &COIN);
				putimage_1(270, 145, &MONEY);
				gravity_collide(Player, testenemy);
				if (testenemy.position.x < 90)
				{
					testenemy.position.x = 90;
				}
				if (testenemy.position.x + CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth() >= 1360)
				{
					testenemy.position.x = 1360 - CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth();
				}

				drawplayer(testenemy, CurrentEnemyAnimation);


				putimage_1(53, 40, &SOUL);

				for (int i = 0; i < 7; i++)
				{
					putimage_1(200 + 65 * i, 70, &EMPTYBLOOD);
				}

				for (int i = 0; i < Player.HP; i++)
				{
					putimage_1(200 + 65 * i, 70, BLOOD.frame_list[BLOOD.current_frameidx]);
				}
				BLOOD.tmp_interval[BLOOD.current_frameidx] -= 1; if (BLOOD.tmp_interval[BLOOD.current_frameidx] == 0)
				{
					BLOOD.current_frameidx += 1;
					if (BLOOD.current_frameidx == BLOOD.frame_num)
					{
						BLOOD.current_frameidx = 0; BLOOD.tmp_interval = BLOOD.frameinterval;
					}

				}
				if (BLOOD_DAMAGE.current_frameidx == 0 && BLOOD_DAMAGE.tmp_interval[0] < 15 ||
					BLOOD_DAMAGE.current_frameidx>0 || CurrentPlayerAnimation.animationname == PLAYER_DAMAGE0 && CurrentPlayerAnimation.current_frameidx == 0)
				{
					drawUI(BLOOD_DAMAGE, 200 + 65 * Player.HP, 70);
				}






				if (CurrentPlayerAnimation.animationname != NU)
				{


					if (Player.position.x <= 140)
					{
						Player.position.x = 140;
					}
					if (Player.position.y > 680)
					{
						Player.speed.vy = 0; Player.position.y = 680;
					}
					if (Player.position.x + CurrentPlayerAnimation.frame_list[CurrentPlayerAnimation.current_frameidx]->getwidth() >= 1320)
					{
						Player.position.x = 1320 - CurrentPlayerAnimation.frame_list[CurrentPlayerAnimation.current_frameidx]->getwidth();
					}

					drawplayer(Player, CurrentPlayerAnimation);
					if (CurrentPlayerAnimation.animationname == PLAYER_DAMAGE0 && CurrentPlayerAnimation.tmp_interval[0] == CurrentPlayerAnimation.frameinterval[0] - 2)
					{
						Sleep(300);
					}



					FlushBatchDraw(); DWORD End_Time = GetTickCount();
					//延时控制60帧
					if (End_Time - Start_Time < 1000 / 45)
					{
						Sleep(1000 / 45 - End_Time + Start_Time);
					}
					continue;
				}
				else
				{
					//	Player.position.x += Player.speed.vx; Player.position.y += Player.speed.vy;//更新坐标，此时未实现重力	


					if (Player.position.x <= 140)
					{
						Player.position.x = 140;

					}if (Player.position.y >= 680)
					{
						Player.position.y = 680;
					}
					if (Player.position.x + CurrentPlayerLoopAnimation.maxwidth >= 1320)
					{
						Player.position.x = 1320 - CurrentPlayerLoopAnimation.maxwidth;
					}
					if (CurrentPlayerLoopAnimation.animationname != NU)
					{
						drawplayer(Player, CurrentPlayerLoopAnimation);
					}
					FlushBatchDraw(); DWORD End_Time = GetTickCount();
					//延时控制60帧
					if (End_Time - Start_Time < 1000 / 45)
					{
						Sleep(1000 / 45 - End_Time + Start_Time);
					}continue;
				}



				FlushBatchDraw();

				DWORD End_Time = GetTickCount();
				//延时控制60帧
				if (End_Time - Start_Time < 1000 / 60)
				{
					Sleep(1000 / 60 - End_Time + Start_Time);
				}
			}


			if (Player.HP==0)
			{
				mciSendString(_T("play playerdamage from 0"), NULL, 0, NULL);
				for (int i = 0; i < 10; i++)
				{
					DWORD s1 = GetTickCount();
					BeginBatchDraw();
					putimage(0, 0, &Background); putimage_1(210, 140, &COIN); putimage_1(270, 145, &MONEY);
					gravity_collide(Player, testenemy);
					if (testenemy.position.x < 90)
					{
						testenemy.position.x = 90;
					}
					/*if (CurrentEnemyAnimation.animationname != NU && testenemy.position.x + CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth() >= 1360)
					{
						testenemy.position.x = 1360 - CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth();
					}
					drawplayer(testenemy, CurrentEnemyAnimation);*/
					putimage_1(testenemy.position.x, testenemy.position.y, B_IDLE[testenemy.Facing_Right].frame_list[0]);
					putimage_1(53, 40, &SOUL);

					for (int i = 0; i < 7; i++)
					{
						putimage_1(200 + 65 * i, 70, &EMPTYBLOOD);
					}
					putimage_1(Player.position.x, Player.position.y, PLAYER_DEATH.frame_list[i]);
					FlushBatchDraw();
					DWORD s2 = GetTickCount();
					if (i == 0)
					{
						Sleep(100);
					}if (i == 9)
					{
						Sleep(200);
					}
					if (s2 - s1 < 1000 / 15)
					{
						Sleep(1000 / 15 + s1 - s2);
					}
				}
				scenemanager = 2;
			}
			if (testenemy.HP==0)
			{
				mciSendString(_T("play hea from 0"), NULL, 0, NULL);
				CurrentEnemyAnimation = B_IDLE[testenemy.Facing_Right];
				for (int i = 0; i < 15; i++)
				{
					DWORD s3 = GetTickCount();
					BeginBatchDraw();
					putimage(0, 0, &Background); putimage_1(210, 140, &COIN); putimage_1(270, 145, &MONEY);
					putimage_1(53, 40, &SOUL);
					for (int i = 0; i < 7; i++)
					{
						putimage_1(200 + 65 * i, 70, &EMPTYBLOOD);
						for (int i = 0; i < Player.HP; i++)
						{
							putimage_1(200 + 65 * i, 70, BLOOD.frame_list[0]);
						}
					}
					gravity_collide(Player, testenemy);
					if (testenemy.position.x < 90)
					{
						testenemy.position.x = 90;
					}
					if (CurrentEnemyAnimation.animationname != NU && testenemy.position.x + CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth() >= 1360)
					{
						testenemy.position.x = 1360 - CurrentEnemyAnimation.frame_list[CurrentEnemyAnimation.current_frameidx]->getwidth();
					}
					drawplayer(testenemy, CurrentEnemyAnimation);
					putimage_1(Player.position.x, Player.position.y, PLAYER_IDLE[Player.Facing_Right].frame_list[0]);
					DWORD s4 = GetTickCount();
					if (s4-s3<1000/30)
					{
						Sleep(1000 / 30 + s3 - s4);
					}
				}
				Sleep(400);
				scenemanager = 2;
			}
			
		}
		if (scenemanager==2)
		{
			mciSendString(_T("stop bgm"), NULL, 0, NULL);
			BeginBatchDraw(); 
			cleardevice();
			putimage_1(720 - PLAYAGAIN.getwidth() / 2, 450 - PLAYAGAIN.getheight() / 2, &PLAYAGAIN); FlushBatchDraw();
			ExMessage control;
			while (true)
			{
				peekmessage(&control);
				if (control.lbutton==1&&control.x>780&&control.x<880&&control.y>420&&control.y<500)
				{
					
					scenemanager = 3; break;
				}
				if (control.lbutton==1&&control.x<660&&control.x>560 && control.y > 420 && control.y < 500)
				{
					mciSendString(_T("play Confirm from 0"), NULL, 0, NULL);
					scenemanager = 1; break;
				}	
			}
		}
		if (scenemanager==3)
		{
			break;
		}

	}
	


	

	//
	EndBatchDraw();
	return 0;
}