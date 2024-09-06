// https://how2electronics.com/how-to-use-ads1115-16-bit-adc-module-with-esp32/

#include <ESP8266WiFi.h>
#include <Adafruit_ADS1X15.h>
Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
//Adafruit_ADS1015 ads;     /* Use this for the 12-bit version */
#include "I2CScanner.h"
I2CScanner scanner;

#define ADS1115_SDA D2
#define ADS1115_SCL D1

const char* ssid = "BionicControl";
const char* password = "123456789";
const char* serverAddress = "192.168.4.1";

WiFiClient client;

void setup(void)
{
  Serial.begin(115200);

  pinMode(ADS1115_SDA, INPUT_PULLUP);
  pinMode(ADS1115_SCL, INPUT_PULLUP);

  Wire.begin(ADS1115_SDA, ADS1115_SCL);
  scanner.Init();
  scanner.Scan();
  while (!ads.begin())
  {
    Serial.println("Failed to initialize ADS.");
    Serial.flush();
    delay(1000);
  }

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  //ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  ads.setGain(GAIN_SIXTEEN);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Median filter over a sliding window of 3 values
float buf0[3] = {0.0, 0.0, 0.0};
float buf1[3] = {0.0, 0.0, 0.0};
int8_t bufidx = 0;
float med0 = 0.0;
float med1 = 0.0;

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

// Average over 20 values
int8_t count = 0;
int8_t avgover = 20;
float sum0 = 0.0;
float sum1 = 0.0;
float avg0 = 0.0;
float avg1 = 0.0;

int8_t f_count = 0;
float f_sum0 = 0.0;
float f_sum1 = 0.0;
float f_avg0 = 0.0;
float f_avg1 = 0.0;

// To measure concentration level
const uint8_t buffer_size = 50;
float buffer0[buffer_size], buffer1[buffer_size];
uint8_t buffer_index = 0;
float maxvalue0 = -100;
float minvalue0 = 100;
float maxvalue1 = -100;
float minvalue1 = 100;

// DC Filter to separate AC and DC
// http://sam-koblenski.blogspot.com/2015/10/everyday-dsp-for-programmers-signal.html
class KalmanFilter {
  float dc_value;
  float alpha;
  public:
  KalmanFilter(float a=0.95)
  { dc_value = 0.0; alpha = a; }

  float ac(float v)
  {
    dc_value = (1 - alpha) * v + alpha * dc_value;
    return (v - dc_value);
  }

  float dc()
  { return (dc_value); }
};

KalmanFilter filter0;
KalmanFilter filter1;

void loop(void)
{
  int16_t adc0, adc1;
  float volts0, volts1;

  adc0 = ads.readADC_Differential_0_1();
  adc1 = ads.readADC_Differential_2_3();

  volts0 = ads.computeVolts(adc0);
  volts1 = ads.computeVolts(adc1);
  
  // Median filter
  buf0[bufidx] = volts0;
  buf1[bufidx] = volts1;
  bufidx = (bufidx+1)%3;
  med0 = (MAX(MIN(buf0[0], buf0[1]), MIN(MAX(buf0[0], buf0[1]), buf0[2])))*1000;
  med1 = (MAX(MIN(buf1[0], buf1[1]), MIN(MAX(buf1[0], buf1[1]), buf1[2])))*1000;
  // Serial.print(med0);Serial.print(",");Serial.println(med1);

  // Kalman filter
  filter0.ac(med0);
  filter1.ac(med1);

  // Average over values
  sum0 += filter0.dc();
  sum1 += filter1.dc();
  count++;
  if(count >= avgover){
    avg0 = sum0/avgover;
    avg1 = sum1/avgover;

    sum0 = 0.0;
    sum1 = 0.0;
    count = 0;

    // Compute the range of each
    buffer0[buffer_index] = avg0;
    buffer1[buffer_index] = avg1;
    buffer_index = (buffer_index+1)%buffer_size;
    maxvalue0 = minvalue0 = buffer0[0];
    maxvalue1 = minvalue1 = buffer1[0];
    for(uint8_t i = 0; i < buffer_size; i++){
      if(buffer0[i]>maxvalue0) maxvalue0 = buffer0[i];
      if(buffer0[i]<minvalue0) minvalue0 = buffer0[i];
      if(buffer1[i]>maxvalue1) maxvalue1 = buffer1[i];
      if(buffer1[i]<minvalue1) minvalue1 = buffer1[i];
    }
    
    f_sum0 += maxvalue0-minvalue0;
    f_sum1 += maxvalue1-minvalue1;
    f_count++;
    
    Serial.println();
    Serial.print(maxvalue0-minvalue0);Serial.print(" , ");Serial.println(maxvalue1-minvalue1);
    
    if(f_count >= avgover){
      f_avg0 = f_sum0/avgover;
      f_avg1 = f_sum1/avgover;

      f_sum0=0;
      f_sum1=0;
      f_count=0;

      if (!client.connected()) { // Connect only if not already connected
        if (client.connect(serverAddress, 80)) {
          Serial.println("Connected to server");
        } else {
          Serial.println("Connection failed");
          return; // Exit loop if connection fails
        }
      }
      if(f_avg0 <= 0.09){
        Serial.println("Calling Motor 1 to move ");
        client.println("GET /FWD1 HTTP/1.1");
        client.println("Host: BionicControl");    // Replace with your AP's hostname if different
        client.println("Connection: keep-alive"); // Keep connection alive
        client.println();
      }
      if(f_avg1 <= 0.09){
        Serial.println("Calling Motor 2 to move ");
        client.println("GET /FWD2 HTTP/1.1");
        client.println("Host: BionicControl");    // Replace with your AP's hostname if different
        client.println("Connection: keep-alive"); // Keep connection alive
        client.println();
      }
    }
  }
}