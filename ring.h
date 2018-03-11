
#pragma once

// must be power of two
#define RING_BUFFER_SIZE	32

enum {
  RING_BUFFER_OK,
  RING_BUFFER_FULL,
  RING_BUFFER_NO_SPACE
};

typedef struct {
 uint8_t data[RING_BUFFER_SIZE];
 uint16_t head, tail;
} ring_t;

extern uint16_t ring_freespace(ring_t *r);
extern uint16_t ring_datalen(ring_t *r);
extern uint16_t ring_read(ring_t *r, uint8_t *data, uint16_t len);
extern uchar ring_write(ring_t *r, uint8_t *data, uint16_t len);
