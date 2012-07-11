#ifndef GYRO_H_
#define GYRO_H_

/* Most of this code was adapted from the Sparkfun Example code:
  by: Jim Lindblom
  SparkFun Electronics
  date: 4/18/11
  license: CC-SA 3.0 - Use this code however you'd like, all we ask
  for is attribution. And let us know if you've improved anything!
*/

/*************************
    L3G4200D Registers
*************************/
#define L3G4200D_WHO_AM_I 0x0F
#define L3G4200D_CTRL_REG1 0x20
#define L3G4200D_CTRL_REG2 0x21
#define L3G4200D_CTRL_REG3 0x22
#define L3G4200D_CTRL_REG4 0x23
#define L3G4200D_CTRL_REG5 0x24
#define L3G4200D_REFERENCE 0x25
#define L3G4200D_OUT_TEMP 0x26
#define L3G4200D_STATUS_REG 0x27
#define L3G4200D_OUT_X_L 0x28
#define L3G4200D_OUT_X_H 0x29
#define L3G4200D_OUT_Y_L 0x2A
#define L3G4200D_OUT_Y_H 0x2B
#define L3G4200D_OUT_Z_L 0x2C
#define L3G4200D_OUT_Z_H 0x2D
#define L3G4200D_FIFO_CTRL_REG 0x2E
#define L3G4200D_FIFO_SRC_REG 0x2F
#define L3G4200D_INT1_CFG 0x30
#define L3G4200D_INT1_SRC 0x31
#define L3G4200D_INT1_TSH_XH 0x32
#define L3G4200D_INT1_TSH_XL 0x33
#define L3G4200D_INT1_TSH_YH 0x34
#define L3G4200D_INT1_TSH_YL 0x35
#define L3G4200D_INT1_TSH_ZH 0x36
#define L3G4200D_INT1_TSH_ZL 0x37
#define L3G4200D_INT1_DURATION 0x38

///Initializes the gyro
///@arg fullScale 0: 250 dps, 1: 500 dps, 2: 2000 dps
int L3G4200D_init(unsigned char fullScale) {
  cs=CS_GYRO;
  
  // Let's first check that we're communicating properly
  // The WHO_AM_I register should read 0xD3
  if(spi_read(L3G4200D_WHO_AM_I)!=0xD3)
    return -1;
    
  // Enable x, y, z and turn off power down:
  spi_write(L3G4200D_CTRL_REG1, 0b00001111);
  
  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  spi_write(L3G4200D_CTRL_REG2, 0b00000000);
  
  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  spi_write(L3G4200D_CTRL_REG3, 0b00001000);
  
  // CTRL_REG4 controls the full-scale range, among other things:
  fullScale &= 0x03;
  spi_write(L3G4200D_CTRL_REG4, fullScale<<4);
  
  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  spi_write(L3G4200D_CTRL_REG5, 0b00000000);
  
  return 0;
}

void getGyroValues(signed int gyro[3]) {
  cs=CS_GYRO;
  
  gyro[X] = (spi_read(0x29)&0xFF)<<8;
  gyro[X] |= (spi_read(0x28)&0xFF);
  
  gyro[Y] = (spi_read(0x2B)&0xFF)<<8;
  gyro[Y] |= (spi_read(0x2A)&0xFF);
  
  gyro[Z] = (spi_read(0x2D)&0xFF)<<8;
  gyro[Z] |= (spi_read(0x2C)&0xFF);
}

#endif