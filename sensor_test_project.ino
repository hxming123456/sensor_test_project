#include <DallasTemperature.h>
#include <OneWire.h>
#include <dht.h>
#include "Nextion.h"

NexText temp[11] = {NexText(0,2,"t1"),NexText(0,2,"t4"),NexText(0,2,"t7"),NexText(0,2,"t10"),NexText(0,2,"t13"),NexText(0,2,"t16"),
                    NexText(0,2,"t19"),NexText(0,2,"t22"),NexText(0,2,"t25"),NexText(0,2,"t28"),NexText(0,2,"t31")};

NexText humi[11] = {NexText(0,2,"t2"),NexText(0,2,"t5"),NexText(0,2,"t8"),NexText(0,2,"t11"),NexText(0,2,"t14"),NexText(0,2,"t17"),
                    NexText(0,2,"t20"),NexText(0,2,"t23"),NexText(0,2,"t26"),NexText(0,2,"t29"),NexText(0,2,"t32")};

NexText button_status = NexText(0,2,"t34");          
NexText device = NexText(0,2,"t0");
                
uint32_t Ds_temp[11] = {0};                    
uint32_t Dht_temp[11] = {0};
uint32_t Dht_humi[11] = {0};

bool ds_error_status[11] = {0};
bool dht_error_temp_status[11] = {0};
bool dht_error_humi_status[11] = {0};
dht DHT;

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
  
    sensor[port-1].requestTemperatures();
    temp=sensor[port-1].getTempC(tempDeviceAddress);

    Ds_temp[port-1] = (uint32_t)temp;
    return temp;
}

void Fresh_one_device_temp_to_nextion(uint32_t num,char Temp)
{
     char Temp_buf[5];

     Temp_buf[0] = 'T';
     Temp_buf[1] = ':';
     Temp_buf[2] = (Temp / 10)+'0';
     Temp_buf[3] = (Temp % 10)+'0';
     Temp_buf[4] = '\0';
     temp[num-1].setText(Temp_buf);
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
    int pin_num;
   
    pin_num = 33 - port;
    
    temp = DHT.temperature;
    Dht_temp[port-1] = temp;
    return temp;
}

float Get_dht_device_humi(uint32_t port)
{
    float humi;
    int pin_num;
   
    pin_num = 33 - port;
   
    humi = DHT.humidity;
    Dht_humi[port-1] = humi;
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

void setup() 
{
    nexSerial.begin(9600);
    Serial.begin(9600);
    pinMode(3,INPUT);

    nextion_clear_data();
}

uint32_t find_the_most_element(uint32_t *data, int32_t len)
{
    int32_t position_the_most_ele = 0;
    int32_t num_the_most_ele = 0;
    int32_t num = 1;
    int32_t i,j;
    
    for(i = 0; i < len - 1; i++)
    {
        for(j = i + 1; j < len; j++)
        {
            if( *(data + i) == *(data + j) && (*(data + i) != 0))
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

void check_error_ds_device_data(uint32_t len)
{
    uint32_t return_data,i,j;
    uint32_t error_val_max,error_val_min;
    
    return_data = find_the_most_element((uint32_t*)Ds_temp,len);
   
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

void Fresh_error_ds_to_nextion(uint32_t len)
{
    int i;

    for(i=0;i<len;i++)
    {
       if(ds_error_status[i] == 1)
       {
          temp[i].Set_background_color_bco(63488);
       }
       else
       {
          temp[i].Set_background_color_bco(65535);
       }
    }
}

void check_error_dht_device_data(uint32_t len)
{
    uint32_t buf[11];
    uint32_t return_temp_data,return_humi_data,i,j;
    uint32_t error_temp_val_max,error_temp_val_min;
    uint32_t error_humi_val_max,error_humi_val_min;
    
    return_temp_data = find_the_most_element((uint32_t*)Dht_temp,len);
    return_humi_data = find_the_most_element((uint32_t*)Dht_humi,len);
    Serial.println(return_temp_data);
    Serial.println(return_humi_data);
    error_temp_val_max = return_temp_data + 3;
    error_temp_val_min = return_temp_data - 3;

    error_humi_val_max = return_humi_data + 10;
    error_humi_val_min = return_humi_data - 10;

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

void Fresh_error_dht_to_nextion(uint32_t len)
{
    int i;

    for(i=0;i<len;i++)
    {
       if(dht_error_temp_status[i] == 1)
       {
          temp[i].Set_background_color_bco(63488);
       }
       else
       {
          temp[i].Set_background_color_bco(65535);
       }

       if(dht_error_humi_status[i] == 1)
       {
          humi[i].Set_background_color_bco(63488);
       }
       else
       {
          humi[i].Set_background_color_bco(65535);
       }
    }
}

void loop() 
{
    bool DS_flag=0;
    bool Dht_flag=0;
    uint32_t show_cnt=0;
    
    while(1)
    {
        if(Check_button_status())
        {
            show_cnt = 0;
            nextion_show_button_on();
            for(int i=0;i<11;i++)
            {  
                if(Is_dht_device(i+1))
                {
                  Dht_flag = 1;
                  nextion_show_dht_device();
                  Get_dht_device_temp(i+1);
                  Get_dht_device_humi(i+1);
                  
                  Fresh_one_device_humi_to_nextion(i+1,(char)Dht_humi[i]);
                  Fresh_one_device_temp_to_nextion(i+1,(char)Dht_temp[i]);   

                  if(Dht_flag)
                  {
                      Dht_flag = 0;
                      check_error_dht_device_data(11);
                      Fresh_error_dht_to_nextion(11);
                   }
                }

                if(Is_DS18B20_device(i+1))
                {
                  DS_flag = 1;
                  nextion_show_ds_device();
                  Setup_ds_device(i+1);
                  Get_ds_device_temp(i+1);
                  
                  Fresh_one_device_temp_to_nextion(i+1,(char)Ds_temp[i]);

                  if(DS_flag)
                  {
                      DS_flag = 0;
                      check_error_ds_device_data(11);
                      Fresh_error_ds_to_nextion(11);
                  }
                }
            }
         }
         else
         {
                show_cnt++;
                if(show_cnt == 1)
                {
                    nextion_show_button_off();
                    nextion_clear_data();
                }
                else
                {
                    show_cnt = 2;
                }
         }

          /*if(Dht_flag)
          {
                Dht_flag = 0;
                check_error_dht_device_data(11);
                Fresh_error_dht_to_nextion(11);
           }
         if(DS_flag)
         {
                DS_flag = 0;
                check_error_ds_device_data(11);
                Fresh_error_ds_to_nextion(11);
          }*/
    }
}


