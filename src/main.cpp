#include <M5Stack.h>
#include <Wire.h>
#include <DFRobot_SHT20.h>
#include <PMserial.h>

#define TFT_GREY 0x7BEF

#define X_LOCAL 40
#define Y_LOCAL 30

#define X_OFFSET 160
#define Y_OFFSET 23

SerialPM pms(PMSA003, Serial2); // PMSx003, UART
DFRobot_SHT20    sht20;

// Print the header for a display screen
void header(const char *string, uint16_t color)
{
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_BLACK, color);
    M5.Lcd.fillRect(0, 0, 320, 30, color);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.drawString(string, 160, 3, 4);
}

void setup() {

    M5.begin();
    Serial.begin(9600);
    pinMode(10, OUTPUT);
    digitalWrite(10, 1);

    M5.Lcd.fillScreen(TFT_BLACK);
    header("PM2.5", TFT_BLACK);

    sht20.initSHT20();                                  // Init SHT20 Sensor
    delay(100);
    sht20.checkSHT20();
    pms.init();
}

#define FRONT 2

long get_us_aqi(uint16_t pm25, uint16_t pm100)
{
    long aqi_pm25, aqi_pm100;

    switch(pm25){
    case 0 ... 12:
        aqi_pm25 = map(pm25, 0, 12, 0, 50);
        break;
    case 13 ... 35:
        aqi_pm25 = map(pm25, 13, 35, 51, 100);
        break;
    case 36 ... 55:
        aqi_pm25 = map(pm25, 36, 55, 101, 150);
        break;
    case 56 ... 150:
        aqi_pm25 = map(pm25, 56, 150, 151, 200);
        break;
    case 151 ... 250:
        aqi_pm25 = map(pm25, 151, 250, 201, 300);
        break;
    case 251 ... 350:
        aqi_pm25 = map(pm25, 251, 350, 301, 400);
        break;
    case 351 ... 500:
        aqi_pm25 = map(pm25, 351, 500, 401, 500);
        break;
    default:
        aqi_pm25 = 500;
    }

    switch(pm100){
    case 0 ... 12:
        aqi_pm100 = map(pm100, 0, 54, 0, 50);
        break;
    case 13 ... 35:
        aqi_pm100 = map(pm100, 55, 154, 51, 100);
        break;
    case 36 ... 55:
        aqi_pm100 = map(pm100, 155, 254, 101, 150);
        break;
    case 56 ... 150:
        aqi_pm100 = map(pm100, 255, 354, 151, 200);
        break;
    case 151 ... 250:
        aqi_pm100 = map(pm100, 355, 424, 201, 300);
        break;
    case 251 ... 350:
        aqi_pm100 = map(pm100, 425, 504, 301, 400);
        break;
    case 351 ... 500:
        aqi_pm100 = map(pm100, 505, 604, 401, 500);
        break;
    default:
        aqi_pm100 = 500;
    }
    return max(aqi_pm25, aqi_pm100);
}

uint16_t get_us_aqi_colour(long aqi) {
    switch(aqi) {
    case 0 ... 50:
        return M5.Lcd.color565(0x00, 0xe4, 0x00);
    case 51 ... 100:
        return M5.Lcd.color565(0xff, 0xff, 0x00);
    case 101 ... 150:
        return M5.Lcd.color565(0xff, 0x7e, 0x00);
    case 151 ... 200:
        return M5.Lcd.color565(0xff, 0x00, 0x00);
    case 201 ... 300:
        return M5.Lcd.color565(0x99, 0x00, 0x4c);
    case 301 ... 500:
    default:
        return M5.Lcd.color565(0x7e, 0x00, 0x23);
    }
}

long get_indian_aqi(uint16_t pm25, uint16_t pm100)
{
    long aqi_pm25, aqi_pm100;
    // map(value, fromLow, fromHigh, toLow, toHigh)

    switch(pm25){
    case 0 ... 30:
        aqi_pm25 = map(pm25, 0, 30, 0, 50);
        break;
    case 31 ... 60:
        aqi_pm25 = map(pm25, 31, 60, 51, 100);
        break;
    case 61 ... 90:
        aqi_pm25 = map(pm25, 61, 90, 101, 200);
        break;
    case 91 ... 120:
        aqi_pm25 = map(pm25, 91, 120, 201, 300);
        break;
    case 121 ... 250:
        aqi_pm25 = map(pm25, 121, 250, 301, 400);
        break;
    case 251 ... 380:
        aqi_pm25 = map(pm25, 251, 380, 401, 500);
        break;
    default:
        aqi_pm25 = 500;
    }

    switch(pm100){
    case 0 ... 100:
        // first 2 tiers are identical to pm100
        aqi_pm100 = pm100;
        break;
    case 101 ... 250:
        aqi_pm100 = map(pm100, 101, 250, 101, 200);
        break;
    case 251 ... 350:
        aqi_pm100 = map(pm100, 251, 350, 201, 300);
        break;
    case 351 ... 430:
        aqi_pm100 = map(pm100, 351, 430, 301, 400);
        break;
    case 431 ... 510:
        aqi_pm100 = map(pm100, 431, 510, 401, 500);
        break;
    default:
        aqi_pm100 = 500;
    }
    return max(aqi_pm25, aqi_pm100);
}

long get_uk_aqi(uint16_t pm25, uint16_t pm100)
{
    long aqi_pm25, aqi_pm100;

    switch(pm25){
    case 0 ... 11:
        aqi_pm25 = 1;
        break;
    case 12 ... 23:
        aqi_pm25 = 2;
        break;
    case 24 ... 35:
        aqi_pm25 = 3;
        break;
    case 36 ... 41:
        aqi_pm25 = 4;
        break;
    case 42 ... 47:
        aqi_pm25 = 5;
        break;
    case 48 ... 53:
        aqi_pm25 = 6;
        break;
    case 54 ... 58:
        aqi_pm25 = 7;
        break;
    case 59 ... 64:
        aqi_pm25 = 8;
        break;
    case 65 ... 70:
        aqi_pm25 = 9;
        break;
    case 71:
    default:
        aqi_pm25 = 10;
    }

    switch(pm100){
    case 0 ... 16:
        aqi_pm100 = 1;
        break;
    case 17 ... 33:
        aqi_pm100 = 2;
        break;
    case 34 ... 50:
        aqi_pm100 = 3;
        break;
    case 51 ... 58:
        aqi_pm100 = 4;
        break;
    case 59 ... 66:
        aqi_pm100 = 5;
        break;
    case 67 ... 75:
        aqi_pm100 = 6;
        break;
    case 76 ... 83:
        aqi_pm100 = 7;
        break;
    case 84 ... 91:
        aqi_pm100 = 8;
        break;
    case 92 ... 100:
        aqi_pm100 = 9;
        break;
    case 101:
    default:
        aqi_pm100 = 10;
    }
    return max(aqi_pm25, aqi_pm100);
}

void LCD_Display_Val(void)
{
    long us_aqi = get_us_aqi(pms.pm25, pms.pm10);

    header("PM2.5", get_us_aqi_colour(us_aqi));

    M5.Lcd.setTextColor(TFT_RED,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL , FRONT);
    M5.Lcd.print("AQI");

    M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET , FRONT);
    M5.Lcd.print("US:  ");
    M5.Lcd.print(us_aqi);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*2 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*2 , FRONT);
    M5.Lcd.print("IND: ");
    M5.Lcd.print(get_indian_aqi(pms.pm25, pms.pm10));

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*3 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*3 , FRONT);
    M5.Lcd.print("UK:  ");
    M5.Lcd.print(get_uk_aqi(pms.pm25, pms.pm10));

    M5.Lcd.setTextColor(TFT_RED,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL , FRONT);
    M5.Lcd.print("ATME");

    M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET, FRONT);
    M5.Lcd.print("PM1.0: ");
    M5.Lcd.print(pms.pm01);

    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*2 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*2 , FRONT);
    M5.Lcd.print("PM2.5: ");
    M5.Lcd.print(pms.pm25);

    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*3 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*3 , FRONT);
    M5.Lcd.print("PM10:  ");
    M5.Lcd.print(pms.pm10);

    M5.Lcd.setTextColor(TFT_RED,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET/4, Y_LOCAL + Y_OFFSET*4 , FRONT);
    M5.Lcd.print("Number of particles");

    M5.Lcd.setTextColor(TFT_WHITE,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*5 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*5 , FRONT);
    M5.Lcd.print("0.3um: ");
    M5.Lcd.print(pms.n0p3);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*6 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*6 , FRONT);
    M5.Lcd.print("0.5um: ");
    M5.Lcd.print(pms.n0p5);

    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*7 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*7 , FRONT);
    M5.Lcd.print("1.0um: ");
    M5.Lcd.print(pms.n1p0);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET*5 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET*5 , FRONT);
    M5.Lcd.print("2.5um: ");
    M5.Lcd.print(pms.n2p5);

    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*6 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*6 , FRONT);
    M5.Lcd.print("5.0um: ");
    M5.Lcd.print(pms.n5p0);

    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*7 , FRONT);
    M5.Lcd.print("             ");
    M5.Lcd.setCursor(X_LOCAL+ X_OFFSET, Y_LOCAL + Y_OFFSET*7 , FRONT);
    M5.Lcd.print("10um : ");
    M5.Lcd.print(pms.n10p0);
}


void TempHumRead(void)
{
    float humd = sht20.readHumidity();                  // Read Humidity
    float temp = sht20.readTemperature();               // Read Temperature


    M5.Lcd.setTextColor(TFT_GREEN,TFT_BLACK);
    M5.Lcd.setCursor(X_LOCAL, Y_LOCAL + Y_OFFSET*8 , FRONT);
    M5.Lcd.print("TMP: ");
    M5.Lcd.print(temp);

    M5.Lcd.setCursor(X_LOCAL + X_OFFSET, Y_LOCAL + Y_OFFSET*8, FRONT);
    M5.Lcd.print("HUM: ");
    M5.Lcd.print(humd);
}

void loop() {
    pms.read(true);
    if(pms) {
        LCD_Display_Val();
        TempHumRead();
    }
}
