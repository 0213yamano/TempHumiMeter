/***********************************************************************/
/*****2024/2/2　作成
/*****更新
/*****車載ソフトウェアチーム　温湿度計設計プロジェクト
/*****詳細設計用プロトタイプコード
/*****山野陽平
/***********************************************************************/
/***********************************************************************
Library
***********************************************************************/
//#include <Wire.h>           //I2C用のライブラリ LCDのライブラリに含まれるためコメントアウト
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

/***********************************************************************
config
***********************************************************************/
#define DHT_PIN 2        // DHT11のDATAピンをデジタルピン2に定義
#define DHT_MODEL DHT11  // 接続するセンサの型番を定義する(DHT11やDHT22など)

DHT dht(DHT_PIN, DHT_MODEL);         // センサーの初期化
LiquidCrystal_I2C lcd(0x27, 20, 4);  //LCDのI2Cアドレスと表示領域を指定　I2CアドレスはI2Cモジュールにて決まっているためそれに従う（抵抗パターンで決めている。フルオープンの場合は0x27になる（初期状態）） 


/*グローバル変数　*/ // !!!!!アクセスの制限や手順が無く、意図しない動作を起こしやすい。ポインタで実装できるようにしたい
float Humidity              = 99.99;            //動作確認のため極端な数値
float Temperature           = 99.99;            //動作確認のため極端な数値

unsigned long SetUpTime     = 0;
unsigned long RequestTiming = 0;
unsigned long now           = 0;
unsigned long ReqTimeCalc   = 0;

/***********************************************************************
/*************************セットアップフェーズ***************************/
/***********************************************************************
デバイスの初期化　initDevice関数  
LCD初期化→LCD初期表示→DHT11の初期化→シリアルモニタ初期化
***********************************************************************/
void initDevice(){
  /*LCD初期化*/
  lcd.init();       //LCD初期化
  lcd.backlight();  //LCDバックライト点灯
  //lcd.home();       //カーソルを左上左端　表示開始場所を毎回指定するときは不要
  //lcd.noBlink();    //カーソル点灯をOFF

  /*LCD初期出力*/
  lcd.setCursor (0, 0);             //次のlcd.printで書き始める場所の指定
  lcd.print     ("Temp & Humi Meter");  //記載内容
  lcd.setCursor (0, 2);  //次のlcd.printで書き始める場所の指定
  lcd.print     ("Temp:");   //記載内容
  lcd.setCursor (8, 2);
  lcd.print     ("\xDF"); 
  lcd.setCursor (9, 2);
  lcd.print     ("C");
  lcd.setCursor (0, 3);     //次のlcd.printで書き始める場所の指定
  lcd.print     ("Humi:   %");  //記載内容

  /*DHT11初期化*/
  dht.begin();           // センサーの動作開始

  /*シリアルモニタ初期化*/
  Serial.begin(115200);  // シリアル通信の準備をする

}

/***********************************************************************
セットアップ　setup関数　
デバイスの初期化
セットアップ時間の取得
***********************************************************************/
void setup() {
  
  initDevice();
  SetUpTime = millis();

}

/***********************************************************************
/****************************ループフェーズ*****************************/
/***********************************************************************
温度測定　ReadTempHumi関数
  温度取得
  湿度取得
***********************************************************************/
void ReadTempHumi(){

  Temperature = dht.readTemperature();  // 温度の読み取り(摂氏)
  Humidity    = dht.readHumidity();        // 湿度の読み取り 

}

/***********************************************************************
シリアルモニタ出力　DebugMoni関数

***********************************************************************
void DebugMoni(){

  Serial.println  ("-------------------------------");
  Serial.print    ("温度      : ");
  Serial.println  (Temp);
  Serial.print    ("湿度      : ");
  Serial.println  (Humi);
  Serial.print    ("セットアップ時間      : ");
  Serial.println  (SetUpTime);
  Serial.print    ("リクエスト時間        : ");
  Serial.println  (RequestTiming);
  Serial.print    ("実行周期              : ");
  Serial.println  (RequestTiming - ReqTimeCalc);

}


/***********************************************************************
温湿度計プログラム
  温度測定
  温度表示
  時間測定
  シリアルモニタ出力
***********************************************************************/
void loop() {

 // delay(2000);  // センサーの読み取りを2秒間隔
  now = millis();
  unsigned long T = RequestTiming;

  if(now - T >= 500){

    ReqTimeCalc = T;
    RequestTiming = now;
    
    ReadTempHumi();

    int Temp = Temperature;   //表示用に整数化　小数点以下切り捨て
    int Humi = Humidity;

     if (isnan(Humidity) || isnan(Temperature)) {  // 読み取りのチェック エラー処理

    lcd.setCursor(0, 2);
    lcd.print("Temp: ERR     ");
    lcd.setCursor(0, 3);
    lcd.print("Humi: ERR     ");

    return;
  }
  if (Temperature > 50) {
    Temp = 50;
  }
  if (Temperature < 0) {
    Temp = 0;
  }
  lcd.setCursor(6, 2);
  lcd.print(Temp);
  lcd.setCursor(8, 2);
 // lcd.write    (0xDF);   //「°」の表示指示
  lcd.print     ("\xDF"); 
  lcd.setCursor(9, 2);
  lcd.print("C");

  if (Humidity > 80) {
    Humi = 80;
  }
  if (Humidity < 20) {
    Humi = 20;
  }
  lcd.setCursor(6, 3);
  lcd.print(Humi);
  lcd.setCursor(8, 3);
  lcd.print("%");

  lcd.setCursor(1, 1);
  lcd.print(RequestTiming);

  Serial.println  ("-------------------------------");
  Serial.print    ("温度      : ");
  Serial.println  (Temp);
  Serial.print    ("湿度      : ");
  Serial.println  (Humi);
  Serial.print    ("セットアップ時間      : ");
  Serial.println  (SetUpTime);
  Serial.print    ("リクエスト時間        : ");
  Serial.println  (RequestTiming);
  Serial.print    ("実行周期              : ");
  Serial.println  (RequestTiming - ReqTimeCalc);

  }else{
    return;

  }

  
}