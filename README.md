# BtScreenLocker
This daemon looks for your trusted Bluetooth devices whether or not they're near, and if they aren't, lock your screen to avoid unauthorized access to your computer.

# Building and Installing
First of all, make sure you have all the required dependencies installed on your system. They are:
| Dependency  |  Modules                           |
|-------------|------------------------------------|
| Qt6         | Bluetooth, DBus, Linguist, Widgets |

## Installing Dependencies
Here is a non-exhaustive table of how you can install the dependencies depending on your Linux distribution:

**Make sure you run the corresponding command as super user.**
| Distribution              | Command                                                              |
|---------------------------|----------------------------------------------------------------------|
| Arch Linux                | pacman -S qt6-base qt6-connectivity qt6-tools                        |
| Debian, Ubuntu and alike  | apt install libqt6bluetooth6 libqt6dbus6 linguist-qt6 libqt6widgets6 |
| Fedora                    | dnf install qt6-qtbase-devel qt6-qtconnectivity-devel qt6-linguist   |
| Gentoo                    | emerge qtbase (+dbus +widgets) qtbluetooth                           |

Note: Not all Linux distribution offer Qt6, in case you just have Qt5 available, edit `CMakeLists.txt` and look for this line at the beginning: `find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)` and remove Qt6 from there.

It then should look like: `find_package(QT NAMES Qt5 REQUIRED COMPONENTS Bluetooth Core LinguistTools Widgets DBus)`.

Even though I develop this software in Qt6, I try to make it Qt5-compatible!
## Getting the Project Sources
Since you already have all the required dependencies installed on your system, get the project sources with:
```
git clone https://github.com/brookiestein/BtScreenLocker
```
### Building
This project is made using CMake as build system making the build process as easy as:
```
mkdir BtScreenLocker/build
cd BtScreenLocker
cmake -G Ninja -S . -B build -DCMAKE_BUILD_TYPE=Release
sudo cmake --build build --target install --parallel `nproc`
```

What you did was:
1. Made a build directory not to mess the project development directory (not mandatory).
2. Entered to the project directory.
3. Letting CMake know how to build and install the software, and;
4. Building and installing in the same command.
5. --parallel `nproc` is just for CMake to use all you CPU threads to build faster.

CMake will tell you where it installed BtScreenLocker, but normally it's installed at `/usr/local`.

# Usage
Using BtScreenLocker is pretty easy actually. The first time you run it, it'll scan for Bluetooth devices and after a couple of seconds, it'll show you the discovered ones. **Please make sure the device you want to trust on is discoverable!**

In the window that just appeared, double click on the device you want to trust and it's going to be saved on the trusted list.

After that, BtScreenLocker will run as a daemon without needing your interaction. Every 30 seconds, it'll scan again to look for that trusted device and if it determines that devices has gone away, it'll lock your screen in order to protect your computer from unauthorized access.

As soon as you unlock your computer, it's going to start scanning again every 30 seconds.

If for some reason you want to stop scanning and, in consequence, locking your screen if there aren't any trusted device near just run in a terminal:
```
BtScreenLocker -p
```
`-p` stands for `--pause`.

That will tell the already-running BtScreenLocker instance to stop scanning without closing itself.

If at some point you want to start scanning again, do the opposite process, run in a terminal:
```
BtScreenLocker -r
```
`-r` stands for `--resume`.

Things become even more interesting! After the first run, BtScreenLocker won't scan for Bluetooth devices to add them to the trusted list because there's already one, but you can add more at any time by running:
```
BtScreenLocker -d
```
`-d` stands for `--discover`.

Even more interesting! If you already have a running instance of BtScreenLocker, just run:
```
BtScreenLocker -s
```
`-s` stands for `--scan-again`.
That will tell the already-running BtScreenLocker instance to stop scanning for trusted Bluetooth devices and scan to add more to the trusted list instead!

Both if you added a new Bluetooth device to the trusted list as if you didn't, BtScreenLocker will start scanning for the trusted devices you had before (including the possibly added new one).

# Cheat Sheet
| Long Option  | Short Option  | Description                                                                 |
|--------------|---------------|-----------------------------------------------------------------------------|
| --debug      | -D            | Enable debug log (implicitly enables verbose mode).                         |
| --discover   | -d            | Discover new Bluetooth devices in order to add them to the trusted list.    |
| --help       | -h            | Show this help.                                                             |
| --kill       | -k            | End an existing BtScreenLocker instance.                                    |
| --pause      | -p            | Pause an already running BtScreenLocker instance.                           |
| --resume     | -r            | Resume an already running and paused BtScreenLocker instance.               |
| --scan-again | -s            | Same as --discover, but for an already running BtScreenLocker instance.     |
| --verbose    | -v            | Enable verbose log.                                                         |
| --version    | -V            | Show this program version.                                                  |
