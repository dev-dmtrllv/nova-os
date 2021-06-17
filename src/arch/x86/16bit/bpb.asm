

OEMLabel				equ	0x7c03
bytesPerSector			equ	0x7c0b
sectorsPerCluster		equ	0x7c0d
reservedSectors			equ	0x7c0e
numberOfFats			equ	0x7c10
rootDirEntries			equ	0x7c11
smallNumberOfSectors	equ	0x7c13
mediaDescriptor			equ	0x7c15
sectorsPerFat			equ	0x7c16
sectorsPerTrack			equ	0x7c18
headsPerCylinder		equ	0x7c1a
hiddenSectors			equ	0x7c1c
largeSectors			equ	0x7c20

drive					equ	0x7c24
signature				equ	0x7c26
volumeID				equ	0x7c27
volumeLabel				equ	0x7c2b
fileSystem				equ	0x7c36
