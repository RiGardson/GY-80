#include "mpu6050.h"


void mpu_config (void){
	// power management register 0X6B we should write all 0's to wake the sensor up
  // Set DATA RATE of 1KHz by writing SMPLRT_DIV register
	Write_byte (GY80_ADDR, CTRL_REG1, 0x4F);
	
	// Set Gyroscopic configuration in GYRO_CONFIG Register
        // XG_ST=0,YG_ST=0,ZG_ST=0, FS_SEL=0 -> ? 250 ^0/s
	Write_byte (GY80_ADDR, CTRL_REG4, 0x80);
}



	//--------------MPU6050 Config------------------------------
void Write_byte (uint8_t DevADDR, uint8_t Mem_Rigister, uint8_t Data){ 
	Start();
	I2C2->DR = DevADDR;
	while ((I2C2->SR1&I2C_SR1_ADDR) != I2C_SR1_ADDR);//EV6
	(void)(I2C2->SR1|I2C2->SR2);
  while ((I2C2->SR1 & I2C_SR1_TXE) != I2C_SR1_TXE);
	
  SUB_single(Mem_Rigister);
	SendLASTDATA(Data);
	Stop();
}
	
//---------------------------------------------------------------------------
void READ_bytes(uint8_t DevADDR, uint8_t Mem_Rigister, uint8_t *Data){
	int8_t i=0;
	Start();
	SendADDR_Wite(DevADDR);
	SUB_mutil(Mem_Rigister);
	
	I2C2->CR1 |= I2C_CR1_START;//S
	while ((I2C2->SR1 & I2C_SR1_SB ) != I2C_SR1_SB);//WAIT SB = 1//EV5
	(void) I2C2->SR1;
	
	I2C2->DR = 0xD3;// (DevADDR|0x01);
	while (!(I2C2->SR1 & I2C_SR1_ADDR));  // wait for ADDR bit to set
	 // read SR1 and SR2 to clear the ADDR bit
	(void) I2C2->SR1;
	(void) I2C2->SR2;
	
		for(i=0;i<6;i++)
	{
	  if(i<(3))
	  {
		  while (!(I2C2->SR1 & I2C_SR1_RXNE)){};
		  Data[i] = I2C2->DR;
	  }
		//When 3 bytes remain to be read:
	  else
	  {
			if (i==3)
				{
	  	while (!(I2C2->SR1 & I2C_SR1_RXNE)){};//RxNE = 1 => Nothing (DataN-2 not read).
				//DataN-1 received	
			while (!(I2C2->SR1 & I2C_SR1_BTF)){};//BTF = 1 because both shift and data registers are full: DataN-2 in DR and DataN-1 in
                                         //the shift register => SCL tied low: no other data will be received on the bus.
	    I2C2->CR1 &= ~I2C_CR1_ACK;
	    Data[i] = I2C2->DR;//Read DataN-2 in DR
	    I2C2->CR1 |= I2C_CR1_STOP;
			}
				else{
					if(i==4){
					Data[i] = I2C2->DR;//Read DataN-1
					while (!(I2C2->SR1 & I2C_SR1_RXNE)){};
					}
					else{
					Data[i] = I2C2->DR;
					}
			}//Read DataN
	  }
	}
}	
//---------------------------------------------------------------	

