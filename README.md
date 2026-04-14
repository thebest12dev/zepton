> [!WARNING]
> Zepton Macro is still in **very** early development, so don't expect it to run at all!

<div align="center">
  <img src="resources/zepton.png" width=200>

  ### Zepton Macro
  An open-source Bee Swarm Simulator macro NOT written in AutoHotkey!
</div>



Zepton Macro is a cross-platform and intuitive Bee Swarm Simulator macro that aims to finally allow Linux, macOS and iOS users to macro under one single codebase. 
Supports Windows as of right now, with plans to bring it to
Linux, macOS, Android and iOS.

## Installation
### Windows
Download `zepton_x.y.z_windows` from the Releases page, then simply extract and run!
### Linux
> [!NOTE]
> Zepton Macro supports KDE (X11 and Wayland) and GNOME (X11-only). Anything else may not work due to incompatibilities!

> [!WARNING]
> Zepton Macro will NOT work on GNOME (Wayland) due to restrictions with OCR. If you still want to proceed, recompile with `ZEPTON_PLATFORM_SUPPORTS_OCR=false`

1. Download `zepton_x.y.z_linux` from the Releases page.
2. Install these dependencies:
```
sudo apt install scrot ydotool ydotoold xdotool
```
3. Configure the ydotool daemon
   - You should add the line below to autostart
   - ydotoold is needed for performance reasons
```
sudo ydotoold --socket-path="/run/user/1000/.ydotool_socket" --socket-own="1000:1000"
```
4. Extract and run!
   - You could optionally run it directly from the CLI by creating `zepton.config` and passing that in the `--config`/`-c` flag.



## Contributing
Zepton Macro is open-source, and well open source projects got to have some contribution.
We greatly appreciate every contribution, so don't hesitate on sending bugs, suggestions or patches!
 - Bugs: Create a bug report and we will help you as soon as possible. If it's a security vulnerability, please read our security policy.
 - Features: If you'd like a feature in this project, submit a feature request.
 - Development: Want to submit patches or bugs? Go on! We value all pull requests for their intended purpose.

## Support and Community
Unfortunately, we **do** not have a discord server. Once this project gains enough traction, we may consider
having a dedicated server for the community.

## Credits
Well, it's blank other than @thebest12dev...

## License
This project is licensed under the GNU GPLv3. Head to license.md for the full license, and attribution.md for attribution.