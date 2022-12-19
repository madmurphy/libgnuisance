#!/usr/bin/bash
#
# try-me.sh
#

gcc -Wall -Wextra -Winline -pedantic `pkg-config --cflags gnuiflow` \
	`pkg-config --libs gnuiflow` -o '/tmp/try-me' \
	xyz-foo-bar.c try-me.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/try-me' || \
	'/tmp/try-me' "${@}") && rm '/tmp/try-me'
