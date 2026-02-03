#include <memory/alloc.hpp>

namespace nosok {
	namespace mem {

		heap_block_header_t* heap_top;

		void heap_allocator_init(void* base, uint32_t size) {
			heap_top = (heap_block_header_t*)base;

			heap_top->is_allocated = false;
			heap_top->size = size;
		}

		void* kmalloc(unsigned int size) {
			heap_block_header_t* result = 0;

			for (heap_block_header_t* chunk = heap_top; chunk != 0 && result == 0; chunk = chunk->next) {
				if (chunk->size > size && !chunk->is_allocated) result = chunk;
			}
			if (result == 0) return (void*)-1;

			if (result->size > size + sizeof(heap_block_header_t)) {
				heap_block_header_t* tmp = (heap_block_header_t*)((uint32_t)result + sizeof(heap_block_header_t) + size);

				tmp->is_allocated = false;
				tmp->prev = result;
				tmp->next = result->next;
				tmp->size = result->size - size - sizeof(heap_block_header_t);

				if (tmp->next != 0) tmp->next->prev = tmp;

				result->next = tmp;
				result->size = size;
			}

			result->is_allocated = true;

			return (void*)((uint32_t)result + sizeof(heap_block_header_t));

		}

		void kfree(void* ptr) {
			heap_block_header_t* block = (heap_block_header_t*)((uint32_t)ptr - sizeof(heap_block_header_t));

			block->is_allocated = false;

			if (block->prev != 0 && !block->prev->is_allocated) {
				block->prev->next = block->next;
				if (block->next != 0)
					block->next->prev = block->prev;

				block->prev->size += block->size + sizeof(heap_block_header_t);

				block = block->prev;
			}

			if (block->next != 0 && !block->next->is_allocated) {
				block->size += block->next->size + sizeof(heap_block_header_t);
				block->next = block->next->next;
				
				if (block->next != 0)
					block->next->prev = block;
			}
		}

	}
}

void* operator new(size_t size) {
	return nosok::mem::kmalloc(size);
}

void* operator new[](size_t size) {
	return nosok::mem::kmalloc(size);
}

void operator delete(void* ptr) {
	nosok::mem::kfree(ptr);
}

void operator delete(void* ptr, size_t) {
	nosok::mem::kfree(ptr);
}

void operator delete[](void* ptr) {
	nosok::mem::kfree(ptr);
}

void operator delete[](void* ptr, size_t) {
	nosok::mem::kfree(ptr);
}