#!/usr/bin/bash
#
# flow-sandbox-run.sh
#

gcc -lm -Wall -Wextra -Winline -pedantic `pkg-config --cflags gtk4` \
	`pkg-config --libs gtk4` -DGNUISANCE_BUILD_FLAG_MANUAL_ENVIRONMENT \
	-I../../src -I../../src/widgets/flow -o '/tmp/flow' \
	../../src/widgets/flow/gnui-flow.c flow-sandbox.c && \
(test "x${1}" = 'x-d' && GTK_DEBUG=interactive '/tmp/flow' || \
	'/tmp/flow' "${@}") && rm '/tmp/flow'
