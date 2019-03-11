#!/usr/bin/perl

use strict;
use HTML::Entities;

my $folderHtml = "InfoPages";

if (defined($ARGV[0])){
    $folderHtml=$ARGV[0];
}



open (FILE, "> music_blank.ts");
print FILE "<?xml version=\"1.0\" encoding=\"utf-8\"?>
<!DOCTYPE TS>
<TS version=\"2.1\">
<context>
    <name>QtWindow</name>
";

opendir DIR, $folderHtml or die $!;
while(my $fname = readdir DIR) {
    next unless ($fname=~/(\.html)$/);
    print $fname."\n";
    my $text = readFile($folderHtml."/".$fname);
    print FILE "    <message>\n";
    print FILE "       <source>$text</source>\n";
    print FILE "        <translation type=\"unfinished\"></translation>\n";
    print FILE "    </message>";
}
closedir DIR;


print FILE "</context>\n";
print FILE "</TS>\n";
close FILE;



sub readFile {
    my $filename = shift;

    my $text="";

    open (FILE2,"< $filename");
    $text.=$_ while(<FILE2>);
    close FILE2;

    $text = encode_entities($text);
}
