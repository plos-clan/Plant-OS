// This code is released under the MIT License

#include <libc.h>
#include <sys.h>
#include <zstd.h>

static void compress_orDie(const char *fname, const char *oname) {
  size_t const fSize     = __syscall(SYSCALL_FILE_SIZE, fname);
  void *const  fBuff     = xmalloc(fSize);
  size_t const fReadSize = __syscall(SYSCALL_LOAD_FILE, fname, fBuff, fSize);
  size_t const cBuffSize = ZSTD_compressBound(fSize);
  void *const  cBuff     = xmalloc(cBuffSize);
  assert(fReadSize == fSize, "read file error");

  size_t const cSize = ZSTD_compress(cBuff, cBuffSize, fBuff, fSize, 1);
  assert(!ZSTD_isError(cSize), "ZSTD_compress error");

  // saveFile_orDie(oname, cBuff, cSize);

  printf("%25s : %6u -> %7u - %s \n", fname, (unsigned)fSize, (unsigned)cSize, oname);

  free(fBuff);
  free(cBuff);
}

static char *createOutFilename_orDie(const char *filename) {
  size_t const inL      = strlen(filename);
  size_t const outL     = inL + 5;
  void *const  outSpace = xmalloc(outL);
  memset(outSpace, 0, outL);
  strcat(outSpace, filename);
  strcat(outSpace, ".zst");
  return (char *)outSpace;
}

int main(int argc, const char **argv) {
  const char *const exeName = argv[0];

  if (argc != 2) {
    printf("wrong arguments\n");
    printf("usage:\n");
    printf("%s FILE\n", exeName);
    return 1;
  }

  const char *const inFilename = argv[1];

  char *const outFilename = createOutFilename_orDie(inFilename);
  compress_orDie(inFilename, outFilename);
  free(outFilename);
  return 0;
}
