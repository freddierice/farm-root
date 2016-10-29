# Farm Root
Farm root is a rooting utility for android devices using the dirty cow vulnerability. 

## Support
* Right now it doesn't have support for easy mode root, just pulling and pushing images.
* only tested on galaxy s7 active
* only works for arm64v8

## Showing the status
```bash
make log
```

## Pulling an image
```bash
make pull
```

## Pushing an image (flashing)
Place the image you want to flash in the root directory with the name `recovery_push.img`. This is a really dangerous function. Only do this if you know what you are doing.
```bash
make push
```

## Notes
If the log gets stuck here
```
farm-root: [*] waiting for process to finish
```
try to get system-server to run a toolbox command. (turning lockscreen on and off seems to work for galaxy s7 active). 

## TODO
* fix for armeabi? doesn't seem to work for this architecture right now.

## Help/Sources
* https://github.com/timwr/CVE-2016-5195 for dirtycow exploit.
* https://github.com/jcadduono/android_external_dirtycow for some SELinux context ideas

