/* 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15

*/

#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C 

#define RST_PIN        		 9          
#define SS_PIN          	10         

#define A_PIN							8
#define B_PIN							7
#define C_PIN							6
#define LATCH_PIN 				4
#define SERIAL_DATA_PIN		5
#define CLK_PIN						3

#define PAWN						0x02
#define ROOK						0x03
#define KNIGHT					0x04
#define BISHOP					0x05
#define QUEEN						0x06
#define KING						0x07


#define FIELD_WIDTH 13
#define FIELD_HEIGHT 21
#define BLOCK_SIZE 3 
#define FIGURE_START_X  ((SCREEN_WIDTH  - FIELD_WIDTH  * BLOCK_SIZE) / 2)
#define FIGURE_START_Y  ((SCREEN_HEIGHT - FIELD_HEIGHT * BLOCK_SIZE) / 2)

/*============================================Arrays with figure images==================================================*/

const uint8_t img_Pawn[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,0,0},
  {0,0,1,1,1,1,1,1,1,1,1,0,0},
  {0,1,1,1,1,1,1,1,1,1,1,1,0},
  {0,1,1,1,1,1,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const uint8_t img_Rook[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0,0},
  {0,0,1,1,0,1,1,0,1,1,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,0,1,1,1,1,1,1,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,1,0,0,0},
  {0,1,1,1,1,1,1,1,1,1,1,0,0},
  {1,1,1,1,1,1,1,1,1,1,1,1,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const uint8_t img_Knight[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,0,1,0,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,0,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,1,1,1,1,1,1,1,1,0,0,0,0},
  {1,1,1,1,1,1,1,1,1,1,0,0,0},
  {1,1,1,1,1,1,0,1,1,1,0,0,0},
  {1,1,1,0,0,0,1,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,1,1,1,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,1,1,0,1,1,1,1,1,0,0,0},
  {0,0,1,0,1,1,1,1,1,0,0,0,0},
  {0,0,1,0,1,1,1,1,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,1,1,1,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,0,0,0,0,0,0},
  {0,0,1,1,1,1,1,1,1,0,0,0,0},
  {0,1,1,0,0,1,1,1,1,1,0,0,0},
  {0,1,1,1,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const uint8_t img_Bishop[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0,0,0,0},
};

const uint8_t img_Queen[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,1,0,1,1,0,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
};

const uint8_t img_King[FIELD_HEIGHT][FIELD_WIDTH] PROGMEM = {
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,0,1,1,0,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,0,1,1,1,1,0,0,0,0},
  {0,0,0,0,1,1,1,1,1,1,0,0,0},
  {0,0,0,1,1,1,1,1,1,1,1,0,0},
};

uint8_t board_data[64];	//Main array storing figure locations on the board
uint8_t up_click;
uint8_t down_click;
uint8_t enter_click;
uint8_t button_pressed;
uint8_t game_allowed = 1;

MFRC522 mfrc522(SS_PIN, RST_PIN);  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/*======================================Shift register functions - controls columns of the board - RST pins of the sensors==================================*/
void hc595_init(void){
  pinMode(CLK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(SERIAL_DATA_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, HIGH);
}

void hc595_send(uint8_t output_reg){
  digitalWrite (LATCH_PIN, LOW);
  for(int8_t i = 7; i >= 0; i--){
		digitalWrite(CLK_PIN, LOW);
    digitalWrite(SERIAL_DATA_PIN, ((output_reg >> i) & 0x01) ? HIGH : LOW);	//Bitwise level change of the data pin for HC595
    digitalWrite(CLK_PIN, HIGH);
  }
  digitalWrite (LATCH_PIN, HIGH);
}

void enable_column (uint8_t column_num){	//enables selected column
	uint8_t column_reg = 0x00;
		column_reg |= (1 << column_num);
	hc595_send(column_reg); 
}

void disable_segment(){			//disables all the columns
	hc595_send(0x00); 
}

/*======================================Decoder function - controls rows of the board - NSS(CS) pins of the sensors==================================*/

void hc138_init(void){
	pinMode(A_PIN, OUTPUT);
	pinMode(B_PIN, OUTPUT);
	pinMode(C_PIN, OUTPUT);
}
void enable_row(uint8_t mux_output){		//enables CS row  0..7. Sets the address on A, B, C pins according to mux_output variable
	digitalWrite(A_PIN, (mux_output & 0x01) ? HIGH : LOW);
	digitalWrite(B_PIN, (mux_output & 0x02) ? HIGH : LOW);
	digitalWrite(C_PIN, (mux_output & 0x04) ? HIGH : LOW);
}

/*======================================The function enables particular sensor of the board  0..63 ==================================*/

void enable_segment(uint8_t segment){			//board segments 0..63
	uint8_t row = segment / 8;							//select row (CS)
	uint8_t column = segment % 8;						//select column (RST)
	enable_column(column);									//RST - HIGH
	enable_row(row);												//Select SS
}

/*====================================== Data writing procedure to the RFID tag, returns 99 for OK ==================================*/
uint8_t write_rfid(uint8_t figure){
	enable_segment(56);
	mfrc522.PCD_Init();		
	mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_avg);	
	byte buffer[16] = {0x00};
	buffer[0] = figure;
  byte block = 1;
  MFRC522::StatusCode status;
	
	MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) 
		key.keyByte[i] = 0xFF;
	if ( ! mfrc522.PICC_IsNewCardPresent()) 
    return 0;
  if ( ! mfrc522.PICC_ReadCardSerial()) 
    return 1;
	status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) 
		return 2;
	status = mfrc522.MIFARE_Write(block, buffer, 16);
  if (status != MFRC522::STATUS_OK) 
		return 3;
	
	mfrc522.PICC_HaltA(); 
  mfrc522.PCD_StopCrypto1();  
	return 99;
}
/*====================================== Writes the data to the RFID tag on A1 cell, up to 50 attempts ==================================*/
uint8_t set_board_data(uint8_t figure){
	for (uint8_t i = 0; i <= 50; i++) {
		enable_segment(56);
		//delay(5);
		if (write_rfid(figure) == 99){
			disable_segment();
			return 99;
		}
	}
	return 0;
}

/*====================================== Tag reading procedure, returns 0 if no tag, returns 1 if reading error ==================================*/

uint8_t read_rfid(void){
	mfrc522.PCD_Init();	
	mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_33dB);
	  
	byte buffer[18] = {0x00};
  byte block = 1;
	byte len = 18;
  MFRC522::StatusCode status;

	MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) 
		key.keyByte[i] = 0xFF;

	if ( ! mfrc522.PICC_IsNewCardPresent()) 
		return 0;
	if ( ! mfrc522.PICC_ReadCardSerial()) 
		return 1;
	status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, block, &key, &(mfrc522.uid));
	if (status != MFRC522::STATUS_OK) 
		return 1;
	status = mfrc522.MIFARE_Read(block, buffer, &len);
	if (status != MFRC522::STATUS_OK) 
		return 1;
	mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
	return buffer[0];
}

/*==========================================Reads all tags on the board==================================================*/
void get_board_data(){
	for(uint8_t i = 0; i <= 63; i++){
		for (uint8_t j = 0; j <= 50; j++) {
			enable_segment(i);
			board_data[i] = read_rfid();
			disable_segment();
			if((board_data[i] == 0) || (board_data[i] >= 4))
				break;
		}
	}
}

/*=======================Checks if command to read all the tags is received over the UART. Command must be 'a' =============================*/
uint8_t serial_check(void){
	uint8_t val = 0;
	if(Serial.available()){
  	val = Serial.read();
		if (val == 97)	
			return 1;
	}
	return 0;
}

/*======================================================Functions for menu drawing===========================================================*/
void draw_success(void){
	display.clearDisplay();
	display.setTextSize(3);
	display.setCursor(15, 20);     
  display.println(F("OK"));
	display.display();
	delay(2000);
}
void draw_failure(void){
	display.clearDisplay();
	display.setTextSize(3);
	display.setCursor(15, 20);     
  display.println(F("ERROR"));
	display.display();
	delay(2000);
}
void draw_position(void){
	display.clearDisplay();
	display.setTextSize(3);
	display.setCursor(15, 5);     
  display.println(F("PLACE"));
	display.setCursor(15, 35);     
  display.println(F("ON A1"));
	display.display();
	delay(2000);
}
void draw_state_0(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("GAME"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
}

void draw_state_1(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("WRITE"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
}
void draw_state_2(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("BLACK"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
}
void draw_state_3(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("WHITE"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
}
void draw_state_4(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("EXIT"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);    
	display.write(25);
  display.display();
}

/*=========================Functions for figure drawing================================================================================*/
void draw_pawn(void){
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_Pawn[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 0);     
  display.println(F("PAWN"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display(); 
}
void draw_rook(void){
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_Rook[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);     
  display.println(F("ROOK"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display(); 
}
void draw_knight(void){
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_Knight[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 0);     
  display.println(F("KNIGHT"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display(); 
}
void draw_bishop(void){
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_Bishop[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 0);     
  display.println(F("BISHOP"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display(); 
}
void draw_queen() {
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_Queen[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 0);     
  display.println(F("QUEEN"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display(); 
}


void draw_king() {
  display.clearDisplay();
  for (int y = 0; y < FIELD_HEIGHT; y++) {
    for (int x = 0; x < FIELD_WIDTH; x++) {
      uint8_t pixel = pgm_read_byte(&img_King[y][x]);
      if (pixel) {
        display.fillRect(FIGURE_START_X + x * BLOCK_SIZE, FIGURE_START_Y + y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, SSD1306_WHITE);
      }
    }
  }
	display.setTextSize(1);     
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 0);     
  display.println(F("KING"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);     
	display.write(25);
  display.display();
  display.display();  
}

void draw_exit(void){
	display.clearDisplay();
  display.setTextSize(3);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(10, 20);     
  display.println(F("EXIT"));
  display.cp437(true);
	display.setTextSize(3);
	display.setCursor(104, 0);     
  display.write(24);
	display.setCursor(104, 40);    
	display.write(25);
  display.display();
}

/*=================================================State automaton to check if any of the buttons was pressed===============================*/
void button_check(void){
	static uint8_t state = 0;		
	static unsigned long start_time;
	static uint8_t input_pin;
	switch (state){
		case 0:{
			
			if(digitalRead(A0) == 0){
				input_pin = A0;
				state = 1;
				start_time = millis();
			}
			if(digitalRead(A1) == 0){
				input_pin = A1;
				state = 1;
				start_time = millis();
			}
			if(digitalRead(A2) == 0){
				input_pin = A2;
				state = 1;
				start_time = millis();
			}
		}
		break;
		case 1:{
			if ((millis() - start_time) >= 25){
				if(digitalRead(input_pin) == 0){ 
					state = 2;
					if(input_pin == A0)
						button_pressed = 1;
					if(input_pin == A1)
						button_pressed = 2;
					if(input_pin == A2)
						button_pressed = 3;
				}
				else 
					state = 0;
			}
		}
		break;
		case 2:{
			if (digitalRead(input_pin) == 1) {
				start_time = millis();
				state = 3;
			}
		}
		break;
		case 3:{
			if ((millis() - start_time) >= 25){
				if(digitalRead(input_pin) == 1)
					state = 0;
			}
		}
		break;
	}
}

/*=================================================State automaton for menu pages===============================*/

void menu_state_automaton(void){
	static uint8_t state = 0;
	switch (state){
		case 0:{
			game_allowed = 1;
			draw_state_0();
			if ((button_pressed == 1) || (button_pressed == 2)){
				button_pressed = 0;
				state = 1;
			}
		}
		break;
		case 1:{
			game_allowed = 0;
			draw_state_1();
			if ((button_pressed == 1) || (button_pressed == 2)){
				button_pressed = 0;
				state = 0;
			}
			if (button_pressed == 3){
				button_pressed = 0;
				state = 2;
			}
		}
		break;
		case 2:{
			draw_state_2();
			
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 4;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 3;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				state = 5;
			}
		}
		break;
		case 3:{
			draw_state_3();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 2;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 4;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				state = 12;
			}

		}
		break;
		case 4:{
			draw_exit();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 3;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 2;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				state = 0;
			}
		}
		break;
		case 5:{
			draw_pawn();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 11;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 6;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(PAWN<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}
		}
		break;
		case 6:{
			draw_rook();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 5;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 7;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(ROOK<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}
		}
		break;
		case 7:{
			draw_knight();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 6;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 8;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(KNIGHT<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}			
		}
		break;
		case 8:{
			draw_bishop();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 7;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 9;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(BISHOP<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 9:{
			draw_queen();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 8;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 10;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(QUEEN<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 10:{
			draw_king();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 9;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 11;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data(KING<<1) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 11:{
			draw_exit();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 10;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 5;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				state = 3;
			}	
		}
		break;
		case 12:{
			draw_pawn();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 18;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 13;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((PAWN<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 13:{
			draw_rook();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 12;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 14;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((ROOK<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 14:{
			draw_knight();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 13;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 15;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((KNIGHT<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 15:{
			draw_bishop();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 14;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 16;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((BISHOP<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 16:{
			draw_queen();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 15;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 17;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((QUEEN<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 17:{
			draw_king();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 16;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 18;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				draw_position();
				if(set_board_data((KING<<1)|0x01) == 99){
					draw_success();
				}
				else{
					draw_failure();
				}
			}	
		}
		break;
		case 18:{
			draw_exit();
			if (button_pressed == 1) {
				button_pressed = 0;
				state = 17;
			}
			if (button_pressed == 2) {
				button_pressed = 0;
				state = 12;
			}
			if (button_pressed == 3) {
				button_pressed = 0;
				state = 2;
			}	
		}
		break;


	}
}

void setup() {
	hc595_init();
	hc138_init();
  disable_segment();		//disable all the sensors
	Serial.begin(9600);		// Initialize serial communications with the PC
	
	while (!Serial);		// Do nothing if no serial port is opened
	SPI.begin();			// Init SPI bus
	display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);	//init display
}

void loop() {
	if(serial_check() && game_allowed){ // If command 'a' is received over UART and menu page GAME is currently active,
		get_board_data();									// all the tags on the boars are red

		for(int i=0;i<=63;i++){
			Serial.print(board_data[i]); //Transfers the data oa all the sensors to UART, space is transferred after every segment
			Serial.print(" ");
		}
	}
	button_check();								//check if the button was pressed
	menu_state_automaton();				//change the active menu page according to the button pressed (if any)
}
