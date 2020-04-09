#include <SoftwareSerial.h>
#include <WiFiEsp.h>
#include <DHT.h>

#define WIFI_SSID "ISMP_HI"  //填入WiFi帳號
#define WIFI_PASSWORD "********"  //填入WiFi密碼

int WiFi_Status = WL_IDLE_STATUS; //ＷiFi狀態

unsigned long prevMillis = 0; //暫存經過時間(單位:毫秒)

#define DHTPIN 8
#define DHTTYPE DHT11
const char *host = "misclicked01.ubddns.org"; // ip or dns
const int port = 8097;

WiFiEspClient client;

SoftwareSerial ESP8266(3, 4);  //設定序列埠物件
DHT dht(DHTPIN, DHT11);  //設定DHT物件


void setup()
{

    //設定序列埠傳輸速率(9600bps)
    Serial.begin(9600);

    //wifi設定
    wifi_Setting();


    Serial.print("connecting to ");
    Serial.println(host);
    if (!client.connect(host, port))
    {   // 判斷連線情況
        Serial.println("connection failed");
        return;
    }

    dht.begin();
}

void loop()
{
    // 組拼url地址
    String url = "/index.php?temperature=" + String(dht.readTemperature()) + "&humidity=" + String(dht.readHumidity());
    Serial.print("requesting URL: ");
    Serial.println(url);
    while (true)
    {
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        if (isnan(h) || isnan(t))
        {
            Serial.println("Failed to read from DHT sensor!");
            return;
        }
        Serial.print("Humidity: ");
        Serial.print(h);
        Serial.print("%\t");
        Serial.print("Temperature: ");
        Serial.print(t);
        Serial.print("*C\n");
        // 傳送GET請求
        // 組拼HTTPS請求的Header
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" +
                     "Connection: Keep Alive\r\n\r\n");

        Serial.println("request sent");

        // 讀取連線情況
        while (client.available())
        {
            String line = client.readStringUntil('\n');
            if (line == "\r")
            {
                Serial.println("headers received");
                break;
            }
        }

        // 讀取資料
        String line = client.readStringUntil('\n');
        Serial.println("reply was:");
        Serial.println("==========");
        Serial.println(line);   // 列印接受到的資料
        Serial.println("==========");
        delay(50);
    }
}

//wifi設定方法
void wifi_Setting()
{
    ESP8266.begin(9600);

    //初始化ESP模組
    WiFi.init(&ESP8266);

    Serial.print("進行WiFi設定!\r\n");
    do
    {
        Serial.println("WiFi 連接中 ...");
        WiFi_Status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(500);
    } while (WiFi_Status != WL_CONNECTED);

    Serial.print("ＷiFi 連接成功!\r\n");
    Serial.print("IP 位址: ");
    Serial.println(WiFi.localIP());
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.println("WiFi 設定結束\r\n");

}
