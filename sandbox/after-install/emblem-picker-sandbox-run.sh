#!/usr/bin/bash
#
# emblem-picker-sandbox-run.sh
#

gcc -pedantic -Wall -Wextra -Winline `pkg-config --cflags gnuiemblempicker` \
	`pkg-config --cflags libadwaita-1` `pkg-config --libs libadwaita-1` \
	`pkg-config --libs gnuiemblempicker` -o '/tmp/emblem-picker' \
	emblem-picker-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/emblem-picker' || \
	'/tmp/emblem-picker' "${@}") && rm '/tmp/emblem-picker'
