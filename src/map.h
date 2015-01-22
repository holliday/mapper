////////////////////////////////////////////////////////////////////////////////
// DEFINE INPUT AND OUTPUT DEVICES
////////////////////////////////////////////////////////////////////////////////
#ifdef DEFINE_DEVICE

////////////////////////////////////////////////////////////////////////////////
// Define input devices using the following macros:
//
// input_device(id, path);
// exclusive_device(id, path);
//
// Where 'id' is an arbitrary number assigned to the device.
// And 'path' is the path to the device, usually somewhere in /dev/input/...
// Use 'exclusive_device' macro to open the device in exclusive mode.
//
// Example:
//
// input_device(0, /dev/input/event1);
// input_device(7, /dev/input/event7);
// exclusive_device(42, /dev/input/by-id/usb-The_Ultimate_Answer);



////////////////////////////////////////////////////////////////////////////////
// Define output devices using the following macros:
//
// output_device(id, type);
//
// Where 'id' is an arbitrary number assigned to the device.
// And 'type' is one of the following: keyboard, mouse, joystick.
//
// Example:
//
// output_device( 0, keyboard);
// output_device(69, mouse);
// output_device(88, joystick);



#endif

////////////////////////////////////////////////////////////////////////////////
// DEFINE MAPPINGS
////////////////////////////////////////////////////////////////////////////////
#ifdef DEFINE_MAPPING
#endif
