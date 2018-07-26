# i3lock_image

![alt text](res/example.gif)

A simple program intended to be used in conjuction with i3lock & scrot to improve a lock screen's appearance.

Use it as follows:
```
scrot screenshot.png -e 'i3lock_image $f i3lock_image.png' && i3lock -i i3lock_image.png
```

## Install
```
sudo apt install scrot libpng++-dev libx11-xcb-dev
git clone https://github.com/donqustix/i3lock_image
cd i3lock_image
make

```
