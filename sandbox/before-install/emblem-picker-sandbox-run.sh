#!/usr/bin/bash
#
# emblem-picker-sandbox-run.sh
#

gcc -pedantic -Wall -Wextra -Winline -DGNUISANCE_BUILD_FLAG_MANUAL_ENVIRONMENT \
	-I../../src `pkg-config --cflags gtk4` \
	`pkg-config --cflags libadwaita-1` `pkg-config --libs gtk4` \
	`pkg-config --libs libadwaita-1` -o '/tmp/emblem-picker' \
	../../src/widgets/emblem-picker/gnui-emblem-picker.c \
	emblem-picker-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/emblem-picker' || \
	'/tmp/emblem-picker' "${@}") && rm '/tmp/emblem-picker'
