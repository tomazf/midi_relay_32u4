# midi_relay_32u4 - MIDI GPO box

4 channel isolated MIDI GPO box with LED indicators

Basic description:
  - made with native MIDIUSB.h library
  - HW GPO is isolated, meaning relay outputs are used
  - relays are low-current - USB power supply is adequate
  - driver is simple BC547 NPN switch

Default setup:
  - midi_channel = 1
  - midi_note_offset = 64 (DEC)
  
Control:
  - send noteON with your SW to MIDI device -> action: relay_ON
  - send noteOFF with your SW to MIDI device -> action: relay_OFF

Notes: 
   - MIDI notes used, are numbers from (midi_note_offset + 3) -> 64, 65, 66, 67
   - Serial out is not used (if you need to DEBUG, then te port must be opened, otherwise MIDI won't work!)
   - you can enable ACK's if the functionality is needed
   - some code is commented out - for velocity etc.
