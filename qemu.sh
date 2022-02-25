# kvm -fda fdImage

#VBoxManage startvm myos --putenv VBOX_GUI_DBG_ENABLED=true

qemu-system-x86_64 -fda fdImage.vfd
