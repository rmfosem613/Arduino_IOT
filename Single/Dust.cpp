//
//    FILE: dust.ino
//  AUTHOR: Kyuho Kim (ekyuho@gmail.com)
// CREATED: September 4, 2017
// Released to the public domain
//
#include "Dust.h" 

byte header[2] = { 0x42, 0x4d };
byte old_b=0, chksum=0, sum=0;
int bi=0, flen, pm25, pm10;

void Dust::do_char(char b, void (*function)(int, int)) {
    if (bi > 2 && old_b == header[0] && b == header[1]) {
      Serial.print("\n D Discard bi="+String(bi)+" ");
      bi = 1;
    }
    switch(bi) {
    case 0: 
      if (b == header[0]) {
        bi++; 
        //Serial.println();
        chksum = b;
      }
      break;
    case 1:
      if (b == header[1]) {
        bi++;
        chksum += b;
      } else bi = 0;
      break;
    case 2:
      chksum += b; bi++;
      flen = b*256;
      break;
    case 3:
      chksum += b; bi++;
      flen += b;
      break;
    case 4:
      chksum += b;
      pm25 = b*256; bi++;
      break;
    case 5:
      chksum += b;
      pm25 += b; bi++;
      break;
    case 8:
      chksum += b;
      pm10 = b*256; bi++;
      break;
    case 9:
      chksum += b;
      pm10 += b; bi++;
      break;
    default:
      if (bi == flen+2) {
        sum = b*256;
        bi++;
      } else if (bi == flen+3) {
        sum += b;
        if (chksum == sum) {
          function(pm25, pm10);
          bi = 0;
          old_b = b;
        } else {
          Serial.print(" Checksum Error: PMA3003 ");
          Serial.print(chksum, HEX);
          Serial.print("!=");
          Serial.print(sum, HEX);
          Serial.print("\n");
          bi = 0;
        }
      } else if (bi > flen+3) {
        bi = 0;
      } else {
        bi++;
        chksum += b;
      }
  }
  old_b = b;
}
