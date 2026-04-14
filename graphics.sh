#!/bin/sh
printf '\033c\033]0;%s\a' PixelPartyGraphics
base_path="$(dirname "$(realpath "$0")")"
"$base_path/graphics.arm64" "$@"
