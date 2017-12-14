#!/usr/bin/perl -w

## use lib "/home/ansell/exe/getMk";

use CMakeList;
use strict;

## EXECUTABLES
my @masterprog=("activation");



## GENERAL Directory 
my @sublibdir=qw(src global);


my @libnames=@sublibdir;

## INCLUDES


## SYSTEM Directory

my @incdir=qw( include fileSupportInc globalInc );


my $gM=new CMakeList;
$gM->setParameters(\@ARGV);
$gM->addMasterProgs(\@masterprog);
$gM->addIncDir("",\@incdir);
$gM->findSubIncDir("System");

$gM->findSubSrcDir("System");
$gM->findSubSrcDir("");


$gM->addDepUnit("activation",   ["src","attachComp","input","log","process",
				 "geometry","glob","mersenne","monte",
				 "src","xml","poly","support","md5",
				 "fileSupport","work"]);
$gM->writeCMake();

print "FINISH CMake.pl\n";
exit 0;

