all: build

build:
	ndk-build NDK_PROJECT_PATH=. APP_BUILD_SCRIPT=./Android.mk

log: 
	adb logcat | grep -a farm-root

pull: build 
	adb push toolbox_pull /data/local/tmp/toolbox
	adb push libs/arm64-v8a/farm /data/local/tmp/farm
	adb push till /data/local/tmp/till
	adb push libs/arm64-v8a/bridge_pull /data/local/tmp/bridge
	adb shell /data/local/tmp/farm
	adb pull /data/local/tmp/recovery_pull.img
	adb reboot

push: build recovery_push.img
	adb push recovery_push.img /data/local/tmp/recovery_push.img
	adb push toolbox_push /data/local/tmp/toolbox
	adb push libs/arm64-v8a/farm /data/local/tmp/farm
	adb push till /data/local/tmp/till
	adb push libs/arm64-v8a/bridge_push /data/local/tmp/bridge
	adb shell /data/local/tmp/farm
	adb reboot

recovery_push.img:
	@echo "place recovery_push.img in the farm-root directory"

clean:
	rm -rf libs
	rm -rf obj

