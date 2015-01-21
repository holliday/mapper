////////////////////////////////////////////////////////////////////////////////
#include "enum.hpp"
#include "errno_error.hpp"
#include "input.hpp"
#include "storage/file.hpp"
#include "uinput.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <linux/input.h>

using namespace app;

////////////////////////////////////////////////////////////////////////////////
#define VERSION_MAJOR 0
#define VERSION_MINOR 0
#define VERSION_MICRO 0

constexpr char _n = '\n';

typedef std::vector<input> input_devices;
typedef std::vector<uinput> output_devices;

////////////////////////////////////////////////////////////////////////////////
input_devices inputs;
output_devices outputs;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void add_input_device(int number, const std::string& path, bool exclusive = false)
{
    for(const app::input& input: inputs)
        if(input.number() == number)
    throw std::invalid_argument("Duplicate input device " + std::to_string(number));

    std::cout << "Adding input device " << number << " - " << path << _n;
    inputs.emplace_back(number, path, exclusive);
}

////////////////////////////////////////////////////////////////////////////////
void add_output_device(int number, std::string&& name, const app::events& events)
{
    for(const app::uinput& output: outputs)
        if(output.number() == number)
    throw std::invalid_argument("Duplicate output device " + std::to_string(number));

    std::cout << "Adding output device " << number << _n;
    outputs.emplace_back(number, std::move(name), events);
}

////////////////////////////////////////////////////////////////////////////////
app::events KEYBOARD
{
    { EV_KEY, range(KEY_ESC, KEY_UNKNOWN) }
};

////////////////////////////////////////////////////////////////////////////////
app::events MOUSE
{
    { EV_KEY, range(BTN_MISC, BTN_TASK) },
    { EV_REL, range(REL_X, REL_MAX) },
};

////////////////////////////////////////////////////////////////////////////////
app::events JOYSTICK
{
    { EV_KEY, range(BTN_JOYSTICK, BTN_DEAD) },
    { EV_ABS, range(ABS_X, ABS_MAX) },
};

////////////////////////////////////////////////////////////////////////////////
#define ADD_INPUT_DEVICE(number, path) add_input_device(number, #path)
#define ADD_EXCLUSIVE_DEVICE(number, path) add_input_device(number, #path, true)

#define ADD_OUTPUT_KEYBOARD(number) add_output_device(number, "event", KEYBOARD)
#define ADD_OUTPUT_MOUSE(number)    add_output_device(number, "mouse", MOUSE)
#define ADD_OUTPUT_JOYSTICK(number) add_output_device(number, "js", JOYSTICK)

////////////////////////////////////////////////////////////////////////////////
int main(int , char* [])
{
    try
    {
        // define input and output devices
        #define DEFINE_DEVICE
        #include "map.h"
        #undef DEFINE_DEVICE

        // open input devices
        for(app::input& input: inputs)
        {
            std::cout << "Opening input device " << input.number() << _n;
            input.open();
        }

        // open and initialize output devices
        for(app::uinput& output: outputs)
        {
            std::cout << "Opening output device " << output.number() << _n;
            output.open();
        }

        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << _n;
        return 1;
    }
}
