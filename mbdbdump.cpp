#include "mbdb_record.h"

#include <err.h>
#include <fcntl.h>
#include <libgen.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <list>
#include <iostream>

#define PACKED          __attribute__((__packed__))
#define ASIZEOF(Array)  (sizeof (Array) / sizeof (Array[0]))

#define MBDB_SIG        "mbdb\5\0"
#define MBDB_SIG_LEN    ((sizeof MBDB_SIG) - 1)

static void map(const char* path, const char** begin_addr, const char** end_addr)
{
  int     fd;
  struct  stat buf;
  void*   addr;

  if ((fd = open(path, O_RDONLY)) == -1)
    err(2, "%s", path);

  if (fstat(fd, &buf) == -1)
    err(2, "fstat(%s)", path);

  if ((addr = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    err(2, "mmap(%s)", path);

  close(fd);

  *begin_addr = (const char*) addr;
  *end_addr = (const char*) addr + buf.st_size;
}

static void unmap(const char* begin_addr, const char* end_addr)
{
  munmap((void*) begin_addr, end_addr - begin_addr);
}

void usage_die(char* argv0)
{
  const char* base = basename(argv0);

  std::cerr
    << "usage: " << base << " MBDB_FILE [ list | extract ]" << std::endl
    << "       list:    list contents of the backup" << std::endl
    << "       extract: extract contents of the backup to the current directory" << std::endl;

  exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
  const char*             begin_addr;
  const char*             end_addr;
  const char*             addr;
  std::list<mbdb_record>  contents;

  if (argc < 2)
    usage_die(argv[0]);

  map(argv[1], &begin_addr, &end_addr);

  addr = begin_addr;

  if (memcmp(addr, MBDB_SIG, MBDB_SIG_LEN) != 0)
    errx(2, "%s: is not a valid MBDB file", argv[1]);

  addr += MBDB_SIG_LEN;

  while (addr < end_addr)
    contents.push_back(mbdb_record(addr));

  if (argc == 2 || strcmp(argv[2], "list") == 0) {
    for (const auto& e : contents)
      e.dump(std::cout);
  } else if (strcmp(argv[2], "extract") == 0) {
    const char* dir = dirname(argv[1]);

    for (const auto& e : contents)
      e.extract(dir);
  } else {
    usage_die(argv[0]);
  }

  unmap(begin_addr, end_addr);

  return EXIT_SUCCESS;
}
