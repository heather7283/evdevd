# evdevd
Daemon that listens to evdev events and spawn processes according to rules.

Usage:
```sh
evdevd DEVICE RULE...
```
Each RULE is a string of the form of `type:code:value:command`, where `type`,
`code` and `value` can be specified as `1,2,5-10,20` or `*` to match anything.
If a rule matches, `command` is executed as an argument to `sh -c` with its argv
set to event timestamp, type, code and value.

Example:
```sh
evdevd /dev/input/event0 '1:116:*:echo "$@"'
1791240905.45288 1 116 1
1791240905.219437 1 116 0
17912409068.986585 1 116 1
1791240969.172035 1 116 0
1791240909.859411 1 116 1
1791240910.4337 1 116 0
1791240911.109381 1 116 1
1791240911.267826 1 116 0
```

## TODO
- Support specifying codes as `EV_KEY`, `KEY_A`, etc

## License
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

## References
- https://www.kernel.org/doc/html/latest/input/input.html#evdev
- https://www.kernel.org/doc/html/latest/input/event-codes.html

