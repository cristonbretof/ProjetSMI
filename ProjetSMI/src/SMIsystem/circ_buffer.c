#include <SMIsystem/Includes/circ_buffer.h>

Circ_buffer_handle_t Circ_buffer_init(uint8_t* buffer)
{
	assert(buffer);

	Circ_buffer_handle_t Cbuffer = malloc(sizeof(Circ_buffer_t));
	assert(Cbuffer);

	Cbuffer->buffer = buffer;
	Circ_buffer_reset(Cbuffer);

	assert(Circ_buffer_empty(Cbuffer));

	return Cbuffer;
}

void Circ_buffer_free(Circ_buffer_handle_t buffer)
{
	assert(buffer);
	free(buffer);
}

void Circ_buffer_reset(Circ_buffer_handle_t buffer)
{
    assert(buffer);

    buffer->head = 0;
    buffer->tail = 0;
    buffer->full = 0;
}

uint8_t Circ_buffer_full(Circ_buffer_handle_t buffer)
{
	assert(buffer);

    return buffer->full;
}

uint8_t Circ_buffer_empty(Circ_buffer_handle_t buffer)
{
	assert(buffer);

    return (!buffer->full && (buffer->head == buffer->tail));
}

size_t Circ_buffer_size(Circ_buffer_handle_t buffer)
{
	assert(buffer);

	size_t size = BUF_LEN;

	if(!buffer->full)
	{
		if(buffer->head >= buffer->tail)
		{
			size = (buffer->head - buffer->tail);
		}
		else
		{
			size = (BUF_LEN + buffer->head - buffer->tail);
		}
	}
	return size;
}

void Circ_buffer_put(Circ_buffer_handle_t buffer, uint8_t data)
{
	assert(buffer && buffer->buffer);

	buffer->buffer[buffer->head] = data;

	if(buffer->full)
	{
		buffer->tail = (buffer->tail + 1) % BUF_LEN;
	}

	buffer->head = (buffer->head + 1) % BUF_LEN;
	buffer->full = (buffer->head == buffer->tail);
}

int Circ_buffer_get(Circ_buffer_handle_t buffer, uint8_t *data)
{
    assert(buffer && data);

    int ret = -1;

    if(!Circ_buffer_empty(buffer))
    {
        *data = buffer->buffer[buffer->tail];
        buffer->full = 0;
        buffer->tail = (buffer->tail + 1) % BUF_LEN;

        ret = 0;
    }
    return ret;
}


