/** THIS IS AN AUTOMATICALLY GENERATED FILE.  DO NOT MODIFY
 * BY HAND!!
 *
 * Generated by lcm-gen
 **/

#include <string.h>
#include "laser_t.h"

static int __laser_t_hash_computed;
static int64_t __laser_t_hash;
 
int64_t __laser_t_hash_recursive(const __lcm_hash_ptr *p)
{
    const __lcm_hash_ptr *fp;
    for (fp = p; fp != NULL; fp = fp->parent)
        if (fp->v == __laser_t_get_hash)
            return 0;
 
    const __lcm_hash_ptr cp = { p, (void*)__laser_t_get_hash };
    (void) cp;
 
    int64_t hash = 0xf1e8ba118c05af46LL
         + __int64_t_hash_recursive(&cp)
         + __int32_t_hash_recursive(&cp)
         + __float_hash_recursive(&cp)
         + __int32_t_hash_recursive(&cp)
         + __float_hash_recursive(&cp)
         + __float_hash_recursive(&cp)
         + __float_hash_recursive(&cp)
        ;
 
    return (hash<<1) + ((hash>>63)&1);
}
 
int64_t __laser_t_get_hash(void)
{
    if (!__laser_t_hash_computed) {
        __laser_t_hash = __laser_t_hash_recursive(NULL);
        __laser_t_hash_computed = 1;
    }
 
    return __laser_t_hash;
}
 
int __laser_t_encode_array(void *buf, int offset, int maxlen, const laser_t *p, int elements)
{
    int pos = 0, thislen, element;
 
    for (element = 0; element < elements; element++) {
 
        thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __int32_t_encode_array(buf, offset + pos, maxlen - pos, &(p[element].nranges), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_encode_array(buf, offset + pos, maxlen - pos, p[element].ranges, p[element].nranges);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __int32_t_encode_array(buf, offset + pos, maxlen - pos, &(p[element].nintensities), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_encode_array(buf, offset + pos, maxlen - pos, p[element].intensities, p[element].nintensities);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_encode_array(buf, offset + pos, maxlen - pos, &(p[element].rad0), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_encode_array(buf, offset + pos, maxlen - pos, &(p[element].radstep), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
    }
    return pos;
}
 
int laser_t_encode(void *buf, int offset, int maxlen, const laser_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __laser_t_get_hash();
 
    thislen = __int64_t_encode_array(buf, offset + pos, maxlen - pos, &hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    thislen = __laser_t_encode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    return pos;
}
 
int __laser_t_encoded_array_size(const laser_t *p, int elements)
{
    int size = 0, element;
    for (element = 0; element < elements; element++) {
 
        size += __int64_t_encoded_array_size(&(p[element].utime), 1);
 
        size += __int32_t_encoded_array_size(&(p[element].nranges), 1);
 
        size += __float_encoded_array_size(p[element].ranges, p[element].nranges);
 
        size += __int32_t_encoded_array_size(&(p[element].nintensities), 1);
 
        size += __float_encoded_array_size(p[element].intensities, p[element].nintensities);
 
        size += __float_encoded_array_size(&(p[element].rad0), 1);
 
        size += __float_encoded_array_size(&(p[element].radstep), 1);
 
    }
    return size;
}
 
int laser_t_encoded_size(const laser_t *p)
{
    return 8 + __laser_t_encoded_array_size(p, 1);
}
 
int __laser_t_decode_array(const void *buf, int offset, int maxlen, laser_t *p, int elements)
{
    int pos = 0, thislen, element;
 
    for (element = 0; element < elements; element++) {
 
        thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &(p[element].utime), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __int32_t_decode_array(buf, offset + pos, maxlen - pos, &(p[element].nranges), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        p[element].ranges = (float*) lcm_malloc(sizeof(float) * p[element].nranges);
        thislen = __float_decode_array(buf, offset + pos, maxlen - pos, p[element].ranges, p[element].nranges);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __int32_t_decode_array(buf, offset + pos, maxlen - pos, &(p[element].nintensities), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        p[element].intensities = (float*) lcm_malloc(sizeof(float) * p[element].nintensities);
        thislen = __float_decode_array(buf, offset + pos, maxlen - pos, p[element].intensities, p[element].nintensities);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_decode_array(buf, offset + pos, maxlen - pos, &(p[element].rad0), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
        thislen = __float_decode_array(buf, offset + pos, maxlen - pos, &(p[element].radstep), 1);
        if (thislen < 0) return thislen; else pos += thislen;
 
    }
    return pos;
}
 
int __laser_t_decode_array_cleanup(laser_t *p, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {
 
        __int64_t_decode_array_cleanup(&(p[element].utime), 1);
 
        __int32_t_decode_array_cleanup(&(p[element].nranges), 1);
 
        __float_decode_array_cleanup(p[element].ranges, p[element].nranges);
        if (p[element].ranges) free(p[element].ranges);
 
        __int32_t_decode_array_cleanup(&(p[element].nintensities), 1);
 
        __float_decode_array_cleanup(p[element].intensities, p[element].nintensities);
        if (p[element].intensities) free(p[element].intensities);
 
        __float_decode_array_cleanup(&(p[element].rad0), 1);
 
        __float_decode_array_cleanup(&(p[element].radstep), 1);
 
    }
    return 0;
}
 
int laser_t_decode(const void *buf, int offset, int maxlen, laser_t *p)
{
    int pos = 0, thislen;
    int64_t hash = __laser_t_get_hash();
 
    int64_t this_hash;
    thislen = __int64_t_decode_array(buf, offset + pos, maxlen - pos, &this_hash, 1);
    if (thislen < 0) return thislen; else pos += thislen;
    if (this_hash != hash) return -1;
 
    thislen = __laser_t_decode_array(buf, offset + pos, maxlen - pos, p, 1);
    if (thislen < 0) return thislen; else pos += thislen;
 
    return pos;
}
 
int laser_t_decode_cleanup(laser_t *p)
{
    return __laser_t_decode_array_cleanup(p, 1);
}
 
int __laser_t_clone_array(const laser_t *p, laser_t *q, int elements)
{
    int element;
    for (element = 0; element < elements; element++) {
 
        __int64_t_clone_array(&(p[element].utime), &(q[element].utime), 1);
 
        __int32_t_clone_array(&(p[element].nranges), &(q[element].nranges), 1);
 
        q[element].ranges = (float*) lcm_malloc(sizeof(float) * q[element].nranges);
        __float_clone_array(p[element].ranges, q[element].ranges, p[element].nranges);
 
        __int32_t_clone_array(&(p[element].nintensities), &(q[element].nintensities), 1);
 
        q[element].intensities = (float*) lcm_malloc(sizeof(float) * q[element].nintensities);
        __float_clone_array(p[element].intensities, q[element].intensities, p[element].nintensities);
 
        __float_clone_array(&(p[element].rad0), &(q[element].rad0), 1);
 
        __float_clone_array(&(p[element].radstep), &(q[element].radstep), 1);
 
    }
    return 0;
}
 
laser_t *laser_t_copy(const laser_t *p)
{
    laser_t *q = (laser_t*) malloc(sizeof(laser_t));
    __laser_t_clone_array(p, q, 1);
    return q;
}
 
void laser_t_destroy(laser_t *p)
{
    __laser_t_decode_array_cleanup(p, 1);
    free(p);
}
 
int laser_t_publish(lcm_t *lc, const char *channel, const laser_t *p)
{
      int max_data_size = laser_t_encoded_size (p);
      uint8_t *buf = (uint8_t*) malloc (max_data_size);
      if (!buf) return -1;
      int data_size = laser_t_encode (buf, 0, max_data_size, p);
      if (data_size < 0) {
          free (buf);
          return data_size;
      }
      int status = lcm_publish (lc, channel, buf, data_size);
      free (buf);
      return status;
}

struct _laser_t_subscription_t {
    laser_t_handler_t user_handler;
    void *userdata;
    lcm_subscription_t *lc_h;
};
static
void laser_t_handler_stub (const lcm_recv_buf_t *rbuf, 
                            const char *channel, void *userdata)
{
    int status;
    laser_t p;
    memset(&p, 0, sizeof(laser_t));
    status = laser_t_decode (rbuf->data, 0, rbuf->data_size, &p);
    if (status < 0) {
        fprintf (stderr, "error %d decoding laser_t!!!\n", status);
        return;
    }

    laser_t_subscription_t *h = (laser_t_subscription_t*) userdata;
    h->user_handler (rbuf, channel, &p, h->userdata);

    laser_t_decode_cleanup (&p);
}

laser_t_subscription_t* laser_t_subscribe (lcm_t *lcm, 
                    const char *channel, 
                    laser_t_handler_t f, void *userdata)
{
    laser_t_subscription_t *n = (laser_t_subscription_t*)
                       malloc(sizeof(laser_t_subscription_t));
    n->user_handler = f;
    n->userdata = userdata;
    n->lc_h = lcm_subscribe (lcm, channel, 
                                 laser_t_handler_stub, n);
    if (n->lc_h == NULL) {
        fprintf (stderr,"couldn't reg laser_t LCM handler!\n");
        free (n);
        return NULL;
    }
    return n;
}

int laser_t_subscription_set_queue_capacity (laser_t_subscription_t* subs, 
                              int num_messages)
{
    return lcm_subscription_set_queue_capacity (subs->lc_h, num_messages);
}

int laser_t_unsubscribe(lcm_t *lcm, laser_t_subscription_t* hid)
{
    int status = lcm_unsubscribe (lcm, hid->lc_h);
    if (0 != status) {
        fprintf(stderr, 
           "couldn't unsubscribe laser_t_handler %p!\n", hid);
        return -1;
    }
    free (hid);
    return 0;
}
