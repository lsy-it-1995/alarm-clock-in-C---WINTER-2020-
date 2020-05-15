#include <avr/io.h>
#include <stdio.h>
#include "avr.c"
#include "lcd.c"

#define SPK_ON SET_BIT(PORTB,3)
#define SPK_OFF CLR_BIT(PORTB,3)

#define W 8		// W = Whole
#define H 4
#define Q 2
#define E 1

#define h_A		440
#define h_As	466.16
#define h_B		493.88
#define h_C		523.25
#define h_Cs	554.37
#define h_D		587.33
#define h_E		659.26
#define h_F		698.46
#define h_Fs	739.99
#define h_G		783.99
#define h_Gs	830.61

#define l_A		220
#define l_As	233.08
#define l_B		246.94
#define l_C		261.63
#define l_Cs	277.18
#define l_D		293.66
#define l_E		329.63
#define l_F		349.23
#define l_Fs	369.99
#define l_G		392
#define l_Gs	415.3

typedef enum {false, true} bool;
	
struct note{
	float freq, duration;
};

struct time_variable{
	int year,
	month,
	day,
	hour,
	minute,
	second;
};

#define numNote1 sizeof(SONG1) / sizeof(SONG1[0])
const struct note SONG1[]= {
	{l_C, Q}, {l_C, Q}, {l_G, Q}, {l_G, Q}, {h_A, Q}, {h_A, Q}, {l_G, H}, {l_F, Q},
	{l_F, Q}, {l_E, Q}, {l_E, Q}, {l_D, Q}, {l_D, Q}, {l_C, H}, {l_G, Q}, {l_G, Q},
	{l_F, Q}, {l_F, Q}, {l_E, Q}, {l_E, Q}, {l_D, H}, {l_G, Q}, {l_G, Q}, {l_F, Q},
	{l_F, Q}, {l_E, Q}, {l_E, Q}, {l_D, H}, {l_C, Q}, {l_C, Q}, {l_G, Q}, {l_G, Q},
	{h_A, Q}, {h_A, Q}, {l_G, H}, {l_F, Q}, {l_F, Q}, {l_E, Q}, {l_E, Q}, {l_D, Q},
	{l_D, Q}, {l_C, H},
};

#define numNote2 sizeof(SONG2) / sizeof(SONG2[0])
const struct note SONG2[] = {
	{l_D, Q}, {l_C, Q}, {l_As, Q}, {l_C, Q}, {l_D, Q}, {l_D, Q}, {l_D, H}, {l_C, Q},
	{l_C, Q}, {l_C, H}, {l_C, Q}, {l_D, Q}, {l_F, Q}, {l_F, H}, {l_D, Q}, {l_C, Q},
	{l_As, Q}, {l_C, Q}, {l_D, Q}, {l_D, Q}, {l_D, Q}, {l_D, Q}, {l_C, Q}, {l_C, Q},
	{l_D, Q}, {l_C, Q}, {l_As, W},
};

#define numNote3 sizeof(SONG3)/sizeof(SONG3[0])
const struct note SONG3[]={
	{l_C, E}, {l_C, E}, {l_D, Q}, {l_C, Q}, {l_F, Q}, {l_E, H}, {l_C, E}, {l_C, E},
	{l_D, Q}, {l_C, Q}, {l_G, Q}, {l_F, H}, {l_C, E}, {l_C, E}, {h_C, Q}, {l_A, Q},
	{l_F, Q}, {l_E, Q}, {l_D, H}, {l_As, E}, {l_As, E}, {l_A, Q}, {l_F, Q}, {l_G, Q},
	{l_F, H},
};

#define TimersNum 5
//GLOBAL VARVIABLE
bool timers_check[TimersNum], military_time, leap_year, alarm_activation, 
	world_clock_mode, timer_mode, stopwatch_mode,daily_alarm, view_timeres_only,
	set_time_mode;
int time_in_12, song_index, world_clock_hr, timer_index;
struct time_variable tm, dailyAlarmtm,stopwatch_alarm;
struct time_variable timers[5];


//function declaration
int get_key(void);
int is_pressed(int r, int c);
void autoModify(struct time_variable *t);
void initial_time();
void lcd_display();
void play_note(int freq, int dur);
void alarm_play();
void led_alarm(int k);
void led_off();
void check_timers(struct time_variable *t, int i);
int current_timer_index= -1;
int main(void)
{
	lcd_init();
	lcd_clr();
	SET_BIT(DDRB,3);
	int i;
	for(i = 0; i < TimersNum; i++){
		timers_check[i] = false;
	}
	timer_index = -1;
	leap_year = false;
	alarm_activation = false;
	view_timeres_only = false;
	military_time = false;
	timer_mode = false;
	world_clock_mode = false;
    set_time_mode = false;
	daily_alarm = false;
	world_clock_hr = 0;
	stopwatch_mode = false;
	int k;
	int stopwatch_commnd_PausePlay = 0, stopwatch_command_Stop= 0;
	int daily_alarm_active;
	initial_time();
	while (1) 
    {
		k = get_key();
		
		if(dailyAlarmtm.hour == tm.hour && dailyAlarmtm.minute == tm.minute && dailyAlarmtm.second == tm.second && daily_alarm_active == 1){
			alarm_play();
		}
		
		if(view_timeres_only == true){
			switch(k){
				case 1://next
					for(i = 0; i < TimersNum; i++){
						if(timers_check[i] == true && timer_index<i){
						timer_index = i;
						break;	
						}
					}
					break;
				case 5://previous
					for(i = TimersNum-1; i > -1 ; i--){
						if(timers_check[i] == true && timer_index>i){
							timer_index = i;
							break;		
						}
					}
					break;
				case 16://quit
					view_timeres_only = false;
					break;
				default:
					break;
			}
		}else if(set_time_mode == true){
			switch(k){
				case 1: //inc. year
					if(tm.year<10000){
						tm.year++;
					}
					break;
				case 2: //dec. year
					tm.year--;	
					break;
				case 3: //inc. hr
					tm.hour++;
					break;
				case 4: //dec. hr
					tm.hour--;
					break;		
				case 5: //inc. month
					tm.month++;
					break;
				case 6: //dec. month
					tm.month--;
					break;
				case 7: //inc min
					tm.minute++;
					break;
				case 8: // dec min
					tm.minute--;
					break;
				case 9: //inc day
					tm.day++;
					break;
				case 10: //dec day
					tm.day--;
					break;
				case 11: //inc sec
					tm.second++;
					break;
				case 12: //dec sec
					tm.second--;
					break;
				case 16: //run	
					set_time_mode = false;
					break;
				default:
					break;
			}
			autoModify(&tm);
		}else if(world_clock_mode == true){
			switch(k){
				case 1://new york
					tm.hour -= world_clock_hr;
					world_clock_hr = 3;
					tm.hour += world_clock_hr;
					break;
				case 2://chicago			
					tm.hour -= world_clock_hr;
					world_clock_hr = 2;
					tm.hour += world_clock_hr;
					break;
				case 3:// taiwan			
					tm.hour -= world_clock_hr;
					world_clock_hr = 16;
					tm.hour += world_clock_hr;
					break;
				case 5://japan
					tm.hour -= world_clock_hr;
					world_clock_hr = 17;
					tm.hour += world_clock_hr;
					break;
				case 6: //united kingtom
					tm.hour -= world_clock_hr;
					world_clock_hr = 8;
					tm.hour += world_clock_hr;
					break;
				case 7: //thailand			
					tm.hour -= world_clock_hr;
					world_clock_hr = 15;
					tm.hour += world_clock_hr;
					break;
				case 16:
					world_clock_mode = false;
					tm.hour -= world_clock_hr;
					world_clock_hr = 0;
					break;
				default:
					break;
			}
			autoModify(&tm);
		}else if(timer_mode == true){
			switch(k){
				case 1:
					if(timers[current_timer_index].hour<23)
						timers[current_timer_index].hour++;
					break;
				case 2:
					if(timers[current_timer_index].hour>0)
						timers[current_timer_index].hour--;
					break;
				case 5:
					if(timers[current_timer_index].minute<59)
						timers[current_timer_index].minute++;
					break;
				case 6:
					if(timers[current_timer_index].minute>0)
						timers[current_timer_index].minute--;
					break;
				case 9:
					if(timers[current_timer_index].second<59)
						timers[current_timer_index].second++;
					break;
				case 10:
					if(timers[current_timer_index].second>0)
						timers[current_timer_index].second--;
					break;
				case 15://cancel
					timers_check[current_timer_index] = false;
				case 16://confirm
					timer_mode = false;
					if(timers[current_timer_index].second== 0 && timers[current_timer_index].minute == 0 &&timers[current_timer_index].hour == 0){
						timers_check[current_timer_index] = false;	
					}else
						timers_check[current_timer_index] = true;
					break;
				default:
					break;
			}
			autoModify(&timers[current_timer_index]);
		}else if(daily_alarm == true){
			switch(k){
				case 1:
					if(dailyAlarmtm.hour<23)
						dailyAlarmtm.hour++;
					break;
				case 2:
					if(dailyAlarmtm.hour>0)
						dailyAlarmtm.hour--;
					break;
				case 5:
					if(dailyAlarmtm.minute<59)
						dailyAlarmtm.minute++;
					break;
				case 6:
					if(dailyAlarmtm.minute>0)
						dailyAlarmtm.minute--;
					break;
				case 9:
					if(dailyAlarmtm.second<59)
						dailyAlarmtm.second++;
					break;
				case 10:
					if(dailyAlarmtm.second>0)
						dailyAlarmtm.second--;
					break;
				case 13:
					daily_alarm = false;
					daily_alarm_active = 0;
					break;
				case 16://confirm
					daily_alarm = false;
					daily_alarm_active = 1;
					break;
				default:
					break;
			}	
		}else if(stopwatch_mode == true){
			switch(k){
				case 1: //start
					stopwatch_commnd_PausePlay = 1;
					break;
				case 2: //pause
					stopwatch_commnd_PausePlay = 2;
					break;
				case 3: //stop
					stopwatch_command_Stop = 1;
					break;	
				case 16: //quit
					stopwatch_mode = false;
					break;
				default:
					break;
			}
			if(stopwatch_commnd_PausePlay == 1 && stopwatch_command_Stop == 0){
				stopwatch_alarm.second++;
			}else if(stopwatch_commnd_PausePlay == 2 && stopwatch_command_Stop == 0){
				//dont move
			}
		}else{
			/*key#: mode
				2:  set timers
				3:	look timers
				4:	world clock
				5:	12 hours mode
				6:	set a daily alarm
				13:	stopwatch
				15:	set date&time
			*/
			switch(k){
				case 2://multiple-timers (count down)
					timer_mode = false;
					for(i = 0; i < TimersNum; i++){
						if(timers_check[i] == false){
							timer_mode = true;
							current_timer_index = i;
							timers[current_timer_index].hour = 0;
							timers[current_timer_index].minute = 0;
							timers[current_timer_index].second = 0;
							break;
						}else{
							current_timer_index = -1;
						}
					}
					break;
				case 3://look at your timers
					view_timeres_only = true;
					for(i = 0; i < TimersNum; i++){
						if(timers_check[i] == true){
							timer_index = i;
							break;
						}
					}
					break;
				case 4://world clock
					world_clock_mode = true;
					break;
				case 5://12 hour mode on/off
					military_time = !military_time;
					break;
				case 6://set an alarm for every single day
					daily_alarm = true;
					dailyAlarmtm.hour = tm.hour;
					dailyAlarmtm.minute = tm.minute;
					dailyAlarmtm.second = tm.second;
					break;
				case 13://stopwatch (count up)
					stopwatch_mode = true;
					stopwatch_alarm.hour = 0;
					stopwatch_alarm.minute = 0;
					stopwatch_alarm.second = 0;
					break;
				case 15://set the time and date
					set_time_mode = true;
					break;
				
				default:
					break;
			}
		}
		
		for(i = 0; i < TimersNum; i++){
			if(timers_check[i] == true){
				timers[i].second--;
				autoModify(&timers[i]);
				check_timers(&timers[i], i);
			}
		}
		
		autoModify(&tm);
		lcd_display();
		if(set_time_mode == false)
			tm.second++;

		avr_wait(500);
    }
}

void led_alarm(int k){
	SET_BIT(DDRA, 0);
	SET_BIT(DDRA, 1);
	SET_BIT(DDRA, 2);
	SET_BIT(DDRA, 3);
	SET_BIT(DDRA, 4);
	SET_BIT(DDRA, 5);
	SET_BIT(DDRA, 6);
	
	if(k == 0){
		CLR_BIT(PORTA,1);
		CLR_BIT(PORTA,3);
		CLR_BIT(PORTA,5);
		SET_BIT(PORTA,0);
		SET_BIT(PORTA,2);
		SET_BIT(PORTA,4);
		SET_BIT(PORTA,6);
	}else{
		CLR_BIT(PORTA,0);
		CLR_BIT(PORTA,2);
		CLR_BIT(PORTA,4);
		CLR_BIT(PORTA,6);
		SET_BIT(PORTA,1);
		SET_BIT(PORTA,3);
		SET_BIT(PORTA,5);
	}
}

void led_off(){
	int i;
	for(i = 0; i < 7; i++){
		CLR_BIT(PORTA,i);
	}
}

void check_timers(struct time_variable *t, int i){
	if(t->hour == 0 && t->minute == 0 && t->second == 0){
		alarm_play();
		timers_check[i] = false;
	}
}

void lcd_display(){
	lcd_clr();
	char top_buff[16], bot_buff[16];
	if(view_timeres_only == true){
		int check = 0;
		int i;
		for(i = 0; i < TimersNum; i++){
			if(timers_check[i] == true){
				check = 1;
				break;
			}
		}
		if(check == 0){
			lcd_pos(0,1);
			lcd_puts2("NO TIMERS YET");
			view_timeres_only = false;
		}else{
			lcd_pos(0,1);
			sprintf(top_buff, "VIEW TIMER %d", timer_index);
			lcd_puts2(top_buff);
			lcd_pos(1,1);
			sprintf(bot_buff, "%02d:%02d:%02d TMR",timers[timer_index].hour, timers[timer_index].minute, timers[timer_index].second);
			lcd_puts2(bot_buff);
		}
	}else if(timer_mode == true){
		lcd_pos(0,1);
		sprintf(top_buff, "Your #%d ", current_timer_index);
		lcd_puts2(top_buff);
		lcd_pos(1,1);
		sprintf(bot_buff, "%02d:%02d:%02d",timers[current_timer_index].hour,timers[current_timer_index].minute,timers[current_timer_index].second);
		lcd_puts2(bot_buff);
	}else if(stopwatch_mode == true){
		lcd_pos(0,1);
		lcd_puts2("STOPWATCH MODE");
		lcd_pos(1,1);
		sprintf(bot_buff, "%02d:%02d:%02d STP",stopwatch_alarm.hour,stopwatch_alarm.minute, stopwatch_alarm.second);
		lcd_puts2(bot_buff);
	}else if(daily_alarm == true){
		lcd_pos(0,1);
		lcd_puts2("DAILY ALARM SET");
		lcd_pos(1,1);
		sprintf(bot_buff, "%02d:%02d:%02d",dailyAlarmtm.hour,dailyAlarmtm.minute, dailyAlarmtm.second);
		lcd_puts2(bot_buff);
	}else if(alarm_activation == true){
		lcd_pos(0,1);
		lcd_puts2("ALARM ACTIVES");
		lcd_pos(1,1);
		sprintf(bot_buff, "%02d:%02d:%02d",tm.hour,tm.minute, tm.second);
		lcd_puts2(bot_buff);
	}else if(set_time_mode == true){
		lcd_pos(0,1);
		sprintf(top_buff, "%02d/%02d/%02d SET",tm.year,tm.month, tm.day);
		lcd_puts2(top_buff);
		lcd_pos(1,1);
		sprintf(bot_buff, "%02d:%02d:%02d",tm.hour,tm.minute, tm.second);
		lcd_puts2(bot_buff);
	}else{ //else and worldclockmode
		lcd_pos(0,1);
		if(world_clock_mode == true)
			sprintf(top_buff, "%02d/%02d/%02d WLD",tm.year,tm.month, tm.day);
		else
			sprintf(top_buff, "%02d/%02d/%02d",tm.year,tm.month, tm.day);
		lcd_puts2(top_buff);
		lcd_pos(1,1);
		if(military_time == true)
			sprintf(bot_buff, "%02d:%02d:%02d",time_in_12,tm.minute, tm.second);
		else
			sprintf(bot_buff, "%02d:%02d:%02d",tm.hour,tm.minute, tm.second);
		lcd_puts2(bot_buff);	
	}
}
void initial_time(){
	tm.year = 2020;
	tm.month = 3;
	tm.day = 20;
	tm.hour = 0;
	tm.minute = 0;
	tm.second = 5;
	
	time_in_12 = tm.hour%12;
}

void autoModify(struct time_variable *t){
	
	int i =(((t->year % 4 == 0) && (t->year % 100 != 0)) ||(t->year % 400 == 0))?1:0;
	leap_year = ((i==1)?true:false);
	 
	if(t->second>=60){
		t->second-=60;
		t->minute++;
	}
	if(t->minute>=60){
		t->minute-=60;
		t->hour++;
	}
	if(t->hour>=12){
		time_in_12 = t->hour%12;
	}
	if(t->hour>=24){
		t->hour-=24;
		t->day++;
	}else if(t->hour<0){
		t->day--;
		t->hour+=24;
	}
	//decreasing mode on h/m/s
	if(t->second<0){
		t->second+=60;
		t->minute--;
		if(t->minute<0){
			t->minute+=60;
			t->hour--;
			if(t->hour<0){
				t->hour+=24;
				t->day--;
			}
		}
	}
	
	//decreasing mode on y/m/d
	if(t->day<1){
		if(t->month == 3){
			if(leap_year == true){
				t->day = 29;
				t->month--;	
			}else{
				t->day = 28;
				t->month--;
			}
		}else if(t->month == 2 || t->month == 4 || t->month == 6 || t->month == 9 || t->month == 11){
			t->day = 31;
			t->month--;
		}else if(t->month == 12 || t->month == 10 || t->month == 7 || t->month == 5){
			t->day = 30;
			t->month--;
		}else{//January
			t->year--;
			t->month = 12;
			t->day = 31;
		}
	}
	
	//increasing mode
	if(t->month == 2){
		if(leap_year == false){
			if(t->day>28){
				t->day = 1;
				t->month++;
			}
		}else{
			if(t->day==30){
				t->day = 1;
				t->month++;
			}
		}
	}else if(t->month == 4 ||t->month == 6 ||t->month == 9 ||t->month == 11){
		if(t->day>30){
			t->day = 1;
			t->month++;
		}
	}else{
		if(t->day>31){
			t->day = 1;
			t->month++;
		}
	}
	if(t->month>=13){
		t->month=1;
		t->year++;
	}	
}

void alarm_play(){
	alarm_activation = true;
	int i;
	int song_len = 0;
	if(song_index == 0){
		song_len = numNote1;
	}else if(song_index == 1){
		song_len = numNote2;
	}else{
		song_len = numNote3;
	}
	song_index++;
	 
	lcd_display();
	
	int k = 0;
	for(i = 0; i < song_len; i++){
		
		led_alarm(k);
		if(k == 0){
			k = 1;
		}else{
			k = 0;
		}
		
		if(song_index == 0){
			play_note((int)(SONG1[i].freq), SONG1[i].duration*1000);
		}else if(song_index == 1){
			play_note((int)(SONG2[i].freq), SONG2[i].duration*1000);
		}else{
			play_note((int)(SONG3[i].freq), SONG3[i].duration*1000);
		}
		if(get_key() == 16){
			SPK_OFF;
			break;
		}
	}
	led_off();
	alarm_activation = false;
	
}

void play_note(int freq, int dur){
	double period = 10000.0/(double)freq;
	int i, t = period/2;
	int cycle= dur/period;
	
	for(i = 0; i < cycle; i++){
		SPK_ON;
		avr_wait_music(t);
		SPK_OFF;
		avr_wait_music(t);
	}
}

int is_pressed(int r, int c){
	//set all
	DDRC = 0;
	PORTC = 0;
	
	CLR_BIT(DDRC,r);
	SET_BIT(PORTC,r);
	
	SET_BIT(DDRC,c+4);
	CLR_BIT(PORTC,c+4);
	return (GET_BIT(PINC,r))?0:1; //if r bit is 0 return 1 else return 0
}

int get_key(void){
	int r, c;
	for(r = 0; r < 4; r++){
		for(c = 0; c < 4; c++){
			if(is_pressed(r,c)){
				return (r*4)+c+1;
			}
		}
	}
	return 0;
}
