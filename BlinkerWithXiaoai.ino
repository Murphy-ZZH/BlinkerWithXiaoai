#define BLINKER_WIFI
#define BLINKER_MIOT_OUTLET  // 支持小爱同学
#define BLINKER_MIOT_SENSOR
#include <Servo.h>  // ESP8266使用普通Servo库
#include <Blinker.h>

// 对应客户端的秘钥
char auth[] = "********";
char ssid[] = "********";
char pswd[] = "********";

Servo myservo;  // 初始化舵机对象
BlinkerButton Button1("btn-max");  // 最大值按钮
BlinkerButton Button2("btn-min");  // 最小值按钮
BlinkerButton Button3("btn-set");  // 复位-90°
int servo_max = 180, servo_min = 0, servo_set = 90, time_num = 2000;
bool wsState;

// 按钮控制回调函数
void button1_callback(const String& state) {  // 最大值按钮功能
  BLINKER_LOG("get button state: ", state);
  myservo.write(servo_max);
  Blinker.vibrate();
  Blinker.delay(200);
  myservo.write(servo_set);
}

void button2_callback(const String& state) {  // 最小值按钮功能
  BLINKER_LOG("get button state: ", state);
  myservo.write(servo_min);
  Blinker.vibrate();
  Blinker.delay(200);
  myservo.write(servo_set);
}

void button3_callback(const String& state) {  // 重置按钮功能
  BLINKER_LOG("get button state: ", state);
  myservo.write(servo_set);
  Blinker.vibrate();
}

// 小爱电源类操作的回调函数
void miotPowerState(const String& state) {
  BLINKER_LOG("need set power state: ", state);
  if (state == BLINKER_CMD_ON) {
    wsState = true;
    myservo.write(servo_max);  // 启动舵机，达到最大值
    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();
    Blinker.delay(400);
    myservo.write(servo_set);
    // Blinker.delay(200);
    // myservo.write(75);
    // Blinker.delay(200);
    // myservo.write(servo_set);
  } else if (state == BLINKER_CMD_OFF) {
    wsState = false;
    myservo.write(servo_min);  // 将舵机恢复到最小值
    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();
    Blinker.delay(400);
    myservo.write(servo_set);
  }
}

// 小爱同学的回调查询函数
void miotQuery(int32_t queryCode) {
  BLINKER_LOG("MIOT Query codes: ", queryCode);
  switch (queryCode) {
    case BLINKER_CMD_QUERY_ALL_NUMBER:
      BLINKER_LOG("MIOT Query All");
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER:
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.print();
      break;
    default:
      BlinkerMIOT.powerState(wsState ? "on" : "off");
      BlinkerMIOT.print();
      break;
  }
}

void setup() {
  // 初始化串口，并开启调试信息
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);

  // 初始化舵机
  myservo.attach(D1);        // 将舵机控制引脚设置为 GPIO5（D1）
  myservo.write(servo_set);  // 默认设置为 90°（中间位置）

  // 初始化 Blinker，启用 SSL 加密通信
  Blinker.begin(auth, ssid, pswd);

  // 设置按钮的回调函数
  Button1.attach(button1_callback);
  Button2.attach(button2_callback);
  Button3.attach(button3_callback);

  // 注册小爱同学的回调函数
  BlinkerMIOT.attachPowerState(miotPowerState);
  BlinkerMIOT.attachQuery(miotQuery);
}

void loop() {
  Blinker.run();
  Blinker.delay(2000);  // 延迟 2 秒
}
