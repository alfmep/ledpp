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
#include <led++.hpp>
#include <fstream>
#include <cstring>
#include <cerrno>
#include <filesystem>
#include <sstream>

#include <iostream>

namespace ledpp {


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    led::led (const std::string& name_arg)
        : led_name (name_arg)
    {
        if (led_name.empty())
            throw std::system_error (EINVAL, std::generic_category());

        // Make sure the LED name is just a name, and not an absolute or relative path
        std::filesystem::path device_pathname (led_name);
        if (device_pathname.parent_path().empty() == false)
            throw std::system_error (ENODEV, std::generic_category());

        device_pathname = "/sys/class/leds" / device_pathname;

        // Make sure the path to the led device exists
        if (!std::filesystem::exists(device_pathname)) {
            int errnum = errno==ENOENT ? ENODEV : errno;
            throw std::system_error (errnum, std::generic_category());
        }

        path_brightness = device_pathname / "brightness";
        path_max_brightness = device_pathname / "max_brightness";
        path_multi_intensity = device_pathname / "multi_intensity";
        path_trigger = device_pathname / "trigger";

        std::filesystem::path path_multi_index = device_pathname / "multi_index";
        if (std::filesystem::exists(path_multi_index)) {
            std::ifstream s (path_multi_index);
            while (s.good()) {
                std::string color_name;
                s >> color_name;
                if (!s.fail()) {
                    colors.emplace_back (color_name);
                }
                else if (!s.eof()) {
                    int errnum = errno;
                    throw std::system_error (errnum, std::generic_category());
                }
            }
        }
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int led::get_value (const std::filesystem::path& pathname)
    {
        int value = -1;
        std::ifstream s (pathname);
        if (s.good()) {
            s >> value;
            s.close ();
        }
        return s.fail() ? -1 : value;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int led::set_value (const std::filesystem::path& pathname, const std::string& value)
    {
        std::ofstream s (pathname);
        if (s.good()) {
            s << value;
            s.close ();
        }
        return s.fail() ? -1 : 0;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    std::vector<unsigned> led::color_intensity ()
    {
        std::vector<unsigned> ci;
        if (colors.empty())
            return ci;

        std::ifstream s (path_multi_intensity);
        while (s.good()) {
            unsigned value;
            s >> value;
            if (s.fail())
                return ci;
            ci.emplace_back (value);
        }
        return ci;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    int led::color_intensity (const std::vector<unsigned>& values)
    {
        std::stringstream ss;
        for (auto value : values)
            ss << ' ' << value;
        return set_value (path_multi_intensity, ss.str());
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    const std::set<std::string> led::triggers ()
    {
        std::set<std::string> trigger_set;

        std::ifstream s (path_trigger);
        while (s.good()) {
            std::string name;
            s >> name;
            if (!s.fail()) {
                size_t len = name.size ();
                if (len >= 3) {
                    if (name.front() == '['  &&  name.back() == ']')
                        name = name.substr (1, len-2);
                }
                trigger_set.emplace (name);
            }
        }
        return trigger_set;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    std::string led::trigger ()
    {
        std::string active_trigger;
        std::ifstream s (path_trigger);
        while (s.good()) {
            std::string name;
            s >> name;
            if (!s.fail()) {
                size_t len = name.size ();
                if (len >= 3) {
                    if (name.front() == '['  &&  name.back() == ']') {
                        active_trigger = name.substr (1, len-2);
                        break;
                    }
                }
            }
        }
        return active_trigger;
    }


    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    std::set<std::string> led::led_names ()
    {
        std::set<std::string> names;
        std::filesystem::directory_iterator dir_iter ("/sys/class/leds");
        for (auto& entry : dir_iter) {
            std::string name = entry.path().filename().string ();
            if (name.empty() == false)
                names.emplace (name);
        }
        return names;
    }


}
