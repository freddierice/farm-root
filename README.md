# Farm Root
Farm root is a root for android devices using the dirty cow vulnerability in the linux kernel. Right now it doesn't have support for easy mode root, just pulling and pushing images.

## Showing the status
```bash
make log
```

## Pulling an image
```bash
make pull
```

## Pushing an image (flashing)
place the image you want to flash in the root directory with the name `recovery_push.img`.
```bash
make push
```

## TODO
* fix for armeabi? doesn't seem to work for this architecture right now.

## Help/Sources
* https://github.com/timwr/CVE-2016-5195 for dirtycow exploit.
* https://github.com/jcadduono/android_external_dirtycow for some SELinux context ideas

