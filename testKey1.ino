//使用するキーが1つの場合のキー処理
//※無保証※本ソースコートを使用したことによりいかなる事態も作成者はその責を負いません

//PORT
#define KEY1PORT 2            //キーを繋いでいるポート　この例ではDigital2ピン

//KEY
#define KEY_SHORTTIMEFROM1 30 //短押し判定時間1[ms]　この時間より短い時間はチャタリングとして捨てられる
#define KEY_SHORTTIMETO1 300  //短押し判定時間2[ms]　この時間を超えて押すとキャンセルされる
                    //この例の場合、押している時間が30[ms]より長く、300[ms]より短い時短押しと判定される
#define KEY_LONGTIME1 1000    //長押し判定時間[ms]　この時間以上押していると長押しと判定される

bool longPushFlg;             //長押し中Trueになる

void setup() {
  pinMode(KEY1PORT, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);

  longPushFlg = false;        //長押しフラグの初期化
}

void loop() {
  while (true) {
    keyCheck();               //メインループの中でキーチェック関数を走らせる
    if (longPushFlg) {        //長押しの処理をここに記載
      digitalWrite(LED_BUILTIN, HIGH);
    }else{
      digitalWrite(LED_BUILTIN, LOW);
    }

    //その他の処理を実装する
    
  }
}

/*
   Key Check Routine
*/
void key_Short(void) {
  Serial.println("KEY1 SHORT");       //キー短押し時に実行する内容
}

void key_Long(void) {
  Serial.println("KEY1 LONG");        //キー長押し検出時に実行する内容
}

void keyCheck(void) {
  keyCheck_(KEY1PORT, KEY_SHORTTIMEFROM1, KEY_SHORTTIMETO1, KEY_LONGTIME1);
}

void keyCheck_(byte port_, unsigned int shortTimeFrom_, unsigned int shortTimeTo_, unsigned int longTime_) {
  unsigned long nowTime;              //現在の時間を保存しておく変数
  unsigned int port;                  //現在のキーポート状況を保持する変数
  static unsigned long keyPushTime;   //キーを押した時の時間を保持する変数　キーが押されていないときは0
  static byte longPush;               //長押しステータスを保持する変数
  //0:長押しされていない　グローバル変数の長押しフラグを倒す
  //1:長押し検出された　長押し初回のルーチンを起動する　グローバル変数の長押しフラグを立てる
  //2:長押し検出後押されたままの状態　グローバル変数の長押しフラグを立てたままにする

  port = digitalRead(port_);   //キーポートの状態をゲット
  nowTime = millis();          //現在時刻をゲット

  if (longPush == 1) {         //もし、長押し検出されていれば
    key_Long();                  //キー長押し初回のルーチン起動
    longPush = 2;                //長押しステータス2（長押し検出後押されたままの状態）とする
    longPushFlg = true;          //長押しフラグを立てる
  } else if (port == HIGH) {   //もし、ポートがHIGH、つまりキーが離されていれば
    if ((keyPushTime != 0) and (longPush != 2)) {
                                 //もし、キーを押してから経過時間が累積されている（つまりキーを離した直後）
                                 //かつ長押し中では無い時
      if (((nowTime - keyPushTime) > shortTimeFrom_) and ((nowTime - keyPushTime) < shortTimeTo_)) {
                               //キーを押してからの経過時間が　短押し最短時間＜　＜短押し最長時間であれば
        key_Short();             //キー短押しのルーチン起動
      }
    }
    keyPushTime = 0;           //キーを押した時間を0とする
    longPush = 0;              //キーが離されていると認識されたので、長押しステータスを0（長押しされていない状態）とする
    longPushFlg = false;       //
  } else if (port == LOW) {    //もし、ポートがLOW、つまり、キーが押されていれば
    if (keyPushTime == 0) {     //もし、押された直後であれば
      keyPushTime = nowTime;      //キーを押した時間を記録
    } else {                    //押された直後でなければ
      if (((nowTime - keyPushTime) > longTime_) and (longPush != 2)) {
                                  //もしキーを押してからの経過時間＞長押し判定時間 かつ長押し中でない
        longPush = 1;               //長押し検出し、長押し初回のルーチンを起動
      }
    }
  }
}
