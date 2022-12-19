#!/usr/bin/bash
#
# tagged-entry-sandbox-run.sh
#

gcc -Wall -Wextra -Winline -pedantic `pkg-config --cflags gnuitaggedentry` \
	`pkg-config --libs gnuitaggedentry` -o '/tmp/tagged-entry' \
	tagged-entry-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/tagged-entry' || \
	'/tmp/tagged-entry' "${@}") && rm '/tmp/tagged-entry'
