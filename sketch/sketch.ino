// M5Stack Markdown Web Server 
//
// The M5Stack serves a webpage created using markdown, advertises the webpage IP address & counts and dsplays the number of page views.
// Features micro SD / TF card access to display icons on the M5Stack display that are stored on the micro SD / TF card. 
// Sourced from the M5Stack Community Forum - listed under the "News and Lessons" section - with credit to Dimi.
// http://forum.m5stack.com/topic/110/lesson-5-tf-markdown-web-server/2


#include <M5Stack.h>
#include <WiFi.h>

extern unsigned char timer_logo[];
extern unsigned char insertsd_logo[];
extern unsigned char error_logo[];
extern unsigned char wifi_logo[];
extern unsigned char views_logo[];

WiFiServer server(80);

void setup() {
  M5.begin();
  M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
  M5.Lcd.setTextColor(0x7bef);
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawBitmap(30, 75, 59, 59, (uint16_t *)timer_logo);
  M5.Lcd.setCursor(110, 90);
  M5.Lcd.print("STARTING...");
  M5.Lcd.setCursor(110, 110);
  M5.Lcd.print("WAIT A MOMENT");
    
  if (!SD.begin())
  {
    M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
    M5.Lcd.drawBitmap(50, 70, 62, 115, (uint16_t *)insertsd_logo);
    M5.Lcd.setCursor(130, 70);
    M5.Lcd.print("INSERT");
    M5.Lcd.setCursor(130, 90);
    M5.Lcd.print("THE TF-CARD");
    M5.Lcd.setCursor(130, 110);
    M5.Lcd.print("AND TAP");
    M5.Lcd.setCursor(130, 130);
    M5.Lcd.setTextColor(0xe8e4);
    M5.Lcd.print("POWER");
    M5.Lcd.setTextColor(0x7bef);
    M5.Lcd.print(" BUTTON"); 
    while(true);
  }
  else
  {
    if (!configWifi())
    {
      M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
      M5.Lcd.drawBitmap(30, 75, 59, 59, (uint16_t *)error_logo);
      M5.Lcd.setCursor(110, 70);
      M5.Lcd.print("CHECK YOUR");
      M5.Lcd.setCursor(110, 90);
      M5.Lcd.print("WI-FI SETTINGS");
      M5.Lcd.setCursor(110, 110);
      M5.Lcd.print("AND TAP");
      M5.Lcd.setCursor(110, 130);
      M5.Lcd.setTextColor(0xe8e4);
      M5.Lcd.print("POWER");
      M5.Lcd.setTextColor(0x7bef);
      M5.Lcd.print(" BUTTON");
      while(true);
    }
    else
    {
      M5.Lcd.fillRoundRect(0, 0, 320, 240, 7, 0xffff);
      M5.Lcd.drawBitmap(40, 80, 59, 59, (uint16_t *)wifi_logo);  
      M5.Lcd.setCursor(130, 70);
      M5.Lcd.print("MARKDOWN");
      M5.Lcd.setCursor(130, 90);
      M5.Lcd.print("WEB SERVER");
      M5.Lcd.setCursor(130, 110);
      M5.Lcd.print("ON M5STACK");
      M5.Lcd.setCursor(130, 130);
      M5.Lcd.print(WiFi.localIP());
      M5.Lcd.setCursor(130, 150);
      drawViews();
      server.begin();
    }
  }
}

String TFReadFile(String path) {
  File file = SD.open(strToChar(path));
  String buf = "";
  if (file)
  {
    while(file.available())
    {
      buf += (char)file.read();
    }
    file.close();
  }
  return buf;
}

bool TFWriteFile(String path, String str) {
  File file = SD.open(strToChar(path), FILE_WRITE);
  bool res = false;
  if (file)
  {
    if (file.print(str)) res = true;
  }
  file.close();
  return false;
}

char* strToChar(String str) {
  int len = str.length() + 1;
  char* buf = new char[len];
  strcpy(buf, str.c_str());
  return buf;
}

String parseString(int idSeparator, char separator, String str) { // like a split JS
  String output = "";
  int separatorCout = 0;
  for (int i = 0; i < str.length(); i++)
  {
    if ((char)str[i] == separator)
    {
      separatorCout++;
    }
    else
    {
      if (separatorCout == idSeparator)
      {
        output += (char)str[i];
      }
      else if (separatorCout > idSeparator)
      {
        break;
      }
    }
  }
  return output;
}

int cntChrs(String str, char chr) {
  int cnt = 0;
  for (int i = 0; i < str.length(); i++)
  {
    if (str[i] == chr) cnt++;  
  }
  return cnt;
}

bool configWifi() {
  /* Get WiFi SSID & password from wifi.ini from TF-card */
  String file = TFReadFile("/system/wifi.ini");
  if (file != "")
  {
    for (int i = 0; i < cntChrs(file, '\n'); i++)
    {
      String wifi = parseString(i, '\n', file);
      wifi = wifi.substring(0, (wifi.length() - 1)); // remove last char '\r'
      String ssid = parseString(0, ' ', wifi);
      String pswd = parseString(1, ' ', wifi);
      char* ssid_ = strToChar(ssid);
      char* pswd_ = strToChar(pswd);
      if (WiFi.begin(ssid_, pswd_))
      {
        delay(10);
        unsigned long timeout = 10000;
        unsigned long previousMillis = millis();
        while (true)
        {
          unsigned long currentMillis = millis();
          if (currentMillis - previousMillis > timeout) break;
          if (WiFi.status() == WL_CONNECTED) return true;
          delay(100);
        }
      }
    }
  }
  return false;
}

String parseGET(String str) {
  String tmp = "";
  for (int i = 0, j = 0; i < str.length(); i++)
  {
    if (str[i] == ' ') j++;
    if (j == 1)
    {
      if (str[i] != ' ') tmp += str[i];
    }
    if (j == 2) break;
  }
  return tmp;  // tmp.substring(1)
}

String openPage(String page) {
  page += ".md";
  String content = TFReadFile(page);
  if (content != "")
  {
    increaseViews();
    drawViews();
    return content;
  }
  return "# 404 NOT FOUND #\n### MARKDOWN WEB SERVER ON M5STACK  ###"; // if not found 404
}

int getViews() {
  String file = TFReadFile("/system/views");
  if (file != "") return file.toInt();
  return -1;
}

bool increaseViews() {
  int total = getViews();
  if (total != -1)
  {
    total++;
    if (TFWriteFile("/system/views", (String)(total))) return true;
  }
  else
  {
    if (TFWriteFile("/system/views", (String)(1))) return true;
  }
  return false;
}

void drawViews() {
  int total = getViews();
  if (total != -1)
  {
    M5.Lcd.fillRect(5, 185, 315, 60, 0xffff);
    M5.Lcd.drawBitmap(5, 185, 59, 59, (uint16_t *)views_logo);
    M5.Lcd.setCursor(55, 205);
    M5.Lcd.print(total);
  }
}

void loop() {
  String currentString = "";
  bool readyResponse = false;
  WiFiClient client = server.available();
  while (client.connected())
  {
    if (client.available())
    {
      char c = client.read();
      if ((c != '\r') && (c != '\n'))
          currentString += c;
      else
        readyResponse = true;
        
      if (readyResponse)
      {
        String GET = parseGET(currentString);
        String mrkdwnContent = openPage(GET);
        client.flush();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-type:text/html");
        client.println();
        client.println("<html>");
        client.println("<head>");
        client.println("<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>");
        client.println("<title>Markdown page | M5STACK</title>");
        client.println("<link rel=\"icon\" type=\"image/x-icon\" href=\"http://m5stack.com/favicon.ico\">");
        client.println("<script type=\"text/javascript\">" + TFReadFile("/system/markdown.js") + "</script>");
        client.println("<style type=\"text/css\">" + TFReadFile("/system/style.css") + "</style>"); 
        client.println("</head>");
        client.println("<body>");
        client.println("<article></article>");
        client.println("<script type=\"text/javascript\">");
        client.println("const message = `" + mrkdwnContent + "`;");
        client.println("const article = document.querySelector('article');");
        client.println("article.innerHTML = markdown.toHTML(message);");
        client.println("</script>");
        client.println("</body>");
        client.print("</html>");       
        client.println();
        client.println();
        readyResponse = false;
        currentString = "";
        client.stop();
      }
    }
  }
}

