#include <WiFiClientSecure.h>
#include <vector>
#include <UrlEncode.h>
#include "./env.hpp"

const char* ssid     = "R409";     // your network SSID (name of wifi network)
const char* password = ""; // your network password

const char*  server = "osu-workers.oein0219.workers.dev";  // Server URL

const char* test_root_ca= \
     R"(
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)";


WiFiClientSecure client;

template<typename T>
class NetworkResponse {
public:
  bool success;
  T data;

  NetworkResponse(bool s) {
    success = s;
  }
  NetworkResponse(bool s, T d) {
    success = s;
    data = d;
  }
};

class NetworkStreamParser {
private:
  int base64ToNumber(const String base64) {
    const String base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int num = 0;
    int power = 1;

    for (int i = 0; i < base64.length(); i++) {
        if(base64[i] == '_') {
            continue;
        }
        int index = -1;
        for(int j = 0; j < base.length(); ++j) {
            if (base[j] == base64[i]) {
                index = j;
                break;
            }
        }
        if (index != -1) {
            num += index * power;
            power *= 64;
        } else {
            printf("Invalid base64 character: %c, parsing %s\n", base64[i], base64.c_str());
            return -1; // Return an error code for invalid input
        }
    }

    return num;
  }

  unsigned long base64ToLargeNumber(const String base64) {
    const String base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned long num = 0;
    unsigned long power = 1;

    for (int i = 0; i < base64.length(); i++) {
        if(base64[i] == '_') {
            continue;
        }
        int index = -1;
        for(int j = 0; j < base.length(); ++j) {
            if (base[j] == base64[i]) {
                index = j;
                break;
            }
        }
        if (index != -1) {
            num += index * power;
            power *= 64;
        } else {
            printf("Invalid base64 character: %c, parsing %s\n", base64[i], base64.c_str());
            return -1; // Return an error code for invalid input
        }
    }

    return num;
  }
public:
  void consumeHeader() {
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
  }

  String getStringOfLength(int length) {
    String result = "";
    // printf("==(%d)> ", length);
    while (length > 0) {
      if (client.available()) {
        char c = client.read();
        result += c;
        length--;
        // printf("%c", c);
      }
    }
    // printf("\n");
    return result;
  }
  
  int getNumber(int length) {
    return base64ToNumber(getStringOfLength(length));
  }

  unsigned long getLargeNumber(int length) {
    return base64ToLargeNumber(getStringOfLength(length));
  }

  String parseString(int length) {
    int len = getNumber(length);
    return getStringOfLength(len);
  }
};

class OSUBeatmapItem {
public:
  unsigned long id;
  String version;
  float star;
};

class OSUMapResponseItem {
public:
  String title;
  String artist;
  std::vector<OSUBeatmapItem> maps;
};

#define IOSUMapResponse NetworkResponse<std::vector<OSUMapResponseItem>>

NetworkStreamParser parser;

class OSUAPI {
public:
  IOSUMapResponse search(String keyword = "", int page = 0) {
    if (!client.connect(server, 443))
      return IOSUMapResponse(false);
    client.print("GET https://osu-workers.oein0219.workers.dev/s?t=");
    client.print(_API_TOKEN_);
    if(keyword != ""){
      client.print("&q=");
      client.print(urlEncode(keyword.c_str()).c_str());
    }
    client.print("&p=");
    client.print(String(page).c_str());
    client.println(" HTTP/1.0");
    client.println("Host: osu-workers.oein0219.workers.dev");
    client.println("Connection: close");
    client.println();
    parser.consumeHeader();
    
    unsigned long length = parser.getLargeNumber(2);
    std::vector<OSUMapResponseItem> result;
    for (int i = 0; i < length; i++) {
      OSUMapResponseItem item;
      // printf("Getting map %d out of %zu\n", i, length);
      parser.getStringOfLength(2); // consume length
      item.title = parser.parseString(2);
      // printf("  Title: %s\n", item.title.c_str());
      item.artist = parser.parseString(2);
      // printf("  Artist: %s\n", item.artist.c_str());
      unsigned long mapLength = parser.getLargeNumber(2);
      for (int j = 0; j < mapLength; j++) {
        // printf("  Getting beat %d out of %zu\n", j, mapLength);
        OSUBeatmapItem map;
        parser.getStringOfLength(2); // consume length
        map.id = parser.parseString(2).toInt();
        // printf("    ID: %lu\n", map.id);
        map.version = parser.parseString(2);
        // printf("    Version: %s\n", map.version.c_str());
        map.star = parser.parseString(2).toFloat();
        // printf("    Star: %f\n", map.star);
        item.maps.push_back(map);
      }
      result.push_back(item);
    }

    client.stop();
    return IOSUMapResponse(true, result);
  }
};

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  delay(100);

  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

  client.setCACert(test_root_ca);
  //client.setCertificate(test_client_cert); // for client verification
  //client.setPrivateKey(test_client_key);	// for client verification

  OSUAPI api;
  IOSUMapResponse res = api.search("");
  if (res.success) {
    for (int i = 0; i < res.data.size(); i++) {
      OSUMapResponseItem item = res.data[i];
      Serial.println("==================");
      Serial.print("Title: ");
      Serial.println(item.title);
      Serial.print("Artist: ");
      Serial.println(item.artist);
      for (int j = 0; j < item.maps.size(); j++) {
        OSUBeatmapItem map = item.maps[j];
        Serial.print("  ID : ");
        Serial.print(map.id);
        Serial.print(" | VER : ");
        Serial.print(map.version);
        Serial.print(" | STAR : ");
        Serial.println(map.star);
      }
    }
  } else {
    Serial.println("Failed");
  }
}

void loop() {
  // do nothing
}
