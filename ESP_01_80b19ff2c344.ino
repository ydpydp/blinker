#define BLINKER_WIFI
//#define BLINKER_ALIGENIE_LIGHT //天猫精灵
#include <Blinker.h>

char auth[] = "80b19ff2c344";
char ssid[] = "Routera11";
char pswd[] = "hta11888999";
int pinRelay =0; 
int ESP01S_LED = LED_BUILTIN;//模块内置led
int relay_state=0;


//**软重启定义
uint32_t con_time = 0;    //断网记时
int con_flag = 0;    //断网标记，1为断网
void(*resetFunc) (void) = 0;//定义一个reset函数（用于重启）

 
// 新建组件对象
BlinkerButton Button1("btn-01");
BlinkerNumber Number1("num-01"); 
// 按下按键即会执行该函数
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    if (state=="on") {
        // 反馈开关状态
        Button1.print("on");
        digitalWrite(ESP01S_LED, HIGH);
        digitalWrite(pinRelay,LOW);
        relay_state=1;
    } else if(state=="off"){
        digitalWrite(pinRelay,HIGH);
        digitalWrite(ESP01S_LED, LOW);
        relay_state=0;

    }
    
  Serial.println(state);

}
 void heartbeat()  //心跳包
{

      if (digitalRead(pinRelay) == LOW) {
        Button1.icon("fas fa-toggle-on");//开关式样，在这里https://fontawesome.com/v5.15/icons/toggle-on?style=light
        Button1.color("#fddb00");//开关背景色
        Button1.text("on");//开关标签显示的文字为on，可以改成自己想要的
        Button1.print("on");//这句告诉app，现在是on，下次按，state就应该是off
    } else if(digitalRead(pinRelay) == HIGH){
        Button1.icon("fas fa-toggle-off");//开关式样，在这里https://fontawesome.com/v5.15/icons/toggle-on?style=light
        Button1.color("#eeeeee");//开关背景色
        Button1.text("off");//开关标签显示的文字为off，可以改成自己想要的
        Button1.print("off");//这句告诉app，现在是off，下次按，state就应该是off，app端会用“get state”索取硬件现时状态

    }
  Number1.print(WiFi.RSSI());  //信号强度
  Serial.println(WiFi.RSSI());

}
void setup() {
    // 初始化串口
    Serial.begin(115200);
    // 初始化有LED的IO
    pinMode(ESP01S_LED, OUTPUT);
    pinMode(pinRelay, OUTPUT);
    digitalWrite(ESP01S_LED, LOW);
     digitalWrite(pinRelay, LOW);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    delay(10000);
    
    Button1.attach(button1_callback);
    //BlinkerAliGenie.attachPowerState(aligeniePowerState);
    Blinker.attachHeartbeat(heartbeat);
}
 
void loop() {
    Blinker.run();//Blinker.run()语句负责处理Blinker收到的数据，每次运行都会将设备收到的数据进行一次解析。//在使用WiFi接入时，该语句也负责保持网络连接
    if(relay_state==1){
        delay(2000);
        relay_state=3;
        Button1.icon("fas fa-toggle-on");//开关式样，在这里https://fontawesome.com/v5.15/icons/toggle-on?style=light
        Button1.color("#fddb00");//开关背景色
        Button1.text("on");//开关标签显示的文字为on，可以改成自己想要的
        Button1.print("on");//这句告诉app，现在是on，下次按，state就应该是off
      }
    else if(relay_state==0){
        delay(2000);
        relay_state=3;
      
        Button1.icon("fas fa-toggle-off");//开关式样，在这里https://fontawesome.com/v5.15/icons/toggle-on?style=light
        Button1.color("#eeeeee");//开关背景色
        Button1.text("off");//开关标签显示的文字为off，可以改成自己想要的
        Button1.print("off");//这句告诉app，现在是off，下次按，state就应该是off，app端会用“get state”索取硬件现时状态
        }
    
    //**软重启
    if (Blinker.connected())//Blinker.connected()函数反馈网络连接状态
    {
      con_flag = 0;
    } else
    {
      if (con_flag == 0)
      {
        con_time = millis();    //给断网时间赋初始值
        con_flag = 1;
      } else
      {
        if ((millis() - con_time) >= 90000)    //判断断网时间超90秒后执行重启，这个时间可根据实际需要调整
        {
          resetFunc();//启用这个函数就软重启设备
        }
      }
    }
}