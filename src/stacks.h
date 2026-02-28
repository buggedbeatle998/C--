#ifndef BEATLE_INCLUDE_STACKS
#define BEATLE_INCLUDE_STACKS

#ifndef HLOCAL
    #define HLOCAL static
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


// Stack
typedef struct {
    void *stack;
    size_t ele_size;
    size_t capacity;
    size_t size;
} Stack;

HLOCAL Stack *stack_init(size_t capacity, size_t ele_size);
HLOCAL void stack_free(Stack *stack);
HLOCAL void stack_push(Stack *stack, void *value);
HLOCAL void stack_pop(Stack *stack);
HLOCAL void *stack_peek(Stack *stack);
HLOCAL void * stack_peekpop(Stack *stack);
HLOCAL void stack_clear(Stack *stack);
HLOCAL bool stack_is_empty(Stack *stack);


#ifdef IMPLEMENT_STACK
//start
    Stack *stack_init(size_t capacity, size_t ele_size) {
        Stack *stack = malloc(sizeof(Stack));
        stack->stack = malloc(ele_size * capacity);
        stack->ele_size = ele_size;
        stack->capacity = capacity;
        stack->size = 0;
    
        return stack;
    }
    
    HLOCAL void stack_free(Stack *stack) {
        free(stack->stack);
        free(stack);
    }
    
    HLOCAL void stack_push(Stack *stack, void *value) {
        assert(stack->size < stack->capacity && "Stack overflow!");
        memcpy(stack->stack + stack->size++ * stack->ele_size, value, stack->ele_size);
    }
    
    HLOCAL void stack_pop(Stack *stack) {
        assert(stack->size > 0 && "Stack underflow!");
        --stack->size;
    }
    
    HLOCAL void *stack_peek(Stack *stack) {
        assert(stack->size > 0 && "Can not peek empty stack!");
        return stack->stack + (stack->size - 1) * stack->ele_size;
    }
    
    HLOCAL void *stack_peekpop(Stack *stack) {
        assert(stack->size > 0 && "Stack underflow!");
        return stack->stack + --stack->size * stack->ele_size;
    }
    
    HLOCAL void stack_clear(Stack *stack) {
        stack->size = 0;
    }
    
    HLOCAL bool stack_is_empty(Stack *stack) {
        return stack->size == 0;
    }

// generic macro

    #define IMPLEMENT_TYPE_STACK(type, handle) \
    typedef struct { \
        Stack *stack; \
    } handle; \
    static inline handle handle##_init     (size_t capacity) {return (handle){stack_init(capacity, sizeof(type))};} \
    static inline void   handle##_free     (handle stack) {stack_free(stack.stack);} \
    static inline void   handle##_push     (handle stack, type value) {stack_push(stack.stack, &value);} \
    static inline void   handle##_pop      (handle stack) {stack_pop(stack.stack);} \
    static inline type   handle##_peek     (handle stack) {return *(type *)stack_peek(stack.stack);} \
    static inline type   handle##_peekpop  (handle stack) {return *(type *)stack_peekpop(stack.stack);} \
    static inline void   handle##_clear    (handle stack) {stack_clear(stack.stack);} \
    static inline bool   handle##_is_empty (handle stack) {return stack_is_empty(stack.stack);}
// end
#endif


#endif
