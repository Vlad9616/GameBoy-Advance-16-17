#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <stdio.h>
#include <stdlib.h>
#include <gba_input.h>
#include <math.h>
#include <maxmod.h>
#include "soundbank.h"
#include "soundbank_bin.h"

extern unsigned int charData[];
extern unsigned int mapData[];

extern void backB (int *increment,int* bgZero, int* bgThree);
extern void upPos(int * position);
extern void downPos(int *position);

extern void collisionx (int *xOne,int *xTwo,int* result);
extern void collisiony (int *yOne,int *yTwo,int *result);

unsigned short* control=(unsigned short*)0x04000000+0x132;
unsigned short* oam=(unsigned short*)0x07000000;

//FINAL VERSION
//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------

/*NOT WORKING
int collision(int x1,int y1,int x2,int y2)
{
	int e1=(x2-x1)*(x2-x1);
	int e2=(y2-y1)*(y2-y1);
	int result=sqrt(e1+e2);
	return result;
}
*/

void aiPlane(int y,int x,int visible)
{
	oam[8]=y<<0 | 0<<8 | visible<<9 | 1<<14;
	oam[9]=x<<0 | 1<<12 | 2<<14; 
	oam[10]=1<<0 | 1<<10 |1<<12;
	oam[11]=0;
}

void Bullet (int y,int x,int visible)
{
	oam[20]=y<<0 | 0<<8 | visible<<9 | 1<<14;
	oam[21]=x<<0 | 0<<14; 
	oam[22]=13<<0 | 1<<10 |0<<14;
	oam[23]=0;
}

void aiBullet(int y,int x,int visible)
{
	oam[28]=y<<0 | 0<<8 | visible<<9 | 1<<14;
	oam[29]=x<<0 | 1<<12 | 0<<14; 
	oam[30]=13<<0 | 1<<10 | 1<<12;
	oam[31]=0;
}

void Player(int y,int x,int visible)
{
	oam[0]=y<<0 | 0<<8 | visible<<9 | 1<<14;
	oam[1]=x<<0 | 0<<12 | 2<<14; 
	oam[2]=1<<0 | 1<<10 |0<<12;
	oam[3]=0;
}

void Explosion(int y,int x,int visible)
{
	oam[24]=y<<0 | 0<<8 | visible<<9 | 0<<15;
	oam[25]=x<<0 | 1<<14; 
	oam[26]=9<<0 | 1<<10 |0<<14;
	oam[27]=0;
}

void Health1(int y,int x, int visible)
{
	oam[32]=y<<0 | 0<<8 | visible<<9 | 0<<15;
	oam[33]=x<<0 | 0<<14; 
	oam[34]=15<<0 | 1<<10 |0<<14;
	oam[35]=0;
}
void Health2(int y,int x,int visible)
{
	oam[36]=y<<0 | 0<<8 | visible<<9 | 0<<15;
	oam[37]=x<<0 | 0<<14; 
	oam[38]=15<<0 | 1<<10 |0<<14;
	oam[39]=0;
}
void Health3(int y, int x,int visible)
{
	oam[40]=y<<0 | 0<<8 | visible<<9 | 0<<15;
	oam[41]=x<<0 | 0<<14; 
	oam[42]=15<<0 | 1<<10 |0<<14;
	oam[43]=0;
}
int gameState;

typedef enum GAME_STATES
{
	 PLAY=1,
	 PAUSE=2,
	 SET_LEVEL=3
}GAME_STATES;

typedef enum WAVES
{
	NOWAVE=0,
	WAVE1=1,
	WAVE2=2,
	WAVE3=3,
	END=4
	
}WAVES;

int wave;

bool deadSound=true;
bool menuSound=true;
bool winSound=true;
bool gameSound=false;

int main(void) 
{
//--------------------------------------------------------------------------------
	// the vblank interrupt must be enabled for VBlankIntrWait() to work
	irqInit();
	irqSet(IRQ_VBLANK,mmVBlank);
	unsigned short keyTest;
	
	//SOUND
	mmInitDefault((mm_addr)soundbank_bin,8);	
	
	
	//REFFERENCE: I USED THIS WEBSITE IN ORDER TO GET XEXADECIMAL VALUES OF COLORS
	//Gameboy Color Palettes.(2013).Retrieved from: http://www.huderlem.com/demos/gameboypalette.html
	
	//COLORS
	//backgroud color
	unsigned short* back=(unsigned short*)0x05000000;
	back[0]=0x531f;
	//pallete used to create tiles
	unsigned short* pallete15=(unsigned short*)0x050001E0;
	pallete15[0]=0;
	pallete15[1]=0x0011;
	pallete15[2]=0x6337;
	pallete15[3]=0x46b6;
	pallete15[4]=0x0400;
	pallete15[5]=0x7f9a;
	pallete15[6]=0x4bff;
	pallete15[7]=0x49ff;
	pallete15[8]=0x001f;
	
	unsigned short* pallete14=(unsigned short*)0x050001C0;
	pallete14[4]=0x0400;
	pallete14[5]= 0x4e41;
	pallete14[6]=0x7f54;
	pallete14[7]=0x4e71;
	
	//pallets used on objects
	unsigned short* palleteOne=(unsigned short*) 0x5000200;
	unsigned short* palleteTwo=(unsigned short*) 0x5000220;
	float afd=sqrt(4);
	palleteOne[0]=0;		
	palleteOne[1]=0x1c20;
	palleteOne[3]=0x6af2;
	palleteOne[2]=0x6f79;				
	palleteOne[4]=0x01bf;	
	palleteOne[5]=0x03ff;	
	palleteOne[6]=0x4e40;		
	palleteOne[7]=0x6640;	
	palleteOne[8]=0x5eb1;
	palleteOne[9]=0x001f;
	palleteOne[10]=0x0015;
	palleteOne[11]=0x031e;
	
	palleteTwo[0]=0;		
	palleteTwo[1]=0x1c20;
	palleteTwo[3]=0x6af2;
	palleteTwo[2]=0x6f79;				
	palleteTwo[4]=0x01bf;  
	palleteTwo[5]=0x03ff;  
	palleteTwo[6]=0x28ff;  	
	palleteTwo[7]=0x49b9;	
	palleteTwo[8]=0x5eb1;
				
	//DISPLAY CONTROL REGISTER
	unsigned short* dcr=(unsigned short*)0x04000000;
	dcr[0]= 0<<0 | 1<<6 | 1<<8 | 1<<9 | 1<<10 | 1<<11 | 1<<12;
	
	//LAYERS
	unsigned short* bg0=(unsigned short*)0x04000008;  //layer1
	unsigned short* bg1=(unsigned short*)0x0400000A;  //layer2
	unsigned short* bg2=(unsigned short*)0x0400000C;  //layer3
	unsigned short* bg3=(unsigned short*)0x0400000E;  //layer4
	
    //2- character base block   8-screen base block
	bg0[0]=2<<0 | 4<<2 | 0<<7 | 11<<8;
	bg1[0]=0<<0 | 0<<2 | 10<<8;   
	bg2[0]=2<<0 | 0<<2 | 9<<8;    
	bg3[0]=1<<0 | 0<<2 | 8<<8;
	
	//SCREEN BASE BLOCKS
	unsigned short* sbb_8 =(unsigned short*)0x6004000;  //base block 8
	unsigned short* sbb_9 =(unsigned short*)0x6004800;  //base block 9 
	unsigned short* sbb_10=(unsigned short*)0x6005000;  //base block 10 
	unsigned short* sbb_11=(unsigned short*)0x6005800;  //base block 11
	
	//reading character data
	unsigned int* myChar=(unsigned int*)0x6000020;
	for (int i=0 ;i<=2050;i++)
	{
		myChar[i]=mapData[i];
	}
	
	//reading sprite data
	unsigned int* spriteData=(unsigned int*)0x06010020;
	for (int i=0;i<=2050;i++)
	{
		spriteData[i]=charData[i];
	}
	
	//background handling variables
	volatile short* bg0_x=(volatile short*)0x4000010;
	volatile short* bg0_y=(volatile short*)0x4000012;
	
	volatile short* bg3_x=(volatile short*)0x400001c;
	volatile short* bg3_y=(volatile short*)0x400001e;
	
	//Player Character
	int playerXpos,playerYpos;  //player x and y position
	playerXpos=10; 				//set player x
	playerYpos=70;				//set player y
	
	bool alive=true;			//check if player is alive
	int health=3;				//player max health
	int currentHealth=3;		//player current health
	bool isFired=false;			//
	//AI
		//Rocket
	int rocketX=playerXpos;     //x pos of player's rocket
	int rocketY=playerYpos;  	//y pos of player's roclet

	//Plane
	int planeX=250;				//x pos of enemy plane
	int planeY=50;				//y pos of enemy plane
	
	int aiBulletX=planeX;		//x pos of enemy plane's rocket
	int aiBulletY=planeY;		//y pos of enemy plane's rocket
	
	bool explosion=false;		//check if explosion happened
	bool enemyLoad=false;		//check if enemy plane was loaded
	bool enemyFire=true;		
	
	//Backgrounds
	int bg0_xScroll=0;					
	int bg3_xScroll=0;
	
	int i=0;
	bool game=1;
	bool loaded=false;
	int playSong=0;
	
	gameState=PAUSE;
	wave=NOWAVE;
	int xResult=0;
	int yResult=0;
	
	int xResult2=0;
	int yResult2=0;
	
	while (game)
	{
		//sound
		mmFrame();
		
		//input
		scanKeys();  
		keyTest=keysDown();
		
		if (menuSound==true)
		{
		mmStart(MOD_MENU,MM_PLAY_LOOP );
		menuSound=false;
		}

		if (gameState==PAUSE)
		{
			//START GAME TEXT
			oam[12]=55<<0 | 0<<8 | 0<<9 | 1<<14;
			oam[13]=110<<0 | 1<<14; 
			oam[14]=16<<0 | 3<<10 |0<<14;
			oam[15]=0;
			
			//press BUTTON TEXT
			oam[16]=70<<0 | 0<<8 | 0<<9 | 1<<14;
			oam[17]=110<<0 | 1<<14; 
			oam[18]=20<<0 | 3<<10 |0<<14;
			oam[19]=0;
			
			//plane
			Player(20,110,0);
			Bullet(20,110,0);
			
			//CLEAR SCREEN
			oam[20]=1<<9; //bullet
			oam[8]=1<<9; //AI plane
				
			//start game
			if (KEY_A-keyTest==0)
				gameState=SET_LEVEL;
		}
		
		///DRAW MAP TILES START
			backB (&i,&bg0_xScroll,&bg3_xScroll);
			
			sbb_11[(18*32)+i]=1<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(18*32)+i+1]=2<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(19*32)+i]=3<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(19*32)+i+1]=4<<0 | 0<<10 | 0<<11 | 15<<12;
			
			sbb_11[(16*32)+i]=5<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(16*32)+i+1]=6<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(17*32)+i]=7<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(17*32)+i+1]=8<<0 | 0<<10 | 0<<11 | 15<<12;
			
			if (i%5==0 && i%2==0)
		{	
			sbb_11[(12*32)+i]=13<<0| 0<<10 | 0<<11 | 14<<12;
			sbb_11[(12*32)+i+1]=14<<0 | 0<<10 | 0<<11 | 14<<12;
			sbb_11[(13*32)+i]=15<<0 | 0<<10 | 0<<11 | 14<<12;
			sbb_11[(13*32)+i+1]=16<<0 | 0<<10 | 0<<11 | 14<<12;
			sbb_11[(14*32)+i]=17<<0| 0<<10 | 0<<11 | 14<<12;
			sbb_11[(14*32)+i+1]=18<<0 | 0<<10 | 0<<11 | 14<<12;
			sbb_11[(15*32)+i]=19<<0 | 0<<10 | 0<<11 | 14<<12;
			sbb_11[(15*32)+i+1]=20<<0 | 0<<10 | 0<<11 | 14<<12;
		}
		else
			if (i%5==0 && i%2!=0)
		{	
			sbb_11[(12*32)+i]=13<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(12*32)+i+1]=14<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(13*32)+i]=15<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(13*32)+i+1]=16<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(14*32)+i]=17<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(14*32)+i+1]=18<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i]=19<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i+1]=20<<0 | 0<<10 | 0<<11 | 15<<12;
		}
			
		if (i%4==0 && i%5!=0)
		{
			sbb_11[(14*32)+i]=22<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(14*32)+i+1]=23<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i]=24<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i+1]=25<<0 | 0<<10 | 0<<11 | 15<<12;
		}
		if ((i%3==0 && i%5!=0) && i%4!=0)
		{
			sbb_11[(12*32)+i]=32<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(12*32)+i+1]=33<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(13*32)+i]=34<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(13*32)+i+1]=35<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(14*32)+i]=36<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_11[(14*32)+i+1]=37<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i]=38<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_11[(15*32)+i+1]=39<<0 | 0<<10 | 0<<11 | 15<<12;
		}
		
			sbb_8[(4*32)+7]=9<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_8[(4*32)+8]=10<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(5*32)+7]=11<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(5*32)+8]=12<<0 | 0<<10 | 0<<11 | 15<<12;
			
			sbb_8[(2*32)+10]=9<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_8[(2*32)+11]=10<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(3*32)+10]=11<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(3*32)+11]=12<<0 | 0<<10 | 0<<11 | 15<<12;
			
			sbb_8[(8*32)+15]=9<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_8[(8*32)+16]=10<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(9*32)+15]=11<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(9*32)+16]=12<<0 | 0<<10 | 0<<11 | 15<<12;
			
			sbb_8[(13*32)+15]=26<<0| 0<<10 | 0<<11 | 15<<12;
			sbb_8[(13*32)+16]=27<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(13*32)+17]=28<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(14*32)+15]=29<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(14*32)+16]=30<<0 | 0<<10 | 0<<11 | 15<<12;
			sbb_8[(14*32)+17]=31<<0 | 0<<10 | 0<<11 | 15<<12;
			
			*bg0_x=bg0_xScroll;
			*bg3_x=bg3_xScroll;
		
		if (loaded==false && gameState==SET_LEVEL)
		{
			mmStop();
			gameSound=true;
			//CLEAR MENU
			oam[4]=1<<9;
			oam[12]= 1<<9;
			oam[16]=1<<9;
			
			//SET
			playerXpos=10;
			playerYpos=100;
	
			Player(playerYpos,playerXpos,0);
			Bullet(playerYpos+7,playerXpos,0);
			
			//AI PLANE
			planeX=250;
			planeY=50;
			
			aiPlane(planeY,planeX,0);
			//Bullet(playerYpos,playerXpos,0);
			loaded=true;
		}
		
		if (gameSound==true)
		{
		if (playSong==0&& gameSound==true)
		{
		mmJingle(MOD_LEVEL_LOOP);
		playSong=1;
		}
		playSong=mmActiveSub();
		}
		
		if (gameState==SET_LEVEL)
		{
			//PLAYER PLANE ANIMATION
			if (playerYpos>50)
			{
			Player(playerYpos--,playerXpos,0);
			Bullet(playerYpos+7,playerXpos,0);
			}
			
			//start waves when player is at position y=50
			if(playerYpos==50)
			{
			gameState=PLAY;
			wave=WAVE1;
			}
		}
		
		if (alive==false)
		{
			playerYpos=playerYpos+1;
			Player(0,0,1);
			if (playerYpos>150)
			Explosion(150,playerXpos,0);
		else
			Explosion(playerYpos,playerXpos,0);
		}
		else
		{
			Explosion(150,playerXpos,1);
		}
		
		if (gameState==PLAY)
		{
			//player control
			if (KEY_UP-keyTest==0)
			{
				playerYpos=playerYpos-3;
				Player(playerYpos,playerXpos,0);	
			}
			
			if (KEY_DOWN-keyTest==0)
			{
				playerYpos=playerYpos+3;
				Player(playerYpos,playerXpos,0);
			}
			
			//set health
			if (health==3)
			{
			Health1(10,5,0);
			Health2(10,15,0);
			Health3(10,25,0);
			}
			else
			if (health==2)
			{
				Health1(10,5,0);
				Health2(10,15,0);
				Health3(10,25,1);
			}
				else
					if (health==1)
					{
						Health1(10,5,0);
						Health2(10,15,1);
						Health3(10,25,1);
					}
					else
						if (health==0)
						{
							Health1(10,5,1);
							Health2(10,15,1);
							Health3(10,25,1);
						}
								
			//update rocket position to player position
			if (isFired==false || alive==false)
			{
			rocketY=playerYpos;
			Bullet(rocketY+7,playerXpos,1);
			}
			
			//FIRE KEY
			if (KEY_A-keyTest==0 && isFired==false)
			{
				mmStart(MOD_SHOOT2,MM_PLAY_ONCE);
				isFired=true;
			}
			
			//update the rocket position
			if (isFired==true && alive==true)
			{
				upPos(&rocketX);
				Bullet(rocketY+7,rocketX,0);
			}
			else
			{
				Bullet(0,0,1);
			}
			
			//reset rocket
			if (rocketX>250)
			{
				isFired=false;
				rocketX=playerXpos;
				rocketY=playerYpos;
			}
	
				//spawn enemy plane
				if (enemyLoad==false)
				{
				aiPlane(planeY,planeX,0);
				
				//enemy plane animation
					if (planeX>200)
					{
					aiPlane(planeY,planeX,0);
					downPos(&planeX);
					}
					else
					{
						planeX=200;
						enemyLoad=true;
						aiBulletX=planeX;
						aiBulletY=planeY;
					}
				}
				
				if (enemyFire==true)
				{
				aiBullet(aiBulletY+5,aiBulletX--,0);
				}
				//reset enemy bullet
				if (aiBulletX<=0)
				{
					aiBullet(planeY+5,planeX,1);
				}
				
				xResult=0;
				yResult=0;
				if (alive==true)
				{
				collisionx(&planeX,&rocketX,&xResult);
				collisiony(&rocketY,&planeY,&yResult);
				if(xResult==1 && yResult==2)
				{
				//
				if (explosion==false)
				{
				mmStart(MOD_DESTROY,MM_PLAY_ONCE);
				explosion=true;
				rocketX=playerXpos;
				rocketY=playerYpos;
				oam[8]=1<<9; //AI plane
				isFired=false;
				}
				}
				}
				
				xResult2=0;
				yResult2=0;
				if (wave!=END)
				{
				collisionx(&playerXpos,&aiBulletX,&xResult2);
				collisiony(&aiBulletY,&playerYpos,&yResult2);
				if (xResult2==1 && yResult2==2)
				{
					mmStart(MOD_HIT,MM_PLAY_ONCE);
					downPos(&health);
				}
				
				//set the explosion position to enemy player position
				if (explosion==true)
				{
				planeX=planeX-1;
				Explosion(planeY,planeX,0);
				rocketX=playerXpos;
				rocketY=playerYpos;
				isFired=false;
				}
				}
				
				//update explosion position
				if (planeX>=5 && explosion==true)
				{
					planeX=planeX-1;
					Explosion(planeY,planeX,0);
				}
				
				//set different waves
				if (planeX<5 && wave==WAVE1)
				{
					Explosion(planeY,planeX,1);
					wave=WAVE2;
					planeX=250;
					planeY=70;
					enemyLoad=false;
					explosion=false;
					isFired=false;
				}
				if (planeX<5 && wave==WAVE2)
				{
					Explosion(planeY,planeX,1);
					wave=WAVE3;
					planeX=250;
					planeY=30;
					enemyLoad=false;
					explosion=false;
					isFired=false;
				}
				if (planeX<5 && wave==WAVE3)
				{
					Explosion(planeY,planeX,1);
					wave=END;
					aiPlane(0,0,1);
					enemyLoad=false;
					explosion=false;
					isFired=false;
				}
			
			if (wave==END)
			{
				//END GAME TEXT
					oam[44]=50<<0 | 0<<8 | 0<<9 | 1<<14;
					oam[45]=100<<0 | 1<<14; 
					oam[46]=29<<0 | 3<<10 | 0<<14;
					oam[47]=0;
				//RESTART TEXT
					oam[52]=70<<0 | 0<<8 | 0<<9 | 1<<14;
					oam[53]=100<<0 | 1<<14; 
					oam[54]=33<<0 | 3<<10 | 0<<14;
					oam[55]=0;
					
					
					aiPlane(0,0,1);
					aiBullet(0,0,1);
					
				gameSound=false;
				enemyFire=false;
				if (playerXpos<250)
				{
					Player(playerYpos,playerXpos++,0);
				}
				
				if (winSound==true)
				{
				mmStart(MOD_WIN	,MM_PLAY_ONCE);
				winSound=false;
				}
			}
			
			//if player dies
			if(health==0)
			{
				//game over text
				oam[48]=50<<0 | 0<<8 | 0<<9 | 1<<14;
				oam[49]=100<<0 | 1<<14; 
				oam[50]=24<<0 | 3<<10 | 0<<14;
				oam[51]=0;
				//restart text
				oam[52]=70<<0 | 0<<8 | 0<<9 | 1<<14;
				oam[53]=100<<0 | 1<<14; 
				oam[54]=33<<0 | 3<<10 | 0<<14;
				oam[55]=0;
				
				alive=false;
				gameSound=false;
				if (deadSound==true)
				{
				mmStart(MOD_ENDGAME,MM_PLAY_ONCE);
				deadSound=false;
				}
			}
			
		//RESTART GAME
		if (KEY_B-keyTest==0 && (wave==END || health==0))
			{
				oam[44]=50<<0 | 0<<8 | 1<<9 | 1<<14;//hide text
				oam[48]=50<<0 | 0<<8 | 1<<9 | 1<<14;//hide text
				//RESET VALUES
				alive=true;
				oam[52]=1<<9;
				health=3;
				loaded=false;
				gameState=SET_LEVEL;
				enemyLoad=false;
				wave=NOWAVE;
				winSound=true;
				deadSound=true;
				gameSound=true;
				enemyFire=true;
				aiPlane(50,250,0);
			}	
		}
		VBlankIntrWait();
	}
}

