# M5Core2_SG90_StackChan_VoiceText_Ataru forked from @robo8080

[@robo8080さんからフォーク](https://github.com/robo8080/M5Core2_SG90_StackChan_VoiceText_Ataru)したものです。  
PlatformIOを使用していますので、Arduino IDEでコンパイルできない場合があるかもしれません。  


## 変更点

- WiFiやVoiceText Web APIサービスの設定をEvn.hファイルに移動しました。  
  Env.h.sampleファイルをコピーしEnv.hに名称変更して編集してください。
- PandFaceとTVFaceを追加しました。<br />
  Button Aで __あたる__ と __スースー__ を切り替えます。<br />
  Button Bで __ラム__ と __ブラウン__ を切り替えます。<br />
- WiFiMultiで複数のWiFiポイントに接続できます。
- 時刻を喋る様にしました。<br />
  Env.hのTIME_ANNOUNCE_INTERVALで喋らせるインターバルを指定します。

## WiFi設定

Env.h.sampleコピーしEnv.hファイルを作成します。

```
// WiFi settings
#ifndef WIFI_SSID
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
// If you have another point, uncomment below two lines.
//#define WIFI_SSID2       "YOUR_WIFI_SSID2"
//#define WIFI_PASSWORD2   "YOUR_WIFI_PASSWORD2"
// If you have more points, repeat WIFI_SSID3 and WIFI_PASSWORD3, 4, 5 ...
#endif
```

"YOUR_WIFI_SSID"にSSID名を入力し、"YOUR_WIFI_PASSWORD"にパスワードを入力します。  
WiFiポイントが2つある場合はWIFI_SSID2とWIFI_PASSWORD2の定義のコメントを外し、"YOUR_WIFI_SSID2"、"YOUR_WIFI_PASSWORD2"に入力します。
3カ所以上ある場合はWIFI_PASSWORD2定義の下に同じ様にWIFI_SSID3、WIFI_PASSWORD3、WIFI_SSID4、WIFI_PASSWORD4...と追加し、M5Core2_SG90_StackChan_VoiceText_Ataru.inoファイルも下の様に```wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2)```の下に追加します。

```
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
#ifdef WIFI_SSID2
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASSWORD2);
#endif
  wifiMulti.addAP(WIFI_SSID3, WIFI_PASSWORD3);
  wifiMulti.addAP(WIFI_SSID4, WIFI_PASSWORD4);
  .
  .
```



以下オリジナルのドキュメントです。

# M5Core2_SG90_StackChan_VoiceText_Ataru
M5Core2_SG90_StackChan_VoiceText_Ataru

@mongonta555 さんが[ｽﾀｯｸﾁｬﾝ M5GoBottom版組み立てキット](https://raspberrypi.mongonta.com/about-products-stackchan-m5gobottom-version/ "Title")の頒布を始められたので、それに対応したｽﾀｯｸﾁｬﾝファームを作ってみました。<br>

![画像ataru](images/ataru.png)<br>

---
### M5GoBottom版ｽﾀｯｸﾁｬﾝ本体を作るのに必要な物、及び作り方 ###
こちらを参照してください。<br>
* [ｽﾀｯｸﾁｬﾝ M5GoBottom版組み立てキット](https://raspberrypi.mongonta.com/about-products-stackchan-m5gobottom-version/ "Title")<br>

### このプログラムをビルドするのに必要な物 ###
* Arduino IDE (バージョン 1.8.15で動作確認をしました。~~1.8.16だとエラーになるという報告を頂きました。~~)<br>
* [M5Stack-Avatar](https://github.com/meganetaaan/m5stack-avatar/ "Title")ライブラリ(バージョン 0.7.3で動作確認をしました。)<br>
* [ServoEasing](https://github.com/ArminJo/ServoEasing/ "Title")ライブラリ(バージョン 2.4.0で動作確認をしました。)<br>
* [ESP32Servo](https://github.com/madhephaestus/ESP32Servo/ "Title")ライブラリ(バージョン 0.9.0で動作確認をしました。)<br>
* [ESP8266Audio](https://github.com/earlephilhower/ESP8266Audio/ "Title")ライブラリ(バージョン 1.9.3で動作確認をしました。)<br><br>
(arduino-esp32 v2.0.1だとエラーになるという報告を頂きました。)<br>

### M5Stack-Avatarライブラリを変更する ###
* "Avatar.cpp"の下図赤枠部分を変更します。
![画像1](images/image1.png)<br><br><br>
* "Avatar.cpp""Avatar.h"の下図赤枠部分を追加します。
![画像2](images/image2.png)<br><br><br>
![画像3](images/image3.png)<br><br><br>
* "DrawContext.h"の下図赤枠部分を変更します。
![画像4](images/image4.png)<br><br><br>


---

---

M5Stack Core2の場合、M5Stack-AvatarとHOYA社が提供する[VoiceText Web APIサービス](https://cloud.voicetext.jp/webapi "Title")を使った音声合成(TTS)を使うことができます。


VoiceText TTSは、kghrlaboさんのesp32_text_to_speechを参考にさせていただきました。<br>
オリジナルはこちら。<br>
esp32_text_to_speech <https://github.com/kghrlabo/esp32_text_to_speech><br>

### VoiceTextの有効化 ###
* M5Core2_SG90_StackChan_VoiceText_Ataru.inoの22行目のコメントを外して”USE_VOICE_TEXT”を有効化してください。

### WiFiの設定 ###
* M5Core2_SG90_StackChan_VoiceText_Ataru.inoの31行目付近、SSIDとPASSWORDを設定してください。

### VoiceText Wev API api キーの設定 ###
* AudioFileSourceVoiceTextStream.cppの30行目付近、YOUR_TSS_API_KEYを設定してください。<br>
APIキーは、[ここ](https://cloud.voicetext.jp/webapi/ "Title")の「無料利用登録」から申請すれば、メールで送られて来ます。<br>

---

### 使い方 ###
* VoiceTextを有効にしていない場合：M5Stack Core2のボタンA,B,Cを押すと、それぞれ異なった顔を表示します。　<br>
* VoiceTextを有効にしている場合：M5Stack Core2のボタンA,B,Cを押すと、それぞれ異なった顔と声でしゃべります。　<br>
音声データをダウンロード中は顔にハートマークが表示されます。<br>
TTSのパラメータの詳細はこちらを参照してください。<br>
[VoiceText Web API [API マニュアル](https://cloud.voicetext.jp/webapi/docs/api/ "Title")]
<br><br>

