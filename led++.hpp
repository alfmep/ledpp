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
#ifndef LEDPP_LED_HPP
#define LEDPP_LED_HPP

#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <system_error>


/**
 * Namespace for library led++.
 */
namespace ledpp {


    /**
     * Class to interface with LED devices in Linux.
     */
    class led {
    public:
        /**
         * Create an object to interface with a LED device.
         * @param name_arg The name of the LED.
         * @throw std::system_error If the LED doesn't exist,
         *                          or can't be accessed.
         */
        led (const std::string& name_arg);

        /**
         * Return the name of the LED.
         * @return The name of the LED.
         */
        const std::string& name () const {
            return led_name;
        }

        /**
         * Get the maximum brightness value of the LED.
         * @return A maximum brightness value, or -1 on error.
         *         On error, <code>errno</code> is set.
         */
        int max_brightness () {
            return get_value (path_max_brightness);
        }

        /**
         * Get the current brightness value of the LED.
         * @return The current brightness value, or -1 on error.
         *         On error, <code>errno</code> is set.
         */
        int brightness () {
            return get_value (path_brightness);
        }

        /**
         * Set the current brightness value of the LED.
         * @param value The new brightness value.
         * @return On success: 0. On failure: -1 and
         *         <code>errno</code> is set.
         */
        int brightness (unsigned value) {
            return set_value (path_brightness, std::to_string(value));
        }

        /**
         * Check if this is a multicolor LED.
         * @return <code>true</code> if this is a multicolor LED,
         *         <code>false</code> if not.
         */
        bool is_multicolor () const {
            return !colors.empty();
        }

        /**
         * Return a vector of color names if this is a multicolor LED.
         * @return A vector of color names. The vector will be
         *         empty if this isn't a multicolor LED.
         */
        const std::vector<std::string>& color_names () const {
            return colors;
        }

        /**
         * Return the intensity of each color of a multicolor LED.
         * The brightness of each individual color is calculated using the
         * formula:<br/>
         * <code>color_brightness = brightness * color_intensity/max_brightness</code>
         *
         * @return A vector of color intensity values. The vector will be
         *         empty if this isn't a multicolor LED,
         *         or if the values can't be read.
         */
        std::vector<unsigned> color_intensity ();

        /**
         * Set the intensity of each individual color for a multicolor LED.
         * The brightness of each individual color is calculated using the
         * formula:<br/>
         * <code>color_brightness = brightness * color_intensity/max_brightness</code>
         *
         * @param values A vector of intensity values for each individual color.
         *               The number of intensity values must match the number
         *               of colors this LED has.
         * @return 0 on success. -1 on error, and <code>errno</code> is set.
         */
        int color_intensity (const std::vector<unsigned>& values);

        /**
         * Return the available triggers for the LED.
         * @return A set of strings containing the names of
         *         the available triggers for this LED.
         */
        const std::set<std::string> triggers ();

        /**
         * Return the name of the current trigger for this LED.
         * @return A trigger name.
         */
        std::string trigger ();

        /**
         * Set a trigger for this LED.
         * @param name The name of the trigger.
         * @return 0 on success, -1 on error.
         */
        int trigger (const std::string& name) {
            return set_value (path_trigger, name);
        }

        /**
         * Get a list of available led devices in the system.
         * @return A list of led names.
         */
        static std::set<std::string> led_names ();


    private:
        std::string led_name;
        std::vector<std::string> colors;
        std::filesystem::path path_brightness;
        std::filesystem::path path_max_brightness;
        std::filesystem::path path_multi_intensity;
        std::filesystem::path path_trigger;

        static int get_value (const std::filesystem::path& pathname);
        static int set_value (const std::filesystem::path& pathname, const std::string& value);
    };


}
#endif
