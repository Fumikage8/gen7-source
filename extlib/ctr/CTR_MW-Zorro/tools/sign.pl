#!/bin/perl

# -----------------------------------------------------------------------
# Project: zorro
# File: sign.pl
# Copyright (C) 2015 Nintendo, All rights reserved.
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
# The content herein is highly confidential and should be handled accordingly.
# -----------------------------------------------------------------------


# To generate key with OpenSSL:
# =============================
# openssl genrsa 768 > key        # (replace 768 by the key size you want)
# openssl rsa -in key -outform der -out privKey.der
# openssl rsa -in key -pubout -outform der -out pubKey.der

# Dependencies
# ============
# To use this script, first install a perl distribution, and then dependencies by calling:
# perl -MCPAN -e shell
# and typing
# install Crypt::Cipher::AES
# install Crypt::PK::RSA
# install Crypt::Digest::SHA1

# Usage example
# =============
# Assume you have 768 bit public/private keys in pubKey.der and privKey.der for signing with 8 bytes
# overhead.
# Let's put the message to sign in message_file:
# echo "This message has been signed by the perl script in the tools/ directory and has 88 chars" > message_file
# Call the tool:
# ./sign.pl message_file pubKey.der privKey.der 8 > signature

use strict;
use warnings;

use Crypt::Cipher::AES;
use Crypt::PK::RSA;
use Crypt::Digest::SHA1 qw(sha1);

if ($#ARGV != 3) {
	print "Usage: ./sign.pl message_file pubKey.der privKey.der overhead\n";
	exit;
}

my $message=$ARGV[0];
my $pubkeyFile=$ARGV[1];
my $privkeyFile=$ARGV[2];
my $overhead=$ARGV[3];

if ($overhead >= 20)
{
    die "Too much overhead for SHA1";
}

my $privKey = Crypt::PK::RSA->new($privkeyFile);
my $modulusSize = $privKey->size;

my $messageSize = -s $message;
my $messageContents;

my $pubkeyFileSize = -s $pubkeyFile;
my $pubkeyContents;


open(MSG, $message) or die "Could not open $message";
binmode MSG;
read MSG, $messageContents, $messageSize;
close(MSG);
if ($messageSize != length($messageContents))
{
    die "Message size different from expected";
}

open(PKEY, $pubkeyFile) or die "Could not open $pubkeyFile";
binmode PKEY;
read PKEY, $pubkeyContents, $pubkeyFileSize;
close(PKEY);
if ($pubkeyFileSize != length($pubkeyContents))
{
    die "Public key size different from expected";
}

my $messagePart2Size = $modulusSize - $overhead;

if ($messageSize < $messagePart2Size) {
    die "Message too short (", $messageSize, ") needs ", $modulusSize - $overhead
}

my $clearPart = substr $messageContents, 0, -$messagePart2Size;
my $signPart = substr $messageContents, -$messagePart2Size;
my $messageDigest = sha1($messageContents);
$signPart .= substr $messageDigest, 0, $overhead;
  
my $aesKey = sha1($pubkeyContents, $clearPart);
$aesKey = substr $aesKey, 0, 16;

my $aes = Crypt::Cipher::AES->new($aesKey);

my $iv = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

my $interm = "";
my $blockCount = length($signPart) / 16;
for (my $i = 0; $i < $blockCount; ++$i)
{
    my $curSrc = substr $signPart, $i * 16, 16;
    $curSrc ^= $iv;
    $iv = $aes->encrypt($curSrc);
    $interm .= $iv;
}

my $xoredBlock = substr $interm, 0, 16;
my $lastBlock = substr $interm, -16;

$xoredBlock ^= $lastBlock;

my $bitstring = unpack "B*", $xoredBlock;
my $bitshifted = substr $bitstring, 1;
$bitshifted = $bitshifted . "0";

my $need87 = substr $bitstring, 0, 1 == '1';

my $mask = pack "B*", $bitshifted;

if ($need87) {
    my $lastBlock = substr $mask, -1;
    my $beginning = substr $mask, 0, -1;
    my $xored = $lastBlock ^ chr(0x87);
    $mask = $beginning . $xored;
}

$iv = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
my $result = "";
for (my $i=$blockCount - 1; $i >= 0; --$i) {
    my $nextIv = substr $interm, 16 * $i, 16;
    $nextIv ^= $mask;
    my $cur = $aes->encrypt($nextIv);
    $result = ($cur ^ $iv) . $result;
    $iv = $nextIv;
}

my $result0 = substr $result, 0, 1;
$result0 &= chr(0x7F);
$result = $result0 . (substr $result, 1);

my $sign = $privKey->decrypt($result, 'none');

binmode(STDOUT) or die "cannot binmode standard output";
print STDOUT $clearPart;
print STDOUT $sign;
