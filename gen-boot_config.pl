#!/usr/bin/perl
use strict;
use warnings;

$_ = << 'EOF';
%ifndef _BOOT_CONFIG_INC_H_
%define _BOOT_CONFIG_INC_H_

%define NUM_TRACKS_TO_LOAD      XXX

%endif ; _BOOT_CONFIG_INC_H_
EOF

if ($ARGV[0] && $ARGV[0] =~ /^[0-9]+$/) {
    s/XXX/$ARGV[0]/;
} else {
    exit 1;
}

print
