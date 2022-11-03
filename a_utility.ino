#define RED_THRESHOLD 200 // Raw RED threshold 
#define RED_ORANGE_THRESHOLD 110 // Raw GREEN threshold to differentiate b/w red and orange
#define PURPLE_GREENBLUE_THRESHOLD 1.2 // Ratio of red and green to differentiate b/w purple and green/blue
#define WHITE_THRESHOLD 200

int match_color() {
  int red = currentColor[R], green = currentColor[G], blue = currentColor[B];
  if (red > WHITE_THRESHOLD and green > WHITE_THRESHOLD and blue > WHITE_THRESHOLD) return C_WHITE;
  
  if (red > RED_THRESHOLD) { // Either RED or ORANGE
    if (green > RED_ORANGE_THRESHOLD) return C_ORANGE;
    else return C_RED;
    
  } else {
    float small = min(red, green);
    float large = max(red, green);
    if (small <= 0) small = 1; // Handle division by 0
    float diffGreenRed = large / small;

    if (red >= green or abs(diffGreenRed) < PURPLE_GREENBLUE_THRESHOLD) 
      return C_PURPLE;
    else {
      if (green > blue) return C_GREEN;
      else return C_BLUE;
    }
  }
}

float low_pass_alpha = 0.05;
float initial_threshold = 3.0;
float prev_signal = 0.0; 

float apply_low_pass_filter(float distance_cm) {
  float filtered_signal;

  if (abs(distance_cm - prev_signal) > initial_threshold) filtered_signal = distance_cm;
  else filtered_signal = (1-low_pass_alpha) * prev_signal + (low_pass_alpha * distance_cm);
  
#ifndef DEBUG_FILTER
  Serial.print("Filtered: ");
  Serial.println(filtered_signal);
#endif

  prev_signal = filtered_signal;
  return filtered_signal;
}

void read_eeprom() {
  int eeAddress = 0;
  
  Serial.println("Retrieving balance values from EEPROM...");
  for (int b = 0; b < 3; b++) {
    for (int c = 0; c < 3; c++) {
      EEPROM.get(eeAddress, balanceArray[b][c]);
      eeAddress += sizeof(int);
      
      Serial.print(balanceArray[b][c]);
      Serial.print(",");
    }
    Serial.println("");
  }
  
  char firstCheck = EEPROM.read(eeAddress); 
  char secondCheck = EEPROM.read(eeAddress + 1);
  char thirdCheck = EEPROM.read(eeAddress + 2);

  if (firstCheck != 'J' or secondCheck != 'B' or thirdCheck != 'M') {
    Serial.println("Issue retrieving value from EEPROM! Please run calibration again!");
    while(true);
  }
}
