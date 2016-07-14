dd if=/dev/zero of=/swap0 bs=1M count=512
mkswap /swap0
echo "/swap0 swap swap" >>/etc/fstab
swapon -a
