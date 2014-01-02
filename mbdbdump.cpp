#include "mbdb_record.h"

#include <err.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <iostream>

#define PACKED          __attribute__((__packed__))
#define ASIZEOF(Array)  (sizeof (Array) / sizeof (Array[0]))

#define MBDB_SIG        "mbdb\5\0"
#define MBDB_SIG_LEN    ((sizeof MBDB_SIG) - 1)

static void map(const char* path, const char** begin_addr, const char** end_addr)
{
  int fd;
  struct stat buf;
  void *addr;

  if ((fd = open(path, O_RDONLY)) == -1)
    err(2, "%s", path);

  if (fstat(fd, &buf) == -1)
    err(2, "fstat(%s)", path);

  if ((addr = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    err(2, "mmap(%s)", path);

  *begin_addr = (const char*) addr;
  *end_addr = (const char*) addr + buf.st_size;
}

int main(int argc, char** argv)
{
  const char* addr;
  const char* end_addr;

  if (argc != 2)
    errx(1, "requires one argument");

  map(argv[1], &addr, &end_addr);

  if (memcmp(addr, MBDB_SIG, MBDB_SIG_LEN) != 0)
    errx(2, "%s: is not a valid MBDB file", argv[1]);

  addr += MBDB_SIG_LEN;

  while (addr < end_addr) {
    auto r = mbdb_record(addr);
    r.dump(std::cout);
  }

  return 0;
}
