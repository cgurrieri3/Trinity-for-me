#include <SPI.h>
#include "AD7173.h"
#include "AD5686R.h"
#include <YetAnotherPcInt.h>

#define ResetPin        3
#define interruptPin    12

#define NofADC          4
#define ENABLED_CH      16
#define Max_msg_size    16

#define RESET_ATMEGA          0x01
#define GET_UC_TEMP           0x02
#define INIT_ADC              0x03
#define EN_ADC_LOG            0x04
#define DIS_ADC_LOG           0x05
#define GET_ADC_VAL           0x06
#define SET_LOOP_DELAY        0x07
#define INIT_DAC              0x08
#define SET_FIXED_HV          0x09
#define SET_UPDATED_HV        0x0A
#define SWITCH_TO_BIN_LOG     0x0B
#define SWITCH_TO_ASCII_LOG   0x0C

volatile bool bin_mode = false;
volatile bool dec_mode = true;
volatile bool log_data = true;
volatile int ADCFlag = 0;
volatile int sample_counter = 0;
volatile uint32_t loop_delay = 10000;
volatile double adc_vals[NofADC][16];
//const float HV_offset[4] = {0.0, 0.08, 0.16, 0.29};
//const float HV_Fixed_Val[4] = {42.00, 42.08, 42.16, 42.29};
const float HV_offset[8] = {0.0, 0.08, 0.16, 0.29, 1.0, 1.08, 1.16, 1.29};
const float HV_Fixed_Val[8] = {41.00, 41.50, 42.00, 42.50, 43.00, 43.50, 44.00, 44.5};
const char *HK_list[] = {"BP0", "BP1", "HK1", "HK2"}; 
const int DAC_Ch_list[4] = {AD5686R_ADDR_DAC_A, AD5686R_ADDR_DAC_B, AD5686R_ADDR_DAC_C, AD5686R_ADDR_DAC_D};

double convfactor[64] = {2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05,  // BP0 - CH0 to CH7
                         2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05,  // BP0 - CH8 to CH15
                         2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05,  // BP1 - CH0 to CH7
                         2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05, 2.441406E-05,  // BP1 - CH8 to CH15
                         2.441406E-06, 2.441406E-06, 2.441406E-06, 2.441406E-06, 2.441406E-06, 2.441406E-06, 2.441406E-06, 2.441406E-06,  // HK-ADC - CH0 to CH7  - 8 HVPS current consumption channel
                         2.441406E-04, 2.441406E-04, 2.441406E-04, 2.441406E-04, 2.441406E-04, 2.441406E-04, 2.441406E-05, 2.441406E-05,  // HK-ADC - CH8 to CH15 - AsAd1,AsAd2,TB,CoBo,CPU-5V,CPU-12v,TMP-Sensor
                         1.245117E-05, 1.245117E-05, 1.245117E-05, 1.245117E-05, 1.245117E-05, 1.245117E-05, 1.245117E-05, 1.245117E-05,  // HK-ADC - CH0 to CH7 - HV Value
                         1.96695328E-07, 1.96695328E-07, 1.96695328E-07, 1.96695328E-07, 1.96695328E-07, 1.96695328E-07, 1.96695328E-07, 1.96695328E-07};

void Serial_Flush()
{
  while(Serial.available() > 0)
  {
    char t = Serial.read();
  }
}

int HextoInt(char str[])
{
  return (int)strtol(str, 0, 16);
}

void PrintHex32(uint32_t value)
{
  char rtmp[8], ltmp[8];
  uint32_t LeftBytes  = ((value >> 16) & 0xFFFF);
  uint32_t RightBytes = (value & 0xFFFF);
  sprintf(ltmp, "0x%.4X", LeftBytes);
  sprintf(rtmp, "%.4X", RightBytes);
  Serial.print(ltmp);
  Serial.print(rtmp);
}

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial_Flush();
  delay(100);
  Serial.print("****\t");
  Serial.println("Welcome to LVPS/HVPS Controller");
  
  pinMode(CSDAC, OUTPUT);
  digitalWrite(CSDAC, HIGH);
  delay(100);
  pinMode(interruptPin, INPUT);
  for (int i=0; i<NofADC; i++)
  {
    pinMode(CSHK[i], OUTPUT);
    digitalWrite(CSHK[i], HIGH);
    delay(100);
  }
  
  Init_DAC();
  delay(500);
  Set_Fixed_HV();
  delay(1000);
  Init_ADC();
  delay(500);
}

void loop()
{
  char message[Max_msg_size];
  uint8_t message_pos = 0;
  uint8_t cmd[8] = {0};
  
  while ((Serial.available() > 0) && (message_pos < Max_msg_size))
  {
    char inByte = Serial.read();
    message[message_pos] = inByte;
    message_pos++;
        
    if(message_pos == 16)
    {
      Serial_Flush();
      
      //Serial.print("****\t");
      for(int i=0; i<8; i++)
      {
        char tmp[3];
        strncpy(tmp, message+2*i, 2);
        tmp[2]='\0';
        cmd[i]=HextoInt(tmp);
        //Serial.print(cmd[i], HEX);
        //Serial.print("\t");
      }
      //Serial.println("");
      
      switch(cmd[0])
      {
        case RESET_ATMEGA:
          Reset_Atmega328P();
          break;
        case GET_UC_TEMP:
          Get_UC_Temp();
          break;
        case INIT_ADC:
          Init_ADC();
          break;
        case EN_ADC_LOG:
          Enable_ADC_LOG();
          break;
        case DIS_ADC_LOG:
          Disable_ADC_LOG();
          break;
        case GET_ADC_VAL:
          Get_ADC_Value();
          break;
        case SET_LOOP_DELAY:
          Set_Loop_Delay(cmd);
          break;
        case INIT_DAC:
          Init_DAC();
          break;
        case SET_FIXED_HV:
          Set_Fixed_HV();
          break;
        case SET_UPDATED_HV:
          Set_Updated_HV(cmd);
          break;
        case SWITCH_TO_BIN_LOG:
          Switch_to_Binary_Log();
          break;
        case SWITCH_TO_ASCII_LOG:
          Switch_to_Ascii_Log();
          break;
      } // end of switch case
    } // end of if statement
  } // end of while loop

  if(log_data)
  {
    Get_ADC_Value();
  }
  delay(loop_delay);
}

void Reset_Atmega328P()
{
  Serial.println("****\tReset Command is received");
  delay(50);
  digitalWrite(ResetPin, HIGH);
  delay(200);
  pinMode(ResetPin, OUTPUT);
  delay(200);
  digitalWrite(ResetPin, LOW);
}

uint32_t Get_UC_Temp()
{
  uint32_t wADC;
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));  // Set the internal reference and mux.
  ADCSRA |= _BV(ADEN);                            // enable the ADC
  delay(50);                                      // wait for voltages to become stable.
  ADCSRA |= _BV(ADSC);                            // Start the ADC
  while (bit_is_set(ADCSRA,ADSC));                // Detect end-of-conversion
  wADC = ADCW;                                    // Reading register "ADCW" takes care of how to read ADCL and ADCH.
  Serial.print("****\t");
  Serial.print("UC Temp: ");
  Serial.println(wADC);
  return wADC;
}

void Init_ADC()
{
  AD7173.init();
  for (int i=0; i<NofADC; i++){
    AD7173.init_ADC(i+1);
    delay(500);
  }  
  PcInt::attachInterrupt(interruptPin, ADC_ready_int, (const char*) 0 , FALLING);
  delay(500);
}

void Init_DAC()
{
  AD5686R_Init();
  AD5686R_Reset();
  AD5686R_WriteFunction(AD5686R_CMD_SET_DCEN, AD5686R_ADDR_DAC_ALL, AD5686R_DCEN_ENABLE);
  AD5686R_InternalVoltageReference(AD5686R_INT_REF_ON);
  Reset_All_HV();
}

void ADC_ready_int(const char* message, bool pinstate)
{
  cli();
  if (sample_counter < 16)
  {
    byte result[4];
    AD7173.get_data(result);
    if (result[3]&0x70) {return;}       //check for any error bits
    uint8_t channel = result[3]&0x0f;   //channel is last 4 bits
    adc_vals[ADCFlag][channel] = (((uint32_t)result[0] << 16) | ((uint32_t)result[1] << 8) | ((uint32_t)result[2]));
    sample_counter++;
  }
  sei();
}

void Enable_ADC_LOG()
{
  log_data = true;
}

void Disable_ADC_LOG()
{
  log_data = false;
}

void Get_ADC_Value()
{
  for (int i=0; i<NofADC; i++)
  {
    ADCFlag = i;
    sample_counter = 0;
    digitalWrite(CSHK[i], LOW);
    delay(32);
    digitalWrite(CSHK[i], HIGH);
  }

  if(bin_mode)
  {
    for (int i=0; i<NofADC; i++)
    {
      for (int j=0; j<ENABLED_CH; j++)
      {
        PrintHex32(adc_vals[i][j]);
      }
    }
    Serial.println("");
  }

  if(dec_mode)
  {
    Serial.print("****\t");
    Serial.print("\t");
    for (int i=0; i<ENABLED_CH; i++)
    {
      Serial.print(i+1);
      Serial.print("\t");
    }
    Serial.println("");
    for (int i=0; i<NofADC; i++)
    {
      Serial.print("****\t");
      Serial.print(HK_list[i]);
      Serial.print("\t");
      for (int j=0; j<ENABLED_CH; j++)
      {
        double value = adc_vals[i][j]*convfactor[i*16+j];
        Serial.print(value, 3);
        Serial.print("\t");
      }
      Serial.println("");
    }
    Serial.println("");
  }
  memset(adc_vals, 0, sizeof(adc_vals));
}

void Set_Loop_Delay(uint8_t cmd[])
{
  loop_delay = (((uint32_t)cmd[1] << 8) | (uint32_t)cmd[2]);
}

void Reset_All_HV()
{
  Serial.print("****\t");
  Serial.println("Resetting HV on All Channels to zero ...");
  digitalWrite(CSDAC,LOW);
  for(int i=0; i<8; i++)
  {
    AD5686R_SetVoltage(AD5686R_ADDR_DAC_ALL, (float)0);
    delay(50);
  }
  digitalWrite(CSDAC,HIGH);
}

void Set_Fixed_HV()
{
  Reset_All_HV();

  digitalWrite(CSDAC,LOW);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Top Channel 1 to ");
  Serial.println(HV_Fixed_Val[4]);
  AD5686R_SetVoltage(DAC_Ch_list[0], HV_Fixed_Val[4]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Top Channel 2 to ");
  Serial.println(HV_Fixed_Val[5]);
  AD5686R_SetVoltage(DAC_Ch_list[1], HV_Fixed_Val[5]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Top Channel 3 to ");
  Serial.println(HV_Fixed_Val[6]);
  AD5686R_SetVoltage(DAC_Ch_list[2], HV_Fixed_Val[6]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Top Channel 4 to ");
  Serial.println(HV_Fixed_Val[7]);
  AD5686R_SetVoltage(DAC_Ch_list[3], HV_Fixed_Val[7]);
  delay(200);
  
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Bottom Channel 1 to ");
  Serial.println(HV_Fixed_Val[0]);
  AD5686R_SetVoltage(DAC_Ch_list[0], HV_Fixed_Val[0]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Bottom Channel 2 to ");
  Serial.println(HV_Fixed_Val[1]);
  AD5686R_SetVoltage(DAC_Ch_list[1], HV_Fixed_Val[1]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Bottom Channel 3 to ");
  Serial.println(HV_Fixed_Val[2]);
  AD5686R_SetVoltage(DAC_Ch_list[2], HV_Fixed_Val[2]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Setting Fixed HV on Bottom Channel 4 to ");
  Serial.println(HV_Fixed_Val[3]);
  AD5686R_SetVoltage(DAC_Ch_list[3], HV_Fixed_Val[3]);
  delay(200);
  digitalWrite(CSDAC,HIGH);
}

void Set_Updated_HV(uint8_t cmd[])
{
  float HV_Val[8] = {0.0};
  float exponent = ((float)cmd[1]*10.0 + (float)cmd[2]);
  float fraction = (((float)cmd[3]*10.0 + (float)cmd[4]) * 0.01);
  float HV_Val_Base = exponent + fraction;
  for(int i=0; i<8; i++)
  {
    HV_Val[i] = HV_Val_Base + HV_offset[i];
  }

  Reset_All_HV();

  digitalWrite(CSDAC,LOW);
  Serial.print("****\t");
  Serial.print("Updating HV on Top Channel 1 to ");
  Serial.println(HV_Val[4]);
  AD5686R_SetVoltage(DAC_Ch_list[0], HV_Val[4]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Top Channel 2 to ");
  Serial.println(HV_Val[5]);
  AD5686R_SetVoltage(DAC_Ch_list[1], HV_Val[5]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Top Channel 3 to ");
  Serial.println(HV_Val[6]);
  AD5686R_SetVoltage(DAC_Ch_list[2], HV_Val[6]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Top Channel 4 to ");
  Serial.println(HV_Val[7]);
  AD5686R_SetVoltage(DAC_Ch_list[3], HV_Val[7]);
  delay(200);

  Serial.print("****\t");
  Serial.print("Updating HV on Bottom Channel 1 to ");
  Serial.println(HV_Val[0]);
  AD5686R_SetVoltage(DAC_Ch_list[0], HV_Val[0]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Bottom Channel 2 to ");
  Serial.println(HV_Val[1]);
  AD5686R_SetVoltage(DAC_Ch_list[1], HV_Val[1]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Bottom Channel 3 to ");
  Serial.println(HV_Val[2]);
  AD5686R_SetVoltage(DAC_Ch_list[2], HV_Val[2]);
  delay(200);
  Serial.print("****\t");
  Serial.print("Updating HV on Bottom Channel 4 to ");
  Serial.println(HV_Val[3]);
  AD5686R_SetVoltage(DAC_Ch_list[3], HV_Val[3]);
  delay(200);
  digitalWrite(CSDAC,HIGH);
}

void Switch_to_Binary_Log()
{
  dec_mode = false;
  bin_mode = true;
}

void Switch_to_Ascii_Log()
{
  bin_mode = false;
  dec_mode = true;
}
