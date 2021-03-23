// duck for RP2040 microcontroller + pico display
// NC 10.03.21

#include "pico_display.hpp"
#include <math.h>
#include <time.h>
#include <cstdlib>

using namespace pimoroni;
extern unsigned char _binary_mallard_tga_start[];

uint16_t buffer[PicoDisplay::WIDTH * PicoDisplay::HEIGHT];
PicoDisplay pico_display(buffer);

int angle;
int angle_offset = 0;
int y_offset = 34;
int dukx=100;
int duky=50;
int duk_width = 79;
int duk_height = 53;
int duk_knots = 2;
int sunx = 210;
int suny = 105;

int sleepy_time_ms = 1;   

uint8_t r,g,b ;

const uint16_t white_cloud = pico_display.create_pen(240,240,240);
const uint16_t green_reed  = pico_display.create_pen(0,220,31);
const uint16_t sky_blue    = pico_display.create_pen(51, 153, 255);
const uint16_t sea_blue    = pico_display.create_pen(0, 51, 204);
const uint16_t sun         = pico_display.create_pen(255,117,0);

double degtorad(double degree) {  // degrees > radians
    double pi = 3.14159265359; 
    return (degree * (pi / 180)); 
}

class cloud {
   public:
     int wait;
     int nump;
     int y;
     int x;
     int offset;
     int cycles = 0;
     int cparts[10][2];

     cloud(int n, int h, int v, int d, int f) {
        nump = n;
        x = h;
        y = v;
        wait = d;
        offset = f;
        upset();
     }

     void upset(){
         for (int i=0; i<nump; i++){
             cparts[i][1] = rand() % 10;
             cparts[i][2] = rand() % 8 + 5;                       
         } 
     }

     void show(){
        pico_display.set_pen(white_cloud);
        for (int i=0; i<nump; i++){
            pico_display.circle(Point(x+(i*offset), y+cparts[i][1]), cparts[i][2]); 
        }
        cycles++;        
        if (cycles > wait){
           x--;
           cycles = 0;
        }
        if (x < -100){
            x = 245;
            cycles = 0;
            upset();
        }
    }
};

class reedClump {
    public:
        int x;
        int cycles=0;
        int wait, wait2;
        int rparts[30][4];
        int stalks;

    reedClump(int w, int w2) {              
        wait = w;
        wait2 = w2;
        x = rand() % 100 + 239;                // init display x  
        upset();
    }

    void upset(){
        stalks = rand() % 23+6;                // clump size
        for(int i=0; i<stalks; i++){
            rparts[i][0] = rand() % 45 + 45;   // point
            rparts[i][1] = rand() % 30;        // point offset
            rparts[i][2] = rand() % 10;        // base width
            rparts[i][3] = rand() % 100;       // base offset
        }
    }

    void show(){
        pico_display.set_pen(green_reed);
        for( int i=0; i<stalks; i++){
            pico_display.triangle(\
            Point(x+rparts[i][3], 0), \
            Point(x+rparts[i][2]+rparts[i][3], 0), \
            Point(x+rparts[i][3]+(rparts[i][2]/2) +10 -(rparts[i][1]/2) , rparts[i][0]));
        } 
        cycles++;        
        if (cycles > wait){
           x--;
           cycles = 0;
        }

        if (x < -100){
            x = rand() % wait2 + 239;
            cycles = 0;
            upset();
        } 
    } 
};

void duck(int xpos, int ypos){
    for(int y = 0; y < duk_height; y++) {
       uint16_t *dest = pico_display.frame_buffer + (y * (duk_width-1) * 3);    
       uint8_t *src = _binary_mallard_tga_start + 18 + (y * duk_width * 3);
      
       for(int x = 0; x < duk_width; x++) {
          b = *src++;                        // Note bgr order
          g = *src++;
          r = *src++;
          pico_display.set_pen(r, g, b);     
          if(r < 253 ) {                     // Remove red background
              pico_display.pixel(Point(x + xpos, duk_height - y + ypos));
          }
        }
    }
}

int main() {
    pico_display.init();
    pico_display.set_backlight(150);    

    cloud cloud1(9,150,90,10,12); // no blobs, init x, init y, delay, x offset
    cloud cloud2(7,50,100,8,11); 

    reedClump bed1(7, 150);          // delay cycles. gap before next instance
    reedClump bed2(5, 500);
    reedClump bed3(3, 150);

    while(true) {

        pico_display.set_pen(sky_blue);        //   blue sky background
        pico_display.clear();
        
        pico_display.set_pen(sun);             //   sun
        pico_display.circle(Point(sunx, suny), 15);
  
        cloud1.show();
        cloud2.show();

        bed1.show();

        duck(dukx -60 ,duky - 25);  

        bed2.show();

        for(int x=0; x<240; x++) {
            angle = (x*1.5)+angle_offset;
            int y = int(sin(degtorad(angle)) * 15) + y_offset;
  
            pico_display.set_pen(sea_blue);                   //dark blue wave
            pico_display.line(Point(x, 0), Point(x, y));
          
            if(x==dukx || dukx > 239 || dukx < 1){       // keep swimming if x off screen
                duky = y;
            }
        }  

        bed3.show();

        pico_display.update();
    
        sleep_ms(sleepy_time_ms);  

        angle_offset = angle_offset+10;
        if(angle_offset>360) {
            angle_offset = 0;

            dukx = dukx + duk_knots;
            if (dukx > 290) {
                 dukx = -10;
            }
        }
    } 
}


