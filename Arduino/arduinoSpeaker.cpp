/**
 * \file MP3Shield_Library_Demo.ino
 *
 * \brief Example sketch of using the MP3Shield Arduino driver, demonstrating all methods and functions.
 * \remarks comments are implemented with Doxygen Markdown format
 *
 * \author Bill Porter
 * \author Michael P. Flaga
 *
 * This sketch listens for commands from a serial terminal (like the Serial
 * Monitor in the Arduino IDE). If it sees 1-9 it will try to play an MP3 file
 * named track00x.mp3 where x is a number from 1 to 9. For eaxmple, pressing
 * 2 will play 'track002.mp3'. A lowe case 's' will stop playing the mp3.
 * 'f' will play an MP3 by calling it by it's filename as opposed to a track
 * number.
 *
 * Sketch assumes you have MP3 files with filenames like "track001.mp3",
 * "track002.mp3", etc on an SD card loaded into the shield.
 */

#include <SPI.h>

//Add the SdFat Libraries
#include <SdFat.h>
#include <SdFatUtil.h>

//and the MP3 Shield Library
#include <SFEMP3Shield.h>

// Below is not needed if interrupt driven. Safe to remove if not using.
#if defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_Timer1
  #include <TimerOne.h>
#elif defined(USE_MP3_REFILL_MEANS) && USE_MP3_REFILL_MEANS == USE_MP3_SimpleTimer
  #include <SimpleTimer.h>
#endif

/**
 * \brief Object instancing the SdFat library.
 *
 * principal object for handling all SdCard functions.
 */
SdFat sd;

/**
 * \brief Object instancing the SFEMP3Shield library.
 *
 * principal object for handling all the attributes, members and functions for the library.
 */
SFEMP3Shield MP3player;

//------------------------------------------------------------------------------
/**
 * \brief Setup the Arduino Chip's feature for our use.
 *
 * After Arduino's kernel has booted initialize basic features for this
 * application, such as Serial port and MP3player objects with .begin.
 * Along with displaying the Help Menu.
 *
 * \note returned Error codes are typically passed up from MP3player.
 * Whicn in turns creates and initializes the SdCard objects.
 *
 * \see
 * \ref Error_Codes
 */
void setup() {

  uint8_t result; //result code from some function as to be tested at later time.

  Serial.begin(115200);
/*
  Serial.print(F("F_CPU = "));
  Serial.println(F_CPU);
  Serial.print(F("Free RAM = ")); // available in Version 1.0 F() bases the string to into Flash, to use less SRAM.
  Serial.print(FreeRam(), DEC);  // FreeRam() is provided by SdFatUtil.h
  Serial.println(F(" Should be a base line of 1028, on ATmega328 when using INTx"));
*/

  //Initialize the SdCard.
  if(!sd.begin(SD_SEL, SPI_FULL_SPEED)) sd.initErrorHalt();
  // depending upon your SdCard environment, SPI_HAVE_SPEED may work better.
  if(!sd.chdir("/")) sd.errorHalt("sd.chdir");

  //Initialize the MP3 Player Shield
  result = MP3player.begin();
  //check result, see readme for error codes.
  /*
  if(result != 0) {
    Serial.print(F("Error code: "));
    Serial.print(result);
    Serial.println(F(" when trying to start MP3 player"));
    if( result == 6 ) {
      Serial.println(F("Warning: patch file not found, skipping.")); // can be removed for space, if needed.
      Serial.println(F("Use the \"d\" command to verify SdCard can be read")); // can be removed for space, if needed.
    }
  }
  */

/*
#if defined(__BIOFEEDBACK_MEGA__) // or other reasons, of your choosing.
  // Typically not used by most shields, hence commented out.
  Serial.println(F("Applying ADMixer patch."));
  if(MP3player.ADMixerLoad("admxster.053") == 0) {
    Serial.println(F("Setting ADMixer Volume."));
    MP3player.ADMixerVol(-3);
  }
#endif
*/
  //help();
}



uint8_t sel_music = 1;
uint8_t present_song;
uint8_t flag = 0;
//------------------------------------------------------------------------------
/**
 * \brief Main Loop the Arduino Chip
 *
 * This is called at the end of Arduino kernel's main loop before recycling.
 * And is where the user's serial input of bytes are read and analyzed by
 * parsed_menu.
 *
 * Additionally, if the means of refilling is not interrupt based then the
 * MP3player object is serviced with the availaible function.
 *
 * \note Actual examples of the libraries public functions are implemented in
 * the parse_menu() function.
 */
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
     
     // Serial.println("v in?");
    
      input_order((char*)&input);
      
      Serial.println(input);
      
      play_music(atoi(input));
    }else if(in == 'v'){    // 0~99   //vol 40 -> v240.   vol 1 -> v11.
      
      input_order((char*)&input);
      
      //Serial.println(input);
   
    union twobyte mp3_vol; // create key_command existing variable that can be both word and double byte of left and right.
    mp3_vol.word = MP3player.getVolume(); // returns a double uint8_t of Left and Right packed into int16_t

    mp3_vol.byte[1] = 160-atoi(input)*160/100;
    MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]); // commit new volume
      
      
    }else if(in == 'd'){   // daum song next song -> ex) d223 -> track023.mp3 
      input_order((char*)&input);
      
     // Serial.println(input);
      
      sel_music = atoi(input);
    
    }else if(in == 'x'){   //mp3 state if) stop -> x0, play -> x1, pause -> x2;
    
    char x[5];
    x[0] = 'x';
    x[1] = '1';
    x[2] = MP3player.isPlaying() + '0';
    x[3] = '\0';
     Serial.println(x);
      
      
    }else{
        parse_menu(in); // get command from serial input
    }
  }
  
 
    if( flag == 1){
      if(MP3player.isPlaying() == 0){
         
        play_music(sel_music);
      }
     
    }
  

  delay(100);
}

uint32_t  millis_prv = millis();

//------------------------------------------------------------------------------
/**
 * \brief Decode the Menu.
 *
 * Parses through the characters of the users input, executing corresponding
 * MP3player library functions and features then displaying a brief menu and
 * prompting for next input command.
 */
 
 void play_music(uint8_t sel){
   
   uint8_t result; // result code from some function as to be tested at later time.
   
  char title[30]; // buffer to contain the extract the Title from the current filehandles
  char artist[30]; // buffer to contain the extract the artist name from the current filehandles
  char album[30]; // buffer to contain the extract the album name from the current filehandles

   
   #if USE_MULTIPLE_CARDS
    sd.chvol(); // assign desired sdcard's volume.
#endif
    //tell the MP3 Shield to play a track
    MP3player.stopTrack();
    result = MP3player.playTrack(sel);

    //check result, see readme for error codes.
    if(result != 0) {
     
//      Serial.print(result);
//      Serial.println(F(" when trying to play track"));
    } else {
      
    
      millis_prv = millis();
      present_song = sel;
      flag = 1;
//      Serial.println(F("Playing:"));

      //we can get track info by using thv240e following functions and arguments
      //the functions will extract the requested information, and put it in the array we pass in
      MP3player.trackTitle((char*)&title);
      MP3player.trackArtist((char*)&artist);
      MP3player.trackAlbum((char*)&album);

/*      //print out the arrays of track information
      Serial.write((byte*)&title, 30);
      Serial.println();
      Serial.print(F("by:  "));
      Serial.write((byte*)&artist, 30);
      Serial.println();
      Serial.print(F("Album:  "));
      Serial.write((byte*)&album, 30);
      Serial.println();
*/
   
    }
   
   
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

  // Note these buffer may be desired to exist globably.
  // but do take much space if only needed temporarily, hence they are here.

//  Serial.print(F("Received command: "));
//  Serial.write(key_command);
//  Serial.println(F(" "));

  //if s, stop the current track
  if(key_command == 's') {
//    Serial.println(F("Stopping"));
    MP3player.stopTrack();
    flag = 0;

  //if 1-9, play corresponding track
  } else if(key_command >= '1' && key_command <= '9') {
    //convert ascii numbers to real numbers
    key_command = key_command - 48;

    play_music(key_command);
    

  //if +/- to change volume
  } else if((key_command == '-') || (key_command == '+')) {
    union twobyte mp3_vol; // create key_command existing variable that can be both word and double byte of left and right.
    mp3_vol.word = MP3player.getVolume(); // returns a double uint8_t of Left and Right packed into int16_t

    if(key_command == '-') { // note dB is negative
      // assume equal balance and use byte[1] for math
      if(mp3_vol.byte[1] >= 254) { // range check
        mp3_vol.byte[1] = 254;
      } else {
        mp3_vol.byte[1] += 2; // keep it simpler with whole dB's
      }
    } else {
      if(mp3_vol.byte[1] <= 2) { // range check
        mp3_vol.byte[1] = 2;
      } else {
        mp3_vol.byte[1] -= 2;
      }
    }
    // push byte[1] into both left and right assuming equal balance.
    MP3player.setVolume(mp3_vol.byte[1], mp3_vol.byte[1]); // commit new volume
//    Serial.print(F("Volume changed to -"));
//    Serial.print(mp3_vol.byte[1]>>1, 1);
//    Serial.println(F("[dB]"));

  //if < or > to change Play Speed
  } else if(key_command == 'p') {
    if( MP3player.getState() == playback) {
      MP3player.pauseMusic();
      //Serial.println(F("Pausing"));
    } else if( MP3player.getState() == paused_playback) {
      MP3player.resumeMusic();
      //Serial.println(F("Resuming"));
    }

  } else if(key_command == 't') {
    
    getTime();
    
  } else if(key_command == 'n'){   //next song
    play_music(sel_music);
  }else if(key_command == 'w'){   //what song
      getTrack();
  }

}

//------------------------------------------------------------------------------
/**
 * \brief Print Help Menu.
 *
 * Prints a full menu of the commands available along with descriptions.
 */
 /*
void help() {
  Serial.println(F("Arduino SFEMP3Shield Library Example:"));
  Serial.println(F(" courtesy of Bill Porter & Michael P. Flaga"));
  Serial.println(F("COMMANDS:"));
  Serial.println(F(" [1-9] to play a track"));
  Serial.println(F(" [f] play track001.mp3 by filename example"));
  Serial.println(F(" [F] same as [f] but with initial skip of 2 second"));
  Serial.println(F(" [s] to stop playing"));
  Serial.println(F(" [d] display directory of SdCard"));
  Serial.println(F(" [+ or -] to change volume"));
  Serial.println(F(" [> or <] to increment or decrement play speed by 1 factor"));
  Serial.println(F(" [i] retrieve current audio information (partial list)"));
  Serial.println(F(" [p] to pause."));
  Serial.println(F(" [t] to toggle sine wave test"));
  Serial.println(F(" [S] Show State of Device."));
  Serial.println(F(" [b] Play a MIDI File Beep"));
#if !defined(__AVR_ATmega32U4__)
  Serial.println(F(" [e] increment Spatial EarSpeaker, default is 0, wraps after 4"));
  Serial.println(F(" [m] perform memory test. reset is needed after to recover."));
  Serial.println(F(" [M] Toggle between Mono and Stereo Output."));
  Serial.println(F(" [g] Skip to a predetermined offset of ms in current track."));
  Serial.println(F(" [k] Skip a predetermined number of ms in current track."));
  Serial.println(F(" [r] resumes play from 2s from begin of file"));
  Serial.println(F(" [R] Resets and initializes VS10xx chip."));
  Serial.println(F(" [O] turns OFF the VS10xx into low power reset."));
  Serial.println(F(" [o] turns ON the VS10xx out of low power reset."));
  Serial.println(F(" [D] to toggle SM_DIFF between inphase and differential output"));
  Serial.println(F(" [V] Enable VU meter Test."));
  Serial.println(F(" [B] Increament bass frequency by 10Hz"));
  Serial.println(F(" [C] Increament bass amplitude by 1dB"));
  Serial.println(F(" [T] Increament treble frequency by 1000Hz"));
  Serial.println(F(" [E] Increament treble amplitude by 1dB"));
#endif
  Serial.println(F(" [h] this help"));
}
*/

