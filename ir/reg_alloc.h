//
// Created by Ryan Senoune on 2025-02-02.
//

#ifndef COMPILER_NAIVEREGALLOC_H
#define COMPILER_NAIVEREGALLOC_H

#include "ir.h"

std::unordered_map<int, int> naive_reg_alloc(std::vector<std::shared_ptr<Instruction>>& instructions) {

    std::unordered_map<int, int> reg_to_mem;
    int offset = 0;

    for (const auto& inst : instructions) {
        for (const auto& reg : inst->registers) {
            if (reg && reg_to_mem.find(reg->id) == reg_to_mem.end()) {
                reg_to_mem[reg->id] = offset;
                offset += 8;
            }
        }
    }

    if (offset > 0) {

        auto data_array = std::make_shared<Instruction>(
                "global",
                std::vector<std::shared_ptr<VirtualRegister>>{},
                std::to_string(offset)
        );
        data_array->label = "reg_alloc";

        instructions.insert(instructions.begin(), data_array);
    }

    return reg_to_mem;
}


#endif //COMPILER_NAIVEREGALLOC_H
