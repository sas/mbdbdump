#ifndef MBDB_RECORD_H_
# define MBDB_RECORD_H_

# include <cstdint>
# include <iostream>
# include <map>
# include <string>

struct mbdb_record {
  typedef std::map<std::string, std::string> prop_table;

  mbdb_record(const char*& addr);
  void dump(std::ostream& out) const;
  void extract(const char* mbdb_dir) const;

  std::string domain;
  std::string path;
  std::string link_target;
  std::string data_hash;
  uint16_t    mode;
  uint32_t    uid;
  uint32_t    gid;
  uint32_t    mtime;
  uint32_t    atime;
  uint32_t    ctime;
  uint64_t    size;
  uint8_t     flag;
  prop_table  properties;

  std::string storage_hash;
};

#endif /* !MBDB_RECORD_H_ */
