#include <deprecated.h>
#include <DW1000.h>
#include <DW1000CompileOptions.h>
#include <DW1000Constants.h>
#include <DW1000Device.h>
#include <DW1000Mac.h>
#include <DW1000Ranging.h>
#include <DW1000Time.h>
#include <require_cpp11.h>

/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
//#include "DW1000Ranging.h"

// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

//initialize distance buffer to 0
float distance[4] = {0};
//initialize anchor position buffers

// Anchor 0, A
float x1 =7.75;
float y1 =0;
float z1 =2.18;

// Anchor 1, B 
float x2 =0;
float y2 =0;
float z2 =1.72;

// Anchor 2, C
float x3 =7.75;
float y3 =4.05;
float z3 =1.5;

// Anchor 3, D
float x4 =0;
float y4 =4.05;
float z4 =1.27;

float x[4] = {x1,x2,x3,x4};
float y[4] = {y1,y2,y3,y4};
float z[4] = {z1,z2,z3,z4};

//float XYZ[3] = {0};
float XYZ[3] = {0};


void setup() {
  Serial.begin(115200);
  delay(1000);
  //init the configuration
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); //Reset, CS, IRQ pin
  //define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  //Enable the filter to smooth the distance
  //DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY,false);
}

void loop() {
  DW1000Ranging.loop();
}

float toPos(float in) {
  if (in < 0) {
    in *= -1;
  }
  return in;
}

float toNeg(float in) {
  if (in > 0) {
    in *= -1;
  }
  return in;
}

float calibrateForError(float raw) {
  double error = 3*pow(10,-6)*pow(raw,6) - 2*pow(10,-5)*pow(raw,5) - 0.0011*pow(raw,4) + 0.0175*pow(raw,3) - 0.0959*pow(raw,2) + 0.2274*raw + 0.1322;
  error += 0.3;
  if(raw<.5) error += 0.16; 
  return raw - error;
}


void solveComponents(float * d, float * x, float * y, float * z) {

// Matrix M of n anchors

//float a = x[0] - x[1];
//float b = y[0] - y[1];
//float c = z[0] - z[1];
//float d = x[0] - x[2];
//float e = y[0] - y[2];
//float f = z[0] - z[2];
//float g = x[0] - x[3];
//float h = y[0] - y[3];
//float i = z[0] - z[3];
//
//// Vector R of n dimensions
//float R1 = pow(r[1], 2) - pow(r[0], 2) + pow(x[0], 2) - pow(x[1], 2) + pow(y[0], 2) - pow(y[1], 2) + pow(z[0], 2) - pow(z[1], 2);
//float R2 = pow(r[2], 2) - pow(r[0], 2) + pow(x[0], 2) - pow(x[2], 2) + pow(y[0], 2) - pow(y[2], 2) + pow(z[0], 2) - pow(z[2], 2);
//float R3 = pow(r[3], 2) - pow(r[0], 2) + pow(x[0], 2) - pow(x[3], 2) + pow(y[0], 2) - pow(y[3], 2) + pow(z[0], 2) - pow(z[3], 2);
//
//// Invert Matrix M (3x3 matrix) by method of cofactors
//
//// Replace every entry by its minor
//
//float a1 = f*h - e*i;
//float b1 = f*g - d*i;
//float c1 = e*g - d*h;
//float d1 = c*h - b*i;
//float e1 = c*g - a*i;
//float f1 = b*g - a*h;
//float g1 = c*e - b*f;
//float h1 = c*d - a*f;
//float i1 = b*d - a*e;
//
//// Change signs of all entries
//
//float a2 = toPos(a1);
//float b2 = toNeg(b1);
//float c2 = toPos(c1);
//float d2 = toNeg(d1);
//float e2 = toPos(e1);
//float f2 = toNeg(f1);
//float g2 = toPos(g1);
//float h2 = toNeg(h1);
//float i2 = toPos(i1);
//
//// Transpose matrix
//
//float a3 = a2;
//float b3 = d2;
//float c3 = g2;
//float d3 = b2;
//float e3 = e2;
//float f3 = h2;
//float g3 = c2;
//float h3 = f2;
//float i3 = i2;
//
//// Divide by the determinant of original matrix M
//
//float Mdet = (a * e * i + b * f * g + c * d * h) - (b * d * i + a * f * h + c * e * g);
//a3 = a3 / Mdet;
//b3 = b3 / Mdet;
//c3 = c3 / Mdet;
//d3 = d3 / Mdet;
//e3 = e3 / Mdet;
//f3 = f3 / Mdet;
//g3 = g3 / Mdet;
//h3 = h3 / Mdet;
//i3 = i3 / Mdet;
//
//// Multiply vector R by Matrix M
//float X = a3*R1 + b3*R2 + c3*R3;
//float Y = d3*R1 + e3*R2 + f3*R3;
//float Z = g3*R1 + h3*R2 + i3*R3;
//
//// Divide XYZ by 2 to capture coefficient
//X = X/2;
//Y = Y/2;
//Z = Z/2;
//
//// Return XYZ coordinates
//XYZ[0] = X;
//XYZ[1] = Y;
//XYZ[2] = Z;
  
////void solveComponents(float * d, float * x, float * y, float * z) {
////
////x[0] = 0;
////x[1] = 0.6;
////x[2] = 0;
////x[3] = 0.6;
////y[0] = 1.02;
////y[1] = 0;
////y[2] = 0;
////y[3] = 1.02;
////z[0] = 0;
////z[1] = 0;
////z[2] = 0;
////z[3] = 0;
////d[0] = 0.61;
////d[1] = 0.61;
////d[2] = 0.61;
////d[3] = 0.61;
////
//// Initialize coomon tri elements
float tri2 = (pow(d[0],2) - pow(d[1], 2)) - (pow(x[0],2) - pow(x[1], 2)) - (pow(y[0],2)-pow(y[1],2));
float tri3 = (pow(d[0],2) - pow(d[2], 2)) - (pow(x[0],2) - pow(x[2], 2)) - (pow(y[0],2)-pow(y[2],2));

//Serial.print("Tri2: "); Serial.print(tri2); Serial.print("\t"); Serial.print("Tri3: "); Serial.print(tri3); Serial.print("\n");


// Solve X numerator elements

float Xa = tri2;
float Xb = 2*(y[1]-y[0]);
float Xc = tri3;
float Xd = 2*(y[2]-y[0]);

//Serial.print("Xb: "); Serial.print(Xb); Serial.print("\t"); Serial.print("Xd: "); Serial.print(Xd); Serial.print("\n");

float Ya = 2*(x[1]-x[0]);
float Yb = tri2;
float Yc = 2*(x[2]-x[0]);
float Yd = tri3;

//Serial.print("Ya: "); Serial.print(Ya); Serial.print("\t"); Serial.print("Yc: "); Serial.print(Yc); Serial.print("\n");


// Solve XY denominator elements
float Xa2 = 2*(x[1]-x[0]);
float Xb2 = 2*(y[1]-y[0]);
float Xc2 = 2*(x[2]-x[0]);
float Xd2 = 2*(y[2]-y[0]);

//Serial.print("Xa2: "); Serial.print(Xa2); Serial.print("\t"); Serial.print("Xb2: "); Serial.print(Xb2); Serial.print("\n");
//Serial.print("Xc2: "); Serial.print(Xc2); Serial.print("\t"); Serial.print("Xd2: "); Serial.print(Xd2); Serial.print("\n");

// Solve numerator determinant
float Xnum = Xa * Xd - Xb * Xc;
float Ynum = Ya * Yd - Yb * Yc;

//Serial.print("Xnum: "); Serial.print(Xnum); Serial.print("\t"); Serial.print("Ynum: "); Serial.print(Ynum); Serial.print("\n");


// Solve denominator determinant
float Xden = Xa2 * Xd2 - Xb2 * Xc2;
float Yden = Xden;

//Serial.print("Xden: "); Serial.print(Xden); Serial.print("\t"); Serial.print("Yden: "); Serial.print(Yden); Serial.print("\n");


float X = Xnum / Xden;
float Y = Ynum / Yden;

//Serial.print("X: "); Serial.print(X); Serial.print("\t"); Serial.print("Y: "); Serial.print(Y); Serial.print("\n");


XYZ[0] = X;
XYZ[1] = Y;



//// Initialize common tri elements
//
//float tri2 = (pow(d[0],2)-pow(d[1],2)) - (pow(x[0],2)-pow(x[1],2)) - (pow(y[0],2)-pow(y[1],2)) - (pow(z[0],2) - pow(z[1],2));
//Serial.print("Tri2: "); Serial.print(tri2); Serial.print("\t");
//float tri3 = (pow(d[0],2)-pow(d[2],2)) - (pow(x[0],2)-pow(x[2],2)) - (pow(y[0],2)-pow(y[2],2)) - (pow(z[0],2) - pow(z[2],2));
//Serial.print("Tri3: "); Serial.print(tri3); Serial.print("\t"); 
//float tri4 = (pow(d[0],2)-pow(d[3],2)) - (pow(x[0],2)-pow(x[3],2)) - (pow(y[0],2)-pow(y[3],2)) - (pow(z[0],2) - pow(z[3],2));
//Serial.print("Tri4: "); Serial.print(tri4); Serial.print("\n"); 
//
//
//// Solve X numerator elements
//
//float Xa = tri2;
//float Xb = 2*(y[1]-y[0]);
//float Xc = 2*(z[1]-z[0]);
//float Xd = tri3;
//float Xe = 2*(y[2]-y[0]);
//float Xf = 2*(z[2]-z[0]);
//float Xg = tri4;
//float Xh = 2*(y[3]-y[0]);
//float Xi = 2*(z[3]-z[0]);
//
//Serial.print("Xb: "); Serial.print(Xb); Serial.print("\t");
//Serial.print("Xc: "); Serial.print(Xc); Serial.print("\t");
//Serial.print("Xe: "); Serial.print(Xe); Serial.print("\t");
//Serial.print("Xf: "); Serial.print(Xf); Serial.print("\t");
//Serial.print("Xh: "); Serial.print(Xh); Serial.print("\t");
//Serial.print("Xi: "); Serial.print(Xi); Serial.print("\n");
//
//
//// Solve Y numerator elements
//
//float Ya = 2*(x[1]-x[0]);
//float Yb = tri2;
//float Yc = 2*(z[1]-z[0]);
//float Yd = 2*(x[2]-x[0]);
//float Ye = tri3;
//float Yf = 2*(z[2]-z[0]);
//float Yg = 2*(x[3]-x[0]);
//float Yh = tri4;
//float Yi = 2*(z[3]-z[0]);
//
//// Solve Z numerator elements
//
//float Za = 2*(x[1]-x[0]);
//float Zb = 2*(y[1]-y[0]);
//float Zc = tri2;
//float Zd = 2*(x[2]-x[0]);
//float Ze = 2*(y[2]-y[0]);
//float Zf = tri3;
//float Zg = 2*(x[3]-x[0]);
//float Zh = 2*(y[3]-y[0]);
//float Zi = tri4;
//
//
//// Solve X Y Z denominator elements
//
//float Xa2 = 2*(x[1]-x[0]);
//float Xb2 = 2*(y[1]-y[0]);
//float Xc2 = 2*(z[1]-z[0]);
//float Xd2 = 2*(x[2]-x[0]);
//float Xe2 = 2*(y[2]-y[0]);
//float Xf2 = 2*(z[2]-z[0]);
//float Xg2 = 2*(x[3]-x[0]);
//float Xh2 = 2*(y[3]-y[0]);
//float Xi2 = 2*(z[3]-z[0]);
//
//
//Serial.print("Xa2: "); Serial.print(Xa2); Serial.print("\t");
//Serial.print("Xb2: "); Serial.print(Xb2); Serial.print("\t");
//Serial.print("Xc2: "); Serial.print(Xc2); Serial.print("\t");
//Serial.print("Xd2: "); Serial.print(Xd2); Serial.print("\t");
//Serial.print("Xe2: "); Serial.print(Xe2); Serial.print("\t");
//Serial.print("Xf2: "); Serial.print(Xf2); Serial.print("\n");
//Serial.print("Xg2: "); Serial.print(Xg2); Serial.print("\t");
//Serial.print("Xh2: "); Serial.print(Xh2); Serial.print("\t");
//Serial.print("Xi2: "); Serial.print(Xi2); Serial.print("\n");
//
//// Find X numerator determinant
//float Xnum = (Xa * Xe * Xi + Xb * Xf * Xg + Xc * Xd * Xh) - (Xb * Xd * Xi + Xa * Xf * Xh + Xc * Xe * Xg);
//// Find Y numerator determinant
//float Ynum = (Ya * Ye * Yi + Yb * Yf * Yg + Yc * Yd * Yh) - (Yb * Yd * Yi + Ya * Yf * Yh + Yc * Ye * Yg);
//// Find Z numerator determinant
//float Znum = (Za * Ze * Zi + Zb * Zf * Zg + Zc * Zd * Zh) - (Zb * Zd * Zi + Za * Zf * Zh + Zc * Ze * Zg);
//// Find X Y Z denominator determinant
//float Xden = (Xa2 * Xe2 * Xi2 + Xb2 * Xf2 * Xg2 + Xc2 * Xd2 * Xh2) - (Xb2 * Xd2 * Xi2 + Xa * Xf2 * Xh2 + Xc2 * Xe2 * Xg2);
//float Yden = Xden;
//float Zden = Xden;
//
//// Calculate final X Y Z coordinates
//float X = Xnum / Xden;
//float Y = Ynum / Yden;
//float Z = Znum / Zden;
//
//Serial.print("XYZnum: "); Serial.print(Xnum); Serial.print("\t"); Serial.print(Ynum); Serial.print("\t"); Serial.print(Znum); Serial.print("\t"); Serial.print("\n");
//Serial.print("XYZden: "); Serial.print(Xden); Serial.print("\t"); Serial.print(Yden); Serial.print("\t"); Serial.print(Zden); Serial.print("\t"); Serial.print("\n");
//
//
//Serial.print("XYZ: "); Serial.print(X); Serial.print("\t"); Serial.print(Y); Serial.print("\t"); Serial.print(Z); Serial.print("\t"); Serial.print("\n");
//
//// Set coordinates as 3 column vector
//XYZ[0] = X;
//XYZ[1] = Y;
//XYZ[2] = Z;

}

void newRange() {
  switch(DW1000Ranging.getDistantDevice()->getShortAddress()) {
    case 1: distance[0] = calibrateForError(DW1000Ranging.getDistantDevice()->getRange()); break;
    case 2: distance[1] = calibrateForError(DW1000Ranging.getDistantDevice()->getRange()); break;
    case 3: distance[2] = calibrateForError(DW1000Ranging.getDistantDevice()->getRange()); break;
    case 4: distance[3] = calibrateForError(DW1000Ranging.getDistantDevice()->getRange()); break;
    default: Serial.print("Unknown Anchor, distance not updated");
  }

    if((distance[0] != 0) && (distance[1] != 0) && (distance[2] != 0) && (distance[3] != 0)) {
      solveComponents(distance, x, y, z);
      //print out XYZ location of tag
      
      Serial.print("~ ");
      for(int i=0; i<3; i++) {
        Serial.print(XYZ[i]);Serial.print(" ");
      }
      Serial.println("");
      
  }

//  if((distance[0] != 0) && (distance[0] != 0) && (distance[0] != 0) && (distance[0] != 0)) {
//      float * XYZ = solveComponents(distance, x, y, z);
//      //print out XYZ location of tag
//      for(int i=0; i<3; i++) {
//        Serial.print(XYZ[i]);Serial.print(" ");
//      }
//        Serial.print("\n");
//  }

//    //print out tuples of distances
//  for(int i=0; i<3; i++) {
//    Serial.print(distance[i]);Serial.print(" ");
//  }
//  Serial.println(distance[3]); //damn you fencepost problem.
  
  //Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  //Serial.print("\t Range: "); Serial.print(DW1000Ranging.getDistantDevice()->getRange()); Serial.print(" m");
  //Serial.print("\t RX power: "); Serial.print(DW1000Ranging.getDistantDevice()->getRXPower()); Serial.println(" dBm");
}

void newDevice(DW1000Device* device) {
  Serial.print("ranging init; 1 device added ! -> ");
  Serial.print(" short:");
  Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device* device) {
  Serial.print("delete inactive device: ");
  Serial.println(device->getShortAddress(), HEX);
}
