#!/usr/bin/perl

# This script generates music_blank.ts
# License: same with Pianobooster
# Author: DanSoft <http://dansoft.krasnokamensk.ru/>

use strict;

if (!defined($ARGV[0])){
    print("use $0 <folder>\n");
    exit;
}

my $folderBooks=$ARGV[0];


open (FILE, "> music_blank.ts");
print FILE "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<!DOCTYPE TS>
<TS version=\"2.1\">
<context>
    <name>QtWindow</name>
";


opendir DIR_FOLDER, $folderBooks or die $!;
while(my $folder = readdir DIR_FOLDER) {
    next if (-f $folder or $folder eq ".." or $folder eq ".");
    
    my $folderHtml = $folderBooks."/".$folder."/InfoPages/";
    
    opendir DIR, $folderHtml or die $!;
    while(my $fname = readdir DIR) {
	next unless ($fname=~/(en\.html)$/);
        print $fname."\n";
	my $text = readFile($folderHtml."/".$fname);
        print FILE "    <message>\n";
	print FILE "       <source>$text</source>\n";
        print FILE "        <translation type=\"unfinished\"></translation>\n";
        print FILE "    </message>\n";
    }
    closedir DIR;
}
closedir DIR_FOLDER;


print FILE "</context>\n";
print FILE "</TS>\n";
close FILE;



sub readFile {
    my $filename = shift;

    my $text="";

    open (FILE2,"< $filename");
    $text.=$_ while(<FILE2>);
    close FILE2;

    $text=~s/</&lt;/gm;
    $text=~s/>/&gt;/gm;
    $text=~s/&rsquo;/'/gm;
    $text=~s/&hellip;/. . ./gm;

    return $text;
}
