#include <EEPROM.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include <SI7021.h>
#include "Nextion.h"

NexText temp[11] = {NexText(0,2,"t1"),NexText(0,2,"t4"),NexText(0,2,"t7"),NexText(0,2,"t10"),NexText(0,2,"t13"),NexText(0,2,"t16"),
                    NexText(0,2,"t19"),NexText(0,2,"t22"),NexText(0,2,"t25"),NexText(0,2,"t28"),NexText(0,2,"t31")};

NexText humi[11] = {NexText(0,2,"t2"),NexText(0,2,"t5"),NexText(0,2,"t8"),NexText(0,2,"t11"),NexText(0,2,"t14"),NexText(0,2,"t17"),
                    NexText(0,2,"t20"),NexText(0,2,"t23"),NexText(0,2,"t26"),NexText(0,2,"t29"),NexText(0,2,"t32")};

NexText button_status = NexText(0,2,"t34");          
NexText device = NexText(0,2,"t0");
NexButton mode = NexButton(0, 36,"b0");

NexNumber sensor_temp = NexNumber(0,37,"n0");
NexNumber sensor_humi = NexNumber(0,41,"n1");
                
uint32_t Ds_temp[11] = {0};                    
uint32_t Dht_temp[11] = {0};
uint32_t Dht_humi[11] = {0};
uint32_t Si7021_temp[11] = {0};
uint32_t Si7021_humi[11] = {0};

bool ds_error_status[11] = {0};
bool dht_error_temp_status[11] = {0};
bool dht_error_humi_status[11] = {0};
bool si7021_error_temp_status[11] = {0};
bool si7021_error_humi_status[11] = {0};

bool ds_temp_change_lot_error[11] = {0};
bool dht_humi_change_lot_error[11] = {0};
bool dht_temp_change_lot_error[11] = {0};
bool si7021_temp_change_lot_error[11] = {0};
bool si7021_humi_change_lot_error[11] = {0};

dht DHT;
SI7021 si7021;

#define TEMPERATURE_PRECISION 8

OneWire wire[] = {OneWire(32),OneWire(31),OneWire(30),OneWire(29),OneWire(28),OneWire(27)
                  ,OneWire(26),OneWire(25),OneWire(24),OneWire(23),OneWire(22)};

DallasTemperature sensor[] = {DallasTemperature(&wire[0]),
                                  DallasTemperature(&wire[1]),
                                  DallasTemperature(&wire[2]),
                                  DallasTemperature(&wire[3]),
                                  DallasTemperature(&wire[4]),
                                  DallasTemperature(&wire[5]),
                                  DallasTemperature(&wire[6]),
                                  DallasTemperature(&wire[7]),
                                  DallasTemperature(&wire[8]),
                                  DallasTemperature(&wire[9]),
                                  DallasTemperature(&wire[10])};

DeviceAddress tempDeviceAddress;

uint32_t Setup_ds_device(uint32_t port)
{
    sensor[port-1].begin();
    sensor[port-1].getAddress(tempDeviceAddress,0);
    sensor[port-1].setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    return port;
}

float Get_ds_device_temp(uint32_t port)
{
    float temp;
    float temp_max,temp_min;
  
    sensor[port-1].requestTemperatures();
    temp=sensor[port-1].getTempC(tempDeviceAddress);
    temp_max = temp + 5;
    temp_min = temp - 5 ;
    if(Ds_temp[port-1] != 0)
    {
        if(Ds_temp[port-1] < temp_min)
        {
           ds_temp_change_lot_error[port-1] = 1;
        }
        if(Ds_temp[port-1] > temp_max)
        {
          ds_temp_change_lot_error[port-1] = 1;
        }
    }
    

    Ds_temp[port-1] = (uint32_t)temp;
    return temp;
}

void Fresh_one_device_temp_to_nextion(uint32_t num,char Temp)
{
     char Temp_buf[5];

    // if(temp > 0 && temp < 100)
    // {
          Temp_buf[0] = 'T';
          Temp_buf[1] = ':';
          Temp_buf[2] = (Temp / 10)+'0';
          Temp_buf[3] = (Temp % 10)+'0';
          Temp_buf[4] = '\0';
          temp[num-1].setText(Temp_buf);
   //  }
}

void Fresh_one_device_humi_to_nextion(uint32_t num,char Humi)
{
     char humi_buf[5];

     humi_buf[0] = 'H';
     humi_buf[1] = ':';
     humi_buf[2] = (Humi / 10)+'0';
     humi_buf[3] = (Humi % 10)+'0';
     humi_buf[4] = '\0';
     humi[num-1].setText(humi_buf);
}

bool Is_DS18B20_device(uint32_t port)
{
     sensor[port-1].begin(); 
     if(sensor[port-1].getAddress(tempDeviceAddress,0))
     {
         if(sensor[port-1].validAddress(tempDeviceAddress))
         {
             return true;
         }
         else
         {
             return false;
         }
    }
    else
    {
         return false;
    }
}

bool Is_dht_device(uint32_t port)
{
   int pin_num,chk;
   
   pin_num = 33 - port;
   chk = DHT.read21(pin_num);
   if(chk != 0)
   {
      return false;
   }
   else
   {
       return true;
   }
}

bool Is_si7021_device(uint32_t port)
{
    int pin_num,chk;

    pin_num = 33 - port;
    chk  = si7021.Si7021_read(pin_num);
    if(chk==1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

uint32_t Setup_dht_device(uint32_t port)
{
   int pin_num;
   
   pin_num = 33 - port;
   DHT.read21(pin_num);
   return port;
}

float Get_dht_device_temp(uint32_t port)
{
    float temp;
    float temp_max,temp_min;
    int pin_num;
   
    pin_num = 33 - port;
    
    temp = DHT.temperature;
    temp_max = temp + 5;
    temp_min = temp - 5 ;
    if(Dht_temp[port-1] != 0)
    {
        if(Dht_temp[port-1] < temp_min)
        {
           dht_temp_change_lot_error[port-1] = 1;
        }
        if(Dht_temp[port-1] > temp_max)
        {
          dht_temp_change_lot_error[port-1] = 1;
        }
    }
    Dht_temp[port-1] = temp;
    return temp;
}

float Get_si7021_device_temp(uint32_t port)
{
    uint8_t temp;
    uint8_t temp_max,temp_min;
    uint32_t pin_num;

    pin_num = 33 - port;
    temp = si7021.temp;
    temp_max = temp + 5;
    temp_min = temp - 5 ;
    
    if(Si7021_temp[port-1] != 0)
    {
        if(Si7021_temp[port-1] < temp_min)
        {
           si7021_temp_change_lot_error[port-1] = 1;
        }
        if(Si7021_temp[port-1] > temp_max)
        {
          si7021_temp_change_lot_error[port-1] = 1;
        }
    }
    Si7021_temp[port-1] = temp;
    return temp;
}

float Get_dht_device_humi(uint32_t port)
{
    float humi;
    float humi_max,humi_min;
    int pin_num;
   
    pin_num = 33 - port;
   
    humi = DHT.humidity;
    humi_max = humi + 10;
    humi_min = humi - 10;
    if(Dht_humi[port-1] != 0)
    {
        if(Dht_humi[port-1] < humi_min)
        {
           dht_humi_change_lot_error[port-1] = 1;
        }
        if(Dht_humi[port-1] > humi_max)
        {
          dht_humi_change_lot_error[port-1] = 1;
        }
    }
    Dht_humi[port-1] = humi;
    return humi;
}

float Get_si7021_device_humi(uint32_t port)
{
    uint8_t humi;
    uint8_t humi_max,humi_min;
    uint32_t pin_num;

    pin_num = 33 - port;
    humi = si7021.humi;
    humi_max = humi + 10;
    humi_min = humi - 10 ;
    
    if(Si7021_humi[port-1] != 0)
    {
        if(Si7021_humi[port-1] < humi_min)
        {
           si7021_humi_change_lot_error[port-1] = 1;
        }
        if(Si7021_humi[port-1] > humi_max)
        {
          si7021_humi_change_lot_error[port-1] = 1;
        }
    }
    Si7021_humi[port-1] = humi;
    return humi;
}

bool Check_button_status(void)
{
    if(digitalRead(3))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void nextion_clear_data(void)
{
    device.setText("Device");
    for(int i=0;i<11;i++)
    {
        temp[i].Set_background_color_bco(65535);
        humi[i].Set_background_color_bco(65535);
        temp[i].setText("T:");
        humi[i].setText("H:");

        Ds_temp[i] = 0;                    
        Dht_temp[i] = 0;
        Dht_humi[i] = 0;

        ds_temp_change_lot_error[i] = 0;
        dht_humi_change_lot_error[i] = 0;
        dht_temp_change_lot_error[i] = 0;
    }
}

void nextion_show_button_off(void)
{
    button_status.Set_background_color_bco(63488);
    button_status.setText("OFF");
}

void nextion_show_button_on(void)
{
    button_status.Set_background_color_bco(2032);
    button_status.setText("ON");
}

void nextion_show_ds_device(void)
{
    device.setText("DS18B20");
}

void nextion_show_dht_device(void)
{
    device.setText("DHT21");
}

void nextion_show_si7021_device(void)
{
    device.setText("SI7021");
}

uint32_t find_the_temp_most_element(uint32_t *data, int32_t len)
{
    int32_t position_the_most_ele = 0;
    int32_t num_the_most_ele = 0;
    int32_t num = 1;
    int32_t i=0,j=0;
    uint32_t tmp;

    for(i = 0; i < len - 1; i++)
    {
        for(j = 0; j < len-1; j++)
        {
            tmp = *(data + j) - 1;
            if( ((*(data + i) == *(data + j)) || (*(data + i) == tmp))
         
            && (*(data + i) != 0) )
            {   
               num++;
            }
        }
        if(num > num_the_most_ele)
        {
           num_the_most_ele = num;
           position_the_most_ele = i;
        }
        num = 1;
     }
    return *(data + position_the_most_ele);
}

uint32_t find_the_humi_most_element(uint32_t *data, int32_t len)
{
    int32_t position_the_most_ele = 0;
    int32_t num_the_most_ele = 0;
    int32_t num = 1;
    int32_t i,j;
    
    for(i = 0; i < len - 1; i++)
    {
        for(j = 0; j < len-1; j++)
        {
            if( ((*(data + i) >= (*(data + j)-5)) && (*(data + i) <= (*(data + j)+5)))
         
            && (*(data + i) != 0) )
            {   
               num++;
            }
        }
        if(num > num_the_most_ele)
        {
           num_the_most_ele = num;
           position_the_most_ele = i;
        }
        num = 1;
     }
    return *(data + position_the_most_ele);
}


void check_error_ds_device_data_mode_0(uint32_t len)
{
    uint32_t temp_buf[11] = {0};
    uint32_t temp_buf_len = 0;
    uint32_t return_data,i,j;
    uint32_t error_val_max,error_val_min;

    for(i=0;i<len;i++)
    {
       if((ds_error_status[i]==0) && (ds_temp_change_lot_error[i]==0))
       {
          temp_buf[temp_buf_len] = Ds_temp[i];
          temp_buf_len++;
       }
    }
    
    return_data = find_the_temp_most_element((uint32_t*)temp_buf,temp_buf_len);
   
    error_val_max = return_data + 3;
    error_val_min = return_data - 3;

    for(i=0;i<len;i++)
    {
       if(Ds_temp[i] > error_val_max)
       {
            ds_error_status[i] = 1;
       }
       else if((Ds_temp[i] < error_val_min) && (Ds_temp[i] != 0))
       {
           ds_error_status[i] = 1;
       }
       else 
       {
            ds_error_status[i] = 0;
       }
    }
}


void check_error_ds_device_data_mode_1(uint32_t len)
{
    uint32_t temp_stand = 0;
    uint32_t return_data,i,j;
    uint32_t error_val_max,error_val_min;

    temp_stand = Ds_temp[0];
    error_val_max = temp_stand + 3;
    error_val_min = temp_stand - 3;

    for(i=1;i<len;i++)
    {
       if(Ds_temp[i] > error_val_max)
       {
            ds_error_status[i] = 1;
       }
       else if((Ds_temp[i] < error_val_min) && (Ds_temp[i] != 0))
       {
           ds_error_status[i] = 1;
       }
       else 
       {
            ds_error_status[i] = 0;
       }
    }
}

void check_error_ds_device_data(uint32_t len)
{
    uint32_t temp_stand = 0;
    uint32_t i,j;
    uint32_t error_val_max,error_val_min;
    uint8_t read_buf[2] = {0};

    sensor_temp.getValue(&temp_stand);
  
    EEPROM.write(0x20,temp_stand);

    error_val_max = temp_stand + 3;
    error_val_min = temp_stand - 3;

    for(i=0;i<len;i++)
    {
       if(Ds_temp[i] > error_val_max)
       {
            ds_error_status[i] = 1;
       }
       else if((Ds_temp[i] < error_val_min) && (Ds_temp[i] != 0))
       {
           ds_error_status[i] = 1;
       }
       else 
       {
            ds_error_status[i] = 0;
       }
    }
}

void Fresh_error_ds_to_nextion(uint32_t len)
{
    int i;

    for(i=0;i<len;i++)
    {
       if((ds_error_status[i] == 1) || (ds_temp_change_lot_error[i] == 1))
       {
          temp[i].Set_background_color_bco(63488);
       }
       else
       {
          temp[i].Set_background_color_bco(65535);
       }
    }
}

void check_error_dht_device_data_mode_0(uint32_t len)
{
    uint32_t temp_buf[11] = {0};
    uint32_t humi_buf[11] = {0};
    uint32_t temp_buf_len = 0;
    uint32_t humi_buf_len = 0;
    uint32_t return_temp_data,return_humi_data,i,j;
    uint32_t error_temp_val_max,error_temp_val_min;
    uint32_t error_humi_val_max,error_humi_val_min;

    for(i=0;i<len;i++)
    {
      if(dht_error_temp_status[i]==0 && dht_temp_change_lot_error[i]==0)
      {
          temp_buf[temp_buf_len] = Dht_temp[i];
          temp_buf_len++;
      }
      if(dht_error_humi_status[i]==0 && dht_humi_change_lot_error[i]==0)
      {
          humi_buf[humi_buf_len] = Dht_humi[i];
          humi_buf_len++;
      }
    }
    
    return_temp_data = find_the_temp_most_element((uint32_t*)temp_buf,temp_buf_len);
    return_humi_data = find_the_humi_most_element((uint32_t*)humi_buf,humi_buf_len);
    error_temp_val_max = return_temp_data + 3;
    error_temp_val_min = return_temp_data - 3;

    error_humi_val_max = return_humi_data + 8;
    error_humi_val_min = return_humi_data - 8;

    for(i=0;i<len;i++)
    {
       if(Dht_temp[i] > error_temp_val_max)
       {
            dht_error_temp_status[i] = 1;
       }
       else if((Dht_temp[i] < error_temp_val_min) && (Dht_temp[i] != 0))
       {
           dht_error_temp_status[i] = 1;
       }
       else 
       {
            dht_error_temp_status[i] = 0;
       }
//  
       if(Dht_humi[i] > error_humi_val_max)
       {
            dht_error_humi_status[i] = 1;
          
       }
       else if((Dht_humi[i] < error_humi_val_min) && (Dht_humi[i] != 0))
       {
            dht_error_humi_status[i] = 1;
            
       }
       else 
       {
            dht_error_humi_status[i] = 0;
            
       }
    }
}


void check_error_dht_device_data_mode_1(uint32_t len)
{
    uint32_t temp_stand = 0;
    uint32_t humi_stand = 0;
    uint32_t i=0,j=0;
    uint32_t error_temp_val_max,error_temp_val_min;
    uint32_t error_humi_val_max,error_humi_val_min;

    temp_stand = Dht_temp[0];
    humi_stand = Dht_humi[0];
    error_temp_val_max = temp_stand + 3;
    error_temp_val_min = temp_stand - 3;
    error_humi_val_max = humi_stand + 10;
    error_humi_val_min = humi_stand - 10;

    for(i=1;i<len;i++)
    {
      if(Dht_temp[i] > error_temp_val_max)
       {
            dht_error_temp_status[i] = 1;
       }
       else if((Dht_temp[i] < error_temp_val_min) && (Dht_temp[i] != 0))
       {
           dht_error_temp_status[i] = 1;
       }
       else 
       {
            dht_error_temp_status[i] = 0;
       }

       if(Dht_humi[i] > error_humi_val_max)
       {
            dht_error_humi_status[i] = 1;
          
       }
       else if((Dht_humi[i] < error_humi_val_min) && (Dht_humi[i] != 0))
       {
            dht_error_humi_status[i] = 1;
       }
       else 
       {
            dht_error_humi_status[i] = 0;            
       }
    }
}


void check_error_dht_device_data(uint32_t len)
{
    uint32_t temp_stand = 0;
    uint32_t humi_stand = 0;
    uint32_t i,j;
    uint32_t error_temp_val_max,error_temp_val_min;
    uint32_t error_humi_val_max,error_humi_val_min;

    sensor_temp.getValue(&temp_stand);
    sensor_humi.getValue(&humi_stand);

    EEPROM.write(0x00,temp_stand);
    EEPROM.write(0x10,humi_stand);

    error_temp_val_max = temp_stand + 3;
    error_temp_val_min = temp_stand - 3;

    error_humi_val_max = humi_stand + 15;
    error_humi_val_min = humi_stand - 15;

    for(i=0;i<len;i++)
    {
      if(Dht_temp[i] > error_temp_val_max)
       {
            dht_error_temp_status[i] = 1;
       }
       else if((Dht_temp[i] < error_temp_val_min) && (Dht_temp[i] != 0))
       {
           dht_error_temp_status[i] = 1;
       }
       else 
       {
            dht_error_temp_status[i] = 0;
       }

       if(Dht_humi[i] > error_humi_val_max)
       {
            dht_error_humi_status[i] = 1;
          
       }
       else if((Dht_humi[i] < error_humi_val_min) && (Dht_humi[i] != 0))
       {
            dht_error_humi_status[i] = 1;
       }
       else 
       {
            dht_error_humi_status[i] = 0;            
       }
    }
}

void Fresh_error_dht_to_nextion(uint32_t len)
{
    int i;

    for(i=0;i<len;i++)
    {
       if((dht_error_temp_status[i] == 1) || (dht_temp_change_lot_error[i] == 1))
       {
          temp[i].Set_background_color_bco(63488);
       }
       else
       {
          temp[i].Set_background_color_bco(65535);
       }

       if((dht_error_humi_status[i] == 1) || (dht_humi_change_lot_error[i] == 1))
       {
          humi[i].Set_background_color_bco(63488);
       }
       else
       {
          humi[i].Set_background_color_bco(65535);
       }
    }
}

void check_error_si7021_device_data(uint32_t len)
{
    uint32_t temp_stand = 0;
    uint32_t humi_stand = 0;
    uint32_t i,j;
    uint32_t error_temp_val_max,error_temp_val_min;
    uint32_t error_humi_val_max,error_humi_val_min;

    sensor_temp.getValue(&temp_stand);
    sensor_humi.getValue(&humi_stand);

    EEPROM.write(0x00,temp_stand);
    EEPROM.write(0x10,humi_stand);

    error_temp_val_max = temp_stand + 3;
    error_temp_val_min = temp_stand - 3;

    Serial.print(humi_stand);
    error_humi_val_max = humi_stand + 15;
    error_humi_val_min = humi_stand - 15;

    for(i=0;i<len;i++)
    {
      if(Si7021_temp[i] > error_temp_val_max)
       {
            si7021_error_temp_status[i] = 1;
       }
       else if((Si7021_temp[i] < error_temp_val_min) && (Si7021_temp[i] != 0))
       {
           si7021_error_temp_status[i] = 1;
       }
       else 
       {
            si7021_error_temp_status[i] = 0;
       }
///////////////////////
        Serial.print(Si7021_humi[i]);
       if(Si7021_humi[i] > error_humi_val_max)
       {
            si7021_error_humi_status[i] = 1;
       }
       else if((Si7021_humi[i] < error_humi_val_min) && (Si7021_humi[i] != 0))
       {
            si7021_error_humi_status[i] = 1;
       }
       else 
       {
            si7021_error_humi_status[i] = 0;            
       }
    }
}

void Fresh_error_si7021_to_nextion(uint32_t len)
{
    int i;
  
    for(i=0;i<len;i++)
    {
       if((si7021_error_temp_status[i] == 1) || (si7021_temp_change_lot_error[i] == 1))
       {
          temp[i].Set_background_color_bco(63488);
       }
       else
       {
          temp[i].Set_background_color_bco(65535);
       }

       if((si7021_error_humi_status[i] == 1) || (si7021_humi_change_lot_error[i] == 1))
       {
          humi[i].Set_background_color_bco(63488);
       }
       else
       {
          humi[i].Set_background_color_bco(65535);
       }
    }
}

void setup() 
{
    uint32_t temp_stand = 0;
    uint32_t humi_stand = 0;
  
    nexSerial.begin(9600);
    Serial.begin(9600);
    pinMode(3,INPUT);

    nextion_clear_data();
    temp_stand = EEPROM.read(0x00);
    humi_stand = EEPROM.read(0x10);
    if((temp_stand>100) && (humi_stand>100))
    {
          temp_stand = 23;
          humi_stand = 50;
    }           
    sensor_temp.setValue(temp_stand);
    sensor_humi.setValue(humi_stand);
}

void loop() 
{
    bool DS_flag=0;
    bool Dht_flag=0;
    uint32_t show_cnt=0;
    uint32_t ds_device_count=0;
    uint32_t dht_device_count=0;
    uint32_t si7021_device_count=0;
    uint8_t read_mode[6] = {0};
    uint32_t temp_stand = 0;
    uint32_t humi_stand = 0;
    
    while(1)
    {
        sensor_temp.getValue(&temp_stand);
        sensor_humi.getValue(&humi_stand);
        if((temp_stand!=0) && (humi_stand!=0))
        {
            EEPROM.write(0x00,temp_stand);
            EEPROM.write(0x10,humi_stand);
        }
      
        if(Check_button_status())
        {
            show_cnt = 0;
            dht_device_count = 0;
            ds_device_count = 0;
            si7021_device_count=0;
            nextion_show_button_on();

            for(int i=0;i<11;i++)
            {  
                if(!Check_button_status())
                {
                    break;
                }
                #if 1
                if(Is_dht_device(i+1))
                {
                   dht_device_count++;
                   nextion_show_dht_device();
                   Get_dht_device_temp(i+1);
                   Get_dht_device_humi(i+1);
                  
                  Fresh_one_device_humi_to_nextion(i+1,(char)Dht_humi[i]);
                  Fresh_one_device_temp_to_nextion(i+1,(char)Dht_temp[i]);
                  continue;
                }     
                #endif    

                 #if 1
                if(Is_DS18B20_device(i+1))
                {
                   ds_device_count++;
                   nextion_show_ds_device();
                   Setup_ds_device(i+1);
                   Get_ds_device_temp(i+1);
                  
                  Fresh_one_device_temp_to_nextion(i+1,(char)Ds_temp[i]);
                  continue;
                }
                #endif
                        
                #if 1
                if(Is_si7021_device(i+1))
                {
                    si7021_device_count++;
                    nextion_show_si7021_device();
                    Get_si7021_device_temp(i+1);
                    Get_si7021_device_humi(i+1);

                    Fresh_one_device_humi_to_nextion(i+1,(char)Si7021_humi[i]);
                    Fresh_one_device_temp_to_nextion(i+1,(char)Si7021_temp[i]);
                    continue;
                }
                #endif
            }
            if(dht_device_count > 0)
            {
              #if 0
                mode.getText(read_mode,6);
                if(strstr(read_mode,"mode 0"))
                {
                  check_error_dht_device_data_mode_0(11);
                }
                else
                {
                  check_error_dht_device_data_mode_1(11);
                }  
                #endif         
                check_error_dht_device_data(11);   
                Fresh_error_dht_to_nextion(11);
            }
            if(ds_device_count > 0)
            {
              #if 0
                mode.getText(read_mode,6);
                if(strstr(read_mode,"mode 0"))
                {
                  check_error_ds_device_data_mode_0(11);
                }
                else
                {
                  check_error_ds_device_data_mode_1(11);
                }
                #endif
                check_error_ds_device_data(11);
                Fresh_error_ds_to_nextion(11);
            }
            if(si7021_device_count > 0)
            {
                check_error_si7021_device_data(11);
                Fresh_error_si7021_to_nextion(11);
            }
         }
         else
         {
                show_cnt++;
                if(show_cnt == 1)
                {
                    //temp = EEPROM.read(0x00);
                    //humi = EEPROM.read(0x10);
                    //sensor_temp.setValue(temp);
                    //sensor_humi.setValue(humi);
                    nextion_show_button_off();
                    nextion_clear_data();

                }
                else
                {
                    show_cnt = 2;
                }
         }
    }
}



