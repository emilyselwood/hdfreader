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

char* readHdfBytes(int32 sds_id, int* dimsizes) {
  char* data = (char*) malloc(sizeof(char) * dimsizes[0] * dimsizes[1]);
  int32 start[2] = {0, 0};

  int status = SDreaddata(sds_id, start, NULL, dimsizes, data);
  check(status, "Error reading chunk\n");
  return data;
}

short* readHdfShort(int32 sds_id, int* dimsizes) {
  short* data = (short*) malloc(sizeof(short) * dimsizes[0] * dimsizes[1]);
  int32 start[2] = {0, 0};

  int status = SDreaddata(sds_id, start, NULL, dimsizes, data);
  check(status, "Error reading chunk\n");
  return data;
}

int readAttrInt(int32 sds_id, int32 nt, int entry, int count, char * atrname, char * name) {

  int status;
  /* figure out the size of the attribute */
  int32 dsize = DFKNTsize(nt);
  check(dsize, "Error got invalid size for attribute %s in section %s\n", atrname, name);

  if(nt == 21) {
    char * tbuff = (char*) malloc(dsize * (count + 1));
    if(tbuff == NULL) {
      printf("Error could not allocate space for attribute\n");
      return 1;
    }

    status = SDreadattr(sds_id, entry, tbuff);
    check(status, "Error failed to read attribute %d from %d\n", atrname, name);
    return *tbuff;
  } else if(nt == 22) {

    /* allocate space for the attribute */
    short * tbuff = (short*) malloc(dsize * (count + 1));
    if(tbuff == NULL) {
      printf("Error could not allocate space for attribute\n");
      return 1;
    }

    status = SDreadattr(sds_id, entry, tbuff);
    check(status, "Error failed to read attribute %d from %d\n", atrname, name);
    return *tbuff;
  }
  else {
    printf("ERROR unknown datatype %d\n", nt);
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

  int32 fillValue = -1;


  if(argc != 3) {

    printf("%d usage: hfsprocessor file part\n", argc);
    return 1;
  }
  //TODO: this should be passed in  some how.
  //char * filename = "/home/wselwood/data/real/MYD11A2.A2015009.h22v10.005.2015020130228.hdf";
  char * filename = argv[1]; //"/home/wselwood/data/real/MYD13A3.A2014305.h22v10.005.2014349100449.hdf";
  //char * requiredPart = "Clear_sky_days";
  char * requiredPart = argv[2]; //"1 km monthly NDVI";

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
      int datatype = nt;

      for (int j = 0; j < nattrs; j++) {
        status = SDattrinfo(sds_id, j, atrname, &nt, &count);
        check(status, "Error getting attribute info for %s", name);
        //printf("attr %s type %d value %d\n", atrname, nt, count);
        if (strcmp("_FillValue", atrname) == 0) {
          fillValue = readAttrInt(sds_id, nt, j, count, atrname, name);
        }

      }

      printf("%s x:%d y:%d fillValue:%d\n", name, dimsizes[0], dimsizes[1], fillValue);
      if(fillValue == -1) {
        printf("ERROR fill value not found\n");
        return 1;
      }


      if (datatype == 21) { // bytes
        char * data = readHdfBytes(sds_id, dimsizes);
        for (int y = 0; y < dimsizes[0]; y++) {
          for (int x = 0; x < dimsizes[1]; x++) {
            int value = data[(y*dimsizes[0])+x];
            if (value != fillValue) {
              printf("#");
            } else {
              printf(" ");
            }
          }
          printf("\n");
        }

        free(data);
      } else if (datatype == 22) {
        short * data = readHdfShort(sds_id, dimsizes);
        for (int y = 0; y < dimsizes[0]; y++) {
          for (int x = 0; x < dimsizes[1]; x++) {
            int value = data[(y*dimsizes[0])+x];
            if (value != fillValue) {
              printf("#");
            } else {
              printf(" ");
            }
          }
          printf("\n");
        }

        free(data);
      } else {
        printf("ERROR unknown datatype %d\n", datatype);
        return 1;
      }

    }

  }

  return 0;
}
