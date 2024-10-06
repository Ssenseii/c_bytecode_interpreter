#include <stdio.h>
#include <stdint.h>

#define STACK_MAX 256
struct
{
    uint8_t *ip;
    uint64_t stack[STACK_MAX];
    uint64_t *stack_top;
    uint64_t result;

} vm;

typedef enum
{
    OP_PUSHI, // Push immediate value onto the stack
    OP_ADD,
    OP_SUB,
    OP_DIV,
    OP_MUL,
    OP_POP_RES, // Pop the top of the stack and store it as the result
    OP_DONE,    // Done
} opcode;

typedef enum interpret_result
{
    SUCCESS,
    ERROR_DIVISION_BY_ZERO,
    ERROR_UNKNOWN_OPCODE,
} interpret_result;

void vm_reset(void)
{
    puts("Resetting the VM...");
    vm = (typeof(vm)){NULL};
    vm.stack_top = vm.stack; // point at the top of the stack
}

void vm_stack_push(uint64_t value)
{
    *vm.stack_top = value;
    vm.stack_top++; // Move the stack pointer up after pushing a value.
}

uint64_t vm_stack_pop(void)
{
    vm.stack_top--; // Move the stack pointer down to pop the top value.
    return *vm.stack_top;
}

interpret_result vm_interpret(uint8_t *bytecode)
{
    vm_reset();
    puts("Starting VM...");
    vm.ip = bytecode;

    for (;;)
    {
        uint8_t instruction = *vm.ip++;
        switch (instruction)
        {
        case OP_PUSHI:
        {
            uint8_t arg = *vm.ip++;
            vm_stack_push(arg);
            break;
        }
        case OP_ADD:
        {
            uint64_t arg_right = vm_stack_pop(); 
            uint64_t arg_left = vm_stack_pop();
            uint64_t res = arg_left + arg_right; 
            vm_stack_push(res);                  
            break;
        }
        case OP_SUB:
        {
            uint64_t arg_right = vm_stack_pop(); 
            uint64_t arg_left = vm_stack_pop();
            uint64_t res = arg_left - arg_right; 
            vm_stack_push(res);                  
            break;
        }
        case OP_MUL:
        {
            uint64_t arg_right = vm_stack_pop(); 
            uint64_t arg_left = vm_stack_pop();
            uint64_t res = arg_left * arg_right; 
            vm_stack_push(res);                  
            break;
        }
        case OP_DIV:
        {
            uint64_t arg_right = vm_stack_pop();
            if (arg_right == 0)                // Division by zero check
                return ERROR_DIVISION_BY_ZERO; // Return an error if dividing by zero
            uint64_t arg_left = vm_stack_pop();
            uint64_t res = arg_left / arg_right; // Divide the two values
            vm_stack_push(res);                  // Push the result back onto the stack
            break;
        }
        case OP_POP_RES:
        {
            uint64_t res = vm_stack_pop(); 
            vm.result = res;               
            break;
        }
        case OP_DONE:
        {
            return SUCCESS;
        }
        default:
            return ERROR_UNKNOWN_OPCODE;
        }
    }
    return SUCCESS;
}

int main(void)
{
    // Sample bytecode to run the VM:
    // Push 10, Push 20, Add, Push 5, Multiply, Pop result, Done
    uint8_t bytecode[] = {
        OP_PUSHI, 10, // Push 10 onto the stack
        OP_PUSHI, 20, // Push 20 onto the stack
        OP_ADD,       // Add top two values (10 + 20 = 30)
        OP_PUSHI, 5,  // Push 5 onto the stack
        OP_MUL,       // Multiply top two values (30 * 5 = 150)
        OP_POP_RES,   // Pop the top value (150) into the result register
        OP_DONE       // End execution
    };

    // Interpret the bytecode
    interpret_result result = vm_interpret(bytecode);

    // Check for errors
    if (result == SUCCESS)
    {
        printf("VM Execution Success. Result: %llu\n", vm.result);
    }
    else
    {
        printf("VM Execution Failed with error code: %d\n", result);
    }

    return 0;
}

/*

Initial State:          Stack: []
1. OP_PUSHI, 10    ->   Stack: [10]
2. OP_PUSHI, 20    ->   Stack: [10, 20]
3. OP_ADD          ->   Stack: [30]           (10 + 20 = 30)
4. OP_PUSHI, 5     ->   Stack: [30, 5]
5. OP_MUL          ->   Stack: [150]          (30 * 5 = 150)
6. OP_POP_RES      ->   Stack: []             (Pop 150, set result to 150)
7. OP_DONE         ->   Halt execution.

 */