/*
 * Traffic_Lights.cpp
 *
 * Created: 11/5/2019 2:46:46 PM
 *  Author: students
 */ 

#include <avr/io.h>

//Signal Times
const int GreenTime=30;
const int GreenMinTime=15;
const int YellowTime=3;
const int PedestrianYellowTime=3;
const int PedestrianCrossingTime=15;
const int MaxPedestrianCrossingTime=23;
const int PedestrianCrossingIncrementTime=8;

//Delay Time Variables
const int MultiplicationFactor=670;
const int FrequencyFactor=8;

void Initialize();
void AllRed();

void Road1Green();
void Road1Yellow();
void Road1StraightYellow();

void Road2Green();
void Road2Yellow();
void Road2StraightYellow();

void Road3Green();
void Road3Yellow();
void Road3StraightYellow();

void Road4Green();
void Road4Yellow();
void Road4StraightYellow();

void Ped1Cross();
void Ped1Yellow();
void Ped2Cross();
void Ped2Yellow();

int checkPed1Cross();
int checkPed2Cross();
int checkPedInput();

int pedestrian(int force,int CallNo);
int vehdelay(int TotalTime,int MinTime);
int peddelay(int z);

int main()
{
	Initialize();
	AllRed();
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	
	int NextSignal=1;
	int check=0;
	
	//First Signal
	Signal1:
	NextSignal=2;
    Road1Green();
	check=vehdelay(GreenTime,GreenMinTime);
	if(check)
		goto SignalCheck;
	Road1Yellow();
	check=vehdelay(YellowTime,-1);
	if(check)
		goto SignalCheck;
	
	//Second Signal
	Signal2:
	NextSignal=3;
	Road2Green();
	check=vehdelay(GreenTime,GreenMinTime);
	if(check)
		goto SignalCheck;
	Road2Yellow();
	check=vehdelay(YellowTime,-1);
	if(check)
		goto SignalCheck;
	
	//Third Signal
	Signal3:
	NextSignal=4;
	Road3Green();
	check=vehdelay(GreenTime,GreenMinTime);
	if(check)
		goto SignalCheck;
	Road3Yellow();
	check=vehdelay(YellowTime,-1);
	if(check)
		goto SignalCheck;
	
	//Fourth Signal
	Signal4:
	NextSignal=1;
	Road4Green();
	check=vehdelay(GreenTime,GreenMinTime);
	if(check)
		goto SignalCheck;
	Road4Yellow();
	check=vehdelay(YellowTime,-1);
	if(check)
		goto SignalCheck;
	goto Signal1;

	SignalCheck:
	if(NextSignal==1)
		goto Signal1;
	else if(NextSignal==2)
		goto Signal2;
	else if(NextSignal==3)
		goto Signal3;
	else if(NextSignal==4)
		goto Signal4;
	return(0);
}

int vehdelay(int TotalTime,int MinTime)  
{
	TCNT0=0x00;
	TCCR0=0x01;
	int curtime=0;
	int prevtime=0;
	int actime=0;
	int mult=0;
	int check=0,force=0;
	
	while(curtime<TotalTime )	//tr=time remaining   t0=time()
	{
		if(((prevtime)&(~(TCNT0&0x80)))|((~prevtime)&(TCNT0&0x80)))
			actime++;
		if(actime==FrequencyFactor)
		{
			actime=0;
			mult++;
		}
		if(mult==MultiplicationFactor)
		{
			mult=0;
			curtime++;
		}
		prevtime=TCNT0&(1<<7);
		if(curtime<MinTime)
		{
			if (checkPed1Cross())
				force=1;
			else if (checkPed2Cross())
				force=2;
		}
		else if(curtime>=MinTime)
		{
			check=pedestrian(force,1);
			if(check)
				return(1);
		}
	}
	return(0);
}

int pedestrian(int force,int CallNo)
{	
	int check=0;
	int j;
	int z;
	if(checkPedInput() || force)
	{
		check=1;
		if (checkPed1Cross()||(force==1))
		{
			z=1;
			if(PORTA & (1<<6)||PORTA & (1<<5) )
			{	
				if(PORTB & (1<<6)||PORTB & (1<<5))
				{
					Ped2Yellow();
					j=peddelay(z);
				}
				else
				{
					Road2Yellow();
					j=peddelay(z);
				}
			}
			else if(PORTB & (1<<6)||PORTB & (1<<5))
			{	
				Road4Yellow();
				j=peddelay(z);
			}
			else if(PORTA & (1<<2)||PORTA & (1<<1))
			{
				Road1StraightYellow();
				j=peddelay(z);
			}
			else
			{	
				Road3StraightYellow();
				j=peddelay(z);
			}
			Ped1Cross();
		}
		else
		{	
			z=0;
			if(PORTA & (1<<2)||PORTA & (1<<1))
			{
				if(PORTB & (1<<2)||PORTB & (1<<1))
				{
					Ped1Yellow();
					j=peddelay(z);
				}
				else
				{
					Road1Yellow();
					j=peddelay(z);
				}
			}
			else if(PORTB & (1<<6)||PORTB & (1<<5))
			{
				Road3Yellow();
				j=peddelay(z);
			}
			else if(PORTA & (1<<2)||PORTA & (1<<1))
			{
				Road2StraightYellow();
				j=peddelay(z);
			}
			else
			{
				Road4StraightYellow();
				j=peddelay(z);
			}
			Ped2Cross();
		}

		TCNT0=0x00;
		TCCR0=0x01;
		int curtime=0;
		int prevtime=0;
		int actime=0;
		int mult=0;
		int a=PedestrianCrossingTime;
		
		while( curtime<a && a<MaxPedestrianCrossingTime)	// tr=time remaining   t0=time()
		{
			if(((prevtime)&(~(TCNT0&0x80)))|((~prevtime)&(TCNT0&0x80)))
			actime++;
			if(actime==FrequencyFactor)
			{
				actime=0;
				mult++;
			}
			if(mult==MultiplicationFactor)
			{
				mult=0;
				curtime++;
			}
			prevtime=TCNT0&(1<<7);
			if(z==1)
			{
				if(checkPed1Cross())
				{	
					if(curtime>PedestrianCrossingTime-PedestrianCrossingIncrementTime)
						a=curtime+PedestrianCrossingIncrementTime;
					if(a>MaxPedestrianCrossingTime)
						a=MaxPedestrianCrossingTime;	
				}
				else if(checkPed2Cross())
					j=1;
			}
			else if(z==0)
			{	
				if(checkPed2Cross())
				{
					if(curtime>PedestrianCrossingTime-PedestrianCrossingIncrementTime)
						a=curtime+PedestrianCrossingIncrementTime;
					if(a>MaxPedestrianCrossingTime)
						a=MaxPedestrianCrossingTime;
				}
				else if(checkPed1Cross())
					j=1;	
			}
		}
		if (j==1&&CallNo==1)
			pedestrian(3-z,2);
	
		AllRed();
	}
	return(check);
}

int peddelay(int z)
{
	TCNT0=0x00;
	TCCR0=0x01;
	
	int curtime=0;
	int prevtime=0;
	int actime=0;
	int mult=0;
	int j=0;
	
	while(curtime<PedestrianYellowTime) 	//tr=time remaining   t0=time()
	{
		if(((prevtime)&(~(TCNT0&0x80)))|((~prevtime)&(TCNT0&0x80)))
		actime++;
		if(actime==FrequencyFactor)
		{
			actime=0;
			mult++;
		}
		if(mult==MultiplicationFactor)
		{
			mult=0;
			curtime++;
		}
		prevtime=TCNT0&(1<<7);
		
		if(z==1)
		{
			if(checkPed2Cross())	
				j=1;
		}
		else if(z==0)
		{
			if(checkPed1Cross())
				j=1;
		}
	}
	return(j);
	AllRed();
}

void Initialize()
{
	DDRA=0XFF;
	DDRB=0XFF;
	DDRC=0XFF;
	DDRD=0X00;
}

void AllRed()
{
	PORTA=0x11;
	PORTB=0x11;
	PORTC=0x55;
}

void Road1Green()
{
	PORTA=0x1C;
	PORTB=0x11;
	PORTC=0x55;
}

void Road1Yellow()
{
	PORTA=0x12;
	PORTB=0x11;
	PORTC=0x55;
}

void Road1StraightYellow()
{
	PORTA=0x16;
	PORTB=0x11;
	PORTC=0x55;
}

void Road2Green()
{
	PORTA=0xC1;
	PORTB=0x11;
	PORTC=0x55;
}

void Road2Yellow()
{
	PORTA=0x21;
	PORTB=0x11;
	PORTC=0x55;
}

void Road2StraightYellow()
{
	PORTA=0x61;
	PORTB=0x11;
	PORTC=0x55;
}

void Road3Green()
{
	PORTA=0x11;
	PORTB=0x1C;
	PORTC=0x55;
}

void Road3Yellow()
{
	PORTA=0x11;
	PORTB=0x12;
	PORTC=0x55;
}

void Road3StraightYellow()
{
	PORTA=0x11;
	PORTB=0x16;
	PORTC=0x55;
}

void Road4Green()
{
	PORTA=0x11;
	PORTB=0xC1;
	PORTC=0x55;
}

void Road4Yellow()
{
	PORTA=0x11;
	PORTB=0x21;
	PORTC=0x55;
}

void Road4StraightYellow()
{
	PORTA=0x11;
	PORTB=0x61;
	PORTC=0x55;
}

void Ped1Cross()
{
	PORTA=0x14;
	PORTB=0x14;
	PORTC=0x99;
}

void Ped1Yellow()
{
	PORTA=0x12;
	PORTB=0x12;
	PORTC=0x99;
}

void Ped2Cross()
{
	PORTA=0x41;
	PORTB=0x41;
	PORTC=0x66;
}

void Ped2Yellow()
{
	PORTA=0x21;
	PORTB=0x21;
	PORTC=0x66;
}

int checkPed1Cross()
{
	return ((PIND & (1<<4))||(PIND & (1<<5))||(PIND & (1<<6))||(PIND & (1<<7)));
}

int checkPed2Cross()
{
	return ((PIND & (1))||(PIND & (1<<1))||(PIND & (1<<2))||(PIND & (1<<3)));
}

int checkPedInput()
{
	return (PIND & 0xFF);
}