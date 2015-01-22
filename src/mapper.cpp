////////////////////////////////////////////////////////////////////////////////
#include "enum.hpp"
#include "errno_error.hpp"
#include "input.hpp"
#include "storage/file.hpp"
#include "uinput.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <linux/input.h>
#include <poll.h>
#include <signal.h>

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
void create_input(int number, const std::string& path, bool exclusive = false)
{
    for(const app::input& input: inputs)
        if(input.number() == number)
    throw std::invalid_argument("Duplicate input device " + std::to_string(number));

    std::cout << "Adding input device " << number << " - " << path << _n;
    inputs.emplace_back(number, path, exclusive);
}

////////////////////////////////////////////////////////////////////////////////
void create_output(int number, std::string&& name, const app::events& events)
{
    for(const app::uinput& output: outputs)
        if(output.number() == number)
    throw std::invalid_argument("Duplicate output device " + std::to_string(number));

    std::cout << "Adding output device " << number << _n;
    outputs.emplace_back(number, std::move(name), events);
}

////////////////////////////////////////////////////////////////////////////////
const app::events keyboard
{
    { EV_KEY, range(KEY_ESC, KEY_UNKNOWN) }
};

////////////////////////////////////////////////////////////////////////////////
const app::events mouse
{
    { EV_KEY, range(BTN_MISC, BTN_TASK) },
    { EV_REL, range(REL_X, REL_MAX) },
};

////////////////////////////////////////////////////////////////////////////////
const app::events joystick
{
    { EV_KEY, range(BTN_JOYSTICK, BTN_DEAD) },
    { EV_ABS, range(ABS_X, ABS_MAX) },
};

////////////////////////////////////////////////////////////////////////////////
#define input_device(number, path) create_input(number, #path)
#define exclusive_device(number, path) create_input(number, #path, true)

#define output_device(number, type) create_output(number, #type, type)

////////////////////////////////////////////////////////////////////////////////
int main(int , char* [])
{
    try
    {
        // define input and output devices
        #define DEFINE_DEVICE
        #include "map.h"
        #undef DEFINE_DEVICE

        std::vector<pollfd> desc;
        desc.reserve(inputs.size());

        // open input devices
        std::cout << "Opening input devices:";
        for(app::input& input: inputs)
        {
            std::cout << ' ' << input.number();
            input.open();

            desc.emplace_back(pollfd { .fd = input.get_id(), .events = POLL_IN });
        }
        std::cout << _n;

        // open and initialize output devices
        std::cout << "Opening output devices:";
        for(app::uinput& output: outputs)
        {
            std::cout << ' ' << output.number();
            output.open();
        }
        std::cout << _n;

        input_event event;

        // main loop
        for(;;)
        {
            int n = poll(&desc[0], desc.size(), -1);
            if(n < 0)
            {
                if(errno == EINTR)
                    break;
                else throw errno_error();
            }

            for(int ri = 0; n > 0 && ri < desc.size(); ++ri)
            {
                if(desc[ri].revents & POLL_IN)
                {
                    auto read = inputs[ri].read(&event, sizeof(event));
                        if(read != sizeof(event))
                    throw std::runtime_error("Short read from device " + std::to_string(inputs[ri].number()));

                    --n;
                }
            }
            if(n > 0) throw std::runtime_error("Didn't process all incoming data");
        }

        return 0;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << _n;
        return 1;
    }
}
