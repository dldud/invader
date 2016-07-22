#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>
#include <math.h>

#include "../engine/engine2d.h"
#include "../mapEditor/map.h"

#include "plane.h"
#include "bullet.h"
#include "alien.h"

struct timespec work_timer;
double acc_tick,last_tick;
int bLoop = 1;

_S_MAP_OBJECT gScreenBuf[2];

_S_MAP_OBJECT gPlayerModel;
_S_MAP_OBJECT gBulletModel;
_S_MAP_OBJECT gAlienModel;


_S_Plane gPlayerObject;
_S_BULLET_OBJECT gBulletObject[32];
_S_ALIEN_OBJECT gAlienObjects[8];

int main()
{

	for(int i=0;i<2;i++)
	{
		map_init(&gScreenBuf[i]);
		map_new(&gScreenBuf[i],35,35);
	}

	map_init(&gPlayerModel);
	map_load(&gPlayerModel,"airplane.dat");
	
	map_init(&gBulletModel);
	map_load(&gBulletModel,"bullet1.dat");
	
	map_init(&gAlienModel);
	map_load(&gAlienModel,"tank1.dat");
	
	Plane_init(&gPlayerObject,&gPlayerModel,17,10);
	//bullet_init(&gBulletModel,0,0,0,&gBulletModel);
		
	gPlayerObject.m_nFSM = 1;

	double TablePosition[] = {0,8,16,24};
	
/*	for(int i=0;i<2;i++)
	{
		_S_BULLET_OBJECT *pObj = &gBulletObject[i];
		bullet_init(pObj,&gBulletObject[i];
		pObj->m_nFSM = 1;
	}
*/
	for(int i=0;i<4;i++)
	{
		_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
		alien_init(pObj,&gAlienModel);
		pObj->m_fXpos = TablePosition[i];
		pObj->m_fYpos = 2;
		pObj->m_nFSM = 1;
	}
/*	set_conio_terminal_mode();
	acc_tick=last_tick=0;

	int targetx,targety;

	int fire_x, fire_y;
*/
	
	system("clear");
	
	set_conio_terminal_mode();
	acc_tick=last_tick=0;

	while(bLoop) {
		//타이밍처리 
		clock_gettime(CLOCK_MONOTONIC,&work_timer);
		double cur_tick = work_timer.tv_sec + 
			(double)(work_timer.tv_nsec * 1e-9);
		double delta_tick = cur_tick - last_tick;
		last_tick = cur_tick;
		//실시간 입력
		
/*		fire_x = gAlienObjects.m_nXpos;
		fire_y = gAlienObjects.m_nYpos;

		targetx = gPlayerObject.m_nXpos;
		targety = gPlayerObject.m_nypos;
*/	
		if(kbhit() != 0) {
			char ch = getch();
			if(ch == 'q') {
				bLoop = 0;
				puts("bye~ \r");
			}
/*			else if(ch == 'j') {
				double vx,vy,c;
				vx = targetx - fire_x;
				vy = targety - fire_y;
				c = sqrt(vx*vx+vy*vy);
				vx /= c;
				vy /= c;

				bullet_fire(&gBulletObject,fire_x,fire_y,10.0,vx,vy,10.0);
			
			}*/
		gPlayerObject.pfApply(&gPlayerObject,delta_tick,ch);
//		bullet_apply(&gBulletObject,delta_tick);
		}

		for(int i=0;i<4;i++ ) 
		{
			_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
			pObj->pfApply(pObj,delta_tick);

		}


		//타이밍 계산 
		acc_tick += delta_tick;
		if(acc_tick > 0.1) {
			gotoxy(0,0);
			map_drawTile(&gScreenBuf[0],0,0,&gScreenBuf[1]);
	//		bullet_draw(&gBulletObjects, &gScreenBuf[1]);
			gPlayerObject.pfDraw(&gPlayerObject,&gScreenBuf[1]);
				
			for(int i=0;i<4;i++ ) 
			{
				_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
				pObj->pfDraw(pObj,&gScreenBuf[1]);
			}

			map_dump(&gScreenBuf[1],Default_Tilepalete);
			acc_tick = 0;
		}

	}

	return 0;
}
