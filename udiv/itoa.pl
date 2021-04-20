#!/usr/bin/env perl

use 5.14.0;
use warnings;
use integer;

my ($num) = @ARGV or die "usage: $0 number";
say itoa($num);
exit;

sub itoa {
    my $str = '';
    do {
        $str .= chr(ord('0') + $num % 10);
        $num /= 10;
    } while ($num > 0);

    return $str;
}
