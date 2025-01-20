#!/usr/bin/sh

rm -r ./plos-efi || true
mkdir -p ./plos-efi/EFI/Boot

cp ./boot.efi ./plos-efi/EFI/Boot/bootx64.efi
