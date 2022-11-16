#include <gb/gb.h>
#include <gb/font.h>
#include <stdio.h>
#include "blipBoy.c"
#include "blipBg.c"
#include "windowMap.c"
#include "blipBgTile.c"
#include "GameObjects.c"



void delayer(UINT8 numloops) //Calls the delay of an entire screen to pass before continuing operations
{
 UINT8 i;
 for(i=0; i< numloops; i++)
 {
	 wait_vbl_done();
 }
}


struct Player player1;
 
const UINT8 spriteSize = 8;
const char blankMap[1] = {0x26}; 
INT8 playerLocation[2]; // array of two values to store X and Y of the player
INT8 playerVelocity;
BYTE jumping;
BYTE falling;
INT8 gravity = -2;
INT8 currentYSpeed =0;
INT8 floor = 80; //need to change to y of background tiles 
INT16 playerX;
INT8 HitSurface(INT8 yPos);


void setupPlayer();
void moveSprite(struct Player* player1, UINT8 x, UINT8 y);
void gameloops();
void main() 
{	
	font_t min_font;
	font_init();
	min_font = font_load(font_min);
	font_set(min_font);
	NR52_REG = 0x80; // is 1000 0000 in binary
	NR50_REG = 0x11; // sets volume of left and right channel "0x77" is max.
	NR51_REG = 0xFF; // is 1111 1111 in binary, select which channel we want to use in this case all of them. one bit for the L and one for the right.
	jumping = 0;
	falling = 0;
	playerX =88;
	playerVelocity = 0;
	playerLocation[0] = 0; //x
	playerLocation[1] = 0; //y
	set_sprite_data(0, 4, blipBoy); //initialize sprite
	setupPlayer();
	SHOW_SPRITES; //make sprites visible
	set_bkg_data(38,8,blipBoyTile);
	set_bkg_tiles(0,0,32,18,blipBg);

	//set_win_tiles(0,0,5,1,windowMap); //HUD box
	move_win(7,120);
	SHOW_BKG;//make background visible
	SHOW_WIN;
	DISPLAY_ON;// Turn on the display
	gameloops();
}

//checks to see if play can move on to the tile in the grid
UBYTE canMove(UINT16 playerY, UINT16 playerX ) 
{
	
	UINT16 indexTLx, indexTLy, tileIndexTL;
	UBYTE result;
	//needs the x and y of player but scroll background does not change x
	indexTLx = (playerX-8) /8;
	indexTLy = (playerY-16) /8;
	tileIndexTL = 32 * indexTLy + indexTLx; // mapsize X TLy+ TLx
	result = blipBg[tileIndexTL] == blankMap[0];
	//if(!result){playerVelocity=0;}
	return result;
}

void moveSprite(struct Player* player1, UINT8 x, UINT8 y)
{
	move_sprite(player1->spriteID[0],x, y);
	move_sprite(player1->spriteID[1], x + spriteSize, y);
	move_sprite(player1->spriteID[2],x, y + spriteSize);
	move_sprite(player1->spriteID[3],x + spriteSize, y + spriteSize);
}

void setupPlayer()
{
	player1.x = 88;
	player1.y = 80;
	player1.width = 16;
	player1.height = 16;
	
	set_sprite_tile(0,0);
	player1.spriteID[0] = 0;
	set_sprite_tile(1,1);
	player1.spriteID[1] = 1;
	set_sprite_tile(2,2);
	player1.spriteID[2] = 2;
	set_sprite_tile(3,3);
	player1.spriteID[3] = 3;

	moveSprite(&player1,player1.x,player1.y);
}

//checks to see if sprite can fall and if not returns -1 so that sprite can stop scrolling.
INT8 HitSurface(INT8 yPos)
{
	if(yPos >= floor )
	{
		return floor;
	}
	return -1;
}
//jump function that propels character in the Y direction.
void jump(struct Player* player1){
    INT8 possiblesurfaceY;

    if(jumping==0){
        jumping=1;
        currentYSpeed = 8;
    }

    // work out current speed - effect of gravities accelleration down
    currentYSpeed = currentYSpeed + gravity;    

    player1->y = player1->y - currentYSpeed;

    possiblesurfaceY = HitSurface(player1->y);
	
    if(possiblesurfaceY != -1){
        jumping = 0;
        moveSprite(player1, player1->x,possiblesurfaceY);
    }
    else{
		moveSprite(player1,player1->x,player1->y);
	}
}

//sets falling flag on. Minuses and scrolls player y by gravity constant
void gravityWorld(struct Player* player1)
{
	INT8 possiblesurface;

    if(falling==0){
        falling=1;
		currentYSpeed = 0; 
    }
	currentYSpeed = currentYSpeed + gravity;

	possiblesurface = HitSurface(player1->y);

    if(possiblesurface != -1){
        falling = 0;
         moveSprite(player1, player1->x,possiblesurface);
    }
	else{
		moveSprite(player1,player1->x,player1->y);
    }
}
void gameloops()
{
	
	UINT8 currentSpriteIndex=0;

	while(1)
	{	
		UBYTE joypad_state = joypad();
		if(joypad_state & J_A )
		{
			printf("\n %s %u \n %s %u","Player y:",(INT16)player1.y,"Player x:",(INT16)player1.x); 
		}
		//set_sprite_tile(0,currentSpriteIndex); //sets sprites current value
		/* if (joypad_state & J_LEFT) //walking sound
		{	
			NR10_REG = 0x06;
			NR11_REG = 0x44;
			NR12_REG = 0x73;
			NR13_REG = 0x00;
			NR14_REG = 0xC0;
			delayer(1);
		} */
		
		if((canMove(player1.y+8,playerX)&& canMove(player1.y+8,playerX+8)) || (falling == 0))
		{
			gravityWorld(&player1);
		}
		if((joypad_state & J_LEFT))
		{
			if(canMove(player1.y+8,playerX-8)){
			if(playerX!=0)
			{
				playerX-=8;
			}
			else
			{
				playerX = 248;
			}
			scroll_bkg(-8, playerLocation[1]);
			}
		}
		if((joypad_state & J_RIGHT))
		{
			if(canMove(player1.y+8,playerX+(UINT16)16))
			{
			
			if(playerX!=248)
			{
				playerX+=8;
			}
			else
			{
				playerX = 0;
			}
			scroll_bkg(8, playerLocation[1]);
			}
		}
		if((joypad_state & J_UP) || jumping==1)
		{
				if((canMove(player1.y+8,playerX)&& canMove(player1.y+8,playerX+(UINT16)8))){
					jump(&player1);
					}
				else
				{
					player1.y -= 8;
					moveSprite(&player1, player1.x, player1.y);
					jumping =0;
				}
		}
		delayer(4);//updates frames
	}
}

//switch(joypad()) //inputs
//		{
//			case J_LEFT:
//			scroll_bkg(-5,0);
//			break;
//			case J_RIGHT:
//			scroll_bkg(5,0);
//			break;
//			case J_UP:
//			break;
//			case J_DOWN:
//			break;
//		}