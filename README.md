![title_top](https://user-images.githubusercontent.com/70841910/188854698-7a2a4b7e-bae5-4723-bf7b-628ab14c97ae.png)

<h1 align="center">Phrosty</h1>
<p align="center">
  <strong>A Global Picture-in-Picture (PIP) tool for Linux</strong>
</p>

<br>
<br>

Phrosty is a global Picture-in-Picture (PIP) tool for Linux.

Unlike other tools, Phrosty does **not depend on a specific desktop environment** and can be used in any environment.

## Requirement

* OpenGL 4.2 or later
* Qt5
* libnotify

In most systems, these requirements should already be satisfied.  
As for now, Wayland support is incomplete, so it is recommended to use on X11.

## Installation

### 1. Install Phrosty

Currently, you can download the binary packages from the [Release page](https://github.com/crux14/phrosty/releases).  
To build from source, see [Building Phrosty](#bulding-phrosty).

### 2. Set a shortcut key for Phrosty

Phrosty is a CLI tool, but it's always recommended to use it thoroughly by a global shortcut key.

![hotkey](https://user-images.githubusercontent.com/70841910/188854876-74304a1e-283d-4910-8b5f-7bfcb8a1585e.png)

## Usage

Launch phrosty and click on the window you want to display in PIP. Then your PIP window will show up. After that, you can tweak window settings as the video below shows. Default settings for Phrosty can be overridden by the command line arguments. For instance, the command `phrosty --use-default-border=1` overrides the `use-default-border` option so that the default window border is enabled. For details, check `phrosty -h`.

<br>

https://user-images.githubusercontent.com/70841910/188854986-f1224534-8289-4adc-92c2-ff9fc87ed036.mp4

## Bulding Phrosty

Phrosty requires cmake(>=20) for manual build.  

You also need to install build dependencies.  

**For Debian-based distros (Debian, Ubuntu, ...etc)**, you can install them by the command below.  

`apt-get install qtbase5-dev libqt5x11extras5-dev libx11-dev libnotify-dev libglm-dev libxcomposite-dev libxrender-dev`

**For other distros**, you need to install equivalent dependencies as the command above.  

Once you have all necessary dependencies, execute this command.  

`mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make -j$(nproc) && make install`

## Attributions

### Desktop icon

<a target="_blank" href="https://icons8.com/icon/13151/cooling">Cooling</a> icon by <a target="_blank" href="https://icons8.com">Icons8</a>

### Others

See the credits folder which should be bundled in the distributed package.
