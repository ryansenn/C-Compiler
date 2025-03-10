void print_c(char c){
    emit_asm("sub rsp, 8");
    emit_asm("mov [rsp], dil");
    emit_asm("mov rsi, rsp");
    emit_asm("mov rdi, 1");
    emit_asm("mov rdx, 1");
    emit_asm("mov rax, 0x2000004");
    emit_asm("syscall");
    emit_asm("add rsp, 8");
}

void print_i(int i){
    emit_asm("sub rsp, 20");
    emit_asm("mov rax, rdi");
    emit_asm("mov rcx, 10");
    emit_asm("mov byte [rsp], 0");
    emit_asm("test rax, rax");
    emit_asm("jge .not_neg");
    emit_asm("neg rax");
    emit_asm("mov byte [rsp], 1");
    emit_asm(".not_neg:");
    emit_asm("lea rdi, [rsp+19]");
    emit_asm(".convert:");
    emit_asm("xor rdx, rdx");
    emit_asm("div rcx");
    emit_asm("add dl, '0'");
    emit_asm("dec rdi");
    emit_asm("mov [rdi], dl");
    emit_asm("test rax, rax");
    emit_asm("jnz .convert");
    emit_asm("cmp byte [rsp], 0");
    emit_asm("je .done_sign");
    emit_asm("dec rdi");
    emit_asm("mov byte [rdi], '-'");
    emit_asm(".done_sign:");
    emit_asm("mov rax, 0x2000004");
    emit_asm("mov rsi, rdi");
    emit_asm("lea rdx, [rsp+19]");
    emit_asm("sub rdx, rdi");
    emit_asm("mov rdi, 1");
    emit_asm("syscall");
    emit_asm("add rsp, 20");
}