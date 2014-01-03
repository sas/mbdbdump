mbdbdump
========

This tool is used to dump contents of an MBDB backup.

## Building

Just type `make` :)

You will need GNU make for this, and this tool depends on OpenSSL.

## Usage

`mbdbdump MBDB_FILE [ list | extract ]`

* `list` will list contents of the backup
* `extract` will extract contents of the backup to the current directory
