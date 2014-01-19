mbdbdump
========

## NAME

mbdbdump - analyze contents of an iPhone backup

## SYNOPSIS

`mbdbdump MBDB_FILE [ list | extract | cat PATH ]`

## DESCRIPTION

Perform operations on `MBDB_FILE`.

### list

list the contents of the backup described by `MBDB_FILE`.

### extract

extract the entierty of the backup described by `MBDB_DUMP` to the current
directory.

### cat

dump contents of `PATH` to the standard output. `PATH` is a file present in the
backup described by `MBDB_FILE`.

## EXAMPLES

* `mbdbdump ~/Backups/iPhone/2014-10-19/Manifest.mbdb list`
* `mbdbdump ~/Backups/iPhone/2014-10-19/Manifest.mbdb cat HomeDomain-Library/SMS/sms.db`
* `cd /tmp/iPhone-extract; mbdbdump ~/Backups/iPhone/2014-10-19/Manifest.mbdb extract`

## AUTHORS

Written by Stephane Sezer `<sas@cd80.net>`.

## COPYRIGHT

This tool is published under the BSD 3-clause license.
