#ifndef MYFORK_H // include guard
#define MYFORK_H

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#include <bits/stdc++.h>
#include <dirent.h>
#include <limits.h>

#include <cstdio>
#include <iostream>
#include <string>

#include "../include/dmtcp.h"

#define INTS_PER_LOOP 5


class myfork
{
  private:
  unsigned long i = 0;
  int count = 0;
  int rr;

  int numCheckpoints, numRestarts;

  DIR *dir;
  
  struct dirent *ent;
  char cwd[PATH_MAX];

  public:
  myfork();
  int rfork();
  int checkpoint();
  void rename_me();
};


#endif