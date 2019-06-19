/**
 * 
 * @todo
 *  - move strings to flash (less RAM consumption)
 *  - fix deprecated convertation form string to char* startAsTag
 *  - give example description
 */
#include <SPI.h>
#include "DW1000Ranging.h"

// connection pins
const uint8_t PIN_RST = 9; // reset pin
const uint8_t PIN_IRQ = 2; // irq pin
const uint8_t PIN_SS = SS; // spi select pin

// Anchor names
String anchor1_name = "";
String anchor2_name = "";
String anchor3_name = "";
String anchor4_name = "";
String current_addr = "";
String previous_addr = "";


// anchor ranges
double anchor1_range = 0;
double anchor2_range = 0;
double anchor3_range = 0;
double anchor4_range = 0;


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
  DW1000Ranging.useRangeFilter(true);
  
  //we start the module as a tag
  DW1000Ranging.startAsTag("7D:00:22:EA:82:60:3B:9C", DW1000.MODE_LONGDATA_RANGE_ACCURACY);
}

void loop() {
  DW1000Ranging.loop();
}

void newRange() {
  String current_addr = String(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
  
  if(!anchor1_name.equals("") && !anchor2_name.equals("") && !anchor3_name.equals("") && !anchor4_name.equals("")) { // Set range
      if (current_addr.equals(anchor1_name)) {
        anchor1_range = DW1000Ranging.getDistantDevice()->getRange();
        //Serial.print(anchor1_range); Serial.print("  m from (1 - "); Serial.print(anchor1_name); Serial.print(" )\n");
      }
      else if(current_addr.equals(anchor2_name)) {
        anchor2_range = DW1000Ranging.getDistantDevice()->getRange();
        //Serial.print(anchor2_range); Serial.print("  m from (2 - "); Serial.print(anchor2_name); Serial.print(" )\n");  
      }
      else if(current_addr.equals(anchor3_name)) {
        anchor3_range = DW1000Ranging.getDistantDevice()->getRange();
        //Serial.print(anchor3_range); Serial.print("  m from (3 - "); Serial.print(anchor3_name); Serial.print(" )\n");  
      }
      else if(current_addr.equals(anchor4_name)) {
        anchor4_range = DW1000Ranging.getDistantDevice()->getRange();
        //Serial.print(anchor4_range); Serial.print("  m from (4 - "); Serial.print(anchor4_name); Serial.print(" )\n");
      }
      Serial.print(anchor1_range); Serial.print(" m\t"); 
      Serial.print(anchor2_range); Serial.print(" m\t"); 
      Serial.print(anchor3_range); Serial.print(" m\t"); 
      Serial.print(anchor4_range); Serial.print(" m\t\n");
  }
  else { // Set anchor name
      if(!(current_addr.equals(anchor2_name)) && !(current_addr.equals(anchor3_name)) && !(current_addr.equals(anchor4_name)) && anchor1_name.equals("")) {
        anchor1_name = current_addr;
        Serial.print(anchor1_name); Serial.print(" initialized as 1\n");
      }
      else if(!(current_addr.equals(anchor1_name)) && !(current_addr.equals(anchor3_name)) && !(current_addr.equals(anchor4_name)) && anchor2_name.equals("")) {
        anchor2_name = current_addr;
        Serial.print(anchor2_name); Serial.print(" initialized as 2\n");
      }
      else if(!(current_addr.equals(anchor1_name)) && !(current_addr.equals(anchor2_name)) && !(current_addr.equals(anchor4_name)) && anchor3_name.equals("")) {
        anchor3_name = current_addr;
        Serial.print(anchor3_name); Serial.print(" initialized as 3\n");
      }
      else if(!(current_addr.equals(anchor1_name)) && !(current_addr.equals(anchor2_name)) && !(current_addr.equals(anchor3_name)) && anchor4_name.equals("")) {
        anchor4_name = current_addr;
        Serial.print(anchor4_name); Serial.print(" initialized as 4\n");
      }
      else { // Error handling
        Serial.print(current_addr); Serial.print(" Looking for all 4 tags\n");
      }   
  }
  //previous_addr = current_addr;
  
 // Serial.print("from: "); Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
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
