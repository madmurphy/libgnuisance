#!/usr/bin/bash
#
# flow-sandbox-run.sh
#

gcc -Wall -Wextra -Winline -pedantic `pkg-config --cflags gnuiflow` \
	 `pkg-config --libs gnuiflow` -o '/tmp/flow' flow-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/flow' || \
	'/tmp/flow' "${@}") && rm '/tmp/flow'
