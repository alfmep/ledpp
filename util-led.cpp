/*
 * Copyright (C) 2024,2025 Dan Arrhenius <dan@ultramarin.se>
 *
 * This file is part of led++.
 *
 * led++ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published
 * by the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <cerrno>
#include <unistd.h>
#include <getopt.h>
#include <led++.hpp>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;

#define FONT_NORMAL "\033[0m"
#define FONT_BOLD "\033[1m"


struct appargs_t {
    std::string led_name;
    std::string trigger;
    int brightness;
    std::vector<unsigned> colors;
    bool list;
    bool names_only;
    bool show_info;
    bool set_only_colors;

    appargs_t (int argc, char* argv[]);
    void print_usage ();
    void parse_arguments (int argc, char* argv[]);
    void print_lsled_usage ();
    void parse_lsled_arguments (int argc, char* argv[]);
};

static const char* fn_normal = "";
static const char* fn_bold = "";


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void appargs_t::print_usage ()
{
    cout << endl;
    cout << fn_bold << "Usage: " << program_invocation_short_name << " [OPTIONS] [LED_NAME] [BRIGHTNESS [COLOR_INTENSITY ...]]" << fn_normal << endl;
    cout << "  List LEDs, modify or show LED brightness, color and trigger." << endl;
    cout << "  If only argument LED_NAME is supplied, show the current and" << endl;
    cout << "  maximum brightness, trigger name, and color intensity values." << endl;
    cout << "  If argument BRIGHTNESS is supplied, set the brightness value." << endl;
    cout << "  If COLOR_INTENSITY arguments are supplied, set the color" << endl;
    cout << "  intensity value for each color." << endl;
    cout << endl;
    cout << fn_bold << "  LED_NAME         " << fn_normal << "The name of the LED to operate on." << endl;
    cout << fn_bold << "  BRIGHTNESS       " << fn_normal << "Set the brightness level to this value." << endl;
    cout << fn_bold << "  COLOR_INTENSITY  " << fn_normal << "Set the color intensity values." << endl;
    cout << endl;
    cout << fn_bold << "Options:" << fn_normal << endl;
    cout << "  -l, --list             List available LEDs. This option ignores other arguments." << endl;
    cout << "  -i, --info             Print detailed information about the LED." << endl;
    cout << "  -c, --colors           Set only color values. This assumes all arguments after LED_NAME are color intensity values." << endl;
    cout << "  -t, --trigger=TRIGGER  Set a trigger for the LED." << endl;
    cout << "  -h, --help             Print this help message." << endl;
    cout << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void appargs_t::parse_arguments (int argc, char* argv[])
{
    static struct option long_options[] = {
        { "list",    no_argument, 0, 'l'},
        { "info",    no_argument, 0, 'i'},
        { "colors",  no_argument, 0, 'c'},
        { "trigger", no_argument, 0, 't'},
        { "help",    no_argument, 0, 'h'},
        { 0, 0, 0, 0}
    };
    static const char* arg_format = "lict:h";

    while (1) {
        int c = getopt_long (argc, argv, arg_format, long_options, NULL);
        if (c == -1)
            break;
        switch (c) {
        case 'l':
            list = true;
            break;
        case 'i':
            show_info = true;
            break;
        case 'c':
            set_only_colors = true;
            break;
        case 't':
            trigger = optarg;
            break;
        case 'h':
            print_usage ();
            exit (0);
            break;
        default:
            cerr << "Use option -h for help." << endl;
            exit (1);
        }
    }

    if (list) {
        if (optind < argc) {
            cerr << "Error: Too many arguments, use option -h for help." << endl;
            exit (1);
        }
        return;
    }
    if (optind >= argc) {
        cerr << "Error: Missing arguments, use option -h for help." << endl;
        exit (1);
    }

    led_name = argv[optind++];

    if (show_info) {
        if (optind < argc) {
            cerr << "Error: Too many arguments, use option -h for help." << endl;
            exit (1);
        }
        return;
    }

    try {
        int val;
        if (optind < argc) {
            val = std::stoi (argv[optind++]);
            if (val < 0)
                throw val;
            if (set_only_colors)
                colors.emplace_back (val);
            else
                brightness = val;
        }
        while (optind < argc) {
            val = std::stoi (argv[optind++]);
            if (val < 0)
                throw val;
            colors.emplace_back (val);
        }
    }
    catch (...){
        cerr << "Error: Invalid argument." << endl;
        exit (1);
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void appargs_t::print_lsled_usage ()
{
    cout << endl;
    cout << fn_bold << "Usage: " << program_invocation_short_name << " [OPTIONS]" << fn_normal << endl;
    cout << "  List information about available LEDs." << endl;
    cout << endl;
    cout << fn_bold << "Options:" << fn_normal << endl;
    cout << "  -n, --names  Print only the names of the available LEDs." << endl;
    cout << "  -h, --help   Print this help message." << endl;
    cout << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void appargs_t::parse_lsled_arguments (int argc, char* argv[])
{
    static struct option long_options[] = {
        { "names", no_argument, 0, 'n'},
        { "help",  no_argument, 0, 'h'},
        { 0, 0, 0, 0}
    };
    static const char* arg_format = "nh";

    while (1) {
        int c = getopt_long (argc, argv, arg_format, long_options, NULL);
        if (c == -1)
            break;
        switch (c) {
        case 'n':
            names_only = true;
            break;
        case 'h':
            print_lsled_usage ();
            exit (0);
            break;
        default:
            cerr << "Use option -h for help." << endl;
            exit (1);
        }
    }
    if (optind < argc) {
        cerr << "Error: Too many arguments, use option -h for help." << endl;
        exit (1);
    }

    list = true;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
appargs_t::appargs_t (int argc, char* argv[])
    : brightness (-1),
      list (false),
      names_only (false),
      show_info (false),
      set_only_colors (false)
{
    if (std::string(program_invocation_short_name) == "lsled")
        parse_lsled_arguments (argc, argv);
    else
        parse_arguments (argc, argv);
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void add_field (const std::string& str, unsigned& max_len, std::vector<std::string>& str_list)
{
    if (str.size() > max_len)
        max_len = str.size ();
    str_list.emplace_back (str);
}


//------------------------------------------------------------------------------
// NOTE: The following is assumed: str.size() >= col_size
//------------------------------------------------------------------------------
static void show_field (const std::string& str, unsigned col_size, bool align_right=false)
{
    int num_spaces = col_size - str.size();
    if (!align_right)
        cout << str;
    for (int i=0; i<num_spaces; ++i)
        cout << ' ';
    if (align_right)
        cout << str;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void list_leds ()
{
    std::vector<std::string> led_names;
    std::vector<std::string> br_txt;
    std::vector<std::string> trigger_names;
    std::vector<std::string> colors;
    unsigned num_lines = 0;
    unsigned len_led_name = 0;
    unsigned len_br_txt = 0;
    unsigned len_trigger_names = 0;
    unsigned len_colors = 0;
    bool has_colors = false;

    add_field ("NAME", len_led_name, led_names);
    add_field ("CUR/MAX", len_br_txt, br_txt);
    add_field ("TRIGGER", len_trigger_names, trigger_names);
    add_field ("COLOR:VALUE[,COLOR:VALUE...]", len_colors, colors);
    ++num_lines;

    auto leds = ledpp::led::led_names ();
    for (auto& name : leds) {
        ledpp::led led (name);
        std::string txt;
        int value;

        // Name
        add_field (led.name(), len_led_name, led_names);

        // Brightness / Max brightness
        value = led.brightness ();
        if (value >= 0)
            txt = std::to_string (value);
        else
            txt = "-";
        txt.push_back ('/');
        value = led.max_brightness ();
        if (value >= 0)
            txt.append (std::to_string(value));
        else
            txt.push_back ('-');
        add_field (txt, len_br_txt, br_txt);

        // Trigger
        txt = led.trigger ();
        if (txt.empty())
            add_field ("-", len_trigger_names, trigger_names);
        else
            add_field (txt, len_trigger_names, trigger_names);

        // Colors
        auto c_names = led.color_names ();
        auto c_values = led.color_intensity ();
        txt.clear ();
        if (!c_names.empty() && c_names.size() == c_values.size()) {
            has_colors = true;
            for (unsigned c=0; c<c_names.size(); ++c) {
                if (c)
                    txt.push_back (',');
                txt.append (c_names[c]);
                txt.push_back (':');
                txt.append (std::to_string(c_values[c]));
            }
            add_field (txt, len_colors, colors);
        }else{
            if (c_names.size() != c_values.size())
                ; // WTF !?!
            add_field ("", len_colors, colors);
        }

        ++num_lines;
    }

    for (unsigned i=0; i<num_lines; ++i) {
        show_field (led_names[i], len_led_name);
        cout << ' ';
        show_field (br_txt[i], len_br_txt);

        if (!trigger_names[i].empty() || !colors[i].empty()) {
            if (len_trigger_names) {
                cout << ' ';
                if (colors[i].empty())
                    cout << trigger_names[i];
                else
                    show_field (trigger_names[i], len_trigger_names);
            }
            if (has_colors) {
                if (!colors[i].empty()) {
                    cout << ' ';
                    cout << colors[i];
                }
            }
        }
        cout << endl;
    }
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void print_led_info (appargs_t& opt)
{
    ledpp::led led (opt.led_name);

    std::string max_br_str = std::to_string (led.max_brightness());
    unsigned w = max_br_str.size ();

    cout << "Name          : " << led.name() << endl;
    cout << "Location      : " << "/sys/class/leds/" << led.name() << endl;
    cout << "Brightness    : " << std::setw(w) << led.brightness() << endl;
    cout << "Max brightness: " << max_br_str << endl;
    cout << "Multicolor    : ";
    if (led.is_multicolor()) {
        cout << "Yes" << endl;
        cout << "Color values  : ";
        auto& colors = led.color_names ();
        auto values = led.color_intensity ();
        if (colors.size() != values.size()) {
            cerr << "Error getting color values." << endl;
            exit (1);
        }
        for (size_t i=0; i<colors.size(); ++i) {
            if (i)
                cout << ' ';
            cout << colors[i] << ":" << values[i];
        }
        cout << endl;
    }else{
        cout << "No" << endl;
    }

    cout << "Triggers      : ";
    int i = 0;
    auto active_trigger = led.trigger ();
    for (auto& trigger : led.triggers()) {
        if (i++)
            cout << ' ';
        if (trigger == active_trigger)
            cout << '[' << fn_bold << trigger << fn_normal << ']';
        else
            cout << trigger;
    }
    cout << endl;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
int main (int argc, char* argv[])
{
    if (isatty(fileno(stdout))) {
        fn_normal = FONT_NORMAL;
        fn_bold = FONT_BOLD;
    }

    try {
        appargs_t opt (argc, argv);

        if (opt.list) {
            if (opt.names_only) {
                auto leds = ledpp::led::led_names ();
                for (auto& name : leds)
                    cout << name << endl;
            }else{
                list_leds ();
            }
            return 0;
        }
        if (opt.show_info) {
            print_led_info (opt);
            return 0;
        }

        ledpp::led led (opt.led_name);

        if (opt.brightness<0 && opt.colors.empty() && opt.trigger.empty()) {
            //
            // Show current LED status
            //
            cout << led.brightness() << '/' << led.max_brightness();
            if (led.is_multicolor()) {
                cout << '\t';
                auto cnames = led.color_names ();
                auto cvalue = led.color_intensity ();
                for (size_t i=0; i<cnames.size(); ++i) {
                    if (i)
                        cout << ',';
                    cout << cnames[i] << ':' << cvalue[i];
                }
            }
            auto t = led.trigger ();
            if (t != "none")
                cout << "\ttrigger:" << t;
            cout << endl;
            return 0;
        }

        if (!opt.trigger.empty()) {
            //
            // Set LED trigger
            //
            if (led.trigger(opt.trigger)) {
                int errnum = errno;
                throw std::system_error (errnum, std::generic_category());
            }
        }

        if (opt.brightness >= 0) {
            //
            // Set LED brightness
            //
            if (led.brightness(opt.brightness)) {
                int errnum = errno;
                throw std::system_error (errnum, std::generic_category());
            }
        }

        if (opt.colors.empty() == false) {
            //
            // Set intensity of LED colors
            //
            if (opt.colors.size() != led.color_names().size()) {
                cerr << "Error: Invalid number of color values" << endl;
                exit (1);
            }
            if (led.color_intensity(opt.colors)) {
                int errnum = errno;
                throw std::system_error (errnum, std::generic_category());
            }
        }
    }
    catch (std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        exit (1);
    }

    return 0;
}
