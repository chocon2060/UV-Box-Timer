#include <Wire.h> 
#include <LiquidCrystal.h>
#define menu_button 3
#define up_button 4
#define down_button 5
#define buzzer 2
#define UV_led 6
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
unsigned int set_time;
unsigned long future_time;
unsigned long micro_time_now;
unsigned long micro_time_last = 0;
unsigned long change_time_last = 0;
uint8_t t_hour = 0;
uint8_t t_minute = 0;
uint8_t t_seconds = 0;
uint8_t percent;
bool status_start_button = false;
bool status_button;
uint8_t arrow[] = {0x04,0x0E,0x15,0x04,0x04,0x04,0x04,0x04};
uint8_t onecow[] = {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10};
uint8_t twocow[] = {0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18};
uint8_t threecow[] = {0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C,0x1C};
uint8_t fourow[] = {0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E,0x1E};
uint8_t fivecow[] = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};
void setup() {
  for(int i = 3; i<=5;i++){
    pinMode(i,INPUT);
  }
  pinMode(buzzer,OUTPUT);
  pinMode(UV_led,OUTPUT);
  lcd.begin(16,2);
  lcd.createChar(0,arrow);
  lcd.createChar(1,onecow);
  lcd.createChar(2,twocow);
  lcd.createChar(3,threecow);
  lcd.createChar(4,fourow);
  lcd.createChar(5,fivecow);
  lcd.home();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome to UVBox");
  lcd.setCursor(0,1);
  lcd.print("Prs Menu to next");
  while(!digitalRead(menu_button));
  while(digitalRead(menu_button));
  delay(500);
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("Menu Start");
  delay(1000);
}
void loop() {
  detachInterrupt(digitalPinToInterrupt(menu_button));
  menu_loop();
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("Timer  Start");
  while(digitalRead(menu_button));
  delay(500);
  set_time = ((t_hour*60 +t_minute)*60)+ t_seconds;
  future_time = set_time + (millis()/1000);
  analogWrite(UV_led,150);
  attachInterrupt(digitalPinToInterrupt(menu_button),INT_Stop,HIGH);
  count_loop();
}
void menu_loop(){
  uint8_t menu_mode = 0;
  status_button = true;
  micro_time_last = 0;
  t_hour = 0;
  t_minute = 0;
  t_seconds = 0;
  bool status_ok_button = false;
  do{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Time: ");
    lcd_print_number(t_hour);
    lcd.print(":");
    lcd_print_number(t_minute);
    lcd.print(":");
    lcd_print_number(t_seconds);
    while(status_button){
      if(!status_ok_button){
        lcd.setCursor(7+menu_mode,1);
        lcd.write(byte(0));

        if(digitalRead(up_button)){
          while(digitalRead(up_button));
            menu_mode += 3;
            status_button = false;

        }else if(digitalRead(down_button)){
          while(digitalRead(down_button));
            menu_mode -= 3;
            status_button = false;

        }else if(digitalRead(menu_button)){
           change_time_last =millis();
           while(digitalRead(menu_button)){
             if(millis()-change_time_last >2000){
              status_start_button = true;
              break;
             }
           }
           status_ok_button = true;
           status_button = false;
        }

        if(menu_mode>=6){
          menu_mode = 6;
        }else if(menu_mode <=0){
          menu_mode =0;
        }

      }else{
          if(digitalRead(up_button)){
            if(menu_mode ==0 )
            t_hour++;
            else if(menu_mode ==3)
            t_minute++;
            else
            t_seconds++;

            status_button = false;
            delay(200);

          }else if(digitalRead(down_button)){
            if(menu_mode ==0 )
            t_hour--;
            else if(menu_mode ==3)
            t_minute--;
            else
            t_seconds--;

            status_button = false;
            delay(200);
          }
        cheack_t_time();
        if(digitalRead(menu_button)){
          change_time_last =millis();
           while(digitalRead(menu_button)){
             if(millis()-change_time_last >2000){
              status_start_button = true;
              break;
             }
           }
           status_ok_button = false;
           status_button = false;
        }
        micro_time_now = millis();
        if((micro_time_now - micro_time_last) >= 500){
          lcd.setCursor(7+menu_mode,1);
          lcd.write(byte(0));
        if(micro_time_now - micro_time_last >= 1000){
          micro_time_last = micro_time_now;
          status_button = false;
          }
        }
      }
    }
    status_button = true;
  } 
  while (!status_start_button);
}
void count_loop(){
    micro_time_last = 0;
    unsigned long time_range,last_time_range;

  do
  {
    time_range = future_time - (millis()/1000);
    t_hour = time_range/3600;
    t_minute = (time_range%3600)/60; 
    t_seconds = (time_range%3600)%60;
    if(time_range != last_time_range){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Time: ");
      lcd_print_number(t_hour);
      lcd.print(":");
      lcd_print_number(t_minute);
      lcd.print(":");
      lcd_print_number(t_seconds);

      percent = (time_range*80)/set_time;
      for(int i = 0;i <= (uint8_t(percent/5));i++){
        lcd.setCursor(i,1);
        if(i >= uint8_t(percent/5)){
          if(percent%5 > 0){
          lcd.write(uint8_t(percent%5));
          }
        }
        else{
          lcd.write(5);
        }
      }
      last_time_range = time_range;
    }
    if(time_range == 0){
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("Timer Finish");
      lcd.setCursor(3,1);
      lcd.print("Press Menu");
      analogWrite(UV_led,0);
      micro_time_last = millis();
      while(!digitalRead(menu_button)){
        micro_time_now = millis();
        if(micro_time_now - micro_time_last >500){
          digitalWrite(buzzer,HIGH);
        }
        if(micro_time_now - micro_time_last >1000){
          digitalWrite(buzzer,LOW);
          micro_time_last = micro_time_now;
        }
      }
      while(digitalRead(menu_button));
      status_start_button = false;
    }
    
  }
  while (status_start_button); 
}
void cheack_t_time(){
  if(t_hour >24 && t_hour < 250)
    t_hour = 0;
  else if(t_hour > 250)
    t_hour = 24;
  
  if(t_minute >60 && t_minute < 250 )
    t_minute = 0;
  else if(t_minute > 250)
    t_minute = 59;

  if(t_seconds >60 && t_seconds <250 )
    t_seconds = 0;
  else if(t_seconds >250)
    t_seconds = 59;
}
void lcd_print_number(uint8_t number){
  if (number < 10)
  {
    lcd.print("0");
    lcd.print(number);
  }
  else{
    lcd.print(number);
  }
  
} 
void INT_Stop(){
  if(digitalRead(up_button) == 1 && digitalRead(down_button) == 1){
    status_start_button = false;
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("! Emergency !");
    lcd.setCursor(3,1);
    lcd.print("Timer Stop");
    while(digitalRead(menu_button));
    delay(1000);
  }
}
