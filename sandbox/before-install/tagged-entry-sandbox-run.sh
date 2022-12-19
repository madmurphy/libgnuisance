#!/usr/bin/bash
#
# tagged-entry-sandbox-run.sh
#

gcc -lm -Wall -Wextra -Winline -pedantic `pkg-config --cflags gtk4` \
	`pkg-config --libs gtk4` -I../../src  -I../../src/widgets/flow \
	-DGNUISANCE_BUILD_FLAG_MANUAL_ENVIRONMENT -o '/tmp/tagged-entry' \
	../../src/widgets/flow/gnui-flow.c \
	../../src/widgets/tagged-entry/gnui-tagged-entry.c \
	tagged-entry-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/tagged-entry' || \
	'/tmp/tagged-entry' "${@}") && rm '/tmp/tagged-entry'
