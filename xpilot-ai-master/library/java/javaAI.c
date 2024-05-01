//Xpilot-AI Team 2012
//Xpilot-AI Team 2011
//from xpilot.c -EGG
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#ifndef _WINDOWS
# include <unistd.h>
# ifndef __hpuxf
#  include <sys/time.h>
# endif
# include <sys/param.h>
# include <netdb.h>
#endif
#ifdef _WINDOWS
# include "NT/winNet.h"
# include "NT/winClient.h"
#endif
#include "version.h"
#include "config.h"
#include "const.h"
#include "types.h"
#include "pack.h"
#include "bit.h"
#include "error.h"
#include "socklib.h"
#include "net.h"
#include "connectparam.h"
#include "protoclient.h"
#include "portability.h"
#include "checknames.h"
#include "commonproto.h"
//end from xpilot.c -EGG
//from xpclient_x11.h -EGG
#include "clientAI.h" //originally xpclient.h -EGG
#ifdef HAVE_X11_X_H
#  include <X11/X.h>
#endif
#ifdef HAVE_X11_XLIB_H
#  include <X11/Xlib.h>
#endif
#ifdef HAVE_X11_XOS_H
#  include <X11/Xos.h>
#endif
#ifdef HAVE_X11_XUTIL_H
#  include <X11/Xutil.h>
#endif
#ifdef HAVE_X11_KEYSYM_H
#  include <X11/keysym.h>
#endif
#ifdef HAVE_X11_XATOM_H
#  include <X11/Xatom.h>
#endif
#ifdef HAVE_X11_XMD_H
#  include <X11/Xmd.h>
#endif
/* X client specific headers */
#ifndef _WINDOWS
#include <X11/Xlib.h> //for X graphics -EGG
#include <X11/keysym.h> //for X keys -EGG
#endif
#include "blockbitmaps.h" //originally bitmaps.h -EGG
#include "dbuff.h"
#include "paint.h" //originally xpaint.h, moved from below xinit.h -EGG
#include "paintdata.h"
#include "record.h"
#include "widget.h"
#include "../common/keys.h"
#include "xevent.h"
//#include "xeventhandlers.h"
#include "xinit.h"
//end from xpclient_x11.h -EGG
//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <sys/mman.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include "xpclient_x11.h"
#include <math.h>
#include "../common/rules.h"
#include "../common/setup.h"
#include "configure.h"
#include "netclient.h"
#ifdef _WINDOWS
# include "../common/NT/winXKey.h" //added for XK_keys
#endif
#define BASE_X(i)	(((i % x_areas) << 8) + ext_view_x_offset)
#define BASE_Y(i)	((ext_view_height - 1 - (((i / x_areas) % y_areas) << 8)) - ext_view_y_offset)
#define PI_AI 3.1415926536
#define sgn(x) ((x<0)?-1:((x>0)?1:0)) //Returns the sign of a number
#define AI_MSGLEN   256 //Max length of a message 
#define AI_MSGMAX   16 //Size of (incoming) message buffer - default maxMessage is 8
#ifdef _WINDOWS
# define XK_Shift_L 0xFFE1
# define XK_Control_L 0xFFE3
#endif
//Added for headless -EGG
int headless = 0;
//Defined some stuff to avoid undefined warnings -EGG
extern int mainAI(int argc, char* argv[]);
message_t *TalkMsg[MAX_MSGS], *GameMsg[MAX_MSGS];
score_object_t	score_objects[MAX_SCORE_OBJECTS];
double Java_javaAI_selfHeadingDeg(JNIEnv * env, jobject jobj);
struct AI_msg_struct {
    char body[AI_MSGLEN];
    char from[32];
    char to[32];
} AI_msg[AI_MSGMAX];
//Ship stuff -JNE
//Stores all the information that gets updated in the ship buffer -JNE
typedef struct {
    double vel;
    double xVel;
    double yVel;
    double trackingDeg;
    double trackingRad;
    double d;
    int reload;
    ship_t ship;
} shipData_t;
shipData_t allShips[128][3];
int prevFrameShips = 0;
//Shot tracking stuff -EGG
//Can currently track up to 100 shots. -EGG
#define AISHOT_MAX		100	/*max number of shots */
struct AIshot_struct {
    int x;
    int y;
    int dist;
    int xdir;
    int vel;
    int veldir;
    int imaginary;		/* 1 if just a predicted shot and 0 if a real shot */
    int fresh_shot;		/* -1 if not, and index pointer of ship who shot if */
    int idir;			/* direction from you shot will intercept nearest */
    int idist;			/* intercept distance */
    int itime;			/* time to intercept */
    int alert;			/*MIN(yatx + timex, xaty + timey) */
    int id;			/*bullet id */
} AIshot[AISHOT_MAX];
struct AIshot_struct AIshot_buffer[AISHOT_MAX];
//Our many thanks to Darel Rex Finley for the quickSort we based this on - <3 the xpilot-ai team
//quickSort
//
//This public-domain C implementation by Darel Rex Finley.
//
//* Returns YES if sort was successful, or NO if the nested
//pivots went too deep, in which case your array will have
//been re-ordered, but probably not sorted correctly.
//
//* This function assumes it is called with valid parameters.
//
//* Example calls:
//quickSort(&myArray[0],5); //sorts elements 0, 1, 2, 3, and 4
//quickSort(&myArray[3],5); //sorts elements 3, 4, 5, 6, and 7
int quickSortShots() {
    #define  MAX_LEVELS  1000
    struct AIshot_struct piv;
    int  beg[MAX_LEVELS], end[MAX_LEVELS], i=0, L, R ;
    while (AIshot_buffer[i].x != -1) i++;
    beg[0]=0; end[0]=i; i=0;
    while (i>=0) {
        L=beg[i]; R=end[i]-1;
        if (L<R) {
            piv=AIshot_buffer[L]; if (i==MAX_LEVELS-1) return -1;
            while (L<R) {
                while (AIshot_buffer[R].alert>=piv.alert && L<R) R--; if (L<R) AIshot_buffer[L++]=AIshot_buffer[R];
                while (AIshot_buffer[L].alert<=piv.alert && L<R) L++; if (L<R) AIshot_buffer[R--]=AIshot_buffer[L]; }
            AIshot_buffer[L]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
        else {
            i--; }}
    return 1; }
//
//START FROM OLD AI CODE
//
//AI.h
struct AIshot_image_struct {
    int x;
    int y;
} AIshot_image[3][AISHOT_MAX];
int AI_delaystart;
int AI_shotspeed;
int AI_repeatrate;
int AIshot_toggle;
int AIshot_previoustoggle;
int AIshot_shipping;
int AI_alerttimemult;
float AI_degree(float radian) {
    return (360.0 * radian) / (2.0 * PI_AI);
}
int AI_distance(int x1, int y1, int x2, int y2) {
    return (int) sqrt(sqr(abs(x1 - x2)) + sqr(abs(y1 - y2)));
}
float AI_radian(float degree) {
    return 2.0 * PI_AI / 360.0 * degree;
}
void AIshot_reset() {
    int i = 0;
    while (i < AISHOT_MAX && AIshot[i].x != -1) {
	AIshot[i].x = -1;
	AIshot[i].y = -1;
	AIshot[i].dist = -1;
	AIshot[i].xdir = -1;
	AIshot[i].vel = -1;
	AIshot[i].veldir = -1;
	AIshot[i].idir = -1;
	AIshot[i].idist = -1;
	AIshot[i].itime = -1;
	AIshot[i].imaginary = -1;
	AIshot[i].fresh_shot = -1;
	AIshot[i].alert = -1;
	i++;
    }
    return;
}
void AIshot_buffer_reset() {
    int i = 0;
    while (i < AISHOT_MAX && AIshot_buffer[i].x != -1) {
	AIshot_buffer[i].x = -1;
	AIshot_buffer[i].y = -1;
	AIshot_buffer[i].dist = -1;
	AIshot_buffer[i].xdir = -1;
	AIshot_buffer[i].vel = -1;
	AIshot_buffer[i].veldir = -1;
	AIshot_buffer[i].idir = -1;
	AIshot_buffer[i].idist = -1;
	AIshot_buffer[i].itime = -1;
	AIshot_buffer[i].imaginary = -1;
	AIshot_buffer[i].fresh_shot = -1;
	AIshot_buffer[i].alert = -1;
	i++;
    }
    return;
}
void AIshot_image_reset(int index) {
    int i = index;
    while (i < AISHOT_MAX && AIshot_image[index][i].x != -1) {
	AIshot_image[index][i].x = -1;
	AIshot_image[index][i].y = -1;
	i++;
    }
    return;
}
int AIshot_calcxdir(int si) {
    if (AIshot[si].y - pos.y < 0) {
	return (int) (360.0 -
		      (360.0 / (2.0 * PI_AI)) *
		      acos((float)
			   ((float) (AIshot[si].x - pos.x) /
			    (float) AIshot[si].dist)));
    }
    else {
	return (int) (360.0 / (2.0 * PI_AI)) *
	    acos((float)
		 ((float) (AIshot[si].x - pos.x) /
		  (float) AIshot[si].dist));
    }
}
void AIshot_refresh() {
    int i;
    if (AIshot_toggle > 0) {
	i = 1;
	while (i == -1)
	    i = quickSortShots();
	AIshot_reset();
	i = 0;
	while (i < AISHOT_MAX && AIshot_buffer[i].x != -1) {
	    AIshot[i].x = AIshot_buffer[i].x;
	    AIshot[i].y = AIshot_buffer[i].y;
	    AIshot[i].dist =
		sqrt(sqr(abs(AIshot[i].x - pos.x)) +
		     sqr(abs(AIshot[i].y - pos.y)));
	    AIshot[i].xdir = AIshot_calcxdir(i);
	    AIshot[i].vel = AIshot_buffer[i].vel;
	    AIshot[i].veldir = AIshot_buffer[i].veldir;
	    AIshot[i].itime = AIshot_buffer[i].itime;
	    AIshot[i].imaginary = AIshot_buffer[i].imaginary;
	    AIshot[i].fresh_shot = AIshot_buffer[i].fresh_shot;
	    AIshot[i].idist = AIshot_buffer[i].idist;
	    AIshot[i].idir = AIshot_buffer[i].idir;

	    AIshot[i].alert = AIshot_buffer[i].alert;
	    i++;
	}
	AIshot_image_reset(2);
	i = 0;
	while (i < AISHOT_MAX && AIshot_image[1][i].x != -1) {
	    AIshot_image[2][i].x = AIshot_image[1][i].x;
	    AIshot_image[2][i].y = AIshot_image[1][i].y;
	    i++;
	}
	AIshot_image_reset(1);
	i = 0;
	while (i < AISHOT_MAX && AIshot_image[0][i].x != -1) {
	    AIshot_image[1][i].x = AIshot_image[0][i].x;
	    AIshot_image[1][i].y = AIshot_image[0][i].y;
	    i++;
	}
	AIshot_image_reset(0);
	AIshot_buffer_reset();
	AIshot_toggle = 0;
    }
    return;
}
float AIshot_calcVel(int newX, int newY, int oldX, int oldY) {
    if (oldX == -1 && oldY == -1)
	return -1.0;
    else
	return sqrt(sqr(fabs((float) newX - (float) oldX)) +
		    sqr(fabs((float) newY - (float) oldY)));
}
float AIshot_calcVelDir(int newX, int newY, int oldX, int oldY, float vel) {
    if (oldX == -1)
	return -1.0;
    else if (vel > 0.0) {
	if (newY - oldY < 0) {
	    return (360.0 -
		    (360.0 / (2.0 * PI_AI)) *
		    acos((float) ((float) (newX - oldX) / vel)));
	}
	else {
	    return (360.0 / (2.0 * PI_AI)) *
		acos((float) ((float) (newX - oldX) / vel));
	}
    }
    else
	return 0.0;
}
void AIshot_addtobuffer(int x, int y, float vel, float veldir, int imaginary, int fresh_shot) {
    float theta1, theta2;
    float A, B, C, BAC;
    int newx1, newx2, newy1, newy2;
    int i = 0;
    while (i < AISHOT_MAX && AIshot_buffer[i].x != -1)
	i++;
    AIshot_buffer[i].x = x;
    AIshot_buffer[i].y = y;
    AIshot_buffer[i].dist =
                     (int) sqrt(sqr(abs(AIshot_buffer[i].x - pos.x)) +
		     sqr(abs(AIshot_buffer[i].y - pos.y)));
    AIshot_buffer[i].vel = (int) vel;
    AIshot_buffer[i].veldir = (int) veldir;
    AIshot_buffer[i].imaginary = imaginary;
    AIshot_buffer[i].fresh_shot = fresh_shot;
    theta1 = AI_radian((float) selfTrackingDeg());
    theta2 = AI_radian(veldir);
    A = pow(selfSpeed() * sin(theta1) - vel * sin(theta2),
	    2) + pow(selfSpeed() * cos(theta1) - vel * cos(theta2), 2);
    B = 2 * ((pos.y - y) *
	     (selfSpeed() * sin(theta1) - vel * sin(theta2)) + (pos.x - x) *
	     (selfSpeed() * cos(theta1) - vel * cos(theta2)));
    C = pow(pos.x - x, 2) + pow(pos.y - y, 2);
    BAC = pow(B, 2) - 4 * A * C;
    if (BAC >= 0) {
	BAC = (-1 * B + pow(BAC, .5));
	if ((BAC / (2 * A)) < 0)
	    BAC = (-1 * B - pow(pow(B, 2) - 4 * A * C, .5));
	AIshot_buffer[i].itime = BAC / (2 * A);
	AIshot_buffer[i].idist = 777;
	AIshot_buffer[i].idir = 777;
    }
    else {
	AIshot_buffer[i].itime = (-1 * B) / (2 * A);
	AIshot_buffer[i].idist = C - pow(B, 2) / (4 * A);
	AIshot_buffer[i].idir = 777;
    }
    newx1 = pos.x + selfSpeed() * cos(theta1) * AIshot_buffer[i].itime;
    newx2 = x + vel * cos(theta2) * AIshot_buffer[i].itime;
    newy1 = pos.y + selfSpeed() * sin(theta1) * AIshot_buffer[i].itime;
    newy2 = y + vel * sin(theta2) * AIshot_buffer[i].itime;
    AIshot_buffer[i].idist = AI_distance(newx1, newy1, newx2, newy2);
    if ((newy2 - newy1) < 0) {
	AIshot_buffer[i].idir =
	    (int) (360.0 -
		   (360.0 / (2.0 * PI_AI)) *
		   acos((float)
			((float) (newx2 - newx1) /
			 (float) AIshot_buffer[i].idist)));
    }
    else {
	AIshot_buffer[i].idir =
	    (int) (360.0 / (2.0 * PI_AI)) *
	    acos((float)
		 ((float) (newx2 - newx1) /
		  (float) AIshot_buffer[i].idist));
    }
    AIshot_buffer[i].alert =
	abs(AIshot_buffer[i].idist +
	    (int) (AIshot_buffer[i].itime * AI_alerttimemult));
    if (AIshot_buffer[i].itime < 0 || AIshot_buffer[i].itime == 0) {
	AIshot_buffer[i].alert = 30000;
    }
    AIshot[i].id = i;
    return;
}
/* this calculates the velocity and direction of each shot by comparing all possible velocites and directions for three images of shot locations: present, first past, second past.  If it can't find two matching velocites and directions in a row for a present shot, it will check to see if a ship was there two frames ago that could have shot it.  works farely well but occasionally there are some bad calculations where shots that don't really exist are added*/
void AIshot_addshot(int px, int py) {
    int i1, i2;
    float tempvel1, tempdir1, tempvel2, tempdir2;
    int found = 0;
    int i = 0;
    if (AIshot_toggle == 0) AIshot_reset();
    while (AIshot_image[0][i].x != -1 && i < AISHOT_MAX) i++;
    AIshot_image[0][i].x = pos.x - ext_view_width / 2 + WINSCALE(px);
    AIshot_image[0][i].y = pos.y + ext_view_height / 2 - WINSCALE(py);
    i1 = 0;
    while (i1 < AISHOT_MAX && AIshot_image[1][i1].x != -1) {
	tempvel1 =
	    AIshot_calcVel(AIshot_image[0][i].x, AIshot_image[0][i].y,
			   AIshot_image[1][i1].x, AIshot_image[1][i1].y);
	tempdir1 =
	    AIshot_calcVelDir(AIshot_image[0][i].x, AIshot_image[0][i].y,
			      AIshot_image[1][i1].x, AIshot_image[1][i1].y,
			      tempvel1);
	i2 = 0;
	while (i2 < AISHOT_MAX && AIshot_image[2][i2].x != -1) {
	    tempvel2 =
		AIshot_calcVel(AIshot_image[1][i1].x,
			       AIshot_image[1][i1].y,
			       AIshot_image[2][i2].x,
			       AIshot_image[2][i2].y);
	    if (fabs(tempvel1 - tempvel2) < 6.0) {
		tempdir2 =
		    AIshot_calcVelDir(AIshot_image[1][i1].x,
				      AIshot_image[1][i1].y,
				      AIshot_image[2][i2].x,
				      AIshot_image[2][i2].y, tempvel2);
		if (fabs(tempdir1 - tempdir2) < 6.0) {
		    AIshot_addtobuffer(AIshot_image[0][i].x,
				       AIshot_image[0][i].y,
				       (tempvel1 + tempvel2) / 2.0,
				       (tempdir1 + tempdir2) / 2.0, 0, -1);
		    found = 1;
		    i2 = AISHOT_MAX;
		    i1 = AISHOT_MAX;
		}
	    }
	    i2++;
	}
	if (found == 0) { 
	    i2 = 0;
	    while (i2 < num_ship) {
		tempvel2 =
		    AIshot_calcVel(AIshot_image[1][i1].x,
				   AIshot_image[1][i1].y,
				   allShips[i2][2].ship.x +
				   (int) (18.0 *
					  cos(AI_radian
					      ((float)
					       allShips[i2][2].ship.dir))),
				   allShips[i2][2].ship.y +
				   (int) (18.0 *
					  sin(AI_radian
					      ((float)
					       allShips[i2][2].ship.dir))));
		if (fabs(tempvel1 - tempvel2) < 17.0) {
		    tempdir2 =
			AIshot_calcVelDir(AIshot_image[1][i1].x,
					  AIshot_image[1][i1].y,
					  allShips[i2][2].ship.x +
					  (int) (18.0 *
						 cos(AI_radian
						     ((float)
						      allShips[i2][2].ship.dir))),
					  allShips[i2][2].ship.y +
					  (int) (18.0 *
						 sin(AI_radian
						     ((float)
						      allShips[i2][2].ship.dir))),
					  tempvel2);
		    if (fabs(tempdir1 - tempdir2) < 17.0) {
			AIshot_addtobuffer(AIshot_image[0][i].x,
					   AIshot_image[0][i].y,
					   (tempvel1 + tempvel2) / 2.0,
					   (tempdir1 + tempdir2) / 2.0, 0,
					   (int)&allShips[i2][2].ship);
			allShips[i2][0].reload = AI_repeatrate - 1; 
			found = 1;
			i2 = num_ship;
			i1 = AISHOT_MAX;
		    }
		}
		i2++;
	    }
	}
	i1++;
    }
    AIshot_toggle++;
    return;
}
//
//END FROM OLD AI CODE
//
void prepareShots() {
    int num_shots = 0;
    int i, x_areas, y_areas, areas, max_;
    AIshot_refresh();
    x_areas = (active_view_width + 255) >> 8;
    y_areas = (active_view_height + 255) >> 8;
    areas = x_areas * y_areas;
    max_ = areas * (num_spark_colors >= 3 ? num_spark_colors : 4);
    for (i = 0; i < max_; i++) {
        int x, y, j;
        if (num_fastshot[i] > 0) {
            x = BASE_X(i);
            y = BASE_Y(i);
            for (j = 0; j < num_fastshot[i]; j++) {
                num_shots++;
                if (num_shots < 100) AIshot_addshot(x + fastshot_ptr[i][j].x, y - fastshot_ptr[i][j].y);
                else printf("HOLY SHOTS, BATMAN! There are more than 100 shots on screen, we can't track that yet!\n");
            }
        }
    }
    if (quickSortShots() == -1) printf("OH NOES! Unable to sort the shots with given MAX_LEVELS");
}
//END Shot tracking stuff -EGG
//From xpilot-ng's event.c to make key functions easier -EGG
typedef int xp_keysym_t;
void Keyboard_button_pressed(xp_keysym_t ks)
{
    bool change = false;
    keys_t key;
    for (key = Lookup_key(NULL, ks, true);
	 key != KEY_DUMMY;
	 key = Lookup_key(NULL, ks, false))
	change |= Key_press(key);
    if (change)
	Net_key_change();
}
void Keyboard_button_released(xp_keysym_t ks)
{
    bool change = false;
    keys_t key;
    for (key = Lookup_key(NULL, ks, true);
	 key != KEY_DUMMY;
	 key = Lookup_key(NULL, ks, false))
	change |= Key_release(key);
    if (change)
	Net_key_change();
}
//END from event.c
//The initial JNIEnv and jobject will be stored here -EGG
JNIEnv * prime_env;
jobject prime_jobj;
//Reload tracker
int reload = 0;
//HERE LIES THE JAVA BINDINGS -EGG
// All button press methods are documented on 
JNIEXPORT void JNICALL Java_javaAI_turnLeft(JNIEnv * env, jobject jobj, int flag) {	// turns left as if the 'a' key was pressed -JNE
    if (flag) 
        Keyboard_button_pressed(XK_a);
    else
        Keyboard_button_released(XK_a);
}
JNIEXPORT void JNICALL Java_javaAI_turnRight(JNIEnv * env, jobject jobj, int flag) { // turns right as if the 's' key was pressed -JNE
    if (flag) 
        Keyboard_button_pressed(XK_s);
    else
        Keyboard_button_released(XK_s);
}
JNIEXPORT void JNICALL Java_javaAI_turn(JNIEnv * env, jobject jobj, int deg) {	// turns based on the speed, 'deg', that is passed in -JNE
     deg = deg<0 ? MAX(deg, -MAX_PLAYER_TURNSPEED) : MIN(deg, MAX_PLAYER_TURNSPEED);
     if (deg)
	  Send_pointer_move(deg * -128 / 360);
}
JNIEXPORT void JNICALL Java_javaAI_turnToDeg(JNIEnv * env, jobject jobj, int deg) {
	// sets the ship's heading to a fixed degree -JNE
	int selfHead;
	selfHead = (int)Java_javaAI_selfHeadingDeg(prime_env, prime_jobj);
	int speed;
	int dif;
	dif = abs(deg-selfHead);
	if (dif != 0) {			// sets speed depending on how close it is to the target angle -JNE
		if (dif > 20 && dif < 340) {
			speed = 64;
		}
		else if (dif >= 2 && dif <= 358) {
			speed = 8;
		}
		else {		// heading in Xpilot goes in increments of 2.8125 (because it uses a scale of 0-127 instead of 0-360), so we stop if we're close -JNE
			speed = 0;
		}
		if (deg < 180) {
			if (selfHead-deg<180 && selfHead-deg>0) {
				Java_javaAI_turn(prime_env,prime_jobj,-speed);
			}
			else {
				Java_javaAI_turn(prime_env,prime_jobj,speed);
			}
		}
		else if (deg >= 180) {
			if (deg-selfHead<180 && deg-selfHead>0) {
				Java_javaAI_turn(prime_env,prime_jobj,speed);
			}
			else {
				Java_javaAI_turn(prime_env,prime_jobj,-speed);
			}
		}
	}
}
JNIEXPORT void JNICALL Java_javaAI_thrust(JNIEnv * env, jobject jobj, int flag) {
    if (flag) 
        Keyboard_button_pressed(XK_Shift_L);
    else
        Keyboard_button_released(XK_Shift_L);
}
//Sets the player's turnspeed. -EGG
//Will not take effect until the player STARTS turning AFTER this is called. -EGG
//Parameters: int for speed, min = 0, max = 64. -EGG
void setTurnSpeed(double s) {
    turnspeed = s;
    turnspeed = MIN(turnspeed, MAX_PLAYER_TURNSPEED);
    turnspeed = MAX(turnspeed, MIN_PLAYER_TURNSPEED);
    Send_turnspeed(turnspeed);
    Config_redraw();
    control_count = CONTROL_DELAY;
}
//Sets the player's turnspeed. -EGG
//Will not take effect until the player STARTS turning AFTER this is called. -EGG
//Parameters: int for speed, min = 0, max = 64. -EGG
JNIEXPORT void JNICALL Java_javaAI_setTurnSpeed(JNIEnv * env, jobject jobj, double s) {
    turnspeed = s;
    turnspeed = MIN(turnspeed, MAX_PLAYER_TURNSPEED);
    turnspeed = MAX(turnspeed, MIN_PLAYER_TURNSPEED);
    Send_turnspeed(turnspeed);
    Config_redraw();
    control_count = CONTROL_DELAY;
}
//8/3 magical XPilot units = 1 degree -EGG Java conversion -CJG
JNIEXPORT void JNICALL Java_javaAI_setTurnSpeedDeg(JNIEnv * env, jobject jobj, int s) {
    setTurnSpeed((8.0/3.0)*(double)s);
}
JNIEXPORT void JNICALL Java_javaAI_setPower(JNIEnv * env, jobject jobj, double s) {
    power = s;
    power = MIN(power, MAX_PLAYER_POWER);
    power = MAX(power, MIN_PLAYER_POWER);
    Send_power(power);
    Config_redraw();
    control_count = CONTROL_DELAY;
}
JNIEXPORT void JNICALL Java_javaAI_fasterTurnrate(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_Add);
    Keyboard_button_released(XK_KP_Add);
}
JNIEXPORT void JNICALL Java_javaAI_slowerTurnrate(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_Subtract);
    Keyboard_button_released(XK_KP_Subtract);
}
JNIEXPORT void JNICALL Java_javaAI_morePower(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_Multiply);
    Keyboard_button_released(XK_KP_Multiply);
}
JNIEXPORT void JNICALL Java_javaAI_lessPower(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_Divide);
    Keyboard_button_released(XK_KP_Divide);
}
// End movement methods -JNE
// Shooting methods -JNE
JNIEXPORT void JNICALL Java_javaAI_fireShot(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Return);
    Keyboard_button_released(XK_Return);
    if (reload==0) reload = AI_repeatrate - 1;
}
JNIEXPORT void JNICALL Java_javaAI_fireMissile(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_backslash);
    Keyboard_button_released(XK_backslash);
}
JNIEXPORT void JNICALL Java_javaAI_fireTorpedo(JNIEnv * env, jobject jobj) { 
    Keyboard_button_pressed(XK_apostrophe);
    Keyboard_button_released(XK_apostrophe);
}
JNIEXPORT void JNICALL Java_javaAI_fireHeat(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_semicolon);
    Keyboard_button_released(XK_semicolon);
}
JNIEXPORT void JNICALL Java_javaAI_dropMine(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Tab);
    Keyboard_button_released(XK_Tab);
}
JNIEXPORT void JNICALL Java_javaAI_detachMine(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_bracketright);
    Keyboard_button_released(XK_bracketright);
}
JNIEXPORT void JNICALL Java_javaAI_detonateMines(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_equal);
    Keyboard_button_released(XK_equal);
}
JNIEXPORT void JNICALL Java_javaAI_fireLaser(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_slash);
    Keyboard_button_released(XK_slash);
}
// End shooting methods -JNE
// Item usage methods -JNE
JNIEXPORT void JNICALL Java_javaAI_tankDetach(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_r);
    Keyboard_button_released(XK_r);
}
JNIEXPORT void JNICALL Java_javaAI_cloak(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_BackSpace);
    Keyboard_button_released(XK_BackSpace);
}
JNIEXPORT void JNICALL Java_javaAI_ecm(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_bracketleft);
    Keyboard_button_released(XK_bracketleft);
}
JNIEXPORT void JNICALL Java_javaAI_transporter(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_t);
    Keyboard_button_released(XK_t);
}
JNIEXPORT void JNICALL Java_javaAI_tractorBeam(JNIEnv * env, jobject jobj, int flag) {
    if (flag)
        Keyboard_button_pressed(XK_comma);
    else
        Keyboard_button_released(XK_comma);
}
JNIEXPORT void JNICALL Java_javaAI_pressorBeam(JNIEnv * env, jobject jobj, int flag) {
    if (flag)
        Keyboard_button_pressed(XK_period);
    else
        Keyboard_button_released(XK_period);
}
JNIEXPORT void JNICALL Java_javaAI_phasing(JNIEnv * env, jobject jobj){
	Keyboard_button_pressed(XK_p);
	Keyboard_button_released(XK_p);
}
JNIEXPORT void JNICALL Java_javaAI_shield(JNIEnv * env, jobject jobj, int flag) {
    if (flag)
       Keyboard_button_pressed(XK_space);
    else
        Keyboard_button_released(XK_space);
}
JNIEXPORT void JNICALL Java_javaAI_emergencyShield(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_g);
    Keyboard_button_released(XK_g);
}
JNIEXPORT void JNICALL Java_javaAI_hyperjump(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_q);
    Keyboard_button_released(XK_q);
}
JNIEXPORT void JNICALL Java_javaAI_nextTank(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_e);
    Keyboard_button_released(XK_e);
}
JNIEXPORT void JNICALL Java_javaAI_prevTank(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_w);
    Keyboard_button_released(XK_w);
}
JNIEXPORT void JNICALL Java_javaAI_toggleAutopilot(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_h);
    Keyboard_button_released(XK_h);
}
JNIEXPORT void JNICALL Java_javaAI_emergencyThrust(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_j);
    Keyboard_button_released(XK_j);
}
JNIEXPORT void JNICALL Java_javaAI_deflector(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_0);
    Keyboard_button_released(XK_0);
}
JNIEXPORT void JNICALL Java_javaAI_selectItem(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_0);
    Keyboard_button_released(XK_KP_0);
}
JNIEXPORT void JNICALL Java_javaAI_loseItem(JNIEnv * env, jobject jobj) { 
    Keyboard_button_pressed(XK_KP_Decimal);
    Keyboard_button_released(XK_KP_Decimal);
}
// End item usage methods -JNE
// Lock methods -JNE
JNIEXPORT void JNICALL Java_javaAI_lockNext(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Right);
    Keyboard_button_released(XK_Right);
}
JNIEXPORT void JNICALL Java_javaAI_lockPrev(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Left);
    Keyboard_button_pressed(XK_Left);
}
JNIEXPORT void JNICALL Java_javaAI_lockClose(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Up);
    Keyboard_button_released(XK_Up);
}
JNIEXPORT void JNICALL Java_javaAI_lockNextClose(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Down);
    Keyboard_button_released(XK_Down);
}
JNIEXPORT void JNICALL Java_javaAI_loadLock1(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_5);
	Keyboard_button_released(XK_5);
}
JNIEXPORT void JNICALL Java_javaAI_loadLock2(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_6);
	Keyboard_button_released(XK_6);
}
JNIEXPORT void JNICALL Java_javaAI_loadLock3(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_7);
	Keyboard_button_released(XK_7);
}
JNIEXPORT void JNICALL Java_javaAI_loadLock4(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_8);
	Keyboard_button_released(XK_8);
}
// End lock methods -JNE
// Modifier methods -JNE
JNIEXPORT void JNICALL Java_javaAI_toggleNuclear(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_n);
    Keyboard_button_released(XK_n);
}
JNIEXPORT void JNICALL Java_javaAI_togglePower(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_b);
    Keyboard_button_released(XK_b);
}
JNIEXPORT void JNICALL Java_javaAI_toggleVelocity(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_v);
    Keyboard_button_released(XK_v);
}
JNIEXPORT void JNICALL Java_javaAI_toggleCluster(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_c);
    Keyboard_button_released(XK_c);
}
JNIEXPORT void JNICALL Java_javaAI_toggleMini(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_x);
    Keyboard_button_released(XK_x);
}
JNIEXPORT void JNICALL Java_javaAI_toggleSpread(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_z);
    Keyboard_button_released(XK_z);
}
JNIEXPORT void JNICALL Java_javaAI_toggleLaser(JNIEnv * env, jobject jobj) {
	Keyboard_button_pressed(XK_l);
	Keyboard_button_released(XK_l);
}
JNIEXPORT void JNICALL Java_javaAI_toggleImplosion(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_i);
    Keyboard_button_released(XK_i);
}
JNIEXPORT void JNICALL Java_javaAI_toggleUserName(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_u);
    Keyboard_button_released(XK_u);
}
JNIEXPORT void JNICALL Java_javaAI_loadModifiers1(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_1);
    Keyboard_button_released(XK_1);
}
JNIEXPORT void JNICALL Java_javaAI_loadModifiers2(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_2);
    Keyboard_button_released(XK_2);
}
JNIEXPORT void JNICALL Java_javaAI_loadModifiers3(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_3);
    Keyboard_button_released(XK_3);
}
JNIEXPORT void JNICALL Java_javaAI_loadModifiers4(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_4);
    Keyboard_button_released(XK_4);
}
JNIEXPORT void JNICALL Java_javaAI_clearModifiers(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_k);
    Keyboard_button_released(XK_k);
}
// End modifier methods -JNE
// Map features -JNE
JNIEXPORT void JNICALL Java_javaAI_connector(JNIEnv * env, jobject jobj, int flag) {
    if (flag)
        Keyboard_button_pressed(XK_Control_L);
    else
        Keyboard_button_released(XK_Control_L);
}
JNIEXPORT void JNICALL Java_javaAI_dropBall(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_d);
    Keyboard_button_released(XK_d);
}
JNIEXPORT void JNICALL Java_javaAI_refuel(JNIEnv * env, jobject jobj, int flag) {
    if (flag)
        Keyboard_button_pressed(XK_Control_L);
    else
        Keyboard_button_released(XK_Control_L);
}
// End map features -JNE
// Other options -JNE

JNIEXPORT void JNICALL Java_javaAI_keyHome(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Home);
    Keyboard_button_released(XK_Home);
}
JNIEXPORT void JNICALL Java_javaAI_selfDestruct(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_End);
    Keyboard_button_released(XK_End);
}
JNIEXPORT void JNICALL Java_javaAI_pauseAI(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Pause);
    Keyboard_button_released(XK_Pause);
}
JNIEXPORT void JNICALL Java_javaAI_swapSettings(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_Escape);
	Keyboard_button_released(XK_Escape);
}
JNIEXPORT void JNICALL Java_javaAI_quitAI(JNIEnv * env, jobject jobj) {
    quitting = true;
}
JNIEXPORT void JNICALL Java_javaAI_talkKey(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_m);
    Keyboard_button_released(XK_m);
}
JNIEXPORT void JNICALL Java_javaAI_toggleShowMessage(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_9);
    Keyboard_button_released(XK_KP_9);
}
JNIEXPORT void JNICALL Java_javaAI_toggleShowItems(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_8);
    Keyboard_button_released(XK_KP_8);
}
JNIEXPORT void JNICALL Java_javaAI_toggleCompass(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_KP_7);
    Keyboard_button_released(XK_KP_7);
}
JNIEXPORT void JNICALL Java_javaAI_repair(JNIEnv * env, jobject jobj) {
	Keyboard_button_pressed(XK_f);
	Keyboard_button_released(XK_f);
}
JNIEXPORT void JNICALL Java_javaAI_reprogram(JNIEnv * env, jobject jobj) {
    Keyboard_button_pressed(XK_grave);
	Keyboard_button_released(XK_grave);
}
// Talk Function, can't be called too frequently or client will flood - JTO
JNIEXPORT void JNICALL Java_javaAI_talk(JNIEnv * env, jobject jobj, jstring talk_str) {
	char* str;
	str = (*env)->GetStringUTFChars(env, talk_str, NULL); //Converts jstring to char[] and places in str -JTO
	Net_talk(str);
        (*env)->ReleaseStringUTFChars(env, talk_str, str); //Converts jstring to char[] and places in argv -EGG
}
JNIEXPORT jstring JNICALL Java_javaAI_scanMsg(JNIEnv * env, jobject jobj, int id) {
    if (id<MAX_MSGS) return (*env)->NewStringUTF(env,TalkMsg[id]->txt);
    return (*env)->NewStringUTF(env,"");
}
//Added scanGameMsg -CJG
JNIEXPORT jstring JNICALL Java_javaAI_scanGameMsg(JNIEnv * env, jobject jobj, int id) {
    if (id<MAX_MSGS) return (*env)->NewStringUTF(env,GameMsg[id]->txt);
    return (*env)->NewStringUTF(env,"");
}
// End other options -JNE
// Self properties -JNE
JNIEXPORT int JNICALL Java_javaAI_selfX(JNIEnv * env, jobject jobj) {       // returns the player's x position
    return pos.x;
}
JNIEXPORT int JNICALL Java_javaAI_selfY(JNIEnv * env, jobject jobj) {       // returns the player's y position
    return pos.y;
}
//Returns the player's X coord on radar (int). -EGG
JNIEXPORT int JNICALL Java_javaAI_selfRadarX() {
    if(selfVisible) return radar_ptr[0].x;
    return -1;
}
//Returns the player's Y coord on radar (int). -EGG
JNIEXPORT int JNICALL Java_javaAI_selfRadarY() {
    if(selfVisible) return radar_ptr[0].y;
    return -1;
}
JNIEXPORT int JNICALL Java_javaAI_selfVelX(JNIEnv * env, jobject jobj) {    // returns the player's x velocity
    return vel.x;
}
JNIEXPORT int JNICALL Java_javaAI_selfVelY(JNIEnv * env, jobject jobj) {    // returns the player's y velocity
    return vel.y;
}
JNIEXPORT int JNICALL Java_javaAI_selfSpeed(JNIEnv * env, jobject jobj) {   // returns speed of the player's ship
    return sqrt(pow(vel.x,2)+pow(vel.y,2));
}
JNIEXPORT double JNICALL Java_javaAI_lockHeadingDeg(JNIEnv * env, jobject jobj) {   // returns the angle at which the player's lock is in relation to the player's ship -JNE
    return (double)lock_dir*2.8125;
}
JNIEXPORT double JNICALL Java_javaAI_lockHeadingRad(JNIEnv * env, jobject jobj) {   // returns the angle at which the player's lock is in relation to the player's ship -JNE
     return (double)lock_dir*.049087;
}
JNIEXPORT short JNICALL Java_javaAI_selfLockDist(JNIEnv * env, jobject jobj) {      // returns the distance of the ship the player is locked onto
    return lock_dist; //-JNE
}
JNIEXPORT int JNICALL Java_javaAI_selfReload(JNIEnv * env, jobject jobj) {    // returns the player's reload time remaining
    return reload;
}
//Gets the player's ID, returns an int. -EGG
JNIEXPORT int JNICALL Java_javaAI_selfID() {
    if (self != NULL)
        return self->id;
}
//Returns 1 if the player is alive, 0 if they are not. -EGG
JNIEXPORT int JNICALL Java_javaAI_selfAlive() {
    return selfVisible;
}
//Returns the player's team (int). -EGG
JNIEXPORT int JNICALL Java_javaAI_selfTeam() {
    if (self != NULL)
        return self->team;
}
//Returns the player's lives remaining (if there is a life limit) or the number of lives spent (int). -EGG
JNIEXPORT int JNICALL Java_javaAI_selfLives() {
    if (self != NULL)
        return self->life;
}
double selfHeadingDeg() {   //returns the player's heading in degrees	-JNE
    return (double)heading*2.8125;
}
double selfHeadingRad() {   //returns the player's heading in radians	-JNE
    return (double)heading*.049087;
}
JNIEXPORT double JNICALL Java_javaAI_selfTrackingRad(JNIEnv * env, jobject jobj) {  // returns the player's tracking in radians	-JNE
    if (vel.y == 0 && vel.x == 0) return selfHeadingRad(); //fix for NaN -EGG java translation -CJG
    double ans = atan((double)vel.y/(double)vel.x);       //calculate tracking
    if (vel.y >= 0 && vel.x < 0) {        //format the tracking properly
        ans += PI_AI;
    }
    else if (vel.y < 0 && vel.x < 0) {
        //ans = -ans;
        ans += PI_AI;
    }
    else if (vel.y < 0 && vel.x >= 0) {
        ans+=2*PI_AI;
    }
    return ans;
}
JNIEXPORT double JNICALL Java_javaAI_selfTrackingDeg(JNIEnv * env, jobject jobj) {  // returns the player's tracking in degrees -JNE
    if (vel.y == 0 && vel.x == 0) return selfHeadingDeg(); //fix for NaN -EGG Java Translation -CJG
    return (double)Java_javaAI_selfTrackingRad(env,jobj)*180/PI	;
}
JNIEXPORT double JNICALL Java_javaAI_selfHeadingDeg(JNIEnv * env, jobject jobj) {   // returns the player's heading in degrees	-JNE
    return (double)heading*2.8125;
}
JNIEXPORT double JNICALL Java_javaAI_selfHeadingRad(JNIEnv * env, jobject jobj) {   // returns the player's heading in radians	-JNE
    return (double)heading*.049087;
}
//Renamed to hud and changed to display message at given index -CJG
JNIEXPORT jstring JNICALL Java_javaAI_hud(JNIEnv * env, jobject jobj, int i) {         // if the HUD is displaying a name, return it	-JNE
    if (i < MAX_SCORE_OBJECTS) {
        if (score_objects[i].hud_msg_len>0) {
            return (*env)->NewStringUTF(env,score_objects[i].hud_msg);
        }
    }
    return (*env)->NewStringUTF(env,"");
}
//hudScore now displays message at a given index -CJG
JNIEXPORT jstring JNICALL Java_javaAI_hudScore(JNIEnv * env, jobject jobj, int i) {        // if the HUD is displaying a score, return it	-JNE
    if (i < MAX_SCORE_OBJECTS) {
        if (score_objects[i].hud_msg_len>0) {
            return (*env)->NewStringUTF(env,score_objects[i].msg);
        }
    }
    return (*env)->NewStringUTF(env,"");
}
//hudTimeLeft displays message at given index -CJG
JNIEXPORT double JNICALL Java_javaAI_hudTimeLeft(JNIEnv * env, jobject jobj, int i) {      // returns how much time the HUD will keep displaying a score for, in seconds	-JNE
    if (i<MAX_SCORE_OBJECTS) {
        if (score_objects[i].hud_msg_len>0) {
            return (100-score_objects[i].count);
        }
    }
    return 0;
}
//Gets the player's turnspeed, returns a double. -EGG
JNIEXPORT double JNICALL Java_javaAI_getTurnSpeed() {
    return turnspeed;
}
JNIEXPORT double JNICALL Java_javaAI_getPower() {
    return power;
}
//Returns 1 if the player's shield is on, 0 if it is not, -1 if player is not alive. -EGG
JNIEXPORT int JNICALL Java_javaAI_selfShield(JNIEnv * env, jobject jobj) {
    int i;
    for (i=0;i<num_ship;i++) if ((self != NULL) && (ship_ptr[i].id==self->id)) return (int)ship_ptr[i].shield;
    return -1;
}
//Returns the player's username (string). -EGG
JNIEXPORT jstring JNICALL Java_javaAI_selfName(JNIEnv * env, jobject jobj) {
    if (self != NULL) return (*env)->NewStringUTF(env,self->name);
}
//Returns the player's score (double). -EGG
JNIEXPORT double JNICALL Java_javaAI_selfScore(JNIEnv * env, jobject jobj) {
    if (self != NULL) return self->score;
}
// End self properties -JNE

JNIEXPORT int JNICALL Java_javaAI_closestRadarX(JNIEnv * env, jobject jobj) {       // returns x radar coordinate (0-256) of closest ship on radar
    int i, x;
    double best = -1, l = -1;
    for (i = 1;i < num_radar;i++) {         // go through each enemy
       // if (radar_ptr[i].type == RadarEnemy) {  // skip if they are a friend
            l = sqrt(pow(radar_ptr[i].x-radar_ptr[0].x,2)+pow(radar_ptr[i].y-radar_ptr[0].y,2));    // get enemy distance
            if ((l < best) || (best == -1)) {       // if distance is the smallest or this is the first distance measured
                best = l;                       // update distance
                x = radar_ptr[i].x;             // update value to be returned
            }
        //}
    }
    if (best ==-1) //If so, there are no enemies (alive).
        return -1;
    return x;
}
JNIEXPORT int JNICALL Java_javaAI_closestRadarY(JNIEnv * env, jobject jobj) {       // returns y radar coordinate (0-256) of closest ship on radar -JNE
    int i, y;
    double best = -1, l = -1;
    for (i = 1;i < num_radar;i++) {         // go through each ship on radar
        //if (radar_ptr[i].type == RadarEnemy) {  // skip if they are a friend
            l = sqrt(pow(radar_ptr[i].x-radar_ptr[0].y,2)+pow(radar_ptr[i].y-radar_ptr[0].y,2));    // get enemy distance
            if (l < best || best == -1) {           // if distance is the smallest or this is the first distance measured
                best = l;                       // update distance
                y = radar_ptr[i].y;             // update value to be returned
            }
        //}
    }
    if (best == -1) // if so, there are no enemies alive -JNE
        return NULL;
    return y;
}
JNIEXPORT int JNICALL Java_javaAI_closestItemX(JNIEnv * env, jobject jobj) {                // returns x coordinate of closest item on screen -JNE
    int i, x;
    double best = -1, l = -1;
    for (i=0;i<num_itemtype;i++) {          // go through each item on screen
        l = sqrt(pow(itemtype_ptr[i].x-pos.x,2)+pow(itemtype_ptr[i].y-pos.y,2));        // get distance
        if (l < best || best == -1) {           // if distance is smallest yet
            best = l;                       // update distance
            x=itemtype_ptr[i].x;            // update x coordinate
        }
    }
    if (best==-1)   // if so there are no items on screen
        return -1;
    return x;
}
JNIEXPORT int JNICALL Java_javaAI_closestItemY(JNIEnv * env, jobject jobj) {                // returns y coordinate of closest item on screen -JNE
    int i, y;
    double best = -1, l = -1;
    for (i=0;i<num_itemtype;i++) {          // go through each item on screen
        l = sqrt(pow(itemtype_ptr[i].x-pos.x,2)+pow(itemtype_ptr[i].y-pos.y,2));        // get distance
        if (l < best || best == -1) {           // if distance is smallest yet
            best = l;                       // update distance
            y=itemtype_ptr[i].y;            // update y coordinate
        }
    }
    if (best==-1)   // if so there are no items on screen
        return -1;
    return y;
}
// Start wrap helper functions -JNE
// Checks if the map wraps between two x or y coordinates; if it does, it returns a usable value for the first coordinate -JNE
// May glitch if the map is smaller than ext_view_width and height -JNE
int wrapX(int firstX, int selfX) {
	int tempX;
	tempX = firstX - selfX;
	if (abs(tempX)>ext_view_width) {
		if (firstX > selfX) {
			firstX = firstX - Setup->width;
		}
		else {
			firstX = firstX + Setup->width;
		}
	}
	return firstX;
}
int wrapY(int firstY, int selfY) {
	int tempY;
	tempY = firstY - selfY;
	if (abs(tempY)>ext_view_height) {
		if (firstY > selfY) {
			firstY = firstY - Setup->height;
		}
		else {
			firstY = firstY + Setup->height;
		}
	}
	return firstY;
}
// End wrap helper functions -JNE
JNIEXPORT int JNICALL Java_javaAI_closestShipId(JNIEnv * env, jobject jobj) {		// returns ID of closest ship on screen -JNE
	int i, d;
	double best = -1, l = -1;
	for (i=0;i<num_ship;i++) {		// go through each ship on screen
		if (ship_ptr[i].x!=pos.x || ship_ptr[i].y!=pos.y){		// make sure ship is not player's ship
			l = sqrt(pow(wrapX(ship_ptr[i].x,pos.x)-pos.x,2)+pow(wrapY(ship_ptr[i].y,pos.y)-pos.y,2));		// distance formula
			if (l < best || best == -1) {		// if distance is smallest yet
				best = l;			// update distance
				d = ship_ptr[i].id;		// update id
			}
		}
	}
	if (best == -1)	// if so there are no ships on screen
		return -1;
	return d;
}
// End closest functions -JNE
// ID functions -JNE
JNIEXPORT double JNICALL Java_javaAI_enemySpeedId(JNIEnv * env, jobject jobj, int id) {	// returns velocity (in pixels per frame) of an enemy with a particular ID -JNE
	int i, j;
	shipData_t temp;
	for (i=0;i<num_ship;i++) {	// go through each ship
		if (ship_ptr[i].id==id) {	// find ship with correct id
			for (j=0;j<128;j++) {	// go through [][], look for same ship
				if (allShips[j][0].ship.id==id) {
					if (allShips[j][2].vel != -1) {	// ship is there, push onto array, calculate distance, return distance
						return allShips[j][0].vel;
					}
				}
			}
		}
	}
	return -1;
}
JNIEXPORT double JNICALL Java_javaAI_enemyTrackingRadId(JNIEnv * env, jobject jobj, int id) {	// returns direction of a ship's velocity in radians -JNE
	int i, j;
	shipData_t temp;
	for (i=0;i<num_ship;i++) {
		if (ship_ptr[i].id==id) {
			for (j=0;j<128;j++) {
				if (allShips[j][0].ship.id==id) {
					if (allShips[j][2].vel != -1) { // ship is there, push onto array, calculate distance ,return distance
						return allShips[j][0].trackingRad;
					}
				}
			}
		}
	}
}
JNIEXPORT double JNICALL Java_javaAI_enemyTrackingDegId(JNIEnv * env, jobject jobj, int id) {	// returns direction of a ship's velocity in degrees -JNE
	int i, j;
	shipData_t temp;
	for (i=0;i<num_ship;i++) {
		if (ship_ptr[i].id==id) {
			for (j=0;j<128;j++) {
				if (allShips[j][0].ship.id==id) {
					if (allShips[j][2].vel != -1) { // ship is there, push onto array, calculate distance ,return distance
						return allShips[j][0].trackingDeg;
					}
				}
			}
		}
	}
}
JNIEXPORT int JNICALL Java_javaAI_enemyReloadId(JNIEnv * env, jobject jobj, int id) {
	int i, j;
	shipData_t temp;
	for (i=0;i<num_ship;i++) {
		if (ship_ptr[i].id==id) {
			for (j=0;j<128;j++) {
				if (allShips[j][0].ship.id==id) {
					if (allShips[j][2].vel != -1) { 
						return allShips[j][0].reload;
					}
				}
			}
		}
	}
}
JNIEXPORT int JNICALL Java_javaAI_screenEnemyXId(JNIEnv * env, jobject jobj, int id) {	// returns x coordinate of closest ship on screen -JNE
	int i;
	for (i = 0;i < num_ship;i++) {		// go through each ship on screen
		if (ship_ptr[i].id==id) {
			return ship_ptr[i].x;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_screenEnemyYId(JNIEnv * env, jobject jobj, int id) {	// returns y coordinate of closest ship on screen -JNE
	int i;
	for (i = 0;i < num_ship;i++) {		// go through each ship on screen
		if (ship_ptr[i].id==id) {
			return ship_ptr[i].y;
		}
	}
	return -1;
}
JNIEXPORT double JNICALL Java_javaAI_enemyHeadingDegId(JNIEnv * env, jobject jobj, int id) {  // returns the heading of ship with a particular ID in degrees -JNE
    int i;
    for (i=0;i<num_ship;i++) {
        if (ship_ptr[i].id==id) {
            return (double)ship_ptr[i].dir*2.8125;	// convert from 0-127 scale to 0-360 scale
        }
    }
    return -1;              // program reaches here if there is not a ship on screen with that id
}
JNIEXPORT double JNICALL Java_javaAI_enemyHeadingRadId(JNIEnv * env, jobject jobj, int id) {  // returns the heading of ship with a particular ID in radians -JNE
    int i;
    for (i=0;i<num_ship;i++) {
        if (ship_ptr[i].id==id) {
            return (ship_ptr[i].dir*.049087); // convert from 0-127 scale to 0-2pi scale
        }
    }
    return -1;
}
JNIEXPORT int JNICALL Java_javaAI_enemyShieldId(JNIEnv * env, jobject jobj, int id) {         // return 1 if the enemy has a shield up, 0 if it does not -JNE
    int i;
    for (i=0;i<num_ship;i++) {              // check all ships on screen for that id
        if (ship_ptr[i].x!=pos.x || ship_ptr[i].y!=pos.y){              // make sure ship is not player's ship
            if (ship_ptr[i].id==id) {
                return (int)ship_ptr[i].shield;         // 0 if no shield, 1 if shield
            }
        }
    }
    return -1;              // program reaches here if there is not a ship on screen with that id 
}
JNIEXPORT int JNICALL Java_javaAI_enemyLivesId(JNIEnv * env, jobject jobj, int id) {          // return the number of lives for enemy with a particular id	-JNE
    int i;
    for (i=0;i<num_others;i++) {    // look for the other with the id that was passed in
        if (Others[i].id==id) {
            return (int)Others[i].life;
        }
    }
    return -1;              // return -1 if the id is invalid
}
JNIEXPORT jstring JNICALL Java_javaAI_enemyNameId(JNIEnv * env, jobject jobj, int id) {       // return the name of the enemy with the corresponding id	-JNE
    int i;
    for (i=0;i<num_others;i++) {    // look for the other with the id that was passed in
        if (Others[i].id==id) {
            return (*env)->NewStringUTF(env,Others[i].name); // make a jstring out of name
        }
    }
    return (*env)->NewStringUTF(env,"");
}
JNIEXPORT double JNICALL Java_javaAI_enemyScoreId(JNIEnv * env, jobject jobj, int id) {        // returns the overall score of a ship with a particular ID	-JNE
    int i;
    for (i=0;i<num_others;i++) {
        if (Others[i].id==id) {
            return Others[i].score;
        }
    }
    return -1;
}
JNIEXPORT int JNICALL Java_javaAI_enemyTeamId(JNIEnv * env, jobject jobj, int id) {           // returns team of a ship with a particular ID, or -1 if the map is not a team play map -JNE
    int i;
    if (num_playing_teams == 0) {
        return -1;
    }
    else {
        for (i=0;i<num_others;i++) {
            if (Others[i].id==id) {
                return Others[i].team;
            }
        }
    }
    return -1;
}
JNIEXPORT double JNICALL Java_javaAI_enemyDistanceId(JNIEnv * env, jobject jobj, int id) {		// returns the distance of a ship with a particular ID
	int i;
	for (i=0;i<128;i++) {
		if (allShips[i][0].ship.id == id) {
			return allShips[i][0].d;
		}
	}
	return -1;
}
// End ID functions -JNE
// Begin idx functions! -JNE
JNIEXPORT double JNICALL Java_javaAI_enemyDistance(JNIEnv * env, jobject jobj, int idx) {	// returns the distance of a ship with a particular index -JNE
	return allShips[idx][0].d;
}
JNIEXPORT double JNICALL Java_javaAI_enemySpeed(JNIEnv * env, jobject jobj, int idx) {	// returns velocity of a ship with a particular index -JNE
	return allShips[idx][0].vel;
}
JNIEXPORT int JNICALL Java_javaAI_enemyReload(JNIEnv * env, jobject jobj, int idx) {	// returns velocity of a ship with a particular index -JNE
	return allShips[idx][0].reload;
}
JNIEXPORT double JNICALL Java_javaAI_enemyTrackingRad(JNIEnv * env, jobject jobj, int idx) {	// returns tracking based on index -JNE
	return allShips[idx][0].trackingRad;
}
JNIEXPORT double JNICALL Java_javaAI_enemyTrackingDeg(JNIEnv * env, jobject jobj, int idx) {	// returns tracking based on index -JNE
	return allShips[idx][0].trackingDeg;
}
JNIEXPORT int JNICALL Java_javaAI_screenEnemyX(JNIEnv * env, jobject jobj, int idx) {		// returns x coordinate of enemy at an index -JNE
	return allShips[idx][0].ship.x;
}
JNIEXPORT int JNICALL Java_javaAI_screenEnemyY(JNIEnv * env, jobject jobj, int idx) {		// returns y coordinate of enemy at an index -JNE
	return allShips[idx][0].ship.y;
}
JNIEXPORT double JNICALL Java_javaAI_enemyHeadingDeg(JNIEnv * env, jobject jobj, int idx) {		// returns heading in degrees of enemy at an index -JNE
	return allShips[idx][0].ship.dir*2.8125;
}
JNIEXPORT double JNICALL Java_javaAI_enemyHeadingRad(JNIEnv * env, jobject jobj, int idx) {		// returns heading in radians of enemy at an index -JNE
	return allShips[idx][0].ship.dir*.049087;
}
JNIEXPORT int JNICALL Java_javaAI_enemyShield(JNIEnv * env, jobject jobj, int idx) {		// returns shield status of enemy at an index -JNE
	return allShips[idx][0].ship.shield;
}
JNIEXPORT int JNICALL Java_javaAI_enemyLives(JNIEnv * env, jobject jobj, int idx) {		// returns lives of enemy at an index -JNE
	int i;
	for (i=0;i<num_others;i++) {
		if (Others[i].id == allShips[idx][0].ship.id) {
			return Others[i].life;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_enemyTeam(JNIEnv * env, jobject jobj, int idx) {	// returns team of enemy at an index -JNE
	int i;
	for (i=0;i<num_others;i++) {
		if (Others[i].id == allShips[idx][0].ship.id) {
			return Others[i].team;
		}
	}
	return -1;
}
JNIEXPORT jstring JNICALL Java_javaAI_enemyName(JNIEnv * env, jobject jobj, int idx) {		// returns name of enemy at an index -JNE
	int i;
	for (i=0;i<num_others;i++) {
		if (Others[i].id == allShips[idx][0].ship.id) {
			return (*env)->NewStringUTF(env,Others[i].name);	// make a jstring out of name
		}
	}
	return "";
}
JNIEXPORT double JNICALL Java_javaAI_enemyScore(JNIEnv * env, jobject jobj, int idx) {		// returns score of enemy at an index -JNE
	int i;
	for (i=0;i<num_others;i++) {
		if (Others[i].id == allShips[idx][0].ship.id) {
			return Others[i].score;
		}
	}
}
// End idx functions. -JNE
//Converts degrees (int) to radians (double). -EGG
JNIEXPORT double JNICALL Java_javaAI_degToRad(JNIEnv * env, jobject jobj, int deg) {
    return (deg * (PI / 180.0));
}
//Converts radians (double) to degrees (int). -EGG
JNIEXPORT int JNICALL Java_javaAI_radToDeg(JNIEnv * env, jobject jobj, double rad) {
    return (int)(rad * (180.0 / PI));
}
//Converts radians (double) to degrees (int). -EGG
int radToDeg(double rad) {
    return (int)(rad * (180.0 / PI));
}
//Returns the smallest angle which angle1 could add to itself to be equal to angle2. -EGG
//This is useful for turning particular directions. -EGG
JNIEXPORT int JNICALL Java_javaAI_angleDiff(JNIEnv * env, jobject jobj, int angle1, int angle2) {
    int difference = angle2 - angle1;
    while (difference < -180) difference += 360;
    while (difference > 180) difference -= 360;
    return difference;
}
//Returns the result of adding two angles together. -EGG
JNIEXPORT int JNICALL Java_javaAI_angleAdd(JNIEnv * env, jobject jobj, int angle1, int angle2) {
  return (angle1 + angle2) % 360;
}
//Wall_here -EGG
//Parameters: x, y, flag to draw wall feelers, flag to draw wall detection. -EGG
//Returns 1 if there is a wall at the given x,y or 0 if not. -EGG
//Removed detection drawing flags -CJG
int Wall_here(int x_here, int y_here) {
    int			xi, yi, xb, yb;//, xe, ye;
    int			rxb, ryb;
    int			x, y;
    int			type;
    unsigned char	*mapptr, *mapbase;
    //if (!oldServer) {
	//printf("This is a 'new server'. Wall detection will not work here.\n");
       // return;
    //}
    xb = ((x_here < 0) ? (x_here - (BLOCK_SZ - 1)) : x_here) / BLOCK_SZ;
    yb = ((y_here < 0) ? (y_here - (BLOCK_SZ - 1)) : y_here) / BLOCK_SZ;
    if (!BIT(Setup->mode, WRAP_PLAY)) {
	if (xb < 0)
	    xb = 0;
	if (yb < 0)
	    yb = 0;
	/*if (xe >= Setup->x)
	    xe = Setup->x - 1;
	if (ye >= Setup->y)
	    ye = Setup->y - 1;*/
    }
    y = yb * BLOCK_SZ;
    yi = mod(yb, Setup->y);
    mapbase = Setup->map_data + yi;
    ryb = yb;
    if (yi == Setup->y) {
	yi = 0;
	mapbase = Setup->map_data;
    }
    x = xb * BLOCK_SZ;
    xi = mod(xb, Setup->x);
    mapptr = mapbase + xi * Setup->y;
    rxb = xb;
    if (xi == Setup->x) {
        xi = 0;
        mapptr = mapbase;
    }
    type = *mapptr;
    if (type & BLUE_BIT) return 1;
    return 0;
}
//wallFeeler! -EGG
//Parameters: distance of line to 'feel', angle in degrees, flag to draw wall feelers, flag to draw wall detection. -EGG
//Returns 1 if there is a wall from the player's ship at the given angle and distance or 0 if not. -EGG
//Removed detection drawing flags -CJG
JNIEXPORT int JNICALL Java_javaAI_wallFeeler(JNIEnv * env, jobject jobj, int dist, int angle) {
    double a = angle * PI / 180.0;
    int x = pos.x + cos(a)*dist;
    int y = pos.y + sin(a)*dist;
    int ret = Java_javaAI_wallBetween(env, jobj, pos.x, pos.y, x, y);
    if (ret == -1) return dist; // Returns the distance of the feeler if no wall is felt - JTO
    return ret;
}
//wallFeeler that uses radians! -EGG
//Removed detection drawing flags -CJG
JNIEXPORT int JNICALL Java_javaAI_wallFeelerRad(JNIEnv * env, jobject jobj, int dist, double a) {
    int x = pos.x + cos(a)*dist;
    int y = pos.y + sin(a)*dist;
    int ret =  Java_javaAI_wallBetween(env, jobj, pos.x, pos.y, x, y);
    if (ret == -1) return dist; // Returns the distance of the feeler if no wall is felt - JTO
    return ret;
}
//Detects if there is a wall between two points, returns 0 or 1 -EGG
//Utilizes Bresenham's line-drawing algorithm (no multiplication or division!) -EGG
//Adopted from http://www.brackeen.com/vga/source/djgpp20/lines.c.html (THANK YOU!) -EGG
//Parameters: x1, y1, x2, y2, flag to draw wall feelers, flag to draw wall detection. -EGG
//Returns 1 if there is a wall between the two points or 0 if not. -EGG
//Removed detection drawing flags -CJG
JNIEXPORT int JNICALL Java_javaAI_wallBetween(JNIEnv * env, jobject jobj, int x1, int y1, int x2, int y2) {
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py,ret;
  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;
  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      ret = Wall_here(px, py);
      if (ret) return sqrt(pow((px-x1),2)+pow((py-y1),2));
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      ret = Wall_here(px, py);
      if (ret) return sqrt(pow((px-x1),2)+pow((py-y1),2));
    }
  }
  return -1;
}
// Shot functions
JNIEXPORT int JNICALL Java_javaAI_shotAlert(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].alert;
}
JNIEXPORT int JNICALL Java_javaAI_shotX(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].x;
}
JNIEXPORT int JNICALL Java_javaAI_shotY(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].y;
}
JNIEXPORT int JNICALL Java_javaAI_shotDist(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].dist;
}
JNIEXPORT int JNICALL Java_javaAI_shotVel(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].vel;
}
JNIEXPORT int JNICALL Java_javaAI_shotVelDir(JNIEnv * env, jobject jobj, int idx) {
	return AIshot_buffer[idx].veldir;
}
// Calculates the direction that self would need to point to hit the ship idx (ported from old code) -EGG
JNIEXPORT int JNICALL Java_javaAI_aimdir(JNIEnv * env, jobject jobj, int idx) {
    if (allShips[idx][0].vel==-1) return -1;
    float Bx, By, Bvel, Cx, Cy, Svx, Svy, Sx, Sy, forgo, tugo, mugo;
    float degs1, degs2, time1, time2, Bvx;
    Bx = (float) pos.x;
    By = (float) pos.y;
    Bvel = (float) AI_shotspeed;
    Cx = cos(AI_radian(selfTrackingDeg())) * selfSpeed();
    Cy = sin(AI_radian(selfTrackingDeg())) * selfSpeed();
    Svx = cos(AI_radian(allShips[idx][0].trackingDeg)) * allShips[idx][0].vel;
    Svy = sin(AI_radian(allShips[idx][0].trackingDeg)) * allShips[idx][0].vel;
    Sx = allShips[idx][0].ship.x;
    Sy = allShips[idx][0].ship.y;
    tugo = pow(pow(Bvel * Sx - Bvel * Bx, 2) + pow(Bvel * By - Bvel * Sy, 2), 0.5);
    mugo = AI_degree(acos((Bvel * Sx - Bvel * Bx) / tugo));
    forgo = AI_degree(asin
		  ((By * Svx - Bx * Svy + Bx * Cy - By * Cx + Sx * Svy -
		    Sy * Svx - Cy * Sx + Sy * Cx) / tugo));
    degs1 = fabs(forgo + mugo);
    degs2 = fabs(forgo - mugo);
    Bvx = cos(AI_radian(degs1)) * Bvel + cos(AI_radian(selfTrackingDeg())) * selfSpeed();
    time1 = (Bx - Sx) / (Svx - Bvx);
    Bvx = cos(AI_radian(degs2)) * Bvel + cos(AI_radian(selfTrackingDeg())) * selfSpeed();
    time2 = (Bx - Sx) / (Svx - Bvx);
    /*It's because those asin and acos that I must do all the below, because they return values that may or may not be in the correct quadrant.  Someone must figure out how to tell what quadrant they should be in */
    if (time1 < 0 && time2 < 0) return -1;
    else if (time1 < 0) {
	if ((Sy + Svy * time2) <= (pos.y + sin(AI_radian(selfTrackingDeg())) * selfSpeed() * time2)) return 360 - (int)degs2;
	else return -1;
    }
    else if (time2 < 0) {
	if ((Sy + Svy * time1) >= (pos.y + sin(AI_radian(selfTrackingDeg())) * selfSpeed() * time1)) return (int)degs1;
	else return -1;
    }
    else {
	if ((Sy + Svy * time1) >= (pos.y + sin(AI_radian(selfTrackingDeg())) * selfSpeed() * time1)) return (int)degs1;
	else if ((Sy + Svy * time2) < (pos.y + sin(AI_radian(selfTrackingDeg())) * selfSpeed() * time2)) return 360 - (int)degs2;
	else return (int)degs2;
    }
    return -1;
}
//Capture the flag functions - Sarah Penrose
JNIEXPORT int JNICALL Java_javaAI_ballX(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_ball;i++) {
		if (ball_ptr[i].x != -1) {
			return ball_ptr[i].x;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_ballY(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_ball;i++) {
		if (ball_ptr[i].x != -1) {
			return ball_ptr[i].y;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_connectorX0(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_connector;i++) {
		if (connector_ptr[i].x0 != -1) {
			return connector_ptr[i].x0;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_connectorX1(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_connector;i++) {	
		if (connector_ptr[i].x1 != -1) {
			return connector_ptr[i].x1;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_connectorY0(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_connector;i++) {
		if (connector_ptr[i].y0 != -1) {
			return connector_ptr[i].y0;
		}
	}
	return -1;
}
JNIEXPORT int JNICALL Java_javaAI_connectorY1(JNIEnv * env, jobject jobj) {
	int i;
	for (i=0;i<num_connector;i++) {
		if (connector_ptr[i].y1 != -1) {
			return connector_ptr[i].y1;
		}
	}	
	return -1;
}
// Returns self tracking (direction of velocity) in radians. -JNE
//Do not touch the next three functions, they are needed for other functions to run -JRA
double selfTrackingRad() {
    double ans = atan((double)vel.y/(double)vel.x);
    if (vel.y >= 0 && vel.x < 0)
        ans += 3.141592653589793;
    else if (vel.y < 0 && vel.x < 0)
	ans += 3.141592653589793;
    else if (vel.y < 0 && vel.x >= 0)
	ans+=2*3.141592653589793;
    return ans;
}
// Returns self tracking (direction of velocity) in degrees. -JNE
int selfTrackingDeg() {
    return radToDeg(selfTrackingRad());
}
int selfSpeed() {   // Returns the speed of the player's ship. -EGG
        return sqrt(pow(vel.x,2)+pow(vel.y,2));
}
// Methods to help AI loop -JNE
void calcStuff(int j) {			// updates data in allShips for velocity and tracking in degrees and radians -JNE
	allShips[j][0].d = sqrt(pow(wrapX(allShips[j][0].ship.x,pos.x)-pos.x,2)+pow(wrapX(allShips[j][0].ship.y,pos.y)-pos.y,2));
	allShips[j][0].vel = sqrt(pow(wrapX(allShips[j][0].ship.x,allShips[j][2].ship.x)-allShips[j][2].ship.x,2)+pow(wrapY(allShips[j][0].ship.y,allShips[j][2].ship.y)-allShips[j][2].ship.y,2))/2;		// calculate velocity
	allShips[j][0].xVel = wrapX(allShips[j][0].ship.x,allShips[j][2].ship.x)-allShips[j][2].ship.x;	// calculate x velocity
	allShips[j][0].yVel = wrapY(allShips[j][0].ship.y,allShips[j][2].ship.y)-allShips[j][2].ship.y;	// calculate y velocity
	allShips[j][0].trackingRad = atan(allShips[j][0].yVel/allShips[j][0].xVel);	// calculate tracking
	if (allShips[j][0].xVel >= 0 && allShips[j][0].yVel < 0) {	// re-format tracking
		allShips[j][0].trackingRad += 2*3.141592653589793;
	}
	else if (allShips[j][0].xVel < 0 && allShips[j][0].yVel < 0) {
		allShips[j][0].trackingRad += 3.141592653589793;
	}
	else if (allShips[j][0].xVel < 0 && allShips[j][0].yVel >= 0) {
		allShips[j][0].trackingRad = 3.141592653589793+allShips[j][0].trackingRad;
	}
	allShips[j][0].trackingDeg = allShips[j][0].trackingRad*180/3.141592653589793;
}
void updateSlots() {	// moves everything in allShips over by a frame -JNE
	int i;
	ship_t theShip;
	theShip.x=-1;
	theShip.y=-1;
	theShip.dir=-1;
	theShip.shield=-1;
	theShip.id=-1;
	for (i=0;i<128;i++) {			// check every slot in allShips
		if (allShips[i][0].vel!=-1 || allShips[i][1].vel!=-1 || allShips[i][2].vel!=-1) {	// only update slots that were updated in the last three frames
			allShips[i][2] = allShips[i][1];	// bump the last two down one
			allShips[i][1] = allShips[i][0];
			allShips[i][0].vel = -1;		// this is updated later if the ship is still on screen
			allShips[i][0].d = 9999;
			allShips[i][0].xVel=-1;
			allShips[i][0].yVel=-1;
			allShips[i][0].trackingDeg=-1;
			allShips[i][0].trackingRad=-1;
                        if (allShips[i][1].reload > 0) allShips[i][0].reload=allShips[i][1].reload-1; // reload tracking -EGG
			else if (allShips[i][1].vel!=-1) allShips[i][0].reload=0;
			else allShips[i][0].reload=-1;
			allShips[i][0].ship=theShip;
		}
	}
}
int updateFirstOpen() {	// goes through allShips, returning the index of the first open spot -JNE
	int i;
	for (i=0;i<128;i++) {
		if (allShips[i][0].vel==-1 && allShips[i][1].vel==-1 && allShips[i][2].vel==-1) {
			return i;
		}
	}
	return -1;
}
bool updateShip(int i) { // goes through allShips and checks if a particular ship is there, returning true if it is and false if it isn't -JNE
        int j;
        for (j=0;j<128;j++) {
                if (ship_ptr[i].id==allShips[j][1].ship.id) {   // find the spot where the ship's ID is located
                        allShips[j][0].ship = ship_ptr[i];
                        if (allShips[j][2].vel >= 0) {
                                calcStuff(j);
                        }
                        else {
                                allShips[j][0].vel = 0;
                        }
                        return true;            // ship was found, so don't add it as a new ship
                }
        }
        return false;
}
void addNewShip(int firstOpen, int i) { // add a ship that has never been on screen before -JNE
        if (Java_javaAI_selfID(prime_env,prime_jobj) != ship_ptr[i].id) {
                if (updateShip(i)==false) {
                        allShips[firstOpen][0].ship = ship_ptr[i];
                        allShips[firstOpen][0].vel = 0;
                }
        }
}
void sortShips() {	// sorts the ships in the allShips buffer by how far away they are from the player -JNE
//See our previous quicksort thanks ;)
	#define  MAX_LEVELS  1000
	shipData_t piv;
	int  beg[MAX_LEVELS], end[MAX_LEVELS], i=0, L, R ;
	beg[0]=0; end[0]=128;
	while (i>=0) {
		L=beg[i]; R=end[i]-1;
		if (L<R) {
			piv=allShips[L][0]; if (i==MAX_LEVELS-1) return -1;
			while (L<R) {
				while (allShips[R][0].d>=piv.d && L<R) R--;
				if (L<R) {
					allShips[L++][0]=allShips[R][0];
					allShips[L][1]=allShips[R][1];
					allShips[L][2]=allShips[R][2];
				}
				while (allShips[L][0].d<=piv.d && L<R) L++;
				if (L<R) {
					allShips[R--][0]=allShips[L][0];
					allShips[R][1]=allShips[L][1];
					allShips[R][2]=allShips[L][2];
				}
			}
			allShips[L][0]=piv; beg[i+1]=L+1; end[i+1]=end[i]; end[i++]=L; }
		else {
			i--;
		}
	}
   return 1;
}
// update ships' velocity and tracking
void prepareShips() {
    updateSlots();          // move all the ship data one slot (or frame) to the right -JNE
    int firstOpen;
    firstOpen = 0;
    int i;
    for (i=0;i<num_ship;i++) {      // go through each ship on screen, updating their position and adding them if they are not there -JNE
        firstOpen = updateFirstOpen();
        addNewShip(firstOpen, i);
    }
    sortShips();
    if (reload > 0) reload--;
}
// End of methods to help AI_loop -JNE
//THE L00PZ -EGG
void AI_loop() {
    jclass cls = (*prime_env)->GetObjectClass(prime_env, prime_jobj);
    jmethodID loop = 
        (*prime_env)->GetMethodID(prime_env, cls, "AI_loop", "()V");
    (*prime_env)->CallVoidMethod(prime_env, prime_jobj, loop);
}
//END L00PZ -EGG
//Inject our loop -EGG
void injectAI() { 
    if (AI_delaystart > 2) { 
        prepareShips();
        prepareShots();
        AI_loop();
    }
    else {
        if (AI_delaystart == -5) Net_talk("/get shotspeed");
        else if (AI_delaystart == -4) Net_talk("/get firerepeatrate");
        else if (AI_delaystart < 2) {
             sscanf(TalkMsg[0]->txt, "The value of firerepeatrate is %d", &AI_repeatrate);
             sscanf(TalkMsg[1]->txt, "The value of shotspeed is %d", &AI_shotspeed);
        }
        if ((AI_shotspeed != -1 && AI_repeatrate != -1) || AI_delaystart < 1) AI_delaystart++;
    }
}
//END inject -EGG
//Run xpilot without a window -EGG
JNIEXPORT void JNICALL Java_javaAI_headlessMode(JNIEnv * env, jobject jobj) {headless = 1;}
//THE JNI TO START XPILOT CLIENT. Called as AI.start(int,String[]). -EGG
JNIEXPORT int JNICALL Java_javaAI_start(JNIEnv * env, jobject jobj, int argc, jstring args[]) {
    prime_env = env;
    prime_jobj = jobj;
    jstring s;
    char *argv[argc];
    int i,j,k;
    for(i = 0; i < argc; i++) {
        s = (*env)->GetObjectArrayElement(env, args, i); //Gets object at index i from args -EGG
        argv[i] = (*env)->GetStringUTFChars(env, s, NULL); //Converts jstring to char[] and places in argv -EGG
    }
    ship_t theShip;
    theShip.x=-1;
    theShip.y=-1;
    theShip.dir=-1;
    theShip.shield=-1;
    theShip.id=-1;
    for (j=0;j<128;j++) {	// Initialize allShips for enemy velocity
        for (k=0;k<3;k++) {
            allShips[j][k].vel=-1;
            allShips[j][k].d=9999;		// needs to be arbitrarily high so that it is sorted correctly in allShips
            allShips[j][k].ship = theShip;
            allShips[j][k].xVel=-1;
            allShips[j][k].yVel=-1;
            allShips[j][k].trackingDeg=-1;
            allShips[j][k].trackingRad=-1;
            allShips[j][k].reload=-1;
        }
    }
    // AI.c
    AI_delaystart = -5;
    AIshot_reset();
    AIshot_toggle = 1;
    AI_shotspeed = -1;
    AI_repeatrate = -1;
    AI_alerttimemult = 5;
    //CALL XPilot's Main (which we can't live without)! -EGG
    int retval = mainAI(argc,argv);
    //Cleanup, cleanup! Everybody do your share! -EGG
    for(i = 0; i < argc; i++) {
        s = (*env)->GetObjectArrayElement(env, args, i); //Gets object at index i from args -EGG
        (*env)->ReleaseStringUTFChars(env, s, argv[i]); //Releases memory where object was stored
    }
    return retval;
}
