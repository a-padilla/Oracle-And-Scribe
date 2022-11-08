#include "scribe.h"

/* ---------- BLUETOOTH ---------- */
void bt_setup(BluetoothSerial& SerialBT){
    Serial.begin(115200);
    SerialBT.begin("Scribe"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!");
}

bool bt_loop(BluetoothSerial& SerialBT, vector<string>& page){
    // if (Serial.available()) {
    //     SerialBT.write(Serial.read());
    // }
    bool is_digit=false;
    string num_lines="",holder="";
    if (SerialBT.available()) {
      while(SerialBT.available()){
        num_lines+=(char)SerialBT.read();
      }
      num_lines=trim(num_lines);
      is_digit=true;
      for(char c: num_lines){
        if(!isdigit(c)){
          is_digit=false;
          break;
        }
      }
      if(is_digit && stoi(num_lines)!=0){
        Serial.write('1');
        page.clear();
        int t=stoi(num_lines);
        while(t){
          delay(50);
          if(SerialBT.available()){
            holder="";
            while(SerialBT.available()){
              holder+=(char)SerialBT.read();
            }
            page.push_back(trim(holder));
            Serial.write('2');
            t--;
          }
        }
        for(char c: num_lines){
          Serial.write(c);
        }
        return true;
      }
    }
    delay(50);
    return false;
}

/* ---------- MISC FUNCTION ---------- */
bool is_lower(char c){
  if(c>=97 && c<=122) return true;
  return false;
}

char to_upper(char c){
  return (char)(c-32);
}

string trim(string s){
  string holder="";
  for(char c: s){
    if(c != '\n' && c != '\r'){
      holder+= c;
    }
  }
  return holder;
}

uint8_t decode(char c){
  char letter=c;
  if(is_lower(c)){
    letter = to_upper(c);
  }
  switch(letter){
    case ' ': return 0b000000;
    case '!': return 0b011101;
    case '"': return 0b000010;
    case '#': return 0b001111;
    case '$': return 0b110101;
    case '%': return 0b100101;
    case '&': return 0b111101;
    case '\'': return 0b001000;
    case '(': return 0b111011;
    case ')': return 0b011111;
    case '*': return 0b100001;
    case '+': return 0b001101;
    case ',': return 0b000001;
    case '-': return 0b001001;
    case '.': return 0b000101;
    case '/': return 0b001100;
    case '0': return 0b001011;
    case '1': return 0b010000;
    case '2': return 0b011000;
    case '3': return 0b010010;
    case '4': return 0b100011;
    case '5': return 0b010001;
    case '6': return 0b011010;
    case '7': return 0b011011;
    case '8': return 0b011001;
    case '9': return 0b001010;
    case ':': return 0b100011;
    case ';': return 0b000011;
    case '<': return 0b110001;
    case '=': return 0b111111;
    case '>': return 0b001110;
    case '?': return 0b100111;
    case '@': return 0b000100;
    case 'A': return 0b100000;
    case 'B': return 0b110000;
    case 'C': return 0b100100;
    case 'D': return 0b100110;
    case 'E': return 0b100010;
    case 'F': return 0b110100;
    case 'G': return 0b110110;
    case 'H': return 0b110010;
    case 'I': return 0b010100;
    case 'J': return 0b010110;
    case 'K': return 0b101000;
    case 'L': return 0b111000;
    case 'M': return 0b101100;
    case 'N': return 0b101110;
    case 'O': return 0b101010;
    case 'P': return 0b111100;
    case 'Q': return 0b111110;
    case 'R': return 0b111010;
    case 'S': return 0b011100;
    case 'T': return 0b011110;
    case 'U': return 0b101001;
    case 'V': return 0b111001;
    case 'W': return 0b010111;
    case 'X': return 0b101101;
    case 'Y': return 0b101111;
    case 'Z': return 0b101011;
    case '[': return 0b010101;
    case '\\': return 0b110011;
    case ']': return 0b110111;
    case '^': return 0b000110;
    case '_': return 0b000111;
    default: return 0b000000;
  }
}

/* ---------- BLUETOOTH FUNCTIONS ---------- */

// void bt_setup(SoftwareSerial& obj){
//     pinMode(bt_en_pin, OUTPUT);
//     digitalWrite(bt_en_pin, HIGH);

//     Serial.begin(BAUD_RATE);
//     Serial.println("Enter AT Commands:");
//     //obj.begin(COMMAND_BAUD_RATE);
// }

// void bt_loop(SoftwareSerial& obj){
//     if(Serial.available())
//         obj.write(Serial.read());

//     if(obj.available())
//         Serial.write(obj.read());
// }