Keymapper v1.1
==

Keymapper is a portable tool to remap the Caps Lock key to Backspace without rebooting on Windows®.

Intended for Colemak enthusiasts roaming on QWERTY keyboards.
	
How it Works
--

Caps Lock keypresses are intercepted with a global keyboard hook (SetWindowsHookEx). A backspace keypress is then simulated in its place to improve touch typing using the keybd_event function.

Known Issues
--

Certain anti-virus heuristics may classify the executable as a potential threat due to the keylogging potential of a global keyboard hook. A more permanent way to remap your keyboard is using a registry tool like SharpKeys.