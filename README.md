# Synopsis

    xdo ACTION [OPTIONS] [WID ...]

# Description

Apply the given action to the given windows.

If no window IDs and no options are given, the action applies to the focused window.

# Actions

- `close`: close the window.

- `kill`: kill the client.

- `hide`: unmap the window.

- `show`: map the window.

- `activate`: activate (via EWMH) the window.

- `id`: print the window's ID.

- `pid`: print the window's pid.

- `key`: simulate a key press/release event.

- `button`: simulate a button press/release event.

- `-h`: print the synopsis and exit.

- `-v`: print the version and exit.

# Options

When options are provided, the action applies to all the children of the root window that match the comparisons implied by the options in relation to the focused window.

- `-r`: distinct ID.

- `-c`: same class.

- `-C`: distinct class.

- `-d`: same desktop.

- `-D`: distinct desktop.

- `-k CODE`: use the given code for the `key` and `button` actions.

# Examples

Close the focused window:

    xdo close

Close all the windows having the same class as the focused window:

    xdo close -c

Hide all the windows of the current desktop except the focused window:

    xdo hide -dr

Activate the window which ID is 0x800109:

    xdo activate 0x800109

Send fake key press/release events with keycode 24 to the focused window:

    xdo key -k 24
