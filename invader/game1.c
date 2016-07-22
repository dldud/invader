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
_S_MAP_OBJECT gPlaneBulletModel;
_S_MAP_OBJECT gAlienModel;
_S_MAP_OBJECT gAlienBulletModel;

_S_Plane gPlayerObject;
_S_BULLET_OBJECT gAlienBulletObjects[32];
_S_ALIEN_OBJECT gAlienObjects[8];
_S_BULLET_OBJECT gPlaneBulletObjects[32];
int main()
{

	for(int i=0;i<2;i++)
	{
		map_init(&gScreenBuf[i]);
		map_new(&gScreenBuf[i],35,35);
	}

	map_init(&gPlayerModel);
	map_load(&gPlayerModel,"airplane.dat");

	map_init(&gPlaneBulletModel);
	map_load(&gPlaneBulletModel,"bullet.dat");

	map_init(&gAlienBulletModel);
	map_load(&gAlienBulletModel,"plasma.dat");

	map_init(&gAlienModel);
	map_load(&gAlienModel,"tank1.dat");

	Plane_init(&gPlayerObject,&gPlayerModel,17,30);

	gPlayerObject.m_nFSM = 1;

	for(int i=0;i<sizeof(gPlaneBulletObjects)/sizeof(_S_BULLET_OBJECT);i++){
		bullet_init(&gPlaneBulletObjects[i],0,0,0,&gPlaneBulletModel);
	}

	for(int i=0;i<sizeof(gAlienBulletObjects)/sizeof(_S_BULLET_OBJECT);i++){
		bullet_init(&gAlienBulletObjects[i],0,0,0,&gAlienBulletModel);
	}
		double TablePosition[] = {0,7,14,21};


		for(int i=0;i<4;i++)
		{
			_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
			alien_init(pObj,&gAlienModel);
			pObj->m_fXpos = TablePosition[i];
			pObj->m_fYpos = 2;
			pObj->m_nFSM = 1;
			gAlienObjects[i].m_pBullet=&gAlienBulletObjects[i];
		}


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
			if(kbhit() != 0) {	
				char ch = getch();
				if(ch == 'q') {
					bLoop = 0;
					puts("bye~ \r");
				}

				else if(ch=='j') {

					for(int i=0;i<sizeof(gPlaneBulletObjects)/sizeof(_S_BULLET_OBJECT);i++) {
						double vx,vy,c;

						vx=gAlienObjects[i].m_fXpos-gPlayerObject.m_fXpos;
						vy=gAlienObjects[i].m_fYpos-gPlayerObject.m_fYpos;
						c=sqrt(vx*vx+vy*vy);
						vx/=c;	vy/=c;																

						_S_BULLET_OBJECT *pObj = &gPlaneBulletObjects[i];
						if(pObj->m_nFSM==0) { //슬립상태
							pObj->pfFire(pObj,gPlayerObject.m_fXpos,gPlayerObject.m_fYpos,10.0,vx,vy,10.0);
							break;
						}
					}
				}

				gPlayerObject.pfApply(&gPlayerObject,delta_tick,ch);

			}
			for(int i=0;i<sizeof(gPlaneBulletObjects)/sizeof(_S_BULLET_OBJECT);i++) {
				_S_BULLET_OBJECT *pObj=&gPlaneBulletObjects[i];
				pObj->pfApply(pObj,delta_tick);
			}

			for(int i=0;i<4;i++ ) 
			{
				_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
				pObj->pfApply(pObj,delta_tick);

			}

			for(int i=0;i<sizeof(gAlienBulletObjects)/sizeof(_S_BULLET_OBJECT);i++) {
				gAlienBulletObjects[i].pfApply(&gAlienBulletObjects[i],delta_tick);
			}


			//타이밍 계산 
			acc_tick += delta_tick;
			if(acc_tick > 0.1) {
				gotoxy(0,0);
				map_drawTile(&gScreenBuf[0],0,0,&gScreenBuf[1]);

				gPlayerObject.pfDraw(&gPlayerObject,&gScreenBuf[1]);

				for(int i=0;i<4;i++ ) 
				{
					_S_ALIEN_OBJECT *pObj = &gAlienObjects[i];
					pObj->pfDraw(pObj,&gScreenBuf[1]);
				}

				for(int i=0;i<sizeof(gAlienBulletObjects)/sizeof(_S_BULLET_OBJECT);i++){
					gAlienBulletObjects[i].pfDraw(&gAlienBulletObjects[i],&gScreenBuf[1]);
				}

				for(int i=0;i<sizeof(gPlaneBulletObjects)/sizeof(_S_BULLET_OBJECT);i++){
					gPlaneBulletObjects[i].pfDraw(&gPlaneBulletObjects[i],&gScreenBuf[1]);
				}

				map_dump(&gScreenBuf[1],Default_Tilepalete);
				acc_tick = 0;
			}

		}

		return 0;

	}

