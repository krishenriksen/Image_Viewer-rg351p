# Image Viewer for rg351p
Display a large amount of image formats to the screen with simple functions like zooming, flipping...  
Linux, macOS and Windows platforms are supported.  
Program is written in POSIX C and uses SDL2 for rendering.

## Usage
```
image-viewer <image file> <image file> <image file>
```
`<image file>` is the image file to load.  
  
Use `image-viewer --help` to display program usage and control keys.

Scroll images with up and down on the d-pad and rotate with A button.

## Adding to emulationstation in ArkOS
Add a new folder under /roms/ called images and place your images there.

Modify /etc/emulationstation/es_systems.cfg and add the following:

```
  <system>
    <name>Image</name>
    <fullname>Image</fullname>
    <path>/roms/images/</path>
    <extension>.jpg .JPG .png .PNG .tif .TIF .bmp .BMP</extension>
    <command>/roms/images/show.sh %ROM%</command>
    <platform>images</platform>
    <theme>images</theme>
  </system>
```

Now copy show.sh under dependencies to /roms/images/

And copy the folder `images` under dependencies to your theme located in /etc/emulationstation/ and change theme.xml to your theme name

`<include>./../gbz35.xml</include>`

To quit the image viewer, press select or start on the Anbernic rg351p

Support the project
---

[![Donate](https://github.com/krishenriksen/AnberPorts/raw/master/donate.png)](https://www.paypal.me/krishenriksendk)
[![Patreon](https://github.com/krishenriksen/AnberPorts/raw/master/patreon.png)](https://www.patreon.com/bePatron?u=54003740)
[![Sponsor](https://github.com/krishenriksen/AnberPorts/raw/master/sponsor.png)](https://github.com/sponsors/krishenriksen)
