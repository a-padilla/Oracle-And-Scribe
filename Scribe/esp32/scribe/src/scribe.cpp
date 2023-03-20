#include "scribe.h"


/* =========== BT FUNCTIONS =========== */

void bt_setup(BluetoothSerial& SerialBT){
    Serial.begin(115200);
    SerialBT.begin("Scribe"); //Bluetooth device name
    Serial.println("The device started, now you can pair it with bluetooth!\n");
}

bool bt_loop(BluetoothSerial& SerialBT, vector<string>& page){
    
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
        // Serial.write('1');
        SerialBT.write('1');
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
            // Serial.write('2');
            SerialBT.write('2');
            t--;
          }
        }
        for(char c: num_lines){
          // Serial.write(c);
          SerialBT.write(c);
        }
        return true;
      }else if(stoi(num_lines)==0){
        SerialBT.write('1');
        for(char c: num_lines){
          // Serial.write(c);
          SerialBT.write(c);
        }
      }
    }
    delay(50);
    return false;
}


/* ========== CORE FUNCTIONS ========== */

void poll(const int button_pin, long &lbt, int &button, char bp, char np, BluetoothSerial& SerialBT, vector<string>& page, vector<string>& book, string& curr_page, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind){
    button=digitalRead(button_pin);
    if(((long)millis()-lbt)>DEBOUNCE_DELAY){
      // if button pressed
      if(button==HIGH){
        if(bp=='b'){
          if(np=='n'){
            curr_burst=next_burst(curr_page, curr_burst, page_ind, burst_ind, char_ind);
            print_info(page_ind, burst_ind, (int)book.size(), curr_page, curr_burst);
          }else{
            curr_burst=prev_burst(curr_page, curr_burst, page_ind, burst_ind, char_ind);
            print_info(page_ind, burst_ind, (int)book.size(), curr_page, curr_burst);
          }
        }else if(bp=='p'){
          if(np=='n'){
            curr_page=next_page(book, curr_burst, page_ind, burst_ind, char_ind, SerialBT, page);
            curr_burst = burst_from_page(curr_page, burst_ind);
            print_info(page_ind, burst_ind, (int)book.size(), curr_page, curr_burst);
          }else{
            curr_page=prev_page(book, curr_burst, page_ind, burst_ind, char_ind);
            curr_burst = burst_from_page(curr_page, burst_ind);
            print_info(page_ind, burst_ind, (int)book.size(), curr_page, curr_burst);
          }
        }
        lbt=millis();
      }
    }
}

string next_burst(string& curr_page, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind){
  if(!last_burst(curr_page, burst_ind)){
    char_ind += BURST_LEN;
    burst_ind++;
    return burst_from_page(curr_page, burst_ind);
  }else{
    return burst_from_page(curr_page, burst_ind);
  }
}

string prev_burst(string& curr_page, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind){
  if(burst_ind==0){
    char_ind=0;
    return curr_burst;
  }else{
    burst_ind--;
    char_ind-=BURST_LEN;
    return burst_from_page(curr_page, burst_ind);
  }
}

string next_page(vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind, BluetoothSerial& SerialBT, vector<string>& page){
  if(page_ind==book.size()-1){
    get_page(SerialBT, page, book, curr_burst, page_ind, burst_ind, char_ind);
    return book[page_ind];
  }else{
    page_ind++;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }
}

string prev_page(vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind){
  if(page_ind==0){
    page_ind=0;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }else{
    page_ind--;
    char_ind=0;
    burst_ind=0;
    return book[page_ind];
  }
}

void get_page(BluetoothSerial& SerialBT, vector<string>& page, vector<string>& book, string& curr_burst, int& page_ind, int& burst_ind, int& char_ind){
  if(bt_loop(SerialBT, page)){ // new page?
    if(book.size()==MAX_PAGES)
      book.erase(book.begin());
    book.push_back(change_page_format(page));
    char_ind=0;
    burst_ind=0;
    page_ind=book.size()-1;
  }
}

uint8_t decode(char c){
  char letter=c;
  if(is_lower(c)){
    letter = to_upper(c);
  }
  switch(letter){
    case ' ': return 0b000000; case '!': return 0b011101; case '"': return 0b000010; case '#': return 0b001111;
    case '$': return 0b110101; case '%': return 0b100101; case '&': return 0b111101; case '\'': return 0b001000;
    case '(': return 0b111011; case ')': return 0b011111; case '*': return 0b100001; case '+': return 0b001101;
    case ',': return 0b000001; case '-': return 0b001001; case '.': return 0b000101; case '/': return 0b001100;
    case '0': return 0b001011; case '1': return 0b010000; case '2': return 0b011000; case '3': return 0b010010;
    case '4': return 0b100011; case '5': return 0b010001; case '6': return 0b011010; case '7': return 0b011011;
    case '8': return 0b011001; case '9': return 0b001010; case ':': return 0b100011; case ';': return 0b000011;
    case '<': return 0b110001; case '=': return 0b111111; case '>': return 0b001110; case '?': return 0b100111;
    case '@': return 0b000100; case 'A': return 0b100000; case 'B': return 0b110000; case 'C': return 0b100100;
    case 'D': return 0b100110; case 'E': return 0b100010; case 'F': return 0b110100; case 'G': return 0b110110; 
    case 'H': return 0b110010; case 'I': return 0b010100; case 'J': return 0b010110; case 'K': return 0b101000;
    case 'L': return 0b111000; case 'M': return 0b101100; case 'N': return 0b101110; case 'O': return 0b101010;
    case 'P': return 0b111100; case 'Q': return 0b111110; case 'R': return 0b111010; case 'S': return 0b011100;
    case 'T': return 0b011110; case 'U': return 0b101001; case 'V': return 0b111001; case 'W': return 0b010111;
    case 'X': return 0b101101; case 'Y': return 0b101111; case 'Z': return 0b101011; case '[': return 0b010101;
    case '\\': return 0b110011; case ']': return 0b110111; case '^': return 0b000110; case '_': return 0b000111;
    default: return 0b000000;
  }
}

void set_led(uint8_t c, vector<string>& book, string& curr_page, int& page_ind, int& burst_ind, int& char_ind){
  // conditions for beginning and end of page
  digitalWrite(last_page_pin, (page_ind==book.size()-1) ? HIGH : LOW);
  digitalWrite(last_burst_pin, last_burst(curr_page, burst_ind) ? HIGH : LOW);

  // conditions for characters
  digitalWrite(led5, (c & 0b1) == 1 ? HIGH : LOW);
  digitalWrite(led4, ((c>>1) & 0b1) == 1 ? HIGH : LOW);
  digitalWrite(led3, ((c>>2) & 0b1) == 1 ? HIGH : LOW);
  digitalWrite(led2, ((c>>3) & 0b1) == 1 ? HIGH : LOW);
  digitalWrite(led1, ((c>>4) & 0b1) == 1 ? HIGH : LOW);
  digitalWrite(led0, ((c>>5) & 0b1) == 1 ? HIGH : LOW);
}

string burst_from_page(string current_page, int burst_index){
  if(burst_index<current_page.length()/BURST_LEN){
    return current_page.substr(burst_index*BURST_LEN, BURST_LEN);  
  }else{
    string temp = current_page.substr(burst_index*BURST_LEN);
    for(unsigned i=temp.length(); i<BURST_LEN; i++){
      temp += " ";
    }
    return temp;
  }
}

string burst_to_braille(string burst){
  string _ba="";
  for(char ch : burst){
    _ba.append(char_to_braille(ch));
  }
  return _ba;
}

bool last_burst(string curr_page, int burst_ind){
  int highest = curr_page.length()/BURST_LEN;
  if(curr_page.length()%BURST_LEN==0){
    highest--;
  }

  if(burst_ind==highest) return true;
  else return false;
}


/* ========= HELPER FUNCTIONS ========= */

string change_page_format(vector<string> old_page){
  string _page;
  for(unsigned i=0; i<old_page.size(); i++){
    _page.append(old_page[i]);
    if(i!=old_page.size()-1)
      _page.append(" ");
  }
  return _page;
}

string char_to_braille(char chr){
  string _ba="";
  uint8_t byte = decode(chr);
  _ba.append(to_string((int)((byte & BIT5) >> 5)));
  _ba.append(to_string((int)((byte & BIT4) >> 4)));
  _ba.append(to_string((int)((byte & BIT3) >> 3)));
  _ba.append(to_string((int)((byte & BIT2) >> 2)));
  _ba.append(to_string((int)((byte & BIT1) >> 1)));
  _ba.append(to_string((int)(byte & BIT0)));
  return _ba;
}

void print_info(int page_ind, int burst_ind, int book_size, string curr_page, string curr_burst){
  int highest = curr_page.length()/BURST_LEN;
  if(curr_page.length()%BURST_LEN!=0){
    highest++;
  }
  
  string front(curr_page.substr(0,BURST_LEN*burst_ind));
  string back="";
  if(!last_burst(curr_page, burst_ind)){
    back =curr_page.substr(BURST_LEN*(burst_ind+1), curr_page.length()-1);
  }
  string info("LOCATION:\nPage: "+std::to_string(page_ind+1)+" / "+std::to_string(book_size)+"\n"+
    "Burst: "+std::to_string(burst_ind+1)+" / "+std::to_string(highest)+"\n"+
    "CURRENT PLACE: "+front+" '"+curr_burst+"' "+back+"\n\n");
  Serial.println(info.c_str());
}

char to_upper(char c){
  return (char)(c-32);
}

bool is_lower(char c){
  if(c>=97 && c<=122) return true;
  return false;
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

/* =========== SPI CODE =========== */
static void spi_init() {
    esp_err_t ret;
    spi_bus_config_t buscfg={
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = -1,
        .sclk_io_num = CLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
    spi_device_interface_config_t devcfg={
        .mode = 0,                  //SPI mode 0
        .clock_speed_hz = 1000000,  // 1 MHz
        .spics_io_num = CS_PIN,     
        .flags = SPI_DEVICE_HALFDUPLEX,
        .queue_size = 1,
        .pre_cb = NULL,
        .post_cb = NULL,
    };
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi2));
};

static void write_byte(uint8_t data) {
    uint8_t tx_data[1] = { data };
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = tx_data,
    };
    ESP_ERROR_CHECK(spi_device_polling_transmit(spi2, &t));
    vTaskDelay(1);
}