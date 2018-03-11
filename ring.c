
#include <lib.h>

uint16_t ring_freespace(ring_t *r)
{
 if (r->tail == r->head)
   return RING_BUFFER_SIZE - 1;
 if (r->head > r->tail)
   return RING_BUFFER_SIZE - ((r->head - r->tail) + 1);
 else
   return (r->tail - r->head) - 1;
}

uint16_t ring_datalen(ring_t *r)
{
 return RING_BUFFER_SIZE - (ring_freespace(r) + 1);
}

uint16_t ring_read(ring_t *r, uint8_t *data, uint16_t len)
{
 uint16_t counter = 0;
 while (r->tail != r->head && counter < len) {
   data[counter++] = r->data[r->tail];
   r->tail = (r->tail + 1) % RING_BUFFER_SIZE;
 }
 return counter;
}

uchar ring_write(ring_t *r, uint8_t *data, uint16_t len)
{
 uint16_t counter = 0;
 uint16_t freeSpace = ring_freespace(r);
 if (!freeSpace)
   return RING_BUFFER_FULL;
 else if (freeSpace < len)
   return RING_BUFFER_NO_SPACE;
 while (counter < len) {
   r->data[r->head] = data[counter++];
   r->head = (r->head + 1) % RING_BUFFER_SIZE;
 }
 return RING_BUFFER_OK;
}
