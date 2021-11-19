/*
   This file is part of Wool, a library for fine-grained independent
   task parallelism

   Copyright 2009- Karl-Filip Faxén, kff@sics.se
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
       * Redistributions of source code must retain the above copyright
         notice, this list of conditions and the following disclaimer.
       * Redistributions in binary form must reproduce the above copyright
         notice, this list of conditions and the following disclaimer in the
         documentation and/or other materials provided with the distribution.
       * Neither "Wool" nor the names of its contributors may be used to endorse
         or promote products derived from this software without specific prior
         written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR OTHER CONTRIBUTORS BE LIABLE FOR ANY
   DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   This is Wool version @WOOL_VERSION@
*/

#include "wool.h"
#include <stdio.h>
#include <stdlib.h>

// Basic matrix multiply code, no blocking, parallelization of outermost loop, accumulating.

LOOP_BODY_4( mm, LARGE_BODY/1, int, i, int, rows, double*, a, double*, b, double*, c)
{
  int j;

  for( j=0; j<rows; j++ ) {
    double sum = 0.0;
    int k;
    for( k=0; k<rows; k++ ) {
      sum += a[i*rows+k]*b[k*rows+j];
    }
    c[i*rows+j] = sum;
  }
}


TASK_2(int, main, int, argc, char**, argv) {
  /* Decode arguments */

  if(argc < 3) {
    fprintf(stderr, "Usage: %s [wool options] <matrix rows> <repetitions>\n", argv[0]);
    exit(1);
  }
  int rows = atoi(argv[1]);
  int reps = atoi(argv[2]);


  /* Allocate and initialize matrices */

  double *a = malloc(rows*rows*sizeof(double));
  double *b = malloc(rows*rows*sizeof(double));
  double *c = malloc(rows*rows*sizeof(double));

  for( int i=0; i<rows; i++ ) {
    for( int j=0; j<rows; j++ ) {
      a[i*rows+j] = 0.0;
      b[i*rows+j] = 0.0;
    }
    a[i*rows+i] = 1.0;
    b[i*rows+i] = 1.0;
  }


  /* Multiply matrices */

  for( int i=0; i<reps; i++ ) {
    FOR( mm, 0, rows, rows, a, b, c );
    double *t = a; a = c; c = t;
  }

  /* Check result */

  int ok = 1;
  for( int i=0; i<rows; i++ ) {
    for( int j=0; j<rows; j++ ) {
      if( i!=j && a[i*rows+j] != 0.0 ) {
        ok = 0;
      }
      if( i==j && a[i*rows+j] != 1.0 ) {
        ok = 0;
      }
    }
  }

  printf("Ok: %d\n", ok);
  free(a);
  free(b);
  free(c);
  return 0;

}
