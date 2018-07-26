#ifndef REGION_H
#define REGION_H

typedef struct {
	unsigned int Start;
	unsigned int End;
	char Name[24];
} REGION_T;

extern const REGION_T RegionTable[];
extern const unsigned NumProfileRegions;
extern volatile unsigned RegionCount[];
extern unsigned SortedRegions[];

#endif
