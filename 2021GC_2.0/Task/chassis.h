#ifndef _CHASSIS_H
#define _CHASSIS_H


void chassis_init(void);
void chassis_task(void);

typedef struct
{
	
	float Va;
	float Vd;
	float Vw;
	float Vs;
	
	float set_x;
	float set_y;
	
}key_chassis;





#endif
