
#include <SPI.h>


#include <SdFat.h>
#include <SdFatUtil.h>



#include <SFEMP3Shield.h>

#if defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_Timer1
  #include <TimerOne.h>
#elif defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer
  #include <SimpleTimer.h>
#endif

SdFat sd;


SFEMP3Shield MP3player;

void setup() {

  uint8_t result; 

  Serial.begin(115200);


  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();
  
}



uint8_t sel_music = 1;
uint8_t present_song;
uint8_t flag = 0;

void loop() {

  byte in;
  uint8_t input_length;
  char input[10];
// Below is only needed if not interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) \
    && ( (USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer) \
    ||   (USE_MP3_REFILL_MEANS == USE_MP3_Polled)      )

  MP3player.available();
#endif

  if(Serial.available()) {
    
    in = Serial.read();
    
    if(in == 'a'){    // a = playmusic ex) a223 -> track023.mp3 
     
      input_order((char*)&input);
     
      
      play_music(atoi(input));
    }else if(in == 'v'){    // 0~99   //vol 40 -> v240.   vol 1 -> v11.
      
      input_order((char*)&input);
      
      
   
    union twobyte mp3_vol; 
  

    //mp3_vol.byte[1] = 160-atoi(input)*160/100;
    mp3_vol.byte[1] = 100-atoi(input);
    
    MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]);
      
      
    }else if(in == 'd'){   // daum song next song -> ex) d223 -> track023.mp3 
      input_order((char*)&input);
      
  
      sel_music = atoi(input);
    
    }else if(in == 'x'){   //mp3 state if) stop -> x10, play -> x11, pause -> x12;
    
	getState();
   
      
      
    }else{
        parse_menu(in); // get command from serial input
    }
  }
  
 
    if( flag == 1){
      if(MP3player.isPlaying() == 0){
         
        //play_music(sel_music);   //이전곡이 끝나면 다음곡재생하는 부분.
      }
     
    }
  

  delay(1000);
	getTime(); 
}

uint32_t  millis_prv = millis();


 void play_music(uint8_t sel){
   
   uint8_t result; // result code from some function as to be tested at later time.
   
  char title[30]; // buffer to contain the extract the Title from the current filehandles
  char artist[30]; // buffer to contain the extract the artist name from the current filehandles
  char album[30]; // buffer to contain the extract the album name from the current filehandles

   
   #if USE_MULTIPLE_CARDS
    sd.chvol(); 
#endif
    //tell the MP3 Shield to play a track
    MP3player.stopTrack();
    result = MP3player.playTrack(sel);

    //check result, see readme for error codes.
    if(result != 0) {
 
    } else {
      
    
      millis_prv = millis();
      present_song = sel;
	getTrack();
        getState();
      flag = 1;

	
      MP3player.trackTitle((char*)&title);
      MP3player.trackArtist((char*)&artist);
      MP3player.trackAlbum((char*)&album);

   
    }
   
   
 }
 void getState(){
   
    char x[5];
    x[0] = 'x';
    x[1] = '1';
    x[2] = MP3player.isPlaying() + '0';
    x[3] = '\0';
    Serial.println(x);
    
}
 
 void getTime(){    // ex) 23sec -> t223,   241sec -> t3241, 3sec -> t13
  char t[10];
  uint8_t cur = 0; 
  uint8_t time;
  
  time = (millis() -  millis_prv)/1000;
  
  t[0] = 't';            
  
  if(time >= 100){
    t[1] = '3' ;
    t[2] = time/100 + '0';
    t[3] = (time/10)%10 + '0';
    t[4] = time%10 + '0';
    t[5] = '\0';
  }else if(time >= 10){
    t[1] = '2';
    t[2] = time/10 + '0';
    t[3] = time%10 + '0';
    t[4] = '\0'; 
  }else{
    t[1] = '1';
    t[2] = time + '0';
    t[3] = '\0'; 
  }
  
  Serial.println(t);  
 
 
 }
 
 void getTrack(){      //ex) track023 ->  s223
   char t[10];
  
    t[0] = 's';
  
  
   if(present_song >= 100){
    t[1] = '3' ;
    t[2] = present_song/100 + '0';
    t[3] = (present_song/10)%10 + '0';
    t[4] = present_song%10 + '0';
    t[5] = '\0';
  }else if(present_song >= 10){
    t[1] = '2';
    t[2] = present_song/10 + '0';
    t[3] = present_song%10 + '0';
    t[4] = '\0'; 
  }else{
    t[1] = '1';
    t[2] = present_song + '0';
    t[3] = '\0'; 
  }
   
   
   Serial.println(t);
   
   
 }
 
 void input_order(char* in){
   
   uint8_t len;
   
   len = Serial.read() - '0';
   
   
   for(int i=0; i< len; i++){
     
     in[i] = Serial.read();
   }
   
   in[len] = '\0';
   
 }
 
 
 
void parse_menu(byte key_command) {

  uint8_t result; // result code from some function as to be tested at later time.

  if(key_command == 's') {

    MP3player.stopTrack();
    flag = 0;

  } else if(key_command >= '1' && key_command <= '9') {
    //convert ascii numbers to real numbers
    key_command = key_command - 48;
	
    play_music(key_command);
    

  //if +/- to change volume
  }else if(key_command == 'p') {
    if( MP3player.getState() == playback) {
      MP3player.pauseMusic();
    
    } else if( MP3player.getState() == paused_playback) {
      MP3player.resumeMusic();
    
    }

	getState();

  } else if(key_command == 't') {
    
    getTime();
    
  } else if(key_command == 'n'){   //next song
    play_music(sel_music);
  }else if(key_command == 'w'){   //what song
      getTrack();
  }

}


