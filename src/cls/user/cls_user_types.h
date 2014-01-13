// -*- mode:C; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab

#ifndef CEPH_CLS_USER_TYPES_H
#define CEPH_CLS_USER_TYPES_H

#include "include/encoding.h"
#include "include/types.h"
#include "include/utime.h"

/*
 * this needs to be compatible with with rgw_bucket, as it replaces it
 */
struct cls_user_bucket {
  std::string name;
  std::string data_pool;
  std::string index_pool;
  std::string marker;
  std::string bucket_id;

  void encode(bufferlist& bl) const {
     ENCODE_START(6, 3, bl);
    ::encode(name, bl);
    ::encode(data_pool, bl);
    ::encode(marker, bl);
    ::encode(bucket_id, bl);
    ::encode(index_pool, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& bl) {
    DECODE_START_LEGACY_COMPAT_LEN(6, 3, 3, bl);
    ::decode(name, bl);
    ::decode(data_pool, bl);
    if (struct_v >= 2) {
      ::decode(marker, bl);
      if (struct_v <= 3) {
        uint64_t id;
        ::decode(id, bl);
        char buf[16];
        snprintf(buf, sizeof(buf), "%llu", (long long)id);
        bucket_id = buf;
      } else {
        ::decode(bucket_id, bl);
      }
    }
    if (struct_v >= 5) {
      ::decode(index_pool, bl);
    } else {
      index_pool = data_pool;
    }
    DECODE_FINISH(bl);
  }

  bool operator<(const cls_user_bucket& b) const {
    return name.compare(b.name) < 0;
  }
};
WRITE_CLASS_ENCODER(cls_user_bucket)

/*
 * this structure overrides RGWBucketEnt
 */
struct cls_user_bucket_entry {
  cls_user_bucket bucket;
  size_t size;
  size_t size_rounded;
  time_t creation_time;
  uint64_t count;
  bool user_stats_sync;

  cls_user_bucket_entry() : size(0), size_rounded(0), creation_time(0), count(0), user_stats_sync(false) {}

  void encode(bufferlist& bl) const {
    ENCODE_START(6, 5, bl);
    uint64_t s = size;
    __u32 mt = creation_time;
    string empty_str;  // originally had the bucket name here, but we encode bucket later
    ::encode(empty_str, bl);
    ::encode(s, bl);
    ::encode(mt, bl);
    ::encode(count, bl);
    ::encode(bucket, bl);
    s = size_rounded;
    ::encode(s, bl);
    ::encode(user_stats_sync, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& bl) {
    DECODE_START_LEGACY_COMPAT_LEN(6, 5, 5, bl);
    __u32 mt;
    uint64_t s;
    string empty_str;  // backward compatibility
    ::decode(empty_str, bl);
    ::decode(s, bl);
    ::decode(mt, bl);
    size = s;
    creation_time = mt;
    if (struct_v >= 2)
      ::decode(count, bl);
    if (struct_v >= 3)
      ::decode(bucket, bl);
    if (struct_v >= 4)
      ::decode(s, bl);
    size_rounded = s;
    if (struct_v >= 6)
      ::decode(user_stats_sync, bl);
    DECODE_FINISH(bl);
  }
};
WRITE_CLASS_ENCODER(cls_user_bucket_entry)

struct cls_user_stats {
  uint64_t total_entries;
  uint64_t total_bytes;
  uint64_t total_bytes_rounded;

  void encode(bufferlist& bl) const {
     ENCODE_START(1, 1, bl);
    ::encode(total_entries, bl);
    ::encode(total_bytes, bl);
    ::encode(total_bytes_rounded, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& bl) {
    DECODE_START(1, bl);
    ::decode(total_entries, bl);
    ::decode(total_bytes, bl);
    ::decode(total_bytes_rounded, bl);
    DECODE_FINISH(bl);
  }

  void dump(Formatter *f) const;
};
WRITE_CLASS_ENCODER(cls_user_stats)

/*
 * this needs to be compatible with with rgw_bucket, as it replaces it
 */
struct cls_user_header {
  cls_user_stats stats;

  void encode(bufferlist& bl) const {
     ENCODE_START(1, 1, bl);
    ::encode(stats, bl);
    ENCODE_FINISH(bl);
  }
  void decode(bufferlist::iterator& bl) {
    DECODE_START(1, bl);
    ::decode(stats, bl);
    DECODE_FINISH(bl);
  }

  void dump(Formatter *f) const;
};
WRITE_CLASS_ENCODER(cls_user_header)

#endif

