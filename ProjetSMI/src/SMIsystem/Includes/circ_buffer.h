/*
 * circ_buffer.h
 *
 *  Created on: Nov 14, 2020
 *      Author: phant
 */

#ifndef SMISYSTEM_INCLUDES_CIRC_BUFFER_H_
#define SMISYSTEM_INCLUDES_CIRC_BUFFER_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <assert.h>

#define BUF_LEN 20

// The hidden definition of our circular buffer structure
typedef struct {
	uint8_t * buffer;
	size_t head;
	size_t tail;
	uint8_t full;
} Circ_buffer_t;

typedef Circ_buffer_t* Circ_buffer_handle_t;

Circ_buffer_handle_t Circ_buffer_init(uint8_t* buffer);

void Circ_buffer_free(Circ_buffer_handle_t buffer);
void Circ_buffer_reset(Circ_buffer_handle_t buffer);

uint8_t Circ_buffer_full(Circ_buffer_handle_t buffer);
uint8_t Circ_buffer_empty(Circ_buffer_handle_t buffer);
size_t Circ_buffer_size(Circ_buffer_handle_t buffer);
void Circ_buffer_put(Circ_buffer_handle_t buffer, uint8_t data);
int Circ_buffer_get(Circ_buffer_handle_t buffer, uint8_t * data);

#endif /* SMISYSTEM_INCLUDES_CIRC_BUFFER_H_ */
