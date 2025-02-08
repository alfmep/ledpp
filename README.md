# A utility and C++ library to handle LEDs in a Linux system.

## led application

```shell
Usage: led [OPTIONS] [LED_NAME] [BRIGHTNESS [COLOR_INTENSITY ...]]
  List LEDs, modify or show LED brightness, color and trigger.
  If only argument LED_NAME is supplied, show the current and
  maximum brightness, trigger name, and color intensity values.
  If argument BRIGHTNESS is supplied, set the brightness value.
  If COLOR_INTENSITY arguments are supplied, set the color
  intensity value for each color.

  LED_NAME         The name of the LED to operate on.
  BRIGHTNESS       Set the brightness level to this value.
  COLOR_INTENSITY  Set the color intensity values.

Options:
  -l, --list             List available LEDs. This option ignores other arguments.
  -i, --info             Print detailed information about the LED.
  -c, --colors           Set only color values. This assumes all arguments after LED_NAME are color intensity values.
  -t, --trigger=TRIGGER  Set a trigger for the LED.
  -h, --help             Print this help message.
```

