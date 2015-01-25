#include <stdio.h>
#include <mfhdf.h>
#include <hdf.h>
#include <stdarg.h>
#include <strings.h>

void check (int status, char * message, ...) {
  if (status < 0) {
    va_list(args);
    va_start(args, message);
    vprintf(message, args);
    exit(1);
  }
}

int main (int argc, char** argv) {

  int32 nsds;
  int32 ngattr;
  int32 nattrs;
  int32 nt;
  int32 rank;
  int32 dimsizes[50];
  int32 count;
  char name[512];
  char atrname[512];

  int32 fillValue;
  int32 xSize;
  int32 ySize;

  //TODO: this should be passed in  some how.
  char * filename = "/home/wselwood/data/real/MYD11A2.A2015009.h22v10.005.2015020130228.hdf";
  char * requiredPart = "Clear_sky_days";

  for (int i = 0; i < 50; i++) {
    dimsizes[i] = 0;
  }

  int fid = SDstart(filename, DFACC_RDONLY);
  check(fid, "Error opening file %s returned code: %d\n", filename, fid);


  int status = SDfileinfo(fid, &nsds, &ngattr);
  check(status, "Error extracting attributes from %s returned %d\n", filename, fid);

  for (int i = 0; i < nsds; i++) {
    int32 sds_id = SDselect(fid, i);
    check(sds_id, "Error selecting section %d from file %s returned %d\n", i, filename, sds_id);

    status = SDgetinfo(sds_id, name, &rank, dimsizes, &nt, &nattrs);
    check(status, "Error getting info for section %d in %s returned %d\n", i, filename, sds_id);


    if (strcmp(name, requiredPart) == 0) {


      // Hack: find the number of elements in the dim sizes array that are actually populated.
      int n = 0;
      while(dimsizes[n] != 0) {
        n++;
      }

      if(n == 2) {
        xSize = dimsizes[0];
        ySize = dimsizes[1];
      }
      else {
        printf("Error incorrect number of dimentions %d\n", n);
        return 1;
      }

      for (int j = 0; j < nattrs; j++) {
        status = SDattrinfo(sds_id, j, atrname, &nt, &count);
        check(status, "Error getting attribute info for %s", name);
        //printf("attr %s type %d value %d\n", atrname, nt, count);
        if (strcmp("_FillValue", atrname) == 0) {
          char *tbuff;
          /* figure out the size of the attribute */
          int32 dsize = DFKNTsize(nt);
          check(dsize, "Error got invalid size for attribute %s in section %s\n", atrname, name);

          /* allocate space for the attribute */
          tbuff = HDgetspace(dsize * (count + 1));
          if(tbuff == NULL) {
            printf("Error could not allocate space for attribute\n");
            return 1;
          }

          status = SDreadattr(sds_id, j, tbuff);
          fillValue = atoi(tbuff);
        }
      }

      printf("%s x:%d y:%d fillValue:%d\n", name, xSize, ySize, fillValue);

      char* data = (char*) malloc(sizeof(char) * xSize * ySize);
      if(data == NULL) {
        printf("ERROR could not allocate data");
        return 1;
      }
      printf("allocated memory\n");

      int32 start[2], edges[2];

      start[0] = 0;
      start[1] = 0;
      edges[0] = xSize;
      edges[1] = ySize;
      status = SDreaddata(sds_id, start, NULL, edges, data);
      check(status, "Error reading chunk from %s\n", name);
      printf("read chunk\n");
      for (int x = 0; x < xSize; x++) {
        for (int y = 0; y < ySize; y++) {
          char value = data[(x*xSize)+y];
          if (value != fillValue) {
            printf("%d", value);
          } else {
            printf(" ");
          }

        }
        printf("\n");
      }
      free(data);
    }

  }

  return 0;
}