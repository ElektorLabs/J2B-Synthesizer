/*
Copyright 2013 Paul Soulsby www.soulsbysynths.com
    This file is part of Atmegatron.

    Atmegatron is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Atmegatron is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Atmegatron.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "atmegatron.h"

#if PRODUCT==ATMEGATRON || PRODUCT==ATCYCLOTRON || PRODUCT==DELAYERTRON

//****THIS IS WHERE THE ARPEGGIATOR PATTERNS ARE STORED*******
//The way its is by up the notes currently being held down
//So if C,D,E,F,G  were held down and pattern was {5,3,2,4,1},  
//the arp would be G,E,D,F,C.  Minus nums count downwards


const signed char arp_patstore[16][16] =
{
  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                          //THIS ONE SHOULD ALWAYS BE BLANK
  {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16},                   //up
  {-1,-2,-3,-4,-5,-6,-7,-8,-9,-10,-11,-12,-13,-14,-15,-16},   //down
  {1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8},                          //octave
  {1,1,2,3,4,4,5,6,7,7,8,9,10,10,11,12},                      //5ths
  {1,3,2,4,3,5,4,6,5,7,6,8,7,9,8,10},                         //skip 1 up
  {-1,-3,-2,-4,-3,-5,-4,-6,-5,-7,-6,-8,-7,-9,-8,-10},         //skip 1 down
  {-1,-1,-1,-1,-3,-3,-2,-2,-1,-1,-1,-1,-3,-3,-2,-2},          //I feel love
  {2,1,3,2,4,5,6,1,2,1,3,2,4,5,6,1},                          //Horton
  {-1,-2,-1,-4,-2,-1,-4,-1,-4,-2,-3,-2,-1,-3,-2,-1},          //Etheridge
  {1,2,3,2,4,3,2,3,1,3,2,4,1,3,2,1},                          //Meads
  {1,2,3,2,-1,6,5,4,0,0,-1,-2,0,0,4,0},                       //Palmer
  {4,1,4,1,3,1,4,1,2,1,3,1,4,1,4,1},                          //gwEm 1
  {1,1,4,4,1,1,4,4,3,3,6,6,2,2,5,5},                          //gwEm 2 
  {1,1,3,2,2,4,3,3,5,4,4,6,5,5,7,6},                          //Triplet up
  {1,2,3,2,1,2,3,2,1,2,3,2,1,2,3,2},                          //You Will Be Robyn
};

//local vars
unsigned int arp_ticksperstep = 250;      //ticks each step of arp should hold for
//unsigned long arp_nexttick = 0;            //tick when next step fires
unsigned int arp_curtick = 0;             //current tick
//unsigned long arp_starttick = 0;           //start tick
   
byte arp_curpos = 0;                       //current position in pattern
byte arp_curnote = 255;                    //current MIDI note in pattern
boolean arp_pongdown = false;              //current direction if in ping-pong mode

//lets and gets
boolean arp_pingpong = false;
byte arp_curpat = 0;
unsigned int arp_speed = 8191;


//lets and gets
void Arp_Let_Type(byte newpat)
{             //set pattern number (0-15)
  //byte i;
  if (newpat!=arp_curpat){                  //if new pattern is different to current
    arp_curpat = newpat;                    //set it
    Arp_Reset();                            //Reset vars for safety
  }
}


byte Arp_Get_Type(void)
{
  return arp_curpat;
}


void Arp_Let_Speed(unsigned int newspeed)
{                                  //Set speed (speed is stored as an integer multiplier (speed_store on hardware page))
  //unsigned long cticks;
  arp_speed = newspeed;
  arp_ticksperstep = ((unsigned long)arp_speed+1) * MASTER_BEATTICK >> 13;  //multiplier * ticks-per-beat (500ms=120bpm) / 8192.  This is a way of not storing speed as floats 
}


unsigned int Arp_Get_Speed(void)
{
  return arp_speed;
}


void Arp_Let_PingPong(boolean newpingpong)
{  //set ping-pong mode.   This runs sequence forwards, backwards, forwards, backwards etc etc
  arp_pingpong = newpingpong;
}


boolean Arp_Get_PingPong(void)
{
  return arp_pingpong;
}


//reset all local vars before starting arp
void Arp_Reset(void)
{
  arp_curpos = 0;               //restart pattern at start
  arp_curnote = 0;              //restart note search at bottom
  //arp_starttick = master_tick;  //set start tick
  arp_curtick = 0;              //reset current tick
  //arp_nexttick = 0;             //reset current tick
}


//Poll arpegiator and see if it's time for a new note to fire
void Arp_Poll(void)
{
	// is MIDI clock off and pattern <> 0 (MIDI_ClockTick() handles arpeggiator when clock is present)
	if (MIDI_Get_ClockPresent()==false && arp_curpat>0)
	{
		//is it time for next note?
		if (arp_curtick>=arp_ticksperstep)
		{
			if (Hardware_Get_Ctrl_Shift()==GREEN)
			{
				Board_LED_Toggle(BOARD_LED1_GREEN);
				Board_LED_Set(BOARD_LED2_GREEN,!Board_LED_Test(BOARD_LED1_GREEN));
			}
			else
			{
				Board_LED_Toggle(BOARD_LED1_RED);
				Board_LED_Set(BOARD_LED2_RED,!Board_LED_Test(BOARD_LED1_RED));
			}
			//SysTick_LED_Flash(BOARD_LED1_GREEN,10);
			do
			{
				Arp_TriggerStep();  //fire note
				arp_curtick -= arp_ticksperstep;  //set the next tick for a note
			}
			while (arp_ticksperstep<arp_curtick);  //in a nasty do loop because there's a chance the poll time is slower than arp_ticksperstep
		}
		arp_curtick += master_tickspassed;  //refresh current tick
	}
}


void Arp_TriggerStep(void)
{                                                                  //fire note in pattern
  arp_curnote = Arp_FindNote(arp_patstore[arp_curpat][arp_curpos]);    //attempt to find a note to fire
  if (arp_curnote==255){                                                                 //255 = failed to find note
    if (arp_pingpong==true){                                                             //if in ping-pong mode
      if (arp_pongdown==false){                                                          //probably coz its hit the end of pattern and needs to flip direction
        arp_curpos -= 2;                                                                 //flip direction and set position
        arp_pongdown = true;
      }
      else{
        arp_curpos += 2;
        arp_pongdown = false;
      }        
    }
    else{                                                                                //if not ping-pong, give up and reset to start of pattern
      arp_curpos = 0;
    }
    arp_curnote = Arp_FindNote(arp_patstore[arp_curpat][arp_curpos]);  //have another stab at finding a note now
  }
  if (arp_curnote!=255){                                                                 //assuming it now has found as note
    MIDI_TriggerNote(arp_curnote);                                                       //fire it
  }          
  Arp_IncPatPos();                                                                       //increment current position in pattern  (more complex than just arp_curpos++)
}


/*int Arp_FindNoteUp(int count)
{
	int n = 0;
	while (count>0)
	{
		while (n<128)
		{
			n++;
			if (MIDI_Get_KeyDown(n)==true)
			{
				break;
			}
		}
		count--;
	}
	return n<128? n : 255;
}


int Arp_FindNoteDown(int count)
{
	int n = 127;
	while (count>0)
	{
		while (n>0)
		{
			n--;
			if (MIDI_Get_KeyDown(n)==true)
			{
				break;
			}
		}
		count--;
	}
	return n>0? n : 255;
}


//this finds out which note to fire, based on which notes are held down
byte Arp_FindNote(signed char pos)
{
	user_interface_print_value(1,0,pos,false);
	if (pos<0)
	{
		return Arp_FindNoteDown(-pos);
	}
	if (pos>0)
	{
		return Arp_FindNoteUp(pos);
	}
	return 255;
}*/


byte Arp_FindNote(signed char pos)
{
  byte i,j,n,l,o;
  //set to unused value = next note not found
  o = 255;
  if (pos<0)
  {
	    //set start of search at top or bottom of keyboard (at the moment you can't arpeggiate top or bottom note)
	n = 127;
  }
  else
  {
    n = 0;
  }
  l = abs(pos);
  //now track up/down keyboard finding notes.  loop l times, setting o=note if note is down
  for (i=0; i<l; i++)
  {
    for (j=0; j<128; j++)
    {
      if (pos>0)
      {
        if (n<127)
        {
          n++;
        }
        else
        {
          o = 255;
          break;
        }
      }
      else if (pos<0)
      {
        if (n>0)
        {
          n--;
        }
        else
        {
          o = 255;
          break;
        }
      }
      else
      {
        break;  //safety
      }
      if (MIDI_Get_KeyDown(n)==true)
      {
        o = n;
        break;
      }
    }
  }
  //return o.  It'll either be 255 if failed to find note, or the note num.
  return o;
}


//increments arp_curpos
void Arp_IncPatPos(void)
{                
  if (arp_pingpong==false){        //if not in ping-pong mode
    if (arp_curpos==PATLEN-1){     //if at end of pattern
      arp_curpos = 0;              //restart
    }
    else{
      arp_curpos++;                //otherwise increment
    }
  }
  else{                            //if in ping-pong  
    if (arp_pongdown==false){      //and heading up through pattern
      if (arp_curpos>=PATLEN-1){   //if hit end of pattern flip direction
        arp_curpos--;
        arp_pongdown = true;
      }
      else{
        arp_curpos++;              //otherwise decrement
      }        
    }
    else{                          //if heading down the pattern
      if (arp_curpos<=0){          //and hit the start, flip direction
        arp_curpos++;
        arp_pongdown = false;
      }
      else{
        arp_curpos--;              //otherwise decrement
      }
    }
  }
}

#endif /* PRODUCT */
