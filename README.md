# LAP (Lisp Audio Player)

### Play audio with scheme



#### Debian Requirements:

dependencies:
```
sudo apt install libjack-jackd2-dev pipewire-jack jackd2
```

configure the path to pipewire:
```
echo "/usr/lib/x86_64-linux-gnu/pipewire-0.3/jack" | sudo tee /etc/ld.so.conf.d/pipewire-jack.conf
sudo ldconfig
```

make sure pipewire is running:
```
systemctl --user enable --now pipewire wireplumber pipewire-pulse
```

