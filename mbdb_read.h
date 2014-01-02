#ifndef MBDB_READ_H_
# define MBDB_READ_H_

# include <cstdint>
# include <string>
# include <vector>

template<typename T>
static void read(const char*& addr, T& output)
{
  output = *((const T*) addr);
  addr += sizeof (T);
}

template<>
void read<uint16_t>(const char*& addr, uint16_t& output)
{
  output = ntohs(*((uint16_t*) addr));
  addr += sizeof (uint16_t);
}

template<>
void read<uint32_t>(const char*& addr, uint32_t& output)
{
  output = ntohl(*((uint32_t*) addr));
  addr += sizeof (uint32_t);
}

template<>
void read<uint64_t>(const char*& addr, uint64_t& output)
{
  uint32_t tmp[2];

  read<uint32_t>(addr, tmp[0]);
  read<uint32_t>(addr, tmp[1]);
  output = ((uint64_t) tmp[0]) << 32 | tmp[1];
}

template<>
void read<std::string>(const char*& addr, std::string& output)
{
  uint16_t len;

  read<uint16_t>(addr, len);

  if (len == 0xffff)
    len = 0;

  output.assign(addr, len);

  addr += len;
}

template<>
void read<std::vector<uint8_t>>(const char*& addr, std::vector<uint8_t>& output)
{
  uint16_t len;

  read<uint16_t>(addr, len);

  if (len == 0xffff)
    len = 0;

  output.assign(addr, addr + len);

  addr += len;
}

#endif /* !MBDB_READ_H_ */
