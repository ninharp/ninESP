#!/usr/bin/perl -w

use strict;
use POSIX qw(strftime);

my $max_minor = 199;
my $max_build = 1999;

my $date = strftime "%d.%m.%Y", localtime;
my @def_files = ( "include/app_defaults.h" );

# Incrementing Source files with C defined Version Code
foreach my $f (@def_files) {
	my @new;
	open(F, "<$f") or die $!;
	my @content = <F>;
	my $major_set = 0;
	my $minor_set = 0;
	foreach my $line (@content) {
		if ($line =~ m/\#define\ VER_BUILD\t\t(\d+)/) {
			my $build = $1;
			$build++;
			if ($build > $max_build) {
				$build = 0;
				$minor_set = 1;
			}
			$line =~ s/(\#define\ VER_BUILD\t\t)\d+(.*)$/$1$build$2/;
		}
		elsif ($line =~ m/\#define.?VER_MINOR\t\t(\d+)/) {
			if ($minor_set > 0) {
				$minor_set = 0;
				my $minor = $1;
				$minor++;
				if ($minor > $max_minor) {
					$minor = 0;
					$major_set = 1;
				}
				$line =~ s/(\#define.?VER_MINOR\t\t)\d+(.*)$/$1$minor$2/;
			}
		} 
		elsif ($line =~ m/\#define.?VER_MAJOR.?(\d+).*$/) {
			if ($major_set > 0) {
				$major_set = 0;
				my $major = $1;
				$major++;
				$line =~ s/(\#define.?VER_MAJOR.?)\d+(.*)$/$1$major$2/;
			}
		}
		push(@new, $line);
	}
	close(F);
	open(F, ">$f") or die $!;
	foreach my $line (@new) {
		print F $line;
	}
	close(F);
}

printf "Version Codes updated!\n";
