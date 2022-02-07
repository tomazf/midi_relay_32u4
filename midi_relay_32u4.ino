/*****************
   RELAY box - output only based on sending note
             - made for SLO CuePilot event january/february 2022
   @Ferbi v0.1
      - added LED indication
      - updated HW - Arduino Micro 32u4
*/

#include <MIDIUSB.h>      // for Atmega32u4 - Micro, Pro Micro, Leonardo...
#include <Streaming.h>

#define relay_1 9         // relay pins
#define relay_2 8
#define relay_3 7
#define relay_4 6

//#define sendACKs                    // if we want to send ACK back
//#define SERIAL_DMP

bool state = false;

// MIDI setup
uint8_t midi_channel = 1;
uint8_t num_buttons = 4;
uint8_t midi_note_offset = 64;      // for sending note - set to FIRST note
uint8_t midi_relay_offset = 1;      // phy offset relative to note in HEX (64 = 40hex -> so we add 1 to get first relay)
#ifdef sendACKs
uint8_t midi_note_ack = 48;         // for sending and receiving - button
#endif
//uint8_t midi_vel_on = 127;        // if velocity control is used
//uint8_t midi_vel_off = 0;

//en kanal, različne note za gumbe, tip note določi stanje LEDice
//CH1, note 64, noteON = rele_1_ON, noteON = rele_1_OFF
//CH2, note 65, noteON = rele_2_ON, noteON = rele_2_OFF
//CH3, note 66, noteON = rele_3_ON, noteON = rele_3_OFF
//CH4, note 67, noteON = rele_4_ON, noteON = rele_4_OFF


void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void setup() {

#ifdef SERIAL_DMP
  // local serial setup
  Serial.begin(115200);
  while (!Serial);
#endif

  //pin setup
  pinMode(relay_1, OUTPUT);     // sets the digital pin as output
  pinMode(relay_2, OUTPUT);     // sets the digital pin as output
  pinMode(relay_3, OUTPUT);     // sets the digital pin as output
  pinMode(relay_4, OUTPUT);     // sets the digital pin as output

  // reset relay states
  digitalWrite(relay_1, LOW);
  digitalWrite(relay_2, LOW);
  digitalWrite(relay_3, LOW);
  digitalWrite(relay_4, LOW);

  delay(200);
  
#ifdef SERIAL_DMP
  Serial.println();                                 // just debug print after startup on seperate serial
  Serial.println("*************************");
  Serial.println("***   4 CH relay BOX  ***");
  Serial.println("*** with MIDI control ***");
  Serial.println("*** ----------------- ***");
  Serial << "***  receiving CH:  " << midi_channel << " ***\r\n";
  Serial << "*** starting note: " << midi_note_offset << " ***\r\n";
  Serial.println("*** ----------------- ***");
  Serial.println("***   Arduino ready   ***");
  Serial.println("*************************");
#endif
}

void loop() {

  // read incoming MIDI data
  midiEventPacket_t rx;

  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {

      //Serial.print("Received: ");
      //Serial.print(rx.header, HEX);
      //Serial.print("-");

      if (rx.header == 0x09) {                            // noteOn
        HandleNote(rx.byte1, rx.byte2, rx.byte3, true);
      }
      else if (rx.header == 0x08) {                       // noteOff
        HandleNote(rx.byte1, rx.byte2, rx.byte3, false);
      }
    }
  } while (rx.header != 0);
}

void HandleNote(byte channel, byte pitch, byte velocity, bool vrednost)
{
  if (vrednost) channel = channel - 0x90 + 1;        // to get only channel number wihout offset
  else channel = channel - 0x80 + 1;

  if (channel == midi_channel) {                                                          // check channel
    if ( (pitch >= midi_note_offset) && (pitch < midi_note_offset + num_buttons) ) {      // check note

      /*
        if (velocity == midi_vel_on) state = true;
        else if (velocity == midi_vel_off) state = false;
        else return;
      */

#ifdef SERIAL_DMP
      Serial << "note " << pitch << "/" << (vrednost ? 1 : 0);
#endif
      change_relay((pitch & 0x0F) + midi_relay_offset, vrednost);

      //Serial.print(" **** ch: ");
      //Serial.print(channel);
      //Serial.print("-");
      //Serial.print(pitch);
      //Serial.print("-");
      //Serial.println(velocity);
    }
  }
}

void change_relay(int relay, bool mode)
{
#ifdef SERIAL_DMP
  Serial << ": relay " << relay << "/" << mode;
  Serial.println();
#endif

  switch (relay)
  {
    case 1: digitalWrite(relay_1, mode ? 1 : 0); break;
    case 2: digitalWrite(relay_2, mode ? 1 : 0); break;
    case 3: digitalWrite(relay_3, mode ? 1 : 0); break;
    case 4: digitalWrite(relay_4, mode ? 1 : 0); break;
  }
  
#ifdef sendACKs
  sendACK(relay);
#endif
}

#ifdef sendACKs
void sendACK(uint8_t pin)
{
#ifdef SERIAL_DMP
  Serial.println("Sending ACK");
#endif
  noteOn(midi_channel, midi_note_ack, pin);              // Channel 4, middle C, normal velocity
  MidiUSB.flush();
}
#endif
